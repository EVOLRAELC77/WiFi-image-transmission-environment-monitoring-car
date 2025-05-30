#include "motor_370.h"

// 为四个电机分别创建PID控制器
PID_Controller pid_controllers[4];
// 每个电机脉冲对应的距离
double motor_pulse_meter;

// 四个电机的方向，-1表示反向，1表示正向
int8_t direction_motor[4];   
// 四个电机上上次捕获的时间
uint32_t t0_motor[4];
// 四个电机上次捕获的时间
uint32_t t1_motor[4];
// 四个电机的PWM控制值
int32_t motor_pwm[4];

// 小车的三个速度分量，分别为x方向速度、y方向速度和角速度
double car_speed[3];
// 小车的三个速度设定值，分别为x方向速度设定值、y方向速度设定值和角速度设定值
double car_speed_set[3];
// 四个电机的实际速度
double wheel_speed[4];
// 四个电机的速度设定值
double wheel_speed_set[4];

// 初始化PID控制器
void pid_init(PID_Controller *pid) 
{
    // 设置PID控制器的比例系数
    pid->kp = KP;
    // 设置PID控制器的积分系数
    pid->ki = KI;
    // 设置PID控制器的微分系数
    pid->kd = KD;
    // 设置PID控制器的设定值为0
    pid->setpoint = 0;
    // 初始化积分项为0
    pid->integral = 0;
    // 初始化上一次的误差为0
    pid->prev_error = 0;
    // 设置积分项的上限
    pid->integral_limit = INTEGRAL_LIMIT;
    // 设置输出的上限
    pid->output_limit = OUT_LIMIT;
}

// PID计算函数，根据当前值和设定值计算输出
double pid_compute(PID_Controller *pid, double current_value)
{
    // 计算当前误差，即设定值与当前值的差值
    double error = pid->setpoint - current_value;
    // 积分项累加当前误差
    pid->integral += error;

    // 积分限幅，防止积分项过大
    if (pid->integral > pid->integral_limit) {
        pid->integral = pid->integral_limit;
    } else if (pid->integral < -pid->integral_limit) {
        pid->integral = -pid->integral_limit;
    }

    // 计算微分项，即当前误差与上一次误差的差值
    double derivative = error - pid->prev_error;
    // 根据PID公式计算输出
    double output = pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;

    // 输出限幅，防止输出过大
    if (output > pid->output_limit) {
        output = pid->output_limit;
    } else if (output < -pid->output_limit) {
        output = -pid->output_limit;
    }

    // 更新上一次的误差为当前误差
    pid->prev_error = error;
    return output;
}

// 控制电机状态的函数，根据电机位置和状态控制电机
void control_motor_sta(/*motor_location location,*/uint8_t location, motor_sta sta)
{
    // 根据电机位置选择不同的控制逻辑
    switch(location)
    {
        case 0:
        {
            // 如果电机状态为停止
            if(sta == stop)
            {
                // 关闭电机的两个控制引脚
                AIN1_OFF;
                AIN2_OFF;
            }
            // 如果电机状态为后退
            else if(sta == back)
            {
                // 关闭AIN1，打开AIN2
                AIN1_OFF;
                AIN2_ON;
            }
            // 如果电机状态为前进
            else if(sta == go)
            {
                // 打开AIN1，关闭AIN2
                AIN1_ON;
                AIN2_OFF;
            }
        }break;
        case 1:
        {
            // 如果电机状态为停止
            if(sta == stop)
            {
                // 关闭电机的两个控制引脚
                BIN1_OFF;
                BIN2_OFF;
            }
            // 如果电机状态为后退
            else if(sta == back)
            {
                // 关闭BIN1，打开BIN2
                BIN1_OFF;
                BIN2_ON;
            }
            // 如果电机状态为前进
            else if(sta == go)
            {
                // 打开BIN1，关闭BIN2
                BIN1_ON;
                BIN2_OFF;
            }
        }break;
        case 2:
        {
            // 如果电机状态为停止
            if(sta == stop)
            {
                // 关闭电机的两个控制引脚
                CIN1_OFF;
                CIN2_OFF;
            }
            // 如果电机状态为后退
            else if(sta == back)
            {
                // 关闭CIN1，打开CIN2
                CIN1_OFF;
                CIN2_ON;
            }
            // 如果电机状态为前进
            else if(sta == go)
            {
                // 打开CIN1，关闭CIN2
                CIN1_ON;
                CIN2_OFF;
            }
        }break;
        case 3:
        {
            // 如果电机状态为停止
            if(sta == stop)
            {
                // 关闭电机的两个控制引脚
                DIN1_OFF;
                DIN2_OFF;
            }
            // 如果电机状态为后退
            else if(sta == back)
            {
                // 关闭DIN1，打开DIN2
                DIN1_OFF;
                DIN2_ON;
            }
            // 如果电机状态为前进
            else if(sta == go)
            {
                // 打开DIN1，关闭DIN2
                DIN1_ON;
                DIN2_OFF;
            }
        }break;
        default:break;
    }
}

// 控制电机速度的函数，根据PWM值控制电机的速度和方向
void control_motor_speed(void)
{
    // 遍历四个电机
    for(uint8_t i = 0; i < 4; i++)
    {
        // 如果PWM值大于0，电机正转
        if(motor_pwm[i] > 0)
        {
            // 设置电机状态为前进
            control_motor_sta(i, go);
            // 设置定时器的比较值，控制PWM输出
            __HAL_TIM_SetCompare(&CAR_PWM_HTIM, TIM_CHANNEL_1 + 4 * i, (uint16_t)motor_pwm[i]);
        }
        // 如果PWM值小于0，电机反转
        else if(motor_pwm[i] < 0)
        {
            // 设置电机状态为后退
            control_motor_sta(i, back);
            // 设置定时器的比较值，控制PWM输出
            __HAL_TIM_SetCompare(&CAR_PWM_HTIM, TIM_CHANNEL_1 + 4 * i, (uint16_t)(-1 * motor_pwm[i]));
        }
        // 如果PWM值等于0，电机停止
        else
        {
            // 设置电机状态为停止
            control_motor_sta(i, stop);
            // 设置定时器的比较值为0，停止PWM输出
            __HAL_TIM_SetCompare(&CAR_PWM_HTIM, TIM_CHANNEL_1 + 4 * i, 0);
        }
    }
}

// 计算电机速度的函数，根据定时器捕获的时间计算电机的实际速度
void calculate_speed(void)
{
    // 用于临时存储t0_motor的值
    uint32_t t0_motor_copy[4];
    // 用于临时存储t1_motor的值
    uint32_t t1_motor_copy[4];
    // 用于临时存储电机方向的值
    int8_t direction_motor_temp[4];
    // 禁止中断，防止数据读取过程中被中断修改
    __disable_irq();
    // 复制t0_motor的值到t0_motor_copy
    memcpy(t0_motor_copy, t0_motor, sizeof(t0_motor_copy));
    // 复制t1_motor的值到t1_motor_copy
    memcpy(t1_motor_copy, t1_motor, sizeof(t1_motor_copy));
    // 复制电机方向的值到direction_motor_temp
    memcpy(direction_motor_temp, direction_motor, sizeof(direction_motor_temp));
    // 允许中断
    __enable_irq();
    // 获取当前定时器CAR_TIME_HTIM的计数值
    uint32_t now_time = __HAL_TIM_GetCounter(&CAR_TIME_HTIM);
    // 遍历四个电机
    for(uint8_t i = 0; i < 4; i++)
    {
        // 计算时间间隔，取当前时间与上次捕获时间的差值和上次捕获时间与上上次捕获时间的差值中的较大值
        uint32_t T_time = (now_time - t1_motor_copy[i]) > (t1_motor_copy[i] - t0_motor_copy[i]) ? now_time - t1_motor_copy[i] : t1_motor_copy[i] - t0_motor_copy[i];
        // 根据时间间隔和电机方向计算电机的实际速度
        wheel_speed[i] = direction_motor_temp[i] * motor_pulse_meter / T_time * 10000;     
    }
    // 根据四个电机的速度计算小车x方向的速度
    car_speed[0] = (wheel_speed[1] + wheel_speed[2]) / 2;
    // 根据四个电机的速度计算小车y方向的速度
    car_speed[1] = (wheel_speed[1] - wheel_speed[0]) / 2;
    // 根据四个电机的速度计算小车的角速度
    car_speed[2] = (wheel_speed[3] - wheel_speed[1]) / 2 / (D_HALF_X + D_HALF_Y);
}

// 小车初始化函数，初始化电机参数和PID控制器
void car_init(void)
{
    // 计算每个电机脉冲对应的距离
    motor_pulse_meter = 2 * PI * WHEEL_RADIUS / ENCODER_RESOLUTION;
    // 初始化四个PID控制器
    for (int i = 0; i < 4; i++) {
        pid_init(&pid_controllers[i]);
    }
    // 设置小车的初始速度为0
    set_car_speed(0, 0, 0);
}

// 设置小车速度的函数，根据输入的x方向速度、y方向速度和角速度设置小车的速度
void set_car_speed(double car_vx, double car_vy, double car_w)
{
    // 设置小车x方向的速度设定值
    car_speed_set[0] = car_vx;
    // 设置小车y方向的速度设定值
    car_speed_set[1] = car_vy;
    // 设置小车的角速度设定值
    car_speed_set[2] = car_w;
    // 根据小车的速度设定值计算四个电机的速度设定值
    set_whell_speed();	
}

// 根据小车的速度设定值计算四个电机的速度设定值
void set_whell_speed(void)
{
    // 计算左前电机的速度设定值
    wheel_speed_set[0] = car_speed_set[0] - car_speed_set[1] + car_speed_set[2] * (D_HALF_X + D_HALF_Y);
    // 计算右前电机的速度设定值
    wheel_speed_set[1] = car_speed_set[0] + car_speed_set[1] + car_speed_set[2] * (D_HALF_X + D_HALF_Y);
    // 计算左后电机的速度设定值
    wheel_speed_set[2] = car_speed_set[0] - car_speed_set[1] - car_speed_set[2] * (D_HALF_X + D_HALF_Y);
    // 计算右后电机的速度设定值
    wheel_speed_set[3] = car_speed_set[0] + car_speed_set[1] - car_speed_set[2] * (D_HALF_X + D_HALF_Y);
    // 设置左前电机PID控制器的设定值
    pid_controllers[0].setpoint = wheel_speed_set[0];
    // 设置右前电机PID控制器的设定值
    pid_controllers[1].setpoint = wheel_speed_set[1];
    // 设置左后电机PID控制器的设定值
    pid_controllers[2].setpoint = wheel_speed_set[2];
    // 设置右后电机PID控制器的设定值
    pid_controllers[3].setpoint = wheel_speed_set[3];
}

// 电机处理函数，计算电机速度，使用PID控制电机速度
void motor_proc(void)
{
    // 计算电机的实际速度
    calculate_speed();

    // 使用PID控制器计算每个电机的PWM输出
    for (int i = 0; i < 4; i++) {
        // 调用PID计算函数，根据当前速度和设定速度计算PID输出
        double pid_output = pid_compute(&pid_controllers[i], wheel_speed[i]);
        // 将PID输出转换为电机的PWM控制值
        motor_pwm[i] = (int32_t)pid_output;
        // 如果PWM控制值在-50到50之间，清零积分项，防止积分饱和
        if(motor_pwm[i] < 50 && motor_pwm[i] > -50)
        {
            pid_controllers[i].integral = 0;
        }
    }

    // 根据计算得到的PWM控制值控制电机的速度和方向
    control_motor_speed();
	
//	myusart_transmit(&huart1, "%.5lf,%.5lf,%.5lf,%.5lf\n",
//                         wheel_speed[left_front]*10, 
//						wheel_speed[left_rear]*10, 
//						wheel_speed[right_rear]*10,
//						wheel_speed[right_front]*10);
}    


