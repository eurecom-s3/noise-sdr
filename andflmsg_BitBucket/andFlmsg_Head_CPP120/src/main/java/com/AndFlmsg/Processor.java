/*
 * Processor.java
 *
 * Copyright (C) 2011 John Douyere (VK2ETA)
 * Based on Pskmail from Per Crusefalk and Rein Couperus
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

import java.util.Calendar;
import java.util.concurrent.Semaphore;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.io.*;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.Service;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Environment;
import android.os.IBinder;
import ar.com.daidalos.afiledialog.FileChooserDialog;


/**
 * @author John Douyere (VK2ETA)
 */
public class Processor extends Service {


    static String application = "AndFlmsg 1.4.3"; // Used to preset an empty status
    static String version = "Version 1.4.3, 20210310";

    static boolean onWindows = true;
    static String ModemPreamble = "";  // String to send before any Tx Buffer
    static String ModemPostamble = ""; // String to send after any Tx buffer
    static String HomePath = "";
    static String Dirprefix = "";
    static final String DirInbox = "Inbox";
    static final String DirDrafts = "Drafts";
    static final String DirOutbox = "Outbox";
    static final String DirSent = "Sent";
    static final String DirEntryForms = "EntryForms";
    static final String DirDisplayForms = "DisplayForms";
    static final String DirTemplates = "Templates";
    static final String DirTemp = "Temp";
    static final String DirLogs = "Logs";
    static final String DirImages = "NBEMS-Images";
    static final String messageLogFile = "messagelog.txt";
    static final String tempCSVfn = "csvdata.csv";
    static final String tempPicfn = "tempicture.jpg";
    static String Separator = "";
    static boolean compressedMsg = false;
    static String Sendline = "";
    static String SendCommand = "";
    static String CrcString = "";
    static String FileNameString = "";
    static int DCD = 0;
    static int MAXDCD = 3;
    static boolean TXActive = false;
    //File name of last message received for appending a newly received picture
    static public String lastReceivedMessageFname = "";
    static public String[] lastMessageImgFieldName = new String[10];//Max 10 images per message
    static public int lastMessageNoExpectedImages = 0;
    static public int currentImageSequenceNo = 0;
    static public long lastMessageEndTxTime = 0;
    //Less than 20 seconds between the end of the text message and
    //	the start of mfsk picture transmission
    static boolean pictureRxInTime = false;


    //JD temp FIX: init as first modem in list
    static int TxModem = Modem.customModeListInt[0];
    static int RxModem = Modem.customModeListInt[0];

    static int imageTxModemIndex = 0;


    //Semaphores to instruct the RxTx Thread to start or stop
    public static Semaphore restartRxModem = new Semaphore(1, false);

    // globals to pass info to gui windows
    static String monitor = "";
    static String TXmonitor = "";
    static String TermWindow = "";
    static String Status = AndFlmsg.myContext.getString(R.string.txt_Listening);
    static int cpuload;

    // globals for communication
    static String mycall;     // my call sign from options
    static int DCDthrow;
    static boolean ReceivingForm = false;
    static String TX_Text; // output queue
    // Error handling and logging object
    static loggingclass log;


    public static void processor() {
        //Nothing as this is a service
    }

    @Override
    public void onCreate() {

        //Save Environment if we need to access Java code/variables from C++
        Message.saveEnv();

        // Create error handling class
        log = new loggingclass("AndFlmsg");

        // Get settings and initialize
        handleinitialization();

        //Initialize Modem (creates the various type of modem objects)
        Modem.ModemInit();

        //Check that we have a current mode, otherwise take the first one in the list (useful when we have a NIL list of custom modes)
        Processor.RxModem = Processor.TxModem = Modem.customModeListInt[Modem.getModeIndex(Processor.RxModem)];

        //Set the image modes defaults and limits
        imageTxModemIndex = Modem.getModeIndexFullList(Modem.getMode("MFSK64"));
        Modem.minImageModeIndex = Modem.getModeIndexFullList(Modem.getMode("MFSK16"));
        Modem.maxImageModeIndex = Modem.getModeIndexFullList(Modem.getMode("MFSK128"));

        //Reset frequency and squelch
        Modem.reset();

        //Make sure the display strings are blank
        Processor.monitor = "";
        Processor.TXmonitor = "";
        Processor.TermWindow = "";
    }


    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        // Start the RxThread
        Modem.startmodem();

        //Make sure Android keeps this running even if resources are limited
        //Display the notification in the system bar at the top at the same time
        startForeground(1, AndFlmsg.myNotification);

        // Keep this service running until it is explicitly stopped, so use sticky.
        //VK2ETA To-DO: Check if START_STICKY causes the service restart on ACRA report
        //VK2ETA	return START_STICKY;
        return START_NOT_STICKY;

    }


    @Override
    public void onDestroy() {
        // Kill the Rx Modem thread
        Modem.stopRxModem();
    }


    //Post to main terminal window
    public static void PostToTerminal(String text) {
        Processor.TermWindow += text;
        AndFlmsg.mHandler.post(AndFlmsg.addtoterminal);
    }


    //Post to main terminal window
    public static void PostToModem(String text) {
        Processor.monitor += text;
        AndFlmsg.mHandler.post(AndFlmsg.addtomodem);
    }


    //Process one block of received data. Called from Modem.RxBlock when a complete 
    //  block is received
    public static void processWrapBlock(String Blockline) {
        FileWriter out = null;
        try {
            boolean unwrapResult = Message.ProcessWrapBuffer(Blockline);
            if (unwrapResult) {
                //Save file and log it
                String resultString = Message.getUnwrapText();
                String resultFilename = Message.getUnwrapFilename();
                lastReceivedMessageFname = resultFilename;
                String inboxFolderPath = Processor.HomePath +
                        Processor.Dirprefix + Processor.DirInbox + Processor.Separator;
                File msgReceivedFile = new File(inboxFolderPath + lastReceivedMessageFname);
                if (msgReceivedFile.exists()) {
                    msgReceivedFile.delete();
                }
                out = new FileWriter(msgReceivedFile, true);
                out.write(resultString);
                out.close();
                //We now check how many, if any, attached image are expected
                Pattern psc = Pattern.compile("(^_img.*),<analog>(\\d+)", Pattern.MULTILINE);
                Matcher msc = psc.matcher(resultString);
                lastMessageNoExpectedImages = 0;
                currentImageSequenceNo = 0;
                boolean keepLooking = true;
                for (int start = 0; keepLooking; ) {
                    keepLooking = msc.find(start);
                    if (keepLooking) {
                        //Store the field name for this image (sequence in form = sequence of Tx)
                        lastMessageImgFieldName[lastMessageNoExpectedImages++] = msc.group(1);
                        start = msc.end();
                    }
                }
                //Save the time of end of Rx (any attached picture must be send within a set time)
                lastMessageEndTxTime = System.currentTimeMillis();
                //Advise reception of message
                PostToTerminal(AndFlmsg.myContext.getString(R.string.txt_ChecksumOK));
                PostToTerminal("\n" + AndFlmsg.myContext.getString(R.string.txt_SavedFile) + ": " + resultFilename);
                Message.addEntryToLog(AndFlmsg.myContext.getString(R.string.txt_ReceivedMessage) + ": " + resultFilename);
            } else {
                PostToTerminal("\n " + AndFlmsg.myContext.getString(R.string.txt_BadCrcFileContent) +
                        "\n" + Message.geterrtext());
                //Save file anyway into a Blank form file for further usage
                String resultString = Message.geterrtext();
                //Remove the WRAP section and try to store as a normal form
                int formStart = resultString.indexOf("]<flmsg>");
                if (formStart != -1) {
                    resultString = resultString.substring(formStart + 1);
                }
                File msgReceivedFile = new File(Processor.HomePath +
                        Processor.Dirprefix + Processor.DirInbox + Processor.Separator
                        + "Last_Bad_CrC_Rx");
                if (msgReceivedFile.exists()) {
                    msgReceivedFile.delete();
                }
                out = new FileWriter(msgReceivedFile, true);
                out.write(resultString);
                out.close();
                PostToTerminal("\n\n" + AndFlmsg.myContext.getString(R.string.txt_SavedRecoveredData));
                Message.addEntryToLog(AndFlmsg.myContext.getString(R.string.txt_SavedRecoveredData));
            }
        } catch (Exception e) {
            loggingclass.writelog("Exception Error in 'processWrapBlock' " + e.getMessage(), null, true);
        }
    }


    /**
     * Create or check the necessary folder structure
     */
    public static void handlefolderstructure() {

        //VK2ETA To-Do: add exception here when there is no external storage
        final String defaultPath = Environment.getExternalStorageDirectory().getAbsolutePath();
        // are we on Linux/Android OR Windows?
        if (File.separator.equals("/")) {
            Separator = "/";
            Dirprefix = "/NBEMS.files/";
            onWindows = false;
        } else {
            Separator = "\\";
            Dirprefix = "\\NBEMS.files\\";
            onWindows = true;
        }

        //debug
        //Toast toast = Toast.makeText(AndFlmsg.myContext, "Default: " + HomePath, Toast.LENGTH_LONG);
        //toast.show();

        //Choose the location of the base directory if not defined yet (first run or after de-install)
        HomePath = config.getPreferenceS("BASEPATH", "");
        if (HomePath.length() == 0) {
            AlertDialog.Builder myAlertDialog = new AlertDialog.Builder(AndFlmsg.myContext);
            myAlertDialog.setMessage(AndFlmsg.myContext.getString(R.string.txt_ChooseLocForNbemsFiles));
            myAlertDialog.setCancelable(false);
            myAlertDialog.setPositiveButton(AndFlmsg.myContext.getString(R.string.txt_UseDefault),
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                            //Save new Home Folder
                            SharedPreferences.Editor editor = AndFlmsg.mysp.edit();
                            editor.putString("BASEPATH", defaultPath);
                            // Commit the edits!
                            editor.commit();
                            //Make folders and copy forms
                            makeDirectoriesAndCopyForms(defaultPath);
                        }
                    });
            myAlertDialog.setNegativeButton(AndFlmsg.myContext.getString(R.string.txt_UseThisFolder),
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                            FileChooserDialog folderDialog = new FileChooserDialog(AndFlmsg.myContext);
                            folderDialog.loadFolder(defaultPath);
                            folderDialog.setFolderMode(true);
                            folderDialog.addListener(onFileSelectedListener);
                            folderDialog.show();
                        }
                    });
            myAlertDialog.show();
        } else {
            makeDirectoriesAndCopyForms(HomePath);
        }
    }


    // Handling of selected folder//
    private static FileChooserDialog.OnFileSelectedListener onFileSelectedListener = new FileChooserDialog.OnFileSelectedListener() {
        public void onFileSelected(Dialog source, File file) {
            source.hide();
            //Toast toast = Toast.makeText(AndFlmsg.myContext, "Base Folder Selected: " + file.getAbsoluteFile().getAbsolutePath(), Toast.LENGTH_LONG);
            //toast.show();
            //Save new Home Folder
            SharedPreferences.Editor editor = AndFlmsg.mysp.edit();
            editor.putString("BASEPATH", file.getAbsoluteFile().getAbsolutePath());
            // Commit the edits!
            editor.commit();
            //Now make the directories if not present and copy forms if at first run
            makeDirectoriesAndCopyForms(file.getAbsoluteFile().getAbsolutePath());
        }

        public void onFileSelected(Dialog source, File folder, String name) {
            source.hide();
            loggingclass.writelog("ERROR in File Chooser - Should not ask to create file: " + folder.getName() + "/" + name, null, true);
        }
    };


    /* Not used
    private static void deleteRecursive(File fileOrDirectory) {
	    if (fileOrDirectory.isDirectory())
	        for (File child : fileOrDirectory.listFiles())
	            deleteRecursive(child);
	    fileOrDirectory.delete();
	}
     */

    private static void makeDirectoriesAndCopyForms(String newHomePath) {
        try {

            //If different than default, make sure the default is removed first if present
            //if (!HomePath.equals(newHomePath)) {
            //File HomePathFile = new File(HomePath + Dirprefix);
            //Toast toast = Toast.makeText(AndFlmsg.myContext, "Please remove the old Folder: " + HomePath + Dirprefix, Toast.LENGTH_LONG);
            //toast.show();
            //Not used: check permission and security issues if the sdcards is seletected
            //deleteRecursive(HomePathFile);
            //}

            //Then save the new path
            HomePath = newHomePath;

            //Check if base directory exists, create if not
            File baseDir = new File(HomePath + Dirprefix);
            if (!baseDir.isDirectory()) {
                baseDir.mkdir();
            }

            File thisDir;

            //Check if Inbox directory exists, create if not
            thisDir = new File(HomePath + Dirprefix + DirInbox + Separator);
            if (!thisDir.isDirectory()) {
                thisDir.mkdir();
            }

            //Check if Outbox directory exists, create if not
            thisDir = new File(HomePath + Dirprefix + DirOutbox + Separator);
            if (!thisDir.isDirectory()) {
                thisDir.mkdir();
            }

            //Check if Sent directory exists, create if not
            thisDir = new File(HomePath + Dirprefix + DirSent + Separator);
            if (!thisDir.isDirectory()) {
                thisDir.mkdir();
            }

            //Check if Data Entry Forms directory exists, create if not
            thisDir = new File(HomePath + Dirprefix + DirEntryForms + Separator);
            if (!thisDir.isDirectory()) {
                thisDir.mkdir();
            }

            //Check if Display Forms directory exists, create if not
            thisDir = new File(HomePath + Dirprefix + DirDisplayForms + Separator);
            if (!thisDir.isDirectory()) {
                thisDir.mkdir();
            }

            //Check if Templates directory exists, create if not
            thisDir = new File(HomePath + Dirprefix + DirTemplates + Separator);
            if (!thisDir.isDirectory()) {
                thisDir.mkdir();
            }

            //Check if Drafts directory exists, create if not
            thisDir = new File(HomePath + Dirprefix + DirDrafts + Separator);
            if (!thisDir.isDirectory()) {
                thisDir.mkdir();
            }

            //Check if Logs directory exists, create if not
            thisDir = new File(HomePath + Dirprefix + DirLogs + Separator);
            if (!thisDir.isDirectory()) {
                thisDir.mkdir();
            }

            //Check if Images directory exists, create if not
            thisDir = new File(HomePath + Dirprefix + DirImages + Separator);
            if (!thisDir.isDirectory()) {
                thisDir.mkdir();
            }

            //Check if Temp directory exists, create if not
            thisDir = new File(HomePath + Dirprefix + DirTemp + Separator);
            if (!thisDir.isDirectory()) {
                thisDir.mkdir();
            }
            // Now clears the Temp directory of temporary files
            // This filter only returns files
            FileFilter fileFilter = new FileFilter() {
                public boolean accept(File file) {
                    return file.isFile();
                }
            };
            //Build array of strings containing the file names
            File[] files = thisDir.listFiles(fileFilter);
            for (int i = 0; i < files.length; i++) {
                files[i].delete();
            }

            //Now copies Forms into the EntryForms and DisplayForms folders BUT ONLY if
            //   the HAVERUNONCEBEFORE flag is not set in the preferences
            Boolean haveRunOnceBefore = config.getPreferenceB("HAVERUNONCEBEFORE", false);
            if (!haveRunOnceBefore) {
                //Scan the folders and copy accross
                String[] list;
                int size;
                byte[] buffer = new byte[2048];
                try {
                    list = AndFlmsg.myContext.getAssets().list("entryforms"); //All asset subfolders must be lower case
                    if (list.length > 0) {
                        // This is a folder
                        for (String file : list) {
                            InputStream is = AndFlmsg.myContext.getAssets().open("entryforms" + "/" + file);
                            FileOutputStream fout = new FileOutputStream(HomePath + Dirprefix + DirEntryForms + Separator + file);
                            BufferedOutputStream bufferOut = new BufferedOutputStream(fout, buffer.length);
                            //Copy file content
                            while ((size = is.read(buffer, 0, buffer.length)) != -1) {
                                bufferOut.write(buffer, 0, size);
                            }
                            bufferOut.flush();
                            bufferOut.close();
                            is.close();
                            fout.close();
                        }
                    }
                    list = AndFlmsg.myContext.getAssets().list("displayforms"); //All asset subfolders must be lower case
                    if (list.length > 0) {
                        // This is a folder
                        for (String file : list) {
                            InputStream is = AndFlmsg.myContext.getAssets().open("displayforms" + "/" + file);
                            FileOutputStream fout = new FileOutputStream(HomePath + Dirprefix + DirDisplayForms + Separator + file);
                            BufferedOutputStream bufferOut = new BufferedOutputStream(fout, buffer.length);
                            //Copy file content
                            while ((size = is.read(buffer, 0, buffer.length)) != -1) {
                                bufferOut.write(buffer, 0, size);
                            }
                            bufferOut.flush();
                            bufferOut.close();
                            is.close();
                            fout.close();
                        }
                    }
                    //Now save the flag that we have copied once (this is to avoid overwritting forms that would have been
                    //  modified or copied into the NBEMS.files subfolders
                    SharedPreferences.Editor editor = AndFlmsg.mysp.edit();
                    editor.putBoolean("HAVERUNONCEBEFORE", true);
                    // Commit the edits!
                    editor.commit();
                } catch (IOException ex) {
                    loggingclass.writelog("Could not copy Forms to the NBEMS.files folder. ", ex, true);
                }
            }
        } catch (Exception ex) {
            loggingclass.writelog("Problem when handling AndFlmsg folder structure.", ex, true);
        }
    }


    private static void handleinitialization() {

        try {
            // Initialize send queue
            TX_Text = "";
            ModemPreamble = config.getPreferenceS("MODEMPREAMBLE", "");
            ModemPostamble = config.getPreferenceS("MODEMPOSTAMBLE", "");
            // Compression settings
            compressedMsg = config.getPreferenceB("COMPRESSED");
            Processor.mycall = config.getPreferenceS("CALL");
        } catch (Exception e) {
            loggingclass.writelog("Problems with config parameter.", e, true);
        }
    }


    static double decayaverage(double oldaverage, double newvalue, double factor) {

        double newaverage = oldaverage;
        if (factor > 1) {
            newaverage = (oldaverage * (1 - 1 / factor)) + (newvalue / factor);
        }
        return newaverage;
    }


    //Return current time as a String
    static String myTime() {
        // create a java calendar instance
        Calendar calendar = Calendar.getInstance();

        // get a java.util.Date from the calendar instance.
        // this date will represent the current instant, or "now".
        java.util.Date now = calendar.getTime();

        // a java current time (now) instance
        java.sql.Timestamp currentTimestamp = new java.sql.Timestamp(now.getTime());

        return currentTimestamp.toString().substring(0, 16);
    }

    //Logging
    static void log(String logtext) {
        //           File consolelog = new File (HomePath + Dirprefix + "logfile");
        try {
            // Create file
            FileWriter logstream = new FileWriter(HomePath + Dirprefix + "logfile", true);
            BufferedWriter out = new BufferedWriter(logstream);

            out.write(myTime() + " " + logtext + "\n");
            //Close the output stream
            out.close();

        } catch (Exception e) {//Catch exception if any
            loggingclass.writelog("LogError " + e.getMessage(), null, true);
        }
        Processor.PostToTerminal(myTime() + " " + logtext + "\n");
    }


    @Override
    public IBinder onBind(Intent arg0) {
        // Nothing here, not used
        return null;
    }


}

