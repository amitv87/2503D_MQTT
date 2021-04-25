/*******************************************************************************************
 * Filename:
 * Author :
 * Date :
 * Comment:
 ******************************************************************************************/

/****************************************************************************
*  Include Files                        包含头文件
*****************************************************************************/
#include "track_cust.h"
#include "Conversions.h"
#include <math.h>
#include "us_timer.h"
#include "uem_context.h"
#include "track_at_call.h"
#include "track_os_ell.h"
#include "track_nvram_default_value.h"
#include "track_version.h"
#include "track_os_paramete.h"
/*****************************************************************************
*  Define                               宏定义
******************************************************************************/
#define ON_OFF(value) (value==0?s_OFF:(value==1?s_ON:s_Error))


/*****************************************************************************
*  Typedef  Enum                        枚举定义
*****************************************************************************/
#if defined(__JM81__)
extern void track_cust_mlsp_shutdown_req(void);
extern void track_cust_mlsp_write_backup_req(kal_uint8* write_data,int len);
extern void track_cust_mlsp_read_backup_req(void);
extern void track_cust_mlsp_get_peripherals_status(void);
extern void track_cust_mlsp_set_nextboot_time(applib_time_struct* time);
#endif
extern void track_cust_module_update_mode(void);
/*****************************************************************************
*  Struct                               数据结构定义
******************************************************************************/
typedef struct
{
    char**          cmd;
    kal_uint8       count;
    Cmd_Type        type;
} cmd_online_struct;

typedef void(*Func_CustRecvCmdPtr)(char*, char*, Cmd_Type);

typedef struct
{
    char                    *cmd_string;
    Func_CustRecvCmdPtr     func_ptr;
} cust_recv_cmd_struct;

typedef void(*Func_CustRecvPtr)(CMD_DATA_STRUCT*);

typedef struct
{
    const char           *cmd_string;
    Func_CustRecvPtr     func_ptr;
} cust_recv_struct;


/*****************************************************************************
*  Local variable                       局部变量
*****************************************************************************/
#if defined(__GERMANY__)
static char  *s_ON = {"EIN"}, *s_OFF = {"AUS"}, *s_Error = {"Fehler"};
#else
static char  *s_ON = {"ON"}, *s_OFF = {"OFF"}, *s_Error = {"ERROR"};
#endif

/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/
kal_uint8 g_test_flag = 0;
char dail_num[TRACK_DEFINE_PHONE_NUMBER_LEN_MAX] = {0};
kal_uint32 dail_count = 0;
static kal_uint32 ft_timer = 0;
static kal_uint32  ft_work_timer = 0;
#if defined(__GT420D__)
static kal_uint8 gps_speed_change = 0;
#endif

#if defined (__GT02__)
/*单GT02使用，关起来节约空间       --    chengjun  2017-04-13*/
const nvram_importance_parameter_struct NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT_SERVER_COOACCESS =
{
    {__DATE__},
    {__TIME__},
    {"WM"},
    {_TRACK_VER_PARAMETER_},
    {KAL_FALSE, "CMNET", "", ""},
    {
        /*www.cooaccess.net*/
        {0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xF4, 0xF2, 0xE1, 0xE3, 0xEB, 0xF3, 0xEF, 0xEC, 0xE9, 0xE4, 0xAE, 0xE3, 0xEF, 0xED},
        1, {183, 60, 142, 137}, 21101, 0, 0, 0, 0
    }
    , {
        /*agps.topstargps.com:12283*/
        {0xE4 , 0xE5 , 0xF6 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE3 , 0xEC , 0xEF , 0xF5 , 0xE4 , 0xAE , 0xE3 , 0xEF , 0xED},
        1, {183, 63, 53, 246}, 12283, 0, 0, 0
    },
    {
        {

            {0XF7 , 0xF7 , 0xF7 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE1 , 0xE7 , 0xF0 , 0xF3 , 0xAE , 0xE3 , 0xEF , 0xED},
            1, {116, 204, 8, 57}, 12283, 0, 0, 0
        }
        , {
            {0xF7 , 0xF7 , 0xF7 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE1 , 0xED , 0xF3 , 0xAE , 0xE3 , 0xEF , 0xED},
            1, {116, 204, 8, 57}, 12283, 0, 0, 0
        }
        , {
            {0xE4 , 0xE5 , 0xF6 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE3 , 0xEC , 0xEF , 0xF5 , 0xE4 , 0xAE , 0xE3 , 0xEF , 0xED},
            1, {116, 204, 8, 57}, 12283, 0, 0, 0
        }
        , {
            {0xE1, 0xE7, 0xF0, 0xF3, 0xAE, 0xF4, 0xEF, 0xF0, 0xF3, 0xF4, 0xE1, 0xF2, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED},
            0, {114, 67, 59, 162}, 12283, 0, 0, 0
        }
    }
    , {
        {0xE7 , 0xF4, 0xB0 , 0xB2, 0xAE , 0xE7 , 0xEF, 0xEF, 0xED , 0xE5 , 0xE7, 0xF0, 0xF3, 0xAE, 0xEE, 0xE5 , 0xF4},
        1,
        {202, 104, 149, 80},
        9888,
        0,
        0,
        0,
        0,
    }
#if defined(__BACKUP_SERVER__)
    ,{0}
#endif/*__BACKUP_SERVER__*/

    , {3, 1, 1, 0} //外贸版本，SENSOR，支持国外版功能

};

const nvram_importance_parameter_struct NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT_SERVER_TUQIANG =
{
    {__DATE__},
    {__TIME__},
    {"TQ"},
    {_TRACK_VER_PARAMETER_},
    {KAL_FALSE, "CMNET", "", ""},
    {
        /*atr02.tuqianggps.com*/
        {0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED},
        1, {202, 70, 2, 53}, 21101, 0, 0, 0, 0
    }
    , {
        /*agps.topstargps.com:12283*/
        {0xE4 , 0xE5 , 0xF6 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE3 , 0xEC , 0xEF , 0xF5 , 0xE4 , 0xAE , 0xE3 , 0xEF , 0xED},
        1, {183, 63, 53, 246}, 12283, 0, 0, 0
    }
    ,
    {
        {

            {0XF7 , 0xF7 , 0xF7 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE1 , 0xE7 , 0xF0 , 0xF3 , 0xAE , 0xE3 , 0xEF , 0xED},
            1, {116, 204, 8, 57}, 12283, 0, 0, 0
        }
        , {
            {0xF7 , 0xF7 , 0xF7 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE1 , 0xED , 0xF3 , 0xAE , 0xE3 , 0xEF , 0xED},
            1, {116, 204, 8, 57}, 12283, 0, 0, 0
        }
        , {
            {0xE4 , 0xE5 , 0xF6 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE3 , 0xEC , 0xEF , 0xF5 , 0xE4 , 0xAE , 0xE3 , 0xEF , 0xED},
            1, {116, 204, 8, 57}, 12283, 0, 0, 0
        }
        , {
            {0xE1, 0xE7, 0xF0, 0xF3, 0xAE, 0xF4, 0xEF, 0xF0, 0xF3, 0xF4, 0xE1, 0xF2, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED},
            0, {114, 67, 59, 162}, 12283, 0, 0, 0
        }
    }
    , {
        {0xE7 , 0xF4, 0xB0 , 0xB2, 0xAE , 0xE7 , 0xEF, 0xEF, 0xED , 0xE5 , 0xE7, 0xF0, 0xF3, 0xAE, 0xEE, 0xE5 , 0xF4},
        1,
        {202, 104, 149, 80},
        9888,
        0,
        0,
        0,
        0,
    }
#if defined(__BACKUP_SERVER__)
    ,{0}
#endif/*__BACKUP_SERVER__*/

    , {4, 1, 0, 0} //图强版本，支持SENSOR，不支持国外版功能


};


const nvram_importance_parameter_struct NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT_SERVER_GUMI =
{
    {__DATE__},
    {__TIME__},
    {"GM"},
    {_TRACK_VER_PARAMETER_},
    {KAL_FALSE, "CMNET", "", ""},
    {
        /*atr02.tuqianggps.com*/
        {0xE1, 0xF4, 0xF2, 0xB0, 0xB2, 0xAE, 0xF4, 0xF5, 0xF1, 0xE9, 0xE1, 0xEE, 0xE7, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED},
        1, {202, 70, 2, 53}, 8821, 0, 0, 0, 0
    }
    , {
        /*agps.topstargps.com:12283*/
        {0xE4 , 0xE5 , 0xF6 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE3 , 0xEC , 0xEF , 0xF5 , 0xE4 , 0xAE , 0xE3 , 0xEF , 0xED},
        1, {183, 63, 53, 246}, 12283, 0, 0, 0, 0
    },
    {
        {

            {0XF7 , 0xF7 , 0xF7 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE1 , 0xE7 , 0xF0 , 0xF3 , 0xAE , 0xE3 , 0xEF , 0xED},
            1, {116, 204, 8, 57}, 12283, 0, 0, 0
        }
        , {
            {0xF7 , 0xF7 , 0xF7 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE1 , 0xED , 0xF3 , 0xAE , 0xE3 , 0xEF , 0xED},
            1, {116, 204, 8, 57}, 12283, 0, 0, 0
        }
        , {
            {0xE4 , 0xE5 , 0xF6 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE3 , 0xEC , 0xEF , 0xF5 , 0xE4 , 0xAE , 0xE3 , 0xEF , 0xED},
            1, {116, 204, 8, 57}, 12283, 0, 0, 0
        }
        , {
            {0xE1, 0xE7, 0xF0, 0xF3, 0xAE, 0xF4, 0xEF, 0xF0, 0xF3, 0xF4, 0xE1, 0xF2, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED},
            0, {114, 67, 59, 162}, 12283, 0, 0, 0
        }
    }
    , {
        //gt02.goomegps.net
        {0xE7 , 0xF4, 0xB0 , 0xB2, 0xAE , 0xE7 , 0xEF, 0xEF, 0xED , 0xE5 , 0xE7, 0xF0, 0xF3, 0xAE, 0xEE, 0xE5 , 0xF4},
        1,
        {202, 104, 149, 80},
        9888,
        0,
        1,
        0,
        0,
    }
#if defined(__BACKUP_SERVER__)
    ,{0}
#endif/*__BACKUP_SERVER__*/

    , {1, 1, 0, 1} //谷米版本，不支持SENSOR，不支持国外版功能

};

const nvram_importance_parameter_struct NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT_GUMI_GENERAL =
{
    {__DATE__},
    {__TIME__},
    {"TY"},
    {_TRACK_VER_PARAMETER_},
    {KAL_FALSE, "CMNET", "", ""},
    {
        /*atr02.tuqianggps.com*/
        {0xE1, 0xF4, 0xF2, 0xB0, 0xB2, 0xAE, 0xF4, 0xF5, 0xF1, 0xE9, 0xE1, 0xEE, 0xE7, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED},
        1, {202, 70, 2, 53}, 8821, 0, 0, 0, 0
    }
    , {
        /*agps.topstargps.com:12283*/
        {0xE4 , 0xE5 , 0xF6 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE3 , 0xEC , 0xEF , 0xF5 , 0xE4 , 0xAE , 0xE3 , 0xEF , 0xED},
        1, {183, 63, 53, 246}, 12283, 0, 0, 0
    }
    ,
    {
        {

            {0XF7 , 0xF7 , 0xF7 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE1 , 0xE7 , 0xF0 , 0xF3 , 0xAE , 0xE3 , 0xEF , 0xED},
            1, {116, 204, 8, 57}, 12283, 0, 0, 0
        }
        , {
            {0xF7 , 0xF7 , 0xF7 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE1 , 0xED , 0xF3 , 0xAE , 0xE3 , 0xEF , 0xED},
            1, {116, 204, 8, 57}, 12283, 0, 0, 0
        }
        , {
            {0xE4 , 0xE5 , 0xF6 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE3 , 0xEC , 0xEF , 0xF5 , 0xE4 , 0xAE , 0xE3 , 0xEF , 0xED},
            1, {116, 204, 8, 57}, 12283, 0, 0, 0
        }
        , {
            {0xE1, 0xE7, 0xF0, 0xF3, 0xAE, 0xF4, 0xEF, 0xF0, 0xF3, 0xF4, 0xE1, 0xF2, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED},
            0, {114, 67, 59, 162}, 12283, 0, 0, 0
        }
    }

    , {
        //gt02.goomegps.net
        {0xE7 , 0xF4, 0xB0 , 0xB2, 0xAE , 0xE7 , 0xEF, 0xEF, 0xED , 0xE5 , 0xE7, 0xF0, 0xF3, 0xAE, 0xEE, 0xE5 , 0xF4},
        1,
        {202, 104, 149, 80},
        9888,
        0,
        1,
        0,
        0,
    }
#if defined(__BACKUP_SERVER__)
    ,{0}
#endif/*__BACKUP_SERVER__*/

    , {2, 1, 0, 0} //谷米通用版本，不支持SENSOR，不支持国外版功能

};

#endif /* __GT02__ */

/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/
extern nvram_parameter_struct     G_parameter;
extern nvram_importance_parameter_struct G_importance_parameter_data;
extern nvram_phone_number_struct       G_phone_number;

/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/
extern void track_cust_module_sd_acc(void *arg);
extern nvram_GPRS_APN_struct *track_socket_get_use_apn(void);
extern void track_soc_send(void *par);
extern void track_cust_sleep_mode(void);
extern void track_cust_wake_mode(void);
extern void cmd_seefence(CMD_DATA_STRUCT *cmd);
extern kal_uint8 track_cust_dial_makecall(void* arg);
extern void track_cust_server2_conn(void *arg);
extern char *track_get_app_version_datetime(void);
extern kal_char *build_date_time(void);
extern char *track_cust_get_version_for_upload_server(void);
extern S32 track_cust_get_data_from_file(char *file_path, U8 *data, U32 len);
extern U32 track_get_gps_distance(void);
extern void track_set_gps_distance(U32 distance);
extern kal_bool track_recv_cmd_status(kal_uint8 par);
extern kal_uint32 track_cust_gpson_timer(kal_uint32 time);
extern void track_socket_check_apn_diskdata(void);
extern void lbs_cmd_send(CMD_DATA_STRUCT * cmd);
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
extern void track_cust_module_sw_init(void);
#endif /* __GT300S__ */
#ifdef __WIFI__
extern void track_drv_wifi_work_mode(void);
#endif /* __WIFI__ */



#if defined (__XYBB__)
extern kal_bool track_cust_check_login_01_information(void);
extern kal_bool track_cust_check_login_02_information(void);
#endif /* __XYBB__ */

/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/

#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      本地函数
******************************************************************************/

/******************************************************************************
 *  Function    -  cmd_updata_to_net
 *
 *  Purpose     -  在非在线指令情况下同步传数据到后台
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 04-01-2014,  ZengPing && Liujw  written
 * ----------------------------------------
 ******************************************************************************/
static void cmd_updata_to_net(CMD_DATA_STRUCT * cmd)
{
    if (cmd->return_sms.reply_character_encode == 0)
    {
        cmd->rsp_length = strlen(cmd->rsp_msg);
        if (cmd->rsp_length > 0)
        {
            if (cmd->return_sms.cm_type != CM_ONLINE)
            {
                track_cust_paket_msg_upload(cmd->return_sms.stamp, KAL_TRUE, cmd->rsp_msg, cmd->rsp_length);
            }
        }
    }
    else
    {
        if (cmd->return_sms.cm_type != CM_ONLINE && cmd->rsp_length > 0)
        {
            track_cust_paket_msg_upload(cmd->return_sms.stamp, KAL_FALSE, cmd->rsp_msg, cmd->rsp_length);
        }
    }
}

static kal_bool enter_lat_lon(kal_bool NS, kal_uint8 *str, float *value)
{
    if (track_fun_check_str_is_number(3, str) != 0)
    {
        *value = atof(str);
    }
    else if (NS && (str[0] == 'N' || str[0] == 'S') && track_fun_check_str_is_number(2, &str[1]) != 0)
    {
        if (str[0] == 'N')
        {
            *value = atof(&str[1]);
        }
        else
        {
            *value = -atof(&str[1]);
        }
    }
    else if (!NS && (str[0] == 'E' || str[0] == 'W') && track_fun_check_str_is_number(2, &str[1]) != 0)
    {
        if (str[0] == 'E')
            *value = atof(&str[1]);
        else
            *value = -atof(&str[1]);
    }
    else return KAL_FALSE;
    return KAL_TRUE;
}

static void return_message(CMD_DATA_STRUCT *cmd, char *english_string, char *chinese_string)
{
    /* 由于考虑到服务器的兼容性问题，在线指令仅进行英文信息回复 */
    if (G_parameter.lang == 1 && cmd->return_sms.cm_type != CM_ONLINE && cmd->return_sms.cm_type != CM_ONLINE2)
    {
        if (cmd->return_sms.cm_type == CM_AT)
        {
            cmd->rsp_length = strlen(chinese_string);
            if (cmd->rsp_length >= CM_PARAM_LONG_LEN_MAX)
            {
                cmd->rsp_length = CM_PARAM_LONG_LEN_MAX - 2;
            }
            memcpy(cmd->rsp_msg, chinese_string, cmd->rsp_length);
            cmd->return_sms.reply_character_encode = 2;
        }
        else
        {
            cmd->rsp_length = track_fun_GB2312_to_UTF16BE(chinese_string, strlen(chinese_string), cmd->rsp_msg, 320);
            cmd->return_sms.reply_character_encode = 1;
        }
    }
    else
    {
        snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "%s", english_string);
    }
}

#if defined(__AUDIO_RC__)
/******************************************************************************
 *  Function    -  track_call_mode_stop_action
 *
 *  Purpose     -
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 21-11-2013,  WangQ  written
 * ----------------------------------------
*******************************************************************************/
static void track_call_mode_stop_action(void* flag)
{
    int f = (int)flag;
    LOGD(L_APP, L_V5, "f:%d", f);

    if (f)
    {
        if (G_parameter.callmode == 0 || G_parameter.callmode == 2)
        {
            track_cust_audio_play_voice(MT_RING);
        }

        if (G_parameter.callmode >= 1)
        {
            track_cust_ring_Vibration((void*)0);
            track_cust_vib_interval(0, 5000, 0);
            track_cust_ring_Vibration((void*)1);
        }
    }
    else
    {
        track_cust_audio_play_voice(MT_DISCONNECT);

        if (TR_CALL_RING == track_get_call_state())
        {
            track_cust_audio_play_voice(MT_RING);
        }
    }
}
#endif /* __AUDIO_RC__ */

/**********************************************************************/
/**********************************************************************/
static void cmd_Uart2_PassThrough(CMD_DATA_STRUCT * cmd)
{
    int len, j;
    j = cmd->pars[1] - cmd->rcv_msg;
    //LOGD(L_APP, L_V2, "%p,%p,%p,{%s},{%s},{%s}",
    //    cmd->rcv_msg_source, cmd->pars[1], cmd->rcv_msg, cmd->pars[1], cmd->rcv_msg, cmd->rcv_msg_source);
    len = strlen(cmd->rcv_msg_source);
    LOGD(L_APP, L_V2, "%d,%d,%d", len, j, strlen(cmd->rcv_msg_source));
    LOGH(L_APP, L_V2, cmd->rcv_msg_source, strlen(cmd->rcv_msg_source));
    if (cmd->rcv_msg_source[len - 1] == 0x0D)
    {
        cmd->rcv_msg_source[len] = 0x0A;
        cmd->rcv_msg_source[len + 1] = 0x00;
        len++;
    }
    if (len > 0 && len < 200 && j > 0)
    {
        track_drv_uart2_write_data(cmd->rcv_msg_source + j, len - j);
    }
}

static void cmd_OBD_PassThrough_mode(CMD_DATA_STRUCT * cmd)
{
#if defined(__OBD__)
    int len, j;
    j = cmd->pars[1] - cmd->rcv_msg;
    //LOGD(L_APP, L_V2, "%p,%p,%p,{%s},{%s},{%s}",
    //    cmd->rcv_msg_source, cmd->pars[1], cmd->rcv_msg, cmd->pars[1], cmd->rcv_msg, cmd->rcv_msg_source);
    len = strlen(cmd->rcv_msg_source);
    LOGD(L_APP, L_V2, "%d,%d,%d", len, j, strlen(cmd->rcv_msg_source));
    LOGH(L_APP, L_V2, cmd->rcv_msg_source, strlen(cmd->rcv_msg_source));
    if (cmd->rcv_msg_source[len - 1] == 0x0D)
    {
        cmd->rcv_msg_source[len] = 0x0A;
        cmd->rcv_msg_source[len + 1] = 0x00;
        len++;
    }
    if (len > 0 && len < 200 && j > 0)
    {
        track_cust_obd_tc(cmd->rcv_msg_source + j, len - j);
    }
#endif /* __OBD__ */
}

static void cmd_OBD_cmd(CMD_DATA_STRUCT * cmd)
{
#if defined(__OBD__)
    int len, j;
    char *data;
    j = cmd->pars[1] - cmd->rcv_msg;
    len = strlen(cmd->rcv_msg_source) - j;
    data = cmd->rcv_msg_source + (cmd->pars[1] - cmd->rcv_msg);
    LOGD(L_APP, L_V2, "%d,%d,%p,%p,{%s},{%s},{%s}", j, len, cmd->rcv_msg_source,
         cmd->pars[1], cmd->rcv_msg, cmd->pars[1], cmd->rcv_msg, data);
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], ON_OFF(G_parameter.obd.sw));
        return;
    }
    else if (len > 0 && j > 0)
    {
        if (!strcmp(cmd->pars[1], "ON"))
        {
            G_parameter.obd.sw = 1;
            obd_poweron(33);
            sprintf(cmd->rsp_msg, "OBD: ON");
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        }
        else if (!strcmp(cmd->pars[1], "OFF"))
        {
            G_parameter.obd.sw = 0;
            obd_poweron(4);
            sprintf(cmd->rsp_msg, "OBD: OFF");
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        }
        else if (!strcmp(cmd->pars[1], "VERSION"))
        {
            sprintf(cmd->rsp_msg, "OBD Version:%s", track_obd_get_version());
        }
        else if (!strcmp(cmd->pars[1], "UPLOADTIME"))
        {
            int value = 0;
            if (cmd->part == 1)
            {
                sprintf(cmd->rsp_msg, "%s %s:%d", cmd->pars[0], cmd->pars[1], G_parameter.obd.upload_time);
                return;
            }
            else if (cmd->part > 2)
            {
                sprintf(cmd->rsp_msg, "ERROR:100");//指令长度不正确
                return;
            }
            if (strlen(cmd->pars[2]) > 0)
                value = atoi(cmd->pars[2]);
            if (value < 10 || value > 3600)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (10 - 3600)");
                return;
            }
            G_parameter.obd.upload_time = value;
            sprintf(cmd->rsp_msg, "%s %s: %d", cmd->pars[0], cmd->pars[1], G_parameter.obd.upload_time);
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
            obd_upload_time_update_parameter();
        }
        else if (!strcmp(cmd->pars[1], "CAR"))
        {
            int car = 0, total_mileage = 0;
            if (cmd->part == 1)
            {
                sprintf(cmd->rsp_msg, "%s %s:%d (total mileage:%d)", cmd->pars[0], cmd->pars[1], G_parameter.obd.car, G_parameter.obd.total_mileage);
                return;
            }
            else if (cmd->part > 3)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter!");
                return;
            }
            if (cmd->part >= 2)
            {
                if (strlen(cmd->pars[2]) > 0)
                    car = atoi(cmd->pars[2]);
                if (car <= 0 || car > 28)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 2 (1 - 28)");
                    return;
                }
            }
            if (cmd->part == 3)
            {
                if (strlen(cmd->pars[3]) > 0)
                    total_mileage = atoi(cmd->pars[3]);
                if (total_mileage < 0 || total_mileage > 0x00FFFFFF)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 3 (0 - 16777215)");
                    return;
                }
            }
            if (car != G_parameter.obd.car || G_parameter.obd.total_mileage != total_mileage)
            {
                track_obd_setcar_and_odometer(car, total_mileage);
                Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
            }
            sprintf(cmd->rsp_msg, "%s %s:%d (total mileage:%d)", cmd->pars[0], cmd->pars[1], G_parameter.obd.car, G_parameter.obd.total_mileage);
        }
        else if (!strcmp(cmd->pars[1], "ODOMETER"))
        {
            int total_mileage = 0;
            if (cmd->part == 1)
            {
                sprintf(cmd->rsp_msg, "%s %s:%d", cmd->pars[0], G_parameter.obd.total_mileage);
                return;
            }
            else if (cmd->part > 2)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter!");
                return;
            }
            if (cmd->part >= 2)
            {
                if (strlen(cmd->pars[2]) > 0)
                    total_mileage = atoi(cmd->pars[2]);
                if (total_mileage < 0 || total_mileage > 0x00FFFFFF)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 2 (0 - 16777215)");
                    return;
                }
            }
            if (G_parameter.obd.total_mileage != total_mileage)
            {
                track_obd_setcar_and_odometer(1, total_mileage);
                Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
            }
            sprintf(cmd->rsp_msg, "%s %s:%d", cmd->pars[0], cmd->pars[1], G_parameter.obd.total_mileage);
        }
        else if (!strcmp(cmd->pars[1], "UPLOADTYPE"))
        {
            int value = 0;
            if (strlen(cmd->pars[2]) > 0)
                value = atoi(cmd->pars[2]);
            if (value > 0 && value < 10)
                G_parameter.obd.upload_type = value;
            sprintf(cmd->rsp_msg, "upload type: %d", G_parameter.obd.upload_type);
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        }
        else if (!strcmp(cmd->pars[1], "VIN"))
        {
            track_obd_getVIN(cmd);
        }
        else if (!strcmp(cmd->pars[1], "ERROR_ID"))
        {
            if (cmd->part == 1)
            {
                track_obd_getErrorId(cmd);
            }
            else if (cmd->part == 2 && !strcmp(cmd->pars[2], "CLEAN"))
            {
                track_obd_clsobd(cmd);
            }
            else
            {
                sprintf(cmd->rsp_msg, "Error: Parameter!");
                return;
            }
        }
        else
        {
            track_cust_obd_testcmd(cmd->rsp_msg, cmd);
        }
    }
#endif /* __OBD__ */
}

static void cmd_set_version2(CMD_DATA_STRUCT * cmd)
{
    char *ver;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s", G_importance_parameter_data.version_string);
        return;
    }
    else if (cmd->part != 1)
    {
        sprintf(cmd->rsp_msg, "ERROR:100");//指令长度不正确
        return;
    }
    if (strlen(cmd->pars[1]) >= 20)
    {
        sprintf(cmd->rsp_msg, "parameter error! string length must be less than 20.");
        return;
    }
    strcpy(G_importance_parameter_data.version_string, cmd->pars[1]);
    ver = track_get_app_version();
    if (strlen(ver) <= 0)
    {
        ver = (char*)release_verno();
    }
    sprintf(cmd->rsp_msg, "[VERSION]%s_%s\r\n[BUILD]%s\r\n", ver, G_importance_parameter_data.version_string, build_date_time());
}

/******************************************************************************
 *  Function    -  cmd_get_app_version
 *
 *  Purpose     -  获取软件版本号
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_get_base_version(CMD_DATA_STRUCT * cmd)
{
    if (cmd->part == 1 && !strcmp(cmd->pars[1], "BUILD"))
    {
        sprintf(cmd->rsp_msg, "[BASE]%s [BUILD]%s", track_get_base_version(), build_date_time());
        return;
    }
    else if (cmd->part != 0)
    {
        sprintf(cmd->rsp_msg, "ERROR:100");//指令长度不正确
        return;
    }
    sprintf(cmd->rsp_msg, "[BASE]%s", track_get_base_version());
}

/******************************************************************************
 *  Function    -  cmd_get_SW_version
 *
 *  Purpose     -  获取软件版本号
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_get_version(CMD_DATA_STRUCT * cmd)
{
    char *ver, *build;
    ver = track_get_app_version();
    build = track_get_app_version_datetime();
    if (strlen(ver) <= 0)
    {
        ver = (char*)release_verno();
        build = build_date_time();
    }
    if (cmd->part == 1 && !strcmp(cmd->pars[1], "BUILD"))
    {
        if (strlen(G_importance_parameter_data.version_string) > 0)
        {
            sprintf(cmd->rsp_msg, "[VERSION]%s_%s [BUILD]%s", ver, G_importance_parameter_data.version_string, build);
        }
        else
        {
            sprintf(cmd->rsp_msg, "[VERSION]%s [BUILD]%s", ver, build);
        }
        return;
    }
    else if (cmd->part != 0)
    {
        sprintf(cmd->rsp_msg, "ERROR:100");//指令长度不正确
        return;
    }
    if (strlen(G_importance_parameter_data.version_string) > 0)
    {
        sprintf(cmd->rsp_msg, "[VERSION]%s_%s", ver, G_importance_parameter_data.version_string);
    }
    else
    {
        sprintf(cmd->rsp_msg, "[VERSION]%s", ver);
    }
}

#if defined(__GT740__)||defined(__GT420D__)
static void cmd_SYS_VERSION(CMD_DATA_STRUCT *cmd)
{
    char *p;
    int len, version;
    kal_bool factory=KAL_FALSE;
    if (cmd->part == 0)
    {
        if (!strcmp(cmd->pars[0], "FACTORYALL") || !strcmp(cmd->pars[0], "FACTORY"))
        {
            version = G_importance_parameter_data.app_version.version;
            factory=KAL_TRUE;
        }
        else
        {
            sprintf(cmd->rsp_msg, "version:%d", G_importance_parameter_data.app_version.version);
            return;
        }
    }
    else
    {
        version = track_fun_atoi(cmd->pars[1]);
    }

    if (version == 1)
    {
        G_importance_parameter_data.app_version.version=version;
        #if !defined(__GT420D__)
        if (G_parameter.teardown_str.sw != 1)
        {
            G_parameter.teardown_str.sw = 1;
            track_cust_notify_st_remove_alarm_modify(1);
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        }
        #endif
    }
    else if (version == 2)
    {
        G_importance_parameter_data.app_version.version=version;

        if (G_parameter.teardown_str.sw != 1)
        {
            G_parameter.teardown_str.sw = 1;
            track_cust_notify_st_remove_alarm_modify(1);
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        }
    }
    else if (version == 3)
    {
        G_importance_parameter_data.app_version.version=version;

        if (G_parameter.teardown_str.sw != 0)
        {
            G_parameter.teardown_str.sw = 0;
            track_cust_notify_st_remove_alarm_modify(0);
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        }
    }
    else
    {
        if (!factory)
        {
            sprintf(cmd->rsp_msg, "Error: Value only in from 1 to 3");
        }
        return;
    }
    track_cust_init_full_ele();
    Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
    sprintf(cmd->rsp_msg, "OK");

}
#endif


static void cmd_test_net_log(CMD_DATA_STRUCT * cmd)
{
    kal_uint32 value;
    int ret;

    if (cmd->part == 1)
    {
        ret = atoi((char*)cmd->pars[1]);
        value = ret;
        G_realtime_data.netLogControl = value;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        LOGD(L_CMD, L_V4, "Custom_ReStart() netLogControl=%d", G_realtime_data.netLogControl);
        sprintf(cmd->rsp_msg, "OK");
    }
    else
    {
        sprintf(cmd->rsp_msg, "netLogControl=%d", G_realtime_data.netLogControl);
    }
}

static void cmd_up_param(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        char tmp[20] = {0};
        if (G_realtime_data.up_param == 0) strcpy(tmp, "OFF");
        else if (G_realtime_data.up_param == 1) strcpy(tmp, "ON");
        else strcpy(tmp, "ERROR");
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], tmp);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }

    if (!strcmp(cmd->pars[1], "ON"))
    {
        G_realtime_data.up_param = 1;
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        G_realtime_data.up_param = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    if (Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

#ifndef __XCWS__

#if defined(__WIFI__)
void track_ljdw_1min_overtime(void *stamp)
{
    track_gps_data_struct *gd = track_drv_get_gps_data();
    if (gd->bdgga.sat >= 2)
    {
        return;
    }
    track_drv_wifi_work_mode();
    tr_stop_timer(TRACK_CUST_LJDW_OVERTIME_2MIN_TIMER_ID);
    track_cust_paket_msg_upload((kal_uint8*)stamp, KAL_TRUE, "GPS TIMEOUT", strlen("GPS TIMEOUT"));
}

void track_ljdw_2min_overtime(void *stamp)
{
    track_drv_wifi_work_mode();
    track_cust_paket_msg_upload((kal_uint8*)stamp, KAL_TRUE, "GPS TIMEOUT", strlen("GPS TIMEOUT"));
}

static void cmd_ljdw(CMD_DATA_STRUCT *cmd)
{
    static kal_uint8 stamp[4] = {0};
    track_drv_wifi_work_mode();
    track_cust_gps_work_req((void*)17);
    memcpy(stamp, cmd->return_sms.stamp, 4);
    track_start_timer(TRACK_CUST_LJDW_OVERTIME_1MIN_TIMER_ID, 60000, track_ljdw_1min_overtime, (void*) stamp); // 定位超时
    track_start_timer(TRACK_CUST_LJDW_OVERTIME_2MIN_TIMER_ID, 120000, track_ljdw_2min_overtime, (void*)stamp); // 定位超时
}
#endif /* __WIFI__ */

static void cmd_dw(CMD_DATA_STRUCT *cmd)
{
    LOGD(L_APP, L_V5, "%s, %s, %s, %s", cmd->return_sms.ph_num, cmd->pars[0], cmd->pars[1], cmd->pars[2]);
    if (cmd->part == 0 || ((cmd->return_sms.cm_type == CM_ONLINE || cmd->return_sms.cm_type == CM_ONLINE2) && cmd->part == 1 && !strcmp(cmd->pars[1], "000000")))   //超级密码写死？  chengj
    {
#if !defined(__GT420D__)
        track_cust_module_get_addr(cmd);
#endif
    }
}

/******************************************************************************
 *  Function    -  cmd_eurl
 *
 *  Purpose     -  编辑默认的URL连接
 *
 *  Description -
        EURL,网络链接#
            设置经纬度连接；默认值：http://maps.google.com/maps?q=
        EURL#
            查询设置的值
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 24-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_eurl(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s", G_parameter.url);
        return;
    }
    if (strlen(cmd->pars[1]) == 0 || cmd->part != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (strlen(cmd->pars[1]) >= 100)
    {
        sprintf(cmd->rsp_msg, "Error: URL string is too long!");
        return;
    }
    else
    {
        char tmp[100] = {0};
        int len;
        memset(G_parameter.url, 0, 100);
        len = strlen(cmd->pars[1]);
        memcpy(tmp, cmd->rcv_msg_source + (cmd->pars[1] - cmd->rcv_msg), (len >= 100 ? 99 : len));
        snprintf(G_parameter.url, 100, "%s", tmp);
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        sprintf(cmd->rsp_msg, "OK!");
    }
}

static void cmd_smstc(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d(1=ON, 0=OFF)", cmd->pars[0], G_parameter.smstc.sw);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (!strcmp(cmd->pars[1], "1") || !strcmp(cmd->pars[1], "ON"))
    {
        G_parameter.smstc.sw = 1;
    }
    else if (!strcmp(cmd->pars[1], "0") || !strcmp(cmd->pars[1], "OFF"))
    {
        G_parameter.smstc.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0/1)");
        return;
    }

    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}
#if !defined(__GT420D__)
static void cmd_ledsleep(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        char tmp[20] = {0};
        if (G_parameter.led_sleep == 2) strcpy(tmp, ON_OFF(0));
        else if (G_parameter.led_sleep == 1) strcpy(tmp, ON_OFF(1));
        else if (G_parameter.led_sleep == 3) strcpy(tmp, "LED ALWAYS OFF");
        else strcpy(tmp, "ERROR");
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], tmp);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
#if defined(__ET200__) || defined(__V20__)
    if (!strcmp(cmd->pars[1], "ON"))
    {
        G_parameter.led_sleep = 1;
        if (track_cust_gps_status() == GPS_STAUS_OFF)
        {
            track_cust_led_sleep(2);
        }
        else
        {
            track_cust_led_sleep(1);
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        G_parameter.led_sleep = 2;
        track_cust_led_sleep(1);
    }
#else
    if (!strcmp(cmd->pars[1], "ON"))
    {
        G_parameter.led_sleep = 1;
#if defined(__GT02__)
        if (track_cust_gps_status() == GPS_STAUS_OFF || G_realtime_data.key_status == 0)
        {
            track_cust_led_sleep(2);
        }
        else
        {
            track_cust_led_sleep(1);
        }
#else
    G_realtime_data.key_status = 1;
    if (track_cust_gps_status() == GPS_STAUS_OFF)
    {
        track_cust_led_sleep(2);
    }
    else
    {
        track_cust_led_sleep(1);
    }
#endif /* __GT02__ */
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        G_parameter.led_sleep = 2;
#if defined(__GT02__)
        if (G_realtime_data.key_status)
        {
            track_cust_led_sleep(1);
        }
#else
    G_realtime_data.key_status = 1;
    track_cust_led_sleep(1);
#endif /* __GT02__ */
    }
    else if (!strcmp(cmd->pars[1], "LED_ALWAYS_OFF"))
    {
        G_parameter.led_sleep = 3;
        track_cust_led_sleep(2);
    }
#endif /* __ET200__ */
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}
static void cmd_ledsw(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        char tmp[20] = {0};
        if (G_realtime_data.key_status == 0) strcpy(tmp, "OFF");
        else if (G_realtime_data.key_status == 1) strcpy(tmp, "ON");
        else strcpy(tmp, "ERROR");
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], tmp);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }

    if (!strcmp(cmd->pars[1], "ON"))
    {
        G_realtime_data.key_status = 1;
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        G_realtime_data.key_status = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }

    if (G_realtime_data.key_status == 0)
    {
        track_cust_led_sleep(2);
    }
    else if (G_realtime_data.key_status == 1)
    {
        track_cust_led_sleep(1);
    }

    if (Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}
#endif
static void cmd_lockip(CMD_DATA_STRUCT *cmd)
{
    if (cmd->supercmd != 1 && cmd->return_sms.cm_type == CM_SMS)
    {
        if (track_cust_sos_or_center_check(3, cmd->return_sms.ph_num) != 9)
        {
#if defined(__GT300__) || defined(__GT500__) ||defined(__GT03D__)||defined(__GT300S__)
            sprintf(cmd->rsp_msg, "Error: Only SOS phone numbers are allowed to set.");
#else
#if defined(__GERMANY__)
            sprintf(cmd->rsp_msg, "SMS kommt nicht von der Center-Nr. und wird nicht aktiviert.");

#else
            sprintf(cmd->rsp_msg, "Error: only SOS number and center number is able to execute the command.");
#endif/*__GERMANY__*/
#endif
            return;
        }
    }
    if (cmd->part == 0)
    {
        nvram_server_addr_struct ser;
        memcpy(&ser, &track_nvram_alone_parameter_read()->server, sizeof(nvram_server_addr_struct));
        ser.lock = 1;
        nvram_alone_parameter_cmd.server_set(&ser);
        sprintf(cmd->rsp_msg, "Domain successfully locked!");
        //track_os_intoTaskMsgQueue(track_cust_write_sim_backup_for_parameter);
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
}

static void cmd_qlockip(CMD_DATA_STRUCT *cmd)
{
    if (cmd->supercmd != 1 && cmd->return_sms.cm_type == CM_SMS)
    {
        if (track_cust_sos_or_center_check(3, cmd->return_sms.ph_num) != 9)
        {
#if defined(__GT300__) ||defined(__GT500__)||defined(__GT03D__)||defined(__GT300S__)
            sprintf(cmd->rsp_msg, "Error: Only SOS phone numbers are allowed to query.");
#else
#if defined(__GERMANY__)
            sprintf(cmd->rsp_msg, "SMS kommt nicht von der Center-Nr. und wird nicht aktiviert.");

#else
            sprintf(cmd->rsp_msg, "Error: only SOS number and center number is able to execute the command.");
#endif/*__GERMANY__*/
#endif
            return;
        }
    }
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d(0:unlock  1:lock)", cmd->pars[0],
                track_nvram_alone_parameter_read()->server.lock);
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
    }
    return;
}


#if defined(__GERMANY__)
/********************************************************************************

        ausgang,1#
            A=0/1；1 接通电路，0 断开电路
            A=2；执行接通电路, 保持2秒后断开
            A=10;执行接通电路, 保持10分钟后断开
            A=20;执行接通电路, 保持20分钟后断开
            A=30;执行接通电路, 保持30分钟后断开
        ausgang#
            查询已设置的参数
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 02-23-2017,  cjj  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_ausgang(CMD_DATA_STRUCT *cmd)
{
    kal_uint8 time;
    if (track_cust_status_acc() == 1)
    {
        sprintf(cmd->rsp_msg, "Zuendung ist eingeschaltet");
        return;
    }
    if (cmd->part != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (cmd->return_sms.cm_type == CM_SMS && track_fun_convert_phone_number_to_int(cmd->return_sms.ph_num) != track_fun_convert_phone_number_to_int(G_parameter.centerNumber))
    {
#if defined(__GERMANY__)
        return_message(
            cmd,
            "SMS kommt nicht von der Center-Nr.und wird nicht aktiviert.",
            "来自非法中心号码的指令不执行！");
#else
        return_message(
            cmd,
            "The command is not from the center number, The command is not executed!",
            "来自非法中心号码的指令不执行！");
#endif
        return;
    }
    if (!strcmp(cmd->pars[1], "0") || !strcmp(cmd->pars[1], "1") || !strcmp(cmd->pars[1], "2") ||
            !strcmp(cmd->pars[1], "10") || !strcmp(cmd->pars[1], "20") || !strcmp(cmd->pars[1], "30"))
    {
        time = track_fun_atoi(cmd->pars[1]);
        track_cust_module_Preheat_System(time);
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 error");
        return;
    }

    sprintf(cmd->rsp_msg, "OK!");
}
#endif

#else
static void cmd_acclt(CMD_DATA_STRUCT *cmd)
{
    int value;
    LOGD(L_APP, L_V5, "");
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], G_parameter.acc_starting_time);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    value = track_fun_atoi(cmd->pars[1]);
    if (value < 1 || value > 65535)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (1 to 65535)");
        return;
    }
    if (G_parameter.acc_starting_time != value)
    {
        G_parameter.acc_starting_time = value;
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    sprintf(cmd->rsp_msg, "OK!");
}
#endif
/******************************************************************************
 *  Function    -  cmd_lang
 *
 *  Purpose     -  语言设置
 *
 *  Description -
        LANG,X#
            X=1/2；1 中文，2 英文；默认值为：1
        LANG#
            返回当前设置的语言
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_lang(CMD_DATA_STRUCT *cmd)
{
    int value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d (0=English, 1=Chinese)", cmd->pars[0], G_parameter.lang);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    value = track_fun_atoi2(0,cmd->pars[1]);
    if (value < 0 || value > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Range error. (0=English, 1=Chinese)");
        return;
    }
    G_parameter.lang = value;
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

static void cmd_apn(CMD_DATA_STRUCT *cmd)
{
    nvram_GPRS_APN_struct apn = {0};
    char tmp[100] = {0};
    int len;
    if (cmd->part == 0)
    {
        nvram_GPRS_APN_struct *use_apn = track_socket_get_use_apn();
#if defined(__GERMANY__)
        sprintf(cmd->rsp_msg, "Eingestellter APN:%s,%s,%s\r\nAPN Auto Set:%s",
                use_apn->apn,
                use_apn->username,
                use_apn->password,
                (track_nvram_alone_parameter_read()->apn.custom_apn_valid ? s_OFF : s_ON));
#else
        sprintf(cmd->rsp_msg, "Currently use APN:%s,%s,%s\r\nAPN Auto Set:%s",
                use_apn->apn,
                use_apn->username,
                use_apn->password,
                (track_nvram_alone_parameter_read()->apn.custom_apn_valid ? s_OFF : s_ON));
#endif
        return;
    }
    else if (cmd->part > 3)
    {
        sprintf(cmd->rsp_msg, "FAIL");
        return;
    }
    memcpy(&apn, &track_nvram_alone_parameter_read()->apn, sizeof(nvram_GPRS_APN_struct));
    len = strlen(cmd->pars[1]);
    if (len > 0 && len < 100)
    {
        memcpy(tmp, cmd->rcv_msg_source + (cmd->pars[1] - cmd->rcv_msg), len);
        strcpy(apn.apn, tmp);
        memset(apn.username, 0, 32);
        memset(apn.password, 0, 32);
    }
    else
    {
        sprintf(cmd->rsp_msg, "FAIL");
        return;
    }
    if (cmd->part >= 2)
    {
        len = strlen(cmd->pars[2]);
        if (len >= 0 && len < 32)
        {
            memcpy(tmp, cmd->rcv_msg_source + (cmd->pars[2] - cmd->rcv_msg), len);
            tmp[len] = 0;
            strcpy(apn.username, tmp);
        }
        else
        {
            sprintf(cmd->rsp_msg, "FAIL");
            return;
        }
    }
    if (cmd->part == 3)
    {
        len = strlen(cmd->pars[3]);
        if (len >= 0 && len < 32)
        {
            memcpy(tmp, cmd->rcv_msg_source + (cmd->pars[3] - cmd->rcv_msg), len);
            tmp[len] = 0;
            strcpy(apn.password, tmp);
        }
        else
        {
            sprintf(cmd->rsp_msg, "FAIL");
            return;
        }
    }
    apn.custom_apn_valid = 1;  //APN设置后，自适应APN需自动关闭
    if (memcmp(&track_nvram_alone_parameter_read()->apn, &apn, sizeof(nvram_GPRS_APN_struct)))
    {
        nvram_alone_parameter_cmd.apn_set(&apn);
#if defined(__GERMANY__)
        sprintf(cmd->rsp_msg, "OK! Zum Speichern des neuen APN erfolgt ein Neustart in 10 Sek.");
#else
        sprintf(cmd->rsp_msg, "SUCCESS");
#endif
        track_socket_check_apn_diskdata();
        //track_os_intoTaskMsgQueue(track_cust_write_sim_backup_for_parameter);
        track_cust_restart(59, 10);
    }
    else
    {
        sprintf(cmd->rsp_msg, "FAIL");
    }
}

static void cmd_asetapn(CMD_DATA_STRUCT *cmd)
{
    kal_bool value;
    if (cmd->part == 0)
    {
        nvram_GPRS_APN_struct *use_apn = track_socket_get_use_apn();
#if defined(__GERMANY__)
        sprintf(cmd->rsp_msg, "%s:%s\r\nEingestellter APN:%s,%s,%s", cmd->pars[0],
                (track_nvram_alone_parameter_read()->apn.custom_apn_valid ? s_OFF : s_ON),
                use_apn->apn,
                use_apn->username,
                use_apn->password);
#else
        sprintf(cmd->rsp_msg, "%s:%s\r\nCurrently use APN:%s,%s,%s", cmd->pars[0],
                (track_nvram_alone_parameter_read()->apn.custom_apn_valid ? s_OFF : s_ON),
                use_apn->apn,
                use_apn->username,
                use_apn->password);
#endif
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (!strcmp(cmd->pars[1], "ON")) // APN 自适应有效
    {
        value = 0;
    }
    else if (!strcmp(cmd->pars[1], "OFF")) // APN 自适应失效
    {
        value = 1;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    if (value != track_nvram_alone_parameter_read()->apn.custom_apn_valid)
    {
        nvram_GPRS_APN_struct apn = {0};
        memcpy(&apn, &track_nvram_alone_parameter_read()->apn, sizeof(nvram_GPRS_APN_struct));
        apn.custom_apn_valid = value;
        nvram_alone_parameter_cmd.apn_set(&apn);
#if defined(__GERMANY__)
        sprintf(cmd->rsp_msg, "OK! Zum Speichern des neuen APN erfolgt ein Neustart in 10 Sek.");
#else
        sprintf(cmd->rsp_msg, "OK! for the newly-set APN to take effect, the device will reboot after 10s.");
#endif
        //track_os_intoTaskMsgQueue(track_cust_write_sim_backup_for_parameter);
        track_cust_restart(59, 10);
    }
    else
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
}

static void cmd_imei(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], (char *)track_drv_get_imei(0));
        return;
    }
    if (cmd->part == 2 && !strcmp(cmd->pars[1], "1"))
    {
        nvram_ef_imei_imeisv_struct imei_data = {0};
        int len, i, j;
        char imei_tmp[17];
        len = strlen(cmd->pars[2]);
        if (len != 15)
        {
            sprintf(cmd->rsp_msg, "Error: length!");
            return;
        }
        Track_nvram_read(NVRAM_EF_IMEI_IMEISV_LID, 1, (void *)&imei_data, NVRAM_EF_IMEI_IMEISV_SIZE);
        sprintf(imei_tmp, "%s0", cmd->pars[2]);
        LOGD(L_APP, L_V5, "%s", imei_tmp);
        for (i = 0, j = 0; i < 8; i++)
        {
            if (imei_tmp[j] < 0x30 || imei_tmp[j] > 0x39)
            {
                sprintf(cmd->rsp_msg, "Error: The %d non-numeric characters!", j);
                return;
            }
            imei_data.imei[i] = imei_tmp[j++] - 0x30;
            if (imei_tmp[j] < 0x30 || imei_tmp[j] > 0x39)
            {
                sprintf(cmd->rsp_msg, "Error: The %d non-numeric characters!", j);
                return;
            }
            imei_data.imei[i] |= ((imei_tmp[j++] - 0x30) << 4);
        }
        LOGH(L_APP, L_V5, imei_data.imei, 10);
        Track_nvram_write(NVRAM_EF_IMEI_IMEISV_LID, 1, (void *)&imei_data, NVRAM_EF_IMEI_IMEISV_SIZE);
        track_read_imei_data();
        #if defined(__MQTT__)
        MQTT_main();
        #else
        track_soc_gprs_reconnect((void*)21);
        #endif

    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter!");
        return;
    }
    sprintf(cmd->rsp_msg, "OK! %s", (char *)track_drv_get_imei(0));
    track_os_intoTaskMsgQueue(track_cust_write_sim_backup_for_parameter);
}

static void cmd_imsi(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], (char *)track_drv_get_imsi(0));
        return;
    }
}

static void cmd_iccid(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], (char *)OTA_ell_get_iccid());
        return;
    }
}

static void cmd_viewip(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
#ifdef __GT02__
        if (G_importance_parameter_data.dserver.server_type == 1)
        {
            sprintf(cmd->rsp_msg, "Default:%s,%s,%s,%d.%d.%d.%d:%d; ",
                    G_importance_parameter_data.dserver.conecttype == 0 ? "IP" : "domain",
                    G_importance_parameter_data.dserver.soc_type == 0 ? "TCP" : "UDP",
                    track_domain_decode(G_importance_parameter_data.dserver.url),
                    G_importance_parameter_data.dserver.server_ip[0],
                    G_importance_parameter_data.dserver.server_ip[1],
                    G_importance_parameter_data.dserver.server_ip[2],
                    G_importance_parameter_data.dserver.server_ip[3],
                    G_importance_parameter_data.dserver.server_port
                   );
            track_soc_view_IP(cmd->rsp_msg + strlen(cmd->rsp_msg));
            return;
        }
#endif /* __GT02__ */

        sprintf(cmd->rsp_msg, "Default:%s,%s,%s,%d.%d.%d.%d:%d; ",
                track_nvram_alone_parameter_read()->server.conecttype == 0 ? "IP" : "domain",
                track_nvram_alone_parameter_read()->server.soc_type == 0 ? "TCP" : "UDP",
                track_domain_decode(track_nvram_alone_parameter_read()->server.url),
                track_nvram_alone_parameter_read()->server.server_ip[0],
                track_nvram_alone_parameter_read()->server.server_ip[1],
                track_nvram_alone_parameter_read()->server.server_ip[2],
                track_nvram_alone_parameter_read()->server.server_ip[3],
                track_nvram_alone_parameter_read()->server.server_port
               );
        track_soc_view_IP(cmd->rsp_msg + strlen(cmd->rsp_msg));
    }
}


/******************************************************************************
 *  Function    -  cmd_server
 *
 *  Purpose     -  设置定位服务器地址参数
 *
 *  Description -  SERVER,<使用密码>, <模式>,<地址>,<端口>,<传输协议模式>#
 *                 SERVER,0,202.173.231.112,8821,0#
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 08-01-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_server(CMD_DATA_STRUCT *cmd)
{
    int value, plen;
    nvram_server_addr_struct server = {0};
    extern  nvram_importance_parameter_struct NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT;
#ifdef __GT02__
    if (G_importance_parameter_data.app_version.server_lock && strcmp(cmd->pars[0], "SERVER1"))
    {
        return ;
    }
    if (G_importance_parameter_data.dserver.server_type == 1 && (cmd->part == 0))
    {
        if (G_importance_parameter_data.dserver.conecttype == 0)
        {
            sprintf(cmd->rsp_msg, "DSERVER,0,%d.%d.%d.%d,%d,%d",
                    G_importance_parameter_data.dserver.server_ip[0], G_importance_parameter_data.dserver.server_ip[1],
                    G_importance_parameter_data.dserver.server_ip[2], G_importance_parameter_data.dserver.server_ip[3],
                    G_importance_parameter_data.dserver.server_port, G_importance_parameter_data.dserver.soc_type);
        }
        else
        {
            sprintf(cmd->rsp_msg, "DSERVER,%d,%s,%d,%d",
                    G_importance_parameter_data.dserver.conecttype,
                    track_domain_decode(G_importance_parameter_data.dserver.url),
                    G_importance_parameter_data.dserver.server_port,
                    G_importance_parameter_data.dserver.soc_type);
        }
        return;
    }
#endif /* __GT02__ */

    if (cmd->part == 0)
    {
        if (track_nvram_alone_parameter_read()->server.conecttype == 0)
        {
            sprintf(cmd->rsp_msg, "SERVER,0,%d.%d.%d.%d,%d,%d",
                    track_nvram_alone_parameter_read()->server.server_ip[0], track_nvram_alone_parameter_read()->server.server_ip[1],
                    track_nvram_alone_parameter_read()->server.server_ip[2], track_nvram_alone_parameter_read()->server.server_ip[3],
                    track_nvram_alone_parameter_read()->server.server_port, track_nvram_alone_parameter_read()->server.soc_type);
        }
        else
        {
            sprintf(cmd->rsp_msg, "SERVER,%d,%s,%d,%d",
                    track_nvram_alone_parameter_read()->server.conecttype,
                    track_domain_decode(track_nvram_alone_parameter_read()->server.url),
                    track_nvram_alone_parameter_read()->server.server_port,
                    track_nvram_alone_parameter_read()->server.soc_type);
        }
        return;
    }
    else if (cmd->part == 1 && !strcmp(cmd->pars[CM_Par1], "RESET"))
    {
        nvram_alone_parameter_cmd.server_set(&NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT.server);
        sprintf(cmd->rsp_msg, "reset OK!");
        return;
    }
    else if (cmd->part == 1 && !strcmp(cmd->pars[CM_Par1], "NULL"))
    {
        memset(&server, 0, sizeof(nvram_server_addr_struct));
        nvram_alone_parameter_cmd.server_set(&server);
        sprintf(cmd->rsp_msg, "OK");
        track_soc_disconnect();
        //track_os_intoTaskMsgQueue(track_cust_write_sim_backup_for_parameter);
        return;
    }
    if (cmd->part != 3 && cmd->part != 4)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (track_nvram_alone_parameter_read()->server.lock != 0)
    {
#if defined(__GERMANY__)
        sprintf(cmd->rsp_msg, "Fehler! Die Plattform Adresse wird nicht erkannt!");
#else
        sprintf(cmd->rsp_msg, "Error: The platform addresses are Locked modify!");
#endif
        return;
    }
    memset(cmd->rsp_msg, 0, sizeof(cmd->rsp_msg));

    memcpy(&server, &track_nvram_alone_parameter_read()->server, sizeof(nvram_server_addr_struct));

    value = track_fun_atoi(cmd->pars[CM_Par1]);
    if (value != 0 && value != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0/1)");
        return;
    }
    server.conecttype = value;

    plen = strlen(cmd->pars[CM_Par2]);
    if (plen <= 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 2 is empty!");
        return;
    }
    if (plen >= 99)
    {
        sprintf(cmd->rsp_msg, "Error: Parameters 2 characters too long!");
        return;
    }
    if (track_fun_check_ip(cmd->pars[CM_Par2]))
    {
        if (server.conecttype != 0)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 domain is not formatted correctly!");
            return;
        }
        memset(server.server_ip, 0, 4);
        if (!track_fun_asciiIP2Hex(cmd->pars[CM_Par2], (void*)&server.server_ip[0]))
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 is not a valid IP address format!");
            return;
        }
        server.url[0] = 0;
    }
    else
    {
        if (server.conecttype != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 is not a valid IP address format!");
            return;
        }
        if (strstr(cmd->pars[CM_Par2], "http://"))
        {
            sprintf(cmd->rsp_msg, "Error: The domain name does not include \"http://\"!");
            return;
        }
        {
            char domain[TRACK_MAX_URL_SIZE] = {0};
            memcpy(domain, cmd->rcv_msg_source + (cmd->pars[CM_Par2] - cmd->rcv_msg), plen);
            track_domain_encode(domain, server.url);
            memset(server.server_ip, 0, 4);
            memcpy(G_realtime_data.server_ip, server.server_ip, 4);
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        }
    }

    if (!track_fun_check_data_is_port(cmd->pars[CM_Par3]))
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 3 (0~65535)");
        return;
    }
    server.server_port = track_fun_string2U16(cmd->pars[CM_Par3]);

    if (cmd->part == 4)
    {
        value = track_fun_atoi(cmd->pars[CM_Par4]);
        if (value != 0 && value != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 4 (0/1)");
            return;
        }
    }
    server.soc_type = 0; // 当前只支持TCP
    nvram_alone_parameter_cmd.server_set(&server);
#if defined(__GT02__)
    G_importance_parameter_data.dserver.server_type = 0;
    track_set_server_ip_status(0);
    G_importance_parameter_data.dserver.ip_update = 0;
    server.ip_update = 0;
    nvram_alone_parameter_cmd.server_set(&server);
#endif
#if defined(__GT530__)
    track_cust_serverset(0);
#endif
    Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
    sprintf(cmd->rsp_msg, "OK");
    track_soc_reset_reconnect_count2();
    track_os_intoTaskMsgQueueExt(track_soc_gprs_reconnect, (void*)22);//修改域名后，重新建立连接
    //track_os_intoTaskMsgQueue(track_cust_write_sim_backup_for_parameter);

#if defined(__GT740__)
    track_cust_module_resetpro();
    if (cmd->return_sms.cm_type != CM_PARAM_CMD)
    {
        if (RUNING_TIME_SEC>10)
        {
            if (G_parameter.wifi.sw)
            {
                track_cust_sendWIFI_paket();
            }
            else
            {
                track_cust_sendlbs_limit();
            }
        }
        else
        {
            tr_start_timer(TRACK_CUST_WIFI_WORK_TIMER_ID, 2*1000, track_cust_sendWIFI_paket);
        }
        track_cust_lbs_send_rst(KAL_TRUE);
        if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
        {
            track_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
        }
    }
#endif
}

static void cmd_server2(CMD_DATA_STRUCT *cmd)
{
    int value, plen;
    nvram_server_addr_struct server = {0};

    if (cmd->part == 0)
    {
        if (track_nvram_alone_parameter_read()->ams_server.conecttype == 0)
        {
            sprintf(cmd->rsp_msg, "SERVER2,0,%d.%d.%d.%d,%d,%d",
                    track_nvram_alone_parameter_read()->ams_server.server_ip[0], track_nvram_alone_parameter_read()->ams_server.server_ip[1],
                    track_nvram_alone_parameter_read()->ams_server.server_ip[2], track_nvram_alone_parameter_read()->ams_server.server_ip[3],
                    track_nvram_alone_parameter_read()->ams_server.server_port, track_nvram_alone_parameter_read()->ams_server.soc_type);
        }
        else if (track_nvram_alone_parameter_read()->ams_server.conecttype == 1)
        {
            sprintf(cmd->rsp_msg, "SERVER2,%d,%s,%d,%d",
                    track_nvram_alone_parameter_read()->ams_server.conecttype,
                    track_domain_decode(track_nvram_alone_parameter_read()->ams_server.url),
                    track_nvram_alone_parameter_read()->ams_server.server_port,
                    track_nvram_alone_parameter_read()->ams_server.soc_type);
        }
        else
        {
            sprintf(cmd->rsp_msg, "SERVER2,OFF");
        }
        return;
    }
    if (cmd->part != 3 && cmd->part != 4)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (track_nvram_alone_parameter_read()->ams_server.lock != 0)
    {
#if defined(__GERMANY__)
        sprintf(cmd->rsp_msg, "Fehler! Die Plattform Adresse wird nicht erkannt!");
#else
        sprintf(cmd->rsp_msg, "Error: The platform addresses are Locked modify!");
#endif
        return;
    }
    memset(cmd->rsp_msg, 0, sizeof(cmd->rsp_msg));

    memcpy(&server, &track_nvram_alone_parameter_read()->ams_server, sizeof(nvram_server_addr_struct));

    value = track_fun_atoi(cmd->pars[CM_Par1]);
    if (value != 0 && value != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0/1)");
        return;
    }
    server.conecttype = value;

    plen = strlen(cmd->pars[CM_Par2]);
    if (plen <= 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 2 is empty!");
        return;
    }
    if (plen >= 99)
    {
        sprintf(cmd->rsp_msg, "Error: Parameters 2 characters too long!");
        return;
    }
    if (track_fun_check_ip(cmd->pars[CM_Par2]))
    {
        if (server.conecttype != 0)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 domain is not formatted correctly!");
            return;
        }
        memset(server.server_ip, 0, 4);
        if (!track_fun_asciiIP2Hex(cmd->pars[CM_Par2], (void*)&server.server_ip[0]))
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 is not a valid IP address format!");
            return;
        }
    }
    else
    {
        if (server.conecttype != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 is not a valid IP address format!");
            return;
        }
        if (strstr(cmd->pars[CM_Par2], "http://"))
        {
            sprintf(cmd->rsp_msg, "Error: The domain name does not include \"http://\"!");
            return;
        }
        {
            char domain[TRACK_MAX_URL_SIZE] = {0};
            memcpy(domain, cmd->rcv_msg_source + (cmd->pars[CM_Par2] - cmd->rcv_msg), plen);
            track_domain_encode(domain, server.url);
        }
    }

    if (!track_fun_check_data_is_port(cmd->pars[CM_Par3]))
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 3 (0~65535)");
        return;
    }
    server.server_port = track_fun_string2U16(cmd->pars[CM_Par3]);

    if (cmd->part == 4)
    {
        value = track_fun_atoi(cmd->pars[CM_Par4]);
        if (value != 0 && value != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 4 (0/1)");
            return;
        }
    }
#if defined(__GT530__)
    track_cust_serverset(2);
#endif
    server.soc_type = 0; // 当前只支持TCP
    nvram_alone_parameter_cmd.server_ams_set(&server);
    sprintf(cmd->rsp_msg, "OK!");

}


#if defined (__LUYING__)
static void cmd_server4(CMD_DATA_STRUCT *cmd)
{
    int value, plen;
    nvram_server_addr_struct server = {0};

    if (cmd->part == 0)
    {
        if (G_importance_parameter_data.server4.conecttype == 0)
        {
            sprintf(cmd->rsp_msg, "SERVER4,0,%d.%d.%d.%d,%d,%d",
                    G_importance_parameter_data.server4.server_ip[0], G_importance_parameter_data.server4.server_ip[1],
                    G_importance_parameter_data.server4.server_ip[2], G_importance_parameter_data.server4.server_ip[3],
                    G_importance_parameter_data.server4.server_port, G_importance_parameter_data.server4.soc_type);
        }
        else if (G_importance_parameter_data.server4.conecttype == 1)
        {
            sprintf(cmd->rsp_msg, "SERVER4,%d,%s,%d,%d",
                    G_importance_parameter_data.server4.conecttype,
                    track_domain_decode(G_importance_parameter_data.server4.url),
                    G_importance_parameter_data.server4.server_port,
                    G_importance_parameter_data.server4.soc_type);
        }
        else
        {
            sprintf(cmd->rsp_msg, "SERVER4,OFF");
        }
        return;
    }
    if (cmd->part != 3 && cmd->part != 4)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (G_importance_parameter_data.server4.lock != 0)
    {
        sprintf(cmd->rsp_msg, "Error: The platform addresses are Locked modify!");
        return;
    }
    memset(cmd->rsp_msg, 0, sizeof(cmd->rsp_msg));

    memcpy(&server, &G_importance_parameter_data.server4, sizeof(nvram_server_addr_struct));

    value = track_fun_atoi(cmd->pars[CM_Par1]);
    if (value != 0 && value != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0/1)");
        return;
    }
    server.conecttype = value;

    plen = strlen(cmd->pars[CM_Par2]);
    if (plen <= 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 2 is empty!");
        return;
    }
    if (plen >= 99)
    {
        sprintf(cmd->rsp_msg, "Error: Parameters 2 characters too long!");
        return;
    }
    if (track_fun_check_ip(cmd->pars[CM_Par2]))
    {
        if (server.conecttype != 0)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 domain is not formatted correctly!");
            return;
        }
        memset(server.server_ip, 0, 4);
        if (!track_fun_asciiIP2Hex(cmd->pars[CM_Par2], (void*)&server.server_ip[0]))
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 is not a valid IP address format!");
            return;
        }
    }
    else
    {
        if (server.conecttype != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 is not a valid IP address format!");
            return;
        }
        if (strstr(cmd->pars[CM_Par2], "http://"))
        {
            sprintf(cmd->rsp_msg, "Error: The domain name does not include \"http://\"!");
            return;
        }
        {
            char domain[TRACK_MAX_URL_SIZE] = {0};
            memcpy(domain, cmd->rcv_msg_source + (cmd->pars[CM_Par2] - cmd->rcv_msg), plen);
            track_domain_encode(domain, server.url);
        }
    }

    if (!track_fun_check_data_is_port(cmd->pars[CM_Par3]))
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 3 (0~65535)");
        return;
    }
    server.server_port = track_fun_string2U16(cmd->pars[CM_Par3]);

    if (cmd->part == 4)
    {
        value = track_fun_atoi(cmd->pars[CM_Par4]);
        if (value != 0 && value != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 4 (0/1)");
            return;
        }
    }
    server.soc_type = 0; // 当前只支持TCP
    memcpy(&G_importance_parameter_data.server4, &server, sizeof(nvram_server_addr_struct));
    if (Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
        track_socket_check_apn_diskdata();
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}
#endif /* __LUYING__ */

static void cmd_agps(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], ON_OFF(G_parameter.agps.sw));
        return;
    }
    /* else if(cmd->part > 1)
     {
         sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
         return;
     }*/
    if (!strcmp(cmd->pars[1], "ON"))
    {
        G_parameter.agps.sw = 1;
        track_cust_active_epoint();
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        G_parameter.agps.sw = 0;
    }
#if 0/*2503平台使用的GPS是内置的，AGPS功能也是独立实现的*/
    else if (!strcmp(cmd->pars[1], "LPON"))
    {
        G_parameter.agps.last_position_valid = 1;
    }
    else if (!strcmp(cmd->pars[1], "LPOFF"))
    {
        G_parameter.agps.last_position_valid = 0;
    }
    else if (!strcmp(cmd->pars[1], "UPDATE2"))
    {
        track_cust_server2_login();
        sprintf(cmd->rsp_msg, "OK!");
        return;
    }
    else if (!strcmp(cmd->pars[1], "UPDATE"))
    {
        track_cust_agps_get_data_force(2);
        sprintf(cmd->rsp_msg, "OK!");
        return;
    }
    else if (!strcmp(cmd->pars[1], "MODE"))
    {
        sprintf(cmd->rsp_msg, "MODE:%d", G_parameter.agps.server_mode);
        return;
    }
    else if (!strcmp(cmd->pars[1], "MODE0"))
    {
        G_parameter.agps.server_mode = 0;
    }
    else if (!strcmp(cmd->pars[1], "MODE1"))
    {
        G_parameter.agps.server_mode = 1;
    }
    else if (!strcmp(cmd->pars[1], "STATUS"))
    {
        int l;
        l = sprintf(cmd->rsp_msg, "%s:%s\r\n", cmd->pars[0], ON_OFF(G_parameter.agps.sw));
        track_gps_hour_to_utc_view(cmd->rsp_msg + l);
        //sprintf(cmd->rsp_msg, "OK!");
        return;
    }
    else if (!strcmp(cmd->pars[1], "DELEPO"))
    {
        kal_int8 result = track_epo_delete_file();
        if (result)
        {
            sprintf(cmd->rsp_msg, "Del EPO Error %d", result);
        }
        else
        {
            sprintf(cmd->rsp_msg, "OK");
        }
        return;
    }
#endif /* 0 */
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1");
        return;
    }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

/******************************************************************************
 *  Function    -  cmd_ams
 *
 *  Purpose     -  AMS Server2服务器开关控制
 *
 *  Description -
 *       track_nvram_alone_parameter_read()->ams_server.conecttype
 *             开     关
 *          =  0      10
 *          =  1      11
 * modification history
 * ----------------------------------------
 * v1.0  , 05-06-2015,  Cml  written
 * ----------------------------------------
 ******************************************************************************/
static void cmd_ams(CMD_DATA_STRUCT *cmd)
{
    nvram_server_addr_struct server2 ={0};
    memcpy(&server2,&track_nvram_alone_parameter_read()->ams_server,sizeof(nvram_server_addr_struct));
    if (cmd->part == 0)
    {
        if (track_nvram_alone_parameter_read()->ams_server.conecttype >= 2)
            sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], ON_OFF(0));
        else
            sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], ON_OFF(1));
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (track_nvram_alone_parameter_read()->ams_server.conecttype >= 10)
        {
            server2.conecttype -= 10;
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (track_nvram_alone_parameter_read()->ams_server.conecttype < 10)
        {
            server2.conecttype += 10;
        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    nvram_alone_parameter_cmd.server_ams_set(&server2);
    sprintf(cmd->rsp_msg, "OK!");
}

static void cmd_amsconn(CMD_DATA_STRUCT *cmd)
{
    S32 value;
    nvram_check_ota_struct g_otacheck = {0};
    Track_nvram_read(NVRAM_EF_CHECK_OTA_LID, 1, (void *)&g_otacheck, NVRAM_EF_CHECK_OTA_SIZE);
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], g_otacheck.resetCountMax);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    value = track_fun_atoi(cmd->pars[1]);
    if (value < 1 || value > 100)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (1 to 100)");
        return;
    }
    g_otacheck.resetCountMax = value;
    if (Track_nvram_write(NVRAM_EF_CHECK_OTA_LID, 1, (void *)&g_otacheck, NVRAM_EF_CHECK_OTA_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

static void cmd_asetgmt(CMD_DATA_STRUCT *cmd)
{
    if (!track_cust_is_upload_UTC_time())
    {
        return;
    }
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s; Current Timezone(GMT):%c,%d,%d", cmd->pars[0],
                ON_OFF(G_parameter.zone.timezone_auto), G_parameter.zone.zone,
                G_parameter.zone.time, G_parameter.zone.time_min);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (!strcmp(cmd->pars[1], "ON"))
    {
        G_parameter.zone.timezone_auto = 1;
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        G_parameter.zone.timezone_auto = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
    if (G_parameter.zone.timezone_auto == 1)
    {
        nvram_gps_time_zone_struct *zone_p;
        zone_p = track_mcc_to_timezone();
        if (zone_p != NULL)
        {
            memcpy(&G_parameter.zone, zone_p, sizeof(nvram_gps_time_zone_struct));
        }
    }
}

void cmd_check(CMD_DATA_STRUCT *cmd)
{
    extern kal_uint8* track_drv_get_420d_mcu_version(void);
    int n, g, i, j, x;// signal = 0;
    char tmp[1000] = {0}, PRN[80] = {0};
    kal_bool head = 1;
    float volt = track_cust_get_battery_volt();
    nvram_GPRS_APN_struct *use_apn = track_socket_get_use_apn();
    track_gps_data_struct *gd = track_drv_get_gps_data();
    charge_measure_struct charge_measure = {0};
    applib_time_struct ap = {0};
    char *ver = track_get_app_version();
    track_bmt_get_ADC_result(&charge_measure);
    n = snprintf(tmp, 999, "IMEI:%s;VERSION:%s;MCU_VERSION:%s;", (char *)track_drv_get_imei(0), ver, track_drv_get_420d_mcu_version());
    if (track_nvram_alone_parameter_read()->server.conecttype == 0)
    {
        n += sprintf(tmp + n, "SERVER:%d.%d.%d.%d,%d;",
                     track_nvram_alone_parameter_read()->server.server_ip[0], track_nvram_alone_parameter_read()->server.server_ip[1],
                     track_nvram_alone_parameter_read()->server.server_ip[2], track_nvram_alone_parameter_read()->server.server_ip[3],
                     track_nvram_alone_parameter_read()->server.server_port);
    }
    else
    {
        n += sprintf(tmp + n, "SERVER:%s,%d;",
                     track_domain_decode(track_nvram_alone_parameter_read()->server.url),
                     track_nvram_alone_parameter_read()->server.server_port);
        n += sprintf(tmp + n, "GET IP:%s;", track_soc_ip_from_domain());
    }

    n += sprintf(tmp + n, "APN:%s,%d;CSQ:%d;GPRS:%d;",
                 use_apn->apn, (track_nvram_alone_parameter_read()->apn.custom_apn_valid ? 0 : 1),
                 track_cust_get_rssi(-1),
                 track_soc_login_status());

    g = track_cust_gps_status();

    /* 找出信号强度大于40db的卫星个数 */
    //  for(x = 0; x < MTK_GPS_SV_MAX_NUM; x++)
    //      if(gd->gpgsv.SNR[x] > 40) signal++;

    for (i = 0, j = 0; i < gd->gpgga.sat && i < MTK_GPS_SV_MAX_NUM && i < 12; i++)
    {
        for (x = 0; x < MTK_GPS_SV_MAX_NUM; x++)
        {
            if (gd->gpgsv.Satellite_ID[x] == gd->gpgsa.satellite_used[i])
            {
                if (head)
                    j += snprintf(PRN + j, 5, "%d", gd->gpgsv.SNR[x]);
                else
                    j += snprintf(PRN + j, 5, ",%d", gd->gpgsv.SNR[x]);
                head = 0;
                break;
            }
            else if (x == MTK_GPS_SV_MAX_NUM - 1)
            {
                LOGD(L_APP, L_V5, "not find:%d", gd->gpgsa.satellite_used[i]);
            }
        }
    }

    if (g >= GPS_STAUS_2D)
    {
        //1 GPS已定位
        n += sprintf(tmp + n, "GPS:1,%d,%s;",
                     gd->gpgga.sat, PRN);
    }
    else if (g == GPS_STAUS_OFF)
    {
        //2 GPS未开启
        n += sprintf(tmp + n, "GPS:2;");
    }
    else
    {
        //3 GPS搜星中
        n += sprintf(tmp + n, "GPS:3,%d,%s;",
                     gd->gpgga.sat, PRN);
    }

    n += sprintf(tmp + n, "BAT:%.2f;", volt / 1000000);

#ifdef __EXT_VBAT_ADC__
    /* 外部电压，部分项目具备 */
    n += sprintf(tmp + n, "POWER:%0.2f;", (float)track_cust_get_ext_volt() / 1000000.0) ;
#endif /* __EXT_VBAT_ADC__ */

#if defined (__NT33__)
#if defined (__FUN_WITH_ACC__)
    if (G_parameter.gps_work.Fti.sw)
        n += sprintf(tmp + n, "TIMER:%d,%d;", G_parameter.gps_work.Fti.interval_acc_on, G_parameter.gps_work.Fti.interval_acc_off);
#else
    if (G_parameter.gps_work.Fti.sw)
        n += sprintf(tmp + n, "TIMER:%d,%d;", G_parameter.gps_work.Fti.interval_acc_off, G_parameter.gps_work.Fti.lbs_interval);
#endif /* __FUN_WITH_ACC__ */

#else
#if !defined(__GT420D__)
    if (G_parameter.gps_work.Fti.sw)
        n += sprintf(tmp + n, "TIMER:%d,%d;", G_parameter.gps_work.Fti.interval_acc_on, G_parameter.gps_work.Fti.interval_acc_off);
#endif
#endif

    if (G_parameter.gps_work.Fd.sw)
        n += sprintf(tmp + n, "DISTANCE:%d;", G_parameter.gps_work.Fd.distance);

    /* SOS */
    if (3 == TRACK_DEFINE_SOS_COUNT)
    {
        n += sprintf(tmp + n, "SOS NUM:%s,%s,%s;", G_parameter.sos_num[0], G_parameter.sos_num[1], G_parameter.sos_num[2]);
    }
    if (4 == TRACK_DEFINE_SOS_COUNT)
    {
        n += sprintf(tmp + n, "SOS NUM:%s,%s,%s,%s;", G_parameter.sos_num[0], G_parameter.sos_num[1], G_parameter.sos_num[2], G_parameter.sos_num[3]);
    }

#if defined (__NT33__)

#else
    /* 中心号码 */
#if !defined(__GT420D__)
    n += sprintf(tmp + n, "CENTER:%s;", G_parameter.centerNumber);
#endif
#endif /* __NT33__ */

    /* 亲情号码 */
    //n += sprintf(tmp+n, "FN NUM:");

    /* iccid */
    n += sprintf(tmp + n, "ICCID:%s;", (char *)OTA_ell_get_iccid());

    /*imsi*/
    n += sprintf(tmp + n, "IMSI:%s;", (char *)track_drv_get_imsi(0));

    /* eurl */
#if !defined(__GT420D__)
    n += sprintf(tmp + n, "EURL:%s;", G_parameter.url);
#endif

#if defined(__NT33__)||defined(__NT37__)
    n += sprintf(tmp + n, "BAT TEMP:%d \r\n", (kal_uint32)charge_measure.bmt_vTemp / 1000);
#elif defined(__GT800__)||defined(__NT31__)&&!defined(__GT02__)
    n += sprintf(tmp + n, "BAT TEMP:%d \r\n", (kal_uint32)charge_measure.bmt_vTemp / 1000);
#elif defined(__GT740__)||defined(__GT420D__)
#if !defined(__GT420D__)
    n += sprintf(tmp + n, "STSN %d;", get_st_sn(-1));
    get_st_time(&ap);
    n += sprintf(tmp + n, "STTIME %d:%d:%d:%d:%d;", ap.nHour, ap.nMin, ap.nYear, ap.nMonth, ap.nDay);
    n += sprintf(tmp + n, "STWAKE %d;", track_cust_modeule_get_wakemode());
    n += sprintf(tmp + n, "STELE %d;", track_cust_modeule_get_st_ele());
    n += sprintf(tmp + n, "RST %d;", track_cust_modeule_get_rst());
#endif
    {
        char m_buf[100]={0};
#if defined(__GT740__)
        if (G_realtime_data.tracking_mode)
        {
            sprintf(m_buf, "MODE=MODE,2,%0.2d:%0.2d,%d(MIN)",G_parameter.extchip.mode2_par1 / 60, G_parameter.extchip.mode2_par1 % 60, G_parameter.teardown_str.alarm_vel_min);
        }
        else if (G_parameter.extchip.mode1_en)
        {
            sprintf(m_buf, "MODE=MODE,1,%d,%d",G_parameter.gps_work.Fti.interval_acc_off, G_parameter.extchip.mode1_par2);
        }
        else if (G_parameter.extchip.mode == 1)
        {
            sprintf(m_buf, "MODE=MODE,%d,%d,%d", G_parameter.extchip.mode, G_parameter.gps_work.Fti.interval_acc_off, G_parameter.extchip.mode1_par2);
        }
        else if (G_parameter.extchip.mode == 2)
        {
            if (G_parameter.extchip.mode2_par2 % 60 == 0)
            {
                sprintf(m_buf, "MODE=MODE,%d,%0.2d:%0.2d,%d", G_parameter.extchip.mode, G_parameter.extchip.mode2_par1 / 60, G_parameter.extchip.mode2_par1 % 60, G_parameter.extchip.mode2_par2 / 60);
            }
            else
            {
                sprintf(m_buf, "MODE=MODE,%d,%0.2d:%0.2d,%d(MIN)", G_parameter.extchip.mode, G_parameter.extchip.mode2_par1 / 60, G_parameter.extchip.mode2_par1 % 60, G_parameter.extchip.mode2_par2);
            }
        }
        else if (G_parameter.extchip.mode == 3)
        {
            sprintf(m_buf, "MODE=MODE,%d,%d,%0.2d:%0.2d", G_parameter.extchip.mode, G_parameter.extchip.mode3_par1,
                    G_parameter.extchip.mode3_par2 / 60, G_parameter.extchip.mode3_par2 % 60);
        }
        else if (G_parameter.extchip.mode == 4)
        {
            U8 vle = G_parameter.extchip.mode4_par1, i = 0;
            char m_buf1[100] = {0};
            for (; i < vle; i++)
            {
                if (0 == i)
                {
                    snprintf(m_buf1 + strlen(m_buf1), 99, "%0.2d:%0.2d", G_parameter.extchip.mode4_par2[i] / 60, G_parameter.extchip.mode4_par2[i] % 60);
                }
                else
                {
                    snprintf(m_buf1 + strlen(m_buf1), 99, ",%0.2d:%0.2d", G_parameter.extchip.mode4_par2[i] / 60, G_parameter.extchip.mode4_par2[i] % 60);
                }
            }
            snprintf(m_buf, 99, "MODE=MODE,%d,%d,%s", G_parameter.extchip.mode, G_parameter.extchip.mode4_par1, m_buf1);
        }
#elif defined(__GT420D__)
    if (G_parameter.extchip.mode == 1)
    {
        if(track_cust_gt420d_carstatus_data(99) == 2)
            {
                sprintf(m_buf, "MODE:%d,%d", G_parameter.extchip.mode, G_parameter.statictime.mode1_statictime);
                }
            else
            {
                sprintf(m_buf, "MODE:%d,%d", G_parameter.extchip.mode, G_parameter.extchip.mode1_par1);
            }
    }
    else if (G_parameter.extchip.mode == 2)
    {
        if(track_cust_gt420d_carstatus_data(99) == 2)
            {
                sprintf(m_buf, "MODE:%d,%d", G_parameter.extchip.mode, G_parameter.statictime.mode2_statictime);
                }
            else
            {
                sprintf(m_buf, "MODE:%d,%d", G_parameter.extchip.mode, G_parameter.extchip.mode2_par1);
            }
    }
#endif
        n += snprintf(tmp + n,sizeof(tmp), "%s \r\n", m_buf);

    }
#endif

    if (n > 0)
    {
        if (cmd->return_sms.cm_type == CM_SMS)
        {
            if (n > 320)
            {
                track_cust_sms_send(cmd->return_sms.num_type, cmd->return_sms.ph_num, KAL_TRUE, tmp, 320);
                track_cust_sms_send(cmd->return_sms.num_type, cmd->return_sms.ph_num, KAL_TRUE, tmp + 320, n - 320);

            }
            else
                track_cust_sms_send(cmd->return_sms.num_type, cmd->return_sms.ph_num, KAL_TRUE, tmp, n);
        }
        else if (cmd->return_sms.cm_type == CM_ONLINE)
        {
            track_cust_paket_msg_upload(cmd->return_sms.stamp, KAL_TRUE, tmp, n);
        }
        else if (cmd->return_sms.cm_type == CM_ONLINE2)
        {
            track_cust_paket_FD_cmd(cmd->return_sms.stamp, KAL_TRUE, tmp, n);
        }
    }
    LOGS("%s", tmp);
    if (cmd->return_sms.cm_type == CMD_NET2)
        track_cust_net2_param(strlen(tmp), tmp);
    memset(cmd->rsp_msg, 0, 320);
}

static void cmd_status(CMD_DATA_STRUCT *cmd)
{
#if defined(__GT420D__)
    kal_uint32 value1;
    float value;
    value1 = track_cust_MCUrefer_adc_data(99);
    LOGD(L_APP,L_V5,"value1:%d",value1);
    value =((float)value1)/100;
#endif
    if (cmd->part == 1 && !strcmp(cmd->pars[1], "BD"))
    {
        track_gps_data_struct *gd = track_drv_get_gps_data();
        char GPS_status[40] = {0};
        char GPS_PRN[80] = {0};
        char PRN[5] = {0};
        kal_uint8 tmp, i, j, x, sat = 0;
        kal_bool head = KAL_TRUE;
        //BD
        tmp = track_cust_gps_status();
        if (tmp == GPS_STAUS_ON)
            snprintf(GPS_status, 40, "ON, Uart no data");
        else if (tmp == GPS_STAUS_SCAN)
            snprintf(GPS_status, 40, "Searching satellite");
        else if (tmp >= GPS_STAUS_2D)
            snprintf(GPS_status, 40, "Successful positioning");
        else
            snprintf(GPS_status, 40, "OFF");

        if (tmp != 0)
        {
            for (i = 0, j = 80; i < gd->bdgga.sat; i++)
            {
                for (x = 0; x < MTK_GPS_SV_MAX_NUM; x++)
                {
                    if (gd->bdgsv.Satellite_ID[x] == gd->bdgsa.satellite_used[i])
                    {
                        if (head)
                            snprintf(PRN, 5, "%d", gd->bdgsv.SNR[x]);
                        else
                            snprintf(PRN, 5, ",%d", gd->bdgsv.SNR[x]);
                        j -= 5;
                        strncat(GPS_PRN, PRN, j);
                        head = KAL_FALSE;
                        break;
                    }
                    else if (x == MTK_GPS_SV_MAX_NUM - 1)
                    {
                        LOGD(L_APP, L_V5, "not find:%d", gd->bdgsa.satellite_used[i]);
                    }
                }
            }
            sat = gd->bdgga.sat;
        }

        if (tmp != GPS_STAUS_OFF)
        {
            snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "BD:%s, SVS Used in fix:%d(%d), BD Signal Level:%s;",
                     GPS_status, sat, gd->bdgsv.Satellites_In_View, GPS_PRN);
        }
        else
        {
            snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "BD:%s", GPS_status);
        }

    }
    else if (cmd->part == 0)
    {
        kal_uint8 tmp, i, j, x, sat = 0;
        track_gps_data_struct *gd = track_drv_get_gps_data();
#if defined (__NT33__)
        char Battery[128] = {0};
        char Battery_ext[60] = {0};
#else
        char Battery[60] = {0};
#endif
        char GPRS[40] = {0};
        char GSMSignalLevel[40] = {0};
        char GPS_status[40] = {0};
        char GPS_PRN[100] = {0};
        char GPS_Use[30] = {0};
        char PRN[5] = {0};
        kal_bool head = KAL_TRUE;
        char ACC[10] = {0};
        char Defense[16] = {0};
        float volt = 0;
        char power_saving_mode[20] = {0};
        char CSQ[20] = 0;

        //电池等级
        volt = track_cust_get_battery_volt();

#if defined (__GT02__)  //最简版
        if (volt == 0)
        {
            sprintf(Battery, "Battery:(Checking);");
        }
        else
        {
            volt = volt / 1000000;
            tmp = track_cust_get_battery_level();
            if (tmp > 2)
            {
                sprintf(Battery, "Battery:NORMAL;");
            }
            else if (tmp == 2)
            {
                sprintf(Battery, "Battery:LOW;");
            }
            else
            {
                sprintf(Battery, "Battery:Battery too low! Warning;");
            }
        }
#elif defined (__NT33__)    //带电压、温度、充电状态
        if (volt == 0)
        {
            sprintf(Battery, "; Battery:(Checking)");
        }
        else
        {
            kal_int8 bat_temperature;
            kal_uint8 chargerStatus;
            charge_measure_struct charge_measure = {0};

            volt = volt / 1000000;
            tmp = track_cust_get_battery_level();

            chargerStatus = track_cust_status_charger();
            track_bmt_get_ADC_result(&charge_measure);
            bat_temperature = (kal_int8)((kal_uint16)charge_measure.bmt_vTemp / 1000);

            if (chargerStatus == 0)
            {
                sprintf(Battery_ext, "charge out;");
            }
            else
            {
                sprintf(Battery_ext, "charge in,Temp:%d'C;", bat_temperature);
            }


            if (tmp > 2)
            {
                sprintf(Battery, "Battery:%.2fV,NORMAL;", volt);
            }
            else if (tmp == 2)
            {
                sprintf(Battery, "Battery:%.2fV,LOW;", volt);
            }
            else
            {
                sprintf(Battery, "Battery:%.2fV,Battery too low! Warning;", volt);
            }
            strcat(Battery, Battery_ext);

        }
#elif defined(__GT420D__)
        if (value == 0)
        {
            sprintf(Battery, "; Battery:(Checking)");
        }
        else
        {
            tmp = track_cust_get_battery_level();
            if (tmp > 2)
            {
                sprintf(Battery, "Battery:%.2fV,NORMAL;", value);
            }
            else if (tmp == 2)
            {
                sprintf(Battery, "Battery:%.2fV,LOW;", value);
            }
            else
            {
                sprintf(Battery, "Battery:%.2fV,Battery too low! Warning;", value);
            }
        }
#else   //带电压
        if (volt == 0)
        {
            sprintf(Battery, "; Battery:(Checking)");
        }
        else
        {
            volt = volt / 1000000;
            tmp = track_cust_get_battery_level();
            if (tmp > 2)
            {
                sprintf(Battery, "Battery:%.2fV,NORMAL;", volt);
            }
            else if (tmp == 2)
            {
                sprintf(Battery, "Battery:%.2fV,LOW;", volt);
            }
            else
            {
                sprintf(Battery, "Battery:%.2fV,Battery too low! Warning;", volt);
            }
        }
#endif

        //GPRS连接状态
        #if defined(__MQTT__) 
        if(MQTT_get_gprs_status())
        #else
        if (track_soc_login_status())
        #endif
            snprintf(GPRS, 40, "GPRS:Link Up;");
        else
            snprintf(GPRS, 40, "GPRS:Link Down;");
#if defined(__XCWS_TPATH__)
        if (track_soc_server3_login_status())
            strcat(GPRS, ",S3 Link Up;");
        else
            strcat(GPRS, ",S3 Link Dow;");
#endif

        //GSM信号
        switch (track_cust_get_rssi_level())
        {
            case -1:
                snprintf(GSMSignalLevel, 35, "GSM Signal Level:Invaild;");
                break;
            case 0:
                snprintf(GSMSignalLevel, 35, "GSM Signal Level:No Singal;");
                break;
            case 1:
                snprintf(GSMSignalLevel, 35, "GSM Signal Level:Weak;");
                break;
            case 2:
                snprintf(GSMSignalLevel, 35, "GSM Signal Level:Low;");
                break;
            case 3:
                snprintf(GSMSignalLevel, 35, "GSM Signal Level:Good;");
                break;
            case 4:
                snprintf(GSMSignalLevel, 35, "GSM Signal Level:Strong;");
                break;
            default:
                snprintf(GSMSignalLevel, 35, "GSM Signal Level:Invaild;");
                break;
        }

        //GPS
        tmp = track_cust_gps_status();
        LOGD(L_APP,L_V5,"TMP:%d",tmp);
        if (tmp == GPS_STAUS_ON)
            snprintf(GPS_status, 40, "GPS:ON, Uart no data;");
        else if (tmp == GPS_STAUS_SCAN)
            snprintf(GPS_status, 40, "GPS:Searching satellite;");
        else if (tmp >= GPS_STAUS_2D)
            snprintf(GPS_status, 40, "GPS:Successful positioning;");
        else
            snprintf(GPS_status, 40, "GPS:OFF;");

        if (tmp != 0)
        {
            snprintf(GPS_PRN, 40, ", GPS Signal Level:");
            for (i = 0, j = 80; i < gd->gpgga.sat && i < MTK_GPS_SV_MAX_NUM; i++)
            {
                for (x = 0; x < MTK_GPS_SV_MAX_NUM; x++)
                {
                    if (gd->gpgsv.Satellite_ID[x] == gd->gpgsa.satellite_used[i])
                    {
                        if (head)
                        {
                            snprintf(PRN, 5, "%d", gd->gpgsv.SNR[x]);
                        }
                        else
                        {
                            snprintf(PRN, 5, ",%d", gd->gpgsv.SNR[x]);
                        }
                        j -= 5;
                        strncat(GPS_PRN, PRN, j);
                        head = KAL_FALSE;
                        break;
                    }
                    else if (x == MTK_GPS_SV_MAX_NUM - 1)
                    {
                        LOGD(L_APP, L_V5, "not find:%d", gd->gpgsa.satellite_used[i]);
                    }
                }
            }
            sat = gd->gpgga.sat;
            LOGD(L_APP, L_V4, "sat:%d, j:%d, %d(%s)", gd->bdgga.sat, j, strlen(GPS_PRN), GPS_PRN);
            for (i = 0; i < gd->bdgga.sat && i < MTK_GPS_SV_MAX_NUM; i++)
            {
                for (x = 0; x < MTK_GPS_SV_MAX_NUM; x++)
                {
                    if (gd->bdgsv.Satellite_ID[x] != 0 && gd->bdgsv.Satellite_ID[x] == gd->bdgsa.satellite_used[i])
                    {
                        LOGD(L_APP, L_V4, "Satellite_ID:%d, satellite_used:%d", gd->bdgsv.Satellite_ID[x], gd->bdgsa.satellite_used[i]);
                        if (head)
                            snprintf(PRN, 5, "%d", gd->bdgsv.SNR[x]);
                        else
                            snprintf(PRN, 5, ",%d", gd->bdgsv.SNR[x]);
                        j -= 5;
                        strncat(GPS_PRN, PRN, j);
                        head = KAL_FALSE;
                        break;
                    }
                    else if (x == MTK_GPS_SV_MAX_NUM - 1)
                    {
                        LOGD(L_APP, L_V5, "not find:%d;", gd->bdgsa.satellite_used[i]);
                    }
                }
            }
            sat += gd->bdgga.sat;
            sprintf(GPS_Use, "SVS Used in fix:%d(%d);", sat, gd->gpgsv.Satellites_In_View + gd->bdgsv.Satellites_In_View);
        }
#if !defined(__NO_ACC__)
        //ACC
        if (track_cust_status_acc() == 1)
            snprintf(ACC, 10, "ACC:ON;");
        else
            snprintf(ACC, 10, "ACC:OFF;");
#endif /* __NO_ACC__ */

        //Defense
#if !defined(__NO_DEFENSE__)//无设防只需添加此功能宏
        if (track_cust_status_defences() == 2)
            snprintf(Defense, 15, "Defense:ON;");
        else
            snprintf(Defense, 15, "Defense:OFF;");
#endif /* __NO_DEFENSE__ */


        /*
                if(tmp != GPS_STAUS_OFF)
                {
                    snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "Battery:%s; GPRS:%s; GSM Signal Level:%s; GPS:%s, SVS Used in fix:%d(%d), GPS Signal Level:%s; ",
                             Battery, GPRS, GSMSignalLevel, GPS_status, sat, gd->gpgsv.Satellites_In_View + gd->bdgsv.Satellites_In_View, GPS_PRN);
                }
                else
                {
                    snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "Battery:%s; GPRS:%s; GSM Signal Level:%s; GPS:%s",
                             Battery, GPRS, GSMSignalLevel, GPS_status);
                }
                return;
        */
#if defined(__GT100__)
        if (G_realtime_data.power_saving_mode == 1)
        {
            snprintf(power_saving_mode, 20, "Sleep Mode:ON;");
        }
        else
        {
            snprintf(power_saving_mode, 20, "Sleep Mode:OFF;");
        }
#endif /* __GT100__ */
#if defined(__GT420D__)
        sprintf(CSQ, "CSQ :%d;", track_cust_get_rssi(-1));
#endif
        snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "%s %s %s %s %s %s %s %s %s %s",
                 Battery, GPRS, GSMSignalLevel, GPS_status, GPS_Use, GPS_PRN, ACC, Defense, power_saving_mode, CSQ);
        //个别空字符串前后空白，导致结果难看 chengj
    }
}

static void cmd_param(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        //  IMEI:353419032348877;           --- 设备IMEI号
        //  TIMER:10,10;            --- GPS数据上传间隔为10秒。GPRS上传时间间隔（GPRS Interval）
        //  Distance:500;                   --- 定距上传，间隔距离，单位米
        //  SENDS:5;                        --- ACC OFF情况下震动触发GPS工作的时间，设置范围：1-300分钟,默认：300分钟。
        //  SOS:15942703401;                --- SOS号码，最大可设置3个SOS号码，用于报警及监听。
        //  Center Number:15942703401;      --- 服务中心号码，用于断油电和恢复油电，仅能设置一个服务中心号码。
        //  Sensorset:10,3,1,180;           --- 在10秒内检测3次震动; 震动报警延时180秒
        //  Defense time:10;                --- 设防延时为10分钟。
        //  TimeZone:E,8,0;                 --- 时区为东八区。时区（TimeZone）
        char tmp[50] = {0}, tmp2[50] = {0}, num_tmp[TRACK_DEFINE_PHONE_NUMBER_LEN_MAX * TRACK_DEFINE_SOS_COUNT + 3] = {0};
        char center[40] = {0}, defense[40] = {0};
        int ret, j, num_len = 0;
        char *p = num_tmp;
        if (G_parameter.gps_work.Fti.sw == 1)
        {
#if defined(__GPS_INTERMITTENT_START__)

#if defined(__GT370__)|| defined (__GT380__)
            snprintf(tmp, 50, "TIMER:%d; ", G_parameter.gps_work.Fti.lbs_interval);
#elif defined(__GT300S__)
            snprintf(tmp, 50, "TIMER:%d; ", G_parameter.gps_work.Fti.lbs_interval);
#else
            snprintf(tmp, 50, "TIMER:%d,%d,%d; ", G_parameter.gps_work.Fti.lbs_interval, G_parameter.gps_work.Fti.interval_acc_on, G_parameter.gps_work.Fti.mode1_gps_time);
#endif /* __GT300S__ */
#elif defined(__NT33__)

#if defined (__FUN_WITH_ACC__)
            snprintf(tmp, 50, "TIMER:%d,%d; ", G_parameter.gps_work.Fti.interval_acc_on, G_parameter.gps_work.Fti.interval_acc_off);
#else
            snprintf(tmp, 50, "TIMER:%d,%d; ", G_parameter.gps_work.Fti.interval_acc_off, G_parameter.gps_work.Fti.lbs_interval);
#endif /* __FUN_WITH_ACC__ */

#elif defined(__NO_ACC__)
            snprintf(tmp, 50, "TIMER:%d,%d; ", G_parameter.gps_work.Fti.lbs_interval, G_parameter.gps_work.Fti.interval_acc_off);
#else
            snprintf(tmp, 50, "TIMER:%d,%d; ", G_parameter.gps_work.Fti.interval_acc_on, G_parameter.gps_work.Fti.interval_acc_off);
#endif /* __GPS_INTERMITTENT_START__ */
        }
#if !defined(__GW100__)
        if (G_parameter.gps_work.Fd.sw == 1)
        {
            snprintf(tmp2, 50, "Distance:%d; ", G_parameter.gps_work.Fd.distance);
        }
#endif
        if (g_GT03D == KAL_FALSE)
        {
#if !(defined(__GT500__) || defined(__GT300__)||defined(__GT300S__) || defined(__GT370__) || defined (__GT380__)|| defined (__NT33__))
#if defined(__GERMANY__)
            snprintf(center, 40, "Center Nummer:%s; ", G_parameter.centerNumber);

#else
            snprintf(center, 40, "Center Number:%s; ", G_parameter.centerNumber);
#endif/*__GERMANY__*/
#endif

#if defined(__GT740__) || defined(__GT370__)|| defined (__GT380__)||defined(__GT420D__)
            //不需要Defense time
#elif defined(__NT36__)||defined(__NT31__)||defined (__NT33__)
            snprintf(defense, 40, "Defense time:%d; ", G_parameter.defense_time / 60);
#elif defined(__ET310__)
            snprintf(defense, 40, "Defense time:%dsec; ", G_parameter.defense_time);
#else
            snprintf(defense, 40, "Defense time:%d; ", G_parameter.defense_time);
#endif
        }

        for (j = 0; j < TRACK_DEFINE_SOS_COUNT; j++)
        {
            if (j == 0)
            {
                num_len = TRACK_DEFINE_PHONE_NUMBER_LEN_MAX * TRACK_DEFINE_SOS_COUNT + 3;
                ret = snprintf(p, num_len, "%s", G_parameter.sos_num[j]);
            }
            else
            {
                ret = snprintf(p, num_len, ",%s", G_parameter.sos_num[j]);
            }
            p += ret;
        }
#if defined(__GW100__)
        snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "IMEI:%s; %s%s; SOS:%s; TimeZone:%c,%d,%d;",
                 track_drv_get_imei(0), tmp, tmp2,
                 num_tmp,
                 G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min);
#elif defined (__GT420D__)
        if(G_parameter.extchip.mode == 1)
       {
        if(track_cust_gt420d_carstatus_data(99) == 2)
            {
                snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "IMEI:%s; MODE:%d,%d; SOS:%s; %s;TimeZone:%c,%d,%d;",
                 track_drv_get_imei(0), G_parameter.extchip.mode,G_parameter.statictime.mode1_statictime,num_tmp,center,
                 G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min);
                }
            else
            {
                snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "IMEI:%s; MODE:%d,%d; SOS:%s; %s;TimeZone:%c,%d,%d;",
                 track_drv_get_imei(0), G_parameter.extchip.mode,G_parameter.extchip.mode1_par1,num_tmp,center,
                 G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min);
            }
       }
       else if(G_parameter.extchip.mode == 2)
       {
        if(track_cust_gt420d_carstatus_data(99) == 2)
            {
                snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "IMEI:%s; MODE:%d,%d; SOS:%s; %s;TimeZone:%c,%d,%d;",
                 track_drv_get_imei(0), G_parameter.extchip.mode,G_parameter.statictime.mode2_statictime,num_tmp,center,
                 G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min);
                }
            else
            {
                snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "IMEI:%s; MODE:%d,%d; SOS:%s; %s;TimeZone:%c,%d,%d;",
                 track_drv_get_imei(0), G_parameter.extchip.mode,G_parameter.extchip.mode2_par1,num_tmp,center,
                 G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min);
            }
       }
       else
       {
        snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "IMEI:%s; MODE:%d; SOS:%s; %s;TimeZone:%c,%d,%d;",
                 track_drv_get_imei(0), G_parameter.extchip.mode,num_tmp,center,
                 G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min);}
#else
        snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "IMEI:%s; %s%sSENDS:%d; SOS:%s; %sSensorset:%d,%d,%d,%d; %sTimeZone:%c,%d,%d;",
                 track_drv_get_imei(0), tmp, tmp2,
                 G_parameter.sensor_gps_work, num_tmp,
                 center,
                 G_parameter.vibrates_alarm.detection_time, G_parameter.gps_work.vibrates.detection_count, G_parameter.vibrates_alarm.detection_count, G_parameter.vibrates_alarm.sampling_interval,
                 defense, G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min);
#endif /* __GW100__ */
    }
}

static void cmd_scxsz(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        /*
        网络名,网络ID,运营商代理IP,用户名,密码;
        acc on GPS上传时间间隔,acc off GPS上传时间间隔 / 定距上传的最短距离;
        检到一次Sensor触发开启GPS工作的时长;
        时区,半时区;
        模式（1 域名，0 IP）,后台域名或IP,端口号,协议（0 TCP，1 UPD）;
        SOS号码1,SOS号码2,SOS号码3;
        中心号码;
        震动传感器检测时间;
        设防时间;
        语言（1中文，0英文）;*/
        char server[100] = {0};
        char tmp[50] = {0}, tmp2[50] = {0}, num_tmp[TRACK_DEFINE_PHONE_NUMBER_LEN_MAX * TRACK_DEFINE_SOS_COUNT + 3] = {0};
        int ret, j, num_len = 0;
        char *p = num_tmp;
        nvram_GPRS_APN_struct *use_apn = track_socket_get_use_apn();
        if (track_nvram_alone_parameter_read()->server.conecttype == 0)
            snprintf(server, 100, "%d.%d.%d.%d", track_nvram_alone_parameter_read()->server.server_ip[0], track_nvram_alone_parameter_read()->server.server_ip[1], track_nvram_alone_parameter_read()->server.server_ip[2], track_nvram_alone_parameter_read()->server.server_ip[3]);
        else snprintf(server, 100, "%s", track_domain_decode(track_nvram_alone_parameter_read()->server.url));

        for (j = 0; j < TRACK_DEFINE_SOS_COUNT; j++)
        {
            if (j == 0)
            {
                num_len = TRACK_DEFINE_PHONE_NUMBER_LEN_MAX * TRACK_DEFINE_SOS_COUNT + 3;
                ret = snprintf(p, num_len, "%s", G_parameter.sos_num[j]);
            }
            else
            {
                ret = snprintf(p, num_len, ",%s", G_parameter.sos_num[j]);
            }
            p += ret;
        }
        if (G_parameter.gps_work.Fti.sw == 1)
        {
#if defined(__GW100__)
            snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX,
                     "%s,%s,%s;%d,%d,%d;%c%d,%d;%d,%s,%d,%d;%s;%d;",
                     use_apn->apn, use_apn->username, use_apn->password,
                     G_parameter.gps_work.Fti.lbs_interval, G_parameter.gps_work.Fti.interval_acc_on, G_parameter.gps_work.Fti.mode1_gps_time,
                     G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min,
                     track_nvram_alone_parameter_read()->server.conecttype, server, track_nvram_alone_parameter_read()->server.server_port, track_nvram_alone_parameter_read()->server.soc_type,
                     num_tmp,
                     G_parameter.lang);
#elif defined(__GT300__) || defined(__GT03D__)||defined(__GT300S__)
            snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX,
                     "%s,%s,%s;%d,%d,%d;%d;%c%d,%d;%d,%s,%d,%d;%s;%d;%d;",
                     use_apn->apn, use_apn->username, use_apn->password,
                     G_parameter.gps_work.Fti.lbs_interval, G_parameter.gps_work.Fti.interval_acc_on, G_parameter.gps_work.Fti.mode1_gps_time,
                     G_parameter.sensor_gps_work,
                     G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min,
                     track_nvram_alone_parameter_read()->server.conecttype, server, track_nvram_alone_parameter_read()->server.server_port, track_nvram_alone_parameter_read()->server.soc_type,
                     num_tmp,
                     G_parameter.vibrates_alarm.detection_time,
                     G_parameter.lang);
#elif defined(__GT500__)
            snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX,
                     "%s,%s,%s;%d,%d;%d;%c%d,%d;%d,%s,%d,%d;%s;%d;%d;%d;",
                     use_apn->apn, use_apn->username, use_apn->password,
                     G_parameter.gps_work.Fti.interval_acc_on, G_parameter.gps_work.Fti.interval_acc_off,
                     G_parameter.sensor_gps_work,
                     G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min,
                     track_nvram_alone_parameter_read()->server.conecttype, server, track_nvram_alone_parameter_read()->server.server_port, track_nvram_alone_parameter_read()->server.soc_type,
                     num_tmp,
                     G_parameter.vibrates_alarm.detection_time,
                     G_parameter.defense_time,
                     G_parameter.lang);
#elif defined(__GT420D__)
            snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX,
                     "%s,%s,%s;%c%d,%d;%d,%s,%d,%d;%s;%d;",
                     use_apn->apn, use_apn->username, use_apn->password,
                     G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min,
                     track_nvram_alone_parameter_read()->server.conecttype, server, track_nvram_alone_parameter_read()->server.server_port, track_nvram_alone_parameter_read()->server.soc_type,
                     num_tmp,
                     G_parameter.lang);
#else
            snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX,
                     "%s,%s,%s;%d,%d;%d;%c%d,%d;%d,%s,%d,%d;%s;%s;%d;%d;%d;",
                     use_apn->apn, use_apn->username, use_apn->password,
                     G_parameter.gps_work.Fti.interval_acc_on, G_parameter.gps_work.Fti.interval_acc_off,
                     G_parameter.sensor_gps_work,
                     G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min,
                     track_nvram_alone_parameter_read()->server.conecttype, server, track_nvram_alone_parameter_read()->server.server_port, track_nvram_alone_parameter_read()->server.soc_type,
                     num_tmp,
                     G_parameter.centerNumber,
                     G_parameter.vibrates_alarm.detection_time,
                     G_parameter.defense_time / 60,
                     G_parameter.lang);
#endif
        }
        else
        {
#if defined(__GW100__)
            snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX,
                     "%s,%s,%s;%d;%c%d,%d;%d,%s,%d,%d;%s;%d;",
                     use_apn->apn, use_apn->username, use_apn->password,
                     G_parameter.gps_work.Fd.distance,
                     G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min,
                     track_nvram_alone_parameter_read()->server.conecttype, server, track_nvram_alone_parameter_read()->server.server_port, track_nvram_alone_parameter_read()->server.soc_type,
                     num_tmp,
                     G_parameter.lang);
#elif defined(__GT500__)
            snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX,
                     "%s,%s,%s;%d;%d;%c%d,%d;%d,%s,%d,%d;%s;%d;%d;%d;",
                     use_apn->apn, use_apn->username, use_apn->password,
                     G_parameter.gps_work.Fd.distance,
                     G_parameter.sensor_gps_work,
                     G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min,
                     track_nvram_alone_parameter_read()->server.conecttype, server, track_nvram_alone_parameter_read()->server.server_port, track_nvram_alone_parameter_read()->server.soc_type,
                     num_tmp,
                     G_parameter.vibrates_alarm.detection_time,
                     G_parameter.defense_time,
                     G_parameter.lang);
#elif defined(__GT300__) ||defined(__GT03D__)||defined(__GT300S__)
            snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX,
                     "%s,%s,%s;%d;%d;%c%d,%d;%d,%s,%d,%d;%s;%d;%d;",
                     use_apn->apn, use_apn->username, use_apn->password,
                     G_parameter.gps_work.Fd.distance,
                     G_parameter.sensor_gps_work,
                     G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min,
                     track_nvram_alone_parameter_read()->server.conecttype, server, track_nvram_alone_parameter_read()->server.server_port, track_nvram_alone_parameter_read()->server.soc_type,
                     num_tmp,
                     G_parameter.vibrates_alarm.detection_time,
                     G_parameter.lang);
#else
            snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX,
                     "%s,%s,%s;%d;%d;%c%d,%d;%d,%s,%d,%d;%s;%s;%d;%d;%d;",
                     use_apn->apn, use_apn->username, use_apn->password,
                     G_parameter.gps_work.Fd.distance,
                     G_parameter.sensor_gps_work,
                     G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min,
                     track_nvram_alone_parameter_read()->server.conecttype, server, track_nvram_alone_parameter_read()->server.server_port, track_nvram_alone_parameter_read()->server.soc_type,
                     num_tmp,
                     G_parameter.centerNumber,
                     G_parameter.vibrates_alarm.detection_time,
                     G_parameter.defense_time,
                     G_parameter.lang);
#endif
        }
    }
}

static void cmd_gprsset(CMD_DATA_STRUCT *cmd)
{
    char GPRS[5] = {0};
    char server[99] = {0};
    nvram_GPRS_APN_struct *use_apn = track_socket_get_use_apn();
    //GPRS连接状态
    if (track_nvram_alone_parameter_read()->gprson)
        snprintf(GPRS, 20, "ON");
    else
        snprintf(GPRS, 20, "OFF");
#ifdef __GT02__
    if (G_importance_parameter_data.dserver.server_type && track_get_server_ip_status() == 0)
    {
        if (G_importance_parameter_data.dserver.conecttype == 0)
        {
            //ip
            snprintf(server, 20, "%d.%d.%d.%d", G_importance_parameter_data.dserver.server_ip[0], G_importance_parameter_data.dserver.server_ip[1], G_importance_parameter_data.dserver.server_ip[2], G_importance_parameter_data.dserver.server_ip[3]);
        }
        else
        {
            //Domain name
            snprintf(server, 98, "%s", track_domain_decode(G_importance_parameter_data.dserver.url));
        }

        snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "GPRS:%s; Currently use APN:%s,%s,%s; DServer:%d,%s,%d,%d; URL:%s;",
                 GPRS,
                 use_apn->apn,
                 use_apn->username,
                 use_apn->password,
                 G_importance_parameter_data.dserver.conecttype,
                 server,
                 G_importance_parameter_data.dserver.server_port,
                 G_importance_parameter_data.dserver.soc_type,
                 G_parameter.url);
    }
    else
    {
        if (track_nvram_alone_parameter_read()->server.conecttype == 0)
        {
            //ip
            snprintf(server, 20, "%d.%d.%d.%d", track_nvram_alone_parameter_read()->server.server_ip[0], track_nvram_alone_parameter_read()->server.server_ip[1], track_nvram_alone_parameter_read()->server.server_ip[2], track_nvram_alone_parameter_read()->server.server_ip[3]);
        }
        else
        {
            //Domain name
            snprintf(server, 98, "%s", track_domain_decode(track_nvram_alone_parameter_read()->server.url));
        }

        snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "GPRS:%s; Currently use APN:%s,%s,%s; Server:%d,%s,%d,%d; URL:%s;",
                 GPRS,
                 use_apn->apn,
                 use_apn->username,
                 use_apn->password,
                 track_nvram_alone_parameter_read()->server.conecttype,
                 server,
                 track_nvram_alone_parameter_read()->server.server_port,
                 track_nvram_alone_parameter_read()->server.soc_type,
                 G_parameter.url);
    }
#else
    if (track_nvram_alone_parameter_read()->server.conecttype == 0)
    {
        //ip
        snprintf(server, 20, "%d.%d.%d.%d", track_nvram_alone_parameter_read()->server.server_ip[0], track_nvram_alone_parameter_read()->server.server_ip[1], track_nvram_alone_parameter_read()->server.server_ip[2], track_nvram_alone_parameter_read()->server.server_ip[3]);
    }
    else
    {
        //Domain name
        snprintf(server, 98, "%s", track_domain_decode(track_nvram_alone_parameter_read()->server.url));
    }

#if defined(__GERMANY__)
    snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "GPRS:%s; Eingestellter APN:%s,%s,%s; Server:%d,%s,%d,%d; URL:%s;",
             GPRS,
             use_apn->apn,
             use_apn->username,
             use_apn->password,
             track_nvram_alone_parameter_read()->server.conecttype,
             server,
             track_nvram_alone_parameter_read()->server.server_port,
             track_nvram_alone_parameter_read()->server.soc_type,
             G_parameter.url);

#else
    snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "GPRS:%s; Currently use APN:%s,%s,%s; Server:%d,%s,%d,%d; URL:%s;",
             GPRS,
             use_apn->apn,
             use_apn->username,
             use_apn->password,
             track_nvram_alone_parameter_read()->server.conecttype,
             server,
             track_nvram_alone_parameter_read()->server.server_port,
             track_nvram_alone_parameter_read()->server.soc_type,
             G_parameter.url);
#endif/*__GERMANY__*/

#endif
}


/******************************************************************************
 *  Function    -  cmd_sos
 *
 *  Purpose     -  SOS设置
 *
 *  Description -
        SOS,<A>,<号码 1>[,号码 2][,号码 3]#
            添加SOS号码
        SOS,<D>,<号码序号 1>[,号码序号 2][,号码序号 3]#
            根据序号，删除对应的SOS号码
        SOS,<D>,<电话号码>#
            根据号码，全匹配，删除对应的SOS号码
        SOS#
            查询设置的SOS号码
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * v1.1  , 11-10-2013,  WangQi modify TRACK_DEFINE_SOS_COUNT
 * ----------------------------------------
*******************************************************************************/
static void cmd_sos(CMD_DATA_STRUCT *cmd)
{
    char num[TRACK_DEFINE_SOS_COUNT][TRACK_DEFINE_PHONE_NUMBER_LEN_MAX] = {0};
    LOGD(L_APP, L_V5, "");
    memcpy(num, G_parameter.sos_num, TRACK_DEFINE_SOS_COUNT * TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
    if (cmd->part == 0)
    {
        if (4 == TRACK_DEFINE_SOS_COUNT)
        {
            sprintf(cmd->rsp_msg, "SOS1:%s SOS2:%s SOS3:%s SOS4:%s", num[0], num[1], num[2], num[3]);
        }
        else
        {
            sprintf(cmd->rsp_msg, "SOS1:%s SOS2:%s SOS3:%s", num[0], num[1], num[2]);
        }
        return;
    }
    else if (cmd->part == 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (cmd->supercmd != 1 && cmd->return_sms.cm_type == CM_SMS && G_parameter.Permission.Permit && track_cust_sos_or_center_check(3, cmd->return_sms.ph_num) != 9)
    {
#if defined(__GT500__) || defined(__GT300__) ||defined(__GT03D__)||defined(__GT300S__)
        sprintf(cmd->rsp_msg, "Error! Only SOS phone numbers are allowed to set.");
#else
#if defined(__GERMANY__)
        sprintf(cmd->rsp_msg, "Fehler! Nur die SOS- und Center-Nr.kann Befehl senden.");

#else
        sprintf(cmd->rsp_msg, "Error! Only center phone number and SOS phone numbers are allowed to set.");
#endif/*__GERMANY__*/
#endif
        return;
    }
    if (!strcmp(cmd->pars[1], "A"))
    {
        int len, i, add = 0, count = 0;
#if defined(__GT300__) || defined(__GW100__)||defined(__GT300S__)
        if (cmd->return_sms.cm_type == CM_ONLINE)
        {
            memset(num, 0, TRACK_DEFINE_SOS_COUNT * TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
        }
#endif /* __GT300__ */
        if (cmd->part > (TRACK_DEFINE_SOS_COUNT + 1))
        {
#if defined (__GERMANY__)
            sprintf(cmd->rsp_msg, "Fehler: Max. %d Nummern eingeben!", TRACK_DEFINE_SOS_COUNT);
#else
            sprintf(cmd->rsp_msg, "Error: At most once to add %d numbers!", TRACK_DEFINE_SOS_COUNT);
#endif
            return;
        }
        for (i = 2; i <= cmd->part; i++)
        {
            len = strlen(cmd->pars[i]);
            if (len > 0)
            {
                if (track_fun_check_str_is_number(1, cmd->pars[i]) == 0)
                {
                    sprintf(cmd->rsp_msg, "Error: Number contains non-numeric characters!");
                    return;
                }
                if (len >= TRACK_DEFINE_PHONE_NUMBER_LEN_MAX)
                {
                    sprintf(cmd->rsp_msg, "Error: Number length exceeds 20 characters!");
                    return;
                }
                else if (len < 3)
                {
#if defined (__GERMANY__)
                    sprintf(cmd->rsp_msg, "Fehler: Mindestens 3 Zeichen eingeben!");
#else
                    sprintf(cmd->rsp_msg, "Error: Number length must exceeds 3 characters!");
#endif
                    return;
                }
                strcpy(num[i - 2], cmd->pars[i]);
                add++;
            }
        }
        if (add || cmd->return_sms.cm_type == CM_ONLINE)
        {
            memcpy(G_parameter.sos_num, num, TRACK_DEFINE_SOS_COUNT * TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
            if (4 == TRACK_DEFINE_SOS_COUNT)
            {
                sprintf(cmd->rsp_msg, "OK! \r\nSOS1:%s SOS2:%s SOS3:%s SOS4:%s", G_parameter.sos_num[0], G_parameter.sos_num[1], G_parameter.sos_num[2], G_parameter.sos_num[3]);
            }
            else
            {
                sprintf(cmd->rsp_msg, "OK! \r\nSOS1:%s SOS2:%s SOS3:%s", G_parameter.sos_num[0], G_parameter.sos_num[1], G_parameter.sos_num[2]);
            }
        }
#if defined(__GT300__) || defined(__GW100__)||defined(__GT300S__)
        cmd_updata_to_net(cmd);
#endif /* __GT300__ */
#if defined(__ET310__)||defined (__ET320__)|| defined(__GT370__)|| defined (__GT380__)|| defined (__NT37__)|| defined (__NT33__) || defined(__GT420D__)
        track_cust_paket_9404_handle();
#endif
    }
    else if (!strcmp(cmd->pars[1], "D"))
    {
        if (cmd->part > (TRACK_DEFINE_SOS_COUNT + 1))
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        if (strlen(cmd->pars[2]) == 1)
        {
            // 序号
            int no, j;
            for (j = 2; j <= cmd->part; j++)
            {
                if (strlen(cmd->pars[j]) > 0)
                {
                    no = track_fun_atoi(cmd->pars[j]);
                    if (no < 1 || no > TRACK_DEFINE_SOS_COUNT)
                    {
                        sprintf(cmd->rsp_msg, "Error: serial number can only from 1 to %d in!", TRACK_DEFINE_SOS_COUNT);
                        return;
                    }
                    if (strlen(num[no - 1]) == 0)
                    {
                        sprintf(cmd->rsp_msg, "Error: The number %d does not exist in the sos number!", no);
                        return;
                    }
                    memset(num[no - 1], 0, TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
                }
            }
            memcpy(G_parameter.sos_num, num, TRACK_DEFINE_SOS_COUNT * TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
            if (4 == TRACK_DEFINE_SOS_COUNT)
            {
                sprintf(cmd->rsp_msg, "OK! \r\nSOS1:%s SOS2:%s SOS3:%s SOS4:%s", G_parameter.sos_num[0], G_parameter.sos_num[1], G_parameter.sos_num[2], G_parameter.sos_num[3]);
            }
            else
            {
                sprintf(cmd->rsp_msg, "OK! \r\nSOS1:%s SOS2:%s SOS3:%s", G_parameter.sos_num[0], G_parameter.sos_num[1], G_parameter.sos_num[2]);
            }
#if defined(__GT300__) || defined(__GW100__)||defined(__GT300S__)
            cmd_updata_to_net(cmd);
#endif /* __GT300__ */
#if defined(__ET310__)||defined (__ET320__)|| defined(__GT370__)|| defined (__GT380__)|| defined (__NT37__)|| defined (__NT33__)
            track_cust_paket_9404_handle();
#endif
        }
        else
        {
            // 号码
            int i;
            if (track_fun_check_str_is_number(1, cmd->pars[2]) == 0)
            {
                sprintf(cmd->rsp_msg, "Error: Number contains non-numeric characters!");
                return;
            }
            if (strlen(cmd->pars[2]) >= TRACK_DEFINE_PHONE_NUMBER_LEN_MAX)
            {
                sprintf(cmd->rsp_msg, "Error: Number length exceeds 20 characters!");
                return;
            }
            if (cmd->part > 2)
            {
                sprintf(cmd->rsp_msg, "only remove one number by once!");
                return;
            }
            for (i = 0; i < TRACK_DEFINE_SOS_COUNT; i++)
            {
                if (!strcmp(cmd->pars[2], G_parameter.sos_num[i]) && strlen(G_parameter.sos_num[i]) > 0)
                {
                    memset(G_parameter.sos_num[i], 0, TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
                    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
                    if (4 == TRACK_DEFINE_SOS_COUNT)
                    {
                        sprintf(cmd->rsp_msg, "OK! \r\nSOS1:%s SOS2:%s SOS3:%s SOS4:%s", G_parameter.sos_num[0], G_parameter.sos_num[1], G_parameter.sos_num[2], G_parameter.sos_num[3]);
                    }
                    else
                    {
                        sprintf(cmd->rsp_msg, "OK! \r\nSOS1:%s SOS2:%s SOS3:%s", G_parameter.sos_num[0], G_parameter.sos_num[1], G_parameter.sos_num[2]);
                    }
                }
            }
#if defined(__GT300__) || defined(__GW100__)||defined(__GT300S__)
            cmd_updata_to_net(cmd);
#endif /* __GT300__ */
#if defined(__ET310__)||defined (__ET320__)|| defined(__GT370__)|| defined (__GT380__)|| defined (__NT37__)|| defined (__NT33__) ||defined(__GT420D__)
            track_cust_paket_9404_handle();
#endif
        }
    }
    else if (track_cust_password_sw())
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 2 is not a valid!");
        return;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 is not a valid!");
        return;
    }
}
#if defined( __GT310__)
static cmd_callmode(CMD_DATA_STRUCT *cmd)
{
    int value = 0, value2 = 0;
    kal_uint8   audio_mode = track_drv_sys_aud_get_active_audio_mode();
    kal_uint8   voice_level = track_drv_sys_aud_get_volume_level(audio_mode, 4/*AUD_VOLUME_SPH*/);

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "CallMode:%d,AudioMode:%d, Level:%d", G_parameter.callmode, audio_mode, voice_level);
        return;
    }
    if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    value = track_fun_atoi(cmd->pars[1]);
    if (value != 0 && value != 3)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter (0,3)");
        return;
    }
    G_parameter.callmode = value;
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }

    track_call_mode_stop_action((void*)1);
    track_start_timer(TRACK_CUST_CALLMODE_STOP_TIMER_ID, 5000, track_call_mode_stop_action, (void*)0);
}


static void cmd_fn(CMD_DATA_STRUCT *cmd)
{
    char num[TRACK_DEFINE_FN_COUNT][TRACK_DEFINE_PHONE_NUMBER_LEN_MAX] = {0};
    LOGD(L_APP, L_V5, "");
    memcpy(num, G_phone_number.fn_num, TRACK_DEFINE_FN_COUNT * TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
    if (cmd->part == 0)
    {
        if (2 == TRACK_DEFINE_FN_COUNT)
        {
            sprintf(cmd->rsp_msg, "FN1:%s FN2:%s", num[0], num[1]);
        }
        return;
    }
    else if (cmd->part == 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    /*
    if(cmd->return_sms.cm_type == CM_SMS && G_parameter.Permission.Permit && track_cust_sos_or_center_check(3, cmd->return_sms.ph_num) != 9)
    {
        sprintf(cmd->rsp_msg, "Error! Only center phone number and SOS phone number to set.");
        return;
    }*/
    if (!strcmp(cmd->pars[1], "A"))
    {
        int len, i, add = 0, count = 0;

#if defined(__GT300__)||defined(__GT300S__)
        if (cmd->return_sms.cm_type == CM_ONLINE)
        {
            memset(num, 0, TRACK_DEFINE_FN_COUNT * TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
        }
#endif /* __GT300__ */
        if (cmd->part > (TRACK_DEFINE_FN_COUNT + 1))
        {
            sprintf(cmd->rsp_msg, "Error: At most once to add %d numbers!", TRACK_DEFINE_FN_COUNT);
            return;
        }
        for (i = 2; i <= cmd->part; i++)
        {
            len = strlen(cmd->pars[i]);
            if (len > 0)
            {
                if (track_fun_check_str_is_number(1, cmd->pars[i]) == 0)
                {
                    sprintf(cmd->rsp_msg, "Error: Number contains non-numeric characters!");
                    return;
                }
                if (len >= TRACK_DEFINE_PHONE_NUMBER_LEN_MAX)
                {
                    sprintf(cmd->rsp_msg, "Error: Number length exceeds 20 characters!");
                    return;
                }
                else if (len < 3)
                {
                    sprintf(cmd->rsp_msg, "Error: Number length must exceeds 3 characters!");
                    return;
                }
                strcpy(num[i - 2], cmd->pars[i]);
                add++;
            }
        }
        if (add || cmd->return_sms.cm_type == CM_ONLINE)
        {
            memcpy(G_phone_number.fn_num, num, TRACK_DEFINE_FN_COUNT * TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
            Track_nvram_write(NVRAM_EF_PHONE_NUMBER_LID, 1, (void *)&G_phone_number, NVRAM_EF_PHONE_NUMBER_SIZE);
            if (2 == TRACK_DEFINE_FN_COUNT)
            {
                sprintf(cmd->rsp_msg, "OK! \r\nFN1:%s FN2:%s ", G_phone_number.fn_num[0], G_phone_number.fn_num[1]);
            }
        }
#if defined(__GT300__)||defined(__GT300S__)
        cmd_updata_to_net(cmd);
#endif /* __GT300__ */
    }
    else if (!strcmp(cmd->pars[1], "D"))
    {
        if (cmd->part > (TRACK_DEFINE_FN_COUNT + 1))
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        if (strlen(cmd->pars[2]) == 1)
        {
            // 序号
            int no, j;
            for (j = 2; j <= cmd->part; j++)
            {
                if (strlen(cmd->pars[j]) > 0)
                {
                    no = track_fun_atoi(cmd->pars[j]);
                    if (no < 1 || no > TRACK_DEFINE_FN_COUNT)
                    {
                        sprintf(cmd->rsp_msg, "Error: serial number can only from 1 to %d in!", TRACK_DEFINE_FN_COUNT);
                        return;
                    }
                    if (strlen(num[no - 1]) == 0)
                    {
                        sprintf(cmd->rsp_msg, "Error: The number %d does not exist in the fn number!", no);
                        return;
                    }
                    memset(num[no - 1], 0, TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
                }
            }
            memcpy(G_phone_number.fn_num, num, TRACK_DEFINE_FN_COUNT * TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
            Track_nvram_write(NVRAM_EF_PHONE_NUMBER_LID, 1, (void *)&G_phone_number, NVRAM_EF_PHONE_NUMBER_SIZE);
            if (2 == TRACK_DEFINE_FN_COUNT)
            {
                sprintf(cmd->rsp_msg, "OK! \r\nFN1:%s FN2:%s", G_phone_number.fn_num[0], G_phone_number.fn_num[1]);
            }
#if defined(__GT300__)||defined(__GT300S__)
            cmd_updata_to_net(cmd);
#endif /* __GT300__ */
        }
        else
        {
            // 号码
            int i;
            if (track_fun_check_str_is_number(1, cmd->pars[2]) == 0)
            {
                sprintf(cmd->rsp_msg, "Error: Number contains non-numeric characters!");
                return;
            }
            if (strlen(cmd->pars[2]) >= TRACK_DEFINE_PHONE_NUMBER_LEN_MAX)
            {
                sprintf(cmd->rsp_msg, "Error: Number length exceeds 20 characters!");
                return;
            }
            if (cmd->part > 2)
            {
                sprintf(cmd->rsp_msg, "only remove one number by once!");
                return;
            }
            for (i = 0; i < 3; i++)
            {
                if (!strcmp(cmd->pars[2], G_phone_number.fn_num[i]) && strlen(G_phone_number.fn_num[i]) > 0)
                {
                    memset(G_phone_number.fn_num[i], 0, TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
                    Track_nvram_write(NVRAM_EF_PHONE_NUMBER_LID, 1, (void *)&G_phone_number, NVRAM_EF_PHONE_NUMBER_SIZE);
                    if (2 == TRACK_DEFINE_FN_COUNT)
                    {
                        sprintf(cmd->rsp_msg, "OK! \r\nFN1:%s FN2:%s ", G_phone_number.fn_num[0], G_phone_number.fn_num[1]);
                    }
                }
            }
#if defined(__GT300__)||defined(__GT300S__)
            cmd_updata_to_net(cmd);
#endif /* __GT300__ */
        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter '%s' is not a valid!", cmd->pars[1]);
        return;
    }
}
#endif
static void cmd_almrep(CMD_DATA_STRUCT *cmd)
{
    int len, i, add = 0, count = 0, value = 0;
    kal_bool almrep[TRACK_DEFINE_SOS_COUNT] = {0};

    LOGD(L_APP, L_V5, "");
    memcpy(almrep, G_phone_number.almrep, TRACK_DEFINE_SOS_COUNT);

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "SOS1:%d,SOS2:%d,SOS3:%d", almrep[0], almrep[1], almrep[2]);
        return;
    }
    else if (cmd->part > TRACK_DEFINE_SOS_COUNT)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    for (i = 1; i <= cmd->part; i++)
    {
        len = strlen(cmd->pars[i]);
        if (len > 0)
        {
            value = track_fun_atoi(cmd->pars[i]);

            if (0 != value && 1 != value)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter %d (0/1)", i);
                return;
            }
            almrep[i - 1] = value;
            LOGD(L_APP, L_V5, "value:%d,arlmrep[%d]:%d", value, i, almrep[i - 1]);
            add++;
        }
    }
    if (add)
    {
        memcpy(G_phone_number.almrep, almrep, TRACK_DEFINE_SOS_COUNT);
        Track_nvram_write(NVRAM_EF_PHONE_NUMBER_LID, 1, (void *)&G_phone_number, NVRAM_EF_PHONE_NUMBER_SIZE);
        sprintf(cmd->rsp_msg, "OK! \r\SOS1:%d,SOS2:%d,SOS3:%d", G_phone_number.almrep[0], G_phone_number.almrep[1], G_phone_number.almrep[2]);
    }
    else
    {
        sprintf(cmd->rsp_msg, "SOS1:%d,SOS2:%d,SOS3:%d", almrep[0], almrep[1], almrep[2]);
    }
}

static void cmd_see_sos(CMD_DATA_STRUCT *cmd)
{
    char num[TRACK_DEFINE_SOS_COUNT][TRACK_DEFINE_PHONE_NUMBER_LEN_MAX] = {0};
    LOGD(L_APP, L_V5, "type:%d", cmd->return_sms.cm_type);

    if (cmd->return_sms.cm_type == CM_ONLINE)
    {
        memcpy(num, G_parameter.sos_num, TRACK_DEFINE_SOS_COUNT * TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);

        cmd->rsp_length = sprintf(cmd->rsp_msg, "SEESOS:%s,%s,%s,%s#", num[0], num[1], num[2], num[3]);
    }
}

/******************************************************************************
 *  Function    -  cmd_sosalm
 *
 *  Purpose     -  SOS 开/关 模式 设置
 *
 *  Description -  SOS 报警指令：SOSALM,ON[,M]#
 *
 *                 ON=开启 SOS 报警；OFF=关闭 SOS 报警；（默认为开启）
 *
 *                 M=报警上报方式（默认为 1）
 *                     M=0 仅GPRS;
 *                     M=1 SMS+GPRS;
 *                     M=2 GPRS+SMS+电话；
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 25-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_sosalm(CMD_DATA_STRUCT *cmd)
{
    nvram_sos_alarm_struct alarm = {0};
    LOGD(L_APP, L_V5, "");
    if (cmd->part == 0)
    {
#if defined(__GW100__)
        sprintf(cmd->rsp_msg, "%s:%s mode:%d(0=only GPRS,1=GPRS+SMS,2=GPRS+SMS+录音,3=GPRS+录音)", cmd->pars[0],
                ON_OFF(G_parameter.sos_alarm.sw), G_parameter.sos_alarm.alarm_type);
#else
        sprintf(cmd->rsp_msg, "%s:%s mode:%d(0=only GPRS,1=GPRS+SMS,2=GPRS+SMS+Call,3=GPRS+Call)", cmd->pars[0],
                ON_OFF(G_parameter.sos_alarm.sw), G_parameter.sos_alarm.alarm_type);
#endif /* __GW100__ */
        return;
    }
    memcpy(&alarm, &G_parameter.sos_alarm, sizeof(nvram_sos_alarm_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part != 1 && cmd->part != 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 1;
        if (cmd->part == 2)
        {
            if (!strcmp(cmd->pars[2], "0"))
            {
                alarm.alarm_type = 0;
            }
            else if (!strcmp(cmd->pars[2], "1"))
            {
                alarm.alarm_type = 1;
            }
            else if (!strcmp(cmd->pars[2], "2"))
            {
                alarm.alarm_type = 2;
            }
            else if (!strcmp(cmd->pars[2], "3"))
            {
                alarm.alarm_type = 3;
            }
            else
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1/2/3)");
                return;
            }
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.sos_alarm, &alarm, sizeof(nvram_sos_alarm_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
#if defined(__ET310__)|| defined(__GT370__)|| defined (__GT380__)|| defined (__NT37__)|| defined (__NT33__)
        track_cust_paket_9404_handle();
#endif

    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

static void cmd_factory(CMD_DATA_STRUCT *cmd)
{
    S32 adc4_offset = G_parameter.adc4_offset;
    nvram_FCFALM_struct alarm = {0};

    kal_uint8 fake_cell_enable = G_parameter.fake_cell_enable;
//     memcpy( &alarm ,&G_parameter.fcfalm,sizeof(nvram_FCFALM_struct));
    extern void track_cust_send09_out(void);
    extern void track_cust_send21(void* par);
    extern void track_cust_send04(void* par);
    extern void track_cust_send06(void* par);
    extern void track_cust_module_Handshake(void);
    extern void gps_data_backup_manage(kal_uint8 type , kal_uint8 * data, kal_uint32  id, kal_uint32  len);
    if (cmd->part == 0)
    {
        if (!strcmp(cmd->pars[0], "FACTORYALL"))
        {
            Track_nvram_write(NVRAM_EF_LOGD_LID, 1, OTA_NVRAM_EF_LOGD_DEFAULT, NVRAM_EF_LOGD_SIZE);
            Track_nvram_write(NVRAM_EF_PORT_SETTING_LID, 1, OTA_NVRAM_EF_PORT_SETTING_DEFAULT, NVRAM_EF_PORT_SETTING_SIZE);
        }
        else
        {
            nvram_ef_logd_struct log_setting_tmp = {0};
            nvram_ef_logd_struct log_setting = {0};
            Track_nvram_read(NVRAM_EF_LOGD_LID, 1, (void *)&log_setting, NVRAM_EF_LOGD_SIZE);
            memcpy(&log_setting_tmp, OTA_NVRAM_EF_LOGD_DEFAULT, sizeof(nvram_ef_logd_struct));
            log_setting_tmp.logd_target = log_setting.logd_target;
            Track_nvram_write(NVRAM_EF_LOGD_LID, 1, &log_setting_tmp, NVRAM_EF_LOGD_SIZE);
        }
        track_soc_clear_queue();
        gps_data_backup_manage(5, NULL, 0, 0);
        Track_nvram_write(NVRAM_EF_GPS_BACKUP_DATA_PARAM_LID, 1, OTA_NVRAM_EF_GPS_BACKUP_DATA_PARAM_DEFAULT, NVRAM_EF_GPS_BACKUP_DATA_PARAM_SIZE);
        GPS_NVRAM_Backup_Init();

        track_cust_backup_gps_data(5, NULL);

        /*if(track_nvram_alone_parameter_read()->server.lock != 1)
        {
            memcpy(&G_importance_parameter_data, OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        }
        Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        */
        memcpy(&G_parameter, OTA_NVRAM_EF_PARAMETER_DEFAULT, NVRAM_EF_PARAMETER_SIZE);
#if defined(__GT02__) && defined(__SYS_VERSION__)
        track_set_server_ip_status(0);
        track_os_intoTaskMsgQueueExt(track_soc_gprs_reconnect, (void*)22);//修改域名后，重新建立连接
        if (G_importance_parameter_data.app_version.version == 1)
        {
            G_parameter.lang = 1;
            //     G_parameter.sensor_gps_work = 0;
        }
        else if (G_importance_parameter_data.app_version.version == 2)
        {
            G_parameter.lang = 1;
            //  G_parameter.sensor_gps_work = 0;
        }
        else if (G_importance_parameter_data.app_version.version == 3)
        {
            G_parameter.lang = 0;
            G_parameter.sensor_gps_work = 5;
            G_parameter.agps.sw = 0;
            memcpy(&G_parameter.url, "http://maps.google.com/maps?q=", strlen("http://maps.google.com/maps?q="));

        }
        else if (G_importance_parameter_data.app_version.version == 4)
        {
            G_parameter.lang = 1;
            //   G_parameter.sensor_gps_work = 5;
        }
        /* else
        {
             memcpy(&G_parameter, OTA_NVRAM_EF_PARAMETER_DEFAULT, NVRAM_EF_PARAMETER_SIZE);
         }
         */
#elif defined(__ET310__) || defined(__MT200__)
        G_parameter.adc4_offset = adc4_offset;
#endif

        G_parameter.fake_cell_enable = fake_cell_enable; //格式化不清除防伪功能，只受加密串口指令
        //  memcpy(&G_parameter.fcfalm, &alarm, sizeof(nvram_FCFALM_struct));
        G_realtime_data.record_tracking_time = 0;
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);

        memcpy(&G_realtime_data, OTA_NVRAM_EF_REALTIME_DEFAULT, NVRAM_EF_REALTIME_SIZE);
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

        memcpy(&G_phone_number, OTA_NVRAM_EF_PHONE_NUMBER_DEFAULT, NVRAM_EF_PHONE_NUMBER_SIZE);
        Track_nvram_write(NVRAM_EF_PHONE_NUMBER_LID, 1, (void *)&G_phone_number, NVRAM_EF_PHONE_NUMBER_SIZE);
        {
#if defined(__GERMANY__)
            sprintf(cmd->rsp_msg, "OK - der Tracker startet in 60 Sek.neu.");
#elif defined(__SPANISH__)
            sprintf(cmd->rsp_msg, "La terminal se reiniciara despues de 60 segundos!");
#else
            sprintf(cmd->rsp_msg, "OK!\r\nThe terminal will restart after 60s!");
#endif
        }

#if defined(__ET310__)
        {
            nvram_check_ota_struct g_otacheck = {0};
            memcpy(&g_otacheck, NVRAM_EF_CHECK_OTA_DEFAULT, NVRAM_EF_CHECK_OTA_SIZE);
            Track_nvram_write(NVRAM_EF_CHECK_OTA_LID, 1, (void *)&g_otacheck, NVRAM_EF_CHECK_OTA_SIZE);
        }
#endif /* __ET310__ */

#if defined(__NVRAM_BACKUP_PARTITION__)
        //track_cmd_send_to_nvram(0); /* NVRAM_SDS_ACCESS_BACKUP */
#endif /* __NVRAM_BACKUP_PARTITION__ */
#if defined(__GT740__)||defined(__GT420D__)
        track_cust_module_resetpro();
        //track_cust_resetbat(1);
#endif
#if defined(__GT420D__)
        if (track_is_timer_run(TRACK_CUST_NET_MAXMUM_WORK_TIME_TIMER_ID))
        {
            track_stop_timer(TRACK_CUST_NET_MAXMUM_WORK_TIME_TIMER_ID);
        }
        if (track_is_timer_run(TRACK_CUST_SEND_DATA_MAXMUM_WORK_TIME_TIMER_ID))
        {
            track_stop_timer(TRACK_CUST_SEND_DATA_MAXMUM_WORK_TIME_TIMER_ID);
        }
        if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
        {
            track_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
        }
#endif
        {
            track_cust_send09_out();
            tr_start_timer(TRACK_CUST_EXTCHIP_WAIT08_TIMER_ID, 1000, track_cust_module_Handshake);
            track_start_timer(TRACK_CUST_EXTCHIP_WAIT06_TIMER_ID, 3000, track_cust_send06, (void *)G_parameter.teardown_str.sw);
            track_start_timer(TRACK_CUST_EXTCHIP_WAIT04_TIMER_ID, 5000, track_cust_send04, (void *)G_parameter.extchip.mode);
            track_start_timer(TRACK_CUST_EXTCHIP_WAIT21_CHECKSTATUS_TIMER_ID,6000,track_cust_send21,(void *)5);//查询静止运动状态
            track_start_timer(TRACK_CUST_EXTCHIP_WAIT21_SENDSTATIC_TIMER_ID,7000,track_cust_send21,(void *)6);
            track_start_timer(TRACK_CUST_EXTCHIP_WAIT21_SENDSPORT_TIMER_ID,8000,track_cust_send21,(void *)7);
        }
        track_cust_restart(3, 60);
    }
}
static void cmd_factory_all(CMD_DATA_STRUCT *cmd)
{
    U16 ver=0;
    nvram_mqtt_login_struct  mqtt_login = {0};
    if (cmd->part == 0)
    {
#ifdef __GT02__
        track_set_server_ip_status(0);
        track_os_intoTaskMsgQueueExt(track_soc_gprs_reconnect, (void*)22);//修改域名后，重新建立连接
#if defined(__GT02__) && defined(__SYS_VERSION__)
        if (G_importance_parameter_data.app_version.version == 1)
        {
            memcpy(&G_importance_parameter_data, &NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT_SERVER_GUMI, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        }
        else if (G_importance_parameter_data.app_version.version == 2)
        {
            memcpy(&G_importance_parameter_data, &NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT_GUMI_GENERAL, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        }
        else if (G_importance_parameter_data.app_version.version == 3)
        {

            memcpy(&G_importance_parameter_data, &NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT_SERVER_COOACCESS, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        }
        else if (G_importance_parameter_data.app_version.version == 4)
        {
            memcpy(&G_importance_parameter_data, &NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT_SERVER_TUQIANG, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        }
        else
#endif /* __GT02__ */
        {
            memcpy(&G_importance_parameter_data, OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        }
        memcpy(G_importance_parameter_data.build_date, OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT->build_date, 15);
        memcpy(G_importance_parameter_data.build_time, OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT->build_time, 15);

#else
#ifdef __XCWS__
        nvram_ef_device_setting_struct logindata;
        memcpy(logindata.SequenceNumber, G_importance_parameter_data.login.SequenceNumber, 19);
        //memcpy(logindata.host_num, G_importance_parameter_data.login.host_num, TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
        //memcpy(logindata.owner_num, G_importance_parameter_data.login.owner_num, TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
        memcpy(&G_importance_parameter_data, (void*)track_drv_get_importPara_default(), NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        memcpy(G_importance_parameter_data.login.SequenceNumber, logindata.SequenceNumber, 19);
#elif defined(__GT740__)
        ver = G_importance_parameter_data.app_version.version;
        memcpy(&G_importance_parameter_data, (void*)track_drv_get_importPara_default(), NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        G_importance_parameter_data.app_version.version = ver;
#else
        memcpy(&mqtt_login, &G_importance_parameter_data.mqtt_login, sizeof(nvram_mqtt_login_struct));
        memcpy(&G_importance_parameter_data, (void*)track_drv_get_importPara_default(), NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        memcpy(&G_importance_parameter_data.mqtt_login, &mqtt_login, sizeof(nvram_mqtt_login_struct));
#endif
        nvram_alone_parameter_cmd.reset();
#endif /* __GT02__ */
        Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        cmd_factory(cmd);
        LOGS("%s",cmd->rsp_msg);
#if defined(__GT740__)||defined(__GT420D__)
        cmd_SYS_VERSION(cmd);
        sprintf(cmd->rsp_msg,"");
#endif
    }
}

static void cmd_delay(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%ds", cmd->pars[0], G_parameter.monitor_delay);
        return;
    }
    else if (cmd->part == 1)
    {
        int value;
        if (track_fun_check_str_is_number(1, cmd->pars[1]) == 0)
        {
            sprintf(cmd->rsp_msg, "Error: The first parameter is not a number!");
            return;
        }
        value = track_fun_atoi(cmd->pars[1]);
        if (value < 5 || value > 18)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (5-18)");
            return;
        }
        G_parameter.monitor_delay = value;
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
}

static void cmd_pwdsw(CMD_DATA_STRUCT *cmd)
{
    int value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], ON_OFF(G_parameter.Permission.password_sw));
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (cmd->supercmd != 1 && cmd->return_sms.cm_type == CM_SMS && track_cust_sos_or_center_check(3, cmd->return_sms.ph_num) != 9)
    {
#if defined(__GT300__) || defined(__GT500__) || defined(__GT03D__)||defined(__GT300S__)
        sprintf(cmd->rsp_msg, "Error! Only SOS phone numbers are allowed to set.");
#else
#if defined(__GERMANY__)
        sprintf(cmd->rsp_msg, "Fehler! Nur die SOS- und Center-Nr. kann Befehl senden.");
#else
        sprintf(cmd->rsp_msg, "Error! Only center phone number and SOS phone numbers are allowed to set.");
#endif/*__GERMANY__*/

#endif /* __GT300__ */
        return;
    }
    if (!strcmp(cmd->pars[1], "ON"))
        G_parameter.Permission.password_sw = 1;
    else if (!strcmp(cmd->pars[1], "OFF"))
        G_parameter.Permission.password_sw = 0;
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

static void cmd_password(CMD_DATA_STRUCT *cmd)
{
    int value, len;
    char newpassword[20] = {0}, oldpassword[20] = {0};
    //LOGD(L_APP, L_V5, "%s,%s", G_parameter.Permission.password, G_parameter.Permission.superPassword);
    if (G_parameter.Permission.password_sw)
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        len = strlen(cmd->pars[1]);
        if (len == 0)
        {
            sprintf(cmd->rsp_msg, "Error: new password can not be empty!");
            return;
        }
        if (len > 19)
        {
            sprintf(cmd->rsp_msg, "Error: Character length should be less than 20 characters!");
            return;
        }

        memcpy(newpassword, cmd->rcv_msg_source + (cmd->pars[1] - cmd->rcv_msg), len);
    }
    else
    {
        if (cmd->part != 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        len = strlen(cmd->pars[2]);
        if (strlen(cmd->pars[1]) == 0 || len == 0)
        {
            sprintf(cmd->rsp_msg, "Error: Old password or new password can not be empty!");
            return;
        }
        if (len > 19)
        {
            sprintf(cmd->rsp_msg, "Error: Character length should be less than 20 characters!");
            return;
        }

        memcpy(oldpassword, cmd->rcv_msg_source + (cmd->pars[1] - cmd->rcv_msg), strlen(cmd->pars[1]));
        if (strcmp(oldpassword, G_parameter.Permission.password))
        {
            sprintf(cmd->rsp_msg, "Error: Old password error!");
            return;
        }
        memcpy(newpassword, cmd->rcv_msg_source + (cmd->pars[2] - cmd->rcv_msg), len);
    }
    if (track_fun_check_str_is_number_and_string(0, newpassword) <= 0)
    {
        sprintf(cmd->rsp_msg, "Error: password characters can only be numbers or letters.");
        return;
    }
    //LOGD(L_APP, L_V5, "old:|%s|", G_parameter.Permission.password);
    if (!strcmp(G_parameter.Permission.password, newpassword))
    {
        sprintf(cmd->rsp_msg, "The new password and the same old password!");
        return;
    }
    track_fun_strncpy(G_parameter.Permission.password, newpassword, 20);
    //LOGD(L_APP, L_V5, "new:|%s|", G_parameter.Permission.password);
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

static void cmd_recover(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        if (cmd->supercmd == 1)
        {
            memcpy(G_parameter.Permission.password, OTA_NVRAM_EF_PARAMETER_DEFAULT->Permission.password, 20);
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
            sprintf(cmd->rsp_msg, "OK!");
        }
        else
        {
            sprintf(cmd->rsp_msg, "Error: Not authorized!");
            return;
        }
    }
}
static kal_uint8 string3_en[] = {"The terminal is in Deep Sleep, please wake up!"};

/******************************************************************************
 *  Function    -  cmd_BLINDALM
 *
 *  Purpose     -  GPS盲区报警
 *
 *  Description -
        BLINDALM,ON,20,60,1#
        BLINDALM#
            查询已设置的参数
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  JWJ  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_BLINDALM(CMD_DATA_STRUCT *cmd)
{
    nvram_blind_struct alarm = {0};
    int value;
    extern  void track_cust_module_alarm_blind_init(void);

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d,%d,%d", cmd->pars[0], ON_OFF(G_parameter.gps_blind.sw), G_parameter.gps_blind.blind_in_time, G_parameter.gps_blind.blind_out_time, G_parameter.gps_blind.alarm_type);
        return;
    }
    if (track_cust_get_work_mode() == WORK_MODE_3)
    {
        snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, string3_en);
        return;
    }
    memcpy(&alarm, &G_parameter.gps_blind, sizeof(nvram_blind_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part < 1 || cmd->part > 4)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 1;
        if (cmd->part >= 2)
        {
            value = track_fun_atoi(cmd->pars[2]);
            if (value < 5 || value > 600)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (5-600)");
                return;
            }
            alarm.blind_in_time = value;
        }
        if (cmd->part >= 3)
        {
            value = track_fun_atoi(cmd->pars[3]);
            if (value < 5 || value > 600)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (5-600)");
                return;
            }
            alarm.blind_out_time = value;
        }
        if (cmd->part >= 4)
        {
            value = track_fun_atoi(cmd->pars[4]);
#if defined(__NT36__)
            if (value < 0 || value > 1)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 4 (0/1)");
                return;
            }
#else
            if (value < 0 || value > 3)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 4 (0/1/2/3)");
                return;
            }
#endif
            alarm.alarm_type = value;
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.gps_blind, &alarm, sizeof(nvram_blind_struct));
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);

//   track_cust_defences_cmd_update();
    sprintf(cmd->rsp_msg, "OK!");
}


/******************************************************************************
 *  Function    -  cmd_low_power_alarm
 *
 *  Purpose     -  低电报警
 *
 *  Description -
        BATALM,<A>[,M]#
            A=ON/OFF；默认值：ON
            M=0 - 1；默认值：1；
        BATALM,OFF#
            关闭电池低电报警
        BATALM#
            查询已设置的参数
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_batalm(CMD_DATA_STRUCT *cmd)
{
    nvram_low_power_alarm_struct alarm = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d", cmd->pars[0],
                ON_OFF(G_parameter.low_power_alarm.sw), G_parameter.low_power_alarm.alarm_type);
        return;
    }
    memcpy(&alarm, &G_parameter.low_power_alarm, sizeof(nvram_low_power_alarm_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part < 1 || cmd->part > 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 1;
        if (cmd->part >= 2)
        {
            alarm.alarm_type = track_fun_atoi(cmd->pars[2]);
#if defined(__ET310__)||defined(__NT36__)||defined (__ET320__)||defined(__MT200__)||defined(__GT420D__)
            if (alarm.alarm_type != 0 )
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0)");
                return;
            }
#else
            if (alarm.alarm_type < 0 || alarm.alarm_type > 3)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1/2/3)");
                return;
            }
#endif /* __ET310__ */
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
#if !defined(__GT420D__)
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
#endif
        alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.low_power_alarm, &alarm, sizeof(nvram_low_power_alarm_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
#if defined(__ET310__)||defined (__ET320__)|| defined(__GT370__)|| defined (__GT380__)|| defined (__NT37__)|| defined (__NT33__)
    track_cust_paket_9404_handle();
#endif
}


#if defined(__GT420D__)




static cmd_lowbat(CMD_DATA_STRUCT * cmd)
{
    float volt = 0;
    nvram_low_power_alarm_struct alarm = {0};
    U8   v_percnt[9] = {10,20,30,40,50,60,70,80,90};
    U32 v_level[9] = { 3590000, 3630000, 3670000, 3710000, 3760000,3830000,3890000,3950000,3990000};
    U32 value2 = 0;
    int  i = 0,percent=0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], G_parameter.percent);
        return;
    }
    if (cmd->part >1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    percent = track_fun_atoi(cmd->pars[1]);
    for (i; i<9; i++)
    {
        if (percent==v_percnt[i])
        {
            G_parameter.percent  = track_fun_atoi(cmd->pars[1]);
            value2 = v_level[i];
            volt = track_cust_MCUrefer_adc_data(99);
            volt =((float)volt)/100;
            if ( volt>((float)value2/1000000))
            {
                track_cust_lowpower_flag_data(0);

                LOGD(L_APP, L_V5, "低电报警可可再次进入");
            }
            LOGD(L_APP, L_V5, "低电报警阀值:%.2fV,当前电压%.2fV", ((float)value2/1000000) ,volt);
            if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
            {
                sprintf(cmd->rsp_msg, "OK!");
                return;
            }
            else
            {
                sprintf(cmd->rsp_msg, "Error!");
                }
            return;
        }
    }
    sprintf(cmd->rsp_msg, "Error:The correct interval is not set!");
}





#endif
/******************************************************************************
 *  Function    -  cmd_gmt
 *
 *  Purpose     -  时区设置
 *
 *  Description -  GMT,<E/W>,<0 - 12>[,0/15/30/45]
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 24-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_gmt(CMD_DATA_STRUCT *cmd)
{
    nvram_gps_time_zone_struct zone = {0};
    int value;
    kal_bool flag = 1;
    if (cmd->part == 0)
    {
        if (G_parameter.zone.timezone_auto == 1)
        {
            sprintf(cmd->rsp_msg, "%s:%c,%d,%d (AUTO)", cmd->pars[0],
                    G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min);
        }
        else
        {
            sprintf(cmd->rsp_msg, "%s:%c,%d,%d", cmd->pars[0],
                    G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min);
        }
        return;
    }
    else if (cmd->part < 1 || cmd->part > 3)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    memcpy(&zone, &G_parameter.zone, sizeof(nvram_gps_time_zone_struct));
    if (!strcmp(cmd->pars[1], "AUTO"))
    {
        zone.timezone_auto = 1;
    }
    else if (!strcmp(cmd->pars[1], "E") || !strcmp(cmd->pars[1], "W"))
    {
        zone.timezone_auto = 0;
        zone.zone = cmd->pars[1][0];
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (E/W)");
        return;
    }
    if (cmd->part >= 2)
    {
        value = track_fun_atoi(cmd->pars[2]);
        if (value < 0 || value > 12)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (0 to 12)");
            return;
        }
        zone.time = value;
        flag = 0;
    }
    if (cmd->part == 3)
    {
        value = track_fun_atoi(cmd->pars[3]);
        if (value != 0 && value != 15 && value != 30 && value != 45)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 3 (0/15/30/45)");
            return;
        }
        zone.time_min = value;
    }
    memcpy(&G_parameter.zone, &zone, sizeof(nvram_gps_time_zone_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
    track_cust_mlsp_set_time(NULL);
#endif//__MCU_LONG_STANDBY_PROTOCOL__
}
#if !defined(__GT420D__)
static void cmd_exbatalm(CMD_DATA_STRUCT *cmd)
{
    int value;
    nvram_power_extbatalm_struct extbatalm = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d,%d,%d,%d", cmd->pars[0], ON_OFF(G_parameter.power_extbatalm.sw),
                G_parameter.power_extbatalm.alarm_type, G_parameter.power_extbatalm.threshold,
                G_parameter.power_extbatalm.recover_threshold, G_parameter.power_extbatalm.detection_time);
        return;
    }
    memcpy(&extbatalm, &G_parameter.power_extbatalm, sizeof(nvram_power_extbatalm_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part < 1 || cmd->part > 5)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        extbatalm.sw = 1;
        if (cmd->part >= 2)
        {
            value = track_fun_atoi(cmd->pars[2]);
            if (value != 0 && value != 1)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1)");
                return;
            }
            extbatalm.alarm_type = value;
        }
        if (cmd->part >= 3)
        {
            value = track_fun_atoi(cmd->pars[3]);
            if (value < 10 || value > 360)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (10 - 360)");
                return;
            }
            extbatalm.threshold = value;
        }
        if (cmd->part >= 4)
        {
            value = track_fun_atoi(cmd->pars[4]);
            if (value < 10 || value > 360)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 4 (10 - 360)");
                return;
            }
            if (value <= extbatalm.threshold)
            {
                sprintf(cmd->rsp_msg, "Error: The fourth parameter can not be less than the third parameter");
                return;
            }
            extbatalm.recover_threshold = value;
        }
        if (cmd->part >= 5)
        {
            value = track_fun_atoi(cmd->pars[5]);
            if (value < 1 || value > 300)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 5 (1 - 300)");
                return;
            }
            extbatalm.detection_time = value;
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        extbatalm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.power_extbatalm, &extbatalm, sizeof(nvram_power_extbatalm_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
#if defined(__ET310__)||defined (__ET320__)|| defined(__GT370__)|| defined (__GT380__)|| defined (__NT37__)|| defined (__NT33__)
    track_cust_paket_9404_handle();
#endif
}
#endif
static void cmd_extbatacc(CMD_DATA_STRUCT *cmd)
{
#if defined( __EXT_VBAT_ADC__)
    int value;
    nvram_ext_vbat_check_acc_struct vbat_check_acc = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:L:%d,H:%d, The current value:%d", cmd->pars[0],
                G_parameter.vbat_check_acc.acc_l_threshold_vbat,
                G_parameter.vbat_check_acc.acc_h_thresholdvbat,
                track_drv_get_external_batter(1));
        return;
    }
    memcpy(&vbat_check_acc, &G_parameter.vbat_check_acc, sizeof(nvram_ext_vbat_check_acc_struct));
    if (cmd->part == 1 && !strcmp(cmd->pars[1], "HIGH"))
    {
        value = track_drv_get_external_batter(1) / 100000 - 1;
        vbat_check_acc.acc_h_thresholdvbat = value;
    }
    else if (cmd->part == 1 && !strcmp(cmd->pars[1], "LOW"))
    {
        value = track_drv_get_external_batter(1) / 100000 + 1;
        vbat_check_acc.acc_l_threshold_vbat = value;
    }
    else if (cmd->part == 2)
    {
        value = track_fun_atoi(cmd->pars[1]);
        if (value < 10 || value > 360)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (10 - 360)");
            return;
        }
        vbat_check_acc.acc_l_threshold_vbat = value;

        value = track_fun_atoi(cmd->pars[2]);
        if (value < 10 || value > 360)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (10 - 360)");
            return;
        }
        vbat_check_acc.acc_h_thresholdvbat = value;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    memcpy(&G_parameter.vbat_check_acc, &vbat_check_acc, sizeof(nvram_ext_vbat_check_acc_struct));
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    sprintf(cmd->rsp_msg, "OK! %s:L:%d,H:%d, The current value:%d", cmd->pars[0],
            G_parameter.vbat_check_acc.acc_l_threshold_vbat,
            G_parameter.vbat_check_acc.acc_h_thresholdvbat,
            track_drv_get_external_batter(1));
#endif /* __EXT_VBAT_ADC__ */
}
#if !defined(__GT420D__)
static void cmd_extbatcut(CMD_DATA_STRUCT *cmd)
{
    int value;
    nvram_power_extbatcut_struct extbatcut = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d,%d,%d,%d", cmd->pars[0], ON_OFF(G_parameter.power_extbatcut.sw),
                G_parameter.power_extbatcut.alarm_type, G_parameter.power_extbatcut.threshold,
                G_parameter.power_extbatcut.recover_threshold, G_parameter.power_extbatcut.detection_time);
        return;
    }
    memcpy(&extbatcut, &G_parameter.power_extbatcut, sizeof(nvram_power_extbatcut_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part < 1 || cmd->part > 5)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        extbatcut.sw = 1;
        if (cmd->part >= 2)
        {
            value = track_fun_atoi(cmd->pars[2]);
            if (value != 0 && value != 1)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1)");
                return;
            }
            extbatcut.alarm_type = value;
        }
        if (cmd->part >= 3)
        {
            value = track_fun_atoi(cmd->pars[3]);
            if (value < 10 || value > 360)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (10 - 360)");
                return;
            }
            extbatcut.threshold = value;
        }
        if (cmd->part >= 4)
        {
            value = track_fun_atoi(cmd->pars[4]);
            if (value < 10 || value > 360)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 4 (10 - 360)");
                return;
            }
            if (value <= extbatcut.threshold)
            {
                sprintf(cmd->rsp_msg, "Error: The fourth parameter can not be less than the third parameter");
                return;
            }
            extbatcut.recover_threshold = value;
        }
        if (cmd->part >= 5)
        {
            value = track_fun_atoi(cmd->pars[5]);
            if (value < 1 || value > 300)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 5 (1 - 300)");
                return;
            }
            extbatcut.detection_time = value;
        }
    }
#if defined (__CUST_ACC_BY_MCU__)
    /*该功能由单片机控制，始终有效       --    chengjun  2017-10-16*/
#else
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        extbatcut.sw = 0;
    }
#endif /* __CUST_ACC_BY_MCU__ */
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.power_extbatcut, &extbatcut, sizeof(nvram_power_extbatcut_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");

#if defined (__CUST_ACC_BY_MCU__)
        //结果同步到单片机
        track_drv_set_protection_voltage();
#endif /* __MCU_BJ8M601A_NT13V__ */
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

static void cmd_pwrcut(CMD_DATA_STRUCT *cmd)
{
    nvram_power_extbatcut_struct pwrcut = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d,%d,%d", cmd->pars[0],
                ON_OFF(G_parameter.power_extbatcut.sw), G_parameter.power_extbatcut.alarm_type, G_parameter.power_extbatcut.threshold, G_parameter.power_extbatcut.detection_time);
        return;
    }
    else if (cmd->part > 4)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    memcpy(&pwrcut, &G_parameter.power_extbatcut, sizeof(nvram_power_extbatcut_struct));
    if (!strcmp(cmd->pars[1], "ON"))
        pwrcut.sw = 1;
    else if (!strcmp(cmd->pars[1], "OFF"))
        pwrcut.sw = 0;
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    if (cmd->part >= 2)
    {
        pwrcut.alarm_type = track_fun_atoi(cmd->pars[2]);
        if (pwrcut.alarm_type != 0 && pwrcut.alarm_type != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (0 - 1)");
            return;
        }
    }
    if (cmd->part >= 3)
    {
        pwrcut.threshold = track_fun_atoi(cmd->pars[3]);
        pwrcut.recover_threshold = track_fun_atoi(cmd->pars[3]);
        if (pwrcut.threshold < 10 || pwrcut.threshold > 360)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 3 (10 - 360)");
            return;
        }
    }
    if (cmd->part >= 4)
    {
        pwrcut.detection_time = track_fun_atoi(cmd->pars[4]);
        if (pwrcut.detection_time < 1 || pwrcut.detection_time > 300)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 4 (1 - 300)");
            return;
        }
    }

    memcpy(&G_parameter.power_extbatcut, &pwrcut, sizeof(nvram_power_extbatcut_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}
#endif

static void cmd_gprson(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d(1=ON, 0=OFF)", cmd->pars[0], track_nvram_alone_parameter_read()->gprson);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (!strcmp(cmd->pars[1], "1"))
    {
#if defined(__GW100__)
        G_parameter.sos_alarm.alarm_type = 0;
#endif
        nvram_alone_parameter_cmd.gprson_set(1);
        track_soc_gprs_reconnect((void*)301);
    }
    else if (!strcmp(cmd->pars[1], "0"))
    {
#if defined(__GW100__)
        G_parameter.sos_alarm.alarm_type = 1;
#endif
        nvram_alone_parameter_cmd.gprson_set(0);
#if defined(__GT530__)
        track_drv_lte_server_close(0);
#endif
        track_soc_disconnect();
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0/1)");
        return;
    }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

static void cmd_link(CMD_DATA_STRUCT *cmd)
{
    int value = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], G_parameter.gprs_obstruction_alarm.link);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    value = track_fun_atoi(cmd->pars[1]);
    if (value < 1 || value > 10)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (1-10)");
        return;
    }
    G_parameter.gprs_obstruction_alarm.link = value;
    if (track_soc_conn_valid()) track_soc_gprs_reconnect((void*)23);    //LINK指令设置后，当前处于连接状态，重新建立连接
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

/******************************************************************************
 *  Function    -  cmd_hbt
 *
 *  Purpose     -  心跳包间隔时间设置
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 01-06-2013,  Cml       written
 * v1.1  , 31-07-2013,  zengping  written
 * v1.2  , 01-08-2013,  Cml       written
 * v1.3  , 06-25-2015,  Cml       written
 * ----------------------------------------
*******************************************************************************/
static void cmd_hbt(CMD_DATA_STRUCT *cmd)
{
    int hbt_acc_on = G_parameter.hbt_acc_on;
    int hbt_acc_off = G_parameter.hbt_acc_off;
#if defined(__NO_ACC__)
    if (cmd->part == 0)
    {
        if (G_parameter.hbt_acc_off % 60 == 0)
            sprintf(cmd->rsp_msg, "%s:%dmin", cmd->pars[0], G_parameter.hbt_acc_off / 60);
        else
            sprintf(cmd->rsp_msg, "%s:%dsec", cmd->pars[0], G_parameter.hbt_acc_off);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (cmd->pars[1][0] == 'S')
    {
        hbt_acc_on = track_fun_atoi(&cmd->pars[1][1]);
        if (hbt_acc_on < 10 || hbt_acc_on > 18000)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (10 - 18000)");
            return;
        }
    }
    else
    {
        hbt_acc_on = track_fun_atoi(cmd->pars[1]);
#if defined(__GT740__)||defined(__GT420D__)
        if (hbt_acc_on < 1 || hbt_acc_on > 7)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (1-7)");
            return;
        }
#else
        if (hbt_acc_on < 1 || hbt_acc_on > 300)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (1-300)");
            return;
        }
#endif
        hbt_acc_on *= 60;
        hbt_acc_off = hbt_acc_on;
    }
#else
    if (cmd->part == 0)
    {
        if (G_parameter.hbt_acc_on % 60 == 0 && G_parameter.hbt_acc_off % 60 == 0)
            sprintf(cmd->rsp_msg, "%s:%dmin, %dmin", cmd->pars[0], G_parameter.hbt_acc_on / 60, G_parameter.hbt_acc_off / 60);
        else
            sprintf(cmd->rsp_msg, "%s:%dsec, %dsec", cmd->pars[0], G_parameter.hbt_acc_on, G_parameter.hbt_acc_off);
        return;
    }
    else if (cmd->part > 2)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (cmd->pars[1][0] == 'S')
    {
        hbt_acc_on = track_fun_atoi(&cmd->pars[1][1]);
        if (hbt_acc_on < 10 || hbt_acc_on > 18000)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (10sec - 18000sec)");
            return;
        }
    }
    else
    {
        hbt_acc_on = track_fun_atoi(cmd->pars[1]);
        if (hbt_acc_on < 1 || hbt_acc_on > 300)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (1min - 300min)");
            return;
        }
        hbt_acc_on *= 60;
    }
    hbt_acc_off = hbt_acc_on;
    if (cmd->part == 2)
    {
        if (cmd->pars[2][0] == 'S')
        {
            hbt_acc_off = track_fun_atoi(&cmd->pars[2][1]);
            if (hbt_acc_off < 1 || hbt_acc_off > 18000)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (10sec - 18000sec)");
                return;
            }
        }
        else
        {
            hbt_acc_off = track_fun_atoi(cmd->pars[2]);
            if (hbt_acc_off < 1 || hbt_acc_off > 300)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (1min - 300min)");
                return;
            }
            hbt_acc_off *= 60;
        }        
    }
#endif /* __NO_ACC__ */
    G_parameter.hbt_acc_off = hbt_acc_off;
    G_parameter.hbt_acc_on = hbt_acc_on;
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
    track_stop_timer(TRACK_CUST_NET_HEARTBEAT_OVER_TIMER_ID);
    heartbeat_packets((void *)112);
}

/******************************************************************************
 *  Function    -  cmd_report
 *
 *  Purpose     -  定位上报方式设置
 *
 *  Description -
        REPORT,<0>,<T1>[,T2]#              // 定时上传
        REPORT,<1>,<D>#                    // 定距上传
        REPORT,<2>,<T1>,<T2>,<D>#          // 定时+定距 开
        REPORT,<3>#                        // 定时+定距 关
            X=0/1；0 定时上传，1 定距上传，2 定时+定距 开，3 定时+定距 关；默认值 0;
            T1=0、5 - 18000秒；ACC ON 状态下上传间隔；默认值：10
            T2=0、5 - 18000秒；ACC OFF 状态下上传间隔；默认值：0
            D=0、50 - 10000米；定距距离；默认值：300
        REPORT#
            查询当前是定时上传还是定距上传，及对应的参数
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 08-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_report(CMD_DATA_STRUCT *cmd)
{
    nvram_gps_work_struct tmp = {0};
    int value = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "Timing report:(%s, acc on:%d, acc off:%d); Fixed pitch:(%s, distance:%d)",
                ON_OFF(G_parameter.gps_work.Fti.sw), G_parameter.gps_work.Fti.interval_acc_on, G_parameter.gps_work.Fti.interval_acc_off,
                ON_OFF(G_parameter.gps_work.Fd.sw), G_parameter.gps_work.Fd.distance);
        return;
    }
    value = track_fun_atoi(cmd->pars[1]);
    if (value < 0 || value > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0-1)");
        return;
    }
    memcpy(&tmp, &G_parameter.gps_work, sizeof(nvram_gps_work_struct));
    switch (value)
    {
        case 0:
            tmp.Fti.sw = 1;
            tmp.Fd.sw = 0;
            if (cmd->part < 1 || cmd->part > 3)
            {
                sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
                return;
            }
            if (cmd->part >= 2)
            {
                value = track_fun_atoi(cmd->pars[2]);
                if ((value < 5 || value > 18000) && value != 0)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 2 (0, 5-18000)");
                    return;
                }
                tmp.Fti.interval_acc_on = value;
            }
            if (cmd->part == 3)
            {
                value = track_fun_atoi(cmd->pars[3]);
                if ((value < 5 || value > 18000) && value != 0)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 3 (0, 5-18000)");
                    return;
                }
                tmp.Fti.interval_acc_off = value;
            }
            break;
        case 1:
            tmp.Fti.sw = 0;
            tmp.Fd.sw = 1;
            if (cmd->part < 1 || cmd->part > 2)
            {
                sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
                return;
            }
            if (cmd->part >= 2)
            {
                value = track_fun_atoi(cmd->pars[2]);
                if ((value < 50 || value > 10000) && value != 0)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 2 (0, 50-10000)");
                    return;
                }
                tmp.Fd.distance = value;
            }
            break;
        case 2:
            tmp.Fti.sw = 1;
            tmp.Fd.sw = 1;
            if (cmd->part < 1 || cmd->part > 4)
            {
                sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
                return;
            }
            if (cmd->part >= 2)
            {
                value = track_fun_atoi(cmd->pars[2]);
                if (value < 0 || value > 18000)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 2 (5-18000)");
                    return;
                }
                tmp.Fti.interval_acc_on = value;
            }
            if (cmd->part >= 3)
            {
                value = track_fun_atoi(cmd->pars[3]);
                if (value < 0 || value > 18000)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 3 (5-18000)");
                    return;
                }
                tmp.Fti.interval_acc_off = value;
            }
            if (cmd->part >= 4)
            {
                value = track_fun_atoi(cmd->pars[4]);
                if ((value < 50 || value > 10000) && value != 0)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 4 (0,50-10000)");
                    return;
                }
                tmp.Fd.distance = value;
            }
            break;
        case 3:
            tmp.Fti.sw = 0;
            tmp.Fd.sw = 0;
            if (cmd->part != 1)
            {
                sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
                return;
            }
            break;
    }
    memcpy(&G_parameter.gps_work, &tmp, sizeof(nvram_gps_work_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}
#if defined(__GT740__)||defined(__GT420D__)
#elif defined( __GT03D__)||defined(__GT300__) || defined(__GW100__)

static void cmd_gt03_timer(CMD_DATA_STRUCT *cmd)
{
    nvram_gps_work_struct tmp = {0};
    int value = 0;
    kal_bool flag = 1;
    if (cmd->part == 0)
    {
#if (defined (__GT300__) || defined (__GT03D__)||defined(__GT300S__)) && !defined(__GW100__)
        sprintf(cmd->rsp_msg, "%s:%d,%d,%d", cmd->pars[0],
                G_parameter.gps_work.Fti.lbs_interval, G_parameter.gps_work.Fti.interval_acc_off, G_parameter.gps_work.Fti.mode1_gps_time);
#else
sprintf(cmd->rsp_msg, "%s:%d,%d", cmd->pars[0],
        G_parameter.gps_work.Fti.lbs_interval, G_parameter.gps_work.Fti.interval_acc_off);
#endif /* __GT300__ */
        return;
    }
    memcpy(&tmp, &G_parameter.gps_work, sizeof(nvram_gps_work_struct));
    tmp.Fti.sw = 1;
    tmp.Fd.sw = 0;
#if (defined (__GT300__) || defined (__GT03D__)||defined(__GT300S__)) && !defined(__GW100__)
    if (cmd->part > 3)
#else
if (cmd->part > 2)
#endif /* __GT300__ */
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (strlen(cmd->pars[1]) > 0)
    {
        value = track_fun_atoi(cmd->pars[1]);
#if defined (__GW100__)
        if ((value < 0 || value > 60))
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (0-60)");
            return;
        }
#else
if ((value < 1 || value > 60))
{
    sprintf(cmd->rsp_msg, "Error: Parameter 1 (1-60)");
    return;
}
#endif /* __GW100__ */
        tmp.Fti.lbs_interval = value;
        flag = 0;
    }
#if (defined (__GT300__) || defined (__GT03D__)||defined(__GT300S__)) && !defined(__GW100__)
    if (cmd->part >= 2 && strlen(cmd->pars[2]) > 0)
    {
        value = track_fun_atoi(cmd->pars[2]);
        if ((value < 5 || value > 18000) && value != 0)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (0, 5-18000)");
            return;
        }
        tmp.Fti.interval_acc_off = value;
        tmp.Fti.interval_acc_on = value;
        flag = 0;
    }

    if (cmd->part == 3 && strlen(cmd->pars[3]) > 0)
    {
        value = track_fun_atoi(cmd->pars[3]);
        if ((value < 5 || value > 300))
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 3 (5-300)");
            return;
        }
        tmp.Fti.mode1_gps_time = value;
        flag = 0;
    }
#else
if (cmd->part == 2 && strlen(cmd->pars[2]) > 0)
{
    value = track_fun_atoi(cmd->pars[2]);
    if ((value < 5 || value > 18000) && value != 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 2 (0, 5-18000)");
        return;
    }
    tmp.Fti.interval_acc_off = value;
    tmp.Fti.interval_acc_on = value;
    flag = 0;
}
#endif /* __GT300__ */
    if (flag)
    {
        sprintf(cmd->rsp_msg, "Error: Parameters are as empty!");
        return;
    }
    else
    {
        memcpy(&G_parameter.gps_work, &tmp, sizeof(nvram_gps_work_struct));
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
#if (defined (__GT300__) || defined (__GT03D__)||defined(__GT300S__)) && !defined(__GW100__)
        if (track_cust_get_work_mode() == WORK_MODE_1)
        {
            track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID, G_parameter.gps_work.Fti.mode1_gps_time * 60 * 1000, track_cust_gps_work_req, (void *)13);
        }
        else
        {
            tr_stop_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID);
            tr_stop_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_OVERTIME_TIMER_ID);
        }
#endif /* __GT300__ */
        sprintf(cmd->rsp_msg, "OK!");
    }
}
#else
static void cmd_timer(CMD_DATA_STRUCT *cmd)
{
    nvram_gps_work_struct tmp = {0};
    int value = 0;
    kal_bool flag = 1;
    if (cmd->part == 0)
    {
        //这个值在check同步到服务器，但是没按项目分别处理  chengj
#if defined(__GPS_INTERMITTENT_START__)&&!(defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__))
        sprintf(cmd->rsp_msg, "%s:%d,%d,%d", cmd->pars[0],
                G_parameter.gps_work.Fti.lbs_interval, G_parameter.gps_work.Fti.interval_acc_off, G_parameter.gps_work.Fti.mode1_gps_time);
#elif defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], G_parameter.gps_work.Fti.lbs_interval);
#elif defined(__NT33__)

#if defined (__CUSTOM_DDWL__)
//GPS上报间隔，单位都是秒,只要一个参数
sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], G_parameter.gps_work.Fti.interval_acc_off);
#elif defined (__FUN_WITH_ACC__)
sprintf(cmd->rsp_msg, "%s:%d,%d", cmd->pars[0],
        G_parameter.gps_work.Fti.interval_acc_on, G_parameter.gps_work.Fti.interval_acc_off);
#else
//GPS和LBS上报间隔，单位都是秒
sprintf(cmd->rsp_msg, "%s:%d,%d", cmd->pars[0],
        G_parameter.gps_work.Fti.interval_acc_off, G_parameter.gps_work.Fti.lbs_interval);
#endif /* __CUSTOM_DDWL__ */

#elif defined(__NO_ACC__)
sprintf(cmd->rsp_msg, "%s:%d,%d", cmd->pars[0],
        G_parameter.gps_work.Fti.lbs_interval, G_parameter.gps_work.Fti.interval_acc_off);
#else
sprintf(cmd->rsp_msg, "%s:%d,%d", cmd->pars[0],
        G_parameter.gps_work.Fti.interval_acc_on, G_parameter.gps_work.Fti.interval_acc_off);
#endif /* __NO_ACC__ */
        return;
    }
    memcpy(&tmp, &G_parameter.gps_work, sizeof(nvram_gps_work_struct));
    tmp.Fti.sw = 1;
    tmp.Fd.sw = 0;

#if defined (__NT33__) && defined (__CUSTOM_DDWL__)
    //定制项目，只要1个参数
    if (cmd->part != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }

#else
if (cmd->part > 2)
{
    sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
    return;
}
#endif

#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
    if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (strlen(cmd->pars[1]) > 0)
    {
        value = track_fun_atoi(cmd->pars[1]);
        if ((value < 1 || value > 60))
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (1-60)");
            return;
        }
        tmp.Fti.lbs_interval = value;
        flag = 0;
    }
#elif defined(__GPS_INTERMITTENT_START__)
if (strlen(cmd->pars[1]) > 0)
{
    value = track_fun_atoi(cmd->pars[1]);
    if ((value < 1 || value > 60))
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (1-60)");
        return;
    }
    tmp.Fti.lbs_interval = value;
    flag = 0;
}
if (cmd->part >= 2 && strlen(cmd->pars[2]) > 0)
{
    value = track_fun_atoi(cmd->pars[2]);
    if ((value < 5 || value > 18000) && value != 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 2 (0, 5-18000)");
        return;
    }
    tmp.Fti.interval_acc_off = value;
    tmp.Fti.interval_acc_on = value;
    flag = 0;
}
if (cmd->part >= 3 && strlen(cmd->pars[3]) > 0)
{
    value = track_fun_atoi(cmd->pars[3]);
    if ((value < 5 || value > 300))
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 3 (5-300)");
        return;
    }
    tmp.Fti.mode1_gps_time = value;
    flag = 0;
}
#elif defined (__NT33__)

#if defined (__CUSTOM_DDWL__)
if (strlen(cmd->pars[1]) > 0)
{
    value = track_fun_atoi(cmd->pars[1]);
    if ((value < 5 || value > 18000) && value != 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0, 5-18000)");
        return;
    }
    tmp.Fti.interval_acc_off = value;
    tmp.Fti.interval_acc_on = value;
    tmp.Fti.lbs_interval = value;
    flag = 0;
}
else
{
    //后面有处理
}

#elif defined (__FUN_WITH_ACC__)
if (strlen(cmd->pars[1]) > 0)
{
    value = track_fun_atoi(cmd->pars[1]);
    if ((value < 5 || value > 18000) && value != 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0, 5-18000)");
        return;
    }
    tmp.Fti.interval_acc_on = value;
    flag = 0;
}
if (cmd->part >= 2 && strlen(cmd->pars[2]) > 0)
{
    value = track_fun_atoi(cmd->pars[2]);
    if ((value < 5 || value > 18000) && value != 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 2 (0, 5-18000)");
        return;
    }
    tmp.Fti.interval_acc_off = value;
    flag = 0;
}
#else
if (strlen(cmd->pars[1]) > 0)
{
    value = track_fun_atoi(cmd->pars[1]);
    if ((value < 5 || value > 18000) && value != 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0, 5-18000)");
        return;
    }
    tmp.Fti.interval_acc_off = value;
    tmp.Fti.interval_acc_on = value;
    flag = 0;
}

if ((cmd->part >= 2) && (strlen(cmd->pars[2]) > 0))
{
    value = track_fun_atoi(cmd->pars[2]);
    if ((value < 5 || value > 18000) && value != 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 2 (0, 5-18000)");
        return;
    }
    tmp.Fti.lbs_interval = value;
    flag = 0;
}
#endif /* __CUSTOM_DDWL__ */

#elif defined(__NO_ACC__)
if (strlen(cmd->pars[1]) > 0)
{
    value = track_fun_atoi(cmd->pars[1]);
    if ((value < 0 || value > 60))
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0-60)");
        return;
    }
    tmp.Fti.lbs_interval = value;
    flag = 0;
}
if (cmd->part >= 2 && strlen(cmd->pars[2]) > 0)
{
    value = track_fun_atoi(cmd->pars[2]);
    if ((value < 5 || value > 18000) && value != 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 2 (0, 5-18000)");
        return;
    }
    tmp.Fti.interval_acc_off = value;
    tmp.Fti.interval_acc_on = value;
    flag = 0;
}
#else
if (strlen(cmd->pars[1]) > 0)
{
    value = track_fun_atoi(cmd->pars[1]);
    if ((value < 5 || value > 18000) && value != 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0, 5-18000)");
        return;
    }
    tmp.Fti.interval_acc_on = value;
    flag = 0;
}
if (cmd->part >= 2 && strlen(cmd->pars[2]) > 0)
{
    value = track_fun_atoi(cmd->pars[2]);
    if ((value < 5 || value > 18000) && value != 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 2 (0, 5-18000)");
        return;
    }
    tmp.Fti.interval_acc_off = value;
    flag = 0;
}
#endif /* __NO_ACC__ */
    if (flag)
    {
        sprintf(cmd->rsp_msg, "Error: Parameters are as empty!");
        return;
    }
    else
    {
        memcpy(&G_parameter.gps_work, &tmp, sizeof(nvram_gps_work_struct));
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
#if defined(__GPS_INTERMITTENT_START__)
        if (track_cust_get_work_mode() == WORK_MODE_1)
        {
#if defined (__GT300S__)
            track_mode1_gt300s_reset_work();
#elif defined(__GT370__)|| defined (__GT380__)

#else
track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID, G_parameter.gps_work.Fti.mode1_gps_time * 60 * 1000, track_cust_gps_work_req, (void *)13);
#endif /* __GT300S__ */
        }
        else
        {
            tr_stop_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID);
            tr_stop_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_OVERTIME_TIMER_ID);
        }
#endif /* __GPS_INTERMITTENT_START__ */
        sprintf(cmd->rsp_msg, "OK!");
    }
}
#endif

/******************************************************************************
 *  Function    -  cmd_anglerep
 *
 *  Purpose     -  拐点补传
 *
 *  Description -
        ANGLEREP,<X>,<A>,<B>#
            X=ON/OFF，默认值：ON
            A=30 - 180度，默认值：60度，偏转角度
            B=2 - 10秒，默认值：5秒，检测时间
        ANGLEREP,OFF#
            关闭拐点补传
        ANGLEREP#
            查询拐点补传功能是否启用，对应的参数设置值
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 08-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_anglerep(CMD_DATA_STRUCT *cmd)
{
    nvram_gps_anglerep_struct anglerep = {0};
    int value = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d,%d", cmd->pars[0],
                ON_OFF(G_parameter.gps_work.anglerep.sw),
                G_parameter.gps_work.anglerep.angleOfDeflection,
                G_parameter.gps_work.anglerep.detection_time);
        return;
    }
    memcpy(&anglerep, &G_parameter.gps_work.anglerep, sizeof(nvram_gps_anglerep_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part > 3)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        anglerep.sw = 1;
        if (cmd->part >= 2)
        {
            if (strlen(cmd->pars[2]) > 0)
            {
                value = track_fun_atoi(cmd->pars[2]);
                if (value < 5 || value > 180)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 2 (5-180)");
                    return;
                }
                anglerep.angleOfDeflection = value;
            }
            else
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (5-180)");
                return;
            }
        }
        if (cmd->part == 3)
        {
            if (strlen(cmd->pars[3]) > 0)
            {
                value = track_fun_atoi(cmd->pars[3]);
                if (value < 2 || value > 5)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 3 (2-5)");
                    return;
                }
                anglerep.detection_time = value;
            }
            else
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (2-5)");
                return;
            }
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        anglerep.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.gps_work.anglerep, &anglerep, sizeof(nvram_gps_anglerep_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

#if !defined(__GT740__) && !defined(__GT420D__)
/******************************************************************************
 *  Function    -  cmd_sends
 *
 *  Purpose     -  SENSOR 控制 GPS 时间
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_sends(CMD_DATA_STRUCT *cmd)
{
    int value = 0;
#ifdef __GT02__
    if (!G_importance_parameter_data.app_version.support_sensor)
    {
        return;
    }
#endif /* __GT02__ */

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], G_parameter.sensor_gps_work);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    value = track_fun_atoi(cmd->pars[1]);
    if (value < 0 || value > 300)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0,1-300)");
        return;
    }
    if (G_parameter.sensor_gps_work != value)
    {
        G_parameter.sensor_gps_work = value;
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        track_cust_gps_cmd_change_set();
    }
    sprintf(cmd->rsp_msg, "OK!");
}

static void cmd_hst(CMD_DATA_STRUCT *cmd)
{
    int value = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0],
                G_parameter.speed_limit);
        return;
    }

    if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    value = track_fun_atoi(cmd->pars[1]);
    if (value < 0 || value > 255)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0-255)");
        return;
    }
    G_parameter.speed_limit = value;
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

static void cmd_PWRONALM(CMD_DATA_STRUCT *cmd)
{
    nvram_boot_struct alarm = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d", cmd->pars[0],
                ON_OFF(G_parameter.boot.sw), G_parameter.boot.alarm_type);
        return;
    }
    memcpy(&alarm, &G_parameter.boot, sizeof(nvram_boot_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part < 1 || cmd->part > 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 1;
        if (cmd->part >= 2)
        {
            alarm.alarm_type = track_fun_atoi(cmd->pars[2]);
            if (alarm.alarm_type != 0 && alarm.alarm_type != 1)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1)");
                return;
            }
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.boot, &alarm, sizeof(nvram_boot_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

static void cmd_PWROFFALM(CMD_DATA_STRUCT *cmd)
{
    nvram_boot_struct alarm = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d", cmd->pars[0],
                ON_OFF(G_parameter.pwroff.sw), G_parameter.pwroff.alarm_type);
        return;
    }
    memcpy(&alarm, &G_parameter.pwroff, sizeof(nvram_boot_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part < 1 || cmd->part > 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 1;
        if (cmd->part >= 2)
        {
            alarm.alarm_type = track_fun_atoi(cmd->pars[2]);
            if (alarm.alarm_type != 0 && alarm.alarm_type != 1)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1)");
                return;
            }
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.pwroff, &alarm, sizeof(nvram_boot_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

static void cmd_simalm(CMD_DATA_STRUCT *cmd)
{
    int value = 0;
    nvram_sim_alm_struct alarm = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s mode:%d(0=only GPRS,1=GPRS+SMS)", cmd->pars[0],
                ON_OFF(G_parameter.sim_alarm.sw), G_parameter.sim_alarm.simalm_type);
        LOGD(L_CMD, L_V5, "IMSI=%s", G_parameter.sim_alarm.sim_imsi);
        return;
    }
    memcpy(&alarm, &G_parameter.sim_alarm, sizeof(nvram_sim_alm_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part != 1 && cmd->part != 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 1;
        if (cmd->part == 2)
        {
            if (!strcmp(cmd->pars[2], "0"))
            {
                alarm.simalm_type = 0;
            }
            else if (!strcmp(cmd->pars[2], "1"))
            {
                alarm.simalm_type = 1;
            }
            else
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1)");
                return;
            }
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.sim_alarm, &alarm, sizeof(nvram_sos_alarm_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}
static void cmd_clearsim(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    sprintf(G_parameter.sim_alarm.sim_imsi, "%s", "0000000000000000");
    G_realtime_data.sim_alm = 0;
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}
static void cmd_pwrlimit(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0],
                ON_OFF(G_parameter.pwrlimit_sw));
        return;
    }
    if (cmd->supercmd != 1 && cmd->return_sms.cm_type == CM_SMS && track_cust_sos_or_center_check(3, cmd->return_sms.ph_num) != 9)
    {
#if defined(__GT370__)|| defined (__GT380__)
        sprintf(cmd->rsp_msg, "Error! Only SOS phone numbers are allowed to set.");
#else
#if defined(__GERMANY__)
        sprintf(cmd->rsp_msg, "Fehler! Nur die SOS- und Center-Nr.kann Befehl senden.");

#else
        sprintf(cmd->rsp_msg, "Error! Only center phone number and SOS phone numbers are allowed to set.");
#endif/*__GERMANY__*/
#endif
        return;
    }
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        G_parameter.pwrlimit_sw = 1;
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        G_parameter.pwrlimit_sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
    track_cust_module_sw_init();
#endif /* __GT300S__ */

}
static void cmd_almsms(CMD_DATA_STRUCT *cmd)
{
    int AlarmNumCount;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], G_parameter.Permission.AlarmNumCount);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }

    AlarmNumCount = track_fun_atoi(cmd->pars[1]);
    if (AlarmNumCount > 1 || AlarmNumCount < 0)
    {
        sprintf(cmd->rsp_msg, "Error: Value only in from 0 to 1!");
        return;
    }
    G_parameter.Permission.AlarmNumCount = AlarmNumCount;
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }

}

static void cmd_smsclear(CMD_DATA_STRUCT *cmd)
{
    if (track_cust_status_sim() == 1)
    {
        track_sim_clean_allsms();
        sprintf(cmd->rsp_msg, "OK!");
    }
}

/* BALANCE,*888## */
static void cmd_balance(CMD_DATA_STRUCT *cmd)
{
    char *p;
    int len;
    if (cmd->part < 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error! %d", cmd->part);
        return;
    }
    p = cmd->rcv_msg_source + (cmd->pars[1] - cmd->rcv_msg);
    len = strlen(p);
    if (len > 318)
    {
        len = 318;
    }
    if (p[len - 2] == 0x0D && p[len - 1] == 0x0A)
    {
        len -= 2;
    }
    else if (p[len - 1] == 0x0D)
    {
        len--;
    }
    if (p[len - 1] == '#')
    {
        len--;
    }
    track_ussd_req(cmd, p, len);

    sprintf(cmd->rsp_msg, "The request is issued, please receive the return result later.");
}

/******************************************************************************
 *  Function    -  cmd_tql
 *
 *  Purpose     -  电话查询位置
 *
 *  Description -
        TQL,<X>#
            X=ON/OFF；ON:开启电话查询位置；OFF:关闭电话查询位置；默认开关状态为OFF
        TQL#
            查询总里程，及设置的初值
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 11-25-2014,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_tql(CMD_DATA_STRUCT *cmd)
{
    U8 call_get_addr_sw = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], ON_OFF(G_parameter.call_get_addr_sw));
        return;
    }
    call_get_addr_sw = G_parameter.call_get_addr_sw;
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        call_get_addr_sw = 1;
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        call_get_addr_sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    G_parameter.call_get_addr_sw = call_get_addr_sw;
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    sprintf(cmd->rsp_msg, "OK!");
}
/******************************************************************************
 *  Function    -  cmd_center
 *
 *  Purpose     -  中心号码设置
 *
 *  Description -
        CENTER,A,<中心号码>#
            添加中心号码
        CENTER,D#
            删除中心号码
        CENTER#
            查询设置的中心号码
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_center(CMD_DATA_STRUCT *cmd)
{
    LOGD(L_APP, L_V5, "");
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], G_parameter.centerNumber);
        return;
    }
    if (cmd->supercmd != 1 && cmd->return_sms.cm_type == CM_SMS)
    {
        if (G_parameter.Permission.Permit == 0 && track_cust_sos_or_center_check(3, cmd->return_sms.ph_num) != 9)
        {
#if defined(__GERMANY__)
            sprintf(cmd->rsp_msg, "Fehler! Nur die SOS- und Center-Nr.kann Befehl senden.");
#else
            sprintf(cmd->rsp_msg, "Error! Only center phone number and SOS phone numbers are allowed to set.");
#endif
            return;
        }
        else if (G_parameter.Permission.Permit == 1 && track_cust_sos_or_center_check(1, cmd->return_sms.ph_num) == 1)
        {
            sprintf(cmd->rsp_msg, "Error! Only center phone number to set.");
            return;
        }
    }

    if (!strcmp(cmd->pars[1], "A"))
    {
        int len, i;
        if (cmd->part != 2)
        {
#if defined (__GERMANY__)
            sprintf(cmd->rsp_msg, "Fehler: Max. 1 Nummern eingeben!");
#else
            sprintf(cmd->rsp_msg, "Error: At most once to add one numbers!");
#endif
            return;
        }
        len = strlen(cmd->pars[2]);
        if (len > 0)
        {
            if (track_fun_check_str_is_number(1, cmd->pars[2]) == 0)
            {
                sprintf(cmd->rsp_msg, "Error: Number contains non-numeric characters!");
                return;
            }
            if (len > 20)
            {
                sprintf(cmd->rsp_msg, "Error: Number length exceeds 20 characters!");
                return;
            }
            else if (len < 3)
            {
#if defined (__GERMANY__)
                sprintf(cmd->rsp_msg, "Fehler: Mindestens 3 Zeichen eingeben!");
#else
                sprintf(cmd->rsp_msg, "Error: Number length must exceeds 3 characters!");
#endif
                return;
            }
            strcpy(G_parameter.centerNumber, cmd->pars[2]);
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
#if defined (__GERMANY__)
            sprintf(cmd->rsp_msg, "OK-\r\nCenter Nummer:%s", G_parameter.centerNumber);
#else
            sprintf(cmd->rsp_msg, "OK\r\nCenter Number:%s", G_parameter.centerNumber);
#endif
        }
    }
    else if (!strcmp(cmd->pars[1], "D"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        memset(G_parameter.centerNumber, 0, TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
#if defined (__GERMANY__)
        sprintf(cmd->rsp_msg, "OK-\r\nCenter Nummer:%s", G_parameter.centerNumber);
#else
        sprintf(cmd->rsp_msg, "OK\r\nCenter Number:%s", G_parameter.centerNumber);
#endif
    }
#if defined(__ET310__)||defined (__ET320__)|| defined(__GT370__)|| defined (__GT380__)|| defined (__NT37__)|| defined (__NT33__)
    track_cust_paket_9404_handle();
#endif
}

/******************************************************************************
 *  Function    -  cmd_monpermit
 *
 *  Purpose     -  监听权限控制
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 21-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_monpermit(CMD_DATA_STRUCT *cmd)
{
    int value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], G_parameter.Permission.MonPermit);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (cmd->supercmd != 1 && cmd->return_sms.cm_type == CM_SMS && track_cust_sos_or_center_check(3, cmd->return_sms.ph_num) != 9)
    {
#if defined(__GERMANY__)
        sprintf(cmd->rsp_msg, "Fehler! Nur die SOS- und Center-Nr.kann Befehl senden.");

#else
        sprintf(cmd->rsp_msg, "Error! Only center phone number and SOS phone numbers are allowed to set.");
#endif/*__GERMANY__*/
        return;
    }
    value = track_fun_atoi(cmd->pars[1]);
    if (value < 0 || value > 3)
    {
        sprintf(cmd->rsp_msg, "Error: Value only in from 0 to 3!");
        return;
    }
    G_parameter.Permission.MonPermit = value;
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

static void cmd_permit(CMD_DATA_STRUCT *cmd)
{
    int value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], G_parameter.Permission.Permit);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (cmd->supercmd != 1 && cmd->return_sms.cm_type == CM_SMS && track_cust_sos_or_center_check(3, cmd->return_sms.ph_num) != 9)
    {
#if defined (__GT370__)||defined(__GT380__)
        sprintf(cmd->rsp_msg, "Error! Only SOS phone numbers are allowed to set.");
#else
        sprintf(cmd->rsp_msg, "Error! Only center phone number and SOS phone numbers are allowed to set.");
#endif

        return;
    }
    value = track_fun_atoi(cmd->pars[1]);
    if (value < 0 || value > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Value only in from 0 to 1!");
        return;
    }
    G_parameter.Permission.Permit = value;
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

/******************************************************************************
 *  Function    -  cmd_sospermit
 *
 *  Purpose     -  SOS权限设置指令
 *
 *  Description -
        SOSPERMIT,<A>[,B]#
            A=0-1；参数设置查询权限范围；默认值：0
            B=0-1；报警号码权限；  默认值：1
        SOSPERMIT#
            查询 SOS 号码参数设置查询权限
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 19-03-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_sospermit(CMD_DATA_STRUCT *cmd)
{
    int SOSPermit, AlarmNumCount;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d,%d", cmd->pars[0], G_parameter.Permission.SOSPermit, G_parameter.Permission.AlarmNumCount);
        return;
    }
    else if (cmd->part > 2)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (cmd->supercmd != 1 && cmd->return_sms.cm_type == CM_SMS && track_cust_sos_or_center_check(3, cmd->return_sms.ph_num) != 9)
    {
#if defined(__GT500__)  ||defined(__GT03D__)
        sprintf(cmd->rsp_msg, "Error! Only SOS phone numbers are allowed to set.");
#else
#if defined(__GERMANY__)
        sprintf(cmd->rsp_msg, "Fehler! Nur die SOS- und Center-Nr. kann Befehl senden.");
#else
        sprintf(cmd->rsp_msg, "Error! Only center phone number and SOS phone numbers are allowed to set.");
#endif/*__GERMANY__*/
#endif
        return;
    }
    SOSPermit = track_fun_atoi(cmd->pars[1]);
    if (SOSPermit < 0 || SOSPermit > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Value only in from 0 to 1!");
        return;
    }
    if (cmd->part == 2)
    {
        AlarmNumCount = track_fun_atoi(cmd->pars[2]);
        if (AlarmNumCount < 0 || AlarmNumCount > 1)
        {
            sprintf(cmd->rsp_msg, "Error: Value only in from 0 to 1!");
            return;
        }
        G_parameter.Permission.AlarmNumCount = AlarmNumCount;
    }
    G_parameter.Permission.SOSPermit = SOSPermit;
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}


/******************************************************************************
 *  Function    -  cmd_sensorset
 *
 *  Purpose     -  触发震动报警条件
 *
 *  Description -
        SENSORALM,<A>,<B>,<C>[,D]#
            A=1-300 秒；SENSOR检测时间范围；默认值：10秒
            B=1-300 次；震动激活GPS触发次数门限；默认值：1次
            C=1-300 次；震动触发次数门限；默认值：3次
            D=1-4 秒;   SENSOR 检测间隔（C 秒），默认值：1

            限制条件：A ≥ C * D ≥ B * D
        SENSORALM#
            查询设置的震动报警触发条件
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 13-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_sensorset(CMD_DATA_STRUCT *cmd)
{
    nvram_vibrates_alarm_struct alarm = {0};
    nvram_vibrates_start_gps_struct vibrates = {0};
    int value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d,%d,%d,%d", cmd->pars[0],
                G_parameter.vibrates_alarm.detection_time,
                G_parameter.gps_work.vibrates.detection_count,
                G_parameter.vibrates_alarm.detection_count,
                G_parameter.vibrates_alarm.sampling_interval);
        return;
    }
    else if (cmd->part > 4)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    memcpy(&alarm, &G_parameter.vibrates_alarm, sizeof(nvram_vibrates_alarm_struct));
    memcpy(&vibrates, &G_parameter.gps_work.vibrates, sizeof(nvram_vibrates_start_gps_struct));
    value  = track_fun_atoi(cmd->pars[1]);
#if defined(__ET310__)||defined (__ET320__)||defined(__MT200__)
    if (value < 1 || value > 300)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (1 to 300)");
        return;
    }
#else
    if (value < 10 || value > 300)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (10 to 300)");
        return;
    }
#endif /* __ET310__ */
    alarm.detection_time = value;
    vibrates.detection_time = value;
    if (cmd->part > 1)
    {
        value = track_fun_atoi(cmd->pars[2]);
        if (value < 1 || value > 20)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (1 to 20)");
            return;
        }
        vibrates.detection_count = value;
    }
    if (cmd->part > 2)
    {
        value = track_fun_atoi(cmd->pars[3]);
        if (value < 1 || value > 20)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 3 (1 to 20)");
            return;
        }
        alarm.detection_count = value;
    }
    if (cmd->part > 3)
    {
        value = track_fun_atoi(cmd->pars[4]);
        if (value < 1 || value > 3)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 4 (1 to 3)");
            return;
        }
        alarm.sampling_interval = value;
    }
    if (alarm.detection_time < alarm.detection_count * alarm.sampling_interval)
    {
#if defined(__GERMANY__)
        return_message(
            cmd,
            "Fehler! Eintrag 3 und 4 sollen geringer o. gleich als Eintrag 1 sein.",
            "错误！参数3和参数4之乘积应不大于参数1。");
#else
        return_message(
            cmd,
            "Error! The parameters 3 and parameters 4 of the product should be less than or equal to the value of parameter 1.",
            "错误！参数3和参数4之乘积应不大于参数1。");
#endif
        return;
    }
    else if (alarm.detection_count < vibrates.detection_count)
    {
#if defined(__GERMANY__)
        return_message(
            cmd,
            "Fehler! Eintrag 2 sollte geringer o. gleich als Eintrag 3 sein.",
            "错误！参数2应不大于参数3。");
#else
        return_message(
            cmd,
            "Error! The value of parameters 2 should be less than or equal to the value of parameter 3.",
            "错误！参数2应不大于参数3。");
#endif
        return;
    }
#if defined(__JM81__)
    alarm.sensor_sw = 1;
#endif
    memcpy(&G_parameter.vibrates_alarm, &alarm, sizeof(nvram_vibrates_alarm_struct));
    memcpy(&G_parameter.gps_work.vibrates, &vibrates, sizeof(nvram_vibrates_start_gps_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
#if defined(__JM81__)
    track_cust_mlsp_set_sensor();
#endif
}

/******************************************************************************
 *  Function    -  cmd_sensor
 *
 *  Purpose     -  震动参数设置
 *
 *  Description -  SENSOR,<A>[,B][,C]#
 *      A : 检测时间（秒）        取值范围：10-300  默认值：10
 *      B : 报警延时（秒）        取值范围：10-300  默认值：180
 *      C : 最短报警间隔（分）    取值范围：1-3000  默认值：30
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 27-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_sensor2(CMD_DATA_STRUCT *cmd)
{
    nvram_vibrates_alarm_struct alarm = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "SENSOR:%d,%d,%d",
                G_parameter.vibrates_alarm.detection_time, G_parameter.vibrates_alarm.alarm_delay_time,
                G_parameter.vibrates_alarm.alarm_interval);
        return;
    }
    else if (cmd->part > 3)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    memcpy(&alarm, &G_parameter.vibrates_alarm, sizeof(nvram_vibrates_alarm_struct));
    alarm.detection_time = track_fun_atoi(cmd->pars[1]);
#if defined(__ET310__)||defined (__ET320__)||defined (__ET350__)
    if (alarm.detection_time < 1 || alarm.detection_time > 300)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (1 to 300)");
        return;
    }
#else
    if (alarm.detection_time < 10 || alarm.detection_time > 300)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (10 to 300)");
        return;
    }
#endif /* __ET310__ */
    if (cmd->part >= 2)
    {
        alarm.alarm_delay_time = track_fun_atoi(cmd->pars[2]);
#if defined(__ET310__) ||defined (__ET320__)|| defined(__MT200__)||defined (__ET350__)
        if (alarm.alarm_delay_time < 1 || alarm.alarm_delay_time > 300)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (1 to 300)");
            return;
        }
#elif defined (__NT33__)
        if (alarm.alarm_delay_time < 1 || alarm.alarm_delay_time > 10)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (1 to 10)");
            return;
        }
#else
        if (alarm.alarm_delay_time < 10 || alarm.alarm_delay_time > 300)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (10 to 300)");
            return;
        }
#endif
    }
    if (cmd->part >= 3)
    {
        alarm.alarm_interval = track_fun_atoi(cmd->pars[3]);
#if defined(__XCWS__)
        if (alarm.alarm_interval < 0 || alarm.alarm_interval > 3000)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 3 (0 to 3000)");
            return;
        }
#elif defined(__ET310__) || defined(__MT200__)||defined (__ET320__)
        if (alarm.alarm_interval < 10 || alarm.alarm_interval > 180000)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 3 (10 to 180000)");
            return;
        }
        track_cust_shocks_quiet_time_update(alarm.alarm_interval);  // 更新静默时间
#else
        if (alarm.alarm_interval < 1 || alarm.alarm_interval > 3000)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 3 (1 to 3000)");
            return;
        }
        track_cust_shocks_quiet_time_update(alarm.alarm_interval);  // 更新静默时间
#endif
    }
    memcpy(&G_parameter.vibrates_alarm, &alarm, sizeof(nvram_vibrates_alarm_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

static void cmd_sensor(CMD_DATA_STRUCT *cmd)
{
    nvram_vibrates_alarm_struct alarm = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "SENSOR:%d,%d,%d",
                G_parameter.vibrates_alarm.detection_time, G_parameter.vibrates_alarm.alarm_delay_time,
                G_parameter.vibrates_alarm.alarm_interval / 60);
        return;
    }
    else if (cmd->part > 3)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    memcpy(&alarm, &G_parameter.vibrates_alarm, sizeof(nvram_vibrates_alarm_struct));
    alarm.detection_time = track_fun_atoi(cmd->pars[1]);
#if defined(__ET310__)||defined (__ET320__)||defined(__MT200__)
    if (alarm.detection_time < 1 || alarm.detection_time > 300)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (1 to 300)");
        return;
    }
#else
    if (alarm.detection_time < 10 || alarm.detection_time > 300)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (10 to 300)");
        return;
    }
#endif /* __ET310__ */
    if (cmd->part >= 2)
    {
        alarm.alarm_delay_time = track_fun_atoi(cmd->pars[2]);
#if defined (__NT33__)
        if (alarm.alarm_delay_time < 1 || alarm.alarm_delay_time > 10)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (1 to 10)");
            return;
        }
#elif !defined(__BCA__) && (defined(__ET310__) || defined(__MT200__)||defined (__ET320__))
        if (alarm.alarm_delay_time < 1 || alarm.alarm_delay_time > 300)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (1 to 300)");
            return;
        }
#else
        if (alarm.alarm_delay_time < 10 || alarm.alarm_delay_time > 300)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (10 to 300)");
            return;
        }
#endif
    }
    if (cmd->part >= 3)
    {
        alarm.alarm_interval = track_fun_atoi(cmd->pars[3]);
#if defined(__XCWS__)
        if (alarm.alarm_interval < 0 || alarm.alarm_interval > 3000)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 3 (0 to 3000)");
            return;
        }
        alarm.alarm_interval = alarm.alarm_interval * 60;
#else
        if (alarm.alarm_interval < 1 || alarm.alarm_interval > 3000)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 3 (1 to 3000)");
            return;
        }
        alarm.alarm_interval = alarm.alarm_interval * 60;
        track_cust_shocks_quiet_time_update(alarm.alarm_interval);  // 更新静默时间
#endif
    }
    memcpy(&G_parameter.vibrates_alarm, &alarm, sizeof(nvram_vibrates_alarm_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}


/******************************************************************************
 *  Function    -  cmd_fence
 *
 *  Purpose     -  电子围栏参数设置
 *
 *  Description -
 *        FENCE,<B>,0,<D>,<E>,<F>[,X][,M]#
 *            圆形区域
 *            B=ON/OFF；开启/关闭围栏报警；默认值为：关闭
 *            D=圆心纬度
 *            E=圆心经度
 *            F=100 - 9999；围栏半径，单位：米
 *            X=IN/OUT；IN：入围栏报警，OUT：出围栏报警，为空：进/出围栏报警；默认为进出围栏都报警
 *            M=0/1；报警上报方式，0：仅GPRS，1：SMS+GPRS，默认为：1
 *        FENCE,<B>,1,<D>,<E>,<F>,<G>[,X][,M]#
 *            方形区域设置
 *            B=ON/OFF；开启/关闭围栏报警；默认值为：关闭
 *            D=坐标1纬度
 *            E=坐标1经度
 *            F=坐标2纬度
 *            G=坐标2经度
 *            X=0/1；0 入围栏报警，1 出围栏报警，2 进/出围栏报警；默认为2
 *            M=0/1；报警上报方式，0：仅GPRS，1：SMS+GPRS，默认为：1
 *        FENCE#
 *            查询围栏设置参数
 *
 *        FENCE#
 *        FENCE,OFF#
 *        FENCE,ON,0,,,1#
 *        FENCE,ON,0,23.10267,114.39671,1#
 *        FENCE,ON,0,23.10267,114.39671,1,2,1#
 *        FENCE,ON,1,23.10267,114.39671,23.20267,114.49671#
 *        FENCE,ON,1,23.10267,114.39671,23.20267,114.59671,2,1#
 *  modification history
 * ----------------------------------------
 * v1.0  , 25-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_fence(CMD_DATA_STRUCT *cmd)
{
    nvram_electronic_fence_struct fence = {0};
    track_gps_data_struct *gpsPoint = NULL;
    kal_bool autoLatLon = KAL_FALSE;
    int sn = 0, tmp, i = 1;

    if (cmd->part > 0 && !strcmp(cmd->pars[0], "SEEFENCE")) return;
    if (cmd->part == 0 || !strcmp(cmd->pars[0], "SEEFENCE"))
    {
        char NS[2] = {0}, EW[2] = {0}, NS2[2] = {0}, EW2[2] = {0}, in_out[10] = {0};
        float lat, lon, lat2, lon2;
        if (G_parameter.fence[sn].lat >= 0)
        {
            if (G_parameter.fence[sn].lat != 0) *NS = 'N';
            lat = G_parameter.fence[sn].lat;
        }
        else
        {
            if (G_parameter.fence[sn].lat != 0) *NS = 'S';
            lat = -G_parameter.fence[sn].lat;
        }

        if (G_parameter.fence[sn].lon >= 0)
        {
            if (G_parameter.fence[sn].lon != 0) *EW = 'E';
            lon = G_parameter.fence[sn].lon;
        }
        else
        {
            if (G_parameter.fence[sn].lon != 0) *EW = 'W';
            lon = -G_parameter.fence[sn].lon;
        }
        if (G_parameter.fence[sn].in_out == 0)
            sprintf(in_out, "IN");
        else if (G_parameter.fence[sn].in_out == 1)
            sprintf(in_out, "OUT");
        else if (G_parameter.fence[sn].in_out == 2)
            sprintf(in_out, "IN or OUT");
        if (G_parameter.fence[sn].square)
        {
            if (G_parameter.fence[sn].lat2 >= 0)
            {
                if (G_parameter.fence[sn].lat2 != 0) *NS2 = 'N';
                lat2 = G_parameter.fence[sn].lat2;
            }
            else
            {
                if (G_parameter.fence[sn].lat2 != 0) *NS2 = 'S';
                lat2 = -G_parameter.fence[sn].lat2;
            }
            if (G_parameter.fence[sn].lon2 >= 0)
            {
                if (G_parameter.fence[sn].lon2 != 0) *EW2 = 'E';
                lon2 = G_parameter.fence[sn].lon2;
            }
            else
            {
                if (G_parameter.fence[sn].lon2 != 0) *EW2 = 'W';
                lon2 = -G_parameter.fence[sn].lon2;
            }
#if defined(__GERMANY__)
            sprintf(cmd->rsp_msg, "GeoFence Typ:Square, %s, Latitude1:%s%.6f, Longitude1:%s%.6f, Latitude2:%s%.6f, Longitude2:%s%.6f, REIN RAUS:%s, Alarm typ:%d",
                    ON_OFF(G_parameter.fence[sn].sw),
                    NS, lat, EW, lon,
                    NS2, lat2, EW2, lon2,
                    in_out, G_parameter.fence[sn].alarm_type);
#else
            sprintf(cmd->rsp_msg, "FenceType:Square, %s, Latitude1:%s%.6f, Longitude1:%s%.6f, Latitude2:%s%.6f, Longitude2:%s%.6f, in out:%s, alarm type:%d",
                    ON_OFF(G_parameter.fence[sn].sw),
                    NS, lat, EW, lon,
                    NS2, lat2, EW2, lon2,
                    in_out, G_parameter.fence[sn].alarm_type);
#endif
        }
        else
        {
#if defined(__GERMANY__)
            sprintf(cmd->rsp_msg, "GeoFence Typ:Kreis, %s, Latitude:%s%.6f, Longitude:%s%.6f, Radius:%dm, REIN RAUS:%s, Alarm typ:%d",
                    ON_OFF(G_parameter.fence[sn].sw),
                    NS, lat, EW, lon, G_parameter.fence[sn].radius * 100,
                    in_out, G_parameter.fence[sn].alarm_type);
#else
            sprintf(cmd->rsp_msg, "FenceType:Circle, %s, Latitude:%s%.6f, Longitude:%s%.6f, radius:%dm, in out:%s, alarm type:%d",
                    ON_OFF(G_parameter.fence[sn].sw),
                    NS, lat, EW, lon, G_parameter.fence[sn].radius * 100,
                    in_out, G_parameter.fence[sn].alarm_type);
#endif
        }
        return;
    }
    memcpy(&fence, &G_parameter.fence[sn], sizeof(nvram_electronic_fence_struct));
    if (!strcmp(cmd->pars[i], "ON"))
    {
        char *lat1, *lon1, *lat2, *lon2;
        fence.sw = 1;
        i++;
        LOGD(L_APP, L_V6, "%d,%d", fence.sw, cmd->part);
        tmp = track_fun_atoi(cmd->pars[i++]);
        if (tmp != 0 && tmp != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1)");
            return;
        }
        fence.square = tmp;

        if (fence.square == 0)
        {
            //圆形
            if (cmd->part < 5 || cmd->part > 7)
            {
                sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
                return;
            }
            lat1 = cmd->pars[i];
            lon1 = cmd->pars[i + 1];
            if ((!strcmp(lat1, "0") || *lat1 == 0) && (!strcmp(lon1, "0") || *lon1 == 0))
            {
                if (track_cust_gps_status() < GPS_STAUS_2D)
                {
                    if (track_cust_gps_status() == GPS_STAUS_OFF)
                        track_cust_gps_work_req((void*)5);
#if defined(__GERMANY__)
                    return_message(
                        cmd,
                        "GPS nicht erkannt. Bitte erneut senden, wenn GPS erkannt wird.",
                        "当前卫星定位未定位，请在卫星定位后设置！");
#else
#if defined (__TRACK_TYPE_GPS_BEIDOU__)
                    return_message(
                        cmd,
                        "BD/GPS not fixed, Please set again after BD/GPS fixed!",
                        "当前卫星定位未定位，请在卫星定位后设置！");
#else
                    return_message(
                        cmd,
                        "GPS not fixed, Please set again after GPS fixed!",
                        "当前卫星定位未定位，请在卫星定位后设置！");
#endif/*__GERMANY__*/
#endif /* __TRACK_TYPE_GPS_BEIDOU__ */

                    return;
                }
                fence.lat = 0;
                fence.lon = 0;
                gpsPoint = track_drv_get_gps_data();
                if (gpsPoint->gprmc.NS == 'S')
                    fence.lat = -gpsPoint->gprmc.Latitude;
                else
                    fence.lat = gpsPoint->gprmc.Latitude;
                if (gpsPoint->gprmc.EW == 'W')
                    fence.lon = -gpsPoint->gprmc.Longitude;
                else
                    fence.lon = gpsPoint->gprmc.Longitude;
                autoLatLon = KAL_TRUE;
            }
            else if (strlen(lat1) > 0 || strlen(lon1) > 0)
            {
                if (!enter_lat_lon(1, lat1, &fence.lat))
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 3!");
                    return;
                }
                if (!enter_lat_lon(0, lon1, &fence.lon))
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 4!");
                    return;
                }
                if (fence.lat == 0 || fence.lon == 0)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 3 or Parameter 4 !\r\nlat:%s, lon:%s, (%f,%f)", &lat1[1], &lon1[1], fence.lat, fence.lon);
                    return;
                }
                if (fence.lat > 90 || fence.lat < -90 || fence.lon > 180 || fence.lon < -180)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 3 or Parameter 4.");
                    return;
                }
            }
            else
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 or Parameter 4!");
                return;
            }
            i += 2;
            if (cmd->part >= i)
            {
                tmp = track_fun_atoi(cmd->pars[i]);
                if (tmp < 1 || tmp > 9999)
                {
#if defined(__GERMANY__)
                    sprintf(cmd->rsp_msg, "Fehler: Parameter 5 (1-9999) Einheit = 100m");
#else
                    sprintf(cmd->rsp_msg, "Error: Parameter 5 (1-9999) (Unit: hundred meters)");
#endif
                    return;
                }
                fence.radius = tmp;
            }
            i++;
            //fence.lat2 = 0;
            //fence.lon2 = 0;
        }
        else if (fence.square == 1)
        {
            //方形
            if (cmd->part < 6 || cmd->part > 8)
            {
                sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
                return;
            }
            lat1 = cmd->pars[i];
            lon1 = cmd->pars[i + 1];
            lat2 = cmd->pars[i + 2];
            lon2 = cmd->pars[i + 3];
            LOGD(L_CMD, L_V5, "%s;%s;%s;%s", cmd->pars[i], cmd->pars[i + 1], cmd->pars[i + 2], cmd->pars[i + 3]);
            LOGD(L_CMD, L_V5, "%s;%s;%s;%s", lat1, lon1, lat2, lon2);
            if (strlen(lat1) > 0 && strlen(lon1) > 0 && strlen(lat2) > 0 && strlen(lon2) > 0)
            {
                if (!enter_lat_lon(1, lat1, &fence.lat))
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 3!");
                    return;
                }
                if (!enter_lat_lon(0, lon1, &fence.lon))
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 4!");
                    return;
                }
                if (!enter_lat_lon(1, lat2, &fence.lat2))
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 5!");
                    return;
                }
                if (!enter_lat_lon(0, lon2, &fence.lon2))
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 6!");
                    return;
                }
                /*if(fence.lat == 0 || fence.lon == 0 || fence.lat2 == 0 || fence.lon2 == 0)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameters 3 to 6 can not be zero!");
                    return;
                }*/
                if (fence.lat > 90 || fence.lat < -90 || fence.lon > 180 || fence.lon < -180
                        || fence.lat2 > 90 || fence.lat2 < -90 || fence.lon2 > 180 || fence.lon2 < -180)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameters 3 to 6.");
                    return;
                }
                LOGD(L_CMD, L_V5, "%.6f;%.6f;%.6f;%.6f", fence.lat, fence.lon, fence.lat2, fence.lon2);
                if (fence.lat == fence.lat2 || fence.lon == fence.lon2)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameters 3 and 5 of the same or parameters 4 and 6 of the same!");
                    return;
                }
            }
            else
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 to 6!");
                return;
            }
            i += 4;
            //fence.radius = 0;
        }
        fence.in_out = OTA_NVRAM_EF_PARAMETER_DEFAULT->fence[sn].in_out;
        if (cmd->part >= i)
        {
            if (!strcmp("IN", cmd->pars[i]))
            {
                fence.in_out = 0;
            }
            else if (!strcmp("OUT", cmd->pars[i]))
            {
                fence.in_out = 1;
            }
            else if (strlen(cmd->pars[i]) == 0)
            {
                fence.in_out = 2;
            }
            else
            {
                sprintf(cmd->rsp_msg, "Error: Parameter %d (IN/OUT)", i);
                return;
            }
        }
        i++;
        fence.alarm_type = OTA_NVRAM_EF_PARAMETER_DEFAULT->fence[sn].alarm_type;
        if (cmd->part >= i)
        {
            tmp = track_fun_atoi(cmd->pars[i]);
            if (tmp != 0 && tmp != 1)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter %d (0/1)", i);
                return;
            }
            fence.alarm_type = tmp;
        }
    }
    else if (!strcmp(cmd->pars[i], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        fence.sw = 0;
        fence.lat = 0;
        fence.lon = 0;
        fence.lat2 = 0;
        fence.lon2 = 0;
        fence.in_out = OTA_NVRAM_EF_PARAMETER_DEFAULT->fence[sn].in_out;
        fence.alarm_type = OTA_NVRAM_EF_PARAMETER_DEFAULT->fence[sn].alarm_type;
        if (G_parameter.lang == 1 && cmd->return_sms.cm_type != CM_ONLINE && cmd->return_sms.cm_type != CM_ONLINE2)
        {
            char buf[320] = {0};
            int ret;
            char *tmp = NULL, *str_in = "入", *str_out = "出", *str_in_and_out = "出入";
            if (fence.in_out == 0)
            {
                tmp = str_in;
            }
            else if (fence.in_out == 2)
            {
                tmp = str_in_and_out;
            }
            else
            {
                tmp = str_out;
            }
            if (fence.square == 0)
            {
                snprintf(buf, 320, "设置成功！圆形围栏已关闭。");
                /*snprintf(buf, 320, "设置成功！圆形围栏，报警已关闭；圆心：0.000000，0.000000，半径：%d米，%s围栏报警，报警类型：%d",
                             fence.radius * 100, tmp, fence.alarm_type);*/
            }
            else
            {
                snprintf(buf, 320, "设置成功！矩形围栏已关闭。");
                /*snprintf(buf, 320, "设置成功！矩形围栏，报警已关闭；纬度1：0.000000，经度1：0.000000，纬度2：0.000000，经度2：0.000000；%s围栏报警，报警类型：%d",
                             tmp, fence.alarm_type);*/
            }
            if (cmd->return_sms.cm_type == CM_AT)
            {
                cmd->rsp_length = strlen(buf);
                memcpy(cmd->rsp_msg, buf, cmd->rsp_length);
                cmd->return_sms.reply_character_encode = 2;
            }
            else
            {
                cmd->rsp_length = track_fun_GB2312_to_UTF16BE(buf, strlen(buf), cmd->rsp_msg, 320);
                cmd->return_sms.reply_character_encode = 1;
            }
        }
        else
        {
            char *tmp = NULL, *str_in = "IN", *str_out = "OUT", *str_in_and_out = "IN or OUT";
            if (fence.in_out == 0)
            {
                tmp = str_in;
            }
            else if (fence.in_out == 2)
            {
                tmp = str_in_and_out;
            }
            else
            {
                tmp = str_out;
            }
            if (fence.square == 0)
            {
#if defined(__GERMANY__)
                sprintf(cmd->rsp_msg, "OK! GeoZonen Alarm deaktiviert !");
#else
                sprintf(cmd->rsp_msg, "OK!Disable Circle Fence set successfully!");
                /*sprintf(cmd->rsp_msg, "OK! FenceType:Circle, OFF, Latitude:0.000000, Longitude:0.000000, radius:%dm, in out:%s, alarm type:%d.",
                            fence.radius * 100, tmp, fence.alarm_type);*/
#endif
            }
            else
            {
#if defined(__GERMANY__)
                sprintf(cmd->rsp_msg, "OK! GeoZonen Alarm deaktiviert !");
#else
                sprintf(cmd->rsp_msg, "OK!Disable Square Fence set successfully!");
                /*sprintf(cmd->rsp_msg, "OK! FenceType:Square, OFF, Latitude1:0.000000, Longitude1:0.000000, Latitude2:0.000000, Longitude2:0.000000, in out:%s, alarm type:%d.",
                            tmp, fence.alarm_type);*/
#endif
            }
        }
    }
    else if (!strcmp(cmd->pars[i], "C") && fence.square == 0)
    {
        float tmp, lat, lon;
        tmp = fence.radius * 100;
        tmp = tmp / 111000;
        lat = fence.lat;
        lon = fence.lon;
        fence.lat = lat - tmp;
        fence.lon = lon + tmp;
        fence.lat2 = lat + tmp;
        fence.lon2 = lon - tmp;
        fence.square = 1;
#if defined(__GERMANY__)
        sprintf(cmd->rsp_msg, "GeoFence Typ:Square, %s, Latitude1:%.6f, Longitude1:%.6f, Latitude2:%.6f, Longitude2:%.6f, REIN RAUS :%d, alarm typ:%d",
                ON_OFF(fence.sw),
                fence.lat, fence.lon,
                fence.lat2, fence.lon2,
                fence.in_out, fence.alarm_type);
#else
        sprintf(cmd->rsp_msg, "FenceType:Square, %s, Latitude1:%.6f, Longitude1:%.6f, Latitude2:%.6f, Longitude2:%.6f, in out:%d, alarm type:%d",
                ON_OFF(fence.sw),
                fence.lat, fence.lon,
                fence.lat2, fence.lon2,
                fence.in_out, fence.alarm_type);
#endif
    }
    else if (!strcmp(cmd->pars[i], "SEE"))
    {
        if (G_parameter.fence[0].lat > 0 && G_parameter.fence[0].lon > 0)
        {
            float res = 0;
            if (track_cust_gps_status() >= GPS_STAUS_2D)
            {
                res = track_fun_get_GPS_2Points_distance(
                          G_parameter.fence[0].lat, G_parameter.fence[0].lon,
                          track_drv_get_gps_RMC()->Latitude, track_drv_get_gps_RMC()->Longitude);
                sprintf(cmd->rsp_msg, "Distance from the origin : %.2fM", res);
            }
            else
            {
                sprintf(cmd->rsp_msg, "No location!");
            }
        }
        else
        {
            sprintf(cmd->rsp_msg, "Error: Origin of longitude and latitude is no set!");
        }
        return;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.fence[sn], &fence, sizeof(nvram_electronic_fence_struct));
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    if (fence.sw == 1 && !strlen(cmd->rsp_msg))
    {
        applib_time_struct time = {0};
        char NS, EW, NS2, EW2;
        float lat, lon, lat2, lon2;
        if (fence.lat >= 0)
        {
            NS = 'N';
            lat = fence.lat;
        }
        else
        {
            NS = 'S';
            lat = -fence.lat;
        }
        if (fence.lon >= 0)
        {
            EW = 'E';
            lon = fence.lon;
        }
        else
        {
            EW = 'W';
            lon = -fence.lon;
        }
        if (fence.square == 1)
        {
            if (fence.lat2 >= 0)
            {
                NS2 = 'N';
                lat2 = fence.lat2;
            }
            else
            {
                NS2 = 'S';
                lat2 = -fence.lat2;
            }
            if (fence.lon2 >= 0)
            {
                EW2 = 'E';
                lon2 = fence.lon2;
            }
            else
            {
                EW2 = 'W';
                lon2 = -fence.lon2;
            }
        }
        /* GPS数据中的时间加上时区校正 */
        if (track_cust_is_upload_UTC_time())
        {
            track_drv_utc_timezone_to_rtc(&time, (applib_time_struct*)&gpsPoint->gprmc.Date_Time, G_parameter.zone);
        }
        else
        {
            memcpy(&time, &gpsPoint->gprmc.Date_Time, sizeof(applib_time_struct));
        }
        if (G_parameter.lang == 1 && cmd->return_sms.cm_type != CM_ONLINE && cmd->return_sms.cm_type != CM_ONLINE2)
        {
            char buf[320] = {0};
            int ret;
            char *tmp = NULL, *str_in = "入", *str_out = "出", *str_in_and_out = "出入";
            if (fence.in_out == 0)
            {
                tmp = str_in;
            }
            else if (fence.in_out == 2)
            {
                tmp = str_in_and_out;
            }
            else
            {
                tmp = str_out;
            }
            if (autoLatLon)
            {
                sprintf(buf, "OK！已自动获取圆心位置：%c%.6f,%c%.6f，时间：%.2d-%.2d-%.2d %.2d:%.2d:%.2d",
                        NS, lat, EW, lon,
                        time.nYear, time.nMonth, time.nDay,
                        time.nHour, time.nMin, time.nSec);

            }
            else if (fence.square == 0)
            {
                sprintf(buf, "设置成功！圆形围栏，报警已开启；圆心：%c%.6f，%c%.6f，半径：%d米，%s围栏报警，报警类型：%d",
                        NS, lat, EW, lon,
                        fence.radius * 100, tmp, fence.alarm_type);
            }
            else
            {
                sprintf(buf, "设置成功！矩形围栏，报警已开启；纬度1：%c%.6f，经度1：%c%.6f，纬度2：%c%.6f，经度2：%c%.6f；%s围栏报警，报警类型：%d",
                        NS, lat, EW, lon, NS2, lat2, EW2, lon2,
                        tmp, fence.alarm_type);
            }
            if (cmd->return_sms.cm_type == CM_AT)
            {
                cmd->rsp_length = strlen(buf);
                memcpy(cmd->rsp_msg, buf, cmd->rsp_length);
                cmd->return_sms.reply_character_encode = 2;
            }
            else
            {
                cmd->rsp_length = track_fun_GB2312_to_UTF16BE(buf, strlen(buf), cmd->rsp_msg, 320);
                cmd->return_sms.reply_character_encode = 1;
            }
        }
        else
        {
            char *tmp = NULL, *str_in = "IN", *str_out = "OUT", *str_in_and_out = "IN or OUT";
            if (fence.in_out == 0)
            {
                tmp = str_in;
            }
            else if (fence.in_out == 2)
            {
                tmp = str_in_and_out;
            }
            else
            {
                tmp = str_out;
            }
            if (autoLatLon)
            {
#if defined(__GERMANY__)
                sprintf(cmd->rsp_msg, "OK! GPS automatisch erkennen! Latitude:%c%.6f Longitude:%c%.6f",
                        NS, lat, EW, lon
                       );
#else
                sprintf(cmd->rsp_msg, "OK! Automatically obtain latitude:%c%.6f Longitude:%c%.6f, in the time %.2d-%.2d-%.2d %.2d:%.2d:%.2d",
                        NS, lat, EW, lon,
                        time.nYear, time.nMonth, time.nDay,
                        time.nHour, time.nMin, time.nSec);
#endif
            }
            else if (fence.square == 0)
            {
#if defined(__GERMANY__)
                sprintf(cmd->rsp_msg, "OK! GeoFence Typ:Kreis,EIN,Latitude:%c%.6f, Longitude:%c%.6f, Radius:%dm, REIN RAUS:%s, Alarm typ:%d.",
                        NS, lat, EW, lon,
                        fence.radius * 100, tmp, fence.alarm_type);
#else
                sprintf(cmd->rsp_msg, "OK! FenceType:Circle, ON, Latitude:%c%.6f, Longitude:%c%.6f, radius:%dm, in out:%s, alarm type:%d.",
                        NS, lat, EW, lon,
                        fence.radius * 100, tmp, fence.alarm_type);
#endif
            }
            else
            {
#if defined(__GERMANY__)
                sprintf(cmd->rsp_msg, "OK! GeoFence Typ:Square,EIN,Latitude1:%c%.6f, Longitude1:%c%.6f, Latitude2:%c%.6f, Longitude2:%c%.6f, REIN RAUS:%s, Alarm typ:%d.",
                        NS, lat, EW, lon, NS2, lat2, EW2, lon2,
                        tmp, fence.alarm_type);
#else
                sprintf(cmd->rsp_msg, "OK! FenceType:Square, ON, Latitude1:%c%.6f, Longitude1:%c%.6f, Latitude2:%c%.6f, Longitude2:%c%.6f, in out:%s, alarm type:%d.",
                        NS, lat, EW, lon, NS2, lat2, EW2, lon2,
                        tmp, fence.alarm_type);
#endif
            }
        }
    }
    if (!strlen(cmd->rsp_msg) && cmd->rsp_length == 0) sprintf(cmd->rsp_msg, "OK!");
    track_cust_module_fence_cmd_update(0);
#if defined(__ET310__)||defined (__ET320__)|| defined(__GT370__)|| defined (__GT380__)|| defined (__NT37__)|| defined (__NT33__)
    track_cust_paket_9404_handle();
#endif
}

/******************************************************************************
 *  Function    -  cmd_gfence
 *
 *  Purpose     -  电子围栏参数设置
 *
 *  Description -
 *        FENCE,<N>,<B>,0,<D>,<E>,<F>[,X][,M]#
 *            圆形区域
 *            N=1-5,1到5的围栏
 *            B=ON/OFF；开启/关闭围栏报警；默认值为：关闭
 *            D=圆心纬度
 *            E=圆心经度
 *            F=100 - 9999；围栏半径，单位：米
 *            X=IN/OUT；IN：入围栏报警，OUT：出围栏报警，为空：进/出围栏报警；默认为进出围栏都报警
 *            M=0/1；报警上报方式，0：仅GPRS，1：SMS+GPRS，默认为：1
 *        FENCE,<N>,<B>,1,<D>,<E>,<F>,<G>[,X][,M]#
 *            方形区域设置
 *            N=1-5,1到5的围栏
 *            B=ON/OFF；开启/关闭围栏报警；默认值为：关闭
 *            D=坐标1纬度
 *            E=坐标1经度
 *            F=坐标2纬度
 *            G=坐标2经度
 *            X=0/1；0 入围栏报警，1 出围栏报警，2 进/出围栏报警；默认为2
 *            M=0/1；报警上报方式，0：仅GPRS，1：SMS+GPRS，默认为：1
 *        FENCE#
 *            查询围栏设置参数
 *
 *        GFENCE#
 *        GFENCE,1,OFF#
 *        GFENCE,1,ON,0,,,1#
 *        GFENCE,1,ON,0,23.10267,114.39671,1#
 *        GFENCE,1,ON,0,23.10267,114.39671,1,2,1#
 *        GFENCE,1,ON,1,23.10267,114.39671,23.20267,114.49671#
 *        GFENCE,1,ON,1,23.10267,114.39671,23.20267,114.59671,2,1#
 *  modification history
 * ----------------------------------------
 * v1.0  , 25-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_gfence(CMD_DATA_STRUCT *cmd)
{
    nvram_electronic_fence_struct fence = {0};
    track_gps_data_struct *gpsPoint = NULL;
    kal_bool autoLatLon = KAL_FALSE;
    U8 j;
    char msg[50];
    int sn = 0, tmp, i = 1;

    if (cmd->part == 0)
    {
        for (j = 0; j < TRACK_DEFINE_FENCE_SUM; j++)
        {
            if (j == TRACK_DEFINE_FENCE_SUM - 1)
            {
                sprintf(msg, "fence%d:%s", j + 1, ON_OFF(G_parameter.fence[j].sw));
            }
            else
            {
                sprintf(msg, "fence%d:%s,", j + 1, ON_OFF(G_parameter.fence[j].sw));
            }
            strcat(cmd->rsp_msg, msg);
        }
        return;
    }
    if (track_cust_get_work_mode() == WORK_MODE_3)
    {
        snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, string3_en);
        return;
    }
    sn = track_fun_atoi(cmd->pars[i]) - 1;
    i++;
    if (sn < 0 || sn > TRACK_DEFINE_FENCE_SUM - 1)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 2 (1-%d)", TRACK_DEFINE_FENCE_SUM);
        return;
    }

    if (cmd->part == 1)
    {
        if (G_parameter.fence[sn].sw)
        {
            char NS[2] = {0}, EW[2] = {0}, NS2[2] = {0}, EW2[2] = {0}, in_out[10] = {0};
            float lat, lon, lat2, lon2;
            if (G_parameter.fence[sn].lat >= 0)
            {
                if (G_parameter.fence[sn].lat != 0) *NS = 'N';
                lat = G_parameter.fence[sn].lat;
            }
            else
            {
                if (G_parameter.fence[sn].lat != 0) *NS = 'S';
                lat = -G_parameter.fence[sn].lat;
            }

            if (G_parameter.fence[sn].lon >= 0)
            {
                if (G_parameter.fence[sn].lon != 0) *EW = 'E';
                lon = G_parameter.fence[sn].lon;
            }
            else
            {
                if (G_parameter.fence[sn].lon != 0) *EW = 'W';
                lon = -G_parameter.fence[sn].lon;
            }
            if (G_parameter.fence[sn].in_out == 0)
                sprintf(in_out, "IN");
            else if (G_parameter.fence[sn].in_out == 1)
                sprintf(in_out, "OUT");
            else if (G_parameter.fence[sn].in_out == 2)
                sprintf(in_out, "IN or OUT");
            if (G_parameter.fence[sn].square)
            {
                if (G_parameter.fence[sn].lat2 >= 0)
                {
                    if (G_parameter.fence[sn].lat2 != 0) *NS2 = 'N';
                    lat2 = G_parameter.fence[sn].lat2;
                }
                else
                {
                    if (G_parameter.fence[sn].lat2 != 0) *NS2 = 'S';
                    lat2 = -G_parameter.fence[sn].lat2;
                }
                if (G_parameter.fence[sn].lon2 >= 0)
                {
                    if (G_parameter.fence[sn].lon2 != 0) *EW2 = 'E';
                    lon2 = G_parameter.fence[sn].lon2;
                }
                else
                {
                    if (G_parameter.fence[sn].lon2 != 0) *EW2 = 'W';
                    lon2 = -G_parameter.fence[sn].lon2;
                }
                sprintf(cmd->rsp_msg, "Fence %d Type:Square, %s, Latitude1:%s%.6f, Longitude1:%s%.6f, Latitude2:%s%.6f, Longitude2:%s%.6f, in out:%s, alarm type:%d",
                        sn + 1, ON_OFF(G_parameter.fence[sn].sw),
                        NS, lat, EW, lon,
                        NS2, lat2, EW2, lon2,
                        in_out, G_parameter.fence[sn].alarm_type);
            }
            else
            {
                sprintf(cmd->rsp_msg, "Fence %d Type:Circle, %s, Latitude:%s%.6f, Longitude:%s%.6f, radius:%dm, in out:%s, alarm type:%d",
                        sn + 1, ON_OFF(G_parameter.fence[sn].sw),
                        NS, lat, EW, lon, G_parameter.fence[sn].radius * 100,
                        in_out, G_parameter.fence[sn].alarm_type);
            }
        }
        else
        {
            sprintf(cmd->rsp_msg, "%s %d:OFF" , cmd->pars[0], sn + 1);
        }
        return;
    }
    memcpy(&fence, &G_parameter.fence[sn], sizeof(nvram_electronic_fence_struct));
    if (!strcmp(cmd->pars[i], "ON"))
    {
        char *lat1, *lon1, *lat2, *lon2;
        fence.sw = 1;
        i++;
        LOGD(L_APP, L_V6, "%d,%d", fence.sw, cmd->part);
        tmp = track_fun_atoi(cmd->pars[i++]);
        if (tmp != 0 && tmp != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 3 (0/1)");
            return;
        }
        fence.square = tmp;

        if (fence.square == 0)
        {
            //圆形
            if (cmd->part < 6 || cmd->part > 8)
            {
                sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
                return;
            }
            lat1 = cmd->pars[i];
            lon1 = cmd->pars[i + 1];
            if ((!strcmp(lat1, "0") || *lat1 == 0) && (!strcmp(lon1, "0") || *lon1 == 0))
            {
                if (track_cust_gps_status() == GPS_STAUS_OFF)
                {
                    return_message(
                        cmd,
                        "GPS is not working, Please open the GPS first!",
                        "当前GPS未开启，请开启GPS定位！");
                    return;
                }
                else if (track_cust_gps_status() < GPS_STAUS_2D)
                {
                    return_message(
                        cmd,
                        "GPS not fixed, Please set again after GPS fixed!",
                        "当前GPS未定位，请在GPS定位后设置！");
                    return;
                }
                fence.lat = 0;
                fence.lon = 0;
                gpsPoint = track_drv_get_gps_data();
                if (gpsPoint->gprmc.NS == 'S')
                    fence.lat = -gpsPoint->gprmc.Latitude;
                else
                    fence.lat = gpsPoint->gprmc.Latitude;
                if (gpsPoint->gprmc.EW == 'W')
                    fence.lon = -gpsPoint->gprmc.Longitude;
                else
                    fence.lon = gpsPoint->gprmc.Longitude;
                autoLatLon = KAL_TRUE;
            }
            else if (strlen(lat1) > 0 || strlen(lon1) > 0)
            {
                if (!enter_lat_lon(1, lat1, &fence.lat))
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 4!");
                    return;
                }
                if (!enter_lat_lon(0, lon1, &fence.lon))
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 5!");
                    return;
                }
                if (fence.lat == 0 || fence.lon == 0)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 4or Parameter 5 !\r\nlat:%s, lon:%s, (%f,%f)", &lat1[1], &lon1[1], fence.lat, fence.lon);
                    return;
                }
                if (fence.lat > 90 || fence.lat < -90 || fence.lon > 180 || fence.lon < -180)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 4 or Parameter 5.");
                    return;
                }
            }
            else
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 4 or Parameter 5!");
                return;
            }
            i += 2;
            if (cmd->part >= i)
            {
                tmp = track_fun_atoi(cmd->pars[i]);
                if (tmp < 1 || tmp > 9999)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 6 (1-9999) (Unit: hundred meters)");
                    return;
                }
                fence.radius = tmp;
            }
            i++;
            //fence.lat2 = 0;
            //fence.lon2 = 0;
        }
        else if (fence.square == 1)
        {
            //方形
            if (cmd->part < 7 || cmd->part > 9)
            {
                sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
                return;
            }
            lat1 = cmd->pars[i];
            lon1 = cmd->pars[i + 1];
            lat2 = cmd->pars[i + 2];
            lon2 = cmd->pars[i + 3];
            LOGD(L_CMD, L_V5, "%s;%s;%s;%s", cmd->pars[i], cmd->pars[i + 1], cmd->pars[i + 2], cmd->pars[i + 3]);
            LOGD(L_CMD, L_V5, "%s;%s;%s;%s", lat1, lon1, lat2, lon2);
            if (strlen(lat1) > 0 && strlen(lon1) > 0 && strlen(lat2) > 0 && strlen(lon2) > 0)
            {
                if (!enter_lat_lon(1, lat1, &fence.lat))
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 4!");
                    return;
                }
                if (!enter_lat_lon(0, lon1, &fence.lon))
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 5!");
                    return;
                }
                if (!enter_lat_lon(1, lat2, &fence.lat2))
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 6!");
                    return;
                }
                if (!enter_lat_lon(0, lon2, &fence.lon2))
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 7!");
                    return;
                }
                if (fence.lat == 0 || fence.lon == 0 || fence.lat2 == 0 || fence.lon2 == 0)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameters 4 to 7 can not be zero!");
                    return;
                }
                if (fence.lat > 90 || fence.lat < -90 || fence.lon > 180 || fence.lon < -180
                        || fence.lat2 > 90 || fence.lat2 < -90 || fence.lon2 > 180 || fence.lon2 < -180)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameters 4 to 7.");
                    return;
                }
                LOGD(L_CMD, L_V5, "%.6f;%.6f;%.6f;%.6f", fence.lat, fence.lon, fence.lat2, fence.lon2);
                if (fence.lat == fence.lat2 || fence.lon == fence.lon2)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameters 4 and 6 of the same or parameters 4 and 6 of the same!");
                    return;
                }
            }
            else
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 4 to 6!");
                return;
            }
            i += 4;
            //fence.radius = 0;
        }
        fence.in_out = OTA_NVRAM_EF_PARAMETER_DEFAULT->fence[sn].in_out;
        if (cmd->part >= i)
        {
            if (!strcmp("IN", cmd->pars[i]))
            {
                fence.in_out = 0;
            }
            else if (!strcmp("OUT", cmd->pars[i]))
            {
                fence.in_out = 1;
            }
            else if (strlen(cmd->pars[i]) == 0)
            {
                fence.in_out = 2;
            }
            else
            {
                sprintf(cmd->rsp_msg, "Error: Parameter %d (IN/OUT)", i);
                return;
            }
        }
        i++;
        fence.alarm_type = OTA_NVRAM_EF_PARAMETER_DEFAULT->fence[sn].alarm_type;
        if (cmd->part >= i)
        {
            tmp = track_fun_atoi(cmd->pars[i]);
            if (tmp != 0 && tmp != 1 && tmp != 2 && tmp != 3)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter %d (0/1/2/3)", i);
                return;
            }
            fence.alarm_type = tmp;
        }
    }
    else if (!strcmp(cmd->pars[i], "OFF"))
    {
        if (cmd->part != 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        fence.sw = 0;
        fence.lat = 0;
        fence.lon = 0;
        fence.lat2 = 0;
        fence.lon2 = 0;
        fence.in_out = OTA_NVRAM_EF_PARAMETER_DEFAULT->fence[sn].in_out;
        fence.alarm_type = OTA_NVRAM_EF_PARAMETER_DEFAULT->fence[sn].alarm_type;
        if (G_parameter.lang == 1 && cmd->return_sms.cm_type != CM_ONLINE && cmd->return_sms.cm_type != CM_ONLINE2)
        {
            char buf[320] = {0};
            int ret;
            char *tmp = NULL, *str_in = "入", *str_out = "出", *str_in_and_out = "出入";
            if (fence.in_out == 0)
            {
                tmp = str_in;
            }
            else if (fence.in_out == 2)
            {
                tmp = str_in_and_out;
            }
            else
            {
                tmp = str_out;
            }
            if (fence.square == 0)
            {
                sprintf(buf, "圆形围栏%d已关闭!", sn + 1);
            }
            else
            {
                sprintf(buf, "矩形围栏%d已关闭!", sn + 1);
            }
            if (cmd->return_sms.cm_type == CM_AT)
            {
                cmd->rsp_length = strlen(buf);
                memcpy(cmd->rsp_msg, buf, cmd->rsp_length);
                cmd->return_sms.reply_character_encode = 2;
            }
            else
            {
                cmd->rsp_length = track_fun_GB2312_to_UTF16BE(buf, strlen(buf), cmd->rsp_msg, 320);
                cmd->return_sms.reply_character_encode = 1;
            }
        }
        else
        {
            char *tmp = NULL, *str_in = "IN", *str_out = "OUT", *str_in_and_out = "IN or OUT";
            if (fence.in_out == 0)
            {
                tmp = str_in;
            }
            else if (fence.in_out == 2)
            {
                tmp = str_in_and_out;
            }
            else
            {
                tmp = str_out;
            }
            if (fence.square == 0)
            {
                sprintf(cmd->rsp_msg, "Disable Circle Fence %d set successfully! ", sn + 1);
            }
            else
            {
                sprintf(cmd->rsp_msg, "Disable Square Fence %d set successfully!", sn + 1);
            }
        }
    }
    else if (!strcmp(cmd->pars[i], "C") && fence.square == 0)
    {
        float tmp, lat, lon;
        tmp = fence.radius * 100;
        tmp = tmp / 111000;
        lat = fence.lat;
        lon = fence.lon;
        fence.lat = lat - tmp;
        fence.lon = lon + tmp;
        fence.lat2 = lat + tmp;
        fence.lon2 = lon - tmp;
        fence.square = 1;
        sprintf(cmd->rsp_msg, "Fence %d set successfully!FenceType:Square, %s, Latitude1:%.6f, Longitude1:%.6f, Latitude2:%.6f, Longitude2:%.6f, in out:%d, alarm type:%d",
                ON_OFF(fence.sw),
                fence.lat, fence.lon,
                fence.lat2, fence.lon2,
                fence.in_out, fence.alarm_type);
    }
    else if (!strcmp(cmd->pars[i], "SEE"))
    {
        if (G_parameter.fence[sn].lat > 0 && G_parameter.fence[sn].lon > 0)
        {
            float res = 0;
            if (track_cust_gps_status() >= GPS_STAUS_2D)
            {
                res = track_fun_get_GPS_2Points_distance(
                          G_parameter.fence[sn].lat, G_parameter.fence[sn].lon,
                          track_drv_get_gps_RMC()->Latitude, track_drv_get_gps_RMC()->Longitude);
                sprintf(cmd->rsp_msg, "Distance from the origin : %.2fM", res);
            }
            else
            {
                sprintf(cmd->rsp_msg, "No location!");
            }
        }
        else
        {
            sprintf(cmd->rsp_msg, "Error: Origin of longitude and latitude is no set!");
        }
        return;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 2 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.fence[sn], &fence, sizeof(nvram_electronic_fence_struct));
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    if (fence.sw == 1 && !strlen(cmd->rsp_msg))
    {
        applib_time_struct time = {0};
        char NS, EW, NS2, EW2;
        float lat, lon, lat2, lon2;
        if (fence.lat >= 0)
        {
            NS = 'N';
            lat = fence.lat;
        }
        else
        {
            NS = 'S';
            lat = -fence.lat;
        }
        if (fence.lon >= 0)
        {
            EW = 'E';
            lon = fence.lon;
        }
        else
        {
            EW = 'W';
            lon = -fence.lon;
        }
        if (fence.square == 1)
        {
            if (fence.lat2 >= 0)
            {
                NS2 = 'N';
                lat2 = fence.lat2;
            }
            else
            {
                NS2 = 'S';
                lat2 = -fence.lat2;
            }
            if (fence.lon2 >= 0)
            {
                EW2 = 'E';
                lon2 = fence.lon2;
            }
            else
            {
                EW2 = 'W';
                lon2 = -fence.lon2;
            }
        }
        /* GPS数据中的时间加上时区校正 */
        if (track_cust_is_upload_UTC_time())
        {
            track_drv_utc_timezone_to_rtc(&time, (applib_time_struct*)&gpsPoint->gprmc.Date_Time, G_parameter.zone);
        }
        else
        {
            memcpy(&time, &gpsPoint->gprmc.Date_Time, sizeof(applib_time_struct));
        }
        if (G_parameter.lang == 1 && cmd->return_sms.cm_type != CM_ONLINE && cmd->return_sms.cm_type != CM_ONLINE2)
        {
            char buf[320] = {0};
            int ret;
            char *tmp = NULL, *str_in = "入", *str_out = "出", *str_in_and_out = "出入";
            if (fence.in_out == 0)
            {
                tmp = str_in;
            }
            else if (fence.in_out == 2)
            {
                tmp = str_in_and_out;
            }
            else
            {
                tmp = str_out;
            }
            if (autoLatLon)
            {
                sprintf(buf, "OK！围栏%d已自动获取圆心位置：%c%.6f,%c%.6f，时间：%.2d-%.2d-%.2d %.2d:%.2d:%.2d",
                        sn + 1, NS, lat, EW, lon,
                        time.nYear, time.nMonth, time.nDay,
                        time.nHour, time.nMin, time.nSec);

            }
            else if (fence.square == 0)
            {
                sprintf(buf, "围栏%d设置成功！圆形围栏，报警已开启；圆心：%c%.6f，%c%.6f，半径：%d米，%s围栏报警，报警类型：%d",
                        sn + 1, NS, lat, EW, lon,
                        fence.radius * 100, tmp, fence.alarm_type);
            }
            else
            {
                sprintf(buf, "围栏%d设置成功！矩形围栏，报警已开启；纬度1：%c%.6f，经度1：%c%.6f，纬度2：%c%.6f，经度2：%c%.6f；%s围栏报警，报警类型：%d",
                        sn + 1, NS, lat, EW, lon, NS2, lat2, EW2, lon2,
                        tmp, fence.alarm_type);
            }
            if (cmd->return_sms.cm_type == CM_AT)
            {
                cmd->rsp_length = strlen(buf);
                memcpy(cmd->rsp_msg, buf, cmd->rsp_length);
                cmd->return_sms.reply_character_encode = 2;
            }
            else
            {
                cmd->rsp_length = track_fun_GB2312_to_UTF16BE(buf, strlen(buf), cmd->rsp_msg, 320);
                cmd->return_sms.reply_character_encode = 1;
            }
        }
        else
        {
            char *tmp = NULL, *str_in = "IN", *str_out = "OUT", *str_in_and_out = "IN or OUT";
            if (fence.in_out == 0)
            {
                tmp = str_in;
            }
            else if (fence.in_out == 2)
            {
                tmp = str_in_and_out;
            }
            else
            {
                tmp = str_out;
            }
            if (autoLatLon)
            {
                sprintf(cmd->rsp_msg, "OK!The Fence %d Automatically obtain latitude:%c%.6f Longitude:%c%.6f, in the time %.2d-%.2d-%.2d %.2d:%.2d:%.2d",
                        sn + 1, NS, lat, EW, lon,
                        time.nYear, time.nMonth, time.nDay,
                        time.nHour, time.nMin, time.nSec);
            }
            else if (fence.square == 0)
            {
                sprintf(cmd->rsp_msg, "Fence %d set successfully! FenceType:Circle, ON, Latitude:%c%.6f, Longitude:%c%.6f, radius:%dm, in out:%s, alarm type:%d.",
                        sn + 1, NS, lat, EW, lon,
                        fence.radius * 100, tmp, fence.alarm_type);
            }
            else
            {
                sprintf(cmd->rsp_msg, "Fence %d set successfully! FenceType:Square, ON, Latitude1:%c%.6f, Longitude1:%c%.6f, Latitude2:%c%.6f, Longitude2:%c%.6f, in out:%s, alarm type:%d.",
                        sn + 1, NS, lat, EW, lon, NS2, lat2, EW2, lon2,
                        tmp, fence.alarm_type);
            }
        }
    }
    if (!strlen(cmd->rsp_msg) && cmd->rsp_length == 0) sprintf(cmd->rsp_msg, "OK!");
    track_cust_module_fence_cmd_update(sn);
}


static void cmd_wn(CMD_DATA_STRUCT * cmd)
{
    int index = 0, length = 0 , total = 0;
    char str[50] = {0};
    for (; index < TRACK_DEFINE_WN_COUNT; index++)
    {
        length = strlen(G_phone_number.white_num[index]);
        if (length > 0 && length < TRACK_DEFINE_PHONE_NUMBER_LEN_MAX)
        {
            length = snprintf(str, 49, "%d,%s;", index, G_phone_number.white_num[index]);
            LOGD(L_APP, L_V5, "length:%d,total:", length, total);
            total += length;

            if (total > CM_PARAM_LONG_LEN_MAX - 1)break;
            strcat(cmd->rsp_msg, G_phone_number.white_num[index]);
        }
    }
}

/******************************************************************************
 *  Function    -  cmd_park
 *
 *  Purpose     -  停车超时提醒设置
 *
 *  Description -
        PARK,<A>,<B>,[M]#
            A=ON/OFF；默认值：OFF
            B=5 - 3600分钟；停车时长；默认值：60
            M=0/1；报警上报方式，0 仅GPRS，1 SMS+GPRS；默认值：1
        PARK#
            查询已设置的参数
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_park(CMD_DATA_STRUCT *cmd)
{
    nvram_carpark_alarm_struct carpark_alarm = {0};
    int value = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d,%d", cmd->pars[0], ON_OFF(G_parameter.carpark_alarm.sw), G_parameter.carpark_alarm.time, G_parameter.carpark_alarm.alarm_type);
        return;
    }
    memcpy(&carpark_alarm, &G_parameter.carpark_alarm, sizeof(nvram_carpark_alarm_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part < 2 || cmd->part > 3)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        carpark_alarm.sw = 1;
        value = track_fun_atoi(cmd->pars[2]);
        if (value < 5 || value > 3600)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (5-3600)");
            return;
        }
        carpark_alarm.time = value;
        if (cmd->part == 3)
        {
            value = track_fun_atoi(cmd->pars[3]);
            if (value < 0 || value > 1)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (0-1)");
                return;
            }
            carpark_alarm.alarm_type = value;
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        carpark_alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.carpark_alarm, &carpark_alarm, sizeof(nvram_carpark_alarm_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}


/******************************************************************************
 *  Function    -  cmd_fdt
 *
 *  Purpose     -  疲劳驾驶报警设置
 *
 *  Description -
        FDT,<A>,<T1>,<T2>[,M]#
            A=ON/OFF；超速报警；默认值：OFF
            T1=1 - 3600分钟；疲劳驾驶持续时间；默认值：
            T2=1 - 3600分钟；驾驶清零计时时间；默认值：
            M=0/1；报警上报方式，0 仅GPRS，1 SMS+GPRS；默认值：0
        FDT#
            查询已设置的参数
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_fdt(CMD_DATA_STRUCT *cmd)
{
    nvram_fatigue_driving_alarm_struct fatigue_driving_alarm = {0};
    int value = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d,%d,%d", cmd->pars[0],
                ON_OFF(G_parameter.fatigue_driving_alarm.sw), G_parameter.fatigue_driving_alarm.delay_time,
                G_parameter.fatigue_driving_alarm.delay_reset, G_parameter.fatigue_driving_alarm.alarm_type);
        return;
    }
    memcpy(&fatigue_driving_alarm, &G_parameter.fatigue_driving_alarm, sizeof(nvram_fatigue_driving_alarm_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part < 3 || cmd->part > 4)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        fatigue_driving_alarm.sw = 1;
        value = track_fun_atoi(cmd->pars[2]);
        if (value < 1 || value > 3600)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (1-3600)");
            return;
        }
        fatigue_driving_alarm.delay_time = value;
        value = track_fun_atoi(cmd->pars[3]);
        if (value < 1 || value > 3600)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (1-3600)");
            return;
        }
        fatigue_driving_alarm.delay_reset = value;
        if (cmd->part == 4)
        {
            value = track_fun_atoi(cmd->pars[4]);
            if (value < 0 || value > 1)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (0-1)");
                return;
            }
            fatigue_driving_alarm.alarm_type = value;
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        fatigue_driving_alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.fatigue_driving_alarm, &fatigue_driving_alarm, sizeof(nvram_fatigue_driving_alarm_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

static void cmd_BOOTALM(CMD_DATA_STRUCT *cmd)
{
    nvram_boot_struct alarm = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d", cmd->pars[0],
                ON_OFF(G_parameter.boot.sw), G_parameter.boot.alarm_type);
        return;
    }
    memcpy(&alarm, &G_parameter.boot, sizeof(nvram_boot_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part < 1 || cmd->part > 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 1;
        if (cmd->part >= 2)
        {
            alarm.alarm_type = track_fun_atoi(cmd->pars[2]);
            if (alarm.alarm_type != 0 && alarm.alarm_type != 1)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1)");
                return;
            }
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.boot, &alarm, sizeof(nvram_boot_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

static void cmd_accalm(CMD_DATA_STRUCT *cmd)
{
#if defined(__ACC_ALARM__)
    kal_uint8 time;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s: %s,%d,%d,%d", cmd->pars[0],
                ON_OFF(G_parameter.accalm.accalm_sw), G_parameter.accalm.accalm_type, G_parameter.accalm.accalm_time, G_parameter.accalm.accalm_cause);
        return;
    }
#else
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s: %s", cmd->pars[0],
                ON_OFF(G_parameter.accalm_sw));
        return;
    }
#endif
    if (!strcmp(cmd->pars[1], "ON"))
    {
#if defined(__ACC_ALARM__)
        if (cmd->part < 1 || cmd->part > 4)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        else if (cmd->part >= 2)
        {
            time = track_fun_atoi(cmd->pars[2]);
            if (time < 0 || time > 1)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1)");
                return;
            }
            G_parameter.accalm.accalm_type = time;
            if (cmd->part >= 3)
            {
                time = track_fun_atoi(cmd->pars[3]);
                if (time < 5 || time > 60 || time != track_fun_atoi(cmd->pars[3]))
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 3 (5~60)");
                    return;
                }
                G_parameter.accalm.accalm_time = time;
            }
            if (cmd->part >= 4)
            {
                time = track_fun_atoi(cmd->pars[4]);
                if (time < 0 || time > 2 || time != track_fun_atoi(cmd->pars[4]))
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 4 (0~2)");
                    return;
                }
                G_parameter.accalm.accalm_cause = time;
            }
        }
        G_parameter.accalm.accalm_sw = 1;
#else
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        G_parameter.accalm_sw = 1;
#endif
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
#if defined(__ACC_ALARM__)
        G_parameter.accalm.accalm_sw = 0;
#else
        G_parameter.accalm_sw = 0;
#endif
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    sprintf(cmd->rsp_msg, "OK!");
}

/******************************************************************************
 *  Function    -  cmd_mileage
 *
 *  Purpose     -  里程设置指令
 *
 *  Description -
        MILEAGE,<A>,<N>#
            A=ON/OFF；超速报警；默认值：OFF
            N=0 - 5000000 km/h；里程初值；默认为：0
        MILEAGE,ON,0#
            里程归零
        MILEAGE,OFF#
            关闭里程统计
        MILEAGE#
            查询总里程，及设置的初值
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_mileage(CMD_DATA_STRUCT *cmd)
{
    nvram_mileage_statistics_struct mileage_statistics = {0};
    double value = 0;
    U32 gps_mileage;
    if (cmd->part == 0)
    {
        gps_mileage = track_get_gps_mileage() / 1000;
        sprintf(cmd->rsp_msg, "%s:%s, Total Mileage:%dkm,K:%d", cmd->pars[0], ON_OFF(G_parameter.mileage_statistics.sw),
                gps_mileage, G_parameter.mileage_statistics.K);
        return;
    }
    memcpy(&mileage_statistics, &G_parameter.mileage_statistics, sizeof(nvram_mileage_statistics_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part > 3)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        mileage_statistics.sw = 1;
        if (cmd->part >= 2 && cmd->part < 4)
        {
            if (track_fun_check_str_is_number(2, cmd->pars[2]) == 0)
            {
                sprintf(cmd->rsp_msg, "Error: Number contains non-numeric characters!");
                return;
            }
            value = atof(cmd->pars[2]);
            if (value < 0 || value > 999999)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0-999999)");
                return;
            }
            mileage_statistics.mileage = value * 1000;
            track_set_gps_distance(0);
            if (cmd->part == 3)
            {
                value = track_fun_atoi(cmd->pars[3]);
                if (value < 1000 || value > 1200)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 3 (1000 to 1200)");
                    return;
                }
                mileage_statistics.K = value;
            }
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        mileage_statistics.sw = 0;
        mileage_statistics.mileage = 0;
        track_set_gps_distance(0);
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.mileage_statistics, &mileage_statistics, sizeof(nvram_mileage_statistics_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}


#if defined(__NT31__)
static void cmd_clearalm(CMD_DATA_STRUCT *cmd)
{
    G_realtime_data.power_fails_flg = 0;
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    track_stop_timer(TRACK_CUST_POWER_FAILS_TIMER_ID);
    sprintf(cmd->rsp_msg, "OK!");

}
/******************************************************************************
 *  Function    -  cmd_power_fails_alarm
 *
 *  Purpose     -  断电报警
 *
 *  Description -
        POWERALM,<A>[,T[,M[,S]]]#
            A =ON/OFF；默认值：ON
            M =0 - 2；0 仅GPRS，1 SMS+GPRS，2 GPRS+SMS+电话；默认值：2；
            T1=2 - 3600，单位：秒，默认值：5,通电时间
            T2=2 - 3600，单位：秒；默认值：5，断开时间
            T3=1 - 3600 秒；ACC ON to OFF 跳变禁止报警时间
        POWERALM,OFF#
            关闭断电报警
        POWERALM#
            查询已设置的参数
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_poweralm(CMD_DATA_STRUCT *cmd)
{
    nvram_power_fails_alarm_struct alarm = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d,%d,%d,%d,%d", cmd->pars[0],
                ON_OFF(G_parameter.power_fails_alarm.sw),
                G_parameter.power_fails_alarm.alarm_type,
                G_parameter.power_fails_alarm.delay_time_off,
                G_parameter.power_fails_alarm.delay_time_on,
                G_parameter.power_fails_alarm.delay_time_acc_on_to_off,
                G_parameter.power_fails_alarm.send_alarm_time);
        return;
    }
    memcpy(&alarm, &G_parameter.power_fails_alarm, sizeof(nvram_power_fails_alarm_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part < 1 || cmd->part > 6)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 1;
        if (cmd->part > 1)
        {
            alarm.alarm_type = track_fun_atoi(cmd->pars[2]);
            if (alarm.alarm_type < 0 || alarm.alarm_type > 3)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1/2/3)");
                return;
            }
        }
        if (cmd->part > 2)
        {
            alarm.delay_time_off = track_fun_atoi(cmd->pars[3]);
            alarm.delay_time_off = track_fun_atoi(cmd->pars[3]);
            if (alarm.delay_time_off < 2 || alarm.delay_time_off > 3600)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (2 ~ 3600)");
                return;
            }
        }
        if (cmd->part > 3)
        {
            alarm.delay_time_on = track_fun_atoi(cmd->pars[4]);
            if (alarm.delay_time_on < 1 || alarm.delay_time_on > 3600)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 4 (1 ~ 3600)");
                return;
            }
        }
        if (cmd->part > 4)
        {
            alarm.delay_time_acc_on_to_off = track_fun_atoi(cmd->pars[5]);
            if (alarm.delay_time_acc_on_to_off < 0 || alarm.delay_time_acc_on_to_off > 3600)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 5 (0 ~ 3600)");
                return;
            }
        }
        if (cmd->part > 5)
        {
            alarm.send_alarm_time = track_fun_atoi(cmd->pars[6]);
            if ((alarm.send_alarm_time < 5 || alarm.send_alarm_time > 3600) && alarm.send_alarm_time != 0)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 6 (0,5 ~ 3600)");
                return;
            }
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.power_fails_alarm, &alarm, sizeof(nvram_power_fails_alarm_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

#else
/******************************************************************************
 *  Function    -  cmd_power_fails_alarm
 *
 *  Purpose     -  断电报警
 *
 *  Description -
        POWERALM,<A>[,T[,M[,S]]]#
            A =ON/OFF；默认值：ON
            M =0 - 2；0 仅GPRS，1 SMS+GPRS，2 GPRS+SMS+电话；默认值：2；
            T1=2 - 3600，单位：秒，默认值：5,通电时间
            T2=2 - 3600，单位：秒；默认值：5，断开时间
            T3=1 - 3600 秒；ACC ON to OFF 跳变禁止报警时间
        POWERALM,OFF#
            关闭断电报警
        POWERALM#
            查询已设置的参数
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_poweralm(CMD_DATA_STRUCT *cmd)
{
    nvram_power_fails_alarm_struct alarm = {0};

#if defined (__NO_ACC__)
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d,%d,%d", cmd->pars[0],
                ON_OFF(G_parameter.power_fails_alarm.sw),
                G_parameter.power_fails_alarm.alarm_type,
                G_parameter.power_fails_alarm.delay_time_off,
                G_parameter.power_fails_alarm.delay_time_on);
        return;
    }
#else
if (cmd->part == 0)
{
    sprintf(cmd->rsp_msg, "%s:%s,%d,%d,%d,%d", cmd->pars[0],
            ON_OFF(G_parameter.power_fails_alarm.sw),
            G_parameter.power_fails_alarm.alarm_type,
            G_parameter.power_fails_alarm.delay_time_off,
            G_parameter.power_fails_alarm.delay_time_on,
            G_parameter.power_fails_alarm.delay_time_acc_on_to_off);
    return;
}
#endif

    memcpy(&alarm, &G_parameter.power_fails_alarm, sizeof(nvram_power_fails_alarm_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part < 1 || cmd->part > 5)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 1;
        if (cmd->part > 1)
        {
            alarm.alarm_type = track_fun_atoi(cmd->pars[2]);
#if defined (__ET200__)||defined(__ET130__)||defined(__GT500__)||defined(__GT06D__)|| defined(__V20__) || defined(__ET310__)||defined(__GT300S__) || defined(__MT200__)||defined(__NT36__)||defined (__ET320__)||defined(__NT37__)
            if (alarm.alarm_type < 0 || alarm.alarm_type > 3)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1/2/3)");
                return;
            }
#else
if (alarm.alarm_type < 0 || alarm.alarm_type > 2)
{
    sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1/2)");
    return;
}
#endif /* __ET200__ */
        }
        if (cmd->part > 2)
        {
            alarm.delay_time_off = track_fun_atoi(cmd->pars[3]);
#if defined(__ET310__)||defined(__NT31__)||defined(__NT36__)
            alarm.delay_time_off = track_fun_atoi(cmd->pars[3]);
            if (alarm.delay_time_off < 2 || alarm.delay_time_off > 3600)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (2 ~ 3600)");
                return;
            }
#else
if (alarm.delay_time_off < 2 || alarm.delay_time_off > 60)
{
    sprintf(cmd->rsp_msg, "Error: Parameter 3 (2 ~ 60)");
    return;
}
#endif /* __ET310__ */
        }
        if (cmd->part > 3)
        {
            alarm.delay_time_on = track_fun_atoi(cmd->pars[4]);
            if (alarm.delay_time_on < 1 || alarm.delay_time_on > 3600)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 4 (1 ~ 3600)");
                return;
            }
        }
        if (cmd->part > 4)
        {
            alarm.delay_time_acc_on_to_off = track_fun_atoi(cmd->pars[5]);
            if (alarm.delay_time_acc_on_to_off < 0 || alarm.delay_time_acc_on_to_off > 3600)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 5 (0 ~ 3600)");
                return;
            }
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.power_fails_alarm, &alarm, sizeof(nvram_power_fails_alarm_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
#if defined(__ET310__)||defined (__ET320__)|| defined(__GT370__)|| defined (__GT380__)|| defined (__NT37__)|| defined (__NT33__)
    track_cust_paket_9404_handle();
#endif
}
#endif

/******************************************************************************
 *  Function    -  cmd_speed
 *
 *  Purpose     -  超速提醒设置
 *
 *  Description -
        SPEED,<A>,<B>,<C>,[M]#
            A=ON/OFF；超速报警；默认值：OFF
            B=5 - 600秒；时间范围；默认值：20秒
            C=1 - 255km/h；超速门限范围；默认值：100km/h
            M=0/1；报警上报方式，0 仅GPRS，1 SMS+GPRS；默认值：1
        SPEED#
            查询已设置的参数
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_speed(CMD_DATA_STRUCT *cmd)
{
    nvram_speed_limit_alarm_struct speed_limit_alarm = {0};
    int value = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d,%d,%d", cmd->pars[0],
                ON_OFF(G_parameter.speed_limit_alarm.sw), G_parameter.speed_limit_alarm.delay_time,
                G_parameter.speed_limit_alarm.threshold, G_parameter.speed_limit_alarm.alarm_type);
        return;
    }
    if (track_cust_get_work_mode() == WORK_MODE_3)
    {
        snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, string3_en);
        return;
    }
    memcpy(&speed_limit_alarm, &G_parameter.speed_limit_alarm, sizeof(nvram_speed_limit_alarm_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part > 4)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        speed_limit_alarm.sw = 1;
        if (cmd->part >= 2)
        {
            value = track_fun_atoi(cmd->pars[2]);
            if (value < 5 || value > 600)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (5-600)");
                return;
            }
            speed_limit_alarm.delay_time = value;
        }
        if (cmd->part >= 3)
        {
            value = track_fun_atoi(cmd->pars[3]);
            if (value < 1 || value > 255)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (1-255)");
                return;
            }
            speed_limit_alarm.threshold = value;
        }
        if (cmd->part == 4)
        {
            value = track_fun_atoi(cmd->pars[4]);
#if defined(__ET310__)||defined(__NT36__)||defined (__ET320__)||defined(__MT200__)
            if (value < 0 || value > 1)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 4 (0-1)");
                return;
            }
#else
            if (value < 0 || value > 3)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 4 (0-3)");
                return;
            }
#endif /* __ET310__ */
            speed_limit_alarm.alarm_type = value;
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        speed_limit_alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.speed_limit_alarm, &speed_limit_alarm, sizeof(nvram_speed_limit_alarm_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
#if defined(__ET310__)||defined (__ET320__)|| defined(__GT370__)|| defined (__GT380__)|| defined (__NT37__)|| defined (__NT33__)
    track_cust_paket_9404_handle();
#endif
    track_cust_module_speed_limit_cmd_update();
}

/******************************************************************************
 *  Function    -  cmd_vibrates_alarm
 *
 *  Purpose     -  震动报警
 *
 *  Description -
        SENALM,<A>[,M]#
            A=ON/OFF；默认值：OFF
            M=0/1；报警上报方式，0 仅GPRS，1 SMS+GPRS；默认值：0
        SENALM,OFF#
            关闭震动报警
        SENALM#
            查询已设置的参数
 *
 2016/8/24 11:15:16 经过与谭工沟通，以下面的描述为准：
     在自动设防的状态下，震动报警功能开/关影响到设/撤防状态；
     在手动设防的状态下，震动报警功能开/关不会影响到设防（关闭震动报警会导致虽然处于设防模式，但无法触发震动报警）
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_senalm(CMD_DATA_STRUCT *cmd)
{
    nvram_vibrates_alarm_struct alarm = {0};
    kal_uint8 defences = 0;
    kal_int8 sw_tmp = 0;

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d", cmd->pars[0],
                ON_OFF(G_parameter.vibrates_alarm.sw), G_parameter.vibrates_alarm.alarm_type);
        return;
    }
    sw_tmp = G_parameter.vibrates_alarm.sw;
    memcpy(&alarm, &G_parameter.vibrates_alarm, sizeof(nvram_vibrates_alarm_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part < 1 || cmd->part > 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 1;
        if (cmd->part >= 2)
        {
            alarm.alarm_type = track_fun_atoi(cmd->pars[2]);
#if defined(__GT06A__)
            if (alarm.alarm_type != 0 && alarm.alarm_type != 1)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1)");
                return;
            }
#elif defined (__ET200__)||defined(__GT370__)|| defined (__GT380__)||defined(__GT500__)||defined(__GT06D__)|| defined(__V20__)  || defined(__ET310__)||defined (__ET320__)||defined(__GT300S__) || defined(__MT200__)||defined(__NT36__)||defined(__NT33__)||defined(__NT37__)
            if (alarm.alarm_type < 0 || alarm.alarm_type > 3)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1/2/3)");
                return;
            }
#else
            if (alarm.alarm_type != 0 && alarm.alarm_type != 1 && alarm.alarm_type != 2)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1/2)");
                return;
            }
#endif /* __GT06A__ */
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.vibrates_alarm, &alarm, sizeof(nvram_vibrates_alarm_struct));
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
#if defined(__ET310__)||defined (__ET320__)|| defined(__GT370__)|| defined (__GT380__)|| defined (__NT37__)|| defined (__NT33__)
    track_cust_paket_9404_handle();
#endif
#if defined(__GT03D__) || defined(__GT300__)||defined(__GT300S__)||defined(__GT370__)|| defined (__GT380__)
    /*手持机设防有单独指令控制,震动报警关闭时更新设防状态*/
    if (G_parameter.vibrates_alarm.sw == 0)
    {
        track_cust_defences_cmd_update();
    }
#elif defined(__GT500__) || defined(__ET200__)||defined(__ET130__)||defined(__NT23__)|| defined(__V20__)||defined(__ET310__) ||defined (__ET320__)|| defined(__MT200__)
    defences = track_cust_status_defences();
    if (G_realtime_data.defense_mode == 0 && G_parameter.vibrates_alarm.sw != sw_tmp)
    {
        track_cust_defences_cmd_update();
    }
    LOGD(L_APP, L_V5, "defences=%d,%d,%d", defences, sw_tmp, G_parameter.vibrates_alarm.sw);
    if (defences != 0 && G_parameter.vibrates_alarm.sw == 0 && sw_tmp == 1)
    {
        track_cust_check_sos_incall(cmd, 3);
        return;
    }
#else
    //无ACC自动控制的
    track_cust_defences_cmd_update();
#endif/*__GT03D__*/
    sprintf(cmd->rsp_msg, "OK!");
}
static void cmd_distance(CMD_DATA_STRUCT *cmd)
{
    nvram_gps_work_struct tmp = {0};
    int value = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], G_parameter.gps_work.Fd.distance);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    memcpy(&tmp, &G_parameter.gps_work, sizeof(nvram_gps_work_struct));
    tmp.Fti.sw = 0;
    tmp.Fd.sw = 1;
    value = track_fun_atoi(cmd->pars[1]);
    if ((value < 50 || value > 10000) && value != 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0, 50-10000)");
        return;
    }
    tmp.Fd.distance = value;
    memcpy(&G_parameter.gps_work, &tmp, sizeof(nvram_gps_work_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

/******************************************************************************
 *  Function    -  cmd_accrep
 *
 *  Purpose     -  ACC 状态变化补传
 *
 *  Description -
        ACCREP,X#
            X=ON/OFF，状态变化补传，默认开启
        ACCREP#
            查询当前ACC状态变化补传功能开启还是关闭
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 08-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_accrep(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], ON_OFF(G_parameter.gps_work.accrep));
        return;
    }
    else if (cmd->part != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (!strcmp(cmd->pars[1], "ON"))
        G_parameter.gps_work.accrep = 1;
    else if (!strcmp(cmd->pars[1], "OFF"))
        G_parameter.gps_work.accrep = 0;
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

#elif defined(__GT420D__)
static void cmd_simalm(CMD_DATA_STRUCT *cmd)
{
    int value = 0;
    nvram_sim_alm_struct alarm = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s mode:%d(0=only GPRS)", cmd->pars[0],
                ON_OFF(G_parameter.sim_alarm.sw), G_parameter.sim_alarm.simalm_type);
        LOGD(L_CMD, L_V5, "IMSI=%s", G_parameter.sim_alarm.sim_imsi);
        return;
    }
    memcpy(&alarm, &G_parameter.sim_alarm, sizeof(nvram_sim_alm_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part != 1 && cmd->part != 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 1;
        if (cmd->part == 2)
        {
            if (!strcmp(cmd->pars[2], "0"))
            {
                alarm.simalm_type = 0;
            }
            else
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0)");
                return;
            }
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.sim_alarm, &alarm, sizeof(nvram_sos_alarm_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

#endif/*!__GT740__*/
#if !defined(__GT420D__)
/******************************************************************************
 *  Function    -  cmd_batch
 *
 *  Purpose     -  GPS 数据打包发送设置
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_batch(CMD_DATA_STRUCT *cmd)
{
    nvram_group_package_struct group_package = {0};
    int value = 0;//, j = 1000 / BACKUP_DATA_PACKET_MAX
    kal_bool flag = KAL_FALSE;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d", cmd->pars[0], ON_OFF(G_parameter.group_package.sw), G_parameter.group_package.at_least_pack);
        return;
    }
    memcpy(&group_package, &G_parameter.group_package, sizeof(nvram_group_package_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part > 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        group_package.sw = 1;
        if (cmd->part > 1)
        {
            value = track_fun_atoi(cmd->pars[2]);
            if (value < 1 || value > 50)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (1-50)");
                return;
            }
            group_package.at_least_pack = value;
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part > 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        group_package.sw = 0;
    }
    else if (!strcmp(cmd->pars[1], "ON2"))
    {
        if (cmd->part == 1)
        {
            sprintf(cmd->rsp_msg, "%s:%s,%d,%d", cmd->pars[0], ON_OFF(G_parameter.group_package.sw), G_parameter.group_package.at_least_pack, G_parameter.group_package.batch_paket);
            return;
        }
        if (cmd->part > 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        if (cmd->part > 1)
        {
            group_package.batch_paket = track_fun_atoi(cmd->pars[2]);
            if (group_package.batch_paket < 1 || group_package.batch_paket > 50)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (2-50)");
                return;
            }
            flag = KAL_TRUE;
        }
        else
        {
            sprintf(cmd->rsp_msg, "%s:batch_paket=%d", cmd->pars[0], G_parameter.group_package.batch_paket);
            return;
        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.group_package, &group_package, sizeof(nvram_group_package_struct));
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    if (flag) track_soc_set_batch_paket(G_parameter.group_package.batch_paket);
    sprintf(cmd->rsp_msg, "OK!");
}
#endif

/******************************************************************************
 *  Function    -  cmd_dsreset
 *
 *  Purpose     -  重置设防状态指令
 *
 *  Description -
        DSRESET#
            强制设置撤防状态

2016/8/23 15:47:01 康凯丝 谭志强:
    DSRESET是暂时取消设防状态的一个指令。比如ACC OFF ，进入设防，发这个指令可撤销设防状态，而当后续ACC 再一次由ON切换为OFF时，又会进入设防。
    DSRESET 这个指令目前很少用。作用跟000不完全一样
    主要在自动模式起作用，在手动模式下，作用就于000差不多
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_dsreset(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        if (G_realtime_data.defense_mode == 0)
        {
            track_cust_status_defences_change((void*)0);
            sprintf(cmd->rsp_msg, "OK!");
        }
        else
        {
            track_cust_check_sos_incall(cmd, 0);
        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
}

/******************************************************************************
 *  Function    -  cmd_clear
 *
 *  Purpose     -  备份数据清除指令
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_clear(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        track_soc_clear_queue();
        sprintf(cmd->rsp_msg, "OK!");
        return;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
}

/******************************************************************************
 *  Function    -  cmd_cleargps
 *
 *  Purpose     -  GPS星历清除指令
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_cleargps(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
#if defined(__MNL_SUPPORT__)
        track_drv_sys_mtk_gps_sys_storage_clear();
        sprintf(cmd->rsp_msg, "OK!");
#endif /* __MNL_SUPPORT__ */
        return;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }

}

static void cmd_dwdepo(CMD_DATA_STRUCT *cmd)
{
    int value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], ON_OFF(G_parameter.dwd_epo_sw));
        return;
    }
    if (!strcmp(cmd->pars[1], "ON"))
    {
        G_parameter.dwd_epo_sw = 1;
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        G_parameter.dwd_epo_sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

/******************************************************************************
 *  Function    -  cmd_sf
 *
 *  Purpose     -  静态数据过滤开关
 *
 *  Description -
        SF,X#
            X=ON/OFF，默认值：ON
        SF#
            查询已设置的参数
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_sf(CMD_DATA_STRUCT *cmd)
{
    int value;
    U8 static_filter_sw;
    U16 static_filter_distance;
    if (cmd->part == 0 && G_parameter.gps_work.static_filter_sw != 5)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d", cmd->pars[0], ON_OFF(G_parameter.gps_work.static_filter_sw),
                G_parameter.gps_work.static_filter_distance);
        return;
    }
    else if (cmd->part == 0 && G_parameter.gps_work.static_filter_sw == 5)
    {
        sprintf(cmd->rsp_msg, "%s:ON2,%d", cmd->pars[0], G_parameter.gps_work.static_filter_distance);
        return;
    }
    static_filter_sw = G_parameter.gps_work.static_filter_sw;
    static_filter_distance = G_parameter.gps_work.static_filter_distance;
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part > 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        if (cmd->part >= 1)
        {
            static_filter_sw = 1;
        }
        if (cmd->part > 1)
        {
            value  = track_fun_atoi(cmd->pars[2]);
            if (value < 10 || value > 1000)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (10 to 1000)");
                return;
            }
            static_filter_distance = value;
        }
    }
    else if (!strcmp(cmd->pars[1], "ON2"))
    {
        if (cmd->part > 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        if (cmd->part >= 1)
        {
            static_filter_sw = 5;
        }
        if (cmd->part > 1)
        {
            value  = track_fun_atoi(cmd->pars[2]);
            if (value < 10 || value > 1000)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (10 to 1000)");
                return;
            }
            static_filter_distance = value;
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        static_filter_sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    G_parameter.gps_work.static_filter_sw = static_filter_sw;
    G_parameter.gps_work.static_filter_distance = static_filter_distance;
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

#ifdef __WIFI__
static void cmd_WIFION(CMD_DATA_STRUCT *cmd)
{

    track_drv_wifi_work_mode();
    sprintf(cmd->rsp_msg, "OK!");

}
static void cmd_WF(CMD_DATA_STRUCT *cmd)
{
    int value;
    nvram_wifi_struct wifi = {0};
    memcpy(&wifi, &G_parameter.wifi, sizeof(nvram_wifi_struct));

    if (cmd->part == 0)
    {
#if defined(__GT740__)||defined (__GT370__)|| defined (__GT380__)||defined(__GT420D__)
        sprintf(cmd->rsp_msg, "%s:%s,%d", cmd->pars[0], ON_OFF(G_parameter.wifi.sw),
                G_parameter.wifi.sacn_timer);
        track_drv_wifi_work_mode();
#else
        sprintf(cmd->rsp_msg, "%s:%s,%d,%d", cmd->pars[0], ON_OFF(G_parameter.wifi.sw),
                G_parameter.wifi.sacn_timer, G_parameter.wifi.work_timer);
#endif

        return;
    }
    if (!strcmp(cmd->pars[1], "ON"))
    {
        wifi.sw = 1;
        if (cmd->part > 1)
        {
            value  = track_fun_atoi(cmd->pars[2]);
            if (value < 1 || value > 60)
            {
                #if !defined(__GT420D__)
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (1 to 60)");
                return;
                #else
                sprintf(cmd->rsp_msg, "FAIL");
                return;
                #endif
            }
            wifi.sacn_timer = value;
        }
        if (cmd->part > 2)
        {
#if defined(__GT740__)||defined (__GT370__)|| defined (__GT380__)||defined(__GT420D__)
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
#else
            value  = track_fun_atoi(cmd->pars[3]);
            if (value < 10 || value > 18000)
            {
                #if !defined(__GT420D__)
                sprintf(cmd->rsp_msg, "Error: Parameter 3(10 to 18000)");
                return;
                #else
                sprintf(cmd->rsp_msg, "FAIL");
                return;
                #endif
            }
            wifi.work_timer = value;
#endif
        }
        if (cmd->part > 3)
        {
            #if !defined(__GT420D__)
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
            #else
            sprintf(cmd->rsp_msg, "FAIL");
            return;
            #endif
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            #if !defined(__GT420D__)
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            #else
            sprintf(cmd->rsp_msg, "FAIL");
            #endif
            return;
        }
        wifi.sw = 0;
    }
    else
    {
        #if !defined(__GT420D__)
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        #else
        sprintf(cmd->rsp_msg, "FAIL");
        #endif
        return;
    }
    memcpy(&G_parameter.wifi, &wifi, sizeof(nvram_wifi_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        #if !defined(__GT420D__)
        sprintf(cmd->rsp_msg, "OK!");
        #else
        sprintf(cmd->rsp_msg, "SUCCESS");
        #endif
    }
    else
    {
        #if !defined(__GT420D__)
        sprintf(cmd->rsp_msg, "Error!");
        #else
        sprintf(cmd->rsp_msg, "FAIL");
        #endif
    }

    #if !defined(__GT420D__)
    if (G_parameter.wifi.sw == 1)
    {
        track_drv_wifi_work_mode();
    }
    #endif
}
#endif /* __WIFI__ */

/******************************************************************************
 *  Function    -  cmd_eph
 *
 *  Purpose     -  GPS 星历更新时间设置
 *
 *  Description -
        EPH,<A>#
            A=60 - 240分钟，默认值：60分钟
        EPH#
            查询已设置的参数
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_eph(CMD_DATA_STRUCT *cmd)
{
    int value = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], G_parameter.gps_work.ephemeris_interval);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (!strcmp(cmd->pars[1], "OFF"))
    {
        value = 0;
    }
    else
    {
        value = track_fun_atoi(cmd->pars[1]);
        if (value < 60 || value > 240)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (60-240)");
            return;
        }
    }
    G_parameter.gps_work.ephemeris_interval = value;
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

/******************************************************************************
 *  Function    -  cmd_call
 *
 *  Purpose     -  电话重拨次数
 *
 *  Description -
        CALL,N#
            N=1 - 3；默认值：3；针对所有电话报警
        CALL#
            查询已设置的参数
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_call(CMD_DATA_STRUCT *cmd)
{
    int value = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], G_parameter.redial_count);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    value = track_fun_atoi(cmd->pars[1]);
    if (value < 1 || value > 3)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (1-3)");
        return;
    }
    G_parameter.redial_count = value;
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

/******************************************************************************
 *  Function    -  cmd_moving
 *
 *  Purpose     -  位移报警设置
 *
 *  Description -
 *      MOVING,<A>,<R>[,M]#
 *          A=ON/OFF；默认值：OFF
 *          R=100 - 1000；默认值：300；位移半径
 *          M=0 - 2；0 仅GPRS，1 SMS+GPRS，2 GPRS+SMS+电话；默认值：1；
 *      MOVING,OFF#
 *          关闭位移报警
 *      MOVING#
 *          查询当前位移开启还是关闭的状态，设置的半径，报警方式，位移原点
 *
 *  Example    -
 *      MOVING,ON,100#
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_moving(CMD_DATA_STRUCT *cmd)
{
    nvram_displacement_alarm_struct displacement_alarm = {0};
    int value = 0;
    if (cmd->part == 0 || (cmd->part == 1 && !strcmp(cmd->pars[1], "STATUS")))
    {
        if (G_parameter.displacement_alarm.sw)
        {
            if (G_realtime_data.latitude == 0 || G_realtime_data.longitude == 0)
            {
#if defined(__GERMANY__)
                sprintf(cmd->rsp_msg, "Bewegungsalarm:%s; Lat:NONE; Lon:NONE; Radius:%dm; Alarmtyp:%d",
                        ON_OFF(G_parameter.displacement_alarm.sw),
                        G_parameter.displacement_alarm.radius, G_parameter.displacement_alarm.alarm_type);
#else
                sprintf(cmd->rsp_msg, "Moving Switch:%s; Lat:NONE; Lon:NONE; Radius:%dm; Alarm type:%d",
                        ON_OFF(G_parameter.displacement_alarm.sw),
                        G_parameter.displacement_alarm.radius, G_parameter.displacement_alarm.alarm_type);
#endif
            }
            else
            {
                char EW[2] = {0}, NS[2] = {0};
                float lon, lat;
                if (G_realtime_data.longitude > 0)
                {
                    if (G_realtime_data.longitude != 0) *EW = 'E';
                    lon = G_realtime_data.longitude;
                }
                else
                {
                    if (G_realtime_data.longitude != 0) *EW = 'W';
                    lon = G_realtime_data.longitude * -1;
                }
                if (G_realtime_data.latitude > 0)
                {
                    if (G_realtime_data.latitude != 0) *NS = 'N';
                    lat = G_realtime_data.latitude;
                }
                else
                {
                    if (G_realtime_data.latitude != 0) *NS = 'S';
                    lat = G_realtime_data.latitude * -1;
                }
#if defined(__GERMANY__)
                sprintf(cmd->rsp_msg, "Bewegungsalarm:%s; Lat:%s%.6f; Lon:%s%.6f; Radius:%dm; Alarmtyp:%d",
                        ON_OFF(G_parameter.displacement_alarm.sw),
                        NS, lat, EW, lon,
                        G_parameter.displacement_alarm.radius, G_parameter.displacement_alarm.alarm_type);

#else
                sprintf(cmd->rsp_msg, "Moving Switch:%s; Lat:%s%.6f; Lon:%s%.6f; Radius:%dm; Alarm type:%d",
                        ON_OFF(G_parameter.displacement_alarm.sw),
                        NS, lat, EW, lon,
                        G_parameter.displacement_alarm.radius, G_parameter.displacement_alarm.alarm_type);
#endif
                if (!strcmp(cmd->pars[1], "STATUS"))
                {
                    char *p;
                    kal_uint8 gpsStatus;
                    p = cmd->rsp_msg + strlen(cmd->rsp_msg);
                    gpsStatus = track_cust_gps_status();
                    if (gpsStatus == GPS_STAUS_OFF)
                    {
                        sprintf(p, " (GPS Close!)");
                    }
                    else if (gpsStatus == GPS_STAUS_ON)
                    {
                        sprintf(p, " (GPS ON!)");
                    }
                    else if (gpsStatus == GPS_STAUS_SCAN)
                    {
                        sprintf(p, " (GPS Searching!)");
                    }
                    else if (gpsStatus >= GPS_STAUS_2D)
                    {
                        float res = 0;
                        res = track_fun_get_GPS_2Points_distance(
                                  G_realtime_data.latitude, G_realtime_data.longitude,
                                  track_drv_get_gps_RMC()->Latitude, track_drv_get_gps_RMC()->Longitude);
                        sprintf(p, " (Distance far point:%.2fM)", res);
                    }
                }
            }
        }
        else
        {
#if defined(__GERMANY__)
            sprintf(cmd->rsp_msg, "Bewegungsalarm:%s; Radius:%dm; Alarmtyp:%d",
                    ON_OFF(G_parameter.displacement_alarm.sw),
                    G_parameter.displacement_alarm.radius, G_parameter.displacement_alarm.alarm_type);
#else
            sprintf(cmd->rsp_msg, "Moving Switch:%s; Radius:%dm; Alarm type:%d",
                    ON_OFF(G_parameter.displacement_alarm.sw),
                    G_parameter.displacement_alarm.radius, G_parameter.displacement_alarm.alarm_type);
#endif
        }
        return;
    }
    memcpy(&displacement_alarm, &G_parameter.displacement_alarm, sizeof(nvram_displacement_alarm_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part < 1 || cmd->part > 3)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        displacement_alarm.sw = 1;
        if (cmd->part >= 2)
        {
            value = track_fun_atoi(cmd->pars[2]);
            if (value < 100 || value > 1000)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (100-1000)");
                return;
            }
            displacement_alarm.radius = value;
        }
        if (cmd->part == 3)
        {
            value = track_fun_atoi(cmd->pars[3]);
#if defined (__ET200__)||defined(__ET130__)||defined(__GT500__)||defined(__GT06D__)|| defined(__V20__)  || defined(__ET310__)||defined (__ET320__) || defined(__MT200__)||defined(__NT36__)||defined(__NT37__)
            if (value < 0 || value > 3)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (0/1/2/3)");
                return;
            }
#else
            if (value < 0 || value > 2)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (0/1/2)");
                return;
            }
#endif /* __ET200__ */

            displacement_alarm.alarm_type = value;
        }
        if (track_cust_status_acc() == 1)
        {
            track_os_intoTaskMsgQueueExt(track_cust_module_sd_acc, (void *)4);
        }
        else
        {
            track_os_intoTaskMsgQueueExt(track_cust_module_sd_acc, (void *)3);
        }
        if (G_parameter.lang == 1 && cmd->return_sms.cm_type != CM_ONLINE && cmd->return_sms.cm_type != CM_ONLINE2)
        {
            char buf[320] = {0};
            int ret;
            sprintf(buf, "设置成功! 位移报警开启；位移半径：%dM；报警类型：%d",
                    displacement_alarm.radius, displacement_alarm.alarm_type);
            if (cmd->return_sms.cm_type == CM_AT)
            {
                cmd->rsp_length = strlen(buf);
                memcpy(cmd->rsp_msg, buf, cmd->rsp_length);
                cmd->return_sms.reply_character_encode = 2;
            }
            else
            {
                cmd->rsp_length = track_fun_GB2312_to_UTF16BE(buf, strlen(buf), cmd->rsp_msg, 320);
                cmd->return_sms.reply_character_encode = 1;
            }
        }
        else
        {
#if defined(__GERMANY__)
            sprintf(cmd->rsp_msg, "Bewegungsalarm:EIN; Radius:%dm; Alarmtyp:%d",
                    displacement_alarm.radius, displacement_alarm.alarm_type);
#else
            sprintf(cmd->rsp_msg, "OK! Moving Switch:ON; Radius:%dm; Alarm type:%d",
                    displacement_alarm.radius, displacement_alarm.alarm_type);
#endif
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        displacement_alarm.sw = 0;
        track_cust_module_sd_stop();
        if (G_parameter.lang == 1 && cmd->return_sms.cm_type != CM_ONLINE && cmd->return_sms.cm_type != CM_ONLINE2)
        {
            char buf[320] = {0};
            int ret;
            sprintf(buf, "设置成功！位移报警已关闭。");
            if (cmd->return_sms.cm_type == CM_AT)
            {
                cmd->rsp_length = strlen(buf);
                memcpy(cmd->rsp_msg, buf, cmd->rsp_length);
                cmd->return_sms.reply_character_encode = 2;
            }
            else
            {
                cmd->rsp_length = track_fun_GB2312_to_UTF16BE(buf, strlen(buf), cmd->rsp_msg, 320);
                cmd->return_sms.reply_character_encode = 1;
            }
        }
        else
        {
#if defined(__GERMANY__)
            sprintf(cmd->rsp_msg, "Bewegungsalarm: AUS.",
                    displacement_alarm.radius, displacement_alarm.alarm_type);
#else
            sprintf(cmd->rsp_msg, "OK! Moving Switch:OFF.",
                    displacement_alarm.radius, displacement_alarm.alarm_type);
#endif

        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.displacement_alarm, &displacement_alarm, sizeof(nvram_displacement_alarm_struct));
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    //if(!strlen(cmd->rsp_msg)) sprintf(cmd->rsp_msg, "OK!");
#if defined(__ET310__)||defined (__ET320__)|| defined(__GT370__)|| defined (__GT380__)|| defined (__NT37__)|| defined (__NT33__)
    track_cust_paket_9404_handle();
#endif
}

static void cmd_gprsalm(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], ON_OFF(G_parameter.gprs_obstruction_alarm.sw));
        return;
    }
    else if (track_cust_get_work_mode() == WORK_MODE_3)
    {
        snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, string3_en);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (!strcmp(cmd->pars[1], "ON"))
    {
        G_parameter.gprs_obstruction_alarm.sw = 1;
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        G_parameter.gprs_obstruction_alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

/******************************************************************************
 *  Function    -  cmd_staticrep
 *
 *  Purpose     -  静止上报最后位置开关指令
 *
 *  Description -
 *                 STATICREP,<A>[,B[,C]]#
 *                     A=ON/OFF；SENSOR 检测静止上报最后位置开关；默认值：OFF
 *                     B=10 - 300 秒；时间范围；默认值：180 秒
 *                     C=0 - 100 km/h，最小速度
 *                     D=0 - 20 连续监测的个数
 *                 STATICREP#
 *                     查询 STATICREP# 设置参数
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 01-03-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_staticrep(CMD_DATA_STRUCT *cmd)
{
    nvram_staticrep_struct staticrep = {0};
    int value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d,%d,%d", cmd->pars[0], ON_OFF(G_parameter.staticrep.sw),
                G_parameter.staticrep.detection_time, G_parameter.staticrep.speed, G_parameter.staticrep.count);
        return;
    }
    memcpy(&staticrep, &G_parameter.staticrep, sizeof(nvram_staticrep_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part > 4)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        staticrep.sw = 1;
        if (cmd->part > 1)
        {
            value  = track_fun_atoi(cmd->pars[2]);
            if (value < 10 || value > 300)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (10 to 300)");
                return;
            }
            staticrep.detection_time = value;
        }
        if (cmd->part > 2)
        {
            value  = track_fun_atoi(cmd->pars[3]);
            if (value < 0 || value > 100)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (0 to 100)");
                return;
            }
            staticrep.speed = value;
        }
        if (cmd->part > 3)
        {
            value  = track_fun_atoi(cmd->pars[4]);
            if (value < 1 || value > 20)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 4 (1 to 20)");
                return;
            }
            staticrep.count = value;
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        staticrep.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.staticrep, &staticrep, sizeof(nvram_staticrep_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

/******************************************************************************
 *  Function    -  cmd_RENEW
 *
 *  Purpose     -  平台或SMS 短信透传短信内容
 *
 *  Description -  FW，设定号码，信息内容#
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 07-05-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_RENEW(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        char num[32] = {0};
        if (track_cust_sms_relay_status(1, num) == 1)
        {
            sprintf(cmd->rsp_msg, "%s:ON relay num:%s", cmd->pars[0], num);
        }
        else
        {
            sprintf(cmd->rsp_msg, "%s:OFF", cmd->pars[0]);
        }
        return;
    }
    else if (cmd->part < 2)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (track_fun_check_str_is_number(1, cmd->pars[2]) <= 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (phone number)");
        return;
    }
    else
    {
        char buf[320] = {0};
        int len;

        len = strlen(cmd->pars[1]);
        if (len > 318)
        {
            len = 318;
        }
        memcpy(buf, cmd->rcv_msg_source + (cmd->pars[1] - cmd->rcv_msg), len);
        if (cmd->return_sms.cm_type == CM_SMS)
        {
            track_cust_sms_relay_status(2, cmd->return_sms.ph_num);
            track_cust_sms_relay_status(cmd->return_sms.num_type, cmd->return_sms.ph_num);
        }
        else if (cmd->return_sms.cm_type == CM_ONLINE || cmd->return_sms.cm_type == CM_ONLINE2)
        {
            track_cust_sms_relay_status(3, cmd->return_sms.stamp);
        }
        if (cmd->character_encode == 1)
        {
            int ret;
            char buf1[320] = {0};
            ret = track_fun_GB2312_to_UTF16BE(buf, strlen(buf), buf1, 320);
            track_cust_sms_send(0, cmd->pars[2], KAL_FALSE, buf1, ret);
        }
        else
        {
            track_cust_sms_send(0, cmd->pars[2], KAL_TRUE, buf, strlen(buf));
        }
        LOGD(L_APP, L_V3, "%s", cmd->rcv_msg_source);
        cmd->rsp_msg[0] = 0;
    }
}

/******************************************************************************
 *  Function    -  cmd_fw
 *
 *  Purpose     -  平台或SMS 短信透传短信内容
 *
 *  Description -  FW，设定号码，信息内容#
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 07-05-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_fw(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        char num[32] = {0};
        if (track_cust_sms_relay_status(1, num) == 1)
        {
            sprintf(cmd->rsp_msg, "%s:ON relay num:%s", cmd->pars[0], num);
        }
        else if (track_cust_sms_relay_status(1, num) == 2)
        {
            sprintf(cmd->rsp_msg, "%s:ON ", cmd->pars[0]);
        }
        else
        {
            sprintf(cmd->rsp_msg, "%s:OFF", cmd->pars[0]);
        }
        return;
    }
    else if (cmd->part < 2)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (strlen(cmd->pars[1]) == 0 || track_fun_check_str_is_number(1, cmd->pars[1]) <= 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (phone number)");
        return;
    }
    else
    {
        char buf[320] = {0};
        int len;

        // len = strlen(cmd->pars[2]);
        //  len=len-1;
        //   len = strlen(cmd->rcv_msg_source + (cmd->pars[2] - cmd->rcv_msg));
        if (strlen(cmd->pars[2]) == 0)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (FW content can't null)");
            return;
        }
        if (cmd->return_sms.cm_type == CM_SMS)
        {
            len = strlen(cmd->rcv_msg_source + (cmd->pars[2] - cmd->rcv_msg)) - 1;
        }
        else
        {
            len = strlen(cmd->rcv_msg_source + (cmd->pars[2] - cmd->rcv_msg));
        }
        LOGD(L_APP, L_V3, "%d", len);
        if (len > 318)
        {
            len = 318;
        }
        memcpy(buf, cmd->rcv_msg_source + (cmd->pars[2] - cmd->rcv_msg), len);
        if (cmd->return_sms.cm_type == CM_SMS)
        {
            track_cust_sms_relay_status(2, cmd->return_sms.ph_num);
            track_cust_sms_relay_status(cmd->return_sms.num_type, cmd->return_sms.ph_num);
        }
        else if (cmd->return_sms.cm_type == CM_ONLINE || cmd->return_sms.cm_type == CM_ONLINE2)
        {
            track_cust_sms_relay_status(3, cmd->return_sms.stamp);
        }
        if (cmd->character_encode == 1)
        {
            int ret;
            char buf1[320] = {0};
            ret = track_fun_GB2312_to_UTF16BE(buf, strlen(buf), buf1, 320);
            track_cust_sms_send(0, cmd->pars[1], KAL_FALSE, buf1, ret);
        }
        else
        {
            track_cust_sms_send(0, cmd->pars[1], KAL_TRUE, buf, strlen(buf));
        }
        LOGD(L_APP, L_V3, "%s", cmd->rcv_msg_source);
        cmd->rsp_msg[0] = 0;
    }
}

#if defined (__GT740__)||defined (__GT370__)||defined (__GT380__)||defined (__JM81__)||defined(__GT420D__)
/*手持项目无需断油电相关功能       --    chengjun  2017-04-18*/
#else
static void cmd_motorset(CMD_DATA_STRUCT *cmd)
{
    nvram_motor_lock_struct motor_lock = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d,%d,%d", cmd->pars[0],
                ON_OFF(G_parameter.motor_lock.sw),
                G_parameter.motor_lock.lock_continue_time,
                G_parameter.motor_lock.lock_on_time,
                G_parameter.motor_lock.lock_off_time);
        return;
    }
    memcpy(&motor_lock, &G_parameter.motor_lock, sizeof(nvram_motor_lock_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part < 1 || cmd->part > 4)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        motor_lock.sw = 1;
        if (cmd->part > 1)
        {
            motor_lock.lock_continue_time = track_fun_atoi(cmd->pars[2]);
            if (motor_lock.lock_continue_time < 1 || motor_lock.lock_continue_time > 86400)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (1 ~ 86400)");
                return;
            }
        }
        if (cmd->part > 2)
        {
            motor_lock.lock_on_time = track_fun_atoi(cmd->pars[3]);
            if (motor_lock.lock_on_time < 40 || motor_lock.lock_on_time > 30000)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (40 ~ 30000)");
                return;
            }
        }
        if (cmd->part > 3)
        {
            motor_lock.lock_off_time = track_fun_atoi(cmd->pars[4]);
            if (motor_lock.lock_off_time < 40 || motor_lock.lock_off_time > 10000)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 4 (40 ~ 10000)");
                return;
            }
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        motor_lock.sw = 0;
        track_motor_lock_change_status(MOTOR_END_REQ);
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.motor_lock, &motor_lock, sizeof(nvram_motor_lock_struct));
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    track_cust_defences_cmd_update();
    sprintf(cmd->rsp_msg, "OK!");
}

static void cmd_motor1(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%d", track_cust_motor_lock(2));
        return;
    }
    else if (cmd->part != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (!strcmp(cmd->pars[1], "ON"))
    {
        track_cust_motor_lock(1);
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        track_cust_motor_lock(0);
    }
    else if (!strcmp(cmd->pars[1], "MODE0"))
    {
        if (G_parameter.motor_lock.mode != 0)
        {
            G_parameter.motor_lock.mode = 0;
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        }
    }
    else if (!strcmp(cmd->pars[1], "MODE1"))
    {
        if (G_parameter.motor_lock.mode != 1)
        {
            G_parameter.motor_lock.mode = 1;
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    sprintf(cmd->rsp_msg, "OK!");
}


/********************************************************************************

        RELAY,1#
            A=0/1；0 接通油电，1 断开油电；默认值为：0
        RELAY#
            查询已设置的参数
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_relay(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], track_cust_oil_cut(99));
        return;
    }
    else if (cmd->part != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
#if !defined(__SPANISH__)
#if defined(__MEX__)
    if (!strcmp(cmd->pars[0], "SRELAY"))
    {
        //墨西哥版本，任意号码可断油电。
    }
    else
#endif
    {
        if (cmd->return_sms.cm_type == CM_SMS && track_fun_convert_phone_number_to_int(cmd->return_sms.ph_num) != track_fun_convert_phone_number_to_int(G_parameter.centerNumber))
        {
#if defined(__GERMANY__)
            return_message(
                cmd,
                "SMS kommt nicht von der Center-Nr.und wird nicht aktiviert.",
                "来自非法中心号码的指令不执行！");
#else
return_message(
    cmd,
    "The command is not from the center number, The command is not executed!",
    "来自非法中心号码的指令不执行！");
#endif
            return;
        }
    }
#else//__SPANISH__
if (cmd->return_sms.cm_type == CM_SMS && (track_cust_sos_or_center_check(2, cmd->return_sms.ph_num) % 8) != 0 && cmd->supercmd != 1)
{
    return_message(
        cmd,
        "Error: El comando no se ejecutara. No fue enviado de un numero SOS!",
        "来自非法SOS号码的指令不执行！");
    return;
}
#endif/*__SPANISH__*/
#if defined(__GERMANY__)
    track_cust_module_Preheat_System(99);
#endif
#if defined(__NT51__)||defined (__NT37__)
    if (!strcmp(cmd->pars[0], "ERELAY"))
    {
        //MT200 ERELAY 断油电指令
        if (!strcmp(cmd->pars[1], "0"))//恢复油电
        {
            track_cust_module_oil_cut_cmd(cmd, KAL_FALSE, KAL_TRUE);
        }
        else if (!strcmp(cmd->pars[1], "1"))
        {
            track_cust_module_oil_cut_cmd(cmd, KAL_TRUE, KAL_TRUE);
        }
        else
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (0/1)");
        }
    }
    else
    {
        if (!strcmp(cmd->pars[1], "0"))
        {
            track_cust_module_oil_cut_cmd(cmd, KAL_FALSE, KAL_FALSE);
        }
        else if (!strcmp(cmd->pars[1], "1"))
        {
            track_cust_module_oil_cut_cmd(cmd, KAL_TRUE, KAL_FALSE);
        }
        else
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (0/1)");
        }
    }
#else
if (!strcmp(cmd->pars[1], "0"))
{
    track_cust_module_oil_cut_cmd(cmd, KAL_FALSE, KAL_FALSE);
}
else if (!strcmp(cmd->pars[1], "1"))
{
    track_cust_module_oil_cut_cmd(cmd, KAL_TRUE, KAL_FALSE);
}
else
{
    sprintf(cmd->rsp_msg, "Error: Parameter 1 (0/1)");
}
#endif
}
/********************************************************************************

        RELAY,1#
            A=0/1；0 接通油电，1 断开油电；默认值为：0
        RELAY#
            查询已设置的参数
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_prelay(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], track_cust_oil_cut(99));
        return;
    }
    else if (cmd->part != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
#if !defined(__SPANISH__)
    if (cmd->return_sms.cm_type == CM_SMS && track_fun_convert_phone_number_to_int(cmd->return_sms.ph_num) != track_fun_convert_phone_number_to_int(G_parameter.centerNumber))
    {
#if defined(__GERMANY__)
        return_message(
            cmd,
            "SMS kommt nicht von der Center-Nr.und wird nicht aktiviert.",
            "来自非法中心号码的指令不执行！");
#else
return_message(
    cmd,
    "The command is not from the center number, The command is not executed!",
    "来自非法中心号码的指令不执行！");
#endif
        return;
    }
#else
if (cmd->return_sms.cm_type == CM_SMS && (track_cust_sos_or_center_check(2, cmd->return_sms.ph_num) % 8) != 0 && cmd->supercmd != 1)
{
    return_message(
        cmd,
        "Error: El comando no se ejecutara. No fue enviado de un numero SOS!",
        "来自非法SOS号码的指令不执行！");
    return;
}
#endif/*__SPANISH__*/
    if (!strcmp(cmd->pars[1], "0"))
    {
        track_cust_module_oil_cut_cmd(cmd, KAL_FALSE, KAL_TRUE);
    }
    else if (!strcmp(cmd->pars[1], "1"))
    {
        track_cust_module_oil_cut_cmd(cmd, KAL_TRUE, KAL_TRUE);
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0/1)");
    }
}


/******************************************************************************
 *  Function    -  cmd_dyd
 *
 *  Purpose     -  油电控制
 *
 *  Description -  旧的指令，由于服务器使用了，被迫继续兼容；但不支持该短信指令
 *      DYD,000000#
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_dyd(CMD_DATA_STRUCT *cmd)
{
    if (cmd->return_sms.cm_type == CM_SMS) return;
    if (cmd->part != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (!strcmp(cmd->pars[1], "000000"))
    {
        track_cust_module_oil_cut_cmd(cmd, KAL_TRUE);
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: password error!");
        return;
    }
}
/******************************************************************************
 *  Function    -  cmd_frelay
 *
 *  Purpose     -  强制减速断油电
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-08-08,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void cmd_frelay(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], track_cust_oil_cut(99));
        return;
    }
    else if (cmd->part != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (cmd->return_sms.cm_type == CM_SMS && track_fun_convert_phone_number_to_int(cmd->return_sms.ph_num) != track_fun_convert_phone_number_to_int(G_parameter.centerNumber))
    {
        return_message(cmd, "The command is not from the center number, The command is not executed!", "来自非法中心号码的指令不执行！");
        return;
    }
    if (!strcmp(cmd->pars[1], "0"))
    {
        track_cust_module_oil_cut_pulse(cmd, 0);
    }
    else if (!strcmp(cmd->pars[1], "1"))
    {
        track_cust_module_oil_cut_pulse(cmd, 1);
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0/1)");
        return;
    }
}

/******************************************************************************
 *  Function    -  cmd_hdyd
 *
 *  Purpose     -  恢复油电
 *
 *  Description -  旧的指令，由于服务器使用了，被迫继续兼容；但不支持该短信指令
 *      HDYD,000000#
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_hdyd(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (!strcmp(cmd->pars[1], "000000"))
    {
        track_cust_module_oil_cut_cmd(cmd, KAL_FALSE);
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: password error!");
        return;
    }
}


/******************************************************************************
 *  Function    -  cmd_work_mode_set
 *
 *  Purpose     -  终端工作模式设置
 *
 *  Description -  NULL
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 07-08-2013,  ZengPing && Liujw  written
 * ----------------------------------------
 ******************************************************************************/
static void cmd_work_mode_set(CMD_DATA_STRUCT *cmd)
{
    int value;

    if (G_parameter.work_mode.sw == 0)
    {
        sprintf(cmd->rsp_msg, "Error:Mode Not On!");
        return;
    }
    if (cmd->part == 0)
    {
        value = G_parameter.work_mode.mode;
        LOGD(L_CMD, L_V5, "value =%d;work_mode.mode =%d ", value, G_parameter.work_mode.mode);

        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], value);
        return;
    }
    if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    value = track_fun_atoi(cmd->pars[1]);
#if defined(__GT300__)||defined(__GT300S__)
    if (value < 1 || value > 4)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter (1-4)");
        return;
    }
#else
if (value < 1 || value > 3)
{
    sprintf(cmd->rsp_msg, "Error: Parameter (1-3)");
    return;
}
#endif /* __GT300__ */


    G_parameter.work_mode.mode = value;
    if (value == 3)
    {
        track_cust_sleep_mode();
        track_cust_mode3_work_check((void*)0);
    }
    else
    {
        track_cust_wake_mode();
    }
    if (value != 1)
    {
        tr_stop_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID);
        tr_stop_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_OVERTIME_TIMER_ID);
    }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

/********************************************************************************

        RELAY,1#
            A=0/1；0 接通油电，1 断开油电；默认值为：0
          apagar# = relay,1#
        encender# = relay,0#

 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 30-3-2017,  Cjj  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_spanish_relay(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part > 0)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (cmd->return_sms.cm_type == CM_SMS && (track_cust_sos_or_center_check(2, cmd->return_sms.ph_num) % 8) != 0 && cmd->supercmd != 1)
    {
        return_message(
            cmd,
            "The command is not from the SOS number, The command is not executed!",
            "来自非法SOS号码的指令不执行！");
        return;
    }
    if (!strcmp(cmd->pars[0], "ENCENDER"))
    {
        cmd->pars[0] = "RELAY";
        cmd->pars[1] = "0";
        track_cust_module_oil_cut_cmd(cmd, KAL_FALSE, KAL_FALSE);
    }
    else if (!strcmp(cmd->pars[0], "APAGAR"))
    {
        cmd->pars[0] = "RELAY";
        cmd->pars[1] = "1";
        track_cust_module_oil_cut_cmd(cmd, KAL_TRUE, KAL_FALSE);
    }

}
#endif

static void cmd_ebat(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        kal_int32 external_voltage;
        float external_batter;
        external_voltage = track_drv_get_external_voltage_status();
        if (external_voltage == -1)
        {
            sprintf(cmd->rsp_msg, "External batter may over the range");
        }
        else if (external_voltage == 0)
        {
            sprintf(cmd->rsp_msg, "No external batter");
        }
        else
        {
            external_batter = (float)external_voltage / 1000000.0;
            sprintf(cmd->rsp_msg, "%s:%4.1fV", cmd->pars[0], external_batter);
        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
}


#if defined (__EXT_VBAT_ADC__)
static cmd_exbat(CMD_DATA_STRUCT *cmd)
{
    int current_ext_v = 0;
    if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    current_ext_v = track_drv_get_external_voltage_status();
    if (current_ext_v < 0) current_ext_v = 0;
    sprintf(cmd->rsp_msg, "Current vehicle battery voltage:%0.2fVdc", (float)current_ext_v / 1000000);
}

static cmd_exbat2(CMD_DATA_STRUCT *cmd)
{
    int current_ext_v = 0;
    if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    current_ext_v = track_drv_get_external_batter2();
    if (current_ext_v < 0) current_ext_v = 0;
    sprintf(cmd->rsp_msg, "Current vehicle battery voltage2:%0.2fVdc", (float)current_ext_v / 1000000);
}

#endif /* __EXT_VBAT_ADC__ */


static void cmd_delay_make_sos(void* index)
{
    if (strlen(G_parameter.sos_num[(int)index]))
    {
        track_drv_sys_audio_set_volume(2/*VOL_TYPE_MIC*/, 3/*GAIN_NOR_MIC_VOL6*/);
#if defined(__AUDIO_RC__)
        track_cust_monitor_set_speaket(1);
#endif /* __AUDIO_RC__ */
        track_cust_call(G_parameter.sos_num[(int)index]);
    }
    else
    {
        LOGD(L_APP, L_V5, "SOS %d 已经改变", index);
    }
}

#if !defined(__GT420D__)
/******************************************************************************
 *  Function    -  cmd_JT
 *
 *  Purpose     -  短信触发回拨监听
 *
 *  Description -  NULL
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 12-12M-2013,  WangQi  written
 * ----------------------------------------
 ******************************************************************************/
static void cmd_JT(CMD_DATA_STRUCT * cmd)
{
#if defined(__GW100__)
    if (cmd->return_sms.cm_type == CM_ONLINE || cmd->return_sms.cm_type == CM_AT)
    {
        track_cust_record(0, 1, 10);
        sprintf(cmd->rsp_msg, "OK!");
    }
    else if (cmd->return_sms.cm_type == CM_SMS)
    {
        if (cmd->supercmd == 1 || (track_cust_sos_or_center_check(2, cmd->return_sms.ph_num) %  8) == 0)
        {
            track_cust_record(0, 1, 10);
            sprintf(cmd->rsp_msg, "OK!");
        }
        else
        {
#if defined(__GERMANY__)
            sprintf(cmd->rsp_msg, "Fehler: SMS kommt nicht von der Center-Nr. und wird nicht aktiviert.");
#else
            sprintf(cmd->rsp_msg, "Error! Only SOS phone numbers are allowed to listen.");
#endif
        }
    }
#else
    kal_uint8 ret = track_cust_sos_or_center_check(2, cmd->return_sms.ph_num);
    kal_int8 index = 0;
#if defined(__GT740__)||defined(__GT420D__)
    if (G_parameter.extchip.mode != 1)
    {
        return;
    }
#endif
    LOGD(L_APP, L_V5, "mode:%d,ret:%d,index:%d", track_cust_get_work_mode(), ret, ret / 8);
    if (track_cust_get_work_mode() == WORK_MODE_3)
    {
        return;
    }

    if (cmd->supercmd == 1 || (ret %  8) == 0)
    {
        index = ret / 8 - 1;
        if (0 <= index && index < TRACK_DEFINE_SOS_COUNT)
        {
            sprintf(cmd->rsp_msg, "OK!");
            track_start_timer(TRACK_DELAY_MAKECALL_TEIMER_ID, 30000, cmd_delay_make_sos, (void *)index);
        }
    }
    else
    {
#if defined(__GERMANY__)
        sprintf(cmd->rsp_msg, "Fehler: SMS kommt nicht von der Center-Nr. und wird nicht aktiviert.");
#else
    sprintf(cmd->rsp_msg, "Error! Only SOS phone numbers are allowed to listen.");
#endif
    }
#endif /* __GW100__ */
}

/******************************************************************************
 *  Function    -  cmd_SLEEPSET
 *  Purpose     T为休眠间隔，范围：0-1000小时，,0,表示持续休眠（除非退出休眠模式），默认：0
                N为上传点数, 范围：1-10，默认为1；（当T1为非0时）
                （当需要传一个以上的点时，上传间隔为1秒）；
 *  Description -  NULL
 * modification history
 * ----------------------------------------
 * v1.0  , 20141117,  WangQi  written
 * ----------------------------------------
 ******************************************************************************/
static void cmd_SLEEPSET(CMD_DATA_STRUCT * cmd)
{
    int value = 0, value1 = 0;

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "SLEEPSET,%d,%d", G_parameter.mode3_work_interval / 60, G_parameter.mode3_up_counts);
        return;
    }
    if (cmd->part > 2)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }

    value = track_fun_atoi(cmd->pars[1]);
    if (value < 0 || value > 1000)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 [0-1000]");
        return;
    }

    if (cmd->part > 1)
    {
        value1 = track_fun_atoi(cmd->pars[2]);
        if (value1 < 1 || value1 > 10)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 [1-10]");
            return;
        }
    }
    LOGD(L_CMD, L_V5, "%d %d,%d", cmd->part, value, value1);
    G_parameter.mode3_work_interval = value * 60;
    G_parameter.mode3_up_counts = (value1 > 0 ? value1 : G_parameter.mode3_up_counts);

    track_cust_mode3_work_check((void*)0);

    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }


}

static void cmd_lbs(CMD_DATA_STRUCT *cmd)
{
#if defined(__BCA__)
    LBS_Multi_Cell_Data_Struct *lbs;
    if (cmd->part > 0)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    lbs = track_drv_get_lbs_data();
    track_cust_paket_18(lbs, 1);
    //track_soc_lbs_packets_need_send();
    sprintf(cmd->rsp_msg, "OK!");
    return;
#elif defined(__MT200__) || defined(__NT36__)|| defined(__NT31__) || defined(__ET310__)|| defined(__NT37__)||defined(__NT33__)
    LBS_Multi_Cell_Data_Struct *lbs;
    if (cmd->part > 0)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    lbs = track_drv_get_lbs_data();
    track_cust_paket_18(lbs, 1);
    sprintf(cmd->rsp_msg, "OK!");
    return;

#else

    lbs_cmd_send(cmd);

#endif
}

static void cmd_lbson(CMD_DATA_STRUCT *cmd)
{
    nvram_lbson_struct lbson = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d,%d", cmd->pars[0],
                ON_OFF(G_parameter.lbson.sw), G_parameter.lbson.t1, G_parameter.lbson.t2);
        return;
    }
    else if (cmd->part > 3)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    memcpy(&lbson, &G_parameter.lbson, sizeof(nvram_lbson_struct));
    if (!strcmp(cmd->pars[1], "ON"))
        lbson.sw = 1;
    else if (!strcmp(cmd->pars[1], "OFF"))
        lbson.sw = 0;
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    if (cmd->part >= 2)
    {
        lbson.t1 = track_fun_atoi(cmd->pars[2]);
        if (lbson.t1 < 10 || lbson.t1 > 3600)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (10 - 3600)");
            return;
        }
    }
    if (cmd->part >= 3)
    {
        lbson.t2 = track_fun_atoi(cmd->pars[3]);
        if (lbson.t2 < 10 || lbson.t2 > 3600)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 3 (10 - 3600)");
            return;
        }
    }
    memcpy(&G_parameter.lbson, &lbson, sizeof(nvram_lbson_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
        track_cust_LBS_upload();
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}
#endif
#if defined __USE_SPI__
static void cmd_spi(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "");
        track_spi_wr();
        return;
    }
    track_settimersec(atoi(cmd->pars[1]));
}
void spi_tc(CMD_DATA_STRUCT *cmd)
{
    int len, j;
    j = cmd->pars[1] - cmd->rcv_msg;
    //LOGD(L_APP, L_V2, "%p,%p,%p,{%s},{%s},{%s}",
    //    cmd->rcv_msg_source, cmd->pars[1], cmd->rcv_msg, cmd->pars[1], cmd->rcv_msg, cmd->rcv_msg_source);
    len = strlen(cmd->rcv_msg_source);
    LOGD(L_APP, L_V2, "%d,%d,%d", len, j, strlen(cmd->rcv_msg_source));
    LOGH(L_APP, L_V2, cmd->rcv_msg_source, strlen(cmd->rcv_msg_source));
    if (cmd->rcv_msg_source[len - 1] == 0x0D)
    {
        // cmd->rcv_msg_source[len] = 0x0A;
        // cmd->rcv_msg_source[len + 1] = 0x00;
        cmd->rcv_msg_source[len - 1] = 0;
        cmd->rcv_msg_source[len] = 0x00;
        len--;
    }
    if (len > 0 && len < 200 && j > 0)
    {
        track_spi_write(cmd->rcv_msg_source + j, len - j);
    }
}

static void cmd_spi_cmd(CMD_DATA_STRUCT *cmd)
{
    kal_uint8 i = 0, sendbuf[50] = 0;
    if (cmd->part == 0)
    {
        return;
    }
    i = atoi(cmd->pars[1]);
    switch (i)
    {
        case 1:
            sprintf(sendbuf, "AT^GT_CM=01#");
            track_spi_write(sendbuf, strlen(sendbuf));
            break;
        case 11:
            sprintf(sendbuf, "AT^GT_CM=01#\r\n");
            track_spi_write(sendbuf, strlen(sendbuf));
            break;
        case 2:
            sprintf(sendbuf, "AT^GT_CM=02,13,05#");
            track_spi_write(sendbuf, strlen(sendbuf));
            break;
        case 3:
            sprintf(sendbuf, "AT^GT_CM=03#");
            track_spi_write(sendbuf, strlen(sendbuf));
            break;
        case 4:
            sprintf(sendbuf, "AT^GT_CM=04,1,120#");
            track_spi_write(sendbuf, strlen(sendbuf));
            break;
        case 14:
            sprintf(sendbuf, "AT^GT_CM=04,2,5#");
            track_spi_write(sendbuf, strlen(sendbuf));
            break;
        case 24:
            sprintf(sendbuf, "AT^GT_CM=04,2,5#\r\n");
            track_spi_write(sendbuf, strlen(sendbuf));
            break;
        case 5:
            sprintf(sendbuf, "AT^GT_CM=05#");
            track_spi_write(sendbuf, strlen(sendbuf));
            break;
        case 6:
            sprintf(sendbuf, "AT^GT_CM=06#");
            track_spi_write(sendbuf, strlen(sendbuf));
            break;
        case 8:
            sprintf(sendbuf, "AT^GT_CM=08#");
            track_spi_write(sendbuf, strlen(sendbuf));
            break;
        default:
            break;

    }
}



static void cmd_spi_data(CMD_DATA_STRUCT *cmd)
{
    kal_uint8 i = 0, sendbuf[50] = 0;
    extern  void track_cust_spi_read_data(kal_uint8 * data, int len);

    if (cmd->part == 0)
    {
        return;
    }
    i = atoi(cmd->pars[1]);
    switch (i)
    {
        case 1:
            sprintf(sendbuf, "AT^GT_CM=01#");
            track_cust_spi_read_data(sendbuf, strlen(sendbuf));
            break;
        case 2:
            sprintf(sendbuf, "AT^GT_CM=02,13,05#");
            track_cust_spi_read_data(sendbuf, strlen(sendbuf));
            break;
        case 3:
            sprintf(sendbuf, "AT^GT_CM=03#");
            track_cust_spi_read_data(sendbuf, strlen(sendbuf));
            break;
        case 4:
            sprintf(sendbuf, "AT^GT_CM=04,2,1,120#");
            track_cust_spi_read_data(sendbuf, strlen(sendbuf));
            break;
        case 5:
            sprintf(sendbuf, "AT^GT_CM=05#");
            track_cust_spi_read_data(sendbuf, strlen(sendbuf));
            break;
        case 6:
            sprintf(sendbuf, "AT^GT_CM=06#");
            track_cust_spi_read_data(sendbuf, strlen(sendbuf));
            break;
        case 8:
            sprintf(sendbuf, "AT^GT_CM=08#");
            track_cust_spi_read_data(sendbuf, strlen(sendbuf));
            break;
        default:
            break;

    }
}
#endif

#if defined(__BCA__)
static void cmd_set_pw5v(CMD_DATA_STRUCT *cmd)
{
    U8 value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], G_parameter.PW5);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (!strcmp(cmd->pars[1], "0"))
    {
        value = 0;
        track_drv_gpio_pw5v(value);
    }
    else if (!strcmp(cmd->pars[1], "1"))
    {
        value = 1;
        track_drv_gpio_pw5v(value);
    }
    else if (!strcmp(cmd->pars[1], "2"))
    {
        value = 2;
        if (track_cust_oil_cut(99) == 1)
            track_drv_gpio_pw5v(1);
        else
            track_drv_gpio_pw5v(0);
    }
    else if (!strcmp(cmd->pars[1], "3"))
    {
        value = 3;
        if (track_cust_status_acc() == 1)
            track_drv_gpio_pw5v(1);
        else
            track_drv_gpio_pw5v(0);
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0/1/2/3)");
        return;
    }
    if (G_parameter.PW5 != value)
    {
        G_parameter.PW5 = value;
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    sprintf(cmd->rsp_msg, "OK!");
}
#endif

#if defined(__GT420D__)
kal_uint8 track_cust_check_gpsflag_status(kal_uint8 arg)
{
    if (arg != 99)
    {
        G_realtime_data.gps_420d_flag = arg;
        LOGD2(L_APP,L_V5,"GPS标志位:%d",G_realtime_data.gps_420d_flag);
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    }
    else
    {
        LOGD2(L_APP,L_V5,"GPS标志位,(%d)",G_realtime_data.gps_420d_flag);
        return G_realtime_data.gps_420d_flag;
    }
}
#endif

static void cmd_sys(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "");
        return;
    }
    if (!strcmp(cmd->pars[1], "A"))
    {
        track_test_view_backup(cmd->rsp_msg);
        track_test_view_queue(cmd->rsp_msg + strlen(cmd->rsp_msg));
        Ram_view(cmd->rsp_msg + strlen(cmd->rsp_msg));
    }
    else if (!strcmp(cmd->pars[1], "B"))
    {
        if (!strcmp(cmd->pars[2], "DEL"))
        {
            track_os_log_abnormal_data_reset();
        }
        track_os_log_abnormal_data_view(cmd->rsp_msg);
    }
    else if (!strcmp(cmd->pars[1], "SETTIME"))
    {
        applib_time_struct set_time;
        set_time.nYear = track_fun_atoi(cmd->pars[2]);
        set_time.nMonth= track_fun_atoi(cmd->pars[3]);
        set_time.nDay= track_fun_atoi(cmd->pars[4]);
        set_time.nHour= track_fun_atoi(cmd->pars[5]);
        set_time.nMin= track_fun_atoi(cmd->pars[6]);
        set_time.nSec= track_fun_atoi(cmd->pars[7]);

        track_drv_update_time_to_system(set_time);
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
        track_cust_mlsp_set_time(NULL);
#endif/*__MCU_LONG_STANDBY_PROTOCOL__*/
    }
    else if (!strcmp(cmd->pars[1], "C"))
    {
        kal_uint32 addr = NULL;
        int len;
        if (cmd->part != 3)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        addr = track_fun_parse4(cmd->pars[2]);
        len  = track_fun_atoi(cmd->pars[3]);
        if (len < 1 || len > 1000)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (1 to 1000)");
            return;
        }
        LOGD(L_APP, L_V3, "%p", (void*)addr);
        LOGH(L_APP, L_V4, (void*)addr, len);
        if (cmd->return_sms.cm_type == CM_SMS)
        {
            track_fun_mem_to_ascii(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, (void*)addr, len);
        }
    }
    else if (!strcmp(cmd->pars[1], "MIAN"))
    {
        track_drv_sys_l4cuem_set_audio_mode(atoi(cmd->pars[2]));
    }
    else if (!strcmp(cmd->pars[1], "GLBS"))
    {
    }
    else if (!strcmp(cmd->pars[1], "AGPS"))
    {
        sprintf(cmd->rsp_msg, "agps:%d",track_cust_apgs_sw_status());
    }
#if 0
    else if (!strcmp(cmd->pars[1], "DISOUT"))
    {
        track_nvram_alone_parameter_read()->ams_server.server_port=0xffff;
        track_nvram_alone_parameter_read()->server.soc_type=0xff;
        Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
    }
#endif
    else if (!strcmp(cmd->pars[1], "D") || !strcmp(cmd->pars[1], "START_TIME"))
    {
        kal_uint32 tick, ms, sec, min, hour, day;
        tick = kal_get_systicks();
        ms = kal_ticks_to_milli_secs(tick);
        if (ms > 86400000)
        {
            day = ms / 86400000;
            ms -= day * 86400000;
            hour = ms / 3600000;
            ms -= hour * 3600000;
            min = ms / 60000;
            ms -= min * 60000;
            sec = ms / 1000;
            ms -= sec * 1000;
            sprintf(cmd->rsp_msg, "Start system:%dday %dhour %dmin %dsec %dms", day, hour, min, sec, ms);
        }
        else if (ms > 3600000)
        {
            hour = ms / 3600000;
            ms -= hour * 3600000;
            min = ms / 60000;
            ms -= min * 60000;
            sec = ms / 1000;
            ms -= sec * 1000;
            sprintf(cmd->rsp_msg, "Start system:%dhour %dmin %dsec %dms", hour, min, sec, ms);
        }
        else if (ms > 60000)
        {
            min = ms / 60000;
            ms -= min * 60000;
            sec = ms / 1000;
            ms -= sec * 1000;
            sprintf(cmd->rsp_msg, "Start system:%dmin %dsec %dms", min, sec, ms);
        }
        else if (ms > 1000)
        {
            sec = ms / 1000;
            ms -= sec * 1000;
            sprintf(cmd->rsp_msg, "Start system:%dsec %dms", sec, ms);
        }
        else
        {
            sprintf(cmd->rsp_msg, "Start system:%dms, ", ms);
        }
    }
    else if (!strcmp(cmd->pars[1], "TIME"))
    {
        applib_time_struct currTime = {0};
        applib_dt_get_rtc_time(&currTime);
        sprintf(cmd->rsp_msg, "%0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
                currTime.nYear, currTime.nMonth, currTime.nDay,
                currTime.nHour, currTime.nMin, currTime.nSec);
    }
    else if (!strcmp(cmd->pars[1], "SERVER2OFF"))
    {
        nvram_server_addr_struct server2 ={0};
        memcpy(&server2,&track_nvram_alone_parameter_read()->ams_server,sizeof(nvram_server_addr_struct));
        server2.conecttype = 2;
        nvram_alone_parameter_cmd.server_ams_set(&server2);
        sprintf(cmd->rsp_msg, "OK!");
    }
    else if (!strcmp(cmd->pars[1], "SLEEPTEST"))
    {
        track_drv_sleep_enable(atoi(cmd->pars[2]), atoi(cmd->pars[3]));
        sprintf(cmd->rsp_msg, "SLEEP Test,%d,%d", atoi(cmd->pars[2]), atoi(cmd->pars[3]));
    }
    else if (!strcmp(cmd->pars[1], "E"))
    {
        track_cust_led_status_view(cmd->rsp_msg);
    }
    else if (!strcmp(cmd->pars[1], "F") && cmd->part == 3)
    {
        track_test_ram(cmd->rsp_msg, track_fun_atoi(cmd->pars[2]), track_fun_atoi(cmd->pars[3]));
    }
    else if (!strcmp(cmd->pars[1], "G") || !strcmp(cmd->pars[1], "GPS"))
    {
        if(cmd->part == 1)
        {
            if(track_cust_check_gpsflag_status(99) == 1)
                {
                    sprintf(cmd->rsp_msg, "GPS SW ON");
                    }
            else if(track_cust_check_gpsflag_status(99) == 2)
            {
                sprintf(cmd->rsp_msg, "GPS SW OFF");
                }
            }
        else
        {
        if (!strcmp(cmd->pars[2], "ON"))
        {
            track_cust_check_gpsflag_status(1);
            track_cust_gps_work_req((void *)13);
            sprintf(cmd->rsp_msg, "SUCCESS");
        }
        else if (!strcmp(cmd->pars[2], "OFF"))
        {
            track_cust_check_gpsflag_status(2);
            track_cust_gps_off_req((void *)3);
            sprintf(cmd->rsp_msg, "SUCCESS");
        }
        else
        {
            sprintf(cmd->rsp_msg, "FAIL");
            }
        #if !defined(__GT420D__)
        else if (!strcmp(cmd->pars[2], "RESET_STATUS"))
        {
            //    track_drv_gps_reset_status();
            track_drv_gps_epo_start();
        }
        #endif
        }
    }
    else if (!strcmp(cmd->pars[1], "BMT"))
    {
        uem_bmt_struct bmt_status = {0};
        uem_get_bat_status(&bmt_status);
        sprintf(cmd->rsp_msg, "OK! vbat_percentage:%d, status:%d, volt:%d, adc:%f",
                uem_get_vbat_percentage(),
                (kal_uint8)bmt_status.status,
                (kal_uint32)bmt_status.volt,
                (double)bmt_status.adc);
    }
    else if (!strcmp(cmd->pars[1], "TIMER"))
    {
        track_timer_Remaining(cmd->rsp_msg, atoi(cmd->pars[2]));
    }
    else if (!strcmp(cmd->pars[1], "RESET1"))
    {
        DRV_RESET();
        LOGD(L_APP, L_V5, "DRV_RESET");
    }
    else if (!strcmp(cmd->pars[1], "POWER_OFF"))
    {
        uem_drv_power_off();
        LOGD(L_APP, L_V5, "uem_drv_power_off");
    }
    else if (!strcmp(cmd->pars[1], "L4_POWER_OFF"))
    {
        l4cuem_power_off_req();
        LOGD(L_APP, L_V5, "l4cuem_power_off_req");
    }
    else if (!strcmp(cmd->pars[1], "ASSERT"))
    {
        ASSERT(0);
        sprintf(cmd->rsp_msg, "OK!");
    }
    else if (!strcmp(cmd->pars[1], "ASSERT2"))
    {
        *OTA_INT_MemoryDumpFlag = 0x26409001;
        ASSERT(0);
        //sprintf(cmd->rsp_msg, "OK!");
    }
    else if (!strcmp(cmd->pars[1], "ASSERT_NOT_R"))
    {
        *OTA_INT_MemoryDumpFlag = 0x26409001;
        sprintf(cmd->rsp_msg, "OK! ASSERT can not be restart.");
    }
    else if (!strcmp(cmd->pars[1], "WATCH_DOG_S"))
    {
        track_drv_watch_dog_close(0);
        sprintf(cmd->rsp_msg, "Watch dog OFF");
    }
    else if (!strcmp(cmd->pars[1], "DELFILE"))
    {
        int result;
        WCHAR FilePath[60] = {0};
        kal_wsprintf(FilePath, cmd->pars[2]);
        result = FS_Delete(FilePath);
        sprintf(cmd->rsp_msg, "Del %s = %d", cmd->pars[2], result);
        return;
    }
    else if (!strcmp(cmd->pars[1], "RECONNSERVER2"))
    {
        extern kal_uint8  is_rev_data;
        is_rev_data = 0;
        track_cust_server2_file_retry();
        track_os_intoTaskMsgQueueExt(track_cust_server2_conn, (void*)2);
        sprintf(cmd->rsp_msg, "OK");
    }
    else if (!strcmp(cmd->pars[1], "VER"))
    {
        char ver_string[300] = {0};
        int ver_str_len = 0;
        kal_uint8 *result;
        result = track_drv_get_chip_id();
        ver_str_len += snprintf(ver_string + ver_str_len, 299, "Chip RID:%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X, ",
                                result[0], result[1], result[2], result[3], result[4], result[5], result[6], result[7],
                                result[8], result[9], result[10], result[11], result[12], result[13], result[14], result[15]);
        ver_str_len += snprintf(ver_string + ver_str_len, 299, "IMEI:%s, %s", (char*)track_drv_get_imei(0), track_cust_get_version_for_upload_server());
        sprintf(cmd->rsp_msg, "[%d]%s", ver_str_len, ver_string);
    }
    else if (!strcmp(cmd->pars[1], "VIEWIP"))
    {
        track_soc_view_IP(cmd->rsp_msg);
    }
    else if (!strcmp(cmd->pars[1], "TWO"))
    {
        if (!strcmp(cmd->pars[2], "R"))
        {
            int a = atoi(cmd->pars[3]);
            int B = atoi(cmd->pars[4]);
            track_drv_led_switch(LED_GREEN, KAL_FALSE);
            LED_Set_Flash(LED_RED, a, B);
        }
        else if (!strcmp(cmd->pars[2], "G"))
        {
            int a = atoi(cmd->pars[3]);
            int B = atoi(cmd->pars[4]);

            track_drv_led_switch(LED_RED, KAL_FALSE);
            LED_Set_Flash(LED_GREEN, a, B);

        }
        else
        {
            int a = atoi(cmd->pars[3]);
            int B = atoi(cmd->pars[4]);
            LED_Set_Flash(LED_GREEN, a, B);
            LED_Set_Flash(LED_RED, a, B);
        }

    }
    else if (!strcmp(cmd->pars[1], "RESTARTSYSTEM"))
    {
        int len;
        nvram_check_ota_struct g_otacheck = {0};
        Track_nvram_read(NVRAM_EF_CHECK_OTA_LID, 1, (void *)&g_otacheck, NVRAM_EF_CHECK_OTA_SIZE);
        len = sprintf(cmd->rsp_msg, "[%02d-%02d-%02d] Count:%d/%d, Day:%d/%d",
                      g_otacheck.dateCount[0] + 2000, g_otacheck.dateCount[1], g_otacheck.dateCount[2],
                      g_otacheck.resetCount, g_otacheck.resetCountMax,
                      g_otacheck.resetDay, g_otacheck.resetDayMax);
        if (cmd->part > 2)
        {
            g_otacheck.resetCount    = track_fun_atoi(cmd->pars[2]);
            g_otacheck.resetCountMax = track_fun_atoi(cmd->pars[3]);
            g_otacheck.resetDay      = track_fun_atoi(cmd->pars[4]);
            g_otacheck.resetDayMax   = track_fun_atoi(cmd->pars[5]);
            sprintf(cmd->rsp_msg + len, " change (Count:%d/%d, Day:%d/%d)",
                    g_otacheck.resetCount, g_otacheck.resetCountMax,
                    g_otacheck.resetDay, g_otacheck.resetDayMax);
            Track_nvram_write(NVRAM_EF_CHECK_OTA_LID, 1, (void *)&g_otacheck, NVRAM_EF_CHECK_OTA_SIZE);
        }
    }
    else
    {
        int value = track_fun_atoi(cmd->pars[1]);
        switch (value)
        {
            case 11:
            {
                track_cust_fetion_auto_authorize_valid((void*)1);
                sprintf(cmd->rsp_msg, "OK!");
                break;
            }
            case 111:
            {
                extern kal_bool test_test_test;
                if (!strcmp(cmd->pars[2], "0"))
                {
                    test_test_test = 0;
                }
                else
                {
                    test_test_test = 1;
                }
                break;
            }
            case 112:
            {
                sprintf(cmd->rsp_msg, "SUM:%d, G_importance_parameter_data:%d, G_parameter:%d, G_realtime_data:%d",
                        sizeof(G_importance_parameter_data) + sizeof(G_parameter) + sizeof(G_realtime_data),
                        sizeof(G_importance_parameter_data), sizeof(G_parameter), sizeof(G_realtime_data));
                break;
            }
            case 113:
            {
                sprintf(cmd->rsp_msg, "FS_GetDiskInfo:%d", track_fun_FS_GetDiskInfo());
                break;
            }
            case 114:
            {
                //track_read_imsi_data();
                track_socket_apn_auto_set();
                break;
            }
            case 115:
            {
#if defined(__JSON__)
                main44();
#endif /* __JSON__ */
                sprintf(cmd->rsp_msg, "main44!");
                break;
            }
            case 12:
            {
                // ASCII
                if (track_nvram_alone_parameter_read()->server.conecttype == 0)
                {
                    sprintf(cmd->rsp_msg, "IMEI:%s, SERVER,0,%d.%d.%d.%d,%d,%d, [VERSION]%s\r\n[BUILD]%s\r\n", (char *)track_drv_get_imei(0),
                            track_nvram_alone_parameter_read()->server.server_ip[0], track_nvram_alone_parameter_read()->server.server_ip[1],
                            track_nvram_alone_parameter_read()->server.server_ip[2], track_nvram_alone_parameter_read()->server.server_ip[3],
                            track_nvram_alone_parameter_read()->server.server_port, track_nvram_alone_parameter_read()->server.soc_type,
                            release_verno(), build_date_time());
                }
                else
                {
                    sprintf(cmd->rsp_msg, "IMEI:%s, SERVER,%d,%s,%d,%d, [VERSION]%s\r\n[BUILD]%s\r\n", (char *)track_drv_get_imei(0),
                            track_nvram_alone_parameter_read()->server.conecttype,
                            track_domain_decode(track_nvram_alone_parameter_read()->server.url),
                            track_nvram_alone_parameter_read()->server.server_port,
                            track_nvram_alone_parameter_read()->server.soc_type,
                            release_verno(), build_date_time());
                }
                if (strlen(cmd->pars[2]) > 0)
                {
                    track_cust_sms_send(cmd->return_sms.num_type, cmd->pars[2], KAL_TRUE, cmd->rsp_msg, strlen(cmd->rsp_msg));
                    if (cmd->return_sms.cm_type == CM_SMS) cmd->rsp_msg[0] = 0;
                }
                break;
            }
            case 121:
            {
                track_drv_sys_cfun(0);
                sprintf(cmd->rsp_msg, "turn off radio and SIM power!");
                break;
            }
            case 122:
            {
                track_drv_sys_cfun(1);
                sprintf(cmd->rsp_msg, "fly mode!");
                break;
            }
            case 123:
            {
                track_drv_sys_cfun(4);
                sprintf(cmd->rsp_msg, "exit fly mode!");
                break;
            }
            case 131:
            {
                track_epo_file_size();
                sprintf(cmd->rsp_msg, "OK!");
                break;
            }
            case 132:
            {
                /* 1422748800 = 2015-02-01 00:00:00*/
                U32 t1, t2, t3;
                applib_time_struct *time = (applib_time_struct*)Ram_Alloc(1, sizeof(applib_time_struct));
                time->DayIndex = 0;
                time->nHour = 0;
                time->nMin = 0;
                time->nSec = 0;
                time->nDay = 1;
                time->nMonth = 2;
                time->nYear = 2017;
                t1 = OTA_applib_dt_mytime_2_utc_sec(time, 0);
                time->nMonth = 1;
                t2 = OTA_applib_dt_mytime_2_utc_sec(time, 0);
                time->nYear = 2015;
                time->nMonth = 2;
                t3 = OTA_applib_dt_mytime_2_utc_sec(time, 0);
                sprintf(cmd->rsp_msg, "%d,%d, 2015 %d", t1, t2, t3);
                Ram_Free(time);
                break;
            }
            case 133:
            {
                G_parameter.gps_work.static_filter_sw = 2;
                Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
                sprintf(cmd->rsp_msg, "Two static inhibition open!");
                break;
            }
            case 134:
            {
                nvram_check_ota_struct g_otacheck = {0};
                Track_nvram_read(NVRAM_EF_CHECK_OTA_LID, 1, (void *)&g_otacheck, NVRAM_EF_CHECK_OTA_SIZE);
                g_otacheck.unconditional = track_fun_atoi(cmd->pars[2]);
                Track_nvram_write(NVRAM_EF_CHECK_OTA_LID, 1, (void *)&g_otacheck, NVRAM_EF_CHECK_OTA_SIZE);
                sprintf(cmd->rsp_msg, "g_otacheck.unconditional=%d", g_otacheck.unconditional);
                break;
            }
            case 135:
            {
                U8 md5_for_file[16] = {0};
                track_cust_get_data_from_file(FOTA_MD5_FILE_PATH, md5_for_file, 16);
                track_fun_mem_to_ascii(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, (void*)md5_for_file, 16);
            }
            case 14:
            {
                G_parameter.gps_work.Fd.sw = 0;
                G_parameter.gps_work.Fti.sw = 1;
                G_parameter.gps_work.Fti.interval_acc_off = track_fun_atoi(cmd->pars[2]);
                G_parameter.gps_work.Fti.interval_acc_on = track_fun_atoi(cmd->pars[2]);
                Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
                sprintf(cmd->rsp_msg, "OK!");
                break;
            }
            case 146:
            {
                nvram_gps_last_data_struct backup = {0};
                track_cust_nvram_gps_last_rmc(1, (kal_uint8*)&backup);
                LOGD(L_APP, L_V5, "status=%d, nYear=%d,month=%d,nDay=%d, Lat:%0.6f, Lon:%0.6f",
                     backup.rmc.status, backup.rmc.Date_Time.nYear, backup.rmc.Date_Time.nMonth, backup.rmc.Date_Time.nDay, backup.rmc.Latitude, backup.rmc.Longitude);
                break;
            }
            case 15:
            {
                extern void track_cust_sim_restart(void * arg);
                track_cust_sim_restart((void*)1);
                sprintf(cmd->rsp_msg, "OK! Restarting SIM...");
                break;
            }

            case 161:
            {
                sprintf(cmd->rsp_msg, "%d (MOD_ATCI:%d, MOD_MMI:%d, MOD_L4C:%d, MOD_DRV_HISR:%d, MOD_EINT_HISR:%d)",
                        kal_get_active_module_id(),
                        MOD_ATCI,
                        MOD_MMI,
                        MOD_L4C,
                        MOD_DRV_HISR,
                        MOD_EINT_HISR);
                break;
            }
            case 163:
            {
                track_at_to_atci_send("at+csq\r\n", 8);
                sprintf(cmd->rsp_msg, "OK!");
                break;
            }
            case 164:
            {
                track_cust_sim_backup_for_parameter();
                sprintf(cmd->rsp_msg, "OK!");
                break;
            }
            case 3:
            {
                extern void track_soc_queue_all_to_nvram(void);
                track_os_intoTaskMsgQueue(track_soc_queue_all_to_nvram);//
                sprintf(cmd->rsp_msg, "OK!");
            }
            break;

            case 32:
            {
                //track_cust_ram_alarm(1034004, 16);
                //sprintf(cmd->rsp_msg, "tick=%d", kal_secs_to_ticks(30*60));
                track_check_tick(2, cmd->rsp_msg);
            }
            break;

            case 4:
                g_test_flag = track_fun_atoi(cmd->pars[2]);
                sprintf(cmd->rsp_msg, "OK!");
                break;

            case 41:
                track_cust_write_sim_backup_for_parameter();
                sprintf(cmd->rsp_msg, "OK!");
                break;

            case 42:
                break;

            case 423:/*时间测试*/
            {
                U32 timep;
                static applib_time_struct tmp = {0};
                applib_dt_get_rtc_time(&tmp);
                LOGS("time: %0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", tmp.nYear, tmp.nMonth, tmp.nDay, tmp.nHour, tmp.nMin, tmp.nSec);

                /*获得当前时间的秒数*/
                /*timep = OTA_app_getcurrtime();
                applib_dt_utc_sec_2_mytime(timep, &tmp, 0);
                LOGS("time: %0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", tmp.nYear, tmp.nMonth, tmp.nDay, tmp.nHour, tmp.nMin, tmp.nSec);

                tmp.nYear = 2015;
                tmp.nMonth = 2;
                tmp.nDay = 1;
                tmp.nHour = 0;
                tmp.nMin = 0;
                tmp.nSec = 0;
                timep = applib_dt_mytime_2_utc_sec(&tmp, 0);
                LOGS("距离2014年秒数:%d, %d", timep, OTA_app_getcurrtime() - timep);*/

                /* 1420070400 = 2015-01-01 00:00:00*/

                tmp.nYear = 2015;
                tmp.nMonth = 1;
                tmp.nDay = 1;
                tmp.nHour = 0;
                tmp.nMin = 0;
                tmp.nSec = 0;
                timep = applib_dt_mytime_2_utc_sec(&tmp, 0);
                LOGS("2015年秒数:%d", timep);

                tmp.nYear = 2015;
                tmp.nMonth = 2;
                tmp.nDay = 1;
                tmp.nHour = 0;
                tmp.nMin = 0;
                tmp.nSec = 0;
                timep = applib_dt_mytime_2_utc_sec(&tmp, 0);
                LOGS("2015年2月秒数:%d", timep);
            }
            break;

            case 43:
                G_realtime_data.in_or_out[0] = track_fun_atoi(cmd->pars[2]);
                sprintf(cmd->rsp_msg, "OK!");
                break;

#if defined(__SIM_CU_CALL__)
            case 431:
                track_cust_SIM_findBalance();
                sprintf(cmd->rsp_msg, "话费查询 OK!");
                break;

            case 432:
                track_cust_SIM_call("10086");
                sprintf(cmd->rsp_msg, "OK!");
                break;
#endif /* __SIM_CU_CALL__ */
        }
    }
}

static void cmd_(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "");
        return;
    }
    else if (cmd->part < 3 || cmd->part > 4)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
}
static void cmd_gpson(CMD_DATA_STRUCT *cmd)
{
    nvram_gps_work_struct tmp = {0};
    int value = 0;
    kal_bool flag = 1;
    extern   void track_cust_module_init(void);

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "OK!");
        track_cust_gpson_timer(0);
        track_cust_gps_work_req((void *)14);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    value = track_fun_atoi(cmd->pars[1]);
    if (value < 5 || value > 300)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (5-300)");
        return;
    }
    track_cust_gpson_timer(value);
    track_cust_gps_work_req((void *)14);
#ifdef __GW100__
    track_cust_module_init();
#endif /* __GW100__ */
    sprintf(cmd->rsp_msg, "OK!");
}

/******************************************************************************
 *  Function    -  cmd_server
 *
 *  Purpose     -  设置定位服务器地址参数
 *
 *  Description -  SERVER,<使用密码>, <模式>,<地址>,<端口>,<传输协议模式>#
 *                 SERVER,0,202.173.231.112,8821,0#
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 08-01-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_dserver(CMD_DATA_STRUCT *cmd)
{
    int value, plen;
    nvram_server_addr_struct server = {0};
    nvram_server_addr_struct dserver = {0};
    memcpy(&server, &track_nvram_alone_parameter_read()->server, sizeof(nvram_server_addr_struct));
#ifdef __GT02__
    if (G_importance_parameter_data.app_version.version == 2)
    {
        return ;
    }
#endif /* __GT02__ */
    if (cmd->part == 0)
    {
        if (G_importance_parameter_data.dserver.conecttype == 0)
        {
            sprintf(cmd->rsp_msg, "DSERVER,0,%d.%d.%d.%d,%d,%d",
                    G_importance_parameter_data.dserver.server_ip[0], G_importance_parameter_data.dserver.server_ip[1],
                    G_importance_parameter_data.dserver.server_ip[2], G_importance_parameter_data.dserver.server_ip[3],
                    G_importance_parameter_data.dserver.server_port, G_importance_parameter_data.dserver.soc_type);
        }
        else
        {
            sprintf(cmd->rsp_msg, "SERVER,%d,%s,%d,%d",
                    G_importance_parameter_data.dserver.conecttype,
                    track_domain_decode(G_importance_parameter_data.dserver.url),
                    G_importance_parameter_data.dserver.server_port,
                    G_importance_parameter_data.dserver.soc_type);
        }
        return;
    }
    if (cmd->part != 3 && cmd->part != 4)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (track_nvram_alone_parameter_read()->server.lock != 0)
    {
        sprintf(cmd->rsp_msg, "Error: The platform addresses are Locked modify!");
        return;
    }
    memset(cmd->rsp_msg, 0, sizeof(cmd->rsp_msg));

    memcpy(&dserver, &G_importance_parameter_data.dserver, sizeof(nvram_server_addr_struct));

    value = track_fun_atoi(cmd->pars[CM_Par1]);
    if (value != 0 && value != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0/1)");
        return;
    }
    dserver.conecttype = value;

    plen = strlen(cmd->pars[CM_Par2]);
    if (plen <= 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 2 is empty!");
        return;
    }
    if (plen >= 99)
    {
        sprintf(cmd->rsp_msg, "Error: Parameters 2 characters too long!");
        return;
    }
    if (track_fun_check_ip(cmd->pars[CM_Par2]))
    {
        if (dserver.conecttype != 0)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 domain is not formatted correctly!");
            return;
        }
        memset(dserver.server_ip, 0, 4);
        if (!track_fun_asciiIP2Hex(cmd->pars[CM_Par2], (void*)&dserver.server_ip[0]))
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 is not a valid IP address format!");
            return;
        }
    }
    else
    {
        if (dserver.conecttype != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 is not a valid IP address format!");
            return;
        }
        if (strstr(cmd->pars[CM_Par2], "http://"))
        {
            sprintf(cmd->rsp_msg, "Error: The domain name does not include \"http://\"!");
            return;
        }
        {
            char domain[TRACK_MAX_URL_SIZE] = {0};
            memcpy(domain, cmd->rcv_msg_source + (cmd->pars[CM_Par2] - cmd->rcv_msg), plen);
            track_domain_encode(domain, dserver.url);
        }
    }

    if (!track_fun_check_data_is_port(cmd->pars[CM_Par3]))
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 3 (0~65535)");
        return;
    }
    dserver.server_port = track_fun_string2U16(cmd->pars[CM_Par3]);

    if (cmd->part == 4)
    {
        value = track_fun_atoi(cmd->pars[CM_Par4]);
        if (value != 0 && value != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 4 (0/1)");
            return;
        }
    }
    dserver.soc_type = 0; // 当前只支持TCP
    dserver.server_type = 1;
    dserver.ip_update = 0;
    memcpy(&G_importance_parameter_data.dserver, &dserver, sizeof(nvram_server_addr_struct));

    server.ip_update = 0;
    nvram_alone_parameter_cmd.server_set(&server);
    Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
    sprintf(cmd->rsp_msg, "OK");
    track_set_server_ip_status(0);

    track_os_intoTaskMsgQueueExt(track_soc_gprs_reconnect, (void*)22);//修改域名后，重新建立连接
    //track_os_intoTaskMsgQueue(track_cust_write_sim_backup_for_parameter);
}

/******************************************************************************
 *  Function    -  sms_cmd_see_parameters
 *
 *  Purpose     -  查看SOS,IMEI,及震动,油控状态
 *
 *  Description - PARAM#
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 17-05-2010,  jwj  written
 * ----------------------------------------
 ******************************************************************************/
static void gt02_param(CMD_DATA_STRUCT *cmd)
{
    S8 temp[300], data[300];
    int i = 0;
    U32 time = 0;
    if (cmd->part != 0)
    {
        sprintf((char *)cmd->rsp_msg, "ERROR:100");
        return;
    }
    memset(data, 0, sizeof(data));
    memset(temp, 0, sizeof(temp));

    sprintf(temp, "IMEI:%s;", track_drv_get_imei(0));
    strcpy(data, temp);
    memset(temp, 0, sizeof(temp));

    sprintf(temp, "TimeZone:%c,%d,%d;", G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min);
    strcat(data, temp);
    memset(temp, 0, sizeof(temp));
#ifdef __GT02__
    if (G_importance_parameter_data.app_version.support_sensor)
#endif /* __GT02__ */
    {
        strcat(data, "Sends:");
        sprintf(temp, "%d;", G_parameter.sensor_gps_work) ;
        strcat(data, temp);

        strcat(data, "Level:");
        sprintf(temp, "%d;", G_parameter.vibrates_alarm.sensitivity_level);
        strcat(data, temp);
        memset(temp, 0, 300);
    }


    //默认语言
    sprintf(temp, "Language:%d;", G_parameter.lang);
    strcat(data, temp);
    memset(temp, 0, 300);

    sprintf(temp, "HBT:%d;", G_parameter.hbt_acc_off);
    strcat(data, temp);
    memset(temp, 0, 300);
    if (G_parameter.gps_work.anglerep.sw)
    {
        sprintf(temp, "Angler:ON,%d;", G_parameter.gps_work.anglerep.angleOfDeflection, G_parameter.gps_work.anglerep.detection_time);
        strcat(data, temp);
        memset(temp, 0, 300);
    }
    else
    {
        sprintf(temp, "Angler:OFF");
        strcat(data, temp);
        memset(temp, 0, 300);
    }

    if (G_parameter.gps_work.Fti.sw == 1)
    {
        sprintf(temp, "Timer:%d", G_parameter.gps_work.Fti.interval_acc_off);
        strcat(data, temp);
        memset(temp, 0, 300);
    }
    else
    {
        sprintf(temp, "Distance:%d,", G_parameter.gps_work.Fd.distance);
        strcat(data, temp);
        memset(temp, 0, 300);
    }
    if (G_parameter.group_package.sw)
    {
        sprintf(temp, ";Batch:ON,%d", G_parameter.group_package.at_least_pack);
        strcat(data, temp);
        memset(temp, 0, 300);
    }
    else
    {
        sprintf(temp, ";batch:OFF");
        strcat(data, temp);
        memset(temp, 0, 300);
    }
    strcpy((char *)cmd->rsp_msg, (char *)data);
}
/******************************************************************************
 *  Function    -  sms_cmd_see_set
 *
 *  Purpose     -  查看终端设置参数
 *
 *  Description -SCXSZ[,使用密码]
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 11-05-2010,  jwj  written
 * ----------------------------------------
 ******************************************************************************/
static void gt02_scxsz(CMD_DATA_STRUCT *cmd)
{
    char temp[300], temp1[300];
    char i;
    U16 RingId = 0;
    nvram_GPRS_APN_struct *use_apn = track_socket_get_use_apn();
    char GPRS[5] = {0};
    char server[99] = {0};
    if (cmd->part != 0)
    {
        sprintf(cmd->rsp_msg, "ERROR:100");
        return;
    }
    memset(cmd->rsp_msg, 0, sizeof(cmd->rsp_msg));
    memset(temp, 0, 300);
    memset(temp1, 0, 300);

    if (track_nvram_alone_parameter_read()->gprson)
        snprintf(GPRS, 20, "ON");
    else
        snprintf(GPRS, 20, "OFF");
    if (G_parameter.gps_work.Fti.sw == 1)
    {
        sprintf(temp1, "Timer:%d,%d,%d;", G_parameter.gps_work.Fti.lbs_interval,
                G_parameter.gps_work.Fti.interval_acc_off, G_parameter.gps_work.ephemeris_interval);
    }
    if (G_parameter.gps_work.Fd.sw == 1)
    {
        snprintf(temp1, 50, "Distance:%d; ", G_parameter.gps_work.Fd.distance);
    }
    strcat(temp, temp1);
    memset(temp1, 0, 300);
    if (G_importance_parameter_data.dserver.server_type && track_get_server_ip_status() == 0)
    {
        if (G_importance_parameter_data.dserver.conecttype == 0)
        {
            //ip
            snprintf(server, 20, "%d.%d.%d.%d", G_importance_parameter_data.dserver.server_ip[0], G_importance_parameter_data.dserver.server_ip[1], G_importance_parameter_data.dserver.server_ip[2], G_importance_parameter_data.dserver.server_ip[3]);
        }
        else
        {
            //Domain name
            snprintf(server, 98, "%s", track_domain_decode(G_importance_parameter_data.dserver.url));
        }

        snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "GPRS:%s; Currently use APN:%s,%s,%s; DServer:%d,%s,%d,%d; URL:%s,%s",
                 GPRS,
                 use_apn->apn,
                 use_apn->username,
                 use_apn->password,
                 G_importance_parameter_data.dserver.conecttype,
                 server,
                 G_importance_parameter_data.dserver.server_port,
                 G_importance_parameter_data.dserver.soc_type,
                 G_parameter.url, temp);
    }
    else
    {
        if (track_nvram_alone_parameter_read()->server.conecttype == 0)
        {
            //ip
            snprintf(server, 20, "%d.%d.%d.%d", track_nvram_alone_parameter_read()->server.server_ip[0], track_nvram_alone_parameter_read()->server.server_ip[1], track_nvram_alone_parameter_read()->server.server_ip[2], track_nvram_alone_parameter_read()->server.server_ip[3]);
        }
        else
        {
            //Domain name
            snprintf(server, 98, "%s", track_domain_decode(track_nvram_alone_parameter_read()->server.url));
        }

        snprintf(cmd->rsp_msg, CM_PARAM_LONG_LEN_MAX, "GPRS:%s; Currently use APN:%s,%s,%s; Server:%d,%s,%d,%d; URL:%s,%s",
                 GPRS,
                 use_apn->apn,
                 use_apn->username,
                 use_apn->password,
                 track_nvram_alone_parameter_read()->server.conecttype,
                 server,
                 track_nvram_alone_parameter_read()->server.server_port,
                 track_nvram_alone_parameter_read()->server.soc_type,
                 G_parameter.url, temp);
    }

}

/******************************************************************************
 *  Function    -  cmd_cancel
 *
 *  Purpose     -  撤销拆卸报警指令
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 2015-12-08,  xzq  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_cancel(CMD_DATA_STRUCT *cmd)
{
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)||defined(__GT420D__)
    extern void track_mode2_to_mode1(void);
    extern kal_uint16 track_cust_check_teardown_flag(kal_uint16 arg);
    extern U8 track_cust_remove_alarm_flag(U8 flag);
    kal_uint16 i = 0;
#endif /* __GT300S__ */

    if (cmd->part > 0)
    {
        sprintf(cmd->rsp_msg, "ERROR: NO PARAMETERS!");
        return;
    }
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
    if (track_is_timer_run(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID) || (G_realtime_data.tracking_mode == 1))
    {
        track_stop_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID);
        tr_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, 20 * 1000, track_mode2_to_mode1);

    }
#elif defined(__GT740__)
    sprintf(cmd->rsp_msg, "OK!");
    if (G_realtime_data.tracking_mode)
    {
        G_realtime_data.tracking_count = G_parameter.teardown_str.alarm_upcount;
        track_cust_module_deal_end_trackmode();
    }
    return;
#elif defined(__GT420D__)
    i = track_cust_check_teardown_flag(99);
    LOGD(L_APP,L_V5,"tracking_status_mode:%d",i);
    if (track_cust_check_teardown_flag(99) == 1||track_cust_remove_alarm_flag(99)==1)
    {
        track_cust_remove_alarm_flag(0);
        track_cust_check_teardown_flag(0);
        track_stop_timer(TRACK_CUST_EXTCHIP_SEND_REMOVE_ALARM_TIMER_ID);
        LOGD(L_APP,L_V5,"20秒后更新模式状态");
        tr_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, 20 * 1000, track_mode2_to_mode1);
        tr_stop_timer(TRACK_CUST_EXTCHIP_TEARDOWN_DELAY_SEND_GPSORLBS_TIMER_ID);
    }
#endif /* __GT300S__ */
    sprintf(cmd->rsp_msg, "OK!");

}

static void cmd_track_status(CMD_DATA_STRUCT *cmd)
{
    U32 timep;
    static applib_time_struct tmp = {0};
    OTA_applib_dt_get_rtc_time(&tmp);
    timep = OTA_applib_dt_mytime_2_utc_sec(&tmp, 0);

    if (G_realtime_data.tracking_time >= 1422748800 && timep >= 1422748800 && G_realtime_data.tracking_mode)
    {
        if (G_realtime_data.tracking_time + 60 * 60 >= timep)
        {
            timep = G_realtime_data.tracking_time + 60 * 60 - timep;
            LOGD(L_OBD, L_V4, "模式一工作%d分钟%d秒之后，自动切换至模式二", timep / 60, timep % 60);
            sprintf(cmd->rsp_msg, "tracking_mode:%s,old mode:%d,time:%d", ON_OFF(G_realtime_data.tracking_mode), G_realtime_data.tracking_status_mode, timep / 60);
            return;
        }
    }
    sprintf(cmd->rsp_msg, "tracking_mode:%s,old mode:%d", ON_OFF(G_realtime_data.tracking_mode), G_realtime_data.tracking_status_mode);


}
/******************************************************************************
 *  Function    -  cmd_adt
 *
 *  Purpose     -  信息传输开关(几米国际上传外电压值)
 *
 *  Description -  ADC,A[,T]#
 *                          A=ON/OFF,  ON 开启AD数据上传，OFF为关闭AD数据上传，默认为ON状态;
 *                          T为上传间隔，5~3600秒；默认为60秒;
 *                          ADT#,查询当前设置信息
 *  modification history
 * ----------------------------------------
 * v1.0  , 07-23-2012,  Cjjl  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_adt(CMD_DATA_STRUCT *cmd)
{
    kal_uint16 time = 0;
    if (cmd->part > 2)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d", cmd->pars[0], ON_OFF(G_parameter.info_up.sw), G_parameter.info_up.time);
        return;
    }
    if (!strcmp(cmd->pars[1], "ON") || !strcmp(cmd->pars[1], "1"))
    {
        if (cmd->part == 1)
        {
            G_parameter.info_up.sw = 1;
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
            sprintf(cmd->rsp_msg, "%s,%d", ON_OFF(G_parameter.info_up.sw), G_parameter.info_up.time);
        }
        else if (cmd->part == 2)
        {
            G_parameter.info_up.sw = 1;
            time = track_fun_atoi(cmd->pars[2]);
            if (time < 5 || time > 3600)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (5~3600)");
                return;
            }
            else
            {
                G_parameter.info_up.time = time;
                Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
                sprintf(cmd->rsp_msg, "%s,%d", ON_OFF(G_parameter.info_up.sw), G_parameter.info_up.time);
            }
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF") || !strcmp(cmd->pars[1], "0"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        G_parameter.info_up.sw = 0;
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        sprintf(cmd->rsp_msg, "%s,%d", ON_OFF(G_parameter.info_up.sw), G_parameter.info_up.time);
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
    information_packets();
}


#if defined(__GT740__)||defined(__GT300S__) || defined (__GT370__)|| defined (__GT380__)||defined(__GT420D__)
#if defined(__GT300S__) || defined (__GT370__)|| defined (__GT380__)||defined(__GT420D__)
static void cmd_remset(CMD_DATA_STRUCT * cmd)
{
    extern void track_mode2_to_mode1(void);
    int value;
    nvram_work_mode_struct chip = {0};

    if (cmd->part == 0)
    {
#if defined(__GT420D__)
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], G_parameter.work_mode.teardown_time);
#else
        sprintf(cmd->rsp_msg, "%s:%d,%d", cmd->pars[0],  G_parameter.work_mode.teardown_mode, G_parameter.work_mode.teardown_time);
#endif
        return;
    }
#if defined(__GT420D__)
    if (cmd->part > 1)
#else
    if (cmd->part > 2)
#endif
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }

    memcpy(&chip, &G_parameter.work_mode, sizeof(nvram_work_mode_struct));
#if !defined(__GT420D__)
    value = track_fun_atoi(cmd->pars[1]);
    if (value < 1 || value > 2)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter (1-2)");
        return;
    }
    chip.teardown_mode = value;

    value = track_fun_atoi(cmd->pars[2]);
    if (value < 3 || value > 120)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter (3-120)");
        return;
    }

    chip.teardown_time = value;
#else
    chip.teardown_mode = 2;
    value = track_fun_atoi(cmd->pars[1]);
    if (value < 3 || value > 120)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter1 (3-120)");
        return;
    }
    chip.teardown_time = value;
#endif
    memcpy(&G_parameter.work_mode, &chip, sizeof(nvram_work_mode_struct));
#if defined(__GT420D__)
    LOGD(L_APP,L_V5,"重置REMSET定时器");
    if (G_realtime_data.tracking_mode == 1)
    {
        tr_start_timer(TRACK_CUST_EXTCHIP_MODE_AUTO_SW_TIMER_ID, \
                       G_parameter.work_mode.teardown_time * 60 * 1000 + 5000, track_mode2_to_mode1);
    }
#endif
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }

}
static void cmd_tracktime(CMD_DATA_STRUCT *cmd)
{
    U8 value = 0;
    kal_int32 min = 5;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s,%d", cmd->pars[0], G_parameter.work_mode.teardown_time);
        return;
    }
    if (cmd->part == 1)
    {
        if (strlen(cmd->pars[1]) > 0)
        {
            value = track_fun_atoi(cmd->pars[1]);
            if ((value < min || value > 60))
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 1 (%d-60)", min);
                return;
            }
            G_parameter.work_mode.teardown_time = value;
            if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
            {
                sprintf(cmd->rsp_msg, "OK!");
            }
            else
            {
                sprintf(cmd->rsp_msg, "Write Nvram Error!");
                return;
            }

        }
        else
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (%d-60)", min);
            return;
        }

    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }


}

#endif
static void cmd_mode_view(CMD_DATA_STRUCT * cmd)
{
    if (cmd->part == 1 && !strcmp(cmd->pars[1], "MODE"))
    {
        if (G_parameter.extchip.mode == 1)
        {
            sprintf(cmd->rsp_msg, "MODE:%d,%d,%d", G_parameter.extchip.mode,
                    G_parameter.gps_work.Fti.interval_acc_off, G_parameter.extchip.mode1_par2);
        }
        else if (G_parameter.extchip.mode == 2)
        {
            if (G_parameter.extchip.mode2_par2 % 60 == 0)
            {
                sprintf(cmd->rsp_msg, "MODE:%d,%0.2d:%0.2d,%d", G_parameter.extchip.mode,
                        G_parameter.extchip.mode2_par1 / 60, G_parameter.extchip.mode2_par1 % 60, G_parameter.extchip.mode2_par2 / 60);
            }
            else
            {
                sprintf(cmd->rsp_msg, "MODE:%d,%0.2d:%0.2d,%d(MIN)", G_parameter.extchip.mode,
                        G_parameter.extchip.mode2_par1 / 60, G_parameter.extchip.mode2_par1 % 60, G_parameter.extchip.mode2_par2);
            }
        }
        else
        {
            sprintf(cmd->rsp_msg, "Error: Parameter");
        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter");
    }
}

static void cmd_teardown(CMD_DATA_STRUCT * cmd)
{
#if defined(__GT420D__)
    extern void track_mode2_to_mode1(void);//
#endif
    nvram_teardown_struct alarm = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s,%s,%d", cmd->pars[0],
                ON_OFF(G_parameter.teardown_str.sw), G_parameter.teardown_str.alarm_type);
        return;
    }
    memcpy(&alarm, &G_parameter.teardown_str, sizeof(nvram_teardown_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part != 1 && cmd->part != 2)
        {
            sprintf(cmd->rsp_msg, "FAIL");
            return;
        }
        alarm.sw = 1;
        if (cmd->part == 2)
        {
            if (!strcmp(cmd->pars[2], "0"))
            {
                alarm.alarm_type = 0;
            }
#if defined(__GT740__)
            else
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1)");
                return;
            }
#else
            else
            {
                sprintf(cmd->rsp_msg, "FAIL");
                return;
            }
#endif

        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
#if !defined(__GT420D__)
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
#endif
#if defined(__GT420D__)
        if (cmd->part == 2)
        {
            alarm.alarm_type = track_fun_atoi(cmd->pars[2]);
            if (alarm.alarm_type != 0)
            {
                sprintf(cmd->rsp_msg, "FAIL");
                return;
            }
        }
#endif
        alarm.sw = 0;
        tr_stop_timer(TRACK_CUST_EXTCHIP_TEARDOWN_DELAY_SEND_GPSORLBS_TIMER_ID);
        tr_stop_timer(TRACK_CUST_EXTCHIP_DARKALM_DELAY_SEND_GPSORLBS_TIMER_ID);
    }
    else
    {
        sprintf(cmd->rsp_msg, "FAIL");
        return;
    }
#if defined(__GT740__)||defined(__GT420D__)
    track_cust_notify_st_remove_alarm_modify(alarm.sw);
#endif
        memcpy(&G_parameter.teardown_str, &alarm, sizeof(nvram_teardown_struct));
        if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
        {
            sprintf(cmd->rsp_msg, "SUCCESS");
#if defined (__GT370__)||defined (__GT380__)
            track_cust_paket_9404_handle();
#endif
#if defined(__GT420D__)
        track_cust_remove_alarm_flag(0);
        track_cust_check_teardown_flag(0);
        track_cust_check_downalm_flag(0);
        if (track_is_timer_run( TRACK_CUST_CHECK_DOWNALM_FLAG))
            {
                tr_stop_timer( TRACK_CUST_CHECK_DOWNALM_FLAG);
            }
            if (track_is_timer_run( TRACK_CUST_CHECK_TEARDOWN_FLAG))
            {
                tr_stop_timer( TRACK_CUST_CHECK_TEARDOWN_FLAG);
            }
#endif
        }
        else
        {
            sprintf(cmd->rsp_msg, "FAIL");
        }
}

#if defined (__GT370__)|| defined (__GT380__)
static void cmd_remalmf(CMD_DATA_STRUCT * cmd)
{
    nvram_open_shell_alarm_struct alarm = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d", cmd->pars[0],
                ON_OFF(G_parameter.open_shell_alarm.sw), G_parameter.open_shell_alarm.alarm_type);
        return;
    }
    memcpy(&alarm, &G_parameter.open_shell_alarm, sizeof(nvram_open_shell_alarm_struct));

    if (strcmp(cmd->pars[1], "ON") == 0)
    {
        if (cmd->part != 1 && cmd->part != 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 1;
        if (cmd->part == 2)
        {
            if (!strcmp(cmd->pars[2], "0"))
            {
                alarm.alarm_type = 0;
            }
            else if (!strcmp(cmd->pars[2], "1"))
            {
                alarm.alarm_type = 1;
            }
            else
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1)");
                return;
            }
        }
    }
    else if (strcmp(cmd->pars[1], "OFF") == 0)
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }

    memcpy(&G_parameter.open_shell_alarm, &alarm, sizeof(nvram_open_shell_alarm_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
        track_cust_paket_9404_handle();
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}
#endif

#if defined(__GT740__) || defined (__GT370__)|| defined (__GT380__)||defined(__GT420D__)
static void cmd_extchip(CMD_DATA_STRUCT * cmd)
{
    int len, j;
    U8 buf[100] = {"AT^GT_CM="};
    extern void track_cust_module_power_cmd(cmd_return_struct * return_sms, kal_uint8 * data, int len);

    j = cmd->pars[1] - cmd->rcv_msg;
    len = strlen(cmd->rcv_msg_source);
    LOGD(L_APP, L_V2, "%d,%d,%d", len, j, strlen(cmd->rcv_msg_source));
    LOGH(L_APP, L_V2, cmd->rcv_msg_source, strlen(cmd->rcv_msg_source));

#if defined (__GT370__)|| defined (__GT380__)||defined(__GT420D__)
    /*必须保证最后有#结束符       --    chengjun  2017-04-25*/
    if (strchr(cmd->rcv_msg_source, '#') != NULL)
    {
        if (cmd->rcv_msg_source[len - 1] == 0x0D)
        {
            cmd->rcv_msg_source[len] = 0x0A;
            cmd->rcv_msg_source[len + 1] = 0x00;
            len++;
        }
        else if (strstr(cmd->rcv_msg_source, "\n") == NULL)
        {
            cmd->rcv_msg_source[len] = 0x0D;
            cmd->rcv_msg_source[len + 1] = 0x0A;
            cmd->rcv_msg_source[len + 2] = 0x00;
            len += 2;
        }
    }
    else
    {
        if (cmd->rcv_msg_source[len - 1] == 0x0D)
        {
            cmd->rcv_msg_source[len - 1] = '#';
            cmd->rcv_msg_source[len] = 0x0D;
            cmd->rcv_msg_source[len+1] = 0x0A;
            cmd->rcv_msg_source[len + 1] = 0x00;
            len += 2;
        }
        else if (strchr(cmd->rcv_msg_source, '\n') == NULL)
        {
            cmd->rcv_msg_source[len] = '#';
            cmd->rcv_msg_source[len+1] = 0x0D;
            cmd->rcv_msg_source[len+2] = 0x0A;
            cmd->rcv_msg_source[len + 3] = 0x00;
            len += 3;
        }
    }
#else
    if (cmd->rcv_msg_source[len - 1] == 0x0D)
    {
        cmd->rcv_msg_source[len] = 0x0A;
        cmd->rcv_msg_source[len + 1] = 0x00;
        len++;
    }
    else if (strstr(cmd->rcv_msg_source, "\n") == NULL)
    {
        cmd->rcv_msg_source[len] = 0x0D;
        cmd->rcv_msg_source[len + 1] = 0x0A;
        cmd->rcv_msg_source[len + 2] = 0x00;
        len += 2;
    }
#endif

    if (len > 0 && len < 200 && j > 0 && len - j < 90)
    {
        memcpy(&buf[9], cmd->rcv_msg_source + j, len - j);
        track_cust_module_power_cmd(&cmd->return_sms, buf, 9 + len - j);
        //track_drv_uart2_write_data(buf, 9 + len - j);
    }
}

#endif

#if defined ( __GT300S__)
static void cmd_extchip(CMD_DATA_STRUCT * cmd)
{
    int len, j;
    U8 buf[100] = {"AT^GT_CM="};
    extern void track_cust_module_power_cmd(cmd_return_struct * return_sms, kal_uint8 * data, int len);
    j = cmd->pars[1] - cmd->rcv_msg;
    len = strlen(cmd->rcv_msg_source);
    LOGD(L_APP, L_V2, "%d,%d,%d", len, j, strlen(cmd->rcv_msg_source));
    LOGH(L_APP, L_V2, cmd->rcv_msg_source, strlen(cmd->rcv_msg_source));
    if (cmd->rcv_msg_source[len - 1] == 0x0D)
    {
        cmd->rcv_msg_source[len - 1] = 0x00;
        len--;
    }
    if (len > 0 && len < 200 && j > 0 && len - j < 90)
    {
        memcpy(&buf[9], cmd->rcv_msg_source + j, len - j);
        track_cust_module_power_cmd(&cmd->return_sms, buf, 9 + len - j);
        //track_drv_uart2_write_data(buf, 9 + len - j);
    }
}

static void cmd_mode(CMD_DATA_STRUCT * cmd)
{
    nvram_work_mode_struct chip = {0};
    int value = 0;
    int value1 = 0;
    int value2 = 0;
    int mode = 0;
    extern void track_cust_module_extchip_shutdown(void);



    if (cmd->part == 0)
    {
        if (G_parameter.work_mode.mode == 2)
        {
            sprintf(cmd->rsp_msg, "%s:%d,%d", cmd->pars[0], G_parameter.work_mode.mode, G_parameter.gps_work.Fti.interval_acc_on);
        }
        else  if (G_parameter.work_mode.mode == 1)
        {
            sprintf(cmd->rsp_msg, "%s:%d,%d", cmd->pars[0], G_parameter.work_mode.mode, G_parameter.work_mode.mode1_timer);

        }
        else if (G_parameter.work_mode.mode == 3)
        {

            sprintf(cmd->rsp_msg, "%s:%d,%0.2d:%0.2d,%d", cmd->pars[0], G_parameter.work_mode.mode,
                    G_parameter.work_mode.datetime / 60,  G_parameter.work_mode.datetime % 60,  G_parameter.work_mode.mode3_timer / 60);

        }
        else
        {
            sprintf(cmd->rsp_msg, "Error: Parameter");
        }
        return;
    }
    memcpy(&chip, &G_parameter.work_mode, sizeof(nvram_work_mode_struct));
    if (!strcmp(cmd->pars[1], "2"))
    {
        /*
            T1 为定时上传间隔: 10 - 1800 秒
            T2 为持续工作时间:  5 - 43200 分钟
        */
        U16 par;

        if (cmd->part != 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error! %d", cmd->part);
            return;
        }
        //if (cmd->part >= 2)
        {
            value  = track_fun_atoi(cmd->pars[2]);
            if (value < 10 || value > 1800)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (10 to 1800)");
                return;
            }
            chip.mode = 2;
            G_parameter.gps_work.Fti.interval_acc_off = value ;
            G_parameter.gps_work.Fti.interval_acc_on = value ;
        }
    }
    else if (!strcmp(cmd->pars[1], "1"))
    {
        chip.mode = 1;
        value = track_fun_atoi(cmd->pars[2]);
        if ((value < 5 || value > 7200) && value != 0)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 :0,5 to 7200");
            return;
        }

        chip.mode1_timer =  value;

    }
    else if (!strcmp(cmd->pars[1], "3"))
    {
        /*
            T1 起始时间
            T2 间隔时间
        */
        if (cmd->part != 3)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error! %d", cmd->part);
            return;
        }
        if (cmd->part >= 2)
        {
            char *par = strchr(cmd->pars[2], ':');
            mode = track_fun_atoi(cmd->pars[1]);
            if (NULL == par || strlen(cmd->pars[2]) > 5 || strlen(cmd->pars[2]) < 3)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM)");
                return;
            }
            *par = 0;
            value  = track_fun_atoi(cmd->pars[2]);
            if (value < 0 || value > 23)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM) HH:0 - 23");
                return;
            }
            value1  = track_fun_atoi(par + 1);
            if (value1 < 0 || value1 > 59)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM) MM:0 - 59");
                return;
            }
            value2 = track_fun_atoi(cmd->pars[3]);
            if (value2 < 1 || value2 > 24)
            {
                if (mode == 2 && value2 != 0)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 3 0~24");
                    return;
                }
                if (mode == 3)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 3 1~24");
                    return;
                }
            }
            if (mode <= 0 || mode > 3)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter1 1~3");
                return;
            }
            chip.mode = mode;
            chip.datetime = value * 60 + value1;
            chip.mode3_timer =  value2 * 60;
            if (chip.mode3_timer == 0)
            {

            }
            else if ((24 % (chip.mode3_timer / 60) != 0) && chip.mode3_timer != 0)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter3  0,1,2,3,4,6,8,12,24");
                return;
            }
        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter1 1~3");
        return;
    }
    memcpy(&G_parameter.work_mode, &chip, sizeof(nvram_work_mode_struct));
    if (G_realtime_data.tracking_mode)
    {
        G_realtime_data.tracking_mode = 0;
        G_realtime_data.tracking_time = 0;
        G_realtime_data.tracking_status_mode = 0;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
        track_cust_module_sw_init();
        track_cust_wake_mode();
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

#elif defined(__GT740__) || defined (__GT420D__)

//__GT370__/GT740 UART
static void cmd_RESETPRO(CMD_DATA_STRUCT * cmd)
{
    track_cust_module_resetpro();
    sprintf(cmd->rsp_msg, "OK!");
}

static void cmd_remalmset(CMD_DATA_STRUCT * cmd)
{
    nvram_teardown_struct alarm = {0};
    int val;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d,%d", cmd->pars[0], G_parameter.teardown_str.alarm_upcount,G_parameter.teardown_str.alarm_vel_min);
        return;
    }

    memcpy(&alarm, &G_parameter.teardown_str, sizeof(nvram_teardown_struct));
    val = track_fun_atoi(cmd->pars[1]);

    if (val < 1 || val > 99)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (1 to 99)");
        return;
    }
    alarm.alarm_upcount=val;
    if (cmd->part >= 2)
    {
        val = track_fun_atoi(cmd->pars[2]);

        if (val < 6 || val > 99)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (6 to 99)");
            return;
        }
        alarm.alarm_vel_min=val;
    }
    if (cmd->part >= 3)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    memcpy(&G_parameter.teardown_str, &alarm, sizeof(nvram_teardown_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

static void cmd_BATPTC(CMD_DATA_STRUCT * cmd)
{
    int value;
    if (cmd->part == 0)
    {
        //track_cust_batptc_data(cmd->rsp_msg,-1);
        return;
    }
    else
    {
        value = atoi(cmd->pars[1]);
        if (value < 0 || value > 100)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (0 to 100)");
            return;
        }
        //track_cust_batptc_data(cmd->rsp_msg,value);
    }

}
static void cmd_batcal(CMD_DATA_STRUCT * cmd)
{
    U8 static_filter_sw;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], ON_OFF(!G_parameter.NO_calibration));
        return;
    }
    static_filter_sw = G_parameter.NO_calibration;
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part > 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        static_filter_sw = 0;
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        static_filter_sw = 1;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    G_parameter.NO_calibration = static_filter_sw;
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

static void cmd_lbssw(CMD_DATA_STRUCT * cmd)
{
    U8 static_filter_sw;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], ON_OFF(G_parameter.lbs_sw));
        return;
    }
    static_filter_sw = G_parameter.lbs_sw;
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part > 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        static_filter_sw = 1;
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        static_filter_sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    G_parameter.lbs_sw = static_filter_sw;
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

static void cmd_gpswakeup(CMD_DATA_STRUCT * cmd)
{
    U8 static_filter_sw;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], ON_OFF(G_parameter.gpswake));
        return;
    }
    static_filter_sw = G_parameter.gpswake;
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part > 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        static_filter_sw = 1;
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        static_filter_sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    G_parameter.gpswake = static_filter_sw;
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}


static void cmd_amslog(CMD_DATA_STRUCT * cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s" , cmd->pars[0],
                ON_OFF(G_realtime_data.ams_log));
        return;
    }
    if (!strcmp(cmd->pars[1], "ON"))
    {
        G_realtime_data.ams_log = 1;
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        G_realtime_data.ams_log = 0;

    }
    else if (!strcmp(cmd->pars[1], "DEL"))
    {
        int rst = 0;
        rst = track_cust_netlog_del();
        sprintf(cmd->rsp_msg, "del %d", rst);
        return;

    }
    else if (!strcmp(cmd->pars[1], "PUSH"))
    {
        U8 resetlt = 0;
        LOGD(L_OBD, L_V5, "PUSH");
        if (!strcmp(cmd->pars[2], "STOP"))
        {
            G_realtime_data.ams_push = 0;
            sprintf(cmd->rsp_msg, "%s", ON_OFF(G_realtime_data.ams_push));
        }
        else
        {
            G_realtime_data.ams_push = 1;
            sprintf(cmd->rsp_msg, "%s", ON_OFF(G_realtime_data.ams_push));
        }
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        return;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1");
        return;
    }
    if (Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
}

static void cmd_RESETBAT(CMD_DATA_STRUCT * cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "RESETBAT:OK");
        track_cust_resetbat(2);
        return;
    }
}
static void cmd_RESETBATDIS(CMD_DATA_STRUCT * cmd)
{
    if (!strcmp(cmd->pars[1], "ON"))
    {
        G_parameter.cal_time_sw = 1;
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        G_parameter.cal_time_sw = 0;
    }
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
}
#if !defined(__GT420D__)
static void cmd_mode(CMD_DATA_STRUCT * cmd)
{
    nvram_ext_chip_struct chip = {0};
    int value;
    kal_bool real=KAL_FALSE;
    if (cmd->part == 0)
    {
        if (G_realtime_data.tracking_mode)
        {
            sprintf(cmd->rsp_msg, "%s:2,%0.2d:%0.2d,%d(MIN)", cmd->pars[0],
                    G_parameter.extchip.mode2_par1 / 60, G_parameter.extchip.mode2_par1 % 60, G_parameter.teardown_str.alarm_vel_min);
        }
        else if (G_parameter.extchip.mode1_en)
        {
            sprintf(cmd->rsp_msg, "%s:1,%d,%d", cmd->pars[0],
                    G_parameter.gps_work.Fti.interval_acc_off, G_parameter.extchip.mode1_par2);
        }
        else if (G_parameter.extchip.mode == 1)
        {
            sprintf(cmd->rsp_msg, "%s:%d,%d,%d", cmd->pars[0], G_parameter.extchip.mode,
                    G_parameter.gps_work.Fti.interval_acc_off, G_parameter.extchip.mode1_par2);
        }
        else if (G_parameter.extchip.mode == 2)
        {
            if (G_parameter.extchip.mode2_par2 % 60 == 0)
            {
                sprintf(cmd->rsp_msg, "%s:%d,%0.2d:%0.2d,%d", cmd->pars[0], G_parameter.extchip.mode,
                        G_parameter.extchip.mode2_par1 / 60, G_parameter.extchip.mode2_par1 % 60, G_parameter.extchip.mode2_par2 / 60);
            }
            else
            {
                sprintf(cmd->rsp_msg, "%s:%d,%0.2d:%0.2d,%d(MIN)", cmd->pars[0], G_parameter.extchip.mode,
                        G_parameter.extchip.mode2_par1 / 60, G_parameter.extchip.mode2_par1 % 60, G_parameter.extchip.mode2_par2);
            }
        }
        else if (G_parameter.extchip.mode == 3)
        {
            sprintf(cmd->rsp_msg, "%s:%d,%d,%0.2d:%0.2d", cmd->pars[0], G_parameter.extchip.mode, G_parameter.extchip.mode3_par1,
                    G_parameter.extchip.mode3_par2 / 60, G_parameter.extchip.mode3_par2 % 60);
        }
        else if (G_parameter.extchip.mode == 4)
        {
            char m_buf[100] = {0};
            U8 vle = G_parameter.extchip.mode4_par1, i = 0;
            for (; i < vle; i++)
            {
                if (0 == i)
                {
                    snprintf(m_buf + strlen(m_buf), 99, "%0.2d:%0.2d", G_parameter.extchip.mode4_par2[i] / 60, G_parameter.extchip.mode4_par2[i] % 60);
                }
                else
                {
                    snprintf(m_buf + strlen(m_buf), 99, ",%0.2d:%0.2d", G_parameter.extchip.mode4_par2[i] / 60, G_parameter.extchip.mode4_par2[i] % 60);
                }
            }
            sprintf(cmd->rsp_msg, "%s:%d,%d,%s", cmd->pars[0], G_parameter.extchip.mode, G_parameter.extchip.mode4_par1, m_buf);
        }
        else
        {
            sprintf(cmd->rsp_msg, "Error: Parameter");
        }
        return;
    }
    memcpy(&chip, &G_parameter.extchip, sizeof(nvram_ext_chip_struct));
    if (!strcmp(cmd->pars[1], "1"))
    {
        /*
            T1 为定时上传间隔: 10 - 43200 秒
            T2 为持续工作时间:  5 - 43200 分钟
        */
        U16 par;
        if (!strcmp(cmd->pars[2], "VIEW"))
        {
            track_cust_module_auto_sw_mode();
            return;
        }
        if (cmd->part != 3)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error! %d", cmd->part);
            return;
        }
        if (G_parameter.extchip.mode != 1)
        {
            chip.premode = G_parameter.extchip.mode;
        }
        chip.mode = 1;
        par = G_parameter.gps_work.Fti.interval_acc_on;
        if (cmd->part >= 2)
        {
            value  = track_fun_atoi(cmd->pars[2]);
#ifdef __GT741__
            if ((value < 60 || value > 43200) )
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (60 to 43200)");
                return;
            }
#else
if (value < 10 || value > 43200)
{
    sprintf(cmd->rsp_msg, "Error: Parameter 2 (10 to 43200)");
    return;
}
#endif
            chip.mode1_par1 = value;
            if (value>=EXCHANGE_MODE_THRESHOLD_VALUESEC)
            {
                chip.mode=2;
                chip.mode1_en=KAL_TRUE;
            }
            else
            {
                chip.mode1_en=KAL_FALSE;
            }
        }
        if (cmd->part >= 3)
        {
            value  = track_fun_atoi(cmd->pars[3]);
            if (value < 5 || value > 43200)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (5 to 43200)");
                return;
            }
            chip.mode1_par2 = value;
            {
                static applib_time_struct tmp = {0};
                OTA_applib_dt_get_rtc_time(&tmp);
                chip.mode1_time = OTA_applib_dt_mytime_2_utc_sec(&tmp, 0) + chip.mode1_par2 * 60;
                LOGD(L_OBD, L_V5, "time: %0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d ",
                     tmp.nYear, tmp.nMonth, tmp.nDay,
                     tmp.nHour, tmp.nMin, tmp.nSec);
            }
        }
        if (G_parameter.gps_work.Fti.interval_acc_on != chip.mode1_par1 || G_parameter.gps_work.Fti.interval_acc_off != chip.mode1_par1)
        {
            G_parameter.gps_work.Fti.interval_acc_on = chip.mode1_par1;
            G_parameter.gps_work.Fti.interval_acc_off = chip.mode1_par1;
            G_parameter.gps_work.Fti.lbs_interval = chip.mode1_par1;
        }
    }
    else if (!strcmp(cmd->pars[1], "2"))
    {
        if (cmd->part < 2 || cmd->part > 3)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error! %d", cmd->part);
            return;
        }
        if (cmd->part >= 2)
        {
            int value1;
            char *par = strchr(cmd->pars[2], ':');
            if (NULL == par || strlen(cmd->pars[2]) > 5 || strlen(cmd->pars[2]) < 3)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM)");
                return;
            }
            *par = 0;
            value  = track_fun_atoi(cmd->pars[2]);
            if (value < 0 || value > 23)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM) HH:0 - 23");
                return;
            }
            value1  = track_fun_atoi(par + 1);
            if (value1 < 0 || value1 > 59)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM) MM:0 - 59");
                return;
            }
            chip.mode = 2;
            chip.mode2_par1 = value * 60 + value1;
        }
        if (cmd->part >= 3)
        {
            if (strstr(cmd->pars[3], "MIN") == cmd->pars[3])
            {
                value  = track_fun_atoi(cmd->pars[3] + 3);
                if (value < 5)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 3(%d < 5MIN)", value);
                    return;
                }
                chip.mode2_par2 = value;
            }
            else
            {
                value  = track_fun_atoi(cmd->pars[3]);
                if (value<1 || value>1440)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 3 (1-1440)");
                    return;
                }
                if (value<24)
                {
                    if (24 % value!=0)
                    {
                        sprintf(cmd->rsp_msg, "Error: Parameter 3!");
                        return;
                    }
                }
                else
                {
                    if (value % 24!=0)
                    {
                        sprintf(cmd->rsp_msg, "Error: Parameter 3!");
                        return;
                    }
                }
                chip.mode2_par2 = value * 60;
            }
        }
    }
    else if (!strcmp(cmd->pars[1], "3"))
    {
        kal_int32 vle = 0;
        if (cmd->part < 2 || cmd->part > 3)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error! %d", cmd->part);
            return;
        }
        if (cmd->part >= 2)
        {
            vle = track_fun_atoi2(4, cmd->pars[2]);
            if (-99 == vle)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 Number contains non-numeric characters or range out(1-7)!");
                return;
            }
            else if (strlen(cmd->pars[2]) > 7)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 Number length MAX 7!");
                return;
            }
            chip.mode3_par1 = vle;
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
            {
                int i;
                memset(chip.mode3_weekday, 0x00, 7);
                for (i = 0; i < strlen(cmd->pars[2]); i++)
                {
                    if (*(cmd->pars[2] + i) == '7')
                    {
                        *(cmd->pars[2] + i) = '0';
                    }
                    chip.mode3_weekday[*(cmd->pars[2] + i) - 0x30] = 1;
                }
                LOGH(L_APP, L_V5, chip.mode3_weekday, 7);
            }
#endif//__MCU_LONG_STANDBY_PROTOCOL__
        }
        if (cmd->part >= 3)
        {
            int value1;
            char *par = strchr(cmd->pars[3], ':');
            if (NULL == par || strlen(cmd->pars[3]) > 5 || strlen(cmd->pars[3]) < 3)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (HH:MM)");
                return;
            }
            *par = 0;
            value  = track_fun_atoi(cmd->pars[3]);
            if (value < 0 || value > 23)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (HH:MM) HH:0 - 23");
                return;
            }
            value1  = track_fun_atoi(par + 1);
            if (value1 < 0 || value1 > 59)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (HH:MM) MM:0 - 59");
                return;
            }
            chip.mode = 3;
            chip.mode3_par2 = value * 60 + value1;
        }
    }
    else if (!strcmp(cmd->pars[1], "4"))
    {
        int vle = 0;
        if (cmd->part < 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error! %d", cmd->part);
            return;
        }
        if (cmd->part >= 2)
        {
            vle = atoi(cmd->pars[2]);
            if (vle < 1 || vle > 12)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (1-12)");
                return;
            }
            chip.mode4_par1 = vle;
        }
        if (cmd->part - 2 == vle) //mode,4,12,11:00,,,,,,,
        {
            int value1, i = 0;
            for (; i < vle; i++)
            {
                char *par = strchr(cmd->pars[3 + i], ':');
                if (NULL == par || strlen(cmd->pars[3 + i]) > 5 || strlen(cmd->pars[3 + i]) < 3)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter %d (HH:MM)", 3 + i);
                    return;
                }
                *par = 0;
                value  = track_fun_atoi(cmd->pars[3 + i]);
                if (value < 0 || value > 23)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter %d (HH:MM) HH:0 - 23", 3 + i);
                    return;
                }
                value1  = track_fun_atoi(par + 1);
                if (value1 < 0 || value1 > 59)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter %d (HH:MM) MM:0 - 59", 3 + i);
                    return;
                }
                chip.mode4_par2[i] = value * 60 + value1;
            }
            chip.mode = 4;
        }
        else
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!(HH:MM) %d", vle);
            return;
        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (1/2/3/4)");
        return;
    }

#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
    if (chip.mode == 2)
    {
        applib_time_struct time = {0};
        track_fun_get_time(&time, KAL_FALSE, &G_parameter.zone);
        G_parameter.extchip.mode2_start_year = time.nYear;
        G_parameter.extchip.mode2_start_month = time.nMonth;
        G_parameter.extchip.mode2_start_day = time.nDay;
        LOGD(L_APP, L_V1, "mode2 start time:%d,%d,%d", G_parameter.extchip.mode2_start_year,G_parameter.extchip.mode2_start_month, G_parameter.extchip.mode2_start_day);
    }
#endif//__MCU_LONG_STANDBY_PROTOCOL__
    if (strcmp(cmd->pars[1], "1"))
    {
        chip.mode1_en=KAL_FALSE;
    }
    if (chip.mode1_en)
    {
        track_cust_gps_module_work((void *)18);
        track_cust_into_mode1en();
        track_cust_rem_tracking_time(2,NULL);
        if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
        {
            tr_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
        }
    }
    else
    {
        track_cust_rem_tracking_time(2,NULL);
    }
    if (G_realtime_data.remove_time>0)
    {
        G_realtime_data.remove_time=0;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    }

    if (!memcmp(&G_parameter.extchip, &chip, sizeof(nvram_ext_chip_struct)))
    {
        sprintf(cmd->rsp_msg, "OK!");
        if (track_cust_module_tracking_mode(99))
        {
            track_cust_module_tracking_mode(0);
            track_cust_module_update_mode();//
        }
        LOGD(L_OBD, L_V5, "no change! %d,%d", G_parameter.extchip.mode, chip.mode);
        return;
    }
    if (G_parameter.extchip.mode==1 && chip.mode!=1)
    {
        real=KAL_TRUE;
    }
    memcpy(&G_parameter.extchip, &chip, sizeof(nvram_ext_chip_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
#if defined(__GT740__)
        if (track_cust_module_tracking_mode(99))
        {
            track_cust_module_tracking_mode(0);
        }
        track_cust_shutdown_all_location();
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
        if (OTA_kal_get_systicks() > 600)
        {
            track_cust_module_update_mode();//
        }
        track_mlsp_GT740_delay_force_shutdown();
#else
track_cust_module_update_mode();//
#endif//__MCU_LONG_STANDBY_PROTOCOL__
        track_cust_upload_param();
        track_stop_timer(TRACK_CUST_EXTCHIP_TRACK_MODE_TIMEROUT_TIMER_ID);
        if (G_parameter.extchip.mode == 1)
        {
            track_cust_gps_module_work((void *)8);
            track_cust_rem_tracking_time(1,NULL);
            if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
            {
                tr_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
            }
        }
        else
        {
            if (cmd->return_sms.cm_type != CM_PARAM_CMD && real)
            {
                if (G_parameter.wifi.sw)
                {
                    track_cust_sendWIFI_paket();
                }
                else
                {
                    track_cust_sendlbs_limit();
                }
            }
        }
#endif
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

#else
static void cmd_mode(CMD_DATA_STRUCT * cmd)//420
{
    extern void track_cust_module_delay_close(U8 sec);
    extern void track_cust_send04(void* par);
    //extern void track_mode2_to_mode1(void);
    nvram_ext_chip_struct chip = {0};
    kal_uint16 value;
    kal_bool real=KAL_FALSE;
    if (cmd->part == 0)
    {
        if (G_parameter.extchip.mode == 1)
        {
            if(track_cust_gt420d_carstatus_data(99) == 2)
            {
                sprintf(cmd->rsp_msg, "%s:%d,%d(min)", cmd->pars[0], G_parameter.extchip.mode, G_parameter.statictime.mode1_statictime);
                }
            else
            {
                sprintf(cmd->rsp_msg, "%s:%d,%d(min)", cmd->pars[0], G_parameter.extchip.mode, G_parameter.extchip.mode1_par1);
            }
        }
        else if (G_parameter.extchip.mode == 2)
        {
            if(track_cust_gt420d_carstatus_data(99) == 2)
            {
                sprintf(cmd->rsp_msg, "%s:%d,%d(min)", cmd->pars[0], G_parameter.extchip.mode, G_parameter.statictime.mode2_statictime);
                }
            else
            {
                sprintf(cmd->rsp_msg, "%s:%d,%d(s)", cmd->pars[0], G_parameter.extchip.mode, G_parameter.extchip.mode2_par1);
            }
        }
        else if (G_parameter.extchip.mode == 3)
        {
            sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], G_parameter.extchip.mode);
        }
        else
        {
            sprintf(cmd->rsp_msg, "Error: Parameter");
        }
        return;
    }
    memcpy(&chip, &G_parameter.extchip, sizeof(nvram_ext_chip_struct));
    if (!strcmp(cmd->pars[1], "1"))
    {
        /*
        T1 为定时上传间隔: 1- 43200 min
        T1 :1-5min			  GSM长连接，WIFI根据T1的间隔间歇开启定位功能。
        T1 :6-43200min		  根据T1的间隔，单片机定位唤醒GSM。
        */
        if (cmd->part > 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error! %d", cmd->part);
            return;
        }

        if (cmd->part == 1)
        {
            chip.mode = 1;
        }
        if (cmd->part == 2)
        {
            chip.mode = 1;
            value  = track_fun_atoi(cmd->pars[2]);
            if ((value < 1 || value > 43200))
            {
                sprintf(cmd->rsp_msg, "FAIL");
                return;
            }
            chip.mode1_par1 = value;
        }
    }
    else if (!strcmp(cmd->pars[1], "2"))
    {
        /*
        T1 为定时上传间隔: 0-60 day
        T1 :0			 GSM一直关机，
        T1 :1-60	按照T1时间唤醒GSM。唤醒后工作流程同模式1间隔大于5分钟。
        */
        if (cmd->part > 2)
        {
            sprintf(cmd->rsp_msg, "FAIL");
            return;
        }
        if (cmd->part == 1)
        {
            chip.mode = 2;
        }
        if (cmd->part == 2)
        {
            chip.mode = 2;
            value  = track_fun_atoi(cmd->pars[2]);
            if ((value < 10 || value > 3600))
            {
                sprintf(cmd->rsp_msg, "FAIL");
                return;
            }
            chip.mode2_par1 = value;
        }
    }
    else if (!strcmp(cmd->pars[1], "3"))
    {
        if (cmd->part > 1)
        {
            sprintf(cmd->rsp_msg, "FAIL");
            return;
        }
        chip.mode = 3;
    }
    else if (strcmp(cmd->pars[1], "2") && strcmp(cmd->pars[1], "1") && strcmp(cmd->pars[1], "3"))
    {
        sprintf(cmd->rsp_msg, "FAIL");
        return;
    }

    memcpy(&G_parameter.extchip, &chip, sizeof(nvram_ext_chip_struct));
    
    if (G_parameter.extchip.mode == 1)
    {
        track_cust_send17((void*)1,(void*)1);
    }
    if (G_parameter.extchip.mode == 2)
    {
        track_cust_send17((void*)2,(void*)1);
    }
   
        if (track_is_timer_run(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID))
        {
            track_stop_timer(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID);
        }
        if (G_parameter.extchip.mode == 2)
        {
            if (track_is_timer_run(TRACK_CUST_NET_MAXMUM_WORK_TIME_TIMER_ID))
            {
                track_stop_timer(TRACK_CUST_NET_MAXMUM_WORK_TIME_TIMER_ID);
            }
            if (track_is_timer_run(TRACK_CUST_SEND_DATA_MAXMUM_WORK_TIME_TIMER_ID))
            {
                track_stop_timer(TRACK_CUST_SEND_DATA_MAXMUM_WORK_TIME_TIMER_ID);
            }
            if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
            {
                track_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
            }
            if (track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
            {
                tr_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
            }
        }
        if (G_parameter.extchip.mode == 1)
        {
            track_stop_timer(TRACK_CUST_MODE2_KEEPGOING_TIMER_ID);
            track_stop_timer(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
            track_start_timer(TRACK_CUST_NET_MAXMUM_WORK_TIME_TIMER_ID,270000, track_cust_module_delay_close, (void *)30);
            track_start_timer(TRACK_CUST_SEND_DATA_MAXMUM_WORK_TIME_TIMER_ID,270000, track_cust_module_delay_close, (void *)30);
        }
        
         if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE)\
            && Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE))
    {
        sprintf(cmd->rsp_msg, "SUCCESS");
        track_cust_paket_9404_handle();
        LOGD(L_APP,L_V5,"修改了MODE 指令");
        if (strcmp(cmd->pars[1], "3"))
        {
            track_stop_timer(TRACK_CUST_MODE2_KEEPGOING_TIMER_ID);
        }
        if (G_parameter.extchip.mode == 1 ||G_parameter.extchip.mode == 2 )
        {
            track_cust_send04((void *)G_parameter.extchip.mode);
            if (track_cust_wifi_scan_status())
            {
                LOGD(L_APP,L_V5,"检测到WIFI正在搜寻，中断关闭WIFI");
                track_drv_wifi_cmd(0);
            }
            track_cust_module_update_mode();
        }
        else
        {
            track_start_timer(TRACK_CUST_EXTCHIP_WAIT04_TIMER_ID, 5000, track_cust_send04, (void *)G_parameter.extchip.mode);
        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "FAIL");
    }
}


static void cmd_collide(CMD_DATA_STRUCT * cmd)
{
    nvram_crash_struct temp = {0};
    int value = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg,"%s,%s,%d,%d",cmd->pars[0],ON_OFF(G_parameter.crash_str.sw),G_parameter.crash_str.alarm_type,\
                G_parameter.crash_str.crash_grade);
        return;
    }
    if (cmd->part > 0)
    {
        if (!strcmp(cmd->pars[1], "ON"))
        {
            if (cmd->part < 1 || cmd->part > 3)
            {
                sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
                return;
            }
            temp.sw = 1;
            if (cmd->part >= 2)
            {
                value = track_fun_atoi(cmd->pars[2]);
                if (value != 0 && value != 1)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1)");
                    return;
                }
                temp.alarm_type = value;
            }
            if (cmd->part >= 3)
            {
                value = track_fun_atoi(cmd->pars[3]);
                if (value < 10 || value > 1024)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 3 (10 - 1024)");
                    return;
                }
                temp.crash_grade = value;
            }
        }
        else if (!strcmp(cmd->pars[1], "OFF"))
        {
            if (cmd->part != 1)
            {
                sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
                return;
            }
            temp.sw = 0;
        }
        else
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
            return;
        }
    }
    memcpy(&G_parameter.crash_str, &temp, sizeof(nvram_crash_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
        track_cust_updata_crash_mcu();
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }

}
#if 0
static void cmd_tiltalm(CMD_DATA_STRUCT * cmd)
{
    nvram_incline_struct temp = {0};
    int value = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg,"%s,%s,%d,%d",cmd->pars[0],ON_OFF(G_parameter.incline_str.sw),G_parameter.incline_str.alarm_type,\
                G_parameter.incline_str.incline_grade);
        return;
    }
    if (cmd->part > 0)
    {
        if (!strcmp(cmd->pars[1], "ON"))
        {
            if (cmd->part < 1 || cmd->part > 3)
            {
                sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
                return;
            }
            temp.sw = 1;
            if (cmd->part >= 2)
            {
                value = track_fun_atoi(cmd->pars[2]);
                if (value != 0 && value != 1)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1)");
                    return;
                }
                temp.alarm_type = value;
            }
            if (cmd->part >= 3)
            {
                value = track_fun_atoi(cmd->pars[3]);
                if (value < 1 || value > 180)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 3 (1 - 180)");
                    return;
                }
                temp.incline_grade= value;
            }
        }
        else if (!strcmp(cmd->pars[1], "OFF"))
        {
            if (cmd->part != 1)
            {
                sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
                return;
            }
            temp.sw = 0;
        }
        else
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
            return;
        }
    }
    memcpy(&G_parameter.incline_str, &temp, sizeof(nvram_crash_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
        track_cust_updata_incline_mcu();
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}
#endif
#endif

#elif defined(__GT370__)|| defined (__GT380__)
extern void track_cust_charge_switch_mode(kal_uint8 chr_in);
static void cmd_mode(CMD_DATA_STRUCT * cmd)
{
    nvram_work_mode_struct chip = {0};
    int value = 0;
    int value1 = 0;
    int value2 = 0;
    int mode = 0;

    if (cmd->part == 0)
    {
        mode = (G_parameter.work_mode.mode > 1 ? G_parameter.work_mode.mode - 1 : G_parameter.work_mode.mode);
        if (mode == 1)
        {
            sprintf(cmd->rsp_msg, "%s:%d,%d,%d", cmd->pars[0], mode, \
                    G_parameter.gps_work.Fti.interval_acc_on, G_parameter.gps_work.Fti.interval_acc_off);
        }
        else if (mode == 2)
        {
            //sprintf(cmd->rsp_msg, "%s:%d,%0.2d:%0.2d,%d", cmd->pars[0], mode,
            //        G_parameter.work_mode.datetime / 60,  G_parameter.work_mode.datetime % 60,  G_parameter.work_mode.mode3_timer / 60);

            if ((G_parameter.work_mode.mode3_timer % 60) != 0)
            {
                sprintf(cmd->rsp_msg, "%s:%d,%0.2d:%0.2d,%dmin", cmd->pars[0], G_parameter.extchip.mode,
                        G_parameter.work_mode.datetime / 60,  G_parameter.work_mode.datetime % 60,  G_parameter.work_mode.mode3_timer);
            }
            else
            {
                sprintf(cmd->rsp_msg, "%s:%d,%0.2d:%0.2d,%d", cmd->pars[0], mode,
                        G_parameter.work_mode.datetime / 60,  G_parameter.work_mode.datetime % 60,  G_parameter.work_mode.mode3_timer / 60);
            }

        }
        else
        {
            sprintf(cmd->rsp_msg, "Error: Parameter");
        }
        return;
    }
    memcpy(&chip, &G_parameter.work_mode, sizeof(nvram_work_mode_struct));
    if (!strcmp(cmd->pars[1], "1"))
    {
        /*     T1 为定时上传间隔: 10 - 3600 秒
               T2 为持续工作时间: 10 - 86400 秒        */
        U16 par;

        if (cmd->part != 3)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error! %d", cmd->part);
            return;
        }

        value  = track_fun_atoi(cmd->pars[2]);
        if (value < 10 || value > 3600)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (10 to 3600)");
            return;
        }

        G_parameter.gps_work.Fti.interval_acc_on = value ;

        value = track_fun_atoi(cmd->pars[3]);
        if ((value < 180 || value > 65535) && value != 0)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 :180 to 65535");
            return;
        }
        G_parameter.gps_work.Fti.interval_acc_off = value ;
        chip.mode1_timer =	G_parameter.gps_work.Fti.interval_acc_on;

        //chip.mode1_timer =	value;
        if (G_parameter.gps_work.Fti.interval_acc_on > 120)
        {
            chip.mode = 1;
        }
        else
        {
            chip.mode = 2;
        }

        G_parameter.pre_mode.mode = 0;

    }
    else if (!strcmp(cmd->pars[1], "2"))
    {
        /* T1 起始时间
           T2 间隔时间 */
        if (cmd->part != 3)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error! %d", cmd->part);
            return;
        }
        if (cmd->part >= 2)
        {
            char *par = strchr(cmd->pars[2], ':');
            mode = track_fun_atoi(cmd->pars[1]);//无聊不?
            if (NULL == par || strlen(cmd->pars[2]) > 5 || strlen(cmd->pars[2]) < 3)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM)");
                return;
            }
            *par = 0;
            value  = track_fun_atoi(cmd->pars[2]);
            if (value < 0 || value > 23)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM) HH:0 - 23");
                return;
            }
            value1  = track_fun_atoi(par + 1);
            if (value1 < 0 || value1 > 59)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM) MM:0 - 59");
                return;
            }

            /*小范围便于测试       --    chengjun  2017-04-01*/
            if (strstr(cmd->pars[3], "MIN") == cmd->pars[3])
            {
                value2  = track_fun_atoi(cmd->pars[3] + 3);
                if (value2 < 5)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 3(%d < 5MIN)", value2);
                    return;
                }
                chip.mode = 3;
                chip.datetime = value * 60 + value1;
                chip.mode3_timer =  value2;
            }
            else
            {
                value2 = track_fun_atoi(cmd->pars[3]);
#if 1
                if ((value2 == 1) || (value2 == 2) || (value2 == 3) || (value2 == 4) || (value2 == 6) || (value2 == 8) || (value2 == 12) || (value2 == 24)\
                        || (value2 == 48) || (value2 == 72))
                {
                    //1,2,3,4,6,8,12,24,48,72
                }
                else
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter 3 (1,2,3,4,6,8,12,24,48,72)");
                    return;
                }

#else
if (value2 < 1 || value2 > 72)
{
    if (mode == 2)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 3 1~72");
        return;
    }
}
#endif

                if (mode <= 0 || mode > 2)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter1 (1~2)");
                    return;
                }
                chip.mode = 3;
                chip.datetime = value * 60 + value1;
                chip.mode3_timer =  value2 * 60;
            }
        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter1 (1~2)");
        return;
    }
    memcpy(&G_parameter.work_mode, &chip, sizeof(nvram_work_mode_struct));
    if (G_realtime_data.tracking_mode)
    {
        G_realtime_data.tracking_mode = 0;
        G_realtime_data.tracking_time = 0;
        G_realtime_data.tracking_status_mode = 0;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    }

    /*充电中改模式3超长待机的特殊处理       --    chengjun  2017-06-24*/
    if (track_drv_bmt_get_charging_status()!=0)
    {
        track_cust_charge_switch_mode(1);
    }

    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
        track_cust_module_sw_init();
        if (G_parameter.work_mode.mode != 3)
        {
            track_stop_timer(TRACK_POWER_RESET_TIMER_ID);
        }

        track_cust_gpsdup_interval_update();

        track_cust_wake_mode();

        track_cust_paket_9404_handle();
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

#endif
#endif
//MCU
static void cmd_dial(CMD_DATA_STRUCT * Cmds)
{
    kal_uint32 len = 0;
    kal_uint32 value = 0;
    LOGD(L_CMD, L_V5, "part= %d", Cmds->part);
    if (Cmds->part > 2)
    {
        sprintf(Cmds->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    len = strlen(Cmds->pars[1]);
    if (len > 0)
    {
        if (len > 0)
        {
            if (track_fun_check_str_is_number(1, Cmds->pars[1]) == 0)
            {
                sprintf(Cmds->rsp_msg, "Error: Number contains non-numeric characters!");
                return;
            }
            if (len >= TRACK_DEFINE_PHONE_NUMBER_LEN_MAX)
            {
                sprintf(Cmds->rsp_msg, "Error: Number length exceeds 20 characters!");
                return;
            }
            else if (len < 3)
            {
#if defined (__GERMANY__)
                sprintf(Cmds->rsp_msg, "Fehler: Mindestens 3 Zeichen eingeben!");
#else
                sprintf(Cmds->rsp_msg, "Error: Number length must exceeds 3 characters!");
#endif
                return;
            }
            memset(dail_num, 0, sizeof(dail_num));
            dail_count = 0;
            strcpy(dail_num, Cmds->pars[1]);
        }
        if (strlen(Cmds->pars[2]) != 0)
        {
            value = track_fun_atoi(Cmds->pars[2]);
            if ((value < 1 || value > 3))
            {
                sprintf(Cmds->rsp_msg, "Error: Parameter 2 (1-3)");
                return;
            }
            dail_count = value;
        }
        if (strlen(Cmds->pars[2]) != 0)
        {
            LOGD(L_CMD, L_V5, "pars[1] = %s;dail_num=%s", Cmds->pars[1], dail_num);
            LOGD(L_CMD, L_V5, "pars[2] = %s;dail_count=%s", Cmds->pars[2], dail_count);
        }
        else
        {
            LOGD(L_CMD, L_V5, "pars[1] = %s;dail_num=%s", Cmds->pars[1], dail_num);
        }
        sprintf(Cmds->rsp_msg, "OK");
        track_start_timer(TRACK_CUST_DIAL_NUM_TIMER_ID, 5 * 1000, track_cust_dial_makecall, (void*)1);
    }
    else
    {
        sprintf(Cmds->rsp_msg, "ERROR:100");
    }
}
#if defined(__BCA__)
static void cmd_reset(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        track_cust_restart(2, 20);
    }
    else if (cmd->part == 1 && strlen(cmd->pars[CM_Par1]) > 0)
    {
        int sec = atoi(cmd->pars[CM_Par1]);
        if (sec <= 0) track_cust_restart(2, 20);
        track_cust_restart(2, sec);
    }
    else
    {
        sprintf(cmd->rsp_msg, "ERROR:100");
    }
}
#endif
#if defined(__SPANISH__)
/******************************************************************************
 *  Function    -  cmd_sosbt
 *
 *  Purpose     -  sos报警触发模式
 *
 *  Description -
                SOSBT,0# 短按(按一次 触发)
                SOSBT,1# 长按(长按 3秒)
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 13-9-2014,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_sosbt(CMD_DATA_STRUCT *cmd)
{
    kal_uint8 value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], G_parameter.sos_alarm.key_type);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    else
    {
        value = track_fun_atoi(cmd->pars[1]);
        if (value != 1 && value != 0)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (0-1)");
            return;
        }
        else if (G_parameter.sos_alarm.key_type != value)
        {
            G_parameter.sos_alarm.key_type = value;
        }
    }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}
/******************************************************************************
 *  Function    -  cmd_esos
 *
 *  Purpose     -  SOS报警短信自定义
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 13-9-2014,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_esos(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s", G_parameter.sos_alarm.alarm_sms_head);
        return;
    }
    if (strlen(cmd->pars[1]) == 0 || cmd->part != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (strlen(cmd->pars[1]) >= 100)
    {
        sprintf(cmd->rsp_msg, "Error: string is too long!");
        return;
    }
    else
    {
        char tmp[100] = {0};
        int len;
        memset(G_parameter.sos_alarm.alarm_sms_head, 0, 100);
        len = strlen(cmd->pars[1]);
        memcpy(tmp, cmd->rcv_msg_source + (cmd->pars[1] - cmd->rcv_msg), (len >= 100 ? 99 : len));
        sprintf(G_parameter.sos_alarm.alarm_sms_head, tmp);
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        sprintf(cmd->rsp_msg, "OK!");
    }
}
/******************************************************************************
 *  Function    -  cmd_itp
 *
 *  Purpose     -  itp,1# 关闭94包上传功能
                   itp,0# 打开      默认:1
                   itp# 查询当前状态
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 30-3-2017,  Cjj  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_itp(CMD_DATA_STRUCT *cmd)
{
    kal_uint8 value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d", cmd->pars[0], G_parameter.itp_sw);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    else
    {
        value = track_fun_atoi(cmd->pars[1]);
        if (value != 1 && value != 0)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (0-1)");
            return;
        }
        else if (G_parameter.itp_sw != value)
        {
            G_parameter.itp_sw = value;
        }
    }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}
#endif
#if defined(__XCWS__)
//"null"
static char string_null[] = {0x00, 0x00};
static char string_comma[] = {0xFF, 0x0C, 0x00, 0x00};
static char string_point[] = {0x30, 0x02, 0x00, 0x00};
//注册失败，密码错误。
static const char string1[] = {0x4E , 0x1A , 0x52 , 0xA1, 0x6C, 0xE8, 0x51, 0x8C, 0x59, 0x31, 0x8D, 0x25, 0xFF, 0x0C, 0x5B, 0xC6, 0x78, 0x01, 0x8F , 0x93 , 0x51 , 0x65, 0x95, 0x19, 0x8B, 0xEF, 0x30, 0x02, 0x00, 0x00};
//注册失败,终端ID号输入错误。
static const char string2[] = {0x4E , 0x1A , 0x52 , 0xA1, 0x6C, 0xE8, 0x51, 0x8C, 0x59, 0x31, 0x8D, 0x25, 0xFF, 0x0C, 0x7E, 0xC8, 0x7A, 0xEF, 0x5E, 0x8F, 0x52, 0x17 , 0x53, 0xF7 , 0x53 , 0xF7 , 0x78 , 0x01, 0x8F, 0x93, 0x51, 0x65, 0x95, 0x19, 0x8B, 0xEF, 0x30, 0x02, 0x00, 0x00};
//注册失败,终端SIM卡号验证超时
static const char string3[] = {0x4E , 0x1A , 0x52 , 0xA1, 0x6C, 0xE8, 0x51, 0x8C, 0x59, 0x31, 0x8D, 0x25, 0xFF, 0x0C, 0x7E, 0xC8, 0x7A, 0xEF, 0x00, 0x53, 0x00, 0x49, 0x00, 0x4D, 0x53, 0x61, 0x53, 0xF7, 0x9A, 0x8C, 0x8B, 0xC1, 0x8D, 0x85, 0x65, 0xF6, 0x30, 0x02, 0x00, 0x00};
//注册失败,终端车主号码输入错误。
static const char string4[] = {0x4E , 0x1A , 0x52 , 0xA1, 0x6C, 0xE8, 0x51, 0x8C, 0x59, 0x31, 0x8D, 0x25, 0xFF, 0x0C, 0x7E, 0xC8, 0x7A, 0xEF, 0x8F, 0x66, 0x4E, 0x3B, 0x53, 0xF7 , 0x78, 0x01, 0x8F, 0x93, 0x51, 0x65, 0x95, 0x19, 0x8B, 0xEF, 0x30, 0x02, 0x0, 0x0};
//注册成功！正在登陆服务器，请稍后.....。
static const char string5[] = {0x6C, 0xE8, 0x51, 0x8C, 0x62, 0x10, 0x52, 0x9F, 0xFF, 0x01, 0x6B, 0x63, 0x57, 0x28, 0x76, 0x7B, 0x96, 0x46, 0x67, 0x0D, 0x52, 0xA1, 0x56, 0x68, 0xFF, 0x0C, 0x8B, 0xF7, 0x7A, 0x0D, 0x54, 0x0E, 0x00, 0x2E, 0x00, 0x2E, 0x00, 0x2E, 0x00, 0x2E, 0x00, 0x2E, 0x30, 0x02, 0x00, 0x00};
//终端SIM卡已换，请重新设置号码。
static const char string6[] = {0x7E, 0xC8, 0x7A, 0xEF, 0x00, 0x53, 0x00, 0x49, 0x00, 0x4D, 0x53, 0x61, 0x5D, 0xF2, 0x63, 0x62, 0xFF, 0x0C, 0x8B, 0xF7, 0x91, 0xCD, 0x65, 0xB0, 0x8B, 0xBE, 0x7F, 0x6E, 0x53, 0xF7, 0x78, 0x01, 0x30, 0x02, 0x00, 0x00};
//本机号码未设置,请先设定。
static const char string7[] = {0x67, 0x2C, 0x67, 0x3A, 0x53, 0xF7, 0x78, 0x01, 0x67, 0x2A, 0x8B, 0xBE, 0x7F, 0x6E, 0xFF, 0x0C, 0x8B, 0xF7, 0x51, 0x48, 0x8B, 0xBE, 0x5B, 0x9A, 0x30, 0x02, 0x00, 0x00};
//登陆被拒绝。
static const char string8[] = {0x76, 0x7B, 0x96, 0x46, 0x88, 0xAB, 0x62, 0xD2, 0x7E, 0xDD, 0x30, 0x02, 0x00, 0x00};
//车主现变更为：
static const char string9[] = {0x8F, 0x66, 0x4E, 0x3B, 0x73, 0xB0, 0x53, 0xD8, 0x66, 0xF4, 0x4E, 0x3A, 0xFF, 0x1A, 0x00, 0x00};

//监控状态已开启中，如需关闭，请直接回复CF。
static const char string10[] = {0x76 , 0xD1 , 0x63 , 0xA7 , 0x72 , 0xB6 , 0x60 , 0x01 , 0x5D , 0xF2 , 0x5F , 0x00 , 0x54 , 0x2F , 0x4E , 0x2D , 0xFF , 0x0C , 0x59 , 0x82 , 0x97 , 0x00 , 0x51 , 0x73 , 0x95 , 0xED , 0xFF , 0x0C , 0x8B , 0xF7 , 0x76 , 0xF4 , 0x63 , 0xA5 , 0x56 , 0xDE , 0x59 , 0x0D , 0x00 , 0x43 , 0x00 , 0x46 , 0x30 , 0x02 , 0x00, 0x00};

//监控已关闭，如需开启，请直接回复SF。
static const char string11[] = {0x76, 0xD1 , 0x63 , 0xA7 , 0x5D , 0xF2 , 0x51 , 0x73 , 0x95 , 0xED , 0xFF , 0x0C , 0x59 , 0x82 , 0x97 , 0x00 , 0x5F , 0x00 , 0x54 , 0x2F , 0xFF , 0x0C , 0x8B , 0xF7 , 0x76 , 0xF4 , 0x63 , 0xA5 , 0x56 , 0xDE , 0x59 , 0x0D , 0x00 , 0x53 , 0x00 , 0x46 , 0x30 , 0x02, 0x00, 0x00};
//位置解析失败，当前车辆GPS处于无效定位状态。
static const char string12[] = {0x4F , 0x4D , 0x7F , 0x6E , 0x89 , 0xE3 , 0x67 , 0x90 , 0x59 , 0x31 , 0x8D , 0x25 , 0xFF , 0x0C , 0x5F , 0x53 , 0x52 , 0x4D , 0x8F , 0x66 , 0x8F , 0x86 , 0x00 , 0x47 , 0x00 , 0x50 , 0x00 , 0x53 , 0x59 , 0x04 , 0x4E , 0x8E , 0x65 , 0xE0 , 0x65 , 0x48 , 0x5B , 0x9A , 0x4F , 0x4D , 0x72 , 0xB6 , 0x60 , 0x01 , 0x30 , 0x02, 0x00, 0x00};
//位置解析失败。
static const char string13[] = {0x4F, 0x4D, 0x7F, 0x6E, 0x89, 0xE3, 0x67, 0x90, 0x59, 0x31, 0x8D, 0x25, 0x30, 0x02, 0x00, 0x00};

//密码修改失败，原有密码输入有误。
static const char string14[] = {0x5B, 0xC6, 0x78, 0x01, 0x4F, 0xEE, 0x65, 0x39, 0x59, 0x31, 0x8D, 0x25, 0xFF, 0x0C, 0x53 , 0x9F , 0x67 , 0x09 , 0x5B, 0xC6, 0x78, 0x01, 0x8F, 0x93, 0x51, 0x65, 0x67, 0x09, 0x8B, 0xEF, 0x30, 0x02, 0x00, 0x00};
//密码修改时报，新密码输入有误。
static const char string15[] = {0x5B, 0xC6, 0x78, 0x01, 0x4F, 0xEE, 0x65, 0x39, 0x59, 0x31, 0x8D, 0x25,  0xFF, 0x0C, 0x65, 0xB0, 0x5B, 0xC6, 0x78, 0x01, 0x8F, 0x93, 0x51, 0x65, 0x67, 0x09, 0x8B, 0xEF, 0x30, 0x02, 0x00, 0x00};
//密码修改成功。
static const char string16[] = {0x5B, 0xC6, 0x78, 0x01, 0x4F, 0xEE, 0x65, 0x39, 0x62, 0x10, 0x52, 0x9F, 0x30, 0x02, 0x00, 0x00};
//，设置成功!
static const char string17[] = {0x8B, 0xBE, 0x7F, 0x6E, 0x62, 0x10, 0x52, 0x9F, 0xFF, 0x01, 0x00, 0x00};
//设置失败
static const char string18[] = {0x8B, 0xBE, 0x7F, 0x6E, 0x59, 0x31, 0x8D, 0x25, 0x30, 0x02, 0x00, 0x00};
//查询成功:
static const char string19[] = {0x67, 0xE5, 0x8B, 0xE2, 0x62, 0x10, 0x52, 0x9F, 0xFF, 0x1A, 0x00, 0x00};
//查询失败:
static const char string20[] = {0x67, 0xE5, 0x8B, 0xE2, 0x59, 0x31, 0x8D, 0x25, 0xFF, 0x1A, 0x00, 0x00};
//恢复出厂设置成功，请重新绑定车主。
static const char string21[] = {0x60, 0x62, 0x59, 0x0D, 0x51, 0xFA, 0x53, 0x82, 0x8B, 0xBE, 0x7F, 0x6E, 0x62, 0x10, 0x52, 0x9F, 0xFF, 0x0C, 0x8B, 0xF7, 0x91, 0xCD, 0x65, 0xB0, 0x7E, 0xD1, 0x5B, 0x9A, 0x8F, 0x66, 0x4E, 0x3B, 0x30, 0x02, 0x00, 0x00};
//登陆服务器成功，您可正常使用。
static const char string22[] = {0x76, 0x7B, 0x96, 0x46, 0x67, 0x0D, 0x52, 0xA1, 0x56, 0x68, 0x62, 0x10, 0x52, 0x9F, 0xFF, 0x0C, 0x60, 0xA8, 0x53, 0xEF, 0x6B, 0x63, 0x5E, 0x38, 0x4F, 0x7F, 0x75, 0x28, 0x30, 0x02, 0x00, 0x00};

//重启成功。
static const char string23[] = {0x91 , 0xCD , 0x54 , 0x2F , 0x62 , 0x10 , 0x52 , 0x9F , 0x30 , 0x02 , 0x00, 0x00};

//车主
static const char string24[] = {0x8F , 0x66 , 0x4E , 0x3B, 0x00, 0x00};
//现在已经成功变更为
static const char string25[] = { 0x73, 0xB0, 0x57, 0x28, 0x5D, 0xF2, 0x7E, 0xCF, 0x62, 0x10, 0x52, 0x9F, 0x53, 0xD8, 0x66, 0xF4, 0x4E, 0x3A, 0x00, 0x00};

//车载电源移除
static char string26[] = {0x8F, 0x66, 0x8F, 0x7D, 0x75, 0x35, 0x6E, 0x90, 0x79, 0xFB, 0x96, 0x64, 0x00, 0x00};
//设防状态
static char string27[] = {0x8B, 0xBE, 0x96, 0x32, 0x72, 0xB6, 0x60, 0x01, 0x00, 0x00};
//撤防状态
static char string28[] = {0x64, 0xA4, 0x96, 0x32, 0x72, 0xB6, 0x60, 0x01, 0x00, 0x00};
//休眠节电模式
static char string29[] = {0x4F, 0x11, 0x77, 0x20, 0x82, 0x82, 0x75, 0x35, 0x6A, 0x21, 0x5F, 0x0F, 0x00, 0x00};

const char string30[] = {0x30, 0x10, 0x79 , 0xFB , 0x52 , 0xA8 , 0x8F , 0x66 , 0x53 , 0x6B , 0x58 , 0xEB , 0x30 , 0x11, 0x00, 0x00};
static const char string31[] = {0x8B, 0xBE, 0x7F, 0x6E, 0x53, 0xC2, 0x65, 0x70, 0x95, 0x19, 0x8B, 0xEF, 0x00, 0x00};

typedef struct
{
    nvram_ef_device_setting_struct  Local;
    char   sms_num[TRACK_DEFINE_PHONE_NUMBER_LEN_MAX];       //本机SIM卡号码
} device_setting_struct;
static device_setting_struct setting = {0};


void set_nvram_device_setting(void)
{
    Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
}
void track_cmd_zc_setting(void *nicai)
{
    int opertion = (int)nicai;
    char rspmsg[200] = {0};
    sms_msg_struct ssms = {0};

    LOGD(L_CMD, L_V5, "");

    if (opertion)
    {
        /*if((strlen((char *)G_importance_parameter_data.login.host_num) != 0) && \
                (strcmp((char *)G_importance_parameter_data.login.host_num, (char *)setting.Local.host_num) != 0))
        {
            track_send_sms_comm(4, (kal_uint8 *)G_importance_parameter_data.login.host_num, (kal_uint8 *)setting.Local.host_num);
        }*/
        memset(G_importance_parameter_data.login.SequenceNumber, 0, sizeof(G_importance_parameter_data.login.SequenceNumber));
        memset(G_importance_parameter_data.login.owner_num, 0, sizeof(G_importance_parameter_data.login.owner_num));
        memset(G_importance_parameter_data.login.host_num, 0, sizeof(G_importance_parameter_data.login.host_num));

        memcpy(G_importance_parameter_data.login.SequenceNumber, setting.Local.SequenceNumber, 18);
        memcpy(G_importance_parameter_data.login.owner_num, setting.Local.owner_num, TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
        memcpy(G_importance_parameter_data.login.host_num, setting.Local.host_num, TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
        Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);

        //kal_wsprintf((WCHAR *)rspmsg, "%w", string5);
        //track_soc_close();
        //flag_zc_reconnect_success_msg=KAL_TRUE;
        track_os_intoTaskMsgQueueExt(track_soc_gprs_reconnect, (void*)2);//重新建立连接
    }
    else if (0 == opertion)
    {
        kal_wsprintf((WCHAR *)rspmsg, "%w", string3);
    }

    app_ucs2_strcat((char *)rspmsg, string30);
    ssms.ctxlen = app_ucs2_strlen((kal_int8 *)rspmsg) * 2;

    LOGD(L_CMD, L_V1, "%d", ssms.ctxlen);

    if (ssms.ctxlen)
    {
        if (strlen(G_importance_parameter_data.login.host_num) == 0)
        {
            ssms.numbers[0] = (char *)setting.sms_num;
        }
        else
        {
            ssms.numbers[0] = (char *)setting.Local.host_num;
        }
        if (strlen(ssms.numbers[0]) && ssms.ctxlen)
        {
            //  ssms.ctxlen += 2;
            ssms.codetype = KAL_FALSE;
            ssms.ctx = rspmsg;
            ssms.nocount = 1;
            //track_sms_sender(&ssms);
        }
    }
    memset(&setting, 0, sizeof(device_setting_struct));
}
static void cmd_ZC(CMD_DATA_STRUCT * Cmds)
{
    LOGD(L_CMD, L_V4, "num=%s;passwd=%s", Cmds->return_sms.ph_num, G_importance_parameter_data.login.password);
    LOGD(L_CMD, L_V4, "SIM ID=%s;", G_importance_parameter_data.login.SequenceNumber);
    LOGD(L_CMD, L_V4, "HOST NUM=%s;", G_importance_parameter_data.login.host_num);
    LOGD(L_CMD, L_V4, "SMS SIM ID=%s;", Cmds->pars[CM_Par1]);
    LOGD(L_CMD, L_V4, "SMS PASSWORD=%s;", Cmds->pars[CM_Par2]);
    LOGD(L_CMD, L_V4, "SMS SIM=%s;", Cmds->pars[CM_Par3]);
    LOGD(L_CMD, L_V4, "SMS HOST NUM=%s;", Cmds->pars[CM_Par4]);
    memset(Cmds->rsp_msg, 0, CM_PARAM_LONG_LEN_MAX);
    if (Cmds->part == 0)
    {
        sprintf(Cmds->rsp_msg, "passwd=%s;SIM ID=%s;HOST NUM=%s;OWNER NUM=%s", G_importance_parameter_data.login.password,
                G_importance_parameter_data.login.SequenceNumber,
                G_importance_parameter_data.login.host_num, G_importance_parameter_data.login.owner_num);
        return;
    }
    //Cmds->sms_type = GSM_UCS2;
    /*
        if(Cmds->part != 4)
        {
            kal_wsprintf((WCHAR *)Cmds->rsp_msg, "%w", string31);
            return ;
        }
        if(strlen((char*)G_importance_parameter_data.login.SequenceNumber) != 0 &&
            ((strlen((S8 *)Cmds->pars[CM_Par1]) != 18) ||
            (strcmp((char *) Cmds->pars[CM_Par1] , (char*)G_importance_parameter_data.login.SequenceNumber) != 0))) //device id
        {
            kal_wsprintf((WCHAR *)Cmds->rsp_msg, "%w", string2);
            return;
        }
        else if((strlen((S8 *)Cmds->pars[CM_Par2]) != 6) ||
            ((strcmp((char *) Cmds->pars[CM_Par2] , (char *) G_importance_parameter_data.login.password) != 0) &&
            (strcmp((char *) Cmds->pars[CM_Par2] , (char *) G_parameter.Permission.superPassword) != 0))) //password
        {
            kal_wsprintf((WCHAR *)Cmds->rsp_msg, "%w", string1);
            return;
        }
        else if(track_check_number(2, (char *)Cmds->pars[CM_Par3], NULL) == 0) //sim number
        {
            kal_wsprintf((WCHAR *)Cmds->rsp_msg, "%w", string3);
            return;
        }
        else if(Cmds->cm_type == CM_SMS &&
            track_check_number(1, (char *)Cmds->pars[CM_Par4], (char *)Cmds->ph_num) == 0) //host number
        {
            kal_wsprintf((WCHAR *)Cmds->rsp_msg, "%w", string4);
            return;
        }
    */
    memset(&setting, 0, sizeof(device_setting_struct));

    memcpy(setting.Local.SequenceNumber, Cmds->pars[CM_Par1], 18);
    memcpy(setting.Local.owner_num, Cmds->pars[CM_Par3], TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
    memcpy(setting.Local.host_num, Cmds->pars[CM_Par4], TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
    memcpy(setting.sms_num, Cmds->return_sms.ph_num, TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);

    //send_sms_valid_owner_num((char *)setting.Local.owner_num);
    track_cmd_zc_setting((void*) 1);
    //track_start_timer(TRACK_ZC_TIMEOUT_TIMER_ID, 65 * 1000, track_cmd_zc_setting, (void *) 0);
    sprintf(Cmds->rsp_msg, "OK");
}
void get_nvram_device_setting(char *msg)
{
    if (msg != NULL)
    {
        sprintf(msg, "MachineType=%d,PV=%s,SN=%s", G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber);
    }
}
int parseInt(char * str)
{
    static int value = 0;
    int radix = 0;
    value = 0;

    if (*str == 0)return -1;
    if (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X'))
    {
        radix = 16;
        str += 2;
    }
    else
        radix = 10;

    while (*str)
    {
        if (radix == 16)
        {
            if (*str >= '0' && *str <= '9')
                value = value * radix + *str - '0';
            else if ((*str >= 'a' || *str >= 'A') && (*str <= 'f' || *str <= 'F'))
                value = value * radix + (*str | 0x20) - 'a' + 10;
            else
                value = -1;
            // value = value * radix + *str - 'a' + 10; // 也没问题啊
        }
        else
        {
            if (*str >= '0' && *str <= '9')
                value = value * radix + *str - '0';
            else
                value = -1;
        }

        str++;
    }

    return value;
}

void cmd_set_device_setting(CMD_DATA_STRUCT * Cmds)
{
    kal_bool formaterror = KAL_FALSE;
    kal_int16 temp = 0;

    if (Cmds->part == 0)
    {
        get_nvram_device_setting(Cmds->rsp_msg);
        return;
    }
    if (Cmds->part != 3)
    {
        formaterror = KAL_TRUE;
    }
    if (Cmds->part > 0 && strlen(Cmds->pars[CM_Par1]) > 0)
    {
        if ((temp = parseInt(Cmds->pars[CM_Par1])) >= 0 && temp < 0xFFFF)
        {
            G_importance_parameter_data.login.MachineType = temp;
        }
        else
        {
            formaterror = KAL_TRUE;
        }
    }

    if (formaterror == KAL_FALSE && Cmds->part > 1)
    {
        temp = strlen(Cmds->pars[CM_Par2]);
        if (temp > 0 && temp < 8)
        {
            memcpy(G_importance_parameter_data.login.ProtocolVersion, Cmds->pars[CM_Par2], 8);
        }
        else if (temp >= 8)
        {
            formaterror = KAL_TRUE;
        }
    }

    if (formaterror == KAL_FALSE && Cmds->part > 2)
    {
        temp = strlen(Cmds->pars[CM_Par3]);

        if (temp > 0 && temp < 19)
        {
            memcpy(G_importance_parameter_data.login.SequenceNumber, Cmds->pars[CM_Par3], 18);
        }
        else if (temp > 18)
        {
            formaterror = KAL_TRUE;
        }
    }

    sprintf(Cmds->rsp_msg, (formaterror ? "ERROR 100" : "OK"));
    if (formaterror == KAL_FALSE)
    {
        set_nvram_device_setting();
    }
    track_os_intoTaskMsgQueueExt(track_soc_gprs_reconnect, (void*)4);//重新建立连接
}

#endif
#if defined(__EXT_VBAT_ADC__)
void cmd_adc_bc(CMD_DATA_STRUCT * Cmds)
{
    kal_int32 parm = 0;
    kal_int32 parm1 = 0;
    float volt_plus = 0;
    kal_int32 vol = 0;
    kal_uint8 buffer[10] = NULL;

    //extern kal_int32 track_drv_get_external_voltage_status(void);
    if (Cmds->part > 2)
    {
        sprintf(Cmds->rsp_msg, "ERROR:part %d", Cmds->part);
        return;
    }
    if (Cmds->part == 0)
    {
        volt_plus = G_parameter.adc4_offset;
        sprintf(Cmds->rsp_msg, "ADC4 Offset:%0.2fV", volt_plus / 1000000);
        return;
    }
    if (Cmds->part >= 1 && !strcmp(Cmds->pars[1], "OFF"))
    {
        volt_plus = 0;
    }
    else if (Cmds->part >= 1 && !strcmp(Cmds->pars[1], "A"))
    {
        //输入为补偿差值（单位为V）
        if (strlen(Cmds->pars[2]) && track_fun_check_str_is_number(3, Cmds->pars[2]) > 0)
        {
            volt_plus = atof(Cmds->pars[2]) * 1000000;
        }
        else
        {
            sprintf(Cmds->rsp_msg, "BC_D Fail: %s", Cmds->pars[2]);
            return;
        }
    }
    else if (Cmds->part >= 1 && !strcmp(Cmds->pars[1], "B"))
    {
        //输入为补偿差值（单位为微伏）
        LOGD(L_CMD, L_V4, "offset=%d;", atoi(Cmds->pars[2]));
        if (strlen(Cmds->pars[2]) && track_fun_check_str_is_number(3, Cmds->pars[2]) > 0)
        {
            volt_plus = atoi(Cmds->pars[2]);       // > 100000
        }
        else
        {
            sprintf(Cmds->rsp_msg, "BC_D Fail: %0.2fV", volt_plus / 1000000);
            return;
        }
    }
    else
    {
        //参数为当前正确的电压，自动计算补偿差值
        if (Cmds->part >= 1 && strlen(Cmds->pars[1]))
        {
            parm = (atof(Cmds->pars[1]) * 1000000);
        }
        if (strlen(Cmds->pars[2]))
        {
            parm1 = (atof(Cmds->pars[2]) * 1000000);
        }
        LOGC(L_CMD, L_V5, "BC parm: %d,parm1:%d", parm, parm1);
        vol = track_drv_get_external_voltage_status() - G_parameter.adc4_offset; //得出终端实际测量值
        LOGC(L_CMD, L_V5, "BC_A VOL: %d", vol);
        if (vol <= 0)
        {
            sprintf(Cmds->rsp_msg, "BC_D Fail: no charge");
            return;
        }
        volt_plus = parm - vol;
        if (strlen(Cmds->pars[2]))
        {
            LOGC(L_CMD, L_V5, "BC volt_plus: %d,parm1:%d", volt_plus, parm1);
            if (volt_plus > parm1 || volt_plus < (-parm1))
            {
                sprintf(Cmds->rsp_msg, "BC_D Fail: %0.2fV", volt_plus / 1000000);
                return;
            }
        }
        else
        {
            if ((volt_plus > 350000) || (volt_plus < (-350000)))
            {
                sprintf(Cmds->rsp_msg, "BC_D Fail: %0.2fV", volt_plus / 1000000);
                return;
            }

        }
    }
    G_parameter.adc4_offset = volt_plus;
    vol = track_drv_get_external_voltage_status();
    sprintf(Cmds->rsp_msg, "BC_B VOL: %d", vol);
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    //track_os_intoTaskMsgQueue(track_cust_write_sim_backup_for_parameter);
}

#endif /* __EXT_VBAT_ADC__ */


#if defined (__GT02__)
/*单GT02使用，关起来节约空间       --    chengjun  2017-04-13*/
static void cmd_SYS_VERSION(CMD_DATA_STRUCT *cmd)
{
    char *p;
    int len;
    kal_uint8 chip_rid[16];
    kal_uint16 CRC, CRC_IN = 0;
    char version;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "version:%d", G_importance_parameter_data.app_version.version);
        return;
    }
    else if (!strcmp(cmd->pars[1], "A007700"))
    {
        memcpy(chip_rid, track_drv_get_chip_id(), 16);
        CRC = GetCrc16(chip_rid, 16);
        CRC_IN = track_fun_atoi(cmd->pars[2]);
        sprintf(cmd->rsp_msg, "CRC=%d", CRC);
        return;
    }
    else if (cmd->part > 2)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }

    memcpy(chip_rid, track_drv_get_chip_id(), 16);
    CRC = GetCrc16(chip_rid, 16);
    CRC_IN = track_fun_atoi(cmd->pars[2]);
    if (CRC != CRC_IN)
    {
        sprintf(cmd->rsp_msg, "Error:password error!");
        return;
    }
    G_parameter.agps.sw = 1;

#if defined(__NT31__)
    G_parameter.fake_cell_enable=0;//NT32S防伪基站报警都关闭LYL 2017.08.16
#else
    G_parameter.fake_cell_enable=1
#endif

    version = track_fun_atoi(cmd->pars[1]);
    if (version == 1)
    {
        G_parameter.lang = 1;
        //   G_parameter.sensor_gps_work = 0;
        memcpy(&G_importance_parameter_data, &NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT_SERVER_GUMI, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);


    }
    else if (version == 2)
    {

        G_parameter.lang = 1;
        //  G_parameter.sensor_gps_work = 0;
        memcpy(&G_importance_parameter_data, &NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT_GUMI_GENERAL, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);


    }
    else if (version == 3)
    {
        G_parameter.agps.sw = 0;
        G_parameter.lang = 0;
        G_parameter.fake_cell_enable=0;
        //G_parameter.sensor_gps_work = 5;
        memcpy(&G_importance_parameter_data, &NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT_SERVER_COOACCESS, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);

    }
    else if (version == 4)
    {
        G_parameter.lang = 1;
        // G_parameter.sensor_gps_work = 5;
        memcpy(&G_importance_parameter_data, &NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT_SERVER_TUQIANG, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);

    }
    else if (version == 0)
    {
        memcpy(&G_parameter, OTA_NVRAM_EF_PARAMETER_DEFAULT, NVRAM_EF_PARAMETER_SIZE);
        memcpy(&G_importance_parameter_data, OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Value only in from 0 to 4!");
        return;
    }
    if (G_parameter.lang == 1)
    {
        memset(G_parameter.url, 0x00, 100);
        memcpy(&G_parameter.url, "http://ditu.google.cn/maps?q=", strlen("http://ditu.google.cn/maps?q="));
    }
    else
    {
        memset(G_parameter.url, 0x00, 100);
        memcpy(&G_parameter.url, "http://maps.google.com/maps?q=", strlen("http://maps.google.com/maps?q="));
    }
    memcpy(G_importance_parameter_data.build_date, OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT->build_date, 15);
    memcpy(G_importance_parameter_data.build_time, OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT->build_time, 15);
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
    sprintf(cmd->rsp_msg, "OK");
    track_set_server_ip_status(0);
    track_os_intoTaskMsgQueueExt(track_soc_gprs_reconnect, (void*)22);//修改域名后，重新建立连接

    //track_os_intoTaskMsgQueue(track_cust_write_sim_backup_for_parameter);
}
#endif /* __GT02__ */

#if defined(__GT530__)
void cmd_mifi(CMD_DATA_STRUCT *cmd)
{
    int value, len, str_len;
    char temp[50];
    char newpassword[20] = {0}, oldpassword[20] = {0};
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        track_drv_encode_exmode(0x77, 0x0101, 0x91, "WIFION", 6);
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        track_drv_encode_exmode(0x77, 0x0101, 0x91, "WIFIOFF", 7);
    }
    else if (!strcmp(cmd->pars[1], "RECOVER"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        track_drv_encode_exmode(0x77, 0x0101, 0x91, "WIFIRECOVER", 11);
    }
    else if (!strcmp(cmd->pars[1], "QUERY"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        track_drv_encode_exmode(0x77, 0x0101, 0x91, "WIFIQUERY", 9);

    }
    else if (!strcmp(cmd->pars[1], "PASSWORD"))
    {
        if (cmd->part != 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        len = strlen(cmd->pars[2]);
        if (len == 0)
        {
            sprintf(cmd->rsp_msg, "Error: Password can not be empty!");
            return;
        }
        if (len > 19)
        {
            sprintf(cmd->rsp_msg, "Error: Character length should be less than 20 characters!");
            return;
        }

        memcpy(newpassword, cmd->rcv_msg_source + (cmd->pars[2] - cmd->rcv_msg), len);
        if (track_fun_check_str_is_number_and_string(0, newpassword) <= 0)
        {
            sprintf(cmd->rsp_msg, "Error: password characters can only be numbers.");
            return;
        }
        str_len = snprintf(temp, 50, "WIFIPASSWORD,666666,%s", newpassword);
        track_drv_encode_exmode(0x77, 0x0101, 0x91, temp, str_len);
    }
    else if (!strcmp(cmd->pars[1], "NAME"))
    {
        if (cmd->part != 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        len = strlen(cmd->pars[2]);
        if (len == 0)
        {
            sprintf(cmd->rsp_msg, "Error: Wifiname can not be empty!");
            return;
        }
        if (len > 29)
        {
            sprintf(cmd->rsp_msg, "Error: Character length should be less than 30 characters!");
            return;
        }
        str_len = snprintf(temp, 50, "WIFINAME,%s", cmd->pars[2]);
        track_drv_encode_exmode(0x77, 0x0101, 0x91, temp, str_len);
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    sprintf(cmd->rsp_msg, "OK");


}
void cmd_data(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part != 3)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    sprintf(cmd->rsp_msg, "OK");
}
#endif


#if 0
static void cmd_test_433_on_control(kal_uint8 mode)
{
    LOGD(L_CMD, L_V5, "mode =%d", mode);
    switch (mode)
    {
        case 1:
        case 2:
        case 3:
            track_drv_gpio_set(1, AP_433_GPIO);
            if (mode == 1)
            {
                track_start_timer(TRACK_CUST_433_CONTROL_TIMER_ID, 1000, cmd_test_433_on_control, (void*)4);
            }
            else if (mode == 2)
            {
                track_start_timer(TRACK_CUST_433_CONTROL_TIMER_ID, 4000, cmd_test_433_on_control, (void*)4);
            }
            else if (mode == 3)
            {
                track_start_timer(TRACK_CUST_433_CONTROL_TIMER_ID, 7000, cmd_test_433_on_control, (void*)4);
            }
            break;
        case 4:
            GPIO_ModeSetup(CTRL_433_GPIO, 0);
            GPIO_InitIO(1/*OUTPUT*/, CTRL_433_GPIO);
            track_drv_gpio_set(0, CTRL_433_GPIO);
            break;
        default:
            break;

    }
}

static void cmd_test_433_on(kal_uint8 mode)
{
    LOGD(L_CMD, L_V5, "mode=%d", mode);
    //初始化GPIO口
    GPIO_ModeSetup(CTRL_433_GPIO, 0);
    GPIO_InitIO(1/*OUTPUT*/, CTRL_433_GPIO);
    GPIO_ModeSetup(AP_433_GPIO, 0);
    GPIO_InitIO(1/*OUTPUT*/, AP_433_GPIO);
    switch (mode)
    {
        case 1://ACC OFF
            LOGS("@433 acc off");
            if ((track_cust_status_acc() == 1 && G_realtime_data.defense_mode == 1) || (G_realtime_data.oil_lock_status == 1 && track_cust_status_acc() == 1))
            {
                track_drv_gpio_set(1, CTRL_433_GPIO);
                track_drv_gpio_set(1, AP_433_GPIO);
                track_start_timer(TRACK_CUST_433_CONTROL_TIMER_ID, 60 * 1000, cmd_test_433_on_control, (void*)4);
            }
            else
            {
                track_drv_gpio_set(0, CTRL_433_GPIO);
            }
            break;
        case 2://远程喇叭设防
            LOGS("@433 defence on");
            track_drv_gpio_set(1, CTRL_433_GPIO);
            track_drv_gpio_set(0, AP_433_GPIO);
            track_start_timer(TRACK_CUST_433_CONTROL_TIMER_ID, 100, cmd_test_433_on_control, (void*)1);
            break;
        case 3://远程喇叭寻车
            LOGS("@433 search");
            track_drv_gpio_set(1, CTRL_433_GPIO);
            track_drv_gpio_set(0, AP_433_GPIO);
            track_start_timer(TRACK_CUST_433_CONTROL_TIMER_ID, 100, cmd_test_433_on_control, (void*)2);
            break;
        case 4://远程喇叭报警
            LOGS("@433 alarm");
            track_drv_gpio_set(1, CTRL_433_GPIO);
            track_drv_gpio_set(0, AP_433_GPIO);
            track_start_timer(TRACK_CUST_433_CONTROL_TIMER_ID, 100, cmd_test_433_on_control, (void*)3);
            break;
        case 5://恢复断油电
            LOGS("@433  defence off");
            track_drv_gpio_set(1, CTRL_433_GPIO);
            track_drv_gpio_set(0, AP_433_GPIO);
            break;
        default:
            LOGD(L_CMD, L_V5, "@99");
            break;
    }
}


static void cmd_test_433(CMD_DATA_STRUCT *cmd)
{
    kal_uint8 mode = 0;
    LOGD(L_CMD, L_V5, "");
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:ON:(1:acc off;2:defence on;3:search;4:alarm;5:defence off)", cmd->pars[0]);
        return;
    }
    gpio_433M(99);

    if (cmd->part > 2)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }

    if (!strcmp(cmd->pars[1], "ON"))
    {
        mode = track_fun_atoi(cmd->pars[2]);
        LOGD(L_CMD, L_V5, "mode=%d", mode);
        cmd_test_433_on(mode);
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        //初始化GPIO口
        GPIO_ModeSetup(CTRL_433_GPIO, 0);
        GPIO_InitIO(1/*OUTPUT*/, CTRL_433_GPIO);
        GPIO_ModeSetup(AP_433_GPIO, 0);
        GPIO_InitIO(1/*OUTPUT*/, AP_433_GPIO);
        track_drv_gpio_set(0, CTRL_433_GPIO);
        track_drv_gpio_set(0, AP_433_GPIO);
        gpio_433M(98);
    }

}

#endif
#if defined(__433M__)
static void cmd_433_spks(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part > 0)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    LOGD(L_CMD, L_V5, "");
    track_cust_433_control(0);
    track_cust_433_control(5);
    sprintf(cmd->rsp_msg, "OK");
}
static void cmd_433_spkdef(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part > 0)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    LOGD(L_CMD, L_V5, "");
    track_cust_433_control(0);
    track_cust_433_control(3);
    sprintf(cmd->rsp_msg, "OK");
}

static void cmd_433_spkalarm(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part > 0)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    LOGD(L_CMD, L_V5, "");
    track_cust_433_control(0);
    track_cust_433_control(6);
    sprintf(cmd->rsp_msg, "OK");
}
static void cmd_433_spkoff(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part > 0)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    LOGD(L_CMD, L_V5, "");
    track_cust_433_control(0);
    track_cust_433_control(2);
    sprintf(cmd->rsp_msg, "OK");
}
#endif

#if defined(__NT36__)||defined(__GT370__)||defined(__GT380__)||defined(__GT310__)||defined(__GT740__)||defined(__GT420D__)
//nc
#else
void  cmd_DOOR(CMD_DATA_STRUCT *cmd)
{
    int value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d",  cmd->pars[0], G_parameter.door_alarm.door_status);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    value = track_fun_atoi(cmd->pars[1]);
    if (value < 0 || value > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Range error. ");
        return;
    }
    G_parameter.door_alarm.door_status = value;

#if defined (__NT37__)
    G_parameter.door_alarm.alarm_enable=1;
#endif

    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}
void track_find_car(void)
{
    static kal_uint8 cnt = 0;
    if (cnt < 12)
    {
        cnt++;
        if (cnt % 2 == 0)
        {
            track_drv_gpio_out1(0);
        }
        else
        {
            track_drv_gpio_out1(1);
        }
        tr_start_timer(TRACK_CUST_FIND_CAR_TIMER_ID, 1000, track_find_car);
    }
    else
    {
        track_drv_gpio_out1(0);
        cnt = 0;
    }

}
void cmd_FIND(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "OK!");
        track_find_car();
        return;
    }
}

/******************************************************************************
 *  Function    -  cmd_door_alarm
 *
 *  Purpose     -  报警
 *
 *  Description -
        DOORALM,<A>[,M]#
            A=ON/OFF；默认值：OFF
            M=0/1；报警上报方式，0 仅GPRS，1 SMS+GPRS；默认值：0
        DOOR,OFF#
            关闭震动报警
        DOOR#
            查询已设置的参数
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_dooralm(CMD_DATA_STRUCT *cmd)
{
    nvram_door_alarm_struct  alarm = {0};
    kal_uint8 defences = 0;
    kal_int8 sw_tmp = 0;

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s: %s, %d", cmd->pars[0],
                ON_OFF(G_parameter.door_alarm.sw), G_parameter.door_alarm.alarm_type);
        return;
    }
    sw_tmp = G_parameter.door_alarm.sw;
    memcpy(&alarm, &G_parameter.door_alarm, sizeof(nvram_door_alarm_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part < 1 || cmd->part > 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 1;
        if (cmd->part >= 2)
        {
            alarm.alarm_type = track_fun_atoi(cmd->pars[2]);
            if (alarm.alarm_type != 0 && alarm.alarm_type != 1 && alarm.alarm_type != 2)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1/2)");
                return;
            }
        }

#if defined (__NT37__)
        alarm.alarm_enable=1;
#endif

    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.door_alarm, &alarm, sizeof(nvram_door_alarm_struct));
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    sprintf(cmd->rsp_msg, "OK!");
}

#endif

static void cmd_mserveri(CMD_DATA_STRUCT *cmd)
{
    int value, plen;
    nvram_server_addr_struct server = {0};
    char domain[TRACK_MAX_URL_SIZE] = {0};
    U8 i = 0;
    if (strstr(cmd->pars[0], "MSERVER1"))
    {
        i = 0;
    }
    else if (strstr(cmd->pars[0], "MSERVER2"))
    {
        i = 1;
    }
    else if (strstr(cmd->pars[0], "MSERVER3"))
    {
        i = 2;
    }
    else if (strstr(cmd->pars[0], "MSERVER4"))
    {
        i = 3;
    }
    if (cmd->part == 0)
    {
        if (G_importance_parameter_data.mserver2[i].conecttype == 0)
        {
            sprintf(cmd->rsp_msg, "%s,0,%d.%d.%d.%d,%d,%d", cmd->pars[0],
                    G_importance_parameter_data.mserver2[i].server_ip[0], G_importance_parameter_data.mserver2[i].server_ip[1],
                    G_importance_parameter_data.mserver2[i].server_ip[2], G_importance_parameter_data.mserver2[i].server_ip[3],
                    G_importance_parameter_data.mserver2[i].server_port, G_importance_parameter_data.mserver2[i].soc_type);
        }
        else
        {
            sprintf(cmd->rsp_msg, "%s,%d,%s,%d,%d", cmd->pars[0],
                    G_importance_parameter_data.mserver2[i].conecttype,
                    track_domain_decode(G_importance_parameter_data.mserver2[i].url),
                    G_importance_parameter_data.mserver2[i].server_port,
                    G_importance_parameter_data.mserver2[i].soc_type);
        }
        return;
    }
    if (cmd->part != 3 && cmd->part != 4 && cmd->part != 5)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (G_importance_parameter_data.mserver2[i].lock != 0)
    {
#if defined(__GERMANY__)
        sprintf(cmd->rsp_msg, "Fehler! Die Plattform Adresse wird nicht erkannt!");
#else
        sprintf(cmd->rsp_msg, "Error: The platform addresses are Locked modify!");
#endif
        return;
    }
    memset(cmd->rsp_msg, 0, sizeof(cmd->rsp_msg));

    memcpy(&server, &G_importance_parameter_data.mserver2[i], sizeof(nvram_server_addr_struct));

    value = track_fun_atoi(cmd->pars[CM_Par1]);
    if (value != 0 && value != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0/1)");
        return;
    }
    server.conecttype = value;

    plen = strlen(cmd->pars[CM_Par2]);
    if (plen <= 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 2 is empty!");
        return;
    }
    if (plen >= 98)
    {
        sprintf(cmd->rsp_msg, "Error: Parameters 2 characters too long!");
        return;
    }
    if (track_fun_check_ip(cmd->pars[CM_Par2]))
    {
        if (server.conecttype != 0)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 domain is not formatted correctly!");
            return;
        }
        memset(server.server_ip, 0, 4);
        if (!track_fun_asciiIP2Hex(cmd->pars[CM_Par2], (void*)&server.server_ip[0]))
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 is not a valid IP address format!");
            return;
        }
        server.url[0] = 0;
    }
    else
    {
        if (server.conecttype != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 is not a valid IP address format!");
            return;
        }
        if (strstr(cmd->pars[CM_Par2], "http://"))
        {
            sprintf(cmd->rsp_msg, "Error: The domain name does not include \"http://\"!");
            return;
        }
        {
            memcpy(domain, cmd->rcv_msg_source + (cmd->pars[CM_Par2] - cmd->rcv_msg), plen);
            track_domain_encode(domain, server.url);
            memset(server.server_ip, 0, 4);
        }
    }

    if (!track_fun_check_data_is_port(cmd->pars[CM_Par3]))
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 3 (0~65535)");
        return;
    }
    server.server_port = track_fun_string2U16(cmd->pars[CM_Par3]);

    if (cmd->part == 4)
    {
        value = track_fun_atoi(cmd->pars[CM_Par4]);
        if (value != 0 && value != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 4 (0/1)");
            return;
        }
    }
    server.soc_type = 0; // 当前只支持TCP
    memcpy(&G_importance_parameter_data.mserver2[i], &server, sizeof(nvram_server_addr_struct));
    Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
    sprintf(cmd->rsp_msg, "OK");
}

static void cmd_bat(CMD_DATA_STRUCT *cmd)
{
    charge_measure_struct charge_measure = {0};
    kal_int32 BatTemp = 0;
    float charger = 0;
#if defined(__GT420D__)
    kal_uint16 value = 0;
#endif
    if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
#if !defined(__GT420D__)
    track_bmt_get_ADC_result(&charge_measure);
    charger = track_cust_status_charger();
    if (charger > 1) charger = 1;
#else
    value = track_cust_MCUrefer_adc_data(99);
    charger = track_cust_refer_charge_data(99);
#endif
    //BatTemp = (float)charge_measure.bmt_vTemp/1000;
#if !defined(__GT420D__)
    sprintf(cmd->rsp_msg, "battery temp:%f ,VBAT:%f ,charge:%s", charge_measure.bmt_vTemp / 1000.00, charge_measure.bmt_vBat, ON_OFF(charger));
#else
    sprintf(cmd->rsp_msg, "VBAT:%0.2f ,charge:%s", ((float)value)/100, ON_OFF(charger));
#endif
}

static void cms_mserver(CMD_DATA_STRUCT *cmd)
{
    U8 i = 0;
    char tmp[100] = {0};
    if (cmd->part == 0)
    {
        for (i = 0; i < 4; i++)
        {
            if (G_importance_parameter_data.mserver2[i].conecttype == 0)
            {
                sprintf(tmp, "[%d]:0,%d.%d.%d.%d,%d,%d;", i + 1,
                        G_importance_parameter_data.mserver2[i].server_ip[0], G_importance_parameter_data.mserver2[i].server_ip[1],
                        G_importance_parameter_data.mserver2[i].server_ip[2], G_importance_parameter_data.mserver2[i].server_ip[3],
                        G_importance_parameter_data.mserver2[i].server_port, G_importance_parameter_data.mserver2[i].soc_type);
            }
            else
            {
                sprintf(tmp, "[%d]:%d,%s,%d,%d", i + 1,
                        G_importance_parameter_data.mserver2[i].conecttype,
                        track_domain_decode(G_importance_parameter_data.mserver2[i].url),
                        G_importance_parameter_data.mserver2[i].server_port,
                        G_importance_parameter_data.mserver2[i].soc_type);
            }
            strcat(cmd->rsp_msg, tmp);
        }
        return;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error! %d", cmd->part);
        return;
    }
}

#if defined __CUSTOM_WIFI_FEATURES_SWITCH__
static void cms_wificmd(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "Error!");
        return;
    }
    if (!strcmp(cmd->pars[1], "ON"))
    {
        track_drv_wifi_cmd(1);
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        track_drv_wifi_cmd(0);
    }
    else if (!strcmp(cmd->pars[1], "SCAN"))
    {
        track_drv_wifi_cmd(2);
    }
    sprintf(cmd->rsp_msg, "OK!");

}
#endif

kal_uint32 track_get_ft_timer(void)
{
    return ft_timer;
}
kal_uint32 track_get_ft_work_timer(void)
{
    return ft_work_timer;
}
static void cmd_FT(CMD_DATA_STRUCT * cmd)
{
    int value, timer1, timer2;
    if (G_parameter.group_package.sw == 0)
    {
        sprintf(cmd->rsp_msg, "Error: batch:OFF");
        return;
    }
    if (cmd->part > 0)
    {
        if (cmd->part == 2)
        {

            value  = track_fun_atoi(cmd->pars[1]);
            if (value < 1 || value > 18000)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 1 (1 to 18000)");
                return;
            }
            ft_timer = value;

            value  = track_fun_atoi(cmd->pars[2]);
            ft_work_timer = value;
            if (value < 10 || value > 18000)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2(10 to 18000)");
                return;
            }

        }
        else if (cmd->part == 1)
        {
            value  = track_fun_atoi(cmd->pars[1]);
            if (value == 0)
            {
                sprintf(cmd->rsp_msg, "OK!");
                tr_stop_timer(TRACK_CUST_FT_WORK_TIMER_ID);
                track_cust_gps_location_timeout(10);
                return;
            }
            else
            {
                sprintf(cmd->rsp_msg, "Error: Number of parameters error! %d", cmd->part);
                return;
            }

        }
        else
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error! %d", cmd->part);
            return;
        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error! %d", cmd->part);
        return;
    }
    track_cust_gps_work_req((void *)16);
    sprintf(cmd->rsp_msg, "OK!");
}


#if defined(__GT310__)
static void cmd_fly(CMD_DATA_STRUCT *cmd)
{
    track_fly_mode_struct fly = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%0.2d:%0.2d,%d", cmd->pars[0], ON_OFF(G_parameter.fly_stu.sw),
                G_parameter.fly_stu.mode1_startTime / 60, G_parameter.fly_stu.mode1_startTime % 60, G_parameter.fly_stu.mode1_endTime / 60, G_parameter.fly_stu.mode1_endTime % 60, G_parameter.fly_stu.mode2_sensorOutTime);
        return;
    }
    memcpy(&fly, &G_parameter.fly_stu, sizeof(track_fly_mode_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        fly.sw = 1;
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (fly.sw == 1)
        {
            if (track_cust_work_mode_get_fly_mode())
            {
                track_cust_fly_mode_change((void*)88);
            }
        }
        fly.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    if (cmd->part > 3)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (cmd->part >= 2)
    {
        int value1, value;
        char *par = strchr(cmd->pars[2], ':'), *par1;
        if (NULL == par || strlen(cmd->pars[2]) > 11 || strlen(cmd->pars[2]) < 3)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM-HH:MM)");
            return;
        }
        *par = 0;
        value  = track_fun_atoi(cmd->pars[2]);
        if (value < 0 || value > 23)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM) HH:0 - 23");
            return;
        }
        par1 = strchr(par + 1, '-');
        if (NULL == par)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM-HH:MM)");
            return;
        }
        *par1 = 0;
        value1  = track_fun_atoi(par + 1);
        if (value1 < 0 || value1 > 59)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM-...) MM:0 - 59");
            return;
        }
        fly.mode1_startTime = value * 60 + value1;
        par = strchr(par1 + 1, ':');
        if (NULL == par)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM-HH:MM)");
            return;
        }
        *par = 0;
        value  = track_fun_atoi(par1 + 1);
        if (value < 0 || value > 23)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM) HH:0 - 23");
            return;
        }

        value1 = track_fun_atoi(par + 1);
        if (value1 < 0 || value1 > 59)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (...-HH:MM) MM:0 - 59");
            return;
        }
        fly.mode1_endTime = value * 60 + value1;
        if (fly.mode1_startTime < 0 || fly.mode1_startTime > 1440)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (0:00 to 23:59)");
        }
    }
    if (cmd->part >= 3)
    {
        fly.mode1_endTime = track_fun_atoi(cmd->pars[3]);
        if (fly.mode1_endTime < 5 || fly.mode1_startTime > 3600)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 3 (5 to 3600)");
        }
    }
    memcpy(&G_parameter.fly_stu, &fly, sizeof(track_fly_mode_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
};
#endif

#if defined(__FLY_MODE__)
static void cmd_flycut(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0],
                ON_OFF(G_parameter.flycut));
        return;
    }
    if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }

    if (!strcmp(cmd->pars[1], "ON"))
    {
        G_parameter.flycut = 1;
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (G_parameter.flycut == 1)
        {
            track_cust_flymode_set(0);
        }
        G_parameter.flycut = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }

    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}
#endif

#if defined(__XCWS_TPATH__)
extern void track_cust_server3_reconnect(void *arg);

static void cmd_server3(CMD_DATA_STRUCT *cmd)
{
    int value, plen;
    nvram_server_addr_struct server = {0};

    if (cmd->part == 0)
    {
        if (G_importance_parameter_data.server3.conecttype == 0)
        {
            sprintf(cmd->rsp_msg, "SERVER3,0,%d.%d.%d.%d,%d,%d",
                    G_importance_parameter_data.server3.server_ip[0], G_importance_parameter_data.server3.server_ip[1],
                    G_importance_parameter_data.server3.server_ip[2], G_importance_parameter_data.server3.server_ip[3],
                    G_importance_parameter_data.server3.server_port, G_importance_parameter_data.server3.soc_type);
        }
        else
        {
            sprintf(cmd->rsp_msg, "SERVER3,%d,%s,%d,%d",
                    G_importance_parameter_data.server3.conecttype,
                    track_domain_decode(G_importance_parameter_data.server3.url),
                    G_importance_parameter_data.server3.server_port,
                    G_importance_parameter_data.server3.soc_type);
        }
        return;
    }
    else if (cmd->part == 1 && !strcmp(cmd->pars[CM_Par1], "RESET"))
    {
        memcpy(&G_importance_parameter_data.server3, &OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT->server3, sizeof(nvram_server_addr_struct));
        Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        sprintf(cmd->rsp_msg, "reset OK!");
    }
    else if (cmd->part == 1 && !strcmp(cmd->pars[CM_Par1], "NULL"))
    {
        memset(&G_importance_parameter_data.server3, 0, sizeof(nvram_server_addr_struct));
        Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        sprintf(cmd->rsp_msg, "OK");
        track_soc_disconnect();
        //track_os_intoTaskMsgQueue(track_cust_write_sim_backup_for_parameter);
        return;
    }
    if (cmd->part != 3 && cmd->part != 4)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (G_importance_parameter_data.server3.lock != 0)
    {
        sprintf(cmd->rsp_msg, "Error: The platform addresses are Locked modify!");
        return;
    }
    memset(cmd->rsp_msg, 0, sizeof(cmd->rsp_msg));

    memcpy(&server, &G_importance_parameter_data.server3, sizeof(nvram_server_addr_struct));

    value = track_fun_atoi(cmd->pars[CM_Par1]);
    if (value != 0 && value != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0/1)");
        return;
    }
    server.conecttype = value;

    plen = strlen(cmd->pars[CM_Par2]);
    if (plen <= 0)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 2 is empty!");
        return;
    }
    if (plen >= 99)
    {
        sprintf(cmd->rsp_msg, "Error: Parameters 2 characters too long!");
        return;
    }
    if (track_fun_check_ip(cmd->pars[CM_Par2]))
    {
        if (server.conecttype != 0)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 domain is not formatted correctly!");
            return;
        }
        memset(server.server_ip, 0, 4);
        if (!track_fun_asciiIP2Hex(cmd->pars[CM_Par2], (void*)&server.server_ip[0]))
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 is not a valid IP address format!");
            return;
        }
        server.url[0] = 0;
    }
    else
    {
        if (server.conecttype != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 is not a valid IP address format!");
            return;
        }
        if (strstr(cmd->pars[CM_Par2], "http://"))
        {
            sprintf(cmd->rsp_msg, "Error: The domain name does not include \"http://\"!");
            return;
        }
        {
            char domain[TRACK_MAX_URL_SIZE] = {0};
            memcpy(domain, cmd->rcv_msg_source + (cmd->pars[CM_Par2] - cmd->rcv_msg), plen);
            track_domain_encode(domain, server.url);
            memset(server.server_ip, 0, 4);
        }
    }

    if (!track_fun_check_data_is_port(cmd->pars[CM_Par3]))
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 3 (0~65535)");
        return;
    }
    server.server_port = track_fun_string2U16(cmd->pars[CM_Par3]);

    if (cmd->part == 4)
    {
        value = track_fun_atoi(cmd->pars[CM_Par4]);
        if (value != 0 && value != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 4 (0/1)");
            return;
        }
    }
    server.soc_type = 0; // 当前只支持TCP
    memcpy(&G_importance_parameter_data.server3, &server, sizeof(nvram_server_addr_struct));

    Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
    sprintf(cmd->rsp_msg, "OK");
    //track_soc_reset_reconnect_count2();
    tr_start_timer(TRACK_CUST_NET_CONN_TIMEOUT_TIMER_ID3, 20 * 1000, track_cust_server3_conn);
    track_os_intoTaskMsgQueueExt(track_cust_server3_reconnect, (void*)4);//修改域名后，重新建立连接
    //track_os_intoTaskMsgQueue(track_cust_write_sim_backup_for_parameter);
}

static void cmd_ser3sw(CMD_DATA_STRUCT *cmd)
{
    int value;
    int status = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d (0=OFF, 1=ON)", cmd->pars[0], G_parameter.server3sw);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    value = track_fun_atoi(cmd->pars[1]);
    if (value < 0 || value > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Range error. (0=OFF, 1=ON)");
        return;
    }
    status = G_parameter.server3sw;
    G_parameter.server3sw = value;
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        if (G_parameter.server3sw == 1 && status == 0)
        {
            tr_stop_timer(TRACK_CUST_NET_CONN_TIMER_ID3);
            tr_start_timer(TRACK_CUST_NET_CONN_TIMEOUT_TIMER_ID3, 20 * 1000, track_cust_server3_conn);
        }
        else if (G_parameter.server3sw == 0 && status == 1)
        {
            track_cust_server3_disconnect();
        }
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }

}
#endif
#if defined(__XCWS__)
static void cmd_xcws(CMD_DATA_STRUCT *cmd)
{
    char str_data[200] = {0};
    char temp[50] = {0};
    kal_uint16 len = 0;
    extern void track_cust_cmd_build_format(kal_uint8 * data, kal_uint16 len, Cmd_Type cmd_type, CMD_DATA_STRUCT * gps_cmd);
    if (cmd->part > 2 || cmd->part < 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    LOGD(L_APP, L_V5, "%d,%s,%s,%s", cmd->part, cmd->pars[0], cmd->pars[1], cmd->pars[2]);
    sprintf(str_data, "%s", cmd->pars[1]);
    if (cmd->part == 2)
    {
        sprintf(temp, ",%s", cmd->pars[2]);
        strcat(str_data, temp);
    }
    len = strlen(str_data);
    //cmd->return_sms.ph_num;
    track_cust_cmd_build_format(str_data, len, cmd->return_sms.cm_type, cmd);

}
static void cmd_xlh(CMD_DATA_STRUCT *Cmds)
{
    kal_bool formaterror = KAL_FALSE;
    kal_int16 temp = 0;
    if (Cmds->part == 0)
    {
        sprintf(Cmds->rsp_msg, "%s:%s", Cmds->pars[0], G_importance_parameter_data.login.SequenceNumber);
        return;
    }
    if (Cmds->part == 1)
    {
        temp = strlen(Cmds->pars[1]);

        if (temp > 0 && temp < 19)
        {
            memcpy(G_importance_parameter_data.login.SequenceNumber, Cmds->pars[1], 18);
        }
        else if (temp > 18)
        {
            sprintf(Cmds->rsp_msg, "Error: Length of parameter1 error!");
            return;
        }
    }
    else
    {
        sprintf(Cmds->rsp_msg, "Error: Number of parameters error!");
        return;
    }

    sprintf(Cmds->rsp_msg, "OK!");
    if (formaterror == KAL_FALSE)
    {
        set_nvram_device_setting();
        track_os_intoTaskMsgQueueExt(track_soc_gprs_reconnect, (void*)4);//重新建立连接
    }
}
#endif /* __XCWS__ */


/******************************************************************************
 *  Function    -  cmd_fakecell
 *
 *  Purpose     -  防伪基站功能使能
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-11-11,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void cmd_fakecell(CMD_DATA_STRUCT *cmd)
{
    char *p;
    int len;
    kal_uint8 chip_rid[16];
    kal_uint16 CRC, CRC_IN = 0;
    char enable;

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "fake cell:%s", ON_OFF(G_parameter.fake_cell_enable) );
        return;
    }

    if (cmd->return_sms.cm_type != CM_AT)
    {
        sprintf(cmd->rsp_msg, "Error:support AT!");
        return;
    }

    if (cmd->part != 2)
    {
        sprintf(cmd->rsp_msg, "Error:Number of parameters error!");
        return;
    }

    memcpy(chip_rid, track_drv_get_chip_id(), 16);
    CRC = GetCrc16(chip_rid, 16);
    CRC_IN = track_fun_atoi(cmd->pars[2]);
    if (CRC != CRC_IN)
    {
        sprintf(cmd->rsp_msg, "Error:password error!");
        return;
    }

    enable = track_fun_atoi(cmd->pars[1]);
    if (enable != 0 && enable != 1)
    {
        sprintf(cmd->rsp_msg, "Error:Parameter 1 ");
        return;
    }

    if (G_parameter.fake_cell_enable != enable)
    {
        G_parameter.fake_cell_enable = enable;
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        OTA_track_custom_fake_cell_feature_enable(enable);
    }

    sprintf(cmd->rsp_msg, "OK");
}


static void cmd_fakebs(CMD_DATA_STRUCT *cmd)
{
    kal_uint8 fake_cell_enable;
    kal_uint8 fake_cell_to_ams;
    kal_int32 tmp;

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d", cmd->pars[0], ON_OFF(G_parameter.fake_cell_enable), G_parameter.fake_cell_to_ams);
        return;
    }
    else if (cmd->part > 2)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }

    fake_cell_enable = G_parameter.fake_cell_enable;
    fake_cell_to_ams = G_parameter.fake_cell_to_ams;

    if (strcmp(cmd->pars[1], "ON") == 0)
    {
        if (cmd->part < 1 || cmd->part > 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        fake_cell_enable = 1;
        if (cmd->part >= 2)
        {
            tmp = track_fun_atoi(cmd->pars[2]);
            if (tmp < 0 || tmp > 1)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0 to 1)");
                return;
            }
            fake_cell_to_ams = tmp;
        }
    }
    else if (strcmp(cmd->pars[1], "OFF") == 0)
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        fake_cell_enable = 0;
    }
    else if (strcmp(cmd->pars[1], "TEST") == 0)//便于测试查看当前是否防伪基站
    {
        if (cmd->part == 1)
        {
            sprintf(cmd->rsp_msg, "fakeCell temp status:%d", G_parameter.fake_cell_auto_temp);
            return;
        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }

    if (G_parameter.fake_cell_enable != fake_cell_enable)
    {
        G_parameter.fake_cell_enable = fake_cell_enable;
        OTA_track_custom_fake_cell_feature_enable(fake_cell_enable);
    }

    G_parameter.fake_cell_to_ams = fake_cell_to_ams;
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);

    sprintf(cmd->rsp_msg, "OK");
}


#if defined (__CUST_BT__)
/******************************************************************************
 *  Function    -  cmd_ble
 *
 *  Purpose     -  蓝牙设置
 *
 *  Description -  ON/OFF，广播时间，监听时间，休眠时间   (秒)
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-11-17,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static cmd_ble(CMD_DATA_STRUCT *cmd)
{
    nvram_bt_struct bt;
    kal_int32 tmp;

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d,%d,%d", cmd->pars[0], ON_OFF(G_parameter.bt.sw), \
                G_parameter.bt.broadcast_time, G_parameter.bt.listen_time, G_parameter.bt.sleep_time);
        return;
    }
    else if (cmd->part > 4)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }

    memcpy(&bt, &G_parameter.bt, sizeof(nvram_bt_struct));


    if (strcmp(cmd->pars[1], "ON") == 0)
    {
        bt.sw = 1;
    }
    else if (strcmp(cmd->pars[1], "OFF") == 0)
    {
        bt.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }

    if (cmd->part >= 2)
    {
        tmp = track_fun_atoi(cmd->pars[2]);
        if (tmp < 0 || tmp > 255)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (0 to 255)");
            return;
        }
        bt.broadcast_time = tmp;
    }

    if (cmd->part >= 3)
    {
        tmp = track_fun_atoi(cmd->pars[3]);
        if (tmp < 0 || tmp > 255)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 3 (0 to 255)");
            return;
        }
        bt.listen_time = tmp;
    }

    if (cmd->part >= 4)
    {
        tmp = track_fun_atoi(cmd->pars[4]);
        if (tmp < 0 || tmp > 65535)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 4 (0 to 65535)");
            return;
        }
        bt.sleep_time = tmp;
    }

    memcpy(&G_parameter.bt, &bt, sizeof(nvram_bt_struct));
    track_cust_bt_init();

    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

#endif /* __CUST_BT__ */

static void cmd_gpsdup(CMD_DATA_STRUCT *cmd)
{
    kal_uint8 gpsdup;

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], ON_OFF(G_parameter.gpsDup));
        return;
    }

    if (cmd->part != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }

    if (strcmp(cmd->pars[1], "ON") == 0)
    {
        gpsdup = 1;
    }
    else if (strcmp(cmd->pars[1], "OFF") == 0)
    {
        gpsdup = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }

    G_parameter.gpsDup = gpsdup;
    track_cust_gpsdup_switch(gpsdup);

    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }

}

static void cmd_fcfalm(CMD_DATA_STRUCT *cmd)
{
    nvram_FCFALM_struct alarm = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d,%d", cmd->pars[0],
                ON_OFF(G_parameter.fcfalm.sw), G_parameter.fcfalm.alarm_type, G_parameter.fcfalm.alarm_filter);
        return;
    }
    memcpy(&alarm, &G_parameter.fcfalm, sizeof(nvram_FCFALM_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part < 1 || cmd->part > 3)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 1;
        if (cmd->part >= 2)
        {
            alarm.alarm_type = track_fun_atoi(cmd->pars[2]);
            if (alarm.alarm_type != 0 && alarm.alarm_type != 1 && alarm.alarm_type != 2 && alarm.alarm_type != 3)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1/2/3)");
                return;
            }
        }

        if (cmd->part == 3)
        {
            alarm.alarm_filter = track_fun_atoi(cmd->pars[3]);
            if (alarm.alarm_filter < 15  || alarm.alarm_filter > 120)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (15-120)");
                return;
            }
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.fcfalm, &alarm, sizeof(nvram_FCFALM_struct));

    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

#if defined (__CUST_RS485__)
static void cmd_RS485(CMD_DATA_STRUCT *cmd)
{
    extern void track_rs485_read_vehicle_status_req(void);
    //track_drv_rs485_write("test RS485 send\r\n", strlen("test RS485 send\r\n"));
    track_rs485_read_vehicle_status_req();
    sprintf(cmd->rsp_msg, "RS485 send HEX");
}
#endif /* __CUST_RS485__ */

void cmd_sncovery(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        track_os_importance_backup_read();
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
    }
}

#if defined(__BACKUP_SERVER__)
static void cmd_Bserver(CMD_DATA_STRUCT *cmd)
{
    {
        int value, plen;
        nvram_server_addr_struct server = {0};
        extern  nvram_importance_parameter_struct NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT;
        if (cmd->part == 0)
        {
            if (G_importance_parameter_data.bserver.conecttype == 0)
            {
                sprintf(cmd->rsp_msg, "BSERVER,0,%d.%d.%d.%d,%d,%d",
                        G_importance_parameter_data.bserver.server_ip[0], G_importance_parameter_data.bserver.server_ip[1],
                        G_importance_parameter_data.bserver.server_ip[2], G_importance_parameter_data.bserver.server_ip[3],
                        G_importance_parameter_data.bserver.server_port, G_importance_parameter_data.bserver.soc_type);
            }
            else
            {
                sprintf(cmd->rsp_msg, "BSERVER,%d,%s,%d,%d",
                        G_importance_parameter_data.bserver.conecttype,
                        track_domain_decode(G_importance_parameter_data.bserver.url),
                        G_importance_parameter_data.bserver.server_port,
                        G_importance_parameter_data.bserver.soc_type);
            }
            return;
        }
        else if (cmd->part == 1 && !strcmp(cmd->pars[CM_Par1], "RESET"))
        {
            //extern nvram_importance_parameter_struct NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT[];
            memcpy(&G_importance_parameter_data.bserver, &NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT.bserver, sizeof(nvram_server_addr_struct));
            Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
            sprintf(cmd->rsp_msg, "reset OK!");
        }
        else if (cmd->part == 1 && !strcmp(cmd->pars[CM_Par1], "NULL"))
        {
            memset(&G_importance_parameter_data.bserver, 0, sizeof(nvram_server_addr_struct));
            Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
            sprintf(cmd->rsp_msg, "OK");
            track_soc_disconnect();
            //track_os_intoTaskMsgQueue(track_cust_write_sim_backup_for_parameter);

            return;
        }
        if (cmd->part != 3 && cmd->part != 4)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        if (G_importance_parameter_data.bserver.lock != 0)
        {
            sprintf(cmd->rsp_msg, "Error: The platform addresses are Locked modify!");
            return;
        }
        memset(cmd->rsp_msg, 0, sizeof(cmd->rsp_msg));

        memcpy(&server, &G_importance_parameter_data.bserver, sizeof(nvram_server_addr_struct));

        value = track_fun_atoi(cmd->pars[CM_Par1]);
        if (value != 0 && value != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (0/1)");
            return;
        }
        server.conecttype = value;

        plen = strlen(cmd->pars[CM_Par2]);
        if (plen <= 0)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 is empty!");
            return;
        }
        if (plen >= 99)
        {
            sprintf(cmd->rsp_msg, "Error: Parameters 2 characters too long!");
            return;
        }
        if (track_fun_check_ip(cmd->pars[CM_Par2]))
        {
            if (server.conecttype != 0)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 domain is not formatted correctly!");
                return;
            }
            memset(server.server_ip, 0, 4);
            if (!track_fun_asciiIP2Hex(cmd->pars[CM_Par2], (void*)&server.server_ip[0]))
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 is not a valid IP address format!");
                return;
            }
            server.url[0] = 0;
        }
        else
        {
            if (server.conecttype != 1)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 is not a valid IP address format!");
                return;
            }
            if (strstr(cmd->pars[CM_Par2], "http://"))
            {
                sprintf(cmd->rsp_msg, "Error: The domain name does not include \"http://\"!");
                return;
            }
            {
                char domain[TRACK_MAX_URL_SIZE] = {0};
                memcpy(domain, cmd->rcv_msg_source + (cmd->pars[CM_Par2] - cmd->rcv_msg), plen);
                track_domain_encode(domain, server.url);
            }
        }

        if (!track_fun_check_data_is_port(cmd->pars[CM_Par3]))
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 3 (0~65535)");
            return;
        }
        server.server_port = track_fun_string2U16(cmd->pars[CM_Par3]);

        if (cmd->part == 4)
        {
            value = track_fun_atoi(cmd->pars[CM_Par4]);
            if (value != 0 && value != 1)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 4 (0/1)");
                return;
            }
        }
        server.soc_type = 0; // 当前只支持TCP
        memcpy(&G_importance_parameter_data.bserver, &server, sizeof(nvram_server_addr_struct));
        Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        sprintf(cmd->rsp_msg, "OK");
        track_soc_reset_reconnect_count2();
        //track_os_intoTaskMsgQueue(track_cust_write_sim_backup_for_parameter);
    }
}
static void cmd_BsClear(CMD_DATA_STRUCT *cmd)
{
    nvram_server_addr_struct server = {0};
    if (cmd->part == 0)
    {
        memcpy(&G_importance_parameter_data.bserver, &server, sizeof(nvram_server_addr_struct));
        Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    sprintf(cmd->rsp_msg, "OK");
}
#endif

#if defined(__LUYING__)
extern  void track_cust_server4_conn(void);
extern void track_cust_stop_server4_conn(void);

kal_uint8* track_get_LY_stamp(void)
{
    LOGH(L_DRV, L_V5, G_realtime_data.record_stamp, 6);
    return G_realtime_data.record_stamp;
}

void track_ly_timeout(void)
{
    track_cust_paket_msg_upload(track_get_LY_stamp(), KAL_TRUE, "LY fail", strlen("LY fail"));
}

static void cmd_LY(CMD_DATA_STRUCT * cmd)
{
    int value;

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s,%d", cmd->pars[0], G_realtime_data.record_time);
        return;
    }

    if (track_cust_is_recording(2) || track_get_audio_queue() != 0)
    {
        track_os_intoTaskMsgQueue(track_cust_server4_conn);
        sprintf(cmd->rsp_msg, "Recording,Please try again later!");
        return;
    }

    if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }

    memset(G_realtime_data.record_stamp, 0, 4);
    if (cmd->return_sms.cm_type == CM_ONLINE)
    {
        memcpy(G_realtime_data.record_stamp, cmd->return_sms.stamp, 4);
    }

    value = track_fun_atoi(cmd->pars[1]);

#if defined (__GT370__)
    if (value < 10 || value > 30)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter (10-30)");
    }
#else
    if (value < 10 || value > 60)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter (10-60)");
    }
#endif /* __GT370__ */
    else
    {

        track_os_intoTaskMsgQueue(track_cust_server4_conn);
        track_cust_record(3, 1, RECORD_INTERVAL);
        G_realtime_data.record_time  = value;
        G_realtime_data.record_cnt = value / RECORD_INTERVAL;
        G_realtime_data.record_last_time = value % RECORD_INTERVAL;
        G_realtime_data.record_file = 0;
        G_realtime_data.record_up = 0;
        tr_start_timer(TRACK_LY_TIMEOUT_TEIMER_ID, (60 + value) * 1000, track_ly_timeout);
        tr_start_timer(TRACK_LY_TIMEOUT_10min_TEIMER_ID, (10 * 60 + value) * 1000, track_cust_stop_server4_conn);

        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE) || (Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE)))
        {
            sprintf(cmd->rsp_msg, "OK!");
        }
        else
        {
            sprintf(cmd->rsp_msg, "Error!");
        }
    }
}
#endif

#if defined(__DROP_ALM__)
static void cmd_dropalm(CMD_DATA_STRUCT * cmd)
{
    U8 value;
    nvram_drop_alarm nv_drop_alm = {0};
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d(0=only GPRS,1=GPRS+SMS,2=GPRS+SMS+CALL,3=GPRS+CALL)", cmd->pars[0], ON_OFF(G_parameter.drop_alm.sw));
        return;
    }
    else if (cmd->part != 2)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (!strcmp(cmd->pars[1], "ON"))
        nv_drop_alm.sw = 1;
    else if (!strcmp(cmd->pars[1], "OFF"))
        nv_drop_alm.sw = 0;
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    value = atoi(cmd->pars[2]);

    if (value > 3)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/3)");
        return;

    }
    nv_drop_alm.alarm_type = value;
    memcpy(&G_parameter.drop_alm, &nv_drop_alm, sizeof(nvram_drop_alarm));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}
#endif
#if defined(__SV_UPLOAD__)
static void cmd_sv(CMD_DATA_STRUCT *cmd)
{
    U16 value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%ds,%dmin", cmd->pars[0],
                ON_OFF(G_parameter.sv.sw), G_parameter.sv.t1, G_parameter.sv.t2);
        return;
    }
    if (cmd->part > 3)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (!strcmp(cmd->pars[1], "ON"))
    {
        G_parameter.sv.sw = 1;
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        G_parameter.sv.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    if (cmd->part > 1)
    {
        value = track_fun_atoi(cmd->pars[2]);
        if (value < 5 || value > 18000)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (5-18000)");
            return;
        }
        G_parameter.sv.t1 = value;
    }
    if (cmd->part > 2)
    {
        value = track_fun_atoi(cmd->pars[3]);
        if (value < 0 || value > 3600)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 3 (0-3600)");
            return;
        }
        G_parameter.sv.t2 = value;
    }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
    if (G_parameter.sv.sw == 1)
    {
        track_cust_net_sv_upload(0);
    }
}

static void cmd_cxsv(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        track_cust_paket_9409_handle();
        sprintf(cmd->rsp_msg, "OK!");
    }
}
#endif /* __SV_UPLOAD__ */

static void cmd_nbver(CMD_DATA_STRUCT *cmd)
{
    kal_uint8 sw_nbver;

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], ON_OFF(G_parameter.sw_nbver));
        return;
    }
    if (cmd->part != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (strcmp(cmd->pars[1], "ON") == 0)
    {
        sw_nbver = 1;
    }
    else if (strcmp(cmd->pars[1], "OFF") == 0)
    {
        sw_nbver = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    G_parameter.sw_nbver = sw_nbver;
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}


#if defined (__CUSTOM_DDWL__)
static void cmd_dormancy(CMD_DATA_STRUCT *cmd)
{
    kal_uint8 hour, min;

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d,%d", cmd->pars[0], ON_OFF(G_parameter.dormancy.sw), G_parameter.dormancy.hour, G_parameter.dormancy.min);
        return;
    }

    hour = G_parameter.dormancy.hour;
    min = G_parameter.dormancy.min;

    if (strcmp(cmd->pars[1], "ON") == 0)
    {
        if (cmd->part > 3)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }

        if (strlen(cmd->pars[2]) > 0)
        {
            hour = track_fun_atoi(cmd->pars[2]);
            if ((hour > 23) || (hour < 0))
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0-23)");
                return;
            }
        }

        if (strlen(cmd->pars[3]) > 0)
        {
            min = track_fun_atoi(cmd->pars[3]);
            if ((min > 59) || (min < 0))
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (0-59)");
                return;
            }
        }

        G_parameter.dormancy.sw = 1;
        G_parameter.dormancy.hour = hour;
        G_parameter.dormancy.min = min;
    }
    else if (strcmp(cmd->pars[1], "OFF") == 0)
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        G_parameter.dormancy.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }

    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
        if (G_parameter.dormancy.sw == 0)
        {
            track_cust_dormancy_change_flight_mode(0);
        }
        else
        {
            if (track_cust_gps_status() < 2)
            {
                track_cust_dormancy_change_flight_mode(1);
            }

            track_cust_dormancy_mode_check_wakeup();
        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

#endif /* __CUSTOM_DDWL__ */


#if defined (__GT380__)
extern void track_cust_poweroff_ready(void);
static void track_cust_cmd_shutdown_alarm(void)
{
    kal_uint32 index = 0;
    LOGD(L_APP, L_V5, "");

    if (!track_is_timer_run(TRACK_CUST_SHUTDOWN_DELAY_TIMER_ID))
    {
        if (G_parameter.pwroff.sw == 1)
        {
            track_cust_led_sleep(2);
            index = track_cust_check_sosnum();
            track_cust_alarm_pwoffalm();
            if (G_parameter.pwroff.alarm_type == 1 && index != 0)
            {
                tr_start_timer(TRACK_CUST_SHUTDOWN_DELAY_TIMER_ID, ((index+1) * 15) * 1000, track_cust_poweroff_ready);
            }
            else
            {
                tr_start_timer(TRACK_CUST_SHUTDOWN_DELAY_TIMER_ID, 15 * 1000, track_cust_poweroff_ready);
            }
        }
        else
        {
            tr_start_timer(TRACK_CUST_SHUTDOWN_DELAY_TIMER_ID, 15 * 1000, track_cust_poweroff_ready);
        }
    }
    else
    {
        LOGD(L_APP, L_V5, "关机报警中");
    }
}


static void cmd_shutdown(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        cmd->rsp_length = sprintf(cmd->rsp_msg, "The terminal will shutdown after 20sec!");
        if (G_parameter.pwroff.sw == 1)
        {
            tr_start_timer(TRACK_CUST_SHUTDOWN_DELAY_TIMER_ID, 3 * 1000, track_cust_cmd_shutdown_alarm);
        }
        else
        {
            tr_start_timer(TRACK_CUST_SHUTDOWN_DELAY_TIMER_ID, 18 * 1000, track_cust_poweroff_ready);
        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
    }
}

static void cmd_chargealm(CMD_DATA_STRUCT *cmd)
{
    kal_uint8 type;
    kal_int32 var;

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d", cmd->pars[0], ON_OFF(G_parameter.charge_alarm.sw), G_parameter.charge_alarm.alarm_type);
        return;
    }

    type = G_parameter.charge_alarm.alarm_type;

    if (strcmp(cmd->pars[1], "ON") == 0)
    {
        if (cmd->part > 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }

        if (strlen(cmd->pars[2]) > 0)
        {
            var = track_fun_atoi(cmd->pars[2]);
            if ((var > 1) || (var < 0))
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1)");
                return;
            }
            type = var;
        }

        G_parameter.charge_alarm.sw = 1;
        G_parameter.charge_alarm.alarm_type = type;
    }
    else if (strcmp(cmd->pars[1], "OFF") == 0)
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        G_parameter.charge_alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }

    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "ERROR!");
    }
}

#endif /* __GT380__ */
#if defined (__XYBB__)
extern 	void track_nt22_test_data(kal_uint16 index);
static void cmd_nt22_test(CMD_DATA_STRUCT *cmd)
{
    kal_uint8 time;

    if (strlen(cmd->pars[1]) > 0)
    {
        time = track_fun_atoi(cmd->pars[1]);
        if ((time > 23) || (time < 0))
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (0-23)");
            return;
        }
    }
    track_nt22_test_data(time);
    sprintf(cmd->rsp_msg, "OK!");
}

/******************************************************************************
 *  Function    -  cmd_login
 *
 *  Purpose     -  登录信息
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-23,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
static void cmd_login(CMD_DATA_STRUCT *cmd)
{
    kal_uint16 time = 0, server_len = 0;
    char *server_buf;
    char *server_port;
    nvram_importance_parameter_struct imp = {0};
    kal_uint16 len = 0;
    kal_uint8 *buf[5];
    char domain[TRACK_MAX_URL_SIZE] = {0};


    if (cmd->part == 0)
    {
        if (track_nvram_alone_parameter_read()->server.conecttype == 0)
        {
            sprintf(cmd->rsp_msg, "%s,%d,%d.%d.%d.%d:%d,,%d,%s,%s,%s,%s,%s,%d,%s", cmd->pars[0], G_importance_parameter_data.login_data.mode, track_nvram_alone_parameter_read()->server.server_ip[0], track_nvram_alone_parameter_read()->server.server_ip[1], 
                    track_nvram_alone_parameter_read()->server.server_ip[2], track_nvram_alone_parameter_read()->server.server_ip[3], track_nvram_alone_parameter_read()->server.server_port, track_nvram_alone_parameter_read()->server.soc_type,
                    G_importance_parameter_data.login_data.sim_num, G_importance_parameter_data.login_data.provincial, G_importance_parameter_data.login_data.manufacturer,
                    G_importance_parameter_data.login_data.terminal_model, G_importance_parameter_data.login_data.terminal_id, G_importance_parameter_data.login_data.car_color,
                    G_importance_parameter_data.login_data.car_num);
        }
        else
        {
            sprintf(cmd->rsp_msg, "%s,%d,%s:%d,,%d,%s,%s,%s,%s,%s,%d,%s", cmd->pars[0], G_importance_parameter_data.login_data.mode, track_domain_decode(track_nvram_alone_parameter_read()->server.url), track_nvram_alone_parameter_read()->server.server_port, track_nvram_alone_parameter_read()->server.soc_type,
                    G_importance_parameter_data.login_data.sim_num, G_importance_parameter_data.login_data.provincial, G_importance_parameter_data.login_data.manufacturer,
                    G_importance_parameter_data.login_data.terminal_model, G_importance_parameter_data.login_data.terminal_id, G_importance_parameter_data.login_data.car_color,
                    G_importance_parameter_data.login_data.car_num);
        }
        return;
    }



    memcpy(&imp, &G_importance_parameter_data, sizeof(nvram_importance_parameter_struct));


    if (strlen(cmd->pars[1]) > 0)//注册模式
    {
        time = track_fun_atoi(cmd->pars[1]);
        if ((time > 2) || (time < 0))
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (0-2)");
            return;
        }
        imp.login_data.mode = time;
    }
    LOGD(L_APP, L_V5, "pars[2] = %d", strlen(cmd->pars[2]));
    if (strlen(cmd->pars[2]) > 0)//主IP
    {

#if 0
        server_buf = strstr(cmd->pars[2], ":");
        len = server_buf - cmd->pars[2];
        LOGD(L_APP, L_V5, "len = %d", len);
        memcpy(server_buf, cmd->pars[2], len);
        LOGD(L_APP, L_V5, "server_buf = %s", server_buf);
#endif
        memset(buf, 0, sizeof(buf));

        track_fun_str_analyse(cmd->pars[2], buf, 2, NULL, "\r\n", ':');
        LOGD(L_APP, L_V5, "buf[0] = %s", buf[0]);
        LOGD(L_APP, L_V5, "buf[1] = %s", buf[1]);
        memcpy(domain, buf[0], strlen(buf[0]));

        if (track_fun_check_ip(domain))
        {
            imp.server.conecttype = 0;
            memset(imp.server.server_ip, 0, 4);
            if (!track_fun_asciiIP2Hex(domain, (void*)&imp.server.server_ip[0]))
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 is not a valid IP address format!");
                return;
            }
            imp.server.url[0] = 0;
        }
        else
        {


            imp.server.conecttype = 1;
            track_domain_encode(domain, imp.server.url);
            memset(imp.server.server_ip, 0, 4);
            memcpy(G_realtime_data.server_ip, imp.server.server_ip, 4);
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        }

        if (!track_fun_check_data_is_port(buf[1]))
        {
            sprintf(cmd->rsp_msg, "Error: server port (0~65535)");
            return;
        }
        imp.server.server_port = track_fun_string2U16(buf[1]);
    }
    if (strlen(cmd->pars[3]) > 0)//副IP
    {

    }
    if (strlen(cmd->pars[4]) > 0)//通讯方式
    {
        time = track_fun_atoi(cmd->pars[4]);
        if ((time > 1) || (time < 0))
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 4 (0-1)");
            return;
        }
        imp.server.soc_type = time;
    }
    len = strlen(cmd->pars[5]);//SIM卡号
    if (len > 0)
    {
        if (track_fun_check_str_is_number(1, cmd->pars[5]) == 0)
        {
            sprintf(cmd->rsp_msg, "Error:5 Number contains non-numeric characters!");
            return;
        }
        if (len > 11)
        {
            sprintf(cmd->rsp_msg, "Error:5 Number length exceeds 11 characters!");
            return;
        }
        else if (len < 3)
        {
            sprintf(cmd->rsp_msg, "Error:5 Number length must exceeds 3 characters!");
            return;
        }
        strcpy(imp.login_data.sim_num, cmd->pars[5]);
    }
    len = strlen(cmd->pars[6]);//省域ID
    if (len > 0)
    {
        if (len > 6)
        {
            sprintf(cmd->rsp_msg, "Error: 6");
            return;
        }
        strcpy(imp.login_data.provincial, cmd->pars[6]);
    }
    len = strlen(cmd->pars[7]);//制造商ID
    if (len > 0)
    {
        if (len > 5)
        {
            sprintf(cmd->rsp_msg, "Error: 7");
            return;
        }
        strcpy(imp.login_data.manufacturer, cmd->pars[7]);
    }
    len = strlen(cmd->pars[8]);//终端型号
    if (len > 0)
    {
        if (len > 20)
        {
            sprintf(cmd->rsp_msg, "Error: 8");
            return;
        }
        strcpy(imp.login_data.terminal_model, cmd->pars[8]);
    }
    len = strlen(cmd->pars[9]);//终端ID
    if (len > 0)
    {
        if (len > 7)
        {
            sprintf(cmd->rsp_msg, "Error: 9");
            return;
        }
        strcpy(imp.login_data.terminal_id, cmd->pars[9]);
    }
    if (strlen(cmd->pars[10]) > 0)//车牌颜色
    {
        time = track_fun_atoi(cmd->pars[10]);
        imp.login_data.car_color = time;
    }
    if (strlen(cmd->pars[11]) > 0)//车牌号码
    {
        strcpy(imp.login_data.car_num, cmd->pars[11]);
        LOGD(L_APP, L_V5, "car_num = %s", imp.login_data.car_num);
        LOGD(L_APP, L_V5, "car_num = %s", G_importance_parameter_data.login_data.car_num);
    }


    memcpy(&G_importance_parameter_data, &imp, sizeof(nvram_importance_parameter_struct));
    Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);

    track_soc_reset_reconnect_count2();
    track_os_intoTaskMsgQueueExt(track_soc_gprs_reconnect, (void*)22);//修改域名后，重新建立连接
    //track_os_intoTaskMsgQueue(track_cust_write_sim_backup_for_parameter);
    sprintf(cmd->rsp_msg, "OK!");
}


/******************************************************************************
 *  Function    -  cmd_code
 *
 *  Purpose     -  授权码
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-23,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
void cmd_code(CMD_DATA_STRUCT *cmd)
{
    kal_uint16 len = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], G_parameter.bb_data.authorize_code);
        return;
    }
    if (strlen(cmd->pars[1]) > 0)
    {
        if (len > 18)
        {
            sprintf(cmd->rsp_msg, "Error: data too long");
            return;
        }
        strcpy(G_parameter.bb_data.authorize_code, cmd->pars[1]);
    }
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    sprintf(cmd->rsp_msg, "OK!");
}

/******************************************************************************
 *  Function    -  cmd_acc
 *
 *  Purpose     -  ACC检测
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-23,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
void cmd_acc(CMD_DATA_STRUCT *cmd)
{
    kal_uint16 len = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0], ON_OFF(G_parameter.bb_data.acc_check));
        return;
    }
    if (strcmp(cmd->pars[1], "ON") == 0)
    {
        G_parameter.bb_data.acc_check = 1;
    }
    else if (strcmp(cmd->pars[1], "OFF") == 0)
    {
        G_parameter.bb_data.acc_check = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    sprintf(cmd->rsp_msg, "OK!");
}

#endif /* __XYBB__ */


#if defined(__OIL_DETECTION__)
extern void track_cust_oil_main(void);
static cmd_aioil(CMD_DATA_STRUCT *cmd)
{
    kal_uint16 value = 0;
    kal_uint16 value1 = 0;
    nvram_oil_struct oil_tmp;

    LOGD(L_APP, L_V5, "");
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,timer:%d,type:%d", cmd->pars[0],
                ON_OFF(G_parameter.oil.sw), G_parameter.oil.timer,G_parameter.oil.type);
        return;
    }
    memcpy(&oil_tmp, &G_parameter.oil, sizeof(nvram_oil_struct));
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part < 1 && cmd->part > 3)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }

        if (cmd->part > 1 && strlen(cmd->pars[2]) != 0)
        {
            value = track_fun_atoi(cmd->pars[2]);
            if ((value != track_fun_atoi(cmd->pars[2])) || (value <0 || value >65535))
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2[0-65535]");
                return;
            }
            oil_tmp.timer = value;
        }
        if (cmd->part > 2 && strlen(cmd->pars[3]) != 0)
        {
            value1 = track_fun_atoi(cmd->pars[3]);
            if ((value1 != track_fun_atoi(cmd->pars[3]))||(value1 <0 || value1 >255))
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2[0-255]");
                return;
            }
            oil_tmp.type = value1;
        }
        if (oil_tmp.sw == 0)
        {
            track_obd_init();
        }
        oil_tmp.sw = 1;
        tr_start_timer(TRACK_CUST_OIL_TIMER_ID, 5 * 1000, track_cust_oil_main);
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        oil_tmp.sw = 0;
        tr_stop_timer(TRACK_CUST_OIL_TIMER_ID);
        tr_stop_timer(DEALY_INIT_OBD_TIMER_ID);
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.oil, &oil_tmp, sizeof(nvram_oil_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}
#endif

#if defined(__MULTIPLE_OUTPUT__)
static kal_uint8 sign = 0;
static void track_cust_output_pluse(void);
//可控低输出开始
void track_start_pluse_output(void)
{
    LOGD(L_APP, L_V4, "脉冲输出 times=%d",G_parameter.outset.out_times);

    G_parameter.outset.output_always = 0;
    G_parameter.outset.sw = G_parameter.outset.out_times;
    if (G_parameter.outset.out_times == 0 || G_parameter.outset.out_continue_time == 0)
    {
        G_parameter.outset.sw = 1;
    }

    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    if (track_is_timer_run(TRACK_CUST_PULSE_OUTPUT_TIMER_ID))
    {
        tr_stop_timer(TRACK_CUST_PULSE_OUTPUT_TIMER_ID);
    }

    sign = 0;
    track_drv_gpio_out1(0);

    track_cust_output_pluse();
}


#if 1
/*可控低输出 -- chengjun  2017-07-05*/
static void track_cust_output_pluse(void)
{
    LOGD(L_APP, L_V5, "cnt=%d,sign=%d,time=%d",G_parameter.outset.sw,sign,G_parameter.outset.out_times);
    if (sign == 0)
    {
        track_drv_gpio_out1(1);
        sign = 1;
        if (G_parameter.outset.out_continue_time != 0)
        {
            tr_start_timer(TRACK_CUST_PULSE_OUTPUT_TIMER_ID, G_parameter.outset.out_continue_time, track_cust_output_pluse);
            if (G_parameter.outset.sw > 0 && G_parameter.outset.out_interval_time > 0 && G_parameter.outset.out_times != 0)
            {
                G_parameter.outset.sw --;
                Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
            }
        }
    }
    else
    {
        track_drv_gpio_out1(0);
        sign = 0;
        if (G_parameter.outset.out_interval_time == 0)
        {
            return;
        }
        if (G_parameter.outset.out_times == 0 || G_parameter.outset.sw > 0)
        {
            tr_start_timer(TRACK_CUST_PULSE_OUTPUT_TIMER_ID, G_parameter.outset.out_interval_time, track_cust_output_pluse);
            return;
        }
    }
}

static void cmd_outset(CMD_DATA_STRUCT *cmd)
{
    float time1 = 0,time2 = 0;
    kal_uint16 tally = 0;
    nvram_outset_struct outset = {0};
    if (cmd->part == 0)
    {
        time1 = (float)G_parameter.outset.out_continue_time/1000.00;
        time2 = (float)G_parameter.outset.out_interval_time/1000.00;
        sprintf(cmd->rsp_msg,"%s:%.1f,%.1f,%d",cmd->pars[0],time1,time2,G_parameter.outset.out_times);
        return;
    }
    if (cmd->part > 3)
    {
        sprintf(cmd->rsp_msg,"Error:Number of parameters error!");
        return;
    }
    memcpy(&outset,&G_parameter.outset,sizeof(nvram_outset_struct));
    if (cmd->part >= 1)
    {
        time1 = track_fun_atof(cmd->pars[1]);
        if (((time1 >= 0.1 && time1 <= 36000) ||time1 == 0) && time1 == track_fun_atof(cmd->pars[1]))
        {
            outset.out_continue_time = time1*1000;//设置单位秒，保存单位毫秒
        }
        else
        {
            sprintf(cmd->rsp_msg,"Error:Parameter 1 (0,0.1-36000)");
            return;
        }
        if (cmd->part > 1)
        {
            time2 = track_fun_atof(cmd->pars[2]);
            if (((time2 >= 0.1 && time2 <= 36000) || time2 == 0) && time2 == track_fun_atof(cmd->pars[2]))
            {
                outset.out_interval_time = time2*1000;
            }
            else
            {
                sprintf(cmd->rsp_msg,"Error:Parameter 2 (0,0.1-36000)");
                return;
            }
        }
        if (cmd->part > 2)
        {
            tally = track_fun_atoi(cmd->pars[3]);
            if ((tally >= 0 && tally <= 65535) && tally == track_fun_atoi(cmd->pars[3]))
            {
                outset.out_times = tally;
            }
            else
            {
                sprintf(cmd->rsp_msg,"Error:Parameter 3 (0-65535)");
                return;
            }
        }
        memcpy(&G_parameter.outset,&outset,sizeof(nvram_outset_struct));

        if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
        {
            sprintf(cmd->rsp_msg, "OK!");
        }
        else
        {
            sprintf(cmd->rsp_msg, "Error!");
        }
    }
}

static void cmd_output(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part != 0)
    {
        sprintf(cmd->rsp_msg,"Error:Number of parameters error!");
        return;
    }

    track_start_pluse_output();

    sprintf(cmd->rsp_msg, "OK!");
}

static void track_stop_pluse_output(void)
{
    sign = 0;
    G_parameter.outset.sw = 0;
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);

    tr_stop_timer(TRACK_CUST_PULSE_OUTPUT_TIMER_ID);
    track_drv_gpio_out1(0);
}

static void cmd_stopout(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part != 0)
    {
        sprintf(cmd->rsp_msg,"Error:Number of parameters error!");
        return;
    }
    track_stop_pluse_output();

    sprintf(cmd->rsp_msg, "OK!");
}


static void cmd_relay2_output(CMD_DATA_STRUCT *cmd)
{
    int value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg,"%s:%d",cmd->pars[0],G_parameter.outset.output_always);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    value = track_fun_atoi(cmd->pars[1]);
    if (value < 0 || value > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Range error. (0=English, 1=Chinese)");
        return;
    }
    G_parameter.outset.output_always = value;

    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
        track_stop_pluse_output();
        if (G_parameter.outset.output_always==1)
        {
            track_drv_gpio_out1(1);
        }
        else
        {
            track_drv_gpio_out1(0);
        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }

}


void track_cust_output_init(void)
{
    if (G_parameter.outset.output_always==1)
    {
        track_drv_gpio_out1(1);
    }
    else if ((G_parameter.outset.out_interval_time != 0 || G_parameter.outset.out_continue_time == 0 || (G_parameter.outset.out_times == 0 && G_parameter.outset.out_interval_time != 0))&& G_parameter.outset.sw > 0)
    {
        tr_start_timer(TRACK_CUST_PULSE_OUTPUT_TIMER_ID, 10*1000, track_cust_output_pluse);
    }
    else
    {
        track_drv_gpio_out1(0);
    }
}
#endif

#if 1
/*可控高输出 -- chengjun  2017-07-05*/
static kal_uint8 sign2 = 0;
static void track_cust_output2_pluse(void)
{
    LOGD(L_APP, L_V1, "cnt=%d,sign=%d,time=%d",G_parameter.outset2.sw,sign2,G_parameter.outset2.out_times);
    if (sign2 == 0)
    {
        track_drv_gpio_out2(1);
        sign2 = 1;
        if (G_parameter.outset2.out_continue_time != 0)
        {
            tr_start_timer(TRACK_CUST_PULSE_OUTPUT2_TIMER_ID, G_parameter.outset2.out_continue_time, track_cust_output2_pluse);
            if (G_parameter.outset2.sw > 0 && G_parameter.outset2.out_interval_time > 0 && G_parameter.outset2.out_times != 0)
            {
                G_parameter.outset2.sw --;
                Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
            }
        }
    }
    else
    {
        track_drv_gpio_out2(0);
        sign2 = 0;
        if (G_parameter.outset2.out_interval_time == 0)
        {
            return;
        }
        if (G_parameter.outset2.out_times == 0 || G_parameter.outset2.sw > 0)
        {
            tr_start_timer(TRACK_CUST_PULSE_OUTPUT2_TIMER_ID, G_parameter.outset2.out_interval_time, track_cust_output2_pluse);
            return;
        }
    }
}

static void cmd_outset2(CMD_DATA_STRUCT *cmd)
{
    float time1 = 0,time2 = 0;
    kal_uint16 tally = 0;
    nvram_outset_struct outset = {0};
    if (cmd->part == 0)
    {
        time1 = (float)G_parameter.outset2.out_continue_time/1000.00;
        time2 = (float)G_parameter.outset2.out_interval_time/1000.00;
        sprintf(cmd->rsp_msg,"%s:%.1f,%.1f,%d",cmd->pars[0],time1,time2,G_parameter.outset2.out_times);
        return;
    }
    if (cmd->part > 3)
    {
        sprintf(cmd->rsp_msg,"Error:Number of parameters error!");
        return;
    }
    memcpy(&outset,&G_parameter.outset2,sizeof(nvram_outset_struct));
    if (cmd->part >= 1)
    {
        time1 = track_fun_atof(cmd->pars[1]);
        if (((time1 >= 0.1 && time1 <= 36000) ||time1 == 0) && time1 == track_fun_atof(cmd->pars[1]))
        {
            outset.out_continue_time = time1*1000;//设置单位秒，保存单位毫秒
        }
        else
        {
            sprintf(cmd->rsp_msg,"Error:Parameter 1 (0,0.1-36000)");
            return;
        }
        if (cmd->part > 1)
        {
            time2 = track_fun_atof(cmd->pars[2]);
            if (((time2 >= 0.1 && time2 <= 36000) || time2 == 0) && time2 == track_fun_atof(cmd->pars[2]))
            {
                outset.out_interval_time = time2*1000;
            }
            else
            {
                sprintf(cmd->rsp_msg,"Error:Parameter 2 (0,0.1-36000)");
                return;
            }
        }
        if (cmd->part > 2)
        {
            tally = track_fun_atoi(cmd->pars[3]);
            if ((tally >= 0 && tally <= 65535) && tally == track_fun_atoi(cmd->pars[3]))
            {
                outset.out_times = tally;
            }
            else
            {
                sprintf(cmd->rsp_msg,"Error:Parameter 3 (0-65535)");
                return;
            }
        }
        memcpy(&G_parameter.outset2,&outset,sizeof(nvram_outset_struct));

        if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
        {
            sprintf(cmd->rsp_msg, "OK!");
        }
        else
        {
            sprintf(cmd->rsp_msg, "Error!");
        }
    }
}

static void cmd_output2(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part != 0)
    {
        sprintf(cmd->rsp_msg,"Error:Number of parameters error!");
        return;
    }

    G_parameter.outset2.output_always = 0;
    G_parameter.outset2.sw = G_parameter.outset2.out_times;
    if (G_parameter.outset2.out_times == 0 || G_parameter.outset2.out_continue_time == 0)
    {
        G_parameter.outset2.sw = 1;
    }

    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    if (track_is_timer_run(TRACK_CUST_PULSE_OUTPUT2_TIMER_ID))
    {
        tr_stop_timer(TRACK_CUST_PULSE_OUTPUT2_TIMER_ID);
    }

    sign2 = 0;
    track_drv_gpio_out2(0);

    track_cust_output2_pluse();
    sprintf(cmd->rsp_msg, "OK!");
}

static void track_stop_pluse_output2(void)
{
    sign2 = 0;
    G_parameter.outset2.sw = 0;
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);

    tr_stop_timer(TRACK_CUST_PULSE_OUTPUT2_TIMER_ID);
    track_drv_gpio_out2(0);
}

static void cmd_stopout2(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part != 0)
    {
        sprintf(cmd->rsp_msg,"Error:Number of parameters error!");
        return;
    }
    track_stop_pluse_output2();

    sprintf(cmd->rsp_msg, "OK!");
}


static void cmd_relay3_output(CMD_DATA_STRUCT *cmd)
{
    int value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg,"%s:%d",cmd->pars[0],G_parameter.outset2.output_always);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    value = track_fun_atoi(cmd->pars[1]);
    if (value < 0 || value > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Range error. (0=English, 1=Chinese)");
        return;
    }
    G_parameter.outset2.output_always = value;

    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
        track_stop_pluse_output2();
        if (G_parameter.outset2.output_always==1)
        {
            track_drv_gpio_out2(1);
        }
        else
        {
            track_drv_gpio_out2(0);
        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }

}


void track_cust_output2_init(void)
{
    if (G_parameter.outset2.output_always==1)
    {
        track_drv_gpio_out2(1);
    }
    else if ((G_parameter.outset2.out_interval_time != 0 || G_parameter.outset2.out_continue_time == 0 || (G_parameter.outset2.out_times == 0 && G_parameter.outset2.out_interval_time != 0))&& G_parameter.outset2.sw > 0)
    {
        tr_start_timer(TRACK_CUST_PULSE_OUTPUT2_TIMER_ID, 10*1000, track_cust_output2_pluse);
    }
    else
    {
        track_drv_gpio_out2(0);
    }
}
#endif

static void cmd_incout(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg,"%s:%s",cmd->pars[0],ON_OFF(G_parameter.incout));
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }

    if (strcmp(cmd->pars[1], "ON") == 0)
    {
        G_parameter.incout=1;

    }
    else if (strcmp(cmd->pars[1], "OFF")==0)
    {
        G_parameter.incout=0;

    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }

    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}

#endif

#if defined (__NT37__)
static void cmd_input(CMD_DATA_STRUCT *cmd)
{
    char *sms_head=NULL;
    kal_uint8 len;

    //中文不支持
    if (G_parameter.lang==1)
    {
        sprintf(cmd->rsp_msg, "Only English version support");
        return;
    }

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg,"%s:%s",cmd->pars[0],G_parameter.customize_alarm_sms);
        return;
    }

    LOGD(1, 1, "len=%d,smsall=%s", cmd->rcv_length,cmd->rcv_msg_source);

    sms_head=strchr(cmd->rcv_msg_source,',');
    if (sms_head==NULL)
    {
        sprintf(cmd->rsp_msg, "Error!");
        return;
    }

    sms_head+=1;
    LOGD(1, 1, "sms=%s", sms_head);

    len=strlen(sms_head);

    if (len<3)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (message is too short)");
        return;
    }

    if (cmd->return_sms.cm_type == CM_SMS)
    {
        len-=1;//除去#
    }

    LOGD(L_APP, L_V3, "%d", len);
    if (len > (sizeof(G_parameter.customize_alarm_sms)-1))
    {
        sprintf(cmd->rsp_msg, "Error!,Custom information is too long");
        return;
    }

    strncpy(G_parameter.customize_alarm_sms, sms_head,sizeof(G_parameter.customize_alarm_sms));

    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
}
#endif /* __NT37__ */

static void track_gpsmode(CMD_DATA_STRUCT * Cmds)
{
    kal_int16 gmode =0;
    if (Cmds->part == 0)
    {
        sprintf(Cmds->rsp_msg, "GPS Work Mode is %d (0 BD;1 GPS;2 Double)", G_realtime_data.gps_mode);
    }
    else if (Cmds->part == 1)
    {
        gmode=  track_fun_atoi(Cmds->pars[1]);
        if (gmode<3&&gmode>=0)
        {

        }
        else
        {
            sprintf(Cmds->rsp_msg, "Error Parameter");
            return;
        }
        if (track_cust_gps_status() == 0)
        {
            track_drv_gpsmode_set(99);//清标志
        }
        else
        {
            track_cust_set_gps_mode(gmode);
        }
        if (G_realtime_data.gps_mode != gmode)
        {
            G_realtime_data.gps_mode=gmode;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        }
        sprintf(Cmds->rsp_msg, "OK!GPS Work Mode is %d (0 BD;1 GPS;2 Double)", G_realtime_data.gps_mode);


    }
    else
    {
        sprintf(Cmds->rsp_msg, "Error: Parameter 1");
    }


}

#if defined(__JM66__)//_liwen
extern void track_cust_jm66_shutdown(void);

static void cmd_jm66_mode(CMD_DATA_STRUCT *cmd)
{
    U16 mode2_interval;
    int value;
    if (cmd->part == 0 && strcmp(cmd->pars[0], "ALMCANCEL"))
    {
        if (Jm66_Current_mode > 10)
        {
            sprintf(cmd->rsp_msg, "Mode:2,%d", G_parameter.mode2_interval);
            return;
        }
        else
        {
            sprintf(cmd->rsp_msg, "Mode:1");
            return;
        }
    }
    mode2_interval = G_parameter.mode2_interval;
    if (!strcmp(cmd->pars[1], "1") || !strcmp(cmd->pars[0], "ALMCANCEL"))
    {
        if (cmd->part > 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        if (Jm66_Current_mode < 10)
        {
            sprintf(cmd->rsp_msg, "Error: Equipment is already in normal mode.");
            return;
        }
        else
        {
            sprintf(cmd->rsp_msg, "OK!GSM will back to normal mode and shutdown.");
            tr_start_timer(TRACK_CUST_DELAY_SHUTDOWN_FORCE_TIMER_ID, 4000, track_cust_jm66_shutdown);
            return;
        }
    }
    else if (!strcmp(cmd->pars[1], "2"))
    {
        if (cmd->part > 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        if (cmd->part == 2)
        {
            value  = track_fun_atoi(cmd->pars[2]);
            if (value < 1 || value > 60)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (1 to 60)");
                return;
            }
            mode2_interval = value;
        }
    }
    G_parameter.mode2_interval = mode2_interval;
    Jm66_Current_mode = 14;
    track_cust_into_tracking_mode(1);
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!Into tracking mode.");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!Write flash failed.");
    }
}
static void cmd_blsset(CMD_DATA_STRUCT *cmd)
{
    jm66_bt_timepar_struct timepar = {0};
    int value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d,%d", cmd->pars[0],
                G_parameter.bt_timepar.scan_interval,
                G_parameter.bt_timepar.scan_last);
        return;
    }
    else if (cmd->part > 2)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    memcpy(&timepar, &G_parameter.bt_timepar, sizeof(jm66_bt_timepar_struct));
    value  = track_fun_atoi(cmd->pars[1]);
    if (value < 0 || value > 120)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0 to 120)");
        return;
    }
    timepar.scan_interval = value;
    if (cmd->part > 1)
    {
        value = track_fun_atoi(cmd->pars[2]);
        if (value < 0 || value > 60)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (0 to 60)");
            return;
        }
        timepar.scan_last = value;
    }
    memcpy(&G_parameter.bt_timepar, &timepar, sizeof(jm66_bt_timepar_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
    track_cust_tbt100_set_timepar();
}
static void cmd_blbset(CMD_DATA_STRUCT *cmd)
{
    jm66_bt_timepar_struct timepar = {0};
    int value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d,%d", cmd->pars[0],
                G_parameter.bt_timepar.broadcast_interval,
                G_parameter.bt_timepar.broadcast_last);
        return;
    }
    else if (cmd->part > 2)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    memcpy(&timepar, &G_parameter.bt_timepar, sizeof(jm66_bt_timepar_struct));
    value  = track_fun_atoi(cmd->pars[1]);
    if (value < 0 || value > 60)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0 to 60)");
        return;
    }
    timepar.broadcast_interval = value;
    if (cmd->part > 1)
    {
        value = track_fun_atoi(cmd->pars[2]);
        if (value < 0 || value > 1000)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (0 to 1000)");
            return;
        }
        timepar.broadcast_last = value;
    }
    memcpy(&G_parameter.bt_timepar, &timepar, sizeof(jm66_bt_timepar_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
    track_cust_tbt100_set_timepar();
}
static void cmd_senswork(CMD_DATA_STRUCT *cmd)
{
    jm66_sensor_struct senswork = {0};
    int value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%0.2d:%0.2d,%0.2d:%0.2d", cmd->pars[0],
                G_parameter.sensorpar.sensor_start_hour,G_parameter.sensorpar.sensor_start_min,
                G_parameter.sensorpar.sensor_end_hour,G_parameter.sensorpar.sensor_end_min);
        return;
    }
    else if (cmd->part > 2)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    memcpy(&senswork, &G_parameter.sensorpar, sizeof(jm66_sensor_struct));
    if (cmd->part >= 1)
    {
        int value1;
        char *par = strchr(cmd->pars[1], ':');
        if (NULL == par || strlen(cmd->pars[1]) > 5 || strlen(cmd->pars[1]) < 3)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM)");
            return;
        }
        *par = 0;
        value  = track_fun_atoi(cmd->pars[1]);
        if (value < 0 || value > 23)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM) HH:0 - 23");
            return;
        }
        value1  = track_fun_atoi(par + 1);
        if (value1 < 0 || value1 > 59)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM) MM:0 - 59");
            return;
        }
        senswork.sensor_start_hour = value;
        senswork.sensor_start_min = value1;
    }
    if (cmd->part >= 2)
    {
        int value1;
        char *par = strchr(cmd->pars[2], ':');
        if (NULL == par || strlen(cmd->pars[2]) > 5 || strlen(cmd->pars[2]) < 3)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM)");
            return;
        }
        *par = 0;
        value  = track_fun_atoi(cmd->pars[2]);
        if (value < 0 || value > 23)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM) HH:0 - 23");
            return;
        }
        value1  = track_fun_atoi(par + 1);
        if (value1 < 0 || value1 > 59)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM) MM:0 - 59");
            return;
        }
        senswork.sensor_end_hour = value;
        senswork.sensor_end_min = value1;
    }
    memcpy(&G_parameter.sensorpar, &senswork, sizeof(jm66_sensor_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
    track_cust_tbt100_set_sensorpar();
}
static void cmd_jm66_sensorset(CMD_DATA_STRUCT *cmd)
{
    nvram_vibrates_alarm_struct alarm = {0};
    int value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d,%d,%d", cmd->pars[0],
                G_parameter.vibrates_alarm.detection_time,
                G_parameter.vibrates_alarm.detection_count,
                G_parameter.vibrates_alarm.sampling_interval);
        return;
    }
    else if (cmd->part > 3)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    memcpy(&alarm, &G_parameter.vibrates_alarm, sizeof(nvram_vibrates_alarm_struct));
    value  = track_fun_atoi(cmd->pars[1]);
    if (value < 10 || value > 300)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (10 to 300)");
        return;
    }
    alarm.detection_time = value;
    if (cmd->part > 1)
    {
        value = track_fun_atoi(cmd->pars[2]);
        if (value < 1 || value > 20)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 (1 to 20)");
            return;
        }
        alarm.detection_count = value;
    }
    if (cmd->part > 2)
    {
        value = track_fun_atoi(cmd->pars[3]);
        if (value < 1 || value > 3)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 3 (1 to 3)");
            return;
        }
        alarm.detection_count = value;
    }
    if (cmd->part > 3)
    {
        value = track_fun_atoi(cmd->pars[4]);
        if (value < 1 || value > 3)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 4 (1 to 3)");
            return;
        }
        alarm.sampling_interval = value;
    }
    memcpy(&G_parameter.vibrates_alarm, &alarm, sizeof(nvram_vibrates_alarm_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
    track_cust_tbt100_set_sensorpar();
}
static void cmd_sensormode(CMD_DATA_STRUCT *cmd)
{
    jm66_sensor_struct sensormode = {0};
    int value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d\r\n0:Normal,1:Standby,2:LowPow1\r\n3:LowPow2,4:Suspend,5:DeepSuspend",
                cmd->pars[0], G_parameter.sensorpar.sensor_mode);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    memcpy(&sensormode, &G_parameter.sensorpar, sizeof(jm66_sensor_struct));
    value  = track_fun_atoi(cmd->pars[1]);
    if (value < 0 || value > 5)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0 to 5)");
        return;
    }
    sensormode.sensor_mode = value;
    memcpy(&G_parameter.sensorpar, &sensormode, sizeof(jm66_sensor_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
    track_cust_tbt100_set_sensorpar();
}
static void cmd_bsensor(CMD_DATA_STRUCT *cmd)
{
    kal_uint8 bsensor = 0;
    int value;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d",
                cmd->pars[0], G_parameter.bsensor_sampling_interval);
        return;
    }
    else if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    bsensor = G_parameter.bsensor_sampling_interval;
    value  = track_fun_atoi(cmd->pars[1]);
    if (value < 0 || value > 12)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0 to 12)");
        return;
    }
    bsensor = value;
    G_parameter.bsensor_sampling_interval = bsensor;
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
    track_cust_tbt100_set_envpar();
}
static void cmd_powersave(CMD_DATA_STRUCT *cmd)
{
    track_cust_tbt100_into_transport();
    sprintf(cmd->rsp_msg, "OK!");
}
static void cmd_mtwork(CMD_DATA_STRUCT *cmd)
{
    static char boottype[3][4] = {"","SID","GPS"};
    kal_uint16 mtworkt[2][24] = {0};
    kal_uint8 mtworkt_count = 0;
    int value, gpscount = 0;
    if (cmd->part == 0)
    {
        char m_buf[500] = {0};
        U8 vle = G_parameter.mtworkt_count, i = 0;
        for (; i < vle; i++)
        {
            snprintf(m_buf + strlen(m_buf), 99, "#%d %0.2d:%0.2d %s", i+1, G_parameter.mtworkt[0][i] / 60, G_parameter.mtworkt[0][i] % 60, boottype[G_parameter.mtworkt[1][i]]);
        }
        sprintf(cmd->rsp_msg, "MTWORK:%s", m_buf);
        return;
    }
    mtworkt_count = G_parameter.mtworkt_count;
    memcpy(mtworkt, &G_parameter.mtworkt, 96);
    if (!strcmp(cmd->pars[1], "A"))
    {
        int i, j, n, value1, value2 = 1;
        U16 tmp,  tmp2;
        kal_bool is_add = KAL_TRUE;//0则为修改已有项目
        if (cmd->part > 3)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        if (mtworkt_count >= 24)
        {
            sprintf(cmd->rsp_msg, "Error: Have reached the maximum 24,could not add.");
            return;
        }
        if (cmd->part >= 2)
        {
            char *par = strchr(cmd->pars[2], ':');
            if (NULL == par || strlen(cmd->pars[2]) > 5 || strlen(cmd->pars[2]) < 3)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM)");
                return;
            }
            *par = 0;
            value  = track_fun_atoi(cmd->pars[2]);
            if (value < 0 || value > 23)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM) HH:0 - 23");
                return;
            }
            value1  = track_fun_atoi(par + 1);
            if (value1 < 0 || value1 > 59)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (HH:MM) MM:0 - 59");
                return;
            }
        }
        if (cmd->part >= 3)
        {
            if (!strcmp(cmd->pars[3], "GPS"))
            {
                value2 = 2;
            }
            else if (!strcmp(cmd->pars[3], "SID"))
            {
                value2 = 1;
            }
            else
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (GPS/SID)");
                return;
            }
        }
        i = value * 60 + value1;
        j = value2;
        for (n = 0; n < mtworkt_count; n++)
        {
            if (i == mtworkt[0][n])
            {
                if (j == mtworkt[1][n])
                {
                    sprintf(cmd->rsp_msg, "Error: Exactly the same project.");
                    return;
                }
                else
                {
                    is_add = KAL_FALSE;
                    break;
                }
            }
        }
        if (is_add)
        {
            mtworkt[0][mtworkt_count] = i;
            mtworkt[1][mtworkt_count] = j;
            mtworkt_count++;
        }
        else
        {
            mtworkt[1][n] = j;
        }
        for (n = 0; n < mtworkt_count; n++)
        {
            if (mtworkt[1][n] == 2)gpscount++;
        }
        if (gpscount < 4 || gpscount > 6)
        {
            sprintf(cmd->rsp_msg, "Error: The number of GPS BOOT should >=4 and <=6.");
            return;
        }
        for (j = 0; j < mtworkt_count -1; j++)//排序
        {
            for (i = 0; i < mtworkt_count -1 - j; i++)
            {
                if (mtworkt[0][i] > mtworkt[0][i + 1])
                {
                    tmp = mtworkt[0][i];
                    mtworkt[0][i] = mtworkt[0][i + 1];
                    mtworkt[0][i + 1] = tmp;

                    tmp2 = mtworkt[1][i];
                    mtworkt[1][i] = mtworkt[1][i + 1];
                    mtworkt[1][i + 1] = tmp2;
                }
            }
        }
        for (j = 1; j < mtworkt_count; j++)
        {
            if ((mtworkt[0][j] - mtworkt[0][j - 1]) < 60)
            {
                sprintf(cmd->rsp_msg, "Error: Two time intervals could not less than one hour.");
                return;
            }
        }
        if (mtworkt_count > 1 &&  (mtworkt[0][0] + 1440 - mtworkt[0][mtworkt_count - 1]) < 60)
        {
            sprintf(cmd->rsp_msg, "Error: Two time intervals could not less than one hour.");
            return;
        }
        G_parameter.mtworkt_count = mtworkt_count;
        memcpy(&G_parameter.mtworkt, mtworkt, 96);
        if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
            sprintf(cmd->rsp_msg, "OK!");
        else
            sprintf(cmd->rsp_msg, "Error!Write flash failed!");
    }
    else if (!strcmp(cmd->pars[1], "D"))//注意序号1对应数组下标0
    {
        int count, value1, value,n,i,j;
        U8 delete_index[24] = {0};
        kal_bool is_find = KAL_FALSE;
        kal_uint16 tmp1, tmp2;
        if (mtworkt_count < 1)
        {
            sprintf(cmd->rsp_msg, "Error: The list has no project,could not delete.");
            return;
        }
        if (cmd->part < 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        for (count = 0; count < cmd->part - 1; count++)
        {
            char *par = strchr(cmd->pars[2 + count], ':');
            if (NULL != par)
            {
                if (strlen(cmd->pars[2 + count]) > 5 || strlen(cmd->pars[2 + count]) < 3)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter %d (HH:MM/Index)", 2 + count);
                    return;
                }
                else
                {
                    *par = 0;
                    value  = track_fun_atoi(cmd->pars[2 + count]);
                    if (value < 0 || value > 23)
                    {
                        sprintf(cmd->rsp_msg, "Error: Parameter %d (HH:MM) HH:0 - 23", 2 + count);
                        return;
                    }
                    value1  = track_fun_atoi(par + 1);
                    if (value1 < 0 || value1 > 59)
                    {
                        sprintf(cmd->rsp_msg, "Error: Parameter %d (HH:MM) MM:0 - 59", 2 + count);
                        return;
                    }
                    tmp1 = value * 60 + value1;
                    for (n = 0; n < mtworkt_count; n++)
                    {
                        if (mtworkt[0][n] == tmp1)
                        {
                            delete_index[n] = 1;
                            is_find = KAL_TRUE;
                            break;
                        }
                    }
                    if (!is_find)
                    {
                        sprintf(cmd->rsp_msg, "Error: Parameter %d .This time is not exist.", 2 + count);
                        return;
                    }
                }
            }
            else if (NULL == par)
            {
                tmp2 = track_fun_atoi(cmd->pars[2 + count]);
                if (tmp2 > mtworkt_count || tmp2 < 1)
                {
                    sprintf(cmd->rsp_msg, "Error: Parameter %d .The index to the project does not exist.", 2 + count);
                    return;
                }
                else
                {
                    delete_index[tmp2 -1] = 1;
                }
            }
        }
        tmp2 = 0;
        for (tmp1 = 0; tmp1 < 24; tmp1++)
        {
            if (delete_index[tmp1])
            {
                mtworkt[0][tmp1] = 0xFFFF;
                mtworkt[1][tmp1] = 0;
                mtworkt_count--;
            }
            else if (mtworkt[1][tmp1] == 0)
            {
                mtworkt[0][tmp1] = 0xFFFF;
            }
        }
        for (n = 0; n < 24; n++)
        {
            if (mtworkt[1][n] == 2)gpscount++;
        }
        if (gpscount < 4 || gpscount > 6)
        {
            sprintf(cmd->rsp_msg, "Error: The number of GPS BOOT should >=4 and <=6.");
            return;
        }
        for (j = 0; j < 23; j++)//排序
        {
            for (i = 0; i < 23 - j; i++)
            {
                if (mtworkt[0][i] > mtworkt[0][i + 1])
                {
                    tmp1 = mtworkt[0][i];
                    mtworkt[0][i] = mtworkt[0][i + 1];
                    mtworkt[0][i + 1] = tmp1;

                    tmp2 = mtworkt[1][i];
                    mtworkt[1][i] = mtworkt[1][i + 1];
                    mtworkt[1][i + 1] = tmp2;
                }
            }
        }
        G_parameter.mtworkt_count = mtworkt_count;
        memcpy(&G_parameter.mtworkt, mtworkt, 96);
        if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
            sprintf(cmd->rsp_msg, "OK!");
        else
            sprintf(cmd->rsp_msg, "Error!Write flash failed!");
    }
    else if (!strcmp(cmd->pars[1], "LBSRESET"))
    {
        memset(&G_realtime_data.cell_id[0], 0, 40);
        G_realtime_data.valid_count = 0;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        sprintf(cmd->rsp_msg, "OK!LBS list reset successfully!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (A/D)");
        return;
    }
}
static void cmd_tbt_test(CMD_DATA_STRUCT * cmd)
{
    int value = 0,value2 = 0;
    value = track_fun_atoi(cmd->pars[1]);
    switch (value)
    {
        case 0:
        {
            track_cust_tbt100_into_transport();
            break;
        }
        case 1:
        {
            track_cust_tbt100_get_timepar();
            break;
        }
        case 2:
        {
            track_cust_tbt100_get_sensorpar();
            break;
        }
        case 3:
        {
            track_cust_tbt100_get_envpar();
            break;
        }
        case 4:
        {
            track_cust_tbt100_set_imei();
            break;
        }
        case 5:
        {
            value2 = track_fun_atoi(cmd->pars[2]);
            track_cust_tbt100_set_nextboot((kal_uint16)value2);
            break;
        }
        case 6:
        {
            track_cust_tbt100_get_baseversion();
            break;
        }
        case 7:
        {
            track_cust_tbt100_into_transport();
            break;
        }
        case 8:
        {
            value2 = track_fun_atoi(cmd->pars[2]);
            track_cust_tbt100_time_sync((kal_bool)value2, NULL);
            break;
        }
        case 9:
        {
            track_cust_tbt100_shutdown_req();
            break;
        }
        case 10:
        {
            track_cust_tbt100_get_bt_resettimes();
            break;
        }
        case 11:
        {
            track_cust_tbt100_get_bt_mac();
            break;
        }
        case 12:
        {
            track_cust_tbt100_get_bt_vbat();
            break;
        }
        case 13:
        {
            track_cust_tbt100_bt_reset();
            break;
        }
        case 14:
        {
            track_cust_tbt100_get_appversion();
            break;
        }
        case 15:
        {
            track_drv_sys_power_off_req();
            break;
        }
        case 16:
        {
            track_cust_tbt100_req_scan();
            break;
        }
        case 17:
        {
            track_cust_tbt100_get_sensoractive_times();
            break;
        }
        case 18:
        {
            track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_QUIT_TRANSPORT_ALM, KAL_FALSE, 0);
            break;
        }
        case 19:
        {
            track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_MAYBE_FAR, KAL_FALSE, 0);
            break;
        }
        case 20:
        {
            int i;
            memset(&simulation_lbs, 0, sizeof(LBS_Multi_Cell_Data_Struct));
            simulation_lbs.status = 1;
            if (cmd->part >= 2)
            {
                simulation_lbs.MainCell_Info.cell_id = (U16)track_fun_atoi(cmd->pars[2]);
            }
            for (i = 0; i < 6; i++)
            {
                if (cmd->part >= (i+3))
                {
                    simulation_lbs.NbrCell_Info[i].cell_id = (U16)track_fun_atoi(cmd->pars[i+3]);
                    simulation_lbs.valid_cel_num++;
                }
            }
            break;
        }
    }
}
#endif//__JM66__

#if defined(__MCU_LONG_STANDBY_PROTOCOL__)||defined(__JM66__)
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
#define MLSP_MAX_LEN 120
extern void track_cust_mlsp_completion(int no,unsigned char* data,int data_len);
extern kal_uint8* track_cust_mlsp_get_mcu_version(void);
#endif//__MCU_LONG_STANDBY_PROTOCOL__
#if defined(__JM66__)
#define MLSP_MAX_LEN 200
int track_cust_tbt100_completion(kal_uint8 cmd,kal_uint8* data,kal_uint16 data_len,kal_bool is_send_to_gsm);
#endif//__JM66__
static void cmd_tc_mcu_or_gsm(CMD_DATA_STRUCT *cmd)//向单片机/GSM 透传
{
    int len = 0;
    kal_uint8 value;
    kal_uint8 whole_data[MLSP_MAX_LEN] = {0};
    kal_uint8* data = NULL;

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "The number of arguments is wrong!");
        return;
    }
    track_fun_ASCII_to_HEX((unsigned char *)cmd->pars[1], &value, 1);
    if (value > 255)return;

    if (cmd->part > 1)
    {
        data = (kal_uint8*)(cmd->rcv_msg_source + (cmd->pars[2] - cmd->rcv_msg));
        LOGD(L_APP, L_V1, "%s",data);
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
        len = track_fun_ASCII_to_HEX(data, whole_data + 4, MLSP_MAX_LEN);
#elif defined(__JM66__)
        len = track_fun_ASCII_to_HEX(data, whole_data + 5, MLSP_MAX_LEN);
#endif//__MCU_LONG_STANDBY_PROTOCOL__
    }
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
    track_cust_mlsp_completion((int)value, whole_data, len);
#endif//__MCU_LONG_STANDBY_PROTOCOL__
    if (!strcmp(cmd->pars[0], "MCU"))
    {
#if defined(__JM66__)
        track_cust_tbt100_completion(value, whole_data, (kal_uint16)len, 0);
#endif//__JM66__
        track_cust_mlsp_set_return(&cmd->return_sms);
        track_drv_write_exmode(whole_data, len+7);
    }
    else
    {
#if defined(__JM66__)
        track_cust_tbt100_completion(value, whole_data, (kal_uint16)len, 1);
#endif//__JM66__
        track_cust_module_power_data(whole_data, len+7);
    }
}
#endif//__MCU_LONG_STANDBY_PROTOCOL__ 
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
extern U16 track_cust_mlsp_get_gsmboot_times(void);
static void cmd_MLSP_test(CMD_DATA_STRUCT * cmd)
{
    int value1 = 0,value2 = 0;
    if (!strcmp(cmd->pars[1], "SHUTDOWN"))//请求关机
    {
        if (cmd->part == 1)
        {
            track_cust_mlsp_shutdown_req();
        }
        else
        {
            if (!strcmp(cmd->pars[2], "A"))
            {
                track_cust_module_update_mode();//
                track_cust_mlsp_shutdown_req();
                sprintf(cmd->rsp_msg, "OK!");
            }
        }
    }
    else if (!strcmp(cmd->pars[1], "WBACKUP"))//写备份
    {
        track_cust_mlsp_write_backup_req(cmd->pars[2],strlen(cmd->pars[2]));
    }
    else if (!strcmp(cmd->pars[1], "BOOTTIMES"))//查询单片机记录的开启GSM次数
    {
        sprintf(cmd->rsp_msg, "MCU:%d,GSM:%d",track_cust_mlsp_get_gsmboot_times() - G_realtime_data.gsm_boot_times_offset, G_realtime_data.gsm_boot_times);
    }
    else if (!strcmp(cmd->pars[1], "RBACKUP"))//读备份
    {
        track_cust_mlsp_read_backup_req();
    }
    else if (!strcmp(cmd->pars[1], "SENSOR"))//读传感器信息
    {
        track_cust_mlsp_get_peripherals_status(0x20);
        track_cust_mlsp_set_return(&cmd->return_sms);
    }
    else if (!strcmp(cmd->pars[1], "TEARDOWN"))//读防拆键信息
    {
#if defined(__TEARDOWN_PHOTOSENSITIVE__)
        track_cust_mlsp_get_peripherals_status(0x08);
#elif defined(__TEARDOWN_KEY__)
        track_cust_mlsp_get_peripherals_status(0x02);
#else
#error
#endif//__TEARDOWN_PHOTOSENSITIVE__
        track_cust_mlsp_set_return(&cmd->return_sms);
    }
    else if (!strcmp(cmd->pars[1], "SETBOOT"))//设置开机时间
    {
        applib_time_struct time;
        time.nYear = track_fun_atoi(cmd->pars[2]);
        time.nMonth= track_fun_atoi(cmd->pars[3]);
        time.nDay= track_fun_atoi(cmd->pars[4]);
        time.nHour= track_fun_atoi(cmd->pars[5]);
        time.nMin= track_fun_atoi(cmd->pars[6]);
        time.nSec= track_fun_atoi(cmd->pars[7]);
        track_cust_mlsp_set_nextboot_time(&time);
        track_cust_mlsp_set_return(&cmd->return_sms);
    }
    else if (!strcmp(cmd->pars[1], "TEST"))//进入测试模式
    {
        track_cust_mlsp_test();
        sprintf(cmd->rsp_msg, "OK!");
    }
    else if (!strcmp(cmd->pars[1], "VERSION"))//查询单片机版本
    {
        sprintf(cmd->rsp_msg, "MCU_VERSION:%s",track_cust_mlsp_get_mcu_version());
    }
    else if (!strcmp(cmd->pars[1], "MCUTIME"))//查询单片机时间
    {
        extern applib_time_struct* track_cust_mlsp_get_mcu_time(void);
        applib_time_struct* mcu_time = track_cust_mlsp_get_mcu_time();
        sprintf(cmd->rsp_msg, "MCU_TIME:%d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", mcu_time->nYear, mcu_time->nMonth, mcu_time->nDay,
                mcu_time->nHour, mcu_time->nMin, mcu_time->nSec);
    }
    else if (!strcmp(cmd->pars[1], "FZ"))//辅助测试
    {
        if (cmd->part == 1)
        {
            LOGS("FZ:%s",ON_OFF(G_realtime_data.assist_sw));
            return;
        }
        if (!strcmp(cmd->pars[2], "ON"))
        {
            G_realtime_data.assist_sw = 1;
        }
        else if (!strcmp(cmd->pars[2], "OFF"))
        {
            G_realtime_data.assist_sw = 0;
        }
        else return;
        sprintf(cmd->rsp_msg, "OK!");
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    }
}
#endif/* __MCU_LONG_STANDBY_PROTOCOL__ */

#if defined(__INDIA__)
static void cmd_pwonalm(CMD_DATA_STRUCT * cmd)
{
    nvram_pwonalm_struct alarm = {0};

    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%d", cmd->pars[0],
                ON_OFF(G_parameter.pwonalm.sw), G_parameter.pwonalm.alarm_type);
        return;
    }
    if (!strcmp(cmd->pars[1], "ON"))
    {
        if (cmd->part < 1 || cmd->part > 2)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 1;
        if (cmd->part >= 2)
        {
            alarm.alarm_type = track_fun_atoi(cmd->pars[2]);
            if (alarm.alarm_type != 0 && alarm.alarm_type != 1 && alarm.alarm_type != 2 && alarm.alarm_type != 3)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (0/1/2/3)");
                return;
            }
        }
    }
    else if (!strcmp(cmd->pars[1], "OFF"))
    {
        if (cmd->part != 1)
        {
            sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
            return;
        }
        alarm.sw = 0;
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (ON/OFF)");
        return;
    }
    memcpy(&G_parameter.pwonalm, &alarm, sizeof(nvram_pwonalm_struct));
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    sprintf(cmd->rsp_msg, "OK!");
}
#endif /* __INDIA__ */

#if defined(__ATGM_UPDATE__)
static void cmd_ATGM(CMD_DATA_STRUCT *cmd)
{
    if (!strcmp(cmd->pars[1], "VERSION"))
    {
        sprintf(cmd->rsp_msg, "%s", track_cust_atgm_return_version());
    }
    else if (!strcmp(cmd->pars[1], "CLEARFAIL"))//清除失败次数
    {
        G_realtime_data.atgm_update_failtimes = 0;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        sprintf(cmd->rsp_msg, "OK!");
    }
    else if (!strcmp(cmd->pars[1], "CHECKFAIL"))//查看失败次数
    {
        if (track_cust_atgm_update_status(STATUS_CHECK) >= ATGM_UPDATE_STATUS_update_begin_req
                && track_cust_atgm_update_status(STATUS_CHECK) < ATGM_UPDATE_STATUS_update_done)
            sprintf(cmd->rsp_msg, "FAIL:%d/%d", G_realtime_data.atgm_update_failtimes - 1, ATGM_MAX_FAIL_TIMES);
        else
            sprintf(cmd->rsp_msg, "FAIL:%d/%d", G_realtime_data.atgm_update_failtimes, ATGM_MAX_FAIL_TIMES);
    }
    else if (!strcmp(cmd->pars[1], "FF"))//强制失败
    {
        track_cust_atgm_update_init();
        track_cust_atgm_update_fail(99);
    }
}

#endif /*__ATGM_UPDATE__*/

static void cmd_bmt(CMD_DATA_STRUCT *cmd)
{
    float value;
    kal_uint32 value1;
    if (cmd->part == 0)
    {
        value1 = track_cust_MCUrefer_adc_data(99);
        LOGD(L_APP,L_V5,"value1:%d",value1);
        value =((float)value1)/100;
        if (track_cust_refer_charge_data(99) == 1)
        {
            sprintf(cmd->rsp_msg, "VOLT:%0.2f,CHARGE IN",value);
        }
        else
        {
            sprintf(cmd->rsp_msg, "VOLT:%0.2f,NO CHARGE",value);
        }
        return;
    }
}

static void cmd_mcuver(CMD_DATA_STRUCT * cmd)
{
    if (cmd->part == 0)
    {
        extern kal_uint8* track_drv_get_420d_mcu_version(void);
        kal_uint8* ver=track_drv_get_420d_mcu_version();
        if (strlen(ver)==0)
        {
            sprintf(cmd->rsp_msg, "MCU_VERSION:NULL");
        }
        else
        {
            sprintf(cmd->rsp_msg, "MCU_VERSION:%s", ver);
        }
        return;
    }
}

#if defined(__GT420D__)
void cmd_MLSP_test(CMD_DATA_STRUCT * cmd)
{
    if (!strcmp(cmd->pars[1], "MCUTIME"))//查询单片机时间
    {
        extern applib_time_struct* track_cust_mlsp_get_mcu_time(void);
        applib_time_struct* mcu_time = track_cust_mlsp_get_mcu_time();
        sprintf(cmd->rsp_msg, "MCU_TIME:%d-%0.2d-%0.2d %0.2d:%0.2d", mcu_time->nYear, mcu_time->nMonth, mcu_time->nDay,
                mcu_time->nHour, mcu_time->nMin);
    }
}

void cmd_11(CMD_DATA_STRUCT * cmd)
{
    U8 buf[100] = {0};
    if (cmd->part == 0)
    {
        sprintf(buf, "AT^GT_CM=10,0,400,OK#");
        track_cust_module_power_data((kal_uint8*)buf, strlen(buf));
    }
    if (cmd->part == 1)
    {
        if (!strcmp(cmd->pars[1], "0"))
        {
            sprintf(buf, "AT^GT_CM=10,0#");
            track_cust_module_power_data((kal_uint8*)buf, strlen(buf));
        }
        if (!strcmp(cmd->pars[1], "1"))
        {
            sprintf(buf, "AT^GT_CM=10,1#");
            track_cust_module_power_data((kal_uint8*)buf, strlen(buf));
        }
    }

}
#endif

#if defined(__GT420D__)

static cmd_gsentype(CMD_DATA_STRUCT * cmd)
{
    kal_uint8 sensor_type;
    char *sensor_type_tab[] = {"Silan SC7A20","Bosch bma253", "Kionix kxtj3-1057", "Mira DA213",/*"mCube mc3410", "Micro afa750", "Freescale mma8452Q",*/ "MECH"};
    sensor_type = track_cust_refer_sensor_data(99);
    LOGD(L_APP,L_V5,"sensor_type:%d",sensor_type);
    if (cmd->part == 0)
    {
        if ((sensor_type <= SENSOR_NOT_READY) || (sensor_type >= SENSOR_TYPE_MAX))
        {
            sprintf(cmd->rsp_msg, "sensor type not detected");
        }
        else
        {
            sprintf(cmd->rsp_msg, "sensor type:%s", sensor_type_tab[sensor_type-1]);
        }
    }
    if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
}
#endif

void cmd_1(CMD_DATA_STRUCT * cmd)
{
    char buf[100] = {0};
    if (cmd->part == 1 && !strcmp(cmd->pars[1], "0"))
    {
        sprintf(buf, "AT^GT_CM=22,2,5,50,OK#\r\n");
    }
    else if (cmd->part == 1 && !strcmp(cmd->pars[1], "1"))
    {
        sprintf(buf, "AT^GT_CM=22,1,5,20,OK#\r\n");
    }
    else if (cmd->part == 1 && !strcmp(cmd->pars[1], "2"))
    {
        sprintf(buf, "AT^GT_CM=22,0,200,300,OK#\r\n");
    }
    else if (cmd->part == 1 && !strcmp(cmd->pars[1], "3"))
    {
        sprintf(buf, "AT^GT_CM=22,0,301,301#\r\n");
    }
    else if (cmd->part == 1 && !strcmp(cmd->pars[1], "4"))
    {
        sprintf(buf, "AT^GT_CM=22,3,%d,0,OK#\r\n",G_parameter.humiture_sw);
    }
    else if (cmd->part == 1 && !strcmp(cmd->pars[1], "5"))
    {
        sprintf(buf, "AT^GT_CM=05,04,01#\r\n");
    }
    else if (cmd->part == 1 && !strcmp(cmd->pars[1], "6"))
    {
        sprintf(buf, "AT^GT_CM=05,04,02#\r\n");
    }
    track_cust_module_power_data(buf, strlen(buf));
    LOGD(L_OBD, L_V5, "");
}

void cmd_sensortime(CMD_DATA_STRUCT * cmd)
{
    nvram_sensortime_struct sensortime = {0};
    kal_uint16 value = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    if (cmd->part > 4)
    {
        sprintf(cmd->rsp_msg, "Error: Number of parameters error!");
        return;
    }
    memcpy(&sensortime, &G_parameter.sensortime, sizeof(nvram_sensortime_struct));
    if (cmd->part == 1)
    {
        value = track_fun_atoi(cmd->pars[1]);
        LOGD(L_APP,L_V5,"value:%d",value);
        if (value != 1 && value != 2)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (1/2)");
            return;
        }
        if (!strcmp(cmd->pars[1], "1"))
        {
            sprintf(cmd->rsp_msg, "%s:1,%d,%d", cmd->pars[0],
                    G_parameter.sensortime.static_check_time,
                    G_parameter.sensortime.static_sensorcount);
            return;
        }
        else if (!strcmp(cmd->pars[1], "2"))
        {
            sprintf(cmd->rsp_msg, "%s:2,%d,%d", cmd->pars[0],
                    G_parameter.sensortime.exercise_check_time,
                    G_parameter.sensortime.exercise_sensorcount);
            return;
        }
        sensortime.modeset = value;
    }
    if (cmd->part > 1)
    {
        value = track_fun_atoi(cmd->pars[1]);
        if (value != 1 &&value != 2)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (1/2)");
            return;
        }
        sensortime.modeset = value;
        if (cmd->part >= 2)
        {
            value = track_fun_atoi(cmd->pars[2]);
            if (value < 10 ||value > 255)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 2 (10-255)");
                return;
            }
            if (sensortime.modeset == 1)
            {
                sensortime.static_check_time = value;
            }
            else if (sensortime.modeset == 2)
            {
                sensortime.exercise_check_time = value;
            }
        }
        if (cmd->part == 3)
        {
            value = track_fun_atoi(cmd->pars[3]);
            if (value < 1 ||value > 20)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 3 (1-20)");
                return;
            }
            if (sensortime.modeset == 1)
            {
                sensortime.static_sensorcount = value;
            }
            else if (sensortime.modeset == 2)
            {
                sensortime.exercise_sensorcount = value;
            }
        }
    }
    memcpy(&G_parameter.sensortime, &sensortime, sizeof(nvram_sensortime_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }
    if (G_parameter.sensortime.modeset == 1)
    {
        track_cust_send21((void *)6);
    }
    if (G_parameter.sensortime.modeset == 2)
    {
        track_cust_send21((void *)7);
    }
}

kal_uint8 track_cust_change_gps_speed(kal_uint8 arg)
{
    if (arg != 99)
    {
        gps_speed_change = arg;
        LOGD(L_APP,L_V5,"gps_speed_change:%d",gps_speed_change);
    }
    else
    {
        LOGD(L_APP,L_V5,"gps_speed_change:%d",gps_speed_change);
        return gps_speed_change;
    }
    }

void cmd_2(CMD_DATA_STRUCT * cmd)
{
    char buf[100] = {0};
    if (cmd->part == 1 && !strcmp(cmd->pars[1], "0"))
    {
        track_cust_change_gps_speed(0);
        }
    else if (cmd->part == 1 && !strcmp(cmd->pars[1], "1"))
    {
        track_cust_change_gps_speed(1);
    }
    else if (cmd->part == 1 && !strcmp(cmd->pars[1], "2"))
    {
    track_cust_gt420d_send_wifiorlbs(5,TR_CUST_POSITION_TIMING);
    }
    LOGD(L_OBD, L_V5, "");
}

void cmd_humiture(CMD_DATA_STRUCT * cmd)
{
    extern void track_cust_send22(void* arg);
    kal_uint8 value = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0],ON_OFF(G_parameter.humiture_sw));
        return;
    }
    if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "FAIL");
        return;
    }
    if (!strcmp(cmd->pars[1], "ON"))
    {
        G_parameter.humiture_sw = 1;
        }
    else if(!strcmp(cmd->pars[1], "OFF"))
    {
        G_parameter.humiture_sw = 0;
        }
    else
    {
        sprintf(cmd->rsp_msg, "FAIL");
        return;
        }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        track_cust_check_sethumiflag_flag(1);
        sprintf(cmd->rsp_msg, "SUCCESS");
        track_start_timer(TRACK_CUST_EXTCHIP_WAIT22_TIMER_ID, 1000, track_cust_send22, (void *)3);
        return;
    }
    else
    {
        sprintf(cmd->rsp_msg, "FAIL");
    }
}

void cmd_humiture_threshold(CMD_DATA_STRUCT * cmd)
{
    extern void track_cust_send22(void* arg);
    float value = 0 , value2 = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%f,%f", cmd->pars[0],G_parameter.temperature_threshold,G_parameter.humidity_threshold);
        return;
    }
    if (cmd->part > 2 )
    {
        sprintf(cmd->rsp_msg, "FAIL");
        return;
    }
    if(cmd->part >= 1)
    {
        value = track_fun_atof(cmd->pars[1]);
        if(value < 0.1 || value > 5.0)
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 1 (0.1-5)");
            return;
            }
        if(cmd->part > 1)
        {
            value2 = track_fun_atof(cmd->pars[2]);
            if(value2 < 0.1 || value2 > 5.0)
            {
                sprintf(cmd->rsp_msg, "Error: Parameter 1 (0.1-5)");
                return;
                }
                }
    }
    G_parameter.temperature_threshold = value;
    G_parameter.humidity_threshold = value2;
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "SUCCESS");
        track_cust_check_sethumiflag_flag(1);
        if(G_parameter.humiture_sw == 1)
        {
            track_cust_send22((void *)3);
        }
    }
    else
    {
        sprintf(cmd->rsp_msg, "FAIL");
    }
}

#if defined(__MQTT__)
static void cmd_transit(CMD_DATA_STRUCT *cmd)
{
    int value;
    nvram_transit_server_struct server = {0};
    if(cmd->part == 0)
    {

        sprintf(cmd->rsp_msg, "SERVER,%s,%d.%d.%d.%d:%d",ON_OFF(G_parameter.transit.IsTransit),
                G_parameter.transit.server_ip[0], G_parameter.transit.server_ip[1],
                G_parameter.transit.server_ip[2], G_parameter.transit.server_ip[3],
                G_importance_parameter_data.server.server_port);
        return;
    }
    value = track_fun_atoi(cmd->pars[CM_Par1]);
    if(value != 0 && value != 1)
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 1 (0/1)");
        return;
    }  
    server.IsTransit =value;
    if(track_fun_check_ip(cmd->pars[CM_Par2]))
    {
        memset(server.server_ip, 0, 4);
        if(!track_fun_asciiIP2Hex(cmd->pars[CM_Par2], (void*)&server.server_ip[0]))
        {
            sprintf(cmd->rsp_msg, "Error: Parameter 2 is not a valid IP address format!");
            return;
        }
    }  
    if(!track_fun_check_data_is_port(cmd->pars[CM_Par3]))
    {
        sprintf(cmd->rsp_msg, "Error: Parameter 3 (0~65535)");
        return;
    }
    server.server_port = track_fun_string2U16(cmd->pars[CM_Par3]);    
    memcpy(&G_parameter.transit,&server,sizeof(nvram_transit_server_struct));
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    }    
    delay_MQTT_main();
}

#endif

static void cmd_parameter(CMD_DATA_STRUCT *cmd)
{
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s,%s", cmd->pars[0],
                G_importance_parameter_data.mqtt_login.product_key,
                G_importance_parameter_data.mqtt_login.device_secret);
        return;
    }
}
static void cmd_iot(CMD_DATA_STRUCT *cmd)
{
    if(cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:mqtt_pk:%s,mqtt_ds:%s", cmd->pars[0],
                G_importance_parameter_data.mqtt_login.product_key,               
                G_importance_parameter_data.mqtt_login.device_secret
                );
        return;
    }
    if(strlen(cmd->pars[1])>0)
    {
        sprintf(G_importance_parameter_data.mqtt_login.product_key,"%s",cmd->pars[1]);      
    } 
    if(strlen(cmd->pars[2])>0)
    {
        sprintf(G_importance_parameter_data.mqtt_login.device_secret,"%s",cmd->pars[2]);      
    }
    Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
    MQTT_main();
}

void cmd_referstatus(CMD_DATA_STRUCT * cmd)
{
    kal_uint8 value = 0;
    if (cmd->part == 0)
    {
        return;
    }
    if (cmd->part > 2)
    {
        sprintf(cmd->rsp_msg, "FAIL");
        return;
    }
    if (!strcmp(cmd->pars[1], "1"))
    {
        track_cust_needsend_charge_data(1);
        }
    else if(!strcmp(cmd->pars[1], "2"))
    {
        track_cust_refer_keyin_data(0);
    }
    else if(!strcmp(cmd->pars[1], "3"))
    {
        track_cust_check_downalm_flag(98);
    }
    else if(!strcmp(cmd->pars[1], "4"))
    {
        track_cust_check_teardown_flag(98);
    }
    if (1)
    {
        sprintf(cmd->rsp_msg, "SUCCESS");
    }
    else
    {
        sprintf(cmd->rsp_msg, "FAIL");
    }
}

void cmd_anglerepspeed(CMD_DATA_STRUCT * cmd)
{
    kal_uint8 value = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%d",cmd->pars[0],G_parameter.anglerep_speed);
        return;
    }
    if (cmd->part == 1)
    {
        value = track_fun_atoi(cmd->pars[1]);
        G_parameter.anglerep_speed = value;
    }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "OK!");
    }
    else
    {
        sprintf(cmd->rsp_msg, "Error!");
    } 
    }

void cmd_statictime(CMD_DATA_STRUCT * cmd)
{
    extern void track_cust_send04(void* par);
    nvram_mode_statictime_struct statictime = {0};
    kal_uint16 value = 0;
    if (cmd->part == 0)
    {
        if (G_parameter.extchip.mode == 1)
        {
            sprintf(cmd->rsp_msg, "%s:%d,%d",cmd->pars[0],G_parameter.extchip.mode,G_parameter.statictime.mode1_statictime);
            return;
            }
        if (G_parameter.extchip.mode == 2)
        {
            sprintf(cmd->rsp_msg, "%s:%d,%d",cmd->pars[0],G_parameter.extchip.mode,G_parameter.statictime.mode2_statictime);
            return;
            }
    }
    memcpy(&statictime, &G_parameter.statictime, sizeof(nvram_mode_statictime_struct));
    if (cmd->part == 1)
    {
        if (!strcmp(cmd->pars[1], "1"))
        {
            sprintf(cmd->rsp_msg, "%s:%d,%d",cmd->pars[0],G_parameter.extchip.mode,G_parameter.statictime.mode1_statictime);
            return;
            }
        else if (!strcmp(cmd->pars[1], "2"))
        {
            sprintf(cmd->rsp_msg, "%s:%d,%d",cmd->pars[0],G_parameter.extchip.mode,G_parameter.statictime.mode2_statictime);
            return;
            }
        else
        {
            sprintf(cmd->rsp_msg, "FAIL");
            }
        return;
    }
    if(cmd->part == 2)
    {
        value = track_fun_atoi(cmd->pars[1]);
        if (value == 1)
        {
            value = track_fun_atoi(cmd->pars[2]);
            if(value < 1 || value > 1440)
            {
                sprintf(cmd->rsp_msg, "FAIL");
                return;
                }
            statictime.mode1_statictime = value;
            }
        else if (value == 2)
        {
            value = track_fun_atoi(cmd->pars[2]);
            if(value < 1 || value > 360)
            {
                sprintf(cmd->rsp_msg, "FAIL");
                return;
                }
            statictime.mode2_statictime = value;
            }
        else
        {
            sprintf(cmd->rsp_msg, "FAIL");
            return;
                }
            }
    memcpy(&G_parameter.statictime, &statictime, sizeof(nvram_mode_statictime_struct));
    if(G_parameter.extchip.mode == 1)
    {
        track_cust_send04((void *)G_parameter.extchip.mode);
        }
    if(G_parameter.extchip.mode == 2)
    {
        if(track_cust_gt420d_carstatus_data(99) == 2)
            {
                LOGD(L_APP,L_V5,"%d分钟后上传数据",G_parameter.statictime.mode2_statictime);
                tr_start_timer(TRACK_CUST_MODE2_KEEPGOING_TIMER_ID,G_parameter.statictime.mode2_statictime * 60 *1000,track_cust_module_update_mode);
            }
        }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "SUCCESS");
    }
    else
    {
        sprintf(cmd->rsp_msg, "FAIL");
    } 
    }

#if defined(__GT420D__)
/******************************************************************************
 *  Function    -  cmd_reset_system
 *
 *  Purpose     -  重启指令处理
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void cmd_reset_system(CMD_DATA_STRUCT * cmd)
{
    if(cmd->part == 0)
    {
        cmd->rsp_length = sprintf(cmd->rsp_msg, "The terminal will restart after 20sec!");   
        track_cust_restart(2, 20);
    }
    else if(cmd->part == 1 && strlen(cmd->pars[CM_Par1]) > 0)
    {
        int sec = atoi(cmd->pars[CM_Par1]);
        if(sec <= 0) track_cust_restart(2, 20);
        track_cust_restart(2, sec);
        sprintf(cmd->rsp_msg, "SUCCESS");
    }
    else
    {
        sprintf(cmd->rsp_msg, "FAIL");
    }
}
#endif

void cmd_sendpaketled(CMD_DATA_STRUCT * cmd)
{
    kal_uint8 value = 0;
    if (cmd->part == 0)
    {
        sprintf(cmd->rsp_msg, "%s:%s", cmd->pars[0],ON_OFF(G_parameter.sendpaketled_sw));
        return;
    }
    if (cmd->part > 1)
    {
        sprintf(cmd->rsp_msg, "FAIL");
        return;
    }
    if (!strcmp(cmd->pars[1], "ON"))
    {
        G_parameter.sendpaketled_sw = 1;
        }
    else if(!strcmp(cmd->pars[1], "OFF"))
    {
        G_parameter.sendpaketled_sw = 0;
        }
    else
    {
        sprintf(cmd->rsp_msg, "FAIL");
        return;
        }
    if (Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE))
    {
        sprintf(cmd->rsp_msg, "SUCCESS");
        return;
    }
    else
    {
        sprintf(cmd->rsp_msg, "FAIL");
    }
}

static cust_recv_struct cust_recv_table[] =
{
    #if defined(__MQTT__)
    {"TRANSIT",					cmd_transit},				//设置中转与否
    {"PARAMETERS",					cmd_parameter},				//参数请求
    {"IOT",					cmd_iot},				//设置IOT参数
    #endif

#if defined(__GT420D__)
    {"SENDPAKETLED",cmd_sendpaketled,},
    {"HUMITHR",  cmd_humiture_threshold,},
    {"RESET",        cmd_reset_system,},      //重启
    {"STATICTIME",  cmd_statictime,},
    {"ANGLEREPSPEED",  cmd_anglerepspeed,},
    {"REFERSTATUS",  cmd_referstatus,},
    {"HUMITURE",  cmd_humiture,},
    {"2",  cmd_2,},
    {"SENSORTIME",    cmd_sensortime,},
    {"GSENTYPE",    cmd_gsentype,},
    {"1",cmd_1,},
    {"MLSP",    cmd_MLSP_test,    },
    {"BMT",          cmd_bmt,},
    {"MCU_VERSION", cmd_mcuver},//单片机版本
#endif
#if defined(__ATGM_UPDATE__)
    {"ATGM",    cmd_ATGM,
    },
#endif /*__ATGM_UPDATE__*/

    {"SET_VER2",    cmd_set_version2,       },          // 软件版本
    {"BASEVERSION", cmd_get_base_version,   },          // 软件版本
    {"VERSION",     cmd_get_version,        },          // 软件版本查询
    {"NETLOG",      cmd_test_net_log,       },          // 临时测试使用
#if defined(__LUYING__)
    {"LY",      cmd_LY,
    },
    {"REC",      cmd_LY,             },
    {"SERVER4",   cmd_server4,      },          //录音上报服务器地址
#endif
#if defined(__DROP_ALM__)
    {"DROPALM", cmd_dropalm,
    },
#endif
#ifndef __XCWS__
    {"_DW",          cmd_dw,                 },          // 取中文地址
    {"_POSITION",    cmd_dw,                 },          //
    {"_123",         cmd_dw,                 },          //
    {"_DWXX",        cmd_dw,                 },          // 在线查询位置指令
    {"_WHERE",       cmd_dw,                 },          //
    {"_URL",         cmd_dw,                 },          //
    {"EURL",        cmd_eurl,               },          //
    {"LOCKIP",      cmd_lockip,             },          //
    {"QLOCKIP",     cmd_qlockip,            },          //
    {"SMSTC",       cmd_smstc,              },
#else
#if defined(__XCWS_TPATH__)
    {"SERVER3",     cmd_server3,
    },          // server3设置
    {"SER3SW",      cmd_ser3sw,             },          // server3设置
#endif /* 0 */
    {"ACCLT",       cmd_acclt,},          // 设防延时时间
#endif
    {"LANG",        cmd_lang,               },          //
    {"APN",         cmd_apn,                },          //
    {"ASETAPN",     cmd_asetapn,            },          //
    {"IMEI",        cmd_imei,               },          //
    {"IMSI",        cmd_imsi,               },          //
    {"ICCID",       cmd_iccid,              },          //
    {"VIEWIP",      cmd_viewip,             },          //
#if!(defined(__GT03D__) || defined(__GT300__) || defined(__GT500__) || defined(__GW100__)||defined(__GT300S__)||defined( __XCWS__)||defined(__GT420D__))
    {"LEDSLEEP",    cmd_ledsleep,
    },          //
    {"LEDSW",       cmd_ledsw,              },          //

#endif
#if defined(__GT500__) || defined(__ET200__) || defined(__V20__) || defined(__ET310__)||defined (__ET320__) || defined(__MT200__)
    {"LEDON",       track_cust_led_on,
    },          // LED灯控制
    {"LEDOFF",      track_cust_led_off,     },          // LED灯控制
    {"PS",          cmd_ps,                 },
#endif /* __GT500__ */         // LED灯控制
#if defined(__GT500__) || defined(__ET200__)||defined(__ET130__)||defined(__NT23__)|| defined(__V20__) || defined(__ET310__) || defined(__MT200__)||defined (__ET320__)||defined(__NT22__)||defined(__ET350__)
    {"DEFMODE",     cmd_defmode,
    },
#endif /* __GT500__ */

    {"SERVER1",     cmd_server,             },          //
    {"SERVER",      cmd_server,             },          //

    {"SERVER2",     cmd_server2,            },          //
    {"AGPS",        cmd_agps,               },          // APGS Server
    {"AMS",         cmd_ams,                },          // APGS Server
    {"AMSCONN",     cmd_amsconn,            },          // 设置每天连接AMS次数上限
    {"ASETGMT",     cmd_asetgmt,            },          // 时区自适应状态查询
    {"GMT",         cmd_gmt,                },          // 时区设置

    {"STATUS",      cmd_status,             },          //
#ifdef __GT02__
    {"PARAM",      gt02_param,              },          //
    {"DSERVER",      cmd_dserver,           },          //
    {"DSERVER2",      cmd_dserver,          },          //
#else
    {"PARAM",       cmd_param,              },          //
#endif /* __GT02__ */

#ifdef __GT02__
    {"SCXSZ",       gt02_scxsz,             },          //
#else
    {"SCXSZ",       cmd_scxsz,              },          //
#endif /* __GT02__ */

    {"GPRSSET",     cmd_gprsset,            },          //
    {"FACTORY",     cmd_factory,            },          //
    {"FACTORYALL",  cmd_factory_all,        },          //
    {"SOS",         cmd_sos,                },          // 三组SOS号码设置
    {"ALMREP",      cmd_almrep,             },          // 设置接收报警号码（除SOS报警）
    {"PWDSW",       cmd_pwdsw,              },          //
    {"PASSWORD",    cmd_password,           },          //
    {"RECOVER",     cmd_recover,            },          //
    //{"SPWD",        cmd_spwd,               },          //

#if !defined(__GT06A__)
#if defined(__ALM_SOS__)
    {"SOSALM",      cmd_sosalm,
    },          // SOS 开/关 模式 设置
#endif /* __ALM_SOS__ */

    {"DELAY",       cmd_delay,              },          // 延时监听

#endif /* __GT06A__ */
#if defined(__GFENCE__)
    {"GFENCE",      cmd_gfence,
    },          // 电子围栏设置
#elif defined(__FENCE__)
    {"FENCE",       cmd_fence,
    },          // 电子围栏设置
    {"SEEFENCE",    cmd_fence,              },          // 电子围栏设置
#endif /*  defined(__GT03D__)||defined(__GT300__) */
    {"BATALM",      cmd_batalm,             },          // 低电报警设置
#if defined(__GT420D__)
    {"LOWBAT",     cmd_lowbat,
    },
#endif
#if defined(__ET310__) || defined(__MT200__)||defined (__ET320__)||defined (__ET350__)
    {"SENSOR2",      cmd_sensor2,
    },          // 震动设置
#endif
    {"_GPRSON",      cmd_gprson,             },          //
    {"LINK",        cmd_link,               },          //
    {"HBT",         cmd_hbt,                },          // 心跳包设置间隔
#if defined(__GT03D__) || defined(__GT06D__) || defined(__GT300__) || defined(__GT500__)||defined(__GT300S__)||defined(__NT36__)||defined(__NT37__)
    {"REPORT",      cmd_report,
    },          // GPS 数据发送模式选择
#endif
#if !defined(__GT420D__)
    {"_BATCH",       cmd_batch,
    },          // GPS 数据打包发送设置
#endif

#if defined(__GT740__)||defined(__GT420D__)

#elif defined( __GT03D__)||defined(__GT300__) || defined(__GW100__)
    {"TIMER",       cmd_gt03_timer,
    },          // GPS 数据上传间?
#else
    {"TIMER",       cmd_timer,              },          // GPS 数据上传间隔
#endif
    {"ANGLEREP",    cmd_anglerep,           },          // 拐点补传设置
#if !(defined(__GT740__)||defined(__JM66__)||defined(__GT420D__))
    {"SENDS",       cmd_sends,
    },          // SENSOR 控制 GPS 时间
    {"RENEW",       cmd_RENEW,              },
    {"SIMALM",      cmd_simalm,             },          // 换卡报警
    {"CLEARSIM",    cmd_clearsim,           },          // 清除换卡报警
    {"ALMSMS",      cmd_almsms,             },          // 短信报警设置
    {"SMSCLEAR",    cmd_smsclear,           },          // 清空短信箱
    {"BALANCE",     cmd_balance,            },          // USSD String
    {"TQL",         cmd_tql,                },
    {"HST",         cmd_hst,                },          // 电动车限速阀值
    {"MONPERMIT",   cmd_monpermit,          },          // 监听权限控制
    {"PERMIT",      cmd_permit,             },          // 中心号码与邋SOS号码权限分配
    {"SENSOR",      cmd_sensor,             },          // 震动设置
    {"SENSORSET",   cmd_sensorset,          },          // 设置触发震动报警条件
    {"WN",          cmd_wn,                 },
    {"SENALM",      cmd_senalm,             },          // 震动报警设置
    {"SPEED",       cmd_speed,              },          // 超速提醒设置
    {"POWERALM",    cmd_poweralm,           },          // 断电报警设置
    {"ACCREP",      cmd_accrep,             },          // ACC 状态变化补传
    {"DISTANCE",    cmd_distance,           },          // GPS 数据定距上传
    {"PARK",        cmd_park,               },          // 停车超时提醒设置
    {"FDT",         cmd_fdt,                },          // 疲劳驾驶报警设置

#if!(defined(__GT03D__) || defined(__GT370__)|| defined (__GT380__) || defined(__GT500__)|| defined(__GT02__) || defined(__GW100__)||defined(__GT300S__) || defined (__NT33__))
    {"CENTER",      cmd_center,
    },          // 中心号码设置
#endif
#if !(defined(__GT300__) || defined(__GW100__)||defined(__GT300S__)|| defined(__GT370__)|| defined (__GT380__))
    {"SOSPERMIT",   cmd_sospermit,
    },          // 参数的查询设置权限，报警的发送号码个数
#endif
#if defined (__GT380__)
//nc
#else
    {"PWRLIMIT",    cmd_pwrlimit,           },          // 限制关机
#endif /* __GT380__ */
#if !defined(__BCA__)
    {"MILEAGE",     cmd_mileage,
    },          // 里程设置指令
#endif
#elif defined(__GT420D__)
    {"SIMALM",      cmd_simalm,             },          // 换卡报警
#endif/*! __GT740__ */

#if !(defined(__GT500__) ||defined(__GT300__) || defined(__GW100__)||defined(__GT300S__)|| defined(__GT370__)|| defined (__GT380__)||defined (__NT33__))
    {"DSRESET",     cmd_dsreset,
    },          // 重置设防状态指令
#endif
    {"CLEAR",       cmd_clear,              },          // 备份数据清除指令
    {"CLEARGPS",    cmd_cleargps,           },          // GPS星历清除指令
    {"SF",          cmd_sf,                 },          // 静态数据过滤开关
    {"GPSON",		cmd_gpson,				},
#if defined(__NT36__)||defined(__GT370__)|| defined (__GT380__)
    {"EPH",         cmd_eph,
    },          //
#endif
    {"CALL",        cmd_call,               },          // 电话重拨次数
#if!(defined(__GT03D__) || defined(__GT300__) || defined(__GW100__)||defined(__GT300S__))
    {"MOVING",      cmd_moving,
    },          // 位移报警设置
#endif
    {"GPRSALM",     cmd_gprsalm,            },          //
    {"STATICREP",   cmd_staticrep,          },          // SENSOR 检测静止上报最后位置开关指令
    {"FW",          cmd_fw,                 },          // 平台或SMS 短信透传短信内容

#if defined(__GT370__)|| defined (__GT380__)
    {"BOOTALM",     cmd_BOOTALM,
    },          //开机报警
    {"PWROFFALM",   cmd_PWROFFALM,          },          //关机报警
#elif defined(__ALM_BOOT__) && defined(__GW100__)
    {"PWRONALM",    cmd_PWRONALM,
    },          //儿童机开机报警设置
    {"PWROFFALM",   cmd_PWROFFALM,          },          //儿童机关机报警设置
#elif defined(__ALM_BOOT__)
    {"BOOTALM",     cmd_BOOTALM,
    },          //cmd_BOOTALM和cmd_PWRONALM函数一模一样  chengj
#endif

#if defined(__GT06D__)
    {"ACCALM",      cmd_accalm,
    },
#else
#if defined(__ACC_ALARM__)
    {"ACCALM",      cmd_accalm,
    },
#endif
    {"BLINDALM",    cmd_BLINDALM,           },          // GPS盲区
#endif /* __GT06D__ */

#if defined(__GT740__)||defined (__GT370__)||defined (__GT380__) ||defined (__JM81__)||defined(__GT420D__)
//nc
    /*非车载项目 -- chengjun  2017-08-23*/
#else
#if defined(__ET100__) && !defined(__GT100__)
    {"EBAT",        cmd_ebat,
    },            //放在外电检测宏多好  chengj
    //{"MOTORSET",    cmd_motorset,           },          // 锁电机参数设置
    //{"MOTOR1",      cmd_motor1,             },          // 锁电机参数设置
#elif !(defined(__GT06A__) ||defined(__XCWS__))
#if defined(__GERMANY__)
    {"STOP",       cmd_relay,
    },          // 断油电
    {"AUSGANG",       cmd_ausgang,              },          // 断油电
#endif
#if defined(__SPANISH__)
    {"APAGAR",       cmd_spanish_relay,
    },          // =RELAY,1#
    {"ENCENDER",       cmd_spanish_relay,              },          // =RELAY,0#
#endif
#if defined(__MEX__)
    {"SRELAY",       cmd_relay,
    },          // 断油电
#endif
#if defined(__NT51__)||defined (__NT37__)
    {"ERELAY",       cmd_relay,
    },          // 断油电
#endif
    {"RELAY",       cmd_relay,              },          // 断油电
    {"DYD",         cmd_dyd,                },          // 断油电（旧指令，因为网络指令用，暂不能移除）

#endif
#endif /* __GT740__ */


#if defined(__NO_MIC__)|| defined (__GT380__)||defined(__GT420D__)
    //no need
#else
    {"JT",          cmd_JT,    },          //
    {"MONITOR",     cmd_JT,                 },          //
    {"SLEEPSET",    cmd_SLEEPSET,           },          //
#endif


#if defined(__GT300S__)|| defined (__GT420D__)
    {"REMSET",		 cmd_remset,
    },
#elif defined (__GT370__)|| defined (__GT380__)
    {"TRACKTIME",    cmd_tracktime,
    },
    {"REMALMF",      cmd_remalmf          },    //开盖报警
#endif /* __GT300S__ */

#if defined(__GT740__)||defined(__GT300S__)||defined (__GT370__)|| defined (__GT380__)||defined(__GT420D__)
    {"MODE",        cmd_mode
    },          //
    {"QUERY",       cmd_mode_view,          },          //
    {"EXTCHIP",     cmd_extchip             },  //单片机数据透传发
    {"TEARDOWN",      cmd_teardown          },  //摘除报警
    {"REMALM",        cmd_teardown          },  //摘除报警
#if defined(__GT740__)||defined(__GT420D__)
    {"RESETPRO",		cmd_RESETPRO,
    },
    {"AMSLOG",		  cmd_amslog,	 },
    {"RESETBAT",	   cmd_RESETBAT,		  },		  //
    {"RESETBATDIS", 	  cmd_RESETBATDIS,			},			//
    {"GPSWAKE",          cmd_gpswakeup,            },
    {"REMALMSET",          cmd_remalmset,            },
    {"LBSSW",          cmd_lbssw,            },
    {"BATPTC",          cmd_BATPTC,            },
    {"BATCAL",			cmd_batcal, 		   },
#endif
#ifdef __GT420D__
    {"COLLIDE",			cmd_collide, 		   },
//{"TILTALM",			cmd_tiltalm, 		   },
#endif
#elif defined(__GT310__)
    {"MODE",        cmd_mode,
    },          //
#elif defined(__JM66__)
    {"ALMCANCEL",        cmd_jm66_mode,
    },          //
    {"MODE",        cmd_jm66_mode,      },          //
#else
    {"MODE",        cmd_work_mode_set,      },          //
#endif /* __GT740__ */

    //{"VMC",         cmd_set_vmc,            },        //重复 chengj
#if !defined(__GT420D__)
    {"LBS",         cmd_lbs,
    },
#if !defined(__BCA__)
    {"LBSON",       cmd_lbson,
    },
#endif /* __BCA__ */
#endif

#if defined(__GW100__)
    {"PERIOD",      cmd_period,
    },
#endif /* __GW100__ */
    {"SEEGFENCE",   cmd_seefence,           },
#if defined(__ET200__) || defined(__ET130__) || defined(__GT06D__)|| defined(__V20__) || defined(__ET310__)||defined (__ET320__)|| defined(__NT31__) || defined(__MT200__)||defined(__NT36__)||defined(__NT37__)
#if !defined( __XCWS__)
    {"PRELAY",      cmd_prelay,
    },          // 国贸定制需求，无条件断油电
#endif
#endif
    {"DIAL",        cmd_dial,               },          // 先能网络拨打电话
#if defined(__XCWS__)
    {"ZC",          cmd_ZC,
    },          // 设置注册信息
    {"SN",          cmd_set_device_setting, },          // 注册终端ID
#endif

#if defined(__EXT_VBAT_ADC__)
    {"EXBATALM",    cmd_exbatalm,
    },          // 外部电源低电报警
#if !defined(__MT200__)||defined(__CUST_ACC_BY_MCU__)
    {"EXBATCUT",    cmd_extbatcut,
    },          // 外部低电保护电压提醒
#else
    {"EXBATCUT",    cmd_pwrcut,          },          // 外部低电保护电压提醒
#endif
    {"EXBAT",       cmd_exbat,              },
    {"EXBAT2",       cmd_exbat2,              },

#if defined(__GT500__)
    {"EXTBATACC",   cmd_extbatacc,
    },          // ACC 电压检测设置
#endif /* __GT520__ */
#endif

    {"UART2TC",     cmd_Uart2_PassThrough   },          // UART2 透传
#if defined(__OBD__)
    { "OBDTC",   cmd_OBD_PassThrough_mode
    },          // OBD 透传
    {"OBDCMD",      cmd_OBD_cmd,            },          // OBD 透传
#endif /* __OBD__ */

#if defined(__EXT_VBAT_ADC__)
    {"ADC_BC",      cmd_adc_bc,
    },          // ADC补偿值
#endif /* __EXT_VBAT_ADC__ */
    {"_SYS",        cmd_sys,                },

#ifdef __GT741__

#elif defined (__GT02__) || defined(__GT740__)||defined(__GT420D__)
    {"SYS_VERSION", cmd_SYS_VERSION,
    },          //VERSION
#endif /* __GT02__ */

#if defined(__GT530__)
    { "MIFI",       cmd_mifi,
    },//设防延时时间
    {"DATA",       cmd_data,                 },//设防延时时间
#endif
#if defined(__433M__)
    {"TEST_433",      cmd_test_433,
    },//433测试控制
    {"SPKS",              cmd_433_spks,},//433远程喇叭寻车
    {"SPKDEF",          cmd_433_spkdef,},//433远程喇叭设防
    {"SPKALARM",      cmd_433_spkalarm,},//433远程喇叭报警
    {"SPKOFF",      cmd_433_spkoff,},//433远程喇叭报警
#endif /* __433M__ */

#if defined(__NT36__)||defined(__GT370__)||defined(__GT380__)||defined(__GT310__)||defined(__GT740__)||defined(__GT420D__)
    //没门
#else
    {"DOOR",       cmd_DOOR,                 },//边门检测       //NT37
    {"FIND",       cmd_FIND,                 },//声光寻车
    {"DOORALM",       cmd_dooralm,              },//边门检测
#endif

#if defined(__GT02F__) || defined(__ET310__)||defined (__ET320__) || defined(__GT240__) || defined(__MT200__)||defined(__GT380__)||defined(__NT37__)
    {"ADT",       cmd_adt,
    },
#endif
#if defined(__BCA__)
    {"RESET",      cmd_reset,
    },          // BCA【BCX】reset#专用
#endif
    {"CANCEL",   cmd_cancel,                },//撤销拆卸报警
    {"MSERVER",     cms_mserver,             },
    {"MSERVER1",    cmd_mserveri,             },
    {"MSERVER2",     cmd_mserveri,             },
    {"MSERVER3",     cmd_mserveri,             },
    {"MSERVER4",     cmd_mserveri,             },

#ifdef __USE_SPI__
    {"SPI_TC",       spi_tc,              },
    {"SPI_DATA",       cmd_spi_data,              },
    {"SPI_CMD", cmd_spi_cmd,},
    {"TRACK_STATUS",       cmd_track_status,    },
    {"SPI", cmd_spi,},
#endif /* __USE_SPI__ */

#if defined __CUSTOM_WIFI_FEATURES_SWITCH__
    //{"BATTEMP",       cmd_battemp,},
    {"WIFI_CMD",       cms_wificmd,              },
#endif
    {"CHECK",        cmd_check,              },
#ifdef __WIFI__
    {"WF",        cmd_WF,              },
    {"WIFION",        cmd_WIFION,              },
    {"FT",        cmd_FT,              },
#endif /* __WIFI__ */
#if defined(__FLY_MODE__) && !defined(__MT200__)
    {"FLYCUT",      cmd_flycut,
    },
#endif
    {"DWDEPO", cmd_dwdepo,                },
#if defined(__NT36__)||defined (__NT33__)||defined (__NT37__)||defined (__GT420D__)
    {"BAT",       cmd_bat,
    },
#endif
    {"UP_PARAM",       cmd_up_param,},

#if defined(__XCWS__)
    {"XLH",     cmd_xlh
    },
    {"XCWS",       cmd_xcws,},
#endif
#if defined (__NT31__)
    {"FRELAY",       cmd_frelay,
    },          // 断油电
#endif
    {"SNCOVERY",      cmd_sncovery,             },          // server3设置

#if defined(__BCA__)
    {"PW5",           cmd_set_pw5v,
    },
#endif /* __BCA__ */

#ifdef __NT31__
    {"CLEARALM",      cmd_clearalm,             },
#endif /* __NT31__ */
    {"FAKECELL",    cmd_fakecell,       },      //防伪基站使能

#if defined (__NT33__)||defined (__NT31__) || defined(__MT200__)||defined(__GT370__)|| defined (__GT380__)|| defined (__ET310__)|| defined (__NT37__)|| defined (__ET350__)
    {"GPSDUP",    cmd_gpsdup,
    },      //位置数据过滤开关
#endif

#if defined (__NT33__)|| defined (__NT31__)
#if defined (__CUST_BT__)
    {"BLE",         cmd_ble,
    },      //蓝牙
#endif /* __CUST_BT__ */
#endif

#if defined (__NT33__) || defined(__GT740__) ||defined (__NT31__)||defined(__GT420D__)
    {"FAKEBS",    cmd_fakebs,
    },      //防伪基站功能使能开关，功能同FAKECELL，但是不需要密码
#endif

    {"FCFALM",    cmd_fcfalm,       },      //防伪基站报警
#ifdef __GT300S__
    {"LJDW",    cmd_ljdw,       },
#endif /* __GT300S__ */

#if defined (__CUST_RS485__)
    {"RS485",    cmd_RS485,
    },
#endif
#if defined(__BACKUP_SERVER__)
    {"BSERVER",      cmd_Bserver,
    },          //
    {"BSCLEAR",      cmd_BsClear,             },          //清空副域名
#endif
#if defined(__SPANISH__)
    {"SOSBT",      cmd_sosbt,
    },
    {"ESOS",      cmd_esos,             },
    {"ITP",      cmd_itp,             },
#endif
#if defined(__SV_UPLOAD__)
    {"SV",          cmd_sv,
    },
    {"CXSV",        cmd_cxsv,               },
#endif /* __SV_UPLOAD__ */
    {"NBVER",        cmd_nbver,               },

#if defined (__CUSTOM_DDWL__)
    {"DORMANCY",	cmd_dormancy,
    },
#endif /* __CUSTOM_DDWL__ */

#if defined (__GT380__)
    {"SHUTDOWN",	cmd_shutdown,
    },
    {"CHARGEALM",	cmd_chargealm,			  },
#endif /* __GT380__ */
#if defined (__XYBB__)
    {"NT22_TEST",	cmd_nt22_test,
    },
    {"LOGIN",		cmd_login,				  },//注册信息
    {"CODE",		cmd_code,				  },//授权码
    {"ACC",		cmd_acc,				  },//ACC检测功能
#endif /* __XYBB__ */

#if defined (__OIL_DETECTION__)
    {"AIOIL",    cmd_aioil,
    },      //油量传感器
#endif

#if defined(__MULTIPLE_OUTPUT__)
    {"OUTSET", cmd_outset
    },
    {"OUTPUT", cmd_output},
    {"STOPOUT", cmd_stopout},
    {"RELAY2", cmd_relay2_output},

    {"OUTSET1", cmd_outset2},
    {"OUTPUT1", cmd_output2},
    {"STOPOUT1", cmd_stopout2},
    {"RELAY3", cmd_relay3_output},

    {"INCOUT", cmd_incout},
#endif

#if defined (__NT37__)
    {"INPUT",    cmd_input,
    },      //客制化报警短信内容
#endif
    {"GPSMODE", track_gpsmode},
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)||defined(__JM66__)
    {"MCU",    cmd_tc_mcu_or_gsm,
    },      //透传指令向单片机
    {"GSM",    cmd_tc_mcu_or_gsm,       },      //透传指令向GSM
#endif//__MCU_LONG_STANDBY_PROTOCOL__
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
    {"MLSP",    cmd_MLSP_test,
    },      //单片机超长待机协议测试指令
#endif//__MCU_LONG_STANDBY_PROTOCOL__
#if defined(__JM66__)
    {"TBT",    cmd_tbt_test,
    },      //tbt协议测试指令
    {"BLSSET",    cmd_blsset,    },      //
    {"BLBSET",    cmd_blbset,    },      //
    {"SENSWORK",    cmd_senswork,    },      //
    {"SENSORSET",    cmd_jm66_sensorset,    },      //
    {"BSENSOR",    cmd_bsensor,    },      //
    {"MTWORK",    cmd_mtwork,    },      //
    {"POWERSAVE",    cmd_powersave,    },      //
#endif//__JM66__
#if defined(__INDIA__)
    {"PWONALM",    cmd_pwonalm,
    },
#endif /* __INDIA__ */
    {" ", NULL}
};

/*****************************************************************************
*  Global Functions                     全局函数
******************************************************************************/
void track_cust_cmd_reply(cmd_return_struct * return_sms, char * data, int len)
{
    LOGD(L_APP, L_V6, "len:%d", len);
    if (return_sms->reply_character_encode == 0)
    {
        len = strlen(data);
        LOGD(L_APP, L_V6, "len:%d", len);
        if (len > 0)
        {
            if (return_sms->cm_type == CM_SMS)
            {
                track_cust_sms_send(return_sms->num_type, return_sms->ph_num, KAL_TRUE, data, len);
            }
            else if (return_sms->cm_type == CM_ONLINE)
            {
                #if defined(__DASOUCHE__) 
                track_cust_command_rsp(return_sms->requestId,KAL_TRUE,data,len);
#else                
                track_cust_paket_msg_upload(return_sms->stamp, KAL_TRUE, data, len);
#endif
            }
            else if (return_sms->cm_type == CM_ONLINE2)
            {
                track_cust_paket_FD_cmd(return_sms->stamp, KAL_TRUE, data, len);
            }
        }
        LOGS("%s", data);
    }
    else// if(return_sms->reply_character_encode == 1)
    {
        if (return_sms->cm_type == CM_SMS && len > 0)
        {
            track_cust_sms_send(return_sms->num_type, return_sms->ph_num, KAL_FALSE, data, len);
        }
        else if (return_sms->cm_type == CM_AT && len > 0)
        {
            LOGS("%s", data);
        }
        else if (return_sms->cm_type == CM_ONLINE && len > 0)
        {
            #if defined(__DASOUCHE__) 
            track_cust_command_rsp(return_sms->requestId,KAL_FALSE,data,len,NULL);
            #else             
            track_cust_paket_msg_upload(return_sms->stamp, KAL_FALSE, data, len);
            #endif
        }
        else if (return_sms->cm_type == CM_ONLINE2 && len > 0)
        {
            track_cust_paket_FD_cmd(return_sms->stamp, KAL_FALSE, data, len);
        }
    }
}

kal_bool track_cust_cmd(CMD_DATA_STRUCT * cmd)
{
    int i, size, ret;
    kal_bool find = KAL_FALSE, pass_status = KAL_FALSE;
    size = sizeof(cust_recv_table) / sizeof(cust_recv_struct);
    LOGD(L_APP, L_V6, "num_type:%d, len:%d, data:%s   [%d/%d]%s|%s|%s|%s|%s",
         cmd->return_sms.num_type, cmd->rcv_length, cmd->rcv_msg_source, cmd->part, size, cmd->pars[0], cmd->pars[1], cmd->pars[2], cmd->pars[3], cmd->pars[4]);

    if (cmd->rcv_length >= 1024)
    {
        return KAL_TRUE;
    }
    pass_status = track_cmd_check_authentication(cmd);
#if defined(__BCA__)
    if (cmd->flag == 1 && cmd->part != 0)
    {
        cmd->return_sms.num_type = 0xFF;
    }
#endif

    for (i = 0; i < size; i++)
    {
        ret = strcmp(cmd->pars[0], cust_recv_table[i].cmd_string);
        LOGD(L_APP, L_V9, "ret=%d|%s,%s|%p", ret, cmd->pars[0], cust_recv_table[i].cmd_string, cust_recv_table[i].cmd_string);
        if (!ret)
        {
            find = KAL_TRUE;
            cmd->rsp_length = 0;
            memset(cmd->rsp_msg, 0, sizeof(cmd->rsp_msg));
            track_is_sms_cmd(1);
            if (cmd->supercmd == 1 || cmd->return_sms.cm_type != CM_SMS || pass_status)
            {
                if (cust_recv_table[i].func_ptr != NULL)
                {
                    track_recv_cmd_status(1);
                    cust_recv_table[i].func_ptr(cmd);
#if defined(__JM66__)
                    if (Jm66_Current_mode == 2)
                    {
                        LOGD(L_APP, L_V4, "收到指令,更新2分钟关机定时器");
                        tr_start_timer(TRACK_CUST_KEY_DOWN_DELAY_SHUTDOWN_TIMER_ID, 120 * 1000, track_cust_jm66_shutdown);
                    }
#endif//__JM66__
                    break;
                }
            }
            else
            {
                LOGD(L_APP, L_V4, "Password error!");
                sprintf(cmd->rsp_msg, "Password error!");      //密码错误
            }
            break;
        }
    }
    if (find)
    {
        /*  指令解析执行体只填充信息，全部在此发出*/
        track_cust_cmd_reply(&cmd->return_sms, cmd->rsp_msg, cmd->rsp_length);
    }
    else
    {
        LOGD(L_APP, L_V5, "invalid command!");
    }
    return KAL_TRUE;        // 执行操作后删除短信
}

/******************************************************************************
 *  Function    -  track_execute_cust_cmd
 *
 *  Purpose     -  回复权限错误
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 11-10-2014,  ZengPing && Liujunwu  written
 * ----------------------------------------
 ******************************************************************************/
void track_execute_cust_cmd(CMD_DATA_STRUCT * gps_cmd)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 index = 0;
    int i, size, ret;
    kal_bool find = KAL_FALSE;
    struct_msg_send g_msg = {0};
    kal_bool isChar = KAL_TRUE;
    char *data;
    int len;
    static char *str_cmd_error_en = "Error: Not authorized!";
    static char str_cmd_error_cn[18] =
    {
        0x95, 0x19, 0x8B, 0xEF, 0xFF, 0x01, 0x97, 0x5E, 0x63, 0x88, 0x67, 0x43, 0x53, 0xF7, 0x78, 0x01, 0xFF, 0x01
    };
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOGD(L_APP, L_V5, "%d,%d,%s", gps_cmd->return_sms.cm_type, gps_cmd->rcv_length, gps_cmd->pars[CM_Main]);
    size = sizeof(cust_recv_table) / sizeof(cust_recv_struct);
    for (index = 0; index < size; index++)
    {
        if (!strcmp((S8*)cust_recv_table[index].cmd_string, gps_cmd->pars[CM_Main]))
        {
            find = TRUE;
            break;
        }
    }
    if (find)
    {

        snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_cmd_error_en);
        g_msg.msg_cn_len = 18;
        memcpy(g_msg.msg_cn, str_cmd_error_cn, g_msg.msg_cn_len);
        if (G_parameter.lang == 1)
        {
            isChar = KAL_FALSE;
            len = g_msg.msg_cn_len;
            data = g_msg.msg_cn;
        }
        else
        {
            isChar = KAL_TRUE;
            len = strlen(g_msg.msg_en);
            data = g_msg.msg_en;
        }
        if (g_msg.msg_cn_len)
        {
            /*  指令解析执行体只填充信息，全部在此发出*/
            if (gps_cmd->return_sms.cm_type == CM_SMS)
            {
                //track_cmd_sms_rsp(cmd);
                track_cust_sms_send(gps_cmd->return_sms.num_type, gps_cmd->return_sms.ph_num, isChar, data, len);
            }
            else if (gps_cmd->return_sms.cm_type == CM_ONLINE)
            {
                track_cust_paket_msg_upload(gps_cmd->return_sms.stamp, isChar, data, len);
            }
            else if (gps_cmd->return_sms.cm_type == CM_ONLINE2)
            {
                track_cust_paket_FD_cmd(gps_cmd->return_sms.stamp, isChar, data, len);
            }
            else
            {
                LOGS("%s", g_msg.msg_en);
            }
        }
    }
}
void track_cmd_xcws_factory(void)
{
    CMD_DATA_STRUCT cmd = {0};
    cmd_factory(&cmd);
}

