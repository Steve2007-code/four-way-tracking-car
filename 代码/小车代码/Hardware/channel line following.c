#include "stm32f10x.h"                  // Device header
#include "TB_PWM.h"

//4路B12 B13 B14 B15

void Channel_Line_Following_Init(void)		//4路循迹模块初始化
{
	//GPIOB开启设置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	//4路循迹GPIOB初始化
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//四路GPIO EXTI中断初始化
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource12);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource13);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource14);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource15);
	
	//NVIC中断分组初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//EXTI中断初始化
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line12 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
	EXTI_Init(&EXTI_InitStruct);
}

/*-----------------------电机转向控制函数-----------------------------
电机速度控制 函数为  TB_PWM_CCRA_Set(uint8_t CCR)  //TIM2 CCR设置（CCR范围 0 ~ 100）
电机速度控制 函数为TB_PWM_CCRB_Set(uint8_t CCR)    //TIM1 CCR设置（CCR范围 0 ~ 100）
---------------------------------------------------------------------*/
//4路循迹判断变量
uint8_t Channel_12,Channel_13,Channel_14,Channel_15;
//4路循迹 黑为1 白为0
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line12) == SET)
	{
		Channel_12 = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12);
		EXTI_ClearITPendingBit(EXTI_Line12);
	}
	if(EXTI_GetITStatus(EXTI_Line12) == SET)
	{
		Channel_13 = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12);
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
	if(EXTI_GetITStatus(EXTI_Line13) == SET)
	{
		Channel_14 = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12);
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
	if(EXTI_GetITStatus(EXTI_Line14) == SET)
	{
		Channel_15 = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12);
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}

//CCRA CCRB;//A B电机PWM占空比
uint8_t OldCCRA,OldCCRB;//A B 电机原占空比
uint8_t Motor_State = 0;//状态机调整寻路

//4路循迹调设置状态机
void Channel_Line_Following_Speed(void)
{
	if(Motor_State == 0)
	{
		OldCCRA = CCRA;
		OldCCRB = CCRB;
	}
	if(Channel_13 == 1 && Channel_14 == 1)//直行
	{
		Motor_State = 1;
	}
	if(Channel_13 == 1 && Channel_14 == 0)//右转
	{
		Motor_State = 2;
	}
	if(Channel_13 == 0 && Channel_14 == 1)//左转
	{
		Motor_State = 3;
	}
	else
		Motor_State = 0;
}

//4路循迹调设置电机函数
void Motor_State_F(void)
{
	Channel_Line_Following_Speed();//不断查询灰度传感器状态
	switch(Motor_State)
	{
		case 1://直行
			TB_PWM_CCRA_Set(OldCCRA);
			TB_PWM_CCRB_Set(OldCCRB);
			break;
		case 2://右转
			TB_PWM_CCRA_Set(CCRA);
			TB_PWM_CCRB_Set(CCRB + 10);
			break;
		case 3://左转
			TB_PWM_CCRA_Set(CCRA + 10);
			TB_PWM_CCRB_Set(CCRB);
			break;
		default:
			break;
	}
}
