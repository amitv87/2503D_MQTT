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
static kal_bool delay_valid = KAL_FALSE;

/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/
extern void track_cust_intermittent_send_LBS(void *arg);

#if defined (__GT380__)
extern void track_cust_simple_gsm_led_resume(void);
extern void track_cust_simple_gsm_led_sleep(void);
#endif /* __GT380__ */

#if defined (__VIRTUAL_ACC__)
extern void track_sensor_delay_invalid_virtual_acc_off(void);
#endif

/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/
kal_uint8 track_cust_sensor_count(kal_uint8 par);

/*****************************************************************************
*  Local Functions                      本地函数
******************************************************************************/

/******************************************************************************
 *  Function    -  sensor_delay_valid
 *
 *  Purpose     -  Sensor振动一次判定为运动状态的时间结束
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void sensor_delay_valid(void)
{
    delay_valid = KAL_FALSE;
#if defined(__GT370__)|| defined (__GT380__)
	LOGD(L_SENSOR, L_V5, "进入静止状态");
	track_cust_mode_change(1);
#endif
}

//3分钟内没有检测到10秒5次震动，则进入静止状态
void track_sensor_set_moving_mode(void)
{
#if defined(__GT370__)|| defined (__GT380__)
    extern kal_uint8 g_when_still_no_gps_enable_pwron_gps;//静止时GPS没有定位，需要重新开GPS 2分钟

    if((KAL_FALSE == delay_valid) && (track_cust_get_work_mode() == WORK_MODE_1))
    {
        LOGD(L_SENSOR, L_V5,"模式1运动");
        delay_valid = KAL_TRUE;
        track_cust_mode_change(5);
    }
	
    delay_valid = KAL_TRUE;

    g_when_still_no_gps_enable_pwron_gps=1;
    tr_start_timer(TRACK_CUST_SENSOR_DELAY_VALID_TIMER_ID, 180000, sensor_delay_valid);
#endif

}
/******************************************************************************
 *  Function    -  xzx_sensor_callback_work
 *
 *  Purpose     -  sensor检测震动后回调函数
 *
 *  Description -  return Null
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 21-07-2012,  Liujw  written
 * ----------------------------------------
 ******************************************************************************/
void sensor_callback_work(void)
{
    if(track_is_testmode())
    {
        LOGS("SENSOR OK#");//白卡自动进测试模式，但震动不会亮LED  chengj
    }
#if defined(__FLY_MODE__) && !defined(__MT200__)
    else if(G_parameter.flycut != 0 && track_cust_isbatlow() && !(track_is_testmode() || track_is_autotestmode()))
    {
        LOGD(L_SENSOR, L_V4, "检测到震动，但电池低电忽略");
        return;
    }
#endif /* __FLY_MODE__ */
    else
    {
        LOGD(L_SENSOR, L_V4, "检测到震动");
    }
    if(g_GT03D == KAL_TRUE && track_cust_status_defences() == 2 && track_is_in_call() && G_parameter.callmode > 0 && track_get_incall_type() == 1)
    {
        LOGD(L_SENSOR, L_V5, "设防来电震动返回");
        return;
    }
#if defined(__GT370__)|| defined (__GT380__)
	track_cust_sensor_count(1);
	track_cust_module_gps_vibrates_wake();
#else
    if(G_realtime_data.power_saving_mode == 1 || (g_GT03D == KAL_TRUE && track_cust_get_work_mode() == WORK_MODE_1 && track_cust_status_defences() == 0))
    {
        tr_stop_timer(TRACK_CUST_MODULE_ALARM_VIBRATES_DELAY_TIMER_ID);
        LOGD(L_SENSOR, L_V6, "省电模式直接返回");
        return;
    }
    track_cust_sensor_count(1);
    delay_valid = KAL_TRUE;
    if(G_parameter.gps_work.static_filter_sw&4)
    {
        tr_start_timer(TRACK_CUST_SENSOR_DELAY_VALID_TIMER_ID, 30000, sensor_delay_valid);
        #if defined (__VIRTUAL_ACC__)
        tr_start_timer(TRACK_CUST_SENSOR_INVALID_ACCOFF_TIMER_ID, 25000, track_sensor_delay_invalid_virtual_acc_off);
        #endif /* __VIRTUAL_ACC__ */
    }
    else
    {
        #if defined(__GT420D__)
        tr_start_timer(TRACK_CUST_SENSOR_DELAY_VALID_TIMER_ID, 30000, sensor_delay_valid);
        #else
        tr_start_timer(TRACK_CUST_SENSOR_DELAY_VALID_TIMER_ID, 300000, sensor_delay_valid);
        #endif
        #if defined (__VIRTUAL_ACC__)
        tr_start_timer(TRACK_CUST_SENSOR_INVALID_ACCOFF_TIMER_ID, 270000, track_sensor_delay_invalid_virtual_acc_off);
        #endif /* __VIRTUAL_ACC__ */
    }
#endif


    track_cust_sensor_add_position((void*)0);
#if defined(__GT300__)  ||defined(__GT03F__) || defined(__GW100__)||defined(__GT300S__)
    if(track_cust_get_work_mode() != WORK_MODE_1)
    {
        track_cust_module_gps_vibrates_wake();
    }
#elif defined(__GT310__)
	if(track_cust_get_work_mode() == WORK_MODE_2 )
    {
        track_cust_module_gps_vibrates_wake();
    }	
#else
    #if !defined(__GT420D__)
    track_cust_module_gps_vibrates_wake();
    #endif 
#endif /* __GT300__ */

#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
    track_cust_intermittent_send_LBS((void *)1);
#endif 


#if defined(__BCA__)
    track_cust_shocks_lbs_update((void *)1);
#endif /* __GT740__ */
#if defined(__GT370__)|| defined (__GT380__)|| defined(__GT300S__)
	if(track_cust_get_work_mode() != WORK_MODE_3)
	{
		track_cust_module_alarm_vibrates((void *)1);
	}
#else
    track_cust_module_alarm_vibrates((void *)1);
#endif

#if defined (__GT380__)
    LOGD(L_OBD, L_V5, "震动LED 1分钟");
    track_cust_simple_gsm_led_resume();
    tr_start_timer(TRACK_CUST_LED_WAKEUP_TIMER, 60000, track_cust_simple_gsm_led_sleep);
#endif /* __GT380__ */

    track_cust_periodic_restart((void*)1);
#if !defined(__NT37__)
    track_motor_lock_change_status(MOTOR_SENSOR_IND);
#endif
}

/*****************************************************************************
*  Global Functions                     全局函数
******************************************************************************/

/******************************************************************************
 *  Function    -  track_cust_sensor_count
 *
 *  Purpose     -  Sensor振动累计计数
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint8 track_cust_sensor_count(kal_uint8 par)
{
    static kal_uint8 count = 0;
    kal_uint8 tmp;
    if(par == 1)
    {
        // 累加计数
        count++;
        tmp = count;
    }
    else if(par == 2)
    {
        // 查询返回值
        tmp = count;
    }
    else if(par == 3)
    {
        // 返回且清零
        tmp = count;
        count = 0;
    }
    return tmp;
}

/******************************************************************************
 *  Function    -  track_cust_sensor_delay_valid
 *
 *  Purpose     -  查询是否处于振动后的有效延时内
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_bool track_cust_sensor_delay_valid()
{
    if(G_parameter.sensor_gps_work == 0)
    {
        return  TRUE;
    }
    return delay_valid;     /*KAL_FALSE=静止    --    chengjun  2017-04-19*/
}

kal_bool track_cust_sensor_delay_valid_acc()
{
    return delay_valid;
}

/******************************************************************************
 *  Function    -  xzx_custome_sensor_init
 *
 *  Purpose     -  sensor初始化处理函数
 *
 *  Description -  return Null
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 21-07-2012,  Liujw  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_sensor_init(void)
{
    kal_int8 sensor_l = 0;
    if((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        return;
    }
#if defined (__GT740__) || defined(__JM66__)||defined(__GT420D__)
    return;
#endif
    sensor_l = G_parameter.vibrates_alarm.sensitivity_level;
#if defined(__XCWS__)
    sensor_l  =  sensor_l/3 + 1;
    if(sensor_l > 5) sensor_l = 5;
#endif
    LOGD(L_APP, L_V5, "sensitivity level:%d,%d", G_parameter.vibrates_alarm.sensitivity_level,sensor_l);
#ifdef __GT300S__
  if(G_parameter.work_mode.mode == 1)
  {
     return;
  }
    track_drv_sensor_on(sensor_l, sensor_callback_work) ;
#else
    track_drv_sensor_on(sensor_l, sensor_callback_work) ;
#endif /* __GT300S__ */
}

