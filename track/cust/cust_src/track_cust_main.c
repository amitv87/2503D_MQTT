/*******************************************************************************************
 * Filename:
 * Author :
 * Date :
 * Comment:
 ******************************************************************************************/

/****************************************************************************
*  Include Files                        包含头文件
*****************************************************************************/
#include "Conversions.h"
#include "l4c2smsal_struct.h"

#include "track_cust.h"
#include "track_cust_main.h"
#include "track_at_call.h"
#include "track_at_sim.h"
#include "track_os_ell.h"
#include "track_version.h"

#include "l4c_common_enum.h"
#include "gps_struct.h"

/*****************************************************************************
*  Define                               宏定义
******************************************************************************/
#define PI    3.14159265358979
#define T_PI  0.01745329251994    /* PI/180.0 */
#define EARTH_RAD 6378.1370       /* 取WGS84标准参考椭球中的地球长半径(单位:km) */
#define MAX_STATIC_DIFFDATA	  0.03


/*****************************************************************************
*  Typedef  Enum                        枚举定义
*****************************************************************************/

/*****************************************************************************
*  Struct                               数据结构定义
******************************************************************************/

/*****************************************************************************
*  Local variable                       局部变量
*****************************************************************************/

/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/
#if defined(__GT03F__)|| defined(__GT310__) || defined(__GW100__)||defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
kal_bool g_GT03D = KAL_TRUE;
#else
kal_bool g_GT03D = KAL_FALSE;
#endif /* __GT03F__ */
#if defined(__GT420D__)
static kal_uint32 hour24_time = 0;
static kal_uint8 first_poweron = 1;
#endif
/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/
extern void track_testmode_eint_handler(U8 eint, U8 level);
extern void track_TM_autotest_call_status_change(kal_uint16 callType, kal_uint16 callState);
extern void track_soc_clear_dw_queue(void);
extern void track_cust_key_led_sleep(void);
extern void acc_status_change(void *arg);
#if defined(__OBD__)
extern char *track_obd_get_version(void);
#endif /* __OBD__ */
extern void track_cust_molude_simalarm_sms(void);
extern kal_int8 strtoime(const char *data, const char *time, applib_time_struct *t);
extern void track_drv_lte_reset_pwron(void* op);
extern void track_cust_intermittent_send_LBS(void *arg);
extern void track_cust_module_sw_init(void);
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
extern void track_cust_work_off(void);
#endif /* __GT300S__ */
#if defined(__GT740__)||defined(__GT420D__)
extern void track_cust_module_boot_cal();
extern void track_cust_module_cal_electricity(void* par);
extern void track_cust_send03(void *delay_sec);
extern void track_cust_module_sync_mcu_data();
extern void track_cust_module_delay_close(U8 sec);
extern void track_cust_module_status_inquiry();
extern kal_uint8 track_cust_modeule_get_wakemode();

#endif
extern kal_bool track_drv_nvram_write_switch(kal_uint8 sw);

#if defined (__MCU_SW_UPDATE__)
extern kal_uint8* track_get_mcu_sw_version(void);
#endif 

#if defined(__GT370__)|| defined (__GT380__)
extern void track_cust_simple_gsm_led_sleep(void);
#endif

#if defined (__CUSTOM_DDWL__)
extern void track_cust_dormancy_mode_check_wakeup(void);
extern kal_uint8 track_cust_dormancy_change_flight_mode(kal_uint8 enter);
#endif /* __CUSTOM_DDWL__ */

#if defined(__MULTIPLE_OUTPUT__)
extern void track_cust_output_init(void);
extern void track_cust_output2_init(void);

#endif

#if defined(__NETWORK_LICENSE__)
extern void track_cust_into_network_licenes_mode();
#endif

static kal_uint8 epo_status = 0;

/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/

/*****************************************************************************
*  Local Functions                      本地函数
******************************************************************************/
void track_cust_poweron_modeled_init(void);
/*****************************************************************************
*  Global Functions                     全局函数
******************************************************************************/
static kal_bool sendmessage = KAL_FALSE;
/*****************************************/
/*    OS -> APP                          */
/*****************************************/

kal_uint16 track_cust_check_firstpoweron_flag(kal_uint16 arg)
{
    if (arg == 1||arg == 0)
    {
        first_poweron = arg;
        LOGD(L_APP,L_V5,"first_poweron:%d",first_poweron);
    }
    else
    {
        LOGD(L_APP,L_V5,"first_poweron:%d",first_poweron);
        return first_poweron;
    }
}
/******************************************************************************
 *  Function    -  track_translate_gsm_4level
 *
 *  Purpose     -  GSM信号强度由[0-32]转换为4级
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
int track_translate_gsm_4level(int qdbm)
{
    int level = 0;
    if(qdbm == 99)
    {
        level = 0;
    }
    else
    {
        level = qdbm / 8;
    }
    return level;
}
/******************************************************************************
 *  Function    -  track_cust_get_rssi
 *
 *  Purpose     -  GSM信号强度上报函数
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
int track_cust_get_rssi(int arg)
{
    static int cur_rssi = 99;
    if(arg != -1)
    {
        if(arg <= 15)
        {
            LOGD(L_APP, L_V1, "请检查GSM天线，信号偏弱 %d -> %d", cur_rssi, arg);
        }
        cur_rssi = arg;
    }
    else
    {
        LOGD(L_CMD, L_V9, "cur_rssi:%d, %d", cur_rssi, arg);
    }

    return cur_rssi;
}

/******************************************************************************
 *  Function    -  track_cust_get_rssi_level
 *
 *  Purpose     -  对GSM信号强度分等级
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
int track_cust_get_rssi_level(void)
{
    int tmp, ret;
    tmp = track_cust_get_rssi(-1);
    LOGD(L_APP, L_V6, "rssi=%d", tmp);
    if(tmp == 99)
        ret = -1;
    else if(tmp == 0)
        ret = 0;
    else if(tmp <= 9)
        ret = 1;
    else if(tmp <= 12)
        ret = 2;
    else if(tmp <= 15)
        ret = 3;
    else
        ret = 4;
    return ret;
}

/******************************************************************************
 *  Function    -  track_cust_at_simulation_online_cmd
 *
 *  Purpose     -  在线指令处理（暂时未从这里处理）
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_at_simulation_online_cmd(U8* data, int len, kal_uint32 no)
{
    LOGD(L_APP, L_V5, "not support!");
#if defined(__XCWS__)
    track_soc_write_req(data, len, 0, 0);
#endif
}
/******************************************************************************
 *  Function    -  track_cust_at_cmd
 *
 *  Purpose     -  AT指令串口函数
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_bool track_cust_at_cmd(CMD_DATA_STRUCT *cmd)
{
    return track_cust_cmd(cmd);
}
kal_uint8 track_is_sms_cmd(kal_uint8 status)
{
    static kal_uint8 cmd_status = 0;
    if(status == 99)
    {
        return cmd_status;
    }
    cmd_status = status;
}
kal_bool track_cust_message_status()
{
    return sendmessage;
}
/******************************************************************************
 *  Function    -  track_app_receive_sms
 *
 *  Purpose     -  对接收到的短信进行号码处理
 *
 *  Description -
 *                 识别飞信来源
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_app_receive_sms(track_sms_param *sms_rev)
{
    char *p = NULL,*q=NULL,*s=NULL;
    CMD_DATA_STRUCT sms_cmd = {0};

    if(sms_rev == NULL)
    {
        return;
    }
    sms_cmd.return_sms.cm_type = CM_SMS;
    sms_cmd.return_sms.num_type = sms_rev->P_TYPE;
    track_is_sms_cmd(0);
    memcpy(&sms_cmd.return_sms.ph_num[0], sms_rev->TPA, sms_rev->ODAL);
    /*at指令不大于90*/
    if(sms_rev->TP_DCS == GSM_7BIT || sms_rev->TP_DCS == GSM_8BIT)
    {
        LOGD(L_CMD, L_V5, "TYPE:%d, TP_DCS:%d, TP_UDL:%d, CTX:%s, NUM:%s", sms_rev->P_TYPE, sms_rev->TP_DCS, sms_rev->TP_UDL, sms_rev->TP_UD, sms_rev->TPA);
        if(strstr(sms_rev->TPA, "12520")) //飞信号码
        {
            p = strstr(sms_rev->TP_UD, ": ");
        }
#if 1
			q = strstr(sms_rev->TP_UD, "#");
			s = strstr(sms_rev->TP_UD, "$");
#endif /* __ZYT__ */

        if(p)
        {
            p += 2;
            sms_cmd.rcv_length = sms_rev->TP_UDL - (p  - sms_rev->TP_UD);
            memcpy(sms_cmd.rcv_msg, p, sms_cmd.rcv_length);
        }
#if 1
        else if(q > 0)
        {
            LOGD(L_APP, L_V5, " IMSI = %s", track_drv_get_imsi(0));
            LOGD(L_APP, L_V5, " q  = %d,TP_UD = %d", q, sms_rev->TP_UD);
			if(s == NULL)
			{
				s = (kal_uint8 *)sms_rev->TP_UD;
			}
			else
			{
				if(s[1] == 0x20)
				{
					s += 2;
				}
				else
				{
					s += 1;
				}
			}
            q += 1;

            sms_cmd.rcv_length = q  - s;
            LOGD(L_APP, L_V5, " TP_UDL = %d,rcv_length = %d", sms_rev->TP_UDL, sms_cmd.rcv_length);
            memcpy(sms_cmd.rcv_msg, s, sms_cmd.rcv_length);
        }
        else
        {
            sms_cmd.rcv_length = sms_rev->TP_UDL;
            memcpy(sms_cmd.rcv_msg, sms_rev->TP_UD, sms_rev->TP_UDL);
        }
#else
        else
        {
            sms_cmd.rcv_length = sms_rev->TP_UDL;
            memcpy(sms_cmd.rcv_msg, sms_rev->TP_UD, sms_rev->TP_UDL);
        }
#endif /* __ZYT__ */
        LOGD(L_APP, L_V5, "%d:cmd:%s,num:%s", sms_cmd.rcv_length, sms_cmd.rcv_msg, sms_cmd.return_sms.ph_num);
    }
    else if(sms_rev->TP_DCS == GSM_UCS2)
    {
        kal_uint8 wchr3[2] = {0xFF, 0x0C};  // ，
        kal_uint8 wchr4[2] = {0x00, 0x2C};  // ,
        kal_uint8 wchr5[2] = {0xFF, 0x03};  // ＃
        kal_uint8 wchr6[2] = {0x00, 0x23};  // #
        kal_uint8 wchr7[2] = {0xFF, 0x04};  // ＄
        kal_uint8 wchr8[2] = {0x00, 0x24};  // $
        kal_uint8 *start, *end;
        LOGD(L_APP, L_V4, "来短信(%d,%d):%s", sms_rev->P_TYPE, sms_rev->TP_UDL, sms_rev->TPA);
        LOGH(L_APP, L_V6, sms_rev->TP_UD, sms_rev->TP_UDL);
        track_fun_str_replace_chr(wchr3, wchr4, sms_rev->TP_UD, sms_rev->TP_UDL);
        track_fun_str_replace_chr(wchr5, wchr6, sms_rev->TP_UD, sms_rev->TP_UDL);
        track_fun_str_replace_chr(wchr7, wchr8, sms_rev->TP_UD, sms_rev->TP_UDL);
        if(strstr(sms_rev->TPA, "12520") == sms_rev->TPA)   //飞信号码
        {
            kal_uint8 wchr1[2] = {0x00, 0x3A};  // :
            kal_uint8 wchr2[2] = {0x00, 0x23};  // #
            start = track_fun_strWchr(wchr1, sms_rev->TP_UD, sms_rev->TP_UDL);
            if(start == NULL)
            {
                LOGD(L_APP, L_V5, "error 1");
                track_fun_view_UTF16BE_string(sms_rev->TP_UD, sms_rev->TP_UDL);
                start = (kal_uint8 *)sms_rev->TP_UD;
            }
            else
            {
                if(start[2] == 0x00 && start[3] == 0x20) start += 4;
                else start += 2;
            }
            end = track_fun_strWchr(wchr2, start, sms_rev->TP_UDL - (start - sms_rev->TP_UD));
            if(end == NULL)
            {
                track_fun_view_UTF16BE_string(sms_rev->TP_UD, sms_rev->TP_UDL);
                track_cust_fetion_auto_authorize(sms_rev); // 飞信自动授权，测试用
                track_cust_sms_relay(sms_rev);
                return;
            }
            end += 2;
            LOGH(L_APP, L_V6, start, end - start);
            track_fun_UTF16BE_to_GB2312(start, end - start, sms_cmd.rcv_msg, CM_PARAM_LONG_LEN_MAX);
            sms_cmd.rcv_length = strlen(sms_cmd.rcv_msg);
            LOGH(L_APP, L_V6, sms_cmd.rcv_msg, CM_PARAM_LONG_LEN_MAX);
        }
#if 1
		else
		{
			start = track_fun_strWchr(wchr8, sms_rev->TP_UD, sms_rev->TP_UDL);
			if(start == NULL)
			{
				LOGD(L_APP, L_V5, "error 1");
				track_fun_view_UTF16BE_string(sms_rev->TP_UD, sms_rev->TP_UDL);
				start = (kal_uint8 *)sms_rev->TP_UD;
			}
			else
			{
				if(start[2] == 0x00 && start[3] == 0x20) start += 4;
				else start += 2;
			}

			end = track_fun_strWchr(wchr6, start, sms_rev->TP_UDL - (start - sms_rev->TP_UD));
			if(end != NULL)
			{
				end += 2;
				track_fun_UTF16BE_to_GB2312(start, end - start, sms_cmd.rcv_msg, CM_PARAM_LONG_LEN_MAX);
			}
			sms_cmd.rcv_length = strlen(sms_cmd.rcv_msg);
			LOGH(L_APP, L_V6, sms_cmd.rcv_msg, CM_PARAM_LONG_LEN_MAX);
		}
#else
		else
		{
			track_fun_UTF16BE_to_GB2312(sms_rev->TP_UD, sms_rev->TP_UDL, sms_cmd.rcv_msg, CM_PARAM_LONG_LEN_MAX);
			sms_cmd.rcv_length = strlen(sms_cmd.rcv_msg);
			//LOGD(L_APP, L_V4, "发送短信内容：|%s|", sms_cmd.rcv_msg);
			LOGH(L_APP, L_V6, sms_cmd.rcv_msg, strlen(sms_cmd.rcv_msg));
		}
#endif /* __ZYT__ */

        sms_cmd.character_encode = 1;
    }
    else
    {
        LOGD(L_CMD, L_V5, "TP_DCS:%d, TP_UDL:%d, NUM:%s", sms_rev->TP_DCS, sms_rev->TP_UDL, sms_rev->TPA);
        LOGH(L_CMD, L_V5, sms_rev->TP_UD, sms_rev->TP_UDL);
    }
    if(sms_cmd.rcv_length)
    {
        track_command_operate(&sms_cmd);
    }

    track_cust_sms_relay(sms_rev);

}

/******************************************************************************
 *  Function    -  track_cust_gps_decode_done
 *
 *  Purpose     -  GPS数据接口
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_gps_decode_done(track_gps_data_struct *track_gps_data)
{
    track_gps_locate_data_receive(track_gps_data);
}

/******************************************************************************
 *  Function    -  track_cust_eint_trigger_response
 *
 *  Purpose     -  中断响应接口
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_eint_trigger_response(U8 eint, U8 level)
{
    if(track_is_testmode())
    {
        track_testmode_eint_handler(eint, level);
    }
    else if(track_is_autotestmode())
    {
        track_autotestmode_eint_handler(eint, level);
    }
    else
    {
        track_cust_status_eint_check_set(eint, level);
    }
}
#if defined(__NT51__)
void trak_cust_mcuextvolt_convey(kal_uint32 ext_volt)
{
    track_cust_chargestatus_vbtstatus_check(PMIC_VBAT_STATUS,99,ext_volt);
}
#endif
/******************************************************************************
 *  Function    -  track_cust_chargestatus_vbtstatus_check
 *
 *  Purpose     -  电源状态信息上报
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_chargestatus_vbtstatus_check(pmic_status_enum status, battery_level_enum level, kal_uint32 volt)
{
    if(track_is_testmode() || track_is_autotestmode())
    {
        LOGD(L_APP, L_V7, "status=%d, level=%d, volt=%d", status, level, volt);
    }
    else
    {
        track_cust_status_chargestatus_vbtstatus_check(status, level, volt);
    }
}

/******************************************************************************
 * FUNCTION:  - track_cust_call_status_change
 *
 * DESCRIPTION: - 电话状态切换
    switch (callState)
    {
        case TR_CALL_SETUP:  // 来电 /
            break;
        case TR_CALL_RING: //响铃/
            break;
        case TR_CALL_CONNECT:// 接通 /
            break;
        case TR_CALL_MT_BUSY://对方忙/
            break;
        case TR_CALL_NULL_NO://号码为空/
            break;
        case TR_CALL_DISCONNECT:  // 挂断 //
            break;
        default:
            break;
    }
 *
 * modification history
 * --------------------
 * v1.0  , 14-11-2012,  wangqi  written
 * --------------------
 ******************************************************************************/
void track_cust_call_status_change(void * msg)
{
    call_state_struct *ptr = (call_state_struct *) msg;
    kal_uint16 _type = ptr->type;/*call_type_enum*/
    kal_uint16 callState = ptr->status;/*track_call_state_enum*/
    LOGD(L_APP, L_V5, "type:%d,status:%d", ptr->type, ptr->status);

    if(track_is_testmode())
    {
        track_TM_call_status_change(_type, callState);
    }
    else if(track_is_autotestmode() == KAL_TRUE)
    {
        track_TM_autotest_call_status_change(_type, callState);
    }
    else
    {
        track_cust_sos_call_status(_type, callState);
    }
}

/******************************************************************************
 *  Function    -  track_cust_sos_and_center_permit
 *
 *  Purpose     -  SOS、中心号码的指令控制
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 20-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_bool track_cust_sos_and_center_permit(char *num)
{
    if(G_parameter.Permission.SOSPermit == 0)
    {
        LOGD(L_APP, L_V5, "1");
        return KAL_TRUE;
    }
    if(track_cust_sos_or_center_check(3, num) == 3)
    {
        LOGD(L_APP, L_V5, "2");
        return KAL_FALSE;
    }
    LOGD(L_APP, L_V5, "3");
    return KAL_TRUE;
}

/******************************************************************************
 *  Function    -  track_cust_password_sw
 *
 *  Purpose     -  指令密码控制函数
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_bool track_cust_password_sw(void)
{
    if(G_parameter.Permission.password_sw)
    {
        return KAL_TRUE;
    }
    else
    {
        return KAL_FALSE;
    }
}

/******************************************************************************
 *  Function    -  track_cust_password_check
 *
 *  Purpose     -  指令密码检查函数
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint8 track_cust_password_check(char *password)
{
    kal_uint8 ret = 0;
#if defined(__GT02__)
    if(!strcmp(password, G_parameter.Permission.superCmd))
    {
        ret |= 2;
    }
    else
#endif /* __GT02__ */
    {
        if(!strcmp(password, G_parameter.Permission.password))
        {
            ret |= 1;
        }
    }
    return ret;
}

kal_bool track_cmd_super_cmd(CMD_DATA_STRUCT *command)
{
    char super_cmd[20] = {0};
    int len;

    len = strlen(command->pars[0]);
    if(len >= 20)
    {
        return KAL_FALSE;   // 验证超级指令头
    }
    memcpy(super_cmd, command->rcv_msg_source + (command->pars[0] - command->rcv_msg), len);
    if(!strcmp(super_cmd, G_parameter.Permission.superCmd))
    {
        return KAL_TRUE;
    }
    return KAL_FALSE;
}
/******************************************************************************
 *  Function    -  track_app_sms_rsp
 *
 *  Purpose     -  短信发送回执
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_app_sms_rsp(sms_send_rsp_struct *rsps)
{
    LOGD(L_APP, L_V5, "短信发送反馈 index:%d,status:%d,result:%d,at:%d",
         rsps->index, rsps->status, rsps->result, track_get_vs_at_status());
}


/******************************************************************************
 *  Function    -  track_cust_agps_valid
 *
 *  Purpose     -  AGPS状态
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_bool track_cust_agps_valid(kal_uint8 par)
{
    if(par == 1)
    {
        return track_cust_apgs_sw_status();
    }
    else if(par == 2)
    {
        return G_parameter.agps.last_position_valid;
    }
}
static float track_fun_get_GPS_Points_distance(double lat1, double lon1, double lat2, double lon2)
{
    double radLat1, radLat2, aLat, aLon, s;
    float ret;
    LOGD(L_APP, L_V5, "%f,%f,%f,%f", lat1, lon1, lat2, lon2);

    radLat1 = lat1 * T_PI;
    radLat2 = lat2 * T_PI;

    aLat = radLat1 - radLat2;       //纬度之差
    aLon = (lon1 - lon2) * T_PI;    //经度之差

    s = 2 * asin(sqrt(pow(sin(aLat / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(aLon / 2), 2)));
    s = s * EARTH_RAD * 1000;
    ret = s;
    return ret;
}
/******************************************************************************
 *  Function    -  track_cust_agps_get_last_position
 *
 *  Purpose     -  供AGPS使用的上一个定位点
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_agps_get_last_position(kal_uint8 type, mtk_param_epo_pos_cfg *epo_pos_cfg)
{
    static mtk_param_epo_pos_cfg epo_pos_cfg_from_lbs = {0};
    float  distance;
    track_gps_data_struct *gpsLastPoint = NULL;
    mtk_param_epo_pos_cfg epo_pos_cfg1;
    gpsLastPoint = track_cust_backup_gps_data(0, NULL);
    /*
    if(gpsLastPoint->gprmc.status != 0 && type==2)
    {
        memcpy(&epo_pos_cfg1, epo_pos_cfg, sizeof(mtk_param_epo_pos_cfg));
        LOGD(L_APP, L_V5, "%f,%f,%f,%f", epo_pos_cfg1.dfLAT,epo_pos_cfg1.dfLON, gpsLastPoint->gprmc.Latitude, gpsLastPoint->gprmc.Longitude);
        distance = track_fun_get_GPS_Points_distance(epo_pos_cfg1.dfALT,epo_pos_cfg1.dfLON, gpsLastPoint->gprmc.Latitude, gpsLastPoint->gprmc.Longitude);
        LOGD(L_APP, L_V5, "LBS与GPS距离相差=%f", distance);
    }
    */
    if(type == 1)
    {
        LOGD(L_APP, L_V5, "track_cust_backup_gps_data=%d", gpsLastPoint->gprmc.status);
        if(gpsLastPoint->gprmc.status != 0)
        {
            epo_pos_cfg->dfALT = gpsLastPoint->gpgga.msl_altitude;
            if(gpsLastPoint->gprmc.NS == 'S')
            {
                epo_pos_cfg->dfLAT = -gpsLastPoint->gprmc.Latitude;
            }
            else
            {
                epo_pos_cfg->dfLAT = gpsLastPoint->gprmc.Latitude;
            }
            if(gpsLastPoint->gprmc.EW == 'W')
            {
                epo_pos_cfg->dfLON = -gpsLastPoint->gprmc.Longitude;
            }
            else
            {
                epo_pos_cfg->dfLON = gpsLastPoint->gprmc.Longitude;
            }
            epo_pos_cfg->u2YEAR  = gpsLastPoint->gprmc.Date_Time.nYear;
            epo_pos_cfg->u1MONTH = gpsLastPoint->gprmc.Date_Time.nMonth;
            epo_pos_cfg->u1DAY = gpsLastPoint->gprmc.Date_Time.nDay;
            epo_pos_cfg->u1HOUR = gpsLastPoint->gprmc.Date_Time.nHour;
            epo_pos_cfg->u1MIN = gpsLastPoint->gprmc.Date_Time.nMin;
            epo_pos_cfg->u1SEC = gpsLastPoint->gprmc.Date_Time.nSec;
        }
        else
        {
            memcpy(epo_pos_cfg, &epo_pos_cfg_from_lbs, sizeof(mtk_param_epo_pos_cfg));
        }
    }
    else if(type == 2)
    {
        memcpy(&epo_pos_cfg_from_lbs, epo_pos_cfg, sizeof(mtk_param_epo_pos_cfg));
    }
}

/******************************************************************************
 *  Function    -  track_cust_epo_int
 *
 *  Purpose     -  epo_int是秒定注入必须执行函数，不然在拿到时间后并不会注入EPO
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 28-06-2016,  xzq  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_epo_int()
{
    applib_time_struct currTime = {0};
    applib_dt_get_rtc_time(&currTime);

    LOGD(L_GPS, L_V2, "%0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d   %d",
         currTime.nYear, currTime.nMonth, currTime.nDay,
         currTime.nHour, currTime.nMin, currTime.nSec, (*OTA_DEFAULT_HARDWARE_YEAR + 2000));
    if(currTime.nYear != (*OTA_DEFAULT_HARDWARE_YEAR + 2000) && epo_status == 0)
    {
        epo_status = 1;
        tr_stop_timer(TRACK_CUST_EPO_INT_TIMER_ID);
        if(track_cust_apgs_sw_status() == 1)
        {
            ilm_struct *send_ilm;
            gps_assist_ind_struct *myMsgPtr = NULL;
            myMsgPtr = (gps_assist_ind_struct*) construct_local_para(sizeof(gps_assist_ind_struct), TD_RESET);
            myMsgPtr->account_id = Socket_get_account_id();
            track_fun_msg_send(MSG_ID_ENABLE_EPO_DOWNLOAD, myMsgPtr, MOD_MMI, MOD_GPS);// 防止开机开GPS时因为系统时间复位导致将EPO错误删除 xzq
            LOGD(L_GPS, L_V2, "account id %d", myMsgPtr->account_id);
        }
    }
    else if(currTime.nYear == (*OTA_DEFAULT_HARDWARE_YEAR + 2000))
    {
        tr_start_timer(TRACK_CUST_EPO_INT_TIMER_ID, 5000, track_cust_epo_int);
    }
}

track_cust_active_epoint()
{
	
	LOGD(L_GPS, L_V2, "epo_status %d", epo_status);
	if(epo_status==0 && G_parameter.extchip.mode==1)
	{
		track_cust_epo_int();
	}
}

void track_cust_poweron_open_gps()
{
    //开机开启GPS
#if defined(__GW100__) ||defined(__V20__)//test 20150521
    if(!track_is_testmode() && ! track_is_autotestmode())
    {
        track_cust_gps_control(0);
    }
#elif defined(__GT310__)
    {
        //模式切换
        if(G_parameter.work_mode.sw == 1)
        {
            if(track_cust_get_work_mode() != WORK_MODE_3)
            {
                if(track_cust_get_work_mode() == WORK_MODE_1)
                {
                    track_drv_sensor_off();
                }
                // gps on
                track_cust_open_location_mode(2);
            }
            else
            {
                track_drv_sensor_off();
                track_cust_fly_mode_change((void*)43);
            }
        }
        else
        {
            // gps on
            track_cust_gps_work_req((void *)2);
        }
    }
#elif defined(__ET310__)
    static kal_bool def_tmp = 0;
    if(def_tmp == 0 && G_realtime_data.defense_mode == 1)   /* 手动设防模式 */
    {
        def_tmp = 1;
        if(G_realtime_data.defences_status >= 2 && !track_is_timer_run(TRACK_CUST_STATUS_DEFENCES_TIMER_ID))
        {
            track_os_intoTaskMsgQueueExt(track_cust_status_defences_change, (void *)2);
        }
    }
    track_cust_gps_work_req((void *)2);    // gps on
#elif defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
    {
        //模式切换
        if(G_parameter.work_mode.sw == 1)
        {
            if(track_cust_get_work_mode() != WORK_MODE_2)
            {
#if defined(__GT300S__)
                track_drv_sensor_off();
#endif
                // gps on
                track_cust_gps_work_req((void *)13);//
            }
            else
            {
                track_cust_gps_work_req((void *)2);
            }
            if(G_realtime_data.defences_status >= 2 && !track_is_timer_run(TRACK_CUST_STATUS_DEFENCES_TIMER_ID))
            {
                track_os_intoTaskMsgQueueExt(track_cust_status_defences_change, (void *)2);
            }
        }
        else
        {
            // gps on
            track_cust_gps_work_req((void *)2);
        }
    }
#elif defined(__GT530__)
    track_read_imsi_data();
    return;

#elif defined(__GT740__)
    if(G_parameter.extchip.mode==1 || track_cust_gps_need_open())
    {
        track_cust_gps_module_work((void*)2);
    }
#elif defined(__JM66__)
    return;
#else
    track_cust_gps_work_req((void *)2);    // gps on
#endif

}

/******************************************************************************
 *  Function    -  track_cust_eind
 *
 *  Purpose     -  EIND事件处理
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * v2.0  , 28-06-2016,   xzq  GPS开启函数放到 track_cust_poweron_open_gps
 * ----------------------------------------
*******************************************************************************/
void track_cust_eind(void)
{
    static kal_bool isFirst = KAL_TRUE;
    nvram_gps_time_zone_struct *zone_p;

    LOGD(L_APP, L_V5, "");
    if(isFirst)
    {
        LOGD(L_APP, L_V5, "1");
        isFirst = KAL_FALSE;
        #if !defined(__GT420D__)
        track_cust_poweron_open_gps();
        #endif
        track_cust_get_sim_status();
        if(track_is_timer_run(TRACK_QUERY_SIM_TYPE_TIMER_ID))
        {
            track_stop_timer(TRACK_QUERY_SIM_TYPE_TIMER_ID);
            track_drv_query_sim_type();
        }
        //track_sim_clean_allsms();//清空所有短信，IMEI备份的短信也将被删除
        track_read_imsi_data();
        track_cust_check_simalm();//换卡报警
        if(track_cust_is_upload_UTC_time() && G_parameter.zone.timezone_auto)
        {
            zone_p = track_mcc_to_timezone();
            if(zone_p != NULL)
            {
                memcpy(&G_parameter.zone, zone_p, sizeof(nvram_gps_time_zone_struct));
            }
        }

#if defined (__GT370__)|| defined (__GT380__)
        track_cust_paket_8A();//放在登录包回复后面，开机报警包已经入队列，导致发出去非准确时间，必须先放队列
#endif /* __GT370__ */

        /* 延后开机自动检测备份参数至SIM卡的时间，避开工厂测试环节 */
        //track_cust_sim_backup_for_parameter();
        tr_start_timer(TRACK_CUST_SIM_BACKUP_PARAM_TIMER, 300000, track_cust_sim_backup_for_parameter);
        if(PWRKEYPWRON == mmi_Get_PowerOn_Type() || CHRPWRON == mmi_Get_PowerOn_Type())
        {
#if defined (__GT370__)|| defined (__GT380__)
            tr_start_timer(TRACK_CUST_bootalm_sms_TIMER, 60000, track_cust_alarm_boot);
#else
            tr_start_timer(TRACK_CUST_bootalm_sms_TIMER, 3000, track_cust_alarm_boot);
#endif /* __GT370__ */
        }
        else
        {
            LOGD(L_APP, L_V5, "mmi_Get_PowerOn_Type:%d", mmi_Get_PowerOn_Type());
        }
        tr_start_timer(TRACK_CUST_CLEAN_SMS_TIMER, 30 * 1000, track_sim_clean_reveicebox);
    }
}

/******************************************************************************
 *  Function    -  track_nw_attach_ind
 *
 *  Purpose     -  从系统L4层上报来的GSM网络搜索情况
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_nw_attach_ind(void *msg)
{
    track_soc_nw_attach_ind(msg);
}

//#if 0//defined(__GT420D__)
void track_cust_check_24hour_timer(void)
{
    if(track_is_timer_run(TRACK_CUST_PERIODIC_RESTART_TIMER_ID))
    {
        LOGD(L_APP,L_V5,"24小时重启在跑着,%d",hour24_time/1000);
        hour24_time -= 60000;
        tr_start_timer(TRACK_CUST_CHECK_24HOUR_TIMER_ID,60*1000,track_cust_check_24hour_timer);
        }
    else
    {
        LOGD(L_APP,L_V5,"24小时重启被停掉了");
        hour24_time = 0;
        tr_stop_timer(TRACK_CUST_CHECK_24HOUR_TIMER_ID);
        }
    }

void track_cust_check_lowbat_poweroff_status(void)
{
    extern void track_cust_send03_lowpower(void* delay_off_sec);
    if(G_parameter.lowbat_status == 1)
    {
        LOGD(L_APP,L_V5,"检测到此次开机非正常低电充电开机，10秒后关机");
        track_start_timer(TRACK_CUST_EXTCHIP_WAIT03_TIMER_ID, 10*1000, track_cust_send03_lowpower, (void *)5);
    }
    }
//#endif
/******************************************************************************
 *  Function    -  track_cust_init
 *
 *  Purpose     -  应用初始化主函数
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_init(void)
{
    #if defined(__GT420D__)
    extern void track_cust_module_update_mode(void);
    extern void track_cust_send05(void);
    extern void track_cust_send10(void);
    extern void track_cust_updata_crash_mcu();
    extern void track_cust_send06(void *par);
    extern void track_cust_send04(void* par);
    extern void track_cust_send21(void* par);
    extern void track_cust_send22(void* arg);
    extern void track_cust_send98();
    extern void track_cust_send28(void);
    #endif
#ifdef __NT31__
    extern void track_power_fails_delay_timer(void);
#endif /* __NT31__ */
    #if defined(__GT420D__)
    if(G_parameter.lowbat_status == 1)
    {
        LOGD(L_APP,L_V5,"检测到此次开机非正常低电充电开机，5秒后检测是否充电开机");
        tr_start_timer(TRACK_CUST_CHECK_LOWBAT_POWEROFF_TIMER_ID,5*1000,track_cust_check_lowbat_poweroff_status);
    }
    #endif
#if defined(__NT31__) || defined(__ET310__) ||defined(__MT200__) ||defined(__NT36__)||defined (__ET320__)
    tr_start_timer(TRACK_CUST_LED_START_TIMER, 30 * 60000, track_cust_led_first_start);
#elif defined(__V20__)//test power wangqi
    tr_start_timer(TRACK_CUST_LED_START_TIMER, 10000, track_cust_led_first_start);//test
#elif defined (__NT33__)
    tr_start_timer(TRACK_CUST_LED_START_TIMER, 5 * 60000, track_cust_led_first_start);
#elif !(defined( __GT03D__) ||defined(__GT500__) ||defined(__ET200__)||defined(__ET130__) ||defined(__V20__))
    tr_start_timer(TRACK_CUST_LED_START_TIMER, 60000, track_cust_led_first_start);
#endif /* __GT03D__ */

#if defined (__GT310__)
    led_level_state_switch();
#else
    LED_PWR_StatusSet(PW_NORMAL_STATE);    //增加电池灯

    LED_GSM_StatusSet(GSM_INITIAL_STATE);
#endif
    LOGD(L_APP, L_V4, "开机%d;%d", G_parameter.work_mode.sw, track_cust_get_work_mode());

#if defined( __GT310__) || defined(__GW100__)||defined(__GT300S__)
    // track_cust_audio_play_voice(0);
    ControlVibration();
    track_cust_led_sleep(1);//GT03开机默认为模式
#endif /* __GT300__ */

    // Shock sensor
    track_cust_sensor_init();

#if defined(__GW100__)
    track_cust_update_LBS();

#elif defined(__ET310__) || defined(__MT200__)||defined(__NT36__)||defined(__NT31__)
    track_cust_LBS_upload();
#endif /* __GW100__ */

	{
    // 24小时周期性重启
    	track_start_timer(TRACK_CUST_PERIODIC_RESTART_TIMER_ID, track_drv_get_one_day_reset_time(), track_cust_periodic_restart, (void*)2);//24h=86400s
    	CTimer_Start(CTIMER_24HOUR_RESET, track_drv_get_one_day_reset_time(), track_cust_periodic_restart, (void*)2);
       #if defined(__GT420D__)
       hour24_time = track_drv_get_one_day_reset_time();
       track_cust_check_24hour_timer();
       #endif
	}

#ifdef __GT530__
    track_cust_lte_pwron(0);
#else
    // 20分钟不能连接上服务器，自动重启
    track_cust_gprs_conn_fail((void*)1);
#endif /* __GT530__ */

#if defined(__GT300S__)
    tr_start_timer(TRACK_CUST_POWERON_LED_TIME_ID, 60 * 1000, track_cust_key_led_sleep);
    track_cust_mode3_work_check((void*)1);
#elif defined(__GT370__)|| defined (__GT380__)
    track_cust_simple_gsm_led_work(GSM_INITIAL_STATE);
    tr_start_timer(TRACK_CUST_POWERON_LED_TIME_ID, 180 * 1000, track_cust_simple_gsm_led_sleep);
    track_cust_mode3_work_check((void*)1);
#elif defined(__GT310__)
    if(WORK_MODE_3 == track_cust_get_work_mode())
    {
        G_parameter.mode3_work_interval = track_cust_mode3_extchange(G_parameter.work_mode.datetime, G_parameter.work_mode.mode3_timer);
        G_realtime_data.work3_countdowm = G_parameter.mode3_work_interval;
        track_cust_mode3_work_check((void*)1);
        track_drv_sensor_off();
    }
#elif defined(__GW100__)
    if(G_parameter.lcd_on_time)
    {
    }
#elif defined(__GT500__)
#elif defined(__GT03F__)
#endif/* __GT500__ */

#if defined (__EXT_VBAT_ADC__)
#if defined(__GT380__)
/*莫名其妙的逻辑       --    chengjun  2017-06-13*/
#else
    track_start_timer(TRACK_CUST_ACC_INIT_OVERTIME_ID, 20000, acc_status_change, (void*)0);
#endif
#endif /* __EXT_VBAT_ADC__ */

#if defined(__XCWS__)
    if(G_parameter.sleep_wakeup.t_sleep == 1 && track_cust_status_charger() == 0)
    {
        track_cust_work_mode_ind(w_charge_out_msg);
    }
#endif

    track_cust_is_china_sales();

#if defined(__GT740__)
    {
        applib_time_struct currTime = {0};
        applib_dt_get_rtc_time(&currTime);
        LOGS("%0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
             currTime.nYear, currTime.nMonth, currTime.nDay,
             currTime.nHour, currTime.nMin, currTime.nSec);
    }
    //if(G_realtime_data.nw_reset_sign == 0)
    {
#if !defined(__MCU_LONG_STANDBY_PROTOCOL__)
        track_cust_module_Handshake();
#endif//__MCU_LONG_STANDBY_PROTOCOL__
    }
    
    track_cust_module_auto_sw_mode();

    //track_start_timer(TRACK_CUST_CALCULATE_ELE_TIMER_ID, 30 * 1000, track_cust_module_cal_electricity,(void*)0);
	
#if defined(__NETWORK_LICENSE__)
	tr_start_timer(TRACK_CUST_POWERON_LED_TIME_ID, 20*1000, track_cust_into_network_licenes_mode);
#endif
    track_os_intoTaskMsgQueueExt(track_cust_send_lbs, (void*)2);

    if(G_parameter.extchip.mode != 1)
    {
        tr_start_timer(TRACK_CUST_EXTCHIP_DELAY_CAL_MODE2_ELE_TIMER_ID, 10 * 1000, track_cust_module_boot_cal);
    }
    
#endif /* __GT740__ */
#if defined(__GT420D__)
      track_cust_module_Handshake();//
      if(G_parameter.extchip.mode == 1)
            {
                track_start_timer(TRACK_CUST_NET_MAXMUM_WORK_TIME_TIMER_ID,290000, track_cust_module_delay_close, (void *)10);
                track_start_timer(TRACK_CUST_SEND_DATA_MAXMUM_WORK_TIME_TIMER_ID,290000, track_cust_module_delay_close, (void *)10);
                }
      if(G_parameter.extchip.mode == 3)
        {
            LOGD(L_APP,L_V5,"检测到模式三下开机，延时10秒关机");
            track_start_timer(TRACK_CUST_EXTCHIP_WAIT04_POWERON_CHECK_TIMER_ID,10000,track_cust_send04,(void *)3);
            }
      //############10秒后检测静止震动状态开关GPS
      LOGD(L_APP,L_V5,"remove:%d",track_cust_remove_alarm_flag(99));
      tr_start_timer(TRACK_CUST_MODE1_T_OPEN_LOCATION_TIME_ID, 10 * 1000, track_cust_module_update_mode);
      tr_start_timer(TRACK_CUST_POWERON_MODELED_INIT_TIMER_ID,4000,track_cust_poweron_modeled_init);//初始化LED状态
        tr_start_timer(TRACK_CUST_EXTCHIP_WAIT98_TIMER_ID, 4500, track_cust_send98);
      #ifdef __GT420D__
       track_start_timer(TRACK_CUST_EXTCHIP_POWERON_WAIT05_TIMER_ID, 5500, track_cust_send05,(void *)0);
      #endif
      tr_start_timer(TRACK_CUST_EXTCHIP_RESEND_WAIT10_TIMER_ID, 6000, track_cust_send10);
      track_start_timer(TRACK_CUST_EXTCHIP_WAIT22_TIMER_ID, 6500, track_cust_send22,(void*)3);
      track_start_timer(TRACK_CUST_EXTCHIP_WAIT06_TIMER_ID, 7000, track_cust_send06, (void *)G_parameter.teardown_str.sw);
      //tr_start_timer(TRACK_CUST_EXTCHIP_WAIT28_TIMER_ID, 7500, track_cust_send28);
      track_start_timer(TRACK_CUST_EXTCHIP_WAIT21_CHECKSTATUS_TIMER_ID,8000,track_cust_send21,(void *)5);//查询静止运动状态

#if defined(__NETWORK_LICENSE__)
	tr_start_timer(TRACK_CUST_POWERON_LED_TIME_ID, 20*1000, track_cust_into_network_licenes_mode);
#endif
//	tr_start_timer(TRACK_CUST_WORK_MODE_POWERON_TIME_ID,10*1000,track_cust_module_sync_mcu_data);
#endif


#if defined(__GT300S__)
    tr_start_timer(TRACK_CUST_GT300S_MODE_INIT_SW_TIMER_ID, 3000, track_cust_module_sw_init);
    if(track_cust_get_work_mode() == WORK_MODE_3)
    {
        tr_start_timer(TRACK_CUST_SEND_LBS_TIMER_ID, 2 * 60 * 1000, track_cust_send_lbs);
        tr_start_timer(TRACK_CUST_WORK_OFF_MODE3_TIMER_ID, (4 * 60) * 1000, track_cust_work_off);
    }
#elif defined(__GT370__)|| defined (__GT380__)
    tr_start_timer(TRACK_CUST_GT300S_MODE_INIT_SW_TIMER_ID, 3000, track_cust_module_sw_init);
    tr_start_timer(TRACK_CUST_SEND_LBS_TIMER_ID, 20 * 1000, track_cust_send_lbs);
    if(track_cust_get_work_mode() == WORK_MODE_3)
    {
        tr_start_timer(TRACK_CUST_WORK_OFF_MODE3_TIMER_ID, (4 * 60 + 20) * 1000, track_cust_work_off);
    } 
#endif

#if defined(__BCA__)
    if(G_parameter.PW5 == 0 || G_parameter.PW5 == 1)
    {
        track_drv_gpio_pw5v(G_parameter.PW5);
    }
#endif

#ifdef __NT31__
    tr_start_timer(TRACK_CUST_POWER_FAILS_TIMER_ID, 10 * 1000, track_power_fails_delay_timer);
#endif /* __NT31__ */

#if defined (__CUSTOM_DDWL__)
    track_cust_dormancy_mode_check_wakeup();
#endif /* __CUSTOM_DDWL__ */


#if defined(__MULTIPLE_OUTPUT__)
	track_cust_output_init();
	track_cust_output2_init();
#endif

}

/******************************************************************************
 *  Function    -  track_os_nvram_init
 *
 *  Purpose     -  开机NVRAM初始化主函数
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_os_nvram_init(void)
{
    track_cust_nvram_data_init();
}
#define _TRACK_VER_APP2_         "NT76_10_A1D_V08"
#define _TRACK_VER_APP3_         "NT77_10_A1D_V08"


/*****************************************/
/*    APP -> OS                          */
/*****************************************/

char *track_get_app_version(void)
{
    static kal_bool first = KAL_TRUE;
    static char tmp[100], *src_ver, *ver, *p, *m;
    int i = 0;
    if(first)
    {
        memset(tmp, 0, 100);
 #if defined(__GT740__) && !defined(__GT741__)||defined(__GT420D__)
        //if(!strcmp(G_importance_parameter_data.version_string3, "NT76"))
       if(G_importance_parameter_data.app_version.version==2)
       {
                 src_ver = _TRACK_VER_APP2_;
       }
        else if(G_importance_parameter_data.app_version.version==3)
        {
                 src_ver = _TRACK_VER_APP3_;
        }
        else
        {
                 src_ver = _TRACK_VER_APP_;
        }
#else
        first = KAL_FALSE;
        src_ver = _TRACK_VER_APP_;
#endif
        p = strstr(src_ver, "_V");
        if(p != NULL)
        {
            m = p + 2;
            while(*m)
            {
                if(*m >= '0' && *m <= '9')
                {
                    i++;
                }
                else if(*m == '_')
                {
                    break;
                }
                else
                {
                    p = NULL;
                    break;
                }
                m++;
            }
        }
        if(i > 5) p = NULL;
        ver = G_importance_parameter_data.version_string2;
        if(p && ver && strlen(p) > 0 && strlen(ver) > 0)
        {
            int len = p - src_ver;
            if(len > 99) len = 99;
            memcpy(tmp, src_ver, len);
            sprintf(tmp + len, "_%s%s", ver, p);
        }
        else
        {
            strncpy(tmp, src_ver, 99);
        }
    }
    return tmp;
}

char *track_get_app_version_datetime(void)
{
    //Sep 28 2014 09:02:45
    //2014/09/28 08:58
    applib_time_struct t = {0};
    static char datetime[50];
    kal_int8 ret;
    ret = strtoime(__DATE__, __TIME__, &t);
    //snprintf(datetime, 50, "%s %s", __DATE__, __TIME__);
    snprintf(datetime, 50, "%02d/%02d/%02d %02d:%02d", t.nYear, t.nMonth, t.nDay, t.nHour, t.nMin);
    return datetime;
}

char *track_get_base_version(void)
{
    return (char*)_TRACK_VER_BASE_;
}

char *track_cust_get_version_for_upload_server(void)
{
    extern kal_uint8* track_drv_get_420d_mcu_version(void);
    static char ver_string[200] = {0};
    int ver_str_len = 0;
    char *ver;
    //1，OTA    2，OBD    3，软件基础版本"
    //ver_str_len += snprintf(ver_string, 199, "{t:'3',v:'[BASE]%s [BUILD]%s'}", track_get_base_version(), build_date_time());
    ver_str_len += snprintf(ver_string, 199, "{t:'3',v:'%s'}", track_get_base_version());
#if defined(__TRACK_FOTA_UPDATE__)
    ver = track_get_app_version();
    if(strlen(ver) <= 0)
    {
        ver = (char*)release_verno();
    }
    ver_str_len += snprintf(ver_string + ver_str_len, 199 - ver_str_len, ",{t:'1',v:'%s'}" , ver);
#endif /* __TRACK__ */
#if defined(__OBD__)
    if(strlen(track_obd_get_version()) > 0)
    {
        ver_str_len += snprintf(ver_string + ver_str_len, 199 - ver_str_len, ",{t:'2',v:'%s'}", track_obd_get_version());
    }
#endif /* __OBD__ */
#if defined (__MCU_SW_UPDATE__)&& !defined(__GT420D__)
    if(strlen(track_get_mcu_sw_version()) > 0)
    {
        ver_str_len += snprintf(ver_string + ver_str_len, 199 - ver_str_len, ",{t:'4',v:'%s'}", track_get_mcu_sw_version());
    }
#endif 
#if defined(__GT420D__)
    if(strlen(track_drv_get_420d_mcu_version()) > 0)
    {
        ver_str_len += snprintf(ver_string + ver_str_len, 199 - ver_str_len, ",{t:'4',v:'%s'}", track_drv_get_420d_mcu_version());
    }
#endif
#if defined(__ATGM_UPDATE__)
    if (strlen(track_cust_atgm_return_version()) > 0)
    {
        ver_str_len += snprintf(ver_string + ver_str_len, 199 - ver_str_len, ",{t:'5',v:'%s'}", track_cust_atgm_return_version());
    }
#endif /*__ATGM_UPDATE__*/

    return ver_string;
}

/******************************************************************************
 *  Function    -  track_cmd_net_recv
 *
 *  Purpose     -  网络下发的指令
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 30-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cmd_net_recv(char *data, kal_uint8 *stamp)
{
    int len;
    len = strlen(data);
    if(len < CM_PARAM_LONG_LEN_MAX - 2)
    {
        CMD_DATA_STRUCT cmd = {0};
        cmd.return_sms.cm_type = CM_ONLINE;
        cmd.rcv_length = len;
        memcpy(cmd.return_sms.stamp, stamp, 4);
        strcpy(cmd.rcv_msg, data);
        track_command_operate(&cmd);
    }
}

void track_cmd_netlog_recv(char *data)
{
    int len;
    len = strlen(data);
    if(len < CM_PARAM_LONG_LEN_MAX - 2)
    {
        CMD_DATA_STRUCT cmd = {0};
        cmd.return_sms.cm_type = CM_AT;
        cmd.rcv_length = len;
        strcpy(cmd.rcv_msg, data);
        track_command_operate(&cmd);
    }
}

void track_cmd_params_recv(char *data, Cmd_Type source)
{
    int len,i = 1;
    char *start = NULL, *end = NULL, *endF = NULL;
    kal_bool valid = KAL_FALSE;
    U16 cmd_length = 0;
    CMD_DATA_STRUCT cmd = {0};
    LOGD(L_APP, L_V5, "source:%d", source);
    track_drv_nvram_write_switch(0);
    start = (char*)data;
    len = strlen(data);
    endF = start + len;
    do
    {
        end = strchr(start, '#');
        if(end != NULL)
        {
            *end = 0;
        }
        else
        {
            end = strchr(start, '\n');
            if(end != NULL)
            {
                *end = 0;
            }
        }
        start = track_fun_trim2(start);
        memset(&cmd,0,sizeof(CMD_DATA_STRUCT));
        cmd.rcv_length = strlen(start);
        if(*start != '#' && cmd.rcv_length < CM_PARAM_LONG_LEN_MAX - 2 && cmd.rcv_length > 0)
        {
            cmd.return_sms.cm_type = source;
            strcpy(cmd.rcv_msg, start);
            LOGS("[%d] %s", i++,cmd.rcv_msg);
            track_command_operate(&cmd);
            valid = KAL_TRUE;
        }
        if(end == NULL)
        {
            break;
        }
        if(end + 1 >= endF)
        {
            break;
        }
        start = end + 1;
    }
    while(end != NULL);
    track_drv_nvram_write_switch(1);
    if(valid)
    {
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        Track_nvram_write(NVRAM_EF_PHONE_NUMBER_LID, 1, (void *)&G_phone_number, NVRAM_EF_PHONE_NUMBER_SIZE);
    }
}

#if defined (__DASOUCHE__)

/******************************************************************************
 *  Function    -  track_cmd_mqtt_recv
 *
 *  Purpose     -  网络下发的指令
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 11-12-2018,  chenjiajun  written
 * ----------------------------------------
*******************************************************************************/
void track_cmd_mqtt_recv(char *data, char *stamp)
{
    int len;
    len = strlen(data);
    if(len < CM_PARAM_LONG_LEN_MAX - 2)
    {
        CMD_DATA_STRUCT cmd = {0};
        cmd.return_sms.cm_type = CM_ONLINE;
        cmd.rcv_length = len;
        strcpy(cmd.return_sms.requestId,stamp);
        strcpy(cmd.rcv_msg, data);
        track_command_operate(&cmd);
    }
}

#endif /* __DASOUCHE__ */

/******************************************************************************
 *  Function    -  track_cust_sms_send
 *
 *  Purpose     -  发送短信
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 25-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_sms_send(kal_uint8 number_type, char *number, kal_bool isChar, kal_uint8 *data, kal_uint16 len)
{
    static kal_uint32 index = 0;
    sms_msg_struct ssms = {0};
    int len2, number_len;
    //track_os_intoTaskMsgQueue(track_soc_disconnect); // 通过测试反馈，繁忙的GPRS通讯会影响短信的成功发送。
    index++;
    if(number == NULL || strlen(number) <= 0)
    {
        LOGD(L_APP, L_V1, "WARN: 发送异常，号码为空");
        return;
    }
    if(0xFF == number_type)
    {
        LOGD(L_APP, L_V1, "WARN: 忽略发送，号码类型为0xFF");
        return;
    }
#if defined(__GT740__)
    if(G_parameter.extchip.mode != 1)
    {
        sendmessage = KAL_TRUE;
    }
#elif defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
    if(G_parameter.work_mode.mode == 3)
    {
        sendmessage = KAL_TRUE;
    }
#elif defined(__GT370__)|| defined (__GT380__)
    if(G_parameter.work_mode.mode == 1)
    {
        sendmessage = KAL_TRUE;
    }
#endif
    if(isChar)
    {
        len2 = strlen((char*)data);
        if(len > len2)
        {
            LOGD(L_APP, L_V4, "短信发送请求: index=%d, len=%d -> %d, num(%d)=%s|%s|", index, len, len2, number_type, number, data);
            len = len2;
        }
        else
        {
            LOGD(L_APP, L_V4, "短信发送请求: index=%d, len=%d, num(%d)=%s|%s|", index, len, number_type, number, data);
        }
    }
    else
    {
        track_fun_view_UTF16BE_string(data, len);
        len2 = mmi_chset_utf16_strlen(data, MMI_CHSET_UTF16BE) * 2;
        if(len > len2)
        {
            LOGD(L_APP, L_V4, "短信发送请求: index=%d, len=%d -> %d, num(%d)=%s", index, len, len2, number_type, number);
            len = len2;
        }
        else
        {
            LOGD(L_APP, L_V4, "短信发送请求: index=%d, len=%d, num(%d)=%s", index, len, number_type, number);
        }
    }
    LOGH(L_APP, L_V7, data, len);
    ssms.codetype = isChar;
    ssms.num_type = number_type;
    ssms.nocount = 1;
    ssms.numbers[0] = number;
    ssms.ctxlen = len;//(len > 120 ? 120 : len);
    ssms.ctx = data;
    ssms.index = index;
    track_sms_sender(&ssms);
    if((G_realtime_data.netLogControl & 64) && len < 256)
    {
        kal_uint8 *sendData;
        number_len = strlen(number);
        sendData = (kal_uint8*)Ram_Alloc(5, len + number_len + 2);
        sendData[0] = isChar;
        sendData[1] = number_len;
        memcpy(&sendData[2], number, number_len);
        memcpy(&sendData[2 + number_len], data, len);
        cust_packet_log_data(2, sendData, len + number_len + 2);
        Ram_Free(sendData);
    }
}

/******************************************************************************
 *  Function    -  track_cust_call_get_incomming_number
 *
 *  Purpose     -  取得当前来电号码
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 14-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
char* track_cust_call_get_incomming_number(void)
{
    return track_get_incomming_call_number();
}

/******************************************************************************
 *  Function    -  track_cust_call
 *
 *  Purpose     -  呼叫号码
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 25-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_call(char *number)
{
    track_make_call(number);
}

/******************************************************************************
 *  Function    -  track_cust_call_hangup
 *
 *  Purpose     -  挂断当前通话
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 25-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_call_hangup(void)
{
    track_hangup_call();
}

/******************************************************************************
 *  Function    -  track_cust_call_accept
 *
 *  Purpose     -  接起来电
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 14-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_call_accept(void)
{
    track_send_at_accept_call();
}

/******************************************************************************
 *  Function    -  track_cust_gps_location_time
 *
 *  Purpose     -  定位时间计算
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_gps_location_time(kal_uint8 status)
{
    static kal_uint32 tick;
#if defined(__GT740__)
     if(status == 1)
#else
    if(status == 1 && (tick == 0 || tick > kal_secs_to_ticks(600)))
#endif
    {
        LOGD(L_APP, L_V4, "请求开启GPS");
        tick = kal_get_systicks();
    }
    else if(status == 2 && tick > 0)
    {
        kal_uint32 curr_tick;
        curr_tick = kal_get_systicks();
        LOGD(L_APP, L_V4, "本次有开启定位到完成定位耗时%d秒", kal_ticks_to_secs(curr_tick - tick));
        tick = 0;
    }
}

/******************************************************************************
 *  Function    -  track_cust_gps_switch
 *
 *  Purpose     -  GPS开关接口
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_gps_switch(U8 status)
{
    static U8 fst = 0;
#if defined( __GPS_G3333__)
    if(track_get_gps_ota(0xff))
    {
        return;
    }
#endif /* __GPS_G3333__ */

#if defined(__GT741__)
    LOGD(L_APP, L_V4, "GT741不操作GPS");
	return;
#endif
    switch(status)
    {
        case 0:
            track_drv_gps_switch(0);
            LED_GPS_StatusSet(GPS_INVALID_STATE);
            LOGD(L_APP, L_V4, "GPS 关闭");
#if defined (__GT740__)||defined(__GT420D__)
            track_cust_module_gps_cal_time();
            track_cust_module_gps_sleep_time();
#endif

            break;

        case 1:
            track_drv_gps_switch(status);
            LED_GPS_StatusSet(GPS_PWON_STATE);
            LOGD(L_APP, L_V4, "GPS 开启");
#if defined (__GT740__)||defined(__GT420D__)
            track_cust_module_gps_boot_time();
            track_cust_module_gps_cal_sleep_time();
#endif

            break;
    }
}

/******************************************************************************
 *  Function    -  track_cust_oil_cut
 *
 *  Purpose     -  断油电控制
 *
 *  Description -  1 = 油电断开
 *                 0 = 油电接通
 *                 other  其它值查询返回当前的状态
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 30-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int8 track_cust_oil_cut(kal_uint8 value)
{
    LOGD(L_APP, L_V5, "%d,%d", value, G_realtime_data.oil_status);
    return track_drv_gpio_cut_oil_elec(value);
}

/******************************************************************************
 *  Function    -  track_cust_motor_lock
 *
 *  Purpose     -  锁电机
 *
 *  Description -  1 = 锁电机
 *                 0 = 解锁电机
 *                 other  其它值查询返回当前的状态
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 10-05-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int8 track_cust_motor_lock(kal_int8 value)
{
    kal_int8 ret;
    if(value == 1 && track_cust_status_charger() == 0)
    {
        LOGD(L_APP, L_V1, "锁电机失败，外电未接入");
        ret = track_drv_gpio_cut_oil_elec(0);
    }
    else
    {
        ret = track_drv_gpio_cut_oil_elec(value);
    }
    LOGD(L_APP, L_V5, "%d,%d", value, ret);
    return ret;
}

/******************************************************************************
 *  Function    -  track_cust_restart
 *
 *  Purpose     -  重启控制接口
 *
 *  Description    真难理解c
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_restart(kal_uint8 mode, kal_uint32 sec)
{
    static int lte_reset  = 0;
    static kal_uint8 pre_mode = 0;//记录实际重启参数
#if defined( __NULL_SERVER__)
    kal_uint8 iLeng = strlen((char*)track_nvram_alone_parameter_read()->server.url);
    LOGD(L_APP, L_V4, "域名为空不重启mode%d", mode);

    if(0 == iLeng &&  3 != mode && 2 != mode)
    {
        return;
    }
#endif /* __NULL_SERVER__ */
#if defined(__ATGM_UPDATE__)
    if(mode == 100 || mode == 101 || mode == 102 || mode == 34)
    {

    }
    else
    {
        if(track_cust_atgm_update_status(STATUS_CHECK) >=  ATGM_UPDATE_STATUS_update_begin_req 
            && track_cust_atgm_update_status(STATUS_CHECK) <=  ATGM_UPDATE_STATUS_send_done)
        {
            LOGD(L_APP, L_V1, "ATGM升级中禁止其他重启%d", mode);
            return;
        }
    }

#endif /*__ATGM_UPDATE__*/

#if defined (__NETWORK_LICENSE__)
    return;
#endif /* __NETWORK_LICENSE__ */


#if defined (__MCU_SW_UPDATE__)
    if(track_is_in_update_bootload_cmd())
    {
        LOGD(L_DRV, L_V1, "MCU升级中禁止自动重启");
        return;
    }
#elif defined(__GT740__)||defined(__GT420D__)
    track_cust_netlog_cache(0);

#endif 

    if(mode != 3 && (pre_mode != 3))
    {
        track_soc_queue_all_to_nvram();
        track_cust_backup_gps_data(4, NULL);
    }
    if(mode != 34)
    {
        pre_mode = mode;
        track_os_log_abnormal_data(mode);
    }
    socket_free();
#if defined(__GT530__)
    if(sec == 0 && lte_reset == 0)
    {
        lte_reset = 1;
        sec = 2;
        track_drv_lte_reset_pwron((void*) 128);
    }
#endif
    if(sec == 0)
    {
        LOGD(L_APP, L_V4, "(%d)即将重启，立即进行重启前的数据备份工作。", mode);
        track_drv_watch_dog_close(0);
        if(mode == 28 && track_os_log_abnormal_is_net_error())
        {
            char *tmp = (char *)40;
            track_check_tick(3, (char*)tmp);
        }
        else
#if !(defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__))
            if(mode != 63)
#endif /* __GT300S__ */
            {
                track_device_reset(0);
            }
    }
    else
    {
        LOGD(L_APP, L_V4, "(%d)即将在%d秒后重启，立即进行重启前的数据备份工作。", mode, sec);
        track_device_reset(sec);
    }
}

/******************************************************************************
 *  Function    -  track_cust_get_sim_status
 *
 *  Purpose     -  获取当前SIM卡状态
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 24-01-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int8 track_cust_get_sim_status(void)
{
    kal_int8 sim_type;
    sim_type = track_drv_get_sim_type();
    if(sim_type == 1)
        track_cust_status_sim_set(1);
    else
        track_cust_status_sim_set(0);
    LOGD(L_APP, L_V6, "%d", sim_type);
    return sim_type;
}

/******************************************************************************
 *  Function    -  track_cust_is_upload_UTC_time
 *
 *  Purpose     -  当前项目是否使用上传 UTC 时间
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 05-03-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_bool track_cust_is_upload_UTC_time(void)
{
#if defined(__WM_OLD06__) || defined(__GT06F__) || defined(__GT03F__) || defined(__GT02__) || defined(__NT36__)||defined (__RTC_OLD_PROTOCOL__)
    return KAL_FALSE;
#else
    return KAL_TRUE;
#endif /* __GT06B__ */
}

/******************************************************************************
 *  Function    -  track_cust_is_china_sales
 *
 *  Purpose     -  销售区域限制
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 13-05-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_bool track_cust_is_china_sales(void)
{
    char *mcc = track_drv_get_mcc_from_imsi();

#ifdef __GT02__
    if(!G_importance_parameter_data.app_version.support_WM)
    {
        LOGD(L_APP, L_V4, "仅限国内大陆区域使用!");
        if(!strcmp(mcc, "460") || !strcmp(mcc, "461"))  // 中国大陆
        {
            /*符合条件允许使用*/
            return KAL_TRUE;
        }
        LOGD(L_APP, L_V4, "使用受限!");
        return KAL_FALSE;
    }
    else
    {
        return KAL_TRUE;
    }
#endif /* __GT02__ */

//待定
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
    LOGD(L_APP, L_V4, "无使用区域限制!");
    return KAL_TRUE;
#endif

#if defined(__LIMIT_SALES_AREA__)
    LOGD(L_APP, L_V4, "仅限国内大陆区域使用!");
    if(!strcmp(mcc, "460") || !strcmp(mcc, "461"))  // 中国大陆
    {
        /*符合条件允许使用*/
        return KAL_TRUE;
    }
    LOGD(L_APP, L_V4, "使用受限!");
    return KAL_FALSE;
#elif defined(__BLOCK_SALES_AREA_INDIA_ET__)
//定制版，封锁以下五个国家地区使用
    LOGD(L_APP, L_V4, "印度、肯尼亚、阿根廷、巴西、巴拉圭5个国家限制使用!");
    if(!strcmp(mcc, "404")/*印度*/ || !strcmp(mcc, "405")/*印度*/ || !strcmp(mcc, "639")/*肯尼亚*/ || !strcmp(mcc, "722")/*阿根廷*/ || !strcmp(mcc, "724")/*巴西*/ || !strcmp(mcc, "744")/*巴拉圭*/)
    {
        /*符合条件禁止使用*/
        LOGD(L_APP, L_V4, "使用受限!");
        return KAL_FALSE;
    }
    return KAL_TRUE;
#elif defined(__BLOCK_SALES_AREA_ARENTINA_ET__)
//定制版，封锁以下三个国家地区使用
    LOGD(L_APP, L_V4, "阿根廷、巴西、巴拉圭3个国家限制使用!");
    if(!strcmp(mcc, "722")/*阿根廷*/ || !strcmp(mcc, "724")/*巴西*/ || !strcmp(mcc, "744")/*巴拉圭*/)
    {
        /*符合条件禁止使用*/
        LOGD(L_APP, L_V4, "使用受限!");
        return KAL_FALSE;
    }
    return KAL_TRUE;
#else

    LOGD(L_APP, L_V4, "无使用区域限制!");
    return KAL_TRUE;
#endif
}

/******************************************************************************
 *  Function    -  track_cust_sleep_mode
 *
 *  Purpose     -  深度休眠，gsm待机
 *
 *  Description -  NULL
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 07-08-2013,  ZengPing && Liujw  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_sleep_mode(void)
{
    track_stop_timer(TRACK_CUST_ALARM_GPS_WORK_TIMER);
    track_stop_timer(TRACK_CUST_POWERON_CLOSE_GPS_TIMER);
    tr_stop_timer(TRACK_CUST_GPS_EPHEMERIS_UPDATE_TIMER_ID);

    LOGD(L_APP, L_V6, "work_mode = %d", track_cust_get_work_mode());
    if(track_cust_gps_status() != 0)
    {
        track_cust_gps_off_req((void*)4);
    }
    track_os_intoTaskMsgQueue2(track_drv_sensor_off);
    track_os_intoTaskMsgQueue2(track_soc_disconnect);
    track_os_intoTaskMsgQueue2(track_soc_clear_dw_queue);

}

kal_uint16 track_cust_get_pack_time(void)
{
    kal_bool moving = track_cust_sensor_delay_valid();

    if(moving)
    {
        return G_parameter.gps_work.Fti.interval_acc_on;
    }
    else
    {
        return G_parameter.gps_work.Fti.interval_acc_off;
    }

}

#if defined (__GT370__)|| defined (__GT380__)
kal_uint8 g_when_still_no_gps_enable_pwron_gps = 0; //静止时GPS没有定位，需要重新开GPS 2分钟
extern void track_gps_static_delay_send_position(void);
void track_cust_mode_change(kal_uint8 flg)
{
    kal_uint16 T3 = 0, per_mode = 0;
    kal_bool moving = track_cust_sensor_delay_valid();

    T3 = track_cust_get_pack_time();

#if 0
    if(track_cust_get_work_mode() != WORK_MODE_3)
    {
        if(T3 > 120)
        {
            per_mode = 1;
        }
        else
        {
            per_mode = 2;
        }
        if(0 == flag && G_parameter.work_mode.mode != per_mode)
        {
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE)
        }
    }
#endif
    LOGD(L_APP, L_V5, "%d,moving status:%d,T1:%d,T2:%d,T3:%d,mode:%d,gps:%d", flg, moving,
         G_parameter.gps_work.Fti.interval_acc_on, G_parameter.gps_work.Fti.interval_acc_off, \
         T3, track_cust_get_work_mode(), track_cust_gps_status());

    if((track_cust_get_work_mode() == WORK_MODE_2)) //2=为需求里的模式1，T<120s
    {
        /*track_cust_sensor_add_position有静止补传功能，再这样就会重复       --    chengjun  2017-04-19*/
#if 0
        if(1 == flg && track_cust_gps_status() > GPS_STAUS_2D)
        {
            track_gps_data_struct *gpsdata = track_cust_backup_gps_data(0, NULL);
            track_cust_paket_position(gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_STATIC, 0);
        }
#endif

        if(track_is_timer_run(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID))
        {
            //关闭模式1定时器
            track_stop_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID);
        }

        if(moving)//在震动...
        {
            LOGD(L_APP, L_V5, "运动模式2 %d", flg);
            if(0 == flg)
            {
                track_cust_gps_work_req((void*)3);
            }
        }
        else
        {
            if(KAL_FALSE == track_is_timer_run(TRACK_CUST_LBS_UPDATE_L_TIMER_ID) || 0 == flg)
            {
                LOGD(L_APP, L_V5, "静止模式2下次传WIFI %d", G_parameter.gps_work.Fti.interval_acc_off);
                tr_start_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID, G_parameter.gps_work.Fti.interval_acc_off * 1000, track_cust_update_LBS);
            }

            //静止GPS没定位时
                if((1 == flg) && (G_parameter.staticrep.sw))
                {
                    if(g_when_still_no_gps_enable_pwron_gps == 1)
                    {
                        g_when_still_no_gps_enable_pwron_gps = 0;
                        //if(track_cust_gps_status() == GPS_STAUS_SCAN)
						if ((track_cust_gps_status() == GPS_STAUS_SCAN) || ((track_cust_is_fix_when_gps_off()==0)&&(track_cust_gps_status() < GPS_STAUS_2D)))
                        {
                            //track_gps_data_struct *gpsdata = track_cust_backup_gps_data(0, NULL);
                            //track_cust_paket_position(gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_STATIC, 0);
                    
                            if(G_parameter.wifi.sw==1)
                            {
                                track_cust_sendWIFI_paket();//传WIFI
                            }
                            else
                            {
                                track_os_intoTaskMsgQueueExt(track_cust_send_lbs, (void*)4);
                            }
                        }
                    }
                }
            //------------------------------------
                
        }

    }
    else if(track_cust_get_work_mode() == WORK_MODE_1)  //1=为需求里的模式1，T>120s
    {
        track_cust_stop_update_LBS();

#if 1
        //静止补传
        if((1 == flg) && (G_parameter.staticrep.sw))
        {
            if(g_when_still_no_gps_enable_pwron_gps == 1)
            {
                g_when_still_no_gps_enable_pwron_gps = 0;
                if(track_cust_gps_status() >= GPS_STAUS_2D)
                {
                    track_gps_data_struct *gpsdata = track_cust_backup_gps_data(0, NULL);
                    track_cust_paket_position(gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_STATIC, 0);
                }
                else
                {
                    LOGD(L_APP, L_V5, "模式(1&T>120S) 运动变静止时未定位开GPS");
                    track_os_intoTaskMsgQueueExt(track_cust_gps_work_req, (void*)20);
                    tr_start_timer(TRACK_CUST_WHEN_STILL_NO_POSITION_GPS_ON_TIMER_ID, 110 * 1000, track_gps_static_delay_send_position);
                }
            }
        }
#endif

        if(0 == flg || 5 == flg)
        {
            track_cust_gps_work_req((void*)13);//
        }
        else
        {
            if(moving)
            {
                LOGD(L_APP, L_V5, "模式(1&T>120S) 下次开启GPS %ds", track_gt370_get_upload_time());
                track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID, track_gt370_get_upload_time() * 1000, track_cust_gps_work_req, (void *)13);//
            }
            else
            {
                LOGD(L_APP, L_V5, "模式(1&T>120S) 静止不开GPS");
            }
        }
    }
}

#endif

/******************************************************************************
 *  Function    -  track_cust_wake_mode
 *
 *  Purpose     -  唤醒休眠
 *
 *  Description -  NULL
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 07-08-2013,  ZengPing && Liujw  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_wake_mode(void)
{
#if defined(__GT370__)|| defined (__GT380__)
    LOGD(L_APP, L_V5, "");
    track_cust_mode_change(0);
#elif defined(__GT310__)
    if((track_cust_get_work_mode() == WORK_MODE_2))
    {
        track_cust_open_location_mode(3);
        track_cust_sensor_init();
    }
    else if(track_cust_get_work_mode() == WORK_MODE_1)
    {
        track_cust_open_location_mode(13);
    }
#else
    if((track_cust_get_work_mode() == WORK_MODE_2) || (track_cust_get_work_mode() == WORK_MODE_4))
    {
        track_cust_gps_work_req((void*)3);
        track_cust_sensor_init();
    }
    else if(track_cust_get_work_mode() == WORK_MODE_1)
    {
        track_cust_gps_work_req((void*)13);//

        if(track_cust_status_defences() != 0)
        {
            track_cust_sensor_init();
        }
        else
        {
            track_drv_sensor_off();
        }
    }
#endif

    track_soc_gprs_reconnect((void*)304);
}

/******************************************************************************
 *  Function    -  track_cust_external_batter_measure_done
 *
 *  Purpose     -  外电检测控制GPIO28
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 08-10-2013,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_external_batter_measure_done(kal_int32 ext_volt)
{
    char exe = 0;
    LOGD(L_APP, L_V6, "%d", ext_volt);
    if((ext_volt <= 12500000))
    {
        exe = GPIO_ReadIO(28);
        if(exe == 1)
        {
            return;
        }
        GPIO_ModeSetup(28, 0);
        GPIO_InitIO(1, 28);
        GPIO_WriteIO(1, 28);
        LOGD(L_APP, L_V6, "GPIO 28 H");
    }
    else if(ext_volt >= 13000000)
    {
        exe = GPIO_ReadIO(28);
        if(exe == 0)
        {
            return;
        }
        GPIO_ModeSetup(28, 0);
        GPIO_InitIO(1, 28);
        GPIO_WriteIO(0, 28);
        exe = GPIO_ReadIO(28);
        LOGD(L_APP, L_V6, "GPIO 28 L");
    }

}

kal_uint32 track_cust_gprson_status(kal_uint32 flag)
{
    static kal_uint32 short_login = 0;
    LOGD(L_APP, L_V5, "flag:%d short_login:%d mod:%d gprson:%d", flag, short_login, G_parameter.work_mode.mode, G_parameter.gprson);
    if(flag == 1 || flag == 0)
    {
        return (short_login = flag);
    }

    if(g_GT03D == KAL_TRUE)
    {
        LOGD(L_APP, L_V6, "mode:%d;gprson:%d;short_login:%d", G_parameter.work_mode.mode, G_parameter.gprson, short_login);
#if defined (__GT300S__) || defined(__GT370__)|| defined (__GT380__)
        return G_parameter.gprson;
#else
        if(G_parameter.work_mode.mode != 3)
        {
            return G_parameter.gprson;
        }
        else
        {
            if(G_parameter.gprson == 1 && short_login == 1)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
#endif /* __GT300S__ */
    }
    else
    {
        return G_parameter.gprson;
    }
}

/******************************************************************************
 *  Function    -  track_cust_check_simalm
 *
 *  Purpose     -  检测是否需要上报换卡报警
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 08-11-2014,  ZengPing && Liujunwu  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_check_simalm(void)
{
    kal_uint32 sim_nvram_flag = 0;
    char *imsi;

    if(G_parameter.sim_alarm.sw == 1)
    {
        imsi = (char *)track_drv_get_imsi(0);
        LOGD(L_APP, L_V5, "IMSI imsi=%s", imsi);
        LOGD(L_APP, L_V5, "IMSI nvram imsi=%s sim_alm=%d", G_parameter.sim_alarm.sim_imsi, G_realtime_data.sim_alm);
        if((!strcmp(G_parameter.sim_alarm.sim_imsi, "0000000000000000")) && ((strcmp(imsi, "000000000000000")) != 0))
        {
            LOGD(L_APP, L_V5, "IMSI NONE");
            sim_nvram_flag = 1;
            sprintf(G_parameter.sim_alarm.sim_imsi, "%s", imsi);
        }
        else if(((strcmp(G_parameter.sim_alarm.sim_imsi, imsi)) == 0) && G_realtime_data.sim_alm == 1)
        {
            LOGD(L_APP, L_V5, "IMSI the same");
            sim_nvram_flag = 2;
            G_realtime_data.sim_alm = 0;
        }
        else if(((strcmp(G_parameter.sim_alarm.sim_imsi, imsi)) != 0) && (G_realtime_data.sim_alm == 0))
        {
            LOGD(L_APP, L_V5, "IMSI CHANGE");
            G_realtime_data.sim_alm = 1;
            snprintf(G_parameter.sim_alarm.sim_imsi, sizeof(G_parameter.sim_alarm.sim_imsi), "%s", imsi);
            sim_nvram_flag = 2;
            track_cust_check_simalm_flag(1);
            track_cust_molude_simalarm_sms();
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        }
        else if(G_realtime_data.sim_alm == 1)
        {
            LOGD(L_APP, L_V5, "IMSI @SMS@ has been sent");
        }
        if(sim_nvram_flag != 0)
        {
            LOGD(L_APP, L_V5, "sim alm write to nvram  sim_nvram_flag =%d", sim_nvram_flag);
            if(sim_nvram_flag == 1)
            {
                Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
            }
            else if(sim_nvram_flag == 2)
            {
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
            }
        }
    }
}

extern void track_cust_mode3_update_LBS(void *f);


void track_cust_mode3_work_check(void* flag)
{
    kal_uint32 f = (kal_uint32)flag;
    U8 coutdown = 10;
    LOGD(L_APP, L_V5, "@@flag:%d:interval:%d mins:countdown:%d mins:work_mode:%d,up:%d", f, G_parameter.mode3_work_interval,
         G_realtime_data.work3_countdowm, track_cust_get_work_mode(), G_parameter.mode3_up_counts);

    if(G_parameter.mode3_work_interval <= 0)
    {
        return;
    }

    if(0 == f) /*重新设置*/
    {
        G_realtime_data.work3_countdowm = G_parameter.mode3_work_interval;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    }

    if(WORK_MODE_3 != track_cust_get_work_mode())
    {
        return;
    }

    if(1 == f && mmi_Get_PowerOn_Type() != PWRKEYPWRON)
    {
        if(G_realtime_data.work3_countdowm + 10 < G_parameter.mode3_work_interval)
        {
            G_realtime_data.work3_countdowm += 5;//异常重启补尝
        }
    }

    if(G_realtime_data.work3_countdowm > G_parameter.mode3_work_interval)
    {
        G_realtime_data.work3_countdowm = G_parameter.mode3_work_interval;
    }

    if(G_realtime_data.work3_countdowm > 10)
    {
        LOGD(L_APP, L_V5, "深度休眠还有%d分钟结束!!!!", G_realtime_data.work3_countdowm);
        G_realtime_data.work3_countdowm -= 10;
    }
    else
    {

#if defined (__GT310__)
        G_parameter.mode3_work_interval = track_cust_mode3_extchange(G_parameter.work_mode.datetime, G_parameter.work_mode.mode3_timer);
#endif /* __GT310__ */

        G_realtime_data.work3_countdowm = G_parameter.mode3_work_interval;

        LOGD(L_APP, L_V5, "MODE3定时时间到");
#if defined(__GT310__)
        track_cust_fly_mode_change((void*)79);
        track_cust_mode3_uping(1);
        track_cust_open_location_mode(15);
#else
        track_cust_gprson_status(1);
        track_soc_gprs_reconnect((void*)305);
        //唤醒GPS工作
        track_cust_gps_work_req((void*)15);
        track_cust_mode3_uping(1);
        track_start_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID, 185 * 1000, track_cust_mode3_update_LBS, (void *)0);
#endif

    }

    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    /*以10分钟为步进向前推*/
    track_start_timer(TRACK_CUST_WORK_MODE3_TIMER_ID, coutdown * 60 * 1000, track_cust_mode3_work_check, (void*)99);
}

void track_cust_set_vcama(kal_bool ON, kal_uint32 ldo_volt)
{
    track_drv_set_vcama(ON, ldo_volt);
}
#if defined(__GT530__)
void track_cust_check_sim(void)
{
    track_drv_check_sim();
}
extern void track_set_imei_str(void);
void track_cust_get_imei(void)
{
    static int count = 0;
    if(count > 2)
    {
        track_set_imei_str();
        track_read_imei_data();
    }
    else
    {
        count++;
        track_drv_check_imei();
        tr_start_timer(TRACK_CUST_GET_IMEI_TIMER_ID, 1000, track_cust_get_imei);
    }
    LOGS("获取IMEI号");

}

void track_cust_lte_pwron(kal_uint8 s)
{
    track_drv_lte_pwron(s);
}

kal_uint8 track_cust_lte_systemup(kal_uint8 work)
{
    return track_drv_lte_systemup(work);
}
kal_uint8 track_cust_agps_domain()
{
    track_cust_server2_file_retry();
    track_cust_server2_conn((void*)3);
}

void track_cust_wifi_close(void)
{
    track_drv_wifi_switch(0);
}
void track_cust_lte_systemup_ind()
{
    if(track_is_testmode())
    {
        track_drv_gps_switch(1);
    }
#if defined( __NETWORK_LICENSE__)
    tr_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID, 5000, track_cust_wifi_close);
    track_drv_network_license_auto_make_call(10000);
    LOGS("CTA CE TEST\r\n");
#else
    track_start_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID, 500, track_cust_gps_work_req, (void *)2);
    tr_start_timer(TRACK_QUERY_SIM_TYPE_TIMER_ID, 3000, track_cust_check_sim);
    tr_start_timer(TRACK_CUST_GET_IMEI_TIMER_ID, 1000, track_cust_get_imei);//获取IMEI号
    tr_start_timer(TRACK_CUST_NET_CONN_TIMER_ID, 10000, track_cust_net_conn);
    //tr_start_timer(TRACK_CUST_NET_CONN_TIMER_ID2,8000,track_cust_agps_domain);//agps服务器
    track_cust_serverset(0);
    track_cust_lte_ready(1);
#endif /* __NETWORK_LICENSE__ */

}

//唤醒61
void track_cust_lte_wakeup(void)
{
    LOGD(L_APP, L_V5, "");
    track_drv_sleep_enable(SLEEP_DRV_MOD, KAL_FALSE);
    track_drv_lte_uart1_ctrl(1);
    track_drv_lte_init_wdt(1);
}
//61休眠
void track_cust_pre_lte_sleep(void)
{
    LOGD(L_APP, L_V5, "");
    track_drv_sleep_enable(SLEEP_DRV_MOD, KAL_TRUE);
    track_drv_lte_uart1_ctrl(0);
    track_drv_lte_init_wdt(0xFF);
}
#endif


#if defined(__433M__)

/******************************************************************************
 * FUNCTION:  - 控制433模块模式
 *
 * DESCRIPTION: -
 *
 *          mode 1://初始化GPIO口
 *          mode 2://停止433 工作
 *          mode 3://ACC OFF
 *          mode 4://远程喇叭设防
 *          mode 5://远程喇叭寻车
 *          mode 6://远程喇叭报警
 *          mode 7://恢复断油电
 * Input:
 * Output:
 * Returns:
 *
 * modification history
 * --------------------
 * v1.0  , 14-09-2015,  ZengPing  written
 * --------------------
 ******************************************************************************/
void track_cust_433_control(kal_uint32 mode)
{
    static int lock_status = 0;
    LOGD(L_APP, L_V5, "mode =%d   lock_status =%d", mode, lock_status);
    if(mode == 0)
    {
        lock_status = mode;
        LOGD(L_APP, L_V5, "reset lock_status");
        return;
    }
    if(lock_status != mode)
    {
        lock_status = mode;
        track_drv_433_control(mode);
#if defined(__V20__)
        if(mode == 3)
        {
            LOGD(L_APP, L_V5, "track_cust_oil_cut_req(1)");
            track_cust_oil_cut_req(1);
        }
        else if((mode == 7) && (G_realtime_data.oil_lock_status == 0))
        {
            LOGD(L_APP, L_V5, "track_cust_oil_cut_req(0)");
            track_cust_oil_cut_req(0);
        }
#endif /* __V20__ */
    }
    else
    {
        LOGD(L_APP, L_V5, "相同状态不执行操作mode =%d   lock_status =%d", mode, lock_status);
    }
}


/******************************************************************************
 * FUNCTION:  - 初始化433模块并给GPIO33口上电,为了让模块间适配
 *
 * DESCRIPTION: -
 *    GPIO33 不上电可能会导致后续操作因无适配而失败
 *
 * Input:
 * Output:
 * Returns:
 *
 * modification history
 * --------------------
 * v1.0  , 14-09-2015,  ZengPing  written
 * --------------------
 ******************************************************************************/
void track_cust_433_init(void)
{
    LOGD(L_APP, L_V5, "");
    track_cust_433_control(1);
}
#else
void track_cust_433_control(kal_uint32 mode)
{
}
void track_cust_433_init(void)
{
}
#endif /* __433M__ */



void track_cust_get_timer(void)
{

}


#if defined (__CUSTOM_DDWL__)
kal_uint8 g_flymode_daily_send = 0; //飞行模式下每天定时开启上报一次数据，>0 表示需要唤醒上报
kal_uint8 g_daily_gps_fix=0;
void track_cust_daily_to_flight_mode(void);
kal_uint8 track_cust_dormancy_change_flight_mode(kal_uint8 enter);
void track_cust_dormancy_delay_to_flight_mode(void);
void track_cust_daily_out_flight_mode_send_location(void);

void track_cust_dormancy_send_daily_last_gps(void)
{
    track_gps_data_struct last_gps_data = {0};
    track_gps_data_struct *gpsLastPoint = NULL;
    
    LOGD(L_APP, L_V4, "飞行模式定时GPS补点");

    g_flymode_daily_send = 0;

    gpsLastPoint = track_cust_backup_gps_data(2, NULL);
    memcpy(&last_gps_data, gpsLastPoint, sizeof(track_gps_data_struct));

    track_cust_paket_position(&last_gps_data, track_drv_get_lbs_data(), TR_CUST_POSITION_TIMING, 1);
    tr_start_timer(TRACK_CUST_DORMANCY_TO_FLIGHT_MODE_TIMER_ID, 30 * 1000, track_cust_daily_to_flight_mode);
}

void track_cust_dormancy_send_daily_gps(void)
{
    track_gps_data_struct last_gps_data = {0};
    track_gps_data_struct *gpsLastPoint = NULL;

    if(g_flymode_daily_send == 0)
    {
        return;
    }

    if(g_daily_gps_fix==1)
    {
        g_flymode_daily_send = 0;

        LOGD(L_APP, L_V4, "飞行模式定时GPS新点");

        if(track_is_timer_run(TRACK_CUST_DORMANCY_GPS_FIX_TIMEOUT_TIMER_ID))
        {
            track_stop_timer(TRACK_CUST_DORMANCY_GPS_FIX_TIMEOUT_TIMER_ID);
        }
        
        gpsLastPoint = track_cust_backup_gps_data(0, NULL);

        memcpy(&last_gps_data, gpsLastPoint, sizeof(track_gps_data_struct));

        track_cust_paket_position(&last_gps_data, track_drv_get_lbs_data(), TR_CUST_POSITION_TIMING, 0);
        track_stop_timer(TRACK_CUST_DORMANCY_ALARM_DELAY_TO_FLIGHT_TIMER_ID);
        tr_start_timer(TRACK_CUST_DORMANCY_TO_FLIGHT_MODE_TIMER_ID, 30 * 1000, track_cust_daily_to_flight_mode);        
    }
    else
    {
        if(track_is_timer_run(TRACK_CUST_DORMANCY_GPS_FIX_TIMEOUT_TIMER_ID))
        {
            tr_start_timer(TRACK_CUST_DORMANCY_GPS_FIX_TIMEOUT_TIMER_ID, 300*1000, track_cust_dormancy_send_daily_last_gps);
        }
    }
}

void track_cust_dormancy_daily_gps_fix_check_gprs(void)
{
    track_gps_data_struct last_gps_data = {0};
    track_gps_data_struct *gpsLastPoint = NULL;

    if(g_flymode_daily_send == 0)
    {
        return;
    }

    if(track_soc_login_status())
    {
        g_flymode_daily_send = 0;

        LOGD(L_APP, L_V4, "飞行模式定时唤醒GPS定位");
        
        gpsLastPoint = track_cust_backup_gps_data(0, NULL);

        memcpy(&last_gps_data, gpsLastPoint, sizeof(track_gps_data_struct));

        track_cust_paket_position(&last_gps_data, track_drv_get_lbs_data(), TR_CUST_POSITION_TIMING, 0);
        track_stop_timer(TRACK_CUST_DORMANCY_ALARM_DELAY_TO_FLIGHT_TIMER_ID);
        tr_start_timer(TRACK_CUST_DORMANCY_TO_FLIGHT_MODE_TIMER_ID, 30 * 1000, track_cust_daily_to_flight_mode);
    }
}


void track_cust_daily_out_flight_mode_resend_location(void)
{
    applib_time_struct rtcTime = {0};
        
    if ((g_flymode_daily_send == 0)||(g_flymode_daily_send == 1))
    {
        return;
    }

    if(G_parameter.dormancy.sw == 0)
    {
        return;
    }

    g_flymode_daily_send--;

    LOGD(L_APP, L_V5, "定时唤醒重发 %d",g_flymode_daily_send);

    track_cust_daily_out_flight_mode_send_location();
}

void track_cust_daily_out_flight_mode_send_location(void)
{
    LOGD(L_APP, L_V5, "定时唤醒设备");

    if(track_cust_dormancy_change_flight_mode(0xFF) == 1)
    {
        LOGD(L_APP, L_V4, "定时退出飞行模式");
        g_daily_gps_fix=0;
        track_cust_gps_work_req((void*)10);
        track_cust_dormancy_change_flight_mode(0);
        tr_start_timer(TRACK_CUST_DORMANCY_GPS_FIX_TIMEOUT_TIMER_ID, 300*1000, track_cust_dormancy_send_daily_last_gps);
        tr_start_timer(TRACK_CUST_DORMANCY_ALARM_DELAY_TO_FLIGHT_TIMER_ID, 360*1000, track_cust_dormancy_delay_to_flight_mode);    
        tr_start_timer(TRACK_CUST_DORMANCY_TO_FLIGHT_MODE_TIMER_ID, 20*60*1000, track_cust_daily_to_flight_mode);
    }
    
    tr_start_timer(TRACK_CUST_DORMANCY_CHECK_RESEND_TIMER_ID, 330*1000, track_cust_daily_out_flight_mode_resend_location);
}

    
void track_cust_daily_to_flight_mode(void)
{
    if(track_cust_sensor_delay_valid() > 0)
    {
        LOGD(L_APP, L_V5, "有过震动，不进飞行模式");
    }
    else
    {
        LOGD(L_APP, L_V5, "定时工作完成进飞行模式");
        track_cust_dormancy_change_flight_mode(1);
    }
    g_flymode_daily_send = 0;
}

/******************************************************************************
 *  Function    -  track_cust_dormancy_mode_check_wakeup
 *
 *  Purpose     -  定时查询是否临时退出飞行模式上报一次数据
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-05-02,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_dormancy_mode_check_wakeup(void)
{
    applib_time_struct rtcTime = {0};
    kal_uint8 random;
    kal_int8 diff;

    if(G_parameter.dormancy.sw == 0)
    {
        return;
    }

    track_fun_get_time(&rtcTime, KAL_FALSE, &G_parameter.zone);

    LOGD(L_APP, L_V5, "时间检测 %02d:%02d--%02d:%02d", rtcTime.nHour,rtcTime.nMin,G_parameter.dormancy.hour,G_parameter.dormancy.min);

    if(rtcTime.nHour == G_parameter.dormancy.hour)
    {
        diff=rtcTime.nMin-G_parameter.dormancy.min;
        
        if( (diff>=0) && (diff<3) )
        {
            g_flymode_daily_send = 2;
            track_cust_daily_out_flight_mode_send_location();
            tr_start_timer(TRACK_CUST_DORMANCY_CHECK_TIMER_ID, 300 * 1000, track_cust_dormancy_mode_check_wakeup);
            return;
        }
    }
    
    tr_start_timer(TRACK_CUST_DORMANCY_CHECK_TIMER_ID, 120 * 1000, track_cust_dormancy_mode_check_wakeup);
}

/******************************************************************************
 *  Function    -  track_cust_dormancy_delay_to_flight_mode
 *
 *  Purpose     -  报警延时进入飞行模式
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-05-24,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_dormancy_delay_to_flight_mode(void)
{
    LOGD(L_APP, L_V4, "断电或定时唤醒超时进飞行");
    track_cust_dormancy_change_flight_mode(1);
}

void track_cust_dormancy_exit_flight_mode_unlock(void)
{
    //nc
}

void track_cust_dormancy_exit_flight_mode_delay(void)
{
    track_drv_sys_cfun(1);
    track_soc_exit_fly_only();
    track_cust_call_hangup();
    track_sms_reset();
}

/******************************************************************************
 *  Function    -  track_cust_dormancy_change_flight_mode
 *
 *  Purpose     -  进入或退出飞行模式
 *
 *  Description -   1=进入飞行模式,0xFF读状态
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-05-02,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint8 track_cust_dormancy_change_flight_mode(kal_uint8 enter)
{
    static kal_uint8 inFlyMode = 0;

    if(enter == 0xFF)
    {
        return inFlyMode;
    }

    if(inFlyMode == enter)
    {
        return 0xFF;
    }

    if(enter == 0)
    {

        if(track_is_timer_run(TRACK_CUST_DORMANCY_ALARM_DELAY_TO_FLIGHT_TIMER_ID))
        {
            track_stop_timer(TRACK_CUST_DORMANCY_ALARM_DELAY_TO_FLIGHT_TIMER_ID);
        }

        inFlyMode = 0;
        LOGD(L_APP, L_V4, "退出飞行模式");

        if(track_is_timer_run(TRACK_CUST_DORMANCY_FILGHT_MODE_UNLOCK_TIMER_ID))
        {
            track_stop_timer(TRACK_CUST_DORMANCY_FILGHT_MODE_UNLOCK_TIMER_ID);
            tr_start_timer(TRACK_CUST_DORMANCY_FILGHT_MODE_UNLOCK_TIMER_ID, 10*1000, track_cust_dormancy_exit_flight_mode_delay);
        }
        else
        {
            track_cust_dormancy_exit_flight_mode_delay();
        }
    }
    else if(enter == 1)
    {
        if(G_parameter.dormancy.sw == 0)
        {
            return 0xFF;
        }

        if(track_is_testmode() || track_is_autotestmode())
        {
            return 0xFF;
        }

        if(track_is_timer_run(TRACK_CUST_DORMANCY_ALARM_DELAY_TO_FLIGHT_TIMER_ID))
        {
            LOGD(L_APP, L_V4, "断电报警/定时唤醒，不进飞行");
            return 0xFF;
        }

        inFlyMode = 1;

        LOGD(L_APP, L_V4, "进入飞行模式");

        track_cust_gps_control(0);
        track_cust_led_sleep(2);
        track_soc_disconnect();
        track_cust_server2_disconnect();
        track_cust_make_call((void*)23);
        track_cust_call_hangup();
        track_sms_reset();
        track_drv_sys_cfun(4);

        if(track_is_timer_run(TRACK_CUST_DORMANCY_FILGHT_MODE_UNLOCK_TIMER_ID))
        {
            track_stop_timer(TRACK_CUST_DORMANCY_FILGHT_MODE_UNLOCK_TIMER_ID);            
        }
        tr_start_timer(TRACK_CUST_DORMANCY_FILGHT_MODE_UNLOCK_TIMER_ID, 10*1000, track_cust_dormancy_exit_flight_mode_unlock);        
    }
    else
    {
        return inFlyMode;
    }

    if(!track_is_timer_run(TRACK_CUST_DORMANCY_CHECK_TIMER_ID))
    {
        tr_start_timer(TRACK_CUST_DORMANCY_CHECK_TIMER_ID, 300 * 1000, track_cust_dormancy_mode_check_wakeup);
    }

    return inFlyMode;
}

#endif /* __CUSTOM_DDWL__ */


#if defined (__VIRTUAL_ACC__)
static kal_uint8 virtual_acc = 0xFF;

void track_cust_virtual_acc_set(kal_uint8 acc_on)
{
    LOGD(L_APP, L_V4, "模拟ACC %d->%d",virtual_acc,acc_on);

    if(virtual_acc!=acc_on)
    {
        virtual_acc=acc_on;
        track_os_intoTaskMsgQueueExt(heartbeat_packets, (void *)116);
    }
}


void track_sensor_delay_invalid_virtual_acc_off(void)
{
    track_cust_virtual_acc_set(0);
}

kal_uint8 track_cust_virtual_acc(void)
{
    return virtual_acc;
}

#endif /* __VIRTUAL_ACC__ */

#if defined(__GT420D__)
void track_cust_poweron_modeled_init(void)
{
    if(G_parameter.extchip.mode ==1)
        {
            track_cust_send17((void*)2,(void*)1);
            }
    else
        {
            track_cust_send17((void*)1,(void*)1);
            }
        }
#endif
