#ifndef __OV5640_SCCB_H
#define __OV5640_SCCB_H

#include "bsp_system.h"

// 定义SCCB通信的时钟线GPIO端口
#define SCCB_SCL_GPIO_PORT           GPIOF
// 定义SCCB通信的时钟线GPIO引脚
#define SCCB_SCL_GPIO_PIN            GPIO_PIN_14
// 使能SCCB通信的时钟线GPIO端口时钟
#define SCCB_SCL_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)
// 定义SCCB通信的数据线GPIO端口
#define SCCB_SDA_GPIO_PORT           GPIOF
// 定义SCCB通信的数据线GPIO引脚
#define SCCB_SDA_GPIO_PIN            GPIO_PIN_15
// 使能SCCB通信的数据线GPIO端口时钟
#define SCCB_SDA_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)

// 设置SCCB时钟线电平的宏定义
#define SCCB_SCL(x)                  do{ x ?                                                                                             \
                                                    HAL_GPIO_WritePin(SCCB_SCL_GPIO_PORT, SCCB_SCL_GPIO_PIN, GPIO_PIN_SET) :  \
                                                    HAL_GPIO_WritePin(SCCB_SCL_GPIO_PORT, SCCB_SCL_GPIO_PIN, GPIO_PIN_RESET); \
                                                }while(0)
// 设置SCCB数据线电平的宏定义
#define SCCB_SDA(x)                  do{ x ?                                                                                             \
                                                    HAL_GPIO_WritePin(SCCB_SDA_GPIO_PORT, SCCB_SDA_GPIO_PIN, GPIO_PIN_SET) :  \
                                                    HAL_GPIO_WritePin(SCCB_SDA_GPIO_PORT, SCCB_SDA_GPIO_PIN, GPIO_PIN_RESET); \
                                                }while(0)
// 读取SCCB数据线电平的宏定义
#define SCCB_READ_SDA()              HAL_GPIO_ReadPin(SCCB_SDA_GPIO_PORT, SCCB_SDA_GPIO_PIN)

// SCCB初始化函数声明
void sccb_init(void);
// SCCB三阶段写操作函数声明
void sccb_3_phase_write(uint8_t id_addr, uint16_t sub_addr, uint8_t dat);
// SCCB两阶段写操作函数声明
void sccb_2_phase_write(uint8_t id_addr, uint16_t sub_addr);
// SCCB两阶段读操作函数声明
void sccb_2_phase_read(uint8_t id_addr, uint8_t *dat);
												
#endif


