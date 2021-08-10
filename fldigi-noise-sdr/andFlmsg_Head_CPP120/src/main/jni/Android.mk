LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := AndFlmsg_Modem_Interface
LOCAL_CPP_EXTENSION := .cxx .cpp .cc
LOCAL_SRC_FILES := AndFlmsg_Fldigi_Interface.cpp fldigi/modem.cxx \
			fldigi/psk/psk.cxx fldigi/psk/pskcoeff.cxx fldigi/psk/pskvaricode.cxx \
			fldigi/filters/filters.cxx fldigi/filters/viterbi.cxx fldigi/filters/fftfilt.cxx \
			fldigi/mfsk/interleave.cxx fldigi/mfsk/mfskvaricode.cxx fldigi/mfsk/mfsk.cxx \
			fldigi/misc/misc.cxx fldigi/misc/util.cxx fldigi/misc/configuration.cxx \
			fldigi/mt63/dsp.cxx fldigi/mt63/mt63base.cxx fldigi/mt63/mt63.cxx \
		    fldigi/rsid/rsid.cxx \
			fldigi/fft/fft.cxx \
			fldigi/dominoex/dominoex.cxx fldigi/dominoex/dominovar.cxx \
			fldigi/thor/thor.cxx fldigi/thor/thorvaricode.cxx \
			fldigi/olivia/olivia.cxx \
			fldigi/contestia/contestia.cxx
LOCAL_C_INCLUDES := $(LOCAL_PATH)/fldigi/include
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := AndFlmsg_Flmsg_Interface
LOCAL_CPP_EXTENSION := .cxx .cpp .cc
LOCAL_SRC_FILES := 	AndFlmsg_Flmsg_Interface.cpp flmsg/flmsg.cxx \
					flmsg/Fltk/fltk_extracts.cxx \
					flmsg/utils/status.cxx  \
					flmsg/utils/time_table.cxx \
					flmsg/utils/ext_string.cxx \
					flmsg/utils/base64.cxx flmsg/utils/base128.cxx flmsg/utils/base256.cxx \
					flmsg/lzma/Alloc.c flmsg/lzma/LzFind.c flmsg/lzma/LzmaDec.c \
					flmsg/lzma/LzmaEnc.c flmsg/lzma/LzmaLib.c \
					flmsg/utils/util.cxx flmsg/utils/wrap.cxx \
					flmsg/custom/custom.cxx \
					flmsg/radiogram/radiogram.cxx flmsg/radiogram/arl_msgs.cxx flmsg/radiogram/hx_msgs.cxx
LOCAL_C_INCLUDES := $(LOCAL_PATH)/flmsg/include
include $(BUILD_SHARED_LIBRARY)