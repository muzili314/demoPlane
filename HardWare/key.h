/*key.h
 *brief :
 *
 *
*/

#ifndef _KEY_H_
#define _KEY_H_

#include "sys.h"

#define KEY1 PAin(0)   	
#define KEY2 PCin(13)	

#define KEY_1	1
#define KEY_2	2

void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8);  	//����ɨ�躯��


#endif
