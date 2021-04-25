#ifndef _TRACK_CUST_PAKET_BB_H
#define _TRACK_CUST_PAKET_BB_H
#if defined (__XYBB__)

/****************************************************************************
*  Include Files                        ����ͷ�ļ�
*****************************************************************************/
#include "track_drv_gps_decode.h"
#include "track_drv_lbs_data.h"

/*****************************************************************************
*  Define                               �궨��
******************************************************************************/
/*****************************************************************************
*  Typedef  Enum                        ö�ٶ���
*****************************************************************************/
/*****************************************************************************
*  Struct                               ���ݽṹ����
******************************************************************************/
typedef struct
{
    kal_uint16 cmd;					//���յ���Э���
    kal_uint8 buf[600];				//��Ϣ������
    kal_uint16 buf_len;				//��Ϣ�峤��
    kal_uint16 rev_serialnumber;	//�յ�����ˮ��
} nvram_bb_7edata_struct;

/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/

/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/
extern void track_cust_decode_7EData(kal_uint8 *buffer, kal_uint16 len);
extern void packet_login_bb(void * par);
extern void track_cust_paket_0002(void);
extern void track_cust_paket_0200(track_gps_data_struct *gd, LBS_Multi_Cell_Data_Struct *lbs);
extern void track_cust_paket_0201(track_gps_data_struct *gd, LBS_Multi_Cell_Data_Struct *lbs, kal_uint16 no);
extern void packet_login_bb(void * par);
extern void track_cust_decode_7EData(kal_uint8 *buffer, kal_uint16 len);
extern void track_cust_bb_set_soc_close_status(void);
extern void heartbeat_packets_bb(void);
extern void track_cust_paket_0104(kal_uint16 no);
extern void track_cust_paket_0001(nvram_bb_7edata_struct *data,kal_uint16 result);
#endif /* __XYBB__ */
#endif  /* _TRACK_CUST_PAKET_BB_H */

