#if defined (__DASOUCHE__)

/******************************************************************************
 * track_cust_dasouche.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *       定制项目大搜车
 *
 *
 * modification history
 * --------------------
 * v1.0   2018-12-6,  chenjiajun create this file
 *
 ******************************************************************************/

/****************************************************************************
* Include Files                   		包含头文件
*****************************************************************************/
#include "track_drv.h"
//#include "init_public.h"
#include "track_soc_Lcon.h"
#include "track_os_ell.h"
#include "track_cust.h"
#include "track_fun_cJSON.h"
#include "c_RamBuffer.h"
#include "MQTT_main.h"
#include "track_fun_common.h"

/*****************************************************************************
 *  Define					宏定义

*****************************************************************************/

/*****************************************************************************
*  Struct                               数据结构定义
******************************************************************************/
typedef struct
{
    char product_key[20];
    char product_secret[48];
    char vin[48];
    kal_uint16	frequencyLocationValue;//定位上传频率
    kal_uint8	frequencyHeartBeat;//心跳上传频率
    kal_uint32  travelDistance;//里程
    char type[30];
    char requestId[100];
    char extend[100];
    char command[100];
} rev_dasouche_strurt;
/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/
static kal_uint8 dsc_alarm = 0;
static kal_uint8 time_str[16] = {0};
//static kal_uint16 DSC_interval_time = 0xFFFE;
/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/
extern kal_uint8* track_drv_get_imei(kal_uint8 flag);
extern kal_uint8* track_drv_get_imsi(kal_uint8 flag);
extern LBS_Multi_Cell_Data_Struct * track_drv_get_lbs_data(void);
extern void MQTT_main(void);
extern int mqtt_publish(kal_uint16 queue_id,const char* msg, const int length,kal_uint8 topic);
extern product_tag_t product_tag ;
extern nvram_realtime_struct  G_realtime_data;

static void free2(void *arg)
{
    Ram_Free(arg);
}


char* track_LbsCell_Data_to_str(cJSON* root, Multi_Cell_Info_Struct* cell_data)
{
    // 创建JSON Object
    cJSON *fld ;
    kal_uint8 MCC[10] = {0};
    kal_uint8 MNC[10] = {0};
    kal_uint8 LAC[10] = {0};
    kal_uint8 CELLID[10] = {0};
    kal_uint8 RSSI[10] = {0};
    sprintf(MCC,"%d",cell_data->mcc);
    sprintf(MNC,"%d",cell_data->mnc);
    sprintf(LAC,"%d",cell_data->lac);
    sprintf(CELLID,"%d",cell_data->cell_id);
    //sprintf(RSSI,"%d",cell_data->rxlev);
    cJSON_AddItemToArray(root, fld = cJSON_CreateObject());
    // 加入节点（键值对），节点名称为value，节点值为123.4
    cJSON_AddStringToObject(fld, "CellID", CELLID);
    cJSON_AddStringToObject(fld, "LAC", LAC);
    cJSON_AddStringToObject(fld, "MCC", MCC);
    cJSON_AddStringToObject(fld, "MNC", MNC);
    cJSON_AddStringToObject(fld, "RSSI", RSSI);
}

char* track_cust_get_lbs_str(cJSON* root,LBS_Multi_Cell_Data_Struct* lbs)
{
    kal_uint8 i;
    cJSON *lbs_root ;
    cJSON_AddItemToObject(root, "originalLBS", lbs_root = cJSON_CreateArray());
    if (lbs->MainCell_Info.cell_id != 0)
        track_LbsCell_Data_to_str(lbs_root, &lbs->MainCell_Info);
    for (i = 0; i < 4; i++)
    {
            if (lbs->NbrCell_Info[i].cell_id != 0 && lbs->NbrCell_Info[i].lac != 0)
            {
                track_LbsCell_Data_to_str(lbs_root, &lbs->NbrCell_Info[i]);
        }
    }
}

char* track_cust_get_locationsw_str(cJSON* root,Locationsw_Data_Struct* locationsw)
{
    cJSON *locationsw_root ;
    kal_uint8 GPSSW[10] = {0};
    kal_uint8 WIFISW[10] = {0};
    cJSON_AddItemToObject(root, "locateSwitch", locationsw_root = cJSON_CreateObject());
    //track_location_sw_to_str(locationsw_root,locationsw);
    sprintf(GPSSW,"%d",locationsw->GPSSW);
    sprintf(WIFISW,"%d",locationsw->WFSW);
    cJSON_AddNumberToObject(locationsw_root, "LBS", 1);
    cJSON_AddNumberToObject(locationsw_root, "GPS", locationsw->GPSSW);
    cJSON_AddNumberToObject(locationsw_root, "WIFI", locationsw->WFSW);
}

char* track_static_time_to_str(cJSON* root, Statictime_Data_Struct* staticmode,kal_uint8 i)
{
    // 创建JSON Object
    cJSON *fld;
    kal_uint8 staticmode1time[10] = {0};
    kal_uint8 staticmode2time[10] = {0};
    cJSON_AddItemToArray(root, fld = cJSON_CreateObject());
    // 加入节点（键值对），节点名称为value，节点值为123.4
    sprintf(staticmode1time,"%d",staticmode->staticmode1time);
    sprintf(staticmode2time,"%d",staticmode->staticmode2time);
    if(i == 0)
    {
        cJSON_AddNumberToObject(fld, "mode", 0);
        cJSON_AddNumberToObject(fld, "staticTime", staticmode->staticmode1time);
    }
    else if(i == 1)
    {
        cJSON_AddNumberToObject(fld, "mode", 1);
        cJSON_AddNumberToObject(fld, "staticTime", staticmode->staticmode2time);
        }
}

char* track_cust_get_statictime_str(cJSON* root,Statictime_Data_Struct* staticmode)
{
    kal_uint8 i;
    cJSON *statictime_root ;
    cJSON_AddItemToObject(root, "staticCommun", statictime_root = cJSON_CreateArray());
    for (i = 0; i < 2; i++)
    { 
        track_static_time_to_str(statictime_root,staticmode,i);
    }
}

char* track_WifiCell_Data_to_str(cJSON* root, track_wifi_struct* cell_data,kal_uint8 i)
{
    // 创建JSON Object
    cJSON *fld;
    kal_uint8 RSSI[10] = {0};
    sprintf(RSSI,"%d",cell_data->wifi_rssi[i]);
    cJSON_AddItemToArray(root, fld = cJSON_CreateObject());
    // 加入节点（键值对），节点名称为value，节点值为123.4
    cJSON_AddStringToObject(fld, "mac", cell_data->wifi_mac[i]);
    cJSON_AddStringToObject(fld, "power", RSSI);
    //cJSON_AddNumberToObject(fld, "RSSI", -90-tmp);
    //cJSON_AddStringToObject(fld, "MAC", "00:06:f4:c7:5f:e0");
}
char* track_cust_get_wifi_str(cJSON* root,track_wifi_struct*wifi)
{
    kal_uint8 i;
    // 创建JSON Object
    cJSON *wifi_root ;
    cJSON_AddItemToObject(root, "wifi", wifi_root = cJSON_CreateArray());
    //cJSON_AddItemToArray(root, wifi_root = cJSON_CreateArray());
    for (i = 0; i < 4; i++)
    {
        if(wifi->wifi_rssi[i] != 0)
        {
            track_WifiCell_Data_to_str(wifi_root,wifi,i);
            }
    }
}
track_gps_data_struct*  track_get_gps_data(track_gps_data_struct *gd)
{
    static track_gps_data_struct gpsLastPoint ;
    //kal_uint8 gps_status = track_cust_gps_status();
    memcpy(&gpsLastPoint, gd, sizeof(track_gps_data_struct));
    if (gpsLastPoint.gprmc.EW != 'E')
    {
        gpsLastPoint.gprmc.Longitude = -gpsLastPoint.gprmc.Longitude;
    }
    if (gpsLastPoint.gprmc.NS != 'N')
    {
        gpsLastPoint.gprmc.Latitude = -gpsLastPoint.gprmc.Latitude;
    }

    return &gpsLastPoint;
}

kal_uint32 track_cust_get_timestamp(void)
{
    static applib_time_struct tmp = {0};
    kal_uint32 timep;
    OTA_applib_dt_get_rtc_time(&tmp);
    timep = OTA_applib_dt_mytime_2_utc_sec(&tmp, 0);
    return timep;
}
track_cust_dsc_alarm_status_clean(enum_alarm_type alarm_type)
{
    if (alarm_type ==TR_CUST_ALARM_Ext_LowBattery)//终端主电源欠压 001
    {
        dsc_alarm &= 0xFE;//dsc_alarm &= 0xFE 取消
    }
    else if (alarm_type ==TR_CUST_ALARM_PowerFailure)//终端主电源掉电 002 TR_CUST_ALARM_PULL_ALM 拆除报警 004
    {
        dsc_alarm &= 0xF9;//dsc_alarm &= 0xFD;消除断电；dsc_alarm &= 0xFB;消除拔出。合成=F9
    }
    else if (alarm_type ==TR_CUST_ALARM_Speed_Limit)//超速报警 007
    {
        dsc_alarm &= 0xF7;//dsc_alarm &= 0xF7 取消
    }
}
void track_cust_alarm_status_rsp(cJSON* root,enum_alarm_type alarm_type)
{

    kal_uint8 len =0;
    char  alarmInfo[100]={0};
    if (alarm_type ==TR_CUST_ALARM_Ext_LowBattery)//终端主电源欠压 001
    {
        dsc_alarm |= 0x01;//alarm &= 0xFE 取消
    }
    else if (alarm_type ==TR_CUST_ALARM_PowerFailure)//终端主电源掉电 002
    {
        dsc_alarm |= 0x02;//alarm &= 0xFD 取消
    }
    else if (alarm_type ==TR_CUST_ALARM_Speed_Limit)//超速报警 007
    {
        dsc_alarm |= 0x10;//alarm &= 0xF7 取消
    }
    LOGD(L_APP, L_V5, "dsc_alarm(%d),%d",dsc_alarm,alarm_type);
    if (dsc_alarm&0x01)
    {
        len += sprintf(alarmInfo,"%s",DSC_Ext_LowBattery);
        LOGD(L_APP, L_V5, "1alarmInfo(%d):%s",len,alarmInfo);
    }
    if (dsc_alarm&0x02)
    {
        if (len==0)
        {
            len += sprintf(alarmInfo+len,"%s",DSC_PowerFailure);
        }
        else
        {
            len += sprintf(alarmInfo+len,"|%s",DSC_PowerFailure);
        }
        LOGD(L_APP, L_V5, "2alarmInfo(%d):%s",len,alarmInfo);
    }
    if (dsc_alarm&0x04)
    {
        if (len==0)
        {
            len += sprintf(alarmInfo+len,"%s",DSC_PULL_ALM);
        }
        else
        {
            len += sprintf(alarmInfo+len,"|%s",DSC_PULL_ALM);
        }
        LOGD(L_APP, L_V5, "3alarmInfo(%d):%s",len,alarmInfo);
    }
    if (dsc_alarm&0x10)
    {
        if (len==0)
        {
            len += sprintf(alarmInfo+len,"%s",DSC_Speed_Limit);
        }
        else
        {
            len += sprintf(alarmInfo+len,"|%s",DSC_Speed_Limit);
        }
        LOGD(L_APP, L_V5, "4alarmInfo(%d):%s",len,alarmInfo);
    }

    if (alarm_type ==TR_CUST_ALARM_Vibration)//震动报警 009
    {
        if (len==0)
        {
            len += sprintf(alarmInfo+len,"%s",DSC_Vibration);
        }
        else
        {
            len += sprintf(alarmInfo+len,"|%s",DSC_Vibration);
        }
        LOGD(L_APP, L_V5, "5alarmInfo(%d):%s",len,alarmInfo);
    }
    else
    {
        LOGD(L_APP, L_V5, "无报警信息");

    }
    LOGD(L_APP, L_V5, "alarmInfo(%d):%s",len,alarmInfo);
    cJSON_AddStringToObject(root, "alarmInfo", alarmInfo);
}
char* track_cust_stamp_rsp(char* stamp)
{
    static char data[100] ={0};
    if (stamp==NULL || strlen(stamp)==0)
    {
        return data;
    }
    LOGD(L_GPS, L_V4, "请求ID%s", data);
    strcpy(data,stamp);

}

kal_uint8 track_cust_get_battery_percent()
{
    kal_uint8 v_percent[66] = {1,3,6,8,11,13,15,18,20,23,25,27,30,32,34,36,39,41,43,45,48,50,51,53,54,56,57,59,60,62,63,65,66,68,69,71,72,74,75,77,80,82,85,87,89,92,94,97,99,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100};
    U32 v_level[66] = {355,356,357,358,359,360, 361, 362, 363, 364, 365, 366, 367,368, 369, 370, 371, 372, 373, 374,375, 376, 377, 378,379,\
                       380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,400,401,402,403,404,405,406,407,408,409,410,411,412,413,414,415,416,417,418,419,420};
    kal_uint16 volt = 0;
    int i = 0,j = 0;
    volt = track_cust_MCUrefer_adc_data(99);
    LOGD(L_APP,L_V5,"%d",volt);
    
        for (i; i<66; i++)
        {
            LOGD(L_APP,L_V7,"%d,%d",v_level[i],volt);
            if (v_level[i] == volt )
            {
                if (G_realtime_data.last_percent == 0)
                {
                    G_realtime_data.last_percent = v_percent[i];
                    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                    LOGD(L_APP,L_V7,"1返回%d",v_percent[i]);
                    return v_percent[i];
                    break;
                }
                else
                {
                    if (track_cust_refer_charge_data(99) == 1)
                    {
                        if (v_percent[i] < G_realtime_data.last_percent)
                        {
                            LOGD(L_APP,L_V7,"2返回%d",G_realtime_data.last_percent);
                            return G_realtime_data.last_percent;
                            break;
                        }
                        else
                        {
                            G_realtime_data.last_percent = v_percent[i];
                            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                            LOGD(L_APP,L_V7,"3返回%d",v_percent[i]);
                            return v_percent[i];
                            break;
                        }
                    }
                    if (track_cust_refer_charge_data(99) == 0)
                    {
                        if(G_parameter.extchip.mode == 2)
                        {
                            if (v_percent[i] > G_realtime_data.last_percent)
                            {
                                LOGD(L_APP,L_V7,"4返回%d",G_realtime_data.last_percent);
                                return G_realtime_data.last_percent;
                                break;
                                }
                            else
                            {
                                LOGD(L_APP,L_V7,"5返回%d",v_percent[i]);
                                G_realtime_data.last_percent = v_percent[i];
                                Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                                return v_percent[i];
                                break;
                                }
                       }
                       //#if 0
                        else
                        {
                            for (j; j<66; j++)
                                {
                                    if(G_realtime_data.last_percent == v_percent[j])
                                        {
                                            if(volt > v_level[j])
                                            {
                                                if(volt - v_level[j] < 3)
                                                {
                                                    LOGD(L_APP,L_V7,"10返回%d",G_realtime_data.last_percent);
                                                    return G_realtime_data.last_percent;
                                                    break;
                                                    }
                                                else
                                                {
                                                    LOGD(L_APP,L_V7,"12返回%d",v_percent[i]);
                                                    G_realtime_data.last_percent = v_percent[i];
                                                    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                                                    return v_percent[i];
                                                    break;
                                                    }
                                                }
                                            else
                                            {
                                                if(v_level[j] - volt < 3)
                                                {
                                                    LOGD(L_APP,L_V7,"10返回%d",G_realtime_data.last_percent);
                                                    return G_realtime_data.last_percent;
                                                    break;
                                                    }
                                                else
                                                {
                                                    LOGD(L_APP,L_V7,"13返回%d",v_percent[i]);
                                                    G_realtime_data.last_percent = v_percent[i];
                                                    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                                                    return v_percent[i];
                                                    break;
                                                    }
                                                }
                                            }
                                    else
                                        {
                                            LOGD(L_APP,L_V7,"11返回%d",v_percent[i]);
                                            G_realtime_data.last_percent = v_percent[i];
                                            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
                                            return v_percent[i];
                                            break;
                                            }
                                        }
                            }
                        //#endif
                    }
                }
            }
        }
    if (volt >= 404 )
    {
        LOGD(L_APP,L_V5,"1");
        if (track_cust_refer_charge_data(99) == 1)
        {
            G_realtime_data.last_percent = 100;
            }
        else
            {
                //#if 0
                if(G_parameter.extchip.mode == 1)
                {
                    G_realtime_data.last_percent = 100;
                    }
                //#endif
                LOGD(L_APP,L_V7,"6返回%d",G_realtime_data.last_percent);
                return G_realtime_data.last_percent;
                }
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        LOGD(L_APP,L_V7,"7");
        return 100;
    }
    else if (volt < 355)
    {
        LOGD(L_APP,L_V5,"0");
        if (track_cust_refer_charge_data(99) == 1)
        {
            LOGD(L_APP,L_V7,"8返回%d",G_realtime_data.last_percent);
            return G_realtime_data.last_percent;
            }
        else
        {
            G_realtime_data.last_percent = 0;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        }
        LOGD(L_APP,L_V7,"9");
        return 0;
    }
}

char* track_battery_Data_to_str(cJSON* root)
{
    extern kal_uint8 track_cust_get_battery_percent();
    // 创建JSON Object
    cJSON *fld ;
    kal_uint8 battery = 0;
    float battery_percent = 0;
    battery = track_cust_get_battery_percent();
    battery_percent = (((float)battery)/100);
    //cJSON_AddItemToObject(root, fld = cJSON_CreateObject());
    // 加入节点（键值对），节点名称为value，节点值为123.4
    cJSON_AddNumberToObject(root, "battery", battery_percent);
    cJSON_AddNumberToObject(root, "created", 155858576);
}

char* track_cust_get_battery_str(cJSON* root)
{
    kal_uint8 i;
    cJSON *data_root ;
    cJSON_AddItemToObject(root, "data", data_root = cJSON_CreateObject());
    track_battery_Data_to_str(data_root);
}

int track_cust_send_battery_alarm_msg(int arg)
{
    // 创建JSON Object
    char *out = NULL;
    char *data = NULL;
    int rc;
    cJSON *root = cJSON_CreateObject();
    data = (char*)Ram_Alloc(3, 800);
    memset(data, 0x00, 800);
    if (data == NULL)
    {
        LOGD(L_APP, L_V1, "Ram_Alloc NULL!!");
        return;
    }
    if (arg == 1)
    {
        cJSON_AddNumberToObject(root, "code", 10004);
    }
    cJSON_AddStringToObject(root, "imei", (char*)track_drv_get_imei(0));
    track_cust_get_battery_str(root);//

    // 打印JSON数据包
    out = cJSON_PrintUnformatted(root);
    strcpy(data, out);
    LOGD(L_APP, L_V5, "");
    cJSON_Delete(root);
    free2(out);
    if (mqtt_publish(PACKET_ALARM_DATA,data, strlen(data),DSC_TOPIC_ACCEPTOR) >= 0)
    {
        LOGD(L_APP,L_V5,"发送低电报警CODE包成功");
        tr_stop_timer(TRACK_CUST_SEND_LOWBATALM_DELAY_TIMER_ID);
    }
    else
    {
        LOGD(L_APP,L_V5,"发送低电报警CODE包失败，10s后重发");
        track_start_timer(TRACK_CUST_SEND_LOWBATALM_DELAY_TIMER_ID,10*1000,track_cust_send_battery_alarm_msg,(void*)1);
    }
    Ram_Free(data);
    return rc;
}


char* track_light_Data_to_str(cJSON* root)
{
    // 创建JSON Object
    cJSON *fld ;
    kal_uint8 light_status;
    light_status = track_cust_refer_lightsensor_data(99);
    //cJSON_AddItemToObject(root, fld = cJSON_CreateObject());
    // 加入节点（键值对），节点名称为value，节点值为123.4
    cJSON_AddNumberToObject(root, "lightSensor", light_status);
    cJSON_AddNumberToObject(root, "created", 155858576);
}

char* track_cust_get_light_str(cJSON* root)
{
    kal_uint8 i;
    cJSON *data_root ;
    cJSON_AddItemToObject(root, "data", data_root = cJSON_CreateObject());
    track_light_Data_to_str(data_root);
}

char* track_iccid_Data_to_str(cJSON* root)
{
    // 创建JSON Object
    cJSON *fld ;
    //cJSON_AddItemToObject(root, fld = cJSON_CreateObject());
    // 加入节点（键值对），节点名称为value，节点值为123.4
    cJSON_AddStringToObject(root, "iccid", (char *)OTA_ell_get_iccid());
    cJSON_AddNumberToObject(root, "created", 155858576);
}

char* track_cust_get_iccid_str(cJSON* root)
{
    kal_uint8 i;
    cJSON *data_root ;
    cJSON_AddItemToObject(root, "data", data_root = cJSON_CreateObject());
    track_iccid_Data_to_str(data_root);
}

int track_cust_send_tear_dark_alarm_msg(int arg)
{
    // 创建JSON Object
    char *out = NULL;
    char *data = NULL;
    int rc;
    cJSON *root = cJSON_CreateObject();
    data = (char*)Ram_Alloc(3, 800);
    memset(data, 0x00, 800);
    if (data == NULL)
    {
        LOGD(L_APP, L_V1, "Ram_Alloc NULL!!");
        return;
    }
    if (arg == 1)
    {
        cJSON_AddNumberToObject(root, "code", 10003);
    }
    cJSON_AddStringToObject(root, "imei", (char*)track_drv_get_imei(0));
    track_cust_get_light_str(root);//

    // 打印JSON数据包
    out = cJSON_PrintUnformatted(root);
    strcpy(data, out);
    LOGD(L_APP, L_V5, "");
    cJSON_Delete(root);
    free2(out);
    if (mqtt_publish(PACKET_ALARM_DATA,data, strlen(data),DSC_TOPIC_ACCEPTOR) >= 0)
    {
        LOGD(L_APP,L_V5,"发送有无光报警CODE包成功");
        tr_stop_timer(TRACK_CUST_SEND_TEARDOWN_DELAY_TIMER_ID);
    }
    else
    {
        LOGD(L_APP,L_V5,"发送有无光报警CODE包失败，10S后重发");
        track_start_timer(TRACK_CUST_SEND_TEARDOWN_DELAY_TIMER_ID,10*1000,track_cust_send_tear_dark_alarm_msg,(void*)1);
    }
    Ram_Free(data);
    return rc;
}

int track_cust_send_simalarm_msg(int arg)
{
    // 创建JSON Object
    char *out = NULL;
    char *data = NULL;
    int rc;
    cJSON *root = cJSON_CreateObject();
    data = (char*)Ram_Alloc(3, 800);
    memset(data, 0x00, 800);
    if (data == NULL)
    {
        LOGD(L_APP, L_V1, "Ram_Alloc NULL!!");
        return;
    }
    if (arg == 1)
    {
        cJSON_AddNumberToObject(root, "code", 10003);
    }
    if (arg == 2)
    {
        LOGD(L_APP,L_V5,"上报换卡信息");
        cJSON_AddNumberToObject(root, "code", 10002);
    }
    cJSON_AddStringToObject(root, "imei", (char*)track_drv_get_imei(0));
    track_cust_get_iccid_str(root);//

    // 打印JSON数据包
    out = cJSON_PrintUnformatted(root);
    strcpy(data, out);
    LOGD(L_APP, L_V5, "");
    cJSON_Delete(root);
    free2(out);
    if (mqtt_publish(PACKET_ALARM_DATA,data, strlen(data),DSC_TOPIC_ACCEPTOR) >= 0)
    {
        LOGD(L_APP,L_V5,"发送换卡报警CODE包成功");
        tr_stop_timer(TRACK_CUST_SEND_SIMALM_DELAY_TIMER_ID);
    }
    else
    {
        LOGD(L_APP,L_V5,"发送换卡报警CODE包失败，10S后重发");
        track_start_timer(TRACK_CUST_SEND_SIMALM_DELAY_TIMER_ID,10*1000,track_cust_send_simalarm_msg,(void*)2);
    }
    Ram_Free(data);
    return rc;
}

int track_cust_poweron_msg()
{
    // 创建JSON Object
    char *out = NULL;
    char *data = NULL;
    int rc;
    cJSON *root = cJSON_CreateObject();
    data = (char*)Ram_Alloc(3, 800);
    memset(data, 0x00, 800);
    if (data == NULL)
    {
        LOGD(L_APP, L_V1, "Ram_Alloc NULL!!");
        return;
    }
    LOGD(L_APP,L_V5,"上报开机信息");
    cJSON_AddNumberToObject(root, "code", 10001);
    cJSON_AddStringToObject(root, "imei", (char*)track_drv_get_imei(0));
    track_cust_get_iccid_str(root);//

    // 打印JSON数据包
    out = cJSON_PrintUnformatted(root);
    strcpy(data, out);
    LOGD(L_APP, L_V5, "");
    cJSON_Delete(root);
    free2(out);
    if (mqtt_publish(PACKET_ALARM_DATA,data, strlen(data),DSC_TOPIC_ACCEPTOR) >= 0)
    {
        LOGD(L_APP,L_V5,"发送开机报警CODE包成功");
        tr_stop_timer(TRACK_CUST_SEND_POWERON_DELAY_TIMER_ID);
    }
    else
    {
        LOGD(L_APP,L_V5,"发送开机报警CODE包失败，重发");
        //tr_start_timer(TRACK_CUST_SEND_POWERON_DELAY_TIMER_ID,10*1000,track_cust_poweron_msg);
    }
    Ram_Free(data);
    return rc;
}

//定位数据包and报警包
int track_cust_location(kal_uint16 queue_id,kal_uint8 *location_data)
{
    char buf1[100] = {0};
    applib_time_struct currTime_ams = {0};
    // 创建JSON Object
    char *out = NULL;
    char *data = NULL;
    kal_uint8 light_status = 0,battery = 0,charge_Status = 0,lightSensorStatus = 0,mode = 0,runStatus = 0,alarm_type = 0;
    kal_uint16 mode_time = 0;
    float battery_percent = 0,temp = 0,humidity = 0;
    cJSON *root = cJSON_CreateObject();
    dasouche_location_struct location ={0};
    track_gps_data_struct last_gd ={0};
    track_gps_data_struct *gd =&last_gd;
    int rc;
    kal_uint8 MCC[10] = {0};
    kal_uint8 MNC[10] = {0};
    kal_uint8 LAC[10] = {0};
    kal_uint8 CELLID[10] = {0};
    kal_uint8 RSSI[10] = {0};
    int csq = 0;
    kal_uint8 buf[40] = {0};
    applib_time_struct time = {0};
    float volt = 0;
    kal_uint32 volt_u32 = 0;

    
    csq = track_cust_get_rssi(-1);
    //从队列里获取大搜车的定位数据包结构体内容
    data = (char*)Ram_Alloc(3, 2048);
    memset(data, 0x00, 2048);
    if (data == NULL)
    {
        LOGD(L_APP, L_V1, "Ram_Alloc NULL!!");
        return;
    }
    memcpy(&location,&location_data[4],sizeof(dasouche_location_struct));

    track_fun_get_time(&time, KAL_TRUE, NULL);
    memcpy(&time,&location.locationtime,sizeof(applib_time_struct));
    sprintf(buf,"%0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",time.nYear,time.nMonth,time.nDay,time.nHour,time.nMin,time.nSec);
    
    sprintf(MCC,"%d",location.lbs.MainCell_Info.mcc);
    sprintf(MNC,"%d",location.lbs.MainCell_Info.mnc);
    sprintf(LAC,"%d",location.lbs.MainCell_Info.lac);
    sprintf(CELLID,"%d",location.lbs.MainCell_Info.cell_id);
    sprintf(RSSI,"%d",location.lbs.MainCell_Info.rxlev);

    battery_percent = (((float)location.battery)/100);
    LOGD(L_APP,L_V5,"location.battery:%d,battery_percent:%0.2f",location.battery,battery_percent);

    LOGD(L_APP, L_V1, "location_type=%d,dataDesc=%d,alarm_type=%d",location.location_type,location.dataDesc,location.alarm_type);
    //定位状态
    gd->gprmc.status = location.status;
    //经纬度
    gd->gprmc.Latitude =location.Latitude;
    gd->gprmc.Longitude =location.Longitude;
    //东西经
    gd->gprmc.EW = location.EW;
    //南北纬
    gd->gprmc.NS = location.NS;
    //速度
    gd->gprmc.Speed = location.Speed;

    alarm_type = location.alarm_type;
    //方向
    gd->gprmc.Course = location.Course;
    //有光无光状态
    light_status = location.light_status;
    //温度
    temp = location.temp;
    //湿度
    humidity = location.humidity;
    //充电状态
    charge_Status = location.charge_Status;
    //光感开关
    lightSensorStatus = location.lightSensorStatus;
    //模式时间
    mode_time = location.mode_time;
    //模式
    mode = location.mode;
    //运动静止状态
    runStatus = location.runStatus;
    //电压
    volt_u32 = location.volt;
    volt = ((float)volt_u32)/100.0;
    
    cJSON_AddStringToObject(root, "imei", (char*)track_drv_get_imei(0));
    if(location.alarm_type == 1 || location.alarm_type == 2 || location.alarm_type == 6)
    {
        cJSON_AddStringToObject(root, "iccid", (char *)OTA_ell_get_iccid());
        }
    cJSON_AddNumberToObject(root, "chargerState", charge_Status);
    cJSON_AddNumberToObject(root, "battery", battery_percent);
    //cJSON_AddNumberToObject(root, "battery", (((float)track_cust_MCUrefer_adc_data(99))/100));
    // 加入节点（键值对），节点名称为value，节点值为123.4
    cJSON_AddStringToObject(root, "MTK_VER", "3.0.3");
    cJSON_AddStringToObject(root, "MCU_VER", "3.0.2");
    cJSON_AddNumberToObject(root, "alarmType", alarm_type);
    
    if ((gd->gprmc.status == 1 || location.location_type == 1) || \
        ((location.alarm_type == 6 || location.alarm_type == 5) && G_realtime_data.last_location_status ==1))//
    {
        cJSON_AddNumberToObject(root, "longitude", track_get_gps_data(gd)->gprmc.Longitude);
        cJSON_AddNumberToObject(root, "latitude", track_get_gps_data(gd)->gprmc.Latitude);

        if (gd->gprmc.EW == 69 )
        {
            cJSON_AddStringToObject(root, "dir_longitude", "E");
        }
        else
        {
            cJSON_AddStringToObject(root, "dir_longitude", "W");
        }

        if (gd->gprmc.NS == 78)
        {
            cJSON_AddStringToObject(root, "dir_latitude", "N");
        }
        else
        {
            cJSON_AddStringToObject(root, "dir_latitude", "S");
        }

        cJSON_AddNumberToObject(root, "direction", (kal_uint16)(track_get_gps_data(gd)->gprmc.Course));
        if(track_cust_gt420d_carstatus_data(99) == 2)
        {
            cJSON_AddNumberToObject(root, "speed", 0);
            }
        else
        {
        cJSON_AddNumberToObject(root, "speed", track_get_gps_data(gd)->gprmc.Speed);
        }
        cJSON_AddNumberToObject(root, "altitude", location.altitude);
    }
    
    cJSON_AddNumberToObject(root, "lightSensorStatus", lightSensorStatus);

    if(location.upload_type == 6)
    {
    cJSON_AddNumberToObject(root, "lightSensor", 1);
    }
    else if(location.upload_type == 7)
    {
        cJSON_AddNumberToObject(root, "lightSensor", 0);
        }
    else
    {
    cJSON_AddNumberToObject(root, "lightSensor", light_status);
    }
    cJSON_AddNumberToObject(root, "humitureStatus", location.humitureStatus);
    if (G_parameter.humiture_sw == 1)
    {
        cJSON_AddNumberToObject(root, "temperature", temp);
        cJSON_AddNumberToObject(root, "humidity", humidity);
    }
    cJSON_AddStringToObject(root, "MCC", MCC);
    cJSON_AddStringToObject(root, "MNC", MNC);
    cJSON_AddStringToObject(root, "LAC", LAC);
    cJSON_AddStringToObject(root, "CellID", CELLID);
    cJSON_AddStringToObject(root, "RSSI", RSSI);

    if (gd->gprmc.status != 1)
    {
        if (location.location_type == 2)
        {
            track_cust_get_lbs_str(root,&location.lbs);//LBS
        }
        if (location.location_type == 6)
        {
#if defined(__WIFI__)
            track_cust_get_wifi_str(root,&location.wifi);//wifi
#endif
        }
    }
    cJSON_AddNumberToObject(root, "GSM", csq);

    cJSON_AddNumberToObject(root, "mode", location.mode);
    cJSON_AddNumberToObject(root, "reportedRate", location.mode_time);

    track_cust_get_statictime_str(root,&location.staticmode);
    #if 0
    if(location.mode == 0)
    {
        cJSON_AddNumberToObject(root, "staticTime", location.staticmode1time);
    }
    else if(location.mode == 1)
    {
        cJSON_AddNumberToObject(root, "staticTime", location.staticmode2time);
    }
    #endif
    cJSON_AddNumberToObject(root, "locateType", location.location_type);
    cJSON_AddNumberToObject(root, "runStatus", runStatus);
    cJSON_AddStringToObject(root, "deviceTime", (char*)buf);
    cJSON_AddNumberToObject(root, "vendor", 3);
    cJSON_AddNumberToObject(root, "uploadtype", location.upload_type);
    track_cust_get_locationsw_str(root,&location.locationsw);
    cJSON_AddNumberToObject(root, "volt", volt);
    cJSON_AddNumberToObject(root, "LOWBATFLAG", G_parameter.lowbat_status);
    // 打印JSON数据包
    out = cJSON_PrintUnformatted(root);
    strcpy(data, out);
    LOGD(L_APP, L_V5, "");
    LOGD(L_APP, L_V5, "len:%d",strlen(data));
    cJSON_Delete(root);
    free2(out);
    rc = mqtt_publish(queue_id,data, strlen(data),DSC_TOPIC_LOCATIN);
    Ram_Free(data);
    #if defined(__GT420D__)
                track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                snprintf(buf1, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d push out,%d,%d,%d\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec,
                            location.location_type,
                            location.upload_type,
                            location.alarm_type
                            );
        track_cust_save_logfile(buf1, strlen(buf1));
    #endif
    return rc;
}

void track_cust_Param_data_to_str(cJSON* root)
{
    // 创建JSON Object
    cJSON *fld ;
    cJSON_AddItemToArray(root, fld = cJSON_CreateObject());
    // 加入节点（键值对），节点名称为value，节点值为123.4
    cJSON_AddStringToObject(fld, "productKey", G_importance_parameter_data.mqtt_login.product_key);
    cJSON_AddStringToObject(fld, "productSecret", G_importance_parameter_data.mqtt_login.product_secret);
}

void track_cust_command_rsp(char*requestId,kal_bool issucess,char* msg,kal_uint8 len,track_gps_data_struct *gpsLastPoint)
{
    // 创建JSON Object
    char *out ;
    static  char data[1000] = {0};
    //char location_data[1000] = {0};
    cJSON *root = cJSON_CreateObject();
    cJSON *param_root ;
    LBS_Multi_Cell_Data_Struct *lbs = track_drv_get_lbs_data();
    cJSON_AddStringToObject(root, "id", requestId);

    cJSON_AddStringToObject(root, "data", msg);

    // 打印JSON数据包
    out = cJSON_PrintUnformatted(root);
    memset(data, 0x00, 1000);
    strcpy(data, out);
    LOGD(L_APP, L_V5, "out:%d//%s",strlen(out),out);
    LOGD(L_APP, L_V5, "data:%d//%s",strlen(data),data);
    cJSON_Delete(root);
    free2(out);
    mqtt_publish(0,data, strlen(data),DSC_TOPIC_RESPONSE);

}
void track_cust_command(rev_dasouche_strurt * rev_dasouche_data)
{
    kal_uint16 tmp;
    kal_uint8 iscommand =0;
    kal_uint8 isreconet =0;
    char cmd[256] = {0};
    if (strlen(rev_dasouche_data->command)!=0)
    {
        iscommand =1;
        sprintf(cmd,"%s",rev_dasouche_data->command);
    }


    if (iscommand)
    {
        track_cmd_mqtt_recv(cmd,rev_dasouche_data->requestId);
    }
    else
    {
        track_cust_command_rsp(rev_dasouche_data->requestId,KAL_TRUE,NULL,0,NULL);
    }
}
void track_cust_mqtt_rev_data(char* data)
{
    char *out ;
    cJSON *json, *json_value, *json_timestamp;
    rev_dasouche_strurt  rev_dasouche_data = {0};
    char data2[200];

    json = cJSON_Parse(data);
    if (!json)
    {
        sprintf(data2, "Error before: [%s]\n", cJSON_GetErrorPtr());
    }
    else
    {
        doit(data);
        json_value = cJSON_GetObjectItem(json, "id");
        if (json_value->type == cJSON_String)
        {
            //rev_dasouche_data.requestId = json_value->valuestring;
            sprintf(rev_dasouche_data.requestId, "%s", json_value->valuestring);
            LOGD(L_APP, L_V5, "requestId:%s\r\n", rev_dasouche_data.requestId);
        }

        json_value = cJSON_GetObjectItem(json, "cmd");
        if (json_value->type == cJSON_String)
        {
            // 从valueint中获得结果
            sprintf(rev_dasouche_data.command, "%s", json_value->valuestring);
            LOGD(L_APP, L_V5, "cmd:%s\r\n", json_value->valuestring);
        }
        else
        {
            LOGD(L_APP, L_V5, "无cmd");
        }
    }
    cJSON_Delete(json);
    track_cust_command(&rev_dasouche_data);

}

void track_cust_iot_rev_data(char* data)
{
    char *p;
    char *start_p;
    char *end_p;
    //获取productSecret
    p = strstr(data, "productSecret");
    if (p == NULL)
    {
        LOGD(L_APP, L_V5, "[LOCKER] error");
        return;
    }
    LOGD(L_APP, L_V9, "p=%s", p);
    start_p = strchr(p, ':');
    end_p =strchr(p, ',');
    //{productSecret\":\"kGb0YfNTZOxnp66H\",\"deviceSecret\":\"kSpLeP2XIcJVZRGW4geOCXCwJu18c9Wn\",\"productKey\":\"a1tYEVEJ3kI\",\"deviceName\":\"868120184368576\"}",}
    memcpy(G_importance_parameter_data.mqtt_login.product_secret,start_p+3,(end_p-start_p-5));
    LOGD(L_APP, L_V6, "productSecret=%s", G_importance_parameter_data.mqtt_login.product_secret);
    //获取deviceSecret
    p = strstr(data, "deviceSecret");
    if (p == NULL)
    {
        LOGD(L_APP, L_V5, "[LOCKER] error");
        return;
    }
    LOGD(L_APP, L_V9, "p=%s", p);
    start_p = strchr(p, ':');
    end_p =strchr(p, ',');
    memcpy(G_importance_parameter_data.mqtt_login.device_secret,start_p+3,(end_p-start_p-5));
    LOGD(L_APP, L_V6, "deviceSecret=%s", G_importance_parameter_data.mqtt_login.device_secret);

    //获取productKey
    p = strstr(data, "productKey");
    if (p == NULL)
    {
        LOGD(L_APP, L_V5, "[LOCKER] error");
        return;
    }
    LOGD(L_APP, L_V9, "p=%s", p);
    start_p = strchr(p, ':');
    end_p =strchr(p, ',');
    memcpy(G_importance_parameter_data.mqtt_login.product_key,start_p+3,(end_p-start_p-5));
    LOGD(L_APP, L_V6, "productKey=%s", G_importance_parameter_data.mqtt_login.product_key);

    //获取deviceName
    p = strstr(data, "deviceName");
    if (p == NULL)
    {
        LOGD(L_APP, L_V5, "[LOCKER] error");
        return;
    }
    LOGD(L_APP, L_V9, "p=%s", p);
    start_p = strchr(p, ':');
    end_p =strchr(p, '}');
    memcpy(G_importance_parameter_data.mqtt_login.device_name,start_p+3,(end_p-start_p-5));
    LOGD(L_APP, L_V6, "deviceName=%s", G_importance_parameter_data.mqtt_login.device_name);
    LOGD(L_APP, L_V5, "productSecret=%s,deviceSecret=%s,productKey=%s,deviceName=%s",
         G_importance_parameter_data.mqtt_login.product_secret,
         G_importance_parameter_data.mqtt_login.device_secret,
         G_importance_parameter_data.mqtt_login.product_key,
         G_importance_parameter_data.mqtt_login.device_name);
    Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
    get_device_info();
}

#endif /* __DASOUCHE__ */

