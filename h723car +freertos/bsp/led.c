#include "led.h"

// 定义LED状态变量
uint8_t led;

/**
 * @brief  初始化LED，关闭LED
 */
void led_init(void)
{
    // 关闭LED
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}

/**
 * @brief  根据LED状态变量控制LED亮灭
 */
void led_disp(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, led ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

/**
 * @brief  处理LED闪烁逻辑，切换LED状态并更新显示
 */
void led_proc(void)
{
    // 切换LED状态
    led ^= 1;
    // 更新LED显示
    led_disp();
}