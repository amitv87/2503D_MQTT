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
#include "mtk_gps_type.h"
#include "track_os_ell.h"

/*****************************************************************************
*  Define                               �궨��
******************************************************************************/

/*****************************************************************************
*  Typedef  Enum                        ö�ٶ���
*****************************************************************************/

/*****************************************************************************
*  Struct                               ���ݽṹ����
******************************************************************************/
typedef struct
{
    kal_uint8 a;
    char *b;
} error_msg_struct;

/*****************************************************************************
*  Local variable                       �ֲ�����
*****************************************************************************/
static kal_uint32 file_flag = 0;
static kal_uint16 req_size = 0, file_crc = 0, recv_size = 0;
static kal_bool force_update = KAL_FALSE;      // �Ѿ�������Ч��EPO�ļ���Ȼǿ�Ƹ���
static kal_bool only_update = KAL_TRUE;        // ��ֹ�ӷ������»�����EPO������Ч�ģ���Ȼ�������ص�����
static int retry;
static kal_uint32 start_time_tick = 0;
static kal_uint8 _filetype = 0;
static kal_uint8 nowUpdateFile = 0;

static error_msg_struct error_msg[] =
{
    {0x00, ""},
    {0x01, "�ɹ�"},
    {0x02, "CRCУ��ʧ��"},
    {0x03, "�����ȴ���"},
    {0x04, "�����ʹ���"},
    {0x05, "�ļ����ʹ���"},
    {0x06, "IMEI��Ч"},
    {0x07, "оƬID��Ч"},
    {0x08, "оƬID��IMEI��ƥ��"},
    {0x09, "����汾����Ч"},
    {0x0A, "�ļ�ʶ�������"},
    {0x0B, "���ݰ����ȴ���"},
    {0x0C, "�����ļ���ʼλ�ô���"},
    {0x0D, "δ��¼���������ļ�"},
    {0x0E, "δ��¼���ͷָ��ļ�����"},
    {0x0F, "δ���������ļ����ͷָ��ļ�����"},
    {0x10, "���ݰ�����"},
    {0x11, "��������æ"},
    {0x12, "�������ļ�����"}
};

static float s_lat=0;
static float s_lon=0;
static kal_uint8 No_Last_Pos=0;//���ϴ�һ�ζ�λ���־

/****************************************************************************
*  Global Variable                      ȫ�ֱ���
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/
extern kal_int8 track_send_assistance_data(kal_bool ischeck, kal_uint16 iYr, kal_uint8 iMo, kal_uint8 iDay, kal_uint8 iHr);
extern kal_int32 track_fun_decryption(kal_uint8 *dec, kal_uint8 *src, int size);
extern kal_int8 track_drv_gps_reset(kal_uint8 delay);
/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/

#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/

void track_cust_agps_time_add_6hour(applib_time_struct *time)
{
    applib_time_struct diff_time = {0}, in_dt = {0};

    memcpy(&in_dt, time, sizeof(applib_time_struct));
    //�������ʱ���Ƴ�6Сʱ
    diff_time.nHour = 6;
    
    applib_dt_increase_time(&in_dt, &diff_time, time);
}


/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/

/******************************************************************************
 *  Function    -  track_cust_agps_restart
 *
 *  Purpose     -  �����ǰGPSδ��ɶ�λ���ر�GPS�����¿�����Ϊ����EPO�ļ������ӿ춨λ
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 11-06-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_agps_restart(void)
{
    LOGD(L_APP, L_V5, "");
    if(GPS_STAUS_OFF != track_cust_gps_status())
    {
        track_drv_gps_reset(1);
    }
}
kal_uint8 track_cust_agps_check_from_time(mtk_param_epo_time_cfg *epo_time)
{
    kal_int8 r;
    kal_uint8 ret = 0;
    track_gps_data_struct *gpsLastPoint = NULL;
#if defined (__SECOND_SET__)
    return 0x01;// xzq ��ȡ��γ��
#endif
    gpsLastPoint = track_cust_backup_gps_data(0, NULL);
    r = track_send_assistance_data(KAL_TRUE, epo_time->u2YEAR, epo_time->u1MONTH, epo_time->u1DAY, epo_time->u1HOUR);
    LOGD(L_APP, L_V4, "ret=%d, ��½�ɹ� %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", ret,
         epo_time->u2YEAR, epo_time->u1MONTH, epo_time->u1DAY,
         epo_time->u1HOUR, epo_time->u1MIN, epo_time->u1SEC);
    if(force_update)
    {
        return 3;
    }
    if(gpsLastPoint->gprmc.status == 0)
    {
        ret |= 1;
    }
    if(only_update && r <= 4)
    {
        ret |= 2;
    }
    if(!ret && track_cust_gps_status() == GPS_STAUS_SCAN)
    {
        //track_cust_agps_restart();
        track_drv_gps_epo_start();
    }
    return ret;
}

#if defined(__ZKE_EPO__)
kal_uint8  track_zkw_eph_update(void)
{
    extern gps_gptxt_struct * track_drv_get_txt_data(void);
    kal_uint8 ret = 0;
    track_gps_data_struct *gpsLastPoint = NULL;
    gpsLastPoint = track_cust_backup_gps_data(0, NULL);
    
    if(force_update)
    {
        LOGD(5,5,"1");
        return 3;
    }
    LOGD(5,5,"ret:%d",ret);
    if(track_cust_gps_status()>2)
    {
        LOGD(L_DRV, L_V5, "�Ѷ�λ������Ҫ����");
        track_zkw_datatxt_clear();
        return 0;
    }
    LOGD(5,5,"ret:%d",ret);
    if(gpsLastPoint->gprmc.status == 0)
    {
        ret |= 1;
    }
    LOGD(5,5,"ret:%d",ret);
    if(only_update)
    {
        ret |= track_drv_get_txt_data()->status;
    }
    LOGD(5,5,"ret:%d",ret);
    return ret;
}
#endif

kal_uint8 track_cust_apgs_sw_status()
{
    kal_uint8 result=0;
#if defined(__GT740__)
    if(G_parameter.agps.sw)
    {
        if(G_parameter.extchip.mode==1 || G_realtime_data.tracking_mode==1 || G_parameter.extchip.mode1_en)
        {
            result = 1;
        }
        else if(G_parameter.gpswake==1)
        {
            result =1;
        }
    }
    return result;
#else
    return G_parameter.agps.sw;
#endif
}

/******************************************************************************
 *  Function    -  track_cust_agps_status
 *
 *  Purpose     -  ��¼��ǰ�Ƿ���AGPS��EPO�ļ�����������
 *
 *  Description -  ����     ����
 *                 0        �˳���������
 *                 1        ��ǵ�ǰ���ڸ���������
 *                 ����ֵ
 *                     0    ���ڸ���������
 *                     1    ���ڸ���������
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 11-06-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint8 track_cust_agps_status(kal_uint8 par)
{
    static kal_uint8 status = 0;
    if(par == 0)
    {
        status = 0;
    }
    else if(par == 1)
    {
        status = 1;
    }
    else if(par == 99)
    {
        if(G_parameter.agps.sw == 0)
        {
            return 0;
        }
    }
    return status;
}

/******************************************************************************
 *  Function    -  track_cust_agps_main
 *
 *  Purpose     -  AGPS��EPO�ļ������������������ǰEPO�ļ�����Ч�ģ���ô����������
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 11-06-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_agps_main(void)
{
    kal_uint8 agps_status = track_cust_agps_status(99);
    LOGD(L_APP, L_V5, "%d", agps_status);
    if(agps_status == 0)
    {
        retry = 0;
        track_cust_agps_status(1);
        track_cust_server2_conn((void*)1);
    }
}

/******************************************************************************
 *  Function    -  track_cust_agps_get_data_force
 *
 *  Purpose     -  ǿ�Ƹ���AGPS��EPO�ļ������Ե�ǰ�Ѿ�������Ч��EPO�ļ�
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 11-06-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_agps_get_data_force(kal_uint8 arg)
{
    LOGD(L_APP, L_V5, "");
    if(arg == 2) force_update = KAL_TRUE;
    track_cust_agps_status(0);
    track_cust_agps_start();
}

void track_cust_agps_start(void)
{
    applib_time_struct currTime = {0};
    kal_bool valid = KAL_TRUE;

    LOGD(L_APP, L_V5, "");
    
    track_os_intoTaskMsgQueue(track_cust_agps_main);

    if(track_cust_agps_valid(1))
    {
        track_fun_get_time(&currTime, KAL_TRUE, NULL);
        if(currTime.nYear == 2000 + (*OTA_DEFAULT_HARDWARE_YEAR) && currTime.nMonth == (*OTA_DEFAULT_HARDWARE_MON))
        {
            LOGD(L_APP, L_V5, "4");
            valid = KAL_FALSE;
        }
        else
        {
            kal_int8 ret = track_send_assistance_data(KAL_TRUE, currTime.nYear, currTime.nMonth, currTime.nDay, currTime.nHour);
            LOGD(L_APP, L_V4, "��ǰϵͳʱ��: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
                 currTime.nYear, currTime.nMonth, currTime.nDay,
                 currTime.nHour, currTime.nMin, currTime.nSec);
            if(ret < 4)
            {
                LOGD(L_APP, L_V5, "5");
                valid = KAL_FALSE;
            }
        }

        if(valid && track_cust_gps_status() == GPS_STAUS_SCAN)
        {
            LOGD(L_APP, L_V5, "6");
            //track_cust_agps_restart();
            track_drv_gps_epo_start();
        }
    }
}

U8 track_cust_xor_buf(char* buf, U8 len)
{
	U8 ch,i=1;
	ch=buf[0];
	while(i<len)
	{
	    ch = ch^buf[i];
	    i++;
	}
	return ch;
}
/******************************************************************************
 *  Function    -  track_cust_secset
 *
 *  Purpose     -  ע����һ�ζ�λ��ο�λ��
 *
 *  Description -  ĿǰĬ��ע����һ�ζ�λ�㣬�������������Ʒ����ͨ�������ֻע���������
 *
 *  modification history
 * ----------------------------------------
 * v1.0   08-06-2016,  xzq
 * ----------------------------------------
*******************************************************************************/
static void track_cust_secset(float* lat1,float* lon1)
{
    float lat = 0, lon = 0, altitude=0;
    char singlebuf[100] = {0};
    U8 checksum = 0, gstus = 0;
    float fp = 2;
    track_gps_data_struct *gpsLastPoint2 = NULL;
    applib_time_struct currTime = {0};
    track_fun_get_time(&currTime, KAL_TRUE, NULL);//utcʱ��
    gpsLastPoint2 = track_cust_backup_gps_data(0, NULL);
    gstus = track_cust_gps_status();
    if(gstus > GPS_STAUS_SCAN)
    {
        LOGD(L_TEST, L_V6, "�Ѷ�λ������ע��!");
        return;
    }
#if !(defined(__GT360__) || defined(__GT740__)||defined(__GT420D__))
    if(gpsLastPoint2->gprmc.status == 1)
    {
		if(gpsLastPoint2->gprmc.EW == 'E')
        {
            lon = gpsLastPoint2->gprmc.Longitude;
        }
        else
        {
            lon = -gpsLastPoint2->gprmc.Longitude;
        }
        if(gpsLastPoint2->gprmc.NS == 'N')
        {
            lat = gpsLastPoint2->gprmc.Latitude;
        }
        else
        {
            lat = -gpsLastPoint2->gprmc.Latitude;
        }
		altitude = gpsLastPoint2->gpgga.msl_altitude;
    }
    else if(s_lat != 0 && s_lon != 0)
#else
	if(s_lat != 0 && s_lon != 0)// �ֳ���Ŀ��ע���ϴζ�λ�㣬������
#endif
    {
        lat = s_lat;
        lon = s_lon;
		altitude = 0;
	}
    else
    {
        LOGD(L_TEST, L_V5, "��ע��ο��㣡��");
        No_Last_Pos = 1;
        return;
    }
    snprintf(singlebuf, 99, "$PMTK713,%0.6f,%0.6f,%0.6f,%d,%d,%d,%d", lat, lon, altitude, 30000, 30000, 0, 1200);
    checksum = track_cust_xor_buf(&singlebuf[1], strlen(singlebuf) - 1);
    snprintf(singlebuf + strlen(singlebuf), 99, "*%02X", checksum);
    track_drv_uart2_write_data(singlebuf, strlen(singlebuf));
    LOGD(L_TEST, L_V5, "%s, %d, %d, %x", singlebuf, strlen(singlebuf), checksum, checksum);
}

/******************************************************************************
 *  Function    -  track_cust_secset_interface
 *
 *  Purpose     -  �붨ע����һ�ζ�λ��Ψһ�ӿ�
 *
 *  Description -  
 *  modification history
 * ----------------------------------------
 * v1.0   08-06-2016,  xzq
 * ----------------------------------------
*******************************************************************************/
void track_cust_secset_interface()
{
	float i1=0,i2=0;
	track_stop_timer(TRACK_DRV_ACTIVE_REFTIME_TIMER_ID);
	track_cust_secset(&i1,&i2);
}
/******************************************************************************
 *  Function    -  track_cust_decode_lbs
 *
 *  Purpose     -  ������ת�������ľ�γ��
 *
 *  Description -  
 *  modification history
 * ----------------------------------------
 * v1.0   08-06-2016,  xzq
 * ----------------------------------------
*******************************************************************************/
void track_cust_decode_lbs(float* lat1,float* lon1)
{
	U8 singlebuf[100]={0},checksum;
	U8 gstus=0;
	gstus = track_cust_gps_status();
	s_lat=*lat1;
	s_lon=*lon1;
	if(No_Last_Pos && (gstus<GPS_STAUS_2D && gstus>GPS_STAUS_OFF))
	{
		snprintf(singlebuf, 99, "$PMTK713,%0.6f,%0.6f,%0.0f,%d,%d,%d,%d",s_lat,s_lon,0.00000,30000,30000,0,1200);
		checksum = track_cust_xor_buf(&singlebuf[1],strlen(singlebuf)-1);
   		snprintf(singlebuf+strlen(singlebuf), 99,"*%02X",checksum);
		track_drv_uart2_write_data(singlebuf, strlen(singlebuf));
		LOGD(L_TEST, L_V5, "%s",singlebuf);
		No_Last_Pos=KAL_FALSE;
	}
    LOGD(L_TEST, L_V5, "LBSת�������ĵ� %f, %f, gps_status %d",s_lat,s_lon,gstus);
}


void track_cust_active_aiding()
{
	applib_time_struct currTime = {0};
	applib_dt_get_rtc_time(&currTime);
	LOGD(L_TEST,L_V2,"%0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
	        currTime.nYear, currTime.nMonth, currTime.nDay,
	        currTime.nHour, currTime.nMin, currTime.nSec);
	if(currTime.nYear!=2015)
	{
		track_fun_update_time_to_system(2,&currTime);
	}
}

