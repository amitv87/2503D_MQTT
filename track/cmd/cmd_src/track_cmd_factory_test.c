/******************************************************************************
 * track_factory_test.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        工厂测试模式
 *
 * modification history
 * --------------------
 * v1.0   2012-08-18,  wangqi create this file
 *
 ******************************************************************************/
/****************************************************************************
* Include Files             包含头文件
*****************************************************************************/
#include "track_cmd.h"
//#include "track_os_call.h"
#include "track_drv_led.h"
#include "track_drv_eint_gpio.h"
#include "track_at_context.h"
#include "GlobalConstants.h"
#include "track_at_call.h"
#include "Track_at.h"
/*****************************************************************************
 *  Define			    宏定义
*****************************************************************************/
#define PWRLED 0
#define GSMLED 1
#define GPSLED 2

#define __OIL_CUT__
/*****************************************************************************
* Typedef  Enum         枚举
*****************************************************************************/
#define  TEST_KEY_MAX    4
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

kal_uint32 track_test_acc_flag(kal_uint32 flag);
static void track_ft_led_off(LED_Type Type);
kal_uint8 key_flag[TEST_KEY_MAX] = {0};
/*****************************************************************************
 *  Global Functions            全局函数
*****************************************************************************/
extern void ControlVibration(void);

void gpio_433M(kal_uint32 mode)
{
#if defined(__433M__)
    static kal_uint32 status =0;
    kal_uint32 exe = 0;
    LOGD(L_TEST, L_V5, " mode = %d",mode);
    if(mode == 99)
    {
        status=1;
    }
    else if(mode == 98)
    {
        status =0;
    }
    else if(status == 1)
    {
        LOGD(L_TEST, L_V5, "不需自动切换433 GPIO");
    }
    else
    {
        LOGD(L_TEST, L_V5, "自动切换433 GPIO");
        GPIO_ModeSetup(AP_433_GPIO, 0);
        GPIO_InitIO(1, AP_433_GPIO);
        GPIO_WriteIO(mode, AP_433_GPIO);
        exe = GPIO_ReadIO(AP_433_GPIO);
        if (mode != exe)
        {
            LOGS("433M Error!");
        }
    }
#endif /* __433M__ */
}


static void led_site_restoration(void)
{
    track_ft_led_off(LED_Num_Max);
    gpio_433M(0);
}

static void track_ft_led_on(LED_Type Type)
{
    if (Type < LED_Num1 || Type > LED_Num_Max)
    {
        LOGC(L_TEST, L_V4, "track_ft_led_on Type Error!");
        return;
    }
    track_drv_led_status_lock(0);

    if (Type == LED_Num_Max)
    {
        track_drv_led_set_all_on();
    }
    else
    {
        track_drv_led_set_status(Type, LedMode_ON);
    }
    track_drv_led_status_lock(1);

}

static void track_ft_led_off(LED_Type Type)
{
    if (Type < LED_Num1 || Type > LED_Num_Max)
    {
        LOGC(L_TEST, L_V4, "track_ft_led_on Type Error!");
        return;
    }
    track_drv_led_status_lock(0);

    if (Type == LED_Num_Max)
    {
        track_drv_led_set_all_off();
    }
    else
    {
        track_drv_led_set_status(Type, LedMode_OFF);
    }
    track_drv_led_status_lock(1);

}
void track_makecall_test(CMD_DATA_STRUCT * Cmds)
{
    LOGC(L_TEST, L_V4, "track_makecall_test= %d", Cmds->part);
    if (Cmds->part == 1)
    {
        LOGC(L_TEST, L_V4, "track_makecall_test= %s", Cmds->pars[1]);
        track_drv_sys_l4cuem_set_audio_mode(track_status_audio_mode(0xFF));
        track_make_call((char *) Cmds->pars[1]);
    }
    else
    {
        sprintf(Cmds->rsp_msg, "ERROR:100");
    }
}
/******************************************************************************
 * FUNCTION:  - track_set_testmode(kal_bool mode)
 * DESCRIPTION: - 设置当前是否是测试模式
 * Input:
 * Output:
 * Returns:
 * modification history
 * --------------------
 * v1.0  , 18-08-2012,  ZengPing & Liujw  written
 * --------------------
 ******************************************************************************/
void track_set_testmode(kal_uint8 mode)
{
    track_drv_uart_log_mode(2);
    track_status_test(1);
    track_ft_led_off(LED_Num_Max);
    LOGC(L_TEST, L_V4, "track_set_testmode = %d", track_status_test(2));
}

kal_bool track_is_testmode(void)
{
    return track_status_test(2);
}

void track_query_sim_type(kal_int8 sim_type)
{
    if (sim_type == 2)
    {
        track_set_testmode(1);
    }
}
extern void track_send_at_accept_call(void);
void track_TM_call_status_change(kal_uint16 callType, kal_uint16 callState)
{
    LOGD(L_TEST, L_V5, "type:%d,status:%d", callType, callState);
    if (callType == CALL_MT_IN)
    {
        switch (callState)
        {
                LOGD(L_TEST, L_V5, "2 type:%d,status:%d", callType, callState);
            case TR_CALL_RING:
                LOGD(L_TEST, L_V5, "3 type:%d,status:%d", callType, callState);
                track_ft_led_off(LED_Num_Max);
#if defined( __GT530__)
                tr_start_timer(TRACK_CUST_SOS_IN_CALL_TIMER, 3000, track_send_at_accept_call);
#else
                track_send_at_accept_call();
#endif /* __GT530__ */
                custom_cmd_send("RING_OK", KAL_FALSE);//RING
                break;
            case TR_CALL_CONNECT:
                track_drv_led_set_status(GSMLED, LedMode_ON);
                custom_cmd_send("THROUGH", KAL_FALSE);
                break;
            case TR_CALL_DISCONNECT:
                custom_cmd_send("DISCONNECT", KAL_FALSE);
                break;
            default:
                break;
        }
    }
    else if (callType == CALL_MO_OUT)
    {
        switch (callState)
        {
            case TR_CALL_RING:
                track_ft_led_off(LED_Num_Max);
                custom_cmd_send("RING_OK", KAL_FALSE);
                break;
            case TR_CALL_CONNECT:
                track_ft_led_off(LED_Num_Max);
                track_ft_led_on(GSMLED);
                custom_cmd_send("THROUGH", KAL_FALSE);
                break;
            case TR_CALL_DISCONNECT:
                custom_cmd_send("DISCONNECT", KAL_FALSE);
                break;
            default:
                break;
        }
    }
}
void track_cust_call_status_change_test(void* msg)
{
    call_state_struct *ptr = (call_state_struct *) msg;
    kal_uint16 _type = ptr->type;/*call_type_enum*/
    kal_uint16 callState = ptr->status;/*track_call_state_enum*/
    LOGD(L_TEST, L_V5, "type:%d,status:%d", ptr->type, ptr->status);

    track_TM_call_status_change(_type, callState);
}

static void Sensor_Callback_Test(void)
{
    LOGC(L_TEST, L_V4, "SENSOR OK3#");
    custom_cmd_send("Sensor_OK", KAL_FALSE);

    track_ft_led_on(LED_Num_Max);
    gpio_433M(1);
    tr_start_timer(TRACK_FACTORY_RSTORE_LED_TIME_ID, 1000, led_site_restoration);
}

void track_cust_sensor_test_init(void)
{
    //track_ft_led_all_off();
    LOGC(L_TEST, L_V4, "SENSOR INIT#");
    track_drv_sensor_on(3, Sensor_Callback_Test);
}

/******************************************************************************
 *  Function    -  track_test_acc_flag
 *
 *  Purpose     -  得出当前ACC状态
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 08-03-2013,  ZengPing & Liujw  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint32 track_test_acc_flag(kal_uint32 flag)
{
    static int status = 0;
    if (flag == 0 || flag == 1)
    {
        status = flag;
    }
    return status;
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
static void track_test_acc_high_mode(void)
{
    LOGC(L_TEST, L_V4, "ACC IS HIGH ^ Realys ON");
    track_stop_timer(TRACK_FACTORY_RSTORE_LED_TIME_ID);
    #if !defined(__GT420D__)
    LOGS("ACC_H");
    #endif
    track_test_acc_flag(1);
    //track_ft_led_on(LED_Num_Max);
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
static void track_test_acc_low_mode(void)
{
    #if !defined(__GT420D__)
    LOGC(L_TEST, L_V4, "ACC IS LOW V Realys OFF");
    custom_cmd_send("ACC_L", KAL_FALSE);
    #endif
    track_test_acc_flag(0);
    //track_ft_led_off(LED_Num_Max);
    //tr_start_timer(TRACK_FACTORY_KEY_CHECK_TIME_ID, 200,track_test_key_set_down);
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
void track_test_set_acc_status(kal_uint8 status)
{
    LOGC(L_TEST, L_V4, "%d", status);
    if (status)
    {
        tr_start_timer(TRACK_FACTORY_ACC_CHECK_TIME_ID, 500, track_test_acc_low_mode);
    }
    else
    {
        tr_start_timer(TRACK_FACTORY_ACC_CHECK_TIME_ID, 500, track_test_acc_high_mode);
    }
}

void track_test_set_key_status(kal_uint8 eintno, kal_uint8 status)
{
    LOGC(L_TEST, L_V4, "%d,%d", eintno, status);
    if (status == 1)
    {
        track_ft_led_off(LED_Num_Max);
        LOGS("EINT:%d,LEVEL:%d,KEY_OFF", eintno, status);
    }
    else
    {
        track_ft_led_on(LED_Num_Max);
        LOGS("EINT:%d,LEVEL:%d,KEY_ON", eintno, status);
    }
}
/******************************************************************************
 *  Function    -  track_test_Akey_high_mode *
 *  Purpose     -   高处理函数 *
 *  Description -  return Null *
 *  modification history
 * ----------------------------------------
 * v1.0  , 18-08-2012,  ZengPing & Liujw  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_Akey_high_mode(void)
{
    LOGC(L_TEST, L_V4, "KEY IS HIGH ^ LED ON ");
    //track_save_scene();
    track_ft_led_on(LED_Num_Max);
    track_cust_sensor_test_init();
}
/******************************************************************************
 *  Function    -  track_test_Akey_low_mode *
 *  Purpose     -   低处理函数 *
 *  Description -  return Null *
 *  modification history
 * ----------------------------------------
 * v1.0  , 18-08-2012,  ZengPing & Liujw  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_Akey_low_mode(void)
{
    //LOGC(L_TEST, L_V4, "KEY IS LOW V LED OFF ");
    track_ft_led_off(LED_Num_Max);

}
static void track_test_key_set_callback(void)
{
    static kal_uint32 key_flag = 0;
    LOGC(L_TEST, L_V4, "KEY IS LOW V LED OFF ");
    if (key_flag == 0)
    {
        track_ft_led_on(LED_Num_Max);
        LOGS("LEDKEY_ON");
        key_flag = 1;
    }
    else
    {
        track_ft_led_off(LED_Num_Max);
        LOGS("LEDKEY_OFF");
        key_flag = 0;
    }
}
static void cmd_test_set_keyflag(kal_uint32 key)
{
    int i = 0;
    LOGD(L_TEST, L_V4, "");
    if (key == 99)
    {
        memset(&key_flag, 0, sizeof(key_flag));
    }
    else if (key == 88)
    {
        for (i = 0; i < TEST_KEY_MAX; i++)
        {
            if (key_flag[i] == 0)
            {
                return;
            }
        }
        LOGS("KEY_ALL_OK");
        ControlVibration();
    }
    else
    {
        key_flag[key] = 1;
    }
}
static void track_test_key0_set_callback(void)
{
    cmd_test_set_keyflag(0);
    LOGD(L_TEST, L_V4, "");
    LOGS("KEY0_OK");
    cmd_test_set_keyflag(88);
}
static void track_test_key1_set_callback(void)
{
    cmd_test_set_keyflag(1);
    LOGD(L_TEST, L_V4, "");
    LOGS("KEY1_OK");
    cmd_test_set_keyflag(88);
}
static void track_test_key2_set_callback(void)
{
    cmd_test_set_keyflag(2);
    LOGD(L_TEST, L_V4, "");
    LOGS("KEY2_OK");
    cmd_test_set_keyflag(88);
}
static void track_test_key3_set_callback(void)
{
    cmd_test_set_keyflag(3);
    LOGD(L_TEST, L_V4, "");
    LOGS("KEY3_OK");
    cmd_test_set_keyflag(88);
}
static void cmd_test_allkey(void)
{

}
void track_test_key_set_init(void)
{
#if defined(__KEYPAD_MOD__)
#if defined (__GT03D__)  || defined(__GW100__)
    cmd_test_set_keyflag(99);
    track_drv_set_key_handler(track_test_key0_set_callback, KEY_SOS, KEY_EVENT_DOWN);
    track_drv_set_key_handler(track_test_key1_set_callback, KEY_TR1, KEY_EVENT_DOWN);
    track_drv_set_key_handler(track_test_key2_set_callback, KEY_TR2, KEY_EVENT_DOWN);
    track_drv_set_key_handler(track_test_key3_set_callback, KEY_FUN, KEY_EVENT_DOWN);

    track_drv_set_key_handler(cmd_test_allkey, KEY_SOS, KEY_SHORT_PRESS);
    track_drv_set_key_handler(cmd_test_allkey, KEY_TR1, KEY_SHORT_PRESS);
    track_drv_set_key_handler(cmd_test_allkey, KEY_TR2, KEY_SHORT_PRESS);
    track_drv_set_key_handler(cmd_test_allkey, KEY_FUN, KEY_SHORT_PRESS);

    track_drv_set_key_handler(cmd_test_allkey, KEY_SOS, KEY_LONG_PRESS);
    track_drv_set_key_handler(cmd_test_allkey, KEY_TR1, KEY_LONG_PRESS);
    track_drv_set_key_handler(cmd_test_allkey, KEY_TR2, KEY_LONG_PRESS);
    track_drv_set_key_handler(cmd_test_allkey, KEY_FUN, KEY_LONG_PRESS);
#else
 //   track_drv_set_key_handler(track_test_key_set_callback, KEY_SOS, KEY_EVENT_DOWN);
#endif /* __GT300__ */
#endif /* __KEYPAD_MOD__ */
}

void track_testmode_eint_handler(U8 eint, U8 level)
{
#ifdef __USE_SPI__
    	kal_uint8 buffer[50]=0;
#endif /* __USE_SPI__ */

    switch (eint)
    {
        case ACC_EINT_NO:
            track_test_set_acc_status(level);
            break;
            // case AUX_EINT_NO:
            // break;
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
        case AUX_EINT_NO:
            track_test_set_key_status(eint, level);
            break;
#endif /* __KEY_EINT__ */
        default:
            break;
    }
}
void track_test_ALLLED(char* part)
{
    if (strcmp((S8*)part, "ON") == 0)
    {
        track_ft_led_on(LED_Num_Max);
        LOGS("ALLLED_ON_OK");
    }
    else if (strcmp((S8*)part, "OFF") == 0)
    {
        track_ft_led_off(LED_Num_Max);
        LOGS("ALLLED_OFF_OK");
    }
}

