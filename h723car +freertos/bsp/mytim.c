#include "mytim.h"

/**
 * @brief  定时器输入捕获回调函数，处理电机编码器数据
 * @param  htim: 定时器句柄
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim == &A_HTIM)
    {
        if(direction_motor[0] == 0)
        {
            // 初始化电机方向
            direction_motor[0] = (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2) == 0 ? -1 : 1);
        }
        else
        {
            // 更新电机方向
            direction_motor[0] = ((__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2) == 0 ? -1 : 1) + direction_motor[0]) / 2;
        }
        
        // 更新电机编码器时间戳
        t0_motor[0] = t1_motor[0];
        t1_motor[0] = __HAL_TIM_GetCounter(&CAR_TIME_HTIM);
    }
    if(htim == &B_HTIM)
    {
        if(direction_motor[1] == 0)
        {
            // 初始化电机方向
            direction_motor[1] = (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3) == 0 ? -1 : 1);
        }
        else
        {
            // 更新电机方向
            direction_motor[1] = ((__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3) == 0 ? -1 : 1) + direction_motor[1]) / 2;
        }
        
        // 更新电机编码器时间戳
        t0_motor[1] = t1_motor[1];
        t1_motor[1] = __HAL_TIM_GetCounter(&CAR_TIME_HTIM);
    }
    if(htim == &C_HTIM)
    {
        if(direction_motor[2] == 0)
        {
            // 初始化电机方向
            direction_motor[2] = (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim4) == 0 ? -1 : 1);
        }
        else
        {
            // 更新电机方向
            direction_motor[2] = ((__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim4) == 0 ? -1 : 1) + direction_motor[2]) / 2;
        }
        
        // 更新电机编码器时间戳
        t0_motor[2] = t1_motor[2];
        t1_motor[2] = __HAL_TIM_GetCounter(&CAR_TIME_HTIM);
    }
    if(htim == &D_HTIM)
    {
        if(direction_motor[3] == 0)
        {
            // 初始化电机方向
            direction_motor[3] = (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim5) == 0 ? -1 : 1);
        }
        else
        {
            // 更新电机方向
            direction_motor[3] = ((__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim5) == 0 ? -1 : 1) + direction_motor[3]) / 2;
        }
        
        // 更新电机编码器时间戳
        t0_motor[3] = t1_motor[3];
        t1_motor[3] = __HAL_TIM_GetCounter(&CAR_TIME_HTIM);
    }
}

/**
 * @brief  微秒级延时函数
 * @param  time_us: 延时时间（微秒）
 */
void mydelay_us(uint16_t time_us)
{
    // 进入临界区，禁止中断
    taskENTER_CRITICAL();
    uint16_t now_time = 0xffff - time_us - 5;
    // 设置定时器计数器值
    __HAL_TIM_SetCounter(&DELAY_TIM, now_time);
    // 启动定时器
    HAL_TIM_Base_Start(&DELAY_TIM);
    
    // 等待定时器计数器值达到指定值
    while(now_time < 0xffff - 5)
    {
        now_time = __HAL_TIM_GetCounter(&DELAY_TIM);
    }
    
    // 停止定时器
    HAL_TIM_Base_Stop(&DELAY_TIM);
    // 退出临界区，允许中断
    taskEXIT_CRITICAL();
}


