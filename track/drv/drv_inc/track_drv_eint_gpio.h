/******************************************************************************
 * track_interrupt.h -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        �жϿ�/GPIO  ����
 *
 * modification history
 * --------------------
 * v1.0   2012-07-26,  chengjun create this file
 *
 ******************************************************************************/

#ifndef _TRACK_DRV_EINT_GPIO_H
#define _TRACK_DRV_EINT_GPIO_H

/****************************************************************************
* Include Files                   		����ͷ�ļ�
*****************************************************************************/
#include "Eint.h"
#include "intrCtrl.h"
#include "kal_public_defs.h"
#include "track_os_data_types.h"

/****************************************************************************
* Global Variable - Extern          ����ȫ�ֱ���
*****************************************************************************/
typedef enum
{
    SIM_STATE_ABSENT = 0,
    SIM_STATE_READY,
    SIM_STATE_UNKNOWN,
    SIM_STATE_NETWORK_LOCKED,
    SIM_STATE_PIN_REQUIRED,
    SIM_STATE_PUK_REQUIRED,
    SIM_STATE_MAX
} enum_SIM_STATE;

/*****************************************************************************
 *  Define					�궨��
 *****************************************************************************/

/*-----   �жϿ�����start----*/

#if defined(__GT740__) /* 61оƬ */
/*1��Drv_Tool ���õ��ж��ڸ�ģ���޷����ã�������¶���*/
/* ���⹦����Sensor ���ж�������ģ�鴦������ֻ��ͨ�ýӿ�*/

#define ACC_EINT_NO  0xFF
#define EXTRA_A_KEY_EINT_NO  0xFF
#define EXTRA_B_KEY_EINT_NO  0xFF

/*2��EINT_A_NO ������ʾ�����жϿڣ�ֻ��������ģ��ע�������ֵ
    ����ø��ݶ����ֵ�ж�*/
//nc-EINT_CHANNEL_NOT_EXIST

#define EINT_A_NO  ACC_EINT_NO
#define EINT_B_NO  EXTRA_A_KEY_EINT_NO
#define EINT_C_NO  EXTRA_B_KEY_EINT_NO

/*3���ж϶�Ӧ��IO ��*/
//nc-0xFF
#define EINT_A_GPIO 0xFF
#define EINT_B_GPIO 0xFF
#define EINT_C_GPIO 0xFF


/*4��IO�ڵ��ж�ģʽֵ�в���*/
//nc-0
#define EINT_A_GPIO_MODE    0
#define EINT_A_EINT_MODE    0
#define EINT_B_GPIO_MODE    0
#define EINT_B_EINT_MODE    4
#define EINT_C_GPIO_MODE    0
#define EINT_C_EINT_MODE    0

#elif defined(__GT420D__)
/*1��Drv_Tool ���õ��ж��ڸ�ģ���޷����ã�������¶���*/
/* ���⹦����Sensor ���ж�������ģ�鴦������ֻ��ͨ�ýӿ�*/

#define ACC_EINT_NO  14	//����
#define EXTRA_A_KEY_EINT_NO  0xFF
#define EXTRA_B_KEY_EINT_NO  0xFF

/*2��EINT_A_NO ������ʾ�����жϿڣ�ֻ��������ģ��ע�������ֵ
    ����ø��ݶ����ֵ�ж�*/
//nc-EINT_CHANNEL_NOT_EXIST

#define EINT_A_NO  ACC_EINT_NO
#define EINT_B_NO  EXTRA_A_KEY_EINT_NO
#define EINT_C_NO  EXTRA_B_KEY_EINT_NO

/*3���ж϶�Ӧ��IO ��*/
//nc-0xFF
#define EINT_A_GPIO 20
#define EINT_B_GPIO 0xFF
#define EINT_C_GPIO 0xFF


/*4��IO�ڵ��ж�ģʽֵ�в���*/
//nc-0
#define EINT_A_GPIO_MODE    0
#define EINT_A_EINT_MODE    2
#define EINT_B_GPIO_MODE    0
#define EINT_B_EINT_MODE    0
#define EINT_C_GPIO_MODE    0
#define EINT_C_EINT_MODE    0


#elif defined(__GT03F__) || defined(__GT300__) || defined(__BD300__) || defined(__GT500__) || defined(__GW100__)|| defined(__GT300S__)
/*1��Drv_Tool ���õ��ж��ڸ�ģ���޷����ã�������¶���*/
/* ���⹦����Sensor ���ж�������ģ�鴦������ֻ��ͨ�ýӿ�*/

#if defined( __GT03F__) || defined(__GW100__)
#define ACC_EINT_NO  EINT_CHANNEL_NOT_EXIST
#elif defined __GT300__
#define ACC_EINT_NO  3
#else
#define ACC_EINT_NO  1
#endif /* __GT03F__ */

#if defined( __KEY_EINT__)
#define EXTRA_A_KEY_EINT_NO  3
#else
#define EXTRA_A_KEY_EINT_NO  EINT_CHANNEL_NOT_EXIST
#endif

#if defined( __BD300__)
#define EXTRA_B_KEY_EINT_NO  31
#else
#define EXTRA_B_KEY_EINT_NO  0xFF
#endif /* __BD300__ */

/*2��EINT_A_NO ������ʾ�����жϿڣ�ֻ��������ģ��ע�������ֵ
    ����ø��ݶ����ֵ�ж�*/
//nc-EINT_CHANNEL_NOT_EXIST


#define EINT_A_NO  ACC_EINT_NO
#define EINT_B_NO  EXTRA_A_KEY_EINT_NO
#define EINT_C_NO  EXTRA_B_KEY_EINT_NO



/*3���ж϶�Ӧ��IO ��*/
//nc-0xFF
#if defined __GT300__
#define EINT_A_GPIO 4
#else
#define EINT_A_GPIO 5
#endif

#define EINT_B_GPIO 4
#define EINT_C_GPIO 76


/*4��IO�ڵ��ж�ģʽֵ�в���*/
//nc-0
#define EINT_A_GPIO_MODE    0
#define EINT_A_EINT_MODE    3
#define EINT_B_GPIO_MODE    0
#define EINT_B_EINT_MODE    1
#define EINT_C_GPIO_MODE    0
#define EINT_C_EINT_MODE    3


#elif defined(__ET350__)
#define ACC_EINT_NO 16

#define EINT_A_NO  ACC_EINT_NO
#define EINT_B_NO  EINT_CHANNEL_NOT_EXIST
#define EINT_C_NO  EINT_CHANNEL_NOT_EXIST

/*3���ж϶�Ӧ��IO ��*/
//nc-0xFF
#define EINT_A_GPIO 30
#define EINT_B_GPIO 0xFF
#define EINT_C_GPIO 0xFF


/*4��IO�ڵ��ж�ģʽֵ�в���*/
//nc-0
#define EINT_A_GPIO_MODE    0
#define EINT_A_EINT_MODE    4
#define EINT_B_GPIO_MODE    0
#define EINT_B_EINT_MODE    2
#define EINT_C_GPIO_MODE    0
#define EINT_C_EINT_MODE    0
#elif defined(__ET310__) && defined(__UART1_SOS__)
#define ACC_EINT_NO  16
#define EXTRA_A_KEY_EINT_NO  9
#define EXTRA_B_KEY_EINT_NO  0xFF

#define EINT_A_NO  ACC_EINT_NO
#define EINT_B_NO  9
#define EINT_C_NO  EINT_CHANNEL_NOT_EXIST

/*3���ж϶�Ӧ��IO ��*/
//nc-0xFF
#define EINT_A_GPIO 30
#define EINT_B_GPIO 10//2503����1 RX,�жϺŶ�ӦΪ9
#define EINT_C_GPIO 0xFF


/*4��IO�ڵ��ж�ģʽֵ�в���*/
//nc-0
#define EINT_A_GPIO_MODE    0
#define EINT_A_EINT_MODE    4
#define EINT_B_GPIO_MODE    0
#define EINT_B_EINT_MODE    3
#define EINT_C_GPIO_MODE    0
#define EINT_C_EINT_MODE    0
#elif defined(__MT200__) ||defined(__ET310__) ||defined(__ET320__)
#define ACC_EINT_NO  16
#define EINT_A_NO  ACC_EINT_NO
#define EINT_B_NO  EINT_CHANNEL_NOT_EXIST
#define EINT_C_NO  EINT_CHANNEL_NOT_EXIST

/*3���ж϶�Ӧ��IO ��*/
//nc-0xFF
#define EINT_A_GPIO 30
#define EINT_B_GPIO 0xFF
#define EINT_C_GPIO 0xFF


/*4��IO�ڵ��ж�ģʽֵ�в���*/
//nc-0
#define EINT_A_GPIO_MODE    0
#define EINT_A_EINT_MODE    4
#define EINT_B_GPIO_MODE    0
#define EINT_B_EINT_MODE    0
#define EINT_C_GPIO_MODE    0
#define EINT_C_EINT_MODE    0

/*5�����DRV_TOOL ���ϱ�Ķ�Ӧ��ϵ����ͬƽ̨���ڲ���*/
#elif defined(__GT800__) ||defined(__NT31__)||defined(__NT36__) || defined(__NT37__)|| defined(__NT22__)
/*2��EINT_A_NO ������ʾ�����жϿڣ�ֻ��������ģ��ע�������ֵ
    ����ø��ݶ����ֵ�ж�*/
//nc-EINT_CHANNEL_NOT_EXIST
#define ACC_EINT_NO  16
#define EXTRA_B_KEY_EINT_NO 0xFF
#if  defined(__NT31__)&&! defined(__NT32__)
#define EXTRA_A_KEY_EINT_NO 0xFF//sos 
#define EXTRA_A_EINT_NO  0xFE //�ⲿ����
#else
#define EXTRA_A_KEY_EINT_NO 0 //sos
#define EXTRA_A_EINT_NO  1 //�ⲿ����
#endif /* __NT31__ */

#define EINT_A_NO  ACC_EINT_NO
#define EINT_B_NO  EXTRA_A_KEY_EINT_NO
#define EINT_C_NO  EXTRA_B_KEY_EINT_NO
#define EINT_D_NO  EXTRA_A_EINT_NO


/*3���ж϶�Ӧ��IO ��*/
//nc-0xFF
#define EINT_A_GPIO 30
#define EINT_B_GPIO 0
#define EINT_C_GPIO 0xff
#define EINT_D_GPIO 1

/*4��IO�ڵ��ж�ģʽֵ�в���*/
//nc-0
#define EINT_A_GPIO_MODE    0
#define EINT_A_EINT_MODE    4
#define EINT_B_GPIO_MODE    0
#define EINT_B_EINT_MODE    1
#define EINT_C_GPIO_MODE    0
#define EINT_C_EINT_MODE    2
#define EINT_D_GPIO_MODE    0
#define EINT_D_EINT_MODE    1

#elif defined(__NT33__)
#if defined (__FUN_WITH_ACC__)
/*1��Drv_Tool ���õ��ж��ڸ�ģ���޷����ã�������¶���*/
/* ���⹦����Sensor ���ж�������ģ�鴦������ֻ��ͨ�ýӿ�*/

#define ACC_EINT_NO  16
#define EXTRA_A_KEY_EINT_NO  0xFF
#define EXTRA_B_KEY_EINT_NO  0xFF
#define EXTRA_A_EINT_NO 12
/*2��EINT_A_NO ������ʾ�����жϿڣ�ֻ��������ģ��ע�������ֵ
  ����ø��ݶ����ֵ�ж�*/
//nc-EINT_CHANNEL_NOT_EXIST

#define EINT_A_NO  ACC_EINT_NO
#define EINT_B_NO  EINT_CHANNEL_NOT_EXIST
#define EINT_C_NO  EXTRA_A_EINT_NO       //BT����GSM


/*3���ж϶�Ӧ��IO ��*/
//nc-0xFF
#define EINT_A_GPIO 30
#define EINT_B_GPIO 0xFF
#define EINT_C_GPIO 14


/*4��IO�ڵ��ж�ģʽֵ�в���*/
//nc-0
#define EINT_A_GPIO_MODE    0
#define EINT_A_EINT_MODE    4
#define EINT_B_GPIO_MODE    0
#define EINT_B_EINT_MODE    3
#define EINT_C_GPIO_MODE    0
#define EINT_C_EINT_MODE    2

#else
//NT33��ACC�汾
/*1��Drv_Tool ���õ��ж��ڸ�ģ���޷����ã�������¶���*/
/* ���⹦����Sensor ���ж�������ģ�鴦������ֻ��ͨ�ýӿ�*/

#define ACC_EINT_NO  0xFF
#define EXTRA_A_KEY_EINT_NO  0xFF
#define EXTRA_B_KEY_EINT_NO  0xFF
#define EXTRA_A_EINT_NO 12
/*2��EINT_A_NO ������ʾ�����жϿڣ�ֻ��������ģ��ע�������ֵ
    ����ø��ݶ����ֵ�ж�*/
//nc-EINT_CHANNEL_NOT_EXIST

#define EINT_A_NO  EINT_CHANNEL_NOT_EXIST
#define EINT_B_NO  EINT_CHANNEL_NOT_EXIST
#define EINT_C_NO  EXTRA_A_EINT_NO       //BT����GSM


/*3���ж϶�Ӧ��IO ��*/
//nc-0xFF
#define EINT_A_GPIO 0xFF
#define EINT_B_GPIO 0xFF
#define EINT_C_GPIO 14


/*4��IO�ڵ��ж�ģʽֵ�в���*/
//nc-0
#define EINT_A_GPIO_MODE    0
#define EINT_A_EINT_MODE    3
#define EINT_B_GPIO_MODE    0
#define EINT_B_EINT_MODE    3
#define EINT_C_GPIO_MODE    0
#define EINT_C_EINT_MODE    2
#endif
#elif defined(__GT310__)
/*1��Drv_Tool ���õ��ж��ڸ�ģ���޷����ã�������¶���*/
/* ���⹦����Sensor ���ж�������ģ�鴦������ֻ��ͨ�ýӿ�*/

#define ACC_EINT_NO  0xFF
#define EXTRA_A_KEY_EINT_NO  0  //SOS
#define EXTRA_B_KEY_EINT_NO  0xFD
/*2��EINT_A_NO ������ʾ�����жϿڣ�ֻ��������ģ��ע�������ֵ
    ����ø��ݶ����ֵ�ж�*/
//nc-EINT_CHANNEL_NOT_EXIST

#define EINT_A_NO  EINT_CHANNEL_NOT_EXIST
#define EINT_B_NO  EXTRA_A_KEY_EINT_NO
#define EINT_C_NO  EINT_CHANNEL_NOT_EXIST
//EXTRA_A_EINT_NO

/*3���ж϶�Ӧ��IO ��*/
//nc-0xFF
#define EINT_A_GPIO 0xFF
#define EINT_B_GPIO 0
#define EINT_C_GPIO 0xFF
//30


/*4��IO�ڵ��ж�ģʽֵ�в���*/
//nc-0
#define EINT_A_GPIO_MODE    0
#define EINT_A_EINT_MODE    0
#define EINT_B_GPIO_MODE    0
#define EINT_B_EINT_MODE    1
#define EINT_C_GPIO_MODE    0
#define EINT_C_EINT_MODE    0

#elif defined(__GT370__) || defined (__GT380__)
/*1��Drv_Tool ���õ��ж��ڸ�ģ���޷����ã�������¶���*/
/* ���⹦����Sensor ���ж�������ģ�鴦������ֻ��ͨ�ýӿ�*/

#define ACC_EINT_NO  0xFF
#define EXTRA_A_KEY_EINT_NO  0xFF
#define EXTRA_B_KEY_EINT_NO  0xFF
#define EXTRA_A_EINT_NO 16
/*2��EINT_A_NO ������ʾ�����жϿڣ�ֻ��������ģ��ע�������ֵ
    ����ø��ݶ����ֵ�ж�*/
//nc-EINT_CHANNEL_NOT_EXIST

#define EINT_A_NO  EINT_CHANNEL_NOT_EXIST
#define EINT_B_NO  EINT_CHANNEL_NOT_EXIST
#define EINT_C_NO  EINT_CHANNEL_NOT_EXIST
//EXTRA_A_EINT_NO

/*3���ж϶�Ӧ��IO ��*/
//nc-0xFF
#define EINT_A_GPIO 0xFF
#define EINT_B_GPIO 0xFF
#define EINT_C_GPIO 0xFF
//30


/*4��IO�ڵ��ж�ģʽֵ�в���*/
//nc-0
#define EINT_A_GPIO_MODE    0
#define EINT_A_EINT_MODE    3
#define EINT_B_GPIO_MODE    0
#define EINT_B_EINT_MODE    3
#define EINT_C_GPIO_MODE    0
#define EINT_C_EINT_MODE    3

#elif defined (__JM81__)||defined (__JM66__)
//��Ŀ���ж�
/*1��Drv_Tool ���õ��ж��ڸ�ģ���޷����ã�������¶���*/
/* ���⹦����Sensor ���ж�������ģ�鴦������ֻ��ͨ�ýӿ�*/

#define ACC_EINT_NO  0xFF
#define EXTRA_A_KEY_EINT_NO  0xFF
#define EXTRA_B_KEY_EINT_NO  0xFF
#define EXTRA_A_EINT_NO 0xFF
/*2��EINT_A_NO ������ʾ�����жϿڣ�ֻ��������ģ��ע�������ֵ
    ����ø��ݶ����ֵ�ж�*/
//nc-EINT_CHANNEL_NOT_EXIST

#define EINT_A_NO  EINT_CHANNEL_NOT_EXIST
#define EINT_B_NO  EINT_CHANNEL_NOT_EXIST
#define EINT_C_NO  EINT_CHANNEL_NOT_EXIST
//EXTRA_A_EINT_NO

/*3���ж϶�Ӧ��IO ��*/
//nc-0xFF
#define EINT_A_GPIO 0xFF
#define EINT_B_GPIO 0xFF
#define EINT_C_GPIO 0xFF
//30


/*4��IO�ڵ��ж�ģʽֵ�в���*/
//nc-0
#define EINT_A_GPIO_MODE    0
#define EINT_A_EINT_MODE    3
#define EINT_B_GPIO_MODE    0
#define EINT_B_EINT_MODE    3
#define EINT_C_GPIO_MODE    0
#define EINT_C_EINT_MODE    3

#elif defined(__HVT001__)
//��Ŀ���ж�
/*1��Drv_Tool ���õ��ж��ڸ�ģ���޷����ã�������¶���*/
/* ���⹦����Sensor ���ж�������ģ�鴦������ֻ��ͨ�ýӿ�*/

#define ACC_EINT_NO  0xFF
#define EXTRA_A_KEY_EINT_NO  0xFF
#define EXTRA_B_KEY_EINT_NO  0xFF
#define EXTRA_A_EINT_NO 20
/*2��EINT_A_NO ������ʾ�����жϿڣ�ֻ��������ģ��ע�������ֵ
    ����ø��ݶ����ֵ�ж�*/
//nc-EINT_CHANNEL_NOT_EXIST

#define EINT_A_NO  EXTRA_A_EINT_NO
#define EINT_B_NO  EINT_CHANNEL_NOT_EXIST
#define EINT_C_NO  EINT_CHANNEL_NOT_EXIST
//EXTRA_A_EINT_NO

/*3���ж϶�Ӧ��IO ��*/
//nc-0xFF
#define EINT_A_GPIO 46
#define EINT_B_GPIO 0xFF
#define EINT_C_GPIO 0xFF
//30


/*4��IO�ڵ��ж�ģʽֵ�в���*/
//nc-0
#define EINT_A_GPIO_MODE    0
#define EINT_A_EINT_MODE    2
#define EINT_B_GPIO_MODE    0
#define EINT_B_EINT_MODE    3
#define EINT_C_GPIO_MODE    0
#define EINT_C_EINT_MODE    3

#else
#error "please define project_null"
#endif

/*6��Ĭ��Ϊ��ƽ����ģʽ*/
/*����: �޸�ΪEDGE_SENSITIVE ģʽ������ϸ�鿴���룬��Ҫ�޸�����*/
#define EINT_SENSITIVE_MODE  LEVEL_SENSITIVE


/*7��Ĭ���������200ms  */
//unit 10ms
#if defined( __GT03F__) || defined(__GW100__) || defined(__GT530__)||defined (__GT370__)|| defined (__GT380__)
#define EINT_SW_DEBOUNCE_TIME  2
#elif defined (__GT370__)|| defined (__GT380__)
#define EINT_SW_DEBOUNCE_TIME  1
#elif defined(__GT420D__)
#define EINT_SW_DEBOUNCE_TIME  2
#else
#define EINT_SW_DEBOUNCE_TIME  20
#define EINT_SW_DEBOUNCE_20MS  2
#endif /* __GT03F__ */

#if defined __GT300S__
#define EINT_SPI_NO 2  //�жϺ�
#define EINT_SPI_GPIO 2 //��ӦGPIO��
#endif

#if 0
#define EINT_EDGE_NO        0xFF
#define EINT_EDGE_GPIO      0xFF
#endif
/*-----   �жϿ�����end ----*/

#if 1
//����ACC�Ͷ��͵�
#endif

/*-----  GPIO ����start ----*/
//�͵���ƶ˿�
#if defined (__V20__) || defined (__GT02F__)
#define CUT_OIL_ELE_GPIO  21
#elif defined(__NT31__) || defined(__MT200__)||defined(__ET310__) ||defined(__ET320__) ||defined(__NT36__)  ||defined(__NT37__) || defined(__NT22__)
#define CUT_OIL_ELE_GPIO  29
#elif defined(__GT02E__) || defined(__BD300__) || defined(__GT500__) || defined(__ET130__) || defined(__NT23__) || defined(__GT06D__)
#define CUT_OIL_ELE_GPIO  12
#elif defined (__ET100__)||defined (__GT02D__)
#define CUT_OIL_ELE_GPIO  22
#elif  defined(__GT03F__) || defined(__GT800__) || defined(__GT300S__) || defined(__GT530__)|| defined(__GT740__)||defined(__GT420D__) || defined (__NT33__)|| defined(__GT370__)|| defined (__GT380__) ||defined(__ET350__)
#define CUT_OIL_ELE_GPIO  0xff
#else
#define CUT_OIL_ELE_GPIO  0xff
#endif

//5V���
#if defined (__GT02F__)
#define CUST_OUT1_GPIO      20
#define CTRL_5V_OUT_GPIO    19
#elif defined(__NT31__) ||defined(__NT36__) || defined(__NT22__)
#define CUST_OUT1_GPIO      0xFF
#define CTRL_5V_OUT_GPIO    3
#elif defined(__NT37__)
#define CUST_OUT1_GPIO      27
#define CUST_OUT2_GPIO      25
#define CTRL_5V_OUT_GPIO    3
#else
#define CUST_OUT1_GPIO      0xFF
#define CTRL_5V_OUT_GPIO    0xFF
#endif


#if defined(__NT37__)
#define CUST_OUT2_GPIO      25
#else
#define CUST_OUT2_GPIO      0xFF
#endif

#if defined(__NT23__)
#define OBD_POWER_GPIO  30
#elif defined(__GT530__)
#define OBD_POWER_GPIO  5
#elif defined(__GT500__)
#define OBD_POWER_GPIO  61
#else
#define OBD_POWER_GPIO  30//Ϊ�˱���ͨ��xzq 
#endif

#if defined(__EXT_VBAT_ADC__)
#if defined (__ET130__)
#define EXT_POWER_GPIO    7
#elif defined(__GT02F__) || defined (__GT380__)
#define EXT_POWER_GPIO    0xFF
#elif defined (__V20__)
#define EXT_POWER_GPIO    6
#elif defined (__ET200__)
#define EXT_POWER_GPIO    8
#elif defined (__ET310__) ||defined(__ET320__) || defined(__MT200__)
#define EXT_POWER_GPIO    1
#else
#define EXT_POWER_GPIO    28
#endif /* __ET130__ */
#endif /* __EXT_VBAT_ADC__ */

#if defined( __433M__)
#define CTRL_433_GPIO        33
#define AP_433_GPIO             36
#else
#define CTRL_433_GPIO        0xff
#define AP_433_GPIO             0xff
#endif /* __433M__ */

#if defined(__GT06F__) //|| defined (__GT800__)
#define AUX_EINT_NO  23
#define AUX_EINT_GPIO_NO  52
#else
#define AUX_EINT_NO  0xfc
#define AUX_EINT_GPIO_NO  0xFF
#endif
/*-----  GPIO ����end----*/

#ifndef  EINT_D_NO
#define EINT_D_NO 0xfe
#endif

/*****************************************************************************
* Typedef  Enum
*****************************************************************************/


/*****************************************************************************
 *  Struct					���ݽṹ����
 *****************************************************************************/
typedef struct
{
    LOCAL_PARA_HDR
    U8 eint;
    U8 level;
} eint_status_struct;

/*****************************************************************************
* Local variable				�ֲ�����
*****************************************************************************/


/*****************************************************************************
* Golbal variable				ȫ�ֱ���
*****************************************************************************/




/*****************************************************************************
 *  Global Functions	- Extern		�����ⲿ����
 *****************************************************************************/


/*****************************************************************************
 *  Local Functions			���غ���
 *****************************************************************************/
extern void track_eint_B_registration(void);


/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/
extern void track_drv_eint_init_registration(void);
extern kal_int8 track_drv_gpio_set(kal_int8 state, kal_int8 port);

extern void track_drv_lte_pwron(kal_uint8 s);
extern void track_drv_lte_wake(void);
extern void track_drv_lte_uart1_ctrl1(const char * fn, kal_uint8 s);

#define track_drv_lte_uart1_ctrl(s)     track_drv_lte_uart1_ctrl1(__func__, s);

extern void track_drv_set_vcama(kal_bool ON,/* kal_uint32 ldo,*/ kal_uint32 ldo_volt);
extern kal_uint32 track_drv_lte_init_wdt(kal_uint8 op);

#endif  /* _TRACK_DRV_EINT_GPIO_H */
