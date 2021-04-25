/******************************************************************************
 * track_lbs_data.h - 
 * 
 * Copyright .
 * 
 * DESCRIPTION: - 
 *        ��ȡLBS ��Ϣ 
 * 
 * modification history
 * --------------------
 * v1.0   2012-07-27,  chengjun create this file
 * 
 ******************************************************************************/

#ifndef _TRACK_DRV_LBS_DATA_H
#define _TRACK_DRV_LBS_DATA_H


/*****************************************************************************
 *  Define					�궨��
 *****************************************************************************/
//#define  __LBS_DATA_DEBUG__


/****************************************************************************
* Include Files                         ����ͷ�ļ�
*****************************************************************************/
#include "stack_msgs.h"
#include "stdlib.h"
#include "kal_public_defs.h"
#include "track_os_data_types.h"
#include "as2l4c_struct.h"

/*****************************************************************************
 *  Struct                              ���ݽṹ����
 *****************************************************************************/
typedef struct
{
    kal_uint16 mcc;
    kal_uint16 mnc;
    kal_uint16 lac;
    kal_uint16 cell_id;
    kal_int16 rxlev;   /*  rxlev  : ͨ��catcher ��Ϣ�鿴�����ֵӦ���Ǳ��������źŵ�ƽ��
                                    ���źŵȼ�ֵ�������������ź�ǿ��ֵ */
                              /*track_drv_convert_rxlev_to_rssi  -�źŵȼ���ת��Ϊ�ź�ǿ�� -- chengjun  2012-03-08*/
} Multi_Cell_Info_Struct;

typedef struct
{
    BOOL status;    //  TRUE = ��Ч
    Multi_Cell_Info_Struct      MainCell_Info;
    Multi_Cell_Info_Struct      NbrCell_Info[6];
    kal_uint8 valid_cel_num;
    kal_uint8 ta;
} LBS_Multi_Cell_Data_Struct;

/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/
extern void track_drv_get_network_info(void);

#endif  /* _TRACK_DRV_LBS_DATA_H */

