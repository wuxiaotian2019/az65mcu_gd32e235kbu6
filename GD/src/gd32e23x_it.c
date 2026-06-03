/*!
    \file    gd32e23x_it.c
    \brief   interrupt service routines

    \version 2025-08-08, V2.4.0, firmware for GD32E23x
*/

/*
    Copyright (c) 2025, GigaDevice Semiconductor Inc.

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

#include "gd32e23x_it.h"
#include "main.h"
#include <stdio.h>
#include "systick.h"
#include "Process_Packet.h"
#include "sprd_config.h"
#include "GPIO_init.h"
#include "Spl_stdio.h"

#define SRAM_PARITY_CHECK_ERROR_HANDLE(s)    do{}while(1)

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
    if(SET == syscfg_flag_get(SYSCFG_SRAM_PCEF)) {
        SRAM_PARITY_CHECK_ERROR_HANDLE("SRAM parity check error error\r\n");
    } else {
        /* if NMI exception occurs, go to infinite loop */
        /* HXTAL clock monitor NMI error or NMI pin error */
        while(1) {
        }
    }
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
    /* if SVC exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
    /* if PendSV exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    delay_decrement();
    timer_handle.Timer_Count++;
    check_key_press_during_boot();

    if (timer_handle.Timer_Start_Flag) {
       if (timer_handle.Timer_Count >= timer_handle.timeout_time) {
            if (global_sio_handle.sio_state == BSL_UART_ENUM_FLAG) {
                printf("romcode enum timeout, mcu ready to enter deepsleep\n");
                // mcu_enter_deepsleep();
            } else {
                printf("timer tick reset\n");
                printf("global_sio_handle.sio_state:0x%x\n",global_sio_handle.sio_state);
                printf("global_sio_handle.pkt_state:0x%x\n", global_sio_handle.pkt_state);
                printf("global_sio_handle.data_size:0x%x\n", global_sio_handle.data_size);
                printf("global_sio_handle.read_ptr buf\n");
                hexdump(global_sio_handle.read_ptr, global_sio_handle.data_size);
                global_sio_handle.sio_state = SPL_TRANSPORT_TIMEOUT;
                MCU_Reset_Soc_Pbint();
            }
            timer_handle.Timer_Start_Flag = 0;
       }
    }
}

void USART0_IRQHandler(void)
{
    uint16_t ch=0;
    // uint8_t cnt=0;

    if(RESET != usart_interrupt_flag_get(EVAL_COM, USART_INT_FLAG_RBNE)) {
        /* receive data */
        // cnt=usart_receive_fifo_counter_number(EVAL_COM);
        ch=usart_data_receive(EVAL_COM);
        usart_interrupt_flag_clear(EVAL_COM, USART_INT_FLAG_RBNE);
        global_sio_handle.read_ptr[global_sio_handle.data_size++] = ch;
    } else if(RESET != usart_interrupt_flag_get(EVAL_COM, USART_INT_FLAG_IDLE)) {
       usart_interrupt_flag_clear(EVAL_COM,USART_INT_FLAG_IDLE);
       if(global_sio_handle.pkt_state == PKT_RECV) {
          global_sio_handle.sio_state = BSL_UART_RECIVE_FLAG;
          usart_interrupt_disable(EVAL_COM, USART_INT_IDLE);
          usart_interrupt_disable(EVAL_COM, USART_INT_RBNE);
       }
    } else if(RESET != usart_interrupt_flag_get(EVAL_COM, USART_INT_FLAG_WU)) {
        // printf("1\n");
        usart_interrupt_flag_clear(EVAL_COM, USART_INT_FLAG_WU);
        usart_flag_clear(EVAL_COM, USART_FLAG_WU);
        global_sio_handle.sio_state = BSL_UART_WAKEUP_FLAG;
    } else if(RESET != usart_interrupt_flag_get(EVAL_COM, USART_INT_FLAG_AM)) {
        // printf("4\n");
        usart_interrupt_flag_clear(EVAL_COM, USART_INT_FLAG_AM);
    } else if(RESET != usart_interrupt_flag_get(EVAL_COM, USART_INT_FLAG_PERR)) {
        // printf("5\n");
        usart_interrupt_flag_clear(EVAL_COM, USART_INT_FLAG_PERR);
    } else if(RESET != usart_interrupt_flag_get(EVAL_COM, USART_INT_FLAG_RBNE_ORERR)) {
        // printf("8\n");
        usart_interrupt_flag_clear(EVAL_COM, USART_INT_FLAG_RBNE_ORERR);
    } else if(RESET != usart_interrupt_flag_get(EVAL_COM, USART_INT_FLAG_ERR_NERR)) {
        // printf("11\n");
        usart_interrupt_flag_clear(EVAL_COM, USART_INT_FLAG_ERR_NERR);
    } else if(RESET != usart_interrupt_flag_get(EVAL_COM, USART_INT_FLAG_ERR_ORERR)) {
        // printf("12\n");
        usart_interrupt_flag_clear(EVAL_COM, USART_INT_FLAG_ERR_ORERR);
        usart_data_receive(EVAL_COM);
        usart_interrupt_flag_clear(EVAL_COM,USART_INT_FLAG_RBNE);
    } else if(RESET != usart_interrupt_flag_get(EVAL_COM, USART_INT_FLAG_ERR_FERR)) {
        // printf("13\n");
        usart_interrupt_flag_clear(EVAL_COM, USART_INT_FLAG_ERR_FERR);
    } else if(RESET != usart_interrupt_flag_get(EVAL_COM, USART_INT_FLAG_RFF)) {
        // printf("14\n");
        usart_interrupt_flag_clear(EVAL_COM, USART_INT_FLAG_RFF);
    }
}
