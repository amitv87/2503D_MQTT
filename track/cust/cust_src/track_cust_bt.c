/******************************************************************************
 * track_cust_bt.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        BT��CSR1012��Ӧ��
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
#include "stack_msgs.h"
#include "stack_ltlcom.h"
#include "drvsignals.h"
#include "track_drv_bt.h"
#include "track_os_ell.h"
#include "track_os_paramete.h"

/*****************************************************************************
* Typedef  Enum                         ö�ٶ���
*****************************************************************************/

/*****************************************************************************
 *  Struct                              ���ݽṹ����
 *****************************************************************************/


/*****************************************************************************
 *  Define					�궨��
 *****************************************************************************/

/*****************************************************************************
* Local variable                        �ֲ�����
*****************************************************************************/

/****************************************************************************
* Global Variable                       ȫ�ֱ���
*****************************************************************************/


/****************************************************************************
* Global Variable - Extern              ����ȫ�ֱ���
*****************************************************************************/
extern nvram_parameter_struct G_parameter;

/*****************************************************************************
 *  Global Functions    - Extern        �����ⲿ����
 *****************************************************************************/

/*****************************************************************************
 *  Local Functions                     ���غ���
 *****************************************************************************/


/*****************************************************************************
 *  Global Functions			ȫ�ֺ���
 *****************************************************************************/
void track_cust_bt_init(void)
{
    if(G_parameter.bt.sw != 0)
    {
        track_drv_bt_switch(1);
        track_drv_set_bt_work_time(G_parameter.bt.broadcast_time,G_parameter.bt.listen_time,G_parameter.bt.sleep_time);
    }
    else
    {
        track_drv_bt_switch(0);
    }
}

/******************************************************************************
 *  Function    -  track_cust_test_bt_event_handle
 *
 *  Purpose     -  ����ģʽ��־���
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-09-02,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_test_bt_event_handle(kal_uint8 bt_event)
{
    switch(bt_event)
    {
        case BT_POWON_INIT_EVENT:
            break;
        case BT_LISTEN_RECEIVE_DATA:
            break;
        case BT_SET_CONCOX_PARAM:
            break;
        case BT_GET_CONCOX_PARAM:
            break;
        case BT_SET_SCAN_TIME:
            break;
        case BT_GET_SCAN_TIME:
            break;
        case BT_GET_RUN_STATUS:
            break;
        case BT_SET_WORK_TIME:
            break;
        case BT_GET_WORK_TIME:
            break;
        case BT_SET_BROADCAST_PARAM:
            break;
        case BT_GET_BROADCAST_PARAM:
            break;
        case  BT_SET_FACTORY:
            break;
        case BT_GET_SW_VERSION:
            break;
        default:
            break;
    }

}


/******************************************************************************
 *  Function    -  track_cust_bt_event_handle
 *
 *  Purpose     -  Ӧ�ò㴦������ģ����Ϣ
 *
 *  Description -  ������Ϣ������ֱ�Ӵ�����δ�ϱ���track_drv_bt_rec_data
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-09-02,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_bt_event_handle(void *msg)
{
    kal_uint8 bt_event;
    kal_uint8 *data;
    bt_events_msg_struct* bt_events_msg = (bt_events_msg_struct*)msg;

    bt_event = bt_events_msg->msg_body.event;
    data = bt_events_msg->msg_body.data;

    LOGD(L_APP, L_V5, "event:%d", bt_event);

    if(track_is_testmode())
    {
        track_cust_test_bt_event_handle(bt_event);
    }

    switch(bt_event)
    {
        case BT_POWON_INIT_EVENT:
            break;
        case BT_LISTEN_RECEIVE_DATA:
            {
                kal_uint8 title[7] = {0}, imei[17] = {0};
                kal_uint8 alarm;
                kal_int8 power_tx, power_rx;

                memcpy(title, data, 6);
                sprintf(imei, "%02X%02X%02X%02X%02X%02X%02X%02X", \
                            *(data + 6), *(data + 7), *(data + 8), *(data + 9), *(data + 10), *(data + 11), *(data + 12), *(data + 13));
                alarm = *(data + 14);
                power_tx = *(data + 15);
                power_rx = *(data + 16);
                LOGD(L_APP, L_V2, "BT�������� %s,%s,alarm=0x%02X,TX=%d,RX=%d", title, imei, alarm, power_tx, power_rx);
            }
            break;
        case BT_SET_CONCOX_PARAM:
            break;
        case BT_GET_CONCOX_PARAM:
            break;
        case BT_SET_SCAN_TIME:
            break;
        case BT_GET_SCAN_TIME:
            break;
        case BT_GET_RUN_STATUS:
            break;
        case BT_SET_WORK_TIME:
            break;
        case BT_GET_WORK_TIME:
            break;
        case BT_SET_BROADCAST_PARAM:
            break;
        case BT_GET_BROADCAST_PARAM:
            break;
        case  BT_SET_FACTORY:
            break;
        case BT_GET_SW_VERSION:
            break;
        default:
            break;
    }

}


#endif  /*__CUST_BT__*/



