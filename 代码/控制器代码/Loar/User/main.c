#include "stm32f10x.h"                  // Device header
#include "Serial.h"
#include "OLED.h"
#include "Delay.h"

int main()
{
	OLED_Init();
	Serial_Init();
	
	uint8_t Temp[] = {0x00,0x00};
	
	while(1)
	{
		Delay_s(1);
		Serial_SendByte(Temp[0]);
		if(Serial_GetRxFlag() == 1)
		{
			Temp[1] = Serial_GetRxReturnData();
		}
		OLED_ShowString(1,1,"TX:");
		OLED_ShowNum(2,1,Temp[0],2);
		OLED_ShowString(3,1,"RX:");
		OLED_ShowNum(4,1,Temp[1],2);
		Temp[0]++;
	}
}
