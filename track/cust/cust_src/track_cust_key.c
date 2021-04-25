/******************************************************************************
 * track_cust_key.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        GT300按键处理
 *
 * modification history
 * --------------------
 * v1.0   2013-07-5,  wangqi create this file
 *
 ******************************************************************************/



/****************************************************************************
* Include Files                   		包含头文件
*****************************************************************************/
#include "med_api.h"
#include "track_cust.h"
#include "track_cust_key.h"
#include "track_at_call.h"
#include "track_drv_system_interface.h"

/*****************************************************************************
 *  Define					宏定义
 *****************************************************************************/
#define DOBULE_TIME 1000
/*****************************************************************************
* Typedef  Enum
*****************************************************************************/


/*****************************************************************************
* Typedef  Enum
*****************************************************************************/


/*****************************************************************************
 *  Struct					数据结构定义
 *****************************************************************************/


/*****************************************************************************
* Local variable				局部变量
*****************************************************************************/

/*****************************************************************************
* Golbal variable				全局变量
*****************************************************************************/


/****************************************************************************
* Global Variable - Extern          引用全局变量
*****************************************************************************/
extern void track_cust_module_sos_trigger_button(void);

/*****************************************************************************
 *  Global Functions	- Extern		引用外部函数
 *****************************************************************************/
#if defined (__NETWORK_LICENSE__)
extern void track_drv_network_license_key_make_call(void);
#endif /* __NETWORK_LICENSE__ */
extern void track_cust_set_call_state(void);
extern void track_cust_alarm_pwoffalm(void);
extern kal_uint32 track_cust_check_sosnum(void);
/*****************************************************************************
 *  Local Functions			本地函数
 *****************************************************************************/
void track_cust_key_updata_position(void * operate);


/*****************************************************************************
 *  Global Functions			全局函数
 *****************************************************************************/
/******************************************************************************
 *  Function    -  track_cust_key_single_press
 *
 *  Purpose     -
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0   13-08-2013,  Wangqi
 * ----------------------------------------
*******************************************************************************/
static void track_cust_key_single_press(void)
{
    extern kal_uint8 track_cust_key_dail_way(kal_uint8 option);

    LOGD(L_APP, L_V5, "=====");
#if defined(__GT310__)
	if(track_get_call_state() == TR_CALL_RING)
    {
		track_cust_call_accept();
	}

#else
    if(track_is_in_call())
    {
        if(track_cust_get_SOS_callout())
        {
            //   track_cust_key_control_led();
        }
        else
        {
            track_cust_key_dail_way(2);
            track_cust_call_hangup();
        }
    }
    else
    {
        // track_cust_key_control_led();
    }
#endif /* __GT03F__ */
}
/******************************************************************************
 *  Function    -  track_cust_key_sos_double_press
 *
 *  Purpose     -  SOS按键双击事件处理
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0   13-08-2013,  Wangqi
 * v1.1   29-10-2013,  ZengPing && LiuJW
 * ----------------------------------------
*******************************************************************************/
static kal_uint8 track_cust_key_sos_double_press(void)
{
    kal_uint8 result = 0xFF;
    kal_uint8 defence_status = track_cust_status_defences();
    LOGD(L_APP, L_V5, "=====current status:%d,sw:%d", defence_status , G_parameter.vibrates_alarm.sw);

    if(G_parameter.vibrates_alarm.sw == 0 || track_cust_get_work_mode() == WORK_MODE_3)
    {
        LOGD(L_APP, L_V5, "报警开关为关闭状态设防操作失效");
    }
    else if(0 == defence_status)
    {
        result = 1;
        track_cust_status_defences_change((void*)1);
        track_stop_timer(TRACK_CUST_MODULE_ALARM_VIBRATES_TIMER_ID);
#if defined(__GT03F__) ||defined(__GT300__) || defined(__GW100__)||defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
        track_start_timer(
            TRACK_CUST_STATUS_DEFENCES_TIMER_ID, 
            30 * 1000, 
            track_cust_status_defences_change, (void*)2);
#else
        track_start_timer(
            TRACK_CUST_STATUS_DEFENCES_TIMER_ID, 
            G_parameter.defense_time * 1000, 
            track_cust_status_defences_change, (void*)2);
#endif
    }
    else if(1 == defence_status || 2 == defence_status)
    {
        result = 0;
        track_cust_status_defences_change((void*)0);
    }
    else
    {
        LOGD(L_APP, L_V5, "=====ERROR defence status:%d", defence_status);
        //重启
    }
    return result;
}

/******************************************************************************
 *  Function    -  track_cust_key_led_sleep
 *
 *  Purpose     -  GT03进入休眠状态
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 14-10-2013,  jwj  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_key_led_sleep(void)
{
    LOGD(L_APP, L_V5, "");
#if defined(__GT500__)  //500 外电接入的时候5分钟关闭
    if((track_cust_status_charger() >= 1 || track_cust_gps_status() == 0) && G_realtime_data.key_status == 0)
    {
        track_cust_led_sleep(2);
    }
#elif !defined(__GT740__)||!defined(__GT420D__)
    track_cust_led_sleep(2);
#endif
#if defined (__GT310__)
	if(track_cust_status_charger() >= 1)
    {
       led_level_state_switch();
    }

#elif defined(__GT03F__)||defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
    if(track_cust_status_charger() == 1)
    {
        LED_PWR_StatusSet(PW_CHARGING_STATE);
    }
    else if(track_cust_status_charger() == 2)
    {
        LED_PWR_StatusSet(PW_FULLCHARGED_STATE);
    }
#endif /* __GT300__ */

    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
}

void track_cust_led_wake(void)
{
    LOGD(L_APP, L_V5, "");
    G_realtime_data.key_status = 1;
    track_cust_led_sleep(1);
    if(G_parameter.lcd_on_time)
    {
        tr_start_timer(TRACK_CUST_SOS_KEY_LED_ON_TIMER, G_parameter.lcd_on_time * 1000, track_cust_key_led_sleep);
    }
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
}
/******************************************************************************
 *  Function    -  track_cust_key_control_led
 *
 *  Purpose     -  按键控制led灯
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 15-10-2013,  jwj  written
 * ----------------------------------------
 ******************************************************************************/
 void track_cust_key_control_led(void)
{
    LOGD(L_APP, L_V5, "按键按下=%d", G_realtime_data.key_status);
#if defined(__GT310__)||defined(__GT300S__)
    if(track_is_timer_run(TRACK_CUST_SOS_KEY_LED_ON_TIMER))
    {
        tr_start_timer(TRACK_CUST_SOS_KEY_LED_ON_TIMER, 20 * 1000, track_cust_key_led_sleep);
    }
    else
    {
        track_cust_led_sleep(1);
        tr_start_timer(TRACK_CUST_SOS_KEY_LED_ON_TIMER, 20 * 1000, track_cust_key_led_sleep);
    }
#elif defined(__GT02E__) || defined(__GT02__)|| defined(__ET130__) || defined(__NT23__)|| defined(__NT31__)||defined(__NT36__)
    if(track_cust_led_sleep(99) == 1)
    {
        tr_stop_timer(TRACK_CUST_SOS_KEY_LED_ON_TIMER);
        G_realtime_data.key_status = 0;
        track_cust_led_sleep(2);
    }
    else
    {
        G_realtime_data.key_status = 1;
        track_cust_led_sleep(1);
    }
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
#else
    if(track_cust_led_sleep(99) == 1)
    {
        tr_stop_timer(TRACK_CUST_SOS_KEY_LED_ON_TIMER);
        G_realtime_data.key_status = 0;
        track_cust_key_led_sleep();
    }
    else
    {
	track_cust_led_wake();
    }
#endif
}
/******************************************************************************
 *  Function    -  track_cust_key_sos
 *
 *  Purpose     -  SOS按键处理事件
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * v1.5 , 13-08-2013,  Wangqi
 * ----------------------------------------
*******************************************************************************/
void track_cust_key_sos(void)
{
    LOGD(L_APP, L_V5, "按键按下=%d", G_realtime_data.key_status);
    if(KAL_TRUE == track_is_timer_run(TRACK_CUST_PWROFFALM_TIMER_ID))
    {
        LOGD(L_APP, L_V5, "关机报警发送中,不处理SOS按键");
        return;
    }
    track_stop_timer(TRACK_CUST_LED_START_TIMER);

#if defined( __GT03F__) || defined(__GT310__)||defined(__GT300S__)
    if(track_is_timer_run(TRACK_CUST_SOS_KEY_DOUBLE_TIMER))
    {
		tr_stop_timer(TRACK_CUST_SOS_KEY_DOUBLE_TIMER);
        track_cust_key_sos_double_press();
    }
    else
    {
        tr_start_timer(TRACK_CUST_SOS_KEY_DOUBLE_TIMER, DOBULE_TIME, track_cust_key_single_press);
    }
#endif /* __GT03F__ */

#if !defined( __OLED__)
    track_cust_key_control_led();
#endif /* __OLED__ */
}

/******************************************************************************
 *  Function    -  track_cust_key_dail_way
 *  Purpose     -  按键触发打电话
 *  Description - return1 是FN触发
                        2 SOS键
 *  modification history
 * ----------------------------------------
 * v1.0   09-12M-2013,  Wangqi
 * ----------------------------------------
*******************************************************************************/
kal_uint8 track_cust_key_dail_way(kal_uint8 option)
{
    static kal_uint8 cur_way = 0;
    if(option == 0 || option == 1 || option == 2)
    {
        cur_way = option;
        if(option == 1)
        {
#if defined(__AUDIO_RC__)
            track_cust_monitor_set_speaket(0);
#endif /* __AUDIO_RC__ */
        }
    }

    return cur_way;
}



#if defined(  __GT300__) || defined(__GW100__)||defined(__GT300S__)
/******************************************************************************
 *  Function    -  track_cust_key_short_press_sos
 *
 *  Purpose     -  GT300 SOS按键处理事件
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0   13-08-2013,  Wangqi
 * ----------------------------------------
*******************************************************************************/
void track_cust_key_short_press_sos(void)
{
    LOGD(L_APP, L_V5, "");
    if(track_is_in_call())
    {
        if(track_cust_get_SOS_callout())
        {
            track_cust_key_control_led();
        }
        else
        {
            track_cust_key_dail_way(2);
            track_cust_call_hangup();
        }
    }
    else
    {
#if !defined( __OLED__)
        track_cust_key_control_led();
#endif
    }
}
/******************************************************************************
 *  Function    -  track_cust_key_up
 *
 *  Purpose     -  上(UP) 按键处理事件
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0   13-08-2013,  Wangqi
 * ----------------------------------------
*******************************************************************************/
static void track_cust_key_up(void)
{
    kal_uint8 voice_level , audio_mode;
    LOGD(L_APP, L_V5, "call_state:%d", track_get_call_state());
    if(track_cust_is_working_mode4())
    {
        return;
    }
    if(track_is_calling())
    {
        audio_mode = track_drv_sys_aud_get_active_audio_mode();
        voice_level = track_drv_sys_aud_get_volume_level(audio_mode, 4/*AUD_VOLUME_SPH*/);
        LOGD(L_APP, L_V5, "mode:%d,level:%d", audio_mode, voice_level);
        if(voice_level < 6)
        {
            track_drv_sys_audio_set_volume(4/*VOL_TYPE_SPH*/, voice_level + 1);
        }
        else
        {
            track_cust_Vibration_100ms();
        }
    }
    else if(track_get_call_state() == TR_CALL_RING)
    {
        track_cust_call_accept();
    }
    else
    {
        track_cust_key_control_led();
    }
}
/******************************************************************************
 *  Function    -  track_cust_key_down
 *
 *  Purpose     -  下(DOWN)按键处理事件
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0   13-08-2013,  Wangqi
 * ----------------------------------------
*******************************************************************************/
static void track_cust_key_down(void)
{
    kal_uint8 voice_level , audio_mode;

    LOGD(L_APP, L_V5, "call_state:%d", track_get_call_state());
    if(track_cust_is_working_mode4())
    {
        return;
    }
    if(track_is_calling())
    {
        audio_mode = track_drv_sys_aud_get_active_audio_mode();
        voice_level = track_drv_sys_aud_get_volume_level(audio_mode, 4/*AUD_VOLUME_SPH*/);
        LOGD(L_APP, L_V5, "mode:%d,level:%d", audio_mode, voice_level);
        if(voice_level > 0)
        {
            track_drv_sys_audio_set_volume(4, voice_level - 1);
        }
        else
        {
            track_cust_Vibration_100ms();
        }
    }
    else if(track_get_call_state() == TR_CALL_RING)
    {
        track_cust_call_accept();
    }
    else
    {
        track_cust_key_control_led();
    }
}
/******************************************************************************
 *  Function    -  track_cust_key_fun
 *
 *  Purpose     -  功能键(FUN)按键处理事件
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0   13-08-2013,  Wangqi
 * ----------------------------------------
*******************************************************************************/
static void track_cust_fn_key_single_press(void)
{
    LOGD(L_APP, L_V5, " ");
}
 void track_cust_key_fun(void)
{
    LOGD(L_APP, L_V5, "按键按下=%d", G_realtime_data.key_status);

    track_stop_timer(TRACK_CUST_LED_START_TIMER);

#if defined( __GT03F__) || defined(__GT310__) || defined(__GW100__)||defined(__GT300S__)
    if(track_is_timer_run(TRACK_CUST_SOS_KEY_DOUBLE_TIMER))
    {
        tr_stop_timer(TRACK_CUST_SOS_KEY_DOUBLE_TIMER);
        track_cust_key_sos_double_press();
    }
    else
    {
        tr_start_timer(TRACK_CUST_SOS_KEY_DOUBLE_TIMER, DOBULE_TIME, track_cust_fn_key_single_press);
    }
#endif /* __GT03F__ */

    track_cust_key_control_led();
}
static void track_cust_call_sos_hangup(void)
{
    if(track_is_in_call())
    {
        if(track_cust_get_SOS_callout())
        {
            track_cust_make_call((void*)23);
            track_cust_dial_makecall((void*)23);
            track_cust_key_dail_way(2);
            track_cust_call_hangup();
        }
    }
}
/******************************************************************************
 *  Function    -  track_cust_key_power
 *
 *  Purpose     -  Power键短按键处理事件
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0   13-08-2013,  Wangqi
 * ----------------------------------------
*******************************************************************************/
static void track_cust_key_power(void)
{
    LOGD(L_APP, L_V5, "");
#if defined(__GW100__)
    if(KAL_TRUE == track_is_timer_run(TRACK_CUST_PWROFFALM_TIMER_ID))
    {
        if(track_cust_led_sleep(99) == 1)
        {
            tr_stop_timer(TRACK_CUST_SOS_KEY_LED_ON_TIMER);
            G_realtime_data.key_status = 0;
            track_cust_key_led_sleep();
        }
    }
    else
#endif
    {
        track_cust_key_control_led();
    }

}
/******************************************************************************
 *  Function    -  track_cust_key_change_mode
 *  Purpose     -  来回切换免提听筒模式2为免提
 *  Description -
 *  modification history
 * ----------------------------------------
 * v1.0   06d-12m-2013y,  Wangqi
 * ----------------------------------------
*******************************************************************************/
static void track_cust_key_change_mode(void)
{
    static kal_uint8 current_mode ;
    current_mode = track_drv_sys_aud_get_active_audio_mode();
    LOGD(L_APP, L_V5, "current_mode:%d,%d", track_status_audio_mode(0XFF));
    current_mode = (current_mode == 2 ? 0 : 2);
    track_drv_sys_l4cuem_set_audio_mode(current_mode);//可用切换到免提
}

/******************************************************************************
 *  Function    -  track_cust_key_long_press_up
 *
 *  Purpose     -  上(UP)键长按键处理事件
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0   13-08-2013,  Wangqi
 * ----------------------------------------
*******************************************************************************/
static void track_cust_key_long_press_up(void)
{
    LOGD(L_APP, L_V5, "calling:%d", track_is_calling());
    if(track_cust_is_working_mode4())
    {
        return;
    }
    if(track_is_in_call())
    {
        track_cust_key_change_mode();
    }
#if defined (__GT310__)||defined(__GT300S__)
    else if((strlen(G_phone_number.fn_num[0])) && (track_cust_get_work_mode() != WORK_MODE_4))
#else
    else if(strlen(G_phone_number.fn_num[0]))
#endif /* __GT300__ */
    {
#if defined (__GT310__) || defined(__GW100__)||defined(__GT300S__)
        ControlVibration();
        track_cust_set_call_state();
        LED_GSM_StatusSet(GSM_IN_CALL_STATE);
#endif /* __GT300__ */
        track_cust_key_dail_way(1);
        track_cust_call(G_phone_number.fn_num[0]);
    }
}
/******************************************************************************
 *  Function    -  track_cust_key_long_press_down
 *
 *  Purpose     -  下(DOWN)键长按键处理事件
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0   13-08-2013,  Wangqi
 * ----------------------------------------
*******************************************************************************/
static void track_cust_key_long_press_down(void)
{
    LOGD(L_APP, L_V5, "calling:%d", track_is_calling());
    if(track_cust_is_working_mode4())
    {
        return;
    }
    if(track_is_in_call())
    {
        track_cust_key_change_mode();
    }
#if defined (__GT300__)||defined(__GT300S__)
    else if((strlen(G_phone_number.fn_num[1])) && (track_cust_get_work_mode() != WORK_MODE_4))
#else
    else if(strlen(G_phone_number.fn_num[1]))
#endif /* __GT300__ */
    {
#if defined (__GT300__) || defined(__GW100__)||defined(__GT300S__)
        ControlVibration();
        track_cust_set_call_state();
        LED_GSM_StatusSet(GSM_IN_CALL_STATE);
#endif /* __GT300__ */
        track_cust_key_dail_way(1);
        track_cust_call(G_phone_number.fn_num[1]);
    }
}
/******************************************************************************
 *  Function    -  track_cust_key_long_press_fun
 *
 *  Purpose     -  功能键(FUN)长按键处理事件
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0   13-08-2013,  Wangqi
 * ----------------------------------------
*******************************************************************************/
static void track_cust_key_long_press_fun(void)
{

    LOGD(L_APP, L_V5, "Short Press Function Key, and Send one Position to Server!");

    track_cust_Vibration_100ms();
    if(track_cust_gps_status() >= GPS_STAUS_2D)
    {
        //track_os_intoTaskMsgQueue(track_cust_key_updata_position);
        track_cust_key_updata_position((void *)2);
    }
    else
    {
        track_cust_gprson_status(1);
        track_soc_gprs_reconnect((void*)303);
        track_cust_gps_work_req((void*)12);

    }

}
#endif

#if defined(__GT310__) || defined(__GW100__)||defined(__GT300S__)
/******************************************************************************
 *  Function    -  track_cust_key_up_power
 *
 *  Purpose     -  Power键长按键处理事件
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0   13-08-2013,  Wangqi
 * ----------------------------------------
*******************************************************************************/
static void track_cust_key_up_power(void)
{
    LOGD(L_CON, L_V5, "");
    track_os_intoTaskMsgQueue2(track_drv_sys_power_off_req);
}

/******************************************************************************
 *  Function    -  track_cust_key_long_press_power
 *
 *  Purpose     -  SOS按键处理事件
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0   13-08-2013,  Wangqi
 * ----------------------------------------
*******************************************************************************/
static void track_cust_key_long_press_power(void)
{
    LOGD(L_APP, L_V5, "");
#if !defined(__GT300S__)
    track_cust_audio_play_voice(1);
    ControlVibration();
#endif
    track_drv_set_key_handler(track_cust_key_up_power, KEY_POWER, KEY_EVENT_UP);
}

/******************************************************************************
 *  Function    -  track_cust_key_long_press_power_off
 *
 *  Purpose     -  关机处理函数
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 25-09-2014,  ZengPing && Liujunwu  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_key_long_press_power_off(void)
{
    kal_uint32 index = 0;
    LOGD(L_APP, L_V5, "");

    if(KAL_FALSE == track_is_timer_run(TRACK_CUST_PWROFFALM_TIMER_ID))
    {
#if defined(__GT310__)
		if(G_parameter.pwrlimit_sw == 0 && track_cust_status_sim() != 1)
#else
        if(G_parameter.pwrlimit_sw == 0)
#endif
        {
#if defined(__GT300S__)
            track_cust_key_long_press_power();
#endif /* __GT300__ */
            if(G_parameter.pwroff.sw == 1)
            {
                track_cust_led_sleep(2);
                index = track_cust_check_sosnum();
                track_cust_alarm_pwoffalm();
                if(G_parameter.pwroff.alarm_type == 1 && index != 0)
                {
                    tr_start_timer(TRACK_CUST_PWROFFALM_TIMER_ID, (index * 11) * 1000, track_cust_key_up_power);
                }
                else
                {
                    tr_start_timer(TRACK_CUST_PWROFFALM_TIMER_ID, 5 * 1000, track_cust_key_up_power);
                }
            }
            else
            {
                track_drv_set_key_handler(track_cust_key_up_power, KEY_POWER, KEY_EVENT_UP);
            }
        }
        else
        {
            LOGD(L_APP, L_V5, "限制关机!");
        }
    }
    else
    {
        LOGD(L_APP, L_V5, "关机报警发送短信中!");
    }
}


//短按
static void track_cust_shortpress_fun(kal_uint16 flag)
{
    static kal_uint16 index = 0;
    LOGD(L_APP, L_V5, "index = %d;flag = %d", index, flag);
    if(flag == 1)
    {
        index++;
        if(index < 5)
        {
            track_start_timer(TRACK_CUST_KEY_SHORTPRESSFUN_TIMER_ID, 700, track_cust_shortpress_fun, (void *)2);
        }
    }
    else if(flag == 2)
    {
        if(index >= 5)
        {
            LOGD(L_APP, L_V6, "1");
            tr_start_timer(TRACK_CUST_PWROFF_TIMER_ID, 2 * 1000, track_drv_sys_power_off_req);
        }
        else
        {
            LOGD(L_APP, L_V6, "5");
            track_cust_key_control_led();
        }
        index = 0;
    }
}


/******************************************************************************
 *  Function    -  track_cust_key_updata
 *
 *  Purpose     -  处理按键上报位置
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0   14-10-2013,  Wangqi
 * ----------------------------------------
*******************************************************************************/
void track_cust_key_updata_position(void * operate)
{
    int opt = (int)operate;
    kal_uint8 gps_status = track_cust_gps_status();

    LOGD(L_APP, L_V5, "opt:%d,gps_status:%d", operate, gps_status);

    if(opt == 0)
    {
        track_cust_paket_18(track_drv_get_lbs_data(), 0);
        track_cust_gps_location_timeout(5);
    }
    else if(opt == 1)
    {
        track_stop_timer(TRACK_CUST_KEY_UPDATA_OVER_TIMER_ID);
        track_cust_gps_location_timeout(20);
    }
    else if(opt == 2)
    {
        track_gps_data_struct *p_gpsdata = track_cust_gpsdata_alarm();
        if(p_gpsdata->gprmc.status != 0 && (p_gpsdata->gprmc.Latitude != 0 || p_gpsdata->gprmc.Longitude != 0))
        {
            kal_uint16 sn;
            if(p_gpsdata->gprmc.status == 1)
            {
                track_cust_backup_gps_data(3, p_gpsdata);
            }
            sn = track_cust_paket_position(p_gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_KEY, 0);
        }
        else
        {
            LOGD(L_APP, L_V6, "设备一次定位都未完成过");
        }
    }

}


/******************************************************************************
 *  Function    -  track_cust_switch_keyshortpress_control
 *
 *  Purpose     -  GT360短按键判断
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 29-07-2016,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_switch_keyshortpress_control(void)
{
    LOGD(L_APP, L_V5, "");
    track_cust_shortpress_fun(1);
}


#endif /*  __GT300__ */


#if defined (__GT370__)|| defined (__GT380__)
void track_cust_powerkey_long_press_power_off(void)
{        
    if((track_is_autotestmode() == KAL_TRUE) || (track_is_testmode() == KAL_TRUE))
    {
        return;
    }
    
    if(G_parameter.pwrlimit_sw == 0)
    {
        LOGD(L_DRV, L_V1, "单片机控制关机!");
        //track_os_intoTaskMsgQueue2(track_drv_sys_power_off_req);
    }
    else
    {
        LOGD(L_APP, L_V5, "限制关机!");
    }
}
#endif /* __GT370__ */

static void track_cust_key_event_wakeup(void)
{
    LOGD(L_APP, L_V5,"MCU唤醒GSM");
    track_drv_mcu_and_gsm_sleep(1);
}


#if defined(__GT310__)
void track_cust_key_double_sos_updata_position(void * operate)
{
    int opt = (int)operate;
    kal_uint8 gps_status = track_cust_gps_status();

    LOGD(L_APP, L_V5, "opt:%d,gps_status:%d", operate, gps_status);

    if(opt == 0)
    {
		///track_cust_paket_18(track_drv_get_lbs_data(), 0);
		track_cust_sendWIFI_paket();
        track_cust_gps_location_timeout(5);
    }
    else if(opt == 1)
    {
        track_stop_timer(TRACK_CUST_KEY_UPDATA_OVER_TIMER_ID);
        track_cust_gps_location_timeout(20);
    }
    else if(opt == 2)
    {
        track_gps_data_struct *p_gpsdata = track_cust_gpsdata_alarm();
        if(p_gpsdata->gprmc.status != 0 && (p_gpsdata->gprmc.Latitude != 0 || p_gpsdata->gprmc.Longitude != 0))
        {
            kal_uint16 sn;
            if(p_gpsdata->gprmc.status == 1)
            {
                track_cust_backup_gps_data(3, p_gpsdata);
            }
            sn = track_cust_paket_position(p_gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_KEY, 0);
        }
        else
        {
            LOGD(L_APP, L_V6, "设备一次定位都未完成过");
        }
    }

}


void track_cust_key_sos_double(void)
{
	 LOGD(L_APP, L_V5, "Short Press Function Key, and Send one Position to Server!");

    if(track_cust_gps_status() >= GPS_STAUS_2D)
    {
        track_cust_key_double_sos_updata_position((void *)2);
    }
    else
    {
		track_cust_open_location_mode(12);
    }

}
#endif

/******************************************************************************
 *  Function    -  track_cust_keypad_init
 *
 *  Purpose     -  按键注册
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0   13-08-2013,  Wangqi
 * ----------------------------------------
*******************************************************************************/
void track_cust_keypad_init(void)
{
    if((track_is_autotestmode() == KAL_TRUE) || (track_is_testmode() == KAL_TRUE))
    {
#if defined (__GT310__) || defined(__GW100__)||defined(__GT300S__)
        track_drv_set_key_handler(track_cust_key_long_press_power, KEY_POWER, KEY_LONG_PRESS);
#endif /* __GT300__ */
        return;
    }
#if defined (__NETWORK_LICENSE__)
    track_drv_network_license_key_make_call();
#endif /* __NETWORK_LICENSE__ */

#if defined(__GT310__)
	track_drv_set_key_handler(track_cust_switch_keyshortpress_control, KEY_POWER, KEY_SHORT_PRESS);
	track_drv_set_key_handler(track_cust_key_long_press_power_off, KEY_POWER, KEY_LONG_PRESS);
#else

#if defined(__GT03F__) ||defined( __GT300__)  || defined(__GW100__)||defined(__GT300S__)
    track_drv_set_key_handler(track_cust_module_sos_trigger_button, KEY_SOS, KEY_LONG_PRESS);
#if defined(__GT300__) || defined(__GW100__)||defined(__GT300S__)
    track_drv_set_key_handler(track_cust_key_short_press_sos, KEY_SOS, KEY_SHORT_PRESS);
    track_drv_set_key_handler(track_cust_key_up, KEY_TR1, KEY_SHORT_PRESS);
    track_drv_set_key_handler(track_cust_key_down, KEY_TR2, KEY_SHORT_PRESS);
    track_drv_set_key_handler(track_cust_key_fun, KEY_FUN, KEY_SHORT_PRESS);
    track_drv_set_key_handler(track_cust_key_power, KEY_POWER, KEY_SHORT_PRESS);

    track_drv_set_key_handler(track_cust_key_long_press_up, KEY_TR1, KEY_LONG_PRESS);
    track_drv_set_key_handler(track_cust_key_long_press_down, KEY_TR2, KEY_LONG_PRESS);
    track_drv_set_key_handler(track_cust_key_long_press_fun, KEY_FUN, KEY_LONG_PRESS);
#if defined( __GW100__) || defined(__GT300__)||defined(__GT300S__)
    track_drv_set_key_handler(track_cust_key_long_press_power_off, KEY_POWER, KEY_LONG_PRESS);
#else
    track_drv_set_key_handler(track_cust_key_long_press_power, KEY_POWER, KEY_LONG_PRESS);
#endif /* __GW100__ */
#endif
#endif /* __GT300__ */

#if defined (__GT370__)|| defined (__GT380__)
    track_drv_set_key_handler(track_cust_powerkey_long_press_power_off, KEY_POWER, KEY_LONG_PRESS);
    track_drv_set_key_handler(track_cust_key_event_wakeup, KEY_POWER, KEY_FULL_PRESS_DOWN);
#endif /* __GT370__ */

#if defined (__MT200__) && defined (__CUST_ACC_BY_MCU__)
    track_drv_set_key_handler(track_cust_key_event_wakeup, KEY_POWER, KEY_FULL_PRESS_DOWN);
#endif /* __MT200__ */

#if defined (__JM81__)||defined (__JM66__)
    track_drv_set_key_handler(track_cust_key_event_wakeup, KEY_POWER, KEY_FULL_PRESS_DOWN);
#endif /* __JM81__ */

#if defined (__HVT001__)
    track_drv_set_key_handler(track_cust_module_sos_trigger_button, KEY_SOS, KEY_LONG_PRESS);
#endif /* __HVT001__ */

#endif
}

