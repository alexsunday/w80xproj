
#include "wm_uart.h"
#include "wm_mem.h"

#include "shell/shell.h"
#include "shell/log.h"

#include "uart_shell.h"

#define SHELL_QUEUE_SIZE	32
#define SHELL_CONSOLE_BUF_SIZE   512
#define SHELL_TASK_SIZE      (2048)

void uartLogWriter(char* buffer, short len);

static tls_os_queue_t *shell_q = NULL;
static unsigned int shell_task_stack[SHELL_TASK_SIZE];

Shell shell;
char shell_buffer[512];
Log uartLog = {
  .write = uartLogWriter,
  .active = 1,
  .level = LOG_DEBUG
};


void shell_writer(const char word)
{
  tls_uart_write(TLS_UART_0, (char*)(&word), 1);
}

void uartLogWriter(char* buffer, short len)
{
  if(uartLog.shell) {
    shellWriteEndLine(uartLog.shell, buffer, len);
  }
}

s16 shell_console_rx(u16 len, void* user_data)
{
  u32 length = len;
  return tls_os_queue_send(shell_q, (void *)length, sizeof(u32));
}

void uart_shell_task_entry(void* params)
{
  int i;
  u32 length;
  int ret = 0;
  u8 shell_buf[128];

  tls_uart_set_baud_rate(TLS_UART_0, 115200);
  tls_uart_rx_callback_register(TLS_UART_0, shell_console_rx, NULL);

  shell.read = NULL;
  shell.write = shell_writer;
  shellInit(&shell, shell_buffer, sizeof(shell_buffer));
  logRegister(&uartLog, &shell);

  while(1) {
    tls_os_time_delay(1);
    tls_os_queue_receive(shell_q, (void **)&length, sizeof(u32), 0);
    tls_os_time_delay(1);
  
    ret = tls_uart_read(TLS_UART_0, shell_buf, length);
    if(ret <= 0) {
      break;
    }

    for(i=0; i!=ret; ++i) {
      shellHandler(&shell, shell_buf[i]);
    }
  }
}

void start_uart_shell()
{
  tls_os_status_t rc = tls_os_queue_create(&shell_q, SHELL_QUEUE_SIZE);
  if(rc != TLS_OS_SUCCESS) {
    printf("create shell queue failed.\r\n");
  }

  rc = tls_os_task_create(NULL, "shell", uart_shell_task_entry, NULL,
    (u8*)shell_task_stack,
    SHELL_TASK_SIZE * sizeof(u32), 30, 0);
  if(rc != TLS_OS_SUCCESS) {
    printf("create shell task failed.\r\n");
  }
}
