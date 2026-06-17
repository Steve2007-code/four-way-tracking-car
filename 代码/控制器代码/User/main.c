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
	}
}
