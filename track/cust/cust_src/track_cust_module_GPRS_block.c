/*******************************************************************************************
 * Filename:                                                                              *
 * Author  :                                                                              *
 * Date    :                                                                              *
 * Comment :                                                                              *
*******************************************************************************************/

/****************************************************************************
*  Include Files                        ����ͷ�ļ�
*****************************************************************************/
#include "track_cust.h"

/*****************************************************************************
*  Define                               �궨��
******************************************************************************/

/*****************************************************************************
*  Typedef  Enum                        ö�ٶ���
*****************************************************************************/

/*****************************************************************************
*  Struct                               ���ݽṹ����
******************************************************************************/

/*****************************************************************************
*  Local variable                       �ֲ�����
*****************************************************************************/
#ifndef __GW100__
#if defined(__GERMANY__)
static char *str_gprs_block_en = "Achtung der GPS Tracker ist offline!";
#else
static char *str_gprs_block_en = "Attention!The device is offline!";
#endif
static kal_uint8 str_gprs_block_cn[28] =   //ע�⣡�ն�GPRS�����ߣ�
{
    0x6C, 0xE8, 0x61, 0x0F, 0xFF, 0x01, 0x7E, 0xC8, 0x7A, 0xEF, 0x00, 0x47,
    0x00, 0x50, 0x00, 0x52, 0x00, 0x53, 0x5D, 0xF2, 0x79, 0xBB, 0x7E, 0xBF, 0xFF, 0x01, 0x00, 0x00
};
#else
static char *str_gprs_block_en = "Attention! The monitoring phone is offline!";
static kal_uint8 str_gprs_block_cn[34] =   //��ע�⣡�໤�ֻ�GPRS�����ߣ� 
{
0x8B ,0xF7 ,0x6C ,0xE8 ,0x61 ,0x0F ,0xFF ,0x01 ,0x76 ,0xD1 ,0x62 ,0xA4 ,0x62 ,0x4B ,0x67 ,0x3A ,0x00  ,0x47 ,
    0x00 ,0x50 ,0x00 ,0x52 ,0x00 ,0x53 ,0x5D ,0xF2 ,0x79 ,0xBB ,0x7E ,0xBF ,0xFF ,0x01 , 0x00, 0x00
};
#endif /* __GW100__ */

/******************************************************************************
*  Global Variable                      ȫ�ֱ���
******************************************************************************/

/******************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
******************************************************************************/

/******************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/

/******************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/

#if 0
#endif /* 0 */
/******************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/

/******************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/

/******************************************************************************
 *  Function    -  track_cust_module_alarm
 *
 *  Purpose     -  GPRS������������
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
******************************************************************************/
void track_cust_module_alarm(kal_uint32 *count)
{
    static kal_uint8  flag = 3;
    LOGD(L_APP, L_V5, "count:%d, sw:%d, set:%d, flag:%d %d", *count, G_parameter.gprs_obstruction_alarm.sw, 
		G_parameter.gprs_obstruction_alarm.link, flag, track_cust_get_work_mode());
	if(track_cust_get_work_mode() == WORK_MODE_3)
	{
		return;
	}	

    if(flag == 3)
    {
        flag = G_realtime_data.reconnect_send_flg;
        if(G_realtime_data.reconnect_send_flg == 0)
        {
            *count = G_parameter.gprs_obstruction_alarm.link+1;
        }
    }
    if(!G_parameter.gprs_obstruction_alarm.sw)
    {
        return;
    }

    if(G_parameter.gprs_obstruction_alarm.link < *count && flag)
    {
        struct_msg_send g_msg = {0};
        flag = 0;
        g_msg.addUrl = KAL_FALSE;
        g_msg.cm_type = CM_SMS;
        snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_gprs_block_en);
#ifndef __GW100__
        g_msg.msg_cn_len = 28;
#else
        g_msg.msg_cn_len = 34;
#endif /* __GW100__ */
        memcpy(g_msg.msg_cn, str_gprs_block_cn, g_msg.msg_cn_len);
        if(!track_cust_module_alarm_msg_send(&g_msg, NULL))
        {
            flag = 1;
        }
    }
    else if(G_parameter.gprs_obstruction_alarm.link >= *count)
    {
        flag = 1;
    }
    if(G_realtime_data.reconnect_send_flg != flag)
    {
        G_realtime_data.reconnect_send_flg = flag;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    }
}

