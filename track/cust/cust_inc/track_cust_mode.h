#if defined(__XCWS__)
#ifndef _TRACK_CUST_MODE_H
#define _TRACK_CUST_MODE_H
/******************************************************************************
 * track_cust_mode.h -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        工作模式头文件
 *
 * modification history
 * --------------------
 * v1.0   2012-07-26,  Liujw create this file
 *
 ******************************************************************************/
#define __TRACK_MODE_DEBUG__
/****************************************************************************
* Include Files             包含头文件
*****************************************************************************/
/*****************************************************************************
 *  Define			    宏定义
*****************************************************************************/


/*****************************************************************************
* Typedef  Enum         枚举
*****************************************************************************/
typedef enum
{
    NORNAL_MODE=0,                                            //正常模式
    SLEEP_MODE,                               //休眠模式
    MODE_MAX
} terminal_work_mode;                            //终端工作模式
typedef enum
{
    w_charge_out_msg=0, //充电移除   
    w_charge_in_msg, //充电移除                     
    w_sensor_smg,             // 异常震动
    w_nornal_charge_out_msg,//开机未接充电器
    w_timer_in_nornal_msg,//定时进入正常模式
    w_contorl_gps_on_msg,
    w_acc_low_msg,
    w_acc_high_msg,
    w_dw_msg,

}  work_msg_ind;
typedef enum
{ 
    DISARM_NORNAL=1,              // 撤防模式
    DELAY_FORTIFACATION_MODE,
    FORTIFACATION_MODE,        //设防模式                          
} terminal_defense_mode;    

typedef enum
{
    d_acc_high_msg=0,  //ACC高
    d_acc_low_msg,  //ACC低
    d_cmd_disarm_msg,             // 指令撤防
    d_cmd_fortifation_msg,              // 指令 设防
    d_nvram_fortifation_msg //nvram设防
}  defense_msg_ind;


/*****************************************************************************
*  Struct			    数据结构定义
*****************************************************************************/


/*****************************************************************************
* Local variable            局部变量
*****************************************************************************/


/****************************************************************************
* Global Variable           全局变量
*****************************************************************************/


/*****************************************************************************
 *  Local Functions	            本地函数
*****************************************************************************/
extern void track_cust_defense_mode_ind(defense_msg_ind type);
extern void track_cust_work_mode_ind(work_msg_ind type);
extern terminal_work_mode track_cust_get_work_mode_cq(void);
extern terminal_defense_mode track_cust_get_defense_mode(void);
extern void track_cust_set_work_mode(void *arg);
extern void track_cust_set_defense_mode_cq(void *arg);
/*****************************************************************************
 *  Global Functions            全局函数
*****************************************************************************/
#endif /* _TRACK_CUST_MODE_H */
#endif /* __XCWS__ */
