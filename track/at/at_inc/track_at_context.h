/******************************************************************************
 * track_context.h -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        
 *
 * modification history
 * --------------------
 * v1.0   2013-01-11,  WangQi create this file
 *
 ******************************************************************************/
#ifndef TR_CONTEXT_H
#define TR_CONTEXT_H

#include "Device.h"

#include "track_os_data_types.h"

/*****************************************************************************
 *  Define			    宏定义
*****************************************************************************/


/************************************************************************

************************************************************************/


/*****************************************************************************
* Typedef  Enum         枚举
*****************************************************************************/


/*****************************************************************************
 *  Struct			    数据结构定义
*****************************************************************************/

typedef struct
{
    kal_bool            GPS_Pass_state ;
    kal_bool            PhoneTestMode;
    kal_bool            nvram_init_flag;
    kal_bool            sms_ready;
    kal_bool            PhoneAutoTestMode;
    kal_uint8           audio_mode;
	kal_bool            notquit;
} gps_runtime_evn_struct;

/****************************************************************************
* Global Variable           引用的全局变量
*****************************************************************************/

/****************************************************************************
* Global Variable - Extern          引用全局函数
*****************************************************************************/
extern kal_bool track_status_gpstc(kal_uint8 flag);

extern kal_bool track_status_test(kal_uint8 flag);

extern kal_uint8 track_status_audio_mode(kal_uint8 mode);

extern int track_cust_get_rssi(int arg);
#endif /*TR_CONTEXT_H*/