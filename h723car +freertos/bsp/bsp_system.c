#include "bsp_system.h"

void bsp_init(void)
{
	HAL_TIM_Base_Start(&CAR_TIME_HTIM);
	HAL_TIM_Encoder_Start_IT(&A_HTIM,TIM_CHANNEL_1);
	HAL_TIM_Encoder_Start(&A_HTIM,TIM_CHANNEL_2);
	HAL_TIM_Encoder_Start_IT(&B_HTIM,TIM_CHANNEL_1);
	HAL_TIM_Encoder_Start(&B_HTIM,TIM_CHANNEL_2);
	HAL_TIM_Encoder_Start_IT(&C_HTIM,TIM_CHANNEL_1);
	HAL_TIM_Encoder_Start(&C_HTIM,TIM_CHANNEL_2);
	HAL_TIM_Encoder_Start_IT(&D_HTIM,TIM_CHANNEL_1);
	HAL_TIM_Encoder_Start(&D_HTIM,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&CAR_PWM_HTIM,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&CAR_PWM_HTIM,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&CAR_PWM_HTIM,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&CAR_PWM_HTIM,TIM_CHANNEL_4);
	car_init();
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t*)recv_buf_uart1,200);  
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2,(uint8_t*)recv_buf_uart2,200);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart3,(uint8_t*)recv_buf_uart3,200);	
//	camera_JPEG_init();
}






