#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

uint8_t Serial_RxData;		//定义串口接收的数据变量
uint8_t Serial_RxFlag;		//定义串口接收的标志位变量

void Serial_Init()
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
}

void Serial_SendByte(uint8_t Byte)//单字节发送
{
	USART_SendData(USART2,Byte);
	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == DISABLE);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)//数组发送
{
	uint16_t i;
	for(i = 0; i <Length; i++)
	{
		Serial_SendByte(Array[i]);
	}
	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == DISABLE);
}

void Serial_SendString(char *String)//字符发送
{
	uint16_t i;
	for(i = 0; String[i] != '\0'; i++)
	{
		Serial_SendByte(String[i]);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == DISABLE);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)//次方函数
{
	uint32_t result = 1;
	while(Y--)
	{
		result *= X ;
	}
	return result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)//发送数字
{
	uint16_t i = 0;
	for(i = 0; i < Length; i++)
	{
		Serial_SendByte((Number / Serial_Pow(10,Length - i - 1)) % 10 + '0');
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == DISABLE);
	}
}

void Serial_Printf(char *format, ...)
{
	char String[100];				//定义字符数组
	va_list arg;					//定义可变参数列表数据类型的变量arg
	va_start(arg, format);			//从format开始，接收参数列表到arg变量
	vsprintf(String, format, arg);	//使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);					//结束变量arg
	Serial_SendString(String);		//串口发送字符数组（字符串）
}

uint8_t Serial_GetRxFlag(void)
{
	if(Serial_RxFlag == 1)
	{
		return 1;
	}
	return 0;
}

uint8_t Serial_GetRxReturnData(void)
{
	Serial_RxFlag = 0;
	return Serial_RxData;
}

void USART2_IRQHandler(void)//中断接收数据
{
	if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET)
	{
		Serial_RxData = USART_ReceiveData(USART2);
		Serial_RxFlag = 1;
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	}
}
