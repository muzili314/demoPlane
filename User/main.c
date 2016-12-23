/*ϵͳͷ�ļ�*/
#include "sys.h"
#include "delay.h"
#include "usart.h"
/*Ucosiiͷ�ļ�*/
#include "includes.h"
#include "task.h"
#include "msg.h"
/*������ͷ�ļ�*/
#include "hmi_driver.h"
#include "cmd_queue.h"
#include "cmd_process.h"
#include "stdio.h"
/*����ͷ�ļ�*/
#include "malloc.h"
#include "led.h"
#include "key.h"
#include "timer.h"
#include "uart2.h"
#include "uart3.h"
#include "exit.h"
#include "motor.h"
#include "machine.h"
#include "steer.h"
#include "math.h"

//�����ջ	
OS_STK START_TASK_STK[START_STK_SIZE];
OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
OS_STK MOTOR_TASK_STK[MOTOR_STK_SIZE];
OS_STK MOTOR1_TASK_STK[MOTOR1_STK_SIZE];

//�����ʱ��
OS_TMR   *tmr1;		     	//�����ʱ��1
//�ź���
OS_EVENT *uartRx_Sem;
OS_EVENT *pwm1Change_Sem;
OS_EVENT *motor1_Sem;
OS_EVENT *motor2_Sem;
u8 MODEL = 0;

//���λ��
#define STACK_NUM 3*6
s8 stack[STACK_NUM] = {-5,15,-4,-3,15,-4,-1,15,-4,1,15,-4,3,15,-4,5,15,-4};
u8 item = 0;
	
void armGrab(u8 id,float x,float y,float z);
void armCapture(u8 id);
void armDrop(u8 id);
int main(void)
{
	Stm32_Clock_Init(9);     //ϵͳʱ����
	/*uart1 ��Ϊ���ڵ���*/
	uart_init(72,115200);	 	//��ʼ�����ڲ�����115200  APB2
	/*uart2 �봮��������ͨ��*/
	uart2_Init(36,115200);  //ע��uart2��ʱ�����Ƶ��36M  APB1
	/*uart3 ���е�۷���ָ��*/
//	uart3_Init(36,9600);  
	delay_init(72);	        //��ʱ��ʼ��
	LED_Init();
	MOTOR_Init();
	steer_Init(19999,71,1500); //f=1000k counter to 20ms 20 000
	TIM2_PWM_Init(500,35);
	TIM1_Init(500,35);
	EXTIX_Init();
	//uart test
	printf("test 123 uart1\r\n");
  uart2SendStr("abc 123 uart2\r\n");

 	mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��	
	
	/*��ʼ��Ucosii*/	
	OSInit();
	
	OSTaskCreate(
								StartTask,
								NULL,
								(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],
								START_TASK_PRIO 
	            );
	OSStart();	
	
}

void StartTask(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
//	u8 err;	    	             //�Լ�����ı���Ҫ��pdata = pdata֮ǰ
	pdata = pdata; 						//��ֹ�������
	  
	OSStatInit();					//��ʼ��ͳ������.�������ʱ1��������	
	/*�����ź�����*/
	uartRx_Sem = OSSemCreate(0);
	motor1_Sem = OSSemCreate(0);
	motor2_Sem = OSSemCreate(0);
 	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��) 
  /*�����ʱ��*/	
/* 	tmr1=OSTmrCreate(
										10,10,
										OS_TMR_OPT_PERIODIC,
										(OS_TMR_CALLBACK)tmr1_callback,
										0,"tmr1",&err
										);		     //100msִ��һ��	   10*10*/
//	OSTmrStart(tmr1,&err);       //���������ʱ��1	
	

	/*��������*/
	OSTaskCreate(
								MainTask,
								NULL,
							 (OS_STK *)&MAIN_TASK_STK[MAIN_STK_SIZE-1],
								MAIN_TASK_PRIO 
	           );
						 
	OSTaskCreate(
								MotorTask,
								NULL,
							 (OS_STK *)&MOTOR_TASK_STK[MOTOR_STK_SIZE-1],
								MOTOR_TASK_PRIO 
	           );	
						 
	OSTaskCreate(
								Motor1Task,
								NULL,
							 (OS_STK *)&MOTOR1_TASK_STK[MOTOR1_STK_SIZE-1],
								MOTOR1_TASK_PRIO 
	           );	

	
	OSTaskDel(OS_PRIO_SELF);	//ɾ����ʼ����
	OS_EXIT_CRITICAL();				//�˳��ٽ���
}

//�����ʱ��1�Ļص�����	
/*ÿ100ms ˢ����Ļ*/
void tmr1_callback(OS_TMR *ptmr,void *p_arg)
{
		update_en = 0;
		UpdateUI();
}	
/*�����źŴ�������*/
void MainTask(void *pdata)
{
//	OS_CPU_SR cpu_sr=0;
	u8 err;
	qsize size = 0;   	            
	pdata = pdata; 					
	/*������ڽ��ջ�����*/
	queue_reset();
  /*�ȴ���������ʼ��*/
	delay_ms(300);
	while(1)
	{
		/*�ȴ������յ�����*/
		OSSemPend(uartRx_Sem,0,&err);
		size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE);        
		if(size>0)
		{
			ProcessMessage((PCTRL_MSG)cmd_buffer, size);
		}
		delay_ms(200);
		LED1 = ~LED1; //led1��˸ָʾϵͳ��������
		
	}
}



/********�������1����*********/
void MotorTask(void *pdata)
{
	//delay time once
	u16 time = MOTORTIME*1000/(SAMPLES-1);
	u8 i = 0;
	u8 err;
	pdata = pdata;
	
	//����Ϊ�� ʹ�ܵ��
	MOTOR_DIR = 1;
	MOTOR_EN = 0;
	delay_ms(2000);
	MOTOR_DIR = 0;
	while(1)
	{
		if(EDDY0 == 1)
		{
			delay_ms(100);
			if(EDDY0 == 1)
				break;
		}
		delay_ms(200);
	}
	delay_ms(1200);
	MOTOR_EN = 1;
	MOTOR_DIR = 1;

	//����ǰ���񣬵ȴ���������������
	OSTaskSuspend(OS_PRIO_SELF);
	MOTOR_EN = 0;
	while(1)
	{
		for(i = 0;i < SAMPLES - 1;i++)
		{
			TIM2->ARR = load[i];
			TIM2->CCR2 = load[i]/2;
			TIM2->CNT = 0;
			delay_ms(time);
			//�����յ㻹��3000ms ��е�ۿ�ʼ�˶�
			if(i == (SAMPLES-2-2000/time) && MOTOR_DIR)
			{
				armGrab(1,13.5,2,1);
			}
		}		
		MOTOR_EN = 1;
		MOTOR_DIR = !MOTOR_DIR;
		//���������յ�
		if(!MOTOR_DIR)
		{
			delay_ms(1000);
			armCapture(1);
			//�ƶ����ͷ�λ��
			armGrab(1,-13,0.5,1);
			OSSemPost(motor2_Sem);
			//�����˶�
			MOTOR_EN = 0;
		}
		//���︺�����յ�
		else
		{
			switch(MODEL)
			{
				//ģʽ1
				case 0:
          OSSemPend(motor1_Sem,0,&err);
				//�ͷ�ľ��
					delay_ms(2500);
				  armDrop(2);
				//�����˶�
					MOTOR_EN = 0;
					break;
				//ģʽ2
				case 1:
					OSSemPend(motor1_Sem,0,&err);
					delay_ms(2000);
				  MOTOR_EN = 0;
					break;
				default:
					break;
			}
		}
		
	}
}

//�������2����

void Motor1Task(void *pdata)
{
 //delay time once
	u16 time = MOTORTIME*1000/(SAMPLES-1);
	u8 i = 0;
	u8 err = 0;
	pdata = pdata;

	MOTOR1_DIR = 1;
	MOTOR1_EN = 0;
	delay_ms(5000);
	MOTOR1_DIR = 0;
	while(1)
	{
		if(EDDY1 == 1)
		{
				delay_ms(100);
			if(EDDY1 == 1)
				break;
		}
		delay_ms(200);
	}

	delay_ms(3800);
	MOTOR1_EN = 1;
	MOTOR1_DIR = 1;

	//����ǰ����ȴ�������������
	OSTaskSuspend(OS_PRIO_SELF);
  OSSemPend(motor2_Sem,0,&err);
	
	delay_ms(2500);
	armDrop(1);
	MOTOR1_EN = 0;
	while(1)
	{
		for(i = 0;i < SAMPLES - 1;i++)
		{
			TIM1->ARR = load[i];
			TIM1->CCR1 = load[i]/2;
			TIM1->CNT = 0;
			delay_ms(time);
			//�����յ㻹��3000ms ��е�ۿ�ʼ�˶�
			if(i == (SAMPLES-2-3000/time) && MOTOR1_DIR)
			{
				armGrab(2,12,1.5,1);
			}
		}
		MOTOR1_EN = 1;
		MOTOR1_DIR = !MOTOR1_DIR;
		//�������յ�
	  if(!MOTOR1_DIR)
		{
			delay_ms(1000);
			armCapture(2);
			switch(MODEL)
			{
				//model1
				case 0:
					armGrab(2,-12,3,1);
					MOTOR1_EN = 0;
					OSSemPost(motor1_Sem);
					break;
				//model 2
				case 1:
				//���
					armGrab(2,stack[item],stack[item+1],stack[item+2]);
				  item +=3;
				  if(item == STACK_NUM)
						item = 0;
					MOTOR1_EN = 0;
					armDrop(2);
					OSSemPost(motor1_Sem);
					break;
				default:
					break;
			 }
		}
		//���︺���յ�
		else
		{
			//�ȴ���е��1����ľ��
			OSSemPend(motor2_Sem,0,&err);
			delay_ms(2500);
			armDrop(1);
			MOTOR1_EN = 0;		
		}
				
	}
}

/*��е��ץȡ����
 *id ��е�۱��
 *
*/
void armGrab(u8 id,float x,float y,float z)
{
  if(id == 1)
	{
		axis_x = x;
		axis_y = y;
		axis_z = z;
		axisToAngle(x,y,z);
	}
	if(id == 2)
	{
		axis2_x = x;
		axis2_y = y;
		axis2_z = z;
		axisToAngle_2(x,y,z);
	}
		
}


//���� �ſ麯��
/*armCapture
 *
 *
 *
*/
void armCapture(u8 id)
{
	//����
	//��ѹ
	//̧��
	if(id == 1)
	{
		AIR_PUMP = 1;
		RELAY_1 = 1;
		delay_ms(500);
		TIM3->CCR2 = pwm1_dest-70;
		delay_ms(1000);
		pwm1_dest += 100;
		TIM3->CCR2 = pwm1_dest;
		
	}
	else if(id == 2)
	{
		AIR_PUMP = 1;
		RELAY_3 = 1;
		delay_ms(500);
		TIM4->CCR3 = pwm21_dest-40;
		delay_ms(1000);
		pwm21_dest += 50;
		TIM4->CCR3 = pwm21_dest;
	}
}

/*armDrop
 *
 *
 *
*/
void armDrop(u8 id)
{
	//��ѹ
	//����
	//̧��
	if(id == 1)
	{
		TIM3->CCR2 = pwm1_dest -70;
		delay_ms(1000);
		AIR_PUMP = 0;
		RELAY_1 = 0;
		delay_ms(500);
		RELAY_4 = 1;
		delay_ms(1000);
		RELAY_4 = 0;
		pwm1_dest += 100;
		TIM3->CCR2 = pwm1_dest;
	}
	else if(id == 2)
	{
		TIM4->CCR3 = pwm21_dest-40;
		delay_ms(1000);
		AIR_PUMP = 0;
		RELAY_3 = 0;
		delay_ms(500);
		RELAY_2 = 1;
		delay_ms(1000);
		RELAY_2 = 0;
		pwm21_dest += 50;
		TIM4->CCR3 = pwm21_dest;
	}
}
/********************task end**********************************************/
