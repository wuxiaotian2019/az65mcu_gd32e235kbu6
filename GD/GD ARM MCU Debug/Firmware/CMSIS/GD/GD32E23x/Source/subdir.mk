################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Firmware/CMSIS/GD/GD32E23x/Source/syscalls.c \
../Firmware/CMSIS/GD/GD32E23x/Source/system_gd32e23x.c 

C_DEPS += \
./Firmware/CMSIS/GD/GD32E23x/Source/syscalls.d \
./Firmware/CMSIS/GD/GD32E23x/Source/system_gd32e23x.d 

OBJS += \
./Firmware/CMSIS/GD/GD32E23x/Source/syscalls.o \
./Firmware/CMSIS/GD/GD32E23x/Source/system_gd32e23x.o 


# Each subdirectory must supply rules for building sources it contributes
Firmware/CMSIS/GD/GD32E23x/Source/%.o: ../Firmware/CMSIS/GD/GD32E23x/Source/%.c Firmware/CMSIS/GD/GD32E23x/Source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD ARM MCU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m23 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -std=gnu11 -DGD32E23x -DGD_ECLIPSE_GCC -DUSE_STDPERIPH_DRIVER -I"../Firmware/CMSIS/" -I"../Firmware/CMSIS/GD/GD32E23x/Include/" -I"../Firmware/GD32E23x_standard_peripheral/Include/" -I"../inc/" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns="$@.lst"   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Firmware-2f-CMSIS-2f-GD-2f-GD32E23x-2f-Source

clean-Firmware-2f-CMSIS-2f-GD-2f-GD32E23x-2f-Source:
	-$(RM) ./Firmware/CMSIS/GD/GD32E23x/Source/syscalls.d ./Firmware/CMSIS/GD/GD32E23x/Source/syscalls.o ./Firmware/CMSIS/GD/GD32E23x/Source/system_gd32e23x.d ./Firmware/CMSIS/GD/GD32E23x/Source/system_gd32e23x.o

.PHONY: clean-Firmware-2f-CMSIS-2f-GD-2f-GD32E23x-2f-Source

