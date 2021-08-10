/*
 * Copyright 2020, Giovanni Camurati
 * plus mix of fldigi/fldmsg and drammer
 */

#include "AndFlmsg_Fldigi_Interface.h"
#include "globals.h"
#include <fstream>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#ifdef SMALL
#define SIZE 1000
#else
#define SIZE 10000
#endif

// Hello message
// https://onlineasciitools.com/convert-text-to-ascii-art
void hello(void) {
  printf("####### #                               #     #                      "
         "         #####  ######  ###### \n");
  printf("#       #       #####  #  ####  #       ##    #  ####  #  ####  "
         "######       #     # #     # #     #\n");
  printf("#       #       #    # # #    # #       # #   # #    # # #      #    "
         "        #       #     # #     #\n");
  printf("#####   #       #    # # #      # ##### #  #  # #    # #  ####  "
         "#####  #####  #####  #     # ###### \n");
  printf("#       #       #    # # #  ### #       #   # # #    # #      # #    "
         "              # #     # #   #  \n");
  printf("#       #       #    # # #    # #       #    ## #    # # #    # #    "
         "        #     # #     # #    # \n");
  printf("#       ####### #####  #  ####  #       #     #  ####  #  ####  "
         "######        #####  ######  #     #\n");
}

// Help message
void help(char *program_name) {
  fprintf(stderr, "Usage: %s <options>   : Transmit\n", program_name);
  fprintf(stderr, "          -d data     : Data to send\n");
  fprintf(stderr, "          -m mode     : Mode\n");
  fprintf(stderr, "          -s fs       : Sample rate\n");
  fprintf(stderr, "          -c fc       : Carrier\n");
  fprintf(stderr, "          -o filename : Save output buffer to file\n");
  fprintf(stderr, "          -w filename : Save rfpwm to file\n");
  fprintf(stderr, "          -i filename : Input file for data to send\n");
  fprintf(stderr, "          -r          : Reed Solomon Identifier\n");
  fprintf(stderr, "          -f          : Run forever\n");
  fprintf(stderr, "          -v wpm      : WPM for CW\n");
  fprintf(stderr, "          -e edge     : Edge <0.0-15.0> (ms) for CW\n");
  fprintf(stderr, "          -b baud     : Baudrate for RTTY\n");
  fprintf(stderr, "                        0: 45\n");
  fprintf(stderr, "                        1: 45.45\n");
  fprintf(stderr, "                        2: 50\n");
  fprintf(stderr, "                        3: 56\n");
  fprintf(stderr, "                        4: 75\n");
  fprintf(stderr, "                        5: 100\n");
  fprintf(stderr, "                        6: 110\n");
  fprintf(stderr, "                        7: 150\n");
  fprintf(stderr, "                        8: 200\n");
  fprintf(stderr, "                        9: 300\n");
  fprintf(stderr, "          -n          : RTTY not shaped\n");
  // fprintf(stderr, "Usage: %s -l filename : Transmit\n", program_name);
  // fprintf(stderr, "          -m          : Measure basic op\n");
  // fprintf(stderr, "          -u          : Synchronize FT4 to UTC time\n");
  // fprintf(stderr, "          -q cpu      : Pin to core\n");
}

// Main program
int main(int argc, char **argv) {

  int c;
  double frequency = 1000;
  // int samplerate = 8000;
  bool forever = false;
  bool rsid = false;
  int datalen = 6;
  signed char buffer[SIZE] = "hello\n";
  MODES mode = MODE_PSK31;
  char *buffile = NULL;
  char *rfpwmfile = NULL;
  double wpm = progdefaults.CWspeed;
  double cwrisetime = progdefaults.CWrisetime;
  int rtty_baud = 0;
  bool rtty_shaped = true;

  hello();

  opterr = 0;
  while ((c = getopt(argc, argv, ":b:v:e:d:m:s:c:o:w:i:frn")) != -1) {
    switch (c) {
    case 'i': {
      int i = 0;
      char c;
      std::ifstream infile(optarg);
      while (infile.get(c) && i < SIZE) {
        buffer[i++] = c;
      }
      datalen = i;
      infile.close();
      break;
    }
    case 'd': {
      datalen = strlen(optarg);
      int i = 0;
      while (i < datalen) {
        if (i > SIZE - 1) {
          datalen = i - 1;
          break;
        }
        buffer[i] = optarg[i];
        i++;
      }
      buffer[i] = '\n';
      datalen++;
      // printf("datalen %d\n", datalen);
      // for (int i = 0; i < datalen; i++)
      // printf("%c", buffer[i]);
    } break;
    case 'm': {
      auto it = modes_map.find(optarg);
      if (it != modes_map.end()) {
        mode = it->second;
      } else {
        fprintf(stderr, "Mode %s does not exist.\n", optarg);
        abort();
      }
    } break;
    case 'f':
      forever = true;
      break;
    case 'r':
      rsid = true;
      break;
    case 'v':
      wpm = atof(optarg);
      break;
    case 'e':
      cwrisetime = atof(optarg);
      break;
    // case 's':
    // samplerate = atoi(optarg);
    // break;
    case 'c':
      frequency = atof(optarg);
      break;
    case 'o':
      buffile = optarg;
      break;
    case 'w':
      rfpwmfile = optarg;
      break;
    case 'b':
      rtty_baud = atoi(optarg);
      if (rtty_baud < 0 || rtty_baud > 9) {
        fprintf(stderr, "Invalid baud rate index\n");
        abort();
      }
      break;
    case 'n':
      rtty_shaped = false;
      break;
    // case 'u':
    // sync = true;
    // break;
    // case 'q':
    // cpu_pinning = strtol(optarg, NULL, 10);
    // break;
    case '?':
      if (optopt == 'd' || optopt == 'm' || optopt == 's' || optopt == 'c' ||
          optopt == 'o' || optopt == 'w' || optopt == 'i' || optopt == 'v' ||
          optopt == 'e' || optopt == 'b')
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint(optopt))
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      else
        fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
      help(argv[0]);
      return 1;
    default:
      abort();
    }
  }

  modem_createModem(mode, wpm, cwrisetime, rtty_baud, rtty_shaped);
  modem_txInit(frequency);

  if (rsid) {
    modem_createRsidModem(wpm, cwrisetime, rtty_baud, rtty_shaped);
    modem_txRSID();
  }

  // for(int i=0; i < 128; i++){
  // buffer[i] = 'a'+i%24;
  // buffer[i+1] = 'b';
  //}
  // buffer[127] = '\n';
  // modem_txCProcess(buffer, 128);
  // for (int i = 0; i < 60; i += 6) {
  // buffer[0 + i] = 'h';
  // buffer[1 + i] = 'e';
  // buffer[2 + i] = 'l';
  // buffer[3 + i] = 'l';
  // buffer[4 + i] = 'o';
  // buffer[5 + i] = '\n';
  //}
  //
  modem_txCProcess(buffer, datalen);

  RFPWM(forever, buffile, rfpwmfile);

  modem_destroyRsidModem();
  modem_destroyModem();
  return 0;
}
