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
static kal_uint8 acc = 2;
static kal_uint8 chargerStatus = 0;
static kal_uint8 battery_level = 0;
static kal_uint8 defences = 0;         //0��������1��Ԥ�����2�����
static kal_uint32 g_volt = 0;
static kal_uint32 g_ext_volt = 0;
static kal_uint32 g_real_volt = 0;
#if defined(__FLY_MODE__)
static kal_bool g_islowbat = 0;
#endif /* __FLY_MODE__ */
/****************************************************************************
*  Global Variable                      ȫ�ֱ���
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/
extern kal_uint8 red_led_status;
/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/
extern void track_cust_module_sd_acc(void *arg);
extern void track_cust_module_sos_trigger_button(void);

#if defined(__GT740__)||defined(__GT420D__)
extern void track_cust_modeule_into_sleep_mode();
#endif

#if !(defined (__XCWS__)|| defined(__GT740__)||defined(__GT420D__))

extern void track_cust_oil_cut_boot(void);
#endif
extern void track_cust_key_sos(void);
#if defined (__NETWORK_LICENSE__)
extern void track_drv_network_license_eint_make_call(void);
#endif
#if defined __USE_SPI__
extern void track_spi_re(kal_uint8 *buffer);
#endif
extern void track_cust_alarm_acc_change(void *arg);
#if defined(__ACC_ALARM__)
extern void track_cust_acc_change_alarm(void *arg);
#endif
extern void track_cust_key_short_press_sos(void);

#if defined (__CUST_BT__)
extern void track_drv_bt_and_gsm_sleep(void);
#endif

#if defined (__GT370__)|| defined (__GT380__)
extern void track_drv_mcu_and_gsm_sleep(kal_uint8 option);
#endif /* __GT370__ */

#if defined (__CUST_RS485__)
extern void track_drv_rs485_sleep_enable(void);
#endif /* __CUST_RS485__ */
#if defined(__NT51__)||defined (__NT37__)
extern void track_cust_erelay_send_sms(void * arg);
#endif
/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/
kal_uint8 track_cust_status_sim();
kal_uint8 track_cust_status_acc(void);
void acc_status_high(void);
kal_uint8 track_cust_status_charger(void);
#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/
#if defined(__GT420D__)
kal_uint32 track_cust_MCUset_adc_data(kal_uint32 arg)
{
    g_volt = arg * 10000;
    }
#endif

#if defined(__XCWS__)
static void acc_low_disable_chargout_alarm(void)
{

}
#endif
/******************************************************************************
 *  Function    -  acc_status_change
 *
 *  Purpose     -  ACC״̬
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void acc_status_change(void *arg)
{
    static kal_uint8 def_tmp = 0;
    LOGD(L_APP, L_V5, "ACC :%d,%d,%d", (kal_uint32)arg, G_realtime_data.acc_last_status, def_tmp);
#if defined(__NETWORK_LICENSE__)
    return;
#endif

#if defined(__FLY_MODE__) && !defined(__MT200__)
    if(G_parameter.flycut != 0 && track_cust_isbatlow())
    {
        LOGD(L_APP, L_V5, "��ص͵�״̬�£���ά�ֳ�磬ACC״̬����¼����������Ӧ����");
        if((U32)arg == 1 || (U32)arg == 0)
            acc = (U32)arg;
        return;
    }
#endif /* __FLY_MODE__ */

#if !defined(__ET310__)
    if(def_tmp == 0 && G_realtime_data.defense_mode == 1)    /* �ֶ����ģʽ */
    {
        if(G_realtime_data.defences_status >= 2)
        {
            track_os_intoTaskMsgQueueExt(track_cust_status_defences_change, (void *)2);
        }
        def_tmp = 1;
    }
#endif /* __ET310__ */
#if defined (__NT22__)
    if(G_parameter.bb_data.acc_check  == 0)
    {
        if(def_tmp == 0)
        {
            def_tmp =  1;
            acc_status_high();
        }
        return;
    }
    else
    {
        def_tmp = 0;
    }
#endif /* __XYBB__ */
    if((kal_uint32)arg)
    {
        #if !defined(__NT37__)
        if(G_parameter.motor_lock.sw == 1)
        {
            track_motor_lock_change_status(MOTOR_ACC_HIGH);
            if(track_cust_motor_lock_status_check())
            {
                // ����������Ĵ��������У�����ACC�����
                return;
            }
        }
        #endif
        acc_status_high();
    }
    else
    {
        #if !defined(__NT37__)
        if(G_parameter.motor_lock.sw == 1)
        {
            track_motor_lock_change_status(MOTOR_ACC_LOW);
            if(track_cust_motor_lock_status_check())
            {
                // �����������״̬�£�����ACC�����
                return;
            }
        }
        #endif
        // ����Ϩ��
        LOGD(L_APP, L_V5, "ACC OFF");
        acc = 0;
#if defined(__NT51__)||defined (__NT37__)
        if(G_parameter.erelay_flag)
        {
            if(track_cust_oil_cut_req(99) == 1)
            {
                //LOGD(L_APP,L_V4,"�Ѵ��ڶ��͵�״̬,erelay���͵粻ִ��");
                track_os_intoTaskMsgQueueExt(track_cust_erelay_send_sms, (void*)0);
            }
            else
            {
                track_cust_oil_cut_req(1);
                //LOGD(L_APP,L_V4,"ERELAY���͵�ִ�гɹ�!");
                track_os_intoTaskMsgQueueExt(track_cust_erelay_send_sms, (void*)1);
            }
            G_parameter.erelay_flag = KAL_FALSE;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        }
#endif
#if !(defined(__GT02__) || defined(__XCWS__))
        track_os_intoTaskMsgQueueExt(heartbeat_packets, (void *)116);
#endif /* __GT02__ */
        track_os_intoTaskMsgQueue(track_cust_sensor_init);
        if(G_realtime_data.acc_last_status != (kal_uint32)arg)
        {
            track_os_intoTaskMsgQueueExt(track_cust_module_sd_acc, (void *)1);
            track_cust_module_power_fails((void *)20);
            track_os_intoTaskMsgQueue(track_cust_add_position);
#if !(defined (__XCWS__)|| defined(__GT740__)|| defined(__GT370__)|| defined(__GT380__)||defined(__GT420D__))
            track_os_intoTaskMsgQueue(track_cust_oil_cut_boot);
#endif
            track_cust_led_acc_change_status(0);

#if defined(__ACC_ALARM__)
            if(G_parameter.accalm.accalm_sw == 1)
            {
                if(track_is_timer_run(TRACK_CUST_ALARM_ACC_TIMER))
                {
                    track_stop_timer(TRACK_CUST_ALARM_ACC_TIMER);
                }
                track_start_timer(TRACK_CUST_ALARM_ACC_TIMER, G_parameter.accalm.accalm_time * 1000, track_cust_acc_change_alarm, (void*)0);
            }
#else
            if(G_parameter.accalm_sw == 1)
            {
                track_start_timer(TRACK_CUST_ALARM_ACC_TIMER, 1000, track_cust_alarm_acc_change, (void*)0);
            }
#endif

            track_cust_gps_work_req((void *)4);

#if defined(__BCA__)
            if(G_parameter.PW5 == 3)
            {
                track_drv_gpio_pw5v(0);
            }
#endif /* __BCA__ */
        }
#if defined(__XCWS__)
        track_cust_led_red_status_change((void*)0);
        if(G_parameter.acc_low_chargeout_time != 0)
        {
            tr_start_timer(TRACK_CUST_ACCL_CHARGEOUT_CHECK_TIMER_ID, (G_parameter.acc_low_chargeout_time) * 1000, acc_low_disable_chargout_alarm);
        }
        if(G_realtime_data.acc_last_status == 1)
        {
            track_cust_defense_mode_ind(d_acc_low_msg);
        }
        else
        {
            if(G_realtime_data.defences_status == 2)
            {
                track_cust_defense_mode_ind(d_acc_low_msg);
            }
            else if(G_realtime_data.defences_status == 3)
            {
                if(G_parameter.t_acclock == 1)
                {
                    track_cust_defense_mode_ind(d_nvram_fortifation_msg);
                }
                else
                {
                    track_cust_set_defense_mode_cq((void*)1);//����
                }
            }
            else
            {
                track_cust_set_defense_mode_cq((void*)1);//����
                if(G_parameter.t_acclock == 1)
                    track_start_timer(TRACK_CUST_STATUS_DEFENCES_TIMER_ID, G_parameter.acc_starting_time * 1000, track_cust_status_defences_change, (void*)2);
            }
        }
        track_cust_work_mode_ind(w_acc_low_msg);
#else
        if(G_realtime_data.defense_mode == 0)    /* �Զ����ģʽ */
        {
            if(G_realtime_data.defences_status == 3)
            {
                track_os_intoTaskMsgQueueExt(track_cust_status_defences_change, (void *)2);
            }
            else
            {
                track_cust_status_defences_change((void*)1);
                track_start_timer(
                    TRACK_CUST_STATUS_DEFENCES_TIMER_ID,
                    G_parameter.defense_time * 1000,
                    track_cust_status_defences_change, (void*)2);
            }
#if defined(__V20__)
            track_cust_433_control(3);
#endif /* __V20__ */
        }
#if defined(__OBD__)
        if(G_parameter.obd.sw == 1)
        {
            track_obd_acc((void*)0);
        }
#endif /* __OBD__ */

#endif
    }
    G_realtime_data.acc_last_status = (kal_uint32)arg;
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
}

/****************************************************************************
*  Global Functions                     ȫ�ֺ���
*****************************************************************************/

/******************************************************************************
 *  Function    -  acc_status_high
 *
 *  Purpose     -  ACC�߶�Ӧ����
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void acc_status_high(void)
{
    // �������
    LOGD(L_APP, L_V5, "ACC ON");
    acc = 1;
#if defined(__XCWS__)
    track_cust_led_red_status_change((void*)1);
    if(G_realtime_data.acc_last_status == 0)
    {
        track_cust_defense_mode_ind(d_acc_high_msg);
    }
    else
    {
        if(G_realtime_data.defences_status == 3)
        {
            track_cust_defense_mode_ind(d_nvram_fortifation_msg);//���
        }
        else
        {
            track_cust_defense_mode_ind(d_acc_high_msg);
        }
    }
    track_cust_work_mode_ind(w_acc_high_msg);
    tr_stop_timer(TRACK_CUST_ACCL_CHARGEOUT_CHECK_TIMER_ID);
#elif defined(__GT500__) ||defined(__ET200__)||defined(__ET130__)||defined(__NT23__)|| defined(__V20__) || defined(__ET310__) || defined(__MT200__)
    if(G_realtime_data.defense_mode == 0)
    {
        track_cust_status_defences_change((void*)0);
#if defined(__V20__)
        track_cust_433_control(7);
#endif /* __V20__ */
    }
#else
    track_cust_status_defences_change((void*)0);
#endif

    // ACC �������GPS�ر���򿪣����ϴ����µ�ACC״̬��������
    track_cust_gps_work_req((void *)4);

#if !defined(__XCWS__)
    track_os_intoTaskMsgQueueExt(heartbeat_packets, (void *)117);
#endif
    track_os_intoTaskMsgQueueExt(track_cust_module_sd_acc, (void *)4);
    track_cust_module_power_fails((void *)21);
    if(G_realtime_data.acc_last_status != 1)
    {
        track_os_intoTaskMsgQueue(track_cust_add_position);
        track_cust_led_acc_change_status(1);
#if defined(__FLY_MODE__)
        track_cust_flymode_set(1);
#endif /* __FLY_MODE__ */
#if defined(__ACC_ALARM__)
        if(G_parameter.accalm.accalm_sw == 1)
        {
            if(track_is_timer_run(TRACK_CUST_ALARM_ACC_TIMER))
            {
                track_stop_timer(TRACK_CUST_ALARM_ACC_TIMER);
            }
            track_start_timer(TRACK_CUST_ALARM_ACC_TIMER, G_parameter.accalm.accalm_time * 1000, track_cust_acc_change_alarm, (void*)1);
        }
#else
        if(G_parameter.accalm_sw == 1)
        {
            track_start_timer(TRACK_CUST_ALARM_ACC_TIMER, 1000, track_cust_alarm_acc_change, (void*)1);
        }
#endif
#if defined(__BCA__)
        if(G_parameter.PW5 == 3)
        {
            track_drv_gpio_pw5v(1);
        }
#endif /* __BCA__ */
    }
#if defined(__OBD__)
    if(G_parameter.obd.sw == 1)
    {
        track_obd_acc((void*)1);
    }
#endif /* __OBD__ */
}

/******************************************************************************
 *  Function    -  track_cust_status_defences_update_time
 *
 *  Purpose     -  ָ�����������ʱ��������ǰ������ʱ�У�����¶�ʱ����ʱ
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 15-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_status_defences_update_time(void)
{
    if(track_cust_status_acc() == 0 && defences <= 1)
    {
        track_cust_status_defences_change((void*)1);
        track_start_timer(
            TRACK_CUST_STATUS_DEFENCES_TIMER_ID,
            G_parameter.defense_time * 1000,
            track_cust_status_defences_change, (void*)2);
    }
}

/******************************************************************************
 *  Function    -  track_cust_status_defences
 *
 *  Purpose     -  �������״̬
 *
 *  Description -
 *      defences : 0 ����  1 Ԥ���  2 ���
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 21-05-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint8 track_cust_status_defences(void)
{
#if defined(__GT740__)||defined(__GT420D__)
    return 0;
#else
    LOGD(L_APP, L_V5, "defences = %d", defences);
    return defences;
#endif
}

/******************************************************************************
 *  Function    -  track_cust_status_defences_change
 *
 *  Purpose     -  �����״̬���иı�
 *
 *  Description -
 *      defences : 0 ����  1 Ԥ���  2 ���
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 10-01-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_status_defences_change(void *arg)
{
    LOGD(L_APP, L_V5, "defense_mode:%d, vibrates_alarm:%d;motor_lock:%d;arg=%d", G_realtime_data.defense_mode, G_parameter.vibrates_alarm.sw, G_parameter.motor_lock.sw, arg);
    if(G_parameter.vibrates_alarm.sw == 0 && G_parameter.motor_lock.sw == 0 && (kal_uint32)arg != 0)
    {
        return;
    }
    defences = (kal_uint32)arg;
    if(defences == 0 || defences == 2)
    {
        track_stop_timer(TRACK_CUST_STATUS_DEFENCES_TIMER_ID);
    }
#if defined (__GT300__)  ||defined(__GT03D__) || defined(__GW100__)||defined(__GT300S__)
    if(track_cust_get_work_mode() == WORK_MODE_1)
    {
        if(defences == 2)
        {
            track_cust_sensor_init();
        }
        else if(defences == 1)
        {
            track_drv_sensor_off();
        }
    }
    if(track_cust_led_sleep(99) == 1)
    {
        track_cust_led_defences(defences);
    }
#else
    track_cust_led_defences(defences);
#endif /* __GT300__ */
    track_cust_module_alarm_defences(defences);
#if defined(__V20__)
    if(defences == 0)
    {
        track_cust_433_control(7);
    }
    else if(defences == 2)
    {
        track_cust_433_control(3);
    }
#endif /* __V20__ */
}

/******************************************************************************
 *  Function    -  track_eint_status_check
 *
 *  Purpose     -  �ն�״̬�仯������
 *
 *  Description -  return Null
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 30-07-2012,  Liujw  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_status_eint_check_set(U8 eint, U8 level)
{
    kal_uint8 door_status;
    kal_uint8 buffer[50] = 0;
    kal_uint8 chr_in = 0xff;
    extern void track_cust_paket_94(kal_uint8 datatype, kal_uint8 * data, kal_uint16 len);
    extern  void track_cust_key_control_led(void);

    switch(eint)
    {
        case ACC_EINT_NO:
            if(track_is_timer_run(TRACK_CUST_ACC_INIT_OVERTIME_ID))
            {
                tr_stop_timer(TRACK_CUST_ACC_INIT_OVERTIME_ID);//ֹͣ����ʱ���õĶ�ʱ��
            }
            if(level == 0)
            {
                //track_start_timer(TRACK_CUST_STATUS_ACC_TIMER_ID, 100, acc_status_change, (void*)1);
#if defined(__GT420D__)
				track_drv_sleep_enable(SLEEP_OBD_MOD, FALSE);
				tr_start_timer(TRACK_CUST_WAKEUP_UART_TIMER_ID,10 * 1000, track_cust_modeule_into_sleep_mode);			
#else
                //track_start_timer(TRACK_CUST_STATUS_ACC_TIMER_ID, 100, acc_status_change, (void*)0);
                acc_status_change((void*)1);
#endif
            }
            else
            {
#if !defined(__GT420D__)
                acc_status_change((void*)0);
#endif
            }
            #if !defined(__GT420D__)
            LOGD(L_APP, L_V5, "ACC_EINT_NO level=%d", level);
            #endif
            break;
#if defined __USE_SPI__
        case EINT_SPI_NO:
            LOGD(L_APP, L_V5, "EINT_SPI_NO level=%d", level);
            track_spi_re(&buffer[0]);//����Ĳ���ûʹ�ã�����û����  chengj
            LOGD(L_APP, L_V5, "%s, strlen: %d", (char*)buffer, strlen(buffer));
            break;
#endif

#if defined (__NT33__) && defined (__CUST_BT__)
        case  EXTRA_A_EINT_NO://BT����GSM
            LOGD(L_APP, L_V5, "BT����GSM  level=%d", level);

            if(track_is_timer_run(TRACK_DRV_BT_WAKEUP_GSM_TIMER_ID))
            {
                tr_stop_timer(TRACK_DRV_BT_WAKEUP_GSM_TIMER_ID);
            }
            if(level == 1)
            {
                //BT����GSM
                track_drv_bt_wakeup_gsm();
            }
            else
            {
                track_drv_bt_and_gsm_sleep();
            }
            break;
#endif

#if defined (__SOD_ALM__)
        case  EXTRA_A_EINT_NO://�������
            LOGD(L_APP, L_V5, "������� %d", level);

            //track_cust_module_sod(level);
            break;
#endif

#if 0
            defined(__ET350__) && defined(__CUST_RS485__)
        case EINT_EDGE_NO://RS485���軽���豸��5����������ߣ�ͬʱ�ټ����ж�
            {
                chr_in = track_cust_status_charger();
                LOGD(L_APP, L_V2, "RS485���軽�� chr=%d", chr_in);

                if(chr_in == 0)
                {
                    LOGD(L_DRV, L_V2, "δ��������-����");
                }
                else
                {
                    if(track_is_timer_run(TRACK_DRV_RS485_RX_WAKEUP_TIMER_ID))
                    {
                        tr_stop_timer(TRACK_DRV_RS485_RX_WAKEUP_TIMER_ID);
                    }
                    track_drv_sleep_enable(SLEEP_EXT_UART_MOD, KAL_FALSE);
                    tr_start_timer(TRACK_DRV_RS485_RX_WAKEUP_TIMER_ID, 5 * 1000, track_drv_sleep_rs485_eint_enable);
                }
            }
            break;
#endif

#if defined (__GT370__)|| defined (__GT380__)
        case  EXTRA_A_EINT_NO://MCU����GSM
            LOGD(L_APP, L_V5, "MCU����GSM  level=%d", level);
            track_drv_mcu_and_gsm_sleep(level);
            break;

            //sos�ж� �����¼�
            /*
            #elif defined(__GT740__)
            		case  EXTRA_A_KEY_EINT_NO://MCU����GSM
                       if(level == 0)
                       {
            			   LOGD(L_APP, L_V5,"st����GSM  ");
            			   track_drv_sleep_enable(SLEEP_OBD_MOD, KAL_FALSE);
            			   tr_start_timer(TRACK_CUST_EXTCHIP_SLEEP_TIMER_ID, 2000, track_cust_modeule_into_sleep_mode);
            		   }
            		   break;*/
#elif defined(__UART1_SOS__) || (defined(__KEY_EINT__) && !defined(__GT02E__)  && !defined(__NT23__))||defined (__NT37__)
        case EXTRA_A_KEY_EINT_NO:
            LOGD(L_APP, L_V5, "EXTRA_A_KEY_EINT_NO level=%d", level);
#if defined(__ET130__) ||defined(__V20__)
            if(track_is_autotestmode())
            {
                track_autotest_soskey_status(EXTRA_A_KEY_EINT_NO, level);
                return;
            }
#elif defined(__GT310__)
            if(level == 0)
            {
                track_os_intoTaskMsgQueue(track_cust_key_sos);
                tr_start_timer(TRACK_CUST_SOS_KEY_TIMER_ID, 3000, track_cust_module_sos_trigger_button);
            }
            else
            {
                track_stop_timer(TRACK_CUST_SOS_KEY_TIMER_ID);
            }
#elif defined(__GT530__)
            if(level == 0 && track_cust_lte_systemup(0xff))
            {
                track_cust_lte_wakeup();
            }
            //track_drv_sleep_enable(SLEEP_DRV_MOD, (1 == level ? KAL_TRUE: KAL_FALSE));
            break;
#elif defined(__NT31__)
            if(level == 0)
                track_cust_key_control_led();
            break;
#endif
            if(level == 0)
            {
#if defined (__NETWORK_LICENSE__)
                tr_start_timer(TRACK_CUST_SOS_KEY_TIMER_ID, 3000, track_drv_network_license_eint_make_call);
#elif defined(__GT06D__) ||defined(__NT36__) || defined(__UART1_SOS__)||defined (__NT37__)
#if defined(__SPANISH__)
                if(G_parameter.sos_alarm.key_type == 0)
                {
                    static kal_uint32 tick = 0;
                    kal_uint32 tick2 = kal_get_systicks();
                    if(tick2 - tick >= KAL_TICKS_3_SEC)
                    {
                        track_cust_module_sos_trigger_button();
                        tick = tick2;
                    }
                }
                else
#endif/*__SPANISH__*/
                    tr_start_timer(TRACK_CUST_SOS_KEY_TIMER_ID, 3000, track_cust_module_sos_trigger_button);
#elif defined(__GW100__)//�ʹ���
                track_os_intoTaskMsgQueue(track_cust_key_short_press_sos);
                tr_start_timer(TRACK_CUST_SOS_KEY_TIMER_ID, 3000, track_cust_module_sos_trigger_button);
#endif /* __NETWORK_LICENSE__ */
            }
            else
            {
                track_stop_timer(TRACK_CUST_SOS_KEY_TIMER_ID);
                //GT03D�ߵ�ƽ��������
#if defined( __GT03F__)
                //         track_os_intoTaskMsgQueue(track_cust_key_sos);
#endif /* __GT03F__ */
            }
            break;

        case EXTRA_B_KEY_EINT_NO:
            LOGD(L_APP, L_V5, "EXTRA_B_KEY_EINT_NO level=%d", level);
            //track_autotest_soskey_status(level);
#if defined(__GT530__)
            track_drv_lte_dect_wdt_eint(level);
            break;
#endif /* __GT530__ */

            if(level == 0)
            {
                LOGS("==EXTRA_B_KEY_EINT_NO LOW  %d==", level);
                track_cust_key_control_led();
            }
            else
            {
                LOGS("==EXTRA_B_KEY_EINT_NO HIGH  %d==", level);
            }
            break;

#if defined(__NT36__)
        case  EXTRA_A_EINT_NO  ://�ⲿ�����жϣ���������û˵������
            LOGD(L_APP, L_V5, "EINT=%d  level=%d", EXTRA_A_EINT_NO, level);
            break;
#endif

#if defined(__GT06F__) || defined(__GT800__)||defined (__NT37__)
        case  EXTRA_A_EINT_NO  ://�ⲿ����,���ż��       	level��ʵ�ʽ���ĵ�ƽ�෴
            LOGD(L_APP, L_V5, "Door %d,%d;%d,%d,%d", eint, level, G_parameter.incout, G_parameter.door_alarm.door_status, G_parameter.door_alarm.sw);
#if defined (__NT37__)
            if(G_parameter.incout == 1)
            {
                if(level == 0)  //��⵽�ߵ�ƽ���룬���Ƶ����
                {
                    track_start_pluse_output();
                }
                break;
            }

#endif /* __NT37__ */

            if(G_parameter.door_alarm.door_status == 0)
            {
                if(level == 0)
                {
#if defined (__NT37__)
                    track_cust_door_alarm_enable(1);
#endif

                    door_status = 0;
                    track_cust_paket_94(0x5, &door_status, 1);
                }
                else
                {

                    door_status = 1;
                    track_cust_paket_94(0x5, &door_status, 1);
                    if(G_parameter.door_alarm.sw && track_cust_status_defences() == 2)
                    {
                        track_cust_alarm_door();

#if defined (__NT37__)
                        track_cust_door_alarm_enable(0);
#endif
                    }
                }

            }
            else if(G_parameter.door_alarm.door_status == 1)
            {
                if(level == 0)
                {
                    door_status = 3;
                    track_cust_paket_94(0x5, &door_status, 1);
                    if(G_parameter.door_alarm.sw && track_cust_status_defences() == 2)
                    {
                        track_cust_alarm_door();
#if defined (__NT37__)
                        track_cust_door_alarm_enable(0);
#endif
                    }
                }
                else
                {
#if defined (__NT37__)
                    track_cust_door_alarm_enable(1);
#endif

                    door_status = 2;
                    track_cust_paket_94(0x5, &door_status, 1);
                }
            }
            if(level == 0)
            {
                LOGS("==EXTRA_A_EINT_NO LOW  EINT=%d", level);
            }
            else
            {
                LOGS("==EXTRA_A_EINT_NO HIGH  EINT=%d", level);
            }
            break;
#endif


#if defined(__GT06F__)|| defined(__GT800__)||defined(__NT31__)
        case  AUX_EINT_NO  ://��������:
            if(level == 0)
            {
                //�ж�������
                LOGS("==AUX_EINT_NO LOW  EINT=%d", level);
                track_drv_sys_l4cuem_set_audio_mode(1);//�л�������
            }
            else
            {
                LOGS("==AUX_EINT_NO HIGH  EINT=%d", level);
                track_drv_sys_l4cuem_set_audio_mode(2);//�л�������
            }
            break;
#endif

#endif /* __KEY_EINT__ */

        default:
            LOGD(L_APP, L_V5, "eint=%d, level=%d", eint, level);
            break;
    }
}


#if defined (__NT33__)
/******************************************************************************
 *  Function    -  track_cust_no_acc_defences_init
 *
 *  Purpose     -  ��ACC״̬�¼�����������������ó�ʼ��
 *
 *  Description -  ��ACC�汾
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-11-18,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_no_acc_defences_init(void)
{
    if(G_realtime_data.defences_status >= 2 && !track_is_timer_run(TRACK_CUST_STATUS_DEFENCES_TIMER_ID))
    {
        track_os_intoTaskMsgQueueExt(track_cust_status_defences_change, (void *)2);
    }
}

#endif /* __NT33__ */

/******************************************************************************
 *  Function    -  track_cust_status_chargestatus_vbtstatus_check
 *
 *  Purpose     -  ������״̬����
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_status_chargestatus_vbtstatus_check(pmic_status_enum status, battery_level_enum level, kal_uint32 volt)
{
#if defined (__BAT_TEMP_TO_AMS__)
    static kal_uint8 bat_tmp = 0;
    if(bat_tmp == 0)
    {
        bat_tmp = 1;
        track_cust_bat_temperatrue_data();
    }
#endif /* __BAT_TEMP_TO_AMS__ */

    if(level == 99)
    {
        LOGD(L_APP, L_V8, "status=%d, level=%d, volt=%d", status, level, volt);
    }
    else if(status == PMIC_VBAT_STATUS)
    {
        LOGD(L_APP, L_V6, "status=%d, level=%d, volt=%d", status, level, volt);
    }
    else
    {
        LOGD(L_APP, L_V6, "status=%d", status);
    }
    switch(status)
    {
        // ���״̬
        case PMIC_USB_NO_CHARGER_IN:
        case PMIC_USB_CHARGER_IN:
            LOGD(L_APP, L_V6, "PMIC_USB_CHARGER_IN");
        case PMIC_CHARGER_IN:
            LOGD(L_APP, L_V5, "CHARGER_IN %d", status);
            chargerStatus = 1;
            //track_log_switch(0x00);
#if defined(__GT03F__)||defined(__GT310__) || defined(__GW100__)||defined(__GT300S__)
            if(track_cust_led_sleep(99) == 0)
            {
                track_cust_led_sleep(2);
            }
#endif
#if defined(__NT31__) || defined(__ET310__) ||defined(__MT200__) ||defined(__NT36__)//||defined (__ET320__)
            track_cust_led_sleep(3);
            tr_start_timer(TRACK_CUST_LED_START_TIMER, 30 * 60000, track_cust_led_first_start);
#endif /* __GT02__ */

#if defined (__NT33__)
            track_cust_led_sleep(3);
            tr_start_timer(TRACK_CUST_LED_START_TIMER, 5 * 60000, track_cust_led_first_start);
#endif

#if defined(__XCWS__)
            LED_PWR_StatusSet(PW_ON_STATE);
#elif defined(__GT310__)
            led_level_state_switch();
#else
            //LOGD(L_APP, L_V4, "�����������־����֤�����ɹ�");
            LED_PWR_StatusSet(PW_CHARGING_STATE);
#endif
#if defined(__XCWS__)
            track_cust_work_mode_ind(w_charge_in_msg);
#endif
#if defined (__ET200__)||defined(__ET130__)|| defined(__V20__) || defined(__ET310__) //|| defined(__MT200__)//||defined (__ET320__)
            track_cust_led_alarm();
#endif /* __ET200__ */
#if !defined(__EXT_VBAT_Protection__)
            track_cust_module_power_fails((void *)6);
#endif /* __EXT_VBAT_Protection__ */
#if defined(__INDIA__) && defined(__NT31__)
            if(G_parameter.pwonalm_flag == 1)
            {
                track_cust_power_in_alarm();
            }
            else
            {
                LOGD(L_APP, L_V4, "δ���ӵ籨����Ĭʱ��!");
            }
#endif
#if defined( __OLED__)
            track_cust_lcd_updata_charge(1);
            if(/*CHRPWRON*/1 != mmi_Get_PowerOn_Type())
            {
                track_cust_led_wake();
            }
#endif /* __OLED__ */

#if defined(__GT370__) || defined(__GT380__)
            heartbeat_packets((void *)110);
#endif
            break;

        //���γ��¼�
        case PMIC_USB_NO_CHARGER_OUT:
        case PMIC_USB_CHARGER_OUT:
        case PMIC_CHARGER_OUT:
            LOGD(L_APP, L_V5, "CHARGER_OUT %d", status);
            chargerStatus = 0;
#if defined(__GT03F__)||defined(__GT310__) || defined(__GW100__)||defined(__GT300S__)
            if(track_cust_led_sleep(99) == 0 || G_realtime_data.key_status == 0 || track_cust_led_sleep(99) == 2)
            {
                track_cust_led_sleep(2);
            }
#endif
#if defined(__XCWS__)
            LED_PWR_StatusSet(PW_OFF_STATE);
#else
            LED_PWR_StatusSet(PW_NORMAL_STATE);
#endif
#if defined (__ET200__)||defined(__ET130__)|| defined(__V20__) || defined(__ET310__)// || defined(__MT200__)//||defined (__ET320__)
            track_cust_led_alarm();
#endif /* __ET200__ */
#if !defined(__EXT_VBAT_Protection__)
            track_cust_module_power_fails((void *)1);
#endif /* __EXT_VBAT_Protection__ */
            if(G_parameter.motor_lock.sw == 1)
            {
                track_cust_motor_lock(0);  // �����������Ҫ�������ǰ�ᣬΪ�˱������Ͽ�����ACC���
            }
#if defined(__XCWS__)
            track_cust_work_mode_ind(w_charge_out_msg);
#endif

#if defined( __OLED__)
            track_cust_lcd_updata_charge(0);
#endif /* __OLED__ */

#if defined(__GT370__) || defined(__GT380__)
            heartbeat_packets((void *)110);
#endif

            break;

        //������״̬
        case PMIC_CHARGE_COMPLETE:
            chargerStatus = 2;
#if defined(__GT310__)
            led_level_state_switch();
#else
            LED_PWR_StatusSet(PW_FULLCHARGED_STATE);
#endif
            //track_pmic_charger_completed_status();
#if defined( __OLED__)
            track_cust_lcd_updata_charge(3);
#endif /* __OLED__ */
            break;

        case PMIC_VBAT_STATUS:
            {
                static U32 last_volt = 0;
                static int low_count = 0;
                static int power_down_count = 0;

                battery_level = level;
#if !defined(__GT740__)||!defined(__GT420D__)
                //��ȡ��ѹֵ
                if(level != 99)
                {
                    if(last_volt == 0 || OTA_abs(last_volt - volt) < 100000)
                    {
                        g_volt = volt;
#if defined(__FLY_MODE__)
                        if(g_volt < 3590000)
                        {
                            g_islowbat = 1;
                            track_cust_flymode_set(193);
                        }
                        else if(g_volt > 3610000) g_islowbat = 0;
#endif /* __FLY_MODE__ */
                    }
                    last_volt = volt;
                }
#endif /* __GT740__ */

#if defined(__XCWS__)
                LOGD(L_APP, L_V7, "threshold=%d, volt=%d,%d,%d,%d", G_parameter.low_power_alarm.threshold, volt, chargerStatus, low_count, g_volt);
                if(chargerStatus == 0 && g_volt != 0 && g_volt < G_parameter.low_power_alarm.threshold && G_realtime_data.lowBattery == 0)
                {
                    if(low_count > 3)
                    {
                        low_count = 0;
                        track_cust_module_low_power(KAL_TRUE);
                    }
                    else
                    {
                        low_count++;
                    }
                }

                if(g_volt > (G_parameter.low_power_alarm.threshold + 100000) && G_realtime_data.lowBattery == 1)
                {
                    track_cust_module_low_power(KAL_FALSE);
                    if(low_count != 0) low_count = 0;
                }
#endif
                switch(level)
                {
                    case BATTERY_LOW_POWEROFF:      // �͵�ػ��¼�
                    case BATTERY_LOW_TX_PROHIBIT:   // �͵�ر�GSM
                    case BATTERY_LOW_WARNING:       // �͵羯��״̬ NVRAM_EF_CUST_HW_LEVEL_TBL_DEFAULT[]
#if defined(__XCWS__)
                        break;
#endif
                        if(chargerStatus == 0 && volt != 0 && volt < LOWER_BATTERY_LEVEL)
                        {
                            if(g_GT03D && volt < 3550000)
                            {
                                power_down_count++;

#if defined (__GT370__) || defined (__GT380__)
                                if((track_cust_get_work_mode() == 3) || (power_down_count > 2)) //�������ģʽ2������������ʱ��̣����ٴ���
                                {
                                    track_cust_module_power_off_ready();
                                }

#else
                                if(power_down_count > 2)
                                {
                                    track_cust_module_power_off_ready();
                                }
#endif
                            }
#if defined(__GT740__)||defined(__GT420D__)
//nc
#elif defined (__GT370__) || defined (__GT380__)
                            if(volt < LOWER_BATTERY_LEVEL)
                            {
                                low_count++;

                                LOGD(L_APP, L_V5, "LOWER_BATTERY_LEVEL %d", low_count);
                                if((track_cust_get_work_mode() == 3) || (low_count > 2)) //�������ģʽ2������������ʱ��̣����ٴ���
                                {
                                    LED_PWR_StatusSet(PW_LOW_ALARM_STATE);
                                    track_cust_module_low_power(KAL_TRUE);
                                }
                            }

#else
                            if(volt < LOWER_BATTERY_LEVEL)
                            {
                                low_count++;
                                if(low_count > 2)
                                {
#if defined(__GT310__)
                                    led_level_state_switch();
#else
                                    LED_PWR_StatusSet(PW_LOW_ALARM_STATE);
#endif
                                    track_cust_module_low_power(KAL_TRUE);
#if defined( __OLED__)
                                    track_cust_lcd_updata_charge(4);
#endif /* __OLED__ */
                                }
                            }
#endif /* __GT740__ */
                        }
                        else
                        {
                            low_count = 0;
                            power_down_count = 0;
                        }
                        break;

                    //��������״̬
                    case BATTERY_LEVEL_0:
                    case BATTERY_LEVEL_1:
                    case BATTERY_LEVEL_2:
                    case BATTERY_LEVEL_3:
#if defined(__XCWS__)
                        if(chargerStatus == 0 && volt != 0 && (red_led_status != PW_NORMAL_STATE))
                        {

                            LED_PWR_StatusSet(PW_NORMAL_STATE);
                        }
                        break;
#endif
#if !defined(__GT740__)||defined(__GT420D__)
                        if(volt >= 3800000) track_cust_module_low_power(KAL_FALSE);
#endif /* __GT740__ */
#if defined(__GT03F__)
                        if(chargerStatus == 0 && volt != 0 && (red_led_status != PW_NORMAL_STATE))
#else
                        if(chargerStatus == 0 && volt != 0)
#endif
                        {
                            LED_PWR_StatusSet(PW_NORMAL_STATE);
                        }
                        low_count = 0;
#if defined(__OLED__)
                        track_cust_volt_init(volt);
#endif /* __OLED__ */
                        break;

                    case 99: /* �ⲿ��ѹ��� */
                        g_ext_volt = volt;

                        track_cust_module_ext_vbat(volt);

#if defined (__GT370__) || defined (__GT380__)
//nc
#elif defined (__EXT_VBAT_ADC__) && !(defined (__NT31__)||defined(__NT36__))
                        track_cust_module_ext_voltage_values(volt);
#endif /* __EXT_VBAT_ADC__ */
#if defined(__OBD__)
                        if(G_parameter.obd.sw == 1)
                            track_cust_module_ext_vbat_to_obd(volt);
#endif
                        break;

                    default:
                        LOGD(L_CON, L_V1, "Battery level error");
                        break;
                }
                break;
            }

        case PMIC_INVALID_BATTERY:
        case PMIC_OVERBATTEMP:
        case PMIC_OVERVOLPROTECT:
        case PMIC_OVERCHARGECURRENT:
        case PMIC_LOWBATTEMP:
        case PMIC_CHARGING_TIMEOUT:
        case PMIC_INVALID_CHARGER:
        case PMIC_LOWCHARGECURRENT:
        case PMIC_CHARGE_BAD_CONTACT:
        case PMIC_BATTERY_BAD_CONTACT:
            //case PMIC_USB_NO_CHARGER_IN:
            break;

        default:
            break;
    }

}

/******************************************************************************
 *  Function    -  track_cust_status_acc
 *
 *  Purpose     -  ����ACC״̬
 *
 *  Description -   ���=1   Ϩ��=0
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * v1.0  , 29-07-2013,  jwj  written
 * ----------------------------------------
*******************************************************************************/
kal_uint8 track_cust_status_acc(void)
{
#if defined(__NO_ACC__)
    return 0;
#endif
    return acc;
}

/******************************************************************************
 *  Function    -  track_cust_status_charger
 *
 *  Purpose     -  ���״̬��ѯ
 *
 *  Description -        ����
 *    chargerStatus = 0  ���Ͽ�
 *                    1  �����
 *                    2  �����
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 18-06-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
/*track_cust_status_charger()��chargerStatus �ڲ���ģʽ������*/
/*track_drv_bmt_get_charging_status �����������״̬*/
/*Ҳ���ǲ���ģʽ�£�������ѯ���״̬�ĺ����������ͬ��ɾ��һ����?*/
/*chengjun  2017-03-24*/
kal_uint8 track_cust_status_charger()
{
    LOGD(L_APP, L_V6, "chargerStatus=%d", chargerStatus);
    return chargerStatus;
}

/******************************************************************************
 *  Function    -  track_cust_status_sim_set
 *
 *  Purpose     -  ����GSM����״̬��������LED����ʾ
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_status_sim_set(kal_int8 value)
{
    if(value == 1)
    {
        LED_GSM_StatusSet(GSM_NORMAL_STATE);
    }
    else
    {
        LED_GSM_StatusSet(GSM_OFF_STATE);
    }
}

/******************************************************************************
 *  Function    -  track_cust_status_sim
 *
 *  Purpose     -  ����SIM��״̬ �Ƿ���ڱ�׼SIM��
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v2.0  , 2017-03-06,  chengjun  written
 * ----------------------------------------
*******************************************************************************/
kal_uint8 track_cust_status_sim()
{
    kal_uint8 is_normal_sim;
    is_normal_sim = track_drv_get_sim_type(); //�ú����ӿ��Զ������״̬
    if(is_normal_sim == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/******************************************************************************
 *  Function    -  track_cust_get_battery_level
 *
 *  Purpose     -  ��ص�ѹ�ȼ�����
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint8 track_cust_get_battery_level(void)
{
#if defined(__GT740__)||defined(__GT420D__)
    U32 v_level[] = {2200000, 2300000, 2500000, 2700000, 3000000, 3200000};
#else
    U32 v_level[] = {3450000, 3550000, 3650000, 3700000, 3800000, 3900000};
#endif /* __GT740__ */
    if(g_volt <= v_level[0])
    {
        return 0;
    }
    else if(g_volt <= v_level[1])
    {
        return 1;
    }
    else if(g_volt <= v_level[2])
    {
        return 2;
    }
    else if(g_volt <= v_level[3])
    {
        return 3;
    }
    else if(g_volt <= v_level[4])
    {
        return 4;
    }
    else if(g_volt <= v_level[5])
    {
        return 5;
    }
    else
    {
        return 6;
    }
}

/******************************************************************************
 *  Function    -  track_cust_conversion_volt
 *
 *  Purpose     -  �����������ص�ѹ
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 16-03-2016,  xzq  written
 * ----------------------------------------
*******************************************************************************/
kal_uint32 track_cust_conversion_volt(U8 ele)
{
#if defined(__GT740__)||!defined(__GT420D__)
    U32 v_level[][2] = {{100, 2930000}, {90, 2880000}, {80, 2820000}, {70, 2770000}, {60, 2700000}, {50, 2660000}, {40, 2600000}, {30, 2550000}, {20, 2400000}, {10, 2300000}, {0, 2200000}};
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
    if(ele > 100)g_volt = 2930000;
    else
    {
        g_volt = v_level[10 - ((ele + 9) / 10)][1];
    }
#else
    if(ele <= v_level[10][0])
    {
        g_volt = v_level[10][1];
    }
    else if(ele <= v_level[9][0])
    {
        g_volt = v_level[9][1];
    }
    else if(ele <= v_level[8][0])
    {
        g_volt = v_level[8][1];
    }
    else if(ele <= v_level[7][0])
    {
        g_volt = v_level[7][1];
    }
    else if(ele <= v_level[6][0])
    {
        g_volt = v_level[6][1];
    }
    else if(ele <= v_level[5][0])
    {
        g_volt = v_level[5][1];
    }
    else if(ele <= v_level[4][0])
    {
        g_volt = v_level[4][1];
    }
    else if(ele <= v_level[3][0])
    {
        g_volt = v_level[3][1];
    }
    else if(ele <= v_level[2][0])
    {
        g_volt = v_level[2][1];
    }
    else if(ele <= v_level[1][0])
    {
        g_volt = v_level[1][1];
    }
    else if(ele <= v_level[0][0])
    {
        g_volt = v_level[0][1];
    }
    else
    {
        g_volt = 2930000;
    }
#endif//__MCU_LONG_STANDBY_PROTOCOL__
    LOGD(L_OBD, L_V5, "volt:%d", g_volt);
    return g_volt;
#endif /* __GT740__ */

}

#if defined(__GT740__)||defined(__GT420D__)
void track_cust_set_real_battery_volt(U32 v)
{
    g_real_volt = v * 10000;
}
kal_uint32 track_cust_get_real_battery_volt(void)
{
    LOGD(L_APP, L_V7, "volt:%d", g_real_volt);
	
    return g_real_volt;
}
#endif

/******************************************************************************
 *  Function    -  track_cust_get_battery_volt
 *
 *  Purpose     -  ���ص�ص�ѹ
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint32 track_cust_get_battery_volt(void)
{
#if defined(__JM66__)
    return track_cust_get_tbt100_vbat();
#else
    LOGD(L_APP, L_V7, "volt:%d", g_volt);
    return g_volt;
#endif//__JM66__
}

kal_uint32 track_cust_set_battery_volt(U32 v)
{
    static U8 low_count = 0, off_count = 0, mode2_open = 1;
    static U32 low_power_tick = 0;
    g_volt = v * 10000;
    LOGD(L_APP, L_V5, "v:%d, volt:%d,tick %d", v, g_volt, low_power_tick);
#if defined(__GT740__)||defined(__GT420D__)
    if(low_power_tick != 0)
    {
        if(OTA_kal_get_systicks() - low_power_tick >= (KAL_TICKS_1_MIN * 58))
        {
            low_power_tick = OTA_kal_get_systicks();
            track_cust_module_low_power(KAL_FALSE);
        }
    }
    if(g_volt <= 2400000)
    {
        low_count++;
        if(G_realtime_data.lowBattery == 1  && mode2_open)
        {
            G_realtime_data.lowBattery = 0;
        }
        mode2_open = 0;
        if(low_count >= 1)
        {
            LED_PWR_StatusSet(PW_LOW_ALARM_STATE);
            track_cust_module_low_power(KAL_TRUE);
        }
        if(low_power_tick == 0)
        {
            low_power_tick = OTA_kal_get_systicks();
        }
    }

#endif
    return g_volt;
}

/******************************************************************************
 *  Function    -  track_cust_get_work_mode
 *
 *  Purpose     -  �����ն˹���ģʽ
 *
 *  Description -
 *
 * modification history 1:��ʱ��λ����ģʽ;2:���ܶ�λģʽ;3:�������ģʽ
 * ----------------------------------------
 * v1.0  , 07-08-2013,  ZengPing && Liujw  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint8 track_cust_get_work_mode(void)
{
    return G_parameter.work_mode.mode;
}

void track_cust_into_sleep_mode_option()
{
    track_stop_timer(TRACK_CUST_LED_START_TIMER);
    track_cust_led_sleep(2);
    //����
}

kal_bool track_cust_is_working_mode4(void)
{
    LOGD(L_APP, L_V5, "Current work mode:%d", G_parameter.work_mode.mode);
    return (kal_bool)(G_parameter.work_mode.mode == WORK_MODE_4);
}

kal_bool track_cust_is_mute_mode(void)
{
    LOGD(L_APP, L_V5, "Current callmode:%d", G_parameter.callmode);
    return (kal_bool)(3 == G_parameter.callmode);
}
/******************************************************************************
 *  Function    -  track_cust_check_sos_incall
 *
 *  Purpose     -  �绰�����š����� ���� ���
 *
 *  Description -  0-----����1-----Ԥ���2-----���
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 02-08-2013,  ZengPing && Liujw  written
 * v1.1 ,  05-09-2013,  ZengPing && WangQi
 * v1.2 ,  09-17-2013,  jwj
 * v1.3 ,  30-10-2013,  ZengPing && Liujw  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_check_sos_incall(CMD_DATA_STRUCT * Cmds, kal_int8 state)
{
    struct_msg_send g_msg = {0};
    kal_bool isChar = KAL_TRUE;
    char *data;
    int len;

    /*��������ʹ�õ���ʱ�����������Ƶ�      --    chengjun  2017-03-31*/
    kal_uint8 string1[14] = {0x8F, 0xDC, 0x7A, 0x0B, 0x8B, 0xBE, 0x96, 0x32, 0x62, 0x10, 0x52, 0x9F, 0x00, 0x00};/*Զ������ɹ�!*/
    kal_uint8 string2[14] = {0x8F, 0xDC, 0x7A, 0x0B, 0x64, 0xA4, 0x96, 0x32, 0x62, 0x10, 0x52, 0x9F, 0x00, 0x00};/*Զ�̳����ɹ�!*/
    kal_uint8 string3[36] =
    {
        /*�ն˴���������ߣ������˳�����״̬��*/
        0x7E, 0xC8, 0x7A, 0xEF, 0x59, 0x04, 0x4E, 0x8E, 0x6D, 0xF1, 0x5E, 0xA6, 0x4F, 0x11, 0x77, 0x20,
        0xFF, 0x0C, 0x8B, 0xF7, 0x51, 0x48, 0x90, 0x00, 0x51, 0xFA, 0x4F, 0x11, 0x77, 0x20, 0x72, 0xB6,
        0x60, 0x01, 0xFF, 0x01
    };
    kal_uint8 string4[44] =
    {
        /*Զ��������ɹ����뽫�𶯱������ܴ򿪺����ԡ�*/
        0x8F, 0xDC, 0x7A, 0x0B, 0x8B, 0xBE, 0x96, 0x32, 0x4E, 0x0D, 0x62, 0x10, 0x52, 0x9F, 0xFF, 0x0C,
        0x8B, 0xF7, 0x5C, 0x06, 0x97, 0x07, 0x52, 0xA8, 0x62, 0xA5, 0x8B, 0x66, 0x52, 0x9F, 0x80, 0xFD,
        0x62, 0x53, 0x5F, 0x00, 0x54, 0x0E, 0x51, 0x8D, 0x8B, 0xD5, 0x30, 0x02
    };
    kal_uint8 string5[44] =
    {
        /*�������������𶯱������ܺ���ִ�д˲�����*/
        0x95, 0x19, 0x8B, 0xEF, 0xFF, 0x01, 0x8B, 0xF7, 0x57, 0x28, 0x54, 0x2F, 0x75, 0x28, 0x97, 0x07,
        0x52, 0xA8, 0x62, 0xA5, 0x8B, 0x66, 0x52, 0x9F, 0x80, 0xFD, 0x54, 0x0E, 0xFF, 0x0C, 0x51, 0x8D,
        0x62, 0x67, 0x88, 0x4C, 0x6B, 0x64, 0x64, 0xCD, 0x4F, 0x5C, 0x30, 0x02
    };
    kal_uint8 string6[36] =
    {
        /*�ն��Ѵ������״̬����ָ���ִ�У�*/
        0x7E, 0xC8, 0x7A, 0xEF, 0x5D, 0xF2, 0x59, 0x04, 0x4E, 0x8E, 0x8B, 0xBE, 0x96, 0x32, 0x72, 0xB6,
        0x60, 0x01, 0xFF, 0x0C, 0x67, 0x2C, 0x63, 0x07, 0x4E, 0xE4, 0x4E, 0x0D, 0x51, 0x8D, 0x62, 0x67,
        0x88, 0x4C, 0xFF, 0x01
    };
    kal_uint8 string7[36] =
    {
        /*�ն��Ѵ��ڳ���״̬����ָ���ִ�У�*/
        0x7E, 0xC8, 0x7A, 0xEF, 0x5D, 0xF2, 0x59, 0x04, 0x4E, 0x8E, 0x64, 0xA4, 0x96, 0x32, 0x72, 0xB6,
        0x60, 0x01, 0xFF, 0x0C, 0x67, 0x2C, 0x63, 0x07, 0x4E, 0xE4, 0x4E, 0x0D, 0x51, 0x8D, 0x62, 0x67,
        0x88, 0x4C, 0xFF, 0x01
    };

    kal_uint8 string8[36] =
    {
        /*�ն˴����Զ����ģʽ����ָ�ִ�С�*/
        0x7E, 0xC8, 0x7A, 0xEF, 0x59, 0x04, 0x4E, 0x8E, 0x81, 0xEA, 0x52, 0xA8, 0x8B, 0xBE, 0x96, 0x32,
        0x6A, 0x21, 0x5F, 0x0F, 0xFF, 0x0C, 0x67, 0x2C, 0x63, 0x07, 0x4E, 0xE4, 0x4E, 0x0D, 0x62, 0x67,
        0x88, 0x4C, 0xFF, 0x01
    };

    kal_uint8 str_message_cn[20] =
    {
        /*�������ܹرգ�������*/
        0x62, 0xA5, 0x8B, 0x66, 0x52, 0x9F, 0x80, 0xFD, 0x51, 0x73, 0x95, 0xED, 0xFF, 0x0C, 0x64, 0xA4,
        0x96, 0x32, 0xFF, 0x01
    };

    kal_uint8 string1_en[] = {"Remote fortification success!"};
    kal_uint8 string2_en[] = {"Remote disarm success!"};
    kal_uint8 string3_en[] = {"The terminal is in Deep Sleep, please wake up!"};
    kal_uint8 string4_en[] = {"The remote fortification unsuccessful, please vibration alarm function is turned on and try again."};
    kal_uint8 string5_en[] = {" Error! Please first enable vibration alarm function, and then perform the operation."};
    kal_uint8 string6_en[] = {"Already in the state of fortification, The command is not executed!"};
    kal_uint8 string7_en[] = {"Already in the state of disarm, The command is not executed!"};
    kal_uint8 string8_en[] = {"The terminal is in the automatic fortification mode, this command is not executed!"};
    kal_uint8 str_message_en[] = {"Vibration alarm is off, the device disarms!"};

    kal_uint8 status_defences = track_cust_status_defences();
    kal_uint8 work_mode = track_cust_get_work_mode();

    LOGD(L_APP, L_V5, "state =%d", state);
    //g_msg.addUrl = KAL_FALSE;
    //g_msg.cm_type = CM_SMS;
    if(state == 3)
    {
        snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, str_message_en);
        g_msg.msg_cn_len = 20;
        memcpy(g_msg.msg_cn, str_message_cn, g_msg.msg_cn_len);
    }
    else if(G_parameter.vibrates_alarm.sw == 0)
    {
#if defined (__GT370__)|| defined (__GT380__)
        snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, string4_en);
        g_msg.msg_cn_len = 44;
        memcpy(g_msg.msg_cn, string4, g_msg.msg_cn_len);
#else
        snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, string5_en);
        g_msg.msg_cn_len = 44;
        memcpy(g_msg.msg_cn, string5, g_msg.msg_cn_len);
#endif /* __GT370__ */
    }
#if defined(__GT500__) ||defined(__ET200__)||defined(__ET130__)||defined(__NT23__)|| defined(__V20__) || defined(__ET310__)\
     || defined(__MT200__)||defined (__ET320__)
    else if(G_realtime_data.defense_mode == 0)
    {
        snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, string8_en);
        g_msg.msg_cn_len = 36;
        memcpy(g_msg.msg_cn, string8, g_msg.msg_cn_len);
    }
#endif
    else
    {
#if defined (__GT300__)  ||defined(__GT03F__) || defined(__GW100__)||defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
        if(work_mode == 3)
        {
            snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, string3_en);
            g_msg.msg_cn_len = 36;
            memcpy(g_msg.msg_cn, string3, g_msg.msg_cn_len);
            // track_cust_module_alarm_msg_send(&g_msg, NULL);
            //    return;
        }
        else if(state == 2 && work_mode != 3)
        {
            if(status_defences == 2)
            {
                snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, string6_en);
                g_msg.msg_cn_len = 36;
                memcpy(g_msg.msg_cn, string6, g_msg.msg_cn_len);
            }
            else
            {
                if(work_mode == 1)
                {
                    track_cust_sensor_init();
                }
                track_stop_timer(TRACK_CUST_MODULE_ALARM_VIBRATES_TIMER_ID);
                track_cust_status_defences_change((void*)2);
                if(track_cust_status_defences() == 2)
                {
                    snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, string1_en);
                    g_msg.msg_cn_len = 14;
                    memcpy(g_msg.msg_cn, string1, g_msg.msg_cn_len);
                }
                else
                {
                    snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, string4_en);
                    g_msg.msg_cn_len = 44;
                    memcpy(g_msg.msg_cn, string4, g_msg.msg_cn_len);
                }
            }
            //    track_cust_module_alarm_msg_send(&g_msg, NULL);
        }
        else if(state == 0 && status_defences == 0)
        {
            snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, string7_en);
            g_msg.msg_cn_len = 36;
            memcpy(g_msg.msg_cn, string7, g_msg.msg_cn_len);
        }
        else if(state == 0 && work_mode != 3)
        {
#if defined(__GT370__)|| defined (__GT380__)
            //nc
#else
            if(work_mode == 1)
            {
                track_drv_sensor_off();
            }
#endif
            track_cust_status_defences_change((void*)0);
            snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, string2_en);
            g_msg.msg_cn_len = 14;
            memcpy(g_msg.msg_cn, string2, g_msg.msg_cn_len);
            //    track_cust_module_alarm_msg_send(&g_msg, NULL);
        }
#else
        if(state == 2 && status_defences == 2)
        {
            snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, string6_en);
            g_msg.msg_cn_len = 36;
            memcpy(g_msg.msg_cn, string6, g_msg.msg_cn_len);
        }
        else if(state == 2)
        {
            track_stop_timer(TRACK_CUST_MODULE_ALARM_VIBRATES_TIMER_ID);
            track_cust_status_defences_change((void*)2);
#if defined(__V20__)
            track_cust_433_control(3);
#endif /* __V20__ */
            if(track_cust_status_defences() == 2)
            {
                snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, string1_en);
                g_msg.msg_cn_len = 14;
                memcpy(g_msg.msg_cn, string1, g_msg.msg_cn_len);

            }
            else
            {
                snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, string4_en);
                g_msg.msg_cn_len = 44;
                memcpy(g_msg.msg_cn, string4, g_msg.msg_cn_len);
            }
            //    track_cust_module_alarm_msg_send(&g_msg, NULL);
        }
        else if(state == 0 && status_defences == 0)
        {
            snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, string7_en);
            g_msg.msg_cn_len = 36;
            memcpy(g_msg.msg_cn, string7, g_msg.msg_cn_len);
        }
        else if(state == 0)
        {
            track_cust_status_defences_change((void*)0);
            snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, string2_en);
            g_msg.msg_cn_len = 14;
            memcpy(g_msg.msg_cn, string2, g_msg.msg_cn_len);
            //    track_cust_module_alarm_msg_send(&g_msg, NULL);
#if defined(__V20__)
            track_cust_433_control(7);
#endif /* __V20__ */
        }
#endif /* __GT300__ */
    }
    if(G_parameter.lang == 1)
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
    if(g_msg.msg_cn_len)
    {
        /*  ָ�����ִ����ֻ�����Ϣ��ȫ���ڴ˷���*/
        if(Cmds->return_sms.cm_type == CM_SMS)
        {
            //track_cmd_sms_rsp(cmd);
            track_cust_sms_send(Cmds->return_sms.num_type, Cmds->return_sms.ph_num, isChar, data, len);
        }
        else if(Cmds->return_sms.cm_type == CM_ONLINE)
        {
            track_cust_paket_msg_upload(Cmds->return_sms.stamp, isChar, data, len);
        }
        else if(Cmds->return_sms.cm_type == CM_ONLINE2)
        {
            track_cust_paket_FD_cmd(Cmds->return_sms.stamp, isChar, data, len);
        }

        LOGS("%s", g_msg.msg_en);
        //LOGS("%s", Cmds->rsp_msg);
    }
}
#if defined(__XCWS__)
kal_uint8 track_get_gsm_signal_level(void)
{
    kal_uint8 level;
    level = track_drv_get_service_cell_level();

    if(level == 0xFF || level <= 0)
    {
        return 0;
    }
    else if(level >= 52)
    {
        return 5;
    }
    else
    {
        return (kal_uint8)(level / 13) + 1;
    }
}
/******************************************************************************
 *  Function    -  track_get_terminal_status
 *
 *  Purpose     -  ��ȡ�ն�״̬
 *
 *  Description -  1-�����2-������255-�ϵ磻511-����
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 23-08-2012,  Liujw  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint16 track_get_terminal_status(void)
{
    /*�����ƶ�ר��*/
    kal_uint16 state = 0;

    track_gps_data_struct *gpsdata = track_drv_get_gps_data();
    if(track_cust_get_work_mode_cq() == SLEEP_MODE)
    {
        state |= 0x01;
    }
    if(track_cust_get_defense_mode() == FORTIFACATION_MODE)
    {
        state |= 0x02;
    }
    if((track_cust_status_acc() != 1) && ((gpsdata->status >= 2 && gpsdata->gprmc.Speed < 3) || track_cust_gps_status() == 0))
    {
        state |= 0x04;
    }
    if(track_cust_status_charger() == 0)
    {
        state |= 0x08;
    }
    return state;
}
#endif
/******************************************************************************
 *  Function    -  track_recv_cmd_status
 *  Purpose     -  ��ȡ�ն�״̬
 *  return -  0-��ģʽ3��ʱ�ϴ�ģʽ��1-ģʽ3��ʱ�ϴ�ģʽ��
 * modification history
 * ----------------------------------------
 * v1.0  , 20141205,  WQ  written
 * ----------------------------------------
 ******************************************************************************/
kal_bool track_recv_cmd_status(kal_uint8 par)
{
    static kal_bool cmd = KAL_FALSE;
    if(par == 1)
    {
        cmd = KAL_TRUE;
    }
    return cmd;
}


kal_uint8 track_cust_mode3_uping(kal_uint8 option)
{
    static kal_uint8 op = 0;
    if(0 == option || 1 == option)
    {
        op = option;
    }
    return op;
}

kal_bool track_cust_lte_ready(kal_uint8 par)
{
    static kal_bool ready = KAL_FALSE;
    if(par == 1 || par == 0)
    {
        ready = par;
    }

    return ready;
}

kal_uint32 track_cust_get_ext_volt(void)
{
    /*12450000 */
    return g_ext_volt;
}

#if defined(__FLY_MODE__)
kal_bool track_cust_isbatlow(void)
{
    return g_islowbat;
}
#endif /* __FLY_MODE__ */

