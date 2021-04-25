/*******************************************************************************************
 * Filename:
 * Author :
 * Date :
 * Comment:
 ******************************************************************************************/

/****************************************************************************
*  Include Files                        ����ͷ�ļ�
*****************************************************************************/
#include "track_cust.h"
#include "track_at_call.h"

/*****************************************************************************
*  Define                               �궨��
******************************************************************************/

/*****************************************************************************
*  Typedef  Enum                        ö�ٶ���
*****************************************************************************/

/*****************************************************************************
*  Struct                               ���ݽṹ����
******************************************************************************/

/*****************************************************************************
*  Local variable                       �ֲ�����
*****************************************************************************/
#if defined(__GERMANY__)
static char* msg_alarm_sos_en = "Alarm am Fahrzeug!";
static char* msg_alarm_sos_en1 = "Alarm am Fahrzeug!";
#else
static char* msg_alarm_sos_en = "Emergency call!";
static char* msg_alarm_sos_en1 = "Emergency call!Please pay attention!";
#endif
static kal_uint8 msg_alarm_sos_cn[16] =   // �������У�
{
    0x7D, 0x27, 0x60, 0x25, 0x54, 0x7C, 0x53, 0xEB, 0xFF, 0x01
};
static kal_uint8 msg_alarm_sos_cn1[18] = // ��������!���ע��
{
    0x7D, 0x27, 0x60, 0x25, 0x54, 0x7C, 0x53, 0xEB, 0xFF, 0x01, 0x8B, 0xF7, 0x51, 0x73, 0x6C, 0xE8, 0xFF, 0x01
};

static struct_msg_send g_msg = {0};
static kal_uint8 g_call_status = 0;
static kal_uint8 g_monitor_call_status = 0;
static track_gps_data_struct gps_data_buf = {0};

static U16 g_last_alerm_no = 0;

/****************************************************************************
*  Global Variable                      ȫ�ֱ���
*****************************************************************************/
/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/
extern kal_uint8 g_alarm_gps_status;
extern char dail_num[TRACK_DEFINE_PHONE_NUMBER_LEN_MAX];
extern kal_uint32 dail_count;
/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/
extern void track_cust_call(char *number);      // ����
extern void track_cust_call_hangup(void);       // �Ҷ�
extern kal_bool track_is_in_call(void);          //�Ƿ���ͨ��״̬
extern kal_uint8 track_cust_dial_makecall(void * arg);
/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/
void track_cust_sos_call_status(kal_uint16 call_type, kal_uint16 callState);
void track_cust_make_call_function(void *arg);
void track_cust_send_paket_16(kal_uint32 flag, track_gps_data_struct *gd, kal_uint8 call_type);

#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/
static void Delayms(kal_uint16 data)
{
    kal_uint32 i;
    while(data--)
    {
        for(i = 0; i < 2000; i++) {}
    }
}

/*---------------------------------------------------------------------------
 * FUNCTION			ControlVibration
 *
 * DESCRIPTION		����������0.5S
 *
 * PARAMETERS
 *		kal_bool
 * RETURNS
 *  		void
 *
 *---------------------------------------------------------------------------*/
void ControlVibration(void)
{
    track_drv_set_vibr(KAL_TRUE);
    Delayms(200);
    track_drv_set_vibr(KAL_FALSE);
    Delayms(100);
    track_drv_set_vibr(KAL_TRUE);
    Delayms(200);
    track_drv_set_vibr(KAL_FALSE);
}

/*---------------------------------------------------------------------------
 * FUNCTION			track_cust_Vibration_100ms
 *
 * DESCRIPTION		����������100ms
 *
 * PARAMETERS
 *		kal_bool
 * RETURNS
 *  		void
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-10-2013,  wangqi  written
 * ----------------------------------------
 *---------------------------------------------------------------------------*/
void track_cust_Vibration_100ms(void)
{
    track_drv_set_vibr(KAL_TRUE);
    Delayms(100);
    track_drv_set_vibr(KAL_FALSE);
}

typedef struct
{
    kal_uint32 keep_on;
    kal_uint32 keep_off;
    kal_uint8   overtimer;
} vib_setting_struct;

vib_setting_struct * track_cust_vib_interval(kal_uint8 overtime, kal_uint32 ontime, kal_uint32 offtime)
{
    static vib_setting_struct vib_setting = {1000, 2000, 20};

    if(overtime || ontime || offtime)
    {
        vib_setting.overtimer = overtime;
        vib_setting.keep_on = ontime;
        vib_setting.keep_off = offtime;
    }

    LOGD(L_APP, L_V6, "on:%d, off:%d, count:%d", vib_setting.keep_on, vib_setting.keep_on, vib_setting.overtimer);
    return &vib_setting;
}
/*---------------------------------------------------------------------------
 * FUNCTION			track_cust_ring_Vibration
 *
 * DESCRIPTION		ѭ����
 *  ������һ�� 20����Ҫһ���ӣ���һ�ε绰���ʱ��һ���ӣ�������쳣
 *  û���¼��ر��𶯣��˺��������Լ������ر��𶯡�
 * PARAMETERS
 *		kal_bool
 * RETURNS
 *  		void
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-10-2013,  wangqi  written
 * ----------------------------------------
 *---------------------------------------------------------------------------*/
void track_cust_ring_Vibration(void * go_on)
{
    static kal_uint8 vib_count = 0;
    int go_next = (int) go_on;
    vib_setting_struct *vib_setting = track_cust_vib_interval(0, 0, 0);

    LOGD(L_APP, L_V7, "go_on:%d, vib_count:%d", go_on, vib_count);

    if(go_next == 0 || vib_count > vib_setting->overtimer)
    {
        vib_count = 0;
        track_stop_timer(TRACK_CUST_RING_VIB_TIMER_ID);
        track_drv_set_vibr(KAL_FALSE);
    }
    else if(go_next == 1)
    {
        ++vib_count;
        track_drv_set_vibr(KAL_TRUE);

        track_start_timer(TRACK_CUST_RING_VIB_TIMER_ID, vib_setting->keep_on, track_cust_ring_Vibration, (void*)2);
    }
    else if(go_next == 2 && vib_setting->keep_off)
    {
        track_drv_set_vibr(KAL_FALSE);
        track_start_timer(TRACK_CUST_RING_VIB_TIMER_ID, vib_setting->keep_off, track_cust_ring_Vibration, (void*)1);
    }
}

/******************************************************************************
 *  Function    -  track_cust_sos_or_center_check
 *
 *  Purpose     -  ���������SOS��������ĺ���Ƚ�
 *
 *  Parameter   -
 *      type��
 *            1�������ĺ���ȶ�
 *            2����SOS����ȶ�
 *            3�������ĺ��롢SOS�ȶ�
 *            4: ��FN����ԱȽ�
 *            5: ��WN����ԱȽ�
 *            6: ��SOS FN WN����Ա�
 *      num���������
 *
 *  Return      -
 *      0xff���������Ϊ��

 *      1�����ĺ��������ã�����������벻���
 *      4�����ĺ���δ����
 *      7�������ĺ������

 *      2��SOS�����ã�����������벻���
 *      5��SOSδ����
 *      8����SOS������� 8 16 32 40

 *      3��SOS�����ĺ��������ã�����������붼�����
 *      6��SOS�����ĺ��붼δ����
 *      9����SOS��������ĺ���������
 *      10: ��FN����������
 *      11: ��WN����������
 *      12: ��SOS FN WN����������
 *      13: FN ���������ã�����������벻���
 *      14: WN���������ã�����������벻���
 *      15: SOS FN WN����Ϊ��
 *      17: SOS FN WN���������ã�����������벻���
 *  modification history
 * ----------------------------------------
 * v1.0  , 20-11-2012,  Cml  written
 * v1.1  , 12-12M-2013,  Wangqi  modify
 * ----------------------------------------
*******************************************************************************/
kal_uint8 track_cust_sos_or_center_check(kal_int8 type, char *number)
{
    int i;
    kal_uint8 ret, flag, fn = 0, wn = 0;
    kal_uint32 num, num2;

    if(strlen(number) == 0)
    {
        return 0xff;
    }

    num = track_fun_convert_phone_number_to_int(number);
    LOGD(L_APP, L_V6, "type=%d, num=%d", type, num);
    if(type == 1 || type == 3)
    {
        if(strlen(G_parameter.centerNumber) > 0)
        {
            num2 = track_fun_convert_phone_number_to_int(G_parameter.centerNumber);
            LOGD(L_APP, L_V6, "num2=%d", num2);
            if(num == num2)
            {
                ret = 7;
            }
            else
            {
                ret = 1;
            }
        }
        else
        {
            ret = 4;
        }
        if(type == 1)
        {
            return ret;
        }
    }

    if(type == 2 || type == 3 || type == 6)//SOS
    {
        flag = 5;
        for(i = 0; i < TRACK_DEFINE_SOS_COUNT; i++)
        {
            if(strlen(G_parameter.sos_num[i]) > 0)
            {
                num2 = track_fun_convert_phone_number_to_int(G_parameter.sos_num[i]);
                LOGD(L_APP, L_V6, "sos%d=%d", i, num2);
                if(num == num2)
                {
                    flag = 8 * (i + 1);
                    break;
                }
                else
                {
                    flag = 2;
                }
            }
        }
        if(type == 2)
        {
            return flag;
        }
    }

    if(type == 4 || type == 6)//FN
    {
        for(i = 0; i < TRACK_DEFINE_FN_COUNT; i++)
        {
            if(strlen(G_phone_number.fn_num[i]) > 0)
            {
                num2 = track_fun_convert_phone_number_to_int(G_phone_number.fn_num[i]);
                LOGD(L_APP, L_V6, "fn num%d=%d", i, num2);
                if(num == num2)
                {
                    fn = 10;
                    break;
                }
                else
                {
                    fn = 13;
                }
            }
        }
        if(type == 4)
        {
            return fn;
        }
    }

    if(type == 5 || type == 6)//WN
    {
        for(i = 0; i < TRACK_DEFINE_WN_COUNT; i++)
        {
            if(strlen(G_phone_number.white_num[i]) > 0)
            {
                num2 = track_fun_convert_phone_number_to_int(G_phone_number.white_num[i]);
                LOGD(L_APP, L_V6, "wn num%d=%d", i, num2);
                if(num == num2)
                {
                    wn = 11;
                    break;
                }
                else
                {
                    wn = 14;
                }
            }
        }
        if(type == 5)
        {
            return wn;
        }
    }

    if(type == 3)
    {
        if(ret == 7 || flag % 8 == 0)
        {
            return 9;
        }
        if(ret == 4 && flag == 5)
        {
            return 6;
        }
        return 3;
    }
    else if(type == 6)
    {
        if(fn == 10 || wn == 11 || flag % 8 == 0)
        {
            ret = 12;//������һ�����
        }
        else if(flag == 2 || fn == 13 || wn == 14)
        {
            ret = 17;//�����費���
        }
        else
        {
            ret = 15;//�����FN SOS WN
        }
    }
    return ret;
}

/******************************************************************************
 *  Function    -  track_cust_make_call
 *
 *  Purpose     -  ѭ�����н�������
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
    kal_uint8 track_cust_make_call(void* arg)
{
    static kal_uint8 handUp = 0;
    static kal_uint8 flag = 0;
    kal_uint32 par = (kal_uint32)arg;
    LOGD(L_APP, L_V5, "%d, handUp=%d, flag=%d", par, handUp, flag);
    if(par == 99)
    {
        return flag;
    }
    else if(par == 0)
    {
        flag = 0;
        LOGD(L_APP, L_V4, "!!! ѭ���������̽��� !!!");
        track_cust_module_alarm_lock(99, 0);
    }
    else if(par == 1)
    {
        track_cust_dial_makecall((void *)0);//�ر�����ѭ������绰����
        if(flag != 0)
        {
            LOGD(L_APP, L_V4, "��ǰ�������ڽ��е�ѭ�����У�����������ԡ�");
            return flag;
        }

#if defined(__LUYING__)
                if(track_cust_is_recording(0xff))
                {
                    track_cust_close_record();
                }
#endif
        
        flag = 1;
        handUp = 0;
        track_os_intoTaskMsgQueueExt(track_cust_make_call_function, (void*)1);
    }
    else if(flag == 1 && par == 21)   // ��ͨ�绰
    {
        LOGD(L_APP, L_V4, "!!! ��ͨ�绰 !!!");
        handUp = 1;
        tr_stop_timer(TRACK_CUST_SCROLL_MAKECALL_TIMER);
    }
    else if(flag == 1 && par == 22)   // �Ҷϵ绰
    {
        if(handUp)
        {
            handUp = 0;
            flag = 0;
            LOGD(L_APP, L_V4, "!!! ѭ���������̽��� !!!defined");
            track_cust_module_alarm_lock(99, 0);
        }
        else
        {
            track_start_timer(TRACK_CUST_SCROLL_MAKECALL_TIMER, 20000, track_cust_make_call_function, NULL);
        }
    }
    else if(par == 23) //sos�����������һ���ͣ��sos��������
    {
        tr_stop_timer(TRACK_CUST_SCROLL_MAKECALL_TIMER);
        handUp = 0;
        flag = 0;
        LOGD(L_APP, L_V4, "!!! �һ�ѭ���������̽��� !!!defined");
        track_cust_module_alarm_lock(99, 0);
    }
    return flag;
}

/******************************************************************************
 *  Function    -  track_cust_soscall_OverTime_call
 *
 *  Purpose     -  ѭ�����г�ʱ������״̬
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void track_cust_soscall_OverTime_call()
{
    track_cust_sos_call_status(CALL_MO_OUT, CALL_MO_NORSP);
}

/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/

/******************************************************************************
 *  Function    -  track_cust_make_call_function
 *
 *  Purpose     -  ѭ�����д���
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_make_call_function(void *arg)
{
    static kal_uint8 call_ind = 0;                  //��������ÿ���趨�������
    static kal_uint8 ScrollDialNumber = 0;    //����������ѭ����������
    kal_uint32 par = (kal_uint32)arg;
    kal_bool sms_send_status = track_sms_get_send_status();
    kal_int8 is_sos_alarm = track_cust_module_alarm_lock(TR_CUST_ALARM_SOS, 2);

    LOGD(L_APP, L_V5, "par=%d, ScrollDialNumber=%d,call_ind=%d,sms:%d, sos_alarm:%d",
         par, ScrollDialNumber, call_ind, sms_send_status, is_sos_alarm);

    if(par == 1)
    {
        call_ind = 0;
        ScrollDialNumber = 0;
        LOGD(L_APP, L_V4, "!!! ѭ���������̿�ʼ !!!");
    }
    track_os_intoTaskMsgQueue(track_soc_disconnect); // ͨ�����Է�������æ��GPRSͨѶ��Ӱ��绰�ĳɹ����С�
    if(sms_send_status != 0)
    {
        track_start_timer(TRACK_CUST_SCROLL_MAKECALL_TIMER, 1000, track_cust_make_call_function, NULL);
    }
    else if(track_is_in_call())
    {
        track_cust_key_dail_way(2);
        track_cust_call_hangup();
        track_start_timer(TRACK_CUST_SCROLL_MAKECALL_TIMER, 1000, track_cust_make_call_function, NULL);
    }
    else
    {
        if(call_ind == TRACK_DEFINE_SOS_COUNT)
        {
            call_ind = 0;
            ScrollDialNumber++;
        }
        if(ScrollDialNumber < G_parameter.redial_count)
        {
            if((G_phone_number.almrep[call_ind] || is_sos_alarm == 1) && strlen(G_parameter.sos_num[call_ind]))
            {
                g_call_status = 4;
#if defined(__GT310__)
                LED_TWO_COLOR_StatusSet(TWO_GSM_IN_CALL_STATE);
#elif  defined (__GT370__) || defined (__GT380__)
                track_cust_simple_gsm_led_work(GSM_IN_CALL_STATE);
#else
                LED_GSM_StatusSet(GSM_IN_CALL_STATE);
#endif

#if defined(__AUDIO_RC__)
                track_cust_monitor_set_speaket(1);
#endif /* __AUDIO_RC__ */
                track_cust_key_dail_way(0x0);
                track_cust_call(G_parameter.sos_num[call_ind]);
                tr_start_timer(TRACK_CUST_SCROLL_MAKECALL_OVERTIME_TIMER, 120000, track_cust_soscall_OverTime_call);
                if(0 == G_parameter.Permission.AlarmNumCount)
                {
                    ScrollDialNumber++;
                }
                else
                {
                    call_ind++;
                }

            }
            else
            {
                call_ind++;
                track_cust_make_call_function(NULL);
            }
        }
        else
        {
            if(track_cust_make_call((void*)99))
            {
                track_cust_make_call((void*)0);
            }
        }
    }
}
void track_cust_sos_gps_dalay(void)
{
    track_cust_module_alarm_lock(TR_CUST_ALARM_SOS, 0);
    //track_cust_module_alarm_set_type(0);
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_SOS);
}
/******************************************************************************
 *  Function    -  track_cust_soscall_OverTime_sendsms
 *
 *  Purpose     -  ����SOS��������
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_soscall_OverTime_sendsms(void)
{
    track_gps_data_struct *gpsLastPoint = NULL;
    if(G_parameter.sos_alarm.sw)
    {

        if(G_parameter.sos_alarm.alarm_type > 0 && G_parameter.sos_alarm.alarm_type < 3)
        {
            memset(&g_msg, 0, sizeof(struct_msg_send));
#if defined (__GT370__)|| defined (__GT380__) ||defined(__GT500__) ||defined(__GT03F__) ||defined(__ET200__) || defined(__GW100__)|| defined(__V20__)\
|| defined(__ET310__)||defined(__GT300S__) || defined(__MT200__)||defined (__ET320__)
            LOGD(L_APP, L_V6, "g_alarm_gps_status = %d", g_alarm_gps_status);
            if(g_alarm_gps_status == 1 || track_cust_gps_status() > 2)
            {
                gpsLastPoint = track_cust_backup_gps_data(0, NULL);
                memcpy(&gps_data_buf, gpsLastPoint, sizeof(track_gps_data_struct));
                g_alarm_gps_status = 0;
            }
            LOGD(L_APP, L_V6, "gprmc.status = %d", gps_data_buf.gprmc.status);
            if(gpsLastPoint != NULL && gpsLastPoint->gprmc.status == 1) //gps_data_buf.gprmc.status == 1
            {
                g_msg.addUrl = KAL_TRUE;
            }
            else
            {
                g_msg.addUrl = KAL_FALSE;
            }
#else
            g_msg.addUrl = KAL_TRUE;
#endif /* __GT300__ */
            g_msg.cm_type = CM_SMS;

            g_msg.not_need_almrep = 1;
            if(g_msg.addUrl == KAL_TRUE)
            {
#if defined(__SPANISH__)
                track_fun_strncpy(g_msg.msg_en, G_parameter.sos_alarm.alarm_sms_head, CM_PARAM_LONG_LEN_MAX);
#else
                track_fun_strncpy(g_msg.msg_en, msg_alarm_sos_en, CM_PARAM_LONG_LEN_MAX);
#endif
                g_msg.msg_cn_len = 10;                
                memcpy(g_msg.msg_cn, msg_alarm_sos_cn, g_msg.msg_cn_len);
            }
            else
            {

                track_fun_strncpy(g_msg.msg_en, msg_alarm_sos_en1, CM_PARAM_LONG_LEN_MAX);
                g_msg.msg_cn_len = 18;
                memcpy(g_msg.msg_cn, msg_alarm_sos_cn1, g_msg.msg_cn_len);
            }
            g_msg.is_sos_alarm_type = TRUE;
            track_cust_module_alarm_msg_send(&g_msg, &gps_data_buf);
        }
        else
        {
            LOGD(L_APP, L_V4, "SOS�������������������������֪ͨ��");
        }
    }

#if !defined (__GW100__)
    if(G_parameter.sos_alarm.alarm_type == 2 || G_parameter.sos_alarm.alarm_type == 3)
    {
        track_start_timer(TRACK_CUST_SOS_MAKECALL_TIMER, 60000, track_cust_make_call, (void*)1);
    }
    else
#endif /* __GW100__ */
    {
        track_cust_module_alarm_lock(TR_CUST_ALARM_SOS, 0);
    }
    //track_cust_module_alarm_set_type(0);
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_SOS);
}

/******************************************************************************
 *  Function    -  track_cust_module_sos_trigger_button
 *
 *  Purpose     -  SOS����
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 25-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_sos_trigger_button(void)
{
    track_gps_data_struct *gpsLastPoint = NULL;
    U16 sn;
    kal_uint8 call_state = track_get_call_state();

    LOGD(L_APP, L_V5, "%d",G_parameter.sos_alarm.sw);
    
#if defined( __GT03F__)||defined(__GT300__) || defined(__GW100__)||defined(__GT300S__)
    if(G_parameter.sos_alarm.sw)
    {
        ControlVibration();
    }
#elif defined(__GT310__)
    if(track_cust_get_work_mode()==WORK_MODE_3)
    {
        track_cust_fly_mode_change((void*)78);
    }
#elif defined (__HVT001__)
    if((track_is_autotestmode() == KAL_TRUE) || (track_is_testmode() == KAL_TRUE))
    {
        LOGS("SOS KEY");
    }
#endif

    if(track_cust_status_sim() != 1 || !G_parameter.sos_alarm.sw)
    {
        return;
    }

#if defined( __OLED__)
    track_cust_lcd_sos();
#endif /* __OLED__ */

    LOGD(L_APP, L_V4, "!!! SOS ���� !!! call_state:%d", call_state);
    if(track_cust_module_alarm_lock(TR_CUST_ALARM_SOS, 2) != 0)
    {
        LOGD(L_APP, L_V4, "��һSOS����δ���!");
        return;
    }
#if defined( __GT03F__) || defined(__GT310__) || defined(__GW100__)||defined(__GT300S__)
    if(TR_CALL_SETUP <= call_state && call_state <= TR_CALL_DISCONNECT)
    {
        track_cust_key_dail_way(2);
        track_cust_call_hangup();
    }
#endif
#if defined( __GT06D__)||defined(__NT36__)||defined(__GT310__)
    if(g_monitor_call_status == 2)
    {
        LOGD(L_APP, L_V4, "��ǰ���ڼ����У�SOS��������ǿ�ƹҶϼ���������SOS��������.");
        track_cust_call_hangup();
    }
#endif /* __GT03F__ */
    track_cust_module_alarm_lock(TR_CUST_ALARM_SOS, 1);
    LOGD(L_APP, L_V5, "gprson:%d, alarm_type:%d", track_nvram_alone_parameter_read()->gprson, G_parameter.sos_alarm.alarm_type);
    gpsLastPoint = track_cust_gpsdata_alarm();
    memcpy(&gps_data_buf, gpsLastPoint, sizeof(track_gps_data_struct));
    if(g_GT03D == KAL_TRUE)
    {
        g_alarm_gps_status = 0;
        track_cust_gprson_status(1);
        track_soc_gprs_reconnect((void*)307);

        if(track_cust_gps_status() > 2)
        {
            g_alarm_gps_status = 1;
        }
        else
        {
            track_cust_gps_work_req((void *)10);
            if(G_parameter.lang == 1 || track_cust_gprson_status(99) == 1)
            {
                track_cust_send_paket_16(2, gpsLastPoint, 0);
            }
            /*��ʱ����ͳһ����*/
        }
#if defined (__GW100__)
        //GW100 ǿ��Ҫ��GPS��SOS״̬�¹���5����,����������Ŀ��δ��ȷ˵��
        //�ʴ˴�ר��ΪGW100ǿ�Ƶ���һ��
        track_cust_gps_work_req((void *)10);
#endif /* __GW100__ */
        if(track_cust_gprson_status(99) == 1)
        {
            track_cust_alarm_type(TR_CUST_ALARM_SOS);
            if(g_alarm_gps_status == 1)
            {
                sn = track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_SOS, KAL_TRUE, 1);
                g_last_alerm_no = sn;
                tr_start_timer(TRACK_CUST_SOS_SMS_OVERTIME_TIMER, 60 * 1000, track_cust_soscall_OverTime_sendsms);
            }
            else
            {
                sn = track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_SOS, KAL_TRUE, 1);
                g_last_alerm_no = sn;
#if defined(__GW100__)
                tr_start_timer(TRACK_CUST_SOS_GPS_DELAY_TIMER_ID, 4 * 60 * 1000 + 40000, track_cust_sos_gps_dalay);
                tr_start_timer(TRACK_CUST_SOS_SMS_OVERTIME_TIMER, 5 * 60 * 1000, track_cust_soscall_OverTime_sendsms);
#else
                tr_start_timer(TRACK_CUST_SOS_SMS_OVERTIME_TIMER, 5 * 60 * 1000, track_cust_soscall_OverTime_sendsms);
#endif
            }
        }
#if defined (__GW100__)
        if(G_parameter.sos_alarm.alarm_type > 2)
        {
            tr_stop_timer(TRACK_CUST_SOS_GPS_DELAY_TIMER_ID);
            tr_stop_timer(TRACK_CUST_SOS_SMS_OVERTIME_TIMER);
            track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_SOS);
        }

        if(G_parameter.sos_alarm.alarm_type >= 2)
        {
            LOGD(L_APP, L_V5, "track_cust_record 1,1,10");
            track_cust_record(1, 1, 10);//wangqi
        }
#else
        if(G_parameter.sos_alarm.alarm_type == 3)
        {
            tr_stop_timer(TRACK_CUST_SOS_GPS_DELAY_TIMER_ID);
            tr_stop_timer(TRACK_CUST_SOS_SMS_OVERTIME_TIMER);
            track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_SOS);

            track_start_timer(TRACK_CUST_SOS_MAKECALL_TIMER, 10 * 1000, track_cust_make_call, (void*)1);

        }
#endif /* __GW100__ */
        else if(G_parameter.sos_alarm.alarm_type > 0)
        {
#if defined (__GT300__) && defined(__LANG_ENGLISH__)
            if(track_cust_gprson_status(99) == 0)
#else
            if(G_parameter.lang == 0 || track_cust_gprson_status(99) == 0)
#endif /* __GT300__ */
            {
                tr_stop_timer(TRACK_CUST_SOS_GPS_DELAY_TIMER_ID);
                tr_stop_timer(TRACK_CUST_SOS_SMS_OVERTIME_TIMER);
                track_os_intoTaskMsgQueue(track_cust_soscall_OverTime_sendsms);
            }
        }
        else
        {
            track_cust_module_alarm_lock(TR_CUST_ALARM_SOS, 0);
        }

    }
    else
    {
        if(track_nvram_alone_parameter_read()->gprson)
        {
            sn = track_cust_paket_16(gpsLastPoint, track_drv_get_lbs_data(), TR_CUST_ALARM_SOS, KAL_TRUE, 1);
            g_last_alerm_no = sn;
        }
        if(G_parameter.sos_alarm.alarm_type == 3)
        {
            track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_SOS);
            track_cust_make_call((void*)1);
        }
        else if(G_parameter.sos_alarm.alarm_type > 0)
        {
#if defined (__GT300__) && defined(__LANG_ENGLISH__)
            if(track_nvram_alone_parameter_read()->gprson == 0)
#else
            if(G_parameter.lang == 0 || track_nvram_alone_parameter_read()->gprson == 0)
#endif /* __GT300__ */
            {
                track_os_intoTaskMsgQueue(track_cust_soscall_OverTime_sendsms);
            }
            else
            {
                tr_start_timer(TRACK_CUST_SOS_SMS_OVERTIME_TIMER, TRACK_ALARM_DELAY_TIME_SECONDS, track_cust_soscall_OverTime_sendsms);
            }
        }
        else
        {
            track_cust_module_alarm_lock(TR_CUST_ALARM_SOS, 0);
        }
    }
}

void track_call_sos_incall(void)
{
#if defined( __GT310__)
    if(!track_cust_is_working_mode4() && !track_cust_is_mute_mode())
    {
        track_cust_audio_play_voice(MT_RING);
    }
    LED_TWO_COLOR_StatusSet(TWO_GSM_IN_CALL_STATE);
#if defined (__GT300__)||defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
    if(track_cust_get_work_mode() == WORK_MODE_4)
    {
        track_cust_call_accept();
    }
#endif /* __GT300__ */
#elif defined( __NT31__) 

#else
    track_cust_call_accept();
#endif /* __GT300__ */
    g_monitor_call_status = 1;
}
#if defined(__XCWS__)
kal_bool  is_host_number(char* number)
{
    kal_uint32 host_no = 0, sms_no = 0;
    host_no = track_fun_convert_phone_number_to_int((char*)track_cust_get_host_num());
    sms_no = track_fun_convert_phone_number_to_int(number);

    LOGD(L_CMD, L_V5, "%d,%d,%s,%s", host_no, sms_no, track_cust_get_host_num(), number);

    if(host_no != sms_no)
    {
        LOGD(L_CMD, L_V5, "�������벻ƥ��");
        return FALSE;
    }
    else
    {
        LOGD(L_CMD, L_V5, "��������ƥ��");
        return TRUE;
    }
}
#endif
/******************************************************************************
 *  Function    -  track_cust_sos_call_status
 *
 *  Purpose     -  ����״̬���
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * v1.1  , 29-10-2013,  Wangqi  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_sos_call_status(kal_uint16 call_type, kal_uint16 callState)
{
    static kal_bool is_incall = FALSE;
    static kal_bool flag = KAL_FALSE;
    CMD_DATA_STRUCT  Cmds = {0};
    static kal_bool is_host = KAL_FALSE;
    static kal_uint32 s_tick = 0, e_tick = 0;
    CMD_DATA_STRUCT cmd = {0};
    kal_int8 len = 0;

    LOGD(L_APP, L_V5, "call_type=%d,callState=%d, g_call_status=%d", call_type, callState, g_call_status);
    if(call_type == CALL_FREE)
    {
        return;
    }
    switch(callState)
    {
        case CALL_MO_SMS_BUSY:  /* ���ڷ��Ͷ��� */
        case TR_CALL_MT_BUSY:
        case CALL_MO_BUSY:
            break;

        case TR_CALL_SETUP:  /* ���� */
            g_call_status = 1;
            break;

        case TR_CALL_RING: //����/
            g_call_status = 2;
            s_tick = kal_get_systicks();
            LOGD(L_APP, L_V5, "���� s_tick=%d, %d", s_tick, track_cust_get_work_mode());

#if defined(__GT370__)|| defined (__GT380__)
			if(track_cust_get_work_mode() == WORK_MODE_3)
			{
				break;
			}
#endif
	        flag = KAL_TRUE;

            if(flag)
            {
                kal_bool permit = KAL_FALSE;
                kal_uint8 snw_number = 0, sos_number = 0,center_number =0;
                char *num = track_cust_call_get_incomming_number();
#if defined (__NETWORK_LICENSE__) && !defined(__GT530__)
                track_drv_network_license_auto_answer_action();
                //ֱ��ȫ���Զ�����
                break;
#endif /* __NETWORK_LICENSE__ */
#if defined(__XCWS__)
                if(is_host_number(num))
                {
                    is_host = TRUE;
                }
#endif


#if defined(__LUYING__)
/*¼��ʱ����ͨ��       --    chengjun  2017-10-09*/
                if(track_cust_is_recording(0xff))
                {
                    track_cust_close_record();
                }
#endif

#if defined( __GT300__)  ||defined(__GT03F__) || defined(__GW100__)||defined(__GT300S__)
                track_drv_sleep_enable(SLEEP_CALL_MOD, FALSE);
                snw_number = track_cust_sos_or_center_check(6, track_cust_call_get_incomming_number());
                sos_number = track_cust_sos_or_center_check(2, track_cust_call_get_incomming_number());
                 center_number = track_cust_sos_or_center_check(1, track_cust_call_get_incomming_number());

                LOGD(L_APP, L_V5, "snw_number = %d, sos:%d, num:%s", snw_number, sos_number, num);
                if(snw_number == 17&&(G_parameter.Permission.MonPermit != 3&&center_number!=7))
                {
                    track_cust_call_hangup();
                    break;
                }
                else if(sos_number % 8 != 0)
                {
#ifndef __GT300S__
                    if(!track_cust_is_working_mode4() && !track_cust_is_mute_mode())
                    {
                        track_cust_audio_play_voice(MT_RING);
                    }
#endif /* __GT300S__ */

					LED_GSM_StatusSet(GSM_IN_CALL_STATE);
                }
                else
                {
                    LOGD(L_APP, L_V5, "�������%d����:num:%s", sos_number + 1, num);
                }
#elif defined(__SPANISH__)
                sos_number = track_cust_sos_or_center_check(2, track_cust_call_get_incomming_number());
                if(sos_number % 8 != 0)
                {
                    track_cust_call_hangup();
                    break;
                }
                else
                {
                    LED_GSM_StatusSet(GSM_IN_CALL_STATE);
                }
#elif defined(__GT310__)
                track_drv_sleep_enable(SLEEP_CALL_MOD, FALSE);
				track_cust_led_sleep(1);
                if(G_parameter.jc_mode == 0)
                {
                    snw_number = track_cust_sos_or_center_check(6, track_cust_call_get_incomming_number());
                }
                else
                {
                    snw_number = track_cust_sos_or_center_check(2, track_cust_call_get_incomming_number());
                }
                sos_number = track_cust_sos_or_center_check(2, track_cust_call_get_incomming_number());
                LOGD(L_APP, L_V5, "snw_number = %d, sos:%d, num:%s", snw_number, sos_number, num);
                LOGD(L_APP, L_V5, "MonPermit = %d", G_parameter.Permission.MonPermit);
#if defined(__LUYING__)
                if(track_cust_is_recording(0xff))
                {
                    track_cust_close_record();
                }
#endif//(__LUYING__)
				LED_TWO_COLOR_StatusSet(TWO_GSM_IN_CALL_STATE);
                if(G_parameter.Permission.MonPermit == 3 && G_parameter.jc_mode == 1)
                {
                    LOGD(L_APP, L_V5, "���������Լ���. MonPermit = %d", G_parameter.Permission.MonPermit);
                }
                else if(G_parameter.jc_mode == 0 && G_parameter.Permission.CallPermit == 3)
                {
                    LOGD(L_APP, L_V5, "���������Խ���. MonPermit = %d", G_parameter.Permission.CallPermit);
                                        
                }
                else if(snw_number == 17  || ((snw_number == 2 || snw_number == 5 || G_parameter.Permission.MonPermit == 0 || len < 1) && G_parameter.jc_mode == 1)\
                        || (G_parameter.jc_mode == 0 && G_parameter.Permission.CallPermit == 2 && len < 1))
                {
                    track_cust_call_hangup();
                    break;
                }
                else if(sos_number % 8 != 0 || (G_parameter.jc_mode == 0 && G_parameter.answer == 0))
                {
                                        
                }
                else
                {
                    LOGD(L_APP, L_V5, "�������%d����:num:%s", sos_number + 1, num);
                }
				track_cust_audio_play_voice(MT_RING);


#elif  defined (__GT370__) || defined (__GT380__)
                track_cust_simple_gsm_led_work(GSM_IN_CALL_STATE);
#else
                LED_GSM_StatusSet(GSM_IN_CALL_STATE);

#if defined (__NO_MIC__)
                return;
#endif /* __NO_MIC__ */
#endif /* __GT300__ */
                LOGD(L_APP, L_V5, "flag=%d, MonPermit=%d, num:%s", flag, G_parameter.Permission.MonPermit, num);
                flag = KAL_FALSE;


                if(G_parameter.Permission.MonPermit == 3)
                {
                    permit = KAL_TRUE;
                }
                else
                {
                    kal_uint8 cen;
                    if(strlen(num) > 0)
                    {
                        if(G_parameter.Permission.MonPermit == 1)
                        {
                            cen = track_cust_sos_or_center_check(1, num);
                            if(cen == 7)
                            {
                                permit = KAL_TRUE;
                            }
                        }
                        else if(G_parameter.Permission.MonPermit == 2)
                        {
                            cen = track_cust_sos_or_center_check(3, num);
                            if(cen == 9)
                            {
                                permit = KAL_TRUE;
                            }
                        }
                        LOGD(L_APP, L_V5, "TR_CALL_SETUP, Call authentication %s,%d", num, cen);
                        /*if(cen <= 3)     // SOS��������ĺ��벻Ϊ���Ҳ�����Զ��Ҷ�
                        {
                            track_cust_call_hangup();
                        }*/
                    }
                }
#if defined (__GT310__)
                if(permit == KAL_TRUE && G_parameter.jc_mode == 1)
#else
                if(permit)
#endif
                {
                    /*Ӧ����20140712���ʼ�Ҫ��ͨ��ʱ�л�������*/
                    track_drv_sys_l4cuem_set_audio_mode(track_status_audio_mode(0xFF));
                    // �������ܣ�SOS��������Զ�����//��300ms�����RING�Ϸ�ʱ��ͻRING����һ��
                    LOGD(L_APP, L_V5, "TR_CALL_RING sos index:%s: %ds���Զ�����", num, G_parameter.monitor_delay);
                    tr_start_timer(TRACK_CUST_SOS_IN_CALL_TIMER, G_parameter.monitor_delay * 1000 + 300, track_call_sos_incall);
                }

            }
            break;

        case TR_CALL_CONNECT: /* ��ͨ */

            track_stop_timer(TRACK_CUST_SCROLL_MAKECALL_OVERTIME_TIMER);
            g_call_status = 3;
            if(g_monitor_call_status == 1) g_monitor_call_status = 2;
            flag = KAL_FALSE;
            track_cust_make_call((void*)21);
            track_cust_dial_makecall((void*)21);
            if(track_is_timer_run(TRACK_CUST_SOS_IN_CALL_TIMER) == TRUE)
            {
                track_stop_timer(TRACK_CUST_SOS_IN_CALL_TIMER);
            }
#if defined( __GT310__) || defined(__GW100__) 
            {
                kal_uint8 issos = track_cust_sos_or_center_check(2, track_cust_call_get_incomming_number());
                kal_uint8 isFN_call = track_cust_key_dail_way(0xFF);
                track_stop_timer(TRACK_CUST_SOS_IN_CALL_TIMER);
                LOGD(L_APP, L_V5, "is sos:%d,calltype:%d,is fn call:%d", issos, call_type, isFN_call);
#if defined (__GT300__)||defined(__GT300S__)
                if(((issos % 8 == 0) && call_type == CALL_MO_OUT && isFN_call == 0) || track_cust_is_working_mode4())
#else
                if((issos % 8 == 0) && call_type == CALL_MO_OUT && isFN_call == 0)
#endif /* __GT300__ */
                {
                    track_cust_audio_play_voice(MT_MONITOR);
                }
                else
                {
                    track_cust_audio_play_voice(MT_CONNECT);
                    LED_GSM_StatusSet(GSM_IN_CALL_STATE);
                }
#if defined( __GT310__)
					LED_TWO_COLOR_StatusSet(TWO_GSM_IN_CALL_STATE);
#endif
            }
#elif defined( __GT03F__)
            if(track_get_incall_type() == CALL_MO_OUT)
            {
                LED_GSM_StatusSet(GSM_IN_CALL_STATE);
            }
#elif (defined(__GT06F__) || defined(__NT36__)||defined(__NT37__)) && defined(__AUDIO_RC__)
            if(call_type == CALL_MO_OUT)
            {
                track_cust_monitor_set_speaket(1);
            }
            else
            {
                track_cust_monitor_set_speaket(0);
            }
#elif  defined (__GT370__) || defined (__GT380__)
            track_cust_simple_gsm_led_work(GSM_IN_CALL_STATE);
#else
            LED_GSM_StatusSet(GSM_IN_CALL_STATE);
#endif /* __GT300__ */
            break;

        case CALL_MO_ATUNRSP:
        case TR_CALL_NULL_NO: /* Ϊ�� */
        case TR_CALL_ERROR:
        case CALL_MO_SIMERROR:
        case CALL_MO_NUMBERERROR:
        case CALL_MO_UNKNOWERROR:
        case CALL_MO_NORSP:
        case CALL_GETNUMBERERROR:
            //track_cust_call_hangup();
        case TR_CALL_DISCONNECT:
            {
#if defined(__XCWS__)
                track_stop_timer(TRACK_CUST_SCROLL_MAKECALL_OVERTIME_TIMER);
                flag = KAL_FALSE;
                g_call_status = 0;
                LED_GSM_StatusSet(GSM_NORMAL_STATE);
                track_stop_timer(TRACK_CUST_SOS_IN_CALL_TIMER);
                track_cust_make_call((void*)22);
                LOGD(L_APP, L_V5, "flag=%d, MonPermit=%d", flag, G_parameter.Permission.MonPermit);
                e_tick = kal_get_systicks();
                LOGD(L_APP, L_V5, "�Ҷ� e_tick=%d��", e_tick);
                if(is_host)
                {
                    is_host = FALSE;
                    if(G_parameter.t_calllock == 1 && s_tick != 0 && e_tick != 0)
                    {
                        if((kal_ticks_to_secs((e_tick - s_tick)) < 7) && track_cust_get_defense_mode() != 1)
                        {
                            //����
                            track_cust_defense_mode_ind(d_cmd_disarm_msg);
                        }
                        else if(kal_ticks_to_secs(e_tick - s_tick) >= 7)
                        {
                            //���
                            track_cust_defense_mode_ind(d_cmd_fortifation_msg);
                        }
                    }
                    else if(G_parameter.t_calllock == 0)
                    {
                        track_cust_gps_work_req((void *)8);
                    }
                }
                s_tick = 0;
                e_tick = 0;
#else
                kal_uint8 sos_hung_up = track_cust_key_dail_way(0xFF);
                LOGD(L_APP, L_V5, "tick %d,%d,%d,%d", s_tick, G_parameter.call_get_addr_sw, kal_get_systicks() - s_tick, 10 * KAL_TICKS_1_SEC);
                if(s_tick != 0 && G_parameter.call_get_addr_sw && kal_get_systicks() - s_tick < 10 * KAL_TICKS_1_SEC)
                {
                    static CMD_DATA_STRUCT cmd;
                    s_tick = 0;
                    memset(&cmd, 0, sizeof(CMD_DATA_STRUCT));
                    cmd.pars[0] = "DW";
                    cmd.return_sms.cm_type = CM_SMS;
                    snprintf(cmd.return_sms.ph_num, 32, "%s", track_cust_call_get_incomming_number());
                    #if !defined(__GT420D__)
                    track_cust_module_get_addr(&cmd);
                    #endif
                }
    #if defined( __GT300__) || defined(__GW100__)
                track_cust_audio_play_voice(MT_DISCONNECT);
                track_cust_key_dail_way(0);
                track_drv_sleep_enable(SLEEP_CALL_MOD, TRUE);
    #else
                track_cust_key_dail_way(0);
    #endif /* __GT300__ */
                track_stop_timer(TRACK_CUST_SCROLL_MAKECALL_OVERTIME_TIMER);
                flag = KAL_FALSE;
                g_call_status = 0;
#if defined(__GT310__)
            led_level_state_switch();
#elif  defined (__GT370__) || defined (__GT380__)
            //track_cust_simple_gsm_led_work(GPRS_CONNECT_STATE);
            track_cust_simple_gsm_led_sleep();
#else
            LED_GSM_StatusSet(GSM_NORMAL_STATE);
#endif


    #if defined(__GT03F__)||defined(__GT310__) || defined(__GW100__)||defined(__GT300S__)
                if(track_cust_led_sleep(99) == 0 || G_realtime_data.key_status == 0 || track_cust_led_sleep(99) == 2)
                {
                    track_cust_led_sleep(2);
                }
                else
    #endif
                    if(track_cust_status_defences() == 2)
                    {
                        track_stop_timer(TRACK_CUST_MODULE_ALARM_VIBRATES_TIMER_ID);
                        track_cust_led_defences(2);
                    }
                //track_stop_timer(TRACK_CUST_INCOMING_CALL_PLAY_TONE_TIMER);
                track_cust_make_call((void*)22);
                track_cust_dial_makecall((void*)22);

#if defined(__NT31__)||defined(__NT36__)
                /*            if(track_is_timer_run(TRACK_CUST_SOS_IN_CALL_TIMER) == TRUE)
                            {
                                    track_stop_timer(TRACK_CUST_SOS_IN_CALL_TIMER);
                                    cmd.return_sms.cm_type = CM_SMS;
                                    memcpy(&cmd.return_sms.ph_num, track_cust_call_get_incomming_number(), strlen(track_cust_call_get_incomming_number()));
                                    track_cust_gt03_dw(&cmd);
                            }
                            */
#else
                if((track_is_timer_run(TRACK_CUST_SOS_IN_CALL_TIMER) == TRUE) && g_GT03D == KAL_TRUE && sos_hung_up != 2)
                {
                    track_stop_timer(TRACK_CUST_SOS_IN_CALL_TIMER);
                    memcpy(Cmds.return_sms.ph_num, track_cust_call_get_incomming_number(), strlen(track_cust_call_get_incomming_number()));
                    Cmds.return_sms.cm_type = CM_SMS;

                    #if defined (__GT370__) || defined (__GT380__)
                        //��Ҫ�绰�������������
                    #else
                    if(track_cust_status_defences() != 0)
                    {
                        track_cust_check_sos_incall(&Cmds, 0);
                    }
                    else
                    {
                        track_cust_check_sos_incall(&Cmds, 2);
                    }
                    #endif
                }
                else
                {
                    track_stop_timer(TRACK_CUST_SOS_IN_CALL_TIMER);
                }
#endif /* __GW100__ */
#endif
            }
            break;

        default:
            break;
    }

}

/******************************************************************************
 *  Function    -  track_cust_module_call_status
 *
 *  Purpose     -  ���غ���״̬
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint8 track_cust_module_call_status(void)
{
    LOGD(L_APP, L_V5, "g_call_status=%d", g_call_status);
    return g_call_status;
}

void track_cust_online_cmd(kal_uint8 *stamp, char *cmdstr)
{
    char        *data_ptr, split_ch = ',';
    int         cmd_Len, par_len, i, ll;
    char        *pars[17];
    char        *Success = "WN=Success!", *Fail = "WN=Fail!";
    kal_uint8   *sendData, *p;

    LOGD(L_APP, L_V5, "online cmd=%s", cmdstr);
    par_len = track_fun_str_analyse(cmdstr, pars, 17, NULL, "#", split_ch);
    LOGD(L_APP, L_V5, "online cmd[%d]=%s,%s,%s", par_len, pars[0], pars[1], pars[2]);
    if(par_len > 17 || par_len < 0)
    {
        track_cust_paket_msg_upload(stamp, KAL_TRUE, Fail, strlen(Fail));
        return;
    }

    if(!strcmp(pars[0], "WN"))
    {
        if(par_len == 17 && !strcmp(pars[1], "A"))
        {
            for(i = 0; i < TRACK_DEFINE_WN_COUNT; i++)
            {
                ll = strlen(pars[2 + i]);
                if(ll == 0)
                {
                    memset(G_phone_number.white_num[i], 0, TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
                }
                else if(ll < TRACK_DEFINE_PHONE_NUMBER_LEN_MAX && track_fun_check_phone_number(pars[2 + i]) >= 3)
                {
                    memset(G_phone_number.white_num[i], 0, TRACK_DEFINE_PHONE_NUMBER_LEN_MAX);
                    memcpy(G_phone_number.white_num[i], pars[2 + i], ll);
                    LOGD(L_APP, L_V5, "wn:%d,number=%s", i, G_phone_number.white_num[i]);
                }
                else
                {
                    Track_nvram_read(NVRAM_EF_PHONE_NUMBER_LID, 1, (void *)&G_phone_number, NVRAM_EF_PHONE_NUMBER_SIZE);
                    track_cust_paket_msg_upload(stamp, KAL_TRUE, Fail, strlen(Fail));
                    return;
                }
            }
            Track_nvram_write(NVRAM_EF_PHONE_NUMBER_LID, 1, (void *)&G_phone_number, NVRAM_EF_PHONE_NUMBER_SIZE);
            track_cust_paket_msg_upload(stamp, KAL_TRUE, Success, strlen(Success));
        }
        else if(par_len == 1)
        {
            sendData = (U8*)Ram_Alloc(5, 500);
            if(sendData == NULL)
            {
                LOGD(L_APP, L_V1, "Ram_Alloc NULL!!");
                return;
            }
            ll = snprintf(sendData, TRACK_DEFINE_PHONE_NUMBER_LEN_MAX, "WN=%s", G_phone_number.white_num[0]);
            p = sendData + ll;
            for(i = 1; i < TRACK_DEFINE_WN_COUNT; i++)
            {
                ll = sprintf(p, ",%s", G_phone_number.white_num[i]);
                p += ll;
                if(p - sendData > 450)
                {
                    LOGD(L_APP, L_V5, "���ȳ���!");
                    track_cust_paket_msg_upload(stamp, KAL_TRUE, Fail, strlen(Fail));
                    return;
                }
            }
            LOGD(L_APP, L_V6, "�ϴ�����:%s", sendData);
            track_cust_paket_msg_upload(stamp, KAL_TRUE, sendData, strlen(sendData));
            Ram_Free(sendData);
        }
    }
}

kal_uint8 track_cust_get_SOS_callout(void)
{
    kal_uint8 issos = track_cust_sos_or_center_check(2, track_cust_call_get_incomming_number());
    kal_uint8 isFN_call = track_cust_key_dail_way(0xFF);
    kal_uint8 call_type = track_get_incall_type();
    LOGD(L_APP, L_V5, "SOS:%d,FN call:%d, call_type:%d", issos, isFN_call, call_type);

    if(issos % 8 == 0 && isFN_call == 0 && call_type == CALL_MO_OUT) //�ն���������
    {
        return 1;
    }
    return 0;
}

void track_cust_set_call_state(void)
{
    g_call_status = 1;
}



void track_cust_dial_makecall_function(void *arg)
{
    kal_uint32 status = (kal_uint32)arg;
    static kal_uint8 ScrollDialNumber = 0;    //����������ѭ����������
    static kal_uint8 ScrollDialCount = 1;//Ĭ�ϲ���һ�ε绰
    kal_bool sms_send_status = track_sms_get_send_status();

    LOGD(L_APP, L_V5, "status=%d, ScrollDialNumber=%d,sms:%d ",
         status, ScrollDialNumber, sms_send_status);
    LOGD(L_APP, L_V5, "num=%s", dail_num);

    if(status == 1)
    {
        ScrollDialNumber = 0;
        if(dail_count > 0)
        {
            ScrollDialCount = dail_count;
        }
        else
        {
            ScrollDialCount = 1;
        }
        LOGD(L_APP, L_V4, "!!! ָ��ѭ���������̿�ʼ !!!");
    }
    track_os_intoTaskMsgQueue(track_soc_disconnect); // ͨ�����Է�������æ��GPRSͨѶ��Ӱ��绰�ĳɹ����С�
    if(sms_send_status != 0)
    {
        track_start_timer(TRACK_CUST_DAIL_MAKECALL_TIMER, 1000, track_cust_dial_makecall_function, NULL);
    }
    else if(track_is_in_call())
    {
        track_cust_key_dail_way(2);
        track_cust_call_hangup();
    }
    else
    {

        if(ScrollDialNumber < ScrollDialCount)
        {
            g_call_status = 4;
#if defined(__GT310__)
            LED_TWO_COLOR_StatusSet(TWO_GSM_IN_CALL_STATE);
#elif  defined (__GT370__) || defined (__GT380__)
            track_cust_simple_gsm_led_work(GSM_IN_CALL_STATE);
#else
            LED_GSM_StatusSet(GSM_IN_CALL_STATE);
#endif

#if defined(__AUDIO_RC__)
            track_cust_monitor_set_speaket(1);
#endif /* __AUDIO_RC__ */
            track_cust_key_dail_way(0x0);
            track_cust_call(dail_num);
            tr_start_timer(TRACK_CUST_DAIL_MAKECALL_OVERTIME_TIMER, 120000, track_cust_soscall_OverTime_call);
            ScrollDialNumber++;
            LOGD(L_APP, L_V4, "!!!��(%d)�β����ܲ���(%d)��", ScrollDialNumber, ScrollDialCount);
        }
        else
        {
            if(track_cust_dial_makecall((void*)99))
            {
                track_cust_dial_makecall((void*)0);
            }
        }
    }
}
kal_uint8 track_cust_dial_makecall(void* arg)
{
    static kal_uint8 handUp = 0;
    static kal_uint8 flag = 0;
    kal_uint32 par = (kal_uint32)arg;
    LOGD(L_APP, L_V5, "%d, handUp=%d, flag=%d", par, handUp, flag);
    if(par == 99)
    {
        return flag;
    }
    else if(par == 0)
    {
        tr_stop_timer(TRACK_CUST_DAIL_MAKECALL_OVERTIME_TIMER);
        flag = 0;
        LOGD(L_APP, L_V4, "!!! ָ��ѭ���������̽��� !!!");
    }
    else if(par == 1)
    {
        if(flag != 0)
        {
            LOGD(L_APP, L_V4, "��ǰ�������ڽ��е�ָ��ѭ�����У�����������ԡ�");
            return flag;
        }

    #if defined (__LUYING__)
        if(track_cust_is_recording(99) == 1)
        {
            LOGD(L_APP, L_V4, "����¼��,ǿ�ƽ���");
            track_cust_close_record();
        }
    #endif

        flag = 1;
        handUp = 0;
        track_os_intoTaskMsgQueueExt(track_cust_dial_makecall_function, (void*)1);
    }
    else if(flag == 1 && par == 21)   // ��ͨ�绰
    {
        handUp = 1;
        tr_stop_timer(TRACK_CUST_DAIL_MAKECALL_TIMER);
        tr_stop_timer(TRACK_CUST_DAIL_MAKECALL_OVERTIME_TIMER);
    }
    else if(flag == 1 && par == 22)   // �Ҷϵ绰
    {
        tr_stop_timer(TRACK_CUST_DAIL_MAKECALL_OVERTIME_TIMER);
        if(handUp)
        {
            handUp = 0;
            flag = 0;
            LOGD(L_APP, L_V4, "!!! ָ��ѭ���������̽��� !!!");
        }
        else
        {
            track_start_timer(TRACK_CUST_DAIL_MAKECALL_TIMER, 20000, track_cust_dial_makecall_function, NULL);
        }
    }
    else if(par == 23) //sos�����������һ���ͣ��sos��������
    {
        tr_stop_timer(TRACK_CUST_DAIL_MAKECALL_TIMER);
        tr_stop_timer(TRACK_CUST_DAIL_MAKECALL_OVERTIME_TIMER);
        handUp = 0;
        flag = 0;
        LOGD(L_APP, L_V4, "!!! sos����ǿ��ֹͣ !!!");
    }
    return flag;
}

U16 track_cust_sos_get_alarm_sn(void)
{
    return g_last_alerm_no;
}

