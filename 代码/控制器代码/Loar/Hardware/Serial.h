#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdio.h>

void Serial_Init();
void Serial_SendByte(uint8_t Byte);//发送单个字节
void Serial_SendArray(uint8_t *Array, uint16_t Length);//发送数组
void Serial_SendString(char *String);//发送字符串
void Serial_SendNumber(uint32_t Number, uint8_t Length);//发送数字

uint8_t Serial_GetRxReturnData(void);//获取串口接收到的数据
uint8_t Serial_GetRxFlag(void);//串口接收数据标志位

#endif
