// ----------------------------------------------------------------------------
//    mt63.cxx  --  MT63 modem for fldigi
//
// Copyright (C) 1999-2004 Pawel Jalocha, SP9VRC
// Copyright (c) 2007-2011 Dave Freese, W1HKJ
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

// Android #include "configuration.h"
// Android #include "fl_digi.h"
// Android #include "status.h"
// Android
#include "mt63.h"
#include "AndFlmsg_Fldigi_Interface.h"
#include "misc.h"
#include <string.h>

//------------------------------------------------------------------------------
// Android #include "threads.h"

// Android static pthread_mutex_t mt63_mutex = PTHREAD_MUTEX_INITIALIZER;
//------------------------------------------------------------------------------

using namespace std;
bool startflag = true;

// Android void mt63::tx_init(SoundBase *sb)
void mt63::tx_init() {
  // Android	guard_lock dsp_lock(&mt63_mutex);

  // Android	scard = sb;

  Tx->Preset(frequency, (int)bandwidth, Interleave == 64 ? 1 : 0);
  flush = Tx->DataInterleave;
  videoText();
  startflag = true;
}

void mt63::rx_init() {
  // Android	guard_lock dsp_lock(&mt63_mutex);

  Rx->Preset(frequency, (int)bandwidth, Interleave == 64 ? 1 : 0,
             long_integral ? 32 : 16);
  InpLevel->Preset(64.0, 0.75);
  escape = 0;
}

double peak = 0.0;

int mt63::tx_process() {
  rx_flush();
  // do not put above rx_flush()

  // Android	guard_lock dsp_lock(&mt63_mutex);

  int c;
  double maxval = 0;

  if (startflag == true) {
    startflag = false;
    if (progdefaults.mt63_usetones) {
      double maxval = 0.0;
      for (int i = 0; i < (bandwidth * progdefaults.mt63_tone_duration / 96);
           i++) {
        Tx->SendTune(progdefaults.mt63_twotones);
        for (int i = 0; i < Tx->Comb.Output.Len; i++)
          if (fabs(Tx->Comb.Output.Data[i]) > maxval)
            maxval = fabs(Tx->Comb.Output.Data[i]);
        for (int i = 0; i < Tx->Comb.Output.Len; i++)
          Tx->Comb.Output.Data[i] /= maxval;
        ModulateXmtr((Tx->Comb.Output.Data), Tx->Comb.Output.Len);
      }
    }

    for (int i = 0; i < Tx->DataInterleave; i++)
      Tx->SendChar(0);
  }

  c = get_tx_char();
  if (c == GET_TX_CHAR_ETX) {
    stopflag = true;
    flush = Tx->DataInterleave;
  }
  if (c == GET_TX_CHAR_NODATA || stopflag == true)
    c = 0;

  if (stopflag) {
    stopflag = false;
    while (--flush) {
      Tx->SendChar(0);
      for (int i = 0; i < Tx->Comb.Output.Len; i++)
        if (fabs(Tx->Comb.Output.Data[i]) > maxval)
          maxval = fabs(Tx->Comb.Output.Data[i]);
      for (int i = 0; i < Tx->Comb.Output.Len; i++) {
        Tx->Comb.Output.Data[i] /= maxval;
      }
      ModulateXmtr((Tx->Comb.Output.Data), Tx->Comb.Output.Len);
    }
    Tx->SendJam();
    for (int i = 0; i < Tx->Comb.Output.Len; i++)
      if (fabs(Tx->Comb.Output.Data[i]) > maxval)
        maxval = fabs(Tx->Comb.Output.Data[i]);
    for (int i = 0; i < Tx->Comb.Output.Len; i++)
      Tx->Comb.Output.Data[i] /= maxval;
    ModulateXmtr((Tx->Comb.Output.Data), Tx->Comb.Output.Len);
    cwid();
    return -1; /* we're done */
  }

  // Android (to Utf-8 bytes)
  c &= 0xff;

  if (c > 255 || (!progdefaults.mt63_8bit && c > 127))
    c = '.';

  int sendc = c;

  if (sendc > 127) {
    sendc &= 127;
    Tx->SendChar(127);
    for (int i = 0; i < Tx->Comb.Output.Len; i++)
      if (fabs(Tx->Comb.Output.Data[i]) > maxval)
        maxval = fabs(Tx->Comb.Output.Data[i]);
    for (int i = 0; i < Tx->Comb.Output.Len; i++)
      Tx->Comb.Output.Data[i] /= maxval;
    ModulateXmtr((Tx->Comb.Output.Data), Tx->Comb.Output.Len);
  }

  Tx->SendChar(sendc);
  for (int i = 0; i < Tx->Comb.Output.Len; i++)
    if (fabs(Tx->Comb.Output.Data[i]) > maxval)
      maxval = fabs(Tx->Comb.Output.Data[i]);
  for (int i = 0; i < Tx->Comb.Output.Len; i++) {
    Tx->Comb.Output.Data[i] /= maxval;
  }
  ModulateXmtr((Tx->Comb.Output.Data), Tx->Comb.Output.Len);

  put_echo_char(c);

  return 0;
}

// Android buffer of shorts now int mt63::rx_process(const double *buf, int len)
int mt63::rx_process(const short *buf, int len) {
  double snr;
  unsigned int c;
  int i;
  static char msg1[20];
  static char msg2[20];
  double f_offset;

  //	if (Interleave != progdefaults.mt63_interleave) {
  //		Interleave = progdefaults.mt63_interleave;
  //		restart();
  //	}
  if (long_integral != progdefaults.mt63_rx_integration) {
    long_integral = progdefaults.mt63_rx_integration;
    restart();
  }

  if (InpBuff->EnsureSpace(len) == -1) {
    // Android not yet		fprintf(stderr, "mt63_rxprocess: buffer
    // error\n");
    return -1;
  }

  for (i = 0; i < len; i++)
    // Android keep an eye on automatic casting here
    InpBuff->Data[i] = buf[i];

  { // critical section
    // Android		guard_lock dsp_lock(&mt63_mutex);

    InpBuff->Len = len;
    InpLevel->Process(InpBuff);

    Rx->Process(InpBuff);

    snr = Rx->FEC_SNR();

    //		if (progStatus.sqlonoff && snr < progStatus.sldrSquelchValue) {
    if (snr < sldrSquelchValue) {
      // Android put_Status1("");
      // Android put_Status2("");
      display_metric(0);
      return 0;
    }

    for (i = 0; i < Rx->Output.Len; i++) {
      c = Rx->Output.Data[i];
      if (!progdefaults.mt63_8bit) {
        put_rx_char(c);
        continue;
      }
      // Android: this is why the lower control characters are never sent.
      // Reason????
      if ((c < 8) && (escape == 0))
        continue;
      if (c == 127) {
        escape = 1;
        continue;
      }
      if (escape) {
        c += 128;
        escape = 0;
      }
      put_rx_char(c);
    }

    f_offset = Rx->TotalFreqOffset();

  } // end critical section

  if (snr > 99.9)
    snr = 99.9;

  display_metric(snr);

  double s2n = 10.0 * log10(snr == 0 ? 0.001 : snr);
  // Android	snprintf(msg1, sizeof(msg1), "s/n %2d dB", (int)(floor(s2n)));
  // Android    put_Status1(msg1);

  // Android    snprintf(msg2, sizeof(msg2), "f/o %+4.1f Hz", f_offset);
  // Android    put_Status2(msg2, 5, STATUS_CLEAR);

  flushbuffer = true;

  return 0;
}

void mt63::rx_flush() {
  //	guard_lock dsp_lock(&mt63_mutex);

  unsigned int c;
  int len = 512;
  int dlen = 0;

  if (!flushbuffer)
    return;

  if (emptyBuff->EnsureSpace(len) == -1) {
    flushbuffer = false;
    return;
  }

  for (int j = 0; j < len; j++)
    emptyBuff->Data[j] = 0.0;
  emptyBuff->Len = len;
  InpLevel->Process(emptyBuff);
  Rx->Process(emptyBuff);
  dlen = Rx->Output.Len;

  while (Rx->SYNC_LockStatus()) {
    for (int i = 0; i < dlen; i++) {
      c = Rx->Output.Data[i];
      if (!progdefaults.mt63_8bit) {
        put_rx_char(c);
        continue;
      }
      if ((c < 8) && (escape == 0))
        continue;
      if (c == 127) {
        escape = 1;
        continue;
      }
      if (escape) {
        c += 128;
        escape = 0;
      }
      put_rx_char(c);
    }
    for (int j = 0; j < len; j++)
      emptyBuff->Data[j] = 0.0;
    emptyBuff->Len = len;
    InpLevel->Process(emptyBuff);
    Rx->Process(emptyBuff);
    dlen = Rx->Output.Len;
  }
  flushbuffer = false;

  return;
}

void mt63::restart() {
  int err;

  // Android	put_MODEstatus(mode);
  // Android	set_scope_mode(Digiscope::BLANK);

  { // critical section
    // Android		guard_lock dsp_lock(&mt63_mutex);

    err = Tx->Preset(frequency, (int)bandwidth, Interleave == 64 ? 1 : 0);
    if (err)
      // Android not yet			fprintf(stderr, "mt63_txinit:
      // init
      // failed\n");
      flush = Tx->DataInterleave;

    err = Rx->Preset(frequency, (int)bandwidth, Interleave == 64 ? 1 : 0,
                     long_integral ? 32 : 16);
  } // end critical section
  if (err)
    // Android not yet		fprintf(stderr, "mt63_rxinit: init failed\n");
    InpLevel->Preset(64.0, 0.75);
  stopflag = false;
}

void mt63::init() {
  modem::init();
  restart();
  flushbuffer = false;
}

// Android mt63::mt63 (trx_mode mt63_mode) : modem()
mt63::mt63(int mt63_mode) : modem() {
  // Android Added access to progdefaults(preferences) in the Java side
  progdefaults.mt63_8bit = true; // getPreferenceB("MT638BIT", true);
  progdefaults.mt63_rx_integration =
      true;                          // getPreferenceB("MT63INTEGRATION", true);
  progdefaults.mt63_usetones = true; // getPreferenceB("MT63USETONES", true);
  progdefaults.mt63_twotones = true; // getPreferenceB("MT63TWOTONES", true);
  progdefaults.mt63_tone_duration = 4; // getPreferenceI("MT63TONEDURATION", 4);
  // Reversed logic for mt63_at500 (GUI says: "Allow Manual Tuning")
  progdefaults.mt63_at500 = false; //! getPreferenceB("MT63AT500", true);

  progdefaults.mt63_tone_duration =
      (int)clamp(progdefaults.mt63_tone_duration, 1.1, 10.1);

  mode = mt63_mode;
  switch (mode) {
  case MODE_MT63_500S:
    Interleave = 32;
    bandwidth = 500;
    break;
  case MODE_MT63_500L:
    Interleave = 64;
    bandwidth = 500;
    break;
  case MODE_MT63_1000S:
    Interleave = 32;
    bandwidth = 1000;
    break;
  case MODE_MT63_1000L:
    Interleave = 64;
    bandwidth = 1000;
    break;
  case MODE_MT63_2000S:
    Interleave = 32;
    bandwidth = 2000;
    break;
  case MODE_MT63_2000L:
    Interleave = 64;
    bandwidth = 2000;
    break;
  }
  //	Interleave = progdefaults.mt63_interleave;
  long_integral = progdefaults.mt63_rx_integration;

  Tx = new MT63tx;
  Rx = new MT63rx;

  InpLevel = new dspLevelMonitor;
  InpBuff = new double_buff;
  emptyBuff = new double_buff;

  samplerate = 8000;
  fragmentsize = 1024;

  // Android added for testing (maybe ok here with parameter)
  // set_freq(1500.0f);
}

mt63::~mt63() {
  // Android	guard_lock dsp_lock(&mt63_mutex);

  if (Tx)
    delete Tx;
  if (Rx)
    delete Rx;

  if (InpLevel)
    delete InpLevel;
  if (InpBuff)
    delete InpBuff;
}

// W1HKJ
// user can select manual or fixed positioning of the MT63 encoder/decoder
// progdefaults.mt63_at500 TRUE ==> fixed position
void mt63::set_freq(double f) {
  if (progdefaults.mt63_at500)
    frequency = 500 + bandwidth / 2;
  else
    frequency = f;

  modem::set_freq(frequency);
  rx_init();
}
