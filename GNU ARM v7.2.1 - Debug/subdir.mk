################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LE_timer0.c \
../LaunchPad.c \
../Logging.c \
../Maze.c \
../Motor_withPI.c \
../Reflectance_ADC.c \
../SysTickInts.c \
../Tachometer.c \
../UART0.c \
../app.c \
../battery.c \
../color_tcs34725.c \
../commandline.c \
../configure.c \
../crc.c \
../display.c \
../display_ssd1327.c \
../fonts.c \
../i2c_drv.c \
../keyboard.c \
../main.c \
../placeholder.c \
../profiler.c \
../rand.c \
../spi_drv.c 

OBJS += \
./LE_timer0.o \
./LaunchPad.o \
./Logging.o \
./Maze.o \
./Motor_withPI.o \
./Reflectance_ADC.o \
./SysTickInts.o \
./Tachometer.o \
./UART0.o \
./app.o \
./battery.o \
./color_tcs34725.o \
./commandline.o \
./configure.o \
./crc.o \
./display.o \
./display_ssd1327.o \
./fonts.o \
./i2c_drv.o \
./keyboard.o \
./main.o \
./placeholder.o \
./profiler.o \
./rand.o \
./spi_drv.o 

C_DEPS += \
./LE_timer0.d \
./LaunchPad.d \
./Logging.d \
./Maze.d \
./Motor_withPI.d \
./Reflectance_ADC.d \
./SysTickInts.d \
./Tachometer.d \
./UART0.d \
./app.d \
./battery.d \
./color_tcs34725.d \
./commandline.d \
./configure.d \
./crc.d \
./display.d \
./display_ssd1327.d \
./fonts.d \
./i2c_drv.d \
./keyboard.d \
./main.d \
./placeholder.d \
./profiler.d \
./rand.d \
./spi_drv.d 


# Each subdirectory must supply rules for building sources it contributes
LE_timer0.o: ../LE_timer0.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"LE_timer0.d" -MT"LE_timer0.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

LaunchPad.o: ../LaunchPad.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"LaunchPad.d" -MT"LaunchPad.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Logging.o: ../Logging.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"Logging.d" -MT"Logging.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Maze.o: ../Maze.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"Maze.d" -MT"Maze.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Motor_withPI.o: ../Motor_withPI.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"Motor_withPI.d" -MT"Motor_withPI.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Reflectance_ADC.o: ../Reflectance_ADC.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"Reflectance_ADC.d" -MT"Reflectance_ADC.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

SysTickInts.o: ../SysTickInts.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"SysTickInts.d" -MT"SysTickInts.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Tachometer.o: ../Tachometer.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"Tachometer.d" -MT"Tachometer.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

UART0.o: ../UART0.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"UART0.d" -MT"UART0.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

app.o: ../app.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"app.d" -MT"app.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

battery.o: ../battery.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"battery.d" -MT"battery.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

color_tcs34725.o: ../color_tcs34725.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"color_tcs34725.d" -MT"color_tcs34725.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

commandline.o: ../commandline.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"commandline.d" -MT"commandline.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

configure.o: ../configure.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"configure.d" -MT"configure.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

crc.o: ../crc.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"crc.d" -MT"crc.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

display.o: ../display.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"display.d" -MT"display.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

display_ssd1327.o: ../display_ssd1327.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"display_ssd1327.d" -MT"display_ssd1327.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

fonts.o: ../fonts.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"fonts.d" -MT"fonts.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

i2c_drv.o: ../i2c_drv.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"i2c_drv.d" -MT"i2c_drv.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

keyboard.o: ../keyboard.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"keyboard.d" -MT"keyboard.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.o: ../main.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"main.d" -MT"main.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

placeholder.o: ../placeholder.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"placeholder.d" -MT"placeholder.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

profiler.o: ../profiler.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"profiler.d" -MT"profiler.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

rand.o: ../rand.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"rand.d" -MT"rand.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

spi_drv.o: ../spi_drv.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG12B810F1024GM64=1' '-DSL_BOARD_NAME="BRD2207A"' '-DSL_BOARD_REV="A02"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\autogen" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze\config" -I"C:\Users\wl\SimplicityStudio\v5_workspace\Maze" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFM32GG12B/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Include" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/host/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/wl/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -O0 -Wall -Wextra -fno-builtin -fomit-frame-pointer -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -c -fmessage-length=0 -MMD -MP -MF"spi_drv.d" -MT"spi_drv.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


