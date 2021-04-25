/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2006
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
 *   combo_flash_id.h
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   This Module defines the Flash ID table for Combo Memory.
 *
 * Author:
 * -------
 * -------
 *
 *   Memory Device database last modified on 2014/10/31
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * removed!
 * removed!
 * removed!
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

COMBO_MEM_TYPE_MODIFIER COMBO_MEM_TYPE_NAME COMBO_MEM_INST_NAME = {
    COMBO_MEM_STRUCT_HEAD
    {   // MTKSIP_6261_SF_32_01
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xC2, 0x25, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    },
    {   // MTKSIP_6261_SF_32_02
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xEF, 0x40, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    },
    {   // MTKSIP_6261_SF_32_03
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xC2, 0x20, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    },
    {   // MTKSIP_6261_SF_32_04
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xEF, 0x70, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    },
    {   // MTKSIP_6261_SF_32_05
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xC8, 0x60, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    },
    {   // MTKSIP_6261_SF_32_06
        CMEM_TYPE_SERIAL_NOR_FLASH,
        3,  // Valid ID length
        {0xC2, 0x25, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00}  // Flash ID
    }

    COMBO_MEM_STRUCT_FOOT
};
