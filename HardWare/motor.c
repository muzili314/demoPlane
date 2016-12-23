/*brief:
 *
 *MOTOR_EN = 1; ʧ�ܵ��  PA.5
 *MOTOR_DIR = 1; ����Ϊ��  PA.4
 *MOTOR1_EN  PA.7
 *MOTOR1_DIR  PA.6
*/
// PA.4 PA.5
#include "motor.h"
#include <math.h>

#define PI 3.1415926

u16 load[SAMPLES-1];

void MOTOR_Init(void)
{
	float motorx[SAMPLES+1];
	u16 frence[SAMPLES+1];
	u8 i = 0;
	float sum = 0;
	u16 coffe;
	//����ʱ��
	RCC->APB2ENR |= 1<<2;     //ʹ��PORTAʱ��
	RCC->APB2ENR |= 1<<3;     //ʹ��PORTBʱ��
	RCC->APB2ENR |= 1<<4;			//ʹ��PORTCʱ��
//	RCC->APB2ENR |= 1<<6;			//ʹ��PORTEʱ��
	
	GPIOA->CRL &= 0X0000FFFF;
	GPIOA->CRL |= 0X33330000;   //PA.4 PA.5 PA.6 PA.7�������
//��ʼ������IO
	GPIOC->CRH &= 0XFFFF00FF;
	GPIOC->CRH |= 0X00008800;
	
	//��ʼ��Relay IO and air pump IO
	//PB12-15 output push-pull
	GPIOB->CRH &= 0X00000FFF;
	GPIOB->CRH |= 0X33333000;

	/*Ĭ��disable  ����Ϊ��*/
	MOTOR_EN = 1;
	MOTOR_DIR = 1;
	
	MOTOR1_EN = 1;
	MOTOR1_DIR = 1;
	
	AIR_PUMP = 0;
	RELAY_1 = 0;
	RELAY_2 = 0;
	RELAY_3 = 0;
	RELAY_4 = 0;
	//�ٶ�����
	//x[i]
	for(i = 0;i < SAMPLES+1;i++)
	{
		motorx[i] = PI/SAMPLES*i;
	}
	
	for(i = 0;i < SAMPLES+1;i++)
	{
		sum = sum + sinf(motorx[i]);
	}
	//f=k*sin(t)  �ó�ϵ��k
	//С��������ʽ
//	coffe = (u16)LENGTH/0.02225*SAMPLES/MOTORTIME/sum;
	//����������ʽ
	coffe = (u16)LENGTH*1000/2.816*SAMPLES/MOTORTIME/sum;
	
	for(i = 0;i < SAMPLES+1;i++)
	{
		frence[i] = coffe*sinf(motorx[i]);
	}
	
	for(i = 0;i < SAMPLES-1;i++)
	{
		load[i] = FRENCE/frence[i+1];
	}
	

}
