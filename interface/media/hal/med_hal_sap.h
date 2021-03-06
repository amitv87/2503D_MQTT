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
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained
 *  herein is confidential. The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of MediaTek Inc. (C) 2003
 *
 *******************************************************************************/

 /*******************************************************************************
 * Filename:
 * ---------
 *   med_sap.h
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 *   Media task HAL related message identity definition
 *
 * Author:
 * -------
 * -------
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * removed!
 *
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
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
   /*------------------------MED_HAL_MSG_CODE_BEGIN ---------------*/
    MSG_ID_MED_STARTUP_REQ = MSG_ID_MED_HAL_CODE_BEGIN,
    MSG_ID_MED_STARTUP_CNF,
    MSG_ID_MEDIA_IN_PROC_CALL_REQ,
    MSG_ID_MEDIA_IN_PROC_CALL_CNF,

    MSG_ID_MEDIA_AUD_SET_AUDIO_MODE_REQ,
    MSG_ID_MED_AUD_HAL_CODE_BEGIN = MSG_ID_MEDIA_AUD_SET_AUDIO_MODE_REQ,
    MSG_ID_MEDIA_AUD_SET_AUDIO_MODE_CNF,
    MSG_ID_MEDIA_AUD_GET_AUDIO_PROFILE_REQ,
    MSG_ID_MEDIA_AUD_GET_AUDIO_PROFILE_CNF,
    MSG_ID_MEDIA_AUD_SET_AUDIO_PROFILE_REQ,
    MSG_ID_MEDIA_AUD_SET_AUDIO_PROFILE_CNF,
    MSG_ID_MEDIA_AUD_GET_AUDIO_PARAM_REQ,
    MSG_ID_MEDIA_AUD_GET_AUDIO_PARAM_CNF,
    MSG_ID_MEDIA_AUD_SET_AUDIO_PARAM_REQ,
    MSG_ID_MEDIA_AUD_SET_AUDIO_PARAM_CNF,

    MSG_ID_MEDIA_AUD_GET_VOLUME_REQ,
    MSG_ID_MEDIA_AUD_GET_VOLUME_CNF,
    MSG_ID_MEDIA_AUD_SET_VOLUME_REQ,
    MSG_ID_MEDIA_AUD_SET_VOLUME_CNF,
    MSG_ID_MEDIA_AUD_SET_DEVICE_REQ,
    MSG_ID_MEDIA_AUD_SET_DEVICE_CNF,
    MSG_ID_MEDIA_AUD_MUTE_REQ,
    MSG_ID_MEDIA_AUD_MUTE_CNF,
    MSG_ID_GET_MEDIA_MUTE_STATUS_REQ,//Note add
    MSG_ID_GET_MEDIA_MUTE_STATUS_CNF,
    MSG_ID_MEDIA_AUD_SET_HEADSET_MODE_MIC_REQ,

    MSG_ID_MEDIA_AUD_PLAY_BY_ID_REQ,
    MSG_ID_MEDIA_AUD_PLAY_BY_ID_CNF,
    MSG_ID_MEDIA_AUD_STOP_BY_ID_REQ,
    MSG_ID_MEDIA_AUD_STOP_BY_ID_CNF,
    MSG_ID_MEDIA_AUD_PLAY_BY_STRING_REQ,
    MSG_ID_MEDIA_AUD_PLAY_BY_STRING_CNF,
    MSG_ID_MEDIA_AUD_STOP_BY_STRING_REQ,
    MSG_ID_MEDIA_AUD_STOP_BY_STRING_CNF,
    MSG_ID_MEDIA_AUD_PLAY_BY_FILE_REQ,
    MSG_ID_MEDIA_AUD_PLAY_BY_FILE_CNF,
    MSG_ID_MEDIA_AUD_STOP_REQ,
    MSG_ID_MEDIA_AUD_STOP_CNF,
    MSG_ID_MEDIA_AUD_PLAY_FINISH_IND,

    MSG_ID_MEDIA_AUD_EMSD_MONITOR_REQ,
    MSG_ID_MEDIA_AUD_EMSD_CODE_BEGIN = MSG_ID_MEDIA_AUD_EMSD_MONITOR_REQ,
    MSG_ID_MEDIA_AUD_EMSD_MONITOR_CNF,
    MSG_ID_MEDIA_AUD_EMSD_SET_REQ,
    MSG_ID_MEDIA_AUD_EMSD_SET_CNF,
    MSG_ID_MEDIA_AUD_EMSD_PUSH_REQ,
    MSG_ID_MEDIA_AUD_EMSD_PUSH_CNF,
    MSG_ID_MEDIA_AUD_EMSD_PULL_IND,
    MSG_ID_MEDIA_AUD_EMSD_HACK_IND,
    MSG_ID_MEDIA_AUD_EMSD_LACK_IND,
    MSG_ID_MEDIA_AUD_EMSD_SYNC_IND,
    MSG_ID_MEDIA_AUD_EMSD_PSAP_MONITOR_REQ,
    MSG_ID_MEDIA_AUD_EMSD_PSAP_MONITOR_CNF,
    MSG_ID_MEDIA_AUD_EMSD_PSAP_PULL_REQ,
    MSG_ID_MEDIA_AUD_EMSD_PSAP_PULL_CNF,
    MSG_ID_MEDIA_AUD_EMSD_PSAP_DATA_IND,
    MSG_ID_MEDIA_AUD_EMSD_CODE_END = MSG_ID_MEDIA_AUD_EMSD_PSAP_DATA_IND,


    MSG_ID_MEDIA_AUD_SP_SET_MODE_REQ,
    MSG_ID_MEDIA_AUD_SP_SET_MODE_CNF,
    MSG_ID_MEDIA_AUD_SP_SET_FIR_COEFF_REQ,
    MSG_ID_MEDIA_AUD_GET_AUDIO_WB_INPUT_FIR_PARAM_REQ,
    MSG_ID_MEDIA_AUD_GET_AUDIO_WB_INPUT_FIR_PARAM_CNF,
    MSG_ID_MEDIA_AUD_GET_AUDIO_WB_OUTPUT_FIR_PARAM_REQ,
    MSG_ID_MEDIA_AUD_GET_AUDIO_WB_OUTPUT_FIR_PARAM_CNF,
    MSG_ID_MEDIA_AUD_GET_AUDIO_WB_MODE_PARAM_REQ,
    MSG_ID_MEDIA_AUD_GET_AUDIO_WB_MODE_PARAM_CNF,
    MSG_ID_MEDIA_AUD_SET_AUDIO_WB_INPUT_FIR_PARAM_REQ,
    MSG_ID_MEDIA_AUD_SET_AUDIO_WB_INPUT_FIR_PARAM_CNF,
    MSG_ID_MEDIA_AUD_SET_AUDIO_WB_OUTPUT_FIR_PARAM_REQ,
    MSG_ID_MEDIA_AUD_SET_AUDIO_WB_OUTPUT_FIR_PARAM_CNF,
    MSG_ID_MEDIA_AUD_SET_AUDIO_WB_MODE_PARAM_REQ,
    MSG_ID_MEDIA_AUD_SET_AUDIO_WB_MODE_PARAM_CNF,
    MSG_ID_MEDIA_AUD_SP_SET_ENHANCE_MODE_REQ,
	MSG_ID_MEDIA_AUD_SET_AT_AUDIO_SPEECH_INFO_REQ,

    MSG_ID_MED_HAL_AUD_CODE_END = MSG_ID_MEDIA_AUD_SP_SET_ENHANCE_MODE_REQ,

    MSG_ID_MED_HAL_CODE_END = MSG_ID_MED_HAL_AUD_CODE_END,

/*------------------------MED_HAL_MSG_CODE_END -----------------*/

