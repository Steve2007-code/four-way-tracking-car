#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "Command_set.h"

extern uint8_t Key;

void Serial_Init(void);//串口初始化
void Serial_SendByte(uint8_t Byte);//单字节发送
void Serial_TB_Cmd(void);//按键控制
uint8_t Serial_GetRxFlag(void);//接收数据标志位查询
uint8_t Serial_GetRxReturnData(void);//返回串口接收到的数据

#endif
