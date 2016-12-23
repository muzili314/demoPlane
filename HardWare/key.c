/*
 *
 *
 *
*/
#include "key.h"
#include "delay.h"

//������ʼ������
/*key2 PA.0 key3 PC.13*/
void KEY_Init(void)
{
	RCC->APB2ENR|=1<<2;     //ʹ��PORTAʱ��
	RCC->APB2ENR|=1<<4;     //ʹ��PORTCʱ��

	GPIOA->CRL&=0XFFFFFFF0;	//PA0���ó����룬����	  
	GPIOA->CRL|=0X00000008; 
	GPIOA->ODR|=0X00000001;

	GPIOC->CRH&=0XFF0FFFFF;	//PC13���ó����룬����	  
	GPIOC->CRH|=0X00800000; 
	GPIOC->ODR|=0X00002000;
	  
 }
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY1����
//2��KEY2����
//3��KEY3���� 
//4��KEY4����
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>KEY3!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up = 1;  //���ɿ���־ �ɿ�λ1 �м�����Ϊ0
	if(mode)  key_up = 1;  //֧������	
		  
	if(key_up&&(KEY1 == 0||KEY2 == 0))
	{
		delay_ms(10);//ȥ���� 
		key_up = 0;
		if(KEY1 == 0)      return 1;
		else if(KEY2 == 0) return 2;
	
	}
	else if(KEY1 == 1 && KEY2 == 1)	  //û�м�����
         key_up = 1; 	    
  return 0;       // �ް�������
    
}
