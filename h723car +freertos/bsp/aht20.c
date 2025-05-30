#include "aht20.h"
#include "i2c.h"

// 定义AHT20传感器的I2C地址
#define AHT20_ADDRESS 0x70

// 定义温度变量
float temperature;

// 定义湿度变量
float humidity;

/**
 * @brief  初始化AHT20传感器
 */
void AHT20_Init()
{
    uint8_t readBuffer;
    // 延时40ms等待传感器稳定
    osDelay(40);
    // 从AHT20传感器读取一个字节数据
    HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDRESS, &readBuffer, 1, HAL_MAX_DELAY);
    // 检查传感器是否已校准，如果未校准则发送校准命令
    if ((readBuffer & 0x08) == 0x00)
    {
        uint8_t sendBuffer[3] = {0xBE, 0x08, 0x00};
        HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDRESS, sendBuffer, 3, HAL_MAX_DELAY);
    }
}

/**
 * @brief  从AHT20传感器读取温度和湿度数据
 */
void AHT20_Read(void)
{
    uint8_t sendBuffer[3] = {0xAC, 0x33, 0x00};
    uint8_t readBuffer[6] = {0};

    // 发送读取数据命令
    HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDRESS, sendBuffer, 3, HAL_MAX_DELAY);
    // 延时75ms等待传感器测量完成
    osDelay(75);
    // 从AHT20传感器读取6字节数据
    HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDRESS, readBuffer, 6, HAL_MAX_DELAY);

    // 检查数据是否有效
    if ((readBuffer[0] & 0x80) == 0x00)
    {
        uint32_t data = 0;
        // 解析湿度数据
        data = ((uint32_t)readBuffer[3] >> 4) + ((uint32_t)readBuffer[2] << 4) + ((uint32_t)readBuffer[1] << 12);
        humidity = data * 100.0f / (1 << 20);

        // 解析温度数据
        data = (((uint32_t)readBuffer[3] & 0x0F) << 16) + ((uint32_t)readBuffer[4] << 8) + (uint32_t)readBuffer[5];
        temperature = data * 200.0f / (1 << 20) - 50;

        // 通过UART3发送温度和湿度数据
        myusart_transmit(&huart3, "%2.1f%2.1f", temperature, humidity);
    } 
}


