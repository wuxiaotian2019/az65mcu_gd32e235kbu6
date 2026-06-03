################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/drivers/packet_process/Spl_stdio.c 

C_DEPS += \
./src/drivers/packet_process/Spl_stdio.d 

OBJS += \
./src/drivers/packet_process/Spl_stdio.o 


# Each subdirectory must supply rules for building sources it contributes
src/drivers/packet_process/%.o: ../src/drivers/packet_process/%.c src/drivers/packet_process/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD ARM MCU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m23 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -std=gnu11 -DGD32E23x -DGD_ECLIPSE_GCC -DUSE_STDPERIPH_DRIVER -I"../Firmware/CMSIS/" -I"../Firmware/CMSIS/GD/GD32E23x/Include/" -I"../Firmware/GD32E23x_standard_peripheral/Include/" -I"../inc/" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns="$@.lst"   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-drivers-2f-packet_process

clean-src-2f-drivers-2f-packet_process:
	-$(RM) ./src/drivers/packet_process/Spl_stdio.d ./src/drivers/packet_process/Spl_stdio.o

.PHONY: clean-src-2f-drivers-2f-packet_process

