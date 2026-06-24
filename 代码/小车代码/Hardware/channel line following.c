#include "stm32f10x.h"                  // Device header
#include "TB_PWM.h"
#include "Delay.h"

//4路B12 B13 B14 B15

void Channel_Line_Following_Init(void)		//4路循迹模块初始化
{
	//GPIOB AFIO开启设置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
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
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//双沿触发
	EXTI_Init(&EXTI_InitStruct);
}

/*-----------------------电机转向控制函数-----------------------------
电机速度控制 函数为  TB_PWM_CCRA_Set(uint8_t CCR)  //TIM2 CCR设置（CCR范围 0 ~ 100）
电机速度控制 函数为TB_PWM_CCRB_Set(uint8_t CCR)    //TIM1 CCR设置（CCR范围 0 ~ 100）
---------------------------------------------------------------------*/
//4路循迹判断变量
uint8_t Channel_12,Channel_13,Channel_14,Channel_15;

//CCRA CCRB;//A B电机PWM占空比
uint8_t OldCCRA,OldCCRB;//A B 电机原占空比
uint8_t Motor_State = 0;//状态机调整寻路
uint8_t CornerPassed = 0;//转角是否已过弯

//4路循迹调设置状态机
void Channel_Line_Following_Speed(void)
{
	//读取4路
	Channel_12 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
	Channel_13 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);
	Channel_14 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);
	Channel_15 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);
	
	//保存基准速度
	if(Motor_State == 0)
	{
		CCRA = TIM_GetCapture1(TIM1);
		CCRB = TIM_GetCapture2(TIM1);
		OldCCRA = CCRA;
		OldCCRB = CCRB;
		Motor_State = 1;  //默认直行
	}
	
	if(Motor_State == 4)       //正在急右转中
	{
		if(CornerPassed == 1 && (Channel_13 == 1 || Channel_14 == 1))
			Motor_State = 1;   //P12已离开 且 中间压线 → 退出转弯
		if(Channel_12 == 0)
			CornerPassed = 1;  //P12离开线了，标记已过弯
	}
	else if(Motor_State == 5)  //正在急左转中
	{
		if(CornerPassed == 1 && (Channel_13 == 1 || Channel_14 == 1))
			Motor_State = 1;
		if(Channel_15 == 0)
			CornerPassed = 1;
	}
	else                        //直行/微调中，检查触发
	{
		if(Channel_12 == 1)
		{
			Motor_State = 4;
			Delay_ms(50);
			CornerPassed = 0;  //进入转弯，重置标志
		}
		else if(Channel_15 == 1)
		{
			Motor_State = 5;
			Delay_ms(50);
			CornerPassed = 0;
		}
		else if(Channel_13 == 1 && Channel_14 == 0)
			Motor_State = 2;
		else if(Channel_13 == 0 && Channel_14 == 1)
			Motor_State = 3;
		else
			Motor_State = 1;
	}
}

//4路循迹调设置电机函数
void Motor_State_F(void)
{
	Channel_Line_Following_Speed();
	switch(Motor_State)
	{
		case 1:  //直行
			TB_PWM_CCRA_Set(OldCCRA);
			TB_PWM_CCRB_Set(OldCCRB);
			break;
		case 2:  //微右修
			TB_PWM_CCRA_Set(OldCCRA - 6);
			TB_PWM_CCRB_Set(OldCCRB + 6);
			break;
		case 3:  //微左修
			TB_PWM_CCRA_Set(OldCCRA + 6);
			TB_PWM_CCRB_Set(OldCCRB - 6);
			break;
		case 4:  //急右转
			TB_PWM_CCRA_Set(0);
			TB_PWM_CCRB_Set(14);
			break;
		case 5:  //急左转
			TB_PWM_CCRA_Set(14);
			TB_PWM_CCRB_Set(0);
			break;
		default:
			break;
	}
}
