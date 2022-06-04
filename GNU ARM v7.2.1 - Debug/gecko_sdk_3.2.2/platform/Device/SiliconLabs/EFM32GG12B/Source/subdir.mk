################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.2/platform/Device/SiliconLabs/EFM32GG12B/Source/system_efm32gg12b.c 

OBJS += \
./gecko_sdk_3.2.2/platform/Device/SiliconLabs/EFM32GG12B/Source/system_efm32gg12b.o 

C_DEPS += \
./gecko_sdk_3.2.2/platform/Device/SiliconLabs/EFM32GG12B/Source/system_efm32gg12b.d 


# Each subdirectory must supply rules for building sources it contributes
gecko_sdk_3.2.2/platform/Device/SiliconLabs/EFM32GG12B/Source/system_efm32gg12b.o: C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.2/platform/Device/SiliconLabs/EFM32GG12B/Source/system_efm32gg12b.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -IC:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.2/platform/Device/SiliconLabs/EFM32GG12B/Include -IC:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.2/hardware/board/inc -IC:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.2/platform/CMSIS/Include -IC:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.2/platform/service/device_init/inc -IC:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.2/platform/emlib/inc -IC:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.2/platform/common/inc -IC:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.2/platform/common/toolchain/inc -IC:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.2/platform/service/system/inc -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"gecko_sdk_3.2.2/platform/Device/SiliconLabs/EFM32GG12B/Source/system_efm32gg12b.d" -MT"gecko_sdk_3.2.2/platform/Device/SiliconLabs/EFM32GG12B/Source/system_efm32gg12b.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


