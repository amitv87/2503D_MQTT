/*******************************************************************************************
 * Filename:    
 * Author :     
 * Date :       
 * Comment:     
 ******************************************************************************************/

/****************************************************************************
*  Include Files                        ����ͷ�ļ�
*****************************************************************************/
#include "MMIDataType.h"
#include "TimerEvents.h"
#include "app_datetime.h"
#include "Track_cust.h"

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

/****************************************************************************
*  Global Variable                      ȫ�ֱ���
*****************************************************************************/
nvram_parameter_struct              G_parameter = {0};
nvram_realtime_struct               G_realtime_data = {0};
nvram_phone_number_struct           G_phone_number = {0};
nvram_iot_params_struct                G_iot = {0};

/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/
nvram_importance_parameter_struct G_importance_parameter_data;

/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/

/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/

/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/

/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/

/******************************************************************************
 *  Function    -  track_cust_nvram_data_init
 *
 *  Purpose     -  NVRAM�����״γ�ʼ��
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_nvram_data_init(void)
{
    static kal_bool isfirst = KAL_TRUE;
    LOGD(L_APP, L_V5, "%d",isfirst);
    //isfirst = KAL_TRUE;
    if(isfirst)
    {
    	nvram_realtime_struct iread={0};

        isfirst = KAL_FALSE;
        LOGD(L_APP, L_V5, "NVRAM_EF_PARAMETER_SIZE=%d", NVRAM_EF_PARAMETER_SIZE);
        LOGS("NVRAM_EF_REALTIME_SIZE %d",NVRAM_EF_REALTIME_SIZE);
		
        memcpy(&iread, OTA_NVRAM_EF_REALTIME_DEFAULT,  sizeof(*OTA_NVRAM_EF_REALTIME_DEFAULT));
        LOGS("track_cust_nvram_data_init %d,%d,%d,%d,%d,%d,%d,%d",iread.ams_log,iread.ams_push,iread.flashtime,iread.tracking_time,iread.up_param,iread.glp);

        Track_nvram_read(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        Track_nvram_read(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        Track_nvram_read(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        Track_nvram_read(NVRAM_EF_PHONE_NUMBER_LID, 1, (void *)&G_phone_number, NVRAM_EF_PHONE_NUMBER_SIZE);
        Track_nvram_read(NVRAM_EF_IOT_PARAMS_LID, 1, (void *)&G_iot, NVRAM_EF_IOT_PARAMS_SIZE);
        track_cust_backup_gps_data(0, NULL);
        track_gps_distance_init();
        GPS_NVRAM_Backup_Init();
#if !(defined (__XCWS__)|| defined(__GT740__)|| defined(__GT370__)|| defined(__GT380__)|| defined(__JM66__)||defined(__GT420D__))
        track_cust_oil_cut_boot();
#endif
        track_drv_sys_init_fake_cell();
#if defined(__JM66__)
        if(G_realtime_data.is_first_boot == 1)//�˳�����ģʽ����
        {
            is_quit_transport_boot = 1;
            G_realtime_data.is_first_boot = 0;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        }
#endif//__JM66__
    }
}

/******************************************************************************
 *  Function    -  track_cust_nvram_gps_last_rmc
 *
 *  Purpose     -  ��NVRAM�ж�/ȡ���һ���ϴ���
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_nvram_gps_last_rmc(kal_uint8 arg, kal_uint8 *data)
{
    LOGD(L_GPS, L_V7, "arg=%d", arg);
    if(arg == 1)
    {
        Track_nvram_read(NVRAM_EF_GPS_POSITION_LID, 1, data, NVRAM_EF_GPS_POSITION_SIZE);
    }
    else if(arg == 2)
    {
        Track_nvram_write(NVRAM_EF_GPS_POSITION_LID, 1, data, NVRAM_EF_GPS_POSITION_SIZE);
    }
}

