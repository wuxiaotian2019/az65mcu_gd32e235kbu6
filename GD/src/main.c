/* SPDX-License-Identifier: LicenseRef-Unisoc-General-1.0
 *
 * SPDX-FileCopyrightText: 2026 Unisoc (Shanghai) Technologies Co., Ltd
 *
 * Copyright 2026 Unisoc (Shanghai) Technologies Co., Ltd.
 *
 * Licensed under the Unisoc General Software License, version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.unisoc.com/en_us/license/UNISOC_GENERAL_LICENSE_V1.0-EN_US
 *
 * Software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 * See the Unisoc General Software License, version 1.0 for more details.
 */
#include "gd32e23x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "gd32e235c_eval.h"
#include "Spl_stdio.h"
#include "GPIO_init.h"
#include "Uart_init.h"
#include "Process_Packet.h"
#include "sprd_config.h"
#include <timer.h>

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    uint32_t last_print_time = 0, current_time = 0;
    static int initialized = 0;

wakeup:
    systick_config();

    /* initilize the com */
    com_gpio_init();
    com_usart_init(CONFIG_BAUDRATE);
    com1_gpio_init();
#ifdef UART_SPEED_UP
    com1_usart_init(CONFIG_BAUDRATE_SPEEDUP);
    printf("mcu wakeup: %d, change baudrate to 921600!\n", (global_sio_handle.sio_state == BSL_UART_WAKEUP_FLAG) ? 1 : 0);
#else
    com1_usart_init(CONFIG_BAUDRATE);
    printf("mcu wakeup: %d, use baudrate 115200 as default!\n", (global_sio_handle.sio_state == BSL_UART_WAKEUP_FLAG) ? 1 : 0);
#endif

    printf("Firmware built on: %s %s!\n", __DATE__, __TIME__);

    gd_eval_led_init(LED5);

    nvic_irq_enable(USART0_IRQn, 0);
    nvic_irq_enable(USART1_IRQn, 0);
    // nvic_irq_disable(SysTick_IRQn);

    sio_handle_init();
    init_key_detection();

    printf("uart stat:0x%x\n", USART_STAT(USART0));
    usart_interrupt_enable(USART0, USART_INT_RBNE);
    usart_interrupt_enable(USART0, USART_INT_IDLE);
    usart_interrupt_enable(USART0, USART_INT_ERR);
    usart_interrupt_enable(USART0, USART_INT_AM);
    usart_interrupt_enable(USART0, USART_INT_PERR);
    usart_receive_fifo_enable(USART0);

    Timer_handler_Start(START_TIMEOUT_TIME);
    while(1)
    {
        if (!initialized && is_long_key_or_nokey_press_power()) {
#ifdef MCU_SUPPORT_LCD_DISPLAY
            lcd_display();
#else
            // set led/backlight
            MCU_Set_Led_Up();
#endif
            MCU_Detect_Key_Pressed();
            initialized = 1;
        }
        Process_packets();
        if (global_sio_handle.sio_state == BSL_START_FLAG) {
            printf("1 send 0x7e!\n");
            usart_put_char(USART0,PKT_HDLC_FLAG);
            delay_1ms(1);
            usart_put_char(USART0,PKT_HDLC_FLAG);
            delay_1ms(1);
            usart_put_char(USART0,PKT_HDLC_FLAG);
            delay_1ms(1);
            usart_put_char(USART0,PKT_HDLC_FLAG);
            delay_1ms(1);
            usart_put_char(USART0,PKT_HDLC_FLAG);
            delay_1ms(1);
        } else if(global_sio_handle.sio_state == BSL_UART_ENUM_FLAG) {
            if (global_sio_handle.buf_ptr[0]==PKT_HDLC_START) {
                printf("2 receive 0x55!global_sio_handle.buf_size:0x%x, global_sio_handle.data_size:0x%x\n", global_sio_handle.buf_size, global_sio_handle.data_size);
                memset(global_sio_handle.buf_ptr, 0, global_sio_handle.buf_size);
                global_sio_handle.buf_size=0;
                global_sio_handle.data_size=0;
                global_sio_handle.pkt_state=PKT_NONE;
                // global_sio_handle.read_ptr = global_sio_handle.buf_ptr;
                usart_interrupt_enable(USART0, USART_INT_RBNE);
                usart_interrupt_enable(USART0, USART_INT_IDLE);

                usart_put_char(USART0,PKT_HDLC_FLAG);
                delay_1ms(1);
                usart_put_char(USART0,PKT_HDLC_FLAG);
                delay_1ms(1);
                usart_put_char(USART0,PKT_HDLC_FLAG);
                delay_1ms(1);

                global_sio_handle.sio_state = BSL_UART_RECIVE_FLAG;
                Timer_handler_Start(TRANS_TIMEOUT_TIME);
            } else {
                current_time = timer_handle.Timer_Count;
                if (last_print_time == 0 || current_time - last_print_time >= 1000) {
                    printf("wait for soc romcode uart enum\n");
                    last_print_time = current_time;
                }
            }
        } else if(global_sio_handle.sio_state == BSL_UART_WAKEUP_FLAG) {
            printf("mcu wakeup success\n");
            system_clock_config();
            rcu_usart_clock_config(RCU_USART0SRC_CKAPB2);
            initialized = 0;
            goto wakeup;
        } else if (global_sio_handle.sio_state == SPL_TRANSPORT_TIMEOUT) {
            /* reset soc for retry */
            printf("SPL_TRANSPORT_TIMEOUT, reset soc\n");
            MCU_Set_Soc_Pbint();
            com_usart_change_baud(CONFIG_BAUDRATE);
            Set_SPI_High_Resistance();
            sio_handle_init();
            Timer_handler_Start(PMIC_RESET_TIME);
        }
    }
}

#ifdef GD_ECLIPSE_GCC
/* retarget the C library printf function to the USART, in Eclipse GCC environment */
int __io_putchar(int ch)
{
    usart_data_transmit(USART1, (uint8_t) ch);
    while(RESET == usart_flag_get(USART1, USART_FLAG_TBE));
    return ch;
}
#else
/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART1, (uint8_t)ch);
    while(RESET == usart_flag_get(USART1, USART_FLAG_TBE));

    return ch;
}
#endif /* GD_ECLIPSE_GCC */
