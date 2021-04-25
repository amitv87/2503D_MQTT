#if defined(__XCWS__)
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
#include "track_cust_paket_cq.h"
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
static kal_uint16 g_SerialNumber = 0;

static char *MachineType = {"1"};
//static char *G_importance_parameter_data.login.ProtocolVersion = {"V1.0.0"};
//static char *SequenceNumber = {"000004003"};
//static char *SIM_NO = {"15119036876"};                     // SIM����
//static char *MasterSIM_NO = {"1314691211"};               // �����ֻ���
static char *PlatformPasswork = {"123456"};           // ����
//static char *IMSI = {"460021075219613"};        // IMSI 460021190700523
//static char *IMEI = {"355889008722099"};        // IMEI
static char *ManufacturerIdentifier = {"KKS"};     // �ն˳��̱�ʶ
/****************************************************************************
*  Global Variable                      ȫ�ֱ���
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/
extern kal_uint8 track_cust_gps_status(void);

/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/

#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/
void track_cust_get_time_from_system(kal_bool isUTC, applib_time_struct *time)
{
    if(isUTC)
    {
        applib_dt_get_rtc_time(time);
    }
    else
    {
        applib_time_struct currTime = {0};
        applib_dt_get_rtc_time(&currTime);
        track_drv_utc_timezone_to_rtc(time, &currTime, G_parameter.zone);
    }
}
static char *get_time(char *timebuf, int buflen, track_gps_data_struct *gd)
{
    applib_time_struct time = {0};
    applib_time_struct currTime = {0};
    if(gd == NULL)
    {
        track_cust_get_time_from_system(KAL_TRUE, &time);
    }
    else
    {
        LOGD(L_APP, L_V5, "gps status:%d", gd->gprmc.status);
        if(gd->gprmc.status && track_cust_gps_status() > 2)
        {
            memcpy(&time, &gd->gprmc.Date_Time, sizeof(applib_time_struct));
        }
        else
        {
            track_cust_get_time_from_system(KAL_TRUE, &time);
        }
    }
    track_drv_utc_timezone_to_rtc(&currTime, &time, G_parameter.zone);

    snprintf(timebuf, buflen, "%0.4d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
             currTime.nYear, currTime.nMonth, currTime.nDay, currTime.nHour, currTime.nMin, currTime.nSec);
    LOGD(L_APP, L_V5, "time: %s", timebuf);
    return timebuf;
}

static char *get_gpsstr(char *gpsbuf, int buflen, track_gps_data_struct *gd)
{
    //0/1/2(1:��ʾ��λ�ɹ���0:��ʾ��λʧ�ܣ�2:��ʾ�Ǳ������γ��),E/W(E:��ʾ������W:��ʾ����),����,N/S(N:��ʾ��γ��S:��ʾ��γ),γ��,�ٶ�,��λ��
    //0,E,0.000000,N,0.000000,0.0,0.0
    //1,E,113.252432,N,22.564152,120.3,270.5
    if((gd->status == 2 || gd->status == 3) && track_cust_gps_status() > 2)//0
    {
        snprintf(gpsbuf, buflen, "1,%c,%.6f,%c,%.6f,%.1f,%.1f",
                 gd->gprmc.EW, gd->gprmc.Longitude, gd->gprmc.NS, gd->gprmc.Latitude, (gd->gprmc.Speed), gd->gprmc.Course);
    }
    else if(gd->status == 2 || gd->status == 3)
    {
        snprintf(gpsbuf, buflen, "2,%c,%.6f,%c,%.6f,%.1f,%.1f",
                 gd->gprmc.EW, gd->gprmc.Longitude, gd->gprmc.NS, gd->gprmc.Latitude, (gd->gprmc.Speed), gd->gprmc.Course);
    }
    else
    {
        gps_gprmc_struct *last_rmc;
        last_rmc = track_drv_get_gps_RMC();
        if(last_rmc->status)
        {
            snprintf(gpsbuf, buflen, "2,%c,%.6f,%c,%.6f,%.1f,%.1f",
                     last_rmc->EW, last_rmc->Longitude, last_rmc->NS, last_rmc->Latitude, (last_rmc->Speed), last_rmc->Course);
        }
        else
        {
            snprintf(gpsbuf, buflen, "0,,,,,,");
        }
    }
    return gpsbuf;
}

static char *get_lbsstr(char *lbdbuf, LBS_Multi_Cell_Data_Struct *lbs)
{
    /* 460:00:04125:26390 */
    //static char buf[20] = {0};
    snprintf(lbdbuf, 20, "%d:%d:%d:%d", lbs->MainCell_Info.mcc, lbs->MainCell_Info.mnc, lbs->MainCell_Info.lac, lbs->MainCell_Info.cell_id);
    //LOGD(L_APP, L_V5, "%s,%d:%d:%d:%d", lbdbuf, lbs->MainCell_Info.mcc, lbs->MainCell_Info.mnc, lbs->MainCell_Info.lac, lbs->MainCell_Info.cell_id);
    return lbdbuf;
}

static kal_uint8 get_battery_level(float value)
{
    kal_uint8 bmt;
    //LOGD(L_APP, L_V5, "%f",value);
    if(value <= 3.65) bmt = 0;
    else if(value <= 3.7) bmt = 3;
    else if(value <= 4.15) bmt = 6;
    else bmt = 9;
    return bmt;
}

/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/

kal_int8 track_cust_paket_T1(kal_uint8 *out, const int outlimit, const char *RestartErrorCode)//��������������һ��11
{
    char timebuf[64] = {0};
    int ret;

    ret = snprintf(out, outlimit, "[%s,%d,%s,%s,T1,%s,%s,%s,%s,%s,%s]",
                   get_time(timebuf, 64, NULL), G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber,
                   G_importance_parameter_data.login.owner_num, G_importance_parameter_data.login.host_num, G_importance_parameter_data.login.password, track_drv_get_imsi(0), track_drv_get_imei(0), ManufacturerIdentifier);
    if(ret >= outlimit) return -1;
    return ret;
}

//[ʱ��,�ն�����,Э��汾��,�ն����к�,��������,��λ���֣���һλ��ʾ�������Ŀ��� GPS �����������ڶ�λ��ʾ GSM �ź�ǿ�ȣ�����λ�� ʾ���õ�ؿ��õ�����ʣ�������ѹ 3.65v �C���������Ϣ��ʾΪ 0��ʣ�������ѹ 3.7v �C�� ��������Ϣ��ʾΪ 3��ʣ�������ѹ 4.15v �C���������Ϣ��ʾΪ 6��ʣ�������ѹ 4.2v �C�� ��������Ϣ��ʾΪ 9]
//[,,,000872209,T0,086]
void track_cust_paket_T2()
{
    char buf[200] = {0};
    char timebuf[64] = {0};
    int ret;
    kal_uint8 sat = 0;
    float battery;
    battery = track_cust_get_battery_volt();
    battery /= 1000000;
    LOGD(L_APP, L_V5, "%f", battery);
    if(track_cust_gps_status() != 0)
    {
        track_gps_data_struct *gpsdata = track_drv_get_gps_data();
        sat = gpsdata->gpgga.sat;
        if(sat > 9)
        {
            sat = 9;
        }
    }
    ret = snprintf(buf, 200, "[%s,%d,%s,%s,T2,%d%d%d]",
                   get_time(timebuf, 64, NULL), G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber,
                   sat, (kal_uint8)track_cust_get_rssi_level(), (kal_uint8)get_battery_level(battery));
    if(ret >= 200)
    {
        LOGD(L_APP, L_V1, "��ʱ����������");
    }
    else
    {
        track_soc_write_req(buf, ret, 0, 0);
    }
}

kal_int16 track_cust_paket_T3(kal_uint8 *out, const int outlimit, track_cust_packet_params_struct *data)
{
    char timebuf[64] = {0}, gpsbuf[64] = {0}, lbsbuf[20] = {0};
    int ret;
    float battery;
    kal_uint8 sat = 0;
    battery = track_cust_get_battery_volt();
    battery /= 1000000;
    if(data->gd->gprmc.status != 0)
    {
        sat = data->gd->gpgga.sat;
        if(sat > 9)
        {
            sat = 9;
        }
    }
    ret = snprintf(out, outlimit, "[%s,%d,%s,%s,T3,%s,%d,%s,%d%d%d]",
                   get_time(timebuf, 64, data->gd),
                   G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber,
                   get_gpsstr(gpsbuf, 64, data->gd), (kal_uint16)data->status, get_lbsstr(lbsbuf, data->lbs), sat, (kal_uint8)data->GSMSignalStrength, (kal_uint8)get_battery_level(battery));
    if(ret >= outlimit) return -1;
    return ret;
}

/*ƽ̨������λ�ն�*/
//[ʱ��,�ն�����,Э��汾��,�ն����к�,��������,0/1/2(1:��ʾ��λ�ɹ���0:��ʾ��λʧ�ܣ�2:��ʾ�Ǳ������γ��),E/W(E:��ʾ������W:��ʾ����),����,N/S(N:��ʾ��γ��S:��ʾ��γ),γ��,�ٶ�,��λ��,״̬]]
//[2008-12-16 10:00:00, 0,W1,10000001,T10,1,E,113.252432,N,22.564152,120.3,270.5,1, 460:00:10101:03633]

kal_int8 track_cust_paket_T4(kal_uint8 *out, const int outlimit, track_cust_packet_params_struct *data)
{
    char timebuf[64] = {0}, gpsbuf[64] = {0}, lbsbuf[20] = {0};
    int ret;

    ret = snprintf(out, outlimit, "[%s,%d,%s,%s,T4,%s,%d,%s]",
                   get_time(timebuf, 64, data->gd), G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber, get_gpsstr(gpsbuf, 64, data->gd), (kal_uint16)data->status, get_lbsstr(lbsbuf, data->lbs));
    if(ret >= outlimit) return -1;
    return ret;
}


//[ʱ��,�ն�����,Э��汾��,�ն����к�,��������,������]
//[2008-12-16 10:00:00, 0,W1,10000001,T14,PSW=123456]
//[ʱ��,�ն�����,Э��汾��,�ն����к�,��������,������]
//[2008-12-16 10:00:00, 0,W1,10000001,T14,PSW=123456]
kal_int8 track_cust_paket_T5(kal_uint8 *out, const int outlimit, char *name, char *value)
{
    char timebuf[64] = {0};
    int ret;

    ret = snprintf(out, outlimit, "[%s,%d,%s,%s,T5,%s=%s]",
                   get_time(timebuf, 64, NULL), G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber, name, value);
    LOGD(L_APP, L_V5, "ret:%d,outlimit:%d", ret, outlimit);
    if(ret >= outlimit) return -1;
    return ret;
}

//[ʱ��,�ն�����,Э��汾��,�ն����к�,��������,������,0/1(1:��ʾ�޸ĳɹ� 0:��ʾʧ��)]
//[2008-12-16 10:00:00,0,W1, 10000001,T2,DOMAIN,1]
kal_int8 track_cust_paket_T6(kal_uint8 *out, const int outlimit, const char *CommandCode, kal_bool isOK)
{
    char timebuf[64] = {0};
    int ret;

    ret = snprintf(out, outlimit, "[%s,%d,%s,%s,T6,%s,%d]",
                   get_time(timebuf, 64, NULL), G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber,
                   CommandCode, isOK);
    if(ret >= outlimit) return -1;
    return ret;
}

#if 0
//[ʱ��,�ն�����,Э��汾��,�ն����к�,��������,��ص���]
//[2008-12-16 10:00:00,0,W1,10000001,T7,3780]
kal_int8 track_cust_paket_T7(kal_uint8 *out, const int outlimit, float battery)
{
    char timebuf[64] = {0};
    int ret;

    ret = snprintf(out, outlimit, "[%s,%d,%s,%s,T7,%d]",
                   get_time(timebuf, 64, NULL), G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber, battery);
    if(ret >= outlimit) return -1;
    return ret;
}
#endif
/*Զ������ظ�Liujw*/
//[ʱ��,�ն�����,Э��汾��,�ն����к�,��������,(1 ������ɹ���0 �����ʧ�ܣ�2 ������ GPS ����ɹ� 3 ������ GPS)]
//[2008-12-16 10:00:00, 0,W1,10000001,T12,1]
kal_int8 track_cust_paket_T8(kal_uint8 *out, const int outlimit, kal_uint8 status)
{
    char timebuf[64] = {0};
    int ret;

    ret = snprintf(out, outlimit, "[%s,%d,%s,%s,T8,%d]",
                   get_time(timebuf, 64, NULL), G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber, status);
    if(ret >= outlimit) return -1;
    return ret;
}


/*Զ�̳����ظ�*/
//[ʱ��,�ն�����,Э��汾��,�ն����к�,��������,(1 ������ɹ���0 �����ʧ�ܣ�2 ������ GPS ����ɹ� 3 ������ GPS)]
//[2008-12-16 10:00:00, 0,W1,10000001,T13,1]
kal_int8 track_cust_paket_T9(kal_uint8 *out, const int outlimit, kal_uint8 status)
{
    char timebuf[64] = {0};
    int ret;

    ret = snprintf(out, outlimit, "[%s,%d,%s,%s,T9,%d]",
                   get_time(timebuf, 64, NULL), G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber, status);
    if(ret >= outlimit) return -1;
    return ret;
}


/*λ�Ƹ澯���*/

kal_int8 track_cust_paket_T11(kal_uint8 *out, const int outlimit, track_cust_packet_params_struct *data)
{
    char timebuf[64] = {0}, gpsbuf[64] = {0}, lbsbuf[20] = {0};
    int ret;

    ret = snprintf(out, outlimit, "[%s,%d,%s,%s,T11,%s,%d,%s]",
                   get_time(timebuf, 64, data->gd), G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber, get_gpsstr(gpsbuf, 64, data->gd), (kal_uint16)data->status, get_lbsstr(lbsbuf, data->lbs));
    if(ret >= outlimit) return -1;
    return ret;
}

/*�ϵ�澯���*/

kal_int8 track_cust_paket_T12(kal_uint8 *out, const int outlimit, track_cust_packet_params_struct *data)
{
    char timebuf[64] = {0}, gpsbuf[64] = {0}, lbsbuf[20] = {0};
    int ret;

    ret = snprintf(out, outlimit, "[%s,%d,%s,%s,T12,%s,%d,%s]",
                   get_time(timebuf, 64, data->gd), G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber, get_gpsstr(gpsbuf, 64, data->gd), (kal_uint16)data->status, get_lbsstr(lbsbuf, data->lbs));
    if(ret >= outlimit) return -1;
    return ret;
}


/*�͵籨����*/

kal_int8 track_cust_paket_T13(kal_uint8 *out, const int outlimit, kal_uint32 battery)
{
    char timebuf[64] = {0};
    int ret;

    ret = snprintf(out, outlimit, "[%s,%d,%s,%s,T13,%d]",
                   get_time(timebuf, 64, NULL), G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber, battery);
    if(ret >= outlimit) return -1;
    return ret;
}

/*Խ��澯���*/
kal_int8 track_cust_paket_T14(kal_uint8 *out, const int outlimit, track_cust_packet_params_struct *data)
{
    char timebuf[64] = {0}, gpsbuf[64] = {0}, lbsbuf[20] = {0};
    int ret;

    ret = snprintf(out, outlimit, "[%s,%d,%s,%s,T14,%s,%d,%s]",
                   get_time(timebuf, 64, data->gd), G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber, get_gpsstr(gpsbuf, 64, data->gd), (kal_uint16)data->status, get_lbsstr(lbsbuf, data->lbs));
    if(ret >= outlimit) return -1;
    return ret;
}


/*���ٸ澯���*/
kal_int8 track_cust_paket_T15(kal_uint8 *out, const int outlimit, track_cust_packet_params_struct *data)
{
    char timebuf[64] = {0}, gpsbuf[64] = {0}, lbsbuf[20] = {0};
    int ret;

    ret = snprintf(out, outlimit, "[%s,%d,%s,%s,T15,%s,%d,%s]", get_time(timebuf, 64, data->gd), G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber, get_gpsstr(gpsbuf, 64, data->gd), (kal_uint16)data->status, get_lbsstr(lbsbuf, data->lbs));
    if(ret >= outlimit) return -1;
    return ret;
}


kal_int8 track_cust_paket_T16(kal_uint8 *out, const int outlimit, char *name, char *value)
{
    char timebuf[64] = {0};
    int ret;

    ret = snprintf(out, outlimit, "[%s,%d,%s,%s,T16,%s=%s]",
                   get_time(timebuf, 64, NULL), G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber, name, value);
    if(ret >= outlimit) return -1;
    return ret;
}

/*��������澯Ӧ��*/
kal_int8 track_cust_paket_T19(kal_uint8 *out, const int outlimit, kal_bool flag)
{
    char timebuf[64] = {0};
    int ret;

    ret = snprintf(out, outlimit, "[%s,%d,%s,%s,T19,%d]",
                   get_time(timebuf, 64, NULL), G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber, flag);
    if(ret >= outlimit)
    {
        return -1;
    }
    return ret;
}
/*ȡ������澯Ӧ��*/
kal_int8 track_cust_paket_T20(kal_uint8 *out, const int outlimit, kal_bool flag)
{
    char timebuf[64] = {0};
    int ret;

    ret = snprintf(out, outlimit, "[%s,%d,%s,%s,T20,%d]",
                   get_time(timebuf, 64, NULL), G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber, flag);

    if(ret >= outlimit)
    {
        return -1;
    }
    return ret;
}

/*�ն��ϱ�����澯*/
kal_int8 track_cust_paket_T21(kal_uint8 *out, const int outlimit, track_cust_packet_params_struct *data, kal_bool fence_out)
{
    nvram_electronic_fence_struct* track_cust_get_module_fence_data(void);
    char timebuf[64] = {0}, gpsbuf[64] = {0}, lbsbuf[20] = {0};
    int ret;
    nvram_electronic_fence_struct*fence;

    fence = track_cust_get_module_fence_data();

    ret = snprintf(out, outlimit, "[%s,%d,%s,%s,T21,%s,%d,%s,%d]",
                   get_time(timebuf, 64, data->gd), G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber, \
                   get_gpsstr(gpsbuf, 64, data->gd), (kal_uint16)data->status, get_lbsstr(lbsbuf, data->lbs), fence_out);
    if(ret >= outlimit)
    {
        return -1;
    }
    return ret;
}

/******************************************************************************
 *  Function    -  gprs_login_data_send
 *
 *  Purpose     -  ��½������
 *
 *  Description -  return Null
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 07-08-2012,  Liujw  written
 * ----------------------------------------
 ******************************************************************************/
void gprs_login_data_send(void)
{
    char buf[128] = {0};
    int len;
    LOGD(L_APP, L_V5, "gprs_login_data_send");
    len = track_cust_paket_T1(buf, 128, "11");
    if(len <= 0)
    {
        LOGD(L_APP, L_V5, "ERROR 1");
        return;
    }
    track_soc_write_req(buf, len, 0, 0);
}

void heartbeat_packets_cq(void)
{
    LOGD(L_APP, L_V5, " acc=%d,%d,%d,%d,%d,", track_cust_status_acc(), G_parameter.hbt_acc_on, G_parameter.hbt_acc_off, G_parameter.hbt_acc_off, G_parameter.hbt_acc_off);
    track_cust_paket_T2();
    if(track_cust_status_acc() == 1)
    {
        if(G_parameter.hbt_acc_on > 0)
        {
            tr_start_timer(TRACK_CUST_NET_HEARTBEAT_TIMER_ID, G_parameter.hbt_acc_on * 1000, heartbeat_packets_cq);
        }
        if(!track_is_timer_run(TRACK_CUST_NET_HEARTBEAT_OVER_TIMER_ID))
        {
            track_start_timer(TRACK_CUST_NET_HEARTBEAT_OVER_TIMER_ID, G_parameter.hbt_acc_on * 2000 + 60000, track_soc_gprs_reconnect, (void *)2);
        }
    }
    else
    {
        if(G_parameter.hbt_acc_off > 0)
        {
            tr_start_timer(TRACK_CUST_NET_HEARTBEAT_TIMER_ID, G_parameter.hbt_acc_off * 1000, heartbeat_packets_cq);
        }
        if(!track_is_timer_run(TRACK_CUST_NET_HEARTBEAT_OVER_TIMER_ID))
        {
            track_start_timer(TRACK_CUST_NET_HEARTBEAT_OVER_TIMER_ID, G_parameter.hbt_acc_off * 2000 + 60000, track_soc_gprs_reconnect, (void *)2);
        }
    }
}

void gprs_gps_data_send(track_gps_data_struct *gpsdata)
{
    LOGD(L_APP, L_V5, "");
    if(gpsdata != NULL)
    {
        track_cust_packet_params_struct t3 = {0};
        char buf[256] = {0};
        int len;
        t3.gd = gpsdata;
        t3.status = track_get_terminal_status();//�ն�״̬λ
        t3.GSMSignalStrength = track_cust_get_rssi_level();
        t3.battery = track_cust_get_battery_volt();
        t3.lbs = track_drv_get_lbs_data();
        LOGD(L_APP, L_V5, "%d,%f", gpsdata->gpgga.sat, t3.battery);
        len = track_cust_paket_T3(buf, sizeof(buf), &t3);
        if(len <= 0)
        {
            LOGD(L_APP, L_V5, "ERROR 1");
            return;
        }
        //track_soc_write_req(buf, len, 0, 0);
        track_socket_queue_position_send_reg(buf, len, 0, 0);
#if defined(__KENAER__)
        track_cust_server3_queue_send_reg(buf, len, 0, 0);
#endif
    }
}
void track_cust_call_gps_position(void* par)
{
    kal_uint8 str = 0;
    track_gps_data_struct *gpsdata;
    str = (kal_uint8)par;
    LOGD(L_APP, L_V5, "%d", str);
    if(str == 1)
    {
        if(track_cust_gps_status() > 2)
        {
            gpsdata = track_drv_get_gps_data();
            gprs_gps_data_send(gpsdata);
        }
        else
        {
            track_os_intoTaskMsgQueueExt(track_cust_call_gps_position,(void *) 2);
        }
    }
    else if(str == 2)
    {
        track_start_timer(TRACK_CUST_CALL_GPS_POSITION_TIMER_ID, G_parameter.gps_work.location_timeout * 60000 - 20000, track_cust_call_gps_position, (void *) 3);
    }
    else if(str == 3)
    {
        track_stop_timer(TRACK_CUST_CALL_GPS_POSITION_TIMER_ID);
        if(track_cust_gps_status() > 2)
        {
            gpsdata = track_drv_get_gps_data();
        }
        /*else
        {
            gpsdata = track_cust_backup_gps_data(0, NULL);
        }*/
        gprs_gps_data_send(gpsdata);
    }
}
#endif /* __XCWS__ */
