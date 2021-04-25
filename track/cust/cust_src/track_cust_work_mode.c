/*******************************************************************************************
 * Filename:track_cust_work_mode.c
 * Author :xzq
 * Date :04062017
 * Comment:GT310����ģʽ
 ******************************************************************************************/
#if defined (__GT310__)

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

/****************************************************************************
*  Global Variable - Extern 			����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern			�����ⲿ����
******************************************************************************/
extern void track_cust_mode3_update_LBS(void * f);

/*****************************************************************************
*  Local variable                       �ֲ�����
*****************************************************************************/
static track_enum_sms_staus sms_staus = SMS_NORMAL_MODE; //��¼SMS��״̬������ģʽor ����ģʽ

/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/
void track_cust_fly_mode_change(void* port);
void track_cust_mode3_upload_location(void * f);

/******************************************************************************
 *  Function    -  track_cust_mode3_extchange
 *
 *  Purpose     -  ģʽ�������´λ���ʱ����
 *
 *  Description -  
 *                               
 * starTime_mins :��ʼʱ�����                 
 * interval_mins :���ʱ�����                      
 *
 * return:�´λ���ʱ��������
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 04-07-2017,  xzq 
 * ----------------------------------------
 ******************************************************************************/
U32 track_cust_mode3_extchange(U32 startTime_mins, U16 interval_mins)
{
	int i;
	applib_time_struct currTime = {0};	
    U32 currMins = 0,wkup_time;
	U32 tmp;
	kal_bool find_ok=KAL_FALSE;
	track_fun_get_time(&currTime, KAL_FALSE, &G_parameter.zone);
	if(currTime.nYear == (2000 + (*OTA_DEFAULT_HARDWARE_YEAR)))return 0;
	currMins = currTime.nHour*60+currTime.nMin;
	
	if(interval_mins<=1440)//��?����
	{		
	   if(currMins<=startTime_mins)
	   {
		 tmp=startTime_mins-currMins;
		 wkup_time=tmp%interval_mins;
		 if(wkup_time==0)
		 {
		   wkup_time=interval_mins;
		 }
		 else
		 {
		 }
		 find_ok=KAL_TRUE;	
	   }
	   else
	   {			  
		 for(i=0;i<=288;i++)//
		   {
			 tmp=startTime_mins+interval_mins*i;
			 
			 LOGD(L_APP, L_V9, "s %d, c %d, tmp %d", startTime_mins,currMins,tmp);
			 if(tmp>currMins && (tmp-currMins<interval_mins))
			  {
				wkup_time=tmp-currMins;
				find_ok=KAL_TRUE;
				break;
			  }
		  }
	   }		
	  if(find_ok==KAL_FALSE)
	   {
		 wkup_time=interval_mins;
	   } 

	  LOGD(L_APP, L_V5, "��%dʱ%d�ֺ���.", wkup_time/60,wkup_time%60);
	}

	return wkup_time;
}



U8 track_cust_open_location_mode(U8 par)
{
	if(G_parameter.fixmode.fix_level==GPS_FIX)
    {
		if(G_parameter.fixmode.gps_fix_sw)
		{
			track_cust_gps_work_req((void *)par);
			if(track_cust_get_work_mode()==WORK_MODE_3)
			{
				track_start_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID, 125 * 1000, track_cust_mode3_upload_location, (void *)0);
			}
		}
	}
	else
	{
		if(G_parameter.fixmode.wf_lbs_fix_sw)
		{
			if(G_parameter.wifi.sw == 1)
			{
				track_cust_sendWIFI_paket();
				if(track_cust_get_work_mode() == WORK_MODE_1)
				{
#if defined(__GT310__)
					tr_start_timer(TRACK_CUST_WIFI_WORK_TIMER_ID, G_parameter.work_mode.mode1_time_sec* 1000, track_cust_sendWIFI_paket);//��ʱ�ϴ�
#endif
				}
			}
			else
			{
				track_cust_sendlbs_limit();
			}
		}
	}
	LOGD(L_APP, L_V5, "������λ %d",par);
}

void track_cust_mode1_gpsfix_overtime()
{
    LOGD(L_APP, L_V5, "ģʽ1��λ�ϴ���ʱ,ת����һ����λ����");

	if(G_parameter.fixmode.fix_level==GPS_FIX)
	{
		if(G_parameter.fixmode.wf_lbs_fix_sw)
		{
			if(G_parameter.wifi.sw == 1)
			{
				track_drv_wifi_work_mode();
			}
			else
			{
				track_cust_sendlbs_limit();
			}
		}
		
	}
	else
	{
		if(G_parameter.fixmode.gps_fix_sw)
		{
			track_cust_gps_work_req((void *)2);
		}
		
	}
}



//����ģʽʱ�ж��Ƿ��ʱ�����������ģʽ
//return ��1 ���ڣ�0������
kal_uint8 track_cust_deep_sleep_mode(void* par)
{
	static U8 deep_sleep_mode = 0;
	U8 op =(U8)par; 
	if(op==99)
	{
		return deep_sleep_mode;
	}
	else if(op==1)
	{
		deep_sleep_mode = 1;
		track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_INTO_DEEP_SLEEP_MODE, KAL_TRUE, 0);
		track_cust_fly_mode_change((void*)44);
		//sleep
		//ƽ̨
	}
	else
	{
		if(deep_sleep_mode==1)
		{
			deep_sleep_mode = 0;
			//�˳��������
			track_cust_fly_mode_change((void*)77);
		}
	}
	return deep_sleep_mode;
}



void track_cust_fly_mode_change(void* par)
{
    static kal_uint8 time_reconn  = 0;
	U8 port = (U8)par;
#if defined(__GT310__)
    if(G_parameter.fly_stu.sw== 0) return;
#endif
    LOGD(L_APP, L_V5, "port:%d, sms_staus:%d", port, sms_staus);
    if(((port >= 0 && port < 50)|| port==0xFF) && sms_staus != SMS_AIRPLANE_MODE && !(track_is_testmode() || track_is_autotestmode()))
    {
		if(port!=0xFF)
		{
			track_cust_paket_19(track_drv_get_lbs_data(), TR_CUST_ALARM_INTO_DEEP_SLEEP_MODE, KAL_TRUE, 0);
			track_start_timer(TRACK_CUST_INTO_DEEP_SLEEP_MODE_TIMER_ID, 10*1000, track_cust_fly_mode_change, (void*)0xFF);
			return;
		}
        time_reconn = 0;
        sms_staus = SMS_AIRPLANE_MODE;
        track_stop_timer(TRACK_CUST_ALARM_LowBattery_Fly_TIMER);
        track_stop_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_OVERTIME_TIMER_ID);
        track_stop_timer(TRACK_CUST_WORK_MODE_GPSUPDATE_TIMER_ID);
        //track_cust_gps_off_req((void*)5);
        track_cust_gps_control(0);
        //track_drv_sensor_off();
        track_cust_led_sleep(2);
        track_drv_sys_cfun(4);
        track_soc_disconnect();
        track_cust_server2_disconnect();
        //track_os_intoTaskMsgQueue(track_drv_ext_vbat_measure_modify_parameters);
        track_cust_make_call((void*)23);
        track_cust_call_hangup();
        track_sms_reset();
        LOGD(L_APP, L_V4, "!!! �������ģʽ!!!");
    }
    else if(port >= 50 && port < 90 && sms_staus != SMS_NORMAL_MODE)
    {
        track_stop_timer(TRACK_CUST_ALARM_LowBattery_Fly_TIMER);
        time_reconn = 0;
        sms_staus = SMS_NORMAL_MODE;
        //track_cust_gps_work_req((void *)2);    // gps on
        //if(G_parameter.sensor_gps_work == 0) track_cust_gps_control(1);
        //track_cust_sensor_init();
		track_cust_gprson_status(1);
        track_drv_sys_cfun(1);
        track_stop_timer(TRACK_CUST_NET_CONN_TIMEOUT_TIMER_ID);
        tr_stop_timer(TRACK_CUST_ALARM_RETURN_FlyMode_TIMER);
        tr_stop_timer(TRACK_CUST_INTO_DEEP_SLEEP_MODE_TIMER_ID);
        //track_os_intoTaskMsgQueue(track_drv_ext_vbat_measure_modify_parameters);
        track_soc_exit_fly_only();
        track_cust_call_hangup();
        track_sms_reset();
		track_soc_gprs_reconnect((void*)305);
        LOGD(L_APP, L_V4, "!!! �˳�����ģʽ!!!");
    }
}

kal_bool track_cust_work_mode_get_fly_mode()
{
	LOGD(L_APP, L_V6, "!!! flymode %d",sms_staus);
	return (sms_staus == SMS_AIRPLANE_MODE ? 1 : 0);
}

kal_int8 track_mode3_upload_location(kal_int8 fix)
{
	static kal_int8 up=-1;
	if(fix==99)
	{
		return up;
	}
	else if(fix==GPS_FIX)
	{
		up = GPS_FIX;
	}
	else if(fix==WF_FIX)
	{
		up = WF_FIX;
	}
	else if(fix==LBS_FIX)
	{
		up = WF_FIX;
	}
	else
	{
		up = fix;
	}
	return up;
}

void track_cust_mode3_upload_location(void * f)
{
    static kal_uint8 up_counts = 0;
    kal_uint32 flag = (kal_uint32)f;
    LOGD(L_GPS, L_V5, "flag:%d,up_counts:%d -----------", flag, up_counts);

    if(WORK_MODE_3 == track_cust_get_work_mode() && G_parameter.mode3_work_interval)
    {
        track_stop_timer(TRACK_CUST_LBS_UPDATE_L_TIMER_ID);

        if(0 == flag)
        {
			if(G_parameter.fixmode.wf_lbs_fix_sw)
			{
				if(G_parameter.wifi.sw == 1)
				{
					track_drv_wifi_work_mode();
				}
				else
				{
					track_cust_sendlbs_limit();
				}
			}
        }
		else if(flag==1)
		{
			if(up_counts==1)return;//�򵥴���ֻ��һ����
			up_counts=1;
			if(track_cust_gps_status() >= GPS_STAUS_2D)
	        {
	            track_gps_data_struct *p_gpsdata = track_cust_backup_gps_data(0, NULL);
	            LOGD(L_GPS, L_V6, "��ʱ����GPS���ϴ���λ��");
	            track_cust_backup_gps_data(3, p_gpsdata);
	            track_cust_paket_position(p_gpsdata, track_drv_get_lbs_data(), TR_CUST_POSITION_MODE_GPSUPDATE, 0);
	        }
		}
        else if(flag==2)
        {
            up_counts = 0;
            track_cust_mode3_uping(0);
			track_mode3_upload_location(-1);
			track_cust_fly_mode_change((void*)47);
            LOGD(L_APP, L_V5, "MODE3�ϴ�����");
            return ;
        }
        
    }
}

#endif /* __GT310__ */
