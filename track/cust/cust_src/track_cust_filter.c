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
//#include "track_drv.h"
//#include "stdlib.h"
//#include <math.h>

/*****************************************************************************
*  Define                               �궨��
******************************************************************************/
#define PI    3.14159265358979
#define T_PI  0.01745329251994    /* PI/180.0 */
#define EARTH_RAD 6378.1370       /* ȡWGS84��׼�ο������еĵ��򳤰뾶(��λ:km) */
#define MAX_STATIC_DIFFDATA	  0.03

/*****************************************************************************
*  Typedef  Enum                        ö�ٶ���
*****************************************************************************/

/*****************************************************************************
*  Struct                               ���ݽṹ����
******************************************************************************/

/*****************************************************************************
*  Local variable                       �ֲ�����
*****************************************************************************/

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

/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/

void track_cust_filter_upload_position(
    track_gps_data_struct *gpsdata, 
    kal_uint8 gps_just_location,
    gps_gprmc_struct *last_upload_rmc,
    float gps_distance)
{
    static gps_gprmc_struct last_sec_rmc = {0};
    static kal_uint32 g_sec = 0, count = 0;
    static float g_speed = 0;
    float distance, speed;
    kal_uint32 sec;
    track_gps_data_struct *gpsLastPoint=NULL;
    int i, x, j;
    kal_uint8 snr[3] = {0};
    
    while(G_parameter.gps_work.static_filter_sw)
    {
        // ����ͻȻ����ļ��ٶ�
        static int jj = 1;
        static float speed2 = -99;
        static kal_uint32 ff[9] = {0};
        int speed_limit = G_parameter.gps_work.static_filter_distance * 10;
        if(speed2 >= 0 && ((gpsdata->gprmc.Speed - speed2) / jj >= 50))
        {
            jj++;
            gpsdata->filter = 21;
            LOGD(L_APP, L_V7, "ͻȻ������ٶ�%.2f,%.2f,%d,��ǹ���!", speed2, gpsdata->gprmc.Speed, jj);
            break;
        }
        jj = 1;
        speed2 = gpsdata->gprmc.Speed;
        
        if(last_sec_rmc.Latitude != 0 && last_sec_rmc.Longitude != 0)
        {
            sec = applib_get_time_difference_ext_in_second((applib_time_struct *)&last_sec_rmc.Date_Time, (applib_time_struct *)&gpsdata->gprmc.Date_Time);
            distance = track_fun_get_GPS_2Points_distance(last_sec_rmc.Latitude, last_sec_rmc.Longitude, gpsdata->gprmc.Latitude, gpsdata->gprmc.Longitude);
            if((sec == 0 && distance > 77)
                || (sec > 0 && sec <= 5 && (distance / sec) > 77))
            {
                LOGD(L_APP, L_V7, "%d�����ܳ�����%d�ף���ǹ���!", sec, distance);
                gpsdata->filter = 22;
                break;
            }
        }
        memcpy(&last_sec_rmc, &gpsdata->gprmc, sizeof(gps_gprmc_struct));

        g_sec = applib_get_time_difference_ext_in_second((applib_time_struct *)&gpsdata->gprmc.Date_Time, (applib_time_struct *)&last_upload_rmc->Date_Time);
        if(g_sec > 0) g_speed = gps_distance / g_sec;
        else g_speed = gps_distance;
        if(g_speed > 77 && count < 10)
        {
            count++;
            LOGD(L_APP, L_V7, "�ٶȳ���̫��! ��ǹ���!(%.0f,%d,%.2f,%d)", gps_distance, g_sec, g_speed, count);
            gpsdata->filter = 23;
            break;
        }
        count = 0;
        LOGD(L_APP, L_V7, "distance=%.0f, sec=%d, speed=%.2f", gps_distance, g_sec, g_speed);

        if(gps_distance < speed_limit && gpsdata->gprmc.Speed <= 1)
        {
            LOGD(L_APP, L_V7, "�ٶ�:%.2fkm/m������:%.0f, sat:%d����ǹ���!", 
                gpsdata->gprmc.Speed, gps_distance, gpsdata->gpgga.sat);
            gpsdata->filter = 27;
            if(gpsdata->gpgsa.satellite_SNR_used_average >= 39)
            {
                ff[7]++;
                if(ff[7] > 30)
                {
                    ff[7] = 0;
                    gpsdata->filter = 1;
                }
            }
            else ff[7] = 0;
            break;
        }
        ff[7] = 0;

        if(gps_distance < speed_limit && gpsdata->gprmc.Speed <= 6 && gpsdata->gpgsa.satellite_SNR_used_average <= 35)
        {
            LOGD(L_APP, L_V7, "�ٶ�:%.2fkm/m������:%.0f, sat:%d����ǹ���!", 
                gpsdata->gprmc.Speed, gps_distance, gpsdata->gpgga.sat);
            gpsdata->filter = 24;
            break;
        }

        if(gps_distance < G_parameter.gps_work.static_filter_distance && gpsdata->gprmc.Speed <= 6 && gpsdata->gpgsa.satellite_SNR_used_average > 35)
        {
            LOGD(L_APP, L_V7, "%d,�ٶ�:%.2fkm/m������:%.0f, sat:%d����ǹ���!", 
                ff[5], gpsdata->gprmc.Speed, gps_distance, gpsdata->gpgga.sat);
            gpsdata->filter = 25;
            if(gpsdata->gpgsa.satellite_SNR_used_average >= 39 && gps_distance > 60)
            {
                ff[5]++;
                if(ff[5] > 30)
                {
                    ff[5] = 0;
                    gpsdata->filter = 2;
                }
            }
            else ff[5] = 0;
            break;
        }
        ff[5] = 0;

        if(gpsdata->gpgga.sat < 5 && gpsdata->gprmc.Speed < 10 && gps_just_location && gpsdata->gpgsa.satellite_SNR_used_average < 39)
        {
            LOGD(L_APP, L_V7, "ʹ�õĶ�λ����ֻ��%d�ţ����ٶ�%.2f����ǹ���!", gpsdata->gpgga.sat, gpsdata->gprmc.Speed);
            gpsdata->filter = 26;
            break;
        }
        break;
    }
}
