#if defined(__AMS_LOG__)
/*******************************************************************************************
 * Filename:    track_cust_netlog.c
 * Author :     
 * Date :       
 * Comment:     
 ******************************************************************************************/

/****************************************************************************
*  Include Files                        ����ͷ�ļ�
*****************************************************************************/
#include "track_cust.h"
#include "track_drv_uart.h"
/*****************************************************************************
*  Define                               �궨��
******************************************************************************/
#define CACHE_LOG_MAX_LEN (3500) // ���ο����ϴ�������־��󳤶�
#define SUCESS_LOG_MAX_LEN (100) // �ϴ�������־��󳤶�
#define NETLOG_LENG (1000) // ÿ�δ��ϴ�������־��󳤶�
#define CACHE_UP_AMS_MAX_LEN (1024*50)
/*****************************************************************************
*  Typedef  Enum                        ö�ٶ���
*****************************************************************************/


/*****************************************************************************
*  Struct                               ���ݽṹ����
******************************************************************************/

/*****************************************************************************
*  Local variable                       �ֲ�����
*****************************************************************************/

static kal_uint8 st_file_name[20] = {"ST.txt"};
static char cache_log[CACHE_LOG_MAX_LEN]={0};
static char sucess_cache_log[SUCESS_LOG_MAX_LEN]={0};
static char cache_up_ams[CACHE_UP_AMS_MAX_LEN]={0};//�ϴ�AMSʱʹ��

static 	int l =0, b = 0;

/****************************************************************************
*  Global Variable                      ȫ�ֱ���
*****************************************************************************/


/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/


/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/

/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/
void track_cust_save_logfile(kal_uint8 *data, kal_uint16 len);
void track_cust_cache_netlink(char *buf, U16  len);



/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/

void track_cust_cache_netlink(char *buf, U16  len)
{
	if(G_realtime_data.ams_log)
	{
	    LOGD(L_OBD, L_V7, "strlen(cache_log):%d buflen %d", strlen(cache_log),len);
		if(strlen(cache_log)+len<CACHE_LOG_MAX_LEN-10)
		{
			  sprintf(cache_log + strlen(cache_log), buf);
		}
		else
		{
			return;
		}
		if(strlen(cache_log)<CACHE_LOG_MAX_LEN-2)
		{
			sprintf(cache_log + strlen(cache_log),",");
		}
	    LOGD(L_OBD, L_V8, "%s", &cache_log[0]);
	}

}

//����������LOG
void track_cust_cache_sucssful_log(char *buf, U16  len)
{
	if(G_realtime_data.ams_log==1 && G_parameter.extchip.mode!=1)
	{
	    LOGD(L_OBD, L_V5, "strlen(cache_log):%d buflen %d", strlen(sucess_cache_log),len);
		if(strlen(sucess_cache_log)+len<SUCESS_LOG_MAX_LEN-10)
		{
			  sprintf(sucess_cache_log + strlen(sucess_cache_log), buf);
		}
		else
		{
			return;
		}
		if(strlen(sucess_cache_log)<SUCESS_LOG_MAX_LEN-2)
		{
			sprintf(sucess_cache_log + strlen(sucess_cache_log),",");
		}
	    LOGD(L_OBD, L_V7, "%s", &sucess_cache_log[0]);
	}}


kal_int8  track_file_stlog_upload()
{
    kal_uint32 filelen=0, readsize=0;
    WCHAR tmpfile[40] = {0};
    int fs_result=0, i = 1,uret=0;
    FS_HANDLE ret = 0;
    int d=0;
    kal_wsprintf(tmpfile, st_file_name);
    ret = FS_Open(tmpfile, FS_READ_WRITE);
    if(ret < 0)
    {
        FS_Close(ret);
        LOGD(L_OBD, L_V5, "Can not create this file! folder_hd:%d", ret);
        return ret;
    }
    FS_GetFileSize(ret, &filelen);
	memset(cache_up_ams, 0, CACHE_UP_AMS_MAX_LEN);
	fs_result = FS_Read(ret, cache_up_ams, filelen, &readsize);	
	l=filelen;
	LOGD(L_OBD, L_V5, "==[DVT]  read: %d=%d=%d,%d, b %d", filelen, readsize,fs_result,l,b);
    {
        if(l > NETLOG_LENG)
        {
		    //cust_packet_log_data(8, &sendData[b], NETLOG_LENG);
			uret=track_cust_paket_09(&cache_up_ams[b], NETLOG_LENG);
			LOGH(L_OBD, L_V6, (void*)&cache_up_ams[b], NETLOG_LENG);

			//track_cust_paket_8D(g_alarm_type, rec_data.recSize, 0, (U8*)&crc, b, 1000, p + b);
            l -= NETLOG_LENG;
            b += NETLOG_LENG;
        }
        else
        {
		    //cust_packet_log_data(8, &sendData[b], l);
		    uret=track_cust_paket_09(&cache_up_ams[b], l);
			LOGH(L_OBD, L_V7, (void*)&cache_up_ams[b], l);
            b += l;
            l = 0;
        }
		LOGD(L_OBD, L_V7, "l%d, b %d", l, b);
    }
    FS_Close(ret);
    LOGD(L_OBD, L_V8, "%s",(char*)cache_up_ams);
    //G_realtime_data.ams_push=0;
	Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
	d = track_cust_netlog_del();
	LOGD(L_OBD, L_V1, "==[DVT] %d,d %d",b,d);
    return 0;
}

void track_resend_amslog()
{
	if(l>0)
	{
        if(l > NETLOG_LENG)
        {
		    //cust_packet_log_data(8, &sendData[b], NETLOG_LENG);
			track_cust_paket_09(&cache_up_ams[b], NETLOG_LENG);
			LOGH(L_OBD, L_V6, (void*)&cache_up_ams[b], NETLOG_LENG);

			//track_cust_paket_8D(g_alarm_type, rec_data.recSize, 0, (U8*)&crc, b, 1000, p + b);
            l -= NETLOG_LENG;
            b += NETLOG_LENG;
        }
        else
        {
		    //cust_packet_log_data(8, &sendData[b], l);
		    track_cust_paket_09(&cache_up_ams[b], l);
			LOGH(L_OBD, L_V6, (void*)&cache_up_ams[b], l);
            b += l;
            l = 0;
        }
		LOGD(L_OBD, L_V6, "l%d, b %d", l, b);
    }
}

/************************************************
   �������е�������־
parameter: 0  δ������־   1:������־
************************************************/
void track_cust_netlog_cache(U8 num)
{
	if(0==num)
	{
	    if(strlen(cache_log))
		{
		    track_cust_save_logfile(cache_log,strlen(cache_log));	
			memset(cache_log,0,sizeof(cache_log));
		}
		else
		{
			LOGD(L_OBD, L_V5, "����־!!");
		}
	}
	else
	{
	    if(strlen(sucess_cache_log))
		{
		    track_cust_save_logfile(sucess_cache_log,strlen(sucess_cache_log));
		    memset(sucess_cache_log,0,sizeof(sucess_cache_log));
	    }
		else
		{
		    LOGD(L_OBD, L_V5, "����־!!!");
		}
	}
    LOGD(L_OBD, L_V5, "�����쳣��־,�´ο����ϴ�AMS!!!");
}

int track_cust_netlog_del()
{
	    FS_HANDLE ret = NULL;
		int ret2;
		WCHAR tmpfile[40] = {0}, tmpfile2[40] = {0};
	    kal_wsprintf(tmpfile, st_file_name);
	    ret2=  FS_Delete(tmpfile);
	    LOGD(L_APP, L_V5, "FS_Delete=%d", ret2);
		return ret2;
}

// ��־�����ļ��ӿ�
void track_cust_save_logfile(kal_uint8 *data, kal_uint16 len)
{
 	if(G_realtime_data.ams_log==1)
 	{
		static U8 first=1;
	    kal_int8 i = 1, j = 1;
		U32 filelen=CACHE_UP_AMS_MAX_LEN,readsize=0;
	    FS_HANDLE ret = NULL;
	    kal_uint16 ret2 = 0;
	    WCHAR tmpfile[40] = {0}, tmpfile2[40] = {0};
	    kal_uint8 tmpname[20] = {0};
	    int close_ret = 0,   fs_result = 0;
	    int wSize = 0;
	    kal_wsprintf(tmpfile, st_file_name);
	    ret = FS_Open(tmpfile, FS_READ_WRITE | FS_CREATE);
	    if(ret < 0)
	    {
	        FS_Close(ret);
	        ret = 0;
	        LOGD(L_APP, L_V5, "Can not create this file! ret:%d", ret);
	        return;
	    }
		FS_GetFileSize(ret, &readsize);
		//FS_Read(ret, g_rec_data2, filelen, &readsize);
	    LOGD(L_OBD, L_V3, "==file %d %d", filelen,readsize);
	  	if(readsize>=filelen)
		{
	        LOGD(L_OBD, L_V5, "���ݴ���50K,ɾ��!");
	        FS_Close(ret);
	        ret2 = FS_Delete(tmpfile);
	        LOGD(L_APP, L_V5, "FS_Delete=%d", ret2);
			return;
		}
	    fs_result = FS_Seek(ret, 0, FS_FILE_END);
	    if(fs_result < FS_NO_ERROR)
	    {
	       FS_Close(ret);
	        LOGD(L_APP, L_V5, "FS_Seek %d", fs_result);
	       return;
	    }
	    ret2 = FS_Write(ret, (void *)data, len , &wSize);
	    if(ret2 < FS_NO_ERROR)
	   {
	        FS_Close(ret2);
	        LOGD(L_APP, L_V5, "ret2:%d", ret2);
	        ret2 = FS_Delete(tmpfile);
	        LOGD(L_APP, L_V5, "FS_Delete=%d", ret2);
	   }
	    FS_Commit(ret2);
	    close_ret = FS_Close(ret);
	    LOGD(L_OBD, L_V6, "==[DVT] %d %d %d %d %d==delefault=%d", ret, close_ret, ret2, wSize);
 	}
}

#endif
