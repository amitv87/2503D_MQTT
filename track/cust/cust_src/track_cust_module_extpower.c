/******************************************************************************
 * track_cust_module_extpower.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        GSM与MCU通信
 *        文件结构特殊，是因为每个项目的协议不同，又存在一定的共性，结构如下

        单片机升级共用部分
        GT740协议处理部分
        GT300S协议处理部分
        GT370A协议处理部分

        MT200协议处理部分
		JM81协议处理部分
 *
 *
 * modification history
 * --------------------
 * v1.0   2017-03-22,  chengjun create this file
 *
 ******************************************************************************/
//单片机升级共用部分

/****************************************************************************
*  Include Files                        包含头文件
*****************************************************************************/
#if defined(__GT740__) || defined(__GT370__)|| defined (__GT380__) || defined(__GT300S__) || defined(__MT200__) || defined(__JM81__)|| defined(__JM66__) || defined(__GT420D__)
#include "track_cust.h"
#include "track_drv_uart.h"
#include "track_drv_pmu.h"
#include "c_vector.h"

/*****************************************************************************
*  Define                               宏定义
******************************************************************************/
#define PARAMETER_MAX   16
#define	COLLISION_LEVEL_MAX 1024
#define MCU_REG_MAX 127
static cmd_return_struct g_return_sms = {0};
static kal_uint8 g_return_sms_valid = 0;
#if defined(__GT420D__)
static kal_uint16 tracking_status_mode = 0;
static kal_uint16 tracking_status_needsend = 2;
static kal_uint16 downalm_status_mode = 0;
static kal_uint16 downalm_status_needsend = 2;
static kal_uint16 simalm = 0;
static kal_uint16 batalm = 0;
static kal_uint8 sensor_type = 0;
static kal_uint8 forbid_change_parameter = 0;
static kal_uint8 keyin_status = 0;
static kal_uint8 charge_status = 0;
static kal_uint8 light_status = 0;
static kal_uint8 Motion_state_status = 0;
static kal_uint8 chargein_status = 0;
static kal_uint8 sportorstatic_flag = 0;
static kal_uint8 poweroff_needsend_location= 1;
static kal_uint8 refer_flag = 0;
static kal_uint8 change_sethumi_flag = 0;
#endif
/*****************************************************************************
*  Typedef  Enum                        枚举定义
*****************************************************************************/
typedef void (*Func_CustRecvPtr)(void);

typedef struct
{
    const char           *cmd_string;
    Func_CustRecvPtr     func_ptr;
} cust_recv_struct;

/*****************************************************************************
*  Struct                               数据结构定义
******************************************************************************/
static char *blank = {""}, *s_ON = {"ON"}, *s_OFF = {"OFF"}, *s_Error = {"ERROR"};
static U8 lbsalm=0,gpsalm=0,gps_nosat=0,gps_nofix=0,gps_fixed=0;

#define ON_OFF(value) (value==0?s_OFF:(value==1?s_ON:s_Error))

extern void teardown_alarm_overtime(void);
void track_cust_module_power_data(kal_uint8 *data, int len);
void track_cust_send17(void* pt1, void* pt2);
void track_cust_change_to_static(void);
void track_cust_module_update_mode(void);
kal_uint32 track_cust_refer_charge_data(kal_uint32 arg);
kal_uint8 track_cust_need_check_sportorstatic(kal_uint8 arg);
kal_uint8 track_cust_refer_sensor_data(kal_uint8 arg);
kal_uint8 track_cust_refer_motion_status_data(kal_uint8 arg);
kal_uint8 track_cust_refer_chargein_data(kal_uint8 arg);
kal_uint8 track_cust_refer_keyin_data(kal_uint8 arg);
kal_uint8 track_cust_needsend_charge_data(kal_uint8 arg);
kal_uint32 track_cust_refer_lightsensor_data(kal_uint32 arg);

kal_uint8 track_cust_check_humiflag_flag(kal_uint8 arg)
{
    if (arg == 1||arg == 0)
    {
        refer_flag = arg;
        LOGD(L_APP,L_V5,"refer_flag:%d",refer_flag);
    }
    else
    {
        LOGD(L_APP,L_V5,"refer_flag:%d",refer_flag);
        return refer_flag;
    }
}

kal_uint8 track_cust_check_sethumiflag_flag(kal_uint8 arg)
{
    if (arg == 1||arg == 0)
    {
        change_sethumi_flag = arg;
        LOGD(L_APP,L_V5,"change_sethumi_flag:%d",change_sethumi_flag);
    }
    else
    {
        LOGD(L_APP,L_V5,"change_sethumi_flag:%d",change_sethumi_flag);
        return change_sethumi_flag;
    }
}

kal_uint16 track_cust_check_beforepoweroff_flag(kal_uint8 arg)
{
    if (arg == 1||arg == 0)
    {
        poweroff_needsend_location = arg;
        LOGD(L_APP,L_V5,"poweroff_needsend_location:%d",poweroff_needsend_location);
    }
    else
    {
        LOGD(L_APP,L_V5,"poweroff_needsend_location:%d",poweroff_needsend_location);
        return poweroff_needsend_location;
    }
}

kal_uint16 track_cust_check_simalm_flag(kal_uint8 arg)
{
    if (arg == 1||arg == 0)
    {
        simalm = arg;
        LOGD(L_APP,L_V5,"simalm:%d",simalm);
    }
    else
    {
        LOGD(L_APP,L_V5,"simalm:%d",simalm);
        return simalm;
    }
}

kal_uint16 track_cust_check_batteryalm_flag(kal_uint8 arg)
{
    if (arg == 1||arg == 0)
    {
        batalm = arg;
        LOGD(L_APP,L_V5,"batalm:%d",batalm);
    }
    else
    {
        LOGD(L_APP,L_V5,"batalm:%d",batalm);
        return batalm;
    }
}

kal_uint16 track_cust_check_teardown_flag(kal_uint8 arg)
{
    if (arg == 1||arg == 0)
    {
        tracking_status_mode = arg;
        LOGD(L_APP,L_V5,"tracking_status_mode:%d",tracking_status_mode);
    }
    else if(arg == 2||arg == 3)
    {
        tracking_status_needsend = arg;
        LOGD(L_APP,L_V5,"tracking_status_needsend:%d",tracking_status_needsend);
        }
    else if(arg == 98)
    {
        LOGD(L_APP,L_V5,"tracking_status_needsend:%d",tracking_status_needsend);
        return tracking_status_needsend;
    }
    else if(arg == 99)
    {
        LOGD(L_APP,L_V5,"tracking_status_mode:%d",tracking_status_mode);
        return tracking_status_mode;
    }
}

kal_uint16 track_cust_check_downalm_flag(kal_uint8 arg)
{
    if (arg == 1||arg == 0)
    {
        downalm_status_mode = arg;
        LOGD(L_APP,L_V5,"downalm_status_mode:%d",downalm_status_mode);
    }
    else if(arg == 2||arg == 3)
    {
        downalm_status_needsend = arg;
        LOGD(L_APP,L_V5,"downalm_status_needsend:%d",downalm_status_needsend);
        }
    else if(arg == 98)
    {
        LOGD(L_APP,L_V5,"downalm_status_needsend:%d",downalm_status_needsend);
        return downalm_status_needsend;
    }
    else if(arg == 99)
    {
        LOGD(L_APP,L_V5,"downalm_status_mode:%d",downalm_status_mode);
        return downalm_status_mode;
    }
}

/******************************************************************************
 * track_cust_module_extpower.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        GSM模块与单片机模块通信
 *
 * modification history
 * --------------------
 * v1.0   2016-10-14,  chengjun create this file
 *
 ******************************************************************************/
#if defined (__MCU_SW_UPDATE__)
kal_uint8 mcu_sw_version_string[32] = {0};
kal_uint8* track_get_mcu_sw_version(void)
{
    return mcu_sw_version_string;
}

void track_cust_module_uart_update_data(kal_uint8 *data, int len)
{
    if ((data == NULL) || (len <= 0))
    {
        return;
    }

    LOGD(L_DRV, L_V1, "串口2数据 len=%d", len);
    LOGH(L_DRV, L_V5, data, len);

    if ((len == 1) && (*data == 0 || *data == 0xFF))
    {
        return;
    }

    if (strstr(data, "AT^GT_CM=") != NULL)
    {
        track_cust_module_power_data(data, len);
    }
    else if (track_is_in_update_bootload_cmd())
    {
        track_drv_urat_mcu_updata_ymode_decode(data, len);
    }
    else if (strstr(data, "BB") != NULL)
    {
        track_drv_urat_mcu_updata_ymode_decode(data, len);
    }
    else
    {
        track_cust_module_power_data(data, len);
    }
}

void track_cust_ota_cmd(void)
{
    char buf[100] = {0};
    sprintf(buf, "AT^GT_CM=07,15,240#\r\n");
    track_drv_write_exmode(buf, strlen(buf));
    LOGD(L_OBD, L_V5, "");
}

#endif

void track_cust_modeule_into_sleep_mode(void)
{
    track_drv_sleep_enable(SLEEP_OBD_MOD, KAL_TRUE);
}
void track_drv_mcu_and_gsm_sleep(void * op)
{
    kal_uint32 level = (kal_uint32)op;

    LOGD(L_APP, L_V5, "MCU唤醒GSM两秒后休眠 %d", level);
    if (track_is_timer_run(TRACK_DRV_MCU_WAKEUP_GSM_TIMER_ID))
    {
        tr_stop_timer(TRACK_DRV_MCU_WAKEUP_GSM_TIMER_ID);
    }

    if (1 == level)
    {
        track_drv_sleep_enable(SLEEP_EXT_UART_MOD, KAL_FALSE);
        track_start_timer(TRACK_DRV_MCU_WAKEUP_GSM_TIMER_ID, 5 * 1000, track_drv_mcu_and_gsm_sleep, (void*)0xff);
    }
    else if (0 != level)
    {
        track_drv_sleep_enable(SLEEP_EXT_UART_MOD, KAL_TRUE);
    }
}


#endif

#if defined(__GT420D__)
static applib_time_struct Mcu_Time;                            //MCU时间
#endif
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)

kal_uint8 track_cust_get_weekday_from_date(applib_time_struct* time);
void track_cust_mlsp_test(void);
void track_cust_mlsp_boot(void);
void track_cust_mlsp_set_time(applib_time_struct* set_time);
void track_cust_mlsp_set_nextboot_time(applib_time_struct* time);
void track_cust_mlsp_shutdown_req(void);
void track_cust_mlsp_write_backup_req(kal_uint8* write_data,int len);
void track_cust_mlsp_read_backup_req(void);
void track_cust_mlsp_get_peripherals_status(kal_uint8 arg);
void track_cust_mlsp_set_device_enable(kal_uint8 device_type, kal_bool enable);
void track_cust_send17(void* pt1, void* pt2);


#define MSLP_MAX_LEN                        120//整包最大长度
#define END_LEN                                   3//crc+0d+0a
#define MSLP_MCU_VERSION_LEN         24//MCU版本字符长度
#define BACKUP_LEN                             16//可备份数据的长度

#define HEADER1 0x43//包头1
#define HEADER2 0x4A//包头2

static applib_time_struct Next_Boot_Time = {0};        //下次开机时间
static kal_uint8 Proticol_Version=0xFF;                        //协议版本
static applib_time_struct Mcu_Time;                            //MCU时间
static applib_time_struct Set_Mcu_Time;             //设置MCU的时间(-2000)
static kal_uint8 Mcu_Version[MSLP_MCU_VERSION_LEN]={0};//MCU版本
static kal_uint16 Battery_Volt=0xFFFF;                       //电池电压
static kal_uint16 Ext_Volt=0xFFFF;                             //外源电压
static kal_uint8 Charge_Status = 0xFF;                       //充电状态0:无外电1:充电中2:有外电但未充电
static kal_uint8 Boot_Reason=0xFF;                           //开机原因
static kal_uint8 Back_UP_Data_Write[BACKUP_LEN];   //要写入的备份数据
static kal_uint8 Back_UP_Data_Read[BACKUP_LEN];   //读出的备份数据
static kal_uint8 Mcu_Sensor_Enable = 0xFF;              //实际传感器使能
static kal_uint8 Mcu_Sensor_Type = 0xFF;                  //传感器类型
static bool Back_UP_Data_Vaild = FALSE;                    //当前Back_UP_Data_Read是否有效
static bool Req_Shutdown = FALSE;                             //请求关机标志
static bool Send_Position_Fail = FALSE;                       //传点失败,N分钟后起来
static U16 Mcu_Boot_GSM_Times = 0xFFFF;                       //单片机开启GSM的次数
typedef enum
{
    UODATE_NO_NEED,
    UPDATE_STATUE_ENABLE_REQ,   //发 AT^GT_CM=99#
    UPDATE_STATUE_ENABLE_RSP,   //收到 AT^GT_CM=99,OK#
    UPDATE_BOOTLOADER_MODE_REQ,     //发 UPGRADE
    UPDATE_BOOTLOADER_MODE_RSP,     //收到 CCCCC
    UPDATE_YMODE_DOWNLOAD,          //Ymode下载中
    UPDATE_YMODE_EOT_END,
    UPDATE_NEW_SW_FINISH,           //收工
} mcu_sw_update_status_enum;
typedef enum
{
    MLSP_BOOT_SYN = 0x01,		//开机参数同步
    MLSP_SET_TIME = 0x02,		//设置MCU时间
    MLSP_SET_BOOT_TIME = 0x03,	//设置开机时间
    MLSP_SHUTDOWN_REQ = 0x04, 	//请求关机
    MLSP_BACKUP_REQ = 0x05,		//请求备份数据
    MLSP_SET_GSENSOR = 0x06,	//设置加速度传感器
    MLSP_GET_HW_STATUS = 0x10,	//读取硬件状态
    MLSP_GET_PAR = 0x11,		//读取MCU内参数
    MLSP_MCU_TEST = 0x80,       //单片机进入测试模式
    MLSP_MCU_EVENT = 0x81,		//MCU事件通知
    MLSP_GSENSOR_EVENT = 0x82,	//加速度传感器事件通知
    MLSP_DEVICE_ALARM = 0x83,	//设备报警事件通知
    MLSP_DEVICE_ENABLE = 0x91,	//设备工作使能
    MLSP_MCU_SW_UPDATE = 0xF0,	//单片机软件升级
} mcu_long_standby_protocol;
typedef enum
{
    MLSP_NODEFINE_BOOT = 0x00,//未定义
    MLSP_POWER_AUTO_BOOT = 0x01,//上电自动开机
    MLSP_KEY_BOOT = 0x02,//按键触发开机
    MLSP_TIMING_BOOT = 0x03,//定时唤醒开机
    MLSP_CHARGE_BOOT = 0x04,//充电开机

    MLSP_VIBRATION_BOOT = 0x10,//震动报警开机
    MLSP_LOW_BATTERY_BOOT = 0x11,//电池低电开机
    MLSP_LOW_VEXT_BOOT = 0x12,//外电低电开机
    MLSP_FAIL_VEXT_BOOT = 0x13,//外电断电开机
} MLSP_BOOT_REASON;
typedef enum
{
    MLSP_DEVICE_POWER_KEY = 0x01,//电源键
    MLSP_DEVICE_TEARDOWN_KEY = 0x02,//防拆键
    MLSP_DEVICE_PHOTOSENSITIVE = 0x08,//光敏器件
    MLSP_DEVICE_CHARGER = 0x10,//充电器
    MLSP_DEVICE_GSENSOR = 0x20,//加速度传感器
} MLSP_DEVICE_TYPE;
#endif//__MCU_LONG_STANDBY_PROTOCOL__
#if defined(__JM66__)

U8 Jm66_Current_mode = 0;//0 SID/1 GPS/2 KEY/>10 TRACKING
U8 is_quit_transport_boot = 0;
U8 is_update_mode = 0;


static U8* update_image_p = NULL;
static U16 update_image_len = 0;
static U16 update_image_offset = 0;
static U16 this_time_send_len = 0;
static U16 index_send = 0;
static kal_bool is_first_package = TRUE;
static kal_uint8 tbt100_bt_mac[6] = {0};//蓝牙MAC地址
static kal_uint8 tbt100_bt_baseversion[20] = {0};//蓝牙基础版本
static kal_uint8 tbt100_bt_appversion[20] = {0};//蓝牙应用版本
//static kal_uint8 current_env_data[6] = {0};//当前环境数据
//static kal_uint8 oneday_env_air_data[48] = {0};//一天的气压数据
//static kal_uint8 oneday_env_temp_data[48] = {0};//一天的温度数据
//static kal_uint8 oneday_env_humidity_data[48] = {0};//一天的湿度数据
//static kal_uint16 current_env_data16[3] = {0};//当前环境数据
static applib_time_struct tbt100_bt_time = {0};//读取到的蓝牙时间
static kal_uint16 tbt100_bt_reset_times = 0xFFFF;//蓝牙复位次数
static kal_uint16 tbt100_bt_vbat = 0xFFFF;//蓝牙电池电压×1000
static kal_uint8 tbt100_get_AES_key[22] = {0};//获取到的AESKEY
static kal_bool will_shutdown = FALSE;
LBS_Multi_Cell_Data_Struct simulation_lbs = {0};
//协议号枚举
typedef enum
{
    TBT100_SET_TIMEPAR = 0x20,		              //蓝牙时间参数设置(扫描,广播)
    TBT100_GET_TIMEPAR = 0x21,		              //蓝牙时间参数读取(扫描,广播)
    TBT100_SET_SENSORPAR = 0x22,	                     //G-Sensor参数配置
    TBT100_GET_SENSORPAR = 0x23, 	              //G-Sensor参数读取
    TBT100_SET_ENVPAR = 0x24,		                     //环境参数配置
    TBT100_GET_ENVPAR = 0x25,	                     //环境参数读取
    TBT100_SET_IMEI = 0x26,	                     //向蓝牙写IMEI
    TBT100_GET_IMEI = 0x27,	                     //向蓝牙读IMEI
    TBT100_GET_SENSOR_ACTIVE_TIMES = 0x50,   //读取GSENSOR激活次数
    TBT100_GET_ENVDATA = 0x51,		              //读取环境数据
    TBT100_EVENT_COLLAR_BROKEN = 0x52,          //项圈剪断报警
    TBT100_EVENT_TEARDOWN = 0x53,	             //外壳拆开报警
    TBT100_EVENT_OUTLIER = 0x54,	                     //牦牛离群报警
    TBT100_SET_NEXTBOOT = 0x55,	                     //设置下次开机时间(N分钟后开机)
    TBT100_EVENT_KEYDOWN = 0x56,	              //按键被按下
    TBT100_EVENT_TIMING = 0x57,	                     //定时开启GSM的事件
    TBT100_EVENT_CATTLE = 0x58,	                     //周边蓝牙信息(牛群)
    TBT100_EVENT_CATTLE_END = 0x59,	              //周边蓝牙信息(牛群)结束
    TBT100_REQ_SCAN = 0x60,	                            //立即扫描
    TBT100_EVENT_FIRST_BOOT = 0x61,	              //首次开机事件
    TBT100_GET_BT_BASEVERSION = 0xD0,            //获取蓝牙基础版本号
    TBT100_UPDATE_START = 0xD1,	                    //开始升级
    TBT100_UPDATE_IMAGE = 0xD2,	                    //升级镜像文件
    TBT100_UPDATE_CHECKFILE = 0xD3,	             //升级文件校验
    TBT100_UPDATE_END = 0xD4,	                    //升级结束
    TBT100_QUIT_UPDATE = 0xD5,	                    //结束升级模式
    TBT100_TRANSPORT = 0xE0,	                           //运输模式
    TBT100_SYNC_TIME = 0xE1,	                           //时间同步
    TBT100_SHUTDOWN_REQ = 0xE2,	                    //请求关机
    TBT100_TELL_GSM_SHUTDOWN = 0xE3,	      //蓝牙关闭GSM事件
    TBT100_TELL_BT_VBAT = 0xE4,	                    //GSM通知蓝牙电池电压
    TBT100_GET_BT_RESET_TIMES = 0xF0,	      //查询蓝牙硬件复位次数
    TBT100_GET_BT_MAC = 0xF1,	                    //查询蓝牙MAC地址
    TBT100_GET_BTVBAT = 0xF2,	                           //查询蓝牙电池电压
    TBT100_EVENT_CONNECT = 0xF3,	                    //蓝牙连接与断开事件
    TBT100_GET_KEY = 0xF4,	                           //获取蓝牙通信密钥
    TBT100_SET_KEY = 0xF5,	                           //设置蓝牙通信密钥
    TBT100_BTTEST = 0xFC,	                                  //蓝牙测试通道
    TBT100_BT_RESET = 0xFD,	                           //蓝牙系统复位
    TBT100_CLEAR_BTLOG = 0xFE,	                   //清除蓝牙日志
    TBT100_GET_BT_APPVERSION = 0xFF,	            //查询软件版本信息
} tbt100_protocol;

//蓝牙有关时间参数类型枚举
typedef enum
{
    TBT100_BT_SCAN_INTERVAL = 0x00,		       //蓝牙扫描间隔参数min
    TBT100_BT_SCAN_LAST = 0x21,		              //蓝牙扫描持续时长参数 sec
    TBT100_BT_BROADCAST_INTERVAL = 0x22,	       //蓝牙广播间隔sec
    TBT100_BT_BROADCAST_LAST = 0x23, 	       //蓝牙广播持续时长参数 ms
    TBT100_BT_ALLTIMEPAR = 0x24,		              //蓝牙时间所有参数
} TBT100_GET_TIMEPAR_TYPE;
#endif//__JM66__
static U8 par1 = 0;
static U8 par2 = 0;
static kal_uint32 adc_data = 0;
/******************************************************************************
 * 与单片机通信使用队列请加宏↓
 ******************************************************************************/
#if defined(__MCU_LONG_STANDBY_PROTOCOL__) || defined(__JM66__)
//宏定义
#define REPEAT_TIME  3000//重发间隔
//引用外部
extern c_vector* vec_send_mcu_queue;
//本地函数声明
void confirm(char *str);
static void track_mlsp_send(void* parr);


void track_mlsp_send_new()
{
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
    if (track_drv_get_mcu_sw_update_status() != UODATE_NO_NEED || track_is_timer_run(TRACK_CUST_MLSP_NOT_SEND_TIMER_ID))
    {
        LOGD(L_OBD, L_V1, "1单片机正在升级中,延后处理");
        tr_start_timer(TRACK_CUST_MLSP_SEND_TIMER_ID, 10*1000, track_mlsp_send_new);
        return;
    }
#endif//__MCU_LONG_STANDBY_PROTOCOL__
    track_mlsp_send((void*)0);
}
/******************************************************************************
 *  Function    -  track_mlsp_send
 *  Description - 自动的发送处理
 * ----------------------------------------
 * v1.0  , 2017-08-15,  liwen  written
 * ----------------------------------------
 ******************************************************************************/
/*单片机升级时 track_is_in_update_bootload_cmd，
最好停掉重发定时器，禁止发非升级相关的指令 -- chengjun  2017-08-24*/

static void track_mlsp_send(void* parr)//0xFF:回复为失败0xFE:无回复,协议号:回复为成功
{
    static kal_uint8 last_cmd = 0;
    static kal_uint8 retry_count = 0;
    static kal_bool flag = KAL_TRUE;
    int v_size = 0;
    kal_uint8* buf = NULL;
    kal_uint8 par = (kal_uint8)parr;
    if (par >= 1)
    {
        track_stop_timer(TRACK_CUST_MLSP_REPEAT_TIMER_ID);
        if (last_cmd == par)
        {
            VECTOR_Erase2(vec_send_mcu_queue, 0, 1, KAL_FALSE);
            flag = KAL_TRUE;
            retry_count = 0;
        }
        else
        {
            retry_count++;
            if (retry_count >= 10)
            {
                VECTOR_Erase2(vec_send_mcu_queue, 0, 1, KAL_FALSE);
                flag = KAL_TRUE;
                retry_count = 0;
                if (G_realtime_data.reset_count < 5)
                {
                    G_realtime_data.reset_count++;
                    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                    LOGD(L_OBD, L_V1, "与单片机通信失败,重启%d", G_realtime_data.reset_count);
                    track_cust_restart(66, 0);
                    return;
                }
                else
                {
                    LOGD(L_OBD, L_V1, "丢弃,协议号为%02X", last_cmd);
                }
            }
        }
    }
    v_size = VECTOR_Size(vec_send_mcu_queue);
    LOGD(L_OBD, L_V1, "##Size:%d,Par:%02X,Last:%02X##", v_size, par, last_cmd);
    if (v_size <= 0)return;

    if (!flag && par == 0)
    {
        LOGD(L_OBD, L_V1, "其他协议指令未收到回复,延后发送");
        tr_start_timer(TRACK_CUST_MLSP_SEND_TIMER_ID, REPEAT_TIME, track_mlsp_send_new);
        return;
    }
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
    if (track_drv_get_mcu_sw_update_status() != UODATE_NO_NEED || track_is_timer_run(TRACK_CUST_MLSP_NOT_SEND_TIMER_ID))
    {
        LOGD(L_OBD, L_V1, "2单片机正在升级中,延后处理");
        flag = KAL_TRUE;
        retry_count = 0;
        tr_start_timer(TRACK_CUST_MLSP_SEND_TIMER_ID, 10*1000, track_mlsp_send_new);
        return;
    }
#endif//__MCU_LONG_STANDBY_PROTOCOL__
    flag = KAL_FALSE;
    buf = (kal_uint8*)VECTOR_At(vec_send_mcu_queue, 0);
    if (buf)
    {
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
        last_cmd = buf[3];
        track_drv_write_exmode(buf, buf[2]);
#elif defined(__JM66__)
        last_cmd = buf[2];
        track_drv_write_exmode(buf, buf[3] + ((buf[4] & 0x00FF)<<8) + 7);
#endif//__MCU_LONG_STANDBY_PROTOCOL__
        track_start_timer(TRACK_CUST_MLSP_REPEAT_TIMER_ID, REPEAT_TIME, track_mlsp_send, (void*)0xFE);
    }
    else
    {
        LOGD(L_OBD, L_V1,  "ERROR BUFF!");
        VECTOR_Erase2(vec_send_mcu_queue, 0, 1, KAL_FALSE);
        flag = KAL_TRUE;
        retry_count = 0;
        track_mlsp_send_new();
    }
}
/******************************************************************************
 *  Function    -  track_mlsp_queue_send_req
 *  Description -  待发送数据推到队列
 * ----------------------------------------
 * v1.0  , 2017-08-15,  liwen  written
 * ----------------------------------------
 ******************************************************************************/
void track_mlsp_queue_send_req(kal_uint8* data, int len)
{
    kal_uint8* buf = NULL;
    int v_size = 0;
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
    if (len > MSLP_MAX_LEN || len <= 0)
#else
    if (0)
#endif//__MCU_LONG_STANDBY_PROTOCOL__
    {
        LOGD(L_OBD, L_V1,  "数据长度超出范围");
        return;
    }
    else
    {
        v_size = VECTOR_Size(vec_send_mcu_queue);
        if (v_size >= 50)
        {
            LOGD(L_OBD, L_V1, "MLSP队列已满,通信失败报警");
            //处理报警
            return;
        }
        buf = (kal_uint8*)Ram_Alloc(5, len + 1);
        if (buf)
        {
            memcpy(buf, data , len);
            VECTOR_PushBack(vec_send_mcu_queue, buf);
        }
        else
        {
            LOGD(L_OBD, L_V1, "Ram_Alloc 申请临时内存空间失败!!!");
            track_cust_restart(22, 0);
        }
        track_mlsp_send_new();
    }
}
static track_cust_mlsp_return_timeout(void)
{
    confirm("外设无回复");
}
void track_cust_mlsp_set_return(cmd_return_struct *return_sms)
{
    memcpy(&g_return_sms, return_sms, sizeof(cmd_return_struct));
    g_return_sms_valid = 1;
    tr_start_timer(TRACK_CUST_MLSP_COMMUNICATE_FAIL_TIMER_ID, 4 * 1000, track_cust_mlsp_return_timeout);
}
#endif//__MCU_LONG_STANDBY_PROTOCOL__

//不同项目不同协议各自处理
#if defined (__GT740__)|| defined(__GT420D__)

#define TRACKING_TIME 20 //追踪模式持续分钟
#define BOOT_TIME 3 //开机多久启动防拆报警 分钟
#define CAL_ELETRCITY 300 //  10*60计算电量频率
#define BOOT_ELE 50 //  电量2.85-2.95电量初始值

#define BAT_SWT_LOW_VALUE 275 // 更换电池下限
#define BAT_SWT_HIGH_VALUE 305 // 更换电池上限

#define BAT_CALIBRATIO_VOLT 272  //校准电压
//#define BAT_DYN_LOW_VALUE  275// 动态更换电池下限
//#define BAT_DYN_DIFFENCE_VALUE  20// 更换差值大于0.2v

#define HISTORY_ELE_FOR_LOW 60 //对应历史数据消耗电量值百分比,

#define RESEND_COMMAND_TIMEOUT_SEC 2000

typedef enum
{
    ST_VERSION = 1,
    ST_VOL,
    ST_TIME,
    ST_UPTIME, //远程升级延迟关机时间
    ST_LOWVOL,
    ST_HANDSHAKE,
    ST_KEYSTATUS,
    ST_RST
} st_parameter;

typedef enum
{
    sthour = 0,
    stmin,
    styear,
    stmonth,
    stday,
} st_time_sort;

/*****************************************************************************
*  Local variable                       局部变量
*****************************************************************************/
static char *cmd_parameters[PARAMETER_MAX] = {0};
static int cmd_parameters_count = 0;
static U32 g_tick_new = 0;
static kal_bool g_02_ok = KAL_FALSE;
static kal_uint8 g_mode = 0;
static kal_bool ensleep = FALSE,gps_need=KAL_FALSE;
static kal_bool b_track_mode = KAL_FALSE;//追踪模式
//static U8 Arithmetic_logic = 1;
static U8 first_syn = 0,fir_cmd01=0; //首次同步电压
static U8 wakeup2_syn = 0; // 模式二唤醒
static U16 first_volt = 0;
//static U16 first_calibration_ele=0;//电压校准的电量百分比
static kal_uint8 single_rst = 0xff;// 单片机首次上电标志
static kal_uint8 gsm_ele = 0; // 耗电逻辑得到剩余电量百分比
static kal_uint32 single_ele_value = 0; // 保存在单片机里的消耗电量值
static kal_uint32 first_10_value = 0; // 首次10包交互结果
static U8 RESETBAT = 0; // 是否换电池
static kal_uint32 gpsboottick = 0, gpsclosetick = 0, gpssleeptick = 0, gpsclosesleeptick = 0, gpsworksec = 0, gpssleepsec = 0;
#if defined(__GT741__)
static float InitElet = 3000000; // 2500*3600 2500mas单位改成秒  GT760 950*3600   GT770 540*3600

#else
static float InitElet = 6000000; // 2500*3600 2500mas单位改成秒  GT760 950*3600   GT770 540*3600
#endif
static const kal_uint32  mode2_pwrconsumption = 4320; // 1.2*3600 mas
static const kal_uint32  mode2_pwrgpsconsumption=2180 ;
static const kal_uint32  mode1_gps_pwrcons = 120;
static const kal_uint32  mode1_gps_sleeppwrcons = 8;
static kal_uint8 st_time[10] = {0};
static kal_uint8 wakeup_mode = 99; // 0 正常定时唤醒, 1 补传唤醒， 2 连接AMS唤醒,3防拆
static kal_uint8 st_rst = 99,led_num=99;
static U8 loaction_timeout_sec = 15;
static float temp = 0;
static float humidity = 0;
static kal_uint16 stsn = 0;//MCU给GSM开机次数查询
static kal_uint16 rst_flag = 0;
static char sttime[100] = {0};

/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/


/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/


/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/
extern void track_cust_teardown_alarm();
extern void track_cust_collision_alarm();
extern void track_cust_tilt_alarm();

extern void track_drv_sys_power_off_req(void);
extern void track_cust_sendlbs_limit(void);
/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/
void track_cust_module_update_mode(void);
void track_cust_module_auto_sw_mode(void);
void track_cust_module_status_inquiry(void);
void track_cust_query_st_teardown_status();
void track_cust_notify_st_remove_alarm_modify(U8 op);
void track_cust_upload_param();
void auto_mode_callback(void* par);
void track_cust_module_extchip_shutdown(void);
void track_cust_module_delay_close(U8 op);
void track_cust_module_gps_boot_time();
void track_cust_module_cal_electricity(void* par);
void track_cust_module_boot_cal();
void track_cust_resetbat(U8 par);
void track_cust_module_clearup();
void track_cust_save_logfile(kal_uint8 *data, kal_uint16 len);
kal_bool track_cust_is_innormal_wakeup();
U32 track_cust_rem_tracking_time(U8 reset,U8* op);
void track_cust_send03(void* delay_off_sec);
void track_cust_send04(void* par);
void track_cust_send06(void* par);
void track_cust_upload_param(void);
kal_uint8 track_project_save_switch(U8 op, sw_type type);
void track_cust_openup_trackmode_lbsalm(U8 par);
U8 track_cust_syn_st_time(kal_uint32 arg, kal_uint8 app);
void track_cust_get_ele_by_st();
void track_cust_module_cal_ele_condition(U8 par);
void track_cust_send21(void* par);
void track_cust_module_Handshake(void);
/*****************************************************************************
*  Local Functions                      本地函数
******************************************************************************/

kal_uint32 track_cust_gt420d_carstatus_data(kal_uint8 arg)
{
    char buf[100] = {0};
    applib_time_struct currTime_ams = {0};
    if (arg != 99)
    {
        G_realtime_data.car_status = arg;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
            #if defined(__GT420D__)
            track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                snprintf(buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d CAR STATUS %d\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec,
                            G_realtime_data.car_status);
                LOGD(5,5,"len:%d",strlen(buf));
                track_cust_save_logfile(buf, strlen(buf));
            #endif

        LOGD2(L_APP,L_V5,"车辆状态标志位:%d",G_realtime_data.car_status);
    }
    else
    {
        LOGD2(L_APP,L_V5,"车辆状态标志位,(%d)",G_realtime_data.car_status);
        return G_realtime_data.car_status;
    }
}

kal_uint32 track_cust_lowpower_flag_data(kal_uint32 arg)
{
    if (arg != 99)
    {
        G_realtime_data.lowpower_flag = arg;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        LOGD2(L_APP,L_V7,"低电报警标志位:%d",G_realtime_data.lowpower_flag);
    }
    else
    {
        LOGD2(L_APP,L_V7,"低电报警标志位,(%d)",G_realtime_data.lowpower_flag);
        return G_realtime_data.lowpower_flag;
    }
}

void get_st_time(applib_time_struct *at)
{
    at->nYear = st_time[styear];
    at->nMonth = st_time[stmonth];
    at->nDay = st_time[stday];
    at->nHour = st_time[sthour];
    at->nMin = st_time[stmin];
}
kal_uint32 get_st_sn(kal_int32 sn)
{
    static U32 result = 0;
    if (sn==-1)
    {
        return result;
    }
    result = sn;
}

void track_cust_init_full_ele()
{
#ifdef __GT741__
#else
    if (G_importance_parameter_data.app_version.version==2)
    {
        InitElet = 3700000;
    }
    else  if (G_importance_parameter_data.app_version.version==3)
    {
        InitElet = 1800000;
    }
    else
    {
        InitElet = 6000000;
    }
#endif
    LOGD(L_OBD, L_V2, "%.0f", InitElet);
}

kal_uint8 track_cust_module_get_syn_vol()
{
    return first_syn;
}

U8 track_cust_module_tracking_mode(U8 op)
{
    if (op == 99)
    {
        return G_realtime_data.tracking_mode;
    }
    if (op != G_realtime_data.tracking_mode)
    {
        G_realtime_data.tracking_mode = op;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    }
    LOGD(L_OBD, L_V5, "%d", op);
    return 0;
}

U8 track_cust_remove_alarm_flag(U8 flag)
{
    static U8 lockup = 0;
    if (flag != 99)
    {
        lockup = flag;
        LOGD(L_APP,L_V5,"lockup:%d",lockup);
    }
    return lockup;
}

U8 track_cust_remove_alarm_lock(U8 lock)
{
    static U8 lockup = 0;
    if (lock != 99)
    {
        lockup = lock;
    }
    return lockup;
}
#ifdef __GT740__
//刚开机延迟30秒发短信
static void track_cust_delay_send_msg()
{
    track_os_intoTaskMsgQueue(track_cust_teardown_alarm);
    track_cust_remove_alarm_lock(1);//锁上
}
#elif __GT420D__
static void track_cust_delay_send_msg(void *par)
{
    kal_uint8 ser = (kal_uint8)par;
    switch (ser)
    {
        case 1:
            track_os_intoTaskMsgQueue(track_cust_teardown_alarm);
            break;
        case 2:
            track_os_intoTaskMsgQueue(track_cust_collision_alarm);
            break;
        case 3:
            track_os_intoTaskMsgQueue(track_cust_tilt_alarm);
            break;
        default:
            return ;
    }
    track_cust_remove_alarm_lock(1);//锁上
}
#endif
static void confirm(char *str)
{
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        LOGS("%s", str);
        return;
    }
    if (g_return_sms_valid)
    {
        track_cust_cmd_reply(&g_return_sms, str, strlen(str));
        g_return_sms_valid = 0;
    }
    else LOGD(L_OBD, L_V5, "%s", str);
}


kal_uint8 track_cust_05_status(U8 in)
{
    static kal_uint8 rlt = 0;
    if (in == 1 || in == 0)
    {
        rlt = in;
    }
    LOGD(L_OBD, L_V6, "rlt %d", rlt);
    return rlt;
}

U16 track_cust_calibration_table(U16 v)
{
    U8 i = 0;
    U16 v_table[19][2] =
    {
        {272, 18}, {270, 17},
        {269, 16}, {268, 15}, {267, 14}, {266, 13}, {264, 12}, {263, 11}, {262, 10},
        {259, 9}, {258, 8}, {256, 7}, {254, 6},{252, 5}, {249, 4}, {245, 3}, {241, 2}, {237, 1}, {200, 0}
    };
    /*
    {
    {285, 37}, {284, 36}, {283, 34}, {282, 32}, {281, 30}, {280, 29}, {279, 27}, {278, 26},
    {277, 25}, {276, 23}, {275, 22}, {274, 21}, {273, 19}, {272, 19}, {271, 18}, {270, 17},
    {269, 16}, {268, 15}, {267, 14}, {266, 13}, {265, 13}, {264, 12}, {263, 11}, {262, 10},
    {261, 10}, {260, 9}, {259, 9}, {258, 8}, {257, 7}, {256, 7}, {255, 6}, {254, 6}, {253, 5},
    {252, 5}, {249, 4}, {245, 3}, {241, 2}, {237, 1}, {200, 0}
    };*/
    for (; i < 19; i++)
    {
        if (v >= v_table[i][0])
        {
            return v_table[i][1];
        }
    }
}

kal_bool track_cust_existence_history_Date()
{
    if (G_realtime_data.cal_wk_time_struct.comsultion_ele == 0 && single_ele_value == 0)
    {
        return KAL_FALSE;
    }
    if (G_realtime_data.cal_wk_time_struct.comsultion_ele == 0 && single_ele_value != 0)
    {
        LOGD(L_OBD, L_V5, "无历史数据，取单片机保存的数据 %d", single_ele_value);
        if (G_realtime_data.ams_log)
        {
            char sttime_str[100] = {0};
            snprintf(sttime_str, 99, "(R)(%d)GSMELE DATA LOST,GET BY ST %d!!!", RUNING_TIME_SEC,single_ele_value);
            //track_cust_save_logfile(sttime_str, strlen(sttime_str));
            G_realtime_data.ams_push=1;

        }
        G_realtime_data.cal_wk_time_struct.comsultion_ele = single_ele_value;
    }
    return KAL_TRUE;
}

void track_cust_update_trackmode()
{
    U8 buf[50],result=0;
    U16 currtime;
    applib_time_struct time = {0};
    track_fun_get_time(&time, KAL_FALSE, &G_parameter.zone);
    currtime = time.nHour*60+time.nMin;
    if (G_realtime_data.tracking_mode)
    {
        sprintf(buf, "AT^GT_CM=04,1,2,%d,%d#\r\n",G_parameter.teardown_str.alarm_vel_min,currtime);
        result=1;
    }
    else if (G_parameter.extchip.mode1_en)
    {
        if (G_parameter.extchip.mode1_par1>60)
        {
            sprintf(buf, "AT^GT_CM=04,1,2,%d,%d#\r\n",G_parameter.extchip.mode1_par1/60,currtime);
        }
        else
        {

        }
        result = 1;
    }
    LOGD(L_OBD, L_V5, "%s", buf);
    if (result)
    {
        track_drv_write_exmode(buf, strlen(buf));
    }
}

void track_cust_module_deal_end_trackmode()
{
    if (G_realtime_data.tracking_mode)
    {
        U8 *p1,*p2;
        U32 *p3 = &G_realtime_data.trackmode_end_sec;
        p1 = &G_parameter.teardown_str.alarm_upcount;
        p2 = &G_realtime_data.tracking_count;
        LOGD(L_OBD, L_V5, "tracking count %d,count %d,end_time %d",*p2, *p1,*p3);

        if (*p2>=G_parameter.teardown_str.alarm_upcount)
        {
            LOGD(L_OBD, L_V5, "防拆模式结束!");
            G_realtime_data.tracking_mode=0;
            *p2=0;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
            track_cust_module_update_mode();//
        }
    }

}

void track_cust_module_overtime_upwifi()
{
    char buf[50] = {0};
#if defined(__GT741__)||defined(__GT420D__)
#else
    LOGD(L_OBD, L_V5, "超时未定位或搜不到星上传WIFI or LBS");
#endif
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        return;
    }
#if defined(__GT741__)
    if (G_parameter.extchip.mode==1)
#else if defined(__GT420D__)
    if (G_parameter.extchip.mode==1 || G_parameter.extchip.mode==2)
#endif
    {
        if(track_cust_gt420d_carstatus_data(99) == 2)
        {
            if(G_parameter.extchip.mode == 1)
            {
                if(track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 && track_cust_refer_motion_status_data(99) == 0 \
                    && track_cust_check_simalm_flag(99) == 0 && track_cust_check_firstpoweron_flag(99) == 1 &&track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0)
                {
                    return;
                }
                }
            else if(G_parameter.extchip.mode == 2)
            {
                if(track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 && track_cust_refer_motion_status_data(99) == 0 \
                    && track_cust_check_simalm_flag(99) == 0 && track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0)
                {
                    return;
                }
                }
            }
        if (track_is_timer_run(TRACK_CUST_EXTCHIP_MODE1_OVERTIME_TIMER_ID))
        {
            track_stop_timer(TRACK_CUST_EXTCHIP_MODE1_OVERTIME_TIMER_ID);
        }
        if (G_parameter.wifi.sw)
        {
            #if defined(__GT420D__)
                snprintf(buf, 50, "WIFI ON\n\r");
                LOGD(5,5,"len:%d",strlen(buf));
                track_cust_save_logfile(buf, strlen(buf));
        #endif
            track_cust_sendWIFI_paket();
        }
        else
        {
            track_cust_sendlbs_limit();

        }
        track_cust_lbs_send_rst(KAL_TRUE);
    }
#if defined(__GT741__)
    if (G_parameter.extchip.mode==1)track_start_timer(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID, (G_parameter.extchip.mode1_par1)*1000,track_cust_gps_module_work,(void*)3);//
#endif
}

//开机 LBS防拆报警条件
/******************************************************************************
 *  Function    -  track_cust_openup_trackmode_lbsalm
 *
 *  Purpose     - 处于防拆模式，第二次开机LBS报警逻辑
 *
 *  Description - par : 1 上线  2 同步时间
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 2017-09-04 xzq
 * v2.0  , 2017-11-27 xzq  不需要时间
 * ----------------------------------------
*******************************************************************************/
void track_cust_openup_trackmode_lbsalm(U8 par)
{
    static kal_bool linkup=KAL_FALSE, syn_time=KAL_FALSE;
    if (par == 1)
    {
        linkup=KAL_TRUE;
    }
    else if (par==2)
    {
        syn_time =KAL_TRUE;
    }
    if (linkup)
    {
        kal_uint16 snno=0;
        linkup=KAL_FALSE;
        snno = track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_PRE_REMOVE, KAL_TRUE, 0);
        track_cust_lock_serial_no(0, snno,LBS_ALM,track_cust_shutdown_condition);
    }
}

void track_cust_set_gps_status(track_enum_teardown_alarm_packet par)
{
    switch (par)
    {
        case GPS_NO_SAT:
            gps_nosat=1;
            break;
        case GPS_NOFIX:
            gps_nofix=1;
            if (track_project_save_switch(99, sw_mode1_gpsalm))
            {
                track_project_save_switch(0,sw_mode1_gpsalm);
            }
            break;
        case GPS_FIXED:
            gps_fixed=1;
            if (track_project_save_switch(99, sw_mode1_gpsalm))
            {
                track_project_save_switch(0,sw_mode1_gpsalm);
            }
            break;
        default:
            break;
    }

    track_cust_shutdown_condition((void*)par);

}


/******************************************************************************
 *  Function    -  track_cust_shutdown_condition
 *
 *  Purpose     - 关机逻辑
 *
 *  Description - par :Enum_Extalm_reply
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 2017-11-28 xzq
 * ----------------------------------------
*******************************************************************************/
void track_cust_shutdown_condition(void* par)
{
    static U8 uped=0,gps_up=0;
    U8 op = (U8)par;
    switch (op)
    {
        case LBS_ALM:
            lbsalm=1;
            break;
        case GPS_ALM:
            gpsalm=1;
            break;
        case POS_UPED:
            uped=1;
            break;
        case GPS_FIXED:
            gps_up=1;
            break;
        default:
            break;
    }

    if (G_parameter.extchip.mode !=1 && track_soc_login_status())
    {
        if (G_realtime_data.tracking_mode)
        {
#if defined( __GT741__ )||defined(__GT420D__)
            if (lbsalm)
#else
            if (lbsalm&&(gpsalm||gps_nosat||gps_nofix))
#endif
            {
                track_cust_module_deal_end_trackmode();
                if (!track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
                {
                    track_cust_module_delay_close(30);
                    tr_start_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE2_TIMER_ID, 25*1000, track_cust_linkout_alarm);
                }
            }
        }
        else
        {
            if (track_cust_gps_need_open())
            {
                if ((gps_up || (uped&&(gps_nosat||gps_nofix)))  && track_soc_position_queue()==0)
                {
                    uped=0;
                    if (!track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
                    {
                        track_cust_module_delay_close(30);
                        tr_start_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE2_TIMER_ID, 25*1000, track_cust_linkout_alarm);
                    }
                }
            }
            else
            {
                if ((uped||gps_up)  && track_soc_position_queue()==0)
                {
                    uped=0;
                    if (!track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
                    {
                        track_cust_module_delay_close(30);
                        tr_start_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE2_TIMER_ID, 25*1000, track_cust_linkout_alarm);
                    }
                }
            }
        }
    }

    LOGD(L_OBD, L_V5, "op %d ,uped/%d,gps_up/%d,mode %d,trackmode %d",op,uped,gps_up,G_parameter.extchip.mode,G_realtime_data.tracking_mode);
}

Ext_alarm_struct ext_alarm[2]={0};

void track_cust_extalm_callback(U16 snno)
{
    int i=0;
    for (i=0; i<19; i++)
    {
        LOGD(L_OBD, L_V6, "snno %d, %d ",snno,ext_alarm[0].no[i]);
        if (ext_alarm[0].no[i]==snno)
        {
            if (ext_alarm[0].func)
            {
                ext_alarm[0].func((void*)ext_alarm[0].alarm_reply[i]);
                break;
            }
        }
    }

}

/****************************************
* Function    track_cust_lock_serial_no

* Parameter

* Purpose     记录定位包的序列号

* Modification
******************************************/
U16 track_cust_lock_serial_no(U8 index,int no,kal_uint8 reply,Func_Extalm_Ptr func)
{
    static U8 ser_sign=0;

    LOGD(L_OBD, L_V3, "ser_sign %d, no %d, reply %d",ser_sign,no,reply);
    if (index>1 || ser_sign>19)return;
    ext_alarm[0].alarm_reply[ser_sign] = reply;
    ext_alarm[0].func = func;
    ext_alarm[0].no[ser_sign]=no;
    ser_sign++;
    return 0;
}

void track_cust_shutdown_all_location()
{
    if (track_is_timer_run(TRACK_CUST_WIFI_WORK_TIMER_ID))
    {
        track_stop_timer(TRACK_CUST_WIFI_WORK_TIMER_ID);
    }
    if (track_is_timer_run(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID))
    {
        track_stop_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID);
    }
    track_stop_timer(TRACK_CUST_WIFI_WORK_TIMER_ID);
    track_stop_timer(TRACK_CUST_T_TIME_ID);
    track_stop_timer(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID);
}

void track_cust_gps_module_work(void *par)
{
    U8 op = (U8)par;
#if defined(__GT741__)
    LOGD(L_APP, L_V5, "定时搜索WF%d",op);
    gps_nosat=1;
    track_cust_module_overtime_upwifi();
#else
    LOGD(L_APP, L_V5, "唤醒GPS定位模块%d",op);
    track_cust_gps_work_req((void *)19);

    if (!track_is_timer_run(TRACK_CUST_GPS_OPEN_TIMEROUT_TIMER_ID))
    {
        tr_start_timer(TRACK_CUST_GPS_OPEN_TIMEROUT_TIMER_ID, 30 * 1000, track_cust_gps_search_timeout);
        LOGD(L_APP, L_V5, "打开无星关闭定时器",);
    }
    if (G_parameter.extchip.mode==1)
    {
        if (!track_is_timer_run(TRACK_CUST_EXTCHIP_MODE1_OVERTIME_TIMER_ID))
        {
            tr_start_timer(TRACK_CUST_EXTCHIP_MODE1_OVERTIME_TIMER_ID, 120*1000, track_cust_module_overtime_upwifi);
        }
    }
#endif

}
void track_cust_into_mode1en()
{
    LOGD(L_APP, L_V5, "进入mode1_en 模式");
    if (RUNING_TIME_SEC>120&&RUNING_TIME_SEC<360)
    {
        track_cust_send03((void *) 180);//
    }
}

void track_cust_update_time_by_st()
{
    static kal_uint8 first=1;
    applib_time_struct sttime = {0};
    applib_time_struct sttime2 = {0};
    nvram_gps_time_zone_struct  zone1 = {0};
    applib_time_struct *currTime = NULL;
    nvram_gps_time_zone_struct *zone_p;

    if (first && st_time[styear] >= 17)
    {

#if (defined(__GT740__)||defined(__GT420D__)) && defined(__LANG_ENGLISH__)
        if (track_cust_is_upload_UTC_time() && G_parameter.zone.timezone_auto)
        {
            zone_p = track_mcc_to_timezone();
            if (zone_p != NULL)
            {
                memcpy(&zone1, zone_p, sizeof(nvram_gps_time_zone_struct));
            }
            else
            {
                LOGD(L_OBD, L_V3, "IMSI没有获取到!!!");
                return;
            }
        }
        else
#endif
        {
            memcpy(&zone1, &G_parameter.zone, sizeof(nvram_gps_time_zone_struct));
        }
        first = 0;
        if (zone1.zone == 'E')
        {
            zone1.zone = 'W'; //往前
        }
        else
        {
            zone1.zone = 'E'; //往后
        }
        currTime = &sttime2;
        if (1)
        {
            sttime.nYear = st_time[styear] + 2000;
            sttime.nMonth = st_time[stmonth];
            sttime.nDay = st_time[stday];
            sttime.nHour = st_time[sthour];
            sttime.nMin = st_time[stmin];
            sttime.nSec = 0;
            track_drv_utc_timezone_to_rtc(currTime, &sttime, zone1);
            track_fun_update_time_to_system(4, currTime);
        }
    }

}
/******************************************************************************
 *  Function    -  track_cust_ext_teardown_alm
 *
 *  Purpose     -  防拆报警处理
 *
 *  Description - par 1按键触发,2定时器回调
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 2016-01-30 xzq
 * ----------------------------------------
*******************************************************************************/
#ifndef __GT420D__
void track_cust_ext_teardown_alm(void* par)
{
    LBS_Multi_Cell_Data_Struct *lbs_strcut = track_drv_get_lbs_data();
    applib_time_struct time = {0};
    nvram_ext_chip_struct chip = {0}, chip2 = {0};
    kal_uint32 tick = 0;
    static kal_uint32 cur_tick = 0;
    kal_uint8 parameter = (kal_uint8)par;
    kal_bool lbs_up=KAL_FALSE;
    static kal_uint8 just_one = 0;
    kal_uint16 snno=0;
    LOGD(L_OBD, L_V5, "parameter %d", parameter);
    if (G_parameter.teardown_str.sw == 1 || G_parameter.extchip.mode == 1) //兼容模式一按键不会关机
    {
        if (G_parameter.teardown_str.sw == 0)
        {
            return;
        }
        LOGD(L_OBD, L_V5, "防拆报警触发!");
        //短信和模式切换触发即起作用

        if (G_parameter.teardown_str.alarm_type > 0 && G_parameter.teardown_str.alarm_type < 3) // 1和2
        {
            //短信
            tr_start_timer(TRACK_CUST_EXTCHIP_DELAY_SEND_ALARM_MSG_TIMER_ID, 30 * 1000, track_cust_delay_send_msg);
        }
        track_cust_remove_alarm_lock(0);//解锁

        if (track_soc_login_status())
        {
            lbs_up=KAL_TRUE;
            snno= track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_PRE_REMOVE, KAL_TRUE, 0);

            if (G_parameter.extchip.mode != 1 && G_parameter.extchip.mode1_en==0)
            {
                track_cust_lock_serial_no(0, snno,LBS_ALM,track_cust_shutdown_condition);
            }
        }
        if (G_parameter.extchip.mode != 1 && G_parameter.extchip.mode1_en==0)
        {
            U8 writer=0;

            if (G_realtime_data.tracking_mode==0)
            {
                writer = 1;
                G_realtime_data.tracking_mode=1;
                if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
                {
                    track_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
                }

                if (track_cust_gps_status()==0)
                {
                    gps_nosat=0;
                    gps_nofix=0;
                    gpsalm=0;
                }

                if (RUNING_TIME_SEC>150)
                {
                    if (!just_one)
                    {
                        just_one=1;
                        track_start_timer(TRACK_CUST_SEND_LBS_TIMER_ID, 2*60*1000, track_cust_send_lbs,(void*)3);
                        track_cust_send03((void *) 150);//
                    }
                }

                track_cust_module_update_mode();//

            }

            track_fun_get_time(&time, KAL_FALSE, &G_parameter.zone);
            if (time.nYear > (2000 + (*OTA_DEFAULT_HARDWARE_YEAR)) && track_cust_syn_st_time(0,99)!=0)
            {
                U32 timep;
                applib_time_struct tmp = {0};
                OTA_applib_dt_get_rtc_time(&tmp);
                timep = OTA_applib_dt_mytime_2_utc_sec(&tmp, 0);
                G_realtime_data.trackmode_end_sec = timep+G_parameter.teardown_str.alarm_upcount*G_parameter.teardown_str.alarm_vel_min*60+300;
                LOGD(L_OBD, L_V5, "sec%d ,temp %d",G_realtime_data.trackmode_end_sec,timep);
            }
            else
            {
                G_realtime_data.trackmode_end_sec=0;
            }

            if (G_realtime_data.tracking_count!=1)
            {
                writer=1;
                G_realtime_data.tracking_count=1;
            }
            //if(writer)
            {
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
            }

        }
        else
        {
            if (!lbs_up)track_project_save_switch(1, sw_cmd05_en);
            track_project_save_switch(1, sw_mode1_gpsalm);
        }

        track_cust_gps_module_work((void *)18);
    }
    else if (G_parameter.extchip.mode != 1)
    {
        //track_cust_module_delay_close(0);
    }
}
#else
void track_cust_module_shutdown()
{
    static kal_uint8 send_flag = 1;
    if (track_soc_position_queue() == 0 && track_soc_login_status())
    {
        track_cust_module_delay_close(30);
        send_flag = 0;
    }
    if (send_flag)
    {
        if (KAL_FALSE == track_is_timer_run(TRACK_CUST_ALM_SEND_SIGN_TIMER_ID))
            tr_start_timer(TRACK_CUST_ALM_SEND_SIGN_TIMER_ID, 2 * 1000, track_cust_module_shutdown);
    }
}

void track_cust_pre_remove_alarm()//740
{
    kal_uint8 send_packet18_counts = G_parameter.work_mode.teardown_time;
    static kal_uint8 i = 1;
    LBS_Multi_Cell_Data_Struct *lbs;
    BOOL lbs_valid=FALSE;
    LOGD(L_OBD, L_V2, "index:%d,timer is:%d", i, track_is_timer_run(TRACK_CUST_EXTCHIP_SEND_REMOVE_ALARM_TIMER_ID));
    if (!track_is_timer_run(TRACK_CUST_EXTCHIP_SEND_REMOVE_ALARM_TIMER_ID))
    {
        tr_start_timer(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID, 10 * 1000, track_cust_module_update_mode);
    }
    else
    {
        //防止冲突之前的报警
        LOGD(L_OBD, L_V2, "冲突,延迟2秒!");
        if (!track_is_timer_run(TRACK_CUST_EXTCHIP_SEND_REMOVE_ALARM_TIMER_ID))
        {
            tr_start_timer(TRACK_CUST_EXTCHIP_DELAY_SEND_REMOVE_ALARM_TIMER_ID, 2000, track_cust_pre_remove_alarm);
        }
    }
}

void track_mode2_to_mode1(void)//740
{
    LOGD(L_OBD, L_V5, "mode:%d,t_status_mode:%d", G_parameter.extchip.mode, G_realtime_data.tracking_status_mode);
#if 0
    if (track_is_timer_run(TRACK_CUST_EXTCHIP_SEND_REMOVE_ALARM_TIMER_ID))
    {
        track_stop_timer(TRACK_CUST_EXTCHIP_SEND_REMOVE_ALARM_TIMER_ID);
    }
#endif
    track_cust_remove_alarm_flag(0);
    G_realtime_data.tracking_mode = 0;
    if (G_parameter.extchip.mode == 1)
    {
        track_stop_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID);
        LOGD(L_APP,L_V5,"报警模式时间到，恢复之前的模式时间");
        //track_cust_check_teardown_flag(0);
        track_cust_module_update_mode();//
    }
    G_parameter.extchip.mode1_par1 = G_realtime_data.tracking_status_time;
    G_parameter.extchip.mode = G_realtime_data.tracking_status_mode;
    G_realtime_data.record_tracking_time = 0;
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    if ((G_parameter.extchip.mode == 1 && G_parameter.extchip.mode1_par1 > 5 )|| G_parameter.extchip.mode == 2 )
    {
        track_start_timer(TRACK_CUST_NET_MAXMUM_WORK_TIME_TIMER_ID,270000, track_cust_module_delay_close, (void *)30);
        track_start_timer(TRACK_CUST_SEND_DATA_MAXMUM_WORK_TIME_TIMER_ID,270000, track_cust_module_delay_close, (void *)30);
    }
}

void track_cust_checksend_gpsorlbs_paket(void* arg)
{
    track_gps_data_struct *gpsLastPoint = NULL;
    kal_uint8 param = (kal_uint8)arg;
    gpsLastPoint = track_cust_gpsdata_alarm();
    if (param == 1)
    {
        if (track_cust_gps_status() > 2)
        {
            track_cust_dsc_location_paket(gpsLastPoint,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,3,6);
            track_cust_check_beforepoweroff_flag(0);
            track_cust_check_teardown_flag(2);
            tr_stop_timer(TRACK_CUST_EXTCHIP_TEARDOWN_DELAY_SEND_GPSORLBS_TIMER_ID);
        }
        else
        {
            if (G_parameter.wifi.sw)
            {
                track_cust_sendWIFI_paket();
            }
            else
            {
                track_cust_dsc_location_paket(NULL,track_drv_get_lbs_data(),LOCA_MORE_LBS,DSC_CUST_POSITION_TIMING,3,6);
                track_cust_check_beforepoweroff_flag(0);
            }
        }
    }
    else
    {
        if (track_cust_gps_status() > 2)
        {
            track_cust_dsc_location_paket(gpsLastPoint,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,3,7);
            track_cust_check_beforepoweroff_flag(0);
            track_cust_check_downalm_flag(2);
            tr_stop_timer(TRACK_CUST_EXTCHIP_DARKALM_DELAY_SEND_GPSORLBS_TIMER_ID);
        }
        else
        {
            if (G_parameter.wifi.sw)
            {
                track_cust_sendWIFI_paket();
            }
            else
            {
                track_cust_dsc_location_paket(NULL,track_drv_get_lbs_data(),LOCA_MORE_LBS,DSC_CUST_POSITION_TIMING,3,7);
                track_cust_check_beforepoweroff_flag(0);
            }
        }
    }
}

void track_cust_ext_teardown_alm(void* par)
{
    applib_time_struct tmp = {0};
    nvram_work_mode_struct chip = {0};
    kal_uint32 tick = 0;
    static kal_uint32 cur_tick = 0;
    kal_uint8 param = (kal_uint8)par;
    track_gps_data_struct *gpsLastPoint = NULL;
    LOGD(L_DRV, L_V5, "teardown_sw:%d,tm:%d",G_parameter.teardown_str.sw,\
         G_parameter.teardown_str.alarm_vel_min);

    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        return;
    }
    track_cust_send_tear_dark_alarm_msg(1);
    LOGD(L_OBD, L_V5, "param:%d",param);
    if (param == 1)
        LOGD(L_OBD, L_V5, "开箱报警触发");
    else
        LOGD(L_OBD, L_V5, "无光报警触发");
    if (G_parameter.teardown_str.sw == 1)
    {
        if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
        {
            track_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
        }
        if(track_cust_check_gpsflag_status(99) ==  1)
        {
            track_cust_gps_work_req((void *)13);
        }
        else
        {
            if(G_parameter.wifi.sw)
                {
                    LOGD(5,5,"WIFI开关开着");
                    if (track_cust_wifi_scan_status())
                        {
                            LOGD(L_APP,L_V5,"检测到WIFI正在搜寻,继续搜WIFI");
                                }
                    else
                        {
                            LOGD(L_APP,L_V5,"检测到WIFI没在搜寻，开启WIFI");
                            track_cust_module_overtime_upwifi();
                            }
                    }
            }
        track_os_intoTaskMsgQueue(track_cust_pre_remove_alarm);
        if (param == 1)
        {
            track_cust_remove_alarm_flag(1);
            track_start_timer(TRACK_CUST_CHECK_TEARDOWN_FLAG, 60 * 1000, track_cust_check_teardown_flag, (void*)0);
            track_cust_check_teardown_flag(3);
            track_cust_check_teardown_flag(1);
        }
        else
        {
            track_start_timer(TRACK_CUST_CHECK_DOWNALM_FLAG, 60 * 1000, track_cust_check_downalm_flag, (void*)0);
            track_cust_check_downalm_flag(3);
            track_cust_check_downalm_flag(1);
        }

        if (param == 1)
        {
            LOGD(L_APP,L_V5,"延时40S定位确认发送GPS报警包还是WIFI包还是LBS报警包，防止开箱唤醒没有定位数据");
            track_start_timer(TRACK_CUST_EXTCHIP_TEARDOWN_DELAY_SEND_GPSORLBS_TIMER_ID, 40 * 1000, track_cust_checksend_gpsorlbs_paket,(void *)1);
        }
        else
        {
            LOGD(L_APP,L_V5,"延时40S定位确认发送GPS报警包还是WIFI包还是LBS报警包，防止无光唤醒没有定位数据");
            track_start_timer(TRACK_CUST_EXTCHIP_DARKALM_DELAY_SEND_GPSORLBS_TIMER_ID, 40 * 1000, track_cust_checksend_gpsorlbs_paket,(void *)2);
        }
        #if 0
        if (G_parameter.teardown_str.alarm_type > 0 && G_parameter.teardown_str.alarm_type < 3) //1和2
        {
            //短信
            tr_start_timer(TRACK_CUST_EXTCHIP_DELAY_SEND_ALARM_MSG_TIMER_ID, 30 * 1000, teardown_alarm_overtime);
            LOGD(L_APP,L_V5,"报警短信发送");
        }
        if (G_parameter.teardown_str.alarm_type > 1) //2和3
        {
            //call
            track_start_timer(TRACK_CUST_ALARM_TEAR_DOWN_CALL_TIMER, 10 * 1000, track_cust_make_call, (void*)1);
        }
        #endif
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        LOGD(L_OBD, L_V5, "time: %0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",tmp.nYear, tmp.nMonth, tmp.nDay, tmp.nHour, tmp.nMin, tmp.nSec);

        track_cust_remove_alarm_lock(0);//解锁
        if (G_parameter.extchip.mode == 1)
        {
            if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
            {
                tr_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
            }
            track_cust_module_alarm_lock(TR_CUST_ALARM_PRE_REMOVE, 1);
            track_cust_module_delay_close(150);
        }
    }
}

/*void track_cust_send26(void *par)
{
    U8 buf[100];
    kal_uint8 mode = (kal_uint8)par;
    sprintf(buf, "AT^GT_CM=26,%d,%d#\r\n",mode,G_parameter.crash_str.crash_grade);
    track_drv_write_exmode(buf, strlen(buf));
    LOGD(L_OBD, L_V5, "crash status is %d ", mode);
    track_start_timer(TRACK_CUST_EXTCHIP_WAIT26_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send26, (void *)mode);
}*/
#if 0
void track_cust_send27(void *par)
{
    U8 buf[100];
    kal_uint8 mode = (kal_uint8)par;
    sprintf(buf, "AT^GT_CM=27,%d,%d#\r\n",mode,G_parameter.incline_str.incline_grade);
    track_drv_write_exmode(buf, strlen(buf));
    LOGD(L_OBD, L_V5, "licline status is %d ", mode);
    track_start_timer(TRACK_CUST_EXTCHIP_WAIT27_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send27, (void *)mode);
}
#endif

void track_cust_updata_crash_mcu()
{
    LOGD(L_OBD, L_V5, "crash = %d,crash_grade = %d",G_parameter.crash_str.sw,G_parameter.crash_str.crash_grade);
    track_cust_send21((void *)1);
}
#if 0
void track_cust_updata_incline_mcu()
{
    LOGD(L_OBD, L_V5, "incline = %d,incline_grade = %d",G_parameter.incline_str.sw,G_parameter.incline_str.incline_grade);
    track_cust_send27((void *)G_parameter.incline_str.sw);
}
#endif

static kal_uint16 track_cust_module_conve(kal_uint16 par)
{
    kal_uint16 temp = 0;
    LOGD(L_APP,L_V5,"par:%d",par);
    temp = par / (COLLISION_LEVEL_MAX / MCU_REG_MAX) + ((par % (COLLISION_LEVEL_MAX / MCU_REG_MAX) > COLLISION_LEVEL_MAX / (MCU_REG_MAX*2)) ? 1 : 0);
    //LOGS("temp:%d",temp);
    return temp;
}

applib_time_struct* track_cust_mlsp_get_mcu_time(void)
{
    //返回MCU时间
    return &Mcu_Time;
}

static void  track_temp_form_mcu(float temp_s)
{
    temp = 	temp_s;
}
float track_get_temp_form_mcu()
{
    LOGD(L_APP,L_V5,"%f",temp);
    return temp /10;
}
static void track_humidity_form_mcu(float humidity_s)
{
    humidity = humidity_s;
}
float track_get_humidity_form_mcu()
{
    LOGD(L_APP,L_V5,"%f",humidity);
    return humidity /10;
}

kal_uint16 track_cust_module_get_stsn()
{
    return stsn;
}
kal_uint16 track_cust_module_get_rst()
{
    return rst_flag;
}

char* track_cust_module_get_sttime()
{
    return sttime;
}
#endif

kal_bool track_cust_check_disk1_nvram_parameter(kal_uint8 type)
{
    kal_bool result = KAL_TRUE;
    kal_uint8 plac1 = 0, plac2 = 0;
    char * ret;

    if (G_parameter.hbt_acc_on== 0  && G_parameter.hbt_acc_on== 0)
    {
        LOGD(L_DRV, L_V1, "警告,数据区错乱!!!!hbt_acc_on%d  !!",G_parameter.hbt_acc_on);
        result = KAL_FALSE;
    }
    if (track_nvram_alone_parameter_read()->server.conecttype==255 || track_nvram_alone_parameter_read()->server.soc_type==255)
    {
        LOGD(L_DRV, L_V1, "警告,数据区域名错乱!!!!");
        result = KAL_FALSE;
    }
    if (!result)
    {
        if (G_realtime_data.ams_log)
        {
            applib_time_struct ap = {0};
            char sttime_str[100] = {0};
            get_st_time(&ap);
            snprintf(sttime_str, 99, "(%d)SERVER2 ERROR %s %d,%d,%d %d-%d-%d %d:%d!!!", RUNING_TIME_SEC,(char*)track_nvram_alone_parameter_read()->ams_server.url,track_nvram_alone_parameter_read()->ams_server.conecttype,track_nvram_alone_parameter_read()->ams_server.server_port,track_nvram_alone_parameter_read()->ams_server.server_type,ap.nYear,ap.nMonth,ap.nDay,ap.nHour,ap.nMin);
            //track_cust_save_logfile(sttime_str, strlen(sttime_str));
            G_realtime_data.ams_push=1;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

        }

    }
    return result;
}

kal_uint32 track_cust_MCUrefer_adc_data(kal_uint32 arg)
{
    extern kal_uint32 track_cust_MCUset_adc_data(kal_uint32 arg);
    U16   V_LEVEL;
    if(G_parameter.percent >= 60)
    {
        V_LEVEL = 400;
        }
    else
    {
        V_LEVEL = 380;
        }

    if (arg != 99)
    {
        adc_data = arg;
        if (adc_data > V_LEVEL)
        {
            track_cust_lowpower_flag_data(0);//电压超过3.8V才允许下一次低电报警发
        }
        track_cust_MCUset_adc_data(adc_data);
        LOGD2(L_APP,L_V5,"获取到的电压为:(%0.2fV),%d",((float)adc_data)/100,V_LEVEL);
    }
    else
    {
        LOGD2(L_APP,L_V5,"采集电池电压,(%0.2fV)",((float)adc_data)/100);
        return adc_data;
    }
}

static kal_uint8 mcu_420d_version[50]={0};
kal_uint8* track_drv_get_420d_mcu_version(void)
{
    return mcu_420d_version;
}

void track_cust_send03_lowpower(void* delay_off_sec)
{
    extern void track_soc_queue_all_to_nvram(void);
    const int sec = (int)delay_off_sec;
    if (G_parameter.extchip.mode == 2||G_parameter.extchip.mode == 1)
    {
        char buf[100] = {0};
        sprintf(buf, "AT^GT_CM=03,%d#\r\n", sec);
        track_drv_write_exmode(buf, strlen(buf));
        LOGD(L_OBD, L_V5, "下发 低电03指令");
        LOGD(L_OBD, L_V5, "delay shutoff %d sec", sec);
        LOGD(L_OBD, L_V5, "[%s]", buf);
        track_soc_queue_all_to_nvram();
        track_start_timer(TRACK_CUST_EXTCHIP_WAIT03_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send03, delay_off_sec);
    }
    
}

void track_cust_send03(void* delay_off_sec)
{
    extern void track_soc_queue_all_to_nvram(void);
    static int i = 0;
    char buf[100] = {0};
    char buf_ams[20] = {0};
    const int sec = (int)delay_off_sec;
#if defined(__NETWORK_LICENSE__)
    return;
#endif
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        return;
    }
    if (G_parameter.extchip.mode == 1)
    {
        if(track_is_timer_run(TRACK_CUST_EXTCHIP_WAIT04_TIMER_ID))
        {
            if( i < 20)
            {
                i++;
                LOGD(L_APP,L_V5,"模式一还有04协议包没有回复");
                track_start_timer(TRACK_CUST_EXTCHIP_WAIT03_TIMER_ID, 3000, track_cust_send03, (void *)10);
                return;
            }
            else
            {
                #if defined(__GT420D__)
                snprintf(buf_ams, 50, "04 error\n\r");
                LOGD(5,5,"len:%d",strlen(buf_ams));
                track_cust_save_logfile(buf_ams, strlen(buf_ams));
                #endif
                tr_stop_timer(TRACK_CUST_EXTCHIP_WAIT03_TIMER_ID);
            }
        }
        sprintf(buf, "AT^GT_CM=03,%d#\r\n", sec);
        track_drv_write_exmode(buf, strlen(buf));
        LOGD(L_OBD, L_V5, "下发 03指令");
        LOGD(L_OBD, L_V5, "delay shutoff %d sec", sec);
        LOGD(L_OBD, L_V5, "[%s]", buf);
        track_soc_queue_all_to_nvram();
        track_start_timer(TRACK_CUST_EXTCHIP_WAIT03_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send03, delay_off_sec);
    }
    
}

static void track_cust_send02()
{
    U8 buf[50];
    applib_time_struct time = {0};
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        return;
    }
    track_fun_get_time(&time, KAL_FALSE, &G_parameter.zone);
    if(time.nYear == 2015)
    {
        LOGD(5,5,"系统时间错误，return");
        return;
        }
    snprintf(buf, 50, "AT^GT_CM=02,%02d,%02d,%02d,%02d,%02d,%02d#\r\n", time.nHour, time.nMin, time.nSec, time.nYear - 2000, time.nMonth, time.nDay);
    track_drv_write_exmode(buf, strlen(buf));
    tr_start_timer(TRACK_CUST_EXTCHIP_WAIT02_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send02);
    LOGD(L_OBD, L_V5, "[%s]", buf);
}


void track_cust_send04(void* par)
{
    U8 buf[100];
    kal_uint8 mode = (kal_uint8)par;
    static U8 content = 0;
    if (mode == 2)
    {
        if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
        {
            return;
        }
        sprintf(buf, "AT^GT_CM=04,1,1,0,0#\r\n");
    }
    else if (mode == 1)
    {
        if(track_cust_gt420d_carstatus_data(99) == 2)
        {
            sprintf(buf, "AT^GT_CM=04,1,2,%d,0#\r\n",G_parameter.statictime.mode1_statictime);
                }
        else
        {
            sprintf(buf, "AT^GT_CM=04,1,2,%d,0#\r\n",G_parameter.extchip.mode1_par1);
            }
        track_cust_save_logfile(buf, strlen(buf));
    }
    else if (mode == 3)//运输模式
    {
        sprintf(buf, "AT^GT_CM=04,1,5,0#\r\n");
    }
    else if (mode == 0)
    {
        sprintf(buf, "AT^GT_CM=04,0,0,0#\r\n");
    }
    else
    {
        LOGD(L_OBD, L_V5, "参数错误!==mode %d==", mode);
        return;
    }
    track_drv_write_exmode(buf, strlen(buf));
    content++;
    track_start_timer(TRACK_CUST_EXTCHIP_WAIT04_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send04, (void *)mode);
    LOGD(L_OBD, L_V5, "[%s]", buf);
}

kal_uint8 track_cust_send05(kal_uint8 arg)
{
    U8 stbuf[30] = {0};
    if (arg == 0)
    {
        sprintf(stbuf, "AT^GT_CM=05,04,00#\r\n");
    }
    else if (arg == 1)
    {
        sprintf(stbuf, "AT^GT_CM=05,04,01,OK#\r\n");
    }
    else if (arg == 2)
    {
        sprintf(stbuf, "AT^GT_CM=05,04,02,OK#\r\n");
    }
    track_drv_write_exmode(stbuf, sizeof(stbuf));
    track_start_timer(TRACK_CUST_EXTCHIP_WAIT05_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send05,(void *)arg);
    LOGD(L_OBD, L_V5, "[%s]", stbuf);
}

void track_cust_send06(void *par)
{
    U8 buf[100];
    static U8 content = 0;
    kal_uint8 flag = (kal_uint8)par;
    sprintf(buf, "AT^GT_CM=06,%d#\r\n",flag);
    track_drv_write_exmode(buf, strlen(buf));
    LOGD(L_OBD, L_V5, "light status is %d ", flag);
    track_start_timer(TRACK_CUST_EXTCHIP_WAIT06_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send06, (void *)flag);
    LOGD(L_OBD, L_V5, "[%s]", buf);
}


void track_cust_send09(void)
{
    U8 buf[30];
    sprintf(buf, "AT^GT_CM=09#\r\n");
    track_drv_write_exmode(buf, strlen(buf));
    tr_start_timer(TRACK_CUST_EXTCHIP_WAIT09_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send09);
    LOGD(L_OBD, L_V5, "[%s]", buf);
}

void track_cust_send09_out(void)
{
    U8 buf[30];
    sprintf(buf, "AT^GT_CM=09,0#\r\n");
    track_drv_write_exmode(buf, strlen(buf));
    tr_start_timer(TRACK_CUST_EXTCHIP_WAIT09_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send09_out);
    LOGD(L_OBD, L_V5, "[%s]", buf);
}


//单片机读取电池电压
void track_cust_send10(void)
{
    U8 buf[30];
    sprintf(buf, "AT^GT_CM=10,0,0#\r\n"); // 写
    track_drv_write_exmode(buf, strlen(buf));
    tr_start_timer(TRACK_CUST_EXTCHIP_RESEND_WAIT10_TIMER_ID, 2*1000, track_cust_send10);
    LOGD(L_OBD, L_V5, "[%s]", buf);
}
static void track_cust_send11()
{
    U8 buf[30];
    sprintf(buf, "AT^GT_CM=11,OK#\r\n");
    track_drv_write_exmode(buf, strlen(buf));
    LOGD(L_OBD, L_V5, "[%s]", buf);
}
static void track_cust_send13(void* pt)
{
    U8 buf[30];
    U8 par = (U8)pt;
    if (par == 4)
    {
        sprintf(buf, "AT^GT_CM=13,%d#\r\n", par);
        track_start_timer(TRACK_CUST_EXTCHIP_WAIT13_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send13, pt);
    }
    else
    {
        sprintf(buf, "AT^GT_CM=13,%d,OK#\r\n", par);
    }
    track_drv_write_exmode(buf, strlen(buf));
    LOGD(L_OBD, L_V5, "[%s]", buf);
}

kal_uint8 track_cust_420d_testmode_ledset(kal_uint8 arg)
{
    if (arg == 1)
    {
        track_cust_send17((void*)3,(void*)1);
        track_start_timer(TRACK_CUST_TESTMODE_LEDSET_TIMER_ID, 3000, track_cust_420d_testmode_ledset,(void*)0);
    }
    else
    {
        track_cust_send17((void*)3,(void*)0);
    }
}

void track_cust_resend_17(void)
{
    track_cust_send17((void*) par1,(void*) par2);
}

void track_cust_send17(void* pt1, void* pt2)
{
    U8 buf[30];
    par1 = (U8)pt1;
    par2 = (U8)pt2;
    //if(led_num==par)return;
    sprintf(buf, "AT^GT_CM=17,%d,%d#\r\n", par1,par2);
    track_drv_write_exmode(buf, strlen(buf));

    tr_start_timer(TRACK_CUST_EXTCHIP_WAIT17_TIMER_ID, 4000, track_cust_resend_17);
    LOGD(L_OBD, L_V5, "[%s]", buf);
}

void track_cust_send18(void* par)
{
    kal_uint8 arg = (kal_uint8)par;
    U8 stbuf[30] = {0};
    if (arg == 1)
    {
        sprintf(stbuf, "AT^GT_CM=18#\r\n");
    }
    else if (arg == 2)
    {
        sprintf(stbuf, "AT^GT_CM=18,1,OK#\r\n");
        track_drv_write_exmode(stbuf, sizeof(stbuf));
        confirm("18,1, OK!");
        LOGD(L_OBD, L_V5, "[%s]", stbuf);
        return;
    }
    else
    {
        sprintf(stbuf, "AT^GT_CM=18,0,OK#\r\n");
        track_drv_write_exmode(stbuf, sizeof(stbuf));
        confirm("18,0, OK!");
        LOGD(L_OBD, L_V5, "[%s]", stbuf);
        return;
    }
    track_drv_write_exmode(stbuf, sizeof(stbuf));
    tr_start_timer(TRACK_CUST_EXTCHIP_WAIT18_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send18);
    LOGD(L_OBD, L_V5, "[%s]", stbuf);
}

void track_cust_send21(void* par)
{
    char buf[100] = {0};
    static U8 content = 0;
    kal_uint8 value = (kal_uint8)par;
    if (value == 1)
    {
        sprintf(buf, "AT^GT_CM=21,%d,%d#\r\n", G_parameter.crash_str.sw,track_cust_module_conve(G_parameter.crash_str.crash_grade));
    }
    else if (value == 2)
    {
        sprintf(buf, "AT^GT_CM=21,2,1,0,0#\r\n");//误报运动
    }
    else if (value == 3)
    {
        sprintf(buf, "AT^GT_CM=21,2,2,0,0#\r\n");//误报静止
    }
    else if (value == 4)
    {
        sprintf(buf, "AT^GT_CM=21,3,1,0,0#\r\n");//查询设置参数
    }
    else if (value == 5)
    {
        sprintf(buf, "AT^GT_CM=21,3,2,0,0#\r\n");//查询静止运动状态
    }
    else if (value == 6)
    {
        return;
        sprintf(buf, "AT^GT_CM=21,4,1,%d,%d#\r\n",G_parameter.sensortime.static_check_time,G_parameter.sensortime.static_sensorcount);//设置运动SENSOR检测时间和震动次数
    }
    else if (value == 7)
    {
        return;
        sprintf(buf, "AT^GT_CM=21,4,2,%d,%d#\r\n",G_parameter.sensortime.exercise_check_time,G_parameter.sensortime.exercise_sensorcount);//设置静止SENSOR检测时间和震动次数
    }
    else if (value == 8)
    {
        sprintf(buf, "AT^GT_CM=21,5,0,0,0#\r\n");//查询设置的SENSOR检测时间和震动次数
    }
    track_drv_write_exmode(buf, strlen(buf));
    LOGD(L_OBD, L_V5, "collision is set to %d ,%d", G_parameter.crash_str.sw,G_parameter.crash_str.crash_grade);
    track_start_timer(TRACK_CUST_EXTCHIP_WAIT21_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send21,(void *)value);
    LOGD(L_OBD, L_V5, "[%s]", buf);
}

void track_cust_send22(void* par)
{
    char buf[100] = {0};
    static U8 content = 0;
    kal_uint8 value = (kal_uint8)par;
    LOGD(5,5,"%d",value);
    if(value == 3)//查询温湿度
    {
        sprintf(buf, "AT^GT_CM=22,3,%d#\r\n",G_parameter.humiture_sw);
        track_drv_write_exmode(buf, strlen(buf));
        LOGD(L_OBD, L_V5, "HUMITURE SET");
        track_start_timer(TRACK_CUST_EXTCHIP_WAIT22_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send22, (void *)value);
        }
    else if(value == 2)//查询温湿度阈值
    {
        sprintf(buf, "AT^GT_CM=22,2#\r\n");
        track_drv_write_exmode(buf, strlen(buf));
        LOGD(L_OBD, L_V5, "HUMITURE_ON ");
        track_start_timer(TRACK_CUST_EXTCHIP_WAIT22_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send22, (void *)value);
    }
    else if(value == 1)//设置温湿度阈值
    {
        sprintf(buf, "AT^GT_CM=22,1,%d,%d#\r\n",(kal_uint8)(G_parameter.temperature_threshold*10),(kal_uint8)(G_parameter.humidity_threshold*10));
        track_drv_write_exmode(buf, strlen(buf));
        LOGD(L_OBD, L_V5, "HUMITURE SET");
        track_start_timer(TRACK_CUST_EXTCHIP_WAIT22_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send22, (void *)value);
        }
    else if(value == 0)//查询温湿度
    {
        sprintf(buf, "AT^GT_CM=22,0#\r\n");
        track_drv_write_exmode(buf, strlen(buf));
        LOGD(L_OBD, L_V5, "HUMITURE SET");
        track_start_timer(TRACK_CUST_EXTCHIP_WAIT22_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send22, (void *)value);
        }
    LOGD(L_OBD, L_V5, "[%s]", buf);
}

void track_cust_send28(void)
{
    char buf[100] = {0};
    sprintf(buf, "AT^GT_CM=28#\r\n");
    track_drv_write_exmode(buf, strlen(buf));
    LOGD(L_OBD, L_V5, "[%s]", buf);
    tr_start_timer(TRACK_CUST_EXTCHIP_WAIT28_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send28);
}

#if 0
void track_cust_send25(void *par)
{
    U8 buf[100];
    static U8 content = 0;
    kal_uint8 flag = (kal_uint8)par;
    sprintf(buf, "AT^GT_CM=25,%d#\r\n",flag);
    track_drv_write_exmode(buf, strlen(buf));
    LOGD(L_OBD, L_V5, "light status is %d ", flag);
    track_start_timer(TRACK_CUST_EXTCHIP_WAIT25_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send25, (void *)flag);
    content ++;
    if (content > 5)
    {
        tr_stop_timer(TRACK_CUST_EXTCHIP_WAIT25_TIMER_ID);
        content = 0;
        LOGD(L_OBD, L_V5, "AT^GT_CM=25# 请求未成功");
    }
}

static void track_cust_send24(void* par)
{
    U8 buf[100];
    kal_uint8 mode = (kal_uint8)par;
    if (mode == 1)
    {
        sprintf(buf, "AT^GT_CM=24,1,1,%d#\r\n",G_parameter.extchip.mode1_par1);//定时
    }
    else if (mode == 2)
    {
        sprintf(buf, "AT^GT_CM=24,1,2,%d#\r\n",G_parameter.extchip.mode2_par1);//库存
    }
    else if (mode == 3)
    {
        sprintf(buf, "AT^GT_CM=24,1,1,5#\r\n");
    }
    else
    {
        LOGD(L_OBD, L_V5, "参数错误!==mode %d==", mode);
        return;
    }
    track_drv_write_exmode(buf, strlen(buf));
    track_start_timer(TRACK_CUST_EXTCHIP_WAIT24_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send24, (void *)mode);
    LOGD(L_OBD, L_V5, "[%s]", buf);
}

#endif
static void track_cust_send90()
{
    char buf[30] = {0};
    LOGD(L_OBD, L_V1, "");
    sprintf(buf, "AT^GT_CM=90#\r\n");
    track_drv_write_exmode(buf, strlen(buf));
    LOGD(L_OBD, L_V5, "[%s]", buf);
}

void track_cust_send98()
{
    char buf[30] = {0};
    LOGD(L_OBD, L_V1, "");
    sprintf(buf, "AT^GT_CM=98#\r\n");
    track_drv_write_exmode(buf, strlen(buf));
    LOGD(L_OBD, L_V5, "[%s]", buf);
    tr_start_timer(TRACK_CUST_EXTCHIP_WAIT98_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send98);
}

static void track_cust_send99()
{
    char buf[30] = {0};
    sprintf(buf, "AT^GT_CM=99#\r\n");
    tr_start_timer(TRACK_CUST_EXTCHIP_WAIT99_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send99);
    track_drv_write_exmode(buf, strlen(buf));
    LOGD(L_OBD, L_V5, "[%s]", buf);
}

#if defined(__GT420D__)
kal_uint32 track_cust_refer_charge_data(kal_uint32 arg)
{
    if (arg != 99)
    {
        G_parameter.charge_status = arg;
        LOGD2(L_APP,L_V5,"改变充电状态:%d",G_parameter.charge_status);
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    else
    {
        LOGD2(L_APP,L_V5,"充电状态为,(%d)",G_parameter.charge_status);
        return G_parameter.charge_status;
    }
}

kal_uint32 track_cust_refer_lightsensor_data(kal_uint32 arg)
{
    if (arg != 99)
    {
        light_status = arg;
        LOGD2(L_APP,L_V5,"改变光亮状态:%d",light_status);
    }
    else
    {
        LOGD2(L_APP,L_V5,"光亮状态为,(%d)",light_status);
        return light_status;
    }
}

kal_uint8 track_cust_needsend_charge_data(kal_uint8 arg)
{
    if (arg != 99)
    {
        charge_status = arg;
        LOGD2(L_APP,L_V5,"充电传点状态:%d",charge_status);
    }
    else
    {
        LOGD2(L_APP,L_V5,"充电传点状态:(%d)",charge_status);
        return charge_status;
    }
}

kal_uint8 track_cust_refer_keyin_data(kal_uint8 arg)
{
    if (arg != 99)
    {
        keyin_status = arg;
        LOGD2(L_APP,L_V5,"改变按键传点状态:%d",keyin_status);
    }
    else
    {
        LOGD2(L_APP,L_V5,"按键传点状态为,(%d)",keyin_status);
        return keyin_status;
    }
}

kal_uint8 track_cust_refer_chargein_data(kal_uint8 arg)
{
    if (arg != 99)
    {
        chargein_status = arg;
        LOGD2(L_APP,L_V5,"改变充电传点状态:%d",chargein_status);
    }
    else
    {
        LOGD2(L_APP,L_V5,"充电传点状态为,(%d)",chargein_status);
        return chargein_status;
    }
}

kal_uint8 track_cust_refer_motion_status_data(kal_uint8 arg)
{
    if (arg != 99)
    {
        Motion_state_status = arg;
        LOGD2(L_APP,L_V5,"运动状态传点状态:%d",Motion_state_status);
    }
    else
    {
        LOGD2(L_APP,L_V5,"运动状态状态为,(%d)",Motion_state_status);
        return Motion_state_status;
    }
}

kal_uint8 track_cust_refer_sensor_data(kal_uint8 arg)
{
    if (arg != 99)
    {
        sensor_type = arg;
        LOGD(L_APP,L_V5,"sensor_type:%d",sensor_type);
    }
    else
    {
        LOGD(L_APP,L_V5,"sensor_type:%d",sensor_type);
        return sensor_type;
    }
}
#endif

kal_uint8 track_cust_need_check_sportorstatic(kal_uint8 arg)
{
    if (arg != 99)
    {
        sportorstatic_flag = arg;
        LOGD(L_APP,L_V6,"sportorstatic_flag:%d",sportorstatic_flag);
    }
    else
    {
        LOGD(L_APP,L_V6,"sportorstatic_flag:%d",sportorstatic_flag);
        return sportorstatic_flag;
    }
}

kal_uint8 track_cust_set_sportorstatic(kal_uint8 par)
{
    extern void track_cust_check_sport_to_static_overtime(void);
    extern kal_uint8 track_cust_change_gps_speed(kal_uint8 arg);
    U8 buf[30];
    char buf1[100] = {0};
    applib_time_struct currTime_ams = {0};
    kal_uint8 tmp = 0;
    static int i = 0,j = 0;
    track_gps_data_struct *p_gpsdata;
    tmp = track_cust_gps_status();
    if (track_cust_need_check_sportorstatic(99) != 1)
    {
        LOGD(L_APP,L_V5,"1");
        return;
    }
    if(par == 0)
    {
        i = 0;
        j = 0;
        track_cust_need_check_sportorstatic(0);
        tr_stop_timer(TRACK_CUST_MODE_CHECK_SPORTORSTATIC_TIMER_ID);
        tr_stop_timer(TRACK_CUST_MODE_CHECK_SPORTORTOSTATIC_OVERTIME_TIMER_ID);
        return;
        }
    p_gpsdata = track_cust_backup_gps_data(0, NULL);
    LOGD(L_APP,L_V7,"tmp:%d,速度:%0.2f",tmp,p_gpsdata->gprmc.Speed);
    if (tmp > GPS_STAUS_OFF)
    {
        if (tmp >= GPS_STAUS_2D)
        {
            p_gpsdata = track_cust_backup_gps_data(0, NULL);
            if (track_cust_change_gps_speed(99) == 1)
            {
                p_gpsdata->gprmc.Speed = 5;
            }
            if (i < 30)
            {
                if (p_gpsdata->gprmc.Speed >= 0.5)
                {
                    LOGD(L_APP,L_V7,"速度大于0.5km/h,%d,%0.2f,",j,p_gpsdata->gprmc.Speed);
                    j++;
                }
                else
                {
                    LOGD(L_APP,L_V7,"速度小于0.5km/h,%d,%0.2f,",i,p_gpsdata->gprmc.Speed);
                }
                i++;
                track_start_timer(TRACK_CUST_MODE_CHECK_SPORTORSTATIC_TIMER_ID,1000,track_cust_set_sportorstatic,(void *)1);
                return;
            }
            else
            {
                if (j >= 15)
                {
                    tr_stop_timer(TRACK_CUST_MODE_CHECK_SPORTORSTATIC_TIMER_ID);
                    LOGD(L_APP,L_V7,"连续%d次速度大于0.5km/h，通知单片机误报停车",j);
                    #if defined(__GT420D__)
                    track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                    snprintf(buf1, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d MISTAKE STOP\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec);
                    LOGD(5,5,"len:%d",strlen(buf1));
                    track_cust_save_logfile(buf1, strlen(buf1));
                    #endif

                    track_cust_send21((void *)3);
                    track_cust_need_check_sportorstatic(0);
                    j = 0;
                    i = 0;
                    return;
                }
                else
                {
                    LOGD(L_APP,L_V7,"只有%d次速度大于0.5km/h,状态更新为静止",j);
                    tr_stop_timer(TRACK_CUST_MODE_CHECK_SPORTORSTATIC_TIMER_ID);
                    #if defined(__GT420D__)
                    track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                    snprintf(buf1, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d CAR STOP\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec);
                    LOGD(5,5,"len:%d",strlen(buf1));
                    track_cust_save_logfile(buf1, strlen(buf1));
                    #endif

                    track_cust_need_check_sportorstatic(0);
                    i = 0;
                    j = 0;
                }
            }
        }
        else
        {
            LOGD(L_APP,L_V5,"GPS不定位，30秒延时最长定位时间");
            tr_stop_timer(TRACK_CUST_MODE_CHECK_SPORTORSTATIC_TIMER_ID);
                     #if defined(__GT420D__)
                    track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                    snprintf(buf1, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d TIMEOVER BEGIN\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec);
                    LOGD(5,5,"len:%d",strlen(buf1));
                    track_cust_save_logfile(buf1, strlen(buf1));
                    #endif

            tr_start_timer(TRACK_CUST_MODE_CHECK_SPORTORTOSTATIC_OVERTIME_TIMER_ID,30*1000,track_cust_check_sport_to_static_overtime);
            i = 0;
            j = 0;
            return;
        }
    }
    else
    {
        if (j >= 15)
        {
            tr_stop_timer(TRACK_CUST_MODE_CHECK_SPORTORSTATIC_TIMER_ID);
            LOGD(L_APP,L_V7,"当前定位状态变为不定位，上一次连续%d次速度大于0.5km/h，通知单片机误报停车",j);
            i = 0;
            j = 0;
            track_cust_send21((void *)3);
            track_cust_need_check_sportorstatic(0);
            return;
        }
        else
        {
            i = 0;
            j = 0;
            tr_stop_timer(TRACK_CUST_MODE_CHECK_SPORTORSTATIC_TIMER_ID);
            track_cust_gps_work_req((void *)13);
            if (G_parameter.extchip.mode == 2)
            {
                LOGD(L_APP,L_V7,"模式2 当前定位状态为不定位，60秒延时最长定位时间");
                tr_start_timer(TRACK_CUST_MODE_CHECK_SPORTORTOSTATIC_OVERTIME_TIMER_ID,60*1000,track_cust_check_sport_to_static_overtime);
                return;
            }
            else if (G_parameter.extchip.mode == 1)
            {
                LOGD(L_APP,L_V7,"模式1 当前定位状态为不定位，60秒延时最长定位时间");
                tr_start_timer(TRACK_CUST_MODE_CHECK_SPORTORTOSTATIC_OVERTIME_TIMER_ID,60*1000,track_cust_check_sport_to_static_overtime);
                return;
            }
        }
    }
    track_cust_change_to_static();
}

void track_cust_change_to_static(void)
{
    char buf[100] = {0};
    applib_time_struct currTime_ams = {0};
    kal_uint8 tmp = 0;
    track_gps_data_struct *p_gpsdata;
    tmp = track_cust_gps_status();
    track_cust_gt420d_carstatus_data(2);
    if (G_parameter.extchip.mode == 2)
        {
                track_cust_refer_motion_status_data(1);
                LOGD(L_APP,L_V5,"模式二下静止传点");
                if(track_cust_check_gpsflag_status(99) ==  1)
                {
                    if (tmp >= GPS_STAUS_2D)
                    {
                        LOGD(L_APP,L_V5,"GPS定位着，直接传点");
                        p_gpsdata = track_cust_backup_gps_data(0, NULL);
                        #if defined(__GT420D__)
                        track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                        snprintf(buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d DIRECT UPLOAD\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec);
                        LOGD(5,5,"len:%d",strlen(buf));
                        track_cust_save_logfile(buf, strlen(buf));
                        #endif

                        track_cust_dsc_location_paket(p_gpsdata,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,1);
                        #if defined(__GT420D__)
                        track_cust_backup_gps_data(3, p_gpsdata);
                        LOGD(L_APP,L_V7,"更新有效点");
                        #endif
                        track_cust_check_beforepoweroff_flag(0);
                        track_cust_refer_motion_status_data(0);
                    }
                    else
                    {
                        if(tmp > GPS_STAUS_OFF && tmp <= GPS_STAUS_SCAN)
                        {
                            LOGD(L_APP,L_V5,"GPS未定位");
                            if (track_cust_wifi_scan_status())
                            {
                                LOGD(L_APP,L_V5,"检测到WIFI正在搜寻，上传WIFI定位");
                                }
                            else
                            {
                                track_cust_module_overtime_upwifi();
                                }
                            }
                        else if(tmp == GPS_STAUS_OFF)
                        {
                            LOGD(L_APP,L_V5,"GPS未开,打开GPS");
                            track_cust_gps_work_req((void *)13);
                            }
                        }
                    track_cust_gps_off_req((void *)3);
                    }
                else
                {
                    if(G_parameter.wifi.sw)
                        {
                            LOGD(5,5,"WIFI开关开着");
                            if (track_cust_wifi_scan_status())
                            {
                                LOGD(L_APP,L_V5,"检测到WIFI正在搜寻,继续搜WIFI");
                                }
                            else
                                {
                                    LOGD(L_APP,L_V5,"检测到WIFI没在搜寻，开启WIFI");
                                    track_cust_module_overtime_upwifi();
                                    }
                            }
                    else
                    {
                        LOGD(5,5,"WIFI开关没开,5秒后发送LBS");
                        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 5000, track_cust_sendlbs_limit);
                        }
                    }
            }
    else if (G_parameter.extchip.mode == 1)
        {
                track_cust_refer_motion_status_data(1);
                LOGD(L_APP,L_V5,"模式一下静止传点");
                if(track_cust_check_gpsflag_status(99) ==  1)
                {
                    if (tmp > GPS_STAUS_OFF && tmp < GPS_STAUS_2D)
                    {
                        LOGD(L_APP,L_V5,"检测到GPS正在搜星");
                        track_cust_gps_work_req((void *)13);
                    }
                    else if(tmp >= GPS_STAUS_2D)
                    {
                        p_gpsdata = track_cust_backup_gps_data(0, NULL);
                        #if defined(__GT420D__)
                        track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                        snprintf(buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d DIRECT UPLOAD\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec);
                        LOGD(5,5,"len:%d",strlen(buf));
                        track_cust_save_logfile(buf, strlen(buf));
                        #endif

                        LOGD(L_APP,L_V5,"GPS定位着，直接传点");
                        track_cust_dsc_location_paket(p_gpsdata,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,1);
                        #if defined(__GT420D__)
                        track_cust_backup_gps_data(3, p_gpsdata);
                        LOGD(L_APP,L_V7,"更新有效点");
                        #endif
                        track_cust_check_beforepoweroff_flag(0);
                        track_cust_refer_motion_status_data(0);
                        }
                    else
                    {
                        if (track_cust_wifi_scan_status())
                        {
                            LOGD(L_APP,L_V5,"检测到WIFI正在搜寻，关闭WIFI,开启GPS");
                            track_drv_wifi_cmd(0);
                            track_cust_gps_work_req((void *)13);
                        }
                        else
                        {
                            LOGD(L_APP,L_V5,"检测到WIFI没在搜寻，开启GPS");
                            track_cust_gps_work_req((void *)13);
                            }
                        }
                    }
                else
                {
                    if(G_parameter.wifi.sw)
                        {
                            LOGD(5,5,"WIFI开关开着");
                            if (track_cust_wifi_scan_status())
                            {
                                LOGD(L_APP,L_V5,"检测到WIFI正在搜寻,继续搜WIFI");
                                }
                            else
                                {
                                    LOGD(L_APP,L_V5,"检测到WIFI没在搜寻，开启WIFI");
                                    track_cust_module_overtime_upwifi();
                                    }
                            }
                    else
                    {
                        LOGD(5,5,"WIFI开关没开,30秒后发送LBS");
                        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 30000, track_cust_sendlbs_limit);
                        }
                    }
            }
    if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
    {
        tr_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
    }
    track_cust_module_delay_close(150);
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
}

void track_cust_change_to_sport(void)
{
    kal_uint8 tmp = 0;
    track_gps_data_struct *p_gpsdata;
    tmp = track_cust_gps_status();
    track_cust_gt420d_carstatus_data(1);
    if (G_parameter.extchip.mode == 2)
        {
            {
            LOGD(L_APP,L_V5,"收到运动状态，模式二上报间隔恢复为%d",G_parameter.extchip.mode2_par1);
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
            LOGD(L_APP,L_V6,"10秒之后执行模式2传点逻辑");
            tr_start_timer(TRACK_CUST_MODE2_KEEPGOING_TIMER_ID,10000,track_cust_module_update_mode);
                }
                track_cust_refer_motion_status_data(1);
                LOGD(L_APP,L_V5,"模式二下运动传点");
                if(track_cust_check_gpsflag_status(99) ==  1)
                {
                    if (tmp >= GPS_STAUS_2D)
                    {
                        LOGD(L_APP,L_V5,"GPS定位着，直接传点");
                        p_gpsdata = track_cust_backup_gps_data(0, NULL);
                        track_cust_dsc_location_paket(p_gpsdata,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,2);
                        #if defined(__GT420D__)
                        track_cust_backup_gps_data(3, p_gpsdata);
                        LOGD(L_APP,L_V7,"更新有效点");
                        #endif
                        track_cust_check_beforepoweroff_flag(0);
                        track_cust_refer_motion_status_data(0);
                    }
                    else
                    {
                        if(tmp > GPS_STAUS_OFF && tmp <= GPS_STAUS_SCAN)
                        {
                            LOGD(L_APP,L_V5,"GPS未定位");
                            if (track_cust_wifi_scan_status())
                            {
                                LOGD(L_APP,L_V5,"检测到WIFI正在搜寻，上传WIFI定位");
                                }
                            else
                            {
                                track_cust_module_overtime_upwifi();
                                }
                            }
                        else if(tmp == GPS_STAUS_OFF)
                        {
                            LOGD(L_APP,L_V5,"GPS未开,打开GPS");
                            track_cust_gps_work_req((void *)13);
                            }
                        }
                    }
                else
                {
                    if(G_parameter.wifi.sw)
                        {
                            LOGD(5,5,"WIFI开关开着");
                            if (track_cust_wifi_scan_status())
                            {
                                LOGD(L_APP,L_V5,"检测到WIFI正在搜寻,继续搜WIFI");
                                }
                            else
                                {
                                    LOGD(L_APP,L_V5,"检测到WIFI没在搜寻，开启WIFI");
                                    track_cust_module_overtime_upwifi();
                                    }
                            }
                    else
                    {
                        LOGD(5,5,"WIFI开关没开,5秒后发送LBS");
                        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 5000, track_cust_sendlbs_limit);
                        }
                    }
            }
    else if (G_parameter.extchip.mode == 1)
        {
            {
            LOGD(L_APP,L_V5,"收到运动状态，模式一上报间隔恢复为%d",G_parameter.extchip.mode1_par1);
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
                }
                track_cust_refer_motion_status_data(1);
                LOGD(L_APP,L_V5,"模式一下运动传点");
                if(track_cust_check_gpsflag_status(99) ==  1)
                {
                    if (tmp > GPS_STAUS_OFF && tmp < GPS_STAUS_2D)
                    {
                        LOGD(L_APP,L_V5,"检测到GPS正在搜星");
                        track_cust_gps_work_req((void *)13);
                    }
                    else if(tmp >= GPS_STAUS_2D)
                    {
                        p_gpsdata = track_cust_backup_gps_data(0, NULL);
                        LOGD(L_APP,L_V5,"GPS定位着，直接传点");
                        track_cust_dsc_location_paket(p_gpsdata,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,2);
                        #if defined(__GT420D__)
                        track_cust_backup_gps_data(3, p_gpsdata);
                        LOGD(L_APP,L_V7,"更新有效点");
                        #endif
                        track_cust_check_beforepoweroff_flag(0);
                        track_cust_refer_motion_status_data(0);
                        }
                    else
                    {
                        if (track_cust_wifi_scan_status())
                        {
                            LOGD(L_APP,L_V5,"检测到WIFI正在搜寻，关闭WIFI,开启GPS");
                            track_drv_wifi_cmd(0);
                            track_cust_gps_work_req((void *)13);
                        }
                        else
                        {
                            LOGD(L_APP,L_V5,"检测到WIFI没在搜寻，开启GPS");
                            track_cust_gps_work_req((void *)13);
                            }
                        }
                    }
                else
                {
                    if(G_parameter.wifi.sw)
                        {
                            LOGD(5,5,"WIFI开关开着");
                            if (track_cust_wifi_scan_status())
                            {
                                LOGD(L_APP,L_V5,"检测到WIFI正在搜寻,继续搜WIFI");
                                }
                            else
                                {
                                    LOGD(L_APP,L_V5,"检测到WIFI没在搜寻，开启WIFI");
                                    track_cust_module_overtime_upwifi();
                                    }
                            }
                    else
                    {
                        LOGD(5,5,"WIFI开关没开,30秒后发送LBS");
                        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 30000, track_cust_sendlbs_limit);
                        }
                    }
            }
    if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
    {
        tr_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
    }
    track_cust_module_delay_close(150);
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
}

static void cmd_01(void)
{
    //AT^GT_CM=01,KKS_GT700_V3.0_1.12,286,0:0,2,60,575,6,270,0,1199#
    //AT^GT_CM=01,GT700_V1.01,258,13:05,01,6,300,10,275,0,1,CRC#
    int mode, i, sum = 0;
    char buf[100] = {0};
    char* gettime = NULL;
    applib_time_struct currTime = {0};
    char *p, tmp[200] = {0},tmp2[50] = {0};
    static U8 t_j = 0, recode_time = 0;
    LOGD(L_OBD, L_V1, "");
    track_cust_query_st_teardown_status();
    /*
        if(track_is_timer_run(TRACK_CUST_CALCULATE_ELE_TIMER_ID))
        {
            tr_stop_timer(TRACK_CUST_CALCULATE_ELE_TIMER_ID);
        }
    */

    if (track_is_timer_run(TRACK_CUST_EXTCHIP_BATTERY_VOLTAGE_TIMER_ID))
    {
        tr_stop_timer(TRACK_CUST_EXTCHIP_BATTERY_VOLTAGE_TIMER_ID);
    }
    #if defined (__MCU_SW_UPDATE__)
    memset(mcu_sw_version_string, 0x00, sizeof(mcu_sw_version_string));
    snprintf(mcu_sw_version_string, sizeof(mcu_sw_version_string) - 1, "%s", cmd_parameters[1]);
    #endif
    for (i = 0; i < cmd_parameters_count; i++)
    {
        if (i >= 2 && i < cmd_parameters_count - 1)
        {
            char *p2;
            sum += atoi(cmd_parameters[i]);
            p = strchr(cmd_parameters[i], ':');
            while (p != NULL)
            {
                if (t_j == 0)
                {
                    st_time[t_j++] = atoi(cmd_parameters[i]);
                    LOGD(L_OBD, L_V8, "%d", st_time[t_j-1]);
                }
                if (t_j < sizeof(st_time))
                {
                    st_time[t_j++] = atoi(p + 1);
                    LOGD(L_OBD, L_V8, "%d", st_time[t_j-1]);
                }
                sum += atoi(p + 1);
                p++;//向前移一位
                if (strchr(p, ':') != NULL)
                {
                    p = strchr(p, ':');
                }
                else
                {
                    break;
                }
            }
                //AT^GT_CM=01,NT42A_1_190722,412,14:05:36:19:09:25,2,180,0,2,88,190814,0,15,1,9,60559#
        }
        LOGD(L_OBD, L_V8, "[%d,%d] %s -> %d,%d\r\n", i, t_j, cmd_parameters[i], sum, atoi(cmd_parameters[i]));
    }
    t_j = 0;
    if (sum != atoi(cmd_parameters[cmd_parameters_count - 1]))
    {
        LOGD(L_OBD, L_V1, "ERROR: 校验不通过. %d,%d", sum, atoi(cmd_parameters[cmd_parameters_count - 1]));
        tr_start_timer(TRACK_CUST_EXTCHIP_BATTERY_VOLTAGE_TIMER_ID, 5000, track_cust_module_status_inquiry);
        return;
    }
    if(G_parameter.drop_paket < 65534)
    {
        G_parameter.drop_paket += 1;
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    else
    {
        G_parameter.drop_paket = 0;
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        }
    snprintf(tmp, 500, "version:(%s)\r\n电压:(%s);\r\n当前时间:%s\r\n模式与参数:%s,%s\r\n起始时间:%s\r\n远程升级的时间:%s\r\n低电唤醒值:%s\r\n握手次数:%s\r\n光敏状态:%s\r\n单片机首次电池上电:%s\r\nSENSOR类型:%s\r\n开机原因类型:%s\r\nCRC :%s;",
             cmd_parameters[1], cmd_parameters[2], cmd_parameters[3],
             cmd_parameters[4], cmd_parameters[5], cmd_parameters[6],
             cmd_parameters[7], cmd_parameters[8], cmd_parameters[9],
             cmd_parameters[10], cmd_parameters[11], cmd_parameters[12],
             cmd_parameters[13], cmd_parameters[14]);
    LOGD(L_OBD, L_V1, "%s", tmp);

    wakeup_mode = atoi(cmd_parameters[13]);
    LOGD(L_OBD, L_V1, "wakeup_mode:%d", wakeup_mode);

    rst_flag = atoi(cmd_parameters[8]);
    LOGD(L_OBD, L_V1, "rst_flag:%d", rst_flag);

    snprintf(sttime, 100, "%s",cmd_parameters[3]);
    LOGD(L_OBD, L_V1, "sttime:%s", sttime);
    
    //if(first_syn && (G_realtime_data.cal_wk_time_struct.gsm_alrigothm_logic && G_realtime_data.cal_wk_time_struct.gsm_volt_low_time>=3))
#if defined (__MCU_SW_UPDATE__)
    memset(mcu_sw_version_string, 0x00, sizeof(mcu_sw_version_string));
    snprintf(mcu_sw_version_string, sizeof(mcu_sw_version_string) - 1, "%s", cmd_parameters[1]);
#endif
    track_cust_set_real_battery_volt(atoi(cmd_parameters[ST_VOL]));
    if (track_is_testmode())
    {
        snprintf(tmp, 150, "version:(%s)\r\nvoltage:(%s);\r\ntime:%s;  update time:%s; low voltage:%s; key_satus:%s; rst %s",
                 cmd_parameters[ST_VERSION], cmd_parameters[ST_VOL], cmd_parameters[ST_TIME],
                 cmd_parameters[ST_UPTIME], cmd_parameters[ST_LOWVOL], cmd_parameters[ST_KEYSTATUS],
                 cmd_parameters[ST_RST]);
    }
    else
    {
        snprintf(tmp, 200, "version:%s\r\nvoltage:%s;\r\ntime:%s;\r\n update time:%s;\r\n low voltage:%s;\r\n key_satus:%s;\r\n rst %s",
                 cmd_parameters[ST_VERSION], cmd_parameters[ST_VOL], cmd_parameters[ST_TIME],
                 cmd_parameters[ST_UPTIME], cmd_parameters[ST_LOWVOL], cmd_parameters[ST_KEYSTATUS],
                 cmd_parameters[ST_RST]);
    }

    confirm(tmp);
    /*----------------------------------------分隔线---------------------------------------*/
    /*       为了不影响工程测试，任何应用逻辑的return 在此下面增加               */
    /*----------------------------------------分隔线---------------------------------------*/
#if defined(__GT420D__)
    gettime = (kal_uint8*)Ram_Alloc(2, strlen(cmd_parameters[3]) + 17);
    if (gettime == NULL)
    {
        LOGD(L_OBD, L_V1, "动态分配内存出错!");
    }
    snprintf(gettime, strlen(cmd_parameters[3]) + 17, "%s,", cmd_parameters[3]);
    Mcu_Time.nYear=(gettime[10]-'0')  + ((gettime[9]-'0')*10) + 2000;
    Mcu_Time.nMonth=(gettime[13]-'0') + (gettime[12]-'0')*10;
    Mcu_Time.nDay=(gettime[16]-'0') + (gettime[15]-'0')*10;
    Mcu_Time.nHour=(gettime[1]-'0') + (gettime[0]-'0')*10;
    Mcu_Time.nMin=(gettime[4]-'0') + (gettime[3]-'0')*10;
    Mcu_Time.nSec=(gettime[7]-'0') + (gettime[6]-'0')*10;
    LOGD(L_APP, L_V5, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
         Mcu_Time.nYear,
         Mcu_Time.nMonth,
         Mcu_Time.nDay,
         Mcu_Time.nHour,
         Mcu_Time.nMin,
         Mcu_Time.nSec);
    if (Mcu_Time.nYear == 2015 && Mcu_Time.nMonth == 1 && Mcu_Time.nDay == 1)
    {
        LOGD(L_APP,L_V5,"单片机的时间有问题，不同步");
        }
    else
    {
        LOGD(L_APP,L_V5,"同步单片机的时间");
        track_drv_rtc_to_gmt0(&currTime, &Mcu_Time, G_parameter.zone);
        LOGD(L_APP, L_V5, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
             currTime.nYear,
             currTime.nMonth,
             currTime.nDay,
             currTime.nHour,
             currTime.nMin,
             currTime.nSec);
        track_drv_update_time_to_system(currTime);
    }
#if defined(__GT420D__)
    snprintf(buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d power on,%s,%s,%d\n\r",
             Mcu_Time.nYear,
             Mcu_Time.nMonth,
             Mcu_Time.nDay,
             Mcu_Time.nHour,
             Mcu_Time.nMin,
             Mcu_Time.nSec,
             cmd_parameters[13],
             cmd_parameters[8],
             G_parameter.drop_paket);
    LOGD(5,5,"len:%d",strlen(buf));
        track_cust_save_logfile(buf, strlen(buf));
    #endif    
    snprintf(tmp2, 50, "%s",cmd_parameters[1]);
    strncpy(mcu_420d_version,tmp2,50);
    LOGD(L_APP,L_V5,"mcu_420d_version:%s",mcu_420d_version);
    confirm(tmp2);
    track_cust_refer_sensor_data(atoi(cmd_parameters[12]));
#endif
    if (G_parameter.cal_time_sw==0 || (track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        track_cust_set_battery_volt(atoi(cmd_parameters[ST_VOL]));
        return;
    }

    if (recode_time == 0)
    {
        //track_os_intoTaskMsgQueue(track_cust_update_time_by_st);
        if (G_parameter.extchip.mode != 1 && G_realtime_data.ams_log == 1)
        {
            char* sttime_str = NULL;
            char* sttime_str2 = NULL;
            sttime_str = (kal_uint8*)Ram_Alloc(2, strlen(cmd_parameters[ST_TIME]) + 15);
            sttime_str2 = (kal_uint8*)Ram_Alloc(2, strlen(cmd_parameters[ST_TIME]) + 15);
            if (sttime_str == NULL ||sttime_str2 == NULL)
            {
                LOGD(L_OBD, L_V1, "动态分配内存出错!");
            }
            else
            {
                #if !defined(__GT420D__)
                if (wakeup_mode==wk_rem_alm)
                {
                    snprintf(sttime_str, strlen(cmd_parameters[ST_TIME]) + 15, "[%s][REMALM],", cmd_parameters[ST_TIME]);
                    G_realtime_data.ams_push=1;
                    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                }
                else
                {
                    snprintf(sttime_str, strlen(cmd_parameters[ST_TIME]) + 15, "[%s],", cmd_parameters[ST_TIME]);
                }
                #endif
                //track_cust_save_logfile(sttime_str, strlen(cmd_parameters[ST_TIME]) + 15);
                Ram_Free(sttime_str);
            }
#if defined(__GT420D__)
            snprintf(sttime_str2, strlen(cmd_parameters[ST_TIME]) + 15, "[%s],", cmd_parameters[ST_TIME]);
            Mcu_Time.nYear=(sttime_str2[7]-'0')  + ((sttime_str2[6]-'0')*10) + 2000;
            Mcu_Time.nMonth=(sttime_str2[10]-'0') + (sttime_str2[9]-'0')*10;
            Mcu_Time.nDay=(sttime_str2[13]-'0') + (sttime_str2[12]-'0')*10;
            Mcu_Time.nHour=(sttime_str2[1]-'0') + (sttime_str2[0]-'0')*10;
            Mcu_Time.nMin=(sttime_str2[4]-'0') + (sttime_str2[3]-'0')*10;
            LOGD(L_APP, L_V7, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
                 Mcu_Time.nYear,
                 Mcu_Time.nMonth,
                 Mcu_Time.nDay,
                 Mcu_Time.nHour,
                 Mcu_Time.nMin,
                 Mcu_Time.nSec);
#endif
        }
        fir_cmd01=1;
    }
    recode_time++;

    if (G_realtime_data.switch_down != atoi(cmd_parameters[ST_KEYSTATUS]))
    {
        G_realtime_data.switch_down = atoi(cmd_parameters[ST_KEYSTATUS]);
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
#ifndef __GT420D__
        track_cust_upload_param();
#endif
    }
    st_rst = atoi(cmd_parameters[ST_RST]);

}

static void cmd_02(void)
{
    U8 buf[100] = {0};
    LOGD(L_OBD, L_V1, "02 OK!");
    //confirm("02 OK!");
    track_stop_timer(TRACK_CUST_EXTCHIP_WAIT02_TIMER_ID);
}
static void cmd_03(void)
{
    LOGD(L_OBD, L_V1, "03 OK!");
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        confirm("03 OK!");
    }
    if (track_is_timer_run(TRACK_CUST_EXTCHIP_WAIT03_TIMER_ID))
    {
        track_stop_timer(TRACK_CUST_EXTCHIP_WAIT03_TIMER_ID);
        return;
    }
#if defined(__NETWORK_LICENSE__)
    return;
#endif
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        tr_start_timer(TRACK_CUST_EXTCHIP_DELAY_SHUTDOWN_TIMER_ID, 500, track_drv_sys_power_off_req);
    }
    else
    {
        track_stop_timer(TRACK_POWER_RESET_TIMER_ID);
//        track_drv_sys_power_off_req();
    }
}
static void cmd_04(void)
{
    LOGD(L_OBD, L_V1, "04 OK!");
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        confirm("04 OK!");
    }
    track_stop_timer(TRACK_CUST_EXTCHIP_WAIT04_TIMER_ID);
}

static void cmd_05(void)
{
    kal_uint8 tmp = 0;
    U8 buf[100] = {0};
    track_gps_data_struct *p_gpsdata = NULL;
    //confirm("(Short press)!");
    kal_uint8 value = 0;
    tr_stop_timer(TRACK_CUST_EXTCHIP_WAIT05_TIMER_ID);
    tmp = track_cust_gps_status();
    if (strcmp(cmd_parameters[2], "01") == 0)  //按下
    {
        if (strcmp(cmd_parameters[1], "04") == 0) //插入充电器
        {
            if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
            {
                confirm("charge in");
            }
            if(cmd_parameters_count != 4)
            {
                track_cust_check_firstpoweron_flag(0);
            }
            if (G_parameter.extchip.mode == 3)
            {
                G_parameter.extchip.mode = 1;
                LOGD(L_APP,L_V5,"模式三充电器插入,切换到模式一");
                tr_stop_timer(TRACK_CUST_EXTCHIP_WAIT04_POWERON_CHECK_TIMER_ID);
                track_cust_module_update_mode();
                Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
                sprintf(buf, "AT^GT_CM=05,04,01,OK#\r\n");
                LOGD(L_OBD, L_V5, "[%s]", buf);
                track_drv_write_exmode(buf, strlen(buf));
                return;
            }
            value = track_cust_refer_charge_data(99);
            if (value == 1)
            {
                sprintf(buf, "AT^GT_CM=05,04,01,OK#\r\n");
                LOGD(L_OBD, L_V5, "[%s]", buf);
                track_drv_write_exmode(buf, strlen(buf));
                LOGD(L_APP,L_V5,"充电器状态没改变，RETURN");
                return;
            }
            LOGD(L_APP,L_V5,"充电器插入");
            track_os_intoTaskMsgQueueExt(heartbeat_packets, (void *)115);
            track_cust_needsend_charge_data(1);
            track_cust_refer_charge_data(1);
            track_cust_refer_keyin_data(0);//中断掉按键上传
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
            if (G_parameter.extchip.mode == 2)
            {
                LOGD(L_APP,L_V5,"模式二下充电传点");
                if(track_cust_check_gpsflag_status(99) ==  1)
                {
                    if (tmp >= GPS_STAUS_2D)
                    {
                        LOGD(L_APP,L_V5,"GPS定位着，直接传点");
                        p_gpsdata = track_cust_backup_gps_data(0, NULL);
                        track_cust_dsc_location_paket(p_gpsdata,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,4);
                        #if defined(__GT420D__)
                        track_cust_backup_gps_data(3, p_gpsdata);
                        LOGD(L_APP,L_V7,"更新有效点");
                        #endif
                        track_cust_check_beforepoweroff_flag(0);
                        track_cust_needsend_charge_data(0);
                    }
                    else
                    {
                        if(tmp > GPS_STAUS_OFF && tmp <= GPS_STAUS_SCAN)
                        {
                            LOGD(L_APP,L_V5,"GPS未定位");
                            if (track_cust_wifi_scan_status())
                            {
                                LOGD(L_APP,L_V5,"检测到WIFI正在搜寻，上传WIFI定位");
                                }
                            else
                            {
                                track_cust_module_overtime_upwifi();
                                }
                            }
                        else if(tmp == GPS_STAUS_OFF)
                        {
                            LOGD(L_APP,L_V5,"GPS未开,打开GPS");
                            track_cust_gps_work_req((void *)13);
                            }
                        }
                    }
                else
                {
                    if(G_parameter.wifi.sw)
                        {
                            LOGD(5,5,"WIFI开关开着");
                            if (track_cust_wifi_scan_status())
                            {
                                LOGD(L_APP,L_V5,"检测到WIFI正在搜寻,继续搜WIFI");
                                }
                            else
                                {
                                    LOGD(L_APP,L_V5,"检测到WIFI没在搜寻，开启WIFI");
                                    track_cust_module_overtime_upwifi();
                                    }
                            }
                    else
                    {
                        LOGD(5,5,"WIFI开关没开,5秒后发送LBS");
                        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 5000, track_cust_sendlbs_limit);
                        }
                    }
            }
            if (G_parameter.extchip.mode == 1)
            {
                LOGD(L_APP,L_V5,"模式一下充电传点");
                if(track_cust_check_gpsflag_status(99) ==  1)
                {
                    if (tmp > GPS_STAUS_OFF && tmp < GPS_STAUS_2D)
                    {
                        LOGD(L_APP,L_V5,"检测到GPS正在搜星");
                        track_cust_gps_work_req((void *)13);
                    }
                    else if(tmp >= GPS_STAUS_2D)
                    {
                        p_gpsdata = track_cust_backup_gps_data(0, NULL);
                        LOGD(L_APP,L_V5,"GPS定位着，直接传点");
                        track_cust_dsc_location_paket(p_gpsdata,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,4);
                        #if defined(__GT420D__)
                        track_cust_backup_gps_data(3, p_gpsdata);
                        LOGD(L_APP,L_V7,"更新有效点");
                        #endif
                        track_cust_check_beforepoweroff_flag(0);
                        track_cust_needsend_charge_data(0);
                        }
                    else
                    {
                        if (track_cust_wifi_scan_status())
                        {
                            LOGD(L_APP,L_V5,"检测到WIFI正在搜寻，关闭WIFI,开启GPS");
                            track_drv_wifi_cmd(0);
                            track_cust_gps_work_req((void *)13);
                        }
                        else
                        {
                            LOGD(L_APP,L_V5,"检测到WIFI没在搜寻，开启GPS");
                            track_cust_gps_work_req((void *)13);
                            }
                        }
                    }
                else
                {
                    if(G_parameter.wifi.sw)
                        {
                            LOGD(5,5,"WIFI开关开着");
                            if (track_cust_wifi_scan_status())
                            {
                                LOGD(L_APP,L_V5,"检测到WIFI正在搜寻,继续搜WIFI");
                                }
                            else
                                {
                                    LOGD(L_APP,L_V5,"检测到WIFI没在搜寻，开启WIFI");
                                    track_cust_module_overtime_upwifi();
                                    }
                            }
                    else
                    {
                        LOGD(5,5,"WIFI开关没开,30秒后发送LBS");
                        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 30000, track_cust_sendlbs_limit);
                        }
                    }
            }
            if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
            {
                tr_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
            }
            sprintf(buf, "AT^GT_CM=05,04,01,OK#\r\n");
            LOGD(L_OBD, L_V5, "[%s]", buf);
        }
        if (strcmp(cmd_parameters[1], "05") == 0)
        {
            if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
            {
                confirm("Power Key Short Press Event");
                track_cust_420d_testmode_ledset(1);
            }
            LOGD(L_APP,L_V5,"五次按键上传点");
            track_cust_check_firstpoweron_flag(0);
            track_cust_refer_keyin_data(1);
            track_cust_refer_chargein_data(0);//中断掉充电上传
            tmp = track_cust_gps_status();
            if (G_parameter.extchip.mode == 2)
            {
                LOGD(L_APP,L_V5,"模式二下按键传点");
                if(track_cust_check_gpsflag_status(99) ==  1)
                {
                    if (tmp >= GPS_STAUS_2D)
                    {
                        LOGD(L_APP,L_V5,"GPS定位着，直接传点");
                        p_gpsdata = track_cust_backup_gps_data(0, NULL);
                        track_cust_dsc_location_paket(p_gpsdata,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,3);
                        #if defined(__GT420D__)
                        track_cust_backup_gps_data(3, p_gpsdata);
                        LOGD(L_APP,L_V7,"更新有效点");
                        #endif
                        track_cust_check_beforepoweroff_flag(0);
                        track_cust_refer_keyin_data(0);
                    }
                    else
                    {
                        if(tmp > GPS_STAUS_OFF && tmp <= GPS_STAUS_SCAN)
                        {
                            LOGD(L_APP,L_V5,"GPS未定位");
                            if (track_cust_wifi_scan_status())
                            {
                                LOGD(L_APP,L_V5,"检测到WIFI正在搜寻，上传WIFI定位");
                                }
                            else
                            {
                                track_cust_module_overtime_upwifi();
                                }
                            }
                        else if(tmp == GPS_STAUS_OFF)
                        {
                            LOGD(L_APP,L_V5,"GPS未开,打开GPS");
                            track_cust_gps_work_req((void *)13);
                            }
                        }
                    }
                else
                {
                    if(G_parameter.wifi.sw)
                        {
                            LOGD(5,5,"WIFI开关开着");
                            if (track_cust_wifi_scan_status())
                            {
                                LOGD(L_APP,L_V5,"检测到WIFI正在搜寻,继续搜WIFI");
                                }
                            else
                                {
                                    LOGD(L_APP,L_V5,"检测到WIFI没在搜寻，开启WIFI");
                                    track_cust_module_overtime_upwifi();
                                    }
                            }
                    else
                    {
                        LOGD(5,5,"WIFI开关没开,5秒后发送LBS");
                        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 5000, track_cust_sendlbs_limit);
                        }
                    }
            }
            if (G_parameter.extchip.mode == 1)
            {
                LOGD(L_APP,L_V5,"模式一下按键传点");
                if(track_cust_check_gpsflag_status(99) ==  1)
                {
                    if (tmp > GPS_STAUS_OFF && tmp < GPS_STAUS_2D)
                    {
                        LOGD(L_APP,L_V5,"检测到GPS正在搜星");
                        track_cust_gps_work_req((void *)13);
                    }
                    else if(tmp >= GPS_STAUS_2D)
                    {
                        p_gpsdata = track_cust_backup_gps_data(0, NULL);
                        LOGD(L_APP,L_V5,"GPS定位着，直接传点");
                        track_cust_dsc_location_paket(p_gpsdata,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,3);
                        #if defined(__GT420D__)
                        track_cust_backup_gps_data(3, p_gpsdata);
                        LOGD(L_APP,L_V7,"更新有效点");
                        #endif
                        track_cust_check_beforepoweroff_flag(0);
                        track_cust_refer_keyin_data(0);
                        }
                    else
                    {
                        if (track_cust_wifi_scan_status())
                        {
                            LOGD(L_APP,L_V5,"检测到WIFI正在搜寻，关闭WIFI,开启GPS");
                            track_drv_wifi_cmd(0);
                            track_cust_gps_work_req((void *)13);
                        }
                        else
                        {
                            LOGD(L_APP,L_V5,"检测到WIFI没在搜寻，开启GPS");
                            track_cust_gps_work_req((void *)13);
                            }
                        }
                    }
                else
                {
                    if(G_parameter.wifi.sw)
                        {
                            LOGD(5,5,"WIFI开关开着");
                            if (track_cust_wifi_scan_status())
                            {
                                LOGD(L_APP,L_V5,"检测到WIFI正在搜寻,继续搜WIFI");
                                }
                            else
                                {
                                    LOGD(L_APP,L_V5,"检测到WIFI没在搜寻，开启WIFI");
                                    track_cust_module_overtime_upwifi();
                                    }
                            }
                    else
                    {
                        LOGD(5,5,"WIFI开关没开,30秒后发送LBS");
                        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 30000, track_cust_sendlbs_limit);
                        }
                    }
            }
            if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
            {
                tr_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
            }
            track_cust_module_delay_close(150);
            sprintf(buf, "AT^GT_CM=05,05,01,OK#\r\n");
            LOGD(L_OBD, L_V5, "[%s]", buf);
        }
    }
    else if (strcmp(cmd_parameters[2], "02") == 0)  //弹起
    {
        if (strcmp(cmd_parameters[1], "04") == 0) //拔出充电器
        {
            if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
            {
                LOGS("no charge");
            }
            if(cmd_parameters_count != 4)
            {
                track_cust_check_firstpoweron_flag(0);
            }
            value = track_cust_refer_charge_data(99);
            if (value == 0)
            {
                sprintf(buf, "AT^GT_CM=05,04,02,OK#\r\n");
                LOGD(L_OBD, L_V5, "[%s]", buf);
                track_drv_write_exmode(buf, strlen(buf));
                LOGD(L_APP,L_V5,"充电器状态没改变，RETURN");
                return;
            }
            LOGD(L_APP,L_V5,"充电器拔出");
            track_os_intoTaskMsgQueueExt(heartbeat_packets, (void *)114);
            track_cust_refer_charge_data(0);
            track_cust_needsend_charge_data(2);
            track_cust_refer_keyin_data(0);//中断掉按键上传
            if (G_parameter.extchip.mode == 2)
            {
                LOGD(L_APP,L_V5,"模式二下拔电传点");
                if(track_cust_check_gpsflag_status(99) ==  1)
                {
                    if (tmp >= GPS_STAUS_2D)
                    {
                        LOGD(L_APP,L_V5,"GPS定位着，直接传点");
                        p_gpsdata = track_cust_backup_gps_data(0, NULL);
                        track_cust_dsc_location_paket(p_gpsdata,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,5);
                        #if defined(__GT420D__)
                        track_cust_backup_gps_data(3, p_gpsdata);
                        LOGD(L_APP,L_V7,"更新有效点");
                        #endif
                        track_cust_check_beforepoweroff_flag(0);
                        track_cust_needsend_charge_data(0);
                    }
                    else
                    {
                        if(tmp > GPS_STAUS_OFF && tmp <= GPS_STAUS_SCAN)
                        {
                            LOGD(L_APP,L_V5,"GPS未定位");
                            if (track_cust_wifi_scan_status())
                            {
                                LOGD(L_APP,L_V5,"检测到WIFI正在搜寻，上传WIFI定位");
                                }
                            else
                            {
                                track_cust_module_overtime_upwifi();
                                }
                            }
                        else if(tmp == GPS_STAUS_OFF)
                        {
                            LOGD(L_APP,L_V5,"GPS未开,打开GPS");
                            track_cust_gps_work_req((void *)13);
                            }
                        }
                    }
                else
                {
                    if(G_parameter.wifi.sw)
                        {
                            LOGD(5,5,"WIFI开关开着");
                            if (track_cust_wifi_scan_status())
                            {
                                LOGD(L_APP,L_V5,"检测到WIFI正在搜寻,继续搜WIFI");
                                }
                            else
                                {
                                    LOGD(L_APP,L_V5,"检测到WIFI没在搜寻，开启WIFI");
                                    track_cust_module_overtime_upwifi();
                                    }
                            }
                    else
                    {
                        LOGD(5,5,"WIFI开关没开,5秒后发送LBS");
                        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 5000, track_cust_sendlbs_limit);
                        }
                    }
            }
            if (G_parameter.extchip.mode == 1)
            {
                LOGD(L_APP,L_V5,"模式一下拔电传点");
                if(track_cust_check_gpsflag_status(99) ==  1)
                {
                    if (tmp > GPS_STAUS_OFF && tmp < GPS_STAUS_2D)
                    {
                        LOGD(L_APP,L_V5,"检测到GPS正在搜星");
                        track_cust_gps_work_req((void *)13);
                    }
                    else if(tmp >= GPS_STAUS_2D)
                    {
                        p_gpsdata = track_cust_backup_gps_data(0, NULL);
                        LOGD(L_APP,L_V5,"GPS定位着，直接传点");
                        track_cust_dsc_location_paket(p_gpsdata,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,5);
                        #if defined(__GT420D__)
                        track_cust_backup_gps_data(3, p_gpsdata);
                        LOGD(L_APP,L_V7,"更新有效点");
                        #endif
                        track_cust_check_beforepoweroff_flag(0);
                        track_cust_needsend_charge_data(0);
                        }
                    else
                    {
                        if (track_cust_wifi_scan_status())
                        {
                            LOGD(L_APP,L_V5,"检测到WIFI正在搜寻，关闭WIFI,开启GPS");
                            track_drv_wifi_cmd(0);
                            track_cust_gps_work_req((void *)13);
                        }
                        else
                        {
                            LOGD(L_APP,L_V5,"检测到WIFI没在搜寻，开启GPS");
                            track_cust_gps_work_req((void *)13);
                            }
                        }
                    }
                else
                {
                    if(G_parameter.wifi.sw)
                        {
                            LOGD(5,5,"WIFI开关开着");
                            if (track_cust_wifi_scan_status())
                            {
                                LOGD(L_APP,L_V5,"检测到WIFI正在搜寻,继续搜WIFI");
                                }
                            else
                                {
                                    LOGD(L_APP,L_V5,"检测到WIFI没在搜寻，开启WIFI");
                                    track_cust_module_overtime_upwifi();
                                    }
                            }
                    else
                    {
                        LOGD(5,5,"WIFI开关没开,30秒后发送LBS");
                        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 30000, track_cust_sendlbs_limit);
                        }
                    } 
            }
            if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
            {
                tr_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
            }
            track_cust_module_delay_close(150);
            sprintf(buf, "AT^GT_CM=05,04,02,OK#\r\n");
            LOGD(L_OBD, L_V5, "[%s]", buf);
        }
    }
    track_drv_write_exmode(buf, strlen(buf));
    if (fir_cmd01)
        LOGD(L_OBD, L_V3, "");
}

void cmd_06(void)
{
    U8 buf[20] = {0};
    char buf1[100] = {0};
    applib_time_struct currTime_ams = {0};

    if (cmd_parameters_count == 3)
    {
        LOGD(L_OBD, L_V1, "06 OK!");
        //confirm("06 OK!");
        track_stop_timer(TRACK_CUST_EXTCHIP_WAIT06_TIMER_ID);
        return;
    }
    //wakeup_mode = wk_rem_alm;
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        sprintf(buf, "AT^GT_CM=06,OK#\r\n");
        track_drv_write_exmode(buf, strlen(buf));
        confirm("Short press");
        return;
    }
    LOGD(L_APP,L_V5,"cmd_parameters[1]:%s",cmd_parameters[1]);
    #if defined(__GT420D__)
                track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                snprintf(buf1, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d receive 06,%s\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec,
                            cmd_parameters[1]
                            );
        track_cust_save_logfile(buf1, strlen(buf1));
    #endif
    if (!strcmp(cmd_parameters[1], "01") == 1)
    {
        LOGD(L_APP,L_V5,"1");
        if (G_parameter.extchip.mode != 3)
        {
            if (track_cust_check_teardown_flag(99) != 1)
            {
                track_cust_ext_teardown_alm((void *)1);
            }
            else
            {
                LOGD(L_APP,L_V5,"静默时间未到开箱报警不报");
            }
        }
        else
        {
            LOGD(L_APP,L_V5,"模式三不触发开箱报警");
        }
        sprintf(buf, "AT^GT_CM=06,01,OK#\r\n");
        track_drv_write_exmode(buf, strlen(buf));
    }
    else if (!strcmp(cmd_parameters[1], "02") == 1)
    {
        LOGD(L_APP,L_V5,"2");
        if (G_parameter.extchip.mode != 3)
        {
            if (track_cust_check_downalm_flag(99) != 1)
            {
                track_cust_ext_teardown_alm((void *)2);
            }
            else
            {
                LOGD(L_APP,L_V5,"静默时间未到无光报警不报");
            }
        }
        else
        {
            LOGD(L_APP,L_V5,"模式三不触发无光报警");
        }
        sprintf(buf, "AT^GT_CM=06,02,OK#\r\n");
        track_drv_write_exmode(buf, strlen(buf));
    }
    else
    {
        LOGD(L_APP, L_V5, "06协议有问题");
    }
    LOGD(L_OBD, L_V5, "[%s]", buf);
}

static void cmd_07(void)
{
    LOGD(L_OBD, L_V1, "07 OK!");
    //confirm("07 OK!");
}

static void cmd_08(void)
{
    static kal_bool _fir=KAL_TRUE;
    LOGD(L_OBD, L_V1, "08 OK!");
    //confirm("08 OK!");
    track_stop_timer(TRACK_CUST_EXTCHIP_WAIT08_TIMER_ID);
    if (_fir)
    {
        _fir=KAL_FALSE;
        track_cust_send10();
        tr_start_timer(TRACK_CUST_EXTCHIP_BATTERY_VOLTAGE_TIMER_ID, 500, track_cust_module_status_inquiry);
    }
}

static void cmd_09(void)
{
    LOGD(L_OBD, L_V1, "09 OK!");
    //confirm("09 OK!");
    track_stop_timer(TRACK_CUST_EXTCHIP_WAIT09_TIMER_ID);
}

static void cmd_10(void)
{
    U8 buf[20];
    static U8 count = 1;
    float value = 0;
    kal_uint8 poweroff_status = 0;
    kal_uint32 value2 = 0;
    U32 value3 = 0;
    U32 v_level[9] = { 3590000, 3630000, 3670000, 3710000, 3760000,3830000,3890000,3950000,3990000};
    LOGD(L_OBD, L_V1, "10 OK!");
    //confirm("10 OK!");
    first_10_value = 1;
    value2 = atoi(cmd_parameters[2]);
    LOGD(L_OBD, L_V5, "value2:%d,cmd_parameters[0]:%s,cmd_parameters[1]:%s,cmd_parameters[2]:%s", value2,cmd_parameters[0],cmd_parameters[1],cmd_parameters[2]);
    if (value2 != 0)
    {
        single_ele_value = atoi(cmd_parameters[2]);
        track_cust_MCUrefer_adc_data(single_ele_value);
        value3 = v_level[(G_parameter.percent/10) - 1];
        if (single_ele_value==0)
        {
            tr_start_timer(TRACK_CUST_EXTCHIP_WAIT10_TIMER_ID, 5*1000, track_cust_send10);
            return;
        }
        else
        {
            tr_stop_timer(TRACK_CUST_EXTCHIP_WAIT10_TIMER_ID);
        }
        LOGD(L_APP,L_V5,"single_ele_value:%d", single_ele_value);
        value = (((float)single_ele_value)/100.0);
        if (track_cust_lowpower_flag_data(99) != 1)
        {
            LOGD(L_APP,L_V5,"value:%0.2f,threshold:%d,threshold:%0.2f", value,value3,(((float)value3)/1000000.0));
            if (value < (((float)value3)/1000000.0))
            {
                LOGD(L_APP,L_V5,"count:%d", count);
                if (count < 4)
                {
                    count++;
                    tr_stop_timer(TRACK_CUST_EXTCHIP_RESEND_WAIT10_TIMER_ID);
                    tr_start_timer(TRACK_CUST_EXTCHIP_LOWBAT_WAIT10_TIMER_ID, 10*1000, track_cust_send10);
                }
                else
                {
                    tr_stop_timer(TRACK_CUST_EXTCHIP_LOWBAT_WAIT10_TIMER_ID);
                    tr_stop_timer(TRACK_CUST_EXTCHIP_RESEND_WAIT10_TIMER_ID);
                    tr_start_timer(TRACK_CUST_EXTCHIP_RESEND_WAIT10_TIMER_ID, 3*60*1000, track_cust_send10);
                    track_cust_module_420d_low_power_alarm();
                    count = 1;
                }
            }
            else
            {
                tr_stop_timer(TRACK_CUST_EXTCHIP_LOWBAT_WAIT10_TIMER_ID);
                tr_stop_timer(TRACK_CUST_EXTCHIP_RESEND_WAIT10_TIMER_ID);
                tr_start_timer(TRACK_CUST_EXTCHIP_RESEND_WAIT10_TIMER_ID, 3*60*1000, track_cust_send10);
                count =1;
            }
        }
        else
        {
            tr_stop_timer(TRACK_CUST_EXTCHIP_RESEND_WAIT10_TIMER_ID);
            tr_start_timer(TRACK_CUST_EXTCHIP_RESEND_WAIT10_TIMER_ID, 3*60*1000, track_cust_send10);
        }
        if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
        {
            LOGS("single_ele_value %0.2fV", value);
        }
    }
    else
    {
        poweroff_status = atoi(cmd_parameters[1]);
        if (poweroff_status ==0)
        {
            G_parameter.lowbat_status = 1;
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
            LOGD(L_APP,L_V5,"收到低电关机协议，10秒后关机");
            track_start_timer(TRACK_CUST_EXTCHIP_WAIT03_TIMER_ID, 10*1000, track_cust_send03_lowpower, (void *)10);
        }
        else
        {
            LOGD(L_APP,L_V5,"收到低电充电正常开机协议");
            tr_stop_timer(TRACK_CUST_CHECK_LOWBAT_POWEROFF_TIMER_ID);
            tr_stop_timer(TRACK_CUST_EXTCHIP_WAIT03_TIMER_ID);
            G_parameter.lowbat_status = 0;
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        }
        sprintf(buf, "AT^GT_CM=10,%d,OK#\r\n",poweroff_status);
        LOGD(L_OBD, L_V5, "[%s]", buf);
        track_drv_write_exmode(buf, strlen(buf));
    }
}

static void cmd_11(void)
{
    U8 status = 0, p1, p2, p3, singlebuff[100] = {0};
    LOGD(L_OBD, L_V1, "11 OK!");
    //confirm("11 OK!");
    get_st_sn(atoi(cmd_parameters[1]));
    LOGD(L_OBD, L_V5, "SC LOG %s", cmd_parameters[1]);
}
static void cmd_12(void)
{
    U8 buf[20];
    LOGD(L_OBD, L_V1, "12 OK!");
    //confirm("12 OK!");
    sprintf(buf, "AT^GT_CM=12,OK#\r\n");
    track_drv_write_exmode(buf, strlen(buf));
}

static void cmd_13(void)
{
    U8 par = atoi(cmd_parameters[1]);
    char buf[20] = {0};
    LOGD(L_OBD, L_V1, "13 OK!");
    sprintf(&buf[0], "13,%d OK!", par);
    confirm(&buf[0]);
    if ((G_realtime_data.nw_reset_sign>0 && mmi_Get_PowerOn_Type() == ABNRESET)  ||  track_cust_is_innormal_wakeup() )//搜不到网主动重启
    {
        track_cust_entry_sleep();
    }
    if (4 == par)
    {
        track_stop_timer(TRACK_CUST_EXTCHIP_WAIT13_TIMER_ID);
    }
    else
    {
        if (wakeup_mode != wk_rem_alm)
        {
            wakeup_mode = par;
        }
        track_cust_send13((void*)par);
        switch (par)
        {
            case 1:
                LOGD(L_OBD, L_V1, "补传唤醒GSM");
                track_cust_gps_off_req((void*)6);//禁止开启GPS
                break;
            case 2:
                LOGD(L_OBD, L_V1, "AMS连接唤醒GSM");
                track_cust_gps_off_req((void*)6);//禁止开启GPS
                break;
            case 3:
                LOGD(L_OBD, L_V1, "防拆触发唤醒GSM");
                break;
            default:
                LOGD(L_OBD, L_V1, "正常唤醒GSM");
                break;
        }
    }
}

static void cmd_17(void)
{
    U8 buf[20];
    LOGD(L_OBD, L_V1, "17 OK!");
    par1 = par2 = 0;
    track_stop_timer(TRACK_CUST_EXTCHIP_WAIT17_TIMER_ID);
}

static void cmd_18(void)
{
    U8 buf[20];
    if (strcmp(cmd_parameters[1], "1") == 0 ||strcmp(cmd_parameters[1], "0") == 0)  //按下
    {
        if (strcmp(cmd_parameters[1], "1") == 0)
        {
            LOGD(L_APP,L_V5,"看门狗检测正常，原件没问题");
            track_cust_send18((void *)2);
        }
        else
        {
            LOGD(L_APP,L_V5,"看门狗检测正常，原件有问题");
            track_cust_send18((void *)3);
        }
    }
    else
    {
        LOGD(L_OBD, L_V1, "18 OK!");
        confirm("18 OK!");
        track_stop_timer(TRACK_CUST_EXTCHIP_WAIT18_TIMER_ID);
    }

}

static void cmd_21(void)
{
    extern void sensor_callback_work(void);
    kal_uint8 tmp = 0;
    char buf[100] = {0};
    applib_time_struct currTime_ams = {0};
    char buf1[100] = {0};
    track_gps_data_struct *p_gpsdata;
    U8 value1, value2, value3,value4,value5;
    U8 par,sensor_status;
    tmp = track_cust_gps_status();
    LOGD(L_APP,L_V5,"G_parameter.extchip.mode=%d",G_parameter.extchip.mode);
    if (cmd_parameters_count == 1)
        {
            sprintf(buf, "AT^GT_CM=21,OK#\r\n");
            track_drv_write_exmode(buf, strlen(buf));
            sensor_callback_work();
            LOGD(L_OBD, L_V1, "Collision detected");
            //wakeup_mode = wk_collision_alm;
            LOGD(L_OBD, L_V1, "21 OK!");
            return ;
        }
    else
    {
        if (cmd_parameters_count == 2)
        {
            if (strcmp(cmd_parameters[1], "2") == 0 )
            {
                LOGD(L_APP,L_V5,"收到静止标志");
                sprintf(buf, "AT^GT_CM=21,2,OK#\r\n");
                track_cust_check_firstpoweron_flag(0);
                track_drv_write_exmode(buf, strlen(buf));
                if (track_cust_gt420d_carstatus_data(99) == 2)
                {
                    track_cust_check_static_to_sport(0);
                    LOGD(L_APP,L_V5,"查询到当前行车状态位静止，不处理");
                    sprintf(buf, "AT^GT_CM=21,2,OK#\r\n");
                    track_drv_write_exmode(buf, strlen(buf));
                    return;
                }
                track_cust_need_check_sportorstatic(1);
                    #if defined(__GT420D__)
                    track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                    snprintf(buf1, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d RECEIVE STATIC %d\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec,
                            track_cust_gt420d_carstatus_data(99));
                    LOGD(5,5,"len:%d",strlen(buf1));
                    track_cust_save_logfile(buf1, strlen(buf1));
                    #endif

                if (track_cust_gt420d_carstatus_data(99) == 1)
                {
                    track_cust_set_sportorstatic(1);
                }
                track_cust_module_delay_close(150);
            }
            else if (strcmp(cmd_parameters[1], "1") == 0 )
            {
                LOGD(L_APP,L_V5,"收到运动标志");
                sprintf(buf, "AT^GT_CM=21,1,OK#\r\n");
                track_cust_check_firstpoweron_flag(0);
                track_drv_write_exmode(buf, strlen(buf));
                if (track_cust_gt420d_carstatus_data(99) == 1)
                {
                    track_cust_set_sportorstatic(0);
                    LOGD(L_APP,L_V5,"查询到当前行车状态位运动，不处理");
                    sprintf(buf, "AT^GT_CM=21,1,OK#\r\n");
                    track_drv_write_exmode(buf, strlen(buf));
                    return;
                }
                track_cust_need_check_sportorstatic(1);
                if (track_cust_gt420d_carstatus_data(99) == 2)
                {
                    track_cust_check_static_to_sport(1);
                }
                track_cust_module_delay_close(150);
                LOGD(L_OBD, L_V5, "[%s]", buf);
            }
        }
        else if (cmd_parameters_count > 2)
        {
            value1 = atoi(cmd_parameters[1]);
            value2 = atoi(cmd_parameters[2]);
            value3 = atoi(cmd_parameters[3]);//当value1 == 3时,此位为开/停车标志
            value4 = atoi(cmd_parameters[4]);
            if (value1 == 0 || value1 == 1)
            {
                LOGD(L_DRV,L_V5,"sensor 参数设置成功");
                track_stop_timer(TRACK_CUST_EXTCHIP_WAIT21_TIMER_ID);
            }
            else if (value1 == 2)
            {
                if (value2 == 1)
                {
                    LOGD(L_APP,L_V5,"通知单片机误报开车状态 成功");
                }
                else if (value2 == 2)
                {
                    LOGD(L_APP,L_V5,"通知单片机误报停车状态 成功");
                }
            }
            else if (value1 == 3)//收到了 查询的车状态
            {
                if (value2 == 2)
                {
                    LOGD(L_APP,L_V5,"查到了车的状态=%d", value3);
                    if (value3 != 1 && value3 != 2)
                    {
                        LOGD(5,5,"回复错误的查询状态日志，重新查询");
                        track_cust_send21((void *)5);
                        return;
                            }
                    if (track_cust_gt420d_carstatus_data(99) == value3)
                    {
                        LOGD(L_APP, L_V5,"查到单片机给的状态与当前的状态相同,不处理");
                        track_stop_timer(TRACK_CUST_EXTCHIP_WAIT21_TIMER_ID);
                        return;
                    }
                    else
                    {
                        LOGD(L_APP, L_V5,"查到单片机给的状态与当前的状态不同");
                        if (value3 == 1)
                        {
                            sprintf(buf, "AT^GT_CM=21,1#\r\n");
                        }
                        else if(value3 == 2)
                        {
                            sprintf(buf, "AT^GT_CM=21,2#\r\n");
                        }
                        else
                        {
                            LOGD(5,5,"回复错误的查询状态日志，重新查询");
                            track_cust_send21((void *)5);
                            return;
                            }
                        track_cust_module_power_data(buf, strlen(buf));
                        //这里开始走中断判断
                    }
                }
                else if (value2 == 1)
                {
                    LOGD(L_OBD, L_V5,"sensor_sw:%d,sensor_level:%d", value4, value3);
                }
            }
            else if (value1 == 4 || value1 == 5)// 测试设置和查询回复
            {
                LOGD(L_OBD, L_V5,"当前震动测试参数为：");
                LOGD(L_OBD, L_V5,"AT^GT_CM=21,%d,%d,%d,%d,OK#",value1,value2,value3,value4);
                if (1 == value2)
                {
                    LOGD(L_OBD, L_V5,"\r\n开车参数:\r\n持续判断时间T1=%dS\r\n有效震动次数NUM1=%d", value3, value4);
                }
                else if (2 == value2)
                {
                    LOGD(L_OBD, L_V5,"\r\n停车参数:\r\n持续判断时间T2=%dS\r\n有效震动次数NUM2=%d", value3, value4);
                }
            }
            else if (value1 == 3 && value2 == 1)
            {
                LOGD(L_OBD, L_V5,"查询参数,待加");
            }
            else
            {
                LOGD(L_OBD, L_V5,"error:解析不了");
            }
        }
        LOGD(L_OBD, L_V5, "[%s]", buf);
    }
    track_stop_timer(TRACK_CUST_EXTCHIP_WAIT21_TIMER_ID);

}

static void cmd_22(void)
{
    U8 buf[100] = {0};
    float temp = 0, temperature_threshold = 0, humidity_threshold = 0;
    float humidity = 0;
    static kal_uint8 refer_flag = 0;
    kal_uint16 temperature = 0,humidity1 = 0,value = 0;
    if (cmd_parameters_count == 2)
    {
        confirm("Detection of failure");
        LOGD(L_OBD, L_V5, "AT^GT_CM=22# 请求未成功");
        if (G_parameter.humiture_sw == 1)
        {
            track_stop_timer(TRACK_CUST_EXTCHIP_WAIT22_TIMER_ID);
            track_cust_send22((void*)3);
        }
        return;
    }
    if(cmd_parameters_count == 4)
    {
        if(strcmp(cmd_parameters[1], "0") == 0)//温湿度主动上报
        {
            temp = atof(cmd_parameters[2]);
            humidity = atof(cmd_parameters[3]);
            temperature = atoi(cmd_parameters[2]);
            humidity1 = atoi(cmd_parameters[3]);
            confirm("22 0 OK!");
            if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
            {
                LOGS("humiture_ok,temp = %f ,humidity = %f",temp/10,humidity/10);
            }
            else
            {
                LOGD(L_APP,L_V5,"humiture_ok,temp = %f ,humidity = %f",temp/10,humidity/10);
            }
            track_temp_form_mcu(temp);
            track_humidity_form_mcu(humidity);
            sprintf(buf, "AT^GT_CM=22,0,%d,%d,OK#\r\n",temperature,humidity1);
            track_drv_write_exmode(buf, strlen(buf));
            LOGD(L_OBD, L_V5, "[%s]", buf);
            return;
        }
        else
        {
            confirm("Detection of failure");
            LOGD(L_OBD, L_V5, "AT^GT_CM=22# 请求未成功");
            if (G_parameter.humiture_sw == 1)
            {
                track_cust_send22((void*)3);
                track_stop_timer(TRACK_CUST_EXTCHIP_WAIT22_TIMER_ID);
                }
            return;
        }
    }
    if(cmd_parameters_count == 5)
    {
        if(strcmp(cmd_parameters[1], "0") == 0)//查询温湿度
        {
            temp = atof(cmd_parameters[2]);
            humidity = atof(cmd_parameters[3]);
            temperature = atoi(cmd_parameters[2]);
            humidity1 = atoi(cmd_parameters[3]);
            confirm("22 0 OK!");
            if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
            {
                LOGS("humiture_ok,temp = %f ,humidity = %f",temp/10,humidity/10);
            }
            else
            {
                LOGD(L_APP,L_V5,"humiture_ok,temp = %f ,humidity = %f",temp/10,humidity/10);
            }
            track_temp_form_mcu(temp);
            track_humidity_form_mcu(humidity);
            sprintf(buf, "AT^GT_CM=22,0,%d,%d,OK#\r\n",temperature,humidity1);
            track_drv_write_exmode(buf, strlen(buf));
            LOGD(L_OBD, L_V5, "[%s]", buf);
            if(track_cust_check_humiflag_flag(99) == 0)
            {
                track_cust_check_humiflag_flag(1);
                track_stop_timer(TRACK_CUST_EXTCHIP_WAIT22_TIMER_ID);
                track_start_timer(TRACK_CUST_EXTCHIP_WAIT22_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send22, (void *)1);
                return;
                }
            else if(track_cust_check_humiflag_flag(99) == 1)
            {
                if(track_cust_check_sethumiflag_flag(99) == 1)
                    {
                        track_cust_check_sethumiflag_flag(0);
                        track_stop_timer(TRACK_CUST_EXTCHIP_WAIT22_TIMER_ID);
                        track_start_timer(TRACK_CUST_EXTCHIP_WAIT22_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send22, (void *)1);
                        return;
                        }
                }
        }
        if(strcmp(cmd_parameters[1], "1") == 0)//设置温湿度阈值
        {
            temperature = atoi(cmd_parameters[2]);
            humidity1 = atoi(cmd_parameters[3]);
            LOGD(L_APP,L_V5,"humiture_ok,temp1 = %d ,humidity1 = %d",temperature,humidity1);
            if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
            {
                LOGD(L_APP,L_V5,"humiture_ok,temp = %f ,humidity = %f",((float)temperature)/10,((float)humidity1)/10);
            }
            else
            {
                LOGD(L_APP,L_V5,"humiture_ok,temp1 = %f ,humidity1 = %f",((float)temperature)/10,((float)humidity1)/10);
            }
            }
        else if(strcmp(cmd_parameters[1], "2") == 0)
        {
            temperature = atoi(cmd_parameters[2]);
            humidity1 = atoi(cmd_parameters[3]);
            LOGD(L_APP,L_V5,"humiture_ok,temp1 = %d ,humidity1 = %d",temperature,humidity1);
            if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
            {
                LOGD(L_APP,L_V5,"humiture_ok,temp = %f ,humidity = %f",((float)temperature)/10,((float)humidity1)/10);
            }
            else
            {
                LOGD(L_APP,L_V5,"humiture_ok,temp1 = %f ,humidity1 = %f",((float)temperature)/10,((float)humidity1)/10);
            }
            }
        else if(strcmp(cmd_parameters[1], "3") == 0)
        {
            value = atoi(cmd_parameters[2]);
            LOGD(L_APP,L_V5,"humiture_sw = %d",value);
            if(value == 1)
            {
                track_stop_timer(TRACK_CUST_EXTCHIP_WAIT22_TIMER_ID);
                track_start_timer(TRACK_CUST_EXTCHIP_WAIT22_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_send22, (void *)0);
                return;
                }
            }
    }
    track_stop_timer(TRACK_CUST_EXTCHIP_WAIT22_TIMER_ID);
}

#if 0
static void cmd_23()
{
    U8 buf[20] = {0};
    float volt = 0;
    volt = atof(cmd_parameters[1]);
    confirm("23 OK!");
    LOGD(L_OBD, L_V1, "volt = (%f)",volt);
    track_stop_timer(TRACK_CUST_EXTCHIP_WAIT23_TIMER_ID);
}


static void cmd_24()
{
    confirm("24 OK!");
    track_stop_timer(TRACK_CUST_EXTCHIP_WAIT24_TIMER_ID);
}
static void cmd_25()
{
    U8 buf[20] = {0};
    if (cmd_parameters_count == 3)
    {
        confirm("25 OK!");
        track_stop_timer(TRACK_CUST_EXTCHIP_WAIT25_TIMER_ID);
        return;
    }
    sprintf(buf, "AT^GT_CM=25,OK#\r\n");
    track_drv_write_exmode(buf, strlen(buf));
    LOGD(L_OBD, L_V1, "HIGHT_OK,检测到光敏");
    wakeup_mode = wk_rem_alm;
    //track_cust_ext_teardown_alm();
}
#endif
/*static void cmd_26()
{
	U8 buf[20] = {0};
	if(cmd_parameters_count == 4)
    {
    	confirm("26 OK!");
    	track_stop_timer(TRACK_CUST_EXTCHIP_WAIT26_TIMER_ID);
		return;
    }
  	sprintf(buf, "AT^GT_CM=26,OK#\r\n");
    track_drv_write_exmode(buf, strlen(buf));
	LOGD(L_OBD, L_V1, "检测到碰撞");
 	wakeup_mode = wk_collision_alm;
   	if((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        return;
    }
	track_cust_ext_collision_alm();
}*/
static void cmd_27()
{
#if 0
    U8 buf[20] = {0};
    if (cmd_parameters_count == 4)
    {
        LOGD(L_OBD, L_V1, "27 OK!");
        //confirm("27 OK!");
        track_stop_timer(TRACK_CUST_EXTCHIP_WAIT27_TIMER_ID);
        return;
    }
    sprintf(buf, "AT^GT_CM=27,OK#\r\n");
    track_drv_write_exmode(buf, strlen(buf));
    LOGD(L_OBD, L_V1, "检测到倾斜");
    wakeup_mode = wk_tilt_alm;
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        return;
    }
    track_cust_ext_ilcline_alm();
#endif
}

static cmd_28()
{
    kal_uint16 num1;
    num1 = atoi(cmd_parameters[1]);
    stsn = num1;
    LOGD(L_OBD, L_V1, "28 OK!");
    tr_stop_timer(TRACK_CUST_EXTCHIP_WAIT28_TIMER_ID);
}

void track_cust_synchronous_data_to_mcu(void)
{
    tr_start_timer(TRACK_CUST_EXTCHIP_WAIT08_TIMER_ID, 1000, track_cust_module_Handshake);
    track_start_timer(TRACK_CUST_EXTCHIP_WAIT06_TIMER_ID, 3000, track_cust_send06, (void *)G_parameter.teardown_str.sw);
    tr_start_timer(TRACK_CUST_EXTCHIP_WAIT02_TIMER_ID, 4000, track_cust_send02);
    track_start_timer(TRACK_CUST_EXTCHIP_WAIT04_TIMER_ID, 5000, track_cust_send04, (void *)G_parameter.extchip.mode);
    track_start_timer(TRACK_CUST_EXTCHIP_WAIT21_CHECKSTATUS_TIMER_ID,6000,track_cust_send21,(void *)5);//查询静止运动状态
    track_start_timer(TRACK_CUST_EXTCHIP_WAIT21_SENDSTATIC_TIMER_ID,7000,track_cust_send21,(void *)6);
    track_start_timer(TRACK_CUST_EXTCHIP_WAIT21_SENDSPORT_TIMER_ID,8000,track_cust_send21,(void *)7);
    track_cust_check_humiflag_flag(0);
    track_start_timer(TRACK_CUST_EXTCHIP_WAIT22_TIMER_ID, 9000, track_cust_send22, (void *)3);
}

static cmd_29()
{
    char buf_ams[100] = {0};
    applib_time_struct currTime_ams = {0};
    U8 buf[20] = {0};
    sprintf(buf, "AT^GT_CM=29,OK#\r\n");
    track_drv_write_exmode(buf, strlen(buf));
    LOGD(L_OBD, L_V1, "29 OK!");
    track_cust_synchronous_data_to_mcu();
    #if defined(__GT420D__)
    track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
    snprintf(buf_ams, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d 29\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec);
                LOGD(5,5,"len:%d",strlen(buf_ams));
        track_cust_save_logfile(buf_ams, strlen(buf_ams));
    #endif
    //confirm("29 OK!");
}

static void cmd_90()
{
    U8 par = atoi(cmd_parameters[1]);
    LOGD(L_OBD, L_V1, "%d",par);
    if (par!=G_parameter.teardown_str.sw)
    {
        LOGD(L_OBD, L_V1, "ST单片机防拆开关不符!");
        track_cust_notify_st_remove_alarm_modify(G_parameter.teardown_str.sw);
    }
}

static void cmd_98(void)
{
    U8 par = atoi(cmd_parameters[1]);

    if (par==1)
    {
        track_cust_refer_lightsensor_data(0);
        confirm("HIGH LEVEL");
    }
    else
    {
        track_cust_refer_lightsensor_data(1);
        confirm("LOW LEVEL");
    }
    track_stop_timer(TRACK_CUST_EXTCHIP_WAIT98_TIMER_ID);
}

static void cmd_99(void)
{
#if defined (__MCU_SW_UPDATE__)
    track_drv_mcu_sw_update_rsp();
#endif
    LOGD(L_OBD, L_V5, "");
    confirm("99 OK!");
}
static cust_recv_struct CMD_Data[] =
{
    {"01",        cmd_01},
    {"02",        cmd_02},
    {"03",        cmd_03},
    {"04",        cmd_04},
    {"05",        cmd_05},
    {"06",        cmd_06},
    {"07",        cmd_07},
    {"08",        cmd_08},
    {"08 OK",     cmd_08},
    {"09 OK",     cmd_09},
    {"09",        cmd_09},
    {"10",        cmd_10},
    {"11",        cmd_11},
    {"12",        cmd_12},
    {"13",        cmd_13},
    {"17",		  cmd_17},
    {"18",		  cmd_18},
    {"21",		cmd_21},
    {"22",		cmd_22},
    {"27",      	cmd_27},
    {"28",      	cmd_28},
    {"29",      	cmd_29},
    {"90",        cmd_90},
    {"98",        cmd_98},
    {"99",        cmd_99}
};

//清除定位包协议
void track_cust_module_resetpro()
{
    G_realtime_data.TPRO_COUNT = 0;
    G_parameter.new_position_sw = 0;
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    LOGD(L_OBD, L_V1, "重新设置定位包协议!");
}
void track_cust_modeule_Supplements_data()
{
    LOGD(L_OBD, L_V2, "had supplements data, notify st!");
    track_cust_send13((void*)4);
}
kal_uint8 track_cust_modeule_get_wakemode()
{
    return wakeup_mode;
}

kal_uint8 track_cust_modeule_get_rst()
{
    return st_rst;
}

kal_uint32 track_cust_modeule_get_st_ele()
{
    if (single_ele_value==0 && first_10_value==0)
    {
        return 1;
    }
    return single_ele_value;
}

void track_set_ele_num(U8 para)
{
    track_work_time tmp_struct={0};
    U8 par = 100-para;
    tmp_struct.comsultion_ele=par/100.0*InitElet;
    memcpy(&G_realtime_data.cal_wk_time_struct,&tmp_struct,sizeof(track_work_time));
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    LOGD(L_OBD, L_V2, "电量消耗为%0.f",G_realtime_data.cal_wk_time_struct.comsultion_ele);
    gpsworksec = 0;
    gpssleepsec = 0;
    single_ele_value = 0;
    //track_cust_send10(1, G_realtime_data.cal_wk_time_struct.comsultion_ele);
    //track_cust_module_cal_electricity((void*)4);
}

kal_bool track_cust_is_innormal_wakeup()
{
    if (wakeup_mode==wk_ams_link || wakeup_mode==wk_supplements)
    {
        return KAL_TRUE;
    }
    else
    {
        return KAL_FALSE;
    }
}
void track_cust_module_power_cmd(cmd_return_struct *return_sms, kal_uint8 *data, int len)
{
    memcpy(&g_return_sms, return_sms, sizeof(cmd_return_struct));
    g_return_sms_valid = 1;
    track_drv_write_exmode(data, len);
}
U8 track_cust_syn_st_time(kal_uint32 arg, kal_uint8 app)
{
    static U8 tapp=0;
    U32 timep = 0;
    applib_time_struct time = {0};
    if (app==99)
    {
        return tapp;
    }
    else
    {
        tapp=app;
    }
    track_fun_get_time(&time, KAL_FALSE, &G_parameter.zone);
    LOGD(L_OBD, L_V5, "%d", app);
    LOGD(L_OBD, L_V8, "time: %0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
         time.nYear,
         time.nMonth,
         time.nDay,
         time.nHour,
         time.nMin,
         time.nSec);
    if (time.nYear > (2000 + (*OTA_DEFAULT_HARDWARE_YEAR)))
    {
        U8 buf[100] = {0}, valid = 0,real=0;
        static applib_time_struct tmp = {0};
        OTA_applib_dt_get_rtc_time(&tmp);
        timep = OTA_applib_dt_mytime_2_utc_sec(&tmp, 0);
        if (app != 4)
        {
            track_cust_send02();
        }
        if (G_realtime_data.trackmode_end_sec==0)
        {
            G_realtime_data.trackmode_end_sec = timep+G_parameter.teardown_str.alarm_upcount*G_parameter.teardown_str.alarm_vel_min*60+300;
            real=1;
            LOGD(L_OBD, L_V5, "sec%d ,temp %d",G_realtime_data.trackmode_end_sec,timep);
        }

        if (G_realtime_data.tracking_mode)
        {
            if (timep>=G_realtime_data.trackmode_end_sec)
            {
                LOGD(L_OBD, L_V5, "%d 防拆模式超时结束%d!!!",G_realtime_data.trackmode_end_sec,timep);
                G_realtime_data.tracking_mode=0;
                G_realtime_data.trackmode_end_sec=0;
                G_realtime_data.tracking_count=0;
                real=1;
            }
            else
            {
                //track_cust_openup_trackmode_lbsalm(2);
                LOGD(L_OBD, L_V5, "%d,%d",G_realtime_data.trackmode_end_sec,timep);
            }
        }
        if (!g_02_ok)
        {
            g_02_ok = KAL_TRUE;
        }
        if ((G_parameter.extchip.mode == 1 ||G_parameter.extchip.mode1_en) && G_parameter.extchip.mode1_time < DEFAULT_FEB_TIMESEC && G_parameter.extchip.mode1_time + arg >= DEFAULT_FEB_TIMESEC)
        {
            G_parameter.extchip.mode1_time += arg;
            valid = 1;
        }
        if (G_parameter.extchip.mode2_par1 == 0)
        {
            G_parameter.extchip.mode2_par1 = time.nHour * 60 + time.nMin;
            if (G_parameter.extchip.mode == 2) track_cust_module_update_mode();//
            valid = 1;
        }
        if (valid) Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        if (real)
        {
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

        }
        track_cust_module_auto_sw_mode();
    }
    return 0;
}

void track_cust_module_Handshake(void)
{
    char buf[30] = {0};
    sprintf(buf, "AT^GT_CM=08#\r\n");
    track_drv_write_exmode(buf, strlen(buf));
    LOGD(L_OBD, L_V5, "[%s]", buf);
    tr_start_timer(TRACK_CUST_EXTCHIP_WAIT08_TIMER_ID, RESEND_COMMAND_TIMEOUT_SEC, track_cust_module_Handshake);
}

/******************************************************************************
 *  Function    -  track_cust_module_clearup
 *
 *  Purpose     -  清除电池电量计算相关变量
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 2016-05-04 xzq
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_clearup()
{
    gpsworksec = 0;
    gpssleepsec = 0;
    gsm_ele = 100;
    single_ele_value = 0;
}

void track_cust_get_ele_by_st()
{
    //track_cust_send10(0, 0);
}

void track_cust_update_ele_to_st()
{
    if (G_realtime_data.cal_wk_time_struct.comsultion_ele < single_ele_value)
    {

        if (G_realtime_data.ams_log)
        {
            char sttime_str[100] = {0};
            snprintf(sttime_str, 99, "(V)(%d)ERROR ELE GSMELE %.0f STELE %d!!!", RUNING_TIME_SEC,G_realtime_data.cal_wk_time_struct.comsultion_ele,single_ele_value);
            //track_cust_save_logfile(sttime_str, strlen(sttime_str));
            G_realtime_data.ams_push=1;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

        }
        LOGD(L_OBD, L_V5, "实时电量低于单片机保存的数据GSM %.0f, %d",G_realtime_data.cal_wk_time_struct.comsultion_ele, single_ele_value);
        G_realtime_data.cal_wk_time_struct.comsultion_ele = single_ele_value;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    }
    //track_cust_send10(1, G_realtime_data.cal_wk_time_struct.comsultion_ele);
    if (G_parameter.extchip.mode==1)
    {
        track_cust_get_ele_by_st();
    }
}

void track_cust_batptc_data(char *data,int par)
{
    if (par==-1)
    {
        snprintf(data, 90, "ELE:%0.f, %d\%",InitElet-G_realtime_data.cal_wk_time_struct.comsultion_ele,gsm_ele);
    }
    else
    {
        track_set_ele_num(par);
        snprintf(data, 90, "OK!");
    }
}
kal_uint32 track_cust_module_get_ele_data()
{
    return InitElet-G_realtime_data.cal_wk_time_struct.comsultion_ele;
}
void track_cust_resetbat(U8 par)
{
    if (G_parameter.cal_time_sw)
    {
        track_work_time tmp_struct = {0};
        RESETBAT = 1;
        tmp_struct.comsultion_ele = 0;
        memcpy(&G_realtime_data.cal_wk_time_struct, &tmp_struct, sizeof(track_work_time));
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        LOGD(L_OBD, L_V2, "电量重置");
        track_cust_module_clearup();
        track_cust_set_battery_volt(293);
        //track_cust_send10(1, 0);
        heartbeat_packets((void*)110);

        if (G_realtime_data.ams_log)
        {
            char sttime_str[100] = {0};
            snprintf(sttime_str, 99, "(U)(%d)RESET BAT %d!!!", RUNING_TIME_SEC,par);
            //track_cust_save_logfile(sttime_str, strlen(sttime_str));
            G_realtime_data.ams_push=1;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

        }
    }
}

void track_cust_module_gps_sleep_time(void)
{
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE) || G_parameter.extchip.mode != 1)
    {
        return;
    }
    if (G_parameter.cal_time_sw)
    {
        gpssleeptick = kal_get_systicks();
        LOGD(L_OBD, L_V1, "GPS 休眠开始计算或重置时间 %d", gpssleeptick);
        if (G_realtime_data.netLogControl & 32)
        {
            char buf[50];
            sprintf(buf, "GPS sleep time boot or reset %d", gpssleeptick);
            cust_packet_log_data(8, (kal_uint8 *)buf, strlen((char *)buf));
        }
    }
}

void track_cust_module_gps_cal_sleep_time(void)
{
    kal_uint32 gsmtick;
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        return;
    }
    if (G_parameter.extchip.mode != 1 || G_parameter.cal_time_sw == 0)
    {
        return;
    }
    gpsclosesleeptick = kal_get_systicks();
    gsmtick = gpsclosesleeptick - gpssleeptick;
    gsmtick = gsmtick / KAL_TICKS_1_SEC;
    gpssleepsec += gsmtick;
    LOGD(L_OBD, L_V7, "%d , %d", gpssleeptick, gpsclosesleeptick);
    LOGD(L_OBD, L_V1, "GPS 本次关闭时间%d 秒,  累计关闭时间为 %d 秒",  gsmtick, gpssleepsec);

}
void track_cust_module_gps_boot_time(void)
{
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE) || G_parameter.extchip.mode != 1)
    {
        return;
    }
    if (G_parameter.cal_time_sw)
    {
        gpsboottick = kal_get_systicks();
        LOGD(L_OBD, L_V1, "GPS开启计算或重置时间 %d", gpsboottick);
    }
}

void track_cust_module_gps_cal_time(void)
{
    kal_uint32 gsmtick;
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        return;
    }
    if (G_parameter.extchip.mode != 1 || G_parameter.cal_time_sw == 0)
    {
        return;
    }
    gpsclosetick = kal_get_systicks();
    gsmtick = gpsclosetick - gpsboottick;
    gsmtick = gsmtick / KAL_TICKS_1_SEC;
    gpsworksec += gsmtick;
    LOGD(L_OBD, L_V7, "%d , %d", gpsboottick, gpsclosetick);
    LOGD(L_OBD, L_V1, "GPS本次工作时间 %d 秒, 累计工作时间%d 秒",  gsmtick, gpsworksec);
}

// 10秒防拆报警delay
void track_cust_module_boot_cal()
{
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        return;
    }
    if (G_parameter.extchip.mode != 1 && mmi_Get_PowerOn_Type() != ABNRESET)
    {
        if (track_cust_gps_need_open())
        {
            G_realtime_data.cal_wk_time_struct.comsultion_ele += 6500.0;
            LOGD(L_OBD,L_V1,"省电模式开机增加一次消耗电量(6500), 总电量%.0f,目前消耗电量为%.0f",InitElet,G_realtime_data.cal_wk_time_struct.comsultion_ele);
        }
        else
        {
            if (G_realtime_data.netLogControl & 4096)
            {
                U8 buf[20]={0};
                sprintf(buf,"pre %.0f", G_realtime_data.cal_wk_time_struct.comsultion_ele);
                cust_packet_log_data(8, buf, 20);
            }
            LOGD(L_OBD,L_V1,"%d",G_realtime_data.cal_wk_time_struct.comsultion_ele);
            G_realtime_data.cal_wk_time_struct.comsultion_ele += 4320.0;
            if (G_realtime_data.netLogControl & 4096)
            {
                U8 buf[30]={0};
                sprintf(buf,"after %.0f, st %d ", G_realtime_data.cal_wk_time_struct.comsultion_ele,track_cust_modeule_get_st_ele());
                cust_packet_log_data(8, buf, 30);
            }
            LOGD(L_OBD,L_V1,"省电模式开机增加一次消耗电量(4320), 总电量%.0f,目前消耗电量为%.0f",InitElet,G_realtime_data.cal_wk_time_struct.comsultion_ele);
        }
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    }
}


/******************************************************************************
 *  Function    -  track_cust_rem_tracking_time
 *
 *  Purpose     - 计算模式一运行时间,重启有效
 *
 *  Description -return 运行时间
 *
 *  parameter 1 开始关闭计时 parameter 2  标记计时开关
 * ----------------------------------------
 * v1.0  , 2016-05-04 xzq
 * ----------------------------------------
*******************************************************************************/
U32 track_cust_rem_tracking_time(U8 reset,U8* op)
{
    static U32 tick=0,cur_tick=0;
    static kal_bool strart=KAL_FALSE;
    U32 result=0;
    LOGD(L_OBD, L_V7, "%d ,%d,%d", reset,tick,strart);
    if (reset==1 && !strart)//开启计时
    {
        tick = kal_get_systicks();
        cur_tick=0;
        strart = KAL_TRUE;
    }
    else if (reset==1 && strart)//重新定时
    {
        tick = kal_get_systicks();
        cur_tick=0;
    }
    else if (reset==2)//结束
    {
        strart = KAL_FALSE;
    }
    else if (strart && reset==0)//取运行时间
    {
        cur_tick = kal_get_systicks();
        result = (cur_tick  - tick)/KAL_TICKS_1_SEC;
        tick = cur_tick;
    }
    if (op!=NULL)
    {
        *op = strart;
    }
    return result;
}

void track_cust_module_cal_ele_condition(U8 par)
{
    static kal_bool cmd10=KAL_FALSE;
    static kal_bool cmd01=KAL_FALSE;
    if (par == 1)
    {
        cmd10 = KAL_TRUE;
    }
    else if (par==2)
    {
        cmd01 = KAL_TRUE;
    }
    if (cmd10)
    {
        //track_cust_module_cal_electricity((void*)3);
        cmd10=KAL_FALSE;
        cmd01=KAL_FALSE;
    }

    LOGD(L_OBD, L_V1, "par %d", par);
}
#if defined(__NETWORK_LICENSE__)
void track_cust_into_network_licenes_mode()
{
    tr_start_timer(TRACK_CUST_POWERON_LED_TIME_ID, 3*60*1000, track_cust_into_network_licenes_mode);
}
#endif
void track_cust_module_cal_electricity(void* par)
{
    kal_uint32 tracking_pwrcons = 0, volt,tracking_time=0;
    float ele_per = 0,comsult_ele=0;
    static U8 fir = 0;
    kal_bool tracking_en=KAL_FALSE;
    static U32 tick=0;
    U8 op = (U8)par;

    tracking_time = track_cust_rem_tracking_time(0,&tracking_en);
    if (tracking_en==KAL_TRUE)
    {
        U8 tmp = (tracking_time%60)>30?1:0;
        G_realtime_data.remove_time += tracking_time/60+tmp;
        LOGD(L_OBD, L_V1, "tracking time %d", G_realtime_data.remove_time);
    }

    if (G_parameter.cal_time_sw)
    {
        if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
        {
            return;
        }

        if (fir == 0)
        {
            fir = 1;
            if (RESETBAT == 0 && G_realtime_data.cal_wk_time_struct.comsultion_ele == 0)
            {
                if (single_ele_value != 0 && single_ele_value != 1)
                {
                    G_realtime_data.cal_wk_time_struct.comsultion_ele = single_ele_value;
                    LOGD(L_OBD, L_V1, "GSM 数据丢失, 从单片机获得消耗电量 %d", single_ele_value);

                    if (G_realtime_data.ams_log)
                    {
                        char sttime_str[100] = {0};
                        snprintf(sttime_str, 99, "(S)(%d)ELE DATA LOST,GET BY ST %d!!!", RUNING_TIME_SEC,single_ele_value);
                        //track_cust_save_logfile(sttime_str, strlen(sttime_str));
                        G_realtime_data.ams_push=1;
                        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

                    }
                }
                else
                {
                    if (G_realtime_data.ams_log)
                    {
                        char sttime_str[100] = {0};
                        snprintf(sttime_str, 99, "(S)(%d)GSM AND ST HAD NO ELE DATA!!!", RUNING_TIME_SEC);
                        //track_cust_save_logfile(sttime_str, strlen(sttime_str));
                        G_realtime_data.ams_push=1;
                        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

                    }

                }
            }
        }
        {
            if (track_cust_get_battery_volt() == 0)
            {
                tracking_pwrcons = 0;
            }
            else if (track_cust_gps_status() == 0)
            {
                tracking_pwrcons = gpsworksec * mode1_gps_pwrcons;
                track_cust_module_gps_cal_sleep_time();//  计算休眠时间
                tracking_pwrcons += gpssleepsec * mode1_gps_sleeppwrcons;
#if defined(__GT420D__)
                tracking_pwrcons += G_realtime_data.cal_wk_time_struct.awakentimes * mode2_pwrconsumption;
#endif
                track_cust_module_gps_sleep_time();
                G_realtime_data.cal_wk_time_struct.awakentimes=0;
            }
            else
            {
                //tracking_pwrcons = G_realtime_data.cal_wk_time_struct.gps_sleep_time * mode1_gps_sleeppwrcons;
                tracking_pwrcons = gpssleepsec * mode1_gps_sleeppwrcons;
                track_cust_module_gps_cal_time();// 计算工作时间
                tracking_pwrcons += gpsworksec * mode1_gps_pwrcons;
                track_cust_module_gps_boot_time();
            }
            G_realtime_data.cal_wk_time_struct.comsultion_ele += (float)tracking_pwrcons;
            if (G_realtime_data.cal_wk_time_struct.comsultion_ele<single_ele_value)
            {
                LOGD(L_OBD, L_V1, "电量计算异常,实时电量小于存储ST里电量,恢复ST存储电量!!!!");
                if (G_realtime_data.ams_log)
                {
                    char sttime_str[100] = {0};
                    snprintf(sttime_str, 99, "(S)(%d)ELE CAL DATA ERROR par:%d, GSM %d, ST %d!!!", RUNING_TIME_SEC,par,G_realtime_data.cal_wk_time_struct.comsultion_ele,single_ele_value);
                    //track_cust_save_logfile(sttime_str, strlen(sttime_str));
                    G_realtime_data.ams_push=1;
                    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

                }
                G_realtime_data.cal_wk_time_struct.comsultion_ele = single_ele_value;
            }
            if (G_realtime_data.cal_wk_time_struct.comsultion_ele < InitElet)
            {
                gsm_ele = (U8)(((InitElet - G_realtime_data.cal_wk_time_struct.comsultion_ele) / InitElet) * 100.0);

            }
            else
            {
                G_realtime_data.cal_wk_time_struct.comsultion_ele = InitElet;
                gsm_ele = 0;
            }

            LOGD(L_OBD, L_V1, "本次GPS开启%d 秒,关闭%d 秒.",gpsworksec ,gpssleepsec);
            if (op==0 || op==3)
            {
                LOGD(L_OBD, L_V1, "开机首次计算消耗电量为%d", tracking_pwrcons);
            }
            else if (op==1)
            {
                LOGD(L_OBD, L_V1, "5分钟计算电量间隔到,本次消耗电量为%d", tracking_pwrcons);
            }
            else if (op==2)
            {
                LOGD(L_OBD, L_V1, "切换模式计算本次消耗电量为%d", tracking_pwrcons);
            }
            else
            {
                LOGD(L_OBD, L_V1, "主动计算一次电量", tracking_pwrcons);
            }
            LOGD(L_OBD, L_V1, "目前消耗总量 %.0f, 剩余电量百分比 %d", G_realtime_data.cal_wk_time_struct.comsultion_ele,  gsm_ele);
            gpsworksec = 0;
            gpssleepsec = 0;

            if (gsm_ele>=20)
            {
                G_realtime_data.lowBatteryFlag=0;
            }

            volt = track_cust_conversion_volt(gsm_ele);
            track_cust_set_battery_volt(volt / 10000);

            //track_start_timer(TRACK_CUST_CALCULATE_ELE_TIMER_ID, CAL_ELETRCITY * 1000, track_cust_module_cal_electricity,(void*)1);
        }
        if (tick==0)
        {
            tick =kal_get_systicks();
        }
        else
        {
            U32 tmp = kal_get_systicks();
            if ((tmp  - tick)/KAL_TICKS_1_SEC >3600)
            {
                LOGD(L_OBD, L_V1, "常开大于1小时，存储更新电量到单片机!!!");
                tick=kal_get_systicks();
                track_cust_update_ele_to_st();
            }
        }
    }
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

}
void track_cust_module_status_inquiry(void)
{
    char buf[100] = {0};
    sprintf(buf, "AT^GT_CM=01#\r\n");
    track_drv_write_exmode(buf, strlen(buf));
    LOGD(L_OBD, L_V1, "");
    tr_start_timer(TRACK_CUST_EXTCHIP_BATTERY_VOLTAGE_TIMER_ID, 2000, track_cust_module_status_inquiry);
    LOGD(L_OBD, L_V5, "[%s]", buf);
    //tr_start_timer(TRACK_CUST_EXTCHIP_BATTERY_VOLTAGE_TIMER_ID, 60000, track_cust_module_status_inquiry);
}
void track_cust_query_st_teardown_status()
{
    LOGD(L_OBD, L_V1, "");
    track_cust_send90();
}

void track_cust_module_extchip_shutdown(void)//740
{
#if defined(__NETWORK_LICENSE__)
    return;
#endif
#if !defined(__GT420D__)
    if (G_parameter.extchip.mode != 1)
#endif
    {
#if !defined(__GT420D__)
        char buf[100] = {0};
        if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
        {
            confirm("测试模式下不发03");
            return;
        }
        sprintf(buf, "AT^GT_CM=03,10#\r\n");
        track_drv_write_exmode(buf, strlen(buf));
        LOGD(L_OBD, L_V5, "下发 03指令");
        tr_start_timer(TRACK_POWER_RESET_TIMER_ID, 2000, track_cust_module_extchip_shutdown);
#else
    if (G_parameter.extchip.mode == 2)
        {
            LOGD(L_APP,L_V5,"长连接模式不关机");
            return;
            }
    if(track_cust_check_beforepoweroff_flag(99) == 1 && MQTT_get_gprs_status())
        {
            track_cust_sendlbs_limit();
            }
            track_cust_send03((void *)10);
#endif
    }
}

void track_cust_delay_timeout_close()
{
    char ams_buf[100] = {0};
    applib_time_struct currTime = {0};
    if(track_cust_refer_keyin_data(99) == 1 || track_cust_needsend_charge_data(99) == 1 || track_cust_refer_motion_status_data(99) == 1 \
                    || track_cust_check_simalm_flag(99) == 1 ||track_cust_check_downalm_flag(98) == 3 \
                    || track_cust_check_teardown_flag(98) == 3 || track_cust_check_batteryalm_flag(99) == 1)
    {
        LOGD(L_APP,L_V5,"还有操作不能关机，延时30秒关机");
        tr_start_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID, 20*1000, track_cust_delay_timeout_close);
        return;
            }
    if (G_parameter.extchip.mode == 2)
    {
        LOGD(L_APP,L_V5,"长连接模式不关机");
        return;
    }
    LOGD(L_OBD, L_V5, "延时关机时间到!");
    LOGS("\r\n\r\n!! Delay time out!\r\n!! Shutdown...\r\n");
    if (G_parameter.extchip.mode == 1)
    {
        track_cust_send04((void *)G_parameter.extchip.mode);
        #if defined(__GT420D__)
        track_fun_get_time(&currTime, KAL_FALSE, &G_parameter.zone);
        snprintf(ams_buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d (%d)power off\n\r",
                            currTime.nYear,
                            currTime.nMonth,
                            currTime.nDay,
                            currTime.nHour,
                            currTime.nMin,
                            currTime.nSec,
                            track_cust_gt420d_carstatus_data(99));
        LOGD(5,5,"len:%d",strlen(ams_buf));
                track_cust_save_logfile(ams_buf, strlen(ams_buf));
                #endif
        }
    if (track_soc_position_queue() != 0)
    {
        LOGD(L_OBD, L_V5, "1");
        track_soc_bakup_data((void*)3);
        tr_start_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID, 5000, track_cust_module_extchip_shutdown);
    }
    else
    {
        LOGD(L_OBD, L_V5, "2");
        track_cust_module_extchip_shutdown();
    }
}

void track_cust_module_delay_close(U8 sec)//740
{
    kal_uint32 dla = sec * 1000;
    LOGD(L_OBD, L_V5, "延时(%d)秒关机!", sec);

    LOGD(L_OBD,L_V5,"%d还有报警未处理不关机",track_cust_module_alarm_lock(98, NULL));
    if (sec==10) //未联网的情况
    {
        track_cust_netlog_cache(0);//日志保存文件
    }
    if (dla == 0)dla = 1000;
#if defined(__GT420D__)
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        return;
    }
    
    if (G_parameter.extchip.mode == 2)
    {
        LOGD(L_APP,L_V5,"长连接模式不关机");
        return;
    }
#endif
    tr_start_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID, dla, track_cust_delay_timeout_close);
}


void track_cust_module_update_mode(void)
{
    applib_time_struct currTime_ams = {0};
    U8 buf[100] = {0};
    track_gps_data_struct *gd = NULL;
    kal_uint8 tmp = 0;
    kal_uint16 sn = 0;
    track_gps_data_struct *p_gpsdata;
#if defined(__NETWORK_LICENSE__)
    return;
#endif
#ifdef __GT420D__

    LOGD(L_OBD, L_V5, "mode:%d,mode1:%d,mode2:%d",G_parameter.extchip.mode,G_parameter.extchip.mode1_par1,G_parameter.extchip.mode2_par1);
    if (G_parameter.extchip.mode == 3)
    {
        LOGD(L_APP,L_V5,"当前工作模式三，不进行定位数据工作");
        return;
    }
    if (G_parameter.extchip.mode == 2)
    {
        LOGD(L_APP,L_V5,"模式%d",G_parameter.extchip.mode);
        tmp = track_cust_gps_status();
        if(track_cust_gt420d_carstatus_data(99) == 2)
        {
            if(track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 && track_cust_refer_motion_status_data(99) == 0 \
                    && track_cust_check_simalm_flag(99) == 0 &&track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0)
            {
            if(G_realtime_data.last_location_status == 1)
                {
                    if(track_cust_check_gpsflag_status(99) == 1)
                    {
                        gd = track_cust_backup_gps_data(2, NULL);
                        track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,5,9);
                        track_cust_check_beforepoweroff_flag(0);
                        if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
                        {
                            track_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
                            }
                        {
                            LOGD(L_APP,L_V5,"%d分钟后上传数据",G_parameter.statictime.mode2_statictime);
                            tr_start_timer(TRACK_CUST_MODE2_KEEPGOING_TIMER_ID,G_parameter.statictime.mode2_statictime * 60 *1000,track_cust_module_update_mode);
                            }
                        return;
                        }
                    else
                    {
                        track_cust_sendlbs_limit();
                        if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
                        {
                            track_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
                            }
                        {
                            LOGD(L_APP,L_V5,"%d分钟后上传数据",G_parameter.statictime.mode2_statictime);
                            tr_start_timer(TRACK_CUST_MODE2_KEEPGOING_TIMER_ID,G_parameter.statictime.mode2_statictime * 60 *1000,track_cust_module_update_mode);
                            }
                            return;
                        }
            }
            else
            {
                track_cust_sendlbs_limit();
                if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
                    {
                        track_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
                        }
                {
                    LOGD(L_APP,L_V5,"%d分钟后上传数据",G_parameter.statictime.mode2_statictime);
                    tr_start_timer(TRACK_CUST_MODE2_KEEPGOING_TIMER_ID,G_parameter.statictime.mode2_statictime * 60 *1000,track_cust_module_update_mode);
                        }
                    return;
                }
            }
                }
        if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
        {
            track_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
        }
        if(track_cust_check_gpsflag_status(99) == 1)
        {
            if (tmp > GPS_STAUS_OFF)
            {
                if (tmp >= GPS_STAUS_2D)
                {
                    if(track_cust_check_gpdfilter_flag(99) == 0)
                    {
                        LOGD(L_APP,L_V5,"GPS需要被过滤,上报上一个GPS点");
                        p_gpsdata = track_cust_backup_gps_data(2, NULL);
                        if(p_gpsdata->gprmc.Latitude == 0 && p_gpsdata->gprmc.Longitude == 0)
                        {
                            p_gpsdata = track_cust_backup_gps_data(0, NULL);
                            #if defined(__GT420D__)
                            track_cust_backup_gps_data(3, p_gpsdata);
                            LOGD(L_APP,L_V7,"没有上一个有效点，更新有效点");
                            #endif
                            }
                        track_cust_dsc_location_paket(p_gpsdata,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,9);
                        }
                    else
                    {
                        track_cust_check_gpdfilter_flag(0);
                        LOGD(L_APP,L_V5,"GPS定位着，直接上传");
                        p_gpsdata = track_cust_backup_gps_data(0, NULL);
                        #if defined(__GT420D__)
                        track_cust_backup_gps_data(3, p_gpsdata);
                        LOGD(L_APP,L_V7,"更新有效点");
                        #endif
                        track_cust_dsc_location_paket(p_gpsdata,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,9);
                        }
                    track_cust_check_beforepoweroff_flag(0);
                    }
                else
                {
                    LOGD(L_APP,L_V5,"GPS定位未定位，开启WIFI");
                    track_cust_module_overtime_upwifi();
                    }
            }
            else
            {
                track_cust_gps_work_req((void *)13);
                track_cust_module_overtime_upwifi();
                }
        }
        else
        {
            if(G_parameter.wifi.sw == 1)
                {
                    LOGD(L_APP,L_V5,"GPS开关未开，开启WIFI");
                    track_cust_module_overtime_upwifi();
                }
            else
                {
                    LOGD(L_APP,L_V5,"GPS.WIFI开关未开,上报LBS");
                    track_cust_sendlbs_limit();
                    }
            }
        if(track_cust_gt420d_carstatus_data(99) == 1)
        {
            LOGD(L_APP,L_V5,"%d秒后上传数据",G_parameter.extchip.mode2_par1);
            tr_start_timer(TRACK_CUST_MODE2_KEEPGOING_TIMER_ID,G_parameter.extchip.mode2_par1 * 1000,track_cust_module_update_mode);
        }
        else
            {
                LOGD(L_APP,L_V5,"%d分钟后上传数据",G_parameter.statictime.mode2_statictime * 60);
                tr_start_timer(TRACK_CUST_MODE2_KEEPGOING_TIMER_ID,G_parameter.statictime.mode2_statictime * 60 *1000,track_cust_module_update_mode);
            }
    }
    if (G_parameter.extchip.mode == 1)
    {
        if(track_cust_gt420d_carstatus_data(99) == 2)
        {
            track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
            if (currTime_ams.nYear == 2015 && currTime_ams.nMonth == 1 && currTime_ams.nDay == 1)
                {
                    LOGD(L_GPS, L_V5, "Warning: current time is not Synchronous! ");
                    tr_start_timer(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID, 5000, track_cust_module_update_mode);
                    return;
                    }
            if(track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 && track_cust_refer_motion_status_data(99) == 0 \
                    && track_cust_check_simalm_flag(99) == 0 && track_cust_check_firstpoweron_flag(99) == 1 && track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0 && track_cust_need_check_sportorstatic(99) == 0)
            {
            if(G_realtime_data.last_location_status ==1)
                {
                    if(track_cust_check_gpsflag_status(99) == 1)
                    {
                    gd = track_cust_backup_gps_data(2, NULL);
                    gd->gprmc.status = 0;
                    track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,6,15);
                    track_cust_check_beforepoweroff_flag(0);
                    if (track_is_timer_run(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID))
                        {
                            track_stop_timer(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID);
                            }
                    track_cust_check_firstpoweron_flag(0);
                    return;
                    }
                    else if(track_cust_check_gpsflag_status(99) == 2)
                    {
                        LOGD(L_APP,L_V5,"120秒后上报LBS");
                        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 2*60*1000, track_cust_sendlbs_limit);
                        if (track_is_timer_run(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID))
                        {
                            track_stop_timer(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID);
                            }
                        return;
                        }
                    }
            else
            {
                LOGD(L_APP,L_V5,"120秒后上报LBS");
                tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 2*60*1000, track_cust_sendlbs_limit);
                if (track_is_timer_run(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID))
                        {
                            track_stop_timer(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID);
                            }
                    return;
                }
            }
                }
        if(track_cust_check_gpsflag_status(99) == 1)
        {
            LOGD(L_APP,L_V5,"模式%d开启GPS",G_parameter.extchip.mode);
            track_cust_gps_work_req((void *)13);
            }
        else
        {
            if(G_parameter.wifi.sw == 1)
                {
                    LOGD(L_APP,L_V5,"GPS开关未开，开启WIFI");
                    track_cust_module_overtime_upwifi();
                }
            else
                {
                    LOGD(L_APP,L_V5,"GPS.WIFI开关未开,60秒后上报LBS");
                    tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 60000, track_cust_sendlbs_limit);
                    }
            }
    }
    if (track_is_timer_run(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID))
    {
        track_stop_timer(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID);
    }
#endif
}

void auto_mode_callback(void* par)
{
    char buf[30] = {0};
    U8 parameter = (U8)par;
    kal_bool t1 = track_is_timer_run(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID);
    applib_time_struct tmp = {0};
    track_fun_get_time(&tmp, KAL_FALSE, &G_parameter.zone);
    LOGD(L_OBD, L_V4, "parameter %d,%d", parameter, t1);
    //track_cust_module_cal_electricity((void*)2);
    if (parameter == 99 || parameter == 98 || parameter == 100)
    {
        if (parameter == 99)
        {
            /* 1420070400 = 2015-01-01 00:00:00*/ /* 1422748800 = 2015-02-01 00:00:00*/
            U32 timep;
            timep = OTA_applib_dt_mytime_2_utc_sec(&tmp, 0);
            LOGD(L_OBD, L_V5, "mode1_time:%d, mode1_par2:%d, timep:%d", G_parameter.extchip.mode1_time, G_parameter.extchip.mode1_par2, timep);
            if (G_parameter.extchip.mode1_time >= 1422748800 && timep >= 1422748800)
            {
                if (G_parameter.extchip.mode1_time > timep)
                {
                    timep = G_parameter.extchip.mode1_time - timep;
                    LOGD(L_OBD, L_V4, "模式一工作%d分钟%d秒之后，自动切换至模式%d", timep / 60, timep % 60, G_parameter.extchip.premode);
                    track_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, timep * 1000, auto_mode_callback, (void*)99);
                    return;
                }
            }
        }

        if (G_parameter.extchip.premode == 0 || G_parameter.extchip.premode == 1)
        {
            G_parameter.extchip.mode = 2;
        }
        else
        {
            G_parameter.extchip.mode = G_parameter.extchip.premode;
        }

        if (G_parameter.extchip.mode == 2)
        {
            if (G_parameter.extchip.mode2_par1 == 0)
            {
                G_parameter.extchip.mode2_par1 = tmp.nHour * 60 + tmp.nMin;
            }
        }
        if (G_realtime_data.remove_time>0)
        {
            G_realtime_data.remove_time=0;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        }
        G_parameter.extchip.mode1_en=KAL_FALSE;
        track_cust_shutdown_all_location();
        track_cust_rem_tracking_time(2,NULL);
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        LOGD(L_OBD, L_V4, "模式一时间到，自动切换至模式%d", G_parameter.extchip.mode);
        track_cust_module_tracking_mode(0);

        if (G_parameter.wifi.sw)
        {
            track_cust_sendWIFI_paket();
        }
        else
        {
            track_cust_sendlbs_limit();
        }
        track_cust_lbs_send_rst(KAL_TRUE);
        if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
        {
            track_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
        }
    }
}

void track_cust_module_auto_sw_mode(void)
{
#if defined(__GT420D__)
    return;
#endif
    /* 1420070400 = 2015-01-01 00:00:00*/ /* 1422748800 = 2015-02-01 00:00:00*/
    if (G_parameter.extchip.mode == 1 || G_parameter.extchip.mode1_en)
    {
        U32 timep;
        static applib_time_struct tmp = {0};
        OTA_applib_dt_get_rtc_time(&tmp);
        timep = OTA_applib_dt_mytime_2_utc_sec(&tmp, 0);
        LOGD(L_OBD, L_V5, "mode1_time:%d, mode1_par2:%d, timep:%d", G_parameter.extchip.mode1_time, G_parameter.extchip.mode1_par2, timep);
        if (G_parameter.extchip.mode1_time >= 1422748800 && timep >= 1422748800 && track_cust_syn_st_time(0,99)!=0)
        {
            if (G_parameter.extchip.mode1_time >= timep)
            {
                timep = G_parameter.extchip.mode1_time - timep;
                if (timep/60 > G_parameter.extchip.mode1_par2+10)
                {
                    LOGD(L_OBD, L_V4, "警告，模式一持续时间还有%d 分钟，远大于参数设置(%d 分钟)，马上切换省电模式!!", timep / 60,G_parameter.extchip.mode1_par2);
                    track_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, 1000, auto_mode_callback, (void*)100);

                }
                else
                {
                    LOGD(L_OBD, L_V4, "模式一工作%d分钟%d秒之后，自动切换至模式二", timep / 60, timep % 60);
                    if (0 == timep)
                    {
                        track_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, 1000, auto_mode_callback, (void*)100);
                    }
                    else
                    {
                        track_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, timep * 1000, auto_mode_callback, (void*)99);
                    }
                }
            }
            else
            {
                timep -= G_parameter.extchip.mode1_time;
                LOGD(L_OBD, L_V4, "WARN: 模式一已经超时工作%d分钟%d秒，立即自动切换至模式二", timep / 60, timep % 60);
                track_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, 1000, auto_mode_callback, (void*)100);
            }
        }
        else if (G_realtime_data.remove_time>=G_parameter.extchip.mode1_par2)
        {
            LOGD(L_OBD, L_V4, "警告，模式一运行已经运行%d分钟，马上切换省电模式!!", G_realtime_data.remove_time);
            //track_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, 1000, auto_mode_callback, (void*)100);
        }
        else if (timep < 1422748800)
        {
            kal_bool tracking_en=KAL_FALSE;
            track_cust_rem_tracking_time(99,&tracking_en);
            if (tracking_en==KAL_FALSE)
            {
                track_cust_rem_tracking_time(1,NULL);
            }
            if (!track_is_timer_run(TRACK_CUST_CALCULATE_ELE_TIMER_ID))
            {
                //track_start_timer(TRACK_CUST_CALCULATE_ELE_TIMER_ID, CAL_ELETRCITY * 1000, track_cust_module_cal_electricity,(void*)1);
            }
            tr_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, 30000, track_cust_module_auto_sw_mode);
            return;
        }
        else
        {
            LOGD(L_OBD, L_V5, "ERROR 1");
        }
    }
}

void track_got_st_keep_time(kal_uint8* timep)
{
    LOGD(L_OBD, L_V5, "st_time %d %d %d %d %d", st_time[0], st_time[1], st_time[2], st_time[3], st_time[4]);
    if (st_time[2] > 15) //13+05+16+08+12   时分年月日
    {
        timep[0] = st_time[styear];
        timep[1] = st_time[stmonth];
        timep[2] = st_time[stday];
        timep[3] = st_time[sthour];
        timep[4] = st_time[stmin];
        timep[5] = 0;
    }
}


void track_cust_notify_st_remove_alarm_modify(U8 op)
{
#ifdef __GT420D__
    track_cust_send06((void *)op);
#else
    track_cust_send05((void *)op);
#endif
}

void track_cust_nw_reset2()
{
    if (G_realtime_data.nw_sign>0)
    {
        G_realtime_data.nw_sign=0;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        LOGD(L_OBD, L_V3, "已搜网成功2分钟后还未能联网，清除标记!", G_realtime_data.nw_reset_sign);
    }
}

void track_cust_release_nw_value()
{
    G_realtime_data.nw_reset_sign = 0;
    G_realtime_data.nw_sign = 0;
    G_realtime_data.MainCellId = 0;
    G_realtime_data.ams_push = 1;
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
}

void track_cust_nw_reset()
{
    #if !defined(__GT420D__)
    if (G_parameter.extchip.mode != 1 && G_realtime_data.tracking_mode==0 && G_parameter.extchip.mode1_en==0 && track_cust_is_innormal_wakeup()==KAL_FALSE)
    #else
    if (G_parameter.extchip.mode != 1 && G_realtime_data.tracking_mode==0 && G_parameter.extchip.mode1_en==0)
    #endif
    {
        int v_size = 0;
        v_size = track_soc_position_queue();
        if (v_size )
        {
            track_soc_bakup_data((void*)3);
        }
        if (!track_is_testmode()  && (track_cust_get_rssi(-1) != 99) && track_cust_status_sim())
        {
            track_cust_send03((void*)240);//
            LOGD(L_OBD, L_V3, "未搜到网络 RESET NOW! %d", G_realtime_data.nw_reset_sign);
            if (G_realtime_data.nw_reset_sign >= 2)
            {
                char ams_buf[30] = {0};
                snprintf(ams_buf, sizeof(ams_buf), "(Q)(%d)%d", RUNING_TIME_SEC, G_realtime_data.nw_reset_sign);
                //track_cust_cache_netlink(ams_buf, strlen(ams_buf));
                track_cust_release_nw_value();
                track_os_intoTaskMsgQueueExt(track_cust_send_lbs, (void*)3);
                return;
            }
            else if (G_realtime_data.nw_reset_sign == 0)
            {
                LBS_Multi_Cell_Data_Struct *lbs_strcut = track_drv_get_lbs_data();
                G_realtime_data.MainCellId = lbs_strcut->MainCell_Info.cell_id;
            }
            track_cust_netlog_cache(0);//日志保存文件
            G_realtime_data.nw_sign++;
            G_realtime_data.nw_reset_sign++;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
            track_cust_restart(128, 5);
        }
        else
        {
            char ams_buf[30] = {0};
            snprintf(ams_buf, sizeof(ams_buf), "(Q)(%d)%d", RUNING_TIME_SEC, G_realtime_data.nw_reset_sign);
            //track_cust_cache_netlink(ams_buf, strlen(ams_buf));
            track_cust_release_nw_value();
            track_os_intoTaskMsgQueueExt(track_cust_send_lbs, (void*)3);
        }

    }
}

kal_bool track_cust_gps_need_open()
{
    kal_bool result = KAL_FALSE;
    if (G_parameter.gpswake || G_parameter.extchip.mode1_en || G_realtime_data.tracking_mode )
    {
        result = KAL_TRUE;
    }
#if defined(__GT741__)||defined(__GT420D__)
    result = KAL_FALSE;
#endif
    return result;
}


void track_cust_check_SNR_timeout()
{
    int i;
    char buf[100] = {0};
    applib_time_struct currTime_ams = {0};
    track_gps_data_struct *gd = track_drv_get_gps_data();
    LOGD(L_GPS, L_V5, "%d satinview %d, bd %d",track_cust_gps_status(), gd->gpgsv.Satellites_In_View, gd->bdgsv.Satellites_In_View);
    for (i=0; i<gd->gpgsv.Satellites_In_View; i++)
    {
        LOGD(L_GPS, L_V7, "Sat%d:%d,",i,gd->gpgsv.SNR[i]);
        if (gd->gpgsv.SNR[i]>25)
            return;

    }
    if (track_is_timer_run(TRACK_CUST_POWERON_CLOSE_GPS_TIMER))
    {
        track_stop_timer(TRACK_CUST_POWERON_CLOSE_GPS_TIMER);
    }

    if (G_parameter.extchip.mode==1)
    {
        #if defined(__GT420D__)
                track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                snprintf(buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d GPS OFF\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec);
                LOGD(5,5,"len:%d",strlen(buf));
                track_cust_save_logfile(buf, strlen(buf));
        #endif
        LOGD(L_GPS, L_V5, "信噪比没有一个超过25 GPS 关闭!!!!");
        if (track_cust_need_check_sportorstatic(99) == 1)
            {
                LOGD(L_APP,L_V5,"检测到运动静止需要切换下需要检测速度，暂时不关闭GPS");
                return;
            }
        track_cust_gps_control(0);
    }
    track_stop_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_OVERTIME_TIMER_ID);
    if (G_parameter.extchip.mode==1)
    {
        track_cust_module_overtime_upwifi();
    }
    track_cust_set_gps_status(GPS_NO_SAT);
}


void track_cust_gps_search_timeout()
{
    int i;
    char buf[100] = {0};
    applib_time_struct currTime_ams = {0};
    track_gps_data_struct *gd = track_drv_get_gps_data();
    LOGD(L_GPS, L_V5, "%d satinview %d, bd %d",track_cust_gps_status(), gd->gpgsv.Satellites_In_View, gd->bdgsv.Satellites_In_View);
    for (i=0; i<gd->gpgsv.Satellites_In_View; i++)
    {
        LOGD(L_GPS, L_V7, "Sat%d:%d,",i,gd->gpgsv.SNR[i]);
    }
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        return;
    }
    if (track_cust_gps_status()!=GPS_STAUS_SCAN || gd->gpgsv.Satellites_In_View >=1 || gd->bdgsv.Satellites_In_View>=1)
    {
        tr_start_timer(TRACK_CUST_GPS_SNR_TIMER_ID, 30*1000, track_cust_check_SNR_timeout);
        return;
    }

    if (track_is_timer_run(TRACK_CUST_POWERON_CLOSE_GPS_TIMER))
    {
        track_stop_timer(TRACK_CUST_POWERON_CLOSE_GPS_TIMER);
    }

    if (G_parameter.extchip.mode==1)
    {
        #if defined(__GT420D__)
                track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                snprintf(buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d GPS OFF\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec);
                LOGD(5,5,"len:%d",strlen(buf));
                track_cust_save_logfile(buf, strlen(buf));
            #endif
        LOGD(L_GPS, L_V5, "GPS 未搜到星关闭!!!!");
        if (track_cust_need_check_sportorstatic(99) == 1)
            {
                LOGD(L_APP,L_V5,"检测到运动静止需要切换下需要检测速度，暂时不关闭GPS");
                return;
            }
        track_cust_gps_control(0);
    }
    track_stop_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_OVERTIME_TIMER_ID);
    if (G_parameter.extchip.mode==1)
    {
        track_cust_module_overtime_upwifi();
    }
    track_cust_set_gps_status(GPS_NO_SAT);
}

void track_cust_soc_plugup_timeout()
{
    int pSize = track_soc_position_queue();
    if (Socket_get_write_status(0) && pSize)
    {
        LOGD(L_SOC, L_V5, "Socket 堵住啦，有%d条定位数据未发出马上重连!!!!...",pSize);
        track_soc_gprs_reconnect((void*)4);
    }
}

void track_cust_module_power_data(kal_uint8 *data, int len)
{
    static kal_uint8  buf[2000] = {0};
    static kal_uint32 flag = 0;
    static kal_uint32 tick = 0;
    static int last_len = 0;
    kal_uint8 *start = NULL, *end = NULL;
    int j = 0, x;
    LOGD(L_OBD,L_V5,"单片机来的数据:%s",data);
    if (len <= 0 || (len == 1 && (*data == 0 || *data == 0xFF))) return;

    if (tick > 0 && OTA_kal_get_systicks() - tick > OTA_kal_secs_to_ticks(20))
    {
        flag = 0;
    }
    tick = OTA_kal_get_systicks();

    if (flag + len < 1999)
    {
        for (j = 0; j < len; j++)
        {
            if (data[j] > 0 && data[j] < 128)
            {
                buf[flag++] = data[j];
            }
        }
        buf[flag] = 0;
    }
    else
    {
        if (len > 1999)
        {
            LOGD(L_OBD, L_V1, "ERROR : Data too long!");
            return;
        }
        for (j = 0, flag = 0; j < len; j++)
        {
            if (data[j] > 0 && data[j] <= 126)
            {
                buf[flag++] = data[j];
            }
        }
        buf[flag] = 0;
    }
    LOGD(L_OBD, L_V7, "[recv](%d,%d,%d): %s", strlen(buf), flag, len, data);
    LOGH(L_OBD, 7, buf, flag);
#if defined(__GT740__)||defined(__GT420D__)
    track_drv_sleep_enable(SLEEP_OBD_MOD, KAL_FALSE);
    tr_start_timer(TRACK_CUST_EXTCHIP_SLEEP_TIMER_ID, 5*1000, track_cust_modeule_into_sleep_mode);
#endif
    if (flag <= 0) return;
    buf[flag] = 0;

    for (j = 0; j < flag - 5; j++)
    {
        if (buf[j] == 'A' && buf[j+1] == 'T' && buf[j+2] == '^' && buf[j+3] == 'G' && buf[j+4] == 'T'
                && buf[j+5] == '_' && buf[j+6] == 'C' && buf[j+7] == 'M' && buf[j+8] == '=')
        {
            start = &buf[j];
            //end = strstr(start, "\r\n");
            end = strstr(start, "#");
            if (end != NULL)
            {
                int size, i, ret;
                j = end - buf;
                if (j > flag)
                {
                    LOGD(L_OBD, L_V1, "ERROR");
                    memset(buf, 0, 2000);
                    flag = 0;
                    return;
                }
                if (G_realtime_data.netLogControl & 256)
                {
                    cust_packet_log_data_ext2(8, "[Recv]", (kal_uint8 *)buf, strlen((char *)buf));
                }
                cmd_parameters_count = track_fun_str_analyse(start + 9, cmd_parameters, PARAMETER_MAX, NULL, "#", ',');

                size = sizeof(CMD_Data) / sizeof(cust_recv_struct);
                LOGD(L_OBD, L_V1, "[%d %d] %s,%s", cmd_parameters_count, size, start, end);

                LOGH(L_OBD, 6, cmd_parameters, 15);

                for (i = 0; i < size; i++)
                {
                    ret = strcmp(cmd_parameters[0], CMD_Data[i].cmd_string);
                    LOGD(L_OBD, L_V9, "ret=%d|%s,%s", ret, cmd_parameters[0], CMD_Data[i].cmd_string);
                    if (!ret)
                    {
                        LOGD(L_OBD, L_V9, "[%d], %s", i, CMD_Data[i].cmd_string);
                        if (CMD_Data[i].func_ptr != NULL)
                        {
                            CMD_Data[i].func_ptr();
                            LOGD(L_OBD, L_V8, "run [%d], %s", i, CMD_Data[i].cmd_string);
                            g_tick_new = OTA_kal_get_systicks();
                        }
                        break;
                    }
                }
                //LOGD(L_OBD, L_V1, "flag:%d,%d", flag, end - buf);
            }
        }
    }
    //LOGD(L_OBD, L_V2, "2 flag:%d, buf:%p, start:%p, end:%p", flag, buf, start, end);
    if (start == NULL)
    {
        start = strstr(buf, "08 OK\r\n");
        if (start)
        {
            end = start + 7;
            cmd_08();
        }
        else
        {
            start = strstr(buf, "02 OK\r\n");
            if (start)
            {
                end = start + 7;
                cmd_02();
            }
        }
        if (start == NULL)
        {
            return;
        }
    }
    if (end == 0 && start > buf)
    {
        end = start;
    }
    if (end > buf)
    {
        kal_uint8 *bb;
        if (*(end + 1) == 0x0a) end += 2;
        if (flag == (end - buf))
        {
            flag = 0;
        }
        else if (flag > (end - buf))
        {
            flag -= (end - buf);
            if (flag > 2000)
            {
                LOGD(L_OBD, L_V1, "error");
                return;
            }
            bb = (U8*)Ram_Alloc(6, flag);
            if (bb == NULL)
            {
                LOGD(L_OBD, L_V1, "error 2");
                return;
            }
            memcpy(bb, end, flag);
            memset(buf, 0, 2000);
            memcpy(buf, bb, flag);
            Ram_Free(bb);
        }
    }
    LOGD(L_OBD, L_V7, "3 flag:%d, buf:%p, start:%p, end:%p", flag, buf, start, end);
}
static kal_uint8 sw_status[10]={0};
kal_uint8 track_project_save_switch(U8 op, sw_type type)
{
    if (type<sw_max)
    {
        if (op==1 || op==0)
        {
            sw_status[type]=op;
        }
        return sw_status[type];
    }
    return 0;
}

void track_cust_upload_param(void)
{
    kal_uint8 *sendData;
    char data[300] = {0}, sos[68] = {0}, center[26] = {0}, fence[70] = {0}, m_buf[100] = {0};
    char in_out[10] = {0};
    kal_uint16 p1, p2, p3;
    applib_time_struct ap = {0};
    get_st_time(&ap);
    sendData = (U8*)Ram_Alloc(5, 5);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, 5);

    if (G_parameter.displacement_alarm.alarm_type == 0)
    {
        sendData[0] |= 0x04;
    }
    if (G_parameter.displacement_alarm.alarm_type == 1)
    {
        sendData[0]   |= 0x05;
    }
    if (G_parameter.displacement_alarm.alarm_type == 2)
    {
        sendData[0]   |= 0x07;
    }
    if (G_parameter.displacement_alarm.alarm_type == 3)
    {
        sendData[0]   |= 0x06;
    }
    if (G_parameter.displacement_alarm.sw == 1)
    {
        sendData[0]   |= 0x08 ;
    }
    if (G_parameter.vibrates_alarm.alarm_type == 0)
    {
        sendData[0]  |= 0x40;
    }
    if (G_parameter.vibrates_alarm.alarm_type == 1)
    {
        sendData[0]   |= 0x50 ;
    }
    if (G_parameter.vibrates_alarm.alarm_type == 2)
    {
        sendData[0]   |= 0x70;
    }
    if (G_parameter.vibrates_alarm.alarm_type == 3)
    {
        sendData[0]   |= 0x60;
    }
    if (G_parameter.vibrates_alarm.sw == 1)
    {
        sendData[0]   |= 0x80;
    }
    LOGD(L_APP, L_V5, "ALM1=%x", sendData[0]);

    if (G_parameter.power_extbatalm.alarm_type == 0)
    {
        sendData[1] |= 0x04;
    }
    if (G_parameter.power_extbatalm.alarm_type == 1)
    {
        sendData[1]   |= 0x05;
    }
    if (G_parameter.power_extbatalm.alarm_type == 2)
    {
        sendData[1]   |= 0x07;
    }
    if (G_parameter.power_extbatalm.alarm_type == 3)
    {
        sendData[0]   |= 0x06;
    }
    if (G_parameter.power_extbatalm.sw == 1)
    {
        sendData[1]   |= 0x08 ;
    }
    if (G_parameter.low_power_alarm.alarm_type == 0)
    {
        sendData[1]  |= 0x40;
    }
    if (G_parameter.low_power_alarm.alarm_type == 1)
    {
        sendData[1]   |= 0x50 ;
    }
    if (G_parameter.low_power_alarm.alarm_type == 2)
    {
        sendData[1]   |= 0x70;
    }
    if (G_parameter.low_power_alarm.alarm_type == 3)
    {
        sendData[0]   |= 0x60;
    }
    if (G_parameter.low_power_alarm.sw == 1)
    {
        sendData[1]   |= 0x80;
    }
    if (G_parameter.power_fails_alarm.alarm_type == 0)
    {
        sendData[2] |= 0x04;
    }
    if (G_parameter.power_fails_alarm.alarm_type == 1)
    {
        sendData[2]   |= 0x05;
    }
    if (G_parameter.power_fails_alarm.alarm_type == 2)
    {
        sendData[2]   |= 0x07;
    }
    if (G_parameter.power_fails_alarm.alarm_type == 3)
    {
        sendData[0]   |= 0x06;
    }
    if (G_parameter.power_fails_alarm.sw == 1)
    {
        sendData[2]   |= 0x08 ;
    }
    if (G_parameter.speed_limit_alarm.alarm_type == 0)
    {
        sendData[2]  |= 0x40;
    }
    if (G_parameter.speed_limit_alarm.alarm_type == 1)
    {
        sendData[2]   |= 0x50 ;
    }
    if (G_parameter.speed_limit_alarm.alarm_type == 2)
    {
        sendData[2]   |= 0x70;
    }
    if (G_parameter.speed_limit_alarm.alarm_type == 3)
    {
        sendData[0]   |= 0x60;
    }
    if (G_parameter.speed_limit_alarm.sw == 1)
    {
        sendData[2]   |= 0x80;
    }

    if (G_realtime_data.defense_mode == 1)
    {
        sendData[3]   |= 0x20 ; //手动
    }
    else if (G_realtime_data.defense_mode == 2)
    {
        sendData[3]   |= 0x10 ; //自动设防手动撤防
    }
    else
    {
        sendData[3]   |= 0x40 ; //自动
    }
    if (G_realtime_data.defences_status == 3 || G_realtime_data.defences_status == 2)
    {
        sendData[3]   |= 0x80 ;
    }
    if (G_realtime_data.switch_down == 1)
    {
        sendData[3] |= 0x02;
    }
    if (G_parameter.teardown_str.sw == 1) //防拆报警开关
    {
        sendData[3] |= 0x01;
    }
    if (G_realtime_data.oil_status == 0)
    {
        sendData[4] |= 0x01;
    }
    if (G_realtime_data.oil_status == 1)
    {
        sendData[4]   |= 0x02;
    }
    if (G_realtime_data.oil_status == 2)
    {
        sendData[4]   |= 0x04;
    }
    if (G_realtime_data.oil_status == 3)
    {
        sendData[4]   |= 0x08 ;
    }

    //SOS号码
    sprintf(sos, "SOS=%s,%s,%s", G_parameter.sos_num[0], G_parameter.sos_num[1], G_parameter.sos_num[2]);
    //中心号码
    sprintf(center, "CENTER=%s",  G_parameter.centerNumber);
    //围栏
    if (G_parameter.fence[0].in_out == 0)
        sprintf(in_out, "IN");
    else if (G_parameter.fence[0].in_out == 1)
        sprintf(in_out, "OUT");
    else if (G_parameter.fence[0].in_out == 2)
        sprintf(in_out, "IN or OUT");
    if (G_parameter.fence[0].square == 0)
    {
        sprintf(fence, "Fence,%s,%d,%.6f,%.6f,%d,%s,%d",
                ON_OFF(G_parameter.fence[0].sw),
                G_parameter.fence[0].square,
                G_parameter.fence[0].lat,
                G_parameter.fence[0].lon,
                G_parameter.fence[0].radius * 100,
                in_out, G_parameter.fence[0].alarm_type);
    }
    else
    {
        sprintf(fence, "Fence,%s,%d,%.6f,%.6f,%.6f,%.6f,%s,%d",
                ON_OFF(G_parameter.fence[0].sw),
                G_parameter.fence[0].square,
                G_parameter.fence[0].lat,
                G_parameter.fence[0].lon,
                G_parameter.fence[0].lat2,
                G_parameter.fence[0].lon2,
                in_out, G_parameter.fence[0].alarm_type);
    }
#if !defined(__GT420D__)
    if (G_realtime_data.tracking_mode)
    {
        sprintf(m_buf, "MODE=MODE,2,%0.2d:%0.2d,%d(MIN)",G_parameter.extchip.mode2_par1 / 60, G_parameter.extchip.mode2_par1 % 60, G_parameter.teardown_str.alarm_vel_min);
    }
    else if (G_parameter.extchip.mode1_en)
    {
        sprintf(m_buf, "MODE=MODE,1,%d,%d",G_parameter.gps_work.Fti.interval_acc_off, G_parameter.extchip.mode1_par2);
    }
    else if (G_parameter.extchip.mode == 1)
    {
        sprintf(m_buf, "MODE=MODE,%d,%d,%d", G_parameter.extchip.mode, G_parameter.gps_work.Fti.interval_acc_off, G_parameter.extchip.mode1_par2);
    }
    else if (G_parameter.extchip.mode == 2)
    {
        if (G_parameter.extchip.mode2_par2 % 60 == 0)
        {
            sprintf(m_buf, "MODE=MODE,%d,%0.2d:%0.2d,%d", G_parameter.extchip.mode, G_parameter.extchip.mode2_par1 / 60, G_parameter.extchip.mode2_par1 % 60, G_parameter.extchip.mode2_par2 / 60);
        }
        else
        {
            sprintf(m_buf, "MODE=MODE,%d,%0.2d:%0.2d,%d(MIN)", G_parameter.extchip.mode, G_parameter.extchip.mode2_par1 / 60, G_parameter.extchip.mode2_par1 % 60, G_parameter.extchip.mode2_par2);
        }
    }
    else if (G_parameter.extchip.mode == 3)
    {
        sprintf(m_buf, "MODE=MODE,%d,%d,%0.2d:%0.2d", G_parameter.extchip.mode, G_parameter.extchip.mode3_par1,
                G_parameter.extchip.mode3_par2 / 60, G_parameter.extchip.mode3_par2 % 60);
    }
    else if (G_parameter.extchip.mode == 4)
    {
        U8 vle = G_parameter.extchip.mode4_par1, i = 0;
        char m_buf1[100] = {0};
        for (; i < vle; i++)
        {
            if (0 == i)
            {
                snprintf(m_buf1 + strlen(m_buf1), 99, "%0.2d:%0.2d", G_parameter.extchip.mode4_par2[i] / 60, G_parameter.extchip.mode4_par2[i] % 60);
            }
            else
            {
                snprintf(m_buf1 + strlen(m_buf1), 99, ",%0.2d:%0.2d", G_parameter.extchip.mode4_par2[i] / 60, G_parameter.extchip.mode4_par2[i] % 60);
            }
        }
        snprintf(m_buf, 99, "MODE=MODE,%d,%d,%s", G_parameter.extchip.mode, G_parameter.extchip.mode4_par1, m_buf1);
    }
#else
    if (G_parameter.extchip.mode == 1)
    {
        sprintf(m_buf, "MODE=MODE,%d,%d", G_parameter.extchip.mode, G_parameter.extchip.mode1_par1);
    }
    else if (G_parameter.extchip.mode == 2)
    {
        sprintf(m_buf, "MODE=MODE,%d,,%d", G_parameter.extchip.mode, G_parameter.extchip.mode2_par1);
    }
#endif
    snprintf(data, sizeof(data) - 1, "ALM1=%0.2X;ALM2=%0.2X;ALM3=%0.2X;STA1=%0.2X;DYD=%0.2X;%s;%s;FENCE=%s;ICCID=%s;%s;RST=%d;WAKE=%d;STTIME %d:%d:%d:%d:%d;", sendData[0], sendData[1], sendData[2], sendData[3], sendData[4], sos, center, fence, OTA_ell_get_iccid(), m_buf, st_rst,wakeup_mode,ap.nHour, ap.nMin, ap.nYear, ap.nMonth, ap.nDay);
    LOGD(L_APP, L_V5, "%s", data);
    track_cust_paket_94(04, data, strlen(data));
    Ram_Free(sendData);

}

#elif defined(__GT300S__)


/*****************************************************************************
*  Local variable                       局部变量
*****************************************************************************/
static char *cmd_parameters[PARAMETER_MAX] = {0};
static int cmd_parameters_count = 0;
static U32 g_tick_new = 0;
static cmd_return_struct g_return_sms = {0};
static kal_bool g_02_ok = KAL_FALSE;
static kal_uint8 g_mode = 0,  g_return_sms_valid = 0;
static kal_bool ensleep = FALSE;
static kal_bool b_track_mode = KAL_FALSE;//追踪模式
static kal_uint8 st_file_name[20] = {"ST.txt"};

/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/


/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/


/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/
extern void track_cust_teardown_alarm();

extern void track_spi_write(kal_uint8 *data, kal_uint8 len);
extern void track_drv_sys_power_off_req(void);
extern void teardown_alarm_overtime(void);

/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/


/*****************************************************************************
*  Local Functions                      本地函数
******************************************************************************/
void track_cust_save_logfile(kal_uint8 *data, kal_uint16 len)
{
    kal_int8 i = 1, j = 1;
    U32 filelen = 1024 * 2, readsize = 0;
    FS_HANDLE ret = NULL;
    kal_uint16 ret2 = 0;
    WCHAR tmpfile[40] = {0}, tmpfile2[40] = {0};
    kal_uint8 tmpname[20] = {0};
    int close_ret = 0, close_ret2 = 0, delefault = 99, fs_result = 0;
    int wSize = 0;
    kal_uint8 *g_rec_data2 = NULL;
    kal_wsprintf(tmpfile, st_file_name);
    ret = FS_Open(tmpfile, FS_READ_WRITE | FS_CREATE);
    if (g_rec_data2 == NULL)
    {
        g_rec_data2 = (U8*)Ram_Alloc(6, 2 * 1024);
        if (g_rec_data2 == NULL)
        {
            LOGD(L_OBD, L_V1, "ERROR: Ram_Alloc");
            track_cust_restart(64, 0);
            return;
        }
    }
    if (ret < 0)
    {
        FS_Close(ret);
        ret = 0;
        LOGD(L_APP, L_V5, "Can not create this file! ret:%d", ret);
        return;
    }
    FS_Read(ret, g_rec_data2, filelen, &readsize);
    LOGD(L_OBD, L_V3, "==file %d %d %d", filelen, readsize, strlen(g_rec_data2));
    if (readsize >= 5 * 1024)
    {
        LOGD(L_OBD, L_V5, "数据大于5K,删除!");
        FS_Close(ret);
        ret2 = FS_Delete(tmpfile);
        LOGD(L_APP, L_V5, "FS_Delete=%d", ret2);
        return;
    }
    fs_result = FS_Seek(ret, 0, FS_FILE_END);
    if (fs_result < FS_NO_ERROR)
    {
        FS_Close(ret);
        LOGD(L_APP, L_V5, "FS_Seek %d", fs_result);
        return;
    }
    ret2 = FS_Write(ret, (void *)data, len , &wSize);
    if (ret2 < FS_NO_ERROR)
    {
        FS_Close(ret2);
        LOGD(L_APP, L_V5, "ret2:%d", ret2);
        ret2 = FS_Delete(tmpfile);
        LOGD(L_APP, L_V5, "FS_Delete=%d", ret2);
    }
    FS_Commit(ret2);
    close_ret = FS_Close(ret2);
    close_ret2 = FS_Close(ret);

    LOGD(L_OBD, L_V6, "==[DVT] %d %d %d %d %d==delefault=%d", ret, close_ret, ret2, wSize, delefault, close_ret2);

}

void track_cust_pre_remove_alarm()//300s
{
    kal_uint8 send_packet16_counts = 1;
    static kal_uint8 i = 1;
    track_gps_data_struct *gpsLastPoint = NULL;
    LOGD(L_OBD, L_V2, "index:%d,timer is:%d", i, track_is_timer_run(TRACK_CUST_EXTCHIP_SEND_REMOVE_ALARM_TIMER_ID));
    if (!track_is_timer_run(TRACK_CUST_EXTCHIP_SEND_REMOVE_ALARM_TIMER_ID))
    {
        gpsLastPoint = track_cust_gpsdata_alarm();
        track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_PRE_REMOVE, KAL_TRUE, 0);
        i++;
        if (i <= send_packet16_counts) //发送三次,定时器运行两次
        {
            tr_start_timer(TRACK_CUST_EXTCHIP_SEND_REMOVE_ALARM_TIMER_ID, 60 * 1000, track_cust_pre_remove_alarm);
        }
        else
        {
            track_stop_timer(TRACK_CUST_EXTCHIP_SEND_REMOVE_ALARM_TIMER_ID);
            i = 1;
        }

    }
    else
    {
        //防止冲突之前的报警
        LOGD(L_OBD, L_V2, "冲突,延迟2秒!");
        if (!track_is_timer_run(TRACK_CUST_EXTCHIP_SEND_REMOVE_ALARM_TIMER_ID))
        {
            tr_start_timer(TRACK_CUST_EXTCHIP_DELAY_SEND_REMOVE_ALARM_TIMER_ID, 2000, track_cust_pre_remove_alarm);
        }
    }

}

U8 track_cust_module_tracking_mode(U8 op)
{
    if (op == 99)
    {
        return G_realtime_data.tracking_mode;
    }
    if (op != G_realtime_data.tracking_mode)
    {
        G_realtime_data.tracking_mode = op;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    }
    LOGD(L_OBD, L_V5, "%d", op);
    return 0;
}
//刚开机延迟30秒发短信
static void track_cust_delay_send_msg()
{
    track_os_intoTaskMsgQueue(track_cust_teardown_alarm);
}


static void confirm(char *str)
{
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        LOGS("%s", str);
        return;
    }
    if (g_return_sms_valid)
    {
        track_cust_cmd_reply(&g_return_sms, str, strlen(str));
        g_return_sms_valid = 0;
    }
    else LOGD(L_OBD, L_V5, "%s", str);
}

kal_bool track_cust_get_mode_status(kal_bool status)
{
    static kal_bool tmp = 0;
    LOGD(L_OBD, L_V5, "%d,%d", status, tmp);
    if (status == 0XFF)
    {
        return tmp;
    }
    tmp = status;
}

static void track_cust_module_gt300s_sw_mode(void)
{
    char buf[100] = {0};
    LOGD(L_OBD, L_V5, "mode:%d,m1:%d,dt:%d", G_parameter.work_mode.mode, G_parameter.work_mode.mode1_timer, G_parameter.work_mode.datetime);
    if (G_parameter.work_mode.mode == 3)
    {
        sprintf(buf, "AT^GT_CM=04,2,%d,%d#", G_parameter.work_mode.mode3_timer, G_parameter.work_mode.datetime);
        track_spi_write(buf, strlen(buf));
    }
    else if (G_parameter.work_mode.mode == 2 || G_parameter.work_mode.mode == 1)
    {
        sprintf(buf, "AT^GT_CM=04,1,%d,%d#", G_parameter.work_mode.mode1_timer, G_parameter.work_mode.datetime);
        track_spi_write(buf, strlen(buf));
    }
    tr_start_timer(TRACK_CUST_GT300S_MODE_AUTO_SW_TIMER_ID, 6000, track_cust_module_gt300s_sw_mode);
}

void track_cust_module_extchip_shutdown(void)//300s
{
    char buf[100] = {0};
    LOGD(L_APP, L_V5, "mode:%d", G_parameter.work_mode.mode);
    if (G_parameter.work_mode.mode != 3)
    {
        return;
    }
    track_soc_bakup_data((void*)3);
    if (track_cust_get_mode_status(0xff))
    {
        sprintf(buf, "AT^GT_CM=03#");
        track_spi_write(buf, strlen(buf));
        LOGD(L_OBD, L_V5, "准备关机");
    }
    else
    {
        LOGD(L_APP, L_V1, "模式未改成功，模块不主动关机");
    }

}

void track_mode2_to_mode1(void)//300s
{
    LOGD(L_OBD, L_V5, "mode:%d,t_status_mode:%d", G_parameter.work_mode.mode, G_realtime_data.tracking_status_mode);
    if (G_realtime_data.tracking_status_mode < 4 && G_realtime_data.tracking_status_mode > 0)
    {
        G_parameter.work_mode.mode = G_realtime_data.tracking_status_mode;

        G_parameter.gps_work.Fti.interval_acc_on = G_parameter.extchip.mode1_par1;
        G_parameter.gps_work.Fti.interval_acc_off = G_parameter.extchip.mode1_par2;
    }
    G_realtime_data.tracking_mode = 0;
    track_cust_module_gt300s_sw_mode();
    if (G_parameter.work_mode.mode == 3)
    {
        tr_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, 10000, track_cust_module_extchip_shutdown);
    }
    else if (G_parameter.work_mode.mode == 1)
    {
        track_stop_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID);
#ifdef __GT300S__
        track_cust_gps_control(0);
#endif /* __GT300S__ */

        track_cust_wake_mode();

    }

    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
}

#define START_UP_UTC_TIME_MS  1422748800    /* 1420070400 = 2015-01-01 00:00:00*/ /* 1422748800 = 2015-02-01 00:00:00*/
void track_cust_module_gt300s_auto_sw_mode(void)
{
#if defined(__GT300S__)
    if (G_parameter.work_mode.mode == 2 && G_realtime_data.tracking_mode == 1)
    {
        kal_uint32 timep, deadtime;
        static applib_time_struct tmp = {0};
        static kal_uint8 cnt = 0;
        OTA_applib_dt_get_rtc_time(&tmp);
        timep = OTA_applib_dt_mytime_2_utc_sec(&tmp, 0);
        LOGD(L_OBD, L_V5, "tracking_time:%d, mode:%d.t_status_mode=%d,ms:%d", G_realtime_data.tracking_time, \
             G_parameter.work_mode.mode, G_realtime_data.tracking_status_mode, timep);
        if (G_realtime_data.tracking_time >= START_UP_UTC_TIME_MS && timep >= START_UP_UTC_TIME_MS)
        {
            deadtime = G_realtime_data.tracking_time + G_parameter.work_mode.teardown_time * 60;
            if (deadtime >= timep)
            {
                timep = deadtime - timep;
                LOGD(L_OBD, L_V4, "模式一工作%d分钟%d秒之后，自动切换至模式二", timep / 60, timep % 60);
                tr_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, timep * 1000, track_mode2_to_mode1);//
            }
            else
            {
                LOGD(L_OBD, L_V4, "WARN: 模式一已经超时工作%d分钟%d秒，立即自动切换至模式二", timep / 60, timep % 60);
                tr_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, 1000, track_mode2_to_mode1);//
            }
        }
        else if (timep < START_UP_UTC_TIME_MS)
        {
            tr_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, 30000, track_cust_module_gt300s_auto_sw_mode);
            cnt++;
            if (cnt > 30)
            {
                cnt = 0;
                track_mode2_to_mode1();//
                LOGD(L_OBD, L_V5, "15分钟都获取不到时间啊timep:%d,G_realtime_data.tracking_time=%d ", timep, G_realtime_data.tracking_time);
            }
            return;
        }
        else
        {
            tr_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, G_parameter.work_mode.teardown_time * 60 * 1000, track_mode2_to_mode1);//
            LOGD(L_OBD, L_V5, "时间出错，为什么会这样呢timep:%d,G_realtime_data.tracking_time=%d ", timep, G_realtime_data.tracking_time);
        }
    }

#endif /* __GT300S__ */

}

void track_cust_module_sw_init(void)
{
    char buf[100] = {0};
    //初始化串口
    LOGD(L_OBD, L_V5, "");

    sprintf(buf, "AT^GT_CM=01#");
    track_spi_write(buf, strlen(buf));
    tr_start_timer(TRACK_CUST_GT300S_MODE_INIT_SW_TIMER_ID, 10000, track_cust_module_sw_init);
}

void track_cust_module_delay_close(kal_uint8 op)//300s
{
    LOGD(L_OBD, L_V5, "关机时间到!:%d", op);
    LOGS("\r\n\r\n!! track_cust_module_delay_close GPS positioning has been completed, \
		Location uploaded!\r\n!! Shutdown...\r\n");
    track_cust_module_extchip_shutdown();
    tr_start_timer(TRACK_POWER_RESET_TIMER_ID, 1000, track_drv_sys_power_off_req);
}

void track_cust_work_off(void)
{
    LOGD(L_OBD, L_V5, "备份准备关机");
    if (track_cust_get_work_mode() == WORK_MODE_3)
    {
        LOGD(L_OBD, L_V5, "备份准备关机");
        track_soc_bakup_data((void*)3);
        track_cust_module_delay_close(10);
    }
}

void track_cust_power_mode_set(void)
{
    char buf[100] = {0};
    LOGD(L_OBD, L_V5, "");
    sprintf(buf, "AT^GT_CM=14,%d#", G_parameter.pwrlimit_sw);
    track_spi_write(buf, strlen(buf));
    if (!track_is_timer_run(TRACK_CUST_GT300S_MODE_INIT_SW_TIMER_ID))
    {
        tr_start_timer(TRACK_CUST_GT300S_MODE_INIT_SW_TIMER_ID, 30000, track_cust_module_sw_init);
    }
}


void cmd_gt300s_01(void)
{
    //AT^GT_CM=01,KKS_GT700_V3.0_1.12,286,0:0,2,60,575,6,270,0,1199#
    //AT^GT_CM=01,GT700_V1.01,258,13:05,01,6,300,10,275,0,1,CRC#
    int mode, i, sum = 0;
    char *p, tmp[150] = {0};
    kal_uint16 datetime;
    kal_uint16 update_time, cnt, timer, status;
    static   kal_uint8 flg = 0;
    static U8 singlebuf[100] = {0};

    tr_stop_timer(TRACK_CUST_GT300S_MODE_INIT_SW_TIMER_ID);

    //这里有问题
    for (i = 0; i < cmd_parameters_count; i++)
    {
        if (i >= 2 && i < cmd_parameters_count - 1)
        {
            sum += atoi(cmd_parameters[i]);
            p = strchr(cmd_parameters[i], ':');
            if (p != NULL)
            {
                sum += atoi(p + 1);
            }
        }
        LOGD(L_OBD, L_V5, "[%d] %s -> %d,%d\r\n", i, cmd_parameters[i], sum, atoi(cmd_parameters[i]));
    }
    if (sum != atoi(cmd_parameters[cmd_parameters_count - 1]))
    {
        LOGD(L_OBD, L_V1, "ERROR: 校验不通过. %d,%d", sum, atoi(cmd_parameters[cmd_parameters_count - 1]));
        return;
    }
#if defined (__MCU_SW_UPDATE__)
    memset(mcu_sw_version_string, 0x00, sizeof(mcu_sw_version_string));
    snprintf(mcu_sw_version_string, sizeof(mcu_sw_version_string) - 1, "%s", cmd_parameters[1]);
#endif


    snprintf(tmp, 150, "version:(%s)\r\n电压:(%s);\r\n当前时间:%s; 模式与参数:%s,%s; 起始时间:%s; 更新 时间:%s; cnt:%s,限制关机:%s;",
             cmd_parameters[1], cmd_parameters[2], cmd_parameters[3],
             cmd_parameters[4], cmd_parameters[5], cmd_parameters[6],
             cmd_parameters[7], cmd_parameters[8], cmd_parameters[9]);
    LOGD(L_OBD, L_V1, "%s", tmp);

    confirm(tmp);
    mode = atoi(cmd_parameters[4]);
    datetime = atoi(cmd_parameters[6]);
    update_time = atoi(cmd_parameters[7]);
    cnt = atoi(cmd_parameters[8]);
    timer = atoi(cmd_parameters[5]);
    status = atoi(cmd_parameters[9]);
    if (flg == 0)
    {
        flg = 1;
        snprintf(singlebuf, 99, "ver:%s,bat:%s,timer:%s,mode:%s,type:%d\n\r", cmd_parameters[1], cmd_parameters[2], cmd_parameters[3], cmd_parameters[4], mmi_Get_PowerOn_Type());
        //track_cust_save_logfile(singlebuf, strlen(singlebuf));
    }
    if (mode == 0) mode++;

    LOGD(L_APP, L_V5, "mode:%d,work mode:%d,mode3 timer:%d,work datetime:[%d,%d]", mode, G_parameter.work_mode.mode, G_parameter.work_mode.mode3_timer, \
         G_parameter.work_mode.datetime, datetime);

    if ((mode == 1) && (G_parameter.work_mode.mode != 1 && G_parameter.work_mode.mode != 2))
    {
        LOGD(L_OBD, L_V1, "模式1 不一致,%d,%d,%d", G_parameter.work_mode.mode1_timer, timer, G_parameter.work_mode.mode);
        track_cust_module_gt300s_sw_mode();
    }
    else if (mode == 2)
    {
        if (G_parameter.work_mode.mode3_timer != 0 && G_parameter.work_mode.mode3_timer != timer || G_parameter.work_mode.mode != 3)
        {
            LOGD(L_OBD, L_V1, "模式2 不一致,%d,%d,%d", G_parameter.work_mode.mode3_timer, timer, G_parameter.work_mode.mode);
            track_cust_module_gt300s_sw_mode();
            track_cust_get_mode_status(0);

        }
        else
        {
            track_cust_get_mode_status(1);
            LOGD(L_OBD, L_V1, "模式参数一致");
        }

    }
    if (datetime != G_parameter.work_mode.datetime && mode == 2)
    {
        LOGD(L_OBD, L_V1, "起始时间不一%d,%d", datetime, G_parameter.work_mode.datetime);
        track_cust_module_gt300s_sw_mode();
        track_cust_get_mode_status(0);
    }
    else
    {
        LOGD(L_OBD, L_V1, "起始时间一致");

    }

    if (status != G_parameter.pwrlimit_sw)
    {
        track_cust_power_mode_set();
    }
    track_cust_module_gt300s_auto_sw_mode();//追踪模式
    if (G_realtime_data.switch_down != atoi(cmd_parameters[10]))
    {
        G_realtime_data.switch_down = atoi(cmd_parameters[10]);
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        //  track_cust_upload_param();
    }
}

static void cmd_02(void)
{
    U8 buf[100] = {0};
    LOGD(L_OBD, L_V5, "");
    confirm("02 OK!");
    if (!g_02_ok) return;
    track_cust_module_gt300s_sw_mode();
}

static void cmd_03(void)
{
    LOGD(L_OBD, L_V5, "");
    confirm("03 OK!");
    track_drv_sys_power_off_req();
}
void cmd_gt300s_04(void)
{
    LOGD(L_OBD, L_V5, "");
    confirm("04 OK!");
    track_stop_timer(TRACK_CUST_GT300S_MODE_AUTO_SW_TIMER_ID);
    track_cust_get_mode_status(1);
    //ensleep = KAL_TRUE;
    /*if(G_parameter.extchip.mode == 1)
    {
        char buf[100] = {0};
        applib_time_struct time = {0};
        track_fun_get_time(&time, KAL_FALSE, &G_parameter.zone.zone);
        if(time.nYear > 2014)
        {
            sprintf(buf, "AT^GT_CM=02,%d,%d#\r\n", time.nHour, time.nMin);
            track_drv_uart2_write_data(buf, strlen(buf));
            LOGD(L_OBD, L_V5, "[%s]", buf);
        }
    }*/
    /*sprintf(buf, "AT^GT_CM=01#\r\n");
    track_drv_uart2_write_data(buf, strlen(buf));*/
}

kal_uint8  track_is_updata(kal_uint8 type)
{
    static kal_uint8 status = 0;
    if (type == 0xff)
    {
        return status;
    }
    else
    {
        status = type;
        return 0xff;
    }
}
static void cmd_05(void)
{
    U8 buf[100] = {0};
    if (!strcmp(cmd_parameters[1], "02"))
    {

        confirm("(Short press)!key=5,0");
        sprintf(buf, "AT^GT_CM=05#\r\n");
        track_spi_write(buf, strlen(buf));
    }
    else if (!strcmp(cmd_parameters[1], "03"))
    {
        confirm("(Short press)!key=5,1");
        sprintf(buf, "AT^GT_CM=05#\r\n");
        track_spi_write(buf, strlen(buf));
    }
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        return;
    }
//   track_cust_05_status(1);
//track_cust_ext_teardown_alm((void*)1);
#if defined(__GT300S__)
    if (!strcmp(cmd_parameters[1], "02"))
    {
        track_cust_key_control_led();
        track_is_updata(1);
        if (track_cust_gps_status() >= GPS_STAUS_2D)
        {
            track_gps_data_struct *p_gpsdata = track_cust_backup_gps_data(0, NULL);
            if (p_gpsdata->gprmc.status != 0 && (p_gpsdata->gprmc.Latitude != 0 || p_gpsdata->gprmc.Longitude != 0))
            {
                track_cust_paket_position(p_gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_KEY, 0);
            }
        }
        else
        {
            track_cust_gps_work_req((void *)12);
        }
    }
#endif

}
static void cmd_06(void)
{
    U8 buf[100] = {0};

    LOGD(L_OBD, L_V5, "");
    confirm("(Long press)!key=6");
    sprintf(buf, "AT^GT_CM=06#\r\n");
    track_spi_write(buf, strlen(buf));
}

static void cmd_07(void)
{
    LOGD(L_OBD, L_V5, "");
    confirm("07 OK!");
    tr_stop_timer(TRACK_CUST_07_TIMEOUT_TIMER);
}

void track_send_07(void)
{
    U8 buf[100] = {0};
    static kal_uint8 cnt = 0;
    LOGD(L_OBD, L_V5, "");
    sprintf(buf, "AT^GT_CM=07#");
    track_spi_write(buf, strlen(buf));
    if (cnt < 3)
    {
        cnt++;
        tr_start_timer(TRACK_CUST_07_TIMEOUT_TIMER, 1000, track_send_07);
    }

}
static void cmd_10(void)
{
    U8 buf[100] = {0};
    LOGD(L_OBD, L_V5, "");
    confirm("10 OK!");

    sprintf(buf, "AT^GT_CM=10#");
    track_spi_write(buf, strlen(buf));
}

static void cmd_08(void)
{
    U8 buf[100] = {0};
    LOGD(L_OBD, L_V5, "");
    confirm("08 OK!");

    sprintf(buf, "AT^GT_CM=08#");
    track_spi_write(buf, strlen(buf));
}

static void cmd_12(void)
{
    applib_time_struct tmp = {0};
    nvram_work_mode_struct chip = {0};
    kal_uint32 tick = 0;
    static kal_uint32 cur_tick = 0;
    U8 buf[100] = {0};

    LOGD(L_OBD, L_V5, "(Short press)teardown_sw:%d,tm:%d,tracking:%d,mode:%d,teardown:%d", \
         G_parameter.teardown_str.sw, G_realtime_data.tracking_status_mode, G_realtime_data.tracking_mode, \
         G_parameter.work_mode.mode,	G_parameter.work_mode.teardown_time);

    confirm("(Short press)!key=13,1");
    sprintf(buf, "AT^GT_CM=12#");
    track_spi_write(buf, strlen(buf));
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        return;
    }
    if (G_parameter.teardown_str.sw == 1)
    {
        if (cur_tick == 0)
        {
            cur_tick = kal_get_systicks();
        }
        else
        {
            tick = kal_get_systicks();
            if (tick - cur_tick > KAL_TICKS_1_MIN * 2)
            {
                cur_tick = tick;
            }
            else
            {
                LOGD(L_OBD, L_V5, "防拆报警触发间隔未超过2分钟!");
                return;
            }
        }
        track_cust_gps_work_req((void *)3);
        //tr_start_timer(TRACK_CUST_EXTCHIP_DELAY_SEND_ALARM_TIMER_ID, 30*1000, track_cust_delay_alarm);
        track_os_intoTaskMsgQueue(track_cust_pre_remove_alarm);
        if (G_parameter.teardown_str.alarm_type > 0 && G_parameter.teardown_str.alarm_type < 3) //1和2
        {
            //短信
            tr_start_timer(TRACK_CUST_EXTCHIP_DELAY_SEND_ALARM_MSG_TIMER_ID, 60 * 1000, teardown_alarm_overtime);
        }
        if (G_parameter.teardown_str.alarm_type > 1) //2和3
        {
            //call
            track_start_timer(TRACK_CUST_ALARM_TEAR_DOWN_CALL_TIMER, 10 * 1000, track_cust_make_call, (void*)1);
        }
        if (G_parameter.work_mode.mode == 2)
        {
            LOGD(L_OBD, L_V5, "模式%d,是否处于追踪%d 返回", G_parameter.work_mode.mode, G_realtime_data.tracking_mode);
            return;
        }

        if (G_realtime_data.tracking_mode == 0)
        {
            G_realtime_data.tracking_status_mode = G_parameter.work_mode.mode;
        }
        track_cust_module_tracking_mode(1);

        memcpy(&chip, &G_parameter.work_mode, sizeof(nvram_work_mode_struct));
        chip.mode = G_parameter.work_mode.teardown_mode;

        OTA_applib_dt_get_rtc_time(&tmp);
        G_realtime_data.tracking_time = OTA_applib_dt_mytime_2_utc_sec(&tmp, 0);
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

        LOGD(L_OBD, L_V5, "%d time: %0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d    sec:%d", G_parameter.work_mode.teardown_time,
             tmp.nYear, tmp.nMonth, tmp.nDay, tmp.nHour, tmp.nMin, tmp.nSec, G_realtime_data.tracking_time);

        memcpy(&chip, &G_parameter.work_mode, sizeof(nvram_work_mode_struct));
        chip.mode = G_parameter.work_mode.teardown_mode;

        memcpy(&G_parameter.work_mode, &chip, sizeof(nvram_work_mode_struct));
        //Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
        {
            //track_cust_upload_param();//按键状态改变上传只在01查询时不一致上传
            //track_cust_module_gt300s_auto_sw_mode();
            tr_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, \
                           G_parameter.work_mode.teardown_time * 60 * 1000 + 5000, track_mode2_to_mode1);//
        }
        else
        {
            LOGD(L_OBD, L_V5, "error write nvram 05!");
        }

        track_cust_module_gt300s_sw_mode();
        track_cust_wake_mode();
    }
    /*
    if(track_cust_gps_status() >= GPS_STAUS_2D)
    {
        track_gps_data_struct *p_gpsdata = track_cust_backup_gps_data(0, NULL);
        if(p_gpsdata->gprmc.status != 0 && (p_gpsdata->gprmc.Latitude != 0 || p_gpsdata->gprmc.Longitude != 0))
        {
            track_cust_paket_position(p_gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_KEY, 0);
        }
    }
    else
    {
        track_cust_sendlbs_limit();
    }
    */
}

static void cmd_13(void)
{
    applib_time_struct tmp = {0};
    nvram_work_mode_struct chip = {0};
    kal_uint32 tick = 0;
    static kal_uint32 cur_tick = 0;
    U8 buf[100] = {0};
    if (!strcmp(cmd_parameters[1], "0"))
    {
        LOGD(L_OBD, L_V5, "(Short press)");
        confirm("(Short press)!key=13,0");
        sprintf(buf, "AT^GT_CM=13#");
        track_spi_write(buf, strlen(buf));
    }
    else if (!strcmp(cmd_parameters[1], "1"))
    {
        LOGD(L_OBD, L_V5, "(Short press)");
        confirm("(Short press)!key=13,1");
        sprintf(buf, "AT^GT_CM=13#");
        track_spi_write(buf, strlen(buf));
    }
}


static void cmd_15(void)
{
    LOGD(L_OBD, L_V5, "");
    confirm("15 OK!");
}

static void cmd_16(void)
{
    LOGD(L_OBD, L_V5, "%s", cmd_parameters[1]);
}
static void cmd_17(void)
{
    char tmp[100]={0};
    LOGD(L_OBD, L_V5, "");
    sprintf(tmp,"key_status:%s,%s,%s#",cmd_parameters[0],cmd_parameters[1],cmd_parameters[2]);
    confirm(tmp);

}
static void cmd_99(void)
{
#if defined (__MCU_SW_UPDATE__)
    track_drv_mcu_sw_update_rsp();
#endif
    LOGD(L_OBD, L_V5, "");
    confirm("99 OK!");
}

static cust_recv_struct CMD_Data[] =
{
    {"01",        cmd_gt300s_01},
    {"02",        cmd_02},
    {"03",        cmd_03},
    {"04",        cmd_gt300s_04},
    {"05",        cmd_05},
    {"06",        cmd_06},
    {"07",        cmd_07},
    {"08",        cmd_08},
    {"10",        cmd_10},
    {"12",    	  cmd_12},
    {"13",    	  cmd_13},
    {"15",    	  cmd_15},
    {"16",		  cmd_16},
    {"17",         cmd_17},


    {"99",		  cmd_99},

};

void track_cust_module_power_cmd(cmd_return_struct *return_sms, kal_uint8 *data, int len)
{
    memcpy(&g_return_sms, return_sms, sizeof(cmd_return_struct));
    g_return_sms_valid = 1;
    track_spi_write(data, len);
}

void track_cust_module_extchip_time_synchronization(kal_uint32 arg)
{
    LOGD(L_OBD, L_V5, "%d", arg);
    if (!g_02_ok)
    {
        applib_time_struct time = {0};
        g_02_ok = KAL_TRUE;
        track_fun_get_time(&time, KAL_FALSE, &G_parameter.zone);
        LOGD(L_OBD, L_V5, "time: %0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
             time.nYear,
             time.nMonth,
             time.nDay,
             time.nHour,
             time.nMin,
             time.nSec);
        if (time.nYear >= 2016)
        {
            U8 buf[100] = {0}, valid = 0;
            //U32 timep;
            sprintf(buf, "AT^GT_CM=02,%2d,%2d#", time.nHour, time.nMin);
            track_spi_write(buf, strlen(buf));
            if (G_realtime_data.tracking_time < 1422748800 && G_realtime_data.tracking_time + arg >= 1422748800)
            {
                G_realtime_data.tracking_time += arg;
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
            }
            track_cust_module_gt300s_auto_sw_mode();
        }
    }
}

void track_cust_module_Handshake(void)
{
    char buf[100] = {0};
    sprintf(buf, "AT^GT_CM=08#");
    track_spi_write(buf, strlen(buf));
    LOGD(L_OBD, L_V5, "");
    tr_start_timer(TRACK_CUST_EXTCHIP_BATTERY_VOLTAGE_TIMER_ID, 5000, track_cust_module_Handshake);
}



void track_cust_spi_read_data(kal_uint8 *data, kal_uint16 len)
{
    char* p;
    kal_uint8 count;
    int size, i, ret;
    LOGD(L_DRV, L_V4, "单片机信息:%s", data);
    if (G_realtime_data.netLogControl & 1)
    {
        cust_packet_log_data(1, data, len);
    }

    p = strstr(data, "GT_CM=");
    if (p != NULL)
    {
        count = track_fun_str_analyse(data + 9, cmd_parameters, PARAMETER_MAX, NULL, "#", ',');
        size = sizeof(CMD_Data) / sizeof(cust_recv_struct);
        LOGD(L_OBD, L_V1, "[%d %d] ", count, size);

        LOGH(L_OBD, 6, cmd_parameters, 15);
        for (i = 0; i < size; i++)
        {
            ret = strcmp(cmd_parameters[0], CMD_Data[i].cmd_string);
            if (!ret)
            {
                LOGD(L_OBD, L_V5, "[%d], %s", i, CMD_Data[i].cmd_string);
                if (CMD_Data[i].func_ptr != NULL)
                {
                    CMD_Data[i].func_ptr();
                    LOGD(L_OBD, L_V5, "run [%d], %s", i, CMD_Data[i].cmd_string);
                }
                break;
            }
        }
    }
}

#elif defined (__GT370__)|| defined (__GT380__)
/*为GT370的新单片机串口协议修改       --    chengjun  2017-03-22*/

/*****************************************************************************
*  Local variable                       局部变量
*****************************************************************************/
static char *cmd_parameters[PARAMETER_MAX] = {0};
static int cmd_parameters_count = 0;
static U32 g_tick_new = 0;
static cmd_return_struct g_return_sms = {0};
static kal_bool g_02_ok = KAL_FALSE;//是否给单片机校时了
static kal_uint8 g_mode = 0,  g_return_sms_valid = 0;
static kal_bool ensleep = FALSE;
static kal_bool b_track_mode = KAL_FALSE;//追踪模式
static kal_uint8 st_file_name[20] = {"ST.txt"};

/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/


/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/


/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/
extern void track_cust_teardown_alarm();
extern void track_spi_write(kal_uint8 *data, kal_uint8 len);
extern void track_drv_sys_power_off_req(void);
extern void teardown_alarm_overtime(void);
extern void track_cust_open_shell_alarm();
extern void track_open_shell_alarm_overtime(void);

extern void track_cust_simple_gsm_led_sleep(void);
extern void track_cust_simple_gsm_led_resume(void);
extern void track_cust_gpsdup_interval_update(void);

/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/
void track_cust_charge_switch_mode(kal_uint8 chr_in);

/*****************************************************************************
*  Local Functions                      本地函数
******************************************************************************/
void track_cust_save_logfile(kal_uint8 *data, kal_uint16 len)
{
    kal_int8 i = 1, j = 1;
    U32 filelen = 1024 * 2, readsize = 0;
    FS_HANDLE ret = NULL;
    kal_uint16 ret2 = 0;
    WCHAR tmpfile[40] = {0}, tmpfile2[40] = {0};
    kal_uint8 tmpname[20] = {0};
    int close_ret = 0, close_ret2 = 0, delefault = 99, fs_result = 0;
    int wSize = 0;
    kal_uint8 *g_rec_data2 = NULL;
    kal_wsprintf(tmpfile, st_file_name);
    ret = FS_Open(tmpfile, FS_READ_WRITE | FS_CREATE);
    if (g_rec_data2 == NULL)
    {
        g_rec_data2 = (U8*)Ram_Alloc(6, 2 * 1024);
        if (g_rec_data2 == NULL)
        {
            LOGD(L_OBD, L_V1, "ERROR: Ram_Alloc");
            track_cust_restart(64, 0);
            return;
        }
    }
    if (ret < 0)
    {
        FS_Close(ret);
        ret = 0;
        LOGD(L_APP, L_V5, "Can not create this file! ret:%d", ret);
        return;
    }
    FS_Read(ret, g_rec_data2, filelen, &readsize);
    LOGD(L_OBD, L_V3, "==file %d %d %d", filelen, readsize, strlen(g_rec_data2));
    if (readsize >= 5 * 1024)
    {
        LOGD(L_OBD, L_V5, "数据大于5K,删除!");
        FS_Close(ret);
        ret2 = FS_Delete(tmpfile);
        LOGD(L_APP, L_V5, "FS_Delete=%d", ret2);
        return;
    }
    fs_result = FS_Seek(ret, 0, FS_FILE_END);
    if (fs_result < FS_NO_ERROR)
    {
        FS_Close(ret);
        LOGD(L_APP, L_V5, "FS_Seek %d", fs_result);
        return;
    }
    ret2 = FS_Write(ret, (void *)data, len , &wSize);
    if (ret2 < FS_NO_ERROR)
    {
        FS_Close(ret2);
        LOGD(L_APP, L_V5, "ret2:%d", ret2);
        ret2 = FS_Delete(tmpfile);
        LOGD(L_APP, L_V5, "FS_Delete=%d", ret2);
    }
    FS_Commit(ret2);
    close_ret = FS_Close(ret2);
    close_ret2 = FS_Close(ret);

    LOGD(L_OBD, L_V6, "==[DVT] %d %d %d %d %d==delefault=%d", ret, close_ret, ret2, wSize, delefault, close_ret2);

}

void track_cust_pre_remove_alarm()//370380
{
    kal_uint8 send_packet16_counts = 3;
    static kal_uint8 i = 1;
    track_gps_data_struct *gpsLastPoint = NULL;
    BOOL lbs_valid=FALSE;
    LOGD(L_OBD, L_V2, "index:%d,timer is:%d", i, track_is_timer_run(TRACK_CUST_EXTCHIP_SEND_REMOVE_ALARM_TIMER_ID));
    if (!track_is_timer_run(TRACK_CUST_EXTCHIP_SEND_REMOVE_ALARM_TIMER_ID))
    {
        gpsLastPoint = track_cust_gpsdata_alarm();

        if (gpsLastPoint->gprmc.status == 2) //发了3次报警，会收到3次短信，过滤后面两条
        {
            lbs_valid=track_drv_get_lbs_data()->status;
            if (lbs_valid)
            {
                track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_PRE_REMOVE, KAL_TRUE, 0);
                i++;
            }
            else
            {
                LOGD(L_APP, L_V5, "no lbs ,delay 60s");
            }
        }
        else
        {
            track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_PRE_REMOVE, KAL_TRUE, 0);
            i++;
        }

        if (i <= send_packet16_counts) //发送三次,定时器运行两次
        {
            tr_start_timer(TRACK_CUST_EXTCHIP_SEND_REMOVE_ALARM_TIMER_ID, 60 * 1000, track_cust_pre_remove_alarm);
        }
        else
        {
            track_stop_timer(TRACK_CUST_EXTCHIP_SEND_REMOVE_ALARM_TIMER_ID);
            i = 1;
        }

    }
    else
    {
        //防止冲突之前的报警
        LOGD(L_OBD, L_V2, "冲突,延迟2秒!");
        if (!track_is_timer_run(TRACK_CUST_EXTCHIP_SEND_REMOVE_ALARM_TIMER_ID))
        {
            tr_start_timer(TRACK_CUST_EXTCHIP_DELAY_SEND_REMOVE_ALARM_TIMER_ID, 2000, track_cust_pre_remove_alarm);
        }
    }
}


void track_cust_pre_open_shell_alarm()
{
    kal_uint8 send_packet16_counts = 3;
    static kal_uint8 i = 1;
    track_gps_data_struct *gpsLastPoint = NULL;
    BOOL lbs_valid=FALSE;

    LOGD(L_OBD, L_V2, "index:%d,timer is:%d", i, track_is_timer_run(TRACK_CUST_EXTCHIP_SEND_OPEN_SHELL_ALARM_TIMER_ID));
    if (!track_is_timer_run(TRACK_CUST_EXTCHIP_SEND_OPEN_SHELL_ALARM_TIMER_ID))
    {
        gpsLastPoint = track_cust_gpsdata_alarm();
        if (gpsLastPoint->gprmc.status == 2)
        {
            lbs_valid=track_drv_get_lbs_data()->status;
            if (lbs_valid)
            {
                track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_OPEN_SHELL, KAL_TRUE, 0);
                i++;
            }
            else
            {
                LOGD(L_APP, L_V5, "no lbs ,delay 60s");
            }
        }
        else
        {
            track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_OPEN_SHELL, KAL_TRUE, 0);
            i++;
        }

        if (i <= send_packet16_counts) //发送三次,定时器运行两次
        {
            tr_start_timer(TRACK_CUST_EXTCHIP_SEND_OPEN_SHELL_ALARM_TIMER_ID, 60 * 1000, track_cust_pre_open_shell_alarm);
        }
        else
        {
            track_stop_timer(TRACK_CUST_EXTCHIP_SEND_OPEN_SHELL_ALARM_TIMER_ID);
            i = 1;
        }
    }
    else
    {
        //防止冲突之前的报警
        LOGD(L_OBD, L_V2, "冲突,延迟2秒!");
        if (!track_is_timer_run(TRACK_CUST_EXTCHIP_SEND_OPEN_SHELL_ALARM_TIMER_ID))
        {
            tr_start_timer(TRACK_CUST_EXTCHIP_DELAY_SEND_OPEN_SHELL_ALARM_TIMER_ID, 2000, track_cust_pre_open_shell_alarm);
        }
    }
}


U8 track_cust_module_tracking_mode(U8 op)
{
    if (op == 99)
    {
        return G_realtime_data.tracking_mode;
    }
    if (op != G_realtime_data.tracking_mode)
    {
        G_realtime_data.tracking_mode = op;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    }
    LOGD(L_OBD, L_V5, "%d", op);
    return 0;
}

//刚开机延迟30秒发短信
static void track_cust_delay_send_msg()
{
    track_os_intoTaskMsgQueue(track_cust_teardown_alarm);
}


static void confirm(char *str)
{
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        LOGS("%s", str);
        return;
    }
    if (g_return_sms_valid)
    {
        track_cust_cmd_reply(&g_return_sms, str, strlen(str));
        g_return_sms_valid = 0;
    }
    else
    {
        LOGD(L_OBD, L_V5, "%s", str);
    }
}

kal_bool track_cust_get_mode_status(kal_bool status)
{
    static kal_bool tmp = 0;
    LOGD(L_OBD, L_V5, "%d,%d", status, tmp);
    if (status == 0XFF)
    {
        return tmp;
    }
    tmp = status;
}

static void track_cust_module_sw_mode_to_mcu(void)
{
    char buf[100] = {0};
    LOGD(L_OBD, L_V5, "mode:%d,m1:%d,dt:%d", G_parameter.work_mode.mode, G_parameter.work_mode.mode1_timer, G_parameter.work_mode.datetime);
    if (G_parameter.work_mode.mode == 3)
    {
        sprintf(buf, "AT^GT_CM=04,2,%d,%d#", G_parameter.work_mode.mode3_timer, G_parameter.work_mode.datetime);
        track_spi_write(buf, strlen(buf));
    }
    else if (G_parameter.work_mode.mode == 2 || G_parameter.work_mode.mode == 1)
    {
        sprintf(buf, "AT^GT_CM=04,1,%d,%d#", G_parameter.work_mode.mode1_timer, G_parameter.work_mode.datetime);
        track_spi_write(buf, strlen(buf));
    }
    tr_start_timer(TRACK_CUST_GT300S_MODE_AUTO_SW_TIMER_ID, 6000, track_cust_module_sw_mode_to_mcu);
}

void track_cust_module_extchip_shutdown(void)//370380
{
    char buf[100] = {0};
    LOGD(L_APP, L_V5, "mode:%d", G_parameter.work_mode.mode);
    if (G_parameter.work_mode.mode != 3)
    {
        return;
    }

    if (track_cust_message_status())
    {
        LOGD(L_APP, L_V4, "有短信处理，暂不关机");
        return;
    }

    if (track_drv_bmt_get_charging_status() != 0)
    {
        LOGD(L_APP, L_V4, "充电中，暂不关机");
        return;
    }

    track_soc_bakup_data((void*)3);
    if (track_cust_get_mode_status(0xff))
    {
        sprintf(buf, "AT^GT_CM=03,3#");
        track_spi_write(buf, strlen(buf));
        LOGD(L_OBD, L_V5, "准备关机");
    }
    else
    {
        LOGD(L_APP, L_V1, "模式未改成功，模块不主动关机");
    }

}

/******************************************************************************
 *  Function    -  track_mode2_to_mode1
 *
 *  Purpose     -  报警结束，该回原来的模式了
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-23,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_mode2_to_mode1(void)//370380
{
    LOGD(L_OBD, L_V5, "mode:%d,t_status_mode:%d", G_parameter.work_mode.mode, G_realtime_data.tracking_status_mode);
    if (G_realtime_data.tracking_status_mode < 4 && G_realtime_data.tracking_status_mode > 0)
    {
        G_parameter.work_mode.mode = G_realtime_data.tracking_status_mode;

        G_parameter.gps_work.Fti.interval_acc_on = G_parameter.extchip.mode1_par1;
        G_parameter.gps_work.Fti.interval_acc_off = G_parameter.extchip.mode1_par2;
    }
    G_realtime_data.tracking_mode = 0;
    track_cust_module_sw_mode_to_mcu();
    if (G_parameter.work_mode.mode == 3)//3=为需求里的模式2，定时开机，超长待机
    {
        if (track_drv_bmt_get_charging_status()!=0)
        {
            track_cust_charge_switch_mode(1);
        }
        else
        {
            tr_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, 10000, track_cust_module_extchip_shutdown);
        }
    }
    else if (G_parameter.work_mode.mode == 1)
    {
        track_stop_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID);

        track_cust_gps_control(0);//报警结束退回原模式关闭GPS

        track_cust_wake_mode();
    }
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
}

#define START_UP_UTC_TIME_MS  1422748800    /* 1420070400 = 2015-01-01 00:00:00*/ /* 1422748800 = 2015-02-01 00:00:00*/
void track_cust_module_gt300s_auto_sw_mode(void)
{
    if (G_parameter.work_mode.mode == 2 && G_realtime_data.tracking_mode == 1)
    {
        kal_uint32 timep, deadtime;
        static applib_time_struct tmp = {0};
        static kal_uint8 cnt = 0;
        OTA_applib_dt_get_rtc_time(&tmp);
        timep = OTA_applib_dt_mytime_2_utc_sec(&tmp, 0);
        LOGD(L_OBD, L_V5, "tracking_time:%d, mode:%d.t_status_mode=%d,ms:%d", G_realtime_data.tracking_time, \
             G_parameter.work_mode.mode, G_realtime_data.tracking_status_mode, timep);
        if (G_realtime_data.tracking_time >= START_UP_UTC_TIME_MS && timep >= START_UP_UTC_TIME_MS)
        {
            deadtime = G_realtime_data.tracking_time + G_parameter.work_mode.teardown_time * 60;
            if (deadtime >= timep)
            {
                timep = deadtime - timep;
                LOGD(L_OBD, L_V4, "模式一工作%d分钟%d秒之后，自动切换至模式二", timep / 60, timep % 60);
                tr_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, timep * 1000, track_mode2_to_mode1);//
            }
            else
            {
                LOGD(L_OBD, L_V4, "WARN: 模式一已经超时工作%d分钟%d秒，立即自动切换至模式二", timep / 60, timep % 60);
                tr_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, 1000, track_mode2_to_mode1);//
            }
        }
        else if (timep < START_UP_UTC_TIME_MS)
        {
            tr_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, 30000, track_cust_module_gt300s_auto_sw_mode);
            cnt++;
            if (cnt > 30)
            {
                cnt = 0;
                track_mode2_to_mode1();//
                LOGD(L_OBD, L_V5, "15分钟都获取不到时间啊timep:%d,G_realtime_data.tracking_time=%d ", timep, G_realtime_data.tracking_time);
            }
            return;
        }
        else
        {
            tr_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, G_parameter.work_mode.teardown_time * 60 * 1000, track_mode2_to_mode1);//
            LOGD(L_OBD, L_V5, "时间出错，为什么会这样呢timep:%d,G_realtime_data.tracking_time=%d ", timep, G_realtime_data.tracking_time);
        }
    }
}



/******************************************************************************
 *  Function    -  track_set_cust_status_charger
 *
 *  Purpose     -  更新充电状态标记
 *
 *  Description -   GT370A 单片机串口通知
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-03-23,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_charger_status_updata_from_mcu(kal_uint8 chr_in)
{
    static kal_uint8 pre_chr_in = 0xFF;

    ilm_struct *ilm_ptr;
    charbat_status_struct *myMsgPtr = NULL;
    module_type src_mod_id = kal_get_active_module_id();

    if (chr_in == pre_chr_in)
    {
        return;
    }

    pre_chr_in = chr_in;

    LOGD(L_DRV, L_V5, "chr_in=%d", chr_in);

    //heartbeat_packets((void *)110);
    myMsgPtr = (charbat_status_struct*) construct_local_para(sizeof(charbat_status_struct), TD_CTRL);
    if (myMsgPtr != NULL)
    {
        if (chr_in != 0)
        {
            myMsgPtr->status = PMIC_CHARGER_IN;
        }
        else
        {
            myMsgPtr->status = PMIC_CHARGER_OUT;
        }

        //充电器插拔事件不检查电池电压
        myMsgPtr->level = BATTERY_LEVEL_3 ;
        myMsgPtr->volt = 4000000;

        ilm_ptr = (ilm_struct *)allocate_ilm(src_mod_id);
        if (ilm_ptr != NULL)
        {
            ilm_ptr->src_mod_id = src_mod_id;
            ilm_ptr->dest_mod_id = MOD_MMI;
            ilm_ptr->sap_id = INVALID_SAP;
            ilm_ptr->msg_id = (msg_type) MSG_ID_CHARBAT_BMT_STATUS;
            ilm_ptr->local_para_ptr = (local_para_struct*) myMsgPtr;
            ilm_ptr->peer_buff_ptr = NULL;
            msg_send_ext_queue(ilm_ptr);
        }
    }
}


void track_cust_module_sw_init(void)
{
    char buf[100] = {0};
    //初始化串口
    LOGD(L_OBD, L_V5, "");

    sprintf(buf, "AT^GT_CM=01#");
    track_spi_write(buf, strlen(buf));
    tr_start_timer(TRACK_CUST_GT300S_MODE_INIT_SW_TIMER_ID, 10000, track_cust_module_sw_init);
}

void track_cust_module_delay_close(kal_uint8 op)//370380
{
    LOGD(L_OBD, L_V5, "关机时间到!:%d", op);
    LOGS("\r\n\r\n!! track_cust_module_delay_close GPS positioning has been completed,Location uploaded!\r\n!! Shutdown...\r\n");
    track_cust_module_extchip_shutdown();
    tr_start_timer(TRACK_POWER_RESET_TIMER_ID, 1000, track_drv_sys_power_off_req);
}

void track_cust_work_off(void)
{
    LOGD(L_OBD, L_V5, "备份准备关机");
    if (track_cust_get_work_mode() == WORK_MODE_3)
    {
        LOGD(L_OBD, L_V5, "备份准备关机");
        track_soc_bakup_data((void*)3);
        track_cust_module_delay_close(10);
    }
}


//GSM发：AT^GT_CM=14,A,B# （X=1,表示限制，X=0，表示解除）
//A：按键类型，01表示防拆键，02表示开机键，03表示开盖键，04表示充电插入检测（此项无效），05表示SOS键……
//B：00表示使能，01表示失能
static void track_cust_key_enable_sync_to_mcu(kal_uint8 key_type, kal_uint8 disable)
{
    char buf[100] = {0};
    sprintf(buf, "AT^GT_CM=14,%02d,%02d#", key_type, disable);
    track_spi_write(buf, strlen(buf));
}

/******************************************************************************
 *  Function    -  track_cust_key_mode_check
 *
 *  Purpose     -  按键使能同步     //B：00表示使能，01表示失能
 *
 *  Description -  GSM每次只能发一个同步包
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-03-27,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_key_mode_check(kal_uint8 pwrlimit_sw, kal_uint8 open_shell_alarm, kal_uint8 teardown_str)
{
    kal_uint8 disable = 1;

    LOGD(L_OBD, L_V5, "pwr=%d,%d,open=%d,%d,teardown=%d,%d", G_parameter.pwrlimit_sw, pwrlimit_sw, G_parameter.open_shell_alarm.sw, open_shell_alarm, G_parameter.teardown_str.sw, teardown_str);

    if (G_parameter.pwrlimit_sw != pwrlimit_sw) //限制关机01，使能为01
    {
        if (G_parameter.pwrlimit_sw == 1)
        {
            disable = 1;
        }
        else
        {
            disable = 0;
        }
        track_cust_key_enable_sync_to_mcu(2, disable);
    }
    else if (G_parameter.open_shell_alarm.sw == open_shell_alarm)   //开盖报警开启01，00表示使能
    {
        if (G_parameter.open_shell_alarm.sw == 1)
        {
            disable = 0;
        }
        else
        {
            disable = 1;
        }

        track_cust_key_enable_sync_to_mcu(3, disable);
    }
    else if (G_parameter.teardown_str.sw == teardown_str)   //防拆报警开启01，00表示使能
    {
        if (G_parameter.teardown_str.sw == 1)
        {
            disable = 0;
        }
        else
        {
            disable = 1;
        }
        track_cust_key_enable_sync_to_mcu(1, disable);
    }
    else
    {
        LOGD(L_OBD, L_V5, "键值使能同步");
        return;
    }

    if (!track_is_timer_run(TRACK_CUST_GT300S_MODE_INIT_SW_TIMER_ID))
    {
        tr_start_timer(TRACK_CUST_GT300S_MODE_INIT_SW_TIMER_ID, 5000, track_cust_module_sw_init);
    }
}

kal_uint8  track_is_updata(kal_uint8 type)
{
    static kal_uint8 status = 0;
    if (type == 0xff)
    {
        return status;
    }
    else
    {
        status = type;
        return 0xff;
    }
}

/******************************************************************************
 *  Function    -  track_cust_alarm_teardown
 *
 *  Purpose     -  拆除报警
 *
 *  Description -  单片机检测到按键事件后串口通知设备处理
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-03-22,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_alarm_teardown(void)
{
    applib_time_struct tmp = {0};
    nvram_work_mode_struct chip = {0};
    kal_uint32 tick = 0;
    static kal_uint32 cur_tick = 0;
    U8 buf[100] = {0};

    LOGD(L_DRV, L_V5, "teardown_sw:%d,tm:%d,tracking:%d,mode:%d,teardown:%d", \
         G_parameter.teardown_str.sw, G_realtime_data.tracking_status_mode, G_realtime_data.tracking_mode, \
         G_parameter.work_mode.mode,	G_parameter.work_mode.teardown_time);

    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        return;
    }
    if (G_parameter.teardown_str.sw == 1)
    {
        if (cur_tick == 0)
        {
            cur_tick = kal_get_systicks();
        }
        else
        {
            tick = kal_get_systicks();
            if (tick - cur_tick > KAL_TICKS_1_MIN * 3)
            {
                cur_tick = tick;
            }
            else
            {
                LOGD(L_OBD, L_V5, "防拆报警触发间隔未超过3分钟!");
                return;
            }
        }
        track_cust_gps_work_req((void *)3);
        //tr_start_timer(TRACK_CUST_EXTCHIP_DELAY_SEND_ALARM_TIMER_ID, 30*1000, track_cust_delay_alarm);
        track_os_intoTaskMsgQueue(track_cust_pre_remove_alarm);
        if (G_parameter.teardown_str.alarm_type > 0 && G_parameter.teardown_str.alarm_type < 3) //1和2
        {
            //短信
            tr_start_timer(TRACK_CUST_EXTCHIP_DELAY_SEND_ALARM_MSG_TIMER_ID, 60 * 1000, teardown_alarm_overtime);
        }
        if (G_parameter.teardown_str.alarm_type > 1) //2和3
        {
            //call
            track_start_timer(TRACK_CUST_ALARM_TEAR_DOWN_CALL_TIMER, 10 * 1000, track_cust_make_call, (void*)1);
        }

        if (G_parameter.work_mode.mode == 1 || G_parameter.work_mode.mode == 2 && G_realtime_data.tracking_mode == 0)
        {
            LOGD(L_OBD, L_V5, "模式%d,是否处于追踪%d 返回", G_parameter.work_mode.mode, G_realtime_data.tracking_mode);
            return;
        }

        if (G_realtime_data.tracking_mode == 0)
        {
            G_parameter.extchip.mode1_par1 = G_parameter.gps_work.Fti.interval_acc_on;
            G_parameter.extchip.mode1_par2 = G_parameter.gps_work.Fti.interval_acc_off;
            G_parameter.gps_work.Fti.interval_acc_on = 20;
            G_parameter.gps_work.Fti.interval_acc_off = 20;
            G_realtime_data.tracking_status_mode = G_parameter.work_mode.mode;

            track_cust_gpsdup_interval_update();
        }

        track_cust_module_tracking_mode(1);

        memcpy(&chip, &G_parameter.work_mode, sizeof(nvram_work_mode_struct));
        chip.mode = G_parameter.work_mode.teardown_mode;

        OTA_applib_dt_get_rtc_time(&tmp);
        G_realtime_data.tracking_time = OTA_applib_dt_mytime_2_utc_sec(&tmp, 0);
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

        LOGD(L_OBD, L_V5, "%d time: %0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d    sec:%d", G_parameter.work_mode.teardown_time,
             tmp.nYear, tmp.nMonth, tmp.nDay, tmp.nHour, tmp.nMin, tmp.nSec, G_realtime_data.tracking_time);

        memcpy(&chip, &G_parameter.work_mode, sizeof(nvram_work_mode_struct));
        chip.mode = G_parameter.work_mode.teardown_mode;

        memcpy(&G_parameter.work_mode, &chip, sizeof(nvram_work_mode_struct));
        //Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
        {
            //track_cust_upload_param();//按键状态改变上传只在01查询时不一致上传
            //track_cust_module_gt300s_auto_sw_mode();
            tr_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, \
                           G_parameter.work_mode.teardown_time * 60 * 1000 + 5000, track_mode2_to_mode1);//
        }
        else
        {
            LOGD(L_OBD, L_V5, "error write nvram 05!");
        }

        track_cust_module_sw_mode_to_mcu();
        track_cust_wake_mode();
    }
    /*
    if(track_cust_gps_status() >= GPS_STAUS_2D)
    {
        track_gps_data_struct *p_gpsdata = track_cust_backup_gps_data(0, NULL);
        if(p_gpsdata->gprmc.status != 0 && (p_gpsdata->gprmc.Latitude != 0 || p_gpsdata->gprmc.Longitude != 0))
        {
            track_cust_paket_position(p_gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_KEY, 0);
        }
    }
    else
    {
        track_cust_sendlbs_limit();
    }
    */
}

/******************************************************************************
 *  Function    -  track_cust_alarm_openshell
 *
 *  Purpose     -  开外壳报警
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-03-23,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_alarm_openshell()
{
    applib_time_struct tmp = {0};
    nvram_work_mode_struct chip = {0};
    kal_uint32 tick = 0;
    static kal_uint32 cur_tick = 0;
    U8 buf[100] = {0};

    LOGD(L_DRV, L_V5, "open_shell_alarm:%d,tm:%d,tracking:%d,mode:%d,teardown:%d", \
         G_parameter.open_shell_alarm.sw, G_realtime_data.tracking_status_mode, G_realtime_data.tracking_mode, \
         G_parameter.work_mode.mode,	G_parameter.work_mode.teardown_time);

    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        return;
    }

    if (G_parameter.open_shell_alarm.sw == 1)
    {
        if (cur_tick == 0)
        {
            cur_tick = kal_get_systicks();
        }
        else
        {
            tick = kal_get_systicks();
            if (tick - cur_tick > KAL_TICKS_1_MIN * 3)
            {
                cur_tick = tick;
            }
            else
            {
                LOGD(L_OBD, L_V5, "开盖报警触发间隔未超过3分钟!");
                return;
            }
        }
        track_cust_gps_work_req((void *)3);

        track_os_intoTaskMsgQueue(track_cust_pre_open_shell_alarm);
        if (G_parameter.open_shell_alarm.alarm_type > 0 && G_parameter.open_shell_alarm.alarm_type < 3) //1和2
        {
            //短信
            tr_start_timer(TRACK_CUST_SEND_OPEN_SHELL_SMS_TIMEOVER_TIMER_ID, 60 * 1000, track_open_shell_alarm_overtime);
        }
        if (G_parameter.open_shell_alarm.alarm_type > 1) //2和3
        {
            //call
            track_start_timer(TRACK_CUST_ALARM_TEAR_DOWN_CALL_TIMER, 10 * 1000, track_cust_make_call, (void*)1);
        }

        if (G_parameter.work_mode.mode == 1 || G_parameter.work_mode.mode == 2 && G_realtime_data.tracking_mode == 0)
        {
            LOGD(L_OBD, L_V5, "模式%d,是否处于追踪%d 返回", G_parameter.work_mode.mode, G_realtime_data.tracking_mode);
            return;
        }

        if (G_realtime_data.tracking_mode == 0)
        {
            G_parameter.extchip.mode1_par1 = G_parameter.gps_work.Fti.interval_acc_on;
            G_parameter.extchip.mode1_par2 = G_parameter.gps_work.Fti.interval_acc_off;
            G_parameter.gps_work.Fti.interval_acc_on = 20;
            G_parameter.gps_work.Fti.interval_acc_off = 20;
            G_realtime_data.tracking_status_mode = G_parameter.work_mode.mode;

            track_cust_gpsdup_interval_update();
        }

        track_cust_module_tracking_mode(1);

        memcpy(&chip, &G_parameter.work_mode, sizeof(nvram_work_mode_struct));
        chip.mode = G_parameter.work_mode.teardown_mode;

        OTA_applib_dt_get_rtc_time(&tmp);
        G_realtime_data.tracking_time = OTA_applib_dt_mytime_2_utc_sec(&tmp, 0);
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

        LOGD(L_OBD, L_V5, "%d time: %0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d    sec:%d", G_parameter.work_mode.teardown_time,
             tmp.nYear, tmp.nMonth, tmp.nDay, tmp.nHour, tmp.nMin, tmp.nSec, G_realtime_data.tracking_time);

        memcpy(&chip, &G_parameter.work_mode, sizeof(nvram_work_mode_struct));
        chip.mode = G_parameter.work_mode.teardown_mode;

        memcpy(&G_parameter.work_mode, &chip, sizeof(nvram_work_mode_struct));
        //Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
        {
            //track_cust_upload_param();//按键状态改变上传只在01查询时不一致上传
            //track_cust_module_gt300s_auto_sw_mode();
            tr_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, \
                           G_parameter.work_mode.teardown_time * 60 * 1000 + 5000, track_mode2_to_mode1);//
        }
        else
        {
            LOGD(L_OBD, L_V5, "error write nvram 05!");
        }

        track_cust_module_sw_mode_to_mcu();
        track_cust_wake_mode();
    }

}

//AT^GT_CM=01,GT300S_2.0_V1.0_160101,345,17:03:21:13:05,2,50,300,10,0,0,0,0,0,0,0,checksum#
void cmd_01(void)
{
    int mode, i, sum = 0;
    char *p, *q = NULL, tmp[150] = {0};
    kal_uint16 datetime;
    kal_uint16 update_time, timer;
    kal_uint8 pwrlimit_sw, open_shell_alarm, teardown_str, chr_in;
    static   kal_uint8 flg = 0;
    static U8 singlebuf[100] = {0};

    tr_stop_timer(TRACK_CUST_GT300S_MODE_INIT_SW_TIMER_ID);

    if (cmd_parameters_count != 16)
    {
        LOGD(L_OBD, L_V1, "单片机软件版本错误");
        return;
    }

    for (i = 0; i < cmd_parameters_count; i++)
    {
        if (i >= 2 && i < cmd_parameters_count - 1)
        {
            sum += atoi(cmd_parameters[i]);

            p = strchr(cmd_parameters[i], ':');
            while (p != NULL)
            {
                if (q != NULL)
                {
                    sum += atoi(q);
                    LOGD(L_OBD, L_V6, "sum=%d",  sum);
                }

                q = p + 1;
                p = strchr(q, ':');
                if (p == NULL)
                {
                    sum += atoi(q);
                    LOGD(L_OBD, L_V6, "sum=%d",  sum);
                }
            }
        }
        LOGD(L_OBD, L_V6, "[%d]%s -> %d", i, cmd_parameters[i], sum);
    }

    if (sum != atoi(cmd_parameters[cmd_parameters_count - 1]))
    {
        LOGD(L_OBD, L_V1, "校验和错误 %d,%d", sum, atoi(cmd_parameters[cmd_parameters_count - 1]));
        return;
    }
#if defined (__MCU_SW_UPDATE__)
    memset(mcu_sw_version_string, 0x00, sizeof(mcu_sw_version_string));
    snprintf(mcu_sw_version_string, sizeof(mcu_sw_version_string) - 1, "%s", cmd_parameters[1]);
#endif

    snprintf(tmp, 150, "version:%s,vBat:%s,time:%s\r\nmode:%s,%s; start:%s;upgrade:%s\r\nkey:%s,%s,%s;status:%s,%s,%s,%s",
             cmd_parameters[1], cmd_parameters[2], cmd_parameters[3], cmd_parameters[4], cmd_parameters[5], cmd_parameters[6], cmd_parameters[7],
             cmd_parameters[8], cmd_parameters[9], cmd_parameters[10], cmd_parameters[11], cmd_parameters[12], cmd_parameters[13], cmd_parameters[14]);

    confirm(tmp);
    mode = atoi(cmd_parameters[4]);
    timer = atoi(cmd_parameters[5]);
    datetime = atoi(cmd_parameters[6]);
    update_time = atoi(cmd_parameters[7]);
    pwrlimit_sw = atoi(cmd_parameters[8]);
    open_shell_alarm = atoi(cmd_parameters[9]);
    teardown_str = atoi(cmd_parameters[10]);

    chr_in = atoi(cmd_parameters[12]);
    track_cust_charger_status_updata_from_mcu(chr_in);

    if (flg == 0)
    {
        flg = 1;
        snprintf(singlebuf, 99, "ver:%s,bat:%s,timer:%s,mode:%s,type:%d\n\r", cmd_parameters[1], cmd_parameters[2], cmd_parameters[3], cmd_parameters[4], mmi_Get_PowerOn_Type());
        //track_cust_save_logfile(singlebuf, strlen(singlebuf));
    }
    if (mode == 0) mode++;

    LOGD(L_APP, L_V5, "mode:%d,work mode:%d,mode3 timer:%d,work datetime:[%d,%d]", mode, G_parameter.work_mode.mode, G_parameter.work_mode.mode3_timer, \
         G_parameter.work_mode.datetime, datetime);

    if ((mode == 1) && (G_parameter.work_mode.mode != 1 && G_parameter.work_mode.mode != 2))
    {
        LOGD(L_OBD, L_V1, "模式1 不一致,%d,%d,%d", G_parameter.work_mode.mode1_timer, timer, G_parameter.work_mode.mode);
        track_cust_module_sw_mode_to_mcu();
    }
    else if (mode == 2)
    {
        if (G_parameter.work_mode.mode3_timer != 0 && G_parameter.work_mode.mode3_timer != timer || G_parameter.work_mode.mode != 3)
        {
            LOGD(L_OBD, L_V1, "模式2 不一致,%d,%d,%d", G_parameter.work_mode.mode3_timer, timer, G_parameter.work_mode.mode);
            track_cust_module_sw_mode_to_mcu();
            track_cust_get_mode_status(0);
        }
        else
        {
            track_cust_get_mode_status(1);
            LOGD(L_OBD, L_V1, "模式参数一致");
            track_cust_charge_switch_mode(chr_in);
        }
    }
    if (datetime != G_parameter.work_mode.datetime && mode == 2)
    {
        LOGD(L_OBD, L_V1, "起始时间不一%d,%d", datetime, G_parameter.work_mode.datetime);
        track_cust_module_sw_mode_to_mcu();
        track_cust_get_mode_status(0);
    }
    else
    {
        LOGD(L_OBD, L_V1, "起始时间一致");
    }

    track_cust_key_mode_check(pwrlimit_sw, open_shell_alarm, teardown_str);

    track_cust_module_gt300s_auto_sw_mode();//追踪模式
    if (G_realtime_data.switch_down != atoi(cmd_parameters[10]))
    {
        G_realtime_data.switch_down = atoi(cmd_parameters[10]);
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        //track_cust_upload_param();
    }
}

static void cmd_02(void)
{
    U8 buf[100] = {0};
    LOGD(L_OBD, L_V5, "");
    confirm("02 OK!");
    if (!g_02_ok) return;
    track_cust_module_sw_mode_to_mcu();
}

static void cmd_03(void)
{
    LOGD(L_OBD, L_V5, "");
    confirm("03 OK!");
    track_drv_sys_power_off_req();
}
void cmd_04(void)
{
    LOGD(L_OBD, L_V5, "");
    confirm("04 OK!");
    track_stop_timer(TRACK_CUST_GT300S_MODE_AUTO_SW_TIMER_ID);
    track_cust_get_mode_status(1);
}


static void cmd_05(void)
{
    U8 buf[100] = {0};

    if (track_is_testmode())
    {
        if (strcmp(cmd_parameters[2], "03") == 0)
        {
            snprintf(buf, 100, "KEY=05,%s,02#", cmd_parameters[1]);//单片机嫌的无聊，强制为测试改
        }
        else
        {
            snprintf(buf, 100, "KEY=05,%s,%s", cmd_parameters[1], cmd_parameters[2]);
        }
    }
    else
    {
        snprintf(buf, 100, "KEY=05,%s,%s", cmd_parameters[1], cmd_parameters[2]);
    }
    confirm(buf);

    sprintf(buf, "AT^GT_CM=05,OK#\r\n");
    track_spi_write(buf, strlen(buf));

#if 0
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        return;
    }
#endif

    if (strcmp(cmd_parameters[2], "01") == 0)  //按下
    {
        if (strcmp(cmd_parameters[1], "02") == 0) //开机键
        {
            if (track_is_timer_run(TRACK_CUST_POWERON_LED_TIME_ID))
            {
                LOGD(L_OBD, L_V5, "开机LED 3分钟");
            }
            else
            {
                LOGD(L_OBD, L_V5, "LED 1分钟");
                track_cust_simple_gsm_led_resume();
                tr_start_timer(TRACK_CUST_LED_WAKEUP_TIMER, 60000, track_cust_simple_gsm_led_sleep);
            }
        }
        else if (strcmp(cmd_parameters[1], "04") == 0) //插入充电器
        {
            track_cust_charger_status_updata_from_mcu(1);
            track_cust_charge_switch_mode(1);
        }
    }
    else if (strcmp(cmd_parameters[2], "02") == 0)  //弹起
    {
        if (strcmp(cmd_parameters[1], "04") == 0) //拔出充电器
        {
            track_cust_charger_status_updata_from_mcu(0);
            track_cust_charge_switch_mode(0);
        }
    }
    else if (strcmp(cmd_parameters[2], "03") == 0)   //报警
    {
        if (strcmp(cmd_parameters[1], "01") == 0) //防拆报警
        {
            track_cust_alarm_teardown();//REMALM
        }
        else if (strcmp(cmd_parameters[1], "03") == 0) //开盖报警
        {
            track_cust_alarm_openshell();//REMALMF
        }
    }
}

static void cmd_06(void)
{
    U8 buf[32] = {0};
    snprintf(buf, 32, "KEY=06,%s", cmd_parameters[1]);
    confirm(buf);

    sprintf(buf, "AT^GT_CM=06,OK#\r\n");
    track_spi_write(buf, strlen(buf));

    if (strcmp(cmd_parameters[1], "01") == 0)
    {
        if (track_is_timer_run(TRACK_CUST_PWROFFALM_TIMER_ID))
        {
            tr_stop_timer(TRACK_CUST_PWROFFALM_TIMER_ID);
        }
    }
    else if (strcmp(cmd_parameters[1], "02") == 0)
    {
        LOGD(L_OBD, L_V3, "准备关机");
        track_cust_receive_power_off_msg();
    }
}

/******************************************************************************
 *  Function    -  track_send_07
 *
 *  Purpose     -  GSM在线升级中,告知MCU
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-03-22,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_send_07(void)
{
    U8 buf[100] = {0};
    static kal_uint8 cnt = 0;
    LOGD(L_OBD, L_V5, "");
    sprintf(buf, "AT^GT_CM=07,OK#");
    track_spi_write(buf, strlen(buf));
    if (cnt < 3)
    {
        cnt++;
        tr_start_timer(TRACK_CUST_07_TIMEOUT_TIMER, 1000, track_send_07);
    }
}

static void cmd_07(void)
{
    LOGD(L_OBD, L_V5, "");
    confirm("07 OK!");
    tr_stop_timer(TRACK_CUST_07_TIMEOUT_TIMER);
}

static void cmd_08(void)
{
    U8 buf[100] = {0};
    LOGD(L_OBD, L_V5, "");
    confirm("08 OK!");

    sprintf(buf, "AT^GT_CM=08,OK#");
    track_spi_write(buf, strlen(buf));
}

static void cmd_10(void)
{
    U8 buf[100] = {0};
    LOGD(L_OBD, L_V5, "");
    confirm("10 OK!");

    sprintf(buf, "AT^GT_CM=10,OK#");
    track_spi_write(buf, strlen(buf));
}

static void cmd_14(void)
{
    LOGD(L_OBD, L_V5, "");
    confirm("14 OK!");

}

static void cmd_15(void)
{
    LOGD(L_OBD, L_V5, "");
    confirm("15 OK!");
}

static void cmd_99(void)
{
#if defined (__MCU_SW_UPDATE__)
    track_drv_mcu_sw_update_rsp();
#endif
    LOGD(L_OBD, L_V5, "");
    confirm("99 OK!");
}

static cust_recv_struct CMD_Data[] =
{
    {"01",      cmd_01},
    {"02",      cmd_02},
    {"03",      cmd_03},
    {"04",      cmd_04},
    {"05",      cmd_05},
    {"06",      cmd_06},
    {"07",      cmd_07},
    {"08",      cmd_08},
    {"10",      cmd_10},
    {"14",      cmd_14},
    {"15",      cmd_15},
    {"99",      cmd_99},

};

void track_cust_module_power_cmd(cmd_return_struct *return_sms, kal_uint8 *data, int len)
{
    memcpy(&g_return_sms, return_sms, sizeof(cmd_return_struct));
    g_return_sms_valid = 1;
    track_spi_write(data, len);
}

void track_cust_module_extchip_time_synchronization(kal_uint32 arg)
{
    LOGD(L_OBD, L_V5, "%d", arg);
    if (!g_02_ok)
    {
        applib_time_struct time = {0};
        g_02_ok = KAL_TRUE;
        track_fun_get_time(&time, KAL_FALSE, &G_parameter.zone);
        LOGD(L_OBD, L_V5, "time: %02d-%02d-%02d %02d:%02d:%02d",
             time.nYear,
             time.nMonth,
             time.nDay,
             time.nHour,
             time.nMin,
             time.nSec);
        if (time.nYear >= 2016)
        {
            U8 buf[100] = {0}, valid = 0;

            sprintf(buf, "AT^GT_CM=02,%02d,%02d,%02d,%02d,%02d#", time.nYear - 2000, time.nMonth, time.nDay, time.nHour, time.nMin);
            track_spi_write(buf, strlen(buf));

            if (G_realtime_data.tracking_time < 1422748800 && G_realtime_data.tracking_time + arg >= 1422748800)
            {
                G_realtime_data.tracking_time += arg;
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
            }
            track_cust_module_gt300s_auto_sw_mode();
        }
    }
}

void track_cust_module_Handshake(void)
{
    char buf[100] = {0};
    sprintf(buf, "AT^GT_CM=08#");
    track_spi_write(buf, strlen(buf));
    LOGD(L_OBD, L_V5, "");
    tr_start_timer(TRACK_CUST_EXTCHIP_BATTERY_VOLTAGE_TIMER_ID, 5000, track_cust_module_Handshake);
}



void track_cust_spi_read_data(kal_uint8 *data, kal_uint16 len)
{
    char* p;
    int size, i, ret;
    LOGD(L_DRV, L_V4, "单片机信息:%s", data);
    if (G_realtime_data.netLogControl & 1)
    {
        cust_packet_log_data(1, data, len);
    }

    p = strstr(data, "GT_CM=");
    if (p != NULL)
    {
        cmd_parameters_count = track_fun_str_analyse(data + 9, cmd_parameters, PARAMETER_MAX, NULL, "#", ',');
        size = sizeof(CMD_Data) / sizeof(cust_recv_struct);
        LOGD(L_OBD, L_V4, "part=%d", cmd_parameters_count);

        for (i = 0; i < size; i++)
        {
            ret = strcmp(cmd_parameters[0], CMD_Data[i].cmd_string);
            if (!ret)
            {
                LOGD(L_OBD, L_V5, "Tab[%d], cmd=%s", i, CMD_Data[i].cmd_string);
                if (CMD_Data[i].func_ptr != NULL)
                {
                    CMD_Data[i].func_ptr();
                }
                break;
            }
        }
    }
}

//370/380
void track_cust_module_power_data(kal_uint8 *data, int len)
{
    track_cust_spi_read_data(data, len);
}

void track_spi_write(kal_uint8 *data, kal_uint8 len)
{
    char at_content[128] = {0};
    memcpy(&at_content[0], data, len);
    at_content[len] = 0x0d;
    at_content[len + 1] = 0x0a;
    track_drv_write_exmode(at_content, len + 2);
    //track_drv_write_exmode("\r\n", 2);
}


void track_cust_poweroff_ready()
{
    char buf[100] = {0};

    track_cust_restart(80, 10); //本来就没打算重启，是关机前数据处理，单片机会关机的
    sprintf(buf, "AT^GT_CM=03,2#");
    track_spi_write(buf, strlen(buf));
}


#if 1
//mode-1
//G_parameter.work_mode.mode
//G_parameter.gps_work.Fti.interval_acc_on = chip.mode1_timer
//G_parameter.gps_work.Fti.interval_acc_off
//mode-2
//G_parameter.work_mode.mode
//G_parameter.work_mode.datetime / 60,  G_parameter.work_mode.datetime % 60
//G_parameter.work_mode.mode3_timer
/******************************************************************************
 *  Function    -  track_cust_charge_switch_mode
 *
 *  Purpose     -  充电是进需求里的模式1，拔出充电器后回到需求里的模式2
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-04-20,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_charge_switch_mode(kal_uint8 chr_in)
{
    nvram_work_mode_struct auto_mode1 = {0};

    if (chr_in)
    {
        if (track_cust_get_work_mode() == 3)
        {
            LOGD(L_APP, L_V4, "充电临时切入模式1");

            G_parameter.pre_mode.t1 = G_parameter.work_mode.datetime;
            G_parameter.pre_mode.t2 = G_parameter.work_mode.mode3_timer;
            G_parameter.pre_mode.mode = 3;

            G_parameter.gps_work.Fti.interval_acc_on = 10 ;
            G_parameter.gps_work.Fti.interval_acc_off = 3600 ;
            G_parameter.work_mode.mode1_timer =	G_parameter.gps_work.Fti.interval_acc_on;
            //G_parameter.gps_work.Fti.interval_acc_on > 120
            //G_parameter.work_mode.mode = 1;
            //G_parameter.gps_work.Fti.interval_acc_on < 120
            G_parameter.work_mode.mode = 2;
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);

            track_cust_module_sw_init();
            if (G_parameter.work_mode.mode != 3)
            {
                track_stop_timer(TRACK_POWER_RESET_TIMER_ID);
            }
            track_cust_wake_mode();
        }
    }
    else
    {
        if (G_parameter.pre_mode.mode == 3)
        {
            LOGD(L_APP, L_V4, "拔出充电器，回到以前的模式");

            G_parameter.work_mode.mode = 3;
            G_parameter.work_mode.datetime = G_parameter.pre_mode.t1;
            G_parameter.work_mode.mode3_timer = G_parameter.pre_mode.t2;
            G_parameter.pre_mode.mode = 0;
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);

            track_cust_module_sw_init();
            track_cust_wake_mode();
        }
    }
}
#endif

#endif /*__GT740__*/


#if defined (__MT200__) && defined (__CUST_ACC_BY_MCU__)
void track_drv_acc_by_mcu_read(void);
void track_drv_acc_by_mcu_merge(void* flag);
void track_drv_set_protection_voltage(void);
#if defined(__NT51__)
void track_cust_get_mcu_voltage(kal_uint32 exvolt);
void track_drv_mcu_and_gsm_sleep(void * op);
#endif
extern void acc_status_change(void * arg);

void track_drv_acc_by_mcu_send(kal_uint8* data, kal_uint16 len)
{
    track_drv_mcu_and_gsm_sleep((void*)1);
    track_drv_write_exmode(data, len);
}

kal_uint16 track_cust_read_mcu_extBat_time(void)
{
    kal_uint16 interval=G_parameter.info_up.time;

    if (interval>1200)
    {
        interval=1200;
    }
    else if (interval<120)
    {
        interval=120;
    }

    interval=interval>>1;

    return interval;
}


static kal_uint8 mcu_version[4]={0};
kal_uint8* track_drv_get_mcu_version(void)
{
    return mcu_version;
}

kal_uint8 track_cmd_read_adc_result_flag(kal_uint8 flag)
{
    static kal_uint8 output=0;
    if ((flag==0)||(flag==1))
    {
        output=flag;
    }
    return output;
}

/******************************************************************************
 *  Function    -  track_drv_acc_by_mcu_rece
 *
 *  Purpose     -  接收单片机的数据处理
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-19,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_acc_by_mcu_rece(kal_uint8* buff, kal_uint16 len)
{
    /*只允许检测和发送消息*/
    kal_uint8* p=NULL;
    kal_uint8 data[10]={0};

    static kal_uint8 c0=0,c1=0,c2=0;//test

    if ((buff == NULL)||(len ==0))
    {
        return;
    }

    if (len>9)
    {
        len=9;
    }
    memcpy(data, buff, len);

    LOGH(L_DRV, L_V4, data, len);

    if (strstr(data, "AONN")!=NULL) //点火
    {
        track_drv_acc_by_mcu_send("AONN\r\n",6);
        track_start_timer(TRACK_CUST_ACC_BY_MCU_DELAY_MERGE_TIMER_ID, 3 * 1000, track_drv_acc_by_mcu_merge, (void*)1);

        c0++;
        LOGD(L_DRV, L_V1, "点火 %d",c0);
        track_os_intoTaskMsgQueueExt(acc_status_change,(void*)1);
    }
    else if (strstr(data, "AOFF")!=NULL) //熄火
    {
        track_drv_acc_by_mcu_send("AOFF\r\n",6);
        track_start_timer(TRACK_CUST_ACC_BY_MCU_DELAY_MERGE_TIMER_ID, 3 * 1000, track_drv_acc_by_mcu_merge, (void*)0);

        c1++;
        LOGD(L_DRV, L_V1, "熄火 %d",c1);
        track_os_intoTaskMsgQueueExt(acc_status_change,(void*)0);
    }
    else if (strstr(data, "PTON")!=NULL) //单片机说我要断电关你啦
    {
        track_drv_acc_by_mcu_send("PTON\r\n",6);

        c2++;
        LOGD(L_DRV, L_V1, "低电保护 %d",c2);
        //单片机关闭外电，GSM要得处理报警和断电
    }
    else if (strstr(data, "VR")!=NULL) //单片机软件版本
    {
        p=strstr(data, "VR");
        p+=2;

        LOGD(L_DRV, L_V1, "version:%c.%c",*p,*(p+1));
        mcu_version[0]=(*p);
        mcu_version[1]=(*(p + 1));
        track_drv_set_protection_voltage();//设置低电保护参数
    }
    else if (strstr(data, "VT")!=NULL) //单片机检测到的外电电压
    {
        float v_ext;
        kal_uint32 mcu_extVolt;
        p=strstr(data, "VT");
        p+=2;

        v_ext=(((*p)<<8) | (*(p+1)))*11.0/2048.0;
        mcu_extVolt=v_ext*1000000;
        track_cust_get_mcu_voltage(mcu_extVolt);

        if (track_cmd_read_adc_result_flag(0xFF)==1)
        {
            track_cmd_read_adc_result_flag(0);
            LOGS("MCU vExt:%0.2fV", v_ext);
        }
        LOGD(L_DRV, L_V1, "v_ext:%f",v_ext);
    }
    else if (strstr(data, "ELOW") != NULL) //强制断外电
    {
        LOGT("ELOW");
        track_drv_acc_by_mcu_send("GTVT\r\n",6);//强制关闭外电后立刻主动查外电电压
    }
    else if (strstr(data, "EHIG") != NULL) //强制接通外电
    {
        LOGT("EHIG");
        track_drv_acc_by_mcu_send("GTVT\r\n",6);//强制关闭外电后立刻主动查外电电压
    }
    else if (data[0]==0x53) //外电低电保护电压和回复电压
    {
        float v1,v2;
        v1=((data[1]<<8) | data[2])*11.0/2048.0;
        v2=((data[3]<<8) | data[4])*11.0/2048.0;
        LOGT("PRO_VOLT=%fV,%fV", v1, v2);
    }
    else
    {
        /*有时收到 0x00       --    chengjun  2017-10-19*/
        LOGD(L_DRV, L_V1, "%s","error");
    }
}


void track_drv_set_protection_voltage(void)
{
    kal_uint16 adc_l, adc_h; //低电保护和恢复电压的ADC值
    kal_uint8 buff[8] = {0};

    //adc=2048*volt/11
    //adc_l = 2048 * 11.5 / 11;
    //adc_h = 2048 * 12 / 11;

    adc_l=G_parameter.power_extbatcut.threshold * 18.618182;
    adc_h=G_parameter.power_extbatcut.recover_threshold * 18.618182;

    buff[0]=0x53;
    buff[1]=(adc_l>>8)&0xFF;
    buff[2]=(adc_l&0xFF);
    buff[3]=(adc_h>>8)&0xFF;
    buff[4]=(adc_h&0xFF);
    buff[5]=0x0D;
    track_drv_acc_by_mcu_send(buff,6);
}

void track_drv_acc_by_mcu_read(void)
{
    static kal_uint8 i=0;

    if (i<2)
    {
        i++;
        tr_start_timer(TRACK_CUST_ACC_BY_MCU_TIMER_ID, 2*1000, track_drv_acc_by_mcu_read);
        track_drv_acc_by_mcu_send("GTVR\r\n",6);//版本
        return;
    }

    if (i<12)
    {
        i++;
        tr_start_timer(TRACK_CUST_ACC_BY_MCU_TIMER_ID, 5000, track_drv_acc_by_mcu_read);
        track_drv_acc_by_mcu_send("GTVT\r\n",6);//读单片机检测的外电电压
    }
    else
    {
        tr_start_timer(TRACK_CUST_ACC_BY_MCU_TIMER_ID, track_cust_read_mcu_extBat_time()*1000, track_drv_acc_by_mcu_read);
        track_drv_acc_by_mcu_send("GTVT\r\n",6);//读单片机检测的外电电压
    }
}


void track_drv_mcu_and_gsm_sleep(void * op)
{
    kal_uint32 level = (kal_uint32)op;

    LOGD(L_APP, L_V5, "MCU唤醒GSM两秒后休眠 %d", level);
    if (track_is_timer_run(TRACK_DRV_MCU_WAKEUP_GSM_TIMER_ID))
    {
        tr_stop_timer(TRACK_DRV_MCU_WAKEUP_GSM_TIMER_ID);
    }
    if (1 == level)
    {
        track_drv_sleep_enable(SLEEP_EXT_UART_MOD, KAL_FALSE);
        track_start_timer(TRACK_DRV_MCU_WAKEUP_GSM_TIMER_ID, 5 * 1000, track_drv_mcu_and_gsm_sleep, (void*)0xff);
    }
    else if (0 != level)
    {
        track_drv_sleep_enable(SLEEP_EXT_UART_MOD, KAL_TRUE);
    }
}


void track_drv_acc_by_mcu_merge(void* acc_flag)
{
    kal_uint32 flag = (kal_uint32)acc_flag;

    kal_uint8 mcu_acc=(kal_uint8)flag;
    kal_uint8 gsm_acc=(kal_uint8)track_cust_status_acc();
    kal_uint8 byte_index,h_inc;
    kal_uint8 acc_merge=0x08;

    LOGD(L_DRV, L_V1, "ACC状态 MCU:%d,GSM:%d",mcu_acc,gsm_acc);

    //一个字节内2组值，先使用高4位
    //[0]-7..4
    //[0]-3..0
    //[1]-7..4
    //[1]-3..0
    //count=4

    if (G_parameter.gsm_mcu_acc_merge.count>=128)
    {
        LOGD(L_DRV, L_V1, "ACC状态缓存满");
        return;
    }

    byte_index=G_parameter.gsm_mcu_acc_merge.count/2;
    h_inc=(G_parameter.gsm_mcu_acc_merge.count&0x01);

    acc_merge=(acc_merge|(gsm_acc<<2));
    acc_merge=(acc_merge|(mcu_acc<<1));
    if (h_inc==0)
    {
        acc_merge=(acc_merge<<4);
    }

    G_parameter.gsm_mcu_acc_merge.acc[byte_index]=acc_merge|G_parameter.gsm_mcu_acc_merge.acc[byte_index];

    LOGD(L_DRV, L_V1, "%d,0x%02X,0x%02X",byte_index,acc_merge,G_parameter.gsm_mcu_acc_merge.acc[byte_index]);

    G_parameter.gsm_mcu_acc_merge.count++;

    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
}

void track_cust_gsm_mcu_acc_merge_to_ams(void)
{
    kal_uint8 len;

    if (G_parameter.gsm_mcu_acc_merge.count==0)
    {
        return;
    }

    len=((G_parameter.gsm_mcu_acc_merge.count+1)>>1);
    LOGD(L_APP, L_V5, "acc to AMS %d",len);

    track_cust_paket_77(G_parameter.gsm_mcu_acc_merge.acc, len, 0x07);

    memset(&G_parameter.gsm_mcu_acc_merge, 0, sizeof(nvram_gsm_mcu_acc_merge_struct));
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
}

static void track_cust_get_mcu_voltage(kal_uint32 exvolt)
{
    track_drv_set_mcu_external_voltage(exvolt);
    trak_cust_mcuextvolt_convey(exvolt);
}
#endif /*__CUST_ACC_BY_MCU__*/
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
kal_uint8 track_cust_mlsp_get_boot_reason(void)
{
    //0xFF:未获取;1:上电;2:按键;3:定时;4:充电开机;0x10:震动报警;0x11:电池低电;0x12:外低电;0x13:外断电;
    return Boot_Reason;
}
kal_uint8 track_cust_mlsp_get_charge_status(void)
{
    //0xFF:未获取;0:无外电;1:充电中;2:有外电但未充电
    return Charge_Status;
}
kal_uint16 track_cust_mlsp_get_bat(void)
{
    //电池电压×1000;0xFFFF:未获取
    return Battery_Volt;
}
U16 track_cust_mlsp_get_gsmboot_times(void)
{
    return Mcu_Boot_GSM_Times;
}
kal_uint16 track_cust_mlsp_get_exvolt(void)
{
    //外电电压×1000;0xFFFF:未获取
    return Ext_Volt;
}
kal_uint8* track_cust_mlsp_get_mcu_version(void)
{
    //返回MCU版本
    return Mcu_Version;
}
applib_time_struct* track_cust_mlsp_get_mcu_time(void)
{
    //返回MCU时间
    return &Mcu_Time;
}
/******************************************************************************
 *  Function    -  track_cust_get_weekday_from_date
 *  Description -  从时间换算出星期几
 ******************************************************************************/
kal_uint8 track_cust_get_weekday_from_date(applib_time_struct* time)
{
    kal_uint32 standard_sec = 0, target_sec = 0;
    kal_uint16 day = 0;
    static applib_time_struct standard = {2014, 12, 28, 0, 0, 1, 0};/*2014/12/28 00:00:01 Sun*/
    standard_sec = applib_dt_mytime_2_utc_sec(&standard, 0);
    target_sec = applib_dt_mytime_2_utc_sec(time, 0);
    day = (target_sec - standard_sec) / 86400;
    time->DayIndex = day % 7;
    LOGD(L_APP, L_V1, "##standard_sec:%d,target_sec:%d,day:%d,DayIndex:%d##", standard_sec, target_sec, day, time->DayIndex);
}

/******************************************************************************
 *  Function    -  CRC8
 *  Description - 返回1字节的CRC
 ******************************************************************************/
static unsigned char CRC8(unsigned char *ptr,unsigned char len)
{
    unsigned char crc;
    unsigned char i = 0;
    crc = 0;
    while (len--)
    {
        crc ^= *ptr++;
        for (i = 0; i < 8; i++)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ 0x07;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}
/******************************************************************************
 *  Function    -  track_cust_mlsp_completion/track_cust_mlsp_packet_send
 *  Description -  对要发送到MCU的数据添加头尾CRC,并发送
 * ----------------------------------------
 * v1.0  , 2017-08-15,  liwen  written
 * ----------------------------------------
 ******************************************************************************/
int track_cust_mlsp_completion(int no,unsigned char* data,int data_len)
{
    unsigned char crc;
    data[0] = 0x43;
    data[1] = 0x4A;
    data[2] = data_len+7;
    data[3] = no;
    crc = CRC8(data,data_len+4);
    data[data_len+4] = crc;
    data[data_len+4 + 1] = 0x0D;
    data[data_len+4 + 2] = 0x0A;
    return data_len+7;
}
static void track_cust_mlsp_packet_send(int no,unsigned char* data,int data_len,kal_bool is_vector)
{
    kal_uint8 packet_data[MSLP_MAX_LEN];
    int len = 0;

    if (data!=NULL)
        memcpy(packet_data+4, data, data_len);

    len=track_cust_mlsp_completion(no,packet_data,data_len);
    if (is_vector)//通过队列
        track_mlsp_queue_send_req(packet_data,len);
    else//不通过队列(适用于回复单片机时)
        track_drv_write_exmode(packet_data, len);
}
/******************************************************************************
 *  Function    -
 *  Description -  协议功能函数
 * ----------------------------------------
 * v1.0  , 2017-08-15,  liwen  written
 * ----------------------------------------
 ******************************************************************************/
/*************************************************************
 *  Function    -  track_cust_mlsp_test
 *  Description -  单片机进入测试模式
 *************************************************************/
void track_cust_mlsp_test(void)
{
    LOGD(L_OBD, L_V1, "单片机进入测试模式");
    track_cust_mlsp_packet_send(MLSP_MCU_TEST,NULL,0,0);
}
/*************************************************************
 *  Function    -  track_cust_mlsp_boot
 *  Description -  开机参数同步
 *************************************************************/
void track_cust_mlsp_boot(void)
{
    LOGD(L_OBD, L_V1, "参数同步");
    track_cust_mlsp_packet_send(MLSP_BOOT_SYN,NULL,0,1);
}
/*************************************************************
 *  Function    -  track_cust_mlsp_set_time
 *  Description -  设置MCU时间
 *  参数为NULL时自动使用GSM系统时间设置
 *************************************************************/
void track_cust_mlsp_set_time(applib_time_struct* set_time)
{
    applib_time_struct time = {0};
    applib_time_struct* time_p;
    kal_uint8 data[6] = {0};//年月日时分秒
    int i = 0;
    if (set_time != NULL)
    {
        time_p = set_time;
    }
    else
    {
#if defined(__GT740__)||defined(__GT420D__)
        track_fun_get_time(&time, KAL_FALSE, &G_parameter.zone);
#else
        track_fun_get_time(&time,1,NULL);
#endif//__GT740__
        time_p = &time;
    }
    LOGD(L_OBD, L_V1, "设置MCU时间%d:%d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", (set_time == NULL?0:1)
         , time_p->nYear, time_p->nMonth, time_p->nDay, time_p->nHour, time_p->nMin, time_p->nSec);
    Set_Mcu_Time.nYear = data[i++] = time_p->nYear-2000;
    Set_Mcu_Time.nMonth = data[i++] = time_p->nMonth;
    Set_Mcu_Time.nDay = data[i++] = time_p->nDay;
    Set_Mcu_Time.nHour = data[i++] = time_p->nHour;
    Set_Mcu_Time.nMin = data[i++] = time_p->nMin;
    Set_Mcu_Time.nSec = data[i++] = time_p->nSec;
    track_cust_mlsp_packet_send(MLSP_SET_TIME, data, i, 1);
}
/************************************************************
 *  Function    -  track_cust_mlsp_set_nextboot_time
 *  Description -   设置下一次开机时间
 ************************************************************/
void track_cust_mlsp_set_nextboot_time(applib_time_struct* time)
{
    kal_uint8 data[6] = {0};//年月日时分秒
    int i = 0;
#if defined(__GT740__)||defined(__GT420D__)
    applib_time_struct current_time = {0};
    kal_uint32 current_sec, nextboot_sec, ams_sec;
    track_fun_get_time(&current_time, KAL_FALSE, &G_parameter.zone);
    current_sec = applib_dt_mytime_2_utc_sec(&current_time, 0);
    nextboot_sec = applib_dt_mytime_2_utc_sec(time, 0);
    if ((nextboot_sec - current_sec) >= 259200)//下次开机时间距离当前超过3天
    {
        ams_sec = current_sec + (1440 - (current_time.nHour * 60 + current_time.nMin) + 2880 + 528) * 60;//528→8:48
        if (G_realtime_data.assist_sw)
        {
            ams_sec = current_sec + 600;
        }
        applib_dt_utc_sec_2_mytime(ams_sec, time, 0);
        if (G_realtime_data.next_boot_is_AMS != 1)
        {
            G_realtime_data.next_boot_is_AMS = 1;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        }
        LOGD(L_OBD, L_V1, "下次开机为AMS唤醒%d,%d,%d", current_sec, nextboot_sec, ams_sec);
    }
    else
    {
        if (G_realtime_data.next_boot_is_AMS != 0)
        {
            G_realtime_data.next_boot_is_AMS = 0;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        }
    }
#endif//__GT740__
    Next_Boot_Time.nYear = data[i++]= time->nYear - 2000;
    Next_Boot_Time.nMonth = data[i++]= time->nMonth;
    Next_Boot_Time.nDay = data[i++]= time->nDay;
    Next_Boot_Time.nHour = data[i++]= time->nHour;
    Next_Boot_Time.nMin = data[i++]= time->nMin;
    Next_Boot_Time.nSec = data[i++]= time->nSec;
    LOGD(L_OBD, L_V1, "设置开机时间:%0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
         data[0], data[1], data[2], data[3], data[4],data[5]);
    track_cust_mlsp_packet_send(MLSP_SET_BOOT_TIME,data,i,1);
}
/***********************************************************
 *  Function    -  track_cust_mlsp_shutdown_req
 *  Description -  请求关机
 ***********************************************************/
void track_cust_mlsp_shutdown_req(void)
{
    LOGD(L_OBD, L_V1, "请求关机");
#if !defined(__GT740__)||!defined(__GT420D__)
    if (!track_is_testmode() && !track_is_autotestmode())
    {
        track_soc_queue_all_to_nvram();//
        track_cust_backup_gps_data(4, NULL);
    }
#endif//__GT740__
    track_cust_mlsp_packet_send(MLSP_SHUTDOWN_REQ,NULL,0,1);
}
/***********************************************************
 *  Function    -  track_cust_mlsp_write_backup_req
 *  Description - 写备份
 ***********************************************************/
void track_cust_mlsp_write_backup_req(kal_uint8* write_data,int len)
{
    kal_uint8 tmp[BACKUP_LEN + 1] = {0};

    if (write_data!=NULL && len <= BACKUP_LEN)
    {
        memset(Back_UP_Data_Write, 0x00, BACKUP_LEN);
        memcpy(Back_UP_Data_Write,write_data,len);
        memcpy(tmp+1,Back_UP_Data_Write,BACKUP_LEN);
        tmp[0]=0x01;
        track_cust_mlsp_packet_send(MLSP_BACKUP_REQ,tmp,BACKUP_LEN + 1,1);
    }
    else
        LOGD(L_OBD, L_V1, "ERROR,%d",len);
}
/***********************************************************
 *  Function    -  track_cust_mlsp_read_backup_req
 *  Description - 读备份
 **********************************************************/
void track_cust_mlsp_read_backup_req(void)
{
    kal_uint8 tmp[1];
    LOGD(L_OBD, L_V1, "");
    tmp[0]=0x00;
    track_cust_mlsp_packet_send(MLSP_BACKUP_REQ,tmp,1,1);
}
/*********************************************************
 *  Function    -  track_cust_mlsp_set_sensor
 *  Description - 设置GSENSOR参数
 ********************************************************/
void track_cust_mlsp_set_sensor(kal_bool enable)
{
    kal_uint8 tmp[10] = {0};
    int i = 0;
    LOGD(L_OBD, L_V1, "");
    tmp[i++] = enable;//开关
    tmp[i++] = G_parameter.vibrates_alarm.sensitivity_level;//灵敏
    tmp[i++] = G_parameter.vibrates_alarm.detection_time;//检时
    tmp[i++] = G_parameter.gps_work.vibrates.detection_count;//开机和开GPS门限
    tmp[i++] = G_parameter.vibrates_alarm.detection_count;//触发报警门限
    track_cust_mlsp_packet_send(MLSP_SET_GSENSOR,tmp,i,1);
}
/********************************************************
 *  Function    -  track_cust_mlsp_get_peripherals_status
 *  Description - 读取MCU管理的外设的信息
 *  0x01:电源键0x02:防拆键0x10:充电器 0x20:传感器0x08:光敏
 *******************************************************/
void track_cust_mlsp_get_peripherals_status(kal_uint8 arg)
{
    kal_uint8 tmp[1];
    LOGD(L_OBD, L_V1, "%d", arg);
    tmp[0]=arg;
    track_cust_mlsp_packet_send(MLSP_GET_HW_STATUS,tmp,1,1);
}
/*******************************************************
 *  Function    -  track_cust_mlsp_get_par
 *  Description - 读取MCU内信息
 ******************************************************/
void track_cust_mlsp_get_par(void)
{
    kal_uint8 tmp[1];
    LOGD(L_OBD, L_V1, "");
    tmp[0]=0x01;//包序号
    track_cust_mlsp_packet_send(MLSP_GET_PAR,tmp,1,1);
}
/*******************************************************
 *  Function    -  track_cust_mlsp_set_device_enable
 *  Description - 设备使能设置
 ******************************************************/
void track_cust_mlsp_set_device_enable(kal_uint8 device_type, kal_bool enable)
{
    kal_uint8 tmp[2];
    int i = 0;
    LOGD(L_OBD, L_V1, "%d,%d", device_type, enable);
    tmp[i++]=device_type;
    tmp[i++]=enable;
    track_cust_mlsp_packet_send(MLSP_DEVICE_ENABLE,tmp,i,1);
}
/******************************************************************************
 *  Function    -  track_cust_mlsp_decode
 *  Description -  收到MCU发来的MLSP数据,解码和相关处理
 * ----------------------------------------
 * v1.0  , 2017-08-15,  liwen  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_mlsp_decode(kal_uint8 *data, int len)
{
    kal_uint8 crc_right = 0;
    static kal_bool is_First = TRUE;
    static kal_uint8 confirm_buf[200] = {0};

    if (len < 7)
    {
        LOGS("长度错误!%d",len);
        return;
    }
    LOGD(L_OBD, L_V1, "收到MLSP数据,协议号为:0x%02X",data[3]);
    crc_right = CRC8(data,len-3);
    LOGD(L_OBD, L_V6, "CRC_RIGHT:0x%02X,CRC_GET:0x%02X",crc_right,data[len-3]);
    if (crc_right!=data[len-3])
    {
        LOGS("CRC 错误!");
        return;
    }
    switch (data[3])
    {
        case MLSP_BOOT_SYN:
            Proticol_Version=data[4];
            Mcu_Time.nYear=data[5]+2000;
            Mcu_Time.nMonth=data[6];
            Mcu_Time.nDay=data[7];
            Mcu_Time.nHour=data[8];
            Mcu_Time.nMin=data[9];
            Mcu_Time.nSec=data[10];
            memcpy(Mcu_Version, data+11, 24);

#if defined (__MCU_SW_UPDATE__)
            memset(mcu_sw_version_string, 0, sizeof(mcu_sw_version_string));
            strncpy(mcu_sw_version_string, Mcu_Version, sizeof(mcu_sw_version_string));
#endif
            Boot_Reason=data[35];
            Battery_Volt=((*(data+36)&0x00FF )<< 8) + (*(data+37)&0x00FF);
#if defined(__GT740__)||defined(__GT420D__)
            Battery_Volt = Battery_Volt / 10;
#endif//__GT740__
            Ext_Volt=((*(data+38)&0x00FF )<< 8) + (*(data+39)&0x00FF);
            Charge_Status = data[40];
            Mcu_Boot_GSM_Times = ((*(data+41)&0x00FF )<< 8) + (*(data+42)&0x00FF);
            memset(confirm_buf, 0, sizeof(confirm_buf));
            snprintf(confirm_buf, sizeof(confirm_buf), "Boot_Reason:0x%02X\r\nProtocol_Version:0x%02X\r\nMcu_Time:%0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d\r\nMcu_Version:%s\r\nBat_Volt:%d,Ext_Volt:%d,Charge_Status:%d",
                     Boot_Reason, Proticol_Version,Mcu_Time.nYear,Mcu_Time.nMonth,Mcu_Time.nDay,Mcu_Time.nHour,Mcu_Time.nMin,Mcu_Time.nSec,Mcu_Version,Battery_Volt,Ext_Volt,Charge_Status);
            confirm(confirm_buf);
            track_mlsp_send((void*)MLSP_BOOT_SYN);
#if defined(__GT740__)||defined(__GT420D__)
            if (is_First)
            {
                is_First = FALSE;
                G_realtime_data.gsm_boot_times++;
                if (G_realtime_data.gsm_boot_times == 1)
                {
                    G_realtime_data.gsm_boot_times_offset = Mcu_Boot_GSM_Times - 1;
                }
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                if (Boot_Reason == 0x01)
                {
                    memset(&Mcu_Time, 0, sizeof(Mcu_Time));
                    Mcu_Time.nYear = 2015;
                    Mcu_Time.nMonth = 9;
                    Mcu_Time.nDay = 12;
                    track_cust_mlsp_set_time(&Mcu_Time);
                    G_parameter.extchip.mode2_start_year = 2015;
                    G_parameter.extchip.mode2_start_month = 6;
                    G_parameter.extchip.mode2_start_day = 6;
                }
                cmd_08();
                track_mlsp_GT740_boot_syn_decode(data);
                if (Boot_Reason == 0x14) cmd_05();
                track_mlsp_GT740_delay_force_shutdown();
            }
#endif//__GT740__
            break;
        case MLSP_SET_TIME:
            if (data[4]==1)//设置成功
            {
                confirm("设置时间成功");
                track_mlsp_send((void*)MLSP_SET_TIME);
#if defined(__GT740__)||defined(__GT420D__)
                memcpy(&Mcu_Time, &Set_Mcu_Time, sizeof(Set_Mcu_Time));
                Mcu_Time.nYear += 2000;
                track_cust_module_update_mode();
#endif//__GT740__
            }
            else
            {
                confirm("设置时间失败");
                track_mlsp_send((void*)0xFF);
            }
            break;
        case MLSP_SET_BOOT_TIME:
            if (data[4]==1)//设置成功
            {
                memset(confirm_buf, 0, sizeof(confirm_buf));
                snprintf(confirm_buf, sizeof(confirm_buf), "设置开机时间成功,MCU返回开机时间:%0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
                         data[5], data[6], data[7], data[8], data[9],data[10]);
                confirm(confirm_buf);
                track_mlsp_send((void*)MLSP_SET_BOOT_TIME);
            }
            else
            {
                confirm("设置开机时间失败");
                track_mlsp_send((void*)0xFF);
            }
            break;
        case MLSP_SHUTDOWN_REQ:
            if (data[4]==1)//请求成功
            {
                confirm("SHUTDOWN");
                track_mlsp_send((void*)MLSP_SHUTDOWN_REQ);
#if defined(__GT740__)||defined(__GT420D__)
                cmd_03();
#endif//__GT740__
            }
            else
            {
                confirm("请求关机失败");
                track_mlsp_send((void*)0xFF);
            }
            break;
        case MLSP_BACKUP_REQ:
            if (data[4] == 1)
            {
                if (0 == memcmp(data+5, Back_UP_Data_Write, BACKUP_LEN))
                {
                    confirm("写入备份成功");
                    track_mlsp_send((void*)MLSP_BACKUP_REQ);
                }
                else
                {
                    confirm("写入备份失败");
                    track_mlsp_send((void*)0xFF);
                }
            }
            else if (data[4] == 0)
            {
                confirm("读取备份成功");
                Back_UP_Data_Vaild = TRUE;
                memcpy(Back_UP_Data_Read,data+5,BACKUP_LEN);
                LOGH(L_OBD, L_V5, Back_UP_Data_Read, BACKUP_LEN);
                track_mlsp_send((void*)MLSP_BACKUP_REQ);
#if defined(__GT740__)||defined(__GT420D__)
#endif//__GT740__
            }
            break;
        case MLSP_SET_GSENSOR:
            if (data[4]==1)//设置成功
            {
                confirm("设置传感器成功");
                track_mlsp_send((void*)MLSP_SET_GSENSOR);
            }
            else
            {
                confirm("设置传感器失败");
                track_mlsp_send((void*)0xFF);
            }
            break;
        case MLSP_GET_HW_STATUS:
            if (data[4] == MLSP_DEVICE_GSENSOR)//返回的是传感器信息
            {
                Mcu_Sensor_Enable = data[5];
                Mcu_Sensor_Type = data[6];
                memset(confirm_buf, 0, sizeof(confirm_buf));
                snprintf(confirm_buf, sizeof(confirm_buf), "Mcu_Sensor_Enable:%d,Mcu_Sensor_Type:%d",Mcu_Sensor_Enable,Mcu_Sensor_Type);
                confirm(confirm_buf);
            }
            else if (data[4] == MLSP_DEVICE_TEARDOWN_KEY || data[4] == MLSP_DEVICE_PHOTOSENSITIVE)//返回的是防拆键/光敏信息
            {
#if defined(__GT740__)||defined(__GT420D__)
                data[6] = (data[6] == 0?1:0);//转换一下
                memset(confirm_buf, 0, sizeof(confirm_buf));
                snprintf(confirm_buf, sizeof(confirm_buf), "光感使能[%d],状态:[%s]", data[5], (data[6] == 0?"亮":"暗"));
                confirm(confirm_buf);
                if (data[5] != G_parameter.teardown_str.sw)
                {
                    LOGD(L_OBD, L_V1, "ST单片机防拆开关不符!");
                    track_cust_notify_st_remove_alarm_modify(G_parameter.teardown_str.sw);
                }
                if (G_realtime_data.switch_down != data[6])
                {
                    LOGD(L_OBD, L_V1, "防拆键状态不符!%d,%d", G_realtime_data.switch_down, data[6]);
                    G_realtime_data.switch_down = data[6];
                    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                    track_cust_upload_param();
                }
#endif//__GT740__
            }
            else
            {
                confirm("暂不支持的设备类型");
            }
            track_mlsp_send((void*)MLSP_GET_HW_STATUS);
            break;
        case MLSP_GET_PAR://以GSM为准,发现不一致则重新设置
#if !defined(__GT740__)||defined(__GT420D__)
            Proticol_Version=data[5];
            if (G_parameter.vibrates_alarm.sensor_sw != data[6]||
                    G_parameter.vibrates_alarm.sensitivity_level != data[7]||
                    G_parameter.vibrates_alarm.detection_time != data[8]||
                    G_parameter.gps_work.vibrates.detection_count != data[9]||
                    G_parameter.vibrates_alarm.detection_count != data[10])
            {
                confirm("传感器参数不一致");
                track_mlsp_send((void*)MLSP_GET_PAR);
            }
            else
            {
                confirm("参数一致");
                track_mlsp_send((void*)MLSP_GET_PAR);
            }
            break;
#endif//__GT740__
        case MLSP_MCU_EVENT:
            if (data[4] == 0x10)//充电器事件
            {
                if (data[5] == 0x10)
                {
                    LOGS("外电拔出");
                    Charge_Status = 0;
                }
                else if (data[5] == 0x11)
                {
                    LOGS("开始充电");
                    Charge_Status = 1;
                }
                else if (data[5] == 0x12)
                {
                    LOGS("外电接通但充电停止");
                    Charge_Status = 2;
                }
            }
            track_cust_mlsp_packet_send(MLSP_MCU_EVENT,NULL,0,0);
            break;
        case MLSP_GSENSOR_EVENT:
            if (data[4] == 0)//五分钟无振动
            {
                LOGS("5分钟无振动");
            }
            else if (data[4] == 2)//震动开启GPS
            {
                LOGS("震动开启GPS");
            }
            else if (data[4] == 3)//震动报警
            {
                LOGS("震动报警");
            }
            else if (data[4] == 1)//检测到一次震动
            {
                LOGS("检测到一次震动");
            }
            track_cust_mlsp_packet_send(MLSP_GSENSOR_EVENT,NULL,0,0);
            break;
        case MLSP_DEVICE_ALARM:
            if (data[4] == 0x11)//电池低电报警
            {
                LOGS("电池低电报警");
            }
            else if (data[4] == 0x12)//外电低电报警
            {
                LOGS("外电低电报警");
            }
            else if (data[4] == 0x13)//外电断电报警
            {
                LOGS("外电断电报警");
            }
            else if (data[4] == 0x14)//拆除报警
            {
                LOGS("拆除报警");
#if defined(__GT740__)||defined(__GT420D__)
                cmd_05();
#endif//__GT740__
            }
            track_cust_mlsp_packet_send(MLSP_DEVICE_ALARM,NULL,0,0);
            break;
        case MLSP_DEVICE_ENABLE:
            if (data[4]==1)//设置成功
            {
                track_mlsp_send((void*)MLSP_DEVICE_ENABLE);
                confirm("外设使能设置成功");
            }
            else
            {
                confirm("外设使能设置失败");
                track_mlsp_send((void*)0xFF);
            }
            break;
        case MLSP_MCU_SW_UPDATE:
#if defined (__MCU_SW_UPDATE__)
            track_drv_mcu_sw_update_rsp();
#endif
            break;
        default:
            confirm("不存在的协议号");
            return;
    }
}
//jm81
void track_cust_module_power_data(kal_uint8 *data, int len)
{
    kal_uint8 i  = 0;
    if (data[0]==HEADER1 && data[1]==HEADER2 && len<=MSLP_MAX_LEN)
    {
        if (data[2] == len)//单个包
        {
            track_cust_mlsp_decode(data,len);
        }
        else
        {
            i = 1;
            while (i < len-5)
            {
                if (data[i]==HEADER2 && data[i-1]==HEADER1 &&( i+data[i + 1] - 1) <=len && data[i + 1] <=MSLP_MAX_LEN
                        && data[i -1 +data[i + 1] - 2] == 0x0D && data[i -1 +data[i + 1] - 1] == 0x0A)
                {
                    track_cust_mlsp_decode(data+ i -1,data[i + 1]);
                }
                i++;
            }
        }
    }
    else
    {
        if (strcmp(data,"[MCU]"))
            LOGS("\r\n[MCU]%s",data);
    }
}
#endif /*__MCU_LONG_STANDBY_PROTOCOL__*/

#if defined(__JM66__)
extern void track_drv_sys_power_off_req(void);
extern void track_device_reset(int sec);
static void track_cust_tbt100_update_start(U8* image);
void track_cust_tbt100_set_nextboot(kal_uint16 interval);
void track_cust_jm66_shutdown(void);
void track_cust_into_tracking_mode(kal_uint8 arg);

U32 track_cust_get_tbt100_vbat(void)
{
    return (U32)(tbt100_bt_vbat * 1000);
}
void track_cust_send_alarm(kal_uint8 cmd)
{
    track_gps_data_struct *gpsLastPoint = NULL;
    gpsLastPoint = track_cust_gpsdata_alarm();
    switch (cmd)
    {
        case TBT100_EVENT_COLLAR_BROKEN:
            LOGD(L_APP, L_V5, "发送项圈剪断报警包");
            if (track_cust_gps_positioned_ever_after_booting())
                track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_CUT_ALM, KAL_TRUE, 0);
            else
                track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_CUT_ALM, KAL_TRUE, 0);
            break;
        case TBT100_EVENT_TEARDOWN:
            LOGD(L_APP, L_V5, "发送外壳拆除报警包");
            if (track_cust_gps_positioned_ever_after_booting())
                track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_OPEN_SHELL, KAL_TRUE, 0);
            else
                track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_OPEN_SHELL, KAL_TRUE, 0);
            break;
        case TBT100_EVENT_OUTLIER:
            LOGD(L_APP, L_V5, "发送牦牛离群报警包");
            if (track_cust_gps_positioned_ever_after_booting())
                track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_LEFT_ALM, KAL_TRUE, 0);
            else
                track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_LEFT_ALM, KAL_TRUE, 0);
            break;
        case TR_CUST_ALARM_LINKOUT_ALM:
            LOGD(L_APP, L_V5, "发送主动离线报警包");
            if (track_cust_gps_positioned_ever_after_booting())
                track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_LINKOUT_ALM, KAL_FALSE, 0);
            else
                track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_LINKOUT_ALM, KAL_FALSE, 0);
            break;
    }
}
static track_cust_need_paon_and_gprs()
{
    if (track_drv_PA_control(99) == 1)//PA关闭状态
    {
        track_drv_PA_control(0);//开PA
        if (!track_soc_login_status())
        {
            tr_start_timer(TRACK_CUST_NET_CONN_TIMER_ID, 20000, track_cust_net_conn);
        }
        LOGD(L_APP, L_V5, "开PA,20秒后尝试联网");
    }
    else
    {
        LOGD(L_APP, L_V5, "%d,%d", track_soc_login_status(), kal_get_systicks());
        if (!track_soc_login_status() && kal_get_systicks() > 1200)
        {

            track_cust_net_conn();
        }
    }
}
void track_cust_into_tracking_mode(kal_uint8 arg)
{
    if (arg == 1)
    {
        track_cust_need_paon_and_gprs();
    }
    track_stop_timer(TRACK_CUST_KEY_DOWN_DELAY_SHUTDOWN_TIMER_ID);
    track_stop_timer(TRACK_CUST_WORKTIME_TIMER_ID);
    track_stop_timer(TRACK_CUST_DELAY_COMPARE_LBS_TIMER_ID);
    track_stop_timer(TRACK_CUST_FIRST_BOOT_DELAY_SHUTDOWN_TIMER_ID);
    track_stop_timer(TRACK_CUST_LBS_TIMER_ID);
    //track_stop_timer(TRACK_CUST_DELAY_SHUTDOWN_TIMER_ID);
    if (Jm66_Current_mode > 10)
    {
        track_cust_gps_work_req((void*)3);
        track_start_timer(TRACK_CUST_TRACKING_TIMER_ID, G_parameter.mode2_interval * 60 *1000, track_cust_into_tracking_mode,(void*)0);
    }
    else if (Jm66_Current_mode == 2)
    {
        track_cust_gps_work_req((void *)2);
    }
    else
    {
        LOGS("软件逻辑错误%d", Jm66_Current_mode);
    }
}
kal_bool track_cust_IsAlreadySendPosition(kal_uint8 arg)
{
    static kal_bool is_already_send_position = 0;
    //if(Jm66_is_gps_boot == KAL_FALSE)return 1;
    if (arg <= 1)is_already_send_position = arg;
    return is_already_send_position;
}
void track_cust_worktime_count(void)
{
    static kal_uint16 count = 0;
    static kal_uint16 already_send_count = 0;
    if (Jm66_Current_mode > 10 || track_is_testmode() || track_is_autotestmode())return;

    LOGD(L_APP, L_V1, "[%d]正常模式下已工作%d秒",track_cust_IsAlreadySendPosition(99),count*10);
    track_soc_send((void*)9);
    if (track_cust_IsAlreadySendPosition(99) == 1)
    {
        already_send_count++;
        if (already_send_count >= 3)
        {
            track_cust_jm66_shutdown();
        }
    }
    else if (count >=30)
    {
        track_cust_jm66_shutdown();
    }

    count++;
    tr_start_timer(TRACK_CUST_WORKTIME_TIMER_ID, 10*1000, track_cust_worktime_count);
}
void track_cust_jm66_shutdown(void)
{
    U16 current_min,interval;
    int n;
    applib_time_struct time = {0};

    //if(Jm66_Current_mode > 0)track_cust_send_alarm(TR_CUST_ALARM_LINKOUT_ALM);
    track_fun_get_time(&time, KAL_FALSE, &G_parameter.zone);
    current_min = time.nHour * 60 + time.nMin;
    for (n = 0; n < G_parameter.mtworkt_count; n++)
    {
        if ((current_min + 1) < G_parameter.mtworkt[0][n])break;
    }
    if (n < G_parameter.mtworkt_count)//今天还有时间点未执行
    {
        interval = G_parameter.mtworkt[0][n] - current_min;
    }
    else//今天的时间点已全部执行,在明天第一个闹钟起来
    {
        interval = 1440 + G_parameter.mtworkt[0][0] - current_min;
    }
    will_shutdown = TRUE;
    if (!track_is_testmode() && !track_is_autotestmode())
    {
        track_soc_queue_all_to_nvram();//
        track_cust_backup_gps_data(4, NULL);
    }
    track_cust_tbt100_set_nextboot(interval);
}
static void track_cust_moudle_compare_lbs(void)
{
    static U8 retry = 0;
    int i,n,j;
    U16 new_cell[7] = {0};
    kal_bool is_find = FALSE, is_all_zero = KAL_TRUE;
    U8 lbs_list, already_found = 0;
    kal_uint16 now_cell_id[7] = {0};
    char m_buf[LBS_LIST_MAX * 7] = {0};
    LBS_Multi_Cell_Data_Struct * current_lbs = NULL;
    if (simulation_lbs.status == 0)
        current_lbs = track_drv_get_lbs_data();
    else
        current_lbs = &simulation_lbs;
    LOGD(L_OBD, L_V1, "LBS:%d 0x%04X %d,%d,%d", current_lbs->status, current_lbs->MainCell_Info.cell_id, current_lbs->valid_cel_num, is_quit_transport_boot, track_drv_get_sim_type());
    if (current_lbs->status == TRUE && current_lbs->MainCell_Info.cell_id != 0 && track_drv_get_sim_type() >= 1)
    {
        now_cell_id[0] = current_lbs->MainCell_Info.cell_id;
        for (n = 0; n < current_lbs->valid_cel_num; n++)
        {
            now_cell_id[1 + n] = current_lbs->NbrCell_Info[n].cell_id;
        }
        for (lbs_list = 0; lbs_list < LBS_LIST_MAX; lbs_list++)
        {
            if (G_realtime_data.cell_id[lbs_list] != 0)
            {
                is_all_zero = FALSE;
                break;
            }
        }
        if (is_quit_transport_boot == 1 || is_all_zero == TRUE)
        {
            memset(&G_realtime_data.cell_id[0], 0, LBS_LIST_MAX * 2);
            memcpy(&G_realtime_data.cell_id[0], now_cell_id, sizeof(now_cell_id));
            G_realtime_data.valid_count = current_lbs->valid_cel_num + 1;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
            LOGD(L_OBD, L_V1, "首次开机保存基站列表%d", G_realtime_data.valid_count);
            for (n = 0; n < LBS_LIST_MAX; n++)
            {
                snprintf(m_buf + strlen(m_buf), 99, "%04X ", G_realtime_data.cell_id[n]);
            }
            LOGD(L_OBD, L_V1, "%s", m_buf);
            tr_start_timer(TRACK_CUST_FIRST_BOOT_DELAY_SHUTDOWN_TIMER_ID, 140 * 1000, track_cust_jm66_shutdown);
        }
        else
        {
            LOGD(L_OBD, L_V1, "已有基站列表%d:\r\n", G_realtime_data.valid_count);
            for (n = 0; n < LBS_LIST_MAX; n++)
            {
                snprintf(m_buf + strlen(m_buf), 99, "%04X ", G_realtime_data.cell_id[n]);
            }
            LOGD(L_OBD, L_V1, "%s", m_buf);
            //LOGH(L_OBD, L_V1, &G_realtime_data.cell_id[0], LBS_LIST_MAX * 2);
            LOGD(L_OBD, L_V1, "现扫描到基站:\r\n#NOW[%d][%d] %04X %04X %04X %04X %04X %04X %04X",
                 current_lbs->status, current_lbs->valid_cel_num, now_cell_id[0], now_cell_id[1], now_cell_id[2], now_cell_id[3], now_cell_id[4], now_cell_id[5], now_cell_id[6]);
            n = 0;
            for (i = 0; i < current_lbs->valid_cel_num + 1; i++)
            {
                for (j = 0; j < G_realtime_data.valid_count; j++)
                {
                    if (G_realtime_data.cell_id[j] == now_cell_id[i])
                    {
                        already_found++;
                        break;
                    }
                }
                if (j == G_realtime_data.valid_count)
                {
                    new_cell[n++] = now_cell_id[i];//这是新的CELLID
                    LOGD(L_OBD, L_V1, "ADD 0x%04X,0x%04X", new_cell[n-1], now_cell_id[i]);
                }
            }
            if (already_found == (current_lbs->valid_cel_num + 1))
            {
                is_find = TRUE;
            }
            if (is_find)//全部一致
            {
                LOGD(L_OBD, L_V1, "没出现新基站,关机");
                track_cust_jm66_shutdown();
            }
            else//有不同
            {
                int p;
                if (already_found != 0 || G_realtime_data.valid_count <= 5)
                {
                    for (i = 0; i < n; i++)
                    {
                        if (G_realtime_data.valid_count >= LBS_LIST_MAX)
                        {
                            for (p = 0; p < (LBS_LIST_MAX - 1); p++)
                            {
                                G_realtime_data.cell_id[p] = G_realtime_data.cell_id[p + 1];
                            }
                            G_realtime_data.valid_count--;
                        }
                        LOGD(L_OBD, L_V1, "0x%04X加入基站表[%d]", new_cell[i], G_realtime_data.valid_count);
                        G_realtime_data.cell_id[G_realtime_data.valid_count++] = new_cell[i];
                    }
                    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                }
                else
                {
                    LOGD(L_OBD, L_V1, "所有基站均不同,不加入列表%d", G_realtime_data.valid_count);
                }
                LOGD(L_OBD, L_V1, "出现新基站,连接平台,开启GPS,现基站列表%d:\r\n", G_realtime_data.valid_count);
                memset(m_buf, 0, sizeof(m_buf));
                for (n = 0; n < LBS_LIST_MAX; n++)
                {
                    snprintf(m_buf + strlen(m_buf), 99, "%04X ", G_realtime_data.cell_id[n]);
                }
                LOGD(L_OBD, L_V1, "%s", m_buf);
                Jm66_Current_mode = 1;
                track_cust_need_paon_and_gprs();
                track_cust_gps_work_req((void *)2);
                track_cust_worktime_count();
                track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_MAYBE_FAR, KAL_FALSE, 0);
            }
        }
    }
    else
    {
        tr_start_timer(TRACK_CUST_LBS_TIMER_ID, 10 * 1000, track_cust_moudle_compare_lbs);
        retry++;
        if (retry >= 6)
        {
            LOGD(L_OBD, L_V1, "无法获取有效的基站信息,关机");
            track_cust_jm66_shutdown();
        }
    }
}
int track_cust_tbt100_completion(kal_uint8 cmd,kal_uint8* data,kal_uint16 data_len,kal_bool is_send_to_gsm)
{
    kal_uint16 crc;
    if (is_send_to_gsm)
        data[0] = 0x42;
    else
        data[0] = 0x47;
    data[1] = 0x54;
    data[2] = cmd;
    data[3] = data_len & 0x00FF;
    data[4] = (data_len & 0xFF00)>>8;
    crc = CRC16(data,data_len+5);
    data[data_len+5] = crc & 0x00FF;
    data[data_len+5 + 1] = (crc & 0xFF00)>>8;
    return data_len+7;
}
static void track_cust_tbt100_packet_send(kal_uint8 cmd,kal_uint8* data, kal_uint16 data_len, kal_bool is_vector)
{
    kal_uint8* packet_data = NULL;
    int len = 0;
    if (is_update_mode && (cmd < 0xD0 || cmd > 0xD5))
    {
        LOGD(L_OBD, L_V1, "蓝牙升级中不发送无关指令0x%02X", cmd);
        return;
    }
    packet_data = (kal_uint8*)Ram_Alloc(5, data_len + 8);
    if (data != NULL)
        memcpy(packet_data + 5, data, data_len);
    len = track_cust_tbt100_completion(cmd, packet_data, data_len, 0);
    if (is_vector)//通过队列
        track_mlsp_queue_send_req(packet_data, len);
    else//不通过队列(适用于回复单片机时)
        track_drv_write_exmode(packet_data, len);
    Ram_Free(packet_data);
}
static void confirm(char *str)
{
    if ((track_is_testmode() == KAL_TRUE) || (track_is_autotestmode() == KAL_TRUE))
    {
        LOGS("%s", str);
        return;
    }
    if (g_return_sms_valid)
    {
        track_cust_cmd_reply(&g_return_sms, str, strlen(str));
        g_return_sms_valid = 0;
        track_stop_timer(TRACK_CUST_MLSP_COMMUNICATE_FAIL_TIMER_ID);
    }
    else LOGD(L_OBD, L_V1, "%s", str);
}
static track_cust_delete_tbt100_image(char *file_path)
{
    WCHAR FilePath[60] = {0};
    OTA_kal_wsprintf(FilePath, file_path);
    FS_Delete(FilePath);
}
static kal_bool track_cust_check_tbt100_update_image(char *file_path)
{
    FS_HANDLE fp = NULL;
    WCHAR FilePath[60] = {0};
    OTA_kal_wsprintf(FilePath, file_path);
    fp = FS_Open(FilePath, FS_READ_ONLY);
    if (fp < 0)
    {
        return 0;
    }
    FS_Close(fp);
    LOGD(L_OBD, L_V1, "检测到蓝牙升级文件");
    return 1;
}
static int track_cust_tbt100_read_image(U8* file_path)
{
    FS_HANDLE fp = NULL;
    WCHAR FilePath[60] = {0};
    U32 filelen, readsize;
    int fs_result;

    OTA_kal_wsprintf(FilePath, file_path);
    fp = FS_Open(FilePath, FS_READ_ONLY);
    if (fp < 0)
    {
        return fp;
    }
    FS_GetFileSize(fp, &filelen);
    update_image_p = (U8*)Ram_Alloc(5, filelen+ 1);
    if (update_image_p == NULL)
    {
        FS_Close(fp);
        return -1;
    }
    fs_result = FS_Read(fp, update_image_p, filelen+ 1, &readsize);
    if (fs_result < 0 || readsize != filelen)
    {
        LOGS("%d,%d", readsize, filelen);
        FS_Close(fp);
        return -2;
    }
    FS_Close(fp);
    update_image_len = readsize;
    LOGD(L_OBD, L_V1, "filesize:%d,readsize:%d", filelen, readsize);
    return readsize;
}
void track_tbt100_check_update_bt(void)
{
    if (track_cust_check_tbt100_update_image(BT_UPDATE_IMAGE))
    {
        if (track_cust_tbt100_read_image(BT_UPDATE_IMAGE) > 0)
        {
            track_cust_tbt100_update_start(update_image_p);
            return;
        }
        track_tbt100_quit_update();
    }
}
static track_tbt100_quit_update(void)
{
    LOGD(L_OBD, L_V1, "删除升级文件,3秒后重启");
    if (update_image_p != NULL)Ram_Free(update_image_p);
    track_cust_delete_tbt100_image(BT_UPDATE_IMAGE);
    track_start_timer(TRACK_CUST_DELAY_SHUTDOWN_FORCE_TIMER_ID, 3000, track_device_reset, (void*)0);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_update_start
 *  Description -  告知蓝牙开始升级
 *************************************************************/
static void track_cust_tbt100_update_start(U8* image)
{
    U8 tmp[12] = {0};
    LOGD(L_OBD, L_V1, "告知蓝牙开始升级");
    memcpy(tmp, image, 12);
    track_cust_tbt100_packet_send(TBT100_UPDATE_START, tmp, 12, 0);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_update_end
 *  Description -  告知蓝牙升级结束
 *************************************************************/
static void track_cust_tbt100_update_end(void)
{
    LOGD(L_OBD, L_V1, "告知蓝牙开始结束");
    track_cust_tbt100_packet_send(TBT100_UPDATE_END, NULL, 0, 0);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_update_quit
 *  Description -  告知蓝牙升级因异常退出
 *************************************************************/
static void track_cust_tbt100_update_quit(void)
{
    LOGD(L_OBD, L_V1, "告知蓝牙升级因异常退出");
    track_cust_tbt100_packet_send(TBT100_QUIT_UPDATE, NULL, 0, 0);
    track_tbt100_quit_update();
}
/*************************************************************
 *  Function    -  track_cust_tbt100_update_check
 *  Description -  升级文件校验
 *************************************************************/
static void track_cust_tbt100_update_check(void)
{
    LOGD(L_OBD, L_V1, "升级文件校验");
    track_cust_tbt100_packet_send(TBT100_UPDATE_CHECKFILE, NULL, 0, 0);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_update_send
 *  Description -  升级文件发送
 *************************************************************/
static void track_cust_tbt100_update_send(void)
{
    U32 rest;
    U8* tmp = NULL;
    if (update_image_offset < update_image_len)
    {
        rest = update_image_len - update_image_offset;
        if (is_first_package)
            this_time_send_len = rest > BT_UPDATE_FIRST_DATA_MAX_LEN?BT_UPDATE_FIRST_DATA_MAX_LEN:rest;
        else
            this_time_send_len = rest > BT_UPDATE_ONCE_DATA_MAX_LEN?BT_UPDATE_ONCE_DATA_MAX_LEN:rest;

        tmp = (U8*)Ram_Alloc(5, this_time_send_len + 2);
        *tmp = update_image_offset & 0x00FF;
        *(tmp + 1) = (update_image_offset & 0xFF00) >> 8;
        memcpy(tmp + 2, update_image_p + update_image_offset, this_time_send_len);
        LOGD(L_OBD, L_V1, "##Send image~Index:%d Offset:%d Sendlen:%d Alllen:%d", index_send, update_image_offset, this_time_send_len, update_image_len);
        track_cust_tbt100_packet_send(TBT100_UPDATE_IMAGE, tmp, this_time_send_len + 2, 0);
        Ram_Free(tmp);
    }
    else//传输完毕
    {
        LOGD(L_OBD, L_V1, "##Send Done!Offset:%d Alllen:%d", update_image_offset, update_image_len);
        track_cust_tbt100_update_check();
    }
}
/*************************************************************
 *  Function    -  track_cust_tbt100_bttest
 *  Description -  蓝牙测试通道
 *************************************************************/
void track_cust_tbt100_bttest(kal_uint8* test_data, kal_uint16 datalen)
{
    kal_uint8* tmp = NULL;

    tmp = (kal_uint8*)Ram_Alloc(5, datalen);
    memcpy(tmp, test_data, datalen);
    LOGD(L_OBD, L_V1, "蓝牙测试数据,长度:%d", datalen);
    track_cust_tbt100_packet_send(TBT100_BTTEST, tmp, datalen, 1);
    Ram_Free(tmp);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_set_aeskey
 *  Description -  设置蓝牙通信密钥22位
 *************************************************************/
void track_cust_tbt100_set_aeskey(kal_uint8* aeskey)
{
    kal_uint8 tmp[0x16] = {0};
    memcpy(tmp, aeskey, sizeof(tmp));
    LOGD(L_OBD, L_V1, "设置蓝牙通信密钥");
    track_cust_tbt100_packet_send(TBT100_SET_KEY, tmp, 0x16, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_set_timepar
 *  Description -  蓝牙时间参数配置
 *************************************************************/
void track_cust_tbt100_set_timepar(void)
{
    kal_uint8 tmp[6] = {0};
    int i = 0;
    LOGD(L_OBD, L_V1, "蓝牙时间参数配置");
    tmp[i++] = 0xFF;//配置所有参数
    tmp[i++] = G_parameter.bt_timepar.scan_interval;//扫描间隔
    tmp[i++] = G_parameter.bt_timepar.scan_last;//扫描时长
    tmp[i++] = G_parameter.bt_timepar.broadcast_interval;//广播间隔
    tmp[i++] = G_parameter.bt_timepar.broadcast_last & 0x00FF;//广播时长低
    tmp[i++] = (G_parameter.bt_timepar.broadcast_last & 0xFF00)>>8;//广播时长高
    track_cust_tbt100_packet_send(TBT100_SET_TIMEPAR, tmp, i, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_get_timepar
 *  Description -  蓝牙时间参数读取
 *************************************************************/
void track_cust_tbt100_get_timepar(void)
{
    kal_uint8 tmp[1] = {0};
    int i = 0;
    LOGD(L_OBD, L_V1, "蓝牙时间参数读取");
    tmp[i++] = 0xFF;//读取所有参数
    track_cust_tbt100_packet_send(TBT100_GET_TIMEPAR, tmp, i, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_get_sensorpar
 *  Description -  G-Sensor参数读取
 *************************************************************/
void track_cust_tbt100_get_sensorpar(void)
{
    kal_uint8 tmp[1] = {0};
    int i = 0;
    LOGD(L_OBD, L_V1, "G-Sensor参数读取");
    tmp[i++] = 0xFF;//读取所有参数
    track_cust_tbt100_packet_send(TBT100_GET_SENSORPAR, tmp, i, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_set_sensorpar
 *  Description -  G-Sensor参数配置
 *************************************************************/
void track_cust_tbt100_set_sensorpar(void)
{
    kal_uint8 tmp[10] = {0};
    int i = 0;
    LOGD(L_OBD, L_V1, "G-Sensor参数配置");
    tmp[i++] = 0xFF;//配置所有参数
    tmp[i++] = G_parameter.sensorpar.sensor_start_hour;//开始时间hour
    tmp[i++] = G_parameter.sensorpar.sensor_start_min;//开始时间min
    tmp[i++] = G_parameter.sensorpar.sensor_end_hour;//结束时间hour
    tmp[i++] = G_parameter.sensorpar.sensor_end_min;//结束时间min
    tmp[i++] = G_parameter.sensorpar.sensor_mode;//工作模式
    tmp[i++] = G_parameter.vibrates_alarm.sensitivity_level;//灵敏度
    tmp[i++] = G_parameter.vibrates_alarm.detection_time;//检测窗时长
    tmp[i++] = G_parameter.vibrates_alarm.detection_count;//震动计数门限
    tmp[i++] = G_parameter.vibrates_alarm.sampling_interval;//采样间隔
    track_cust_tbt100_packet_send(TBT100_SET_SENSORPAR, tmp, i, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_set_envpar
 *  Description - 环境传感器参数配置
 *************************************************************/
void track_cust_tbt100_set_envpar(void)
{
    kal_uint8 tmp[1] = {0};
    int i = 0;
    LOGD(L_OBD, L_V1, "配置环境传感器参数");
    tmp[i++] = G_parameter.bsensor_sampling_interval;//环境传感器采样间隔
    track_cust_tbt100_packet_send(TBT100_SET_ENVPAR, tmp, i, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_set_imei/track_cust_tbt100_get_imei
 *  Description - 向蓝牙设置/读取IMEI
 *************************************************************/
void track_cust_tbt100_set_imei(void)
{
    kal_uint8 tmp[8] = {0};
    LOGD(L_OBD, L_V1, "向蓝牙设置IMEI");
    memcpy(tmp, track_drv_get_imei(1), 8);
    track_cust_tbt100_packet_send(TBT100_SET_IMEI, tmp, 8, 1);
}
void track_cust_tbt100_get_imei(void)
{
    LOGD(L_OBD, L_V1, "读取IMEI");
    track_cust_tbt100_packet_send(TBT100_GET_IMEI, NULL, 0, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_get_envpar
 *  Description -  环境传感器参数读取
 *************************************************************/
void track_cust_tbt100_get_envpar(void)
{
    kal_uint8 tmp[1] = {0};
    int i = 0;
    LOGD(L_OBD, L_V1, "读取环境传感器参数");
    tmp[i++] = 0x00;
    track_cust_tbt100_packet_send(TBT100_GET_ENVPAR, tmp, i, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_get_envdata
 *  Description -  环境传感器参数读取
 *  1:当前环境数据2:一天气压数据3:一天温度4:一天湿度5:一天环境数据
 *************************************************************/
void track_cust_tbt100_get_envdata(kal_uint8 type)
{
    kal_uint8 tmp[1] = {0};
    if (G_parameter.bsensor_sampling_interval == 0)
    {
        LOGD(L_OBD, L_V1, "环境传感器参数为0,不读取");
        return;
    }
    LOGD(L_OBD, L_V1, "读取环境传感器数据");
    tmp[0] = type;
    track_cust_tbt100_packet_send(TBT100_GET_ENVDATA, tmp, 1, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_get_envpar
 *  Description -  读取GSENSOR激活次数
 *************************************************************/
void track_cust_tbt100_get_sensoractive_times(void)
{
    LOGD(L_OBD, L_V1, "读取激活次数");
    track_cust_tbt100_packet_send(TBT100_GET_SENSOR_ACTIVE_TIMES, NULL, 0, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_get_baseversion
 *  Description -  获取蓝牙基础版本
 *************************************************************/
void track_cust_tbt100_get_baseversion(void)
{
    LOGD(L_OBD, L_V1, "读取基础版本号");
    track_cust_tbt100_packet_send(TBT100_GET_BT_BASEVERSION, NULL, 0, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_set_nextboot
 *  Description -  设置下次开机时间(分钟时长间隔)
 *************************************************************/
void track_cust_tbt100_set_nextboot(kal_uint16 interval)
{
    kal_uint8 tmp[2] = {0};
    LOGD(L_OBD, L_V1, "设置%d分钟后唤醒GSM", interval);
    tmp[0] = interval & 0x00FF;
    tmp[1] = (interval & 0xFF00)>>8;
    track_cust_tbt100_packet_send(TBT100_SET_NEXTBOOT, tmp, 2, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_into_transport
 *  Description -  向蓝牙请求进入运输模式
 *************************************************************/
void track_cust_tbt100_into_transport(void)
{
    LOGD(L_OBD, L_V1, "向蓝牙请求进入运输模式");
    //恢复出厂,清空标志
    track_cust_tbt100_packet_send(TBT100_TRANSPORT, NULL, 0, 1);
}
void track_cust_tbt100_req_scan(void)
{
    LOGD(L_OBD, L_V1, "请求立即扫描");
    track_cust_tbt100_packet_send(TBT100_REQ_SCAN, NULL, 0, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_time_sync
 *  Description -  与蓝牙时间同步(读,写)
 *  写时,时间参数为NULL则自动使用RTC系统时间设置
 *************************************************************/
void track_cust_tbt100_time_sync(kal_bool is_write, applib_time_struct* set_time)
{
    kal_uint8 data[8] = {0};//读/写年年月日时分秒
    int i = 0;
    data[i++] = is_write;
    if (is_write)
    {
        applib_time_struct time = {0};
        applib_time_struct* time_p;
        if (set_time != NULL)
        {
            time_p = set_time;
        }
        else
        {
            track_fun_get_time(&time, KAL_FALSE, &G_parameter.zone);
            time_p = &time;
        }
        LOGD(L_OBD, L_V1, "设置蓝牙时间%d:%d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", (set_time == NULL?0:1)
             , time_p->nYear, time_p->nMonth, time_p->nDay, time_p->nHour, time_p->nMin, time_p->nSec);
        data[i++] = time_p->nYear & 0x00FF;
        data[i++] = (time_p->nYear & 0xFF00)>>8;
        data[i++] = time_p->nMonth;
        data[i++] = time_p->nDay;
        data[i++] = time_p->nHour;
        data[i++] = time_p->nMin;
        data[i++] = time_p->nSec;
        track_cust_tbt100_packet_send(TBT100_SYNC_TIME, data, i, 1);
    }
    else
    {
        LOGD(L_OBD, L_V1, "读取蓝牙时间");
        track_cust_tbt100_packet_send(TBT100_SYNC_TIME, data, i, 1);
    }
}
/*************************************************************
 *  Function    -  track_cust_tbt100_shutdown_req
 *  Description -  向蓝牙请求关机
 *************************************************************/
void track_cust_tbt100_shutdown_req(void)
{
    float vbatf;
    kal_uint16 vbatui16;
    kal_uint8 data[2] = {0};
    vbatf = track_cust_get_battery_volt();//3000000
    vbatui16 = vbatf / 10000;
    data[0] = vbatui16 & 0x00FF;
    data[1] = (vbatui16 & 0xFF00)>>8;
    LOGD(L_OBD, L_V1, "请求关机,电池电压%d", vbatui16);
    track_cust_tbt100_packet_send(TBT100_SHUTDOWN_REQ, data, 2, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_tell_bt_vbat
 *  Description -  发送电池电压给蓝牙
 *************************************************************/
void track_cust_tbt100_tell_bt_vbat(void)
{
    float vbatf;
    kal_uint16 vbatui16;
    kal_uint8 data[2] = {0};
    vbatf = track_cust_get_battery_volt();//3000000
    vbatui16 = vbatf / 10000;
    data[0] = vbatui16 & 0x00FF;
    data[1] = (vbatui16 & 0xFF00)>>8;
    LOGD(L_OBD, L_V1, "通知蓝牙电池电压%d", vbatui16);
    track_cust_tbt100_packet_send(TBT100_TELL_BT_VBAT, data, 2, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_get_bt_resettimes
 *  Description -  查询蓝牙硬件复位次数
 *************************************************************/
void track_cust_tbt100_get_bt_resettimes(void)
{
    LOGD(L_OBD, L_V1, "查询蓝牙硬件复位次数");
    track_cust_tbt100_packet_send(TBT100_GET_BT_RESET_TIMES, NULL, 0, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_get_bt_mac
 *  Description -  查询蓝牙MAC地址
 *************************************************************/
void track_cust_tbt100_get_bt_mac(void)
{
    LOGD(L_OBD, L_V1, "查询蓝牙MAC地址");
    track_cust_tbt100_packet_send(TBT100_GET_BT_MAC, NULL, 0, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_get_bt_vbat
 *  Description -  查询蓝牙电池电压
 *************************************************************/
void track_cust_tbt100_get_bt_vbat(void)
{
    LOGD(L_OBD, L_V1, "查询蓝牙电池电压");
    track_cust_tbt100_packet_send(TBT100_GET_BTVBAT, NULL, 0, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_get_aeskey
 *  Description -  获取蓝牙通信密钥
 *************************************************************/
void track_cust_tbt100_get_aeskey(void)
{
    LOGD(L_OBD, L_V1, "获取蓝牙通信密钥");
    track_cust_tbt100_packet_send(TBT100_GET_KEY, NULL, 0, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_bt_reset
 *  Description -  蓝牙复位
 *************************************************************/
void track_cust_tbt100_bt_reset(void)
{
    LOGD(L_OBD, L_V1, "蓝牙复位");
    track_cust_tbt100_packet_send(TBT100_BT_RESET, NULL, 0, 1);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_clear_btlog
 *  Description -  清除蓝牙日志
 *************************************************************/
void track_cust_tbt100_clear_btlog(void)
{
    LOGD(L_OBD, L_V1, "清除蓝牙日志");
    track_cust_tbt100_packet_send(TBT100_CLEAR_BTLOG, NULL, 0, 0);
}
/*************************************************************
 *  Function    -  track_cust_tbt100_get_appversion
 *  Description - 获取蓝牙应用版本
 *************************************************************/
void track_cust_tbt100_get_appversion(void)
{
    LOGD(L_OBD, L_V1, "获取蓝牙应用版本");
    track_cust_tbt100_packet_send(TBT100_GET_BT_APPVERSION, NULL, 0, 0);
}
/*************************************************************
 *  Function    -  track_cust_module_tbt100_decode
 *  Description -  TBT100协议解码
 *
 *************************************************************/
static void track_cust_module_tbt100_decode(kal_uint8 cmd, kal_uint8* data, U32 data_len)
{
    static kal_bool is_first = KAL_TRUE;
    static U8 cattle_cache[PACKET_MAC_MAX_LEN] = {0};
    static U32 cattlelen = 0;
    static kal_bool is_cattle_first = KAL_TRUE;
    static U16 count;

    kal_bool is_ok = KAL_FALSE;
    kal_uint8 confirm_buf[100];

    switch (cmd)
    {
//===========================================================
        case TBT100_SET_SENSORPAR ://G-Sensor参数配置
            if (0 == data[1])
            {
                is_ok = KAL_TRUE;
                confirm("G-Sensor参数设置成功");
            }
            else confirm("G-Sensor参数设置失败");
            break;
//===========================================================
        case TBT100_SET_TIMEPAR ://蓝牙时间参数设置(扫描,广播)
            if (0 == data[1])
            {
                is_ok = KAL_TRUE;
                confirm("蓝牙时间参数设置成功");
            }
            else confirm("蓝牙时间参数设置失败");
            break;
//===========================================================
        case TBT100_GET_TIMEPAR ://蓝牙时间参数读取(扫描,广播)
            is_ok = KAL_TRUE;
            LOGD(L_OBD, L_V5, "TIMEPAR:%d?%d,%d?%d,%d?%d,%d?%d", data[1], G_parameter.bt_timepar.scan_interval, data[2], G_parameter.bt_timepar.scan_last,
                 data[3], G_parameter.bt_timepar.broadcast_interval,(data[4] + ((data[5] & 0x00FF)<<8)), G_parameter.bt_timepar.broadcast_last);
            if (data[1] != G_parameter.bt_timepar.scan_interval ||data[2] != G_parameter.bt_timepar.scan_last
                    || data[3] != G_parameter.bt_timepar.broadcast_interval ||(data[4] + ((data[5] & 0x00FF)<<8)) != G_parameter.bt_timepar.broadcast_last)
            {
                confirm("蓝牙时间参数不一致,重新设置");
                //重新设置
                track_cust_tbt100_set_timepar();
            }
            else confirm("蓝牙时间参数一致");
            break;
//===========================================================
        case TBT100_GET_SENSORPAR ://G-Sensor参数读取
            is_ok = KAL_TRUE;
            LOGD(L_OBD, L_V5, "SENSORPAR:%d?%d,%d?%d,%d?%d,%d?%d,%d?%d,%d?%d,%d?%d,%d?%d,%d?%d",
                 data[1], G_parameter.sensorpar.sensor_start_hour, data[2], G_parameter.sensorpar.sensor_start_min,
                 data[3], G_parameter.sensorpar.sensor_end_hour, data[4], G_parameter.sensorpar.sensor_end_min,
                 data[5], G_parameter.sensorpar.sensor_mode, data[6], G_parameter.vibrates_alarm.sensitivity_level,
                 data[7], G_parameter.vibrates_alarm.detection_time, data[8], G_parameter.vibrates_alarm.detection_count, data[9], G_parameter.vibrates_alarm.sampling_interval);
            if (data[1]!= G_parameter.sensorpar.sensor_start_hour|| data[2]!= G_parameter.sensorpar.sensor_start_min||
                    data[3]!= G_parameter.sensorpar.sensor_end_hour|| data[4]!= G_parameter.sensorpar.sensor_end_min||
                    data[5]!= G_parameter.sensorpar.sensor_mode|| data[6]!= G_parameter.vibrates_alarm.sensitivity_level||
                    data[7]!= G_parameter.vibrates_alarm.detection_time|| data[8]!= G_parameter.vibrates_alarm.detection_count|| data[9]!= G_parameter.vibrates_alarm.sampling_interval)
            {
                LOGD(L_OBD, L_V5, "SENSOR参数不一致,重新设置");
                //重新设置
                track_cust_tbt100_set_sensorpar();
            }
            else LOGD(L_OBD, L_V5, "SENSOR参数一致");
            break;
//===========================================================
        case TBT100_SET_ENVPAR ://环境参数配置
            if (0 == data[0])
            {
                is_ok = KAL_TRUE;
                confirm("环境参数设置成功");
            }
            else confirm("环境参数设置失败");
            break;
//===========================================================
        case TBT100_GET_ENVPAR ://环境参数读取
            is_ok = KAL_TRUE;
            if (data[0] != G_parameter.bsensor_sampling_interval)
            {
                LOGD(L_OBD, L_V5, "环境参数不一致,重新设置");
                //重新设置
            }
            else LOGD(L_OBD, L_V5, "环境参数一致");
            break;
//===========================================================
        case TBT100_GET_SENSOR_ACTIVE_TIMES ://读取GSENSOR激活次数
        {
            U8 sensor_act_cache[100] = {0};
            U8 tmp;
            U16 sensor_count;
            U32 sensor_act_len, n;
            U16 tmpp;
            applib_time_struct time = {0};
            if (data_len % 3 != 0 || data_len < 3)
            {
                LOGS("震动数据有误!%d", data_len);
                track_mlsp_send((void*)cmd);
                return;
            }
            track_fun_get_time(&time, KAL_FALSE, &G_parameter.zone);
            is_ok = KAL_TRUE;
            tmpp = ((data[data_len - 1] & 0x00FF) << 8) + data[data_len - 2];
            LOGD(L_OBD, L_V5, "%d,%d,%d", (data[data_len - 1] & 0x00FF) << 8, data[data_len - 2], tmpp);
            for (n = 0; n < data_len; n++)
            {
                if ((n % 3) == 1)
                {
                    tmp = data[n + 1];
                    data[n + 1] = data[n];
                    data[n] = tmp;
                }
            }
            if (time.nHour == data[data_len - 3])//有不完整信息
            {
                sensor_count = data_len / 3 - 1;
                sensor_act_cache[0] = (sensor_count & 0xFF00) >> 8;
                sensor_act_cache[1] = (sensor_count & 0x00FF);
                memcpy(sensor_act_cache + 2, data, data_len - 3);
            }
            else
            {
                sensor_count = data_len / 3;
                sensor_act_cache[0] = (sensor_count & 0xFF00) >> 8;
                sensor_act_cache[1] = (sensor_count & 0x00FF);
                memcpy(sensor_act_cache + 2, data, data_len);
            }
            LOGS("%d:00 次数%d", data[data_len - 3], tmpp);
            if (sensor_count > 0 && sensor_count < 25)
                track_cust_jm66_paket_94_handle(0x16, sensor_act_cache, data_len + 2);
            else
                LOGD(L_OBD, L_V5, "No data %d,%d", sensor_count, data_len);
            break;
        }
//===========================================================
        case TBT100_GET_ENVDATA ://读取环境数据
            is_ok = KAL_TRUE;
            break;
//===========================================================
        case TBT100_EVENT_COLLAR_BROKEN ://项圈剪断报警
        case TBT100_EVENT_TEARDOWN ://外壳拆开报警
        case TBT100_EVENT_OUTLIER ://牦牛离群报警
        {
            if (cmd == TBT100_EVENT_COLLAR_BROKEN)
                LOGS("项圈剪断报警");
            else if (cmd == TBT100_EVENT_TEARDOWN)
                LOGS("外壳拆开报警");
            else if (cmd == TBT100_EVENT_OUTLIER)
                LOGS("牦牛离群报警");
            if (is_quit_transport_boot || track_is_timer_run(TRACK_CUST_FIRST_BOOT_DELAY_SHUTDOWN_TIMER_ID) && !track_is_testmode() && !track_is_autotestmode())
            {
                LOGD(L_OBD, L_V5, "本次开机是退出运输模式,不响应报警%02X", cmd);
                return;
            }
            LOGD(L_OBD, L_V5, "Current Mode:%d", Jm66_Current_mode);
            if (cmd == TBT100_EVENT_COLLAR_BROKEN)
            {
                if (Jm66_Current_mode == 13)return;
                Jm66_Current_mode = 13;
            }
            else if (cmd == TBT100_EVENT_TEARDOWN)
            {
                if (Jm66_Current_mode == 12)return;
                Jm66_Current_mode = 12;
            }
            else if (cmd == TBT100_EVENT_OUTLIER)
            {
                if (Jm66_Current_mode == 11)return;
                Jm66_Current_mode = 11;
            }
            if (kal_get_systicks() < 1200)
            {
                track_cust_tbt100_time_sync(KAL_FALSE, NULL);
            }
            track_cust_send_alarm(cmd);
            track_cust_into_tracking_mode(1);
            return;
        }
//===========================================================
        case TBT100_EVENT_KEYDOWN : //按键按下
        {
            LOGS("按键按下");
            LOGD(L_OBD, L_V5, "Current Mode:%d", Jm66_Current_mode);
            if (Jm66_Current_mode < 2)
            {
                Jm66_Current_mode = 2;
                if (kal_get_systicks() < 1200)
                {
                    track_cust_tbt100_time_sync(KAL_FALSE, NULL);
                }
                track_cust_into_tracking_mode(1);
            }
            return;
        }
//===========================================================
        case TBT100_EVENT_TIMING : //"定时唤醒"
            LOGS("定时唤醒");
            if (kal_get_systicks() < 1200)
            {
                track_cust_tbt100_time_sync(KAL_FALSE, NULL);
            }
            return;
//===========================================================
        case TBT100_SET_NEXTBOOT://设置下次开机时间
            if (0 == data[0])
            {
                is_ok = KAL_TRUE;
                LOGD(L_OBD, L_V5, "设置下次开机时间成功");
                if (will_shutdown)
                {
                    track_cust_tbt100_shutdown_req();
                }
            }
            else LOGD(L_OBD, L_V5, "设置下次开机时间成功失败");
            break;
//===========================================================
        case TBT100_GET_BT_BASEVERSION ://获取蓝牙基础版本号
            is_ok = KAL_TRUE;
            memcpy(tbt100_bt_baseversion, data, data_len);
            LOGS("蓝牙基础版本:%s", tbt100_bt_baseversion);
            break;
//===========================================================
        case TBT100_TRANSPORT ://运输模式
            if (0 == data[0])
            {
                is_ok = KAL_TRUE;
                confirm("请求进入运输模式成功");
                memset(&G_realtime_data.cell_id[0], 0, LBS_LIST_MAX * 2);
                G_realtime_data.valid_count = 0;
                G_realtime_data.is_first_boot = 1;
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                track_soc_clear_queue();
                //track_drv_sys_power_off_req();
            }
            else confirm("请求进入运输模式失败");
            break;
//===========================================================
        case TBT100_SYNC_TIME ://时间同步
            if (data[0] == 0)//读取
            {
                is_ok = KAL_TRUE;
                tbt100_bt_time.nYear = data[1] + ((data[2] & 0x00FF)<<8);
                tbt100_bt_time.nMonth = data[3];
                tbt100_bt_time.nDay = data[4];
                tbt100_bt_time.nHour = data[5];
                tbt100_bt_time.nMin = data[6];
                tbt100_bt_time.nSec = data[7];
                snprintf(confirm_buf, sizeof(confirm_buf),"蓝牙时间:%0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", tbt100_bt_time.nYear, tbt100_bt_time.nMonth, tbt100_bt_time.nDay,
                         tbt100_bt_time.nHour, tbt100_bt_time.nMin, tbt100_bt_time.nSec);
                confirm(confirm_buf);
                if (is_first)
                {
                    U16 min_index, min = 0xFFFF, n,tmp, current_min = 0;
                    applib_time_struct utctime = {0};
                    nvram_gps_time_zone_struct  zone1 = {0};
                    is_first = KAL_FALSE;
                    memcpy(&zone1, &G_parameter.zone, sizeof(nvram_gps_time_zone_struct));
                    zone1.zone = 'W'; //往前
                    track_drv_utc_timezone_to_rtc(&utctime, &tbt100_bt_time, zone1);
                    track_fun_update_time_to_system(4, &utctime);
                    if (is_quit_transport_boot == 1)
                    {
                        LOGD(L_OBD, L_V5, "退出运输模式开机,开启GPS,30秒后扫描基站,3分钟不响应任何报警,之后关机");
                        Jm66_Current_mode = 1;
                        tr_start_timer(TRACK_CUST_DELAY_COMPARE_LBS_TIMER_ID, 30 * 1000, track_cust_moudle_compare_lbs);
                        track_cust_gps_work_req((void *)2);
                        tr_start_timer(TRACK_CUST_FIRST_BOOT_DELAY_SHUTDOWN_TIMER_ID, 180 * 1000, track_cust_jm66_shutdown);
                    }
                    else
                    {
                        if (Jm66_Current_mode == 0)//定时唤醒,需判断GPS/SID
                        {
                            current_min = tbt100_bt_time.nHour * 60 + tbt100_bt_time.nMin;
                            for (n = 0; n < G_parameter.mtworkt_count; n++)
                            {
                                tmp = OTA_abs(current_min - G_parameter.mtworkt[0][n]);
                                if (tmp <= min)
                                {
                                    min = tmp;
                                    min_index = n;
                                }
                            }
                            LOGD(L_OBD, L_V5, "#%d %0.2d:%0.2d %d", min_index + 1, G_parameter.mtworkt[0][min_index] / 60, G_parameter.mtworkt[0][min_index] % 60, G_parameter.mtworkt[1][min_index]);
                            if (G_parameter.mtworkt[1][min_index] == 2)
                            {
                                Jm66_Current_mode = 1;
                                LOGD(L_OBD, L_V5, "GPS:开启");
                                track_cust_gps_work_req((void *)2);
                                track_cust_worktime_count();
                                track_cust_tbt100_get_sensoractive_times();
                                track_cust_tbt100_req_scan();
                            }
                            else
                            {
                                int ret = 99;
                                Jm66_Current_mode = 0;
                                if (track_drv_PA_control(99) == 0)
                                {
                                    ret = track_drv_PA_control(1);//关PA
                                }
                                LOGD(L_OBD, L_V5, "SID:30秒后扫描基站%d", ret);
                                tr_start_timer(TRACK_CUST_DELAY_COMPARE_LBS_TIMER_ID, 30 * 1000, track_cust_moudle_compare_lbs);
                            }
                        }
                    }
                }
            }
            else if (data[0] == 1)//设置
            {
                if (data[1] == 0)
                {
                    is_ok = KAL_TRUE;
                    confirm("设置蓝牙时间成功");
                    track_cust_tbt100_time_sync(KAL_FALSE, NULL);
                }
                else confirm("设置蓝牙时间失败");
            }
            break;
//===========================================================
        case TBT100_SHUTDOWN_REQ ://请求关机
            if (data[0] == 0)
            {
                confirm("请求关机成功");
                is_ok = KAL_TRUE;
            }
            else
            {
                confirm("请求关机失败");
            }
            break;
//===========================================================
        case TBT100_TELL_GSM_SHUTDOWN ://蓝牙关闭GSM事件
            LOGD(L_OBD, L_V5, "蓝牙即将关闭GSM");
            return;
//===========================================================
        case TBT100_TELL_BT_VBAT ://GSM通知蓝牙电池电压
            is_ok = KAL_TRUE;
            break;
//===========================================================
        case TBT100_GET_BT_RESET_TIMES ://查询蓝牙硬件复位次数
            is_ok = KAL_TRUE;
            tbt100_bt_reset_times = data[0] + ((data[1] & 0x00FF)<<8);
            snprintf(confirm_buf, sizeof(confirm_buf),"蓝牙复位次数:%d", tbt100_bt_reset_times);
            confirm(confirm_buf);
            break;
//===========================================================
        case TBT100_GET_BT_MAC ://查询蓝牙MAC地址
            is_ok = KAL_TRUE;
            if (data_len == 6)
            {
                memcpy(tbt100_bt_mac, data, data_len);
                snprintf(confirm_buf, sizeof(confirm_buf),"蓝牙地址:%02X:%02X:%02X:%02X:%02X:%02X",
                         tbt100_bt_mac[0], tbt100_bt_mac[1], tbt100_bt_mac[2],
                         tbt100_bt_mac[3], tbt100_bt_mac[4], tbt100_bt_mac[5]);
                confirm(confirm_buf);
            }
            else confirm("无效的蓝牙地址");
            break;
//===========================================================
        case TBT100_GET_BTVBAT ://查询蓝牙电池电压
        {
            U16 raw;
            is_ok = KAL_TRUE;
            raw = data[0] + ((data[1] & 0x00FF)<<8);
            tbt100_bt_vbat = raw + 160;
            snprintf(confirm_buf, sizeof(confirm_buf),"蓝牙电池电压:%d/%d", raw, tbt100_bt_vbat);
            confirm(confirm_buf);
            break;
        }
//===========================================================
        case TBT100_EVENT_CONNECT ://蓝牙连接与断开事件
            if (data[0] == 0)
            {
                LOGS("蓝牙断开");
            }
            else if (data[0] == 1)
            {
                LOGS("蓝牙连接");
            }
            return;
//===========================================================
        case TBT100_GET_KEY ://获取蓝牙通信密钥
            if (data_len == 0x16)
            {
                is_ok = KAL_TRUE;
                memcpy(tbt100_get_AES_key, data, data_len);
            }
            else LOGD(L_OBD, L_V5, "错误的AESKEY,重新获取");
            break;
//===========================================================
        case TBT100_SET_KEY ://设置蓝牙通信密钥
            if (0 == data[0])
            {
                is_ok = KAL_TRUE;
                LOGD(L_OBD, L_V5, "设置AESKEY成功");
            }
            else LOGD(L_OBD, L_V5, "设置AESKEY失败");
            break;
//===========================================================
        case TBT100_BTTEST ://蓝牙测试通道
            LOGH(L_OBD, L_V1, data, data_len);
            return;
//===========================================================
        case TBT100_BT_RESET ://蓝牙系统复位
            if (data[0] == 0)
            {
                is_ok = KAL_TRUE;
                LOGS("蓝牙复位成功");
            }
            else LOGS("蓝牙复位失败");
            break;
//===========================================================
        case TBT100_CLEAR_BTLOG ://清除蓝牙日志
            if (data[0] == 0)
            {
                LOGD(L_OBD, L_V5, "清除蓝牙日志成功");
            }
            else LOGD(L_OBD, L_V5, "清除蓝牙日志失败");
            return;
//===========================================================
        case TBT100_GET_BT_APPVERSION ://查询软件版本信息
            memcpy(tbt100_bt_appversion, data, data_len);
            snprintf(confirm_buf, sizeof(confirm_buf),"蓝牙应用版本:%s", tbt100_bt_appversion);
            confirm(confirm_buf);
            return;
//===========================================================
        case TBT100_UPDATE_START ://开始升级
            if (*data == 0)
            {
                LOGD(L_OBD, L_V5, "蓝牙接受开始升级");
                is_update_mode = 1;
                update_image_offset += 12;
                track_cust_tbt100_update_send();
            }
            else
            {
                LOGD(L_OBD, L_V5, "蓝牙开始升级失败");
                track_cust_tbt100_update_quit();
            }
            return;
        case TBT100_UPDATE_IMAGE ://升级镜像文件
        {
            static U8 retry_count = 0;
            if (*data == 0)
            {
                if (is_first_package)is_first_package = FALSE;
                LOGD(L_OBD, L_V5, "@@Image Recept!!Index:%d", index_send);
                index_send++;
                update_image_offset += this_time_send_len;
                track_cust_tbt100_update_send();
            }
            else
            {
                retry_count++;
                if (retry_count >= 3)
                {
                    retry_count = 0;
                    track_cust_tbt100_update_quit();
                }
                else
                {
                    LOGD(L_OBD, L_V5, "Image失败,重试%d", retry_count);
                    track_cust_tbt100_update_send();
                }
            }
            return;
        }
        case TBT100_UPDATE_CHECKFILE ://升级文件校验
            if (*data == 0)
            {
                LOGD(L_OBD, L_V5, "文件校验成功");
                track_cust_tbt100_update_end();
            }
            else
            {
                LOGD(L_OBD, L_V5, "文件校验失败");
                track_cust_tbt100_update_quit();
            }
            return;
        case TBT100_UPDATE_END ://升级结束
            if (*data == 0)
            {
                LOGD(L_OBD, L_V5, "升级结束");
                track_tbt100_quit_update();
            }
            else
            {
                LOGD(L_OBD, L_V5, "升级结束失败");
                track_cust_tbt100_update_quit();
            }
            return;
        case TBT100_QUIT_UPDATE ://结束升级模式
            return;
//===========================================================
        case TBT100_REQ_SCAN:
            LOGD(L_OBD, L_V5, "蓝牙开始扫描");
            is_ok = KAL_TRUE;
            break;
        case TBT100_EVENT_CATTLE ://牛群信息
        {
            U32 this_time_mac_len;
            U32 mac_count = data[6];
            U32 n;
            char m_buf[ONETIME_MAC_MAX_LEN * 5] = {0};
            this_time_mac_len = mac_count * 6;
            if (is_cattle_first)
            {
                cattlelen = 0;
                memset(cattle_cache, 0, PACKET_MAC_MAX_LEN);
                is_cattle_first = KAL_FALSE;
                memcpy(cattle_cache + 2, data, 6);//填充时间
                memcpy(cattle_cache + 8, tbt100_bt_mac, 6);//填充本机MAC
                cattlelen += 14;
            }
            for (n = 0; n < mac_count; n++)
            {
                snprintf(m_buf + strlen(m_buf), 99, "MAC_%d:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
                         n+1, data[7 + n*6], data[8 + n*6], data[9 + n*6], data[10 + n*6], data[11 + n*6], data[12 + n*6] );
            }
            LOGS("%s", m_buf);
            if (cattlelen + this_time_mac_len >= PACKET_MAC_MAX_LEN)//需要发包了
            {
                count = (cattlelen - 8)/6;
                LOGD(L_OBD, L_V1, "打包牛群信息%d/%d", cattlelen, PACKET_MAC_MAX_LEN);
                cattle_cache[0] = (count & 0xFF00) >> 8;
                cattle_cache[1] = (count & 0x00FF);
                track_cust_jm66_paket_94_handle(0x15, cattle_cache, cattlelen);
                memset(cattle_cache, 0, PACKET_MAC_MAX_LEN);
                cattlelen = 0;
                memcpy(cattle_cache + 2, data, 6);//填充时间
                memcpy(cattle_cache + 8, tbt100_bt_mac, 6);//填充本机MAC
                cattlelen += 14;
                memcpy(cattle_cache + cattlelen, &data[7], this_time_mac_len);
                cattlelen += this_time_mac_len;
            }
            else
            {
                memcpy(cattle_cache + cattlelen, &data[7], this_time_mac_len);
                cattlelen += this_time_mac_len;
            }
            LOGD(L_OBD, L_V1, "收到牛群信息%d/%d", cattlelen, PACKET_MAC_MAX_LEN);
            return;
        }
        case TBT100_EVENT_CATTLE_END://牛群信息结束
            //track_cust_tbt100_packet_send(TBT100_EVENT_CATTLE_END, NULL, 0, 0);
            count = (cattlelen - 8)/6;
            LOGD(L_OBD, L_V1, "牛群信息结束,打包牛群信息%d/%d", cattlelen, PACKET_MAC_MAX_LEN);
            cattle_cache[0] = (count & 0xFF00) >> 8;
            cattle_cache[1] = (count & 0x00FF);
            track_cust_jm66_paket_94_handle(0x15, cattle_cache, cattlelen);
            is_cattle_first = KAL_TRUE;
            memset(cattle_cache, 0, PACKET_MAC_MAX_LEN);
            cattlelen = 0;
            return;
        case TBT100_EVENT_FIRST_BOOT://首次开机
            LOGS("首次开机");
            is_quit_transport_boot = 1;
            track_cust_tbt100_time_sync(KAL_FALSE, NULL);
            return;
        case TBT100_SET_IMEI:
            if (*data == 0)
            {
                LOGD(L_OBD, L_V1, "设置IMEI成功");
                is_ok = KAL_TRUE;
            }
            else
                LOGD(L_OBD, L_V1, "设置IMEI失败");
            break;
        default:
            LOGD(L_OBD, L_V1, "未定义的协议号");
            return;
    }

    if (is_ok == KAL_TRUE)track_mlsp_send((void*)cmd);
    else track_mlsp_send((void*)0xFF);

}

void track_cust_module_power_data(kal_uint8 *data, int len)//收到串口数据
{
    U32 len32 = 0;
    U16 mycrc, hiscrc;
    LOGH(L_OBD, L_V1, data, len);
    if (data[0] == 0x42 && data[1] == 0x54)
    {
        mycrc = CRC16(data, len - 2);
        hiscrc = data[len - 2] + ((data[len - 1] & 0x00FF) << 8);
        if (mycrc != hiscrc)
        {
            LOGD(L_OBD, L_V1, "CRC错误,my:0x%04X,his:0x%04X", mycrc, hiscrc);
            return;
        }
        len32 = data[3] + ((data[4] & 0x00FF) << 8);
        LOGD(L_OBD, L_V1, "收到协议号:0x%02X,数据内容长度:%d", data[2], len32);
        track_cust_module_tbt100_decode(data[2], &data[5], len32);
    }
}
#endif//__JM66__
