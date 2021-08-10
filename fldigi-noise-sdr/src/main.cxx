/*
 * Copyright 2020, Giovanni Camurati
 * plus mix of fldigi/fldmsg and drammer
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <fstream>
#include "AndFlmsg_Fldigi_Interface.h"
#include "globals.h"

#define SIZE 10000

// Main program
int main(int argc, char **argv) {

  double frequency = 1000;
  int samplerate =   8000;
  bool forever = true;

  modem_createModem(MODE_THOR11);
  modem_txInit(frequency, samplerate);
  //modem_createRsidModem();

  //modem_txRSID();

  signed char buffer[SIZE];
  //for(int i=0; i < 128; i++){
      //buffer[i] = 'a'+i%24;
      //buffer[i+1] = 'b';
  //}
  //buffer[127] = '\n';
  //modem_txCProcess(buffer, 128);
  for(int i=0; i<60;i+=6){
  buffer[0+i] = 'h';
  buffer[1+i] = 'e';
  buffer[2+i] = 'l';
  buffer[3+i] = 'l';
  buffer[4+i] = 'o';
  buffer[5+i] = '\n';
  }
  modem_txCProcess(buffer, 6*10);

  RFPWM(forever);

  modem_destroyRsidModem();
  modem_destroyModem();
  return 0;
}
