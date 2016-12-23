/*uart2.h
 *
 *
 *
*/
#ifndef _H_UART2_H_
#define _H_UART2_H_

#include "sys.h"

extern void uart2_Init(u32 pclk2,u32 bound);
extern void uart2SendStr(char *s);
static void Uart2_Send(u16 ch);
extern void Uart2SendChar(u8 ch);

#endif
