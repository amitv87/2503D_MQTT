#ifndef _TRACK_CUST_PAKET_H
#define _TRACK_CUST_PAKET_H

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
typedef enum
{
    TR_CUST_POSITION_TIMING                 =    0,    // ��ʱ�ϱ�
    TR_CUST_POSITION_DISTANCE               =    1,    // �����ϱ�
    TR_CUST_POSITION_ANGLEREP               =    2,    // �յ��ϴ�
    TR_CUST_POSITION_ACC_CHANGE             =    3,    // ACC ״̬�ı��ϴ�
    TR_CUST_POSITION_STATIC                 =    4,    // ���˶���Ϊ��ֹ״̬�󣬲������һ����λ��
    TR_CUST_POSITION_RECONNECT              =    5,    // ����Ͽ��������ϱ�֮ǰ���һ����Ч�ϴ���
    TR_CUST_POSITION_MODE_GPSUPDATE         =    6,    // ��������ǿ���ϴ�GPS��
    TR_CUST_POSITION_KEY                    =    7,    // �����ϴ���λ��
    TR_CUST_POSITION_POWER_ON               =    8,    // �����ϱ�λ����Ϣ
    TR_CUST_POSITION_LBS                    =    9,    // 
    TR_CUST_POSITION_STATIC_LAST_TIME       = 0x0A,    // �豸��ֹ���ϱ����ľ�γ�ȣ���ʱ�����
    TR_CUST_POSITION_WIFI_TO_COORDINATES    = 0x0B,    // WIFI������γ���ϴ�
    TR_CUST_POSITION_LJDW                   = 0x0C,    // LJDW��������λ��ָ���ϴ�
    TR_CUST_POSITION_GPSOFF                 = 0x0D,    // ��̬�ϴ�    
    TR_CUST_POSITION_GPSDUP                 = 0x0E,    // GPSDUP�ϴ�    
    TR_CUST_POSITION_MAX
} enum_CUST_POSITION_type;

/*****************************************************************************
*  Struct                               ���ݽṹ����
******************************************************************************/

/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/

/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/
extern kal_uint16 track_cust_paket_gpsdata(track_gps_data_struct *gd, LBS_Multi_Cell_Data_Struct *lbs);
extern void track_cust_paket_1C(kal_uint8 *serverid, kal_bool isASCII, kal_uint8 *outstr, kal_uint8 outstrlen);
extern void Server_GPRS_Command(kal_uint8 *serverid, kal_bool isASCII, kal_uint8 *outstr, kal_uint8 outstrlen);
extern kal_uint16 track_cust_paket_1B(track_gps_data_struct *gd, char *number);
extern void Send_IP_Request_data(LBS_Multi_Cell_Data_Struct *lbs_data);
extern void Analysis_ServerData(U8 *data, int len);
#endif  /* _TRACK_CUST_PAKET_H */

