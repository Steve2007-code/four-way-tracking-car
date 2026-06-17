#include "stm32f10x.h"                  // Device header
#include "Serial.h"
#include "OLED.h"
#include "Delay.h"

int main()
{
	OLED_Init();
	Serial_Init();
	
	while(1)
	{
		Serial_TB_Cmd();
		OLED_ShowNum(1,1,Key,2);
		if(Power == 1)
		{
			OLED_ShowString(1,4,"ON ");
		}
		else
		{
			OLED_ShowString(1,4,"OFF");
		}
		OLED_ShowNum(2,1,RotationA,2);
		OLED_ShowNum(3,1,RotationB,2);
		OLED_ShowNum(2,4,SpeedA,3);
		OLED_ShowNum(3,4,SpeedB,3);
	}
}
