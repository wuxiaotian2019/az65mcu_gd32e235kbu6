/*!
    \file  gd32e235_lcd_eval.c
    \brief LCD driver functions (LCD_ILI9320)

    \version 2026-02-26, V2.6.0, demo for GD32E23x
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "lcd_font.h"
#include "gd32e235c_lcd_eval.h"
#include "systick.h"

static uint8_t spi_write_byte(uint32_t spi_periph, uint8_t byte);
static void spi1_init(void);
static void lcd_write_index(uint8_t index);
static void lcd_write_data(uint8_t data);

static void lcd_write_data_16bit(uint8_t datah, uint8_t datal);
static void lcd_reset(void);


/*!
    \brief      write an unsigned 8-bit bytes
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[in]  byte: write byte
    \param[out] none
    \retval     none
*/
static uint8_t spi_write_byte(uint32_t spi_periph, uint8_t byte)
{
    SPI_CTL1(spi_periph) |= SPI_CTL1_BYTEN;
    while(RESET == (SPI_STAT(spi_periph)&SPI_FLAG_TBE));
    SPI_DATA(spi_periph) = byte;

    while(RESET == (SPI_STAT(spi_periph)&SPI_FLAG_RBNE));
    return(SPI_DATA(spi_periph));
}

/*!
    \brief      init SPI1
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void spi1_init(void)
{
    spi_parameter_struct spi_init_struct;

    /* enable the gpio clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_SPI1);

    /* GPIOA config, PA13(LCD RS/DC), PA14(LCD_SPI1_MOSI) */
    gpio_af_set(GPIOA, GPIO_AF_6, GPIO_PIN_14);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);

    /* GPIOA config, PA15(LCD_SPI1_CS) */
    gpio_af_set(GPIOA, GPIO_AF_6, GPIO_PIN_15);
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);

    /* GPIOB config, PB1(SPI1_SCK) */
    gpio_af_set(GPIOB, GPIO_AF_6, GPIO_PIN_1);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);

    /* GPIOA config, PA13(SPI1_MISO) -> Unisoc MISO CD pin gpio109 */
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);

    /* GPIOB config, PB2(LCD_RST) */
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    /* SPI1 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_2;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI1, &spi_init_struct);

    /* set crc polynomial */
    spi_enable(SPI1);
}

/*!
    \brief      write the register address
    \param[in]  index: register address
    \param[out] none
    \retval     none
*/
static void lcd_write_index(uint8_t index)
{
    LCD_RS_CLR;
    spi_write_byte(SPI1, index);
}

/*!
    \brief      write the register data
    \param[in]  data: register data
    \param[out] none
    \retval     none
*/
static void lcd_write_data(uint8_t data)
{
    LCD_RS_SET;
    spi_write_byte(SPI1, data);
}

/*!
    \brief      write the register data(an unsigned 16-bit data)
    \param[in]  datah: register data high 8bit
    \param[in]  datal: register data low 8bit
    \param[out] none
    \retval     none
*/
static void lcd_write_data_16bit(uint8_t datah, uint8_t datal)
{
    lcd_write_data(datah);
    lcd_write_data(datal);
}

/*!
    \brief      lcd reset
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void lcd_reset(void)
{
    LCD_RST_SET;
    delay_1ms(10);

    LCD_RST_CLR;
    delay_1ms(10);

    LCD_RST_SET;
    delay_1ms(120);
}


/*!
    \brief      lcd init
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_init(void)
{
    spi1_init();

    LCD_CS_CLR;
    lcd_reset();

    /* write the register address 0xCB*/
    lcd_write_index(0xfe);

    lcd_write_index(0xef);

    lcd_write_index(0x80);
    lcd_write_data(0xc0);

    lcd_write_index(0x81);
    lcd_write_data(0x01);

    lcd_write_index(0x82);
    lcd_write_data(0x07);

    lcd_write_index(0x83);
    lcd_write_data(0x38);

    lcd_write_index(0x88);
    lcd_write_data(0x64);

    lcd_write_index(0xb4);
    lcd_write_data(0x08);

    lcd_write_index(0x89);
    lcd_write_data(0x86);

    lcd_write_index(0x8b);
    lcd_write_data(0x3c);

    lcd_write_index(0x8d);
    lcd_write_data(0x51);

    lcd_write_index(0x8e);
    lcd_write_data(0x70);

    lcd_write_index(0x36);
    lcd_write_data(0x48);

    lcd_write_index(0x3a);
    lcd_write_data(0x05);

    lcd_write_index(0xbf);
    lcd_write_data(0x1f);

    lcd_write_index(0x7d);
    lcd_write_data(0x45);
    lcd_write_data(0x06);

    lcd_write_index(0xee);
    lcd_write_data(0x00);
    lcd_write_data(0x06);

    lcd_write_index(0xf4);
    lcd_write_data(0x53);

    lcd_write_index(0xf6);
    lcd_write_data(0x07);
    lcd_write_data(0x08);

    lcd_write_index(0x70);
    lcd_write_data(0x4f);
    lcd_write_data(0x4f);

    lcd_write_index(0x71);
    lcd_write_data(0x12);
    lcd_write_data(0x20);

    lcd_write_index(0x72);
    lcd_write_data(0x12);
    lcd_write_data(0x20);

    lcd_write_index(0xb5);
    lcd_write_data(0x50);

    lcd_write_index(0xba);
    lcd_write_data(0x00);

    lcd_write_index(0xec);
    lcd_write_data(0x71);

    lcd_write_index(0x7b);
    lcd_write_data(0x00);
    lcd_write_data(0x0d);

    lcd_write_index(0x7c);
    lcd_write_data(0x0d);
    lcd_write_data(0x03);

    lcd_write_index(0xf5);
    lcd_write_data(0x02);
    lcd_write_data(0x10);
    lcd_write_data(0x12);

    lcd_write_index(0xf0);
    lcd_write_data(0x08);
    lcd_write_data(0x0c);
    lcd_write_data(0x0b);
    lcd_write_data(0x09);
    lcd_write_data(0x03);
    lcd_write_data(0x33);
    lcd_write_data(0x49);
    lcd_write_data(0x6c);
    lcd_write_data(0x9c);
    lcd_write_data(0x2e);
    lcd_write_data(0x30);
    lcd_write_data(0x4f);

    lcd_write_index(0xf1);
    lcd_write_data(0x08);
    lcd_write_data(0x0c);
    lcd_write_data(0x0b);
    lcd_write_data(0x06);
    lcd_write_data(0x05);
    lcd_write_data(0x33);
    lcd_write_data(0x4a);
    lcd_write_data(0xdb);
    lcd_write_data(0x8f);
    lcd_write_data(0x2c);
    lcd_write_data(0x2c);
    lcd_write_data(0x9f);

    lcd_write_index(0x66);
    lcd_write_data(0x15);

    lcd_write_index(0x67);
    lcd_write_data(0x1d);

    lcd_write_index(0x68);
    lcd_write_data(0x1d);

    lcd_write_index(0xca);
    lcd_write_data(0x0f);

    lcd_write_index(0xe8);
    lcd_write_data(0xf0);

    lcd_write_index(0xcb);
    lcd_write_data(0x06);

    lcd_write_index(0xb6);
    lcd_write_data(0x5c);
    lcd_write_data(0x40);
    lcd_write_data(0x40);

    lcd_write_index(0xcc);
    lcd_write_data(0x33);

    lcd_write_index(0xcd);
    lcd_write_data(0x33);

    lcd_write_index(0x35);
    lcd_write_data(0x00);

    lcd_write_index(0x44);
    lcd_write_data(0x00);
    lcd_write_data(0x0a);

    /* exit sleep */
    lcd_write_index(0x11);
    delay_1ms(80);

    lcd_write_index(0xe8);
    lcd_write_data(0xa0);

    lcd_write_index(0xe8);
    lcd_write_data(0xf0);

    lcd_write_index(0xfe);
    lcd_write_index(0xee);

    /* display on */
    lcd_write_index(0x29);
    lcd_write_index(0x2c);
    delay_1ms(10);

    LCD_CS_SET;
}

/*!
    \brief      set lcd display region
    \param[in]  x_start: the x position of the start point
    \param[in]  y_start: the y position of the start point
    \param[in]  x_end: the x position of the end point
    \param[in]  y_end: the y position of the end point
    \param[out] none
    \retval     none
*/
void lcd_set_region(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
    LCD_CS_CLR;

    /* write the register address 0x2A*/
    lcd_write_index(0x2A);
    lcd_write_data_16bit(x_start >> 8, x_start);
    lcd_write_data_16bit(x_end >> 8, x_end);

    /* write the register address 0x2B*/
    lcd_write_index(0x2B);
    lcd_write_data_16bit(y_start >> 8, y_start);
    lcd_write_data_16bit(y_end >> 8, y_end);

    /* write the register address 0x2C*/
    lcd_write_index(0x2C);
    LCD_CS_SET;
}

/*!
    \brief      set the start display point of lcd
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[out] none
    \retval     none
*/
void lcd_set_xy(uint16_t x, uint16_t y)
{
    /* write the register address 0x2A*/
    lcd_write_index(0x2A);
    lcd_write_data_16bit(x >> 8, x);

    /* write the register address 0x2B*/
    lcd_write_index(0x2B);
    lcd_write_data_16bit(y >> 8, y);

    /* write the register address 0x2C*/
    lcd_write_index(0x2C);
}

/*!
    \brief      draw a point on the lcd
    \param[in]  x: the x position of the point
    \param[in]  y: the y position of the point
    \param[in]  data: write the register data
    \param[out] none
    \retval     none
*/
void lcd_draw_point(uint16_t x, uint16_t y, uint16_t data)
{
    lcd_set_xy(x, y);
    lcd_write_data(data >> 8);
    lcd_write_data(data);
}

/*!
    \brief      clear the lcd
    \param[in]  color: lcd display color
    \param[out] none
    \retval     none
*/
void lcd_clear(uint16_t color)
{
    unsigned int i, m;
    /* set lcd display region */
    lcd_set_region(0, 0, X_MAX_PIXEL - 1, Y_MAX_PIXEL - 1);
    LCD_RS_SET;

    LCD_CS_CLR;
    for(i = 0; i < Y_MAX_PIXEL; i ++) {
        for(m = 0; m < X_MAX_PIXEL; m ++) {
            spi_write_byte(SPI1, color >> 8);
            spi_write_byte(SPI1, color);
        }
    }
    LCD_CS_SET;
}

/*!
    \brief      bgr to rgb format conversion
    \param[in]  c: bgr color value
    \param[out] none
    \retval     rgb color value
*/
uint16_t lcd_bgr2rgb(uint16_t c)
{
    uint16_t r, g, b, rgb;
    b = (c >> 0) & 0x1f;
    g = (c >> 5) & 0x3f;
    r = (c >> 11) & 0x1f;
    rgb = (b << 11) + (g << 5) + (r << 0);
    return(rgb);
}

/*!
    \brief      gui circle
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[in]  r: the radius of circle
    \param[in]  fc: display color of font
    \param[out] none
    \retval     none
*/
void lcd_circle_draw(uint16_t x, uint16_t y, uint16_t r, uint16_t fc)
{
    unsigned short a, b;
    int c;
    a = 0;
    b = r;
    c = 3 - 2 * r;
    LCD_CS_CLR;

    while(a < b) {
        /* draw points on the lcd */
        lcd_draw_point(x + a, y + b, fc);
        lcd_draw_point(x - a, y + b, fc);
        lcd_draw_point(x + a, y - b, fc);
        lcd_draw_point(x - a, y - b, fc);
        lcd_draw_point(x + b, y + a, fc);
        lcd_draw_point(x - b, y + a, fc);
        lcd_draw_point(x + b, y - a, fc);
        lcd_draw_point(x - b, y - a, fc);

        if(c < 0) {
            c = c + 4 * a + 6;
        } else {
            c = c + 4 * (a - b) + 10;
            b -= 1;
        }
        a += 1;
    }
    if(a == b) {
        /* draw points on the lcd */
        lcd_draw_point(x + a, y + b, fc);
        lcd_draw_point(x + a, y + b, fc);
        lcd_draw_point(x + a, y - b, fc);
        lcd_draw_point(x - a, y - b, fc);
        lcd_draw_point(x + b, y + a, fc);
        lcd_draw_point(x - b, y + a, fc);
        lcd_draw_point(x + b, y - a, fc);
        lcd_draw_point(x - b, y - a, fc);
    }
    LCD_CS_SET;
}

/*!
    \brief      gui draw line
    \param[in]  x0: the x position of the start point
    \param[in]  y0: the y position of the start point
    \param[in]  x1: the x position of the end point
    \param[in]  y1: the y position of the end point
    \param[in]  color: lcd display color
    \param[out] none
    \retval     none
*/
void lcd_line_draw(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    /* - difference in x's
       - difference in y's
       - dx,dy * 2
       - amount in pixel space to move during drawing
       - amount in pixel space to move during drawing
       - the discriminant i.e. error i.e. decision variable
       - used for looping */
    int dx, dy, dx2, dy2, x_inc, y_inc, error, index;

    LCD_CS_CLR;

    lcd_set_xy(x0, y0);
    /* calculate x distance */
    dx = x1 - x0;
    /* calculate y distance */
    dy = y1 - y0;

    if(dx >= 0) {
        x_inc = 1;
    } else {
        x_inc = -1;
        dx = -dx;
    }

    if(dy >= 0) {
        y_inc = 1;
    } else {
        y_inc = -1;
        dy    = -dy;
    }

    dx2 = dx << 1;
    dy2 = dy << 1;

    if(dx > dy) {
        /* initialize error */
        error = dy2 - dx;
        /* draw the line */
        for(index = 0; index <= dx; index ++) {
            lcd_draw_point(x0, y0, color);
            /* test if error has overflowed */
            if(0 <= error) {
                error -= dx2;
                /* move to next line */
                y0 += y_inc;
            }
            /* adjust the error term */
            error += dy2;
            /* move to the next pixel */
            x0 += x_inc;
        }
    } else {
        /* initialize error term */
        error = dx2 - dy;
        /* draw the linedraw the line*/
        for(index = 0; index <= dy; index ++) {
            /* set the pixel */
            lcd_draw_point(x0, y0, color);

            /* test if error overflowed */
            if(0 <= error) {
                error -= dy2;
                /* move to next line */
                x0 += x_inc;
            }
            /* adjust the error term */
            error += dx2;

            /* move to the next pixel */
            y0 += y_inc;
        }
    }
    LCD_CS_SET;
}

/*!
    \brief      gui box
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[in]  w: the width of the box
    \param[in]  h: the high of the box
    \param[in]  bc: display background color
    \param[out] none
    \retval     none
*/
void lcd_rect_draw(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t line_color)
{
    LCD_CS_CLR;

    /* gui draw line*/
    lcd_line_draw(x, y, x + w, y, line_color);
    lcd_line_draw(x + w, y, x + w, y + h, line_color);
    lcd_line_draw(x, y + h, x + w, y + h, line_color);
    lcd_line_draw(x, y, x, y + h, line_color);

    LCD_CS_SET;
}

/*!
    \brief      lcd box
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[in]  w: the width of the box
    \param[in]  h: the high of the box
    \param[in]  bc: display background color
    \param[out] none
    \retval     none
*/
void lcd_box(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t bc)
{
    LCD_CS_CLR;

    /* gui draw line*/
    lcd_line_draw(x, y, x + w, y, 0xEF7D);
    lcd_line_draw(x + w - 1, y + 1, x + w - 1, y + 1 + h, 0x2965);
    lcd_line_draw(x, y + h, x + w, y + h, 0x2965);
    lcd_line_draw(x, y, x, y + h, 0xEF7D);
    lcd_line_draw(x + 1, y + 1, x + 1 + w - 2, y + 1 + h - 2, bc);

    LCD_CS_SET;
}

/*!
    \brief      lcd box2
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[in]  w: the width of the box
    \param[in]  h: the high of the box
    \param[in]  mode: display color combination mode
    \param[out] none
    \retval     none
*/
void lcd_box2(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t mode)
{
    LCD_CS_CLR;

    /* gui box2 display mode0 */
    if(0 == mode) {
        lcd_line_draw(x, y, x + w, y, 0xEF7D);
        lcd_line_draw(x + w - 1, y + 1, x + w - 1, y + 1 + h, 0x2965);
        lcd_line_draw(x, y + h, x + w, y + h, 0x2965);
        lcd_line_draw(x, y, x, y + h, 0xEF7D);
    }
    /* gui box2 display mode1 */
    if(1 == mode) {
        lcd_line_draw(x, y, x + w, y, 0x2965);
        lcd_line_draw(x + w - 1, y + 1, x + w - 1, y + 1 + h, 0xEF7D);
        lcd_line_draw(x, y + h, x + w, y + h, 0xEF7D);
        lcd_line_draw(x, y, x, y + h, 0x2965);
    }
    /* gui box2 display mode2 */
    if(2 == mode) {
        lcd_line_draw(x, y, x + w, y, 0xffff);
        lcd_line_draw(x + w - 1, y + 1, x + w - 1, y + 1 + h, 0xffff);
        lcd_line_draw(x, y + h, x + w, y + h, 0xffff);
        lcd_line_draw(x, y, x, y + h, 0xffff);
    }
    LCD_CS_SET;
}

/*!
    \brief      gui rect
    \param[in]  x1: the x position of the start point
    \param[in]  y1: the y position of the start point
    \param[in]  x2: the x position of the end point
    \param[in]  y2: the y position of the end point
    \param[in]  fc: display color of font
    \param[out] none
    \retval     none
*/
void lcd_rect_color_draw(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t fc)
{
    int ix, iy;
    LCD_CS_CLR;
    for(ix = x1; ix < x2; ix ++) {
        for(iy = y1; iy < y2; iy ++)
            /* set the pixel */
        {
            lcd_draw_point(ix, iy, fc);
        }
    }

    LCD_CS_SET;
}

/*!
    \brief      display button down
    \param[in]  x1: the x position of the start point
    \param[in]  y1: the y position of the start point
    \param[in]  x2: the x position of the end point
    \param[in]  y2: the y position of the end point
    \param[out] none
    \retval     none
*/
void display_button_down(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_CS_CLR;

    /* gui draw line with gray color*/
    lcd_line_draw(x1, y1, x2, y1, GRAY2);
    lcd_line_draw(x1 + 1, y1 + 1, x2, y1 + 1, GRAY1);
    lcd_line_draw(x1, y1, x1, y2, GRAY2);
    lcd_line_draw(x1 + 1, y1 + 1, x1 + 1, y2, GRAY1);

    /* gui draw line with white color*/
    lcd_line_draw(x1, y2, x2, y2, WHITE);
    lcd_line_draw(x2, y1, x2, y2, WHITE);
    LCD_CS_SET;
}

/*!
    \brief      display button up
    \param[in]  x1: the x position of the start point
    \param[in]  y1: the y position of the start point
    \param[in]  x2: the x position of the end point
    \param[in]  y2: the y position of the end point
    \param[out] none
    \retval     none
*/
void display_button_up(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_CS_CLR;

    /* gui draw line with white color*/
    lcd_line_draw(x1, y1, x2, y1, WHITE);
    lcd_line_draw(x1, y1, x1, y2, WHITE);

    /* gui draw line with gray color*/
    lcd_line_draw(x1 + 1, y2 - 1, x2, y2 - 1, GRAY1);
    lcd_line_draw(x1, y2, x2, y2, GRAY2);
    lcd_line_draw(x2 - 1, y1 + 1, x2 - 1, y2, GRAY1);
    lcd_line_draw(x2, y1, x2, y2, GRAY2);
    LCD_CS_SET;
}

/*!
    \brief      gui draw font to gbk16
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[in]  fc: display color of font
    \param[in]  bc: display background color
    \param[in]  *s: display char
    \param[out] none
    \retval     none
*/
void lcd_draw_font_gbk16(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char *s)
{
    unsigned char i, j;
    unsigned short k, x0;
    x0 = x;

    LCD_CS_CLR;
    while(*s) {
        /* ASCII character table from 32 to 128 */
        if(((uint8_t)(*s)) < 128) {
            k = *s;
            if(13 == k) {
                x = x0;
                y += 16;
            } else {
                if(k > 32) {
                    k -= 32;
                } else {
                    k = 0;
                }
                for(i = 0; i < 16; i ++)
                    for(j = 0; j < 8; j ++) {
                        if(asc16[k * 16 + i] & (0x80 >> j))
                            /* draw a point on the lcd */
                        {
                            lcd_draw_point(x + j, y + i, fc);
                        } else {
                            if(fc != bc)
                                /* draw a point on the lcd */
                            {
                                lcd_draw_point(x + j, y + i, bc);
                            }
                        }
                    }
                x += 8;
            }
            s ++;
        } else {
            for(k = 0; k < hz16_num; k ++) {
                if((hz16[k].Index[0] == *(s)) && (hz16[k].Index[1] == *(s + 1))) {
                    for(i = 0; i < 16; i ++) {
                        for(j = 0; j < 8; j ++) {
                            if(hz16[k].Msk[i * 2] & (0x80 >> j))
                                /* draw a point on the lcd */
                            {
                                lcd_draw_point(x + j, y + i, fc);
                            } else {
                                if(fc != bc)
                                    /* draw a point on the lcd */
                                {
                                    lcd_draw_point(x + j, y + i, bc);
                                }
                            }
                        }
                        for(j = 0; j < 8; j ++) {
                            if(hz16[k].Msk[i * 2 + 1] & (0x80 >> j))
                                /* draw a point on the lcd */
                            {
                                lcd_draw_point(x + j + 8, y + i, fc);
                            } else {
                                if(fc != bc)
                                    /* draw a point on the lcd */
                                {
                                    lcd_draw_point(x + j + 8, y + i, bc);
                                }
                            }
                        }
                    }
                }
            }
            s += 3;
            x += 16;
        }
    }
    LCD_CS_SET;
}

/*!
    \brief      gui draw font to gbk24
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[in]  fc: display color of font
    \param[in]  bc: display background color
    \param[in]  *s: display char
    \param[out] none
    \retval     none
*/
void lcd_draw_font_gbk24(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, char *s)
{
    unsigned char i, j;
    unsigned short k;

    LCD_CS_CLR;
    while(*s) {
        /* ASCII character table from 32 to 128 */
        if(((uint8_t)(*s)) < 0x80) {
            k = *s;
            if(k > 32) {
                k -= 32;
            } else {
                k = 0;
            }

            for(i = 0; i < 16; i ++)
                for(j = 0; j < 8; j ++) {
                    if(asc16[k * 16 + i] & (0x80 >> j))
                        /* draw a point on the lcd */
                    {
                        lcd_draw_point(x + j, y + i, fc);
                    } else {
                        if(fc != bc)
                            /* draw a point on the lcd */
                        {
                            lcd_draw_point(x + j, y + i, bc);
                        }
                    }
                }
            s ++;
            x += 8;
        } else {
            for(k = 0; k < hz24_num; k ++) {
                if((hz24[k].Index[0] == *(s)) && (hz24[k].Index[1] == *(s + 1))) {
                    for(i = 0; i < 24; i ++) {
                        for(j = 0; j < 8; j ++) {
                            if(hz24[k].Msk[i * 3] & (0x80 >> j))
                                /* draw a point on the lcd */
                            {
                                lcd_draw_point(x + j, y + i, fc);
                            } else {
                                if(fc != bc)
                                    /* draw a point on the lcd */
                                {
                                    lcd_draw_point(x + j, y + i, bc);
                                }
                            }
                        }
                        for(j = 0; j < 8; j ++) {
                            if(hz24[k].Msk[i * 3 + 1] & (0x80 >> j))
                                /* draw a point on the lcd */
                            {
                                lcd_draw_point(x + j + 8, y + i, fc);
                            } else {
                                if(fc != bc)
                                    /* draw a point on the lcd */
                                {
                                    lcd_draw_point(x + j + 8, y + i, bc);
                                }
                            }
                        }
                        for(j = 0; j < 8; j ++) {
                            if(hz24[k].Msk[i * 3 + 2] & (0x80 >> j))
                                /* draw a point on the lcd */
                            {
                                lcd_draw_point(x + j + 16, y + i, fc);
                            } else {
                                if(fc != bc)
                                    /* draw a point on the lcd */
                                {
                                    lcd_draw_point(x + j + 16, y + i, bc);
                                }
                            }
                        }
                    }
                }
            }
            s += 3;
            x += 24;
        }
    }
    LCD_CS_SET;
}

/*!
    \brief      gui draw font to num32
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[in]  fc: display color of font
    \param[in]  bc: display background color
    \param[in]  num: display num
    \param[out] none
    \retval     none
*/
void lcd_draw_font_num32(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint16_t num)
{
    unsigned char i, j, k, c;

    LCD_CS_CLR;
    for(i = 0; i < 32; i ++) {
        for(j = 0; j < 4; j++) {
            c = *(sz32 + num * 32 * 4 + i * 4 + j);
            for(k = 0; k < 8; k ++) {
                if(c & (0x80 >> k))
                    /* draw a point on the lcd */
                {
                    lcd_draw_point(x + j * 8 + k, y + i, fc);
                } else {
                    if(fc != bc)
                        /* draw a point on the lcd */
                    {
                        lcd_draw_point(x + j * 8 + k, y + i, bc);
                    }
                }
            }
        }
    }
    LCD_CS_SET;
}

/*!
    \brief      display image from buffer (RGB565 format)
    \param[in]  x: the x position of the start point
    \param[in]  y: the y position of the start point
    \param[in]  width: image width in pixels
    \param[in]  height: image height in pixels
    \param[in]  image_data: pointer to image data buffer (RGB565 format, size = width * height * 2)
    \param[out] none
    \retval     none
*/
void lcd_draw_image(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *image_data)
{
    uint32_t i, j;
    uint16_t color;
    
    /* set lcd display region */
    lcd_set_region(x, y, x + width - 1, y + height - 1);
    
    LCD_CS_CLR;
    LCD_RS_SET;  /* data mode */
    
    /* write pixel data */
    /* Fix vertical flip by writing rows in reverse order */
    for(j = 0; j < height; j++) {
        uint32_t row_offset = (height - 1 - j) * width * 2; /* Calculate row offset in reverse order */
        for(i = 0; i < width; i++) {
            /* RGB565 format: high byte first, then low byte */
            color = image_data[row_offset + i * 2] | (image_data[row_offset + i * 2 + 1] << 8);
            // Swap red and blue components (BBBBBGGGGGGRRRRR to RRRRRGGGGGGBBBBB)
            //color = ((color & 0x1F) << 11) | (color & 0x07E0) | ((color >> 11) & 0x1F);

            spi_write_byte(SPI1, color >> 8);   /* high byte */
            spi_write_byte(SPI1, color & 0xFF); /* low byte */
        }
    }

    LCD_CS_SET;
}

void Set_Lcd_High_Resistance(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_mode_set(GPIOB, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);
}