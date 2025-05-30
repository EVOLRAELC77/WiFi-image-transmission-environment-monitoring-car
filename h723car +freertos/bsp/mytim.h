#ifndef MYTIM_H
#define MYTIM_H

#include "bsp_system.h"
#include "tim.h"

// 定义延时定时器
#define DELAY_TIM htim7

// 微秒级延时函数
void mydelay_us(uint16_t time_us);

#endif


