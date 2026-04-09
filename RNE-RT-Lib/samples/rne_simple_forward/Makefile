ARCH ?= linux_a7_uclibc
ROOT_PATH ?= ../..
USE_DYNAMIC_LIB ?= 0

ifeq ($(strip $(ARCH)),linux_a7)
	CC_PREFIX := arm-none-linux-gnueabihf-
	CPU_FLAGS := -mcpu=cortex-a7 -mfpu=vfp -mfloat-abi=hard
	X_LIB_L_FLAGS := -lrne_pal_linux_a7,-lgcc,-lc,-lm,-lpthread
	LIBS_DIR := $(if $(filter 1,$(USE_DYNAMIC_LIB)),bin,libs)
	LIBS_BIT := 32bit
else ifeq ($(strip $(ARCH)),linux_a7_uclibc)
	CC_PREFIX := arm-ts-linux-uclibcgnueabihf-
	CPU_FLAGS := -mcpu=cortex-a7 -mfpu=vfp -mfloat-abi=hard
	X_LIB_L_FLAGS := -lrne_pal_linux_a7,-lgcc,-lc,-lm,-lpthread
	LIBS_DIR := $(if $(filter 1,$(USE_DYNAMIC_LIB)),arm_ts_linux_uclibcgnueabihf_bin,arm_ts_linux_uclibcgnueabihf_libs)
	LIBS_BIT := 32bit
else
	CC_PREFIX :=
	CPU_FLAGS :=
	X_LIB_L_FLAGS := -lrne_pal_sim,-lrne_vm,-lgcc,-lc,-lm,-lpthread
	LIBS_DIR := $(if $(filter 1,$(USE_DYNAMIC_LIB)),bin,libs)
	LIBS_BIT := 64bit
endif

BUILD_PATH := build_$(ARCH)
DEF_FLAGS := -DRESULT_ALL  -DDISABLE_PRINT_DEBUG_LAYER

INC_FLAGS := -I$(ROOT_PATH)/include/ -I$(ROOT_PATH)/open_source/record_file -I$(ROOT_PATH)/open_source/gp_layers -I$(ROOT_PATH)/open_source/crc

CC := $(CC_PREFIX)gcc
CXX := $(CC_PREFIX)g++
LINKER := $(CXX)
RM := rm -rf

ifeq ($(MAKECMDGOALS),release)
	BUILD_DIR := $(BUILD_PATH)/Release
	OPT_LEVEL := -O2
else ifeq ($(MAKECMDGOALS),debug)
	BUILD_DIR := $(BUILD_PATH)/Debug
	OPT_LEVEL := -O0
else
	BUILD_DIR := $(BUILD_PATH)/Release
	OPT_LEVEL := -O2
endif

CC_FLAGS := $(CPU_FLAGS) $(OPT_LEVEL) $(DEF_FLAGS) -std=c99 -Wall -Werror -Wno-format-truncation -fPIC -ffunction-sections -fdata-sections $(INC_FLAGS)
CXX_FLAGS := $(CPU_FLAGS) $(OPT_LEVEL) $(DEF_FLAGS) -std=c++11 -Wall -Werror -Wno-format-truncation -fPIC -ffunction-sections -fdata-sections $(INC_FLAGS)
LINK_FLAGS := $(OPT_LEVEL)

SRC_DIRS := src ../../open_source/record_file ../../open_source/gp_layers ../../open_source/crc
C_SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
CC_SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

C_OBJS := $(addprefix $(BUILD_DIR)/,$(C_SRCS:.c=.o))
CC_OBJS := $(addprefix $(BUILD_DIR)/,$(CC_SRCS:.cpp=.o))

TARGET_NAME := $(notdir $(CURDIR)).elf
TARGET := $(BUILD_DIR)/$(TARGET_NAME)
ELFSIZE := $(TARGET).size

.PHONY: all release debug clean run pre_build mkdir_build

# 默认目标
all: pre_build release

# Release 目标
release: $(TARGET) $(ELFSIZE)

# Debug 目标
debug: CC_FLAGS += -g
debug: $(TARGET) $(ELFSIZE)

# 链接规则
$(TARGET): $(C_OBJS) $(CC_OBJS)
	@echo 'Linking target: $@'
	$(LINKER) $(LINK_FLAGS) -o $@ $^ -L$(ROOT_PATH)/$(LIBS_DIR)/$(LIBS_BIT) -Wl,--start-group,-lrne_rt_g3,-lrne_common,$(X_LIB_L_FLAGS),-end-group

# 编译规则
$(BUILD_DIR)/%.o: %.c
	@echo 'Compiling file: $<'
	@mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -MMD -MP -MF"$(@:.o=.d)" -MT"$@" -c -o $@ $<

$(BUILD_DIR)/%.o: %.cpp
	@echo 'Compiling file: $<'
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) -MMD -MP -MF"$(@:.o=.d)" -MT"$@" -c -o $@ $<

# 计算目标文件大小
$(ELFSIZE): $(TARGET)
	@echo 'Calculating size: $<'
	$(CC_PREFIX)size $< | tee $@


clean:
	@-$(RM) build*
	@-$(RM) open_source
	@echo 'Clean done'

pre_build: mkdir_build

mkdir_build:
	@mkdir -p $(BUILD_DIR)

.PHONY: clean all pre_build mkdir_build
