#ifndef  _H_MOTOR_
#define  _H_MOTOR_

#include "sys.h"

#define LENGTH 490      //导轨长度 - 滑块长度 mm
#define SAMPLES 100
#define FRENCE 2000000
#define MOTORTIME 10

//电机方向和使能
#define MOTOR_DIR  PAout(4)
#define MOTOR_EN   PAout(5)
#define MOTOR1_DIR PAout(6)
#define MOTOR1_EN  PAout(7)

//检测来自电涡的信号以确定导轨是否到达初始位置
#define EDDY0 PCin(10)
#define EDDY1 PCin(11)


//控制气泵开关
#define AIR_PUMP PBout(11)
//控制继电器的输出信号
//高电平触发
#define RELAY_1	PBout(12)
#define RELAY_2	PBout(13)
#define RELAY_3	PBout(14)
#define RELAY_4	PBout(15)

extern u16 load[SAMPLES-1];
extern void MOTOR_Init(void);

#endif
