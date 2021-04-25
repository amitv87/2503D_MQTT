#if defined(__AUDIO_RC__)
/******************************************************************************
 * track_cust_key.h - 
 * 
 * Copyright .
 * 
 * DESCRIPTION: - 
 *        GT300��Ƶ����
 * 
 * modification history
 * --------------------
 * v1.0   2013-07-5,  wangqi create this file
 * 
 ******************************************************************************/

#ifndef _TRACK_CUST_AUDIO_H
#define _TRACK_CUST_AUDIO_H

/****************************************************************************
* Include Files                   		����ͷ�ļ�
*****************************************************************************/


/*****************************************************************************
 *  Define					�궨��
 *****************************************************************************/


/***************************************************************************** 
* Typedef  Enum
*****************************************************************************/

typedef enum
{
    DEVICE_POWER_ON,
    DEVICE_POWER_OFF,
    MT_SMS,
    MT_RING,        //��������
    MT_CONNECT,     //
    MT_MONITOR,     //����
    MT_DISCONNECT,
    MT_MAX
}action_for_audio_enum;



/*****************************************************************************
 *  Struct					���ݽṹ����
 *****************************************************************************/
typedef struct
{
    kal_uint8                   audio_id;
    kal_uint8                   play_style;
    kal_uint64                  volume;
    kal_uint8                   output_path;
    kal_uint16                  identifier;
}track_drv_aud_play_id_struct;

typedef struct 
{
    const unsigned char *AudioNames;
    int                             AudioLength;
    kal_uint8                   AudioType;
} TRACK_AUDIO_ITEM;

typedef struct 
{
    unsigned short                      MaxAudioNumEXT;
    const TRACK_AUDIO_ITEM * Audio_item;
} TRACK_AUDIO_HEADER;


typedef struct 
{
    action_for_audio_enum ActionNames;
    int                             AudioIndex;
    kal_uint8                   AudioRunType;
} TRACK_AUDIO_SETTING;

#if defined (__LUYING__)
#define REC_BUFFER_LEN 30*1024
#else
#define REC_BUFFER_LEN 12*1024
#endif

typedef struct 
{
    kal_uint16 Record_Buffer[REC_BUFFER_LEN];//˫�ֽ�
    kal_uint32 recSize;//byte size
    kal_uint16 time;//¼�����ʱ��
    kal_uint8  packets;
    kal_uint8  revPacket;
}rec_data_struct;
/*****************************************************************************
 *  Global Functions			ȫ�ֱ���
 *****************************************************************************/

/*****************************************************************************
 *  Local Functions			���غ���
 *****************************************************************************/


/*****************************************************************************
 *  Global Functions			ȫ�ֺ���
 *****************************************************************************/
extern void track_cust_audio_play_voice(kal_uint8 index);
extern void track_cust_audio_stop_voice(void);
extern void track_cust_close_record(void);
#endif  /* _TRACK_CUST_AUDIO_H */
#endif /* __AUDIO_RC__ */
