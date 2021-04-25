/******************************************************************************
 * track_cust_dasouche.h - 
 * 
 * Copyright .
 * 
 * DESCRIPTION: - 
 *        TRAKC_CUST_DASOUCHE 
 * 
 * modification history
 * --------------------
 * v1.0   2018-12-07,  chenjiajun create this file
 * 
 ******************************************************************************/
#if defined(__DASOUCHE__)

#ifndef _TRACK_CUST_DASOUCHE_H
#define _TRACK_CUST_DASOUCHE_H

/****************************************************************************
*  Include Files                        包含头文件
*****************************************************************************/
//#include "track_drv_gps_decode.h"
//#include "track_drv_lbs_data.h"

/*****************************************************************************
*  Define                               宏定义
******************************************************************************/
#define DSC_Ext_LowBattery    "001" 
#define DSC_PowerFailure      "002" 
#define DSC_PULL_ALM          "004" 
#define DSC_Speed_Limit       "007" 
#define DSC_Vibration         "009" 
#define DSC_SPEED_UP_ALM      "011" 
#define DSC_SPEED_DOWN_ALM    "012" 
#define DSC_TURN_R_L_ALM      "013" 

#define DSC_TOPIC_LOCATIN 0
#define DSC_TOPIC_RESPONSE 1
#define DSC_TOPIC_ACCEPTOR 2
/*****************************************************************************
*  Typedef  Enum                        枚举定义
*****************************************************************************/
typedef enum
{
    DSC_CUST_POSITION_ACC_CHANGE_ON          =    1,    // ACC 状态改变上传 关->开   
    DSC_CUST_POSITION_ACC_CHANGE_OFF         =    2,    // ACC 状态改变上传 开->关
    DSC_CUST_POSITION_TIMING                 =    3,    // 定时上报
    DSC_CUST_POSITION_ANGLEREP               =    4,    // 拐点上传
    DSC_CUST_POSITION_BLIND                  =    5,    // 盲区上传
    DSC_CUST_LJDW_POSITION                   =    6,    // 立即定位上传
    DSC_CUST_POSITION_STATIC                 =    7,    // 从运动变为静止状态后，补传最后一个定位点
    DSC_CUST_ALARM                           =    8,    // 报警包   
    DSC_CUST_POSITION_MAX
} enum_MQTT_POSITION_TYPE;

typedef enum
{
	LOCA_NONE,
	LOCA_GPS,
	LOCA_MORE_LBS,
	LOCA_ONE_LBS,//3单基站
	LOCA_WIFI = 6
}enum_location_type;
/*****************************************************************************
*  Struct                               数据结构定义
******************************************************************************/

typedef struct
{
   kal_uint16 staticmode1time;
   kal_uint16 staticmode2time;
} Statictime_Data_Struct;

typedef struct
{
   kal_uint8 GPSSW;
   kal_uint8 WFSW;
} Locationsw_Data_Struct;

typedef struct
{
    kal_uint8 status;//状态【 0=不定位 / 1=定位 / 2=不定位(修改过时间的) 】
    //kal_uint32 timestamp;//时间戳
    float Latitude;//纬度
    kal_uint8 NS;
    float Longitude;//经度
    kal_uint8 EW;
    float Speed;//速度( 解码后单位转为km/h)
    float Course;//方位角
    float altitude;//海拔
    //kal_uint32 mileage;//里程
    //kal_uint8 acc; //ACC状态
    //kal_int8 lastPower;
    kal_uint8 location_type;//定位类型    
    kal_uint8 alarm_type;   //报警类型
    kal_uint8 dataDesc;//数据包类型
    LBS_Multi_Cell_Data_Struct lbs; //LBS数据
#if defined(__WIFI__)    
    track_wifi_struct   wifi;//wifi数据
#endif    
    applib_time_struct locationtime;
    kal_uint8 light_status;
    float temp ;
    float humidity ;
    kal_uint8 battery;
    kal_uint8 charge_Status;
    kal_uint8 lightSensorStatus;
    kal_uint8 mode;
    kal_uint16 mode_time;
    kal_uint8 runStatus;
    kal_uint8 humitureStatus;
    kal_uint8 upload_type;
    Statictime_Data_Struct staticmode;
    Locationsw_Data_Struct locationsw;
    kal_uint32 volt;
    
} dasouche_location_struct;

/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/

/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/
#endif  /* _TRACK_CUST_DASOUCHE_H */
#endif /* __DASOUCHE__ */
