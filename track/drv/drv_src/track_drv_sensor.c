/******************************************************************************
 * track_drv_sensor.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        Sensor 驱动
 *
 * modification history
 * --------------------
 * v1.0   2012-12-01,  chengjun create this file
 *
 ******************************************************************************/
#if defined (__TRACK_SENSOR__)


/****************************************************************************
* Include Files                   		包含头文件
*****************************************************************************/
#include "stack_config.h"
#include "stack_ltlcom.h"
#include "rtc_sw_new.h"
#include "track_cust_main.h"
#include "track_os_log.h"
#include "track_drv_sensor.h"
#include "track_os_ell.h"
/****************************************************************************
* Global Variable - Extern          引用全局变量
*****************************************************************************/


/*****************************************************************************
 *  Define					宏定义
 *****************************************************************************/

/*****************************************************************************
* Typedef  Enum
*****************************************************************************/


/*****************************************************************************
 *  Struct					数据结构定义
 *****************************************************************************/


/*****************************************************************************
* Local variable				局部变量
*****************************************************************************/
//nvram_drv_parameter_struct nvram_drv_parameter={0};
static kal_bool    touch_panel_state=KAL_FALSE;

/*****************************************************************************
* Golbal variable				全局变量
*****************************************************************************/
track_sensor_config_data_struct  track_sensor_config_data = {0};


/*****************************************************************************
 *  Global Functions	- Extern		引用外部函数
 *****************************************************************************/

/*****************************************************************************
 *  Local Functions			本地函数
 *****************************************************************************/

/*****************************************************************************
*  Global Functions                     全局函数
******************************************************************************/


/******************************************************************************
 *  Function    -  track_sensor_threshold_convert
 *
 *  Purpose     -  Sensor等级与灵敏度关系
 *
 *  Description -  默认SENSOR_LEVEL_2
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
 *  Purpose     -  获取sensoe三轴数据
 *
 *  Description -  1、瞬时值
 *                       2、注意有负数
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
 *  Purpose     -  逐一通信验证，查询Sensor类型
 *
 *  Description -
 *
 * modification history
 *
 *  为防止误检，查询2次失败才确认错误
 *
 * ----------------------------------------
 * v1.0  , 25-02-2013,  chengjun  written
 * v1.1  , 15-05-2014,  MagicNing  删除sensor自适应的驱动函数，一直没有使用，未来也不一定会启用
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
 *  Purpose     -  震动触发消息，回调注册函数
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
 *  Purpose     -  发送sensor 触发消息
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
    /*注意:所有中断即使在MOD_MMI注册，
    其响应函数是在MOD_DRV_HISR --  chengjun  2013-05-16*/
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
		LOGD(L_DRV, L_V1, "撞车了");
    }
	else
	{

	}
}

/******************************************************************************
 *  Function    -  track_drv_driving_record_send_msg
 * 
 *  Purpose     -  行车记录异常类型通知消息
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
 *  Purpose     -  查询Sensor 类型
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
 *  Purpose     -  读取sensor检测等级
 *
 *  Description -  如果是track_drv_sensor_threshold_start 直接启动，则返回0xFF
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
 *  Purpose     -  读取sensor 当前灵敏度
 *
 *  Description -  具体的灵敏度数值，不是等级
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
 *  Purpose     -  直接控制灵敏度值开启sensor
 *
 *  Description -  仅用于测试，正式软件使用track_drv_sensor_on
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
 *  Purpose     -  开启sensor 检测
 *
 *  Description -  level  [in] :  sensor 检测灵敏度 等级
 *                                         由驱动根据测试情况映射灵敏度值
 *                       callback  [in] : sensor 触发回调函数
 *
 *  return
 *  -1=参数错误
 *    0=未识别到Sensor类型
 *    1=执行成功
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
	/*丑陋而快捷的改法 -- chengjun  2017-08-16*/
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
 *  Purpose     -  关闭sensor 检测
 *
 *  Description -
  *
 *  return
 *    0=未识别到Sensor类型
 *    1=执行成功
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
*  Purpose     -机械震动中断响应函数                  
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
    kal_uint16 millsec = 0,max_count = 1;//millsec=G_realtime_data.mech_sensor_invel_time,max_count=G_realtime_data.mech_sensor_count;//一秒一次
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
                LOGD(L_SENSOR, L_V8, "间隔时间不够 %d 毫秒",millsec*10); 
                return;        
            }
            else
            {
                kick = curr_tick;
                cnt=1;//超过间隔时间再次响应
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
*  Purpose     -机械震动注册中断
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
    EINT_Registration(MOTION_SENSOR_EINT_NO, KAL_FALSE, 0, track_JogBall_UP_HISR, KAL_TRUE);	//边沿触发是KAL_FALSE,不是KAL_TRUE,之前本地分支全是提错了
    EINT_Set_Sensitivity(MOTION_SENSOR_EINT_NO, EDGE_SENSITIVE);/*******we should set the trigger by EDGE********/
    EINT_Set_Polarity(MOTION_SENSOR_EINT_NO, KAL_FALSE); //false -> 0, negative polarity
    EINT_UnMask(MOTION_SENSOR_EINT_NO);
}

/***************************************************
*  Purpose     -开启机械震动                  
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
 *  Purpose     -  根据sensor类型，对函数指针赋值初始化
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 28-02-2013,  chengjun  written
 * v1.1  , 15-05-2014,  MagicNing  删除sensor自适应的驱动函数，一直没有使用，未来也不一定会启用
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
            
        case SENSOR_MECH://机械震动
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
 *  Purpose     -  sensor自动识别
 *
 *  Description -
 *
 * modification history
 *
 * [Ver0.0]  读出类型后存储在nvram，以后直接使用
 *
 * [Ver1.0]  每次开机都进行自动识别
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

