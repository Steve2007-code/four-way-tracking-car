#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "Command_set.h"

void Serial_Init(void);//串口初始化
void Serial_SendByte(uint8_t Byte);//单字节发送
void Serial_TB_Cmd(void);//按键控制
uint8_t Serial_GetRxFlag(void);//接收数据标志位查询
uint8_t Serial_GetRxReturnData(void);//返回串口接收到的数据

extern uint8_t RotationA, RotationB;		//电机旋转反向变量
extern uint8_t Key;								//按键变量
extern uint8_t Power;							//电机是否开启
extern uint8_t SpeedA, SpeedB;			//电机A速度 电机B速度

#endif
