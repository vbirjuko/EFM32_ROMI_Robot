################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk/platform/emlib/src/em_assert.c \
C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk/platform/emlib/src/em_cmu.c \
C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk/platform/emlib/src/em_core.c \
C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk/platform/emlib/src/em_emu.c \
C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk/platform/emlib/src/em_gpio.c \
C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk/platform/emlib/src/em_ldma.c \
C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk/platform/emlib/src/em_system.c 

OBJS += \
./gecko_sdk_4.0.2/platform/emlib/src/em_assert.o \
./gecko_sdk_4.0.2/platform/emlib/src/em_cmu.o \
./gecko_sdk_4.0.2/platform/emlib/src/em_core.o \
./gecko_sdk_4.0.2/platform/emlib/src/em_emu.o \
./gecko_sdk_4.0.2/platform/emlib/src/em_gpio.o \
./gecko_sdk_4.0.2/platform/emlib/src/em_ldma.o \
./gecko_sdk_4.0.2/platform/emlib/src/em_system.o 

C_DEPS += \
./gecko_sdk_4.0.2/platform/emlib/src/em_assert.d \
./gecko_sdk_4.0.2/platform/emlib/src/em_cmu.d \
./gecko_sdk_4.0.2/platform/emlib/src/em_core.d \
./gecko_sdk_4.0.2/platform/emlib/src/em_emu.d \
./gecko_sdk_4.0.2/platform/emlib/src/em_gpio.d \
./gecko_sdk_4.0.2/platform/emlib/src/em_ldma.d \
./gecko_sdk_4.0.2/platform/emlib/src/em_system.d 


# Each subdirectory must supply rules for building sources it contributes
gecko_sdk_4.0.2/platform/emlib/src/em_assert.o: C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk/platform/emlib/src/em_assert.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"gecko_sdk_4.0.2/platform/emlib/src/em_assert.d" -MT"gecko_sdk_4.0.2/platform/emlib/src/em_assert.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

gecko_sdk_4.0.2/platform/emlib/src/em_cmu.o: C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk/platform/emlib/src/em_cmu.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"gecko_sdk_4.0.2/platform/emlib/src/em_cmu.d" -MT"gecko_sdk_4.0.2/platform/emlib/src/em_cmu.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

gecko_sdk_4.0.2/platform/emlib/src/em_core.o: C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk/platform/emlib/src/em_core.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"gecko_sdk_4.0.2/platform/emlib/src/em_core.d" -MT"gecko_sdk_4.0.2/platform/emlib/src/em_core.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

gecko_sdk_4.0.2/platform/emlib/src/em_emu.o: C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk/platform/emlib/src/em_emu.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"gecko_sdk_4.0.2/platform/emlib/src/em_emu.d" -MT"gecko_sdk_4.0.2/platform/emlib/src/em_emu.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

gecko_sdk_4.0.2/platform/emlib/src/em_gpio.o: C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk/platform/emlib/src/em_gpio.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"gecko_sdk_4.0.2/platform/emlib/src/em_gpio.d" -MT"gecko_sdk_4.0.2/platform/emlib/src/em_gpio.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

gecko_sdk_4.0.2/platform/emlib/src/em_ldma.o: C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk/platform/emlib/src/em_ldma.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"gecko_sdk_4.0.2/platform/emlib/src/em_ldma.d" -MT"gecko_sdk_4.0.2/platform/emlib/src/em_ldma.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

gecko_sdk_4.0.2/platform/emlib/src/em_system.o: C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk/platform/emlib/src/em_system.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"gecko_sdk_4.0.2/platform/emlib/src/em_system.d" -MT"gecko_sdk_4.0.2/platform/emlib/src/em_system.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


