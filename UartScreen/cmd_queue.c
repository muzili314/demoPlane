#include "cmd_queue.h"

/*��Ϣ�Զ��е���ʽ�洢*/
typedef struct _QUEUE
{
	qsize _head;
	qsize _tail;
	qdata _data[QUEUE_MAX_SIZE];	
}QUEUE;

static QUEUE que = {0,0,0};
static qdata cmd_state = 0;       //�� ��
static qsize cmd_pos = 0;         //position

void queue_reset()
{
	que._head = que._tail = 0;
	cmd_pos = cmd_state = 0;
}

void queue_push(qdata _data)
{
	qsize pos = (que._head+1)%QUEUE_MAX_SIZE;
	if(pos!=que._tail)//����״̬
	{
		que._data[que._head] = _data;
		que._head = pos;
	}
}

static void queue_pop(qdata* _data)
{
	if(que._tail!=que._head)//�ǿ�״̬
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
 *  \brief  ��ȡ�����
 *  \param *buffer  ����
 *  \param qsize buf_len ����ȡ��Ϣ����
 *  \return qsize ��Ϣ��С
 */
qsize queue_find_cmd(qdata *buffer,qsize buf_len)
{
	qsize cmd_size = 0;
	qdata _data = 0;
	while(queue_size()>0)
	{
		//ȡһ������
		queue_pop(&_data);

		if(cmd_pos==0&&_data!=0xEE)//֡ͷ��������
		    continue;

		if(cmd_pos<buf_len)//��ֹ���������
			buffer[cmd_pos++] = _data;

		//�ж�֡β
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

		//�õ�������֡β
		if(cmd_state==4)
		{
			cmd_size = cmd_pos;
			cmd_state = 0;
			cmd_pos = 0;

#if(CRC16_ENABLE)
			//ȥ��ָ��ͷβEE��βFFFCFFFF����5���ֽڣ�ֻ�������ݲ���CRC
			if(!CheckCRC16(buffer+1,cmd_size-5))//CRCУ��
				return 0;

			cmd_size -= 2;//ȥ��CRC16
#endif

			return cmd_size;
		}
	}

	return 0;//û���γ�������һ֡
}
