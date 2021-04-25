#ifndef _TRACK_CUST_H
#define _TRACK_CUST_H

/****************************************************************************
*  Include Files                        包含头文件
*****************************************************************************/
#include "MMIDataType.h"
#include "app_datetime.h"
#include "track_cust_main.h"
#include "track_at_sms.h"
#include "track_drv_Eint_gpio.h"
#include "track_drv.h"
#include "track_cmd.h"
#include "track_fun_common.h"
#include "track_soc_Lcon.h"
#include "track_cust_paket.h"
#include "track_cust_led.h"
#include "track_cust_audio.h"
#include "c_RamBuffer.h"
#include "track_os_ell.h"
#include "track_cust_paket_cq.h"
#include "track_cust_mode.h"
#if defined( __OLED__)
#include "track_cust_lcd.h"
#endif /* __OLED__ */
#include "track_os_paramete.h"
#include "track_cust_wifi.h"
#include "track_os_parameter_handle.h"
#include "track_cust_dasouche.h"

#if defined(__ATGM_UPDATE__)
typedef enum{    
	ATGM_UPDATE_STATUS_none = 0,    
	ATGM_UPDATE_STATUS_file_transfer,    
	ATGM_UPDATE_STATUS_update_begin_req,    
	ATGM_UPDATE_STATUS_update_start,    
	ATGM_UPDATE_STATUS_send_file,    
	ATGM_UPDATE_STATUS_send_done,    
	ATGM_UPDATE_STATUS_update_done,    
	ATGM_UPDATE_STATUS_MAX,    
	STATUS_CHECK = 99,
}ATGM_UPDATE_STATUS;
extern U8* track_cust_atgm_return_version(void);
#define ATGM_MAX_FAIL_TIMES 4//失败次数达到此值将不再升级,除非发指令清除
#endif /*__ATGM_UPDATE__*/

/*****************************************************************************
*  Define                               宏定义
**************************
****************************************************/
#if defined (__GT300__)  
#define TRACK_ALARM_DELAY_TIME_SECONDS 5*60*1000     /* 报警延时 */
#elif defined(__GW100__)
#define TRACK_ALARM_DELAY_TIME_SECONDS 3*60*1000     /* 报警延时 */
#else
#define TRACK_ALARM_DELAY_TIME_SECONDS 120000     /* 报警延时 */
#endif /* __GT300__ */
#define TRACK_MAX_LOOP                 20

#if defined( __GT03F__)||defined(__GT500__) ||defined(__ET200__) || defined(__V20__)// || defined(__GT740__)
#else
#define  LOWER_BATTERY_LEVEL 3650000
#endif /* __GT03F__ */
#if defined( __GT740__) || defined(__GT420D__)
#define MAX_RUN_TIME_MIN 5
#define ZKW_EPH_TIMEOUT_MIN 210  
typedef enum
{
	wk_normal=0,
	wk_supplements,
	wk_ams_link,
	wk_rem_alm
	,wk_none,
	wk_collision_alm,
	wk_tilt_alm
}wk_mode;

typedef enum
{
    sw_position_up=0,
    sw_cmd05_en,
    sw_nowifi,
    sw_mode1_nogps,
    sw_mode1_gpsalm,
    sw_lowpower_alm,
    sw_max
}sw_type;
#endif

#define DEFAULT_FEB_TIMESEC 1422748800
#define DEFAULT_SYS_TIMESEC 1420070400
#define RUNING_TIME_SEC ((kal_get_systicks() - 0) / KAL_TICKS_1_SEC)
#define EXCHANGE_MODE_THRESHOLD_VALUESEC 300
extern kal_bool g_GT03D;
#if defined(__ZKE_EPO__)
#define ZKW_EPH_TIMEOUT_MIN 210  
#endif
#if defined (__LUYING__)
#define  RECORD_INTERVAL   30
#endif 

#define LOCATION_MODE_TIMEOUT_SEC 30

#if defined( __TRACK_DISK_NVRAM__)
typedef enum
{
    NVRAM_EF_PARAMETER_LID            =  0,
    NVRAM_EF_IMPORTANCE_PARAMETER_LID ,
    NVRAM_EF_PHONE_NUMBER_LID,
    NVRAM_EF_IOT_PARAMS_LID,
    IMPORT_PARAM_ORG_LID,
    DISK1_LAST_LID,
    DISK1_BACKUP_LID,
    NVRAM_EF_LOGD_LID,
    NVRAM_EF_REALTIME_LID,
    NVRAM_EF_GPS_BACKUP_DATA_LID,
    DISK2_BACKUP_LID,
    DISK1_NVRAM_MAX
} DISK1_NVRAM_ITEM;
#endif /* __TRACK_DISK_NVRAM__ */

#define OBD_FILE_PATH           "OBD.BIN"
#define OBD_MD5_FILE_PATH       "OBD.MD5"
#define FOTA_FILE_PATH          "TRACK.BIN"
#define FOTA_MD5_FILE_PATH      "TRACK.MD5"

#define OTA_FILE_PATH_TMP       "test.tmp"
#define OTA_FILE_PATH           "test.ell"
#define MD5_FILE_PATH           "test.md5"

/*****************************************************************************
*  Typedef  Enum                        枚举定义
*****************************************************************************/
typedef enum
{
    TR_CUST_ALARM_NORMAL                    =    0,   //正常
    TR_CUST_ALARM_SOS                       =    1,   //SOS 求救
    TR_CUST_ALARM_PowerFailure              =    2,   //断电报警
    TR_CUST_ALARM_Vibration                 =    3,   //震动报警
    TR_CUST_ALARM_InFence                   =    4,   //进围栏报警
    TR_CUST_ALARM_OutFence                  =    5,   //出围栏报警
    TR_CUST_ALARM_Speed_Limit               =    6,   //超速报警
    
    TR_CUST_ALARM_displacement              =    9,   //位移报警
    TR_CUST_ALARM_IN_blind                  = 0x0A,   //进GPS盲区报警
    TR_CUST_ALARM_OUT_blind                 = 0x0B,   //出GPS盲区报警
    TR_CUST_ALARM_boot                      = 0x0C,   //开机报警
    TR_CUST_ALARM_gps_successfully          = 0x0D,   //GPS第一次定位报警
    TR_CUST_ALARM_Ext_LowBattery            = 0x0E,   //外电低电报警
    TR_CUST_ALARM_Ext_PowerFailure          = 0x0F,   //外电低电保护报警
    TR_CUST_ALARM_SIM_ALARM                 = 0x10,   //换卡报警
    TR_CUST_ALARM_QUIT_TRANSPORT_ALM     = 0x1A,   //退出运输模式报警
    TR_CUST_ALARM_MAYBE_FAR                 = 0x1B,   //疑似离群
    TR_CUST_ALARM_PWROFF                    = 0x11,   //手动按键关机提醒
    TR_CUST_ALARM_Ext_LowBattery_Fly        = 0x12,   //外电低电保护后进入飞行模式提醒
    TR_CUST_ALARM_PRE_REMOVE                = 0x13,   //拆卸报警  
    TR_CUST_ALARM_door                      = 0x14,   //门报警
    TR_CUST_ALARM_LowBattery_PWROFF         = 0x15,   //内部电池低电即将关机提醒
    TR_CUST_ALARM_SOD                       = 0x16,   //声控报警
    TR_CUST_ALARM_FAKECELL                  = 0x17,   //伪基站报警
    TR_CUST_ALARM_OPEN_SHELL                = 0x18,   //开盖报警（拆外壳）
    TR_CUST_ALARM_LowBattery                = 0x19,   //低电报警
    TR_CUST_ALARM_INTO_DEEP_SLEEP_MODE      = 0x20,   //进入深度休眠报警
    TR_CUST_ALARM_ACC_ON                    = 0x21,   //ACC开（实际值应该是0xFE，但由于非连续值，影响代码逻辑判断）
    TR_CUST_ALARM_ACC_OFF                   = 0x22,   //ACC关（实际值应该是0xFF，但由于非连续值，影响代码逻辑判断）
    TR_CUST_ALARM_DROP_ALM                  = 0x23,   //跌倒报警
    TR_CUST_ALARM_CHARGE_IN_ALM             = 0x24,   //充电器接入报警
    TR_CUST_ALARM_FAR_AWAY_ALM             = 0x26,   //远离报警
    TR_CUST_ALARM_CUT_ALM             = 0x27,   //剪断报警报警
    TR_CUST_ALARM_LINKOUT_ALM             = 0x28,   //主动离线报警
     TR_CUST_ALARM_CRASH_ALM		= 0x2C,		//	碰撞报警
    TR_CUST_ALARM_LEFT_ALM             = 0x31,   //离群报警
    TR_CUST_ALARM_INCLINE_ALM		= 0x4B,		//倾斜报警
    //                                      = 0x62,   //等于98，特殊用途
    //                                      = 0x63,   //等于99，特殊用途
    
    //                                      = 0xFE,   //已占用
    //                                      = 0xFF,   //已占用
    TR_CUST_ALARM_MAX
}enum_alarm_type;

typedef enum
{
    MOTOR_UNLOCK,       // 0
    MOTOR_LOCK,
    MOTOR_ACC_LOW,      // 2
    MOTOR_ACC_HIGH,
    MOTOR_REQ,          // 4
    MOTOR_END_REQ,
    MOTOR_END1,         // 6
    MOTOR_END2,
    MOTOR_END3,         // 8
    MOTOR_END4,
    MOTOR_END5,         // 10
    MOTOR_END6,
    MOTOR_END7,         // 12
    MOTOR_SENSOR_IND
} track_enum_motor;

typedef enum
{
    GPS_STAUS_OFF,       // 0
    GPS_STAUS_ON,
    GPS_STAUS_SCAN,      // 2
    GPS_STAUS_2D,
    GPS_STAUS_3D          // 4
} track_enum_gps_staus;

typedef enum
{
	WF_FIX=0,
	GPS_FIX,
	LBS_FIX
}track_location_level;

typedef enum
{
    LBS_ALM=0,
    GPS_ALM,
    GPS_NO_SAT,
    GPS_NOFIX,
    GPS_FIXED,
    POS_UPED,
    ALM_MAX
}track_enum_teardown_alarm_packet;
/*****************************************************************************
*  Struct                               数据结构定义
******************************************************************************/
typedef struct
{
    kal_bool            addUrl;
    Cmd_Type            cm_type;
    char                phone_num[CM_HEAD_PWD_MXA];
    char                msg_en[CM_PARAM_LONG_LEN_MAX];
    char                msg_cn[CM_PARAM_LONG_LEN_MAX];
    kal_uint16          msg_cn_len;
    kal_uint8           stamp[4];               // 标记位，当前用于网络下发的指令需要保存服务器提供的序列号
    kal_uint8           not_need_almrep;//不需要验证almrep为1,需要验证为0
   kal_uint8            is_sos_alarm_type;//是否为sos报警类型
} struct_msg_send;
#if defined(__XCWS__)
typedef struct
{
    track_gps_data_struct *gd;
    LBS_Multi_Cell_Data_Struct *lbs;
    kal_uint16 status;
    kal_uint8 GSMSignalStrength;
    float battery;
} track_cust_packet_params_struct;

typedef enum
{
    ALARM_NORMAL,                          //正常模式，无报警
    ALARM_SOS,                                 //SOS 报警
    ALARM_BATTERY_LOW,                 //电池低电报警
    ALARM_EXT_BATTERY_LOW,         //外电低电报警
    ALARM_EXT_BATTERY_CUT,          //外电断电报警
    ALARM_VIBRATION,                       //震动报警
    ALARM_FENCE_IN,                        //进围栏报警
    ALARM_FENCE_OUT,                     //出围栏报警
    ALARM_OVER_SPEED,                   //超速报警
    ALARM_MOVE_OUT,                     //位移报警
    ALARM_PRE_REMOVE,                 // 卸载报警/
    ALARM_TYPE_MAX
} alarm_type_enum;


typedef enum
{
    ALARM_GPRS,                             //GPRS 上报到平台
    ALARM_SMS,                              //短信发送到终端
    ALARM_CALL,                             //拨打电话
    ALARM_MODE_MAX
} alarm_mode_enum;
#endif
typedef enum
{
    SMS_NORMAL_MODE,     //正常模式
    SMS_AIRPLANE_MODE,   //  飞行模式
} track_enum_sms_staus;

typedef void (*Func_Extalm_Ptr)(void *);

typedef struct
{
	kal_uint8 alarm_type;
	kal_uint8 alarm_reply[20];
	Func_Extalm_Ptr func;
	U16 no[20];
}Ext_alarm_struct;


/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/
extern nvram_parameter_struct G_parameter;
extern nvram_realtime_struct G_realtime_data;
extern nvram_importance_parameter_struct G_importance_parameter_data;
extern nvram_phone_number_struct G_phone_number;
extern nvram_iot_params_struct G_iot;
#if defined(__JM66__)
extern U8 is_update_mode;
extern U8 is_quit_transport_boot;
extern U8 Jm66_Current_mode;
extern LBS_Multi_Cell_Data_Struct simulation_lbs;
#endif//__JM66__
/*****************************************************************************
*  Functions Define                     函数声明
******************************************************************************/
extern kal_bool track_cust_gps_need_open();
extern void track_cust_shutdown_condition(void* par);
extern U16 track_cust_lock_serial_no(U8 index,int no,kal_uint8 reply,Func_Extalm_Ptr func);
extern void track_cust_set_gps_status(track_enum_teardown_alarm_packet par);
extern void lowbattery(void * arg);
extern char *track_get_app_version(void);
extern track_gps_data_struct * track_drv_get_gps_data(void);
extern void track_cust_linkout_alarm();
extern float track_fun_get_GPS_2Points_distance(float lat1, float lon1, float lat2, float lon2);
extern kal_uint16 track_cust_paket_2C(void);
extern void track_cust_init(void);
extern void track_cust_sensor_init(void);
extern void track_cust_gps_decode_done(track_gps_data_struct *track_gps_data);
extern void track_cust_eint_trigger_response(U8 eint, U8 level);
extern void track_cust_chargestatus_vbtstatus_check(pmic_status_enum status, battery_level_enum level, kal_uint32 volt);
extern void track_cust_periodic_restart(void *arg);
extern void track_cust_call_incomming_number(char *inc_number);
extern void track_cust_call_status(kal_uint16 callState);
extern void track_cust_call(char *number);
extern void track_cust_call_hangup(void);
extern void track_cust_call_accept(void);
extern char *track_cust_call_get_incomming_number(void);
extern void track_cust_net_conn(void);
extern void track_cust_sms_send(kal_uint8 number_type, char *number, kal_bool isChar, kal_uint8 *data, kal_uint16 len);
#if !defined(__GT420D__)
extern void track_cust_module_get_addr(CMD_DATA_STRUCT *cmd);
#endif
extern void track_cust_module_recv_addr(char *number, kal_uint8 *content, int content_len, kal_uint16 sn);
extern void track_cust_module_power_fails(void *arg);
extern void track_cust_module_alarm_vibrates(void *arg);
extern kal_bool track_cust_module_alarm_msg_send(struct_msg_send *msg, track_gps_data_struct *gps_data);
extern void track_cust_status_defences_change(void *arg);
extern void track_cust_add_position(void);
extern void track_cust_paket_13(kal_uint8* data, kal_uint16 *len, kal_uint16 *sn);
extern kal_uint16 track_cust_paket_1A(track_gps_data_struct *gd, char *number);
extern kal_uint16 track_cust_paket_position(track_gps_data_struct *gd, LBS_Multi_Cell_Data_Struct *lbs, kal_uint8 packet_type, kal_uint8 packet_status);
extern void cust_packet_log(kal_uint8 errorid, kal_uint8 type, kal_uint8 filter, kal_uint16 sn, float _distance, float lat, float lon);
extern kal_uint8 track_cust_make_call(void *arg);
extern track_gps_data_struct *track_cust_backup_gps_data(kal_uint8 type, track_gps_data_struct *new_data);
extern kal_bool track_is_testmode(void);
extern kal_int8 track_cust_get_sim_status(void);
extern nvram_gps_time_zone_struct* track_mcc_to_timezone(void);
extern void track_cust_write_sim_backup_for_parameter(void);
extern void track_soc_gprs_reconnect(void *arg);
extern void track_soc_disconnect(void);
extern void track_check_tick(kal_uint8 st, char *outstring);
extern void track_cust_sim_backup_for_parameter(void);
extern void track_cust_led_first_start(void);
extern track_enum_gps_staus track_cust_gps_status(void);
extern void track_cust_module_alarm_blind(void);
extern void track_cust_alarm_boot(void);
extern void track_cust_alarm_gps_successfully(void);
extern track_gps_data_struct *track_cust_gpsdata_alarm(void);
extern void track_cust_stop_update_LBS(void);
extern void track_cust_update_LBS(void);
extern enum_alarm_type track_cust_alarm_type(enum_alarm_type type);
extern void track_cust_gps_send_data(kal_uint32 status);
extern kal_uint8 track_cust_get_work_mode(void);
extern void track_cust_gps_check_nofix(kal_uint8 flag);
extern char *track_domain_decode(kal_uint8 *data);
extern void track_domain_encode(char *src, kal_uint8 *dec);
extern void track_cust_gps_off_req(void *arg);
extern void track_cust_gps_work_req(void *arg);
extern kal_uint16 track_get_incall_type(void);
extern void track_cust_check_sos_incall(CMD_DATA_STRUCT * Cmds, kal_int8 state);
extern void track_cust_agps_start(void);
extern kal_bool track_cust_is_upload_UTC_time(void);
extern void track_cust_set_vcama(kal_bool ON, kal_uint32 ldo_volt);
extern kal_uint8 track_cust_serverset(kal_uint8 sta);
extern kal_bool track_cust_is_china_sales(void);
#if defined(__GT530__)
extern void track_cust_lte_pwron(kal_uint8 s);
extern void track_drv_lte_server_close(kal_uint8 app);
extern kal_bool track_cust_lte_ready(kal_uint8 par);
extern void track_cust_pre_lte_sleep(void);
extern void track_cust_lte_wakeup(void);
#endif
extern void track_cust_send_lbs(void *par);
extern void track_cust_get_lbs_data(void * arg);
extern void track_cust_instant_contract_position_data(kal_uint8 flag);
extern void track_cust_secset_interface();
extern void track_cust_aiding_reftime (applib_time_struct *currTime);
extern void track_drv_ext_vbat_measure_modify_parameters();
extern void heartbeat_packets(void *arg);
#if defined(__XCWS_TPATH__)
extern void track_cust_server3_conn(void);
#endif

#if defined(__FLY_MODE__)
extern kal_bool track_cust_flymode_set(U32 mode);
extern kal_bool track_cust_isbatlow(void);
#endif /* __FLY_MODE__ */

#if defined(__BCA__)
extern void track_cust_paket_29(kal_uint8* data, kal_uint16 *len, kal_uint16 *sn);
#endif /* __BCA__ */

#if defined(__WIFI__)
extern void track_drv_wifi_work_mode(void);
extern void track_cust_sensor_to_rest_supp_wifihot();
#endif /* __WIFI__ */

#if defined(__GT420D__)
extern void track_cust_wifi_work_mode(void);//定时模式，GSM常开情况
#endif

extern void led_level_state_switch();
extern void track_cust_sendlbs_limit(void);
extern U32 track_cust_mode3_extchange(U32 startTime_mins, U16 interval_mins);
extern void track_cust_mode1_gpsfix_overtime();
extern kal_bool track_is_notquit(void);

#if defined(__GT740__)||defined(__GT420D__)
extern void track_cust_gps_search_timeout();
extern void track_cust_soc_plugup_timeout();
extern void track_cust_module_deal_end_trackmode();
extern void track_cust_update_trackmode();
extern void track_cust_module_overtime_upwifi();
extern void track_cust_gps_module_work(void *par);
extern void track_cust_set_st_led_on(void* par);
#endif
extern void track_os_importance_backup_read(void);
extern void track_fun_quicksort(kal_uint16 a[], int low, int high);
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
void track_mlsp_send_new();
#endif//__MCU_LONG_STANDBY_PROTOCOL__

#endif  /* _TRACK_CUST_H */

