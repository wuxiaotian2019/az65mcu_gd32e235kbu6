################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_adc.c \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_cmp.c \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_crc.c \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_dbg.c \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_dma.c \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_exti.c \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_fmc.c \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_fwdgt.c \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_gpio.c \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_i2c.c \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_misc.c \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_pmu.c \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_rcu.c \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_rtc.c \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_spi.c \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_syscfg.c \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_timer.c \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_usart.c \
../Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_wwdgt.c 

C_DEPS += \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_adc.d \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_cmp.d \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_crc.d \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_dbg.d \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_dma.d \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_exti.d \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_fmc.d \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_fwdgt.d \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_gpio.d \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_i2c.d \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_misc.d \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_pmu.d \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_rcu.d \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_rtc.d \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_spi.d \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_syscfg.d \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_timer.d \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_usart.d \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_wwdgt.d 

OBJS += \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_adc.o \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_cmp.o \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_crc.o \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_dbg.o \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_dma.o \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_exti.o \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_fmc.o \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_fwdgt.o \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_gpio.o \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_i2c.o \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_misc.o \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_pmu.o \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_rcu.o \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_rtc.o \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_spi.o \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_syscfg.o \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_timer.o \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_usart.o \
./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_wwdgt.o 


# Each subdirectory must supply rules for building sources it contributes
Firmware/GD32E23x_standard_peripheral/Source/%.o: ../Firmware/GD32E23x_standard_peripheral/Source/%.c Firmware/GD32E23x_standard_peripheral/Source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD ARM MCU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m23 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -std=gnu11 -DGD32E23x -DGD_ECLIPSE_GCC -DUSE_STDPERIPH_DRIVER -I"../Firmware/CMSIS/" -I"../Firmware/CMSIS/GD/GD32E23x/Include/" -I"../Firmware/GD32E23x_standard_peripheral/Include/" -I"../inc/" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns="$@.lst"   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Firmware-2f-GD32E23x_standard_peripheral-2f-Source

clean-Firmware-2f-GD32E23x_standard_peripheral-2f-Source:
	-$(RM) ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_adc.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_adc.o ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_cmp.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_cmp.o ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_crc.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_crc.o ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_dbg.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_dbg.o ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_dma.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_dma.o ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_exti.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_exti.o ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_fmc.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_fmc.o ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_fwdgt.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_fwdgt.o ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_gpio.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_gpio.o ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_i2c.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_i2c.o ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_misc.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_misc.o ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_pmu.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_pmu.o ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_rcu.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_rcu.o ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_rtc.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_rtc.o ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_spi.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_spi.o ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_syscfg.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_syscfg.o ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_timer.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_timer.o ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_usart.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_usart.o ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_wwdgt.d ./Firmware/GD32E23x_standard_peripheral/Source/gd32e23x_wwdgt.o

.PHONY: clean-Firmware-2f-GD32E23x_standard_peripheral-2f-Source

