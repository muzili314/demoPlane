/*
 *
 *
 *
*/
#include "key.h"
#include "delay.h"

//按键初始化函数
/*key2 PA.0 key3 PC.13*/
void KEY_Init(void)
{
	RCC->APB2ENR|=1<<2;     //使能PORTA时钟
	RCC->APB2ENR|=1<<4;     //使能PORTC时钟

	GPIOA->CRL&=0XFFFFFFF0;	//PA0设置成输入，上拉	  
	GPIOA->CRL|=0X00000008; 
	GPIOA->ODR|=0X00000001;

	GPIOC->CRH&=0XFF0FFFFF;	//PC13设置成输入，上拉	  
	GPIOC->CRH|=0X00800000; 
	GPIOC->ODR|=0X00002000;
	  
 }
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY1按下
//2，KEY2按下
//3，KEY3按下 
//4，KEY4按下
//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up = 1;  //按松开标志 松开位1 有键按下为0
	if(mode)  key_up = 1;  //支持连按	
		  
	if(key_up&&(KEY1 == 0||KEY2 == 0))
	{
		delay_ms(10);//去抖动 
		key_up = 0;
		if(KEY1 == 0)      return 1;
		else if(KEY2 == 0) return 2;
	
	}
	else if(KEY1 == 1 && KEY2 == 1)	  //没有键按下
         key_up = 1; 	    
  return 0;       // 无按键按下
    
}
