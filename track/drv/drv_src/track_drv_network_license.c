/******************************************************************************
 * track_drv_network_license.c - 
 * 
 * Copyright .
 * 
 * DESCRIPTION: - 
 *
 *      ���������֤   	__NETWORK_LICENSE__
 *
 *          ����������Ʒ CTA/3C/CE/FCC ��֤���
 *----------------------------------------------------------------------------------
 * 
 *  1���������ӷ��������������ƣ�����Ӱ����������Ƶ���ԣ�
 *
 *  2������30���Զ��������磬���㽨��GPRS�������ӣ�
 *
 *  3����������112��û�а����򿪻�20s�Զ�����112��������Ƶ�������ӣ����������Զ�������
 *
 *-----------------------------------------------------------------------------------
 * modification history
 * --------------------
 * v1.0   2013-04-02,  chengjun create this file
 * 
 ******************************************************************************/

#if defined (__NETWORK_LICENSE__)

/****************************************************************************
*  Include Files                        ����ͷ�ļ�
*****************************************************************************/
#include "stack_ltlcom.h"
#include "stack_msgs.h"
#include "stdlib.h"
#include "kal_public_defs.h"
#include "track_os_data_types.h"
#include "track_os_timer.h"
#include "l4c_common_enum.h"
#include "GlobalConstants.h"

/*****************************************************************************
*  Define                               �궨��
******************************************************************************/

/*****************************************************************************
*  Typedef  Enum                        ö�ٶ���
*****************************************************************************/

/*****************************************************************************
*  Struct                               ���ݽṹ����
******************************************************************************/
typedef struct
{
    LOCAL_PARA_HDR
    kal_uint8	type;   /* l4c_gprs_connect_type_enum */
}nw_set_gprs_connect_type_req_struct;

typedef struct
{
    LOCAL_PARA_HDR
    kal_bool	result;
}nw_set_gprs_connect_type_rsp_struct;
    
/*****************************************************************************
*  Local variable                       �ֲ�����
*****************************************************************************/

/****************************************************************************
*  Global Variable                      ȫ�ֱ���
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/
extern void track_press_key_dial_112(void);
/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/
/******************************************************************************
 *  Function    -  track_drv_make_call_112
 * 
 *  Purpose     -  ����112
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 02-04-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_drv_make_call_112(void)
{
    track_press_key_dial_112();
}

#if 1
/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/
#endif


/******************************************************************************
 *  Function    -  track_drv_network_license_key_make_call
 * 
 *  Purpose     -  ������̵�ע�����112
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 02-04-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_network_license_key_make_call(void)
{
#if defined(__GT02D__) || defined(__GT500__) || defined(__GT03F__) || defined(__GT300__)|| defined(__GT300S__) || defined(__GW100__) ||defined(__GT02F__)||defined(__GT300S__)
//�������--ע�ᰴ����Ӧ
    track_drv_set_key_handler(track_drv_make_call_112, KEY_SOS, KEY_EVENT_DOWN);

#elif defined(__BD300__)
//�ж��Ͱ���--�жϴ����ص�
    track_drv_make_call_112();

#elif defined (__MT200__)|| defined(__GT310__)|| defined(__GT530__) || defined(__GT740__) || defined(__V20__) || defined(__GT800__)|| defined(__ET310__)|| defined(__ET320__)|| defined (__NT33__)|| defined(__GT370__)|| defined (__GT380__) || defined(__NT36__) ||defined(__ET350__) || defined(__NT37__)|| defined(__JM81__)|| defined(__HVT001__)||defined (__JM66__) || defined(__GT420D__)
//û�а���--����20���������У���ǰע��

#else
#error "please define project_null"
#endif
}

/******************************************************************************
 *  Function    -  track_drv_network_license_eint_make_call
 * 
 *  Purpose     -  �ж��Ͱ�����Ӧ����112
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 02-04-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_network_license_eint_make_call(void)
{
#if defined(__GT02D__) || defined(__GT03F__) || defined(__GT800__) || defined (__GT300S__)
//�������--ע�ᰴ����Ӧ

#elif defined(__GT06B__) || defined(__GT02E__) || defined(__BD300__) || defined(__GT500__) || defined(__GW100__) || defined(__ET130__) || defined(__NT23__) || defined(__GT06D__)|| defined(__NT36__)|| defined(__NT37__)
//�ж��Ͱ���--�жϴ����ص�
    track_drv_make_call_112();

#elif defined(__GT740__) || defined(__GT310__)|| defined(__V20__) || defined(__ET310__)|| defined(__ET320__)|| defined (__NT33__)|| defined(__GT370__)|| defined (__GT380__) ||defined(__ET350__) || defined(__JM81__)|| defined(__HVT001__)||defined (__JM66__) || defined(__GT420D__)
//û�а���--����20���������У���ǰע��

#else
#error "please define project_null"
#endif

}

/******************************************************************************
 *  Function    -  track_drv_network_license_auto_make_call
 * 
 *  Purpose     -  û�а����Ŀ���ע��20������112
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 02-04-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_network_license_auto_make_call(int ms)
{
#if defined (__NETWORK_LICENSE_NOCALL__)
	return;
#endif /* __NETWORK_LICENSE_NOCALL__ */
    //û�а���--����20���������У���ǰע��
    tr_start_timer(NETWORK_LICENSE_AUTO_MAKE_CALL_TIMER_ID,ms,track_drv_make_call_112);
}

/******************************************************************************
 *  Function    -  track_drv_network_license_auto_answer_action
 * 
 *  Purpose     -  ���������Զ�����
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 02-04-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_network_license_auto_answer_action(void)
{
#if defined (__NETWORK_LICENSE_NOCALL__)
	track_cust_call_hangup();
	return;
#endif /* __NETWORK_LICENSE_NOCALL__ */
        track_send_at_accept_call();
}
#if 0//�Ѿ����������ӵ�ʱ�򴴽�//modify by zengping & liujunwu

/******************************************************************************
 *  Function    -  track_drv_network_license_gprs_attach_req
 * 
 *  Purpose     -  GPRS��������
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 02-04-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_network_license_gprs_attach_req(void)
{
    ilm_struct *ilm_ptr;
    nw_set_gprs_connect_type_req_struct *myMsgPtr = NULL;

    myMsgPtr = (nw_set_gprs_connect_type_req_struct*) construct_local_para(sizeof(nw_set_gprs_connect_type_req_struct),TD_CTRL);
    myMsgPtr->type = L4C_WHEN_NEEDED;

    ilm_ptr = allocate_ilm(MOD_MMI);
    ilm_ptr->src_mod_id = MOD_MMI;
    ilm_ptr->dest_mod_id = MOD_L4C;
    ilm_ptr->sap_id = INVALID_SAP;
    ilm_ptr->msg_id = (msg_type) MSG_ID_MMI_NW_SET_GPRS_CONNECT_TYPE_REQ;
    ilm_ptr->local_para_ptr = (local_para_struct*) myMsgPtr;
    ilm_ptr->peer_buff_ptr = NULL;
    msg_send_ext_queue(ilm_ptr);  
}


/******************************************************************************
 *  Function    -  track_drv_network_license_gprs_attach_rsp
 * 
 *  Purpose     -  GPRS������Ӧ 
 * 
 *  Description -  MOD���̲ü���L4C�յ�����������󣬵������ؽ������˸ú������ᱻִ��! 
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 02-04-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_network_license_gprs_attach_rsp(void *msg)
{
    nw_set_gprs_connect_type_rsp_struct *gprs_type=(nw_set_gprs_connect_type_rsp_struct *)msg;

    LOGS("gprs_attach result=%d",gprs_type->result);    
}
#endif /* 0 */


#endif /* __NETWORK_LICENSE__ */

