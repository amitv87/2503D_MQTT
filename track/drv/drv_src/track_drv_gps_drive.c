/******************************************************************************
 * track_gps_drive.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        GPS ����
 *
 * modification history
 * --------------------
 * v1.0   2012-07-27,  chengjun create this file
 *
 ******************************************************************************/
/****************************************************************************
* Include Files             ����ͷ�ļ�
*****************************************************************************/
#include "track_drv_gps_drive.h"
#include "stack_msgs.h"
#include "stack_ltlcom.h"
#include "app_datetime.h"
#include "Track_os_log.h"
#include "track_drv_uart.h"
#include "track_drv_system_interface.h"
#include "track_drv_gps_decode.h"
#include "track_os_ell.h"
#include "track_cust.h"
#include "../gps/inc/gps_common.h"
#include "gps_struct.h"
#include "../gps/inc/gps_uart_hdlr.h"
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
static kal_bool  assist_data_ready = KAL_TRUE;
//KAL_TRUE = ����EPO ����ע��MNL ������λ
static kal_bool  allow_data_injection = KAL_TRUE;   //����EPO ����ע��MNL ������λ
static U8     drv_gps_decode_status=1;
/****************************************************************************
* Global Variable           ȫ�ֱ���
*****************************************************************************/
typedef enum
{
	TIME_UPDATE_STATE_NONE,
	TIME_UPDATE_STATE_NTP,
	TIME_UPDATE_STATE_NITZ,
	TIME_UPDATE_STATE_GPS,
} track_time_update_state;

typedef struct
{
    LOCAL_PARA_HDR
	kal_uint32 curr_sec;
	track_time_update_state state;
} track_gps_time_update_struct;

/****************************************************************************
* Global Variable - Extern          ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
 *  Global Functions	- Extern	    �����ⲿ����
*****************************************************************************/

/*****************************************************************************
 *  Local Functions	            ���غ���
*****************************************************************************/
void track_gps_power_on_req(gps_common_uart_work_mode_enum work_mode);
void track_gps_power_off_req(gps_common_uart_work_mode_enum work_mode);

kal_uint8 track_drv_decode_status()
{
	return drv_gps_decode_status;
}

static unsigned char calc_nmea_checksum(const char* sentence)
{
    unsigned char checksum = 0;
    while(*sentence)
    {
        checksum ^= (unsigned char) * sentence++;
    }
    return  checksum;
}

/******************************************************************************
 *  Function    -  track_agps_epo_time
 *
 *  Purpose     -  ע��ο�ʱ��
 *
 *  Description -  UTC (GMT0)
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 22-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_agps_epo_time(applib_time_struct utcTime)
{
    mtk_param_epo_time_cfg epo_time_cfg;

    epo_time_cfg.u2YEAR = utcTime.nYear; //UTC time: year in 4 digits
    epo_time_cfg.u1MONTH = utcTime.nMonth; //UTC time: month
    epo_time_cfg.u1DAY = utcTime.nDay;//UTC time: day
    epo_time_cfg.u1HOUR = utcTime.nHour;//UTC time: hour
    epo_time_cfg.u1MIN = utcTime.nMin;//UTC time: minute
    epo_time_cfg.u1SEC = utcTime.nSec;//UTC time: second

    track_drv_sys_mtk_gps_set_param(MTK_PARAM_CMD_CONFIG_EPO_TIME, &epo_time_cfg);
}

/******************************************************************************
 *  Function    -  track_agps_epo_pos
 *
 *  Purpose     -  ע��ο�λ��
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 22-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_agps_epo_pos(track_gps_epo_struct * gps_data)
{
    mtk_param_epo_pos_cfg epo_pos_cfg;

    //WGS84 geodetic latitude(Degree). Minus: south; Plus: north
    if(gps_data->gprmc.NS == 'N')
    {
        epo_pos_cfg.dfLAT = gps_data->gprmc.Latitude;
    }
    else
    {
        epo_pos_cfg.dfLAT = -gps_data->gprmc.Latitude;
    }


    //WGS84 geodetic longitude(Degree). Minus: west; Plus: east
    if(gps_data->gprmc.EW == 'E')
    {
        epo_pos_cfg.dfLON = gps_data->gprmc.Longitude;
    }
    else
    {
        epo_pos_cfg.dfLON = -gps_data->gprmc.Longitude;
    }

    epo_pos_cfg.dfALT = gps_data->altitude; //WGS84 ellipsoidal altitude(m).
    epo_pos_cfg.u2YEAR = gps_data->gprmc.Date_Time.nYear; // Reference UTC time: year in 4 digits
    epo_pos_cfg.u1MONTH = gps_data->gprmc.Date_Time.nMonth; // Reference UTC time: month
    epo_pos_cfg.u1DAY = gps_data->gprmc.Date_Time.nDay;// Reference UTC time: day
    epo_pos_cfg.u1HOUR = gps_data->gprmc.Date_Time.nHour;// Reference UTC time: hour
    epo_pos_cfg.u1MIN = gps_data->gprmc.Date_Time.nMin; // Reference UTC time: minute
    epo_pos_cfg.u1SEC = gps_data->gprmc.Date_Time.nSec; // Reference UTC time: second

    track_drv_sys_mtk_gps_set_param(MTK_PARAM_CMD_CONFIG_EPO_POS, &epo_pos_cfg);
}



/******************************************************************************
 *  Function    -  track_utc_to_gps_hour
 *
 *  Purpose     -  translate UTC to GPS_Hour
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
int track_utc_to_gps_hour(int iYr, int iMo, int iDay, int iHr)
{
    int iYearsElapsed;     // Years since 1980
    int iDaysElapsed;      // Days elapsed since Jan 6, 1980
    int iLeapDays;         // Leap days since Jan 6, 1980
    int i;

    // Number of days into the year at the start of each month (ignoring leap years)
    const unsigned short doy[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

    iYearsElapsed = iYr - 1980;
    i = 0;
    iLeapDays = 0;

    while(i <= iYearsElapsed)
    {
        if((i % 100) == 20)
        {
            if((i % 400) == 20)
            {
                iLeapDays++;
            }
        }
        else if((i % 4) == 0)
        {
            iLeapDays++;
        }
        i++;
    }

    if((iYearsElapsed % 100) == 20)
    {
        if(((iYearsElapsed % 400) == 20) && (iMo <= 2))
        {
            iLeapDays--;
        }
    }
    else if(((iYearsElapsed % 4) == 0) && (iMo <= 2))
    {
        iLeapDays--;
    }

    iDaysElapsed = iYearsElapsed * 365 + (int)doy[iMo - 1] + iDay + iLeapDays - 6;

    // Convert time to GPS weeks and seconds
    iHr = iHr / 6 * 6;
    return (iDaysElapsed * 24 + iHr);

}

/******************************************************************************
 *  Function    -  track_epo_delete_file
 *
 *  Purpose     -  ɾ���Ѵ��EPO�ļ�
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int8 track_epo_delete_file(void)
{
    WCHAR FilePath[60] = {0};
    kal_wsprintf(FilePath, MTKEPO_FILE_PATH);
    return FS_Delete(FilePath);
}

kal_int32 track_file_check(kal_uint8 *reference_md5, char *file_path)
{
    kal_uint8 ell_file_md5[16] = {0};
    kal_bool ret;

    FS_HANDLE fp = NULL;
    WCHAR FilePath[60] = {0};
    kal_uint32 filelen, readsize;
    int fs_result, len;
    kal_uint8 *data, *buf;

    kal_wsprintf(FilePath, file_path);
    fp = FS_Open(FilePath, FS_READ_ONLY);
    if(fp < 0)
    {
        return fp;
    }
    FS_GetFileSize(fp, &filelen);
    LOGD(L_APP, L_V5, "filelen:%d", filelen);
    if(filelen >= 500000)
    {
        FS_Close(fp);
        return -4;
    }
    buf = (U8*)Ram_Alloc(5, filelen + 10);
    if(!buf)
    {
        return -5;
    }
    data = buf;
    len = filelen;
    fs_result = FS_Seek(fp, 0, FS_FILE_BEGIN);
    if(fs_result < FS_NO_ERROR)
    {
        FS_Close(fp);
        return fs_result;
    }
    while(len > 0)
    {
        if(len > 50000)
        {
            fs_result = FS_Read(fp, data, 50000, &readsize);
        }
        else
        {
            fs_result = FS_Read(fp, data, len, &readsize);
        }
        if(fs_result < FS_NO_ERROR)
        {
            FS_Close(fp);
            //EllLog("fs_result:%d, %d, %d", fs_result, readsize, len);
            return fs_result;
        }
        len -= readsize;
        data += readsize;
    }
    FS_Close(fp);
    ret = IsDataMd5ext(buf, filelen, reference_md5);
    if(buf != NULL)
    {
        Ram_Free(buf);
    }
    if(ret)
        return 1;
    return 0;
}

kal_bool track_file_epo_dele()
{
    FS_HANDLE fp = NULL;
    WCHAR FilePath[60] = {0};
    kal_uint32 filelen, readsize;
    int fs_result, len;
    kal_uint8 *data, *buf;

    kal_wsprintf(FilePath, MTKEPO_FILE2_PATH);
    fp = FS_Open(FilePath, FS_READ_ONLY);
    if(fp < 0)
    {
		FS_Close(fp);
        return KAL_FALSE;
    }
    FS_Close(fp);
    FS_Delete(FilePath);
	return KAL_TRUE;
}

kal_bool track_file_epo_exitence()
{
    FS_HANDLE fp = NULL;
    WCHAR FilePath[60] = {0};
    kal_uint32 filelen, readsize;
    int fs_result, len;
    kal_uint8 *data, *buf;

    kal_wsprintf(FilePath, MTKEPO_FILE2_PATH);
    fp = FS_Open(FilePath, FS_READ_ONLY);
    if(fp < 0)
    {
        kal_wsprintf(FilePath, MTKEPO_FILE3_PATH);
		fp =  FS_Open(FilePath, FS_READ_ONLY);
		if(fp < 0)
    	{
			return KAL_FALSE;
		}
    }
    FS_GetFileSize(fp, &filelen);
	FS_Close(fp);
	return KAL_TRUE;
}

kal_int32 track_md5_file_check(kal_uint8 *reference_md5, char *file_path)
{
    kal_uint8 ell_file_md5[16] = {0};
    kal_bool ret = 0;

    FS_HANDLE fp = NULL;
    WCHAR FilePath[60] = {0};
    kal_uint32 filelen, readsize;
    int fs_result, len;
    kal_uint8 *data, *buf;

    kal_wsprintf(FilePath, file_path);
    fp = FS_Open(FilePath, FS_READ_ONLY);
    if(fp < 0)
    {
        return fp;
    }
    FS_GetFileSize(fp, &filelen);
    LOGD(L_APP, L_V5, "filelen:%d", filelen);
    if(filelen >= 500000)
    {
        FS_Close(fp);
        return -4;
    }
    buf = (U8*)Ram_Alloc(5, filelen + 10);
    if(!buf)
    {
        return -5;
    }
    data = buf;
    len = filelen;
    fs_result = FS_Seek(fp, 0, FS_FILE_BEGIN);
    if(fs_result < FS_NO_ERROR)
    {
        FS_Close(fp);
        return fs_result;
    }
    while(len > 0)
    {
        if(len > 50000)
        {
            fs_result = FS_Read(fp, data, 50000, &readsize);
        }
        else
        {
            fs_result = FS_Read(fp, data, len, &readsize);
        }
        if(fs_result < FS_NO_ERROR)
        {
            FS_Close(fp);
            //EllLog("fs_result:%d, %d, %d", fs_result, readsize, len);
            return fs_result;
        }
        len -= readsize;
        data += readsize;
    }
    FS_Close(fp);
    if(buf != NULL)
    {
        if(!memcmp(buf, reference_md5, 16))
        {
            ret = 1;
        }
        Ram_Free(buf);
    }
    if(ret)
        return 1;
    return 0;
}
/******************************************************************************
 *  Function    -  track_epo_write_file
 *
 *  Purpose     -  д��EPO�ļ�
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int32 track_write_file(char *filepath, kal_uint8 *data, kal_uint32 _len)
{
    FS_HANDLE fp = NULL;
    WCHAR _FilePath[60] = {0};
    kal_uint32 writelen, len = _len;
    int fs_result;

    LOGD(L_DRV, L_V4, "\"%s\" ��Ҫд��%dB", filepath, _len);
    OTA_kal_wsprintf(_FilePath, "%s", filepath);
    OTA_FS_Delete(_FilePath);
    fp = OTA_FS_Open(_FilePath, FS_READ_WRITE | FS_CREATE);
    if(fp < 0)
    {
        LOGD(L_DRV, L_V4, "fp:%d", fp);
        OTA_FS_Close(fp);
        return -3;
    }
    fs_result = FS_Seek(fp, 0, FS_FILE_BEGIN);
    if(fs_result < FS_NO_ERROR)
    {
        FS_Close(fp);
        return -4;
    }
    while(len > 0)
    {
        if(len > 10000)
        {
            fs_result = FS_Write(fp, data, 10000, &writelen);
        }
        else
        {
            fs_result = FS_Write(fp, data, len, &writelen);
        }
        if(fs_result < FS_NO_ERROR)
        {
            FS_Close(fp);
            LOGD(L_DRV, L_V4, "fs_result:%d, %d(%d), %d", fs_result, len, _len - len, writelen);
            return -5;
        }
        FS_Commit(fp);
        len -= writelen;
        if(len > 0)
        {
            data += writelen;
        }
    }
    FS_GetFileSize(fp, &writelen);
    LOGD(L_DRV, L_V4, "\"%s\" �ļ�д��FAT��С:%d", filepath, writelen);
    FS_Close(fp);
    return writelen;
}

kal_int32 track_epo_write_file(kal_uint8 *data, kal_uint32 len)
{
    return track_write_file(MTKEPO_FILE_PATH, data, len);
}

void track_epo_file_size(void)
{
    FS_HANDLE fp = NULL;
    WCHAR FilePath[60] = {0};
    kal_uint32 writelen;

    kal_wsprintf(FilePath, MTKEPO_FILE_PATH);
    fp = FS_Open(FilePath, FS_READ_WRITE | FS_CREATE);
    FS_GetFileSize(fp, &writelen);
    LOGS("EPO�ļ�д��FAT��С:%d", writelen);
    FS_Close(fp);
}

/******************************************************************************
 *  Function    -  track_gps_hour_to_utc_view
 *
 *  Purpose     -  ����AGPS��GPS HOURת����UTC
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int8 track_gps_hour_to_utc_view(char *out)
{
    applib_time_struct result1 = {0}, result2 = {0};
    FS_HANDLE fp = NULL;
    int segment, epo_gps_hour1 = 0, epo_gps_hour2 = 0;
    WCHAR FilePath[60] = {0};
    kal_uint32 read, filesize, segments;
    int fs_result, ret = 0;

    kal_wsprintf(FilePath, MTKEPO_FILE_PATH);
    do
    {
        fp = FS_Open(FilePath, FS_READ_ONLY);
        if(fp < 0)
        {
            snprintf(out, 200, "ERROR: EPO file open error, if the file does not exist?");
            ret = -1;
            break;
        }
        fs_result = FS_GetFileSize(fp, &filesize);
        segments = filesize / (MTKEPO_RECORD_SIZE) / (MTKEPO_SV_NUMBER);
        if(segments <= 0)
        {
            snprintf(out, 200, "ERROR: filesize=%d, segments=%d", filesize, segments);
            ret = -2;
            break;
        }
        if(segments * (MTKEPO_RECORD_SIZE) * (MTKEPO_SV_NUMBER) != filesize)
        {
            snprintf(out, 200, "ERROR: EPO file size is incorrect, segments=%d, filesize=%d", segments, filesize);
            ret = -3;
            break;
        }

        fs_result = FS_Read(fp, &epo_gps_hour1, 4, &read);
        if(read != 4)
        {
            snprintf(out, 200, "ERROR: Error reading EPO file header.");
            ret = -5;
            break;
        }
        if(FS_NO_ERROR > FS_Seek(fp, filesize - MTKEPO_RECORD_SIZE, FS_FILE_BEGIN))
        {
            snprintf(out, 200, "ERROR: Move the EPO file to read the current coordinate error.");
            ret = -6;
            break;
        }
        fs_result = FS_Read(fp, &epo_gps_hour2, 4, &read);
        if(read != 4)
        {
            snprintf(out, 200, "ERROR: end of EPO file read error");
            ret = -7;
            break;
        }
        epo_gps_hour1 &= 0x00FFFFFF;
        epo_gps_hour2 &= 0x00FFFFFF;
        track_gps_hour_to_utc(epo_gps_hour1, &result1);
        track_gps_hour_to_utc(epo_gps_hour2, &result2);
        track_cust_agps_time_add_6hour(&result2);
        snprintf(out, 200, "EPO file validity: %d-%0.2d-%0.2d %0.2d:00:00 �� %d-%0.2d-%0.2d %0.2d:00:00",
                     result1.nYear, result1.nMonth, result1.nDay, result1.nHour,
                     result2.nYear, result2.nMonth, result2.nDay, result2.nHour);
    }
    while(0);
    FS_Close(fp);
    return ret;
}

/******************************************************************************
 *  Function    -  track_send_assistance_data
 *
 *  Purpose     -  ����ʱ���ж��Ƿ����EPO����
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int8 track_send_assistance_data(kal_bool ischeck, kal_uint16 iYr, kal_uint8 iMo, kal_uint8 iDay, kal_uint8 iHr)
{
    FS_HANDLE fp = NULL;
    int i, current_gps_hour/*��ǰϵͳʱ�任���hourֵ*/;
    int epo_gps_hour/*EPO�ļ��ĵ�һ��hourֵ*/, segment/*EPO�ļ���Чʣ�����*/;
    mtk_param_epo_data_cfg epo_data;
    WCHAR FilePath[60] = {0};
    kal_uint32 read, filesize, segments/*�Ѵ��ڵ�EPO�ļ����ּ���*/;
    int fs_result;
    kal_int8 ret = 1;

    // open EPO file and read the header (assume EPO file has passed integrity check)
    kal_wsprintf(FilePath, MTKEPO_FILE_PATH);
    fp = FS_Open(FilePath, FS_READ_ONLY);
    if(fp < 0)
    {
        FS_Close(fp);
        return -1;
    }
    fs_result = FS_GetFileSize(fp, &filesize);
    segments = filesize / (MTKEPO_RECORD_SIZE) / (MTKEPO_SV_NUMBER);
    if(segments <= 0)
    {
        FS_Close(fp);
        track_epo_delete_file();
        return -2;
    }
    if(segments * (MTKEPO_RECORD_SIZE) * (MTKEPO_SV_NUMBER) != filesize)
    {
        FS_Close(fp);
        track_epo_delete_file();
        return -3;
    }

    fs_result = FS_Read(fp, &epo_gps_hour, 4, &read);
    if(read != 4)
    {
        FS_Close(fp);
        track_epo_delete_file();
        return -5;
    }
    epo_gps_hour &= 0x00FFFFFF;

    // determine the segment to use
    current_gps_hour = track_utc_to_gps_hour(iYr, iMo, iDay, iHr);
    segment = (current_gps_hour - epo_gps_hour) / 6;
    LOGD(L_DRV, L_V5, "segment:%d(%d), current_gps_hour:%d, epo_gps_hour:%d", segment, segments, current_gps_hour, epo_gps_hour);
    if((segment < 0) || (segment >= segments/*MTKEPO_SEGMENT_NUM*/))
    {
        FS_Close(fp);
        track_epo_delete_file();
        return -6;
    }
    ret = segments - segment;
    if(!ischeck)
    {
        // seek to the begining of the segment that will be used.
        fs_result = FS_Seek(fp, segment * (MTKEPO_RECORD_SIZE) * (MTKEPO_SV_NUMBER), FS_FILE_BEGIN);

        // Read one segment of SV1 ~ SV32 satellite EPO data and send to MNL
        for(i = 0; i < MTKEPO_SV_NUMBER; i++)
        {
            epo_data.u1SatID = (i + 1);

            // Read one satellite EPO data
            fs_result = FS_Read(fp, epo_data.u4EPOWORD, MTKEPO_RECORD_SIZE, &read);
            if(read != MTKEPO_RECORD_SIZE)
            {
                LOGH(L_DRV, L_V1, epo_data.u4EPOWORD, read);
                LOGD(L_DRV, L_V1, "ʵ�ʶ��������������ȡ���Ȳ�һ��%d", read);
                break;
            }
            // Send one satellite EPO data to MNL
            track_drv_sys_mtk_gps_set_param(MTK_PARAM_CMD_CONFIG_EPO_DATA, &epo_data);
        }
        LOGD(L_APP, L_V4, "GPSʹ��EPO������λ");
    }
    FS_Close(fp);

    return ret;

}

/******************************************************************************
 *  Function    -  track_agps_epo_enable
 *
 *  Purpose     -  EPO ʹ�ܼ��
 *
 *  Description -  ���ݲ�����ʱ������ǿ�����أ������ݷ���Ӱ�춨λ�ٶ�
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 22-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static kal_int8 track_agps_epo_enable(void)
{
    kal_int8 ret = 0;
    t_rtc rtcTime = {0};
    track_gps_epo_struct * gps_data = NULL;

    RTC_GetTimeOnly(&rtcTime);
    //�״ο��������ȡ����ֵΪ���ֵ
    if((rtcTime.rtc_year == *OTA_DEFAULT_HARDWARE_YEAR) && (rtcTime.rtc_mon == *OTA_DEFAULT_HARDWARE_MON)\
            && (rtcTime.rtc_day == *OTA_DEFAULT_HARDWARE_DAY))   //����Ĭ������
    {
        LOGD(L_DRV, L_V5, "error %d-%d-%d", *OTA_DEFAULT_HARDWARE_YEAR, *OTA_DEFAULT_HARDWARE_MON, *OTA_DEFAULT_HARDWARE_DAY);
        ret |= 1;
    }
    gps_data = track_drv_get_gps_epo();
    if(!gps_data || !gps_data->valid)
    {
        ret |= 2;
    }
    return ret;
}

/******************************************************************************
 *  Function    -  track_gps_on
 *
 *  Purpose     -  ��GPS  task ���Ϳ�����Ϣ
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 27-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_gps_on(void)
{
    ilm_struct *send_ilm;
    LOGD(L_DRV, L_V5, "");
    allow_data_injection = KAL_TRUE;
    track_cust_gps_location_time(1);
    track_drv_cal_secset(1);
    track_drv_sleep_enable(SLEEP_DRV_MOD, FALSE);
#if defined( __MNL_SUPPORT__) 
    send_ilm = (ilm_struct *)allocate_ilm(MOD_MMI);
    send_ilm->src_mod_id = MOD_MMI;
    send_ilm->dest_mod_id = MOD_GPS;
    send_ilm->msg_id = MSG_ID_GPS_CONTROL_ON;
    msg_send_ext_queue(send_ilm);
#elif defined(__ZKE_EPO__)
    track_drv_zkw_cdata();
    track_drv_mgps_on();
#elif defined (__GPS_MT3333__)
    drv_gps_decode_status=1;
    track_gps_power_on_req(GPS_UART_MODE_DEBUG_RAW_DATA);
    LOGD(L_DRV, L_V5, "__GPS_MT3333__");
#elif defined(__GPS_M10382__) ||defined(__GPS_MT5631B__) || defined(__GPS_UART__) ||defined(__GT530__)
    track_drv_mgps_on();
#else
#error  "track_gps_on"
#endif /* __MNL_SUPPORT__ */
}


/******************************************************************************
 *  Function    -  track_gps_off
 *
 *  Purpose     -  ��GPS  task ���͹ر���Ϣ
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 27-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_gps_off(void)
{
     ilm_struct *send_ilm;
     LOGD(L_APP,L_V5,"");
#if defined( __MNL_SUPPORT__)
    send_ilm = (ilm_struct *)allocate_ilm(MOD_MMI);
    send_ilm->src_mod_id = MOD_MMI;
    send_ilm->dest_mod_id = MOD_GPS;
    send_ilm->msg_id = MSG_ID_GPS_CONTROL_OFF;
    msg_send_ext_queue(send_ilm);
#elif defined(__ZKE_EPO__)
    track_drv_mgps_off();
#elif defined __GPS_MT3333__
    drv_gps_decode_status=0;
    track_gps_power_off_req(GPS_UART_MODE_DEBUG_RAW_DATA);
#elif defined(__GPS_M10382__) ||defined(__GPS_MT5631B__) || defined(__GPS_UART__) ||defined(__GT530__)
    track_drv_mgps_off();
#else
#error  "track_gps_off"
#endif /* __MNL_SUPPORT__ */
    track_gps_data_clean();
    LOGD(L_DRV, L_V1, "");
    track_drv_sleep_enable(SLEEP_DRV_MOD, TRUE);
}

/*****************************************************************************
 *  Global Functions            ȫ�ֺ���
*****************************************************************************/
extern gps_uart_context_struct g_gps_cntx;
extern kal_uint8 GPS_Get_UART_Port(void);
//extern kal_int32 ret_value;

kal_eventgrpid g_gps_cmd_event_id = 0;
kal_mutexid g_gps_cmd_mutex = 0;

void track_send_msg_to_gps_task(msg_type msg_id,void *local_param_ptr, kal_uint32 para_size)
{
	/*----------------------------------------------------------------*/
	/* Local Variables */
	/*----------------------------------------------------------------*/
	ilm_struct *ilm_ptr;
	module_type src_mod;

	/*----------------------------------------------------------------*/
	/* Code Body */
	/*----------------------------------------------------------------*/
	src_mod = MOD_MMI;//stack_get_active_module_id();
	ilm_ptr = allocate_ilm(src_mod);
	ilm_ptr->src_mod_id = src_mod;
	ilm_ptr->dest_mod_id = MOD_GPS;
	ilm_ptr->sap_id = GPS_MNL_SAP;
	ilm_ptr->msg_id = msg_id;
	ilm_ptr->local_para_ptr = (local_para_struct*) local_param_ptr;
	ilm_ptr->peer_buff_ptr = (peer_buff_struct*) NULL;
	msg_send_ext_queue(ilm_ptr);
}

void track_gps_power_off_req(gps_common_uart_work_mode_enum work_mode)
{ 
	gps_uart_open_req_struct *local_param_ptr;
	module_type                         module_id;
	kal_uint32 event_group;
	kal_eventgrpid event_id;
	kal_int32 ret = 0;

	if (g_gps_cmd_event_id == 0)
	g_gps_cmd_event_id = OTA_kal_create_event_group((kal_char*)"gps_uart");
	if (g_gps_cmd_mutex == 0)
	g_gps_cmd_mutex = kal_create_mutex((kal_char*)"gps_uart_mutex");

	module_id = MOD_MMI;
	local_param_ptr = construct_local_para(sizeof(gps_uart_open_req_struct),TD_CTRL);
	local_param_ptr->port = (kal_uint16)GPS_UART_PORT;
	local_param_ptr->mode = work_mode;
	local_param_ptr->return_val = &ret;
	local_param_ptr->module_id = module_id;
	event_id = g_gps_cmd_event_id;
	local_param_ptr->event_id = event_id;

	kal_take_mutex(g_gps_cmd_mutex);
	track_send_msg_to_gps_task( MSG_ID_GPS_UART_CLOSE_REQ ,
	local_param_ptr,
	sizeof(gps_uart_open_req_struct));

	//l4c_send_msg(MOD_GPS, MSG_ID_GPS_UART_OPEN_REQ, 0, local_param_ptr, NULL);
	OTA_kal_retrieve_eg_events(
	event_id,
	GPS_UART_CLOSE_FINISH_EVENT,
	KAL_OR_CONSUME,
	&event_group,
	KAL_SUSPEND);
	kal_give_mutex(g_gps_cmd_mutex);

}

void track_gps_power_on_req(gps_common_uart_work_mode_enum work_mode)
{ 
	gps_uart_open_req_struct *local_param_ptr;
	module_type                         module_id;
	kal_uint32 event_group;
	kal_eventgrpid event_id;
	kal_int32 ret = 0;
	if (g_gps_cmd_event_id == 0)
	g_gps_cmd_event_id = OTA_kal_create_event_group((kal_char*)"gps_uart");

	if (g_gps_cmd_mutex == 0)
	g_gps_cmd_mutex = kal_create_mutex((kal_char*)"gps_uart_mutex");

	module_id = MOD_MMI;
	local_param_ptr = construct_local_para(sizeof(gps_uart_open_req_struct),TD_CTRL);
	local_param_ptr->port = (kal_uint16)GPS_UART_PORT;
	local_param_ptr->mode = work_mode;
	local_param_ptr->return_val = &ret;
	local_param_ptr->module_id = module_id;
	event_id = g_gps_cmd_event_id;
	local_param_ptr->event_id = event_id;

	kal_take_mutex(g_gps_cmd_mutex);
	track_send_msg_to_gps_task( MSG_ID_GPS_UART_OPEN_REQ,	local_param_ptr,sizeof(gps_uart_open_req_struct));

	#if 1
	OTA_kal_retrieve_eg_events(
	event_id,
	GPS_UART_OPEN_FINISH_EVENT,
	KAL_OR_CONSUME,
	&event_group,
	KAL_SUSPEND);
#endif
	kal_give_mutex(g_gps_cmd_mutex);

}

//����ֻ��Ϊ���ٴ���ռ䣬��Ҫɾ��  chengj
#if 0
void track_gpstc_data_write_to_file(char * string, int len)
{
    static int file_hd;
    int size;
    char* name = "gpstc.txt";
    WCHAR gpstc_file[32] = {0};
    static kal_uint16 count=0;

    if(count==0)
    {
        kal_wsprintf(gpstc_file, name);
        file_hd = FS_Open(gpstc_file, FS_READ_ONLY);
        if(file_hd >= FS_NO_ERROR)
        {
            FS_Close(file_hd);
            FS_Delete(gpstc_file);
        }
        file_hd = FS_Open(gpstc_file, FS_READ_WRITE | FS_CREATE);
        if(file_hd < FS_NO_ERROR)
        {
            return;
        }
        LOGS("GPSTCд�ļ�c:/gpstc.txt");
    }

    if(len==0)
    {
        return;
    }

    if(count>=370)
    {
        return;
    }

    count++;
    
    if(count==370)
    {
        LOGS("GPSTCд�ļ�6���ӣ���д��");
        FS_Close(file_hd);
        count++;
        return;
    }
            
    FS_Write(file_hd, (void *)string, len, &size);
    FS_Commit(file_hd);
}
#endif

void gps_raw_data_hdler(void *msg)
{
/*----------------------------------------------------------------*/
/* Local Variables */
/*----------------------------------------------------------------*/
gps_uart_debug_raw_data_struct* uart_raw_data_ind = (gps_uart_debug_raw_data_struct*)msg;
kal_int8 gps_raw_buffer[1024]={0};
kal_uint32 readed;
kal_uint32 readleng=0;
/*----------------------------------------------------------------*/
/* Code Body */
/*----------------------------------------------------------------*/
    OTA_gps_uart_read_data_dbg(uart_raw_data_ind->port,gps_raw_buffer,256,&readed);
    readleng+=readed;
    OTA_kal_prompt_trace(MOD_GPS,"[buffer1]%s\n",&gps_raw_buffer[0]);
    if(track_status_gpstc(2))
    {
        //track_trace((char*)gps_raw_buffer, strlen((char*)gps_raw_buffer));
    }
    if(drv_gps_decode_status) //GPS�ر�ʱ������
    {
        //track_uart_decode((char*)gps_raw_buffer, strlen((char*)gps_raw_buffer));
    }
	ASSERT(readed<=256);
	gps_raw_buffer[readleng] = 0;
	while (readed == 256)
	{
        OTA_gps_uart_read_data_dbg(uart_raw_data_ind->port,&gps_raw_buffer[readleng],256,&readed);
        readleng+=readed;
        OTA_kal_prompt_trace(MOD_GPS,"[buffer2]%s\n",&gps_raw_buffer[0]);
	 ASSERT(readed<=256);
        gps_raw_buffer[readleng] = 0;
	}

       if(track_status_gpstc(2))
        {
            //͸�����ݴ�FAT  chengj
            //track_gpstc_data_write_to_file((char*)gps_raw_buffer, strlen((char*)gps_raw_buffer));
            
            track_trace((char*)gps_raw_buffer, strlen((char*)gps_raw_buffer));
        }
        
		if(drv_gps_decode_status) //GPS�ر�ʱ������
	    {
	        track_uart_decode((char*)gps_raw_buffer, strlen((char*)gps_raw_buffer));
	    }
}



/******************************************************************************
 *  Function    -  track_drv_get_epo_stage
 *
 *  Purpose     -  ��ѯEPO ���������Ƿ���Ч
 *
 *  Description -
 *      0x00EE = 0000 0000 1110 1110
 *      ��Ч��EPO SV������2, 3, 4, 6, 7, 8.
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 22-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
mtk_param_epo_stage_cfg track_drv_get_epo_stage(void)
{
    mtk_param_epo_stage_cfg epo_stage_cfg = {0};
#if defined (__MNL_SUPPORT__)
    mtk_gps_get_param(MTK_PARAM_EPO_STAGE_CONFIG, &epo_stage_cfg);
#endif /* __MNL_SUPPORT__ */
    return epo_stage_cfg;
}

/******************************************************************************
 *  Function    -  track_drv_gps_msg_epo_ready
 *
 *  Purpose     -  ֪ͨMMI  ����ע��EPO ��������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 29-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_gps_msg_epo_ready(void)
{
    ilm_struct *send_ilm;
    send_ilm = (ilm_struct *)allocate_ilm(MOD_MNL);
    send_ilm->src_mod_id = MOD_MNL;
    send_ilm->dest_mod_id = MOD_MMI;
    send_ilm->msg_id = MSG_ID_GPS_EPO_READY;
    msg_send_ext_queue(send_ilm);
}

#if 1//defined(__ZKE_EPO__)
#if defined(__ZKE_EPO__)
void track_drv_zkw_epo_update(void)
{
    static int attr = 0;
    if(!G_parameter.agps.sw)
    {
        LOGD(L_DRV, L_V4, "AGPS��");
        track_zkw_datatxt_clear();
        return;
    }
	//if(attr != 0)
    {
		if(!track_drv_aiding_zkwepo())
		{
        	LOGD(L_DRV, L_V4, "�п�΢�����������ؿ�ʼ");
        	track_cust_agps_get_data_force(2);
    	}
	}
	//else attr = 1;
}

#endif
// λ�ýṹ��
typedef struct
{
    double 							lat;		// γ�ȣ�������ʾ��γ��������ʾ��γ
    double 							lon;		// ���ȣ�������ʾ������������ʾ����
    double 							alt;		// �߶ȣ�����߶��޷���ȡ����������Ϊ0
    int								valid;

} POS_LLA_STR;

// ʱ��ṹ��(ע�⣺������UTCʱ�䣡�����뱱��ʱ����8��Сʱ�Ĳ�࣬��Ҫֱ��ʹ�ñ���ʱ�䣡����)
// ���籱��ʱ��2016.5.8,10:34:23����ôUTCʱ��Ӧ����2016.5.8,02:34:23
// ���籱��ʱ��2016.5.8,03:34:23����ôUTCʱ��Ӧ����2016.5.7,19:34:23
typedef struct
{
    int								valid;		// ʱ����Ч��־��1=��Ч��������Ч
    int								year;
    int								month;
    int								day;
    int								hour;
    int								minute;
    int								second;
    float							ms;

} DATETIME_STR;

// ������Ϣ��λ�ã�ʱ�䣬Ƶ�ʣ�
typedef struct
{
    double							xOrLat, yOrLon, zOrAlt;
    double							tow;
    float							df;
    float							posAcc;
    float							tAcc;
    float							fAcc;
    unsigned int					res;
    unsigned short int				wn;
    unsigned char					timeSource;
    unsigned char					flags;

} AID_INI_STR;

/*************************************************************************
�������ƣ�casicAgnssAidIni
�������ܣ��Ѹ���λ�ú͸���ʱ������ר�õ����ݸ�ʽ����������Ϣ��ʽ���������
�������룺dateTime��������ʱ�䣬������Ч��־��1��Ч��
		  lla, ��γ�ȱ�־��������Ч��־��1��Ч��
���������aidIniMsg[66]���ַ����飬������Ϣ���ݰ������ȹ̶�
*************************************************************************/
void casicAgnssAidIni(DATETIME_STR dateTime, POS_LLA_STR lla, char aidIniMsg[66]);
kal_int8 track_zkw_eph_assistant_param(void)
{
    applib_time_struct  rtc, gmt0;
    DATETIME_STR datatime;
    POS_LLA_STR pos_lla;
    char aidIniMsg[66];

    mtk_param_epo_pos_cfg epo = {0};

    track_fun_get_time(&rtc, KAL_TRUE, NULL);
    //���ϵͳʱ�����GMT0
    track_agps_epo_time(rtc);
	track_cust_agps_get_last_position(1, &epo);
    if(epo.dfLAT==0 && epo.dfLON==0)
    {
		LOGD(L_APP, L_V4, "��γ�ȴ���!!!!");
		return-2;
	}
	
    if(1)//track_cust_agps_valid(2)
    {
        track_cust_agps_get_last_position(1, &epo);

        datatime.year = rtc.nYear;
        datatime.month = rtc.nMonth;
        datatime.day = rtc.nDay;
        datatime.hour = rtc.nHour;
        datatime.minute = rtc.nMin;
        datatime.second = rtc.nSec;
        datatime.ms = 0.0;
        datatime.valid = 1;

        pos_lla.alt = epo.dfALT;
        pos_lla.lat = epo.dfLAT;
        pos_lla.lon = epo.dfLON;
        pos_lla.valid = 1;
    }
    else
    {
        return -1 ;
    }
    LOGD(L_APP, L_V4, "�п�΢��������year=%d,month=%d,day=%d,hour=%d,miniter= %d,sec=%d;lat=%f,lon=%f,alt=%f",
         datatime.year, datatime.month, datatime.day,
         datatime.hour, datatime.minute, datatime.second, pos_lla.lat, pos_lla.lon, pos_lla.alt);
    casicAgnssAidIni(datatime, pos_lla, aidIniMsg);
    LOGH(L_APP, L_V4, aidIniMsg, 66);
    U_PutUARTBytes(2, aidIniMsg, 66);//GPS_UART_PORT
    return 0;
}
/************************************************************
�������ƣ�isLeapYear
�������ܣ������жϡ��жϹ�������һ�򣬰��겻���İ�������
�������룺year�����ж����
���������1, ���꣬0�������꣨ƽ�꣩
************************************************************/
int isLeapYear(int year)
{
    if((year & 0x3) != 0)
    {
        // ���year����4�ı�����һ����ƽ��
        return 0;
    }
    else if((year % 400) == 0)
    {
        // year��400�ı���
        return 1;
    }
    else if((year % 100) == 0)
    {
        // year��100�ı���
        return 0;
    }
    else
    {
        // year��4�ı���
        return 1;
    }
}
/*************************************************************************
�������ƣ�	gregorian2SvTime
�������ܣ�	ʱ���ʽת��, ��Ҫ����UTC��������
			�����ʱ���ʽ�ǳ����������ʱ�����ʽ��ʱ�䣻
			ת�����ʱ���ʽ��GPSʱ���ʽ��������������ʱ��ʾ��GPS��ʱ�������1980.1.6
			GPSʱ��û��������������������ʱ�䣬������ʱ���Ǿ�������������
			2016�����������ֵ��17��
�������룺	pDateTime,	�ṹ��ָ�룬������ʱ�����ʽ��ʱ��
���������	pAidIni,	�ṹ��ָ�룬����������ʱ����������������ʱ��ʱ���ʽ
*************************************************************************/
void gregorian2SvTime(DATETIME_STR *pDateTime, AID_INI_STR *pAidIni)
{
    int DayMonthTable[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int i, dn, wn;
    double tod, tow;

    // ����ʱ��
    tod = pDateTime->hour * 3600 + pDateTime->minute * 60 + pDateTime->second + pDateTime->ms / 1000.0;

    // �ο�ʱ��: 1980.1.6
    dn = pDateTime->day;
    // ��->��
    for(i = 1980; i < (pDateTime->year); i++)
    {
        if(isLeapYear(i))
        {
            dn += 366;
        }
        else
        {
            dn += 365;
        }
    }
    dn -= 6;
    // ��->��
    if(isLeapYear(pDateTime->year))
    {
        DayMonthTable[1] = 29;
    }
    for(i = 1; i < pDateTime->month; i++)
    {
        dn += DayMonthTable[i - 1];
    }

    // ����+����ʱ��
    wn	= (dn / 7);						// ����
    tow = (dn % 7) * 86400 + tod + 17;	// ����ʱ�䣬��������

    if(tow >= 604800)
    {
        wn++;
        tow -= 604800;
    }

    pAidIni->wn 	= wn;
    pAidIni->tow 	= tow;
}
/*************************************************************************
�������ƣ�casicAgnssAidIni
�������ܣ��Ѹ���λ�ú͸���ʱ������ר�õ����ݸ�ʽ����������Ϣ��ʽ���������
�������룺dateTime��������ʱ�䣬������Ч��־��1��Ч��
		  lla, ��γ�ȱ�־��������Ч��־��1��Ч��
���������aidIniMsg[66]���ַ����飬������Ϣ���ݰ������ȹ̶�
*************************************************************************/
void casicAgnssAidIni(DATETIME_STR dateTime, POS_LLA_STR lla, char aidIniMsg[66])
{
    AID_INI_STR aidIni;
    int ckSum, i;
    int *pDataBuff = (int*)&aidIni;

    gregorian2SvTime(&dateTime, &aidIni);

    aidIni.df			= 0;
    aidIni.xOrLat		= lla.lat;
    aidIni.yOrLon		= lla.lon;
    aidIni.zOrAlt		= lla.alt;
    aidIni.fAcc			= 0;
    aidIni.posAcc		= 0;
    aidIni.tAcc			= 0;
    aidIni.timeSource	= 0;

    aidIni.flags 		= 0x20;											// λ�ø�ʽ��LLA��ʽ���߶���Ч��Ƶ�ʺ�λ�þ��ȹ�����Ч
    aidIni.flags 		= aidIni.flags | ((lla.valid 	  == 1) << 0); 	// BIT0��λ����Ч��־
    aidIni.flags 		= aidIni.flags | ((dateTime.valid == 1) << 1); 	// BIT1��ʱ����Ч��־

    // �������ݴ��
    ckSum = 0x010B0038;
    for(i = 0; i < 14; i++)
    {
        ckSum += pDataBuff[i];
    }

    aidIniMsg[0] = 0xBA;
    aidIniMsg[1] = 0xCE;
    aidIniMsg[2] = 0x38;		// LENGTH
    aidIniMsg[3] = 0x00;
    aidIniMsg[4] = 0x0B;		// CLASS	ID
    aidIniMsg[5] = 0x01;		// MESSAGE	ID

    memcpy(&aidIniMsg[6], (char*)(&aidIni), 56);
    memcpy(&aidIniMsg[62], (char*)(&ckSum),  4);

    return;
}
#endif

/******************************************************************************
 *  Function    -  track_drv_gps_epo_start
 *
 *  Purpose     -  ��ʼע�븨������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 22-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_gps_epo_start(void)
{
    kal_int8 ret = 1;
    applib_time_struct  rtc, gmt0;
	
#if !defined (__SECOND_SET__)
	
    track_drv_gps_epo_req();    //�Զ�����ע��ӿ�
    if(!track_cust_agps_valid(1))
    {
        return;
    }
    ret = track_agps_epo_enable();
    LOGD(L_DRV, L_V4, "%d", ret);
    if(ret & 1)
    {
        LOGD(L_DRV, L_V4, "ERROR:ϵͳʱ�䲻��ȷ");
        return;
    }

    //applib_dt_get_rtc_time(&rtc);
    track_fun_get_time(&rtc, KAL_TRUE, NULL);
    //���ϵͳʱ�����GMT0
    track_agps_epo_time(rtc);

    if(track_cust_agps_valid(2))
    {
        mtk_param_epo_pos_cfg epo = {0};
        track_cust_agps_get_last_position(1, &epo);
        epo.u2YEAR  = rtc.nYear;
        epo.u1MONTH = rtc.nMonth;
        epo.u1DAY = rtc.nDay;
        epo.u1HOUR = rtc.nHour;
        epo.u1MIN = rtc.nMin;
        epo.u1SEC = rtc.nSec;
        ret = track_send_assistance_data(KAL_TRUE, epo.u2YEAR, epo.u1MONTH, epo.u1DAY, epo.u1HOUR);
        if(0 < ret && (epo.dfLAT > 0 || epo.dfLON > 0))
        {
          if(allow_data_injection)
            {
                LOGD(L_DRV, L_V4, "injection");
                allow_data_injection = KAL_FALSE;
            }
            else
            {
                LOGD(L_DRV, L_V4, "�Ѿ�ע�����");
                return;
            }
            LOGD(L_APP, L_V4, "AGPS ע��ο���(%d)  ����%0.2f, Lat:%0.6f, Lon:%0.6f, %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
                 ret, (double)epo.dfALT, (double)epo.dfLAT, (double)epo.dfLON,
                 epo.u2YEAR, epo.u1MONTH, epo.u1DAY,
                 epo.u1HOUR, epo.u1MIN, epo.u1SEC);
            track_drv_sys_mtk_gps_set_param(MTK_PARAM_CMD_CONFIG_EPO_POS, &epo);
        }
        else
        {
            LOGD(L_APP, L_V4, "AGPS ����Ч�ο����������ο���ע��(%d) ����%0.2f, Lat:%0.6f, Lon:%0.6f, %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
                 ret, (double)epo.dfALT, (double)epo.dfLAT, (double)epo.dfLON,
                 epo.u2YEAR, epo.u1MONTH, epo.u1DAY,
                 epo.u1HOUR, epo.u1MIN, epo.u1SEC);
        }
    }
    if(assist_data_ready)
    {
        LOGD(L_APP, L_V4, "AGPS �ο�ʱ��%d-%d-%d %d:00", rtc.nYear, rtc.nMonth, rtc.nDay, rtc.nHour);
        ret = track_send_assistance_data(KAL_FALSE, rtc.nYear, rtc.nMonth, rtc.nDay, rtc.nHour);
        if(ret < 0)
        {
            LOGD(L_APP, L_V4, "ret data download fail (%d)", ret);
        }
    }
#endif
}

/******************************************************************************
 *  Function    -  track_drv_gps_cold_restart
 *
 *  Purpose     -  GPS����������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 29-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_gps_cold_restart(void)
{
    mtk_param_restart cfg;

    track_drv_gps_claer_eph();
#if defined(__MNL_SUPPORT__)
    cfg.restart_type = MTK_GPS_START_COLD;
    mtk_gps_set_param(MTK_PARAM_CMD_RESTART, &cfg);
#elif defined(__GPS_UART__) //||defined(__GPS_MT3333__) 
    //track_drv_gps_set_param(MTK_MSG_REQ_ASSIST, NULL);//wangqi
    track_cust_ubxcold();
    track_gps_data_clean();
#elif defined(__GT530__)
#else
    #error "track_drv_gps_cold_restart"
#endif /* __MNL_SUPPORT__ */
    //track_drv_gps_switch(1);
}

/******************************************************************************
 *  Function    -  track_drv_gps_cold_restart
 *
 *  Purpose     -  GPS����������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 20140806,    written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_gps_warm_restart(void)
{
    mtk_param_restart cfg;

    //track_drv_gps_claer_eph();
#if defined(__MNL_SUPPORT__)
    cfg.restart_type = MTK_GPS_START_WARM;
    mtk_gps_set_param(MTK_PARAM_CMD_RESTART, &cfg);
#elif defined(__GPS_UART__)
    //track_drv_gps_set_param(MTK_MSG_REQ_ASSIST, NULL);//wangqi
    track_cust_exgps_warm();
    track_gps_data_clean();
#elif defined(__GT530__)//||defined(__GPS_MT3333__)
#else
    #error "track_drv_gps_cold_restart"
#endif /* __MNL_SUPPORT__ */
    //track_drv_gps_switch(1);
}

void track_drv_gps_hot_restart(void)
{
    mtk_param_restart cfg;
    LOGD(L_DRV, L_V5, "");
#if defined(__MNL_SUPPORT__)
    cfg.restart_type = MTK_GPS_START_HOT;
    mtk_gps_set_param (MTK_PARAM_CMD_RESTART, &cfg);
#elif defined(__GPS_UART__)//||defined(__GPS_MT3333__) 
    track_cust_ubxhot();
    track_gps_data_clean();
#elif defined(__GT530__)//||defined(__GPS_MT3333__)

#else
    #error "track_drv_gps_cold_restart"
#endif /* __MNL_SUPPORT__ */

}
/******************************************************************************
 *  Function    -  track_drv_gps_claer_eph
 *
 *  Purpose     -  ���GPS����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 27-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_gps_claer_eph(void)
{
    track_drv_sys_mtk_gps_sys_storage_clear();
}

/******************************************************************************
 *  Function    -  track_drv_gps_switch
 *
 *  Purpose     -  GPS ����
 *
 *  Description -   EPO>Hot>Cold
 *     0=�ر�
 *     1=����EPO
 *     2=����������
 *     3=������
 *
 * Return
 *     0=��/1=����Ϊ������/2=����Ϊ������/3=ǿ���ٴο���
 *     error:  -1=���β�������ǰ��ͬ��ִ��/-2= �����������
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 27-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_int8 track_drv_gps_switch(U32 status)
{
    static U8 pre_status = 0xFF;
    kal_int8 exe;

#if defined(__GT741__)
    LOGD(L_APP, L_V4, "GT741������GPS");
	return;
#endif
#ifdef __GT530__
LOGD(L_DRV, L_V5, "%d,LTE:%d", status, track_cust_lte_systemup(9));

    //return;
    if(0 == track_cust_lte_systemup(9))
    {
        return  -2;
    }
#else
	LOGD(L_DRV, L_V5, "%d", status );
#endif /* __GT530__ */

#if defined (FOR_RS485_TEST_TOOL)
	return;
#endif /* FOR_RS485_TEST_TOOL */

    if(status == 1 || status == 3)
    {
        assist_data_ready = KAL_TRUE;
        track_gps_on();
        pre_status = 1;
        return 1;
    }
    else if(status == 2)
    {
        assist_data_ready = KAL_FALSE;
        track_gps_on();
        pre_status = 2;
        return 2;
    }
    else if(status == 0)
    {
        track_stop_timer(GPS_RESET_TIMER_ID);
        if(track_is_timer_run(TRACK_CUST_PERIODIC_RESTART_GPS_TIMER_ID))
        {
            track_stop_timer(TRACK_CUST_PERIODIC_RESTART_GPS_TIMER_ID);
        }
        track_gps_off();
        pre_status = 0;
        return 0;
    }
    else
    {
        return -2;
    }
}

kal_int8 track_drv_gps_reset(kal_uint8 delay)
{
    LOGD(L_DRV, L_V5, "(%d)", delay);
    track_drv_gps_switch(0);
    if(delay == 0)
    {
        delay = 2;
    }
    track_start_timer(GPS_RESET_TIMER_ID, delay * 1000, track_drv_gps_switch, (void*)1);
}


#if defined(__GPS_G3333__) || defined(__GPS_MT5631B__) 
#define GPS_DEFAULT_MODE GN
#define EHP_MODE_NUMBER  3
#elif defined( __BD_AT6558__)
#define GPS_DEFAULT_MODE GN
#define EHP_MODE_NUMBER  6
#else
#define GPS_DEFAULT_MODE GPS
#define EHP_MODE_NUMBER  3
#endif

#if  defined( __BD_AT6558__)
static char * GB[] = {
    "$PCAS04,2*1B\r\n", 
    "$PCAS04,1*18\r\n", 
    "$PCAS04,3*1A\r\n", 
    "$PCAS03,,,,,,,,,,,1*1F\r\n", 
    "$PCAS03,,,,,,,,,,,0*1E\r\n", 
    "$CCRMO,GGA,4,1*38\r\n"
    };
#else
static char * GB[] = {"$PMTK353,0,0,0,0,1*2A\r\n", "$PMTK353,1,0,0,0,0*2A\r\n", "$PMTK353,1,0,0,0,1*2B\r\n","$PMTK353,1,0,0,0,1*2A\r\n"};
#endif
kal_uint16 track_cust_set_gps_mode(int g_mode)
{
    kal_uint8 length ;

    static enum_gps_mode gps_mode = GPS_DEFAULT_MODE;
#if defined(__ATGM_UPDATE__)
    if(track_cust_atgm_update_status(STATUS_CHECK) >= ATGM_UPDATE_STATUS_update_begin_req)
    {
        LOGD(L_APP, L_V1, "����������,������gps_mode");
        return;
    }
#endif /*__ATGM_UPDATE__*/

    if(0 <= g_mode && g_mode <= EHP_MODE_NUMBER)
    {
        length = strlen((char*)GB[g_mode]);
        //track_uart_write(GPS_UART_PORT, GB[g_mode], strlen((char*)GB[g_mode]));
        U_PutUARTBytes(GPS_UART_PORT, GB[g_mode], length);
        gps_mode = g_mode;
        LOGD(L_DRV, L_V5 ,"%s,%d", GB[g_mode], length);

    }

    LOGD(L_DRV, L_V9, "mode:%d, %d", gps_mode, g_mode);
    return gps_mode;
}

int track_drv_gps_set_param(kal_uint16 key, const void* value)
{
    char tmp[200] = {0}, out[200] = {0};
    int len = 0;
    switch(key)
    {
        case MTK_PARAM_CMD_CONFIG_EPO_TIME:
            {
                mtk_param_epo_time_cfg *epo_time_cfg = (mtk_param_epo_time_cfg*)value;
                snprintf(tmp, 200, "PMTK740,%d,%d,%d,%d,%d,%d",
                         epo_time_cfg->u2YEAR, epo_time_cfg->u1MONTH, epo_time_cfg->u1DAY,
                         epo_time_cfg->u1HOUR, epo_time_cfg->u1MIN, epo_time_cfg->u1SEC);
                len = snprintf(out, 200, "$%s*%02X\r\n", tmp, calc_nmea_checksum(tmp));
                break;
            }
        case MTK_PARAM_CMD_CONFIG_EPO_POS:
            {
                mtk_param_epo_pos_cfg *epo_pos_cfg = (mtk_param_epo_pos_cfg*)value;
                snprintf(tmp, 200, "PMTK741,%2.6f,%2.6f,%2.6f,%d,%d,%d,%d,%d,%d",
                         epo_pos_cfg->dfLAT, epo_pos_cfg->dfLON, epo_pos_cfg->dfALT,
                         epo_pos_cfg->u2YEAR, epo_pos_cfg->u1MONTH, epo_pos_cfg->u1DAY,
                         epo_pos_cfg->u1HOUR, epo_pos_cfg->u1MIN, epo_pos_cfg->u1SEC);
                len = snprintf(out, 200, "$%s*%02X\r\n", tmp, calc_nmea_checksum(tmp));
                break;
            }
        case MTK_PARAM_CMD_CONFIG_EPO_DATA:
            {
                mtk_param_epo_data_cfg *epo_data = (mtk_param_epo_data_cfg*)value;
                snprintf(tmp, 200, "PMTK721,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X",
                         epo_data->u1SatID,
                         epo_data->u4EPOWORD[0], epo_data->u4EPOWORD[1], epo_data->u4EPOWORD[2],
                         epo_data->u4EPOWORD[3], epo_data->u4EPOWORD[4], epo_data->u4EPOWORD[5],
                         epo_data->u4EPOWORD[6], epo_data->u4EPOWORD[7], epo_data->u4EPOWORD[8],
                         epo_data->u4EPOWORD[9], epo_data->u4EPOWORD[10], epo_data->u4EPOWORD[11],
                         epo_data->u4EPOWORD[12], epo_data->u4EPOWORD[13], epo_data->u4EPOWORD[14],
                         epo_data->u4EPOWORD[15], epo_data->u4EPOWORD[16], epo_data->u4EPOWORD[17]);
                len = snprintf(out, 200, "$%s*%02X\r\n", tmp, calc_nmea_checksum(tmp));
                break;
            }
        case MTK_MSG_REQ_ASSIST:
            {/*GPS������*/
                len = snprintf(out, 200, "$PMTK103*30\r\n");
                break;
            }
    }
    if(len > 0)
    {
        //LOGD(L_DRV, L_V5, "%s", out);
        U_PutUARTBytes(uart_port2, out, len);
    }
    return 0;
}

