/******************************************************************************
 * track_test_pcba_drv.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        Ӳ��PCBA ����
 *
 *     <<�ο�Ӳ��PCBA����ָ��淶˵��.pdf >>
 *
 * modification history
 * --------------------
 * v1.0   2012-07-27,  chengjun create this file
 *
 ******************************************************************************/
#if defined (__TEST_PCBA__)

/****************************************************************************
* Include Files                   		����ͷ�ļ�
*****************************************************************************/
#include "track_drv.h"
//#include "init_public.h"
#include "track_soc_Lcon.h"
#include "track_os_ell.h"
#include "track_cust.h"
//#include "track_os_string.h"
/*****************************************************************************
 *  Define					�궨��
*****************************************************************************/

#define TEST_PCBA_CMD_LEN_MAX  11
#define TEST_PCBA_EINT_LIST_MAX  32

//static char *blank = {""}, *s_ON = {"ON"}, *s_OFF = {"OFF"}, *s_Error = {"ERROR"};
//#define ON_OFF(value) (value==0?s_OFF:(value==1?s_ON:s_Error))

/*****************************************************************************
* Typedef  Enum
*****************************************************************************/
typedef enum
{
    TEST_PCBA_LED,
    TEST_PCBA_VIB,
    TEST_PCBA_GPS,
    TEST_PCBA_SENSOR,
    TEST_PCBA_GPIO,
    TEST_PCBA_EINT,
    TEST_PCBA_LBS,
    TEST_PCBA_BB,
    TEST_PCBA_PMU,
    TEST_PCBA_GPRS,
    TEST_PCBA_KEY,
    TEST_PCBA_WDT,
    TEST_PCBA_LCD,
    TEST_PCBA_OBD,
    TEST_PCBA_REC,
    TEST_PCBA_LTE,
    TEST_PCBA_VMC,
    TEST_PCBA_DEBUG,
    TEST_PCBA_SLEEP,
    TEST_PCBA_BT,
    TEST_PCBA_RS485,
    TEST_PCBA_UART,
    TEST_PCBA_MCU,
    TEST_PCBA_END,
} test_pcba_enum;

/*****************************************************************************
 *  Struct					���ݽṹ����
*****************************************************************************/
typedef struct
{
    kal_uint8 len;
    //part1 ��test_pcba_tab[] ������Ҫ����
    char part2[TEST_PCBA_CMD_LEN_MAX];
    char part3[TEST_PCBA_CMD_LEN_MAX];
} test_pcba_cmd_struct;

typedef struct
{
    kal_bool eint_output;
    kal_bool adc_output;
} test_pcba_output_struct;

/*****************************************************************************
* Local variable				�ֲ�����
*****************************************************************************/
static kal_uint8 test_gps_fix_time = 0;
//0=������
//1=����AGPS��λʱ��
//2=����GPS������ʱ��
//3=����GPS������ʱ��

static test_pcba_cmd_struct test_pcba_cmd = {0};
static test_pcba_output_struct test_paba_output = {0};
static char test_pcba_eint_list[TEST_PCBA_EINT_LIST_MAX] = {0};

//strlen ����С��100
static const char test_pcba_cmd_not_exist[] = {"Error:this command is not exist ,please use 'AT^GT_CM=PCBA' for help "};
static const char test_pcba_cmd_sw_not_support[] = {"Sorry,the software does not support this function"};
static const char test_pcba_cmd_hw_not_support[] = {"Sorry,the hardware does not support this function"};
static const char test_pcba_debug_not_exist[] = {"Warning:this debug command is not exist "};
/****************************************************************************
* Global Variable - Extern          ȫ�ֱ���
*****************************************************************************/

/****************************************************************************
* Global Variable - Extern              ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
 *  Local Functions			���غ���
*****************************************************************************/

/******************************************************************************
 *  Function    -  track_test_output_datetime
 *
 *  Purpose     -  �����ǰʱ������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 22-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_output_datetime(void)
{
    t_rtc rtcTime = {0};
    RTC_GetTimeOnly(&rtcTime);
    LOGS("DateTime (UTC): %d-%02d-%02d  %02d:%02d:%02d", \
         rtcTime.rtc_year + 2000, rtcTime.rtc_mon, rtcTime.rtc_day, rtcTime.rtc_hour, rtcTime.rtc_min, rtcTime.rtc_sec);
}

/******************************************************************************
 *  Function    -  track_test_gpstc_switch
 *
 *  Purpose     -  GPS����͸�������л�
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 21-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static kal_bool track_test_gpstc_switch(void)
{
    if(track_status_gpstc(2) == 0)
    {
        track_status_gpstc(1);
        return TRUE;
    }
    else
    {
        track_status_gpstc(0);
        return FALSE;
    }
}


/******************************************************************************
 *  Function    -  track_test_pcba_unit_led
 *
 *  Purpose     -  LED ����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_unit_led(char* part2, char* part3)
{
    char result[100] = {0};

    if(strcmp((S8*)part2, "OFF") == 0)
    {
        //track_drv_led_set_all_off();
        track_drv_led_set_all(KAL_FALSE);
        sprintf(result, "All led off ");
    }
    else if(strcmp((S8*)part2, "ON") == 0)
    {
        //track_drv_led_set_all_on();
        track_drv_led_set_all(KAL_TRUE);
        sprintf(result, "All led on ");
    }
    else if(strcmp((S8*)part2, "R") == 0)
    {
        if(strcmp((S8*)part3, "ON") == 0)
        {
            track_drv_test_led_switch(LED_PWR, LedMode_ON);
            sprintf(result, "Red led on ");
        }
        else if(strcmp((S8*)part3, "OFF") == 0)
        {
            track_drv_test_led_switch(LED_PWR, LedMode_OFF);
            sprintf(result, "Red led off ");
        }
        else
        {
            sprintf(result, "%s", test_pcba_cmd_not_exist);
        }
    }
    else if(strcmp((S8*)part2, "G") == 0)
    {
        if(strcmp((S8*)part3, "ON") == 0)
        {
            track_drv_test_led_switch(LED_GSM, LedMode_ON);
            sprintf(result, "Green led on ");
        }
        else if(strcmp((S8*)part3, "OFF") == 0)
        {
            track_drv_test_led_switch(LED_GSM, LedMode_OFF);
            sprintf(result, "Green led off ");
        }
        else
        {
            sprintf(result, "%s", test_pcba_cmd_not_exist);
        }
    }
    else if(strcmp((S8*)part2, "B") == 0)
    {
        if(strcmp((S8*)part3, "ON") == 0)
        {
            track_drv_test_led_switch(LED_GPS, LedMode_ON);
            sprintf(result, "Blue led on ");
        }
        else if(strcmp((S8*)part3, "OFF") == 0)
        {
            track_drv_test_led_switch(LED_GPS, LedMode_OFF);
            sprintf(result, "Blue led off ");
        }
        else
        {
            sprintf(result, "%s", test_pcba_cmd_not_exist);
        }
    }
    else if(strcmp((S8*)part2, "E") == 0)
    {
        if(strcmp((S8*)part3, "ON") == 0)
        {
            track_drv_test_led_switch(LED_OBD, LedMode_ON);
            sprintf(result, "Orange led on ");
        }
        else if(strcmp((S8*)part3, "OFF") == 0)
        {
            track_drv_test_led_switch(LED_OBD, LedMode_OFF);
            sprintf(result, "Orange led off ");
        }
        else
        {
            sprintf(result, "%s", test_pcba_cmd_not_exist);
        }
    }
    else if(strcmp((S8*)part2, "KP") == 0)
    {
        sprintf(result, "%s", test_pcba_cmd_hw_not_support);
    }
    else if(strcmp((S8*)part2, "FLASH") == 0)
    {
        sprintf(result, "%s", "Power led flash");
        track_drv_test_led_switch(LED_PWR, LedMode_MedFlash);
    }
	else if(strcmp((S8*)part2, "TWO") == 0)
    {
		extern LED_TWO_COLOR_StatusSet(U8 status);
		int t = atoi(part3);
        sprintf(result, "TEST %d", t);
		LED_TWO_COLOR_StatusSet(t);
    }
    else
    {
        sprintf(result, "%s", test_pcba_cmd_not_exist);
    }

    LOGS("%s", result);
}

/******************************************************************************
 *  Function    -  track_test_pcba_unit_vib
 *
 *  Purpose     -  ��������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 21-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_unit_vib(char* part2, char* part3)
{
    char result[100] = {0};

    if(strcmp((S8*)part2, "OFF") == 0)
    {
        track_drv_set_vibr(KAL_FALSE);
        sprintf(result, "Vibration off");
    }
    else if(strcmp((S8*)part2, "ON") == 0)
    {
        track_drv_set_vibr(KAL_TRUE);
        sprintf(result, "Vibration on");
    }
    else
    {
        sprintf(result, "%s", test_pcba_cmd_not_exist);
    }

    LOGS("%s", result);
}

/******************************************************************************
 *  Function    -  track_test_display_delay_on/off
 *
 *  Purpose     -  LED ȫ���������Զ�Ϩ��
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_display_delay_off()
{
    track_drv_led_set_all_off();
}

static void track_test_display_delay_on(kal_uint32 ms)
{
    track_drv_led_set_all_on();
    tr_start_timer(DISPLAY_TEST_TIMER_ID, ms, track_test_display_delay_off);
}

/******************************************************************************
 *  Function    -  track_test_pcba_gps_fix_time
 *
 *  Purpose     -  ���GPS��������λ��ʱ��
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 27-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_gps_fix_time_output(kal_bool output)
{
    static kal_uint32 start_tick = 0;
    kal_uint32 fix_tick, time_diff;

    if(!output)
    {
        start_tick = kal_get_systicks();
    }
    else
    {
        fix_tick = kal_get_systicks();
        time_diff = kal_ticks_to_secs(fix_tick - start_tick);

        if(test_gps_fix_time == 1)
        {
            LOGS("GPS positioning success (default start TTFF=%ds) ", time_diff);
        }
        else if(test_gps_fix_time == 2)
        {
            LOGS("GPS positioning success (hot TTFF=%ds) ", time_diff);
        }
        else if(test_gps_fix_time == 3)
        {
            LOGS("GPS positioning success (cold TTFF=%ds) ", time_diff);
        }
        else if(test_gps_fix_time == 4)
        {
            LOGS("GPS positioning success (warm TTFF=%ds) ", time_diff);
        }
        test_gps_fix_time = 0;
    }
}

/******************************************************************************
 *  Function    -  track_test_pcba_unit_gps
 *
 *  Purpose     -  GPS ����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_unit_gps(char* part2, char* part3)
{
    char result[150] = {0};
    int8 exe;
    U8 status = 0;
    mtk_param_epo_stage_cfg epo_stage = {0};

    if(strcmp((S8*)part2, "OFF") == 0)
    {
#if defined( __GT530__)
        track_drv_mgps_off();
        exe = 1;
#else
        exe = track_drv_gps_switch(0);
#endif /* __GT530__ */

        if(exe == -1)
        {
            sprintf(result, "GPS had off ");
        }
        else
        {
            sprintf(result, "GPS off ");
        }

    }
    else if(strcmp((S8*)part2, "ON") == 0)
    {
#if defined( __GT530__)
        track_drv_mgps_on();
        exe = 1;
#else
        exe = track_drv_gps_switch(1);
#endif /* __GT530__ */

        if(exe == -1)
        {
            sprintf(result, "GPS had on ");
        }
        else if(exe == -3)
        {
            sprintf(result, "GPS EPO disable ");
        }
        else
        {
            sprintf(result, "GPS on ");
            test_gps_fix_time = 1;
            track_test_pcba_gps_fix_time_output(KAL_FALSE);
        }

    }
    else if(strcmp((S8*)part2, "GLP") == 0)
    {
        extern nvram_realtime_struct  G_realtime_data;
        if(strcmp((S8*)part3, "ON") == 0)
        {
            G_realtime_data.glp = 1;
        }
        else
        {
            G_realtime_data.glp = 0;
        }
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

    }
    else if(strcmp((S8*)part2, "HOT") == 0)
    {
        sprintf(result, "%s", "GPS hot start (send cmd for restart)");
        track_drv_gps_hot_restart();
        test_gps_fix_time = 2;
        track_test_pcba_gps_fix_time_output(KAL_FALSE);

    }
    else if(strcmp((S8*)part2, "WARM") == 0)
    {
        sprintf(result, "%s", "GPS warm start (send cmd for warm restart)");
        track_drv_gps_warm_restart();
        test_gps_fix_time = 4;
        track_test_pcba_gps_fix_time_output(KAL_FALSE);
    }
    else if(strcmp((S8*)part2, "COLD") == 0)
    {
        sprintf(result, "%s", "GPS cold start (send cmd for COLD restart)");
        track_drv_gps_cold_restart();
        test_gps_fix_time = 3;
        track_test_pcba_gps_fix_time_output(KAL_FALSE);
        track_drv_cal_secset(1);
    }
    else if(strcmp((S8*)part2, "STATUS") == 0)
    {
        status = track_drv_get_gps_data()->status;
        if((status == 2) || (status == 3))
        {
            snprintf(result, sizeof(result), "GPS %dD Locate ,star=%d\r\nDateTime:%0.4d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d\r\nLocate:%c %f,%c %f,speed=%0.2fkm/h", \
                     status, track_drv_get_gps_data()->gpgsv.Satellites_In_View, \
                     track_drv_get_gps_data()->gprmc.Date_Time.nYear, track_drv_get_gps_data()->gprmc.Date_Time.nMonth, \
                     track_drv_get_gps_data()->gprmc.Date_Time.nDay, track_drv_get_gps_data()->gprmc.Date_Time.nHour, \
                     track_drv_get_gps_data()->gprmc.Date_Time.nMin, track_drv_get_gps_data()->gprmc.Date_Time.nSec, \
                     track_drv_get_gps_data()->gprmc.NS, track_drv_get_gps_data()->gprmc.Latitude, \
                     track_drv_get_gps_data()->gprmc.EW, track_drv_get_gps_data()->gprmc.Longitude,
                     track_drv_get_gps_data()->gprmc.Speed);
        }
        else if(status == 1)
        {
            snprintf(result, sizeof(result), "GPS searching satellite (star=%d)", track_drv_get_gps_data()->gpgsv.Satellites_In_View);
        }
        else
        {
            sprintf(result, "GPS closed");
        }
    }
    else if(strcmp((S8*)part2, "GLP") == 0)
    {
        U8 i = atoi(part3);
        track_drv_set_gps_glp_mode(i);
    }
    else if(strcmp((S8*)part2, "EPO") == 0)
    {
        epo_stage = track_drv_get_epo_stage();
        sprintf(result, "AGPS EPO stage : %08X", epo_stage.u4EpoStage);
    }
    else if(strcmp((S8*)part2, "TC") == 0)
    {
#if defined( __GT530__)
        if(part3[0] == '0')
        {
            track_drv_lte_gpstc(0);
            sprintf(result, "%s", "GPS pass througt close");
        }
        else
        {
            track_drv_lte_gpstc(1);
            sprintf(result, "%s", "GPS pass througt open");
        }

        return;
#endif /* __GT530__ */
        if(track_test_gpstc_switch())
        {
            sprintf(result, "%s", "GPS pass througt open");
        }
        else
        {
            sprintf(result, "%s", "GPS pass througt close");
        }

    }
    else if(strcmp((S8*)part2, "CLEAR") == 0)
    {
        track_drv_gps_claer_eph();
        sprintf(result, "Clear gps ephemeris");
    }
    else
    {
#if defined( __GPS_G3333__)||defined( __GPS_MT3333__)
        sprintf(result, "%s", "Current GPS Module is:GPS_G3333");
#elif defined(__GPS_MT3332__)
        sprintf(result, "%s", "Current GPS Module is:GPS_G3332");
#elif defined(__GPS_G3337__)
        sprintf(result, "%s", "Current GPS Module is:GPS_G3337");
#elif defined(__GPS_MT3336__)
        sprintf(result, "%s", "Current GPS Module is:GPS_MT3336");
#elif defined(__GPS_G7020__)
        sprintf(result, "%s", "Current GPS Module is:GPS_G7020");
#elif defined(__GPS_MT5631B__)
        sprintf(result, "%s", "Current GPS Module is:GPS_MT5631B");
#elif defined(__GPS_M10382__)
        sprintf(result, "%s", "Current GPS Module is:GPS_M10382");
        else
            sprintf(result, "%s", test_pcba_cmd_not_exist);
#endif /* __GPS_G3333__ */

    }

    LOGS("%s", result);

}


/******************************************************************************
 *  Function    -  track_test_sensor_count
 *
 *  Purpose     -  �𶯴���ͳ�Ʋ���
 *
 *  Description -   track_test_pcba_unit_sensor_callback --->> track_test_sensor_count(KAL_TRUE);
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 07-03-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_sensor_count(kal_bool enable)
{
    static kal_uint32 count = 0;

    if(enable)
    {
        LOGS("�� (%d) ,speed=%f", count, track_drv_get_gps_RMC()->Speed);
        count++;
    }
    else
    {
        track_drv_sensor_off();
        LOGS("ͳ�ƽ��: Level=%d , Count=%d", track_drv_get_sensor_level(), count);
        count = 0;
    }
}


/******************************************************************************
 *  Function    -  track_test_pcba_unit_sensor / track_test_pcba_unit_sensor_callback
 *
 *  Purpose     -  sensor ����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_unit_sensor_callback(void)
{
    LOGS("%s", "Sensor ok");
    track_test_display_delay_on(2000);
}

static void track_test_pcba_unit_sensor(char* part2, char* part3)
{
    sensor_type_enum sensor_type;
    kal_uint8 level = 0, threshold = 0;
    short x, y, z;
    char result[100] = {0};

    if(strcmp((S8*)part2, "OFF") == 0)
    {
        track_drv_sensor_off();
        sprintf(result, "Sensor off ");
    }
    else if(strcmp((S8*)part2, "ON") == 0)
    {
        level = atoi(part3);
        if((level == 0) || (level > 5))
        {
            level = 2;
        }
        track_drv_sensor_on(level, track_test_pcba_unit_sensor_callback);
        sprintf(result, "Sensor on  (%d)", level);
    }
    else if(strcmp((S8*)part2, "STATUS") == 0)
    {
        sensor_type = track_drv_get_sensor_type();
        level = track_drv_get_sensor_level();

        if((sensor_type <= SENSOR_NOT_READY) || (sensor_type >= SENSOR_TYPE_MAX))
        {
            sprintf(result, "Warning:sensor type not detected");
        }
        else
        {
            if(level != 0xFF)
            {
                switch(sensor_type)
                {
                    case SENSOR_BMA250:
                        sprintf(result, "Bosch bma250 (ID=0x03),level=%d", level);
                        break;
                    case SENSOR_BMA250E:
                        sprintf(result, "Bosch bma250E (ID=0xF9),level=%d", level);
                        break;
                    case SENSOR_BMA253:
                        sprintf(result, "Bosch bma253 (ID=0xFA),level=%d", level);
                        break;
                    case SENSOR_KXTJ31057:
                        sprintf(result, "Kionix kxtj3-1057 (ID=0x35),level=%d", level);
                        break;
                    case SENSOR_SC7A20:
                        sprintf(result, "Silan SC7A20 (ID=0x11),level=%d", level);
                        break;
                    case SENSOR_DA213:
                        sprintf(result, "Mira DA213 (ID=0x13),level=%d", level);
                        break;   
#if !defined (__REMOVE_OBSOLETE_GSENSOR__)
                    case SENSOR_MC3410:
                        sprintf(result, "mCube mc3410 (ID=0x02),level=%d", level);
                        break;
                    case SENSOR_AFA750:
                        sprintf(result, "Micro afa750 (ID=0x3C),level=%d", level);
                        break;
                    case SENSOR_MMA8452Q:
                        sprintf(result, "Freescale mma8452Q (ID=0x2A),level=%d", level);
                        break;
#endif /* __REMOVE_OBSOLETE_GSENSOR__ */
                    default:
                        sprintf(result, "I do not know this gSensor");
                        break;
                }
            }
            else
            {
                threshold = track_drv_get_sensor_threshold();
                switch(sensor_type)
                {
                    case SENSOR_BMA250:
                        sprintf(result, "Bosch bma250,threshold=%d", threshold);
                        break;
                    case SENSOR_BMA250E:
                        sprintf(result, "Bosch bma250E,threshold=%d", threshold);
                        break;
                    case SENSOR_BMA253:
                        sprintf(result, "Bosch bma253,threshold=%d", threshold);
                        break;
                    case SENSOR_KXTJ31057:
                        sprintf(result, "Kionix kxtj3-1057,threshold=%d", threshold);
                        break;
                    case SENSOR_SC7A20:
                        sprintf(result, "Silan SC7A20,threshold=%d", threshold);
                        break; 
                    case SENSOR_DA213:
                        sprintf(result, "Mira DA213,threshold=%d", threshold);
                        break;  
#if !defined (__REMOVE_OBSOLETE_GSENSOR__)
                    case SENSOR_MC3410:
                        sprintf(result, "mCube mc3410,threshold=%d", threshold);
                        break;
                    case SENSOR_AFA750:
                        sprintf(result, "Micro afa750,threshold=%d", threshold);
                        break;
                    case SENSOR_MMA8452Q:
                        sprintf(result, "Freescale mma8452Q,threshold=%d", threshold);
                        break;
#endif /* __REMOVE_OBSOLETE_GSENSOR__ */
                        
                    default:
                        sprintf(result, "I do not know this gSensor ");
                        break;
                }
            }
        }
    }
    else if(strcmp((S8*)part2, "TEST") == 0)
    {
        threshold = atoi(part3);
        threshold = threshold & 0xFF;

        track_drv_sensor_threshold_start(threshold, track_test_pcba_unit_sensor_callback);
        sprintf(result, "Sensor start  for software test threshold (%d)", threshold);
    }
    else
    {
        sprintf(result, "%s", test_pcba_cmd_not_exist);
    }

    LOGS("%s", result);
}

/******************************************************************************
 *  Function    -  track_test_pcba_unit_gpio
 *
 *  Purpose     -  GPIO ����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_unit_gpio(char* part2, char* part3)
{
    char result[100] = {0};
    kal_uint8 gpio;
    char output, exe = 0;

    if(test_pcba_cmd.len != 3 && test_pcba_cmd.len != 2)
    {
        sprintf(result, "%s", test_pcba_cmd_not_exist);
        LOGS("%s", result);
        return;
    }

    gpio = atoi(part2);

    if(strcmp((S8*)part3, "H") == 0)
    {
        output = 1;
        GPIO_ModeSetup(gpio, 0);
        GPIO_InitIO(1, gpio);
        GPIO_WriteIO(output, gpio);
        exe = track_GPIO_ReadIO(gpio);
    }
    else if(strcmp((S8*)part3, "L") == 0)
    {
        output = 0;
        GPIO_ModeSetup(gpio, 0);
        GPIO_InitIO(1, gpio);
        GPIO_WriteIO(output, gpio);
        exe = track_GPIO_ReadIO(gpio);
    }
	else if(strcmp((S8*)part3, "READ") == 0)
	{
        exe = GPIO_ReadIO(gpio);
        sprintf(result, "READ GPIO%d:%d:%d", gpio, exe,track_GPIO_ReadIO(gpio));
        LOGS("%s", result);
        return;
	}
    else
    {
        GPIO_ModeSetup(gpio, 0);
        GPIO_InitIO(1, gpio);
        exe = track_GPIO_ReadIO(gpio);
        sprintf(result, "GPIO%d:%d %d", gpio, exe, OTA_GPIO_ReturnDout(gpio));
        LOGS("%s", result);
        return;
    }
    /*else
    {
        sprintf(result, "%s", test_pcba_cmd_not_exist);
        LOGS("%s", result);
        return;
    }*/

    if(exe == output)
    {
        if(output == 0)
        {
            //track_drv_led_set_all_off();
        }
        else
        {
            //track_drv_led_set_all_on();
        }
        sprintf(result, "GPIO-%d output %s ok", gpio, part3);
    }
    else
    {
        track_test_display_delay_on(1000);
        sprintf(result, "Warn:GPIO-%s output %s fail", part2, part3);
    }

    LOGS("%s", result);
}


/******************************************************************************
 *  Function    -  track_test_pcba_eint_status
 *
 *  Purpose     -  ���ȫ����������ж�״̬
 *
 *  Description -  ��Щ�ж���ʹ�ã�����û���������򲻻����
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 22-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_eint_status(void)
{
    kal_uint8 i;
    kal_bool exist = KAL_FALSE;

    LOGS("%s", "The interrupt status after the trigger:");
    for(i = 0; i < TEST_PCBA_EINT_LIST_MAX; i++)
    {
        if(test_pcba_eint_list[i] != 0)
        {
            LOGS("Eint%d,Level=%c", i, test_pcba_eint_list[i]);
            exist = KAL_TRUE;
        }
    }

    if(!exist)
    {
        LOGS("No interrupt trigger");
    }
}


/******************************************************************************
 *  Function    -  track_test_pcba_unit_eint
 *
 *  Purpose     -  ģ���жϴ���
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_unit_eint(char* part2, char* part3)
{
    char result[100] = {0};
    kal_uint8 eint_id;
    char level;

    if(test_pcba_cmd.len == 2)
    {
        if(strcmp((S8*)part2, "STATUS") == 0)
        {
            track_test_pcba_eint_status();
            return;
        }
        else if(strcmp((S8*)part2, "AUTO") == 0)
        {
            if(test_paba_output.eint_output)
            {
                test_paba_output.eint_output = KAL_FALSE;
                sprintf(result, "%s", "Interrupt status automatic output disable");
            }
            else
            {
                test_paba_output.eint_output = KAL_TRUE;
                sprintf(result, "%s", "Interrupt status automatic output enable");
            }

            LOGS("%s", result);
            return;
        }
        else
        {
            sprintf(result, "%s", test_pcba_cmd_not_exist);
            LOGS("%s", result);
            return;
        }
    }
    else
    {
        eint_id = atoi(part2);
        if(eint_id >= TEST_PCBA_EINT_LIST_MAX)
        {
            sprintf(result, "Warn:please check the interrupt number");
            LOGS("%s", result);
            return;
        }

        if(strcmp((S8*)part3, "H") == 0)
        {
            level = 1;
        }
        else if(strcmp((S8*)part3, "L") == 0)
        {
            level = 0;
        }
        else
        {
            sprintf(result, "%s", test_pcba_cmd_not_exist);
            LOGS("%s", result);
            return;
        }

        sprintf(result, "Forced to trigger [Eint%d ,Level=%s] for sw test", eint_id, part3);
        LOGS("%s", result);

        track_drv_eint_trigger_response(eint_id, level);
    }
}


/******************************************************************************
 *  Function    -  track_test_pcba_unit_lbs
 *
 *  Purpose     -  LBS ����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_unit_lbs(char* part2, char* part3)
{
    char result[150] = {0};
    int8 exe;
    U8 status = 0;

    if(strcmp((S8*)part2, "OFF") == 0)
    {
        exe = track_drv_lbs_info_switch(0);
        if(exe == -1)
        {
            sprintf(result, "LBS had off ");
        }
        else
        {
            sprintf(result, "LBS off ");
        }
    }
    else if(strcmp((S8*)part2, "ON") == 0)
    {
        exe = track_drv_lbs_info_switch(1);
        if(exe == -1)
        {
            sprintf(result, "LBS had on ");
        }
        else
        {
            sprintf(result, "LBS on ");
        }
    }
    else if(strcmp((S8*)part2, "STATUS") == 0)
    {
        status = track_drv_get_lbs_data()->status;
        if(status)
        {
            snprintf(result, sizeof(result) - 1, "MCC=%d,MNC=%02d,TA=%d;Main[%d,%2d,%d];Near {[%2d,%d] [%2d,%d] [%2d,%d] [%2d,%d] [%2d,%d] [%2d,%d]}#",
                     track_drv_get_lbs_data()->MainCell_Info.mcc, track_drv_get_lbs_data()->MainCell_Info.mnc, track_drv_get_lbs_data()->ta,
                     track_drv_get_lbs_data()->MainCell_Info.lac, track_drv_get_lbs_data()->MainCell_Info.cell_id, track_drv_get_lbs_data()->MainCell_Info.rxlev,
                     track_drv_get_lbs_data()->NbrCell_Info[0].cell_id, track_drv_get_lbs_data()->NbrCell_Info[0].rxlev ,
                     track_drv_get_lbs_data()->NbrCell_Info[1].cell_id, track_drv_get_lbs_data()->NbrCell_Info[1].rxlev ,
                     track_drv_get_lbs_data()->NbrCell_Info[2].cell_id, track_drv_get_lbs_data()->NbrCell_Info[2].rxlev ,
                     track_drv_get_lbs_data()->NbrCell_Info[3].cell_id, track_drv_get_lbs_data()->NbrCell_Info[3].rxlev ,
                     track_drv_get_lbs_data()->NbrCell_Info[4].cell_id, track_drv_get_lbs_data()->NbrCell_Info[4].rxlev ,
                     track_drv_get_lbs_data()->NbrCell_Info[5].cell_id, track_drv_get_lbs_data()->NbrCell_Info[5].rxlev);
        }
        else
        {
            sprintf(result, "LBS data stop");
        }

    }
    else
    {
        sprintf(result, "%s", test_pcba_cmd_not_exist);
    }

    LOGS("%s", result);

}


/******************************************************************************
 *  Function    -  track_test_pcba_unit_bb
 *
 *  Purpose     -  �鿴ϵͳ����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 21-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_unit_bb(char* part2, char* part3)
{
    char result[100] = {0}, temp[5];
    char i;
    char *chip_id;
    kal_int8 sim_type;

    if(strcmp((S8*)part2, "DT") == 0)
    {
        track_test_output_datetime();
    }
    else if(strcmp((S8*)part2, "ID") == 0)
    {
        chip_id = track_drv_get_chip_id();
        sprintf(result, "%s", "chip RID:");
        for(i = 0; i < 16; i++)
        {
            memset(temp, 0x00, sizeof(temp));
            sprintf(temp, "%0.2X ", *(chip_id  + i));
            strcat(result, temp);
        }
        LOGS("%s", result);
        LOGS("IMEI:%s", track_drv_get_imei(0));
    }
    else if(strcmp((S8*)part2, "SIM") == 0)
    {
        sim_type = track_drv_get_sim_type();
        track_remind_sim_for_factory(sim_type);
        if(sim_type == 1)
        {
            LOGS("IMSI:%s", track_drv_get_imsi(0));
        }
    }
    else if(strcmp((S8*)part2, "SW") == 0)
    {
        snprintf(result, sizeof(result), "[VERSION]%s\r\n[BUILD]%s", release_verno(), build_date_time());
        LOGS("%s", result);
    }
    else if(strcmp((S8*)part2, "OFF") == 0)
    {
        track_drv_sys_power_off_req();
        LOGS("Sorry,BB is still on!");
    }
#if defined(__433M__)
    else if(strcmp((S8*)part2, "433") == 0)
    {

        if(strcmp((S8*)part3, "OFF") == 0)
        {
            track_drv_433_alarm((void*)0);
            LOGS("%s", "433M GPIO 35 _________");
        }
        else
        {
            track_drv_433_alarm((void*)4);
            LOGS("%s", "433M GPIO 35 VVVVVVVVV");
        }
        return;
    }
#endif
    else if(strcmp((S8*)part2, "SLEEP") == 0)
    {
        track_drv_sensor_off();
        track_drv_gps_switch(0);
        track_drv_led_set_all_off();
        track_drv_led_status_lock(1);

        sprintf(result, "Enter sleep mode (turn off GPS, LED, sensor and only keep GPRS work)");
    }
    else
    {
        sprintf(result, "%s", test_pcba_cmd_not_exist);
        LOGS("%s", result);
    }
}

#if defined( __EXT_VBAT_ADC__)
/******************************************************************************
 *  Function    -  track_test_pcba_adc_output
 *
 *  Purpose     -  �������ADCԭʼֵ
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 22-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_adc_output()
{
    if(!test_paba_output.adc_output)
    {
        return;
    }

    track_drv_view_ext_batter();

    tr_start_timer(PCBA_TEST_OUTPUT_TIMER_ID, 2000, track_test_pcba_adc_output);
}

static void track_test_get_voltbmp(kal_bool out)
{
    LOGS("ADC3 %d",track_drv_get_external_batter2());
    if(out)
    {
        tr_start_timer(PCBA_TEST_OUTPUT2_TIMER_ID, 2000, track_test_get_voltbmp);
    }
    else
    {
        track_stop_timer(PCBA_TEST_OUTPUT2_TIMER_ID);
    }
}
#endif /* __EXT_VBAT_ADC__ */

/******************************************************************************
 *  Function    -  track_test_pcba_unit_pmu
 *
 *  Purpose     -  �鿴���״̬
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 28-09-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_unit_pmu(char* part2, char* part3)
{
    char result[100] = {0};
    kal_int32 external_voltage;
    float external_batter;

    if(strcmp((S8*)part2, "BMT") == 0)
    {
        track_drv_bmt_charging_result(result);
        sprintf(result, "%s", result);
    }
    else if(strcmp((S8*)part2, "EXT") == 0)
    {
#if defined( __EXT_VBAT_ADC__)
        track_drv_view_ext_batter();
#else
        sprintf(result, "%s", test_pcba_cmd_hw_not_support);
#endif /* __EXT_VBAT_ADC__ */
    }
    else if(strcmp((S8*)part2, "ADC") == 0)
    {

#if defined( __EXT_VBAT_ADC__)
        if(test_paba_output.adc_output)
        {
            test_paba_output.adc_output = KAL_FALSE;
            track_drv_ext_vbat_measure_modify_parameters2(10, 6);
            tr_stop_timer(PCBA_TEST_OUTPUT_TIMER_ID);
            sprintf(result, "%s", "ADC data automatic output disable");
        }
        else
        {
            test_paba_output.adc_output = KAL_TRUE;
            track_drv_ext_vbat_measure_modify_parameters2(2, 1);
            tr_start_timer(PCBA_TEST_OUTPUT_TIMER_ID, 2000, track_test_pcba_adc_output);
            sprintf(result, "%s", "ADC data automatic output begian (uV)");
        }
#else
        sprintf(result, "%s", test_pcba_cmd_hw_not_support);
#endif /* __EXT_VBAT_ADC__ */

    }
#if defined( __EXT_VBAT_ADC__)
    else if(strcmp((S8*)part2, "ADCSET") == 0)
    {
        int channel =  atoi(part3);
        if(channel<5)
        {
            track_add_adc_vbat_measure2(5,4,channel);
            sprintf(result, "%s", "ADC%d SET OK!",channel);
        }
        else
        {
            sprintf(result, "%s", "ERROR ADC CHANNEL(1-4)");
        }
    } 
    else if(strcmp((S8*)part2, "ADC3") == 0)
    {
        static U8 i =0;
        if(i==0)
        {
            i++;
            track_test_get_voltbmp(KAL_TRUE);
        }
        else
        {
            i=0;
            track_test_get_voltbmp(KAL_FALSE);
        }
    }
#endif
    else if(strcmp((S8*)part2, "SLEEP") == 0)
    {
        track_drv_sensor_off();
        track_drv_gps_switch(0);
        track_drv_led_set_all_off();
        track_drv_led_status_lock(1);

        sprintf(result, "Enter sleep mode (turn off GPS, LED, sensor and only keep GPRS work)");
    }
    else
    {
        sprintf(result, "%s", test_pcba_cmd_not_exist);
    }
    LOGS("%s", result);
}

/******************************************************************************
 *  Function    -  track_test_pcba_gprs_send_always
 *
 *  Purpose     -  �رվ�̬���ˣ�GPRS �����ϱ�����Ӳ�����Ե���
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 30-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_gprs_send_always(void)
{
    track_cust_sf_switch(0);
    LOGS("%s", "GPRS data send always");
}


/******************************************************************************
 *  Function    -  track_test_pcba_gprs_off
 *
 *  Purpose     -  �ر�GPRS�����ҽ�ֹ��������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 30-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_gprs_off(void)
{
    track_soc_thorough_disconnect();
    track_cust_restart_disable();
    LOGS("%s", "Close GPRS and restart disable");
}

/******************************************************************************
 *  Function    -  track_test_pcba_gprs_status
 *
 *  Purpose     -  �鿴GPRS״̬
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 30-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_gprs_status(void)
{
    extern char *track_soc_view_server_addr(void);
    char result[150] = {0}, tmp[50] = {0};
    char *server;
    kal_uint8 mode;
    kal_int8 sim_type;
    char * imei;
    track_soc_connstatus_enum status;

    imei = track_drv_get_imei(0);
    if((strlen(imei) == 0) || (strcmp(imei, "358688000000158") == 0))
    {
        LOGS("%s", "Warn:please download IMEI");
        return;
    }

    sim_type = track_drv_get_sim_type();
    if(sim_type <= 0)
    {
        LOGS("%s", "Warn:please check the SIM card");
        return;
    }

    status = Socket_get_conn_status(0);
    server = track_soc_view_server_addr();

    if(status == SOC_CONN_SUCCESSFUL)
    {
        snprintf(result, sizeof(result), "Connected to \"%s\" successfully", server);
    }
    else if(status == SOC_CONN_CONNECTING)
    {
        snprintf(result, sizeof(result), "Connecting to \"%s\"", server);
    }
    else
    {
        snprintf(result, sizeof(result), "Fail to connect to \"%s\"", server);
    }

    mode = track_cust_sf_switch(0xFF);
    if(mode == 0)
    {
        snprintf(tmp, sizeof(tmp), "%s", "Hardware testing mode (send all data)");
    }
    else
    {
        snprintf(tmp, sizeof(tmp), "%s", "Software normal mode (filter some data)");
    }

    LOGS("%s \r\n%s", result, tmp);
}

/******************************************************************************
 *  Function    -  track_test_pcba_unit_gprs
 *
 *  Purpose     -  �������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 29-09-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_unit_gprs(char* part2, char* part3)
{
    char result[100] = {0};

    if(strcmp((S8*)part2, "PASS") == 0)
    {
        track_test_pcba_gprs_send_always();
    }
    else if(strcmp((S8*)part2, "OFF") == 0)
    {
        track_test_pcba_gprs_off();
    }
    else if(strcmp((S8*)part2, "STATUS") == 0)
    {
        track_test_pcba_gprs_status();
    }
    else
    {
        sprintf(result, "%s", test_pcba_cmd_not_exist);
        LOGS("%s", result);
    }

}

/******************************************************************************
 *  Function    -  track_test_driver_keypad
 *
 *  Purpose     -  ��������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 29-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_key_event_handle(void)
{
    LOGS("key ok");
    track_test_display_delay_on(200);
}

static void track_test_pcba_unit_keypad(char* part2, char* part3)
{
    char result[100] = {0};

    if(test_pcba_cmd.len == 1)
    {
#if defined(__KEYPAD_MOD__)
        sprintf(result, "%s", "KEY_SOS test");
        track_drv_set_key_handler(track_test_key_event_handle, KEY_SOS, KEY_EVENT_UP);
#else
        sprintf(result, "%s", test_pcba_cmd_hw_not_support);
#endif /* __GT02D__ */
    }
    else
    {
        sprintf(result, "%s", test_pcba_cmd_not_exist);
    }
    LOGS("%s", result);
}
static void track_test_pcba_unit_watchdog(char* part2, char* part3)
{

#if defined( __WATCH_DOG__)
    if(strcmp((S8*)part2, "START") == 0)
    {
        track_drv_watdog_init();
        LOGS("%s", "WDT Start");
    }
    else if(strcmp((S8*)part2, "STOP") == 0)
    {
        track_drv_watdog_close(0);
        LOGS("%s", "WDT Stop");
    }
    else if(strcmp((S8*)part2, "ENABLE") == 0)
    {
        track_drv_watchdog_enable();
        LOGS("%s", "WDT Enable");
    }
    else if(strcmp((S8*)part2, "DISABLE") == 0)
    {
        track_drv_watdog_close(2);
        LOGS("%s", "WDT Disable");
    }
    else if(strcmp((S8*)part2, "WRITEW") == 0)
    {
        WDT_Worng_Data_Test(0xA0A0);
        LOGS("%s", "WDT write wrong data ok!");
    }
    else if(strcmp((S8*)part2, "DISPLAY") == 0)
    {
        track_drv_watch_receive_display();
        LOGS("%s", "WDT Display");
    }
    else
    {
        LOGS("%s", test_pcba_cmd_not_exist);
    }
#else
    LOGS("%s", "This Project no support this function, not found 152!");
#endif /* __WATCH_DOG__ */
}

static void track_test_pcba_unit_lcd(char* part2, char* part3)
{
    kal_uint8 hour = 0, mins = 0;
#if defined(__OLED__)
    if(strcmp((S8*)part2, "BLACK") == 0)
    {
        track_drv_spi_set_background(0x00);
        LOGS("%s", "LCD BLACK");
    }
    else if(strcmp((S8*)part2, "WHITE") == 0)
    {
        track_drv_spi_set_background(0xFF);
        LOGS("%s", "LCD WHITE");
    }
    else if(strcmp((S8*)part2, "X") == 0)
    {

        LOGS("X:%d", "LCD Enable");
    }
    else if(strcmp((S8*)part2, "Y") == 0)
    {

        LOGS("%s", "LCD Disable");
    }
    else if(strcmp((S8*)part2, "WRITE") == 0)
    {

        LOGS("%s", "LCD WRITE");
    }
    else if(strcmp((S8*)part2, "TEST") == 0)
    {
        if(strlen(part3))
        {
            track_drv_lcd_display_test(atoi(part3));
            //LOGS("%s", "LCD TEST");
        }
    }
    else if(strcmp((S8*)part2, "TIME") == 0)
    {
        if(strlen(part3) > 3)
        {
            hour = (part3[0] - '0') * 10 + (part3[1] - '0');
            mins = (part3[2] - '0') * 10 + (part3[3] - '0');
            track_drv_lcd_write_time(hour, mins, hour, mins);
            LOGS("LCD TIME Updata time: %d%d:%d%d", part3[0], part3[1], part3[2], part3[3]);
        }
        else
        {
            LOGS("%s", "LCD TIME Arg3 Error");
        }
    }
    else if(strcmp((S8*)part2, "SLEEP") == 0)
    {
        if(strlen(part3) > 0)
        {
            if(part3[0] == '0')
            {
                track_drv_lcd_exit_sleep();
                LOGS("%s", "LCD Exit Sleep!");
                return;
            }
            else if(part3[0] == '2')
            {
                track_drv_Init_OLED_IC();
                LOGS("%s", "LCD INIT!");
                return;
            }
        }

        track_drv_lcd_enter_sleep();
        LOGS("%s", "LCD Enter Sleep!");
    }
    else
    {
        LOGS("%s", test_pcba_cmd_not_exist);
    }
#else
    LOGS("%s", "This Project no support this function, not found lcd!");
#endif /* __WATCH_DOG__ */
}

static void track_test_pcba_unit_obd(char* part2, char* part3)
{
    kal_uint8 hour = 0, mins = 0;
#if defined(__OBD__)
    if(strcmp((S8*)part2, "ON") == 0)
    {
        if(track_os_uart1_at_mode())
        {
            LOGD(L_DRV, L_V5, "Uart1 AT Mode, �����л���OBDʹ��");
        }
        else
        {
            LOGS("%s", "OBD ON");
            track_obd_init();
        }
    }
    else if(strcmp((S8*)part2, "OFF") == 0)
    {
        LOGS("%s", "OBD OFF");
    }
    else if(strcmp((S8*)part2, "TEST") == 0)
    {
        //LOGS("%s", "LCD TEST");
    }
    else
    {
        LOGS("%s", test_pcba_cmd_not_exist);
    }
#else
    LOGS("%s", "This Project no support this function, not found obd!");
#endif /* __WATCH_DOG__ */
}

#if defined(__AUDIO_RC__)
static void track_test_pcba_unit_rec(char* part2, char* part3)
{
    kal_uint8 hour = 0, mins = 0;
    if(strcmp((S8*)part2, "ON") == 0)
    {
        track_cust_record(0, 1, 10);
        LOGS("%s", "REC ON");
    }
    else if(strcmp((S8*)part2, "OFF") == 0)
    {
        track_cust_record(0, 0, 0);
        LOGS("%s", "REC OFF");
    }
    else if(strcmp((S8*)part2, "TEST") == 0)
    {

    }
    else
    {
        LOGS("%s", test_pcba_cmd_not_exist);
    }
}
#endif /* __AUDIO_RC__ */

static void track_test_pcba_unit_lte(char* part2, char* part3)
{
#if defined(__GT530__)
    kal_uint8 hour = 0, mins = 0;
    if(strcmp((S8*)part2, "POWERON") == 0)
    {
        track_drv_lte_pwron(0);
        LOGS("%s", "LTE POWER ON OK");
    }
    if(strcmp((S8*)part2, "POWEROFF") == 0)
    {
        track_drv_lte_reset_pwron((void*)128);
        LOGS("%s", "LTE POWER OFF OK");
    }
    else if(strcmp((S8*)part2, "WAKE") == 0)
    {
        track_drv_lte_wake();
        LOGS("%s", "LTE WAKE");
    }
    else if(strcmp((S8*)part2, "RESET") == 0)
    {
        track_drv_lte_reset_pwron((void*)256);
        LOGS("%s", "LTE RESET");
    }
    else if(strcmp((S8*)part2, "WDT") == 0)
    {
        LOGS("%s", "LTE WDT");
    }
    else if(strcmp((S8*)part2, "UART") == 0)
    {
        if(strcmp((S8*)part3, "ON") == 0)
        {
            track_drv_lte_uart1_ctrl(1);
            LOGS("%s", "LTE UART ON");
        }
        else
        {
            track_drv_lte_uart1_ctrl(0);
            LOGS("%s", "LTE UART OFF");
        }

    }
    else if(strcmp((S8*)part2, "WIFI") == 0)
    {
        if(strcmp((S8*)part3, "ON") == 0)
        {
            track_drv_wifi_switch(1);
            LOGS("%s", "LTE WIFI ON");
        }
        else if(strcmp((S8*)part3, "OFF") == 0)
        {
            track_drv_wifi_switch(0);
            LOGS("%s", "LTE WIFI OFF");
        }
        else if(strcmp((S8*)part3, "TEST") == 0)
        {
            track_drv_wifi_switch(2);
            LOGS("%s", "LTE WIFI TEST");
        }
    }
    else if(strcmp((S8*)part2, "SIM") == 0)
    {
        track_drv_check_sim();
    }
    else
    {
        LOGS("%s", test_pcba_cmd_not_exist);
    }
#endif
}

static void track_test_pcba_unit_vmc(char* part2, char* part3)
{
    kal_uint8 level = 1 ;
    if(test_pcba_cmd.len != 3 && test_pcba_cmd.len != 2)
    {
        LOGS("%s", test_pcba_cmd_not_exist);
        return;
    }

    if(test_pcba_cmd.len == 3)
    {
        level = atoi(part3);
    }

    if(strcmp((S8*)part2, "ON") == 0)
    {
        track_drv_Set_VMC(KAL_TRUE, level);
        LOGS("\r\nVCM ON %d \t(1=1.8V 2=2.8V 3=3V 4=3.3V)\r\n",  level);
    }
    else if(strcmp((S8*)part2, "OFF") == 0)
    {
        track_drv_Set_VMC(KAL_FALSE, level);
        LOGS("%s", "\r\nVCM OFF\r\n");
    }
    else
    {
        LOGS("%s", test_pcba_cmd_not_exist);
    }
}

#if defined(__CUST_BT__)
static void track_test_pcba_unit_bt(char* part2, char* part3)
{
    if(test_pcba_cmd.len != 3 && test_pcba_cmd.len != 2)
    {
        LOGS("%s", test_pcba_cmd_not_exist);
        return;
    }
    if(strcmp((S8*)part2, "ON") == 0)
    {
        LOGS("BT ON");
        track_drv_bt_switch(1);
    }
    else if(strcmp((S8*)part2, "OFF") == 0)
    {
        LOGS("BT OFF");
        track_drv_bt_switch(0);
    }
    else if(strcmp((S8*)part2, "VERSION") == 0)
    {
        track_drv_bt_setting(BT_VERSINO_REQ);
    }
    else if(strcmp((S8*)part2, "FACTORY") == 0)
    {
        track_drv_bt_setting(BT_RESTORY_FACTORY_REQ);
    }
    else if(strcmp((S8*)part2, "TX") == 0)
    {
        LOGS("BT work in send mode");
        track_drv_set_bt_work_time(50, 0, 5);
    }
    else if(strcmp((S8*)part2, "RX") == 0)
    {
        LOGS("BT work in receive mode");
        track_drv_set_bt_work_time(5, 50, 5);
    }
    else if(strcmp((S8*)part2, "PARAM") == 0)
    {
        LOGS("set BT param");
        track_drv_set_bt_broadcast_param("GV22", track_drv_get_imei(1));
    }
    else
    {
        LOGS("%s", test_pcba_cmd_not_exist);
    }
}
#endif /* __CUST_BT__ */

#if defined (__CUST_RS485__)
static void track_test_pcba_unit_rs485_timeout(void)
{
    LOGS("%s", "RS485 FAIL");
}

static void track_test_pcba_unit_rs485(char* part2, char* part3)
{
    if(test_pcba_cmd.len != 2)
    {
        LOGS("%s", test_pcba_cmd_not_exist);
        return;
    }
	
    if(strcmp((S8*)part2, "STR") == 0)
    {
		char sendBuff[]={"RS485\r\n"};
		track_drv_rs485_write(sendBuff, strlen(sendBuff));
		tr_start_timer(TRACK_RS485_HW_TEST_TIMER_ID, 3000, track_test_pcba_unit_rs485_timeout);

    }
    else if(strcmp((S8*)part2, "HEX") == 0)
    {
		extern void track_rs485_read_vehicle_status_req(void);
		track_rs485_read_vehicle_status_req();
		tr_start_timer(TRACK_RS485_HW_TEST_TIMER_ID, 3000, track_test_pcba_unit_rs485_timeout);
    }
    else
    {
        LOGS("%s", test_pcba_cmd_not_exist);
    }
}

#elif defined (__CUST_UART1_MUX__)
extern void track_drv_uart1_mux_switch(kal_uint8 mode);
extern void track_drv_uart1_mux_write(kal_uint8 *data, int len);

static void track_test_pcba_unit_rs485_timeout(void)
{
	track_drv_uart1_mux_switch(2);
	LOGS("%s", "RS485 FAIL");
}

static void track_test_pcba_unit_rs485_delay_write(void)
{
	char sendBuff[]={"RS485\r\n"};
	track_drv_uart1_mux_switch(1);
	track_drv_uart1_mux_write(sendBuff, strlen(sendBuff));
	tr_start_timer(TRACK_RS485_HW_TEST_TIMER_ID, 3000, track_test_pcba_unit_rs485_timeout);
}

static void track_test_pcba_unit_rs485(char* part2, char* part3)
{
    if(test_pcba_cmd.len != 2)
    {
        LOGS("%s", test_pcba_cmd_not_exist);
        return;
    }
	
    if(strcmp((S8*)part2, "STR") == 0)
    {
		tr_start_timer(TRACK_RS485_HW_TEST_TIMER_ID, 100, track_test_pcba_unit_rs485_delay_write);
    }
    else
    {
        LOGS("%s", test_pcba_cmd_not_exist);
    }
}

#else
static void track_test_pcba_unit_rs485(char* part2, char* part3)
{}
#endif /* __CUST_RS485__ */


#if defined (__CUST_UART1_MUX__)
extern void track_drv_uart1_mux_switch(kal_uint8 mode);
extern void track_drv_uart1_mux_write(kal_uint8 *data, int len);
static void track_test_pcba_unit_uart(char* part2, char* part3)
{
    if(test_pcba_cmd.len != 2)
    {
        LOGS("%s", test_pcba_cmd_not_exist);
        return;
    }
	
    if(strcmp((S8*)part2, "RS485") == 0)
    {
        LOGS("uart1 to RS485");
        track_drv_uart1_mux_switch(1);
        track_drv_uart1_mux_write("RS485",5);
    }
    else if(strcmp((S8*)part2, "RS232") == 0)
    {
        LOGS("uart1 to RS232");
        track_drv_uart1_mux_switch(2);
        track_drv_uart1_mux_write("RS232",5);
    }
    else if(strcmp((S8*)part2, "OFF") == 0)
    {
        LOGS("disable to RS485/RS232");
        track_drv_uart1_mux_switch(0);
    }
    else
    {
        LOGS("%s", test_pcba_cmd_not_exist);
    }

}
#endif /* __CUST_UART1_MUX__ */

/******************************************************************************
 *  Function    -  track_test_pcba_unit_sw_debug
 *
 *  Purpose     -  ����������
 *
 *  Description - AT^GT_CM=PCBA,DEBUG,** [,**]
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 27-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_unit_sw_debug(char* part2, char* part3)
{
    kal_uint8 tmp2 = 0xFF, tmp3 = 0xFF;
    char result[100] = {0};

    tmp2 = atoi(part2);

    if(strlen(part3))
    {
        tmp3 = atoi(part3);
    }

    /*  ������ʱ���Խӿ�*/
    switch(tmp2)
    {
        case 1:
            track_test_sensor_count(KAL_FALSE);
            break;

        case 2:
            LOGD(L_DRV, L_V4, "sensor ����Ӧ");
            track_drv_sensor_type_auto_matching();
            break;

        case 3:
            {
                LOGD(L_DRV, L_V4, "���ֵ %d", track_drv_get_one_day_reset_time());
            }
            break;
#if 0
        case 4:
            {
                unsigned int length;
                kal_uint8* buff;
                extern void track_cust_sms_send(kal_uint8 number_type, char *number, kal_bool isChar, kal_uint8 *data, kal_uint16 len);
                buff=(kal_uint8 *)track_os_get_string_buff(STR_GLOBAL_OK,track_language_German,&length);
                track_cust_sms_send(0, "15899793450", 0, buff, length);
                LOGS("���Ķ��� %d", length);
            }
            break;
#endif

        case 5:
        {
            extern void track_cust_search_fake_cell();
            LOGD(L_DRV, L_V4, "ģ���յ�α��վ��Ϣ");
            track_cust_search_fake_cell();
        }
        break;

        case 6:
        {
            extern void track_cust_fake_cell_test_forced_alarm();
            LOGD(L_DRV, L_V4, "ǿ��α��վ����");
            track_cust_fake_cell_test_forced_alarm();
        }
        break;	
		
#if defined (__CUST_UART1_MUX__)
        case 7:			
        {
            extern void track_drv_uart1_Init(void* owner);
            LOGD(L_DRV, L_V4, "UART1");
            track_drv_uart1_Init((void*)3);
        }
        break;

        case 8:			
        {
            extern void track_drv_sensor_get_temp_humi_req(void);
            LOGD(L_DRV, L_V4, "RS485 send hex");
            track_drv_sensor_get_temp_humi_req();
        }
        break;
#endif /* __CUST_UART1_MUX__ */

#if defined (__CUST_ACC_BY_MCU__)
		case 90:
		{
			extern void track_drv_acc_by_mcu_merge(void* acc_flag);
			track_drv_acc_by_mcu_merge((void*)0);
		}
		break;

		case 91:
		{
			extern void track_drv_acc_by_mcu_merge(void* acc_flag);
			track_drv_acc_by_mcu_merge((void*)1);
		}
		break;
                case 92:
                {
                    extern void track_drv_acc_by_mcu_rece(kal_uint8 * buff, kal_uint16 len);
                    track_drv_acc_by_mcu_rece("AONN\r\n",6);
                }
                break;
                case 93:
                {
                    extern void track_drv_acc_by_mcu_rece(kal_uint8 * buff, kal_uint16 len);
                    track_drv_acc_by_mcu_rece("AOFF\r\n",6);
                }
                break;
#endif /* __CUST_ACC_BY_MCU__ */
		

        default:
            sprintf(result, "%s", test_pcba_debug_not_exist);
            LOGS("%s", result);
            break;
    }
}

static void track_test_pcba_unit_sleep(char* part2, char* part3)
{

    if(strcmp((S8*)part2, "STATUS") == 0)
    {
        track_drv_check_sleep_status();
        LOGS("Sleep status:%d", track_drv_check_sleep_status());
    }
    else
    {
        LOGS("GPS OFF, System sleep.");
        track_cust_gps_control(0);
        track_drv_sleep_enable(SLEEP_MOD_MAX, KAL_TRUE);
    }
}

static void track_test_pcba_unit_mcu(CMD_DATA_STRUCT * cmd)
{
#if defined(__CUST_ACC_BY_MCU__)
    if(strcmp((S8*)cmd->pars[2], "VOLT") == 0)
    {
        U8 tmp[] = {0x53, 0x00, 0x00, 0x00, 0x00, 0x0D};
        float vl,vh;
        U16 l, h;
        vl = atof(cmd->pars[3]);
        vh = atof(cmd->pars[4]);
        vl = vl * 2048 / 11;
        vh = vh * 2048 / 11;
        l = vl;
        h = vh;
        tmp[1] = l >> 8;
        tmp[2] = l & 0x00FF;
        tmp[3] = h >> 8;
        tmp[4] = h & 0x00FF;
        track_drv_write_exmode(tmp, 6);
    }
    else if(strcmp((S8*)cmd->pars[2], "EPOFF") == 0)
    {
        track_drv_write_exmode("ELOW\r\n", 6);
    }
    else if(strcmp((S8*)cmd->pars[2], "EPON") == 0)
    {
        track_drv_write_exmode("EHIG\r\n", 6);
    }
    else if(strcmp((S8*)cmd->pars[2], "ADC") == 0)
    {
        #if 0
        extern kal_uint32 track_drv_get_external_voltage_from_mcu(void);
        float mcu_ext;

        mcu_ext=(float)track_drv_get_external_voltage_from_mcu();
        LOGS("MCU vExt:%0.2fV", mcu_ext / 1000000);
        #else
        track_cmd_read_adc_result_flag(1);
        track_drv_acc_by_mcu_send("GTVT\r\n",6);//���̶���Ƭ����������ѹ����
        //����ģʽ����log,1,5���ܿ������
        #endif
    }
    else if(strcmp((S8*)cmd->pars[2], "VERSION") == 0)
    {
        extern kal_uint8* track_drv_get_mcu_version(void);
        kal_uint8* ver=track_drv_get_mcu_version();
        if(strlen(ver)==0)
        {
            LOGS("MCU verson:null");//��Ƭ����ʼ��������������δ��ȡ
        }
        else
        {
            LOGS("MCU verson:V%s", ver);
        }
    }
    else
    {
        LOGS("%s", test_pcba_cmd_not_exist);
    }
#endif /* __CUST_ACC_BY_MCU__ */
}

/******************************************************************************
 *  Function    -  track_test_pcba_help_remind
 *
 *  Purpose     -  ��ʾ����Ŀ����֧�ֵ�ȫ��ָ��
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 21-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_help_remind(void)
{
    char remind[1024] = {0};
    snprintf(remind, sizeof(remind),
             "now we can support these command :(AT^GT_CM=PCBA,)\r\n\r\n-----------------\r\n"
             "LED,OFF\r\n"
             "LED,ON\r\n"
             "LED,R,OFF\r\n"
             "LED,R,ON\r\n"
             "LED,G,OFF\r\n"
             "LED,G,ON\r\n"
             "LED,B,OFF\r\n"
             "LED,B,ON\r\n"
             "LED,E,OFF\r\n"
             "LED,E,ON\r\n"
             "LED,KP,OFF\r\n"
             "LED,KP,ON\r\n"
             "LED,LOCK\r\n"
             "LED,UNLOCK\r\n"
             "LED,FLASH\r\n\r\n"

             "VIB,OFF\r\n"
             "VIB,ON\r\n\r\n"
             "GPS,OFF\r\n"
             "GPS,ON\r\n"
             "GPS,HOT\r\n"
             "GPS,COLD\r\n"
             "GPS,STATUS\r\n"
             "GPS,EPO\r\n"
             "GPS,TC\r\n\r\n"

             "SENSOR,OFF\r\n"
             "SENSOR,ON,<1-5>\r\n"
             "SENSOR,STATUS\r\n"
             "SENSOR,TEST\r\n\r\n"

             "GPIO,N,H\r\n"
             "GPIO,N,L\r\n\r\n"

             "EINT,N,H\r\n"
             "EINT,N,L\r\n"
             "EINT,AUTO\r\n"
             "EINT,STATUS\r\n\r\n"

             "LBS,OFF\r\n"
             "LBS,ON\r\n"
             "LBS,STATUS\r\n\r\n"

             "BB,DT\r\n"
             "BB,ID\r\n"
             "BB,SIM\r\n"
             "BB,SW\r\n"
             "BB,OFF\r\n\r\n"

             "PMU,BMT\r\n"
             "PMU,EXT\r\n"
             "PMU,ADC\r\n"
             "PMU,SLEEP\r\n\r\n"

             "GPRS,PASS\r\n"
             "GPRS,STATUS\r\n"
             "GPRS,OFF\r\n\r\n"

             "KEY\r\n\r\n"

             "WDT,START\r\n"
             "WDT,STOP\r\n"
             "WDT,ENABLE\r\n"
             "WDT,DISABLE\r\n"
             "WDT,WRITEW\r\n"
             "WDT,DISPALY\r\n\r\n"

             "LCD,Black\r\n"
             "LCD,White\r\n"
             "LCD,B\r\n\r\n\r\n"

             "OBD,ON\r\n"
             "OBD,OFF\r\n"
             "OBD,TEST\r\n\r\n"

             "REC,ON\r\n",
             "REC,OFF\r\n\r\n",

             "LTE,POWERON\r\n",
             "LTE,WAKE\r\n",
             "LTE,RESET\r\n",
             "LTE,WDT\r\n",
             "LTE,USB\r\n",
             "LTE,UART\r\n",

             "VMC,ON\r\n",
             "VMC,OFF\r\n",

             "BT,ON\r\n",
             "BT,OFF\r\n",
             "BT,FACTORY\r\n",
             "BT,VERSION\r\n",

             "RS485\r\n",
             "MCU\r\n"
             "\r\n-----------------\r\n"
            );
    LOGS("%s", remind);
}

/******************************************************************************
 *  Function    -  track_test_pcba_unit_main
 *
 *  Purpose     -  Ӳ����Ԫ����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 22-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_pcba_unit_main(test_pcba_enum type, CMD_DATA_STRUCT * cmd)
{
    switch(type)
    {
        case TEST_PCBA_LED://LED
            track_test_pcba_unit_led(cmd->pars[2], cmd->pars[3]);
            break;

        case TEST_PCBA_VIB://VIB
            track_test_pcba_unit_vib(cmd->pars[2], cmd->pars[3]);
            break;

        case TEST_PCBA_GPS://GPS
            track_test_pcba_unit_gps(cmd->pars[2], cmd->pars[3]);
            break;

        case TEST_PCBA_SENSOR://Sensor
            track_test_pcba_unit_sensor(cmd->pars[2], cmd->pars[3]);
            break;

        case TEST_PCBA_GPIO://GPIO
            track_test_pcba_unit_gpio(cmd->pars[2], cmd->pars[3]);
            break;

        case TEST_PCBA_EINT://Eint
            track_test_pcba_unit_eint(cmd->pars[2], cmd->pars[3]);
            break;

        case TEST_PCBA_LBS://LBS
            track_test_pcba_unit_lbs(cmd->pars[2], cmd->pars[3]);
            break;

        case TEST_PCBA_BB://BB
            track_test_pcba_unit_bb(cmd->pars[2], cmd->pars[3]);
            break;

        case TEST_PCBA_PMU://PMU
            track_test_pcba_unit_pmu(cmd->pars[2], cmd->pars[3]);
            break;

        case TEST_PCBA_GPRS://GPRS
            track_test_pcba_unit_gprs(cmd->pars[2], cmd->pars[3]);
            break;

        case TEST_PCBA_KEY://KEY
            track_test_pcba_unit_keypad(cmd->pars[2], cmd->pars[3]);
            break;

        case TEST_PCBA_WDT:
            track_test_pcba_unit_watchdog(cmd->pars[2], cmd->pars[3]);
            break;

        case TEST_PCBA_LCD:
            track_test_pcba_unit_lcd(cmd->pars[2], cmd->pars[3]);
            break;

        case TEST_PCBA_OBD:
            track_test_pcba_unit_obd(cmd->pars[2], cmd->pars[3]);
            break;

#if defined(__AUDIO_RC__)
        case TEST_PCBA_REC:
            track_test_pcba_unit_rec(cmd->pars[2], cmd->pars[3]);
            break;
#endif /* __AUDIO_RC__ */

        case TEST_PCBA_LTE:
            track_test_pcba_unit_lte(cmd->pars[2], cmd->pars[3]);
            break;

        case TEST_PCBA_VMC:
            track_test_pcba_unit_vmc(cmd->pars[2], cmd->pars[3]);
            break;

        case TEST_PCBA_DEBUG://DEBUG
            track_test_pcba_unit_sw_debug(cmd->pars[2], cmd->pars[3]);
            break;

        case TEST_PCBA_SLEEP:
            track_test_pcba_unit_sleep(cmd->pars[2], cmd->pars[3]);
            break;

#if defined(__CUST_BT__)
        case TEST_PCBA_BT:
            track_test_pcba_unit_bt(cmd->pars[2], cmd->pars[3]);
            break;
#endif /* __CUST_BT__ */

        case TEST_PCBA_RS485:
            track_test_pcba_unit_rs485(cmd->pars[2], cmd->pars[3]);
            break;

#if defined (__CUST_UART1_MUX__)
        case TEST_PCBA_UART:
            track_test_pcba_unit_uart(cmd->pars[2], cmd->pars[3]);
            break;
#endif /* __CUST_UART1_MUX__ */
        case TEST_PCBA_MCU:
            track_test_pcba_unit_mcu(cmd);
            break;

        default:
            LOGS("%s", test_pcba_cmd_not_exist);
            break;
    }
}


#if 1
//PCBA test cmd
#endif


/*****************************************************************************
 *  Global Functions			ȫ�ֺ���
 *****************************************************************************/

/******************************************************************************
 *  Function    -  track_drv_test_gps_fix_time
 *
 *  Purpose     -  ����GPS��λʱ��
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 28-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_test_gps_fix_time(void)
{
    if(test_gps_fix_time == 0)
    {
        return;
    }
    else
    {
        track_test_pcba_gps_fix_time_output(KAL_TRUE);
    }
}

/******************************************************************************
 *  Function    -  track_drv_test_pcba_eint_status_updata
 *
 *  Purpose     -  �ж�״̬�б����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 22-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_test_pcba_eint_status_updata(U8 eint, U8 level)
{
    if(eint >= TEST_PCBA_EINT_LIST_MAX)
    {
        LOGS("Warn:Eint%d,Level=%d", eint, level);
        return;
    }

    if(level == 0)
    {
        test_pcba_eint_list[eint] = 'L';
    }
    else
    {
        test_pcba_eint_list[eint] = 'H';
    }

    if(test_paba_output.eint_output)
    {
        if(level == 0)
        {
            LOGS("Eint%d,Level=L", eint);
        }
        else
        {
            LOGS("Eint%d,Level=H", eint);
        }
    }
}


/******************************************************************************
 *  Function    -  track_drv_test_pcba_entry
 *
 *  Purpose     -  Ӳ��PCBA ����
 *
 *  Description -  AT^GT_CM=PCBA,part1,part2<,part3>
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 27-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_test_pcba_entry(U8 len, CMD_DATA_STRUCT * cmd)
{
    char result[100] = {0};
    kal_uint8 index;
    kal_bool find = KAL_FALSE;

    char *test_pcba_tab[TEST_PCBA_END] =
    {
        "LED",
        "VIB",
        "GPS",
        "SENSOR",
        "GPIO",
        "EINT",
        "LBS",
        "BB",
        "PMU",
        "GPRS",
        "KEY",
        "WDT",
        "LCD",
        "OBD",
        "REC",
        "LTE",
        "VMC",
        "DEBUG",
        "SLEEP",
        "BT",
        "RS485",
        "UART",
        "MCU"
    };
    //������test_pcba_enum ����һ��

    if(len == 0)
    {
        track_test_pcba_help_remind();
        return;
    }

    for(index = 0; index < TEST_PCBA_END; index++)
    {
        if(strcmp((S8*)test_pcba_tab[index], (S8*)cmd->pars[1]) == 0)
        {
            find = KAL_TRUE;
            break;
        }
    }

    if(find)
    {
        memset(&test_pcba_cmd, 0x00, sizeof(test_pcba_cmd));
        test_pcba_cmd.len = len;
        snprintf(test_pcba_cmd.part2, TEST_PCBA_CMD_LEN_MAX, "%s", cmd->pars[2]);
        snprintf(test_pcba_cmd.part3, TEST_PCBA_CMD_LEN_MAX, "%s", cmd->pars[3]);
        track_test_pcba_unit_main((test_pcba_enum)index, cmd);
    }
    else
    {
        sprintf(result, "%s", test_pcba_cmd_not_exist);
        LOGS("%s", result);
    }
}



#endif /* __TEST_PCBA__ */


