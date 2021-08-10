/*
 *  Based on Flmsg source code version 2.0.4, unless noted otherwise.
 *
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
 *
 */


#include "AndFlmsg_Flmsg_Interface.h"
#include <jni.h>
#include "time_table.h"
#include <strings.h>
#include <ctype.h>
#include "wrap.h"
#include "custom.h"
#include "radiogram.h"
#include "status.h"

using std::string;

//***************** FROM C++ to JAVA ******************


//Dummy GUI stuff to get some Flmsg code working as-is
extern void select_form(int form) {

}


//Access to Java config class methods for accessing String preferences
extern string getPreferenceS2(string preferenceString, string defaultValue) {
	jobject returnedObject;
	jstring jprefStr;
	jstring jdefVal;

	//Check that we have valid data
	if (gEnv2 ==NULL) return NULL;
	//if (gJobject2 ==NULL) return NULL;

	//Find the Java class
	jclass cls = gEnv2->FindClass("com/AndFlmsg/config");

	//Find the static Java method (see signature)
	jmethodID mid = gEnv2->GetStaticMethodID(cls, "getPreferenceS", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");

	//Convert strings to jstrings
	jprefStr = gEnv2->NewStringUTF(preferenceString.c_str());
	jdefVal = gEnv2->NewStringUTF(defaultValue.c_str());
	//Call the method
	returnedObject = gEnv2->CallStaticObjectMethod(cls, mid, jprefStr, jdefVal);
	//Convert the returned jstring to C string
	const char* str = gEnv2->GetStringUTFChars((jstring) returnedObject, NULL);

	//Release the intermediate variable
	//gEnv2->DeleteLocalRef(returnedObject);
	//gEnv2->DeleteLocalRef(jprefStr);
	//gEnv2->DeleteLocalRef(jdefVal);

	//Return the result...that simple...phew
	return str;
}


//Access to Java config class methods for accessing Boolean preferences
extern bool getPreferenceB2(string preferenceString, bool defaultValue) {
	jboolean returnedValue;
	jstring jprefStr;

	//Check that we have valid data
	if (gEnv2 == NULL) return false;
	//if (gJobject2 == NULL) return false;

	//Find the Java class
	jclass cls = gEnv2->FindClass("com/AndFlmsg/config");

	//Find the static Java method (see signature)
	jmethodID mid = gEnv2->GetStaticMethodID(cls, "getPreferenceB", "(Ljava/lang/String;Z)Z");

	//Convert strings to jstrings
	jprefStr = gEnv2->NewStringUTF(preferenceString.c_str());
	jboolean jDefault = defaultValue;
	//Call the method
	returnedValue = gEnv2->CallStaticBooleanMethod(cls, mid, jprefStr, jDefault);

	//Release the intermediate variable
	//gEnv2->DeleteLocalRef(jprefStr);

	//Return the result
	return returnedValue;
}


//Access to Java config class methods for accessing Int preferences
extern double getPreferenceD2(string preferenceString, double defaultValue) {
	jdouble returnedValue;
	jstring jprefStr;

	//Check that we have valid data
	if (gEnv2 == NULL) return 0.0f;
	//if (gJobject2 == NULL) return 0.0f;

	//Find the Java class
	jclass cls = gEnv2->FindClass("com/AndFlmsg/config");

	//Find the static Java method (see signature)
	jmethodID mid = gEnv2->GetStaticMethodID(cls, "getPreferenceD", "(Ljava/lang/String;D)D");

	//Convert strings to jstrings
	jprefStr = gEnv2->NewStringUTF(preferenceString.c_str());
	jdouble jDefault = defaultValue;
	//Call the method
	returnedValue = gEnv2->CallStaticDoubleMethod(cls, mid, jprefStr, jDefault);

	//Release the intermediate variable
	gEnv2->DeleteLocalRef(jprefStr);

	//Return the result
	return returnedValue;
}


//Access to Java config class methods for accessing Int preferences
extern int getPreferenceI2(string preferenceString, int defaultValue) {
	jint returnedValue;
	jstring jprefStr;

	//Check that we have valid data
	if (gEnv2 == NULL) return 0;
	//if (gJobject2 == NULL) return 0;

	//Find the Java class
	jclass cls = gEnv2->FindClass("com/AndFlmsg/config");

	//Find the static Java method (see signature)
	jmethodID mid = gEnv2->GetStaticMethodID(cls, "getPreferenceI", "(Ljava/lang/String;I)I");

	//Convert strings to jstrings
	jprefStr = gEnv2->NewStringUTF(preferenceString.c_str());
	jint jDefault = defaultValue;
	//Call the method
	returnedValue = gEnv2->CallStaticIntMethod(cls, mid, jprefStr, jDefault);

	//Release the intermediate variable
	//gEnv2->DeleteLocalRef(jprefStr);

	//Return the result
	return returnedValue;
}


//Access to Java config class methods for updating a preference (e.g. for serial number update)
extern string setPreferenceS2(string preferenceString, string newValue) {
	jobject returnedObject;
	jstring jprefStr;
	jstring jnewVal;

	//Check that we have valid data
	if (gEnv2 ==NULL) return NULL;
	//if (gJobject2 ==NULL) return NULL;

	//Find the Java class
	jclass cls = gEnv2->FindClass("com/AndFlmsg/config");

	//Find the static Java method (see signature)
	jmethodID mid = gEnv2->GetStaticMethodID(cls, "setPreferenceS", "(Ljava/lang/String;Ljava/lang/String;)Z");

	//Convert strings to jstrings
	jprefStr = gEnv2->NewStringUTF(preferenceString.c_str());
	jnewVal = gEnv2->NewStringUTF(newValue.c_str());
	//Call the method
	returnedObject = gEnv2->CallStaticObjectMethod(cls, mid, jprefStr, jnewVal);
	//Convert the returned jstring to C string
	const char* str = gEnv2->GetStringUTFChars((jstring) returnedObject, NULL);

	//Release the intermediate variable
	//gEnv2->DeleteLocalRef(returnedObject);
	//gEnv2->DeleteLocalRef(jprefStr);
	//gEnv2->DeleteLocalRef(jnewVal);

	return str;
}



//***************** FROM JAVA to C++  ******************


//Save environment if we need to call the preference methods in Java
extern "C" JNIEXPORT void
Java_com_AndFlmsg_Message_saveEnv( JNIEnv* env, jobject thishere)
{
	gEnv2 = env;
	gJobject2 = thishere;
}


//Processes raw received wrap file (From [wrap:begin] to [wrap:end])
extern "C" JNIEXPORT jboolean
Java_com_AndFlmsg_Message_ProcessWrapBuffer( JNIEnv* env,
		jobject thishere, jstring myJBuffer)
{
	jboolean decodeResult;
	//Convert to C++ type
	const char *cBuffer = NULL;
	//string cBuffer;
	cBuffer = env->GetStringUTFChars(myJBuffer, NULL);
	if (cBuffer == NULL) return false;
	decodeResult = wrap_import(cBuffer);
	return decodeResult;
}



//Get the text contained within the wrap sequence ("the unwrapped text")
extern "C" JNIEXPORT jstring
Java_com_AndFlmsg_Message_getUnwrapText( JNIEnv* env,
		jobject thishere)
{

	return env->NewStringUTF(unwrapText.c_str());

}


//Get the text contained within the wrap sequence ("the unwrapped text")
extern "C" JNIEXPORT jstring
Java_com_AndFlmsg_Message_getUnwrapFilename( JNIEnv* env,
		jobject thishere)
{

	return env->NewStringUTF(unwrapFilename.c_str());

}


//Get the text contained within the wrap sequence ("the unwrapped text")
extern "C" JNIEXPORT jstring
Java_com_AndFlmsg_Message_geterrtext( JNIEnv* env,
		jobject thishere)
{

	return env->NewStringUTF(errtext.c_str());

}


//Processes HTML form and custom content, returns formatted Html display string
//With Read-Only attribute and values set in the VALUE attribute in the form (can be safely sent via email)
extern "C" JNIEXPORT jstring
Java_com_AndFlmsg_Message_customHtmlDisplayFormat( JNIEnv* env,
		jobject thishere, jstring myHtmlForm, jstring myCustomField)
{
	//bool decodeResult;
	//Convert to C++ type
	const char *cHtmlForm = NULL;
	cHtmlForm = env->GetStringUTFChars(myHtmlForm, NULL);
	const char *cCustomField = NULL;
	cCustomField = env->GetStringUTFChars(myCustomField, NULL);
	html_form.assign(cHtmlForm);
	//Update the table with values in the custom field
	custom_field.assign(cCustomField);
	update_customform();
	//For each field, update the VALUE attribute and set them as Read-Only
	//assign_values(html_form);
	custom_viewer();
	return env->NewStringUTF(html_view.c_str());
//	return env->NewStringUTF(html_form.c_str());
//	return env->NewStringUTF(edit_txt.c_str());
//	return env->NewStringUTF(debug_str.c_str());
}



//Processes HTML form and custom content, returns formatted Html display string
//values set in the VALUE attribute in the form ready for editing in web browser
extern "C" JNIEXPORT jstring
Java_com_AndFlmsg_Message_customHtmlEditingFormat( JNIEnv* env,
		jobject thishere, jstring myHtmlForm, jstring myCustomField)
{
	//bool decodeResult;
	//Convert to C++ type
	const char *cHtmlForm = NULL;
	cHtmlForm = env->GetStringUTFChars(myHtmlForm, NULL);
	const char *cCustomField = NULL;
	cCustomField = env->GetStringUTFChars(myCustomField, NULL);
	html_form.assign(cHtmlForm);
	//Update the table with values in the custom field
	custom_field.assign(cCustomField);
	update_customform();
	//Assign values to the HTML document for editing
	assign_values(html_form);
	//For each field, update the VALUE attribute and set them as Read-Only
	//assign_values(html_form);
	//custom_viewer();
//	return env->NewStringUTF(html_view.c_str());
	return env->NewStringUTF(html_form.c_str());
//	return env->NewStringUTF(edit_txt.c_str());
//	return env->NewStringUTF(debug_str.c_str());
}



//Processes HTML form and custom content, returns formatted Html display string
extern "C" JNIEXPORT
jstring Java_com_AndFlmsg_Message_customHtmlEditFormat( JNIEnv* env,
		jobject thishere, jstring myHtmlForm, jstring myCustomField)
{
	//Convert to C++ type
	const char *cHtmlForm = NULL;
	cHtmlForm = env->GetStringUTFChars(myHtmlForm, NULL);
	const char *cCustomField = NULL;
	cCustomField = env->GetStringUTFChars(myCustomField, NULL);
	html_form.assign(cHtmlForm);
	//Update the table with values in the custom field
	custom_field.assign(cCustomField);
	update_customform();
	//For each field, update the VALUE attribute
	assign_values(html_form);
	return env->NewStringUTF(html_form.c_str());
}


//Creates the customBuffer for storing as data file based on value of myCustomField and the Header Reason
extern "C" JNIEXPORT
jstring Java_com_AndFlmsg_Message_createCustomBuffer( JNIEnv* env,
		jobject thishere, jstring myHeader, jint reason, jstring myCustomField)
{
	//Convert to C++ type
	const char *cCustomField = NULL;
	cCustomField = env->GetStringUTFChars(myCustomField, NULL);
	const char *cMyHeader = NULL;
	cMyHeader = env->GetStringUTFChars(myHeader, NULL);
	//Update the table with values in the custom field
	custom_field.assign(cCustomField);
	createCustomBuffer(cMyHeader, reason);
	return env->NewStringUTF(custombuffer.c_str());
}


//Creates the hard-coded form header (up to first field) for storing as data file based on value of myCustomField and the Header Reason
extern "C" JNIEXPORT
jstring Java_com_AndFlmsg_Message_createHardCodedHeader( JNIEnv* env,
		jobject thishere, jstring myHeader, jint reason, jstring myFormName)
{
	string myNewHeader2;

	//Convert to C++ type
	string cMyHeader;
	cMyHeader = env->GetStringUTFChars(myHeader, NULL);
	string cMyForm;
	cMyForm = env->GetStringUTFChars(myFormName, NULL);

	if (cMyForm.empty()) return env->NewStringUTF("");
	//Init the header with existing data if any
	read_header(cMyHeader);
	//Update based on reason
	update_header((hdr_reason) reason);
	//Create header
	string myNewForm;
	myNewForm.assign("<" + cMyForm + ">");
	myNewHeader2.assign(header(myNewForm.c_str()));
	return env->NewStringUTF(myNewHeader2.c_str());
}


//Updates the header for TXing (mainly)
extern "C" JNIEXPORT
jstring Java_com_AndFlmsg_Message_updateHeader( JNIEnv* env,
		jobject thishere, jstring myCurrentHeader, jint reason, jstring headerForm)
{
	//Convert to C++ type
	string cMyCurrentHeader;
	cMyCurrentHeader.assign(env->GetStringUTFChars(myCurrentHeader, NULL));
	string cHeaderForm;
	cHeaderForm.assign(env->GetStringUTFChars(headerForm, NULL));
	//Init the header with existing data is any
	read_header(cMyCurrentHeader);
	//Update based on reason
	update_header((hdr_reason) reason);
	//Create header
	string newHeader = header(cHeaderForm.c_str());
	return env->NewStringUTF(newHeader.c_str());
}


//Creates the flmsg data file name based on options/preferences
extern "C" JNIEXPORT jstring Java_com_AndFlmsg_Message_namedFile( JNIEnv* env,
		jobject thishere)
{
	char *cNamedFile = NULL;
	cNamedFile = named_file();
	return env->NewStringUTF(cNamedFile);
//debug	return env->NewStringUTF("newfilename.err");
}



//Creates the flmsg data file name based on options/preferences
extern "C" JNIEXPORT jstring Java_com_AndFlmsg_Message_compressMaybe(JNIEnv* env,
		jobject thishere, jstring jBuffer)
{
	//Convert to C++ type
//	const char *cBuffer = NULL;
	string cBuffer;
	cBuffer = env->GetStringUTFChars(jBuffer, NULL);
	compress_maybe(cBuffer, false);
	return env->NewStringUTF(cBuffer.c_str());
}



extern "C" JNIEXPORT jstring Java_com_AndFlmsg_Message_estimate(JNIEnv* env,
		jobject thishere, jstring jModeName, jstring jBufferString) {
	static char sz_xfr_size[30];
	float xfr_time = 0, overhead;

	//Convert to C++ format
	string cBufferString;
	cBufferString = env->GetStringUTFChars(jBufferString, NULL);
	const char *cModeName = NULL;
	cModeName = env->GetStringUTFChars(jModeName, NULL);

	//Android: Calculated
	int transfer_size = cBufferString.length();
	if (transfer_size == 0) {
		strcpy(sz_xfr_size, "");
	} else {

		xfr_time = seconds_from_c_string(cModeName, cBufferString.c_str(), transfer_size, &overhead);
		float tot_xfr_time = xfr_time + overhead;

		if (((int)xfr_time) == 0)
			snprintf(sz_xfr_size, sizeof(sz_xfr_size), "%d bytes / ? tx time", transfer_size);
		else if (xfr_time < 60)
			snprintf(sz_xfr_size, sizeof(sz_xfr_size), "%d bytes / %d secs",
					transfer_size, (int)(tot_xfr_time + 0.5));
		else
			snprintf(sz_xfr_size, sizeof(sz_xfr_size), "%d bytes / %d m %d s",
					transfer_size,
					(int)(xfr_time / 60), ((int)tot_xfr_time) % 60);
	}
	return  env->NewStringUTF(sz_xfr_size);
}



extern "C" JNIEXPORT jstring Java_com_AndFlmsg_Message_dateStamp(JNIEnv* env,
		jobject thishere) {

	//Save the environment for preference access
	gEnv2 = env;
	gJobject2 = thishere;

	progStatus.dtformat	= getPreferenceI2("DTFORMAT", 0);
	progStatus.UTC		= getPreferenceI2("UTC", 0);

	static string dt;
	dt.assign(szDate());

	return  env->NewStringUTF(dt.c_str());
}


extern "C" JNIEXPORT jstring Java_com_AndFlmsg_Message_timeStamp(JNIEnv* env,
		jobject thishere) {

	//Save the environment for preference access
	gEnv2 = env;
	gJobject2 = thishere;

	progStatus.UTC	= getPreferenceI2("UTC", 0);

	static string dt;
	dt.assign(szTime(progStatus.UTC));

	return  env->NewStringUTF(dt.c_str());
}



extern "C" JNIEXPORT jstring Java_com_AndFlmsg_Message_dateTimeStamp(JNIEnv* env,
		jobject thishere) {

	//Save the environment for preference access
	gEnv2 = env;
	gJobject2 = thishere;

	progStatus.dtformat				= getPreferenceI2("DTFORMAT", 0);
	progStatus.UTC					= getPreferenceI2("UTC", 0);

/*	static string dt;
	dt.assign(szDate()).append(" @ ").append(szTime(6));
	return  env->NewStringUTF(dt.c_str());
*/
	static char szDt[80];
	time_t tmptr;
	tm sTime;
	time (&tmptr);
	if (progStatus.UTC) {
		gmtime_r (&tmptr, &sTime);
		strftime(szDt, 79, "%d%H%MZ %b %Y", &sTime);
	} else {
		localtime_r(&tmptr, &sTime);
		strftime(szDt, 79, "%d%H%ML %b %Y", &sTime);
	}

	for (size_t i = 0; i < strlen(szDt); i++) szDt[i] = toupper(szDt[i]);

	return env->NewStringUTF(szDt);


}



//Returns an array of ARL Messages Numbers (in words) (uses the arl_list in arl_msgs.cxx)
extern "C" JNIEXPORT jobjectArray
Java_com_AndFlmsg_Message_getArlMsgTextList(JNIEnv* env,
		jobject thishere)
{
	char *modeCapListString[MAXARLMSGS];
	jobjectArray returnedArray;
	int i;
	int j = 0;

	returnedArray = (jobjectArray)env->NewObjectArray(MAXARLMSGS,
			env->FindClass("java/lang/String"),
			env->NewStringUTF(""));
	int num = sizeof(arl_list) / sizeof(ARL_TEXT);
	for (i=0; i < num; i++) {
		if (arl_list[i].sznbr) {
			string temp = "";
			temp.assign(arl_list[i].sznbr).append(": ").append(arl_list[i].text);
			env->SetObjectArrayElement(returnedArray,i,env->NewStringUTF(temp.c_str()));
		} else {
			env->SetObjectArrayElement(returnedArray,i,env->NewStringUTF(""));
		}
	}

	return(returnedArray);

}


//Returns an array of ARL delivery (HX) codes and text  (uses the hx_list in hx_msgs.cxx)
extern "C" JNIEXPORT jobjectArray
Java_com_AndFlmsg_Message_getArlHxTextList(JNIEnv* env,
		jobject thishere)
{
	char *hxCodesString[MAXHXMSGS];
	jobjectArray returnedArray;
	int i;
	int j = 0;

	returnedArray = (jobjectArray)env->NewObjectArray(MAXHXMSGS,
			env->FindClass("java/lang/String"),
			env->NewStringUTF(""));
	int num = sizeof(hx_list) / sizeof(hx_TEXT);
	for (i=0; i < num; i++) {
		if (hx_list[i].sznbr) {
			string temp = "";
			temp.assign(hx_list[i].text).append(": ").append(hx_list[i].instruct);
			env->SetObjectArrayElement(returnedArray,i,env->NewStringUTF(temp.c_str()));
		} else {
			env->SetObjectArrayElement(returnedArray,i,env->NewStringUTF(""));
		}
	}

	return(returnedArray);

}



extern "C" JNIEXPORT jstring Java_com_AndFlmsg_Message_cbrgcheck(JNIEnv* env,
		jobject thishere, jstring msgFieldContent, jboolean stdFormat) {

	//Convert to C++ format
	string cMsgString = env->GetStringUTFChars(msgFieldContent, NULL);
	//Format msg field and count words for check field
	unescape(cMsgString); //If we called the method more than once, the field will be "escaped" for display in HTML
	//Now process and return the check field, plus update the txt_rg_msg global variable
	string cCheckField = cb_rg_check(cMsgString, stdFormat);
	//Prepare txt_rg_msg for display in HTML (used in next method down)
	escape(txt_rg_msg);

	return  env->NewStringUTF(cCheckField.c_str());
}



//Get the text of the reformatted ARL msg field
extern "C" JNIEXPORT jstring
Java_com_AndFlmsg_Message_gettxtrgmsg( JNIEnv* env,
		jobject thishere)
{

	return env->NewStringUTF(txt_rg_msg.c_str());

}


//Call the escape routine (from data format to HTML format)
extern "C" JNIEXPORT jstring Java_com_AndFlmsg_Message_escape(JNIEnv* env,
		jobject thishere, jstring myString) {

	//Convert to C++ format
	string mycString = env->GetStringUTFChars(myString, NULL);
	//Format msg field and count words for check field
	escape(mycString);

	return  env->NewStringUTF(mycString.c_str());
}


//Call the unescape routine (from HTML format to data format)
extern "C" JNIEXPORT jstring Java_com_AndFlmsg_Message_unescape(JNIEnv* env,
		jobject thishere, jstring myString) {

	//Convert to C++ format
	string mycString = env->GetStringUTFChars(myString, NULL);
	//Format msg field and count words for check field
	unescape(mycString);

	return  env->NewStringUTF(mycString.c_str());
}


//Expands the ARL messages for form display
extern "C" JNIEXPORT jstring Java_com_AndFlmsg_Message_expandarl(JNIEnv* env,
		jobject thishere, jstring myArlMsg) {

	//Convert to C++ format
	string mycArlMsg = env->GetStringUTFChars(myArlMsg, NULL);
	//Format msg field and count words for check field
	string returnStr = expand_arl(mycArlMsg);

	return  env->NewStringUTF(returnStr.c_str());
}


