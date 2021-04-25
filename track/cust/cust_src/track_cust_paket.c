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
#include "track_os_ell.h"

/*****************************************************************************
*  Define                               �궨��
******************************************************************************/
static char *blank = {""}, *s_ON = {"ON"}, *s_OFF = {"OFF"}, *s_Error = {"ERROR"};
#define ON_OFF(value) (value==0?s_OFF:(value==1?s_ON:s_Error))

/*****************************************************************************
*  Typedef  Enum                        ö�ٶ���
*****************************************************************************/

/*****************************************************************************
*  Struct                               ���ݽṹ����
******************************************************************************/

/*****************************************************************************
*  Local variable                       �ֲ�����
*****************************************************************************/
static kal_uint16 g_SerialNumber = 0;

/****************************************************************************
*  Global Variable                      ȫ�ֱ���
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/
#if defined(__GT370__)|| defined (__GT380__)
extern kal_uint32 g_last_position_tick;
#endif
/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/
extern void track_gps_dataupload_modular(void);
extern BOOL track_terminal_sleepwork_get(void);
extern kal_uint8 track_cust_sensor_count(kal_uint8 par);
extern char *track_cust_get_version_for_upload_server(void);
extern nvram_GPRS_APN_struct *track_socket_get_use_apn(void);
extern kal_int8 track_cust_paket_FD(kal_uint8 *data, kal_uint16 len, U8 packet_type);
extern kal_bool track_drv_nvram_write_switch(kal_uint8 sw);
extern kal_uint32 track_cust_get_real_battery_volt();
extern kal_uint8 track_cust_modeule_get_wakemode();
#if defined(__GT370__)|| defined (__GT380__)
extern void track_send_lbs_limit_rtc(void);
#endif
#ifdef __GT420D__
extern float track_get_temp_form_mcu();
extern float track_get_humidity_form_mcu();
extern kal_uint32 track_cust_module_get_ele_data();
extern kal_uint16 track_cust_module_get_stsn();
extern kal_uint8 track_cust_module_get_rst();
#endif
/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/
void track_cust_decode_packet_17(kal_uint8 *data, kal_uint16 len);
kal_uint16 track_cust_check_gprsdata(kal_uint32 flag, kal_uint16 sn);
extern void track_cust_paket_94(kal_uint8 datatype, kal_uint8 *data, kal_uint16 len);
U16 track_cust_lock_pack_no(int no);
#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/


/******************************************************************************
 *  Function    -  getSerialNumber
 *
 *  Purpose     -  ȡ����ˮ��
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static kal_uint16 getSerialNumber()
{
    return g_SerialNumber++;
}

/******************************************************************************
 *  Function    -  DataPacketEncode78
 *
 *  Purpose     -  78Э����
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static kal_uint16 DataPacketEncode78(U8 cmd, U8 *sendData, U8 len)
{
    U16 tmp, tmp1;

    //��ʼλ
    sendData[0] = 0x78;
    sendData[1] = 0x78;

    //����
    sendData[2] = len - 5;

    //Э���
    sendData[3] = cmd;

    //��Ϣ���к�
    tmp = getSerialNumber();
    sendData[len - 6] = (tmp & 0xFF00) >> 8;
    sendData[len - 5] = (tmp & 0x00FF);
    LOGD(L_APP, L_V5, "SerialNumber:%.2X%.2X", sendData[len - 6], sendData[len - 5]);

    //CRCУ��
    tmp1 = GetCrc16(&sendData[2], len - 6);
    sendData[len - 4] = (tmp1 & 0xFF00) >> 8;
    sendData[len - 3] = (tmp1 & 0x00FF);

    //����
    sendData[len - 2] = '\r';
    sendData[len - 1] = '\n';

    return tmp;
}

/******************************************************************************
 *  Function    -  DataPacking
 *
 *  Purpose     -  79Э����
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 07-03-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static U16 DataPacketEncode79(U8 cmd, U8 *sendData, U16 len)
{
    U16 no, crc;

    //��ʼλ
    sendData[0] = 0x79;
    sendData[1] = 0x79;

    //����
    PUT_UINT16_BE(len-6, sendData, 2);

    //Э���
    sendData[4] = cmd;

    //��Ϣ���к�
    no = getSerialNumber();
    PUT_UINT16_BE(no, sendData, len - 6);
    LOGD(L_APP, L_V5, "SerialNumber:%.2X%.2X", sendData[len - 6], sendData[len - 5]);

    //CRCУ��
    crc = GetCrc16(&sendData[2], len - 6);
    PUT_UINT16_BE(crc, sendData, len - 4);

    //����
    sendData[len - 2] = '\r';
    sendData[len - 1] = '\n';

    return no;
}

/******************************************************************************
 *  Function    -  DataPackingDatetime_6
 *
 *  Purpose     -  �������ʱ���ֶ�
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void DataPackingDatetime_6(U8 *sendbuf, track_gps_data_struct *gd)
{
    applib_time_struct time = {0};
    applib_time_struct *currTime = NULL;
#if defined(__GT420D__)
    if (gd->gprmc.status == 0)
#else
    if (gd == NULL)
#endif
    {
        currTime = &time;
        if (track_cust_is_upload_UTC_time())
        {
            track_fun_get_time(currTime, KAL_TRUE, NULL);
        }
        else
        {
            track_fun_get_time(currTime, KAL_FALSE, &G_parameter.zone);
        }
    }
    else
    {
        currTime = (applib_time_struct *)&gd->gprmc.Date_Time;
    }
    LOGD(L_APP, L_V7, "time: %0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
         currTime->nYear,
         currTime->nMonth,
         currTime->nDay,
         currTime->nHour,
         currTime->nMin,
         currTime->nSec);
    sendbuf[0] = currTime->nYear - 2000;
    sendbuf[1] = currTime->nMonth;
    sendbuf[2] = currTime->nDay;
    sendbuf[3] = currTime->nHour;
    sendbuf[4] = currTime->nMin;
    sendbuf[5] = currTime->nSec;
}

/******************************************************************************
 *  Function    -  track_fun_LongitudeLatitudeToHex
 *
 *  Purpose     -  ת����γ�ȸ�ʽ
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_fun_LongitudeLatitudeToHex(kal_uint8 *dec, float value)
{
    kal_uint32 tmp;
    tmp = value * 1800000;
    track_fun_reverse_32(dec, tmp);
}

/******************************************************************************
 *  Function    -  track_fun_signed_LongitudeLatitudeToHex
 *
 *  Purpose     -  ת�������ŵľ�γ�ȸ�ʽ
 *
 *  Description -  ����Ϊ������γΪ��
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-03-10,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_fun_signed_LongitudeLatitudeToHex(kal_uint8 *dec, float value)
{
    kal_int32 signed_tmp,tmp;
    signed_tmp = value * 1000000;
    tmp=(kal_uint32)signed_tmp;
    track_fun_reverse_32(dec, tmp);
}


/******************************************************************************
 *  Function    -  DataPackingGps_12
 *
 *  Purpose     -  ���GPS�����ֶ�����
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void DataPackingGps_12(U8 *sendbuf, track_gps_data_struct *gd)
{
    kal_uint16 Course;
    kal_uint8 sat;

    LOGD(L_APP, L_V6, "status:%d,%d, sat:%d,%d, Latitude:%f%c, Longitude:%f%c, Speed:%f, Course:%f",
         gd->gprmc.status, gd->gpgga.posi_fix, gd->gpgga.sat, gd->bdgga.sat, gd->gprmc.Latitude, gd->gprmc.NS, gd->gprmc.Longitude, gd->gprmc.EW, gd->gprmc.Speed, gd->gprmc.Course);

    sat=gd->gpgga.sat + gd->bdgga.sat;
    if (sat>15)
    {
        sat=15;
    }
    sendbuf[0] = ((12 << 4) | sat);

    track_fun_LongitudeLatitudeToHex(&sendbuf[1], gd->gprmc.Latitude);
    track_fun_LongitudeLatitudeToHex(&sendbuf[5], gd->gprmc.Longitude);
    sendbuf[9] = gd->gprmc.Speed;
    Course = gd->gprmc.Course;
    track_fun_reverse_16(&sendbuf[10], Course);
    if (gd->gprmc.NS == 'N')
    {
        sendbuf[10] |= 1 << 2;
    }
    if (gd->gprmc.EW == 'W')
    {
        sendbuf[10] |= 1 << 3;
    }
    if (gd->gprmc.status == 1)  //��ǰ�Ƿ�λ
    {
        sendbuf[10] |= 1 << 4;
    }
    if (gd->gpgga.posi_fix == 2)
    {
        sendbuf[10] |= 1 << 5;
    }
#if defined (__VIRTUAL_ACC__)
    sendbuf[10] |= 1 << 6;
    if (track_cust_virtual_acc() == 1)
    {
        sendbuf[10] |= 1 << 7;
    }
#elif defined(__ACC_STAUTS__) || defined(__WM_OLD06__)||defined (__RTC_OLD_PROTOCOL__)
    sendbuf[10] |= 1 << 6;
    if (track_cust_status_acc() == 1)
    {
        sendbuf[10] |= 1 << 7;
    }
#endif /* __ACC_STAUTS__ */
}

/******************************************************************************
 *  Function    -  DataPackingLbs_8
 *
 *  Purpose     -  ���LBS�ֶ�����
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void DataPackingLbs_8(U8 *sendbuf, LBS_Multi_Cell_Data_Struct *lbs)
{
    //LOGD(L_APP, L_V6, "mcc:%d, mnc:%d, lac:%d, cell id:%d",
    //     lbs->MainCell_Info.mcc, lbs->MainCell_Info.mnc, lbs->MainCell_Info.lac, lbs->MainCell_Info.cell_id);
    track_fun_reverse_16(&sendbuf[0], lbs->MainCell_Info.mcc);
    sendbuf[2] = (U8)lbs->MainCell_Info.mnc;
    track_fun_reverse_16(&sendbuf[3], lbs->MainCell_Info.lac);
    track_fun_reverse_16(&sendbuf[6], lbs->MainCell_Info.cell_id);
}

/******************************************************************************
 *  Function    -  DataPackingLbs_46
 *
 *  Purpose     -  ���վ���ݴ��
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 08-08-2013,  jwj  written
 * ----------------------------------------
 ******************************************************************************/
static void DataPackingLbs_46(U8 *sendbuf, LBS_Multi_Cell_Data_Struct *lbs)
{
    char i = 0, num;
    LOGD(L_APP, L_V6, "mcc:%d, mnc:%d, lac:%d, cell id:%d",
         lbs->MainCell_Info.mcc, lbs->MainCell_Info.mnc, lbs->MainCell_Info.lac, lbs->MainCell_Info.cell_id);
    track_fun_reverse_16(&sendbuf[0], lbs->MainCell_Info.mcc);
    sendbuf[2] = (U8)lbs->MainCell_Info.mnc;
    track_fun_reverse_16(&sendbuf[3], lbs->MainCell_Info.lac);
    track_fun_reverse_16(&sendbuf[6], lbs->MainCell_Info.cell_id);
    sendbuf[8] = (U8)lbs->MainCell_Info.rxlev;
    num = 9;
    for (i = 0; i < 6; i++)
    {
        LOGD(L_APP, L_V6, "lac:%d, cell id:%d",lbs->NbrCell_Info[i].lac, lbs->NbrCell_Info[i].cell_id);
        track_fun_reverse_16(&sendbuf[num + i * 6], lbs->NbrCell_Info[i].lac);
        track_fun_reverse_16(&sendbuf[num + i * 6 + 3], lbs->NbrCell_Info[i].cell_id);
        sendbuf[num + i * 6 + 5] = (U8)lbs->NbrCell_Info[i].rxlev;
    }
    sendbuf[45] = lbs->ta;
}

static void DataPackingLbs_38(U8 *sendbuf, LBS_Multi_Cell_Data_Struct *lbs)
{
    char i = 0, num;
    //LOGD(L_APP, L_V6, "mcc:%d, mnc:%d, lac:%d, cell id:%d",
    //     lbs->MainCell_Info.mcc, lbs->MainCell_Info.mnc, lbs->MainCell_Info.lac, lbs->MainCell_Info.cell_id);
    track_fun_reverse_16(&sendbuf[0], lbs->MainCell_Info.mcc);
    sendbuf[2] = (U8)lbs->MainCell_Info.mnc;
    track_fun_reverse_16(&sendbuf[3], lbs->MainCell_Info.lac);
    track_fun_reverse_16(&sendbuf[5], lbs->MainCell_Info.cell_id);
    sendbuf[7] = (U8)lbs->MainCell_Info.rxlev;
    num = 8;
    for (i = 0; i < 6; i++)
    {
        track_fun_reverse_16(&sendbuf[num + i * 5], lbs->NbrCell_Info[i].lac);
        track_fun_reverse_16(&sendbuf[num + i * 5 + 2], lbs->NbrCell_Info[i].cell_id);
        sendbuf[num + i * 5 + 4] = (U8)lbs->NbrCell_Info[i].rxlev;
    }
}


#ifdef __GT420D__
static void DataPacking_Modtemp_hum10(U8 *sendbuf)
{
    kal_uint16 mod_num=0x0037,mod_num1=0x0038 ,value,value1;
    int i=0;
    track_fun_reverse_16(&sendbuf[0], mod_num);
    sendbuf[2]= 0x02;
    LOGD(L_APP, L_V5, "temp:%f",track_get_temp_form_mcu());
    value = track_get_temp_form_mcu();
    sendbuf[3] = (value >> 8);
    sendbuf[4] = (value & 0x00FF);//�¶�

    track_fun_reverse_16(&sendbuf[5], mod_num1);

    sendbuf[7]=0x02;
    LOGD(L_APP, L_V5, "humidity:%f", track_get_humidity_form_mcu());
    value1 = track_get_humidity_form_mcu();
    sendbuf[8] = (value1 >> 8);
    sendbuf[9] = (value1 & 0x00FF);//ʪ��
    LOGH(L_APP,L_V5,sendbuf,10);

}


static void DataPacking_ModIMEI11(U8 *sendbuf)
{
    kal_uint16 mod_num=0x0001;
    track_fun_reverse_16(&sendbuf[0], mod_num);
    sendbuf[2]=0x08;
    memcpy(&sendbuf[3], track_drv_get_imei(1), 8);


}

static void DataPacking_gsmverision5(U8 *sendbuf)
{

    kal_uint16 mod_num=0x0026;
    track_fun_reverse_16(&sendbuf[0], mod_num);
    sendbuf[2]=0x02;
    sendbuf[3]=0x01;
    sendbuf[4]= 0x01;
    LOGH(L_APP,L_V5,sendbuf,5);

}

static void DataPacking_status5(U8 *sendbuf)
{

    kal_uint16 mod_num=0x0019;
    track_fun_reverse_16(&sendbuf[0], mod_num);
    sendbuf[2]=0x02;
    sendbuf[3]=0x00;
    sendbuf[4]=track_cust_refer_charge_data(99);
    LOGH(L_APP,L_V5,sendbuf,5);

}

static void DataPacking_percent5(U8 *sendbuf)
{
    U8   v_percnt[] = {1,10,20,30,40,50,60,70,80,90,100};
    U32 v_level[] = {3550000 ,3600000, 3650000, 3680000, 3710000, 3740000,3800000,3850000,3950000,4000000,4100000};
    kal_uint16 mod_num=0x0018;
    kal_uint16 volt = 0;
    int i=0;
    track_fun_reverse_16(&sendbuf[0], mod_num);
    sendbuf[2]=0x02;
    volt = track_cust_MCUrefer_adc_data(99);
    sendbuf[3] = (volt >> 8);
    sendbuf[4] = (volt & 0x00FF);
#if 0
    for (i; i<9; i++)
    {
        if (v_level[i]<=volt<=v_level[i+1])
        {
            sendbuf[3]=0x00;
            track_fun_reverse_16(&sendbuf[4],v_percnt[i] );
            LOGH(L_APP,L_V5,sendbuf,5);
            break;
        }

    }
#endif
    LOGH(L_APP,L_V5,sendbuf,5);
}

static void DataPacking_humiturea_remalm(U8 *sendbuf)
{

    kal_uint16 mod_num=0x003D;
    track_fun_reverse_16(&sendbuf[0], mod_num);
    sendbuf[2]=0x01;
    if(G_parameter.humiture_sw == 0)
    {
        sendbuf[3] |= 0x01;
    }
    if(G_parameter.teardown_str.sw == 0)
    {
        sendbuf[3] |=  0x02;
        }
    LOGH(L_APP,L_V5,sendbuf,4);

}

static void DataPacking_CSQ4(U8 *sendbuf)
{

    kal_uint16 mod_num=0x0016;
    track_fun_reverse_16(&sendbuf[0], mod_num);
    sendbuf[2]=0x01;
    sendbuf[3]=track_cust_get_rssi(-1);
    LOGH(L_APP,L_V5,sendbuf,4);

}

static void DataPacking_MODE6(U8 *sendbuf)
{
    kal_uint16 value = 0;
    kal_uint16 mod_num=0x0039;
    track_fun_reverse_16(&sendbuf[0], mod_num);
    sendbuf[2]=0x03;
    if (G_parameter.extchip.mode == 1)
    {
        sendbuf[3]=0x01;
    }
    else if (G_parameter.extchip.mode == 2)
    {
        sendbuf[3]=0x00;
    }
    if (G_parameter.extchip.mode==1)
    {
        if (track_cust_gt420d_carstatus_data(99) == 2)
        {
            if (G_parameter.lastmode.mode_lasttime < 255)
            {
                sendbuf[4] = 0x00;
                sendbuf[5] = (kal_uint8)G_parameter.lastmode.mode_lasttime;
            }
            else
            {
                value = G_parameter.lastmode.mode_lasttime;
                sendbuf[4] = (value >> 8);
                sendbuf[5] = (value & 0x00FF);
            }
        }
        else if (track_cust_gt420d_carstatus_data(99) == 1)
        {
            if (G_parameter.extchip.mode1_par1 < 255)
            {
                sendbuf[4] = 0x00;
                sendbuf[5] = (kal_uint8)G_parameter.extchip.mode1_par1;
            }
            else
            {
                value = G_parameter.extchip.mode1_par1;
                sendbuf[4] = (value >> 8);
                sendbuf[5] = (value & 0x00FF);
            }
        }
    }
    //track_fun_reverse_16(&sendbuf[4],G_parameter.extchip.mode1_par2);
    else if (G_parameter.extchip.mode==2)
    {
        if (track_cust_gt420d_carstatus_data(99) == 2)
        {
            if (G_parameter.lastmode.mode_lasttime < 255)
            {
                sendbuf[4] = 0x00;
                sendbuf[5] = (kal_uint8)G_parameter.lastmode.mode_lasttime;
            }
            else
            {
                value = G_parameter.lastmode.mode_lasttime;
                sendbuf[4] = (value >> 8);
                sendbuf[5] = (value & 0x00FF);
            }
        }
        else if (track_cust_gt420d_carstatus_data(99) == 1)
        {
            if (G_parameter.extchip.mode2_par1 < 255)
            {
                sendbuf[4] = 0x00;
                sendbuf[5] = (kal_uint8)G_parameter.extchip.mode2_par1;
            }
            else
            {
                value = G_parameter.extchip.mode2_par1;
                sendbuf[4] = (value >> 8);
                sendbuf[5] = (value & 0x00FF);
            }
        }
    }
    //track_fun_reverse_16(&sendbuf[4],G_parameter.extchip.mode2_par2);
    LOGH(L_APP,L_V5,sendbuf,6);
}

static void DataPacking_sportstatus4(U8 *sendbuf)
{

    kal_uint16 mod_num=0x003A;
    track_fun_reverse_16(&sendbuf[0], mod_num);
    sendbuf[2]=0x01;
    if (track_cust_gt420d_carstatus_data(99) == 1)
    {
        LOGD(L_APP,L_V6,"�˶�");
        sendbuf[3] = 1;   //�˶�״̬����
    }
    else
    {
        LOGD(L_APP,L_V6,"��ֹ");
        sendbuf[3] = 0;   //�˶�״̬����
    }
    LOGH(L_APP,L_V5,sendbuf,4);

}



static void DataPacking_alarm4(U8 *sendbuf, kal_uint8 alarm_type)
{
    kal_uint16 mod_num=0x001F;
    track_fun_reverse_16(&sendbuf[0], mod_num);
    sendbuf[2]=0x01;
    sendbuf[3]=alarm_type;
    LOGH(L_APP,L_V5,sendbuf,4);

}
static void DataPacking_LBS12(U8 *sendbuf)
{
    LBS_Multi_Cell_Data_Struct *lbs;
    char i = 0, num;
    kal_uint16 mod_num=0x0012;
    track_fun_reverse_16(&sendbuf[0], mod_num);
    sendbuf[2]=0x24;
    num = 3;
    for (i = 0; i < 6; i++)
    {
        LOGD(L_APP, L_V6, "lac:%d, cell id:%d",lbs->NbrCell_Info[i].lac, lbs->NbrCell_Info[i].cell_id);
        track_fun_reverse_16(&sendbuf[num + i * 6], lbs->NbrCell_Info[i].lac);
        track_fun_reverse_16(&sendbuf[num + i * 6 + 3], lbs->NbrCell_Info[i].cell_id);
        sendbuf[num + i * 6 + 5] = (U8)lbs->NbrCell_Info[i].rxlev;
    }
    LOGH(L_APP,L_V5,sendbuf,42);

}

static void DataPacking_wifi12(U8 *sendbuf)
{

    kal_uint16 mod_num=0x0013;
    int i, index;
    track_fun_reverse_16(&sendbuf[0], mod_num);
    if (wifi_status.wifi_sum>6)
    {
        wifi_status.wifi_sum = 6;
        sendbuf[2]=wifi_status.wifi_sum*7;

    }
    else
    {
        sendbuf[2]=wifi_status.wifi_sum*7;
    }
    index = 3;

    for (i = 0; i < wifi_status.wifi_sum; i++)
    {
        //LOGD(L_APP, L_V5, "");;
        if (strstr(wifi_status.wifi_mac[i], "LAST"))
        {
            LOGD(L_APP, L_V6, "1");
            break;
        }
        else if (wifi_status.wifi_rssi[i] == 0)
        {
            LOGD(L_APP, L_V6, "2");
            continue;
        }
        else
        {
            LOGD(L_APP, L_V6, "3");
            if (strlen(wifi_status.wifi_mac[i]) > 0 && wifi_status.wifi_rssi[i] != 0)
            {
                memcpy(&sendbuf[index], track_fun_stringtohex(wifi_status.wifi_mac[i], 6), 6);
                index = index + 6;
                sendbuf[index] = wifi_status.wifi_rssi[i];
                index++;
            }
        }
    }

}

static void DataPacking_ModmainLbs_14(U8 *sendbuf, LBS_Multi_Cell_Data_Struct *lbs)
{
    kal_uint16 mod_num=0x0011;
    track_fun_reverse_16(&sendbuf[0], mod_num);
    sendbuf[2] = 0x0A;

    track_fun_reverse_16(&sendbuf[3], lbs->MainCell_Info.mcc);
    track_fun_reverse_16(&sendbuf[5], lbs->MainCell_Info.mnc);
    track_fun_reverse_16(&sendbuf[7], lbs->MainCell_Info.lac);
    track_fun_reverse_16(&sendbuf[10], lbs->MainCell_Info.cell_id);//����ǿ�ƶ���2λ  Э��3λ
    sendbuf[12]= (U8)lbs->MainCell_Info.rxlev;
}
#endif


kal_uint16 track_cust_upload_data_repacket(kal_uint8 head, kal_uint16 packet, kal_uint8 pid, kal_uint8 *data, kal_uint16 len)
{
    kal_uint16 no, tmp1;
    int size;
    kal_uint8 *sendData ;
    char tmp[300] = {0};


    LOGD(L_APP, L_V5, "len:%s", len);
    size = 8 + len + 6;
    sendData = (kal_uint8*)Ram_Alloc(1, size);

    if (sendData == NULL)
    {
        return 0;
    }
    memset(sendData, 0, size);

    //��ʼλ
    sendData[0] = head;
    sendData[1] = head;
    //����
    sendData[2] = ((len + 8) & 0xFF00) >> 8;
    sendData[3] = (len + 8) & 0x00FF;
    //Э���
    sendData[4] = pid;
    //�ܰ��ְ�
    sendData[5] = (packet & 0xFF00) >> 8;
    sendData[6] = packet & 0x00FF;
    sendData[7] = 0x00;
    //����
    memcpy(&sendData[8], data,  len);

    //��Ϣ���к�
    tmp1 = 0x43;//getSN();
    sendData[size - 6] = (tmp1 & 0xFF00) >> 8;
    sendData[size - 5] = (tmp1 & 0x00FF);
    LOGD(L_APP, L_V5, "SerialNumber:%.2X%.2X", sendData[size - 6], sendData[size - 5]);

    //CRCУ��
    tmp1 = GetCrc16(&sendData[2], size - 6);
    sendData[size - 4] = (tmp1 & 0xFF00) >> 8;
    sendData[size - 3] = (tmp1 & 0x00FF);

    //����
    sendData[size - 2] = '\r';
    sendData[size - 1] = '\n';
    LOGH(L_APP, L_V5, sendData, size);
    Socket_write_4G_req(0, sendData, size, 0, 1, no, 1);
    Ram_Free(sendData);
    return no;

}

/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/

/******************************************************************************
 *  Function    -  track_cust_paket_01
 *
 *  Purpose     -  Э�����ݰ���0x01������½��
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-10-2012,  Cml  written
 * v1.1  , 30-09-2013,  WangQi modify
 * ----------------------------------------
*******************************************************************************/
void track_cust_paket_01()
{
#if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}
#else
    kal_uint8 *sendData;
    kal_uint16 no, zone;
    int size;
#if defined(__GT03F__) || defined(__GT300__) || defined(__GW100__)||defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)//wangqi
    //���׿�Э��Ҫ�ϴ��������ʱ������
    size = 10 + 8 + 2 + 2;
#else
    if (track_cust_is_upload_UTC_time())
    {
        size = 10 + 8 + 2 + 2;
    }
    else
    {
        size = 10 + 8;
    }
#endif
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);

    // imei
    memcpy(&sendData[4], track_drv_get_imei(1), 8);
#if defined(__GT03F__) || defined(__GT300__) || defined(__GW100__)||defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)//wangqi
    //���׿�Э��Ҫ�ϴ��������ʱ������
#else
    if (track_cust_is_upload_UTC_time())
#endif
    {
        // ����ʶ����
#if defined(__GT03A__)  // �����͵繦��
        sendData[12] = 0x10;
        sendData[13] = 0x0A;
#elif defined(__GT03B__)  // �����͵繦��
        sendData[12] = 0x10;
        sendData[13] = 0x0B;
#elif defined(__GT06B__)  // �����͵繦��
        sendData[12] = 0x10;
        sendData[13] = 0x0E;
#elif defined(__NT32__)
        sendData[12] = 0x20;
        sendData[13] = 0x03;
#elif defined(__NT22__)
        sendData[12] = 0x22;
        sendData[13] = 0x03;
#elif defined(__NT31__)
        sendData[12] = 0x20;
        sendData[13] = 0x02;
#elif defined(__ET130__)//�����������
        sendData[12] = 0x13;
        sendData[13] = 0x00;
#elif defined(__NT33__)
        sendData[12] = 0x20;
        sendData[13] = 0x04;
#elif defined(__GT02D__)/*�����͵繦��*/|| defined(__BD300__) || defined(__GT06D__)||defined(__NT36__)
        sendData[12] = 0x20;
        sendData[13] = 0x00;
#elif defined(__ET100__)
        sendData[12] = 0x20;
        sendData[13] = 0x10;
#elif defined(__GT100__)// �����͵繦��
        sendData[12] = 0x20;
        sendData[13] = 0x11;
#elif defined(__GT03F__)
        sendData[12] = 0x20;
        sendData[13] = 0x20;
#elif defined(__GT800__)
        sendData[12] = 0x21;
        sendData[13] = 0x20;
#elif defined(__ET200__) // �����������
        sendData[12] = 0x22;
        sendData[13] = 0x00;
#elif defined(__NT23__)
        sendData[12] = 0x23;
        sendData[13] = 0x00;
#elif defined(__GT530__)
        sendData[12] = 0x53;
        sendData[13] = 0x00;
#elif defined(__GT500__)
        sendData[12] = 0x50;
        sendData[13] = 0x00;
#elif defined(__GT370__)
        sendData[12] = 0x70;
        sendData[13] = 0x03;
#elif defined(__GT380__)
        sendData[12] = 0x30;
        sendData[13] = 0x04;
#elif defined(__GT741__)
        sendData[12] = 0x70;
        sendData[13] = 0x09;
#elif defined(__GT420D__)
        sendData[12] = 0x80;
        sendData[13] = 0x0D;
#elif defined(__GT740__)
        if (G_importance_parameter_data.app_version.version==2)
        {
            sendData[12] = 0x70;
            sendData[13] = 0x06;
        }
        else  if (G_importance_parameter_data.app_version.version==3)
        {
            sendData[12] = 0x70;
            sendData[13] = 0x07;
        }
        else
        {
            sendData[12] = 0x70;
            sendData[13] = 0x04;
        }
#elif defined(__GW100__)
        sendData[12] = 0x21;
        sendData[13] = 0x01;
#elif defined(__V20__)
        sendData[12] = 0x21;
        sendData[13] = 0x01;
#elif defined(__ET210__)
        sendData[12] = 0x21;
        sendData[13] = 0x01;
#elif defined(__GT300S__)
        sendData[12] = 0x30;
        sendData[13] = 0x00;
#elif defined(__MT200__)
        sendData[12] = 0x22;
        sendData[13] = 0x05;
#elif defined(__NT36__)
        sendData[12] = 0x22;
        sendData[13] = 0x03;
#elif defined(__NT37__)
        sendData[12] = 0x20;
        sendData[13] = 0x08;
#elif defined(__ET320__)
        sendData[12] = 0x22;
        sendData[13] = 0x04;
#elif defined(__ET310__)
        sendData[12] = 0x22;
        sendData[13] = 0x03;
#elif defined(__ET350__)
        sendData[12] = 0x22;
        sendData[13] = 0x03;
#elif defined(__GT310__)
        sendData[12] = 0x30;
        sendData[13] = 0x01;
#elif defined(__JM81__)
        sendData[12] = 0x70;
        sendData[13] = 0x08;
#elif defined(__HVT001__)
        sendData[12] = 0x01;
        sendData[13] = 0x10;
#elif defined(__JM66__)
        sendData[12] = 0x30;
        sendData[13] = 0x07;
#else
#error "please define project_null"
#endif /* __ET100__ */

        // ʱ������
        zone = G_parameter.zone.time;
        zone *= 100;
        zone += G_parameter.zone.time_min;
        zone <<= 4;
        if (G_parameter.zone.zone == 'W')
        {
            zone |= (1 << 3);
        }

        if (G_parameter.lang == 1)
        {
            zone |= 0x02;
        }
        else
        {
            zone |= 0x01;
        }

        LOGD(L_APP, L_V5, "%x", zone);
        sendData[14] = (zone >> 8);
        sendData[15] = (zone & 0x00FF);
        LOGD(L_APP, L_V5, "%x,%x", sendData[14], sendData[15]);
    }

    no = DataPacketEncode78(0x01, sendData, size);
#if defined(__XCWS_TPATH__)
    Socket_write_req(2, sendData, size, 0, 1, 1, 1);
#else
    track_soc_write_req(sendData, size, 1, no);
#endif /* __XCWS_TPATH__ */
    Ram_Free(sendData);
#endif
}

//��־��Ϣ���AMS
int track_cust_paket_09(char* st_buf, U16 len)
{
    U8 *sendData, i = 0;
    U16 no;
    int size, ret;
    size = 1  + 2/*log len*/ + len;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "ERROR: Ram_Alloc");
        return;
    }
    memset(sendData, 0, size);
    //pack_type
    sendData[i++] = 0x09;
    /* ͸�����ݵĳ��� */
    sendData[i++] = (len & 0xFF00) >> 8;
    sendData[i++] = (len & 0x00FF);

    /* �����Ҫ͸�������� */
    memcpy(&sendData[i], st_buf, len);
    ret = track_cust_paket_FD(sendData, size, 0x09);
    Ram_Free(sendData);
    return ret;
}

/******************************************************************************
 *  Function    -  track_cust_paket_10
 *
 *  Purpose     -  Э�����ݰ���0x10����GPS��λ�����ϱ�
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-10-2012,  Jiangweijun  written
 * ----------------------------------------
*******************************************************************************/
kal_uint16 track_cust_paket_10(track_gps_data_struct *gd)
{
#if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}
#else
    U8 *sendData;
    U16 no;
    int size;

    size = 10 + 6/*datatime*/ + 12/*gps*/ + 2;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);

    // datetime
    DataPackingDatetime_6(&sendData[4], gd);

    // gps
    DataPackingGps_12(&sendData[10], gd);

    //����λ
    if (G_parameter.lang == 1)
    {
        sendData[23] = 0x01;
    }
    else
    {
        sendData[23] = 0x02;
    }

    no = DataPacketEncode78(0x10, sendData, size);

    track_socket_queue_position_send_reg(sendData, size, 0x10, no);
    Ram_Free(sendData);
    return no;
#endif /* __MQTT__ */
}

/******************************************************************************
 *  Function    -  track_cust_paket_11
 *
 *  Purpose     -  Э�����ݰ���0x11����LBS��λ����
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 07-30-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint16 track_cust_paket_11(LBS_Multi_Cell_Data_Struct *lbs)
{
    #if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData;
    U16 no;
    int size;

    size = 10 + 6/*datatime*/ + 8/*lbs*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "Ram_Alloc NULL!!");
        return;
    }
    memset(sendData, 0, size);

    // datetime
    DataPackingDatetime_6(&sendData[4], NULL);

    // lbs
    DataPackingLbs_8(&sendData[10], lbs);

    no = DataPacketEncode78(0x11, sendData, size);

    track_socket_queue_position_send_reg(sendData, size, 0x11, no);
    Ram_Free(sendData);

    return no;
    #endif
}

/******************************************************************************
 *  Function    -  track_cust_paket_12
 *
 *  Purpose     -  Э�����ݰ���0x12����GPS&LBS��λ�����ϱ�
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint16 track_cust_paket_12(track_gps_data_struct *gd, LBS_Multi_Cell_Data_Struct *lbs)
{
    #if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData;
    U16 no;
    int size;

    size = 10 + 6/*datatime*/ + 12/*gps*/ + 8/*lbs*/;
    if (G_parameter.mileage_statistics.sw)
    {
        size += 4;
    }
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "Ram_Alloc NULL!!");
        return;
    }
    memset(sendData, 0, size);

    // datetime
    DataPackingDatetime_6(&sendData[4], gd);

    // gps
    DataPackingGps_12(&sendData[10], gd);

    // lbs
    DataPackingLbs_8(&sendData[22], lbs);

    // ���ͳ��
    if (G_parameter.mileage_statistics.sw)
    {
        LOGD(L_APP, L_V1, "%d, %X", track_get_gps_mileage(), track_get_gps_mileage());
        track_fun_reverse_32(&sendData[30], track_get_gps_mileage());
    }

    no = DataPacketEncode78(0x12, sendData, size);
#if defined(__XCWS_TPATH__)
    track_cust_server3_queue_send_reg(sendData, size, 0x12, no);
#else
    track_socket_queue_position_send_reg(sendData, size, 0x12, no);
#endif
    Ram_Free(sendData);

    return no;
    #endif
}

/******************************************************************************
 *  Function    -  track_cust_paket_13
 *
 *  Purpose     -  Э�����ݰ���0x13����״̬��Ϣ\������
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_paket_13(kal_uint8* data, kal_uint16 *len, kal_uint16 *sn)
{
    #if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
#ifndef __XCWS__

    U8 *sendData;
    U8 tmp, i = 4;
    U16 no;
    int size, ret;

    LOGD(L_SOC, L_V5, "");
    size = 10 + 5/*status*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);

    /*
        Bit7       1���͵�Ͽ�     0���͵��ͨ
        Bit6       1��GPS�Ѷ�λ    0��GPSδ��λ
        Bit3~Bit5  100��SOS ��� 0x20
                   011���͵籨�� 0x18
                   010���ϵ籨�� 0x10
                   001���𶯱��� 0x08
                   000������
        Bit2       1���ѽӵ�Դ��� 0��δ�ӵ�Դ���
        Bit1       1��ACC��        0��ACC��
        Bit0       1�����         0������
    */
    //���/����
    if (track_cust_status_defences() == 2)
    {
        sendData[i] |= 0x01;
    }

    //acc
#if defined (__VIRTUAL_ACC__)
    if (track_cust_virtual_acc() == 1)
    {
        sendData[i] |= 0x02;
    }
#else
    if (track_cust_status_acc() == 1)
    {
        sendData[i] |= 0x02;
    }
#endif

    //�����
    if (track_cust_status_charger())
    {
        sendData[i] |= 0x04;
    }
    //��������

    //GPS��λ���
    if (track_cust_gps_status() >= GPS_STAUS_2D)
    {
        sendData[i] |= 0x40;
    }
    //�͵�״̬ �� �����״̬
    if (track_cust_oil_cut(99) || track_cust_motor_lock(99))
    {
        sendData[i] |= 0x80;
    }
    i++;

    //��ѹ�ȼ�
    sendData[i++] = track_cust_get_battery_level();

    /*GSM�ź�ǿ��
    0�����źţ�
    1���źż���
    2���źŽ���
    3���ź�����
    4���ź�ǿ*/
    ret = track_cust_get_rssi_level();
    if (ret >= 1 && ret <= 4)
    {
        sendData[i] = ret;
    }
    else
    {
        sendData[i] = 0;
    }
    i++;

    /*
        0x00������
        0x01��SOS ���
        0x02���ϵ籨��
        0x03: �𶯱���
        0x04: ��Χ������
        0x05: ��Χ������
        0x06: ���ٱ���
        0x07: λ�Ʊ���
    */
    i++;

    //���� 0x01: ���ģ�0x02: Ӣ��
    //sendData[i++] = language;
    if (G_parameter.lang == 1)
    {
        sendData[i++] = 1;
    }
    else
    {
        sendData[i++] = 2;
    }
    no = DataPacketEncode78(0x13, sendData, size);
    memcpy(data, sendData, size);
    *len = size;
    *sn = no;
    Ram_Free(sendData);
#endif
#endif
}

/******************************************************************************
 *  Function    -  track_cust_paket_15
 *
 *  Purpose     -  ��Ӧ�������·������ݰ�(��)
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_paket_15(kal_uint8 *serverid, kal_uint8 *outstr, kal_uint8 outstrlen)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData, tmp, i = 4;
    U16 no;
    int size;
#ifndef __XCWS__

    outstrlen = strlen(outstr);
    if (outstrlen <= 0 || outstrlen > 200)
    {
        LOGD(L_APP, L_V1, "ERROR: outstrlen=%d", outstrlen);
        return;
    }
    size = 10 + 1/*command len*/ + 4/*server id*/ + outstrlen/*content*/ + 2/*language*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);

    //ָ���
    sendData[i++] = outstrlen + 4;

    //��������־λ
    memcpy(&sendData[i], serverid, 4);
    i += 4;

    //ָ������
    memcpy(&sendData[i], outstr, outstrlen);
    i += outstrlen;

    //���� 0x01: ���ģ�0x02: Ӣ��
    sendData[i++] = 0;
    if (G_parameter.lang == 1)
    {
        sendData[i++] = 1;
    }
    else
    {
        sendData[i++] = 2;
    }
    no = DataPacketEncode78(0x15, sendData, size);

    track_socket_queue_send_netcmd_reg(sendData, size, 0x15, no);
#endif
    Ram_Free(sendData);
#endif
}

/******************************************************************************
 *  Function    -  track_cust_paket_16
 *
 *  Purpose     -  ����״̬��
 *
 *  Description -  language ��0x01: ���ģ�0x02: Ӣ��
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 25-10-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint16 track_cust_paket_16(
    track_gps_data_struct *gd,
    LBS_Multi_Cell_Data_Struct *lbs,
    enum_alarm_type alarm_type,
    kal_bool isReturn,
    kal_uint8 call_type)
{
    #if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData;
    U8 tmp, i = 4;
    U16 sn;
    int size, ret;
    kal_uint8 a_type = 0;
    kal_uint32  curr_tick = kal_get_systicks();

#ifndef __XCWS__

    LOGD(L_APP, L_V5, "alarm_type:0x%02X", alarm_type);
#if defined(__MORE_FENCE__)
    size = 10 + 6/*datatime*/ + 12/*gps*/ + 1/*lbs len*/ + 8/*lbs*/ + 5/*status & alarm*/ + 1/*����Χ�����к�*/;
#else
    size = 10 + 6/*datatime*/ + 12/*gps*/ + 1/*lbs len*/ + 8/*lbs*/ + 5/*status & alarm*/;
#endif /* __GT300__ */
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);

    // datetime
    DataPackingDatetime_6(&sendData[i], gd);
    i += 6;

    // gps
    DataPackingGps_12(&sendData[i], gd);
    i += 12;

    // lbs
    sendData[i++] = 9;
    DataPackingLbs_8(&sendData[i], lbs);
    i += 8;

    /*
        Bit7       1���͵�Ͽ�     0���͵��ͨ
        Bit6       1��GPS�Ѷ�λ    0��GPSδ��λ
        Bit3~Bit5  100��SOS ��� 0x20
                   011���͵籨�� 0x18
                   010���ϵ籨�� 0x10
                   001���𶯱��� 0x08
                   000������
        Bit2       1���ѽӵ�Դ��� 0��δ�ӵ�Դ���
        Bit1       1��ACC��        0��ACC��
        Bit0       1�����         0������
    */
    //���/����
    if (track_cust_status_defences() == 2)
    {
        sendData[i] |= 0x01;
    }
    //acc
#if defined (__VIRTUAL_ACC__)
    if (track_cust_virtual_acc() == 1)
    {
        sendData[i] |= 0x02;
    }
#else
    if (track_cust_status_acc() == 1)
    {
        sendData[i] |= 0x02;
    }
#endif

    //�����
    if (track_cust_status_charger())
    {
        sendData[i] |= 0x04;
    }
    //��������
    if (alarm_type == TR_CUST_ALARM_Vibration)
    {
        sendData[i] |= 0x08;
    }
    else if (alarm_type == TR_CUST_ALARM_PowerFailure)
    {
        sendData[i] |= 0x10;
    }
    else if (alarm_type == TR_CUST_ALARM_LowBattery)
    {
        sendData[i] |= 0x18;
    }
    else if (alarm_type == TR_CUST_ALARM_SOS)
    {
        sendData[i] |= 0x20;
    }
    //GPS��λ���
    if (gd->gprmc.status == 1)
    {
        sendData[i] |= 0x40;
    }
    //�͵�״̬
    if (track_cust_oil_cut(99))
    {
        sendData[i] |= 0x80;
    }
    i++;

    //��ѹ�ȼ�
    sendData[i++] = track_cust_get_battery_level();

    /*GSM�ź�ǿ��
    0�����źţ�
    1���źż���
    2���źŽ���
    3���ź�����
    4���ź�ǿ*/
    ret = track_cust_get_rssi_level();
    if (ret >= 1 && ret <= 4)
    {
        sendData[i++] = ret;
    }
    else
    {
        sendData[i++] = 0;
    }

    //��������
    if (alarm_type == TR_CUST_ALARM_ACC_ON)
        sendData[i++] = 0xFE;
    else if (alarm_type == TR_CUST_ALARM_ACC_OFF)
        sendData[i++] = 0xFF;
    else
        sendData[i++] = alarm_type;

    // �������ظ�������������
    if (isReturn)
    {
        if (G_parameter.lang == 1)
        {
            sendData[i++] = 1; //����
        }
        else
        {
            sendData[i++] = 2; //Ӣ��
        }
    }
    else
    {
        sendData[i++] = 0; //����Ҫƽ̨�ظ����ݲ�֧�֣�ԭ�е���ƣ��������б����лظ��ģ�
    }

#if defined(__MORE_FENCE__)
    if (alarm_type == TR_CUST_ALARM_OutFence || alarm_type == TR_CUST_ALARM_InFence)
    {
        sendData[i++] = track_cust_module_get_fence_id();
        a_type = G_parameter.fence[track_cust_module_get_fence_id()].alarm_type;//����Χ��ID ��ȡ������ʽ
        LOGD(L_APP, L_V5, "Χ��=%d,����ģʽ:%d", track_cust_module_get_fence_id() + 1, a_type);
    }
    else
    {
        sendData[i++] = 0xff;
    }
    sn = DataPacketEncode78(0x27, sendData, size);
    track_cust_check_gprsdata(1, sn);
    if (a_type == 1)
    {
        call_type |= 0x02;//sms 10
    }
    else if (a_type == 2)
    {
        call_type |= 0x03;//call&&sms 11
    }
    else if (a_type == 3)
    {
        call_type |= 0x01;//call 01
    }
    LOGD(L_APP, L_V5, "call_type:%d,fenceid=%d", call_type, track_cust_module_get_fence_id() + 1);
#else
    if (track_cust_is_upload_UTC_time())
    {
        sn = DataPacketEncode78(0x26, sendData, size);
    }
    else
    {
        sn = DataPacketEncode78(0x16, sendData, size);
    }
#endif /* __GT300__ */

#if defined(__XCWS_TPATH__)

    track_cust_server3_queue_send_reg(sendData, size, 0x16, sn);
#elif defined(__FLY_MODE__)
    if (alarm_type == TR_CUST_ALARM_Ext_LowBattery_Fly)
    {
        /* ���ڽ�����ģʽ֮ǰһ��ᷢ�����͵籨��������ôΪ�˲��򱨾����ж�����ɽ������ģʽ����δ�ͳ����� */
        track_socket_queue_position_send_reg(sendData, size, 0x16, sn);
    }
    else
    {
        if (isReturn)
        {
            track_alarm_vec_queue_push(alarm_type, sn, call_type);
        }
        track_socket_queue_alarm_send_reg(sendData, size, 0x16, sn);
    }
#elif defined(__MORE_FENCE__)
    if (isReturn || a_type)
    {
        track_alarm_vec_queue_push(alarm_type, sn, call_type);
    }

    track_socket_queue_alarm_send_reg(sendData, size, 0x16, sn);
#else
    if (isReturn)
    {
        track_alarm_vec_queue_push(alarm_type, sn, call_type);
    }

    track_socket_queue_alarm_send_reg(sendData, size, 0x16, sn);
#endif
    Ram_Free(sendData);
#endif
#if defined(__SERVER_IP_LINK__)
    if (!track_soc_login_status())
    {
        track_soc_gprs_reconnect((void*)310);
    }
    else if (track_is_timer_run(TRACK_CUST_LINK_DOWN_TIMER_ID))
    {
        tr_stop_timer(TRACK_CUST_LINK_DOWN_TIMER_ID);
    }
#endif

    return sn;
#endif
}

/******************************************************************************
 *  Function    -  track_cust_paket_17
 *
 *  Purpose     -   LBS���绰�����ѯ��ַ��Ϣ��(0X17)
 *
 *  Description -  DW��Positoin ��ȡ LBS λ����Ϣ
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 30-07-2013,  Cml  written
 * v1.0  , 14-10-2013,  wangqi  written
 * ----------------------------------------
*******************************************************************************/
kal_uint16 track_cust_paket_17(LBS_Multi_Cell_Data_Struct *lbs, char *number)
{
    #if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    kal_uint8 *sendData;
    kal_uint16 no;
    int size;
#ifndef __XCWS__

    LOGD(L_APP, L_V5, "number:%s", number);
    size = 10 + 8/*lbs*/ + 21/*number*/ + 2/*hold ����λ,����*/;

    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return 0;
    }
    memset(sendData, 0, size);

    // lbs
    DataPackingLbs_8(&sendData[4], lbs);

    // number(Max:21)
    snprintf(&sendData[12], 20, number);

    //hold ����λ,����
    sendData[33] = 0;
    if (G_parameter.lang == 1)
    {
        sendData[34] = 1;
    }
    else
    {
        sendData[34] = 2;
    }

    no = DataPacketEncode78(0x17, sendData, size);
    track_socket_queue_alarm_send_reg(sendData, size, 0x17, no);
    Ram_Free(sendData);
#endif
    return no;
#endif
}


/******************************************************************************
 *  Function    -  track_cust_paket_2E
 *
 *  Purpose     -  ���վЭ�� 0X2E   LBS���Ű�
 *
 *  Description -   ��ظ�
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 29-08-2016,  Xzq  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint16 track_cust_paket_2E(LBS_Multi_Cell_Data_Struct *lbs)
{
    #if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData;
    U16 no;
    int size;
    LOGD(L_APP, L_V5, "");
    size = 10 + 6/*datatime*/ + 46/*lbs*/ + 2;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "Ram_Alloc NULL!!");
        return;
    }
    memset(sendData, 0, size);
    // datetime
    DataPackingDatetime_6(&sendData[4], NULL);
    // lbs
    DataPackingLbs_46(&sendData[10], lbs);
    sendData[56] = 0;
    //����λ
    if (G_parameter.lang == 1)
    {
        sendData[57] = 0x01;
    }
    else
    {
        sendData[57] = 0x02;
    }
    LOGH(L_APP, L_V7, sendData, size);
    no = DataPacketEncode78(0x2E, sendData, size);
    //track_socket_queue_send_log_reg(sendData, size, 0x28, no);
    track_socket_queue_position_send_reg(sendData, size, 0x2E, no);
    Ram_Free(sendData);

    return no;
    #endif
}

/******************************************************************************
 *  Function    -  track_cust_paket_18
 *
 *  Purpose     -  ���վЭ�� 0x18/0x28
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 08-08-2013,  jwj  written
 * ----------------------------------------
 ******************************************************************************/
kal_int8 track_cust_paket_18(LBS_Multi_Cell_Data_Struct *lbs, kal_uint8 type)
{
    #if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData;
    S8 ret = 0;
    U16 no;
    int size;
    applib_time_struct currTime = {0};
#if defined(__GT740__)||defined(__GT420D__)
    if (!G_parameter.lbs_sw)return;
#endif
    LOGD(L_APP, L_V5, "");
#ifndef __XCWS__
#if defined (__300S_WIFI_TEST__)
    LOGD(L_APP, L_V5, "2C@1");

    track_cust_sendWIFI_paket();
    no = 3;
#endif /* __300S_WIFI_TEST__ */


#if defined (__GT370__) || defined (__GT380__)
    track_fun_get_time(&currTime, KAL_FALSE, &G_parameter.zone);
    if (currTime.nYear == (*OTA_DEFAULT_HARDWARE_YEAR + 2000))
    {
        LOGD(L_APP, L_V1, "ϵͳʱ��δ����!!");
        if (!track_is_timer_run(TRACK_CUST_SNED_LBS_NO_TIME_DELAY_TIMER_ID))
        {
            tr_start_timer(TRACK_CUST_SNED_LBS_NO_TIME_DELAY_TIMER_ID, 20*1000, track_send_lbs_limit_rtc);
        }
        return;
    }
    else
    {
        if (track_is_timer_run(TRACK_CUST_SNED_LBS_NO_TIME_DELAY_TIMER_ID))
        {
            tr_stop_timer(TRACK_CUST_SNED_LBS_NO_TIME_DELAY_TIMER_ID);
        }
    }


#endif

    size = 10 + 6/*datatime*/ + 46/*lbs*/ + 2;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "Ram_Alloc NULL!!");
        return;
    }

    // datetime
    DataPackingDatetime_6(&sendData[4], NULL);

    // lbs
    DataPackingLbs_46(&sendData[10], lbs);
    sendData[56] = 0;
    //����λ
    if (G_parameter.lang == 1)
    {
        sendData[57] = 0x01;
    }
    else
    {
        sendData[57] = 0x02;
    }
    if (track_cust_is_upload_UTC_time())
    {
        LOGH(L_APP, L_V7, sendData, size);
        no = DataPacketEncode78(0x28, sendData, size);
        if (type == 1)
            ret = Socket_write_req(0, sendData, size, PACKET_LBS_DATA, 0x13, no, 1);
        else
            track_socket_queue_position_send_reg(sendData, size, 0x28, no);
    }
    else
    {
        no = DataPacketEncode78(0x18, sendData, size);
        if (type == 1)
            ret = Socket_write_req(0, sendData, size, PACKET_LBS_DATA, 0x13, no, 1);
        else
            track_socket_queue_position_send_reg(sendData, size, 0x18, no);
    }

    Ram_Free(sendData);
#endif
    return ret;
#endif
}


/******************************************************************************
 *  Function    -  track_cust_paket_19
 *
 *  Purpose     -  SOS���𶯱���ʱ��ȡ LBS ��Ӧλ����Ϣ
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 30-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint16 track_cust_paket_19(LBS_Multi_Cell_Data_Struct *lbs, enum_alarm_type alarm_type, kal_bool isReturn, kal_uint8 call_type)
{
    #if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    kal_uint8 *sendData;
    kal_uint16 sn;
    int size, i, ret;
#ifndef __XCWS__
    LOGD(L_APP, L_V5, "");
    size = 10 + 8/*lbs*/ + 3/*status*/ + 2/*��չλ*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return 0;
    }
    memset(sendData, 0, size);

    // lbs
    DataPackingLbs_8(&sendData[4], lbs);
    i = 12;

    /*
        Bit7       1���͵�Ͽ�     0���͵��ͨ
        Bit6       1��GPS�Ѷ�λ    0��GPSδ��λ
        Bit3~Bit5  100��SOS ��� 0x20
                   011���͵籨�� 0x18
                   010���ϵ籨�� 0x10
                   001���𶯱��� 0x08
                   000������
        Bit2       1���ѽӵ�Դ��� 0��δ�ӵ�Դ���
        Bit1       1��ACC��        0��ACC��
        Bit0       1�����         0������
    */
    if (track_cust_status_charger() > 0)
    {
        sendData[i] = 0x04;
    }

    if (track_cust_status_defences() == 2)
    {
        sendData[i] |= 0x01;//0000 0001
    }

    if (G_parameter.fence[0].sw == 1)
    {
        sendData[i] |= 0x02;//0000 0010
    }

    if (alarm_type == TR_CUST_ALARM_SOS)
    {
        sendData[i] |= 0x20;
    }
    else if (alarm_type == TR_CUST_ALARM_PowerFailure)
    {
        sendData[i] |= 0x10;
    }
    else if (alarm_type == TR_CUST_ALARM_Vibration) //�𶯱���001//0000 1000
    {
        sendData[i] |= 0x08;//0000 1000
    }
    else if (alarm_type == TR_CUST_ALARM_LowBattery)
    {
        sendData[i] |= 0x18;//0001 1000
    }

    //GPS��λ״̬
    if (track_cust_gps_status() > 2)
    {
        sendData[i] |= 0x40;
    }
    i++;

    //��ѹ�ȼ�
    sendData[i++] = track_cust_get_battery_level();

    /*GSM�ź�ǿ��
    0�����źţ�
    1���źż���
    2���źŽ���
    3���ź�����
    4���ź�ǿ*/
    ret = track_cust_get_rssi_level();
    if (ret >= 1 && ret <= 4)
    {
        sendData[i++] = ret;
    }
    else
    {
        sendData[i++] = 0;
    }

    //��������
    if (alarm_type == TR_CUST_ALARM_ACC_ON)
        sendData[i++] = 0xFE;
    else if (alarm_type == TR_CUST_ALARM_ACC_OFF)
        sendData[i++] = 0xFF;
    else
        sendData[i++] = alarm_type;

    if (G_parameter.lang == 1)
    {
        sendData[i++] = 1; //����
    }
    else
    {
        sendData[i++] = 2; //Ӣ��
    }

    sn = DataPacketEncode78(0x19, sendData, size);

    if (isReturn)
    {
        track_alarm_vec_queue_push(alarm_type, sn, call_type);
    }
#if defined(__GT740__)
    if (alarm_type==TR_CUST_ALARM_LINKOUT_ALM)
    {
        Socket_write_req(0, sendData, size, 0, 0x16, sn, 1);
    }
    else
    {
        track_socket_queue_alarm_send_reg(sendData, size, 0x19, sn);
    }
#else
    track_socket_queue_alarm_send_reg(sendData, size, 0x19, sn);

#endif
    Ram_Free(sendData);
#endif
    return sn;
#endif
}

/******************************************************************************
 *  Function    -  track_cust_paket_1A
 *
 *  Purpose     -  ������������ѯ��γ�ȶ�Ӧ�����ĵ�ַ
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint16 track_cust_paket_1A(track_gps_data_struct *gd, char *number)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    kal_uint8 *sendData;
    kal_uint16 no;
    int size;
#ifndef __XCWS__

    LOGD(L_APP, L_V5, "number:%s", number);
    size = 10 + 6/*datatime*/ + 12/*gps*/ + 21/*number*/ + 2/*language*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return 0;
    }
    memset(sendData, 0, size);

    // datetime
    DataPackingDatetime_6(&sendData[4], gd);

    // gps
    DataPackingGps_12(&sendData[10], gd);

    // number(Max:21)
    snprintf(&sendData[22], 20, number);

    // language  0x01: ���ģ�0x02: Ӣ��
    sendData[43] = 0x00;
    if (G_parameter.lang == 1)
    {
        sendData[44] = 1;
    }
    else
    {
        sendData[44] = 2;
    }
    if (track_cust_is_upload_UTC_time())
    {
        no = DataPacketEncode78(0x2A, sendData, size);
    }
    else
    {
        no = DataPacketEncode78(0x1A, sendData, size);
    }
    track_socket_queue_alarm_send_reg(sendData, size, 0x1A, no);
    Ram_Free(sendData);
#endif
    return no;
#endif
}

/******************************************************************************
 *  Function    -  track_cust_paket_21
 *
 *  Purpose     -  ��Ӧ�������·������ݰ�
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_paket_21(kal_uint8 *serverid, kal_bool isASCII, kal_uint8 *data, int datalen)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData, i = 5;
    U16 no;
    int size;

    if (datalen <= 0 || datalen > 1000)
    {
        LOGD(L_APP, L_V1, "ERROR: outstrlen=%d", datalen);
        return;
    }

#if defined(  __GT03F__)
    if (datalen > 0xff)
    {
        LOGD(L_APP, L_V1, "Warm: GT03D 0x81 cant support this length = %d !!", datalen);
        datalen = 0xff;
    }
    size = 10 + 1/*packetlen*/ + 4/*server id*/ + 2/*data len*/ + datalen/*data*/;
    sendData = (U8*)Ram_Alloc(5, size);
    i = 4;
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "ERROR: Ram_Alloc");
        return;
    }
    memset(sendData, 0, size);
    sendData[i++] = (kal_uint8)datalen + 4;//ָ���
    //��������־λ
    memcpy(&sendData[i], serverid, 4);
    i += 4;

    //����
    memcpy(&sendData[i], data, datalen);
    i += datalen;

    sendData[i++] = 0;//����λ
    sendData[i] = (G_parameter.lang == 1 ? 1 : 2);//����λ

    no = DataPacketEncode78(0x81, sendData, size);
    track_socket_queue_send_netcmd_reg(sendData, size, 0x81, no);

#else
    size = 11 + 4/*server id*/ + 1/*code_page*/ + datalen/*data*/;

    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "ERROR: Ram_Alloc");
        return;
    }
    memset(sendData, 0, size);

    //��������־λ
    memcpy(&sendData[i], serverid, 4);
    i += 4;

    //code_page
    if (isASCII)
    {
        sendData[i++] = 1;
    }
    else
    {
        sendData[i++] = 2;
    }

    //����
    memcpy(&sendData[i], data, datalen);
    i += datalen;

    no = DataPacketEncode79(0x21, sendData, size);
    track_socket_queue_send_netcmd_reg(sendData, size, 0x21, no);
#endif /* __GT03F__ */
    Ram_Free(sendData);
#endif
}

/******************************************************************************
 *  Function    -  track_cust_paket_22
 *
 *  Purpose     -  GPS&LBS��λ�����ϱ�(UTCʱ��)
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint16 track_cust_paket_22(
    track_gps_data_struct *gd,
    LBS_Multi_Cell_Data_Struct *lbs,
    kal_uint8 packet_type,
    kal_uint8 packet_status)
{
#if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData;
    U16 no;
    int size;
#ifndef __XCWS__
#if defined(__BCA__)
    kal_int32 mileage = track_get_gps_mileage();
    if (G_parameter.mileage_statistics.sw)//��̹��ܿ���Ӱ�춨λ���Ƿ�����ֵ
    {
        size = 10 + 6/*datatime*/ + 12/*gps*/ + 8/*lbs*/ + 3 + 4/*mileage*/;
    }
    else
#endif
    {
        // ע��size��ֵ���ܴ��� BACKUP_DATA_PACKET_MAX �궨��
        size = 10 + 6/*datatime*/ + 12/*gps*/ + 8/*lbs*/ + 3;
    }
    if (G_parameter.mileage_statistics.sw)
    {
        size += 4;
    }
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "Ram_Alloc NULL!!");
        return;
    }
    memset(sendData, 0, size);

    // datetime
    DataPackingDatetime_6(&sendData[4], gd);

    // gps
    DataPackingGps_12(&sendData[10], gd);

    // lbs
    DataPackingLbs_8(&sendData[22], lbs);

    // ACC
#if defined (__VIRTUAL_ACC__)
    if (track_cust_virtual_acc() == 1)
    {
        sendData[30] = 1;
    }
#else
    if (track_cust_status_acc() == 1)
    {
        sendData[30] = 1;
    }
#endif

    // GPS ���ݵ��ϱ�����
    // 0x00 ��ʱ�ϱ�
    // 0x01 �����ϱ�
    // 0x02 �յ��ϴ�
    // 0x03 ACC ״̬�ı��ϴ�
    // 0x04 ���˶���Ϊ��ֹ״̬�󣬲������һ����λ��
    // 0x05 ����Ͽ��������ϱ�֮ǰ���һ����Ч�ϴ���
    sendData[31] = packet_type;

    // GPS ʵʱ����
    // 0x00 ʵʱ�ϴ�
    // 0x01 ����
    sendData[32] = packet_status;
    if (G_parameter.mileage_statistics.sw)
    {
        LOGD(L_APP, L_V1, "%d, %X", track_get_gps_mileage(), track_get_gps_mileage());
        track_fun_reverse_32(&sendData[33], track_get_gps_mileage());
    }
    no = DataPacketEncode78(0x22, sendData, size);
#if defined(__XCWS_TPATH__)
    track_cust_server3_queue_send_reg(sendData, size, 0x12, no);
#else
    track_socket_queue_position_send_reg(sendData, size, 0x12, no);
#endif
    Ram_Free(sendData);
#endif
#if defined(__SERVER_IP_LINK__)
    if (!track_soc_login_status())
    {
        track_soc_gprs_reconnect((void*)310);
    }
    else if (track_is_timer_run(TRACK_CUST_LINK_DOWN_TIMER_ID))
    {
        tr_stop_timer(TRACK_CUST_LINK_DOWN_TIMER_ID);
    }
#endif

    return no;
#endif
}

/******************************************************************************
 *  Function    -  track_cust_paket_37
 *
 *  Purpose     -  22��+��չģ������
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 2019-05-7,    written
 * ----------------------------------------
*******************************************************************************/
kal_uint16 track_cust_paket_37(
    track_gps_data_struct *gd,
    LBS_Multi_Cell_Data_Struct *lbs,
    kal_uint8 packet_type,
    kal_uint8 packet_status,
    kal_uint8 upload_type,
    kal_uint8 alarm_type)
{
    #if defined (__MQTT__)
{
    return;
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData = NULL;
    U16 no;
    int size = 0;
#ifndef __XCWS__
#if defined(__GT420D__)
    if (upload_type == 2 ||upload_type == 3||upload_type == 4||upload_type == 5)
    {
        gd = track_cust_backup_gps_data(2, NULL);
        gd->gprmc.status = 0;
    }
#endif
#if defined(__BCA__)
    kal_int32 mileage = track_get_gps_mileage();
    if (G_parameter.mileage_statistics.sw)//��̹��ܿ���Ӱ�춨λ���Ƿ�����ֵ
    {
        size = 10 + 6/*datatime*/ + 12/*gps*/ + 8/*lbs*/ + 3 + 4/*mileage*/;
    }
    else
#endif
    {
        // ע��size��ֵ���ܴ��� BACKUP_DATA_PACKET_MAX �궨��
        if (alarm_type != 0)
        {
            size = 10 + 6/*datatime*/ + 12/*gps*/ + 8/*lbs*/ +3+11+5+5+5+4+6+4+10+4+4;
        }
        else
        {
            size = 10 + 6/*datatime*/ + 12/*gps*/ + 8/*lbs*/ +3+11+5+5+5+4+6+4+10+4;
        }
        if (G_parameter.humiture_sw == 0)
        {
            size = size -10;
        }
    }
    if (upload_type == 3)//(wifi_status.wifi_sum <= 1&&gd->gprmc.status != 1)
    {
        LOGD(L_APP,L_V6,"1");
        size += 13/*MAIN LBS*/ + 39/*LBS*/;
    }
    else if (upload_type == 2)//(wifi_status.wifi_sum>0&&gd->gprmc.status != 1)
    {
        LOGD(L_APP,L_V6,"2");
        if (wifi_status.wifi_sum>6)
        {
            wifi_status.wifi_sum = 6;
        }
        size += 7 * wifi_status.wifi_sum  + 3 + 13/*LBS����վ*/ ;
    }
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "Ram_Alloc NULL!!");
        return;
    }
    memset(sendData, 0, size);
    // datetime
    DataPackingDatetime_6(&sendData[4], gd);


    // gps
    DataPackingGps_12(&sendData[10], gd);

    // lbs
    DataPackingLbs_8(&sendData[22], lbs);

    // ACC
    if (track_cust_status_acc() == 1)
    {
        sendData[30] = 1;
    }

    // GPS ���ݵ��ϱ�����
    // 0x00 ��ʱ�ϱ�
    // 0x01 �����ϱ�
    // 0x02 �յ��ϴ�
    // 0x03 ACC ״̬�ı��ϴ�
    // 0x04 ���˶���Ϊ��ֹ״̬�󣬲������һ����λ��
    // 0x05 ����Ͽ��������ϱ�֮ǰ���һ����Ч�ϴ���
    sendData[31] = packet_type;

    // GPS ʵʱ����
    // 0x00 ʵʱ�ϴ�
    // 0x01 ����
    sendData[32] = packet_status;

    DataPacking_ModIMEI11(&sendData[33]);//11
    DataPacking_gsmverision5(&sendData[44]);//5
    DataPacking_status5(&sendData[49]);//5
    DataPacking_percent5(&sendData[54]);//5
    DataPacking_CSQ4(&sendData[59]);//4
    DataPacking_MODE6(&sendData[63]);//6
    DataPacking_sportstatus4(&sendData[69]);//4
    DataPacking_humiturea_remalm(&sendData[73]);//4
    if (G_parameter.humiture_sw == 1)
    {
        DataPacking_Modtemp_hum10(&sendData[77]);//10
    }
    if (alarm_type != 0)
    {
        if (G_parameter.humiture_sw == 1)
        {
            DataPacking_alarm4(&sendData[87],alarm_type);
        }
        else
        {
            DataPacking_alarm4(&sendData[77],alarm_type);
        }
        if (gd->gprmc.status == 1)  //��ǰ�Ƿ�λ
        {
            LOGD(L_APP, L_V1, "GPS��λ����LBS��WIFI");
        }
        else if (upload_type == 3)
        {
            if (G_parameter.humiture_sw == 1)
            {
                DataPacking_ModmainLbs_14(&sendData[91],track_drv_get_lbs_data());
                DataPacking_LBS12(&sendData[104]);
            }
            else
            {
                DataPacking_ModmainLbs_14(&sendData[81],track_drv_get_lbs_data());
                DataPacking_LBS12(&sendData[94]);
            }
        }
        else if (upload_type == 2)
        {
            if (G_parameter.humiture_sw == 1)
            {
                DataPacking_wifi12(&sendData[91]);
                DataPacking_ModmainLbs_14(&sendData[91+7 * wifi_status.wifi_sum + 3],track_drv_get_lbs_data());
            }
            else
            {
                DataPacking_wifi12(&sendData[81]);
                DataPacking_ModmainLbs_14(&sendData[81+7 * wifi_status.wifi_sum + 3],track_drv_get_lbs_data());
            }
        }
    }
    else
    {
        if (gd->gprmc.status == 1)  //��ǰ�Ƿ�λ
        {
            LOGD(L_APP, L_V1, "GPS��λ����LBS��WIFI");

        }
        else if (upload_type == 3)
        {
            LOGD(L_APP, L_V5, "��LBS");
            if (G_parameter.humiture_sw == 1)
            {
                DataPacking_ModmainLbs_14(&sendData[87],track_drv_get_lbs_data());
                DataPacking_LBS12(&sendData[100]);
            }
            else
            {
                DataPacking_ModmainLbs_14(&sendData[77],track_drv_get_lbs_data());
                DataPacking_LBS12(&sendData[90]);
            }
        }
        else if (upload_type == 2)
        {
            LOGD(L_APP, L_V5, "��WIFI");
            if (G_parameter.humiture_sw == 1)
            {
                DataPacking_wifi12(&sendData[87]);
                DataPacking_ModmainLbs_14(&sendData[87+7 * wifi_status.wifi_sum + 3],track_drv_get_lbs_data());
            }
            else
            {
                DataPacking_wifi12(&sendData[77]);
                DataPacking_ModmainLbs_14(&sendData[77+7 * wifi_status.wifi_sum + 3],track_drv_get_lbs_data());
            }
        }
    }
    no = DataPacketEncode78(0x37, sendData, size);
    LOGD(L_APP,L_V5,"size:%d",size);
#if defined(__XCWS_TPATH__)
    track_cust_server3_queue_send_reg(sendData, size, 0x12, no);
#else
    track_socket_queue_position_send_reg(sendData, size, 0x12, no);
#endif
    Ram_Free(sendData);
#endif
#if defined(__SERVER_IP_LINK__)
    if (!track_soc_login_status())
    {
        track_soc_gprs_reconnect((void*)310);
    }
    else if (track_is_timer_run(TRACK_CUST_LINK_DOWN_TIMER_ID))
    {
        tr_stop_timer(TRACK_CUST_LINK_DOWN_TIMER_ID);
    }
#endif

    return no;
#endif
}

kal_uint8 track_cust_gt420d_send_wifiorlbs(kal_uint8 arg,kal_uint8 packet_type)
{
    extern LBS_Multi_Cell_Data_Struct * track_drv_get_lbs_data(void);
    track_gps_data_struct *p_gpsdata = track_cust_backup_gps_data(2, NULL);
    p_gpsdata->gprmc.status = 0;
    track_cust_paket_37(p_gpsdata, track_drv_get_lbs_data(), packet_type, 0,arg,0);
}

/******************************************************************************
 *  Function    -  track_cust_paket_2D
 *
 *  Purpose     -  GPS&LBS��λ�����ϱ�(UTCʱ��)
 *
 *  Description -  ��ƽ̨�ظ�
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-08-2016,  XZQ  written
 * ----------------------------------------
*******************************************************************************/
kal_uint16 track_cust_paket_2D(
    track_gps_data_struct *gd,
    LBS_Multi_Cell_Data_Struct *lbs,
    kal_uint8 packet_type,
    kal_uint8 packet_status)
{
    #if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData;
    U16 no;
    int size;
#if defined(__BCA__)
    kal_int32 mileage = track_get_gps_mileage();
    if (G_parameter.mileage_statistics.sw)//��̹��ܿ���Ӱ�춨λ���Ƿ�����ֵ
    {
        size = 10 + 6/*datatime*/ + 12/*gps*/ + 8/*lbs*/ + 3 + 4/*mileage*/;
    }
    else
#endif
    {
        // ע��size��ֵ���ܴ��� BACKUP_DATA_PACKET_MAX �궨��
        size = 10 + 6/*datatime*/ + 12/*gps*/ + 8/*lbs*/ + 3;
    }
    if (G_parameter.mileage_statistics.sw)
    {
        size += 4;
    }
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "Ram_Alloc NULL!!");
        return;
    }
    memset(sendData, 0, size);

    // datetime
    DataPackingDatetime_6(&sendData[4], gd);

    // gps
    DataPackingGps_12(&sendData[10], gd);

    // lbs
    DataPackingLbs_8(&sendData[22], lbs);

    // ACC
#if defined (__VIRTUAL_ACC__)
    if (track_cust_virtual_acc() == 1)
    {
        sendData[30] = 1;
    }
#else
    if (track_cust_status_acc() == 1)
    {
        sendData[30] = 1;
    }
#endif

    // GPS ���ݵ��ϱ�����
    // 0x00 ��ʱ�ϱ�
    // 0x01 �����ϱ�
    // 0x02 �յ��ϴ�
    // 0x03 ACC ״̬�ı��ϴ�
    // 0x04 ���˶���Ϊ��ֹ״̬�󣬲������һ����λ��
    // 0x05 ����Ͽ��������ϱ�֮ǰ���һ����Ч�ϴ���
    sendData[31] = packet_type;

    // GPS ʵʱ����
    // 0x00 ʵʱ�ϴ�
    // 0x01 ����
    sendData[32] = packet_status;
    if (G_parameter.mileage_statistics.sw)
    {
        LOGD(L_APP, L_V1, "%d, %X", track_get_gps_mileage(), track_get_gps_mileage());
        track_fun_reverse_32(&sendData[33], track_get_gps_mileage());
    }
    no = DataPacketEncode78(0x2D, sendData, size);
#if defined(__CQYD_2SERVER__)
    track_cust_server3_queue_send_reg(sendData, size, 0x12, no);
#else
    track_socket_queue_position_send_reg(sendData, size, 0x2D, no);
#endif
    Ram_Free(sendData);

    return no;
    #endif
}

void track_cust_paket_23(kal_uint8* data, kal_uint16 *len, kal_uint16 *sn)
{
    #if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData;
    U8 tmp, i = 4;
    U16 no, volt2;
    U32 volt;
    int size, ret;
#ifndef __XCWS__

    LOGD(L_SOC, L_V5, "");
    size = 10 + 6/*status*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);

    /*
        Bit7       1���͵�Ͽ�     0���͵��ͨ
        Bit6       1��GPS�Ѷ�λ    0��GPSδ��λ
        Bit3~Bit5  100��SOS ��� 0x20
                   011���͵籨�� 0x18
                   010���ϵ籨�� 0x10
                   001���𶯱��� 0x08
                   000������
        Bit2       1���ѽӵ�Դ��� 0��δ�ӵ�Դ���
        Bit1       1��ACC��        0��ACC��
        Bit0       1�����         0������
    */
    //���/����
    if (track_cust_status_defences() == 2)
    {
        sendData[i] |= 0x01;
    }
    //acc
#if defined (__VIRTUAL_ACC__)
    if (track_cust_virtual_acc() == 1)
    {
        sendData[i] |= 0x02;
    }
#else
    if (track_cust_status_acc() == 1)
    {
        sendData[i] |= 0x02;
    }
#endif

    //�����
#if defined(__GT420D__)
    if (track_cust_refer_charge_data(99))
#else
    if (track_cust_status_charger())
#endif
    {
        sendData[i] |= 0x04;
    }
    //��������

    //GPS��λ���
    if (track_cust_gps_status() >= GPS_STAUS_2D)
    {
        sendData[i] |= 0x40;
    }
    //�͵�״̬ �� �����״̬
    if (track_cust_oil_cut(99) || track_cust_motor_lock(99))
    {
        sendData[i] |= 0x80;
    }
    i++;

    //��ѹ
#if !defined(__GT420D__)
    volt = track_cust_get_battery_volt();
    if (volt > 0) volt /= 10000;
    volt2 = volt;
#else
    volt = track_cust_MCUrefer_adc_data(99);
    volt2 = (kal_uint16)volt;
#endif
    sendData[i++] = (volt2 >> 8);
    sendData[i++] = (volt2 & 0x00FF);

    /*GSM�ź�ǿ��
    0�����źţ�
    1���źż���
    2���źŽ���
    3���ź�����
    4���ź�ǿ*/
    ret = track_cust_get_rssi_level();
    if (ret >= 1 && ret <= 4)
    {
        sendData[i] = ret;
    }
    else
    {
        sendData[i] = 0;
    }
    i++;

    /*
        0x00������
        0x01��SOS ���
        0x02���ϵ籨��
        0x03: �𶯱���
        0x04: ��Χ������
        0x05: ��Χ������
        0x06: ���ٱ���
        0x07: λ�Ʊ���
    */
    i++;

    //���� 0x01: ���ģ�0x02: Ӣ��
    //sendData[i++] = language;
    if (G_parameter.lang == 1)
    {
        sendData[i++] = 1;
    }
    else
    {
        sendData[i++] = 2;
    }
    no = DataPacketEncode78(0x23, sendData, size);
    memcpy(data, sendData, size);
    *len = size;
    *sn = no;
    Ram_Free(sendData);
#endif
#endif
}
//GT411��������,����ѹ+�¶�+ʪ��,by_znn,2018,11.15
void track_cust_paket_36(kal_uint8* data, kal_uint16 *len, kal_uint16 *sn)
{
    #if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData;
    U8 tmp, i = 4;
    U16 no, volt2, tem2, hum2;
    U32 volt;
    int size, ret;
    LOGD(L_SOC, L_V5, "");
    size = 10 + 20/*status*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);
    /*
            Bit7       1���͵�Ͽ�     0���͵��ͨ
            Bit6       1��GPS�Ѷ�λ    0��GPSδ��λ
            Bit3~Bit5  100��SOS ��� 0x20
                       011���͵籨�� 0x18
                       010���ϵ籨�� 0x10
                       001���𶯱��� 0x08
                       000������
            Bit2       1���ѽӵ�Դ��� 0��δ�ӵ�Դ���
            Bit1       1��ACC��        0��ACC��
            Bit0       1�����         0������
        */
    //���/����
    if (track_cust_status_defences() == 2)
    {
        sendData[i] |= 0x01;
    }
    //acc
#if defined (__VIRTUAL_ACC__)
    if (track_cust_virtual_acc() == 1)
    {
        sendData[i] |= 0x02;
    }
#else
    if (track_cust_status_acc() == 1)
    {
        sendData[i] |= 0x02;
    }
#endif

    //�����
    if (track_cust_status_charger())
    {
        sendData[i] |= 0x04;
    }
    //��������

    /* //GPS��λ���
     if(track_cust_gps_status() >= GPS_STAUS_2D)
     {
         sendData[i] |= 0x40;
     }
     //�͵�״̬ �� �����״̬
     if(track_cust_oil_cut(99) || track_cust_motor_lock(99))
     {
         sendData[i] |= 0x80;
     }
    */
    i++;

    //��ѹ�ȼ�
    sendData[i++] = track_cust_get_battery_level();

    /*
     //��ѹ(����:��ѹ�ȼ����ǵ�ѹֵ)
    volt = track_cust_get_battery_volt();
    if(volt > 0) volt /= 10000;
    volt2 = volt;
    sendData[i++] = (volt2 >> 8);
    sendData[i++] = (volt2 & 0x00FF);
    */

    /*GSM�ź�ǿ��
    0�����źţ�
    1���źż���
    2���źŽ���
    3���ź�����
    4���ź�ǿ*/
    ret = track_cust_get_rssi_level();
    if (ret >= 1 && ret <= 4)
    {
        sendData[i] = ret;
    }
    else
    {
        sendData[i] = 0;
    }
    i++;

    i++;//��������ռ2�ֽ�
    //���� 0x01: ���ģ�0x02: Ӣ��
    //sendData[i++] = language;
    if (G_parameter.lang == 1)
    {
        sendData[i++] = 1;
    }
    else
    {
        sendData[i++] = 2;
    }

    //��չ�ֶ�:��ѹ+ �¶�+ ʪ��
    //��ѹ
    sendData[i++]=0x00;
    sendData[i++]=0x27;
    sendData[i++]=0x02;
    volt = track_cust_get_battery_volt();
    if (volt > 0) volt /= 10000;
    volt2 = volt;
    sendData[i++] = (volt2 >> 8);
    sendData[i++] = (volt2 & 0x00FF);
    //�¶�
    sendData[i++]=0x00;
    sendData[i++]=0x37;
    sendData[i++]=0x02;
    tem2 = (U16)track_get_temp_form_mcu();
//��tem1��Ϊ��Ƭ���ɼ������¶ȣ������ʪ��Ҳ��һ��
    sendData[i++]=(tem2>>8);
    sendData[i++]=(tem2 & 0x00FF);
    //ʪ��
    sendData[i++]=0x00;
    sendData[i++]=0x38;
    sendData[i++]=0x02;
    hum2=(U16)track_get_humidity_form_mcu();
    sendData[i++]=(hum2>>8);
    sendData[i++]=(hum2 & 0x00FF);

    no = DataPacketEncode78(0x36, sendData, size);
    memcpy(data, sendData, size);
    *len = size;
    *sn = no;
    Ram_Free(sendData);
    #endif
}


/******************************************************************************
 *  Function    -  track_cust_paket_29
 *
 *  Purpose     -  Э�����ݰ���0x29����״̬��Ϣ\������( �鳵������)
 *
 *  Description -  NULL
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 18-05-2015,  chenjiajun  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_paket_29(kal_uint8* data, kal_uint16 *len, kal_uint16 *sn)
{
    #if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData;
    U8 tmp, i = 4;
    U16 no;
    int size, ret;
    U16 voltage;

    LOGD(L_SOC, L_V5, "");
    size = 10 + 9/*status*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);

    /*
        Bit7       1���͵�Ͽ�     0���͵��ͨ
        Bit6       1��GPS�Ѷ�λ    0��GPSδ��λ
        Bit3~Bit5  100��SOS ��� 0x20
                   011���͵籨�� 0x18
                   010���ϵ籨�� 0x10
                   001���𶯱��� 0x08
                   000������
        Bit2       1���ѽӵ�Դ��� 0��δ�ӵ�Դ���
        Bit1       1��ACC��        0��ACC��
        Bit0       1�����         0������
    */
    //���/����
    if (track_cust_status_defences() == 2)
    {
        sendData[i] |= 0x01;
    }
    //acc
#if defined (__VIRTUAL_ACC__)
    if (track_cust_virtual_acc() == 1)
    {
        sendData[i] |= 0x02;
    }
#else
    if (track_cust_status_acc() == 1)
    {
        sendData[i] |= 0x02;
    }
#endif

    //�����
    if (track_cust_status_charger())
    {
        sendData[i] |= 0x04;
    }
    //��������

    //GPS��λ���
    if (track_cust_gps_status() >= GPS_STAUS_2D)
    {
        sendData[i] |= 0x40;
    }
    //�͵�״̬ �� �����״̬
    if (track_cust_oil_cut(99) || track_cust_motor_lock(99))
    {
        sendData[i] |= 0x80;
    }
    i++;

    //��ѹ�ȼ�
    sendData[i++] = track_cust_get_battery_level();


    /*GSM�ź�ǿ��
    0�����źţ�
    1���źż���
    2���źŽ���
    3���ź�����
    4���ź�ǿ*/
    ret = track_cust_get_rssi_level();
    if (ret >= 1 && ret <= 4)
    {
        sendData[i] = ret;
    }
    else
    {
        sendData[i] = 0;
    }
    i++;

    /*
        0x00������
        0x01��SOS ���
        0x02���ϵ籨��
        0x03: �𶯱���
        0x04: ��Χ������
        0x05: ��Χ������
        0x06: ���ٱ���
        0x07: λ�Ʊ���
    */
    i++;

    //���� 0x01: ���ģ�0x02: Ӣ��
    //sendData[i++] = language;
    if (G_parameter.lang == 1)
    {
        sendData[i++] = 1;
    }
    else
    {
        sendData[i++] = 2;
    }
    //i++;
#if defined (__EXT_VBAT_ADC__)
    LOGD(L_APP, L_V5, "%d", track_drv_get_external_voltage_change());
//��ѹ��Ϣ
    voltage = track_drv_get_external_voltage_change() / 10;
    LOGD(L_APP, L_V5, "%d", voltage);
    track_fun_reverse_16(&sendData[i++], voltage);
    i++;
#else
    /*û���Ҳ��ռ������ -- chengjun          2017-08-09*/
    sendData[i++] = 0;
    sendData[i++] = 0;
#endif
    /*
        Bit7~Bit15  ����
        Bit6       1�����ٱ�����   0�����ٱ�����
        Bit5       1���͵籨����    0���͵籨����
        Bit4       1:     �ϵ籨���� 0:�ϵ籨����
        Bit2~Bit3  �𶯱�����ʽ     00 : ��GPRS      =0
                                                                 01 : CALL+GPRS  =3
                                                                 10 : SMS+GPRS   =1
                                                                 11 : CALL+SMS+GPRS  =2
        Bit1       1���𶯱�����       0���𶯱�����
        Bit0       1��Χ��������       0��Χ��������
    */
    i++;
    //Χ����������״̬
    if (G_parameter.fence[0].sw == 1)
    {
        sendData[i] |= 0x01;
    }
    //�𶯱�������״̬
    if (G_parameter.vibrates_alarm.sw == 1)
    {
        sendData[i] |= 0x02;
    }
    // �𶯱�����ʽ
    if (G_parameter.vibrates_alarm.alarm_type == 0)
    {
        sendData[i] |= 0x00;
    }
    else if (G_parameter.vibrates_alarm.alarm_type == 3)
    {
        sendData[i] |= 0x04;
    }
    else if (G_parameter.vibrates_alarm.alarm_type == 1)
    {
        sendData[i] |= 0x08;
    }
    else if (G_parameter.vibrates_alarm.alarm_type == 2)
    {
        sendData[i] |= 0x0c;
    }
    //�ϵ籨������״̬
    if (G_parameter.power_fails_alarm.sw == 1)
    {
        sendData[i] |= 0x10;
    }
    //�͵籨������״̬
    if (G_parameter.low_power_alarm.sw == 1)
    {
        sendData[i] |= 0x20;
    }
    //���ٱ����� ��״̬
    if (G_parameter.speed_limit_alarm.sw == 1)
    {
        sendData[i] |= 0x40;
    }
    no = DataPacketEncode78(0x29, sendData, size);
    memcpy(data, sendData, size);
    *len = size;
    *sn = no;
    Ram_Free(sendData);
    #endif
}

/******************************************************************************
 *  Function    -  track_cust_paket_8A
 *
 *  Purpose     -  ��Ӧ�÷�����Уʱ
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 16-07-2015,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int8 track_cust_paket_8A(void)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    LBS_Multi_Cell_Data_Struct *lbs = track_drv_get_lbs_data();
    kal_uint8 *sendData;
    kal_int8 ret=0;
    kal_uint16 no;
    int size, i;

    LOGD(L_APP, L_V5, "");
    size = 10;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return ret;
    }
    memset(sendData, 0, size);

    no = DataPacketEncode78(0x8A, sendData, size);

    ret = Socket_write_req(0, sendData, size, PACKET_LBS_DATA, 0x8A, no, 1);

    //track_socket_queue_alarm_send_reg(sendData, size, 0x8A, no);

    Ram_Free(sendData);

    return ret;
    #endif
}

void track_cust_paket_8B(void)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    LBS_Multi_Cell_Data_Struct *lbs = track_drv_get_lbs_data();
    kal_uint8 *sendData;
    kal_uint16 no;
    int size, i, ret;

    LOGD(L_APP, L_V5, "");
    size = 10 + 8/*lbs*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);

    // lbs
    DataPackingLbs_8(&sendData[4], lbs);

    no = DataPacketEncode78(0x8B, sendData, size);
    track_socket_queue_alarm_send_reg(sendData, size, 0x8B, no);
    Ram_Free(sendData);
    #endif
}

void track_cust_paket_8C(kal_uint8 queue, kal_uint8 acc_status, kal_uint8 *data, kal_uint16 len)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    kal_uint8 *sendData;
    kal_uint16 no;
    int size, i = 5;
    track_gps_data_struct *gd = track_cust_gpsdata_alarm();

    LOGD(L_APP, L_V5, "");
    if (len > 1000 || len <= 0)
    {
        LOGD(L_APP, L_V1, "ERROR: Data length abnormalities(%d)!", len);
        return;
    }
    size = 11 + 6/*datatime*/ + 1/*acc*/ + len/*OBD*/ + 12/*gps*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);

    // datetime
    DataPackingDatetime_6(&sendData[i], gd);
    i += 6;

    // ACC
    sendData[i] = acc_status;
    /*if(track_cust_status_acc() == 1)
    {
        sendData[i] = 1;
    }*/
    i++;

    // OBD
    memcpy(&sendData[i], data, len);
    i += len;

    // gps
    DataPackingGps_12(&sendData[i], gd);

    no = DataPacketEncode79(0x8C, sendData, size);
    if (queue == 1)
        track_socket_queue_position_send_reg(sendData, size, 0x8C, 0);
    else
        track_socket_queue_send_netcmd_reg(sendData, size, 0x8C, 0);
    Ram_Free(sendData);
    #endif
}

/*����__LUYING__����ĿҪע�⣬������ϴ���server4       --    chengjun  2017-09-27*/
void track_cust_paket_8D(
    U8  filetype,   /*�ļ�����*/
    U32 filelen,    /*�ļ��ܳ���*/
    U8  errorCheck, /*�ļ�����У������*/
    U8 *checkData,  /*�ļ�����У��*/
    U32 begin,      /*��ʼλ��*/
    U16 datalen,    /*��ǰ���ݳ���*/
    U8 *data)       /*����*/
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    kal_uint8 *sendData;
    kal_uint16 no;
    int size, i = 5;

#if defined(__LUYING__)
    extern kal_uint8* track_get_LY_stamp(void);
#endif

#ifndef __XCWS__

    LOGD(L_APP, L_V5, "filetype:%d, filelen:%d, errorCheck:%d, begin:%d, datalen:%d", filetype, filelen, errorCheck, begin, datalen);
    if (datalen > 1000 || datalen <= 0)
    {
        LOGD(L_APP, L_V1, "ERROR: Data length abnormalities(%d)!", datalen);
        return;
    }
    size = 11 + 1/*filetype*/ + 4/*filelen*/ + 1/*errorCheck*/ + 4/*begin*/ + 2/*datalen*/ + datalen/*data*/;
    if (errorCheck == 0)/*CRC*/
    {
        size += 2/*checkData*/;
    }
    else if (errorCheck == 1)/*MD5*/
    {
        size += 8/*checkData*/;
    }
    else
    {
        LOGD(L_APP, L_V1, "ERROR: errorCheck");
        return;
    }
    if (filetype == 0)
    {
        size += 6/*alerm Datatime*/;
    }
    else if (filetype == 1)
    {
        size += 2/*alerm NO*/;
    }
#if defined(__LUYING__)
    else if (filetype == 3)
    {
        size += 6/*alerm NO*/;
    }
#endif

    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);

    /*�ļ�����*/
    sendData[i++] = filetype;

    /*�ļ��ܳ���*/
    track_fun_reverse_32(&sendData[i], filelen);
    i += 4;

    /*�ļ�����У������*/
    sendData[i++] = errorCheck;

    /*�ļ�����У��*/
    if (errorCheck == 0)/*CRC*/
    {
        sendData[i++] = checkData[1];
        sendData[i++] = checkData[0];
    }
    else if (errorCheck == 1)/*MD5*/
    {
        memcpy(&sendData[i], checkData, 8);
        i += 8;
    }

    /*��ʼλ��*/
    track_fun_reverse_32(&sendData[i], begin);
    i += 4;

    /*��ǰ���ݳ���*/
    track_fun_reverse_16(&sendData[i], datalen);
    i += 2;

    /*����*/
    memcpy(&sendData[i], data, datalen);
    i += datalen;

    if (filetype == 0)
    {
        /*alerm Datatime*/
        DataPackingDatetime_6(&sendData[i], NULL);
    }
    else if (filetype == 1)
    {
        /*alerm NO*/
        U16 tmp = track_cust_sos_get_alarm_sn();
        sendData[i] = (tmp & 0xFF00) >> 8;
        sendData[i + 1] = (tmp & 0x00FF);
    }
#if defined(__LUYING__)
    else if (filetype == 3)
    {
        memcpy(&sendData[i], track_get_LY_stamp(), 6);
    }
#endif

    no = DataPacketEncode79(0x8D, sendData, size);


#if defined (__LUYING__)
    track_cust_server4_queue_send_reg(sendData, size, 0x8D, 0);
#else
    track_socket_queue_send_log_reg(sendData, size, 0x8D, 0);
#endif /* __LUYING__ */


#endif
    Ram_Free(sendData);
#endif
}

void track_cust_paket_8E(kal_uint8 *data, kal_uint16 len)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    kal_uint8 *sendData;
    kal_uint16 no;
    int size, i = 5;
    track_gps_data_struct *gd = track_cust_gpsdata_alarm();

    LOGD(L_APP, L_V5, "");
    if (len > 1000 || len <= 0)
    {
        LOGD(L_APP, L_V1, "ERROR: Data length abnormalities(%d)!", len);
        return;
    }
    size = 11 + 6/*datatime*/ + len/*OBD*/ + 12/*gps*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);

    // datetime
    DataPackingDatetime_6(&sendData[i], gd);
    i += 6;

    // OBD
    memcpy(&sendData[i], data, len);
    i += len;

    // gps
    DataPackingGps_12(&sendData[i], gd);

    no = DataPacketEncode79(0x8E, sendData, size);
    if (track_soc_login_status())
        track_socket_queue_position_send_reg(sendData, size, 0x8E, 0);
    else
        track_socket_queue_send_netcmd_reg(sendData, size, 0x8E, 0);
    Ram_Free(sendData);
    #endif
}

void track_cust_paket_94(kal_uint8 datatype, kal_uint8 *data, kal_uint16 len)
{
    #if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
#ifndef __XCWS__
    kal_uint8 *sendData;
    kal_uint16 no;
    int size, i = 5;
    kal_int16 volt = 0;
    track_gps_data_struct *gd = track_cust_gpsdata_alarm();
#if defined(__EXT_VBAT_ADC__)
    volt = track_drv_get_external_voltage_change();
//   LOGS("volt:%d", volt);
#endif  /*__JIMISHARE__*/
    LOGD(L_APP, L_V5, "");
#if defined(__SPANISH__)
    if (G_parameter.itp_sw ==1)
    {
        return;
    }
#endif
    if (len > 1000 || len <= 0)
    {
        LOGD(L_APP, L_V1, "ERROR: Data length abnormalities(%d)!", len);
        return;
    }
    size = 11 + 1/*datatype*/ + len;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);

    sendData[i++] = datatype;

    if (datatype == 0)
    {
        track_fun_reverse_16(&sendData[i++], volt);
    }
    else
    {
        memcpy(&sendData[i], data, len);
        i += len;
    }

    no = DataPacketEncode79(0x94, sendData, size);
    track_socket_queue_send_netcmd_reg(sendData, size, 0x94, 0);
    Ram_Free(sendData);
#endif
#endif
}

void track_cust_up_900a_handle(void)
{
    #if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    kal_uint8 sendData[50] = {0};
    kal_uint8 Data[23] = {0};
    kal_uint8 tmp_Data[12] = {0};
    kal_uint8 i, j;

    memcpy(sendData, track_drv_get_imei(1), 8);
    memcpy(&sendData[8], track_drv_get_imsi(1), 8);
    memcpy(Data, (kal_uint8*)OTA_ell_get_iccid(), 20);
    memcpy(&sendData[16], track_fun_stringtohex(Data, 20), 10);
    track_cust_paket_94(0x0A, sendData, 26);
    #endif
}
U8 alarm_type_change(U8 sw, U8 alarm_type)
{
    U8 ret = 0;
    switch (alarm_type)
    {
        case 0:
            ret |= 0x04;
            break;
        case 1:
            ret |= 0x05;
            break;
        case 2:
            ret |= 0x07;
            break;
        case 3:
            ret |= 0x06;
            break;
    }
    if (sw == 1) ret |= 0x08;
    return ret;
}
#if defined(__JM66__)
void track_cust_jm66_paket_94_handle(U8 type, U8*data, U32 len)
{
    track_cust_paket_94(type, data, len);
}
#endif//__JM66__
/******************************************************************************
 *  Function    -  track_cust_paket_9404_handle
 *
 *  Purpose     -
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 10-09-2015,  Cjj  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_paket_9404_handle(void)
{
    #if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    // HIGH4  +  LOW4
    kal_uint8 ALM1 = 0, //  ��,   λ��
                     ALM2 = 0, // �ڵ�,   ���
                            ALM3 = 0, // ����,   �ϵ�
                                   ALM4 = 0, // SOS     ,   ����
                                          STA1 = 0, // ���,   ����
                                                 DYD = 0;//δ����,�͵�
    char data[200] = {0}, sos[68] = {0}, center[26] = {0}, fence[70] = {0};
    char in_out[10] = {0}, mode_st[24] = {0};

#if defined(__GT02__) || defined(__BCA__) || defined(__SERVER_IP_LINK__)||defined(__JM66__)
    return;
#endif
    LOGD(L_APP, L_V5, "");
    if (!track_soc_login_status())
    {
        tr_start_timer(TRACK_CUST_PACKET_94O4_DELAY_ID, 60000, track_cust_paket_9404_handle);
        return;
    }

    /*
    ��7   Bit,�𶯱���   ����
    ��6   Bit,���籨��
    ��5   Bit,�绰����
    ��4   Bit,���ű���

    ��3   Bit,λ�Ʊ���   ����
    ��2   Bit,���籨��
    ��1   Bit,�绰����
    ��0   Bit,���ű���
    */
    ALM1 |= alarm_type_change(G_parameter.displacement_alarm.sw, G_parameter.displacement_alarm.alarm_type);
    ALM1 |= alarm_type_change(G_parameter.vibrates_alarm.sw, G_parameter.vibrates_alarm.alarm_type) << 4;


    /*
    ��7   Bit,�ڲ��͵籨��   ����
    ��6   Bit,���籨��
    ��5   Bit,�绰����
    ��4   Bit,���ű���

    ��3   Bit,�ⲿ�͵籨��   ����
    ��2   Bit,���籨��
    ��1   Bit,�绰����
    ��0   Bit,���ű���
    */
    ALM2 |= alarm_type_change(G_parameter.power_extbatalm.sw, G_parameter.power_extbatalm.alarm_type);
    ALM2 |= alarm_type_change(G_parameter.low_power_alarm.sw, G_parameter.low_power_alarm.alarm_type) << 4;

    /*
    ��7   Bit,���ٱ���   ����
    ��6   Bit,���籨��
    ��5   Bit,�绰����
    ��4   Bit,���ű���

    ��3   Bit,�ϵ籨��   ����
    ��2   Bit,���籨��
    ��1   Bit,�绰����
    ��0   Bit,���ű���
    */
    ALM3 |= alarm_type_change(G_parameter.power_fails_alarm.sw, G_parameter.power_fails_alarm.alarm_type);
    ALM3 |= alarm_type_change(G_parameter.speed_limit_alarm.sw, G_parameter.speed_limit_alarm.alarm_type) << 4;

#if defined(__UART1_SOS__)
    /*
    ��7   Bit,SOS ����   ����
    ��6   Bit,���籨��
    ��5   Bit,�绰����
    ��4   Bit,���ű���

    ��3   Bit,���ر���   ����
    ��2   Bit,���籨��
    ��1   Bit,�绰����
    ��0   Bit,���ű���
    */
    ALM4 |= alarm_type_change(G_parameter.sos_alarm.sw, G_parameter.sos_alarm.alarm_type) << 4;
#endif /*__UART1_SOS__*/

    /*
    ��7   Bit,���״̬  �����1
    ��6   Bit,�Զ����
    ��5   Bit,�ֶ����
    ��4   Bit,ң�����

    ��3   Bit,
    ��2   Bit,
    ��1   Bit,���𿪹رպ�
    ��0   Bit,���𱨾�״̬
    */
    if (G_realtime_data.defense_mode == 1)
        STA1   |= 0x20 ; //�ֶ�
    else
        STA1   |= 0x40 ; //�Զ�
    if (G_realtime_data.defences_status == 3)
        STA1   |= 0x80 ;
#if defined(__GT370__)|| defined (__GT380__)||defined(__GT300S__)||defined(__GT420D__)
    if (G_realtime_data.switch_down == 1)
        STA1 |= 0x02;
    if (G_parameter.teardown_str.sw == 1) //���𱨾�����
        STA1 |= 0x01;
#endif

    /*
    ��7   Bit,
    ��6   Bit,
    ��5   Bit,
    ��4   Bit,

    ��3   Bit,�ٶȹ����ӳ�ִ��
    ��2   Bit,δ��λ�ӳ�ִ��
    ��1   Bit,�Ͽ��͵�
    ��0   Bit,�͵��ͨ
    */
    switch (G_realtime_data.oil_status)
    {
        case 0:
            DYD |= 0x01;
            break;
        case 1:
            DYD |= 0x02;
            break;
        case 2:
            DYD |= 0x04;
            break;
        case 3:
            DYD |= 0x08 ;
            break;
    }

    //SOS����
    sprintf(sos, "SOS=%s,%s,%s", G_parameter.sos_num[0], G_parameter.sos_num[1], G_parameter.sos_num[2]);

    //���ĺ���
    sprintf(center, "CENTER=%s", G_parameter.centerNumber);

    //Χ��
    if (G_parameter.fence[0].in_out == 0)
        sprintf(in_out, "IN");
    else if (G_parameter.fence[0].in_out == 1)
        sprintf(in_out, "OUT");
    else if (G_parameter.fence[0].in_out == 2)
        sprintf(in_out, "IN or OUT");

    if (G_parameter.fence[0].square == 0)
    {
        sprintf(fence, "Fence,%s,%d,%.6f,%.6f,%d,%s,%d",
                ON_OFF(G_parameter.fence[0].sw),
                G_parameter.fence[0].square,
                G_parameter.fence[0].lat,
                G_parameter.fence[0].lon,
                G_parameter.fence[0].radius * 100,
                in_out, G_parameter.fence[0].alarm_type);
    }
    else
    {
        sprintf(fence, "Fence,%s,%d,%.6f,%.6f,%.6f,%.6f,%s,%d",
                ON_OFF(G_parameter.fence[0].sw),
                G_parameter.fence[0].square,
                G_parameter.fence[0].lat,
                G_parameter.fence[0].lon,
                G_parameter.fence[0].lat2,
                G_parameter.fence[0].lon2,
                in_out, G_parameter.fence[0].alarm_type);
    }
#if defined(__GT370__)|| defined (__GT380__)
    if (G_parameter.work_mode.mode == 3)
    {
        sprintf(mode_st, "MODE,%d,%d:%d,%d", 2, G_parameter.work_mode.datetime / 60, G_parameter.work_mode.datetime % 60,  G_parameter.work_mode.mode3_timer / 60);
    }
    else
    {
        sprintf(mode_st, "MODE,%d,%d,%d", 1, G_parameter.gps_work.Fti.interval_acc_on, G_parameter.gps_work.Fti.interval_acc_off);
    }
    sprintf(data, "ALM1=%0.2X;ALM2=%0.2X;ALM3=%0.2X;STA1=%0.2X;DYD=%0.2X;%s;%s;FENCE=%s;ICCID=%s;MODE=%s;", \
            ALM1, ALM2, ALM3, STA1, DYD, sos, center, fence,OTA_ell_get_iccid(), mode_st);
#elif  defined(__GT300S__)
    if (G_parameter.work_mode.mode == 2)
        sprintf(mode_st, "MODE,%d,%d", G_parameter.work_mode.mode, G_parameter.gps_work.Fti.interval_acc_on);
    else if (G_parameter.work_mode.mode == 1)
        sprintf(mode_st, "MODE,%d,%d",  G_parameter.work_mode.mode, G_parameter.work_mode.mode1_timer);
    else if (G_parameter.work_mode.mode == 3)
        sprintf(mode_st, "MODE,%d,%0.2d:%0.2d,%d",  G_parameter.work_mode.mode,
                G_parameter.work_mode.datetime / 60,  G_parameter.work_mode.datetime % 60,  G_parameter.work_mode.mode3_timer / 60);
    sprintf(data, "ALM1=%0.2X;ALM2=%0.2X;ALM3=%0.2X;STA1=%0.2X;DYD=%0.2X;%s;%s;FENCE=%s;ICCID=%s;MODE=%s;", \
            ALM1, ALM2, ALM3, STA1, DYD, sos, center, fence,OTA_ell_get_iccid(), mode_st);

#elif defined(__GT420D__)
    LOGD(L_APP, L_V5, "1");
    if (G_parameter.extchip.mode == 1)
    {
        sprintf(mode_st, "MODE,%d,%0.2d",  G_parameter.extchip.mode,G_parameter.extchip.mode1_par1);
    }
    else if (G_parameter.extchip.mode == 2)
    {
        sprintf(mode_st, "MODE,%d,%0.2d",  G_parameter.extchip.mode,G_parameter.extchip.mode2_par1);
    }
    else
    {
        sprintf(mode_st, "MODE,%d",  G_parameter.extchip.mode);
    }
    sprintf(data, "ALM1=%0.2X;ALM2=%0.2X;ALM3=%0.2X;STA1=%0.2X;DYD=%0.2X;%s;%s;FENCE=%s;ICCID=%s;MODE=%s;", \
            ALM1, ALM2, ALM3, STA1, DYD, sos, center, fence,OTA_ell_get_iccid(), mode_st);
#elif defined(__UART1_SOS__)
    sprintf(data, "ALM1=%0.2X;ALM2=%0.2X;ALM3=%0.2X;ALM4=%0.2X;STA1=%0.2X;DYD=%0.2X;%s;%s;FENCE=%s;",
            ALM1, ALM2, ALM3, ALM4, STA1, DYD, sos, center, fence);
#else
    sprintf(data, "ALM1=%0.2X;ALM2=%0.2X;ALM3=%0.2X;STA1=%0.2X;DYD=%0.2X;%s;%s;FENCE=%s;",
            ALM1, ALM2, ALM3, STA1, DYD, sos, center, fence);
#endif
    LOGD(L_APP, L_V5, "data=%d, sos=%d, center=%d, fence=%d\r\ndata:%s", strlen(data), strlen(sos), strlen(center), strlen(fence), data);
    track_cust_paket_94(04, data, strlen(data));
    #endif
}

#if defined(__DASOUCHE__)
void track_cust_sengpaket_led_set(kal_uint8 flag)
{
    if(flag == 1)
    {
        track_cust_send17((void*)4,(void*)1);
        track_start_timer(TRACK_CUST_SENDPAKET_LEDSET_TIMER_ID, 3000, track_cust_sengpaket_led_set, (void *)0);
        }
    else
    {
        track_cust_send17((void*)4,(void*)0);
        }
        }

kal_uint8 track_cust_delaysend_locatepaket()
{
    char buf[100] = {0};
    sprintf(buf, "AT^GT_CM=22#\r\n");
    track_drv_write_exmode(buf, strlen(buf));
    }

/******************************************************************************
 *  Function    -  track_cust_dsc_location_paket
 *
 *  Purpose     -  ���ѳ���λ���ݰ��߶��з��͡�
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 28-12-2018,  Cjj  written
 * ----------------------------------------
*******************************************************************************/
kal_uint16 track_cust_dsc_location_paket(
    track_gps_data_struct *gd,
    LBS_Multi_Cell_Data_Struct *lbs,
    kal_uint8 location_type,
    kal_uint8 dataDesc,
    kal_uint8 alarm_type,
    kal_uint8 upload_type)
{    
    extern float track_get_temp_form_mcu();
    extern float track_get_humidity_form_mcu();
    extern kal_uint8 track_cust_get_battery_percent();
    extern kal_uint32 track_cust_refer_lightsensor_data(kal_uint32 arg);
    char buf1[100] = {0};
    applib_time_struct currTime_ams = {0};
    U8 *sendData;
    U16 no;
    int size;
        applib_time_struct time = {0};
    dasouche_location_struct location ={0};
    // ע��size��ֵ���ܴ��� BACKUP_DATA_PACKET_MAX �궨��
    size = 10 + sizeof(dasouche_location_struct);   
    LOGD(L_APP, L_V1, "size%d,location_type=%d,dataDesc=%d,alarm_type=%d",size,location_type,dataDesc,alarm_type);
    LOGD(L_APP,L_V5,"123:%d",&lbs->MainCell_Info.mcc);
    sendData = (U8*)Ram_Alloc(5, size);
    if(sendData == NULL)
    {
        LOGD(L_APP, L_V1, "Ram_Alloc NULL!!");
        return;
    }
    memset(sendData, 0, size);
    //��λ״̬
    //location.status = track_cust_gps_status();
    //ʱ���
    //location.timestamp = track_cust_get_timestamp();
    
    track_fun_get_time(&time, KAL_TRUE, NULL);
    memcpy(&location.locationtime,&time,sizeof(applib_time_struct));
    
    if (location.location_type == 2 ||location.location_type == 6)
    {
        gd = track_cust_backup_gps_data(2, NULL);
        gd->gprmc.status = 0;
    }
    else if(location.location_type == 1)
    {
        gd = track_cust_backup_gps_data(0, NULL);
        }
    if(track_cust_gt420d_carstatus_data(99) == 2)
    {
        if(location_type == 1)
            {
                G_realtime_data.last_location_status = 1;
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                }
        else
            {
                G_realtime_data.last_location_status = 0;
                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                }
        }
    if(gd)
    {
        //��γ��
        location.Latitude = gd->gprmc.Latitude;
        location.Longitude= gd->gprmc.Longitude;
        //������
        location.EW = gd->gprmc.EW;
        //�ϱ�γ
        location.NS = gd->gprmc.NS;
        //�ٶ�
        if (track_cust_gt420d_carstatus_data(99) == 1)
        {
            location.Speed = gd->gprmc.Speed;
        }
        else if (track_cust_gt420d_carstatus_data(99) == 2)
        {
            location.Speed = 0;
            }
        //����
        location.Course = gd->gprmc.Course;
        //����
        location.altitude = gd->gpgga.msl_altitude;
        //��λ״̬
        location.status = gd->gprmc.status;
    }
    //�ϱ�����
    location.upload_type = upload_type;
    //���ֵ
    //location.mileage = track_get_gps_mileage();
    //ACC
    //location.acc = track_cust_status_acc();
    //��λ����
    location.location_type = location_type;
    //���ݰ�����
    location.dataDesc = dataDesc;
    //��������
    location.alarm_type = alarm_type;
    //����
    //location.lastPower = -1;

    location.battery = track_cust_get_battery_percent();

    location.temp = track_get_temp_form_mcu();
    
    location.humidity = track_get_humidity_form_mcu();

    location.humitureStatus = G_parameter.humiture_sw;
 
    location.light_status = track_cust_refer_lightsensor_data(99);

    location.charge_Status = track_cust_refer_charge_data(99);

    location.lightSensorStatus = G_parameter.teardown_str.sw;

    location.volt = track_cust_MCUrefer_adc_data(99);

    location.staticmode.staticmode1time = G_parameter.statictime.mode1_statictime;

    location.staticmode.staticmode2time = G_parameter.statictime.mode2_statictime;

    if(track_cust_check_gpsflag_status(99) == 1)
    {
        location.locationsw.GPSSW = 1;
            }
    else if(track_cust_check_gpsflag_status(99) == 2)
    {
        location.locationsw.GPSSW = 0;
            }

    location.locationsw.WFSW = G_parameter.wifi.sw;
    
    if (track_cust_gt420d_carstatus_data(99) == 1)
    {
        location.runStatus = 1;
        }
    else
    {
       location.runStatus = 0;
        }
    
    if (G_parameter.extchip.mode == 1||G_parameter.extchip.mode == 2 )
    {
        location.mode = G_parameter.extchip.mode -1;
        }
    if (G_parameter.extchip.mode == 1||G_parameter.extchip.mode == 2 )
    {
        if (G_parameter.extchip.mode == 1)
        {
            location.mode_time = G_parameter.extchip.mode1_par1;
        }
        else
        {
            location.mode_time = G_parameter.extchip.mode2_par1;
        }
        }
    
    if(lbs)
    {
        //LBS����
        memcpy(&location.lbs,lbs,sizeof(LBS_Multi_Cell_Data_Struct));
    }
#if defined(__WIFI__)        
    //WIFI����
    memcpy(&location.wifi,&wifi_status,sizeof(track_wifi_struct));
#endif  

    memcpy(&sendData[4],&location,sizeof(dasouche_location_struct)); 

    no = DataPacketEncode78(0x22, sendData, size);
    track_socket_queue_position_send_reg(sendData, size, 0x22, no);
    #if defined(__GT420D__)
                track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                snprintf(buf1, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d send dsc data,%d,%d,%d\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec,
                            location_type,
                            upload_type,
                            alarm_type
                            );
        track_cust_save_logfile(buf1, strlen(buf1));
    #endif
    if(G_parameter.sendpaketled_sw == 1)
    {
        track_cust_sengpaket_led_set(1);
    }   
    Ram_Free(sendData);
    return no;
}
#endif

/******************************************************************************
 *  Function    -  track_cust_paket_position
 *
 *  Purpose     -  �ж��ϴ������ݰ��Ƿ�ʹ�ô�UTCʱ��
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint16 track_cust_paket_position(
    track_gps_data_struct *gd,
    LBS_Multi_Cell_Data_Struct *lbs,
    kal_uint8 packet_type,
    kal_uint8 packet_status)
{
    kal_uint16 no;
    char buf[100] = {0};
    applib_time_struct currTime = {0};
    applib_time_struct currTime_ams = {0};
#if defined(__XCWS__)

#elif defined(__GT02__)
    LOGD(L_APP, L_V7, "02 Ҫ�����packet_type :%d; packet_status :%d", packet_type, packet_status);
    no = track_cust_paket_gpsdata(gd, lbs);
#elif defined (__300S_WIFI_TEST__)
    LOGD(L_APP, L_V5, "2C@2");
    track_cust_sendWIFI_paket();
    no = 3;
#elif defined (__WM_OLD06__)||defined (__RTC_OLD_PROTOCOL__)
    no = track_cust_paket_12(gd, lbs);
#elif defined(__GT03F__)|| defined(__GT300__) || defined(__GW100__)||defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__) //�����ֳ���Ŀ?
    if (track_cust_is_upload_UTC_time())
    {
        no = track_cust_paket_22(gd, lbs, packet_type, packet_status);
    }
    else
    {
        no = track_cust_paket_10(gd);
    }
#elif defined(__XYBB__)
    LOGD(L_APP, L_V5, "@0200@");
    track_cust_paket_0200(gd, lbs);
#else
    if (track_cust_is_upload_UTC_time())
    {
#if defined(__GT740__)||defined(__GT420D__)
        if (G_parameter.extchip.mode != 1)
        {
            #if  !defined(__GT420D__)
            if (G_parameter.new_position_sw==1)
            {
                no = track_cust_paket_2D(gd, lbs, packet_type, packet_status);
                track_cust_lock_pack_no(no);
            }
            else
            #endif
            {
                #if defined(__GT420D__)
                track_fun_get_time(&currTime, KAL_FALSE, &G_parameter.zone);
                snprintf(buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d send GPS\n\r",
                            currTime.nYear,
                            currTime.nMonth,
                            currTime.nDay,
                            currTime.nHour,
                            currTime.nMin,
                            currTime.nSec);
        track_cust_save_logfile(buf, strlen(buf));
    #endif
#if   defined(__GT420D__)
                if(track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 && track_cust_refer_motion_status_data(99) == 0 \
                    && track_cust_check_simalm_flag(99) == 0 && track_cust_check_firstpoweron_flag(99) == 0 &&track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0)
                {
                    LOGD(L_APP,L_V5,"�����ϴ�GPS״̬��");
                    track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,9);
                }
                if(track_cust_check_firstpoweron_flag(99) == 1 && track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 && track_cust_refer_motion_status_data(99) == 0 \
                    && track_cust_check_simalm_flag(99) == 0 && track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0)
                {
                    LOGD(L_APP,L_V5,"�ϴ�������");
                    track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,1,15);
                    track_cust_check_firstpoweron_flag(0);
                }
                else
                {
                    track_cust_check_firstpoweron_flag(0);
                    }
                if (track_cust_refer_keyin_data(99) == 1)
                {
                    LOGD(L_APP,L_V5,"�����ϴ�GPS״̬��");
                    track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,3);
                    track_cust_refer_keyin_data(0);
                }
                if (track_cust_needsend_charge_data(99) == 1)
                {
                    LOGD(L_APP,L_V5,"����ϴ�GPS״̬��");
                    track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,4);
                    track_cust_needsend_charge_data(0);
                }
                if (track_cust_needsend_charge_data(99) == 2)
                {
                    LOGD(L_APP,L_V5,"�ε��ϴ�GPS״̬��");
                    track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,5);
                    track_cust_needsend_charge_data(0);
                }
                if (track_cust_refer_motion_status_data(99) == 1)
                {
                    #if defined(__GT420D__)
                    track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                    snprintf(buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d SEND CAR CHANGE %d,%d,%d,%d,%d,%d,%d,%d,%d\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec,
                            track_cust_gt420d_carstatus_data(99),
                            track_cust_refer_keyin_data(99),
                            track_cust_needsend_charge_data(99),
                            track_cust_refer_motion_status_data(99),
                            track_cust_check_simalm_flag(99),
                            track_cust_check_firstpoweron_flag(99),
                            track_cust_check_downalm_flag(98),
                            track_cust_check_teardown_flag(98),
                            track_cust_check_batteryalm_flag(99));
                    LOGD(5,5,"len:%d",strlen(buf));
                    track_cust_save_logfile(buf, strlen(buf));
                    #endif

                    LOGD(L_APP,L_V5,"�˶�״̬�ı��ϴ�GPS״̬��");
                    if(track_cust_gt420d_carstatus_data(99) == 2)
                    {
                        track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,1);
                    }
                    else
                    {
                        track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,2);
                        }
                    track_cust_refer_motion_status_data(0);
                }
                if(track_cust_check_downalm_flag(98) == 3)
                {
                    LOGD(L_APP,L_V5,"�޹ⱨ������");
                    track_cust_check_downalm_flag(2);
                    tr_stop_timer(TRACK_CUST_EXTCHIP_DARKALM_DELAY_SEND_GPSORLBS_TIMER_ID);
                    track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,3,7);
                }
                if(track_cust_check_teardown_flag(98) == 3)
                {
                    LOGD(L_APP,L_V5,"���䱨������");
                    track_cust_check_teardown_flag(2);
                    tr_stop_timer(TRACK_CUST_EXTCHIP_TEARDOWN_DELAY_SEND_GPSORLBS_TIMER_ID);
                    track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,3,6);
                }
                if(track_cust_check_batteryalm_flag(99) == 1)
                {
                    LOGD(L_APP,L_V5,"�͵籨������");
                    track_cust_check_batteryalm_flag(0);
                    track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,4,14);
                }
                if(track_cust_check_simalm_flag(99) == 1)
                {
                    LOGD(L_APP,L_V5,"���������ϴ�GPS״̬��");
                    track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,2,13);
                    track_cust_check_simalm_flag(0);
                    }
                track_cust_check_beforepoweroff_flag(0);
#else
    no = track_cust_paket_22(gd, lbs, packet_type, packet_status); //ֱ���ظ��¶�λ�����ٷ��;ɰ�
#endif
            }
        }
        else
        {
#if   defined(__GT420D__)
            if(track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 && track_cust_refer_motion_status_data(99) == 0 \
                    && track_cust_check_simalm_flag(99) == 0 && track_cust_check_firstpoweron_flag(99) == 0 && track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0)            
                    {
                LOGD(L_APP,L_V5,"�����ϴ�GPS״̬��");
                track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,9);
            }
            if(track_cust_check_firstpoweron_flag(99) == 1 && track_cust_refer_keyin_data(99) == 0 && track_cust_needsend_charge_data(99) == 0 && track_cust_refer_motion_status_data(99) == 0 \
                    && track_cust_check_simalm_flag(99) == 0 && track_cust_check_downalm_flag(98) == 2 \
                    && track_cust_check_teardown_flag(98) == 2 && track_cust_check_batteryalm_flag(99) == 0)
                {
                    LOGD(L_APP,L_V5,"�ϴ�������");
                    track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,1,15);
                    track_cust_check_firstpoweron_flag(0);
                }
            else
            {
            track_cust_check_firstpoweron_flag(0);
            }
            if (track_cust_refer_keyin_data(99) == 1)
            {
                LOGD(L_APP,L_V5,"�����ϴ�GPS״̬��");
                track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,3);
                track_cust_refer_keyin_data(0);
            }
            if (track_cust_needsend_charge_data(99) == 1)
            {
                LOGD(L_APP,L_V5,"����ϴ�GPS״̬��");
                track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,4);
                track_cust_needsend_charge_data(0);
            }
            if (track_cust_needsend_charge_data(99) == 2)
                {
                    LOGD(L_APP,L_V5,"�ε��ϴ�GPS״̬��");
                    track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,5);
                    track_cust_needsend_charge_data(0);
                }
            if(track_cust_check_downalm_flag(98) == 3)
                {
                    LOGD(L_APP,L_V5,"�޹ⱨ������");
                    track_cust_check_downalm_flag(2);
                    tr_stop_timer(TRACK_CUST_EXTCHIP_DARKALM_DELAY_SEND_GPSORLBS_TIMER_ID);
                    track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,3,7);
                }
                if(track_cust_check_teardown_flag(98) == 3)
                {
                    LOGD(L_APP,L_V5,"���䱨������");
                    track_cust_check_teardown_flag(2);
                    tr_stop_timer(TRACK_CUST_EXTCHIP_TEARDOWN_DELAY_SEND_GPSORLBS_TIMER_ID);
                    track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,3,6);
                }
                if(track_cust_check_batteryalm_flag(99) == 1)
                {
                    LOGD(L_APP,L_V5,"�͵籨������");
                    track_cust_check_batteryalm_flag(0);
                    track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,4,14);
                }
                if(track_cust_check_simalm_flag(99) == 1)
                {
                    LOGD(L_APP,L_V5,"���������ϴ�GPS״̬��");
                    track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,2,13);
                    track_cust_check_simalm_flag(0);
                    }
            if (track_cust_refer_motion_status_data(99) == 1)
            {
                LOGD(L_APP,L_V5,"�˶�״̬�ı��ϴ�GPS״̬��");
                    #if defined(__GT420D__)
                    track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                    snprintf(buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d SEND CAR CHANGE %d,%d,%d,%d,%d,%d,%d,%d,%d\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec,
                            track_cust_gt420d_carstatus_data(99),
                            track_cust_refer_keyin_data(99),
                            track_cust_needsend_charge_data(99),
                            track_cust_refer_motion_status_data(99),
                            track_cust_check_simalm_flag(99),
                            track_cust_check_firstpoweron_flag(99),
                            track_cust_check_downalm_flag(98),
                            track_cust_check_teardown_flag(98),
                            track_cust_check_batteryalm_flag(99));
                    LOGD(5,5,"len:%d",strlen(buf));
                    track_cust_save_logfile(buf, strlen(buf));
                    #endif

                if (track_cust_gt420d_carstatus_data(99) == 2)
                {
                    track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,1);
                }
                else
                {
                    track_cust_dsc_location_paket(gd,track_drv_get_lbs_data(),LOCA_GPS,DSC_CUST_POSITION_TIMING,0,2);
                    }
                track_cust_refer_motion_status_data(0);
            }
            track_cust_check_beforepoweroff_flag(0);
#else
    no = track_cust_paket_22(gd, lbs, packet_type, packet_status); //ֱ���ظ��¶�λ�����ٷ��;ɰ�
#endif
        }
#else
    no = track_cust_paket_22(gd, lbs, packet_type, packet_status);

#endif
    }
    else
    {
        no = track_cust_paket_12(gd, lbs);
    }
#endif


#if defined(__GT370__)|| defined (__GT380__)
    g_last_position_tick=kal_get_systicks();
#endif

    return no;
}

/******************************************************************************
 *  Function    -  track_cust_paket_msg_upload
 *
 *  Purpose     -  �жϻ�Ӧ�������·���ָ���Ƿ�ʹ��ʹ���µ�Э��
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_paket_msg_upload(kal_uint8 *serverid, kal_bool isASCII, kal_uint8 *data, int datalen)
{
#if defined(__GT02__)
    if (datalen < 255)
    {
        Server_GPRS_Command(serverid, isASCII, data, datalen);
    }
    else
    {
        track_cust_paket_21(serverid, isASCII, data, datalen);
    }

#elif defined (__RTC_OLD_PROTOCOL__)
    if (isASCII)
    {
        if (datalen < 200)
        {
            track_cust_paket_15(serverid, data, datalen);
        }
        else
        {
            track_cust_paket_21(serverid, isASCII, data, datalen);
        }
    }
    else
    {
        track_cust_paket_21(serverid, isASCII, data, datalen);
    }
#elif defined(__NT31__)||defined(__NT33__)
    //�������� __GT06D__
    track_cust_paket_21(serverid, isASCII, data, datalen);
    /*
        if(track_cust_is_upload_UTC_time())
            {
                track_cust_paket_21(serverid, isASCII, data, datalen);
            }
            else if(isASCII)
            {
                if(datalen < 200)
                {
                    track_cust_paket_15(serverid, data, datalen);
                }
                else
                {
                    track_cust_paket_21(serverid, isASCII, data, datalen);
                }
            }
            else
            {
                track_cust_paket_21(serverid, isASCII, data, datalen);
            }
    */
#elif defined(__WM_OLD06__) || defined(__GT06B__) || defined(__GT06D__) || defined(__ET130__)||defined(__NT36__)
    if (isASCII)
    {
        //0X15���г������ƣ�����255��������������check#ָ��Ļظ�
        /*���15����Գ�������200���ڲŷ���       --    chengjun  2017-04-01*/
        if (datalen < 200)
        {
            track_cust_paket_15(serverid, data, datalen);
        }
        else
        {
            track_cust_paket_21(serverid, isASCII, data, datalen);
        }
    }
    else
    {
        track_cust_paket_21(serverid, isASCII, data, datalen);
    }

#else
    track_cust_paket_21(serverid, isASCII, data, datalen);
#endif
}

kal_int8 track_cust_paket_FD(kal_uint8 *data, kal_uint16 len, U8 packet_type)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData, i = 5;
    U16 no;
    int size, ret;
    size = 11 + 8/*imei*/ + 2/*ext data len*/ + len/*ext data*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "ERROR: Ram_Alloc");
        return;
    }
    memset(sendData, 0, size);

    // imei
    memcpy(&sendData[i], track_drv_get_imei(1), 8);
    i += 8;

    /* ͸�����ݵĳ��� */
    sendData[i++] = (len & 0xFF00) >> 8;
    sendData[i++] = (len & 0x00FF);

    /* �����Ҫ͸�������� */
    memcpy(&sendData[i], data, len);

    no = DataPacketEncode79(0xFD, sendData, size);
    LOGD(L_APP, L_V5, "");
    LOGH(L_APP, L_V7, sendData, size);
    track_cust_server2_write_req(0, sendData, size,packet_type);
    Ram_Free(sendData);
    return ret;
    #endif
}

kal_int8 track_cust_paket_terminal_ID(void)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData, *chip_id, i = 0;
    U16 no;
    char *ver_string = track_cust_get_version_for_upload_server();
    int size, ver_str_len = strlen(ver_string);

    size = 1/*packet type*/ + 16/*Chip ID*/ + 1/*version string length*/ + ver_str_len/*version string*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "ERROR: Ram_Alloc");
        return -1;
    }
    memset(sendData, 0, size);

    // packet type
    sendData[i++] = 0;

    // Chip ID
    chip_id = track_drv_get_chip_id();
    memcpy(&sendData[i], chip_id, 16);
    i += 16;

    // version string length
    sendData[i++] = ver_str_len;

    // version string
    memcpy(&sendData[i], ver_string, ver_str_len);
    i += ver_str_len;
    LOGD(L_APP, L_V5, "%s", ver_string);

    track_cust_paket_FD(sendData, size, 0);
    LOGH(L_APP, L_V4, sendData, size);
    Ram_Free(sendData);
    return 0;
    #endif
}

#if defined(__BCA__)
/******************************************************************************
 *  Function    -  track_cust_packet_version_msg_upload
 *
 *  Purpose     -  ÿ�ο�����ƽ̨�ϱ�һ���̼��汾��Ϣ
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int8 track_cust_packet_version_msg_upload(void)
{
    char *ver_string = track_cust_get_version_for_upload_server();
    int ver_str_len = strlen(ver_string);
    kal_uint8 serverId[4] = {0};
    track_cust_paket_21(serverId, KAL_TRUE, ver_string, ver_str_len);
}
#endif

kal_int8 track_cust_paket_epo_file_head(kal_uint8 filetype)
{
    kal_uint8 buf[2];
    buf[0] = filetype;
    buf[1] = 0x01;
    track_cust_paket_FD(buf, 2, 0);
}

kal_int8 track_cust_paket_epo_file_content(kal_uint8 filetype, kal_uint32 data_type, kal_uint32 data_req_begin, kal_uint16 data_req_len)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData, i = 0;
    U16 no;
    int size;
    kal_int8 ret;

    size = 2/*packet type*/ + 4/*data_type*/ + 4/*data_req_begin*/ + 2/*data_req_len*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "ERROR: Ram_Alloc");
        return -1;
    }
    memset(sendData, 0, size);

    // packet type
    sendData[i++] = filetype;
    sendData[i++] = 3;

    // data_type
    memcpy(&sendData[i], &data_type, 4);
    i += 4;

    // data_req_begin
    sendData[i++] = (data_req_begin & 0xFF000000) >> 24;
    sendData[i++] = (data_req_begin & 0x00FF0000) >> 16;
    sendData[i++] = (data_req_begin & 0x0000FF00) >> 8;
    sendData[i++] = (data_req_begin & 0x000000FF);

    // data_req_len
    sendData[i++] = (data_req_len & 0xFF00) >> 8;
    sendData[i++] = (data_req_len & 0x00FF);

    track_cust_paket_FD(sendData, size, 0);
    LOGD(L_APP, L_V5, "");
    Ram_Free(sendData);
    return 0;
    #endif
}

void track_cust_paket_FD2(kal_uint8 *data, kal_uint16 len)
{
    U8 i = 5;

    // imei
    memcpy(&data[i], track_drv_get_imei(1), 8);
    i += 8;

    DataPacketEncode79(0xFD, data, len);
    LOGD(L_APP, L_V5, "");
    LOGH(L_APP, L_V7, data, len);
}

kal_int8 track_cust_net2_param(kal_uint16 len, kal_uint8* data)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    LBS_Multi_Cell_Data_Struct *lbs = track_drv_get_lbs_data();
    U8 *sendData, i = 13;
    U16 volt;
    int size;
    kal_int8 ret;

    len += 1/*packet type*/;
    size = 11 + 8/*imei*/ + 2/*ext data len*/ + len/*ext data*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "ERROR: Ram_Alloc");
        return -1;
    }
    memset(sendData, 0, size);

    /* ͸�����ݵĳ��� */
    sendData[i++] = (len & 0xFF00) >> 8;
    sendData[i++] = (len & 0x00FF);

    // packet type
    sendData[i++] = 0x0a;

    memcpy(&sendData[i], data, len);
    i += len;

    track_cust_paket_FD2(sendData, size);
    track_cust_server2_write_req(1, sendData, size,0);
    LOGD(L_APP, L_V5, "");
    Ram_Free(sendData);
    return 0;
    #endif
}
#ifdef __GT420D__
void track_cust_paket_param(kal_uint8 type)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    extern kal_uint8* track_drv_get_420d_mcu_version(void);
    kal_uint8 data[1000] = {0};
    kal_uint8 tmp[200] = {0};
    kal_uint32 len = 2000, l = 0;
    char PRN[5] = {0};
    kal_uint8  i, j, x, sat = 0;
    char GPS_PRN[80] = {0};
    kal_bool head = KAL_TRUE;
    char *ver;

    nvram_GPRS_APN_struct *use_apn = track_socket_get_use_apn();
    charge_measure_struct charge_measure = {0};
    ver = track_get_app_version();
    if (strlen(ver) <= 0)
    {
        ver = (char*)release_verno();
    }
    l += OTA_snprintf(data + l, len - l, "IMEI:%s;", (char *)track_drv_get_imei(0));

    l += OTA_snprintf(data + l, len - l, "VERSION:%s,RTL8762:%s;", ver,track_drv_get_420d_mcu_version());

    l += OTA_snprintf(data + l, len - l, "SERVER:%s,%d;",track_domain_decode(track_nvram_alone_parameter_read()->server.url),track_nvram_alone_parameter_read()->server.server_port);

    l += OTA_snprintf(data + l, len - l, "%s\r\n", track_soc_get_IP());



    l += OTA_snprintf(data + l, len - l, "APN:%s,%d;", use_apn->apn, !track_nvram_alone_parameter_read()->apn.custom_apn_valid);

    l += OTA_snprintf(data + l, len - l, "CSQ:%d;",  track_cust_get_rssi(-1));


    l += OTA_snprintf(data + l, len - l, "GPRS:%d;", track_nvram_alone_parameter_read()->gprson);


    l += OTA_snprintf(data + l, len - l, "BAT:%f;", ((float)track_cust_get_real_battery_volt())/1000000);
    l += OTA_snprintf(data + l, len - l, "ICCID:%s;", OTA_ell_get_iccid());

    l += OTA_snprintf(data + l, len - l, "IMSI:%s;", track_drv_get_imsi(0));
    l += OTA_snprintf(data + l, len - l, "STSN:%d;",track_cust_module_get_stsn());
    l += OTA_snprintf(data + l, len - l, "STELE:%d;", track_cust_module_get_ele_data());
    l += OTA_snprintf(data + l, len - l, "RST:%d;", track_cust_module_get_rst());
    l += OTA_snprintf(data + l, len - l, "STWAKE:%d;",track_cust_modeule_get_wakemode());
    l += OTA_snprintf(data + l, len - l, "STTIME:%s;",track_cust_module_get_sttime());
    if (G_parameter.extchip.mode == 1)
    {
        l += OTA_snprintf(data + l, len - l, "MODE=%d,%d;",G_parameter.extchip.mode,G_parameter.extchip.mode1_par1);
    }
    else if(G_parameter.extchip.mode == 2)
    { 
        l += OTA_snprintf(data + l, len - l, "MODE=%d,%d;",G_parameter.extchip.mode,G_parameter.extchip.mode2_par1);
    }
    else
    {
        l += OTA_snprintf(data + l, len - l, "MODE=%d;",G_parameter.extchip.mode);
    }
    #if !defined(__GT420D__)
    if (4 == TRACK_DEFINE_SOS_COUNT)
    {
        l += OTA_snprintf(data + l, len - l, "SOS NUM:%s ,%s,%s ,%s\r\n", G_parameter.sos_num[0], G_parameter.sos_num[1], G_parameter.sos_num[2], G_parameter.sos_num[3]);
    }
    else
    {
        l += OTA_snprintf(data + l, len - l, "SOS NUM:%s ,%s,%s \r\n", G_parameter.sos_num[0], G_parameter.sos_num[1], G_parameter.sos_num[2]);
    }
    #endif
    LOGD(L_APP,L_V5,"AMS ��¼���ظ��Լ����:%s\r\n",data);
    if (type == CM_ONLINE)
    {
        track_cust_paket_94(0x8, data, strlen(data));
    }
    else if (type == CM_AT)
    {
        LOGS(data);
    }
    else if (type == CM_ONLINE2)
    {
        track_cust_net2_param(strlen(data), data);
    }
    #endif
}

#else
void track_cust_paket_param(kal_uint8 type)
{
    kal_uint8 data[1000] = {0};
    kal_uint8 tmp[200] = {0};
    kal_uint32 len = 2000, l = 0;
    char PRN[5] = {0};
    kal_uint8  i, j, x, sat = 0;
    char GPS_PRN[80] = {0};
    track_gps_data_struct *gd = track_drv_get_gps_data();
    kal_bool head = KAL_TRUE;
    char *ver;
    nvram_GPRS_APN_struct *use_apn = track_socket_get_use_apn();
    charge_measure_struct charge_measure = {0};
    track_bmt_get_ADC_result(&charge_measure);
    ver = track_get_app_version();
    if (strlen(ver) <= 0)
    {
        ver = (char*)release_verno();
    }

    l += OTA_snprintf(data + l, len - l, "IMEI:%s\r\n", (char *)track_drv_get_imei(0));

    if (strlen(G_importance_parameter_data.version_string) > 0)
    {
        l += OTA_snprintf(data + l, len - l, "VERSION:%s_%s\r\n", ver, G_importance_parameter_data.version_string);
    }
    else
    {
        l += OTA_snprintf(data + l, len - l, "VERSION:%s\r\n", ver);
    }

    if (G_importance_parameter_data.dserver.server_type)
    {
        if (G_importance_parameter_data.dserver.conecttype == 0)
        {
            l += OTA_snprintf(data + l, len - l, "DSERVER:0,%d.%d.%d.%d,%d,%d\r\n",
                              G_importance_parameter_data.dserver.server_ip[0], G_importance_parameter_data.dserver.server_ip[1],
                              G_importance_parameter_data.dserver.server_ip[2], G_importance_parameter_data.dserver.server_ip[3],
                              G_importance_parameter_data.dserver.server_port, G_importance_parameter_data.dserver.soc_type);
        }
        else
        {
            l += OTA_snprintf(data + l, len - l, "DSERVER:%d,%s,%d,%d\r\n",
                              G_importance_parameter_data.dserver.conecttype,
                              track_domain_decode(G_importance_parameter_data.dserver.url),
                              G_importance_parameter_data.dserver.server_port,
                              G_importance_parameter_data.dserver.soc_type);
        }
    }
    else
    {
        if (track_nvram_alone_parameter_read()->server.conecttype == 0)
        {
            l += OTA_snprintf(data + l, len - l, "SERVER:0,%d.%d.%d.%d,%d,%d\r\n",
                              track_nvram_alone_parameter_read()->server.server_ip[0], track_nvram_alone_parameter_read()->server.server_ip[1],
                              track_nvram_alone_parameter_read()->server.server_ip[2], track_nvram_alone_parameter_read()->server.server_ip[3],
                              track_nvram_alone_parameter_read()->server.server_port, track_nvram_alone_parameter_read()->server.soc_type);
        }
        else
        {
            l += OTA_snprintf(data + l, len - l, "SERVER:%d,%s,%d,%d\r\n",
                              track_nvram_alone_parameter_read()->server.conecttype,
                              track_domain_decode(track_nvram_alone_parameter_read()->server.url),
                              track_nvram_alone_parameter_read()->server.server_port,
                              track_nvram_alone_parameter_read()->server.soc_type);
        }
    }
    l += OTA_snprintf(data + l, len - l, "%s\r\n", track_soc_get_IP());



    l += OTA_snprintf(data + l, len - l, "APN:%s,%d\r\n", use_apn->apn, !track_nvram_alone_parameter_read()->apn.custom_apn_valid);

    l += OTA_snprintf(data + l, len - l, "CSQ:%d\r\n",  track_cust_get_rssi(-1));


    l += OTA_snprintf(data + l, len - l, "GPRS:%d\r\n", track_nvram_alone_parameter_read()->gprson);

    for (i = 0, j = 80; i < gd->gpgga.sat && i < MTK_GPS_SV_MAX_NUM; i++)
    {
        for (x = 0; x < MTK_GPS_SV_MAX_NUM; x++)
        {
            if (gd->gpgsv.Satellite_ID[x] == gd->gpgsa.satellite_used[i])
            {
                if (head)
                    snprintf(PRN, 5, "%d", gd->gpgsv.SNR[x]);
                else
                    snprintf(PRN, 5, ",%d", gd->gpgsv.SNR[x]);
                j -= 5;
                strncat(GPS_PRN, PRN, j);
                head = KAL_FALSE;
                break;
            }
            else if (x == MTK_GPS_SV_MAX_NUM - 1)
            {
                LOGD(L_APP, L_V5, "not find:%d", gd->gpgsa.satellite_used[i]);
            }
        }
    }
    i = track_cust_gps_status();
    if (i > 2)i = 1;
    else if (i > 0)i = 2;
    else i = 0;
    l += OTA_snprintf(data + l, len - l, "GPS:%d,%d,%s\r\n", i, gd->gpgga.sat, GPS_PRN);

    l += OTA_snprintf(data + l, len - l, "BAT:%d\r\n", (float)charge_measure.bmt_vBat);


    l += OTA_snprintf(data + l, len - l, "POWER:%d\r\n", (float)track_drv_get_batter_voltage() / 1000000.0);
    if (G_parameter.gps_work.Fti.sw)
    {
        l += OTA_snprintf(data + l, len - l, "TIMER:%d,%d\r\n", G_parameter.gps_work.Fti.interval_acc_on, G_parameter.gps_work.Fti.interval_acc_off);
    }
    if (G_parameter.gps_work.Fd.sw)
    {
        l += OTA_snprintf(data + l, len - l, "DISTANCE:%%d\r\n", G_parameter.gps_work.Fd.distance);
    }
    if (4 == TRACK_DEFINE_SOS_COUNT)
    {
        l += OTA_snprintf(data + l, len - l, "SOS NUM:%s ,%s,%s ,%s\r\n", G_parameter.sos_num[0], G_parameter.sos_num[1], G_parameter.sos_num[2], G_parameter.sos_num[3]);
    }
    else
    {
        l += OTA_snprintf(data + l, len - l, "SOS NUM:%s ,%s,%s \r\n", G_parameter.sos_num[0], G_parameter.sos_num[1], G_parameter.sos_num[2]);

    }
    l += OTA_snprintf(data + l, len - l, "SOS NUM:%s ,%s,%s\r\n", G_parameter.sos_num[0], G_parameter.sos_num[1], G_parameter.sos_num[2]);
    l += OTA_snprintf(data + l, len - l, "FN NUM:%s,%s \r\n", G_phone_number.fn_num[0], G_phone_number.fn_num[1]);

    l += OTA_snprintf(data + l, len - l, "CENTER NUM:%s \r\n", G_parameter.centerNumber);
    l += OTA_snprintf(data + l, len - l, "ICCID:%s \r\n", OTA_ell_get_iccid());
    l += OTA_snprintf(data + l, len - l, "EURL:%s \r\n", G_parameter.url);
    l += OTA_snprintf(data + l, len - l, "BAT TEMP:%d \r\n", charge_measure.bmt_vTemp / 100000);
    if (type == CM_ONLINE)
    {
        track_cust_paket_94(0x8, data, strlen(data));
    }
    else if (type == CM_AT)
    {
        LOGS(data);
    }
    else if (type == CM_ONLINE2)
    {
        track_cust_net2_param(strlen(data), data);
    }
}
#endif
kal_int8 track_cust_paket_volt(U8 type)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    LBS_Multi_Cell_Data_Struct *lbs = track_drv_get_lbs_data();
    U8 *sendData, i = 13;
    U16 volt, len;
    int size;
    kal_int8 ret;

    len = 1/*packet type*/ + 2/*volt*/;
    size = 11 + 8/*imei*/ + 2/*ext data len*/ + len/*ext data*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "ERROR: Ram_Alloc");
        return -1;
    }
    memset(sendData, 0, size);

    /* ͸�����ݵĳ��� */
    sendData[i++] = (len & 0xFF00) >> 8;
    sendData[i++] = (len & 0x00FF);

    // packet type
    sendData[i++] = 0x06;
// volt
#if defined(__GT740__)||defined(__GT420D__)
    if (track_cust_get_real_battery_volt() > 10000)
        volt = track_cust_get_real_battery_volt() / 10000;
    else
        volt = 0;
#else
    if (track_cust_get_ext_volt() > 10000)
        volt = track_cust_get_ext_volt() / 10000;
    else
        volt = 0;
#endif


    sendData[i++] = (volt & 0xFF00) >> 8;
    sendData[i++] = (volt & 0x00FF);

    track_cust_paket_FD2(sendData, size);
    track_cust_server2_write_req(1, sendData, size,0);
    LOGD(L_APP, L_V5, "");
    Ram_Free(sendData);
    return 0;
    #endif
}

kal_int8 track_cust_paket_lbs(void)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    LBS_Multi_Cell_Data_Struct *lbs = track_drv_get_lbs_data();
    U8 *sendData, i = 0;
    U16 no;
    int size;
    kal_int8 ret;

    size = 1/*packet type*/ + 38/*lbs*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "ERROR: Ram_Alloc");
        return -1;
    }
    memset(sendData, 0, size);

    // packet type
    sendData[0] = 0x03;

    // lbs
    DataPackingLbs_38(&sendData[1], lbs);

    track_cust_paket_FD(sendData, size, 0);
    LOGD(L_APP, L_V5, "");
    Ram_Free(sendData);
    return 0;
    #endif
}

void track_cust_paket_FD_cmd(kal_uint8 *serverid, kal_bool isASCII, kal_uint8 *data, int datalen)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData, i = 0;
    U16 no;
    int size;

    if (datalen <= 0 || datalen > 1000)
    {
        LOGD(L_APP, L_V1, "ERROR: outstrlen=%d", datalen);
        return;
    }

    size = 1/*packet type*/ + 4/*server id*/ + 1/*code_page*/ + datalen/*data*/;

    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "ERROR: Ram_Alloc");
        return;
    }
    memset(sendData, 0, size);

    // packet type
    sendData[i++] = 0x04;

    //��������־λ
    memcpy(&sendData[i], serverid, 4);
    i += 4;

    //code_page
    if (isASCII)
    {
        sendData[i++] = 1;
    }
    else
    {
        sendData[i++] = 2;
    }

    //����
    memcpy(&sendData[i], data, datalen);
    i += datalen;

    track_cust_paket_FD(sendData, size, 0);
    LOGD(L_APP, L_V5, "");
    Ram_Free(sendData);
    #endif
}

/******************************************************************************
 *  Function    -  cust_packet_log
 *
 *  Purpose     -  ����������־
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void cust_packet_log(kal_uint8 errorid, kal_uint8 type, kal_uint8 filter, kal_uint16 sn, float _distance, float lat, float lon)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData;
    int size, i;
    applib_time_struct currTime = {0};
    kal_uint32 tmp;
    char tt[10] = {0};
    U16 no;
    track_gps_data_struct * gpsdata = track_drv_get_gps_data();

    applib_dt_get_rtc_time(&currTime);
    size = 49;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);
#if defined(__XCWS__)
#else


    i = 4;
    /* ʱ���� */
    sprintf(tt, "%0.2d%0.2d%0.2d", currTime.nHour, currTime.nMin, currTime.nSec);
    sendData[i++] = ((tt[0] - 0x30) << 4) + (tt[1] - 0x30);
    sendData[i++] = ((tt[2] - 0x30) << 4) + (tt[3] - 0x30);
    sendData[i++] = ((tt[4] - 0x30) << 4) + (tt[5] - 0x30);

    /* ��־���� */
    sendData[i++] = errorid;

    /* λ���ϴ����� */
    sendData[i++] = type;

    /* ���˱�� */
    sendData[i++] = filter;

    /* ��ˮ�� */
    sendData[i++] = ((sn & 0xFF00) >> 8);
    sendData[i++] = (sn & 0x00FF);

    /* GSM�źŵȼ� */
    sendData[i++] = track_cust_get_rssi(-1);

    /* Sensor������һ�μ�⵽�Ĵ��� */
    sendData[i++] = track_cust_sensor_count(3);

    /* ACC */
    sendData[i++] = track_cust_status_acc();

    /* ��ӵ�Դ */
    sendData[i++] = track_cust_status_charger();

    /* GPS��ǰ�Ĺ���״̬ ��/�أ�3D/2D��λ */
    sendData[i++] = track_cust_gps_status();

    /* GPS��ǰ����ʹ�õ������� */
    sendData[i++] = gpsdata->gpgga.sat;

    /* ��ǿ�źŵ�3�����ǵ�ƽ���ź�ֵ */
    sendData[i++] = gpsdata->gpgsa.satellite_SNR_used_average;

    /* GPSˮƽ�������� */
    tmp = gpsdata->gpgsa.HDOP * 100;
    track_fun_reverse_32(&sendData[i], tmp);
    i += 4;

    /* �ٶ� */
    tmp = gpsdata->gprmc.Speed * 100;
    track_fun_reverse_32(&sendData[i], tmp);
    i += 4;

    /* �Ƕ� */
    tmp = gpsdata->gprmc.Course * 100;
    track_fun_reverse_32(&sendData[i], tmp);
    i += 4;

    /* ���� */
    if (_distance > 0)
    {
        tmp = _distance * 100;
    }
    else
    {
        tmp = 0;
    }
    track_fun_reverse_32(&sendData[i], tmp);
    i += 4;

    /* γ�� */
    if (lat > 0)
    {
        track_fun_LongitudeLatitudeToHex(&sendData[i], lat);
    }
    i += 4;

    /* ���� */
    if (lon > 0)
    {
        track_fun_LongitudeLatitudeToHex(&sendData[i], lon);
    }
    i += 4;

    no = DataPacketEncode78(0xFF, sendData, size);
    //no = (sendData[size-5] << 8) + sendData[size-4];
    LOGD(L_APP, L_V6, "no=%d HDOP=%f, speed=%f, Course=%f, distance=%d, lat=%f, lon=%f",
         no, gpsdata->gpgsa.HDOP, gpsdata->gprmc.Speed, gpsdata->gprmc.Course, _distance, lat, lon);
    track_socket_queue_send_log_reg(sendData, size, 0xFF, no);
    Ram_Free(sendData);
#endif
#endif
}

/******************************************************************************
 *  Function    -  cust_packet_log_data
 *
 *  Purpose     -  ����������־���Զ������ݣ�
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void cust_packet_log_data(kal_uint8 type, kal_uint8 *data, kal_uint16 len)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData;
    int size, i;
    applib_time_struct currTime = {0};
    char tt[10] = {0};
    U16 no;

    if (len > 1024 || len == 0) return;
    size = 11 + 5 + len;
    applib_dt_get_rtc_time(&currTime);
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);

    i = 5;
    /* ʱ���� */
    sprintf(tt, "%0.2d%0.2d%0.2d", currTime.nHour, currTime.nMin, currTime.nSec);
    sendData[i++] = ((tt[0] - 0x30) << 4) + (tt[1] - 0x30);
    sendData[i++] = ((tt[2] - 0x30) << 4) + (tt[3] - 0x30);
    sendData[i++] = ((tt[4] - 0x30) << 4) + (tt[5] - 0x30);

    /* ��־���� */
    sendData[i++] = 64;

    /* ��־���� */
    sendData[i++] = type;

    /* �����־���� */
    memcpy(&sendData[i], data, len);

    no = DataPacketEncode79(0xFF, sendData, size);
    LOGD(L_APP, L_V6, "");
    track_socket_queue_send_log_reg(sendData, size, 0xFF, no);
    Ram_Free(sendData);
    #endif
}
void cust_packet_log_data_ext2(kal_uint8 type, char *head, kal_uint8 *data, kal_uint16 len)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 *sendData;
    int size, i, head_len = 0;
    applib_time_struct currTime = {0};
    char tt[10] = {0};
    U16 no;

    if (len > 1024 || len == 0) return;
    if (head != NULL) head_len = strlen(head);
    size = 11 + 5 + len + head_len;
    applib_dt_get_rtc_time(&currTime);
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);

    i = 5;
    /* ʱ���� */
    sprintf(tt, "%0.2d%0.2d%0.2d", currTime.nHour, currTime.nMin, currTime.nSec);
    sendData[i++] = ((tt[0] - 0x30) << 4) + (tt[1] - 0x30);
    sendData[i++] = ((tt[2] - 0x30) << 4) + (tt[3] - 0x30);
    sendData[i++] = ((tt[4] - 0x30) << 4) + (tt[5] - 0x30);

    /* ��־���� */
    sendData[i++] = 64;

    /* ��־���� */
    sendData[i++] = type;

    /* �����־���� */
    if (head_len > 0)
    {
        memcpy(&sendData[i], head, head_len);
        i += head_len;
    }
    memcpy(&sendData[i], data, len);

    no = DataPacketEncode79(0xFF, sendData, size);
    LOGD(L_APP, L_V6, "");
    track_socket_queue_send_log_reg(sendData, size, 0xFF, no);
    Ram_Free(sendData);
    #endif
}

/******************************************************************************
 *  Function    -  track_cust_ram_alarm
 *
 *  Purpose     -  �ڴ��쳣������־
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_ram_alarm(kal_uint32 freesize, kal_uint32 mem_size)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    U8 sendData[30] = {0};
    int size, i;
    applib_time_struct currTime = {0};
    char tt[10] = {0};
    U16 no;

    applib_dt_get_rtc_time(&currTime);
    size = 10 + 3 + 1 + 8;

    i = 4;
    /* ʱ���� */
    sprintf(tt, "%0.2d%0.2d%0.2d", currTime.nHour, currTime.nMin, currTime.nSec);
    sendData[i++] = ((tt[0] - 0x30) << 4) + (tt[1] - 0x30);
    sendData[i++] = ((tt[2] - 0x30) << 4) + (tt[3] - 0x30);
    sendData[i++] = ((tt[4] - 0x30) << 4) + (tt[5] - 0x30);

    /* ��־���� */
    sendData[i++] = 0xFE;

    memcpy(&sendData[i], &freesize, 4);
    i += 4;
    memcpy(&sendData[i], &mem_size, 4);

    no = DataPacketEncode78(0xFF, sendData, size);
    LOGD(L_APP, L_V6, "freesize=%d, mem_size=%d", freesize, mem_size);
    track_socket_queue_send_log_reg(sendData, size, 0xFF, no);
    #endif
}

/******************************************************************************
 *  Function    -  track_cust_paket_position_change_renew
 *
 *  Purpose     -  �ı䱸���еĶ�λ��Ϊ������ʽ
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 05-03-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_paket_position_change_renew(kal_uint8 *data, int len)
{
    #if defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    int size;
    size = 10 + 6/*datatime*/ + 12/*gps*/ + 8/*lbs*/ + 3;
#if defined(__GT740__)
    if (len == size
            && data[0] == 0x78
            && data[1] == 0x78
            && (data[3] == 0x22 || data[3] == 0x2D)
            && data[32] == 0x00)
#elif defined(__NT31__)
    if (data[0] == 0x78
            && data[1] == 0x78
            && data[3] == 0x22
            && data[32] == 0x00)
#elif defined(__GT420D__)
    if (data[0] == 0x78
            && data[1] == 0x78
            && data[3] == 0x37)
#else
    if (len == size
            && data[0] == 0x78
            && data[1] == 0x78
            && data[3] == 0x22
            && data[32] == 0x00)
#endif
    {
        U16 tmp;
        // GPS ʵʱ����
        // 0x00 ʵʱ�ϴ�
        // 0x01 ����
        data[32] = 0x01;

        //���� CRC
        tmp = GetCrc16(&data[2], len - 6);
        data[len - 4] = (tmp & 0xFF00) >> 8;
        data[len - 3] = (tmp & 0x00FF);
        LOGD(L_APP, L_V5, "SerialNumber:%.2X%.2X", data[len - 6], data[len - 5]);
    }
#endif
}

#if defined(__SV_UPLOAD__)
/******************************************************************************
 *  Function    -  packet_gps_block
 *
 *  Purpose     -  ��94��ָ����ʽ���GPS�ź�ǿ������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 22-04-2017,  Cml  written
 * ----------------------------------------
 ******************************************************************************/
static void packet_gps_block(U8 *out, U16 *outlen, gps_gpgsa_struct *gsa, gps_gpgsv_struct *gsv, gps_gpgga_struct *gga)
{
    U8 tmp[MTK_GPS_SV_MAX_NUM + 3][2] = {0};
    int i = 0, d, v, j = 0, vv, m = 0;

    out[i++] = gsa->status;  //0x01 ����  0x02 2D ��λ   0x03 3D ��λ
    for (d = 0; d < MTK_GPS_SV_MAX_NUM; d++)
    {
        for (v = 0; v < MTK_GPS_SV_MAX_NUM; v++)
        {
            if (gsa->satellite_used[d] == 0) break;
            if (gsa->satellite_used[d] == gsv->Satellite_ID[v])
            {
                tmp[j][0] = gsv->Satellite_ID[v];
                tmp[j][1] = gsv->SNR[v];
                //LOGD(L_APP, L_V1, "%d:���붨λ������%d(%d)", j, gsv->Satellite_ID[v], gsv->SNR[v]);
                j++;
                break;
            }
        }
    }
    out[i++] = j;
    vv = i;
    if (j > 0)
    {
        for (d=0; d<j; d++)
        {
            out[i++] = tmp[d][1];
        }
    }
    if (j > 1) track_fun_sort_HighToLow(&out[vv], j);
    vv = i;
    i++;
    for (v = 0; v < MTK_GPS_SV_MAX_NUM; v++)
    {
        U8 flag = 1;
        if (gsv->Satellite_ID[v] == 0) break;
        for (d = 0; d < j; d++)
        {
            if (gsv->Satellite_ID[v] == tmp[d][0])
            {
                flag = 0;
                break;
            }
        }
        if (flag)
        {
            m++;
            out[i++] = gsv->SNR[v];
            //LOGD(L_APP, L_V1, "%d:�����붨λ���ɼ�������%d(%d)", m, gsv->Satellite_ID[v], gsv->SNR[v]);
        }
    }
    out[vv] = m;
    if (m > 1) track_fun_sort_HighToLow(&out[vv+1], m);
    *outlen = i;
}

/******************************************************************************
 *  Function    -  track_cust_paket_9409_handle
 *
 *  Purpose     -
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 14-02-2017,  Cjj  written
 * v2.0  , 22-04-2017,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_paket_9409_handle(void)
{
    kal_uint8 *data, *p;
    U16 len_gps = 0, len_bd = 0, i = 0;
    track_gps_data_struct *gd = track_drv_get_gps_data();

    if (gd == NULL)
    {
        LOGD(L_APP, L_V1, "��GPS����");
        return;
    }

    data = (U8*)Ram_Alloc(5, MTK_GPS_SV_MAX_NUM * 2 + 20);
    if (data == NULL)
    {
        return;
    }

    // GPS
    p = data;
    packet_gps_block(data, &len_gps, &gd->gpgsa, &gd->gpgsv, &gd->gpgga);
    i += len_gps;

    // BeiDou
    packet_gps_block(data + i, &len_bd, &gd->bdgsa, &gd->bdgsv, &gd->bdgga);
    i += len_bd;

    LOGD(L_APP, L_V5, "����%d�����ǵ�ǰ���붨λ��GPS:%d[%d/%d], BeiDou:%d[%d/%d]", gd->gpgga.sat
         , gd->gpgsa.status, data[1], gd->gpgsv.Satellites_In_View
         , gd->bdgsa.status, data[len_gps+1], gd->bdgsv.Satellites_In_View);

    //��չ
    data[i++] = 0x00;
    track_cust_paket_94(9, data, i);
    Ram_Free(data);
}
#endif /* __SV_UPLOAD__ */

/******************************************************************************
 *  Function    -  track_cust_decode_packet_16
 *
 *  Purpose     -  ��Ӧ�������·���0x16���ݰ�����
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * v1.1  , 28-09-2013,  WQ  modify
 * ----------------------------------------
*******************************************************************************/
void track_cust_decode_packet_16(kal_uint8 *data, kal_uint16 len)
{
    LOGD(L_APP, L_V4, "��������Ӧ�ն��ϱ��ı������ݰ���������ַ��");

#if defined( __GT03F__) || defined(__GT300__) || defined(__GW100__)||defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
    track_cust_decode_packet_17(data, len);
    track_soc_send((void*)98);
#endif /* __GT03F__ */
}

/******************************************************************************
 *  Function    -  track_cust_decode_packet_17
 *
 *  Purpose     -  ��Ӧ�������·���0x17���ݰ�����
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * v1.1 , 28-08-2013,   J W  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_decode_packet_17(kal_uint8 *data, kal_uint16 len)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
#ifndef __XCWS__
    kal_uint8 packelen = 0, serverId[4] = {0}, *p, *s;
    kal_uint16 sn = 0, l;
    char *wchr = "&&";               // �ָ��� &&
    char *addr = "ADDRESS", *alarm = "ALARMSMS";
    char tmp[50] = {0}, *number = NULL;
    kal_uint8 check_num[20] = {0x00};

    memset((void *)check_num, 0x00, sizeof((char *)check_num));

    LOGH(L_APP, L_V6, data, len);
    LOGH(L_APP, L_V6, check_num, 20);
    // ָ���
    packelen = data[1];

    // ��������־λ
    memcpy(serverId, &data[2], 4);

    // ��ˮ��
    sn = (data[len - 2] << 8) & 0xFF00;
    sn |= data[len - 1];
    LOGD(L_APP, L_V2, "sn=%d", sn);

    s = (kal_uint8*)strstr((char*)&data[6], wchr);
    if (s == NULL)
    {
        LOGD(L_APP, L_V2, "error 1");
        return;
    }
    if (s - &data[6] >= 50)
    {
        LOGD(L_APP, L_V2, "error 2");
        return;
    }
    memcpy(tmp, &data[6], s - &data[6]);
    s += 2;
    if (!strcmp(tmp, addr) || !strcmp(tmp, alarm))
    {
        p = track_fun_strWchr((U8*)wchr, s, len - (s - data));
        if (p == NULL)
        {
            LOGD(L_APP, L_V2, "error 3");
            return;
        }
        l = p - s;
        number = p + 2;
        LOGH(L_APP, L_V6, number, 28);
        if (strstr(number, "000000000000000000000"))
        {
            number[0] = 0;
        }

        LOGD(L_APP, L_V5, "Number:%s, %0.4X", number, sn);

        if (!strcmp(tmp, addr))
        {
            if (strlen(number) == 0)/*W J*/
            {
                track_cust_module_alarm_recv_addr(number, s, l, sn);
            }
            else
            {
                track_cust_module_recv_addr(number, s, l, sn);
            }
        }
        else if (!strcmp(tmp, alarm))
        {
            track_cust_module_alarm_recv_addr(number, s, l, sn);
        }
    }
    else
    {
        LOGH(L_APP, L_V6, &data[6], len - 6);
    }
#endif
#endif 
}

void track_cust_decode_packet_21(kal_uint8 *data, kal_uint16 len)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    kal_uint8 serverId[4] = {0};
    kal_bool isAscii;
    U8 *sendData;

    memcpy(serverId, &data[1], 4);
    if (data[5] == 1)
    {
        isAscii = KAL_TRUE;
    }
    else
    {
        isAscii = KAL_FALSE;
    }
    sendData = (U8*)Ram_Alloc(5, len);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "Ram_Alloc NULL!!");
        return;
    }
    memcpy(sendData, &data[6], len - 6);
    track_cust_online_cmd(serverId, (char *)sendData);
    Ram_Free(sendData);
    #endif
}

/******************************************************************************
 *  Function    -  track_cust_decode_packet_97
 *
 *  Purpose     -  ��Ӧ�������·���0x17���ݰ�����
 *
 *  Description -
 * 78 78 00 B7 97 00 B0 00 00 00 01 41 44 44 52 45 53 53 26 26 00 50 00 72 00 65 00 63 00 69 00 73 00 65 00 6C 00 79 00 20 00 4C 00 6F 00 63 00 61 00 74 00 69 00 6E 00 67 00 3A 00 68 00 74 00 74 00 70 00 3A 00 2F 00 2F 00 6D 00 61 00 70 00 73 00 2E 00 67 00 6F 00 6F 00 67 00 6C 00 65 00 2E 00 63 00 6F 00 6D 00 2F 00 6D 00 61 00 70 00 73 00 3F 00 71 00 3D 00 32 00 33 00 2E 00 31 00 31 00 31 00 38 00 31 00 35 00 2C 00 31 00 31 00 34 00 2E 00 34 00 30 00 39 00 32 00 39 00 37 26 26 31 32 35 32 30 31 35 38 31 32 35 35 35 37 32 39 00 00 00 00 00 23 23 03 47 B3 28 0D 0A
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * v1.1 , 28-08-2013,  J W  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_decode_packet_97(kal_uint8 *data, kal_uint16 len)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
#ifndef __XCWS__
    kal_uint16 packelen = 0;
    kal_uint8    serverId[4] = {0}, *p, *s;
    char        *wchr = "&&";               // �ָ��� &&
    kal_uint16 sn = 0, l;
    char *addr = {"ADDRESS"}, *alarm = {"ALARMSMS"};
    char tmp[50] = {0}, *number = NULL;
    kal_uint8 check_num[20] = {0x00};
    LOGD(L_APP, L_V4, "��������Ӧ�ն�λ�ò�ѯ���ݰ�");

    memset((void *)check_num, 0x00, sizeof((char *)check_num));

    LOGH(L_APP, L_V6, data, len);
    LOGH(L_APP, L_V6, check_num, 20);
    // ָ���
    packelen = data[1];
    packelen = (packelen << 8) + data[2];
    // ��������־λ
    memcpy(serverId, &data[3], 4);

    // ��ˮ��
    sn = (data[len - 2] << 8) & 0xFF00;
    sn |= data[len - 1];
    LOGD(L_APP, L_V2, "sn=%d", sn);

    s = (kal_uint8*)strstr((char*)&data[7], (char*)wchr);
    if (s == NULL)
    {
        LOGD(L_APP, L_V2, "error 1");
        return;
    }
    if (s - &data[7] >= 50)
    {
        LOGD(L_APP, L_V2, "error 2");
        return;
    }
    memcpy(tmp, &data[7], s - &data[7]);
    s += 2;
    if (!strcmp(tmp, addr) || !strcmp(tmp, alarm))
    {
        p = track_fun_strWchr(wchr, s, len - (s - data));
        if (p == NULL)
        {
            LOGD(L_APP, L_V2, "error 3");
            return;
        }
        l = p - s;
        number = p + 2;
        LOGH(L_APP, L_V6, number, 28);
        if (strstr(number, "000000000000000000000"))
        {
            number[0] = 0;
        }

        LOGD(L_APP, L_V5, "Number:%s, %0.4X", number, sn);

        if (!strcmp(tmp, addr))
        {
            if (strlen(number) == 0)/*W J*/
            {
                track_cust_module_alarm_recv_addr(number, s, l, sn);
            }
            else
            {
                track_cust_module_recv_addr(number, s, l, sn);
            }
        }
        else if (!strcmp(tmp, alarm))
        {
            track_cust_module_alarm_recv_addr(number, s, l, sn);
        }
    }
    else
    {
        LOGH(L_APP, L_V6, &data[6], len - 6);
    }
    track_soc_send((void*)98);
#endif
#endif
}

/******************************************************************************
 *  Function    -  track_cust_decode_packet_80
 *
 *  Purpose     -  ��Ӧ�������·���0x80���ݰ�������ָ�����
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_decode_packet_80(kal_uint8 *data, kal_uint16 len)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
#ifndef __XCWS__   //feizhuliu chengj
    kal_uint8 packelen = 0, serverId[4] = {0};
    char *p = NULL;
    char tmp[256] = {0}, *number = NULL;
    int cmdlen;

    LOGH(L_APP, L_V6, data, len);
    // ָ���
    packelen = data[1];

    // ��������־λ
    memcpy(serverId, &data[2], 4);

    p = strchr((char*)&data[6], '#');
    if (p == NULL)
    {
        LOGD(L_APP, L_V1, "ERROR: Not to '#' end, instruction format wrong!");
        LOGH(L_APP, L_V1, data, len);
        return;
    }
    cmdlen = p - &data[6];
#if !defined(__GT02__)//GT02û�����кź�У��λ
    if (cmdlen > len - 8 || cmdlen > 255)
    {
        LOGD(L_APP, L_V1, "ERROR: cmdlen=%d", cmdlen);
        LOGH(L_APP, L_V1, data, len);
        return;
    }
#endif

    memcpy(tmp, &data[6], cmdlen);
    LOGD(L_APP, L_V4, "�յ�����ָ�%s", tmp);
    track_cmd_net_recv(tmp, serverId);
    #endif 
    #endif
}

void track_cust_decode_packet_8A(kal_uint8 *data, kal_uint16 len)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    static mtk_param_epo_time_cfg epo_time = {0};
    kal_uint8 req_result, i;

    LOGH(L_APP, L_V5, data, len);
    i = 1;
    epo_time.u2YEAR  = 2000 + data[i++];
    epo_time.u1MONTH = data[i++];
    epo_time.u1DAY   = data[i++];
    epo_time.u1HOUR  = data[i++];
    epo_time.u1MIN   = data[i++];
    epo_time.u1SEC   = data[i++];
    LOGD(L_APP, L_V4, "%d-%02d-%02d %02d:%02d:%02d", epo_time.u2YEAR, epo_time.u1MONTH, epo_time.u1DAY, epo_time.u1HOUR, epo_time.u1MIN, epo_time.u1SEC);
    track_fun_update_time_to_system(2, (applib_time_struct *)(&epo_time));
    //track_soc_send((void*)98);
#endif/*__XCWS_TPATH__*/
}

void track_cust_decode_packet_FD_cmd(kal_uint8 *data, kal_uint16 len)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    kal_uint8 serverId[4] = {0};
    kal_bool isAscii;
    U8 *sendData;

    LOGH(L_APP, L_V6, data, len);
    memcpy(serverId, &data[1], 4);
    if (data[5] == 1)
        isAscii = KAL_TRUE;
    else
        isAscii = KAL_FALSE;
    sendData = (U8*)Ram_Alloc(5, len);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "Ram_Alloc NULL!!");
        return;
    }
    memcpy(sendData, &data[6], len - 6);
    LOGD(L_APP, L_V4, "�յ�����ָ�%s", sendData);
    len = strlen(sendData);
    {
        char *start = NULL, *end = NULL;
        kal_bool valid = KAL_FALSE;
        U16 cmd_length = 0;

        track_drv_nvram_write_switch(0);
        start = (char*)sendData;
        do
        {
            end = strchr(start, '#');
            if (end != NULL || strlen(start) > 1)
            {
                if (end != NULL) *end = 0;

                start = track_fun_trim2(start);
                cmd_length = strlen(start);
                if (*start != '#' && cmd_length < CM_PARAM_LONG_LEN_MAX - 2 && cmd_length > 0)
                {
                    CMD_DATA_STRUCT cmd = {0};
                    cmd.return_sms.cm_type = CM_ONLINE2;
                    cmd.rcv_length = cmd_length;
                    memcpy(cmd.return_sms.stamp, serverId, 4);
                    strcpy(cmd.rcv_msg, start);
                    track_command_operate(&cmd);
                    valid = KAL_TRUE;
                }
                start = end + 1;
            }
        }
        while (end != NULL);
        track_drv_nvram_write_switch(1);
        track_cust_server2_write_req(0, NULL, 0,0);
        if (valid)
        {
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
            Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
            Track_nvram_write(NVRAM_EF_PHONE_NUMBER_LID, 1, (void *)&G_phone_number, NVRAM_EF_PHONE_NUMBER_SIZE);
        }
    }
    Ram_Free(sendData);
    #endif
}

void track_cust_decode_lbs_packet(kal_uint8 *data, kal_uint16 len)
{
    #if 0
    defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else
    mtk_param_epo_pos_cfg epo_pos_cfg_from_lbs = {0};
    kal_uint8 *sendData;
    kal_uint8 nw = 0;
    kal_uint16 no;
    int size, i = 1, tmp1, tmp2;
    float lan_temp = 0.0, lon_temp = 0.0;
    track_gps_data_struct gps_data = {0};
    applib_time_struct date = {0};
    track_gps_data_struct *gpsLastPoint = NULL;
    LBS_Multi_Cell_Data_Struct *lbs = track_drv_get_lbs_data();
    static U8 flg = 0;
    gpsLastPoint = track_cust_backup_gps_data(2, NULL);
    if (track_cust_is_upload_UTC_time())
    {
        track_fun_get_time(&date , KAL_TRUE, NULL);
    }
    else
    {
        track_fun_get_time(&date , KAL_FALSE, &G_parameter.zone);
    }
    epo_pos_cfg_from_lbs.u2YEAR = date.nYear;
    epo_pos_cfg_from_lbs.u1MONTH = date.nMonth;
    epo_pos_cfg_from_lbs.u1DAY = date.nDay;
    epo_pos_cfg_from_lbs.u1HOUR = date.nHour;
    epo_pos_cfg_from_lbs.u1MIN = date.nMin  ;
    epo_pos_cfg_from_lbs.u1SEC = date.nSec  ;


    i = 0;
    memcpy(&tmp1, &data[i], 4);
    track_fun_reverse_32((kal_uint8 *)&tmp2, tmp1);
    epo_pos_cfg_from_lbs.dfLAT = tmp2;
    epo_pos_cfg_from_lbs.dfLAT /= 1800000;
    if ((nw & 0x02)) //00000010 ���λΪ����0-�� 1-�� ���ڶ�λ��γ��  0��  1��
    {
        epo_pos_cfg_from_lbs.dfLAT = -epo_pos_cfg_from_lbs.dfLAT;
    }
    i += 4;
    memcpy(&tmp1, &data[i], 4);
    track_fun_reverse_32((kal_uint8 *)&tmp2, tmp1);
    epo_pos_cfg_from_lbs.dfLON = tmp2;
    epo_pos_cfg_from_lbs.dfLON /= 1800000;
    if (nw & 0x01) //00000010 ���λΪ����0-�� 1-�� ���ڶ�λ��γ��  0��  1��
    {
        epo_pos_cfg_from_lbs.dfLON = -epo_pos_cfg_from_lbs.dfLON;
    }
    ///jiangweijun
    memcpy(&gps_data.gprmc.Date_Time, &date, sizeof(date));
    if (!(nw & 0x02))
    {
        gps_data.gprmc.Latitude = epo_pos_cfg_from_lbs.dfLAT;
        gps_data.gprmc.NS = 'N';
    }
    else
    {
        gps_data.gprmc.Latitude = -epo_pos_cfg_from_lbs.dfLAT;
        gps_data.gprmc.NS = 'S';
    }
    if (!(nw & 0x01))
    {
        gps_data.gprmc.Longitude = epo_pos_cfg_from_lbs.dfLON;
        gps_data.gprmc.EW = 'E';

    }
    else
    {
        gps_data.gprmc.Longitude = -epo_pos_cfg_from_lbs.dfLON;
        gps_data.gprmc.EW = 'W';

    }
    gps_data.gprmc.status = 1;

    track_cust_decode_lbs(&gps_data.gprmc.Latitude, &gps_data.gprmc.Longitude);

    //jiangweijun

    LOGD(L_APP, L_V5, "nw:%d, lat:%c%f, lon:%c%f", nw, gps_data.gprmc.NS, epo_pos_cfg_from_lbs.dfLAT, gps_data.gprmc.EW, epo_pos_cfg_from_lbs.dfLON);
    if (epo_pos_cfg_from_lbs.u2YEAR > 2013)
    {
        if (track_cust_gps_status() < 3 && flg == 0)
        {
            flg = 1;
            if (lbs->MainCell_Info.lac != gpsLastPoint->lbs.lac)
            {
                /*if(mmi_Get_PowerOn_Type() == PWRKEYPWRON || CHRPWRON == mmi_Get_PowerOn_Type())
                {
                    track_cust_paket_position(&gps_data, track_drv_get_lbs_data(), TR_CUST_POSITION_LBS, 0);
                }*/
                track_cust_agps_get_last_position(2, &epo_pos_cfg_from_lbs);
            }
        }
    }
    #endif
}

void track_cust_decode_packet_FD(kal_uint8 *data, kal_uint16 len)
{
    //LOGH(L_APP, L_V6, data, len);
    LOGD(L_APP, L_V5, "len:%d", len);
    if (len > 11)
    {
        track_cust_server2_decode_packet(data + 11, len - 11);
    }
}



/******************************************************************************
 *  Function    -  track_cust_check_gprsdata
 *
 *  Purpose     -  ���ڼ��GPS��������ˮ��
 *
 *  Description -  ��Ҫ����GT300 ģʽ3
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 14-01-2014,  ZengPing && Liujw  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint16 track_cust_check_gprsdata(kal_uint32 flag, kal_uint16 sn)
{
    static kal_uint16  gps_sn =  0xffff;
    LOGD(L_APP, L_V5, "flag:%d;sn:%d", flag, sn);
    if (flag == 0)
    {
        track_stop_timer(TRACK_CUST_ALARM_GPS_WORK_TIMER);
        gps_sn = 0xffff;
        track_cust_gps_location_timeout(5);
    }
    else if (flag == 1)
    {
        gps_sn = sn;
        LOGD(L_APP, L_V5, "@1gps_sn:%d;sn=%d", gps_sn, sn);
    }
    else
    {
        LOGD(L_APP, L_V5, "@2gps_sn:%d", gps_sn);
        return gps_sn;
    }
}

/*************************************************/
/************GT02B��Э�����ʼ**************/
/*************************************************/
#ifndef __MQTT__
/******************************************************************************
 *  Function    -  track_cust_paket_gpsdata
 *
 *  Purpose     -  GPS��λ��
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-11-2013,  ZengPing && Liujw  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint16 track_cust_paket_gpsdata(track_gps_data_struct *gd, LBS_Multi_Cell_Data_Struct *lbs)
{

    U8 *sendData;
    U16 no;
    int size;
    U16 temp4 = 0;
    kal_uint16 Course;

    size = 42;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "Ram_Alloc NULL!!");
        return;
    }
    memset(sendData, 0, size);
    //Head
    sendData[0] = 0x68;
    sendData[1] = 0x68;

    //����
    sendData[2] = 24 + 13;
    //HOLD
    sendData[3] = (lbs->MainCell_Info.lac >> 8) & 0x00FF;
    sendData[4] = lbs->MainCell_Info.lac & 0x00FF;


    //�ն�ID
    memcpy(&sendData[5], track_drv_get_imei(1), 8);

    //��Ϣ���к�
    no = getSerialNumber();
    sendData[13] = (no & 0xFF00) >> 8;
    sendData[14] = (no & 0x00FF);

    //Э���
    sendData[15] = 0x10;

    //��λ��Ϣ
    //��λ����
    //����ʱ��
    DataPackingDatetime_6(&sendData[16], gd);

    //γ��
    track_fun_LongitudeLatitudeToHex(&sendData[22], gd->gprmc.Latitude);

    //����
    track_fun_LongitudeLatitudeToHex(&sendData[26], gd->gprmc.Longitude);
    LOGD(L_APP, L_V5, "len:%f,%f",  gd->gprmc.Longitude, gd->gprmc.Latitude);

    //�ٶ�
    sendData[30] = gd->gprmc.Speed;

    //����״̬
    Course = gd->gprmc.Course;
    track_fun_reverse_16(&sendData[31], Course);

    //����λ
    sendData[33] = 0;
    sendData[34] = 0;
    sendData[35] = 0;

    //״̬λ
    sendData[36] = 0;
    sendData[37] = 0;
    sendData[38] = 0;
    if (gd->gprmc.EW == 'E')
        sendData[39] |= 0x04;
    if (gd->gprmc.NS == 'N')
        sendData[39] |= 0x02;
    if (gd->gprmc.status == 1)
        sendData[39] |= 0x01;

    //����
    sendData[40] = 0x0d;
    sendData[41] = 0x0a;

    track_socket_queue_position_send_reg(sendData, size, 0x10, no);
    Ram_Free(sendData);

    return no;
}


void Heartbeat_Data_Packet(kal_uint8* data, kal_uint16 *len, kal_uint16 *sn)
{
    U8 i, k;
    U32 Temp;
    U8 *sendData;
    U16 no;
    int size, ret;

    gps_gpgsv_struct *gpgsv_data = track_drv_get_gps_GSV();
    size = 20 + gpgsv_data->Satellites_In_View;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "Ram_Alloc NULL!!");
        return;
    }
    memset(sendData, 0, size);
    //��ͷ
    sendData[0] = 0x68;
    sendData[1] = 0x68;
    //���ݳ���
    sendData[2] = 15 + gpgsv_data->Satellites_In_View;
    //��ѹ�ȼ�
    sendData[3] = track_cust_get_battery_level();
    //GSM�źŵȼ�
    ret = track_cust_get_rssi_level();
    if (ret >= 1 && ret <= 4)
    {
        sendData[4] = ret;
    }
    else
    {
        sendData[4] = 0;
    }
    //�ն�ID
    memcpy(&sendData[5], track_drv_get_imei(1), 8);

    //��Ϣ���к�
    no = getSerialNumber();
    sendData[13] = (no & 0xFF00) >> 8;
    sendData[14] = (no & 0x00FF);

    //Э���
    sendData[15] = 0x1A;

    //��λ״̬
    if (track_cust_gps_status() == GPS_STAUS_OFF)
    {
        sendData[16] = 0;
    }
    else  if (track_cust_gps_status() >= GPS_STAUS_2D)
    {
        sendData[16] = 1;
    }
    else if (track_cust_gps_status() == GPS_STAUS_ON)
    {
        sendData[16] = 3;
    }
    else  if (track_cust_gps_status() == GPS_STAUS_SCAN)
    {
        sendData[16] = 4;
    }
    sendData[17] = gpgsv_data->Satellites_In_View;
    if (gpgsv_data->Satellites_In_View)
    {
        k = 18;
        for (i = 0; i < gpgsv_data->Satellites_In_View; i++)
        {
            sendData[k] = gpgsv_data->SNR[i];
            k++;
        }
    }
    else
    {
        k = 18;
    }
    sendData[k] = 0x0D;
    k++;
    sendData[k] = 0x0A;



    if (!track_soc_login_status())
    {
        LOGD(L_APP, L_V5, "@1");
        track_soc_write_req(sendData, size, 1, no);
    }
    else
    {
        memcpy(data, sendData, size);
        *len = size;
        *sn = no;
    }
    Ram_Free(sendData);

}
static kal_uint16 DataPacketEncode68(U8 cmd, U8 *sendData, U8 len)
{
    U16 tmp, tmp1;

    //��ʼλ
    sendData[0] = 0x68;
    sendData[1] = 0x68;

    //����
    sendData[2] = len - 5;

    //Э���
    sendData[3] = cmd;

    //��Ϣ���к�
    tmp = getSerialNumber();
    sendData[len - 6] = (tmp & 0xFF00) >> 8;
    sendData[len - 5] = (tmp & 0x00FF);
    LOGD(L_APP, L_V5, "SerialNumber:%.2X%.2X", sendData[len - 6], sendData[len - 5]);

    //CRCУ��
    tmp1 = GetCrc16(&sendData[2], len - 6);
    sendData[len - 4] = (tmp1 & 0xFF00) >> 8;
    sendData[len - 3] = (tmp1 & 0x00FF);

    //����
    sendData[len - 2] = '\r';
    sendData[len - 1] = '\n';

    return tmp;
}

void track_Online_Command_Set_Handle(kal_uint8 * Recbuf, kal_uint16 RecLen)
{
    LOGD(L_APP, L_V5, "");
    track_cust_decode_packet_80(Recbuf, RecLen);
}
void track_cust_paket_1C(kal_uint8 *serverid, kal_bool isASCII, kal_uint8 *outstr, kal_uint8 outstrlen)
{
    U8 *sendData, tmp, i = 4;
    U16 no;
    int size;

    outstrlen = strlen(outstr);
    if (outstrlen <= 0 || outstrlen > 200)
    {
        LOGD(L_APP, L_V1, "ERROR: outstrlen=%d", outstrlen);
        return;
    }
    size = 10 + 1/*command len*/ + 4/*server id*/ + outstrlen + 1/*code page*/; /*content*/// + 2/*language*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);

    //ָ���
    sendData[i++] = outstrlen + 4;

    //��������־λ
    memcpy(&sendData[i], serverid, 4);
    i += 4;
    //code_page
    if (isASCII)
    {
        sendData[i++] = 1;
    }
    else
    {
        sendData[i++] = 2;
    }
    //ָ������
    memcpy(&sendData[i], outstr, outstrlen);
    i += outstrlen;

    no = DataPacketEncode68(0x1C, sendData, size);

    track_socket_queue_send_netcmd_reg(sendData, size, 0x1C, no);
    Ram_Free(sendData);
}

void Server_GPRS_Command(kal_uint8 *serverid, kal_bool isASCII, kal_uint8 *outstr, kal_uint8 outstrlen)
{
    U8 *sendData, tmp, i = 3;
    U16 no;
    int size;

    outstrlen = strlen(outstr);
    if (outstrlen <= 0 || outstrlen > 200)
    {
        LOGD(L_APP, L_V1, "ERROR: outstrlen=%d", outstrlen);
        return;
    }
    size = 2 + 1 + 2 /*����λ*/ + 8/*IMEI*/ + 2/*��Ϣ���к�*/ + 1/*Э���*/ + 1/*command len*/ + 4/*server id*/ + outstrlen + 2; /*content*/ // + 2/*language*/;
    LOGD(L_APP, L_V1, ": size=%d,%d", size, outstrlen);
    LOGD(L_APP, L_V1, ": outstr=%s,%d", outstr, outstrlen);
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);
    //��ʼλ
    sendData[0] = 0x68;
    sendData[1] = 0x68;

    //������
    sendData[2] = outstrlen + 2 + 8 + 2 + 1 + 4 + 1;

    //����λ
    sendData[i++] = 0;
    sendData[i++] = 0;

    //�ն�ID
    // imei
    memcpy(&sendData[i], track_drv_get_imei(1), 8);

    i += 8;

    //��Ϣ���к�
    tmp = getSerialNumber();
    sendData[i++] = (tmp & 0xFF00) >> 8;
    sendData[i++] = (tmp & 0x00FF);
    LOGD(L_APP, L_V5, "SerialNumber:%.2X%.2X", sendData[i - 1], sendData[i - 2]);
    //Э���
    sendData[i++] = 0x1C;

    //ָ���
    sendData[i++] = outstrlen + 4;

    //��������־λ
    memcpy(&sendData[i], serverid, 4);
    i += 4;

    //ָ������
    memcpy(&sendData[i], outstr, outstrlen);

    i += outstrlen;

    //����
    sendData[i++] = 0x0D;
    sendData[i] = 0x0A;
    track_socket_queue_send_netcmd_reg(sendData, size, 0x1C, tmp);
    Ram_Free(sendData);
}

/****************************************
* Function    track_cust_lock_pack_no

* Parameter

* Purpose     ��¼��λ�������к�

* Modification
******************************************/
U16 track_cust_lock_pack_no(int no)
{
    static U16 serial_no = 0;
    LOGD(L_OBD, L_V3, "serial_no %d, no %d", serial_no, no);

    if (no == -1)
    {
        return serial_no;
    }
    else
    {
        serial_no = no;
    }
}

kal_uint16 track_cust_paket_1B(track_gps_data_struct *gd, char *number)
{
    kal_uint8 *sendData;
    kal_uint16 no;
    int size , i = 0;
    kal_uint32 tmp;
    LBS_Multi_Cell_Data_Struct* lbs = NULL;
    kal_uint16 Course;

    LOGD(L_APP, L_V5, "number:%s", number);
    size = 2 + 1 + 2/*LAC*/ + 8/*�ն�ID*/ + 2/*��Ϣ���к�*/ + 1/*Э���*/ + 24/*datatime*//*gps*/ + 21/*number*/ + 1/*language*/ + 2;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return 0;
    }
    memset(sendData, 0, size);
    //��ʼλ
    sendData[i++] = 0x68;
    sendData[i++] = 0x68;

    //������
    sendData[i++] = size - 5;

    //����λ0x0000//����λ��С����
    lbs = track_drv_get_lbs_data();
    sendData[i++] = (lbs->MainCell_Info.lac >> 8) & 0x00FF;
    sendData[i++] = lbs->MainCell_Info.lac & 0x00FF;

    //�ն�ID
    // imei
    memcpy(&sendData[i], track_drv_get_imei(1), 8);
    i += 8;

    //��Ϣ���к�
    tmp = getSerialNumber();
    sendData[i++] = (tmp & 0xFF00) >> 8;
    sendData[i++] = (tmp & 0x00FF);
    LOGD(L_APP, L_V5, "SerialNumber:%.2X%.2X", sendData[i - 1], sendData[i - 2]);

    //Э���
    sendData[i++] = 0x1B;

    //��λ��Ϣ
    if (gd->status >= 2)
    {

        //��λ����
        //����ʱ��
        DataPackingDatetime_6(&sendData[i], gd);
        i += 6;
        //γ��
        track_fun_LongitudeLatitudeToHex(&sendData[i], gd->gprmc.Latitude);
        i += 4;
        //����
        track_fun_LongitudeLatitudeToHex(&sendData[i], gd->gprmc.Longitude);
        i += 4;
        //�ٶ�
        sendData[i++] = gd->gprmc.Speed;

        //����״̬
        Course = gd->gprmc.Course;
        track_fun_reverse_16(&sendData[i], Course);
    }
    i += 2;
    sendData[i++] = lbs->MainCell_Info.mnc;
    sendData[i++] = (lbs->MainCell_Info.cell_id >> 8) & 0x00FF;//��վ��ǰ1�ֽ�
    sendData[i++] = lbs->MainCell_Info.cell_id & 0x00FF;//��վ���1�ֽ�;

    //״̬λ
    sendData[i++] = 0;
    sendData[i++] = 0;
    sendData[i++] = 0;
    if (gd->gprmc.EW == 'E')
        sendData[i] |= 0x04;
    if (gd->gprmc.NS == 'N')
        sendData[i] |= 0x02;
    if (gd->gprmc.status == 1)
        sendData[i] |= 0x01;
    i += 1;
    // number(Max:21)
    snprintf(&sendData[i], 21, number);
    i += 21;
    // language  0x01: ���ģ�0x02: Ӣ��
    //sendData[i++] = 0x00;
    if (G_parameter.lang == 1)
    {
        sendData[i] = 1;
    }
    else
    {
        sendData[i] = 2;
    }
    i += 1;
    sendData[i++] = 0x0D;
    sendData[i] = 0x0A;

    track_socket_queue_alarm_send_reg(sendData, size, 0x1B, tmp);
    Ram_Free(sendData);
    return no;
}
#endif



/******************************************************************************
 *  Function    -  public_packet_handle
 *
 *  Purpose     -  �������������ݹ�������
 *
 *  Description -  NULL
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 06-12-2010,  Cml  written
 * ----------------------------------------
 ******************************************************************************/
static void public_packet_handle(kal_uint8* plbsfulldata, U8 len, kal_uint8 type)
{
#if 0
defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else   
    static kal_uint16 serial = 0;
    kal_uint16 crc;
    U8 i = 0;
    //--��ʼλ--
    plbsfulldata[i++] = 0x78;
    plbsfulldata[i++] = 0x78;

    //--����--
    plbsfulldata[i++] = len - 5;

    //--Э���--
    plbsfulldata[i++] = type;

    i = len - 6;
    // --Ԥ����չλ--

    //--��Ϣ���к�--
    plbsfulldata[i++] = (serial >> 8) & 0x00FF;
    plbsfulldata[i++] = serial & 0x00FF;
    serial++;

    //--CRCУ��--
    crc = GetCrc16((U8 *)&plbsfulldata[2], len - 6);
    plbsfulldata[i++] = (crc >> 8) & 0x00FF;
    plbsfulldata[i++] = crc & 0x00FF;

    //--����--
    plbsfulldata[i++] = 0x0D;
    plbsfulldata[i++] = 0x0A;
    #endif
}
/******************************************************************************
 *  Function    -  LBS_Full_Packet
 *
 *  Purpose     -  ���LBS��Ϣ�����ݰ�
 *
 *  Description -  NULL
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 06-12-2010,  Cml  written
 * ----------------------------------------
 ******************************************************************************/
void LBS_Full_Packet(LBS_Multi_Cell_Data_Struct *pdata, kal_uint8* plbsfulldata, U8 len)
{
    #if 0
defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else   
    kal_uint8 i, j = 0;
    U8 imeiHex[8] = {0};

    // --IMEI NO--
    i = 4;
    //if(!GPS_string2Hex(IMEI_DATA, &plbsfulldata[i], 16))
    //   ASSERT(0);
    memcpy(&plbsfulldata[i], track_drv_get_imei(1), 8);

    // --��Ʒ���--
    i = 12;
    plbsfulldata[i++] = 0x30;
    plbsfulldata[i++] = 0x01;

    // --LBS��Ϣ--
    // MCC
    plbsfulldata[i++] = (pdata->MainCell_Info.mcc >> 8) & 0x00FF;
    plbsfulldata[i++] = pdata->MainCell_Info.mcc & 0x00FF;
    // MNC
    plbsfulldata[i++] = pdata->MainCell_Info.mnc;
    // LAC
    plbsfulldata[i++] = (pdata->MainCell_Info.lac >> 8) & 0x00FF;
    plbsfulldata[i++] = pdata->MainCell_Info.lac & 0x00FF;
    // Cell Id
    i++;
    plbsfulldata[i++] = (pdata->MainCell_Info.cell_id >> 8) & 0x00FF;
    plbsfulldata[i++] = pdata->MainCell_Info.cell_id & 0x00FF;

    // --Ԥ����չλ--

    // �������������
    public_packet_handle(plbsfulldata, len, 0x00);
    #endif
}
/*************************************************/
/************GT02B��Э�������**************/
kal_uint8 track_cust_wifi_packet_option(kal_uint8 sends)
{
    static kal_uint8 send_op = 0;
    if (sends == 0 || sends == 1)
    {
        send_op = sends;
    }
    return send_op;
}
#if defined __CUSTOM_WIFI_FEATURES_SWITCH__
/******************************************************************************
 *  Function    -  track_cust_paket_2C
 *
 *  Purpose     -  GT300S  WIFI��
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 06-05-2016,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint16 track_cust_paket_2C(void)
{
    kal_uint8 *sendData;
    kal_uint16 no;
    int size, i, ret, index, tmp_i, tmp_j;
    kal_uint8 hex_mac[7] = {0};
    track_gps_data_struct *gd = track_cust_gpsdata_alarm();
    LBS_Multi_Cell_Data_Struct *lbs = track_drv_get_lbs_data();
    applib_time_struct currTime = {0};
    static U8 count=0;

    LOGD(L_APP, L_V5, "wifi_status.wifi_sum %d",wifi_status.wifi_sum);
#if defined(__GT740__)||defined(__GT420D__)
    {
        #if !defined(__GT420D__)
        if (track_cust_is_innormal_wakeup()==KAL_TRUE)
        {
            return 0;
        }
        #endif
        track_fun_get_time(&currTime, KAL_FALSE, &G_parameter.zone);

        if (currTime.nYear == (*OTA_DEFAULT_HARDWARE_YEAR + 2000))
        {
            LOGD(L_APP, L_V1, "ע��!!ϵͳʱ��δ����!!");
            track_cust_wifi_packet_option(1);
            return 0;
        }

        if ((lbs->MainCell_Info.cell_id == 0 && count==0) && track_soc_login_status()==KAL_FALSE)
        {
            count=1;
            LOGD(L_APP, L_V5, "����վΪ�գ���ʱ���ϴ�WF! ");
            tr_start_timer(TRACK_CUST_SEND_WF_LIMIT_TIMER_ID, 2*60*1000, track_cust_paket_2C);
            return 0;
        }
    }

#elif defined (__GT370__) || defined (__GT380__)

    if (G_parameter.wifi.sw==0)
    {
        LOGD(L_APP, L_V1, "WIFI OFF");
        return;
    }

    if (wifi_status.wifi_sum==0)
    {
        LOGD(L_APP, L_V1, "�ѵ�WIFI����Ϊ��!!!!");
        return;
    }

    track_fun_get_time(&currTime, KAL_FALSE, &G_parameter.zone);
    if (currTime.nYear == (*OTA_DEFAULT_HARDWARE_YEAR + 2000))
    {
        LOGD(L_APP, L_V1, "ϵͳʱ��δ����!!");
        if (!track_is_timer_run(TRACK_CUST_SNED_WIFI_NO_TIME_DELAY_TIMER_ID))
        {
            tr_start_timer(TRACK_CUST_SNED_WIFI_NO_TIME_DELAY_TIMER_ID, 20*1000, track_cust_paket_2C);
        }
        return;
    }
    else
    {
        if (track_is_timer_run(TRACK_CUST_SNED_WIFI_NO_TIME_DELAY_TIMER_ID))
        {
            tr_stop_timer(TRACK_CUST_SNED_WIFI_NO_TIME_DELAY_TIMER_ID);
        }
    }


#endif
    size = 10 + 6 + 7 * wifi_status.wifi_sum + 46 + 1;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return 0;
    }
    memset(sendData, 0, size);
    // datetime
    DataPackingDatetime_6(&sendData[4], gd);

    DataPackingLbs_46(&sendData[10], lbs);

    index = 56;
    sendData[index] = wifi_status.wifi_sum;
    index++;
    for (i = 0; i < wifi_status.wifi_sum; i++)
    {
        //LOGD(L_APP, L_V5, "");;
        if (strstr(wifi_status.wifi_mac[i], "LAST"))
        {
            LOGD(L_APP, L_V7, "1");
            break;
        }
        else if (wifi_status.wifi_rssi[i] == 0)
        {
            LOGD(L_APP, L_V7, "2");
            continue;
        }
        else
        {
            LOGD(L_APP, L_V7, "3");
            if (strlen(wifi_status.wifi_mac[i]) > 0 && wifi_status.wifi_rssi[i] != 0)
            {
                memcpy(&sendData[index], track_fun_stringtohex(wifi_status.wifi_mac[i], 6), 6);
                index = index + 6;
                sendData[index] = wifi_status.wifi_rssi[i];
                index++;
            }
        }
    }
    no = DataPacketEncode78(0x2C, sendData, size);

    LOGH(L_APP, L_V5, sendData, size);
    //track_socket_queue_alarm_send_reg(sendData, size, 0x2C, no);
#if defined(__GT740__)||defined(__GT420D__)
    track_socket_queue_position_send_reg(sendData, size, 0x2C, no);
#else
    track_socket_queue_send_log_reg(sendData, size, 0x2C, no);
#endif
    Ram_Free(sendData);
    return no;
}

#endif


#if defined (__BAT_TEMP_TO_AMS__)
/*�ɼ�����¶��ϱ�AMS������       --    chengjun  2016-11-16*/

#define  TEMP_LEN    5
#define  TEMP_COUNT  70

kal_int8 track_cust_net2_bat_temperatrue_param(kal_uint16 len, kal_uint8* data)
{
    LBS_Multi_Cell_Data_Struct *lbs = track_drv_get_lbs_data();
    U8 *sendData, i = 13;
    U16 volt, len2;
    int size;
    kal_int8 ret;

    len2 = len + 3/*packet type+temp string len*/;
    size = 11 + 8/*imei*/ + 2/*ext data len*/ + len2/*ext data*/;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        LOGD(L_APP, L_V1, "ERROR: Ram_Alloc");
        return -1;
    }
    memset(sendData, 0, size);

    /* ͸�����ݵĳ��� */
    sendData[i++] = (len2 & 0xFF00) >> 8;
    sendData[i++] = (len2 & 0x00FF);

    // packet type
    sendData[i++] = 0x0F;

    //�¶��ַ�������2���ֽ�
    sendData[i++] = (len >> 8) & 0xFF;
    sendData[i++] = len & 0xFF;

    memcpy(&sendData[i], data, len);

    track_cust_paket_FD2(sendData, size);
    track_cust_server2_write_req(1, sendData, size,0);

    LOGD(L_APP, L_V5, "");
    Ram_Free(sendData);
    return 0;
}


void track_cust_bat_temperatrue_data(void)
{
    char Temperature[TEMP_LEN] = {0};
    kal_int16 temporary = 0;
    int counts = 0;
    charge_measure_struct charge_measure = {0};

    tr_start_timer(TRACK_CUST_TEPERATURE_DATA_TIMER_ID, 2 * 60 * 1000, track_cust_bat_temperatrue_data);
    if (track_cust_status_charger() == 0)
    {
        return;
    }
    track_bmt_get_ADC_result(&charge_measure);
    temporary = (kal_int16)(charge_measure.bmt_vTemp / 1000.00);

    snprintf(Temperature, TEMP_LEN, "%d", temporary);
    counts = G_realtime_data.temp_count;

    memcpy(&G_realtime_data.temperature_data[counts], &Temperature, TEMP_LEN);

    LOGD(L_APP, L_V5, "BatTemp[%d]=%s,%s", counts, Temperature, G_realtime_data.temperature_data[G_realtime_data.temp_count]);
    G_realtime_data.temp_count ++;
    if (G_realtime_data.temp_count >= TEMP_COUNT)
    {
        G_realtime_data.temp_count = 0;
    }

    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
}

void track_cust_send_bat_temperature_data(void)
{
    char temp[1024] = {0};
    int i = 0, n = 0;

    for (i = 0; i < TEMP_COUNT; i++)
    {
        if (strlen(G_realtime_data.temperature_data[i]))
        {
            n += sprintf(temp + n, "%s;", G_realtime_data.temperature_data[i]);
        }
        memset(&G_realtime_data.temperature_data[i], 0, TEMP_LEN);
    }

    LOGD(L_APP, L_V5, "%s,%d,%d", temp, strlen(temp), n);
    if (strlen(temp) <= 0)
    {
        LOGD(L_APP, L_V5, "no bat temperature data to ams");
        return;
    }

    track_cust_net2_bat_temperatrue_param(strlen(temp), temp);
}

#endif /* __BAT_TEMP_TO_AMS__ */


#if defined (__CUST_RS485__)

/*RS485��������͸��       --    chengjun  2017-03-02*/

/******************************************************************************
 *  Function    -  track_cust_decode_packet_9B
 *
 *  Purpose     -  �����������·���͸������
 *
 *  Description -   data �ѽ�ȥ���˰�ͷ ������ CRC ��β��lenΪdata����
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-03-02,  chengjun  written
 * ----------------------------------------
 //79 79 00 1E 9B 01 AA 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 C2 00 C0 01 80 A2 DB 00 0B 79 2B 0D 0A
 //            9B 01 AA 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 C2 00 C0 01 80 A2 DB 00 0B
 ******************************************************************************/
void track_cust_decode_packet_9B(kal_uint8 *data, kal_uint16 len)
{
    kal_uint8 packelen = 0,device;
    char *p = NULL;
    char tmp[256] = {0}, *number = NULL;
    kal_uint16 cmdlen;

    LOGH(L_APP, L_V3, data, len);
    //͸�����ݳ���
    cmdlen = len-4;
    if (cmdlen<16)
    {
        LOGD(L_APP, L_V4, "len error",len);
        return;
    }

    if (data[0]!=0x9B)
    {
        LOGD(L_APP, L_V4, "cmd %02X error",data[4]);
        return;
    }
    device=data[1];

    p=&data[2];

    LOGD(L_APP, L_V4, "�յ�����͸��ָ��");
    track_rs485_write_server_data_to_device(p,cmdlen);
}

/******************************************************************************
 *  Function    -  track_cust_paket_9C
 *
 *  Purpose     -  �յ��������·���9C͸����ִ�к󽫽��������������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-03-02,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_paket_9C(kal_uint8* data, kal_uint16 len, kal_uint8 device)
{
    U8 *sendData;
    U8 tmp;
    U16 no;
    kal_uint16 size, ret;

    LOGD(L_SOC, L_V5, "");
    size = 12+len;
    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);

    sendData[5] = device;
    memcpy(&sendData[6], data, len);

    no = DataPacketEncode79(0x9C, sendData, size);
    track_socket_queue_send_netcmd_reg(sendData, size, 0x9C, no);
    Ram_Free(sendData);
}

#endif /* __CUST_RS485__ */


/******************************************************************************
 *  Function    -  track_cust_paket_77
 *
 *  Purpose     -  �豸ƴ��͸��AMSЭ���
 *
 *  Description -  Э�����ͬʱ�ϱ����ģ��̰������ýӿ�ֻ�ܷ�һ��ģ��̰�
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-03-09,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_paket_77(kal_uint8* data, kal_uint16 len, kal_uint8 device)
{
    #if 0
defined (__MQTT__)
{
    LOGD(L_APP, L_V5, "");
}

#else   
    U8 *sendData;
    int size;
    kal_int8 ret,i=0;

    size = len + 4/*packet type + device type + len2B*/;

    LOGD(L_SOC, L_V5, "0x%02x",device);
    sendData = (U8*)Ram_Alloc(7, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);

    sendData[0] = 0x77;
    sendData[1] = device;
    sendData[2] = (len>>8)&0xFF;
    sendData[3] = len&0xFF;;

    memcpy(&sendData[4], data, len);

    LOGH(L_SOC, L_V7, sendData, size);

    track_cust_paket_FD(sendData, size, 0);

    Ram_Free(sendData);
    #endif
}


#if defined (__LUYING__)
void track_cust_paket_server4_01(void)
{
    kal_uint8 *sendData;
    kal_uint16 no, zone;
    int size;

    if (track_cust_is_upload_UTC_time())
    {
        size = 10 + 8 + 2 + 2;
    }
    else
    {
        size = 10 + 8;
    }

    sendData = (U8*)Ram_Alloc(5, size);
    if (sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);

    // imei
    memcpy(&sendData[4], track_drv_get_imei(1), 8);

    if (track_cust_is_upload_UTC_time())
    {
        // ����ʶ����
#if defined(__GT03A__)  // �����͵繦��
        sendData[12] = 0x10;
        sendData[13] = 0x0A;
#elif defined(__GT03B__)  // �����͵繦��
        sendData[12] = 0x10;
        sendData[13] = 0x0B;
#elif defined(__GT06B__)  // �����͵繦��
        sendData[12] = 0x10;
        sendData[13] = 0x0E;
#elif defined(__NT32__)
        sendData[12] = 0x20;
        sendData[13] = 0x03;
#elif defined(__NT22__)
        sendData[12] = 0x22;
        sendData[13] = 0x03;
#elif defined(__NT31__)
        sendData[12] = 0x20;
        sendData[13] = 0x02;
#elif defined(__ET130__)//�����������
        sendData[12] = 0x13;
        sendData[13] = 0x00;
#elif defined(__NT33__)
        sendData[12] = 0x20;
        sendData[13] = 0x04;
#elif defined(__GT02D__)/*�����͵繦��*/|| defined(__BD300__) || defined(__GT06D__)||defined(__NT36__)
        sendData[12] = 0x20;
        sendData[13] = 0x00;
#elif defined(__ET100__)
        sendData[12] = 0x20;
        sendData[13] = 0x10;
#elif defined(__GT100__)// �����͵繦��
        sendData[12] = 0x20;
        sendData[13] = 0x11;
#elif defined(__GT03F__)
        sendData[12] = 0x20;
        sendData[13] = 0x20;
#elif defined(__GT800__)
        sendData[12] = 0x21;
        sendData[13] = 0x20;
#elif defined(__ET200__) // �����������
        sendData[12] = 0x22;
        sendData[13] = 0x00;
#elif defined(__NT23__)
        sendData[12] = 0x23;
        sendData[13] = 0x00;
#elif defined(__GT530__)
        sendData[12] = 0x53;
        sendData[13] = 0x00;
#elif defined(__GT500__)
        sendData[12] = 0x50;
        sendData[13] = 0x00;
#elif defined(__GT370__)
        sendData[12] = 0x70;
        sendData[13] = 0x03;
#elif defined(__GT380__)
        sendData[12] = 0x30;
        sendData[13] = 0x04;
#elif defined(__GT740__) || defined(__GT420D__)
        if (G_importance_parameter_data.app_version.version==2)
        {
            sendData[12] = 0x70;
            sendData[13] = 0x06;
        }
        else  if (G_importance_parameter_data.app_version.version==3)
        {
            sendData[12] = 0x70;
            sendData[13] = 0x07;
        }
        else
        {
            sendData[12] = 0x70;
            sendData[13] = 0x04;
        }
#elif defined(__GW100__)
        sendData[12] = 0x21;
        sendData[13] = 0x01;
#elif defined(__V20__)
        sendData[12] = 0x21;
        sendData[13] = 0x01;
#elif defined(__ET210__)
        sendData[12] = 0x21;
        sendData[13] = 0x01;
#elif defined(__GT300S__)
        sendData[12] = 0x30;
        sendData[13] = 0x00;
#elif defined(__MT200__)
        sendData[12] = 0x22;
        sendData[13] = 0x05;
#elif defined(__NT36__)
        sendData[12] = 0x22;
        sendData[13] = 0x03;
#elif defined(__NT37__)
        sendData[12] = 0x20;
        sendData[13] = 0x08;
#elif defined(__ET320__)
        sendData[12] = 0x22;
        sendData[13] = 0x04;
#elif defined(__ET310__)
        sendData[12] = 0x22;
        sendData[13] = 0x03;
#elif defined(__ET350__)
        sendData[12] = 0x22;
        sendData[13] = 0x03;
#elif defined(__GT310__)
        sendData[12] = 0x30;
        sendData[13] = 0x01;
#elif defined(__JM81__)
        sendData[12] = 0x70;
        sendData[13] = 0x08;
#elif defined(__HVT001__)
        sendData[12] = 0x01;
        sendData[13] = 0x10;
#else
#error "please define project_null"
#endif /* __ET100__ */

        // ʱ������
        zone = G_parameter.zone.time;
        zone *= 100;
        zone += G_parameter.zone.time_min;
        zone <<= 4;
        if (G_parameter.zone.zone == 'W')
        {
            zone |= (1 << 3);
        }

        if (G_parameter.lang == 1)
        {
            zone |= 0x02;
        }
        else
        {
            zone |= 0x01;
        }

        LOGD(L_APP, L_V5, "%x", zone);
        sendData[14] = (zone >> 8);
        sendData[15] = (zone & 0x00FF);
        LOGD(L_APP, L_V5, "%x,%x", sendData[14], sendData[15]);
    }

    no = DataPacketEncode78(0x01, sendData, size);
    LOGH(L_APP, L_V5, sendData, size);

    Socket_write_req(2, sendData, size, 0, 1, 1, 1);
    Ram_Free(sendData);
}
#endif /* __LUYING__ */

