/*
 * Message.java  
 *
 * Copyright (C) 2014 John Douyere (VK2ETA) 
 *
 * This program is distributed in the hope that it will be useful,  
 * but WITHOUT ANY WARRANTY; without even the implied warranty of  
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
 * GNU General Public License for more details.  
 *
 * You should have received a copy of the GNU General Public License  
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.  
 *
 */


package com.AndFlmsg;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Locale;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.support.v4.content.FileProvider;
import android.util.Base64;


public class Message {

    public static String formName = new String();
    public static String fullDataString = new String();
    public static int attachedPictureCount = 0; //0 to 9 pictures attached MAX
    public static byte[][] attachedPictureArray = new byte[10][];
    public static int[] attachedPictureHeight = new int[10];
    public static int[] attachedPictureWidth = new int[10];
    public static Boolean attachedPictureColour = false;
    public static int attachedPictureTxSPP = 2;

    private final static String radiogramPrecedence[] = {"ROUTINE", "WELFARE", "PRIORITY", "EMERGENCY", "TEST ROUTINE", "TEST WELFARE", "TEST PRIORITY", "TEST EMERGENCY"};
    private final static String iaruPrecedence[] = {"ROUTINE", "PRIORITY", "EMERGENCY"};

    public final static String imageFieldPrefix = "_img";

    public static int embeddedImages = 0;

    //Declaration of native classes
    public native static void saveEnv();

    public native static boolean ProcessWrapBuffer(String rawWrapBuffer);

    public native static String getUnwrapText();

    public native static String getUnwrapFilename();

    public native static String geterrtext();

    public native static String customHtmlDisplayFormat(String html, String dataPairs);

    public native static String customHtmlEditingFormat(String html, String dataPairs);

    public native static String customHtmlEditFormat(String html, String dataPairs);

    public native static String namedFile();

    public native static String createCustomBuffer(String myHeader, int reason, String dataBuffer);

    public native static String createHardCodedHeader(String myHeader, int reason, String myFormName);

    public native static String compressMaybe(String outDataBuffer);

    public native static String updateHeader(String myHeader, int reason, String headerForm);

    public native static String estimate(String jModeName, String jBufferString);

    public native static String dateTimeStamp();

    public native static String dateStamp();

    public native static String timeStamp();

    public native static String[] getArlMsgTextList();

    public native static String[] getArlHxTextList();

    public native static String cbrgcheck(String msgFieldValue, boolean stdFormat);

    public native static String gettxtrgmsg();

    public native static String escape(String stringToEscape);

    public native static String unescape(String stringToUnescape);

    public native static String expandarl(String msgToExpand);


    static {
        System.loadLibrary("AndFlmsg_Flmsg_Interface");
    }

    //Search for a binary sequence in a buffer of bytes (Based on code from "joseluisbz" on www.stackoverflow.com. Thanks)
    public static Integer bytesIndexOf(byte[] Source, byte[] Search, int fromIndex) {
        boolean Find = false;
        int i;
        //Can't have a fromIndex past the buffer's end less test sequence
        if (fromIndex > Source.length - Search.length)
            return -1;
        for (i = fromIndex; i <= Source.length - Search.length; i++) {
            if (Source[i] == Search[0]) {
                Find = true;
                for (int j = 0; j < Search.length; j++) {
                    if (Source[i + j] != Search[j]) {
                        Find = false;
                    }
                }
            }
            if (Find) {
                break;
            }
        }
        if (!Find) {
            return -1;
        }
        return i;
    }


    public static void addEntryToLog(String entry) {
        String logFileName = Processor.HomePath + Processor.Dirprefix + Processor.DirLogs +
                Processor.Separator + Processor.messageLogFile;
        File logFile = new File(logFileName);
        if (!logFile.exists()) {
            try {
                logFile.createNewFile();
            } catch (IOException e) {
                loggingclass.writelog("IO Exception Error in Create file in 'addEntryToLog' " + e.getMessage(), null, true);
            }
        }
        try {
            //BufferedWriter for performance, true to set append to file flag
            BufferedWriter buf = new BufferedWriter(new FileWriter(logFile, true));
            buf.append(entry);
            buf.newLine();
            buf.flush();
            buf.close();
        } catch (IOException e) {
            loggingclass.writelog("IO Exception Error in add line in 'addEntryToLog' " + e.getMessage(), null, true);
        }
    }


    public static boolean saveMessageAsFile(String filePath, String fileName, String dataString) {
        FileWriter out = null;
        File fileToSave = new File(filePath + fileName);
        try {
            if (fileToSave.exists()) {
                fileToSave.delete();
            }
            out = new FileWriter(fileToSave, true);
            out.write(dataString);
            out.close();
            //Update the serial number used for the file name
            if (config.getPreferenceB("SERNBR_FNAME", true)) {
                int serNbr = config.getPreferenceI("SERNBR", 1);
                config.setPreferenceS("SERNBR", Integer.toString(++serNbr));
            }
            AndFlmsg.topToastText(AndFlmsg.myContext.getString(R.string.txt_SavedFile) + ": " + fileName);
            addEntryToLog(Message.dateTimeStamp() + " - " + AndFlmsg.myContext.getString(R.string.txt_SavedFile) + ": " + fileName);
        } catch (IOException e) {
            loggingclass.writelog("Error creating file", e, true);
            return false;
        }
        return true;
    }


    //Save a datastring into a file as specified
    public static boolean saveDataStringAsFile(String filePath, String fileName, String dataString) {
        FileWriter out = null;
        File fileToSave = new File(filePath + fileName);
        try {
            if (fileToSave.exists()) {
                fileToSave.delete();
            }
            out = new FileWriter(fileToSave, true);
            out.write(dataString);
            out.close();
            AndFlmsg.topToastText(AndFlmsg.myContext.getString(R.string.txt_SavedFile) + ": " + fileName + "\n");
            addEntryToLog(Message.dateTimeStamp() + " - " + AndFlmsg.myContext.getString(R.string.txt_SavedFile) + ": " + fileName);
        } catch (IOException e) {
            loggingclass.writelog("Error creating file", e, true);
            return false;
        }
        return true;
    }


    //Delete file 
    public static boolean deleteFile(String mFolder, String fileName, boolean adviseDeletion) {

        String fullFileName = Processor.HomePath + Processor.Dirprefix + mFolder + Processor.Separator + fileName;
        File n = new File(fullFileName);
        if (!n.isFile()) {
            return false;
        } else {
            n.delete();
            if (adviseDeletion) {
                AndFlmsg.middleToastText(AndFlmsg.myContext.getString(R.string.txt_DocumentDeleted));
                addEntryToLog(Message.dateTimeStamp() + " - " + AndFlmsg.myContext.getString(R.string.txt_DocumentDeleted) + ": " + fileName);
            }
            return true;
        }
    }


    //Copy binary or text files from one folder to another CONSERVING THE NAME and CONDITIONALLY LOGGING THE ACTION
    public static boolean copyAnyFile(String originFolder, String fileName, String destinationFolder, boolean adviseCopy) {

        File dir = new File(Processor.HomePath + Processor.Dirprefix + destinationFolder);
        if (dir.exists()) {
            String fullFileName = Processor.HomePath + Processor.Dirprefix + originFolder + Processor.Separator + fileName;
            File mFile = new File(fullFileName);
            if (!mFile.isFile()) {
                loggingclass.writelog("File not found: " + fullFileName, null, true);
                return false;
            } else {
                FileOutputStream fileOutputStrm = null;
                FileInputStream fileInputStrm = null;
                try {
                    fileInputStrm = new FileInputStream(fullFileName);
                    String fullDestinationFileName = Processor.HomePath + Processor.Dirprefix + destinationFolder + Processor.Separator + fileName;
                    fileOutputStrm = new FileOutputStream(fullDestinationFileName);
                    byte[] mBytebuffer = new byte[256];
                    int byteCount = 0;
                    while ((byteCount = fileInputStrm.read(mBytebuffer)) != -1) {
                        fileOutputStrm.write(mBytebuffer, 0, byteCount);
                    }
                } catch (FileNotFoundException e) {
                    loggingclass.writelog("File not found: " + fullFileName, e, true);
                } catch (IOException e) {
                    loggingclass.writelog("Error copying: " + fileName, e, true);
                } finally {
                    try {
                        if (fileInputStrm != null) {
                            fileInputStrm.close();
                        }
                        if (fileOutputStrm != null) {
                            fileOutputStrm.close();
                        }
                    } catch (IOException e) {
                        loggingclass.writelog("File close error: " + fullFileName, e, true);
                    }
                }
                if (adviseCopy)
                    addEntryToLog(Message.dateTimeStamp() + " - " + AndFlmsg.myContext.getString(R.string.txt_CopiedFile) + ": " + fileName +
                            " " + AndFlmsg.myContext.getString(R.string.txt_ToFolder) + ": " + destinationFolder);
                return true;
            }
        } else {
            loggingclass.writelog("Directory not found: " + destinationFolder + " ", null, true);
            return false;
        }
    }


    //Copy binary or text files from one folder to another WHILE CHANGING THE NAME
    public static boolean copyAnyFile(String originFolder, String fileName,
                                      String destinationFolder, String newFileName) {
        boolean result = true;

        File dir = new File(Processor.HomePath + Processor.Dirprefix + destinationFolder);
        if (dir.exists()) {
            String fullFileName = Processor.HomePath + Processor.Dirprefix + originFolder + Processor.Separator + fileName;
            File mFile = new File(fullFileName);
            if (!mFile.isFile()) {
                result = false;
            } else {
                FileOutputStream fileOutputStrm = null;
                FileInputStream fileInputStrm = null;
                try {
                    fileInputStrm = new FileInputStream(fullFileName);
                    String fullDestinationFileName = Processor.HomePath + Processor.Dirprefix + destinationFolder + Processor.Separator + newFileName;
                    fileOutputStrm = new FileOutputStream(fullDestinationFileName);
                    byte[] mBytebuffer = new byte[256];
                    int byteCount = 0;
                    while ((byteCount = fileInputStrm.read(mBytebuffer)) != -1) {
                        fileOutputStrm.write(mBytebuffer, 0, byteCount);
                    }
                } catch (FileNotFoundException e) {
                    loggingclass.writelog("File not found: " + fullFileName + " ", e, true);
                    result = false;
                } catch (IOException e) {
                    loggingclass.writelog("Error copying: " + fileName + " ", e, true);
                    result = false;
                } finally {
                    try {
                        if (fileInputStrm != null) {
                            fileInputStrm.close();
                        }
                        if (fileOutputStrm != null) {
                            fileOutputStrm.close();
                        }
                    } catch (IOException e) {
                        loggingclass.writelog("File close error: " + fullFileName, e, true);
                        result = false;
                    }
                }
            }
        } else {
            loggingclass.writelog("Directory not found: " + destinationFolder + " ", null, true);
            result = false;
        }
        if (result)
            addEntryToLog(Message.dateTimeStamp() + " - " + AndFlmsg.myContext.getString(R.string.txt_CopiedFile) + " " + newFileName +
                    " " + AndFlmsg.myContext.getString(R.string.txt_ToFolder) + " " + destinationFolder);
        return result;
    }


    //Creates a temporary file in Temp folder and a share intent (ready for a start Activity statement)
    //Returns the Intent. Used for sharing the form over the internet (Cloud, email, instant messaging, etc...)
    public static Intent shareInfoIntent(String mDisplayForm, String mFileName, String extension, int sharingAction) {

        FileWriter out = null;
        String tempFileName = new String();
        //Check if we have a file extension or not
        String fileNameWithoutExtension = (mFileName.indexOf(".") == -1) ? mFileName : mFileName.substring(0, mFileName.lastIndexOf("."));
        //Create temporary html file to attach to email
        tempFileName = Processor.HomePath +
                Processor.Dirprefix + Processor.DirTemp + Processor.Separator +
                fileNameWithoutExtension + extension;
        //Debug
        //Processor.PostToTerminal("tempFileName <" + tempFileName + ">\n\n");
        File htmlFormatedFile = new File(tempFileName);
        try {
            if (htmlFormatedFile.exists()) {
                htmlFormatedFile.delete();
            }
            out = new FileWriter(htmlFormatedFile, true);
            out.write(mDisplayForm);
            out.close();
        } catch (IOException e) {
            loggingclass.writelog("Error creating temporary file in Temp folder", e, true);
        }
        Intent share = null;
        if (sharingAction == AndFlmsg.FORSENDINGASATTACHMENT) {
            share = new Intent(Intent.ACTION_SEND);
            share.setType("text/plain");
            share.putExtra(Intent.EXTRA_TEXT, "Form attached\n");
            //share.putExtra(Intent.EXTRA_HTML_TEXT, myDisplayForm);
            //share.putExtra(Intent.EXTRA_TEXT, mDisplayForm);
            //API 24+ does not allow revealing file location
            // share.putExtra(Intent.EXTRA_STREAM, Uri.fromFile(new File(tempFileName)));
            share.putExtra(Intent.EXTRA_STREAM, FileProvider.getUriForFile(AndFlmsg.myContext, AndFlmsg.myContext.getApplicationContext().getPackageName() + ".provider", new File(tempFileName)));
            share.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
            //startActivity(Intent.createChooser(share, "Share Form"));
            //Intent emailIntent = new Intent(Intent.ACTION_SENDTO,
            //								Uri.fromParts("mailto", mailId, null));
            share.putExtra(Intent.EXTRA_SUBJECT, "Message " + mFileName);
            // you can use simple text like this
            // emailIntent.putExtra(android.content.Intent.EXTRA_TEXT,"Body text here");
            // or get fancy with HTML like this
            // share.putExtra(Intent.EXTRA_TEXT, Html.fromHtml(myDisplayForm));
        } else if (sharingAction == AndFlmsg.FORSENDINGASTEXT) {
            share = new Intent(Intent.ACTION_SEND);
            share.setType("text/plain");
            //share.putExtra(Intent.EXTRA_TEXT, "Form attached\n");
            //share.putExtra(Intent.EXTRA_HTML_TEXT, myDisplayForm);
            share.putExtra(Intent.EXTRA_TEXT, mDisplayForm);
            //API 24+ does not allow revealing file location
            // share.putExtra(Intent.EXTRA_STREAM, Uri.fromFile(new File(tempFileName)));
            //share.putExtra(Intent.EXTRA_STREAM, FileProvider.getUriForFile(AndFlmsg.myContext, AndFlmsg.myContext.getApplicationContext().getPackageName() + ".provider", new File(tempFileName)));
            //share.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
            //startActivity(Intent.createChooser(share, "Share Form"));
            //Intent emailIntent = new Intent(Intent.ACTION_SENDTO,
            //								Uri.fromParts("mailto", mailId, null));
            //Add file name information if the format is not a wrap format
            if (!mDisplayForm.startsWith("[WRAP:beg]")) {
                share.putExtra(Intent.EXTRA_SUBJECT, "Message " + mFileName + "\n");
            }
            // you can use simple text like this
            // emailIntent.putExtra(android.content.Intent.EXTRA_TEXT,"Body text here");
            // or get fancy with HTML like this
            // share.putExtra(Intent.EXTRA_TEXT, Html.fromHtml(myDisplayForm));
        } else { //Make sure there is a default: if (sharingAction == AndFlmsg.FORPRINTING) {
            share = new Intent(Intent.ACTION_VIEW);
            //API 24+ does not allow revealing file location
            //share.setDataAndType(Uri.fromFile(new File(tempFileName)), "text/html");
            share.setDataAndType(FileProvider.getUriForFile(AndFlmsg.myContext, AndFlmsg.myContext.getApplicationContext().getPackageName() + ".provider", new File(tempFileName)), "text/html");
            share.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        }
        return share;
    }


    //Takes a directory for the data file, a data file name, and an extension for the form file name and returns a formatted string
    //Returns an HTML format String. For custom forms, ALL INPUT field elements are marked as read-only
    public static String formatForDisplay(String mDir, String mFileName, String extension) {
        boolean isCustomForm = false;
        String dataString = "";
        String mDisplayForm = "";
        fullDataString = "";

        try {
            //First determine the file name of the form used for displaying the data
            //For this, read the data file until we find the form information
            File fi = new File(Processor.HomePath + Processor.Dirprefix
                    + mDir + Processor.Separator + mFileName);
            if (!fi.isFile()) {
                AndFlmsg.middleToastText(AndFlmsg.myContext.getString(R.string.txt_FileNotFound) + ": " + mFileName);
                return "";
            }
            FileInputStream fileISi = new FileInputStream(fi);
            BufferedReader buf0 = new BufferedReader(new InputStreamReader(fileISi));
            String readString0 = new String();
            boolean found_flmsg = false;
            boolean found_form = false;
            boolean found_attachment = false;
            isCustomForm = false;
            dataString = "";

            //Handles both hard-coded forms and custom forms
            while ((readString0 = buf0.readLine()) != null) {
                //While we are reading the file, store its complete content in a string for future usage
                dataString += readString0 + "\n";
                fullDataString += readString0 + "\n";
                if (isCustomForm && readString0.contains("CUSTOM_FORM")) {
                    //Overwrite formName with custom form file name
                    //For custom form, include the extension as it is variable
                    formName = readString0.substring(readString0.indexOf("CUSTOM_FORM") + 12);
                    found_form = true;
                    //Reset the dataString to start at content of :mg: field (CUSTOM_FORM ... then pairs of name, values)
                    dataString = readString0.substring(readString0.indexOf("CUSTOM_FORM")) + "\n";
                }
                if (found_flmsg && !found_form && readString0.contains(">") && readString0.startsWith("<")) { //Stop looking when found
                    found_form = true;
                    formName = readString0.substring(1, readString0.lastIndexOf('>')) + extension;
                    //Reset dataString so that we excude the header information
                    dataString = "";
                }
                if (found_form && formName.equals("customform.html")) {
                    isCustomForm = true;
                    found_form = false; //Re-set until we found the custom form name
                }
                if (readString0.startsWith("<flmsg>")) {
                    found_flmsg = true;
                }
                if (readString0.startsWith(imageFieldPrefix)) {
                    //No processing required as the Base64 encoed image is passed in a
                    //   HIDDEN TEXTAREA field in the form
                    found_attachment = true;
                }
            }
            //Include error handling if form not valid or does not include a form
            //  name (typically for corrupted received messages)
            if (!found_flmsg || !found_form) {
                AndFlmsg.middleToastText(AndFlmsg.myContext.getString(R.string.txt_NoFormNameFound));
                //Convert the HTML reserved characters "&", "<" and ">" to their entity name
                mDisplayForm = fullDataString.replaceAll("&", "&amp;");  //Do this first of course
                mDisplayForm = mDisplayForm.replaceAll("<", "&lt;");
                mDisplayForm = mDisplayForm.replaceAll(">", "&gt;");
                //Since it is displayed as text, replace all \n with <br> as we display in a Webview
                mDisplayForm = mDisplayForm.replaceAll("\n", "<br>");
                //Then escape the special characters
                //mDisplayForm = escape(mDisplayForm);
                return mDisplayForm;
            }
            //First read the form file into a string variable
            //Debug  Processor.PostToTerminal("Templ. name <" + formName + ">\n\n");
            File f = new File(Processor.HomePath + Processor.Dirprefix
                    + Processor.DirDisplayForms + Processor.Separator + formName);
            FileInputStream fileIS = new FileInputStream(f);
            BufferedReader buf = new BufferedReader(new InputStreamReader(fileIS));
            String readString = new String();
            while ((readString = buf.readLine()) != null) {
                mDisplayForm += readString + "\n";
            }
            //Then two processings here: standard hard-coded forms and Custom forms
            if (isCustomForm) {
                //If we found an image element, inject the javascript to display  the images
                if (found_attachment) {
                    if (mDisplayForm.contains("</BODY>")) {
                        mDisplayForm = mDisplayForm.replaceFirst("</BODY>", "</BODY>" + AndFlmsg.injectJSforImages);
                    } else if (mDisplayForm.contains("</body>")) {
                        mDisplayForm = mDisplayForm.replaceFirst("</body>", "</body>" + AndFlmsg.injectJSforImages);
                    }
                }
                //Call Native code to format in HTML with updated VALUE and ReadOnly attributes
                mDisplayForm = Message.customHtmlDisplayFormat(mDisplayForm, dataString); //C++ code
                //Prevent out of memory errors by releasing unused Strings
                dataString = null;
                //Debug Processor.PostToTerminal("\nReturned from C++ <" + mDisplayForm + ">\n\n");
            } else { //Hard coded forms format
                //Perform form specific processing
                if (formName.startsWith("radiogram")) {
                    //Build and add the :rx: field from preferences
                    String rxField = config.getPreferenceS("CALL", "") + "<br>" +
                            config.getPreferenceS("TEL", "") + "<br>" +
                            config.getPreferenceS("NAME", "") + "<br>" +
                            config.getPreferenceS("ADDRESS", "") + "<br>" +
                            config.getPreferenceS("CITYSTATEZIP", "") + "<br>" +
                            config.getPreferenceS("EMAIL", "");
                    dataString += ":rx:" + Integer.toString(rxField.length()) + " " + rxField + "\n";
                    //If selected, expand the ARL Messages
                    if (config.getPreferenceB("SHOWARLDESC", true)) {
                        String messageField = getHcfFieldContent(dataString, "msg");
                        String expandedStr = expandarl(messageField);
                        dataString += ":exp:" + Integer.toString(expandedStr.length()) + " " + expandedStr + "\n";
                    }
                    //Reformat the boolean variables
                    if (dataString.indexOf(":std:1 T") != -1)
                        dataString = dataString.replace(":std:1 T", ":std:2 ON");
                    if (dataString.indexOf(":std:1 F") != -1)
                        dataString = dataString.replace(":std:1 F", ":std:3 OFF");
                    if (dataString.indexOf(":svc:1 T") != -1)
                        dataString = dataString.replace(":svc:1 T", ":svc:2 ON");
                    if (dataString.indexOf(":svc:1 F") != -1)
                        dataString = dataString.replace(":svc:1 F", ":svc:3 OFF");
                    //Reformat the "precedence" field
                    int precValue = Integer.parseInt(getHcfFieldContent(dataString, "prec"));
                    if (precValue < 0 || precValue >= radiogramPrecedence.length) precValue = 0;
                    dataString = setHcfFieldContent(dataString, "prec", radiogramPrecedence[precValue]);
                } else if (formName.startsWith("iaru")) {
                    //Reformat the "precedence" field
                    int precValue = Integer.parseInt(getHcfFieldContent(dataString, "prec"));
                    if (precValue < 0 || precValue >= iaruPrecedence.length) precValue = 0;
                    dataString = setHcfFieldContent(dataString, "prec", iaruPrecedence[precValue]);
                } else if (formName.startsWith("ics206")) {
                    //Reformat the boolean variables
                    for (int i = 0; i < 5; i++) {
                        if (dataString.indexOf(":mpar" + Integer.toString(i) + ": T") != -1)
                            dataString = dataString.replace(":mpar" + Integer.toString(i) + ": T", ":mpar" + Integer.toString(i) + ":3 YES");
                        if (dataString.indexOf(":tpara" + Integer.toString(i) + ": T") != -1)
                            dataString = dataString.replace(":tpara" + Integer.toString(i) + ": T", ":tpara" + Integer.toString(i) + ":3 YES");
                        if (dataString.indexOf(":apara" + Integer.toString(i) + ": T") != -1)
                            dataString = dataString.replace(":apara" + Integer.toString(i) + ": T", ":apara" + Integer.toString(i) + ":3 YES");
                        if (dataString.indexOf(":hheli" + Integer.toString(i) + ": T") != -1)
                            dataString = dataString.replace(":hheli" + Integer.toString(i) + ": T", ":hheli" + Integer.toString(i) + ":3 YES");
                        if (dataString.indexOf(":hburn" + Integer.toString(i) + ": T") != -1)
                            dataString = dataString.replace(":hburn" + Integer.toString(i) + ": T", ":hburn" + Integer.toString(i) + ":3 YES");
                    }
                } else if (formName.startsWith("html_form")) {
                    mDisplayForm = dataString; //Move content into form
                    dataString = " \n"; //Blank content (to prevent fields replacement below)
                }
                //Perform standard processing for Hard Coded forms
                //Send file name as :TITLE: field
                dataString += ":TITLE:" + Integer.toString(mFileName.length()) + " " + mFileName + "\n";
                //Scan the formatted dataString for field and their values
                //    (The 3 patterns are IN THAT ORDER of MATCH <:field:length value> OR <\n> OR <any Text>)
                Pattern psc = Pattern.compile("^(:.+:)(\\d+) (.*)|(^\\n)|(.+)", Pattern.MULTILINE);
                Matcher msc = psc.matcher(dataString);
                boolean keepLooking = true;
                String element = "";
                String value = "";
                String tempBuffer = "";
                int fieldLength = 0;
                int runningFieldLength = 0;
                boolean newUncompleteField = false;
                //If a match is found, ensure we read the <Length> number of characters before declaring we have a complete field.
                // Replace the fields in the form with the values found as we go.
                for (int start = 0; (keepLooking && start < dataString.length()); ) {
                    keepLooking = msc.find(start);
                    if (keepLooking) {
                        if (msc.group(1) != null) { //We have a ":fieldname:nn fieldvalue" line
                            newUncompleteField = true;//Assumed true, reset below if not true
                            element = msc.group(1);
                            fieldLength = Integer.parseInt(msc.group(2));
                            value = msc.group(3);
                            if (fieldLength <= value.length()) {
                                newUncompleteField = false;
                                //Add conversion of "\n" to <br>
                                mDisplayForm = mDisplayForm.replace(element, LFtobr(value));
                            } else {
                                runningFieldLength = value.length();
                                tempBuffer = value;
                            }
                        } else if (msc.group(4) != null) {//We have just a line feed, nothing else
                            if (newUncompleteField) {
                                String a4 = msc.group(4);
                                runningFieldLength += a4.length();
                                tempBuffer += a4;
                                if (fieldLength <= runningFieldLength) {
                                    newUncompleteField = false;
                                    //Add conversion of "\n" to <br>
                                    mDisplayForm = mDisplayForm.replace(element, LFtobr(tempBuffer));
                                    tempBuffer = "";
                                }
                            }
                        } else if (msc.group(5) != null) { //We have a 2nd, 3rd etc.. line of text
                            if (newUncompleteField) {
                                value = msc.group(5);
                                runningFieldLength += value.length() + 1;
                                tempBuffer += "\n" + value;
                                if (fieldLength <= runningFieldLength) {
                                    newUncompleteField = false;
                                    //Add conversion of "\n" to <br>
                                    mDisplayForm = mDisplayForm.replace(element, LFtobr(tempBuffer));
                                    tempBuffer = "";
                                }
                            }
                        }
                    }
                    if (keepLooking) start = msc.end();
                }
                //Then Remove the missing ":xxxx:" field templates that were not transmitted (because they were blank / Un-checked)
                String fieldName = new String();
                psc = Pattern.compile("\\:\\S+\\:");
                msc = psc.matcher(mDisplayForm);
                while (msc.find()) {
                    fieldName = msc.group(0);
                    mDisplayForm = mDisplayForm.replace(fieldName, "");
                }
                //Finally, if it is a text display format, re-format to HTML for Webview
                if (extension.equals(".txt")) {
                    //Convert the HTML reserved characters "&", "<" and ">" to their entity name
                    mDisplayForm = mDisplayForm.replaceAll("&", "&amp;");  //Do this first of course
                    mDisplayForm = mDisplayForm.replaceAll("<", "&lt;");
                    mDisplayForm = mDisplayForm.replaceAll(">", "&gt;");
                    //Since it is displayed as text, replace all \n with <br> as we display in a Webview
                    mDisplayForm = mDisplayForm.replaceAll("\n", "<br>");
                    //Then escape the special characters
                    //mDisplayForm = escape(mDisplayForm);
                }
            }
        } catch (FileNotFoundException e) {
            AndFlmsg.middleToastText(formName + " " + AndFlmsg.myContext.getString(R.string.txt_NotAnExistingForm));
            //Convert the HTML reserved characters "&", "<" and ">" to their entity name
            mDisplayForm = fullDataString.replaceAll("&", "&amp;");  //Do this first of course
            //Minimize memory usage, allow GC of String
            fullDataString = null;
            mDisplayForm = mDisplayForm.replaceAll("<", "&lt;");
            mDisplayForm = mDisplayForm.replaceAll(">", "&gt;");
            //Since it is displayed as text, replace all \n with <br> as we display in a Webview
            mDisplayForm = mDisplayForm.replaceAll("\n", "<br>");
        } catch (IOException e) {
            loggingclass.writelog("IO Exception Error in 'formatForDisplay' " + e.getMessage(), null, true);
        }
        return mDisplayForm;
    }

    //Convert all "\n" (LF) into <br> elements for HTML display of multi-lines TEXTAREAs
    private static String LFtobr(String inString) {

        return inString.replaceAll("\n", "<br>");
    }


    //Takes a directory for the data file, a data file name, and returns a formatted string
    //Returns an HTML format String. For custom forms, ALL field elements are marked as read-only
    public static String formatForEditing(String mDir, String mFileName) {
        boolean isCustomForm = false;
        String dataString = new String();
        String mDisplayForm = new String();
        boolean found_attachment = false;

        try {
            //First determine the file name of the form used for displaying the data
            //For this read the data file until we find the form information
            File fi = new File(Processor.HomePath + Processor.Dirprefix
                    + mDir + Processor.Separator + mFileName);
            FileInputStream fileISi = new FileInputStream(fi);
            BufferedReader buf0 = new BufferedReader(new InputStreamReader(fileISi));
            String readString0 = new String();
            boolean found_flmsg = false;
            boolean found_form = false;
            isCustomForm = false;
            dataString = "";
            //Handles both hard-coded forms and custom forms
            while ((readString0 = buf0.readLine()) != null) {
                //While we are reading the file, store its complete content in a string for future usage
                dataString += readString0 + "\n";
                if (isCustomForm && readString0.contains("CUSTOM_FORM")) {
                    //Overwrite formName with custom form file name
                    //For custom form, include the extension as it is variable
                    formName = readString0.substring(readString0.indexOf("CUSTOM_FORM") + 12);
                    found_form = true;
                    //Reset the dataString to start at content of :mg: field (CUSTOM_FORM ... then pairs of name, values)
                    dataString = readString0.substring(readString0.indexOf("CUSTOM_FORM")) + "\n";
                }
                if (found_flmsg && !found_form && readString0.contains(">") && readString0.startsWith("<")) {
                    found_form = true;
                    formName = readString0.substring(1, readString0.lastIndexOf('>')) + ".html";
                    //Save header if we save draft message
                    AndFlmsg.mHeader = dataString;
                    dataString = ""; //Reset dataString
                }
                if (found_form && formName.equals("customform.html")) {
                    isCustomForm = true;
                    found_form = false; //Re-set until we found the custom form name
                }
                if (readString0.startsWith("<flmsg>")) {
                    found_flmsg = true;
                }
                if (readString0.startsWith(imageFieldPrefix)) {
                    //No processing required as the Base64 encoed image is passed in a
                    //   HIDDEN TEXTAREA field in the form
                    found_attachment = true;
                }
            }
            //Include error handling if form not valid or does not include form name
            if (!found_flmsg || !found_form) {
                AndFlmsg.middleToastText(formName + AndFlmsg.myContext.getString(R.string.txt_NoFormNameFound));
                return "";
            }
            //First read the form file into a string variable
            //Debug  Processor.PostToTerminal("Templ. name <" + formName + ">\n\n");
            File f = new File(Processor.HomePath + Processor.Dirprefix
                    + Processor.DirEntryForms + Processor.Separator + formName);
            FileInputStream fileIS = new FileInputStream(f);
            BufferedReader buf = new BufferedReader(new InputStreamReader(fileIS));
            String readString = new String();
            while ((readString = buf.readLine()) != null) {
                mDisplayForm += readString + "\n";
            }
            //Then two processings here: standard hard-coded forms and Custom forms
            if (isCustomForm) {
                //If we found an image element, inject the javascript to display  the images
                if (found_attachment) {
                    if (mDisplayForm.contains("</BODY>")) {
                        mDisplayForm = mDisplayForm.replaceFirst("</BODY>", "</BODY>" + AndFlmsg.injectJSforImages);
                    } else if (mDisplayForm.contains("</body>")) {
                        mDisplayForm = mDisplayForm.replaceFirst("</body>", "</body>" + AndFlmsg.injectJSforImages);
                    }
                }
            } else { //Hard coded forms format
                //Perform form specific processing
                if (formName.startsWith("radiogram.")) {
                    //Reformat the boolean variables
                    if (dataString.indexOf(":std:1 T") != -1)
                        dataString = dataString.replace(":std:1 T", ":std:2 ON");
                    if (dataString.indexOf(":std:1 F") != -1)
                        dataString = dataString.replace(":std:1 F", ":std:3 OFF");
                    if (dataString.indexOf(":svc:1 T") != -1)
                        dataString = dataString.replace(":svc:1 T", ":svc:2 ON");
                    if (dataString.indexOf(":svc:1 F") != -1)
                        dataString = dataString.replace(":svc:1 F", ":svc:3 OFF");
                } else if (formName.startsWith("hics213.")) {
                    //Reformat the boolean variables
                    if (dataString.indexOf(":5a:1 X") != -1)
                        dataString = dataString.replace(":5a:1 X", ":5.a:7 CHECKED");
                    if (dataString.indexOf(":5b:1 X") != -1)
                        dataString = dataString.replace(":5b:1 X", ":5.b:7 CHECKED");
                    if (dataString.indexOf(":5c:1 X") != -1)
                        dataString = dataString.replace(":5c:1 X", ":5.c:7 CHECKED");
                    if (dataString.indexOf(":6a:1 X") != -1)
                        dataString = dataString.replace(":6a:1 X", ":6.a:7 CHECKED");
                    if (dataString.indexOf(":6b:1 X") != -1)
                        dataString = dataString.replace(":6b:1 X", ":6.b:7 CHECKED");
                    if (dataString.indexOf(":7a:1 X") != -1)
                        dataString = dataString.replace(":7a:1 X", ":7.a:7 CHECKED");
                    if (dataString.indexOf(":7b:1 X") != -1)
                        dataString = dataString.replace(":7b:1 X", ":7.b:7 CHECKED");
                    if (dataString.indexOf(":7c:1 X") != -1)
                        dataString = dataString.replace(":7c:1 X", ":7.c:7 CHECKED");
                }
                //Re-format hard coded form format to custom form format
                Pattern psc = Pattern.compile("^(:.+:)(\\d+) (.*)|(^\\n)|(.+)", Pattern.MULTILINE);
                Matcher msc = psc.matcher(dataString);
                boolean keepLooking = true;
                String element;
                String value = "";
                String tempBuffer = "";
                int fieldLength = 0;
                int runningFieldLength = 0;
                boolean newUncompleteField = false;

                //For compatibility with Flmsg 2.0.12 (as opposed to 2.0.4) the fields values are all escaped
                //  prior to sending to the customform editor
                for (int start = 0; (keepLooking && start < dataString.length()); ) {
                    keepLooking = msc.find(start);
                    if (keepLooking) {
                        if (msc.group(1) != null) { //We have a ":fieldname:nn fieldvalue" line
                            newUncompleteField = true;//Assumed true, reset below if not true
                            element = msc.group(1);
                            // Processor.PostToTerminal("Field name <" + fieldName + ">\n");
                            fieldLength = Integer.parseInt(msc.group(2));
                            value = msc.group(3);
                            // Processor.PostToTerminal("Field value <" + fieldValue + ">\n\n");
                            if (fieldLength <= value.length()) {
                                newUncompleteField = false;
                                //tempBuffer += element + "," + value + "\n";
                                tempBuffer += element + "," + Message.escape(value) + "\n";
                            } else {
                                runningFieldLength = value.length();
                                //No quotes like in Flmsg 2.0.4
                                //tempBuffer += element + ",\"" + value;
                                tempBuffer += element + "," + Message.escape(value);
                            }
                        } else if (msc.group(4) != null) {//We have just a line feed, nothing else
                            if (newUncompleteField) {
                                String a4 = msc.group(4);
                                runningFieldLength += a4.length();
                                tempBuffer += Message.escape(a4);
                                if (fieldLength <= runningFieldLength) {
                                    newUncompleteField = false;
                                    //No quotes like in Flmsg 2.0.4
                                    //tempBuffer += "\"\n";
                                    tempBuffer += Message.escape("\n");
                                }
                            }
                        } else if (msc.group(5) != null) { //We have a 2nd, 3rd etc.. line of text
                            if (newUncompleteField) {
                                value = msc.group(5);
                                runningFieldLength += value.length() + 1;
                                tempBuffer += Message.escape("\n" + value);
                                if (fieldLength <= runningFieldLength) {
                                    newUncompleteField = false;
                                    //No quotes like in Flmsg 2.0.4
                                    //tempBuffer += "\"\n";
                                    tempBuffer += "\n";
                                }
                            }
                        }
                    }
                    if (keepLooking) start = msc.end();
                }
                dataString = tempBuffer;
            }
            //Call Native code to format in HTML with updated VALUE and ReadOnly attributes
            mDisplayForm = Message.customHtmlEditingFormat(mDisplayForm, dataString); //C++ code
            //Debug
            //Processor.PostToTerminal("\nReturned from C++ <" + mDisplayForm + ">\n\n");
        } catch (FileNotFoundException e) {
            AndFlmsg.middleToastText(formName + AndFlmsg.myContext.getString(R.string.txt_NotAnExistingForm) +
                    " " + AndFlmsg.myContext.getString(R.string.txt_CantEditThatMessage));
        } catch (IOException e) {
            loggingclass.writelog("IO Exception Error in 'formatForEditing' " + e.getMessage(), null, true);
        }
        return mDisplayForm;
    }


    //Takes a directory for the data file, a data file name
    //Returns a wrapped and possibly compressed file ready for Txing via Radio Modems
    public static String formatForTx(String mDir, String mFileName, boolean imagesInDigitalForm, boolean allowCompression) {
        String dataString = new String();
        String txBuffer = new String();
        String readString0 = new String();
        String headerString = new String();
        String chksumString = new String();
        boolean found_flmsg = false;
        boolean found_form = false;
        String headerForm = new String();
        attachedPictureCount = 0; //Reset index of extracted pictures

        try {
            //First separate the header from the data fields (Headers are never compressed)
            //For this read the data file until we find the form information
            File fi = new File(Processor.HomePath + Processor.Dirprefix
                    + mDir + Processor.Separator + mFileName);
            FileInputStream fileISi = new FileInputStream(fi);
            BufferedReader buf0 = new BufferedReader(new InputStreamReader(fileISi));
            dataString = "";
            boolean found_attachment = false;
            //Handles both hard-coded forms and custom forms
            while ((readString0 = buf0.readLine()) != null) {
                if (readString0.startsWith(imageFieldPrefix)) {
                    //Assign to an array element for multiple images (up to 10)
                    if (extractPictureArray(readString0)) {
                        attachedPictureCount++;
                        found_attachment = true;
                        if (!imagesInDigitalForm) {
                            //In analog, then insert a marker for this field so that it is transmitted and
                            //  recognised as such on the RX side
                            readString0 = readString0.substring(0, readString0.indexOf(",") + 1) + "<analog>" +
                                    Integer.toString(attachedPictureCount) + "\n";
                        }
                    }
                }
                //While we are reading the file, store its complete content in a string for future usage
                // Skip the textareas fields containing the image(s)
                dataString += readString0 + "\n";
                if (found_flmsg && !found_form && readString0.contains(">") && readString0.startsWith("<")) {
                    found_form = true;
                    headerForm = readString0;
                    headerString = dataString;
                    //Reset for capturing the data fields values
                    dataString = "";
                }
                if (readString0.startsWith("<flmsg>")) {
                    found_flmsg = true;
                }
            }
            //Avoid leaving files and streams open - memory leaks
            buf0.close();
            fileISi.close();
            //Include error handling if form not valid or does not include form name
            if (!found_flmsg) {
                AndFlmsg.middleToastText(AndFlmsg.myContext.getString(R.string.txt_NoFlmsgStatement));
                return "";
            }
            Message.saveEnv(); //To enable access to preferences
            //Update header
            headerString = Message.updateHeader(headerString, AndFlmsg.header_reason_FROM, headerForm);
            //Process message body
            if (allowCompression) dataString = Message.compressMaybe(dataString);
            txBuffer = "[WRAP:fn " + mFileName + "]" +
                    headerString + dataString;
            chksumString = CheckSum.Crc16(txBuffer);
            txBuffer = "[WRAP:beg][WRAP:lf]" + txBuffer +
                    "[WRAP:chksum " + chksumString + "][WRAP:end]\n";
        } catch (FileNotFoundException e) {
            AndFlmsg.middleToastText(AndFlmsg.myContext.getString(R.string.txt_FileNotFound) + ": " + mFileName);
        } catch (IOException e) {
            loggingclass.writelog("IO Exception Error in 'formatForTx' " + e.getMessage(), null, true);
        }
        return txBuffer;
    }


    //Takes a message line containing an URL Base64 encoded image and creates a byte array ready for TX by MFSK
    private static boolean extractPictureArray(String fieldData) {
        String attachmentBuffer;
        Bitmap attachedPictureBitmap = null;

        try {
            //Find first comma
            int commaPos = fieldData.indexOf(",");
            //Find 2nd comma
            commaPos = fieldData.indexOf(",", commaPos + 1);
            if (fieldData.length() > commaPos + 2) {//Just in case the field was blank
                attachmentBuffer = fieldData.substring(commaPos + 1);
            } else attachmentBuffer = "";
            //Process the attached data found
            //Base64 decode to image
            byte[] rawPictureBuffer = Base64.decode(attachmentBuffer, Base64.DEFAULT);
            //Clean large obsolete variables
            attachmentBuffer = null;
            //System.gc();
            //Decode image into Bitmap
            BitmapFactory.Options options = new BitmapFactory.Options();
            options.inPreferredConfig = Bitmap.Config.ARGB_8888;
            attachedPictureBitmap = BitmapFactory.decodeByteArray(rawPictureBuffer, 0, rawPictureBuffer.length, options);
            rawPictureBuffer = null;
            //Propose GC at this point as the ByteBuffer allocation is memory intensive
            System.gc();
            //For resizing
            //Bitmap.createScaledBitmap(yourBitmap, 50, 50, true); // Width and Height in pixel e.g. 50
            //Size of buffer containing the Bitmap
            attachedPictureWidth[attachedPictureCount] = attachedPictureBitmap.getWidth();
            attachedPictureHeight[attachedPictureCount] = attachedPictureBitmap.getHeight();
            int pictureArraySize = attachedPictureWidth[attachedPictureCount] *
                    attachedPictureHeight[attachedPictureCount] * 4;
            //Extract RGB array from Bitmap
            ByteBuffer byteBuffer = ByteBuffer.allocate(pictureArraySize);
            attachedPictureBitmap.copyPixelsToBuffer(byteBuffer);
            attachedPictureBitmap = null;
            System.gc();
            attachedPictureArray[attachedPictureCount] = new byte[pictureArraySize];
            byteBuffer.rewind();
            byteBuffer.get(attachedPictureArray[attachedPictureCount]);
            byteBuffer = null;
            System.gc();

            return true;
        } catch (Exception e) {
            //Invalid bitmap data
            loggingclass.writelog("General Exception Error in 'extractPictureArray' " + e.getMessage(), null, true);
            return false;
        }
    }


    //Takes a directory for the data file, a data file name
    //Returns the text content of the file
    public static String readFile(String mDir, String mFileName) {
        String dataString = new String();
        String readString = new String();

        try {
            //First separate the header from the data fields (Headers are never compressed)
            //For this read the data file until we find the form information
            File fi = new File(Processor.HomePath + Processor.Dirprefix
                    + mDir + Processor.Separator + mFileName);
            FileInputStream fileISi = new FileInputStream(fi);
            BufferedReader buf = new BufferedReader(new InputStreamReader(fileISi));
            dataString = "";
            boolean found_form = false;
            //Handles both hard-coded forms and custom forms
            while ((readString = buf.readLine()) != null) {
                dataString += readString + "\n";
            }
        } catch (FileNotFoundException e) {
            AndFlmsg.middleToastText(AndFlmsg.myContext.getString(R.string.txt_FileNotFound) + ": " + mFileName);
        } catch (IOException e) {
            loggingclass.writelog("IO Exception Error in 'readFile' " + e.getMessage(), null, true);
        }
        return dataString;
    }


    // Creates an array of flmsg messages file names
    public static String[] createFileListFromFolder(String mFolder) {
        String[] fileNamesArray = null;
        try {
            // Get the list of files in the designated folder
            File dir = new File(Processor.HomePath + Processor.Dirprefix
                    + mFolder);
            File[] files = dir.listFiles();
            FileFilter fileFilter = new FileFilter() {
                public boolean accept(File file) {
                    return file.isFile();
                }
            };

            // We should now have an array of strings containing the file names
            files = dir.listFiles(fileFilter);
            if (files.length > 0) {
                fileNamesArray = new String[files.length];
                for (int i = 0; i < files.length; i++) {
                    fileNamesArray[i] = files[i].getName();
                }
            }
        } catch (Exception e) {
            loggingclass.writelog("Error when listing Folder: " + mFolder + "\nDetails: ", e, true);
        }
        return fileNamesArray;
    }


    public final static boolean SORTBYNAME = true;
    public final static boolean SORTBYDATEREVERSED = false;

    // Loads the list of Received flmsg messages
    public static void loadFileListFromFolder(String mFolder, boolean sortByFileName) {
        try {
            // Get the list of files in the designated folder
            File dir = new File(Processor.HomePath + Processor.Dirprefix
                    + mFolder);
            File[] files = dir.listFiles();
            FileFilter fileFilter = new FileFilter() {
                public boolean accept(File file) {
                    return file.isFile();
                }
            };

            // We should now have an array of strings containing the file names
            files = dir.listFiles(fileFilter);
            if (sortByFileName) {//Sort by name, ignoring case
                Arrays.sort(files, new Comparator<File>() {
                    public int compare(File fileA, File fileB) {
                        if (fileA != null && fileB != null) {
                            //Just in case we have directories
                            if (fileB.isDirectory() && (!fileA.isDirectory())) return 1;
                            if (fileA.isDirectory() && (!fileB.isDirectory())) return -1;
                            return fileA.getName().toLowerCase(Locale.US).
                                    compareTo(fileB.getName().toLowerCase(Locale.US));
                        }
                        return 0;
                    }
                });
            } else { //sort by date, reversed
                Arrays.sort(files, new Comparator<File>() {
                    public int compare(File fileA, File fileB) {
                        if (fileA != null && fileB != null) {
                            //Just in case we have directories
                            if (fileB.isDirectory() && (!fileA.isDirectory())) return +1;
                            if (fileA.isDirectory() && (!fileB.isDirectory())) return -1;
                            if (fileA.lastModified() == fileB.lastModified()) return 0;
                            return (fileA.lastModified() > fileB.lastModified() ? -1 : +1);
                        }
                        return 0;
                    }
                });
            }
            AndFlmsg.msgArrayAdapter.clear();
            for (int i = 0; i < files.length; i++) {
                AndFlmsg.msgArrayAdapter.add(files[i].getName());
            }
        } catch (Exception e) {
            loggingclass.writelog("Error when showing Folder: " + mFolder + "\nDetails: ", e, true);
        }
    }


    public static String getExtension(String myFormName, boolean isTemplate) {
        String msgExtension = ".k2s";
        String tempExtension = ".k2t";

        if (myFormName.toLowerCase(Locale.US).startsWith("radiogram.")) {
            msgExtension = ".m2s";
            tempExtension = ".m2t";
        } else if (myFormName.toLowerCase(Locale.US).startsWith("iaru.")) {
            msgExtension = ".i2s";
            tempExtension = ".i2t";
        } else if (myFormName.toLowerCase(Locale.US).startsWith("blankform.")) {
            msgExtension = ".b2s";
            tempExtension = ".b2t";
        } else if (myFormName.toLowerCase(Locale.US).startsWith("plaintext.")) {
            msgExtension = ".p2s";
            tempExtension = ".p2t";
        } else if (myFormName.toLowerCase(Locale.US).startsWith("ics203.")) {
            msgExtension = ".203";
            tempExtension = ".203T";
        } else if (myFormName.toLowerCase(Locale.US).startsWith("ics205.")) {
            msgExtension = ".205";
            tempExtension = ".205T";
        } else if (myFormName.toLowerCase(Locale.US).startsWith("ics205a.")) {
            msgExtension = ".25A";
            tempExtension = ".25AT";
        } else if (myFormName.toLowerCase(Locale.US).startsWith("ics206.")) {
            msgExtension = ".206";
            tempExtension = ".206T";
        } else if (myFormName.toLowerCase(Locale.US).startsWith("ics213.")) {
            msgExtension = ".213";
            tempExtension = ".213T";
        } else if (myFormName.toLowerCase(Locale.US).startsWith("ics214.")) {
            msgExtension = ".214";
            tempExtension = ".214T";
        } else if (myFormName.toLowerCase(Locale.US).startsWith("ics216.")) {
            msgExtension = ".216";
            tempExtension = ".216T";
        } else if (myFormName.toLowerCase(Locale.US).startsWith("ics309.")) {
            msgExtension = ".309";
            tempExtension = ".309T";
        } else if (myFormName.toLowerCase(Locale.US).startsWith("hics203.")) {
            msgExtension = ".H203";
            tempExtension = ".H203T";
        } else if (myFormName.toLowerCase(Locale.US).startsWith("hics206.")) {
            msgExtension = ".H206";
            tempExtension = ".H206T";
        } else if (myFormName.toLowerCase(Locale.US).startsWith("hics213.")) {
            msgExtension = ".H213";
            tempExtension = ".H213T";
        } else if (myFormName.toLowerCase(Locale.US).startsWith("hics214.")) {
            msgExtension = ".H214";
            tempExtension = ".H214T";
        } else if (myFormName.toLowerCase(Locale.US).startsWith("html_form.")) {
            msgExtension = ".html";
            tempExtension = ".html";
        } else if (myFormName.toLowerCase(Locale.US).startsWith("csvform.")) {
            msgExtension = ".c2s";
            tempExtension = ".c2t";
        }

        //Return the value
        if (isTemplate) {
            return tempExtension;
        } else {
            return msgExtension;
        }
    }


    public static boolean isCustomForm(String extension) {
        if (extension.equals(".k2s") || extension.equals(".k2t")) {
            return true;
        } else return false;
    }


    //Extract the field content of a Hard Coded form buffer
    //The field name is passed without ":"
    public static String getHcfFieldContent(String dataBuffer, String fieldName) {
        String fieldValue = "";

        int posStart = dataBuffer.indexOf(":" + fieldName + ":");
        if (posStart != -1) { //There is Match
            //Get length of field
            int posBlankAfterLength = dataBuffer.indexOf(" ", posStart + fieldName.length() + 1);
            int countStringlength = posBlankAfterLength - (posStart + fieldName.length() + 2);
            if (posBlankAfterLength != -1 && countStringlength > 0) {
                int length;
                try {
                    String lengthStr = dataBuffer.substring(posStart + fieldName.length() + 2, posBlankAfterLength);
                    length = Integer.parseInt(lengthStr);
                } catch (NumberFormatException e) {
                    length = 0;
                }
                //Extract value
                if (length > 0 && (dataBuffer.length() >= posBlankAfterLength + length + 1)) {
                    fieldValue = dataBuffer.substring(posBlankAfterLength + 1, length + posBlankAfterLength + 1);
                }
            }
        }
        return fieldValue;
    }


    //Set the field content of a Hard Coded form buffer
    public static String setHcfFieldContent(String dataBuffer, String fieldName, String newValue) {
        String newBuffer = "";

        int posStart = dataBuffer.indexOf(":" + fieldName + ":");
        if (posStart != -1) { //There is Match
            //Get length of field
            int posBlankAfterLength = dataBuffer.indexOf(" ", posStart + fieldName.length() + 1);
            int countStringlength = posBlankAfterLength - (posStart + fieldName.length() + 2);
            if (posBlankAfterLength != -1 && countStringlength > 0) {
                int length;
                try {
                    String lengthStr = dataBuffer.substring(posStart + fieldName.length() + 2, posBlankAfterLength);
                    length = Integer.parseInt(lengthStr);
                } catch (NumberFormatException e) {
                    length = 0;
                }
                //Build section to be replaced (field, length and value)
                String toBeReplaced = dataBuffer.substring(posStart, length + posBlankAfterLength + 1);
                //Build new replacement section. If newValue is blank, remove the whole entry
                String replacement = "";
                if (!newValue.equals("")) {
                    replacement = ":" + fieldName + ":" + Integer.toString(newValue.length()) + " " + newValue;
                }
                newBuffer = dataBuffer.replace(toBeReplaced, replacement);
            }
        } else { //add field since it does not exist in buffer (only if newValue is not blank)
            if (!newValue.equals("")) {
                newBuffer = dataBuffer + ":" + fieldName + ":" + Integer.toString(newValue.length()) + " " + newValue + "\n";
            }
        }
        return newBuffer;
    }


}
