################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/drivers/lcd/gd32e235c_lcd_eval.c \
../src/drivers/lcd/lcd_display.c \
../src/drivers/lcd/lcd_log.c 

C_DEPS += \
./src/drivers/lcd/gd32e235c_lcd_eval.d \
./src/drivers/lcd/lcd_display.d \
./src/drivers/lcd/lcd_log.d 

OBJS += \
./src/drivers/lcd/gd32e235c_lcd_eval.o \
./src/drivers/lcd/lcd_display.o \
./src/drivers/lcd/lcd_log.o 


# Each subdirectory must supply rules for building sources it contributes
src/drivers/lcd/%.o: ../src/drivers/lcd/%.c src/drivers/lcd/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD ARM MCU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m23 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -std=gnu11 -DGD32E23x -DGD_ECLIPSE_GCC -DUSE_STDPERIPH_DRIVER -I"../Firmware/CMSIS/" -I"../Firmware/CMSIS/GD/GD32E23x/Include/" -I"../Firmware/GD32E23x_standard_peripheral/Include/" -I"../inc/" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns="$@.lst"   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-drivers-2f-lcd

clean-src-2f-drivers-2f-lcd:
	-$(RM) ./src/drivers/lcd/gd32e235c_lcd_eval.d ./src/drivers/lcd/gd32e235c_lcd_eval.o ./src/drivers/lcd/lcd_display.d ./src/drivers/lcd/lcd_display.o ./src/drivers/lcd/lcd_log.d ./src/drivers/lcd/lcd_log.o

.PHONY: clean-src-2f-drivers-2f-lcd

