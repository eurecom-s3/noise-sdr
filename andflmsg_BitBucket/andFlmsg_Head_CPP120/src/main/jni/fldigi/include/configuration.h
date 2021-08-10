// ----------------------------------------------------------------------------
// configuration.h
//
// Copyright (C) 2006-2010
//		Dave Freese, W1HKJ
// Copyright (C) 2008-2010
//		Stelios Bounanos, M0GLD
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

#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#include <string>
#include <math.h>

//Android
//#include "rtty.h"
//#include "waterfall.h"
//#include "lookupcall.h"
//#include "psk_browser.h"

/*Android

#if defined(__linux__)
#  define DEFAULT_PTTDEV "/dev/ttyS0"
#  define DEFAULT_CWFSKPORT "/dev/ttyS1"
#  define DEFAULT_HAMRIGDEVICE "/dev/ttyS0"
#elif defined(__FreeBSD__)
#  define DEFAULT_PTTDEV "/dev/ttyd0"
#  define DEFAULT_CWFSKPORT "/dev/ttyd1"
#  define DEFAULT_HAMRIGDEVICE "/dev/ttyd0"
#elif defined(__NetBSD__)
#  define DEFAULT_PTTDEV "/dev/tty00"
#  define DEFAULT_CWFSKPORT "/dev/tty01"
#  define DEFAULT_HAMRIGDEVICE "/dev/tty00"
#elif defined(__OpenBSD__)
#  define DEFAULT_PTTDEV "/dev/tty00"
#  define DEFAULT_CWFSKPORT "/dev/tty01"
#  define DEFAULT_HAMRIGDEVICE "/dev/tty00"
#elif defined(__WOE32__)
#  define DEFAULT_PTTDEV "COM1"
#  define DEFAULT_CWFSKPORT "COM2"
#  define DEFAULT_HAMRIGDEVICE "COM1"
#else // not sure
#  define DEFAULT_PTTDEV "/dev/ptt"
#  define DEFAULT_CWFSKPORT "/dev/fsk"
#  define DEFAULT_HAMRIGDEVICE "/dev/rig"
#endif

*/


// Format: ELEM_(TYPE, VARIABLE-NAME, TAG-STRING, DOC-STRING, DEFAULT-VALUE)
// Variables that are not saved to the xml file have empty TAG-STRINGs and DOC-STRINGs
//
// No preprocessor directives or C++ comments inside this macro!
// Indent with spaces only.

#ifdef ELEM_
#  error ELEM_ should not be defined at this point
#endif

#define CONFIG_LIST                                                                     \
/*Android    ELEM_(bool, confirmExit, "CONFIRMEXIT",                                             \
          "Ensure user wants to leave flgidi",                                          \
          false)                                                                        \
        ELEM_(bool, SaveConfig, "SAVECONFIG",                                           \
              "Save current configuration on exit",                                     \
              false)                                                                    \
        ELEM_(bool, noise, "NOISETEST",                                                 \
              "Noise test on/off",                                                      \
              false)                                                                    \
        ELEM_(double, s2n, "SIGNAL2NOISE",                                              \
              "Signal to Noise ratio for test",                                         \
              +20.0)                                                          */          \
        ELEM_(bool, rsidWideSearch, "RSIDWIDESEARCH",                                   \
              "RSID detector searches the entire passband",                             \
              true)  /*Android changed from false */                                                                  \
        ELEM_(int, rsid_squelch, "RSIDSQUELCH",                                         \
              "RSID detection opens squelch for nn seconds",                            \
              5)                                                                        \
        ELEM_(bool, rsid, "RECEIVERSID",                                                \
              "Enable Reed Soloman ID decoding",                                        \
              false)                                                                    \
        ELEM_(bool, TransmitRSid, "TRANSMITRSID",                                       \
              "Send RSID at beginning and end of transmission",                         \
              false)                                                                    \
/*        ELEM_(bool, rsid_mark, "RSIDMARK",                                              \
              "Append marker (for the previous modem and frequency) to the RX text\n"   \
              "widget before changing to the new modem and/or frequency",               \
              true)                                                                     \
        ELEM_(bool, rsid_notify_only, "RSIDNOTIFYONLY",                                 \
              "Trigger RSID notifications but do not change modem and frequency",       \
              false)                                                                    \
        ELEM_(bool, rsid_auto_disable, "RSIDAUTODISABLE",                               \
              "Disable RSID detection when RsID signal is detected",                    \
              false)                                                                    \
        ELEM_(bool, rsid_post, "RSIDPOST",                                              \
              "Transmit an RSID signal when modem data is concluded",                   \
              false)                                                                    \
        ELEM_(mode_set_t, rsid_rx_modes, "RSIDRXMODESEXCLUDE",                          \
              "Mode names for which RSID reception is disabled",                        \
              mode_set_t())                                                             \
        ELEM_(mode_set_t, rsid_tx_modes, "RSIDTXMODESEXCLUDE",                          \
              "Mode names for which RSID transmission is disabled",                     \
              mode_set_t())                                                             \
*/                                                                                        \
        ELEM_(int, RsID_label_type, "RSID_ERRORS",                                      \
              "values (low, medium, high)  0, 1, 2",                                    \
              1)                                                                        \
/*        ELEM_(bool, disable_rsid_warning_dialog_box, "DISABLE_RSID_WARNING_DIALOG_BOX", \
              "disable displaying the rsid warning dialog box",                         \
              false)                                                      */              \
        ELEM_(bool, slowcpu, "SLOWCPU",                                                 \
              "Disable expensive processing in some decoders",                          \
              true)                                                                     \
        ELEM_(bool, disable_rsid_freq_change, "DISABLERSIDFREQCHANGE",                  \
              "disable changing frequency on rsid modem change/reset",                  \
              false)                                                                    \
		ELEM_(bool, retain_freq_lock, "RETAINFREQLOCK",                                 \
			 "retain frequency lock on rsid modem change/reset",                   \
			 false)                                                                     \
        ELEM_(bool, changed, "", "",  false)                                            \
        /* PSK, filter can be 0, 1, 2, 3 or 4 */                                        \
        ELEM_(int, PSK_filter, "PSKFILTER",                                             \
              "Not configurable; must always be 0",                                     \
              0)                                                                        \
/*        ELEM_(bool, pskbrowser_on, "PSKBROWSER_ON",                                     \
              "Enable psk multi-channel detector - disable for very slow cpus",         \
              true)                                                                     \
*/																						\
        /* PSK / PSKmail interface */                                                   \
        ELEM_(int, SearchRange, "PSKSEARCHRANGE",                                       \
              "PSK signal acquisition search range (Hz)",                               \
              50)                                                                       \
        ELEM_(bool, pskpilot, "PSKPILOT",                                               \
              "Vestigial pilot tone on = 1, off = 0",                                   \
              false)                                                                    \
        ELEM_(double, pilot_power, "PILOT_POWER",                                      \
              "Pilot tone relative power level",                                        \
              -30.0)                                                                    \
        ELEM_(double, ACQsn, "ACQSN",                                                   \
              "PSK signal acquisition S/N (dB)",                                        \
              9.0)                                                                      \
        ELEM_(bool, Pskmails2nreport, "PSKMAILS2NREPORT",                               \
              "Send s2n report to pskmail client/server",                               \
              false)                                                                    \
        ELEM_(bool, StatusDim, "STATUSDIM",                                             \
              "Behaviour of status (S/N and IMD) fields:\n"                             \
              "  0: Clear after timeout\n"                                              \
              "  1: Dim after timeout\n",                                               \
              true)                                                                     \
        ELEM_(double, StatusTimeout, "STATUSTIMEOUT",                                   \
              "Dim or Clear timeout (seconds)",                                         \
              15.0)                                                                     \
        ELEM_(bool, PSKmailSweetSpot, "PSKMAILSWEETSPOT",                               \
              "Reset to carrier when no signal is present",                             \
              false)                                                                    \
        ELEM_(int, ServerOffset, "PSKSERVEROFFSET",                                     \
              "Listen for signals within this range (Hz)",                              \
              50)                                                                       \
        ELEM_(int, ServerCarrier, "PSKSERVERCARRIER",                                   \
              "Default PSKMail listen / transmit frequency",                            \
              1500)                                                                     \
        ELEM_(int, ServerAFCrange, "PSKSERVERAFCRANGE",                                 \
              "Limit AFC movement to this range (Hz)",                                  \
              25)                                                                       \
        ELEM_(double, ServerACQsn, "PSKSERVERACGSN",                                    \
              "Acquisition S/N (dB)",                                                   \
              9.0)                                                                      \
		  /* OLIVIA */                                                                    \
		  ELEM_(int, oliviatones, "OLIVIATONES",                                          \
				"Number of tones. Values are as follows:\n"                               \
				"  0: 2; 1: 4; 2: 8; 3: 16; 4: 32; 5: 64; 6: 128; 7: 256",                \
				2)   /* 8 */                                                              \
		  ELEM_(int, oliviabw, "OLIVIABW",                                                \
				"Bandwidth (Hz). Values are as follows:\n"                                \
				"  0: 125; 1: 250; 2: 500; 3: 1000; 4: 2000.",                            \
				2)   /* 500 */                                                            \
		  ELEM_(int, oliviasmargin, "OLIVIASMARGIN",                                      \
				"Tune margin (tone frequency spacing)",                                   \
				8)                                                                        \
		  ELEM_(int, oliviasinteg, "OLIVIASINTEG",                                        \
				"Integration period (FEC blocks)",                                        \
				4)                                                                        \
		  ELEM_(bool, olivia_reset_fec, "OLIVIARESETFEC",                                 \
				"Force Integration (FEC) depth to be reset when new BW/Tones selected",   \
				false)                                                                    \
		  ELEM_(bool, olivia8bit, "OLIVIA8BIT",                                           \
				"8-bit extended characters",                                              \
				true)                                                                     \
				/* CONTESTIA */                                                          \
        ELEM_(int, contestiatones, "CONTESTIATONES",                                    \
              "Number of tones. Values are as follows:\n"                               \
              "  0: 2; 1: 4; 2: 8; 3: 16; 4: 32; 5: 64; 6: 128; 7: 256",                \
              2)   /* 8 */                                                              \
        ELEM_(int, contestiabw, "CONTESTIABW",                                          \
              "Bandwidth (Hz). Values are as follows:\n"                                \
              "  0: 125; 1: 250; 2: 500; 3: 1000; 4: 2000.",                            \
              2)   /* 500 */                                                            \
        ELEM_(int, contestiasmargin, "CONTESTIASMARGIN",                                \
              "Tune margin (tone frequency spacing)",                                   \
              8)                                                                        \
        ELEM_(int, contestiasinteg, "CONTESTIASINTEG",                                  \
              "Integration period (FEC blocks)",                                        \
              4)                                                                        \
        ELEM_(bool, contestia8bit, "CONTESTIA8BIT",                                     \
              "8-bit extended characters",                                              \
              false)                                                                     \
        ELEM_(bool, contestia_reset_fec, "CONTESTIARESETFEC",                           \
              "Force Integration (FEC) depth to be reset when new BW/Tones selected",   \
              false)                                                                    \
        ELEM_(bool, contestia_start_tones, "CONTESTIASTARTTONES",                       \
              "Send start/stop tones",                                                  \
              true)                                                                     \
        ELEM_(bool, rx_lowercase, "RX_LOWERCASE",                                       \
              "Print Rx in lowercase for CW, RTTY, CONTESTIA and THROB",                \
              false)                                                                    \
		  /* PACKET */                                                                    \
  /*        ELEM_(int, PKT_BAUD_SELECT, "PKTBAUDSELECT",                                    \
				"Packet baud rate. Values are as follows:\n"                              \
				"  0: 1200 (V/UHF); 1: 300 (HF); 2: 2400 (V/UHF)",                        \
				0)   /* 1200 baud (V/UHF) default. */                                     \
  /*        ELEM_(double, PKT_LOSIG_RXGAIN, "LOSIGRXGAIN",                                  \
				"Signal gain for lower frequency (Mark) tone (in dB)",                    \
				0.0)                                                                      \
		  ELEM_(double, PKT_HISIG_RXGAIN, "HISIGRXGAIN",                                  \
				"Signal gain for higher frequency (Space) tone (in dB)",                  \
				0.0)                                                                      \
		  ELEM_(double, PKT_LOSIG_TXGAIN, "LOSIGTXGAIN",                                  \
				"Signal gain for Mark (lower frequency) tone (in dB)",                    \
				0.0)                                                                      \
		  ELEM_(double, PKT_HISIG_TXGAIN, "HISIGTXGAIN",                                  \
				"Signal gain for Space (higher frequency) tone (in dB)",                  \
				0.0)                                                                      \
		  ELEM_(bool, PKT_PreferXhairScope, "PKTPREFERXHAIRSCOPE",                        \
				"Default to syncscope (detected symbol scope)",                           \
				false)                                                                    \
		  ELEM_(bool, PKT_AudioBoost, "PKTAUDIOBOOST",                                    \
				"No extra input gain (similar to Mic Boost) by default",                  \
				false)                                                                    \
																						  \
		  ELEM_(bool, PKT_RXTimestamp, "PKTRXTIMESTAMP",                                  \
				"No timestamps on RX packets by default",                                 \
				false)                                                                    \
																						  \
		  ELEM_(bool, PKT_expandMicE, "PKTEXPANDMICE",                                    \
				"decode received Mic-E data",                                             \
				false)                                                                    \
		  ELEM_(bool, PKT_expandPHG, "PKTEXPANDPHG",                                      \
				"decode received PHG data",                                               \
				false)                                                                    \
		  ELEM_(bool, PKT_expandCmp, "PKTEXPANDCMP",                                      \
				"decode received Compressed Position data",                               \
				false)                                                                    \
		  ELEM_(bool, PKT_unitsSI, "PKTUNITSSI",                                          \
				"display decoded data in SI units",                                       \
				false)                                                                    \
		  ELEM_(bool, PKT_unitsEnglish, "PKTUNITSENGLISH",                                \
				"display decoded data in English units",                                  \
				false)                                                               */     \
		  /* DOMINOEX */                                                                  \
          ELEM_(double, DOMINOEX_BW, "DOMINOEXBW",                                        \
				"Filter bandwidth factor (bandwidth relative to signal width)",           \
				2.0)                                                                      \
/*		  ELEM_(std::string, secText, "SECONDARYTEXT",                                    \
				"Secondary text (sent during keyboard idle times)",                       \
				"")                                                                  */     \
		  ELEM_(bool, DOMINOEX_FILTER, "DOMINOEXFILTER",                                  \
				"Enable filtering before decoding",                                       \
				true)                                                                     \
		  ELEM_(bool, DOMINOEX_FEC, "DOMINOEXFEC",                                        \
				"Enable MultiPSK-compatible FEC",                                         \
				false)                                                                    \
/*		  ELEM_(int, DOMINOEX_PATHS, "DOMINOEXPATHS",                                     \
				"This setting is currently unused",                                       \
				5)                                                                   */     \
		  ELEM_(double, DomCWI, "DOMCWI",                                                 \
				"CWI threshold (CWI detection and suppression)",                          \
				0.0)                                                                      \
				/* Resampler and Audio for future use */									\
/*		  ELEM_(int, in_sample_rate, "INSAMPLERATE",                                      \
				"Input sample rate",                                                      \
				SAMPLE_RATE_UNSET)                                                        \
		  ELEM_(int, out_sample_rate, "OUTSAMPLERATE",                                    \
				"Output sample rate",                                                     \
				SAMPLE_RATE_UNSET)                                                        \
		  ELEM_(int, RX_corr, "RXCORR",                                                   \
				"Input (RX) sample rate correction (PPM)",                                \
				0)                                                                        \
		  ELEM_(int, TX_corr, "TXCORR",                                                   \
				"Output (TX) sample rate correction (PPM)",                               \
				0)																*/		  \
/*		  ELEM_(int, sample_converter, "SAMPLECONVERTER",                                 \
				"Sample rate conversion type. 0: Best SINC; 1: Medium SINC; 2: Fastest SINC; 3: ZOH; 4: Linear. The default is 2.", \
				SRC_SINC_FASTEST)		*/												\
			/* MT63 */                                                                \
			ELEM_(bool, mt63_8bit, "MT638BIT",                                              \
				  "8-bit extended characters",                                              \
				  true)                                                                     \
			ELEM_(bool, mt63_rx_integration, "MT63INTEGRATION",                             \
				  "Long receive integration",                                               \
				  true) /*Android changed from false */                                     \
			ELEM_(bool, mt63_twotones, "MT63TWOTONES",                                      \
				  "Also transmit upper start tone (only if MT63USETONES is enabled)",       \
				  true)                                                                     \
			ELEM_(bool, mt63_usetones, "MT63USETONES",                                      \
				  "Transmit lower start tone",                                              \
				  true)                                                                     \
			ELEM_(int, mt63_tone_duration, "MT63TONEDURATION",                              \
				  "Tone duration (seconds)",                                                \
				  4)                                                                        \
			ELEM_(bool, mt63_at500, "MT63AT500",                                            \
						"Always transmit lowest tone at 500 Hz",                            \
						false)																\
		/* THOR */                                                                      \
		ELEM_(double, THOR_BW, "THORBW",                                                \
			  "Filter bandwidth factor (bandwidth relative to signal width)",           \
			  2.0)                                                                      \
		ELEM_(bool, THOR_FILTER, "THORFILTER",                                          \
			  "Enable filtering before decoding",                                       \
			  true)                                                                     \
		ELEM_(std::string, THORsecText, "THORSECTEXT",                                  \
			  "Secondary text (sent during keyboard idle times)",                       \
			  "")                                                                       \
		ELEM_(int, THOR_PATHS, "THORPATHS",                                             \
			  "This setting is currently unused",                                       \
			  5)                                                                        \
		ELEM_(double, ThorCWI, "THORCWI",                                               \
			  "CWI threshold (CWI detection and suppression)",                          \
			  0.0)                                                                      \
		ELEM_(bool, THOR_PREAMBLE, "THORPREAMBLE",                                      \
			  "Detect THOR preamble (and flush Rx pipeline)",                           \
			  true)                                                                     \
		ELEM_(bool, THOR_SOFTSYMBOLS, "THORSOFTSYMBOLS",                                \
			  "Enable Soft-symbol decoding",                                            \
			  true)                                                                     \
		ELEM_(bool, THOR_SOFTBITS, "THORSOFTBITS",                                      \
			  "Enable Soft-bit decoding",                                               \
			  true)                                                                     \



// declare the struct
#define ELEM_DECLARE_CONFIGURATION(type_, var_, tag_, ...) type_ var_;
#undef ELEM_
#define ELEM_ ELEM_DECLARE_CONFIGURATION
struct configuration
{
	CONFIG_LIST

/*Android
	void writeDefaultsXML();
	void storeDefaults();
	bool readDefaultsXML();
	void loadDefaults();
	void saveDefaults();
	int  setDefaults();
	void resetDefaults(void);
	static void reset(void);

	void initInterface();
	void initMixerDevices();
	void testCommPorts();
	const char* strBaudRate();
	int  BaudRate(size_t);
	int  nBaudRate(const char *);
	void initFonts(void);
*/

};

extern configuration progdefaults;


//Android
//extern void mixerInputs();
//extern void enableMixer(bool);
//extern Fl_Font font_number(const char* name);

enum { SAMPLE_RATE_UNSET = -1, SAMPLE_RATE_AUTO, SAMPLE_RATE_NATIVE, SAMPLE_RATE_OTHER };

#endif
