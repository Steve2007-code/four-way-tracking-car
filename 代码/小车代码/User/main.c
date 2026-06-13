#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "TB_PWM.h"
#include "Lora.h"

int main()
{
	OLED_Init();
	//TB_PWM_Init();
	//TB_B_Rotation_Direction(MOTOR_left);
	//TB_AOx_Cmd(MOTOR_RUN);
	Lora_Init();
	
	while(1)
	{
		OLED_ShowString(1,1,"Hello Word!");
		OLED_ShowHexNum(2,1,LoraData[0],2);
		OLED_ShowHexNum(2,4,LoraData[1],2);
		OLED_ShowHexNum(2,7,LoraData[2],2);
	}
}
