/*��ǰ���������Ϊ	20 ��������޸� os_cfg.h �е�OS_MAX_TASKS(row 46)
 *
 *
 *
*/
#ifndef _TASK_H_
#define _TASK_H_

#include "includes.h"

//START ����
//�����������ȼ�
#define START_TASK_PRIO      	  10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				64
//������
void StartTask(void *pdata);	

/*������ ���ڽ��մ�����������*/
#define MAIN_TASK_PRIO       			2 
#define MAIN_STK_SIZE  					512
void MainTask(void *pdata);


/*�����������*/
#define MOTOR_TASK_PRIO       			3
#define MOTOR_STK_SIZE  					 512
void MotorTask(void *pdata);

/*�����������*/
#define MOTOR1_TASK_PRIO       			4
#define MOTOR1_STK_SIZE  					 512
void Motor1Task(void *pdata);


void tmr1_callback(OS_TMR *ptmr,void *p_arg);

#endif
