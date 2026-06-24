#include "stm32f10x.h"                  // Device header

uint8_t CCRA;//A电机PWM占空比
uint8_t CCRB;//B电机PWM占空比

void TB_PWM_CCRA_Set(uint8_t CCR)//TIM1_CH2 CCR设置（CCR范围 0 ~ 100）
{
	TIM_SetCompare2(TIM1,CCR);
}

void TB_PWM_CCRB_Set(uint8_t CCR)//TIM1_CH1 CCR设置（CCR范围 0 ~ 100）
{
	TIM_SetCompare1(TIM1,CCR);
}

void TB_PWM_Init(void)//TB6612初始化
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;//TIM1_CH1 | TIM1_CH2（A | B）
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);//输出PWMA PWMB配置
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;//TB6612 STBY
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);//电机启停控制
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;//TB6612 AIN1 | AIN2 | BIN1 | BIN2
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);//电机选择方向
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;//TB6612 BIN1 | BIN2
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);//电机选择方向
	
	TIM_InternalClockConfig(TIM1);//选择时钟原 内部时钟
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitTypeDef;//配置TIM1时基单元 10kHz
	TIM_TimeBaseInitTypeDef.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitTypeDef.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitTypeDef.TIM_Period = 100-1;//RCC
	TIM_TimeBaseInitTypeDef.TIM_Prescaler = 72-1;//PSC
	TIM_TimeBaseInitTypeDef.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitTypeDef);
	
	TIM_OCInitTypeDef TIM_OCInitStruct;//TIM1配置输出比较
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;//空闲时低电平
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 40;//CCRB
	TIM_OC1Init(TIM1,&TIM_OCInitStruct);//CH1配置(PWMB)
	
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;//空闲时低电平
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 40;//CCRA
	TIM_OC2Init(TIM1,&TIM_OCInitStruct);//CH2配置(PWMA)
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);//使能TIM1外设的主输出
	
	/*TB6612 STBY默认为不使能(电机关闭)*/
	GPIO_WriteBit(GPIOB,GPIO_Pin_10,Bit_RESET);
	/*AO默认顺时针旋转*/
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
	GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_RESET);
	/*BO默认顺时针旋转*/
	GPIO_WriteBit(GPIOB,GPIO_Pin_0,Bit_SET);
	GPIO_WriteBit(GPIOB,GPIO_Pin_1,Bit_RESET);
	
	TIM_Cmd(TIM1,ENABLE);//使能TIM1
}

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

//A电机旋转方向设置 可以设这个枚举类型中的值&RotationState
void TB_A_Rotation_Direction(uint8_t MOTOR_RotationState)
{
	if(MOTOR_RotationState == 0)//顺时针
	{
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_RESET);
	}
	else if(MOTOR_RotationState == 1)//逆时针
	{
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_SET);
	}
	else if(MOTOR_RotationState == 2)//停止
	{
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_RESET);
	}
}

//B电机旋转方向设置 可以设这个枚举类型中的值&RotationState
void TB_B_Rotation_Direction(uint8_t MOTOR_RotationState)
{
	if(MOTOR_RotationState == 0)//顺时针
	{
		GPIO_WriteBit(GPIOB,GPIO_Pin_0,Bit_SET);
		GPIO_WriteBit(GPIOB,GPIO_Pin_1,Bit_RESET);
	}
	else if(MOTOR_RotationState == 1)//逆时针
	{
		GPIO_WriteBit(GPIOB,GPIO_Pin_0,Bit_RESET);
		GPIO_WriteBit(GPIOB,GPIO_Pin_1,Bit_SET);
	}
	else if(MOTOR_RotationState == 2)//停止
	{
		GPIO_WriteBit(GPIOB,GPIO_Pin_0,Bit_RESET);
		GPIO_WriteBit(GPIOB,GPIO_Pin_1,Bit_RESET);
	}
}

//总电机启停控制 可以设这个枚举类型中的值&MotorState
void TB_AOx_Cmd(uint16_t MOTOR_State)
{
	if(MOTOR_State == 0)// 停止
	{
		GPIO_WriteBit(GPIOB,GPIO_Pin_10,Bit_RESET);
	}
	else if(MOTOR_State == 1)// 运行
	{
		GPIO_WriteBit(GPIOB,GPIO_Pin_10,Bit_SET);
	}
}
