PROJECT_NAME := noise
APPLICATION_TYPE := EMBEDDED
TOOLCHAIN_PREFIX ?= arm-none-eabi-
LD_SCRIPT = ./main/stm32mp15xx_m4.ld

ifneq ($(DEBUG), 1)
OPT_FLAGS = -O2 -ffunction-sections -fdata-sections --specs=nosys.specs --specs=nano.specs -Xlinker -Map="OpenAMP_FreeRTOS_echo_CM4.map"
else
OPT_FLAGS = -O0 -g -ffunction-sections -fdata-sections --specs=nosys.specs --specs=nano.specs -Xlinker -Map="OpenAMP_FreeRTOS_echo_CM4.map"
endif

ARCH_FLAGS = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard 

ERROR_FLAGS = -Wall

CFLAGS = -std=gnu11 $(OPT_FLAGS) $(ARCH_FLAGS) $(ERROR_FLAGS)

CXXFLAGS = -std=gnu++17 $(OPT_FLAGS) $(ARCH_FLAGS) $(ERROR_FLAGS) -fno-exceptions -fno-rtti

ASFLAGS = -x assembler-with-cpp $(ARCH_FLAGS)

LDFLAGS = -T$(LD_SCRIPT) -Xlinker --gc-sections -nodefaultlibs \
-fno-rtti -fno-exceptions -static $(OPT_FLAGS) $(ARCH_FLAGS) $(ERROR_FLAGS)

SYMBOLS = USE_HAL_DRIVER STM32MP157Fxx CORE_CM4

LD_FIRST_OBJECTS = $(BUILD_ROOT_PATH)/main/startup_stm32mp15xx.o

UNUSED_FLAGS =  -nostartfiles


-include ./env.mk
include ../../cpmk/project.mk