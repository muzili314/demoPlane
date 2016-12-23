#include "led.h"

/*brief : 
 *param :
 *return:
 *
*/
//LED IO初始化
void LED_Init(void)
{
	RCC->APB2ENR |= 1<<3;    //使能PORTB时钟	   	 
	RCC->APB2ENR |= 1<<7;    //使能PORTF时钟	
	/*PB.0*/   	 
	GPIOB->CRL &= 0XFFFFFFF0; 
	GPIOB->CRL |= 0X00000003;       //PB.0 推挽输出   	 
  GPIOB->ODR |= 0x00000001;       //PB.0 输出高
	/*PF.7*/										  
	GPIOF->CRL &= 0X0FFFFFFF;
	GPIOF->CRL |= 0X30000000;       //PF.7推挽输出
	GPIOF->ODR |= 1<<7;             //PF.7输出高 
	/*PF.8*/										  
	GPIOF->CRH &= 0XFFFFFFF0;
	GPIOF->CRH |= 0X00000003;       //PF.8推挽输出
	GPIOF->ODR |= 1<<8;             //PF.8输出高 
} 






