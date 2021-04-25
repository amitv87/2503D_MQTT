#ifndef _TRACK_NVRAM_STRUCT_C_
#define _TRACK_NVRAM_STRUCT_C_

#include "audio_nvram_def.h"
#include "kal_general_types.h"
#include "nvram_data_items.h"
#include "custom_mmi_default_value.h"
#include "ps_public_enum.h"

typedef struct
{
    kal_uint8       start_flag;         // �������
    kal_uint8       error_index;        // error index
    kal_uint8       errorCount;
    kal_uint8       error[20];          //
    kal_uint32      errortime[20];      //
    kal_uint32      start_count;        // ������߼����������Ĵ���ͳ��
} nvram_sys_abnormal_data_struct;


typedef struct
{
    //kal_uint16    total;          //�����GPS����������
    kal_uint16  data_first;
    kal_uint16  data_last;          //ָ����һ������GPS���ݵĿռ�
} nvram_gps_backup_data_param_struct;
/*  end  NVRAM���ݶ�λ����   */

typedef struct
{
    MYTIME      Date_Time;
    kal_uint8   status;
    float       Latitude;
    kal_uint8   NS;
    float       Longitude;
    kal_uint8   EW;
    float       Speed;
    float       Course;
    kal_uint8   Magnetic_variation2;
    kal_uint8   Indicater2;
    kal_uint8   Mode;
} nvram_gps_gprmc_struct;

typedef struct
{
    kal_uint8 nHour;//ʱ��
    kal_uint8 nMin;
    kal_uint8 nSec;
    float     Latitude;//γ��
    kal_uint8 NS;
    float     Longitude;//����
    kal_uint8 EW;
    kal_uint8 posi_fix;//GPS״̬ 0=������/1=���㶨λ/2=��ֶ�λ/3-5 =��֧��/6-���ڹ���
    kal_uint8 sat;//��ǰʹ�õ�������
    float     HDOP;
    float     msl_altitude;//����
    kal_uint8 Units1;//��λm
    float     Geoid_Altitude;//������������Դ��ˮ׼��߶�
    kal_uint8 Units2;//��λm
    float     Age;//���ʱ��
    kal_uint16 station_ID;//���վID
} nvram_gps_gpgga_struct;
typedef struct
{
    kal_uint16 mcc;
    kal_uint16 mnc;
    kal_uint16 lac;
    kal_uint16 cell_id;
    kal_int16 rxlev;          /*  rxlev  : ͨ��catcher ��Ϣ�鿴�����ֵӦ���Ǳ��������źŵ�ƽ�����źŵȼ�ֵ�������������ź�ǿ��ֵ */
    /* track_drv_convert_rxlev_to_rssi  -�źŵȼ���ת��Ϊ�ź�ǿ�� -- chengjun  2012-03-08*/
} nvram_Multi_Cell_Info_Struct;

typedef struct
{
    nvram_gps_gprmc_struct rmc;
    nvram_gps_gpgga_struct gga;
    nvram_Multi_Cell_Info_Struct     lbs;
    kal_uint32  distance;
} nvram_gps_last_data_struct;



typedef struct
{
    kal_uint8   flag;//0:Ĭ�����ļ�;1:�����ļ�;2:�ļ���������;
    kal_uint8   index;
    kal_uint8   unconditional;
    kal_uint8   filemd5[16];
    kal_uint8   version[100];
    kal_uint8   server[100];
    kal_uint16  port;
    kal_uint8   dateCount[3];   /*������*/
    kal_uint8   resetCount;     /*Ĭ��ÿ�����100��*/
    kal_uint8   resetDay;       /*Ĭ���������10��*/
    kal_uint8   resetCountMax;  /*Ĭ��ÿ�����100��*/
    kal_uint8   resetDayMax;    /*Ĭ���������10��*/
} nvram_check_ota_struct;



typedef struct
{
    kal_int8           sensor_type;            //sensor ����
} nvram_drv_parameter_struct;


#endif

