/*******************************************************************************************
 * Filename:
 * Author :
 * Date :
 * Comment:
 ******************************************************************************************/

/****************************************************************************
*  Include Files                        ����ͷ�ļ�
*****************************************************************************/
#include "track_cust.h"
#include "app_datetime.h"
#include "track_os_ell.h"

/*****************************************************************************
*  Define                               �궨��
******************************************************************************/

/*****************************************************************************
*  Typedef  Enum                        ö�ٶ���
*****************************************************************************/

/*****************************************************************************
*  Struct                               ���ݽṹ����
******************************************************************************/

/*****************************************************************************
*  Local variable                       �ֲ�����
*****************************************************************************/
static nvram_gps_backup_data_param_struct backup_data_param = {0};
static U8 g_reset_sign = 0;

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

/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/

/******************************************************************************
 *  Function    -  track_cust_backup_data_get_total_in_queue
 *
 *  Purpose     -  ���������ĳ���
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 21-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint32 track_cust_backup_data_get_total_in_queue(void)
{
    kal_uint16 ret = 0;

    if(backup_data_param.data_first == 0)
    {
        backup_data_param.data_first = 1;
    }
    if(backup_data_param.data_last == 0)
    {
        backup_data_param.data_last = 1;
    }
    if(backup_data_param.data_first <= backup_data_param.data_last)
    {
        ret = backup_data_param.data_last - backup_data_param.data_first;
    }
    else
    {
        ret = BACKUP_DATA_MAX - (backup_data_param.data_first - backup_data_param.data_last); //��ѧ����,���һ
    }
    LOGD(L_APP, L_V9, "ret=%d", ret);
    return ret;
}

/******************************************************************************
 *  Function    -  backup_data_add_in_queue
 *
 *  Purpose     -  ���������ӵ�����
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 21-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_backup_data_add_in_queue(void* data)
{
    S16 Error;

    LOGD(L_APP, L_V7, "");
    if(backup_data_param.data_first == 0)
    {
        backup_data_param.data_first = 1;
    }
    if(backup_data_param.data_last == 0)
    {
        backup_data_param.data_last = 1;
    }
    Track_nvram_write(NVRAM_EF_GPS_BACKUP_DATA_LID, backup_data_param.data_last, (void *)data, NVRAM_EF_GPS_BACKUP_DATA_SIZE);
    backup_data_param.data_last++;

    if(backup_data_param.data_last > BACKUP_DATA_MAX) //����λ�ò��ܴ���BACKUP_DATA_MAX
    {
        backup_data_param.data_last = 1;
    }

    if(backup_data_param.data_last == backup_data_param.data_first)
    {
        LOGD(L_APP, L_V4, "NVRAM����1000����������ʧ�������ݱ��������ݣ�");
        backup_data_param.data_first++;

        if(backup_data_param.data_first > BACKUP_DATA_MAX)
        {
            backup_data_param.data_first = 1;
        }
    }
    Track_nvram_write(NVRAM_EF_GPS_BACKUP_DATA_PARAM_LID, 1, (void *)&backup_data_param, NVRAM_EF_GPS_BACKUP_DATA_PARAM_SIZE);
}

/******************************************************************************
 *  Function    -  backup_data_del_out_queue
 *
 *  Purpose     -  ��GPS������ɾ������
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 21-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_bool backup_data_del_out_queue(kal_uint32 total)
{
    S16 Error;
    S16 temp_total = 0;

    LOGD(L_APP, L_V5, "%d", total);
    if(backup_data_param.data_first == 0)
    {
        backup_data_param.data_first = 1;
    }
    if(backup_data_param.data_last == 0)
    {
        backup_data_param.data_last = 1;
    }
    temp_total = track_cust_backup_data_get_total_in_queue();
    if((temp_total == 0) || (total == 0))
    {
        return FALSE;
    }
    if(temp_total < total)
    {
        total = temp_total;
    }
    backup_data_param.data_first += total;

    if(backup_data_param.data_first > BACKUP_DATA_MAX)
    {
        backup_data_param.data_first = backup_data_param.data_first - BACKUP_DATA_MAX;
    }

    Track_nvram_write(NVRAM_EF_GPS_BACKUP_DATA_PARAM_LID, 1, (void *)&backup_data_param, NVRAM_EF_GPS_BACKUP_DATA_PARAM_SIZE);
    return TRUE;
}

/******************************************************************************
 *  Function    -  backup_data_read_from_queue_not_del
 *
 *  Purpose     -  �Ӷ����ж���GPS����
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 21-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int32 backup_data_read_from_queue_not_del(void *to, kal_uint16 step)
{
    S16 Error;
    S16 record_id = 0;

    LOGD(L_APP, L_V5, "");
    if(backup_data_param.data_first == 0)
    {
        backup_data_param.data_first = 1;
    }
    if(backup_data_param.data_last == 0)
    {
        backup_data_param.data_last = 1;
    }
    if((1 > track_cust_backup_data_get_total_in_queue()) || (step < 1))
    {
        return -1;
    }
    
    record_id = backup_data_param.data_first + step - 1;
    
    if(record_id > BACKUP_DATA_MAX)
    {
        record_id -= BACKUP_DATA_MAX;
    }
    
    Track_nvram_read(NVRAM_EF_GPS_BACKUP_DATA_LID, record_id, to, NVRAM_EF_GPS_BACKUP_DATA_SIZE);

    return NVRAM_EF_GPS_BACKUP_DATA_SIZE;
}

/******************************************************************************
 *  Function    -  save_backup_gps_data
 *
 *  Purpose     -  ��ʱ��������
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void save_backup_gps_data(void)
{
    track_cust_backup_gps_data(4, NULL);
}

/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/

/******************************************************************************
 *  Function    -  GPS_NVRAM_Backup_Init
 *
 *  Purpose     -  ���һ����λ�㱸�ݵĳ�ʼ��
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void GPS_NVRAM_Backup_Init(void)
{
    Track_nvram_read(NVRAM_EF_GPS_BACKUP_DATA_PARAM_LID, 1, (void *)&backup_data_param, NVRAM_EF_GPS_BACKUP_DATA_PARAM_SIZE);
}

/******************************************************************************
 *  Function    -  track_cust_backup_gps_data
 *
 *  Purpose     -  ���һ����γ�ȱ��ݡ���ȡ����
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
track_gps_data_struct *track_cust_backup_gps_data(kal_uint8 type, track_gps_data_struct *new_data)
{
    static track_gps_data_struct data = {0};
    static track_gps_data_struct uploaddata = {0};
    static nvram_gps_last_data_struct backup = {0};
    static int realtimeCount = 0;
    static U8 read_sign = 1, read_sign2 = 1;
    LOGD(L_GPS, L_V7, "type=%d, reset_sign=%d", type, g_reset_sign);
    if(g_reset_sign)
    {
        memset(&data, 0, sizeof(track_gps_data_struct));
        return &data;
    }
    if(type == 0)
    { // ���µ�һ���Ѷ�λ��GPS���� ��
        if(read_sign && data.gprmc.status == 0)
        {
            kal_uint32 ret;
            read_sign = 0;
            LOGD(L_GPS, L_V5, "status=%d, nYear=%d, Lat:%0.6f, Lon:%0.6f", 
                data.gprmc.status, data.gprmc.Date_Time.nYear, data.gprmc.Latitude, data.gprmc.Longitude);
            track_cust_nvram_gps_last_rmc(1, (kal_uint8*)&backup);
            if((sizeof(nvram_gps_gprmc_struct) != sizeof(gps_gprmc_struct)) || (sizeof(nvram_gps_gpgga_struct) != sizeof(gps_gpgga_struct)))
            {
                LOGD(L_GPS, L_V1, "ERROR!!!!");
                track_cust_restart(18, 0);
                return &data;
            }
            memcpy(&data.gprmc, &backup.rmc, sizeof(nvram_gps_gprmc_struct));
            memcpy(&data.gpgga, &backup.gga, sizeof(nvram_gps_gpgga_struct));
            memcpy(&data.lbs, &backup.lbs, sizeof(Multi_Cell_Info_Struct));
        }
        LOGD(L_GPS, L_V5, "status=%d, nYear=%d, Lat:%0.6f, Lon:%0.6f", 
            data.gprmc.status, data.gprmc.Date_Time.nYear, data.gprmc.Latitude, data.gprmc.Longitude);
    }
    else if(type == 1)
    { // ���µ�һ���Ѷ�λ��GPS���� д
        memcpy(&data, new_data, sizeof(track_gps_data_struct));
        if((sizeof(nvram_gps_gprmc_struct) != sizeof(gps_gprmc_struct)) || (sizeof(nvram_gps_gpgga_struct) != sizeof(gps_gpgga_struct)))
        {
            LOGD(L_GPS, L_V1, "ERROR!!!!");
            track_cust_restart(19, 0);
            return &data;
        }
        LOGD(L_GPS, L_V7, "realtimeCount=%d, status=%d, nYear=%d, Lat:%0.6f, Lon:%0.6f", 
            realtimeCount, data.gprmc.status, data.gprmc.Date_Time.nYear, data.gprmc.Latitude, data.gprmc.Longitude);
    }
    else if(type == 2)
    { // ��Ч�ϴ������� ��
        if(read_sign2 && uploaddata.gprmc.status == 0)
        {
            LOGD(L_GPS, L_V5, "status=%d, nYear=%d, Lat:%0.6f, Lon:%0.6f", 
                uploaddata.gprmc.status, uploaddata.gprmc.Date_Time.nYear, uploaddata.gprmc.Latitude, uploaddata.gprmc.Longitude);
            track_cust_nvram_gps_last_rmc(1, (kal_uint8*)&backup);
            if((sizeof(nvram_gps_gprmc_struct) != sizeof(gps_gprmc_struct)) || (sizeof(nvram_gps_gpgga_struct) != sizeof(gps_gpgga_struct)))
            {
                LOGD(L_GPS, L_V1, "ERROR!!!!");
                track_cust_restart(20, 0);
                return &uploaddata;
            }
            memcpy(&uploaddata.lbs, &backup.lbs, sizeof(Multi_Cell_Info_Struct));
            if(backup.rmc.status)
            {
                memcpy(&uploaddata.gprmc, &backup.rmc, sizeof(nvram_gps_gprmc_struct));
                memcpy(&uploaddata.gpgga, &backup.gga, sizeof(nvram_gps_gpgga_struct));
            }
            else
            {
                memcpy(&uploaddata, &data, sizeof(track_gps_data_struct));
            }
            if(uploaddata.gprmc.status == 1)
            {
                read_sign2 = 0;
            }
        }
        return &uploaddata;
    }
    else if(type == 3)
    { // ��Ч�ϴ������� д
        static U8 first = 1;
        memcpy(&uploaddata, new_data, sizeof(track_gps_data_struct));
        if((sizeof(nvram_gps_gprmc_struct) != sizeof(gps_gprmc_struct)) || (sizeof(nvram_gps_gpgga_struct) != sizeof(gps_gpgga_struct)))
        {
            LOGD(L_GPS, L_V1, "ERROR!!!!");
            track_cust_restart(21, 0);
            return &uploaddata;
        }
        if(!track_is_timer_run(TRACK_CUST_BACKUP_GPS_TIMER_ID))
        {
            tr_start_timer(TRACK_CUST_BACKUP_GPS_TIMER_ID, 60000, save_backup_gps_data);
        }
        realtimeCount++;
        LOGD(L_GPS, L_V7, "realtimeCount=%d, status=%d, nYear=%d, Lat:%0.6f, Lon:%0.6f", 
            realtimeCount, uploaddata.gprmc.status, uploaddata.gprmc.Date_Time.nYear, uploaddata.gprmc.Latitude, uploaddata.gprmc.Longitude);
        if(first)
        {
            first = 0;
            save_backup_gps_data();
        }
        return &uploaddata;
    }
    else if(type == 4)
    { // �ػ�ǰ�������ݣ�����д��
        static U8 first = 1;
        LBS_Multi_Cell_Data_Struct *lbs = track_drv_get_lbs_data();
        backup.distance=track_get_gps_distance();
        memcpy(&backup.lbs, &lbs->MainCell_Info, sizeof(Multi_Cell_Info_Struct));
        if(uploaddata.gprmc.status == 1 && (realtimeCount > 0 || first))
        {
            first = 0;
            realtimeCount = 0;
            memcpy(&backup.rmc, &uploaddata.gprmc, sizeof(gps_gprmc_struct));
            memcpy(&backup.gga, &uploaddata.gpgga, sizeof(nvram_gps_gpgga_struct));
            track_cust_nvram_gps_last_rmc(2, (kal_uint8*)&backup);
        }
        else
        {
            nvram_gps_last_data_struct backup1 = {0};
            track_cust_nvram_gps_last_rmc(1, (kal_uint8*)&backup1);
            if(backup1.rmc.status == 0 && data.gprmc.status == 1)
            {
                LOGD(L_GPS, L_V4, "��NVRAM�д洢���µĶ�λ��");
                memcpy(&backup.rmc, &data.gprmc, sizeof(gps_gprmc_struct));
                memcpy(&backup.gga, &data.gpgga, sizeof(nvram_gps_gpgga_struct));
                track_cust_nvram_gps_last_rmc(2, (kal_uint8*)&backup);
            }
        }
        return &uploaddata;
    }
    else if(type == 5)
    {
        nvram_gps_last_data_struct tmp = {0};
        Track_nvram_write(NVRAM_EF_GPS_POSITION_LID, 1, &tmp, NVRAM_EF_GPS_POSITION_SIZE);
        g_reset_sign = 1;
    }
    return &data;
}

U32 track_cust_backup_get_reset_sign(void)
{
    return g_reset_sign;
}

/******************************************************************************
 *  Function    -  track_test_view_backup
 *
 *  Purpose     -  ��ȡ��ǰ�ı���״̬
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_test_view_backup(char *rsp_msg)
{
    sprintf(rsp_msg, "nvram data:first=%d,%d\r\n", backup_data_param.data_first, backup_data_param.data_last);
}
void track_reset_backup(void)
{
        LOGD(L_APP, L_V1, "");
        backup_data_param.data_last = 1;
        backup_data_param.data_first= 1;
        gps_data_backup_manage(3,NULL,0,0);
}

