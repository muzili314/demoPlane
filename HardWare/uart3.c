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
	temp=(float)(pclk1*1000000)/(bound*16);//�õ� USARTDIV
	mantissa=temp; //��������
	fraction=(temp-mantissa)*16; //С������
	mantissa<<=4;
	mantissa+=fraction;
	
	/*����ʱ��*/
	RCC->APB1ENR |= (1<<18);//UART3
	RCC->APB2ENR |= (1<<0);//AFIO
	RCC->APB2ENR |= (1<<4);//PC
//	RCC->APB2ENR |= (1<<3);//PB
	/*����IO*/
	/*partial remap*/
	AFIO->MAPR |= 1<<4;
	GPIOC->CRH &= 0XFFFF00FF;
	GPIOC->CRH |= 0X00008B00;
//	GPIOB->CRH &= 0XFFFF00FF;
//	GPIOB->CRH |= 0X00008B00;  //PB.11 �������� PB.10������� 
	/*���ô���ģʽ*/
	/*��λUSART2*/
	RCC->APB1RSTR |= (1<<18);
	RCC->APB1RSTR &= ~(1<<18);
	/*���ò����� ֹͣλ У��λ*/
	USART3->BRR = mantissa;
	USART3->CR1|=0X200C;  //1λֹͣ,��У��λ.UART Transmiter receiver enabled
	/*ʹ���ж� �趨���ȼ�*/
	USART3->CR1 |= (1<<5);//���ջ������ǿ��ж�ʹ��

}

/*
*
*
*/
void USART3_IRQHandler()
{
	u8 uart3Res;
	#ifdef OS_CRITICAL_METHOD 	//���OS_CRITICAL_METHOD������,˵��ʹ��ucosII��.
		OSIntEnter();    
	#endif
	
	if(USART3->SR & (1<<5))//���յ�����
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



/*brief:����һ���ַ���
 *
 *
*bug:���͵ĵڶ����ַ��ᶪʧ
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
