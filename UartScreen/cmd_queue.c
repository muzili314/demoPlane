#include "cmd_queue.h"

/*消息以队列的形式存储*/
typedef struct _QUEUE
{
	qsize _head;
	qsize _tail;
	qdata _data[QUEUE_MAX_SIZE];	
}QUEUE;

static QUEUE que = {0,0,0};
static qdata cmd_state = 0;       //满 空
static qsize cmd_pos = 0;         //position

void queue_reset()
{
	que._head = que._tail = 0;
	cmd_pos = cmd_state = 0;
}

void queue_push(qdata _data)
{
	qsize pos = (que._head+1)%QUEUE_MAX_SIZE;
	if(pos!=que._tail)//非满状态
	{
		que._data[que._head] = _data;
		que._head = pos;
	}
}

static void queue_pop(qdata* _data)
{
	if(que._tail!=que._head)//非空状态
	{
		*_data = que._data[que._tail];
		que._tail = (que._tail+1)%QUEUE_MAX_SIZE;
	}
}

static qsize queue_size()
{
	return ((que._head+QUEUE_MAX_SIZE-que._tail)%QUEUE_MAX_SIZE);
}

/*! 
 *  \brief  获取命令长度
 *  \param *buffer  命令
 *  \param qsize buf_len 待获取消息长度
 *  \return qsize 消息大小
 */
qsize queue_find_cmd(qdata *buffer,qsize buf_len)
{
	qsize cmd_size = 0;
	qdata _data = 0;
	while(queue_size()>0)
	{
		//取一个数据
		queue_pop(&_data);

		if(cmd_pos==0&&_data!=0xEE)//帧头出错，跳过
		    continue;

		if(cmd_pos<buf_len)//防止缓冲区溢出
			buffer[cmd_pos++] = _data;

		//判断帧尾
		if(_data==0xFF)
		{
			switch (cmd_state)
			{			
			case 2:cmd_state = 3;break; //FF FC FF ??
			case 3:cmd_state = 4;break; //FF FC FF FF
			default:cmd_state = 1;break; //FF ?? ?? ??
			}
		}
		else if(_data==0xFC)
		{
			switch (cmd_state)
			{
			case 1:cmd_state = 2;break; //FF FC ?? ??
			case 3:cmd_state = 2;break; //FF FC FF FC
			default:cmd_state = 0;break; //?? ?? ?? ??
			}
		}
		else
			cmd_state = 0;

		//得到完整的帧尾
		if(cmd_state==4)
		{
			cmd_size = cmd_pos;
			cmd_state = 0;
			cmd_pos = 0;

#if(CRC16_ENABLE)
			//去掉指令头尾EE，尾FFFCFFFF共计5个字节，只计算数据部分CRC
			if(!CheckCRC16(buffer+1,cmd_size-5))//CRC校验
				return 0;

			cmd_size -= 2;//去掉CRC16
#endif

			return cmd_size;
		}
	}

	return 0;//没有形成完整的一帧
}
