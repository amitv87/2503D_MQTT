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
static struct_msg_send g_msg = {0};

#if defined(__GERMANY__)
static char *str_power_fails_en = "Stromversorgung unterbrochen! ";
static char *str_power_fails_en1 = "Stromversorgung unterbrochen!";
#elif defined(__SPANISH__)
static char *str_power_fails_en = "Corte de corriente!";
#else
static char *str_power_fails_en = "Cut off Power Alarm! ";
static char *str_power_fails_en1 = "Cut off Power Alarm!Please Attention!";
#endif

#if defined(__GT500__)
static kal_uint8 str_power_fails_cn[10] =
{
    0x62, 0xD4, 0x51, 0xFA, 0x62, 0xA5, 0x8B, 0x66, 0xFF, 0x01
};
/*static kal_uint8 str_power_fails_cn1[18] =
{
    0x65, 0xAD, 0x75, 0x35, 0x62, 0xA5, 0x8B, 0x66, 0xFF, 0x01, 0x8B, 0xF7, 0x51, 0x73, 0x6C, 0xE8,
    0xFF, 0x01
};*/
static kal_uint8 str_power_fails_cn1[18] =
{
    0x62, 0xD4, 0x51, 0xFA, 0x62, 0xA5, 0x8B, 0x66, 0x00, 0x21, 0x8B, 0xF7, 0x51, 0x73, 0x6C, 0xE8,
    0x00, 0x21
};
#else
static kal_uint8 str_power_fails_cn[10] =    // 断电报警！
{
    0x65, 0xAD, 0x75, 0x35, 0x62, 0xA5, 0x8B, 0x66, 0xFF, 0x01
};
#endif

#if defined(__INDIA__)
static char *str_pwonalm_en = "External connection alarm!";

static kal_uint8 str_pwonalm_cn[14] =    // 接电报警！
{
    0x59, 0x16, 0x75, 0x35, 0x63, 0xA5, 0x90, 0x1A, 0x62, 0xA5, 0x8B, 0x66, 0xFF, 0x01
};
#endif /* __INDIA__ */

#if defined (__GT380__)
static char *str_charge_in_alarm_en = "Charger access notification!";
//充电器接入。
static kal_uint8 str_charge_in_alarm_cn[] = {0x51, 0x45, 0x75, 0x35, 0x56, 0x68, 0x63, 0xA5, 0x51, 0x65, 0x30, 0x02, 0x00, 0x00};
#endif /* __GT380__ */

static track_gps_data_struct gps_data_buf = {0};
/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/
static kal_bool  first_poweralm = KAL_TRUE;//开机首次断电报警过滤

/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/
extern kal_uint8 g_alarm_gps_status;//报警记录报警期间GPS定位情况//0,不定位;1,定位
/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/
extern void track_cust_key_led_sleep(void);

#if defined (__CUSTOM_DDWL__)
extern void track_cust_dormancy_delay_to_flight_mode(void);
#endif /* __CUSTOM_DDWL__ */

/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/
#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      本地函数
******************************************************************************/

/******************************************************************************
 *  Function    -  track_cus_power_fails_overTime_sendsms
 *
 *  Purpose     -  断电报警发送短信
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void track_cus_power_fails_overTime_sendsms(void)
{

    track_gps_data_struct *gpsLastPoint = NULL;
    LOGD(L_APP, L_V5, "");
    if(G_parameter.power_fails_alarm.sw && G_parameter.power_fails_alarm.alarm_type)
    {
        memset(&g_msg, 0, sizeof(struct_msg_send));
#if defined(__GT500__)
        if(g_alarm_gps_status == 1 || track_cust_gps_status() > 2)
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
        g_msg.cm_type = CM_SMS;
        if(g_msg.addUrl == KAL_TRUE)
        {
            track_fun_strncpy(g_msg.msg_en, str_power_fails_en, CM_PARAM_LONG_LEN_MAX);
            g_msg.msg_cn_len = 10;
            memcpy(g_msg.msg_cn, str_power_fails_cn, g_msg.msg_cn_len);
        }
        else
        {
            track_fun_strncpy(g_msg.msg_en, str_power_fails_en1, CM_PARAM_LONG_LEN_MAX);
            g_msg.msg_cn_len = 18;
            memcpy(g_msg.msg_cn, str_power_fails_cn1, g_msg.msg_cn_len);
        }
#else
        g_msg.addUrl = KAL_TRUE;
        g_msg.cm_type = CM_SMS;
        track_fun_strncpy(g_msg.msg_en, str_power_fails_en, CM_PARAM_LONG_LEN_MAX);
        g_msg.msg_cn_len = 10;
        memcpy(g_msg.msg_cn, str_power_fails_cn, g_msg.msg_cn_len);
#endif
        track_cust_module_alarm_msg_send(&g_msg, &gps_data_buf);
        if(G_parameter.power_fails_alarm.alarm_type == 2)
        {
            track_start_timer(TRACK_CUST_ALARM_power_fails_Call_TIMER, 60000, track_cust_make_call, (void*)1);
        }
        else
        {
            track_cust_module_alarm_lock(TR_CUST_ALARM_PowerFailure, 0);
        }
    }
    else
    {
        //track_cust_module_alarm_lock(TR_CUST_ALARM_PowerFailure, 0);
        LOGD(L_APP, L_V4, "断电报警，报警类型设置无需短信通知！");
    }
    //track_cust_module_alarm_set_type(0);
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_PowerFailure);
}
#if defined(__XCWS__)
static void track_powerfail_check_delay(void)
{
    LOGD(L_APP, L_V5, "断电报警，静默时间到");
}
#endif

#ifdef __NT31__
void track_power_fails_delay_timer(void)
{
    track_gps_data_struct *gpsLastPoint = NULL;
    if(!G_parameter.power_fails_alarm.sw)
    {
        return;
    }
    if(track_cust_status_charger())
    {
        LOGD(L_APP, L_V5, "已经充电，不再发送");
        return;
    }
    if(G_parameter.power_fails_alarm.send_alarm_time == 0)
    {
        LOGD(L_APP, L_V5, "不在报低电报警");
        return;
    }
    if(G_realtime_data.power_fails_flg == 0)
    {
        LOGD(L_APP, L_V5, "标志位不清楚，不报警");
        return;
    }
    if(!track_soc_login_status())
    {
        tr_start_timer(TRACK_CUST_POWER_FAILS_TIMER_ID, G_parameter.power_fails_alarm.send_alarm_time * 1000, track_power_fails_delay_timer);
        return;
    }
    gpsLastPoint = track_cust_gpsdata_alarm();
    track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_PowerFailure, KAL_FALSE, 0);
    tr_start_timer(TRACK_CUST_POWER_FAILS_TIMER_ID, G_parameter.power_fails_alarm.send_alarm_time * 1000, track_power_fails_delay_timer);
}
#endif /* __NT31__ */
#if defined(__INDIA__)
/******************************************************************************
 *  Function    -  pwonalm_sendsms
 *
 *  Purpose     -  发送外电接入短信
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 25-10-2017,  LYL  written
 * ----------------------------------------
 ******************************************************************************/
void pwonalm_sendsms(void)
{
    struct_msg_send g_msg = {0};
    g_msg.addUrl = KAL_FALSE;
    g_msg.cm_type = CM_SMS;
    snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_pwonalm_en);
    g_msg.msg_cn_len = 14;
    memcpy(g_msg.msg_cn, str_pwonalm_cn, g_msg.msg_cn_len);
    track_cust_module_alarm_msg_send(&g_msg, NULL);
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_CHARGE_IN_ALM);
}
/******************************************************************************
 *  Function    -  track_cust_power_in_alarm
 *
 *  Purpose     -  外电接入报警动作
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 25-10-2017,  LYL  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_power_in_alarm(void)
{
    track_gps_data_struct *gpsLastPoint = NULL;
    gpsLastPoint = track_cust_gpsdata_alarm();
    if(G_parameter.pwonalm.sw == 0)
    {
        return;
    }
    if(track_cust_module_alarm_lock(TR_CUST_ALARM_CHARGE_IN_ALM, 2) != 0)
    {
        LOGD(L_APP, L_V4, "上一外电接入报警请求未完成!1");
        return;
    }
    if(track_is_timer_run(TRACK_CUST_ALARM_pwonalm_Call_TIMER))
    {
        LOGD(L_APP, L_V4, "上一外电接入报警请求未完成!2");
        return;
    }
    track_cust_module_alarm_lock(TR_CUST_ALARM_CHARGE_IN_ALM, 1);
    G_parameter.pwonalm_flag = 0;
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    LOGD(L_APP, L_V4, "接电标志位置0");

    LOGD(L_APP, L_V5, "!!! 外电接入报警 !!!");
    if(G_parameter.pwonalm.alarm_type >= 0)
    {
        if(G_parameter.pwonalm.alarm_type == 0)
        {
            track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_CHARGE_IN_ALM, KAL_TRUE, 1);
            track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_CHARGE_IN_ALM);
        }
        else if(G_parameter.pwonalm.alarm_type == 1)
        {
            track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_CHARGE_IN_ALM, KAL_TRUE, 1);
            track_cust_module_alarm_lock(TR_CUST_ALARM_CHARGE_IN_ALM, 0);
            if(track_nvram_alone_parameter_read()->gprson == 0)
            {
                track_os_intoTaskMsgQueue(pwonalm_sendsms);
            }
            else
            {
                pwonalm_sendsms();
            }
            track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_CHARGE_IN_ALM);
        }
        else if(G_parameter.pwonalm.alarm_type == 2)
        {
            track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_CHARGE_IN_ALM, KAL_TRUE, 1);
            track_cust_module_alarm_lock(TR_CUST_ALARM_CHARGE_IN_ALM, 0);
            if(track_nvram_alone_parameter_read()->gprson == 0)
            {
                track_os_intoTaskMsgQueue(pwonalm_sendsms);
            }
            else
            {
                pwonalm_sendsms();
            }
            track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_CHARGE_IN_ALM);
            track_start_timer(TRACK_CUST_ALARM_pwonalm_Call_TIMER, 60 * 1000, track_cust_make_call, (void*)1);
        }
        else if(G_parameter.pwonalm.alarm_type == 3)
        {
            track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_CHARGE_IN_ALM, KAL_TRUE, 1);
            track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_CHARGE_IN_ALM);
            track_start_timer(TRACK_CUST_ALARM_pwonalm_Call_TIMER, 60 * 1000, track_cust_make_call, (void*)1);
        }
    }
    else
    {
        track_cust_module_alarm_lock(TR_CUST_ALARM_CHARGE_IN_ALM, 0);
    }
}
#endif /* __INDIA__ */
/******************************************************************************
 *  Function    -  power_fails
 *
 *  Purpose     -  触发断电报警动作
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void power_fails(void)
{
    track_gps_data_struct *gpsLastPoint = NULL;

    LOGD(L_APP, L_V5, "");

#if defined(__XCWS__)
    track_stop_timer(TRACK_CUST_MODULE_ALARM_POWER_FAILS_TIMER_ID);
    track_stop_timer(TRACK_CUST_MODULE_ALARM_POWERFAIL_DELAY_TIMER_ID);
    if(!G_parameter.power_fails_alarm.sw)
    {
        return;
    }

    LOGD(L_APP, L_V4, "!!! 断电报警 !!!");

    if(G_parameter.pof == 1)
    {
        track_cust_alarm_trigger_req(ALARM_EXT_BATTERY_LOW, ALARM_GPRS);
#if defined(__XCWS_TPATH__) && !defined(__KENAER__)
        gpsLastPoint = track_cust_gpsdata_alarm();
        if(track_nvram_alone_parameter_read()->gprson)
        {
            track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_PowerFailure, KAL_TRUE, 1);
        }
#endif

    }
    if(G_parameter.pofs == 1)
    {
        track_cust_alarm_trigger_req(ALARM_EXT_BATTERY_LOW, ALARM_SMS);
    }

    if((G_parameter.pof == 1) || (G_parameter.pofs == 1))
    {
        if(G_parameter.pwralm_filter > 0)
        {
            tr_start_timer(TRACK_CUST_MODULE_ALARM_POWERFAIL_TIMER_ID, G_parameter.pwralm_filter * 60 * 1000, track_powerfail_check_delay); //重庆移动静默加gprs
        }
    }
    return;
#endif
    if(!G_parameter.power_fails_alarm.sw)
    {
        return;
    }
    if(track_cust_module_alarm_lock(TR_CUST_ALARM_PowerFailure, 2) != 0)
    {
        LOGD(L_APP, L_V4, "上一断电报警请求未完成!");
        return;
    }
#if defined(__FLY_MODE__)
    track_cust_flymode_set(30);
#endif /*__FLY_MODE__*/

#if defined(__INDIA__) && defined(__NT31__)
    G_parameter.pwonalm_flag = 1;
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    LOGD(L_APP, L_V4, "接电标志位置1");
#endif

    LOGD(L_APP, L_V4, "!!! 断电报警 !!!");

#if defined (__CUSTOM_DDWL__)
    track_cust_dormancy_change_flight_mode(0);
    if(track_cust_gps_status() < 2)
    {
        track_cust_gps_work_req((void*)10);
    }
    tr_start_timer(TRACK_CUST_DORMANCY_ALARM_DELAY_TO_FLIGHT_TIMER_ID, 300 * 1000, track_cust_dormancy_delay_to_flight_mode);
#endif /* __CUSTOM_DDWL__ */

    track_cust_led_alarm();
    track_cust_module_alarm_lock(TR_CUST_ALARM_PowerFailure, 1);
    gpsLastPoint = track_cust_gpsdata_alarm();
#if 0//defined(__GT500__)
    g_alarm_gps_status = 0;
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
        track_cust_alarm_type(TR_CUST_ALARM_PowerFailure);
        if(g_alarm_gps_status == 1)
        {
            track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_PowerFailure, KAL_TRUE, 1);
            tr_start_timer(TRACK_CUST_ALARM_power_fails_OVERTIME_TIMER, TRACK_ALARM_DELAY_TIME_SECONDS, track_cus_power_fails_overTime_sendsms);
        }
        else
        {
            track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_PowerFailure, KAL_TRUE, 1);
            tr_start_timer(TRACK_CUST_ALARM_power_fails_OVERTIME_TIMER, 5 * 60 * 1000, track_cus_power_fails_overTime_sendsms);
        }
    }
#else

    memcpy(&gps_data_buf, gpsLastPoint, sizeof(track_gps_data_struct));
    if(track_cust_gps_status() > 2)
    {
        g_alarm_gps_status = 1;
    }
    if(track_nvram_alone_parameter_read()->gprson)
    {
        track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_PowerFailure, KAL_TRUE, 1);
    }
#endif
    if(G_parameter.power_fails_alarm.alarm_type > 0)
    {

        if(G_parameter.power_fails_alarm.alarm_type == 3)
        {
            track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_PowerFailure);
            track_start_timer(TRACK_CUST_ALARM_power_fails_Call_TIMER, 10 * 1000, track_cust_make_call, (void*)1);
        }
        else
#if defined (__GT300__) && defined(__LANG_ENGLISH__)
            if(track_nvram_alone_parameter_read()->gprson == 0)
#else
            if(G_parameter.lang == 0 || track_nvram_alone_parameter_read()->gprson == 0)
#endif /* __GT300__ */
            {
                //tr_stop_timer(TRACK_CUST_ALARM_power_fails_OVERTIME_TIMER);
                track_os_intoTaskMsgQueue(track_cus_power_fails_overTime_sendsms);
            }
            else
            {
                if(track_is_timer_run(TRACK_CUST_ALARM_power_fails_OVERTIME_TIMER))
                {
                    LOGD(L_APP, L_V4, "上一断电报警请求未完成!");
                }
                else
                {
                    tr_start_timer(TRACK_CUST_ALARM_power_fails_OVERTIME_TIMER, TRACK_ALARM_DELAY_TIME_SECONDS, track_cus_power_fails_overTime_sendsms);
                }
            }
    }
    else
    {
        track_cust_module_alarm_lock(TR_CUST_ALARM_PowerFailure, 0);
    }

#ifdef __NT31__
    G_realtime_data.power_fails_flg = 1;
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    tr_start_timer(TRACK_CUST_POWER_FAILS_TIMER_ID, G_parameter.power_fails_alarm.send_alarm_time * 1000, track_power_fails_delay_timer);
#endif /* __NT31__ */

}

#if defined (__GT380__)
/******************************************************************************
 *  Function    -  track_cust_charge_alarm_overTime_sendsms
 *
 *  Purpose     -  充电器插拔报警超时短信
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-07,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_charge_alarm_overTime_sendsms(void *arg)
{
    kal_uint32 charge_in = (kal_uint32)arg;

    track_gps_data_struct *gpsLastPoint = NULL;
    LOGD(L_APP, L_V5, "%d", charge_in);

    if(G_parameter.charge_alarm.sw && G_parameter.charge_alarm.alarm_type)
    {
        memset(&g_msg, 0, sizeof(struct_msg_send));

        if(charge_in)
        {
            g_msg.addUrl = KAL_TRUE;
            g_msg.cm_type = CM_SMS;
            track_fun_strncpy(g_msg.msg_en, str_charge_in_alarm_en, CM_PARAM_LONG_LEN_MAX);
            g_msg.msg_cn_len = sizeof(str_charge_in_alarm_cn);
            memcpy(g_msg.msg_cn, str_charge_in_alarm_cn, g_msg.msg_cn_len);
        }
        else
        {
            g_msg.addUrl = KAL_TRUE;
            g_msg.cm_type = CM_SMS;
            track_fun_strncpy(g_msg.msg_en, str_power_fails_en, CM_PARAM_LONG_LEN_MAX);
            g_msg.msg_cn_len = sizeof(str_power_fails_cn);
            memcpy(g_msg.msg_cn, str_power_fails_cn, g_msg.msg_cn_len);
        }
        track_cust_module_alarm_msg_send(&g_msg, &gps_data_buf);

        if(G_parameter.charge_alarm.alarm_type == 2)
        {
            track_start_timer(TRACK_CUST_ALARM_power_fails_Call_TIMER, 60000, track_cust_make_call, (void*)1);
        }
        else
        {
            track_cust_module_alarm_lock(TR_CUST_ALARM_PowerFailure, 0);
        }
    }
    else
    {
        LOGD(L_APP, L_V4, "CHARGE ALM无需短信通知");
    }

    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_PowerFailure);
}


/******************************************************************************
 *  Function    -  track_charge_alarm
 *
 *  Purpose     -  充电器插入和拔出报警通知
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-07,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_charge_alarm(void *arg)
{
    static kal_uint8 filter_assert_enable = 1;

    kal_uint32 charge_in = (kal_uint32)arg;
    track_gps_data_struct *gpsLastPoint = NULL;

    LOGD(L_APP, L_V5, "GT380 charge alarm %d", charge_in);


    if(PWRKEYPWRON == mmi_Get_PowerOn_Type() || CHRPWRON == mmi_Get_PowerOn_Type() || (filter_assert_enable == 0))
    {
        //go on
    }
    else
    {
        filter_assert_enable = 0;
        LOGD(L_APP, L_V5, "ABNRESET no alarm");
        return;
    }

    if(!G_parameter.charge_alarm.sw)
    {
        return;
    }
    if(track_cust_module_alarm_lock(TR_CUST_ALARM_PowerFailure, 2) != 0)
    {
        LOGD(L_APP, L_V4, "上一断电/充电报警请求未完成!");
        return;
    }

    track_cust_led_alarm();
    track_cust_module_alarm_lock(TR_CUST_ALARM_PowerFailure, 1);
    gpsLastPoint = track_cust_gpsdata_alarm();

    memcpy(&gps_data_buf, gpsLastPoint, sizeof(track_gps_data_struct));
    if(track_cust_gps_status() > 2)
    {
        g_alarm_gps_status = 1;
    }
    if(track_nvram_alone_parameter_read()->gprson)
    {
        if(charge_in)
        {
            track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_CHARGE_IN_ALM, KAL_TRUE, 1);
        }
        else
        {
            track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_PowerFailure, KAL_TRUE, 1);
        }
    }

    if(G_parameter.charge_alarm.alarm_type > 0)
    {
        if(G_parameter.charge_alarm.alarm_type == 3)        //M=3  GPRS+CALL
        {
            track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_PowerFailure);
            track_start_timer(TRACK_CUST_ALARM_power_fails_Call_TIMER, 10 * 1000, track_cust_make_call, (void*)1);
        }
        else
        {
            //M=1 SMS+GPRS;M=2 GPRS+SMS+CALL
            if(G_parameter.lang == 0 || track_nvram_alone_parameter_read()->gprson == 0)
            {
                track_os_intoTaskMsgQueueExt(track_cust_charge_alarm_overTime_sendsms, (void *)charge_in);
            }
            else
            {
                if(track_is_timer_run(TRACK_CUST_ALARM_power_fails_OVERTIME_TIMER))
                {
                    LOGD(L_APP, L_V4, "上一断电报警请求未完成!");
                }
                else
                {
                    track_start_timer(TRACK_CUST_ALARM_power_fails_OVERTIME_TIMER, TRACK_ALARM_DELAY_TIME_SECONDS, track_cust_charge_alarm_overTime_sendsms, (void *)charge_in);
                }
            }
        }
    }
    else
    {
        track_cust_module_alarm_lock(TR_CUST_ALARM_PowerFailure, 0);
    }
}
#endif /* __GT380__ */

/*****************************************************************************
*  Global Functions                     全局函数
******************************************************************************/

/******************************************************************************
 *  Function    -  track_cust_module_power_fails
 *
 *  Purpose     -  断电报警判断
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 26-02-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_power_fails(void *arg)
{
    static kal_uint8 poweralm_filter = 1;
    static kal_uint8 T2 = 1;
    static kal_uint32 T3 = 0, last_tick = 0;
    kal_uint32 par = (kal_uint32)arg;
    kal_uint32 tick = 0;
    kal_bool valid = KAL_TRUE;

#if defined(__NETWORK_LICENSE__)
    LOGD(L_APP, L_V4, "par:%d __NETWORK_LICENSE__", par);
    return;
#endif

#if defined (__GT380__)
    //俺使用CHARGEALM控制断电和充电都报警
    if(!G_parameter.charge_alarm.sw)
    {
        return;
    }

#else
    if(!G_parameter.power_fails_alarm.sw)
    {
        return;
    }
#endif /* __GT380__ */


    if(par == 21)/* ACC ON */
    {
        T3 = 0;
        last_tick = 0;
        LOGD(L_APP, L_V5, "清除断电报警保护时间");
        return;
    }
    tick = OTA_kal_get_systicks();
    if(par == 20)/* ACC OFF */
    {
        last_tick = tick + 217;
        T3 = tick + kal_secs_to_ticks(G_parameter.power_fails_alarm.delay_time_acc_on_to_off);
        LOGD(L_APP, L_V5, "更新断电报警保护时间 T3=%d, tick=%d", T3, tick);
        return;
    }
    LOGD(L_APP, L_V7, "par=%d, T2=%d, sw=%d, delay_time_on=%d, delay_time_off=%d, T3=%d, last_tick=%d, tick=%d, poweralm_filter:%d, delay_time_acc_on_to_off:%d",
         par, T2,
         G_parameter.power_fails_alarm.sw,
         G_parameter.power_fails_alarm.delay_time_on,
         G_parameter.power_fails_alarm.delay_time_off,
         T3, last_tick, tick, poweralm_filter,
         G_parameter.power_fails_alarm.delay_time_acc_on_to_off);
    switch(par)
    {
        case 0: // 断开充电
        case 1:
#if defined (__GT380__)
//nc
#elif defined (__EXT_VBAT_ADC__) && !defined(__EXT_VBAT_Protection__) && !defined (__NT31__) && !defined(__NT36__)
            if((track_drv_ext_disconnect() == KAL_FALSE))
            {
                if(G_realtime_data.disExtBattery2 == 1)
                {
                    G_realtime_data.disExtBattery2 = 0;
                    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                }
                LOGD(L_APP, L_V5, "根据当前的外电压值，判断是因低电主动断开外电不触发断电报警");//因外电电压低，主动断开外电不触发断电报警
                break;
            }
#endif /* __EXT_VBAT_ADC__ */
            if(G_parameter.power_fails_alarm.delay_time_acc_on_to_off != 0 && T3 != 0 && tick < T3)
            {
                return;
            }
#if defined(__MT200__)
            if(first_poweralm)//使用__EXT_VBAT_Protection__，变成电压检测断电报警，不会出现刚开机，接上外电就触发断电报警吗?
            {
                //MT200会
                first_poweralm = KAL_FALSE;
                return;
            }
#endif
#if defined(__NT36__)||defined(__MT200__) || defined (__NT33__)||defined (__GT380__)
            if(T2 || poweralm_filter)//CJJ,20161029,解决第一次断电报警没有以T1时间为条件的BUG
#else
            if(T2)
#endif
            {
#if defined(__XCWS__)
                if((track_is_timer_run(TRACK_CUST_ACCL_CHARGEOUT_CHECK_TIMER_ID) == KAL_FALSE) && (track_is_timer_run(TRACK_CUST_MODULE_ALARM_POWERFAIL_TIMER_ID) == KAL_FALSE))
                {
                    track_start_timer(TRACK_CUST_MODULE_ALARM_POWERFAIL_DELAY_TIMER_ID, G_parameter.power_fails_alarm.delay_time_off * 1000, track_cust_module_power_fails, (void*)10);
                }
                else
                {
                    LOGD(L_APP, L_V5, "断电报警，静默时间未到");
                }
#else
                track_start_timer(
                    TRACK_CUST_MODULE_ALARM_POWER_FAILS_TIMER_ID,
                    G_parameter.power_fails_alarm.delay_time_off * 1000,/* T1 参数时间 */
                    track_cust_module_power_fails,
                    (void*)10);
#if !defined(__EXT_VBAT_Protection__)
                track_at_vs_send("AT+EADC=1\r\n");
#endif /* __ET310__ */

#endif
            }
            else
            {
                track_stop_timer(TRACK_CUST_MODULE_ALARM_POWER_FAILS_TIMER_ID);
            }
            if(G_realtime_data.netLogControl & 32)
            {
                cust_packet_log(32, 0, 0, 0, 0, 0, 0);
            }
            break;

        case 5: // 接入充电
        case 6:
#ifdef __NT31__
            track_stop_timer(TRACK_CUST_POWER_FAILS_TIMER_ID);
#endif /* __NT31__ */

            track_start_timer(
                TRACK_CUST_MODULE_ALARM_POWER_FAILS_TIMER_ID,
                G_parameter.power_fails_alarm.delay_time_on * 1000,
                track_cust_module_power_fails,
                (void*)11);
            if(G_realtime_data.netLogControl & 32)
            {
                cust_packet_log(32, 1, 0, 0, 0, 0, 0);
            }
            T2 = 0;
#if defined(__XCWS__)
            tr_stop_timer(TRACK_CUST_ACCL_CHARGEOUT_CHECK_TIMER_ID);
            tr_stop_timer(TRACK_CUST_MODULE_ALARM_POWERFAIL_DELAY_TIMER_ID);//断电延时
#endif
            break;

        case 10: // 断开充电 确认
#if defined (__GT380__)
            if(poweralm_filter == 1)
            {
                track_os_intoTaskMsgQueueExt(track_charge_alarm, (void *)0);
                poweralm_filter = 0;
            }
#else
#if  defined (__EXT_VBAT_ADC__) && !defined(__NT36__) && !defined(__EXT_VBAT_Protection__)&& !defined (__NT31__)
            if((track_drv_ext_disconnect() == KAL_FALSE))
            {
                LOGD(L_APP, L_V5, "根据当前的外电压值，判断是因低电主动断开外电不触发断电报警");//因外电电压低，主动断开外电不触发断电报警
                break;
            }
#endif /* __EXT_VBAT_ADC__ */
            if(poweralm_filter == 1)
            {
                track_os_intoTaskMsgQueue(power_fails);
                poweralm_filter = 0;
            }
#endif
            break;

        case 11: // 接入充电 确认
#if defined (__GT380__)
            LOGD(L_APP, L_V5, "确认连接");
            T2 = 1;
            track_os_intoTaskMsgQueueExt(track_charge_alarm, (void *)1);
            poweralm_filter = 1;

#else
            LOGD(L_APP, L_V5, "确认连接");
            T2 = 1;
            poweralm_filter = 1;
#endif
            break;
    }
}



/******************************************************************************
 *  Function    -  track_cust_module_ext_voltage_values
 *
 *  Purpose     -  设备未接独立的ACC检测线，通过外电电压模拟判断ACC状态
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 12-08-2016,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_ext_voltage_values(kal_uint32 volt)
{
    static kal_uint32 lasttick = 0, jj = 0;
    kal_uint32 tick = kal_get_systicks();
    U8 acc = track_cust_status_acc();
    LOGD(L_APP, L_V7, "VOLT:%d,%d", volt, G_realtime_data.disExtBattery2);
    if(volt < 5000000)
    {
        if(lasttick == 0 || jj != 1)
        {
            lasttick = kal_get_systicks() + 3 * KAL_TICKS_1_SEC;
        }
        jj = 1;
        if(lasttick < tick)
        {
            if(acc)
            {
                LOGD(L_APP, L_V5, "ACC 高，等待ACC低才触发");
                return;
            }
            if(G_realtime_data.disExtBattery2 == 0)
            {
#if defined(__MT200__)
                track_cust_led_charger(0);
#endif
                track_cust_module_power_fails((void *)0);
                lasttick = 0;
                G_realtime_data.disExtBattery2 = 1;
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
            }

        }
    }
    else if(volt > 6000000)
    {
        if(first_poweralm == KAL_TRUE)/*外电连接情况下，终端重启了*/
        {
            first_poweralm = KAL_FALSE;
        }
        if(lasttick == 0 || jj != 2)
        {
            lasttick = kal_get_systicks() + 3 * KAL_TICKS_1_SEC;
        }
        jj = 2;
        if(lasttick < tick)
        {
            if(G_realtime_data.disExtBattery2 == 1)
            {
#if defined(__MT200__)
                track_cust_led_charger(1);
#endif
                track_cust_module_power_fails((void *)5);
                lasttick = 0;
                G_realtime_data.disExtBattery2 = 0;
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
            }
        }
    }
}


