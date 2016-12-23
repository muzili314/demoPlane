/*brief:�ⲿ�ж�
 *
 *
 *
*/

#include "exit.h"
#include "key.h"
#include "led.h"
#include "motor.h"

void EXTI0_IRQHandler(void)
{
	
	#ifdef OS_CRITICAL_METHOD 	//���OS_CRITICAL_METHOD������,˵��ʹ��ucosII��.
		OSIntEnter();    
	#endif
	
	delay_ms(10);  //��������
	if(KEY1 == 0)	 	         //PA.0
	{				 
		LED1 = !LED1;	
		MOTOR_EN = 1;
		MOTOR_DIR = !MOTOR_DIR;
		MOTOR_EN = 0;
	}		 
	EXTI->PR |= (1<<0);  //���LINE0�ϵ��жϱ�־λ
	
	#ifdef OS_CRITICAL_METHOD 	
	  OSIntExit();  											 
  #endif
}

void EXTI15_10_IRQHandler(void)
{
	#ifdef OS_CRITICAL_METHOD 
		OSIntEnter();    
	#endif
	delay_ms(10);
	if(KEY2 == 0)
	{
		LED2 = !LED2;
		MOTOR_EN = !MOTOR_EN;
	}
	EXTI->PR |= (1<<13);
	
	#ifdef OS_CRITICAL_METHOD 
		OSIntEnter();    
	#endif
}

/*����NVIC*/
void EXTIX_Init(void)
{
	/*�����ж�*/
	MY_NVIC_Init(1,0,USART2_IRQChannel,2);//��1 ��3 ��2
//	MY_NVIC_Init(1,2,USART3_IRQChannel,2);//��1 ��3 ��2
	/*TIM*/
  MY_NVIC_Init(1,1,TIM2_IRQChannel,2);//��0 ��0 ��2
	MY_NVIC_Init(2,0,TIM3_IRQChannel,2);
	MY_NVIC_Init(2,1,TIM4_IRQChannel,2);
	MY_NVIC_Init(2,2,TIM5_IRQChannel,2);
	/*�����ж�*/
	Ex_NVIC_Config(GPIO_A,0,FTIR); 			
 	Ex_NVIC_Config(GPIO_C,13,FTIR); 		//�½��ش���
	MY_NVIC_Init(2,1,EXTI0_IRQChannel,2);	
	MY_NVIC_Init(2,0,EXTI15_10_IRQChannel,2);	
}

