#ifndef BSP_SYSTEM_H
#define BSP_SYSTEM_H

#include "main.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"

#include "mytim.h"
#include "myusart.h"
#include "motor_370.h"
#include "ov5640.h"
#include "camera_run.h"
#include "myspi.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "aht20.h"

// 初始化板级支持包
void bsp_init(void);

#endif



