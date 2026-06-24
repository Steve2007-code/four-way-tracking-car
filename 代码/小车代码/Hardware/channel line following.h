#ifndef __channel_line_following_H__
#define __channel_line_following_H__

extern uint8_t Channel_12,Channel_13,Channel_14,Channel_15;  //4路循迹判断变量

void Channel_Line_Following_Speed(void);////4路循迹调设置状态机

void Channel_Line_Following_Init(void);	//4路循迹模块初始化
void Motor_State_F(void);//4路循迹调设置电机函数

#endif
