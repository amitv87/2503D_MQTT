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
#include "c_vector.h"

/*****************************************************************************
*  Define                               宏定义
******************************************************************************/

/*****************************************************************************
*  Typedef  Enum                        枚举定义
*****************************************************************************/

/*****************************************************************************
*  Struct                               数据结构定义
******************************************************************************/

/*****************************************************************************
*  Local variable                       局部变量
*****************************************************************************/
static enum_alarm_type s_alarm_type = 0;
static c_vector* vec_alarm_queue = NULL;

#if defined(__ACC_ALARM__)
#if defined(__GERMANY__)
static char *str_accalm_changeoff_en = "Achtung! Zuendung ausgeschaltet";
#else
static char *str_accalm_changeoff_en = "Attention please!ACC OFF";
#endif
static char str_accalm_changeoff_cn[16] = //请注意！车辆熄火
{0x8B, 0xF7, 0x6C, 0xE8, 0x61, 0x0F, 0xFF, 0x01, 0x8F, 0x66, 0x8F, 0x86, 0x71, 0x84, 0x70, 0x6B};

#if defined(__GERMANY__)
static char *str_accalm_changeon_en = "Achtung! Zuendung eingeschaltet";
#else
static char *str_accalm_changeon_en = "Attention please!ACC ON";
#endif
static char str_accalm_changeon_cn[16] = // 请注意！车辆点火
{0x8B, 0xF7, 0x6C, 0xE8, 0x61, 0x0F, 0xFF, 0x01, 0x8F, 0x66, 0x8F, 0x86, 0x70, 0xB9, 0x70, 0x6B};
#endif
/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/
kal_uint8 g_alarm_gps_status = 0;//报警记录报警期间GPS定位情况//0,不定位;1,定位

/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/
extern kal_uint8 g_test_flag;
/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/
extern void track_cus_alarm_vibrates_overTime_sendsms(void);
extern void track_cust_soscall_OverTime_sendsms(void);
/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/
extern kal_uint16 track_cust_check_gprsdata(kal_uint32 flag, kal_uint16 sn);
#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      本地函数
******************************************************************************/

/******************************************************************************
 *  Function    -  track_alarm_vec_queue_push
 *
 *  Purpose     -  报警数据包请求压入队列
 *
 *  Description -
 *  多围栏项目需要有单个围栏报警方式
 *  所以是增加一个围栏 编号，还是增加
 *  是否发短信标示，目前暂时加一个短信标示，for GT500 only Liujw 2014-06-10
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_alarm_vec_queue_push(kal_uint8 alarm_type, kal_uint16 sn, kal_uint8 call_type)
{
    kal_uint32 value = 0;
    kal_uint8 *p = (kal_uint8 *)&value;
    if(NULL == vec_alarm_queue)
    {
        VECTOR_CreateInstance(vec_alarm_queue);
    }
    if(VECTOR_Size(vec_alarm_queue) > 20)
    {
        LOGD(L_APP, L_V4, "报警队列中已经有%d条报警记录了", VECTOR_Size(vec_alarm_queue));
        VECTOR_Erase(vec_alarm_queue, 0, 1);
    }
    memcpy(p, &sn, 2);
    memcpy(p + 2, &alarm_type, 1);
    memcpy(p + 3, &call_type, 1);
    LOGD(L_APP, L_V5, "alarm_type:%d, sn:%0.4X, value:%X", alarm_type, sn, value);
    VECTOR_PushBack(vec_alarm_queue, (void*)value);
}

/******************************************************************************
 *  Function    -  track_alarm_vec_queue_remove
 *
 *  Purpose     -  从队列中移除报警
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint32* track_alarm_vec_queue_remove(kal_uint8 mode, kal_uint16 sn, kal_uint8 alarm_type)
{
    kal_uint32 *value;
    kal_uint8    _alarm_type;
    kal_uint16    _sn;
    int max, i;
    if(NULL == vec_alarm_queue)
    {
        VECTOR_CreateInstance(vec_alarm_queue);
    }
    max = VECTOR_Size(vec_alarm_queue);
    LOGD(L_APP, L_V5, "mode:%d, sn:%0.4X, VECTOR_Size(vec_alarm_queue):%d", mode, sn, max);
    if(max <= 0)
    {
        return 0;
    }
    for(i = 0; i < max; i++)
    {
        value = VECTOR_At(vec_alarm_queue, i);
        memcpy(&_sn, (kal_uint8 *)&value, 2);
        memcpy(&_alarm_type, ((kal_uint8 *)(&value)) + 2, 1);
        LOGD(L_APP, L_V5, "alarm_type:%d, sn:%0.4X, value:%X", _alarm_type, _sn, value);
        if(mode == 1 && _sn == sn)
        {
            VECTOR_Erase(vec_alarm_queue, i, 1);
            return value;
        }
        else if(mode == 2 && _alarm_type == alarm_type)
        {
            VECTOR_Erase(vec_alarm_queue, i, 1);
            return track_alarm_vec_queue_remove(mode, sn, alarm_type);
        }
        else if(mode == 3)
        {
            VECTOR_Erase(vec_alarm_queue, i, 1);
            return value;
        }
    }
    return 0;
}
kal_uint32 track_alarm_vec_queue_check(kal_uint8 alarm_type)
{
    kal_uint32 *value;
    kal_uint8    _alarm_type;
    kal_uint16    _index = 0;
    int max, i;
    if(NULL == vec_alarm_queue)
    {
        VECTOR_CreateInstance(vec_alarm_queue);
    }
    max = VECTOR_Size(vec_alarm_queue);
    LOGD(L_APP, L_V5, "VECTOR_Size(vec_alarm_queue):%d", max);
    if(max <= 0)
    {
        return 0;
    }
    for(i = 0; i < max; i++)
    {
        value = VECTOR_At(vec_alarm_queue, i);
        memcpy(&_alarm_type, ((kal_uint8 *)(&value)) + 2, 1);
        LOGD(L_APP, L_V5, "alarm_type:%d,value:%X", _alarm_type, value);
        if(_alarm_type == alarm_type)
        {
            _index++;
        }
    }
    LOGD(L_APP, L_V5, "_index:%d", _index);
    return _index;
}
/*****************************************************************************
*  Global Functions                     全局函数
******************************************************************************/

/******************************************************************************
 *  Function    -  track_cust_module_alarm_lock
 *
 *  Purpose     -  报警状态记录，避免同一报警未处理完，又有新的报警触发
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int8 track_cust_module_alarm_lock(enum_alarm_type type, kal_uint8 value)
{
    static kal_uint8 status[TR_CUST_ALARM_MAX] = {0};
    LOGD(L_APP, L_V4, "%d,%d", type, value);

	if(type == 98)
    {
        int i;
        for(i = 0; i < TR_CUST_ALARM_MAX; i++)
        {
            if(status[i] == 1) return 1;
        }
        return 0;
    }
    else if(type == 99)
    {
        memset(status, 0, sizeof(status));
        return 0;
    }
    else if(type >= 0 && type < TR_CUST_ALARM_MAX)
    {
        if(value == 0 || value == 1)
        {
            status[type] = value;
        }
        return status[type];
    }
    return -1;
}

static void track_blank(void)
{

}

/******************************************************************************
 *  Function    -  track_cust_module_alarm_recv_addr
 *
 *  Purpose     -  从网络接收到得报警包
 *
 *  Description -  暂时无法区分收到的报警包是什么类型的报警
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 27-02-2013,  Cml  written
 * v1.1  , 14-10-2013,  wangqi  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_alarm_recv_addr(char *number, kal_uint8 *content, int content_len, kal_uint16 sn)
{
    U8 phbNumIndex = 0;
    kal_bool flag = KAL_TRUE, on = KAL_FALSE, call = KAL_FALSE;

    kal_uint32* value;
    kal_uint8 alarm_type, call_type;
#ifndef __XCWS__
    value = track_alarm_vec_queue_remove(1, sn, 0);
    memcpy(&call_type, (kal_uint8 *)&value + 3, 1);
    memcpy(&alarm_type, ((kal_uint8 *)(&value)) + 2, 1);
    LOGD(L_APP, L_V4, "收到服务器下发的报警地址，向SOS号码发送报警短信 sn=%X, alarm_type=%d call_type=%d", sn, alarm_type, call_type);
    /*if(alarm_type == 0xFFFF)
    {
        alarm_type = track_alarm_vec_queue_remove(3, 0, 0);
        LOGD(L_APP, L_V4, "不能在队列中找到对应的报警请求，直接移除最早一个报警 %d, alarm_type=%d", sn, alarm_type);
    }*/
#if defined (__GT370__)|| defined (__GT380__)|| defined(__GT500__)  ||defined(__GT03F__) ||defined(__ET200__)||defined(__ET130__) || defined(__GW100__)|| defined(__V20__) || defined(__ET310__)||defined(__GT300S__)\
     || defined(__MT200__)||defined (__ET320__)
    if(track_cust_check_gprsdata(99, 0xff) == sn)
    {
        track_cust_check_gprsdata(0, 0xff);
    }
#endif /*defined (__GT300__) || defined(__GT500__)  ||defined(__GT03F__)*/

    //*************START********************//用于LBS GPS双报警报流程判断其它可以屏蔽这段
    if((call_type == 0) && (track_alarm_vec_queue_check(alarm_type) >= 1))
    {
        call_type = 1;
        LOGD(L_APP, L_V6, "重置报警电话标志");
    }
    //*************END*********************//


    switch(alarm_type)
    {
        case TR_CUST_ALARM_NORMAL:
            //on = KAL_TRUE;
            LOGD(L_APP, L_V4, "该报警无需短信通知用户!");
            track_fun_view_UTF16BE_string(content, content_len);
            return;
            break;
        case TR_CUST_ALARM_SOS:
            tr_stop_timer(TRACK_CUST_SOS_SMS_OVERTIME_TIMER);
            if(G_parameter.sos_alarm.alarm_type == 0)
            {
                break;
            }
            else if((G_parameter.sos_alarm.alarm_type >= 2) && ((call_type & 1) == 1))
            {
                call = KAL_TRUE;
            }
            on = KAL_TRUE;
#if defined (__GW100__)
            call = KAL_FALSE;
            if(G_parameter.sos_alarm.alarm_type != 2 && G_parameter.sos_alarm.alarm_type != 1)
            {
                on = KAL_FALSE;
            }
#endif /* __GW100__ */
            break;

#if defined (__GT380__)
            //充电+断电报警
            case TR_CUST_ALARM_PowerFailure:
            case TR_CUST_ALARM_CHARGE_IN_ALM:
                    tr_stop_timer(TRACK_CUST_ALARM_power_fails_OVERTIME_TIMER);
                    if(G_parameter.charge_alarm.alarm_type == 0)
                    {
                        break;
                    }
                    else if((G_parameter.charge_alarm.alarm_type == 2 || G_parameter.charge_alarm.alarm_type == 3) && ((call_type & 1) == 1))
                    {
                        call = KAL_TRUE;
                    }
                    if(G_parameter.charge_alarm.alarm_type != 3)
                    {
                        on = KAL_TRUE;
                    }   
                    break;
#else
        case TR_CUST_ALARM_PowerFailure:
            tr_stop_timer(TRACK_CUST_ALARM_power_fails_OVERTIME_TIMER);
            
                if(G_parameter.power_fails_alarm.alarm_type == 0)
                {
                    break;
                }
                else if((G_parameter.power_fails_alarm.alarm_type == 2 || G_parameter.power_fails_alarm.alarm_type == 3) && ((call_type & 1) == 1))
                {
                    call = KAL_TRUE;
                }
                if(G_parameter.power_fails_alarm.alarm_type != 3)
                {
                    on = KAL_TRUE;
                }
            break;
#endif /* __GT380__ */
      
        case TR_CUST_ALARM_Vibration:
            tr_stop_timer(TRACK_CUST_ALARM_alarm_vibrates_OVERTIME_TIMER);
            if(G_parameter.vibrates_alarm.alarm_type == 0)
            {
                break;
            }
            if((G_parameter.vibrates_alarm.alarm_type == 2 || G_parameter.vibrates_alarm.alarm_type == 3) && ((call_type & 1) == 1))
            {
                call = KAL_TRUE;
            }
            if(G_parameter.vibrates_alarm.alarm_type != 3)
            {
                on = KAL_TRUE;
            }
            break;
        case TR_CUST_ALARM_LowBattery:
            tr_stop_timer(TRACK_CUST_ALARM_LowBattery_OVERTIME_TIMER);
            if(G_parameter.low_power_alarm.alarm_type == 0)
            {
                break;
            }
             #if defined(__GT420D__)
            if((track_cust_gps_status()==0)&&track_cust_wifi_scan_status()==0)
        {
               LOGD(L_APP, L_V4, "GPS,wifi已关闭30s后可关机");
             track_cust_module_delay_close(30);
        }
            break;
#endif
            //on = KAL_TRUE; //低电报警取消服务器返回带地址的报警
            break;
        case TR_CUST_ALARM_InFence:
        case TR_CUST_ALARM_OutFence:
            tr_stop_timer(TRACK_CUST_FENCE_OVERTIME_TIMER);
#if defined(__MORE_FENCE__)
            if(call_type & 1)
            {
                call = KAL_TRUE;
            }
            if(!(call_type & 2))
#else
            if(G_parameter.fence[0].alarm_type == 0)
#endif
            {
                LOGD(L_APP, L_V4, "电子围栏不做短信报警");
                break;
            }

            on = KAL_TRUE;
            break;
        case TR_CUST_ALARM_Speed_Limit:
            //tr_stop_timer(TRACK_CUST_ALARM_speed_limit_SMS_OVERTIME_TIMER);
            if(G_parameter.speed_limit_alarm.alarm_type == 0)
            {
                break;
            }
            //on = KAL_TRUE;
            break;
        case TR_CUST_ALARM_displacement:
            tr_stop_timer(TRACK_CUST_ALARM_displacement_OVERTIME_TIMER);
            if(G_parameter.displacement_alarm.alarm_type == 0)
            {
                break;
            }
            if((G_parameter.displacement_alarm.alarm_type == 2 || G_parameter.displacement_alarm.alarm_type == 3) && ((call_type & 1) == 1))
            {
                call = KAL_TRUE;
            }
            on = KAL_TRUE;

    #if defined (__NT33__)
        track_cust_no_acc_disable_moving_once_alarm();
    #endif
            break;
        case TR_CUST_ALARM_PRE_REMOVE:
#if defined(__GT740__)
	if(track_cust_remove_alarm_lock(99) || G_realtime_data.tracking_count>1 ||  G_realtime_data.tracking_count==0)
	{
		return;
	}
	track_cust_remove_alarm_lock(1);
#endif
    #if defined(__GT420D__)
    track_cust_remove_alarm_lock(1);
    track_cust_module_alarm_lock(TR_CUST_ALARM_PRE_REMOVE, 0);
    #endif
            tr_stop_timer(TRACK_CUST_EXTCHIP_DELAY_SEND_ALARM_MSG_TIMER_ID);
            if(G_parameter.teardown_str.alarm_type == 0)
            {
                break;
            }
            if((G_parameter.teardown_str.alarm_type == 2 || G_parameter.teardown_str.alarm_type == 3) && ((call_type & 1) == 1))
            {
                //call = KAL_TRUE;
            }
            if(G_parameter.teardown_str.alarm_type != 3)
            {
#if defined (__GT370__)|| defined (__GT380__)
            if(track_is_timer_run(TRACK_CUST_PRE_REMOVEL_SMS_FILTER_TIMER_ID))
            {
                LOGD(L_APP, L_V4, "3分钟内已发拆除短信");
                 on = KAL_FALSE;
            }
            else
            {
                tr_start_timer(TRACK_CUST_PRE_REMOVEL_SMS_FILTER_TIMER_ID, 170*1000, track_blank);
                 on = KAL_TRUE;
            }

#else
                on = KAL_TRUE;
#endif /* __GT370__ */
            }
            #if defined(__GT420D__)
            if((track_cust_gps_status()==0)&&track_cust_wifi_scan_status()==0)
        {
               LOGD(L_APP, L_V4, "GPS,wifi已关闭30s后可关机");
             track_cust_module_delay_close(30);
        }
            break;
#endif
        case TR_CUST_ALARM_OPEN_SHELL:
#if defined (__GT370__)|| defined (__GT380__)
            tr_stop_timer(TRACK_CUST_SEND_OPEN_SHELL_SMS_TIMEOVER_TIMER_ID);
            if(G_parameter.open_shell_alarm.alarm_type == 0)
            {
                break;
            }
            if((G_parameter.open_shell_alarm.alarm_type == 2 || G_parameter.open_shell_alarm.alarm_type == 3) && ((call_type & 1) == 1))
            {
                call = KAL_TRUE;
            }
            if(G_parameter.open_shell_alarm.alarm_type != 3)
            {
                if(track_is_timer_run(TRACK_CUST_OPEN_SHELL_SMS_FILTER_TIMER_ID))
                {
                    LOGD(L_APP, L_V4, "3分钟内已发开盖短信");
                     on = KAL_FALSE;
                }
                else
                {
                    tr_start_timer(TRACK_CUST_OPEN_SHELL_SMS_FILTER_TIMER_ID, 170*1000, track_blank);
                     on = KAL_TRUE;
                }
            }
#endif /* __GT370__ */
            break;
    }

    LOGD(L_APP, L_V4, "准备就绪，发送地址短信给sos号码哦 on=%d,g_test_flag=%d", on, g_test_flag);
    if(on || g_test_flag)
    {
        for(phbNumIndex = 0; phbNumIndex < TRACK_DEFINE_SOS_COUNT; phbNumIndex++)
        {
            if((TR_CUST_ALARM_SOS == alarm_type || G_phone_number.almrep[phbNumIndex]) && strlen(G_parameter.sos_num[phbNumIndex]))
            {
                track_cust_sms_send(0, G_parameter.sos_num[phbNumIndex], 0, content, content_len);
                flag = KAL_FALSE;
#ifdef __GW100__
                if(!G_parameter.Permission.AlarmNumCount && !(TR_CUST_ALARM_SOS == alarm_type))
#else
                if(!G_parameter.Permission.AlarmNumCount)
#endif /* __GW100__ */
                {
                    break;
                }
            }
        }
        if(flag)
        {
            LOGD(L_APP, L_V4, "SOS号码未设置，报警短信发送不成功");
        }
    }
    s_alarm_type = 0;
    if(call)
    {
        LOGD(L_APP, L_V4, "60 秒后拨打电话。");
        track_start_timer(TRACK_CUST_ALARM_MAKECALL_TIMER, 60000, track_cust_make_call, (void*)1);
    }
    else
    {
#if defined (__GW100__)

#else
        if(G_parameter.sos_alarm.alarm_type < 2 || alarm_type != TR_CUST_ALARM_SOS)
#endif /* __GW100__ */
        {
            track_cust_module_alarm_lock(99, 0);
        }
    }
#endif
}

/******************************************************************************
 *  Function    -  track_cust_module_alarm_msg_send
 *
 *  Purpose     -  触发报警后，发送短信的统一处理函数
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * v1.1  , 11-10-2013,  wangqi modify
 * ----------------------------------------
*******************************************************************************/
kal_bool track_cust_module_alarm_msg_send(struct_msg_send *msg, track_gps_data_struct *gps_data)
{
    int len, len2;
    kal_bool flag = KAL_FALSE;

    LOGD(L_APP, L_V5, "cm_type=%d, %s, almrep:%d", msg->cm_type, msg->msg_en, msg->not_need_almrep);
    if(msg->addUrl)
    {
        char tmp[320] = {0};
        if(gps_data != NULL)
        {
            if(gps_data->gprmc.status)
            {
                track_cust_get_url_str(tmp, &gps_data->gprmc);
                len = strlen(tmp);
                len2 = strlen(msg->msg_en);
                if(len2 + len < CM_PARAM_LONG_LEN_MAX)
                {
                    sprintf(&msg->msg_en[len2], "%s", tmp);
                }
                if((msg->msg_cn_len + len * 2) < CM_PARAM_LONG_LEN_MAX)
                {
                    if(msg->msg_cn_len == 12 && msg->msg_cn[10] == 0xFF && msg->msg_cn[11] == 0x01)
                    {
                        // 为了保证一条短信能发送完，节省短信，适当去掉提示信息中的感叹号。
                        msg->msg_cn_len = 10;
                    }
                    len = track_fun_asc_str_to_ucs2_str(&msg->msg_cn[msg->msg_cn_len], tmp) - 2;
                    LOGH(L_APP, L_V6, &msg->msg_cn[msg->msg_cn_len], len);
                    msg->msg_cn_len += len;
                }
            }
            else
            {
                LOGD(L_APP, L_V5, "gprmc.status:%d", gps_data->gprmc.status);
            }
        }
    }
    if(msg->cm_type == CM_ONLINE)
    {
        track_cust_paket_msg_upload(msg->stamp, KAL_TRUE, msg->msg_en, strlen(msg->msg_en));
        flag = KAL_TRUE;
    }
    else if(msg->cm_type == CM_ONLINE2)
    {
        track_cust_paket_FD_cmd(msg->stamp, KAL_TRUE, msg->msg_en, strlen(msg->msg_en));
        flag = KAL_TRUE;
    }
    else if(msg->cm_type == CM_SMS)
    {
        int i;
        kal_bool isChar = KAL_TRUE;
        char *data;

        if(G_parameter.lang == 1)
        {
            isChar = KAL_FALSE;
            len = msg->msg_cn_len;
            data = msg->msg_cn;
        }
        else
        {
            isChar = KAL_TRUE;
            len = strlen(msg->msg_en);
            data = msg->msg_en;
        }
        if(strlen(msg->phone_num) == 0)
        {
            LOGH(L_APP, L_V6, data, len);
            for(i = 0; i < TRACK_DEFINE_SOS_COUNT; i++)
            {
                if((msg->not_need_almrep == 1 || G_phone_number.almrep[i]) && strlen((S8*) G_parameter.sos_num[i]))
                {
                    track_cust_sms_send(0, G_parameter.sos_num[i], isChar, data, len);
                    flag = KAL_TRUE;
#ifdef __GW100__
                    if(!G_parameter.Permission.AlarmNumCount && !(msg->is_sos_alarm_type))
#else
                    if(!G_parameter.Permission.AlarmNumCount)
#endif /* __GW100__ */
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            track_cust_sms_send(0, msg->phone_num, isChar, data, len);
        }
    }
    else if(msg->cm_type == CM_AT)
    {
        LOGS("%s", msg->msg_en);
        flag = KAL_TRUE;
    }
    return flag;
}

/******************************************************************************
 *  Function    -  track_cust_alarm_type
 *
 *  Purpose     -  设置报警类型
 *
 *  Description -   type == 0  为查询当前报警类型
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 09-08-2013,  ZengPing && Liujw  written
 * ----------------------------------------
 ******************************************************************************/
#if defined (__XYBB__)
enum_alarm_type track_cust_alarm_type(enum_alarm_type type)
{
    static enum_alarm_type s_alarm_type = 0;
	LOGD(L_APP,L_V5,"type = %x",type);
	if(type == TR_CUST_ALARM_MAX)
	{
		s_alarm_type = 0;
		
	}
    else if(type != 0)
    {
        s_alarm_type = type;
    }
    return s_alarm_type;
}

#else
enum_alarm_type track_cust_alarm_type(enum_alarm_type type)
{
    static enum_alarm_type s_alarm_type = 0;
    if(type != 0)
    {
        s_alarm_type = type;
    }
    return s_alarm_type;
}
#endif /* __XYBB__ */


/******************************************************************************
 *  Function    -  track_cust_send_paket_16
 *
 *  Purpose     -  GPS位置请求
 *
 *  Description -   flag :0:不上传1:上传2: 设定上传期望值
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 05-08-2013,  ZengPing && Liujw  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_send_paket_16(kal_uint32 flag, track_gps_data_struct *gd, kal_uint8 call_type)
{
    U16 sn, waiting_time = 60;
    static kal_uint32 status = 0;
    if(flag == 0)
    {
        status = 0;
    }
    else if(flag == 1 &&  status == 1)
    {
        status = 0;
        g_alarm_gps_status = 1;
        LOGD(L_APP, L_V6, "g_alarm_gps_status =%d,status:%d", g_alarm_gps_status, track_soc_login_status());
#if defined(__GW100__)
        if(track_cust_gprson_status(99) == 1)
        {
            sn = track_cust_paket_16(gd, track_drv_get_lbs_data(), track_cust_alarm_type(0), KAL_TRUE, call_type);
        }
#else
        sn = track_cust_paket_16(gd, track_drv_get_lbs_data(), track_cust_alarm_type(0), KAL_TRUE, call_type);
#endif
#if defined(__GW100__)
        if(track_is_timer_run(TRACK_CUST_SOS_GPS_DELAY_TIMER_ID))
        {
            tr_stop_timer(TRACK_CUST_SOS_GPS_DELAY_TIMER_ID);
            if(track_cust_gprson_status(99) == 1)
            {
                tr_start_timer(TRACK_CUST_SOS_SMS_OVERTIME_TIMER, 60 * 1000, track_cust_soscall_OverTime_sendsms);
            }
            else
            {
                track_cust_soscall_OverTime_sendsms();
            }
        }
#else
        if(track_is_timer_run(TRACK_CUST_SOS_SMS_OVERTIME_TIMER))
        {
            tr_start_timer(TRACK_CUST_SOS_SMS_OVERTIME_TIMER, 60 * 1000, track_cust_soscall_OverTime_sendsms);
        }
#endif
        if(track_is_timer_run(TRACK_CUST_ALARM_alarm_vibrates_OVERTIME_TIMER))
        {
#if defined(__GT370__)|| defined (__GT380__)
            if(track_soc_login_status() == 0)
            {
                waiting_time = 2;
            }
#endif
            tr_start_timer(
                TRACK_CUST_ALARM_alarm_vibrates_OVERTIME_TIMER,
                waiting_time * 1000,
                track_cus_alarm_vibrates_overTime_sendsms);
        }
    }
    else if(flag == 2)
    {
        status = 1;
    }
}
#if defined(__ACC_ALARM__)
/******************************************************************************
 *  Function    -  accalm_sendsms
 *
 *  Purpose     -  发送ACC变化报警短信
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 27-10-2016,  Cjj  written
 * ----------------------------------------
*******************************************************************************/
void accalm_sendsms(kal_uint8 par)
{
    struct_msg_send g_msg = {0};
    if(G_parameter.accalm.accalm_type != 1)
    {
        return;
    }
    LOGD(L_APP, L_V5, "");
    g_msg.addUrl = KAL_FALSE;
    g_msg.cm_type = CM_SMS;
    if(par == 0)
    {
        snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_accalm_changeoff_en);
        g_msg.msg_cn_len = 16;
        memcpy(g_msg.msg_cn, str_accalm_changeoff_cn, g_msg.msg_cn_len);
    }
    else
    {
        snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_accalm_changeon_en);
        g_msg.msg_cn_len = 16;
        memcpy(g_msg.msg_cn, str_accalm_changeon_cn, g_msg.msg_cn_len);
    }
    track_cust_module_alarm_msg_send(&g_msg, NULL);

}
void track_cust_acc_change_alarm(void *arg)
{
    kal_uint32 par = (kal_uint32)arg;
    if(G_parameter.accalm.accalm_sw == 1)
    {
        if(par == 0)
        {
           if(G_parameter.accalm.accalm_cause == 0 || G_parameter.accalm.accalm_cause == 1)
           {
            track_cust_paket_16(track_cust_gpsdata_alarm(), track_drv_get_lbs_data(), TR_CUST_ALARM_ACC_OFF, KAL_TRUE, 1);
            accalm_sendsms(0);
           }
        }
        else
        {
          if(G_parameter.accalm.accalm_cause == 0 || G_parameter.accalm.accalm_cause == 2)
          {
            track_cust_paket_16(track_cust_gpsdata_alarm(), track_drv_get_lbs_data(), TR_CUST_ALARM_ACC_ON, KAL_TRUE, 1);
            accalm_sendsms(1);
          }
        }
    }

}
#endif

void track_cust_alarm_acc_change(void *arg)
{
    kal_uint32 par = (kal_uint32)arg;
    LOGD(L_APP, L_V5, "");
    if(G_parameter.accalm_sw == 1)
    {
        char *on  = "Attention please! ACC ON!";
        char *off = "Attention please! ACC OFF!";

		char on_cn[]={0x8B,0xF7,0x6C,0xE8,0x61,0x0F,0xFF,0x01,0x00,0x41,\
			0x00,0x43,0x00,0x43,0x5D,0xF2,0x62,0x53,0x5F,0x00,0xFF,0x01,0x00,0x00};		//请注意！ACC已打开！

		char off_cn[]={0x8B,0xF7,0x6C,0xE8,0x61,0x0F,0xFF,0x01,0x00,0x41,\
			0x00,0x43,0x00,0x43,0x5D,0xF2,0x51,0x73,0x95,0xED,0xFF,0x01,0x00,0x00};	//请注意！ACC已关闭！
				
        char *out_string;
        int i;		

		if(G_parameter.lang == 0)
		{
	        if(par == 0)
	        {
	            out_string = off;
	        }
	        else if(par == 1)
	        {
	            out_string = on;
	        }
		
	        for(i = 0; i < TRACK_DEFINE_SOS_COUNT; i++)
	        {
	            if(G_phone_number.almrep[i] && strlen((S8*)G_parameter.sos_num[i]))
	            {
	                track_cust_sms_send(0, G_parameter.sos_num[i], KAL_TRUE, out_string, strlen(out_string));
	                if(!G_parameter.Permission.AlarmNumCount)
	                {
	                    break;
	                }
	            }
	        }
		}
		else
		{
	        if(par == 0)
	        {
	            out_string = off_cn;
	        }
	        else if(par == 1)
	        {
	            out_string = on_cn;
	        }
		
	        for(i = 0; i < TRACK_DEFINE_SOS_COUNT; i++)
	        {
	            if(G_phone_number.almrep[i] && strlen((S8*)G_parameter.sos_num[i]))
	            {
	                track_cust_sms_send(0, G_parameter.sos_num[i], KAL_FALSE, out_string, sizeof(out_string));
	                if(!G_parameter.Permission.AlarmNumCount)
	                {
	                    break;
	                }
	            }
	        }
		}	
    }
}

#if defined(__XCWS__)
static const char alarm_str_1[] = {0};
static const char alarm_str_2[] = {0};
static const char alarm_str_3[] = {0};
static const char alarm_str_4[] = {0};
static const char alarm_str_5[] = {0};
static const char alarm_str_6[] = {0};
static const char alarm_str_7[] = {0};
static const char alarm_str_8[] = {0};
static const char alarm_str_9[] = {0};
#if 0
//您的爱车于
static const char alarm_str_1[] = {0x60, 0xA8, 0x76, 0x84, 0x72, 0x31, 0x8F, 0x66, 0x4E, 0x8E, 0x00, 0x00};

//点
static const char alarm_str_2[] = {0x70, 0xB9, 0x00, 0x00};

//分发生异常振动，
static const char alarm_str_3[] = {0x52, 0x06, 0x53, 0xD1, 0x75, 0x1F, 0x5F, 0x02, 0x5E, 0x38, 0x63, 0x2F, 0x52, 0xA8, 0xFF, 0x0C, 0x00, 0x00};

//回复DW查询车辆当前位置信息!
static const char alarm_str_4[] = {0x56, 0xDE, 0x59, 0x0D, 0x00, 0x44, 0x00, 0x57, 0x67, 0xE5, 0x8B, 0xE2, 0x8F, 0x66, 0x8F, 0x86, 0x5F, 0x53, 0x52, 0x4D, 0x4F, 0x4D, 0x7F, 0x6E, 0x4F, 0xE1, 0x60, 0x6F, 0xFF, 0x01, 0x00, 0x00};

//分监控器电量不足，请及时接入车载电池。
static const char alarm_str_5[] = {0x52, 0x06, 0x8B, 0xBE, 0x59, 0x07, 0x75, 0x35, 0x91, 0xCF, 0x4E, 0x0D, 0x8D, 0xB3, 0xFF, 0x0C, 0x8B, 0xF7, 0x53, 0xCA, 0x65, 0xF6, 0x63, 0xA5, 0x51, 0x65, 0x8F, 0x66, 0x8F, 0x7D, 0x75, 0x35, 0x6C, 0x60, 0x30, 0x02, 0x00, 0x00};

//分离开您的停车地点，
static const char alarm_str_6[] = {0x52, 0x06, 0x79, 0xBB, 0x5F, 0x00, 0x60, 0xA8, 0x76, 0x84, 0x50, 0x5C, 0x8F, 0x66, 0x57, 0x30, 0x70, 0xB9, 0xFF, 0x0C, 0x00, 0x00};

//分发生设备供电异常，请检查车载电源连接状况和监控器供电情况。
static const char alarm_str_7[] = {0x52, 0x06, 0x53, 0xD1, 0x75, 0x1F, 0x8B, 0xBE, 0x59, 0x07, 0x4F, 0x9B, 0x75, 0x35, 0x5F, 0x02, 0x5E, 0x38, 0xFF, 0x0C, 0x8B, 0xF7, 0x68, 0xC0, 0x67, 0xE5, 0x8F, 0x66, 0x8F, 0x7D, 0x75, 0x35, 0x6E, 0x90, 0x8F, 0xDE, 0x63, 0xA5, 0x72, 0xB6, 0x51, 0xB5, 0x54, 0x8C, 0x76, 0xD1, 0x63, 0xA7, 0x56, 0x68, 0x4F, 0x9B, 0x75, 0x35, 0x60, 0xC5, 0x51, 0xB5, 0x30, 0x02, 0x00, 0x00};

//分进入设定区域，
static const char alarm_str_8[] = {0x52, 0x06, 0x8F, 0xDB, 0x51, 0x65, 0x8B, 0xBE, 0x5B, 0x9A, 0x53, 0x3A, 0x57, 0xDF, 0xFF, 0x0C, 0x00, 0x00};

//分离开设定区域，
static const char alarm_str_9[] = {0x52, 0x06, 0x79, 0xBB, 0x5F, 0x00, 0x8B, 0xBE, 0x5B, 0x9A, 0x53, 0x3A, 0x57, 0xDF, 0xFF, 0x0C, 0x00, 0x00};
#endif

/****************************************************************************
* Global Variable           全局变量
*****************************************************************************/


/****************************************************************************
* Global Variable - Extern          引用全局变量
*****************************************************************************/


/*****************************************************************************
 *  Global Functions	- Extern	    引用外部函数
*****************************************************************************/
extern char * track_cust_get_host_num(void);
extern track_gps_data_struct *track_cust_get_module_fence_alarm_locale(void);
/*****************************************************************************
 *  Local Functions	            本地函数
*****************************************************************************/

/******************************************************************************
 *  Function    -  track_alarm_sms_send
 *
 *  Purpose     -  向终端发送报警短信
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 26-03-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_alarm_sms_send(alarm_type_enum type)
{
    sms_msg_struct ssms = {0};
    char rsp_msg[320] = {0};
    char temp[8] = {0}, temp2[8] = {0}, hour[16] = {0}, min[16] = {0};
    applib_time_struct time = {0};
    extern  const char string30[];
    kal_uint16 ctxlen = 0;
    return;//test
    track_cust_get_time_from_system(KAL_FALSE, &time);

    snprintf(temp, sizeof(temp), "%02d", time.nHour);
    snprintf(temp2, sizeof(temp2), "%02d", time.nMin);

    track_fun_asc_str_to_ucs2_str(hour , temp);
    track_fun_asc_str_to_ucs2_str(min, temp2);

    LOGD(L_APP, L_V5, "[%02d:%02d] alarm=%d", time.nHour, time.nMin, type);

    switch(type)
    {
        case ALARM_SOS:                               //SOS 报警

            break;
        case ALARM_BATTERY_LOW:               //电池低电报警
            kal_wsprintf((WCHAR *)rsp_msg, "%w%w%w%w%w%w", alarm_str_1, hour, alarm_str_2, min, alarm_str_5, alarm_str_4);
            break;
        case ALARM_EXT_BATTERY_LOW:         //外电低电报警
            kal_wsprintf((WCHAR *)rsp_msg, "%w%w%w%w%w%w", alarm_str_1, hour, alarm_str_2, min, alarm_str_7, alarm_str_4);
            break;
        case ALARM_EXT_BATTERY_CUT:          //外电断电报警

            break;
        case ALARM_VIBRATION:                      //震动报警
            kal_wsprintf((WCHAR *)rsp_msg, "%w%w%w%w%w%w", alarm_str_1, hour, alarm_str_2, min, alarm_str_3, alarm_str_4);
            break;
        case ALARM_FENCE_IN:                        //进围栏报警
            kal_wsprintf((WCHAR *)rsp_msg, "%w%w%w%w%w%w", alarm_str_1, hour, alarm_str_2, min, alarm_str_8, alarm_str_4);
            break;
        case ALARM_FENCE_OUT:                   //出围栏报警
            kal_wsprintf((WCHAR *)rsp_msg, "%w%w%w%w%w%w", alarm_str_1, hour, alarm_str_2, min, alarm_str_9, alarm_str_4);
            break;
        case ALARM_OVER_SPEED:                   //超速报警

            break;
        case ALARM_MOVE_OUT:                    //位移报警
            kal_wsprintf((WCHAR *)rsp_msg, "%w%w%w%w%w%w", alarm_str_1, hour, alarm_str_2, min, alarm_str_6, alarm_str_4);
            break;

        default:
            break;
    }
    app_ucs2_strcat((char *)rsp_msg, string30);
    ctxlen = app_ucs2_strlen((unsigned char *)rsp_msg) * 2;

    //track_cust_sms_send(0, "13414691211", KAL_TRUE, rsp_msg, ctxlen);

#if 0
    app_ucs2_strcat((char *)rsp_msg, string30);
    ssms.ctxlen = app_ucs2_strlen((unsigned char *)rsp_msg) * 2;

    ssms.numbers[0] = track_cust_get_host_num();
    if(strlen(ssms.numbers[0]) && ssms.ctxlen)
    {
        // ssms.ctxlen += 2;
        ssms.codetype = KAL_FALSE;
        ssms.ctx = rsp_msg;
        ssms.nocount = 1;
        track_sms_sender(&ssms);
    }
    else
    {
        LOGD(L_APP, L_V5, "Warning");
    }
#endif
}

/******************************************************************************
 *  Function    -  track_alarm_gprs_send
 *
 *  Purpose     -  向平台发送报警包
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 26-03-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_alarm_gprs_send(alarm_type_enum type)
{
    char buf[255] = {0};
    int len = 0;
    track_gps_data_struct *gpsdata = NULL;
    track_gps_data_struct *gpsLastPoint = NULL;

    LOGD(L_APP, L_V4, "packet data");

    gpsLastPoint = track_cust_backup_gps_data(0, NULL);
    switch(type)
    {
        case ALARM_SOS:                       //SOS 报警

            break;
        case ALARM_BATTERY_LOW:               //电池低电报警
            {
                kal_uint32 volt;
                volt = track_cust_get_battery_volt();
                len = track_cust_paket_T13(buf, sizeof(buf), volt);
            }
            break;
        case ALARM_EXT_BATTERY_LOW:         //外电低电报警
            {
                track_cust_packet_params_struct t12 = {0};
                gpsdata = track_drv_get_gps_data();
                if(gpsdata->gprmc.status != 0)
                {
                    t12.gd = gpsdata;
                }
                else
                {
                    t12.gd = gpsLastPoint;
                }

                t12.status = track_get_terminal_status();    //终端状态位
                t12.lbs = track_drv_get_lbs_data();             //LBS信息位
                t12.GSMSignalStrength = 0;
                t12.battery = 0;
                len = track_cust_paket_T12(buf, sizeof(buf), &t12);

            }
            break;
        case ALARM_EXT_BATTERY_CUT:          //外电断电报警

            break;
        case ALARM_VIBRATION:                      //震动报警/ 非法移动告警
            {
                track_cust_packet_params_struct t11 = {0};

                gpsdata = track_drv_get_gps_data();
                if(gpsdata->gprmc.status != 0)
                {
                    t11.gd = gpsdata;
                }
                else
                {
                    t11.gd = gpsLastPoint;
                }

                t11.status = track_get_terminal_status();    //终端状态位
                t11.lbs = track_drv_get_lbs_data();             //LBS信息位
                t11.GSMSignalStrength = 0;
                t11.battery = 0;
                len = track_cust_paket_T11(buf, sizeof(buf), &t11);

            }
            break;
        case ALARM_FENCE_IN:                     //进围栏报警
            {
                track_cust_packet_params_struct t21 = {0};

                gpsdata = track_cust_get_module_fence_alarm_locale();   //取围栏运算有效点，不是最新地址
                if(gpsdata != NULL)
                {
                    t21.gd = gpsdata;
                    t21.status = track_get_terminal_status();    //终端状态位
                    t21.lbs = track_drv_get_lbs_data();             //LBS信息位
                    t21.GSMSignalStrength = 0;
                    t21.battery = 0;
                    len = track_cust_paket_T21(buf, sizeof(buf), &t21, KAL_FALSE);
                }
            }
            break;
        case ALARM_FENCE_OUT:                   //出围栏报警
            {
                track_cust_packet_params_struct t21 = {0};

                gpsdata = track_cust_get_module_fence_alarm_locale();
                if(gpsdata != NULL)
                {
                    t21.gd = gpsdata;
                    t21.status = track_get_terminal_status();    //终端状态位
                    t21.lbs = track_drv_get_lbs_data();             //LBS信息位
                    t21.GSMSignalStrength = 0;
                    t21.battery = 0;
                    len = track_cust_paket_T21(buf, sizeof(buf), &t21, KAL_TRUE);
                }
            }
            break;
        case ALARM_OVER_SPEED:                   //超速报警

            break;
        case ALARM_MOVE_OUT:                    //位移报警
            {
                track_cust_packet_params_struct t14 = {0};
                gpsdata = track_drv_get_gps_data();
                if(gpsdata->gprmc.status != 0)
                {
                    t14.gd = gpsdata;
                }
                else
                {
                    t14.gd = gpsLastPoint;
                }

                t14.status = track_get_terminal_status();       //终端状态位
                t14.lbs = track_drv_get_lbs_data();                //LBS信息位
                t14.GSMSignalStrength = 0;
                t14.battery = 0;
                len = track_cust_paket_T14(buf, sizeof(buf), &t14);

            }
            break;

        default:
            break;
    }
    LOGD(L_APP, L_V5, "len=%d", len);
    if(len > 0)
    {
        track_socket_queue_alarm_send_reg(buf, len, 110, 110);
    }
    else
    {
        LOGD(L_APP, L_V5, "Warning! the size of this packet is too lang");
    }

}

/******************************************************************************
 *  Function    -  track_alarm_call_notiy
 *
 *  Purpose     -  报警电话通知
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 26-03-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_alarm_call_notiy(alarm_type_enum type)
{
    switch(type)
    {
        case ALARM_SOS:                               //SOS 报警

            break;
        case ALARM_BATTERY_LOW:               //电池低电报警

            break;
        case ALARM_EXT_BATTERY_LOW:         //外电低电报警

            break;
        case ALARM_EXT_BATTERY_CUT:          //外电断电报警

            break;
        case ALARM_VIBRATION:                      //震动报警

            break;
        case ALARM_FENCE_IN:                        //进围栏报警

            break;
        case ALARM_FENCE_OUT:                   //出围栏报警

            break;
        case ALARM_OVER_SPEED:                   //超速报警

            break;
        case ALARM_MOVE_OUT:                    //位移报警

            break;

        default:
            break;
    }
}

/******************************************************************************
 *  Function    -  track_cust_alarm_trigger_req
 *
 *  Purpose     -  报警接口
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 26-03-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_int8 track_cust_alarm_trigger_req(alarm_type_enum type, alarm_mode_enum mode)
{
    LOGD(L_APP, L_V4, "type=%d,mode=%d", type, mode);

    if((type >= ALARM_TYPE_MAX) || (mode >= ALARM_MODE_MAX))
    {
        return -1;
    }

    if(mode == ALARM_GPRS)
    {
        track_alarm_gprs_send(type);
    }
    else if(mode == ALARM_SMS)
    {
        track_alarm_sms_send(type);
    }
    else if(mode == ALARM_CALL)
    {
        track_alarm_call_notiy(type);
    }
    else
    {

    }
}
#if 1

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

/*****************************************************************************
*  Define                               宏定义
******************************************************************************/

/*****************************************************************************
*  Typedef  Enum                        枚举定义
*****************************************************************************/

/*****************************************************************************
*  Struct                               数据结构定义
******************************************************************************/

/*****************************************************************************
*  Local variable                       局部变量
*****************************************************************************/
//static enum_alarm_type s_alarm_type = 0;

/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/
extern kal_uint8 g_test_flag;
/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/

/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/
#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      本地函数
******************************************************************************/

/*****************************************************************************
*  Global Functions                     全局函数
******************************************************************************/

#if 0
/*        --    chengjun  2013-03-27*/
kal_int8 track_cust_module_alarm_lock(enum_alarm_type type, kal_uint8 value)
{
    static kal_uint8 status[TR_CUST_ALARM_MAX] = {0};
    if(type >= 0 && type < TR_CUST_ALARM_MAX)
    {
        if(value == 0 || value == 1)
        {
            status[type] = value;
        }
        return status[type];
    }
    else if(type == 99)
    {
        memset(status, 0, sizeof(status));
    }
    return -1;
}
#endif /* 0 */

/******************************************************************************
 *  Function    -  track_cust_module_alarm_recv_addr
 *
 *  Purpose     -  从网络接收到得报警包
 *
 *  Description -  暂时无法区分收到的报警包是什么类型的报警
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 27-02-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
#if 0
void track_cust_module_alarm_recv_addr(char *number, kal_uint8 *content, int content_len, kal_uint16 sn)
{
    /*nc       --    chengjun  2013-03-26*/
}


kal_int16 track_cust_module_alarm_set_type(kal_int16 alarm_type)
{
    if(alarm_type >= 0)
    {
        s_alarm_type = alarm_type;
    }
    return s_alarm_type;
}


void track_cust_module_alarm_msg_send(struct_msg_send *msg)
{
    int len, len2;

    LOGD(L_APP, L_V5, "cm_type=%d, %s", msg->cm_type, msg->msg_en);
    if(msg->addUrl)
    {
        track_gps_data_struct *gpsLastPoint = NULL;
        char tmp[320] = {0};
        gpsLastPoint = track_cust_backup_gps_data(0, NULL);
        if(gpsLastPoint != NULL)
        {
            if(gpsLastPoint->gprmc.status)
            {
                track_cust_get_url_str(tmp, &gpsLastPoint->gprmc);
                len = strlen(tmp);
                len2 = strlen(msg->msg_en);
                if(len2 + len < CM_PARAM_LONG_LEN_MAX)
                {
                    sprintf(&msg->msg_en[len2], "%s", tmp);
                }
                if((msg->msg_cn_len + len * 2) < CM_PARAM_LONG_LEN_MAX)
                {
                    if(msg->msg_cn_len == 12 && msg->msg_cn[10] == 0xFF && msg->msg_cn[11] == 0x01)
                    {
                        // 为了保证一条短信能发送完，节省短信，适当去掉提示信息中的感叹号。
                        msg->msg_cn_len = 10;
                    }
                    len = track_fun_asc_str_to_ucs2_str(&msg->msg_cn[msg->msg_cn_len], tmp) - 2;
                    LOGH(L_APP, L_V6, &msg->msg_cn[msg->msg_cn_len], len);
                    msg->msg_cn_len += len;
                }
            }
        }
    }
    if(msg->cm_type == CM_ONLINE)
    {
        //track_cust_paket_15(msg->stamp, msg->msg_en, strlen(msg->msg_en));
    }
    else if(msg->cm_type == CM_SMS)
    {
        int i;
        kal_bool isChar = KAL_TRUE;
        char *data;

        if(G_parameter.lang == 1)
        {
            isChar = KAL_FALSE;
            len = msg->msg_cn_len;
            data = msg->msg_cn;
        }
        else
        {
            isChar = KAL_TRUE;
            len = strlen(msg->msg_en);
            data = msg->msg_en;
        }
        if(strlen(msg->phone_num) == 0)
        {
            LOGH(L_APP, L_V6, data, len);
            for(i = 0; i < TRACK_DEFINE_SOS_COUNT; i++)
            {
                if(strlen((S8*) G_parameter.sos_num[i]))
                {
                    track_cust_sms_send(0, G_parameter.sos_num[i], isChar, data, len);
                    if(!G_parameter.Permission.AlarmNumCount)
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            track_cust_sms_send(0, msg->phone_num, isChar, data, len);
        }
    }
    else if(msg->cm_type == CM_AT)
    {
        LOGS(msg->msg_en);
    }
}
#endif /* 0 */
#endif

#endif
