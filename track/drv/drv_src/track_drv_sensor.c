/******************************************************************************
 * track_drv_sensor.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        Sensor ����
 *
 * modification history
 * --------------------
 * v1.0   2012-12-01,  chengjun create this file
 *
 ******************************************************************************/
#if defined (__TRACK_SENSOR__)


/****************************************************************************
* Include Files                   		����ͷ�ļ�
*****************************************************************************/
#include "stack_config.h"
#include "stack_ltlcom.h"
#include "rtc_sw_new.h"
#include "track_cust_main.h"
#include "track_os_log.h"
#include "track_drv_sensor.h"
#include "track_os_ell.h"
/****************************************************************************
* Global Variable - Extern          ����ȫ�ֱ���
*****************************************************************************/


/*****************************************************************************
 *  Define					�궨��
 *****************************************************************************/

/*****************************************************************************
* Typedef  Enum
*****************************************************************************/


/*****************************************************************************
 *  Struct					���ݽṹ����
 *****************************************************************************/


/*****************************************************************************
* Local variable				�ֲ�����
*****************************************************************************/
//nvram_drv_parameter_struct nvram_drv_parameter={0};
static kal_bool    touch_panel_state=KAL_FALSE;

/*****************************************************************************
* Golbal variable				ȫ�ֱ���
*****************************************************************************/
track_sensor_config_data_struct  track_sensor_config_data = {0};


/*****************************************************************************
 *  Global Functions	- Extern		�����ⲿ����
 *****************************************************************************/

/*****************************************************************************
 *  Local Functions			���غ���
 *****************************************************************************/

/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/


/******************************************************************************
 *  Function    -  track_sensor_threshold_convert
 *
 *  Purpose     -  Sensor�ȼ��������ȹ�ϵ
 *
 *  Description -  Ĭ��SENSOR_LEVEL_2
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 01-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static kal_uint8 track_sensor_threshold_convert(sensor_threshold_level level)
{
    sensor_type_enum sensor_type;
    const kal_uint8 threshold_tab[SENSOR_TYPE_MAX-1][5] =
    {
        {14, 16, 18, 20, 20},   //SENSOR_BMA250
#ifdef __BOSCH_BMA250F__
        {23, 25, 27, 29, 31},   //SENSOR_BMA250F
#else
        {14, 16, 18, 20, 20},   //SENSOR_BMA250E
#endif /* __BMA250F__ */
        {18, 19, 20, 21, 22},   //SENSOR_BMA253
        {15, 16, 18, 19, 20},   //SENSOR_KXTJ31057
        {8, 9, 10, 11, 12},     //SENSOR_SC7A20
        {18, 20, 21, 22, 24},	//SENSOR_DA213
#if 0
        {2, 3, 4, 5, 6},        //SENSOR_MC3410
        {5, 6, 10, 15, 30},     //SENSOR_AFA750
        {16, 17, 18, 25, 30}    //SENSOR_MMA8452Q
#endif
    };

    sensor_type = track_drv_get_sensor_type();
    if((sensor_type <= SENSOR_NOT_READY) || (sensor_type >= SENSOR_TYPE_MAX))
    {
        LOGD(L_DRV, L_V1, "sensor type error (%d)", sensor_type);
        return 0;
    }

    if((level <= SENSOR_LEVEL_MIN) || (level >= SENSOR_LEVEL_MAX))
    {
        LOGD(L_DRV, L_V1, "sensor level error (%d)", level);
        return 0;
    }

    return threshold_tab[sensor_type-1][level-1];
    //sensor_type and level  form 1 to 5
}

/******************************************************************************
 *  Function    -  track_drv_sensor_get_xyz
 *
 *  Purpose     -  ��ȡsensoe��������
 *
 *  Description -  1��˲ʱֵ
 *                       2��ע���и���
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 02-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_sensor_get_xyz(short*x, short*y, short*z)
{

}

/******************************************************************************
 *  Function    -  track_drv_sensor_type_init
 *
 *  Purpose     -  ��һͨ����֤����ѯSensor����
 *
 *  Description -
 *
 * modification history
 *
 *  Ϊ��ֹ��죬��ѯ2��ʧ�ܲ�ȷ�ϴ���
 *
 * ----------------------------------------
 * v1.0  , 25-02-2013,  chengjun  written
 * v1.1  , 15-05-2014,  MagicNing  ɾ��sensor����Ӧ������������һֱû��ʹ�ã�δ��Ҳ��һ��������
 * ----------------------------------------
 ******************************************************************************/
static sensor_type_enum track_drv_sensor_type_init(void)
{
    kal_uint8 index;
    kal_int8 result;
    kal_bool sensor_flag = KAL_FALSE;
    static kal_uint8 count = 0;

    LOGD(L_DRV, L_V3, "check %d times", count);
    result = track_sensor_bma250_type_check();
    if(result > 0)
    {
        if(result == 1)
        {
            count = 0;
            return SENSOR_BMA250;
        }
        else if(result == 2)
        {
            count = 0;
            return SENSOR_BMA250E;
        }
        else if(result == 3)
        {
            count = 0;
            return SENSOR_BMA253;
        }
    }

    sensor_flag = OTA_track_sensor_kxtj31057_type_check();
    if(sensor_flag)
    {
        count = 0;
        return SENSOR_KXTJ31057;
    }

    sensor_flag = OTA_track_sensor_sc7a20_type_check();
    if(sensor_flag)
    {
        count = 0;
        return SENSOR_SC7A20;
    }

    sensor_flag = OTA_track_sensor_da213_type_check();
    if(sensor_flag)
    {
        count = 0;
        return SENSOR_DA213;
    }
    
#if !defined (__REMOVE_OBSOLETE_GSENSOR__)
    sensor_flag = track_sensor_mc3410_type_check();
    if(sensor_flag)
    {
        count = 0;
        return SENSOR_MC3410;
    }

    sensor_flag = track_sensor_afa750_type_check();
    if(sensor_flag)
    {
        count = 0;
        return SENSOR_AFA750;
    }

    sensor_flag = track_sensor_mma8452q_type_check();
    if(sensor_flag)
    {
        count = 0;
        return SENSOR_MMA8452Q;
    }
#endif

    count++;
    if(count < 2)
    {
        track_drv_sensor_type_init();
    }
    else
    {
        count = 0;
#if defined __MECH_SENSOR__
        return SENSOR_MECH;
#else
        return SENSOR_INIT_ERROR;
#endif
    }
}


/******************************************************************************
 *  Function    -  track_drv_sensor_callback_handle
 *
 *  Purpose     -  �𶯴�����Ϣ���ص�ע�ắ��
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 27-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_sensor_callback_handle(void*msg)
{
    static kal_uint32 count = 0;
    if(track_sensor_config_data.sensor_callback != NULL)
    {
        LOGD(L_DRV, L_V8, "%d", count++);
        track_sensor_config_data.sensor_callback();
    }
}

/******************************************************************************
 *  Function    -  track_drv_sensor_interrupt_send_msg
 *
 *  Purpose     -  ����sensor ������Ϣ
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_sensor_interrupt_send_msg(void)
{
    ilm_struct *ilm_ptr = NULL;

    ilm_ptr = (ilm_struct *)allocate_ilm(MOD_DRV_HISR);
    /*ע��:�����жϼ�ʹ��MOD_MMIע�ᣬ
    ����Ӧ��������MOD_DRV_HISR --  chengjun  2013-05-16*/
    ilm_ptr->src_mod_id = MOD_DRV_HISR;
    ilm_ptr->dest_mod_id = MOD_MMI;
    ilm_ptr->sap_id = INVALID_SAP;
    ilm_ptr->msg_id = MSG_ID_MOTION_SENSOR_RSP;
    ilm_ptr->local_para_ptr = (local_para_struct*)NULL;
    ilm_ptr->peer_buff_ptr = (peer_buff_struct*) NULL;
    msg_send_ext_queue(ilm_ptr);
}


void track_drv_driving_record_handle(void*msg)
{
    track_driving_record_msg_struct *localBuff_p = (track_driving_record_msg_struct*) msg;
    kal_uint8 type;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    type = localBuff_p->type;   
	LOGD(L_DRV, L_V1, "%d",type);

    if(type == 1)
    {
		LOGD(L_DRV, L_V1, "ײ����");
    }
	else
	{

	}
}

/******************************************************************************
 *  Function    -  track_drv_driving_record_send_msg
 * 
 *  Purpose     -  �г���¼�쳣����֪ͨ��Ϣ
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-05-25,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_driving_record_send_msg(kal_uint8 type)
{
    ilm_struct *ilm_ptr = NULL;
    track_driving_record_msg_struct *myMsgPtr = NULL;

    myMsgPtr = (track_driving_record_msg_struct*) construct_local_para(sizeof(track_driving_record_msg_struct), TD_CTRL);
    myMsgPtr->type = type;

    ilm_ptr = (ilm_struct *)allocate_ilm(MOD_DRV_HISR);
    ilm_ptr->src_mod_id = MOD_DRV_HISR;
    ilm_ptr->dest_mod_id = MOD_MMI;
    ilm_ptr->sap_id = INVALID_SAP;
    ilm_ptr->msg_id = (msg_type) MSG_ID_DRIVING_RECORD_IND;
    ilm_ptr->local_para_ptr = (local_para_struct*) myMsgPtr;
    ilm_ptr->peer_buff_ptr = NULL;
    msg_send_ext_queue(ilm_ptr);
}

/******************************************************************************
 *  Function    -  track_drv_get_sensor_type
 *
 *  Purpose     -  ��ѯSensor ����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 25-02-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
sensor_type_enum track_drv_get_sensor_type(void)
{
    return track_sensor_config_data.sensor_type;
}

/******************************************************************************
 *  Function    -  track_drv_get_sensor_level
 *
 *  Purpose     -  ��ȡsensor���ȼ�
 *
 *  Description -  �����track_drv_sensor_threshold_start ֱ���������򷵻�0xFF
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 27-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint8 track_drv_get_sensor_level(void)
{
    return track_sensor_config_data.sensor_level;
}

/******************************************************************************
 *  Function    -  track_drv_get_sensor_threshold
 *
 *  Purpose     -  ��ȡsensor ��ǰ������
 *
 *  Description -  �������������ֵ�����ǵȼ�
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 02-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint8 track_drv_get_sensor_threshold(void)
{
    return track_sensor_config_data.sensor_threshold;
}

/******************************************************************************
 *  Function    -  track_drv_sensor_threshold_start
 *
 *  Purpose     -  ֱ�ӿ���������ֵ����sensor
 *
 *  Description -  �����ڲ��ԣ���ʽ���ʹ��track_drv_sensor_on
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 02-03-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_sensor_threshold_start(kal_uint8 threshold , FuncPtr callback)
{
    sensor_type_enum sensor_type;

    if((threshold <= 0) || (callback == NULL))
    {
        return;
    }

    sensor_type = track_drv_get_sensor_type();
    if((sensor_type <= SENSOR_NOT_READY) || (sensor_type >= SENSOR_TYPE_MAX))
    {
        LOGD(L_DRV, L_V1, "sensor type error (%d)", sensor_type);
        return;
    }

    track_sensor_config_data.sensor_level = 0xFF;
    track_sensor_config_data.sensor_threshold = threshold;
    track_sensor_config_data.sensor_callback = callback;

    if(track_sensor_config_data.sensor_on != NULL)
    {
        track_sensor_config_data.sensor_on(threshold, callback);
    }
}

/******************************************************************************
 *  Function    -  track_drv_sensor_on
 *
 *  Purpose     -  ����sensor ���
 *
 *  Description -  level  [in] :  sensor ��������� �ȼ�
 *                                         ���������ݲ������ӳ��������ֵ
 *                       callback  [in] : sensor �����ص�����
 *
 *  return
 *  -1=��������
 *    0=δʶ��Sensor����
 *    1=ִ�гɹ�
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 01-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_int8 track_drv_sensor_on(kal_uint8 level , FuncPtr callback)
{
    kal_uint8 threshold;
    sensor_type_enum sensor_type;

#if defined (__JM81__)||defined (__JM66__)
	/*��ª����ݵĸķ� -- chengjun  2017-08-16*/
	return;
#endif

    if((level <= 0) || (callback == NULL))
    {
        return -1;
    }

    track_sensor_config_data.sensor_level = level;
    track_sensor_config_data.sensor_callback = callback;

    sensor_type = track_drv_get_sensor_type();
    if((sensor_type <= SENSOR_NOT_READY) || (sensor_type >= SENSOR_TYPE_MAX))
    {
        LOGD(L_DRV, L_V1, "sensor type error (%d)", sensor_type);
        return 0;
    }
    if(sensor_type == SENSOR_MECH)
    {
#if defined __MECH_SENSOR__
        LOGD(L_DRV, L_V5, "sensor mech on !!%d",level);
        track_sensor_config_data.sensor_on(level,callback);
#endif
    }
    else
    {
        threshold = track_sensor_threshold_convert((sensor_threshold_level)level);
        if(threshold == 0)
        {
            LOGD(L_DRV, L_V1, "sensor level fail");
            return -1;
        }
        LOGD(L_DRV, L_V5, "Level=%d -> Threshold=%d", level, threshold);
        track_sensor_config_data.sensor_threshold = threshold;

        if(track_sensor_config_data.sensor_on != NULL)
        {
            track_sensor_config_data.sensor_on(threshold, callback);
        }
    }
    return 1;
}

/******************************************************************************
 *  Function    -  track_drv_sensor_off
 *
 *  Purpose     -  �ر�sensor ���
 *
 *  Description -
  *
 *  return
 *    0=δʶ��Sensor����
 *    1=ִ�гɹ�
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 27-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_int8 track_drv_sensor_off(void)
{
    sensor_type_enum sensor_type;

    sensor_type = track_drv_get_sensor_type();
    if((sensor_type <= SENSOR_NOT_READY) || (sensor_type >= SENSOR_TYPE_MAX))
    {
        LOGD(L_DRV, L_V1, "sensor type error (%d)", sensor_type);
        return 0;
    }
    LOGD(L_APP , L_V5, "track_drv_sensor_off  ok");
    if(track_sensor_config_data.sensor_off != NULL)
    {
        track_sensor_config_data.sensor_off();
    }

    return 1;
}

void track_drv_release_mech_mask()
{
    LOGD(L_DRV, L_V7, "UNMASK");
    EINT_UnMask(MOTION_SENSOR_EINT_NO);
}

/***************************************************
*  Purpose     -��е���ж���Ӧ����                  
*                                                 
*  Description -                                              
*                                                 
*  Modification-                                               
*                                                 
*                                                 
*  writen  xzq   11-18-2015                                             
***************************************************/
extern nvram_realtime_struct  G_realtime_data;

static void track_JogBall_UP_HISR(void)
{
    kal_uint8 level = track_sensor_config_data.sensor_level;
    kal_uint16 millsec = 0,max_count = 1;//millsec=G_realtime_data.mech_sensor_invel_time,max_count=G_realtime_data.mech_sensor_count;//һ��һ��
    const kal_uint16 millsec_tab[5] =
    {
        600,
        700,
        850,
        1000,
        1200
    };
    static kal_uint32 count = 0,cnt=0,kick=0;
    kal_uint32 curr_tick=0,intel_tick=0,invel_time=0;
    millsec = millsec_tab[level-1];
    //EINT_Mask(MOTION_SENSOR_EINT_NO);   
    touch_panel_state = !touch_panel_state;
    curr_tick = kal_get_systicks();
    millsec = millsec / 10;
    LOGD(L_SENSOR, L_V8, "millsec %d, max_count %d",millsec,max_count);

    if(millsec != 0 && max_count!=0)
    {
        invel_time = KAL_TICKS_10_MSEC*millsec;
        if(kick==0)
        {
            kick = curr_tick;
        }
        else
        {
            intel_tick=curr_tick-kick;
        }
        if(cnt<max_count)
        {
            cnt++;        
        }
        else
        {
            if(intel_tick<invel_time )
            {
                LOGD(L_SENSOR, L_V8, "���ʱ�䲻�� %d ����",millsec*10); 
                return;        
            }
            else
            {
                kick = curr_tick;
                cnt=1;//�������ʱ���ٴ���Ӧ
            }
        }
        
        
        LOGD(L_SENSOR, L_V5, "cnt %d, max_count %d, kick %d, currtick %d", cnt,max_count,kick,curr_tick);
        LOGD(L_SENSOR, L_V5, "invel_time %d, interval %d", invel_time,intel_tick);
    }
    track_drv_sensor_interrupt_send_msg();    
    EINT_Set_Polarity(MOTION_SENSOR_EINT_NO,touch_panel_state);  
    //tr_start_timer(TRACK_DRV_MECH_SENSOR_RELEASE_TIMER_ID, 1000, track_drv_release_mech_mask);
    //EINT_UnMask(MOTION_SENSOR_EINT_NO);
}

/***************************************************
*  Purpose     -��е��ע���ж�
*
*  Description -
*
*  Modification-
*
*
*  writen  xzq  11-18-2015
***************************************************/
static void track_drv_mech_sensor_montion_init()
{
    EINT_Mask(MOTION_SENSOR_EINT_NO);
    GPIO_ModeSetup(2, 1);
    EINT_Registration(MOTION_SENSOR_EINT_NO, KAL_FALSE, 0, track_JogBall_UP_HISR, KAL_TRUE);	//���ش�����KAL_FALSE,����KAL_TRUE,֮ǰ���ط�֧ȫ�������
    EINT_Set_Sensitivity(MOTION_SENSOR_EINT_NO, EDGE_SENSITIVE);/*******we should set the trigger by EDGE********/
    EINT_Set_Polarity(MOTION_SENSOR_EINT_NO, KAL_FALSE); //false -> 0, negative polarity
    EINT_UnMask(MOTION_SENSOR_EINT_NO);
}

/***************************************************
*  Purpose     -������е��                  
*                                                 
*  Description -                                             
*                                                 
*  Modification-                                               
*                                                 
*                                                 
*  writen  xzq  11-18-2015                                               
***************************************************/
void track_drv_mech_sensor_on(kal_uint8 level,FuncPtr callback)
{

    if(callback == NULL)
    {
        return;
    }
    track_sensor_config_data.sensor_level = level;
    track_sensor_config_data.sensor_callback = callback;
    track_drv_mech_sensor_montion_init();

}
void track_drv_mech_sensor_off()
{
   tr_stop_timer(TRACK_DRV_MECH_SENSOR_RELEASE_TIMER_ID);
   EINT_Mask(MOTION_SENSOR_EINT_NO);   
}
/******************************************************************************
 *  Function    -  track_drv_sensor_function_init
 *
 *  Purpose     -  ����sensor���ͣ��Ժ���ָ�븳ֵ��ʼ��
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 28-02-2013,  chengjun  written
 * v1.1  , 15-05-2014,  MagicNing  ɾ��sensor����Ӧ������������һֱû��ʹ�ã�δ��Ҳ��һ��������
 * ----------------------------------------
 ******************************************************************************/
static void track_drv_sensor_function_init(sensor_type_enum sensor_type)
{
    switch(sensor_type)
    {
        case SENSOR_BMA250:
        case SENSOR_BMA250E:
        case SENSOR_BMA253:
            track_sensor_config_data.sensor_on = track_sensor_bma250_on;
            track_sensor_config_data.sensor_off = track_sensor_bma250_off;
            break;
            
        case SENSOR_KXTJ31057:
            track_sensor_config_data.sensor_on = OTA_track_sensor_kxtj31057_on;
            track_sensor_config_data.sensor_off = OTA_track_sensor_kxtj31057_off;
            break;

        case SENSOR_SC7A20:
            track_sensor_config_data.sensor_on = OTA_track_sensor_sc7a20_on;
            track_sensor_config_data.sensor_off = OTA_track_sensor_sc7a20_off;
            break;

        case SENSOR_DA213:
            track_sensor_config_data.sensor_on = OTA_track_sensor_da213_on;
            track_sensor_config_data.sensor_off = OTA_track_sensor_da213_off;
            break;
            
        case SENSOR_MECH://��е��
            track_sensor_config_data.sensor_on = track_drv_mech_sensor_on;
            track_sensor_config_data.sensor_off = track_drv_mech_sensor_off;
            break;
            
#if !defined (__REMOVE_OBSOLETE_GSENSOR__)
        case SENSOR_MC3410:
            track_sensor_config_data.sensor_on = track_sensor_mc3410_on;
            track_sensor_config_data.sensor_off = track_sensor_mc3410_off;
            break;

        case SENSOR_AFA750:
            track_sensor_config_data.sensor_on = track_sensor_afa750_on;
            track_sensor_config_data.sensor_off = track_sensor_afa750_off;
            break;

        case SENSOR_MMA8452Q:
            track_sensor_config_data.sensor_on = track_sensor_mma8452q_on;
            track_sensor_config_data.sensor_off = track_sensor_mma8452q_off;
            break;
#endif /* __REMOVE_OBSOLETE_GSENSOR__ */

        default:
            LOGD(L_DRV, L_V1, "Error");
            break;
    }
}

/******************************************************************************
 *  Function    -  track_drv_sensor_type_auto_matching
 *
 *  Purpose     -  sensor�Զ�ʶ��
 *
 *  Description -
 *
 * modification history
 *
 * [Ver0.0]  �������ͺ�洢��nvram���Ժ�ֱ��ʹ��
 *
 * [Ver1.0]  ÿ�ο����������Զ�ʶ��
 *
 * ----------------------------------------
 * v1.0  , 26-02-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_sensor_type_auto_matching(void)
{
    sensor_type_enum sensor_type;
    char *sensor_type_tab[SENSOR_TYPE_MAX-1] = {"Bosch bma250", "Bosch bma250E", "Bosch bma253", "Kionix kxtj3-1057", "Silan SC7A20","Mira DA213",/*"mCube mc3410", "Micro afa750", "Freescale mma8452Q", */"MECH"};
#if defined (__GT740__)||defined(__GT420D__)
    return;
#endif

// [Ver0.0]
#if 0
    Track_nvram_read(NVRAM_EF_DRV_PARAMETER_LID, 1, (void *)&nvram_drv_parameter, NVRAM_EF_DRV_PARAMETER_SIZE);
    track_sensor_config_data.sensor_type = nvram_drv_parameter.sensor_type;

    if((track_sensor_config_data.sensor_type <= SENSOR_NOT_READY) || \
            (track_sensor_config_data.sensor_type >= SENSOR_TYPE_MAX))
    {

        track_sensor_config_data.sensor_type = track_drv_sensor_type_init();

        nvram_drv_parameter.sensor_type = track_sensor_config_data.sensor_type;
        Track_nvram_write(NVRAM_EF_DRV_PARAMETER_LID, 1, (void *)&nvram_drv_parameter, NVRAM_EF_DRV_PARAMETER_SIZE);

        if((track_sensor_config_data.sensor_type <= SENSOR_NOT_READY) || \
                (track_sensor_config_data.sensor_type >= SENSOR_TYPE_MAX))
        {
            LOGD(L_DRV, L_V1, "sensor type error (%d) ", track_sensor_config_data.sensor_type);
        }
        else
        {
            LOGD(L_DRV, L_V1, "sensor auto matching :%s", sensor_type_tab[track_sensor_config_data.sensor_type-1]);
        }
    }
    else
    {
        LOGD(L_DRV, L_V5, "no need to initialize,sensor type:%s ", sensor_type_tab[track_sensor_config_data.sensor_type-1]);
    }
#else
// [Ver1.0]
    track_sensor_config_data.sensor_type = track_drv_sensor_type_init();

    if((track_sensor_config_data.sensor_type <= SENSOR_NOT_READY) || \
            (track_sensor_config_data.sensor_type >= SENSOR_TYPE_MAX))
    {
        LOGD(L_DRV, L_V1, "sensor type error (%d) ", track_sensor_config_data.sensor_type);
    }
    else
    {
        LOGD(L_DRV, L_V1, "sensor auto matching :%s", sensor_type_tab[track_sensor_config_data.sensor_type-1]);
    }
#endif

    track_drv_sensor_function_init(track_sensor_config_data.sensor_type);
}

#endif /* __TRACK_SENSOR__ */

