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
static struct_msg_send g_msg = {0};
#if defined(__GERMANY__)
static char* msg_alarm_vibrates_en = "Vibrationssensor Alarm!";
static char* msg_alarm_vibrates_en1 = "Vibrationssensor Alarm!";
#else
static char* msg_alarm_vibrates_en = "Vibration Alarm!";
static char* msg_alarm_vibrates_en1 = "Vibration Alarm!Please pay attention!";
#endif

static kal_uint8 msg_alarm_vibrates_cn[10] =   // 震动报警！
{
    0x97, 0x07, 0x52, 0xA8, 0x62, 0xA5, 0x8B, 0x66, 0xFF, 0x01
};
static kal_uint8 msg_alarm_vibrates_cn1[18] =   // 震动报警！请关注!
{
    0x97, 0x07, 0x52, 0xA8, 0x62, 0xA5, 0x8B, 0x66, 0xFF, 0x01, 0x8B  , 0xF7  , 0x51  , 0x73 , 0x6C  , 0xE8  , 0xFF , 0x01

};
static track_gps_data_struct gps_data_buf = {0};
/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/
extern kal_uint8 g_alarm_gps_status;//报警记录报警期间GPS定位情况//0,不定位;1,定位
/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/

/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/
#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      本地函数
******************************************************************************/

/******************************************************************************
 *  Function    -  alarm_vibrates_valid_lid
 *
 *  Purpose     -  静默时间到
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void alarm_vibrates_valid_lid(void)
{
    LOGD(L_APP, L_V5, "振动报警的静默时间 结束");
#if defined(__GT500__) || defined(__ET200__)||defined(__ET130__)||defined(__NT23__)|| defined(__V20__) || defined(__ET310__) || defined(__MT200__)||defined (__ET320__)
    if(G_parameter.motor_lock.sw == 0 && (G_realtime_data.defense_mode == 1 || track_cust_status_acc() == 0))
#else
    if(G_parameter.motor_lock.sw == 0 && track_cust_status_acc() == 0)
#endif
    {
        if(track_cust_status_defences() == 1 && !track_is_timer_run(TRACK_CUST_STATUS_DEFENCES_TIMER_ID))
        {
            track_cust_status_defences_change((void*)2);
        }
        else if(track_cust_status_defences() == 2)
        {
            track_cust_module_alarm_vibrates((void *)2);
        }
    }
}

/******************************************************************************
 *  Function    -  track_cus_alarm_vibrates_overTime_sendsms
 *
 *  Purpose     -  发送振动报警短信
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cus_alarm_vibrates_overTime_sendsms(void)
{
    track_gps_data_struct *gpsLastPoint = NULL;

    LOGD(L_APP, L_V5, "%d,%d", G_parameter.vibrates_alarm.sw, G_parameter.vibrates_alarm.alarm_type);
    if(G_parameter.vibrates_alarm.sw && G_parameter.vibrates_alarm.alarm_type)
    {
        memset(&g_msg, 0, sizeof(struct_msg_send));
#if defined (__GT370__)|| defined (__GT380__) ||defined(__GT500__)||defined(__GT03F__) || defined(__GW100__) || defined(__GT740__)||defined(__GT300S__)||defined(__GT420D__)
        LOGD(L_APP, L_V6, "g_alarm_gps_status = %d", g_alarm_gps_status);
#if (!(defined(__ET200__) || defined(__V20__)|| defined(__GT740__)||defined(__GT420D__) || defined(__ET310__) || defined(__MT200__)||defined (__ET320__)))
        if(g_alarm_gps_status == 1  || track_cust_gps_status() > 2)
#endif
        {
            gpsLastPoint = track_cust_backup_gps_data(0, NULL);
            memcpy(&gps_data_buf, gpsLastPoint, sizeof(track_gps_data_struct));
            g_alarm_gps_status = 0;
        }
        if(gpsLastPoint != NULL && gpsLastPoint->gprmc.status == 1)
        {
            g_msg.addUrl = KAL_TRUE;
        }
        else
        {
            g_msg.addUrl = KAL_FALSE;
        }
#else
        g_msg.addUrl = KAL_TRUE;
#endif /* __GT300__ */
        g_msg.cm_type = CM_SMS;


        if(g_msg.addUrl == KAL_TRUE)
        {
            track_fun_strncpy(g_msg.msg_en, msg_alarm_vibrates_en, CM_PARAM_LONG_LEN_MAX);
            g_msg.msg_cn_len = 10;
            memcpy(g_msg.msg_cn, msg_alarm_vibrates_cn, g_msg.msg_cn_len);
        }
        else
        {
            track_fun_strncpy(g_msg.msg_en, msg_alarm_vibrates_en1, CM_PARAM_LONG_LEN_MAX);
            g_msg.msg_cn_len = 18;
            memcpy(g_msg.msg_cn, msg_alarm_vibrates_cn1, g_msg.msg_cn_len);
        }
        track_cust_module_alarm_msg_send(&g_msg, &gps_data_buf);

        if(G_parameter.vibrates_alarm.alarm_type == 2)
        {
            track_start_timer(TRACK_CUST_ALARM_alarm_vibrates_Call_TIMER, 60000, track_cust_make_call, (void*)1);
        }
        else
        {
            track_cust_module_alarm_lock(TR_CUST_ALARM_Vibration, 0);
        }
    }
    else
    {
        track_cust_module_alarm_lock(TR_CUST_ALARM_Vibration, 0);
    }
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_Vibration);
}

/******************************************************************************
 *  Function    -  alarm_vibrates_ind
 *
 *  Purpose     -  振动延时报警时间到，启动报警
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void alarm_vibrates_ind(void)
{
#if defined(__XCWS__)

    if(G_parameter.vibrates_alarm.sw)
    {
        track_gps_data_struct *gpsLastPoint = NULL;
        //char *alarm_str = {"vibrates alarm!"};
        LOGD(L_APP, L_V4, "震动报警未被取消，立即向服务器请求地址信息");

        gpsLastPoint = track_cust_backup_gps_data(0, NULL);

        if(G_parameter.vib == 1)
        {
            track_cust_alarm_trigger_req(ALARM_VIBRATION, ALARM_GPRS);
#if defined(__XCWS_TPATH__) && !defined(__KENAER__)
            gpsLastPoint = track_cust_gpsdata_alarm();
            track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_Vibration, KAL_TRUE, 1);

#endif
        }

        if(G_parameter.vibs == 1)
        {
            track_cust_alarm_trigger_req(ALARM_VIBRATION, ALARM_SMS);
        }

        if(G_parameter.vibrates_alarm.alarm_interval > 0)
        {
            tr_start_timer(TRACK_CUST_MODULE_ALARM_VIBRATES_TIMER_ID, G_parameter.vibrates_alarm.alarm_interval * 1000, alarm_vibrates_valid_lid);
        }
        else
        {
            alarm_vibrates_valid_lid();
        }
    }
    return;
#endif
    if(G_parameter.vibrates_alarm.sw)
    {
        track_gps_data_struct *gpsLastPoint = NULL;

        gpsLastPoint = track_cust_gpsdata_alarm();
        memcpy(&gps_data_buf, gpsLastPoint, sizeof(track_gps_data_struct));
        if(track_is_timer_run(TRACK_CUST_MODULE_ALARM_VIBRATES_TIMER_ID))
        {
            LOGD(L_APP, L_V4, "上一震动报警静默时间未到");
            /*if(track_nvram_alone_parameter_read()->gprson)
            {
                gpsLastPoint = track_cust_backup_gps_data(0, NULL);
                track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_Vibration, KAL_FALSE);
            }*/
            return;
        }
        else if(track_cust_module_alarm_lock(TR_CUST_ALARM_Vibration, 2) != 0)
        {
            LOGD(L_APP, L_V4, "上一震动报警请求未完成");
            if(G_parameter.vibrates_alarm.alarm_interval > 0)
            {
                LOGD(L_APP, L_V4, "启动振动报警的静默时间%d秒", G_parameter.vibrates_alarm.alarm_interval);
                tr_start_timer(
                    TRACK_CUST_MODULE_ALARM_VIBRATES_TIMER_ID,
                    G_parameter.vibrates_alarm.alarm_interval * 1000,
                    alarm_vibrates_valid_lid);
            }
            return;
        }
        else
        {
#if defined (__GT370__)|| defined (__GT380__) ||defined(__GT500__) ||defined(__GT03F__) || defined(__ET200__) || defined(__GW100__)|| defined(__V20__) || defined(__ET310__)||defined(__GT300S__)\
     || defined(__MT200__)||defined (__ET320__)
            g_alarm_gps_status = 0;
            //gpsLastPoint = track_cust_backup_gps_data(0, NULL);
#if defined(__V20__)
            track_cust_433_control(6);
#endif /* __V20__ */
            if(g_GT03D == KAL_TRUE)
            {
                if(track_cust_gps_status() > 2)
                {
                    g_alarm_gps_status = 1;
                }
                else
                {
                    track_cust_gps_work_req((void*)10);
                    track_cust_send_paket_16(2, gpsLastPoint, 0);
                }
                if(track_nvram_alone_parameter_read()->gprson)
                {
                    track_cust_alarm_type(TR_CUST_ALARM_Vibration);
                    if(g_alarm_gps_status == 1)
                    {
                        track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_Vibration, KAL_TRUE, 1);
                        tr_start_timer(
                            TRACK_CUST_ALARM_alarm_vibrates_OVERTIME_TIMER,
                            60 * 1000,
                            track_cus_alarm_vibrates_overTime_sendsms);
                    }
                    else
                    {
                        track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_Vibration, KAL_TRUE, 1);
                        tr_start_timer(
                            TRACK_CUST_ALARM_alarm_vibrates_OVERTIME_TIMER,
                            5 * 60 * 1000,
                            track_cus_alarm_vibrates_overTime_sendsms);
                    }
                }
            }
            else
#endif
#if defined (__XYBB__)
            {
                g_alarm_gps_status = 0;
                gpsLastPoint = track_cust_backup_gps_data(0, NULL);
                track_cust_gps_work_req((void*)10);
                track_cust_alarm_type(TR_CUST_ALARM_Vibration);
                track_cust_paket_0200(gpsLastPoint,track_drv_get_lbs_data());
                tr_start_timer(
                    TRACK_CUST_ALARM_alarm_vibrates_OVERTIME_TIMER,
                    60 * 1000,
                    track_cus_alarm_vibrates_overTime_sendsms);
            }

#else`
            {
                if(track_cust_gps_status() > 2)
                {
                    g_alarm_gps_status = 1;
                }
                if(track_nvram_alone_parameter_read()->gprson)
                {
                    track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_Vibration, KAL_TRUE, 1);
                    tr_start_timer(
                        TRACK_CUST_ALARM_alarm_vibrates_OVERTIME_TIMER,
                        2 * 60 * 1000,  /* 这里预留时间是留给终端发送经纬度给平台，等待平台返回对应的中文地址，短信转发给用户 */
                        track_cus_alarm_vibrates_overTime_sendsms);
                }

            }
#endif /* __XYBB__ */
            if(G_parameter.vibrates_alarm.alarm_interval > 0)
            {
                LOGD(L_APP, L_V4, "启动振动报警的静默时间%d秒", G_parameter.vibrates_alarm.alarm_interval);
                tr_start_timer(
                    TRACK_CUST_MODULE_ALARM_VIBRATES_TIMER_ID,
                    G_parameter.vibrates_alarm.alarm_interval * 1000,
                    alarm_vibrates_valid_lid);
            }
        }
        track_cust_module_alarm_lock(TR_CUST_ALARM_Vibration, 1);
        track_cust_led_alarm();
        if(G_parameter.vibrates_alarm.alarm_type > 0)
        {
            if(G_parameter.vibrates_alarm.alarm_type == 3)
            {
                tr_stop_timer(TRACK_CUST_ALARM_alarm_vibrates_OVERTIME_TIMER);
#if defined (__XYBB__)
#else
                track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_Vibration);
#endif /* __XYBB__ */
                track_start_timer(TRACK_CUST_ALARM_alarm_vibrates_Call_TIMER, 10 * 1000, track_cust_make_call, (void*)1);
            }
            else
#if defined (__GT300__) && defined(__LANG_ENGLISH__)
                if(track_nvram_alone_parameter_read()->gprson == 0)
#else
                if(G_parameter.lang == 0 || track_nvram_alone_parameter_read()->gprson == 0)
#endif /* __GT300__ */
                {
                    tr_stop_timer(TRACK_CUST_ALARM_alarm_vibrates_OVERTIME_TIMER);
                    track_os_intoTaskMsgQueue(track_cus_alarm_vibrates_overTime_sendsms);
                }
        }
        else
        {
            track_cust_module_alarm_lock(TR_CUST_ALARM_Vibration, 0);
        }
    }
    if(!track_is_timer_run(TRACK_CUST_MODULE_ALARM_VIBRATES_TIMER_ID))
    {
        alarm_vibrates_valid_lid();
    }
}

/*****************************************************************************
*  Global Functions                     全局函数
******************************************************************************/


/******************************************************************************
 *  Function    -  track_cust_module_alarm_defences
 *
 *  Purpose     -  设防状态变化
 *
 *  Description -  defences   0：撤防，1：预设防，2：设防
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 27-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_alarm_defences(kal_uint8 defences)
{
    switch(defences)
    {
        case 0:
            LOGD(L_APP, L_V4, "撤防");
            track_cust_module_alarm_vibrates((void *)3);
            tr_stop_timer(TRACK_CUST_MODULE_ALARM_VIBRATES_DELAY_TIMER_ID);
            tr_stop_timer(TRACK_CUST_MODULE_ALARM_VIBRATES_TIMER_ID);  // 撤防同时也撤销静默时间计时
            if(G_realtime_data.defences_status != 1)
            {
                G_realtime_data.defences_status = 1;
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
            }
            break;

        case 1:
            LOGD(L_APP, L_V4, "预设防");
            if(G_realtime_data.defences_status != 2)
            {
                G_realtime_data.defences_status = 2;
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
            }
            break;

        case 2:
            LOGD(L_APP, L_V4, "设防");
            if(G_realtime_data.defences_status != 3)
            {
                G_realtime_data.defences_status = 3;
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
            }
            track_cust_module_alarm_vibrates((void *)2);
            break;
    }
#if defined(__GT740__)|| defined(__XYBB__)||defined(__GT370__)||defined(__GT380__)||defined(__NT33__)||defined(__NT37__)||defined(__GT420D__)
//nc
#else
    track_cust_paket_9404_handle();
#endif
}

/******************************************************************************
 *  Function    -  track_cust_module_alarm_vibrates
 *
 *  Purpose     -  检测到震动
 *
 *  Description -  mode=1：检测到震动
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 27-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_alarm_vibrates(void *arg)
{
    static kal_uint16 vibratesCount = 0;
    static kal_bool valid = KAL_FALSE;
    static U32 sampling_interval_tick = 0;
    U32 mode = (kal_uint32)arg;
    U32 tick = OTA_kal_get_systicks();
    U8 defences;
    if((G_parameter.motor_lock.sw == 0 && G_parameter.vibrates_alarm.sw == 0) || G_parameter.vibrates_alarm.detection_count == 0)
    {
        return;
    }
    defences = track_cust_status_defences();
    if(mode == 1)
    {
        LOGD(L_SENSOR, L_V5, "mode:%d, valid:%d, sampling_interval_tick:%d/%d, defences:%d, sw:%d, alarm_delay_time:%d",
             mode, valid, tick, sampling_interval_tick, defences, G_parameter.vibrates_alarm.sw, G_parameter.vibrates_alarm.alarm_delay_time);
    }
    else
    {
        LOGD(L_APP, L_V6, "mode:%d, valid:%d, sampling_interval_tick:%d/%d, defences:%d, sw:%d, alarm_delay_time:%d",
             mode, valid, tick, sampling_interval_tick, defences, G_parameter.vibrates_alarm.sw, G_parameter.vibrates_alarm.alarm_delay_time);
    }
    if(mode == 1 && valid && tick > sampling_interval_tick && defences == 2)
    {
        vibratesCount++;
        sampling_interval_tick = tick + OTA_kal_secs_to_ticks(G_parameter.vibrates_alarm.sampling_interval) - KAL_TICKS_100_MSEC * 2;
        LOGD(L_SENSOR, L_V6, "vibratesCount:%d / %d",
             vibratesCount, G_parameter.vibrates_alarm.detection_count);
        if(vibratesCount == 1)
        {
            track_start_timer(
                TRACK_CUST_MODULE_ALARM_VIBRATES_CHECK_TIMER_ID,
                G_parameter.vibrates_alarm.detection_time * 1000,
                track_cust_module_alarm_vibrates, (void*)2);
        }
        if(vibratesCount == G_parameter.vibrates_alarm.detection_count)
        {
            LOGD(L_APP, L_V4, "满足震动条件");
            valid = KAL_FALSE;
            vibratesCount = 0;
#if defined(__FLY_MODE__)

#if defined(__MT200__)
//MT200 振动不唤醒GPS也不退出飞行模式，为了使震动报警退出飞行模式流程能正常运行，这里调用下32
            track_cust_flymode_set(32);
            track_cust_flymode_set(31);
#else
            track_cust_flymode_set(30);
#endif/*__MT200__*/

#endif/*__FLY_MODE__*/
#if defined(__XCWS__)
            track_cust_work_mode_ind(w_sensor_smg);
#endif
            if(G_parameter.vibrates_alarm.sw
                    && G_parameter.vibrates_alarm.alarm_delay_time > 0
                    && !track_is_timer_run(TRACK_CUST_MODULE_ALARM_VIBRATES_DELAY_TIMER_ID))
            {
                if(track_is_timer_run(TRACK_CUST_MODULE_ALARM_VIBRATES_TIMER_ID))
                {
                    LOGD(L_APP, L_V4, "上一震动报警静默时间未到");
                    return;
                }
                else if(track_cust_module_alarm_lock(TR_CUST_ALARM_Vibration, 2) != 0)
                {
                    LOGD(L_APP, L_V4, "上一震动报警请求未完成");
                    if(G_parameter.vibrates_alarm.alarm_interval > 0)
                    {
                        LOGD(L_APP, L_V4, "启动振动报警的静默时间%d秒", G_parameter.vibrates_alarm.alarm_interval);
                        tr_start_timer(
                            TRACK_CUST_MODULE_ALARM_VIBRATES_TIMER_ID,
                            G_parameter.vibrates_alarm.alarm_interval * 1000,
                            alarm_vibrates_valid_lid);
                    }
                    return;
                }
                else
                {
                    LOGD(L_APP, L_V4, "启动%d秒延时报警", G_parameter.vibrates_alarm.alarm_delay_time);
#if defined(__GT500__) || defined(__ET200__)||defined(__ET130__)||defined(__NT23__)|| defined(__V20__) || defined(__ET310__) ||defined (__ET320__)|| defined(__MT200__)
                    if(G_realtime_data.defense_mode == 0)
                    {
                        tr_start_timer(
                            TRACK_CUST_MODULE_ALARM_VIBRATES_DELAY_TIMER_ID,
                            G_parameter.vibrates_alarm.alarm_delay_time * 1000,
                            alarm_vibrates_ind);
                    }
                    else
                    {
                        alarm_vibrates_ind();
                    }
#else
                    tr_start_timer(
                        TRACK_CUST_MODULE_ALARM_VIBRATES_DELAY_TIMER_ID,
                        G_parameter.vibrates_alarm.alarm_delay_time * 1000,
                        alarm_vibrates_ind);
#endif
#if !defined(__XCWS__)
                    if(G_parameter.motor_lock.sw == 0)
                    {
                        track_cust_status_defences_change((void*)1);
                    }
#endif
                }
            }
        }
    }
    else if(mode == 2 && track_cust_status_defences() == 2)  // 重新做检测需要处于设防的前提
    {
        valid = KAL_TRUE;
        vibratesCount = 0;
    }
    else if(mode == 3)
    {
        valid = KAL_FALSE;
        vibratesCount = 0;
    }
}

/******************************************************************************
 *  Function    -  track_cust_defences_cmd_update
 *
 *  Purpose     -  指令更新报警状态
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_defences_cmd_update(void)
{
    if(G_realtime_data.defense_mode == 0)   /* 自动设防模式 */
    {
        kal_uint8 defences = track_cust_status_defences();
        kal_uint8 acc_status = track_cust_status_acc();
        LOGD(L_APP, L_V5, "motor_lock.sw:%d, vibrates_alarm.sw:%d, defences=%d, acc_status:%d",
             G_parameter.motor_lock.sw, G_parameter.vibrates_alarm.sw, defences, acc_status);
        if(acc_status || (G_parameter.motor_lock.sw == 0 && G_parameter.vibrates_alarm.sw == 0 && defences != 0))
        {
            track_cust_status_defences_change((void*)0);
        }
        else if(acc_status == 0 && (G_parameter.motor_lock.sw == 1 || G_parameter.vibrates_alarm.sw == 1) && defences == 0)
        {
            track_cust_status_defences_change((void*)1);
            track_start_timer(
                TRACK_CUST_STATUS_DEFENCES_TIMER_ID,
                G_parameter.defense_time * 1000,
                track_cust_status_defences_change, (void*)2);
        }
    }
    else
    {
        /* 手动设防模式 */

    }
}

/******************************************************************************
 *  Function    -  track_cust_shocks_quiet_time_update
 *
 *  Purpose     -  指令更新报警状态2
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_shocks_quiet_time_update(U32 alarm_interval)
{
    if(track_is_timer_run(TRACK_CUST_MODULE_ALARM_VIBRATES_TIMER_ID))
    {
        LOGD(L_APP, L_V5, "");
        tr_start_timer(
            TRACK_CUST_MODULE_ALARM_VIBRATES_TIMER_ID,
            alarm_interval * 1000,
            alarm_vibrates_valid_lid);
    }
}

#if defined(__BCA__)
/******************************************************************************
 *  Function    -  track_cust_shocks_lbs_update
 *
 *  Purpose     -  LBS多基站包上传包(伴车安防屏蔽GPS)
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 10-11-2015,  Cjj  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_shocks_lbs_update(void *arg)
{
    static kal_uint16 ShockCount = 0;
    LBS_Multi_Cell_Data_Struct *lbs;
    kal_uint32 mode = (kal_uint32)arg;

    LOGD(L_APP, L_V5, "%d", mode);
    lbs = track_drv_get_lbs_data();
    if(mode == 1)
    {
        ShockCount++;
    }
    if(ShockCount == 1)//检测到首次震动，开启10秒定时器，过滤无效震动。
    {
        track_start_timer(TRACK_CUST_SHOCKS_UPLBS_CHECK_TIMER_ID, 10000, track_cust_shocks_lbs_update, (void*)3);
    }
    else if(ShockCount == 3)
    {
        ShockCount = 2;
        track_cust_shocks_lbs_update((void *)2);
    }

    if(mode == 2)
    {
        if(track_cust_sensor_delay_valid_acc() == KAL_FALSE || track_cust_gps_status() >= GPS_STAUS_2D)
        {
            //已定位||已经不在震动有效时间内，停止LBS持续上传
            track_stop_timer(TRACK_CUST_SHOCKS_UPLBS_TIMER_ID);
            ShockCount = 0;
            return;
        }
        if(track_is_timer_run(TRACK_CUST_SHOCKS_UPLBS_TIMER_ID) || track_is_timer_run(TRACK_CUST_LBS_UPDATE_L_TIMER_ID))
        {
            //开机会开启两分钟定时器，期间若未定位，则自动上传LBS定位包
            //已经开启了LBS持续上传
            return;
        }

        if(track_cust_gps_status() > GPS_STAUS_OFF)
        {
            track_start_timer(TRACK_CUST_SHOCKS_UPLBS_TIMER_ID, 30000, track_cust_shocks_lbs_update, (void*)99);
        }
    }
    else if(mode == 3) //重新检测
    {
        ShockCount = 0;
    }
    else if(mode == 98)
    {
        track_cust_paket_18(lbs, 0);//伴车安定制，开机不定位两分钟，则发一条LBS包
    }
    else if(mode == 99) //发送包
    {
        track_cust_paket_18(lbs, 0);
        track_cust_shocks_lbs_update((void *)2);
    }
}
#endif

