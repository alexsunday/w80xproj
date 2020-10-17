/***************************************************************************** 
* 
* File Name : main.c
* 
* Description: main 
* 
* Copyright (c) 2014 Winner Micro Electronic Design Co., Ltd. 
* All rights reserved. 
* 
* Author : dave
* 
* Date : 2014-6-14
*****************************************************************************/ 
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "wm_include.h"
#include "wm_watchdog.h"
#include "wm_osal.h"

#include "uart_shell.h"


void UserMain(void)
{
  start_uart_shell();

  tls_watchdog_init(1000000);
   while(1) {
    tls_watchdog_clr();
    tls_os_time_delay(100);
  }
}


int ps_cmd(int argc, char** argv)
{
  char* buf = (char*)malloc(2048);
  if(!buf) {
    printf("no enough buffer!\r\n");
    return -1;
  }
  memset(buf, 0, 2048);

  vTaskList((signed char *)buf);
  
  char* ptr = buf;
  while(*ptr) {
    printf("%c", *ptr++);
  }

  return 0;
}


int reboot_cmd(int argc, char** argv)
{
  printf("rebooting...\r\n");
  tls_sys_reset();
  return 0;
}
