/******************************************************************************
 * track_drv_bt.c -
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
#include "track_drv_bt.h"
#include "track_os_log.h"
#include "track_os_ell.h"
#include "track_drv_system_param.h"

/*****************************************************************************
* Typedef  Enum                         ö�ٶ���
*****************************************************************************/

/*****************************************************************************
 *  Struct                              ���ݽṹ����
 *****************************************************************************/


/*****************************************************************************
 *  Define					�궨��
 *****************************************************************************/
#define CUST_BT_POWER_ENABLE_GPIO 20
#define CUST_BT_WAKEUP_GPIO  15

//��ͬ���ܵ�ָ��ͷ
#define HEAD0_SEND_TO_BT            (0x47)
#define HEAD1_SEND_TO_BT            (0x54)
#define HEAD0_REPLY_FROM_BT         (0x42)
#define HEAD1_REPLY_FROM_BT         (0x54)


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
static void  delay_1ms(kal_uint16 ms)
{
    volatile kal_uint16 i;
    while(ms--)
    {
        for(i = 0; i < 2048; i++)
        {
            ;
        }
    }
}

/*****************************************************************************
 *  Global Functions			ȫ�ֺ���
 *****************************************************************************/
void track_drv_bt_and_gsm_sleep(void)
{
    track_drv_sleep_enable(SLEEP_EXT_UART_MOD, KAL_TRUE);
    GPIO_ModeSetup(0, 0);
    GPIO_ModeSetup(1, 0);
    GPIO_InitIO(0,0);
    GPIO_WriteIO(0, 1); //TX3����ߵ���Ƭ��©��    
}

void track_drv_bt_wakeup_gsm(void)
{
    track_drv_sleep_enable(SLEEP_EXT_UART_MOD, KAL_FALSE);
    GPIO_ModeSetup(0, 3);
    GPIO_ModeSetup(1, 3);
    tr_start_timer(TRACK_DRV_BT_WAKEUP_GSM_TIMER_ID,4*1000,track_drv_bt_and_gsm_sleep);
}

/******************************************************************************
 *  Function    -  track_drv_bt_setting
 * 
 *  Purpose     -  ��BT����ָ��
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-09-03,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_bt_setting(bt_action_request_enum req, kal_uint8*cmd, kal_uint8 len)
{
    kal_uint8 buff[BT_UART_BUFF_LEN_MAX] = {0};
    kal_uint8 req_len = 0;

    if((req >= BT_ACTION_REQUEST_MAX) || (cmd == NULL) || (len >= (BT_UART_BUFF_LEN_MAX - 8)))
    {
        LOGD(L_DRV, L_V5, "BT REQ error");
        return;
    }

    buff[0] = HEAD0_SEND_TO_BT;
    buff[1] = HEAD1_SEND_TO_BT;

    switch(req)
    {
        case BT_RESTORY_FACTORY_REQ:
            buff[2] = BT_SET_FACTORY;
            req_len = 0;
            break;
        case BT_VERSINO_REQ:
            buff[2] = BT_GET_SW_VERSION;
            req_len = 0;
            break;
        case BT_DATA_TC_REQ:
            memcpy(&buff[2], cmd, len);
            req_len = len - 2;
            break;

        default:
            LOGD(L_DRV, L_V5, "Can not support %d", req);
            return;
    }

    buff[3] = req_len;

    LOGH(L_DRV, L_V5, buff, req_len + 4);
    track_drv_write_exmode(buff, req_len + 4);

}

/******************************************************************************
 *  Function    -  track_drv_send_bt_event
 *
 *  Purpose     -  ��������ģ����Ϣ����Ϣ֪ͨӦ�ò�
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-08-30,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_send_bt_event(bt_msg_struct *bt_msg_body)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct *ilm_ptr;
    bt_events_msg_struct *bt_msg = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_msg = (bt_events_msg_struct*) construct_local_para(sizeof(bt_events_msg_struct), TD_CTRL);
    bt_msg->msg_body.event = bt_msg_body->event;
    memcpy(bt_msg->msg_body.data, bt_msg_body->data, BT_UART_BUFF_LEN_MAX);

    ilm_ptr = (ilm_struct *)allocate_ilm(MOD_MMI);
    ilm_ptr->src_mod_id = MOD_MMI;
    ilm_ptr->dest_mod_id = MOD_MMI;
    ilm_ptr->sap_id = INVALID_SAP;
    ilm_ptr->msg_id = (msg_type) MSG_ID_BT_EVENTS_ID;
    ilm_ptr->local_para_ptr = (local_para_struct*) bt_msg;
    ilm_ptr->peer_buff_ptr = NULL;
    msg_send_ext_queue(ilm_ptr);
}


/******************************************************************************
 *  Function    -  track_drv_bt_rec_data
 *
 *  Purpose     -  UART��������ģ����Ϣ����
 *
 *  Description -  Э��ȽϿӵ���ÿ����Ŀ����ͬ
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-08-30,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_bt_rec_data(kal_uint8* data, kal_uint16 len)
{
    kal_uint8 cmd;
    kal_uint8* p_buff;
    static kal_uint8 first=1;
    bt_msg_struct bt_msg_body = {0};

    if(data == NULL)
    {
        return;
    }

    //if(track_is_testmode() || track_is_autotestmode())
    LOGH(L_DRV, L_V4, data, len);

    if((len >= BT_UART_BUFF_LEN_MAX) || (len <= 3))
    {
        return;
    }

    bt_msg_body.event = BT_CMD_ERROR;

    if((*data == HEAD0_REPLY_FROM_BT) && (*(data + 1) == HEAD1_REPLY_FROM_BT))
    {
        cmd = (*(data + 2));
        p_buff = data + 4;  //��Ч���ݶ�

        switch(cmd)
        {
            case BT_POWON_INIT_EVENT:
                LOGD(L_DRV, L_V5, "BT POWON EVENT");
                bt_msg_body.event = cmd;
                break;
            case BT_LISTEN_RECEIVE_DATA:
                memcpy(bt_msg_body.data, p_buff, *(data + 3));
                bt_msg_body.event = cmd;
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
                LOGS("BT restory factory OK");
                break;
            case BT_GET_SW_VERSION:
                LOGS("BT version v%d.%d", *p_buff, *(p_buff + 1));
                break;
            default:
                LOGD(L_DRV, L_V3, "BT UART3 cmd error");
                break;
        }
    }
    else
    {
        LOGD(L_DRV, L_V3, "BT UART3 receive data error");
    }


    if(first==1)
    {
        first=0;
        track_drv_set_bt_broadcast_param("GV22", track_drv_get_imei(1));
    }
    
    if(bt_msg_body.event != BT_CMD_ERROR)
    {
        track_drv_send_bt_event(&bt_msg_body);
    }

}

/******************************************************************************
 *  Function    -  track_drv_bt_switch 
 * 
 *  Purpose     -  BTģ�鹩��ʹ��
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-09-03,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint8 track_drv_bt_switch(kal_uint8 sw)
{
    static kal_bool first = KAL_TRUE;

    if(first)
    {
        first = KAL_FALSE;
        GPIO_ModeSetup(CUST_BT_POWER_ENABLE_GPIO, 0);
        GPIO_InitIO(1, CUST_BT_POWER_ENABLE_GPIO);
    }

    if(sw == 0)
    {
        GPIO_WriteIO(0, CUST_BT_POWER_ENABLE_GPIO);

        //UART3  TX3���0
        GPIO_ModeSetup(0, 0);
        GPIO_ModeSetup(1, 0);
        GPIO_InitIO(0,0);
        GPIO_WriteIO(0, 1); //TX3����ߵ���Ƭ��©��
    
    }
    else
    {
        GPIO_ModeSetup(0, 3);
        GPIO_ModeSetup(1, 3);
        GPIO_WriteIO(1, CUST_BT_POWER_ENABLE_GPIO);
        tr_start_timer(TRACK_DRV_BT_WAKEUP_GSM_TIMER_ID,60*1000,track_drv_bt_and_gsm_sleep);
    }
}


/******************************************************************************
 *  Function    -  track_drv_bt_wakeup
 * 
 *  Purpose     -  �жϻ���BTģ��
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-09-03,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_bt_wakeup(kal_uint8 wakeup)
{
    static kal_bool first = KAL_TRUE;
    LOGD(L_DRV, L_V5,  "wakeup=%d", wakeup);

    if(CUST_BT_WAKEUP_GPIO == 0xFF)
    {
        return;
    }

    if(first)
    {
        first = KAL_FALSE;
        GPIO_ModeSetup(CUST_BT_WAKEUP_GPIO, 0);
        GPIO_InitIO(1, CUST_BT_WAKEUP_GPIO);
    }

    if(wakeup == 1)
    {
        track_drv_bt_wakeup_gsm();
        
        track_drv_gpio_set(0, CUST_BT_WAKEUP_GPIO);
        delay_1ms(2);
        track_drv_gpio_set(1, CUST_BT_WAKEUP_GPIO);
        delay_1ms(10);
        track_drv_gpio_set(0, CUST_BT_WAKEUP_GPIO);
        delay_1ms(3);
    }
}

/******************************************************************************
 *  Function    -  track_drv_set_bt_work_time
 * 
 *  Purpose     -  ������������ʱ��
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-09-03,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_set_bt_work_time(kal_uint8 broadcast, kal_uint8 listen, kal_uint16 sleep)
{
    kal_uint8 buff[7];

    buff[0] = BT_SET_WORK_TIME;
    buff[1] = 0x05;
    buff[2] = 0x05; //OTA�����㲥ʱ��
    buff[3] = broadcast;
    buff[4] = listen;
    buff[5] = (sleep >> 8) & 0xFF;
    buff[6] = sleep & 0xFF;
    track_drv_bt_setting(BT_DATA_TC_REQ,buff,7);
}

/******************************************************************************
 *  Function    -  track_drv_set_bt_broadcast_param
 * 
 *  Purpose     -  ���������㲥����
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-09-03,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_set_bt_broadcast_param(kal_uint8*device_string, kal_uint8* imei_hex)
{
    kal_uint8 buff[16]={0};
    kal_uint8 len;
    
    buff[0] = BT_SET_BROADCAST_PARAM;
    buff[1] = 0x0E;

    len = strlen(device_string);
    if(len>6)
    {
        len=6;
    }    
    memcpy(&buff[2], device_string, len);
    memcpy(&buff[8], imei_hex, 8);    

    track_drv_bt_setting(BT_DATA_TC_REQ,buff,16);
}

#endif  /*__CUST_BT__*/


