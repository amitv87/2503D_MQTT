#if defined(__XCWS__)
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
static terminal_work_mode WORK_MODE = NORNAL_MODE;
/****************************************************************************
*  Global Variable                      ȫ�ֱ���
*****************************************************************************/
/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/
extern nvram_parameter_struct     G_parameter;
/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/

extern void gprs_gps_data_send(track_gps_data_struct *gpsdata);


/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/

/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/
/******************************************************************************
 *  Function    -  track_cust_work_mode_ind
 *
 *  Purpose     -  ��ȡ��������ģʽ�������Ϣ
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 28-03-2013,  jwj  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_work_mode_ind(work_msg_ind type)
{
    LOGD(L_APP, L_V4, "type =%d ", type);
    //track_cust_module_alarm_vibrates((void*)2);
    switch(type)
    {
        
        case w_nornal_charge_out_msg:
        case w_charge_out_msg:
             LOGD(L_APP, L_V4, "t_sleep =%d,charge_out=%d ", G_parameter.sleep_wakeup.t_sleep,G_parameter.sleep_wakeup.chargerout_wait);
             track_cust_set_work_mode((void *)9);
             if(G_parameter.sleep_wakeup.t_sleep == 1)
             {
                track_start_timer(TRACK_GPSOFF_RELAY_TIMER_ID, (G_parameter.sleep_wakeup.chargerout_wait) * 60000, track_cust_set_work_mode, (void *)SLEEP_MODE);
             }
             break;
                
        case w_timer_in_nornal_msg:
        case w_sensor_smg:
            if(track_is_timer_run(TERMINAL_SLEEP_TO_WAKEUP_TIMER_ID))
            {
                tr_stop_timer(TERMINAL_SLEEP_TO_WAKEUP_TIMER_ID);
            }
            if(track_cust_get_work_mode_cq() != NORNAL_MODE)
            {
                tr_stop_timer(TRACK_GPSOFF_RELAY_TIMER_ID);
                track_cust_set_work_mode((void *)NORNAL_MODE);
            }
            break;
            
        case w_charge_in_msg:
            tr_stop_timer(TRACK_GPSOFF_RELAY_TIMER_ID);
            if(track_is_timer_run(TERMINAL_SLEEP_TO_WAKEUP_TIMER_ID))
            {
                tr_stop_timer(TERMINAL_SLEEP_TO_WAKEUP_TIMER_ID);
            }
            if(track_cust_get_work_mode_cq() != NORNAL_MODE)
            {
                track_cust_set_work_mode((void *)NORNAL_MODE);
            }
            else
            {
                track_cust_set_work_mode((void *)9);
                track_cust_gps_work_req((void *)9);
            }
            break;
        case w_contorl_gps_on_msg:
            tr_stop_timer(TRACK_GPSOFF_RELAY_TIMER_ID);
            if(track_is_timer_run(TERMINAL_SLEEP_TO_WAKEUP_TIMER_ID))
            {
                tr_stop_timer(TERMINAL_SLEEP_TO_WAKEUP_TIMER_ID);
            }
            if(track_cust_get_work_mode_cq() != NORNAL_MODE)
            {
                track_cust_set_work_mode((void *)NORNAL_MODE);
            }
            break;
        case w_dw_msg:
            if(track_is_timer_run(TERMINAL_SLEEP_TO_WAKEUP_TIMER_ID))
            {
                tr_stop_timer(TERMINAL_SLEEP_TO_WAKEUP_TIMER_ID);
            }
            if(track_cust_get_work_mode_cq() != NORNAL_MODE)
            {
                tr_stop_timer(TRACK_GPSOFF_RELAY_TIMER_ID);
                track_cust_set_work_mode((void *)NORNAL_MODE);
            }
        case w_acc_low_msg:
             if((G_parameter.sleep_wakeup.t_sleep == 1) && (track_cust_status_charger() == 0))
             {
                if(!track_is_timer_run(TRACK_GPSOFF_RELAY_TIMER_ID))
                {
                    track_start_timer(TRACK_GPSOFF_RELAY_TIMER_ID, (G_parameter.sleep_wakeup.chargerout_wait) * 60000, track_cust_set_work_mode, (void *)SLEEP_MODE);
                }
             }
            break;

        case w_acc_high_msg:
            //tr_stop_timer(TRACK_GPSOFF_RELAY_TIMER_ID);
            //track_cust_set_work_mode((void *)NORNAL_MODE);
            if((G_parameter.sleep_wakeup.t_sleep == 1) && (track_cust_status_charger() == 0))
            {
                if(!track_is_timer_run(TRACK_GPSOFF_RELAY_TIMER_ID))
                {
                    track_start_timer(TRACK_GPSOFF_RELAY_TIMER_ID, (G_parameter.sleep_wakeup.chargerout_wait) * 60000, track_cust_set_work_mode, (void *)SLEEP_MODE);
                }
            }
            break;
        default:
            break;


    }
}
/******************************************************************************
 *  Function    -  track_cust_defense_mode_ind
 *
 *  Purpose     -  ���յ������ص���Ϣ
 * ----------------------------------------
 ******************************************************************************/
void track_cust_defense_mode_ind(defense_msg_ind type)
{
    LOGD(L_APP, L_V4, "type =%d ", type);
    switch(type)
    {
        case d_acc_high_msg:
        case d_cmd_disarm_msg:
            track_cust_set_defense_mode_cq((void *)DISARM_NORNAL);
            tr_stop_timer(TRACK_DEFENCE_RELAY_TIMER_ID);
            break;

        case d_acc_low_msg:
            if(G_parameter.t_acclock == 1)
            {
                track_start_timer(TRACK_DEFENCE_RELAY_TIMER_ID, G_parameter.acc_starting_time * 1000, track_cust_set_defense_mode_cq, (void *) FORTIFACATION_MODE);
                track_cust_set_defense_mode_cq((void*)DELAY_FORTIFACATION_MODE);
            }
            break;
        case d_nvram_fortifation_msg:
        case d_cmd_fortifation_msg:
            track_cust_set_defense_mode_cq((void *)FORTIFACATION_MODE);
            tr_stop_timer(TRACK_DEFENCE_RELAY_TIMER_ID);
            break;
        default:
            break;
    }
}
/******************************************************************************
 *  Function    -  track_cust_set_work_mode
 *
 *  Purpose     -  ��������ͨģʽ��������ģʽ
 */
void track_cust_set_work_mode(void *arg)
{
    static kal_uint32 flag = 0;
    flag = (kal_uint32)arg;

    LOGD(L_APP, L_V4, " %d",flag);
    if(WORK_MODE != flag || flag == 9)
    {
        if(track_soc_login_status())
        {
            LOGD(L_APP, L_V4, " �����ı��ϴ�T3��");
            track_os_intoTaskMsgQueueExt(gprs_gps_data_send, track_cust_backup_gps_data(0, NULL));
        }
        if(flag == 9) return;
    }
    WORK_MODE = flag;
    LOGD(L_APP, L_V4, "WORK_MODE =%d ", WORK_MODE);
    tr_stop_timer(TRACK_CUST_GPS_LOCATION_TIMEOUT_TIMER_ID);
    if(WORK_MODE == SLEEP_MODE)
    {
        LOGD(L_APP, L_V5, "WORK_MODE =SLEEP_MODE �ϵ���Ϣ%d����",G_parameter.sleep_wakeup.GSM_Sleep_Starting_Time_interval);
        track_cust_gps_control(0);
        if(G_parameter.sleep_wakeup.t_wakeup == 1)
        {
            if(!track_is_timer_run(TERMINAL_SLEEP_TO_WAKEUP_TIMER_ID))
            {
                track_start_timer(TERMINAL_SLEEP_TO_WAKEUP_TIMER_ID, G_parameter.sleep_wakeup.GSM_Sleep_Starting_Time_interval * 60000, track_cust_set_work_mode, (void *)NORNAL_MODE);//������ʱ���ѹ���
            }
        }
    }
    else
    {
        LOGD(L_APP, L_V4, "WORK_MODE =NORNAL_MODE%d �������ˣ����ӵ磬�һ��ǻ�˯��", flag);
        WORK_MODE = NORNAL_MODE;
        track_cust_gps_work_req((void *)9);
        if(G_parameter.sleep_wakeup.t_sleep == 1  && track_cust_status_charger() == 0)
        {
            track_start_timer(TRACK_GPSOFF_RELAY_TIMER_ID, (G_parameter.sleep_wakeup.chargerout_wait) * 60000, track_cust_set_work_mode, (void *)SLEEP_MODE);
        }
    }
}

/******************************************************************************
 *  Function    -  track_cust_set_defense_mode_cq
 *
 *  Purpose     -  �����������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 28-03-2013,  jwj  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_set_defense_mode_cq(void *arg)
{
    static kal_uint32 flag = 0;
    flag = (kal_uint32)arg;
    LOGD(L_APP, L_V4, "defense_MODE =%d ", flag);
    if(flag != G_realtime_data.defences_status)
    {
        if(track_soc_login_status(99))
        {
            LOGD(L_APP, L_V4, " ���״̬�ı��ϴ�T3��");
            track_os_intoTaskMsgQueueExt(gprs_gps_data_send, track_cust_backup_gps_data(0, NULL));
        }
    }

    if(DELAY_FORTIFACATION_MODE == flag)
    {
        track_cust_status_defences_change((void *)1);
    }
    else if(FORTIFACATION_MODE == flag)
    {
        track_cust_status_defences_change((void *)2);
    }
    else
    {
        track_cust_status_defences_change((void *)0);
    }

    G_realtime_data.defences_status = flag;
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
}
/******************************************************************************
 *  Function    -  track_cust_get_work_mode
 *
 *  Purpose     -  ��ȡϵͳ����������������ģʽ
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 28-03-2013,  jwj  written
 * ----------------------------------------
 ******************************************************************************/

terminal_work_mode track_cust_get_work_mode_cq(void)
{
    LOGD(L_APP, L_V4, "WORK_MODE =%d ", WORK_MODE);
    return WORK_MODE;
}
/******************************************************************************
 *  Function    -  track_cust_get_defense_mode
 *
 *  Purpose     -  ��ȡ������ǳ���ģʽ
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 28-03-2013,  jwj  written
 * ----------------------------------------
 ******************************************************************************/
terminal_defense_mode track_cust_get_defense_mode(void)
{
    LOGD(L_APP, L_V4, "defense_MODE =%d ", G_realtime_data.defences_status);
    return G_realtime_data.defences_status;
}
#endif /* __XCWS__ */
