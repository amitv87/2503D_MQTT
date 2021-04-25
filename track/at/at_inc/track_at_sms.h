/******************************************************************************
 * track_at_sms.h -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *
 *
 * modification history
 * --------------------
 * v1.0   2012-09-07,  wangqi create this file
 *
 ******************************************************************************/
#ifndef TR_SMSH
#define TR_SMSH
#ifdef __AT_CA__
#include "Ps_public_struct.h"
#include "track_os_data_types.h"
/*****************************************************************************
*  Define			    �궨��
*****************************************************************************/
#define  SMS_ADDRESS_MAX_SIZE       16
#define  GROUP_SEND_MAX	            6
#define  SMS7BITMAX                 160
#define  SMSUCS2MAX                 140

#define  LONG_SMS_7BIT_MAX          152
#define  LONG_SMS_UCS2_MAX          134
// �û���Ϣ���뷽ʽ
#define GSM_7BIT                    0
#define GSM_8BIT                    4
#define GSM_UCS2                    8

#if defined( __GT530__)
#define SMS_MAX_SIZE                900     //��֧��4������
#else
#define SMS_MAX_SIZE                640     //��֧��4������
#endif /* __GT530__ */

#define SMS_SENDER_BOX_LENGTH       10      //ֻ����ǰ10���ռ���
#define SMS_RESEND_LIMIT            3
#define SMS_RESEND_INTERVAL_TIME    60000
/*****************************************************************************
* Typedef  Enum         ö��
*****************************************************************************/
typedef enum
{
    SMS_PDU_INVALID = 0,
    SMS_PDU_DELIVER,
    SMS_PDU_SUBMIT,
    SMS_PDU_STATUS_REPORT
} SmsPduType;

typedef enum
{
    sSTANDBY,
    sSEND_HEAD,
    sSEND_CTX,
    sSEND_WAIT,
    sSS_MAX
} SMS_STATUS_ENUM;

typedef enum
{
    sSENDOK,
    sCTXTOOLONG,
    sSIMERROR,
    sNUMBERERROR,
    sSENDERROR,
    sSENDHEADOVER,
    sSENDOVERTIME,
    sSENDUNKNOW,
    sSSR_MAX
} SMS_SEND_RESULT_ENUM;

typedef enum
{
    SMS_MS_MT,//ֱ�ӷ��ͶԷ�
    SMS_SENDER_BOX,//�淢����
    SMS_DIR_MAX
}SMS_DIRECTION;

/*****************************************************************************
*  Struct			    ���ݽṹ����
*****************************************************************************/
typedef struct
{
    kal_uint8           nocount;
    kal_uint8           num_type;
    char*               numbers[GROUP_SEND_MAX];//phone numbers
    kal_bool            codetype;   //isascii?
    char*               ctx;
    kal_uint16          ctxlen;//���ݳ���unicodeΪ���ֳ���*2
    kal_uint32          index;//���к�
    SMS_DIRECTION       smsdir;
} sms_msg_struct;

typedef struct
{
    kal_uint32              index;
    SMS_DIRECTION           dir;
    SMS_STATUS_ENUM         status;
    SMS_SEND_RESULT_ENUM    result;
} sms_send_rsp_struct;

// ����Ϣ�����ṹ������/���빲��
// ���У��ַ�����0��β
typedef struct
{
    char            SCA[16];       // ����Ϣ�������ĺ���(SMSC��ַ)
    kal_uint8       SCAL;          //SCA���볤��
    kal_uint8       MR;            //
    kal_uint8       P_TYPE;
    kal_uint8       ODAL;          // ����������볤��
    char            TPA[32];       // Ŀ������ظ�����(TP-DA��TP-RA)
    char            TP_PID;        // �û���ϢЭ���ʶ(TP-PID)
    char            TP_DCS;        // �û���Ϣ���뷽ʽ(TP-DCS)
    char            TP_SCTS[16];   // ����ʱ����ַ���(TP_SCTS), ����ʱ�õ�
    char            TP_UD[SMS_MAX_SIZE];    // ԭʼ�û���Ϣ(����ǰ�������TP-UD)
    kal_uint32      index;         // ����Ϣ��ţ��ڶ�ȡʱ�õ�
    kal_uint16      TP_UDL;         //user data length
    kal_uint8       TP_Max_p;       //��������һ���ж��ٰ�
    kal_uint8       TP_Seq_p;       //��ǰ����
    kal_uint16      TP_Unique;       //�ְ���ͬ��Ψһ��ʶ
    kal_uint8       TP_Rev;//һ���յ����ٸ���
} SM_PARAM;

typedef struct
{
    char            SCA[16];       // ����Ϣ�������ĺ���(SMSC��ַ)
    kal_uint8       SCAL;          // SCA���볤��
    kal_uint8       MR;            //
    kal_uint8       P_TYPE;
    kal_uint8       ODAL;          // ����������볤��
    char            TPA[32];       // Ŀ������ظ�����(TP-DA��TP-RA)
    char            TP_PID;        // �û���ϢЭ���ʶ(TP-PID)
    char            TP_DCS;        // �û���Ϣ���뷽ʽ(TP-DCS)
    char            TP_SCTS[16];   // ����ʱ����ַ���(TP_SCTS), ����ʱ�õ�
    char*           TP_UD;         // ��������
    kal_uint16      TP_UDL;        // �������ݳ���
    kal_uint32      index;         // ����Ϣ��ţ��ڶ�ȡʱ�õ�
    kal_uint8       TP_Max_p;      // ��������һ���ж��ٰ�
    kal_uint8       TP_Seq_p;      // ��ǰ����
    kal_uint16      TP_Unique;     // �ְ���ͬ��Ψһ��ʶ
    kal_uint8       TP_Rev;        // һ���յ����ٸ���
} track_sms_param;

typedef struct
{
    kal_uint8	fo;
    kal_uint8	scts[7];
    l4c_number_struct	sca_number;
    l4c_number_struct	oa_number;
    kal_uint8	pid; /* smsal_pid_enum */
    kal_uint8	dcs; /* smsal_dcs_enum */
    kal_uint8	ori_dcs;
    kal_uint16	index;
    kal_uint16	dest_port;
    kal_uint16	src_port;
    kal_uint8	mti;
    kal_uint8	display_type;
    kal_uint8	storage_type;
    smsal_concat_struct concat_info;
    kal_uint8    count;
    kal_uint16	data_len[16];
    kal_uint8	*pdata[16];
} mmi_sms_delivers_struct;


typedef struct
{
    char            sms_ctx[SMS_MAX_SIZE];
    kal_uint16      sms_len;//�������ݵ��ַ�����
    kal_uint16      sms_size;//�������ʵ����
    kal_uint16      sp_size;//AT����120 �ְ���
    kal_uint8       sms_error_conut;//���ʹ������
    kal_uint8       sms_overtime_count;//�������ݻ���ͷ����ʱ
    kal_uint32		sms_index;//Ӧ�ò㷢�����к�
    SMS_STATUS_ENUM sms_status;
    SMS_DIRECTION   sms_dir;
    kal_uint8       smscodetype;//����ʱ��Ψһ��������ڷְ�����
} sms_submit_pdu;



typedef struct
{
    char            encode_number[64];
    char            encode_ctx[1024];
    char*           readptr;
    int             inumlen;
    int             ictxlen;
    int             tpacket;
    int             curpacket;
    int             sendlen;
    int             index;
    SMS_DIRECTION   sms_dir;
    kal_uint16      ctxlimit;
    kal_uint8       smscodetype;
    kal_uint8       tp_index;//����ʱΨһʶ�����

} sms_ctx_encode_struct;

typedef void (*sms_get_senderbox_handlerPtr)(void); 
/*****************************************************************************
* Local variable            �ֲ�����
*****************************************************************************/

/****************************************************************************
* Global Variable           ȫ�ֱ���
*****************************************************************************/

/*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/

/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/

/*****************************************************************************
 *  Local Functions	            ���غ���
*****************************************************************************/


/*****************************************************************************
 *  Global Functions            ȫ�ֺ���
*****************************************************************************/
extern void track_sms_set_auto_to_serial(void);
extern void track_sms_receive(int sms_length, char * sms_context);

/******************************************************************************
 * FUNCTION:  - track_app_sms_rsp
 * DESCRIPTION: -����AT���ͽ�� 
 * Input:
 * Output:
 * modification history
 * --------------------
 * v1.0  , 26-10-2012,  wangqi  written
 * --------------------
 ******************************************************************************/
extern kal_bool track_sms_send_rsp(kal_int8 result);


/******************************************************************************
 * FUNCTION:  - track_sms_sender
 * DESCRIPTION: -֧����� GROUP_SEND_MAX ������Ⱥ������
 * Input:
    kal_uint8       nocount; // ���͸����ٸ�����
    char*           numbers[GROUP_SEND_MAX];//phone numbers
    kal_bool        codetype;//isascii? //�Ƿ�ΪASCII��?
    char*           ctx;//��������
    kal_uint16      ctxlen;//���ݳ���unicodeΪ���ֳ���*2
    kal_uint32      index;//���к�
 * Output:
 * modification history
 * --------------------
 * v1.0  , 26-10-2012,  wangqi  written
 * --------------------
 ******************************************************************************/
extern void track_sms_sender(sms_msg_struct *sms_send);
extern kal_bool track_sms_get_send_status(void);

extern void track_sim_clean_allsms(void);
extern void track_sim_clean_read_sms_reveicebox(void);
extern void track_sim_read_unreadsms_reveicebox(void);
extern void track_sim_clean_reveicebox(void);
extern void track_sim_clean_senderbox(void);
extern void track_sim_sms_set_revicer(sms_get_senderbox_handlerPtr callback);
extern kal_uint8 track_sim_get_sms_from_senderbox(void);

extern void track_sms_clean_out_vec(void);
#endif
#endif/*TR_SMSH*/
