/******************************************************************************
 * track_drv_system_param..h - 
 * 
 * Copyright .
 * 
 * DESCRIPTION: - 
 *        系统参数截取或更新 
 * 
 * modification history
 * --------------------
 * v1.0   2012-08-20,  chengjun create this file
 * 
 ******************************************************************************/
#ifndef _TRACK_DRV_SYSTEM_PARAM__H
#define _TRACK_DRV_SYSTEM_PARAM__H

/****************************************************************************
*  Include Files                        包含头文件
*****************************************************************************/
#include "track_os_data_types.h"
#include "kal_public_defs.h"
#include "stack_msgs.h"
#include "stack_ltlcom.h"
#include "rtc_sw_new.h"
#include "app_datetime.h"
#include "custom_nvram_editor_data_item.h"

/*****************************************************************************
*  Define                               宏定义
******************************************************************************/

/*****************************************************************************
*  Typedef  Enum                        枚举定义
*****************************************************************************/

/*****************************************************************************
*  Struct                               数据结构定义
******************************************************************************/

/*****************************************************************************
*  Local variable                       局部变量
*****************************************************************************/

/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/

/*****************************************************************************
*  Local Functions                      本地函数
******************************************************************************/

/*****************************************************************************
*  Global Functions                     全局函数
******************************************************************************/
kal_bool Track_nvram_read(nvram_lid_enum LID, kal_uint16 rec_index, void *buffer, kal_uint32 buffer_size);
kal_bool Track_nvram_write(nvram_lid_enum LID, kal_uint16 rec_index, void *buffer, kal_uint32 buffer_size);

extern void track_drv_get_system_param(void);
extern char* track_drv_get_mcc_from_imsi(void);

extern kal_uint8* track_drv_get_imei(kal_uint8 flag);
extern kal_uint8* track_drv_get_imsi(kal_uint8 flag);

extern kal_bool track_drv_update_disk1(kal_uint8 type, kal_uint8 *data, kal_uint32 data_index,kal_uint32 len,kal_uint8 *md5);
#endif  /* _TRACK_DRV_SYSTEM_PARAM__H */
