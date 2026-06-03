#include "Uart_init.h"
#include "gd32e23x.h"
#include "gd32e235c_eval.h"
#include "Process_Packet.h"
#include <stdio.h>
#include "Spl_stdio.h"

unsigned int    recv_data_buffer_array[PKT_RECIVE_LIMIT >> 2];
unsigned int    handle_data_buffer_array[PKT_RECIVE_LIMIT >> 2];
unsigned int    send_data_buffer_array[PKT_SEND_LIMIT >> 2];

void sio_handle_init(void)
{
    global_sio_handle.pkt_state = PKT_NONE;
    global_sio_handle.sio_state = BSL_UART_ENUM_FLAG;
    global_sio_handle.buf_size = 0;
    global_sio_handle.data_size = 0;
    global_sio_handle.buf_ptr = (unsigned char *)handle_data_buffer_array;   // 256 byte
    global_sio_handle.write_ptr = (unsigned char *)send_data_buffer_array;  // 1024 byte
    global_sio_handle.read_ptr = (unsigned char *)recv_data_buffer_array;
    global_sio_handle.pkt_type = BSL_PKT_TYPE_MAX;
    global_sio_handle.pkt_count = 0;
}
/*!
    \brief      initilize the USART configuration of the com
    \param[in]  none
    \param[out] none
    \retval     none
*/
void com_usart_init(uint32_t baudval)
{
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* USART configure */
    usart_deinit(USART0);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_oversample_config(USART0, USART_OVSMOD_8);
    usart_baudrate_set(USART0, baudval);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);

    usart_enable(USART0);
}

/*!
    \brief      initilize the USART configuration of the com
    \param[in]  none
    \param[out] none
    \retval     none
*/
void com1_usart_init(uint32_t baudval)
{	
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART1);

    /* USART configure */
    usart_deinit(USART1);
    usart_word_length_set(USART1, USART_WL_8BIT);
    usart_stop_bit_set(USART1, USART_STB_1BIT);
    usart_parity_config(USART1, USART_PM_NONE);
    usart_oversample_config(USART1, USART_OVSMOD_8);
    usart_baudrate_set(USART1, baudval);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);

    usart_enable(USART1);
}

void com_usart_change_baud(uint32_t baudval)
{
    nvic_irq_disable(USART0_IRQn);
    usart_interrupt_disable(USART0, USART_INT_RBNE);
    usart_interrupt_disable(USART0, USART_INT_IDLE);
    com_usart_init(baudval);
    nvic_irq_enable(USART0_IRQn, 0);
    usart_interrupt_enable(USART0, USART_INT_RBNE);
    usart_interrupt_enable(USART0, USART_INT_IDLE);
}

void usart_put_char(uint32_t usart_periph, uint32_t data)
{
  usart_data_transmit(usart_periph,data); 
  while (RESET == usart_flag_get(usart_periph, USART_FLAG_TBE));
}