

#ifndef CAMERA_RUN_H_
#define CAMERA_RUN_H_

#include "bsp_system.h"

extern volatile uint8_t jpeg_uart_idle;

void camera_JPEG_init(void);
void JPEG_run(void);
void ov5640_dcmi_stop(void);
#endif /* CAMERA_RUN_H_ */


