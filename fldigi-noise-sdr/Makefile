# Giovanni Camurati
# NDK android-ndk-r21d 
# Makefile inpired from https://spin.atomicobject.com/2016/08/26/makefile-c-projects/
# and drammer

TOOLCHAIN=${NDK}/toolchains/llvm/prebuilt/linux-x86_64

TARGET_ARCH=aarch64-linux-android
API=21

CC=${TOOLCHAIN}/bin/${TARGET_ARCH}${API}-clang
CXX=${TOOLCHAIN}/bin/${TARGET_ARCH}${API}-clang++
LD=${TOOLCHAIN}/bin/${TARGET_ARCH}${API}-llvm-link

TARGET_EXEC ?= fldigi-noise-sdr

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.cxx -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

HAVE = -DHAVE_STRCASESTR -DHAVE_SETENV
CPPFLAGS ?= -O3 -Wall $(HAVE) $(INC_FLAGS) -MMD -MP
LDDFLAGS = -lstdc++ --static

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cxx.o: %.cxx
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean install

install: $(BUILD_DIR)/$(TARGET_EXEC)
	adb push $(BUILD_DIR)/$(TARGET_EXEC) /data/local/tmp/

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
