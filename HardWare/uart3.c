/*uart3 TX PB.10  RX PB.11
 *partial remap TX PC.10 RX PC.11
 *
 *
 *
*/
#include "uart3.h"

void uart3_Init(u32 pclk1,u32 bound)
{
	float temp;
	u16 mantissa,fraction;
	temp=(float)(pclk1*1000000)/(bound*16);//得到 USARTDIV
	mantissa=temp; //整数部分
	fraction=(temp-mantissa)*16; //小数部分
	mantissa<<=4;
	mantissa+=fraction;
	
	/*开启时钟*/
	RCC->APB1ENR |= (1<<18);//UART3
	RCC->APB2ENR |= (1<<0);//AFIO
	RCC->APB2ENR |= (1<<4);//PC
//	RCC->APB2ENR |= (1<<3);//PB
	/*配置IO*/
	/*partial remap*/
	AFIO->MAPR |= 1<<4;
	GPIOC->CRH &= 0XFFFF00FF;
	GPIOC->CRH |= 0X00008B00;
//	GPIOB->CRH &= 0XFFFF00FF;
//	GPIOB->CRH |= 0X00008B00;  //PB.11 上拉输入 PB.10复用输出 
	/*设置串口模式*/
	/*复位USART2*/
	RCC->APB1RSTR |= (1<<18);
	RCC->APB1RSTR &= ~(1<<18);
	/*设置波特率 停止位 校验位*/
	USART3->BRR = mantissa;
	USART3->CR1|=0X200C;  //1位停止,无校验位.UART Transmiter receiver enabled
	/*使能中断 设定优先级*/
	USART3->CR1 |= (1<<5);//接收缓冲区非空中断使能

}

/*
*
*
*/
void USART3_IRQHandler()
{
	u8 uart3Res;
	#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
		OSIntEnter();    
	#endif
	
	if(USART3->SR & (1<<5))//接收到数据
	{
		uart3Res = USART3->DR; 

	}
	
	#ifdef OS_CRITICAL_METHOD 	
	  OSIntExit();  											 
  #endif
}

/*
 *
 *
 *
 * 
*/
static void Uart3_Send(u8 ch)
{
	USART3->DR = (ch&(u8)0x01FF);
	while((USART3->SR & (1<<6)) == 0);
}



/*brief:发送一个字符串
 *
 *
*bug:发送的第二个字符会丢失
*/
void uart3SendStr(char *s)
{
	char *p = s;
	while(*p != '\0')
	{
		Uart3_Send(*p);
		p++;
	}
}
