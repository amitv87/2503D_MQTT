/*******************************************************************************************
 * Filename:
 * Author :
 * Date :
 * Comment:
 ******************************************************************************************/

/****************************************************************************
*  Include Files                        包含头文件
*****************************************************************************/
#include "track_cust.h"
/*****************************************************************************
*  Define                               宏定义
******************************************************************************/

/*****************************************************************************
*  Typedef  Enum                        枚举定义
*****************************************************************************/

/*****************************************************************************
*  Struct                               数据结构定义
******************************************************************************/

/*****************************************************************************
*  Local variable                       局部变量
*****************************************************************************/
static float g_distance = 0;//有static限制，但也不能出现同名吧  chengj
static kal_uint8 anglerep_detection_time = 0;
static kal_uint16 Fti_interval_time = 0xFFFE, Fti_interval_time2  = 0;;
static gps_gprmc_struct last_upload_rmc = {0};
static kal_uint8 g_isFirst = 1;
static kal_bool acc_off_position = 0;
static kal_uint32 last_status = 0; /* 满足运动判定速度要求得连续次数 */
static U32 g_gps_distance = 0;
static float g_distance2 = 0;
static kal_bool g_test_acc = KAL_FALSE;
static kal_bool gps_upload = KAL_FALSE;
static U8 gps_upload_count = 0;
static kal_uint8 gps_need_filter = 0;

/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/
extern void track_cust_send_paket_16(kal_uint32 flag, track_gps_data_struct *gd, kal_uint8 call_type);
extern void track_cust_poweron_close_gps(void);
extern void FILTER_gpsData_Check(gps_gprmc_struct* pData);
extern void track_cust_gps_location_get_overtime(void);
extern kal_uint8  track_cust_set_anglerep_upload(kal_uint8 status);
extern void track_cust_fake_cell_low_speed_disable_detect(float speed);

/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/
static void track_cust_net_upload_position_curved_conduit();
static kal_uint8 track_cust_net_upload_timeout(kal_uint8 way, kal_uint8 type);
#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      本地函数
******************************************************************************/

kal_uint16 track_cust_check_gpdfilter_flag(kal_uint8 arg)
{
    if (arg == 1||arg == 0)
    {
        gps_need_filter = arg;
        LOGD(L_APP,L_V5,"gps_need_filter:%d",gps_need_filter);
    }
    else
    {
        LOGD(L_APP,L_V5,"gps_need_filter:%d",gps_need_filter);
        return gps_need_filter;
    }
}

/******************************************************************************
 *  Function    -  fun_net_upload_position
 *
 *  Purpose     -  轨迹点上报
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void fun_net_upload_position(kal_uint8 type, track_gps_data_struct *gpsdata)
{
    static kal_bool flag = 1;  /* 开机首次不标记为补传 */
    kal_uint16 sn;
    U32 PowerOn_Type = mmi_Get_PowerOn_Type();
    
    LOGD(L_GPS, L_V5, "(%d,%d) type:%d, filter:%d, g_isFirst=%d", flag, PowerOn_Type, type, gpsdata->filter, g_isFirst);
    if(track_cust_backup_get_reset_sign() == 1)
    {
        LOGD(L_GPS, L_V4, "WARN: 即将重启设备，GPS数据不在处理。");
        return;
    }
#if !defined(__XCWS__)
    Fti_interval_time = 0;
#endif

    anglerep_detection_time = 0;
    if(g_isFirst & 1)
    {
        gpsdata->filter = 3;
    }
    if(gpsdata->filter == 0)
    {
        track_cust_set_anglerep_upload(1);
    }
#if defined(__GT740__)||defined(__GT420D__)
	if(type == TR_CUST_POSITION_GPSOFF)
	{
		gpsdata->filter=0;
	}
#endif
    if(gpsdata->filter < 20 || (flag && PowerOn_Type != 5))
    {
        LOGD(L_GPS, L_V4, "上传一个定位点");
        track_cust_backup_gps_data(3, gpsdata);
        {
            U8 adc_acc = track_cust_status_acc();
#if defined(__OBD__)
            U8 obd_acc = track_cust_module_obd_get_acc();
            if(G_parameter.mileage_statistics.sw && (adc_acc || obd_acc || g_test_acc))
#elif defined (__NT33__)
	#if defined (__FUN_WITH_ACC__)
		if(G_parameter.mileage_statistics.sw && (adc_acc || g_test_acc))
	#else
		if(G_parameter.mileage_statistics.sw) //没ACC线但是想要里程统计功能
	#endif
			
#else
            if(G_parameter.mileage_statistics.sw && (adc_acc || g_test_acc))
#endif /* __OBD__ */
            {
                if(G_realtime_data.netLogControl & 1024)
                {
                    int ret;
                    char tmp[100];
                    ret = sprintf(tmp, "fun_net_upload_position:(%d,%d) g_gps_distance:%d, g_distance:%f",
                                  G_parameter.mileage_statistics.sw, adc_acc,
                                  g_gps_distance, g_distance);
                    cust_packet_log_data(8, (kal_uint8 *)tmp, ret);
                }
                g_gps_distance += g_distance;
            }
        }
        memcpy(&last_upload_rmc, &gpsdata->gprmc, sizeof(gps_gprmc_struct));
#if defined(__XCWS__)
        if(track_cust_net_upload_timeout(1, type))
        {
            gprs_gps_data_send(gpsdata);
        }
#if defined(__XCWS_TPATH__) && !defined(__KENAER__)
        if(track_cust_net_upload_timeout(0, type))
        {
            sn = track_cust_paket_position(gpsdata, track_drv_get_lbs_data(), type, 0);
        }
#endif/*__XCWS_TPATH__*/
#else
#if defined(__GT500__)
        if(G_realtime_data.location_switch)
#endif
        {
            sn = track_cust_paket_position(gpsdata, track_drv_get_lbs_data(), type, 0);
        }
        if(G_realtime_data.netLogControl & 4)
        {
            cust_packet_log(4, type, gpsdata->filter, sn, g_distance, gpsdata->gprmc.Latitude, gpsdata->gprmc.Longitude);
        }
#endif
             flag = 0;
    }
#if defined(__SERVER_IP_LINK__)
	else if(G_parameter.gpsDup == 1)
#else
     else if(G_parameter.gpsDup == 1&&track_soc_login_status())
#endif
    {
         track_gps_location_dup_updata(gpsdata);
     }
    else if(g_isFirst & 4)
    {
        track_gps_data_struct *gpsLastPoint = track_cust_backup_gps_data(2, NULL);
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)||defined(__GT310__)
        if(track_cust_get_work_mode() == WORK_MODE_3)
        {
            gpsLastPoint = gpsdata;
        }
#endif /* __GT300S__ */
        if(gpsLastPoint->gprmc.status != 0 && (gpsLastPoint->gprmc.Latitude != 0 || gpsLastPoint->gprmc.Longitude != 0))
        {
#if defined(__XCWS__)
            gprs_gps_data_send(gpsLastPoint);
#if defined(__XCWS_TPATH__) && !defined(__KENAER__)
            sn = track_cust_paket_position(gpsLastPoint, track_drv_get_lbs_data(), type, 1);
#endif
#else
#if defined(__GT500__)
            if(G_realtime_data.location_switch)
#endif
            {
#if defined (__CUSTOM_DDWL__)
    //nc
#else
                sn = track_cust_paket_position(gpsLastPoint, track_drv_get_lbs_data(), type, 1);
#endif /* __CUSTOM_DDWL__ */
            }
            if(G_realtime_data.netLogControl & 2)
            {
                cust_packet_log(2, type, gpsdata->filter, sn, g_distance, gpsLastPoint->gprmc.Latitude, gpsLastPoint->gprmc.Longitude);
            }
#endif
        }
        else
        {
            LOGD(L_GPS, L_V5, "NVRAM记录的上一个位置点是无效的。");
            if(G_realtime_data.netLogControl & 6)
            {
                cust_packet_log(6, type, gpsdata->filter, sn, g_distance, gpsLastPoint->gprmc.Latitude, gpsLastPoint->gprmc.Longitude);
            }
        }
    }
    else if(G_realtime_data.netLogControl & 1)
    {
        cust_packet_log(1, type, gpsdata->filter, 0, g_distance, gpsdata->gprmc.Latitude, gpsdata->gprmc.Longitude);
    }
    g_isFirst = 2;
}

/******************************************************************************
 *  Function    -  track_cust_net_upload_position_timed
 *
 *  Purpose     -  定时上报经纬度
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 13-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void track_cust_net_upload_position_timed(track_gps_data_struct *gpsdata)
{
    kal_bool isfilter = KAL_FALSE;
    kal_uint8 acc_status;
#if defined(__GT370__)|| defined (__GT380__)
    acc_status = track_cust_sensor_delay_valid();//在震动模式上传按照interval_acc_on否则interval_acc_off
#elif defined(__GT740__)
    return;
#else
    acc_status = track_cust_status_acc();
#endif
    LOGD(L_GPS, L_V8, "定时上传acc_status=%d, isfilter=%d, Fti_interval_time=%d", acc_status, isfilter, Fti_interval_time);
#if !defined(__ET310__)
    if(track_is_timer_run(TRACK_CUST_FT_WORK_TIMER_ID) && G_parameter.group_package.sw == 1 && track_get_ft_timer() > 0 && track_get_ft_work_timer() > 0)
    {
        Fti_interval_time++;
        if(Fti_interval_time < track_get_ft_timer())
        {
            return;
        }
        LOGD(L_GPS, L_V6, "acc_status=%d, isfilter=%d", acc_status, isfilter);
        fun_net_upload_position(TR_CUST_POSITION_TIMING, gpsdata);
        Fti_interval_time = 0;
        return;
    }
#endif /* __ET310__ */

    if((acc_status == 1 && G_parameter.gps_work.Fti.interval_acc_on == 0)
            || (acc_status == 0 && G_parameter.gps_work.Fti.interval_acc_off == 0))
    {
        return;
    }
    Fti_interval_time++;
#if defined(__GT370__)|| defined (__GT380__)
    if(Fti_interval_time < G_parameter.gps_work.Fti.interval_acc_on)
    {
        return;
    }
#else
    if(((acc_status == 1) && Fti_interval_time < G_parameter.gps_work.Fti.interval_acc_on)
            || ((acc_status == 0) && Fti_interval_time < G_parameter.gps_work.Fti.interval_acc_off))
    {
        return;
    }
#endif
    fun_net_upload_position(TR_CUST_POSITION_TIMING, gpsdata);
    Fti_interval_time = 0;
#if defined(__JM66__)
    LOGD(L_APP, L_V5, "定位点推至队列,关闭GPS");
    track_stop_timer(TRACK_CUST_GPS_LOCATION_WORKTIME_TIMER_ID);
    track_cust_gps_off_req((void*)5);
#endif//__JM66__
}
#if defined(__XCWS__)
static kal_uint8 track_cust_net_upload_timeout(kal_uint8 way, kal_uint8 type)
{
    kal_uint8 acc_status;
    if(type != 0) return TRUE;
    LOGD(L_GPS, L_V5, "%d,%d", Fti_interval_time, Fti_interval_time2);
    if(way == 0)
    {
        acc_status = track_cust_status_acc();
        if(((acc_status == 1) && Fti_interval_time < G_parameter.gps_work.Fti.interval_acc_on)
                || ((acc_status == 0) && Fti_interval_time < G_parameter.gps_work.Fti.interval_acc_off))
        {
            return FALSE;
        }
    }
    else
    {
        if(Fti_interval_time2 < G_parameter.yd_gps_interval)
        {
            return FALSE;
        }
    }
    return TRUE;
}
static void track_cust_net_upload_position_timed2(track_gps_data_struct *gpsdata)
{
    //LOGD(L_GPS, L_V6, "%d,%d", G_parameter.yd_gps_interval, Fti_interval_time2);
#if defined(__XCWS_TPATH__) && !defined(__KENAER__)
    track_cust_net_upload_position_timed(gpsdata);
#endif
    if(G_parameter.yd_gps_interval <= 0)
    {
        return;
    }
    Fti_interval_time2++;
    if(Fti_interval_time2 < G_parameter.yd_gps_interval)
    {
        return;
    }

    fun_net_upload_position(TR_CUST_POSITION_TIMING, gpsdata);
    Fti_interval_time2 = 0;
}
#endif

/******************************************************************************
 *  Function    -  track_cust_net_upload_position_curved_conduit
 *
 *  Purpose     -  拐点补传判断逻辑
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 30-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void track_cust_net_upload_position_curved_conduit(track_gps_data_struct *gpsdata)
{
    if (G_parameter.extchip.mode == 1)
    {
        LOGD(L_GPS, L_V7, "模式一不做拐点补传");
        anglerep_detection_time = 0;
        memcpy(&last_upload_rmc, &gpsdata->gprmc, sizeof(gps_gprmc_struct));
        return;
        }
    if (track_cust_gt420d_carstatus_data(99) == 2)
    {
        LOGD(L_GPS, L_V7, "静止状态下不做拐点补传");
        anglerep_detection_time = 0;
        memcpy(&last_upload_rmc, &gpsdata->gprmc, sizeof(gps_gprmc_struct));
        return;
        }
    if(last_upload_rmc.Latitude == 0 || last_upload_rmc.Longitude == 0)
    {
        LOGD(L_GPS, L_V7, "没有记录到上一次的经度或者纬度，return;");
        anglerep_detection_time = 0;
        memcpy(&last_upload_rmc, &gpsdata->gprmc, sizeof(gps_gprmc_struct));
        return;
        }
    //anglerep_detection_time++;
    //if(anglerep_detection_time < G_parameter.gps_work.anglerep.detection_time)
        //return;
    LOGD(L_APP, L_V7, "检测拐点，%d %d %d;",gpsdata->gpgsa.status,gpsdata->gprmc.Speed,track_fun_abs_degrees(gpsdata->gprmc.Course, last_upload_rmc.Course));
    if(track_cust_gps_status() >= GPS_STAUS_2D && G_parameter.gps_work.anglerep.sw)
    {
        #if defined(__GT420D__)
        if(gpsdata->gpgsa.status >= 2 && gpsdata->gprmc.Speed >= G_parameter.anglerep_speed && track_fun_abs_degrees(gpsdata->gprmc.Course, last_upload_rmc.Course) > G_parameter.gps_work.anglerep.angleOfDeflection)
        #else
        if(gpsdata->gpgsa.status == 3 && gpsdata->gprmc.Speed >= 15 && track_fun_abs_degrees(gpsdata->gprmc.Course, last_upload_rmc.Course) > G_parameter.gps_work.anglerep.angleOfDeflection)
        #endif
        {
            #if defined(__GT420D__)
            track_cust_dsc_location_paket(gpsdata,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_ANGLEREP,0,12);
            track_cust_check_beforepoweroff_flag(0);
            anglerep_detection_time = 0;
            #else
            fun_net_upload_position(TR_CUST_POSITION_ANGLEREP, gpsdata);
            #endif
        }
        else if(gpsdata->gprmc.Speed >= 20 && track_fun_abs_degrees(gpsdata->gprmc.Course, last_upload_rmc.Course) > G_parameter.gps_work.anglerep.angleOfDeflection * 2)
        {
            #if defined(__GT420D__)
            track_cust_dsc_location_paket(gpsdata,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_ANGLEREP,0,12);
            track_cust_check_beforepoweroff_flag(0);
            anglerep_detection_time = 0;
            #else
            fun_net_upload_position(TR_CUST_POSITION_ANGLEREP, gpsdata);
            #endif
        }
        //tr_start_timer(TRACK_CUST_GPS_LOCATION_CURVED_CONDUIT_TIMER_ID, 2000, track_cust_net_upload_position_curved_conduit);
         memcpy(&last_upload_rmc, &gpsdata->gprmc, sizeof(gps_gprmc_struct));
    }
}

/******************************************************************************
 *  Function    -  fixed_distance
 *
 *  Purpose     -  定距上传
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 10-12-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void fixed_distance(track_gps_data_struct *gpsdata)
{
    static kal_uint8 count = 0;
    float res;
    LOGD(L_GPS, L_V7, "定距 distance=%.0f, %d, count=%d", g_distance, G_parameter.gps_work.Fd.distance, count);
    if(g_distance >= G_parameter.gps_work.Fd.distance)
        count++;
    else
        count = 0;
    if(count >= 5)
    {
        count = 0;
        fun_net_upload_position(TR_CUST_POSITION_DISTANCE, gpsdata);
    }
}

/*****************************************************************************
*  Global Functions                     全局函数
******************************************************************************/
kal_uint8  track_cust_set_anglerep_upload(kal_uint8 status)
{
    static U8 no_anglerep_upload = 0;
    if(status == 0 || status == 1)
    {
        return  no_anglerep_upload = status;
    }
    else
    {
        return no_anglerep_upload;
    }
}
kal_bool track_cust_upload_status(void)
{
    return gps_upload;
}


/******************************************************************************
 *  Function    -  track_cust_net_upload_position
 *
 *  Purpose     -  经纬度过滤（静态过滤）
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_net_upload_position(track_gps_data_struct *gpsdata, kal_uint32 gps_just_location_tick)
{
    static gps_gprmc_struct last_sec_rmc = {0};
    static kal_uint32 g_sec = 0, count = 0;
    static float g_speed = 0;
    float distance = 0, speed = 0;
    kal_uint32 sec = 0, tick = 0;
    track_gps_data_struct *gpsLastPoint = NULL;
    int i, x, j;
    kal_uint8 snr[3] = {0}, sat = 0;
    static kal_bool  f_gps_fix_flag = KAL_FALSE;
    static kal_uint32 ff[9] = {0};
    static kal_bool  GPS_short_distance_flag = KAL_FALSE;
    kal_uint16 SNNNO=0;
    //电动车限速设置
    if(G_parameter.speed_limit != 0 && gpsdata->gprmc.Speed > G_parameter.speed_limit)
    {
        LOGD(L_GPS, L_V7, "电动车版本过滤速度大于60KM/H的轨迹");
        return;
    }

    // 计算平均速度
    for(i = 0, j = 0; i < gpsdata->gpgga.sat; i++)
    {
        for(x = 0; x < 16; x++)
        {
            if(gpsdata->gpgsa.satellite_used[i] == gpsdata->gpgsv.Satellite_ID[x])
            {
                gpsdata->gpgsa.satellite_SNR_used[i] = gpsdata->gpgsv.SNR[x];
                if(i < 3)
                {
                    snr[i] = gpsdata->gpgsa.satellite_SNR_used[i];
                }
                else
                {
                    for(j = 0; j < 3; j++)
                    {
                        if(snr[j] < gpsdata->gpgsa.satellite_SNR_used[i])
                        {
                            snr[j] = gpsdata->gpgsa.satellite_SNR_used[i];
                            break;
                        }
                    }
                }
                break;
            }
        }
        //LOGD(L_GPS, L_V4, "satellite:[%d]%d", gpsdata->gpgsa.satellite_used[i], gpsdata->gpgsa.satellite_SNR_used[i]);
    }
    if(gpsdata->gw_mode == 2/*GN*/)
    {
        sat = gpsdata->bdgga.sat;
        for(i = 0, j = 0; i < sat && i < MTK_GPS_SV_MAX_NUM; i++)
        {
            for(x = 0; x < 16; x++)
            {
                if(gpsdata->bdgsa.satellite_used[i] == gpsdata->bdgsv.Satellite_ID[x])
                {
                    gpsdata->bdgsa.satellite_SNR_used[i] = gpsdata->bdgsv.SNR[x];
                    if(i < 3)
                    {
                        snr[i] = gpsdata->bdgsa.satellite_SNR_used[i];
                    }
                    else
                    {
                        for(j = 0; j < 3; j++)
                        {
                            if(snr[j] < gpsdata->bdgsa.satellite_SNR_used[i])
                            {
                                snr[j] = gpsdata->bdgsa.satellite_SNR_used[i];
                                break;
                            }
                        }
                    }
                    break;
                }
            }
            //LOGD(L_GPS, L_V4, "satellite:[%d]%d", gpsdata->bdgsa.satellite_used[i], gpsdata->bdgsa.satellite_SNR_used[i]);
        }
    }
    sat += gpsdata->gpgga.sat;
    gpsdata->gpgsa.satellite_SNR_used_average = (snr[0] + snr[1] + snr[2]) / 3;

    gpsLastPoint = track_cust_backup_gps_data(2, NULL);


/*里程统计g_distance与静态过滤绑定static_filter_sw */
/*关闭SF后不累计里程，缺没有清掉g_distance，重启前使用旧值错误累计       --    chengjun  2017-07-21*/
    while(G_parameter.gps_work.static_filter_sw && gpsLastPoint->gprmc.status != 0 && !track_is_timer_run(TRACK_CUST_FT_WORK_TIMER_ID))
    {
        // 过滤突然增大的加速度
        static int jj = 1;
        static float speed2 = -99;
        int speed_limit = G_parameter.gps_work.static_filter_distance * 10;

        #if defined(__GT420D__)
        if(G_parameter.extchip.mode == 1)
        {
            break;
            }
        #endif
        if(last_upload_rmc.Latitude == 0 || last_upload_rmc.Longitude == 0)
        {
            //if(gpsLastPoint->gprmc.status != 0)
            //{
            LOGD(L_GPS, L_V4, "开机首次进入，NVRAM中有保存上一次定位的有效值,%d, lat:%0.6f, lon:%0.6f", gpsLastPoint->gprmc.status, gpsLastPoint->gprmc.Latitude, gpsLastPoint->gprmc.Longitude);
            g_isFirst &= ~1;
            memcpy(&last_upload_rmc, &gpsLastPoint->gprmc, sizeof(gps_gprmc_struct));
            //}
            //else memcpy(&last_upload_rmc, &gpsdata->gprmc, sizeof(gps_gprmc_struct));
        }
        g_distance = track_fun_get_GPS_2Points_distance(last_upload_rmc.Latitude, last_upload_rmc.Longitude, gpsdata->gprmc.Latitude, gpsdata->gprmc.Longitude);
        if(g_distance < 0) g_distance = -g_distance;

        tick = kal_get_systicks();
        if(tick > (5 * KAL_TICKS_1_MIN))
        {
            tick -= (5 * KAL_TICKS_1_MIN);
        }
        else
        {
            tick = 0;
        }
        //LOGD(L_GPS, L_V7, "distance=%.0f, sec=%d, speed=%.2f", g_distance, g_sec, g_speed);
        if(track_is_timer_run(TRACK_CUST_WORK_MODE_GPSON_UPDATE_TIMER_ID))
        {
            gpsdata->filter = 10;
            break;
        }

        if(g_distance < speed_limit && gpsdata->gprmc.Speed <= 1)
        {
            LOGD(L_GPS, L_V7, "速度:%.2fkm/m，距离:%.0f, sat:%d，标记7过滤%d!",
                 gpsdata->gprmc.Speed, g_distance, sat, ff[7]);
            gpsdata->filter = 27;
            if(gpsdata->gpgsa.satellite_SNR_used_average >= 39 && g_distance > 50)
            {
                ff[7]++;
                if(ff[7] > 60)
                {
                    ff[7] = 0;
                    gpsdata->filter = 1;
                    LOGD(L_GPS, L_V7, "标记7过滤 还原!");
                }
                else
                {
                    break;
                }
            }
            else
            {
                ff[7] = 0;
                break;
            }
        }
        ff[7] = 0;

        if(g_distance < speed_limit && gpsdata->gprmc.Speed <= 7 && gpsdata->gpgsa.satellite_SNR_used_average <= 35)
        {
            LOGD(L_GPS, L_V7, "速度:%.2fkm/m，距离:%.0f, sat:%d，标记4过滤!",
                 gpsdata->gprmc.Speed, g_distance, sat);
            gpsdata->filter = 24;
            break;
        }

        if(sat < 5 && gpsdata->gprmc.Speed < 10 && tick < gps_just_location_tick && gpsdata->gpgsa.satellite_SNR_used_average < 39)
        {
            LOGD(L_GPS, L_V7, "用于定位卫星%d颗，且速度%.2f，转入定位状态未超出5分钟，标记6过滤!", sat, gpsdata->gprmc.Speed);
            gpsdata->filter = 26;
            break;
        }

#if !defined (__GT500__)
        if(sat < 8 && tick < gps_just_location_tick && gpsdata->gpgsa.satellite_SNR_used_average <= 26)
        {
            gpsdata->filter = 28;
            LOGD(L_GPS, L_V7, "用于定位卫星%d颗，且转入定位状态未超出5分钟，平均信号强度%d，标记%d过滤!",
                 sat, gpsdata->gpgsa.satellite_SNR_used_average, gpsdata->filter);
            break;
        }
#endif /* __GT500__ */

        if(sat < 8 && !track_cust_sensor_delay_valid() && gpsdata->gpgsa.satellite_SNR_used_average <= 26)
        {
            gpsdata->filter = 29;
            LOGD(L_GPS, L_V7, "用于定位卫星%d颗，且最近5分钟内并未检测到振动，平均信号强度%d，标记%d过滤!",
                 sat, gpsdata->gpgsa.satellite_SNR_used_average, gpsdata->filter);
            break;
        }

        if(speed2 >= 0 && ((gpsdata->gprmc.Speed - speed2) / jj >= 50))
        {
            jj++;
            gpsdata->filter = 21;
            LOGD(L_GPS, L_V7, "突然增大的速度%.2f,%.2f,%d,标记1过滤!", speed2, gpsdata->gprmc.Speed, jj);
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
                LOGD(L_GPS, L_V7, "%d秒内跑出距离%d米，标记2过滤!", sec, distance);
                gpsdata->filter = 22;
                break;
            }
        }
        memcpy(&last_sec_rmc, &gpsdata->gprmc, sizeof(gps_gprmc_struct));

        g_sec = applib_get_time_difference_ext_in_second((applib_time_struct *)&gpsdata->gprmc.Date_Time, (applib_time_struct *)&last_upload_rmc.Date_Time);
        if(g_sec > 0) g_speed = g_distance / g_sec;
        else g_speed = g_distance;
        if(g_speed > 77 && count < 10)
        {
            count++;
            LOGD(L_GPS, L_V7, "速度超出太多! 标记3过滤!(%.0f,%d,%.2f,%d)", g_distance, g_sec, g_speed, count);
            gpsdata->filter = 23;
            break;
        }
        count = 0;
        if(G_parameter.gps_work.static_filter_sw & 4)
        {
            if(g_distance < 200 && gpsdata->gprmc.Speed <= 7)
            {
                LOGD(L_GPS, L_V7, "%d,速度:%.2fkm/m，距离:%.0f, sat:%d，标记5过滤%d!",
                     ff[5], gpsdata->gprmc.Speed, g_distance, sat, ff[5]);
                gpsdata->filter = 25;
                if(gpsdata->gpgsa.satellite_SNR_used_average >= 35 && g_distance > 100)
                {
                    ff[5]++;
                    if(ff[5] > 60 && GPS_short_distance_flag == FALSE)
                    {
                        GPS_short_distance_flag = TRUE;
                        ff[5] = 0;
                        gpsdata->filter = 2;
                        break;
                        LOGD(L_GPS, L_V7, "标记5过滤 还原!");
                    }
                }
                else ff[5] = 0;
                break;

            }

            if(g_distance < 100 && gpsdata->gprmc.Speed <= 15 && gpsdata->gpgsa.satellite_SNR_used_average < 35 && tick < gps_just_location_tick)
            {
                LOGD(L_GPS, L_V7, "%d,速度:%.2fkm/m，距离:%.0f, sat:%d，标记5过滤%d!",
                     ff[6], gpsdata->gprmc.Speed, g_distance, sat, ff[5]);
                gpsdata->filter = 30;
                break;
            }
        }
        else
        {
            if(g_distance < 200 && gpsdata->gprmc.Speed <= 7 && gpsdata->gpgsa.satellite_SNR_used_average > 35)
            {
                LOGD(L_GPS, L_V7, "%d,速度:%.2fkm/m，距离:%.0f, sat:%d，标记5过滤%d!",
                     ff[5], gpsdata->gprmc.Speed, g_distance, sat, ff[5]);
                gpsdata->filter = 25;
                if(gpsdata->gpgsa.satellite_SNR_used_average >= 39 && g_distance > 100)
                {
                    ff[5]++;
                    if(ff[5] > 60)
                    {
                        ff[5] = 0;
                        gpsdata->filter = 2;
                        LOGD(L_GPS, L_V7, "标记5过滤 还原!");
                    }
                }
                else ff[5] = 0;
                break;
            }
            ff[5] = 0;
        }
        if(track_cust_sensor_delay_valid() && gpsdata->gpgsa.satellite_SNR_used_average > 26 && g_distance > 1000)
        {
            if(ff[1] > 30)
            {
                ff[1] = 0;
                gpsdata->filter = 6;
                LOGD(L_GPS, L_V7, "标记6超出距离，强制上传!");
            }
            else
            {
                ff[1]++;
            }
        }
        else
        {
            ff[1] = 0;
        }
        break;
    }
    
#if defined(__NT31__)
    while(G_parameter.gps_work.static_filter_sw == 0 && gpsLastPoint->gprmc.status != 0 && !track_is_timer_run(TRACK_CUST_FT_WORK_TIMER_ID))
    {
        if(last_upload_rmc.Latitude == 0 || last_upload_rmc.Longitude == 0)
        {
            LOGD(L_GPS, L_V4, "开机首次进入，NVRAM中有保存上一次定位的有效值,%d, lat:%0.6f, lon:%0.6f", gpsLastPoint->gprmc.status, gpsLastPoint->gprmc.Latitude, gpsLastPoint->gprmc.Longitude);
            g_isFirst &= ~1;
            memcpy(&last_upload_rmc, &gpsLastPoint->gprmc, sizeof(gps_gprmc_struct));
        }
        g_distance = track_fun_get_GPS_2Points_distance(last_upload_rmc.Latitude, last_upload_rmc.Longitude, gpsdata->gprmc.Latitude, gpsdata->gprmc.Longitude);
        if(g_distance < 0) g_distance = -g_distance;
        break;
    }
#endif
    #if defined(__GT420D__)
    if(gpsdata->filter < 20)
    {
        track_cust_check_gpdfilter_flag(1);
    }
    #endif
#if defined (__NT37__)
if(G_parameter.gps_work.static_filter_sw == 0)
{
    if(g_distance!=0)
    {
        g_distance=0;
    }
}
#endif

    LOGD(L_GPS, L_V7, "filter:%d lat & lon:(%0.6f,%0.6f;%0.6f,%0.6f) speed:%0.2f sat:%d average:%d distance:%0.2f, %0.2f, %d, g_isFirst:%d",
         gpsdata->filter,
         last_sec_rmc.Latitude, last_sec_rmc.Longitude,
         gpsdata->gprmc.Latitude, gpsdata->gprmc.Longitude,
         gpsdata->gprmc.Speed, sat,
         gpsdata->gpgsa.satellite_SNR_used_average,
         distance, g_distance, tick - gps_just_location_tick, g_isFirst);
#if defined(__GT420D__)
    if(G_parameter.gps_work.anglerep.sw)
    {
        track_cust_net_upload_position_curved_conduit(gpsdata);
    }
    return;
#endif
#if defined(__GPRS_SEND_FILTER__)
    if(G_parameter.gps_work.static_filter_sw & 2)
    {
        FILTER_gpsData_Check(&gpsdata->gprmc);
        LOGD(L_GPS, L_V8, "filter:%d lat:%0.6f lon:%0.6f speed:%0.2f sat:%d average:%d distance:%0.2f, %0.2f, %d, g_isFirst:%d",
             gpsdata->filter,
             last_sec_rmc.Latitude, last_sec_rmc.Longitude,
             gpsdata->gprmc.Speed, sat,
             gpsdata->gpgsa.satellite_SNR_used_average,
             distance, g_distance, tick - gps_just_location_tick, g_isFirst);
    }
#endif /* __GPRS_SEND_FILTER__ */


    /* 超速报警 */
    track_cust_module_speed_limit(gpsdata);
#if !defined(__XCWS__)
    /* 定距上传 */
    if(G_parameter.gps_work.Fd.sw && G_parameter.gps_work.Fd.distance > 0)
    {
        fixed_distance(gpsdata);
    }
#endif
    if(WORK_MODE_3 == track_cust_get_work_mode() && 1 == track_cust_mode3_uping(0xff))
    {
#if defined(__GT310__)
		track_mode3_upload_location(GPS_FIX);
		track_cust_mode3_upload_location((void*)1);
#else
        track_cust_mode3_update_LBS(2);
#endif
    }
    else
    {
        /* 定时上传 */
#if defined(__XCWS__)
        if(track_cust_is_filter_send() && (G_parameter.t_trace == 1))
        {
            track_cust_net_upload_position_timed2(gpsdata);
        }
#elif defined(__JM66__)
        if(Jm66_Current_mode > 10 && G_parameter.gps_work.Fti.interval_acc_off > 0)
#else
        if(G_parameter.gps_work.Fti.sw && (G_parameter.gps_work.Fti.interval_acc_on > 0 || G_parameter.gps_work.Fti.interval_acc_off > 0))
#endif
        {
            track_cust_net_upload_position_timed(gpsdata);
        }
    }

/* 拐点补传 */
#if defined (__GT370__)||defined(__GT380__)
    if(track_cust_get_work_mode()==2)
    {
        if(G_parameter.gps_work.static_filter_sw & 4)
        {
            if(G_parameter.gps_work.anglerep.sw && track_cust_sensor_delay_valid() && track_cust_set_anglerep_upload(99))
            {
                track_cust_net_upload_position_curved_conduit(gpsdata);
            }
        }
        else if(G_parameter.gps_work.anglerep.sw)
        {
            track_cust_net_upload_position_curved_conduit(gpsdata);
        }
    }
#else
    if(G_parameter.gps_work.static_filter_sw & 4)
    {
        if(G_parameter.gps_work.anglerep.sw && track_cust_sensor_delay_valid() && track_cust_set_anglerep_upload(99))
        {
            track_cust_net_upload_position_curved_conduit(gpsdata);
        }
    }
    else if(G_parameter.gps_work.anglerep.sw)
    {
        track_cust_net_upload_position_curved_conduit(gpsdata);
    }
#endif /* __GT370__ */


    /* ACC OFF 补传 */
    if(acc_off_position)
    {
        gpsdata->filter = 4;
#if !defined(__XCWS__)
        fun_net_upload_position(TR_CUST_POSITION_ACC_CHANGE, gpsdata);
#endif
    }

#if defined (__NT31__)||defined(__NT36__)||defined(__NT33__)
    if(G_realtime_data.flag == 1)
    {
        track_cust_module_sd_check(gpsdata);
    }
#endif /* __GT02F__ */
    if(gpsdata->filter < 20)
    {
        if(GPS_short_distance_flag && (G_parameter.gps_work.static_filter_sw & 4))
        {
            ff[2]++;
            if(ff[2] > 50)
            {
                GPS_short_distance_flag = FALSE;
                ff[2] = 0;
            }
        }
        /* 位移报警 */
        track_cust_module_sd_check(gpsdata);
#if !defined(__XCWS__)
        /* 电子围栏 */
        for(i = 0; i < TRACK_DEFINE_FENCE_SUM; i++)
        {
            track_cust_module_fence_gps(gpsdata, i);
        }
#endif
    }
#if defined(__ET310__) ||defined(__MT200__) ||defined(__NT36__)||defined (__ET320__)
    if(track_is_timer_run(TRACK_CUST_LED_START_TIMER) == TRUE)
    {
        track_stop_timer(TRACK_CUST_LED_START_TIMER);
        tr_start_timer(TRACK_CUST_LED_OFF_TIMER, 3 * 60000, track_cust_led_first_start);
    }
#elif defined(__NT31__)
    if((track_is_timer_run(TRACK_CUST_LED_START_TIMER) == TRUE) && (track_soc_login_status() == KAL_TRUE))
    {
        track_stop_timer(TRACK_CUST_LED_START_TIMER);
        tr_start_timer(TRACK_CUST_LED_OFF_TIMER, 3 * 60000, track_cust_led_first_start);
    }
#endif /* __NT31__ */

#if defined( __GT310__)||defined( __GT370__)|| defined (__GT380__)||defined( __GT500__) ||defined(__ET200__) ||defined(__ET130__)  || defined(__GW100__)|| defined(__V20__) || defined(__ET310__)||defined(__GT300S__)\
     || defined(__MT200__)||defined (__ET320__)||defined (__JM66__)
    if(f_gps_fix_flag == KAL_FALSE)
    {
        f_gps_fix_flag = KAL_TRUE;
#if defined (__GW100__) || defined (__GT370__)|| defined (__GT380__)||defined (__JM66__)
    //NO GPS fix alarm
#else
        if(mmi_Get_PowerOn_Type() == PWRKEYPWRON || CHRPWRON == mmi_Get_PowerOn_Type())
        {
            track_cust_alarm_gps_successfully();
        }
#endif /* __GW100__ */
    }
    if(track_is_timer_run(TRACK_CUST_ALARM_GPS_WORK_TIMER) == TRUE)
    {
        track_cust_send_paket_16(1, gpsdata, 0);
    }
#ifdef __GT310__
    if(track_is_timer_run(TRACK_CUST_DW_GPS_WORK_TIMER) == TRUE)
#else
    if(track_is_timer_run(TRACK_CUST_DW_GPS_WORK_TIMER) == TRUE && track_is_timer_run(TRACK_CUST_GET_ADDRESS_OVER3_TIMER_ID))
#endif /* __GW100__ */
    {
        track_cust_send_paket_1a(1, NULL);
        track_cust_gps_location_get_overtime();
        LOGD(L_GPS, L_V7, " 打定时器%d,%d", track_is_timer_run(TRACK_CUST_DW_GPS_WORK_TIMER), track_is_timer_run(TRACK_CUST_GET_ADDRESS_OVER3_TIMER_ID));
    }
    else
    {
        LOGD(L_GPS, L_V7, "%d,%d", track_is_timer_run(TRACK_CUST_DW_GPS_WORK_TIMER), track_is_timer_run(TRACK_CUST_GET_ADDRESS_OVER3_TIMER_ID));
    }
    if(track_is_timer_run(TRACK_CUST_URL_OVER_TIMER_ID) == TRUE)
    {
        track_stop_timer(TRACK_CUST_URL_OVER_TIMER_ID);
        track_cust_gt03_url((void*)1);
        track_cust_gps_location_timeout(10);
    }
    if(track_is_timer_run(TRACK_CUST_WHERE_OVER_TIMER_ID) == TRUE)
    {
        track_stop_timer(TRACK_CUST_WHERE_OVER_TIMER_ID);
        track_cust_gt03_where((void*)1);
        track_cust_gps_location_timeout(10);
    }

    if(track_is_timer_run(TRACK_CUST_WORK_MODE3__GPSON_TIMER_ID) == TRUE)
    {
        track_stop_timer(TRACK_CUST_WORK_MODE3__GPSON_TIMER_ID);
        track_cust_gps_location_timeout(G_parameter.mode3_up_counts + 1); //
    }

#if defined(__GT310__) || defined(__GW100__)||defined(__GT300S__)
    if(track_is_timer_run(TRACK_CUST_KEY_UPDATA_OVER_TIMER_ID) == TRUE)
    {
        track_stop_timer(TRACK_CUST_KEY_UPDATA_OVER_TIMER_ID);
        fun_net_upload_position(TR_CUST_POSITION_KEY, gpsdata);
#if defined(__GT310__)
        track_cust_key_double_sos_updata_position((void*)1);
#else
        track_cust_key_updata_position((void*)1);


#endif
        track_cust_gps_location_timeout(10);
    }
#endif /* __GT300__ */
    if(track_is_timer_run(TRACK_CUST_POWERON_CLOSE_GPS_TIMER) == TRUE)
    {
        tr_stop_timer(TRACK_CUST_POWERON_CLOSE_GPS_TIMER);
        tr_start_timer(TRACK_CUST_OVER_TIMER_GPSOFF_TIMER_ID, 5 * 1000, track_cust_poweron_close_gps);
    }
#if defined(__XCWS__)
    if(track_is_timer_run(TRACK_CUST_CALL_GPS_POSITION_TIMER_ID))
    {
        track_cust_call_gps_position((void*)3);
    }
#endif
#endif
    acc_off_position = 0;
    {
        //静止后补传最后位置
        static kal_uint32 count = 0;
        if(gpsdata->gprmc.Speed >= G_parameter.staticrep.speed)
        {
            if(last_status < 0xFFFF)
            {
                last_status++;
            }
        }
        else
        {
            if(last_status <= G_parameter.staticrep.count)
            {
                last_status = 0;
            }
        }
    }

    track_cust_fake_cell_low_speed_disable_detect(gpsdata->gprmc.Speed);
    #if defined(__GT420D__)
    if(G_parameter.extchip.mode == 1)
   	{
		track_cust_module_delay_close(30);
	}
    #endif
}

/******************************************************************************
 *  Function    -  track_cust_gpsmode_net_new_conn
 *
 *  Purpose     -  TCP重连标记
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_gpsmode_net_new_conn(void)
{
    g_isFirst |= 4;
    LOGD(L_GPS, L_V6, "g_isFirst=%d", g_isFirst);
    Fti_interval_time = 0xFFFE;
}

/******************************************************************************
 *  Function    -  track_cust_add_position
 *
 *  Purpose     -  ACC变化，强制上传定位点
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_add_position(void)
{
    LOGD(L_GPS, L_V5, "");
    if(G_parameter.gps_work.accrep == 0)
    {
        return;
    }
    //避免不满足打包条件，熄火后一直看不到定位数据的问题。
   // if(track_cust_status_acc() != 1)
    {
        track_cust_instant_contract_position_data(99);
    }
    if(track_cust_gps_status() >= GPS_STAUS_2D)
    {
        acc_off_position = 1;
    }
    else
    {
        #if defined(__NT31__)
        track_gps_data_struct *p_gpsdata;
    if(track_cust_gps_status() >= GPS_STAUS_2D)
    {
        p_gpsdata = track_cust_backup_gps_data(0, NULL);
    }
    else
    {
        p_gpsdata = track_cust_backup_gps_data(2, NULL);
    }
    #else
        track_gps_data_struct *p_gpsdata = track_cust_gpsdata_alarm();
    #endif
    
        if(p_gpsdata->gprmc.status != 0 && (p_gpsdata->gprmc.Latitude != 0 || p_gpsdata->gprmc.Longitude != 0))
        {
            kal_uint16 sn;
            if(p_gpsdata->gprmc.status == 1)
            {
                track_cust_backup_gps_data(3, p_gpsdata);
                {
                    U8 adc_acc = track_cust_status_acc();
#if defined(__OBD__)
                    U8 obd_acc = track_cust_module_obd_get_acc();
                    if(G_parameter.mileage_statistics.sw && (adc_acc || obd_acc || g_test_acc))
#elif defined (__NT33__)
	#if defined (__FUN_WITH_ACC__)
					if(G_parameter.mileage_statistics.sw && (adc_acc || g_test_acc))

  	#else
                    if(G_parameter.mileage_statistics.sw) //没ACC线但是想要里程统计功能
	#endif
#else
                    if(G_parameter.mileage_statistics.sw && (adc_acc || g_test_acc))
#endif /* __OBD__ */
                    {
                        if(G_realtime_data.netLogControl & 1024)
                        {
                            int ret;
                            char tmp[100];
                            ret = sprintf(tmp, "fun_net_upload_position:(%d,%d) g_gps_distance:%d, g_distance:%f",
                                          G_parameter.mileage_statistics.sw, adc_acc,
                                          g_gps_distance, g_distance);
                            cust_packet_log_data(8, (kal_uint8 *)tmp, ret);
                        }
                        g_gps_distance += g_distance;
                    }
                }
            }
#if defined(__GT500__)
            if(G_realtime_data.location_switch)
            {
                sn = track_cust_paket_position(p_gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_ACC_CHANGE, 1);
            }
#else
            sn = track_cust_paket_position(p_gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_ACC_CHANGE, 0);
#endif
            if(G_realtime_data.netLogControl & 2)
            {
                cust_packet_log(4, TR_CUST_POSITION_ACC_CHANGE, 0, sn, g_distance, p_gpsdata->gprmc.Latitude, p_gpsdata->gprmc.Longitude);
            }
        }
        else
        {
            LOGD(L_GPS, L_V6, "设备一次定位都未完成过");
        }
    }
}


/******************************************************************************
 *  Function    -  track_cust_sensor_add_position
 *
 *  Purpose     -  静止一段时间后补传最后一个点
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_sensor_add_position(void *arg)
{
    kal_uint32 par = (kal_uint32)arg;
    
    #if defined (__NT31__)
    track_gps_data_struct new_gpsdata = {0};
    applib_time_struct currTime = {0};
    #endif
    
    if(par == 0)
    {
        LOGD(L_SENSOR, L_V5, "%d,%d,    %d,%d,%d,%d",
             par, last_status,
             G_parameter.staticrep.sw, G_parameter.staticrep.detection_time,
             G_parameter.staticrep.speed, G_parameter.staticrep.count);
    }
    else
    {
        LOGD(L_GPS, L_V6, "%d,%d,    %d,%d,%d,%d",
             par, last_status,
             G_parameter.staticrep.sw, G_parameter.staticrep.detection_time,
             G_parameter.staticrep.speed, G_parameter.staticrep.count);
    }
    if(par == 0 && last_status > G_parameter.staticrep.count)
    {
        if(G_parameter.staticrep.detection_time > 0)
        {
            LOGD(L_GPS, L_V6, "%d", G_parameter.staticrep.detection_time);
            track_start_timer(
                TRACK_CUST_SENSOR_LOCATION_STATICREP_TIMER_ID,
                G_parameter.staticrep.detection_time * 1000,
                track_cust_sensor_add_position,
                (void*)1);
        }
    }
    else if(par == 1)
    {
        if(G_parameter.staticrep.sw != 0)
        {
            track_gps_data_struct *gpsdata = track_cust_backup_gps_data(0, NULL);
            LOGD(L_GPS, L_V5, "静止补传最后一个定位点");
            if(gpsdata->gprmc.status != 0 && (gpsdata->gprmc.Latitude != 0 || gpsdata->gprmc.Longitude != 0) && gpsdata->gprmc.Speed <= G_parameter.staticrep.speed)
            {
                kal_uint16 sn;
                track_cust_backup_gps_data(3, gpsdata);
                {
                    U8 adc_acc = track_cust_status_acc();
#if defined(__OBD__)
                    U8 obd_acc = track_cust_module_obd_get_acc();
                    if(G_parameter.mileage_statistics.sw && (adc_acc || obd_acc || g_test_acc))
#elif defined (__NT33__)
			#if defined (__FUN_WITH_ACC__)
					if(G_parameter.mileage_statistics.sw && (adc_acc || g_test_acc))
						
			#else
					if(G_parameter.mileage_statistics.sw) //没ACC线但是想要里程统计功能
			#endif

#else
                    if(G_parameter.mileage_statistics.sw && (adc_acc || g_test_acc))
#endif /* __OBD__ */
                    {
                        if(G_realtime_data.netLogControl & 1024)
                        {
                            int ret;
                            char tmp[100];
                            ret = sprintf(tmp, "fun_net_upload_position:(%d,%d) g_gps_distance:%d, g_distance:%f",
                                          G_parameter.mileage_statistics.sw, adc_acc,
                                          g_gps_distance, g_distance);
                            cust_packet_log_data(8, (kal_uint8 *)tmp, ret);
                        }
                        g_gps_distance += g_distance;
                    }
                }
                memcpy(&last_upload_rmc, &gpsdata->gprmc, sizeof(gps_gprmc_struct));
#if defined(__GT500__)
                if(G_realtime_data.location_switch)
                {
                    sn = track_cust_paket_position(gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_STATIC, 1);
                }
#else
                #if defined (__NT31__)
                gpsdata->gprmc.Speed = 0;
                if(track_cust_is_upload_UTC_time())
                    {
                        track_fun_get_time(&currTime, KAL_TRUE, NULL);
                        }
                else
                    {
                        track_fun_get_time(&currTime, KAL_FALSE, &G_parameter.zone);
                        }
                LOGD(L_GPS, L_V6, "%0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
                currTime.nYear, currTime.nMonth, currTime.nDay,
                currTime.nHour, currTime.nMin, currTime.nSec);
                memcpy(&new_gpsdata.gprmc.Date_Time, &currTime, sizeof(applib_time_struct));
                memcpy(&new_gpsdata, gpsdata, sizeof(track_gps_data_struct));
                sn = track_cust_paket_position(&new_gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_STATIC, 0);
                #else
                sn = track_cust_paket_position(gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_STATIC, 0);
                #endif
#endif
                if(G_realtime_data.netLogControl & 2)
                {
                    cust_packet_log(4, TR_CUST_POSITION_STATIC, 0, sn, g_distance, gpsdata->gprmc.Latitude, gpsdata->gprmc.Longitude);
                }
            }
            else
            {
                LOGD(L_GPS, L_V5, "NVRAM记录的上一个位置点是无效的。(%0.2f>%d)", gpsdata->gprmc.Speed, G_parameter.staticrep.speed);
            }
            if(gpsdata->gprmc.Speed > G_parameter.staticrep.speed && track_cust_gps_status() > GPS_STAUS_2D)
            {
                LOGD(L_GPS, L_V5, "位置点速度高于设定值重置发送定时器(%0.2f>%d)", gpsdata->gprmc.Speed, G_parameter.staticrep.speed);
                track_start_timer(
                    TRACK_CUST_SENSOR_LOCATION_STATICREP_TIMER_ID,
                    G_parameter.staticrep.detection_time * 1000,
                    track_cust_sensor_add_position,
                    (void*)1);
            }
            else
            {
                last_status = 0;
            }
        }
        else
        {
            track_soc_lcon_send_sensor();
        }
    }
}

/******************************************************************************
 *  Function    -  track_cust_sf_switch
 *
 *  Purpose     -  静态过滤开关切换
 *
 *  Description -  参数     含义
 *                          0            不过滤，持续上报数据
 *                          1            静态过滤
 *                          0xFF         读状态
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 30-01-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint8 track_cust_sf_switch(kal_uint8 sw)
{
    if(sw == 0)
    {
        G_parameter.gps_work.static_filter_sw = 0;
    }
    else if(sw == 1)
    {
        G_parameter.gps_work.static_filter_sw = 1;
    }
    else if(sw == 0xFF)
    {

    }
    return G_parameter.gps_work.static_filter_sw;
}

/******************************************************************************
 *  Function    -  track_cust_gpsdata_alarm
 *
 *  Purpose     -  报警的时候取得应该上传的经纬度和时间
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 08-08-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
track_gps_data_struct *track_cust_gpsdata_alarm(void)
{
    track_gps_data_struct *p_gpsdata = track_cust_backup_gps_data(0, NULL);
    
    if(track_cust_gps_status() < GPS_STAUS_2D || p_gpsdata->filter >= 20)
    {
        static track_gps_data_struct gpsdata = {0};
        memcpy(&gpsdata, p_gpsdata, sizeof(track_gps_data_struct));
        if(track_fun_update_time_to_system(99, NULL) > 0)
        {
            applib_time_struct currTime = {0};
            if(track_cust_is_upload_UTC_time())
            {
                track_fun_get_time(&currTime, KAL_TRUE, NULL);
            }
            else
            {
                track_fun_get_time(&currTime, KAL_FALSE, &G_parameter.zone);
            }
            LOGD(L_GPS, L_V6, "%0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
                 currTime.nYear, currTime.nMonth, currTime.nDay,
                 currTime.nHour, currTime.nMin, currTime.nSec);
            memcpy(&gpsdata.gprmc.Date_Time, &currTime, sizeof(applib_time_struct));
        }
        if(gpsdata.gprmc.status == 1)
        {
            gpsdata.gprmc.status = 2;
        }
        p_gpsdata = &gpsdata;
    }
    return p_gpsdata;
}

U32 track_get_gps_mileage(void)
{
    return G_parameter.mileage_statistics.mileage + (int)((double)g_gps_distance * ((double)G_parameter.mileage_statistics.K / 1000.0));
}

U32 track_get_gps_distance(void)
{
    return g_gps_distance;
}

void track_set_gps_distance(U32 distance)
{
    nvram_gps_last_data_struct backup = {0};
    g_gps_distance = distance;
    track_cust_nvram_gps_last_rmc(1, (kal_uint8*)&backup);//read
    backup.distance = distance;
    track_cust_nvram_gps_last_rmc(2, (kal_uint8*)&backup);//write
}

void track_gps_distance_init(void)
{
    nvram_gps_last_data_struct backup = {0};
    track_cust_nvram_gps_last_rmc(1, (kal_uint8*)&backup);
    g_gps_distance = backup.distance;
}

void track_cust_gpsmode_acc_test(CMD_DATA_STRUCT *cmd)
{
    g_test_acc = atoi(cmd->pars[2]);
    sprintf(cmd->rsp_msg, "g_test_acc:%d", g_test_acc);
}

kal_uint8 track_cust_gps_upload_count()
{
    return gps_upload_count;
}

#if defined(__SV_UPLOAD__)
void track_cust_net_sv_upload(U8 mode)
{
    static U32 endtick = 0, nexttick = 0;
    if(G_parameter.sv.sw == 1)
    {
        U32 tick = OTA_kal_get_systicks();
#if defined(__NT31__)        
        if(mode == 0 || endtick == 0 || G_parameter.sv.t2 == 0)
#else
        if(mode == 0 || endtick == 0)
#endif   
        {
#if defined(__NT31__)
            if(G_parameter.sv.t2 >= 0) endtick = tick + G_parameter.sv.t2 * KAL_TICKS_1_MIN;
#else
            if(G_parameter.sv.t2 > 0) endtick = tick + G_parameter.sv.t2 * KAL_TICKS_1_MIN;
#endif
        }
        if(endtick < tick)
        {
            endtick = 0;
            nexttick = 0;
            G_parameter.sv.sw = 0;
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
            return;
        }
        if(nexttick > tick)
        {
            return;
        }
        if(!track_soc_login_status())
        {
            return;
        }
        if(G_parameter.sv.t1 > 0) nexttick = tick + G_parameter.sv.t1 * KAL_TICKS_1_SEC;
        track_cust_paket_9409_handle();
    }
}
#endif/*GT500L*/

