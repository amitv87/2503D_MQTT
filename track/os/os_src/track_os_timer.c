
/******************************************************************************
 * track_timer.c -
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
#include "track_os_timer.h"
#include "kal_public_api.h"
#include "track_os_ell.h"

/*****************************************************************************
 *  Define			    �궨��
*****************************************************************************/


/*****************************************************************************
* Typedef  Enum         ö��
*****************************************************************************/


/*****************************************************************************
 *  Struct			    ���ݽṹ����
*****************************************************************************/

/*****************************************************************************
* Local variable            �ֲ�����
*****************************************************************************/
static track_context_struct track_context;
static track_context_struct *track_context_p = &track_context;
static track_timer_table_struct track_timer_table[MAX_NUM_OF_TRACK_TIMER] = {0};

kal_timerid l_ctimer_handle = 0;                           //���Ķ�ʱ�����

ctimer_struct l_ctimer_buf[MAX_CTIMERS] = {0};             //��ʱ����������

kal_uint32 l_TimeBegin = 0;                                //���Ķ�ʱ������ʱ��
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
static void CTimer_KalTimer_GetMinInterval();
#if 0

#endif /* 0 */



/******************************************************************************
 *  Function    -  track_timer_expiry_callback
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
static void track_timer_expiry_callback(void *arg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint8 *timer_id = (kal_uint8*) arg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if((kal_uint32) timer_id < (kal_uint32) MAX_NUM_OF_TRACK_TIMER)
    {
        track_timer_table[(kal_uint32)(timer_id)].event_id = NULL;
        (track_timer_table[(kal_uint32)(timer_id)].callback_func)(track_timer_table[(kal_uint32)(timer_id)].arg);
    }
}

/******************************************************************************
 *  Function    -  CTimer_KalTimer_Response
 *
 *  Purpose     -  ��ʱʱ�䵽����Ӧ����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 *
 * ----------------------------------------
 ******************************************************************************/
static void CTimer_KalTimer_Response(void *data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i = 0, n = 0;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    kal_cancel_timer(l_ctimer_handle);

    for(i = 0 ; i < MAX_CTIMERS ; i++)
    {
        if(l_ctimer_buf[i].delay > 0 && l_ctimer_buf[i].funcPtr != NULL)
        {
            l_ctimer_buf[i].time += 1;
            if(l_ctimer_buf[i].delay <= l_ctimer_buf[i].time)
            {
                //���Ķ�ʱ����ʱ���    
            #ifdef TRACK_TIME_TEST 
                l_ctimer_buf[i].delay = 0;
                if(i<=CTIMER_ID_FLASH_NUM_MAX)
                {
                    l_ctimer_buf[i].funcPtr(l_ctimer_buf[i].funcParamPtr);
                }
                else
                {
                    if(l_ctimer_buf[i].param)
                    {
                        l_ctimer_buf[i].param=KAL_FALSE;
                        track_os_intoTaskMsgQueueExt(l_ctimer_buf[i].funcPtr,l_ctimer_buf[i].funcParamPtr);
                    }
                    else
                    {
                        track_os_intoTaskMsgQueue(l_ctimer_buf[i].funcPtr);                       
                    }
                }

            #else
                l_ctimer_buf[i].delay = 0;
                l_ctimer_buf[i].funcPtr(l_ctimer_buf[i].funcParamPtr);
            #endif
            }
        }
    }
    CTimer_KalTimer_GetMinInterval();
}

/******************************************************************************
 *  Function    -  CTimer_KalTimer_GetMinInterval
 *
 *  Purpose     -  ��ȡ��С��ʱ���
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 *
 * ----------------------------------------
 ******************************************************************************/
static void CTimer_KalTimer_GetMinInterval(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 i = 0, n = 0, min = 0;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    for(i = 0 ; i < MAX_CTIMERS ; i++)
    {
        if(l_ctimer_buf[i].delay > 0 && l_ctimer_buf[i].funcPtr != NULL)
        {
            ASSERT(l_ctimer_buf[i].delay  >=  l_ctimer_buf[i].time);//�쳣�������Ӧ�ó��ֵ�
            n = l_ctimer_buf[i].delay - l_ctimer_buf[i].time ;
#ifdef __C_TIMER_DEBUG__
            LOGD(L_SYS, L_V5, "(%d, %d, %d)", n, l_ctimer_buf[i].delay, l_ctimer_buf[i].time);
#endif
            if(((min > n) || (min == 0)) && (n > 0))
            {
                min = n;
            }
        }
    }
#ifdef __C_TIMER_DEBUG__
    LOGD(L_SYS, L_V5, "(%d)"  , min);
#endif
    if(min)
    {
        kal_set_timer(l_ctimer_handle, (kal_timer_func_ptr)CTimer_KalTimer_Response, NULL, KAL_TICKS_100_MSEC, 0);
    }
    else
    {
        kal_cancel_timer(l_ctimer_handle);
    }

}

#if 1
/*****************************************************************************
 *  Global Functions            ȫ�ֺ���
*****************************************************************************/
#endif

/******************************************************************************
 *  Function    -  track_timer_init
 *
 *  Purpose     -  ��ʱ����ʼ��
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 19-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_timer_init(void)
{
    track_context_p->event_scheduler_ptr = (event_scheduler *)evshed_create("TRACK base timer", MOD_MMI, 0, 0);
}


/******************************************************************************
 *  Function    -  track_timer_expiry
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
void track_timer_expiry(ilm_struct *ilm_ptr)
{
    evshed_timer_handler(track_context_p->event_scheduler_ptr);
}

/******************************************************************************
 *  Function    -  track_start_timer
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
void track_start_timer(kal_uint16 timer_id, kal_uint32 period, kal_timer_func_ptr timer_expiry, void *arg)
{
    kal_uint32 temp ;

    if(timer_id>=MAX_NUM_OF_TRACK_TIMER)
    {
        return;
    }
    
    if(period==0)
    {
        return;
    }
    
    temp=period * 13 / 60 + 1;    /* ~= x 1/4.615 */

    track_stop_timer(timer_id);
    track_timer_table[timer_id].callback_func = timer_expiry;
    track_timer_table[timer_id].arg = arg;
    track_timer_table[timer_id].tick = kal_get_systicks() + temp;
    track_timer_table[timer_id].event_id = (eventid)evshed_set_event(
            track_context_p->event_scheduler_ptr,
            (kal_timer_func_ptr) track_timer_expiry_callback,
            (void*)timer_id,
            temp);
}


/******************************************************************************
 *  Function    -  tr_start_timer
 *
 *  Purpose     -  ���ö�ʱ������������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 26-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void tr_start_timer(kal_uint16 timer_id, kal_uint32 delay, FuncPtr funcPtr)
{
    kal_uint32 temp;

    if(timer_id>=MAX_NUM_OF_TRACK_TIMER)
    {
        return;
    }

    if(delay==0)
    {
        return;
    }

    temp = delay * 13 / 60 + 1;    /* ~= x 1/4.615 */
    
    track_stop_timer(timer_id);
    track_timer_table[timer_id].callback_func = (kal_timer_func_ptr)funcPtr;
    track_timer_table[timer_id].arg = (void*)timer_id;
    track_timer_table[timer_id].tick = kal_get_systicks() + temp;
    track_timer_table[timer_id].event_id = (eventid)evshed_set_event(
            track_context_p->event_scheduler_ptr,
            (kal_timer_func_ptr) track_timer_expiry_callback,
            (void*)timer_id,
            temp);

}

void track_timer_Remaining(char *out, kal_uint16 timer_id)
{
    if(timer_id >= MAX_NUM_OF_TRACK_TIMER)
    {
        sprintf(out, "The Timer id %d is invalid.");
    }
    else if(track_timer_table[timer_id].event_id == NULL)
    {
        sprintf(out, "The Timer id %d is STOP.", timer_id);
    }
    else
    {
        sprintf(out, "The Timer id %d In %d milliseconds after that time.", 
            timer_id, 
            kal_ticks_to_milli_secs(track_timer_table[timer_id].tick - kal_get_systicks()));
    }
}

/******************************************************************************
 *  Function    -  track_stop_timer
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
void track_stop_timer(kal_uint16 timer_id)
{
    if(timer_id >= MAX_NUM_OF_TRACK_TIMER)
    {
        return;
    }

    if(track_timer_table[timer_id].event_id != NULL)
    {
        evshed_cancel_event(track_context_p->event_scheduler_ptr, &(track_timer_table[timer_id].event_id));
        track_timer_table[timer_id].event_id = NULL;
    }
}


void tr_stop_timer(kal_uint16 timer_id)
{
    track_stop_timer(timer_id);
}


/******************************************************************************
 *  Function    -  track_is_timer_run
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
BOOL track_is_timer_run(kal_uint16 timer_id)
{
    if(timer_id >= MAX_NUM_OF_TRACK_TIMER)
    {
        return FALSE;
    }

    if(track_timer_table[timer_id].event_id != NULL)
    {
        return TRUE;
    }
    return FALSE;
}


/******************************************************************************
 *  Function    -  CTimer_Start
 *
 *  Purpose     -  �������Ķ�ʱ��
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 22-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
BOOL CTimer_Start(U16 timerid, kal_uint32 delay, kal_timer_func_ptr funcPtr, void* funcParamPtr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 n = 0;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    // Ҫ���ID  ��û�����Χ
    if(timerid <= CTIMER_START || timerid >=  MAX_CTIMERS)
    {
        return FALSE;
    }

    if(delay==0)
    {
        return FALSE;
    }

#ifdef __C_TIMER_DEBUG__
    LOGD(L_SYS, L_V5, "CTimer_Start( ID = %d ,  Delay = %d ,l_ctimer_handle=%d )", timerid ,  delay , l_ctimer_handle);
#endif

    if(0 == l_ctimer_handle)
    {
        l_ctimer_handle = (kal_timerid)kal_create_timer("C_TIMER");
    }
    if(l_ctimer_handle)
    {
        kal_cancel_timer(l_ctimer_handle);
    }

    //���������ö�ʱ���Ĳ������ص�����
    if(delay<100)
    {
        delay=100;
    }
    l_ctimer_buf[timerid].delay = delay / 100;
    l_ctimer_buf[timerid].time  = 0;
    l_ctimer_buf[timerid].funcPtr = funcPtr;
    l_ctimer_buf[timerid].funcParamPtr = funcParamPtr;

    if(l_ctimer_handle)
    {
        CTimer_KalTimer_GetMinInterval();
    }
    return TRUE;

}


/******************************************************************************
 *  Function    -  CTimer_Stop
 *
 *  Purpose     -  ֹͣһ����ȷ��ʱ��
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 *
 * ----------------------------------------
 ******************************************************************************/
BOOL CTimer_Stop(U16 timerid)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 n = 0;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    // Ҫ���ID  ��û�����Χ
    if(timerid <= CTIMER_START || timerid >=  MAX_CTIMERS)
    {
        return FALSE;
    }

#ifdef __C_TIMER_DEBUG__
    LOGD(L_SYS, L_V5, "CTimer_Stop( %d )" , timerid);
#endif

    if(0 == l_ctimer_handle)
    {
        l_ctimer_handle = (kal_timerid)kal_create_timer("C_TIMER");
    }
    if(l_ctimer_handle)
    {
        kal_cancel_timer(l_ctimer_handle);
    }

    //���������ö�ʱ���Ĳ������ص�����
    memset(&l_ctimer_buf[timerid], 0, sizeof(ctimer_struct));

    if(l_ctimer_handle)
    {
        CTimer_KalTimer_GetMinInterval();
    }
    return TRUE;
}



/******************************************************************************
 *  Function    -  CTimer_IsRun
 *
 *  Purpose     -  �ж϶�ʱ���Ƿ�������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 *
 * ----------------------------------------
 ******************************************************************************/
BOOL CTimer_IsRun(U16 timerid)
{
    // Ҫ���ID  ��û�����Χ
    if(timerid <= CTIMER_START || timerid >=  MAX_CTIMERS)
    {
        return FALSE;
    }

    if(l_ctimer_buf[timerid].delay > 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/******************************************************************************
 *  Function    -  CTimer_debug
 *
 *  Purpose     -  ���Ķ�ʱ���������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 *
 * ----------------------------------------
 ******************************************************************************/
void CTimer_debug(void)
{
#ifdef __C_TIMER_DEBUG__
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 i = 0, n = 0;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    kal_prompt_trace(TRACE_INFO, "CTimer_debug ");

    for(i = 0 ; i < MAX_CTIMERS ; i++)
    {
        kal_prompt_trace(TRACE_INFO, "  l_ctimer_buf[%d].delay   = %d   ", i ,  l_ctimer_buf[i].delay);
        kal_prompt_trace(TRACE_INFO, "  l_ctimer_buf[%d].time   = %d   ", i ,  l_ctimer_buf[i].time);
        kal_prompt_trace(TRACE_INFO, "  l_ctimer_buf[%d].funcPtr   = %d   ", i ,  l_ctimer_buf[i].funcPtr);
    }
#endif
}

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
    LOGD(L_APP, L_V6, "");
    if(myMsgPtr->app != NULL)
    {
        myMsgPtr->app();
    }
    else if(myMsgPtr->appext != NULL)
    {
        myMsgPtr->appext(myMsgPtr->arg);
    }
}

