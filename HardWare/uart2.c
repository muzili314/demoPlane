/*brief: TX PA.2 RX:PA.3
 *
 *
 *note:TX RX�ֱ�ͬTIM2 CH3 CH4 ����IO��
*/

#include "uart2.h"
#include "msg.h"
#include "cmd_queue.h"

void uart2_Init(u32 pclk1,u32 bound)
{
	float temp;
	u16 mantissa,fraction;
	temp=(float)(pclk1*1000000)/(bound*16);//�õ� USARTDIV
	mantissa=temp; //��������
	fraction=(temp-mantissa)*16; //С������
	mantissa<<=4;
	mantissa+=fraction;
	
	/*����ʱ��*/
	RCC->APB1ENR |= (1<<17);//UART2
	RCC->APB2ENR |= (1<<0);//AFIO
//	RCC->APB2ENR |= (1<<2);//PA
	RCC->APB2ENR |= 1<<5; //PD
	//remap
	AFIO->MAPR |= 1<<3;
	/*����IO*/
	GPIOD->CRL &= 0XF00FFFFF;
	GPIOD->CRL |= 0X08B00000;
//	GPIOA->CRL &= 0XFFFF00FF;
//	GPIOA->CRL |= 0X00008B00;  //A.3 �������� A.2������� 
	/*���ô���ģʽ*/
	/*��λUSART2*/
	RCC->APB1RSTR |= (1<<17);
	RCC->APB1RSTR &= ~(1<<17);
	/*���ò����� ֹͣλ У��λ*/
	USART2->BRR = mantissa;
	USART2->CR1|=0X200C;  //1λֹͣ,��У��λ.UART Transmiter receiver enabled
	/*ʹ���ж� �趨���ȼ�*/
	USART2->CR1 |= (1<<5);//���ջ������ǿ��ж�ʹ��

}


void USART2_IRQHandler()
{
	u8 uart2Res;
	#ifdef OS_CRITICAL_METHOD 	//���OS_CRITICAL_METHOD������,˵��ʹ��ucosII��.
		OSIntEnter();    
	#endif
	
	if(USART2->SR & (1<<5))//���յ�����
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
	//while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
  while( ((USART2->SR&0X40)==0) && (i<120000) )
	{
		i++;
	}//ѭ������,ֱ���������  
}

/*brief:����һ���ַ���
 *
 *
 *bug:���͵ĵڶ����ַ��ᶪʧ
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
/*brief:����һ���ַ�
 *
 * 
 *
*/
void Uart2SendChar(u8 ch)
{
	USART2->DR = ch;
	while(((USART2->SR & (1<<6)) == 0));//�ȴ��������
}


