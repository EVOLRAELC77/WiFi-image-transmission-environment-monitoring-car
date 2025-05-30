#include "myusart.h"

// 定义UART1接收缓冲区
char recv_buf_uart1[200];

// 定义UART2接收缓冲区
char recv_buf_uart2[200];

// 定义UART3接收缓冲区
char recv_buf_uart3[200];

// 定义接收数据缓冲区
uint8_t recv_data[200];

// 定义UART1接收通知标志
#define UART1_RECV_NOTIFICATION    (1UL << 0)

// 定义UART2接收通知标志
#define UART2_RECV_NOTIFICATION    (1UL << 1)

// 定义UART3接收通知标志
#define UART3_RECV_NOTIFICATION    (1UL << 2)

// 声明UART任务句柄
extern osThreadId_t uartTaskHandle;

/**
 * @brief  通过UART发送格式化数据
 * @param  huart: UART句柄
 * @param  formate: 格式化字符串
 * @param  ...: 可变参数列表
 */
void myusart_transmit(UART_HandleTypeDef *huart, char * formate, ...)
{
    static char string_uart[100];
    va_list ptr;

    // 初始化可变参数列表
    va_start(ptr, formate);
    // 格式化字符串
    vsprintf(string_uart, formate, ptr);
    // 结束可变参数列表
    va_end(ptr);
    
    // 通过UART发送格式化后的字符串
    HAL_UART_Transmit(huart, (uint8_t*)string_uart, strlen(string_uart), HAL_MAX_DELAY);
}

// 从UART读取格式化数据
//uint8_t vread(char *fmt, ...)
//{
//   int rc;
//   va_list arg_ptr;
//   va_start(arg_ptr, fmt);
//   rc = vscanf(fmt, arg_ptr);
//   va_end(arg_ptr);
//   return rc;
//}

/**
 * @brief  UART接收事件回调函数，处理UART数据接收并设置通知标志
 * @param  huart: UART句柄
 * @param  Size: 接收数据的大小
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    uint32_t notificationValue = 0;
    
    if(huart == &huart1) 
    {
        // 设置UART1接收通知标志
        notificationValue = UART1_RECV_NOTIFICATION;
        // 复制接收数据到recv_data缓冲区
        strncpy((char*)recv_data, recv_buf_uart1, Size); 
        recv_data[Size] = '\0';
    }
    //else if(huart == &huart2) 
    //{
    //    // 设置UART2接收通知标志
    //    notificationValue = UART2_RECV_NOTIFICATION;
    //    // 复制接收数据到recv_data缓冲区
    //    strncpy((char*)recv_data, recv_buf_uart2, Size); 
    //    recv_data[Size] = '\0';
    //}
    else if(huart == &huart3) 
    {
        // 设置UART3接收通知标志
        notificationValue = UART3_RECV_NOTIFICATION;
        // 复制接收数据到recv_data缓冲区
        strncpy((char*)recv_data, recv_buf_uart3, Size); 
        recv_data[Size] = '\0';
    }
    
    // 如果有有效的通知标志，则设置任务标志
    if (notificationValue != 0) 
    {
        osThreadFlagsSet(uartTaskHandle, notificationValue);
    }
}

/**
 * @brief  处理UART数据接收和命令解析
 */
void usart_proc(void)
{
    uint32_t flags;
    
    // 等待UART1或UART3接收通知标志
    flags = osThreadFlagsWait(
            UART1_RECV_NOTIFICATION | UART3_RECV_NOTIFICATION,
            osFlagsWaitAny | osFlagsNoClear,
            osWaitForever
        );
    
    if (flags & UART1_RECV_NOTIFICATION) 
    {
        // 清除UART1接收通知标志
        osThreadFlagsClear(UART1_RECV_NOTIFICATION);
        
        // 解析UART1接收的命令
        if(strcmp((char*)recv_data, "gomove") == 0)
        {
            // 增加前进速度
            car_speed_set[0] += 0.1;
            if(car_speed_set[0] >= 0.5)
                car_speed_set[0] = 0.5;
            set_whell_speed();
        }
        else if(strcmp((char*)recv_data, "backmove") == 0)
        {
            // 减少前进速度
            car_speed_set[0] -= 0.1;
            if(car_speed_set[0] <= -0.5)
                car_speed_set[0] = -0.5;
            set_whell_speed();
        }
        else if(strcmp((char*)recv_data, "leftmove") == 0)
        {
            // 增加向左移动速度
            car_speed_set[1] += 0.1;
            if(car_speed_set[1] >= 0.5)
                car_speed_set[1] = 0.5;
            set_whell_speed();
        }
        else if(strcmp((char*)recv_data, "rightmove") == 0)
        {
            // 减少向左移动速度
            car_speed_set[1] -= 0.1;
            if(car_speed_set[1] <= -0.5)
                car_speed_set[1] = -0.5;
            set_whell_speed();
        }
        else if(strcmp((char*)recv_data, "turnright") == 0)
        {
            // 增加右转速度
            car_speed_set[2] += 0.1;
            if(car_speed_set[2] >= 1.5)
                car_speed_set[2] = 1.5;
            set_whell_speed();
        }
        else if(strcmp((char*)recv_data, "turnleft") == 0)
        {
            // 减少右转速度
            car_speed_set[2] -= 0.1;
            if(car_speed_set[2] <= -1.5)
                car_speed_set[2] = -1.5;
            set_whell_speed();
        }
        else if(strcmp((char*)recv_data, "nomove") == 0)
        {
            // 停止移动
            car_speed_set[0] = 0;
            car_speed_set[1] = 0;
            car_speed_set[2] = 0;
            set_whell_speed();
        }
        else if(strcmp((char*)recv_data, "throttle") == 0)
        {
            // 设置前进速度为0.4
            car_speed_set[0] = 0.4;
            car_speed_set[1] = 0;
            car_speed_set[2] = 0;
            set_whell_speed();
        }
        
        // 重新启动UART1 DMA接收
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)recv_buf_uart1, 200);  
    }
    
    // 如果有UART2接收通知标志（未处理）
    //if (flags & UART2_RECV_NOTIFICATION) 
    //{
    //    // 清除UART2接收通知标志
    //    osThreadFlagsClear(UART2_RECV_NOTIFICATION);
    //    // 通过UART1发送UART2接收的数据
    //    myusart_transmit(&huart1, "%s", recv_data);
    //    // 重新启动UART2 DMA接收
    //    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, (uint8_t*)recv_buf_uart2, 200);            
    //}
    
    // UDP数据格式:   vx:+-%.2dvy+-:%.2dvw:+-%.3d    
    if (flags & UART3_RECV_NOTIFICATION) 
    {
        // 清除UART3接收通知标志
        osThreadFlagsClear(UART3_RECV_NOTIFICATION);
        
        // 检查UART3接收的数据格式是否正确
        if(recv_data[0] == 'v' && recv_data[1] == 'x' && recv_data[2] == ':'
            && recv_data[6] == 'v' && recv_data[7] == 'y' && recv_data[8] == ':'
            && recv_data[12] == 'v' && recv_data[13] == 'w' && recv_data[14] == ':'
            && (recv_data[3] == '+' || recv_data[3] == '-')
            && (recv_data[9] == '+' || recv_data[9] == '-')
            && (recv_data[15] == '+' || recv_data[15] == '-')
            && (recv_data[4] >= '0' && recv_data[4] <= '9')
            && (recv_data[5] >= '0' && recv_data[5] <= '9')
            && (recv_data[10] >= '0' && recv_data[10] <= '9')
            && (recv_data[11] >= '0' && recv_data[11] <= '9')
            && (recv_data[18] >= '0' && recv_data[18] <= '9')
            && (recv_data[16] >= '0' && recv_data[16] <= '9')
            && (recv_data[17] >= '0' && recv_data[17] <= '9')
            )
        {
            // 解析速度数据
            int16_t vx = (recv_data[4] - '0') * 10 + (recv_data[5] - '0');
            int16_t vy = (recv_data[10] - '0') * 10 + (recv_data[11] - '0');
            int16_t vw = (recv_data[16] - '0') * 100 + (recv_data[17] - '0') * 10 + (recv_data[18] - '0');
            
            // 限制速度范围
            if(vx > 50)
                vx = 50;
            if(vy > 50)
                vy = 50;
            if(vw > 150)
                vw = 150;
            
            // 设置车辆速度
            car_speed_set[0] = vx / 100.0 * (recv_data[3] == '+' ? 1 : (-1));
            car_speed_set[1] = vy / 100.0 * (recv_data[9] == '+' ? 1 : (-1));
            car_speed_set[2] = vw / 100.0 * (recv_data[15] == '+' ? 1 : (-1));
            
            // 更新车轮速度
            set_whell_speed();            
        }
        else
        {
            // 速度设置为0
            car_speed_set[0] = 0;
            car_speed_set[1] = 0;
            car_speed_set[2] = 0;
            set_whell_speed();
        }

        // 重新启动UART3 DMA接收
        HAL_UARTEx_ReceiveToIdle_DMA(&huart3, (uint8_t*)recv_buf_uart3, 200);
    }
}


