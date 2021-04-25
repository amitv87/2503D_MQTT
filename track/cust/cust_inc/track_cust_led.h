/******************************************************************************
 * track_led.h - 
 * 
 * Copyright .
 * 
 * DESCRIPTION: - 
 *        LED  ��ʾ 
 * 
 * modification history
 * --------------------
 * v1.0   2012-07-22,  chengjun create this file
 * 
 ******************************************************************************/

#ifndef _TRACK_CUST_LED_H
#define _TRACK_CUST_LED_H

/****************************************************************************
* Include Files                   		����ͷ�ļ�
*****************************************************************************/
#include "track_drv_led.h"
#include "common_nvram_editor_data_item.h"


/*****************************************************************************
 *  Define					�궨��
 *****************************************************************************/

//#define __LED_DEBUG__


#define LED_FIRST_DISPLAY_TIME	( 5*60*1000 )      //�ڲ�Ʒ������������״̬��LED �Ƴ�����ʾ��ʱ��
#define LED_FIRST_PWRON_TIME 20*1000                 /*�����Ƶ�һ���ϵ糣��20S��*/
#define LED_ALWAYS_DISPLAY_TIME  60*1000           /*First time power on ,led display 5 minutes,other display 20S*/
#define LED_KEY_DISPLAY_TIME 5*1000

/*LED ״̬�Ƶ���ʾ�ر�ʱ��       --    MagicNing  2010-05-24*/
#define LED_OFF_DELAY_TIME  80

/***************************************************************************** 
* Typedef  Enum
*****************************************************************************/
typedef enum
{
    GPS_INVALID_STATE,       // GPS�ر�
    GPS_PWON_STATE,            // �տ���
    GPS_SCAN_STATE,             // ��������
    GPS_LOCATION_STATE     // �Ѷ�λ
} GPS_WORK_STATE;

typedef enum
{
    PW_OFF_STATE,
    PW_NORMAL_STATE,
    PW_TOO_LOW_STATE,
    PW_LOW_ALARM_STATE,
    PW_ON_STATE,
    PW_CHARGING_STATE,
    //PW_SHUTDOWN_STATE,
    PW_FULLCHARGED_STATE
} GPS_BATTERY_STATE; //���״̬

typedef enum
{
    GSM_OFF_STATE,
    GSM_NORMAL_STATE,
    GSM_COMING_CALL_STATE,
    GSM_IN_CALL_STATE,
    GSM_INITIAL_STATE,
    GSM_NO_SIGNAL_STATE,
    GPRS_CONNECT_STATE,
    //GSM_SHUTDOWN_STATE
} GSM_SIGNAL_STATE; //GSM

typedef enum 
{
	TWO_GSM_IN_CALL_STATE=0,
    TWO_PW_CHARGING_STATE,
	TWO_PW_FULLCHARGED_STATE,
	TWO_PW_TOO_LOW_STATE,
	TWO_GSM_OFF_STATE,//δ��SIM��
    TWO_GSM_SCAN_STATE,
	TWO_GPS_UNLOCATION_STATE,	   // δ��λ
	TWO_GPRS_DISCONNECT_STATE,
	TWO_GPRS_CONNECT_STATE,
	TWO_PW_ON_STATE,
	TWO_COLOR_MAX
}TWO_COLOR_STATE;

typedef enum 
{
	TLEVEL1,
	TLEVEL2,
	TLEVEL3,
	TLEVEL4,
		TLEVEL5,
	TLEVEL_MAX
}TWO_COLOR_LEVEL_STATE;


/*****************************************************************************
 *  Struct					���ݽṹ����
 *****************************************************************************/


/*****************************************************************************
 *  Global Functions			ȫ�ֱ���
 *****************************************************************************/

/*****************************************************************************
 *  Local Functions			���غ���
 *****************************************************************************/


/*****************************************************************************
 *  Global Functions			ȫ�ֺ���
 *****************************************************************************/
extern void LED_GPS_StatusSet(GPS_WORK_STATE status);
extern void LED_GSM_StatusSet(GSM_SIGNAL_STATE status);
extern void LED_PWR_StatusSet(GPS_BATTERY_STATE status);
extern kal_uint8 track_cust_led_sleep(kal_int8 par);
#endif  /* _TRACK_CUST_LED_H */
