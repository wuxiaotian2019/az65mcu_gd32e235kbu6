#include "gd32e235c_lcd_eval.h"
#include "Spl_stdio.h"
#include "Spi_nand_driver.h"
#include <stdio.h>

uint8_t *logo_buffer = (uint8_t *)0x8010000;

int lcd_display(void)
{
    printf("enter %s\n", __func__);
    /* 1.lcd init */
    lcd_init();

    /* 2.draw logo image */
    lcd_draw_image(0, 0, 240, 320, logo_buffer);

    printf("exit %s\n", __func__);
    return 0;
}