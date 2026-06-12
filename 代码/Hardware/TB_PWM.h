#ifndef __TB_PWM_H__
#define __TB_PWM_H__

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

/*
--------------------------------使用说明---------------------------------
	使用 TB_PWM_Init 进行初始化（初始化默认STBY为低电平，AO,BO默认顺时针转）
	之后使用 TB_Rotation_Direction 设置旋转方向之后使用 TB_AOx_Cmd 开启电机
--------------------------------------------------------------------------
*/

void TB_PWM_Init(void);//TB6612初始化
void TB_A_Rotation_Direction(uint8_t MOTOR_RotationState);//A电机旋转方向设置
void TB_B_Rotation_Direction(uint8_t MOTOR_RotationState);//B电机旋转方向设置
void TB_AOx_Cmd(uint16_t MOTOR_State);//电机启停控制

#endif
