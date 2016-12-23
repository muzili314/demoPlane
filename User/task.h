/*当前最大任务数为	20 若需更多修改 os_cfg.h 中的OS_MAX_TASKS(row 46)
 *
 *
 *
*/
#ifndef _TASK_H_
#define _TASK_H_

#include "includes.h"

//START 任务
//设置任务优先级
#define START_TASK_PRIO      	  10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//任务函数
void StartTask(void *pdata);	

/*主任务 用于接收串口屏的命令*/
#define MAIN_TASK_PRIO       			2 
#define MAIN_STK_SIZE  					512
void MainTask(void *pdata);


/*步进电机任务*/
#define MOTOR_TASK_PRIO       			3
#define MOTOR_STK_SIZE  					 512
void MotorTask(void *pdata);

/*步进电机任务*/
#define MOTOR1_TASK_PRIO       			4
#define MOTOR1_STK_SIZE  					 512
void Motor1Task(void *pdata);


void tmr1_callback(OS_TMR *ptmr,void *p_arg);

#endif
