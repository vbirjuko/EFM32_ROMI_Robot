################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include gecko_sdk_4.0.2/platform/service/system/src/subdir.mk
-include gecko_sdk_4.0.2/platform/service/device_init/src/subdir.mk
-include gecko_sdk_4.0.2/platform/emlib/src/subdir.mk
-include gecko_sdk_4.0.2/platform/common/toolchain/src/subdir.mk
-include gecko_sdk_4.0.2/platform/Device/SiliconLabs/EFM32GG12B/Source/GCC/subdir.mk
-include gecko_sdk_4.0.2/platform/Device/SiliconLabs/EFM32GG12B/Source/subdir.mk
-include gecko_sdk_4.0.2/hardware/board/src/subdir.mk
-include autogen/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Maze.axf

# Tool invocations
Maze.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "C:\Users\wl\SimplicityStudio\v5_workspace\Maze/autogen/linkerfile.ld" -Xlinker --gc-sections -Xlinker -Map="Maze.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o Maze.axf -Wl,--start-group "./LE_timer0.o" "./LaunchPad.o" "./Logging.o" "./Maze.o" "./Motor_withPI.o" "./Reflectance_ADC.o" "./SysTickInts.o" "./Tachometer.o" "./UART0.o" "./app.o" "./battery.o" "./color_tcs34725.o" "./commandline.o" "./configure.o" "./crc.o" "./display.o" "./display_ssd1327.o" "./fonts.o" "./i2c_drv.o" "./keyboard.o" "./main.o" "./placeholder.o" "./profiler.o" "./rand.o" "./spi_drv.o" "./autogen/sl_board_default_init.o" "./autogen/sl_device_init_clocks.o" "./autogen/sl_event_handler.o" "./gecko_sdk_4.0.2/hardware/board/src/sl_board_control_gpio.o" "./gecko_sdk_4.0.2/hardware/board/src/sl_board_init.o" "./gecko_sdk_4.0.2/platform/Device/SiliconLabs/EFM32GG12B/Source/GCC/startup_efm32gg12b.o" "./gecko_sdk_4.0.2/platform/Device/SiliconLabs/EFM32GG12B/Source/system_efm32gg12b.o" "./gecko_sdk_4.0.2/platform/common/toolchain/src/sl_memory.o" "./gecko_sdk_4.0.2/platform/emlib/src/em_assert.o" "./gecko_sdk_4.0.2/platform/emlib/src/em_cmu.o" "./gecko_sdk_4.0.2/platform/emlib/src/em_core.o" "./gecko_sdk_4.0.2/platform/emlib/src/em_emu.o" "./gecko_sdk_4.0.2/platform/emlib/src/em_gpio.o" "./gecko_sdk_4.0.2/platform/emlib/src/em_ldma.o" "./gecko_sdk_4.0.2/platform/emlib/src/em_system.o" "./gecko_sdk_4.0.2/platform/service/device_init/src/sl_device_init_dcdc_s1.o" "./gecko_sdk_4.0.2/platform/service/device_init/src/sl_device_init_emu_s1.o" "./gecko_sdk_4.0.2/platform/service/device_init/src/sl_device_init_hfxo_s1.o" "./gecko_sdk_4.0.2/platform/service/device_init/src/sl_device_init_lfxo_s1.o" "./gecko_sdk_4.0.2/platform/service/device_init/src/sl_device_init_nvic.o" "./gecko_sdk_4.0.2/platform/service/system/src/sl_system_init.o" "./gecko_sdk_4.0.2/platform/service/system/src/sl_system_process_action.o" -lgcc -lc -lm -lnosys -Wl,--end-group -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: Maze.hex'
	arm-none-eabi-objcopy -O ihex "Maze.axf" "Maze.hex"
	@echo ' '

	@echo 'Building bin file: Maze.bin'
	arm-none-eabi-objcopy -O binary "Maze.axf" "Maze.bin"
	@echo ' '

	@echo 'Building s37 file: Maze.s37'
	arm-none-eabi-objcopy -O srec "Maze.axf" "Maze.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "Maze.axf" -A
	@echo ' '

# Other Targets
clean:
	-@$(foreach FILE, $(EXECUTABLES), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(OBJS), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(C_DEPS), $(shell $(RM) $(FILE)) )
	-$(RM) Maze.axf
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
