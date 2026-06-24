#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "TB_PWM.h"
#include "Lora.h"
#include "Channel Line Following.h"

int main()
{
	OLED_Init();//OLED初始化
	/*TB6612初始化*/
	TB_PWM_Init();
	TB_B_Rotation_Direction(MOTOR_left);//B电机默认向左旋转（正向）
	TB_A_Rotation_Direction(MOTOR_left);//A电机默认向左旋转（正向）
	TB_AOx_Cmd(MOTOR_STOP);//电机默认关闭
	/*------------*/
	Channel_Line_Following_Init();//4路循迹模块初始化
	Lora_Init();//Lora初始化
	
	while(1)
	{
		OLED_ShowString(1,1,"Hello Word!");
		Motor_State_F();
		OLED_ShowNum(2,1,LoraFunction,2);
		OLED_ShowNum(2,4,LoraFunctionSet,2);
	}
}
