#ifndef _CMD_PROCESS_H
#define _CMD_PROCESS_H
#include "hmi_driver.h"

extern u8 MODEL;

extern u8 cmd_buffer[CMD_MAX_SIZE];
extern u8 update_en;



#define NOTIFY_TOUCH_PRESS      0X01   //����������֪ͨ
#define NOTIFY_TOUCH_RELEASE  0X03  //�������ɿ�֪ͨ
#define NOTIFY_WRITE_FLASH_OK  0X0C  //дFLASH�ɹ�
#define NOTIFY_WRITE_FLASH_FAILD  0X0D  //дFLASHʧ��
#define NOTIFY_READ_FLASH_OK  0X0B  //��FLASH�ɹ�
#define NOTIFY_READ_FLASH_FAILD  0X0F  //��FLASHʧ��
#define NOTIFY_MENU                        0X14  //�˵��¼�֪ͨ
#define NOTIFY_TIMER                       0X43  //��ʱ����ʱ֪ͨ
#define NOTIFY_CONTROL                0XB1  //�ؼ�����֪ͨ
#define NOTIFY_SCREEN                   0XB2  //�����л�֪ͨ

#define PTR2U16(PTR) ((((u8 *)(PTR))[0]<<8)|((u8 *)(PTR))[1])  //�ӻ�����ȡ16λ����
#define PTR2U32(PTR) ((((u8 *)(PTR))[0]<<24)|(((u8 *)(PTR))[1]<<16)|(((u8 *)(PTR))[2]<<8)|((u8 *)(PTR))[3])  //�ӻ�����ȡ32λ����

enum CtrlType
{
	kCtrlUnknown=0x0,
	kCtrlButton = 0x10,  //��ť
	kCtrlText,  //�ı�  0x11
	kCtrlProgress,  //������ 0x12
	kCtrlSlider,    //������ 0x13
	kCtrlMeter,  //�Ǳ�       0x14
	kCtrlDropList, //�����б�
	kCtrlAnimation, //����
	kCtrlRTC, //ʱ����ʾ
	kCtrlGraph, //����ͼ�ؼ�
	kCtrlTable, //���ؼ�
	kCtrlMenu,//�˵��ؼ�   0x1a
	kCtrlSelector,//ѡ��ؼ�
	kCtrlQRCode,//��ά��
};

#pragma pack(push)
#pragma pack(1)	//���ֽڶ���

typedef struct
{
	u8    cmd_head;  //֡ͷ

	u8    cmd_type;  //��������(UPDATE_CONTROL)	
	u8    ctrl_msg;   //CtrlMsgType-ָʾ��Ϣ������
	u16   screen_id;  //������Ϣ�Ļ���ID
	u16   control_id;  //������Ϣ�Ŀؼ�ID
	u8    control_type; //�ؼ�����

	u8    param[256];//�ɱ䳤�Ȳ��������256���ֽ�

	u8  cmd_tail[4];   //֡β
}CTRL_MSG,*PCTRL_MSG;

#pragma pack(pop)

void UpdateUI(void);

void ProcessMessage( PCTRL_MSG msg, u16 size );

void NotifyScreen(u16 screen_id);

void NotifyTouchXY(u8 press,u16 x,u16 y);

void NotifyButton(u16 screen_id, u16 control_id, u8  state);

void NotifyText(u16 screen_id, u16 control_id, u8 *str);

void NotifyProgress(u16 screen_id, u16 control_id, u32 value);

void NotifySlider(u16 screen_id, u16 control_id, u32 value);

void NotifyMeter(u16 screen_id, u16 control_id, u32 value);

void NotifyMenu(u16 screen_id, u16 control_id, u8  item, u8  state);

void NotifySelector(u16 screen_id, u16 control_id, u8  item);

void NotifyTimer(u16 screen_id, u16 control_id);

void NotifyReadFlash(u8 status,u8 *_data,u16 length);

void NotifyWriteFlash(u8 status);

void axisToAngle(float axis_x,float axis_y,float axis_z);
void axisToAngle_2(float axis2_x,float axis2_y,float axis2_z);

#endif
