/******************************************************************************
 * track_drv.h - 
 * 
 * Copyright .
 * 
 * DESCRIPTION: - 
 *        ���������ӿ� 
 * 
 * modification history
 * --------------------
 * v1.0   2012-07-26,  chengjun create this file
 * 
 ******************************************************************************/
#ifndef DRV_INC_H
#define DRV_INC_H
/****************************************************************************
* Include Files             ����ͷ�ļ�                                             
*****************************************************************************/
#include "GlobalConstants.h"
#include "track_drv_gps_drive.h"
#include "track_drv_gps_decode.h"
#include "track_drv_led.h"
#include "track_drv_pmu.h"
#include "track_drv_eint_gpio.h"
#include "track_drv_lbs_data.h"
#include "track_drv_system_param.h"
#include "track_drv_sensor.h"
#include "track_drv_key.h"
#include "track_drv_bt.h"
#include "track_os_paramete.h"

/*****************************************************************************
 *  Define			    �궨��
*****************************************************************************/
#if defined(__GT530__) ||defined(__GT02F__) ||defined(__BD_AT6558__)
#define GPS_UART_PORT               uart_port3
#define U_TX                1
#define U_TX_M          3
#define U_RX                0
#define U_RX_M          3
//#elif defined __GPS_MT3333__
//#define GPS_UART_PORT               uart_port2
//#define U_TX            12 
//#define U_TX_M          1
//#define U_RX            17
//#define U_RX_M          1
#else
#define GPS_UART_PORT               uart_port2
#define U_TX            21 
#define U_TX_M          1
#define U_RX            20
#define U_RX_M          1
#endif

#define RUNING_TIME_SEC ((kal_get_systicks() - 0) / KAL_TICKS_1_SEC)

/***************************************************************************** 
* Typedef  Enum         ö��
*****************************************************************************/


/*****************************************************************************
 *  Struct			    ���ݽṹ����
*****************************************************************************/


/*****************************************************************************
 *  Global Functions            ȫ�ֺ���
*****************************************************************************/

/****************************************************************************/
/*************GPS***********/
/****************************************************************************/
 /****************************************************************************
 * Function  :   track_drv_gps_switch
 
 * Description : GPS ����
 
 * Parameters
 *  status: [in] 
 *     0=�ر�
 *     1=���ȿ��������������˾�������
 *     2=������
 *
 * Return
 *  0=��/1=����Ϊ������/2=����Ϊ������
 *  error:  -1=���β�������ǰ��ͬ��ִ��/-2= �����������

 ***************************************************************************/
extern kal_int8 track_drv_gps_switch(U32 status);
 /****************************************************************************
 * Function  :   track_drv_gps_cold_restart
 
 * Description : GPS ������(�ȹر�GPS�����������������)
 
 * Parameters
 
 * Return
 *  none

 ***************************************************************************/
extern void track_drv_gps_cold_restart(void);
  /****************************************************************************
 * Function  :   track_drv_gps_claer_eph
 
 * Description : ���GPS ����
 
 * Parameters
 *   
 * Return
 *  none

 ***************************************************************************/
extern void track_drv_gps_claer_eph(void);

 /****************************************************************************
 * Function  :   track_drv_get_gps_
 
 * Description : ��ȡGPS �����������
 
 * Parameters
 *  gps_zda: [IN] memcpy ��������
 
 * Return
 *  none

 ***************************************************************************/
extern gps_gpgga_struct * track_drv_get_gps_GGA(void);
extern gps_gpgsa_struct * track_drv_get_gps_GSA(void);
extern gps_gpgsv_struct * track_drv_get_gps_GSV(void);
extern gps_gprmc_struct* track_drv_get_gps_RMC(void);
extern gps_gpvtg_struct * track_drv_get_gps_VTG(void);
extern track_gps_data_struct * track_drv_get_gps_data(void);

/****************************************************************************/
/*********** Sensor**********/
/****************************************************************************/
/******************************************************************************
 *  Function    -  track_drv_sensor_on
 * 
 *  Description     -  ����sensor ���
 * 
 *  Parameters   level  [in] : sensor ��������ȵȼ� 
 *                       FuncPtr  [in] : sensor �����ص�����
 * 
 * Return
 *  
 ******************************************************************************/
extern kal_int8 track_drv_sensor_on(kal_uint8 level , FuncPtr FuncPtr);
/******************************************************************************
 *  Function    -  track_drv_sensor_off
 * 
 *  Description     -  �ر�sensor ���
 * 
 *  Parameters   no
 * 
 * Return
 *  
 ******************************************************************************/
extern kal_int8 track_drv_sensor_off(void);
/******************************************************************************
 *  Function    -  track_drv_sensor_threshold_start
 * 
 *  Description     -  ����sensor ���
 * 
 *  Parameters   threshold  [in] : sensor ��������� 
 *                       FuncPtr  [in] : sensor �����ص�����
 * 
 * Return
 *  none
 ******************************************************************************/
extern void track_drv_sensor_threshold_start(kal_uint8 threshold , FuncPtr callback);
/******************************************************************************
 *  Function    -  track_drv_get_sensor_type
 * 
 *  Description     -  ��ȡsensor ����
 * 
 *  Parameters   no
 * 
 * Return
 *  sensor_type_enum
 ******************************************************************************/
extern sensor_type_enum track_drv_get_sensor_type(void);
/****************************************************************************/
/*********** Interrupt ***** **/
/****************************************************************************/
//ע������


/****************************************************************************/
/******* Charge batter ********/
/****************************************************************************/
extern void track_drv_bmt_charging_result(char *bmt);

 /****************************************************************************
 * Function  :   track_drv_get_batter_voltage
 
 * Description :��ȡ��ص�ѹ
 
 * Parameters 
 *   
 * Return
 *  ����ֵ��Ҫ/1000000 ������ʵ���
 *
 ***************************************************************************/
extern kal_int32 track_drv_get_batter_voltage(void);

 /****************************************************************************
 * Function  :   track_drv_get_external_batter
 
 * Description :��ȡ�ⲿ����ƿ�ĵ�ѹֵ
 
 * Parameters   flag [in]   
 *
 *  0 = ��ȡ������ADCԭʼֵ
 *  1 = ��ȡ��ӵ�ص�ѹ
 *   
 * Return
 *  ����ֵ��Ҫ/1000000 ������ʵ���
 *
 ***************************************************************************/
extern kal_int32 track_drv_get_external_batter(kal_uint8 flag);
 /****************************************************************************
 * Function  :   track_drv_sleep_enable
 
 * Description : ��ֹ/�����ж�����
 
 * Parameters   
 *                     mod   [in]  ���Ƶ�ģ��
                        enable [in]  TRUE   =  ��������
 *                                        FALSE = ��ֹ����
 * Return
 *    
 *      -1=��������
 *      -2=�Ѿ���������
 *      -3=�Ѿ���ֹ����
 *      1=��������
 *      0=�˳�����
 *
 ***************************************************************************/
extern kal_int8 track_drv_sleep_enable(sleep_mod_enum mod ,BOOL enable);

/******************************************************************************
 *  Function    -  track_drv_check_sleep_status
 * 
 *  Purpose     -  ��ѯ��������״̬
 * 
 * Return -  8bite 
 *                        bite λΪ0��ʾ��ģ���ֹ���ߣ���һλΪ0����ʾû������
 *                        return 0xFF ��ʾ����ȫ����������
 * ----------------------------------------
 ******************************************************************************/
kal_uint8 track_drv_check_sleep_status(void);
/****************************************************************************/
  /****************************************************************************
 * Function  :   track_drv_led_status_lock
 
 * Description : LED ָʾ�ƿ���
 
 * Parameters
 *       status [in]   1     =  ��LED ״̬������������Ч/  0 = ����
 *                       0xFF  =  ��ѯ��ǰ��״̬
 * 
 * Return
 *  ��ǰ��״̬
 *  -1 = ��������
 *  
 ***************************************************************************/
extern kal_int8 track_drv_led_status_lock(U8 status);
/****************************************************************************/
/*************LBS***********/
/****************************************************************************/
 /****************************************************************************
 * Function  :   track_drv_lbs_info_switch
 
 * Description : LBS ����
 
 * Parameters
 *  status: [in] 0=��/1=��
 
 * Return
 *  0=��/1=��
 * -1=���β�������ǰ��ͬ��ִ��/-2= �����������

 ***************************************************************************/
extern kal_int8 track_drv_lbs_info_switch(U8 status);
 /****************************************************************************
 * Function  :   track_drv_get_lbs_data
 
 * Description : ��ȡLBS  �����������
 
 * Parameters
 *  
 
 * Return
 *  &Last_Multi_Cell_Info

 ***************************************************************************/
extern LBS_Multi_Cell_Data_Struct * track_drv_get_lbs_data(void);
/****************************************************************************
 * Function  :   track_drv_convert_rxlev_to_rssi
 
 * Description : ��վ�źŵȼ���ת��Ϊ�ź�ǿ��
 
 * Parameters
 * rxlev :[in] ��վ�źŵȼ� 
 
 * Return
 *  �ź�ǿ��

 ***************************************************************************/
extern kal_int16 track_drv_convert_rxlev_to_rssi(kal_int16 rxlev);
/****************************************************************************
 * Function  :   track_drv_get_service_cell_level
 
 * Description : ��ȡ����С���źŵȼ�
 
 * Parameters
 * no
 
 * Return
 *  ����վ�źŵȼ�( GSM �ź�ǿ�Ȳο�ֵ)
 
 *  ������Χ0-31
 *  0xFF ��ʾ��Чֵ

 ***************************************************************************/
extern kal_uint8 track_drv_get_service_cell_level(void);

/****************************************************************************/
/**********Watch dog*********/
/****************************************************************************/
/******************************************************************************
 *  Function    -  track_drv_watdog_init
 * 
 *  Description     -  ��ʼ�����Ź�
 * 
 *  Parameters   no
 * 
 * Return
 *  none
 ******************************************************************************/
extern void track_drv_watdog_init(void);


/****************************************************************************/
/********** GPIO ********/
/****************************************************************************/
 /****************************************************************************
 * Function  :   track_drv_gpio_cut_oil_elec
 
 * Description : �ж�/�ָ��͵�
 
 * Parameters   state [in]  1   =  �ж��͵�
 *                                     0   =  �ָ��͵�
 *   
 * Return
 *   0/1   =  ִ�гɹ�
 *    -1    =  GPIO ����ʧ��
 *
 *  IO �ڳ�ʼ�����������
 ***************************************************************************/
extern kal_int8 track_drv_gpio_cut_oil_elec(kal_int8 state);
/****************************************************************************/
/********** time ********/
/****************************************************************************/
/******************************************************************************
 *  Function    -  track_drv_utc_timezone_to_rtc
 * 
 *  Description    ʱ��У׼
 * 
 *  Parameters   
 *  in_dt [in]  ʱ������
 *  timezone [in] ʱ��
 * 
 * Return
 *  out_dt [out] У׼���ʱ������

*��ע
*  track_drv_utc_timezone_to_rtc -��GTM0 У׼����ǰʱ��
*  track_drv_rtc_to_gmt0 - ������ʱ��������ת��GMT0
 ******************************************************************************/
extern void track_drv_utc_timezone_to_rtc(applib_time_struct *out_dt, applib_time_struct *in_dt, nvram_gps_time_zone_struct timezone);

extern void track_drv_rtc_to_gmt0(applib_time_struct *out_dt, applib_time_struct *in_dt, nvram_gps_time_zone_struct timezone);

/******************************************************************************
 *  Function    -  track_drv_update_time_to_system
 * 
 *  Description    ��ʱ���������ص��ն� 
 * 
 *  Parameters   
 *  utcTime [in]  ʱ������
 * 
 * Return
 *  none
 ******************************************************************************/
extern void track_drv_update_time_to_system(applib_time_struct utcTime);


/****************************************************************************/
/********** system param ********/
/****************************************************************************/
/******************************************************************************
 *  Function    -  track_drv_get_imei/track_drv_get_imsi
 * 
 *  Description    ��ȡ�ն�IMEI /IMSI
 * 
 *  Parameters   
 *  flag [in]         0     = ��ȡASCII ���ַ�
 *                ���� = ��ȡʮ������
 * Return
 *  kal_uint8 *
 * 
 *  ʾ����ע
 *   IMEI ="353419030099621"
                03 53 41 90 30 09 96 21 
*    IMSI="460021075219613"
                04 60 02 10 75 21 96 13 
                
 * 0=�ַ�����������16 �������飬�������λ��0
 * ���忨ʱIMSI  ȫ0
 ******************************************************************************/
extern kal_uint8* track_drv_get_imei(kal_uint8 flag);
extern kal_uint8* track_drv_get_imsi(kal_uint8 flag);
/******************************************************************************
 *  Function    -  track_drv_get_chip_id
 * 
 *  Description   ��ȡ����оƬ���ID
 * 
 *  Parameters   ��ֵ���岻��
 * 
 * Return
 *  kal_uint8* 
 *
 * chip_rid �����ֵ������Ψһ�ģ��ظ��ĸ���Ϊ1/2 ��128 �η�
 ******************************************************************************/
extern kal_uint8* track_drv_get_chip_id(void);

/******************************************************************************
 *  Function    -  track_drv_get_sim_type
 * 
 *  Purpose     -  ��ѯSIM ��״̬/����
 * 
 *  Parameters   
 *  no
 * 
 * Return
 *  0=�޿�/1=����ͨSIM ��/ 2=�в��Կ�/-1=δ��ʼ��/-2=�쳣/
 * 
 ******************************************************************************/
extern kal_int8 track_drv_get_sim_type(void);

/******************************************************************************
 *  Function    -  Track_nvram_read/Track_nvram_write
 * 
 *  Purpose     -  NVRAM ��д����
 * 
 *  Parameters   
 * 
 * Return
 * 
 ******************************************************************************/
extern kal_bool Track_nvram_read(nvram_lid_enum LID, kal_uint16 rec_index, void *buffer, kal_uint32 buffer_size);
extern kal_bool Track_nvram_write(nvram_lid_enum LID, kal_uint16 rec_index, void *buffer, kal_uint32 buffer_size);

extern kal_bool track_drv_epo_aid_interface();
extern void track_cust_secset_interface();
extern void track_Drv_gps_glp_Set(void* op);
extern void track_drv_set_gps_glp_mode(U8 arg);
extern void track_cust_aiding_reftime (applib_time_struct *currTime);
#endif
