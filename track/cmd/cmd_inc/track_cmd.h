/******************************************************************************
 * track_cmd.h -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        ָ�����
 *
 * modification history
 * --------------------
 * v1.0   2012-07-21,  WangQi create this file
 *
 ******************************************************************************/

#ifndef _TRACK_COMMANDS_H
#define _TRACK_COMMANDS_H

/****************************************************************************
* Include Files                   		����ͷ�ļ�
*****************************************************************************/
#include "kal_public_defs.h"
#include "track_os_data_types.h"
#include "custom_nvram_editor_data_item.h"
#include "track_os_ell.h"
#include "track_os_paramete.h"

/*****************************************************************************
 *  Define					�궨��
*****************************************************************************/
#if defined (__GT740__)||defined(__GT420D__)
#define CM_PARAM_LONG_LEN_MAX  420
#else
#define CM_PARAM_LONG_LEN_MAX  320
#endif
#define CM_PARAM_LEN_MAX 100
#define CM_HEAD_PWD_MXA 32

#define GPS_COMMA_UNIT_MAX 16

/*****************************************************************************
* Typedef  Enum
*****************************************************************************/

typedef enum
{
    CM_AT,              //AT��Դ��ָ��
    CM_SMS,             //������Դ��ָ��
    CM_ONLINE,          //������Դ��ָ��
    CM_ONLINE2,         //������Դ��ָ��
    CM_PARAM_CMD,
    CM_TEST_ONLINE,
    CM_VS_AT,
    CM_VS_EX,
    CMD_NET2,
    CMD_MAX_TYPE
} Cmd_Type;

typedef enum
{
    CM_Main,
#if defined(__GT02B__)
    CM_PWD,
#endif
    CM_Par1,
    CM_Par2,
    CM_Par3,
    CM_Par4,
    CM_Par5,
    CM_Par6,
    CM_Par7,
    CM_Par8,
    CMD_MAX_Pars
} Cmd_Pars;



/*****************************************************************************
 *  Struct					���ݽṹ����
*****************************************************************************/

typedef struct
{
    LOCAL_PARA_HDR 
    kal_uint8           cmd_data[CM_PARAM_LEN_MAX];
    kal_uint16          cmd_length;
    kal_uint16          target;
} cmd_req_struct;

typedef enum
{
    Error_110,
    CMD_Cmd_Ret_Code
} Cmd_Ret_Code;

typedef struct
{
    kal_uint8           reply_character_encode; // 0 ASCII   1 UTF-16BE   2 GB2312
    kal_uint8           num_type;               // ��������//145���� 160 ����//129 10086 ����
    kal_uint8           stamp[4];               // ���λ����ǰ���������·���ָ����Ҫ����������ṩ�����к�
    char                ph_num[CM_HEAD_PWD_MXA];// ��Դ  ���⿨��Ҫ�ڵ绰����ǰ����+�ţ��������ֶ����޷���������
    Cmd_Type            cm_type;
#if defined(__DASOUCHE__)    
    char                requestId[100];        //���ѳ���Զ��ָ������ID
#endif        
} cmd_return_struct;

typedef struct
{
    U8                  part;//��������
    char                rcv_msg_source[CM_PARAM_LONG_LEN_MAX];
    char                rcv_msg[CM_PARAM_LONG_LEN_MAX];
    U16                 rcv_length;
    char                *pars[GPS_COMMA_UNIT_MAX];
    Cmd_Ret_Code        rsp_code;
    char                rsp_msg[CM_PARAM_LONG_LEN_MAX];
    U16                 rsp_length;
    kal_uint8           supercmd;               // Ϊ1 �ı�ʾ��ǰ��ָ����������Ȩ��ָ�������������ָ��
    kal_uint8           cmd_pwd_type;           // 1 ��ǰָ��ƥ���û����룻2 ��ǰָ��ƥ�䳬�����룻3 ��ǰָ�����û�����ͳ������붼ƥ��
    kal_uint8           character_encode;       // 0 ASCII   1 UTF-16BE   2 GB2312
    kal_uint8           flag;                   // ������Ŀʹ�õ�������
    cmd_return_struct   return_sms;
} CMD_DATA_STRUCT;

typedef struct
{
    Cmd_Type            cm_type;
    char                ph_num[CM_HEAD_PWD_MXA];//��Դ
    kal_uint8           character_encode;       // 0 ASCII   GSM_UCS2 UTF-16BE
} CMD_DATA_STRUCT2;

typedef struct
{
    U16                 sms_index;
    U16                 len;  //����Ϣ����
    char                content[320];
    char                num[21];
} GPS_SMS_STRUCT;

typedef void (*FuncPtrEx) (CMD_DATA_STRUCT *);

typedef struct
{
    const S8            *cmd_string;
    FuncPtrEx           func_ptr;
    BOOL                valid;
} CMD_ENGINE_STRUCT;


/****************************************************************************
* Global Variable           ���õ�ȫ�ֱ���
*****************************************************************************/

/****************************************************************************
* Global Variable - Extern          ����ȫ�ֺ���
*****************************************************************************/
extern kal_int8 track_command_operate(CMD_DATA_STRUCT * command);
extern BOOL track_cmd_check_authentication(CMD_DATA_STRUCT * _cmd);
extern void track_cmd_send_rsp(Cmd_Type cm_type,kal_int16 len,char * rsp_msg,kal_uint8 sign);
extern void track_cmd_sms_rsp(CMD_DATA_STRUCT * cmds);
extern void bosch_motion_sensor_NvramData_write(U16 time);//��λ����������
#endif  /* _TRACK_COMMANDS_H */
