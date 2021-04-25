/***********************************************************************
*   This software/firmware and related documentation ("MediaTek Software") 
*   are protected under relevant copyright laws. The information contained 
*   herein is confidential and proprietary to MediaTek Inc. and/or its licensors.
*
*   Without the prior written permission of MediaTek Inc. and/or its licensors, 
*   any reproduction, modification, use or disclosure of MediaTek Software, and 
*   information contained herein, in whole or in part, shall be strictly prohibited.
*
*   MediaTek Inc. (C) [2007]. All rights reserved.
*
*************************************************************************/ 
/*******************************************************************************
* Filename:
* ---------
*  gps_adaptor.c
*
* Project:
* --------
*  MAUI
*
* Description:
* ------------
*  GPS related function run in GPS task, process AT || MRE req
*
* Author:
* -------
 * -------
*
*============================================================================
*             HISTORY
* Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*------------------------------------------------------------------------------
 * removed!
*
 * removed!
 * removed!
 * removed!
 * removed!
*
 * removed!
 * removed!
 * removed!
*
 * removed!
 * removed!
 * removed!
*
 * removed!
 * removed!
 * removed!
*
 * removed!
 * removed!
 * removed!
*
 * removed!
 * removed!
 * removed!
*
 * removed!
 * removed!
 * removed!
*
 * removed!
 * removed!
 * removed!
*
 * removed!
 * removed!
 * removed!
*
 * removed!
 * removed!
 * removed!
*
 * removed!
 * removed!
 * removed!
*
 * removed!
 * removed!
 * removed!
*
 * removed!
 * removed!
 * removed!
*
 * removed!
 * removed!
 * removed!
*
 * removed!
 * removed!
 * removed!
*
 * removed!
 * removed!
 * removed!
*
*date=21 01 2014 
 * removed!
 * removed!
*
 * removed!
*
*------------------------------------------------------------------------------
* Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*============================================================================
*******************************************************************************/

#if defined(__GPS_SUPPORT__)
#ifdef __GPS_ADAPTOR_SUPPORT__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>


#include "kal_release.h"
#include "stack_common.h"
#include "stack_msgs.h"
#include "task_main_func.h"
#include "app_ltlcom.h"
#include "lcd_ip_cqueue.h"
#include "stack_types.h"
#include "task_config.h"
#include "syscomp_config.h"
#include "custom_config.h"
#include "custom_util.h"
#include "stack_init.h"
#include "stack_ltlcom.h"
#include "intrCtrl.h"
#include "kal_trace.h"
#include "stacklib.h"           /* Basic type for dll, evshed, stacktimer */
#include "event_shed.h"         /* Event scheduler */
#include "stack_timer.h"        /* Stack timer */
#include "dcl.h"

#include "stack_msgs.h"
#include "app_ltlcom.h"     /* Task message communiction */
#include "stdlib.h"         /*math lib such as atof*/
#include "math.h"          /*math lib for log()*/
#include "stdio.h"         /*stdio lib*/
#include "kal_trace.h"      /* kal_trace() prototype */
#include "dcl.h"         /*rtc to get time*/
#include "gps_main.h"
#include "gps_struct.h"
#include "gps_trc.h"
#include "gps_uart_hdlr.h"
#include "gps_adaptor.h"
#include "mtk_gps_task.h"
#include "mtk_gps_type.h" 


#if defined(__AGPS_SUPPORT__)
#include "gps2lcsp_enum.h"
#include "gps2lcsp_struct.h"
#include "agps_struct.h"
#include "gps_agps_process.h"
#include "uagps_cp_gps_struct.h"
#endif

//#include "mdi_agps.h"
//#include "EngineerModeAGPSlogGprot.h"
#include "gps_common.h"
#include "string.h"
#include "stack_config.h"
#include "kal_general_types.h"
#include "kal_public_api.h"
#include "tst_sap.h"
#include "epodownload.h"


#define PMTK010_TIME 3000
//#define NMEA_TIME 1000
#define POWEROFF_TIME 0
#define WRITE_TIME 0
#define EPOFILE_CHECK_TIME 15000
#define CNF_OK   0
#define CNF_ERR 1
#define USER_INVALID_DATA 0xffffffff


extern void epodownload_req_hdlr(ilm_struct *ilm_ptr);
extern void epodownload_req_hdlr_socket(ilm_struct *ilm_ptr);
extern void epodownload_req_hdlr_wps(ilm_struct *ilm_ptr);
void epofile_download_socket(void);
void epofile_download_wps(void);
extern void epofile_check(void *param);
extern int mtk_gps_sys_epo_delete (void);


extern gps_uart_context_struct g_gps_cntx;
extern mtk_gps_task_struct *mtk_gps_context_p;
extern kal_eventgrpid gps_init_eventgrpid;
gps_current_sentence_ind_struct g_nmea_buf_struct;
extern kal_uint8    g_gps_gnss_mode ;  //init gps using 
//extern kal_uint32  g_gps_fix_interval ;  //init gps using 
kal_uint32 g_gps_adaptor_fix_interval = 1000;  // default value means 1000ms





typedef struct
{
    eventid event_id;
    kal_timer_func_ptr callback_func;
    void *arg;
}gps_adaptor_timer_table_struct;

typedef struct
{
    event_scheduler *event_scheduler_ptr;
} gps_adaptor_task_struct;



gps_adaptor_timer_table_struct gps_adaptor_timer_table[MAX_NUM_OF_GPS_ADAPTOR_TIMER];
gps_adaptor_task_struct gps_adaptor_cntx;
gps_adaptor_task_struct *gps_adaptor_cntx_p=&gps_adaptor_cntx;
int count;

#if !defined(__GPS_EPO_DOWNLOAD__)
extern epo_socket_cntx_struct g_epo_socket_cntx;
#endif

int get_open_gps_mode_count(void){
	int ret=0;
	int i;
	for(i=0;i<GPS_UART_MAX_MOD_PER_PORT;i++)
	{
		
		if(g_gps_cntx.port.mod_id[i] != MOD_NIL)
		{
			ret++;
		}
			
	}   
	return ret;
}

void gps_adaptor_timer_expiry(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    evshed_timer_handler(gps_adaptor_cntx_p->event_scheduler_ptr);

}

static void gps_adaptor_timer_expiry_callback(void *arg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint8 *timer_id = (kal_uint8*) arg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((kal_uint32) timer_id < (kal_uint32) MAX_NUM_OF_GPS_ADAPTOR_TIMER)
    {
		kal_prompt_trace(MOD_GPSADAPTOR,"[GPS_ADAPTOR]:timerid=%d\n",timer_id);
        gps_adaptor_timer_table[(kal_uint32) (timer_id)].event_id = NULL;
        (gps_adaptor_timer_table[(kal_uint32) (timer_id)].callback_func) (gps_adaptor_timer_table[(kal_uint32) (timer_id)].arg);
    }
}



void gps_adaptor_start_timer(kal_uint8 timer_id, kal_uint32 period, kal_timer_func_ptr timer_expiry, void *arg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint32 temp = period * 13 / 60 + 1;    /* ~= x 1/4.615 */

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	GPSADAPTOR_TRACE((TRACE_FUNC, GPSADAPOOR_START_TIMER,
				(gps_adaptor_timer_enum)timer_id,
				period/1000));
    gps_adaptor_timer_table[timer_id].callback_func = timer_expiry;
    gps_adaptor_timer_table[timer_id].arg = arg;
    gps_adaptor_timer_table[timer_id].event_id = evshed_set_event(
                                            gps_adaptor_cntx_p->event_scheduler_ptr,
                                            (kal_timer_func_ptr) gps_adaptor_timer_expiry_callback,
                                            (void*)timer_id,
                                            temp /** KAL_TICKS_10_MSEC */ );
}

void gps_adaptor_stop_timer(kal_uint8 timer_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (gps_adaptor_timer_table[timer_id].event_id != NULL)
    {
    	GPSADAPTOR_TRACE((TRACE_FUNC, GPSADAPOOR_STOP_TIMER,
				(gps_adaptor_timer_enum)timer_id));
        evshed_cancel_event(gps_adaptor_cntx_p->event_scheduler_ptr, &(gps_adaptor_timer_table[timer_id].event_id));
        gps_adaptor_timer_table[timer_id].event_id = NULL;
    }else{
    	GPSADAPTOR_TRACE((TRACE_FUNC, GPSADAPOOR_ALREADY_STOP_TIMER,
				(gps_adaptor_timer_enum)timer_id));
	}
}




//define timeID   time_interval

void send_msg_to_gps_task(module_type src_mod,module_type dst_mod,msg_type msg_id,void *local_param_ptr, kal_uint32 para_size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct  *ilm_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ilm_ptr = allocate_ilm(src_mod);
    ilm_ptr->src_mod_id = src_mod;
    ilm_ptr->dest_mod_id = dst_mod;
    ilm_ptr->sap_id = 0;
    ilm_ptr->msg_id = msg_id;
    ilm_ptr->local_para_ptr = (local_para_struct*) local_param_ptr;
    ilm_ptr->peer_buff_ptr = (peer_buff_struct*) NULL;
    msg_send_ext_queue(ilm_ptr);
}

void gps_nmea_data_post_hdlr(void *param)
{
	gps_timer_data_struct *timer_data=(gps_timer_data_struct *)param;
	gps_current_sentence_ind_struct  *local_param_ptr;
	kal_uint8 i=0;
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/

	gps_adaptor_stop_timer(GPS_ADAPTOR_TIMER);
	
	
	for(i=0;i<GPS_UART_MAX_MOD_PER_PORT;i++)
	{
		
		if(g_gps_cntx.port.mod_id[i] != MOD_NIL)
		{
			local_param_ptr = construct_local_para(sizeof(gps_current_sentence_ind_struct),TD_CTRL);
			local_param_ptr->result=g_nmea_buf_struct.result;
			local_param_ptr->user_data=g_nmea_buf_struct.user_data;
			memcpy((kal_uint8 *)&(local_param_ptr->gps_sentence_info), (kal_uint8 *)&(g_nmea_buf_struct.gps_sentence_info), sizeof(g_nmea_buf_struct.gps_sentence_info));
		
			send_msg_to_gps_task(MOD_GPSADAPTOR,g_gps_cntx.port.mod_id[i],MSG_ID_GPS_CURRENT_SENTENCE_IND,local_param_ptr,
		                   sizeof(gps_current_sentence_ind_struct));
		}
	}   
	g_nmea_buf_struct.result=KAL_FALSE;
	
	//kal_print_string_trace(MOD_GPS, TRACE_FUNC, "140228 1s  is_start_ok =%d, mnl_state=%d", g_gps_cntx.is_start_ok, mtk_gps_context_p->gps_state);  
	if((g_gps_cntx.is_start_ok == KAL_TRUE) || (mtk_gps_context_p->gps_state == MNL_RUNNING))
	{
		gps_adaptor_start_timer(GPS_ADAPTOR_TIMER, g_gps_adaptor_fix_interval, gps_nmea_data_post_hdlr, NULL);
	} 
}


void gps_poweron_cnf_hdlr(void *param)
{
	gps_poweron_cnf_struct  *local_param_ptr;
	gps_timer_data_struct *timer_data=(gps_timer_data_struct *)param;
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	local_param_ptr = construct_local_para(sizeof(gps_poweron_cnf_struct),TD_CTRL);
	local_param_ptr->user_data=timer_data->user_data;
	local_param_ptr->result=(mtk_gps_context_p->gps_state == MNL_RUNNING? CNF_OK : CNF_ERR);    

	send_msg_to_gps_task(MOD_GPSADAPTOR,timer_data->src_mod,MSG_ID_GPS_POWERON_CNF,local_param_ptr,
	                    sizeof(gps_poweron_cnf_struct));
	
	//kal_print_string_trace(MOD_MNL, TRACE_FUNC, "140227 gps_state =%d", mtk_gps_context_p->gps_state);     
	
	if(mtk_gps_context_p->gps_state == MNL_RUNNING)
	{
		gps_adaptor_start_timer(GPS_ADAPTOR_TIMER, g_gps_adaptor_fix_interval, gps_nmea_data_post_hdlr, NULL);
	}
}
static void gps_poweron_req_hdlr(ilm_struct *ilm_ptr)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	gps_poweron_req_struct  *local_param;
	gps_common_uart_work_mode_enum work_mode;
	gps_uart_open_req_struct   *local_param_ptr;
	gps_uart_close_req_struct   *local_param_ptr_close;
	gps_timer_data_struct timer_data;
	kal_uint32 adatpor_return_val; 
	kal_uint32 event_group;

	//gps task is ready,g_gps_cntx.event_id


	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	count=get_open_gps_mode_count();
	local_param = (gps_poweron_req_struct*)(ilm_ptr->local_para_ptr);
	timer_data.user_data=local_param->user_data;
	timer_data.src_mod=ilm_ptr->src_mod_id;

	local_param_ptr = construct_local_para(sizeof(gps_uart_open_req_struct),TD_CTRL);
	local_param_ptr->port = GPS_Get_UART_Port();
	work_mode=GPS_UART_MODE_NMEA_DATA;
	local_param_ptr->mode = work_mode;
	local_param_ptr->return_val = &adatpor_return_val;
	local_param_ptr->module_id = ilm_ptr->src_mod_id;
	local_param_ptr->event_id = g_gps_cntx.event_id ;
	g_gps_gnss_mode = local_param->gps_type;
	//g_gps_fix_interval = (local_param ->tracking_period) * 1000;
	g_gps_adaptor_fix_interval = (local_param ->tracking_period) * 1000;
	if(g_gps_gnss_mode == GPS_TYPE_GPS_ONLY)
	{
		g_gps_gnss_mode = MTK_CONFIG_GPS_ONLY;
	}
	else if(g_gps_gnss_mode == GPS_TYPE_GPS_GLONASS)
	{
		g_gps_gnss_mode = MTK_CONFIG_GPS_GLONASS;		
	}
	else if(g_gps_gnss_mode == GPS_TYPE_GPS_BEIDOU)
	{
		g_gps_gnss_mode = MTK_CONFIG_GPS_BEIDOU;	
	}
	
	send_msg_to_gps_task(MOD_GPSADAPTOR,MOD_GPS,MSG_ID_GPS_UART_OPEN_REQ,local_param_ptr,
	                    sizeof(gps_uart_open_req_struct));
	
	// CLEAR ALL NMEA DATA
	{	
		kal_mem_set(&g_nmea_buf_struct.gps_sentence_info.GPGGA, 0, MAX_GPGGA_SENTENCE_LENGTH+1);
		kal_mem_set(&g_nmea_buf_struct.gps_sentence_info.GPGSA, 0, MAX_GPGSA_SENTENCE_LENGTH+1);
		kal_mem_set(&g_nmea_buf_struct.gps_sentence_info.GPRMC, 0, MAX_GPRMC_SENTENCE_LENGTH+1);
		kal_mem_set(&g_nmea_buf_struct.gps_sentence_info.GPVTG, 0, MAX_GPVTG_SENTENCE_LENGTH+1);
		kal_mem_set(&g_nmea_buf_struct.gps_sentence_info.GPGSV, 0, MAX_GPGSV_SENTENCE_LENGTH+1);
		kal_mem_set(&g_nmea_buf_struct.gps_sentence_info.GLGSV, 0, MAX_GLGSV_SENTENCE_LENGTH+1);
		kal_mem_set(&g_nmea_buf_struct.gps_sentence_info.GLGSA, 0, MAX_GLGSA_SENTENCE_LENGTH+1);
		kal_mem_set(&g_nmea_buf_struct.gps_sentence_info.BDGSV, 0, MAX_BDGSV_SENTENCE_LENGTH+1);
		kal_mem_set(&g_nmea_buf_struct.gps_sentence_info.BDGSA, 0, MAX_BDGSA_SENTENCE_LENGTH+1);
        kal_mem_set(&g_nmea_buf_struct.gps_sentence_info.GPACC, 0, MAX_GPACC_SENTENCE_LENGTH+1);
    }
	if(count==0){//indicate there is noone.
		kal_retrieve_eg_events(gps_init_eventgrpid,GPS_INIT_FINISH_EVENT,KAL_OR_CONSUME,
		              &event_group,KAL_SUSPEND);
	}
	if(mtk_gps_context_p->gps_state != MNL_RUNNING)	// init falied, 	send close cmd
	{
		local_param_ptr_close = construct_local_para(sizeof(gps_uart_close_req_struct),TD_CTRL);
		local_param_ptr_close->port = GPS_Get_UART_Port();
		work_mode=GPS_UART_MODE_NMEA_DATA;
		local_param_ptr_close->mode = work_mode;
		local_param_ptr_close->module_id = ilm_ptr->src_mod_id;
		local_param_ptr_close->event_id = g_gps_cntx.event_id ;
		send_msg_to_gps_task(MOD_GPSADAPTOR,MOD_GPS,MSG_ID_GPS_UART_CLOSE_REQ,local_param_ptr_close,
	                    sizeof(gps_uart_close_req_struct));
	}
	gps_poweron_cnf_hdlr(&timer_data);
	g_nmea_buf_struct.result=KAL_FALSE;
}


void gps_poweroff_cnf_hdlr(void *param)
{
	gps_poweroff_cnf_struct  *local_param_ptr;
	gps_timer_data_struct *timer_data=(gps_timer_data_struct *)param;
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	gps_adaptor_stop_timer(GPS_ADAPTOR_TIMER);
	local_param_ptr = construct_local_para(sizeof(gps_poweroff_cnf_struct),TD_CTRL);
	local_param_ptr->user_data=timer_data->user_data;
	if(count>1)
	{
		local_param_ptr->result=CNF_OK;
	}else{
		local_param_ptr->result=(mtk_gps_context_p->gps_state == MNL_POWEROFF?CNF_OK: CNF_ERR);
	}
	
	send_msg_to_gps_task(MOD_GPSADAPTOR,timer_data->src_mod,MSG_ID_GPS_POWEROFF_CNF,local_param_ptr,
	                    sizeof(gps_poweroff_cnf_struct));
	
	if(mtk_gps_context_p->gps_state != MNL_POWEROFF)
	{
		gps_adaptor_start_timer(GPS_ADAPTOR_TIMER, g_gps_adaptor_fix_interval, gps_nmea_data_post_hdlr, timer_data);
	} 
}
static void gps_poweroff_req_hdlr(ilm_struct *ilm_ptr)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	gps_poweroff_req_struct  *local_param;
	gps_uart_close_req_struct   *local_param_ptr;
	gps_timer_data_struct timer_data;
	kal_uint32 adatpor_return_val; 
	gps_common_uart_work_mode_enum work_mode;
	kal_uint32 event_group;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	count=get_open_gps_mode_count();
	local_param = (gps_poweroff_req_struct*)(ilm_ptr->local_para_ptr);
	timer_data.user_data=local_param->user_data;
	timer_data.src_mod=ilm_ptr->src_mod_id;

	local_param_ptr = construct_local_para(sizeof(gps_uart_close_req_struct),TD_CTRL);
	local_param_ptr->port = GPS_Get_UART_Port();
	work_mode=GPS_UART_MODE_NMEA_DATA;
	local_param_ptr->mode = work_mode;
	local_param_ptr->module_id = ilm_ptr->src_mod_id;
    
	local_param_ptr->event_id = g_gps_cntx.event_id ;
	send_msg_to_gps_task(MOD_GPSADAPTOR,MOD_GPS,MSG_ID_GPS_UART_CLOSE_REQ,local_param_ptr,
	                            sizeof(gps_uart_close_req_struct));
	if(count==1){//indicate only one module
		kal_retrieve_eg_events(
	                gps_init_eventgrpid,
	                GPS_SHUTDOWN_FINISH_EVENT,
	                KAL_OR_CONSUME,
	                &event_group,
	                KAL_SUSPEND);
	}
	

	gps_poweroff_cnf_hdlr(&timer_data);
}


void gps_start_mode_cnf_hdlr(void *param)
{
	gps_start_mode_cnf_struct  *local_param_ptr;
	gps_timer_data_struct *timer_data=(gps_timer_data_struct *)param;
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	
	local_param_ptr = construct_local_para(sizeof(gps_start_mode_cnf_struct),TD_CTRL);
	local_param_ptr->user_data=timer_data->user_data;
	local_param_ptr->result=timer_data->result;
	local_param_ptr->start_mode=timer_data->start_mode;
	send_msg_to_gps_task(MOD_GPSADAPTOR,timer_data->src_mod,MSG_ID_GPS_SET_START_MODE_CNF,local_param_ptr,sizeof(gps_start_mode_cnf_struct));
}
static void gps_set_start_mode_req_hdlr(ilm_struct *ilm_ptr)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	gps_start_mode_req_struct  *local_param;
	gps_uart_write_req_struct   *local_param_ptr;
	gps_timer_data_struct timer_data;
	kal_uint32 adatpor_return_val; 
	kal_uint32 event_group;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	local_param = (gps_start_mode_req_struct*)(ilm_ptr->local_para_ptr);
	timer_data.user_data=local_param->user_data;
	timer_data.src_mod=ilm_ptr->src_mod_id;
	timer_data.start_mode=local_param->start_mode;

	local_param_ptr = construct_local_para(sizeof(gps_uart_write_req_struct),TD_CTRL);
	local_param_ptr->port = GPS_Get_UART_Port();
	local_param_ptr->is_rawdata=KAL_FALSE;
	switch(local_param->start_mode)
	{
		case GPS_HOT_START:
			local_param_ptr->cmd=GPS_UART_GPS_HOT_START;
			kal_prompt_trace(MOD_GPSADAPTOR,"Set HOT_START OK\n");
			break;
		case GPS_WARM_START:
			local_param_ptr->cmd=GPS_UART_GPS_WARM_START;
			kal_prompt_trace(MOD_GPSADAPTOR,"Set WARM_START OK\n");
			break;
		case GPS_COLD_START:
			local_param_ptr->cmd=GPS_UART_GPS_COLD_START;
			kal_prompt_trace(MOD_GPSADAPTOR,"Set COLD_START OK\n");
			break;
		case GPS_FULL_START:
			local_param_ptr->cmd=GPS_UART_GPS_CLEAR_NVRAM;
			kal_prompt_trace(MOD_GPSADAPTOR,"Set FULL_START OK\n");
			break;		
		default:
			break;
	}
	timer_data.result=-1;
	if(mtk_gps_context_p->gps_state == MNL_RUNNING)
	{
		local_param_ptr->return_val = &adatpor_return_val;
		local_param_ptr->module_id = ilm_ptr->src_mod_id;
		local_param_ptr->event_id = g_gps_cntx.event_id ;
		send_msg_to_gps_task(MOD_GPSADAPTOR,MOD_GPS,MSG_ID_GPS_UART_WRITE_REQ,local_param_ptr,
	                        sizeof(gps_uart_write_req_struct));
		kal_retrieve_eg_events(
			g_gps_cntx.event_id,
			GPS_UART_WRITE_FINISH_EVENT,
			KAL_OR_CONSUME,
			&event_group,
			KAL_SUSPEND);
	}
	
	
	timer_data.result=adatpor_return_val;
	gps_start_mode_cnf_hdlr(&timer_data);
}

static void gps_set_param_req_hdlr(ilm_struct *ilm_ptr)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	gps_set_param_req_struct *local_param;
	gps_set_param_cnf_struct *local_param_ptr;
	gps_timer_data_struct timer_data;
	kal_uint32 adatpor_return_val; 
	kal_uint8 result =CNF_OK;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	local_param = (gps_set_param_req_struct*)(ilm_ptr->local_para_ptr); 
	switch(local_param->param_type)
	{
		case GPS_CMD_SET_LOCATION_REPORT_PREIOD:
			if(local_param->value < 1)                   // smaller than 1000ms, measn err case
			{
				result = CNF_ERR;
			}
			else   // >=1000ms,  it is ok
			{
				result = CNF_OK;
				g_gps_adaptor_fix_interval = (local_param->value) *1000;
				//gps_nmea_data_post_hdlr();
			}
		break;
		default:
			break;
	}

	local_param_ptr = construct_local_para(sizeof(gps_set_param_cnf_struct),TD_CTRL);
	local_param_ptr->result=result;
	local_param_ptr->user_data= local_param->user_data; //g_nmea_buf_struct.user_data;
	local_param_ptr->param_type= local_param->param_type;
	
	send_msg_to_gps_task(MOD_GPSADAPTOR,ilm_ptr->src_mod_id,MSG_ID_GPS_SET_PARAM_CNF,local_param_ptr,
	                    sizeof(gps_set_param_cnf_struct));
}


static void gps_get_current_sentence_req_hdlr(ilm_struct *ilm_ptr)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	gps_get_current_sentence_req_struct *local_param;
	gps_get_current_sentence_cnf_struct*local_param_ptr;
	gps_timer_data_struct timer_data;
	kal_uint32 adatpor_return_val; 

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	local_param = (gps_get_current_sentence_req_struct*)(ilm_ptr->local_para_ptr);
	timer_data.user_data=local_param->user_data;
	timer_data.src_mod=ilm_ptr->src_mod_id;

	local_param_ptr = construct_local_para(sizeof(gps_current_sentence_ind_struct),TD_CTRL);
	local_param_ptr->result=g_nmea_buf_struct.result;
	local_param_ptr->user_data= local_param->user_data; //g_nmea_buf_struct.user_data;
	memcpy((kal_uint8 *)&(local_param_ptr->gps_sentence_info), (kal_uint8 *)&(g_nmea_buf_struct.gps_sentence_info), sizeof(g_nmea_buf_struct.gps_sentence_info));
	
	send_msg_to_gps_task(MOD_GPSADAPTOR,ilm_ptr->src_mod_id,MSG_ID_GPS_GET_CURRENT_SENTENCE_CNF,local_param_ptr,
	                    sizeof(gps_get_current_sentence_cnf_struct));
}


void gps_adaptor_tst_inject_string_handler( ilm_struct *ilm_ptr )
{
#if defined(__TCPIP__) && !defined(__GPS_EPO_DOWNLOAD__)
#ifdef __MMI_TELEPHONY_SUPPORT__
    epodownload_test_handler(ilm_ptr);
#endif
#endif
}


/*****************************************************************************
* FUNCTION
*  gps_agps_req_hdlr
* DESCRIPTION
*  handler of all agps related MSG
* PARAMETERS
*  *ilm_ptr      [IN]       MSG pointer
* RETURNS
*    NULL
*****************************************************************************/
	void gps_adaptor_req_hdlr(ilm_struct *ilm_ptr)
	{
		/*----------------------------------------------------------------*/
		/* Local Variables												  */
		/*----------------------------------------------------------------*/
		kal_uint16 msg_id = ilm_ptr->msg_id;
	
		/*----------------------------------------------------------------*/
		/* Code Body													  */
		/*----------------------------------------------------------------*/
		switch(msg_id)
		{
		
		case MSG_ID_TIMER_EXPIRY:
			gps_adaptor_timer_expiry(ilm_ptr);
			break;
		case MSG_ID_TST_INJECT_STRING:
			gps_adaptor_tst_inject_string_handler(ilm_ptr);
			break;
		case MSG_ID_GPS_POWERON_REQ:
			gps_poweron_req_hdlr(ilm_ptr);
			break;
		case MSG_ID_GPS_POWEROFF_REQ:
			gps_poweroff_req_hdlr(ilm_ptr);
			break;
		case MSG_ID_GPS_SET_START_MODE_REQ:
			gps_set_start_mode_req_hdlr(ilm_ptr);
			break;
		case MSG_ID_GPS_GET_CURRENT_SENTENCE_REQ:
			gps_get_current_sentence_req_hdlr(ilm_ptr);
			break;
		case MSG_ID_GPS_SET_PARAM_REQ:
			gps_set_param_req_hdlr(ilm_ptr);
			break;
		
#if defined(__TCPIP__) && !defined(__GPS_EPO_DOWNLOAD__)
#ifdef __MMI_TELEPHONY_SUPPORT__
		case MSG_ID_WPS_SET_CHANNEL_RSP:
		case MSG_ID_WPS_HTTP_RSP:
		case MSG_ID_WPS_READ_CONTENT_RSP:
		case MSG_ID_WPS_UNSET_CHANNEL_RSP:
			//epodownload_req_hdlr_wps(ilm_ptr);
			break;
		case MSG_ID_APP_SOC_GET_HOST_BY_NAME_IND:
		case MSG_ID_APP_SOC_NOTIFY_IND:
			epodownload_req_hdlr_socket(ilm_ptr);
			break;
#endif
#endif
			
		default:
			kal_prompt_trace(MOD_GPSADAPTOR,"msg ID=%d\n",msg_id);
			#ifdef __IOT_CUSTOMIZE__
			gpsadaptor_agps_test_handler(ilm_ptr);
			#endif
			//ASSERT(0);
			break;
		}
	}


#ifdef __TCPIP__
#ifdef __MMI_TELEPHONY_SUPPORT__
extern void epo_wps_set_channel_req(void);

void epofile_check(void *param)
{
	//check epofile exist or not
	//check epofile valid time ,compared with download time
	kal_prompt_trace(MOD_GPSADAPTOR,"EPO download begin.\n");
	gps_adaptor_stop_timer(GPS_EPO_TIMER);
	//epo_wps_set_channel_req();
	//gps_adaptor_start_timer(GPS_EPO_TIMER, EPOFILE_CHECK_TIME, epofile_check, NULL);
}
#endif
#endif

void gpsAdaptor_task_main(task_entry_struct * task_entry_ptr)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	ilm_struct current_ilm;
	kal_uint32 my_index;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	kal_get_my_task_index(&my_index);
	gps_adaptor_cntx_p->event_scheduler_ptr = evshed_create("GPS adaptor timer", MOD_GPSADAPTOR, 0, 0 );
#if defined(__TCPIP__) && !defined(__GPS_EPO_DOWNLOAD__)
#ifdef __MMI_TELEPHONY_SUPPORT__
	epodownload_cntx_init_after_reboot();


	g_epo_socket_cntx.timer_handler_type=TIMEOUT_TYPE_REBOOT;
	gps_adaptor_start_timer(GPS_EPO_TIMER, EPODOWNLOAD_AFTER_REBOOT, epodownload_timer_hdlr, NULL);	
#endif
#endif

	while ( 1 )
	{
		receive_msg_ext_q(task_info_g[task_entry_ptr->task_indx].task_ext_qid, &current_ilm); //no share mode
		//receive_msg_ext_q_for_stack(task_info_g[task_entry_ptr->task_indx].task_ext_qid, &current_ilm); // share mode
		stack_set_active_module_id(my_index, current_ilm.dest_mod_id);

		//bee_main((void*)&current_ilm);
		gps_adaptor_req_hdlr((void*)&current_ilm);
		free_ilm(&current_ilm);
	}
}

kal_bool gpsAdaptor_create(comptask_handler_struct **handle)
{
   static const comptask_handler_struct gpsAdaptor_handler_info =
   {
      gpsAdaptor_task_main,  /* task entry function */
      NULL,  /* task initialization function */
      NULL,  /* task configuration function */
      NULL,  /* task reset handler */
      NULL,  /* task termination handler */
   };

   *handle = (comptask_handler_struct *)&gpsAdaptor_handler_info;
   return KAL_TRUE;
}

#endif
#endif

#if __IOT_CUSTOMIZE__
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>


#include "kal_release.h"
#include "stack_common.h"
#include "stack_msgs.h"
#include "task_main_func.h"
#include "app_ltlcom.h"
#include "lcd_ip_cqueue.h"
#include "stack_types.h"
#include "task_config.h"
#include "syscomp_config.h"
#include "custom_config.h"
#include "custom_util.h"
#include "stack_init.h"
#include "stack_ltlcom.h"
#include "intrCtrl.h"
#include "kal_trace.h"
#include "stacklib.h"           /* Basic type for dll, evshed, stacktimer */
#include "event_shed.h"         /* Event scheduler */
#include "stack_timer.h"        /* Stack timer */
#include "dcl.h"

#include "stack_msgs.h"
#include "app_ltlcom.h"     /* Task message communiction */
#include "stdlib.h"         /*math lib such as atof*/
#include "math.h"          /*math lib for log()*/
#include "stdio.h"         /*stdio lib*/
#include "kal_trace.h"      /* kal_trace() prototype */
#include "gps_struct.h"
#include "dcl.h"         /*rtc to get time*/
#include "gps_main.h"
#include "gps_trc.h"
#include "gps_uart_hdlr.h"
#include "gps_adaptor.h"
#include "mtk_gps_task.h"
#include "mtk_gps_type.h" 

#if 1//defined (__GPS_ADAPTOR_SUPPORT__) && defined(__CUSTOM_GPS_FEATURE_SWITCH__)

#if defined(__AGPS_SUPPORT__)
#include "gps2lcsp_enum.h"
#include "gps2lcsp_struct.h"
#include "agps_struct.h"
#include "gps_agps_process.h"
#include "uagps_cp_gps_struct.h"
#include "agps_setting.h"
#include "mmi_gps_nvram_def.h"
#endif
#define SUPL_TLS_PORT 7275
#define SUPL_PORT 7276

void gps_adaptor_req_hdlr(ilm_struct *ilm_ptr)
{
	/*----------------------------------------------------------------*/
	/* Local Variables												  */
	/*----------------------------------------------------------------*/
	kal_uint16 msg_id = ilm_ptr->msg_id;

	/*----------------------------------------------------------------*/
	/* Code Body													  */
	/*----------------------------------------------------------------*/
	switch(msg_id)
	{
#if defined(__AGPS_SUPPORT__)
	case MSG_ID_GPS_UART_P_INFO_IND:
	case MSG_ID_SUPL_MMI_STATUS_IND:
	case MSG_ID_SUPL_MMI_ABORT_CNF:
	case MSG_ID_GPS_ASSIST_BIT_MASK_IND:
		gpsadaptor_agps_hdlr(ilm_ptr);
		break;
	case MSG_ID_SUPL_ENABLE:
		{
			gps_supl_ind_struct* config = (gps_supl_ind_struct*) ilm_ptr->local_para_ptr;
			agps_adaptor_set_config(1, config->server, config->port, config->use_tls);
			agps_adaptor_set_accout_id(config->account_id);
		}
		break;
	case MSG_ID_SUPL_DISABLE:
		agps_adaptor_set_config(0, 0, 0, 0);
		agps_adaptor_set_accout_id(0xffffffff);
		break;
    /* 
     //Need add msg for SUPL config
    case GPS_CMD_SET_AGPS_ON_OFF:
      
      kal_print_string_trace(MOD_GPSADAPTOR, TRACE_FUNC, "[AGPS] AGPS Set  value = %d", local_param->value);
        if(local_param->value == 1)                    
          {
            result = CNF_OK;
            gpsadaptor_agps_setting_set_agps_on_off(KAL_TRUE);
          }
          else if(local_param->value == 0) 
          {
            result = CNF_OK;
            gpsadaptor_agps_setting_set_agps_on_off(KAL_FALSE);
          }
          else 
          {   
            kal_print_string_trace(MOD_GPSADAPTOR, TRACE_FUNC, "[AGPS] AGPS Set  Failed");
            result = CNF_ERR;
          }
    
      break;
    case GPS_CMD_SET_SUPL_PROFILE:
          {
            nvram_ef_agps_profile_struct profile;
            agps_server_cfg_struct *agps_server = (agps_server_cfg_struct *)local_param->value;
            gpsadaptor_agps_setting_get_profile(&profile);
            memcpy(profile.addr, agps_server->addr, AGPS_MAX_PROFILE_ADDR_LEN);
            profile.port = agps_server->port;
            gpsadaptor_agps_setting_set_profile(&profile);
            result = CNF_OK;  
            
            kal_print_string_trace(MOD_GPSADAPTOR, TRACE_FUNC, "[SUPL_PROFILE]  SUPL_PROFILE Set address = %s, port = %d", profile.addr, profile.port);
           }
      break;
      */
#endif
	default:
	    #ifdef __IOT_CUSTOMIZE__
		gpsadaptor_agps_test_handler(ilm_ptr);
		#endif
		//ASSERT(0);
		break;	
	}
}

void send_msg_to_gps_task(module_type src_mod,module_type dst_mod,msg_type msg_id,void *local_param_ptr, kal_uint32 para_size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct  *ilm_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ilm_ptr = allocate_ilm(src_mod);
    ilm_ptr->src_mod_id = src_mod;
    ilm_ptr->dest_mod_id = dst_mod;
    ilm_ptr->sap_id = 0;
    ilm_ptr->msg_id = msg_id;
    ilm_ptr->local_para_ptr = (local_para_struct*) local_param_ptr;
    ilm_ptr->peer_buff_ptr = (peer_buff_struct*) NULL;
    msg_send_ext_queue(ilm_ptr);
}

void gps_adaptor_task_main(task_entry_struct * task_entry_ptr)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	ilm_struct current_ilm;
	kal_uint32 my_index;
	kal_uint16 msg_id;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	kal_get_my_task_index(&my_index);

	while ( 1 )
	{
		receive_msg_ext_q(task_info_g[task_entry_ptr->task_indx].task_ext_qid, &current_ilm); //no share mode
		stack_set_active_module_id(my_index, current_ilm.dest_mod_id);
        gps_adaptor_req_hdlr((void*)&current_ilm);

		free_ilm(&current_ilm);
	}
}

kal_bool gps_adaptor_create(comptask_handler_struct **handle)
{
   static const comptask_handler_struct gpsAdaptor_handler_info =
   {
      gps_adaptor_task_main,  /* task entry function */
      NULL,  /* task initialization function */
      NULL,  /* task configuration function */
      NULL,  /* task reset handler */
      NULL,  /* task termination handler */
   };

   *handle = (comptask_handler_struct *)&gpsAdaptor_handler_info;
   return KAL_TRUE;
}
#endif
#endif

