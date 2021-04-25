
/******************************************************************************
 * ell_timer.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        �Զ��嶨ʱ�ص�
 *
 *  ˵��:  ����Ķ�ʱ����ʱͨ���¼����� ��
     ��ȷ��ʱ����ֱ�ӵ��ã���ʱ��Ҫ��ߣ�ֻ����LED����
 *
 * modification history
 * --------------------
 * v1.0   2012-07-15,  chengjun create this file
 *
 ******************************************************************************/

/****************************************************************************
* Include Files             ����ͷ�ļ�
*****************************************************************************/
#include "kal_public_api.h"
#include "ell_timer.h"

/*****************************************************************************
 *  Define			    �궨��
*****************************************************************************/


/*****************************************************************************
* Typedef  Enum         ö��
*****************************************************************************/


/*****************************************************************************
 *  Struct			    ���ݽṹ����
*****************************************************************************/
typedef struct
{
    event_scheduler *event_scheduler_ptr;
} ell_context_struct;

typedef struct
{
    eventid event_id;
    kal_uint32 tick;
    kal_timer_func_ptr callback_func;
    void *arg;
} ell_timer_table_struct;

typedef struct
{
    LOCAL_PARA_HDR
    kal_hisr_func_ptr app;
    kal_timer_func_ptr appext;
    void *arg;
} TaskMsgQueue_struct;

/*****************************************************************************
* Local variable            �ֲ�����
*****************************************************************************/
static ell_context_struct ell_context = {0};
static ell_timer_table_struct ell_timer_table[MAX_NUM_OF_ELL_TIMER];

/****************************************************************************
* Global Variable           ȫ�ֱ���
*****************************************************************************/


/****************************************************************************
* Global Variable - Extern          ����ȫ�ֱ���
*****************************************************************************/


/*****************************************************************************
 *  Global Functions	- Extern	    �����ⲿ����
*****************************************************************************/


/*****************************************************************************
 *  Local Functions	            ���غ���
*****************************************************************************/
#if 0

#endif /* 0 */



/******************************************************************************
 *  Function    -  ell_timer_expiry_callback
 *
 *  Purpose     -  ��ʱ�ص�ִ��
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 19-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void ell_timer_expiry_callback(void *arg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint8 *timer_id = (kal_uint8*) arg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if((kal_uint32) timer_id < (kal_uint32) MAX_NUM_OF_ELL_TIMER)
    {
        ell_timer_table[(kal_uint32)(timer_id)].event_id = NULL;
        (ell_timer_table[(kal_uint32)(timer_id)].callback_func)(ell_timer_table[(kal_uint32)(timer_id)].arg);
    }
}

#if 1
/*****************************************************************************
 *  Global Functions            ȫ�ֺ���
*****************************************************************************/
#endif


/******************************************************************************
 *  Function    -  ell_timer_expiry
 *
 *  Purpose     -  ��ʱ��Ϣ����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 19-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void ell_timer_expiry(ilm_struct *ilm_ptr)
{
    evshed_timer_handler(ell_context.event_scheduler_ptr);
}

/******************************************************************************
 *  Function    -  ell_stop_timer
 *
 *  Purpose     -  ֹͣ��ʱ��
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 19-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void ell_stop_timer(kal_uint16 timer_id)
{
    if(ell_context.event_scheduler_ptr == NULL)
    {
        ell_context.event_scheduler_ptr = evshed_create("ell base timer", MOD_MMI, 0, 0);
    }
    
    if(timer_id >= MAX_NUM_OF_ELL_TIMER)
    {
        return;
    }

    if(ell_timer_table[timer_id].event_id != NULL)
    {
        evshed_cancel_event(ell_context.event_scheduler_ptr, &(ell_timer_table[timer_id].event_id));
        ell_timer_table[timer_id].event_id = NULL;
    }
}

void ell_start_timer(kal_uint16 timer_id, kal_uint32 delay, FuncPtr funcPtr)
{
    kal_uint32 temp;

    if(timer_id>=MAX_NUM_OF_ELL_TIMER)
    {
        return;
    }

    if(delay==0)
    {
        return;
    }

    temp = delay * 13 / 60 + 1;    /* ~= x 1/4.615 */
    
    ell_stop_timer(timer_id);
    ell_timer_table[timer_id].callback_func = (kal_timer_func_ptr)funcPtr;
    ell_timer_table[timer_id].arg = (void*)timer_id;
    ell_timer_table[timer_id].tick = kal_get_systicks() + temp;
    ell_timer_table[timer_id].event_id = evshed_set_event(
            ell_context.event_scheduler_ptr,
            (kal_timer_func_ptr) ell_timer_expiry_callback,
            (void*)timer_id,
            temp);

}

/******************************************************************************
 *  Function    -  ell_start_timer
 *
 *  Purpose     -  ���ö�ʱ��
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 19-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void ell_start_timer_ext(kal_uint16 timer_id, kal_uint32 period, kal_timer_func_ptr timer_expiry, void *arg)
{
    kal_uint32 temp ;

    if(ell_context.event_scheduler_ptr == NULL)
    {
        ell_context.event_scheduler_ptr = evshed_create("ell base timer", MOD_MMI, 0, 0);
    }
    
    if(timer_id>=MAX_NUM_OF_ELL_TIMER)
    {
        return;
    }
    
    if(period==0)
    {
        return;
    }
    
    temp=period * 13 / 60 + 1;    /* ~= x 1/4.615 */

    ell_stop_timer(timer_id);
    ell_timer_table[timer_id].callback_func = timer_expiry;
    ell_timer_table[timer_id].arg = arg;
    ell_timer_table[timer_id].tick = kal_get_systicks() + temp;
    ell_timer_table[timer_id].event_id = evshed_set_event(
            ell_context.event_scheduler_ptr,
            (kal_timer_func_ptr) ell_timer_expiry_callback,
            (void*)timer_id,
            temp);
}

/******************************************************************************
 *  Function    -  ell_is_timer_run
 *
 *  Purpose     -  �ж϶�ʱ���Ƿ�������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 26-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_bool ell_is_timer_run(kal_uint16 timer_id)
{
    if(timer_id >= MAX_NUM_OF_ELL_TIMER)
    {
        return KAL_FALSE;
    }

    if(ell_timer_table[timer_id].event_id != NULL)
    {
        return KAL_TRUE;
    }
    return KAL_FALSE;
}

#if 0
void track_os_intoTaskMsgQueue(kal_hisr_func_ptr app)
{
    TaskMsgQueue_struct *myMsgPtr = NULL;

    myMsgPtr = (TaskMsgQueue_struct*) construct_local_para(sizeof(TaskMsgQueue_struct), TD_CTRL);
    if(myMsgPtr == NULL)
    {
        track_cust_restart(54, 0);
    }
    myMsgPtr->app = app;
    myMsgPtr->appext = NULL;
    track_fun_msg_send(MSG_ID_TASK_IND, myMsgPtr, MOD_MMI, MOD_MMI);
}

void track_os_intoTaskMsgQueue2(kal_hisr_func_ptr app)
{
    TaskMsgQueue_struct *myMsgPtr = NULL;

    myMsgPtr = (TaskMsgQueue_struct*) construct_local_para(sizeof(TaskMsgQueue_struct), TD_CTRL);
    if(myMsgPtr == NULL)
    {
        track_cust_restart(54, 0);
    }
    myMsgPtr->app = app;
    myMsgPtr->appext = NULL;
    track_fun_msg_send(MSG_ID_TASK_IND, myMsgPtr, kal_get_active_module_id(), MOD_MMI);
}

void track_os_intoTaskMsgQueueExt(kal_timer_func_ptr app, void *arg)
{
    TaskMsgQueue_struct *myMsgPtr = NULL;

    myMsgPtr = (TaskMsgQueue_struct*) construct_local_para(sizeof(TaskMsgQueue_struct), TD_CTRL);
    if(myMsgPtr == NULL)
    {
        track_cust_restart(55, 0);
    }
    myMsgPtr->app = NULL;
    myMsgPtr->appext = app;
    myMsgPtr->arg = arg;
    track_fun_msg_send(MSG_ID_TASK_IND, myMsgPtr, MOD_MMI, MOD_MMI);
}

void track_os_task_expiry(void *local_para_ptr)
{
    TaskMsgQueue_struct *myMsgPtr = (TaskMsgQueue_struct *)local_para_ptr;
    EllLog("");
    if(myMsgPtr->app != NULL)
    {
        myMsgPtr->app();
    }
    else if(myMsgPtr->appext != NULL)
    {
        myMsgPtr->appext(myMsgPtr->arg);
    }
}
#endif /* 0 */

