/******************************************************************************
 * track_cust_lcd.h -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        modem ����OLEDӦ�ò�ӿ�
 *
 * modification history
 * --------------------
 * v1.0   2014-4-24,  wangqi create this file
 *
 ******************************************************************************/

#if defined (__OLED__)

#ifndef _TRACK_CUST_LCD_H
#define _TRACK_CUST_LCD_H


/*****************************************************************************
 *  Define					�궨��
 *****************************************************************************/


/****************************************************************************
* Include Files                         ����ͷ�ļ�
*****************************************************************************/
#include "stack_msgs.h"
#include "stdlib.h"
#include "kal_public_defs.h"
#include "track_os_data_types.h"
#include "stack_ltlcom.h"



/*****************************************************************************
 *  Struct                              ���ݽṹ����
 *****************************************************************************/


/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/

extern void track_cust_lcd_change_sleep(void);
extern void track_cust_lcd_updata_gsm(kal_uint8 level);
extern void track_cust_lcd_updata_charge(kal_uint8 type);
extern void track_cust_lcd_sos(void);
#endif  /* _TRACK_CUST_LCD_H */

#endif /* __OLED__ */
