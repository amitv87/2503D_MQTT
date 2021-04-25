/******************************************************************************
 * track_cmd_auto_test.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        工厂自动化测试
 *
 * modification history
 * --------------------
 * v1.0   2013-02-26,  ZengPing create this file
 *
 ******************************************************************************/
#if defined (__AUTO_TEST__)

/****************************************************************************
* Include Files             包含头文件
*****************************************************************************/
#include "track_cmd.h"
#include "track_drv_led.h"
#include "track_drv_eint_gpio.h"
#include "track_at_context.h"
#include "GlobalConstants.h"
#include "track_at_call.h"
#include "Track_at.h"
#include "app_datetime.h"
#include "track_os_ell.h"
/*****************************************************************************
 *  Define			    宏定义
*****************************************************************************/
#define PWRLED 0
#define GSMLED 1
#define GPSLED 2

/*****************************************************************************
* Typedef  Enum         枚举
*****************************************************************************/

/*****************************************************************************
*  Struct			    数据结构定义
*****************************************************************************/
typedef struct
{
    LED_Mode PWR_LED;
    LED_Mode GPS_LED;
    LED_Mode GSM_LED;
} LED_SATAUS;
/*****************************************************************************
* Local variable            局部变量
*****************************************************************************/

/****************************************************************************
* Global Variable           全局变量
*****************************************************************************/
static LED_SATAUS last_mode = {0};
/*****************************************************************************
 *  Local Functions	            本地函数
*****************************************************************************/


/*****************************************************************************
 *  Global Functions            全局函数
*****************************************************************************/
static void led_site_restoration(void)
{
    track_drv_led_set_all_off();
}

static void track_ft_led_on(LED_Type Type)
{
    if(Type < LED_Num1 || Type > LED_Num_Max)
    {
        LOGC(L_TEST, L_V4, "track_ft_led_on Type Error!");
        return;
    }
    if(Type == LED_Num_Max)
    {
        track_drv_led_set_all_on();
    }
    else
    {
        track_drv_led_set_status(Type, LedMode_ON);
    }
}

static void track_ft_led_off(LED_Type Type)
{
    if(Type < LED_Num1 || Type > LED_Num_Max)
    {
        LOGC(L_TEST, L_V4, "track_ft_led_on Type Error!");
        return;
    }
    if(Type == LED_Num_Max)
    {
        track_drv_led_set_all_off();
    }
    else
    {
        track_drv_led_set_status(Type, LedMode_OFF);
    }
}
void track_makecall_autotest(CMD_DATA_STRUCT * Cmds)
{
    LOGC(L_TEST, L_V4, "track_makecall_autotest= %d", Cmds->part);
    if(Cmds->part == 1)
    {
        LOGC(L_TEST, L_V4, "track_makecall_autotest= %s", Cmds->pars[1]);
        track_make_call((char *) Cmds->pars[1]);
    }
    else
    {
        sprintf(Cmds->rsp_msg, "ERROR:100");
    }
}

/******************************************************************************
 *  Function    -  track_set_autotestmode
 *
 *  Purpose     -  设置当前是否是自动测试模式
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 26-02-2013,  ZengPing & Liujw  written
 * ----------------------------------------
 ******************************************************************************/
void track_set_autotestmode(kal_uint8 mode)
{
    track_status_autotest(1);
    if(mode == 2)
    {
        track_ft_led_on(LED_Num_Max);
    }
    LOGC(L_TEST, L_V4, "track_set_autotestmode = %d", track_status_autotest(2));
}

kal_bool track_is_autotestmode(void)
{
    LOGC(L_TEST, L_V7, "track_is_autotestmode = %d", track_status_autotest(2));
    return track_status_autotest(2);
}
void track_TM_autotest_call_status_change(kal_uint16 callType, kal_uint16 callState)
{
    LOGD(L_TEST, L_V5, "type:%d,status:%d", callType, callState);
    if(callType == CALL_MT_IN)
    {
        switch(callState)
        {
                LOGD(L_TEST, L_V5, "2 type:%d,status:%d", callType, callState);
            case TR_CALL_RING:
                LOGD(L_TEST, L_V5, "3 type:%d,status:%d", callType, callState);
                track_drv_led_set_all_off();
                track_send_at_accept_call();
                LOGS("RING_OK");//RING
                break;
            case TR_CALL_CONNECT:
                track_drv_led_set_status(GSMLED, LedMode_ON);
                LOGS("THROUGH");
                break;
            default:
                break;
        }
    }
    else if(callType == CALL_MO_OUT)
    {
        switch(callState)
        {
            case TR_CALL_RING:
                track_drv_led_set_all_off();
                LOGS("RING_OK");
                break;
            case TR_CALL_CONNECT:
                track_drv_led_set_all_off();
                track_drv_led_set_status(GSMLED, LedMode_ON);
                LOGS("THROUGH");
                break;
            default:
                break;
        }
    }
}
void track_cust_call_status_change_autotest(void* msg)
{
    call_state_struct *ptr = (call_state_struct *) msg;
    kal_uint16 _type = ptr->type;/*call_type_enum*/
    kal_uint16 callState = ptr->status;/*track_call_state_enum*/
    LOGD(L_TEST, L_V5, "type:%d,status:%d", ptr->type, ptr->status);

    track_TM_autotest_call_status_change(_type, callState);
}

static void Sensor_Callback_Test(void)
{
    track_drv_led_set_all_on();
    LOGC(L_TEST, L_V4, "SENSOR OK3#");
    LOGS("Sensor_OK");
    tr_start_timer(TRACK_FACTORY_RSTORE_LED_TIME_ID, 1000, track_drv_led_set_all_off);
}

void track_cust_sensor_autotest_init(void)
{
    //track_drv_led_set_all_off();
    LOGC(L_TEST, L_V4, "SENSOR INIT#");
    track_drv_sensor_on(3, Sensor_Callback_Test);
}

/******************************************************************************
 *  Function    -  track_test_acc_high_mode *
 *  Purpose     -  acc 高处理函数 *
 *  Description -  return Null *
 *  modification history
 * ----------------------------------------
 * v1.0  , 18-08-2012,  ZengPing & Liujw  written
 * ----------------------------------------
 ******************************************************************************/
static void track_autotest_acc_high_mode(void)
{
    LOGC(L_TEST, L_V4, "ACC ON");
    #if !defined(__GT420D__)
    LOGS("ACC_H");
    #endif
}
/******************************************************************************
 *  Function    -  track_test_acc_low_mode *
 *  Purpose     -  acc 低处理函数 *
 *  Description -  return Null *
 *  modification history
 * ----------------------------------------
 * v1.0  , 18-08-2012,  ZengPing & Liujw  written
 * ----------------------------------------
 ******************************************************************************/
static void track_autotest_acc_low_mode(void)
{
    #if !defined(__GT420D__)
    LOGC(L_TEST, L_V4, "ACC OFF");
    LOGS("ACC_L");
    #endif
}

/******************************************************************************
 *  Function    -  track_test_acc_status_set *
 *  Purpose     -  acc 状态处理函数 *
 *  Description -  return Null *
 *  modification history
 * ----------------------------------------
 * v1.0  , 18-08-2012,  ZengPing & Liujw  written
 * ----------------------------------------
 ******************************************************************************/
void track_autotest_set_acc_status(kal_uint8 status)
{
    LOGC(L_TEST, L_V4, "%d", status);
    if(status)
    {
        tr_start_timer(TRACK_FACTORY_ACC_CHECK_TIME_ID, 1000, track_autotest_acc_low_mode);
    }
    else
    {
        tr_start_timer(TRACK_FACTORY_ACC_CHECK_TIME_ID, 1000, track_autotest_acc_high_mode);
    }
}

/******************************************************************************
 *  Function    -  track_autotest_Akey_high_mode *
 *  Purpose     -   高处理函数 *
 *  Description -  return Null *
 *  modification history
 * ----------------------------------------
 * v1.0  , 18-08-2012,  ZengPing & Liujw  written
 * ----------------------------------------
 ******************************************************************************/
static void track_autotest_Akey_high_mode(void)
{
    LOGC(L_TEST, L_V4, "KEY IS HIGH ^ LED ON ");
    //track_save_scene();
    track_ft_led_on(LED_Num_Max);
    track_cust_sensor_autotest_init();
}
/******************************************************************************
 *  Function    -  track_autotest_Akey_low_mode *
 *  Purpose     -   低处理函数 *
 *  Description -  return Null *
 *  modification history
 * ----------------------------------------
 * v1.0  , 18-08-2012,  ZengPing & Liujw  written
 * ----------------------------------------
 ******************************************************************************/
static void track_autotest_Akey_low_mode(void)
{
    //LOGC(L_TEST, L_V4, "KEY IS LOW V LED OFF ");
    track_ft_led_off(LED_Num_Max);

}
void track_autotest_key_set_callback(void)
{
    static kal_uint32 key_flag = 0;
    LOGC(L_TEST, L_V4, "KEY IS LOW V LED OFF ");
    if(key_flag == 0)
    {
        track_drv_led_set_all_off();
        LOGS("LEDKEY_OFF");
        key_flag = 1;
    }
    else
    {
        track_drv_led_set_all_on();
        LOGS("LEDKEY_ON");
        key_flag = 0;
    }
}
void track_autotest_key_set_init(void)
{
#if defined(__KEYPAD_MOD__)
    track_drv_set_key_handler(track_autotest_key_set_callback, KEY_SOS, KEY_EVENT_DOWN);
#endif /* __KEYPAD_MOD__ */
}

void track_autotest_soskey_status(kal_uint16 eintno, kal_uint8 status)
{
    LOGC(L_TEST, L_V4, "%d", status);
    if(status==1)
    {
        track_drv_led_set_all_off();
        LOGS("EINT:%d,LEVEL:%d,KEY_OFF",eintno, status);
    }
    else
    {
        track_drv_led_set_all_on();
        LOGS( "EINT:%d,LEVEL:%d,KEY_ON",eintno, status);
    }
}

void track_autotestmode_eint_handler(U8 eint, U8 level)
{
#ifdef __USE_SPI__
    	kal_uint8 buffer[50]=0;
#endif /* __USE_SPI__ */

    switch(eint)
    {
        case ACC_EINT_NO:
            track_autotest_set_acc_status(level);
            break;
            // case AUX_EINT_NO:
            // break;
        //case SOS_EINT_NO:
        //    track_autotest_soskey_status(level);
        //    break;
#if defined __USE_SPI__
        case EINT_SPI_NO:
            LOGD(L_APP, L_V5, "EINT_SPI_NO level=%d", level);
            track_spi_re(&buffer[0]);
            LOGD(L_APP, L_V5, "%s, strlen: %d", (char*)buffer,strlen(buffer));
            break;
#endif

#if defined(__KEY_EINT__)
        case EINT_B_NO:
        case EINT_C_NO:
        case EINT_D_NO:
            track_autotest_soskey_status(eint, level);
            break;
#endif /* __KEY_EINT__ */
        default:
            break;
    }
}


/******************************************************************************
 *  Function    -  track_factoty_autotest_gps_fix_time_output
 *
 *  Purpose     -  自动化GPSTC时间查询
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 26-02-2013,  ZengPing  written
 * ----------------------------------------
 *
 *  说明:使用本地时间差计算定位时间，因为GPS定位后校准时间，部分代码必须按固定顺序，否则计算错误
 *  改用开机tick计算则没有关联，不存在顺序问题
 *
 * v2.0  , 16-05-2013,  ZengPing & Liujw  written 
 * ----------------------------------------
 ******************************************************************************/
void track_factoty_autotest_gps_fix_time_output(kal_uint32 output)
{
    static kal_uint32 start_tick=0;
    kal_uint32 fix_tick,time_diff = 0;
    static kal_int8 buffer[100];
    static kal_uint32 gpscold_flag = 0;
    static kal_int8 starttimebuff[100];
    static kal_int8 fixtimebuff[100];

    if((gpscold_flag == 0 && output != 0) || (gpscold_flag == 2 &&  output == 1))
    {
        return;
    }
    LOGC(L_TEST, L_V4, "output = %d ;gpscold_flag =%d", output, gpscold_flag);
    if(output == 0)//初始设置
    {
        start_tick= kal_get_systicks( );
        gpscold_flag = 1;
    }
    else if(gpscold_flag == 1 && output == 2)//未定位时做查询,回显未定位时间
    {
        fix_tick= kal_get_systicks( );
        time_diff = kal_ticks_to_secs(fix_tick-start_tick);
        memset(buffer, 0, sizeof(buffer));
        sprintf((char *)buffer, "GPS positioning satellite (cold TTFF=%ds) ", time_diff);
        LOGS("%s", buffer);
    }
    else if(gpscold_flag == 1 && output == 1)//定位自动保存耗费时间
    {
        fix_tick= kal_get_systicks( );
        time_diff = kal_ticks_to_secs(fix_tick-start_tick);        
        memset(buffer, 0, sizeof(buffer));
        sprintf((char *)buffer, "FSTTIME_%d#", time_diff);
        gpscold_flag = 2;
        LOGS("GPS_STATE_OK");
    }
    else if(gpscold_flag == 2 && output == 2)//回显查询时间
    {
        LOGS("%s", buffer);
    }
    else// if(gpscold_flag == 0 && output == 2)//未下发冷启指令,直接查询
    {
        LOGS("GPSCOLD_ERROR");
    }

}
kal_uint32 track_factoty_autotest_gpscold_setmode(kal_uint32 mode)
{
    static kal_uint32 mode_flag = 0;
    if(mode == 0 || mode == 1)
    {
        mode_flag = mode;
    }
    return mode_flag;
}

void track_factoty_autotest_gpscold_ttff(void)
{
    if(track_factoty_autotest_gpscold_setmode(3) == 1)
    {
        track_factoty_autotest_gpscold_setmode(0);
        track_factoty_autotest_gps_fix_time_output(1);
    }
}

void track_autotest_unit_gpio(char* part2, char* part3)
{
    char result[100] = {0};
    kal_uint8 gpio;
    char output, exe = 0;
    gpio = atoi(part2);
    if(strcmp((S8*)part3, "H") == 0)
    {
        output = 1;
    }
    else if(strcmp((S8*)part3, "L") == 0)
    {
        output = 0;
    }
    else
    {
        LOGS("Error");
        return;
    }
    GPIO_ModeSetup(gpio, 0);
    GPIO_InitIO(1, gpio);
    GPIO_WriteIO(output, gpio);
    exe = GPIO_ReadIO(gpio);
    if(exe == output)
    {
        sprintf(result, "GPIO_%d_OK", gpio);
    }
    else
    {
        sprintf(result, "GPIO_%d_ERROR", gpio);
    }
    LOGS("%s", result);
}
void track_autotest_unit_sensorlevel(char* part)
{
    kal_uint8 level = 0;

    level = atoi(part);
    LOGS("part=%s level = %d", part, level);
    if(level < 1 || level > 5)
    {
        LOGS("Error");
        return;
    }
    track_drv_sensor_on(level, Sensor_Callback_Test);
    LOGS("SENSOR_LEVEL_OK");
}


/******************************************************************************
 *  Function    -  track_factoty_autotest_epo_req
 * 
 *  Purpose     -  请求PC软件注入EPO数据
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 12-03-2013,  ZengPing & Liujw  written
 * ----------------------------------------
 ******************************************************************************/
void track_factoty_autotest_epo_req(void)
{
    if(track_is_autotestmode())
    {
        LOGS("EPO_REQ");
    }
}

    
#endif /* __AUTO_TEST__ */

