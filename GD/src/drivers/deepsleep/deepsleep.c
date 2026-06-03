#include <deepsleep.h>
#include "GPIO_init.h"
#include <Process_Packet.h>
#include "sprd_config.h"
#include <stdio.h>
#include <timer.h>
#include "Uart_init.h"

void mcu_enter_deepsleep(void)
{
  printf("start to enter deepsleep\n");

  /* 1.set lcd high resistance */
  Set_Lcd_High_Resistance();
  /* 2.reset PA14 for note spl bootup success */
  MCU_Set_Led_Down();

  Timer_handler_Clear();
  global_sio_handle.pkt_type = BSL_PKT_TYPE_MAX;

#ifdef UART_SPEED_UP
  com_usart_change_baud(CONFIG_BAUDRATE);
#endif

  rcu_usart_clock_config(RCU_USART0SRC_IRC8M);

  com_usart_init(CONFIG_BAUDRATE);

  nvic_irq_enable(USART0_IRQn, 0);

  usart_wakeup_mode_config(USART0, USART_WUM_STARTB);

  usart_enable(USART0);
  /* ensure USART is enabled */
  while(RESET == usart_flag_get(USART0, USART_FLAG_REA));

  /* check USART is not transmitting */
  while(SET == usart_flag_get(USART0, USART_FLAG_BSY));

  usart_wakeup_enable(USART0);

  /* enable the WUIE interrupt */
  usart_interrupt_enable(USART0, USART_INT_WU);
  rcu_periph_clock_enable(RCU_PMU);

  printf("mcu enter deepsleep\n");
  pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, WFI_CMD);

  // MCU_DeepSleepMode();
}