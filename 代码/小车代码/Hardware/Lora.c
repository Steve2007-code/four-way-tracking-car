#include "stm32f10x.h"                  // Device header
#include "TB_PWM.h"

uint8_t LoraData[10];//串口接收数据存储变量
uint16_t LoraDataFlag = 0;//电机功能引索是否收到标志位
uint16_t LoraFunction = 0;//电机功能引索
uint16_t LoraFunctionSet = 0;//电机功能设置

void Lora_Init(void)//Lora初始化
{
	//时钟开启
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	//串口GPIO配置
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);//TX配置 PA2
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);//RX配置 PA3
	
	//串口配置
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2,&USART_InitStruct);
	
	//开启RXNE接收中断
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	
	//NVIC优先级配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//使能USART2中断通道
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	//串口使能
	USART_Cmd(USART2,ENABLE);
}

/*
--------------------------------功能说明---------------------------------
	第一次接收到 LoraFunction 之后，判断当前功能是什么，之后进入相应
	的判断之中，再根据第二个接收到的数据 LoraFunctionSet 设置相应的功能
--------------------------------------------------------------------------

//电机状态枚举
typedef enum {
    MOTOR_STOP = 0,    // 停止
    MOTOR_RUN = 1	   // 运行
} MotorState;

//选择方向枚举
typedef enum{
	MOTOR_left = 0,	   //顺时针
	MOTOR_right = 1,   //逆时针
	MOTOR_stop = 2	   //停止
} RotationState;

*/
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET)
	{
		if(LoraDataFlag ==0)//收到第一个功能引索
		LoraFunction = USART_ReceiveData(USART2);
		LoraDataFlag = 1;
		if(LoraDataFlag == 1)
		{
			LoraFunctionSet = USART_ReceiveData(USART2);//收到第一个功能设置
			switch(LoraFunction)
			{
				case 01://A电机选择方向设置
					TB_A_Rotation_Direction(LoraFunctionSet);
					break;
				case 02://B电机旋转方向设置
					TB_B_Rotation_Direction(LoraFunctionSet);
					break;
				case 03://总电机启停控制
					TB_AOx_Cmd(LoraFunctionSet);
					break;
				case 04://A电机选择速度
					TB_PWM_CCRA_Set(LoraFunctionSet);
					break;
				case 05://B电机选择速度
					TB_PWM_CCRB_Set(LoraFunctionSet);
					break;
				default://不是合法的功能引索关闭电机
					TB_AOx_Cmd(MOTOR_stop);
					break;
			}
		}
	}
}
