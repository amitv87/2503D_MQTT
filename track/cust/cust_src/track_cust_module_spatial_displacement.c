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
#if defined(__GERMANY__)
static char *str_displacement_en = "Bewegungsalarm am Fahrzeug! ";
#else
static char *str_displacement_en = "Attention! The vehicle is moved.";
#endif
static kal_uint8 str_displacement_cn[20] =   // �������ƶ������ע��
{
    0x8F, 0x66, 0x8F, 0x86, 0x88, 0xAB, 0x79, 0xFB, 0x52, 0xA8, 0xFF, 0x0C, 0x8B, 0xF7, 0x51, 0x73,
    0x6C, 0xE8, 0xFF, 0x01
};

static char *str_displacement_set_en = "No GPS data, The displacement alarm setting unsuccessful!";
static kal_uint8 str_displacement_set_cn[40] =   // GPSδ����λ������λ�Ʊ������ò��ɹ���
{
    0x00, 0x47, 0x00, 0x50, 0x00, 0x53, 0x67, 0x2A, 0x5B, 0x9A, 0x8F, 0xC7, 0x4F, 0x4D, 0xFF, 0x0C,
    0x67, 0x2C, 0x6B, 0x21, 0x4F, 0x4D, 0x79, 0xFB, 0x62, 0xA5, 0x8B, 0x66, 0x8B, 0xBE, 0x7F, 0x6E,
    0x4E, 0x0D, 0x62, 0x10, 0x52, 0x9F, 0xFF, 0x01
};
static struct_msg_send g_msg = {0};
static track_gps_data_struct gps_data_buf = {0};
static float displacement_distance = 0;//��static���ƣ�g_distance��Ҳ���ܳ���ͬ����  chengj
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
void track_cust_module_sd_stop(void);
void track_cust_module_sd_check(track_gps_data_struct *gps_data);
void track_cust_module_sd_acc(void *arg);
#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/

#if defined (__NT33__)
/******************************************************************************
 *  Function    -  track_cust_moving_alarm_off
 * 
 *  Purpose     -  �յ�ָ���һ��λ�Ʊ��������������������رոù���
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-12-20,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_moving_alarm_off()
{
    G_parameter.displacement_alarm.sw=0;
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
}

void track_cust_no_acc_disable_moving_once_alarm()
{
	#if defined (__FUN_WITH_ACC__)
		return;
	#else
    	tr_start_timer(TRACK_CUST_MOVING_ALARM_ONCE_TIMER, 15*1000, track_cust_moving_alarm_off);
	#endif	
}

#endif


/******************************************************************************
 *  Function    -  displacement_overtime
 *
 *  Purpose     -  ���ͱ�������
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void displacement_overtime(void)
{
#if defined(__XCWS__)
    if(G_parameter.outs == 1)
    {
        track_cust_alarm_trigger_req(ALARM_MOVE_OUT, ALARM_SMS);
    }
    if(G_parameter.out == 1)
    {
        track_cust_alarm_trigger_req(ALARM_MOVE_OUT, ALARM_GPRS);
#if defined(__XCWS_TPATH__)&& !defined(__KENAER__)
        if(track_nvram_alone_parameter_read()->gprson)
        {
            track_cust_paket_16(&gps_data_buf, track_drv_get_lbs_data(), TR_CUST_ALARM_displacement, KAL_TRUE, 1);
        }
#endif
    }
    return;
#endif
    if(G_parameter.displacement_alarm.sw)
    {
        if(!G_parameter.displacement_alarm.alarm_type)
        {
            LOGD(L_APP, L_V4, "λ�Ʊ������������������������֪ͨ��");
            return;
        }
        memset(&g_msg, 0, sizeof(struct_msg_send));
        g_msg.addUrl = KAL_TRUE;
        g_msg.cm_type = CM_SMS;
        track_fun_strncpy(g_msg.msg_en, str_displacement_en, CM_PARAM_LONG_LEN_MAX);
        g_msg.msg_cn_len = 20;
        memcpy(g_msg.msg_cn, str_displacement_cn, g_msg.msg_cn_len);
        track_cust_module_alarm_msg_send(&g_msg, &gps_data_buf);
        if(G_parameter.displacement_alarm.alarm_type == 2)
        {
            track_start_timer(TRACK_CUST_ALARM_displacement_Call_TIMER, 60000, track_cust_make_call, (void*)1);
        }
        else
        {
            track_cust_module_alarm_lock(TR_CUST_ALARM_displacement, 0);
        }
    }
    else
    {
        track_cust_module_alarm_lock(TR_CUST_ALARM_displacement, 0);
    }
    //track_cust_module_alarm_set_type(0);
#if defined (__XYBB__)
#else
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_displacement);
#endif /* __XYBB__ */

#if defined (__NT33__)
    //�յ�ָ���һ��λ�Ʊ��������������������رոù���
    track_cust_no_acc_disable_moving_once_alarm();
#endif

}

/******************************************************************************
 *  Function    -  displacement_set_overtime
 *
 *  Purpose     -  λ�Ʊ����������ĵ�λ�ó�ʱ�����Ͷ���
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void displacement_set_overtime(void)
{
    LOGD(L_APP, L_V4, "%d,%d ʵʱ��ȡλ��ԭ��ʧ�ܣ���ȡ��һ����Ч�ϴ�����Ϊλ��ԭ��",
         G_parameter.displacement_alarm.sw, G_realtime_data.flag);
    if(G_parameter.displacement_alarm.sw && G_realtime_data.flag == 1)
    {
        track_gps_data_struct *gpsLastPoint = track_cust_backup_gps_data(2, NULL);
        if(gpsLastPoint != NULL)
        {
            if(gpsLastPoint->gprmc.status != 0)
            {
                track_cust_module_sd_check(gpsLastPoint);
                return;
            }
            else
            {
                LOGD(L_APP, L_V5, "��õ���һ����Ч�ϴ�����δ��λ��%0.6f,%0.6f", gpsLastPoint->gprmc.Latitude, gpsLastPoint->gprmc.Longitude);
            }
        }
        else
        {
            LOGD(L_APP, L_V5, "��ȡ��һ����Ч�ϴ�����Ϊλ��ԭ�� ʧ��");
        }
        if(G_parameter.displacement_alarm.alarm_type)
        {
            memset(&g_msg, 0, sizeof(struct_msg_send));
            g_msg.cm_type = CM_SMS;
            track_fun_strncpy(g_msg.msg_en, str_displacement_set_en, CM_PARAM_LONG_LEN_MAX);
            g_msg.msg_cn_len = 40;
            memcpy(g_msg.msg_cn, str_displacement_set_cn, g_msg.msg_cn_len);
            track_cust_module_alarm_msg_send(&g_msg, NULL);
        }
        else
        {
            LOGD(L_APP, L_V4, "λ�Ʊ������������������������֪ͨ��");
        }
        track_cust_module_sd_stop();
        G_realtime_data.flag = 9;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    }

#if defined (__NT33__)
    //�յ�ָ���һ��λ�Ʊ��������������������رոù���
    track_cust_no_acc_disable_moving_once_alarm();
#endif

}

/******************************************************************************
 *  Function    -  displacement_alart
 *
 *  Purpose     -  λ�Ʊ����ı�������
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void displacement_alart(void)
{
    LOGD(L_APP, L_V4, "!!! λ�Ʊ��� !!!");
    if(track_cust_module_alarm_lock(TR_CUST_ALARM_displacement, 2) != 0)
    {
        LOGD(L_APP, L_V4, "��һλ�Ʊ�������δ���!");
        return;
    }
    track_cust_led_alarm();
    track_cust_module_alarm_lock(TR_CUST_ALARM_displacement, 1);
    G_realtime_data.longitude = 0;
    G_realtime_data.latitude = 0;
    G_realtime_data.flag = 0;
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    if(G_realtime_data.netLogControl & 16)
    {
        cust_packet_log(16, 2, 0, 0, displacement_distance, gps_data_buf.gprmc.Latitude, gps_data_buf.gprmc.Longitude);
    }

    
#if defined(__XCWS__)
    track_os_intoTaskMsgQueue(displacement_overtime);
    return;
#endif
    if(track_nvram_alone_parameter_read()->gprson)
    {
#if defined (__XYBB__)
		track_cust_alarm_type(TR_CUST_ALARM_displacement);
		track_cust_paket_0200(&gps_data_buf,track_drv_get_lbs_data());
#else
        track_cust_paket_16(&gps_data_buf, track_drv_get_lbs_data(), TR_CUST_ALARM_displacement, KAL_TRUE, 1);
#endif /* __XYBB__ */
    }
    if(G_parameter.displacement_alarm.alarm_type > 0)
    {

        if(G_parameter.displacement_alarm.alarm_type == 3)
        {
#if defined (__XYBB__)
#else
            track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_displacement);
#endif /* __XYBB__ */
            track_start_timer(TRACK_CUST_ALARM_displacement_Call_TIMER, 10 * 1000, track_cust_make_call, (void*)1);
            #if defined (__NT33__)
                //�յ�ָ���һ��λ�Ʊ��������������������رոù���
                track_cust_no_acc_disable_moving_once_alarm();
            #endif

        }
        else
#if defined (__GT300__) && defined(__LANG_ENGLISH__)
            if(track_nvram_alone_parameter_read()->gprson == 0)
#else
            if(G_parameter.lang == 0 || track_nvram_alone_parameter_read()->gprson == 0)
#endif /* __GT300__ */
            {
                track_os_intoTaskMsgQueue(displacement_overtime);
            }
            else
            {
                tr_start_timer(TRACK_CUST_ALARM_displacement_OVERTIME_TIMER, TRACK_ALARM_DELAY_TIME_SECONDS, displacement_overtime);
            }
    }
    else
    {
        track_cust_module_alarm_lock(TR_CUST_ALARM_displacement, 0);
    }
}

/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/



/******************************************************************************
 *  Function    -  track_cust_module_sd_check
 *
 *  Purpose     -  ����Ƿ񴥷�λ�Ʊ���
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 27-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_sd_check(track_gps_data_struct *gps_data)
{
    kal_uint8 acc_status = track_cust_status_acc();
    LOGD(L_APP, L_V8, "λ�Ʊ��� %d,%d,%d", G_parameter.displacement_alarm.sw, acc_status, G_realtime_data.flag);

#if defined (__NT33__)
	#if defined (__FUN_WITH_ACC__)
	    if(!G_parameter.displacement_alarm.sw || (acc_status == 1))
	    {
	        return;
	    }

	#else
	    //�յ�ָ��Ϳ�����ACCû��
	    if(!G_parameter.displacement_alarm.sw)
	    {
	        return;
	    }
	#endif
	
#else
    if(!G_parameter.displacement_alarm.sw || (acc_status == 1))
    {
        return;
    }
#endif /* __NT33__ */
    
    LOGD(L_APP, L_V7, "λ�Ʊ��� %d", G_realtime_data.flag);
    if(G_realtime_data.flag >= 1)
    {
        static kal_uint32 count = 0;
        float lat, lon;

        if(G_realtime_data.flag == 1)
        {
            gps_data = track_cust_backup_gps_data(2, NULL);
            if(gps_data->gprmc.status == 0)
            {
                return;
            }
        }
        if(gps_data->gprmc.EW == 'E')
        {
            lon = gps_data->gprmc.Longitude;
        }
        else
        {
            lon = -gps_data->gprmc.Longitude;
        }
        if(gps_data->gprmc.NS == 'N')
        {
            lat = gps_data->gprmc.Latitude;
        }
        else
        {
            lat = -gps_data->gprmc.Latitude;
        }
        if(G_realtime_data.flag == 1)
        {
            tr_stop_timer(TRACK_CUST_ALARM_displacement_set_OVERTIME_TIMER);
            count = 0;
            G_realtime_data.longitude = lon;
            G_realtime_data.latitude = lat;
            G_realtime_data.flag = 2;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
            LOGD(L_APP, L_V4, "λ��ԭ���趨 Lat:%.6f; Lon:%.6f", lat, lon);
            if(G_realtime_data.netLogControl & 16)
            {
                cust_packet_log(16, 1, 0, 0, 0, lat, lon);
            }
            track_cust_gps_location_timeout(20);
        }
        else if(G_realtime_data.flag == 2)
        {
            float ret = track_fun_get_GPS_2Points_distance(G_realtime_data.latitude, G_realtime_data.longitude, lat, lon);
            LOGD(L_APP, L_V6, "λ�Ʊ��� ret=%.2fm", ret);
            if(ret > G_parameter.displacement_alarm.radius)
            {
                if(!track_is_timer_run(TRACK_CUST_ALARM_displacement_CONFIRM_TIMER))
                {
                    LOGD(L_APP, L_V4, "λ�Ʊ��� ������! 10s��ʱȷ��");
                    memcpy(&gps_data_buf, gps_data, sizeof(track_gps_data_struct));
                    displacement_distance = ret;
                    tr_start_timer(TRACK_CUST_ALARM_displacement_CONFIRM_TIMER, 10000, displacement_alart);
                }
            }
            else
            {
                track_stop_timer(TRACK_CUST_ALARM_displacement_CONFIRM_TIMER);
            }
        }
    }
}

/******************************************************************************
 *  Function    -  track_cust_module_sd_acc
 *
 *  Purpose     -  ����λ��ԭ��
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 30-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_sd_acc(void *arg)
{
    kal_uint32 par = (kal_uint32)arg;
    if(!G_parameter.displacement_alarm.sw)
    {
        return;
    }
    LOGD(L_APP, L_V5, "%d", par);
    switch(par)
    {
        case 1:
            if(G_realtime_data.flag == 9) break;
        case 2:
            if(G_realtime_data.flag == 1) break;
        case 3:
            LOGD(L_APP, L_V4, "ACC OFF λ�Ʊ�����");
            if(G_realtime_data.netLogControl & 16)
            {
                cust_packet_log(16, 0, par, 0, 0, 0, 0);
            }
            track_cust_gps_work_req((void*)6);
            G_realtime_data.flag = 1;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
            
#if defined (__CUSTOM_DDWL__)
            /*��㷢�����������ģʽ���ܷ�����  --   chengjun  2017-06-12*/
            tr_start_timer(TRACK_CUST_ALARM_displacement_set_OVERTIME_TIMER, 270000, displacement_set_overtime);
#else
            tr_start_timer(TRACK_CUST_ALARM_displacement_set_OVERTIME_TIMER, 300000, displacement_set_overtime);
#endif /* __CUSTOM_DDWL__ */
            
            break;
        case 4:
            {
                // acc on
                if(G_realtime_data.flag != 0)
                {
                    LOGD(L_APP, L_V4, "ACC ON λ�Ʊ�����");
                    track_cust_module_sd_stop();
                    if(G_realtime_data.netLogControl & 16)
                    {
                        cust_packet_log(16, 0, par, 0, 0, 0, 0);
                    }
                }
            }
            break;
    }
}

/******************************************************************************
 *  Function    -  track_cust_module_sd_stop
 *
 *  Purpose     -  ֹͣλ�Ʊ���
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_sd_stop(void)
{
    LOGD(L_APP, L_V5, "");
    tr_stop_timer(TRACK_CUST_ALARM_displacement_OVERTIME_TIMER);
    tr_stop_timer(TRACK_CUST_ALARM_displacement_set_OVERTIME_TIMER);
    track_stop_timer(TRACK_CUST_ALARM_displacement_CONFIRM_TIMER);
    track_stop_timer(TRACK_CUST_ALARM_displacement_Call_TIMER);
    track_cust_module_alarm_lock(TR_CUST_ALARM_displacement, 0);
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_displacement);
    G_realtime_data.longitude = 0;
    G_realtime_data.latitude = 0;
    G_realtime_data.flag = 0;
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
}

