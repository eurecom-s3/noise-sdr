/*
 * Code based on Flmsg version 2.0.4
 */

#ifndef ANDFLMSG_FLMSG_INTERFACE_H
#define ANDFLMSG_FLMSG_INTERFACE_H


#include <jni.h>
#include <string>
#include "flmsg.h"

using namespace std;

static int transfer_size;
static 	JNIEnv* gEnv2 = NULL;
static	jobject gJobject2 = NULL;

extern string getPreferenceS2(string preferenceString, string defaultValue);
extern int getPreferenceI2(string preferenceString, int defaultValue);
extern double getPreferenceD2(string preferenceString, double defaultValue);
extern bool getPreferenceB2(string preferenceString, bool defaultValue);

extern void estimate();
extern void select_form(int form);


#endif
