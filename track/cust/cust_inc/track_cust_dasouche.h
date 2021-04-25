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
*  Include Files                        ����ͷ�ļ�
*****************************************************************************/
//#include "track_drv_gps_decode.h"
//#include "track_drv_lbs_data.h"

/*****************************************************************************
*  Define                               �궨��
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
*  Typedef  Enum                        ö�ٶ���
*****************************************************************************/
typedef enum
{
    DSC_CUST_POSITION_ACC_CHANGE_ON          =    1,    // ACC ״̬�ı��ϴ� ��->��   
    DSC_CUST_POSITION_ACC_CHANGE_OFF         =    2,    // ACC ״̬�ı��ϴ� ��->��
    DSC_CUST_POSITION_TIMING                 =    3,    // ��ʱ�ϱ�
    DSC_CUST_POSITION_ANGLEREP               =    4,    // �յ��ϴ�
    DSC_CUST_POSITION_BLIND                  =    5,    // ä���ϴ�
    DSC_CUST_LJDW_POSITION                   =    6,    // ������λ�ϴ�
    DSC_CUST_POSITION_STATIC                 =    7,    // ���˶���Ϊ��ֹ״̬�󣬲������һ����λ��
    DSC_CUST_ALARM                           =    8,    // ������   
    DSC_CUST_POSITION_MAX
} enum_MQTT_POSITION_TYPE;

typedef enum
{
	LOCA_NONE,
	LOCA_GPS,
	LOCA_MORE_LBS,
	LOCA_ONE_LBS,//3����վ
	LOCA_WIFI = 6
}enum_location_type;
/*****************************************************************************
*  Struct                               ���ݽṹ����
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
    kal_uint8 status;//״̬�� 0=����λ / 1=��λ / 2=����λ(�޸Ĺ�ʱ���) ��
    //kal_uint32 timestamp;//ʱ���
    float Latitude;//γ��
    kal_uint8 NS;
    float Longitude;//����
    kal_uint8 EW;
    float Speed;//�ٶ�( �����λתΪkm/h)
    float Course;//��λ��
    float altitude;//����
    //kal_uint32 mileage;//���
    //kal_uint8 acc; //ACC״̬
    //kal_int8 lastPower;
    kal_uint8 location_type;//��λ����    
    kal_uint8 alarm_type;   //��������
    kal_uint8 dataDesc;//���ݰ�����
    LBS_Multi_Cell_Data_Struct lbs; //LBS����
#if defined(__WIFI__)    
    track_wifi_struct   wifi;//wifi����
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
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/

/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/
#endif  /* _TRACK_CUST_DASOUCHE_H */
#endif /* __DASOUCHE__ */
