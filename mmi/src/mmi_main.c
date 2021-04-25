/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*******************************************************************************
 * Filename:
 * ---------
 *	mmi_main.c
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 *   This file is intends for Dummy MMI.
 *
 * Author:
 * -------
 * -------
 *
 *==============================================================================
 * 				HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
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
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#ifndef PLUTO_MMI

//#include <stdarg.h>
//#include "kal_release.h"      	/* Basic data type */
//#include "stack_common.h"
#include "stack_msgs.h"
#include "app_ltlcom.h"       	/* Task message communiction */
//#include "stacklib.h"        	/* Basic type for dll, evshed, stacktimer */
//#include "app_buff_alloc.h"
//#include "stack_timer.h"
//#include "event_shed.h"
//#include "syscomp_config.h"
//#include "task_config.h"
//#include "l4a.h"
//#include "kal_trace.h"

/* add for RHR */
#include "kal_general_types.h"
#include "l4c2smsal_struct.h"

#include "track_os_main.h"
#include "syscomp_config.h"
#include "task_config.h"        /* Task creation */
#include "track_os_timer.h"
#include "ell_os.h"

#include "stack_ltlcom.h"
#include "nbr_public_struct.h"
#include "stdlib.h"
#include "kal_public_defs.h"
#include "as2l4c_struct.h"

#include "kal_public_api.h"
#include "..\track\cust\cust_inc\track_cust_ussd.h"

#ifndef CUST_NBR_CELL_INFO
#define  CUST_NBR_CELL_INFO
#endif

extern void EllLog2(const char* fun_name, const char* message , ...);

/* 有自动添加函数名 */
#define EllLog(fmt, args...) EllLog2(__func__, fmt, ##args)

#if defined( CUST_NBR_CELL_INFO)
extern void l4c_nbr_cell_info_update(local_para_struct *local_para_ptr, peer_buff_struct *peer_buff_ptr, module_type dest_id, msg_type msg_name);
extern void $Super$$l4c_nbr_cell_info_update(local_para_struct *local_para_ptr, peer_buff_struct *peer_buff_ptr, module_type dest_id, msg_type msg_name);
extern void $Sub$$l4c_nbr_cell_info_update(local_para_struct *local_para_ptr, peer_buff_struct *peer_buff_ptr, module_type dest_id, msg_type msg_name);

static kal_uint8 update_status = 0;
extern void $Super$$l4c_main(ilm_struct *ilm_ptr);
extern void $Sub$$l4c_main(ilm_struct *ilm_ptr);
static void fun_msg_send(kal_uint16 msg_id, void *req, module_type mod_src, module_type mod_dst)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct *ilm_send;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ilm_send = (ilm_struct *)allocate_ilm(mod_src);
    ilm_send->src_mod_id = mod_src;
    ilm_send->dest_mod_id = (module_type)mod_dst;
    ilm_send->sap_id = INVALID_SAP;
    ilm_send->msg_id = (msg_type) msg_id;
    ilm_send->local_para_ptr = (local_para_struct*) req;
    ilm_send->peer_buff_ptr = (peer_buff_struct*) NULL;
    msg_send_ext_queue(ilm_send);
}
static ussd_send(kal_uint8 *data, kal_uint8 len)
{
    mmi_ss_ussd_rsp_struct *myMsgPtr = NULL;

    myMsgPtr = (mmi_ss_ussd_rsp_struct*) construct_local_para(sizeof(mmi_ss_ussd_rsp_struct), TD_RESET);
    if(myMsgPtr == NULL)
    {
        return;
    }
    if(len < MAX_DIGITS_USSD)
    {
        myMsgPtr->length = len;
        memcpy(myMsgPtr->ussd_str, data, myMsgPtr->length);
        fun_msg_send(MSG_ID_MMI_SS_USSD_RSP, myMsgPtr, MOD_L4C, MOD_MMI);
    }
}

void $Sub$$l4c_main(ilm_struct *ilm_ptr)
{
    if(ilm_ptr->msg_id == MSG_ID_L4CCSM_CISS_PUSSR_END_CNF)
    {
        l4ccsm_ciss_pussr_end_cnf_struct *par = (l4ccsm_ciss_pussr_end_cnf_struct*)ilm_ptr->local_para_ptr;
        kal_prompt_trace(MOD_TST,"%d,%s",par->ussd_string_length,par->ussd_string);
        ussd_send(par->ussd_string, par->ussd_string_length);
    }
    else if(ilm_ptr->msg_id == MSG_ID_L4CCSM_CISS_USSR_FAC_IND)
    {
      l4ccsm_ciss_pussr_begin_req_struct *par = (l4ccsm_ciss_pussr_begin_req_struct*)ilm_ptr->local_para_ptr;
      ussd_send(par->ussd_string, par->ussd_string_length);
    }
    else if(ilm_ptr->msg_id == MSG_ID_L4CCSM_CISS_CW_END_CNF)/*呼叫等待*/
    {
    }
    else if(ilm_ptr->msg_id == MSG_ID_L4CCSM_CISS_CF_END_CNF)/*无条件转移*/
    {
    }
    $Super$$l4c_main(ilm_ptr);
}

void track_l4c_nbr_cell_info_ind(local_para_struct *nbr_cell_info)
{
    static kal_uint32 pre_tick = 0; //24h不会溢出
    kal_uint32 tick = 0, diff;
    ilm_struct *ilm_ptr;
    l4c_nbr_cell_info_reg_cnf_struct *para = NULL;
    l4cps_nbr_cell_info_start_cnf_struct *msg = (l4cps_nbr_cell_info_start_cnf_struct*)nbr_cell_info;

    tick = kal_get_systicks();
    diff = tick - pre_tick;
    if(diff < 1024 && tick > 7000)
    {
        return;
    }
    //4.615*1024ms 最高频率是5秒输出一次(系统最低频率是5秒调用一次)

    //OTA_LOGD(L_DRV, L_V7,"nbr cell info");
    pre_tick = tick;
    para = (l4cps_nbr_cell_info_start_cnf_struct*)construct_local_para(sizeof(l4cps_nbr_cell_info_start_cnf_struct), TD_RESET);

    if(para == NULL)
    {
        OTA_track_cust_restart(30, 0);
    }

    para->rat_mode = msg->rat_mode;
    para->is_nbr_info_valid = msg->is_nbr_info_valid;
    kal_mem_cpy(&para->ps_nbr_cell_info_union, &msg->ps_nbr_cell_info_union, sizeof(para->ps_nbr_cell_info_union));

    ilm_ptr = (ilm_struct *)allocate_ilm(MOD_L4C);
    ilm_ptr->src_mod_id = MOD_L4C;
    ilm_ptr->dest_mod_id = MOD_MMI;
    ilm_ptr->sap_id = INVALID_SAP;
    ilm_ptr->msg_id = (msg_type) MSG_ID_CUST_NBR_CELL_INFO_IND;
    ilm_ptr->local_para_ptr = (local_para_struct*) para;
    ilm_ptr->peer_buff_ptr = NULL;
    msg_send_ext_queue(ilm_ptr);
}


/******************************************************************************
 *  Function    -  $Sub$$l4c_nbr_cell_info_update
 *
 *  Purpose     -  拦截L4C基站信息
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 06-05-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void $Sub$$l4c_nbr_cell_info_update(local_para_struct *local_para_ptr, peer_buff_struct *peer_buff_ptr, module_type dest_id, msg_type msg_name)
{
    if(update_status == 2)
    {
        track_l4c_nbr_cell_info_ind(local_para_ptr);
    }
    $Super$$l4c_nbr_cell_info_update(local_para_ptr, peer_buff_ptr, dest_id, msg_name);
}


/******************************************************************************
 *  Function    -  l4c_nbr_cell_info_update
 *
 *  Purpose     -  l4c_nbr_cell_info_update 系统函数原型
 *
 *  Description -  参考阅读，不能打开
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 06-05-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
#if 0
void l4c_nbr_cell_info_update(local_para_struct *local_para_ptr, peer_buff_struct *peer_buff_ptr, module_type dest_id, msg_type msg_name)
{
    l4c_nbr_cell_info_reg_cnf_struct *para;
    l4cps_nbr_cell_info_start_cnf_struct *msg = (l4cps_nbr_cell_info_start_cnf_struct*)local_para_ptr;
    if(dest_id == l4c_current_mod_id)
    {
        l4c_update_nbr_information(msg);
    }
    else if(dest_id == MOD_ATCI)
    {
        kal_uint8 temp_src = RMMI_PTR->current_src;
        RMMI_PTR->current_src = RMMI_UNSOLICITED;
        l4c_nbr_cell_info_rind(msg->rat_mode, msg->is_nbr_info_valid, (kal_uint8*)&msg->ps_nbr_cell_info_union, sizeof(ps_nbr_cell_info_union_type));
        RMMI_PTR->current_src = temp_src;
    }
    else
    {
        para = (l4c_nbr_cell_info_reg_cnf_struct*)OTA_construct_local_para((kal_uint16) sizeof(l4c_nbr_cell_info_reg_cnf_struct), TD_RESET);
        if(para == NULL)
        {
            track_cust_restart(40, 0);
        }
        para->rat_mode = msg->rat_mode;
        para->is_nbr_info_valid = msg->is_nbr_info_valid;
        kal_mem_cpy(&para->ps_nbr_cell_info_union,
                    &msg->ps_nbr_cell_info_union,
                    sizeof(para->ps_nbr_cell_info_union));
        l4c_nbr_sendmsg(para, NULL, dest_id,
                        msg_name,
                        INVALID_SAP);
    }
}
#endif

#endif /* CUST_NBR_CELL_INFO */




extern void l4csmsal_send_cnf_hdlr(local_para_struct *local_para_ptr, peer_buff_struct *peer_buff_ptr);
extern void $Super$$l4csmsal_send_cnf_hdlr(local_para_struct *local_para_ptr, peer_buff_struct *peer_buff_ptr);
/******************************************************************************

 * ----------------------------------------
 error_cause : ps_cause_enum MAUI_03480434 wangqi
*******************************************************************************/
void $Sub$$l4csmsal_send_cnf_hdlr(local_para_struct *local_para_ptr, peer_buff_struct *peer_buff_ptr)
{
    l4csmsal_send_cnf_struct *data = (l4csmsal_send_cnf_struct *)local_para_ptr;

    EllLog("====result:%d,error_cause:%d,mr:%d,seq:%d",
           data->result, data->error_cause, data->mr, data->seq_num);

    if(data->result == 0)//send ok
    {
        OTA_track_sms_send_rsp(0);
    }
    else//error
    {
        if(OTA_track_is_timer_run(TRACK_SMS_SEND_CHECK_LOCK_TIMER_ID))
        {
            EllLog("SEDN MSG ERROR");
            OTA_track_sms_send_rsp(-1);
        }
    }

    $Super$$l4csmsal_send_cnf_hdlr(local_para_ptr, peer_buff_ptr);

}

/****************************************************/
/*    ICCID                                         */
/****************************************************/
static char g_iccid[22] = {0};
extern kal_bool $Super$$smu_update_is_simcard_replaced(kal_uint8 *iccid, kal_uint8 simInterface);
kal_bool $Sub$$smu_update_is_simcard_replaced(kal_uint8 *iccid, kal_uint8 simInterface)
{
    int i, j;
    kal_uint8 v;
   
    for(i = 0, j = 0; i < 10; i++)
    {
        v = iccid[i] & 0x0F;
        if(v >= 0x0a)
            g_iccid[j++] = v + 0x37;
        else
            g_iccid[j++] = v + 0x30;
        v = iccid[i] >> 4;
        if(v >= 0x0a)
            g_iccid[j++] = v + 0x37;
        else
            g_iccid[j++] = v + 0x30;
    }
    return $Super$$smu_update_is_simcard_replaced(iccid, simInterface);
}
char *ell_get_iccid(void)
{
    return g_iccid;
}
/******************************************************************************
 *  Function    -  track_drv_charge_batter_status_msg_send
 *
 *  Purpose     -  充电和电池状态消息拦截发送
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 23-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void ell_track_drv_charge_batter_status_msg_send(kal_int8 mode, kal_uint8 battery_status, kal_uint8 battery_level, kal_uint32 volt)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    static charbat_status_struct bak = {0};
    static kal_int8 status = 0;
    module_type src_mod_id = kal_get_active_module_id()/*MOD_UEM*/;
    ilm_struct *ilm_ptr;
    charbat_status_struct *myMsgPtr = NULL;
    /*if(mode == 1 && status <= 1)
    {
        status = 1;
        bak.status = battery_status;
        bak.level = battery_level;
        bak.volt = volt;
    }
    else */
    if(mode == 1)
    {
        myMsgPtr = (charbat_status_struct*) construct_local_para(sizeof(charbat_status_struct), TD_CTRL);
        if(myMsgPtr != NULL)
        {
            myMsgPtr->status = battery_status ;
            myMsgPtr->level = battery_level ;
            myMsgPtr->volt = volt;

            ilm_ptr = (ilm_struct *)allocate_ilm(src_mod_id);
            if(ilm_ptr != NULL)
            {
                ilm_ptr->src_mod_id = src_mod_id;
                ilm_ptr->dest_mod_id = MOD_MMI;
                ilm_ptr->sap_id = INVALID_SAP;
                ilm_ptr->msg_id = (msg_type) MSG_ID_CHARBAT_BMT_STATUS;
                ilm_ptr->local_para_ptr = (local_para_struct*) myMsgPtr;
                ilm_ptr->peer_buff_ptr = NULL;
                msg_send_ext_queue(ilm_ptr);
            }
        }
    }
    else if(mode == 2)
    {
        status = 2;
        if(bak.status != 0)
        {
            myMsgPtr = (charbat_status_struct*) construct_local_para(sizeof(charbat_status_struct), TD_CTRL);
            if(myMsgPtr != NULL)
            {
                myMsgPtr->status = bak.status ;
                myMsgPtr->level = bak.level;
                myMsgPtr->volt = bak.volt;

                ilm_ptr = (ilm_struct *)allocate_ilm(src_mod_id);
                if(ilm_ptr != NULL)
                {
                    ilm_ptr->src_mod_id = src_mod_id;
                    ilm_ptr->dest_mod_id = MOD_MMI;
                    ilm_ptr->sap_id = INVALID_SAP;
                    ilm_ptr->msg_id = (msg_type) MSG_ID_CHARBAT_BMT_STATUS;
                    ilm_ptr->local_para_ptr = (local_para_struct*) myMsgPtr;
                    ilm_ptr->peer_buff_ptr = NULL;
                    msg_send_ext_queue(ilm_ptr);
                }
            }
            bak.status = 0;
        }
    }
}

void app_init_complete_callback(void)
{
    //ell_track_drv_charge_batter_status_msg_send(2, 0, 0, 0);
}

void ell_update_status_set(kal_uint8 arg)
{
    update_status = arg;
}

void ell_update(ilm_struct *ilm_ptr)
{
    switch(ilm_ptr->msg_id)
    {

        case MSG_ID_TASK_IND:
            EllLog("msg_id=MSG_ID_TASK_IND");
            break;

        case MSG_ID_TIMER_EXPIRY:  //定时器超时
            //EllLog ( "msg_id=MSG_ID_TIMER_EXPIRY");
            ell_timer_expiry(ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_GPS_NMEA_DECODE_REQ:  //GPS 解码
            EllLog("msg_id=MSG_ID_GPS_NMEA_DECODE_REQ");
            break;

            /* ABM Socket MSG ID BEGIN */
        case MSG_ID_MMI_ABM_PROFILE_QUERY_IND:
            {
                EllLog("msg_id=MSG_ID_MMI_ABM_PROFILE_QUERY_IND");
                ell_update_Socket_prof_query_ind_hdlr(ilm_ptr->local_para_ptr);
                break;
            }
        case MSG_ID_APP_SOC_NOTIFY_IND:
            {
                EllLog("msg_id=MSG_ID_APP_SOC_NOTIFY_IND");
                ell_update_Socket_notify(ilm_ptr->local_para_ptr);
                break;
            }
        case MSG_ID_APP_SOC_GET_HOST_BY_NAME_IND:
            {
                EllLog("msg_id=MSG_ID_APP_SOC_GET_HOST_BY_NAME_IND");
                ell_update_Socket_get_host_by_name(ilm_ptr->local_para_ptr);
                break;
            }
        case MSG_ID_MMI_ABM_BEARER_INIT_IND:
            {
                EllLog("msg_id=MSG_ID_MMI_ABM_BEARER_INIT_IND");
                ell_update_Socket_store_bearer_init_ind_hdlr(ilm_ptr->local_para_ptr);
                break;
            }
            /*case MSG_ID_MMI_ABM_DEACTIVE_IND:
            {
                EllLog ( "msg_id=MSG_ID_MMI_ABM_DEACTIVE_IND");
                break;
            }*/
        case MSG_ID_MMI_ABM_BEARER_INFO_IND:
            {
                EllLog("msg_id=MSG_ID_MMI_ABM_BEARER_INFO_IND");
                break;
            }
        case MSG_ID_MMI_ABM_RESET_BEARER_INFO_CNF:
            {
                EllLog("msg_id=MSG_ID_MMI_ABM_RESET_BEARER_INFO_CNF");
                break;
            }
        case MSG_ID_SOC_ABM_DEACTIVATE_IND:
            {
                EllLog("msg_id=MSG_ID_SOC_ABM_DEACTIVATE_IND");
                break;
            }
        case MSG_ID_APP_CBM_BEARER_INFO_IND:
            {
                EllLog("msg_id=MSG_ID_APP_CBM_BEARER_INFO_IND");
                break;
            }
        case MSG_ID_MMI_NW_SET_GPRS_CONNECT_TYPE_RSP://网络附着
            {
                EllLog("msg_id=MSG_ID_MMI_NW_SET_GPRS_CONNECT_TYPE_RSP");
                break;
            }
            /* ABM Socket MSG ID END */

        case MSG_ID_MMI_NW_ATTACH_IND:
            EllLog("msg_id=MSG_ID_MMI_NW_ATTACH_IND");
            ell_update_soc_nw_attach_ind(ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_CUST_NBR_CELL_INFO_IND:
            EllLog("MSG_ID_CUST_NBR_CELL_INFO_IND");
            break;

        default:
            EllLog("msg id undefined ! %d (%d,%d,%d,%d,%d\t%d,%d,%d,%d,%d)", ilm_ptr->msg_id,
                   MSG_ID_TASK_IND,
                   MSG_ID_TIMER_EXPIRY,
                   MSG_ID_GPS_NMEA_DECODE_REQ,
                   MSG_ID_MMI_NW_SET_GPRS_CONNECT_TYPE_RSP,
                   MSG_ID_MMI_NW_ATTACH_IND,
                   MSG_ID_MMI_ABM_PROFILE_QUERY_IND, /* 15308 */
                   MSG_ID_APP_SOC_NOTIFY_IND,/* 15370 */
                   MSG_ID_APP_CBM_BEARER_INFO_IND,/* 15349 */
                   MSG_ID_L4C_NBR_CELL_INFO_REG_REQ,/* 1351 */
                   MSG_ID_MMI_SMU_GET_PREFERRED_OPER_LIST_REQ);/* 14074 */
            break;
    }
}

static kal_timerid l_ctimer_handle = 0;
static void PowerLedFlashOFF(void* arg);
static void PowerLedFlashON(void* arg)
{
#if defined(__GT03F__)
    pmu_drv_bl_isink(ISINK2, KAL_TRUE);
#elif defined(__GT300S__)
    GPIO_ModeSetup(50, 0);
    GPIO_InitIO(1, 50);
    GPIO_WriteIO(1, 50);
    pmu_drv_bl_isink(ISINK2, 0);
#else

    pmu_drv_bl_isink(ISINK0, KAL_TRUE);
#endif
    kal_set_timer(l_ctimer_handle, (kal_timer_func_ptr)PowerLedFlashOFF, NULL, KAL_TICKS_100_MSEC * 10, 0);
}
static void PowerLedFlashOFF(void* arg)
{
#if defined(__GT03F__)
    pmu_drv_bl_isink(ISINK2, KAL_FALSE);
#elif defined(__GT300S__)
    GPIO_ModeSetup(50, 0);
    GPIO_InitIO(1, 50);
    GPIO_WriteIO(0, 50);
    pmu_drv_bl_isink(ISINK2, 1);
#else
    pmu_drv_bl_isink(ISINK0, KAL_FALSE);
#endif
    kal_set_timer(l_ctimer_handle, (kal_timer_func_ptr)PowerLedFlashON, NULL, KAL_TICKS_100_MSEC * 30, 0);
}

void mmi_main(ilm_struct *ilm_ptr)
{
#if defined (__GPS_TRACK__)
    if(INT_Get_PowerOn_Type() == CHRPWRON)
    {
        if(MSG_ID_CHARBAT_BMT_STATUS == ilm_ptr->msg_id)//PMIC_CHARGE_COMPLETE
        {
            charbat_status_struct *localBuff_p = (charbat_status_struct*) ilm_ptr->local_para_ptr;
            pmic_status_enum status = localBuff_p->status;
            EllLog("%d,%d", localBuff_p->status, l_ctimer_handle);
#if defined(MT6261)
#if defined( __GT03F__) ||defined( __GT300S__)
            if(PMIC_CHARGE_COMPLETE == status && l_ctimer_handle == 0)
            {
                l_ctimer_handle = (kal_timerid)kal_create_timer("M_TIMER");
                PowerLedFlashON((void*)0);
            }
#else
#if defined( __GT300S__)
            //LED
            GPIO_ModeSetup(47, 0);
            GPIO_InitIO(1, 47);
            GPIO_WriteIO(0, 47);
            GPIO_ModeSetup(48, 0);
            GPIO_InitIO(1, 48);
            GPIO_WriteIO(0, 48);
#endif
#endif
#endif
        }
        return;

    }

    if(update_status == 1)  //升级模式
    {
        ell_update(ilm_ptr);
    }
    else if(update_status == 2)  //正常干活
    {
        kal_int32 cc = msg_get_extq_messages();
        if(cc > 10)
        {
            EllLog("%d", cc);
        }
        OTA_track_app_main(ilm_ptr);
    }
    else
    {
        EllLog("应用还未加载，消息被丢弃 %d", ilm_ptr->msg_id);
    }
#else  /* __GPS_TRACK__ */
    if(ilm_ptr->msg_id >= MSG_ID_L4A_CODE_BEGIN
            && ilm_ptr->msg_id < MSG_ID_L4A_CODE_END)
    {
        //kal_prompt_trace(MOD_MMI,"MMI main received message %d",ilm_ptr->msg_id);
    }
#endif /* __GPS_TRACK__ */
}





#endif


