/******************************************************************************
 * track_drv_system_interface.h -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        ��������ϵͳ���̽ӿ�
            ����������ϵͳ����
 *
 * modification history
 * --------------------
 * v1.0   2013-7-6,  wangqi create this file
 *
 ******************************************************************************/

#ifndef _TRACK_DRV_SYSTEM_INTERFACK_H
#define _TRACK_DRV_SYSTEM_INTERFACK_H

/*****************************************************************************
 *  Define					�궨��
 *****************************************************************************/


/****************************************************************************
* Include Files                         ����ͷ�ļ�
*****************************************************************************/
#include "track_drv.h"
#include "nvram_editor_data_item.h"

/*****************************************************************************
 *  Struct                              ���ݽṹ����
 *****************************************************************************/
typedef struct
{
    LOCAL_PARA_HDR
    kal_uint16 flag;
} fake_cell_struct;


/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/

extern void track_drv_sys_power_off_req(void);

extern int32 track_drv_sys_mtk_gps_set_param (kal_uint16 key, const void* value);

extern int32 track_drv_sys_mtk_gps_get_param(kal_uint16 key, void* value);

extern void track_drv_sys_audio_set_volume(U8 type, U8 volume);

extern kal_uint8 track_drv_sys_aud_get_volume_level(kal_uint8 mode, kal_uint8 type);

extern kal_uint8 track_drv_sys_aud_get_active_audio_mode(void);

extern void track_drv_sys_aud_send_play_id_req(void *id_param);


extern void track_drv_sys_mtk_gps_sys_storage_clear(void);


#endif /* _TRACK_DRV_SYSTEM_INTERFACK_H */
