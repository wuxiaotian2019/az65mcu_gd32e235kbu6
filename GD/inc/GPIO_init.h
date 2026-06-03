#ifndef _GPIO_INIT_T
#define _GPIO_INIT_T
#include "gd32e23x.h"
#include <stdbool.h>
#define ERASE_PAGE                      (63)
#define ERASE_PAGE_START_ADDR           (0x08000000U + 0x400U * ERASE_PAGE)
#define PROGRAM_ADDRESS                 ERASE_PAGE_START_ADDR
#define MCU_FLASH_CLEAR                 0xFFFFFFFF

void com_gpio_init(void);
void com1_gpio_init(void);
void MCU_DeepSleepMode(void);
int soc_pbint_pressed(void);
void Set_SPI_High_Resistance(void);
void MCU_FMC_program(uint32_t value);
uint32_t MCU_FMC_Read(void);
void MCU_Set_Soc_Pbint(void);
void MCU_Reset_Soc_Pbint(void);
/* set led/backlight */
void MCU_Set_Led_Up(void);
void MCU_Set_Led_Down(void);
void MCU_Detect_Key_Pressed(void);
bool MCU_Detect_Soc_bootup(void);
void init_key_detection(void);
void check_key_press_during_boot(void);
uint8_t is_long_key_or_nokey_press_power(void);
#endif
