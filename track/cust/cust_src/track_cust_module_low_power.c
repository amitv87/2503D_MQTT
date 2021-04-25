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

/*****************************************************************************
*  Typedef  Enum                        枚举定义
*****************************************************************************/

/*****************************************************************************
*  Struct                               数据结构定义
******************************************************************************/

/*****************************************************************************
*  Local variable                       局部变量
*****************************************************************************/
#if defined(__GERMANY__)
static char *str_battery_low_en = "Achtung - interne Batterie ist niedrig, bitte Laden !";
#elif defined(__SPANISH__)
static char *str_battery_low_en = "Alerta! Nivel de bateria bajo. Por favor cargarla.";
#else
#if defined(__MT200__)
static char *str_battery_low_en = "Attention!The built-in battery is low!";
#else
static char *str_battery_low_en = "Attention! The built-in battery is low, please change!";
#endif/*__MT200__*/
#endif
#if defined(__MT200__)
static char str_battery_low_cn[23] =   // 注意！终端电池电量低!
{
    0x6C, 0xE8, 0x61, 0x0F, 0xFF, 0x01, 0x7E, 0xC8, 0x7A, 0xEF, 0x75, 0x35, 0x6C, 0x60, 0x75, 0x35,
    0x91, 0xCF, 0x4F, 0x4E, 0xFF, 0x01
};
#else
static char str_battery_low_cn[] =   // 注意！终端电池电量低，请及时更换新电池。
{
    0x6C, 0xE8, 0x61, 0x0F, 0xFF, 0x01, 0x7E, 0xC8, 0x7A, 0xEF, 0x75, 0x35, 0x6C, 0x60, 0x75, 0x35,
    0x91, 0xCF, 0x4F, 0x4E, 0xFF, 0x01, 0x8B, 0xF7, 0x53, 0xCA, 0x65, 0xF6, 0x51, 0x45, 0x75, 0x35, 0x30, 0x02
};
#endif
#if !defined(__GT740__)||!defined(__GT420D__)
static char *str_battery_low_will_turn_off_en = "Attention! The battery is low, the device shutdown!";
static char str_battery_low_will_turn_off_cn[34] =   // 注意！终端电池电量过低，自动关机！
{
    0x6C, 0xE8, 0x61, 0x0F, 0xFF, 0x01, 0x7E, 0xC8, 0x7A, 0xEF, 0x75, 0x35, 0x6C, 0x60, 0x75, 0x35,
    0x91, 0xCF, 0x8F, 0xC7, 0x4F, 0x4E, 0xFF, 0x0C, 0x81, 0xEA, 0x52, 0xA8, 0x51, 0x73, 0x67, 0x3A,
    0xFF, 0x01
};

static char *str_external_power_low_en = "Attention! Vehicle's battery power is low. Please charge!";
static char str_external_power_low_cn[32] =   // 注意！车辆电瓶低电，请及时充电！
{
    0x6C, 0xE8, 0x61, 0x0F, 0xFF, 0x01, 0x8F, 0x66, 0x8F, 0x86, 0x75, 0x35, 0x74, 0xF6, 0x4F, 0x4E,
    0x75, 0x35, 0xFF, 0x0C, 0x8B, 0xF7, 0x53, 0xCA, 0x65, 0xF6, 0x51, 0x45, 0x75, 0x35, 0xFF, 0x01
};

static char *str_disconnect_the_external_power_en = "Vehicle's battery power is low, the low battery protection has been activated, please charge!";
static char str_disconnect_the_external_power_cn[42] =   // 注意！车辆电瓶低电保护已动作，请及时充电！
{
    0x6C, 0xE8, 0x61, 0x0F, 0xFF, 0x01, 0x8F, 0x66, 0x8F, 0x86, 0x75, 0x35, 0x74, 0xF6, 0x4F, 0x4E,
    0x75, 0x35, 0x4F, 0xDD, 0x62, 0xA4, 0x5D, 0xF2, 0x52, 0xA8, 0x4F, 0x5C, 0xFF, 0x0C, 0x8B, 0xF7,
    0x53, 0xCA, 0x65, 0xF6, 0x51, 0x45, 0x75, 0x35, 0xFF, 0x01
};
#endif
U32 tick_for_disconnect_ext_power = 0;
/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/
track_enum_sms_staus sms_staus = SMS_NORMAL_MODE; //记录SMS的状态，正常模式or 飞行模式

/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/

/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/
#if defined (__XYBB__)
extern void track_cust_paket_0200(track_gps_data_struct * gd, LBS_Multi_Cell_Data_Struct * lbs);
#endif /* __XYBB__ */
#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      本地函数
******************************************************************************/

/******************************************************************************
 *  Function    -  lowbattery_sendsms
 *
 *  Purpose     -  发送低电报警短信
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void lowbattery_sendsms(void)
{
#if defined(__XCWS__)
    if (G_parameter.pof == 1)
    {
        track_cust_alarm_trigger_req(ALARM_BATTERY_LOW, ALARM_GPRS);
    }
    if (G_parameter.pofs == 1)
    {
        track_cust_alarm_trigger_req(ALARM_BATTERY_LOW, ALARM_SMS);
    }
    return;
#endif
    if (G_parameter.low_power_alarm.alarm_type >= 1 && G_parameter.low_power_alarm.alarm_type != 3)
    {
        struct_msg_send g_msg = {0};
        g_msg.addUrl = KAL_FALSE;
        g_msg.cm_type = CM_SMS;
        snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_battery_low_en);
#if defined(__MT200__)
        g_msg.msg_cn_len = 23;
#else
        g_msg.msg_cn_len = sizeof(str_battery_low_cn);
#endif
        memcpy(g_msg.msg_cn, str_battery_low_cn, g_msg.msg_cn_len);
        track_cust_module_alarm_msg_send(&g_msg, NULL);
    }
    if (G_parameter.low_power_alarm.alarm_type == 2 || G_parameter.low_power_alarm.alarm_type == 3)
    {
        track_start_timer(TRACK_CUST_ALARM_LowBattery_Call_TIMER, 60000, track_cust_make_call, (void*)1);
    }
#if defined (__XYBB__)

#else
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_LowBattery);
#endif /* __XYBB__ */
}


static void poweroff_sendsms(void)
{
#if !defined(__GT740__)||!defined(__GT420D__)
    LOGD(L_APP, L_V5, "======");
    if (G_parameter.low_power_alarm.alarm_type >= 1)
    {
        struct_msg_send g_msg = {0};
        g_msg.addUrl = KAL_FALSE;
        g_msg.cm_type = CM_SMS;
        snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_battery_low_will_turn_off_en);
        g_msg.msg_cn_len = 34;
        memcpy(g_msg.msg_cn, str_battery_low_will_turn_off_cn, g_msg.msg_cn_len);
        track_cust_module_alarm_msg_send(&g_msg, NULL);
    }
#endif
}
/******************************************************************************
 *  Function    -  lowbattery
 *
 *  Purpose     -  超时处理
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void lowbattery(void *arg)
{
    kal_uint32 par = (kal_uint32)arg;
    LOGD(L_APP, L_V5, "%d", par);
    if (!track_soc_login_status())
    {
        track_project_save_switch(1, sw_lowpower_alm);
        return;
    }
    if (par == 1)
    {
        // 电压恢复 >= 3.8V
        G_realtime_data.lowBattery = 0;
        G_realtime_data.poweroff = 0;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    }
    else if (par == 2)
    {
        track_gps_data_struct *gpsLastPoint = NULL;
        gpsLastPoint = track_cust_gpsdata_alarm();
#if defined(__GT740__)||defined (__GT370__)|| defined (__GT380__)
        if (gpsLastPoint->gprmc.status==1)
        {
            track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_LowBattery, KAL_TRUE, 0);
        }
        else
        {
            track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_LowBattery, KAL_TRUE, 0);
        }

#elif   defined(__GT420D__)
        if (G_realtime_data.lowBatteryFlag==0)
        {
            if (gpsLastPoint->gprmc.status==1)
            {
                track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_LowBattery, KAL_TRUE, 0);
                G_realtime_data.lowBatteryFlag=1;
                // LOGS( "电量发送的状态为%d", G_realtime_data.lowBatteryFlag);
            }
            else
            {
                track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_LowBattery, KAL_TRUE, 0);
                G_realtime_data.lowBatteryFlag=1;
                //  LOGS( "19电量发送的状态为%d", G_realtime_data.lowBatteryFlag);
            }
        }
#elif defined(__XYBB__)
        track_cust_alarm_type(TR_CUST_ALARM_LowBattery);
        track_cust_paket_0200(gpsLastPoint,track_drv_get_lbs_data());
#elif !defined(__XCWS__)
        track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_LowBattery, KAL_TRUE, 0);
#elif defined(__XCWS_TPATH__) && !defined(__KENAER__)
        track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_LowBattery, KAL_TRUE, 0);
#endif
        //tr_start_timer(TRACK_CUST_ALARM_LowBattery_OVERTIME_TIMER,  TRACK_ALARM_DELAY_TIME_SECONDS, track_cus_lowbattery_overTime_sendsms);
        track_os_intoTaskMsgQueue(lowbattery_sendsms);

        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

    }
}

/*****************************************************************************
*  Global Functions                     全局函数
******************************************************************************/

/******************************************************************************
 *  Function    -  track_cust_module_low_power
 *
 *  Purpose     -  低电报警判断，延时起电压防抖的效果
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_low_power(kal_bool batteryStatus)
{
    if (G_parameter.low_power_alarm.sw == 0)
    {
        return;
    }
    if (batteryStatus == 0 && G_realtime_data.lowBattery != 0)
    {
        // 电池电压 >= 3.8V
        LOGD(L_APP, L_V5, "flag=%d, batteryStatus=%d, sw=%d, alarm_type=%d, battery_volt=%d",
             G_realtime_data.lowBattery,
             batteryStatus,
             G_parameter.low_power_alarm.sw,
             G_parameter.low_power_alarm.alarm_type,
             track_cust_get_battery_volt());
        G_realtime_data.lowBattery = 0;
        track_start_timer(TRACK_CUST_ALARM_LowBattery_SHAKE_TIMER, 30000, lowbattery, (void *)1);
#if defined (__XYBB__)
        track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_LowBattery);
#endif /* __XYBB__ */
    }
    else if (batteryStatus && G_realtime_data.lowBattery == 0)
    {
        // 电池低压
        LOGD(L_APP, L_V5, "flag=%d, batteryStatus=%d, sw=%d, alarm_type=%d, battery_volt=%d",
             G_realtime_data.lowBattery,
             batteryStatus,
             G_parameter.low_power_alarm.sw,
             G_parameter.low_power_alarm.alarm_type,
             track_cust_get_battery_volt());
#if defined(__FLY_MODE__)
        track_cust_flymode_set(30);
#endif
        G_realtime_data.lowBattery = 1;
#if defined(__GT740__) || defined (__GT370__)|| defined (__GT380__)||defined(__GT420D__)
        track_start_timer(TRACK_CUST_ALARM_LowBattery_SHAKE_TIMER, 10000, lowbattery, (void *)2);
#else
        track_start_timer(TRACK_CUST_ALARM_LowBattery_SHAKE_TIMER, 30000, lowbattery, (void *)2);
#endif
    }
    else
    {
        LOGD(L_APP, L_V6, "flag=%d, batteryStatus=%d, sw=%d, alarm_type=%d, battery_volt=%d",
             G_realtime_data.lowBattery,
             batteryStatus,
             G_parameter.low_power_alarm.sw,
             G_parameter.low_power_alarm.alarm_type,
             track_cust_get_battery_volt());
    }
}

void track_cust_module_extbat_control(U8 sw)
{
    //LOGD(L_APP, L_V6, "%d, %d", sw, G_parameter.flycut);
#if defined (__GT380__)
    return;
#endif /* __GT380__ */

    if (sw == 1)
    {   // 触发外电保护
        track_gps_data_struct *gpsLastPoint = NULL;
        gpsLastPoint = track_cust_gpsdata_alarm();
#if defined(__FLY_MODE__)
        if (G_parameter.flycut)
        {
            track_cust_flymode_set(10);  // 延时进入飞行模式
        }
#endif

#if !(defined(__XCWS__) || defined(__GT740__)||defined(__GT420D__))
        if (G_realtime_data.disExtBattery != 1)
        {
#if defined(__FLY_MODE__)
            if (G_parameter.flycut)
                track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_Ext_LowBattery_Fly, KAL_TRUE, 1);
            else
#endif /* __FLY_MODE__ */
                track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_Ext_PowerFailure, KAL_TRUE, 1);
            if (G_parameter.power_extbatcut.alarm_type >= 1)
            {
                struct_msg_send g_msg = {0};
                g_msg.addUrl = KAL_FALSE;
                g_msg.cm_type = CM_SMS;
                snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_disconnect_the_external_power_en);
                g_msg.msg_cn_len = 42;
                memcpy(g_msg.msg_cn, str_disconnect_the_external_power_cn, g_msg.msg_cn_len);
                track_cust_module_alarm_msg_send(&g_msg, NULL);
            }
        }
#endif

        track_drv_gpio_disconnect_ext_power(1);
        if (G_realtime_data.disExtBattery != 1)
        {
            G_realtime_data.disExtBattery = 1;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        }
        tick_for_disconnect_ext_power = OTA_kal_get_systicks();
    }
    else
    {   // 解除外电保护
#if defined(__FLY_MODE__)
        track_cust_flymode_set(20);
#endif /* __FLY_MODE__ */
        if (G_realtime_data.disExtBattery != 0)
        {
            LOGD(L_APP, L_V7, "外电低电报警标志恢复:%d", G_realtime_data.lowExtBattery);
            G_realtime_data.disExtBattery = 0;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        }
        track_drv_gpio_disconnect_ext_power(0);
    }
}

U32 get_ms_from_last_disconnect_ext_power()
{
    U32 ms = OTA_kal_ticks_to_milli_secs(OTA_kal_get_systicks() - tick_for_disconnect_ext_power);
    LOGD(L_APP, L_V7, "%d", ms);
    return ms;
}

void track_cust_module_ext_vbat(kal_uint32 volt)
{
    static kal_uint32 last_tick = 0, last_tick2 = 0, last_tick3 = 0, last_tick4 = 0;
    kal_uint32 tick;
    char buf[320] = {0};
    U8 acc = track_cust_status_acc();

    tick = kal_get_systicks();
    LOGD(L_APP, L_V7, "外电 acc:%d, volt:%d", acc, volt);
    if (G_parameter.power_extbatalm.sw)
    {
        LOGD(L_APP, L_V7, "外电低电报警:%d, tick:%d(%d) sec:%d(%d), volt:%d(%d)",
             G_realtime_data.lowExtBattery,
             tick, last_tick, kal_ticks_to_secs(tick - last_tick), G_parameter.power_extbatalm.detection_time,
             volt, G_parameter.power_extbatalm.threshold * 100000);
        // 外电低电报警
        if (volt < G_parameter.power_extbatalm.threshold * 100000)
        {
            if (last_tick != 0 && volt > 5000000)
            {
                if (G_realtime_data.lowExtBattery == 0 && kal_ticks_to_secs(tick - last_tick) > G_parameter.power_extbatalm.detection_time)
                {
#if !(defined(__XCWS__) || defined(__GT740__)||defined(__GT420D__))
                    track_gps_data_struct *gpsLastPoint = NULL;
                    gpsLastPoint = track_cust_gpsdata_alarm();
                    track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_Ext_LowBattery, KAL_TRUE, 1);
                    if (G_parameter.power_extbatalm.alarm_type >= 1)
                    {
                        struct_msg_send g_msg = {0};
                        g_msg.addUrl = KAL_FALSE;
                        g_msg.cm_type = CM_SMS;
                        snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_external_power_low_en);
                        g_msg.msg_cn_len = 32;
                        memcpy(g_msg.msg_cn, str_external_power_low_cn, g_msg.msg_cn_len);
                        track_cust_module_alarm_msg_send(&g_msg, NULL);
                    }
#endif
                    //track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_LowBattery);
                    G_realtime_data.lowExtBattery = 1;
                    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                }
            }
            else
            {
                last_tick = tick;
            }
        }
        else
        {
            last_tick = 0;
        }
    }
    // 外电电压恢复
    if (volt > G_parameter.power_extbatalm.recover_threshold * 100000)
    {
        if (last_tick2 != 0)
        {
            if (G_realtime_data.lowExtBattery == 1 && kal_ticks_to_secs(tick - last_tick2) > G_parameter.power_extbatalm.detection_time)
            {
                G_realtime_data.lowExtBattery = 0;
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
            }
        }
        else
        {
            last_tick2 = tick;
        }
    }
    else
    {
        last_tick2 = 0;
    }
    // ----------------------
    if (acc == 1)
    {
        track_cust_module_extbat_control(0);
    }
    else
    {
        // 外电恢复导通
        if (volt > G_parameter.power_extbatcut.recover_threshold * 100000)
        {
#if defined(__MT200__)
            last_tick3 =0;
#endif
            if (last_tick4 == 0)
            {
                last_tick4 = tick;
            }
            else if (kal_ticks_to_secs(tick - last_tick4) > G_parameter.power_extbatcut.detection_time)
            {
#if !defined(__MT200__) ||defined(__CUST_ACC_BY_MCU__)
                track_cust_module_extbat_control(0);
#endif
            }
        }
        else
        {
            last_tick4 = 0;
            if (!G_parameter.power_extbatcut.sw) return;
            LOGD(L_APP, L_V7, "外电低电断开标记:%d, tick:%d(%d) sec:%d(%d), volt:%d(%d)",
                 G_realtime_data.disExtBattery,
                 tick, last_tick3, kal_ticks_to_secs(tick - last_tick3), G_parameter.power_extbatcut.detection_time,
                 volt, G_parameter.power_extbatcut.threshold * 100000);
            // 外电低电断开
            if (volt < G_parameter.power_extbatcut.threshold * 100000)
            {
#if defined(__MT200__)
                if (volt < 5000000)
                {
                    //修复主动断开和接上外电有几率出现，tick - last_tick3 大于detection_time。
                    last_tick3 =0;
                }
#endif
                if (last_tick3 == 0)
                {
                    last_tick3 = tick;
                }
                else if (volt > 5000000 && kal_ticks_to_secs(tick - last_tick3) > G_parameter.power_extbatcut.detection_time)
                {
                    //LOGD(L_APP, L_V6, "十秒一次才正常");
                    track_cust_module_extbat_control(1);
                    last_tick3 = tick;//不重置last_tick3的话，会出现一秒就来一次，MT200的是要先拉高再拉低，这时候会出现概率性重启，概率还不低。
                }
                LOGD(L_APP, L_V7, "tick:%d,last_tick3:%d; sec:%d(%d)", tick, last_tick3, kal_ticks_to_secs(tick - last_tick3), G_parameter.power_extbatcut.detection_time);
            }
            else
            {
                last_tick3 = 0;
            }
        }
    }
}

void track_cust_module_set_power_saving_mode(kal_int8 status)
{
    if (G_realtime_data.power_saving_mode != status)
    {
        if (status == 0)
        {
            LOGD(L_DRV, L_V5, "@@1 status =%d", status);
            G_realtime_data.power_saving_mode = 0;
        }
        else
        {
            LOGD(L_DRV, L_V5, "@@2 status =%d", status);
            G_realtime_data.power_saving_mode = 1;
        }
        LOGD(L_DRV, L_V5, "status:%d, power_saving_mode:%d ", status, G_realtime_data.power_saving_mode);
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        if (G_realtime_data.power_saving_mode == 1)
        {
            track_cust_led_sleep(2);
            track_cust_gps_off_req((void*)5);
        }
        else
        {
            track_cust_led_sleep(1);
            if (G_parameter.sensor_gps_work == 0)
            {
                track_cust_gps_work_req((void*)3);
            }
            else if (track_cust_status_acc() == 0)
            {
                track_cust_gps_off_req((void*)5);
            }
        }

    }
    else
    {
        LOGD(L_DRV, L_V5, "已经处于该状态status:%d, power_saving_mode:%d;led_sleep=%d ", status, G_realtime_data.power_saving_mode, track_drv_led_sleep_status());
        if (G_realtime_data.power_saving_mode == 1 && track_drv_led_sleep_status() == 1)
        {
            track_cust_led_sleep(2);
        }
    }

}
void track_cust_module_power_off(void)
{
    U8 buf[100] = {0};
    extern void track_drv_sys_power_off_req(void);

    LOGD(L_APP, L_V5, "\r\n======");

    if (track_cust_status_charger() == 0)
    {
        track_drv_watch_dog_close(1);
#if defined(__GT300S__)||defined(__GT370__)|| defined (__GT380__)
        track_soc_bakup_data((void*)3);
        sprintf(buf, "AT^GT_CM=03#");
        track_spi_write(buf, strlen(buf));
        tr_start_timer(TRACK_POWER_RESET_TIMER_ID, 1000, track_drv_sys_power_off_req);
#endif /* __GT300S__ */
    }
    LOGD(L_APP, L_V5, "======\r\n\r\n");

}

/******************************************************************************
 *  Function    -  track_cust_module_power_off_ready
 *
 *  Purpose     -
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 04-09-2013,  ZengPing && Liujw  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_module_power_off_ready(void)
{
    static kal_uint8 flag = 0;
#if defined( __GT03F__) || defined(__GT300__) || defined(__GW100__)||defined(__GT300S__)|| defined(__GT370__)|| defined (__GT380__)

    LOGD(L_APP, L_V5, "======sw =%d  poweroff =%d", G_parameter.low_power_alarm.sw, G_realtime_data.poweroff);
    if (flag == 0)
    {
        flag = 1;
        if (G_realtime_data.lowBattery == 1)
        {
            if (G_parameter.low_power_alarm.sw == 1)
            {
                if (G_realtime_data.poweroff == 0)
                {
                    //send sms
                    poweroff_sendsms();
                    G_realtime_data.poweroff = 1;
                    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                }
            }
        }
        //power off
        if (FALSE == track_is_timer_run(TRACK_DALY_SHUTDOWN_TIMER_ID))
        {
            tr_start_timer(TRACK_DALY_SHUTDOWN_TIMER_ID, 90000, track_cust_module_power_off);
        }
        //track_drv_watch_dog_close();
    }

#endif /* __GT03F__ */
}

#if defined(__FLY_MODE__)
/******************************************************************************
 *  Function    -  track_cust_module_low_power_fly_change
 *
 *  Purpose     -  飞行模式切换
 *
 *  Description -

 * shutdown :
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-6-2015,  chenjiajun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_module_low_power_fly_change(U32 port)
{
    static kal_uint8 time_reconn  = 0;
    if (G_parameter.flycut == 0) return;
    LOGD(L_APP, L_V6, "port:%d, sms_staus:%d", port, sms_staus);
    if (port >= 0 && port < 50 && sms_staus != SMS_AIRPLANE_MODE && !(track_is_testmode() || track_is_autotestmode()))
    {
        time_reconn = 0;
        sms_staus = SMS_AIRPLANE_MODE;
        track_stop_timer(TRACK_CUST_ALARM_LowBattery_Fly_TIMER);
        //track_cust_gps_off_req((void*)5);
        track_cust_gps_control(0);
        //track_drv_sensor_off();
        track_cust_led_sleep(2);
        track_drv_sys_cfun(4);
        track_soc_disconnect();
        track_cust_server2_disconnect();
        //track_os_intoTaskMsgQueue(track_drv_ext_vbat_measure_modify_parameters);
        track_cust_make_call((void*)23);
        track_cust_call_hangup();
        track_sms_reset();
        LOGD(L_APP, L_V4, "!!! 进入飞行模式!!!");
    }
    else if (port >= 50 && port < 90 && sms_staus != SMS_NORMAL_MODE)
    {
        track_stop_timer(TRACK_CUST_ALARM_LowBattery_Fly_TIMER);
        time_reconn = 0;
        sms_staus = SMS_NORMAL_MODE;
        //track_cust_gps_work_req((void *)2);    // gps on
        //if(G_parameter.sensor_gps_work == 0) track_cust_gps_control(1);
        //track_cust_sensor_init();
        track_drv_sys_cfun(1);
        track_stop_timer(TRACK_CUST_NET_CONN_TIMEOUT_TIMER_ID);
        tr_stop_timer(TRACK_CUST_ALARM_RETURN_FlyMode_TIMER);
        //track_cust_net_conn();
        //tr_start_timer(TRACK_CUST_NET_CONN_TIMER_ID, 1000, track_cust_net_conn);
        if (G_realtime_data.key_status != 0) /* LEDSW 有更高优先级 *//*下面那个函数里面不是已经有这个条件判断了吗??*/
            track_cust_led_sleep(1);
        //track_os_intoTaskMsgQueue(track_drv_ext_vbat_measure_modify_parameters);
        track_soc_exit_fly_only();
        track_cust_call_hangup();
        track_sms_reset();
        LOGD(L_APP, L_V4, "!!! 退出飞行模式!!!");
    }
}

kal_bool track_cust_flymode_set(U32 mode)
{
    static kal_bool fly_on = 0, alarm = 0;
    static U32 tick_alarm = 0;
    U32 tick = OTA_kal_get_systicks();
    U32 volt = track_cust_get_battery_volt();
    U32 volt_ext = track_cust_get_ext_volt();
    kal_bool islowbat = track_cust_isbatlow();
    kal_bool accStatus = track_cust_status_acc();

    if (G_parameter.flycut != 0)
    {
        if (mode == 0)
        {   /* 指令关闭强制退出飞行模式，忽略电压 */
            fly_on = 0;
            alarm = 0;
            track_stop_timer(TRACK_CUST_ALARM_LowBattery_Fly_TIMER);
            track_cust_module_low_power_fly_change(51);
        }
#if !defined(__MT200__)
        else if (islowbat)
        {
            track_stop_timer(TRACK_CUST_ALARM_LowBattery_Fly_TIMER);
            if (sms_staus == SMS_NORMAL_MODE)
            {
                fly_on = 1;
                LOGD(L_APP, L_V4, "内部电池低电，无条件进入固定飞行模式");
                fly_on = 1;
                track_cust_module_low_power_fly_change(1);
            }
        }
#endif
        else if (mode == 1 || accStatus)
        {   /* ACC 高，退出飞行模式 */
            fly_on = 0;
            alarm = 0;
            track_stop_timer(TRACK_CUST_ALARM_LowBattery_Fly_TIMER);
            if (sms_staus == SMS_AIRPLANE_MODE)
                track_cust_module_low_power_fly_change(52);
        }
        else if (mode == 10 && fly_on == 0)
        {   /* 进入飞行模式 */
            // fly_on = 1;
            if (sms_staus == SMS_NORMAL_MODE)
            {
                if (track_is_timer_run(TRACK_CUST_MODULE_ALARM_VIBRATES_DELAY_TIMER_ID))
                {
                    LOGD(L_APP, L_V5, "震动报警延时中，延时进入飞行模式");
                    track_start_timer(TRACK_CUST_ALARM_LowBattery_Fly_TIMER,
                                      10000, track_cust_flymode_set, (void*)10);
                }
                else
                {
                    fly_on = 1;
                    track_start_timer(TRACK_CUST_ALARM_LowBattery_Fly_TIMER, 60000,
                                      track_cust_module_low_power_fly_change, (void*)2);
                }
            }
            else
            {
                track_stop_timer(TRACK_CUST_ALARM_LowBattery_Fly_TIMER);
            }
        }
        else if (mode == 20)
        {   /* 退出飞行模式 */
            fly_on = 0;
            alarm = 0;
            track_stop_timer(TRACK_CUST_ALARM_LowBattery_Fly_TIMER);
            if (sms_staus == SMS_AIRPLANE_MODE)
            {
                track_cust_module_low_power_fly_change(53);
            }
        }
        else if (mode == 30 && fly_on == 1)
        {   /* 暂时退出飞行模式，延时等待报警事件处理完再重新进入飞行模式 */
            alarm = 1;
            tick_alarm = OTA_kal_get_systicks();
            track_cust_module_low_power_fly_change(54);
            track_start_timer(TRACK_CUST_ALARM_LowBattery_Fly_TIMER,
                              G_parameter.sensor_gps_work * 60000,
                              track_cust_flymode_set, (void*)40);
        }
        else if (mode == 31 && fly_on == 1 && alarm == 1)
        {   /* 报警已经触发，延时未到，新检测到震动，重新计算延时 */
            tick_alarm = OTA_kal_get_systicks();
            track_start_timer(TRACK_CUST_ALARM_LowBattery_Fly_TIMER,
                              G_parameter.sensor_gps_work * 60000,
                              track_cust_flymode_set, (void*)41);
        }
        else if (mode == 32 && fly_on == 1)
        {   /* 暂时退出飞行模式，延时等待震动唤醒GPS定位事件处理完再重新进入飞行模式 */
            alarm = 1;
            track_cust_module_low_power_fly_change(54);
            track_start_timer(TRACK_CUST_ALARM_LowBattery_Fly_TIMER,
                              G_parameter.sensor_gps_work * 60000,
                              track_cust_flymode_set, (void*)42);
        }
        else if (mode >= 40 && mode <= 49 && fly_on == 1)
        {
            if (mode <= 41 && track_cust_module_alarm_lock(98, 2) != 0
                    && kal_ticks_to_secs(OTA_kal_get_systicks() - tick_alarm) < G_parameter.sensor_gps_work * 120)/*防止临时退出飞行模式进行报警，由于报警无法顺利送出导致不能再进入飞行的问题*/
            {
                track_start_timer(TRACK_CUST_ALARM_LowBattery_Fly_TIMER,
                                  30000,
                                  track_cust_flymode_set, (void*)40);
            }
            else
            {
                alarm = 0;
                track_cust_module_low_power_fly_change(3);
            }
        }
    }
    LOGD(L_APP, L_V7, "%d,%d, volt:%d, volt_ext:%d, flyMode:%d, fly_on:%d, isBatLow:%d, sensor_gps_work:%d",
         G_parameter.flycut, mode, volt, volt_ext, sms_staus, fly_on, islowbat, G_parameter.sensor_gps_work);
    if (mode == 188)
    {
        return fly_on;//CJJ，不应该查这个值判断是否是在飞行中吗?SMS_AIRPLANE_MODE的值，报警触发会暂时改变到正常模式
    }
    return (sms_staus == SMS_AIRPLANE_MODE ? 1 : 0);
}

#endif /* __FLY_MODE__ */

void track_cust_module_420d_low_power_alarm(void)
{
    extern void track_cust_delay_timeout_close();
    track_gps_data_struct *gpsLastPoint = NULL;
    kal_uint8 tmp = 0;
    gpsLastPoint = track_cust_gpsdata_alarm();
    tmp = track_cust_gps_status();
    if (track_cust_lowpower_flag_data(99) == 1)
    {
        LOGD(L_APP, L_V4, "电压未回升到3.8V，本次不报警");
        return;
    }
    if (G_parameter.low_power_alarm.sw)
    {
        if (track_cust_module_alarm_lock(TR_CUST_ALARM_LowBattery, 2) != 0)
        {
            LOGD(L_APP, L_V4, "上一电池低电报警请求未完成!");
            return;
        }
        track_cust_check_batteryalm_flag(1);
        track_cust_module_alarm_lock(TR_CUST_ALARM_LowBattery, 1);
        track_cust_send_battery_alarm_msg(1);
        if (tmp >= GPS_STAUS_2D)
                {
                    track_cust_dsc_location_paket(gpsLastPoint,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,4,14);
                    track_cust_check_beforepoweroff_flag(0);
                    track_cust_check_batteryalm_flag(0);
                }
       else
       {
            if(G_parameter.wifi.sw == 1)
            {
                track_cust_module_overtime_upwifi();
                }
            else
            {
                LOGD(L_APP,L_V5,"低电报警发包");
                track_cust_check_batteryalm_flag(0);
                track_cust_dsc_location_paket(NULL,track_drv_get_lbs_data(),LOCA_MORE_LBS,DSC_CUST_POSITION_TIMING,4,14); 
                track_cust_check_beforepoweroff_flag(0);
                }
            }
        if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
        {
            LOGD(L_APP, L_V5, "电池低电报警,延时2分30秒关机");
            track_cust_module_delay_close(150);
        }
        track_cust_module_alarm_lock(TR_CUST_ALARM_LowBattery, 0);
        track_start_timer(TRACK_CUST_CHECK_LOWBAT_CHANGEFLAG_TIMER_ID, 3000, track_cust_lowpower_flag_data,(void *)1);
    }
}
