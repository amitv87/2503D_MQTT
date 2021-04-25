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
#include "track_os_ell.h"
/*****************************************************************************
*  Define                               宏定义
******************************************************************************/

#if defined( __GW100__)
#define  WHERE_GPS_WORK_TIME   3 * 60 * 1000
#elif defined( __GT300S__) || defined(__GT370__)|| defined (__GT380__)|| defined(__GT740__)||defined(__GT310__)||defined(__GT420D__)
#define  WHERE_GPS_WORK_TIME   2 * 60 * 1000
#elif defined( __JM66__)
#define  WHERE_GPS_WORK_TIME   280 * 1000
#else
#define  WHERE_GPS_WORK_TIME   5 * 60 * 1000
#endif /* __GW100__ */

#if defined( __GT310__)|| defined(__GT420D__)
#define  UP_TIME   5
#else
#define  UP_TIME   10
#endif

#define  EXTCHANGE_MODE_GPS_WORK_TIME  2 * 60 * 1000

/*****************************************************************************
*  Typedef  Enum                        枚举定义
*****************************************************************************/

/*****************************************************************************
*  Struct                               数据结构定义
******************************************************************************/

/*****************************************************************************
*  Local variable                       局部变量
*****************************************************************************/
static track_enum_gps_staus gps_status = 0;   // 0:GPS关 1:GPS开 2:搜星 3:2D定位 4:3D定位
static track_enum_gps_staus gps_before_off_status = 1;
static kal_uint8 net_upload_position = 0; //在GPS开启的状态下，进一步控制是否需要上报定位点到服务器
static kal_uint8 g_restart_gps_count = 0;
static kal_bool positioned = KAL_FALSE;     // 是否 开机之后曾经定位过
static kal_uint8 GPS_NOT_OPEN = 0;// 不允许开启GPS

/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/
#if defined (__CUSTOM_DDWL__)
extern kal_uint8 g_daily_gps_fix;
#endif /* __CUSTOM_DDWL__ */

/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/
extern void track_cust_gps_nodata_restart(void);
extern  void track_cust_sleep_mode(void);
#if !defined(__GT420D__)
extern void track_cust_module_get_addr(CMD_DATA_STRUCT *_cmd);
#endif
#if defined( __GT310__)
extern void track_cust_key_double_sos_updata_position(void * operate);
#elif defined(__GW100__)||defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
extern void track_cust_key_updata_position(void * operate);
#endif
extern kal_uint32 track_cust_gpson_timer(kal_uint32 time);
#if defined(__BCA__)
extern void track_cust_shocks_lbs_update(void * arg);
#endif
#if defined(__GT300S__)
void track_mode1_gt300s_reset_work(void);
#elif defined(__GT370__)|| defined (__GT380__)
void track_mode1_gt370_reset_work(void);
kal_uint16 track_gt370_get_upload_time(void);
void track_mode1_no_gps_send_lbs(void);
#endif /* __GT300S__ */

/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/
static void gps_location_timeout_ind(void * arg);
void track_cust_gps_work_req(void *arg);
void track_cust_mode3_update_LBS(void *f);
void track_cust_intermittent_send_LBS(void *arg);
void track_cust_instant_contract_position_data(kal_uint8 flag);
#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      本地函数
******************************************************************************/


/******************************************************************************
 *  Function    -  gps_location_timeout_ind
 *
 *  Purpose     -  搜星定位超时的处理函数
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 04-09-2012,  Cml  written
 * v1.1  , 07-11-2013,  ZengPing && JWJ  written
 * ----------------------------------------
*******************************************************************************/
static void gps_location_timeout_ind(void * arg)
{
    kal_uint8 work_mode = track_cust_get_work_mode();
    U8 par = (U8)arg;
#if defined(__GT420D__)
    LBS_Multi_Cell_Data_Struct *lbs = track_drv_get_lbs_data();
#endif
    LOGD(L_GPS, L_V5, "gps_mode:%d,%d", work_mode, par);
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
    if (track_cust_get_work_mode() == WORK_MODE_3)
    {
        return;
    }
#endif
#if defined(__JM66__)
    if (par == 7 || par == 12)
    {
        LBS_Multi_Cell_Data_Struct *lbs = track_drv_get_lbs_data();
        LOGD(L_APP, L_V5, "定位超时,发送LBS定位包%d", par);
        track_cust_paket_18(lbs, 0);
    }
    if (Jm66_Current_mode == 2 && par == 12)
    {
        extern void track_cust_jm66_shutdown(void);
        tr_start_timer(TRACK_CUST_KEY_DOWN_DELAY_SHUTDOWN_TIMER_ID, 120 * 1000, track_cust_jm66_shutdown);
    }
#endif//__JM66__
#if defined(__GT03F__)|| defined(__GT300__)|| defined(__GT500__)||defined(__GT02__)||defined(__GT02E__)|| defined(__ET130__) || defined(__ET200__) || defined(__NT23__) || defined(__GT06D__)\
    ||defined(__GW100__)|| defined(__V20__) || defined(__ET310__)||defined (__ET320__)||defined(__GT300S__) || defined(__MT200__)||defined(__NT36__) || defined(__GT370__)|| defined (__GT380__)|| defined(__JM66__)|| defined(__GT420D__)
    if ((!track_is_timer_run(TRACK_CUST_ALARM_GPS_WORK_TIMER)) && (!track_is_timer_run(TRACK_CUST_GPS_LOCATION_TIMEOUT_TIMER_ID)) \
            && (!track_is_timer_run(TRACK_CUST_POWERON_CLOSE_GPS_TIMER)) && (!track_is_timer_run(TRACK_CUST_DW_GPS_WORK_TIMER)) \
            && (!track_is_timer_run(TRACK_CUST_WHERE_GPS_WORK_TIMER_ID)) && (!track_is_timer_run(TRACK_CUST_KEY_UPDATA_OVER_TIMER_ID))\
            && (!track_is_timer_run(TRACK_CUST_GPS_LOCATION_WORKTIME_TIMER_ID)) && (!track_is_timer_run(TRACK_CUST_WORK_MODE_GPSUPDATE_OVERTIME_TIMER_ID))\
            && (!track_is_timer_run(TRACK_CUST_GPS_UPDATE_EPHEMERIS_OVERTIME_TIMER_ID)) && (!track_is_timer_run(TRACK_CUST_WORK_MODE_GPSON_UPDATE_TIMER_ID))
            && (!track_is_timer_run(TRACK_CUST_WORK_MODE3__GPSON_TIMER_ID))  && (!track_is_timer_run(TRACK_CUST_FT_WORK_TIMER_ID)))
    {
#if defined (__GT500__) || defined(__GT02__) || defined(__GT02E__) || defined(__ET130__) || defined(__ET200__) || defined(__NT23__) || defined(__GT06D__)|| defined(__V20__) || defined(__ET310__)\
     || defined(__MT200__)||defined(__NT36__)||defined (__ET320__)

#if defined(__XCWS__)
        if (((track_cust_status_acc() == 0 && track_drv_get_gps_RMC()->Speed < 20) || track_cust_status_charger() == 0) && G_parameter.sensor_gps_work != 0)//(track_cust_status_charger() == 0 && track_drv_get_gps_RMC()->Speed < 20)
#else
        if (track_cust_status_acc() == 0 && track_drv_get_gps_RMC()->Speed < 20 && (G_parameter.sensor_gps_work != 0 || G_realtime_data.power_saving_mode == 1))
#endif
        {
            track_cust_gps_off_req((void*)3);
        }
        else
        {
            LOGD(L_GPS, L_V5, "track_cust_status_acc() =%d,seep=%d,G_parameter.sensor_gps_work=%d ", track_cust_status_acc(), track_drv_get_gps_RMC()->Speed, G_parameter.sensor_gps_work);
            LOGD(L_GPS, L_V5, "reset gps timer time=%d m", G_parameter.gps_work.location_timeout);

            track_start_timer(TRACK_CUST_GPS_LOCATION_TIMEOUT_TIMER_ID, G_parameter.gps_work.location_timeout * 60000, gps_location_timeout_ind, (void*)1);
        }
#elif defined(__GT370__)|| defined (__GT380__)

        if ((track_cust_get_work_mode()==2)&&(track_drv_get_gps_RMC()->Speed > 20)) //运动的时候不要关GPS
        {
            LOGD(L_GPS, L_V5, "(mode2&&speed>20),reset gps timer=%dmin", G_parameter.gps_work.location_timeout);
            track_start_timer(TRACK_CUST_GPS_LOCATION_TIMEOUT_TIMER_ID, G_parameter.gps_work.location_timeout * 60000, gps_location_timeout_ind, (void*)1);
        }
        else if (G_parameter.sensor_gps_work == 0)
        {
            track_start_timer(TRACK_CUST_GPS_LOCATION_TIMEOUT_TIMER_ID, G_parameter.gps_work.location_timeout * 60000, gps_location_timeout_ind, (void*)1);
        }
        else
        {
            track_cust_gps_off_req((void*)3);
        }
#else
        track_cust_gps_off_req((void*)3);
#endif /* __GT500__ */
    }
    else
    {
        if (track_is_timer_run(TRACK_CUST_ALARM_GPS_WORK_TIMER))
        {
            LOGD(L_GPS, L_V6, "1");
        }
        else if (track_is_timer_run(TRACK_CUST_GPS_LOCATION_TIMEOUT_TIMER_ID))
        {
            LOGD(L_GPS, L_V6, "2");
        }
        else if (track_is_timer_run(TRACK_CUST_POWERON_CLOSE_GPS_TIMER))
        {
            LOGD(L_GPS, L_V6, "3");
        }
        else if (track_is_timer_run(TRACK_CUST_DW_GPS_WORK_TIMER))
        {
            LOGD(L_GPS, L_V6, "4");
        }
        else if (track_is_timer_run(TRACK_CUST_WHERE_GPS_WORK_TIMER_ID))
        {
            LOGD(L_GPS, L_V6, "5");
        }
        else if (track_is_timer_run(TRACK_CUST_KEY_UPDATA_OVER_TIMER_ID))
        {
            LOGD(L_GPS, L_V6, "6");
        }
        else if (track_is_timer_run(TRACK_CUST_GPS_LOCATION_WORKTIME_TIMER_ID))
        {
            LOGD(L_GPS, L_V6, "7");
        }
        else if (track_is_timer_run(TRACK_CUST_WORK_MODE_GPSUPDATE_OVERTIME_TIMER_ID))
        {
            LOGD(L_GPS, L_V6, "8");
        }
        else if (track_is_timer_run(TRACK_CUST_GPS_UPDATE_EPHEMERIS_OVERTIME_TIMER_ID))
        {
            LOGD(L_GPS, L_V6, "9");
        }
        else if (track_is_timer_run(TRACK_CUST_WORK_MODE_GPSON_UPDATE_TIMER_ID))
        {
            LOGD(L_GPS, L_V6, "10");
        }
        else
        {
            LOGD(L_GPS, L_V6, "11");
        }
    }
#else
    if (track_cust_status_acc() == 0 && track_drv_get_gps_RMC()->Speed < 20 && (G_parameter.sensor_gps_work != 0 || G_realtime_data.power_saving_mode == 1))
    {
        track_cust_gps_off_req((void*)3);
    }
    else
    {
        track_start_timer(TRACK_CUST_GPS_LOCATION_TIMEOUT_TIMER_ID, G_parameter.gps_work.location_timeout * 60000, gps_location_timeout_ind, (void*)1);
    }
#endif /* __GT03F__ */
}

/*****************************************************************************
*  Global Functions                     全局函数
******************************************************************************/




/******************************************************************************
 *  Function    -  gps_on
 *
 *  Purpose     -  开启GPS函数
 *
 *  Description -  return Null
 *
 * modification history  //开启GPS必须由track_cust_gps_work_req 这个函数开启，不能直接调用
 此函数，否则会出现没有搜星超时现象
 * ----------------------------------------
 * v1.0  , 27-07-2012,  Liujw  written
 * v1.1  , 08-10-2013,  WangQi && JiangWeiJun
 * ----------------------------------------
 ******************************************************************************/
void track_cust_gps_control(kal_uint8 on)
{
    LOGD(L_GPS, L_V5, "control:%d,%d,%d", on, track_cust_status_charger(), gps_status);
#if defined(__GT741__)
    LOGD(L_APP, L_V1, "GT741不操作GPS");
    return;
#endif /* __TEST_DEVICE__ */
#if defined(__TEST_DEVICE__)
    if (track_rid_given()) return;
#endif /* __TEST_DEVICE__ */

#if defined (__CUSTOM_DDWL__)
    if (on==0)
    {
        track_cust_dormancy_change_flight_mode(1);
    }
    else
    {
        track_cust_dormancy_change_flight_mode(0);
    }
#endif /* __CUSTOM_DDWL__ */

    if (on == 1)
    {
        #if defined(__GT420D__)
        if (track_cust_check_gpsflag_status(99) == 2)
        {
            return;
        }
        if(track_cust_gt420d_carstatus_data(99) == 2)
    {
        if(track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 && track_cust_refer_motion_status_data(99) == 0 \
                    && track_cust_check_simalm_flag(99) == 0 && track_cust_check_firstpoweron_flag(99) == 1 && track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0 && track_cust_need_check_sportorstatic(99) == 0)
        {
            LOGD(5,5,"不需要开GPS");
            return;
            }
        }
        #endif
#if defined(__FLY_MODE__) && !defined(__MT200__)
        if (G_parameter.flycut != 0 && track_cust_isbatlow() && !(track_is_testmode() || track_is_autotestmode()))
        {
            LOGD(L_GPS, L_V4, "检测到震动，但电池低电忽略");
            return;
        }
#endif /* __FLY_MODE__ */

        if (gps_status == 0)
        {
            gps_status = GPS_STAUS_ON;
            track_cust_gps_switch(1);
#if defined(__GT740__)
            track_stop_timer(TRACK_CUST_GPSUP_SEND_TIMER_ID);
#endif


#if defined(__GT310__) || defined(__GT03F__) || defined(__GW100__)||defined(__GT300S__)
#elif defined(__GT500__)
            if (track_cust_status_charger() < 1)
            {
                track_cust_led_sleep(1);
            }
#elif defined(__ET200__)|| defined(__V20__) || defined(__ET310__) || defined(__MT200__)||defined (__ET320__)
            if (G_parameter.led_sleep != 3)
            {
                track_cust_led_sleep(1);
            }
#else
            if (G_parameter.led_sleep != 3 && G_realtime_data.key_status == 1)
            {
                track_cust_led_sleep(1);
            }
#endif /* __GT300__ */
            g_restart_gps_count = 0;
#if defined(__GT530__)
            tr_start_timer(TRACK_CUST_PERIODIC_RESTART_GPS_TIMER_ID, 50000, track_cust_gps_nodata_restart);
#else
            tr_start_timer(TRACK_CUST_PERIODIC_RESTART_GPS_TIMER_ID, 20*1000, track_cust_gps_nodata_restart);
#endif
            if (G_realtime_data.netLogControl & 8)
            {
                cust_packet_log(8, 1, 0, 0, 0, 0, 0);
            }
            tr_stop_timer(TRACK_CUST_GPS_EPHEMERIS_UPDATE_TIMER_ID);
        }
    }
    else if (on == 0)
    {
    #if defined(__GT420D__)
    if (G_parameter.extchip.mode == 2 && track_cust_check_gpsflag_status(99) == 1 && track_cust_gt420d_carstatus_data(99) == 1)
    {
        LOGD(L_APP,L_V5,"模式二不关闭GPS");
        return;
    }
    if(track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 && track_cust_refer_motion_status_data(99) == 0 \
                    && track_cust_check_simalm_flag(99) == 0 && track_cust_check_firstpoweron_flag(99) == 0 && track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0 && track_cust_need_check_sportorstatic(99) == 0)
        {
            LOGD(5,5,"可以关闭GPS");
            }
    else
    {
        LOGD(5,5,"不可以关闭GPS");
        return;
        }
    #endif
        if (gps_status > 0)
        {
#if !defined(__BCA__)
            track_cust_instant_contract_position_data(2);
#endif/*__BCA__*/
            track_cust_gps_check_nofix(0);
            track_cust_gps_switch(0);
#if !defined(__GT310__)
            track_stop_timer(TRACK_CUST_GPS_LOCATION_WORKTIME_TIMER_ID);
#endif
            track_stop_timer(TRACK_CUST_GPS_LOCATION_TIMEOUT_TIMER_ID);
            track_stop_timer(TRACK_CUST_GPS_UPDATE_EPHEMERIS_OVERTIME_TIMER_ID);
            track_stop_timer(TRACK_CUST_PERIODIC_RESTART_GPS_TIMER_ID);
#if defined(__GT310__) ||defined(__GT300S__)
            {
                track_cust_led_sleep(2);
            }
#elif defined(__GT03F__) || defined(__GW100__)
            //nc
#elif defined(__GT500__)
            {
                track_cust_led_sleep(2);
            }
#elif defined(__ET200__)|| defined(__V20__) || defined(__ET310__) || defined(__MT200__)||defined (__ET320__)
            if (G_parameter.led_sleep != 2)
            {
                track_cust_led_sleep(2);
            }
#elif defined (__NT33__)
            if (G_realtime_data.key_status == 1)
            {
                if (G_parameter.led_sleep != 2)
                {
                    track_cust_led_sleep(2);
                }
            }
#else
            if (G_parameter.led_sleep != 2)
            {
                track_cust_led_sleep(2);
            }
#endif /* __GT300__ */

            gps_status = GPS_STAUS_OFF;

            net_upload_position = 0;

            g_restart_gps_count = 0;
            /*有AGPS功能去除EPH功能*/
#if 0
            if (G_parameter.gps_work.ephemeris_interval > 0)
            {
                track_start_timer(TRACK_CUST_GPS_EPHEMERIS_UPDATE_TIMER_ID, G_parameter.gps_work.ephemeris_interval * 60000, track_cust_gps_work_req, (void *)1);
            }
#endif
            if (G_realtime_data.netLogControl & 8)
            {
                cust_packet_log(8, 0, 0, 0, 0, 0, 0);
            }
            track_cust_module_alarm_blind();

#if defined(__GPS_INTERMITTENT_START__)
            /* GT300特有，此处检查是否进入休眠 */
#if defined(__GT370__)|| defined (__GT380__)
            if ((track_cust_get_work_mode() == WORK_MODE_2 && KAL_FALSE == track_cust_sensor_delay_valid()))
            {
                LOGD(L_GPS, L_V5, "模式2静止状态");
                if (KAL_FALSE == track_is_timer_run(TRACK_CUST_LBS_UPDATE_L_TIMER_ID))
                {
                    LOGD(L_APP, L_V5, "静止模式2下次传WIFI %d", track_cust_get_pack_time());
                    tr_start_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID, track_cust_get_pack_time() * 1000, track_cust_update_LBS);
                }
            }
            else
            {
                track_cust_stop_update_LBS();
            }
            track_cust_gprson_status(0);

#else
            track_cust_stop_update_LBS();
            track_cust_gprson_status(0);
#endif
            if (track_is_timer_run(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID) == KAL_FALSE && track_cust_get_work_mode() == WORK_MODE_1)
            {
#if defined(__GT300S__)
                track_mode1_gt300s_reset_work();
#elif defined(__GT370__)|| defined (__GT380__)
                track_cust_mode_change(2);
#else
                tr_stop_timer(TRACK_CUST_GPS_EPHEMERIS_UPDATE_TIMER_ID);
#if !defined(__GT420D__)
                track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID, G_parameter.gps_work.Fti.mode1_gps_time  * 60 * 1000, track_cust_gps_work_req, (void *)13);
#endif
#endif /*  __GT300S__ */
            }
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
#else
            if (track_cust_gprson_status(99) == 0 && track_cust_get_work_mode() == WORK_MODE_3)
            {
                track_os_intoTaskMsgQueue2(track_cust_sleep_mode);
            }
#endif /* __GT300S__ */
#elif defined(__GT500__)
            track_cust_stop_update_LBS();
#elif defined(__GW100__)
            if (track_is_timer_run(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID) == KAL_FALSE && G_parameter.gps_work.Fti.mode1_gps_time != 0)
            {
                track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID, G_parameter.gps_work.Fti.mode1_gps_time * 60 * 1000, track_cust_gps_work_req, (void *)13);
            }
#endif /* __GT300__ */
        }
    }
}
/******************************************************************************
 *  Function    -  track_cust_gps_location_timeout
 *
 *  Purpose     -  调用GPS超时关闭函数
 *
 *  Description -  time 已秒为单位
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 03-09-2013,  ZengPing && Liujw  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_gps_location_timeout(kal_uint32 time)
{
    LOGD(L_GPS, L_V6, " time =%d", time);
    track_start_timer(TRACK_CUST_WHERE_GPS_WORK_TIMER_ID, time * 1000, gps_location_timeout_ind, (void*)10);
}


/*记录关闭GPS时是否定位       --    chengjun  2017-04-20*/
#if defined(__GT370__)|| defined (__GT380__)
static kal_uint8 last_gps_status=0;
kal_uint8 track_cust_is_fix_when_gps_off(void)
{
    if (last_gps_status<3)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
#endif

/******************************************************************************
 *  Function    -  track_cust_gps_off_req
 *
 *  Purpose     -  请求关闭GPS
 *
 *  Description -  arg
 *                  1   星历更新超时
 *                  2   AGPS的EPO下载完成，重启GPS载入EPO
 *                  3   搜星定位超时
 *                  4   深度休眠
 *                  5   应用模式切换，需关闭GPS
 *                  6   本次开机不开GPS 710
 *  modification history
 * ----------------------------------------
 * v1.0  , 21-11-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_gps_off_req(void *arg)
{
    kal_uint32 value = (kal_uint32)arg;
    kal_uint8 work_mode = track_cust_get_work_mode();
    LOGD(L_GPS, L_V5, "%d,%d,mode:%d", value, G_parameter.sensor_gps_work, work_mode);
#if defined(__GT420D__)
    if (G_parameter.extchip.mode == 2 && track_cust_check_gpsflag_status(99) == 1 && track_cust_gt420d_carstatus_data(99) == 1)
    {
        LOGD(L_APP,L_V5,"模式二运动中不关闭GPS");
        return;
    }
    if(track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 && track_cust_refer_motion_status_data(99) == 0 \
                    && track_cust_check_simalm_flag(99) == 0 && track_cust_check_firstpoweron_flag(99) == 0 && track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0 && track_cust_need_check_sportorstatic(99) == 0)
        {
            LOGD(5,5,"可以关闭GPS");
            }
    else
    {
        LOGD(5,5,"不可以关闭GPS");
        return;
        }
#endif
    if (track_is_testmode() || track_is_autotestmode())
    {
        return;
    }

#if defined(__ATGM_UPDATE__)
    if (track_cust_atgm_update_status(STATUS_CHECK) >= ATGM_UPDATE_STATUS_update_begin_req)
    {
        LOGD(L_APP, L_V1, "ATGM升级中,不关GPS");
        return;
    }
#endif /*__ATGM_UPDATE__*/

#if defined(__GT300__) || defined(__GT03F__) || defined(__GW100__)||defined(__GT300S__)
    /* 同时满足下列条件，GPS不能关闭：
    1. SENDS 设置为 0；
    2. 关闭模式不是AGPS请求；
    3. 处于模式二下（仅限于GT300/GT500等包含模式切换的项目）。 */
    LOGD(L_GPS, L_V5, "[1] %d,%d", value, G_parameter.sensor_gps_work);
    if (value != 2 && G_parameter.sensor_gps_work == 0 && (work_mode == WORK_MODE_2 || work_mode == WORK_MODE_4))
    {
        return;
    }
#elif defined(__GT370__)|| defined (__GT380__)
    LOGD(L_GPS, L_V5, "[3] %d,%d", value, G_parameter.sensor_gps_work);
    if (G_parameter.sensor_gps_work == 0 && work_mode == WORK_MODE_2)
    {
        return;
    }
#else
    LOGD(L_GPS, L_V5, "[2] %d,%d", value, G_parameter.sensor_gps_work);
    if (value != 2 && G_parameter.sensor_gps_work == 0) return;
#endif /* __GT100__ */
#if defined(__GT530__)
    track_cust_get_lbs_data((void*)3);
#endif
    track_cust_gps_control(0);
#if defined(__GT740__)
    track_cust_set_gps_status(GPS_NOFIX);
    if (6 == value || G_parameter.extchip.mode!=1)
    {
        GPS_NOT_OPEN = 1;
    }
#endif
}

/******************************************************************************
 *  Function    -  track_cust_gps_work_req
 *
 *  Purpose     -  应用请求GPS工作
 *
 *  Description -  arg
 *                  1    星历更新
 *                  2    通电开机首次启动
 *                  3    震动唤醒
 *                  4    ACC变化对GPS的影响（更新GPS延时关闭的定时器）
 *                  5    DW等指令查询位置请求
 *                  6    位移报警请求
 *                  7    AT指令设置GPS常开
 *                  8    强制GPS工作20分钟
 *                  9    AGPS重启
 *                  10   报警强制开启GPS 工作5 Min
 *                  11   WHER与url指令gps延时5分钟
 *                  12   按键上报位置gps延时5分钟
 *                  13   定时开启GPS并上传数据
 *                  14   GPSON开启时间
 *                  15   03D模式3GPS开启  gt310
 *                  16   开启2分钟GPS
 *                  17   ljdw开启gsp
 *                  18   触发防拆和mode指令
 *                  19  GPS模块开启
 *                  20   静止时GPS未定位，开GPS 2分钟
 *  modification history
 * ----------------------------------------
 * v1.0  , 01-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_gps_work_req(void *arg)
{
    kal_uint32 value = (kal_uint32)arg;
    char buf[100] = {0};
    applib_time_struct currTime_ams = {0};
    kal_uint32 timer = 0;
    kal_int32 tick = 0;
    static U8 allow_open = 0;
    LOGD(L_GPS, L_V5, "value:%d", value);
#if defined(__GT420D__)
    if (track_cust_check_gpsflag_status(99) == 2)
    {
        return;
    }
    if(track_cust_gt420d_carstatus_data(99) == 2)
    {
        if(G_parameter.extchip.mode == 1)
        {
            if(track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 && track_cust_refer_motion_status_data(99) == 0 \
                    && track_cust_check_simalm_flag(99) == 0  && track_cust_check_firstpoweron_flag(99) == 1 && track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0 && track_cust_need_check_sportorstatic(99) == 0)
        {
            LOGD(5,5,"不需要开GPS");
            return;
            }
        }
        else if(G_parameter.extchip.mode == 2)
        {
            if(track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 && track_cust_refer_motion_status_data(99) == 0 \
                    && track_cust_check_simalm_flag(99) == 0 && track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0 && track_cust_need_check_sportorstatic(99) == 0)
            {
                LOGD(5,5,"不需要开GPS");
                return;
                }
            }
        }
#endif
#if defined(__XCWS__)
    if ((value < 5 || value == 9) && track_cust_get_work_mode_cq() == SLEEP_MODE)
    {
        return;
    }
    else if (track_cust_get_work_mode_cq() == SLEEP_MODE)
    {
        track_cust_work_mode_ind(w_contorl_gps_on_msg);
    }
#endif
#if defined(__GT741__)
    LOGD(L_GPS, L_V5, "741 无GPS");

    return;
#elif defined(__GT740__)
    if (track_is_testmode() ||track_cust_gps_need_open()|| value==18 || value==19)
    {
        //////////
    }
    else
    {
        if (1==GPS_NOT_OPEN || (G_parameter.gpswake==0) || G_realtime_data.nw_reset_sign>0)
        {
            LOGD(L_GPS, L_V6, "本次开机不开启GPS定位!!! gps_not_open %d,gpswake %d ",GPS_NOT_OPEN,G_parameter.gpswake);
            return;
        }
    }
#endif
#if defined(__ATGM_UPDATE__)
    if (track_cust_atgm_update_status(STATUS_CHECK) >= ATGM_UPDATE_STATUS_update_begin_req)
    {
        LOGD(L_APP, L_V1, "升级中,不gps_on");
        return;
    }
#endif /*__ATGM_UPDATE__*/

    if (value == 1)
    {
        LOGD(L_GPS, L_V4, "星历更新，唤醒GPS，不上传定位点");
    }
    else if (value == 13)
    {
        LOGD(L_GPS, L_V4, "定时唤醒GPS，需上传定位点");
#if defined(__GT420D__)
        net_upload_position = 1;
#endif
#if defined(__GT300S__)
        //     tr_start_timer(TRACK_CUST_SEND_LBS_TIMER_ID, 2 * 60 * 1000, track_cust_send_lbs);
#endif /* __GT300S__ */
    }
    else
    {
        net_upload_position = 1;
    }
    if (value == 1)
    {
        track_start_timer(TRACK_CUST_GPS_UPDATE_EPHEMERIS_OVERTIME_TIMER_ID, WHERE_GPS_WORK_TIME, gps_location_timeout_ind, (void*)11);
    }
    else if (value == 2)
    {
        track_start_timer(TRACK_CUST_POWERON_CLOSE_GPS_TIMER, WHERE_GPS_WORK_TIME, gps_location_timeout_ind, (void*)12);
#if defined(__BCA__)
        track_start_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID, 2 * 1000 * 60, track_cust_shocks_lbs_update, (void*)98);
#endif
    }
    else if (value == 3)
    {
        //G_parameter.sensor_gps_work > 0???
        track_start_timer(TRACK_CUST_GPS_LOCATION_WORKTIME_TIMER_ID, G_parameter.sensor_gps_work * 60000, gps_location_timeout_ind, (void *)7);
#if defined(__GT310__)
        track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_OVERTIME_TIMER_ID, LOCATION_MODE_TIMEOUT_SEC * 1000, track_cust_gps_send_data, (void *)0);
#endif
    }
    else if (value == 5)
    {
#if defined (__ET200__)||defined(__ET130__)|| defined(__V20__) || defined(__ET310__) || defined(__MT200__)||defined (__ET320__)
        track_start_timer(TRACK_CUST_DW_GPS_WORK_TIMER, 5 * 60 * 1000 + 10 * 1000, gps_location_timeout_ind, (void*)15);
#elif defined(__GT370__)|| defined (__GT380__) || defined(__GT740__) || defined(__GT310__)||defined(__GT420D__)
        track_start_timer(TRACK_CUST_DW_GPS_WORK_TIMER, 2 * 60 * 1000 + 10 * 1000, gps_location_timeout_ind, (void*)15);
#else
        track_start_timer(TRACK_CUST_DW_GPS_WORK_TIMER, 6 * 60 * 1000 + 10 * 1000, gps_location_timeout_ind, (void*)15);
#endif /* __ET200__ */
    }
    else if (value == 10)
    {
        track_start_timer(TRACK_CUST_ALARM_GPS_WORK_TIMER, 5 * 60 * 1000, gps_location_timeout_ind, (void*)20);
    }
    else if (value == 11)
    {
        track_start_timer(TRACK_CUST_WHERE_GPS_WORK_TIMER_ID, WHERE_GPS_WORK_TIME, gps_location_timeout_ind, (void*)21);
    }
#if defined( __GT310__)
    else if (value == 12)
    {
        track_start_timer(TRACK_CUST_KEY_UPDATA_OVER_TIMER_ID, 1*60 * 1000, track_cust_key_double_sos_updata_position, (void*)0);
    }
#elif defined(__GT300S__)
    else if (value == 12)
    {
        track_start_timer(TRACK_CUST_KEY_UPDATA_OVER_TIMER_ID, 120 * 1000, track_cust_key_updata_position, (void*)0);
    }
#endif /* __GT300__ */
    else if (value == 13)
    {
#if defined (__GT300S__) || defined(__GT370__)|| defined (__GT380__)
        if (WORK_MODE_3 != track_cust_get_work_mode())
            track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_OVERTIME_TIMER_ID, 2 * 60 * 1000, track_cust_gps_send_data, (void *)0);
#elif defined(__GT310__)
        track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_OVERTIME_TIMER_ID, 2 * 60 * 1000, track_cust_gps_send_data, (void *)0);
#elif defined(__GT420D__)
        LOGD(L_APP,L_V5,"G_parameter.extchip.mode:%d",G_parameter.extchip.mode);
        if (G_parameter.extchip.mode == 1 || G_parameter.extchip.mode == 2)
        {
            #if defined(__GT420D__)
            track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                snprintf(buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d GPS ON\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec);
                LOGD(5,5,"len:%d",strlen(buf));
                track_cust_save_logfile(buf, strlen(buf));
            #endif
            LOGD(L_GPS, L_V5, "唤醒GPS，搜星");
            track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_OVERTIME_TIMER_ID,  90 * 1000, track_cust_gps_send_data, (void *)0);
            tr_start_timer(TRACK_CUST_GPS_VIEW_TIMER_ID, 20*1000,track_cust_gps_search_timeout);
        }
#else
        track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_OVERTIME_TIMER_ID, 3 * 60 * 1000, track_cust_gps_send_data, (void *)0);
#endif /* __GT300S__ */
    }
#if defined(__XCWS__)
    else if (value == 9)
    {
        track_start_timer(TRACK_CUST_GPS_LOCATION_TIMEOUT_TIMER_ID, G_parameter.gps_work.location_timeout * 60000, gps_location_timeout_ind, (void*)19);
    }
#endif
    else if (value == 14)
    {
        timer = track_cust_gpson_timer(999);
        track_start_timer(TRACK_CUST_WORK_MODE_GPSON_UPDATE_TIMER_ID, timer * 60 * 1000, gps_location_timeout_ind, (void *)0);
    }
    else if (value == 15)
    {
        track_start_timer(TRACK_CUST_WORK_MODE3__GPSON_TIMER_ID, 2 * 1000 * 60, gps_location_timeout_ind, (void *)25);
    }
    else if (value == 16)
    {
        track_start_timer(TRACK_CUST_FT_WORK_TIMER_ID, track_get_ft_work_timer() * 1000, gps_location_timeout_ind, (void *)0);
    }
    else if (value == 17)
    {
        track_start_timer(TRACK_CUST_GPS_WORK_DJDW_TIMER_ID, 2 * 60 * 1000, gps_location_timeout_ind, (void *)27);
    }
    else if (value == 18)
    {
        track_start_timer(TRACK_CUST_POWERON_CLOSE_GPS_TIMER, EXTCHANGE_MODE_GPS_WORK_TIME, gps_location_timeout_ind, (void *)29);
    }
    else if (value==19)
    {
        if (!track_is_timer_run(TRACK_CUST_POWERON_CLOSE_GPS_TIMER))
        {
            track_start_timer(TRACK_CUST_POWERON_CLOSE_GPS_TIMER, EXTCHANGE_MODE_GPS_WORK_TIME, gps_location_timeout_ind, (void *)29);
        }
    }
#if defined (__GT370__) || defined (__GT380__)
    else if (value == 20)
    {
        track_start_timer(TRACK_CUST_STILL_NO_POSITION_GPS_ON_TIMEOUT_TIMER_ID, 2 * 60 * 1000, gps_location_timeout_ind, (void *)0);
    }
#endif
    else
    {
        track_start_timer(TRACK_CUST_GPS_LOCATION_TIMEOUT_TIMER_ID, G_parameter.gps_work.location_timeout * 60000, gps_location_timeout_ind, (void*)3);
    }


    if (gps_status == 0)
    {
        LOGD(L_GPS, L_V4, "%d唤醒GPS", value);
        track_cust_gps_control(1);
#if defined(__GT310__) ||defined(__GT03F__)||defined(__GT300S__)
        if (((track_cust_get_work_mode() == WORK_MODE_2) || (track_cust_get_work_mode() == WORK_MODE_4)) && net_upload_position == 1)
        {
            tr_start_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID, 5 * 1000 * 60, track_cust_update_LBS);
        }
#elif defined(__GT370__)|| defined (__GT380__)
        if (track_cust_get_work_mode() == WORK_MODE_2)
        {
            tr_start_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID, 2 * 1000 * 60, track_cust_update_LBS);
        }
#elif defined(__GT500__)
        if (net_upload_position == 1)
        {
            tr_start_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID, 2 * 1000 * 60, track_cust_update_LBS);
        }
#elif defined(__GW100__)|| defined(__GT740__)||defined(__GT420D__)
        if (net_upload_position == 1)
        {
#if !defined(__GT420D__)
            tr_start_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID,  G_parameter.gps_work.Fti.lbs_interval * 1000, track_cust_update_LBS);
#endif
        }
#endif
#if defined(__GT530__)
        track_cust_lte_wakeup();
        track_cust_get_lbs_data(0);
#endif
    }
#if !defined (__GW100__)
    else if (gps_status == 1)
    {
        if (value == 3)
        {
            LOGD(L_SENSOR, L_V8, "延长GPS搜星超时时间");
        }
        else
        {
            LOGD(L_GPS, L_V4, "%d请求延长GPS搜星超时时间", value);
        }

#if defined(__GT370__)|| defined (__GT380__)
        if (value == 13)return;
#elif defined(__GT310__)
//NOTHING
#else
        track_stop_timer(TRACK_CUST_GPS_LOCATION_WORKTIME_TIMER_ID);
        track_start_timer(TRACK_CUST_GPS_LOCATION_TIMEOUT_TIMER_ID, G_parameter.gps_work.location_timeout * 60000, gps_location_timeout_ind, (void*)3);
#endif
    }
    else if (gps_status >= 2)
    {
        tr_stop_timer(TRACK_CUST_GPS_LOCATION_TIMEOUT_TIMER_ID);
#if defined(__GT370__)|| defined (__GT380__)
        if (track_is_timer_run(TRACK_CUST_GPS_LOCATION_WORKTIME_TIMER_ID))
        {
            track_start_timer(TRACK_CUST_GPS_LOCATION_WORKTIME_TIMER_ID, G_parameter.sensor_gps_work * 60000, gps_location_timeout_ind, (void*)4);
        }
#else
#if !defined(__GT420D__)
        track_start_timer(TRACK_CUST_GPS_LOCATION_WORKTIME_TIMER_ID, G_parameter.sensor_gps_work * 60000, gps_location_timeout_ind, (void*)4);
#endif
#endif
    }
#endif /* __GW100__ */
}

/******************************************************************************
 *  Function    -  track_cust_module_gps_vibrates_wake
 *
 *  Purpose     -  振动唤醒GPS
 *
 *  Description -
 *      例如: 十秒检测到三次振动，则唤醒GPS
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_gps_vibrates_wake()
{
    static kal_uint32 tick_loop[TRACK_MAX_LOOP] = {0};
    static kal_uint8 start = 0, end = 0;
    U32 tick = OTA_kal_get_systicks();
    U32 detection_time = G_parameter.gps_work.vibrates.detection_time * KAL_TICKS_1_SEC;
    int i;
    kal_bool flag = KAL_TRUE;

#if defined(__GT370__)|| defined (__GT380__)
    if (track_cust_get_work_mode()==2)  //GPS常开模式
    {
        if (track_cust_gps_status() >= GPS_STAUS_SCAN)
        {
            //flag = KAL_FALSE;
            track_cust_gps_work_req((void *)3);
        }
    }
#elif !(defined(__GT100__) ||defined(__GT06A__) || defined(__ET200__) || defined(__ET130__) || defined(__ET320__))
    /*  GPS工作期间震若检测到一次震动，GPS工作时间顺延 3分钟*/
    if (track_cust_gps_status() >= GPS_STAUS_SCAN)
    {
        LOGD(L_APP,L_V5,"222");
        flag = KAL_FALSE;
        track_cust_gps_work_req((void *)3);
    }
#endif /* __GT100__ */
#if defined(__FLY_MODE__)
    track_cust_flymode_set(31);
#endif/*__FLY_MODE__*/
    /* 判断最新的一次记录是否满足最短间隔 */
    if (start != end && G_parameter.gps_work.vibrates.sampling_interval != 0)
    {
        i = end - 1;
        if (i < 0)
        {
            i = TRACK_MAX_LOOP - 1;
        }
        if (tick < tick_loop[i] + G_parameter.gps_work.vibrates.sampling_interval * KAL_TICKS_1_SEC - KAL_TICKS_100_MSEC * 2)
        {
            LOGD(L_SENSOR, L_V7, "不满足震动检测的最短间隔");
            return;
        }
    }

    /* 放入最新的一次记录 */
    tick_loop[end] = tick;
    end++;
    if (end >= TRACK_MAX_LOOP)
    {
        end = 0;
        if (start == 0) start++;
    }

    /* 移除已经过时的记录 */
    for (i = 0; i < TRACK_MAX_LOOP; i++)
    {
        if (start == end || tick_loop[start] == 0 || tick <= detection_time || tick_loop[start] >= tick - detection_time)
        {
            LOGD(L_SENSOR, L_V9, "%d, %d, %d[%d]", tick, detection_time, start, tick_loop[start]);
            break;
        }
        else
        {
            LOGD(L_SENSOR, L_V9, "del: %d, %d, %d[%d], end:%d", tick, detection_time, start, tick_loop[start], end);
        }
        start++;
        if (start >= TRACK_MAX_LOOP)
        {
            start = 0;
        }
    }

    /* 统计满足条件记录条数 */
    if (start == end)
    {
        i = 1;
    }
    else if (start < end)
    {
        i = end - start;
    }
    else
    {
        i = end + TRACK_MAX_LOOP - start;
    }
    LOGD(L_SENSOR, L_V7, "%d/%d, detection_time:%d,%d, %d / %d", start, end, G_parameter.gps_work.vibrates.detection_time, G_parameter.gps_work.vibrates.sampling_interval, i, G_parameter.gps_work.vibrates.detection_count);
    if (i >= G_parameter.gps_work.vibrates.detection_count)
    {
        if (flag)
        {
#if defined(__XCWS__)
            if (track_cust_get_work_mode_cq() == NORNAL_MODE)
#endif
            {
#if defined(__FLY_MODE__) && defined(__MT200__)
                //  MT200飞行模式中，震动三次不唤醒GPS
                if (!track_cust_flymode_set(189))
                {
                    track_cust_gps_work_req((void *)3);
                }
                else if (i == G_parameter.vibrates_alarm.detection_count && G_parameter.vibrates_alarm.sw && G_parameter.vibrates_alarm.alarm_delay_time > 0 && !track_is_timer_run(TRACK_CUST_MODULE_ALARM_VIBRATES_DELAY_TIMER_ID))//飞行模式中
                {
                    LOGD(L_APP, L_V5, "当前正在飞行中，震动报警唤醒GPS");
                    track_cust_gps_work_req((void *)3);
                }
                else//飞行模式中
                {
                    LOGD(L_APP, L_V5, "当前正在飞行中");
                }
#elif defined(__GT370__)|| defined (__GT380__)
                track_sensor_set_moving_mode();
                if (track_cust_get_work_mode() != WORK_MODE_1)
                {
                    track_cust_gps_work_req((void *)3);
                }
#elif defined(__GT310__)
                if (track_cust_get_work_mode()==WORK_MODE_2)
                {
                    extern void track_cust_fly_mode_change(void* par);
                    track_cust_open_location_mode((void *)3);
                    if (track_cust_work_mode_get_fly_mode())
                    {
                        track_cust_fly_mode_change((void*)71);
                    }
                    track_start_timer(TRACK_CUST_INTO_DEEP_SLEEP_MODE_TIMER_ID, G_parameter.fly_stu.mode2_sensorOutTime*60*1000, track_cust_fly_mode_change, (void*)21);
                }
                if (track_cust_get_work_mode()!=WORK_MODE_3)
                {
#if defined(__DROP_ALM__)
                    track_cust_fall_down(2);
#endif
                }

#else

                //震动唤醒GPS干活
                track_cust_gps_work_req((void *)3);
#endif
                //track_cust_work_mode_ind(w_sensor_smg);
            }
        }
#if !defined(__NT37__)
        if (G_parameter.motor_lock.mode == 0 && track_cust_status_defences() == 2)
        {
            track_motor_lock_change_status(MOTOR_REQ);
            /*track_start_timer(
                TRACK_CUST_MODULE_ALARM_VIBRATES_SHORTEST_DELAY_TIMER_ID,
                1000,
                track_cust_module_alarm_vibrates, (void*)2);*/
        }
#endif
#if defined(__FLY_MODE__) && !defined(__MT200__)
        track_cust_flymode_set(32);
#endif/*__FLY_MODE__*/

#if defined (__VIRTUAL_ACC__)
        track_cust_virtual_acc_set(1);
#endif

    }
}

/******************************************************************************
 *  Function    -  track_cust_stop_update_LBS
 *
 *  Purpose     -  关闭lbs信息发送
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 08-08-2013,  jwj  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_stop_update_LBS(void)
{
    LOGD(L_GPS, L_V5, "");

#if !defined(__GW100__)
    if (track_is_timer_run(TRACK_CUST_LBS_UPDATE_L_TIMER_ID))
    {
        tr_stop_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID);
    }
#endif /* __GW100__ */
}



void track_cust_mode3_update_LBS(void * f)
{
    static kal_uint8 up_counts = 0;
    kal_uint32 flag = (kal_uint32)f;

    LOGD(L_GPS, L_V5, "flag:%d,up_counts:%d -----------", flag, up_counts);

    if (WORK_MODE_3 == track_cust_get_work_mode() && G_parameter.mode3_work_interval)
    {
        track_stop_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID);

        if (0 == flag)
        {
            up_counts = 1;
            track_cust_mode3_uping(0);
        }
        else if (99 == flag)
        {
            up_counts = 1;
            return;
        }
        else if (up_counts++ > G_parameter.mode3_up_counts)
        {
            up_counts = 0;
            track_cust_mode3_uping(0);
            track_os_intoTaskMsgQueue2(track_soc_disconnect);
            track_cust_gprson_status(0);
            LOGD(L_GPS, L_V5, "MODE3上传结束");
            return ;
        }

        track_cust_gprson_status(1);
        if (up_counts == 1)
        {
            track_soc_gprs_reconnect((void*)302);
        }
        if (track_cust_gps_status() > GPS_STAUS_2D)
        {
            track_gps_data_struct *p_gpsdata = track_cust_backup_gps_data(0, NULL);
            LOGD(L_GPS, L_V6, "定时唤醒GPS，上传定位点");
            track_cust_backup_gps_data(3, p_gpsdata);
            track_cust_paket_position(p_gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_MODE_GPSUPDATE, 0);
        }
        else
        {
            track_cust_paket_18(track_drv_get_lbs_data(), 0);
        }
        if (2 != flag)
        {
            track_start_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID, 2000, track_cust_mode3_update_LBS, (void *)1);
        }
    }
}

/************************************************
限制发送LBS包,必须是有效LBS数据和时间
************************************************/
void track_cust_sendlbs_limit(void)
{
    char buf[100] = {0};
    applib_time_struct currTime_ams = {0};
    LBS_Multi_Cell_Data_Struct *lbs_strcut = track_drv_get_lbs_data();
    track_gps_data_struct *gpsLastPoint = NULL;
    track_gps_data_struct *gd = NULL;
    applib_time_struct time = {0};
    applib_time_struct *currTime = NULL;
    currTime = &time;
#if defined(__GT740__)
    if (track_cust_is_innormal_wakeup()==KAL_TRUE)return;
#endif
    if (track_cust_is_upload_UTC_time())
    {
        track_fun_get_time(currTime, KAL_TRUE, NULL);
    }
    else
    {
        track_fun_get_time(currTime, KAL_FALSE, &G_parameter.zone);
    }
    if (currTime->nYear == 2015 && currTime->nMonth == 1 && currTime->nDay == 1)
        {
            LOGD(L_GPS, L_V5, "Warning: current time is not Synchronous! ");
            tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 5000, track_cust_sendlbs_limit);
            return;
        }
    LOGD(L_GPS, L_V5, "time: %d-%d-%d ", currTime->nYear, currTime->nMonth, currTime->nDay);
#if !defined(__GT420D__)
    if (track_cust_syn_st_time(0,99)==0)
    {
        LOGD(L_GPS, L_V5, "未同步过时间!!!");
        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 5000, track_cust_sendlbs_limit);
        return;
    }
#endif
#if defined(__GT420D__)
    if (track_is_timer_run(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID))
    {
        track_stop_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID);
    }
    gpsLastPoint = track_cust_backup_gps_data(2, NULL);
    if ((track_drv_convert_rxlev_to_rssi(lbs_strcut->MainCell_Info.rxlev) == 0 || lbs_strcut->MainCell_Info.cell_id == 0) || !lbs_strcut->status)
    {
        LOGD(L_APP,L_V7,"没GPS,WIFI,LBS，不上报点");
        if(G_parameter.extchip.mode == 1)
        {
        track_cust_check_firstpoweron_flag(0);
        track_cust_refer_keyin_data(0);
        track_cust_needsend_charge_data(0);
        track_cust_refer_motion_status_data(0);
        track_cust_check_simalm_flag(0);
        track_cust_check_downalm_flag(2);
        track_cust_check_teardown_flag(2);
        track_cust_check_batteryalm_flag(0);
        track_cust_module_delay_close(30);
        }
        #if defined(__GT420D__)
                track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                snprintf(buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d cant find LBS,%d,%d,%d,%d,%d,%d,%d,%d\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec,
                            track_cust_gt420d_carstatus_data(99),
                            track_cust_refer_keyin_data(99),
                            track_cust_needsend_charge_data(99),
                            track_cust_refer_motion_status_data(99),
                            track_cust_check_simalm_flag(99),
                            track_cust_check_firstpoweron_flag(99),
                            track_cust_check_downalm_flag(98),
                            track_cust_check_teardown_flag(98),
                            track_cust_check_batteryalm_flag(99));
        track_cust_save_logfile(buf, strlen(buf));
    #endif
        return;     
    }
    else
    {
                #if defined(__GT420D__)
                track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                snprintf(buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d send LBS,%d,%d,%d,%d,%d,%d,%d,%d,%d\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec,
                            track_cust_gt420d_carstatus_data(99),
                            track_cust_refer_keyin_data(99),
                            track_cust_needsend_charge_data(99),
                            track_cust_refer_motion_status_data(99),
                            track_cust_check_simalm_flag(99),
                            track_cust_check_firstpoweron_flag(99),
                            track_cust_check_downalm_flag(98),
                            track_cust_check_teardown_flag(98),
                            track_cust_check_batteryalm_flag(99));
        track_cust_save_logfile(buf, strlen(buf));
    #endif
            if(track_cust_gt420d_carstatus_data(99) == 2)
            {
                LOGD(5,5,"1");
                if(track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 && track_cust_refer_motion_status_data(99) == 0 \
                    && track_cust_check_simalm_flag(99) == 0 && track_cust_check_firstpoweron_flag(99) == 1 && track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0 && track_cust_need_check_sportorstatic(99) == 0)
                    {
                        LOGD(5,5,"2");
                        {
                            gd = track_cust_backup_gps_data(2, NULL);
                            gd->gprmc.status = 0;
                            if(G_parameter.extchip.mode == 1)
                            {
                            track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_MORE_LBS,DSC_CUST_POSITION_TIMING,6,15);
                            if (track_is_timer_run(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID))
                                {
                                    track_stop_timer(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID);
                                    }
                            }
                            else if(G_parameter.extchip.mode == 2)
                            {
                                track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_MORE_LBS,DSC_CUST_POSITION_TIMING,5,15);
                                if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
                                {
                                    track_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
                                    }
                            }
                            track_cust_check_firstpoweron_flag(0);
                            track_cust_check_beforepoweroff_flag(0);
                            return;
                            }
                        }
                else if(track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 
                    && track_cust_refer_motion_status_data(99) == 0 && track_cust_check_simalm_flag(99) == 0 && track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0 && track_cust_need_check_sportorstatic(99) == 0)
                {
                    LOGD(5,5,"3");
                    //if(G_realtime_data.last_location_status ==0)
                        {
                            gd = track_cust_backup_gps_data(2, NULL);
                            gd->gprmc.status = 0;
                            if(G_parameter.extchip.mode == 1)
                            {
                            track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_MORE_LBS,DSC_CUST_POSITION_TIMING,6,9);
                            if (track_is_timer_run(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID))
                                {
                                    track_stop_timer(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID);
                                    }
                            }
                            else if(G_parameter.extchip.mode == 2)
                            {
                                track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_MORE_LBS,DSC_CUST_POSITION_TIMING,5,9);
                                if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
                                {
                                    track_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
                                    }
                            }
                            track_cust_check_firstpoweron_flag(0);
                            track_cust_check_beforepoweroff_flag(0);
                            return;
                            }
                        }
                }
            if(track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 && track_cust_refer_motion_status_data(99) == 0 \
                    && track_cust_check_simalm_flag(99) == 0 && track_cust_check_firstpoweron_flag(99) == 0 &&track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0 && track_cust_need_check_sportorstatic(99) == 0)
            {
                LOGD(L_APP,L_V5,"正常状态上传LBS状态包");
                track_cust_dsc_location_paket(NULL,lbs_strcut,LOCA_MORE_LBS,DSC_CUST_POSITION_TIMING,0,9);
            }
            if(track_cust_check_firstpoweron_flag(99) == 1 && track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 && track_cust_refer_motion_status_data(99) == 0 \
                    && track_cust_check_simalm_flag(99) == 0 && track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0)
                {
                    LOGD(L_APP,L_V5,"上传开机包");
                    track_cust_dsc_location_paket(NULL,lbs_strcut,LOCA_MORE_LBS,DSC_CUST_POSITION_TIMING,1,15);
                    track_cust_check_firstpoweron_flag(0);
                }
            else
                {
                    track_cust_check_firstpoweron_flag(0);
                    }
            if (track_cust_refer_keyin_data(99) == 1)
            {
                LOGD(L_APP,L_V5,"按键上传上LBS状态包");
                track_cust_dsc_location_paket(NULL,lbs_strcut,LOCA_MORE_LBS,DSC_CUST_POSITION_TIMING,0,3);
                track_cust_refer_keyin_data(0);
            }
            if (track_cust_needsend_charge_data(99) == 1)
            {
                LOGD(L_APP,L_V5,"充电上传上LBS状态包");
                track_cust_dsc_location_paket(NULL,lbs_strcut,LOCA_MORE_LBS,DSC_CUST_POSITION_TIMING,0,4);
                track_cust_needsend_charge_data(0);
            }
            if (track_cust_needsend_charge_data(99) == 2)
            {
                LOGD(L_APP,L_V5,"拔电上传上LBS状态包");
                track_cust_dsc_location_paket(NULL,lbs_strcut,LOCA_MORE_LBS,DSC_CUST_POSITION_TIMING,0,5);
                track_cust_needsend_charge_data(0);
            }
            if (track_cust_refer_motion_status_data(99) == 1)
            {
                LOGD(L_APP,L_V5,"运动状态改变上传LBS状态包");
                if(track_cust_gt420d_carstatus_data(99) == 2)
                {
                    track_cust_dsc_location_paket(NULL,lbs_strcut,LOCA_MORE_LBS,DSC_CUST_POSITION_TIMING,0,1);
                }
                else
                {
                    track_cust_dsc_location_paket(NULL,lbs_strcut,LOCA_MORE_LBS,DSC_CUST_POSITION_TIMING,0,2);
                    }
                track_cust_refer_motion_status_data(0);
            }
            if(track_cust_check_downalm_flag(98) == 3)
            {
            LOGD(L_APP,L_V5,"无光报警发包");
            track_cust_check_downalm_flag(2);
            tr_stop_timer(TRACK_CUST_EXTCHIP_DARKALM_DELAY_SEND_GPSORLBS_TIMER_ID);
            track_cust_dsc_location_paket(NULL,lbs_strcut,LOCA_MORE_LBS,DSC_CUST_POSITION_TIMING,3,7);            
            }
            if(track_cust_check_teardown_flag(98) == 3)
            {
            LOGD(L_APP,L_V5,"开箱报警发包");
            track_cust_check_teardown_flag(2);
            tr_stop_timer(TRACK_CUST_EXTCHIP_TEARDOWN_DELAY_SEND_GPSORLBS_TIMER_ID);
            track_cust_dsc_location_paket(NULL,lbs_strcut,LOCA_MORE_LBS,DSC_CUST_POSITION_TIMING,3,6);            
            }
            if(track_cust_check_batteryalm_flag(99) == 1)
            {
                LOGD(L_APP,L_V5,"低电报警发包");
                track_cust_check_batteryalm_flag(0);
                track_cust_dsc_location_paket(NULL,lbs_strcut,LOCA_MORE_LBS,DSC_CUST_POSITION_TIMING,4,14); 
            }
            if(track_cust_check_simalm_flag(99) == 1)
            {
                LOGD(L_APP,L_V5,"换卡报警发包");
                track_cust_check_simalm_flag(0);
                track_cust_dsc_location_paket(NULL,lbs_strcut,LOCA_MORE_LBS,DSC_CUST_POSITION_TIMING,2,13); 
                }
            track_cust_check_beforepoweroff_flag(0);
            }
#else
    if (lbs_strcut->MainCell_Info.cell_id == 0)
    {
        LOGD(L_GPS, L_V5, "cell_id is Zero, cannot upload delay 5s! ");
        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 5000, track_cust_sendlbs_limit);
        return;
    }
    if (currTime->nYear == (*OTA_DEFAULT_HARDWARE_YEAR + 2000) && currTime->nMonth == 1 && currTime->nDay == 1)
    {
        LOGD(L_GPS, L_V5, "current time is not Synchronous! ");
        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 5000, track_cust_sendlbs_limit);
        return;
    }
#endif
#if !defined(__GT420D__)
    track_cust_paket_18(track_drv_get_lbs_data(), 0);
#endif
}

/******************************************************************************
*  Function    -  track_cust_LBS_upload
*
*  Purpose     -  GPS搜星一直不定位才补基站包
*
*  Description -
*
* modification history
* ----------------------------------------
* v1.0  , 2017-09-28,  chengjun  written
* ----------------------------------------
******************************************************************************/
void track_cust_LBS_upload(void)
{
    static U32 last_t2 = 0, last_t1 = 0;
    U32 tick = OTA_kal_get_systicks(), t1, t2;
    LOGD(L_GPS, L_V5, "");
    if (G_parameter.lbson.sw == 0 || G_parameter.lbson.t1 == 0)
    {
        return;
    }

#if defined(__GT370__) || defined (__GT380__)
    if (track_cust_get_work_mode() == 1)
    {
        return;
    }
#endif

#if defined(__FLY_MODE__)
    if (track_cust_flymode_set(199))//飞行模式中
    {
        LOGD(L_GPS, L_V4, "当前正在飞行中");
        return;
    }
#endif /* __FLY_MODE__*/

#if defined (__CUSTOM_DDWL__)
    if (track_cust_dormancy_change_flight_mode(0xFF)==1)
    {
        LOGD(L_APP, L_V4, "当前正在飞行中");
        return;
    }
#endif /* __CUSTOM_DDWL__ */


    //在GPS定位或GPS关闭状态下不上传LBS定位包,仅GPS搜星时上报
    if (track_cust_gps_status() >= GPS_STAUS_2D || track_cust_gps_status() == GPS_STAUS_OFF)
    {
        last_t2 = 0;
        track_stop_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID);
        LOGD(L_GPS, L_V5, "1");
        return; /* LBS 在 GPS 不定位状态下才传 */
    }

    if (last_t2 == 0)
    {
        last_t2 = tick;
        last_t1 = 0;
        LOGD(L_GPS, L_V5, "start the timer");
        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, G_parameter.lbson.t1 * 1000, track_cust_LBS_upload);
        return;
    }
    t2 = kal_ticks_to_secs(tick - last_t2);
    if (G_parameter.lbson.t2 > t2)
    {
        LOGD(L_GPS, L_V5, "t2:%d", t2);
        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, (G_parameter.lbson.t2 - t2) * 1000, track_cust_LBS_upload);
        return; /* GPS不定位时间未超过t2 */
    }
    t1 = kal_ticks_to_secs(tick - last_t1);
    if (G_parameter.lbson.t1 > t1)
    {
        LOGD(L_GPS, L_V5, "t1:%d", t1);
        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, (G_parameter.lbson.t1 - t1) * 1000, track_cust_LBS_upload);
        return; /* GPS不定位时间未超过t1 */
    }
    {
        static kal_uint16 last_cell_id;
        LBS_Multi_Cell_Data_Struct *lbs = track_drv_get_lbs_data();
        applib_time_struct time = {0};
        applib_time_struct *currTime = NULL;
        currTime = &time;
        if (track_cust_is_upload_UTC_time())
        {
            track_fun_get_time(currTime, KAL_TRUE, NULL);
        }
        else
        {
            track_fun_get_time(currTime, KAL_FALSE, &G_parameter.zone);
        }
        LOGD(L_GPS, L_V5, "time: %d-%d-%d ", currTime->nYear, currTime->nMonth, currTime->nDay);
        if (lbs->MainCell_Info.cell_id == 0)
        {
            LOGD(L_GPS, L_V5, "Warning: cell_id is Zero, cannot upload delay 5s! ");
            tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 5000, track_cust_LBS_upload);
            return;
        }
        if (currTime->nYear == 2015 && currTime->nMonth == 1 && currTime->nDay == 1)
        {
            LOGD(L_GPS, L_V5, "Warning: current time is not Synchronous! ");
            tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 5000, track_cust_LBS_upload);
            return;
        }
        if (last_cell_id == lbs->MainCell_Info.cell_id || lbs->MainCell_Info.cell_id == 0)
        {
            LOGD(L_GPS, L_V5, "Warning: cell id the same with the previous! ");
            tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 5000, track_cust_LBS_upload);
            return;
        }
        last_cell_id = lbs->MainCell_Info.cell_id;
        track_cust_paket_18(lbs, 0);
        last_t1 = tick;
        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, G_parameter.lbson.t1 * 1000, track_cust_LBS_upload);
    }
}

/******************************************************************************
 *  Function    -  track_cust_update_LBS
 *
 *  Purpose     -  发送lbs包
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 08-08-2013,  jwj  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_update_LBS(void)
{
#if defined(__GT500__) && !defined(__GW100__)
    if (G_realtime_data.location_switch == 0)
    {
        return;
    }
#elif defined(__GT310__)
    if (track_cust_work_mode_get_fly_mode())return;
#elif defined(__GT02__)
    return;
#elif defined(__GT740__)||defined(__GT420D__)
    if (G_parameter.extchip.mode != 1)
        return;
#elif defined(__GT420D__)
    if (G_parameter.extchip.mode == 3)
        return;
#endif
    if (G_parameter.gps_work.Fti.lbs_interval == 0)
        return;
    LOGD(L_GPS, L_V5, "gps status:%d", track_cust_gps_status());


#if defined(__GT370__)|| defined (__GT380__)
    //在GPS定位或GPS关闭状态下不上传LBS定位包--需求，也就是GPS搜星时中才上报LBS
    if (track_cust_get_work_mode()==3)
    {
        if (track_cust_gps_status() < GPS_STAUS_2D)
        {
            if (G_parameter.wifi.sw==1)
            {
                track_cust_sendWIFI_paket();//传WIFI
            }
            else
            {
                track_os_intoTaskMsgQueueExt(track_cust_send_lbs, (void*)4);
            }
        }
    }
    else
    {
        //在GPS定位或GPS关闭状态下不上传LBS定位包--需求
        if ((G_parameter.wifi.sw==0)&&(track_cust_get_work_mode() == 1))
        {
            if ((track_cust_gps_status() == GPS_STAUS_OFF)&&(track_cust_is_fix_when_gps_off()==0))
            {
                track_os_intoTaskMsgQueue(track_mode1_no_gps_send_lbs);
            }
            else if (track_cust_gps_status() == GPS_STAUS_SCAN)
            {
                track_os_intoTaskMsgQueue(track_mode1_no_gps_send_lbs);
            }
        }
        else if ((track_cust_gps_status() < GPS_STAUS_2D)&&(G_parameter.wifi.sw==1))
        {
            if (track_cust_is_fix_when_gps_off())
            {
                LOGD(L_APP, L_V5, "上次关GPS时定位，本次不发wifi");
            }
            else if ((track_cust_get_work_mode()==2)&&(!track_cust_sensor_delay_valid()))
            {
                //上报间隔小于2分钟，且静止
                //track_cust_sendWIFI_paket();
            }
            else if (track_cust_get_work_mode()==1)
            {
                track_cust_sendWIFI_paket();//传WIFI
            }
        }
    }

    //track_cust_wake_mode();
    /*这个WIFI/LBS会与GPSDUP的包同时上报，且上报WIFI/LBS,影响轨迹   --   chengjun  2017-04-20*/
    if (!track_is_timer_run(TRACK_CUST_LBS_UPDATE_L_TIMER_ID))
    {
        if (track_cust_is_fix_when_gps_off())
        {
            LOGD(L_APP, L_V5, "上次关GPS时定位，不再发wifi");
        }
        else
        {
            LOGD(L_GPS, L_V5, "预计下包WIFI:%ds", track_cust_get_pack_time());
            tr_start_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID, track_cust_get_pack_time() * 1000, track_cust_update_LBS);
        }
    }

    return;
#elif defined (__NT33__) || defined (__GT740__)||defined(__GT420D__)
    //存的就是秒数
    if (!track_is_timer_run(TRACK_CUST_LBS_UPDATE_L_TIMER_ID))
    {
#if !defined(__GT420D__)
        tr_start_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID, G_parameter.gps_work.Fti.lbs_interval * 1000, track_cust_update_LBS);
#endif
    }
#else
    if (!track_is_timer_run(TRACK_CUST_LBS_UPDATE_L_TIMER_ID))
    {
        tr_start_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID, G_parameter.gps_work.Fti.lbs_interval * 1000 * 60, track_cust_update_LBS);
    }
#endif

    {
        LBS_Multi_Cell_Data_Struct *lbs = track_drv_get_lbs_data();
        {
            static kal_uint16 last_cell_id;
#if defined (__GT300S__)
            if (track_cust_get_work_mode() == WORK_MODE_1)
            {
                if (lbs->MainCell_Info.cell_id == 0)
                    return;
            }
            else
#endif /* __GT300S__ */
            {
#if defined(__GT740__)||defined(__GT420D__)
                if (last_cell_id == lbs->MainCell_Info.cell_id || lbs->MainCell_Info.cell_id == 0 || track_cust_sensor_delay_valid_acc() == KAL_FALSE) return;
#else
                if (last_cell_id == lbs->MainCell_Info.cell_id || lbs->MainCell_Info.cell_id == 0)
                    return;
#endif
            }
            last_cell_id = lbs->MainCell_Info.cell_id;
        }
#if defined(__GW100__) || defined(__GT740__) || defined(__GT300S__)||defined(__GT420D__)
        if (track_cust_gps_status() < GPS_STAUS_2D)
        {
            track_cust_paket_18(lbs, 0);
        }
#else
        track_cust_paket_18(lbs, 0);
#endif /* __GW100__ */

    }
}

#if defined(__GT420D__)
void track_cust_check_static_to_sport_overtime(void)
{
   LOGD(L_APP,L_V7,"60秒延时到，状态改变为运动状态");
   tr_stop_timer(TRACK_CUST_MODE_CHECK_STATICTOSPORTOR_OVERTIME_TIMER_ID);
   track_cust_need_check_sportorstatic(0);
   track_cust_change_to_sport();
}

void track_cust_check_sport_to_static_overtime(void)
{
   char buf[100] = {0};
   applib_time_struct currTime_ams = {0};
   LOGD(L_APP,L_V7,"60秒延时到，状态改变为静止状态");
   tr_stop_timer(TRACK_CUST_MODE_CHECK_SPORTORTOSTATIC_OVERTIME_TIMER_ID);
   track_cust_need_check_sportorstatic(0);
                    #if defined(__GT420D__)
                    track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                    snprintf(buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d TIMEOVER\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec);
                    LOGD(5,5,"len:%d",strlen(buf));
                    track_cust_save_logfile(buf, strlen(buf));
                    #endif

   track_cust_change_to_static();
}

kal_uint8 track_cust_check_static_to_sport(kal_uint8 par)
{
	extern kal_uint8 track_cust_change_gps_speed(kal_uint8 arg);
    U8 buf[30];
    kal_uint8 tmp = 0;
    static int i = 0,j;
    track_gps_data_struct *p_gpsdata;
    tmp = track_cust_gps_status();
    if(track_cust_need_check_sportorstatic(99) != 1)
    {
        LOGD(L_APP,L_V5,"1");
        return;
        }
    if(par == 0)
    {
        i = 0;
        j = 0;
        track_cust_need_check_sportorstatic(0);
        tr_stop_timer(TRACK_CUST_MODE_CHECK_STATICTOSPORTOR_TIMER_ID);
        tr_stop_timer(TRACK_CUST_MODE_CHECK_STATICTOSPORTOR_OVERTIME_TIMER_ID);
        return;
        }
    p_gpsdata = track_cust_backup_gps_data(0, NULL);
    LOGD(L_APP,L_V7,"tmp:%d,速度:%0.2f",tmp,p_gpsdata->gprmc.Speed);
    if (tmp > GPS_STAUS_OFF)
    {
        if (tmp >= GPS_STAUS_2D)
        {
        p_gpsdata = track_cust_backup_gps_data(0, NULL);
        if(track_cust_change_gps_speed(99) == 1)
        {
            p_gpsdata->gprmc.Speed = 5;
            }
        if (i < 30)
        {
            if (p_gpsdata->gprmc.Speed >= 0.5)
            {
                LOGD(L_APP,L_V7,"速度大于0.5km/h,%d,%0.2f,",j,p_gpsdata->gprmc.Speed);
                j++;
            }
            else
            {
                LOGD(L_APP,L_V7,"速度小于0.5km/h,%d,%0.2f,",i,p_gpsdata->gprmc.Speed);
                }
            i++;
            track_start_timer(TRACK_CUST_MODE_CHECK_STATICTOSPORTOR_TIMER_ID,1000,track_cust_check_static_to_sport,(void *)1);
            return;
        }
        else
        {
            if(j >= 15)
            {
                tr_stop_timer(TRACK_CUST_MODE_CHECK_STATICTOSPORTOR_TIMER_ID);
                LOGD(L_APP,L_V7,"连续%d次速度大于0.5km/h，状态更新运动",j);
                track_cust_need_check_sportorstatic(0);
                j = 0;
                i = 0;
            }
            else
            {
                LOGD(L_APP,L_V7,"只有%d次速度大于0.5km/h,通知单片机误报开车",j);
                track_cust_send21((void *)2);
                tr_stop_timer(TRACK_CUST_MODE_CHECK_STATICTOSPORTOR_TIMER_ID);
                track_cust_need_check_sportorstatic(0);
                i = 0;
                j = 0;
                #if 0
                if (tmp >= GPS_STAUS_2D)
                    {
                        LOGD(L_APP,L_V5,"GPS定位着，直接传点");
                        p_gpsdata = track_cust_backup_gps_data(0, NULL);
                        track_cust_dsc_location_paket(p_gpsdata,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,10);
                        track_cust_check_beforepoweroff_flag(0);
                    }
                #endif
                track_cust_gps_off_req((void *)3);
                return;
            }
        }
    }
		else
			{
			LOGD(L_APP,L_V5,"GPS不定位，30秒延时最长定位时间");
                    tr_stop_timer(TRACK_CUST_MODE_CHECK_STATICTOSPORTOR_TIMER_ID);
			tr_start_timer(TRACK_CUST_MODE_CHECK_STATICTOSPORTOR_OVERTIME_TIMER_ID,30*1000,track_cust_check_static_to_sport_overtime);
                    i = 0;
                    j = 0;
			return;
			}
    }
    else
    {
        if (j >= 15)
        {
            tr_stop_timer(TRACK_CUST_MODE_CHECK_STATICTOSPORTOR_TIMER_ID);
            LOGD(L_APP,L_V7,"当前定位状态变为不定位，上一次连续%d次速度大于2km/h,状态更新为运动",j);
            i = 0;
            j = 0;
            track_cust_need_check_sportorstatic(0);
        }
        else
        {
            i = 0;
            j = 0;
            tr_stop_timer(TRACK_CUST_MODE_CHECK_STATICTOSPORTOR_TIMER_ID);
            track_cust_gps_work_req((void *)13);
            if (G_parameter.extchip.mode == 2)
            {
                LOGD(L_APP,L_V7,"模式2 当前定位状态为不定位，60秒延时最长定位时间");
				tr_start_timer(TRACK_CUST_MODE_CHECK_STATICTOSPORTOR_OVERTIME_TIMER_ID,60*1000,track_cust_check_static_to_sport_overtime);
				return;
            }
            else if (G_parameter.extchip.mode == 1)
            {
                LOGD(L_APP,L_V7,"模式1 当前定位状态为不定位,开启GPS,60秒延时最长定位时间");
				tr_start_timer(TRACK_CUST_MODE_CHECK_STATICTOSPORTOR_OVERTIME_TIMER_ID,60*1000,track_cust_check_static_to_sport_overtime);
                return;
            }
        }
    }
	track_cust_change_to_sport();
}
#endif

/******************************************************************************
 *  Function    -  track_gps_locate_data_receive
 *
 *  Purpose     -  接收GPS  解码数据
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 03-08-2012,  chengjun  written
 *
 *  提供全部数据包，后续可带实参使用
 * ----------------------------------------
 ******************************************************************************/
void track_gps_locate_data_receive(track_gps_data_struct * gps_decode_data)
{
    static kal_uint8 flag = 0;
    char buf[100] = {0};
    applib_time_struct currTime_ams = {0};
    LOGD(L_GPS, L_V1, "gps_decode_data->status=%d,speed:%0.2f", gps_decode_data->status,gps_decode_data->gprmc.Speed);
    if (gps_decode_data == NULL || gps_status == 0)
    {
        LOGD(L_GPS, L_V1, "gps_status=%d, error!", gps_status);
        return;
    }
    g_restart_gps_count = 0;
#if defined(__GT530__)
    tr_start_timer(TRACK_CUST_PERIODIC_RESTART_GPS_TIMER_ID, 90000, track_cust_gps_nodata_restart);
#else
    tr_start_timer(TRACK_CUST_PERIODIC_RESTART_GPS_TIMER_ID, 20000, track_cust_gps_nodata_restart);
#endif

#if defined(__SV_UPLOAD__)
    //卫星信息上传
    if (G_parameter.sv.sw == 1 && track_soc_login_status() == 1)
    {
        track_cust_net_sv_upload(1);
    }
#endif/*GT500L*/

#if defined(__GT370__)|| defined (__GT380__)
    if (last_gps_status != gps_decode_data->status )
    {
        last_gps_status = gps_decode_data->status;
    }
#endif
    #if 0
    if (gps_status < GPS_STAUS_2D && (gps_decode_data->status == 2|| gps_decode_data->status == 3))
    {
        gps_status = gps_decode_data->status;
    }
    else if ((gps_status == GPS_STAUS_2D ||gps_status == GPS_STAUS_3D) && gps_decode_data->status < GPS_STAUS_2D)
    {
        gps_status = gps_decode_data->status;
        }
    #endif
    //2D-3D定位
    if (gps_decode_data->status == 2 || gps_decode_data->status == 3)
    {
        static kal_uint32 gps_just_location_tick = 0; // 记录由不定位转定位的时间值
#if 0//!defined( __GT03D__) && !defined( __GT300__) && !defined(__GT500__) && !defined( __GT02__) && !defined( __GT02E__) && !defined(__ET130__) && !defined(__ET200__) && !defined(__NT23__) && !defined(__GT06D__)
        tr_stop_timer(TRACK_CUST_GPS_LOCATION_TIMEOUT_TIMER_ID);
        if (net_upload_position && !track_is_timer_run(TRACK_CUST_GPS_LOCATION_WORKTIME_TIMER_ID))
        {
            track_start_timer(TRACK_CUST_GPS_LOCATION_WORKTIME_TIMER_ID, G_parameter.sensor_gps_work * 60000, gps_location_timeout_ind, (void*)5);
        }
#endif
        if (!track_cust_is_upload_UTC_time())
        {
            applib_time_struct time = {0};
            /* GPS数据中的时间加上时区校正 */
            track_drv_utc_timezone_to_rtc(&time, (applib_time_struct*)&gps_decode_data->gprmc.Date_Time, G_parameter.zone);
            memcpy(&gps_decode_data->gprmc.Date_Time, &time, sizeof(applib_time_struct));
        }
        LOGD(L_GPS, L_V7, "time(%d%c): %0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
             G_parameter.zone.time, G_parameter.zone.zone,
             gps_decode_data->gprmc.Date_Time.nYear,
             gps_decode_data->gprmc.Date_Time.nMonth,
             gps_decode_data->gprmc.Date_Time.nDay,
             gps_decode_data->gprmc.Date_Time.nHour,
             gps_decode_data->gprmc.Date_Time.nMin,
             gps_decode_data->gprmc.Date_Time.nSec);

        /* 备份成功定位的最新一条数据 */
        track_cust_backup_gps_data(1, gps_decode_data);
        positioned = KAL_TRUE; 

        #if defined(__GT420D__)
        if (track_is_timer_run(TRACK_CUST_MODE_CHECK_STATICTOSPORTOR_OVERTIME_TIMER_ID) == KAL_TRUE)
        {
            tr_stop_timer(TRACK_CUST_MODE_CHECK_STATICTOSPORTOR_OVERTIME_TIMER_ID);
			}
        if (track_is_timer_run(TRACK_CUST_MODE_CHECK_SPORTORTOSTATIC_OVERTIME_TIMER_ID) == KAL_TRUE)
        {
		tr_stop_timer(TRACK_CUST_MODE_CHECK_SPORTORTOSTATIC_OVERTIME_TIMER_ID);
			}
		#endif

        /* 地址查询请求 */
        if (track_is_timer_run(TRACK_CUST_GET_ADDRESS_OVER2_TIMER_ID))
        {
            track_stop_timer(TRACK_CUST_GET_ADDRESS_OVER2_TIMER_ID);
#if !defined(__GT420D__)
            track_os_intoTaskMsgQueueExt(track_cust_module_get_addr, NULL);
#endif
        }
#if !(defined (__XCWS__)|| defined(__GT740__)|| defined(__GT370__)|| defined(__GT380__)||defined(__GT420D__))
        /* 断油电 */
        track_cust_module_oil_cut_gps(gps_decode_data);
#endif

        if (track_is_timer_run(TRACK_CUST_WORK_MODE_GPSUPDATE_OVERTIME_TIMER_ID) == KAL_TRUE)
        {
            tr_stop_timer(TRACK_CUST_GPS_VIEW_TIMER_ID);
            tr_stop_timer(TRACK_CUST_GPS_SNR_TIMER_ID);
            #if defined(__GT420D__)
            track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                snprintf(buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d GPS LOCATION\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec);
                LOGD(5,5,"len:%d",strlen(buf));
                track_cust_save_logfile(buf, strlen(buf));
            #endif

            track_cust_gps_send_data(2);
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
            if (track_cust_get_work_mode() == WORK_MODE_3)
            {

            }
            else if (!net_upload_position)
#elif defined(__GT310__)
            //if(!net_upload_position) ?????有什么用 xzq
#else
            if (!net_upload_position)
#endif /* __GT300S__ */
            {
                return;
            }
        }
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
        if (track_cust_get_work_mode() == WORK_MODE_3)
        {

        }
        else if ((!net_upload_position) && (track_is_timer_run(TRACK_CUST_WORK_MODE_GPSUPDATE_OVERTIME_TIMER_ID) == KAL_FALSE))
        {
            LOGD(L_GPS, L_V6, "不检围栏、超速，不上传定位点。");
            if (track_is_timer_run(TRACK_CUST_GPS_UPDATE_EPHEMERIS_OVERTIME_TIMER_ID) == KAL_TRUE)
            {
                track_stop_timer(TRACK_CUST_GPS_UPDATE_EPHEMERIS_OVERTIME_TIMER_ID);
                tr_stop_timer(TRACK_CUST_GPS_LOCATION_TIMEOUT_TIMER_ID);
                tr_stop_timer(TRACK_CUST_GPS_LOCATION_WORKTIME_TIMER_ID);
                track_cust_gps_location_timeout(10);
            }
//#ifndef __GT300S__
            return; // 过滤由于星历更新带来的漂移影响
//#endif /* __GT300S__ */
        }
#else
        #if  !defined(__GT420D__)
        if ((!net_upload_position) && (track_is_timer_run(TRACK_CUST_WORK_MODE_GPSUPDATE_OVERTIME_TIMER_ID) == KAL_FALSE))
        {
            LOGD(L_GPS, L_V6, "不检围栏、超速，不上传定位点。");
            if (track_is_timer_run(TRACK_CUST_GPS_UPDATE_EPHEMERIS_OVERTIME_TIMER_ID) == KAL_TRUE)
            {
                track_stop_timer(TRACK_CUST_GPS_UPDATE_EPHEMERIS_OVERTIME_TIMER_ID);
                tr_stop_timer(TRACK_CUST_GPS_LOCATION_TIMEOUT_TIMER_ID);
                tr_stop_timer(TRACK_CUST_GPS_LOCATION_WORKTIME_TIMER_ID);
                track_cust_gps_location_timeout(10);
            }
            return; // 过滤由于星历更新带来的漂移影响
        }
        #endif
#endif /* __GT300S__ */
        LOGD(L_GPS, L_V1, "gps_status=%d,gps_decode_data->status=%d", gps_status,gps_decode_data->status);
        if (gps_status != GPS_STAUS_2D && gps_decode_data->status == 2) //首次进入2D定位
        {
            if (!flag)
            {
                flag = 1;
            }
            gps_status = GPS_STAUS_2D;
#if defined(__GT310__)
            led_level_state_switch();
#else
            LED_GPS_StatusSet(GPS_LOCATION_STATE);
#endif
            track_cust_module_alarm_blind();
#if defined(__GT03D__)|| defined(__GT300__) || defined(__GW100__)||defined(__GT300S__)
            if ((track_cust_get_work_mode() == WORK_MODE_2) || (track_cust_get_work_mode() == WORK_MODE_4))
            {
                track_cust_stop_update_LBS();
            }
#elif defined(__GT310__)
            track_cust_stop_update_LBS();
            track_stop_timer(TRACK_CUST_WORK_MODE1_GPSON_TIMER_ID);
#elif defined(__GT370__)|| defined (__GT380__)
            last_gps_status = GPS_STAUS_2D;
            if ((track_cust_get_work_mode() == WORK_MODE_2))
            {
                track_cust_stop_update_LBS();
            }
#elif defined(__GT500__)
            track_cust_stop_update_LBS();
#endif
            if (track_is_timer_run(TRACK_CUST_DWCHANGE_INSTANT_CONTRACT_TIMER_ID))
            {
                track_stop_timer(TRACK_CUST_DWCHANGE_INSTANT_CONTRACT_TIMER_ID);
            }
            LOGD(L_GPS, L_V4, "GPS 2D定位!");
#if defined(__GT420D__)
            track_cust_backup_gps_data(3, gps_decode_data);
            LOGD(L_APP,L_V8,"更新有效点");
#endif
            if (track_cust_need_check_sportorstatic(99) == 1)
            {
                if(track_cust_gt420d_carstatus_data(99) == 1)
                	{
                	track_cust_set_sportorstatic(1);
                	}
                else if(track_cust_gt420d_carstatus_data(99) == 2)
                    {
                        track_cust_check_static_to_sport(1);
                        }
            }
#if defined(__GT300S__)
            track_cust_intermittent_send_LBS((void *)2);
#endif /* __GT740__ */
            track_cust_LBS_upload();
        }
        else if (gps_status != GPS_STAUS_3D && gps_decode_data->status == 3) //首次进入3D定位
        {
            if (!flag)
            {
                flag = 1;
            }
            gps_status = GPS_STAUS_3D;
#if defined(__GT310__)
            led_level_state_switch();
#else
            LED_GPS_StatusSet(GPS_LOCATION_STATE);
#endif
            track_cust_module_alarm_blind();
#if defined(__GT03D__)|| defined(__GT300__)  || defined(__GW100__)||defined(__GT300S__)
            if ((track_cust_get_work_mode() == WORK_MODE_2) || (track_cust_get_work_mode() == WORK_MODE_4))
            {
                track_cust_stop_update_LBS();
            }
#elif defined(__GT310__)
            track_cust_stop_update_LBS();
            track_stop_timer(TRACK_CUST_WORK_MODE1_GPSON_TIMER_ID);
#elif defined(__GT370__)|| defined (__GT380__)
            last_gps_status = GPS_STAUS_3D;
            if ((track_cust_get_work_mode() == WORK_MODE_2))
            {
                track_cust_stop_update_LBS();
            }
#elif defined(__GT500__)
            track_cust_stop_update_LBS();
#endif
            if (track_is_timer_run(TRACK_CUST_DWCHANGE_INSTANT_CONTRACT_TIMER_ID))
            {
                track_stop_timer(TRACK_CUST_DWCHANGE_INSTANT_CONTRACT_TIMER_ID);
            }
            LOGD(L_GPS, L_V4, "GPS 3D定位!");
#if defined(__GT420D__)
            track_cust_backup_gps_data(3, gps_decode_data);
            LOGD(L_APP,L_V7,"更新有效点");
#endif
            if (track_cust_need_check_sportorstatic(99) == 1)
            {
                if(track_cust_gt420d_carstatus_data(99) == 1)
                	{
                	track_cust_set_sportorstatic(1);
                	}
                else if(track_cust_gt420d_carstatus_data(99) == 2)
                    {
                        track_cust_check_static_to_sport(1);
                        }
            }
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
            track_cust_intermittent_send_LBS((void *)2);
#endif /* __GT740__ */
            track_cust_LBS_upload();
        }
#if defined(__BCA__)
        if (track_is_timer_run(TRACK_CUST_LBS_UPDATE_L_TIMER_ID))
            track_stop_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID);
        if (track_is_timer_run(TRACK_CUST_SHOCKS_UPLBS_TIMER_ID))
            track_stop_timer(TRACK_CUST_SHOCKS_UPLBS_TIMER_ID);
#endif
        if (flag == 1)
        {
            flag = 2;
            gps_just_location_tick = kal_get_systicks();
            if (G_realtime_data.netLogControl & 8)
            {
                cust_packet_log(8, 2, 0, 0, 0, 0, 0);
            }
        }

        /* 定位后的经纬度上传 */
        #if defined(__GT420D__)
        if(G_parameter.extchip.mode == 2)
        {
            track_cust_net_upload_position(gps_decode_data, gps_just_location_tick);
            }
        #else
        track_cust_net_upload_position(gps_decode_data, gps_just_location_tick);
        #endif

        if (track_is_timer_run(TRACK_CUST_GPS_COLD_START_TIMER_ID))
        {
            track_cust_gps_check_nofix(0);
        }

#if defined (__CUSTOM_DDWL__)
        g_daily_gps_fix=1;
        track_cust_dormancy_daily_gps_fix_check_gprs();
#endif /* __CUSTOM_DDWL__ */

    }
    else
    {
        flag = 0;
        if (gps_status != GPS_STAUS_SCAN)
        {
            gps_status = GPS_STAUS_SCAN;
            track_cust_set_anglerep_upload(0);// 定位到不定位不上传拐角
#if defined(__GT310__)
            led_level_state_switch();
#else
            LED_GPS_StatusSet(GPS_SCAN_STATE);
#endif
            track_cust_module_alarm_blind();
#if defined(__GT310__) ||defined(__GT03D__)  || defined(__GW100__)||defined(__GT300S__)
            if (!track_is_timer_run(TRACK_CUST_LBS_UPDATE_L_TIMER_ID) && ((track_cust_get_work_mode() == WORK_MODE_2) || (track_cust_get_work_mode() == WORK_MODE_4)) && net_upload_position)
            {
                tr_start_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID, 5 * 1000 * 60, track_cust_update_LBS);
            }
#elif defined(__GT370__)|| defined (__GT380__)
            last_gps_status = GPS_STAUS_SCAN;
            if (!track_is_timer_run(TRACK_CUST_LBS_UPDATE_L_TIMER_ID) && ((track_cust_get_work_mode() == WORK_MODE_2) || (track_cust_get_work_mode() == WORK_MODE_1)))
            {
                tr_start_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID, 2 * 1000 * 60, track_cust_update_LBS);
            }
#elif defined(__GT500__)
            if (!track_is_timer_run(TRACK_CUST_LBS_UPDATE_L_TIMER_ID) && (net_upload_position == 1))
            {
                tr_start_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID, 2 * 1000 * 60, track_cust_update_LBS);
            }
#elif defined(__GT740__)||defined(__GT420D__)
            if (!track_is_timer_run(TRACK_CUST_LBS_UPDATE_L_TIMER_ID))
            {
#if !defined(__GT420D__)
                tr_start_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID,  G_parameter.gps_work.Fti.lbs_interval * 1000, track_cust_update_LBS);
#endif
            }
#endif
            LOGD(L_GPS, L_V4, "GPS 未定位!");
            tr_stop_timer(TRACK_CUST_MODE_CHECK_STATICTOSPORTOR_TIMER_ID);
            if (G_realtime_data.netLogControl & 8)
            {
                cust_packet_log(8, 3, 0, 0, 0, 0, 0);
            }
#if !defined(__BCA__)
            track_start_timer(TRACK_CUST_DWCHANGE_INSTANT_CONTRACT_TIMER_ID, 30 * 1000, track_cust_instant_contract_position_data, (void *) 1);
#endif/*__BCA__*/
            track_cust_gps_check_nofix(1);//启动gps冷启动检测
            track_cust_LBS_upload();
        }
    }
}

/******************************************************************************
 *  Function    -  track_cust_gps_status
 *
 *  Purpose     -  GPS状态
 *
 *  return      -  0: GPS关闭
 *                 1: GPS开启
 *                 2: 搜  星
 *                 3: 2D定位
 *                 4: 3D定位
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 30-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
track_enum_gps_staus track_cust_gps_status(void)
{
    return gps_status;
}

/******************************************************************************
 *  Function    -  track_cust_gps_positioned_ever_after_booting
 *
 *  Purpose     -  返回开机之后是否曾经定位过
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 17-01-2014,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_bool track_cust_gps_positioned_ever_after_booting(void)
{
    return positioned;
}

/******************************************************************************
 *  Function    -  track_cust_gps_cmd_change_set
 *
 *  Purpose     -  更新在线指令对延时工作时间的修改
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_gps_cmd_change_set(void)
{
    if (G_parameter.sensor_gps_work == 0 && gps_status == 0)
    {
        track_cust_gps_work_req((void*)7);
    }
    if (G_parameter.sensor_gps_work > 0)
    {
        if (!track_is_timer_run(TRACK_CUST_GPS_LOCATION_WORKTIME_TIMER_ID))
        {
            track_start_timer(TRACK_CUST_GPS_LOCATION_WORKTIME_TIMER_ID, G_parameter.sensor_gps_work * 60000, gps_location_timeout_ind, (void*)6);
        }
    }
}

/******************************************************************************
 *  Function    -  track_cust_poweron_close_gps
 *
 *  Purpose     -  开机第一次上传GPS定位点后关闭GPS
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 07-08-2013,  ZengPing && Liujw  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_poweron_close_gps(void)
{
    kal_uint16 sn;
    track_gps_data_struct *p_gpsdata = track_cust_gpsdata_alarm();
    track_cust_backup_gps_data(3, p_gpsdata);
#if defined (__GT300S__) || defined(__GT370__)|| defined (__GT380__)
    if (track_cust_get_work_mode() == WORK_MODE_3)
    {
        return;
    }
#endif
#if defined(__GT500__) && !defined(__GW100__)
    if (G_realtime_data.location_switch)
#endif
#if !defined(__ET310__)
    {
        sn = track_cust_paket_position(p_gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_POWER_ON, 0);
    }
#endif /* __ET310__ */
    LOGD(L_GPS, L_V5, "传完点后关闭GPS %d", track_cust_get_work_mode());
    if (track_is_timer_run(TRACK_CUST_POWERON_CLOSE_GPS_TIMER))
    {
        track_stop_timer(TRACK_CUST_POWERON_CLOSE_GPS_TIMER);
    }
    if (track_is_timer_run(TRACK_CUST_GPS_LOCATION_TIMEOUT_TIMER_ID))
    {
        track_stop_timer(TRACK_CUST_GPS_LOCATION_TIMEOUT_TIMER_ID);
    }
    gps_location_timeout_ind((void*)16);
#if defined (__GT300S__)
#elif defined(__GT370__)|| defined (__GT380__)
    track_cust_mode_change(3);
#elif defined(__GT310__)
    if (track_cust_get_work_mode() == WORK_MODE_1)
    {
        tr_start_timer(TRACK_CUST_WORK_MODE1_GPSON_TIMER_ID, 30*1000,track_cust_mode1_gpsfix_overtime);
        track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID, G_parameter.work_mode.mode1_time_sec*1000,track_cust_gps_work_req, (void *)13);//
    }
#else
#if !defined(__GT420D__)
    if (track_cust_get_work_mode() == WORK_MODE_1)
    {
        track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID, G_parameter.gps_work.Fti.mode1_gps_time * 60 * 1000, track_cust_gps_work_req, (void *)13);//
    }
#else
#if defined(__GT420D__)
    if (G_parameter.extchip.mode == 1 && G_parameter.extchip.mode1_par1 < 5)
    {
        LOGD(L_APP, L_V1, "模式一数据定时搜索成功,%d分钟后进行下次搜索!", G_parameter.extchip.mode1_par1);
        track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID, G_parameter.extchip.mode1_par1 * 60 * 1000, track_cust_gps_work_req, (void *)13);
    }
#endif
#endif
#endif /* __GT300S__ */
#if defined( __GT03D__)
    track_cust_key_led_sleep(2);
#endif /* __GT03D__ */
}
#if defined(__GT370__)|| defined (__GT380__)
void track_mode1_no_gps_send_lbs(void)
{
    track_cust_paket_18(track_drv_get_lbs_data(),0);
}

void track_send_lbs_limit_rtc(void)
{
    track_cust_paket_18(track_drv_get_lbs_data(),0);
}

kal_uint16 track_gt370_get_upload_time(void)
{
    if (track_cust_sensor_delay_valid())//在震动模式上传按照interval_acc_on否则interval_acc_off
    {
        return G_parameter.gps_work.Fti.interval_acc_on;
    }
    else
    {
        return G_parameter.gps_work.Fti.interval_acc_off;
    }
}

void track_mode1_gt370_reset_work(void)
{
    applib_time_struct currTime = {0};
    kal_uint16       datetime;//起始时间

    if (G_parameter.work_mode.mode != 1)
    {
        LOGD(L_GPS, L_V2, " 模式%d  不循环开启.", G_parameter.work_mode.mode);
        return;
    }
    datetime = track_gt370_get_upload_time();
    track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID, datetime * 1000, track_cust_gps_work_req, (void *)13);//
    LOGD(L_GPS, L_V2, " 下次开gps时间%d秒", datetime);
}

#elif defined(__GT300S__)
void track_mode1_gt300s_reset_work(void)
{
    applib_time_struct currTime = {0};
    kal_uint16       datetime;//起始时间
    if (G_parameter.work_mode.mode1_timer == 0)
    {
        LOGD(L_GPS, L_V2, " 间隔时间为0不开启gps以后");
        return;
    }
    if (G_parameter.work_mode.mode != 1)
    {
        LOGD(L_GPS, L_V2, " 模式%d  不循环开启.", G_parameter.work_mode.mode);
        return;
    }

    if (1) ///  if(track_fun_update_time_to_system(0,NULL)==99)
    {
        track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID, G_parameter.work_mode.mode1_timer * 60 * 1000, track_cust_gps_work_req, (void *)13);//
        LOGD(L_GPS, L_V2, " 下次开gps时%d分钟", G_parameter.work_mode.mode1_timer);
    }
    else
    {
        track_fun_get_time(&currTime, KAL_FALSE, &G_parameter.zone);
        datetime = ((G_parameter.work_mode.datetime + 24 * 60) - currTime.nHour * 60 - currTime.nMin) % G_parameter.work_mode.mode1_timer;
        if (datetime == 0)
        {
            datetime = +G_parameter.work_mode.mode1_timer;
        }
        track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID, datetime * 60 * 1000, track_cust_gps_work_req, (void *)13);//

        LOGD(L_GPS, L_V2, " 下次开gps时%d分钟", datetime);

    }
}

#endif /*  __GT300S__ */

void track_cust_check_send_lbs(void)
{
    static kal_uint16 last_cell_id;
    LBS_Multi_Cell_Data_Struct *lbs = track_drv_get_lbs_data();
    applib_time_struct time = {0};
    applib_time_struct *currTime = NULL;
    currTime = &time;
    if (track_cust_gps_status() >= GPS_STAUS_2D)
    {
        track_stop_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID);
        LOGD(L_GPS, L_V5, "1");
        return; /* LBS 在 GPS 不定位状态下才传 */
    }
    if (track_cust_is_upload_UTC_time())
    {
        track_fun_get_time(currTime, KAL_TRUE, NULL);
    }
    else
    {
        track_fun_get_time(currTime, KAL_FALSE, &G_parameter.zone);
    }
    LOGD(L_GPS, L_V5, "time: %d-%d-%d ", currTime->nYear, currTime->nMonth, currTime->nDay);
    if (lbs->MainCell_Info.cell_id == 0)
    {
        LOGD(L_GPS, L_V5, "Warning: cell_id is Zero, cannot upload ");//delay 10s! ");
        if (G_parameter.extchip.mode == 1 && G_parameter.extchip.mode1_par1 >= 6)
        {
            tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 10000, track_cust_check_send_lbs);
        }
        return;
    }
    if (currTime->nYear == 2015 && currTime->nMonth == 1 && currTime->nDay == 1)
    {
        LOGD(L_GPS, L_V5, "Warning: current time is not Synchronous! ");
        if (G_parameter.extchip.mode == 1 && G_parameter.extchip.mode1_par1 >= 6)
        {
            tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 10000, track_cust_check_send_lbs);
        }
        return;
    }
#if 0
    if (last_cell_id == lbs->MainCell_Info.cell_id || lbs->MainCell_Info.cell_id == 0)
    {
        LOGD(L_GPS, L_V5, "Warning: cell id the same with the previous! ");
        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 5000, track_cust_LBS_upload);
        return;
    }
#endif
    last_cell_id = lbs->MainCell_Info.cell_id;
    track_cust_paket_18(lbs, 0);
}

//定时定位工作模式下上传点

/******************************************************************************
 *  Function    -  track_cust_gps_send_data
 *
 *  Purpose     -  定时定位工作模式下上传点
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 17-10-2013,  ZengPing && Liujw  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_gps_send_data(kal_uint32 status)
{
#if defined(__GT420D__)
    extern void track_cust_module_update_mode(void);
    U8 buf[30];
#endif
    kal_uint8 tmp = 0;
    track_gps_data_struct *p_gpsdata;
    static kal_uint32 flag = 0;
    kal_uint16 sn;
    LBS_Multi_Cell_Data_Struct *lbs = track_drv_get_lbs_data();
    tmp = track_cust_gps_status();
    if (flag == 2 && status == 2)
    {
        return ;//避免重复重置10秒关闭GPS定时器
    }
    else
    {
        flag = status;
    }
    LOGD(L_GPS, L_V5, " flag =%d", flag);
    if (flag == 0)
    {
#if defined (__GW100__)
        LOGD(L_GPS, L_V6, "定时唤醒GPS，搜星超时,关闭GPS");
#elif defined(__GT370__)|| defined (__GT380__)
        //传WIFI
        if (track_cust_get_work_mode()==1)
        {
            track_cust_sendWIFI_paket();
        }
#elif defined(__GT310__)
        if (track_cust_get_work_mode()!=WORK_MODE_3)
        {
            track_cust_sendWIFI_paket();
        }
#elif (!defined (__GT740__)|| defined(__GT300S__)) && !defined(__GT420D__)
        LOGD(L_GPS, L_V6, "定时唤醒GPS，搜星超时，发送LBS点");
        track_cust_update_LBS();
#endif /* __GW100__ */
#if defined(__GT420D__)
        if (G_parameter.extchip.mode != 2||G_parameter.extchip.mode != 3)
        {
            LOGD(L_GPS, L_V6, "定时唤醒GPS，搜星超时，关闭GPS");
            gps_location_timeout_ind((void*)17);
#if defined(__GT420D__)
            track_cust_module_overtime_upwifi();
#endif
        }
#else
        gps_location_timeout_ind((void*)17);
#endif
    }
    else if (flag == 1)
    {

        track_gps_data_struct *p_gpsdata = track_cust_backup_gps_data(0, NULL);
        LOGD(L_GPS, L_V6, "定时唤醒GPS，%d秒延时到，上传定位点",UP_TIME);
        track_cust_backup_gps_data(3, p_gpsdata);

#if defined (__GT370__) || defined (__GT380__)|| defined(__GT310__)
        sn = track_cust_paket_position(p_gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_TIMING, 0);
#elif defined(__GT500__) && !defined(__GW100__)
        if (G_realtime_data.location_switch)
        {
            sn = track_cust_paket_position(p_gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_MODE_GPSUPDATE, 0);
        }
#else
            sn = track_cust_paket_position(p_gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_MODE_GPSUPDATE, 0);
#endif
#if defined(__GT420D__)
        if (G_parameter.extchip.mode == 1)
        {
            if (track_cust_need_check_sportorstatic(99) == 1)
            {
                LOGD(L_APP,L_V5,"检测到运动转静止下需要检测速度，暂时不关闭GPS");
                return;
            }
            LOGD(L_APP,L_V5,"准备关闭GPS");
            gps_location_timeout_ind((void*)18);
            track_cust_gps_off_req((void*)3);
        }
#endif
    }
    else if (flag == 2)
    {
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
        if (track_cust_get_work_mode() == WORK_MODE_3 || track_cust_get_work_mode() == WORK_MODE_1)
        {
            track_cust_set_upload_status();
            tr_stop_timer(TRACK_CUST_SEND_LBS_TIMER_ID);
            tr_stop_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID);//取消按键发的LBS点
        }
#endif /* __GT300S__ */
        track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_OVERTIME_TIMER_ID, UP_TIME * 1000, track_cust_gps_send_data, (void *) 1);
        LOGD(L_GPS, L_V6, "定时唤醒GPS，已经定位，%d秒后传点",UP_TIME);
    }
    else
    {
        LOGD(L_GPS, L_V6, " Error  flag =%d", flag);
    }

#if defined (__GT300S__)
    if (flag == 0 || flag == 1)
    {
        track_mode1_gt300s_reset_work();
    }
#elif defined(__GT310__)
    if (flag == 0 || flag == 1)
    {
        if (G_parameter.work_mode.mode==WORK_MODE_1)
        {
            track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID, (G_parameter.work_mode.mode1_time_sec-5) * 1000, track_cust_gps_work_req, (void *)13);//
        }
        else if (G_parameter.work_mode.mode==WORK_MODE_2)
        {
            if (track_is_timer_run(TRACK_CUST_GPS_LOCATION_WORKTIME_TIMER_ID))
            {
                track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID, (G_parameter.work_mode.mode2_time_sec-5) * 1000, track_cust_gps_work_req, (void *)13);//
            }
        }

    }
#elif defined(__GT370__)|| defined (__GT380__)
    if (flag == 0 || flag == 1)
    {
        track_cust_mode_change(4);
    }
#else
#if !defined(__GT420D__)
    if (flag == 0 || flag == 1)
    {

        track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID, G_parameter.gps_work.Fti.mode1_gps_time * 60 * 1000, track_cust_gps_work_req, (void *)13);//
    }
#endif
#endif /* __GT300S__ */
}

/******************************************************************************
 *  Function    -  track_cust_gps_cold_start
 *
 *  Purpose     -  GPS冷启动
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 31-10-2013,  ZengPing && Liujw  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_gps_cold_start(void)
{
    LOGD(L_GPS, L_V4, "执行冷启动");
#if defined (__MNL_SUPPORT__)
    track_drv_gps_cold_restart();
#elif defined(__GPS_UART__)//||defined(__GPS_MT3333__)
    track_cust_ubxcold();
    track_gps_data_clean();
#endif /* __MNL_SUPPORT__ */
    track_cust_gps_check_nofix(2);
}

void track_cust_gps_check_nofix(kal_uint8 flag)
{
    static kal_uint8 state = 0;

    LOGD(L_GPS, L_V6, "flag:%d,state:%d", flag, state);
    if (flag == 1 && state == 0)
    {
        state = 1;
        tr_start_timer(TRACK_CUST_GPS_COLD_START_TIMER_ID, 10 * 60 * 1000, track_cust_gps_cold_start);
    }
    else if (flag == 0 && state == 1)
    {
        state = 0;
        if (track_is_timer_run(TRACK_CUST_GPS_COLD_START_TIMER_ID))
        {
            tr_stop_timer(TRACK_CUST_GPS_COLD_START_TIMER_ID);
        }
    }
    else if (flag == 2)
    {
        state = 2;
    }
}

kal_uint32 track_cust_gpson_timer(kal_uint32 time)
{
    static kal_uint32 gpson_timer = 5;
    if (time == 0)
    {
        gpson_timer = 5;
    }
    else if (time == 999)
    {
        LOGD(L_GPS, L_V6, "gpson_timer = %d", gpson_timer);
        return gpson_timer;
    }
    else
    {
        gpson_timer = time;
    }
    LOGD(L_GPS, L_V6, "time = %d;gpson_timer = %d", time, gpson_timer);
}

kal_bool track_cust_time_slot(void)
{
    int i;
    kal_uint8 DayIndex;
    applib_time_struct currTime = {0};
    if (track_cust_is_upload_UTC_time())
    {
        track_fun_get_time(&currTime, KAL_TRUE, NULL);
    }
    else
    {
        track_fun_get_time(&currTime, KAL_FALSE, &G_parameter.zone);
    }
    DayIndex = currTime.DayIndex;
    if (DayIndex == 0) DayIndex = 2;
    else if (DayIndex == 6) DayIndex = 1;
    else DayIndex = 0;

    for (i = 0; i < 8; i++)
    {
        if (G_realtime_data.timeslot[DayIndex][i][0] == 1
                && currTime.nHour >= G_realtime_data.timeslot[DayIndex][i][1]
                && currTime.nMin >= G_realtime_data.timeslot[DayIndex][i][2]
                && currTime.nHour <= G_realtime_data.timeslot[DayIndex][i][3]
                && currTime.nMin <= G_realtime_data.timeslot[DayIndex][i][4])
        {
            return KAL_TRUE;
        }
    }
    return KAL_FALSE;
}

void track_cust_gps_check(void)
{
    if (track_cust_gps_status() == GPS_STAUS_OFF)
    {
        track_drv_encode_exmode(0x77, 0x0101, 0x91, "GPSOFF", 6);
    }
}

void track_cust_intermittent_send_LBS(void * arg)
{
    static U32 shock_count = 0;
    static kal_bool gps = KAL_FALSE;
    U32 par = (U32)arg;
    LOGD(L_GPS, L_V7, "");
    if (G_parameter.gps_work.Fti.lbs_interval == 0)
        return;
    if (par == 1) // 检测到震动
    {
        shock_count++;
        return;
    }
    else if (par == 2) // GPS 定位
    {
        gps = KAL_TRUE;
        return;
    }
    track_start_timer(
        TRACK_CUST_LBS_UPDATE_L_TIMER_ID,
        G_parameter.gps_work.Fti.lbs_interval * 60000,
        track_cust_intermittent_send_LBS,
        (void*)0);

    if (par == 3) return;

    if (shock_count > G_parameter.gps_work.Fti.lbs_interval * 10
            && !gps && gps_status < 3)
    {
        LBS_Multi_Cell_Data_Struct *lbs = track_drv_get_lbs_data();
        /*static kal_uint16 last_cell_id;
        if(last_cell_id == lbs->MainCell_Info.cell_id || lbs->MainCell_Info.cell_id == 0)
        {
            LOGD(L_GPS, L_V5, "LBS 与上一次相同");
            return;
        }
        last_cell_id = lbs->MainCell_Info.cell_id;*/
        track_cust_paket_18(lbs, 0);
    }
    else
    {
        LOGD(L_GPS, L_V5, "LBS固定间隔上传: 震动=%d/%d, GPS曾经定过位:%d, GPS当前定位状态:%d",
             shock_count, G_parameter.gps_work.Fti.lbs_interval * 10,
             gps, gps_status);
        gps = KAL_FALSE;
        shock_count = 0;
    }
}

/******************************************************************************
 *  Function    -  track_cust_instant_contract_position_data
 *
 *  Purpose     -  即时发包，已打包的定位数据
 *
 *  Description -flag=1,定位→不定位，发包。
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 02-12-2015,  Cjj written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_instant_contract_position_data(kal_uint8 flag)
{
    if (!G_parameter.group_package.sw)
    {
        return;
    }
    LOGD(L_GPS, L_V4, "flag:%d", flag);

    if (flag == 1) //GPS常开条件时触发，待验证效果
    {
        //由定位到不定位，立即上传已有数据包
        LOGD(L_GPS, L_V4, " 定位转不定位发包");
        track_soc_instant_upload_position_status();
        track_soc_send((void*)9);
        return;
    }
    else if (flag == 2)
    {
        //  GPS进入休眠前，立即上传打包已有的定位数据；
        LOGD(L_GPS, L_V5, " !!!!!!!!!!休眠发包!!!!!!!!!!!!!!!");
        track_soc_instant_upload_position_status();
        track_soc_send((void*)9);
    }
    else if (flag == 3)
    {
        //避免不满足打包条件，熄火后一直看不到定位数据的问题。
        LOGD(L_GPS, L_V5, "!!!!!!!!!熄火强制发定位包!!!!!!!");
        track_soc_instant_upload_position_status();
        track_soc_send((void*)9);
    }
    else if (flag == 99)
    {
        //定时器，屏蔽虚假的ACC变化带来的数据包上传。
        track_start_timer(TRACK_CUST_ACCCHANGE_INSTANT_CONTRACT_TIMER_ID, 10 * 1000, track_cust_instant_contract_position_data, (void *) 3);
    }

}

#if defined(__GT370__)|| defined (__GT380__)
kal_uint32 g_last_position_tick=0;
/******************************************************************************
 *  Function    -  track_get_secs_diff_last_position_packet
 *
 *  Purpose     -  现在距离上次上报定位点相隔多少时间了
 *
 *  Description -  单位：秒
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-04-26,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint32 track_get_secs_diff_last_position_packet(void)
{
    kal_uint32 current=kal_get_systicks();
    return kal_ticks_to_secs(current-g_last_position_tick);
}

void track_gps_static_delay_send_position(void)
{
    track_gps_data_struct last_gps_data = {0};
    track_gps_data_struct *gpsLastPoint = NULL;
    applib_time_struct currTime = {0};

    gpsLastPoint = track_cust_backup_gps_data(2, NULL);
    memcpy(&last_gps_data, gpsLastPoint, sizeof(track_gps_data_struct));

    last_gps_data.gprmc.Speed = 0;

    last_gps_data.gprmc.status=track_cust_is_fix_when_gps_off();

    if (last_gps_data.gprmc.status==0)
    {
        track_fun_get_time(&currTime, KAL_TRUE, NULL);
        LOGD(L_APP, L_V5, "%0.4d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", currTime.nYear, currTime.nMonth, currTime.nDay, \
             currTime.nHour, currTime.nMin, currTime.nSec);

        last_gps_data.gprmc.Date_Time.nYear = currTime.nYear;
        last_gps_data.gprmc.Date_Time.nMonth = currTime.nMonth;
        last_gps_data.gprmc.Date_Time.nDay = currTime.nDay;
        last_gps_data.gprmc.Date_Time.nHour = currTime.nHour;
        last_gps_data.gprmc.Date_Time.nMin = currTime.nMin;
        last_gps_data.gprmc.Date_Time.nSec = currTime.nSec;
    }

    if (last_gps_data.gprmc.status==0)
    {
        track_cust_paket_position(&last_gps_data, track_drv_get_lbs_data(), TR_CUST_POSITION_STATIC_LAST_TIME, 0);//不定位，按当前时间
    }
    else
    {
        track_cust_paket_position(&last_gps_data, track_drv_get_lbs_data(), TR_CUST_POSITION_STATIC, 0);//定位时的时间
    }
}

#endif


#if defined (__NT33__)||defined (__NT31__)||defined(__MT200__)||defined(__GT370__)||defined (__GT380__)||defined (__ET310__)||defined (__NT37__)|| defined (__ET350__)
void track_gps_no_location_dup_updata(void)
{
    kal_uint16 interval_timer;
    kal_uint32 last_packet_time_diff;
    track_gps_data_struct last_gps_data = {0};
    static track_gps_data_struct last_tmp_gps_data = {0};

    track_gps_data_struct *gpsLastPoint = NULL;
    applib_time_struct currTime = {0};

    if (G_parameter.gpsDup == 0)
    {
        return;
    }

#if defined (__NT33__)
#if defined (__FUN_WITH_ACC__)
    if (track_cust_status_acc())
    {
        interval_timer = G_parameter.gps_work.Fti.interval_acc_on;
    }
    else
    {
        interval_timer = G_parameter.gps_work.Fti.interval_acc_off;
    }
#else
    interval_timer = G_parameter.gps_work.Fti.lbs_interval;
#endif

#elif defined(__GT370__)|| defined (__GT380__)

    if (track_cust_get_work_mode()==3)
    {
        return;
    }

    interval_timer = G_parameter.gps_work.Fti.interval_acc_off;
    last_packet_time_diff=track_get_secs_diff_last_position_packet();
    if (last_packet_time_diff<interval_timer)
    {
        tr_start_timer(TRACK_CUST_DETECT_NO_VIB_TIMER_ID, (1+interval_timer-last_packet_time_diff) * 1000, track_gps_no_location_dup_updata);
        return;
    }
#else
    if (track_cust_status_acc())
    {
        interval_timer = G_parameter.gps_work.Fti.interval_acc_on;
    }
    else
    {
        interval_timer = G_parameter.gps_work.Fti.interval_acc_off;
    }
#endif
    if (interval_timer == 0)
    {
        return;
    }
#if defined(__SERVER_IP_LINK__)
    if (track_cust_gps_status() >= GPS_STAUS_2D ||(track_cust_flymode_set(197)))
#else
    if (!track_soc_login_status() || track_cust_gps_status() >= GPS_STAUS_2D)
#endif
    {
        tr_start_timer(TRACK_CUST_DETECT_NO_VIB_TIMER_ID, interval_timer * 1000, track_gps_no_location_dup_updata);
        return;
    }

    gpsLastPoint = track_cust_backup_gps_data(2, NULL);
    memcpy(&last_gps_data, gpsLastPoint, sizeof(track_gps_data_struct));

    last_gps_data.gprmc.Speed = 0;

#if defined (__GT370__)||defined (__GT380__)
    last_gps_data.gprmc.status=track_cust_is_fix_when_gps_off();
    //不定位时卫星数也改为0
    if (last_gps_data.gprmc.status==0)
    {
        LOGD(L_APP, L_V5, "last gps not fix");

        last_gps_data.gpgga.sat=0;
        last_gps_data.bdgga.sat=0;
    }
#else
    last_gps_data.gprmc.status = 0;
#endif

    if (track_cust_is_upload_UTC_time())
    {
        track_fun_get_time(&currTime, KAL_TRUE, NULL);
    }
    else
    {
        track_fun_get_time(&currTime, KAL_FALSE, &G_parameter.zone);
    }

    LOGD(L_APP, L_V5, "%0.4d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", currTime.nYear, currTime.nMonth, currTime.nDay, \
         currTime.nHour, currTime.nMin, currTime.nSec);

    last_gps_data.gprmc.Date_Time.nYear = currTime.nYear;
    last_gps_data.gprmc.Date_Time.nMonth = currTime.nMonth;
    last_gps_data.gprmc.Date_Time.nDay = currTime.nDay;
    last_gps_data.gprmc.Date_Time.nHour = currTime.nHour;
    last_gps_data.gprmc.Date_Time.nMin = currTime.nMin;
    last_gps_data.gprmc.Date_Time.nSec = currTime.nSec;
    tr_start_timer(TRACK_CUST_DETECT_NO_VIB_TIMER_ID, interval_timer * 1000, track_gps_no_location_dup_updata);
    track_cust_paket_position(&last_gps_data, track_drv_get_lbs_data(), TR_CUST_POSITION_GPSDUP, 0);
}


/******************************************************************************
 *  Function    -  track_gps_location_dup_updata
 *
 *  Purpose     -  GPS定位但是数据理应被过滤的，补传
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * 	GPSDUP上报的，速度全部为0 -- 按需修改
 * ----------------------------------------
 * v1.0  , 2017-02-07,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_gps_location_dup_updata(track_gps_data_struct * gpsdata)
{
    track_gps_data_struct last_gps_data = {0};
    //static  track_gps_data_struct last_tmp_gps_data = {0};
    track_gps_data_struct *gpsLastPoint = NULL;
    applib_time_struct currTime = {0};

    if (G_parameter.gpsDup == 0)
    {
        return;
    }
    LOGD(L_APP, L_V5,"");

    if (G_parameter.gps_work.static_filter_sw)
    {
        gpsLastPoint = track_cust_backup_gps_data(2, NULL);
        memcpy(&last_gps_data, gpsLastPoint, sizeof(track_gps_data_struct));

        if (track_cust_is_upload_UTC_time())
        {
            track_fun_get_time(&currTime, KAL_TRUE, NULL);
        }
        else
        {
            track_fun_get_time(&currTime, KAL_FALSE, &G_parameter.zone);
        }

        LOGD(L_APP, L_V5, "%0.4d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", currTime.nYear, currTime.nMonth, currTime.nDay, \
             currTime.nHour, currTime.nMin, currTime.nSec);
        last_gps_data.gprmc.Date_Time.nYear = currTime.nYear;
        last_gps_data.gprmc.Date_Time.nMonth = currTime.nMonth;
        last_gps_data.gprmc.Date_Time.nDay = currTime.nDay;
        last_gps_data.gprmc.Date_Time.nHour = currTime.nHour;
        last_gps_data.gprmc.Date_Time.nMin = currTime.nMin;
        last_gps_data.gprmc.Date_Time.nSec = currTime.nSec;

#if 1
        last_gps_data.gprmc.Speed = 0;
#else
        if (memcmp(&last_tmp_gps_data, &last_gps_data, sizeof(track_gps_data_struct)) != 0)
        {
            memcpy(&last_tmp_gps_data, &last_gps_data, sizeof(track_gps_data_struct));
        }
        else
        {
            last_gps_data.gprmc.Speed = 0;
        }
#endif
        track_cust_paket_position(&last_gps_data, track_drv_get_lbs_data(), TR_CUST_POSITION_GPSDUP, 0);
    }
    else
    {
        gpsdata->gprmc.Speed=0;
        track_cust_paket_position(gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_GPSDUP, 0);
    }
}

void track_cust_gpsdup_switch(kal_uint8 gpsdup)
{
    if (gpsdup == 0)
    {
        if (track_is_timer_run(TRACK_CUST_DETECT_NO_VIB_TIMER_ID))
        {
            tr_stop_timer(TRACK_CUST_DETECT_NO_VIB_TIMER_ID);
        }
    }
    else
    {
        if (!track_is_timer_run(TRACK_CUST_DETECT_NO_VIB_TIMER_ID))
        {
            track_gps_no_location_dup_updata();
        }
    }
}

void track_cust_gpsdup_interval_update(void)
{
    if (G_parameter.gpsDup == 0)
    {
        return;
    }

    if (track_is_timer_run(TRACK_CUST_DETECT_NO_VIB_TIMER_ID))
    {
        tr_stop_timer(TRACK_CUST_DETECT_NO_VIB_TIMER_ID);
    }

    track_gps_no_location_dup_updata();
}

#else
void track_gps_no_location_dup_updata(void) {}
void track_gps_location_dup_updata(track_gps_data_struct * gpsdata) {}
void track_cust_gpsdup_switch(kal_uint8 gpsdup) {}
#endif

