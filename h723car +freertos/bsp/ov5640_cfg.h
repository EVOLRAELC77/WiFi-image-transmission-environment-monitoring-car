#ifndef OV5640_CFG_H_
#define OV5640_CFG_H_

#include "main.h"

// OV5640固件下载地址
#define OV5640_FW_DOWNLOAD_ADDR 0x8000

// 定义OV5640寄存器配置结构体，包含寄存器地址和对应的数据
typedef struct
{
    uint16_t reg;  // 寄存器地址
    uint8_t dat;   // 寄存器数据
} ov5640_reg_cfg_t;

// 声明OV5640初始化配置数组
extern ov5640_reg_cfg_t ov5640_init_cfg[207];
//extern ov5640_reg_cfg_t ov5640_init_cfg[254];

// 声明OV5640 RGB565格式配置数组
extern ov5640_reg_cfg_t ov5640_rgb565_cfg[45];
//extern ov5640_reg_cfg_t ov5640_rgb565_cfg[2];

// 声明OV5640 JPEG格式配置数组
extern ov5640_reg_cfg_t ov5640_jpeg_cfg[41];

// 声明OV5640自动对焦固件数组
extern uint8_t ov5640_auto_focus_firmware[4077];

#endif

