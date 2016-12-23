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

//���ڼ���PWM
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
 *  \brief  ��Ϣ�������̣��˴�һ�㲻��Ҫ����
 *  \param msg ��������Ϣ
 *  \param size ��Ϣ����
 */
void ProcessMessage( PCTRL_MSG msg, u16 size )
{
	u8 cmd_type = msg->cmd_type;//ָ������
	//u8 ctrl_msg = msg->ctrl_msg;   //��Ϣ������
	u8 control_type = msg->control_type;//�ؼ�����
	u16 screen_id = PTR2U16(&msg->screen_id);//����ID
	u16 control_id = PTR2U16(&msg->control_id);//�ؼ�ID
	u32 value = PTR2U32(msg->param);//��ֵ

	Uart2SendChar(cmd_type);
	Uart2SendChar(screen_id);
	Uart2SendChar(control_id);
	switch(cmd_type)
	{
	case NOTIFY_SCREEN://�л�����
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
		NotifyReadFlash(1,cmd_buffer+2,size-6);//ȥ��֡ͷ֡β
		break;
	case NOTIFY_READ_FLASH_FAILD:
		NotifyReadFlash(0,0,0);
		break;
	case NOTIFY_CONTROL:
		{
			Uart2SendChar(control_type);
			switch(control_type)
			{
			case kCtrlButton: //��ť�ؼ�
				NotifyButton(screen_id,control_id,msg->param[1]);
				break;
			case kCtrlText://�ı��ؼ�
				NotifyText(screen_id,control_id,msg->param);
				break;
			case kCtrlProgress: //�������ؼ�
				NotifyProgress(screen_id,control_id,value);
				break;
			case kCtrlSlider: //�������ؼ�
				NotifySlider(screen_id,control_id,value);
				break;
			case kCtrlMeter: //�Ǳ�ؼ�
				NotifyMeter(screen_id,control_id,value);
				break;
			case kCtrlMenu://�˵��ؼ�
				NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);
				break;
			case kCtrlSelector://ѡ��ؼ�
				NotifySelector(screen_id,control_id,msg->param[0]);
				break;
			case kCtrlRTC://����ʱ�ؼ�
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
 *  \brief  �����л�֪ͨ
 *  \details  ��ǰ����ı�ʱ(�����GetScreen)��ִ�д˺���
 *  \param screen_id ��ǰ����ID
 */
void NotifyScreen(u16 screen_id)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  ���������¼���Ӧ
 *  \param press 1���´�������3�ɿ�������
 *  \param x x����
 *  \param y y����
 */
void NotifyTouchXY(u8 press,u16 x,u16 y)
{
	//TODO: ����û�����
}

//�ı��ؼ���ʾ����ֵ
void SetTextValueint(u16 screen_id, u16 control_id,long value)
{
	char buffer[12] = {0};
	sprintf(buffer,"%ld",value);
	SetTextValue(screen_id,control_id,(u8 *)buffer);
}

//�ַ���ת����
int StringToint(u8 *str)
{
	long v = 0;
	sscanf((char *)str,"%ld",&v);
	return v;
}

//���½���ؼ���ʾ
void UpdateUI()
{
	SetTextValueint(0,2,test_value);
	SetProgressValue(0,3,test_value);
	SetSliderValue(0,4,test_value);	
	SetMeterValue(0,7,test_value);
}

/*! 
 *  \brief  ��ť�ؼ�֪ͨ
 *  \details  ����ť״̬�ı�(�����GetControlValue)ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param state ��ť״̬��0����1����
 */
void NotifyButton(u16 screen_id, u16 control_id, u8  state)
{
	//TODO: ����û�����
	/*���Դ���*/
	if(screen_id==1 && control_id==3) //start button
	{
		OSTaskResume(MOTOR_TASK_PRIO);
		OSTaskResume(MOTOR1_TASK_PRIO);
		TIM2->CR1 |= 1<<0;    //������ʱ��
		TIM1->CR1 |= 1<<0;    //������ʱ��
//		MOTOR_EN = 0;
//		MOTOR1_EN = 0;
		printf("start\r\n");
	}
	else if(screen_id==1 && control_id==6)//stop button
	{
		OSTaskSuspend(MOTOR_TASK_PRIO);
		OSTaskSuspend(MOTOR1_TASK_PRIO);
		TIM2->CR1 &= ~(1<<0);       //�رն�ʱ��
		TIM1->CR1 &= ~(1<<0);       //�رն�ʱ��
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

	if(screen_id==0&&control_id==5)//������ť
	{
		if(test_value<100)
			++test_value;
	}
	else if(screen_id==0&&control_id==6)//�ݼ���ť
	{
		if(test_value>0)
			--test_value;
	}

	update_en = 1;

}

/*! 
 *  \brief  �ı��ؼ�֪ͨ
 *  \details  ���ı�ͨ�����̸���(�����GetControlValue)ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param str �ı��ؼ�����
 */
void NotifyText(u16 screen_id, u16 control_id, u8 *str)
{
	//TODO: ����û�����
	
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
	
	
	//��е��2
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
 *  \brief  �������ؼ�֪ͨ
 *  \details  ����GetControlValueʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param value ֵ
 */
void NotifyProgress(u16 screen_id, u16 control_id, u32 value)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  �������ؼ�֪ͨ
 *  \details  ���������ı�(�����GetControlValue)ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param value ֵ
 */
void NotifySlider(u16 screen_id, u16 control_id, u32 value)
{
	//TODO: ����û�����
	test_value = value;

	//UpdateUI();
	update_en = 1;
}

/*! 
 *  \brief  �Ǳ�ؼ�֪ͨ
 *  \details  ����GetControlValueʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param value ֵ
 */
void NotifyMeter(u16 screen_id, u16 control_id, u32 value)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  �˵��ؼ�֪ͨ
 *  \details  ���˵���»��ɿ�ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param item �˵�������
 *  \param state ��ť״̬��0�ɿ���1����
 */
void NotifyMenu(u16 screen_id, u16 control_id, u8  item, u8  state)
{
	//TODO: ����û�����
	//��λ��ѡ��ģʽ
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
 *  \brief  ѡ��ؼ�֪ͨ
 *  \details  ��ѡ��ؼ��仯ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param item ��ǰѡ��
 */
void NotifySelector(u16 screen_id, u16 control_id, u8  item)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  ��ʱ����ʱ֪ͨ����
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 */
void NotifyTimer(u16 screen_id, u16 control_id)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  ��ȡ�û�FLASH״̬����
 *  \param status 0ʧ�ܣ�1�ɹ�
 *  \param _data ��������
 *  \param length ���ݳ���
 */
void NotifyReadFlash(u8 status,u8 *_data,u16 length)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  д�û�FLASH״̬����
 *  \param status 0ʧ�ܣ�1�ɹ�
 */
void NotifyWriteFlash(u8 status)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  ���û����������ֵת��Ϊ�����PWM
 *  \param axis_x axis_y axis_z Ϊ����ֵ
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
	//������ʾPWMֵ
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
	//����TIMִ���жϺ���
	//������ʱ���ж�
	TIM3->DIER |= 0X001E;
	TIM4->DIER |= 0X0002;
	
//	sprintf(buff,"%f %f %f %f %f %f %d %d %d %d",axis_x,axis_y,axis_z,alpha,beta,phi,pwm0_dest,pwm1_dest,pwm2_dest,pwm3_dest);
//	printf("%s\n",buff);
	
}

//���ڻ�е��2������--pwmת��
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
	//������ʾPWMֵ
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
	//����TIMִ���жϺ���
	//������ʱ���ж�
	TIM4->DIER |= 0X001C;
	TIM5->DIER |= 0X0018;
	
}

