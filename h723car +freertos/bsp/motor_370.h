#ifndef MOTOR_370_H
#define MOTOR_370_H

#include "bsp_system.h"
#include "tim.h"

// 编码器分辨率
#define ENCODER_RESOLUTION      448.8   // 编码器每转脉冲数，22*20.4

// 车轮半径（米）
#define WHEEL_RADIUS            0.04    // 车轮直径，单位米

// 车辆Y方向半轴距
#define D_HALF_X                     0.06     // 车辆Y方向半轴距

// 车辆X方向半轴距
#define D_HALF_Y                     0.095     // 车辆X方向半轴距

// 积分限幅
#define INTEGRAL_LIMIT 500

// 输出限幅
#define OUT_LIMIT 1000

// 比例系数
#define KP 500

// 积分系数
#define KI 500

// 微分系数
#define KD 0

// 圆周率
#define PI 3.1415926

// 定义电机状态枚举类型
typedef enum{
    stop = 0,  // 停止
    go = 1,    // 前进
    back = -1  // 后退
} motor_sta;

// 定义电机位置枚举类型
typedef enum{
    left_front = 0,  // 左前电机
    left_rear = 1,   // 左后电机
    right_rear = 2,  // 右后电机
    right_front = 3  // 右前电机
} motor_location;

// 定义PID控制器结构体
typedef struct {
    double kp;  // 比例系数
    double ki;  // 积分系数
    double kd;  // 微分系数
    double setpoint;  // 设定值
    double integral;  // 积分项
    double prev_error;  // 上一次误差
    double integral_limit;  // 积分限幅
    double output_limit;  // 输出限幅
} PID_Controller;

// 声明电机方向数组
extern int8_t direction_motor[4];   

// 声明电机编码器上一次时间戳数组
extern uint32_t t0_motor[4];

// 声明电机编码器当前时间戳数组
extern uint32_t t1_motor[4];

// 声明电机PWM值数组
extern int32_t motor_pwm[4];

// 声明车辆速度数组
extern double car_speed[3];

// 声明车辆速度设定值数组
extern double car_speed_set[3];

// 声明车轮速度数组
extern double wheel_speed[4];

// 声明车轮速度设定值数组
extern double wheel_speed_set[4];

// 设置车轮速度
void set_whell_speed(void);

// 初始化PID控制器
void pid_init(PID_Controller *pid);

// 计算PID输出
double pid_compute(PID_Controller *pid, double current_value);

// 控制电机状态
void control_motor_sta(uint8_t location, motor_sta sta);

// 控制电机速度
void control_motor_speed(void);

// 计算车辆速度
void calculate_speed(void);

// 初始化车辆
void car_init(void);

// 设置车辆速度
void set_car_speed(double car_vx, double car_vy, double car_w);

// 处理电机控制任务
void motor_proc(void);

/**************************** 说明 *******************/
/*
电机引脚连接说明:
CD的IN引脚和ENCODER的1、2引脚连接
0-1正转 1-0反转  00/11停止

转向和移动全部通过电机正反转实现

PF0 -----> AIN1     PF1 -----> AIN2
PF2 -----> BIN1     PF3 -----> BIN2
PF4 -----> CIN1     PF5 -----> CIN2
PF6 -----> DIN1     PF7 -----> DIN2
TIM23_CHANNEL_1 -----> APWM  PG12
TIM23_CHANNEL_2 -----> BPWM   PG13
TIM23_CHANNEL_3 -----> CPWM   PF8
TIM23_CHANNEL_4 -----> DPWM   PF9
TIM2_CHANNEL_1 -----> ENCODERA_A  PA15
TIM2_CHANNEL_2 -----> ENCODERA_B  PB3
TIM3_CHANNEL_1 -----> ENCODERB_A  PA7
TIM3_CHANNEL_2 -----> ENCODERB_B  PB4
TIM4_CHANNEL_1 -----> ENCODERC_A  PD12
TIM4_CHANNEL_2 -----> ENCODERC_B  PD13
TIM5_CHANNEL_1 -----> ENCODERD_A  PA0
TIM5_CHANNEL_2 -----> ENCODERD_B  PA1
*/
// 定义电机控制引脚宏
#define AIN1_ON        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_SET);
#define AIN1_OFF       HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_RESET);
#define AIN2_ON        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET);
#define AIN2_OFF       HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET);

#define BIN1_ON        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2, GPIO_PIN_SET);
#define BIN1_OFF       HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2, GPIO_PIN_RESET);
#define BIN2_ON        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_SET);
#define BIN2_OFF       HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_RESET);

#define CIN1_ON        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4, GPIO_PIN_SET);
#define CIN1_OFF       HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4, GPIO_PIN_RESET);
#define CIN2_ON        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_5, GPIO_PIN_SET);
#define CIN2_OFF       HAL_GPIO_WritePin(GPIOF, GPIO_PIN_5, GPIO_PIN_RESET);

#define DIN1_ON        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, GPIO_PIN_SET);
#define DIN1_OFF       HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, GPIO_PIN_RESET);
#define DIN2_ON        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_SET);
#define DIN2_OFF       HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_RESET);

// 定义车辆PWM定时器句柄
#define CAR_PWM_HTIM    htim23

// 定义车辆时间定时器句柄
#define CAR_TIME_HTIM   htim6

// 定义A电机编码器定时器句柄
#define A_HTIM          htim2

// 定义B电机编码器定时器句柄
#define B_HTIM          htim3

// 定义C电机编码器定时器句柄
#define C_HTIM          htim4

// 定义D电机编码器定时器句柄
#define D_HTIM          htim5

#endif

