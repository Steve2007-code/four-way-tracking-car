#ifndef __Lora_H__
#define __Lora_H__

extern uint8_t LoraData[200];//串口接收数据存储变量
extern uint16_t LoraDataFlag;//电机功能引索是否收到标志位
extern uint16_t LoraFunction;//电机功能引索
extern uint16_t LoraFunctionSet;//电机功能设置

void Lora_Init(void);//Lora初始化

#endif
