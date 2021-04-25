/******************************************************************************
 * track_cust_module_blind.c - 
 * 
 * Copyright .
 * 
 * DESCRIPTION: - 
 *        ����С��������
 *
 *  GPSä������ �ػ����� �������� α��վ���� ժ������ ���Ǳ���
 * 
 * modification history
 * --------------------
 * v1.0   2017-03-23,  chengjun create this file
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
#if defined(__GERMANY__)
static char *str_blind_in_en = "Achtung! GPS Signal ist schlecht - Position kann nicht bestimmt werden.";
#else
static char *str_blind_in_en = "Attention! GPS signal is weak, unable to locate in the region.";
#endif
static char str_blind_in_cn[34] =   //����GPS�ź�ä����
{
    0X8F , 0XDB, 0X51 , 0X65 , 0X00 , 0X47 , 0X00 , 0X50 , 0X00 , 0X53 , 0X4F , 0XE1 , 0X53 , 0XF7 , 0X76 , 0XF2 , 0X53 , 0X3A , 0XFF , 0X01
};

#if defined(__GERMANY__)
static char *str_blind_out_en = "GPS-Positionierung erfolgreich!";
#else
static char *str_blind_out_en = "GPS re-positioning successful!";
#endif
static char str_blind_out_cn[34] =   // ���뿪GPS�ź�ä����
{
    0X5D  , 0XF2  , 0X79  , 0XBB  , 0X5F  , 0X00  , 0X00  , 0X47  , 0X00  , 0X50  , 0X00  , 0X53 , 0X4F  , 0XE1 , 0X53  , 0XF7  , 0X76  , 0XF2  , 0X53  , 0X3A  , 0X00  , 0X21
};
static char  blind_status = 2; //0Ϊ����ä���У�1Ϊ�˳�ä����2Ϊ��ʼֵ����
static char str_door_cn[22] =/*ע��! ���ű��� .*/
{
    0x6C , 0xE8 , 0x61 , 0x0F , 0x00 , 0x21 , 0x00 , 0x20 , 0x8F , 0x66 , 0x95 , 0xE8 , 0x88 , 0xAB , 0x62 , 0x53 , 0x5F , 0x00 , 0x00 , 0x20 , 0x00 , 0x2E
};

static char *str_door_en = "Attention! Vehicle side door is opened."; /*Attention! The door is open!*/
#if defined(__GW100__)
static char *str_boot_en = "The The monitoring phone is power-on!";
static char str_boot_cn[16] =/*�໤�ֻ��ѿ�����*/
{
    0x76, 0xD1 , 0x62, 0xA4 , 0x62 , 0x4B , 0x67 , 0x3A , 0x5D , 0xF2 , 0x5F , 0x00 , 0x67 , 0x3A , 0xFF , 0x01
};
#else
static char *str_boot_en = "Terminal boot successfully!";
static char str_boot_cn[14] = /* �ն˿����ɹ��� */
{
    0x7E, 0xC8, 0x7A, 0xEF, 0x5F, 0x00, 0x67, 0x3A, 0x62, 0x10, 0x52, 0x9F, 0xFF, 0x01
};
#endif /* __GW100__ */

static char *str_gps_successfully_en = "GPS Successful positioning!";
static char str_gps_successfully_cn[16] =/*GPS��λ�ɹ�!*/
{
    0X00 , 0X47 , 0X00 , 0X50 , 0X00 , 0X53 , 0X5B , 0X9A , 0X4F , 0X4D , 0X62 , 0X10 , 0X52 , 0X9F , 0X00 , 0X21
};

static char *str_sim_alarm_en = "The monitoring phone SIM card has been changed,please pay attention!";
static char str_sim_alarm_cn[32] =/*�໤�ֻ�SIM���Ѹ��������ע��*/
{
    0x76, 0xD1, 0x62, 0xA4, 0x62, 0x4B, 0x67, 0x3A, 0x00, 0x53, 0x00, 0x49, 0x00, 0x4D, 0x53, 0x61, 0x5D, 0xF2, 0x66, 0xF4, 0x63, 0x62, 0xFF, 0x0C, 0x8B, 0xF7, 0x51, 0x73, 0x6C, 0xE8, 0xFF, 0x01
};

#if defined (__GT370__) || defined (__GT380__)
static char *str_pwoffalm_en = "Attention! The terminal has been shut down!";
static char str_pwoffalm_cn[20] =/*��ע�⣡�ն��ѹػ���*/
{
    0x8B,0xF7,0x6C,0xE8,0x61,0x0F,0xFF,0x01,0x7E,0xC8,0x7A,0xEF,0x5D,0xF2,0x51,0x73,0x67,0x3A,0xFF,0x01
};
#else
static char *str_pwoffalm_en = "Attention! The monitoring phone is power-off!";
static char str_pwoffalm_cn[24] =/*��ע�⣡�໤�ֻ��ѹػ���*/
{
    0x8B, 0xF7, 0x6C, 0xE8, 0x61, 0x0F, 0xFF, 0x01, 0x76, 0xD1, 0x62, 0xA4, 0x62, 0x4B, 0x67, 0x3A, 0x5D, 0xF2, 0x51, 0x73, 0x67, 0x3A, 0xFF, 0x01
};
#endif
#if defined(__GT420D__)
static char *str_teardown_en = "Attention! Packing open!";
static char str_teardown_cn[20] = /*ע�⣡ ��װ�ѿ���!*/
{
    0x6C, 0xE8, 0x61, 0x0F, 0xFF, 0x01, 0x00, 0x20, 0x53, 0x05, 0x88, 0xC5, 0x5D, 0xF2, 0x5F, 0x00, 0x5C, 0x01, 0xFF, 0x01
};

#else
static char *str_teardown_en = "Attention!The device has been torn down!";
static char str_teardown_cn[18] = /*ע�⣡�豸��ժ����*/
{
    0x6C, 0xE8, 0x61, 0x0F, 0xFF, 0x01, 0x8B, 0xBE, 0x59, 0x07, 0x88, 0xAB, 0x64, 0x58, 0x96, 0x64, 0xFF, 0x01
};

#endif

static char *str_open_shell_en = "Attention!The device has been opened!";
static char str_open_shell_cn[18] = /*ע�⣡�豸���򿪣�*/
{
    0x6C, 0xE8, 0x61, 0x0F, 0xFF, 0x01, 0x8B, 0xBE, 0x59, 0x07, 0x88, 0xAB, 0x62, 0x53, 0x5F, 0x00, 0xFF, 0x01
};
static char *str_collision_en = "Attention!The device has been bumped  !";
static char str_collision_cn[18] = /*ע�⣡�豸����ײ��*/
{	
	0x6C, 0xE8, 0x61, 0x0F, 0xFF, 0x01, 0x8B, 0xBE, 0x59, 0x07, 0x88, 0xAB, 0x78, 0xB0, 0x64, 0x9E, 0xFF, 0x01 
};
static char *str_tiltalm_en = "Attention!Equipment tilt  !";
static char str_tiltalm_cn[16] = /*ע�⣡�豸��б��*/
{	
	0x6C, 0xE8, 0x61, 0x0F, 0xFF, 0x01, 0x8B, 0xBE, 0x59, 0x07, 0x50, 0x3E, 0x65, 0x9C, 0xFF, 0x01 
};

static struct_msg_send g_msg = {0};

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

#if defined (__GT370__)|| defined (__GT380__)
extern void track_cust_poweroff_ready(void);
#endif /* __GT370__ */

/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/

#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/

PW_CTRL_POWER_ON_REASON mmi_Get_PowerOn_Type(void)
{
    extern kal_bool track_os_log_abnormal_is_net_error(void);
    static S8 count = 5;
    if(count > 0)
    {
        count--;
        LOGD(L_APP, L_V5, "gps_PWRon =%d", *OTA_gps_PWRon);
    }
    if(track_os_log_abnormal_is_net_error() == TRUE)
    {
        track_os_log_abnormal_net_error_set();
        LOGD(L_APP, L_V5, "������152��������������");
        *OTA_gps_PWRon = 0XF9;
    }
    return *OTA_gps_PWRon;
}

/******************************************************************************
 *  Function    -  boot_sendsms
 *
 *  Purpose     -  ���Ϳ�������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 02-08-2013,  jwj  written
 * ----------------------------------------
 ******************************************************************************/
void boot_sendsms(void)
{
    struct_msg_send g_msg = {0};
    LOGD(L_APP, L_V5, "�������ŷ���");
    g_msg.addUrl = KAL_FALSE;
    g_msg.cm_type = CM_SMS;
    snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_boot_en);
#ifndef __GW100__
    g_msg.msg_cn_len = 14;
#else
    g_msg.msg_cn_len = 16;
#endif /* __GW100__ */
    //g_msg.msg_cn_len = sizeof(str_boot_cn);  chengj

    memcpy(g_msg.msg_cn, str_boot_cn, g_msg.msg_cn_len);
    track_cust_module_alarm_msg_send(&g_msg, NULL);
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_boot);
}

/******************************************************************************
 *  Function    -  gps_successfully_sendsms
 *
 *  Purpose     -  gps��λ�ɹ�����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 02-08-2013,  jwj  written
 * ----------------------------------------
 ******************************************************************************/
void gps_successfully_sendsms(void)
{
    struct_msg_send g_msg = {0};
    LOGD(L_APP, L_V5, "GPS�������ŷ���");

    g_msg.addUrl = KAL_FALSE;
    g_msg.cm_type = CM_SMS;
    snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_gps_successfully_en);
    g_msg.msg_cn_len = 16;
    memcpy(g_msg.msg_cn, str_gps_successfully_cn, g_msg.msg_cn_len);
    track_cust_module_alarm_msg_send(&g_msg, NULL);
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_gps_successfully);
}

/******************************************************************************
 *  Function    -  track_cust_alarm_boot
 *
 *  Purpose     -  ��������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 02-08-2013,  jwj  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_alarm_boot(void)
{
    track_gps_data_struct *gpsLastPoint = NULL;

    if(G_parameter.boot.sw == 0)
    {
        return;
    }
    LOGD(L_APP, L_V5, "��������G_parameter.boot.alarm_type=%d,%d", G_parameter.boot.alarm_type, G_parameter.boot.sw);
    gpsLastPoint = track_cust_gpsdata_alarm();
    if(track_nvram_alone_parameter_read()->gprson)
    {
        track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_boot, KAL_TRUE, 0);
    }
    if(G_parameter.boot.alarm_type == 1)
    {
        boot_sendsms();
    }
}
/******************************************************************************
 *  Function    -  track_cust_alarm_gps_successfully
 *
 *  Purpose     -  gps��λ�ɹ�
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 02-08-2013,  jwj  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_alarm_gps_successfully(void)
{
    track_gps_data_struct *gpsLastPoint = NULL;
    LOGD(L_APP, L_V5, " sw=%d  alarm_type=%d", G_parameter.boot.sw, G_parameter.boot.alarm_type);
    if(G_parameter.boot.sw == 0)
    {
        return;
    }
    gpsLastPoint = track_cust_gpsdata_alarm();
    if(track_nvram_alone_parameter_read()->gprson)
    {
        track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_gps_successfully, KAL_TRUE, 0);
    }
    if(G_parameter.boot.alarm_type == 1)
    {
        gps_successfully_sendsms();
    }
}
/******************************************************************************
 *  Function    -  blind_in_sendsms
 *
 *  Purpose     -ä�����������ŷ���
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 25-10-2012,  JWJ  written
 * ----------------------------------------
*******************************************************************************/
static void blind_out_sendsms(void)
{
    struct_msg_send g_msg = {0};
    LOGD(L_APP, L_V5, "GPSä��������");
    g_msg.addUrl = KAL_FALSE;
    g_msg.cm_type = CM_SMS;
    snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_blind_out_en);
    g_msg.msg_cn_len = 34;
    memcpy(g_msg.msg_cn, str_blind_out_cn, g_msg.msg_cn_len);
    track_cust_module_alarm_msg_send(&g_msg, NULL);
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_OUT_blind);
}
/******************************************************************************
 *  Function    -  blind_in_sendsms
 *
 *  Purpose     -ä��������
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 25-10-2012,  JWJ  written
 * ----------------------------------------
*******************************************************************************/
static void blind_in_sendsms(void)
{
    struct_msg_send g_msg = {0};
    LOGD(L_APP, L_V5, "GPSä��������");
    g_msg.addUrl = KAL_FALSE;
    g_msg.cm_type = CM_SMS;
    snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_blind_in_en);
    g_msg.msg_cn_len = 34;
    memcpy(g_msg.msg_cn, str_blind_in_cn, g_msg.msg_cn_len);
    track_cust_module_alarm_msg_send(&g_msg, NULL);
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_IN_blind);
}
/******************************************************************************
 *  Function    -  track_cust_alarm_blind_in
 *
 *  Purpose     -ä��������
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 25-10-2012,  JWJ  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_alarm_blind_in(void)
{
    track_gps_data_struct *gpsLastPoint = NULL;
    if(G_parameter.gps_blind.sw == 0)
    {
        return;
    }
    gpsLastPoint = track_cust_gpsdata_alarm();
    blind_status = 0;
    if(track_nvram_alone_parameter_read()->gprson)
    {
        track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_IN_blind, KAL_TRUE, 0);
    }
    if(G_parameter.gps_blind.alarm_type == 1 || G_parameter.gps_blind.alarm_type == 2)
    {
        blind_in_sendsms();
    }
    G_realtime_data.blind_status = blind_status;
    if(G_parameter.gps_blind.alarm_type == 2 || G_parameter.gps_blind.alarm_type == 3)
    {
        track_start_timer(TRACK_CUST_ALARM_MAKECALL_TIMER, 60000, track_cust_make_call, (void*)1);
    }

    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
}
/******************************************************************************
 *  Function    -  track_cust_alarm_blind_out
 *
 *  Purpose     -ä��������
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 25-10-2012,  JWJ  written
 * ----------------------------------------
*******************************************************************************/
track_cust_alarm_blind_out(void)
{
    track_gps_data_struct *gpsLastPoint = NULL;
    if(G_parameter.gps_blind.sw == 0)
    {
        return;
    }
    gpsLastPoint = track_cust_gpsdata_alarm();
    if(track_nvram_alone_parameter_read()->gprson)
    {
        track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_OUT_blind, KAL_FALSE, 0);
    }
    blind_status = 1;
    if(G_parameter.gps_blind.alarm_type == 1 || G_parameter.gps_blind.alarm_type == 2)
    {
        blind_out_sendsms();
    }
    G_realtime_data.blind_status = blind_status;
    if(G_parameter.gps_blind.alarm_type == 2 || G_parameter.gps_blind.alarm_type == 3)
    {
        track_start_timer(TRACK_CUST_ALARM_MAKECALL_TIMER, 60000, track_cust_make_call, (void*)1);
    }
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

}
/******************************************************************************
 *  Function    -  track_cust_module_alarm_blind
 *
 *  Purpose     -gpsä�����
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 25-10-2012,  JWJ  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_alarm_blind(void)
{
    LOGD(L_APP, L_V5, "ä�����G_parameter.gps_blind.sw=%d,%d,%d,blind_status=%d", G_parameter.gps_blind.sw,
         G_parameter.gps_blind.blind_in_time, G_parameter.gps_blind.blind_out_time, blind_status);
    if(G_parameter.gps_blind.sw == 0 || track_cust_get_work_mode() == WORK_MODE_3)
    {
        return;
    }
    if(G_realtime_data.blind_status == 0 && blind_status == 2)
    {
        blind_status = G_realtime_data.blind_status;
    }
    if(track_cust_gps_status() > GPS_STAUS_SCAN && blind_status == 0)
    {
        tr_stop_timer(TRACK_CUST_GPS_BLINDALM_IN_TIMER_ID);
        if(!track_is_timer_run(TRACK_CUST_GPS_BLINDALM_OUT_TIMER_ID))
            tr_start_timer(TRACK_CUST_GPS_BLINDALM_OUT_TIMER_ID,  G_parameter.gps_blind.blind_out_time * 1000, track_cust_alarm_blind_out);
    }
    else if(track_cust_gps_status() == GPS_STAUS_SCAN && blind_status == 1)
    {
        tr_stop_timer(TRACK_CUST_GPS_BLINDALM_OUT_TIMER_ID);
        if(!track_is_timer_run(TRACK_CUST_GPS_BLINDALM_IN_TIMER_ID))
            tr_start_timer(TRACK_CUST_GPS_BLINDALM_IN_TIMER_ID, G_parameter.gps_blind.blind_in_time * 1000, track_cust_alarm_blind_in);
    }
    else if(track_cust_gps_status() == 0)
    {
        tr_stop_timer(TRACK_CUST_GPS_BLINDALM_IN_TIMER_ID);
        tr_stop_timer(TRACK_CUST_GPS_BLINDALM_OUT_TIMER_ID);
        if(blind_status)
        {
            blind_status = 2;
        }
    }
    else if(track_cust_gps_status() > GPS_STAUS_SCAN && blind_status == 2)
    {
        blind_status = 1;
    }
    else if(track_cust_gps_status() > GPS_STAUS_SCAN)
    {
        tr_stop_timer(TRACK_CUST_GPS_BLINDALM_IN_TIMER_ID);
    }
    else if(track_cust_gps_status() == GPS_STAUS_SCAN)
    {
        tr_stop_timer(TRACK_CUST_GPS_BLINDALM_OUT_TIMER_ID);
    }
    LOGD(L_APP, L_V5, "ä�����G_parameter.gps_blind.sw=%d,%d,%d,blind_status=%d", G_parameter.gps_blind.sw,
         G_parameter.gps_blind.blind_in_time, G_parameter.gps_blind.blind_out_time, blind_status);
}
void track_cust_module_alarm_blind_init(void)
{
    LOGD(L_APP, L_V5, "");
    G_realtime_data.blind_status = 2;
    blind_status = 2;
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    track_cust_module_alarm_blind();

}

/******************************************************************************
 *  Function    -  track_cust_molude_simalarm_sms
 *
 *  Purpose     -  ������������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 22-09-2014,  ZengPing && Liujunwu  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_molude_simalarm_sms(void)
{
    struct_msg_send g_msg = {0};
    track_gps_data_struct *gpsLastPoint = NULL;
    gpsLastPoint = track_cust_gpsdata_alarm();
    track_cust_send_simalarm_msg(2);
}
/******************************************************************************
 *  Function    -  track_cust_check_sosnum
 *
 *  Purpose     -  ����ж��ٸ�SOS����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 25-09-2014,  ZengPing && Liujunwu  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint32 track_cust_check_sosnum(void)
{
    kal_uint32 phone_index = 0;
    kal_uint32 i = 0;
    for(i = 0; i < TRACK_DEFINE_SOS_COUNT; i++)
    {
        if((G_phone_number.almrep[i]) && strlen((S8*) G_parameter.sos_num[i]))
        {
            phone_index++;
        }
    }
    LOGD(L_APP, L_V5, "phone_index=%d", phone_index);
    return phone_index;
}
/******************************************************************************
 *  Function    -  track_cust_alarm_pwoffalm
 *
 *  Purpose     -  �ػ�����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 25-09-2014,  ZengPing && Liujunwu  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_pwoffalm_sendsms(void)
{
    struct_msg_send g_msg = {0};
    LOGD(L_APP, L_V5, "�ػ����ŷ���");
    g_msg.addUrl = KAL_FALSE;
    g_msg.cm_type = CM_SMS;
    snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_pwoffalm_en);
    g_msg.msg_cn_len = sizeof(str_pwoffalm_cn);
    memcpy(g_msg.msg_cn, str_pwoffalm_cn, g_msg.msg_cn_len);
    track_cust_module_alarm_msg_send(&g_msg, NULL);
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_boot);
}

void track_cust_alarm_pwoffalm(void)
{
    track_gps_data_struct *gpsLastPoint = NULL;
    LOGD(L_APP, L_V5, "�ػ�����G_parameter.pwroff.alarm_type=%d,%d", G_parameter.pwroff.alarm_type, G_parameter.pwroff.sw);

    if(G_parameter.pwroff.sw == 0)
    {
        return;
    }
    gpsLastPoint = track_cust_gpsdata_alarm();
    if(track_nvram_alone_parameter_read()->gprson)
    {
        track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_PWROFF, KAL_FALSE, 0);
    }
    if(G_parameter.pwroff.alarm_type == 1)
    {
        tr_start_timer(TRACK_CUST_SEND_SMS_PWROFFALM_TIMER_ID, 2 * 1000, track_cust_pwoffalm_sendsms);
    }
}

void track_cust_linkout_alarm()
{
    #if !defined(__GT420D__)
    LOGD(L_APP, L_V5, "�ػ����߱���");

    track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_LINKOUT_ALM, KAL_TRUE, 0);
    #endif
}

void door_sendsms(void)
{
    struct_msg_send g_msg = {0};
    LOGD(L_APP, L_V5, "�ű���");
    g_msg.addUrl = KAL_FALSE;
    g_msg.cm_type = CM_SMS;

#if defined (__NT37__) && defined (__LANG_ENGLISH__)
    /*ʹ��INPUTָ�����õ��ַ���       --    chengjun  2017-07-08*/
    snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, G_parameter.customize_alarm_sms);
    g_msg.msg_cn_len = 22;
    memcpy(g_msg.msg_cn, str_door_cn, g_msg.msg_cn_len);
#else
    snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_door_en);
    g_msg.msg_cn_len = 22;
    memcpy(g_msg.msg_cn, str_door_cn, g_msg.msg_cn_len);
#endif
    
    track_cust_module_alarm_msg_send(&g_msg, NULL);
}


#if defined (__NT37__)
kal_uint8 track_cust_door_alarm_enable(kal_uint8 enable)
{
    if(enable==0xFF)
    {
        return G_parameter.door_alarm.alarm_enable;
    }

    if((enable==0)||(enable==1))
    {
        if(G_parameter.door_alarm.alarm_enable!=enable)
        {
            G_parameter.door_alarm.alarm_enable=enable;
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        }
    }
    return enable;
}
#endif /* __NT37__ */

/******************************************************************************
 *  Function    -  track_cust_alarm_door
 *
 *  Purpose     -  �ű���
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 02-08-2013,  jwj  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_alarm_door(void)
{
    track_gps_data_struct *gpsLastPoint = NULL;
    LOGD(L_APP, L_V5, "");

    if(G_parameter.door_alarm.sw == 0)
    {
        return;
    }


#if defined (__NT37__)
    if(track_cust_door_alarm_enable(0xFF)==0)
    {
        LOGD(L_APP, L_V5, "����");
        return;
    }
#endif

    gpsLastPoint = track_cust_gpsdata_alarm();
    if(track_nvram_alone_parameter_read()->gprson)
    {
        track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_door, KAL_FALSE, 0);
    }
    if(G_parameter.door_alarm.alarm_type == 1)
    {
        door_sendsms();
    }
    if(G_parameter.door_alarm.alarm_type == 2)
    {
        door_sendsms();
        track_start_timer(TRACK_CUST_ALARM_DOOR_Call_TIMER, 60000, track_cust_make_call, (void*)1);
    }
}

//��б���
void track_cust_teardown_alarm()
{
    struct_msg_send g_msg = {0};
    LOGD(L_APP, L_V5, "��ж��ŷ���");
    g_msg.addUrl = KAL_FALSE;
    g_msg.cm_type = CM_SMS;
    snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_teardown_en);
    g_msg.msg_cn_len = 20;
    memcpy(g_msg.msg_cn, str_teardown_cn, g_msg.msg_cn_len);
    track_cust_module_alarm_msg_send(&g_msg, NULL);
    //track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_PRE_REMOVE);
}
//��ײ����
void track_cust_collision_alarm()
{
	struct_msg_send g_msg = {0};
    LOGD(L_APP, L_V5, "��ײ�������ŷ���");
    g_msg.addUrl = KAL_FALSE;
    g_msg.cm_type = CM_SMS;
    snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_collision_en);
    g_msg.msg_cn_len = 18;
    memcpy(g_msg.msg_cn, str_collision_cn, g_msg.msg_cn_len);
    track_cust_module_alarm_msg_send(&g_msg, NULL);
}
//��б����
void track_cust_tilt_alarm()
{
	struct_msg_send g_msg = {0};
    LOGD(L_APP, L_V5, "��б�������ŷ���");
    g_msg.addUrl = KAL_FALSE;
    g_msg.cm_type = CM_SMS;
    snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_tiltalm_en);
    g_msg.msg_cn_len = 16;
    memcpy(g_msg.msg_cn, str_tiltalm_cn, g_msg.msg_cn_len);
    track_cust_module_alarm_msg_send(&g_msg, NULL);
}
void teardown_alarm_overtime(void)
{
    track_gps_data_struct *  gps_data = NULL;
    if(track_cust_gps_status() > 2)
    {
        gps_data = track_cust_backup_gps_data(2, NULL);
    }
    if(G_parameter.teardown_str.sw)
    {
        if(!G_parameter.teardown_str.alarm_type || G_parameter.teardown_str.alarm_type == 3)
        {
            LOGD(L_APP, L_V4, "���𱨾����������������������֪ͨ��");
            return;
        }
        memset(&g_msg, 0, sizeof(struct_msg_send));
        g_msg.addUrl = KAL_TRUE;
        g_msg.cm_type = CM_SMS;
        track_fun_strncpy(g_msg.msg_en, str_teardown_en, CM_PARAM_LONG_LEN_MAX);
        g_msg.msg_cn_len = 20;//sizeof(str_teardown_cn);
        memcpy(g_msg.msg_cn, str_teardown_cn, g_msg.msg_cn_len);
        track_cust_module_alarm_msg_send(&g_msg, gps_data);
        track_cust_module_alarm_lock(TR_CUST_ALARM_PRE_REMOVE, 0);
    }
    else
    {
        track_cust_module_alarm_lock(TR_CUST_ALARM_PRE_REMOVE, 0);
    }
    //track_cust_module_alarm_set_type(0);
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_PRE_REMOVE);
#if defined(__GT420D__)
            if((track_cust_gps_status()==0)&&track_cust_wifi_scan_status()==0)
        {
               LOGD(L_APP, L_V4, "GPS,wifi�ѹر�30s��ɹػ�");
             track_cust_module_delay_close(30);
        }
#endif
}


#if defined (__GT370__)|| defined (__GT380__)
void track_cust_receive_power_off_msg(void)
{
    kal_uint32 index = 0;
    LOGD(L_APP, L_V5, "");

    if(!track_is_timer_run(TRACK_CUST_PWROFFALM_TIMER_ID))
    {
        if(G_parameter.pwrlimit_sw == 0)
        {
            if(G_parameter.pwroff.sw == 1)
            {
                track_cust_led_sleep(2);
                index = track_cust_check_sosnum();
                track_cust_alarm_pwoffalm();
                if(G_parameter.pwroff.alarm_type == 1 && index != 0)
                {
                    tr_start_timer(TRACK_CUST_PWROFFALM_TIMER_ID, (index * 15) * 1000, track_cust_poweroff_ready);
                }
                else
                {
                    tr_start_timer(TRACK_CUST_PWROFFALM_TIMER_ID, 5 * 1000, track_cust_poweroff_ready);
                }
            }
            else
            {
                tr_start_timer(TRACK_CUST_PWROFFALM_TIMER_ID, 5 * 1000, track_cust_poweroff_ready);
            }
        }
        else
        {
            LOGD(L_APP, L_V5, "���ƹػ�!");
        }
    }
    else
    {
        LOGD(L_APP, L_V5, "�ػ�������");
    }
}

//����Ǳ���
void track_cust_open_shell_alarm()
{
    struct_msg_send g_msg = {0};
    LOGD(L_APP, L_V5, "���Ǳ������ŷ���");
    g_msg.addUrl = KAL_FALSE;
    g_msg.cm_type = CM_SMS;
    snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_open_shell_en);
    g_msg.msg_cn_len = 18;
    memcpy(g_msg.msg_cn, str_open_shell_cn, g_msg.msg_cn_len);
    track_cust_module_alarm_msg_send(&g_msg, NULL);
}

void track_open_shell_alarm_overtime(void)
{
    track_gps_data_struct *  gps_data = NULL;
    if(track_cust_gps_status() > 2)
    {
        gps_data = track_cust_backup_gps_data(2, NULL);
    }
    if(G_parameter.open_shell_alarm.sw)
    {
        if(!G_parameter.open_shell_alarm.alarm_type || G_parameter.open_shell_alarm.alarm_type == 3)
        {
            LOGD(L_APP, L_V4, "���Ǳ������������������������֪ͨ��");
            return;
        }
        memset(&g_msg, 0, sizeof(struct_msg_send));
        g_msg.addUrl = KAL_TRUE;
        g_msg.cm_type = CM_SMS;
        track_fun_strncpy(g_msg.msg_en, str_open_shell_en, CM_PARAM_LONG_LEN_MAX);
        g_msg.msg_cn_len = 18;
        memcpy(g_msg.msg_cn, str_open_shell_cn, g_msg.msg_cn_len);
        track_cust_module_alarm_msg_send(&g_msg, gps_data);
        track_cust_module_alarm_lock(TR_CUST_ALARM_OPEN_SHELL, 0);
    }
    else
    {
        track_cust_module_alarm_lock(TR_CUST_ALARM_OPEN_SHELL, 0);
    }
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_OPEN_SHELL);
}

#endif

