/******************************************************************************
 * track_cust_module_fakecell.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        ��α��վ��صĴ���
 *
 *        �������ϱ����������Զ�����ʹ��
 *
 * modification history
 * --------------------
 * v1.0   2017-03-28,  chengjun create this file
 *
 ******************************************************************************/
/****************************************************************************
*  Include Files                        ����ͷ�ļ�
*****************************************************************************/
#include "track_cust.h"
#include "dcl_pw.h"
#include "dcl.h"


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
static char *str_fake_cell_en = "Attention! Find The fake cell equipment."; /*Attention! Find The fake cell equipment.*/
static char str_fake_cell_cn[24] =/*��ע�⣡��⵽α��վ�豸*/
{
    0x8B, 0xF7, 0x6C, 0xE8 , 0x61 , 0x0F , 0xFF , 0x01 , 0x68 , 0xC0 , 0x6D , 0x4B , 0x52 , 0x30 , 0x4F , 0x2A , 0x57 , 0xFA , 0x7A , 0xD9 , 0x8B , 0xBE , 0x59 , 0x07
};

/****************************************************************************
*  Global Variable                      ȫ�ֱ���
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/
extern void track_fun_signed_LongitudeLatitudeToHex(kal_uint8 *dec, float value);
extern float track_fun_get_GPS_2Points_distance(float lat1, float lon1, float lat2, float lon2);
/*��Ҫɾ��������������� -- chengjun  2017-03-10*/

/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/


/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/
static void fake_cell_sendsms(void)
{
    struct_msg_send g_msg = {0};
    LOGD(L_APP, L_V5, "α��վ��������");
    g_msg.addUrl = KAL_FALSE;
    g_msg.cm_type = CM_SMS;
    snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_fake_cell_en);
    g_msg.msg_cn_len = 24;
    memcpy(g_msg.msg_cn, str_fake_cell_cn, g_msg.msg_cn_len);
    track_cust_module_alarm_msg_send(&g_msg, NULL);
}

/******************************************************************************
 *  Function    -  track_cust_fake_cell_alarm
 *
 *  Purpose     -  ȷ��α��վ��Ϣ��������--����ƽ̨�ϱ�
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-02-15,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_fake_cell_alarm()
{
    track_gps_data_struct *gpsLastPoint = NULL;

    if(G_realtime_data.fake_cell_info_notes.fake_cell_flg != 2)
    {
        return;
    }

    if(G_parameter.fcfalm.sw == 0)
    {
        //������������Ѿ��رգ���ѯ����������
        if(G_realtime_data.fake_cell_info_notes.fake_cell_flg != 0)
        {
            G_realtime_data.fake_cell_info_notes.fake_cell_flg = 0;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        }
        return;
    }

    if(!track_soc_login_status())
    {
        return;
    }

    LOGD(L_APP, L_V5, "α��վ����");
    gpsLastPoint = track_cust_gpsdata_alarm();
    if(track_nvram_alone_parameter_read()->gprson)
    {
        track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_FAKECELL, KAL_FALSE, 0);
    }
    if(G_parameter.fcfalm.alarm_type == 1)
    {
        fake_cell_sendsms();
    }
    if(G_parameter.fcfalm.alarm_type == 2)
    {
        fake_cell_sendsms();
        track_start_timer(TRACK_CUST_ALARM_FCARM_TIMER, 60000, track_cust_make_call, (void*)1);
    }
    if(G_parameter.fcfalm.alarm_type == 3)
    {
        track_start_timer(TRACK_CUST_ALARM_FCARM_TIMER, 30000, track_cust_make_call, (void*)1);
    }

    G_realtime_data.fake_cell_info_notes.fake_cell_flg = 0;
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
}



/******************************************************************************
 *  Function    -  track_get_fake_cell_time_place_backup
 *
 *  Purpose     -  �ɼ�����α��վ��ʱ�䡢�ص㣬�����ϱ�AMS�ռ�����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-03-09,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint8 track_get_fake_cell_time_place_backup()
{
    float longitude, latitude;
    track_gps_data_struct *gpsLastPoint = NULL;
    applib_time_struct time = {0};

    if(G_parameter.fake_cell_to_ams == 0)
    {
        return 5;
    }

    if(G_realtime_data.fake_cell_info_notes.fake_cell_count >= 4)
    {
        return 4;
    }

    gpsLastPoint = track_cust_backup_gps_data(2, NULL);
    if(gpsLastPoint == NULL)
    {
        latitude = 0;
        longitude = 0;
    }

    if(gpsLastPoint->gprmc.NS == 'S')
    {
        latitude = -gpsLastPoint->gprmc.Latitude;
    }
    else
    {
        latitude = gpsLastPoint->gprmc.Latitude;
    }

    if(gpsLastPoint->gprmc.EW == 'W')
    {
        longitude = -gpsLastPoint->gprmc.Longitude;
    }
    else
    {
        longitude = gpsLastPoint->gprmc.Longitude;
    }


    G_realtime_data.fake_cell_info_notes.last_latitude = latitude;
    G_realtime_data.fake_cell_info_notes.last_longitude = longitude;


    time.nYear = gpsLastPoint->gprmc.Date_Time.nYear;
    time.nMonth = gpsLastPoint->gprmc.Date_Time.nMonth;
    time.nDay = gpsLastPoint->gprmc.Date_Time.nDay;
    time.nHour = gpsLastPoint->gprmc.Date_Time.nHour;
    time.nMin = gpsLastPoint->gprmc.Date_Time.nMin;
    time.nSec = gpsLastPoint->gprmc.Date_Time.nSec;
    G_realtime_data.fake_cell_info_notes.alm_sec[G_realtime_data.fake_cell_info_notes.fake_cell_count] = OTA_applib_dt_mytime_2_utc_sec(&time, 0);
    //G_realtime_data.fake_cell_info_notes.alm_sec[G_realtime_data.fake_cell_info_notes.fake_cell_count]=OTA_app_getcurrtime();

    G_realtime_data.fake_cell_info_notes.alm_longitude[G_realtime_data.fake_cell_info_notes.fake_cell_count] = longitude;
    G_realtime_data.fake_cell_info_notes.alm_latitude[G_realtime_data.fake_cell_info_notes.fake_cell_count] = latitude;

    G_realtime_data.fake_cell_info_notes.fake_cell_count++;

    LOGD(L_APP, L_V5, "α��վ��Ϣ(%d) %d-%d-%d %d:%d:%d (%d) lat:%f,lon:%f", G_realtime_data.fake_cell_info_notes.fake_cell_count, \
         time.nYear, time.nMonth, time.nDay, time.nHour, time.nMin, time.nSec, \
         G_realtime_data.fake_cell_info_notes.alm_sec[G_realtime_data.fake_cell_info_notes.fake_cell_count], latitude, longitude);

    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

    return G_realtime_data.fake_cell_info_notes.fake_cell_count;

}


void track_cust_fake_cell_alrm_info_to_ams(void)
{
    kal_uint8 fake_cell_info[64];
    kal_uint8 i, len;

    LOGD(L_APP, L_V5, "��AMS�ϱ�α��վ��Ϣ %d,%d", G_parameter.fake_cell_to_ams, G_realtime_data.fake_cell_info_notes.fake_cell_count);

    if(G_parameter.fake_cell_to_ams == 0)
    {
        return;
    }

    if(G_realtime_data.fake_cell_info_notes.fake_cell_count == 0)
    {
        return;
    }

    memset(fake_cell_info, 0, sizeof(fake_cell_info));

    len = 0;
    for(i = 0; i < G_realtime_data.fake_cell_info_notes.fake_cell_count; i++)
    {

        LOGD(L_APP, L_V5, "α��վ��Ϣ(%d) %d,%f,%f", i, \
             G_realtime_data.fake_cell_info_notes.alm_sec[i], G_realtime_data.fake_cell_info_notes.alm_latitude[i], G_realtime_data.fake_cell_info_notes.alm_longitude[i]);

        track_fun_reverse_32(&fake_cell_info[len], G_realtime_data.fake_cell_info_notes.alm_sec[i]);
        track_fun_signed_LongitudeLatitudeToHex(&fake_cell_info[len+4], G_realtime_data.fake_cell_info_notes.alm_latitude[i]);
        track_fun_signed_LongitudeLatitudeToHex(&fake_cell_info[len+8], G_realtime_data.fake_cell_info_notes.alm_longitude[i]);
        len = len + 12;
    }

    track_cust_paket_77(fake_cell_info, len, 0x06);

    G_realtime_data.fake_cell_info_notes.fake_cell_count = 0;
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
}


/******************************************************************************
 *  Function    -  track_cust_fake_cell_confirm
 *
 *  Purpose     -  ����10����GPRS������ȷ����α��վ
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-02-15,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_fake_cell_confirm(void)
{
    kal_uint8 ret;

    if(G_realtime_data.fake_cell_info_notes.fake_cell_flg == 1)
    {
        G_realtime_data.fake_cell_info_notes.fake_cell_flg = 2;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

        //������������AMS�ռ���Ϣ
        ret = track_get_fake_cell_time_place_backup();
        LOGD(L_APP, L_V5, "α��վ����ȷ�� %d", ret);
    }
}

/******************************************************************************
 *  Function    -  track_cust_fake_cell_confirm_timeout
 *
 *  Purpose     -  ����α��վ�����ж����󣬺���
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-02-15,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_fake_cell_confirm_timeout(void)
{
    LOGD(L_APP, L_V5, "����α��վ %d", G_realtime_data.fake_cell_info_notes.fake_cell_flg);
    if(G_realtime_data.fake_cell_info_notes.fake_cell_flg == 1)
    {
        G_realtime_data.fake_cell_info_notes.fake_cell_flg = 0;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    }
}


/******************************************************************************
 *  Function    -  track_cust_check_gprs_after_fake_cell
 *
 *  Purpose     -  �ѵ�α��վ��һ��ʱ�䣬���GPRS����״̬
 *
 *  Description -  ����10����ȷ��
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-02-15,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_check_gprs_after_fake_cell(kal_bool connected)
{
    static kal_bool first = KAL_TRUE;

    LOGD(L_APP, L_V5, "%d,%d", G_realtime_data.fake_cell_info_notes.fake_cell_flg, connected);
#if 0
    if(G_parameter.fcfalm.sw == 0)
    {
        //������������Ѿ��رգ���ѯ����������
        if(G_realtime_data.fake_cell_info_notes.fake_cell_flg != 0)
        {
            G_realtime_data.fake_cell_info_notes.fake_cell_flg = 0;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        }
        return;
    }
#endif
    if(G_realtime_data.fake_cell_info_notes.fake_cell_flg == 1)
    {
        //���GPRS����������������������־��ʱ���ƶ�ʱ��
        if(first)
        {
            first = KAL_FALSE;
            if(!track_is_timer_run(TRACK_CUST_FAKECELL_FLAG_TIMER_ID))
            {
                tr_start_timer(TRACK_CUST_FAKECELL_FLAG_TIMER_ID, G_parameter.fcfalm.alarm_filter * 60 * 1000, track_cust_fake_cell_confirm_timeout);
            }
        }

        if(connected)
        {
            if(track_is_timer_run(TRACK_CUST_FAKECELL_DISCONNECT_GPRS_TIMER_ID))
            {
                tr_stop_timer(TRACK_CUST_FAKECELL_DISCONNECT_GPRS_TIMER_ID);
            }
        }
        else
        {
            if(!track_is_timer_run(TRACK_CUST_FAKECELL_DISCONNECT_GPRS_TIMER_ID))
            {
                tr_start_timer(TRACK_CUST_FAKECELL_DISCONNECT_GPRS_TIMER_ID, 600 * 1000, track_cust_fake_cell_confirm);
            }
        }
    }
    else if(G_realtime_data.fake_cell_info_notes.fake_cell_flg == 2)
    {
        if(connected)
        {
            track_cust_fake_cell_alarm();
        }
    }
}


/******************************************************************************
 *  Function    -  track_is_fake_cell_distance_valid
 *
 *  Purpose     -  ���㵱ǰλ�����ϴη���α��վλ�õľ���
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-03-09,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_int8 track_is_fake_cell_distance_valid(void)
{
    float distance = 0;

    track_gps_data_struct *gpsLastPoint = NULL;
    gpsLastPoint = track_cust_backup_gps_data(2, NULL);
    if(gpsLastPoint == NULL)
    {
        return -1;
    }

    if((G_realtime_data.fake_cell_info_notes.last_latitude == 0) && (G_realtime_data.fake_cell_info_notes.last_longitude == 0))
    {
        return 1;
    }

    distance = track_fun_get_GPS_2Points_distance(G_realtime_data.fake_cell_info_notes.last_latitude, G_realtime_data.fake_cell_info_notes.last_longitude, \
               gpsLastPoint->gprmc.Latitude, gpsLastPoint->gprmc.Longitude);
    if(distance > 500)
    {
        return 2;
    }

    return -2;
}


/******************************************************************************
 *  Function    -  track_cust_search_fake_cell
 *
 *  Purpose     -  �ѵ�α��վ,��ʼ���GPRS
 *
 *  Description -  alarm_filter������GPRS��������10���ӣ�ȷ����α��վ
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-02-15,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_search_fake_cell(void)
{
    kal_int8 ret;

    if((G_parameter.fake_cell_to_ams == 0) && (G_parameter.fcfalm.sw == 0))
    {
        LOGD(L_APP, L_V5, "α��վ��Ϣ�ϱ��ر�");
        return;
    }

    if(G_realtime_data.fake_cell_info_notes.fake_cell_flg == 0)
    {
        ret = track_is_fake_cell_distance_valid();
        LOGD(L_APP, L_V5, "%d,%d", G_realtime_data.fake_cell_info_notes.fake_cell_flg, ret);
        if(ret < 0)
        {
            LOGD(L_APP, L_V5, "λ����Ϣ�����㣬����α��վ��Ϣ");
            return;
        }

        G_realtime_data.fake_cell_info_notes.fake_cell_flg = 1;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        tr_start_timer(TRACK_CUST_FAKECELL_FLAG_TIMER_ID, G_parameter.fcfalm.alarm_filter * 60 * 1000, track_cust_fake_cell_confirm_timeout);
        tr_start_timer(TRACK_CUST_FAKECELL_DISCONNECT_GPRS_TIMER_ID, 600 * 1000, track_cust_fake_cell_confirm);
    }
    else
    {
        LOGD(L_APP, L_V5, "�ڴ����ϴ�α��վ����");
    }
}


void track_cust_fake_cell_test_forced_alarm(void)
{
    LOGD(L_APP, L_V5, "����ǿ�ƴ���α��վ����");

    G_realtime_data.fake_cell_info_notes.fake_cell_flg = 1;
    track_cust_fake_cell_confirm();
}



/******************************************************************************
 *  Function    -  track_cust_fake_cell_auto_enable_detect
 *
 *  Purpose     -  �ж��Ƿ�ָ�������α��վ����
 *
 *  Description -  no fix or <2km/h-5s
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-03-28,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_fake_cell_auto_enable_detect(void)
{
    static kal_uint8 count = 0;

    if(G_parameter.fake_cell_enable == 0)
    {
        if(G_parameter.fake_cell_auto_temp != 0xFF)
        {
            G_parameter.fake_cell_auto_temp = 0xFF;
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        }
        return;
    }

    if(G_parameter.fake_cell_auto_temp == 0)
    {
        if(track_cust_gps_status()<=GPS_STAUS_SCAN)
        {
            count++;
        }
        else
        {
            track_gps_data_struct *gpsLastPoint = NULL;
            gpsLastPoint = track_cust_backup_gps_data(0, NULL);
            if(gpsLastPoint->gprmc.Speed<=2)
            {
                count++;
            }
            else
            {
                count=0;
            }
        }

        if(count>5)
        {
            LOGD(L_APP, L_V5, "resume enable fake cell");
            OTA_track_custom_fake_cell_feature_enable(1);
            G_parameter.fake_cell_auto_temp = 0xFF;
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
            count=0;
            return;
        }
    }
    else
    {
        return;
    }
    tr_start_timer(TRACK_CUST_FAKECELL_DISABLE_AUTO_RESUME_TIMER_ID, 1000, track_cust_fake_cell_auto_enable_detect);
}


/******************************************************************************
 *  Function    -  track_cust_fake_cell_low_speed_disable_detect
 *
 *  Purpose     -  �����α������GPS��λ�ҳ����˶����رշ�α����
 *
 *  Description -   >10km/h and 5s
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-03-28,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_fake_cell_low_speed_disable_detect(float speed)
{
    static kal_uint8 count = 0;

    if(G_realtime_data.fake_cell_info_notes.fake_cell_flg != 2)
    {
        return;
    }

    if(G_parameter.fake_cell_enable == 0)
    {
        if(G_parameter.fake_cell_auto_temp != 0xFF)
        {
            G_parameter.fake_cell_auto_temp = 0xFF;
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        }
        return;
    }
    
    if(speed > 10)
    {
        count++;
        if(count > 5)
        {
            LOGD(L_APP, L_V5, "disable fake cell");
            count = 0;
            OTA_track_custom_fake_cell_feature_enable(0);
            G_parameter.fake_cell_auto_temp = 0;
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
            tr_start_timer(TRACK_CUST_FAKECELL_DISABLE_AUTO_RESUME_TIMER_ID, 20000, track_cust_fake_cell_auto_enable_detect);
        }
    }
    else
    {
        count = 0;
    }
}

