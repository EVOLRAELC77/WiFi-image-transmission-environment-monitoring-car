#include "myspi.h"
#include "spi.h"

// 定义ESP32 SPI句柄
#define ESP32SPI hspi4

// 定义ESP32握手信号GPIO端口
#define ESP32HAND_GPIO_PORT GPIOE

// 定义ESP32握手信号GPIO引脚
#define ESP32HAND_GPIO_PIN GPIO_PIN_10

/**
 * @brief  读取ESP32握手信号状态
 * @return 握手信号状态，1表示准备好，0表示未准备好
 */
static uint8_t handsta(void)
{
    if(HAL_GPIO_ReadPin(ESP32HAND_GPIO_PORT, ESP32HAND_GPIO_PIN) == GPIO_PIN_SET)
        return 1;
    else
        return 0;
}

/**
 * @brief  通过SPI发送数据
 * @param  data: 要发送的数据指针
 * @param  size: 要发送的数据大小
 */
void myspi_send(uint8_t* data, uint16_t size)
{
    TickType_t start_time = xTaskGetTickCount();
    
    // 等待ESP32准备好，设置超时时间为200ms
    while(handsta() != 1) 
    {
        if(xTaskGetTickCount() - start_time > 200) 
        {
            return; // 超时退出
        }
        // 延时200us
        mydelay_us(200);
    }
    
    // 通过SPI发送数据
    HAL_SPI_Transmit(&ESP32SPI, data, size, HAL_MAX_DELAY);
}

/**
 * @brief  发送JPEG数据到SPI
 * @param  data: 要发送的JPEG数据指针
 * @param  size: 要发送的JPEG数据大小
 */
void send_jpeg_data_spi(uint8_t* data, uint16_t size)
{
    for(uint8_t i = 0; i < 10; i++)
    {
        // 延时50us
        mydelay_us(50);
        // 每次发送4000字节数据
        myspi_send(data + i * 4000, 4000);	
    }	
}


