#ifndef	MYUSART_H
#define	MYUSART_H

#include "bsp_system.h"
#include "usart.h"

// 定义UART1接收缓冲区
extern char recv_buf_uart1[200];

// 定义UART2接收缓冲区
extern char recv_buf_uart2[200];

// 定义UART3接收缓冲区
extern char recv_buf_uart3[200];

// 定义接收数据缓冲区
extern uint8_t recv_data[200];

// 处理UART数据接收和命令解析
void usart_proc(void);

// 通过UART发送格式化数据
void myusart_transmit(UART_HandleTypeDef *huart, char * formate, ...);


#endif

