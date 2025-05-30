#include "camera_run.h"
#include "dcmi.h"

#define JPEG_OUTPUT_WIDTH  720
#define JPEG_OUTPUT_HEIGHT 540
#define JPEG_BUF_SIZE  (100*1024)

/* JPEG图像缓存空间 */
//volatile uint32_t jpeg_buf[2][JPEG_BUF_SIZE / sizeof(uint32_t)]  __attribute__((section(".bss.ARM.__at_0XC0000000")));
volatile uint32_t jpeg_buf[2][JPEG_BUF_SIZE / sizeof(uint32_t)];

volatile uint8_t dcmi_frame;
volatile uint8_t jpeg_process;


volatile uint8_t dcmi_buf_using;
volatile uint16_t frame_wait_time;

volatile uint8_t jpeg_uart_idle=1;
uint8_t *jpeg_buf_temp;
volatile uint16_t jpeg_len;
uint32_t jpeg_index;
uint32_t jpeg_start_index;
uint32_t jpeg_end_index;

extern osSemaphoreId_t cam_frame_semaphoreHandle;



void camera_JPEG_init(void)
{
	uint8_t ret;
    ret  = ov5640_init();                                               /* 初始化OV5640模块 */
    ret += ov5640_set_output_format(OV5640_OUTPUT_FORMAT_JPEG);   /* 设置OV5640输出JPEG图像数据 */

    ret += ov5640_auto_focus_init();                                    /* 初始化OV5640模块自动对焦 */
    ret += ov5640_auto_focus_continuance();                             /* OV5640模块持续自动对焦 */
//    ret += ov5640_set_light_mode(OV5640_LIGHT_MODE_ADVANCED_AWB);   /* 设置OV5640模块灯光模式 */
//    ret += ov5640_set_color_saturation(OV5640_COLOR_SATURATION_4);  /* 设置OV5640模块色彩饱度 */
//    ret += ov5640_set_brightness(OV5640_BRIGHTNESS_4);              /* 设置OV5640模块亮度 */
//    ret += ov5640_set_contrast(OV5640_CONTRAST_4);                  /* 设置OV5640模块对比度 */
//    ret += ov5640_set_hue(OV5640_HUE_6);                            /* 设置OV5640模块色相 */
//    ret += ov5640_set_special_effect(OV5640_SPECIAL_EFFECT_NORMAL); /* 设置OV5640模块特殊效果 */
//    ret += ov5640_set_exposure_level(OV5640_EXPOSURE_LEVEL_5);      /* 设置OV5640模块曝光度 */
//    ret += ov5640_set_sharpness_level(OV5640_SHARPNESS_OFF);        /* 设置OV5640模块锐度 */
//    ret += ov5640_set_mirror_flip(OV5640_MIRROR_FLIP_1);            /* 设置OV5640模块镜像/翻转 */
//    ret += ov5640_set_test_pattern(OV5640_TEST_PATTERN_OFF);        /* 设置OV5640模块测试图案 */
//    ret += ov5640_set_pre_scaling_window(4, 0);
	ret += ov5640_set_output_size(JPEG_OUTPUT_WIDTH, JPEG_OUTPUT_HEIGHT);         /* 设置OV5640模块输出图像尺寸 */

    if (ret != 0)
    {
        while (1)
        {

        }
    }
}


void ov5640_dcmi_init()
{
	hdcmi.Init.JPEGMode=DCMI_JPEG_ENABLE;
	memset((void*)jpeg_buf,0,JPEG_BUF_SIZE);
	HAL_DCMI_Init(&hdcmi);
}

/**
 * @brief       开始OV5640模块DCMI接口DMA传输
 * @param       dts_addr        : 帧数据的接收目的地址
 *              len             : 传输的帧数据大小
 * @retval      无
 */
void ov5640_dcmi_start(uint32_t dts_addr,uint32_t len)
{
	/* 清空帧接收完成标记
	 * 使能DCMI帧接收中断
	 */

	memset((void*)jpeg_buf,0,JPEG_BUF_SIZE);
	jpeg_len=0;
//	dcmi_frame=0;	
	DCMI->IER=0x00;
	__HAL_DCMI_ENABLE_IT(&hdcmi, DCMI_IT_FRAME);
	__HAL_DMA_SET_COUNTER(hdcmi.DMA_Handle,len);
	HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT, dts_addr, len);
}


void ov5640_dcmi_stop(void)
{
	__HAL_DMA_DISABLE(hdcmi.DMA_Handle);//关闭DMA
	HAL_DCMI_Stop(&hdcmi);
}



/**
 * @brief      DCMI接口帧中断回调函数
 * @param
 * @retval
 */
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	osSemaphoreRelease(cam_frame_semaphoreHandle);
//	dcmi_frame = 1;  //帧中断完成标志置位
	ov5640_dcmi_stop();
}



void JPEG_run(void) 
{
	switch(jpeg_process)
	{
		case 0: //初始化并开始
		{
			ov5640_dcmi_stop();
			ov5640_dcmi_init();
			ov5640_dcmi_start((uint32_t)jpeg_buf[dcmi_buf_using],JPEG_BUF_SIZE/4);
			jpeg_process=1;
		}break;
		case 1://等待一帧接收完成
		{
			osStatus_t ret = osSemaphoreAcquire(cam_frame_semaphoreHandle,5000);
			if(ret!= osOK)
			{
				jpeg_process=0;
			}
			else 
			{
				frame_wait_time=0;
				uint8_t *jpeg_buf_temp;
				uint32_t jpeg_index;
				jpeg_buf_temp = (uint8_t *)jpeg_buf;
				/* 获取JPEG图像数据起始位置 */
				for (jpeg_start_index=UINT32_MAX, jpeg_index=0; jpeg_index<JPEG_BUF_SIZE-1; jpeg_index++)
				{
					if ((jpeg_buf_temp[jpeg_index] == 0xFF) && (jpeg_buf_temp[jpeg_index + 1] == 0xD8))
					{
						jpeg_start_index = jpeg_index;
						break;
					}
				}
				if (jpeg_start_index == UINT32_MAX)
				{
					jpeg_process=0;
					return;
				}

				/* 获取JPEG图像数据结束位置 */
				for (jpeg_end_index=UINT32_MAX, jpeg_index=jpeg_start_index; jpeg_index<JPEG_BUF_SIZE-1; jpeg_index++)
				{
					if ((jpeg_buf_temp[jpeg_index] == 0xFF) && (jpeg_buf_temp[jpeg_index + 1] == 0xD9))
					{
						jpeg_end_index = jpeg_index;
						break;
					}
				}
				if (jpeg_end_index == UINT32_MAX)
				{
					jpeg_process=0;
					return;
				}
				/* 获取JPEG图像数据的长度 */
				jpeg_len = jpeg_end_index - jpeg_start_index + 1 + 1;   //+最后一个0xD9和减法算数量+1		
				*((uint8_t *)jpeg_buf[dcmi_buf_using]+jpeg_end_index+2)=jpeg_len>>8;
				*((uint8_t *)jpeg_buf[dcmi_buf_using]+jpeg_end_index+3)=jpeg_len&0x00ff;
				send_jpeg_data_spi((uint8_t *)jpeg_buf[dcmi_buf_using]+jpeg_start_index,40000);			
				ov5640_dcmi_start((uint32_t)jpeg_buf[dcmi_buf_using],JPEG_BUF_SIZE/4);
			}
		}break;
	}
}


