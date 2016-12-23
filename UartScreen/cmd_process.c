#include "cmd_process.h"
#include "hmi_driver.h"
#include "includes.h"
#include "task.h"
#include "usart.h"
#include "uart2.h"
#include "led.h"
#include "steer.h"
#include <math.h>
#include "msg.h"
#include "motor.h"
u8 cmd_buffer[CMD_MAX_SIZE];

static long test_value = 0;
u8 update_en = 0;

//用于计算PWM
#define PI 3.14159
//covert radian to degree
#define RTD(x) ((x)/PI*180)
//covert degree to radian
#define DTR(x) (float(x)/180*PI)

u8 bodya = 4;
u8 bodyb = 16;
u8 bodyc = 12;

float alpha;
float beta;
float phi;
float gamma;
float lambda;

float length,theta;


/*! 
 *  \brief  消息处理流程，此处一般不需要更改
 *  \param msg 待处理消息
 *  \param size 消息长度
 */
void ProcessMessage( PCTRL_MSG msg, u16 size )
{
	u8 cmd_type = msg->cmd_type;//指令类型
	//u8 ctrl_msg = msg->ctrl_msg;   //消息的类型
	u8 control_type = msg->control_type;//控件类型
	u16 screen_id = PTR2U16(&msg->screen_id);//画面ID
	u16 control_id = PTR2U16(&msg->control_id);//控件ID
	u32 value = PTR2U32(msg->param);//数值

	Uart2SendChar(cmd_type);
	Uart2SendChar(screen_id);
	Uart2SendChar(control_id);
	switch(cmd_type)
	{
	case NOTIFY_SCREEN://切换画面
		NotifyScreen(screen_id);
		break;
	case NOTIFY_TOUCH_PRESS:
	case NOTIFY_TOUCH_RELEASE:
		NotifyTouchXY(cmd_buffer[1],PTR2U16(cmd_buffer+2),PTR2U16(cmd_buffer+4));
		break;	
	case NOTIFY_WRITE_FLASH_OK:
		NotifyWriteFlash(1);
		break;
	case NOTIFY_WRITE_FLASH_FAILD:
		NotifyWriteFlash(0);
		break;
	case NOTIFY_READ_FLASH_OK:
		NotifyReadFlash(1,cmd_buffer+2,size-6);//去除帧头帧尾
		break;
	case NOTIFY_READ_FLASH_FAILD:
		NotifyReadFlash(0,0,0);
		break;
	case NOTIFY_CONTROL:
		{
			Uart2SendChar(control_type);
			switch(control_type)
			{
			case kCtrlButton: //按钮控件
				NotifyButton(screen_id,control_id,msg->param[1]);
				break;
			case kCtrlText://文本控件
				NotifyText(screen_id,control_id,msg->param);
				break;
			case kCtrlProgress: //进度条控件
				NotifyProgress(screen_id,control_id,value);
				break;
			case kCtrlSlider: //滑动条控件
				NotifySlider(screen_id,control_id,value);
				break;
			case kCtrlMeter: //仪表控件
				NotifyMeter(screen_id,control_id,value);
				break;
			case kCtrlMenu://菜单控件
				NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);
				break;
			case kCtrlSelector://选择控件
				NotifySelector(screen_id,control_id,msg->param[0]);
				break;
			case kCtrlRTC://倒计时控件
				NotifyTimer(screen_id,control_id);
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
}

/*! 
 *  \brief  画面切换通知
 *  \details  当前画面改变时(或调用GetScreen)，执行此函数
 *  \param screen_id 当前画面ID
 */
void NotifyScreen(u16 screen_id)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  触摸坐标事件响应
 *  \param press 1按下触摸屏，3松开触摸屏
 *  \param x x坐标
 *  \param y y坐标
 */
void NotifyTouchXY(u8 press,u16 x,u16 y)
{
	//TODO: 添加用户代码
}

//文本控件显示整数值
void SetTextValueint(u16 screen_id, u16 control_id,long value)
{
	char buffer[12] = {0};
	sprintf(buffer,"%ld",value);
	SetTextValue(screen_id,control_id,(u8 *)buffer);
}

//字符串转整数
int StringToint(u8 *str)
{
	long v = 0;
	sscanf((char *)str,"%ld",&v);
	return v;
}

//更新界面控件显示
void UpdateUI()
{
	SetTextValueint(0,2,test_value);
	SetProgressValue(0,3,test_value);
	SetSliderValue(0,4,test_value);	
	SetMeterValue(0,7,test_value);
}

/*! 
 *  \brief  按钮控件通知
 *  \details  当按钮状态改变(或调用GetControlValue)时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param state 按钮状态：0弹起，1按下
 */
void NotifyButton(u16 screen_id, u16 control_id, u8  state)
{
	//TODO: 添加用户代码
	/*测试代码*/
	if(screen_id==1 && control_id==3) //start button
	{
		OSTaskResume(MOTOR_TASK_PRIO);
		OSTaskResume(MOTOR1_TASK_PRIO);
		TIM2->CR1 |= 1<<0;    //开启定时器
		TIM1->CR1 |= 1<<0;    //开启定时器
//		MOTOR_EN = 0;
//		MOTOR1_EN = 0;
		printf("start\r\n");
	}
	else if(screen_id==1 && control_id==6)//stop button
	{
		OSTaskSuspend(MOTOR_TASK_PRIO);
		OSTaskSuspend(MOTOR1_TASK_PRIO);
		TIM2->CR1 &= ~(1<<0);       //关闭定时器
		TIM1->CR1 &= ~(1<<0);       //关闭定时器
//		MOTOR1_EN = 1;
//		MOTOR1_EN = 1;
		printf("stop\r\n");

	}
	//send button
	else if(screen_id == 2 && control_id == 18)
	{
		//update pwm value
		axisToAngle(axis_x,axis_y,axis_z);
	}
	
	//send2 button
	else if(screen_id == 2 && control_id == 51)
	{
		//update pwm value
		axisToAngle_2(axis2_x,axis2_y,axis2_z);
	}

	if(screen_id==0&&control_id==5)//递增按钮
	{
		if(test_value<100)
			++test_value;
	}
	else if(screen_id==0&&control_id==6)//递减按钮
	{
		if(test_value>0)
			--test_value;
	}

	update_en = 1;

}

/*! 
 *  \brief  文本控件通知
 *  \details  当文本通过键盘更新(或调用GetControlValue)时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param str 文本控件内容
 */
void NotifyText(u16 screen_id, u16 control_id, u8 *str)
{
	//TODO: 添加用户代码
	
	test_value = StringToint(str);

//	if(test_value>100)
//		test_value = 100;
//	else if(test_value<0)
//		test_value = 0;
	//axis_x
	if(screen_id == 2 && control_id == 10)
	{
		axis_x = (float)test_value;
	}
	//axis y
	else if(screen_id == 2 && control_id == 11)
	{
		axis_y = (float)test_value;
	}
	//axis z
	else if(screen_id == 2 && control_id == 12)
	{
		axis_z = (float)test_value;
	}
	
	
	//机械臂2
	//axis2_x
	else if(screen_id == 2 && control_id == 46)
	{
		axis2_x = (float)test_value;
	}
	//axis2 y
	else if(screen_id == 2 && control_id == 47)
	{
		axis2_y = (float)test_value;
	}
	//axis2_z
	else if(screen_id == 2 && control_id == 48)
	{
		axis2_z = (float)test_value;
	}
	
	update_en = 1;
	
}

/*! 
 *  \brief  进度条控件通知
 *  \details  调用GetControlValue时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param value 值
 */
void NotifyProgress(u16 screen_id, u16 control_id, u32 value)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  滑动条控件通知
 *  \details  当滑动条改变(或调用GetControlValue)时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param value 值
 */
void NotifySlider(u16 screen_id, u16 control_id, u32 value)
{
	//TODO: 添加用户代码
	test_value = value;

	//UpdateUI();
	update_en = 1;
}

/*! 
 *  \brief  仪表控件通知
 *  \details  调用GetControlValue时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param value 值
 */
void NotifyMeter(u16 screen_id, u16 control_id, u32 value)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  菜单控件通知
 *  \details  当菜单项按下或松开时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param item 菜单项索引
 *  \param state 按钮状态：0松开，1按下
 */
void NotifyMenu(u16 screen_id, u16 control_id, u8  item, u8  state)
{
	//TODO: 添加用户代码
	//上位机选择模式
	if(screen_id == 1 && control_id == 11 && item == 0 && state == 0)
	{
		MODEL = 0;
	}		
	else if(screen_id == 1 && control_id == 11 && item == 1 && state == 0)
	{
		MODEL = 1;
	}		
	
}

/*! 
 *  \brief  选择控件通知
 *  \details  当选择控件变化时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param item 当前选项
 */
void NotifySelector(u16 screen_id, u16 control_id, u8  item)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  定时器超时通知处理
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 */
void NotifyTimer(u16 screen_id, u16 control_id)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  读取用户FLASH状态返回
 *  \param status 0失败，1成功
 *  \param _data 返回数据
 *  \param length 数据长度
 */
void NotifyReadFlash(u8 status,u8 *_data,u16 length)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  写用户FLASH状态返回
 *  \param status 0失败，1成功
 */
void NotifyWriteFlash(u8 status)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  将用户输入的坐标值转化为舵机的PWM
 *  \param axis_x axis_y axis_z 为坐标值
 *  \param 
 */
void axisToAngle(float axis_x,float axis_y,float axis_z)
{
//	char buff[50];
	float temp;
	//covert to angle(radian)
	length = sqrt(pow(axis_x,2) + pow(axis_y,2) + pow(axis_z,2));
	theta = asin(axis_z / length);
	phi = atan(axis_x / axis_y);
	
	alpha = asin((pow(bodyb,2)+pow(length,2)-pow(bodyc,2))/(2*bodyb*length))-theta;	
	temp = length*cos(alpha+theta)/bodyc;
	if(temp > 1)
		temp = 1;
	else if(temp < -1)
		temp = -1;
	temp = acos(temp);
	if(length > 20)
		beta = alpha - temp;
	else 
		beta = alpha + temp;
	gamma = PI/2 - beta;
	lambda = phi;
	if(lambda >= PI/2)
		lambda = PI/2;
	else if(lambda <= -PI/2)
		lambda = -PI/2;
	if(lambda > PI/4)
		lambda = lambda - PI/2;
	else if(lambda < -PI/4)
		lambda = lambda + PI/2;
	
	//convert to degree
	alpha = RTD(alpha);
	beta = RTD(beta);
	phi = RTD(phi);
	gamma = RTD(gamma);
	lambda = RTD(lambda);;
	//convert to PWM
	pwm0_dest = 1500 + 11*phi;
	pwm1_dest = 1500 - 11*alpha;
	pwm2_dest = 1500 + 11*beta;
	pwm3_dest = 1500 + 11*gamma;
	pwm4_dest = 1500 + 11*lambda;
	
	if(pwm0_dest < 500)
		pwm0_dest = 500;
	else if(pwm0_dest > 2500)
		pwm0_dest = 2500;
	
	if(pwm1_dest < 500)
		pwm1_dest = 500;
	else if(pwm1_dest > 2500)
		pwm1_dest = 2500;
	
	if(pwm2_dest < 500)
		pwm2_dest = 500;
	else if(pwm2_dest > 2500)
		pwm2_dest = 2500;
	
	if(pwm3_dest < 500)
		pwm3_dest = 500;
	else if(pwm3_dest > 2500)
		pwm3_dest = 2500;
	
	if(pwm4_dest < 500)
		pwm4_dest = 500;
	else if(pwm4_dest > 2500)
		pwm4_dest = 2500;	
	//更新显示PWM值
  SetTextValueint(2,32,pwm0_dest);
	SetTextValueint(2,33,pwm1_dest);
	SetTextValueint(2,34,pwm2_dest);
	SetTextValueint(2,35,pwm3_dest);
	SetTextValueint(2,36,pwm4_dest);

	isPwm0Change = 1;
	isPwm1Change = 1;
	isPwm2Change = 1;
	isPwm3Change = 1;
	isPwm4Change = 1;
	
	arryDist();
	//允许TIM执行中断函数
	//开启定时器中断
	TIM3->DIER |= 0X001E;
	TIM4->DIER |= 0X0002;
	
//	sprintf(buff,"%f %f %f %f %f %f %d %d %d %d",axis_x,axis_y,axis_z,alpha,beta,phi,pwm0_dest,pwm1_dest,pwm2_dest,pwm3_dest);
//	printf("%s\n",buff);
	
}

//用于机械臂2的坐标--pwm转换
void axisToAngle_2(float axis2_x,float axis2_y,float axis2_z)
{
	float alpha2;
  float beta2;
  float phi2;
  float gamma2;
  float lambda2;

  float length2,theta2;
  float temp;
	//covert to angle(radian)
	length2 = sqrt(pow(axis2_x,2) + pow(axis2_y,2) + pow(axis2_z,2));
	theta2 = asin(axis2_z / length2);
	phi2 = atan(axis2_x / axis2_y);
	
	alpha2 = asin((pow(bodyb,2)+pow(length2,2)-pow(bodyc,2))/(2*bodyb*length2))-theta2;	
	temp = length2*cos(alpha2+theta2)/bodyc;
	if(temp > 1)
		temp = 1;
	else if(temp < -1)
		temp = -1;
	temp = acos(temp);
	if(length2 > 20)
		beta2 = alpha2 - temp;
	else 
		beta2 = alpha2 + temp;
	gamma2 = PI/2 - beta2;
	lambda2 = phi2;
	if(lambda2 >= PI/2)
		lambda2 = PI/2;
	else if(lambda2 <= -PI/2)
		lambda2 = -PI/2;
	if(lambda2 > PI/4)
		lambda2 = lambda2 - PI/2;
	else if(lambda2 < -PI/4)
		lambda2 = lambda2 + PI/2;
	
	//convert to degree
	alpha2 = RTD(alpha2);
	beta2 = RTD(beta2);
	phi2 = RTD(phi2);
	gamma2 = RTD(gamma2);
	lambda2 = RTD(lambda2);
	//convert to PWM
	pwm20_dest = 1500 + 11*phi2;
	pwm21_dest = 1500 - 11*alpha2;
	pwm22_dest = 1500 + 11*beta2;
	pwm23_dest = 1500 + 11*gamma2;
	pwm24_dest = 1500 + 11*lambda2;
	
	if(pwm20_dest < 500)
		pwm20_dest = 500;
	else if(pwm20_dest > 2500)
		pwm20_dest = 2500;
	
	if(pwm21_dest < 500)
		pwm21_dest = 500;
	else if(pwm21_dest > 2500)
		pwm21_dest = 2500;
	
	if(pwm22_dest < 500)
		pwm22_dest = 500;
	else if(pwm22_dest > 2500)
		pwm22_dest = 2500;
	
	if(pwm23_dest < 500)
		pwm23_dest = 500;
	else if(pwm23_dest > 2500)
		pwm23_dest = 2500;
	
	if(pwm24_dest < 500)
		pwm24_dest = 500;
	else if(pwm24_dest > 2500)
		pwm24_dest = 2500;	
	//更新显示PWM值
  SetTextValueint(2,65,pwm20_dest);
	SetTextValueint(2,66,pwm21_dest);
	SetTextValueint(2,67,pwm22_dest);
	SetTextValueint(2,68,pwm23_dest);
	SetTextValueint(2,69,pwm24_dest);

	isPwm20Change = 1;
	isPwm21Change = 1;
	isPwm22Change = 1;
	isPwm23Change = 1;
	isPwm24Change = 1;
	
	arryDist_2();
	//允许TIM执行中断函数
	//开启定时器中断
	TIM4->DIER |= 0X001C;
	TIM5->DIER |= 0X0018;
	
}

