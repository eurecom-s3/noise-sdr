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
#include "contestia.h"
#include "cw.h"
#include "dominoex.h"
#include "lora.h"
#include "mfsk.h"
#include "mt63.h"
#include "olivia.h"
#include "psk.h"
#include "rsid.h"
#include "rtty.h"
#include "thor.h"
#include <complex.h>
#include <fcntl.h>
#include <fstream>
#include <math.h>
#include <time.h>
#ifdef LEAKY_OP_ION
#include "ion/ion.h"
#elif LEAKY_OP_STREAM
#include <emmintrin.h>
#elif LEAKY_OP_CNT
#define NSEC_PER_SEC 1000000000ull
typedef struct timespec mach_timespec_t;
typedef unsigned int mach_port_t;
#endif

#define BILLION 1000000000L
#ifdef SMALL
#define BUFSIZE 2560000
#define EDGESIZE 2560000
#else
#define BUFSIZE 32000000
#define EDGESIZE 32000000
#endif

using std::string;

cRsId *RsidModem = NULL;
modem *active_modem = NULL;
char decodedBuffer[5000];
char utf8PartialBuffer[4];
int utfExpectedChars = 0;
int utfFoundChars = 0;
int lastCharPos;
signed char *txDataBuffer = NULL;
int txDataBufferLength = 0;
int txCounter = 0;
// MFSK Picture Rx/Tx
int pictureRow[4096 * 3];
int pictureW = 0;
int pictureH = 0;

// VK2ETA Needs to be made bullet proof
double outputBuffer[BUFSIZE]; // 3 seconds of audio, in case we have a delay in
                              // the Java rx thread
int sizeOfOutputBuffer = sizeof(outputBuffer) / sizeof(int);
int outputBufferIndex = 0;

// Dummy for now
void put_rx_char(int receivedChar) {}

void androidShowRxViewer(int picW, int picH) {}

void androidSaveLastPicture() {}

void androidUpdateRxPic(int data, int pos) {}

bool getNewAmplReady() { return false; }

void updateWaterfallBuffer(double *processedFft) {}

string getPreferenceS2(string preferenceString, string defaultValue) {
  return NULL;
}

int getPreferenceI(string preferenceString, int defaultValue) { return -1; }

double getPreferenceD(string preferenceString, double defaultValue) {
  return -1;
}

bool getPreferenceB(string preferenceString, bool defaultValue) {
  return false;
}

//----- TX section of the C++ modems ----

// Echoe the transmitted characters
extern void put_echo_char(unsigned int txedChar) {
  printf("Transmit %c\n", txedChar);
}

// Get one character from the input buffer
extern int get_tx_char() {
  int returnValue;

  returnValue = GET_TX_CHAR_ETX;
  if (txDataBuffer != NULL) {
    if (txCounter < txDataBufferLength) {
      returnValue = (int)(*txDataBuffer++);
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

#ifdef LEAKY_OP_CIVAC
// Access the DRAM by LDR + CIVAC
__attribute__((naked)) void hammer_civac(uint64_t *addr) {
  __asm volatile("LDR X9, [X0] \r\n");
  __asm volatile("DC CIVAC, X0 \r\n");
  __asm volatile("DSB 0xB \r\n");
  __asm volatile("RET");
}
#elif LEAKY_OP_LOOP
void evict(uint32_t *addr, int len) {
    int u;
    for(int i=0; i<len; i+=4){
        u = *(volatile uint32_t*)(addr + i);
    }
}
#elif LEAKY_OP_ION
int ion_fd;
int len = 65536; //1024;
int chipset = -1;

// Access DRAM using ION
static inline void ion_leak(void) {
  ion_user_handle_t ion_handle;
  ion_alloc(ion_fd, len, 0, (0x1 << chipset), 0, &ion_handle);
  ion_free(ion_fd, ion_handle);
}
#elif LEAKY_OP_STREAM
// Access DRAM using mm stream
__m128i reg;
__m128i reg_zero;
__m128i reg_one;

void stream(void) {
  _mm_stream_si128(&reg, reg_one);
  _mm_stream_si128(&reg, reg_zero);
}
#elif LEAKY_OP_CNT
// system-bus-radio
static inline void clock_sleep_trap(mach_port_t clock_port, int sleep_type, time_t sec, long nsec, mach_timespec_t *remain) {
    mach_timespec_t req = { sec, nsec };
    int res = clock_nanosleep(sleep_type, CLOCK_MONOTONIC, &req, remain);
    if (res < 0) {
        perror("clock_nanosleep");
        exit(1);
    }
}
#endif

// save raw samples into a buffer
void txModulate(double *buffer, int len) {
  // Accumulate samples

  bool full = false;
  int i = 0;
  while (i < len && !full) {
    if (outputBufferIndex > BUFSIZE - 1) {
      full = true;
      printf("Output buffer full, skipping samples\n");
    } else {
      outputBuffer[outputBufferIndex++] = buffer[i++];
    }
  }
  // printf("Output buffer filled at %d out of %d\n", outputBufferIndex,
  // BUFSIZE);
}

double phaseacc = 0;
cmplx mixer(cmplx in, double f, int samplerate) {
  cmplx z;

  z = in * cmplx(cos(phaseacc), sin(phaseacc));

  phaseacc -= TWOPI * f / samplerate;
  if (phaseacc < 0)
    phaseacc += TWOPI;

  return z;
}

void RFPWM(bool forever, char *buffile, char *rfpwmfile) {
  printf("Starting coversion to RF-PMM\n");
  static uint64_t edges[EDGESIZE] = {0};
  int len = 0;
  int i = 0;
  int samplerate = active_modem->get_samplerate();
  // int samplerate_rfpwm = samplerate * OVERSAMPLING;
  // printf("samplerate %d, samplerate_rfpwm %d\n", samplerate,
  // samplerate_rfpwm);
  printf("samplerate %d\n", samplerate);

  // printf("Starting resampling with interpolation\n");
  // outputBufferResampledIndex = outputBufferIndex * OVERSAMPLING;
  // for (int i = 0; i < outputBufferIndex - 1; i++) {
  // for (int j = 0; j < OVERSAMPLING; j++) {
  // outputBufferResampled[i * OVERSAMPLING + j] = outputBuffer[i];
  // outputBufferResampled[i * OVERSAMPLING + j] +=
  // 1.0*j*(outputBuffer[i+1]-outputBuffer[i])/OVERSAMPLING;
  //}
  //}

  // printf("Starting float to complex conversion\n");
  // C_FIR_filter*xmtfilt;
  // xmtfilt = new C_FIR_filter();
  // xmtfilt->init_lowpass (127, 1, 500/samplerate);
  // phaseacc = 0;
  // for(int i=0; i<outputBufferIndex; i++){
  // c[i].re = outputBuffer[i];
  // c[i].im = 0;
  // c[i] = mixer(c[i], -1000, samplerate);
  // xmtfilt->Irun(c[i].re, c[i].re);
  // xmtfilt->Irun(abs(outputBuffer[i]), outputBuffer[i]);
  // printf("(%e+%ej)\n", outputBuffer[i], 0);
  // printf("(%e+%ej)\n", c[i].re, c[i].im);
  // a = c.mag();
  // a = asin(a)/M_PI;
  // phi = 0; //c.arg();
  // outputBufferResampled[i] = cos(2*M_PI*8000*i/samplerate_rfpwm+phi);
  //}

  // delete xmtfilt;

  // printf("Applying filter for USB\n");
  // fftfilt lpf(0.0/samplerate, 500.0/samplerate, outputBufferIndex);
  // cmplx *clpf;
  // for(int i=0; i<outputBufferIndex; i++){
  // lpf.run(c[i],&clpf);
  //}
  // for(int i=0; i<outputBufferIndex;i++){
  // printf("(%e+%ej)\n", clpf[i].re, clpf[i].im);
  ////printf("%f %f, %f %f\n", c[i].mag(), c[i].arg(), cpbpf[i].mag(),
  /// cpbpf[i].arg());
  //}

  printf("Starting RF-PMM conversion\n");
  while (i < outputBufferIndex && outputBuffer[i++] < 0) {
  }
  while (i < outputBufferIndex && len < EDGESIZE) {
    uint64_t j = 0;
    double a = outputBuffer[i];
    while (i + j < outputBufferIndex and outputBuffer[i + j] >= 0) {
      j++;
      if (outputBuffer[i + j] > a)
        a = outputBuffer[i + j];
    }
    while (i + j < outputBufferIndex and outputBuffer[i + j] < 0) {
      j++;
    }
    // printf("%f\n", a);
    if (len < EDGESIZE - 1) {
      edges[len++] = (asin(a) / M_PI) * (uint64_t)(1e9 * j / samplerate);
      edges[len++] = (uint64_t)(1e9 * j / samplerate);
    } else {
      printf("Edges full, skipping\n");
    }
    i += j;
  }
  // printf("len %d\n", len);

  if (rfpwmfile) {
    printf("Saving RFPWM edges\n");
    std::ofstream f;
    f.open(rfpwmfile);
    for (int i = 0; i < len; i += 2) {
      f << edges[i] << " " << edges[i + 1] << "\n";
    }
    f.close();
  }
  if (buffile) {
    printf("Saving output buffer\n");
    std::ofstream f;
    f.open(buffile);
    for (i = 0; i < outputBufferIndex; i++) {
      f << outputBuffer[i] << "\n";
    }
    f.close();
  }

  printf("Starting RF-PMM transmission with DRAM\n");

#ifdef LEAKY_OP_CIVAC
  // Address to hammer for the basic operation
  uint64_t variable = ~0;
  uint64_t *address = &variable;
#elif LEAKY_OP_LOOP
  uint32_t address[512];
#elif LEAKY_OP_ION
  ion_fd = ion_open();
  chipset = ion_magic_number(ion_fd, len);
  if (chipset == -1) {
    fprintf(stderr, "Chipset not found\n");
    abort();
  }
#elif LEAKY_OP_CNT
  volatile uint32_t cnt = 0;
  mach_port_t clock_port;
  mach_timespec_t remain;
#endif

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
#ifdef LEAKY_OP_CIVAC
        hammer_civac(address);
#elif LEAKY_OP_LOOP
        evict(address, 512);
#elif LEAKY_OP_ION
        ion_leak();
#elif LEAKY_OP_STREAM
        stream();
#elif LEAKY_OP_CNT
        cnt++;
#endif
      }
      // Low period
#ifdef LEAKY_OP_CNT
      clock_sleep_trap(clock_port, CLOCK_MONOTONIC, reset / NSEC_PER_SEC, reset % NSEC_PER_SEC, &remain);
#else
      while (get_ns() < reset) {
      };
#endif
      // Reset the staring point, using reset gives the better result
      // start = high_resolution_clock::now();
      start = reset;
    }
  } while (forever);

#ifdef LEAKY_OP_ION
  ion_close(ion_fd);
#endif

  outputBufferIndex = 0;
  // outputBufferResampledIndex = 0;
  printf("Output buffer empty");
}

// Flushes the end of the sound buffer
void flushTxOutputBuffer() { outputBufferIndex = 0; }
// void flushTxOutputBuffer() {
// if (outputBufferIndex > 0) {
////Find the Java class
// jclass cls = gEnv->FindClass("com/AndFlmsg/Modem");
////Find the static Java method (see signature)
// jmethodID mid = gEnv->GetStaticMethodID(cls, "txModulate", "([DI)V");
////Create a Java array and copy the C++ array into it
// jdoubleArray jBuffer = gEnv->NewDoubleArray(outputBufferIndex);
// gEnv->SetDoubleArrayRegion(jBuffer, 0, outputBufferIndex, outputBuffer);
////Call the method with the Java array
// gEnv->CallStaticVoidMethod(cls, mid, jBuffer, outputBufferIndex);
////Release the intermediate array and the other variables
// gEnv->DeleteLocalRef(jBuffer);
// gEnv->DeleteLocalRef(cls);
// outputBufferIndex = 0;
//}
//}

extern void change_CModem(int modemCode, double newFrequency, double wpm,
                          double cwrisetime, int rtty_baud, bool rtty_shaped) {
  // Delete previously created modem to recover memory used
  if (active_modem) {
    delete active_modem;
    active_modem = NULL;
  }
  // Now re-create with new mode
  if (modemCode >= MODE_PSK_FIRST && modemCode <= MODE_PSK_LAST) {
    active_modem = new psk(modemCode);
  } else if (modemCode >= MODE_DOMINOEX_FIRST &&
             modemCode <= MODE_DOMINOEX_LAST) {
    active_modem = new dominoex(modemCode);
  } else if (modemCode >= MODE_THOR_FIRST && modemCode <= MODE_THOR_LAST) {
    active_modem = new thor(modemCode);
  } else if (modemCode >= MODE_MFSK_FIRST && modemCode <= MODE_MFSK_LAST) {
    active_modem = new mfsk(modemCode);
  } else if (modemCode >= MODE_MT63_FIRST && modemCode <= MODE_MT63_LAST) {
    active_modem = new mt63(modemCode);
  } else if (modemCode >= MODE_OLIVIA_FIRST && modemCode <= MODE_OLIVIA_LAST) {
    active_modem = new olivia(modemCode);
  } else if (modemCode >= MODE_CONTESTIA_FIRST &&
             modemCode <= MODE_CONTESTIA_LAST) {
    active_modem = new contestia(modemCode);
  } else if (modemCode == MODE_CW) {
    active_modem = new cw(wpm, cwrisetime);
  } else if (modemCode == MODE_RTTY) {
    active_modem = new rtty(rtty_baud, rtty_shaped);
  } else if (modemCode >= MODE_LORA_FIRST && modemCode <= MODE_LORA_LAST) {
    active_modem = new lora(modemCode);
  }
  // Do the initializations
  if (active_modem != NULL) {
    // First overall modem initilisation
    active_modem->init();
    // Moved before rx_init as per Fldigi
    // Then set RX frequency
    active_modem->set_freq(newFrequency);
    // Then init of RX side
    active_modem->rx_init();
    // Reset UTF-8 sequence monitor
    utfExpectedChars = utfFoundChars = 0;
  }
}

//***************** C++ Functions to handle FLDigi and RFPWM  **************

// Creates the RSID receive Modem
int modem_createRsidModem(double wpm, double cwrisetime, int rtty_baud,
                          bool rtty_shaped) {
  // Delete previously created RSID modem to recover memory used
  if (RsidModem) {
    delete RsidModem;
    RsidModem = NULL;
  }
  RsidModem = new cRsId(wpm, cwrisetime, rtty_baud, rtty_shaped);
  if (RsidModem != NULL) {
    return 0;
  } else {
    return 1;
  }
}

void modem_destroyRsidModem() {
  if (RsidModem) {
    delete RsidModem;
    RsidModem = NULL;
  }
}

// Creates the requested modem in C++
int modem_createModem(int modemCode, double wpm, double cwrisetime,
                      int rtty_baud, bool rtty_shaped) {
  // Delete previously created modem to recover memory used
  if (active_modem) {
    delete active_modem;
    active_modem = NULL;
  }

  // Now re-create with new mode
  if (modemCode >= MODE_PSK_FIRST && modemCode <= MODE_PSK_LAST) {
    active_modem = new psk(modemCode);
  } else if (modemCode >= MODE_DOMINOEX_FIRST &&
             modemCode <= MODE_DOMINOEX_LAST) {
    active_modem = new dominoex(modemCode);
  } else if (modemCode >= MODE_THOR_FIRST && modemCode <= MODE_THOR_LAST) {
    active_modem = new thor(modemCode);
  } else if (modemCode >= MODE_MFSK_FIRST && modemCode <= MODE_MFSK_LAST) {
    active_modem = new mfsk(modemCode);
  } else if (modemCode >= MODE_MT63_FIRST && modemCode <= MODE_MT63_LAST) {
    active_modem = new mt63(modemCode);
  } else if (modemCode >= MODE_OLIVIA_FIRST && modemCode <= MODE_OLIVIA_LAST) {
    active_modem = new olivia(modemCode);
  } else if (modemCode >= MODE_CONTESTIA_FIRST &&
             modemCode <= MODE_CONTESTIA_LAST) {
    active_modem = new contestia(modemCode);
  } else if (modemCode == MODE_CW) {
    active_modem = new cw(wpm, cwrisetime);
  } else if (modemCode == MODE_RTTY) {
    active_modem = new rtty(rtty_baud, rtty_shaped);
  } else if (modemCode >= MODE_LORA_FIRST && modemCode <= MODE_LORA_LAST) {
    active_modem = new lora(modemCode);
  } else {
    return 1;
  }
  return 0;
}

void modem_destroyModem() {
  if (active_modem) {
    delete active_modem;
    active_modem = NULL;
  }
}

// Transmit section-------------------

// Send RSID of current mode
void modem_txRSID() {
  if (RsidModem != NULL && active_modem != NULL) {
    RsidModem->send(
        true); // Always true as we handle post-rsid decision in higher level
  }
}

// Initializes the TX section of the requested modem in C++
int modem_txInit(double frequency) {
  // Init static variable
  outputBufferIndex = 0;

  if (active_modem != NULL) {
    // Then set TX frequency
    active_modem->set_freq(frequency);
    // active_modem->set_samplerate(samplerate);
    // Init of TX side
    active_modem->tx_init();
    return 0;
  } else {
    return 1;
  }
}

// Processes the data buffer for TX through the current modem in C++
int modem_txCProcess(signed char *buffer, int length) {
  // Reset the static variables
  txCounter = 0;
  // Convert to C++ type
  txDataBuffer = buffer;
  txDataBufferLength = length;
  // Process the buffer in C++
  if (active_modem != NULL) {
    // As there is no idle time in Flmsg context, keep
    //  processing until we have exhausted the data buffer
    while (active_modem->tx_process() >=
           0) { // Character processing is done in the test
    }
  } else {
    return 1;
  }
  // Return the status
  return 0;
}
