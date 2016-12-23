/*用TIM3 和TIM4 驱动6路舵机
 *TIM3 CH1: PC.6 CH2: PC.7  CH3: PC.8  CH4: PC.9 (full remap)
 *TIM4 CH1: PB.6 CH2: PB.7  CH3: PB.8  CH4: PB.9
 *
*/
#ifndef _STEER_H_
#define _STEER_H_

#include "sys.h"
#include "delay.h"
#include "includes.h"

//采样次数
#define SAMPLETIMES 100
#define TIME 2000


extern u8 isPwm0Change;
extern u8 isPwm1Change;
extern u8 isPwm2Change;
extern u8 isPwm3Change;
extern u8 isPwm4Change;

extern u8 isPwm20Change;
extern u8 isPwm21Change;
extern u8 isPwm22Change;
extern u8 isPwm23Change;
extern u8 isPwm24Change;

extern float axis_x,axis_y,axis_z;
extern float axis2_x,axis2_y,axis2_z;
extern u16 pwm0_now,pwm1_now,pwm2_now,pwm3_now,pwm4_now,pwm20_now,pwm21_now,pwm22_now,pwm23_now,pwm24_now;
extern u16 pwm0_dest,pwm1_dest,pwm2_dest,pwm3_dest,pwm4_dest,pwm20_dest,pwm21_dest,pwm22_dest,pwm23_dest,pwm24_dest;

void steer_Init(u16 arr,u16 psc,u16 pwm);
void arryDist(void);
void arryDist_2(void);
float yfunc(float x);
	
#endif
