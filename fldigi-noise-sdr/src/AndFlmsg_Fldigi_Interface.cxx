/*
 *  Based on Fldigi source code version 3.21.82
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
#include "AndFlmsg_Fldigi_Interface.h"
//#include "modem.h"
#include "psk.h"
#include "mt63.h"
#include "thor.h"
#include "mfsk.h"
#include "olivia.h"
#include "contestia.h"
#include "dominoex.h"
#include "rsid.h"
#include <jni.h>
#include <time.h>
#include <complex.h>
#include <math.h>
#define BILLION 1000000000L
#define BUFSIZE 600000
#define EDGESIZE 32000000
#define OVERSAMPLING 1

#include <android/log.h>
#define LOG_TAG "rfpwm"
#define ALOG(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

using std::string;

cRsId *RsidModem = NULL;
modem *active_modem = NULL;
char decodedBuffer[5000];
char utf8PartialBuffer[4];
int utfExpectedChars = 0;
int utfFoundChars = 0;
int lastCharPos;
JNIEnv* gEnv = NULL;
jobject gJobject = NULL;
//Separate variable for Tx thread as Env are thread sensitive
//JNIEnv* txEnv = NULL;
//For Tx buffer handling
signed char* txDataBuffer = NULL;
int txDataBufferLength = 0;
int txCounter = 0;
//MFSK Picture Rx/Tx
int pictureRow[4096 * 3];
int pictureW = 0;
int pictureH = 0;

//VK2ETA Needs to be made bullet proof
double outputBuffer[BUFSIZE]; // 3 seconds of audio, in case we have a delay in
                             // the Java rx thread
int sizeOfOutputBuffer = sizeof(outputBuffer) / sizeof(int);
int outputBufferIndex = 0;

double outputBufferResampled[BUFSIZE*OVERSAMPLING];
int outputBufferResampledIndex = 0;

cmplx c[BUFSIZE*OVERSAMPLING];

//***************** FROM C++ to JAVA ******************

//Get the value of Java boolean static variable newAmplReady of the Java Modem class
extern bool getNewAmplReady() {
		jclass myClass = NULL;
		myClass = gEnv->FindClass("com/AndFlmsg/Modem");
		jfieldID myFid = NULL;
		//Find Java field newAmplReady of type boolean
		myFid = gEnv->GetStaticFieldID(myClass,"newAmplReady","Z");
	bool newAmplReady = gEnv->GetStaticBooleanField(myClass, myFid);
	return newAmplReady;
return true;
}

//Update the waterfall array of amplitudes in Java class Modem
extern void updateWaterfallBuffer(double *aFFTAmpl) {
	//Find Java Modem class
	jclass cls = gEnv->FindClass("com/AndFlmsg/Modem");
	//Find Static Java method updateWaterfall (argument is an array of double and returns Void)
	jmethodID mid = gEnv->GetStaticMethodID(cls, "updateWaterfall", "([D)V");
	//Create a Java array and copy the C++ array into it
	jdoubleArray jaFFTAmpl = gEnv->NewDoubleArray(RSID_FFT_SIZE);
	gEnv->SetDoubleArrayRegion(jaFFTAmpl, 0, RSID_FFT_SIZE, aFFTAmpl);
	//Call the method with the Java array
	gEnv->CallStaticVoidMethod(cls, mid, jaFFTAmpl);
	//Release the intermediate array.
	gEnv->DeleteLocalRef(jaFFTAmpl);
}


//Access to Java config class methods for accessing String preferences
extern string getPreferenceS(string preferenceString, string defaultValue) {

	jobject returnedObject;
	jstring jprefStr;
	jstring jdefVal;

	//Check that we have valid data
	if (gEnv ==NULL) return NULL;
	if (gJobject ==NULL) return NULL;

	//Find the Java class
	jclass cls = gEnv->FindClass("com/AndFlmsg/config");

	//Find the static Java method (see signature)
	jmethodID mid = gEnv->GetStaticMethodID(cls, "getPreferenceS", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");

	//Convert strings to jstrings
	jprefStr = gEnv->NewStringUTF(preferenceString.c_str());
	jdefVal = gEnv->NewStringUTF(defaultValue.c_str());
	//Call the method
	returnedObject = gEnv->CallStaticObjectMethod(cls, mid, jprefStr, jdefVal);
	//Convert the returned jstring to C string
	const char* str = gEnv->GetStringUTFChars((jstring) returnedObject, NULL);

	//Release the intermediate variable
	gEnv->DeleteLocalRef(returnedObject);
	gEnv->DeleteLocalRef(jprefStr);
	gEnv->DeleteLocalRef(jdefVal);

	//Return the result...that simple...phew
	return str;
}


//Access to Java config class methods for accessing Boolean preferences
extern bool getPreferenceB(string preferenceString, bool defaultValue) {
	jboolean returnedValue;
	jstring jprefStr;

	//Check that we have valid data
	if (gEnv == NULL) return false;
	if (gJobject == NULL) return false;

	//Find the Java class
	jclass cls = gEnv->FindClass("com/AndFlmsg/config");

	//Find the static Java method (see signature)
	jmethodID mid = gEnv->GetStaticMethodID(cls, "getPreferenceB", "(Ljava/lang/String;Z)Z");

	//Convert strings to jstrings
	jprefStr = gEnv->NewStringUTF(preferenceString.c_str());
	jboolean jDefault = defaultValue;
	//Call the method
	returnedValue = gEnv->CallStaticBooleanMethod(cls, mid, jprefStr, jDefault);

	//Release the intermediate variable
	gEnv->DeleteLocalRef(jprefStr);

	//Return the result
	return returnedValue;
}


//Access to Java config class methods for accessing Double preferences
extern double getPreferenceD(string preferenceString, double defaultValue) {
	jdouble returnedValue;
	jstring jprefStr;

	//Check that we have valid data
	if (gEnv == NULL) return 0.0f;
	if (gJobject == NULL) return 0.0f;

	//Find the Java class
	jclass cls = gEnv->FindClass("com/AndFlmsg/config");

	//Find the static Java method (see signature)
	jmethodID mid = gEnv->GetStaticMethodID(cls, "getPreferenceD", "(Ljava/lang/String;D)D");

	//Convert strings to jstrings
	jprefStr = gEnv->NewStringUTF(preferenceString.c_str());
	jdouble jDefault = defaultValue;
	//Call the method
	returnedValue = gEnv->CallStaticDoubleMethod(cls, mid, jprefStr, jDefault);

	//Release the intermediate variable
	gEnv->DeleteLocalRef(jprefStr);

	//Return the result
	return returnedValue;
}


//Access to Java config class methods for accessing Int preferences
extern int getPreferenceI(string preferenceString, int defaultValue) {
	jint returnedValue;
	jstring jprefStr;

	//Check that we have valid data
	if (gEnv == NULL) return 0;
	if (gJobject == NULL) return 0;

	//Find the Java class
	jclass cls = gEnv->FindClass("com/AndFlmsg/config");

	//Find the static Java method (see signature)
	jmethodID mid = gEnv->GetStaticMethodID(cls, "getPreferenceI", "(Ljava/lang/String;I)I");

	//Convert strings to jstrings
	jprefStr = gEnv->NewStringUTF(preferenceString.c_str());
	jint jDefault = defaultValue;
	//Call the method
	returnedValue = gEnv->CallStaticIntMethod(cls, mid, jprefStr, jDefault);

	//Release the intermediate variable
	gEnv->DeleteLocalRef(jprefStr);

	//Return the result
	return returnedValue;
}

//RX Section of the C++ modems
extern void put_rx_char(int receivedChar) {
	//Only send complete UTF-8 sequences to Java
	//Behaviour on invalid combinations: discard and re-sync only on valid characters to
	//  avoid exceptions in upstream methods
	//decodedBuffer[lastCharPos++] = receivedChar & 0x7f;
	if (utfExpectedChars < 1) { //Normally zero
		//We are not in a multi-byte sequence yet
		if ((receivedChar & 0xE0) == 0xC0) {
			utfExpectedChars = 1; //One extra characters
			utfFoundChars = 0;
			utf8PartialBuffer[utfFoundChars++] = receivedChar;
		} else if ((receivedChar & 0xF0) == 0xE0) {
			utfExpectedChars = 2; //Two extra characters
			utfFoundChars = 0;
			utf8PartialBuffer[utfFoundChars++] = receivedChar;
		} else if ((receivedChar & 0xF8) == 0xF0) {
			utfExpectedChars = 3; //Three extra characters
			utfFoundChars = 0;
			utf8PartialBuffer[utfFoundChars++] = receivedChar;
		} else if ((receivedChar & 0xC0) == 0x80) { //Is it a follow-on character?
			//Should not be there (missing first character in sequence), discard and reset just in case
			utfExpectedChars = utfFoundChars = 0;
		} else if ((receivedChar & 0x80) == 0x00){ //Could be a single Chareacter, check it is legal
			decodedBuffer[lastCharPos++] = receivedChar;
			utfExpectedChars = utfFoundChars = 0; //Reset to zero in case counter is negative (just in case)
		} else { //Not a legal case, ignore and reset counter
			utfExpectedChars = utfFoundChars = 0; //Reset to zero in case counter is negative (just in case)
		}
	} else { //we are still expecting follow-up UTF-8 characters
		if ((receivedChar & 0xC0) == 0x80) { //Valid follow-on character, store it
			utfExpectedChars--;
			utf8PartialBuffer[utfFoundChars++] = receivedChar;
		} else { //Invalid sequence, discard it and start from scratch
			utfExpectedChars = utfFoundChars = 0;
		}
		//If we have a complete sequence, add to receive buffer
		if (utfExpectedChars < 1 && utfFoundChars > 0) {
			for (int i = 0; i < utfFoundChars; i++) {
				decodedBuffer[lastCharPos++] = utf8PartialBuffer[i];
			}
			utfExpectedChars = utfFoundChars = 0; //Reset

		}
	}
/* Code for Debugging
 * 		decodedBuffer[lastCharPos++] = '(';
		decodedBuffer[lastCharPos++] = 'U';
		decodedBuffer[lastCharPos++] = ')';
		//debug
		 	 	 char hexstr[10];
			 	 int cvlen = sprintf(hexstr, "%0004x", utf8PartialBuffer[i]);
				decodedBuffer[lastCharPos++] = '[';
				for (int i = 0; i < cvlen; i++) {
					decodedBuffer[lastCharPos++] = hexstr[i];
				}
				decodedBuffer[lastCharPos++] = ']';
 *
 */

}



//Call the Java class that echoes the transmitted characters
extern void put_echo_char(unsigned int txedChar) {
    printf("Transmit %c\n", txedChar);
//Do nothing for now
	//Find Java Modem class
	//	jclass cls = gEnv->FindClass("com/AndFlmsg/Modem");
	//Find Static Java method updateWaterfall (argument is an array of double and returns Void)
	//	jmethodID mid = gEnv->GetStaticMethodID(cls, "putEchoChar", "(I)V");
	//Call the method
	//	gEnv->CallStaticVoidMethod(cls, mid, (txedChar & 0x7F));
}


//Prepare a new Picture receiving file and display screen for MFSK picture Rx
void androidShowRxViewer(int picW, int picH) {
	//Save picture size for later
	pictureW = picW;
	pictureH = picH;
	//Find the Java class
	jclass cls = gEnv->FindClass("com/AndFlmsg/Modem");
	//Find the static Java method (see signature)
	jmethodID mid = gEnv->GetStaticMethodID(cls, "showRxViewer", "(II)V");
	//Call the method with the parameters
	gEnv->CallStaticVoidMethod(cls, mid, picW, picH);
	//Release the variables
	gEnv->DeleteLocalRef(cls);
}


//Save the last Picture to file (MFSK picture Rx)
void androidSaveLastPicture() {
	//Find the Java class
	jclass cls = gEnv->FindClass("com/AndFlmsg/Modem");
	//Find the static Java method (see signature)
	jmethodID mid = gEnv->GetStaticMethodID(cls, "saveLastPicture", "()V");
	//Call the method with the parameters
	gEnv->CallStaticVoidMethod(cls, mid);
	//Release the variables
	gEnv->DeleteLocalRef(cls);

}


//Update one datum of Current Rx picture(MFSK picture Rx)
void androidUpdateRxPic(int data, int pixelNumber) {

	if (pixelNumber < 4096 * 3) {
		pictureRow[pixelNumber] = data;
	}
	if (pixelNumber == 3 * pictureW - 1) { //Full row of bytes (3 per pixels), update bitmap.
		//Find the Java class
		jclass cls = gEnv->FindClass("com/AndFlmsg/Modem");
		//Find the static Java method (see signature)
		jmethodID mid = gEnv->GetStaticMethodID(cls, "updatePicture", "([II)V");
		//Create a Java array and copy the C++ array into it
		jintArray jPictureRow = gEnv->NewIntArray(pictureW * 3);
		gEnv->SetIntArrayRegion(jPictureRow, 0, pictureW * 3, pictureRow);
		//Call the method with the parameters
		gEnv->CallStaticVoidMethod(cls, mid, jPictureRow, pictureW);
		//Release the variables
		gEnv->DeleteLocalRef(cls);
		gEnv->DeleteLocalRef(jPictureRow);
	}

}



//----- TX section of the C++ modems ----

//Get one character from the input buffer
extern int get_tx_char() {
	int returnValue;

	returnValue = GET_TX_CHAR_ETX;
	if (txDataBuffer != NULL) {
		if (txCounter < txDataBufferLength) {
			returnValue = (int) (*txDataBuffer++);
			txCounter++;
		}
	}

	return returnValue;
}

// Timer function
static inline uint64_t get_ns(void) {
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  return BILLION * (uint64_t)t.tv_sec + (uint64_t)t.tv_nsec;
}

// Access the DRAM by LDR + CIVAC
__attribute__((naked)) void hammer_civac(uint64_t *addr) {
  __asm volatile("LDR X9, [X0] \r\n");
  __asm volatile("DC CIVAC, X0 \r\n");
  __asm volatile("DSB 0xB \r\n");
  __asm volatile("RET");
}

// save raw samples into a buffer
void txModulate(double *buffer, int len){
  // Accumulate samples
  
  bool full = false;
  int i = 0;
  while(i<len && !full){
      if(outputBufferIndex > BUFSIZE - 1){
          full = true;
          printf("Output buffer full, skipping samples\n");
      } else {
          outputBuffer[outputBufferIndex++] = buffer[i++];
      }
  }
  printf("Output buffer filled at %d out of %d\n", outputBufferIndex, BUFSIZE);
}

double phaseacc = 0;
cmplx mixer(cmplx in, double f, int samplerate)
{
	cmplx z;

	z = in * cmplx( cos(phaseacc), sin(phaseacc) );

	phaseacc -= TWOPI * f / samplerate;
	if (phaseacc < 0) phaseacc += TWOPI;

	return z;
}

void RFPWM(bool forever){
  printf("Starting coversion to RF-PMM\n");
  static uint64_t edges[EDGESIZE] = {0};
  int len = 0;
  int i = 0;
  int samplerate = active_modem->get_samplerate();
  int samplerate_rfpwm = samplerate*OVERSAMPLING;
  printf("samplerate %d, samplerate_rfpwm %d\n", samplerate, samplerate_rfpwm);
  
  printf("Starting resampling with interpolation\n");
  outputBufferResampledIndex = outputBufferIndex * OVERSAMPLING;
  for(int i=0; i < outputBufferIndex-1; i++){
      for(int j=0; j < OVERSAMPLING; j++){
          //outputBufferResampled[i*OVERSAMPLING+j] = j*(outputBuffer[i+1]-outputBuffer[i])/OVERSAMPLING;
          outputBufferResampled[i*OVERSAMPLING+j] = outputBuffer[i];
      }
  }

  //printf("Starting float to complex conversion\n");
  //C_FIR_filter*xmtfilt;
  //xmtfilt = new C_FIR_filter();
  //xmtfilt->init_lowpass (127, 1, 500/samplerate);
  //phaseacc = 0;
  //for(int i=0; i<outputBufferIndex; i++){
      //c[i].re = outputBuffer[i];
      //c[i].im = 0;
      //c[i] = mixer(c[i], -1000, samplerate);
      //xmtfilt->Irun(c[i].re, c[i].re);
      //xmtfilt->Irun(abs(outputBuffer[i]), outputBuffer[i]);
      //printf("(%e+%ej)\n", outputBuffer[i], 0);
      //printf("(%e+%ej)\n", c[i].re, c[i].im);
      //a = c.mag();
      //a = asin(a)/M_PI;
      //phi = 0; //c.arg();
      //outputBufferResampled[i] = cos(2*M_PI*8000*i/samplerate_rfpwm+phi);
  //}
  
  //delete xmtfilt;

  //printf("Applying filter for USB\n");
  //fftfilt lpf(0.0/samplerate, 500.0/samplerate, outputBufferIndex);
  //cmplx *clpf;
  //for(int i=0; i<outputBufferIndex; i++){
      //lpf.run(c[i],&clpf);
  //}
  //for(int i=0; i<outputBufferIndex;i++){
    //printf("(%e+%ej)\n", clpf[i].re, clpf[i].im);
    ////printf("%f %f, %f %f\n", c[i].mag(), c[i].arg(), cpbpf[i].mag(), cpbpf[i].arg());
  //}
  
  //double a, phi;
  //for(int i=0; i<outputBufferResampledIndex; i++){
      //a = c[i].mag();
      //a = asin(a)/M_PI;
      //phi = c[i].arg();
      //outputBufferResampled[i] = cos(2*M_PI*8000*i/samplerate_rfpwm+phi);
  //}
 
  //printf("IF carrier complete\n");
  
  double a;
  while (i < outputBufferResampledIndex && outputBufferResampled[i++] < 0) {
  }
  while (i < outputBufferResampledIndex && len < EDGESIZE) {
    uint64_t j = 0;
    a = outputBufferResampled[i];
    while (i + j < outputBufferResampledIndex and outputBufferResampled[i + j] >= 0) {
      j++;
    }
    while (i + j < outputBufferResampledIndex and outputBufferResampled[i + j] < 0) {
      j++;
    }
    if(len < EDGESIZE - 1){
      //edges[len++] = (asin(c[i].mag())/M_PI)*(uint64_t)(1e9 * j / samplerate_rfpwm);
      edges[len++] = (asin(a)/M_PI)*(uint64_t)(1e9 * j / samplerate_rfpwm);
      //edges[len++] = 0.5*(uint64_t)(1e9 * j / samplerate_rfpwm);
      edges[len++] = (uint64_t)(1e9 * j / samplerate_rfpwm);
    }else{
      printf("Edges full, skipping\n");
    }
    i += j;
  }
  //printf("len %d\n", len);
  for (int i = 0; i < len; i += 2) {
    printf("%lu %lu\n", edges[i], edges[i + 1]);
  }
  //for(i = 0; i < outputBufferResampledIndex; i++){
      //printf("%f\n", outputBufferResampled[i]);
  //}
  printf("Startig RF-PMM transmission with DRAM\n");

  // Address to hammer for the basic operation
  uint64_t variable = ~0;
  uint64_t *address = &variable;

  do {
    int x = 0;
    uint64_t mid, reset;
    uint64_t start = get_ns();
    while (x < len) {
      mid = start + edges[x++];
      reset = start + edges[x++];
    
      // High period
      while (get_ns() < mid) {
        // basic operation
        hammer_civac(address);
      }
      // Low period
      while (get_ns() < reset) {
      };
      // Reset the staring point, using reset gives the better result
      // start = high_resolution_clock::now();
      start = reset;
    }
  } while(forever);
  
  outputBufferIndex = 0;
  outputBufferResampledIndex = 0;
  printf("Output buffer empty");
}

//Flushes the end of the sound buffer
void flushTxOutputBuffer() {
        outputBufferIndex = 0;
}
//void flushTxOutputBuffer() {
	//if (outputBufferIndex > 0) {
		////Find the Java class
		//jclass cls = gEnv->FindClass("com/AndFlmsg/Modem");
		////Find the static Java method (see signature)
		//jmethodID mid = gEnv->GetStaticMethodID(cls, "txModulate", "([DI)V");
		////Create a Java array and copy the C++ array into it
		//jdoubleArray jBuffer = gEnv->NewDoubleArray(outputBufferIndex);
		//gEnv->SetDoubleArrayRegion(jBuffer, 0, outputBufferIndex, outputBuffer);
		////Call the method with the Java array
		//gEnv->CallStaticVoidMethod(cls, mid, jBuffer, outputBufferIndex);
		////Release the intermediate array and the other variables
		//gEnv->DeleteLocalRef(jBuffer);
		//gEnv->DeleteLocalRef(cls);
		//outputBufferIndex = 0;
	//}
//}


extern void change_CModem(int modemCode, double newFrequency) {
	//Delete previously created modem to recover memory used
	if (active_modem) {
		delete active_modem;
		active_modem = NULL;
	}
	//Now re-create with new mode
	if (modemCode >= MODE_PSK_FIRST && modemCode <= MODE_PSK_LAST) {
		active_modem = new psk(modemCode);
	} else if (modemCode >= MODE_DOMINOEX_FIRST && modemCode <= MODE_DOMINOEX_LAST) {
		active_modem = new dominoex(modemCode);
	} else if (modemCode >= MODE_THOR_FIRST && modemCode <= MODE_THOR_LAST) {
		active_modem = new thor(modemCode);
	} else if (modemCode >= MODE_MFSK_FIRST && modemCode <= MODE_MFSK_LAST) {
		active_modem = new mfsk(modemCode);
	} else if (modemCode >= MODE_MT63_FIRST && modemCode <= MODE_MT63_LAST) {
		active_modem = new mt63(modemCode);
	} else if (modemCode >= MODE_OLIVIA_FIRST && modemCode <= MODE_OLIVIA_LAST) {
		active_modem = new olivia(modemCode);
	} else if (modemCode >= MODE_CONTESTIA_FIRST && modemCode <= MODE_CONTESTIA_LAST) {
		active_modem = new contestia(modemCode);
	}
	//Do the initializations
	if (active_modem != NULL) {
		//First overall modem initilisation
		active_modem->init();
		//Moved before rx_init as per Fldigi
		//Then set RX frequency
		active_modem->set_freq(newFrequency);
		//Then init of RX side
		active_modem->rx_init();
		//Reset UTF-8 sequence monitor
		utfExpectedChars = utfFoundChars = 0;
	}
}

//***************** FROM JAVA to C++  ******************


//Save Tx thread environment
extern "C" JNIEXPORT void
Java_com_AndFlmsg_Modem_saveEnv( JNIEnv* env, jobject thishere)
{
	gEnv = env;
}


//Fast modem change (for Txing image for example) change_CModem
extern "C" JNIEXPORT void
Java_com_AndFlmsg_Modem_changeCModem( JNIEnv* env, jobject thishere, jint modemCode, jdouble newFrequency)
{
	gEnv = env;

	//change_CModem(modemCode, active_modem->get_freq()); //Same frequency as previous modem
	change_CModem(modemCode, newFrequency); //Same frequency as previous modem
}


//Creates the RSID receive Modem
int modem_createRsidModem(){
	//Delete previously created RSID modem to recover memory used
	if (RsidModem) {
		delete RsidModem;
		RsidModem = NULL;
	}
	RsidModem = new cRsId();
	if (RsidModem != NULL) {
		return 0;
	} else {
		return 1;
	}
}

void modem_destroyRsidModem(){
    if (RsidModem) {
        delete RsidModem;
        RsidModem = NULL;
    }
}
//extern "C" JNIEXPORT jstring
//Java_com_AndFlmsg_Modem_createRsidModem(JNIEnv* env, jobject thishere) {
	////Save environment if we need to call the preference methods in Java
	//gEnv = env;
	//gJobject = thishere;

	////Delete previously created RSID modem to recover memory used
	//if (RsidModem) {
		//delete RsidModem;
		//RsidModem = NULL;
	//}
	//RsidModem = new cRsId();
	//if (RsidModem != NULL) {
		//return (env)->NewStringUTF("RSID Modem created");
	//} else {
		//return (env)->NewStringUTF("ERROR: RSID Modem Not created");
	//}
//}

//Returns an array of modem names (uses the RSID list in rsid_def.cxx)
extern "C" JNIEXPORT jobjectArray
Java_com_AndFlmsg_Modem_getModemCapListString(JNIEnv* env, jobject thishere)
{
	char *modeCapListString[MAXMODES];
	jobjectArray returnedArray;
	int i;
	int j = 0;

	returnedArray = (jobjectArray)env->NewObjectArray(MAXMODES,
			env->FindClass("java/lang/String"),
			env->NewStringUTF(""));
	for (i=0; i < RsidModem->rsid_ids_size1; i++) {
		if (RsidModem->rsid_ids_1[i].mode != NUM_MODES) {
			env->SetObjectArrayElement(returnedArray,j++,env->NewStringUTF(RsidModem->rsid_ids_1[i].name));
		}
	}

	//do the same for 2nd list
	for (i=0; i < RsidModem->rsid_ids_size2; i++) {
		if (RsidModem->rsid_ids_2[i].mode != NUM_MODES) {
			env->SetObjectArrayElement(returnedArray,j++,env->NewStringUTF(RsidModem->rsid_ids_2[i].name));
		}
	}
//Android debug  env->SetObjectArrayElement(returnedArray,j++,env->NewStringUTF("Bpsk31"));

	//Finalise the array
	env->SetObjectArrayElement(returnedArray,j,env->NewStringUTF("END"));

	return(returnedArray);

}


//Returns an array of modem codes (uses the RSID list in rsid_def.cxx)
extern "C" JNIEXPORT jintArray
Java_com_AndFlmsg_Modem_getModemCapListInt(JNIEnv* env, jobject thishere)
{

	jintArray returnedArray;
	int i;
	int j = 0;

	returnedArray = env->NewIntArray(MAXMODES);
	jint temp[MAXMODES];

	for (i=0; i < RsidModem->rsid_ids_size1; i++) {
		if (RsidModem->rsid_ids_1[i].mode != NUM_MODES) {
			temp[j++] = RsidModem->rsid_ids_1[i].mode;
		}
	}

	//do the same for 2nd list
	for (i=0; i < RsidModem->rsid_ids_size2; i++) {
		if (RsidModem->rsid_ids_2[i].mode != NUM_MODES) {
			temp[j++] = RsidModem->rsid_ids_2[i].mode;
		}
	}

	//Finalise the array with a negative value
	temp[j] = -1;

	//Copy to Java structure
	env->SetIntArrayRegion(returnedArray, 0, MAXMODES, temp);

	return(returnedArray);

}


//Creates the requested modem in C++
int modem_createModem(int modemCode){
	//Delete previously created modem to recover memory used
	if (active_modem) {
		delete active_modem;
		active_modem = NULL;
	}

	//Now re-create with new mode
	if (modemCode >= MODE_PSK_FIRST && modemCode <= MODE_PSK_LAST) {
		active_modem = new psk(modemCode);
	} else if (modemCode >= MODE_DOMINOEX_FIRST && modemCode <= MODE_DOMINOEX_LAST) {
		active_modem = new dominoex(modemCode);
	} else if (modemCode >= MODE_THOR_FIRST && modemCode <= MODE_THOR_LAST) {
		active_modem = new thor(modemCode);
	} else if (modemCode >= MODE_MFSK_FIRST && modemCode <= MODE_MFSK_LAST) {
		active_modem = new mfsk(modemCode);
	} else if (modemCode >= MODE_MT63_FIRST && modemCode <= MODE_MT63_LAST) {
		active_modem = new mt63(modemCode);
	} else if (modemCode >= MODE_OLIVIA_FIRST && modemCode <= MODE_OLIVIA_LAST) {
		active_modem = new olivia(modemCode);
	} else if (modemCode >= MODE_CONTESTIA_FIRST && modemCode <= MODE_CONTESTIA_LAST) {
		active_modem = new contestia(modemCode);
	} else {
		return 1;
	}
	return 0;
}

void modem_destroyModem(){
    if (active_modem) {
        delete active_modem;
        active_modem = NULL;
    }
}

//extern "C" JNIEXPORT jstring
//Java_com_AndFlmsg_Modem_createCModem( JNIEnv* env, jobject thishere,
		//jint modemCode)
//{
	////Save environment if we need to call the preference methods in Java
	//gEnv = env;
	//gJobject = thishere;

	////Delete previously created modem to recover memory used
	//if (active_modem) {
		//delete active_modem;
		//active_modem = NULL;
	//}

	////Now re-create with new mode
	//if (modemCode >= MODE_PSK_FIRST && modemCode <= MODE_PSK_LAST) {
		//active_modem = new psk(modemCode);
	//} else if (modemCode >= MODE_DOMINOEX_FIRST && modemCode <= MODE_DOMINOEX_LAST) {
		//active_modem = new dominoex(modemCode);
	//} else if (modemCode >= MODE_THOR_FIRST && modemCode <= MODE_THOR_LAST) {
		//active_modem = new thor(modemCode);
	//} else if (modemCode >= MODE_MFSK_FIRST && modemCode <= MODE_MFSK_LAST) {
		//active_modem = new mfsk(modemCode);
	//} else if (modemCode >= MODE_MT63_FIRST && modemCode <= MODE_MT63_LAST) {
		//active_modem = new mt63(modemCode);
	//} else if (modemCode >= MODE_OLIVIA_FIRST && modemCode <= MODE_OLIVIA_LAST) {
		//active_modem = new olivia(modemCode);
	//} else if (modemCode >= MODE_CONTESTIA_FIRST && modemCode <= MODE_CONTESTIA_LAST) {
		//active_modem = new contestia(modemCode);
	//} else {
		//return (env)->NewStringUTF("ERROR: Modem NOT created");
	//}
	//return (env)->NewStringUTF("Modem created");
//}


//Initializes the RX section of the requested modem in C++
extern "C" JNIEXPORT jstring
Java_com_AndFlmsg_Modem_initCModem( JNIEnv* env, jobject thishere,
		jdouble frequency)
{
	//Save environment if we need to call the preference methods in Java
	gEnv = env;
	gJobject = thishere;

	if (active_modem != NULL) {
		//First overall modem initialisation
		active_modem->init();
		//Moved from after rx_init as per Fldigi
		//Then set RX frequency
		active_modem->set_freq(frequency);
		//Then init of RX side
		active_modem->rx_init();
		lastCharPos = 0;
		//Reset UTF-8 sequence monitor
		utfExpectedChars = utfFoundChars = 0;
		return (env)->NewStringUTF("Modem Initialized");
	} else {
		return (env)->NewStringUTF("ERROR: Modem NOT Initialized");
	}
}



//Processes the audio buffer through the current modem in C++
extern "C" JNIEXPORT jstring
Java_com_AndFlmsg_Modem_rxCProcess( JNIEnv* env, jobject thishere,
		jshortArray myjbuffer, jint length)
{
	//Save environment if we need to call any methods in Java
	gEnv = env;
	gJobject = thishere;

	//Reset received characters buffer pointer
	lastCharPos = 0;
	//Convert to C++ type
	jshort* shortBuffer = env->GetShortArrayElements(myjbuffer, 0);

	//Process the buffer in C++
	if (active_modem != NULL) {
		active_modem->rx_process(shortBuffer, length);
	} else {
		return (env)->NewStringUTF("ERROR: Modem NOT Initialized");
	}
	//Release the passed parameters
	env->ReleaseShortArrayElements(myjbuffer, shortBuffer, 0);

	//Terminate the buffer
	decodedBuffer[lastCharPos] = 0;

	//Return the decoded data
	return env->NewStringUTF(decodedBuffer);
}


//Updates the modem with the latest GUI squelch level
extern "C" JNIEXPORT jint
Java_com_AndFlmsg_Modem_setSquelchLevel( JNIEnv* env, jobject thishere,
		jdouble squelchLevel)
{
	if ( active_modem != NULL) {
			active_modem->set_squelchLevel(squelchLevel);
		}
	return 0;
}


//Returns the latest signal metric from the current modem in C++
extern "C" JNIEXPORT jdouble
Java_com_AndFlmsg_Modem_getMetric( JNIEnv* env, jobject thishere)
{
	double metric = 0.0;
	if ( active_modem != NULL) {
		metric = active_modem->get_metric();
	}
	return metric;
}

//Processes the audio buffer through the RSID modem
extern "C" JNIEXPORT jstring
Java_com_AndFlmsg_Modem_RsidCModemReceive( JNIEnv* env, jobject thishere,
		jfloatArray myfbuffer, jint length, jboolean doSearch)
{
	//Save environment if we need to call any methods in Java
	gEnv = env;
	gJobject = thishere;

	//Convert to C++ type
	jboolean myjcopy = true;
	jfloat* floatBuffer = env->GetFloatArrayElements(myfbuffer, &myjcopy);
	//Reset received characters buffer pointer
	lastCharPos = 0;
	//Process the buffer in C++
	RsidModem->receive(floatBuffer, length, doSearch);
	//Release the passed parameters
	env->ReleaseFloatArrayElements(myfbuffer, floatBuffer, 0);
	//Terminate the buffer
	decodedBuffer[lastCharPos] = 0;

	return env->NewStringUTF(decodedBuffer);

}

//Returns the current modem frequency (for waterfall tracking after RSID)
extern "C" JNIEXPORT jdouble
Java_com_AndFlmsg_Modem_getCurrentFrequency( JNIEnv* env, jobject thishere)
{
	return active_modem->get_freq();
}

//Returns the current mode (for updating the Java side)
extern "C" JNIEXPORT jint
Java_com_AndFlmsg_Modem_getCurrentMode( JNIEnv* env, jobject thishere)
{
	return active_modem->get_mode();
}

//not used - check code before use
//Returns the decoded characters resulting from the flushing of the rx pipe on RSID rx of new modem
extern "C" JNIEXPORT jint
Java_com_AndFlmsg_Modem_getFlushedRxCharacters( JNIEnv* env, jobject thishere)
{
	return active_modem->get_mode();
}




//Transmit section-------------------



//Send RSID of current mode
void modem_txRSID(){
	if (RsidModem != NULL && active_modem != NULL) {
		RsidModem->send(true); //Always true as we handle post-rsid decision in higher level
	}
}
//extern "C" JNIEXPORT void
//Java_com_AndFlmsg_Modem_txRSID(JNIEnv* env, jobject thishere)
//{
	////Save environment if we need to call any methods in Java
	//gEnv = env;
	//gJobject = thishere;

	//if (RsidModem != NULL && active_modem != NULL) {
		//RsidModem->send(true); //Always true as we handle post-rsid decision in higher level
	//}
//}

//Initializes the TX section of the requested modem in C++
int modem_txInit(double frequency, int samplerate){
    //Init static variable
	outputBufferIndex = 0;

	if (active_modem != NULL) {
		//Then set TX frequency
        active_modem->set_freq(frequency);
        active_modem->set_samplerate(samplerate);
		//Init of TX side
		active_modem->tx_init();
		return 0;
	} else {
		return 1;
	}
}
//extern "C" JNIEXPORT jstring
//Java_com_AndFlmsg_Modem_txInit( JNIEnv* env, jobject thishere,
		//jdouble frequency)
//{
	////Save environment if we need to call any methods in Java
	//gEnv = env;
	//gJobject = thishere;

	////Init static variable
	//outputBufferIndex = 0;

	//if (active_modem != NULL) {
		////Then set TX frequency
		//active_modem->set_freq(frequency);
		////Init of TX side
		//active_modem->tx_init();
		//return (env)->NewStringUTF("Tx Modem Initialized");
	//} else {
		//return (env)->NewStringUTF("ERROR: Tx Modem NOT Initialized");
	//}
//}

//Processes the data buffer for TX through the current modem in C++
int modem_txCProcess(signed char* buffer, int length) {
	//Reset the static variables
	txCounter = 0;
	//Convert to C++ type
	txDataBuffer = buffer;
	txDataBufferLength = length;
	//Process the buffer in C++
	if (active_modem != NULL) {
		//As there is no idle time in Flmsg context, keep
		//  processing until we have exhausted the data buffer
		while (active_modem->tx_process() >= 0){  //Character processing is done in the test
		}
	} else {
		return 1;
	}
	//Flush the tx Sound Buffer
	//flushTxOutputBuffer();
	//Release the passed parameters
	//Copied from RadioMSG to avoid seg fault
 	//env->ReleaseByteArrayElements(myjbuffer, txDataBuffer, 0); OR
   	//env->ReleaseByteArrayElements(myjbuffer, txDataBuffer, JNI_ABORT);
    //env->DeleteLocalRef(myjbuffer);

	//Return the status
	return 0;
}

//extern "C" JNIEXPORT jboolean
//Java_com_AndFlmsg_Modem_txCProcess( JNIEnv* env, jobject thishere,
//		jbyteArray myjbuffer, jint length)
//{
//	//Save environment if we need to call any methods in Java
//	gEnv = env;
//	gJobject = thishere;
//
//	//Reset the static variables
//	txCounter = 0;
//	//Convert to C++ type
//	txDataBuffer = env->GetByteArrayElements(myjbuffer, 0);
//	txDataBufferLength = length;
//	//Process the buffer in C++
//	if (active_modem != NULL) {
//		//As there is no idle time in Flmsg context, keep
//		//  processing until we have exhausted the data buffer
//		while (active_modem->tx_process() >= 0){  //Character processing is done in the test
//		}
//	} else {
//		//Release the passed parameters
//		env->ReleaseByteArrayElements(myjbuffer, txDataBuffer, 0);
//		return JNI_FALSE;
//	}
//	//Flush the tx Sound Buffer
//	flushTxOutputBuffer();
//	//Release the passed parameters
//	//Copied from RadioMSG to avoid seg fault
// 	//env->ReleaseByteArrayElements(myjbuffer, txDataBuffer, 0); OR
//   	//env->ReleaseByteArrayElements(myjbuffer, txDataBuffer, JNI_ABORT);
//    	env->DeleteLocalRef(myjbuffer);
//
//	//Return the status
//	return JNI_TRUE;
//}



//Returns the percent progress of TXing the buffer in characters (not time)
extern "C" JNIEXPORT jint
Java_com_AndFlmsg_Modem_getTxProgressPercent( JNIEnv* env, jobject thishere)
{
	int progressRatio = 0;
	if (txDataBufferLength > 0)
		progressRatio = (100 * txCounter) / txDataBufferLength;
	return progressRatio;
}


//Returns the percent progress of TXing the buffer in characters (not time)
extern "C" JNIEXPORT void
Java_com_AndFlmsg_Modem_setSlowCpuFlag( JNIEnv* env, jobject thishere,
		jboolean mSlowCpu)
{
	progdefaults.slowcpu = mSlowCpu;
	return;
}


//Processes the picture data buffer for TX through the current MFSK modem in C++
extern "C" JNIEXPORT void
Java_com_AndFlmsg_Modem_txPicture( JNIEnv* env, jobject thishere,
		jbyteArray txPictureBuffer, jint txPictureWidth, jint txPictureHeight,
		jint txPictureTxSpeed, jint txPictureColour)
{
	//Save environment if we need to call any methods in Java
	gEnv = env;
	gJobject = thishere;

	//Make sure we have the correct modem
	if (serviceme != active_modem) return;
	//Extract the data and a pointer
	signed char *inbuf = env->GetByteArrayElements(txPictureBuffer, 0);
	//Convert to C++ Picture Buffer
	int W, H, rowstart;
	W = txPictureWidth;
	H = txPictureHeight;
	serviceme->TXspp = txPictureTxSpeed;
//Problem: boolean parameter not passed correctly
	bool colour = (txPictureColour != 0);
	//bool colour = false;
	//Clear old transmit buffer
	if (xmtpicbuff) {
		delete [] xmtpicbuff;
		xmtpicbuff = NULL;
	}
	//Android Bitmap library: we always have a 4 bytes per pixel in RGBA order
	//   representing the bitmap picture regardless
	//   of the colour depth. Discard the Alpha and keep the RGB as the RGB values are pre-multiplied.
	int iy, ix, value;
	if (colour) {
		//RGB = 3 bytes per pixel
		xmtpicbuff = new unsigned char [W*H*3];
		unsigned char *outbuf = xmtpicbuff;
		for (iy = 0; iy < H; iy++) {
			rowstart = iy * W;
			for (ix = 0; ix < W; ix++) {
				//Skip Alpha bytes and change from signed to unsigned char
				value = inbuf[(rowstart + ix)*4];
				outbuf[(rowstart*3 + ix)] = (unsigned char)(value < 0 ? value + 256 : value);
				value = inbuf[(rowstart + ix)*4 + 1];
				outbuf[(rowstart*3 + ix + W)] = (unsigned char)(value < 0 ? value + 256 : value);
				value = inbuf[(rowstart + ix)*4 + 2];
				outbuf[(rowstart*3 + ix + W + W)] = (unsigned char)(value < 0 ? value + 256 : value);
			}
		}
		serviceme->xmtbytes = W * H * 3;
		serviceme->color = true;
		if (serviceme->TXspp == 8)
			snprintf(serviceme->picheader, PICHEADER, "\nSending Pic:%dx%dC;", W, H);
		else
			snprintf(serviceme->picheader, PICHEADER, "\nSending Pic:%dx%dCp%d;", W, H,serviceme->TXspp);
	} else {
		//Grey-Scale = single byte per pixel
		xmtpicbuff = new unsigned char [W*H];
		unsigned char *outbuf = xmtpicbuff;
		int greyTotal;
		for (iy = 0; iy < H; iy++) {
			rowstart = iy * W;
			for (ix = 0; ix < W; ix++) {
				value = inbuf[(rowstart + ix)*4];
				greyTotal = 31 * (value < 0 ? value + 256 : value);
				value = inbuf[(rowstart + ix)*4 + 1];
				greyTotal  += 61 * (value < 0 ? value + 256 : value);
				value = inbuf[(rowstart + ix)*4 + 2];
				greyTotal  += 8 * (value < 0 ? value + 256 : value);
				outbuf[rowstart + ix] = (unsigned char)(greyTotal / 100);
			}
		}
		serviceme->xmtbytes = W * H;
		serviceme->color = false;
		if (serviceme->TXspp == 8) {
			snprintf(serviceme->picheader, PICHEADER, "\nSending Pic:%dx%d;", W, H);
		} else {
			snprintf(serviceme->picheader, PICHEADER, "\nSending Pic:%dx%dp%d;", W, H,serviceme->TXspp);
	    }
	}
	serviceme->rgb = 0;
	serviceme->col = 0;
	serviceme->row = 0;
	serviceme->pixelnbr = 0;

	// start the transmission
	serviceme->startpic = true;

	//Process the buffer in C++
	while (active_modem->tx_process() >= 0){  //Character processing is done in the test
	}
	//Flush the tx Sound Buffer
	flushTxOutputBuffer();
	//Clear the transmit buffer
	if (xmtpicbuff) {
		delete [] xmtpicbuff;
		xmtpicbuff = NULL;
	}
	//Release the passed parameters
	//RadioSMS: avoid seg fault on release
	//env->ReleaseByteArrayElements(txPictureBuffer, inbuf, 0);
	//env->ReleaseByteArrayElements(txPictureBuffer, inbuf, JNI_ABORT);
	env->DeleteLocalRef(txPictureBuffer);

}

