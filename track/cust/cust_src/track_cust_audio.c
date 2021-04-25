/******************************************************************************
 * track_cust_key.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        GT300按键处理
 *
 * modification history
 * --------------------
 * v1.0   2013-07-5,  wangqi create this file
 *
 ******************************************************************************/

#if defined(__AUDIO_RC__)


/****************************************************************************
* Include Files                   		包含头文件
*****************************************************************************/
#include "track_cust.h"
#include "track_cust_audio.h"
#include "track_drv_system_interface.h"
#include "uem_proc_cmd.h"
#include "med_api.h"
/*****************************************************************************
 *  Define					宏定义
 *****************************************************************************/
#define AUDIO_SETTING_TOTAL (sizeof(tr_audio_setting)/sizeof(TRACK_AUDIO_SETTING))
#define K 1024
/*****************************************************************************
* Typedef  Enum
*****************************************************************************/


/*****************************************************************************
 *  Struct					数据结构定义
 *****************************************************************************/


/*****************************************************************************
* Local variable				局部变量
*****************************************************************************/
//static  kal_uint16 *p_rec_buf=NULL;
static  kal_uint32 rec_buf_size=0;
//kal_uint16 Record_Buffer[REC_BUFFER_LEN];//双字节
/*****************************************************************************
* Golbal variable				全局变量
*****************************************************************************/


/****************************************************************************
* Global Variable - Extern          引用全局变量
*****************************************************************************/

/*****************************************************************************
 *  Global Functions	- Extern		引用外部函数
 *****************************************************************************/
/*****************************************************************************
 *  Local Functions			本地函数
 *****************************************************************************/

/*****************************************************************************
 *  Global Functions			全局函数
 *****************************************************************************/

__align(2) static const unsigned char female_mandarin_3_wav[] =
{
0x0,
};

//音频文件移到ell_main 中

#if defined( __GT02__)||defined(__GT300S__) || defined(__GT800__)||defined(__NT31__) || defined(__NT22__) 
__align(4) static const unsigned char audio_ring_d_wav[] = {0};
#else
__align(4) static const unsigned char audio_ring_d_wav[] = {0};
#endif /* __GT02__ */
__align(2) static const unsigned char bubbles_wav[] =
{
0x0,
};



const TRACK_AUDIO_ITEM track_audio[] = 
{
    {audio_ring_d_wav, 16102, 13},
    {bubbles_wav, sizeof(bubbles_wav), 13}
};

const TRACK_AUDIO_HEADER custpack_audio =
{
    2,
    &track_audio[0]
};


static TRACK_AUDIO_SETTING tr_audio_setting[] = 
{
    {DEVICE_POWER_ON , 1        , 2},
    {DEVICE_POWER_OFF, 0        , 2},
    {MT_SMS                   , 0        , 2/*DEVICE_AUDIO_PLAY_ONCE*/},
    {MT_RING                  , 0        , 1},
    {MT_DISCONNECT      , 0XFF  , 1/*DEVICE_AUDIO_PLAY_INFINITE*/}
    
};//此结构体以后可存入NVRAM

static U8 g_alarm_type = 0;

/******************************************************************************
 *  Function    -  track_cust_get_audio_setting
 *
 *  Purpose     -  选择铃音
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0   09-10-2013,  Wangqi
 * ----------------------------------------
*******************************************************************************/
static int track_cust_get_audio_setting(kal_uint8 actionType, kal_uint8 witchPart)
{
    int index = 0 ,result = 0XFF;
    
    for(;index < AUDIO_SETTING_TOTAL; index++)
    {
        if(tr_audio_setting[index].ActionNames == actionType)
        {
            if(1 == witchPart)
            {
                result = tr_audio_setting[index].AudioIndex;
            }
            else if(2 == witchPart)
            {
                result =  tr_audio_setting[index].AudioRunType;
            }
        }
    }
    return result;
}
/******************************************************************************
 *  Function    -  track_cust_audio_stop_voice
 *
 *  Purpose     -  停止播放铃音
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0   09-10-2013,  Wangqi && Jiangweijun
 * ----------------------------------------
*******************************************************************************/
void track_cust_audio_stop_voice(void)
{
    LOGD(L_APP, L_V5, "");
    track_drv_sys_aud_send_stop_string_req(MOD_MMI);
}


/*1=进免提模式，但是关闭喇叭，就是无声音处于监听效果       --    chengjun  2017-07-12*/
void track_cust_monitor_set_speaket(kal_uint8 mute)
{
    LOGD(L_APP, L_V5, "mute:%d", mute);
#if defined( __GT03F__) || defined(__GT300__)  || defined(__GW100__)||defined(__GT06F__)||defined(__GT300S__)||defined(__NT36__)||defined(__NT37__)
//为了不影响其它项目，如果其它项目也要禁用Speaker可去掉这个宏
    if(mute == 1)
    {
        track_drv_sys_l4cuem_set_audio_mode(2);//可用切换到普通模式 
        L1SP_MuteSpeaker(KAL_TRUE);
    }
    else if(mute == 0)
    {
        L1SP_MuteSpeaker(KAL_FALSE);
    }
#endif /* __GT03F__ */
}
/******************************************************************************
 *  Function    -  track_cust_audio_play_voice
 *
 *  Purpose     -  处理铃音和震动主入口
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0   09-10-2013,  Wangqi && Jiangweijun
 * ----------------------------------------
*******************************************************************************/
void track_cust_audio_play_voice(action_for_audio_enum actionType)
{
    aud_play_string_struct play_string = {0};
    kal_uint8 index = track_cust_get_audio_setting(actionType, 1);
    kal_uint8 playType = track_cust_get_audio_setting(actionType, 2);
    kal_uint8 call_mode = track_status_audio_mode(0xFF);
    
    LOGD(L_APP, L_V5, "actionType:%d, audioIndex:%d,plyaType:%d,callmode:%d,%d", actionType, index, playType,G_parameter.callmode,call_mode);

    if(actionType > MT_MAX )
    {
        LOGS( "Exception Action type or  AUDIO_PLAY beyond :%d,%d", actionType, playType);
        return;
    }
    
    if(actionType == MT_DISCONNECT || actionType == MT_CONNECT || actionType == MT_MONITOR)
    {
        track_cust_audio_stop_voice();
        track_cust_ring_Vibration((void*)0);
        
        if(actionType == MT_CONNECT)
        {
            track_cust_Vibration_100ms();
        }
        
#if defined( __GT310__)||defined( __GT06F__)||defined(__GT300S__)||defined(__NT36__)||defined(__NT37__)
        if (actionType == MT_MONITOR)
        {
            track_cust_monitor_set_speaket(1);
        }        
        else if(call_mode != 0)
        {
            track_drv_sys_l4cuem_set_audio_mode(0);//可用切换到普通模式            
        }

        if(actionType == MT_DISCONNECT)
        {
            track_cust_monitor_set_speaket(0);
            //track_drv_sys_audio_set_volume(2/*VOL_TYPE_MIC*/,3/*GAIN_NOR_MIC_VOL3*/);
        }
#endif /* __GT300__ */
        return;
    }
    
    if(G_parameter.callmode >= 1 && actionType >= MT_SMS)
    {
        if(actionType == MT_RING)
        {
            track_cust_vib_interval(20, 1000, 2000);
            track_cust_ring_Vibration((void*)1);
        }
        else 
        {
            ControlVibration();
        }
        
        if(G_parameter.callmode == 1)
        {
            return;
        }
    }
    
    if(index >= custpack_audio.MaxAudioNumEXT || playType > 3/*DEVICE_AUDIO_PLAY_DESCENDO*/)
    {
        LOGS("==%d Voice Index Beyond %d!==",index, custpack_audio.MaxAudioNumEXT);
        return;
    }

    play_string.src_id = MOD_MMI;
    play_string.data_p = OTA_ell_play_voice();//(kal_uint8 *)custpack_audio.Audio_item[index].AudioNames;
    play_string.len = custpack_audio.Audio_item[index].AudioLength;
    play_string.format = custpack_audio.Audio_item[index].AudioType;/*MED_TYPE_WAV*/
    
    if(actionType <= MT_SMS)
    {
        play_string.play_style = 2;/*DEVICE_AUDIO_PLAY_ONCE*/
    }
    else
    {
        play_string.play_style = playType;
    }
    
    play_string.volume = 5;
    play_string.output_path = 6;/*AUDIO_DEVICE_SPEAKER_BOTH*/
    play_string.identifier = 1;
    /* set both start_offset and end_offset to 0 for backward compatible */
    play_string.start_offset = 0;    /* 0 : file begining */
    play_string.end_offset = play_string.len;      /* 0 or file data len-1: file end */
    play_string.blocking = TRUE;

    LOGD(L_CMD, L_V5, "len:%d,format:%d", play_string.len, play_string.format);

    track_drv_sys_aud_send_play_string_req((void *)&play_string);
}



/*录音存FAT验证补丁       --    chengjun  2016-12-24*/
//#define  DEBUG_RECORD_FILE_ON   1

static rec_data_struct rec_data = {0};
static int folder_hd;

#if defined (DEBUG_RECORD_FILE_ON)
kal_uint8 track_record_init_file(void)
{
   kal_wchar filename[40];
   kal_wsprintf(filename, "\\rec_test.amr");
   folder_hd = FS_Open(filename, FS_READ_WRITE | FS_CREATE);
   if(folder_hd < 0)
   {
        folder_hd = 0;
         LOGD(L_DRV, L_V5,"Can not create this file! folder_hd:%d", folder_hd);
   }
   LOGD(L_DRV, L_V5,"create file sucesse! folder_hd:%d", folder_hd);
   return (folder_hd == 0 ? 0 : 1);
}
#endif /* DEBUG_RECORD_FILE_ON */


void track_check_record(void)
{
    if(G_realtime_data.record_cnt > G_realtime_data.record_file + 1)
    {
        G_realtime_data.record_file = G_realtime_data.record_file + 1;
        track_cust_record(g_alarm_type, 1, RECORD_INTERVAL);
    }
    else if(G_realtime_data.record_last_time && G_realtime_data.record_last_time < RECORD_INTERVAL)
    {
        track_cust_record(g_alarm_type, 1, G_realtime_data.record_last_time);
        G_realtime_data.record_last_time = 0;
    }
    else
    {
        G_realtime_data.record_cnt = 0;
        G_realtime_data.record_file = 0;
        G_realtime_data.record_up = 0;
        G_realtime_data.record_last_time = 0;
    }
}

/******************************************************************************
 *  Function    -  track_record_callback
 *  Purpose     -  录音回调函数
 *  Description -
 *  
 *  modification history
 * ----------------------------------------
 * v1.0   20140927,  Wangqi
 * ----------------------------------------
*******************************************************************************/
void track_record_callback(Media_Event event)
{
    kal_uint16 *pwBuf;
    kal_uint32 buf_len, ret, wSize, curSize;
    LOGD(L_APP, L_V5,"event:%d", event);
    switch (event)
    {
    case MEDIA_TERMINATED:
        LOGD(L_APP, L_V5,"[DVT] record end");
        break;
    case MEDIA_DATA_NOTIFICATION:
        OTA_Media_GetReadBuffer(&pwBuf, &buf_len);
        OTA_Media_ReadDataDone(buf_len);

        curSize = rec_data.recSize + buf_len*2;
        if(curSize < REC_BUFFER_LEN)
        {
            rec_data.recSize = curSize;
        }
        
        LOGH(L_APP, L_V8, pwBuf, sizeof(int16) * buf_len);
        LOGD(L_APP, L_V5,"[DVT] record len = %d, %d, %d, %d, g_alarm_type:%d", 
        rec_data.packets, rec_data.revPacket, buf_len, rec_data.recSize, g_alarm_type);
        if(rec_data.revPacket++ > rec_data.packets)
        {
            int l = rec_data.recSize, b = 0;
	     U8 *p;
            U16 crc = GetCrc16(p, l);
#if 0
defined(__LUYING__)
            if(p_rec_buf!=NULL)
            {
                p  = (U8 *)p_rec_buf;
            }
#else
            p = (U8 *)rec_data.Record_Buffer;
#endif
            track_cust_record(g_alarm_type, 0, 0);
            
            LOGD(L_APP, L_V5,"rec_data:%x,apn:%x,%x",&rec_data.Record_Buffer[REC_BUFFER_LEN], track_socket_get_use_apn(), track_nvram_alone_parameter_read()->apn.apn);
            
            if(track_is_testmode())
            {
                break;
            }


            /*连续压栈没问题吗?       --    chengjun  2017-09-01*/
            while(l > 0)
            {
                if(l > 1000)
                {
                    track_cust_paket_8D(g_alarm_type, rec_data.recSize, 0, (U8*)&crc, b, 1000, p + b);
                    l -= 1000;
                    b += 1000;
                }
                else
                {
                    track_cust_paket_8D(g_alarm_type, rec_data.recSize, 0, (U8*)&crc, b, l, p + b);
                    l = 0;
                    b += l;
                }
            }

        track_os_intoTaskMsgQueue(track_check_record);
                            
#if defined (__GW100__)
            if(track_cust_module_alarm_lock(TR_CUST_ALARM_SOS, 2) && G_parameter.sos_alarm.alarm_type > 2)
            {
                track_cust_module_alarm_lock(99, 0);
            }
#endif /* __GW100__ */
            //应该层在此处使用Buffer数据rec_data.Record_Buffer,rec_data.recSize为长度char长度非U16长度
        }
        break;
    default:
        break;
    }
        
} 


/*1=正在录音，0=录音结束，其他是读取状态       --    chengjun  2017-09-27*/
kal_uint8 track_cust_is_recording(kal_uint8 option)
{
    static kal_uint8 going = 0;
    if(0 == option || 1 == option)
    {
        going = option;
    }
    LOGD(L_APP, L_V5,"== %d %d==", going, option);
    return going;
}

/******************************************************************************
 *  Function    -  track_cust_record
 *  Purpose     -  录音主入口
 *  Description -
 *  type:1开启 0关闭
 *  time:录音时长 单位秒 
 *  
 *  modification history
 * ----------------------------------------
 * v1.0   20140927,  Wangqi
 * ----------------------------------------
*******************************************************************************/
int track_cust_record(U8 alarm_type, kal_uint8 type, kal_uint16 time)
{
    int close_ret = 0;
    kal_uint32 ret, wSize;
    kal_uint8 isGoing = track_cust_is_recording(0xff);

    g_alarm_type = alarm_type;
    LOGD(L_APP, L_V5,"isGoing:%d, type:%d, g_alarm_type:%d", isGoing,type, g_alarm_type);

    if(isGoing && type == 1)
    {
        LOGD(L_APP, L_V5,"==[DVT] is recording ==");
        return 0;
    }
    
    if(1 == type)
    {
#if defined(DEBUG_RECORD_FILE_ON)
        if(track_record_init_file() == 0)
        {
            LOGD(L_APP, L_V5,"==[DVT] track_cust_record init 创建文件失败==");
            //return -1;
        }
#endif
        memset(&rec_data, 0, sizeof(rec_data_struct));
        track_cust_is_recording(1);
        
#if 0
defined(__LUYING__)
        memset(p_rec_buf, 0, rec_buf_size);
        p_rec_buf[0] = 0x2123;//低位在前生成文件为23 21 41 4d 52 0a
        p_rec_buf[1] = 0x4d41;
        p_rec_buf[2] = 0x0a52;
#else
        rec_data.Record_Buffer[0] = 0x2123;//低位在前生成文件为23 21 41 4d 52 0a
        rec_data.Record_Buffer[1] = 0x4d41;
        rec_data.Record_Buffer[2] = 0x0a52;
        rec_data.recSize = 6;
#endif

        if(time > 128) 
        {
            time = 128;
        }
        if(time == 0)
        {
            time = 10;
        }
        rec_data.time = time;
        rec_data.packets = time * 2 - time/3;
        rec_data.revPacket = 0;
        LOGD(L_APP, L_V5,"==[DVT] track_cust_record init == %d,%d,%d",rec_data.time, rec_data.packets, rec_data.revPacket);
#if 0
defined(__LUYING__)       
        OTA_Media_SetBuffer(p_rec_buf+3, rec_buf_size-3);
#else
        OTA_Media_SetBuffer((kal_uint16 *)&rec_data.Record_Buffer[3], REC_BUFFER_LEN - 3);
#endif
        OTA_Media_SetRBThreshold(200);
        OTA_Media_Record(3/*MEDIA_FORMAT_AMR*/, track_record_callback, NULL); 
        tr_start_timer(TRACK_CUST_RECODE_OFF_TIMER_ID, (time+2)*1000, track_cust_close_record);
    }
    else if(0 == type)
    {
        LOGD(L_APP, L_V5,"==[DVT] track_cust_record colse ==");
        if(track_is_timer_run(TRACK_CUST_RECODE_OFF_TIMER_ID))
        {
            track_stop_timer(TRACK_CUST_RECODE_OFF_TIMER_ID);
        }
        OTA_Media_Stop();//stop record audio
        //OTA_LOGH(L_DRV, L_V5, rec_data.Record_Buffer, rec_data.recSize);
        
#if defined(DEBUG_RECORD_FILE_ON)
        if(folder_hd)
        {
#if 0
defined(__LUYING__)       
            ret = FS_Write(folder_hd, (void *)p_rec_buf, rec_data.recSize , &wSize);			
#else
            ret = FS_Write(folder_hd, (void *)rec_data.Record_Buffer, rec_data.recSize , &wSize);
#endif
            FS_Commit(folder_hd);
            close_ret = FS_Close(folder_hd);
#if 0
defined(__LUYING__)
            Ram_Free(p_rec_buf);
            p_rec_buf = NULL;
#endif

            LOGD(L_DRV, L_V5,"==[DVT] %d colse ret:%d %d %d==", ret, close_ret,rec_data.recSize, wSize);
        }
#endif

        track_cust_is_recording(0);
        
    }
    return 1;
}


void track_cust_close_record(void)
{
    LOGD(L_DRV, L_V5,"录音结束");
    track_cust_record(0, 0, 0);
}

#if 0
defined(__LUYING__)
void track_cust_audio_init(U8 time)
{
	rec_buf_size = (time+2)*K;
	
	if(p_rec_buf==NULL)
	{
		p_rec_buf  = (U16*)Ram_Alloc(5, rec_buf_size);
		if(p_rec_buf == NULL)
		{
                    LOGD(L_APP, L_V5,"录音申请内存失败!!!!!!");
			return;
		}
            LOGD(L_APP, L_V5,"rec_buf_size %d",rec_buf_size);	
	}
}

#endif


#if defined(__SAVING_CODE_SPACE__)
#else
void track_cust_test_voice(int m)
{
    track_drv_aud_play_id_struct play_voice = {0};
    aud_play_string_struct play_string = {0};
    static kal_uint8 indentifier = 0;
    if(m == 160)
    {
        l4cuem_speech_set_mode_req(KAL_TRUE, 2);
    }
    else if(m == 161)
    {

        l4cuem_speech_set_mode_req(KAL_FALSE, 2);
    }
    else if(m == 162)
    {
        l4cuem_speech_set_mode_req(KAL_TRUE, 0);
    }
    else if(m == 163)
    {
        //处于免提状态执行此功能后无音，执行162后恢复
        l4cuem_speech_set_mode_req(KAL_FALSE, 0);
    }
    else if(m == 164)
    {
        //LOGD(L_CON, L_V5, "",mdi_audio_get_audio_mode());
        ;
    }
    else if(m == 165)
    {
        track_drv_sys_l4cuem_set_audio_mode(0);//可用切换到手持
    }
    else if(m == 166)
    {
        track_drv_sys_l4cuem_set_audio_mode(2);//可用切换到免提
    }
    else if(m == 176)
    {
        track_drv_sys_l4cuem_set_audio_mode(1);//可用切换到免提
    }
    else if(m == 167)
    {
        l4cuem_audio_get_volume_req(MOD_MMI, 0);
    }
    else if(m == 168)
    {
        l4cuem_audio_get_volume_req(MOD_MMI, 4);
    }
    else if(m == 169)
    {
        l4cuem_audio_get_volume_req(MOD_MMI, 2);
    }
    else if(m == 170)
    {
        track_drv_sys_audio_set_volume(4, 1);
    }
    else if(m == 171)
    {
        track_drv_sys_audio_set_volume(4, 6);
    }
    else if(m == 172)
    {
        LOGD(L_CMD, L_V5, "indentifier:%d", indentifier);
        play_voice.audio_id = indentifier;
        play_voice.play_style = 1;/*DEVICE_AUDIO_PLAY_INFINITE*/
        play_voice.volume = 5;
        play_voice.output_path = 6;/*AUDIO_DEVICE_SPEAKER_BOTH*/
        play_voice.identifier = indentifier;
        track_drv_sys_aud_send_play_id_req((void*)&play_voice);//按键声音
        indentifier++;

        //INCOMING_CALL_TONE
    }
    else if(m == 173)
    {
        tr_audio_setting[MT_RING].AudioIndex = 0;
        track_cust_audio_play_voice(3);

    }
    else if(m == 174)
    {
        tr_audio_setting[MT_RING].AudioIndex = 0;
        track_cust_audio_play_voice(3);
    }
    else if(m == 175)
    {
        play_string.src_id = MOD_MMI;
        play_string.data_p = (kal_uint8*)&female_mandarin_3_wav[0];
        play_string.len = sizeof(female_mandarin_3_wav);
        play_string.format = 13;
        play_string.play_style = 1;/*DEVICE_AUDIO_PLAY_INFINITE*/
        play_string.volume = 5;
        play_string.output_path = 6;/*AUDIO_DEVICE_SPEAKER_BOTH*/
        play_string.identifier = indentifier;
        /* set both start_offset and end_offset to 0 for backward compatible */
        play_string.start_offset = 0;    /* 0 : file begining */
        play_string.end_offset = play_string.len;      /* 0 or file data len-1: file end */
        play_string.blocking = TRUE;

        LOGD(L_CMD, L_V5, "indentifier:%d,len:%d", indentifier, play_string.len);

        track_drv_sys_aud_send_play_string_req((void *)&play_string);
    }

}
#endif
#endif /* __GT300__ */
