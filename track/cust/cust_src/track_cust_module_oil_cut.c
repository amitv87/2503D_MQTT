//#ifndef __XCWS__ || #ifndef __GT740__
#if !(defined (__XCWS__)|| defined(__GT740__)|| defined(__GT370__)|| defined(__GT380__)||defined(__GT420D__))
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
static struct_msg_send msg = {0};

static char str_p_already_cut_en[] = "Already in the state of  Power supply cut off, the command is not running!";
static kal_uint8 str_p_already_cut_cn[36] =   // �ն��Ѵ��ڶϵ�״̬,��ָ���ִ��!
{
    0x7E, 0xC8, 0x7A, 0xEF, 0x5D, 0xF2, 0x59, 0x04, 0x4E, 0x8E, 0x65, 0xAD, 0x75, 0x35, 0x72, 0xB6,
    0x60, 0x01, 0x00, 0x2C, 0x67, 0x2C, 0x63, 0x07, 0x4E, 0xE4, 0x4E, 0x0D, 0x51, 0x8D, 0x62, 0x67,
    0x88, 0x4C, 0xFF, 0x01
};

static char str_p_already_en[] = "Already in the state of Power supply to resume, the command is not running!";
static kal_uint8 str_p_already_cn[40] =   // �ն��Ѵ��ڻָ���Դ״̬����ָ���ִ�У�
{
    0x7E, 0xC8, 0x7A, 0xEF, 0x5D, 0xF2, 0x59, 0x04, 0x4E, 0x8E, 0x60, 0x62, 0x59, 0x0D, 0x75, 0x35,
    0x6E, 0x90, 0x72, 0xB6, 0x60, 0x01, 0xFF, 0x0C, 0x67, 0x2C, 0x63, 0x07, 0x4E, 0xE4, 0x4E, 0x0D,
    0x51, 0x8D, 0x62, 0x67, 0x88, 0x4C, 0xFF, 0x01
};

static char str_p_cut_oil_success_en[] = "Cut off the power supply: Success!";
static kal_uint8 str_p_cut_oil_success_cn[10] =   // �ϵ�ɹ���
{
    0x65, 0xAD, 0x75, 0x35, 0x62, 0x10, 0x52, 0x9F, 0xFF, 0x01
};
static char str_p_cut_oil_delay_en[] = "Cut off the power supply: Delay! speed limit: km/h.";
static kal_uint8 str_p_cut_oil_delay_cn[38] =    // �ٶȳ��ޣ�����/ʱ���жϵ�Դ�Ӻ�ִ�У�
{
    0x90, 0x1F, 0x5E, 0xA6, 0x8D, 0x85, 0x96, 0x50, 0xFF, 0x1A, 0x51, 0x6C, 0x91, 0xCC, 0x00, 0x2F,
    0x65, 0xF6, 0xFF, 0x0C, 0x52, 0x07, 0x65, 0xAD, 0x75, 0x35, 0x6E, 0x90, 0x5E, 0xF6, 0x54, 0x0E,
    0x62, 0x67, 0x88, 0x4C, 0xFF, 0x01
};
static char str_p_cut_oil_delay2_en[] = "Cut off the power supply: Delay! GPS not location.";
static kal_uint8 str_p_cut_oil_delay2_cn[30] =   // �жϵ�Դ�Ӻ�ִ�У�GPSδ��λ
{
    0x52, 0x07, 0x65, 0xAD, 0x75, 0x35, 0x6E, 0x90, 0x5E, 0xF6, 0x54, 0x0E, 0x62, 0x67, 0x88, 0x4C,
    0xFF, 0x0C, 0x00, 0x47, 0x00, 0x50, 0x00, 0x53, 0x67, 0x2A, 0x5B, 0x9A, 0x4F, 0x4D
};
#if defined(__NT51__)||defined (__NT37__)
static char str_p_erelay_delay_en[]="Cut off the power supply: Delay! ACC ON.";
static kal_uint8 str_p_erelay_delay_cn[] =   // �жϵ�Դ�Ӻ�ִ�У�ACC ON
{
    0x52, 0x07, 0x65, 0xAD, 0x75, 0x35, 0x6E, 0x90, 0x5E, 0xF6, 0x54, 0x0E, 0x62, 0x67, 0x88, 0x4C,
    0xFF, 0x0C, 0x00,0x41,0x00,0x43,0x00,0x43,0x00,0x20,0x00,0x4F,0x00,0x4E
};
#endif
static char str_p_resume_oil_en[] = "Restore Power supply: Success!";
static kal_uint8 str_p_resume_oil_cn[14] =   //�ָ���Դ�ɹ���
{
    0x60, 0x62, 0x59, 0x0D, 0x75, 0x35, 0x6E, 0x90, 0x62, 0x10, 0x52, 0x9F, 0xFF, 0x01
};
static char str_p_cancel_oil_command_en[] = "cancelled the command of cutting-off Power supply; power supply is on.";
static kal_uint8 str_p_cancel_oil_command_cn[34] =   // �ϵ�ָ����ȡ������Դ��Ϊ��ͨ״̬��
{
    0x65, 0xAD, 0x75, 0x35, 0x63, 0x07, 0x4E, 0xE4, 0x5D, 0xF2, 0x53, 0xD6, 0x6D, 0x88, 0x30, 0x02,
    0x75, 0x35, 0x6E, 0x90, 0x73, 0xB0, 0x4E, 0x3A, 0x63, 0xA5, 0x90, 0x1A, 0x72, 0xB6, 0x60, 0x01,
    0x30, 0x02
};


#if defined(__GERMANY__)
static char str_already_cut_en[] = "Ausgang ist bereits aktiviert!";
#elif defined(__SPANISH__)
static char str_already_cut_en[] = "Corte en estado activo, el comando no puede ejecutarse otra vez!";
#else
static char str_already_cut_en[] = "Already in the state of fuel supply cut off, the command is not running!";
#endif
static kal_uint8 str_already_cut_cn[38] =   // �ն��Ѵ��ڶ��͵�״̬����ָ���ִ�У�
{
    0x7E, 0xC8, 0x7A, 0xEF, 0x5D, 0xF2, 0x59, 0x04, 0x4E, 0x8E, 0x65, 0xAD, 0x6C, 0xB9, 0x75, 0x35,
    0x72, 0xB6, 0x60, 0x01, 0xFF, 0x0C, 0x67, 0x2C, 0x63, 0x07, 0x4E, 0xE4, 0x4E, 0x0D, 0x51, 0x8D,
    0x62, 0x67, 0x88, 0x4C, 0xFF, 0x01
};
#if defined(__GERMANY__)
static char str_already_en[] = "Ausgang ist bereits abgeschaltet!";
#elif defined(__SPANISH__)
static char str_already_en[] = "El corte de combustible se encuentra liberado, No puede ejecutarse el comando otra vez!";
#else
static char str_already_en[] = "Already in the state of fuel supply to resume, the command is not running!";
#endif
static kal_uint8 str_already_cn[40] =   // �ն��Ѵ��ڻָ��͵�״̬����ָ���ִ�У�
{
    0x7E, 0xC8, 0x7A, 0xEF, 0x5D, 0xF2, 0x59, 0x04, 0x4E, 0x8E, 0x60, 0x62, 0x59, 0x0D, 0x6C, 0xB9,
    0x75, 0x35, 0x72, 0xB6, 0x60, 0x01, 0xFF, 0x0C, 0x67, 0x2C, 0x63, 0x07, 0x4E, 0xE4, 0x4E, 0x0D,
    0x51, 0x8D, 0x62, 0x67, 0x88, 0x4C, 0xFF, 0x01
};

#if defined(__GERMANY__)
static char str_cut_oil_success_en[] = "Ausgang erfolgreich geschaltet! Geschwindigkeit: km/h";
#elif defined(__SPANISH__)
static char str_cut_oil_success_en[] = "Corte del suministro de combustible realizado con exito! Velocidad: km/h.";
#else
static char str_cut_oil_success_en[] = "Cut off the fuel supply: Success! Speed: km/h.";
#endif
static kal_uint8 str_cut_oil_success_cn[28] =   // �ж��͵�ɹ����ٶȣ�����/ʱ
{
    0x52, 0x07, 0x65, 0xAD, 0x6C, 0xB9, 0x75, 0x35, 0x62, 0x10, 0x52, 0x9F, 0xFF, 0x01, 0x90, 0x1F,
    0x5E, 0xA6, 0xFF, 0x1A, 0x51, 0x6C, 0x91, 0xCC, 0x00, 0x2F, 0x65, 0xF6
};

#if defined(__GERMANY__)
static char str_cut_oil_delay_en[] = "Geschwindigkeit zu hoch: Km/h Ausgang wird verzoegert geschaltet.";
#else
static char str_cut_oil_delay_en[] = "Cut off the fuel supply: Delay! speed limit: km/h.";
#endif
static kal_uint8 str_cut_oil_delay_cn[38] =    // �ٶȳ��ޣ�����/ʱ���ж��͵��Ӻ�ִ�У�
{
    0x90, 0x1F, 0x5E, 0xA6, 0x8D, 0x85, 0x96, 0x50, 0xFF, 0x1A, 0x51, 0x6C, 0x91, 0xCC, 0x00, 0x2F,
    0x65, 0xF6, 0xFF, 0x0C, 0x52, 0x07, 0x65, 0xAD, 0x6C, 0xB9, 0x75, 0x35, 0x5E, 0xF6, 0x54, 0x0E,
    0x62, 0x67, 0x88, 0x4C, 0xFF, 0x01
};

#if defined(__GERMANY__)
static char str_cut_oil_delay2_en[] = "GPS nicht erkannt - Ausgang wird verzoegert geschaltet.";
#elif defined(__SPANISH__)
static char str_cut_oil_delay2_en[] = "Sin adquirir posicion GPS!!!  El comando de corte se ejecutara posteriormente.";
#else
static char str_cut_oil_delay2_en[] = "GPS not fixed, Cut off the fuel supply operation delay execution!";
#endif
static kal_uint8 str_cut_oil_delay2_cn[30] =   // �ж��͵��Ӻ�ִ�У�GPSδ��λ
{
    0x52, 0x07, 0x65, 0xAD, 0x6C, 0xB9, 0x75, 0x35, 0x5E, 0xF6, 0x54, 0x0E, 0x62, 0x67, 0x88, 0x4C,
    0xFF, 0x0C, 0x00, 0x47, 0x00, 0x50, 0x00, 0x53, 0x67, 0x2A, 0x5B, 0x9A, 0x4F, 0x4D
};

#if defined(__GERMANY__)
static char str_resume_oil_en[] = "Ausgang abgeschaltet!";
#elif defined(__SPANISH__)
static char str_resume_oil_en[] = "El suministro de combustible se ha restablecido con exito!";
#else
static char str_resume_oil_en[] = "Restore fuel supply: Success!";
#endif
static kal_uint8 str_resume_oil_cn[14] =   // �ָ��͵�ɹ���
{
    0x60, 0x62, 0x59, 0x0D, 0x6C, 0xB9, 0x75, 0x35, 0x62, 0x10, 0x52, 0x9F, 0xFF, 0x01
};

#if defined(__GERMANY__)
static char str_cancel_oil_command_en[] = "Schalten des Ausgangs abgebrochen!";
#elif defined(__SPANISH__)
static char str_cancel_oil_command_en[] = "El comando de corte de combustible en demora de ejecucion, se ha cancelado con exito!";
#else
static char str_cancel_oil_command_en[] = "cancelled the command of cutting-off fuel supply; fuel supply is on.";
#endif
static kal_uint8 str_cancel_oil_command_cn[36] =   // ���͵�ָ����ȡ�����͵���Ϊ��ͨ״̬��
{
    0x65, 0xAD, 0x6C, 0xB9, 0x75, 0x35, 0x63, 0x07, 0x4E, 0xE4, 0x5D, 0xF2, 0x53, 0xD6, 0x6D, 0x88,
    0xFF, 0x0C, 0x6C, 0xB9, 0x75, 0x35, 0x73, 0xB0, 0x4E, 0x3A, 0x63, 0xA5, 0x90, 0x1A, 0x72, 0xB6,
    0x60, 0x01, 0x30, 0x02
};

/****************************************************************************
*  Global Variable                      ȫ�ֱ���
*****************************************************************************/


/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/
extern kal_uint8 track_cust_status_acc(void);
extern kal_uint8 track_cust_oil_cut(kal_uint8 value);
extern void track_cust_433_control(kal_uint32 mode);
/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/

#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/

/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/
/******************************************************************************
 /******************************************************************************
 *  Function    -  track_cust_oil_cut_req
 *
 *
 *  Purpose     -  �͵翪���л�����
 *
 *
 *  Description -  ����         ����
 *                 0            ����͵�ָ�
 *                 1            ����͵�Ͽ�
 *                 2            ����Ͽ���GPSδ��λ
 *                 3            ����Ͽ������ǵ�ǰ��δ������Ͽ������� δ�������ӻ�ִ�еĶ���
 *                 4            ����Ͽ������ǵ�ǰ��δ������Ͽ������� �ѷ������ӻ�ִ�еĶ���
 *                 99           ��ѯ��ǰ���״̬
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-01-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int8 track_cust_oil_cut_req(kal_uint8 value)
{
    kal_int8 ret;
    kal_int8 temp;
    if (G_parameter.oil_sw == 0)
    {
        return -1;
    }
    temp = G_realtime_data.oil_status;
#if defined(__BCA__)
    if(G_parameter.PW5 == 2 && (value == 0 || value == 1))
    {
        track_drv_gpio_pw5v(value);
    }
#endif /* __BCA__ */
    ret = track_cust_oil_cut(value);
    if (value >= 0 && value <= 1)
    {
        G_realtime_data.oil_status = value;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        if (value != ret)
        {
            LOGD(L_APP, L_V1, "ERROR:%d,%d ���͵��ָ��͵�ִ��ʧ��", value, ret);
        }
    }
    else if (value >= 2 && value <= 4 && ret == 0)
    {
        if (G_realtime_data.oil_status >= 2 && G_realtime_data.oil_status <= 4)
        {
            if (value > G_realtime_data.oil_status)
            {
                G_realtime_data.oil_status = value;
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
            }
        }
        else
        {
            G_realtime_data.oil_status = value;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        }
    }
    if (ret == 1 || ((G_realtime_data.oil_status == 0 || G_realtime_data.oil_status == 1) && ret == 0))
    {
        if (G_realtime_data.oil_status != ret)
        {
            G_realtime_data.oil_status = ret;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        }
    }
#if defined (__XYBB__)||defined(__GT370__)||defined(__GT380__)
//NC
#else
    if(value!=99)
    {
        //ֻ�в����иı䣬�Ż��ϴ������
        if(temp!=value)
        track_cust_paket_9404_handle();
    }
#endif /* __XYBB__ */
    return G_realtime_data.oil_status;
}

/******************************************************************************
 *  Function    -  track_cust_oil_cut_boot
 *
 *  Purpose     -  �����¼״̬�Ƕ��͵��״̬������ʱ�Զ�ִ�У������������״̬
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_oil_cut_boot(void)
{
    if (G_parameter.oil_sw == 0)
    {
        return;
    }
    LOGD(L_APP, L_V5, "oil_status=%d, oil_cmd_source=%d", G_realtime_data.oil_status, G_realtime_data.oil_cmd_source);
    msg.cm_type = G_realtime_data.oil_cmd_source;
    track_fun_strncpy(msg.phone_num, G_parameter.centerNumber, CM_HEAD_PWD_MXA);
    if (G_realtime_data.oil_status == 1 || G_realtime_data.oil_status == 0)
    {
        track_cust_oil_cut_req(G_realtime_data.oil_status);
#if defined(__V20__)
        if (G_realtime_data.oil_status == 1)
        {
            track_start_timer(TRACK_CUST_433_CONTROL_DELAY_TIMER_ID, 2*1000, track_cust_433_control, (void*)3);
        }
        else if ((G_realtime_data.oil_status == 0))
        {
            track_start_timer(TRACK_CUST_433_CONTROL_DELAY_TIMER_ID, 2*1000, track_cust_433_control, (void*)7);
        }
#endif /* __V20__ */
    }
    else if (G_realtime_data.oil_status >= 2)
    {
        if (track_cust_status_acc() == 0)
        {
            track_cust_oil_cut_req(1);
#if defined(__V20__)
            track_start_timer(TRACK_CUST_433_CONTROL_DELAY_TIMER_ID, 2*1000, track_cust_433_control, (void*)3);
#endif /* __V20__ */
#if defined(__GERMANY__)
            snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Ausgang erfolgreich geschaltet!");
#elif defined(__SPANISH__)
            snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Corte del suministro de combustible realizado con exito!");
#else
            snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Cut off the fuel supply: Success!");
#endif
            msg.msg_cn_len = 14;
            track_fun_strncpy(msg.phone_num, G_parameter.centerNumber, CM_HEAD_PWD_MXA);
            memcpy(msg.msg_cn, str_cut_oil_success_cn, msg.msg_cn_len);
            track_cust_module_alarm_msg_send(&msg, NULL);
        }
    }
}
static void track_cust_oli_cut_pwm_delay_rsp()
{
    track_cust_oil_cut_req(1);
    snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Cut off the fuel supply: Success!");
    msg.msg_cn_len = 14;
    memcpy(msg.msg_cn, str_cut_oil_success_cn, msg.msg_cn_len);
    track_cust_module_alarm_msg_send(&msg, NULL);
}
void track_cust_module_oil_cut_pulse(CMD_DATA_STRUCT *cmd, kal_bool cut)
{
    kal_int8 pre_status;

    memset(&msg, 0, sizeof(struct_msg_send));
    msg.cm_type = cmd->return_sms.cm_type;
    G_realtime_data.oil_cmd_source = msg.cm_type;
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    memcpy(msg.stamp, cmd->return_sms.stamp, 4);
    track_fun_strncpy(msg.phone_num, G_parameter.centerNumber, CM_HEAD_PWD_MXA);

    if(cut == 0) //�ָ��͵�
    {
        if(track_is_timer_run(TRACK_DRV_OIL_CUT_PULSE_TIMER_ID))
        {
            tr_stop_timer(TRACK_DRV_OIL_CUT_PULSE_TIMER_ID);
        }
        if(track_is_timer_run(TRACK_CUST_OIL_CUT_PWM_DELAY_TIMER_ID))
        {
            tr_stop_timer(TRACK_CUST_OIL_CUT_PWM_DELAY_TIMER_ID);
        }
          
        pre_status = track_cust_oil_cut_req(99);
        if(pre_status == 0)
        {
            // �ն��Ѵ��ڻָ��͵�״̬����ָ���ִ�У�
            snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "%s", str_already_en);
            msg.msg_cn_len = 40;
            memcpy(msg.msg_cn, str_already_cn, msg.msg_cn_len);
            track_cust_module_alarm_msg_send(&msg, NULL);
        }
        else if(pre_status >= 2 && pre_status <= 4)
        {
            // ���͵�ָ����ȡ�����͵���Ϊ��ͨ״̬
            track_cust_oil_cut_req(0);
            snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "%s", str_cancel_oil_command_en);
            msg.msg_cn_len = 36;
            memcpy(msg.msg_cn, str_cancel_oil_command_cn, msg.msg_cn_len);
            track_cust_module_alarm_msg_send(&msg, NULL);
        }
        else
        {
            track_cust_oil_cut_req(0);
            snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "%s", str_resume_oil_en);
            msg.msg_cn_len = 14;
            memcpy(msg.msg_cn, str_resume_oil_cn, msg.msg_cn_len);
            track_cust_module_alarm_msg_send(&msg, NULL);
        }
    }
    else  //���͵�
    {
        track_gps_data_struct *p_gpsdata = track_drv_get_gps_data();

        if(track_cust_oil_cut_req(99) == 1)
        {
            snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "%s", str_already_cut_en);
            msg.msg_cn_len = 38;
            memcpy(msg.msg_cn, str_already_cut_cn, msg.msg_cn_len);
            track_cust_module_alarm_msg_send(&msg, NULL);
        }
        else if(track_cust_status_acc() == 0) //Ϩ��
        {
            track_cust_oil_cut_req(1);
            snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Cut off the fuel supply: Success!");
            msg.msg_cn_len = 14;
            memcpy(msg.msg_cn, str_cut_oil_success_cn, msg.msg_cn_len);
            track_cust_module_alarm_msg_send(&msg, NULL);
        }
        else if((p_gpsdata->status >= 2) && (p_gpsdata->gprmc.Speed <= 20)) //��λ+����
        {
            track_cust_oil_cut_req(1);
            snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Cut off the fuel supply: Success!");
            msg.msg_cn_len = 14;
            memcpy(msg.msg_cn, str_cut_oil_success_cn, msg.msg_cn_len);
            track_cust_module_alarm_msg_send(&msg, NULL);
        }
        else  //ACC ON/GPS����λ�����
        {
            track_cust_oil_cut_req(1);//1���Ӻ�����������͵�
            LOGD(L_APP, L_V4, "�̵���������ͨ�ж��͵�(ACC ON/GPS����λ�����)");
            track_drv_oli_cut_pwm((void*)60);
            tr_start_timer(TRACK_CUST_OIL_CUT_PWM_DELAY_TIMER_ID, 65 * 1000, track_cust_oli_cut_pwm_delay_rsp);
        }
    }
}

/******************************************************************************
 *  Function    -  track_cust_module_oil_cut
 *
 *  Purpose     -  ���͵纯��
 *
 *  Description -  ������value = FALSE   �ָ��͵�ͨ·
 *                       value = TRUE    �Ͽ��͵�
 *
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 24-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_oil_cut_cmd(CMD_DATA_STRUCT *cmd, kal_bool value, kal_bool force)
{
    if (G_parameter.oil_sw == 0)
    {
        return;
    }
    LOGD(L_APP, L_V5, "cm_type=%d, oil_status=%d, oil_cmd_source=%d", cmd->return_sms.cm_type, G_realtime_data.oil_status, G_realtime_data.oil_cmd_source);

    memset(&msg, 0, sizeof(struct_msg_send));
    msg.cm_type = cmd->return_sms.cm_type;
    G_realtime_data.oil_cmd_source = msg.cm_type;
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    memcpy(msg.stamp, cmd->return_sms.stamp, 4);
#if defined(__MEX__)
    if(!strcmp(cmd->pars[0], "SRELAY"))
    {
        track_fun_strncpy(msg.phone_num, cmd->return_sms.ph_num, CM_HEAD_PWD_MXA);
    }
    else
#endif
    {
#if defined(__SPANISH__)
        track_fun_strncpy(msg.phone_num, cmd->return_sms.ph_num, CM_HEAD_PWD_MXA);
#else
        track_fun_strncpy(msg.phone_num, G_parameter.centerNumber, CM_HEAD_PWD_MXA);
#endif
    }

    //���͵�
    if (value)
    {
#if defined (__ET200__) ||defined(__ET130__)|| defined(__V20__) || defined(__ET310__) || defined(__MT200__)||defined (__ET320__)||defined (__NT37__)
        if (force == KAL_TRUE)
        {
            if (track_cust_oil_cut_req(99) == 1)
            {
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "%s", str_p_already_cut_en);
                msg.msg_cn_len = 36;
                memcpy(msg.msg_cn, str_p_already_cut_cn, msg.msg_cn_len);
                track_cust_module_alarm_msg_send(&msg, NULL);
            }
            else
            {
#if defined(__NT51__) ||defined (__NT37__)
               /*ERELAY*/
                if(!strcmp(cmd->pars[0], "ERELAY"))
                {
                    if(track_cust_status_acc()==1)
                    {
                        G_parameter.erelay_flag=KAL_TRUE;
                        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                        LOGD(L_APP, L_V4,"���͵��Ӻ�ִ�У�ACCΪ��");
                        snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "%s", str_p_erelay_delay_en);
                        msg.msg_cn_len = 30;
                        memcpy(msg.msg_cn, str_p_erelay_delay_cn, msg.msg_cn_len);
                        track_cust_module_alarm_msg_send(&msg, NULL);
                        return;
                    }
                    else
                    {
                        //track_cust_oil_cut_req(1);
                        G_parameter.erelay_flag=KAL_FALSE;
                        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                        LOGD(L_APP, L_V4,"���͵�ɹ���ACCΪ��");
                    }  
                }
                track_cust_oil_cut_req(1);
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "%s", str_p_cut_oil_success_en);
                msg.msg_cn_len = 10;
                memcpy(msg.msg_cn, str_p_cut_oil_success_cn, msg.msg_cn_len);
                track_cust_module_alarm_msg_send(&msg, NULL);
#else
                track_cust_oil_cut_req(1);
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "%s", str_p_cut_oil_success_en);
                msg.msg_cn_len = 10;
                memcpy(msg.msg_cn, str_p_cut_oil_success_cn, msg.msg_cn_len);
                track_cust_module_alarm_msg_send(&msg, NULL);
#endif
#if defined(__V20__)
                track_cust_433_control(3);
                G_realtime_data.oil_lock_status = 1;
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
#endif /* __V20__ */
            }
        }
        else
#endif /* __ET200__ */
        {
            if (track_cust_oil_cut_req(99) == 1)
            {
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "%s", str_already_cut_en);
                msg.msg_cn_len = 38;
                memcpy(msg.msg_cn, str_already_cut_cn, msg.msg_cn_len);
                track_cust_module_alarm_msg_send(&msg, NULL);
            }
            else if (force || track_cust_status_acc() == 0)
            {
                track_cust_oil_cut_req(1);
#if defined(__GERMANY__)
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Ausgang erfolgreich geschaltet!");
#elif defined(__SPANISH__)
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Corte del suministro de combustible realizado con exito!");
#else
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Cut off the fuel supply: Success!");
#endif
                msg.msg_cn_len = 14;
                memcpy(msg.msg_cn, str_cut_oil_success_cn, msg.msg_cn_len);
                track_cust_module_alarm_msg_send(&msg, NULL);
#if defined(__V20__)
                track_cust_433_control(3);
                G_realtime_data.oil_lock_status = 1;
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
#endif /* __V20__ */
            }
            else if (track_cust_gps_status() < GPS_STAUS_2D)
            {
                LOGD(L_APP, L_V4, "�ж��͵��Ӻ�ִ�У�GPSδ��λ");
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "%s", str_cut_oil_delay2_en);
                msg.msg_cn_len = 30;
                memcpy(msg.msg_cn, str_cut_oil_delay2_cn, msg.msg_cn_len);
                track_cust_module_alarm_msg_send(&msg, NULL);
                track_cust_oil_cut_req(2);
            }
            else
            {
                LOGD(L_APP, L_V5, "�ж��͵��Ӻ�ִ�У�GPS��λ");
                track_cust_oil_cut_req(3);
            }

        }

    }
    else
    {
        kal_int8 ret = track_cust_oil_cut_req(99);
#if defined (__ET200__)||defined(__ET130__)|| defined(__V20__) || defined(__ET310__) || defined(__MT200__)||defined (__ET320__)
        if (force == KAL_TRUE)
        {
            if (ret == 0)
            {
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "%s", str_p_already_en);
                msg.msg_cn_len = 40;
                memcpy(msg.msg_cn, str_p_already_cn, msg.msg_cn_len);
                track_cust_module_alarm_msg_send(&msg, NULL);
            }
            else if (ret >= 2 && ret <= 4)
            {
                track_cust_oil_cut_req(0);
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "%s", str_p_cancel_oil_command_en);
                msg.msg_cn_len = 34;
                memcpy(msg.msg_cn, str_p_cancel_oil_command_cn, msg.msg_cn_len);
                track_cust_module_alarm_msg_send(&msg, NULL);
#if defined(__V20__)
                track_cust_433_control(7);
                G_realtime_data.oil_lock_status = 0;
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
#endif /* __V20__ */
            }
            else
            {
                track_cust_oil_cut_req(0);
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "%s", str_p_resume_oil_en);
                msg.msg_cn_len = 14;
                memcpy(msg.msg_cn, str_p_resume_oil_cn, msg.msg_cn_len);
                track_cust_module_alarm_msg_send(&msg, NULL);
#if defined(__V20__)
                track_cust_433_control(7);
                G_realtime_data.oil_lock_status = 0;
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
#endif /* __V20__ */
            }
        }
        else
#endif /* __ET200__ */
        {
            if (ret == 0)
            {
                // �ն��Ѵ��ڻָ��͵�״̬����ָ���ִ�У�
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "%s", str_already_en);
                msg.msg_cn_len = 40;
                memcpy(msg.msg_cn, str_already_cn, msg.msg_cn_len);
                track_cust_module_alarm_msg_send(&msg, NULL);
            }
            else if (ret >= 2 && ret <= 4)
            {
                // ���͵�ָ����ȡ�����͵���Ϊ��ͨ״̬
                track_cust_oil_cut_req(0);
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "%s", str_cancel_oil_command_en);
                msg.msg_cn_len = 36;
                memcpy(msg.msg_cn, str_cancel_oil_command_cn, msg.msg_cn_len);
                track_cust_module_alarm_msg_send(&msg, NULL);
#if defined(__V20__)
                track_cust_433_control(7);
                G_realtime_data.oil_lock_status = 0;
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
#endif /* __V20__ */
            }
            else
            {
                track_cust_oil_cut_req(0);
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "%s", str_resume_oil_en);
                msg.msg_cn_len = 14;
                memcpy(msg.msg_cn, str_resume_oil_cn, msg.msg_cn_len);
                track_cust_module_alarm_msg_send(&msg, NULL);
#if defined(__V20__)
                track_cust_433_control(7);
                G_realtime_data.oil_lock_status = 0;
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
#endif /* __V20__ */
            }
        }
    }
}
#if defined(__NT51__)||defined (__NT37__)
void track_cust_erelay_send_sms(void*arg)
{
    kal_uint32 send_flag=(kal_uint32)arg;
    if(send_flag==0)
    {
        snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "%s", str_p_already_cut_en);
        msg.msg_cn_len = 36;
        memcpy(msg.msg_cn, str_p_already_cut_cn, msg.msg_cn_len);
        track_cust_module_alarm_msg_send(&msg, NULL);
    }
    else if(send_flag==1)
    {
        snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "%s", str_p_cut_oil_success_en);
        msg.msg_cn_len = 10;
        memcpy(msg.msg_cn, str_p_cut_oil_success_cn, msg.msg_cn_len);
        track_cust_module_alarm_msg_send(&msg, NULL);
    }
}
#endif 
/******************************************************************************
 *  Function    -  track_cust_module_oil_cut_gps
 *
 *  Purpose     -  ����GPS�����е��ٶ��ж��Ƿ���ж��͵�
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_oil_cut_gps(track_gps_data_struct *gps_data)
{
    static kal_uint8 Speed_cnt = 0;
    kal_uint8 speed_temp = 20;
    kal_uint8 count_temp = 10;
    if (G_parameter.oil_sw == 0)
    {
        return;
    }
    if (G_realtime_data.oil_status >= 2 && G_realtime_data.oil_status <= 4)
    {
        char tmp[20] = {0};
        int len, speed;
        speed = gps_data->gprmc.Speed;
        if (speed < speed_temp)
        {
            if (Speed_cnt < count_temp)
            {
                Speed_cnt++;
            }
            else
            {
                track_cust_oil_cut_req(1);
#if defined(__V20__)
                track_cust_433_control(3);
#endif /* __v20__ */
#if defined(__GERMANY__)
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Ausgang erfolgreich geschaltet! Geschwindigkeit: %dkm/h.", speed);
#elif defined(__SPANISH__)
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Corte del suministro de combustible realizado con exito! Velocidad: %dkm/h.", speed);
#else
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Cut off the fuel supply: Success! Speed: %dkm/h.", speed);
#endif
                snprintf(tmp, 20, "%d", speed);
                memcpy(msg.msg_cn, str_cut_oil_success_cn, 20);                 // �ж��͵�ɹ����ٶȣ�
                len = track_fun_asc_str_to_ucs2_str(&msg.msg_cn[20], tmp) - 2;
                msg.msg_cn_len = 28 + len;
                if (CM_PARAM_LONG_LEN_MAX <= msg.msg_cn_len) return;
                memcpy(&msg.msg_cn[len + 20], &str_cut_oil_success_cn[20], 8);  // ����/ʱ
                track_cust_module_alarm_msg_send(&msg, NULL);
            }
        }
        else
        {
            Speed_cnt = 0;
            if (G_realtime_data.oil_status != 4)
            {
#if defined(__GERMANY__)
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Geschwindigkeit zu hoch: %dkm/h Ausgang wird verzoegert geschaltet.", speed);
#elif defined(__SPANISH__)
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "La velocidad excede el limite de seguridad: %dkm/h. El comando se ejecutara posteriormente!", speed);
#else                
                snprintf(msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Speed  exceeds the safety limit: %dkm /h, Cut off the fuel supply operation delay execution!", speed);
#endif
                snprintf(tmp, 20, "%d", speed);
                memcpy(msg.msg_cn, str_cut_oil_delay_cn, 10);
                len = track_fun_asc_str_to_ucs2_str(&msg.msg_cn[10], tmp) - 2;
                msg.msg_cn_len = 38 + len;
                if (CM_PARAM_LONG_LEN_MAX <= msg.msg_cn_len) return;
                memcpy(&msg.msg_cn[len + 10], &str_cut_oil_delay_cn[10], 28);
                track_cust_module_alarm_msg_send(&msg, NULL);
                track_cust_oil_cut_req(4);
            }
        }
    }
    else
    {
        Speed_cnt = 0;
    }
}
#endif
#if defined(__GERMANY__)
/******************************************************************************
 *  Function    -  track_cust_module_Preheat_System
 *
 *  Purpose     -  ����ʱ���ж϶��͵�̵�����ͨʱ��(����Ԥ��ϵͳ)
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 22-12-2016,  Cjj  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_Preheat_System(kal_uint8 time)
{
    if(track_cust_status_acc() == 1)
    {
        return;
    }
        switch(time)
        {
            case 0:
                if(track_is_timer_run(TRACK_CUST_PREHET_SYSTEM_TIMER_ID))
                {
                    track_stop_timer(TRACK_CUST_PREHET_SYSTEM_TIMER_ID);
                }
                track_cust_oil_cut_req(1);
                break;
            case 1:
                track_cust_oil_cut_req(0);
                break;
            case 2:
                track_cust_oil_cut_req(0);
                track_start_timer(TRACK_CUST_PREHET_SYSTEM_TIMER_ID, 2*1000, track_cust_oil_cut_req, (void*)1);
                break;
            case 10:
                track_cust_oil_cut_req(0);
                track_start_timer(TRACK_CUST_PREHET_SYSTEM_TIMER_ID, 10*60*1000, track_cust_oil_cut_req, (void*)1);
                break;
            case 20:
                track_cust_oil_cut_req(0);
                track_start_timer(TRACK_CUST_PREHET_SYSTEM_TIMER_ID, 20*60*1000, track_cust_oil_cut_req, (void*)1);
                break;
            case 30:
                track_cust_oil_cut_req(0);
                track_start_timer(TRACK_CUST_PREHET_SYSTEM_TIMER_ID, 30*60*1000, track_cust_oil_cut_req, (void*)1);
                break;
            default:
                if(track_is_timer_run(TRACK_CUST_PREHET_SYSTEM_TIMER_ID))
                {
                    track_stop_timer(TRACK_CUST_PREHET_SYSTEM_TIMER_ID);
                }
                break;
        }
}
#endif
