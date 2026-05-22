#include "wheel.h"
#include "stm32f1xx_hal.h"


extern TIM_HandleTypeDef htim4;
extern int8_t wheel_num;


//更新滚轮数据
void Mouse_wheel_Updata(void)
{
	if((int16_t)__HAL_TIM_GET_COUNTER(&htim4) > 0)// 返回16位数据，如果需要负值要强制数据类型转换
		wheel_num = 0xFF;   
	else if((int16_t)__HAL_TIM_GET_COUNTER(&htim4) < 0)
		wheel_num = 0x01;
	else
		wheel_num = 0x80;
	
	//清除编码器计数
	TIM4->CNT=0;  // x表示第几个定时器，例如TIM8->CNT=0;
}

