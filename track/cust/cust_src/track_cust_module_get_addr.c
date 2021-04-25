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
#include "track_os_timer.h"
#include "track_os_data_types.h"
/*****************************************************************************
*  Define                               �궨��
******************************************************************************/
#if defined(__GW100__)
#define DW_WHERE_URL_GPS_WTIME  3*1000*60
#elif defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)|| defined(__GT310__)
#define DW_WHERE_URL_GPS_WTIME  2*1000*60
#else
#define DW_WHERE_URL_GPS_WTIME  5*1000*60
#endif /* __GW100__ */

/*****************************************************************************
*  Typedef  Enum                        ö�ٶ���
*****************************************************************************/

/*****************************************************************************
*  Struct                               ���ݽṹ����
******************************************************************************/

/*****************************************************************************
*  Local variable                       �ֲ�����
*****************************************************************************/
static kal_uint8 msg1[26] =    // δ�鵽���ݣ����Ժ����ԣ�
{
    0x67, 0x2A, 0x67, 0xE5, 0x52, 0x30, 0x65, 0x70, 0x63, 0x6E, 0xFF, 0x0C, 0x8B, 0xF7, 0x7A, 0x0D,
    0x54, 0x0E, 0x91, 0xCD, 0x8B, 0xD5, 0xFF, 0x01,0x00,0x00
};

#if defined(__GERMANY__)
static char *msg2 = "Keine GPS-Daten!";
#else
static char *msg2 = "NO DATA!";
#endif

//δʹ�� chengj
static kal_uint8 msg3[32] =    // ��ǰGPSδ��λ�����Ժ����ԣ�
{
    0x5F, 0x53, 0x52, 0x4D, 0x00, 0x47, 0x00, 0x50, 0x00, 0x53, 0x67, 0x2A, 0x5B, 0x9A, 0x4F, 0x4D,
    0xFF, 0x0C, 0x8B, 0xF7, 0x7A, 0x0D, 0x54, 0x0E, 0x91, 0xCD, 0x8B, 0xD5, 0xFF, 0x01,0x00,0x00
};

#if defined (__TRACK_TYPE_GPS_BEIDOU__)
static kal_uint8 msg6[] = //��ǰBD/GPSδ��λ�����Ժ����ԣ�
{
    0x5F, 0x53, 0x52, 0x4D, 0x00, 0x42, 0x00, 0x44, 0x00, 0x2F, 0x00, 0x47, 0x00, 0x50, 0x00, 0x53, 0x67, 0x2A, 0x5B , 0x9A , 
    0x4F , 0x4D , 0xFF , 0x0C , 0x8B , 0xF7 , 0x7A , 0x0D , 0x50 , 0x19 , 0x51 , 0x8D , 0x8B , 0xD5 , 0xFF , 0x01,0x00,0x00
};
#else
static kal_uint8 msg6[24] =    //��ȡλ��ʧ�ܣ������ԣ�
{
    0x83 , 0xB7 , 0x53 , 0xD6 , 0x4F , 0x4D , 0x7F , 0x6E, 0x59 , 0x31 , 0x8D , 0x25 , 0xFF , 0x0C , 0x8B , 0xF7 , 0x91, 0xCD , 0x8B , 0xD5 , 0x00 , 0x21,0x00,0x00
};
#endif
#if defined(__GERMANY__)
static char *msg4 = "GPS nicht erkannt! Bitte erneut versuchen!";
#else
#if defined (__TRACK_TYPE_GPS_BEIDOU__)
static char *msg4 = "BD/GPS not fixed, please wait for a while, and then try again.";
#else
static char *msg4 = "GPS not fixed, please wait for a while, and then try again.";
#endif
#endif

static char *msg7 = "Error!Only SOS phone numbers are allowed to query.";
static char *msg5 = "Failed to get to the location, please try again later!";
static kal_uint16  lbs_sn = 0xffff;
static kal_uint16  gps_sn =  0xffff;
/****************************************************************************
*  Global Variable                      ȫ�ֱ���
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/
extern void track_cust_gps_location_timeout(kal_uint32 time);
extern void track_cust_set_dw_type(CMD_DATA_STRUCT * _cmd);
/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/
char *track_cust_get_url_str(char *dest, gps_gprmc_struct *gpsdata);
extern kal_uint32 track_cust_get_overtime_address(kal_uint32 flag, kal_uint32 time);

#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/

/******************************************************************************
 *  Function    -  get_url
 *
 *  Purpose     -  �����Ƕ��Ż������ߣ��ֻ��Ǵ���ָ��ظ�
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void get_url(CMD_DATA_STRUCT *cmd, track_gps_data_struct *gpsLastPoint)
{
    static char msg[320];
    memset(msg, 0, 320);
    // ��γ��λ�õ� Google ����
    track_cust_get_url_str(msg, &gpsLastPoint->gprmc);
    if(cmd->return_sms.cm_type == CM_SMS && strlen(cmd->return_sms.ph_num) > 0)
    {
        track_cust_sms_send(cmd->return_sms.num_type, cmd->return_sms.ph_num, 1, msg, strlen(msg));
    }
    else if(cmd->return_sms.cm_type == CM_ONLINE)
    {
        track_cust_paket_msg_upload(cmd->return_sms.stamp, KAL_TRUE, msg, strlen(msg));
    }
    else if(cmd->return_sms.cm_type == CM_ONLINE2)
    {
        track_cust_paket_FD_cmd(cmd->return_sms.stamp, KAL_TRUE, msg, strlen(msg));
    }
    else
    {
        track_cmd_send_rsp(cmd->return_sms.cm_type, strlen(msg), msg, 0);
    }
}

void track_cust_send_paket_1a(kal_uint32 flag, char* ph_num)
{
    U16 sn;
    track_gps_data_struct *gpsLastPoint = NULL;
    static kal_uint32 status = 0;
    static char num[CM_HEAD_PWD_MXA];
    static char msg[320];
    memset(msg, 0, 320);
    // ��γ��λ�õ� Google ����
    if(flag == 1 && status == 0)
    {

    }
    else
    {
        LOGD(L_APP, L_V5, "flag = %d;status = %d", flag, status);
    }

    if(flag == 0)
    {
        status = 0;
        memset(num, 0, CM_HEAD_PWD_MXA);
    }
    else if(flag == 1 &&  status == 1)
    {

        if(track_soc_login_status() == 1)
        {
            gpsLastPoint = track_cust_backup_gps_data(0, NULL);
            track_cust_get_url_str(msg, &gpsLastPoint->gprmc);
            sn = track_cust_paket_1A(gpsLastPoint, num);
            gps_sn = sn;
            status = 0;
        }
    }
    else if(flag == 2)
    {
        memset(num, 0, CM_HEAD_PWD_MXA);
        memcpy(num, ph_num, CM_HEAD_PWD_MXA);
        status = 1;
    }
}


/******************************************************************************
 *  Function    -  Qurey_Get_Address_Fail_SendSMS
 *
 *  Purpose     -  ȷ����ȡ����γ�ȵ����ӵ�ַ
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
#if defined(__GT500__)||defined(__GT310__)||defined(__GT03F__) || defined(__GW100__)|| defined(__V20__) || defined(__ET310__)||defined(__GT300S__)||defined (__ET320__)
static void Qurey_Get_Address_Fail_SendSMS(void * par)
{
    track_gps_data_struct *gpsLastPoint = NULL;
    CMD_DATA_STRUCT *cmd = (CMD_DATA_STRUCT*)par;
    char msg[320] = {0};

    LOGD(L_APP, L_V5, "");
    gpsLastPoint = track_cust_backup_gps_data(0, NULL);
    // ����Ч����һ����λ��
#if defined (__GW100__)||defined(__GT310__)
    if(lbs_sn == 0XFFFE)//lbs�Ѿ�
    {
        if(g_GT03D)
        {
            track_cust_send_paket_1a(0, NULL);
        }
        track_stop_timer(TRACK_CUST_DW_GPS_WORK_TIMER);
        track_cust_gps_location_timeout(10);
        track_cust_get_overtime_address(2, NULL);
        lbs_sn = 0;
        gps_sn = 0;
        return;
    }
    lbs_sn = 0;
    gps_sn = 0;
#endif /* __GW100__ */
    if(track_cust_gps_status() > 2)
    {
        get_url(cmd, gpsLastPoint);
    }
    else if(cmd->return_sms.cm_type == CM_SMS && strlen(cmd->return_sms.ph_num) > 0)
    {
        if(G_parameter.lang == 1)
        {
            track_cust_sms_send(cmd->return_sms.num_type, cmd->return_sms.ph_num, 0, msg6, sizeof(msg6));
        }
        else
        {
            track_cust_sms_send(cmd->return_sms.num_type, cmd->return_sms.ph_num, 1, msg5, strlen(msg5));
        }
    }
    else if(cmd->return_sms.cm_type == CM_ONLINE)
    {
        track_cust_paket_msg_upload(cmd->return_sms.stamp, KAL_TRUE, msg5, strlen(msg5));
    }
    else if(cmd->return_sms.cm_type == CM_ONLINE2)
    {
        track_cust_paket_FD_cmd(cmd->return_sms.stamp, KAL_TRUE, msg5, strlen(msg5));
    }
    else
    {
        track_cmd_send_rsp(cmd->return_sms.cm_type, strlen(msg5), msg5, 0);
    }

    if(g_GT03D)
    {
        track_cust_send_paket_1a(0, NULL);
    }
    track_stop_timer(TRACK_CUST_DW_GPS_WORK_TIMER);
    track_cust_gps_location_timeout(10);
    track_cust_get_overtime_address(2, NULL);
}

#else
static void Qurey_Get_Address_Fail_SendSMS(void* par)
{
    track_gps_data_struct *gpsLastPoint = NULL;
    CMD_DATA_STRUCT *cmd = (CMD_DATA_STRUCT*)par;
    char msg[320] = {0};

    LOGD(L_APP, L_V5, "");
    gpsLastPoint = track_cust_backup_gps_data(0, NULL);
    if(gpsLastPoint == NULL || !gpsLastPoint->gprmc.status)
    {
        // ����Ч����һ����λ��
        if(cmd->return_sms.cm_type == CM_SMS && strlen(cmd->return_sms.ph_num) > 0)
        {
            if(G_parameter.lang == 1)
            {
                track_cust_sms_send(cmd->return_sms.num_type, cmd->return_sms.ph_num, 0, msg1, 24);
            }
            else
            {
                track_cust_sms_send(cmd->return_sms.num_type, cmd->return_sms.ph_num, 1, msg2, strlen(msg2));
            }
        }
        else if(cmd->return_sms.cm_type == CM_ONLINE)
        {
            track_cust_paket_msg_upload(cmd->return_sms.stamp, KAL_TRUE, msg2, strlen(msg2));
        }
        else if(cmd->return_sms.cm_type == CM_ONLINE2)
        {
            track_cust_paket_FD_cmd(cmd->return_sms.stamp, KAL_TRUE, msg2, strlen(msg2));
        }
        else
        {
            track_cmd_send_rsp(cmd->return_sms.cm_type, strlen(msg2), msg2, 0);
        }
        return;
    }

    // ��γ��λ�õ� Google ����
    if(gpsLastPoint != NULL)
    {
        get_url(cmd, gpsLastPoint);
    }

}
#endif /* __GT300 */

#if defined (__GT310__)
static void Qurey_Get_Address_Fail_SendSMS2(void* par)
{
    track_gps_data_struct *gpsLastPoint = NULL;
    CMD_DATA_STRUCT *cmd = (CMD_DATA_STRUCT*)par;
    char msg[320] = {0};
    static char num[CM_HEAD_PWD_MXA];
    char sta_num[CM_HEAD_PWD_MXA + 1] = {0};
    LOGD(L_APP, L_V5, "");


    track_stop_timer(TRACK_CUST_GET_ADDRESS_OVER3_TIMER_ID);
    track_stop_timer(TRACK_CUST_GET_ADDRESS_OVER4_TIMER_ID);
    gpsLastPoint = track_cust_backup_gps_data(0, NULL);
    LOGD(L_APP, L_V5, "cmd->return_sms.num_type = %d", cmd->return_sms.num_type);
    track_fun_cov_number(cmd->return_sms.num_type, cmd->return_sms.ph_num, sta_num, CM_HEAD_PWD_MXA);

    get_url(cmd, gpsLastPoint);
    track_stop_timer(TRACK_CUST_DW_GPS_WORK_TIMER);
    track_stop_timer(TRACK_CUST_GET_ADDRESS_OVER_TIMER_ID);
    //track_cust_gps_location_timeout(10);
    track_cust_get_overtime_address(2, NULL);

    track_cust_send_paket_1a(0, NULL);
}
#else
static void Qurey_Get_Address_Fail_SendSMS2(void* par)
{
    track_gps_data_struct *gpsLastPoint = NULL;
    CMD_DATA_STRUCT *cmd = (CMD_DATA_STRUCT*)par;
    char msg[320] = {0};
    static char num[CM_HEAD_PWD_MXA];
    char sta_num[CM_HEAD_PWD_MXA + 1] = {0};
    LOGD(L_APP, L_V5, "");


    track_stop_timer(TRACK_CUST_GET_ADDRESS_OVER3_TIMER_ID);
    track_stop_timer(TRACK_CUST_GET_ADDRESS_OVER4_TIMER_ID);
    gpsLastPoint = track_cust_backup_gps_data(0, NULL);
    LOGD(L_APP, L_V5, "cmd->return_sms.num_type = %d", cmd->return_sms.num_type);
    track_fun_cov_number(cmd->return_sms.num_type, cmd->return_sms.ph_num, sta_num, CM_HEAD_PWD_MXA);
    if(track_cust_gps_status() < 3 && track_soc_login_status())
    {
        lbs_sn = track_cust_paket_17(track_drv_get_lbs_data(), sta_num);
    }
    else if(track_cust_gps_status() > 2)
    {
        get_url(cmd, gpsLastPoint);
        track_stop_timer(TRACK_CUST_DW_GPS_WORK_TIMER);
        track_stop_timer(TRACK_CUST_GET_ADDRESS_OVER_TIMER_ID);
        track_cust_gps_location_timeout(10);
        track_cust_get_overtime_address(2, NULL);
    }
    else if(track_cust_gps_status() < 3)
    {
        if(cmd->return_sms.cm_type == CM_SMS && strlen(cmd->return_sms.ph_num) > 0)
        {
            if(G_parameter.lang == 1)
            {
                track_cust_sms_send(cmd->return_sms.num_type, cmd->return_sms.ph_num, 0, msg6, sizeof(msg6));
            }
            else
            {
                track_cust_sms_send(cmd->return_sms.num_type, cmd->return_sms.ph_num, 1, msg5, strlen(msg5));
            }
        }
        else if(cmd->return_sms.cm_type == CM_ONLINE)
        {
            track_cust_paket_msg_upload(cmd->return_sms.stamp, KAL_TRUE, msg5, strlen(msg5));
        }
        else if(cmd->return_sms.cm_type == CM_ONLINE2)
        {
            track_cust_paket_FD_cmd(cmd->return_sms.stamp, KAL_TRUE, msg5, strlen(msg5));
        }
        else
        {
            track_cmd_send_rsp(cmd->return_sms.cm_type, strlen(msg5), msg5, 0);
        }
        track_stop_timer(TRACK_CUST_DW_GPS_WORK_TIMER);
        track_stop_timer(TRACK_CUST_GET_ADDRESS_OVER_TIMER_ID);
        track_cust_gps_location_timeout(10);
        track_cust_get_overtime_address(2, NULL);
    }
    track_cust_send_paket_1a(0, NULL);
}
#endif
/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/

/******************************************************************************
 *  Function    -  track_cust_get_url_str
 *
 *  Purpose     -  ��ȡ��γ�����ӵ��ַ�����ʽ
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
char *track_cust_get_url_str(char * dest, gps_gprmc_struct * gprmc)
{
    applib_time_struct time = {0};
    /* GPS�����е�ʱ�����ʱ��У�� */
    if(track_cust_is_upload_UTC_time())
    {
        track_drv_utc_timezone_to_rtc(&time, (applib_time_struct*)&gprmc->Date_Time, G_parameter.zone);
    }
    else
    {
        memcpy(&time, &gprmc->Date_Time, sizeof(applib_time_struct));
    }
#if defined(__GERMANY__)
        snprintf(dest, 160, " %s%c%.6f%,%c%.6f",             
             G_parameter.url, gprmc->NS, gprmc->Latitude, gprmc->EW, gprmc->Longitude);

#else
    snprintf(dest, 160, "<%.2d-%.2d %.2d:%.2d>%s%c%.6f%,%c%.6f",
             time.nMonth, time.nDay, time.nHour, time.nMin,
             G_parameter.url, gprmc->NS, gprmc->Latitude, gprmc->EW, gprmc->Longitude);
#endif
    return dest;
}

void get_where(CMD_DATA_STRUCT * _cmd)
{
    applib_time_struct time = {0};
    track_gps_data_struct *gpsLastPoint = NULL;
    static CMD_DATA_STRUCT cmd = {0};
    char msg[320] = {0};
    memcpy(&cmd, _cmd, sizeof(CMD_DATA_STRUCT));
    gpsLastPoint = track_cust_backup_gps_data(0, NULL);
    /* GPS�����е�ʱ�����ʱ��У�� */
    if(gpsLastPoint == NULL || !gpsLastPoint->gprmc.status)
    {
        // ����Ч����һ����λ��
        if(cmd.return_sms.cm_type == CM_SMS && strlen(cmd.return_sms.ph_num) > 0)
        {
#if defined (__GT02D__)
            if(G_parameter.lang == 1)
            {
                track_cust_sms_send(cmd.return_sms.num_type, cmd.return_sms.ph_num, 0, msg1, 24);
            }
            else
#endif /* __GT02D__ */
            {
                track_cust_sms_send(cmd.return_sms.num_type, cmd.return_sms.ph_num, 1, msg2, strlen(msg2));
            }
        }
        else if(cmd.return_sms.cm_type == CM_ONLINE)
        {
            track_cust_paket_msg_upload(cmd.return_sms.stamp, KAL_TRUE, msg2, strlen(msg2));
        }
        else if(cmd.return_sms.cm_type == CM_ONLINE2)
        {
            track_cust_paket_FD_cmd(cmd.return_sms.stamp, KAL_TRUE, msg2, strlen(msg2));
        }
        else
        {
            track_cmd_send_rsp(cmd.return_sms.cm_type, strlen(msg2), msg2, 0);
        }
        return;
    }

    if(track_cust_is_upload_UTC_time())
    {
        track_drv_utc_timezone_to_rtc(&time, (applib_time_struct*)&gpsLastPoint->gprmc.Date_Time, G_parameter.zone);
    }
    else
    {
        memcpy(&time, &gpsLastPoint->gprmc.Date_Time, sizeof(applib_time_struct));
    }
    // ��γ��λ�ò�ѯ
    if(track_cust_gps_status() >= GPS_STAUS_2D)
    {
#if defined(__GERMANY__)        
        sprintf(msg, "Aktuelle Position: Lat:%c%f,Lon:%c%f,Richtung:%.2f,Geschwindigkeit:%.2fKm/h",
                        gpsLastPoint->gprmc.NS, gpsLastPoint->gprmc.Latitude, gpsLastPoint->gprmc.EW, gpsLastPoint->gprmc.Longitude,
                        gpsLastPoint->gprmc.Course, gpsLastPoint->gprmc.Speed);
#else
        sprintf(msg, "Current position! Lat:%c%f,Lon:%c%f,Course:%.2f,Speed:%.2fKm/h,DateTime:%.2d-%.2d-%.2d %.2d:%.2d:%.2d",
                gpsLastPoint->gprmc.NS, gpsLastPoint->gprmc.Latitude, gpsLastPoint->gprmc.EW, gpsLastPoint->gprmc.Longitude,
                gpsLastPoint->gprmc.Course, gpsLastPoint->gprmc.Speed, time.nYear,
                time.nMonth, time.nDay, time.nHour,
                time.nMin, time.nSec);
#endif        
    }
    else
    {
#if defined(__GERMANY__)
                 sprintf(msg, "Letzte Position: Lat:%c%f,Lon:%c%f,Richtung:%.2f,Geschwindigkeit:%.2fKm/h",
                        gpsLastPoint->gprmc.NS, gpsLastPoint->gprmc.Latitude, gpsLastPoint->gprmc.EW, gpsLastPoint->gprmc.Longitude,
                        gpsLastPoint->gprmc.Course, gpsLastPoint->gprmc.Speed
                        );
#else
        sprintf(msg, "Last position! Lat:%c%f,Lon:%c%f,Course:%.2f,Speed:%.2fKm/h,DateTime:%.2d-%.2d-%.2d %.2d:%.2d:%.2d",
                gpsLastPoint->gprmc.NS, gpsLastPoint->gprmc.Latitude, gpsLastPoint->gprmc.EW, gpsLastPoint->gprmc.Longitude,
                gpsLastPoint->gprmc.Course, gpsLastPoint->gprmc.Speed, time.nYear,
                time.nMonth, time.nDay, time.nHour,
                time.nMin, time.nSec);
#endif
    }
    if(cmd.return_sms.cm_type == CM_SMS && strlen(cmd.return_sms.ph_num) > 0)
    {
        track_cust_sms_send(cmd.return_sms.num_type, cmd.return_sms.ph_num, 1, msg, strlen(msg));
    }
    else if(cmd.return_sms.cm_type == CM_ONLINE)
    {
        track_cust_paket_msg_upload(cmd.return_sms.stamp, KAL_TRUE, msg, strlen(msg));
    }
    else if(cmd.return_sms.cm_type == CM_ONLINE2)
    {
        track_cust_paket_FD_cmd(cmd.return_sms.stamp, KAL_TRUE, msg, strlen(msg));
    }
    else
    {
        track_cmd_send_rsp(cmd.return_sms.cm_type, strlen(msg), msg, 0);
    }
}

/******************************************************************************
 *  Function    -  track_cust_gt03_url
 *
 *  Purpose     -  URL��ȡ��ַ����
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  JWJ WQ  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_gt03_url(CMD_DATA_STRUCT * _cmd)
{
    applib_time_struct time = {0};
    track_gps_data_struct *gpsLastPoint = NULL;
    static CMD_DATA_STRUCT cmd = {0};
    kal_uint32 par = (kal_uint32)_cmd;
    LOGD(L_APP, L_V5, "%s", cmd.return_sms.ph_num);
    if(par != 1)
    {
        memcpy(&cmd, _cmd, sizeof(CMD_DATA_STRUCT));
    }
    gpsLastPoint = track_cust_backup_gps_data(0, NULL);
    if(track_cust_gps_status() > 2)
    {
        track_stop_timer(TRACK_CUST_URL_OVER_TIMER_ID);
        get_url(&cmd, gpsLastPoint);
        track_cust_gps_location_timeout(20);
    }
    else if(par == 1)
    {
        if(gpsLastPoint == NULL || !gpsLastPoint->gprmc.status)
        {
            // ����Ч����һ����λ��
            if(cmd.return_sms.cm_type == CM_SMS && strlen(cmd.return_sms.ph_num) > 0)
            {
#if defined (__GT02D__)
                if(G_parameter.lang == 1)
                {
                    track_cust_sms_send(cmd.return_sms.num_type, cmd.return_sms.ph_num, 0, msg1, 24);
                }
                else
#endif /* __GT02D__ */
                {
                    track_cust_sms_send(cmd.return_sms.num_type, cmd.return_sms.ph_num, 1, msg2, strlen(msg2));
                }
            }
            else if(cmd.return_sms.cm_type == CM_ONLINE)
            {
                track_cust_paket_msg_upload(cmd.return_sms.stamp, KAL_TRUE, msg2, strlen(msg2));
            }
            else if(cmd.return_sms.cm_type == CM_ONLINE2)
            {
                track_cust_paket_FD_cmd(cmd.return_sms.stamp, KAL_TRUE, msg2, strlen(msg2));
            }
            else
            {
                track_cmd_send_rsp(cmd.return_sms.cm_type, strlen(msg2), msg2, 0);
            }
            return;
        }
        else
        {
            get_url(&cmd, gpsLastPoint);
        }
    }
    else
    {
        track_cust_gps_work_req((void*)11);
        track_start_timer(TRACK_CUST_URL_OVER_TIMER_ID, DW_WHERE_URL_GPS_WTIME, track_cust_gt03_url, (void*)1);
    }
}

/******************************************************************************
 *  Function    -  track_cust_gt03_where
 *
 *  Purpose     -  where��ȡ��ַ����
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  JWJ WQ  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_gt03_where(CMD_DATA_STRUCT * _cmd)
{
    applib_time_struct time = {0};
    track_gps_data_struct *gpsLastPoint = NULL;
    static CMD_DATA_STRUCT cmd = {0};
    kal_uint32 par = (kal_uint32)_cmd;
    if(par != 1)
    {
        memcpy(&cmd, _cmd, sizeof(CMD_DATA_STRUCT));
    }
    if(track_cust_gps_status() > 2)
    {
        track_stop_timer(TRACK_CUST_WHERE_OVER_TIMER_ID);
        get_where(&cmd);
        track_cust_gps_location_timeout(20);
    }
    else if(par == 1)
    {
        get_where(&cmd);
    }
    else
    {
        track_cust_gps_work_req((void*)11);
        track_start_timer(TRACK_CUST_WHERE_OVER_TIMER_ID, DW_WHERE_URL_GPS_WTIME, track_cust_gt03_where, (void*)1);
    }

}

/******************************************************************************
 *  Function    -  track_cust_gt03_dw
 *
 *  Purpose     -  dw��ȡ��ַ����
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  JWJ WQ  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_gt03_dw(CMD_DATA_STRUCT * _cmd)
{
    track_gps_data_struct *gpsLastPoint = NULL;
    static CMD_DATA_STRUCT cmd = {0};
    char sta_num[CM_HEAD_PWD_MXA + 1] = {0};
    static char msg5_cn[48] =    // ָ���æ����ʱ�޷���������ָ����Ժ����ԣ�
    {
        0x63, 0x07, 0x4E, 0xE4, 0x59, 0x04, 0x74, 0x06, 0x7E, 0x41, 0x5F, 0xD9, 0xFF, 0x0C, 0x66, 0x82,
        0x65, 0xF6, 0x65, 0xE0, 0x6C, 0xD5, 0x59, 0x04, 0x74, 0x06, 0x60, 0xA8, 0x76, 0x84, 0x63, 0x07,
        0x4E, 0xE4, 0xFF, 0x0C, 0x8B, 0xF7, 0x7A, 0x0D, 0x50, 0x19, 0x51, 0x8D, 0x8B, 0xD5, 0xFF, 0x01
    };
    static char *msg5_en = "The command processing busy, unable to process your command, please try again later!";

    if(track_is_timer_run(TRACK_CUST_GET_ADDRESS_OVER_TIMER_ID) == TRUE)
    {
        LOGD(L_APP, L_V5, "ָ���æ����ʱ�޷���������ָ����Ժ����ԣ�");
        if(_cmd->return_sms.cm_type == CM_SMS && strlen(_cmd->return_sms.ph_num) > 0)
        {
            if(G_parameter.lang == 1)
            {
                track_cust_sms_send(_cmd->return_sms.num_type, _cmd->return_sms.ph_num, 0, msg5_cn, 48);
            }
            else
            {
                track_cust_sms_send(_cmd->return_sms.num_type, _cmd->return_sms.ph_num, 1, sta_num, strlen(sta_num));
            }
        }
        else if(_cmd->return_sms.cm_type == CM_ONLINE)
        {
            track_cust_paket_msg_upload(_cmd->return_sms.stamp, KAL_TRUE, sta_num, strlen(sta_num));
        }
        else if(_cmd->return_sms.cm_type == CM_ONLINE2)
        {
            track_cust_paket_FD_cmd(_cmd->return_sms.stamp, KAL_TRUE, sta_num, strlen(sta_num));
        }
        return;
    }
    gpsLastPoint = track_cust_backup_gps_data(0, NULL);
    memcpy(&cmd, _cmd, sizeof(CMD_DATA_STRUCT));
    track_cust_set_dw_type(&cmd);//���浱ǰλ�ò�ѯָ��
    track_fun_cov_number(cmd.return_sms.num_type, cmd.return_sms.ph_num, sta_num, CM_HEAD_PWD_MXA);
    LOGD(L_APP, L_V5, "%s:%s", cmd.return_sms.ph_num, sta_num);
    track_cust_gprson_status(1);
    track_cust_get_overtime_address(1, 360);
    track_cust_send_paket_1a(2, sta_num);
    track_cust_gps_work_req((void*)5);
    if(track_cust_get_work_mode() == WORK_MODE_3)
    {
        track_soc_gprs_reconnect((void*)306);
    }
#ifndef __GW100__
    track_start_timer(TRACK_CUST_GET_ADDRESS_OVER_TIMER_ID, DW_WHERE_URL_GPS_WTIME+60000, Qurey_Get_Address_Fail_SendSMS, (void*)&cmd);
    track_start_timer(TRACK_CUST_GET_ADDRESS_OVER3_TIMER_ID, DW_WHERE_URL_GPS_WTIME, Qurey_Get_Address_Fail_SendSMS2, (void*)&cmd);
#else
    if(track_cust_gps_status() <= 2 && track_soc_login_status())
    {
        lbs_sn = track_cust_paket_17(track_drv_get_lbs_data(), sta_num);
    }
    track_start_timer(TRACK_CUST_GET_ADDRESS_OVER_TIMER_ID, DW_WHERE_URL_GPS_WTIME + 20 * 1000, Qurey_Get_Address_Fail_SendSMS, (void*)&cmd);
#endif /* __GW100__ */

}

void lbs_cmd_send(CMD_DATA_STRUCT *cmd)
{
    static CMD_DATA_STRUCT _cmd={0};
    memcpy(&_cmd, cmd,  sizeof(CMD_DATA_STRUCT));
    track_cust_paket_18(track_drv_get_lbs_data(), 0);
    track_cust_paket_17(track_drv_get_lbs_data(), _cmd.return_sms.ph_num);
    track_start_timer(TRACK_CUST_GET_ADDRESS_OVER_TIMER_ID, 30 * 1000, Qurey_Get_Address_Fail_SendSMS, (void*)&_cmd);
}
#if !defined(__GT420D__)
/******************************************************************************
 *  Function    -  track_cust_module_get_addr
 *
 *  Purpose     -  �����ǻ�ȡ���ĵ�ַ��������
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_get_addr(CMD_DATA_STRUCT * _cmd)
{
    static CMD_DATA_STRUCT cmd = {0};
    static char cmdstr[20];
    kal_bool gpsLastPoint_valid = KAL_TRUE;
    track_gps_data_struct *gpsLastPoint = NULL;
    char msg[320] = {0};
    kal_uint8 gps_status = track_cust_gps_status();
    kal_uint8 mode = 0;
#if defined(__GT310__)||defined(__GT03F__)||defined(__GT500__) || defined(__GW100__)||defined(__GT300S__)||defined(__GT370__)||defined(__GT380__)
    {
        memcpy(&cmd, _cmd, sizeof(CMD_DATA_STRUCT));

#if defined(__GT310__)||defined(__GT03F__) || defined(__GW100__)||defined(__GT300S__)||defined(__GT370__)||defined(__GT380__)
        if(_cmd->return_sms.cm_type == CM_SMS && (track_cust_sos_or_center_check(2, _cmd->return_sms.ph_num) % 8) != 0 && _cmd->supercmd != 1)
        {
            //   track_cmd_send_rsp(_cmd->return_sms.cm_type, strlen(msg7), msg7, 0);
            track_cust_sms_send(_cmd->return_sms.num_type, _cmd->return_sms.ph_num, 1, msg7, strlen(msg7));
            LOGD(L_APP, L_V5, "%s", msg7);
            return;
        }
#endif /* __GT300__ */
        if((!strcmp(_cmd->pars[0], "DW") || !strcmp(_cmd->pars[0], "POSITION") || !strcmp(_cmd->pars[0], "123")))
        {
            track_cust_gt03_dw(&cmd);
        }
        else if(!strcmp(_cmd->pars[0], "WHERE") || !strcmp(_cmd->pars[0], "DWXX"))
        {
            track_cust_gt03_where(_cmd);
        }
        else  if(!strcmp(_cmd->pars[0], "URL"))
        {
            track_cust_gt03_url(_cmd);
        }
        return;
    }
#endif
//ʹ������λ�ò�ѯ���ܵ���Ŀ��Ҫ��track_cust_gps_work_req((void*)5)�����м����Լ��ĺ꣬��Ϊ5���ӷ���ʱ����

    LOGD(L_APP, L_V5, "gps_status:%d", gps_status);

    gpsLastPoint = track_cust_backup_gps_data(0, NULL);
    if(_cmd != NULL)
    {
#if defined(__GT740__)|| defined(__GT420D__)
        if(G_parameter.extchip.mode!=1)
        {
    		LOGD(L_APP, L_V5, "λ�ò�ѯָ����׷��ģʽ����Ч.");
			return;
		}
        if(_cmd->return_sms.cm_type == CM_SMS && (track_cust_sos_or_center_check(2, _cmd->return_sms.ph_num) % 8) != 0 && _cmd->supercmd != 1)
        {
            //   track_cmd_send_rsp(_cmd->return_sms.cm_type, strlen(msg7), msg7, 0);
            track_cust_sms_send(_cmd->return_sms.num_type, _cmd->return_sms.ph_num, 1, msg7, strlen(msg7));
            LOGD(L_APP, L_V5, "%s", msg7);
            return;
        }
#endif /* __GT300__ */
#if defined(__GERMANY__)        
        if(!strcmp(_cmd->pars[0], "WHERE") || !strcmp(_cmd->pars[0], "DWXX") || !strcmp(_cmd->pars[0], "GPS"))
#else
        if(!strcmp(_cmd->pars[0], "WHERE") || !strcmp(_cmd->pars[0], "DWXX"))
#endif
        {
            mode = 1;
        }
#if defined(__SPANISH__)
        else if(!strcmp(_cmd->pars[0], "URL") || !strcmp(_cmd->pars[0], "LOCALIZAR"))  // ��ó�汾��ֱ�ӷ� GOOGLE ���Ӷ���
#else
        else if(!strcmp(_cmd->pars[0], "URL"))  // ��ó�汾��ֱ�ӷ� GOOGLE ���Ӷ���
#endif
        {
            mode = 2;
        }
        else if(!strcmp(_cmd->pars[0], "DW") || !strcmp(_cmd->pars[0], "POSITION") || !strcmp(_cmd->pars[0], "123"))
        {
            mode = 3;
        }
        else
        {
            LOGD(L_APP, L_V1, "ERROR: _cmd->pars[0]=%s", _cmd->pars[0]);
            return;
        }
        if((mode == 1 || mode == 2) && gpsLastPoint->gprmc.status == 0)
        {
#if defined(__GT740__)||defined(__GT420D__)
            track_cust_gps_work_req((void*)5);
#endif
            // ����Ч����һ����λ��
            if(_cmd->return_sms.cm_type == CM_SMS && strlen(_cmd->return_sms.ph_num) > 0)
            {
                if(G_parameter.lang == 1)
                {
                    track_cust_sms_send(_cmd->return_sms.num_type, _cmd->return_sms.ph_num, 0, msg6, sizeof(msg6));
                }
                else
                {
                    track_cust_sms_send(_cmd->return_sms.num_type, _cmd->return_sms.ph_num, 1, msg2, strlen(msg2));
                }
            }
            else if(_cmd->return_sms.cm_type == CM_ONLINE)
            {
                track_cust_paket_msg_upload(_cmd->return_sms.stamp, KAL_TRUE, msg2, strlen(msg2));
            }
            else if(_cmd->return_sms.cm_type == CM_ONLINE2)
            {
                track_cust_paket_FD_cmd(_cmd->return_sms.stamp, KAL_TRUE, msg2, strlen(msg2));
            }
            else
            {
                track_cmd_send_rsp(_cmd->return_sms.cm_type, strlen(msg2), msg2, 0);
            }
            return;
        }
        else if(mode == 1)
        {
            applib_time_struct time = {0};
#if defined(__GT740__)||defined(__GT420D__)
		   if(gps_status < GPS_STAUS_2D)
           {
		   	    track_cust_gps_work_req((void*)5);
		   }
#endif

            /* GPS�����е�ʱ�����ʱ��У�� */
            if(track_cust_is_upload_UTC_time())
            {
                track_drv_utc_timezone_to_rtc(&time, (applib_time_struct*)&gpsLastPoint->gprmc.Date_Time, G_parameter.zone);
            }
            else
            {
                memcpy(&time, &gpsLastPoint->gprmc.Date_Time, sizeof(applib_time_struct));
            }
            // ��γ��λ�ò�ѯ
            if(track_cust_gps_status() >= GPS_STAUS_2D)
            {
#if defined(__GERMANY__)
                sprintf(msg, "Aktuelle Position: Lat:%c%f,Lon:%c%f,Richtung:%.2f,Geschwindigkeit:%.2fKm/h",
                        gpsLastPoint->gprmc.NS, gpsLastPoint->gprmc.Latitude, gpsLastPoint->gprmc.EW, gpsLastPoint->gprmc.Longitude,
                        gpsLastPoint->gprmc.Course, gpsLastPoint->gprmc.Speed);
#elif defined(__SPANISH__)
                 sprintf(msg, "Posicion actual! Lat:%c%f,Lon:%c%f,Curso:%.2f,Velocidad:%.2fKm/h,FechaHora:%.2d-%.2d-%.2d %.2d:%.2d:%.2d",
                        gpsLastPoint->gprmc.NS, gpsLastPoint->gprmc.Latitude, gpsLastPoint->gprmc.EW, gpsLastPoint->gprmc.Longitude,
                        gpsLastPoint->gprmc.Course, gpsLastPoint->gprmc.Speed, time.nYear,
                        time.nMonth, time.nDay, time.nHour,
                        time.nMin, time.nSec);
#else
                 sprintf(msg, "Current position! Lat:%c%f,Lon:%c%f,Course:%.2f,Speed:%.2fKm/h,DateTime:%.2d-%.2d-%.2d %.2d:%.2d:%.2d",
                        gpsLastPoint->gprmc.NS, gpsLastPoint->gprmc.Latitude, gpsLastPoint->gprmc.EW, gpsLastPoint->gprmc.Longitude,
                        gpsLastPoint->gprmc.Course, gpsLastPoint->gprmc.Speed, time.nYear,
                        time.nMonth, time.nDay, time.nHour,
                        time.nMin, time.nSec);
#endif
            }
            else
            {
#if defined(__GERMANY__)
                 sprintf(msg, "Letzte Position: Lat:%c%f,Lon:%c%f,Richtung:%.2f,Geschwindigkeit:%.2fKm/h",
                        gpsLastPoint->gprmc.NS, gpsLastPoint->gprmc.Latitude, gpsLastPoint->gprmc.EW, gpsLastPoint->gprmc.Longitude,
                        gpsLastPoint->gprmc.Course, gpsLastPoint->gprmc.Speed
                        );
#elif defined(__SPANISH__)
                sprintf(msg, "Ultima posicion! Lat:%c%f,Lon:%c%f,Curso:%.2f,Velocidad:%.2fKm/h,FechaHora:%.2d-%.2d-%.2d %.2d:%.2d:%.2d",
                        gpsLastPoint->gprmc.NS, gpsLastPoint->gprmc.Latitude, gpsLastPoint->gprmc.EW, gpsLastPoint->gprmc.Longitude,
                        gpsLastPoint->gprmc.Course, gpsLastPoint->gprmc.Speed, time.nYear,
                        time.nMonth, time.nDay, time.nHour,
                        time.nMin, time.nSec);
#else

                sprintf(msg, "Last position! Lat:%c%f,Lon:%c%f,Course:%.2f,Speed:%.2fKm/h,DateTime:%.2d-%.2d-%.2d %.2d:%.2d:%.2d",
                        gpsLastPoint->gprmc.NS, gpsLastPoint->gprmc.Latitude, gpsLastPoint->gprmc.EW, gpsLastPoint->gprmc.Longitude,
                        gpsLastPoint->gprmc.Course, gpsLastPoint->gprmc.Speed, time.nYear,
                        time.nMonth, time.nDay, time.nHour,
                        time.nMin, time.nSec);
#endif/*__GERMANY__*/
            }
            if(_cmd->return_sms.cm_type == CM_SMS && strlen(_cmd->return_sms.ph_num) > 0)
            {
                track_cust_sms_send(_cmd->return_sms.num_type, _cmd->return_sms.ph_num, 1, msg, strlen(msg));
            }
            else if(_cmd->return_sms.cm_type == CM_ONLINE)
            {
                track_cust_paket_msg_upload(_cmd->return_sms.stamp, KAL_TRUE, msg, strlen(msg));
            }
            else if(_cmd->return_sms.cm_type == CM_ONLINE2)
            {
                track_cust_paket_FD_cmd(_cmd->return_sms.stamp, KAL_TRUE, msg, strlen(msg));
            }
            else
            {
                track_cmd_send_rsp(_cmd->return_sms.cm_type, strlen(msg), msg, 0);
            }
            return;
        }
        else if(mode == 2)  // ��ó�汾��ֱ�ӷ� GOOGLE ���Ӷ���
        {
            // ��γ��λ�õ� Google ����
            get_url(_cmd, gpsLastPoint);
#if defined(__GT740__)||defined(__GT420D__)
		   if(gps_status < GPS_STAUS_2D)
		   {
				track_cust_gps_work_req((void*)5);
		   }
#endif
            return;
        }
        else if(mode == 3)
        {
            if(track_is_timer_run(TRACK_CUST_GET_ADDRESS_OVER2_TIMER_ID))
            {
                LOGD(L_APP, L_V4, "���β�ѯ���ԣ���ǰ�Ѿ��в�ѯ���ڽ���");
                return;
            }
            else
            {
                memcpy(&cmd, _cmd, sizeof(CMD_DATA_STRUCT));
                snprintf(cmdstr, 20, "%s", _cmd->pars[0]);
                cmd.pars[0] = cmdstr;
                if(gps_status < GPS_STAUS_2D)
                {
                    track_cust_gps_work_req((void*)5);
            #if defined(__GT740__)||defined(__GT300S__)||defined(__GT370__)|| defined (__GT380__) || defined(__GT310__)||defined(__GT420D__)
                    track_start_timer(TRACK_CUST_GET_ADDRESS_OVER2_TIMER_ID, 120000, track_cust_module_get_addr, NULL); // ��λ��ʱ
            #else
                    track_start_timer(TRACK_CUST_GET_ADDRESS_OVER2_TIMER_ID, 300000, track_cust_module_get_addr, NULL); // ��λ��ʱ
            #endif /* __GT740__ */
                    return;
                }
#if defined(__GT740__)||defined(__GT420D__)
                track_cust_paket_18(track_drv_get_lbs_data(), 0);
#endif

            }
        }
    }

    //cmd==NULL
    tr_stop_timer(TRACK_CUST_GET_ADDRESS_OVER2_TIMER_ID);
    if(!strcmp(cmd.pars[0], "DW") || !strcmp(cmd.pars[0], "POSITION") || !strcmp(cmd.pars[0], "123"))
    {
        char sta_num[CM_HEAD_PWD_MXA + 1] = {0};

        track_fun_cov_number(cmd.return_sms.num_type, cmd.return_sms.ph_num, sta_num, CM_HEAD_PWD_MXA);
        LOGD(L_APP, L_V5, "%s:%s", cmd.return_sms.ph_num, sta_num);

        if(gps_status < GPS_STAUS_2D)
        {
            // ����Ч����һ����λ��
            if(cmd.return_sms.cm_type == CM_SMS && strlen(cmd.return_sms.ph_num) > 0)
            {
                if(G_parameter.lang == 1)
                {
                    track_cust_sms_send(cmd.return_sms.num_type, cmd.return_sms.ph_num, 0, msg6, sizeof(msg6));
                }
                else
                {
                    track_cust_sms_send(cmd.return_sms.num_type, cmd.return_sms.ph_num, 1, msg4, strlen(msg4));
                }
            }
            else if(cmd.return_sms.cm_type == CM_ONLINE)
            {
                track_cust_paket_msg_upload(cmd.return_sms.stamp, KAL_TRUE, msg4, strlen(msg4));
            }
            else if(cmd.return_sms.cm_type == CM_ONLINE2)
            {
                track_cust_paket_FD_cmd(cmd.return_sms.stamp, KAL_TRUE, msg4, strlen(msg4));
            }
            else
            {
                track_cmd_send_rsp(cmd.return_sms.cm_type, strlen(msg4), msg4, 0);
            }
            return;
        }
#if defined(__GT740__)||defined(__GT420D__)
 		track_stop_timer(TRACK_CUST_DW_GPS_WORK_TIMER);
        track_cust_gps_location_timeout(10);
#endif
        if(track_nvram_alone_parameter_read()->gprson && G_parameter.lang == 1)
        {
            track_start_timer(TRACK_CUST_GET_ADDRESS_OVER_TIMER_ID, TRACK_ALARM_DELAY_TIME_SECONDS, Qurey_Get_Address_Fail_SendSMS, (void*)&cmd);
#if defined(__GT02__)
            track_cust_paket_1B(gpsLastPoint, cmd.return_sms.ph_num);
#else
            track_cust_paket_1A(gpsLastPoint, cmd.return_sms.ph_num);
#endif
        }
        else
        {
            // ��γ��λ�õ� Google ����
            get_url(&cmd, gpsLastPoint);
        }
    }
}
#endif
/******************************************************************************
 *  Function    -  track_cust_module_recv_addr
 *
 *  Purpose     -  ��Ӧ�������·������ĵ�ַ����ת�����û�
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_recv_addr(char * number, kal_uint8 * content, int content_len, kal_uint16 sn)
{
    LOGD(L_APP, L_V4, "�յ��������·��ĵ�ַ,sn=%d,lbs_sn=%d", sn, lbs_sn);

#if defined(__GT500__)||defined(__GT370__)|| defined (__GT380__)||defined(__GT03F__) ||defined(__ET200__) ||defined(__ET130__)|| defined(__V20__) || defined(__ET310__)||defined(__GT300S__)\
     || defined(__MT200__)||defined (__ET320__)
    track_stop_timer(TRACK_CUST_DW_GPS_WORK_TIMER);
    track_stop_timer(TRACK_CUST_GET_ADDRESS_OVER_TIMER_ID);
    track_stop_timer(TRACK_CUST_GET_ADDRESS_OVER3_TIMER_ID);
    track_stop_timer(TRACK_CUST_GET_ADDRESS_OVER4_TIMER_ID);
    track_cust_gps_location_timeout(10);
    track_cust_get_overtime_address(2, NULL);
    track_cust_sms_send(0, number, 0, content, content_len);
    gps_sn = 0;
    lbs_sn = 0;
    return;
#elif defined(__GW100__) ||defined(__GT310__)
    if(gps_sn == sn)
    {
        track_stop_timer(TRACK_CUST_DW_GPS_WORK_TIMER);
        track_stop_timer(TRACK_CUST_GET_ADDRESS_OVER_TIMER_ID);
        track_stop_timer(TRACK_CUST_GET_ADDRESS_OVER3_TIMER_ID);
        track_stop_timer(TRACK_CUST_GET_ADDRESS_OVER4_TIMER_ID);
        track_cust_gps_location_timeout(10);
        track_cust_get_overtime_address(2, NULL);
        track_cust_sms_send(0, number, 0, content, content_len);
        gps_sn = 0;
        lbs_sn = 0;
        return;
    }
    else
    {
        lbs_sn = 0XFFFE;
        track_cust_sms_send(0, number, 0, content, content_len);
    }
    return;
#endif
    track_stop_timer(TRACK_CUST_GET_ADDRESS_OVER_TIMER_ID);
    track_cust_sms_send(0, number, 0, content, content_len);

}


/******************************************************************************
 *  Function    -  track_cust_get_overtime_address
 *
 *  Purpose     -  ���õ�ַ����ʱ�䣬�����������ĵ�ַ
 *
 *  Description -   flag:����״̬��time:��������ʱ��
 *                        return:
                                    1:���ڵ�ַ����ʱ����
                                    2:���ڵ�ַ����ʱ����
                                    88:δ�趨ʱ�䷵��ֵ
                                    99:����ʱ��ɹ�
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 13-12-2013,  ZengPing && Liujw  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint32 track_cust_get_overtime_address(kal_uint32 flag, kal_uint32 time)
{
    static kal_uint32 overtime_tick = 0;
    kal_uint32 tick = 0;

    LOGD(L_APP, L_V6, "flag = %d,time=%d", flag, time);
    if(flag == 99)
    {
        tick = kal_get_systicks();
        if(overtime_tick > tick)
        {
            LOGD(L_APP, L_V6, "�����ȡ���ĵ�ַ��ַʱ����");
            return 1;
        }
        else
        {
            LOGD(L_APP, L_V6, "�������ȡ���ĵ�ַ��ַʱ��");
            return 0;
        }
    }


    if(flag == 1)
    {
        if(time != NULL)
        {
            tick = kal_get_systicks();
        }
        else
        {
            LOGD(L_APP, L_V6, "����ʱ�䲻��");
            return 88;
        }
        overtime_tick =  tick + kal_secs_to_ticks(time);
        LOGD(L_APP, L_V6, "���õ�ַ��������ʱ��overtime_tick=%d;tick=%d", overtime_tick, tick);
    }
    else
    {
        overtime_tick = 0;
        LOGD(L_APP, L_V6, "�����ַ��������ʱ��overtime_tick=%d", overtime_tick);
    }
    return 99;

}
static CMD_DATA_STRUCT cmd_tmp = {0};
void track_cust_set_dw_type(CMD_DATA_STRUCT * _cmd)
{
    if(_cmd != NULL)
    {
        memcpy(&cmd_tmp, _cmd, sizeof(CMD_DATA_STRUCT));
    }
}

void track_cust_gps_location_get_overtime(void)
{
    if(track_is_timer_run(TRACK_CUST_GET_ADDRESS_OVER4_TIMER_ID) == FALSE)
    {
        LOGD(L_APP, L_V5, "��λ�ɹ�������GPS��ַ��ʱ���� %d,%s", cmd_tmp.return_sms.cm_type, cmd_tmp.return_sms.ph_num);
        track_start_timer(TRACK_CUST_GET_ADDRESS_OVER4_TIMER_ID, 1000 * 60, Qurey_Get_Address_Fail_SendSMS2, (void*)&cmd_tmp);
    }
}

