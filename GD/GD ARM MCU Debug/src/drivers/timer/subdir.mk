################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/drivers/timer/timer.c 

C_DEPS += \
./src/drivers/timer/timer.d 

OBJS += \
./src/drivers/timer/timer.o 


# Each subdirectory must supply rules for building sources it contributes
src/drivers/timer/%.o: ../src/drivers/timer/%.c src/drivers/timer/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD ARM MCU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m23 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -std=gnu11 -DGD32E23x -DGD_ECLIPSE_GCC -DUSE_STDPERIPH_DRIVER -I"../Firmware/CMSIS/" -I"../Firmware/CMSIS/GD/GD32E23x/Include/" -I"../Firmware/GD32E23x_standard_peripheral/Include/" -I"../inc/" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns="$@.lst"   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-drivers-2f-timer

clean-src-2f-drivers-2f-timer:
	-$(RM) ./src/drivers/timer/timer.d ./src/drivers/timer/timer.o

.PHONY: clean-src-2f-drivers-2f-timer

