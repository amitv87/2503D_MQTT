/******************************************************************************
 * track_drv_key.h -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        keypad �Զ���
 *
 * modification history
 * --------------------
 * v1.0   2012-12-29,  chengjun create this file
 *
 ******************************************************************************/

#if defined (__KEYPAD_MOD__)

#ifndef _TRACK_DRV_KEY_H
#define _TRACK_DRV_KEY_H


/*****************************************************************************
 *  Define					�궨��
 *****************************************************************************/
#define KEYPAD_CUSTOM_MAP_MAX 5
//��Χ1 ~0xFE

/****************************************************************************
* Include Files                         ����ͷ�ļ�
*****************************************************************************/
#include "stack_msgs.h"
#include "stdlib.h"
#include "kal_public_defs.h"
#include "track_os_data_types.h"
#include "keypad_sw.h"
#include "stack_ltlcom.h"
#include "GlobalConstants.h"
#include "kbd_table.h"

/*****************************************************************************
 *  Struct                              ���ݽṹ����
 *****************************************************************************/

typedef struct
{
    kal_uint8 keyCode;
    kbd_event keyType;
} keypad_handle_struct;

typedef struct
{
    LOCAL_PARA_HDR
    keypad_handle_struct key;
} keypad_msg_struct;

typedef struct
{
    U8 device_key_code;     /* Device key code, see kbd_table.h */
    U8 mmi_key_code;        /* mmi key code, see GlobalConstants.h mmi_keypads_enum */
    kal_bool longpress;
} keypad_code_map_struct;


/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/
void track_drv_push_key_to_buff(kbd_event event, kal_uint8 key);
void track_drv_send_keypad_event(void);
void track_drv_keypad_event_handle(void *msg);

#endif  /* _TRACK_DRV_KEY_H */

#endif /* __KEYPAD_MOD__ */
