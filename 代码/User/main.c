#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "TB_PWM.h"

int main()
{
	OLED_Init();
	TB_PWM_Init();
	TB_B_Rotation_Direction(MOTOR_left);
	TB_AOx_Cmd(MOTOR_RUN);
	
	while(1)
	{
		OLED_ShowString(1,1,"Hello Word!");
	}
}
