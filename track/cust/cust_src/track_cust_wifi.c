#if defined __CUSTOM_WIFI_FEATURES_SWITCH__
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
#include "track_fun_common.h"
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
static kal_bool wifi_send = KAL_FALSE;
static kal_bool wifi_scaning = KAL_FALSE;
static kal_bool wifisend_heartbear = KAL_FALSE;
/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/
track_wifi_struct wifi_status = {0};
/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/
extern kal_uint16 track_cust_paket_2C(void);
extern void heartbeat_packets(void *par);
extern void track_soc_send(void *par);
extern void track_cust_module_delay_close();
extern void track_cust_sendlbs_limit();
static void track_cust_WIFI_off_paket(void);

/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/


/*****************************************************************************
*  Local Functions                      本地函数
******************************************************************************/


/*****************************************************************************

*  Global Functions                     全局函数
******************************************************************************/
void track_cust_wifidata(kal_uint8* data, kal_uint32 len)
{
    char buf_ams[50] = {0};
    char buf[100] = {0};
    applib_time_struct currTime = {0};
    kal_uint8 buf_data[10][200] = {0};
    kal_uint8 *pars[6];
    kal_uint8 tmp[7] = {0};
    int index = 0, buf_len = 0, data_index = 0;
    track_wifi_struct track_wifi_tmp = {0};
    kal_uint8 max_rssi = 255;
    kal_uint8 i, j, max_data;
    LBS_Multi_Cell_Data_Struct *lbs = track_drv_get_lbs_data();

    LOGD(L_APP, L_V5, "%s", data);
    memset(buf_data, 0x00, sizeof(buf_data));
    memset(&wifi_status, 0x00, sizeof(track_wifi_struct));

    memcpy(buf_data, data, sizeof(buf_data));

    for (index = 0, data_index = 0; index < len; index++)
    {
        buf_len = strlen(buf_data[index]);
        //     LOGD(L_APP, L_V7, "%d", buf_len);
        //   LOGD(L_APP, L_V6, "buf_data%s", buf_data[index]);
        //    LOGD(L_APP, L_V6, "data%s", data[index]);
//LOGH(L_APP, L_V7, buf_data[index], buf_len);

        track_fun_str_analyse(buf_data[index], pars, 6, ':', "\r\n", ',');


        if (strlen(pars[0]) && strlen(pars[3]))
        {
            memcpy(track_wifi_tmp.wifi_mac[data_index], pars[0], strlen(pars[0]));
            memcpy(tmp, pars[3], strlen(pars[3]));
            track_fun_del_string(track_wifi_tmp.wifi_mac[data_index], ":");
            track_fun_del_string(tmp, "-");
            track_wifi_tmp.wifi_rssi[data_index] = OTA_atoi(tmp);
            data_index++;
        }
    }
    track_wifi_tmp.wifi_sum = data_index;
    if (track_wifi_tmp.wifi_sum > 10)
        track_wifi_tmp.wifi_sum = 10;

    memset(&wifi_status, 0, sizeof(track_wifi_struct));//排序算法
    for (j = 0; j < track_wifi_tmp.wifi_sum; j++)
    {
        for (i = 0; i < track_wifi_tmp.wifi_sum; i++)
        {
            if (max_rssi > track_wifi_tmp.wifi_rssi[i])
            {
                memcpy(wifi_status.wifi_mac[j], track_wifi_tmp.wifi_mac[i], 20);
                wifi_status.wifi_rssi[j] = track_wifi_tmp.wifi_rssi[i];
                max_rssi = track_wifi_tmp.wifi_rssi[i];
                max_data = i;
            }
        }
        track_wifi_tmp.wifi_rssi[max_data] = 255;
        max_rssi = 255;
    }
    wifi_status.wifi_sum = track_wifi_tmp.wifi_sum;

    for (i = 0; i < wifi_status.wifi_sum; i++)
    {
        LOGD(L_APP, L_V5, "MAC:%s,rssi:%d", wifi_status.wifi_mac[i], wifi_status.wifi_rssi[i]);
    }
    wifi_scaning = KAL_FALSE;
    if (wifi_status.wifi_sum >= 2)
    {
        wifi_send = KAL_TRUE;
#if defined(__GT420D__)
                track_fun_get_time(&currTime, KAL_FALSE, &G_parameter.zone);
                snprintf(buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d send WIFI,%d,%d,%d,%d,%d,%d,%d,%d\n\r",
                            currTime.nYear,
                            currTime.nMonth,
                            currTime.nDay,
                            currTime.nHour,
                            currTime.nMin,
                            currTime.nSec,
                            track_cust_refer_keyin_data(99),
                            track_cust_needsend_charge_data(99),
                            track_cust_refer_motion_status_data(99),
                            track_cust_check_simalm_flag(99),
                            track_cust_check_firstpoweron_flag(99),
                            track_cust_check_downalm_flag(98),
                            track_cust_check_teardown_flag(98),
                            track_cust_check_batteryalm_flag(99));
                LOGD(5,5,"len:%d",strlen(buf));
        track_cust_save_logfile(buf, strlen(buf));
        if(track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 && track_cust_refer_motion_status_data(99) == 0 \
                    && track_cust_check_simalm_flag(99) == 0 && track_cust_check_firstpoweron_flag(99) == 0 &&track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0)
        {
            LOGD(L_APP,L_V5,"正常上传WIFI状态包");
            track_cust_dsc_location_paket(NULL,lbs,LOCA_WIFI,DSC_CUST_POSITION_TIMING,0,9);
            }
        if(track_cust_check_firstpoweron_flag(99) == 1 && track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 && track_cust_refer_motion_status_data(99) == 0 \
                    && track_cust_check_simalm_flag(99) == 0 && track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0)
                {
                    LOGD(L_APP,L_V5,"上传开机包");
                    track_cust_dsc_location_paket(NULL,lbs,LOCA_WIFI,DSC_CUST_POSITION_TIMING,1,15);
                    track_cust_check_firstpoweron_flag(0);
                }
        else
        {
            track_cust_check_firstpoweron_flag(0);
            }
        if (track_cust_refer_keyin_data(99) == 1)
        {
            LOGD(L_APP,L_V5,"按键上传WIFI状态包");
            track_cust_dsc_location_paket(NULL,lbs,LOCA_WIFI,DSC_CUST_POSITION_TIMING,0,3);
            track_cust_refer_keyin_data(0);
        }
        if (track_cust_needsend_charge_data(99) == 1)
        {
            LOGD(L_APP,L_V5,"充电上传WIFI状态包");
            track_cust_dsc_location_paket(NULL,lbs,LOCA_WIFI,DSC_CUST_POSITION_TIMING,0,4);
            track_cust_needsend_charge_data(0);
        }
        if (track_cust_needsend_charge_data(99) == 2)
            {
                LOGD(L_APP,L_V5,"拔电上传上WIFI状态包");
                track_cust_dsc_location_paket(NULL,lbs,LOCA_WIFI,DSC_CUST_POSITION_TIMING,0,5);
                track_cust_needsend_charge_data(0);
            }
        if (track_cust_refer_motion_status_data(99) == 1)
                {
                    LOGD(L_APP,L_V5,"运动状态改变上传WIFI状态包");
                    if(track_cust_gt420d_carstatus_data(99) == 2)
                    {
                        track_cust_dsc_location_paket(NULL,lbs,LOCA_WIFI,DSC_CUST_POSITION_TIMING,0,1);
                    }
                    else
                    {
                        track_cust_dsc_location_paket(NULL,lbs,LOCA_WIFI,DSC_CUST_POSITION_TIMING,0,2);
                        }
                    track_cust_refer_motion_status_data(0);
                }
        if(track_cust_check_downalm_flag(98) == 3)
        {
            LOGD(L_APP,L_V5,"无光报警发包");
            track_cust_check_downalm_flag(2);
            tr_stop_timer(TRACK_CUST_EXTCHIP_DARKALM_DELAY_SEND_GPSORLBS_TIMER_ID);
            track_cust_dsc_location_paket(NULL,lbs,LOCA_WIFI,DSC_CUST_POSITION_TIMING,3,7);
        }
        if(track_cust_check_teardown_flag(98) == 3)
        {
            LOGD(L_APP,L_V5,"开箱报警发包");
            track_cust_check_teardown_flag(2);
            tr_stop_timer(TRACK_CUST_EXTCHIP_TEARDOWN_DELAY_SEND_GPSORLBS_TIMER_ID);
            track_cust_dsc_location_paket(NULL,lbs,LOCA_WIFI,DSC_CUST_POSITION_TIMING,3,6);
        }
        if(track_cust_check_batteryalm_flag(99) == 1)
        {
            LOGD(L_APP,L_V5,"低电报警发包");
            track_cust_check_batteryalm_flag(0);
            track_cust_dsc_location_paket(NULL,lbs,LOCA_WIFI,DSC_CUST_POSITION_TIMING,4,14);
        }
        if(track_cust_check_simalm_flag(99) == 1)
        {
            LOGD(L_APP,L_V5,"换卡报警发包");
            track_cust_check_simalm_flag(0);
            track_cust_dsc_location_paket(NULL,lbs,LOCA_WIFI,DSC_CUST_POSITION_TIMING,3,13);
        }
        track_cust_check_beforepoweroff_flag(0);
#else
        track_cust_paket_2C();
#endif
        tr_stop_timer(TRACK_CUST_WIFI_OFF_TIMER_ID);
        tr_stop_timer(TRACK_CUST_WIFI_SCAN_TIMER_ID);
        track_drv_wifi_cmd(0);
    }
    else
    {
        #if defined(__GT420D__)
                snprintf(buf_ams, 50, "WIFI,OFF\n\r");
                LOGD(5,5,"len:%d",strlen(buf_ams));
                track_cust_save_logfile(buf_ams, strlen(buf_ams));
            #endif
        track_cust_WIFI_off_paket();
    }
    //  wifisend_heartbear = KAL_TRUE;
}

kal_bool track_cust_get_wifi_upload_sign()
{
    return wifi_send;
}
kal_bool track_cust_get_wifidata_send_sign(kal_uint8 par)
{
    if (par == 99)
        return wifisend_heartbear;
    else if (par == 0 || par == 1)
        wifisend_heartbear = par;
    return wifisend_heartbear;
}



#if defined(__GT420D__)
void track_cust_get_wifi_sended_sign()
{
    if (KAL_FALSE == wifisend_heartbear)
    {
        tr_start_timer(TRACK_CUST_NET_WIFI_SENDED_HEARTBEAR_TIMER_ID, 2 * 1000, track_cust_get_wifi_sended_sign);
    }
    else
    {
        if (track_is_timer_run(TRACK_CUST_NET_WIFI_SENDED_HEARTBEAR_TIMER_ID))
            track_stop_timer(TRACK_CUST_NET_WIFI_SENDED_HEARTBEAR_TIMER_ID);
        //track_os_intoTaskMsgQueueExt(heartbeat_packets,(void *)119);
        wifisend_heartbear = KAL_TRUE;

    }
}

void track_cust_wifi_work_mode(void)
{
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        return;
    }
    if (G_parameter.wifi.sw)
    {
        track_cust_sendWIFI_paket();
    }
    if (G_parameter.extchip.mode == 1 && G_parameter.extchip.mode1_par1 > 5 || G_parameter.extchip.mode == 2)
    {
        track_cust_module_delay_close(30);
    }
}


void track_drv_wifi_work_mode(void)
{
    track_cust_sendWIFI_paket();
}
#else
void track_drv_wifi_work_mode(void)
{
    track_cust_sendWIFI_paket();

    if (track_cust_get_work_mode() == WORK_MODE_3)
    {
        return;
    }
    if (G_parameter.wifi.sw == 0)
    {
        return;
    }
    if (G_parameter.wifi.work_timer != 0)
    {
        LOGD(L_APP, L_V1, "WIFI定时搜索开启,%d分钟后搜索!", G_parameter.wifi.work_timer / 60);

        tr_start_timer(TRACK_CUST_WIFI_WORK_TIMER_ID, (G_parameter.wifi.work_timer + G_parameter.wifi.sacn_timer) * 1000, track_drv_wifi_work_mode);
    }
}

#endif


/******************************************************************************
 *  Function    -  track_cust_WIFI_off_paket
 *
 *  Purpose     -  搜索wifi超时
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-02-27,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_WIFI_off_paket(void)
{
    memset(&wifi_status, 0, sizeof(track_wifi_struct));
#if defined(__GT300S__)
    track_cust_paket_2C();
#elif defined (__GT370__)|| defined (__GT380__)
    //需求变更，无wifi不上报
    LOGD(L_APP, L_V5, "超时未搜索到WIFI，不传了");
    //track_os_intoTaskMsgQueueExt(track_cust_send_lbs, (void*)4);
#elif defined(__GT310__)
    LOGD(L_APP, L_V5, "超时没有搜索到WIFI，上传LBS!!");
    track_cust_sendlbs_limit();
    if (WORK_MODE_3 == track_cust_get_work_mode() && 1 == track_cust_mode3_uping(0xff))
    {
        track_mode3_upload_location(LBS_FIX);
    }
#elif defined(__GT740__)
    LOGD(L_APP, L_V5, "未搜索到WIFI %d，上传LBS!!", wifi_status.wifi_sum);

    memset(&wifi_status, 0, sizeof(track_wifi_struct));
    track_os_intoTaskMsgQueueExt(track_cust_send_lbs, (void*)4);
    if (G_parameter.extchip.mode != 1 && G_parameter.gpswake == 0) //mode2
    {
        //track_cust_ele_cal();
        //track_cust_gps_module_work((void *)48);
    }
#elif defined(__GT420D__)
    LOGD(L_APP, L_V5, "未搜索到WIFI %d，上传LBS包!!", wifi_status.wifi_sum);
    track_cust_sendlbs_limit();
#endif
    wifi_scaning = KAL_FALSE;
    tr_stop_timer(TRACK_CUST_WIFI_OFF_TIMER_ID);
    tr_stop_timer(TRACK_CUST_WIFI_SCAN_TIMER_ID);
    track_drv_wifi_cmd(0);

}

void track_cust_sendWIFI_paket(void)
{
    kal_uint16 wifi_close = G_parameter.wifi.sacn_timer + 1;
    kal_uint16 wifi_scan = G_parameter.wifi.sacn_timer - 2;
    static U8 count = 0;

    LOGD(L_APP, L_V5, "count %d", count);
#if defined(__GT740__)||defined(__GT420D__)
    if (RUNING_TIME_SEC < 10)
    {
        LOGD(L_APP, L_V5, "初始化未完成,延迟2秒开启WIFI.");
        tr_start_timer(TRACK_CUST_WIFI_WORK_TIMER_ID, 2 * 1000, track_cust_sendWIFI_paket);
        return;
    }
    #if !defined(__GT420D__)
    if (track_cust_is_innormal_wakeup() == KAL_TRUE)
    {
        return;
    }
    #endif
    if (track_sms_get_send_status())
    {
        if (count < 5)
        {
            count++;
            tr_start_timer(TRACK_CUST_WIFI_WORK_TIMER_ID, 5 * 1000, track_drv_wifi_work_mode);
            return;
        }
    }
    else
    {
        count = 0;
    }
#endif

#if defined(__GT370__)|| defined (__GT380__)
    if (G_parameter.wifi.sw == 0)
    {
        return;
    }
#endif

    wifi_scaning = KAL_TRUE;

    if (track_drv_wifi_status(99) != 1)
    {
        track_drv_wifi_cmd(1);
#if defined(__GT370__)|| defined (__GT380__)
        if (G_parameter.wifi.sacn_timer >= track_gt370_get_upload_time())
        {
            wifi_close = track_gt370_get_upload_time() + 1;
            wifi_scan = track_gt370_get_upload_time() - 2;
        }
        if (KAL_FALSE == track_is_timer_run(TRACK_CUST_WIFI_OFF_TIMER_ID))
        {
            tr_start_timer(TRACK_CUST_WIFI_OFF_TIMER_ID, wifi_close * 1000, track_cust_WIFI_off_paket);
        }
        if (KAL_FALSE == track_is_timer_run(TRACK_CUST_WIFI_SCAN_TIMER_ID))
        {
            track_start_timer(TRACK_CUST_WIFI_SCAN_TIMER_ID, wifi_scan * 1000, track_drv_wifi_cmd, (void *)2); //开启WIFI 跟查询必须做间隔
        }
#elif defined (__GT420D__)
        //#if 0
        if (KAL_FALSE == track_is_timer_run(TRACK_CUST_WIFI_OFF_TIMER_ID))
        {
            tr_start_timer(TRACK_CUST_WIFI_OFF_TIMER_ID, G_parameter.wifi.sacn_timer * 1000 + 10 * 1000, track_cust_WIFI_off_paket);
        }
        if (KAL_FALSE == track_is_timer_run(TRACK_CUST_WIFI_SCAN_TIMER_ID))
        {
            LOGD(L_APP, L_V1, "WiFi已打开，开启%d秒延时，延时扫描", G_parameter.wifi.sacn_timer);
            //G_realtime_data.cal_wk_time_struct.awakentimes++;
            track_start_timer(TRACK_CUST_WIFI_SCAN_TIMER_ID, G_parameter.wifi.sacn_timer * 1000, track_drv_wifi_cmd, (void *)2); //开启WIFI 跟查询必须做间隔
        }
        //#endif
#else
        if (KAL_FALSE == track_is_timer_run(TRACK_CUST_WIFI_OFF_TIMER_ID))
        {
            tr_start_timer(TRACK_CUST_WIFI_OFF_TIMER_ID, G_parameter.wifi.sacn_timer * 1000 + 5 * 1000, track_cust_WIFI_off_paket);
        }
        if (KAL_FALSE == track_is_timer_run(TRACK_CUST_WIFI_SCAN_TIMER_ID))
        {
            track_start_timer(TRACK_CUST_WIFI_SCAN_TIMER_ID, G_parameter.wifi.sacn_timer * 1000 + 1000, track_drv_wifi_cmd, (void *)2); //开启WIFI 跟查询必须做间隔
        }
#endif

    }
    else
    {
        track_drv_wifi_cmd(2);
    }

}

kal_bool track_cust_wifi_scan_status()
{
    return wifi_scaning;
}


void  track_cust_wifi_num()
{
    G_realtime_data.cal_wk_time_struct.awakentimes++;
}

#endif
