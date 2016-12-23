/*
 *TIM3 CH1: PC.6 CH2: PC.7  CH3: PC.8  CH4: PC.9 (full remap)
 *TIM4 CH1: PB.6 CH2: PB.7  CH3: PB.8  CH4: PB.9
 *TIM5 CH3:PA2 CH4:PA3
 *TIM3 ->TIM4 CH1 -> CH4 STEER 0 1 2 3 4 5
 *steer2 PB7 8 9 PA2 3
*/

#include "steer.h"
#include <math.h>
#include <msg.h>
#include <led.h>

//机械臂动作时间

u16 STEER_TIME = 3000;  //ms
u16 STEER_SAMPLE = 150;  //sample = time/40


//用于接收来自上位机的坐标
float axis_x = 0;
float axis_y = 12;
float axis_z = 16;
//机械臂2
float axis2_x = 0;
float axis2_y = 12;
float axis2_z = 16;

//舵机pwm值
//机械臂1
u16 pwm0_now = 1500;
u16 pwm1_now = 1500;
u16 pwm2_now = 1500;
u16 pwm3_now = 1500;
u16 pwm4_now = 1500;
//机械臂2
u16 pwm20_now = 1500;
u16 pwm21_now = 1500;
u16 pwm22_now = 1500;
u16 pwm23_now = 1500;
u16 pwm24_now = 1500;

//机械臂1
u16 pwm0_dest = 1500;
u16 pwm1_dest = 1500;
u16 pwm2_dest = 1500;
u16 pwm3_dest = 1500;
u16 pwm4_dest = 1500;
//机械臂2
u16 pwm20_dest = 1500;
u16 pwm21_dest = 1500;
u16 pwm22_dest = 1500;
u16 pwm23_dest = 1500;
u16 pwm24_dest = 1500;

u16 dist0[SAMPLETIMES+1];
u16 dist1[SAMPLETIMES+1];
u16 dist2[SAMPLETIMES+1];
u16 dist3[SAMPLETIMES+1];
u16 dist4[SAMPLETIMES+1];

//机械臂2
u16 dist20[SAMPLETIMES+1];
u16 dist21[SAMPLETIMES+1];
u16 dist22[SAMPLETIMES+1];
u16 dist23[SAMPLETIMES+1];
u16 dist24[SAMPLETIMES+1];

float x[SAMPLETIMES+1];
//舵机位置改变标志
u8 isPwm0Change = 0;
u8 isPwm1Change = 0;
u8 isPwm2Change = 0;
u8 isPwm3Change = 0;
u8 isPwm4Change = 0;
//机械臂2
u8 isPwm20Change = 0;
u8 isPwm21Change = 0;
u8 isPwm22Change = 0;
u8 isPwm23Change = 0;
u8 isPwm24Change = 0;


void steer_Init(u16 arr,u16 psc,u16 pwm)
{
	u8 i = 0;
   //开启时钟
	RCC->APB1ENR |= 1<<1; //TIM3
	RCC->APB1ENR |= 1<<2; //TIM4
	RCC->APB1ENR |= 1<<3; //TIM5
	RCC->APB2ENR |= 1<<2; //PA
	RCC->APB2ENR |= 1<<3; //PB
	RCC->APB2ENR |= 1<<4; //PC
	RCC->APB2ENR |= 1<<0; //AFIO
	//初始化IO 
	GPIOB->CRL &= 0X00FFFFFF;
	GPIOB->CRL |= 0XBB000000;
	GPIOB->CRH &= 0XFFFFFF00;
	GPIOB->CRH |= 0X000000BB;
	
	GPIOC->CRL &= 0X00FFFFFF;
	GPIOC->CRL |= 0XBB000000;
	GPIOC->CRH &= 0XFFFFFF00;
	GPIOC->CRH |= 0X000000BB;  //复用功能推挽输出
	//TIM5 IO PA2 3
	GPIOA->CRL &= 0XFFFF00FF;
	GPIOA->CRL |= 0X0000BB00;	
	//引脚重映射
	AFIO->MAPR |= 3<<10;   //TIM3 full remap
	//TIM预装载值及分频值
	TIM3->ARR = arr;
	TIM3->PSC = psc;
	TIM4->ARR = arr;
	TIM4->PSC = psc;
	TIM5->ARR = arr;
	TIM5->PSC = psc;
	//配置PWM
	//TIM3
	TIM3->CCMR1 |= 6<<4;  //CCR1 PWM model
	TIM3->CCMR1 |= 1<<3;  //CCR1 preload enabled
	TIM3->CCMR1 |= 6<<12; //CCR2 PWM model
	TIM3->CCMR1 |= 1<<11; //CCR2 preload enabled
	TIM3->CCMR2 |= 6<<4;  //CCR3 PWM model
	TIM3->CCMR2 |= 1<<3;  //CCR3 preload enabled
	TIM3->CCMR2 |= 6<<12; //CCR4 PWM model
	TIM3->CCMR2 |= 1<<11; //CCR4 preload enabled
	//TIM4
	TIM4->CCMR1 |= 6<<4;  //CCR1 PWM model
	TIM4->CCMR1 |= 1<<3;  //CCR1 preload enabled
	TIM4->CCMR1 |= 6<<12; //CCR2 PWM model
	TIM4->CCMR1 |= 1<<11; //CCR2 preload enabled
	TIM4->CCMR2 |= 6<<4;  //CCR3 PWM model
	TIM4->CCMR2 |= 1<<3;  //CCR3 preload enabled
	TIM4->CCMR2 |= 6<<12; //CCR4 PWM model
	TIM4->CCMR2 |= 1<<11; //CCR4 preload enabled
	//TIM5
	TIM5->CCMR2 |= 6<<4;  //CCR3 PWM model
	TIM5->CCMR2 |= 1<<3;  //CCR3 preload enabled
	TIM5->CCMR2 |= 6<<12; //CCR4 PWM model
	TIM5->CCMR2 |= 1<<11; //CCR4 preload enabled
  //配置PWM预装载值
	TIM3->CCR1 = pwm;
	TIM3->CCR2 = pwm;
	TIM3->CCR3 = pwm;
  TIM3->CCR4 = pwm;
	
	TIM4->CCR1 = pwm;
	TIM4->CCR2 = pwm;
	TIM4->CCR3 = pwm;
  TIM4->CCR4 = pwm;
	
	TIM5->CCR3 = pwm;
  TIM5->CCR4 = pwm;

	//使能中断及定时器输出
	TIM3->CCER = 0X1111;
	TIM4->CCER = 0X1111; //使能PWM输出
	TIM5->CCER = 0X1100; //使能PWM输出
	//enabled capture and update interrupt
//	TIM3->DIER = 0X001F;
//	TIM4->DIER = 0X001F;
//	TIM5->DIER = 0X0018;
	
	
	TIM3->CR1 |= 1<<0;
	TIM4->CR1 |= 1<<0;   //使能定时器计数
	TIM5->CR1 |= 1<<0;   //使能定时器计数
	//initial arry x
	for(i = 0;i<SAMPLETIMES+1;i++)
	{
		x[i] = i*1.4/SAMPLETIMES;
	}
	
}


/*arryDist
 *
 *
*/

void arryDist(void)
{
	u8 i = 0;
	for(i = 0;i<SAMPLETIMES+1;i++)
	{
		dist0[i] = pwm0_now+(pwm0_dest-pwm0_now)/2*(yfunc(x[i]));
		dist1[i] = pwm1_now+(pwm1_dest-pwm1_now)/2*(yfunc(x[i]));
		dist2[i] = pwm2_now+(pwm2_dest-pwm2_now)/2*(yfunc(x[i]));
		dist3[i] = pwm3_now+(pwm3_dest-pwm3_now)/2*(yfunc(x[i]));
		dist4[i] = pwm4_now+(pwm4_dest-pwm4_now)/2*(yfunc(x[i]));
	}
	pwm0_now = pwm0_dest;
	pwm1_now = pwm1_dest;
	pwm2_now = pwm2_dest;
	pwm3_now = pwm3_dest;
	pwm4_now = pwm4_dest;
}


void arryDist_2(void)
{
	u8 i = 0;
	for(i = 0;i<SAMPLETIMES+1;i++)
	{
		dist20[i] = pwm20_now+(pwm20_dest-pwm20_now)/2*(yfunc(x[i]));
		dist21[i] = pwm21_now+(pwm21_dest-pwm21_now)/2*(yfunc(x[i]));
		dist22[i] = pwm22_now+(pwm22_dest-pwm22_now)/2*(yfunc(x[i]));
		dist23[i] = pwm23_now+(pwm23_dest-pwm23_now)/2*(yfunc(x[i]));
		dist24[i] = pwm24_now+(pwm24_dest-pwm24_now)/2*(yfunc(x[i]));
	}
	pwm20_now = pwm20_dest;
	pwm21_now = pwm21_dest;
	pwm22_now = pwm22_dest;
	pwm23_now = pwm23_dest;
	pwm24_now = pwm24_dest;
}

float yfunc(float x)
{
	float y;
	if(x <= 0.7)
		y = powf(expf(1),x) - 1;
	else if(x > 0.7 && x <= 1.4)
		y = 3 - powf(expf(1),(1.4 - x));
	else
		printf("the value of x is wrong!\n");
	return y;
}
//TIM Interrupt

/*TIM3 中断函数
 *
 *
 *
*/
void TIM3_IRQHandler(void)
{
	static u8 time0 = 0,time1 = 0,time2 = 0,time3 = 0;
	static u8 i0 = 0,i1 = 0,i2 = 0,i3 = 0;

	// TIME/20/SAMPLETIMES
	OSIntEnter();
	if((TIM3->SR & (1<<0)))//产生更新中断
	{
    TIM3->SR &= ~(1<<0);
	}
	//steer0
	else if(TIM3->SR & (1<<1))  
	{
		if(isPwm0Change)
		{
			time0++;
			if(time0 == TIME/20/SAMPLETIMES)
			{
				time0 = 0;
				TIM3->CCR1 = dist0[i0];
				i0++;
				if(i0 == SAMPLETIMES+1)
				{
					i0 = 0;
					isPwm0Change = 0;
					TIM3->DIER &= ~(1<<1);
				}
					
			}
		}		
		TIM3->SR &= ~(1<<1);
	}
	//steer1
	else if(TIM3->SR & (1<<2))  //CCR2中断
	{
		if(isPwm1Change)
		{
			time1++;
			if(time1 == TIME/20/SAMPLETIMES)
			{
				time1 = 0;
				TIM3->CCR2 = dist1[i1];
				i1++;
				if(i1 == SAMPLETIMES+1)
				{
					i1 = 0;
					isPwm1Change = 0;
					TIM3->DIER &= ~(1<<2);
				}
					
			}
		}	
		TIM3->SR &= ~(1<<2);
	}
	//steer2
	else if(TIM3->SR & (1<<3))  //CCR3中断
	{
		if(isPwm2Change)
		{
			time2++;
			if(time2 == TIME/20/SAMPLETIMES)
			{
				time2 = 0;
				TIM3->CCR3 = dist2[i2];
				i2++;
				if(i2 == SAMPLETIMES+1)
				{
					i2 = 0;
					isPwm2Change = 0;
					TIM3->DIER &= ~(1<<3);
				}
					
			}
		}	
		
		TIM3->SR &= ~(1<<3);
	}
	//steer3
	else if(TIM3->SR & (1<<4))  //CCR4中断
	{
		if(isPwm3Change)
		{
			time3++;
			if(time3 == TIME/20/SAMPLETIMES)
			{
				time3 = 0;
				TIM3->CCR4 = dist3[i3];
				i3++;
				if(i3 == SAMPLETIMES+1)
				{
					i3 = 0;
					isPwm3Change = 0;
					TIM3->DIER &= ~(1<<4);
				}
					
		  }
		}
		TIM3->SR &= ~(1<<4);
		
	}

 OSIntExit();
}


/*TIM4 中断函数
 *
 *
 *
*/
void TIM4_IRQHandler(void)
{
	static u8 time4 = 0,time20 = 0,time21 = 0,time22 = 0;
	static u8 i4 = 0,i20 = 0,i21 = 0,i22 = 0;
	
	OSIntEnter();
	if((TIM4->SR & (1<<0)))//产生更新中断
	{
    TIM4->SR &= ~(1<<0);
	}
	//机械臂1 steer4
	else if(TIM4->SR & (1<<1))  
	{
		if(isPwm4Change)
		{
			time4++;
			if(time4 == TIME/20/SAMPLETIMES)
			{
				time4 = 0;
				TIM4->CCR1 = dist4[i4];
				i4++;
				if(i4 == SAMPLETIMES+1)
				{
					i4 = 0;
					isPwm4Change = 0;
					TIM4->DIER &= ~(1<<1);
				}
					
			}
		}	
		TIM4->SR &= ~(1<<1);
	}
	else if(TIM4->SR & (1<<2))  //CCR2中断
	{
		if(isPwm20Change)
		{
			time20++;
			if(time20 == TIME/20/SAMPLETIMES)
			{
				time20 = 0;
				TIM4->CCR2 = dist20[i20];
				i20++;
				if(i20 == SAMPLETIMES+1)
				{
					i20 = 0;
					isPwm20Change = 0;
					TIM4->DIER &= ~(1<<2);
				}
					
			}
		}	
		TIM4->SR &= ~(1<<2);
	}
	else if(TIM4->SR & (1<<3))  //CCR3中断
	{
		if(isPwm21Change)
		{
			time21++;
			if(time21 == TIME/20/SAMPLETIMES)
			{
				time21 = 0;
				TIM4->CCR3 = dist21[i21];
				i21++;
				if(i21 == SAMPLETIMES+1)
				{
					i21 = 0;
					isPwm21Change = 0;
					TIM4->DIER &= ~(1<<3);
				}
					
			}
		}	
		TIM4->SR &= ~(1<<3);
	}
	else if(TIM4->SR & (1<<4))  //CCR4中断
	{
		if(isPwm22Change)
		{
			time22++;
			if(time22 == TIME/20/SAMPLETIMES)
			{
				time22 = 0;
				TIM4->CCR4 = dist22[i22];
				i22++;
				if(i22 == SAMPLETIMES+1)
				{
					i22 = 0;
					isPwm22Change = 0;
					TIM4->DIER &= ~(1<<4);
				}
					
			}
		}	
		TIM4->SR &= ~(1<<4);
	}
	
   OSIntExit();
}

/*TIM5中断
 *
 *
*/

void TIM5_IRQHandler(void)
{
	static u8 time23 = 0,time24 = 0;
	static u8 i23 = 0,i24 = 0;
	OSIntEnter();
	
	if(TIM5->SR & (1<<0))
	{
		TIM5->SR &= ~(1<<0);
	}
	else if(TIM5->SR & (1<<1))  //CCR1中断
	{
		TIM5->SR &= ~(1<<1);
	}
	else if(TIM5->SR & (1<<2))  //CCR2中断
	{
		TIM5->SR &= ~(1<<2);
	}
	else if(TIM5->SR & (1<<3))  //CCR3中断
	{
		if(isPwm23Change)
		{
			time23++;
			if(time23 == TIME/20/SAMPLETIMES)
			{
				time23 = 0;
				TIM5->CCR3 = dist23[i23];
				i23++;
				if(i23 == SAMPLETIMES+1)
				{
					i23 = 0;
					isPwm23Change = 0;
					TIM5->DIER &= ~(1<<3);
				}
					
			}
		}	
		TIM5->SR &= ~(1<<3);
	}
	else if(TIM5->SR & (1<<4))  //CCR4中断
	{
		if(isPwm24Change)
		{
			time24++;
			if(time24 == TIME/20/SAMPLETIMES)
			{
				time24 = 0;
				TIM5->CCR4 = dist24[i24];
				i24++;
				if(i24 == SAMPLETIMES+1)
				{
					i24 = 0;
					isPwm24Change = 0;
					TIM5->DIER &= ~(1<<4);
				}
					
			}
		}	
		TIM5->SR &= ~(1<<4);
	}
	
	OSIntExit();
}
