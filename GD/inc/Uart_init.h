#ifndef __UART_INIT_H
#define __UART_INIT_H

#include "gd32e23x.h"
#include "gd32e235c_eval.h"

//PKT_RECIVE_LIMIT
#define PKT_SEND_LIMIT          (0x800)       //Changed to 8KB to save memory

#define PKT_RECIVE_LIMIT            (0xF0)

void sio_handle_init(void);
void com_usart_init(uint32_t baudval);
void com1_usart_init(uint32_t baudval);
void com_usart_change_baud(uint32_t baudval);
void usart_put_char(uint32_t usart_periph, uint32_t data);

#endif
