#ifndef _TRACK_SOC_LCON_H
#define _TRACK_SOC_LCON_H

/****************************************************************************
*  Include Files                        ����ͷ�ļ�
*****************************************************************************/
#include "soc_api.h"
#include "track_soc.h"

/*****************************************************************************
*  Define                               �궨��
******************************************************************************/
#define MAX_SND_BUFFER_SIZE 1500

/*****************************************************************************
*  Typedef  Enum                        ö�ٶ���
*****************************************************************************/
typedef enum
{
    SOC_SEND_DATA_ERROR             = -2,           // �������ݴ���
    SOC_CONNECT_ERROR               = -1,           // ���Ӵ���
    SOC_IDLE                        = 0,            // ����
    SOC_BEGIN_CONNECT               = 1,            // ��ʼ����
    SOC_CONNECT_SUCCESS             = 2,            // ���ӳɹ�
    SOC_CONNECT_ServerNotResponding = 3,            //�������޷�Ӧ
    SOC_SEND_DATA                   = 4,
    SOC_RECV_DATA                   = 5,            // ���յ��µ�����
    SOC_RECV_DATA_MayNotBeComplete  = 6,            // ���յ��µ����ݣ����ڻ������������Ƚ�����δ��������������ݷ����ټ�������
    SOC_CONNECT_CLOSE               = 7,            // �ѽ��������ӱ��ر�
    SOC_RECV_HOST_BY_NAME           
} track_soc_status_enum;

typedef enum
{
    SOC_CONN_DISCONNECT,            // ���ӶϿ�
    SOC_CONN_FAILURE,               // ����ʧ��
    SOC_CONN_SUCCESSFUL,            // ���ӳɹ�
    SOC_CONN_CONNECTING,            // ��������
} track_soc_connstatus_enum;

typedef enum
{
    PACKET_ALARM_DATA               = 1,               // 1
    PACKET_NVRAM_BACKUP_DATA,            // 2
    PACKET_POSITION_DATA,            // 3
    PACKET_HEARTBEAT_DATA,            // 4
    PACKET_NET_LOG_DATA,             // 5
    PACKET_NET_CMD_DATA,              // 6
    PACKET_LBS_DATA,              // 7

    PACKET_MAX_DATA			 //
} track_packet_enum;

/*****************************************************************************
*  Struct                               ���ݽṹ����
******************************************************************************/
typedef void (*track_soc_func_ptr)(void *);

typedef void (*Func_Ptr)(void *);

typedef struct
{
    kal_uint8          ref_count;
    kal_uint16         msg_len;
    kal_uint32         status;
} mmi_test_info_req_struct;

#if defined(__MQTT__)
typedef struct
{
    track_soc_status_enum status;       // �ص�����
    kal_uint8 *recvBuff;                // ��������ָ��
    kal_int32 recvBuffLen;              // �������ݵĳ���
    kal_uint8 *databak;                 // ���͵����ݱ���
    kal_uint8 queue_id;                 // ���б��
    kal_uint8 packet_type;              // ���ݰ�����
    kal_uint32 no;                      // ���кţ��������е�һ�£������Ǹ���ǣ�����������
    soc_dns_a_struct ip_entry[SOC_MAX_A_ENTRY];         // �����������󷵻ص�IP��ַ
    kal_uint8        num_entry;
    kal_int8 errorid;                   // �������
    kal_uint8 batchCount;               // 
} track_soc_Callback_struct;
#else
typedef struct
{
    track_soc_status_enum status;       // �ص�����
    kal_uint8 *recvBuff;                // ��������ָ��
    kal_int32 recvBuffLen;              // �������ݵĳ���
    kal_uint8 *databak;                 // ���͵����ݱ���
    kal_uint8 queue_id;                 // ���б��
    kal_uint8 packet_type;              // ���ݰ�����
    kal_uint32 no;                      // ���кţ��������е�һ�£������Ǹ���ǣ�����������
    kal_uint8 ip_from_hostname[4];      // �����������󷵻ص�IP��ַ
    kal_int8 errorid;                   // �������
    kal_uint8 batchCount;               // 
} track_soc_Callback_struct;
#endif/*mqtt*/

/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/

/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/

/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/

/******************************************************************************
 *  Function    -  
 *
 *  Purpose     -  �ص�����
 *
 *  static void event(track_soc_Callback_struct *par)
 *  {
 *      
 *  }
 *
 *  Description -  ����         ����
 *                 ip_addr      ��Ҫ���ӵ�Ŀ���ַ�Ͷ˿ڵĽṹ��
 *                 func         �¼�����ص�����
 *                 no           ��ţ��ص������з��أ������Ǹ�������ã�Ĭ��Ϊ0���ɣ�
 *
 *  return         0            ����
 *                 -1           �ص�����δ����
 *                 -2           ��Ч��IP��ַ��˿�
 *                 -3           ��һ����������δ�����������ظ�����
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 06-08-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/

/******************************************************************************
 *  Function    -  track_soc_conn_req
 *
 *  Purpose     -  Socket����������
 *
 *  Description -  ����         ����
 *                 ip_addr      ��Ҫ���ӵ�Ŀ���ַ�Ͷ˿ڵĽṹ��
 *                 func         �¼�����ص�����
 *                 no           ��ţ��ص������з��أ������Ǹ�������ã�Ĭ��Ϊ0���ɣ�
 *
 *  return         0            ����
 *                 -1           �ص�����δ����
 *                 -2           ��Ч��IP��ַ��˿�
 *                 -3           ��һ����������δ�����������ظ�����
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 06-08-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
extern kal_int8 track_soc_conn_req(sockaddr_struct *ip_addr, Func_Ptr func, kal_uint32 no);

/******************************************************************************
 *  Function    -  track_soc_write_req
 *
 *  Purpose     -  Socket�������ݺ���
 *
 *  Description -  ����         ����
 *                 data         ��Ҫ���͵�����ָ���׵�ַ
 *                 datalen      ���͵������ֽ���
 *                 packet_type  ���ݰ�����
 *                 no           ��ţ��ص������з��أ������Ǹ�������ã�Ĭ��Ϊ0���ɣ�
 *
 *  return         0            ����
 *                 -1           �ص�����δ���ã�����δ���������ܷ�������
 *                 -2           ���͵����ݳ��ȴ���4096�ֽ�
 *                 -3           ��һ���������ݵ�����δ�����������ظ�����
 *                 -4           ���͵�����ָ��Ϊ��
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 06-08-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
extern kal_int8 track_soc_write_req(kal_uint8 *data, kal_int32 datalen, kal_uint8 packet_type, kal_uint32 no);

/******************************************************************************
 *  Function    -  track_soc_gethostbyname
 *
 *  Purpose     -  Socket��������������
 *
 *  Description -  ����         ����
 *                 hostname     ��Ҫ�����������ַ���
 *                 func         �¼�����ص�����
 *                 no           ��ţ��ص������з��أ������Ǹ�������ã�Ĭ��Ϊ0���ɣ�
 *
 *  return         0            ����
 *                 -1           �ص�����δ����
 *                 -2           ����Ϊ�գ��������ַ������ȳ�����100���ַ�
 *                 -3           ��һ��������������δ�����������ظ�����
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 06-08-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
extern kal_int8 track_soc_gethostbyname(char *hostname, Func_Ptr func);


extern kal_int8 track_soc_callback_register(Func_Ptr func);
#endif  /* _TRACK_SOC_LCON_H */

