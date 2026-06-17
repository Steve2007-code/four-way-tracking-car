#include "stm32f10x.h"                  // Device header
#include "Command_set.h"
#include "Delay.h"

uint8_t Serial_RxData;		//定义串口接收的数据变量
uint8_t Serial_RxFlag;		//定义串口接收的标志位变量

uint8_t RotationA = 1, RotationB = 1;		//电机旋转反向变量
uint8_t Key;								//按键变量
uint8_t Power = 0;							//电机是否开启
uint8_t SpeedA = 50, SpeedB = 50;			//电机A速度 电机B速度

void Serial_Init(void)//串口初始化
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;//USART2 RX
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;//USART2 TX
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;//Lora
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);
	GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_RESET);
	
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;//奇偶校验位
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2,&USART_InitStruct);
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//USART中断使能
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//NVIC配置
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART2,ENABLE);
	
	
	//控制按键初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	//速度B设置
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;//速度B + | -
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//速度A设置
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;//速度A + | -
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//电机设置
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;//电机 ON | OFF
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//电机AB转向设置
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;//电机A | 电机B
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}

void Serial_SendByte(uint8_t Byte)//单字节发送
{
	USART_SendData(USART2,Byte);
	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == DISABLE);
}

void Serial_TB_Cmd(void)//按键控制
{
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == SET)//电机开
	{
		Delay_ms(10);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) != RESET);
		Serial_SendByte(C);
		Serial_SendByte(MOTOR_RUN);
		Delay_ms(10);
		Key = 1;
		Power = 1;
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13) == SET)//电机关
	{
		Delay_ms(10);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13) != RESET);
		Serial_SendByte(C);
		Serial_SendByte(MOTOR_STOP);
		Delay_ms(10);
		Key = 2;
		Power = 0;
	}
	
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14) == SET)//速度A+
	{
		Delay_ms(10);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14) != RESET);
		if(SpeedA < 100)
		{
			Serial_SendByte(D);
			SpeedA += 10;
			Serial_SendByte(SpeedA);
			Delay_ms(10);
		}
		Key = 3;
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) == SET)//速度A-
	{
		Delay_ms(10);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) != RESET);
		if(SpeedA > 0)
		{
			Serial_SendByte(D);
			SpeedA = SpeedA-10;
			Serial_SendByte(SpeedA);
			Delay_ms(10);
		}
		Key = 4;
	}
	
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8) == SET)//速度B+
	{
		Delay_ms(10);
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8) != RESET);
		if(SpeedB < 100)
		{
			Serial_SendByte(E);
			SpeedB += 10;
			Serial_SendByte(SpeedB);
			Delay_ms(10);
		}
		Key = 5;
	}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_9) == SET)//速度B-
	{
		Delay_ms(10);
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_9) != RESET);
		if(SpeedB > 0)
		{
			Serial_SendByte(E);
			SpeedB -= 10;
			Serial_SendByte(SpeedB);
			Delay_ms(10);
		}
		Key = 6;
	}
	
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == SET)//A电机旋转反向
	{
		Delay_ms(10);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) != RESET);
		Serial_SendByte(A);
		RotationA = RotationA ? 0 : 1;//电机反转逻辑
		Serial_SendByte(RotationA);
		Delay_ms(10);
		Key = 7;
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0) == SET)//B电机旋转反向
	{
		Delay_ms(10);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0) != RESET);
		Serial_SendByte(B);
		RotationB = RotationB ? 0 : 1;//电机反转逻辑
		Serial_SendByte(RotationB);
		Delay_ms(10);
		Key = 8;
	}
}

uint8_t Serial_GetRxFlag(void)//接收数据标志位查询
{
	if(Serial_RxFlag == 1)
	{
		return 1;
	}
	return 0;
}

uint8_t Serial_GetRxReturnData(void)//返回串口接收到的数据
{
	Serial_RxFlag = 0;
	return Serial_RxData;
}

void USART2_IRQHandler(void)//串口2中断接收数据
{
	if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET)
	{
		Serial_RxData = USART_ReceiveData(USART2);
		Serial_RxFlag = 1;
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	}
}
