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

    /* 2.clear lcd with white */
    lcd_clear(WHITE);

    /* 3.draw logo image 在windows中的rgb565图片大小是38472KB，需要偏移72字节, 才以需要从logo_buffer+72开始绘制
    */
    lcd_draw_image(0, (Y_MAX_PIXEL - 80) / 2, 240, 80, logo_buffer + 72);

    printf("exit %s\n", __func__);
    return 0;
}