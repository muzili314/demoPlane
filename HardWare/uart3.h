#ifndef  _H_UART3_H_
#define  _H_UART3_H_
#include "sys.h"

extern void uart3_Init(u32 pclk1,u32 bound);
static void Uart3_Send(u8 ch);
extern void uart3SendStr(char *s);


#endif
