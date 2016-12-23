/*brief: TX PA.2 RX:PA.3
 *
 *
 *note:TX RX分别同TIM2 CH3 CH4 共用IO口
*/

#include "uart2.h"
#include "msg.h"
#include "cmd_queue.h"

void uart2_Init(u32 pclk1,u32 bound)
{
	float temp;
	u16 mantissa,fraction;
	temp=(float)(pclk1*1000000)/(bound*16);//得到 USARTDIV
	mantissa=temp; //整数部分
	fraction=(temp-mantissa)*16; //小数部分
	mantissa<<=4;
	mantissa+=fraction;
	
	/*开启时钟*/
	RCC->APB1ENR |= (1<<17);//UART2
	RCC->APB2ENR |= (1<<0);//AFIO
//	RCC->APB2ENR |= (1<<2);//PA
	RCC->APB2ENR |= 1<<5; //PD
	//remap
	AFIO->MAPR |= 1<<3;
	/*配置IO*/
	GPIOD->CRL &= 0XF00FFFFF;
	GPIOD->CRL |= 0X08B00000;
//	GPIOA->CRL &= 0XFFFF00FF;
//	GPIOA->CRL |= 0X00008B00;  //A.3 上拉输入 A.2复用输出 
	/*设置串口模式*/
	/*复位USART2*/
	RCC->APB1RSTR |= (1<<17);
	RCC->APB1RSTR &= ~(1<<17);
	/*设置波特率 停止位 校验位*/
	USART2->BRR = mantissa;
	USART2->CR1|=0X200C;  //1位停止,无校验位.UART Transmiter receiver enabled
	/*使能中断 设定优先级*/
	USART2->CR1 |= (1<<5);//接收缓冲区非空中断使能

}


void USART2_IRQHandler()
{
	u8 uart2Res;
	#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
		OSIntEnter();    
	#endif
	
	if(USART2->SR & (1<<5))//接收到数据
	{
		uart2Res=USART2->DR; 
    queue_push(uart2Res);
		OSSemPost(uartRx_Sem);	
	}
	
	#ifdef OS_CRITICAL_METHOD 	
	  OSIntExit();  											 
  #endif
}
static void Uart2_Send(u16 ch)
{
	static u32 i = 0;
	USART2->DR = (ch&(u16)0x01FF);	
	//while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
  while( ((USART2->SR&0X40)==0) && (i<120000) )
	{
		i++;
	}//循环发送,直到发送完毕  
}

/*brief:发送一个字符串
 *
 *
 *bug:发送的第二个字符会丢失
*/
void uart2SendStr(char *s)
{
	char *p = s;
	while(*p != '\0')
	{
		Uart2_Send(*p);
		p++;
	}
}
/*brief:发送一个字符
 *
 * 
 *
*/
void Uart2SendChar(u8 ch)
{
	USART2->DR = ch;
	while(((USART2->SR & (1<<6)) == 0));//等待发送完成
}


