#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "bsp_system.h"

// 定义任务结构体，包含任务函数指针、时间间隔和上次运行时间
typedef struct {
    void (*task_func)();  // 任务函数指针
    uint32_t time_interval;  // 任务执行时间间隔
    uint32_t time_lastrun;  // 任务上次执行时间
} task_struct;

// 初始化任务调度器
void scheduler_init(void);

// 运行任务调度器
void scheduler_run(void);

#endif


