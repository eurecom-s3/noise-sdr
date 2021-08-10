// ----------------------------------------------------------------------------
//
//	rsid.cxx
//
// Copyright (C) 2008-2012
//		Dave Freese, W1HKJ
// Copyright (C) 2009-2012
//		Stelios Bounanos, M0GLD
// Copyright (C) 2012, 2014
//		John Douyere, VK2ETA
//
// This file is part of fldigi.
//
// Fldigi is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Fldigi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with fldigi.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

// Android #include <config.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstring>
#include <float.h>
#include <string>
// Android not yet #include <samplerate.h>

#include "filters.h"
#include "misc.h"
#include "rsid.h"
// Android #include "trx.h"
// Android #include "fl_digi.h"
// Android #include "configuration.h"
// Android #include "confdialog.h"
// Android #include "qrunner.h"
// Android #include "notify.h"
// Android #include "debug.h"

// Android #include "main.h"
// Android #include "arq_io.h"

// Android not yet LOG_FILE_SOURCE(debug::LOG_MODEM);
// Android
#include "AndFlmsg_Fldigi_Interface.h"

//#include "rsid_defs.cxx"
// Android moved to rsid.h
//#define NUM_MODES 9999

#undef ELEM_
#define RSID_LIST                                                                                                           \
                                                                                                                            \
  /* ESCAPE used to transition to 2nd RSID set */                                                                           \
                                                                                                                            \
  ELEM_(6, ESCAPE, NUM_MODES)                                                                                               \
                                                                                                                            \
  ELEM_(1, BPSK31, MODE_PSK31)                                                                                              \
  /*        ELEM_(110, QPSK31, MODE_QPSK31)     */                                                                          \
  ELEM_(2, BPSK63, MODE_PSK63)                                                                                              \
  /*        ELEM_(3, QPSK63, MODE_QPSK63)       */                                                                          \
  ELEM_(4, BPSK125, MODE_PSK125)                                                                                            \
  /*        ELEM_(5, QPSK125, MODE_QPSK125)     */                                                                          \
  ELEM_(126, BPSK250, MODE_PSK250)                                                                                          \
  /*        ELEM_(127, QPSK250, MODE_QPSK250)   */                                                                          \
  ELEM_(173, BPSK500, MODE_PSK500)                                                                                          \
                                                                                                                            \
  ELEM_(183, PSK125R, MODE_PSK125R)                                                                                         \
  ELEM_(186, PSK250R, MODE_PSK250R)                                                                                         \
  ELEM_(187, PSK500R, MODE_PSK500R)                                                                                         \
  /*                                                                                                                        \
          ELEM_(7, PSKFEC31, NUM_MODES)                                                                                     \
          ELEM_(8, PSK10, NUM_MODES)     */                                                                                 \
                                                                                                                            \
  ELEM_(9, MT63_500_LG, MODE_MT63_500L)                                                                                     \
  ELEM_(10, MT63_500_ST, MODE_MT63_500S)                                                                                    \
  /*        ELEM_(11, MT63_500_VST, NUM_MODES)      */                                                                      \
  ELEM_(12, MT63_1000_LG, MODE_MT63_1000L)                                                                                  \
  ELEM_(13, MT63_1000_ST, MODE_MT63_1000S)                                                                                  \
  /*        ELEM_(14, MT63_1000_VST, NUM_MODES)     */                                                                      \
  ELEM_(15, MT63_2000_LG, MODE_MT63_2000L)                                                                                  \
  ELEM_(17, MT63_2000_ST, MODE_MT63_2000S)                                                                                  \
  /*        ELEM_(18, MT63_2000_VST, NUM_MODES)     */                                                                      \
                                                                                                                            \
  /*        ELEM_(19, PSKAM10, NUM_MODES)                                                                                   \
          ELEM_(20, PSKAM31, NUM_MODES)                                                                                     \
          ELEM_(21, PSKAM50, NUM_MODES)                                                                                     \
                                                                                                                            \
          ELEM_(22, PSK63F, MODE_PSK63F)                                                                                    \
          ELEM_(23, PSK220F, NUM_MODES)                                                                                     \
                                                                                                                            \
          ELEM_(24, CHIP64, NUM_MODES)                                                                                      \
          ELEM_(25, CHIP128, NUM_MODES)                                                                                     \
                                                                                                                            \
          ELEM_(26, CW, MODE_CW)                                                                                            \
                                                                                                                            \
          ELEM_(27, CCW_OOK_12, NUM_MODES)                                                                                  \
          ELEM_(28, CCW_OOK_24, NUM_MODES)                                                                                  \
          ELEM_(29, CCW_OOK_48, NUM_MODES)                                                                                  \
          ELEM_(30, CCW_FSK_12, NUM_MODES)                                                                                  \
          ELEM_(31, CCW_FSK_24, NUM_MODES)                                                                                  \
          ELEM_(33, CCW_FSK_48, NUM_MODES)                                                                                  \
                                                                                                                            \
          ELEM_(34, PACTOR1_FEC, NUM_MODES)                                                                                 \
                                                                                                                            \
          ELEM_(113, PACKET_110, NUM_MODES)                                                                                 \
          ELEM_(35, PACKET_300, NUM_MODES)                                                                                  \
          ELEM_(36, PACKET_1200, NUM_MODES)                                                                                 \
                                                                                                                            \
          ELEM_(37, RTTY_ASCII_7, MODE_RTTY)                                                                                \
          ELEM_(38, RTTY_ASCII_8, MODE_RTTY)                                                                                \
          ELEM_(39, RTTY_45, MODE_RTTY)                                                                                     \
          ELEM_(40, RTTY_50, MODE_RTTY)                                                                                     \
          ELEM_(41, RTTY_75, MODE_RTTY)                                                                                     \
                                                                                                                            \
          ELEM_(42, AMTOR_FEC, NUM_MODES)                                                                                   \
                                                                                                                            \
          ELEM_(43, THROB_1, MODE_THROB1)                                                                                   \
          ELEM_(44, THROB_2, MODE_THROB2)                                                                                   \
          ELEM_(45, THROB_4, MODE_THROB4)                                                                                   \
          ELEM_(46, THROBX_1, MODE_THROBX1)                                                                                 \
          ELEM_(47, THROBX_2, MODE_THROBX2)                                                                                 \
          ELEM_(146, THROBX_4, MODE_THROBX4)                                                                                \
  */                                                                                                                        \
  ELEM_(204, CONTESTIA_4_125, MODE_CONTESTIA_4_125)                                                                         \
  ELEM_(55, CONTESTIA_4_250, MODE_CONTESTIA_4_250)                                                                          \
  ELEM_(54, CONTESTIA_4_500, MODE_CONTESTIA_4_500)                                                                          \
  ELEM_(255, CONTESTIA_4_1000, MODE_CONTESTIA_4_1000)                                                                       \
  ELEM_(254, CONTESTIA_4_2000, MODE_CONTESTIA_4_2000)                                                                       \
                                                                                                                            \
  ELEM_(169, CONTESTIA_8_125, MODE_CONTESTIA_8_125)                                                                         \
  ELEM_(49, CONTESTIA_8_250, MODE_CONTESTIA_8_250)                                                                          \
  ELEM_(52, CONTESTIA_8_500, MODE_CONTESTIA_8_500)                                                                          \
  ELEM_(117, CONTESTIA_8_1000, MODE_CONTESTIA_8_1000)                                                                       \
  ELEM_(247, CONTESTIA_8_2000, MODE_CONTESTIA_8_2000)                                                                       \
                                                                                                                            \
  ELEM_(275, CONTESTIA_16_250, MODE_CONTESTIA_16_250)                                                                       \
  ELEM_(50, CONTESTIA_16_500, MODE_CONTESTIA_16_500)                                                                        \
  ELEM_(53, CONTESTIA_16_1000, MODE_CONTESTIA_16_1000)                                                                      \
  ELEM_(259, CONTESTIA_16_2000, MODE_CONTESTIA_16_2000)                                                                     \
                                                                                                                            \
  ELEM_(51, CONTESTIA_32_1000, MODE_CONTESTIA_32_1000)                                                                      \
  ELEM_(201, CONTESTIA_32_2000, MODE_CONTESTIA_32_2000)                                                                     \
                                                                                                                            \
  ELEM_(194, CONTESTIA_64_500, MODE_CONTESTIA_64_500)                                                                       \
  ELEM_(193, CONTESTIA_64_1000, MODE_CONTESTIA_64_1000)                                                                     \
  ELEM_(191, CONTESTIA_64_2000, MODE_CONTESTIA_64_2000)                                                                     \
                                                                                                                            \
  /*        ELEM_(56, VOICE, NUM_MODES)                     \
                                                        \
        ELEM_(60, MFSK8, MODE_MFSK8)                    \
*/ ELEM_(                                                        \
      57, MFSK16, MODE_MFSK16)                                                                                              \
      ELEM_(147, MFSK32,                                                                                                    \
            MODE_MFSK32) /*                                                                                                 \
                                 ELEM_(148, MFSK11, MODE_MFSK11)                                                            \
                                 ELEM_(152, MFSK22, MODE_MFSK22)                                                            \
                                                                                                                            \
                                 ELEM_(61, RTTYM_8_250, NUM_MODES)                                                          \
                                 ELEM_(62, RTTYM_16_500, NUM_MODES) \s                                                      \
                                 ELEM_(63, RTTYM_32_1000, NUM_MODES)                                                        \
                                 ELEM_(65, RTTYM_8_500, NUM_MODES)                                                          \
                                 ELEM_(66, RTTYM_16_1000, NUM_MODES)                                                        \
                                 ELEM_(67, RTTYM_4_500, NUM_MODES)                                                          \
                                 ELEM_(68, RTTYM_4_250, NUM_MODES)                                                          \
                                 ELEM_(119, RTTYM_8_1000, NUM_MODES)                                                        \
                                 ELEM_(170, RTTYM_8_125, NUM_MODES)                                                         \
                                                                                                                            \
                                 ELEM_(203, OLIVIA_4_125, MODE_OLIVIA)   */                                                 \
      ELEM_(203, OLIVIA_4_125, MODE_OLIVIA_4_125)                                                                           \
          ELEM_(75, OLIVIA_4_250, MODE_OLIVIA_4_250)                                                                        \
              ELEM_(74, OLIVIA_4_500, MODE_OLIVIA_4_500)                                                                    \
                  ELEM_(229, OLIVIA_4_1000, MODE_OLIVIA_4_1000)                                                             \
                      ELEM_(238, OLIVIA_4_2000, MODE_OLIVIA_4_2000)                                                         \
                                                                                                                            \
      /*        ELEM_(163, OLIVIA_8_125, MODE_OLIVIA)                                                                       \
              ELEM_(163, OLIVIA_8_125, MODE_OLIVIA_8_125) */                                                                \
      ELEM_(69, OLIVIA_8_250, MODE_OLIVIA_8_250) ELEM_(72, OLIVIA_8_500,                                                    \
                                                       MODE_OLIVIA_8_500)                                                   \
          ELEM_(116, OLIVIA_8_1000, MODE_OLIVIA_8_1000) /*        ELEM_(214,                                                \
                                                         * OLIVIA_8_2000,                                                   \
                                                         * MODE_OLIVIA)                                                     \
                                                        */                                                                  \
      ELEM_(214, OLIVIA_8_2000, MODE_OLIVIA_8_2000)                                                                         \
          ELEM_(70, OLIVIA_16_500,                                                                                          \
                MODE_OLIVIA_16_500) /*Android        ELEM_(73,                                                              \
                                       OLIVIA_16_1000, MODE_OLIVIA)                                                         \
                                            ELEM_(234, OLIVIA_16_2000,                                                      \
                                       MODE_OLIVIA)                                                                         \
                                    */                                                                                      \
      ELEM_(73, OLIVIA_16_1000, MODE_OLIVIA_16_1000)                                                                        \
          ELEM_(234, OLIVIA_16_2000, MODE_OLIVIA_16_2000)                                                                   \
                                                                                                                            \
              ELEM_(71, OLIVIA_32_1000,                                                                                     \
                    MODE_OLIVIA_32_1000) /*Android        ELEM_(221,                                                        \
                                            OLIVIA_32_2000, MODE_OLIVIA) */                                                 \
      ELEM_(221, OLIVIA_32_2000, MODE_OLIVIA_32_2000)                                                                       \
                                                                                                                            \
          ELEM_(211, OLIVIA_64_2000, MODE_OLIVIA_64_2000) /*        ELEM_(NO_RSID, OLIVIA, MODE_OLIVIA) \
                                                        \
        ELEM_(76, PAX, NUM_MODES)                       \
        ELEM_(77, PAX2, NUM_MODES)                      \
        ELEM_(78, DOMINOF, NUM_MODES)                   \
        ELEM_(79, FAX, NUM_MODES)                       \
        ELEM_(81, SSTV, NUM_MODES)                      \
                                                        \
*/ ELEM_(            \
              84, DOMINOEX4, MODE_DOMINOEX4) ELEM_(85, DOMINOEX5,                                                           \
                                                   MODE_DOMINOEX5)                                                          \
              ELEM_(86, DOMINOEX8, MODE_DOMINOEX8) ELEM_(                                                                   \
                  87, DOMINOEX11, MODE_DOMINOEX11) ELEM_(88, DOMINOEX16,                                                    \
                                                         MODE_DOMINOEX16)                                                   \
                  ELEM_(90, DOMINOEX22, MODE_DOMINOEX22) /*        ELEM_(92, DOMINOEX_4_FEC, MODE_DOMINOEX4)       \
        ELEM_(93, DOMINOEX_5_FEC, MODE_DOMINOEX5)       \
        ELEM_(97, DOMINOEX_8_FEC, MODE_DOMINOEX8)       \
        ELEM_(98, DOMINOEX_11_FEC, MODE_DOMINOEX11)     \
        ELEM_(99, DOMINOEX_16_FEC, MODE_DOMINOEX16)     \
        ELEM_(101, DOMINOEX_22_FEC, MODE_DOMINOEX22)    \
                                                        \
        ELEM_(104, FELD_HELL, MODE_FELDHELL)            \
        ELEM_(105, PSK_HELL, NUM_MODES)                 \
        ELEM_(106, HELL_80, MODE_HELL80)                \
        ELEM_(107, FM_HELL_105, MODE_FSKH105)           \
        ELEM_(108, FM_HELL_245, NUM_MODES)              \
                                                        \
        ELEM_(114, MODE_141A, NUM_MODES)                \
        ELEM_(123, DTMF, NUM_MODES)                     \
        ELEM_(125, ALE400, NUM_MODES)                   \
        ELEM_(131, FDMDV, NUM_MODES)                    \
                                                        \
        ELEM_(132, JT65_A, NUM_MODES)                   \
        ELEM_(134, JT65_B, NUM_MODES)                   \
        ELEM_(135, JT65_C, NUM_MODES)                   \
                                                        \
*/ ELEM_( \
                      136, THOR4, MODE_THOR4) ELEM_(137, THOR8, MODE_THOR8)                                                 \
                      ELEM_(138, THOR16, MODE_THOR16)                                                                       \
                          ELEM_(139, THOR5, MODE_THOR5) ELEM_(143, THOR11,                                                  \
                                                              MODE_THOR11)                                                  \
                              ELEM_(145, THOR22,                                                                            \
                                    MODE_THOR22) /*                                                                         \
                                                         ELEM_(153, CALL_ID,                                                \
                                                    NUM_MODES)                                                              \
                                                                                                                            \
                                                         ELEM_(155,                                                         \
                                                    PACKET_PSK1200, NUM_MODES)                                              \
                                                         ELEM_(156,                                                         \
                                                    PACKET_PSK250, NUM_MODES)                                               \
                                                         ELEM_(159,                                                         \
                                                    PACKET_PSK63, NUM_MODES)                                                \
                                                                                                                            \
                                                         ELEM_(172,                                                         \
                                                    MODE_188_110A_8N1,                                                      \
                                                    NUM_MODES)   */                                                         \
                                                                                                                            \
      /* NONE must be the last element */                                                                                   \
      ELEM_(0, NONE, NUM_MODES)

#define ELEM_(code_, tag_, mode_) RSID_##tag_ = code_,
enum { RSID_LIST };
#undef ELEM_

#define ELEM_(code_, tag_, mode_) {RSID_##tag_, mode_, #tag_},
const RSIDs cRsId::rsid_ids_1[] = {RSID_LIST};
#undef ELEM_

const int cRsId::rsid_ids_size1 = sizeof(rsid_ids_1) / sizeof(*rsid_ids_1) - 1;

//======================================================================
/*        ELEM_(6, ESCAPE2, NUM_MODES)                  \ */

#define RSID_LIST2                                                                                                  \
  ELEM2_(450, PSK63RX4, MODE_4X_PSK63R)                                                                             \
  ELEM2_(457, PSK63RX5, MODE_5X_PSK63R)                                                                             \
  ELEM2_(458, PSK63RX10, MODE_10X_PSK63R)                                                                           \
  ELEM2_(460, PSK63RX20, MODE_20X_PSK63R)                                                                           \
  ELEM2_(462, PSK63RX32, MODE_32X_PSK63R)                                                                           \
                                                                                                                    \
  ELEM2_(467, PSK125RX4, MODE_4X_PSK125R)                                                                           \
  ELEM2_(497, PSK125RX5, MODE_5X_PSK125R)                                                                           \
  ELEM2_(513, PSK125RX10, MODE_10X_PSK125R)                                                                         \
  ELEM2_(519, BPSK125X12, MODE_12X_PSK125)                                                                          \
  ELEM2_(522, PSK125RX12, MODE_12X_PSK125R)                                                                         \
  ELEM2_(527, PSK125RX16, MODE_16X_PSK125R)                                                                         \
                                                                                                                    \
  ELEM2_(529, PSK250RX2, MODE_2X_PSK250R)                                                                           \
  ELEM2_(533, PSK250RX3, MODE_3X_PSK250R)                                                                           \
  ELEM2_(539, PSK250RX5, MODE_5X_PSK250R)                                                                           \
  ELEM2_(541, PSK250X6, MODE_6X_PSK250)                                                                             \
  ELEM2_(545, PSK250RX6, MODE_6X_PSK250R)                                                                           \
  ELEM2_(551, PSK250RX7, MODE_7X_PSK250R)                                                                           \
                                                                                                                    \
  ELEM2_(553, PSK500RX2, MODE_2X_PSK500R)                                                                           \
  ELEM2_(558, PSK500RX3, MODE_3X_PSK500R)                                                                           \
  ELEM2_(564, PSK500RX4, MODE_4X_PSK500R)                                                                           \
  ELEM2_(566, BPSK500X2, MODE_2X_PSK500)                                                                            \
  ELEM2_(569, BPSK500X4, MODE_4X_PSK500)                                                                            \
                                                                                                                    \
  ELEM2_(570, BPSK1000, MODE_PSK1000)                                                                               \
  ELEM2_(580, PSK1000R, MODE_PSK1000R)                                                                              \
  ELEM2_(587, BPSK1000X2, MODE_2X_PSK1000)                                                                          \
  ELEM2_(595, PSK1000RX2, MODE_2X_PSK1000R)                                                                         \
  ELEM2_(604, PSK800RX2, MODE_2X_PSK800R)                                                                           \
  ELEM2_(610, PSK800X2, MODE_2X_PSK800)                                                                             \
                                                                                                                    \
  ELEM2_(620, MFSK64, MODE_MFSK64)                                                                                  \
  ELEM2_(625, MFSK128, MODE_MFSK128)                                                                                \
                                                                                                                    \
  ELEM2_(639, THOR25x4, MODE_THOR25x4)                                                                              \
  ELEM2_(649, THOR50x1, MODE_THOR50x1)                                                                              \
  ELEM2_(653, THOR50x2, MODE_THOR50x2)                                                                              \
  ELEM2_(658, THOR100, MODE_THOR100)                                                                                \
                                                                                                                    \
  ELEM2_(662, DOMINOEX44, MODE_DOMINOEX44)                                                                          \
  ELEM2_(681, DOMINOEX88, MODE_DOMINOEX88)                                                                          \
                                                                                                                    \
  ELEM2_(691, DOMINOEX_MICRO, MODE_DOMINOEXMICRO)                                                                   \
  ELEM2_(693, THOR_MICRO, MODE_THORMICRO)                                                                           \
                                                                                                                    \
  /*                                                      \
        ELEM2_(687, MFSK31, MODE_MFSK31)                \
        												\
*/ ELEM2_(                                                 \
      1026, MFSK64L,                                                                                                \
      MODE_MFSK64L) ELEM2_(1029, MFSK128L,                                                                          \
                           MODE_MFSK128L) ELEM2_(1066, 8PSK125,                                                     \
                                                 MODE_8PSK125) ELEM2_(1071,                                         \
                                                                      8PSK250,                                      \
                                                                      MODE_8PSK250)                                 \
      ELEM2_(1076, 8PSK500, MODE_8PSK500) /*        ELEM2_(1047, 8PSK1000, MODE_8PSK1000)           \
                                                        \
*/ ELEM2_(       \
          1037, 8PSK125F, MODE_8PSK125F) ELEM2_(1038, 8PSK250F, MODE_8PSK250F)                                      \
                                                                                                                    \
          ELEM2_(1043, 8PSK500F, MODE_8PSK500F) /*        ELEM2_(1078, 8PSK1000F, MODE_8PSK1000F)         \
        											    \
        ELEM2_(1079, 8PSK500FX2, MODE_8PSK500FX2)       \
        											    \
        ELEM2_(1058, PSK8P1200F, MODE_8PSK1200F)        \
                                                      \
*/ ELEM2_( \
              0, NONE2, NUM_MODES)

#define ELEM2_(code_, tag_, mode_) RSID_##tag_ = code_,
enum { RSID_LIST2 };
#undef ELEM2_

#define ELEM2_(code_, tag_, mode_) {RSID_##tag_, mode_, #tag_},
const RSIDs cRsId::rsid_ids_2[] = {RSID_LIST2};
#undef ELEM2_

const int cRsId::rsid_ids_size2 = sizeof(rsid_ids_2) / sizeof(*rsid_ids_2) - 1;

#define RSWINDOW 1

// Android Could not find the definition anywhere (used arbitrary numbers for
// NONE and NONE2)
#define RSID_NONE 9998
#define RSID_NONE2 9997
#define RSID_ESCAPE 6

// Android no reverse yet
bool bReverse = false;
char msg[100];

const int cRsId::Squares[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,
    3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 0,  2,  4,  6,  8,  10,
    12, 14, 9,  11, 13, 15, 1,  3,  5,  7,  0,  3,  6,  5,  12, 15, 10, 9,  1,
    2,  7,  4,  13, 14, 11, 8,  0,  4,  8,  12, 9,  13, 1,  5,  11, 15, 3,  7,
    2,  6,  10, 14, 0,  5,  10, 15, 13, 8,  7,  2,  3,  6,  9,  12, 14, 11, 4,
    1,  0,  6,  12, 10, 1,  7,  13, 11, 2,  4,  14, 8,  3,  5,  15, 9,  0,  7,
    14, 9,  5,  2,  11, 12, 10, 13, 4,  3,  15, 8,  1,  6,  0,  8,  9,  1,  11,
    3,  2,  10, 15, 7,  6,  14, 4,  12, 13, 5,  0,  9,  11, 2,  15, 6,  4,  13,
    7,  14, 12, 5,  8,  1,  3,  10, 0,  10, 13, 7,  3,  9,  14, 4,  6,  12, 11,
    1,  5,  15, 8,  2,  0,  11, 15, 4,  7,  12, 8,  3,  14, 5,  1,  10, 9,  2,
    6,  13, 0,  12, 1,  13, 2,  14, 3,  15, 4,  8,  5,  9,  6,  10, 7,  11, 0,
    13, 3,  14, 6,  11, 5,  8,  12, 1,  15, 2,  10, 7,  9,  4,  0,  14, 5,  11,
    10, 4,  15, 1,  13, 3,  8,  6,  7,  9,  2,  12, 0,  15, 7,  8,  14, 1,  9,
    6,  5,  10, 2,  13, 11, 4,  12, 3};

const int cRsId::indices[] = {2, 4, 8, 9, 11, 15, 7, 14, 5, 10, 13, 3};

cRsId::cRsId(double _wpm, double _cwrisetime, int _rtty_baud,
             bool _rtty_shaped) {
  // Android Added access to progdefaults(preferences) in the Java side
  progdefaults.RsID_label_type = getPreferenceI("RSID_ERRORS", 2);
  progdefaults.rsidWideSearch = getPreferenceB("RSIDWIDESEARCH", true);

  wpm = _wpm;
  cwrisetime = _cwrisetime;
  rtty_baud = _rtty_baud;
  rtty_shaped = _rtty_shaped;

  int error;
  /* Not yet in C++

  src_state = src_new(aults.sample_converter, 1, &error);
          if (error) {
                  LOG_ERROR("src_new error %d: %s", error, src_strerror(error));
                  abort();
          }
          src_data.end_of_input = 0;
   */
  reset();

  // Android rsfft = new g_fft<rs_fft_type>(RSID_ARRAY_SIZE);
  rsfft = new Cfft(RSID_FFT_SIZE);

  memset(fftwindow, 0, sizeof(fftwindow));

  if (RSWINDOW) {
    for (int i = 0; i < RSID_ARRAY_SIZE; i++)
      //		fftwindow[i] = blackman ( 1.0 * i / RSID_ARRAY_SIZE );
      fftwindow[i] = hamming(1.0 * i / RSID_ARRAY_SIZE);
    //		fftwindow[i] = hanning ( 1.0 * i / RSID_ARRAY_SIZE );
    //		fftwindow[i] = 1.0;
  }

  pCodes1 = new unsigned char[rsid_ids_size1 * RSID_NSYMBOLS];
  memset(pCodes1, 0, sizeof(pCodes1) * sizeof(unsigned char));

  pCodes2 = new unsigned char[rsid_ids_size2 * RSID_NSYMBOLS];
  memset(pCodes2, 0, sizeof(pCodes2) * sizeof(unsigned char));

  // Initialization  of assigned mode/submode IDs.
  unsigned char *c;
  for (int i = 0; i < rsid_ids_size1; i++) {
    c = pCodes1 + i * RSID_NSYMBOLS;
    Encode(rsid_ids_1[i].rs, c);
  }

  for (int i = 0; i < rsid_ids_size2; i++) {
    c = pCodes2 + i * RSID_NSYMBOLS;
    Encode(rsid_ids_2[i].rs, c);
  }

#if 0
	printf("pcode 1\n");
	printf(",rs, name, mode,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14\n");
	for (int i = 0; i < rsid_ids_size1; i++) {
		printf("%d,%d,%s,%d", i, rsid_ids_1[i].rs, rsid_ids_1[i].name, rsid_ids_1[i].mode);
		for (int j = 0; j < RSID_NSYMBOLS + 1; j++)
			printf(",%d", pCodes1[i*(RSID_NSYMBOLS + 1) + j]);
		printf("\n");
	}
	printf("\npcode 2\n");
	printf(", rs, name, mode,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14\n");
	for (int i = 0; i < rsid_ids_size2; i++) {
		printf("%d,%d,%s,%d", i, rsid_ids_2[i].rs, rsid_ids_2[i].name, rsid_ids_2[i].mode);
		for (int j = 0; j < RSID_NSYMBOLS + 1; j++)
			printf(",%d", pCodes2[i*(RSID_NSYMBOLS+ 1) + j]);
		printf("\n");
	}
#endif

  nBinLow = 3;
  nBinHigh = RSID_FFT_SIZE - 32; // - RSID_NTIMES - 2

  outbuf = 0;
  symlen = 0;

  reset();
}

cRsId::~cRsId() {
  delete[] pCodes1;
  delete[] pCodes2;

  delete[] outbuf;
  delete rsfft;
  // Android not yet in C++ 	src_delete(src_state);
}

void cRsId::reset() {
  iPrevDistance = iPrevDistance2 = 99;
  bPrevTimeSliceValid = bPrevTimeSliceValid2 = false;
  found1 = found2 = false;
  rsid_secondary_time_out = 0;

  memset(aInputSamples, 0, (RSID_ARRAY_SIZE * 2) * sizeof(float));
  memset(aFFTReal, 0, RSID_ARRAY_SIZE * sizeof(rs_fft_type));
  memset(aFFTAmpl, 0, RSID_FFT_SIZE * sizeof(rs_fft_type));
  memset(fft_buckets, 0, RSID_NTIMES * RSID_FFT_SIZE * sizeof(int));

  /*Android not yet in C++	int error = src_reset(src_state);
          if (error)
                  LOG_ERROR("src_reset error %d: %s", error,
     src_strerror(error));
          src_data.src_ratio = 0.0;
  */

  inptr = RSID_FFT_SIZE;
  // Android fixed for testing	hamming_resolution = 4;
  hamming_resolution = progdefaults.RsID_label_type;
  // reset counter of ffts for waterfall
  fftCounter = 0;
}

void cRsId::Encode(int code, unsigned char *rsid) {
  rsid[0] = code >> 8;
  rsid[1] = (code >> 4) & 0x0f;
  rsid[2] = code & 0x0f;
  for (int i = 3; i < RSID_NSYMBOLS; i++)
    rsid[i] = 0;
  for (int i = 0; i < 12; i++) {
    for (int j = RSID_NSYMBOLS - 1; j > 0; j--)
      rsid[j] = rsid[j - 1] ^ Squares[(rsid[j] << 4) + indices[i]];
    rsid[0] = Squares[(rsid[0] << 4) + indices[i]];
  }
}

void cRsId::CalculateBuckets(const rs_fft_type *pSpectrum, int iBegin,
                             int iEnd) {
  rs_fft_type Amp = 0.0, AmpMax = 0.0;
  int iBucketMax = iBegin - 2;
  int j;

  for (int i = iBegin; i < iEnd; i += 2) {
    if (iBucketMax == i - 2) {
      AmpMax = pSpectrum[i];
      iBucketMax = i;
      for (j = i + 2; j < i + RSID_NTIMES + 2; j += 2) {
        Amp = pSpectrum[j];
        if (Amp > AmpMax) {
          AmpMax = Amp;
          iBucketMax = j;
        }
      }
    } else {
      j = i + RSID_NTIMES;
      Amp = pSpectrum[j];
      if (Amp > AmpMax) {
        AmpMax = Amp;
        iBucketMax = j;
      }
    }
    fft_buckets[RSID_NTIMES - 1][i] = (iBucketMax - i) >> 1;
  }
}

// Android add flag for rx RSID as we want to keep the FFT processing for the
// waterfall
// void cRsId::receive(const float* buf, size_t len)
void cRsId::receive(const float *buf, size_t len, bool doSearch) {

  if (len == 0)
    return;

  int srclen = static_cast<int>(len);
  // Android not in C++ yet as we resample to 11025Hz in Java for the time being
  //****Warning may be used below for memmove??  double src_ratio =
  // RSID_SAMPLE_RATE / active_modem->get_samplerate();

  if (rsid_secondary_time_out > 0) {
    // Android fixed for now		rsid_secondary_time_out -= (int)(len /
    // src_ratio);
    rsid_secondary_time_out -= (int)(len);
    if (rsid_secondary_time_out <= 0) {
      // Android not yet			LOG_INFO("%s", "Secondary RsID
      // timed out");
      reset();
    }
  }
  /*Android not in C++ yet as we resample in Java
          if (src_data.src_ratio != src_ratio) {
                  src_data.src_ratio = src_ratio;
                  src_set_ratio(src_state, src_data.src_ratio);
          }
  */

  // Android Just copy into buffer since it is already re-sampled to 11025Hz
  // (this allows the re-use
  //   of the resampled data for other modems like mfsk and Thor rather than
  //   re-sample twice)

  while (srclen > 0) {
    /*		src_data.data_in = const_cast<float*>(buf);
                    src_data.input_frames = srclen;
                    src_data.data_out = &aInputSamples[inptr];
                    src_data.output_frames = RSID_ARRAY_SIZE * 2 - inptr;
                    src_data.input_frames_used = 0;
                    int error = src_process(src_state, &src_data);
                    if (unlikely(error)) {
                            LOG_ERROR("src_process error %d: %s", error,
       src_strerror(error));
                            return;
                    }
                    size_t gend = src_data.output_frames_gen;
                    size_t used = src_data.input_frames_used;
                    inptr += gend;
                    buf += used;
                    srclen -= used;
    */
    size_t toCopy = RSID_ARRAY_SIZE * 2 - inptr;
    if (toCopy > srclen) {
      toCopy = srclen;
    }
    memcpy(&aInputSamples[inptr], buf, toCopy * sizeof(float));
    inptr += toCopy;
    buf += toCopy;
    srclen -= toCopy;

    while (inptr >= RSID_ARRAY_SIZE) {
      search(doSearch);
      memmove(&aInputSamples[0], &aInputSamples[RSID_FFT_SAMPLES],
              (RSID_BUFFER_SIZE - RSID_FFT_SAMPLES) * sizeof(float));
      inptr -= RSID_FFT_SAMPLES;
    }
  }
}

// Android conditional search
// void cRsId::search(void)
void cRsId::search(bool doSearch) {
  if (progdefaults.rsidWideSearch) {
    nBinLow = 3;
    nBinHigh = RSID_FFT_SIZE - 32;
  } else {
    float centerfreq = active_modem->get_freq();
    float bpf = 1.0 * RSID_ARRAY_SIZE / RSID_SAMPLE_RATE;
    nBinLow = (int)((centerfreq - 100.0 * 2) * bpf);
    nBinHigh = (int)((centerfreq + 100.0 * 2) * bpf);
  }
  if (nBinLow < 3)
    nBinLow = 3;
  if (nBinHigh > RSID_FFT_SIZE - 32)
    nBinHigh = RSID_FFT_SIZE - 32;

  // Android not yet	bool bReverse = !(wf->Reverse() ^ wf->USB());
  if (bReverse) {
    nBinLow = RSID_FFT_SIZE - nBinHigh;
    nBinHigh = RSID_FFT_SIZE - nBinLow;
  }

  if (RSWINDOW) {
    for (int i = 0; i < RSID_ARRAY_SIZE; i++)
      // Android aFFTcmplx[i] = cmplx(aInputSamples[i] * fftwindow[i], 0);
      aFFTReal[i] = (double)aInputSamples[i] * fftwindow[i];
  } else {
    for (int i = 0; i < RSID_ARRAY_SIZE; i++)
      // Android			aFFTReal[i] = cmplx(aInputSamples[i],
      // 0);
      aFFTReal[i] = (double)aInputSamples[i];
  }
  // Android
  memset(aFFTReal + RSID_FFT_SIZE, 0, RSID_FFT_SIZE * sizeof(double));

  // Android using old fft routines	rsfft->ComplexFFT(aFFTcmplx);

  rsfft->rdft(aFFTReal);

  memset(aFFTAmpl, 0, sizeof(aFFTAmpl));

  static const double pscale = 4.0 / (RSID_FFT_SIZE * RSID_FFT_SIZE);

  /*Android using old fft routines
          if (unlikely(bReverse)) {
                  for (int i = 0; i < RSID_FFT_SIZE; i++)
                          aFFTAmpl[RSID_FFT_SIZE - 1 - i] = norm(aFFTcmplx[i]) *
     pscale;
          } else {
                  for (int i = 0; i < RSID_FFT_SIZE; i++)
                          aFFTAmpl[i] = norm(aFFTcmplx[i]) * pscale;
          }
  */
  double Real, Imag;
  for (int i = 0; i < RSID_FFT_SIZE; i++) {
    Real = aFFTReal[2 * i];
    Imag = aFFTReal[2 * i + 1];
    if (bReverse)
      aFFTAmpl[RSID_FFT_SIZE - 1 - i] = Real * Real + Imag * Imag;
    else
      aFFTAmpl[i] = Real * Real + Imag * Imag;
  }

  // Waterfall needs a new array of amplitudes?
  // Copy to Waterfall array only if the previous dataset has been used
  // Get access to Java boolean static variable newAmplReady of the Java Modem
  // class
  bool newAmplReady = getNewAmplReady();

  if (!newAmplReady) {
    if (fftCounter > 1) { // Once every two FFTs since we have a new FFT for
                          // every half period
      fftCounter = 0;
      // Use by Modem_interface code
      //updateWaterfallBuffer(aFFTAmpl);
    } else
      fftCounter++;
  }

  // Android Added conditional search (we keep the FFT processing for the
  // waterfall display)
  if (doSearch) {

    int bucket_low = 3;
    int bucket_high = RSID_FFT_SIZE - 32;
    if (bReverse) {
      bucket_low = RSID_FFT_SIZE - bucket_high;
      bucket_high = RSID_FFT_SIZE - bucket_low;
    }

    memmove(fft_buckets, &(fft_buckets[1][0]),
            (RSID_NTIMES - 1) * RSID_FFT_SIZE * sizeof(int));
    memset(&(fft_buckets[RSID_NTIMES - 1][0]), 0, RSID_FFT_SIZE * sizeof(int));

    CalculateBuckets(aFFTAmpl, bucket_low, bucket_high - RSID_NTIMES);
    CalculateBuckets(aFFTAmpl, bucket_low + 1, bucket_high - RSID_NTIMES);

    int symbol_out_1 = -1;
    int bin_out_1 = -1;
    int symbol_out_2 = -1;
    int bin_out_2 = -1;

    if (rsid_secondary_time_out == 0) {
      found1 = search_amp(bin_out_1, symbol_out_1, pCodes1);
      if (found1) {
        if (symbol_out_1 != RSID_ESCAPE) {
          if (bReverse)
            bin_out_1 = 1024 - bin_out_1 - 31;
          apply(bin_out_1, symbol_out_1, 0);
          reset();
          return;
        } else {
          rsid_secondary_time_out = 3 * 15 * 1024;
          return;
        }
      } else
        return;
    }

    found2 = search_amp(bin_out_2, symbol_out_2, pCodes2);
    if (found2) {
      if (symbol_out_2 != RSID_NONE2) {
        if (bReverse)
          bin_out_2 = 1024 - bin_out_2 - 31;
        apply(bin_out_2, symbol_out_2, 1);
      }
      reset();
    }
  }
}

void cRsId::setup_mode(int iSymbol) {
  switch (iSymbol) {
  /*	case RSID_RTTY_ASCII_7:
                  progdefaults.rtty_baud = 5;
                  progdefaults.rtty_bits = 1;
                  progdefaults.rtty_shift = 9;
                  REQ(&set_rtty_tab_widgets);
                  break;
          case RSID_RTTY_ASCII_8:
                  progdefaults.rtty_baud = 5;
                  progdefaults.rtty_bits = 2;
                  progdefaults.rtty_shift = 9;
                  REQ(&set_rtty_tab_widgets);
                  break;
          case RSID_RTTY_45:
                  progdefaults.rtty_baud = 1;
                  progdefaults.rtty_bits = 0;
                  progdefaults.rtty_shift = 3;
                  REQ(&set_rtty_tab_widgets);
                  break;
          case RSID_RTTY_50:
                  progdefaults.rtty_baud = 2;
                  progdefaults.rtty_bits = 0;
                  progdefaults.rtty_shift = 3;
                  REQ(&set_rtty_tab_widgets);
                  break;
          case RSID_RTTY_75:
                  progdefaults.rtty_baud = 4;
                  progdefaults.rtty_bits = 0;
                  progdefaults.rtty_shift = 9;
                  REQ(&set_rtty_tab_widgets);
                  break;
  // DominoEX / FEC
          case RSID_DOMINOEX_4: case RSID_DOMINOEX_5: case RSID_DOMINOEX_8:
          case RSID_DOMINOEX_11: case RSID_DOMINOEX_16: case RSID_DOMINOEX_22:
                  progdefaults.DOMINOEX_FEC = false;
                  REQ(&set_dominoex_tab_widgets);
                  break;
          case RSID_DOMINOEX_4_FEC: case RSID_DOMINOEX_5_FEC: case
  RSID_DOMINOEX_8_FEC:
          case RSID_DOMINOEX_11_FEC: case RSID_DOMINOEX_16_FEC: case
  RSID_DOMINOEX_22_FEC:
                  progdefaults.DOMINOEX_FEC = true;
                  REQ(&set_dominoex_tab_widgets);
                  break;
  // olivia parameters
          case RSID_OLIVIA_4_125:
                  progdefaults.oliviatones = 1;
                  progdefaults.oliviabw = 0;
                  REQ(&set_olivia_tab_widgets);
                  break;
          case RSID_OLIVIA_4_250:
                  progdefaults.oliviatones = 1;
                  progdefaults.oliviabw = 1;
  //Android		REQ(&set_olivia_tab_widgets);
                  break;
          case RSID_OLIVIA_4_500:
                  progdefaults.oliviatones = 1;
                  progdefaults.oliviabw = 2;
  //Android		REQ(&set_olivia_tab_widgets);
                  break;
          case RSID_OLIVIA_4_1000:
                  progdefaults.oliviatones = 1;
                  progdefaults.oliviabw = 3;
                  REQ(&set_olivia_tab_widgets);
                  break;
          case RSID_OLIVIA_4_2000:
                  progdefaults.oliviatones = 1;
                  progdefaults.oliviabw = 4;
                  REQ(&set_olivia_tab_widgets);
                  break;
          case RSID_OLIVIA_8_125:
                  progdefaults.oliviatones = 2;
                  progdefaults.oliviabw = 0;
  //Android		REQ(&set_olivia_tab_widgets);
                  break;
          case RSID_OLIVIA_8_250:
                  progdefaults.oliviatones = 2;
                  progdefaults.oliviabw = 1;
  //Android		REQ(&set_olivia_tab_widgets);
                  break;
          case RSID_OLIVIA_8_500:
                  progdefaults.oliviatones = 2;
                  progdefaults.oliviabw = 2;
                  REQ(&set_olivia_tab_widgets);
                  break;
          case RSID_OLIVIA_8_1000:
                  progdefaults.oliviatones = 2;
                  progdefaults.oliviabw = 3;
                  REQ(&set_olivia_tab_widgets);
                  break;
          case RSID_OLIVIA_8_2000:
                  progdefaults.oliviatones = 2;
                  progdefaults.oliviabw = 4;
                  REQ(&set_olivia_tab_widgets);
                  break;
          case RSID_OLIVIA_16_500:
                  progdefaults.oliviatones = 3;
                  progdefaults.oliviabw = 2;
                  REQ(&set_olivia_tab_widgets);
                  break;
          case RSID_OLIVIA_16_1000:
                  progdefaults.oliviatones = 3;
                  progdefaults.oliviabw = 3;
                  REQ(&set_olivia_tab_widgets);
                  break;
          case RSID_OLIVIA_16_2000:
                  progdefaults.oliviatones = 3;
                  progdefaults.oliviabw = 4;
                  REQ(&set_olivia_tab_widgets);
                  break;
          case RSID_OLIVIA_32_1000:
                  progdefaults.oliviatones = 4;
                  progdefaults.oliviabw = 3;
                  REQ(&set_olivia_tab_widgets);
                  break;
          case RSID_OLIVIA_32_2000:
                  progdefaults.oliviatones = 4;
                  progdefaults.oliviabw = 4;
                  REQ(&set_olivia_tab_widgets);
                  break;
          case RSID_OLIVIA_64_2000:
                  progdefaults.oliviatones = 5;
                  progdefaults.oliviabw = 4;
                  REQ(&set_olivia_tab_widgets);
                  break;
  // contestia parameters
          case RSID_CONTESTIA_4_125:
                  progdefaults.contestiatones = 1;
                  progdefaults.contestiabw = 0;
                  REQ(&set_contestia_tab_widgets);
                  break;
          case RSID_CONTESTIA_4_250:
                  progdefaults.contestiatones = 1;
                  progdefaults.contestiabw = 1;
                  REQ(&set_contestia_tab_widgets);
                  break;
          case RSID_CONTESTIA_4_500:
                  progdefaults.contestiatones = 1;
                  progdefaults.contestiabw = 2;
                  REQ(&set_contestia_tab_widgets);
                  break;
          case RSID_CONTESTIA_4_1000:
                  progdefaults.contestiatones = 1;
                  progdefaults.contestiabw = 3;
                  REQ(&set_contestia_tab_widgets);
                  break;
          case RSID_CONTESTIA_4_2000:
                  progdefaults.contestiatones = 1;
                  progdefaults.contestiabw = 4;
                  REQ(&set_contestia_tab_widgets);
                  break;
          case RSID_CONTESTIA_8_125:
                  progdefaults.contestiatones = 2;
                  progdefaults.contestiabw = 0;
                  REQ(&set_contestia_tab_widgets);
                  break;
          case RSID_CONTESTIA_8_250:
                  progdefaults.contestiatones = 2;
                  progdefaults.contestiabw = 1;
                  REQ(&set_contestia_tab_widgets);
                  break;
          case RSID_CONTESTIA_8_500:
                  progdefaults.contestiatones = 2;
                  progdefaults.contestiabw = 2;
                  REQ(&set_contestia_tab_widgets);
                  break;
          case RSID_CONTESTIA_8_1000:
                  progdefaults.contestiatones = 2;
                  progdefaults.contestiabw = 3;
                  REQ(&set_contestia_tab_widgets);
                  break;
          case RSID_CONTESTIA_8_2000:
                  progdefaults.contestiatones = 2;
                  progdefaults.contestiabw = 4;
                  REQ(&set_contestia_tab_widgets);
                  break;
          case RSID_CONTESTIA_16_500:
                  progdefaults.contestiatones = 3;
                  progdefaults.contestiabw = 2;
                  REQ(&set_contestia_tab_widgets);
                  break;
          case RSID_CONTESTIA_16_1000:
                  progdefaults.contestiatones = 3;
                  progdefaults.contestiabw = 3;
                  REQ(&set_contestia_tab_widgets);
                  break;
          case RSID_CONTESTIA_16_2000:
                  progdefaults.contestiatones = 3;
                  progdefaults.contestiabw = 4;
                  REQ(&set_contestia_tab_widgets);
                  break;
          case RSID_CONTESTIA_32_1000:
                  progdefaults.contestiatones = 4;
                  progdefaults.contestiabw = 3;
                  REQ(&set_contestia_tab_widgets);
                  break;
          case RSID_CONTESTIA_32_2000:
                  progdefaults.contestiatones = 4;
                  progdefaults.contestiabw = 4;
                  REQ(&set_contestia_tab_widgets);
                  break;
          case RSID_CONTESTIA_64_500:
                  progdefaults.contestiatones = 5;
                  progdefaults.contestiabw = 2;
                  REQ(&set_contestia_tab_widgets);
                  break;
          case RSID_CONTESTIA_64_1000:
                  progdefaults.contestiatones = 5;
                  progdefaults.contestiabw = 3;
                  REQ(&set_contestia_tab_widgets);
                  break;
          case RSID_CONTESTIA_64_2000:
                  progdefaults.contestiatones = 5;
                  progdefaults.contestiabw = 4;
                  REQ(&set_contestia_tab_widgets);
                  break;
  */
  default:
    break;
  } // switch (iSymbol)
}

void cRsId::apply(int iBin, int iSymbol, int extended) {
  // Android	ENSURE_THREAD(TRX_TID);

  double rsidfreq = 0, currfreq = 0;
  int n, mbin = NUM_MODES;

  int tblsize;
  const RSIDs *p_rsid;

  if (extended) {
    tblsize = rsid_ids_size2;
    p_rsid = rsid_ids_2;
  } else {
    tblsize = rsid_ids_size1;
    p_rsid = rsid_ids_1;
  }

  currfreq = active_modem->get_freq();
  rsidfreq = (iBin + RSID_NSYMBOLS - 0.5) * RSID_SAMPLE_RATE / 2048.0;

  for (n = 0; n < tblsize; n++) {
    if (p_rsid[n].rs == iSymbol) {
      mbin = p_rsid[n].mode;
      break;
    }
  }

  if (mbin == NUM_MODES) {
    char msg[50];
    if (n < tblsize) // RSID known but unimplemented
      snprintf(msg, sizeof(msg), "RSID: %s unimplemented", p_rsid[n].name);
    else // RSID unknown; shouldn't  happen
      snprintf(msg, sizeof(msg), "RSID: code %d unknown", iSymbol);
    // Android		put_status(msg, 4.0);
    // Android		LOG_VERBOSE("%s", msg);
    return;
  }

  /*
  //Android Not here
          if (progdefaults.rsid_rx_modes.test(mbin)) {
                  LOG_VERBOSE("RSID: %s @ %0.1f Hz",
                          p_rsid[n].name, rsidfreq);
          }
          else {
                  LOG_DEBUG("Ignoring RSID: %s @ %0.1f Hz",
                          p_rsid[n].name, rsidfreq);
                  return;
          }

          if (mailclient || mailserver)
                  REQ(pskmail_notify_rsid, mbin);

          if (progdefaults.rsid_auto_disable)
                  REQ(toggleRSID);

          if (!progdefaults.disable_rsid_warning_dialog_box)
                  REQ(notify_rsid, mbin, rsidfreq);

          if (progdefaults.rsid_notify_only) return;

          if (progdefaults.rsid_mark) // mark current modem & freq
                  REQ(note_qrg, false, "\nBefore RSID: ", "\n",
                          active_modem->get_mode(), 0LL, currfreq);

  */

  if (active_modem) // Currently only effects Olivia, Contestia and MT63.
    active_modem->rx_flush();

  // Android Not yet	setup_mode(iSymbol);

  snprintf(msg, sizeof(msg), "\nRSID: %s @ %0.1f Hz", p_rsid[n].name, rsidfreq);
  int ii = 0;
  while (ii < sizeof(msg) && msg[ii] != 'z') {
    put_rx_char(msg[ii]);
    ii++;
  }
  // Last  character of "Hz" above
  put_rx_char('z');

  /*Android if (progdefaults.rsid_squelch)
                  REQ(init_modem_squelch, mbin,
     progdefaults.disable_rsid_freq_change ? currfreq : rsidfreq);
          else
                  REQ(init_modem, mbin, progdefaults.disable_rsid_freq_change ?
     currfreq : rsidfreq);
  */

  change_CModem(p_rsid[n].mode, rsidfreq, wpm, cwrisetime, rtty_baud,
                rtty_shaped);
}

inline int cRsId::HammingDistance(int iBucket, unsigned char *p2) {
  int dist = 0;
  for (int i = 0, j = 1; i < RSID_NSYMBOLS; i++, j += 2) {
    if (fft_buckets[j][iBucket] != p2[i]) {
      ++dist;
      if (dist > hamming_resolution)
        break;
    }
  }
  return dist;
}

bool cRsId::search_amp(int &bin_out, int &symbol_out, unsigned char *pcode) {
  int i, j;
  int iDistanceMin = 1000; // infinity
  int iDistance = 1000;
  int iBin = -1;
  int iSymbol = -1;

  int tblsize;
  const RSIDs *prsid;

  if (pcode == pCodes1) {
    tblsize = rsid_ids_size1;
    prsid = rsid_ids_1;
  } else {
    tblsize = rsid_ids_size2;
    prsid = rsid_ids_2;
  }

  unsigned char *pc = 0;
  for (i = 0; i < tblsize; i++) {
    pc = pcode + i * RSID_NSYMBOLS;
    for (j = nBinLow; j < nBinHigh - RSID_NTIMES; j++) {
      iDistance = HammingDistance(j, pc);
      if (iDistance < iDistanceMin) {
        iDistanceMin = iDistance;
        iSymbol = prsid[i].rs;
        iBin = j;
        if (iDistanceMin == 0)
          break;
      }
    }
  }

  if (iDistanceMin <= hamming_resolution) {
    symbol_out = iSymbol;
    bin_out = iBin;
    return true;
  }

  return false;
}

//=============================================================================
// transmit rsid code for current mode
//=============================================================================

// Android int for the time being bool cRsId::assigned(trx_mode mode) {
bool cRsId::assigned(int mode) {

  rmode = RSID_NONE;
  rmode2 = RSID_NONE2;

  switch (mode) {
    /*	case MODE_RTTY :
                    if (progdefaults.rtty_baud == 5 && progdefaults.rtty_bits ==
       1 && progdefaults.rtty_shift == 9)
                            rmode = RSID_RTTY_ASCII_7;
                    else if (progdefaults.rtty_baud == 5 &&
       progdefaults.rtty_bits == 1 && progdefaults.rtty_shift == 9)
                            rmode = RSID_RTTY_ASCII_8;
                    else if (progdefaults.rtty_baud == 1 &&
       progdefaults.rtty_bits == 0 && progdefaults.rtty_shift == 3)
                            rmode = RSID_RTTY_45;
                    else if (progdefaults.rtty_baud == 2 &&
       progdefaults.rtty_bits == 0 && progdefaults.rtty_shift == 3)
                            rmode = RSID_RTTY_50;
                    else if (progdefaults.rtty_baud == 4 &&
       progdefaults.rtty_bits == 0 && progdefaults.rtty_shift == 9)
                            rmode = RSID_RTTY_75;
                    else
                            return false;
                    return true;
                    break;

            case MODE_OLIVIA:
            case MODE_OLIVIA_4_250:
            case MODE_OLIVIA_8_250:
            case MODE_OLIVIA_4_500:
            case MODE_OLIVIA_8_500:
            case MODE_OLIVIA_16_500:
            case MODE_OLIVIA_8_1000:
            case MODE_OLIVIA_16_1000:
            case MODE_OLIVIA_32_1000:
            case MODE_OLIVIA_64_2000:
                    if (progdefaults.oliviatones == 1 && progdefaults.oliviabw
       == 0)
                            rmode = RSID_OLIVIA_4_125;
                    else if (progdefaults.oliviatones == 1 &&
       progdefaults.oliviabw == 1)
                            rmode = RSID_OLIVIA_4_250;
                    else if (progdefaults.oliviatones == 1 &&
       progdefaults.oliviabw == 2)
                            rmode = RSID_OLIVIA_4_500;
                    else if (progdefaults.oliviatones == 1 &&
       progdefaults.oliviabw == 3)
                            rmode = RSID_OLIVIA_4_1000;
                    else if (progdefaults.oliviatones == 1 &&
       progdefaults.oliviabw == 4)
                            rmode = RSID_OLIVIA_4_2000;

                    else if (progdefaults.oliviatones == 2 &&
       progdefaults.oliviabw == 0)
                            rmode = RSID_OLIVIA_8_125;
                    else if (progdefaults.oliviatones == 2 &&
       progdefaults.oliviabw == 1)
                            rmode = RSID_OLIVIA_8_250;
                    else if (progdefaults.oliviatones == 2 &&
       progdefaults.oliviabw == 2)
                            rmode = RSID_OLIVIA_8_500;
                    else if (progdefaults.oliviatones == 2 &&
       progdefaults.oliviabw == 3)
                            rmode = RSID_OLIVIA_8_1000;
                    else if (progdefaults.oliviatones == 2 &&
       progdefaults.oliviabw == 4)
                            rmode = RSID_OLIVIA_8_2000;

                    else if (progdefaults.oliviatones == 3 &&
       progdefaults.oliviabw == 2)
                            rmode = RSID_OLIVIA_16_500;
                    else if (progdefaults.oliviatones == 3 &&
       progdefaults.oliviabw == 3)
                            rmode = RSID_OLIVIA_16_1000;
                    else if (progdefaults.oliviatones == 3 &&
       progdefaults.oliviabw == 4)
                            rmode = RSID_OLIVIA_16_2000;

                    else if (progdefaults.oliviatones == 4 &&
       progdefaults.oliviabw == 3)
                            rmode = RSID_OLIVIA_32_1000;
                    else if (progdefaults.oliviatones == 4 &&
       progdefaults.oliviabw == 4)
                            rmode = RSID_OLIVIA_32_2000;

                    else if (progdefaults.oliviatones == 5 &&
       progdefaults.oliviabw == 4)
                            rmode = RSID_OLIVIA_64_2000;

                    else
                            return false;
                    return true;
                    break;

            case MODE_CONTESTIA:

                    if (progdefaults.contestiatones == 1 &&
       progdefaults.contestiabw == 0)
                            rmode = RSID_CONTESTIA_4_125;
                    else if (progdefaults.contestiatones == 1 &&
       progdefaults.contestiabw == 1)
                            rmode = RSID_CONTESTIA_4_250;
                    else if (progdefaults.contestiatones == 1 &&
       progdefaults.contestiabw == 2)
                            rmode = RSID_CONTESTIA_4_500;
                    else if (progdefaults.contestiatones == 1 &&
       progdefaults.contestiabw == 3)
                            rmode = RSID_CONTESTIA_4_1000;
                    else if (progdefaults.contestiatones == 1 &&
       progdefaults.contestiabw == 4)
                            rmode = RSID_CONTESTIA_4_2000;

                    else if (progdefaults.contestiatones == 2 &&
       progdefaults.contestiabw == 0)
                            rmode = RSID_CONTESTIA_8_125;
                    else if (progdefaults.contestiatones == 2 &&
       progdefaults.contestiabw == 1)
                            rmode = RSID_CONTESTIA_8_250;
                    else if (progdefaults.contestiatones == 2 &&
       progdefaults.contestiabw == 2)
                            rmode = RSID_CONTESTIA_8_500;
                    else if (progdefaults.contestiatones == 2 &&
       progdefaults.contestiabw == 3)
                            rmode = RSID_CONTESTIA_8_1000;
                    else if (progdefaults.contestiatones == 2 &&
       progdefaults.contestiabw == 4)
                            rmode = RSID_CONTESTIA_8_2000;

                    else if (progdefaults.contestiatones == 3 &&
       progdefaults.contestiabw == 2)
                            rmode = RSID_CONTESTIA_16_500;
                    else if (progdefaults.contestiatones == 3 &&
       progdefaults.contestiabw == 3)
                            rmode = RSID_CONTESTIA_16_1000;
                    else if (progdefaults.contestiatones == 3 &&
       progdefaults.contestiabw == 4)
                            rmode = RSID_CONTESTIA_16_2000;

                    else if (progdefaults.contestiatones == 4 &&
       progdefaults.contestiabw == 3)
                            rmode = RSID_CONTESTIA_32_1000;
                    else if (progdefaults.contestiatones == 4 &&
       progdefaults.contestiabw == 4)
                            rmode = RSID_CONTESTIA_32_2000;

                    else if (progdefaults.contestiatones == 5 &&
       progdefaults.contestiabw == 2)
                            rmode = RSID_CONTESTIA_64_500;
                    else if (progdefaults.contestiatones == 5 &&
       progdefaults.contestiabw == 3)
                            rmode = RSID_CONTESTIA_64_1000;
                    else if (progdefaults.contestiatones == 5 &&
       progdefaults.contestiabw == 4)
                            rmode = RSID_CONTESTIA_64_2000;

                    else
                            return false;
                    return true;
                    break;

            case MODE_DOMINOEX4:
                    if (progdefaults.DOMINOEX_FEC)
                            rmode = RSID_DOMINOEX_4_FEC;
                    break;
            case MODE_DOMINOEX5:
                    if (progdefaults.DOMINOEX_FEC)
                            rmode = RSID_DOMINOEX_5_FEC;
                    break;
            case MODE_DOMINOEX8:
                    if (progdefaults.DOMINOEX_FEC)
                            rmode = RSID_DOMINOEX_8_FEC;
                    break;
            case MODE_DOMINOEX11:
                    if (progdefaults.DOMINOEX_FEC)
                            rmode = RSID_DOMINOEX_11_FEC;
                    break;
            case MODE_DOMINOEX16:
                    if (progdefaults.DOMINOEX_FEC)
                            rmode = RSID_DOMINOEX_16_FEC;
                    break;
            case MODE_DOMINOEX22:
                    if (progdefaults.DOMINOEX_FEC)
                            rmode = RSID_DOMINOEX_22_FEC;
                    break;

            case MODE_MT63_500S:
                    rmode = RSID_MT63_500_ST;
                    break;
            case MODE_MT63_500L:
                    rmode = RSID_MT63_500_LG;
                    break;
            case MODE_MT63_1000S:
                    rmode = RSID_MT63_1000_ST;
                    break;
            case MODE_MT63_1000L:
                    rmode = RSID_MT63_1000_LG;
                    break;
            case MODE_MT63_2000S:
                    rmode = RSID_MT63_2000_ST;
                    break;
            case MODE_MT63_2000L:
                    rmode = RSID_MT63_2000_LG;
                    break;
    */
  }

  // if rmode is still unset, look it up
  // Try secondary table first
  if (rmode == RSID_NONE) {
    for (size_t i = 0; i < sizeof(rsid_ids_2) / sizeof(*rsid_ids_2); i++) {
      if (mode == rsid_ids_2[i].mode) {
        rmode = RSID_ESCAPE;
        rmode2 = rsid_ids_2[i].rs;
        break;
      }
    }
    if (rmode2 == RSID_NONE2) {
      for (size_t i = 0; i < sizeof(rsid_ids_1) / sizeof(*rsid_ids_1); i++) {
        if (mode == rsid_ids_1[i].mode) {
          rmode = rsid_ids_1[i].rs;
          break;
        }
      }
    }
  }
  if (rmode == RSID_NONE) {
    // Android not yet		LOG_DEBUG("%s mode is not assigned an RSID",
    // mode_info[mode].sname);
    return false;
  }
  return true;
}

void cRsId::send(bool preRSID) {
  // Android int for the time being
  // trx_mode mode = active_modem->get_mode();
  int mode = active_modem->get_mode();

  /*	if (!progdefaults.rsid_tx_modes.test(mode)) {
                  LOG_DEBUG("Mode %s excluded, not sending RSID",
     mode_info[mode].sname);
                  return;
          }

          if (!progdefaults.rsid_post && !preRSID)
                  return;
  */
  if (!assigned(mode))
    return;

  unsigned char rsid[RSID_NSYMBOLS];
  double sr;
  size_t len;
  int iTone;
  double freq, phaseincr;
  double fr;
  double phase;

  Encode(rmode, rsid);
  sr = (double)active_modem->get_samplerate();
  len = (size_t)floor(RSID_SYMLEN * sr);
  // Android: no optimization here	if (unlikely(len != symlen)) {
  if (len != symlen) {
    symlen = len;
    delete[] outbuf;
    outbuf = new double[symlen];
  }

  // transmit 5 symbol periods of silence at beginning of rsid
  memset(outbuf, 0, symlen * sizeof(*outbuf));
  for (int i = 0; i < 5; i++)
    active_modem->ModulateXmtr(outbuf, symlen);

  // transmit sequence of 15 symbols (tones)
  fr = 1.0 * active_modem->get_txfreq() - (RSID_SAMPLE_RATE * 7 / 1024);
  phase = 0.0;

  for (int i = 0; i < 15; i++) {
    iTone = rsid[i];
    if (active_modem->get_reverse())
      iTone = 15 - iTone;
    freq = fr + iTone * RSID_SAMPLE_RATE / 1024;
    phaseincr = 2.0 * M_PI * freq / sr;

    for (size_t j = 0; j < symlen; j++) {
      phase += phaseincr;
      if (phase > 2.0 * M_PI)
        phase -= 2.0 * M_PI;
      outbuf[j] = sin(phase);
    }
    active_modem->ModulateXmtr(outbuf, symlen);
  }

  if (rmode == RSID_ESCAPE && rmode2 != RSID_NONE2) {
    // transmit 10 symbol periods of silence between rsid sequences
    memset(outbuf, 0, symlen * sizeof(*outbuf));
    for (int i = 0; i < 10; i++)
      active_modem->ModulateXmtr(outbuf, symlen);

    Encode(rmode2, rsid);
    sr = active_modem->get_samplerate();
    len = (size_t)floor(RSID_SYMLEN * sr);
    // Android		if (unlikely(len != symlen)) {
    if (len != symlen) {
      symlen = len;
      delete[] outbuf;
      outbuf = new double[symlen];
    }
    // transmit sequence of 15 symbols (tones)
    fr = 1.0 * active_modem->get_txfreq() - (RSID_SAMPLE_RATE * 7 / 1024);
    phase = 0.0;

    for (int i = 0; i < 15; i++) {
      iTone = rsid[i];
      if (active_modem->get_reverse())
        iTone = 15 - iTone;
      freq = fr + iTone * RSID_SAMPLE_RATE / 1024;
      phaseincr = 2.0 * M_PI * freq / sr;

      for (size_t j = 0; j < symlen; j++) {
        phase += phaseincr;
        if (phase > 2.0 * M_PI)
          phase -= 2.0 * M_PI;
        outbuf[j] = sin(phase);
      }
      active_modem->ModulateXmtr(outbuf, symlen);
    }
  }

  // 5 symbol periods of silence at end of transmission
  // and between RsID and the data signal
  int nperiods = 5;
  memset(outbuf, 0, symlen * sizeof(*outbuf));
  for (int i = 0; i < nperiods; i++)
    active_modem->ModulateXmtr(outbuf, symlen);
}
