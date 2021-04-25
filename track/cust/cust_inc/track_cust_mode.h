#if defined(__XCWS__)
#ifndef _TRACK_CUST_MODE_H
#define _TRACK_CUST_MODE_H
/******************************************************************************
 * track_cust_mode.h -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        ����ģʽͷ�ļ�
 *
 * modification history
 * --------------------
 * v1.0   2012-07-26,  Liujw create this file
 *
 ******************************************************************************/
#define __TRACK_MODE_DEBUG__
/****************************************************************************
* Include Files             ����ͷ�ļ�
*****************************************************************************/
/*****************************************************************************
 *  Define			    �궨��
*****************************************************************************/


/*****************************************************************************
* Typedef  Enum         ö��
*****************************************************************************/
typedef enum
{
    NORNAL_MODE=0,                                            //����ģʽ
    SLEEP_MODE,                               //����ģʽ
    MODE_MAX
} terminal_work_mode;                            //�ն˹���ģʽ
typedef enum
{
    w_charge_out_msg=0, //����Ƴ�   
    w_charge_in_msg, //����Ƴ�                     
    w_sensor_smg,             // �쳣��
    w_nornal_charge_out_msg,//����δ�ӳ����
    w_timer_in_nornal_msg,//��ʱ��������ģʽ
    w_contorl_gps_on_msg,
    w_acc_low_msg,
    w_acc_high_msg,
    w_dw_msg,

}  work_msg_ind;
typedef enum
{ 
    DISARM_NORNAL=1,              // ����ģʽ
    DELAY_FORTIFACATION_MODE,
    FORTIFACATION_MODE,        //���ģʽ                          
} terminal_defense_mode;    

typedef enum
{
    d_acc_high_msg=0,  //ACC��
    d_acc_low_msg,  //ACC��
    d_cmd_disarm_msg,             // ָ���
    d_cmd_fortifation_msg,              // ָ�� ���
    d_nvram_fortifation_msg //nvram���
}  defense_msg_ind;


/*****************************************************************************
*  Struct			    ���ݽṹ����
*****************************************************************************/


/*****************************************************************************
* Local variable            �ֲ�����
*****************************************************************************/


/****************************************************************************
* Global Variable           ȫ�ֱ���
*****************************************************************************/


/*****************************************************************************
 *  Local Functions	            ���غ���
*****************************************************************************/
extern void track_cust_defense_mode_ind(defense_msg_ind type);
extern void track_cust_work_mode_ind(work_msg_ind type);
extern terminal_work_mode track_cust_get_work_mode_cq(void);
extern terminal_defense_mode track_cust_get_defense_mode(void);
extern void track_cust_set_work_mode(void *arg);
extern void track_cust_set_defense_mode_cq(void *arg);
/*****************************************************************************
 *  Global Functions            ȫ�ֺ���
*****************************************************************************/
#endif /* _TRACK_CUST_MODE_H */
#endif /* __XCWS__ */
