/******************************************************************************
 * track_drv_uart.h -
 *
 * Copyright .
 *
 * DESCRIPTION: -

 * modification history
 * --------------------
 * v1.0   2013-04-17,  wangqi create this file
 *
 ******************************************************************************/
 
#ifndef _TRACK_DRV_UART_H
#define _TRACK_DRV_UART_H

/****************************************************************************
* Include Files             ����ͷ�ļ�
*****************************************************************************/
#include "Device.h"

#include "track_os_data_types.h"

#include "stack_ltlcom.h"

/*******************************************************************************
 * Type Definitions
 *******************************************************************************/

/*
 * general definitions
 */

extern kal_uint16 track_uart_GetBytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, kal_uint8 *status, module_type ownerid);
extern void track_drv_uart_ready_read(void* msg);
extern void track_drv_uart_init(void);
extern void track_drv_uart2_init(void);
extern void track_drv_mgps_on();
extern void track_drv_mgps_off();
extern void track_uart_case(int m);
extern void track_uart_decode(char* buf, kal_uint16 len);
extern void track_cust_ubxcold(void);
extern kal_uint8 track_get_gps_ota(kal_uint8 flag);
extern void track_cust_gps_OTA_start(void);
extern void track_cust_gps_OTA_stop(void);
extern void track_drv_write_exmode(kal_uint8 *data, int len);
extern kal_uint16 track_drv_encode_exmode(kal_uint8 head, kal_uint16 packet, kal_uint8 pid, kal_uint8 *data, kal_uint16 len);
#endif  /* _TRACK_DRV_UART_H */

