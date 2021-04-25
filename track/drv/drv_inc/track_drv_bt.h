/******************************************************************************
 * track_drv_bt.h -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        BT��CSR1012������
 *
 * modification history
 * --------------------
 * v1.0   2016-08-30,  chengjun create this file
 *
 ******************************************************************************/
#if defined(__CUST_BT__)
/****************************************************************************
* Include Files                         ����ͷ�ļ�
*****************************************************************************/
#ifndef _TRACK_DRV_BT_H
#define _TRACK_DRV_BT_H

#include "stack_msgs.h"
#include "stack_ltlcom.h"
#include "drvsignals.h"
#include "kal_public_defs.h"

/*****************************************************************************
* Typedef  Enum                         ö�ٶ���
*****************************************************************************/
typedef enum
{
    BT_RESTORY_FACTORY_REQ,
    BT_VERSINO_REQ,
    BT_DATA_TC_REQ,//����͸��ָ���ָ��ͷ���Զ�������
    BT_ACTION_REQUEST_MAX,
} bt_action_request_enum;

/*****************************************************************************
 *  Struct                              ���ݽṹ����
 *****************************************************************************/
 #define BT_UART_BUFF_LEN_MAX 32
typedef struct
{
    kal_uint8 event;
    kal_uint8 data[BT_UART_BUFF_LEN_MAX];
} bt_msg_struct;

typedef struct
{
    LOCAL_PARA_HDR
    bt_msg_struct msg_body;
} bt_events_msg_struct;


/*****************************************************************************
 *  Define					�궨��
 *****************************************************************************/
#define BT_CMD_ERROR            (0x00)     //������
#define BT_POWON_INIT_EVENT     (0x01)     //�ϵ��ʼ�� 
#define BT_LISTEN_RECEIVE_DATA  (0x07)     //�������� 
#define BT_SET_CONCOX_PARAM     (0x54)     // ��������Concox�㲥����
#define BT_GET_CONCOX_PARAM     (0x55)     // ��ѯ����Concox�㲥����
#define BT_SET_SCAN_TIME        (0x56)     // ��������ɨ��ʱ��
#define BT_GET_SCAN_TIME        (0x57)     // ��ѯ����ɨ��ʱ��
#define BT_GET_RUN_STATUS       (0x58)     // ��ѯ��������״̬
#define BT_SET_WORK_TIME        (0x60)     // ������������ʱ��
#define BT_GET_WORK_TIME        (0x61)     // ��ѯ��������ʱ��
#define BT_SET_BROADCAST_PARAM  (0x62)     // ���ù㲥����
#define BT_GET_BROADCAST_PARAM  (0x63)     // ��ѯ�㲥����  broadcast
#define BT_SET_FACTORY          (0xFE)     // �ָ���������
#define BT_GET_SW_VERSION       (0xFF)     // ��ѯ�����汾��

/*****************************************************************************
* Local variable                        �ֲ�����
*****************************************************************************/

/****************************************************************************
* Global Variable                       ȫ�ֱ���
*****************************************************************************/


/****************************************************************************
* Global Variable - Extern              ����ȫ�ֱ���
*****************************************************************************/


/*****************************************************************************
 *  Global Functions    - Extern        �����ⲿ����
 *****************************************************************************/

/*****************************************************************************
 *  Local Functions                     ���غ���
 *****************************************************************************/


/*****************************************************************************
 *  Global Functions			ȫ�ֺ���
 *****************************************************************************/
extern void track_drv_set_bt_broadcast_param(kal_uint8*device_string, kal_uint8* imei_hex);

#endif  /* _TRACK_DRV_BT_H */

#endif  /*__CUST_BT__*/

