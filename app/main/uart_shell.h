#ifndef _GUARD_H_UART_SHELL_H_
#define _GUARD_H_UART_SHELL_H_

#include <wm_type_def.h>

struct shell_console_t{
  int rx_data_len;
  u8 *rx_buf;		/*uart rx*/
  u8 msg_num;
};

void start_uart_shell();
void start_network();

#endif
