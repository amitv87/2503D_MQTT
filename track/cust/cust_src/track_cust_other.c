/*******************************************************************************************
 * Filename:
 * Author :
 * Date :
 * Comment:
 ******************************************************************************************/

/****************************************************************************
*  Include Files                        包含头文件
*****************************************************************************/
#include "math.h"

#include "track_cust.h"
#include "l4c_common_enum.h"
#include "track_os_ell.h"


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
static kal_bool ReStartValid = KAL_FALSE;
/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/
#if defined(__SERVER_IP_LINK__)
extern kal_bool track_cust_first_link(void);
#endif
/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/

#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      本地函数
******************************************************************************/
void track_cust_gprs_conn_fail(void *arg);
/*****************************************************************************
*  Global Functions                     全局函数
******************************************************************************/

/******************************************************************************
 *  Function    -  track_cust_periodic_restart
 *
 *  Purpose     -  周期性定时重启机制
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 08-12-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_periodic_restart(void *arg)
{
    kal_uint32 par = (kal_uint32)arg;
    LOGD(L_APP, L_V6, "%d", par);
#if defined (__GT300__)||defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
    if(WORK_MODE_3 == track_cust_get_work_mode()) 
    {
        track_start_timer(TRACK_CUST_PERIODIC_RESTART_TIMER_ID, track_drv_get_one_day_reset_time(), track_cust_periodic_restart, (void*)2);
        CTimer_Start(CTIMER_24HOUR_RESET, track_drv_get_one_day_reset_time(), track_cust_periodic_restart, (void*)2);
        return;
    }
#endif /* __GT300__ */
#if defined(__FLY_MODE__)
    if(track_cust_flymode_set(192))//飞行模式中，重置20分钟重启机制
        {
            track_start_timer(TRACK_CUST_PERIODIC_RESTART_TIMER_ID, track_drv_get_one_day_reset_time(), track_cust_periodic_restart, (void*)2);
            CTimer_Start(CTIMER_24HOUR_RESET, track_drv_get_one_day_reset_time(), track_cust_periodic_restart, (void*)2);
            return;
        }
#endif /* __FLY_MODE__*/
    if(par == 1)
    {
        if(ReStartValid)
        {
            LOGD(L_APP, L_V4, "检测到震动，周期性重启继续延后十分钟");
            track_start_timer(TRACK_CUST_PERIODIC_RESTART_DELAY_TIMER_ID, 600000, track_cust_periodic_restart, (void*)3);
            CTimer_Start(CTIMER_24HOUR_RESET, 600000, track_cust_periodic_restart, (void*)3);
        }
    }
    else if(par == 2)
    {
        LOGD(L_APP, L_V4, "24小时到，进入十分钟静止判断");
        ReStartValid = KAL_TRUE;
        track_start_timer(TRACK_CUST_PERIODIC_RESTART_DELAY_TIMER_ID, 600000, track_cust_periodic_restart, (void*)4);
        CTimer_Start(CTIMER_24HOUR_RESET, 600000, track_cust_periodic_restart, (void*)4);
    }
    else if(par == 3)
    {
        LOGD(L_APP, L_V6, "%d", par);
        if((G_realtime_data.netLogControl & 128) == 0)
        {
            track_cust_restart(4, 0);
        }
    }
    else if(par == 4)
    {
        LOGD(L_APP, L_V6, "%d", par);
        if((G_realtime_data.netLogControl & 128) == 0)
        {
            track_cust_restart(27, 0);
        }
    }
}

/******************************************************************************
 *  Function    -  track_cust_gps_nodata_restart
 *
 *  Purpose     -  GPS开启后，无数据输出，重启
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_gps_nodata_restart1(void)
{
    LOGD(L_APP, L_V4, "GPS处于开启状态，已有20秒无数据输出，重启");
#if defined(__TEST_DEVICE__)
    if(track_rid_given()) return;
#endif /* __TEST_DEVICE__ */
#if defined(__ATGM_UPDATE__)
    if(track_cust_atgm_update_status(STATUS_CHECK) >= ATGM_UPDATE_STATUS_update_begin_req)
    {
        LOGD(L_APP, L_V1, "升级中");
        return;
    }
#endif /*__ATGM_UPDATE__*/

    if(track_is_testmode() || track_nvram_alone_parameter_read()->gprson == 0 || (track_is_autotestmode() == KAL_TRUE) || track_get_gps_ota(0xff))
    {
        LOGD(L_APP, L_V7, "测试模式取消重启");
        return;
    }
    if(track_os_log5_status())
    {
        LOGD(L_APP, L_V7, "取消重启");
        return;
    }
    if((G_realtime_data.netLogControl & 128) == 0)
    {
        track_cust_restart(5, 0);
    }
}

void track_cust_gps_nodata_restart(void)
{
    if(track_is_testmode() || track_is_autotestmode()) return;
#if defined(__ATGM_UPDATE__)
    if(track_cust_atgm_update_status(STATUS_CHECK) >= ATGM_UPDATE_STATUS_update_begin_req)
    {
        LOGD(L_APP, L_V1, "升级中,不nodata_restart");
        return;
    }
#endif /*__ATGM_UPDATE__*/    
    
    LOGD(L_APP, L_V4, "GPS处于开启状态，已有20秒无数据输出，尝试重启GPS");
    track_cust_agps_restart();
 //   tr_start_timer(TRACK_CUST_PERIODIC_RESTART_GPS_TIMER_ID, 10000, track_cust_gps_nodata_restart1);
}
void track_cust_conn_fail_ctimer()
{
    LOGD(L_APP,L_V5,"");
    track_cust_gprs_conn_fail((void *)3);
}
/******************************************************************************
 *  Function    -  track_cust_gprs_conn_fail
 *
 *  Purpose     -  GPRS连接失败，定时重启
 *
 *  Description -
 *                 GPRS连接开启，20分钟无法成功连接则判断GPS定位状态
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 22-12-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_gprs_conn_fail(void *arg)
{
    kal_uint32 status = (kal_uint32)arg;
    extern void track_gps_no_location_dup_updata(void);

    if(status == 1)
    {
        kal_uint32 period;
        //period = pow(2, G_realtime_data.reStartSystemCount);
        LOGD(L_APP, L_V6, "如果20分钟内未收到下一心跳包回复，将尝试重启恢复!");
        track_start_timer(TRACK_CUST_PERIODIC_RESTART_GPRS_TIMER_ID, 1200000, track_cust_gprs_conn_fail, (void*)3);
        CTimer_Start(CTIMER_20MIN_RESET, 1500000, track_os_intoTaskMsgQueue, (void*)track_cust_conn_fail_ctimer);

        /*对于GPS非常开情况下，这里调用GPSDUP会导致上报多余，需要过滤       --    chengjun  2017-04-26*/
       if(!track_is_timer_run( TRACK_CUST_DETECT_NO_VIB_TIMER_ID))
       {
            track_gps_no_location_dup_updata();
       }       
    }
    else if(status == 3)
    {
        LOGD(L_APP, L_V5, "status:%d", status);
        if(track_nvram_alone_parameter_read()->gprson == 0 || track_cust_get_work_mode() == WORK_MODE_3||track_is_notquit())
        {
            LOGD(L_APP, L_V4, "20分钟GPRS网络维持机制停止，当前是测试白卡或用户主动关闭了GPRS连接");
            return;
        }

        if(track_is_in_call() || track_cust_make_call((void *)99)==1)//通话过程中重置20分钟重启机制
        {
            LOGD(L_APP, L_V4, "20分钟GPRS网络维持机制重置，当前正在通话");
            track_cust_gprs_conn_fail((void*)1);
            return;
        }
#if defined(__FLY_MODE__)
        if(track_cust_flymode_set(191))//飞行模式中，重置20分钟重启机制
        {
            LOGD(L_APP, L_V4, "20分钟GPRS网络维持机制重置，当前正在飞行中");
            track_cust_gprs_conn_fail((void*)1);
            return;
        }
#endif /* __FLY_MODE__*/
#if defined(__SERVER_IP_LINK__)
		if(track_soc_get_vector_queue() == 0 && track_cust_first_link()) 
		{
		    LOGD(L_APP, L_V4, "20分钟GPRS网络维持机制重置，当前队列中无数据要发送");
            track_cust_gprs_conn_fail((void*)1);
            return;
		}
#endif

#if defined (__NT33__) && defined (__CUSTOM_DDWL__)
		if(track_cust_dormancy_change_flight_mode(0xFF))//飞行模式中，重置20分钟重启机制
		{
			LOGD(L_APP, L_V4, "20分钟GPRS网络维持机制重置，当前正在飞行中");
			track_cust_gprs_conn_fail((void*)1);
			return;
		}
#endif /* __NT33__ */

#if defined(__GT02__)
        if(G_importance_parameter_data.dserver.conecttype== 1&&G_importance_parameter_data.dserver.ip_update&&G_importance_parameter_data.dserver.server_type==1)
        {
            G_importance_parameter_data.dserver.conecttype=0;
            Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        }
        else if(track_nvram_alone_parameter_read()->server.conecttype== 1&&G_importance_parameter_data.dserver.ip_update&&G_importance_parameter_data.dserver.server_type==0)
        {
            track_nvram_alone_parameter_read()->server.conecttype=0;
             Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        }
        else if(track_nvram_alone_parameter_read()->server.conecttype== 0&&G_importance_parameter_data.dserver.ip_update&&G_importance_parameter_data.dserver.server_type==0)
        {
             track_nvram_alone_parameter_read()->server.conecttype=1;
             Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        }
       else if(G_importance_parameter_data.dserver.conecttype== 0&&G_importance_parameter_data.dserver.ip_update&&G_importance_parameter_data.dserver.server_type==1)
        {
            G_importance_parameter_data.dserver.conecttype=1;
           Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        }
       
#endif

        //G_realtime_data.reStartSystemCount++;
        //Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);


        if((G_realtime_data.netLogControl & 128) == 0)
        {
#if defined(__JM66__)
            LOGD(L_APP, L_V4, "20分钟不上线,请求关机");
            track_cust_jm66_shutdown();
#elif defined(__DASOUCHE__) 
            LOGD(L_APP, L_V4, "20分钟不上线,5s后关机");
            track_cust_change_pk_acount((void*)1);
            track_cust_restart(28, 5);            
#else
            track_cust_restart(28, 0);
#endif//__JM66__
        }
    }
}


/******************************************************************************
 *  Function    -  track_cust_fetion_auto_authorize_valid
 *
 *  Purpose     -  为测试方便，特别为飞信开发有限制的自动授权
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 10-01-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint8 track_cust_fetion_auto_authorize_valid(void *arg)
{
    static kal_uint8 valid = 0;
    switch((kal_uint32)arg)
    {
        case 1:
            LOGD(L_APP, L_V4, "十分钟飞信自动授权开启");
            valid = 1;
            track_start_timer(TRACK_CUST_FETION_AUTO_AUTHORIZE_TIMER, 600000, track_cust_fetion_auto_authorize_valid, (void *)2);
            break;

        case 2:
            LOGD(L_APP, L_V4, "飞信自动授权结束");
            valid = 0;
            break;
    }
    return valid;
}

/******************************************************************************
 *  Function    -  track_cust_fetion_auto_authorize
 *
 *  Purpose     -  飞信自动授权处理
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_fetion_auto_authorize(track_sms_param *sms_rev)
{
    LOGD(L_APP, L_V5, "");
    if(track_cust_fetion_auto_authorize_valid(NULL) == 1)
    {
        kal_uint8 *start, *end;
        kal_uint8 data[18] =
        {
            0x5E, 0x0C, 0x67, 0x1B, 0x52, 0xA0, 0x60, 0xA8, 0x4E, 0x3A, 0x98, 0xDE, 0x4F, 0xE1, 0x59, 0x7D,
            0x53, 0xCB
        };
        start = track_fun_strWchr(data, sms_rev->TP_UD, sms_rev->TP_UDL);
        if(start == NULL)
        {
            LOGD(L_APP, L_V5, "error 1");
            return;
        }
        if(memcmp(start, data, 18))
        {
            LOGD(L_APP, L_V5, "error 2");
            return;
        }
        track_cust_sms_send(sms_rev->P_TYPE, sms_rev->TPA, 1, "Y", 1);
    }
}


/******************************************************************************
 *  Function    -  track_cust_restart_disable
 *
 *  Purpose     -  禁止应用逻辑调用重启
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 30-01-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_restart_disable(void)
{
    G_realtime_data.netLogControl |= 128;
}

/******************************************************************************
 *  Function    -  track_cust_sim_restart
 *
 *  Purpose     -  对SIM卡因接触不良或受到干扰，重新初始化SIM回复GSM网络附着
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 20-12-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_sim_restart(void *arg)
{
    static kal_uint8 status = 0;
    kal_uint32 par = (kal_uint32)arg;
    LOGD(L_APP, L_V5, "%d", par);
    if(par == 0)
    {
        if(!track_is_timer_run(TRACK_CUST_SIM_RESTART_TIMER))
        {
            status = 0;
            track_start_timer(TRACK_CUST_SIM_RESTART_TIMER, 60000, track_cust_sim_restart, (void *)1);
        }
    }
    else if(par == 1)
    {
        status = 1;
        l4c_nw_cfun_state_req(RMMI_SRC, 0);  //进入飞行模式
        track_start_timer(TRACK_CUST_SIM_RESTART_TIMER, 3000, track_cust_sim_restart, (void *)10);
    }
    else if(par == 10)
    {
        l4c_nw_cfun_state_req(RMMI_SRC, 1);  //退出飞行模式
    }
    else if(par == 99)
    {
        if(status == 0 && track_is_timer_run(TRACK_CUST_SIM_RESTART_TIMER))
            track_stop_timer(TRACK_CUST_SIM_RESTART_TIMER);
    }
}

