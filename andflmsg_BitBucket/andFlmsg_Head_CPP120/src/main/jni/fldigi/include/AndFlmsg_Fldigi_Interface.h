#ifndef ANDFLMSG_FLDIGI_INTERFACE_H
#define ANDFLMSG_FLDIGI_INTERFACE_H


#include <jni.h>
#include "rsid.h"

using std::string;

extern modem *active_modem;

extern void txModulate(double *buffer, int len);
extern void put_rx_char(int receivedChar);
extern void put_echo_char(unsigned int txedChar);
extern int  get_tx_char();
extern bool getNewAmplReady();
extern void updateWaterfallBuffer(double *processedFft);
extern string getPreferenceS2(string preferenceString, string defaultValue);
extern int getPreferenceI(string preferenceString, int defaultValue);
extern double getPreferenceD(string preferenceString, double defaultValue);
extern bool getPreferenceB(string preferenceString, bool defaultValue);
extern void change_CModem(int newMode, double newFrequency);
extern void androidShowRxViewer(int picW, int picH);
extern void androidSaveLastPicture();
extern void androidUpdateRxPic(int data, int pos);


#endif
