################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/drivers/nand/DS35X2GA.c \
../src/drivers/nand/ESMTF50D1G41MYB.c \
../src/drivers/nand/FORESEE_F35UQB004G.c \
../src/drivers/nand/GD5F1GQ4xExxG.c \
../src/drivers/nand/Spi_nand_driver.c 

C_DEPS += \
./src/drivers/nand/DS35X2GA.d \
./src/drivers/nand/ESMTF50D1G41MYB.d \
./src/drivers/nand/FORESEE_F35UQB004G.d \
./src/drivers/nand/GD5F1GQ4xExxG.d \
./src/drivers/nand/Spi_nand_driver.d 

OBJS += \
./src/drivers/nand/DS35X2GA.o \
./src/drivers/nand/ESMTF50D1G41MYB.o \
./src/drivers/nand/FORESEE_F35UQB004G.o \
./src/drivers/nand/GD5F1GQ4xExxG.o \
./src/drivers/nand/Spi_nand_driver.o 


# Each subdirectory must supply rules for building sources it contributes
src/drivers/nand/%.o: ../src/drivers/nand/%.c src/drivers/nand/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GD ARM MCU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m23 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -std=gnu11 -DGD32E23x -DGD_ECLIPSE_GCC -DUSE_STDPERIPH_DRIVER -I"../Firmware/CMSIS/" -I"../Firmware/CMSIS/GD/GD32E23x/Include/" -I"../Firmware/GD32E23x_standard_peripheral/Include/" -I"../inc/" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -Wa,-adhlns="$@.lst"   -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-drivers-2f-nand

clean-src-2f-drivers-2f-nand:
	-$(RM) ./src/drivers/nand/DS35X2GA.d ./src/drivers/nand/DS35X2GA.o ./src/drivers/nand/ESMTF50D1G41MYB.d ./src/drivers/nand/ESMTF50D1G41MYB.o ./src/drivers/nand/FORESEE_F35UQB004G.d ./src/drivers/nand/FORESEE_F35UQB004G.o ./src/drivers/nand/GD5F1GQ4xExxG.d ./src/drivers/nand/GD5F1GQ4xExxG.o ./src/drivers/nand/Spi_nand_driver.d ./src/drivers/nand/Spi_nand_driver.o

.PHONY: clean-src-2f-drivers-2f-nand

