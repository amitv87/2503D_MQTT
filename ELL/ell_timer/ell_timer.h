/******************************************************************************
 * track_os_timer.h -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        �Զ��嶨ʱ��
 *
 * modification history
 * --------------------
 * v1.0   2012-07-15,  chengjun create this file
 *
 ******************************************************************************/
#ifndef _ELL_TIMER_H
#define _ELL_TIMER_H

/****************************************************************************
* Include Files             ����ͷ�ļ�
*****************************************************************************/
#include "kal_public_defs.h"

/*****************************************************************************
 *  Define			    �궨��
*****************************************************************************/


/*****************************************************************************
* Typedef  Enum         ö��
*****************************************************************************/
typedef enum
{
/*DRV begin*/
    ELL_WATCH_DOG_TIMER_ID,
/*DRV end*/
    
/*NET begin*/
    ELL_RESTART_TIMER_ID,
    ELL_20MIN_RESTART_TIMER_ID,
    ELL_SOCKET_SEND_TIMER_ID,
    ELL_SOC_TIMEOUT_TIMER_ID,
    ELL_SOC_CONN_TIMEOUT_TIMER_ID,
    ELL_SOC_RECONNECT_TIMER_ID,
    ELL_SOC_SEND_TIMER_ID,
    ELL_SOC_SEND_OVER_TIMER_ID,
    ELL_SERVER_OVERTIME_TIMER_ID,
    ELL_SERVER_SEND_REQ_OVERTIME_TIMER_ID,
    ELL_SEND_DATA_OVERTIME_TIMER_ID,
/*NET end*/

    /* Imp: Please do not modify this */
    MAX_NUM_OF_ELL_TIMER,
} ell_timer_enum;


/*****************************************************************************
 *  Struct			    ���ݽṹ����
*****************************************************************************/
typedef void (*FuncPtr) (void);
typedef void (*Func_Ptr)(void *);

/*****************************************************************************
* Local variable            �ֲ�����
*****************************************************************************/

/****************************************************************************
* Global Variable           ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
 *  Local Functions	            ���غ���
*****************************************************************************/

/*****************************************************************************
 *  Global Functions            ȫ�ֺ���
*****************************************************************************/
/*
void track_os_intoTaskMsgQueue(kal_hisr_func_ptr app);
void track_os_intoTaskMsgQueueExt(kal_timer_func_ptr app, void *arg);
void track_os_task_expiry(void *local_para_ptr);*/
#endif  /* _ELL_TIMER_H */
