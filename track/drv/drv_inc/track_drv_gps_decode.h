/******************************************************************************
 * track_gps_data_decode.h -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        GPS ����
 *
 * modification history
 * --------------------
 * v1.0   2012-07-24,  chengjun create this file
 *
 ******************************************************************************/

#ifndef _TRACK_DRV_GPS_DECODE_H
#define _TRACK_DRV_GPS_DECODE_H

/****************************************************************************
* Include Files             ����ͷ�ļ�
*****************************************************************************/
#include "track_os_data_types.h"
#include "custom_mmi_default_value.h"
#include "kal_public_defs.h"
#include "mtk_gps_type.h"
#include "track_drv_lbs_data.h"

/*****************************************************************************
 *  Define			    �궨��
*****************************************************************************/
//#define GPS_DECODE_DEBUG

#define NMEA_BUFF_SIZE  200

/*****************************************************************************
* Typedef  Enum         ö��
*****************************************************************************/
typedef enum
{
    GPS,
    BD,
    GN,
    GLONASS,
    GPS_GLONASS,
    BDS_GLONASS,
    GPS_BDS_GLONASS,
    GPS_MODE_MAX
}enum_gps_mode;

/*****************************************************************************
 *  Struct			    ���ݽṹ����
*****************************************************************************/
typedef struct
{
    kal_uint8 nHour;//ʱ��
    kal_uint8 nMin;
    kal_uint8 nSec;
    float Latitude;//γ��
    kal_uint8 NS;
    float Longitude;//����
    kal_uint8 EW;
    kal_uint8 posi_fix;//GPS״̬
                               //0=������/1=���㶨λ/2=��ֶ�λ/3-5 =��֧��/6-���ڹ���
    kal_uint8 sat;//��ǰʹ�õ�������
    float HDOP;
    float msl_altitude;//����
    kal_uint8  Units1;//��λm
    float  Geoid_Altitude;//������������Դ��ˮ׼��߶�
    kal_uint8  Units2;//��λm
    float Age;//���ʱ��
    kal_uint16 station_ID;//���վID
} gps_gpgga_struct;

typedef struct
{
    kal_uint8 mode;//��λģʽA=�����Զ�2D  ��3D �л�/M=ǿ����2D ��3D����
    kal_uint8 status;//��λ����1=δ��λ/2=2D��λ/3=3D��λ
    kal_uint8 satellite_used[MTK_GPS_SV_MAX_NUM];//PRN��(α���������)
    kal_uint8 satellite_SNR_used[MTK_GPS_SV_MAX_NUM];//�����
    kal_uint8 satellite_SNR_used_average;//��ǿ3����ƽ�������
    float  PDOP;
    float  HDOP;
    float  VDOP;
    kal_uint8  systemid;   //����ϵͳID
} gps_gpgsa_struct;

typedef struct
{
    kal_uint8 Number_Of_Messages;//�������
    kal_uint8 index;//�����������ĵڼ���
    kal_uint8 Satellites_In_View;//�ɼ�������,MTK �������ʱ�׼��ʵ�ʲ��4*4
    kal_uint8 Satellite_ID[MTK_GPS_SV_MAX_NUM];//PRN��
    kal_uint8 Elevation[MTK_GPS_SV_MAX_NUM];//��������
    kal_uint16 Azimuth[MTK_GPS_SV_MAX_NUM];//���Ƿ�λ��
    kal_uint8 SNR[MTK_GPS_SV_MAX_NUM];//�����
    kal_uint8 SignalId; //NMEA ������� GNSS �ź� ID
} gps_gpgsv_struct;


typedef struct
{
    MYTIME Date_Time;//ʱ������
    kal_uint8 status;//״̬�� 0=����λ / 1=��λ / 2=����λ(�޸Ĺ�ʱ���) ��
    float Latitude;//γ��
    kal_uint8 NS;
    float Longitude;//����
    kal_uint8 EW;
    float Speed;//�ٶ�( �����λתΪkm/h)
    float Course;//��λ��
    kal_uint8 Magnetic_variation2;//��ƫ��
    kal_uint8 Indicater2;//��ƫ�Ƿ���(E/W)
    kal_uint8 Mode;
    kal_uint8 NavStatus; //����״̬��ʾ��
} gps_gprmc_struct;


typedef struct
{
    float Course0;//�˶��Ƕ�
    kal_uint8  Reference0;//�汱�ο�ϵ
    float Course1;//�˶��Ƕ�
    kal_uint8  Reference1;//�ű��ο�ϵ
    float Speed0;//�ٶ�
    kal_uint8 Units0;//��λ:��
    float Speed1;//�ٶ�
    kal_uint8 Units1;//��λ:km/h
    kal_uint8 Mode;
} gps_gpvtg_struct;

typedef struct
{
    kal_uint8 status;//�����ʾ
    kal_uint8 t_time;//ʱ����Ч��
    kal_uint8 NS;//��Ч��������
}gps_gptxt_struct;

typedef struct
{
    float Latitude;
    kal_uint8 NS;
    float Longitude;
    kal_uint8 EW;
    kal_uint8 nHour;
    kal_uint8 nMin;
    kal_uint8 nSec;
    kal_uint8 Status;
    kal_uint8 Mode;
} gps_gpgll_struct;

typedef struct
{
    kal_uint8  Signal_Strength;
    kal_uint8  Signal_Ratio;
    float   Beacon_Frequency;
    kal_uint8   Beacon_Bit_Rate;
    kal_uint8   Channel_Number;
} gps_gpmss_struct;



typedef struct
{
    MYTIME Date_Time;
    kal_uint8  Local_zone_hour ;
    kal_uint8  Local_zone_minutes;
} gps_gpzda_struct;

typedef kal_int8 (*gpsFuncPtrEx) (enum_gps_mode, char*);
    
typedef struct
{
    const S8        *cmd_string;
    gpsFuncPtrEx    FuncPtr;
    enum_gps_mode   gw_mode;
} GPS_Decode_struct;

typedef struct
{
    LOCAL_PARA_HDR
    mtk_gps_position pData;
} gps_nmea_decode_req_struct;



typedef struct
{
    kal_uint8 status;//0=gps off 1=δ��λ 2=2D ��λ 3=3D ��λ����$GPGSA �ĵ�2�ֶ�ͬ��
    kal_uint8 filter;//����20Ϊ��ǳɹ��˵���������
    enum_gps_mode     gw_mode;
    gps_gpgga_struct  gpgga;
    gps_gpgsa_struct  gpgsa;
    gps_gpgsv_struct  gpgsv; 
    gps_gprmc_struct  gprmc;
    gps_gpvtg_struct  gpvtg;
    
    gps_gpgga_struct  bdgga;
    gps_gpgsa_struct  bdgsa;
    gps_gpgsv_struct  bdgsv; 
    gps_gprmc_struct  bdrmc;
    gps_gpvtg_struct  bdvtg;
    
    Multi_Cell_Info_Struct   lbs;
} track_gps_data_struct;//GPS �������������
typedef struct
{
    LOCAL_PARA_HDR
    track_gps_data_struct pData;
} gps_nmea_decode_req_struct2;

typedef struct
{
    LOCAL_PARA_HDR
    kal_uint16 len;
}gps_nema_req_string;

typedef struct
{
    kal_bool valid;
    float altitude;
    gps_gprmc_struct  gprmc;
} track_gps_epo_struct;//����AGPS  EPO 
/*track_gps_data_struct �����ϲ��ȡGPS ���ݣ�ʹ��ǰ��ʱ��У׼��
    ������ԭʼ���ݣ���������AGPS EPO ����*/

/*****************************************************************************
* Local variable            �ֲ�����
*****************************************************************************/


/****************************************************************************
* Global Variable           ȫ�ֱ���
*****************************************************************************/


/****************************************************************************
* Global Variable - Extern          ����ȫ�ֱ���
*****************************************************************************/


/*****************************************************************************
 *  Global Functions	- Extern	    �����ⲿ����
*****************************************************************************/
extern void track_drv_test_gps_fix_time(void);

/*****************************************************************************
 *  Local Functions	            ���غ���
*****************************************************************************/
static void track_gps_data_decode_backup(enum_gps_mode gps_working);

/*****************************************************************************
 *  Global Functions            ȫ�ֺ���
*****************************************************************************/
extern void track_gps_data_clean(void);

#endif  /* _TRACK_DRV_GPS_DECODE_H */

