/******************************************************************************
 * track_main.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        主程序功能入口
 *
 * modification history
 * --------------------
 * v1.0   2012-07-14,  chengjun create this file
 *
 ******************************************************************************/



/****************************************************************************
* Include Files             包含头文件
*****************************************************************************/
#include "track_os_main.h"
#include "syscomp_config.h"
#include "task_config.h"        /* Task creation */
#include "track_os_timer.h"
#include "stack_msgs.h"

#include "c_RamBuffer.h"
#include "c_string.h"
#include "c_vector.h"
#include "track_cust_key.h"
#include "track_drv_uart.h"

#include "track_os_log.h"
#include "track_cust_main.h"
#include "track_os_ell.h"
#include "track_cust.h"
/*****************************************************************************
 *  Define			    宏定义
*****************************************************************************/


/*****************************************************************************
* Typedef  Enum         枚举
*****************************************************************************/


/*****************************************************************************
 *  Struct			    数据结构定义
*****************************************************************************/


/*****************************************************************************
* Local variable            局部变量
*****************************************************************************/


/****************************************************************************
* Global Variable           全局变量
*****************************************************************************/


/****************************************************************************
* Global Variable - Extern          引用全局变量
*****************************************************************************/


/*****************************************************************************
 *  Global Functions	- Extern	    引用外部函数
*****************************************************************************/
extern void track_drv_watdog_init(void);
extern void track_drv_query_sim_type(void);
extern void track_drv_get_system_param(void);
extern void track_drv_get_network_info(void);

/*****************************************************************************
 *  Local Functions	            本地函数
*****************************************************************************/
void track_main(void);

static void sds_update_backup(void)
{
    nvram_gps_last_data_struct tmp = {0};
    Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
    Track_nvram_write(NVRAM_EF_LOGD_LID, 1, &OTA_NVRAM_EF_LOGD_DEFAULT, NVRAM_EF_LOGD_SIZE);
    Track_nvram_write(NVRAM_EF_PORT_SETTING_LID, 1, &OTA_NVRAM_EF_PORT_SETTING_DEFAULT, NVRAM_EF_PORT_SETTING_SIZE);
    Track_nvram_write(NVRAM_EF_GPS_BACKUP_DATA_PARAM_LID, 1, &OTA_NVRAM_EF_GPS_BACKUP_DATA_PARAM_DEFAULT, NVRAM_EF_GPS_BACKUP_DATA_PARAM_SIZE);
    Track_nvram_write(NVRAM_EF_GPS_POSITION_LID, 1, &tmp, NVRAM_EF_GPS_POSITION_SIZE);
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&OTA_NVRAM_EF_PARAMETER_DEFAULT, NVRAM_EF_PARAMETER_SIZE);
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&OTA_NVRAM_EF_REALTIME_DEFAULT, NVRAM_EF_REALTIME_SIZE);
    Track_nvram_write(NVRAM_EF_PHONE_NUMBER_LID, 1, (void *)&OTA_NVRAM_EF_PHONE_NUMBER_DEFAULT, NVRAM_EF_PHONE_NUMBER_SIZE);
#if defined(__NVRAM_BACKUP_PARTITION__)
    track_cmd_send_to_nvram(0); /* NVRAM_SDS_ACCESS_BACKUP */
#endif /* __NVRAM_BACKUP_PARTITION__ */
    *OTA_sds_update_status = 0;
    track_os_intoTaskMsgQueue2(track_main);
}

/*****************************************************************************
 *  Global Functions            全局函数
*****************************************************************************/

/******************************************************************************
 *  Function    -  track_main2
 *
 *  Purpose     -  开机初始化
 *
 *  Description -  开机后仅调用一次
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_main2(void)
{
#if defined(__JM81__)
    extern void track_cust_mlsp_set_boot_time(void);
#endif
#ifdef __AT_CA__
    track_at_put_init();
    track_at_init_setting();
#endif /* __AT_CA__ */

#if defined( __KEYPAD_MOD__)
    track_cust_keypad_init();
#endif /* __KEYPAD_MOD__ */ 
    tr_start_timer(SYSTEM_BOOTUP_TIMER_ID, 4000, track_system_bootup_delay);
#if defined(__JM81__)
    tr_start_timer(TRACK_CUST_MLSP_SHUTDOWN_TIMER_ID, 1000*60*10, track_cust_mlsp_set_boot_time);
#endif

#ifdef __GT530__
#else
    tr_start_timer(CHECK_NETWORKINFO_TIMER_ID, 2000, track_drv_get_network_info);
    /*开机串口输出+EUSIM:之后才可以判断SIM*/
    tr_start_timer(TRACK_QUERY_SIM_TYPE_TIMER_ID, 5000, track_drv_query_sim_type);
#endif /* __GT530__ */

#if defined(__EXT_VBAT_ADC__)
    track_drv_ext_vbat_measure_req(5, 4);
#endif /* __EXT_VBAT_ADC__ */

#if defined(__NT22__) && defined(__EXT_VBAT_ADC__)
    track_add_adc_vbat_measure2(5, 4,3);
#endif

    /*DRV start*/
#if defined( __WATCH_DOG__)
    track_drv_watdog_init();
#endif /* __WATCH_DOG__ */

    track_drv_gpio_init();
    track_drv_uart_init();

#if defined __USE_SPI__
    track_spi_Init();
#endif

#if defined(__V20__) && defined(__433M__)
    track_cust_433_init();
#endif /* __V20__ */

#if defined(__GT310__)
    track_drv_pwr_on_rled_init();
#endif

#if defined( __GW100__)||defined(__JM81__)||defined (__JM66__)
//no g-sensor
#else
    track_drv_sensor_type_auto_matching();
#endif /* __TRACK_SENSOR__ */

    /*DRV end*/

#if defined (__NT33__)
    #if defined (__FUN_WITH_ACC__)
    //nc
    #else
    GPIO_ModeSetup(30, 0);//没ACC，设置为IO口模式
    track_cust_no_acc_defences_init();
    #endif
#endif

#if defined( __NETWORK_LICENSE__)||defined(__GT530__)
    //nc
#else
    track_socket_apn_init(); // SOC APN Initialization
#endif /* __NETWORK_LICENSE__ */

    track_drv_get_system_param();

    LOGS("\r\npower on");
    track_drv_statistics_conn_time(1);
#if defined (__ET320__)
    LOGS("current project : ET320\r\n");
#elif defined (__ET310__)
    LOGS("current project : ET310\r\n");
#elif defined(__NT36__)
    LOGS("current project : NT36\r\n");
#elif defined(__GT03F__)
    LOGS("current project : GT03D poweron mode:%d\r\n", mmi_Get_PowerOn_Type());
#elif defined(__NT31__)
    LOGS("current project : NT31\r\n");
#elif defined(__NT22__)
    LOGS("current project : NT22\r\n");
#elif defined(__NT33__)
    LOGS("current project : NT33\r\n");
#elif defined(__NT23__)
    LOGS("current project : NT23\r\n");
#elif defined(__GW100__)
    LOGS("current project : GW100\r\n");
#elif defined(__BD300__)
    LOGS("current project : BD300\r\n");
#elif defined(__GT300__)
    LOGS("current project : GT300\r\n");
#elif defined(__GT530__)
    LOGS("current project : GT530\r\n");
#elif defined(__GT500__)
    LOGS("current project : GT500\r\n");
#elif defined(__GT02E__)
    LOGS("current project : GT02E\r\n");
#elif defined(__GT06F__)
    LOGS("current project : GT06F\r\n");
#elif defined(__GT02F__)
    LOGS("current project : GT02F\r\n");
#elif defined(__V20__)
    LOGS("current project : V20\r\n");
#elif defined(__GT530__)
    LOGS("current project : GT530\r\n");
#elif defined(__GT06D__)
    LOGS("current project : GT06D\r\n");
#elif defined(__GT300S__)
    LOGS("current project : GT300S\r\n");
#elif defined(__ET130__)
#if defined(__GPS_G3333__)
    LOGS("current project : ET150\r\n");
#else
    LOGS("current project : ET130\r\n");
#endif
#elif defined(__GW100__)
    LOGS("current project : GW100\r\n");
#elif defined(__GT800__)
    LOGS("current project : GT800\r\n");
#elif defined(__MT200__)
    LOGS("current project : MT200\r\n");
#elif defined(__NT36__)
    LOGS("current project : NT36\r\n");
#elif defined(__NT37__)
    LOGS("current project : NT37\r\n");
#elif defined(__GT370__)
    LOGS("current project : GT370A\r\n");
#elif defined (__GT380__)
    LOGS("current project : GT380\r\n");
#elif defined (__GT741__)
    LOGS("current project : GT741\r\n");
#elif defined(__GT420D__)
    LOGS("current project : GT420D\r\n");

#elif defined(__GT740__) || defined(__GT420D__)
       if(G_importance_parameter_data.app_version.version==2)
   {
             LOGS("current project : NT76\r\n");
   }
    else if(G_importance_parameter_data.app_version.version==3)
    {
             LOGS("current project : NT77\r\n");
    }
    else
    {
            LOGS("current project : NT74\r\n");
    }
#elif defined(__ET350__)
    LOGS("current project : ET350\r\n");
#elif defined(__GT310__)
    LOGS("current project : GT310\r\n");
#elif defined(__JM81__)
    LOGS("current project : JM81\r\n");
#elif defined (__HVT001__)
    LOGS("current project : HVT001\r\n");
#elif defined(__JM66__)
    LOGS("current project : JM66\r\n");
#else
#error "please define project_null"
#endif

#if defined(__GT740__)||defined(__GT420D__)
track_cust_init_full_ele();
#endif

#if defined( __NETWORK_LICENSE__)
    track_drv_network_license_auto_make_call(20000);
    LOGS("CTA CE TEST\r\n");
#endif /* __NETWORK_LICENSE__ */

#if defined (FOR_RS485_TEST_TOOL)
    LOGS("NT34 RS485 TEST\r\n");
#endif /* FOR_RS485_TEST_TOOL */

#if defined( __EX_HW_RESET__)
    LOGS("EX_HW_RESET\r");
#endif /* __EX_HW_RESET__ */
    track_cust_init();

#if defined( __OLED__)
    track_drv_Init_OLED_IC();
    track_cust_lcd_flash();
#endif /* __OLED__ */

#if defined(__CUST_BT__)
    track_cust_bt_init();//必须在UART3初始化之后执行，需要通过UART3配置参数
#endif

#if defined (__NT22__)
    if(G_parameter.bb_data.acc_check  == 0)
    {
    	acc_status_change((void*)1);
    }
#endif /* __NT22__ */
#if defined(__JM66__)
        track_tbt100_check_update_bt();
        track_cust_tbt100_get_bt_mac();
        track_cust_tbt100_get_bt_vbat();
        track_cust_tbt100_set_imei();
#endif//__JM66__
}

/******************************************************************************
 *  Function    -  track_main
 *
 *  Purpose     -  功能函数入口
 *
 *  Description -  开机后仅调用一次
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 29-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_main(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    track_os_log_abnormal_data_init();
    track_init_logd();
    track_os_nvram_init();

    /*初始化STL控件*/
    RamBuffer_Init();
#if !defined (__REMOVE_LOG_TO_FILE__)
    STRINGMGR_INIT();//干掉日志写入文件功能，这个也可删除  chengj
#endif /* __REMOVE_LOG_TO_FILE__ */
    VECTORMGR_INIT();

    track_socket_queue_init();

    track_os_intoTaskMsgQueue2(track_main2);

    app_init_complete_callback();
}

void track_cust_entry_sleep(void)
{
	if(track_status_test(2) == KAL_TRUE)
	{
		LOGD(L_APP, L_V5, "TEST mode");
		return;
	}
#if defined (__JM81__)
    return;
#endif /* __JM81__ */
#if defined (__GT530__)
    LOGD(L_APP, L_V5, "====power on 5 mins entry sleep mode====");
    track_drv_sleep_enable(SLEEP_DRV_MOD, TRUE);

    track_drv_sleep_enable(SLEEP_MOD_MAX, TRUE);
#else
    LOGD(L_APP, L_V5, "====power on 30s entry sleep mode====");
    track_drv_sleep_enable(SLEEP_SMT_MOD, TRUE);
#endif /* __GT530__ */
}

/******************************************************************************
 *  Function    -  track_system_bootup_delay
 *
 *  Purpose     -  开机5  秒后执行一次
 *
 *  Description -  部分功能模块初始化需延时
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 28-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_system_bootup_delay(void)
{
    /******************************* DRV start *************************/
    track_drv_eint_init_registration();
    //track_drv_query_sim_type();
    //track_drv_get_system_param();

	/*开机前30秒禁止休眠 -- chengjun  2017-06-09*/
    LOGD(L_APP, L_V5, "delay sleep 30s");
    track_drv_sleep_enable(SLEEP_SMT_MOD,  KAL_FALSE);
    tr_start_timer(SYSTEM_BOOTUP_TIMER_ID, 30 * 1000, track_cust_entry_sleep);
    track_drv_sys_l4cuem_set_audio_mode(track_status_audio_mode(0xfe));
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
    track_cust_mlsp_boot();
#endif//__MCU_LONG_STANDBY_PROTOCOL__
#if defined(__JM66__)
    track_cust_tbt100_time_sync(KAL_FALSE, NULL);
    track_cust_tbt100_get_sensorpar();
    track_cust_tbt100_get_timepar();
#endif//__JM66__
#if defined(__ATGM_UPDATE__)
    if(track_cust_check_atgm_update_image_exsist() && track_cust_gps_status() == 0)track_cust_gps_work_req((void *)2);
#endif /*__ATGM_UPDATE__*/

        /******************************* DRV end *************************/
}


/******************************************************************************
 *  Function    -  track_app_main
 *
 *  Purpose     -  track应用的task消息处理函数
 *
 *  Description -  从mmi_main.c移动该函数是为了减少OTA接口API和便于升级
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 06-11-2013,  MagicNing  written
 * ----------------------------------------
 ******************************************************************************/
void track_app_main(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/


    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    track_check_tick(0, NULL);
    //track_cust_agps_check_update_tick(1, 0);


    switch(ilm_ptr->msg_id)
    {
        case MSG_ID_MMI_SS_USSD_RSP:
            LOGD(L_SYS, L_V5, "msg_id=MSG_ID_MMI_SS_USSD_RSP");
            track_ussd(ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_TASK_IND:
            track_os_task_expiry(ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_TIMER_EXPIRY:  //定时器超时
            track_timer_expiry(ilm_ptr);
            break;
        case MSG_ID_UART_READY_TO_READ_IND:
            track_drv_uart_ready_read(ilm_ptr->local_para_ptr);
            //LOGD(L_DRV, L_V5, "MSG_ID_UART_READY_TO_READ_IND");
            break;

        case MSG_ID_UART_READY_TO_WRITE_IND:
            LOGD(L_DRV, L_V5, "MSG_ID_UART_READY_TO_WRITE_IND");
            break;

#if defined( __GT530__)
        case MSG_ID_LTE_SYSYTEMUP_IND:
            LOGD(L_DRV, L_V5, "MSG_ID_LTE_SYSYTEMUP_IND");
            track_cust_lte_systemup(1);
            track_cust_lte_systemup_ind();
            break;
#else
        case MSG_ID_GPS_UART_DEBUG_RAW_DATA:
            gps_raw_data_hdler(ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_GPS_EPO_FINESHED_ID:
            track_cust_secset_interface();
            break;
        case MSG_ID_GPS_NMEA_STRING_REQ: //GPS 解码字符串
            GPS_NEMA_Decode_req3(ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_MMI_ABM_PROFILE_QUERY_IND:
            {
                Socket_prof_query_ind_hdlr(ilm_ptr->local_para_ptr);
                break;
            }
#if defined(__DROP_ALM__)
        case MSG_ID_DROP_ALM_IND:
            {
                track_cust_drop_alm();
            }
#endif
        case MSG_ID_APP_SOC_NOTIFY_IND:
            {
                Socket_notify(ilm_ptr->local_para_ptr);
                break;
            }

        case MSG_ID_APP_SOC_GET_HOST_BY_NAME_IND:
            {
                Socket_get_host_by_name(ilm_ptr->local_para_ptr);
                break;
            }

        case MSG_ID_MMI_ABM_BEARER_INIT_IND:
            {
                Socket_store_bearer_init_ind_hdlr(ilm_ptr->local_para_ptr);
                break;
            }

        case MSG_ID_MMI_ABM_BEARER_INFO_IND:
            {
                LOGD(L_SYS, L_V5, "msg_id=MSG_ID_MMI_ABM_BEARER_INFO_IND");
                break;
            }

        case MSG_ID_MMI_ABM_RESET_BEARER_INFO_CNF:
            {
                LOGD(L_SYS, L_V5, "msg_id=MSG_ID_MMI_ABM_RESET_BEARER_INFO_CNF");
                break;
            }

        case MSG_ID_SOC_ABM_DEACTIVATE_IND:
            {
                LOGD(L_SYS, L_V5, "msg_id=MSG_ID_SOC_ABM_DEACTIVATE_IND");
                break;
            }

        case MSG_ID_APP_CBM_BEARER_INFO_IND:
            {
                Socket_cbm_bearer_info_ind_hdlr(ilm_ptr->local_para_ptr);
                break;
            }

        case MSG_ID_MMI_NW_SET_GPRS_CONNECT_TYPE_RSP://网络附着
            {
                track_drv_network_license_gprs_attach_rsp(ilm_ptr->local_para_ptr);
                break;
            }
            /* ABM Socket MSG ID END */

            /* GPS time, calibration system of time */
        case MSG_ID_GPS_UART_P_INFO_IND:        //GPS 开启成功或定位成功
            LOGD(L_DRV, L_V7, " msg_id=MSG_ID_GPS_UART_P_INFO_IND");
            break;

        case MSG_ID_GPS_EPO_READY:  //EPO 初始化完成
            LOGD(L_DRV, L_V7, " msg_id=MSG_ID_GPS_EPO_READY");
            track_drv_gps_epo_start();
            break;
#endif /* __GT530__ */

        case MSG_ID_CHARBAT_BMT_STATUS:  //充电电池状态
            track_drv_charge_batter_status(ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_MOTION_SENSOR_RSP:  //gSensor 触发回调
            track_drv_sensor_callback_handle(ilm_ptr);
            break;

        case MSG_ID_DRIVING_RECORD_IND:  //行车记录检测到异常
            track_drv_driving_record_handle(ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_EINT_TRIGGER_STATUS://中断触发
            track_drv_eint_trigger_msg(ilm_ptr->local_para_ptr);
            break;

#if defined (__KEYPAD_MOD__)
        case MSG_ID_KEYPAD_EVENT://按键事件
            track_drv_keypad_event_handle(ilm_ptr->local_para_ptr);
            break;
#endif /* __KEYPAD_MOD__ */

#if defined (__CUST_BT__)   //蓝牙
        case MSG_ID_BT_EVENTS_ID:
            track_cust_bt_event_handle(ilm_ptr->local_para_ptr);
            break;
#endif

#ifdef __AT_CA__ //MagicNing             
        case MSG_ID_AT_CMD_RSP:  //指令解析
            track_at_get_ctx_from_l4c(ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_TRACK_DATA_ATCI_TO_MMI_REQ:
            //接收串口及L4返回数据
            track_at_put_cmd2_rsp(ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_TRACK_DATA_ATCI_TO_MMI_IND:
            //ATCI 通知 MMI 模拟AT指令已经读取完
            track_at_data_atci_to_mmi_ind();
            break;

#endif/*__AT_CA__   */


#if defined (__LBS_DATA__)
        case MSG_ID_L4C_NBR_CELL_INFO_REG_CNF:
            LOGD(L_DRV, L_V5, " cell info start");
            break;

        case MSG_ID_L4C_NBR_CELL_INFO_IND:
        case MSG_ID_CUST_NBR_CELL_INFO_IND:
            LOGD(L_DRV, L_V5, " cell info update");
            track_drv_multi_cell_info_decode(ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_L4C_NBR_CELL_INFO_DEREG_CNF:
            LOGD(L_DRV, L_V5, "cell info stop");
            break;
#endif /* __LBS_DATA__ */

#if defined( __EXT_VBAT_ADC__)
        case MSG_ID_BMT_ADC_MEASURE_DONE_CONF:
            track_drv_ext_vbat_measure_reslut(ilm_ptr->local_para_ptr);
            break;
#endif /* __EXT_VBAT_ADC__ */

        case MSG_ID_FAKE_CELL_STATUS_ID:
            track_drv_fake_cell_hdlr(ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_CALL_STATUS_NOTICE:   //电话消息
            track_cust_call_status_change(ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_MMI_NW_RX_LEVEL_IND:
            LOGD(L_CMD, L_V5, "MSG_ID_MMI_NW_RX_LEVEL_IND");
            track_set_rssi_by_l4_ind(ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_MMI_SMS_READY_IND:
            LOGD(L_CMD, L_V5, "MSG_ID_MMI_SMS_READY_IND");
            track_cust_eind();
            break;

        case MSG_ID_MMI_CC_CALL_RING_IND:
            LOGD(L_CMD, L_V5, "MSG_ID_MMI_CC_CALL_RING_IND");
            track_call_ring_ind(ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_MMI_CC_DIAL_IND:
            LOGD(L_CMD, L_V5, "MSG_ID_MMI_CC_DIAL_IND");
            track_calll_ata_setup_lrsp_lind(/*CALL_SETUP_LRSP*/1, ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_MMI_CC_ATA_RSP:
            LOGD(L_CMD, L_V5, "MSG_ID_MMI_CC_ATA_RSP");
            track_calll_ata_setup_lrsp_lind(/*CALL_SETUP_ATA*/0, ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_MMI_CC_CALL_DISCONNECT_IND:
            LOGD(L_CMD, L_V5, "MSG_ID_MMI_CC_CALL_DISCONNECT_IND");
            track_calll_release_lind(0, ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_MMI_CC_CALL_RELEASE_IND:
            LOGD(L_CMD, L_V5, "MSG_ID_MMI_CC_CALL_RELEASE_IND");
            track_calll_release_lind(1, ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_MMI_CC_CALL_ACCEPT_REQ_IND:
            LOGD(L_CMD, L_V5, "MSG_ID_MMI_CC_CALL_ACCEPT_REQ_IND");
            track_calll_accept_req_lind(ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_MMI_CC_CALL_CONNECT_IND:
            LOGD(L_CMD, L_V5, "MSG_ID_MMI_CC_CALL_CONNECT_IND");
            track_calll_accept_req_lind(ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_MMI_NW_ATTACH_IND:
            LOGD(L_CMD, L_V5, "MSG_ID_MMI_NW_ATTACH_IND");
            track_nw_attach_ind(ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_MMI_SMS_DELIVER_MSG_IND:
            LOGD(L_CMD, L_V5, "MSG_ID_MMI_SMS_DELIVER_MSG_IND");
            track_sms_status_deliver_lind(ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_MMI_SMS_STATUS_REPORT_IND:
            LOGD(L_CMD, L_V5, "MSG_ID_MMI_SMS_STATUS_REPORT_IND");
            track_sms_status_report_lind(ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_MMI_SMS_MEM_FULL_IND:
            LOGD(L_CMD, L_V5, "MSG_ID_MMI_SMS_MEM_FULL_IND");
            track_sms_mem_full_lind(ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_NVRAM_SDS_CNF:
            track_cmd_nvram_sds_cnf(ilm_ptr->local_para_ptr);
            break;
        default:
            LOGD(L_SYS, L_V4, "msg id undefined ! %d (%d,%d,%d,%d,%d)", ilm_ptr->msg_id,
                 MSG_ID_MMI_ABM_PROFILE_QUERY_IND, /* 15308 */
                 MSG_ID_APP_SOC_NOTIFY_IND,/* 15370 */
                 MSG_ID_APP_CBM_BEARER_INFO_IND,/* 15349 */
                 MSG_ID_L4C_NBR_CELL_INFO_REG_REQ,/* 1351 */
                 MSG_ID_MMI_SMU_GET_PREFERRED_OPER_LIST_REQ);/* 14074 */
            break;
    }

}

