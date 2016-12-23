/*timer.h
 *
 *
 *
*/
#ifndef _H_TIMER_H_
#define _H_TIMER_H_

#include "sys.h"

extern u16 TIM2_ARR;
extern u16 TIM1_ARR;
extern volatile u8 tim2IntVal;

extern void TIM2_PWM_Init(u16 arr,u16 psc);
extern void TIM1_Init(u16 arr,u16 psc);


#endif

