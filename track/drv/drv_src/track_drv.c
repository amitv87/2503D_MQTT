/******************************************************************************
 * track_drv.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        ���������������ʵ�Ψһ�ӿڼ� �����ϲ�ע��ʵ��
 *
 * modification history
 * --------------------
 * v1.0   2012-07-26,  chengjun create this file
 *
 ******************************************************************************/

/****************************************************************************
* Include Files             ����ͷ�ļ�
*****************************************************************************/
#include "track_drv.h"
#include "track_cust_main.h"

/*****************************************************************************
 *  Define			    �궨��
*****************************************************************************/


/*****************************************************************************
* Typedef  Enum         ö��
*****************************************************************************/


/*****************************************************************************
 *  Struct			    ���ݽṹ����
*****************************************************************************/


/*****************************************************************************
* Local variable            �ֲ�����
*****************************************************************************/


/****************************************************************************
* Global Variable           ȫ�ֱ���
*****************************************************************************/


/****************************************************************************
* Global Variable - Extern          ����ȫ�ֱ���
*****************************************************************************/


/*****************************************************************************
 *  Global Functions	- Extern	    �����ⲿ����
*****************************************************************************/
extern void track_query_sim_type(kal_int8 sim_type);
/*****************************************************************************
 *  Local Functions	            ���غ���
*****************************************************************************/


/*****************************************************************************
 *  Global Functions            ȫ�ֺ���
*****************************************************************************/
#if defined(__JM66__)
#define CMMCLK 29
int track_drv_PA_control(kal_int8 set)//�͵�ƽ��Ч
{
    char status;
    status = track_GPIO_ReadIO(CMMCLK);
    LOGD(L_DRV, L_V5, "PA_CURRENT1:%d", status);
    if(set == 0 || set == 1)
    {
        if(status != set)
        {
            int i;
            GPIO_ModeSetup(CMMCLK, 1);
            GPIO_InitIO(1, CMMCLK);
            for(i = 0; i < 3; i++)
            {
                GPIO_WriteIO(set, CMMCLK);
                status = track_GPIO_ReadIO(CMMCLK);
                LOGD(L_DRV, L_V5, "PA_CURRENT2:%d", status);
                if(status == set) return status;
            }
            if(status != set) return -1;
        }
    }
    return status;
}
#endif//__JM66__
/******************************************************************************
 *  Function    -  track_drv_gps_decode_done
 * 
 *  Purpose     -  һ������nmea ���ݰ��������
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 03-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_gps_decode_done(track_gps_data_struct *track_gps_data)
{
    if(track_gps_data->gw_mode == BD)
    {
        memcpy(&track_gps_data->gprmc, &track_gps_data->bdrmc, sizeof(track_gps_data->gprmc));
        memcpy(&track_gps_data->gpgga, &track_gps_data->bdgga, sizeof(track_gps_data->gpgga));
        memcpy(&track_gps_data->gpgsa, &track_gps_data->bdgsa, sizeof(track_gps_data->gpgsa));
        memcpy(&track_gps_data->gpgsv, &track_gps_data->bdgsv, sizeof(track_gps_data->gpgsv));
    }
    track_cust_gps_decode_done(track_gps_data);
}

/******************************************************************************
 *  Function    -  track_drv_gps_decode_ttff
 * 
 *  Purpose     -  ����GPS �򿪣��״ζ�λʱ�� ��TTFF��
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 04-03-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_gps_decode_ttff(void)
{
#if defined (__AUTO_TEST__)
    track_factoty_autotest_gpscold_ttff();
#endif /* __AUTO_TEST__ */
}

/******************************************************************************
 *  Function    -  track_drv_gps_epo_req
 * 
 *  Purpose     -  EPO׼����ϣ�����ע������
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 12-03-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_gps_epo_req(void)
{
#if defined (__AUTO_TEST__)
    LOGD(L_DRV, L_V5, "");
    track_factoty_autotest_epo_req();
#endif /* __AUTO_TEST__ */
}

    
/******************************************************************************
 *  Function    -  track_drv_charger_bmt_status
 *
 *  Purpose     -  �������Ϣע������
 *
 *  Description -  status : �����״̬
 *                       level : ��ص����ȼ�
 *                       volt : ��ص�ѹֵ(����ֵ��/1000000 )
 * Sample:

     switch(status)
    {
        // ���״̬
        case PMIC_USB_CHARGER_IN:
        case PMIC_CHARGER_IN:
            //track_pmic_charger_in_status();
            break;

        //���γ��¼�
        case PMIC_USB_NO_CHARGER_OUT:
        case PMIC_USB_CHARGER_OUT:
        case PMIC_CHARGER_OUT:
            //track_pmic_charger_out_status();
            break;

        //������״̬
        case PMIC_CHARGE_COMPLETE:
            //track_pmic_charger_completed_status();
            break;

        case PMIC_VBAT_STATUS:
            {
                //��ȡ��ѹֵ
                switch(level)
                {
                    //�͵�ػ��¼�
                    case BATTERY_LOW_POWEROFF:
                        //track_pmic_batter_low_poweroff_status();
                        break;

                    case BATTERY_LOW_TX_PROHIBIT:
                        break;

                    // �͵羯��״̬
                    case BATTERY_LOW_WARNING:
                        //track_pmic_batter_low_warning_status();
                        break;

                    //��������״̬
                    case BATTERY_LEVEL_0:
                    case BATTERY_LEVEL_1:
                    case BATTERY_LEVEL_2:
                    case BATTERY_LEVEL_3:
                        //track_pmic_batter_normal_status();
                        break;

                    default:
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
        case PMIC_USB_NO_CHARGER_IN:
            break;

        default:
            break;
    }
 *
 * ������Ϣÿ����һ�Σ�����¼�������Ӧ
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 27-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_charger_bmt_status(pmic_status_enum status, battery_level_enum level, kal_uint32 volt)
{
    track_cust_chargestatus_vbtstatus_check(status, level, volt);
}

/******************************************************************************
 *  Function    -  track_drv_external_batter_measure_done
 * 
 *  Purpose     -  ��������
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 11-03-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_external_batter_measure_done(kal_int32 ext_volt)
{
    //track_cust_external_batter_measure_done(ext_volt);
}

/******************************************************************************
 *  Function    -  track_drv_eint_trigger_response
 *
 *  Purpose     -  �ж���Ӧע��
 *
 *  Description -  eint :  �жϿ����
 *                                 (�ο�eint_var.c ����track_interrupt.h)
 *                       level : ��ǰ�˿ڵ�ƽ
 *                                 (���ܽ������ܼ��缫���룬��⵽�ĵ�ƽ��ʵ���෴)
 *
 *   ��ʼ�����������
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 27-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_eint_trigger_response(U8 eint, U8 level)
{
    track_cust_eint_trigger_response(eint, level);
}

/******************************************************************************
 *  Function    -  track_drv_remind_sim_type
 * 
 *  Purpose     -  SIM ��״̬����
 * 
 *  Description - ����5 ���ִ��һ��

 * sim_type : 0=�޿�/1=����ͨSIM ��/ 2=�в��Կ�/-1=�쳣/  
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 28-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_remind_sim_type(kal_int8 sim_type)
{
    /*��������Ӳ�ͬSIM ���Ĵ���*/
    track_query_sim_type(sim_type);//�׿�
}

/******************************************************************************
 *  Function    -  track_drv_watch_dog_close
 * 
 *  Purpose     -  ��չر��ⲿ���Ź�152ģ�麯��
 * 
 *  Description - 

 * shutdown : 0=�ر��ṩ��152��ι���źţ��ն˽�����һ�����ڱ�152����
 *            1=�ر�152��ػ�
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 28-9-2013,  wangqi  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_watch_dog_close(kal_uint8 shutdown)
{
    LOGD(L_DRV, L_V5, "%d",shutdown);
#if defined (__NETWORK_LICENSE__)
    LOGS("test mode (restart disable)");
#else
#if defined( __WATCH_DOG__)
    track_drv_watdog_close(shutdown);
#else
    if(shutdown)
    {
        LOGS("====colse watch dog====");
        LOGS("====device power off====");
        track_drv_sys_power_off_req();
    }
#endif /* __WATCH_DOG__ */
#endif
}
