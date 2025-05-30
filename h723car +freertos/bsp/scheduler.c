#include "scheduler.h"

// 定义任务数组，包含多个任务结构体
task_struct task[] =
{
    // {led_proc, 200, 0},
    {usart_proc, 40, 0},
    {motor_proc, 50, 0},
    {JPEG_run, 1, 0}
};

// 定义任务数量变量
uint8_t task_num;

/**
 * @brief  初始化任务调度器，计算任务数量
 */
void scheduler_init(void)
{
    // 计算任务数量
    task_num = sizeof(task) / sizeof(task_struct);
}

/**
 * @brief  运行任务调度器，检查任务执行时间并执行任务
 */
void scheduler_run(void)
{
    uint32_t time_now = HAL_GetTick();
    
    for(uint8_t i = 0; i < task_num; i++)
    {
        // 检查任务是否到执行时间
        if(time_now - task[i].time_lastrun >= task[i].time_interval)
        {
            // 执行任务函数
            task[i].task_func();
            // 更新任务上次执行时间
            task[i].time_lastrun = time_now;
        }
    }
}


