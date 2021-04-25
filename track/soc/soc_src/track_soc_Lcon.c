/*******************************************************************************************
 * Filename:
 * Author :
 * Date :
 * Comment:
 ******************************************************************************************/

/****************************************************************************
*  Include Files                        包含头文件
*****************************************************************************/
#include "Track_soc.h"
#include "track_soc_Lcon.h"
#include "c_vector.h"
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

typedef struct
{
    kal_uint8   buffer[MAX_SND_BUFFER_SIZE];
    int         status; /* 状态:  0 等待中  ;   1 正在发送 */
    int         len;
    kal_uint32  no;
    kal_uint8   packet_type;              // 数据包类型
    kal_uint8   lock;                     // 正在发送数据时避免进行删除
} vec_gprs_socket_send_struct;

#define TIMEOUT_FOR_SEND_LBS_MINSEC  (2*60*1000)
#define TIMEOUT_FOR_CLOSE_MINSEC  (2*60*1000+20*1000)
/*****************************************************************************
*  Local variable                       局部变量
*****************************************************************************/
#if defined(__MCU_LONG_STANDBY_PROTOCOL__) || defined(__JM66__)
c_vector* vec_send_mcu_queue = NULL;//单片机通信队列
#endif//__MCU_LONG_STANDBY_PROTOCOL__
static c_vector* vec_send_alarm_queue = NULL;
static c_vector* vec_send_position_queue = NULL;
static c_vector* vec_send_log_queue = NULL;
static c_vector* vec_send_netcmd_queue = NULL;
static sockaddr_struct ip_addr;
static Func_Ptr s_func = NULL;
static kal_bool connstatus = KAL_FALSE;
static kal_uint8 batch_paket = 1;          // 数据打包发送设置；1～50，默认值为：1
static kal_bool heartbeat_packets_need_send = KAL_FALSE;
static kal_bool LBS_packets_need_send = KAL_FALSE;
static kal_uint8 batchCount_for_del = 1;
static kal_bool bootsendlbs = KAL_FALSE;
static kal_bool Instant_Upload = KAL_FALSE;
extern kal_bool heartback_send_flag;
/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/
extern kal_uint32 track_cust_get_overtime_address(kal_uint32 flag, kal_uint32 time);
#if defined(__XCWS__)
extern void track_cust_paket_T2();
#endif
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
extern void track_cust_module_extchip_shutdown(void);
#endif /* __GT300S__ */
extern void track_drv_sys_power_off_req(void);
//extern void track_cust_module_delay_close();
#if defined(__XCWS_TPATH__)
extern void track_cust_server3_queue_init(void);
#endif

#if defined (__GT740__)||defined(__GT420D__)
extern void track_cust_save_logfile(kal_uint8 *data, kal_uint16 len);
extern void track_cust_module_extchip_shutdown(void);
extern void track_cust_module_delay_close(U8 op);
extern kal_bool track_cust_get_wifidata_send_sign(kal_uint8 par);

#endif

#if defined (__XYBB__)
#include "track_cust_paket_bb.h"
#endif /* __XYBB__ */
/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/
void track_soc_send(void *par);
void heartbeat_packets(void *arg);
void VECTOR_Erase2(c_vector* obj, kal_uint32 pos_start, kal_uint32 num, kal_bool lock_is_valid);
void track_soc_queue_all_to_nvram(void);
#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      本地函数
******************************************************************************/

/******************************************************************************
 *  Function    -  LCon_event
 *
 *  Purpose     -  TCP长连接事件
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void LCon_event(track_soc_Callback_struct *par)
{
    LOGD(L_SOC, L_V5, "status=%d,%d,%d,%d,%x", par->status, par->errorid, par->queue_id, par->batchCount,par->packet_type);
    switch(par->status)
    {
        case SOC_CONNECT_SUCCESS:
            break;

        case SOC_CONNECT_ServerNotResponding:
        case SOC_CONNECT_CLOSE:
            connstatus = KAL_FALSE;
            break;

        case SOC_SEND_DATA:
            {
                if(par->errorid != 0)
                {
                    LOGD(L_SOC, L_V1, "ERROR!!! id=%d", par->errorid);
                    socket_disconnect(0);
                }
                else
                {
                    if(par->queue_id == 0)
                    {
#if 0//defined(__AMS_LOG__)
                        char buf[20] = {0};
                        sprintf(buf, "(N)(%d) ", (kal_get_systicks() - 0) / KAL_TICKS_1_SEC);
                        if(G_parameter.extchip.mode!=1)track_cust_cache_netlink(buf, strlen(buf));
#endif
                        track_os_intoTaskMsgQueueExt(track_soc_send, (void*)9);
                    }
                    else if((par->queue_id >= 1) && (par->queue_id < 9))
                    {
                        batchCount_for_del = par->batchCount;
						track_os_intoTaskMsgQueueExt(track_soc_send, (void*)par->queue_id);
#if defined(__GT740__)||defined(__GT420D__)
                        if(G_parameter.extchip.mode!=1)
                        {
							if(par->queue_id==PACKET_ALARM_DATA)
							{								
								if(G_realtime_data.tracking_mode)
								{
									track_cust_extalm_callback(par->no);
								}
						    }
							 
						}
#endif
                    }
                    else
                    {
                        LOGD(L_SOC, L_V1, "ERROR!!! no=%d", par->queue_id);
                    }
                }
                break;
            }
    }
    if(s_func != NULL)
    {
        s_func(par);
    }
}
void track_soc_status_callback(track_soc_Callback_struct *par)
{
    if(par->errorid != 0)  connstatus = KAL_FALSE;
    LCon_event(par);
}

void track_cust_lbs_send_rst(kal_bool op)
{
    bootsendlbs =op;
}


/******************************************************************************
 *  Function    -  track_cust_send_lbs
 *
 *  Purpose     -  超时发送LBS时间
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 04-11-2016,  XZQ  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_send_lbs(void *par)
{
    kal_bool login = track_soc_login_status();
    kal_bool closetimer =  track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID);
    kal_uint8 parameter = (U8)par;
    kal_int8 sim_status = track_drv_get_sim_type();
    applib_time_struct tmp = {0};
    applib_dt_get_rtc_time(&tmp);

#if defined(__GT370__)|| defined (__GT380__)
    if(track_cust_ota_update(99) == 0)
    {
        if(track_cust_gps_status() < GPS_STAUS_2D)
        {
            bootsendlbs = KAL_TRUE;
            //track_cust_sendWIFI_paket();//传WIFI
            track_cust_paket_18(track_drv_get_lbs_data(),0);//LBS
        }
    }
#elif defined(__GT300S__)
    if(track_cust_ota_update(99) == 0)
    {
        if(track_cust_gps_status() < GPS_STAUS_2D)
        {
            bootsendlbs = KAL_TRUE;
            track_cust_paket_18(track_drv_get_lbs_data());//少个参数
        }
    }
#elif defined(__GT740__)
    LOGD(L_APP, L_V5, "par %dign %d, tracking_mode %d, mode_en %d", parameter, G_realtime_data.nw_reset_sign,G_realtime_data.tracking_mode,G_parameter.extchip.mode1_en);
    if(track_cust_ota_update(99) == 0)
    {
        if(track_cust_modeule_get_wakemode()!=wk_ams_link)
        {
            if(2 == parameter)//power on
            {
                if(G_realtime_data.tracking_mode && track_cust_modeule_get_wakemode()!=wk_rem_alm)
                {
                    G_realtime_data.tracking_count++;
                    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                }
                if(G_parameter.extchip.mode1_en)
                {
                    track_cust_into_mode1en();
                }
                if(G_realtime_data.nw_reset_sign>0 && mmi_Get_PowerOn_Type() == ABNRESET && track_cust_backup_data_get_total_in_queue()>0)//搜不到网主动重启
                {
                      //track_drv_sensor_off();
                }
                else                    
                {
                    if(G_parameter.wifi.sw)
                    {
                        if(!track_is_timer_run(TRACK_CUST_OPEN_WIFI_WORK_TIMER_ID))
                        {
                            tr_start_timer(TRACK_CUST_OPEN_WIFI_WORK_TIMER_ID, 15 * 1000, track_drv_wifi_work_mode);
                        }
                    }
                    else
                    {
                        tr_start_timer(TRACK_CUST_SEND_LBS_LIMIT_TIMER_ID, 20000, track_cust_sendlbs_limit);
                        if(G_parameter.gpswake==0)
                        {
                            bootsendlbs = KAL_TRUE;//不等待定位，上传LBS回调后关机
                        }
                    }
                    if(!track_is_timer_run(TRACK_CUST_SEND_LBS_TIMER_ID))
                   {
                        if( mmi_Get_PowerOn_Type() == ABNRESET)
                        {
                            track_start_timer(TRACK_CUST_SEND_LBS_TIMER_ID, (1* 60 * 1000), track_cust_send_lbs, (void*)1);
                        }
                        else
                        {
    			     track_start_timer(TRACK_CUST_SEND_LBS_TIMER_ID, TIMEOUT_FOR_SEND_LBS_MINSEC, track_cust_send_lbs,(void*)1);
                        }
                   }
                }
                
            }
			else if(4 == parameter)//搜索不到WIFI
			{
				track_cust_sendlbs_limit();
				track_project_save_switch(1, sw_nowifi);
				track_cust_lbs_send_rst(KAL_TRUE);
				if( mmi_Get_PowerOn_Type() == ABNRESET)
				{
					track_start_timer(TRACK_CUST_SEND_LBS_TIMER_ID, (1 * 60 * 1000), track_cust_send_lbs, (void*)1);
				}
				else
				{
					track_start_timer(TRACK_CUST_SEND_LBS_TIMER_ID, TIMEOUT_FOR_SEND_LBS_MINSEC-30*1000, track_cust_send_lbs,(void*)1);
				}			
			}
            else
            {
                if(G_parameter.extchip.mode!=1)
                {
                    char buf[100]={0};
                    int v_size = 0, v_size_nvram;
                    int rssi = track_cust_get_rssi(-1);
                    v_size_nvram = track_cust_backup_data_get_total_in_queue();
                    LOGD(L_APP, L_V5, "超时未定位或未联网 login %d", login);
                    v_size = VECTOR_Size(vec_send_position_queue);
    		        sprintf(buf,"LINKOUT_CODE=%d,SIM=%d,(H)(%d)%d,%d,%d,%d,%d wake%d",track_cust_linkout_par(0xff),sim_status,RUNING_TIME_SEC,rssi,v_size, login,track_cust_gps_status(),closetimer,track_cust_modeule_get_wakemode());                
                    if(!login)
                    {                     
    		            if(parameter!=3)
                        {
                            track_start_timer(TRACK_CUST_SEND_LBS_TIMER_ID, TIMEOUT_FOR_CLOSE_MINSEC, track_cust_send_lbs,(void*)3);	
                            return;
                        }                    
						if(v_size)
	                    {
							track_soc_bakup_data((void*)3);
	                    }
                        if(G_realtime_data.tracking_mode)
                        {
                            track_cust_module_deal_end_trackmode();
                            track_cust_module_delay_close(20);
                        }
                        else if(G_parameter.extchip.mode1_en)
                        {
                            track_cust_module_auto_sw_mode();
                            track_cust_module_delay_close(20);
                        }
                        else
                        {
                            if(v_size>0 || v_size_nvram>0)
                            {
                                track_cust_modeule_Supplements_data();
                            }
#if 0//defined(__AMS_LOG__)
                            sprintf(buf + strlen(buf), "\n");
                            track_cust_cache_netlink(buf, strlen(buf));
                            G_realtime_data.ams_push=1;
#endif
                            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                            track_cust_module_delay_close(10);
                        }
                    }
                    else
                    {
                        int pSize = VECTOR_Size(vec_send_position_queue);
                        bootsendlbs = KAL_TRUE;//超时关机
                        if(Socket_get_write_status(0) && pSize)
                        {
                             LOGD(L_SOC, L_V5, "Socket 堵住啦，有%d条定位数据未发出30秒后重连!!!!...",pSize);
                             tr_start_timer(TRACK_CUST_SOC_PLUGUP_TIMEROUT_TIMER_ID, 30 * 1000, track_cust_soc_plugup_timeout);
                        }
                        else if(pSize==0&& track_project_save_switch(99,sw_position_up)==0)
                        {
                             LOGD(L_SOC, L_V5, "已上线未有定位点产生，主动上发定位点!!!");
                             if(G_parameter.wifi.sw)
                            {
                                track_cust_sendWIFI_paket();
                            }
                            else
                            {
                                track_cust_sendlbs_limit();
                            }   
                        }
						else
						{
						}
                        if(G_parameter.lbs_sw==0 && G_parameter.gpswake==0 && G_parameter.wifi.sw==0)
                        {
                            LOGD(L_SOC, L_V5, "定位数据开关均关闭!!!");
                            track_cust_module_delay_close(29);
                        }
                    }
                }

            }

        }
        
    }
#endif /* __GT06D__ */

}

/******************************************************************************
 *  Function    -  track_soc_conn_req
 *
 *  Purpose     -  Socket连接请求函数
 *
 *  Description -  参数         描述
 *                 ip_addr      需要连接的目标地址和端口的结构体
 *                 func         事件处理回调函数
 *                 no           序号（回调函数中返回，仅仅是个标记作用，默认为0即可）
 *
 *  return         0            正常
 *                 -1           回调函数未设置
 *                 -2           无效的IP地址或端口
 *                 -3           上一个连接请求还未结束，不能重复请求
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 06-08-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int8 track_soc_conn_req(sockaddr_struct *ip_addr, Func_Ptr func, kal_uint32 no)
{
    if(func != NULL)
    {
        s_func = func;
    }
    if(s_func == NULL)
    {
        return -1;
    }
    return Socket_conn_req(0, ip_addr, LCon_event, no);
}

kal_int8 track_soc_callback_register(Func_Ptr func)
{
    LOGD2(L_SOC, L_V1, "");
    if(func != NULL)
    {
        s_func = func;
    }
    if(s_func == NULL)
    {
        return -1;
    }
    return socket_data_event_request(0, LCon_event);
}
/******************************************************************************
 *  Function    -  track_soc_disconnect
 *
 *  Purpose     -  断开当前连接请求
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_soc_disconnect(void)
{
    socket_disconnect(0);
}

/******************************************************************************
 *  Function    -  track_soc_conn_status
 *
 *  Purpose     -  标记连接状态
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_soc_conn_status(kal_bool st)
{
    connstatus = st;
}

/******************************************************************************
 *  Function    -  track_soc_write_req
 *
 *  Purpose     -  Socket发送数据函数
 *
 *  Description -  参数         描述
 *                 data         需要发送的数据指针首地址
 *                 datalen      发送的数据字节数
 *                 no           序号（回调函数中返回，仅仅是个标记作用，默认为0即可）
 *
 *  return         0            正常
 *                 -1           回调函数未设置，连接未建立，不能发送数据
 *                 -2           发送的数据长度大于4096字节
 *                 -3           上一个发送数据的请求还未结束，不能重复请求
 *                 -4           发送的数据指针为空
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 06-08-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int8 track_soc_write_req(kal_uint8 *data, kal_int32 datalen, kal_uint8 packet_type, kal_uint32 no)
{
    if(data == NULL)
    {
        return -12;
    }
    return Socket_write_req(0, data, datalen, 0, packet_type, no, 1);
}

/******************************************************************************
 *  Function    -  track_soc_gethostbyname
 *
 *  Purpose     -  Socket域名解析请求函数
 *
 *  Description -  参数         描述
 *                 hostname     需要解析的域名字符串
 *                 func         事件处理回调函数
 *                 no           序号（回调函数中返回，仅仅是个标记作用，默认为0即可）
 *
 *  return         0            正常
 *                 -1           回调函数未设置
 *                 -2           域名为空，或域名字符串长度超出了100个字符
 *                 -3           上一个域名解析请求还未结束，不能重复请求
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 06-08-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int8 track_soc_gethostbyname(char *hostname, Func_Ptr func)
{
    if(func != NULL)
    {
        s_func = func;
    }
    if(s_func == NULL)
    {
        return -1;
    }
    if(hostname == NULL)
    {
        return -2;
    }
    return Socket_gethostbyname(hostname, LCon_event);
}

#if defined(__DASOUCHE__)
static kal_int8 track_soc_get_send_data(kal_uint16 queue_id, kal_uint8 batchCount)
{
    static kal_uint8 data[3000];
    char buf1[100] = {0};
    applib_time_struct currTime_ams = {0};
    vec_gprs_socket_send_struct *buf = NULL;
    c_vector* vec_queue_tmp = NULL;
    int len, i, ret;
#if defined (__NETWORK_LICENSE__)
    return ;
    //禁止GPRS链接后台
#endif /* __NETWORK_LICENSE__ */
    if(batchCount * BACKUP_DATA_PACKET_MAX > SOC_SOCKET_SEND_DATA_MAX)
    {
        batchCount = SOC_SOCKET_SEND_DATA_MAX / BACKUP_DATA_PACKET_MAX - 1;
    }
    LOGD(L_SOC, L_V5, "queue_id=%d, batchCount=%d", queue_id, batchCount);

    switch(queue_id)
    {
        case PACKET_HEARTBEAT_DATA:   /* 发送心跳包 */
            {

            }
            break;

        case PACKET_ALARM_DATA:   /* 发送报警包 */
            {
                while(1)
                {
                    buf = VECTOR_At(vec_send_alarm_queue, 0);
                    if(buf)
                    {
                        buf->lock = 1;
                        //Socket_write_req(0, buf->buffer, buf->len, queue_id, buf->packet_type, buf->no, 1);
                        return track_cust_location(queue_id,buf->buffer);
                    }
                    else
                    {
                        LOGD(L_SOC, L_V1, "ERROR3: vec_send_alarm_queue");
                        VECTOR_Erase(vec_send_alarm_queue, 0, 1);
                    }
                    break;
                }
            }
            break;

        case PACKET_NET_CMD_DATA:   /* 发送在线指令包 */
            {

            }
            break;

        case PACKET_NVRAM_BACKUP_DATA:   /* 发送NVRAM备份的定位数据包 */
            {
                BACKUP_GPS_DATA tmp = {0};
                kal_uint8 *p = data;
                int v_size, sum = 0, v_size_position;
                v_size = track_cust_backup_data_get_total_in_queue();
                v_size_position = VECTOR_Size(vec_send_position_queue);
                if(v_size_position > 30 && v_size + v_size_position < BACKUP_DATA_MAX)
                {
                    track_soc_bakup_data((void*)3);  // 备份数据不能一次过备份，需要分多次，因此此处不能顺序执行下去
                    return -1;
                    //v_size = track_cust_backup_data_get_total_in_queue();
                }
                if(batchCount <= v_size)
                {
                    v_size = batchCount;
                }
                for(i = 1; i <= v_size; i++)
                {
                    ret = backup_data_read_from_queue_not_del(&tmp, i);
                    #if defined(__GT420D__)
                    snprintf(buf1, 99, "11\n\r");
                    track_cust_save_logfile(buf1, strlen(buf1));
                    #endif
                    if(ret <= 0)
                    {
                        #if defined(__GT420D__)
                        snprintf(buf1, 99, "12,%d\n\r",ret);
                        track_cust_save_logfile(buf1, strlen(buf1));
                        #endif
                        LOGD(L_SOC, L_V1, "ERROR5: ret=%d", ret);
                        break;
                    }
                    track_cust_paket_position_change_renew(tmp.buf, tmp.len);
                    //if(p - data + tmp.len > SOC_SOCKET_SEND_DATA_MAX)
                        //{
                            //LOGD(L_SOC, L_V5, "备份数据条数(%d)已达最大，立即发送",sum);
                            //break;
                            //}
                    #if defined(__GT420D__)
                        snprintf(buf1, 99, "13\n\r");
                        track_cust_save_logfile(buf1, strlen(buf1));
                        #endif
                    memcpy(p, tmp.buf, tmp.len);
                    LOGH(L_SOC, L_V6, p, tmp.len);
                    p += tmp.len;
                    sum++;
                }
                len = p - data;
                if(len > 0)
                {
                    LOGD(L_SOC, L_V5, "nvram data send len=%d,%d,%d", len, queue_id, sum);
                    //Socket_write_req(0, data, len, queue_id, 0, 0, sum);
                    #if defined(__GT420D__)
                        snprintf(buf1, 99, "14\n\r");
                        track_cust_save_logfile(buf1, strlen(buf1));
                        #endif
                    return track_cust_location(queue_id,data);
                }
            }
            break;

        case PACKET_POSITION_DATA:   /* 发送内存队列中的定位包 */
            {
                vec_queue_tmp = vec_send_position_queue;
            }
            break;

        case PACKET_NET_LOG_DATA:   /* 发送网络日志包 */
            {

            }
            break;
    }
    if(vec_queue_tmp != NULL)
    {
        kal_uint8 *p = data;
        int v_size, sum = 0;
        v_size = VECTOR_Size(vec_queue_tmp);
        if(batchCount <= v_size)
        {
            v_size = batchCount;
        }
        for(i = 0; i < v_size; i++)
        {
            buf = VECTOR_At(vec_queue_tmp, i);
            if(buf)
            {
                if((p + buf->len - data) > SOC_SOCKET_SEND_DATA_MAX)
                {
                    LOGD(L_SOC, L_V5, "WARN: 拼包数据过长，无法满足设定的拼包个数，截断。");
                    break;
                }
                buf->lock = 1;
                memcpy(p, buf->buffer, buf->len);
                LOGH(L_SOC, L_V6, p, buf->len);

                p += buf->len;
                sum++;
            }
            else
            {
                LOGD(L_SOC, L_V1, "ERROR3: vec_send_netcmd_queue");
                VECTOR_Erase(vec_queue_tmp, i, 1);
                break;
            }
        }
        len = p - data;
        if(len > 0)
        {
            LOGD(L_SOC, L_V5, "data send len=%d,%d,%d", len, queue_id, sum);
                #if defined(__GT420D__)
                track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                snprintf(buf1, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d data send len:%d,%d,%d\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec,
                            len,queue_id, sum
                            );
                track_cust_save_logfile(buf1, strlen(buf1));
                #endif

            return track_cust_location(queue_id,data);
        }
    }
	LOGD(L_SOC, L_V1, "没有send");
    return 0;
}
void track_soc_send(void *par)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

	//flag = KAL_TRUE 允许发送
    char buf1[100] = {0};
    applib_time_struct currTime_ams = {0};
    char ams_buf[100] = {0};
    applib_time_struct currTime = {0};
    static kal_bool flag = KAL_TRUE;
    vec_gprs_socket_send_struct *buf = NULL;
    kal_uint32 parameter = (kal_uint32)par;
    int v_size_alarm, v_size_position, v_size_nvram,ret;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOGD(L_SOC, L_V8, "parameter=%d, connstatus=%d,batch_paket=%d", parameter, connstatus, batch_paket);
    if(parameter == 99)
    {
        flag = KAL_TRUE;
        return;
    }
    if(!connstatus)
    {
        LOGD(L_SOC, L_V5,  "connstatus=%d", connstatus);
        track_start_timer(TRACK_SOC_SEND_TIMER_ID, 5000, track_soc_send, (void*)9);
        return;
    }
    #if defined(__GT420D__)
        snprintf(buf1, 99, "5\n\r");
        track_cust_save_logfile(buf1, strlen(buf1));
        #endif
    v_size_alarm = VECTOR_Size(vec_send_alarm_queue);
    v_size_position = VECTOR_Size(vec_send_position_queue);
    v_size_nvram = track_cust_backup_data_get_total_in_queue();
    LOGD(L_SOC, L_V5, "flag=%d, v_size_alarm=%d, v_size_nvram=%d, v_size_position=%d, parameter=%d, connstatus=%d",
         flag, v_size_alarm, v_size_nvram, v_size_position, parameter, connstatus);        
    if(parameter == PACKET_ALARM_DATA)
    {
        /* 收到成功送出上一个报警包的确认 */
        if(v_size_alarm)
        {
            LOGD(L_SOC, L_V3, "vec_send_alarm_queue %d", v_size_alarm);
            VECTOR_Erase2(vec_send_alarm_queue, 0, batchCount_for_del, KAL_FALSE);
            v_size_alarm = VECTOR_Size(vec_send_alarm_queue);
            LOGD(L_SOC, L_V3, "vec_send_alarm_queue %d", v_size_alarm);
        }
        flag = KAL_TRUE;
    }
    else if(parameter == PACKET_NVRAM_BACKUP_DATA)
    {
		/* 收到成功送出上一个在NVRAM中备份数据的确认 */
        if(v_size_nvram)
        {
            backup_data_del_out_queue(batchCount_for_del);
            v_size_nvram = track_cust_backup_data_get_total_in_queue();
        }
        flag = KAL_TRUE;
    }
    else if(parameter == PACKET_POSITION_DATA)
    {
		if(v_size_position)
        {
            buf = VECTOR_At(vec_send_position_queue, 0);
            VECTOR_Erase2(vec_send_position_queue, 0, batchCount_for_del, KAL_FALSE);
            v_size_position = VECTOR_Size(vec_send_position_queue);
        }
        flag = KAL_TRUE;
        #if defined(__GT420D__)
            if(G_parameter.extchip.mode == 1)
   	        {
                LOGD(L_APP,L_V5,"收到定位数据确认，延迟30S关机");
                if(track_cust_need_check_sportorstatic(99) == 1)
                {
                LOGD(L_APP,L_V5,"检测到运动转静止下需要检测速度，暂时不关机");
                return;
                }
                if(track_cust_module_alarm_lock(98, NULL)==1)
                  {
                    LOGD(L_APP,L_V5,"还有报警未处理不关机");
         
                  return;
                   }
                #if defined(__GT420D__)
                track_fun_get_time(&currTime, KAL_FALSE, &G_parameter.zone);
                snprintf(ams_buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d data send success\n\r",
                            currTime.nYear,
                            currTime.nMonth,
                            currTime.nDay,
                            currTime.nHour,
                            currTime.nMin,
                            currTime.nSec);
                LOGD(5,5,"len:%d",strlen(ams_buf));
                track_cust_save_logfile(ams_buf, strlen(ams_buf));
                #endif
                track_cust_module_delay_close(30);
                    }
                if(track_cust_check_gpsflag_status(99) == 2 || track_cust_gt420d_carstatus_data(99) == 2)
                {
                    track_cust_gps_off_req((void *)3);
                }
    #endif

    }   
    #if defined(__GT420D__)
        snprintf(buf1, 99, "6\n\r");
        track_cust_save_logfile(buf1, strlen(buf1));
        #endif
    LOGD(L_SOC, L_V5, "flag=%d", flag);    
    track_stop_timer(TRACK_SOC_SEND_TIMER_ID);
    if(v_size_alarm == 0
            && v_size_position == 0
            && v_size_nvram == 0)
    {
    
        /* 当前队列中无待发送的数据 */
        LOGD(L_SOC, L_V5, "return");
        return;
    }
    #if defined(__GT420D__)
        snprintf(buf1, 99, "7\n\r");
        track_cust_save_logfile(buf1, strlen(buf1));
        #endif
    if(Socket_get_write_status(0)) /* Socket是否正在写? */
    {
        track_start_timer(TRACK_SOC_SEND_TIMER_ID, 2000, track_soc_send, (void*)9);
        LOGD(L_SOC, L_V5, "Socket_get_write_status");
        return;
    }
    #if defined(__GT420D__)
        snprintf(buf1, 99, "8\n\r");
        track_cust_save_logfile(buf1, strlen(buf1));
        #endif
    if(!flag)
    {
        LOGD(L_SOC, L_V5, "已发送数据，还未有接收的发送成功的确认");
        track_start_timer(TRACK_SOC_SEND_TIMER_ID, 2000, track_soc_send, (void*)9);
        return;
    }
    #if defined(__GT420D__)
        snprintf(buf1, 99, "9,%d,%d\n\r",v_size_nvram,v_size_position);
        track_cust_save_logfile(buf1, strlen(buf1));
        #endif
    flag = KAL_FALSE;
    if(v_size_nvram)
    {
        /* 发送NVRAM备份的定位数据包 */
        kal_int8 ret;
        LOGD(L_SOC, L_V1, "发送备份包");
        if(G_parameter.group_package.sw == 0 || track_is_timer_run(TRACK_CUST_FT_WORK_TIMER_ID))
        {
            #if defined(__GT420D__)
            snprintf(buf1, 99, "10\n\r");
            track_cust_save_logfile(buf1, strlen(buf1));
            #endif
            ret = track_soc_get_send_data(PACKET_NVRAM_BACKUP_DATA, batch_paket);
        }
        if(ret <= 0)
        {
            #if defined(__GT420D__)
            snprintf(buf1, 99, "ERROR5\n\r");
            track_cust_save_logfile(buf1, strlen(buf1));
            #endif
            LOGD(L_SOC, L_V1, "ERROR5: %d", PACKET_NVRAM_BACKUP_DATA);
            flag = KAL_TRUE;
            track_start_timer(TRACK_SOC_SEND_TIMER_ID, 2000, track_soc_send, (void*)9);
        }
    }
    else if(v_size_alarm)
    {
        /* 发送报警包 */
        LOGD(L_SOC, L_V1, "发送报警包");
        if(track_soc_get_send_data(PACKET_ALARM_DATA, batch_paket) >= 0)
        {

        }
        else
        {
        	LOGD(L_SOC, L_V1, "ERROR3: %d", PACKET_ALARM_DATA);
        	flag = KAL_TRUE;
        }
    }   
    else if(v_size_position)
    {
        /* 发送内存队列中的定位包 */
        LOGD(L_SOC, L_V1, "发送队列定位包");
            #if defined(__GT420D__)
                track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                snprintf(buf1, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d send position\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec
                            );
        track_cust_save_logfile(buf1, strlen(buf1));
    #endif

        if(track_soc_get_send_data(PACKET_POSITION_DATA, batch_paket) < 0)
        {
                #if defined(__GT420D__)
                track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                snprintf(buf1, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d ERROR 6\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec
                            );
        track_cust_save_logfile(buf1, strlen(buf1));
    #endif

            LOGD(L_SOC, L_V1, "ERROR6: %d", PACKET_POSITION_DATA);
            flag = KAL_TRUE;
            track_start_timer(TRACK_SOC_SEND_TIMER_ID, 5000, track_soc_send, (void*)9);
        }
    }
    else
    {
        LOGD(L_SOC, L_V1, "ERROR9: ?");
        flag = KAL_TRUE;
        track_start_timer(TRACK_SOC_SEND_TIMER_ID, 2000, track_soc_send, (void*)9);
    }
}
#else
/******************************************************************************
 *  Function    -  track_soc_get_send_data
 *
 *  Purpose     -  根据数据包类型进行不同优先级的发送处理
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
/*需要判断返回值，但很多情况下无返回值 -- chengjun  2017-08-05*/
static kal_int8 track_soc_get_send_data(kal_uint16 queue_id, kal_uint8 batchCount)
{
    static kal_uint8 data[3000];
    vec_gprs_socket_send_struct *buf = NULL;
    c_vector* vec_queue_tmp = NULL;
    int len, i, ret;
#if defined (__NETWORK_LICENSE__)
    return -1;
    //禁止GPRS链接后台
#endif /* __NETWORK_LICENSE__ */
    if(batchCount * BACKUP_DATA_PACKET_MAX > 2500)
    {
        batchCount = 2500 / BACKUP_DATA_PACKET_MAX - 1;
    }
    LOGD(L_SOC, L_V5, "queue_id=%d, batchCount=%d", queue_id, batchCount);

    switch(queue_id)
    {
        case PACKET_HEARTBEAT_DATA:   /* 发送心跳包 */
            {
                kal_uint16 sn, len2;
#if defined(__XCWS__)
                track_cust_paket_T2();
                return 1;
#endif
#if defined (__XYBB__)
				track_cust_paket_0002();
				return 1;
#endif /* __XYBB__ */
#if defined(__GT02__)
                Heartbeat_Data_Packet(data, &len2, &sn);
#elif defined(__BCA__)
                track_cust_paket_29(data, &len2, &sn);
#elif defined(__GW100__) || defined(__GT740__)|| defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)|| defined (__JM66__)||defined(__GT420D__)
                track_cust_paket_23(data, &len2, &sn);
//#elif defined(__GT420D__)
		//track_cust_paket_36(data, &len2, &sn);
#else
        track_cust_paket_13(data, &len2, &sn);
#endif /* __GT02__ */
                return Socket_write_req(0, data, len2, queue_id, 0x13, sn, 1);
            }
            break;

        case PACKET_ALARM_DATA:   /* 发送报警包 */
            {
                while(1)
                {
                    buf = VECTOR_At(vec_send_alarm_queue, 0);
                    if(buf)
                    {
                        if((g_GT03D == KAL_TRUE) && track_is_timer_run(TRACK_CUST_DW_GPS_WORK_TIMER))
                        {
                            buf->lock = 1;
                            return  Socket_write_req(0, buf->buffer, buf->len, queue_id, buf->packet_type, buf->no, 1);
                        }
#if defined(__GT500__)||defined(__GT310__)||defined(__GT03F__) || defined(__GW100__)||defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
                        if((buf->packet_type == 0x1A || buf->packet_type == 0x17) && (track_cust_get_overtime_address(99, NULL) == 0))
                        {
                            VECTOR_Erase(vec_send_alarm_queue, 0, 1);
                            track_cust_get_overtime_address(2, NULL);
                            if(VECTOR_Size(vec_send_alarm_queue) > 0)
                            {
                                LOGD(L_SOC, L_V5, "在线地址请求包已经超时，不再向服务器请求，舍去");
                                continue;
                            }
                            return -1;
                        }
#else
                        if((buf->packet_type == 0x1B || buf->packet_type == 0x1A) && !track_is_timer_run(TRACK_CUST_GET_ADDRESS_OVER_TIMER_ID))
                        {
                            VECTOR_Erase(vec_send_alarm_queue, 0, 1);
                            if(VECTOR_Size(vec_send_alarm_queue) > 0)
                            {
                                LOGD(L_SOC, L_V5, "在线地址请求包已经超时，不再向服务器请求，舍去");
                                continue;
                            }
                            return -1;
                        }
#endif/*__GT03F__*/

                        buf->lock = 1;
                        return Socket_write_req(0, buf->buffer, buf->len, queue_id, buf->packet_type, buf->no, 1);
                    }
                    else
                    {
                        LOGD(L_SOC, L_V1, "ERROR3: vec_send_alarm_queue");
                        VECTOR_Erase(vec_send_alarm_queue, 0, 1);
                    }
                    break;
                }
            }
            break;

        case PACKET_NET_CMD_DATA:   /* 发送在线指令包 */
            {
                vec_queue_tmp = vec_send_netcmd_queue;
            }
            break;

        case PACKET_NVRAM_BACKUP_DATA:   /* 发送NVRAM备份的定位数据包 */
            {
                BACKUP_GPS_DATA tmp = {0};
                kal_uint8 *p = data;
                int v_size, sum = 0, v_size_position;
                v_size = track_cust_backup_data_get_total_in_queue();
                v_size_position = VECTOR_Size(vec_send_position_queue);
                if(v_size_position > 30 && v_size + v_size_position < BACKUP_DATA_MAX)
                {
                    track_soc_bakup_data((void*)3);  // 备份数据不能一次过备份，需要分多次，因此此处不能顺序执行下去
                    return -1;
                    //v_size = track_cust_backup_data_get_total_in_queue();
                }
                if(batchCount <= v_size)
                {
                    v_size = batchCount;
                }
                for(i = 1; i <= v_size; i++)
                {
                    ret = backup_data_read_from_queue_not_del(&tmp, i);
                    if(ret <= 0)
                    {
                        LOGD(L_SOC, L_V1, "ERROR5: ret=%d", ret);
                        break;
                    }
#ifndef __XCWS__
                    track_cust_paket_position_change_renew(tmp.buf, tmp.len);
#endif
                    memcpy(p, tmp.buf, tmp.len);
                    LOGH(L_SOC, L_V6, p, tmp.len);
                    p += tmp.len;
                    sum++;
                }
                len = p - data;
                if(len > 0)
                {
                    LOGD(L_SOC, L_V5, "nvram data send len=%d,%d,%d", len, queue_id, sum);
                    return Socket_write_req(0, data, len, queue_id, buf->packet_type, buf->no, sum);
                }
            }
            break;

        case PACKET_POSITION_DATA:   /* 发送内存队列中的定位包 */
            {
                vec_queue_tmp = vec_send_position_queue;
#if defined(__GT420D__)
				if(KAL_FALSE == track_cust_get_wifidata_send_sign(99))
          		{
               		track_cust_get_wifidata_send_sign(1);
                }
#endif
        }
            break;

        case PACKET_NET_LOG_DATA:   /* 发送网络日志包 */
            {
                vec_queue_tmp = vec_send_log_queue;
            }
            break;
    }
    if(vec_queue_tmp != NULL)
    {
        kal_uint8 *p = data;
        int v_size, sum = 0;
        v_size = VECTOR_Size(vec_queue_tmp);
        if(batchCount <= v_size)
        {
            v_size = batchCount;
        }
        for(i = 0; i < v_size; i++)
        {
            buf = VECTOR_At(vec_queue_tmp, i);
            if(buf)
            {
                if((p + buf->len - data) > SOC_SOCKET_SEND_DATA_MAX)
                {
                    LOGD(L_SOC, L_V5, "WARN: 拼包数据过长，无法满足设定的拼包个数，截断。");
                    break;
                }
                buf->lock = 1;
#if defined(__JM66__)
                if(buf->buffer[31] == TR_CUST_POSITION_POWER_ON)
                {
                    if(Jm66_Current_mode == 2)
                    {
                        extern void track_cust_jm66_shutdown(void);
                        LOGD(L_APP, L_V5, "按键唤醒成功发送定位点,若没有收到指令则2分钟后关机");
                        tr_start_timer(TRACK_CUST_KEY_DOWN_DELAY_SHUTDOWN_TIMER_ID, 120 * 1000, track_cust_jm66_shutdown);
                    }
                    else
                    {
                        LOGD(L_APP, L_V5, "正常模式成功发送定位点");
                        track_cust_IsAlreadySendPosition(1);
                    }
                }
#endif
                memcpy(p, buf->buffer, buf->len);
#if defined (__GW100__)
                LOGH(L_SOC, L_V8, p, buf->len);
#else
                LOGH(L_SOC, L_V6, p, buf->len);
#endif /* __GW1--__ */
                p += buf->len;
                sum++;
            }
            else
            {
                LOGD(L_SOC, L_V1, "ERROR3: vec_send_netcmd_queue");
                VECTOR_Erase(vec_queue_tmp, i, 1);
                break;
            }
        }
        len = p - data;
        if(len > 0)
        {
            LOGH2(L_APP,L_V5,"data:",data,len);
            return Socket_write_req(0, data, len, queue_id, buf->packet_type, buf->no, sum);
        }
    }
    return -1;
}
/******************************************************************************
 *  Function    -  track_soc_send
 *
 *  Purpose     -  发送结果回调及自动继续发送处理
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_soc_send(void *par)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    static kal_uint32 send_alarm_last_time = 0;

	//flag = KAL_TRUE 允许发送
    static kal_bool flag = KAL_TRUE, alarm_flag = KAL_TRUE;
    vec_gprs_socket_send_struct *buf = NULL;
    kal_uint32 parameter = (kal_uint32)par;
    kal_uint32 time = 0;
    int v_size_alarm, v_size_position, v_size_nvram, v_size_log, v_size_netcmd, ret, i;
    kal_uint8 batchCount = 1;
    kal_uint8 gps_status = track_cust_gps_status();
    kal_bool up = track_cust_upload_status(), msgstatus = track_cust_message_status();
    kal_uint8 gropsw=G_parameter.group_package.sw;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOGD(L_SOC, L_V8, "parameter=%d, connstatus=%d, alarm_flag=%d, batch_paket=%d", parameter, connstatus, alarm_flag, batch_paket);
    if(parameter == 99)
    {
        flag = KAL_TRUE;
        return;
    }
    else if(parameter == 98)
    {
        alarm_flag = KAL_TRUE;
        return;
    }
#if defined(__GT740__)
		if(G_parameter.extchip.mode != 1)
		{
			gropsw=0;//模式二屏蔽打包上传
		}
#endif
#ifdef __GT02__
    if((track_get_server_ip_status() == 0) && (G_importance_parameter_data.dserver.server_type == 1))
    {
        return;
    }
#endif /* __GT02__ */

    v_size_alarm = VECTOR_Size(vec_send_alarm_queue);
    v_size_position = VECTOR_Size(vec_send_position_queue);
    v_size_log = VECTOR_Size(vec_send_log_queue);
    v_size_netcmd = VECTOR_Size(vec_send_netcmd_queue);
    v_size_nvram = track_cust_backup_data_get_total_in_queue();
    LOGD(L_SOC, L_V5, "flag=%d, v_size_alarm=%d, v_size_nvram=%d, v_size_position=%d, v_size_netcmd=%d, v_size_log=%d, parameter=%d, connstatus=%d",
         flag, v_size_alarm, v_size_nvram, v_size_position, v_size_netcmd, v_size_log, parameter, connstatus);

    if(!connstatus)
    {
        LOGD(L_SOC, L_V5,  "connstatus=%d", connstatus);
        return;
    }

    if(parameter == PACKET_HEARTBEAT_DATA)
    {
        /* 收到成功送出上一个心跳包的确认 */
        flag = KAL_TRUE;
    }
    else if(parameter == PACKET_LBS_DATA)
    {
        /* 收到成功送出上一个LBS包的确认 */
        flag = KAL_TRUE;
    }
    else if(parameter == PACKET_ALARM_DATA)
    {
        /* 收到成功送出上一个报警包的确认 */
        if(v_size_alarm)
        {
            LOGD(L_SOC, L_V3, "vec_send_alarm_queue %d", v_size_alarm);
            VECTOR_Erase2(vec_send_alarm_queue, 0, batchCount_for_del, KAL_FALSE);
            v_size_alarm = VECTOR_Size(vec_send_alarm_queue);
            LOGD(L_SOC, L_V3, "vec_send_alarm_queue %d", v_size_alarm);
        }
        flag = KAL_TRUE;
    }
    else if(parameter == PACKET_NET_CMD_DATA)
    {
        /* 收到成功送出上一个在线指令包的确认 */
        if(v_size_netcmd)
        {
            VECTOR_Erase2(vec_send_netcmd_queue, 0, batchCount_for_del, KAL_FALSE);
            v_size_netcmd = VECTOR_Size(vec_send_netcmd_queue);
        }
        flag = KAL_TRUE;
    }
    else if(parameter == PACKET_NVRAM_BACKUP_DATA)
    {
		/* 收到成功送出上一个在NVRAM中备份数据的确认 */
        if(v_size_nvram)
        {
            backup_data_del_out_queue(batchCount_for_del);
            v_size_nvram = track_cust_backup_data_get_total_in_queue();
        }
        flag = KAL_TRUE;
#if defined(__GT740__)
        if(track_cust_modeule_get_wakemode() ==wk_supplements && v_size_nvram == 0)//补传日志
        {
            if(track_cust_ota_update(99) == 0)
            {
                LOGD(L_OBD, L_V4, "Supplements data sucessfully, Shutdown!!!!");
                if(G_realtime_data.ams_log==1)
                {
                    char buf[100]={0};
                    int rssi= track_cust_get_rssi(-1);
                    applib_time_struct t = {0};
                    applib_dt_get_rtc_time(&t);
                    sprintf(buf,"(J)Sup %0.2d-%0.2d-%0.2d %0.2d:%0.2d ",t.nYear, t.nMonth, t.nDay, t.nHour, t.nMin);
                    sprintf(buf+strlen(buf),"\n");
                    track_cust_cache_sucssful_log(buf,strlen(buf));
                    track_cust_netlog_cache(1);
                }            
            }
            track_cust_module_delay_close(5);
         }
	  else if(v_size_nvram == 0 && G_realtime_data.nw_sign && mmi_Get_PowerOn_Type() == ABNRESET)//reset主动重启
         {
            if(track_cust_modeule_get_wakemode() ==wk_normal)
            {
                 char netlog_buf[50]={0};
                int rssi= track_cust_get_rssi(-1),lbs_cmp=0;
                LBS_Multi_Cell_Data_Struct *lbs_strcut = track_drv_get_lbs_data();
                applib_time_struct t = {0};
                if(G_realtime_data.MainCellId != lbs_strcut->MainCell_Info.cell_id && (G_realtime_data.MainCellId !=0) )
                {
                    lbs_cmp=1;
                }
                applib_dt_get_rtc_time(&t);
                sprintf(netlog_buf,"(J)(%d)lbs change?==%d,%d",RUNING_TIME_SEC,lbs_cmp,G_realtime_data.nw_sign);
                sprintf(netlog_buf+strlen(netlog_buf),"\n");
                track_cust_cache_sucssful_log(netlog_buf,strlen(netlog_buf));
                track_cust_netlog_cache(1);
                track_cust_module_delay_close(15);
            }
            G_realtime_data.nw_sign = 0;
            G_realtime_data.MainCellId = 0;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
         }
#endif
    }
    else if(parameter == PACKET_POSITION_DATA)
    {
		if(v_size_position)
        {
            buf = VECTOR_At(vec_send_position_queue, 0);
            VECTOR_Erase2(vec_send_position_queue, 0, batchCount_for_del, KAL_FALSE);
            v_size_position = VECTOR_Size(vec_send_position_queue);
        }
        flag = KAL_TRUE;
        #if defined(__GT420D__)
            if(G_parameter.extchip.mode == 1)
   	        {
                LOGD(L_APP,L_V5,"收到定位数据确认，延迟30S关机");
                if(track_cust_need_check_sportorstatic(99) == 1)
                {
                LOGD(L_APP,L_V5,"检测到运动转静止下需要检测速度，暂时不关机");
                return;
                }
                if(track_cust_module_alarm_lock(98, NULL)==1)
                  {
                    LOGD(L_APP,L_V5,"还有报警未处理不关机");
         
                  return;
                   }
                track_cust_module_delay_close(30);
                    }
    #endif
#if defined(__GT740__)||defined(__GT420D__)
        if(track_cust_ota_update(99) == 0)
        {
            LOGD(L_OBD, L_V4, "gps_status=%d,bootsendlbs=%d,track_cust_upload_status=%d,track_cust_message_status()=%d ,%d", gps_status,bootsendlbs,up,msgstatus,G_parameter.new_position_sw);
            if(G_parameter.extchip.mode != 1 && !track_is_testmode() && !track_is_autotestmode())
            {
                if(G_realtime_data.nw_sign>0)//防止异常时nw_sign未清除
                {
                    G_realtime_data.nw_sign=0;
                    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                }
                if(track_cust_upload_status())
                {
					track_cust_shutdown_condition((void*)GPS_FIXED);
                }
                else
                {
					track_cust_shutdown_condition((void*)POS_UPED);
                }
            }
	     	track_project_save_switch(1,sw_position_up);
        }
#endif /* __GT740__ */

    }
    else if(parameter == PACKET_NET_LOG_DATA)
    {
        /* 收到成功送出上一个网络日志包的确认 */
        if(v_size_log)
        {
            VECTOR_Erase2(vec_send_log_queue, 0, batchCount_for_del, KAL_FALSE);
            v_size_log = VECTOR_Size(vec_send_log_queue);
        }
        flag = KAL_TRUE;
    }
    LOGD(L_SOC, L_V5, "flag=%d", flag);

    track_stop_timer(TRACK_SOC_SEND_TIMER_ID);
    if(v_size_alarm == 0
            && v_size_position == 0
            && v_size_nvram == 0
            && v_size_log == 0
            && v_size_netcmd == 0
            && !heartbeat_packets_need_send
            && !LBS_packets_need_send)
    {
    
#if defined(__SERVER_IP_LINK__)
		tr_start_timer(TRACK_CUST_LINK_DOWN_TIMER_ID, 30*1000, track_soc_disconnect);
		LOGD(L_SOC, L_V5, "无数据发送，关闭连接");
#endif
        /* 当前队列中无待发送的数据 */
        LOGD(L_SOC, L_V5, "return");
        return;
    }
    if(!track_soc_login_status()) /* 是否已经登录成功 */
    {
        track_start_timer(TRACK_SOC_SEND_TIMER_ID, 2000, track_soc_send, (void*)9);
        return;
    }
#if !defined(__GT530__)
    if(Socket_get_write_status(0)) /* Socket是否正在写? */
    {
        track_start_timer(TRACK_SOC_SEND_TIMER_ID, 2000, track_soc_send, (void*)9);
        LOGD(L_SOC, L_V5, "Socket_get_write_status");
        return;
    }
#endif
    if(!flag)
    {
        LOGD(L_SOC, L_V5, "已发送数据，还未有接收的发送成功的确认");
        track_start_timer(TRACK_SOC_SEND_TIMER_ID, 2000, track_soc_send, (void*)9);
        return;
    }

    time = kal_get_systicks();
    flag = KAL_FALSE;
    if(heartbeat_packets_need_send)
    {
        /* 发送心跳包 */
        LOGD(L_SOC, L_V5, "heartbeat_packets_need_send");
        heartbeat_packets((void *)1);
        if(track_soc_get_send_data(PACKET_HEARTBEAT_DATA, batch_paket) < 0)
        {
            LOGD(L_SOC, L_V1, "ERROR2: %d", PACKET_ALARM_DATA);
            flag = KAL_TRUE;
        }
#ifdef __GT420D__
		//if(G_parameter.extchip.mode == 1 && G_parameter.extchip.mode1_par1 > 5 || G_parameter.extchip.mode == 2 )
	  	//{
	        //track_start_timer(TRACK_CUST_HEARTBEAT_BACK_RECV_OVERTIME_TIMER, 10000, track_cust_module_delay_close, (void *)30);
			heartback_send_flag = KAL_FALSE;
	    //}
#endif
#if defined(__XCWS__)
        flag = KAL_TRUE;
#endif
    }
	else if(LBS_packets_need_send)
    {
        /* LBS指令触发立即发送LBS包 */
	    static U8 timecount=0;
        LOGD(L_SOC, L_V5, "8A_packets_need_send");
        if(track_cust_paket_8A() < 0)
        {
			timecount++;
            LOGD(L_SOC, L_V1, "ERROR2: %d", PACKET_LBS_DATA);
            flag = KAL_TRUE;
        }
		else
		{
			LBS_packets_need_send = KAL_FALSE;
		}
		if(timecount>=3)
		{
			LBS_packets_need_send = KAL_FALSE;
		}
    }
    else if(v_size_nvram)
    {
        /* 发送NVRAM备份的定位数据包 */
        kal_int8 ret;
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
        if(G_parameter.group_package.sw == 0 || track_is_timer_run(TRACK_CUST_FT_WORK_TIMER_ID) || G_parameter.work_mode.mode != WORK_MODE_2)
        {
            ret = track_soc_get_send_data(PACKET_NVRAM_BACKUP_DATA, batch_paket);
        }
        else
        {
            ret = track_soc_get_send_data(PACKET_NVRAM_BACKUP_DATA, G_parameter.group_package.at_least_pack);
        }
#else
        if(G_parameter.group_package.sw == 0 || track_is_timer_run(TRACK_CUST_FT_WORK_TIMER_ID))
        {
            ret = track_soc_get_send_data(PACKET_NVRAM_BACKUP_DATA, batch_paket);
        }
        else
        {
            ret = track_soc_get_send_data(PACKET_NVRAM_BACKUP_DATA, G_parameter.group_package.at_least_pack);
        }
#endif /* __GT300S__ */
        if(ret <= 0)
        {
            LOGD(L_SOC, L_V1, "ERROR5: %d", PACKET_NVRAM_BACKUP_DATA);
            flag = KAL_TRUE;
            track_start_timer(TRACK_SOC_SEND_TIMER_ID, 2000, track_soc_send, (void*)9);
        }
    }
    else if(v_size_alarm)
    {
        /* 发送报警包 */
        if(track_soc_get_send_data(PACKET_ALARM_DATA, batch_paket) >= 0)
        {

        }
        else
        {
        	LOGD(L_SOC, L_V1, "ERROR3: %d", PACKET_ALARM_DATA);
        	flag = KAL_TRUE;
        }
    }
    else if(v_size_netcmd)
    {
        /* 发送在线指令包 */
        if(track_soc_get_send_data(PACKET_NET_CMD_DATA, batch_paket) < 0)
        {
            LOGD(L_SOC, L_V1, "ERROR4: %d", PACKET_NET_CMD_DATA);
                flag = KAL_TRUE;
        }
    }
#if defined(__GT740__)||defined(__GT420D__)
	else if(v_size_position && gropsw==0)
#else
    else if(v_size_position && (G_parameter.group_package.sw == 0 || track_is_timer_run(TRACK_CUST_FT_WORK_TIMER_ID)))
#endif
    {
        /* 发送内存队列中的定位包 */
        if(track_soc_get_send_data(PACKET_POSITION_DATA, batch_paket) < 0)
        {
            LOGD(L_SOC, L_V1, "ERROR6: %d", PACKET_POSITION_DATA);
            flag = KAL_TRUE;
        }
    }
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
    else if(G_parameter.group_package.sw != 0 && (v_size_position >= G_parameter.group_package.at_least_pack
            || (v_size_position && (gps_status < GPS_STAUS_2D
                                    || (gps_status >= GPS_STAUS_2D && !track_cust_sensor_delay_valid())))
            || G_parameter.work_mode.mode != WORK_MODE_2
            || track_is_updata(0xff) == 1))
#else
    else if(G_parameter.group_package.sw != 0
            && (v_size_position >= G_parameter.group_package.at_least_pack
                || (v_size_position && (gps_status < GPS_STAUS_2D
                                        || (gps_status >= GPS_STAUS_2D && !track_cust_sensor_delay_valid())
                                        || Instant_Upload))
               ))
#endif /* __GT300S__ */
    {
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
        track_is_updata(0);
#endif /* __GT300S__ */
        /* 发送内存队列中的定位包 */
        if(track_soc_get_send_data(PACKET_POSITION_DATA, G_parameter.group_package.at_least_pack) < 0)
        {
            LOGD(L_SOC, L_V1, "ERROR7: %d", PACKET_POSITION_DATA);
            flag = KAL_TRUE;
        }
        else
        {
            Instant_Upload = KAL_FALSE;
        }
    }
    else if(v_size_log)
    {
        /* 发送网络日志包 */
        if(track_soc_get_send_data(PACKET_NET_LOG_DATA, batch_paket) < 0)
        {
            LOGD(L_SOC, L_V1, "ERROR8: %d", PACKET_NET_LOG_DATA);
            flag = KAL_TRUE;
        }
    }
    else
    {
        LOGD(L_SOC, L_V1, "ERROR9: ?");
        flag = KAL_TRUE;
        track_start_timer(TRACK_SOC_SEND_TIMER_ID, 2000, track_soc_send, (void*)9);
    }
}
#endif
/*   T R A C K _ S O C _ L B S _ P A C K E T S _ N E E D _ S E N D   */
/*-------------------------------------------------------------------------
    触发立即发送一个LBS数据包
-------------------------------------------------------------------------*/
void track_soc_8A_packets_need_send(void)
{
    LBS_packets_need_send = KAL_TRUE;
    track_os_intoTaskMsgQueueExt(track_soc_send, (void*)9);
}

/******************************************************************************
 *  Function    -  track_soc_lcon_send_sensor
 *
 *  Purpose     -  由运动变为静止后发送剩余的数据
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_soc_lcon_send_sensor(void)
{
    if(0 < VECTOR_Size(vec_send_position_queue))
    {
        track_start_timer(TRACK_SOC_SEND_TIMER_ID, 2000, track_soc_send, (void*)9);
    }
}

/******************************************************************************
 *  Function    -  VECTOR_Erase2
 *
 *  Purpose     -  擦除队列中的数据
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void VECTOR_Erase2(c_vector* obj, kal_uint32 pos_start, kal_uint32 num, kal_bool lock_is_valid)
{
    int i;
    vec_gprs_socket_send_struct *buf = NULL;
    while(num > 0)
    {
        buf = (vec_gprs_socket_send_struct*)VECTOR_At(obj, pos_start);
        if(buf)
        {
            LOGD(L_APP, L_V3, "lock %d , num %d", buf->lock, num);
            if(!lock_is_valid || buf->lock == 0)
            {
                VECTOR_Erase(obj, pos_start, 1);
                Ram_Free(buf);
            }
            else
            {
                pos_start++;
            }
        }
        num--;
    }
}

/******************************************************************************
 *  Function    -  track_socket_queue_init
 *
 *  Purpose     -  SOCKET发送队列初始化
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_socket_queue_init(void)
{
    LOGD(L_SOC, L_V5, "");
    if(NULL == vec_send_alarm_queue)
    {
        VECTOR_CreateInstance(vec_send_alarm_queue);
    }
    if(NULL == vec_send_position_queue)
    {
        VECTOR_CreateInstance(vec_send_position_queue);
    }
    if(NULL == vec_send_log_queue)
    {
        VECTOR_CreateInstance(vec_send_log_queue);
    }
    if(NULL == vec_send_netcmd_queue)
    {
        VECTOR_CreateInstance(vec_send_netcmd_queue);
    }
#if defined(__XCWS_TPATH__)
    track_cust_server3_queue_init();
#endif
#if defined(__MCU_LONG_STANDBY_PROTOCOL__) || defined(__JM66__)
    if(NULL == vec_send_mcu_queue)
    {
        VECTOR_CreateInstance(vec_send_mcu_queue);
    }
#endif//__MCU_LONG_STANDBY_PROTOCOL__

}

/******************************************************************************
 *  Function    -  track_socket_queue_alarm_send_reg
 *
 *  Purpose     -  报警数据发送请求
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_socket_queue_alarm_send_reg(kal_uint8* data, int len, kal_uint8 packet_type, kal_uint32 no)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16 ret;
    vec_gprs_socket_send_struct* buf;
    int v_size;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOGD(L_SOC, L_V5,  "data_len=%d, packet_type=%X, connstatus=%d", len, (U8)packet_type, connstatus);

#if defined (__NETWORK_LICENSE__)
    return;
#endif /* __NETWORK_LICENSE__ */
    if(len > MAX_SND_BUFFER_SIZE || len <= 0)
    {
        LOGD(L_SOC, L_V5,  "数据长度超出范围");
        return;
    }
    else
    {
        v_size = VECTOR_Size(vec_send_alarm_queue);
        if(v_size >= 30)
        {
            LOGD(L_SOC, L_V1, "内存报警队列数值已经满%d条，删除早期数据，保留最新数据!!!", v_size);
            VECTOR_Erase2(vec_send_alarm_queue, 0, 15, KAL_TRUE);
        }
        buf = (vec_gprs_socket_send_struct*)Ram_Alloc(3, sizeof(vec_gprs_socket_send_struct));
        if(buf)
        {
            buf->status = 0;
            memcpy(buf->buffer , data , len);
            buf->len = len;
            buf->no = no;
            buf->packet_type = packet_type;
            VECTOR_PushBack(vec_send_alarm_queue, buf);
        }
        else
        {
            LOGD(L_SOC, L_V1, "Ram_Alloc 申请临时内存空间失败!!!");
            track_cust_restart(22, 0);
        }
        track_soc_send((void*)9);
    }
}

/******************************************************************************
 *  Function    -  track_socket_queue_position_send_reg
 *
 *  Purpose     -  轨迹数据发送请求
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_socket_queue_position_send_reg(U8* data, int len, kal_uint8 packet_type, kal_uint32 no)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    vec_gprs_socket_send_struct* buf;
    U16 ret;
    char buf1[100] = {0};
    int v_size, v_size_nvram;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

#if defined( __NULL_SERVER__)
    kal_uint8 i_url = strlen((char*)track_nvram_alone_parameter_read()->server.url);
    LOGD(L_APP, L_V5, "%d,空域名时不连网不备份", i_url);
    if(0 == i_url)
    {
        return;
    }
#endif /* __NULL_SERVER__ */
        #if defined(__GT420D__)
        snprintf(buf1, 99, "1\n\r");
        track_cust_save_logfile(buf1, strlen(buf1));
        #endif
    v_size = VECTOR_Size(vec_send_position_queue);
    LOGD(L_SOC, L_V5,  "VECTOR:%d, data_len=%d", v_size, len);
    if(track_soc_login_status())
    {
        v_size_nvram = track_cust_backup_data_get_total_in_queue();
        if(v_size_nvram > 0 && v_size >= 60)
        {
            VECTOR_Erase2(vec_send_position_queue, 0, 2, KAL_TRUE);
            v_size = VECTOR_Size(vec_send_position_queue);
            LOGD(L_SOC, L_V1, "v_size_nvram=%d, 内存定位队列数值已经满(%d)，删除早期数据，保留最新数据!!!", v_size_nvram, v_size);
        }
        else if(v_size_nvram <= 0 && v_size >= 55)
        {
            track_soc_bakup_data((void*)3);
            v_size = VECTOR_Size(vec_send_position_queue);
        }
    }
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
    else if(v_size >= 1 && (G_parameter.work_mode.mode == 3))
    {
        LOGD(L_SOC, L_V1, "v_size=%d, 未联网,保存定位点到NVRAM!!!", v_size);
        track_soc_bakup_data((void*)3);
        v_size = VECTOR_Size(vec_send_position_queue);
    }
#endif
    else if(v_size >= 50)
    {
        track_soc_bakup_data((void*)3);
        v_size = VECTOR_Size(vec_send_position_queue);
    }
    LOGD(L_SOC, L_V5,  "bak VECTOR:%d, data_len=%d", v_size, len);
    #if defined(__GT420D__)
        snprintf(buf1, 99, "2\n\r");
        track_cust_save_logfile(buf1, strlen(buf1));
        #endif
    if(len > MAX_SND_BUFFER_SIZE || len <= 0)
    {
        LOGD(L_SOC, L_V5,  "数据长度超出范围");
        return;
    }
    else
    {
        if(v_size >= 60)
        {
            LOGD(L_SOC, L_V1, "缓存队列已满 %d", v_size);
            track_cust_restart(26, 0);
        }
        #if defined(__GT420D__)
        snprintf(buf1, 99, "3\n\r");
        track_cust_save_logfile(buf1, strlen(buf1));
        #endif
        buf = (vec_gprs_socket_send_struct*)Ram_Alloc(3, sizeof(vec_gprs_socket_send_struct));
        if(buf)
        {
            buf->status = 0;
            buf->lock = 0;
            memcpy(buf->buffer , data , len);
            buf->len = len;
            buf->no = no;
            buf->packet_type = packet_type;
            VECTOR_PushBack(vec_send_position_queue, buf);
        }
        else
        {
            LOGD(L_SOC, L_V1, "Ram_Alloc 申请临时内存空间失败!!!");
            track_cust_restart(23, 0);
        }
        #if defined(__GT420D__)
        snprintf(buf1, 99, "4\n\r");
        track_cust_save_logfile(buf1, strlen(buf1));
        #endif
        track_soc_send((void*)9);
    }
}

/******************************************************************************
 *  Function    -  track_socket_queue_send_log_reg
 *
 *  Purpose     -  网络日志发送请求
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_socket_queue_send_log_reg(kal_uint8* data, int len, kal_uint8 packet_type, kal_uint32 no)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16 ret;
    vec_gprs_socket_send_struct* buf;
    int v_size;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

#if defined (__NETWORK_LICENSE__)
    return;
#endif /* __NETWORK_LICENSE__ */
    LOGD(L_SOC, L_V5,  "data_len=%d, packet_type=%X, connstatus=%d", len, (U8)packet_type, connstatus);
    if(len > MAX_SND_BUFFER_SIZE || len <= 0)
    {
        LOGD(L_SOC, L_V5,  "数据长度超出范围");
        return;
    }
    else
    {
        v_size = VECTOR_Size(vec_send_log_queue);
        if(v_size >= 30)
        {
            LOGD(L_SOC, L_V5, "内存中的日志队列已达到%d条记录，删除早期数据，保留最新数据!!!");
            VECTOR_Erase2(vec_send_log_queue, 0, 20, KAL_TRUE);
        }
        if(v_size >= 40)
        {
            LOGD(L_SOC, L_V1, "缓存队列已满 %d", v_size);
            track_cust_restart(25, 0);
        }
        buf = (vec_gprs_socket_send_struct*)Ram_Alloc(3, sizeof(vec_gprs_socket_send_struct));
        if(buf)
        {
            buf->status = 0;
            memcpy(buf->buffer , data , len);
            buf->len = len;
            buf->no = no;
            buf->packet_type = packet_type;
            VECTOR_PushBack(vec_send_log_queue, buf);
        }
        else
        {
            LOGD(L_SOC, L_V1, "Ram_Alloc 申请临时内存空间失败!!!");
            track_cust_restart(22, 0);
        }
        track_soc_send((void*)9);
    }
}

/******************************************************************************
 *  Function    -  track_socket_queue_send_netcmd_reg
 *
 *  Purpose     -  在线指令发送请求
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_socket_queue_send_netcmd_reg(kal_uint8* data, int len, kal_uint8 packet_type, kal_uint32 no)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16 ret;
    vec_gprs_socket_send_struct* buf;
    int v_size;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOGD(L_SOC, L_V5,  "data_len=%d, packet_type=%X, connstatus=%d", len, (U8)packet_type, connstatus);
    if(len > MAX_SND_BUFFER_SIZE || len <= 0)
    {
        LOGD(L_SOC, L_V5,  "数据长度超出范围");
        return;
    }
    else
    {
        v_size = VECTOR_Size(vec_send_netcmd_queue);
        if(v_size >= 30)
        {
            LOGD(L_SOC, L_V5, "内存中的在线指令队列已达到30条记录，删除早期数据，保留最新数据!!!");
            VECTOR_Erase2(vec_send_netcmd_queue, 0, 20, KAL_TRUE);
        }
        if(v_size >= 40)
        {
            LOGD(L_SOC, L_V1, "缓存队列已满 %d", v_size);
            track_cust_restart(25, 0);
        }
        buf = (vec_gprs_socket_send_struct*)Ram_Alloc(3, sizeof(vec_gprs_socket_send_struct));
        if(buf)
        {
            buf->status = 0;
            memcpy(buf->buffer , data , len);
            buf->len = len;
            buf->no = no;
            buf->packet_type = packet_type;
            VECTOR_PushBack(vec_send_netcmd_queue, buf);
        }
        else
        {
            LOGD(L_SOC, L_V1, "Ram_Alloc 申请临时内存空间失败!!!");
            track_cust_restart(22, 0);
        }
        track_soc_send((void*)9);
    }
}

/******************************************************************************
 *  Function    -  queue_all_to_nvram
 *
 *  Purpose     -  将备份队列中的所有数据写入NVRAM
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 21-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_soc_queue_all_to_nvram(void)
{
    vec_gprs_socket_send_struct* buf;
    BACKUP_GPS_DATA databuf;
    int i = 0, j = 0, sum;
    if(vec_send_position_queue == NULL)
    {
        return;
    }
    sum = VECTOR_Size(vec_send_position_queue);
    LOGD(L_SOC, L_V5, "nvarm_total:%d, queue_ext:%d", track_cust_backup_data_get_total_in_queue(), sum);
    LOGD(L_APP,L_V5,"我给你保存队列里没有上传的点，队列里有%d 个包",sum);

    if(sum <= 0)
    {
        LOGD(L_SOC, L_V5, "");
        return;
    }
    buf = VECTOR_At(vec_send_position_queue, j);
    while(buf)
    {
        if(buf->len < BACKUP_DATA_PACKET_MAX)
        {
            /*if(i < 5)
            {
                LOGH(L_SOC, L_V6, buf->buffer, buf->len);
            }*/
            databuf.len = buf->len;
            memcpy(databuf.buf, buf->buffer, databuf.len);
            track_cust_backup_data_add_in_queue(&databuf);
            i++;
            VECTOR_Erase(vec_send_position_queue, j, 1);
            Ram_Free(buf);
            if(i > 10)
            {
                track_os_intoTaskMsgQueue(track_soc_queue_all_to_nvram);//
                break;
            }
        }
        else
        {
            LOGD(L_APP, L_V5, "WARN: len=%d, 数据包长度过长，不备份至NVRAM", buf->len);
            j++;
        }
        buf = VECTOR_At(vec_send_position_queue, j);
    }
    LOGD(L_APP, L_V5, "%d,%d,you nvarm_total:%d, write:%d, queue_ext:%d", i, j, track_cust_backup_data_get_total_in_queue(), i, VECTOR_Size(vec_send_position_queue));
}

/******************************************************************************
 *  Function    -  track_soc_clear_queue
 *
 *  Purpose     -  清除未发送的轨迹数据
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_soc_clear_queue(void)
{
    vec_gprs_socket_send_struct* buf;
    while(track_cust_backup_data_get_total_in_queue())
    {
        backup_data_del_out_queue(BACKUP_DATA_MAX);
    }
    while(VECTOR_Size(vec_send_position_queue))
    {
        buf = VECTOR_At(vec_send_position_queue, 0);
        if(buf)
        {
            Ram_Free(buf);
        }
        else
        {
            LOGD(L_SOC, L_V1, "ERROR2: vec_send_position_queue");
        }
        VECTOR_Erase(vec_send_position_queue, 0, 1);
    }
    track_soc_send((void*)99);
}

/******************************************************************************
 *  Function    -  track_soc_check_position_packets_count
 *
 *  Purpose     -  查询未发送的轨迹点个数
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int32 track_soc_check_position_packets_count(void)
{
    vec_gprs_socket_send_struct* buf;
    int j = 0, count = 0, nvarm_total;
    nvarm_total = track_cust_backup_data_get_total_in_queue();
    buf = VECTOR_At(vec_send_position_queue, j);
    while(buf)
    {
        if(buf->len == 36)
        {
            count++;
        }
        j++;
        buf = VECTOR_At(vec_send_position_queue, j);
    }
    LOGD(L_SOC, L_V5, "nvarm_total:%d, ram_queue_ext:%d,%d", nvarm_total, VECTOR_Size(vec_send_position_queue), count);
    return (count + nvarm_total);
}

/******************************************************************************
 *  Function    -  track_soc_set_batch_paket
 *
 *  Purpose     -  设置批量发送的个数
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_soc_set_batch_paket(kal_uint8 value)
{
    batch_paket = value;
}

/******************************************************************************
 *  Function    -  heartbeat_packets
 *
 *  Purpose     -  心跳包周期发送逻辑
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void heartbeat_packets(void *arg)
{
    LOGD(L_APP, L_V5, "[%d] acc=%d;hbt_acc_on=%d,hbt_acc_off=%d", (U32)arg, track_cust_status_acc(), G_parameter.hbt_acc_on, G_parameter.hbt_acc_off);

#if defined (__GT740__)||defined(__GT420D__)
#ifdef __GT741__
     if(G_parameter.extchip.mode==1 && RUNING_TIME_SEC > 180)
     {
	 	track_cust_module_Handshake();
	 }
#endif
     #if !defined (__GT420D__)
     if(track_cust_get_battery_volt()==0 || track_is_timer_run(TRACK_CUST_EXTCHIP_DELAY_CLOSE_TIMER_ID))
     {
         track_start_timer(TRACK_CUST_NET_HEARTBEAT_TIMER_ID, 10000, heartbeat_packets,(void *)arg);
         return;
     }
     #endif
#endif
    if((U32)arg == 0)
    {
        heartbeat_packets_need_send = KAL_FALSE;
        track_stop_timer(TRACK_CUST_NET_HEARTBEAT_TIMER_ID);
        track_start_timer(TRACK_CUST_NET_HEARTBEAT_OVER_TIMER_ID, G_parameter.hbt_acc_on * 2000, track_soc_gprs_reconnect, (void *)27);
        return;
    }
    else if((U32)arg >= 100)
    {
        heartbeat_packets_need_send = KAL_TRUE;
        track_os_intoTaskMsgQueueExt(track_soc_send, (void *)9);
    }
    else
    {
        heartbeat_packets_need_send = KAL_FALSE;
    }
    
    if(track_cust_status_acc() == 1)
    {
        if(G_parameter.hbt_acc_on > 0)
            track_start_timer(TRACK_CUST_NET_HEARTBEAT_TIMER_ID, G_parameter.hbt_acc_on * 1000, heartbeat_packets, (void *)105);
        if(!track_is_timer_run(TRACK_CUST_NET_HEARTBEAT_OVER_TIMER_ID))
    #if defined(__GT530__)
            track_start_timer(TRACK_CUST_NET_HEARTBEAT_OVER_TIMER_ID, 20000, track_soc_gprs_reconnect, (void *)4);
    #else
            track_start_timer(TRACK_CUST_NET_HEARTBEAT_OVER_TIMER_ID, G_parameter.hbt_acc_on * 2000, track_soc_gprs_reconnect, (void *)27);
    #endif
    }
    else
    {
        if(G_parameter.hbt_acc_off > 0)
            track_start_timer(TRACK_CUST_NET_HEARTBEAT_TIMER_ID, G_parameter.hbt_acc_off * 1000, heartbeat_packets, (void *)106);
        if(!track_is_timer_run(TRACK_CUST_NET_HEARTBEAT_OVER_TIMER_ID))
    #if defined(__GT530__)
            track_start_timer(TRACK_CUST_NET_HEARTBEAT_OVER_TIMER_ID, 20000, track_soc_gprs_reconnect, (void *)4);
    #else
            track_start_timer(TRACK_CUST_NET_HEARTBEAT_OVER_TIMER_ID, G_parameter.hbt_acc_off * 2000, track_soc_gprs_reconnect, (void *)28);
    #endif
    }
}
/******************************************************************************
 *  Function    -  information_packets
 *
 *  Purpose     -  0X94包发送周期逻辑(几米国际，外接电源电压信息上传包)
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 23-07-2015,  Cjj  written
 * ----------------------------------------
*******************************************************************************/
void information_packets(void)
{
    if(G_parameter.info_up.sw != 1)
    {
        return;
    }
    if(track_cust_status_charger() > 0)
    {
        //  if(G_parameter.info_up.time > 0)
        tr_start_timer(TRACK_CUST_NET_INFO_UP_TIMER_ID, G_parameter.info_up.time * 1000, information_packets);
        track_cust_paket_94(0, NULL, 2);

    }
    else
    {
        /*没有接外电，暂无特殊处理*/
        //    if(G_parameter.info_up.time > 0)
        tr_start_timer(TRACK_CUST_NET_INFO_UP_TIMER_ID, G_parameter.info_up.time * 1000, information_packets);
        track_cust_paket_94(0, NULL, 2);

    }
}
/******************************************************************************
 *  Function    -  track_test_view_queue
 *
 *  Purpose     -  查看各发送队列的剩余数量
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_test_view_queue(char *rsp_msg)
{
    sprintf(rsp_msg, "nvarm total:%d, alarm queue:%d, position queue:%d, netcmd_queue:%d, netlog queue:%d\r\n",
            track_cust_backup_data_get_total_in_queue(),
            VECTOR_Size(vec_send_alarm_queue),
            VECTOR_Size(vec_send_position_queue),
            VECTOR_Size(vec_send_netcmd_queue),
            VECTOR_Size(vec_send_log_queue));
}

/******************************************************************************
 *  Function    -  track_test_ram
 *
 *  Purpose     -  擦除指定个数的待发送的轨迹点
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_test_ram(char *outmsg, kal_uint16 value1, kal_uint16 value2)
{
    Ram_view(outmsg);
    //VECTOR_Erase(vec_send_position_queue, value1, value2);
    VECTOR_Erase2(vec_send_position_queue, value1, value2, KAL_FALSE);
    Ram_view(outmsg + strlen(outmsg));
}


void track_soc_clear_dw_queue(void)
{
    vec_gprs_socket_send_struct* buf;
    LOGD(L_APP, L_V6, "@@ size:%d", VECTOR_Size(vec_send_alarm_queue));

    while(VECTOR_Size(vec_send_alarm_queue))
    {
        buf = VECTOR_At(vec_send_alarm_queue, 0);
        if(buf)
        {
            Ram_Free(buf);
        }
        else
        {
            LOGD(L_APP, L_V6, "ERROR2: vec_send_position_queue");
        }
        VECTOR_Erase(vec_send_alarm_queue, 0, 1);
    }
}

/******************************************************************************
 *  Function    -  track_soc_instant_upload_position_status
 *
 *  Purpose     -  ACC状态变化触发的定位数据、GPS由定位转换为不定
 *   位状态、GPS进入休眠前,需要立即上传已有的定位数据
 *
 *  Description -1.避免不满足打包条件，熄火后一直看不到定位数据的问题。
 *                      2.避免GPS常开时GPS不定位
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 27-11-2015,  Cjj  written
 * ----------------------------------------
*******************************************************************************/
void track_soc_instant_upload_position_status()
{
    Instant_Upload = KAL_TRUE;
}

int  track_soc_position_clr()
{
	vec_gprs_socket_send_struct *buf = NULL;
	int v_size_position;
	v_size_position = VECTOR_Size(vec_send_position_queue);
	if(v_size_position)
	{
		buf = VECTOR_At(vec_send_position_queue, 0);
		VECTOR_Erase2(vec_send_position_queue, 0, batchCount_for_del, KAL_FALSE);
		v_size_position = VECTOR_Size(vec_send_position_queue);
	}
	return v_size_position;
}

int track_soc_position_queue()
{
	return VECTOR_Size(vec_send_position_queue);
}
#if defined(__SERVER_IP_LINK__)
int track_soc_get_vector_queue()
{
	if(VECTOR_Size(vec_send_position_queue) || VECTOR_Size(vec_send_alarm_queue) || track_cust_backup_data_get_total_in_queue())
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
#endif
