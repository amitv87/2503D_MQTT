/******************************************************************************
 * track_os_paramete.h - 
 * 
 * Copyright .
 * 
 * DESCRIPTION: - 
 *        通用头文件 
 * 
 * modification history
 * --------------------
 * v1.0   2012-07-14,  jwj create this file
 * 
 ******************************************************************************/

#ifndef _TRACK_PARAMETE_H
#define _TRACK_PARAMETE_H

/****************************************************************************
* Include Files             包含头文件                                             
*****************************************************************************/
#include "track_os_data_types.h"
#include "track_version.h"
#include "track_os_log.h"
#include "track_drv_lbs_data.h"
/*****************************************************************************
 *  Define			    宏定义
*****************************************************************************/
#ifdef __TRACK_DISK_NVRAM__
#if defined(__JM66__)
#define ONETIME_MAC_MAX_LEN 60//蓝牙发送牛群信息一次最多的MAC长度
#define PACKET_MAC_MAX 21
#define PACKET_MAC_MAX_LEN  PACKET_MAC_MAX * 6 + 7 + 2
#define LBS_LIST_MAX 20 //基站列表大小
#define BT_UPDATE_IMAGE "TBT100_BT_IMAGE"
#define BT_UPDATE_ONCE_DATA_MAX_LEN 2048
#define BT_UPDATE_FIRST_DATA_MAX_LEN BT_UPDATE_ONCE_DATA_MAX_LEN - 12
#endif//__JM66__

#define MAX_APN_LEN             100
#define MAX_PDP_ADDR_LEN        16
#define TCM_MAX_PEER_ACL_ENTRIES	 	10

#define MAX_GPRS_USER_NAME_LEN 32
#define MAX_GPRS_PASSWORD_LEN  32
#define MAX_GPRS_APN_LEN       100
#define MAX_GPRS_IP_ADDR       4 /* IP address length */
#define TRACK_MAX_URL_SIZE                 99       // URL 允许最大的字符数

#if !defined(__GT420D__)
#define BACKUP_DATA_MAX                  1000       // NVRAM最大允许备份条数
#else
#define BACKUP_DATA_MAX                  200      // NVRAM最大允许备份条数
#endif

#if defined(__XCWS__)
#define BACKUP_DATA_PACKET_MAX             150
#elif defined(__NT23__)
#define BACKUP_DATA_PACKET_MAX             50
#elif defined(__GT740__)
#define BACKUP_DATA_PACKET_MAX             150
#elif defined(__GT420D__)
#define BACKUP_DATA_PACKET_MAX             400
#else
#define BACKUP_DATA_PACKET_MAX             45
#endif
#if defined(__GT03F__)
#define TRACK_DEFINE_SOS_COUNT              4       // 支持 SOS 号码的个数
#else
#define TRACK_DEFINE_SOS_COUNT              3       // 支持 SOS 号码的个数
#endif/*__GT03F__*/
#define TRACK_DEFINE_FN_COUNT               2       // 支持 亲情 号码的个数
#define TRACK_DEFINE_WN_COUNT               15
#define TRACK_DEFINE_PHONE_NUMBER_LEN_MAX  21       // 电话号码最大允许长度
#if defined (__GT310__) || defined(__GW100__)||defined(__GT300S__)||defined(__GT370__)|| defined (__GT380__)
#define TRACK_DEFINE_FENCE_SUM              5       // 电子围栏个数
#elif defined (__GT500__)
#define TRACK_DEFINE_FENCE_SUM             10       // 电子围栏个数
#else
#define TRACK_DEFINE_FENCE_SUM              1       // 电子围栏个数
#endif /* __GT03F__ */
#if defined(__XCWS__)
#define TRACK_PASSWD_LEN_MAX                20      // 密码字符最大长度
#define TRACK_POLYGON_MAX                   55      //多边形电子围栏顶点数最大值
#endif




#define NVRAM_EF_GPS_BACKUP_DATA_SIZE sizeof(BACKUP_GPS_DATA)
#define NVRAM_EF_GPS_BACKUP_DATA_TOTAL BACKUP_DATA_MAX
#define NVRAM_EF_LOGD_TOTAL     1
#define NVRAM_EF_LOGD_SIZE      sizeof(nvram_ef_logd_struct)
#define NVRAM_EF_REALTIME_SIZE sizeof(nvram_realtime_struct)
#define NVRAM_EF_DISPLACEMENT_REALTIME_TOTAL 1

#define NVRAM_EF_PARAMETER_SIZE sizeof(nvram_parameter_struct)

#define NVRAM_EF_IMPORTANCE_PARAMETER_SIZE sizeof(nvram_importance_parameter_struct)
#define NVRAM_EF_PHONE_NUMBER_SIZE sizeof(nvram_phone_number_struct)

#define NVRAM_EF_ALONE_PARAMTER_SIZE sizeof(nvram_alone_parameter_struct)

#define NVRAM_EF_IOT_PARAMS_SIZE sizeof(nvram_iot_params_struct)

//#define NVRAM_EF_DRV__IMP_BACKUP_SIZE   sizeof(nvram_drv_importance_backup_struct)

/***************************************************************************** 
* Typedef  Enum         枚举
*****************************************************************************/
typedef enum
{
    LOG_AUTO,
    LOG_SERIAL,
    LOG_CATCHER,
    LOG_2FILE,
    LOG_MAX
} log_target_enum;

/*****************************************************************************
 *  Struct			    数据结构定义
*****************************************************************************/
#if defined(__DASOUCHE__)
typedef struct
{
	kal_uint8					   is_change_pk;				//是否更改了PK，默认0
	kal_uint8				   	   reconn_acount;					//改了PK后，20分钟不联网重启次数，默认0
	kal_uint8                       pk_factory;                    //PK触发阈值，默认70           
	char                            requestId[100];
} nvram_ischange_pk_struct;
#endif

typedef struct
{
    char product_key[20];
    char product_secret[48];
    char device_secret[48];
    char device_name[48];
}nvram_iot_params_struct;

#if defined(__JM66__)    
typedef struct
{
    kal_uint8              scan_interval;//扫描间隔
    kal_uint8              scan_last;//扫描时长
    kal_uint8              broadcast_interval;//广播间隔
    kal_uint16            broadcast_last;//广播时长
}jm66_bt_timepar_struct;
typedef struct
{
    kal_uint8              sensor_start_hour;//G-Sensor开始监测时间0x061E   (开始时间为6:30)
    kal_uint8              sensor_start_min;//G-Sensor开始监测时间0x061E   (开始时间为6:30)
    kal_uint8              sensor_end_hour;//G-Sensor结束监测时间0x151E   (结束时间为21:30)
    kal_uint8              sensor_end_min;//G-Sensor结束监测时间0x151E   (结束时间为21:30)
    kal_uint8                sensor_mode;//G-Sensor工作模式
}jm66_sensor_struct;

#endif/*__JM66__*/
#if defined(__JM81__)    
typedef struct
{
    kal_uint8                mode;//当前工作模式
    kal_uint16              Mode1_T1;
    kal_uint16              Mode1_T2;
    kal_uint8                Mode2_day;
    kal_uint8                Mode2_hour;
    kal_uint32              Mode99_Min;
}JM81_mode_struct;
#endif/*__JM81__*/
typedef struct
{
    kal_uint8                filename[20];
    kal_uint8                index;
    kal_uint8                not_read;
}audio_file_struct;

typedef struct
{
    kal_uint8   zone;
    kal_uint8   time;
    kal_uint8   time_min;
    kal_uint8   timezone_auto;
} nvram_gps_time_zone_struct;

typedef struct
{
	kal_uint8 sw;
	kal_uint16 up_interval_time;
}nvram_gpsup_struct;

typedef struct
{
    kal_uint8                                   sw;                   // ON/OFF；
    kal_uint8                                   car;                  // Car Type NO (1 仅OBD， 2 丰田， 4 本田， 5 大众， 6 通用)
    kal_uint8                                   upload_type;          // 上传类型
    kal_uint8                                   hard_acceleration;    // 急加速门限值
    kal_uint8                                   rapid_deceleration;   // 急减速门限值
    char                                        ver[20];              // OBD 版本
    kal_uint16                                  upload_time;          // 定时上报数据的间隔时间
    kal_uint16                                  displacement;         // 车辆排量
    kal_uint16                                  fuel_consumption;     // 车辆百公里平均油耗
    kal_uint32                                  total_mileage;        // 标定默认总里程
} nvram_obd_parameter_struct;

typedef struct
{
    kal_uint16  version;                                         // 1为谷米，2为通用，3为外贸，4为图强(1.NT74 2.NT76  3.NT77)
    kal_uint8   support_sensor;                                 //是否支持sensor,0为不支持，1为支持
    kal_uint8   support_WM;                                 //是否为外贸版，主要控制apn修改，0为不是，1为是
    kal_uint8   server_lock;                                  //是否锁定域名，0为不锁定，1为锁定
}app_version_struct;

#if defined(__XCWS__)
typedef struct
{
    kal_uint16  MachineType;
    kal_uint8   ProtocolVersion[8];                                 //版本
    kal_uint8   SequenceNumber[19];                                 //终端序列号
    kal_uint8   host_num[TRACK_DEFINE_PHONE_NUMBER_LEN_MAX];        //车主号码
    kal_uint8   owner_num[TRACK_DEFINE_PHONE_NUMBER_LEN_MAX];       //本机SIM卡号码
    kal_uint8   imsi_num[18];                                       //imsi号
    kal_uint8   password[20];                     //用户密码TRACK_PASSWD_LEN_MAX
} nvram_ef_device_setting_struct;

typedef struct
{
    kal_uint8  t_sleep;             //是否进入休眠
    kal_uint8  t_wakeup;        //是否唤醒
    kal_uint16 GSM_Sleep_Starting_Time_interval;//唤醒时间
    kal_uint16 chargerout_wait;         //断电后进入休眠延时
} nvram_sleep_param_struct;
#endif

#if defined (__XYBB__)
typedef struct
{
    kal_uint8   sim_num[12];        						   //SIM卡号
    kal_uint8   provincial[8];                                 //省域ID
    kal_uint8   manufacturer[8];                               //制造商ID
    kal_uint8   terminal_model[22];                            //终端型号
    kal_uint8   terminal_id[10];       						   //终端ID
    kal_uint8   car_color;									   //车辆颜色
    kal_uint8   car_num[20];                                  //车牌号码
    kal_uint8  	mode;											//注册模式    
} nvram_ef_bb_login_setting_struct;


typedef struct
{ 
    kal_uint8  authorize_code[20];								//授权码
    kal_uint8  acc_check;										//ACC检测
} nvram_ef_bb_setting_struct;


#endif /* __XYBB__ */

#if defined (__OIL_DETECTION__)
typedef struct
{
    kal_uint8 sw;
    kal_uint16 timer;
    kal_uint8 type;
}nvram_oil_struct;
#endif /* __OIL_DETECTION__ */

#if defined (__MULTIPLE_OUTPUT__)
typedef struct
{
    kal_uint16 sw;//这个并不是总开关，只是表示脉冲输出还剩多少周期
    kal_uint32 out_continue_time;//持续时间单位:毫秒
    kal_uint32 out_interval_time;//间隔时间单位:毫秒
    kal_uint16 out_times;//次数
    kal_uint8  output_always;//配合指令Relay2/Relay3奇葩功能，长输出与脉冲输出模式，按后到覆盖前者
}nvram_outset_struct;
#endif /* __MULTIPLE_OUTPUT__ */

/*  begin  NVRAM备份定位数据   */
typedef struct
{
    kal_uint16  len;
    kal_uint8   buf[BACKUP_DATA_PACKET_MAX];
} BACKUP_GPS_DATA;

typedef struct
{
    kal_uint8           logd_module[L_MAX];
    kal_uint8           log_sw;
    log_target_enum     logd_target;
    char                log_file_name[32];
} nvram_ef_logd_struct;

typedef struct
{
    kal_uint8           sw;             // 0=无效  1=启用
    kal_uint8           in_out;         // 0：IN,入围栏报警，1：OUT出围栏报警，2：为空,进/出围栏报警；默认为进出围栏都报警
    kal_uint8           alarm_type;     // 报警类型（0 仅GPRS，1 SMS+GPRS）
    kal_uint8           square;         // 0=圆形  1=矩形
    kal_uint32          radius;         // 半径(米)
    float               lon;
    float               lat;
    float               lon2;
    float               lat2;
} nvram_electronic_fence_struct;

typedef struct
{
    kal_uint8           sw;             // 开启超速报警（默认为关闭）
    kal_uint8           threshold;      // 超速门限范围（km/h）：1-255 ，默认 100km/h
    kal_uint8           alarm_type;     // 报警上报方式（0 仅GPRS；1 SMS+GPRS）
    kal_uint16          delay_time;     // 时间范围（秒）：5-600S  ，默认 20S；
} nvram_speed_limit_alarm_struct;

typedef struct
{
    kal_uint8           sw;                         //
    kal_uint8           alarm_type;                 // 上报方式（默认为 1）(0=only GPRS,1=GPRS+SMS,2=GPRS+SMS+Call,3=GPRS+CALL)
    kal_uint16          delay_time_off;             // 范围：2-60 S,默认 5S
    kal_uint16          delay_time_on;              // 范围：1-3600 S,默认 300S
    kal_uint16          delay_time_acc_on_to_off;   // ACC ON to OFF 跳变禁止报警时间，默认 180 秒
#ifdef __NT31__
    kal_uint16          send_alarm_time;   //  断开报警不停发送报警包的间隔时间，默认为0
#endif /* __NT32__ */

} nvram_power_fails_alarm_struct;

typedef struct
{
    kal_uint8           sw;                     // 震动报警 开/关
#if defined(__JM81__)
    kal_uint8          sensor_sw;                       //传感器开关
#endif
    kal_uint8           alarm_type;             // (0=only GPRS,1=GPRS+SMS,2=GPRS+SMS+CALL,3=GPRS+CALL)
    kal_uint16          detection_time;         // 检测时间（秒），默认为：10
    kal_uint16          detection_count;        // 在检测时间内，检测满足的次数，默认为：5
    kal_uint8           sampling_interval;      // 检测次数采样最短间隔，默认为：1
    kal_uint32          alarm_interval;         // 最短报警间隔（秒）
    kal_uint32          alarm_delay_time;       // 报警延时（秒），默认为：180
    kal_uint8           sensitivity_level;      // 灵敏度等级，默认值为：16
} nvram_vibrates_alarm_struct;
typedef struct
{
    kal_uint8           sw;                     // 震动报警 开/关
    kal_uint8           alarm_type;             // (0=only GPRS,1=GPRS+SMS,2=GPRS+SMS+CALL,3=GPRS+CALL)
    kal_uint8           door_status;		//车门检测触发电平，0=低电平触发，1=高电平触发
    /*实际硬件三极管集电极反向，外界高触发低中断，因此方向处理，如设为1实际在中断收到低时报警 -- chengjun  2017-07-05*/
    #if defined (__NT37__)
    kal_uint8           alarm_enable;		//报警使能，一次报警后为0，不能重发发，反向后清除
    #endif
} nvram_door_alarm_struct;
typedef struct
{
    kal_uint8           sw;
    kal_uint8           alarm_type;                      // (0=only GPRS,1=GPRS+SMS)
#if defined(__XCWS__)||defined(__GT420D__)
    kal_uint32          threshold;                       // 低电量告警阈值，默认为 3700000，即 3.7v
#endif
} nvram_low_power_alarm_struct;

typedef struct
{
    kal_uint8           sw;                              // SOS报警是否启用（默认为开启）
    kal_uint8           alarm_type;                      // SOS报警上报方式（默认为 1）(0=only GPRS,1=GPRS+SMS,2=GPRS+SMS+Call,3=GPRS+CALL)
#if defined(__SPANISH__)    
    kal_uint8           key_type;                        // 0  短按（按一次就触发）; 1  长按（时间3秒）(默认)
    char                alarm_sms_head[100];             // 用于自定义报警提醒短信内容
#endif    
} nvram_sos_alarm_struct;


typedef struct
{
    kal_uint8           sw;                              // 位移报警开关（开：1，关：0）
    kal_uint8           alarm_type;                      // 0～2；0 仅GPRS，1 SMS+GPRS，2 GPRS+SMS+电话,3=GPRS+CALL；默认值：1
    kal_uint16          radius;                          // 位移半径（单位：米）100～1000；默认值：300
} nvram_displacement_alarm_struct;

typedef struct
{
    kal_uint8           sw;                              // 停车超时开关（开：1，关：0）
    kal_uint8           alarm_type;                      // 报警类型（0 仅GPRS，1 SMS+GPRS）
    kal_uint16          time;                            // 停车时长；5～3600分钟；停车时长；默认值：60
} nvram_carpark_alarm_struct;

typedef struct
{
    kal_uint8           sw;                              // 疲劳驾驶报警（开：1，关：0）默认开
    kal_uint8           alarm_type;                      // 报警上报方式（0 仅GPRS；1 SMS+GPRS）默认0
    kal_uint16          delay_time;                      // 1～3600分钟；疲劳驾驶持续时间，默认240分钟
    kal_uint16          delay_reset;                     // 1～3600分钟；驾驶清零计时时间，默认20分钟
} nvram_fatigue_driving_alarm_struct;

typedef struct
{
    kal_uint8           sw;                              // GPS盲区报警（开：1，关：0）
    kal_uint8           alarm_type;                      // 报警上报方式（0 仅GPRS；1 SMS+GPRS）
    kal_uint8           in_blind_area;                   // 1～10分钟；进入盲区报警延时上报时间；默认值：3分钟
    kal_uint8           out_blind_area;                  // 1～10分钟；离开盲区延时上报时间；默认值：1分钟
} nvram_blind_area_alarm_struct;

typedef struct
{
    kal_uint8           sw;                              // ON/OFF；默认值：ON
    kal_uint8           alarm_type;                      // 报警上报方式（0 仅 GPRS，1 SMS+GPRS；默认值：1）
    kal_uint16          threshold;                       // 10～360；默认值：125
    kal_uint16          recover_threshold;               // 10～360；默认值：130
    kal_uint16          detection_time;                  // 1～300秒；默认值：10 秒
} nvram_power_extbatalm_struct;

typedef struct
{
    kal_uint8           sw;                              // ON/OFF；默认值：ON
    kal_uint8           alarm_type;                      // 报警上报方式（0 仅 GPRS，1 SMS+GPRS；默认值：1）
    kal_uint16          threshold;                       // 10～360；默认值：125
    kal_uint16          recover_threshold;               // 10～360；默认值：130
    kal_uint16          detection_time;                  // 1～300秒；默认值：10 秒
} nvram_power_extbatcut_struct;

typedef struct
{
    kal_uint8           sw;                              // ON/OFF；默认值：ON
    kal_uint8           alarm_type;                      // 报警上报方式（0 仅 GPRS，1 SMS+GPRS, 2 SMS+GPRS+CALL,3 GPRS+CALL；默认值：1）
#if defined(__GT740__) ||defined (__GT420D__)
    kal_uint8           alarm_upcount;//上传次数1-99 默认3
    kal_uint8           alarm_vel_min;//间隔分钟15
#endif
} nvram_teardown_struct;

typedef struct
{
 	kal_uint8           sw;						//ON/OFF		默认值OFF
	 kal_uint8           alarm_type;  	//报警上报方式，0 仅GPRS，1 SMS+GPRS.默认值0；
	kal_uint16			crash_grade;		//     10- 1024，碰撞等级，默认值800
}nvram_crash_struct;

typedef struct
{
 	kal_uint8           sw;						//ON/OFF		默认值OFF
	kal_uint8           alarm_type; 		 //报警上报方式，0 仅GPRS，1 SMS+GPRS.默认值0；
	kal_uint16			incline_grade;		//     1-180，倾斜角度，默认值待定
}nvram_incline_struct;

typedef struct
{
    kal_uint8           sw;                              // ON/OFF；默认值：ON
    kal_uint8           alarm_type;                      // 报警上报方式（0 仅 GPRS，1 SMS+GPRS, 2 SMS+GPRS+CALL,3 GPRS+CALL；默认值：1）
} nvram_charge_alarm_struct;


typedef struct
{
    kal_uint8           sw;                              // ON/OFF；默认值：ON
    kal_uint8           sacn_timer;                      // 搜索时间
    kal_uint16          work_timer;                      // 工作间隔时间
}nvram_wifi_struct;
typedef struct
{
    kal_uint16          acc_l_threshold_vbat;                       // 10～360；默认值：132
    kal_uint16          acc_h_thresholdvbat;               // 10～360；默认值：135
} nvram_ext_vbat_check_acc_struct;

typedef struct
{
    kal_uint8                      mode;                 // 0/1,0 定时上传，1 定距上传
    kal_uint16                     interval_acc_on;      // 0、5～18000秒，ACC ON 状态下上传间隔；默认值：10
    kal_uint16                     interval_acc_off;     // 0、5～18000秒，ACC OFF 状态下上传间隔；默认值：0
    kal_uint16                     distance;             // 0、50～10000米，定距距离；默认值：300
} nvram_gps_report_mode_struct;

typedef struct
{
    kal_uint16                     detection_time;       // 检测时间（秒），默认为：10
    kal_uint16                     detection_count;      // 震动激活GPS触发次数门限，在检测时间内，检测满足的次数，默认为：3
    kal_uint8                      sampling_interval;    // 检测次数采样最短间隔，默认为：1
} nvram_vibrates_start_gps_struct;

typedef struct
{
    kal_uint16                     modeset;       // 检测时间（秒），默认为：10
    kal_uint16                     exercise_check_time;      // 震动激活GPS触发次数门限，在检测时间内，检测满足的次数，默认为：3
    kal_uint8                      exercise_sensorcount;    // 检测次数采样最短间隔，默认为：1
    kal_uint16                     static_check_time;      // 震动激活GPS触发次数门限，在检测时间内，检测满足的次数，默认为：3
    kal_uint8                      static_sensorcount;    // 检测次数采样最短间隔，默认为：1
} nvram_sensortime_struct;

typedef struct
{
    kal_uint8                      sw;                   // 拐点补传开启/关闭
    kal_uint8                      angleOfDeflection;    // 30～180度，默认值：60度，偏转角度
    kal_uint8                      detection_time;       // 2～10秒，默认值：5秒，检测时间
} nvram_gps_anglerep_struct;

typedef struct
{
    kal_uint8                      sw;                   // 定距上传 开启/关闭
    kal_uint16                     distance;             // 0、50～10000米，定距距离；默认值：300
} nvram_gps_fixed_distance_struct;

typedef struct
{
    kal_uint8                      sw;                   // 定时上传 开启/关闭
    kal_uint16                     interval_acc_on;      // 0、5～18000秒，ACC ON 状态下上传间隔；默认值：10
    kal_uint16                     interval_acc_off;     // 0、5～18000秒，ACC OFF 状态下上传间隔；默认值：0   ，最大24h-86400秒   -- chengjun  2017-04-25
    kal_uint16                     lbs_interval;         // lbs时间间隔
    kal_uint16                     mode1_gps_time;       // gt300模式一GPS时间间隔
} nvram_gps_fixed_time_interval_struct;

typedef struct
{
    kal_uint8                            static_filter_sw;              // 静态数据过滤开关
    kal_uint16                           static_filter_distance;        // 静态数据最小距离(米)默认值:100米
    kal_uint8                            ephemeris_interval;            // GPS 星历更新时间设置；60～240分钟，默认值：60分钟
    kal_uint8                            accrep;                        // ACC状态变化立即上报功能开启/关闭
    kal_uint8                            location_timeout;              // 搜星超时(分)
    nvram_vibrates_start_gps_struct      vibrates;                      // 震动唤醒GPS的条件
    nvram_gps_anglerep_struct            anglerep;                      // 拐点补传
    nvram_gps_fixed_distance_struct      Fd;                            // 定距上传
    nvram_gps_fixed_time_interval_struct Fti;                           // 定时上传
} nvram_gps_work_struct;

typedef struct
{
    kal_uint8                      sw;                   // 里程统计 开启/关闭
    kal_uint32                     mileage;              // 里程 km/h
    kal_uint32                     K;              //  系数k，默认1000

} nvram_mileage_statistics_struct;

typedef struct
{
    char                           password[20];         // 用户密码
    char                           superCmd[20];         // 超级指令头
    kal_uint8                      password_sw;          // 密码控制指令开/关
    kal_uint8                      Permit;               // SOS与中心号码权限控制，默认为0
    kal_uint8                      SOSPermit;            // 参数的查询设置权限，默认为0
    kal_uint8                      AlarmNumCount;        // 报警的发送号码个数，默认为1
    kal_uint8                      MonPermit;            // 监听权限控制，默认为2
#if defined (__GT310__)
    kal_uint8                      CallPermit;            // 通话权限控制，默认为2    
#endif
} nvram_Permission_settings_struct;

typedef struct
{
    kal_uint8                      sw;                   // GPRS阻塞报警
    kal_uint8                      link;                 // GPRS重连次数
} nvram_GPRS_Obstruction_struct;

typedef struct
{
    kal_uint8                      sw;                   // ON/OFF；SENSOR 检测静止上报最后位置开关；默认值：OFF
    kal_uint16                     detection_time;       // 10～300 秒；时间范围；默认值：180 秒
    kal_uint16                     speed;                // 运动判断的最小速度；默认值：6km/h
    kal_uint16                     count;                // 0～100 连续监测的个数
} nvram_staticrep_struct;

typedef struct
{
    kal_uint8                      sw;                   // ON/OFF；盲区报警开关
    kal_uint16                     blind_in_time;        // gps退出盲区之后gps定位进入盲区多久报告
    kal_uint16                     blind_out_time;       // gps在进入盲区之后gps定位推出盲区多久报告
    kal_uint8                      alarm_type;           // M=0 仅GPRS;M=1 SMS+GPRS；默认上报方式为1//
} nvram_blind_struct;

typedef struct
{
    kal_uint8                      sw;                   // ON/OFF；开机报警开关
    kal_uint8                      alarm_type;           // M=0 仅GPRS;M=1 SMS+GPRS；默认上报方式为1//
} nvram_boot_struct;

typedef struct
{
    kal_uint8                      sw;                   // ON/OFF；伪基站报警
    kal_uint8                      alarm_type;           // M 上报方式
    kal_uint16                     alarm_filter;         //伪基站标志有效时间，单位 分
} nvram_FCFALM_struct;

typedef struct
{
    kal_uint8                      sw;                   // ON/OFF；
    kal_uint8                      mode;                 // 检测模式；
    kal_uint32                     lock_on_time;         // 每次锁的时间；默认值：3000 毫秒
    kal_uint32                     lock_off_time;        // 每次锁释放的间隔时间，默认值：40毫秒
    kal_uint32                     lock_continue_time;   // 在手动解锁之前循环锁持续的时间，默认值：20秒
} nvram_motor_lock_struct;

typedef struct
{
    kal_bool                       custom_apn_valid;
    char                           apn[MAX_GPRS_APN_LEN];
    char                           username[MAX_GPRS_USER_NAME_LEN];
    char                           password[MAX_GPRS_PASSWORD_LEN];
} nvram_GPRS_APN_struct;

typedef struct
{
    kal_uint8   url[TRACK_MAX_URL_SIZE];
    kal_uint8   conecttype;       // 1=domain ;0=IP
    kal_uint8   server_ip[4];     // 十进制
    kal_uint16  server_port;      // 十进制
    kal_uint8   soc_type;         // 0=TCP;1=UDP
    kal_uint8   server_type;      // 1=动态域名
    kal_uint8   lock;             // 锁定修改
    kal_uint8   ip_update;        // ip更新
} nvram_server_addr_struct;

typedef struct
{
    kal_uint8                      sw;                   // ON/OFF；
    kal_uint8                      at_least_pack;        // GPS定位数据打包发送至少需要满足的数量；2～25，默认值为：10
    kal_uint8                      batch_paket;          // 一般数据发送组包个数；1～25，默认值为：10
} nvram_group_package_struct;

typedef struct
{
    kal_uint8                      sw;                   // ON/OFF；
    kal_uint8                      last_position_valid;  // 是否自动注入上一次定位点
    kal_uint8                      server_mode;          // 0 与应用相同服务器，透传数据方式传输； 1 独立服务器，独立地址传输
    kal_uint32                     next_update_time;     // 服务器分配下次更新时间
} nvram_agps_struct;


typedef enum
{
    WORK_MODE_1 = 1,                //定时定位工作模式
    WORK_MODE_2,                    //智能定位模式->默认模式
    WORK_MODE_3,                    //深度休眠模式
    WORK_MODE_4,                    //Tracker工作模式
    WORK_MODE_MAX          // 4
} track_enum_work_mode;

typedef struct
{
    kal_uint8                   sw;                  // ON/OFF；
    track_enum_work_mode        mode;                // 终端工作模式  			
    //注  GT370 -- chengjun  2017-04-11
    //1=为需求里的模式1，T>120s
    //2=为需求里的模式1，T<120s 
    //3=为需求里的模式2，定时开机，超长待机
    kal_uint16          mode3_timer;                 // 间隔时间，秒为单位
    kal_uint16          datetime;                    //起始时间
    kal_uint16          mode1_timer;                 // 模式1的间隔时间
    kal_uint8           teardown_mode;               //防拆模式/开盖模式，总结就是进入异常了
    kal_uint8           teardown_time;               //防拆时间，异常模式持续时间
#if defined(__GT310__)
	kal_uint32			mode1_time_sec;				 // 模式1的间隔时间，秒
	kal_uint32			mode2_time_sec; 			 // 模式2的间隔时间，秒
#endif
} nvram_work_mode_struct;

typedef struct
{
    track_enum_work_mode        mode;                // 终端工作模式  			
    kal_uint16          t1; 
    kal_uint16          t2; 
}nvram_backup_pre_mode_struct;
/*备份前一模式的参数，必要时恢复,mode指令中的两个时间值       --    chengjun  2017-04-20*/

typedef struct
{
	kal_uint8 sw;
	kal_uint8 hour;
    	kal_uint8 min;
}nvram_dormancy_struct;

#if defined(__MQTT__)
typedef struct
{
    char product_key[20];
    char product_secret[48];
    char vin[48];
    char device_secret[48];
    char device_name[48];
} nvram_mqtt_login_struct;

typedef struct
{
    char uid[50];
    kal_uint8 isIOT;
} nvram_dasouche_login_struct;

typedef struct
{
    kal_uint8   IsTransit;       // 1=使用中转工具 ;0=不使用
    kal_uint8   server_ip[4];     // 十进制
    kal_uint16  server_port;      // 十进制
} nvram_transit_server_struct;
#endif/*mqtt*/


typedef struct
{
    kal_uint8                      sw;                   // ON/OFF；换卡报警开关
    kal_uint8                      simalm_type;           // M=0 仅GPRS;M=1 SMS+GPRS；默认上报方式为0
    #if !defined(__GW100__)
    kal_uint8                      sim_imsi[16];
    #else
    kal_uint8                      sim_imsi[17];
    kal_uint8                      sim_imsi2[17];
    #endif
} nvram_sim_alm_struct;

typedef struct
{
    kal_uint8                      sw;                   // ON/OFF；开机报警开关
    kal_uint8                      type;           // 0在线指令，1短信指令
    kal_uint8                      Number[TRACK_DEFINE_PHONE_NUMBER_LEN_MAX];
} nvram_smstc_struct;

typedef struct
{
    kal_uint8                      sw;                  // ON/OFF
    kal_uint16                     t1;                  // 上传间隔；10~3600秒，默认60秒；
    kal_uint16                     t2;                  // GPS不定位启动LBS传点间隔，10-3600秒，默认60秒；
} nvram_lbson_struct;

typedef struct
{
    char                           wifiname[20];
    char                           wifipasw[20];
}nvram_wifi_param_struct;

typedef struct
{
    kal_uint8                           mode_lastmode;
    kal_uint16                          mode_lasttime;
}nvram_mode_laststatus_struct;

typedef struct
{
    kal_uint16                           mode1_statictime;
    kal_uint16                           mode2_statictime;
}nvram_mode_statictime_struct;

typedef struct
{
    kal_uint8                      mode;                // 外挂单片机工作模式
    kal_uint16                     mode1_par1;          // 对于模式1下的参数
    kal_uint16                     mode1_par2;          // 对于模式1下的参数
    kal_uint16                     mode2_par1;          // 对于模式2下的参数起始时间(min)
    kal_uint16                     mode2_par2;          // 对于模式2下的参数唤醒间隔(min)
    U32                            mode3_par1;
    kal_uint16                     mode3_par2;          // 对于模式3下的参数
    U8                             mode4_par1;
    kal_uint16                     mode4_par2[12];          // 对于模式4下的参数
    kal_uint32                     mode1_time;          // 模式一持续工作时间
    kal_uint8                      premode;                // 记录防拆前的模式
    kal_bool                       mode1_en;//是否处于模式一
#if defined(__MCU_LONG_STANDBY_PROTOCOL__) && (defined(__GT740__)||defined(__GT420D__))
    kal_uint16                     mode2_start_year;    //↓
    kal_uint8                       mode2_start_month; //↓
    kal_uint8                       mode2_start_day;     //模式2 起始时间日期
    kal_uint8                       mode3_weekday[7];//Mode2_weekday[0]==1则星期日需唤醒
#endif//__MCU_LONG_STANDBY_PROTOCOL__
} nvram_ext_chip_struct;

typedef struct
{
    kal_uint8 gsm_volt_low_calibration_count;// 单片机传递小于2.85V电压次数
	kal_uint8 gsm_calibration_status;// 校准标志
	kal_uint8 bat_low_time;//低于2.75的次数
 	float comsultion_ele;// 消耗的电量
 	kal_uint8 awakentimes; //wifi唤醒的次数
	U8 first_calibration_ele;//电压校准的电量百分比
	kal_uint32 bat_lowtimep;// 低于2.75时间
	kal_uint32 bat_syntimep;// 高于2.75时间
	kal_uint32 gsm_calibration_time[5];// 校准电压的时间
}track_work_time;

typedef struct
{
	kal_bool gps_fix_sw;//gps定位开关
	kal_bool wf_lbs_fix_sw;//WF定位开关
	kal_uint8 fix_level;//定位优先级，默认1 GPS>WF
}track_fixmode_struct;

typedef struct
{
     kal_uint8 sw;
	 kal_uint16 mode1_startTime;
	 kal_uint16 mode1_endTime;
	 kal_uint16 mode2_sensorOutTime;//震动超时时间进入飞行
}track_fly_mode_struct;

typedef struct
{
	U8 sw;
	U8 alarm_type;    // 报警上报方式（0 仅 GPRS，1 SMS+GPRS, 2 SMS+GPRS+CALL,3 GPRS+CALL；默认值：1）
}nvram_drop_alarm;

typedef struct
{
    kal_uint8                    sw;  // ON/OFF；信息传输开关，默认开
    kal_uint16                    time;          //上传间隔，默认60S
} nvram_info_up_struct;

typedef struct
{
    char                                build_date[15];
    char                                build_time[15];
    char                                version_string[20];
    char                                version_string2[20];
    nvram_GPRS_APN_struct               apn;
    nvram_server_addr_struct            server;
    nvram_server_addr_struct            server2;
    nvram_server_addr_struct            mserver2[4];
    nvram_server_addr_struct            dserver;
#if defined(__BACKUP_SERVER__)
    nvram_server_addr_struct             bserver;
#endif    
#if defined(__XCWS__)
#if defined(__XCWS_TPATH__)
    nvram_server_addr_struct            server3;//车卫士客户定制双通道数据连接
#endif
    nvram_ef_device_setting_struct      login;
#endif
    app_version_struct                           app_version;

#if defined (__XYBB__)
	nvram_ef_bb_login_setting_struct 	login_data;	//登录信息
#endif /* __XYBB__ */

#if defined (__LUYING__)
    nvram_server_addr_struct            server4;    //录音上报服务端地址
#endif /* __LUYING__ */

#if defined(__MQTT__)
    nvram_mqtt_login_struct             mqtt_login;
    nvram_dasouche_login_struct            dasouche_login;
#endif


} nvram_importance_parameter_struct;

typedef struct
{
    kal_uint32                          flag;
    nvram_GPRS_APN_struct               apn;
    nvram_server_addr_struct            server;
    nvram_server_addr_struct            ams_server;
    kal_uint8                           gprson;                         // 1 开启GPRS   0 关闭GPRS
} nvram_alone_parameter_struct;

#if defined(__ACC_ALARM__)
typedef struct
{
    kal_bool                               accalm_sw;                            //ACC报警包开关
    kal_uint8                              accalm_type;                          //ACC报警类型
    kal_uint8                              accalm_time;                          //ACC报警包检测时间
    kal_uint8                              accalm_cause;                          //ACC报警包触发条件0，ACC变化触发；1，ACC变OFF触发；2，acc变ON触发 默认0
}nvram_accalm_struct;
#endif

#if defined (__CUST_BT__)
typedef struct
{
    kal_bool                               sw;                              //BT模块开关
    kal_uint8                              broadcast_time;                  //广播时间
    kal_uint8                              listen_time;                     //监听时间
    kal_uint16                             sleep_time;                      //监听时间
} nvram_bt_struct;
#endif /* __CUST_BT__ */

typedef struct
{
    kal_uint8                      sw;                   // ON/OFF 开盖报警
    kal_uint8                      alarm_type;           // M=0 仅GPRS;M=1 SMS+GPRS；默认上报方式为0
} nvram_open_shell_alarm_struct;

typedef struct
{
    kal_uint8                      sw;                  // ON/OFF
    kal_uint16                     t1;                  // 上传间隔；5~18000秒，默认10秒；
    kal_uint16                     t2;                  // 持续时间(单位:分钟)，0表示无限制，默认5分钟
} nvram_sv_struct;

#if defined (__CUST_ACC_BY_MCU__)
typedef struct
{
	kal_uint8					   count;				//acc结果的个数
	kal_uint8				   	   acc[64];					//缓存ACC结果64x2=128组
} nvram_gsm_mcu_acc_merge_struct;
#endif /* __CUST_ACC_BY_MCU__ */

typedef struct
{
    kal_uint8                      sw;                   // ON/OFF；接外电报警
    kal_uint8                      alarm_type;           // M=0 仅GPRS;M=1 SMS+GPRS;M=2 SMS+GPRS+CALL;M=3 CALL+GPRS默认上报方式为1//
} nvram_pwonalm_struct;

typedef struct
{
    char                                   sos_num[4/*TRACK_DEFINE_SOS_COUNT*/][TRACK_DEFINE_PHONE_NUMBER_LEN_MAX];
    char                                   centerNumber[TRACK_DEFINE_PHONE_NUMBER_LEN_MAX];
    char                                   url[100];                       // URL设置
    kal_uint8                              monitor_delay;                  // 监听延时，范围：5-18S。默认设置 10S
    kal_uint8                              lang;                           // 1 表示为中文，0 表示为英文，用于地址查询
    kal_uint8                              gprson;                         // 1 开启GPRS   0 关闭GPRS
    kal_uint32                             hbt_acc_on;                     // 心跳包设置间隔(分钟)  ACC ON 上传间隔
    kal_uint32                             hbt_acc_off;                    // 心跳包设置间隔(分钟)  ACC OFF 上传间隔
    kal_uint8                              callmode;                       // 来电响铃方式 0 铃声 1震动 2铃声+震动  3静音
    kal_uint8                              redial_count;                   // 电话重拨次数
    kal_uint8                              monitoring_delay;               // 监听延时；0、5～18秒；设置远程监听延时；默认值为：10秒
    kal_uint8                              call_get_addr_sw;               // 来电呼叫自动返回地址（1 有效， 0 禁用）
    kal_uint16                             sensor_gps_work;                // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    kal_uint8                              led_sleep;                      // LED休眠，1 (休眠有效，跟GPS开关绑定关联同开同关)，2 (常开，与GPS开关无关), 3 (常关)
    kal_uint16                             defense_time;                   // 延时进入设防时间（分钟）1～60分钟，默认值为：10分钟
    kal_uint8                              oil_sw;                         // 断油电功能（1 有效， 0 禁用）
    nvram_gps_work_struct                  gps_work;                       // GPS工作模式
    nvram_vibrates_alarm_struct            vibrates_alarm;                 // 震动报警
    nvram_power_fails_alarm_struct         power_fails_alarm;              // 断电报警设置
    nvram_speed_limit_alarm_struct         speed_limit_alarm;              // 超速报警设置
    nvram_Permission_settings_struct       Permission;                     // 权限设置
    nvram_sos_alarm_struct                 sos_alarm;                      // SOS报警设置
    nvram_blind_struct                     gps_blind;                      // GPS盲区设置
    nvram_staticrep_struct                 staticrep;                      // 静止上报最后位置开关指令
    nvram_displacement_alarm_struct        displacement_alarm;             // 位移报警
    nvram_agps_struct                      agps;                           // AGPS
    nvram_boot_struct                      boot;                           // 开机报警开关
    nvram_group_package_struct             group_package;                  // 组包个数
    nvram_mileage_statistics_struct        mileage_statistics;             // 里程统计
    nvram_carpark_alarm_struct             carpark_alarm;                  // 停车超时报警
    nvram_gps_time_zone_struct             zone;                           // 时区设置
    nvram_low_power_alarm_struct           low_power_alarm;                // 低电报警设置
    nvram_fatigue_driving_alarm_struct     fatigue_driving_alarm;          // 疲劳驾驶提醒
    nvram_electronic_fence_struct          fence[TRACK_DEFINE_FENCE_SUM];  // 电子围栏设置
    nvram_motor_lock_struct                motor_lock;                     // 电动车锁电机
    nvram_work_mode_struct                 work_mode;                      // 终端工作模式
    nvram_power_extbatalm_struct           power_extbatalm;                // 外部电源低电报警
    nvram_power_extbatcut_struct           power_extbatcut;                // 外部低电切断保护
    nvram_GPRS_Obstruction_struct          gprs_obstruction_alarm;         // GPRS 阻塞报警
    nvram_door_alarm_struct                door_alarm;                     // 门报警
    nvram_obd_parameter_struct             obd;                            // OBD
    nvram_ext_vbat_check_acc_struct        vbat_check_acc;                 // GT500电压检测acc功能
#if defined(__XCWS__)
    nvram_sleep_param_struct               sleep_wakeup;                    // 休眠参数
    kal_uint32                             acc_starting_time;               // acc off 后多久进入设防
    kal_uint8                              t_acclock;                       // acc 后是否进入设防控制
    kal_uint8                              t_calllock;                      // 电话呼入是车设防还是呼叫定位1-设撤防，0-定位
    kal_uint16                             acc_low_chargeout_time;          // acc off 后多久进行断电判断
    kal_uint8                              t_trace;                         // 是否实时上报定位数据
    kal_uint8                              t_calldsp;                       // 是否开通来显
    kal_uint8                              t_vibcall;                       // 震动呼叫功能
    kal_uint8                              t_sms;                           // 是否需要短信激活
    kal_uint8                              vib;                             // 非法位移报警发短信开关
    kal_uint8                              vibs;                            // 非法位移报警发平台开关
    kal_uint8                              pof;                             // 断电和低电发短信开关
    kal_uint8                              pofs;                            // 断电和低电发平台开关
    kal_uint8                              out;                             // 位移报警短信开关
    kal_uint8                              outs;                            // 位移报警网络开关
    kal_uint8                              acc_state;                       // 标记acc状态变化
    kal_uint32                             yd_gps_interval;                 // 移动gps数据发送频率
    kal_uint8                              pwralm_filter;                   // 断电报警静默时间
#endif
    kal_uint8                              accalm_sw;                       // 0 表示为ACC切换无短信通知，1 表示为ACC切换有短信通知
    kal_uint8                              speed_limit;                     // 电动车限速阀值
    nvram_boot_struct                      pwroff;                          // 关机报警开关
    nvram_sim_alm_struct                   sim_alarm;                       // 换卡报警
    kal_uint8                              pwrlimit_sw;                     // 限制关机功能,只用于有开关机键的项目
    kal_uint8                              lcd_on_time;                     // GW100LCD自动关屏时间
    kal_int32                              adc4_offset;                     // ADC4补偿值
    kal_uint16                             mode3_work_interval;             // 休眠模式下定时上报间隔分钟
    kal_uint8                              mode3_up_counts;                 // 休眠模式下单次工作上报次数
    nvram_ext_chip_struct                  extchip;                         // extchip
    nvram_wifi_param_struct                wifi_param;                      // ap 用户名+密码
    nvram_smstc_struct                     smstc;                           // 1 开启smstc   0 关闭smstc
#if defined(__XCWS_TPATH__)
    kal_uint8                              server3sw;                       // 双线开关
#endif
    kal_uint8                              defense_mode;                    // 自动设防模式 X=0  自动设防模式；X=1 手动设防模式，2 为遥控报警，默认X=1
    nvram_info_up_struct                   info_up;                         // (几米国际)外接电源电压信息上传开关
    nvram_crash_struct						crash_str;				//碰撞报警
	nvram_incline_struct						incline_str;			//倾斜报警
    nvram_wifi_struct                      wifi;                           //wifi设置
    nvram_lbson_struct                     lbson;                           // LBS传点设置
    nvram_FCFALM_struct                    fcfalm;                          // 伪基站报警
    kal_uint8                              flycut;                          // 飞行模式开关
    kal_uint8                              paramFileCrc[2];                 // 在线下载配置参数文件的MD5
    kal_uint8                              dwd_epo_sw;                      // EPO下载开关
    kal_uint8                              PW5;                             // 5V电源输出状态
#if defined(__ACC_ALARM__)
    nvram_accalm_struct                    accalm;                          // 06F定制版，ACC报警
#endif    
#if defined(__GT740__) ||defined(__GT420D__)
    kal_uint8    cal_time_sw;    //电量次数计算开关，默认开
    kal_uint8    new_position_sw;   //启用新定位包上传开关 默认关
    nvram_gpsup_struct             gpsup_struct;    //静态补传
#endif
    kal_uint8 fake_cell_enable;     //防伪基站使能
    kal_uint8 fake_cell_to_ams;     //伪基站信息是否上报AMS
    kal_uint8 gpsDup;               //gps位置数据上报开关
#if defined (__CUST_BT__) && defined(__NT33__)
    nvram_bt_struct bt;
#endif /* __CUST_BT__ */
    kal_uint8    gpswake;  //唤醒GPS 默认0
#if defined (__GT370__)|| defined (__GT380__)
    nvram_open_shell_alarm_struct open_shell_alarm;
#endif /* __GT370__ */
    kal_uint8 fake_cell_auto_temp;  //临时自动开关防伪功能 0=临时关闭，按需恢复，0xFF=恢复后不处理
#if defined(__SPANISH__)
    kal_uint8 itp_sw;                //94包传输开关
#endif
track_fixmode_struct fixmode;//定位优先级
#if defined (__GT310__)
    track_fly_mode_struct fly_stu;//飞行机制时间段
#endif
    nvram_sv_struct                         sv;                             //卫星信息上传设置
    kal_uint8                               sw_nbver;                       //无电池版本设置指令
#if defined(__DROP_ALM__)
    nvram_drop_alarm drop_alm;  //跌倒报警默认开
#endif
    kal_uint8  jc_mode;//0 双向通话； 1 监听  默认值：0
    kal_uint8  answer;// 按键接听 0，自动接听 1

    U8 lbs_sw;//lbs 上传开关    
    kal_uint8    NO_calibration;    //电量不校准 默认关 0
    #if defined(__GT420D__)
    kal_uint8    charge_status;
    kal_uint8    lowbat_status;
    nvram_sensortime_struct sensortime;
    nvram_mode_laststatus_struct lastmode;
    nvram_mode_statictime_struct statictime;
    nvram_teardown_struct                  teardown_str;                    // 防拆报警,默认开启,报警方式1,SMS+GPRS
    kal_uint8 humiture_sw;
    kal_uint8            percent;                         //低电量警告百分比
    kal_uint8   anglerep_speed;
    #endif
    #if defined(__DASOUCHE__)
    nvram_ischange_pk_struct changepk;
    #endif
    #if defined(__MQTT__)
    nvram_transit_server_struct transit;
    kal_uint16 drop_paket;
    float temperature_threshold;
    float humidity_threshold;
    #endif
    kal_uint8 sendpaketled_sw;
} nvram_parameter_struct;

typedef struct
{
    char                                   fn_num[TRACK_DEFINE_FN_COUNT][TRACK_DEFINE_PHONE_NUMBER_LEN_MAX];
    char                                   white_num[TRACK_DEFINE_WN_COUNT][TRACK_DEFINE_PHONE_NUMBER_LEN_MAX];
    kal_uint8                              almrep[TRACK_DEFINE_SOS_COUNT];
} nvram_phone_number_struct;

typedef struct
{
  // nvram_importance_parameter_struct  importance_parameter;
    // char                                   sos_num[4/*TRACK_DEFINE_SOS_COUNT*/][TRACK_DEFINE_PHONE_NUMBER_LEN_MAX];
   // nvram_ext_vbat_check_acc_struct        vbat_check_acc;                 // GT500电压检测acc功能
 //   kal_int32                              adc4_offset;        // ADC4补偿值
  //  kal_uint16                             sensor_gps_work;                // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
  //   kal_uint8       defense_mode;                               // 自动设防模式 X=0  自动设防模式；X=1 手动设防模式，2 为遥控报警，默认X=1
  //  nvram_phone_number_struct   PHONE_NUMBER;
 kal_uint8 data[2000];
}nvram_drv_importance_backup_struct;


typedef struct
{
    kal_uint8 fake_cell_flg;    
    //0=正常,无报警
    //1=检测到伪基站信息，需要继续判断GPRS
    //2=在搜到伪基站的有效时间内GPRS断网10分以上，可以报警了 ，报警后再恢复0 
    //chengjun  2017-02-15

    //搜到伪基站的时间、地点缓存到AMS,最多存4个
    kal_uint8 fake_cell_count;
    kal_uint32 alm_sec[4];
    float alm_longitude[4];
    float alm_latitude[4];
	
    //最后一次搜到伪基站的地点
    float last_longitude;
    float last_latitude;

}fake_cell_info_notes_struct;


typedef struct
{
    kal_uint8       in_or_out[TRACK_DEFINE_FENCE_SUM];          // 电子围栏 当前是出于围栏内(1)还是围栏外(2)
    kal_uint8       lowBattery;                                 // 低电报警发送标记
    kal_uint8      lowBatteryFlag;                           //低电报包只发送一次标记
    kal_uint8       lowExtBattery;                              // 外部电源低电报警发送标记
    kal_uint8       disExtBattery;                              // 外部电源断开发送标记
    kal_uint8       flag;                                       // 位移报警 标记
    float           longitude;                                  // 位移报警
    float           latitude;                                   // 位移报警
    kal_uint32      netLogControl;                              // 在线网络上报日志控制
    kal_uint8       reStartSystemCount;                         // 20分钟重启连续计数
    kal_uint8       defences_status;                            // 设防状态  1 撤防，2 预设防，3 设防
    kal_uint8       key_status;                                 // GT02D按键状态  0 关闭LED显示    1 开启LED显示
    kal_uint8       oil_status;                                 // 油电状态（0 油电正常；1 油电断开；2 请求断开，GPS未定位；3 请求断开，未发因超速延缓执行的短信；4 请求断开，已发因超速延缓执行的短信）
    kal_uint8       oil_cmd_source;                             // 断油电指令来源
    kal_uint8       acc_last_status;                            // 用于记录ACC状态，防止开机后的状态与上次关机前一致
    kal_uint8       poweroff;                                   // 低电关机报警发送标记
    kal_uint8       power_saving_mode;                          // 省电模式(0:正常模式;1:省电模式)
    kal_uint8       reconnect_send_flg;                         // 是否发送阻塞报警短信(0不发送，1发送)
    kal_uint8       restart_system_limited_service;             // L4C_RAC_LIMITED_SERVICE 重启计数
    kal_uint8       blind_status;                               // GPS盲区状态，0为进入盲区，1为盲区外
    kal_uint8       defense_mode;                               // 设防模式： 0 自动设防模式；   1 手动设防模式；   2 自动设防手动撤防；默认1
    kal_uint8       location_switch;                            // 定位数据上传开关  0 关闭  1 打开
    kal_uint8       timeslot[3][8][5];
    kal_uint8       disExtBattery2;                             // 外部电压6v之下报警后设置为1, 6v之上为0
    kal_uint8       sim_alm;                                    // 换卡报警标志位
    kal_uint16      flashtime;
    kal_uint8       audio_count;                                // 音频文件数 默认0
    kal_uint8       record_time;                                // 时长，默认10
    audio_file_struct audio_file[5];                            // 音频文件
    kal_uint16      work3_countdowm;                            // 休眠模式开启工作倒计时
    kal_uint8       server_ip[4];                               // 域名解析得到的IP地址备份
    kal_uint8       oil_lock_status;                            //断油电锁定状态,0:不锁定;1:锁定 V20 433模块适用
    kal_uint8       switch_down;//防拆开关,1按下,默认0
    kal_uint8       mech_sensor_count;
    kal_uint16       mech_sensor_invel_time;
    kal_uint8        tracking_mode;//追踪模式
    kal_uint32        tracking_time;//追踪模式时间
    kal_uint8        tracking_status_mode;//原有状态模式
    kal_uint32        tracking_status_time;//原有状态时间
    kal_uint8        glp;
    kal_uint8        up_param;
    kal_uint8       ams_log;                          //710保存LOG开关
    kal_uint8       ams_push;                         //710上传LOG开关
    kal_uint8       TPRO_COUNT;                       //定位包协议次数
    kal_uint8       nw_reset_sign;                    // 联网重启标记
#if defined (__BAT_TEMP_TO_AMS__)
    kal_int8        temperature_data[70][5];
    kal_uint8       temp_count;
#endif /* __BAT_TEMP_TO_AMS__ */
    
    kal_uint8       power_fails_flg;

    fake_cell_info_notes_struct fake_cell_info_notes;   //防伪基站消息记录 
    U16  MainCellId;
#if defined (__GT740__) || defined(__GT420D__)
    kal_uint8       nw_sign;                    // 重启标志
    kal_uint8   remove_time;//追踪模式已工作时长
    track_work_time         cal_wk_time_struct;
    kal_uint32    trackmode_end_sec;
    kal_uint8  tracking_count;//防拆执行次数
#endif
  kal_int16   gps_mode;

#if defined (__LUYING__)
    kal_uint8         record_file;//存在文件的录音的开始个数
    kal_uint8         record_up;//上传到平台的录音的开始个数
    kal_uint8         record_cnt;//录音的开始总个数
    kal_uint8         record_stamp[6];
    kal_uint8         record_last_time;
#endif /* __LUYING__ */
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
#if defined(__GT740__)||defined(__GT420D__)
    kal_uint8         next_boot_is_AMS;//标记下一次开机是否为AMS唤醒
    kal_uint8         next_boot_is_supplements;//标记下一次开机是否为补传唤醒
    kal_uint8         supplements_retry_count;//补传唤醒重试次数
#endif//__GT740__
    kal_uint8         assist_sw;//辅助测试开关
    U16                gsm_boot_times;//GSM开机次数
    U16                gsm_boot_times_offset;//GSM首次开机时MCU开启了GSM多少次
#endif//__MCU_LONG_STANDBY_PROTOCOL__
#if defined(__MCU_LONG_STANDBY_PROTOCOL__) || defined(__JM66__)
    kal_uint8         reset_count;//因单片机通信失败而重启的计数,最多5次
#endif//__MCU_LONG_STANDBY_PROTOCOL__
#if defined(__JM66__)
    U16                cell_id[LBS_LIST_MAX];
    U8                  valid_count;
    U8                  is_first_boot;
#endif//__JM66__
#if defined(__ATGM_UPDATE__)        
	U8 atgm_update_failtimes;//升级失败次数,必须一开始升级就+1,不然中途未知重启会记录不到这次失败,成功后会清0        
	U8 atgm_version[15];//中科微GPS版本,此GPS一开启就会输出版本信息.
#endif /*__ATGM_UPDATE__*/
	U32  zkw_eph_datatime;//eph数据存储时间
	#if defined(__GT420D__)
        kal_uint8         record_tracking_time;//记录追踪次数
        kal_uint8         lowpower_flag;
        kal_uint8         gps_420d_flag;
        kal_uint8         car_status;
        kal_uint8         last_percent;
        kal_uint8         last_location_status;
       #endif
} nvram_realtime_struct;


/***************************************************************************** 
* Local variable            局部变量
*****************************************************************************/


/****************************************************************************
* Global Variable           全局变量                                                       
*****************************************************************************/


/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/
extern nvram_parameter_struct G_parameter;
//extern nvram_realtime_struct G_realtime_data;
extern nvram_importance_parameter_struct G_importance_parameter_data;
extern nvram_phone_number_struct G_phone_number;
extern nvram_parameter_struct* OTA_NVRAM_EF_PARAMETER_DEFAULT ;
extern nvram_importance_parameter_struct* OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT  ;
extern  nvram_phone_number_struct* OTA_NVRAM_EF_PHONE_NUMBER_DEFAULT;
extern   nvram_ef_logd_struct* OTA_NVRAM_EF_LOGD_DEFAULT;
extern   nvram_realtime_struct* OTA_NVRAM_EF_REALTIME_DEFAULT ;
extern nvram_alone_parameter_struct* OTA_NVRAM_ALONE_PARAMETER_DEFAULT;
    /*****************************************************************************
 *  Local Functions	            本地函数
*****************************************************************************/
static void track_system_bootup_delay(void);

/*****************************************************************************
 *  Global Functions            全局函数
*****************************************************************************/

#endif /* __TRACK_DISK_NVRAM__ */


#endif  /* _TRACK_PARAMETE_H */


