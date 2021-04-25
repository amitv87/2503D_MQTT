/*******************************************************************************************
 * Filename:
 * Author :
 * Date :
 * Comment:
 ******************************************************************************************/

/****************************************************************************
*  Include Files                        ����ͷ�ļ�
*****************************************************************************/
#include "track_cust.h"
//#include "Conversions.h"

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
//Your Vehicle's Speed: 50km/h, Please Check!

static kal_uint8 str_speed_limit_cn[48] =   //�������٣��ٶ� KM/H,��������У���ȫ��ʻ��
{
    0x8F, 0x66, 0x8F, 0x86, 0x8D, 0x85, 0x90, 0x1F, 0xFF, 0x01, 0x90, 0x1F, 0x5E, 0xA6, 0x00, 0x20,
    0x00, 0x4B, 0x00, 0x4D, 0x00, 0x2F, 0x00, 0x48, 0x00, 0x2C, 0x8B, 0xF7, 0x51, 0xCF, 0x90, 0x1F,
    0x61, 0x62, 0x88, 0x4C, 0xFF, 0x0C, 0x5B, 0x89, 0x51, 0x68, 0x9A, 0x7E, 0x9A, 0x76, 0xFF, 0x01
};
//�����ѳ���(68km/h)����������У���ȫ��ʻ��
static kal_uint8 str_2_1[12] =
{
    0x8F, 0x66, 0x8F, 0x86, 0x5D, 0xF2, 0x8D, 0x85, 0x90, 0x1F, 0x00, 0x28
};
static kal_uint8 str_2_2[34] =
{
    0x00, 0x6B, 0x00, 0x6D, 0x00, 0x2F, 0x00, 0x68, 0x00, 0x29, 0xFF, 0x0C, 0x8B, 0xF7, 0x51, 0xCF,
    0x90, 0x1F, 0x61, 0x62, 0x88, 0x4C, 0xFF, 0x0C, 0x5B, 0x89, 0x51, 0x68, 0x9A, 0x7E, 0x9A, 0x76,
    0xFF, 0x01
};
static track_gps_data_struct gps_data_tmp = {0};
static float g_max_speed = 0;
static kal_uint8 speed_limit_status = 0;
/****************************************************************************
*  Global Variable                      ȫ�ֱ���
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/

/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/

#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/

/******************************************************************************
 *  Function    -  speed_limit_overTime
 *
 *  Purpose     -  ���ͳ��ٱ�������
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void speed_limit_overTime(void *arg)
{
    LOGD(L_APP, L_V5, "");
    if(!G_parameter.speed_limit_alarm.alarm_type || G_parameter.speed_limit_alarm.alarm_type == 3)
    {
        LOGD(L_APP, L_V4, "���ٱ������������������������֪ͨ��");
    }
    else
    {
        struct_msg_send g_msg = {0};
        char tmp[100] = {0};
        int len;
        kal_uint32 speed = (kal_uint32)arg;
        g_msg.addUrl = KAL_FALSE;
        g_msg.cm_type = CM_SMS;
#if defined(__GERMANY__)
       snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Geschwindigkeit zu hoch!");
#else
        snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Speeding! The speed of %dKM/H, please slow down, drive safely!", speed);
#endif
        snprintf(tmp, 50, "%d", speed);
        memcpy(g_msg.msg_cn, str_speed_limit_cn, 16);
        len = track_fun_asc_str_to_ucs2_str(&g_msg.msg_cn[16], tmp) - 2;
        g_msg.msg_cn_len = len + 48;
        if(CM_PARAM_LONG_LEN_MAX > g_msg.msg_cn_len)
        {
            memcpy(&g_msg.msg_cn[16+len], &str_speed_limit_cn[16], 32);
            track_cust_module_alarm_msg_send(&g_msg, NULL);
        }
    }
    if(G_parameter.speed_limit_alarm.alarm_type  == 2 || G_parameter.speed_limit_alarm.alarm_type  == 3)
    {
        track_start_timer(TRACK_CUST_ALARM_LowBattery_Call_TIMER, 60000, track_cust_make_call, (void*)1);
    }
#if defined (__XYBB__)
#else
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_Speed_Limit);
#endif /* __XYBB__ */
}

/******************************************************************************
 *  Function    -  speed_over_ind
 *
 *  Purpose     -  ȷ���������ٱ���
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void speed_over_ind(void)
{
    kal_uint32 sp =  g_max_speed;
    LOGD(L_APP, L_V5, "%dʱ�䵽�����ٱ���", kal_ticks_to_secs(kal_get_systicks()));
    g_max_speed = 0;
    speed_limit_status = 2;
    if(track_nvram_alone_parameter_read()->gprson)
    {
        /*���ٱ���ֻ��֪ͨƽ̨������ȴ�ƽ̨���صĵ�ַ*/
#if defined(__XCWS__)
#elif defined(__XYBB__)
		track_cust_alarm_type(TR_CUST_ALARM_Speed_Limit);
		track_cust_paket_0200(&gps_data_tmp,track_drv_get_lbs_data());
#else
        track_cust_paket_16(&gps_data_tmp, track_drv_get_lbs_data(), TR_CUST_ALARM_Speed_Limit, KAL_FALSE, 0);
#endif
    }
    speed_limit_overTime((void*)sp);
}

/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/

/******************************************************************************
 *  Function    -  track_cust_module_speed_limit
 *
 *  Purpose     -  �ж��Ƿ��٣�
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_speed_limit(track_gps_data_struct *gps_data)
{
    if(G_parameter.speed_limit_alarm.sw == 0 || track_cust_get_work_mode() == WORK_MODE_3)
    {
        return;
    }

    if(G_parameter.speed_limit_alarm.sw)
    {
        if(gps_data->status >= 2 && gps_data->gprmc.Speed >= G_parameter.speed_limit_alarm.threshold)
        {
            if(speed_limit_status != 2/* && g_max_speed < gps_data->gprmc.Speed*/)
            {
                g_max_speed = gps_data->gprmc.Speed;
                memcpy(&gps_data_tmp, gps_data, sizeof(track_gps_data_struct));
            }
            if(speed_limit_status == 0 && !track_is_timer_run(TRACK_CUST_ALARM_speed_limit_OVERTIME_TIMER))
            {
                LOGD(L_APP, L_V5, "%d,%d�ѳ��٣���ʱ��ʼ", kal_ticks_to_secs(kal_get_systicks()), G_parameter.speed_limit_alarm.delay_time);
                speed_limit_status = 1;
                tr_start_timer(
                    TRACK_CUST_ALARM_speed_limit_OVERTIME_TIMER,
                    G_parameter.speed_limit_alarm.delay_time * 1000,
                    speed_over_ind);
            }
        }
        else
        {
            speed_limit_status = 0;
            if(track_is_timer_run(TRACK_CUST_ALARM_speed_limit_OVERTIME_TIMER))
            {
                LOGD(L_APP, L_V5, "�ٶȻָ���������ʱ��ֹ��ʼ");
                track_stop_timer(TRACK_CUST_ALARM_speed_limit_OVERTIME_TIMER);
                g_max_speed = 0;
            }
        }
    }
}

/******************************************************************************
 *  Function    -  track_cust_module_speed_limit_cmd_update
 *
 *  Purpose     -  ָ����³��ٱ���״̬
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_speed_limit_cmd_update(void)
{
    speed_limit_status = 0;
    track_stop_timer(TRACK_CUST_ALARM_speed_limit_OVERTIME_TIMER);
    g_max_speed = 0;
}

