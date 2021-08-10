/*
 * Modem.java
 *
 * Copyright (C) 2011 John Douyere (VK2ETA) - for Android platforms
 * Partially based on Modem.java from Pskmail by Per Crusefalk and Rein Couperus
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

package com.AndFlmsg;

import android.app.Activity;
import android.content.*;
import android.graphics.Bitmap;
import android.media.*;
import android.media.MediaRecorder.*;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Locale;
import java.util.regex.*;

import android.os.Process;
import android.provider.MediaStore;
import android.provider.MediaStore.Images;
import android.util.Base64;
import android.widget.Button;
import android.widget.EditText;


public class Modem {


    public static final int RXMODEMIDLE = 0;
    public static final int RXMODEMSTARTING = 1;
    public static final int RXMODEMRUNNING = 2;
    public static final int RXMODEMPAUSED = 3;
    public static final int RXMODEMSTOPPING = 4;

    public static int modemState = RXMODEMIDLE;

    public static boolean modemThreadOn = true;

    public static int NumberOfOverruns = 0;
    private static AudioRecord audiorecorder = null;
    private static boolean RxON = false;
    private static int bufferSize = 0;
    private final static float sampleRate = 8000.0f;

    //Must match the rsid.h declaration: #define RSID_FFT_SIZE		1024
    public static final int RSID_FFT_SIZE = 1024;
    //Must match the rsid.h declaration: #define RSID_SAMPLE_RATE 11025.0
    public final static double RSID_SAMPLE_RATE = 11025.0f;

    private static boolean FirstBracketReceived = false;
    public static String MonitorString = "";
    public static boolean WrapLF = false;
    private static String BlockString = "";
    private static long lastCharacterTime = 0;
    //	private static SampleRateConversion myResampler;

    public static double frequency = 1500.0;
    public static double rsidFrequency = 0;
    static double squelch = 20.0;
    static double metric = 50; //midrange

    //Waterfall interaction
    public static double[] WaterfallAmpl = new double[RSID_FFT_SIZE];
    //Semaphore for waterfall (new array of amplitudes needed for display)
    //Is now also accessed from the c++ native side in rsid.cxx
    public static boolean newAmplReady = false;

    public static boolean stopTX = false;

    //The following value MUST Match the MAXMODES in modem.h
    public static final int MAXMODES = 300;
    //List of modems and modes returned from the C++ modems
    public static int[] modemCapListInt = new int[MAXMODES];
    public static String[] modemCapListString = new String[MAXMODES];
    public static int numModes = 0;
    //Custom list of modes as slected in the preferences (can include all modes above if 
    //  "Custom List" is not selected, or alternatively all modes manually selected in preferences)
    public static int[] customModeListInt = new int[MAXMODES];
    public static String[] customModeListString = new String[MAXMODES];
    public static int customNumModes = 0;


    public static int minImageModeIndex;
    public static int maxImageModeIndex;


    //Tx variables
    public static AudioTrack txAt = null;
    public static boolean modemIsTuning = false;

    //RSID Flags
    public static boolean rxRsidOn;
    public static boolean txRsidOn;

    //JD debug
    private static boolean CModemInitialized = false;

    //Mfsk picture
    public static int picBuffer[];
    private static int pixelNumber;
    public static Bitmap picBitmap = null;
    public static int mfskPicHeight = 0;
    public static int mfskPicWidth = 0;
    public static String lastSavedPictureFn = "";
    public static int slantValue = 0;


    //Declaration of native classes
    private native static String createCModem(int modemCode);

    private native static void changeCModem(int modemCode, double newFrequency);

    private native static String initCModem(double frequency);

    private native static String rxCProcess(short[] buffer, int length);

    private native static int setSquelchLevel(double squelchLevel);

    private native static double getMetric();

    private native static double getCurrentFrequency();

    private native static int getCurrentMode();

    private native static String RsidCModemReceive(float[] myfbuffer, int length, boolean doSearch);

    private native static String createRsidModem();

    private native static int[] getModemCapListInt();

    private native static String[] getModemCapListString();

    private native static String txInit(double frequency);

    private native static boolean txCProcess(byte[] buffer, int length);

    private native static void saveEnv();

    private native static void txRSID();

    public native static int getTxProgressPercent();

    private native static void setSlowCpuFlag(boolean slowcpu);

    private native static void txPicture(byte[] txPictureBuffer, int txPictureWidth,
                                         int txPictureHeight, int txPictureTxSpeed,
                                         int txPictureColour);


    static {
        //Load the C++ modems library
        //System.loadLibrary("stlport_shared");
        System.loadLibrary("c++_shared");
        System.loadLibrary("AndFlmsg_Modem_Interface");
    }


    //Called from the C++ side to modulate the audio output
    public static void txModulate(double[] outDBuffer, int length) {
        int res = 0;
        //Catch the stopTX flag at this point as well
        if (!Modem.stopTX) {
            short[] outSBuffer = new short[length];
            int volumebits = Integer.parseInt(config.getPreferenceS("VOLUME", "8"));
            //Change format and re-scale for Android
            //To be moved to c++ code for speed
            for (int i = 0; i < length; i++) {
                outSBuffer[i] = (short) ((int) (outDBuffer[i] * 8386560.0f) >> volumebits);
            }
            res = txAt.write(outSBuffer, 0, length);
            if (res < 0)
                loggingclass.writelog("Error in writing sound buffer: " + res, null, true);
        }
    }


    //Called from the C++ side to echo the transmitted characters
    public static void putEchoChar(int txedChar) {
        Processor.monitor += (char) txedChar;
        AndFlmsg.mHandler.post(AndFlmsg.addtomodem);
    }


    //Called from the C++ native side in rsid.cxx
    public static void updateWaterfall(double[] aFFTAmpl) {
        System.arraycopy(aFFTAmpl, 0, WaterfallAmpl, 0, RSID_FFT_SIZE);
        Modem.newAmplReady = true;
        AndFlmsg.mHandler.post(AndFlmsg.updatewaterfall);
    }


    //Called from the C++ native side to initialize a new picture and associated viewer. 
    public static void showRxViewer(final int mpicW, final int mpicH) {
        //Less than 20 seconds between the end of the text message and
        //	the start of mfsk picture transmission
        if (Processor.lastMessageNoExpectedImages > 0)
            Processor.pictureRxInTime = (System.currentTimeMillis() -
                    Processor.lastMessageEndTxTime < 25000);

        //Save size for later
        mfskPicWidth = mpicW;
        mfskPicHeight = mpicH;
        //Create buffer
        picBuffer = new int[mpicH * mpicW];
        //Create bitmap
        picBitmap = Bitmap.createBitmap(picBuffer, mfskPicWidth, mfskPicHeight, Bitmap.Config.ARGB_8888);
        //Reset pixel pointer
        pixelNumber = 0;
        //Open the popup window
        ((Activity) AndFlmsg.myContext).runOnUiThread(new Runnable() {
            public void run() {
                // Display the RX picture popup window
                ((AndFlmsg) AndFlmsg.myContext).rxPicturePopup(mpicW, mpicH);
            }
        });
    }


    //Called from the C++ native side to update pixels in a picture. 
    public static void updatePicture(int data[], int picWidth) {
        if (picBuffer != null) {
            //Add a row to the bitmap
            int ii = 0;
            for (int i = 0; i < picWidth; i++) {
                ii = i * 3; //3 bytes per pixel
                picBuffer[pixelNumber++] = 0xff000000 | (data[ii] << 16) | (data[ii + 1] << 8) | data[ii + 2];
            }
            //Create bitmap
            picBitmap = Bitmap.createBitmap(picBuffer, mfskPicWidth, mfskPicHeight, Bitmap.Config.ARGB_8888);
            //update bitmap instead of re-creating at every line update (causes a lot of garbage collections and stops the processes)
            //Fix immutable state of Bitmap first (See Stack Overflow for backward compatible solutions)
            //try {
            //    picBitmap.setPixels(picBuffer, pixelNumber - picWidth, picWidth, 1, (pixelNumber - picWidth) / mfskPicHeight, picWidth, 1);
            //} catch (Exception e) {
            //    e.printStackTrace();
            //}
            //Display updated bitmap
            AndFlmsg.mHandler.post(AndFlmsg.updateMfskPicture);

        }
    }


    //Called from the C++ native side to save the last received picture. 
    public static void saveLastPicture() {

        saveAnalogPicture(true); //New picture

        //Increment picture counter if still expecting more
        if (Processor.currentImageSequenceNo < Processor.lastMessageNoExpectedImages) {

            Processor.currentImageSequenceNo++;
        }

        //Reset the time counter to the end of  the last picture Transmission
        Processor.lastMessageEndTxTime = System.currentTimeMillis();

        //Make the de-salanting buttons visible and pre-set the de-slanting amount
        slantValue = 0;
        //Enable the edit buttons
        ((Activity) AndFlmsg.myContext).runOnUiThread(new Runnable() {
            public void run() {
                Button editButton;
                editButton = (Button) AndFlmsg.pwLayout.findViewById(R.id.button_slant_left);
                if (editButton != null) editButton.setEnabled(true);
                editButton = (Button) AndFlmsg.pwLayout.findViewById(R.id.button_slant_right);
                if (editButton != null) editButton.setEnabled(true);
                editButton = (Button) AndFlmsg.pwLayout.findViewById(R.id.button_save_again);
                if (editButton != null) editButton.setEnabled(true);
            }
        });
    }


    //Save and re-save analog picture
    public static void saveAnalogPicture(boolean newPicture) {
        String fileName = "";
        String filePath = "";

        try {
            if ((Processor.lastMessageNoExpectedImages > 0) && (Processor.pictureRxInTime || !newPicture)) {
                String inboxFolderPath = Processor.HomePath + Processor.Dirprefix + Processor.DirInbox +
                        Processor.Separator;
                String tempFolderPath = Processor.HomePath + Processor.Dirprefix + Processor.DirTemp +
                        Processor.Separator;
                fileName = "rx_imgtoappend.pic";
                File imgFile = new File(tempFolderPath + fileName);
                FileOutputStream out = new FileOutputStream(imgFile);
                File fi = new File(inboxFolderPath + Processor.lastReceivedMessageFname);
                if (!fi.isFile()) {
                    AndFlmsg.middleToastText(AndFlmsg.myContext.getString(R.string.txt_FileNotFound) + ": " +
                            Processor.lastReceivedMessageFname);
                    return;
                }
                FileInputStream fileISi = new FileInputStream(fi);
                BufferedReader buf0 = new BufferedReader(new InputStreamReader(fileISi));
                String readString0 = new String();
                FileOutputStream fileOutputStrm = new FileOutputStream(tempFolderPath + Processor.lastReceivedMessageFname);
                try {
                    picBitmap.compress(Bitmap.CompressFormat.PNG, 100, out);
                    out.flush();
                    out.close();
                    //Read the compressed image file
                    long fileSize = imgFile.length();
                    //Read the full file in a byte buffer
                    byte[] rawPictureBuffer = new byte[(int) fileSize];
                    FileInputStream fileIs = new FileInputStream(imgFile);
                    fileIs.read(rawPictureBuffer);
                    //Add Field name and encode in Base64 to make it text compatible
                    int fieldIndex = Processor.currentImageSequenceNo;
                    if (!newPicture) {
                        if (fieldIndex > 0) fieldIndex--;
                    }
                    String imageField = Processor.lastMessageImgFieldName[fieldIndex] + ",";
                    String encodedImageLine = imageField + "data:image/png;base64," +
                            Base64.encodeToString(rawPictureBuffer, Base64.NO_WRAP) + "\n";
                    //Insert in received message File, replacing "_imgXYZ,<analog 1>" with "_imgXYZ,data....
                    while ((readString0 = buf0.readLine()) != null) {

//To-Do: currently if we miss an image Rx, we loose sync between image and field sequence number 
// We need a prefix before image Tx to positively identify the file and field related to the received image
//Need a sequence like for example: 
//~FLMSG:VK2ETA-20150808-111437Z-90.k2s
//~FIELD:_imgsignature1
//Sending Pic:WxH;

                        //Do we have a match with expected image field stored during message text reception
                        if (readString0.startsWith(imageField)) {
                            readString0 = encodedImageLine; //substitute placeholder with Rx image data
                        } else {
                            readString0 += "\n";
                        }
                        //Write that line
                        //Bug found by Liu Xinnan: Non-ASCII characters were not encoded properly as they were in UTF-8 (multi bytes)
                        // fileOutputStrm.write(readString0.getBytes(), 0, readString0.length());
                        fileOutputStrm.write(readString0.getBytes("UTF-8"), 0, readString0.getBytes("UTF-8").length);
                    }
                } catch (IOException e) {
                    //Processor.PostToTerminal("Error copying: " + fileName + " " + e + "\n");
                    loggingclass.writelog("Error copying: " + fileName, e, true);
                } finally {
                    try {
                        if (fileISi != null) {
                            fileISi.close();
                        }
                        if (fileOutputStrm != null) {
                            fileOutputStrm.close();
                        }
                        if (out != null) {
                            out.close();
                        }
                    } catch (IOException e) {
                        //Processor.PostToTerminal("File close error: " + e + "\n");
                        loggingclass.writelog("File close error: ", e, true);
                    }
                }
                //Delete original file in Inbox folder
                Message.deleteFile(Processor.DirInbox, Processor.lastReceivedMessageFname, false);
                //Copy new temp file in place of original
                Message.copyAnyFile(Processor.DirTemp, Processor.lastReceivedMessageFname, Processor.DirInbox, false);
                //Display a message
                AndFlmsg.topToastText(AndFlmsg.myContext.getString(R.string.txt_ImageInsertedInMessage) + ": " + Processor.lastReceivedMessageFname);
            } else { //Arrived too late, don't attach automatically
                if (newPicture && (Processor.lastMessageNoExpectedImages > 0) && !Processor.pictureRxInTime) {
                    //Display a message, but save the image as an independant picture
                    AndFlmsg.topToastText(AndFlmsg.myContext.getString(R.string.txt_ImageReceivedTooLate));
                }
            }

            //In any case, save the image file for further usage if required
            if (newPicture) {
                fileName = Message.dateTimeStamp() + ".png";
                lastSavedPictureFn = fileName;
            } else {
                fileName = lastSavedPictureFn;
            }
            filePath = Processor.HomePath + Processor.Dirprefix + Processor.DirImages +
                    Processor.Separator;
            File dest = new File(filePath + fileName);
            FileOutputStream out = new FileOutputStream(dest);
            picBitmap.compress(Bitmap.CompressFormat.PNG, 100, out);
            out.flush();
            out.close();
            //Make it available in the System Picture Gallery
            ContentValues values = new ContentValues();
            values.put(Images.Media.TITLE, fileName);
            values.put(Images.Media.DATE_TAKEN, System.currentTimeMillis());
            values.put(Images.ImageColumns.BUCKET_ID, dest.toString().toLowerCase(Locale.US).hashCode());
            values.put(Images.ImageColumns.BUCKET_DISPLAY_NAME, dest.getName().toLowerCase(Locale.US));
            values.put("_data", dest.getAbsolutePath());
            if (Processor.lastMessageNoExpectedImages > 0) {
                values.put(Images.Media.DESCRIPTION, "Flmsg Image");
            } else {
                values.put(Images.Media.DESCRIPTION, "Fldigi Image");
                //Display a message
                AndFlmsg.topToastText(AndFlmsg.myContext.getString(R.string.txt_SavedImage) + ": " + fileName +
                        "\n\n" + AndFlmsg.myContext.getString(R.string.txt_UseGalleryToView));
            }
            ContentResolver cr = AndFlmsg.myContext.getContentResolver();
            cr.insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);
        } catch (Exception e) {
            loggingclass.writelog("Exception Error in 'saveLastPicture' " + e.getMessage(), null, true);
        }
    }


    //Corrects the slant in a received image. Applies the cummulative step parameter.
    public static void deSlant(int step) {

        slantValue += step;
        if (slantValue > mfskPicWidth) slantValue = mfskPicWidth;
        if (slantValue < -1 * mfskPicWidth) slantValue = -1 * mfskPicWidth;
        int pixelNumberAtLineY, sourcePixelNumber, sourcePixel, sourceNextPixel;
        int nextPixelStep = slantValue < 0 ? -1 : 1;
        int pixelShift;
        double thisPixel, nextPixel;
        double slantLineInc = (double) slantValue / (double) mfskPicHeight;
        int[] deSlantedPic = new int[picBuffer.length];
        //copy first line as-is (it is the time/spacial reference)
        for (int x = 0; x < mfskPicWidth; x++) {
            deSlantedPic[x] = picBuffer[x];
        }
        //Perform "de-slanting" from the 2n line onwards
        for (int y = 1; y < mfskPicHeight; y++) {
            pixelNumberAtLineY = mfskPicWidth * y;
            pixelShift = (int) (y * slantLineInc);
            nextPixel = Math.abs((y * slantLineInc) - (double) pixelShift);
            thisPixel = 1 - nextPixel;

            //Shift pixels using fractional value
            for (int x = 0; x < mfskPicWidth; x++) {
                if (x + pixelShift >= 0 && x + pixelShift < mfskPicWidth) {
                    sourcePixelNumber = pixelShift + pixelNumberAtLineY + x > picBuffer.length - 2 ?
                            picBuffer.length - 2 : pixelShift + pixelNumberAtLineY + x;
                    sourcePixel = picBuffer[sourcePixelNumber];
                    sourceNextPixel = picBuffer[sourcePixelNumber + nextPixelStep];
                    double red1 = (((sourcePixel & 0x00ff0000) >> 16) * thisPixel);
                    double red2 = (((sourceNextPixel & 0x00ff0000) >> 16) * nextPixel);
                    int red = (int) (red1 + red2);
                    double green1 = (((sourcePixel & 0x0000ff00) >> 8) * thisPixel);
                    double green2 = (((sourceNextPixel & 0x0000ff00) >> 8) * nextPixel);
                    int green = (int) (green1 + green2);
                    double blue1 = ((sourcePixel & 0x000000ff) * thisPixel);
                    double blue2 = ((sourceNextPixel & 0x000000ff) * nextPixel);
                    int blue = (int) (blue1 + blue2);
                    deSlantedPic[pixelNumberAtLineY + x] = 0xff000000 | (red << 16) | (green << 8) | blue;
                } else {
                    //The line is wrapped around, so need to use the previous/next line pixels
                    sourcePixelNumber = pixelShift + pixelNumberAtLineY + x - (mfskPicWidth * nextPixelStep) > picBuffer.length - 2 ?
                            picBuffer.length - 2 : pixelShift + pixelNumberAtLineY + x - (mfskPicWidth * nextPixelStep);
                    sourcePixel = picBuffer[sourcePixelNumber];
                    sourceNextPixel = picBuffer[sourcePixelNumber + nextPixelStep];
                    double red1 = (((sourcePixel & 0x00ff0000) >> 16) * thisPixel);
                    double red2 = (((sourceNextPixel & 0x00ff0000) >> 16) * nextPixel);
                    int red = (int) (red1 + red2);
                    double green1 = (((sourcePixel & 0x0000ff00) >> 8) * thisPixel);
                    double green2 = (((sourceNextPixel & 0x0000ff00) >> 8) * nextPixel);
                    int green = (int) (green1 + green2);
                    double blue1 = ((sourcePixel & 0x000000ff) * thisPixel);
                    double blue2 = ((sourceNextPixel & 0x000000ff) * nextPixel);
                    int blue = (int) (blue1 + blue2);
                    //Colors have been shifted as we transmit one line of Red, one of Green, then one of Blue
                    if (nextPixelStep > 0) { //Slant towards left
                        deSlantedPic[pixelNumberAtLineY + x] = 0xff000000 | red | (green << 16) | (blue << 8);
                    } else { //Slant towards right
                        deSlantedPic[pixelNumberAtLineY + x] = 0xff000000 | (red << 8) | green | (blue << 16);
                    }
                }
            }
        }
        picBitmap = Bitmap.createBitmap(deSlantedPic, mfskPicWidth, mfskPicHeight, Bitmap.Config.ARGB_8888);
        deSlantedPic = null; //Release for GC
        AndFlmsg.mHandler.post(AndFlmsg.updateMfskPicture);

    }


    //Get capability list of modems from all the C++ modems (taken from rsid_defs.cxx)
    public static void updateModemCapabilityList() {
        //get modem list (int and string description). The C++ side returns its list of available modems.
        modemCapListInt = getModemCapListInt();
        modemCapListString = getModemCapListString();
        //Now find the end of modem list to know how many different modems codes we have
        Modem.numModes = MAXMODES; //Just in case
        for (int i = 0; i < MAXMODES; i++) {
            if (modemCapListInt[i] == -1) {
                Modem.numModes = i;
                //Exit loop
                i = MAXMODES;
            }
        }
        //Sort by mode code to re-group modes of the same modem (as they are in two arrays in rsid_def.cxx)
        boolean swapped = true;
        int tmp;
        String tmpS;
        while (swapped) {
            swapped = false;
            for (int i = 0; i < numModes - 1; i++) {
                if (modemCapListInt[i] > modemCapListInt[i + 1]) {
                    tmp = modemCapListInt[i];
                    tmpS = modemCapListString[i];
                    modemCapListInt[i] = modemCapListInt[i + 1];
                    modemCapListString[i] = modemCapListString[i + 1];
                    modemCapListInt[i + 1] = tmp;
                    modemCapListString[i + 1] = tmpS;
                    swapped = true;
                }
            }
        }
        //Do we want to use a custom list of modes?
        boolean useModeList = config.getPreferenceB("USEMODELIST", false);
        if (useModeList) {
            customNumModes = 0;
            boolean thatmode;
            for (int i = 0; i < numModes; i++) {
                thatmode = config.getPreferenceB("USE" + modemCapListString[i], false);
                if (thatmode) {
                    customModeListInt[customNumModes] = modemCapListInt[i];
                    customModeListString[customNumModes++] = modemCapListString[i];
                }
            }
        } else {
            customModeListInt = modemCapListInt;
            customModeListString = modemCapListString;
            customNumModes = numModes;
        }
        //Check that we have at least one mode (if not, take the first in the capability list)
        if (customNumModes < 1) {
            customNumModes = 1;
            customModeListInt[0] = modemCapListInt[0];
            customModeListString[0] = modemCapListString[0];
        }
    }


    //Initialise RX modem
    public static void ModemInit() {
        //(re)get list of available modems
        updateModemCapabilityList();

        //Android To-DO: Change to C++ resampler instead of Java quadratic resampler
        //Initialize Re-sampling to 11025Hz for RSID, THOR and MFSK modems
        //		myResampler = new SampleRateConversion(11025.0 / 8000.0);
        SampleRateConversion.SampleRateConversionInit((float) (11025.0 / 8000.0));
    }


    //Returns the modem code given a modem name (String)
    public static int getMode(String mstring) {
        int j = 0;
        for (int i = 0; i < modemCapListInt.length; i++) {
            if (modemCapListString[i].equals(mstring)) j = i;
        }
        return modemCapListInt[j];
    }

    //Returns the modem index in the array of modes available given a modem code
    public static int getModeIndex(int mcode) {
        int j = -1;
        for (int i = 0; i < customNumModes; i++) {
            if (mcode == customModeListInt[i]) j = i;
        }
        //In case we didn't find it, return the first mode in the list to avoid segment fault
        if (j == -1) {
            j = 0;
        }
        return j;
    }


    //Receives a modem code. Returns the modem index in the array of ALL modes supplied by the C++ modem
    public static int getModeIndexFullList(int mcode) {
        int j = -1;
        for (int i = 0; i < numModes; i++) {
            if (mcode == modemCapListInt[i]) j = i;
        }
        //In case we didn't find it, return the first mode in the list to avoid segment fault
        if (j == -1) {
            j = 0;
        }
        return j;
    }


    //Return the new mode code or the same if it hits the end of list either way
    public static int getModeUpDown(int currentMode, int increment) {
        //Find position of current mode
        int j = getModeIndex(currentMode);
        j += increment;
        //Circular list
        if (j < 0) j = customNumModes - 1;
        if (j >= customNumModes) j = 0;
        return customModeListInt[j];
    }


    //Return true if the mode can send a picture
    public static boolean canSendPicture(int thisMode) {
        //To check is the modem can TX pictures, use the mode name
        int j = getModeIndexFullList(thisMode);
        String modemName = modemCapListString[j];
        if (modemName.equalsIgnoreCase("MFSK16") ||
                modemName.equalsIgnoreCase("MFSK32") ||
                modemName.equalsIgnoreCase("MFSK64") ||
                modemName.equalsIgnoreCase("MFSK128")) {
            return true;
        } else
            return false;
    }


    private static void soundInInit() {
        bufferSize = (int) sampleRate; // 1 second of Audio max
        if (bufferSize < AudioRecord.getMinBufferSize((int) sampleRate, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT)) {
            // Check to make sure buffer size is not smaller than the smallest allowed one
            bufferSize = AudioRecord.getMinBufferSize((int) sampleRate, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT);
        }
        int ii = 20; //number of 1/4 seconds wait
        while (--ii > 0) {
            if (AndFlmsg.toBluetooth) {
                //Bluetooth hack (use voice call)
                audiorecorder = new AudioRecord(AudioSource.MIC, 8000, android.media.AudioFormat.CHANNEL_IN_MONO,
                        android.media.AudioFormat.ENCODING_PCM_16BIT, bufferSize);
            } else {
                audiorecorder = new AudioRecord(AudioSource.MIC, 8000, android.media.AudioFormat.CHANNEL_IN_MONO,
                        android.media.AudioFormat.ENCODING_PCM_16BIT, bufferSize);
            }
            if (audiorecorder.getState() == AudioRecord.STATE_INITIALIZED) {
                ii = 0;//ok done
            } else {
                if (ii < 16) { //Only if have to wait more than 1 seconds
                    loggingclass.writelog("Waiting for Audio MIC availability...", null, true);
                }
                try {
                    Thread.sleep(250);//1/4 second
                } catch (InterruptedException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        }
        if (audiorecorder.getState() != AudioRecord.STATE_INITIALIZED) {
            //Android add exception catch here
            loggingclass.writelog("Can't open Audio MIC \n", null, true);
        }
    }


    public static void startmodem() {
        modemThreadOn = true;

        new Thread(new Runnable() {
            public void run() {
                //Brings back thread priority to foreground (balance between GUI and modem)
                Process.setThreadPriority(Process.THREAD_PRIORITY_FOREGROUND);

                while (modemThreadOn) {

                    //Save Environment for this thread so it can call back
                    // Java methods while in C++
                    Modem.saveEnv();

                    String rsidReturnedString;
                    String modemReturnedString;
                    modemState = RXMODEMSTARTING;
                    double startproctime = 0;
                    double endproctime = 0;
                    int numSamples8K = 0;
                    Modem.soundInInit();
                    //debugging only
                    //Message.addEntryToLog(Message.dateTimeStamp() + "Done 'soundInInit'");
                    NumberOfOverruns = 0;
                    try {
                        //Catch un-initialized audio recorder
                        audiorecorder.startRecording();
                    } catch (IllegalStateException e) {
                        //e.printStackTrace();
                    }
                    RxON = true;
                    Processor.restartRxModem.drainPermits();
                    //Since the callback is not working, implement a while loop.
                    short[] so8K = new short[bufferSize];
                    int size12Kbuf = (int) ((bufferSize + 1) * 11025.0 / 8000.0);
                    //Android changed to float to match rsid.cxx code
                    //double[] so12K = new double[size12Kbuf];
                    float[] so12K = new float[size12Kbuf];
                    //Initialise modem
                    //debugging only
                    //Message.addEntryToLog(Message.dateTimeStamp() + "About to do 'createCModem' with Modem" + Processor.RxModem);
                    String modemCreateResult = createCModem(Processor.RxModem);
                    //debugging only
                    //Message.addEntryToLog(Message.dateTimeStamp() + "Done 'createCModem'");
                    if (modemCreateResult.contains("ERROR")) {
                        CModemInitialized = false;
                    } else {
                        CModemInitialized = true;
                        //Initialize RX side of modem
                        boolean slowCpu = config.getPreferenceB("SLOWCPU", false);
                        setSlowCpuFlag(slowCpu);
                        //Changed to getPreferencesI in case it is not an interger representation
                        //String frequencySTR = config.getPreferenceS("AFREQUENCY","1000");
                        //double centerfreq = Integer.parseInt(frequencySTR);
                        double centerfreq = config.getPreferenceI("AFREQUENCY", 1000);
                        //Limit it's values too
                        if (centerfreq > 2500) centerfreq = 2500;
                        if (centerfreq < 500) centerfreq = 500;
                        String modemInitResult = initCModem(centerfreq);
                        //debugging only
                        //Message.addEntryToLog(Message.dateTimeStamp() + "Done 'initCModem'");
                        //Android Debug
                        //   Modem.MonitorString = modemInitResult;
                        //Prepare RSID Modem
                        createRsidModem();
                        //debugging only
                        //Message.addEntryToLog(Message.dateTimeStamp() + "Done 'createRsidModem'");
                    }
                    //debugging only
                    //Message.addEntryToLog(Message.dateTimeStamp() + "Starting Rx loop");
                    while (RxON) {
                        endproctime = System.currentTimeMillis();
                        double buffertime = (double) numSamples8K / 8000.0 * 1000.0; //in milliseconds
                        if (numSamples8K > 0)
                            Processor.cpuload = (int) (((double) (endproctime - startproctime)) / buffertime * 100);
                        if (Processor.cpuload > 100) Processor.cpuload = 100;
                        AndFlmsg.mHandler.post(AndFlmsg.updatecpuload);
                        //Android try faster mode changes by having a smaller buffer to process						numSamples8K = audiorecorder.read(so8K, 0, 8000/4); //process only part of the buffer to avoid lumpy processing
                        numSamples8K = audiorecorder.read(so8K, 0, 8000 / 8); //process only part of the buffer to avoid lumpy processing
                        if (numSamples8K > 0) {
                            modemState = RXMODEMRUNNING;
                            startproctime = System.currentTimeMillis();
                            //Process only if Rx is ON, otherwise discard (we have already decided to TX)
                            if (RxON) {
                                if (rxRsidOn || (AndFlmsg.currentview == AndFlmsg.MODEMVIEWwithWF)) {
                                    //Re-sample to 11025Hz for RSID, THOR and MFSK modems
                                    int numSamples12K = SampleRateConversion.Process(so8K, numSamples8K, so12K, size12Kbuf);
                                    //Conditional Rx RSID (keep the FFT processing for the waterfall)
                                    rsidReturnedString = RsidCModemReceive(so12K, numSamples12K, rxRsidOn);
                                    //As we flushed the RX pipe automatically, we need to process
                                    // any left-over characters from the previous modem (critical
                                    // for MT63 and Olivia in particular)
                                    //Done in processRxChar() below now
                                    //Modem.MonitorString += rsidReturnedString;
                                    //Processing of the characters received
                                    for (int i = 0; i < rsidReturnedString.length(); i++) {
                                        processRxChar(rsidReturnedString.charAt(i));
                                    }
                                } else {
                                    rsidReturnedString = "";
                                }
                                if (rxRsidOn) {
                                    //if (rsidReturnedString.startsWith("\nRSID:")) {
                                    if (rsidReturnedString.contains("\nRSID:")) {
                                        //We have a new modem and/or centre frequency
                                        //Modem.MonitorString += rsidReturnedString; //Done elsewhere now
                                        //Update the RSID waterfall frequency too
                                        frequency = getCurrentFrequency();
                                        Processor.RxModem = getCurrentMode();
                                        AndFlmsg.saveLastModeUsed(Processor.RxModem);
                                        AndFlmsg.mHandler.post(AndFlmsg.updatetitle);
                                    }
                                }
                                //Sets the latest squelch level for the modem to use
                                setSquelchLevel(squelch);
                                //Then process the RX data
                                modemReturnedString = rxCProcess(so8K, numSamples8K);
                                //Retreive latest signal quality for display
                                //Remove averaging metric = misc.decayavg(metric, getMetric(), 20);
                                metric = getMetric();
                                //metric = getMetric();
                                AndFlmsg.mHandler.post(AndFlmsg.updatesignalquality);
                                //Now process all the characters received
                                for (int i = 0; i < modemReturnedString.length(); i++) {
                                    processRxChar(modemReturnedString.charAt(i));
                                }
                            }
                            //Post to monitor (Modem) window after each buffer processing
                            //Add TX frame too if present
                            if (Modem.MonitorString.length() > 0 || Processor.TXmonitor.length() > 0) {
                                Processor.monitor += Modem.MonitorString + Processor.TXmonitor;
                                Processor.TXmonitor = "";
                                Modem.MonitorString = "";
                                AndFlmsg.mHandler.post(AndFlmsg.addtomodem);
                            }
                        }
                    }//while (RxON)
                    //We dropped here on pause flag
                    if (audiorecorder != null) {
                        //Avoid some crashes on wrong state
                        if (audiorecorder.getState() == AudioRecord.STATE_INITIALIZED) {
                            if (audiorecorder.getRecordingState() == AudioRecord.RECORDSTATE_RECORDING) {
                                audiorecorder.stop();
                            }
                            audiorecorder.release();
                        }
                    }
                    modemState = RXMODEMPAUSED;
                    //Marker for end of thread (Stop modem thread flag)
                    if (!modemThreadOn) {
                        modemState = RXMODEMIDLE;
                        return;
                    }
                    //Now waits for a restart (or having this thread killed)
                    Processor.restartRxModem.acquireUninterruptibly(1);
                    //Make sure we don's have spare permits
                    Processor.restartRxModem.drainPermits();
                }//while (modemThreadOn)
                //We dropped here on thread stop request
                modemState = RXMODEMIDLE;
            } //run
        }).start(); //	new Thread(new Runnable() {

    }


    public static void stopRxModem() {
        modemThreadOn = false;
        RxON = false;
    }

    public static void pauseRxModem() {
        RxON = false;
    }

    public static void unPauseRxModem() {
        Processor.restartRxModem.release(1);
    }

    static void changemode(int newMode) {
        //Stop the modem receiving side to prevent using the wrong values
        pauseRxModem();
        //Restart modem reception
        unPauseRxModem();

    }

    static void setFrequency(double rxfreq) {
        frequency = rxfreq;
    }

    static void reset() {
        String frequencySTR = config.getPreferenceS("AFREQUENCY", "1000");
        frequency = Integer.parseInt(frequencySTR);
        if (frequency < 500) frequency = 500;
        if (frequency > 2500) frequency = 2500;
        squelch = AndFlmsg.mysp.getFloat("SQUELCHVALUE", (float) 20.0);
    }


    /**
     * @param squelchdiff the delta to add to squelch
     */
    public static void AddtoSquelch(double squelchdiff) {
        squelch += (squelch > 10) ? squelchdiff : squelchdiff / 2;
        if (squelch < 0) squelch = 0;
        if (squelch > 100) squelch = 100;
        //store value into preferences
        SharedPreferences.Editor editor = AndFlmsg.mysp.edit();
        editor.putFloat("SQUELCHVALUE", (float) squelch);
        // Commit the edits!
        editor.commit();
    }

    public static void processRxChar(char inChar) {

        //Save the time of the last character received
        lastCharacterTime = System.currentTimeMillis();

        //For UTF-8 let all characters through
        //if (inChar > 127) {
        // todo: unicode encoding
        //    inChar = 0;
        // }

        switch (inChar) {
            case 0:
                break; // do nothing
            case '[':
                if (Processor.ReceivingForm ||
                        FirstBracketReceived) {
                    BlockString += inChar;
                } else {
                    if (!FirstBracketReceived) {
                        FirstBracketReceived = true;
                        BlockString = "[";
                        //Processor.PostToTerminal("FirstBracket\n");
                    }
                }
                //	            Main.DCD = 0;
                break;
            case ']':
                if (Processor.ReceivingForm) {
                    BlockString += inChar;
                    //Did we receive a new start of block before the end
                    //of the previous block? If so, restart reception
                    if (BlockString.endsWith("[WRAP:beg]")) {
                        BlockString = "[WRAP:beg]";
                        Processor.PostToTerminal(AndFlmsg.myContext.getString(R.string.txt_ReceivingNewMessageDiscardingOld));
                    } else if (BlockString.endsWith("[WRAP:lf]")) {
                        WrapLF = true;
                    } else if (BlockString.endsWith("[WRAP:crlf]")) {
                        WrapLF = false;
                    } else if (BlockString.endsWith("[WRAP:end]")) {
                        //We have a complete block, process it
                        //Processor.PostToTerminal("End of Message. Processing...\n");
                        Processor.processWrapBlock(BlockString);
                        Processor.CrcString = "";
                        Processor.FileNameString = "";
                        Processor.ReceivingForm = false;
                        FirstBracketReceived = false;
                    } else {
                        //Check for CRC section
                        Pattern psc = Pattern.compile("WRAP:chksum ([0-9A-F]{4})");
                        Matcher msc = psc.matcher(BlockString);
                        if (msc.find()) {
                            Processor.CrcString = msc.group(1);
                            //Processor.PostToTerminal("Checksum <" + Processor.CrcString + ">\n");
                        }
                        //Check for file name section
                        psc = Pattern.compile("\\[WRAP:fn (.+\\..+)\\]");
                        msc = psc.matcher(BlockString);
                        if (msc.find()) {
                            if (BlockString.endsWith(msc.group(0))) {
                                Processor.FileNameString = msc.group(1);
                                Processor.PostToTerminal("\n" + AndFlmsg.myContext.getString(R.string.txt_ReceivingFile) + ": " + Processor.FileNameString);
                                //Processor.PostToTerminal("\n" + msc.group(0) + "\n");
                            }
                        }
                    }
                } else {
                    BlockString += inChar;
                    if (BlockString.endsWith("[WRAP:beg]")) {
                        FirstBracketReceived = true;
                        BlockString = "[WRAP:beg]";
                        Processor.ReceivingForm = true;
                        Processor.PostToTerminal(AndFlmsg.myContext.getString(R.string.txt_ReceivingNewMessage));
                    }

                }
                break;
            case 10: //Line Feed
                MonitorString += "\n";
                if (Processor.ReceivingForm ||
                        FirstBracketReceived) {
                    BlockString += inChar;
                }
                break;
            case 13: //ignore Carriage Returns
                break;
            default:
                if (Processor.ReceivingForm ||
                        FirstBracketReceived) {
                    BlockString += inChar;
                }
                break;
        }    // end switch
        //Reset if header not found within
        //  1000 charaters of first bracket
        if (FirstBracketReceived &&
                !Processor.ReceivingForm &&
                BlockString.length() > 1000) {
            BlockString = BlockString.substring(900);
            if (!BlockString.contains("]")) FirstBracketReceived = false;
        }
        //Reset if end marker not found within
        // 100,000 charaters of start (increased from 30,000 as new 8PSK modes allows mode data within a given period of time).
        //to-do: if found crc but not wrap end, process anyway
        if (Processor.ReceivingForm &&
                BlockString.length() > 100000) {
            Processor.CrcString = "";
            Processor.FileNameString = "";
            Processor.ReceivingForm = false;
            FirstBracketReceived = false;
        }
        if (inChar > 31) //Display non-control characters
        {
            MonitorString += inChar;
        }
    }


    //Called every second to check is we have a message reception timeout (This parameter 
    //   is called "Extract Timeout" in Fldigi/NBEMS configuration)
    public static void checkExtractTimeout() {
        int timeout = config.getPreferenceI("EXTRACTTIMEOUT", 4);
        if (Processor.ReceivingForm) {
            if (lastCharacterTime + (timeout * 1000) < System.currentTimeMillis()) {
                Processor.CrcString = "";
                Processor.FileNameString = "";
                Processor.ReceivingForm = false;
                FirstBracketReceived = false;
                Processor.PostToTerminal(AndFlmsg.myContext.getString(R.string.txt_MsgRxTimeout));
            }
        }
    }


    //In a separate thread so that the UI thread is not blocked during TX
    public static void txData(String sendingFolder, String sendingFileName, String Sendline,
                              int numberOfImagesToTx,
                              int pictureTxSpeed, Boolean pictureColour, String pictureTxMode) {
        Runnable TxRun = new TxThread(sendingFolder, sendingFileName, Sendline,
                numberOfImagesToTx,
                pictureTxSpeed, pictureColour, pictureTxMode);
        Sendline = "";
        new Thread(TxRun).start();
        pauseRxModem();
    }


    public static class TxThread implements Runnable {
        private String txSendline = "";
        private String txFolder = "";
        private String txFileName = "";
        private int txNumberOfImagesToTx = 0;
        private int txPictureTxSpeed;
        private int txPictureColour;
        private String txPictureTxMode;

        public TxThread(String sendingFolder, String sendingFileName, String Sendline,
                        int numberOfImagesToTx,
                        int pictureTxSpeed, Boolean pictureColour, String pictureTxMode) {
            txFolder = sendingFolder;
            txFileName = sendingFileName;
            txSendline = Sendline;
            txNumberOfImagesToTx = numberOfImagesToTx;
            txPictureTxSpeed = pictureTxSpeed;
            txPictureColour = pictureColour ? -1 : 0; //Leave option open for further variations
            txPictureTxMode = pictureTxMode;
        }

        public void run() {

            if (txSendline.length() > 0 & !Processor.TXActive) {

                if (Processor.DCDthrow == 0) {
                    try {
                        //Reset the stop flag if it was ON
                        Modem.stopTX = false;
                        //Set flags to TXing
                        Processor.TXActive = true;
                        Processor.Status = AndFlmsg.myContext.getString(R.string.txt_Transmitting);
                        AndFlmsg.mHandler.post(AndFlmsg.updatetitle);
                        //Stop the modem receiving side
                        pauseRxModem();

                        //Wait for the receiving side to be fully stopped???
                        //To-Do: review logic here: while (modemState != RXMODEMPAUSED) {
                        Thread.sleep(500);
                        //						}

                        //Open and initialise the sound system
                        int intSize = 4 * android.media.AudioTrack.getMinBufferSize(8000,
                                AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_16BIT); //Android check the multiplier value for the buffer size
                        if (AndFlmsg.toBluetooth) {
                            txAt = new AudioTrack(AudioManager.STREAM_VOICE_CALL, 8000, AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_16BIT, intSize, AudioTrack.MODE_STREAM);
                        } else {
                            txAt = new AudioTrack(AudioManager.STREAM_MUSIC, 8000, AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_16BIT, intSize, AudioTrack.MODE_STREAM);
                        }
                        //Launch TX
                        txAt.setStereoVolume(1.0f, 1.0f);
                        txAt.play();
                        //Initalise the C++ modem TX side
                        String frequencySTR = config.getPreferenceS("AFREQUENCY", "1500");
                        frequency = Integer.parseInt(frequencySTR);
                        if (frequency < 500) frequency = 500;
                        if (frequency > 2500) frequency = 2500;
                        //Save Environment for this thread so it can call back
                        // Java methods while in C++
                        Modem.saveEnv();
                        //Init current modem for Tx
                        Modem.txInit(frequency);
                        if (txRsidOn) {
                            Modem.txRSID();
                        }
                        //Encode character buffer into sound
                        //Changed for Utf-8 variable length codes
                        //Modem.txCProcess(txSendline.getBytes(), txSendline.length());
                        byte[] bytesToSend = null;
                        try {
                            bytesToSend = txSendline.getBytes("UTF-8");
                        } catch (Exception e) { //Invalid UTF-8 characters
                            bytesToSend[0] = 0;//Null character
                        }
                        Modem.txCProcess(bytesToSend, bytesToSend.length);
                        //Save current mode in case we change it for images Tx
                        //int currentMode = Processor.RxModem;
                        //Is there a picture/signature to send after the text?
                        if (txNumberOfImagesToTx > 0) {
                            //Change to the selected MFSK mode
                            int modemCode = Modem.getMode("MFSK32");
                            if (!txPictureTxMode.equals("")) {
                                modemCode = Modem.getMode(txPictureTxMode);
                            }
                            //Change to MFSK Image modem
//			    Modem.createCModem(modemCode);
//			    Modem.initCModem(frequency);
                            //changeCModem(modemCode, frequency);
                            TxMFSKPicture picModem = new TxMFSKPicture(modemCode);
                            //PictureTxRSID picRSIDTx = new PictureTxRSID();
                            //debugging only
                            //Message.addEntryToLog(Message.dateTimeStamp() + "Done 'changeCModem' with modem # " + modemCode);
                            for (int i = 0; i < txNumberOfImagesToTx; i++) {
                                if (Message.attachedPictureArray[i] != null &&
                                        Message.attachedPictureWidth[i] > 0 &&
                                        Message.attachedPictureHeight[i] > 0) {
                                    //Modem.txInit(frequency);
                                    //debugging only
                                    //Message.addEntryToLog(Message.dateTimeStamp() + "Done 'txInit' at frequency: " + frequency);
                                    //Always send RSID
                                    //Bug found by Stan KG5CKK. Thanks Stan.
                                    // String modemName = Modem.modemCapListString[getModeIndex(modemCode)];
                                    //String modemName = Modem.customModeListString[getModeIndex(modemCode)];
                                    //Bug found by Liu Xinnan (BH5HDE), thank you:
                                    // RSID not sent when image mfsk mode not selected in list of custom modes
                                    String modemName = Modem.modemCapListString[getModeIndexFullList(modemCode)];
                                    PictureTxRSID.send(modemName);
                                    //debugging only
                                    //Message.addEntryToLog(Message.dateTimeStamp() + "Done 'txRSID' ");
                                    //Send picture
                                    picModem.txImageProcess("", Message.attachedPictureArray[i],
                                            Message.attachedPictureWidth[i], Message.attachedPictureHeight[i],
                                            txPictureTxSpeed, txPictureColour);
                                    //Modem.txPicture(Message.attachedPictureArray[i], Message.attachedPictureWidth[i],
                                    //	    Message.attachedPictureHeight[i], txPictureTxSpeed, txPictureColour);
                                    //debugging only
                                    //Message.addEntryToLog(Message.dateTimeStamp() + "Done 'txPicture' ");
                                    //Release the (large) Byte array for GC
                                    Message.attachedPictureArray[i] = null;
                                    //debugging only
                                    //Message.addEntryToLog(Message.dateTimeStamp() + "Done 'release array' ");
                                }
                            }
                            //Change back to the previous mode (for post-Tx-RSID purposes)
                            //Message.addEntryToLog(Message.dateTimeStamp() + "About to execute 'changeCModem' with modem # " + currentMode);
                            //Modem.createCModem(currentMode);
                            //Modem.initCModem(frequency);
                            //changeCModem(currentMode, frequency);
                            //debugging only
                            //Message.addEntryToLog(Message.dateTimeStamp() + "Done 'changeCModem' with modem # " + currentMode);
                        }
                        //Send TX RSID if required
                        //Check and send post-transmission RSID
                        if (txRsidOn && config.getPreferenceB("TXPOSTRSID", false)) {
                            Modem.txRSID();
                            //debugging only
                            //Message.addEntryToLog(Message.dateTimeStamp() + "Done post 'txRSID'");
                        }
                        //Stop audio track
                        txAt.stop();
                        //debugging only
                        //Message.addEntryToLog(Message.dateTimeStamp() + "Done 'txAt.stop'");
                        //Wait for end of audio play to avoid
                        //overlaps between end of TX and start of RX
                        while (txAt.getPlayState() == AudioTrack.PLAYSTATE_PLAYING) {
                            try {
                                Thread.sleep(50);
                            } catch (InterruptedException e) {
                                // TODO Auto-generated catch block
                                //e.printStackTrace();
                            }
                        }
                        //debugging only
                        //Message.addEntryToLog(Message.dateTimeStamp() + "Done 'waiting for end of playing state'");
                        //Android debug add a fixed delay to avoid cutting off the tail end of the modulation
                        Thread.sleep(500);
                        txAt.release();
                        //debugging only
                        //Message.addEntryToLog(Message.dateTimeStamp() + "Done 'txAt.release'");

                        //if the TX was complete, move message from Outbox to Sent folder
                        //Detect if it was a simple text sent from the terminal window (no folder and filename)
                        if (!Modem.stopTX && (txFolder.length() > 0) && (txFileName.length() > 0)) {
                            Message.copyAnyFile(txFolder, txFileName, Processor.DirSent, false);
                            //debugging only
                            //Message.addEntryToLog(Message.dateTimeStamp() + "Done 'copyAnyFile'");
                            Message.deleteFile(Processor.DirOutbox, txFileName, false);//Don't advise deletion
                            //debugging only
                            //Message.addEntryToLog(Message.dateTimeStamp() + "Done 'deleteFile'");
                            //Ensure we are using the right env variable for this TX thread
                            Message.addEntryToLog(Message.dateTimeStamp() + " - " + AndFlmsg.myContext.getString(R.string.txt_SentMessagefile)
                                    + ": " + txFileName);
                            Thread displayMessagesThread = new Thread(AndFlmsg.displayMessagesRunnable);
                            displayMessagesThread.start();
                            //debugging only
                            //Message.addEntryToLog(Message.dateTimeStamp() + "Done 'displayMessagesThread.start'");
                        }

                    } catch (Exception e) {
                        loggingclass.writelog("Can't output sound. Is Sound device busy?", null, true);
                    } finally {
                        Processor.TXActive = false;
                        Processor.Status = AndFlmsg.myContext.getString(R.string.txt_Listening);
                        //VK2ETA added to clear progress info during a tune
                        AndFlmsg.txProgressCount = "";
                        AndFlmsg.mHandler.post(AndFlmsg.updatetitle);
                        //Restart modem reception
                        unPauseRxModem();
                        //debugging only
                        //Message.addEntryToLog(Message.dateTimeStamp() + "Done 'unPauseRxModem'");
                    }

                }

            }
        }
    }

    ;


    //Send Tune in a separate thread so that the UI thread is not blocked
    //  during TX
    public static void TxTune() {
        Runnable TxTuneRun = new TxTuneThread();
        new Thread(TxTuneRun).start();
    }

    private static class TxTuneThread implements Runnable {
        private AudioTrack at = null;

        public TxTuneThread() {
        }

        public void run() {

            //Let the regular title update kno we are just tuning, not transmitting a document
            modemIsTuning = true;

            //Stop the modem receiving side
            pauseRxModem();

            //Wait 1/2 second so that if there is potential RF feedback
            //  on the touchscreen we do not start TXing while the
            //  finger is still on the screen
            try {
                Thread.sleep(500);
            } catch (InterruptedException e1) {
                // TODO Auto-generated catch block
                e1.printStackTrace();
            }
            Processor.TXActive = true;
            AndFlmsg.mHandler.post(AndFlmsg.updatetitle);

            String frequencySTR = config.getPreferenceS("AFREQUENCY", "1500");
            int frequency = Integer.parseInt(frequencySTR);

            int volumebits = Integer.parseInt(config.getPreferenceS("VOLUME", "8"));

            //Note the multiplier value for the buffer size
            int intSize = 4 * android.media.AudioTrack.getMinBufferSize(8000,
                    AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_16BIT);

            if (AndFlmsg.toBluetooth) {
                //Android Bluetooth hack test
                //		        	at = new AudioTrack(AudioManager.STREAM_MUSIC, 8000, AudioFormat.CHANNEL_CONFIGURATION_MONO, AudioFormat.ENCODING_PCM_16BIT, intSize , AudioTrack.MODE_STREAM);
                at = new AudioTrack(AudioManager.STREAM_VOICE_CALL, 8000, AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_16BIT, intSize, AudioTrack.MODE_STREAM);
            } else {
                at = new AudioTrack(AudioManager.STREAM_MUSIC, 8000, AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_16BIT, intSize, AudioTrack.MODE_STREAM);
            }

            //Open audiotrack
            at.setStereoVolume(1.0f, 1.0f);
            at.play();

            int sr = 8000; // should be active_modem->get_samplerate();
            int symlen = (int) (1 * sr); //1 second buffer
            short[] outbuf = new short[symlen];

            double phaseincr;
            double phase = 0.0;
            phaseincr = 2.0 * Math.PI * frequency / sr;

            for (int i = 0; i < 3; i++) { //3 seconds tune
                for (int j = 0; j < symlen; j++) {
                    phase += phaseincr;
                    if (phase > 2.0 * Math.PI) phase -= 2.0 * Math.PI;
                    outbuf[j] = (short) ((int) (Math.sin(phase) * 8386560) >> volumebits);
                }
                at.write(outbuf, 0, symlen);
            }

            //Stop audio track
            at.stop();
            //Wait for end of audio play to avoid
            //overlaps between end of TX and start of RX
            while (at.getPlayState() == AudioTrack.PLAYSTATE_PLAYING) {
                try {
                    Thread.sleep(50);
                } catch (InterruptedException e) {
                    // TODO Auto-generated catch block
                }
            }
            //Close audio track
            at.release();
            Processor.TXActive = false;
            //Restart the modem receiving side
            unPauseRxModem();
            modemIsTuning = false;
            AndFlmsg.mHandler.post(AndFlmsg.updatetitle);
        }
    }

    ;
}
