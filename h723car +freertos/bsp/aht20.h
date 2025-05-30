#ifndef __DHT20_H__
#define __DHT20_H__

#include "bsp_system.h"

// 初始化AHT20传感器
void AHT20_Init(void);

// 从AHT20传感器读取温度和湿度数据
void AHT20_Read(void);

#endif


