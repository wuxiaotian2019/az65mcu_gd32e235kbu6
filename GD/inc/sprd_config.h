#ifndef _SPRD_CONFIG_H
#define _SPRD_CONFIG_H

/* boot once the key is held for KEYPRESS_CHECKTIME; set to 3s after on-board
   tuning so a 3s press is treated as a real power-on and no longer leaves the
   platform in a flash-backlight-then-shut-down state */
#define KEYPRESS_CHECKTIME  3000
#define KEYPRESS_TIME  (KEYPRESS_CHECKTIME - 500)

#define CONFIG_BAUDRATE    115200U
// #define DEBUG_DATA
#define UART_SPEED_UP
#ifdef UART_SPEED_UP
#define CONFIG_BAUDRATE_SPEEDUP    921600U
#endif
#define CONFIG_WAKEUP_FROM_DEEPSLEEP
#define MCU_SUPPORT_LCD_DISPLAY

extern void system_clock_config(void);
#endif
