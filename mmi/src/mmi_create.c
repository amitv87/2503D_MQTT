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

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   dummy_fmi.c
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 *   This file is intends for CC task create function and message handler 
 *   entry point.
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
 *
 * removed!
 * removed!
 * removed!
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifndef PLUTO_MMI

/*****************
Include Statements
******************/
//#include "kal_release.h"      	/* Basic data type */

//#include "stack_common.h"  
//#include "stack_msgs.h"
#include "app_ltlcom.h"       	/* Task message communiction */

#include "syscomp_config.h"
#include "task_config.h"      	/* Task creation */

//#include "stacklib.h"        	/* Basic type for dll, evshed, stacktimer */

//#include "event_shed.h"       	/* Event scheduler */

//#include "stack_timer.h"      	/* Stack timer */
#include "kal_public_defs.h"
#include "kal_general_types.h"
#include "kal_public_api.h"
#include "stack_ltlcom.h"
#include "stack_config.h"
#include "kal_trace.h"
#include "track_os_timer.h"

void mmi_task_main(task_entry_struct *task_entry_ptr);
void mmi_main(ilm_struct *ilm_ptr);

/*********************
Function Definitions 
**********************/

#if defined (__GPS_TRACK__)
#include "ell_os.h"


kal_bool mmi_task_init(task_indx_type task_indx)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/


    return KAL_TRUE;
}
#endif /* __GPS_TRACK__ */
kal_bool mmi_create(comptask_handler_struct **handle)
{
    static comptask_handler_struct mmi_handler_info =
    {
        mmi_task_main,			/* task entry function */
#if defined (__GPS_TRACK__)
        mmi_task_init,
#else
        NULL,			/* task initialization function */
#endif /* __GPS_TRACK__ */
        NULL,		/* task configuration function */
        NULL,				/* task reset handler */
        NULL,				/* task termination handler */
    };

	*handle = &mmi_handler_info;
	return KAL_TRUE;
}
extern PW_CTRL_POWER_ON_REASON INT_Get_PowerOn_Type(void);
extern kal_bool Ell_Check_SDS_Data(void);

void ELL_SDS_Main(void)
{
    ell_main();
    OTA_track_timer_init();
    OTA_track_main();
}
/******************************************************************************
 *  Function    -  track_mmi_charge_poweron_led_interface
 *
 *  Purpose     -  关机充电LED控制
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-08-01,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_mmi_charge_poweron_led_interface(void)
{
#if defined( __GT03F__)
    kal_prompt_trace(MOD_TST,"=====track_mmi_charge_poweron_led_interface");
    pmu_drv_bl_isink(ISINK2, KAL_TRUE);
    GPIO_ModeSetup(51, 0);
    GPIO_InitIO(0, 51);
    GPIO_WriteIO(0, 51);

#elif defined ( __GT03D__)
    pmu_drv_bl_isink(ISINK3, KAL_TRUE);
    //03D新152关机充电时要拉低RESET脚,才能按键开机
    GPIO_ModeSetup(74, 0);
    GPIO_InitIO(0, 74);
    GPIO_WriteIO(0, 74);
    
#elif defined(__GW100__)
    if(Ell_Check_SDS_Data())
    {
        ell_main();
        OTA_track_timer_init();
    }
#endif
}
void mmi_task_main(task_entry_struct *task_entry_ptr)
{
    ilm_struct current_ilm;
    kal_uint32 my_index;
    static kal_bool first = KAL_TRUE;

    kal_get_my_task_index(&my_index);
    kal_prompt_trace(MOD_TST,"=====mmi_task_main:%d,powerOnType:%d", first, INT_Get_PowerOn_Type() );

    if(first)
    {
        first = KAL_FALSE;
        if(INT_Get_PowerOn_Type() == CHRPWRON)
        {
            track_mmi_charge_poweron_led_interface();
        }
        else
        {
            if(Ell_Check_SDS_Data())
            {
                ELL_SDS_Main();
            }
        }
    }

    while(1)
    {
        receive_msg_ext_q(task_info_g[task_entry_ptr->task_indx].task_ext_qid, &current_ilm);
        stack_set_active_module_id(my_index, current_ilm.dest_mod_id);
        mmi_main(&current_ilm);
        free_ilm(&current_ilm);
    }
}

#endif


