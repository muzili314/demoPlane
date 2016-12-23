#ifndef _CMD_PROCESS_H
#define _CMD_PROCESS_H
#include "hmi_driver.h"

extern u8 MODEL;

extern u8 cmd_buffer[CMD_MAX_SIZE];
extern u8 update_en;



#define NOTIFY_TOUCH_PRESS      0X01   //触摸屏按下通知
#define NOTIFY_TOUCH_RELEASE  0X03  //触摸屏松开通知
#define NOTIFY_WRITE_FLASH_OK  0X0C  //写FLASH成功
#define NOTIFY_WRITE_FLASH_FAILD  0X0D  //写FLASH失败
#define NOTIFY_READ_FLASH_OK  0X0B  //读FLASH成功
#define NOTIFY_READ_FLASH_FAILD  0X0F  //读FLASH失败
#define NOTIFY_MENU                        0X14  //菜单事件通知
#define NOTIFY_TIMER                       0X43  //定时器超时通知
#define NOTIFY_CONTROL                0XB1  //控件更新通知
#define NOTIFY_SCREEN                   0XB2  //画面切换通知

#define PTR2U16(PTR) ((((u8 *)(PTR))[0]<<8)|((u8 *)(PTR))[1])  //从缓冲区取16位数据
#define PTR2U32(PTR) ((((u8 *)(PTR))[0]<<24)|(((u8 *)(PTR))[1]<<16)|(((u8 *)(PTR))[2]<<8)|((u8 *)(PTR))[3])  //从缓冲区取32位数据

enum CtrlType
{
	kCtrlUnknown=0x0,
	kCtrlButton = 0x10,  //按钮
	kCtrlText,  //文本  0x11
	kCtrlProgress,  //进度条 0x12
	kCtrlSlider,    //滑动条 0x13
	kCtrlMeter,  //仪表       0x14
	kCtrlDropList, //下拉列表
	kCtrlAnimation, //动画
	kCtrlRTC, //时间显示
	kCtrlGraph, //曲线图控件
	kCtrlTable, //表格控件
	kCtrlMenu,//菜单控件   0x1a
	kCtrlSelector,//选择控件
	kCtrlQRCode,//二维码
};

#pragma pack(push)
#pragma pack(1)	//按字节对齐

typedef struct
{
	u8    cmd_head;  //帧头

	u8    cmd_type;  //命令类型(UPDATE_CONTROL)	
	u8    ctrl_msg;   //CtrlMsgType-指示消息的类型
	u16   screen_id;  //产生消息的画面ID
	u16   control_id;  //产生消息的控件ID
	u8    control_type; //控件类型

	u8    param[256];//可变长度参数，最多256个字节

	u8  cmd_tail[4];   //帧尾
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
