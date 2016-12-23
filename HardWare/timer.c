/*brief:TIM2 CH2 PA1 CH3 PA.2
 *param :arr TIM�Զ���װֵ
 *param :psc TIMԤ��Ƶֵ
 *TIM2 partial remap 
 *CH2 PA.1 CH3 PB.10 CH4 PB11
*/

#include "timer.h"
#include "led.h"
#include "includes.h"

u16 TIM2_ARR;
u16 TIM1_ARR;

void TIM2_PWM_Init(u16 arr,u16 psc)
{
	/*����ʱ��*/
	RCC->APB1ENR |= 1<<0;  //TIM2
	RCC->APB2ENR |= 1<<2;  //PORTA
	RCC->APB2ENR |= 1<<3;  //PORTB
	RCC->APB2ENR |= 1<<0;  //AFIO
	/*����IO*/
//	GPIOA->CRL &= 0XFFFFF00F;
//	GPIOA->CRL |= 0X00000BB0; //PA.1 2���ù����������
	
	/*TIM2�˿ڸ���*/
	AFIO->MAPR &= ~(1<<8);
	AFIO->MAPR |= 1<<9;
	GPIOA->CRL &= 0XFFFFFF0F;
	GPIOA->CRL |= 0X000000B0;  //PA.1
	
	GPIOB->CRH &= 0XFFFFF0FF;
	GPIOB->CRH |= 0X00000B00;  //PB10
	
	/*TIMװ��ֵARR��Ԥ��ƵֵPSC*/
	TIM2->ARR = arr;
	TIM2_ARR = arr;
	TIM2->PSC = psc;
	/*����TIMΪPWMģʽ*/
	TIM2->CCMR1 |= 7<<12;   // 110/111
	/*CCRװ��ֵ*/
	TIM2->CCR2 = arr/2;
	/*CCRԤװ��ʹ��*/
	TIM2->CCMR1 |= 1<<11;   //CH2
  /*�����ж�*/
//	TIM2->DIER |= 1<<0; //����TIM�����ж�
	/*�������ȼ�*/

	/*TIM�Զ���װ��ʹ��*/
	TIM2->CR1 |= 1<<7;
	
	/*����CCR3 PWM ���*/
	TIM2->CCMR2 |= 7<<4;
	TIM2->CCR3 = arr/2;
	TIM2->CCMR2 |= 1<<3;
//	TIM2->DIER |= 1<<3;
//	TIM2->CCER |= 1<<8;  //CCR3
	/*ʹ��PWM�����TIM*/
	TIM2->CCER |= 1<<4;  //CCR2
	TIM2->CR1 |= 0x01;
	//TIM ARR buffer enabled
//	TIM2->CR1 |= 1<<7;
		
}

/*TIM1 Ini
 *full remap CH1-CH4 PE9-PE14
 *
 *
*/
void TIM1_Init(u16 arr,u16 psc)
{
	//clock enabled
	RCC->APB2ENR |= 1<<11;
	RCC->APB2ENR |= 1<<6;
	RCC->APB2ENR |= 1<<0;
	//remap
	AFIO->MAPR |= 3<<6;
	//IO PE.9 CH1
	GPIOE->CRH &= 0XFFFFF00F;
	GPIOE->CRH |= 0X00000BB0;
	
//	GPIOA->CRH &= 0XFFFFFFF0;
//	GPIOA->CRH |= 0X0000000B;
	
	TIM1->BDTR = 0XC0;
	//arr,psc and so on configuration
	TIM1->ARR = arr;
	TIM1->PSC = psc;
	//pwm
	TIM1->CCMR1 |= 6<<4;  //CCR1 PWM model
	TIM1->CCMR1 |= 1<<3;  //CCR1 preload enabled
	TIM1->CCR1 = arr/2;
	TIM1->CCER |= 1<<0;
	
	TIM1->CCMR1 |= 6<<12;  //CCR1 PWM model
	TIM1->CCMR1 |= 1<<11;  //CCR1 preload enabled
	TIM1->CCR2 = arr/2;
//	TIM1->CCER |= 1<<2;
//	TIM1->CCER |= 1<<4;
	//interrupt
	
	//enable
 TIM1->CR1 |= 1<<7;  //APEN
 TIM1->CR1 |= 1<<0;     
 TIM1->BDTR|= 1<<15;  

}

/*Interrupt Function
 *
 *
 *
*/   
void TIM2_IRQHandler(void)
{
//	OSIntEnter();
	if((TIM2->SR & (1<<0)))//���������ж�
	{
		LED1 = !LED1;
		TIM2->SR &= ~(1<<0);
	}
	else if(TIM2->SR & (1<<2))  //CCR2�ж�
	{
		LED2 = !LED2;
		TIM2->SR &= ~(1<<2);
	}

//   OSIntExit();
}
