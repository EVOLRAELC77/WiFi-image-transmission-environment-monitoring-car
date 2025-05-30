#ifndef MYSPI_H
#define MYSPI_H

#include "bsp_system.h"

// 通过SPI发送数据
void myspi_send(uint8_t* data, uint16_t size);

// 发送JPEG数据到SPI
void send_jpeg_data_spi(uint8_t* data,uint16_t size);

#endif



