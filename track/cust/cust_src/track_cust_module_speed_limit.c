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
//#include "Conversions.h"

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
//Your Vehicle's Speed: 50km/h, Please Check!

static kal_uint8 str_speed_limit_cn[48] =   //车辆超速！速度 KM/H,请减速慢行，安全驾驶！
{
    0x8F, 0x66, 0x8F, 0x86, 0x8D, 0x85, 0x90, 0x1F, 0xFF, 0x01, 0x90, 0x1F, 0x5E, 0xA6, 0x00, 0x20,
    0x00, 0x4B, 0x00, 0x4D, 0x00, 0x2F, 0x00, 0x48, 0x00, 0x2C, 0x8B, 0xF7, 0x51, 0xCF, 0x90, 0x1F,
    0x61, 0x62, 0x88, 0x4C, 0xFF, 0x0C, 0x5B, 0x89, 0x51, 0x68, 0x9A, 0x7E, 0x9A, 0x76, 0xFF, 0x01
};
//车辆已超速(68km/h)，请减速慢行，安全驾驶！
static kal_uint8 str_2_1[12] =
{
    0x8F, 0x66, 0x8F, 0x86, 0x5D, 0xF2, 0x8D, 0x85, 0x90, 0x1F, 0x00, 0x28
};
static kal_uint8 str_2_2[34] =
{
    0x00, 0x6B, 0x00, 0x6D, 0x00, 0x2F, 0x00, 0x68, 0x00, 0x29, 0xFF, 0x0C, 0x8B, 0xF7, 0x51, 0xCF,
    0x90, 0x1F, 0x61, 0x62, 0x88, 0x4C, 0xFF, 0x0C, 0x5B, 0x89, 0x51, 0x68, 0x9A, 0x7E, 0x9A, 0x76,
    0xFF, 0x01
};
static track_gps_data_struct gps_data_tmp = {0};
static float g_max_speed = 0;
static kal_uint8 speed_limit_status = 0;
/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/

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
 *  Function    -  speed_limit_overTime
 *
 *  Purpose     -  发送超速报警短信
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void speed_limit_overTime(void *arg)
{
    LOGD(L_APP, L_V5, "");
    if(!G_parameter.speed_limit_alarm.alarm_type || G_parameter.speed_limit_alarm.alarm_type == 3)
    {
        LOGD(L_APP, L_V4, "超速报警，报警类型设置无需短信通知！");
    }
    else
    {
        struct_msg_send g_msg = {0};
        char tmp[100] = {0};
        int len;
        kal_uint32 speed = (kal_uint32)arg;
        g_msg.addUrl = KAL_FALSE;
        g_msg.cm_type = CM_SMS;
#if defined(__GERMANY__)
       snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Geschwindigkeit zu hoch!");
#else
        snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Speeding! The speed of %dKM/H, please slow down, drive safely!", speed);
#endif
        snprintf(tmp, 50, "%d", speed);
        memcpy(g_msg.msg_cn, str_speed_limit_cn, 16);
        len = track_fun_asc_str_to_ucs2_str(&g_msg.msg_cn[16], tmp) - 2;
        g_msg.msg_cn_len = len + 48;
        if(CM_PARAM_LONG_LEN_MAX > g_msg.msg_cn_len)
        {
            memcpy(&g_msg.msg_cn[16+len], &str_speed_limit_cn[16], 32);
            track_cust_module_alarm_msg_send(&g_msg, NULL);
        }
    }
    if(G_parameter.speed_limit_alarm.alarm_type  == 2 || G_parameter.speed_limit_alarm.alarm_type  == 3)
    {
        track_start_timer(TRACK_CUST_ALARM_LowBattery_Call_TIMER, 60000, track_cust_make_call, (void*)1);
    }
#if defined (__XYBB__)
#else
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_Speed_Limit);
#endif /* __XYBB__ */
}

/******************************************************************************
 *  Function    -  speed_over_ind
 *
 *  Purpose     -  确定触发超速报警
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void speed_over_ind(void)
{
    kal_uint32 sp =  g_max_speed;
    LOGD(L_APP, L_V5, "%d时间到，超速报警", kal_ticks_to_secs(kal_get_systicks()));
    g_max_speed = 0;
    speed_limit_status = 2;
    if(track_nvram_alone_parameter_read()->gprson)
    {
        /*超速报警只需通知平台，无需等待平台返回的地址*/
#if defined(__XCWS__)
#elif defined(__XYBB__)
		track_cust_alarm_type(TR_CUST_ALARM_Speed_Limit);
		track_cust_paket_0200(&gps_data_tmp,track_drv_get_lbs_data());
#else
        track_cust_paket_16(&gps_data_tmp, track_drv_get_lbs_data(), TR_CUST_ALARM_Speed_Limit, KAL_FALSE, 0);
#endif
    }
    speed_limit_overTime((void*)sp);
}

/*****************************************************************************
*  Global Functions                     全局函数
******************************************************************************/

/******************************************************************************
 *  Function    -  track_cust_module_speed_limit
 *
 *  Purpose     -  判断是否超速？
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_speed_limit(track_gps_data_struct *gps_data)
{
    if(G_parameter.speed_limit_alarm.sw == 0 || track_cust_get_work_mode() == WORK_MODE_3)
    {
        return;
    }

    if(G_parameter.speed_limit_alarm.sw)
    {
        if(gps_data->status >= 2 && gps_data->gprmc.Speed >= G_parameter.speed_limit_alarm.threshold)
        {
            if(speed_limit_status != 2/* && g_max_speed < gps_data->gprmc.Speed*/)
            {
                g_max_speed = gps_data->gprmc.Speed;
                memcpy(&gps_data_tmp, gps_data, sizeof(track_gps_data_struct));
            }
            if(speed_limit_status == 0 && !track_is_timer_run(TRACK_CUST_ALARM_speed_limit_OVERTIME_TIMER))
            {
                LOGD(L_APP, L_V5, "%d,%d已超速，计时开始", kal_ticks_to_secs(kal_get_systicks()), G_parameter.speed_limit_alarm.delay_time);
                speed_limit_status = 1;
                tr_start_timer(
                    TRACK_CUST_ALARM_speed_limit_OVERTIME_TIMER,
                    G_parameter.speed_limit_alarm.delay_time * 1000,
                    speed_over_ind);
            }
        }
        else
        {
            speed_limit_status = 0;
            if(track_is_timer_run(TRACK_CUST_ALARM_speed_limit_OVERTIME_TIMER))
            {
                LOGD(L_APP, L_V5, "速度恢复正常，计时终止开始");
                track_stop_timer(TRACK_CUST_ALARM_speed_limit_OVERTIME_TIMER);
                g_max_speed = 0;
            }
        }
    }
}

/******************************************************************************
 *  Function    -  track_cust_module_speed_limit_cmd_update
 *
 *  Purpose     -  指令更新超速报警状态
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_speed_limit_cmd_update(void)
{
    speed_limit_status = 0;
    track_stop_timer(TRACK_CUST_ALARM_speed_limit_OVERTIME_TIMER);
    g_max_speed = 0;
}

