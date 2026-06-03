#include <Process_Packet.h>
#include <timer.h>

void Timer_handler_Clear(void)
{
  memset(&timer_handle, 0, sizeof(send_timer_t));
}

void Timer_handler_Start(int times)
{
  memset(&timer_handle, 0, sizeof(send_timer_t));
  timer_handle.Timer_Start_Flag = 1;
  timer_handle.timeout_time = timer_handle.Timer_Count + times;
}