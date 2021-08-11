# Giovanni Camurati
# NDK android-ndk-r21d for Arm-V7A and Arm-V8A
# Clang 3.6.2 for x86
# minggw-x64 for windows
# toolchain-mips_34kc_gcc-5.3.0_musl-1.1.14 for MIPS
# Makefile inpired from https://spin.atomicobject.com/2016/08/26/makefile-c-projects/
# and drammer https://github.com/vusec/drammer

ARCH ?= v8
OP ?= CIVAC
DEBUG ?= false
OS ?= linux

ifeq  ($(strip $(ARCH)),v8)

TARGET_ARCH=aarch64-linux-android
LEAKY_OP=-DLEAKY_OP_$(OP)
API=21
TOOLCHAIN=${NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/${TARGET_ARCH}${API}-
SRC_DIRS=./src/fldigi-noise-sdr ./src/fldigi ./src/libion
LDDFLAGS = -lstdc++ --static
CC=${TOOLCHAIN}clang
CXX=${TOOLCHAIN}clang++
LD=${TOOLCHAIN}llvm-link
INSTALL=adb push $(BUILD_DIR)/$(TARGET_EXEC) /data/local/tmp/

else ifeq ($(strip $(ARCH)),v7)

TARGET_ARCH=armv7a-linux-androideabi
OP=ION
LEAKY_OP=-DLEAKY_OP_$(OP)
API=21
TOOLCHAIN=${NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/${TARGET_ARCH}${API}-
SRC_DIRS=./src/fldigi-noise-sdr ./src/fldigi ./src/libion
LDDFLAGS = -lstdc++ --static
CC=${TOOLCHAIN}clang
CXX=${TOOLCHAIN}clang++
LD=${TOOLCHAIN}llvm-link
INSTALL=adb push $(BUILD_DIR)/$(TARGET_EXEC) /data/local/tmp/

else ifeq ($(strip $(ARCH)),x86)

ifeq  ($(strip $(OS)),linux)
OP=STREAM
LEAKY_OP=-DLEAKY_OP_$(OP)
TOOLCHAIN=
SRC_DIRS=./src/fldigi-noise-sdr ./src/fldigi
LDDFLAGS = -lstdc++ --static -lpthread
CC=${TOOLCHAIN}clang
CXX=${TOOLCHAIN}clang++
LD=${TOOLCHAIN}llvm-link
INSTALL=
else ifeq ($(strip $(OS)),windows)
OP=STREAM
LEAKY_OP=-DLEAKY_OP_$(OP)
TOOLCHAIN=
SRC_DIRS=./src/fldigi-noise-sdr ./src/fldigi
LDDFLAGS = -lstdc++ --static -lpthread
CC=x86_64-w64-mingw32-gcc-posix
CXX=x86_64-w64-mingw32-g++-posix
LD=x86_64-w64-mingw32-ld
INSTALL=
endif

else ifeq ($(strip $(ARCH)),mips)

TARGET_ARCH=mips-openwrt-linux
#OP=CNT
OP=CNT
LEAKY_OP=-DLEAKY_OP_$(OP)
SMALL=-DSMALL
TOOLCHAIN=${TOOLCHAIN_MIPS}/bin/${TARGET_ARCH}-
SRC_DIRS=./src/fldigi-noise-sdr ./src/fldigi
LDDFLAGS = -lstdc++ -lgcc_pic --static
CC=${TOOLCHAIN}gcc
CXX=${TOOLCHAIN}g++
LD=${TOOLCHAIN}ld
INSTALL=scp $(BUILD_DIR)/$(TARGET_EXEC) carambola:/home/carambola/
ARCHFLAGS=-march=mips32r2

endif

ifeq ($(strip $(OS)),windows)
TARGET_EXEC=fldigi-noise-sdr-$(ARCH)-$(OP).exe
else
TARGET_EXEC=fldigi-noise-sdr-$(ARCH)-$(OP)
endif

BUILD_DIR ?= ./build

SRCS := $(shell find $(SRC_DIRS) -name *.cxx -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

HAVE = -DHAVE_STRCASESTR -DHAVE_SETENV
CFLAGS ?= -O3 -Wall $(ARCHFLAGS) $(HAVE) $(LEAKY_OP) $(SMALL) $(INC_FLAGS) -MMD -MP
CXXFLAGS ?= -std=c++11 $(CFLAGS)

ifeq  ($(strip $(DEBUG)),true)
CFLAGS += -ggdb3
CXXFLAGS += -ggdb3
endif

# https://stackoverflow.com/questions/51602926/how-to-rebuild-when-the-recipe-has-changed
ALLFLAGS=$(ARCH)$(OS)$(OP)$(API)$(CFLAGS)$(CXXFLAGS)$(ASFLAGS)$(LDDFLAGS)
BUILT_MARKER := $(BUILD_DIR)/$(shell echo $(ALLFLAGS) | md5sum | head -c 32).built

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	@echo $(INC_DIRS)
	$(CXX) $(OBJS) -o $@ $(LDDFLAGS)

$(BUILT_MARKER):
	@echo $(BUILT_MARKER)
	@mkdir -p $(BUILD_DIR)
	@-rm -f $(BUILD_DIR)/*.built
	@touch $(BUILT_MARKER)

# assembly
$(BUILD_DIR)/%.s.o: %.s $(BUILT_MARKER)
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c $(BUILT_MARKER)
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cxx.o: %.cxx $(BUILT_MARKER)
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean install

install: $(BUILD_DIR)/$(TARGET_EXEC) $(BUILT_MARKER)
	$(INSTALL)
	#adb push $(BUILD_DIR)/$(TARGET_EXEC) /data/local/tmp/

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
