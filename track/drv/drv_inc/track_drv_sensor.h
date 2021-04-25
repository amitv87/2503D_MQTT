/******************************************************************************
 * track_drv_sensor.h -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        Sensor����
 *
 * modification history
 * --------------------
 * v1.0   2012-12-01,  chengjun create this file
 *
 ******************************************************************************/
#ifndef _TRACK_DRV_SENSOR_H
#define _TRACK_DRV_SENSOR_H

#if defined (__TRACK_SENSOR__)

/****************************************************************************
* Include Files                   		����ͷ�ļ�
*****************************************************************************/
#include "track_os_data_types.h"
#include "kal_general_types.h"

#include "track_sensor_bma250.h"
#include "track_sensor_kxtj31057.h"
#include "track_sensor_sc7a20.h"
#include "track_sensor_da213.h"
#if !defined (__REMOVE_OBSOLETE_GSENSOR__)
#include "track_sensor_mc3410.h"
#include "track_sensor_afa750.h"
#include "track_sensor_mma8452q.h"
#endif /* __REMOVE_OBSOLETE_GSENSOR__ */

#include "nvram_data_items.h"
#include "nvram_editor_data_item.h"

/****************************************************************************
* Global Variable - Extern          ����ȫ�ֱ���
*****************************************************************************/


/*****************************************************************************
 *  Define					�궨��
 *****************************************************************************/
#if defined(__GT800__)||defined(__NT31__)||defined(__MT200__)||defined(__GT310__) ||defined(__ET310__) ||defined(__ET320__) ||defined(__NT36__) || defined (__NT33__) || defined(__GT370__)|| defined (__GT380__)|| defined (__GT740__)||defined(__ET350__) || defined(__NT37__)|| defined(__NT22__) || defined(__GT420D__)
#define MOTION_SENSOR_EINT_NO 2
#elif defined(__GT530__) || defined(__V20__) || defined(__GT02F__) || defined(__GT03F__)
#define MOTION_SENSOR_EINT_NO    16   
#elif defined (__GT02E__) || defined(__BD300__) || defined(__GT500__) || defined(__ET130__) || defined(__NT23__) || defined(__GT06D__) || defined(__GW100__)
#define MOTION_SENSOR_EINT_NO    0
#elif defined(__GT300__)
#define MOTION_SENSOR_EINT_NO    1
#elif defined(__ET100__)||defined (__GT02D__)
#define MOTION_SENSOR_EINT_NO    3
#elif defined (__GT300S__)||defined (__HVT001__)
#define MOTION_SENSOR_EINT_NO    23 
#elif defined (__JM81__)||defined (__JM66__)
//gSensor IIC connect to MCU
#define MOTION_SENSOR_EINT_NO    0xFF 

#else
#error "please define project_null"
#endif

#define SENSOR_BMA250_SW_DEBOUNCE_TIME 80    //unit  10ms  Bma250/Bma250E ��ʽ����
#define SENSOR_SW_DEBOUNCE_TIME        80         //unit  10ms 


/*****************************************************************************
* Typedef  Enum
*****************************************************************************/
typedef enum
{
    SENSOR_LEVEL_MIN,
    SENSOR_LEVEL_1,      /*������*/
    SENSOR_LEVEL_2,
    SENSOR_LEVEL_3,
    SENSOR_LEVEL_4,
    SENSOR_LEVEL_5,            /*�����*/
    SENSOR_LEVEL_MAX
} sensor_threshold_level;


#define     SENSOR_LEVEL_DEFAULT        SENSOR_LEVEL_4

typedef enum
{
    SENSOR_INIT_ERROR=-1,        //δ��ʶ��Sensor ����
    SENSOR_NOT_READY,           //��δ���ʶ��Sensor ����,�������Ϊ0 
    SENSOR_BMA250,
    SENSOR_BMA250E,
    SENSOR_BMA253,
    SENSOR_KXTJ31057,
    SENSOR_SC7A20,
    SENSOR_DA213,
#if 0
    SENSOR_MC3410,
    SENSOR_AFA750,
    SENSOR_MMA8452Q,
#endif /* 0 */
    SENSOR_MECH,
    /*ֻ�����ڴ˴�������sensor  �ͺ�     --    chengjun  2013-02-26*/

    /*----------------------------*/
    SENSOR_TYPE_MAX,
} sensor_type_enum;


/*****************************************************************************
 *  Struct					���ݽṹ����
 *****************************************************************************/
typedef struct
{
    sensor_type_enum sensor_type;
    kal_uint8 sensor_level;
    kal_uint8 sensor_threshold;
    FuncPtr sensor_callback;
    void (*sensor_on)(kal_uint8 level , FuncPtr callback);
    FuncPtr sensor_off;
} track_sensor_config_data_struct;


typedef struct
{
    LOCAL_PARA_HDR
    kal_uint8 type;
} track_driving_record_msg_struct;


/*****************************************************************************
* Local variable				�ֲ�����
*****************************************************************************/


/*****************************************************************************
* Golbal variable				ȫ�ֱ���
*****************************************************************************/
extern track_sensor_config_data_struct  track_sensor_config_data;



/*****************************************************************************
 *  Global Functions	- Extern		�����ⲿ����
 *****************************************************************************/


/*****************************************************************************
 *  Local Functions			���غ���
 *****************************************************************************/

/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/
void track_drv_sensor_type_auto_matching(void);
kal_uint8 track_drv_get_sensor_level(void);
sensor_type_enum track_drv_get_sensor_type(void);
void track_drv_sensor_callback_handle(void*msg);
kal_int8 track_drv_sensor_on(kal_uint8 level , FuncPtr callback);
kal_int8 track_drv_sensor_off(void);


#endif /* __TRACK_SENSOR__ */

#endif  /* _TRACK_DRV_SENSOR_H */
