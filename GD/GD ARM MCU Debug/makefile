################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/drivers/uart/subdir.mk
-include src/drivers/timer/subdir.mk
-include src/drivers/packet_process/subdir.mk
-include src/drivers/nand/subdir.mk
-include src/drivers/lcd/subdir.mk
-include src/drivers/gpio/subdir.mk
-include src/drivers/deepsleep/subdir.mk
-include src/subdir.mk
-include gcc_startup/subdir.mk
-include Firmware/GD32E23x_standard_peripheral/Source/subdir.mk
-include Firmware/CMSIS/GD/GD32E23x/Source/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := Project
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH_HEX += \
Project.hex \

SECONDARY_FLASH_BIN += \
Project.bin \

SECONDARY_LIST += \
Project.lst \

SECONDARY_SIZE += \
Project.siz \


# All Target
all: main-build

# Main-build Target
main-build: Project.elf secondary-outputs

# Tool invocations
Project.elf: $(OBJS) $(USER_OBJS) \\TJAND17\sen.he\project\unc_linux_rls_25d\source\bsp\mcu\GD\ldscripts\gd32e23x_flash.ld makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GD ARM MCU C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m23 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3  -T gd32e23x_flash.ld -Xlinker --gc-sections -L"../EXTERN_LIB" -L"../ldscripts/" -Wl,-Map,"Project.map" --specs=nano.specs --specs=nosys.specs -o "Project.elf" @"objects.list" $(USER_OBJS) $(LIBS) -l"Project"
	@echo 'Finished building target: $@'
	@echo ' '

Project.hex: Project.elf makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: GD ARM MCU Flash Image(Hex)'
	arm-none-eabi-objcopy -O ihex  "Project.elf"  "Project.hex"
	@echo 'Finished building: $@'
	@echo ' '

Project.bin: Project.elf makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: GD ARM MCU Flash Image(Bin)'
	arm-none-eabi-objcopy -O binary  "Project.elf"  "Project.bin"
	@echo 'Finished building: $@'
	@echo ' '

Project.lst: Project.elf makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: GD ARM MCU Listing'
	arm-none-eabi-objdump --source --all-headers --demangle --line-numbers --wide "Project.elf" > "Project.lst"
	@echo 'Finished building: $@'
	@echo ' '

Project.siz: Project.elf makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: GD ARM MCU Print Size'
	arm-none-eabi-size --format=berkeley "Project.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Project.bin Project.elf Project.hex Project.lst Project.siz
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH_HEX) $(SECONDARY_FLASH_BIN) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents main-build

-include ../makefile.targets
