#ifndef __COMMADD_SET_H__
#define __COMMADD_SET_H__

/*
--------------------------------功能说明---------------------------------
	第一次发送 LoraFunction 功能索引之后，再根据第二个发送的数据LoraFunctionSet 设置相应的功能
--------------------------------------------------------------------------
*/

//电机状态枚举 电机功能设置
typedef enum {
    MOTOR_STOP = 0,    // 停止 B13
    MOTOR_RUN = 1	   // 运行 B12
} LoraFunctionSet_MotorState;

//选择方向枚举 电机功能设置
typedef enum{
	MOTOR_left = 0,	   //顺时针 NULL
	MOTOR_right = 1,   //逆时针 NULL
	MOTOR_stop = 2	   //停止 NULL
} LoraFunctionSet_RotationState;

//速度设置枚举 电机功能设置
typedef enum{
	MOTOR_SpeedAdd = 0,	   //速度加
	MOTOR_SpeedLose = 1,   //速度减
} LoraFunctionSet_State;

//选择方向枚举 电机功能引索
typedef enum{
	A = 01,		//A电机选择方向设置
	B = 02,		//B电机旋转方向设置
	C = 03,		//总电机启停控制
	D = 04,		//A电机选择速度
	E = 05		//B电机选择速度
} LoraFunction;
				

#endif
