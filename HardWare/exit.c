/*brief:外部中断
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
	
	#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
		OSIntEnter();    
	#endif
	
	delay_ms(10);  //按键消抖
	if(KEY1 == 0)	 	         //PA.0
	{				 
		LED1 = !LED1;	
		MOTOR_EN = 1;
		MOTOR_DIR = !MOTOR_DIR;
		MOTOR_EN = 0;
	}		 
	EXTI->PR |= (1<<0);  //清除LINE0上的中断标志位
	
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

/*配置NVIC*/
void EXTIX_Init(void)
{
	/*串口中断*/
	MY_NVIC_Init(1,0,USART2_IRQChannel,2);//抢1 从3 组2
//	MY_NVIC_Init(1,2,USART3_IRQChannel,2);//抢1 从3 组2
	/*TIM*/
  MY_NVIC_Init(1,1,TIM2_IRQChannel,2);//抢0 从0 组2
	MY_NVIC_Init(2,0,TIM3_IRQChannel,2);
	MY_NVIC_Init(2,1,TIM4_IRQChannel,2);
	MY_NVIC_Init(2,2,TIM5_IRQChannel,2);
	/*键盘中断*/
	Ex_NVIC_Config(GPIO_A,0,FTIR); 			
 	Ex_NVIC_Config(GPIO_C,13,FTIR); 		//下降沿触发
	MY_NVIC_Init(2,1,EXTI0_IRQChannel,2);	
	MY_NVIC_Init(2,0,EXTI15_10_IRQChannel,2);	
}

