################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/drivers/gpio/GPIO_init.c 

C_DEPS += \
./src/drivers/gpio/GPIO_init.d 

OBJS += \
./src/drivers/gpio/GPIO_init.o 


# Each subdirectory must supply rules for building sources it contributes
src/drivers/gpio/%.o: ../src/drivers/gpio/%.c src/drivers/gpio/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD ARM MCU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m23 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -std=gnu11 -DGD32E23x -DGD_ECLIPSE_GCC -DUSE_STDPERIPH_DRIVER -I"../Firmware/CMSIS/" -I"../Firmware/CMSIS/GD/GD32E23x/Include/" -I"../Firmware/GD32E23x_standard_peripheral/Include/" -I"../inc/" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns="$@.lst"   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-drivers-2f-gpio

clean-src-2f-drivers-2f-gpio:
	-$(RM) ./src/drivers/gpio/GPIO_init.d ./src/drivers/gpio/GPIO_init.o

.PHONY: clean-src-2f-drivers-2f-gpio

