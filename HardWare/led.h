/*brief
 *
 *
*/

#ifndef __LED_H
#define __LED_H	 
#include "sys.h"


//LED端口定义
//端口输出低电平LED亮

#define LED1 PBout(0)
#define LED2 PFout(7)
#define LED3 PFout(8)
	
extern void LED_Init(void);//初始化	

#endif


















