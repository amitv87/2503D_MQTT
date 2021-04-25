#if defined (__XYBB__)
/*******************************************************************************************
 * Filename:
 * Author :
 * Date :
 * Comment:
 ******************************************************************************************/
/****************************************************************************
*  Include Files                        包含头文件
*****************************************************************************/
#include "track_cust.h"
#include "track_cust_paket_bb.h"
/*****************************************************************************
*  Define                               宏定义
******************************************************************************/

/*****************************************************************************
*  Typedef  Enum                        枚举定义
*****************************************************************************/

/*****************************************************************************
*  Struct                               数据结构定义
******************************************************************************/

/*****************************************************************************
*  Local variable                       局部变量
*****************************************************************************/
static kal_uint16 g_SerialNumber = 0;
static kal_bool Login_01_Information_Success = KAL_FALSE;
static kal_bool Login_02_Information_Success = KAL_FALSE;
static kal_uint16 no_alarm_0200 = 0;

/****************************************************************************
*  Global Variable                      全局变量
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/
extern void track_soc_send(void *par);
extern void track_cust_login_rev(void);
extern void track_cust_heartbeat_rev(void);

/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/
/******************************************************************************
 *  Function    -  track_cust_rev_8100_data
 *
 *  Purpose     -  登录回复
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-17,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_rev_8100_data(nvram_bb_7edata_struct *data)
{

    kal_uint16 no = 0, result = 0, rev_cmd = 0, g_nvram_par = 0, g_nvram_impor = 0;
    kal_uint8 buffer[512];
    memset(buffer, 0, sizeof(buffer));
    LOGH(L_APP, L_V5, data->buf, data->buf_len);

    LOGD(L_APP, L_V5, "登录回复");
    no = (data->buf[0] << 8) & 0xFF00;
    no |= data->buf[1];
    result = data->buf[2];
    if(data->buf_len > 3)
    {
        memcpy(buffer, &data->buf[3], data->buf_len - 3);
        strcpy(G_parameter.bb_data.authorize_code, buffer);
    }

    LOGD(L_APP, L_V5, "no = %04x,result = %d", no, result);
    LOGD(L_APP, L_V5, "buffer= %s", buffer);
    LOGH(L_APP, L_V5, &data->buf[3], data->buf_len - 3);
    LOGH(L_APP, L_V5, buffer, data->buf_len - 3);
    Login_01_Information_Success =	KAL_TRUE;
    packet_login_bb((void *)2);
    g_nvram_par = 1;

    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
}
/******************************************************************************
 *  Function    -  track_alarmdata_remove
 *
 *  Purpose     -  清除报警回复必须清除的报警标志位
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-24,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
static void track_alarmdata_remove(void)
{
    LOGD(L_APP, L_V5, "");
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_SOS);//sos报警
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_Vibration);//震动报警
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_PRE_REMOVE);//拆除报警
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_InFence);//进围栏报警
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_OutFence);//出围栏报警
    track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_displacement);//位移报警
}
/******************************************************************************
 *  Function    -  track_cust_rev_8001_data
 *
 *  Purpose     -  平台通用应答
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-17,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_rev_8001_data(nvram_bb_7edata_struct *data)
{
    kal_uint16 no = 0, result = 0, rev_cmd = 0, g_nvram_par = 0, g_nvram_impor = 0;
    kal_uint8 buffer[512];
    memset(buffer, 0, sizeof(buffer));
    LOGH(L_APP, L_V5, data->buf, data->buf_len);

    no = (data->buf[0] << 8) & 0xFF00;
    no |= data->buf[1];
    rev_cmd = (data->buf[2] << 8) & 0xFF00;
    rev_cmd |= data->buf[3];
    result = data->buf[4];
    LOGD(L_APP, L_V5, "no = %04x,rev_cmd = %04x,result = %d", no, rev_cmd, result);
    if(rev_cmd == 0x0002)
    {
        LOGD(L_APP, L_V5, "心跳回复");
        track_cust_heartbeat_rev();
    }
    else if(rev_cmd ==	0x0102)
    {

        LOGD(L_APP, L_V5, "鉴权回复");
        Login_02_Information_Success =	KAL_TRUE;
        track_cust_login_rev();
    }
    else if(rev_cmd == 0x0200)
    {
        LOGD(L_APP, L_V5, "位置数据包回复");
        if(no_alarm_0200 == no)
        {
            track_alarmdata_remove();
			no_alarm_0200 = 0;
        }
    }
}

/******************************************************************************
 *  Function    -  track_cust_rev_8201_data
 *
 *  Purpose     -  位置信息查询
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-17,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_rev_8201_data(nvram_bb_7edata_struct *data)
{

    kal_uint16 no = 0, result = 0, rev_cmd = 0, g_nvram_par = 0, g_nvram_impor = 0;
    kal_uint8 buffer[512];

    track_gps_data_struct *gpsPoint = NULL;

    memset(buffer, 0, sizeof(buffer));
    LOGH(L_APP, L_V5, data->buf, data->buf_len);
    LOGD(L_APP, L_V5, "位置信息查询");

    if(track_cust_gps_status() >= GPS_STAUS_2D)
    {
        gpsPoint = track_drv_get_gps_data();
    }

    track_cust_paket_0201(gpsPoint, track_drv_get_lbs_data(), data->rev_serialnumber);
}
/******************************************************************************
 *  Function    -  track_cust_rev_8300_data
 *
 *  Purpose     -  文本信息下发
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-17,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_rev_8300_data(nvram_bb_7edata_struct *data)
{

    kal_uint16 no = 0, result = 0, rev_cmd = 0, g_nvram_par = 0, g_nvram_impor = 0;
    kal_uint8 buffer[512];
    memset(buffer, 0, sizeof(buffer));
    LOGH(L_APP, L_V5, data->buf, data->buf_len);
    LOGD(L_APP, L_V5, "文本信息下发");
}
/******************************************************************************
 *  Function    -  track_cust_rev_8103_data
 *
 *  Purpose     -  设置终端参数
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-17,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/

static void track_set_8103_data(kal_uint32 cmd, kal_uint8 *buf, kal_uint16 buf_len)
{
    kal_uint32 time = 0;
    nvram_importance_parameter_struct imp = {0};
    kal_uint32 g_error = 0;
    kal_uint16 g_gipd = 0, g_reconnect = 0;
    nvram_GPRS_APN_struct apn = {0};
    memcpy(&apn, &track_nvram_alone_parameter_read()->apn, sizeof(nvram_GPRS_APN_struct));
    
    LOGD(L_APP, L_V5, "cmd  = %04x", cmd);
    LOGH(L_APP, L_V5, buf, buf_len);

    memcpy(&imp, &G_importance_parameter_data, sizeof(nvram_importance_parameter_struct));
    if(cmd == 0x0001)
    {
        time = (buf[0] << 24) & 0xFF000000;
        time |= (buf[1] << 16) ;
        time |= (buf[2] << 8) ;
        time |= buf[3];
        LOGD(L_APP, L_V5, "time = %d", time);
        G_parameter.hbt_acc_on = time;
    }
    else if(cmd ==  0x0008)
    {
        time = (buf[0] << 24) & 0xFF000000;
        time |= (buf[1] << 16) ;
        time |= (buf[2] << 8) ;
        time |= buf[3];
        LOGD(L_APP, L_V5, "time = %d", time);
        G_parameter.hbt_acc_off = time;
    }
    else if(cmd ==  0x0010)
    {
        LOGD(L_APP, L_V5, "buf = %s", buf);
        apn.custom_apn_valid = 1;
        strcpy(apn.apn, buf);
        nvram_alone_parameter_cmd.apn_set(&apn);
        g_gipd = 1;
    }
    else if(cmd ==  0x0011)
    {
        LOGD(L_APP, L_V5, "buf = %s", buf);
        strcpy(apn.username, buf);
        nvram_alone_parameter_cmd.apn_set(&apn);
        g_gipd = 1;
    }
    else if(cmd ==  0x0012)
    {
        LOGD(L_APP, L_V5, "buf = %s", buf);
        strcpy(apn.password, buf);
        nvram_alone_parameter_cmd.apn_set(&apn);
        g_gipd = 1;
    }
    else if(cmd ==  0x0013)
    {
        LOGD(L_APP, L_V5, "buf = %s", buf);
        if(track_fun_check_ip(buf))
        {
            imp.server.conecttype = 0;
            memset(imp.server.server_ip, 0, 4);
            if(!track_fun_asciiIP2Hex(buf, (void*)&imp.server.server_ip[0]))
            {
                g_error = 0x0013;
            }
            imp.server.url[0] = 0;
        }
        else
        {
            imp.server.conecttype = 1;
            track_domain_encode(buf, imp.server.url);
            memset(imp.server.server_ip, 0, 4);
            memcpy(G_realtime_data.server_ip, imp.server.server_ip, 4);
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        }
        memcpy(&G_importance_parameter_data, &imp, sizeof(nvram_importance_parameter_struct));
        g_gipd = 1;
        g_reconnect = 1;
    }
    else if(cmd ==  0x0018)
    {
        nvram_server_addr_struct server = {0};
        memcpy(&server,&track_nvram_alone_parameter_read()->server,sizeof(nvram_server_addr_struct));
        time = (buf[0] << 24) & 0xFF000000;
        time |= (buf[1] << 16) ;
        time |= (buf[2] << 8) ;
        time |= buf[3];
        LOGD(L_APP, L_V5, "time = %d", time);
        server.server_port = time;
        g_gipd = 1;
        g_reconnect = 1;
        nvram_alone_parameter_cmd.server_ams_set(&server);
    }
    else if(cmd ==  0x0020)
    {
        time = (buf[0] << 24) & 0xFF000000;
        time |= (buf[1] << 16) ;
        time |= (buf[2] << 8) ;
        time |= buf[3];
        LOGD(L_APP, L_V5, "time = %d", time);
        if(time == 2)
        {
            G_parameter.gps_work.Fti.sw = 1;
            G_parameter.gps_work.Fd.sw = 1;
        }
        else if(time == 1)
        {
            G_parameter.gps_work.Fti.sw = 1;
        }
        else
        {
            G_parameter.gps_work.Fd.sw = 1;
        }
    }
    else if(cmd ==  0x0027)
    {
        time = (buf[0] << 24) & 0xFF000000;
        time |= (buf[1] << 16) ;
        time |= (buf[2] << 8) ;
        time |= buf[3];
        LOGD(L_APP, L_V5, "time = %d", time);
        G_parameter.gps_work.Fti.interval_acc_off = time;
    }
    else if(cmd ==  0x0029)
    {
        time = (buf[0] << 24) & 0xFF000000;
        time |= (buf[1] << 16) ;
        time |= (buf[2] << 8) ;
        time |= buf[3];
        LOGD(L_APP, L_V5, "time = %d", time);
        G_parameter.gps_work.Fti.interval_acc_on = time;
    }
    else if(cmd ==  0x002c)
    {
        time = (buf[0] << 24) & 0xFF000000;
        time |= (buf[1] << 16) ;
        time |= (buf[2] << 8) ;
        time |= buf[3];
        LOGD(L_APP, L_V5, "time = %d", time);
        G_parameter.gps_work.Fd.distance = time;
    }
    else if(cmd ==  0x0030)
    {
        time = (buf[0] << 24) & 0xFF000000;
        time |= (buf[1] << 16) ;
        time |= (buf[2] << 8) ;
        time |= buf[3];
        LOGD(L_APP, L_V5, "time = %d", time);
        G_parameter.gps_work.anglerep.angleOfDeflection = time;
    }
    else if(cmd ==  0x0031)
    {
        time = (buf[0] << 24) & 0xFF000000;
        time |= (buf[1] << 16) ;
        time |= (buf[2] << 8) ;
        time |= buf[3];
        LOGD(L_APP, L_V5, "time = %d", time);
        G_parameter.fence[0].radius = time / 100;
    }
    else if(cmd ==  0x0050)
    {
        time = (buf[0] << 24) & 0xFF000000;
        time |= (buf[1] << 16) ;
        time |= (buf[2] << 8) ;
        time |= buf[3];
        LOGD(L_APP, L_V5, "time = %d", time);
    }
    else if(cmd ==  0x0055)
    {
        time = (buf[0] << 24) & 0xFF000000;
        time |= (buf[1] << 16) ;
        time |= (buf[2] << 8) ;
        time |= buf[3];
        LOGD(L_APP, L_V5, "time = %d", time);
        G_parameter.speed_limit_alarm.threshold = time;
    }
    else if(cmd ==  0x0056)
    {
        time = (buf[0] << 24) & 0xFF000000;
        time |= (buf[1] << 16) ;
        time |= (buf[2] << 8) ;
        time |= buf[3];
        LOGD(L_APP, L_V5, "time = %d", time);
        G_parameter.speed_limit_alarm.delay_time = time;
    }
    else if(cmd ==  0x0080)
    {
        time = (buf[0] << 24) & 0xFF000000;
        time |= (buf[1] << 16) ;
        time |= (buf[2] << 8) ;
        time |= buf[3];
        LOGD(L_APP, L_V5, "time = %d", time);
    }
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    if(g_gipd == 1)
    {
        Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
    }

    if(g_reconnect == 1)
    {
        track_soc_reset_reconnect_count2();
        track_os_intoTaskMsgQueueExt(track_soc_gprs_reconnect, (void*)22);//修改域名后，重新建立连接
        //track_os_intoTaskMsgQueue(track_cust_write_sim_backup_for_parameter);
    }
}
static void track_cust_rev_8103_data(nvram_bb_7edata_struct *data)
{
    int size = 0, i = 0, len = 0, index = 0;
    kal_uint8 buffer[250], buf[100];
    kal_uint32 alarm_data = 0, id = 0, num = 0;
    kal_uint32  time = 0;
    int tmp, ret;

    memset(buffer, 0, sizeof(buffer));
    LOGH(L_APP, L_V5, data->buf, data->buf_len);
    LOGD(L_APP, L_V5, "设置终端参数");
    //总数
    index = data->buf[i++];
    LOGD(L_APP, L_V5, "index = %d", index);
    for(; i < data->buf_len;)
    {
        id = (data->buf[i++] << 24) & 0xFF000000;
        id |= (data->buf[i++] << 16) ;
        id |= (data->buf[i++] << 8) ;
        id |= data->buf[i++];

        len = data->buf[i++];

        memcpy(buffer, &data->buf[i], len);

        LOGD(L_APP, L_V5, "id = %04X,len = %d", id, len);
        LOGH(L_APP, L_V5, buffer, len);
        i = i + len;
        track_set_8103_data(id, buffer, len);
    }

    track_cust_paket_0001(data, 0);
}
/******************************************************************************
 *  Function    -  track_cust_rev_8104_data
 *
 *  Purpose     -  查询终端参数
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-17,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_rev_8104_data(nvram_bb_7edata_struct *data)
{

    kal_uint16 no = 0, result = 0, rev_cmd = 0, g_nvram_par = 0, g_nvram_impor = 0;
    kal_uint8 buffer[512];
    memset(buffer, 0, sizeof(buffer));
    LOGH(L_APP, L_V5, data->buf, data->buf_len);
    LOGD(L_APP, L_V5, "查询终端参数");
    track_cust_paket_0104(data->rev_serialnumber);


}
/******************************************************************************
 *  Function    -  track_cust_rev_8500_data
 *
 *  Purpose     -  车辆控制
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-17,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_rev_8500_data(nvram_bb_7edata_struct *data)
{

    kal_uint16 no = 0, result = 0, rev_cmd = 0, g_nvram_par = 0, g_nvram_impor = 0;
    kal_uint8 buffer[512];
    memset(buffer, 0, sizeof(buffer));
    LOGH(L_APP, L_V5, data->buf, data->buf_len);

    LOGD(L_APP, L_V5, "车辆控制");
    result = data->buf[0] >> 6;
    if(result == 1)
    {
        LOGD(L_APP, L_V5, "回复油电");
    }
    else if(result == 2)
    {
        LOGD(L_APP, L_V5, "断开油电");
    }

}
/******************************************************************************
 *  Function    -  track_cust_rev_8600_data
 *
 *  Purpose     -  设置圆形区域
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-17,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_rev_8600_data(nvram_bb_7edata_struct *data)
{

    kal_uint16 no = 0, result = 0, rev_cmd = 0, set_type = 0, total = 0, status = 0;
    kal_uint16 g_gprstype = 0, g_smstype = 0;
    kal_uint32 local_id = 0, radius = 0, Latitude = 0, Longitude = 0;
    kal_uint8 buffer[512];
    nvram_electronic_fence_struct fence = {0};
    memset(buffer, 0, sizeof(buffer));
    LOGH(L_APP, L_V5, data->buf, data->buf_len);



    memcpy(&fence, &G_parameter.fence[0], sizeof(nvram_electronic_fence_struct));
    LOGD(L_APP, L_V5, "设置圆形区域");
    //设置属性
    set_type = data->buf[0];
    LOGD(L_APP, L_V5, "set_type = %02x", set_type);
    //区域总数
    total = data->buf[1];
    LOGD(L_APP, L_V5, "total = %02x", total);
    /************区域项****************/

    //区域ID
    LOGD(L_APP, L_V5, "@# %x,%x,%x,%x", data->buf[2], data->buf[3], data->buf[4], data->buf[5]);

    local_id = (data->buf[2] << 24) & 0xFF000000;
    local_id |= (data->buf[3] << 16) ;
    local_id |= (data->buf[4] << 8) ;
    local_id |= data->buf[5];
    LOGD(L_APP, L_V5, "local_id = %08x", local_id);
    //区域属性

    status = (data->buf[6] << 8) & 0xFF00;
    status |= data->buf[7];

    LOGD(L_APP, L_V5, "status = %04x", status);
    //纬度Latitude

    Latitude = (data->buf[8] << 24) & 0xFF000000;
    Latitude |= (data->buf[9] << 16) ;
    Latitude |= (data->buf[10] << 8) ;
    Latitude |= data->buf[11];
    LOGD(L_APP, L_V5, "Latitude = %08x", Latitude);
    //经度Longitude
    Longitude = (data->buf[12] << 24) & 0xFF000000;
    Longitude |= (data->buf[13] << 16) ;
    Longitude |= (data->buf[14] << 8) ;
    Longitude |= data->buf[15];
    LOGD(L_APP, L_V5, "Longitude = %08x", Longitude);
    //半径 radius
    radius = (data->buf[16] << 24) & 0xFF000000;
    radius |= (data->buf[17] << 16) ;
    radius |= (data->buf[18] << 8) ;
    radius |= data->buf[19];
    LOGD(L_APP, L_V5, "radius = %08x", radius);

    fence.sw = 1;
    fence.square = 0;

    if((status & 0x0040) > 0)
    {
        fence.lat = -((float)Latitude / 1000000);
    }
    else
    {
        fence.lat = ((float)Latitude / 1000000);
    }
    if((status & 0x0080) > 0)
    {
        fence.lon = -((float)Longitude / 1000000);
    }
    else
    {
        fence.lon = ((float)Longitude / 1000000);
    }
    fence.radius = radius / 100;
    fence.in_out = 2;

    if((status & 0x0004) > 0)
    {
        g_smstype = 1;
    }
    if((status & 0x0008) > 0)
    {
        g_gprstype = 1;
    }
    if(g_smstype == 1 && g_gprstype == 1)
    {
        fence.alarm_type = 1;
    }
    else if(g_gprstype == 1)
    {
        fence.alarm_type = 0;
    }
    else
    {
        fence.alarm_type = 2;
    }

    memcpy(&G_parameter.fence[0], &fence, sizeof(nvram_electronic_fence_struct));
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);

    track_cust_paket_0001(data, 0);
}
/******************************************************************************
 *  Function    -  track_cust_rev_8601_data
 *
 *  Purpose     -  删除电子围栏
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-22,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_rev_8601_data(nvram_bb_7edata_struct *data)
{

    kal_uint16 no = 0, result = 0, rev_cmd = 0, g_nvram_par = 0, g_nvram_impor = 0;
    kal_uint8 buffer[512];

    nvram_electronic_fence_struct fence = {0};
    memcpy(&fence, &G_parameter.fence[0], sizeof(nvram_electronic_fence_struct));
    memset(buffer, 0, sizeof(buffer));
    LOGH(L_APP, L_V5, data->buf, data->buf_len);
    LOGD(L_APP, L_V5, "平台删除围栏");


    fence.sw = 0;
    fence.lat = 0;
    fence.lon = 0;
    fence.lat2 = 0;
    fence.lon2 = 0;
    fence.in_out = OTA_NVRAM_EF_PARAMETER_DEFAULT->fence[0].in_out;
    fence.alarm_type = OTA_NVRAM_EF_PARAMETER_DEFAULT->fence[0].alarm_type;

    memcpy(&G_parameter.fence[0], &fence, sizeof(nvram_electronic_fence_struct));
    Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    track_cust_paket_0001(data, 0);
}



/******************************************************************************
 *  Function    -  track_cust_rev_error_data
 *
 *  Purpose     -  通用型返回错误
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-22,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_rev_error_data(nvram_bb_7edata_struct *data)
{

    kal_uint16 no = 0, result = 0, rev_cmd = 0, g_nvram_par = 0, g_nvram_impor = 0;
    kal_uint8 buffer[512];

    nvram_electronic_fence_struct fence = {0};
    memcpy(&fence, &G_parameter.fence[0], sizeof(nvram_electronic_fence_struct));
    memset(buffer, 0, sizeof(buffer));
    LOGH(L_APP, L_V5, data->buf, data->buf_len);
    LOGD(L_APP, L_V5, "未知协议号");
    track_cust_paket_0001(data, 3);
}

/******************************************************************************
 *  Function    -  track_cust_LongitudeLatitudeToHex
 *
 *  Purpose     -  部标经纬度转换
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-13,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
static void track_cust_LongitudeLatitudeToHex(kal_uint8 *dec, float value)
{
    kal_uint32 tmp;
    tmp = value * 1000000;
    track_fun_reverse_32(dec, tmp);
}
/******************************************************************************
 *  Function    -  DataPackingDatetime_6
 *
 *  Purpose     -  获取时间
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-21,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
static void DataPackingDatetime_6(U8 *sendbuf, track_gps_data_struct *gd)
{
    applib_time_struct time = {0};
    applib_time_struct *currTime = NULL;
    kal_uint8 time_buf[10];
    if(gd == NULL)
    {
        currTime = &time;
        if(track_cust_is_upload_UTC_time())
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
    memset(time_buf, 0, sizeof(time_buf));
    sprintf(time_buf, "%02d%02d%02d%02d%02d%02d", currTime->nYear - 2000, currTime->nMonth, currTime->nDay, currTime->nHour, currTime->nMin, currTime->nSec);
    LOGD(L_APP, L_V5, "time_buf = %s", time_buf);
    memcpy(sendbuf, track_fun_stringtohex(time_buf, 6), 6);
}

/******************************************************************************
 *  Function    -  track_fun_stringtohex
 *
 *  Purpose     -  将字符串直接转换成HEX 如"0b7f"   0x0b 0x7f
 *
 *  Description -   buf:需转换的数组len:要求装换的长度
                            此函数没有做强制检验输入字符串请勿超长或者不符规范
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 09-05-2016,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
static kal_uint8* track_fun_stringtohex(kal_uint8 * buf, kal_uint16 len)
{
    int size, i, ret, index, tmp_i, tmp_j;
    static  kal_uint8 hex_mac[100] = {0};
    kal_uint8 highByte, lowByte;
    memset(hex_mac, 0, 20);
    LOGD(L_APP, L_V5, "buf = %s", buf);
    if(len > 100)
    {
        len = 100;
    }
    if(strlen(buf) > 0)
    {
        for(tmp_i = 0, tmp_j = 0; tmp_i < len; tmp_i++)
        {

            highByte = buf[tmp_j];
            lowByte  = buf[tmp_j+1];

//            LOGD(L_APP, L_V5, "highByte = %d", highByte);
//            LOGD(L_APP, L_V5, "lowByte = %d",lowByte);

            if(highByte > 0x59)
            {
                highByte -= 0x57;
            }
            else if(highByte > 0x39)
            {
                highByte -= 0x37;
            }
            else
            {
                highByte -= 0x30;
            }

            if(lowByte > 0x59)
            {
                lowByte -= 0x57;
            }
            else if(lowByte > 0x39)
            {
                lowByte -= 0x37;
            }
            else
            {
                lowByte -= 0x30;
            }
            hex_mac[tmp_i] = (highByte << 4) | lowByte;
            tmp_j += 2;
        }
        LOGH(L_APP, L_V5, hex_mac, 20);
    }
    return hex_mac;
}

/*****************************************************************************
*  Local Functions                      本地函数
******************************************************************************/
/******************************************************************************
 *	Function	-  getSerialNumber
 *
 *	Purpose 	-  取得流水号
 *
 *	Description -  NULL
 *
 *	modification history
 * ----------------------------------------
 * v1.0  , 09-10-2012,	Cml  written
 * ----------------------------------------
*******************************************************************************/
static kal_uint16 getSerialNumber()
{
    return g_SerialNumber++;
}

static U16 DataPacketEncode7E(U8 cmd, U8 *sendData, U16 len)
{
    U16 no, crc;

    //起始位
    sendData[0] = 0x7E;

    //协议号
    PUT_UINT16_BE(cmd, sendData, 1);

    //信息序列号
    no = getSerialNumber();
    PUT_UINT16_BE(no, sendData, 11);
    LOGD(L_APP, L_V5, "SerialNumber:%.2X%.2X", sendData[len - 6], sendData[len - 5]);

    //校验位
    crc = GetCrc16(&sendData[2], len - 6);
    PUT_UINT16_BE(crc, sendData, len - 4);

    //结束
    sendData[len - 1] = 0x7E;

    return no;
}

/*****************************************************************************
*  Global Functions                     全局函数
******************************************************************************/
#if 0
typedef enum
{
    TR_CUST_ALARM_NORMAL					=	 0,   //正常
    TR_CUST_ALARM_SOS						=	 1,   //SOS 求救
    TR_CUST_ALARM_PowerFailure				=	 2,   //断电报警
    TR_CUST_ALARM_Vibration 				=	 3,   //震动报警
    TR_CUST_ALARM_InFence					=	 4,   //进围栏报警
    TR_CUST_ALARM_OutFence					=	 5,   //出围栏报警
    TR_CUST_ALARM_Speed_Limit				=	 6,   //超速报警

    TR_CUST_ALARM_displacement				=	 9,   //位移报警
    TR_CUST_ALARM_IN_blind					= 0x0A,   //进GPS盲区报警
    TR_CUST_ALARM_OUT_blind 				= 0x0B,   //出GPS盲区报警
    TR_CUST_ALARM_boot						= 0x0C,   //开机报警
    TR_CUST_ALARM_gps_successfully			= 0x0D,   //GPS第一次定位报警
    TR_CUST_ALARM_Ext_LowBattery			= 0x0E,   //外电低电报警
    TR_CUST_ALARM_Ext_PowerFailure			= 0x0F,   //外电低电保护报警
    TR_CUST_ALARM_SIM_ALARM 				= 0x10,   //换卡报警
    TR_CUST_ALARM_PWROFF					= 0x11,   //手动按键关机提醒
    TR_CUST_ALARM_Ext_LowBattery_Fly		= 0x12,   //外电低电保护后进入飞行模式提醒
    TR_CUST_ALARM_PRE_REMOVE				= 0x13,   //拆卸报警
    TR_CUST_ALARM_door						= 0x14,   //门报警
    TR_CUST_ALARM_LowBattery_PWROFF 		= 0x15,   //内部电池低电即将关机提醒
    TR_CUST_ALARM_SOD						= 0x16,   //声控报警
    TR_CUST_ALARM_FAKECELL					= 0x17,   //伪基站报警
    TR_CUST_ALARM_OPEN_SHELL				= 0x18,   //开盖报警（拆外壳）
    TR_CUST_ALARM_LowBattery				= 0x19,   //低电报警
    TR_CUST_ALARM_INTO_DEEP_SLEEP_MODE		= 0x20,   //进入深度休眠报警
    TR_CUST_ALARM_ACC_ON					= 0x21,   //ACC开（实际值应该是0xFE，但由于非连续值，影响代码逻辑判断）
    TR_CUST_ALARM_ACC_OFF					= 0x22,   //ACC关（实际值应该是0xFF，但由于非连续值，影响代码逻辑判断）
    TR_CUST_ALARM_DROP_ALM				= 0x23, //跌倒报警
    //										= 0x62,   //等于98，特殊用途
    //										= 0x63,   //等于99，特殊用途

    //										= 0xFE,   //已占用
    //										= 0xFF,   //已占用
    TR_CUST_ALARM_MAX
} enum_alarm_type;
#endif /* 0 */

/******************************************************************************
 *  Function    -  track_cust_alarm_bb_data
 *
 *  Purpose     -  报警状态
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-12,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint32 track_cust_alarm_bb_data(enum_alarm_type alarm, kal_uint16 status)
{
#if 1
    kal_uint32 alarm_status = 0;
    kal_uint32 data = 0;

    LOGD(L_APP, L_V5, "alarm = %d;status = %d", alarm, status);

    //sos报警 0
    if(track_alarm_vec_queue_check(TR_CUST_ALARM_SOS) > 0)
    {
        alarm_status |= 0x01;
    }
    //超速报警 1
    if(track_alarm_vec_queue_check(TR_CUST_ALARM_Speed_Limit) > 0)
    {
        alarm_status |= 0x02;
    }
    //终端主电源欠压 7
    if(track_alarm_vec_queue_check(TR_CUST_ALARM_Ext_LowBattery) > 0)
    {
        alarm_status |= 0x80;
    }
    //震动报警  15
    if(track_alarm_vec_queue_check(TR_CUST_ALARM_Vibration) > 0)
    {
        alarm_status |= 0x8000;
    }
    //电池低电报警 16
    if(G_realtime_data.lowBattery == 1)
    {
        alarm_status |= 0x010000;
    }
    //拆除报警  17
    if(track_alarm_vec_queue_check(TR_CUST_ALARM_PRE_REMOVE) > 0)
    {
        alarm_status |= 0x020000;
    }
    //进出围栏报警 20
    if(track_alarm_vec_queue_check(TR_CUST_ALARM_InFence) > 0 || track_alarm_vec_queue_check(TR_CUST_ALARM_OutFence) > 0)
    {
        alarm_status |= 0x100000;
    }
    //车辆非法位移报警 28
    if(track_alarm_vec_queue_check(TR_CUST_ALARM_displacement) > 0)
    {
        alarm_status |= 0x1000000;
    }


    LOGD(L_APP, L_V5, "@1 alarm_status = %d", alarm_status);
    return alarm_status;

#else
    static kal_uint32 alarm_status = 0;
    kal_uint32 data = 0;

    LOGD(L_APP, L_V5, "alarm = %d;status = %d", alarm, status);
    if(status == 99)
    {
        LOGD(L_APP, L_V5, "alarm_status = %d", alarm_status);
        return alarm_status;
    }
    if(status == 1)
    {
        if(alarm == TR_CUST_ALARM_SOS)
        {
            data = 1;
        }


        alarm_status = alarm_status | data;
    }
    else
    {
        if(alarm == TR_CUST_ALARM_SOS)
        {
            data = 0xFFFE;
        }

        alarm_status = alarm_status & data;
    }
    LOGD(L_APP, L_V5, "@1 alarm_status = %d", alarm_status);
    return alarm_status;
#endif /* 1 */
}

kal_uint32 track_cust_bb_data(track_gps_data_struct *data, kal_uint16 status)
{
#if 1
    kal_uint32 data_status = 0;	
//acc
    if(track_cust_status_acc() == 1)
    {

        data_status |= 0x01;
    }
//gps
    if(data->gprmc.status >= 0)
    {
        data_status |= 0x02;
        //南北纬
        if(data->gprmc.NS == 'S')
        {
            data_status |= 0x04;
        }
        //东西经
        if(data->gprmc.EW == 'W')
        {
            data_status |= 0x08;
        }
    }

//运营状态
//经纬度保密

//车辆油路状态
//车辆电路状态
//布撤防
//

    return data_status;

#else
    static kal_uint32 data_status = 0;

    LOGD(L_APP, L_V5, "alarm = %d;status = %d", data, status);
    if(status == 99)
    {
        LOGD(L_APP, L_V5, "status = %d", data_status);
        return data_status;
    }
    if(status == 1)
    {
        data_status = data_status | data;
    }
    else
    {
        data_status = data_status & data;
    }
    LOGD(L_APP, L_V5, "@1 status = %d", data_status);
    return data_status;
#endif /* 0 */
}



/******************************************************************************
 *  Function    -  track_cust_check_num
 *
 *  Purpose     -  校验码
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-12,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint16  track_cust_check_num(kal_uint8 *buffer, kal_uint16 len)
{
    int i = 0;
    kal_uint16 num = 0;
    LOGH(L_APP, L_V6, buffer, len);
    for(i = 0; i < len; i++)
    {
        num ^= *buffer;
        buffer++;
    }
    return num;
}


/******************************************************************************
 *  Function    -  track_cust_check_7EData
 *
 *  Purpose     -  检查数据是否包含0x7E
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-12,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint16 track_cust_check_7EData(kal_uint8 *buffer, kal_uint16 len)
{
    kal_uint8 buf[250];
    kal_uint8 *buf_data;
    kal_uint16 i = 0, buf_len = 0, j = 0;

    memset(buf, 0, sizeof(buf));

    memcpy(&buf, buffer, len);
    LOGD(L_APP, L_V5, "@@@@len = %d", len);
    buf_len = len - 2;
    j = 1;
    for(i = 0; i < buf_len; i++)
    {
#if 0
        if(i == 5)
        {
            buf[i+1] = 0x7e;
        }
        else if(i == 10)
        {
            buf[i+1] = 0x7d;
        }
#endif /* 0 */
        if(buf[i+1] == 0x7e)
        {
            buffer[j] = 0x7d;
            j++;
            buffer[j] = 0x02;
        }
        else if(buf[i+1] == 0x7d)
        {
            buffer[j] = 0x7d;
            j++;
            buffer[j] = 0x01;
        }
        else
        {
            buffer[j] = buf[i+1];
        }
        j++;
    }
    buffer[j] = 0x7e;
    j++;
    LOGH(L_APP, L_V5, buffer, j);
    LOGH(L_APP, L_V5, buf, len);
    LOGD(L_APP, L_V5, "j = %d", j);
    return j;
}

/******************************************************************************
 *  Function    -  track_cust_check_rev_7EData
 *
 *  Purpose     -  检查收到的7E数据
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-14,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint16 track_cust_check_rev_7EData(kal_uint8 *buffer, kal_uint16 len)
{
    kal_uint8 buf[250];
    kal_uint8 *buf_data;
    kal_uint16 i = 0, buf_len = 0, j = 0;

    memset(buf, 0, sizeof(buf));

    memcpy(&buf, buffer, len);
    LOGH(L_APP, L_V5, buffer, len);
    buf_len = len - 2;
    j = 1;
    for(i = 0; i < buf_len; i++)
    {
        if(buf[i+1] == 0x7d && buf[i+2] == 0x02)
        {
            buffer[j] = 0x7e;
            i++;
            LOGD(L_APP, L_V5, "@buf[%d]=%x", i + 1, buf[i+1]);
        }
        else if(buf[i+1] == 0x7d && buf[i+2] == 0x01)
        {
            buffer[j] = 0x7d;
            i++;
            LOGD(L_APP, L_V5, "#buf[%d]=%x", i + 1, buf[i+1]);
        }
        else
        {
            buffer[j] = buf[i+1];
        }
        j++;
    }
    buffer[j] = 0x7e;
    j++;
    LOGH(L_APP, L_V5, buffer, j);
    LOGD(L_APP, L_V5, "j = %d", j);
    return j;
}

/******************************************************************************
 *  Function    -  track_cust_paket_7EData
 *
 *  Purpose     -  打包7E数据
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-12,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint16  track_cust_paket_7EData(kal_uint16 cmd, kal_uint8* buffer, kal_uint16 buf_len)
{
    kal_uint8 *sendData;
    kal_uint16 no, zone;
    kal_uint16 size = 0, i = buf_len;
    kal_uint8 buf[21];
    kal_uint16 check_num;
    kal_uint16 len;
    LOGD(L_APP, L_V5, "");

    LOGD(L_APP, L_V5, "%d", i);
    LOGH(L_APP, L_V5, buffer, i);

    size = 2/*包头包尾*/ + 2/*消息ID*/ + 2/*消息体属性*/ + 6/*手机号码*/ + 2/*流水号*/ + i/*消息体*/ + 1/*校验码*/;
    if(size > 200)
    {
        LOGD(L_APP, L_V5, "Error!!!!!! over size");
        return;
    }
    else
    {
        size = 250;
    }
    sendData = (U8*)Ram_Alloc(5, size);
    if(sendData == NULL)
    {
        return;
    }
    memset(sendData, 0, size);
    //包头
    sendData[0] = 0x7e;
    //协议号
    sendData[1] = (cmd & 0xFF00) >> 8;
    sendData[2] = (cmd & 0x00FF);

    //消息体属性
    sendData[3] = (i & 0xFF00) >> 8;
    sendData[4] = (i & 0x00FF);
    //手机号码
    memset(buf, 0, sizeof(buf));
    snprintf(buf, 13, "%012s", G_importance_parameter_data.login_data.sim_num);
    memcpy(&sendData[5], track_fun_stringtohex(buf, 6), 6);
    //流水号
    no = getSerialNumber();
    sendData[11] = (no & 0xFF00) >> 8;
    sendData[12] = (no & 0x00FF);
    //消息体
    memcpy(&sendData[13], buffer, i);

    //检验码
    len = 13 + i - 1;//len去除了7E包头
    check_num = track_cust_check_num(&sendData[1], len);
    len++;
    sendData[len++] =  check_num;
    //包尾
    sendData[len] = 0x7e;

    len++;
    LOGH(L_APP, L_V6, sendData, len);
    //检查数据是否包含0x7e
    len = track_cust_check_7EData(sendData, len);

#if 0
    no = DataPacketEncode7E(0x01, sendData, size);
#if defined(__XCWS_TPATH__)
    Socket_write_req(2, sendData, size, 0, 1, 1, 1);
#else
    track_soc_write_req(sendData, size, 1, no);
#endif /* __XCWS_TPATH__ */
#endif
    if(cmd == 0x0200)
    {
        track_socket_queue_position_send_reg(sendData, len, 0, no);
    }
    else
    {
        track_soc_write_req(sendData, len, 0, no);
    }
    LOGD(L_APP, L_V6, "len = %d", len);
    LOGH(L_APP, L_V6, sendData, len);
    //track_cust_decode_7EData(sendData, len);
    Ram_Free(sendData);
    return no;
}











/******************************************************************************
 *  Function    -  track_cust_paket_0100
 *
 *  Purpose     -  登录包
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-12,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_paket_0100(void)
{

    int size = 0, i = 0;
    kal_uint8 buffer[250];

    LOGD(L_APP, L_V5, "");
    memset(buffer, 0, sizeof(buffer));
//消息体
    LOGD(L_APP, L_V5, "%s", G_importance_parameter_data.login_data.provincial);
    i = 1;
    memcpy(&buffer[i], track_fun_stringtohex(G_importance_parameter_data.login_data.provincial, 1), 1);
    i++;
//LOGH(L_APP, L_V6, buffer, i);//1
    memcpy(&buffer[i], track_fun_stringtohex(&G_importance_parameter_data.login_data.provincial[2], 2), 2);
    i = 4;
//LOGH(L_APP, L_V6, buffer, i);//2
    memcpy(&buffer[i], G_importance_parameter_data.login_data.manufacturer, 5);
    i = 9;
//LOGH(L_APP, L_V6, buffer, i);//3
    memcpy(&buffer[i], G_importance_parameter_data.login_data.terminal_model, strlen(G_importance_parameter_data.login_data.terminal_model));
    i = 29;
//LOGH(L_APP, L_V6, buffer, i);//4
    memcpy(&buffer[i], G_importance_parameter_data.login_data.terminal_id, 7);
    i = 36;
//LOGH(L_APP, L_V6, buffer, i);//5
    memcpy(&buffer[i], &G_importance_parameter_data.login_data.car_color, 1);
//strcat((char *)buffer, (char *)G_importance_parameter_data.login_data.car_color);
    i = 37;
//LOGH(L_APP, L_V6, buffer, i);//6
    if(strlen(G_importance_parameter_data.login_data.car_num) > 0)
    {
        memcpy(&buffer[i], G_importance_parameter_data.login_data.car_num, strlen(G_importance_parameter_data.login_data.car_num));
        i = i + strlen(G_importance_parameter_data.login_data.car_num);
    }
    track_cust_paket_7EData(0x0100, buffer, i);
}



/******************************************************************************
 *  Function    -  track_cust_paket_0102
 *
 *  Purpose     -  鉴权码登录包
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-12,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_paket_0102(void)
{

    int size = 0, i = 0;
    kal_uint8 buffer[250];
    kal_uint16 len;
    LOGD(L_APP, L_V5, "");
    memset(buffer, 0, sizeof(buffer));
//消息体
    LOGD(L_APP, L_V5, "%s", G_parameter.bb_data.authorize_code);
    strcpy(buffer, G_parameter.bb_data.authorize_code);
    len = strlen(buffer);
    track_cust_paket_7EData(0x0102, buffer, len);
}





/******************************************************************************
 *  Function    -  track_cust_paket_0200
 *
 *  Purpose     -  位置信息包
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-12,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_paket_0200(track_gps_data_struct *gd, LBS_Multi_Cell_Data_Struct *lbs)
{

    int size = 0, i = 0;
    kal_uint8 buffer[250];
    kal_uint32 alarm_data = 0;
    int tmp, ret;
    kal_uint16 no = 0;


    LOGD(L_APP, L_V5, "");
    memset(buffer, 0, sizeof(buffer));
    //消息体
    LOGD(L_APP, L_V5, "%s", G_parameter.bb_data.authorize_code);
    if(track_cust_alarm_type(0) > 0)
    {
        track_alarm_vec_queue_push(track_cust_alarm_type(0), 0, 0);
    }
    i = 0;
    //获取到报警状态
    alarm_data = track_cust_alarm_bb_data(0, 99);
    buffer[i++] = (alarm_data & 0xFF000000) >> 24;
    buffer[i++] = (alarm_data & 0x00FF0000) >> 16;
    buffer[i++] = (alarm_data & 0x0000FF00) >> 8;
    buffer[i++] = (alarm_data & 0x000000FF);


    //获取到终端状态
    alarm_data = track_cust_bb_data(gd, 99);
    buffer[i++] = (alarm_data & 0xFF000000) >> 24;
    buffer[i++] = (alarm_data & 0x00FF0000) >> 16;
    buffer[i++] = (alarm_data & 0x0000FF00) >> 8;
    buffer[i++] = (alarm_data & 0x000000FF);

    //track_cust_paket_7EData(0x0102, buffer, i);
    //纬度
    //LOGD(L_APP, L_V5, "Latitude = %f", gd->gprmc.Latitude);
    //track_fun_LongitudeLatitudeToHex(&buffer[i], gd->gprmc.Latitude);
    track_cust_LongitudeLatitudeToHex(&buffer[i],  gd->gprmc.Latitude);
    //经度
    i = i + 4;
    //LOGH(L_APP, L_V6, buffer, i);
    //LOGD(L_APP, L_V5, "Longitude = %f", gd->gprmc.Longitude);
    //track_fun_LongitudeLatitudeToHex(&buffer[i], gd->gprmc.Longitude);
    track_cust_LongitudeLatitudeToHex(&buffer[i],  gd->gprmc.Longitude);
    //高度
    i = i + 4;
    //LOGH(L_APP, L_V6, buffer, i);
    //LOGD(L_APP, L_V5, "msl_altitude = %f", gd->gpgga.msl_altitude);
    buffer[i++] = ((kal_uint16)gd->gpgga.msl_altitude & 0xFF00) >> 8;
    buffer[i++] = ((kal_uint16)gd->gpgga.msl_altitude & 0x00FF);
    //速度
    //LOGH(L_APP, L_V6, buffer, i);
    //LOGD(L_APP, L_V5, "Speed = %f", gd->gprmc.Speed);
    buffer[i++] = ((kal_uint16)gd->gprmc.Speed & 0xFF00) >> 8;
    buffer[i++] = ((kal_uint16)gd->gprmc.Speed & 0x00FF);

    //方向
    //LOGH(L_APP, L_V6, buffer, i);
    //LOGD(L_APP, L_V5, "Course = %f", gd->gprmc.Course);
    buffer[i++] = ((kal_uint16)gd->gprmc.Course & 0xFF00) >> 8;
    buffer[i++] = ((kal_uint16)gd->gprmc.Course & 0x00FF);
    //时间
    //LOGH(L_APP, L_V6, buffer, i);
    DataPackingDatetime_6(&buffer[i], gd);
    //附加信息:

    //
    i = i + 6;
    LOGH(L_APP, L_V6, buffer, i);//1
    //里程
    //GSM信号

    tmp = track_cust_get_rssi(-1);
    if(tmp < 0)
    {
        tmp = 99;
    }
    buffer[i++] = 0x30;
    buffer[i++] = 0x01;
    buffer[i++] = tmp;
    //GNSS定位卫星书
    //基站数据

    LOGH(L_APP, L_V6, buffer, i);//1
    no = track_cust_paket_7EData(0x0200, buffer, i);
    if(track_cust_alarm_type(0) > 0)
    {
        LOGD(L_APP, L_V5, "报警包");
        no_alarm_0200 = no;
        track_cust_alarm_type(TR_CUST_ALARM_MAX);
    }

}


/******************************************************************************
 *  Function    -  track_cust_paket_0201
 *
 *  Purpose     -  位置信息查询应答
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-17,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_paket_0201(track_gps_data_struct *gd, LBS_Multi_Cell_Data_Struct *lbs, kal_uint16 no)
{

    int size = 0, i = 0;
    kal_uint8 buffer[250];
    kal_uint32 alarm_data = 0;
    int tmp, ret;


    LOGD(L_APP, L_V5, "");
    memset(buffer, 0, sizeof(buffer));
    //消息体
    LOGD(L_APP, L_V5, "%d", no);
    i = 0;
    //应答流水号
    buffer[i++] = (no & 0xFF00) >> 8;
    buffer[i++] = (no & 0x00FF);
    //获取到报警状态
    alarm_data = track_cust_alarm_bb_data(0, 99);
    buffer[i++] = (alarm_data & 0xFF000000) >> 24;
    buffer[i++] = (alarm_data & 0x00FF0000) >> 16;
    buffer[i++] = (alarm_data & 0x0000FF00) >> 8;
    buffer[i++] = (alarm_data & 0x000000FF);


    //获取到终端状态
    alarm_data = track_cust_bb_data(gd, 99);
    buffer[i++] = (alarm_data & 0xFF000000) >> 24;
    buffer[i++] = (alarm_data & 0x00FF0000) >> 16;
    buffer[i++] = (alarm_data & 0x0000FF00) >> 8;
    buffer[i++] = (alarm_data & 0x000000FF);

    //track_cust_paket_7EData(0x0102, buffer, i);
    //纬度
    //LOGD(L_APP, L_V5, "Latitude = %f", gd->gprmc.Latitude);
    //track_fun_LongitudeLatitudeToHex(&buffer[i], gd->gprmc.Latitude);
    track_cust_LongitudeLatitudeToHex(&buffer[i],  gd->gprmc.Latitude);
    //经度
    i = i + 4;
    //LOGH(L_APP, L_V6, buffer, i);
    //LOGD(L_APP, L_V5, "Longitude = %f", gd->gprmc.Longitude);
    //track_fun_LongitudeLatitudeToHex(&buffer[i], gd->gprmc.Longitude);
    track_cust_LongitudeLatitudeToHex(&buffer[i],  gd->gprmc.Longitude);
    //高度
    i = i + 4;
    //LOGH(L_APP, L_V6, buffer, i);
    //LOGD(L_APP, L_V5, "msl_altitude = %f", gd->gpgga.msl_altitude);
    buffer[i++] = ((kal_uint16)gd->gpgga.msl_altitude & 0xFF00) >> 8;
    buffer[i++] = ((kal_uint16)gd->gpgga.msl_altitude & 0x00FF);
    //速度
    //LOGH(L_APP, L_V6, buffer, i);
    //LOGD(L_APP, L_V5, "Speed = %f", gd->gprmc.Speed);
    buffer[i++] = ((kal_uint16)gd->gprmc.Speed & 0xFF00) >> 8;
    buffer[i++] = ((kal_uint16)gd->gprmc.Speed & 0x00FF);

    //方向
    //LOGH(L_APP, L_V6, buffer, i);
    //LOGD(L_APP, L_V5, "Course = %f", gd->gprmc.Course);
    buffer[i++] = ((kal_uint16)gd->gprmc.Course & 0xFF00) >> 8;
    buffer[i++] = ((kal_uint16)gd->gprmc.Course & 0x00FF);
    //时间
    //LOGH(L_APP, L_V6, buffer, i);
    DataPackingDatetime_6(&buffer[i], gd);
    //附加信息:

    //
    i = i + 6;
    LOGH(L_APP, L_V6, buffer, i);//1
    //里程
    //GSM信号

    tmp = track_cust_get_rssi(-1);
    if(tmp < 0)
    {
        tmp = 99;
    }
    buffer[i++] = 0x30;
    buffer[i++] = 0x01;
    buffer[i++] = tmp;
    //GNSS定位卫星书
    //基站数据

    LOGH(L_APP, L_V6, buffer, i);//1
    track_cust_paket_7EData(0x0201, buffer, i);
}

/******************************************************************************
 *  Function    -  track_cust_paket_0002
 *
 *  Purpose     -  心跳包
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-13,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_paket_0002(void)
{

    int size = 0, i = 0;
    kal_uint8 buffer[250];
    kal_uint16 len;
    LOGD(L_APP, L_V5, "");
    memset(buffer, 0, sizeof(buffer));
    track_cust_paket_7EData(0x0002, buffer, i);
}


/******************************************************************************
 *  Function    -  track_cust_paket_0104
 *
 *  Purpose     -  查询终端参数应答
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-20,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_paket_0104(kal_uint16 no)
{
    int size = 0, i = 0, len = 0, index = 0;
    kal_uint8 buffer[250], buf[100];
    kal_uint32 alarm_data = 0, id = 0, num = 0;
    kal_uint32  time = 0;
    int tmp, ret;

    LOGD(L_APP, L_V5, "");
    memset(buffer, 0, sizeof(buffer));
    memset(buf, 0, sizeof(buf));
    //消息体
    LOGD(L_APP, L_V5, "%d", no);
    i = 0;
    //应答流水号
    buffer[i++] = (no & 0xFF00) >> 8;
    buffer[i++] = (no & 0x00FF);

    //应答参数个数
    i++;
    //ACC ON心跳包
    id = 0x0001;
    buffer[i++] = (id & 0xFF000000) >> 24;
    buffer[i++] = (id & 0x00FF0000) >> 16;
    buffer[i++] = (id & 0x0000FF00) >> 8;
    buffer[i++] = (id & 0x000000FF);

    buffer[i++] = 4;

    time = G_parameter.hbt_acc_on;

    buffer[i++] = (time & 0xFF000000) >> 24;
    buffer[i++] = (time & 0x00FF0000) >> 16;
    buffer[i++] = (time & 0x0000FF00) >> 8;
    buffer[i++] = (time & 0x000000FF);
    index++;
    //ACC OFF心跳包
    id = 0x0008;
    buffer[i++] = (id & 0xFF000000) >> 24;
    buffer[i++] = (id & 0x00FF0000) >> 16;
    buffer[i++] = (id & 0x0000FF00) >> 8;
    buffer[i++] = (id & 0x000000FF);

    buffer[i++] = 4;

    time = G_parameter.hbt_acc_off;

    buffer[i++] = (time & 0xFF000000) >> 24;
    buffer[i++] = (time & 0x00FF0000) >> 16;
    buffer[i++] = (time & 0x0000FF00) >> 8;
    buffer[i++] = (time & 0x000000FF);
    index++;

    //APN
    id = 0x0010;
    buffer[i++] = (id & 0xFF000000) >> 24;
    buffer[i++] = (id & 0x00FF0000) >> 16;
    buffer[i++] = (id & 0x0000FF00) >> 8;
    buffer[i++] = (id & 0x000000FF);

    strcpy(buf, track_nvram_alone_parameter_read()->apn.apn);
    len = strlen(buf);
    buffer[i++] = len;
    //strcat(buffer, buf);
    memcpy(&buffer[i], buf, len);

    i = i + len;
    index++;
    //APN 用户名
    id = 0x0011;
    buffer[i++] = (id & 0xFF000000) >> 24;
    buffer[i++] = (id & 0x00FF0000) >> 16;
    buffer[i++] = (id & 0x0000FF00) >> 8;
    buffer[i++] = (id & 0x000000FF);

    strcpy(buf, track_nvram_alone_parameter_read()->apn.username);
    len = strlen(buf);
    buffer[i++] = len;
    memcpy(&buffer[i], buf, len);

    i = i + len;
    index++;
    //APN用户密码
    id = 0x0012;
    buffer[i++] = (id & 0xFF000000) >> 24;
    buffer[i++] = (id & 0x00FF0000) >> 16;
    buffer[i++] = (id & 0x0000FF00) >> 8;
    buffer[i++] = (id & 0x000000FF);

    strcpy(buf, track_nvram_alone_parameter_read()->apn.password);
    len = strlen(buf);
    buffer[i++] = len;
    memcpy(&buffer[i], buf, len);

    i = i + len;
    index++;

    //主服务器域名
    id = 0x0013;
    buffer[i++] = (id & 0xFF000000) >> 24;
    buffer[i++] = (id & 0x00FF0000) >> 16;
    buffer[i++] = (id & 0x0000FF00) >> 8;
    buffer[i++] = (id & 0x000000FF);
    if(track_nvram_alone_parameter_read()->server.conecttype == 0)
    {
        sprintf(buf, "%d.%d.%d.%d", track_nvram_alone_parameter_read()->server.server_ip[0], track_nvram_alone_parameter_read()->server.server_ip[1], track_nvram_alone_parameter_read()->server.server_ip[2], track_nvram_alone_parameter_read()->server.server_ip[3]);


    }
    else
    {
        strcpy(buf, track_domain_decode(track_nvram_alone_parameter_read()->server.url));
    }
    LOGD(L_APP, L_V5, "buf = %s", buf);
    len = strlen(buf);
    buffer[i++] = len;
    //strcat(buffer, buf);
    memcpy(&buffer[i], buf, len);
    i = i + len;
    index++;
    //主服务器端口

    id = 0x0018;
    buffer[i++] = (id & 0xFF000000) >> 24;
    buffer[i++] = (id & 0x00FF0000) >> 16;
    buffer[i++] = (id & 0x0000FF00) >> 8;
    buffer[i++] = (id & 0x000000FF);

    buffer[i++] = 4;
    num = track_nvram_alone_parameter_read()->server.server_port;
    buffer[i++] = (num & 0xFF000000) >> 24;
    buffer[i++] = (num & 0x00FF0000) >> 16;
    buffer[i++] = (num & 0x0000FF00) >> 8;
    buffer[i++] = (num & 0x000000FF);
    index++;
    //位置汇报策略
    id = 0x0020;
    buffer[i++] = (id & 0xFF000000) >> 24;
    buffer[i++] = (id & 0x00FF0000) >> 16;
    buffer[i++] = (id & 0x0000FF00) >> 8;
    buffer[i++] = (id & 0x000000FF);

    buffer[i++] = 4;

    num = G_parameter.gps_work.Fd.sw;
    if(G_parameter.gps_work.Fti.sw == 1 && G_parameter.gps_work.Fd.sw == 1)
    {
        num = 2;
    }
    else if(G_parameter.gps_work.Fti.sw == 1)
    {
        num = 0;
    }
    else
    {
        num = 1;
    }
    buffer[i++] = (num & 0xFF000000) >> 24;
    buffer[i++] = (num & 0x00FF0000) >> 16;
    buffer[i++] = (num & 0x0000FF00) >> 8;
    buffer[i++] = (num & 0x000000FF);
    index++;
    if(G_parameter.gps_work.Fti.sw == 1)
    {
        //休眠时汇报时间间隔
        id = 0x0027;
        buffer[i++] = (id & 0xFF000000) >> 24;
        buffer[i++] = (id & 0x00FF0000) >> 16;
        buffer[i++] = (id & 0x0000FF00) >> 8;
        buffer[i++] = (id & 0x000000FF);

        buffer[i++] = 4;

        num = G_parameter.gps_work.Fti.interval_acc_off;
        buffer[i++] = (num & 0xFF000000) >> 24;
        buffer[i++] = (num & 0x00FF0000) >> 16;
        buffer[i++] = (num & 0x0000FF00) >> 8;
        buffer[i++] = (num & 0x000000FF);
        index++;
        //缺省时间汇报间隔
        id = 0x0029;
        buffer[i++] = (id & 0xFF000000) >> 24;
        buffer[i++] = (id & 0x00FF0000) >> 16;
        buffer[i++] = (id & 0x0000FF00) >> 8;
        buffer[i++] = (id & 0x000000FF);
        buffer[i++] = 4;

        num = G_parameter.gps_work.Fti.interval_acc_on;
        buffer[i++] = (num & 0xFF000000) >> 24;
        buffer[i++] = (num & 0x00FF0000) >> 16;
        buffer[i++] = (num & 0x0000FF00) >> 8;
        buffer[i++] = (num & 0x000000FF);
        index++;
    }
    if(G_parameter.gps_work.Fd.sw == 1)
    {
        //缺省距离汇报间隔
        id = 0x002C;
        buffer[i++] = (id & 0xFF000000) >> 24;
        buffer[i++] = (id & 0x00FF0000) >> 16;
        buffer[i++] = (id & 0x0000FF00) >> 8;
        buffer[i++] = (id & 0x000000FF);
        buffer[i++] = 4;

        num = G_parameter.gps_work.Fd.distance;
        buffer[i++] = (num & 0xFF000000) >> 24;
        buffer[i++] = (num & 0x00FF0000) >> 16;
        buffer[i++] = (num & 0x0000FF00) >> 8;
        buffer[i++] = (num & 0x000000FF);
        index++;
    }

    if(G_parameter.gps_work.anglerep.sw == 1)
    {
        //拐点补传角度
        id = 0x0030;
        buffer[i++] = (id & 0xFF000000) >> 24;
        buffer[i++] = (id & 0x00FF0000) >> 16;
        buffer[i++] = (id & 0x0000FF00) >> 8;
        buffer[i++] = (id & 0x000000FF);
        buffer[i++] = 4;

        num = G_parameter.gps_work.anglerep.angleOfDeflection;
        buffer[i++] = (num & 0xFF000000) >> 24;
        buffer[i++] = (num & 0x00FF0000) >> 16;
        buffer[i++] = (num & 0x0000FF00) >> 8;
        buffer[i++] = (num & 0x000000FF);
        index++;
    }
    //电子围栏半径(米)
    id = 0x0031;
    buffer[i++] = (id & 0xFF000000) >> 24;
    buffer[i++] = (id & 0x00FF0000) >> 16;
    buffer[i++] = (id & 0x0000FF00) >> 8;
    buffer[i++] = (id & 0x000000FF);
    buffer[i++] = 4;

    num = G_parameter.fence[0].radius * 100;
    buffer[i++] = (num & 0xFF000000) >> 24;
    buffer[i++] = (num & 0x00FF0000) >> 16;
    buffer[i++] = (num & 0x0000FF00) >> 8;
    buffer[i++] = (num & 0x000000FF);
    index++;
#if 0
    //报警屏蔽字
    id = 0x0050;
    buffer[i++] = (id & 0xFF000000) >> 24;
    buffer[i++] = (id & 0x00FF0000) >> 16;
    buffer[i++] = (id & 0x0000FF00) >> 8;
    buffer[i++] = (id & 0x000000FF);
    buffer[i++] = 4;

    num = G_parameter.gps_work.Fti.interval_acc_on;
    buffer[i++] = (num & 0xFF000000) >> 24;
    buffer[i++] = (num & 0x00FF0000) >> 16;
    buffer[i++] = (num & 0x0000FF00) >> 8;
    buffer[i++] = (num & 0x000000FF);
    index++;
#endif
    if(G_parameter.speed_limit_alarm.sw == 1)
    {
        //最高速度 默认100
        id = 0x0055;
        buffer[i++] = (id & 0xFF000000) >> 24;
        buffer[i++] = (id & 0x00FF0000) >> 16;
        buffer[i++] = (id & 0x0000FF00) >> 8;
        buffer[i++] = (id & 0x000000FF);
        buffer[i++] = 4;

        num = G_parameter.speed_limit_alarm.threshold;
        buffer[i++] = (num & 0xFF000000) >> 24;
        buffer[i++] = (num & 0x00FF0000) >> 16;
        buffer[i++] = (num & 0x0000FF00) >> 8;
        buffer[i++] = (num & 0x000000FF);
        index++;
        //超速持续时间
        id = 0x0056;
        buffer[i++] = (id & 0xFF000000) >> 24;
        buffer[i++] = (id & 0x00FF0000) >> 16;
        buffer[i++] = (id & 0x0000FF00) >> 8;
        buffer[i++] = (id & 0x000000FF);
        buffer[i++] = 4;

        num = G_parameter.speed_limit_alarm.delay_time;
        buffer[i++] = (num & 0xFF000000) >> 24;
        buffer[i++] = (num & 0x00FF0000) >> 16;
        buffer[i++] = (num & 0x0000FF00) >> 8;
        buffer[i++] = (num & 0x000000FF);
        index++;
    }
#if 0
    //设置车辆里程 1/10km
    id = 0x0080;
    buffer[i++] = (id & 0xFF000000) >> 24;
    buffer[i++] = (id & 0x00FF0000) >> 16;
    buffer[i++] = (id & 0x0000FF00) >> 8;
    buffer[i++] = (id & 0x000000FF);
    buffer[i++] = 4;

    num = G_parameter.gps_work.Fti.interval_acc_on;
    buffer[i++] = (num & 0xFF000000) >> 24;
    buffer[i++] = (num & 0x00FF0000) >> 16;
    buffer[i++] = (num & 0x0000FF00) >> 8;
    buffer[i++] = (num & 0x000000FF);
    index++;
#endif
    //应答参数个数
    buffer[2] = index;

    LOGD(L_APP, L_V5, "index = %d", index);
    LOGH(L_APP, L_V6, buffer, i);//1
    track_cust_paket_7EData(0x0104, buffer, i);
}




/******************************************************************************
 *  Function    -  track_cust_paket_0001
 *
 *  Purpose     -  终端通用型应答
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-22,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_paket_0001(nvram_bb_7edata_struct *data, kal_uint16 result)
{

    int size = 0, i = 0;
    kal_uint8 buffer[250];
    kal_uint32 alarm_data = 0;
    int tmp, ret;


    LOGD(L_APP, L_V5, "");
    memset(buffer, 0, sizeof(buffer));
    //消息体
    LOGD(L_APP, L_V5, "%d", data->rev_serialnumber);
    i = 0;
    //应答流水号
    buffer[i++] = (data->rev_serialnumber & 0xFF00) >> 8;
    buffer[i++] = (data->rev_serialnumber & 0x00FF);
    //应答协议号
    buffer[i++] = (data->cmd & 0xFF00) >> 8;
    buffer[i++] = (data->cmd & 0x00FF);
    //应答结果
    buffer[i++] = result;

    LOGH(L_APP, L_V6, buffer, i);//1
    track_cust_paket_7EData(0x0001, buffer, i);
}

/******************************************************************************
 ******************************************************************************
										分界线
 ******************************************************************************
 ******************************************************************************/

void heartbeat_packets_bb(void)
{
    LOGD(L_APP, L_V5, "acc=%d;hbt_acc_on=%d,hbt_acc_off=%d", track_cust_status_acc(), G_parameter.hbt_acc_on, G_parameter.hbt_acc_off);
    if(track_cust_status_acc() == 1)
    {
        if(G_parameter.hbt_acc_on > 0)
        {
            track_start_timer(TRACK_CUST_NET_HEARTBEAT_TIMER_ID, G_parameter.hbt_acc_on * 1000, heartbeat_packets, (void *)105);
        }
        if(!track_is_timer_run(TRACK_CUST_NET_HEARTBEAT_OVER_TIMER_ID))
        {
            track_start_timer(TRACK_CUST_NET_HEARTBEAT_OVER_TIMER_ID, G_parameter.hbt_acc_on * 2000, track_soc_gprs_reconnect, (void *)27);
        }
    }
    else
    {
        if(G_parameter.hbt_acc_off > 0)
        {
            track_start_timer(TRACK_CUST_NET_HEARTBEAT_TIMER_ID, G_parameter.hbt_acc_off * 1000, heartbeat_packets, (void *)106);
        }
        if(!track_is_timer_run(TRACK_CUST_NET_HEARTBEAT_OVER_TIMER_ID))
        {
            track_start_timer(TRACK_CUST_NET_HEARTBEAT_OVER_TIMER_ID, G_parameter.hbt_acc_off * 2000, track_soc_gprs_reconnect, (void *)28);
        }
    }
}

/******************************************************************************
 *  Function    -  track_cust_cmd_operate
 *
 *  Purpose     -  解析收到的数据包
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-15,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/



#if 1
void track_cust_cmd_operate(nvram_bb_7edata_struct *data)
{
    kal_uint16 no = 0, result = 0, rev_cmd = 0, g_nvram_par = 0, g_nvram_impor = 0;
    kal_uint8 buffer[512];
    memset(buffer, 0, sizeof(buffer));
    LOGH(L_APP, L_V5, data->buf, data->buf_len);
    if(data->cmd == 0x8100)
    {
        track_cust_rev_8100_data(data);
    }
    else if(data->cmd == 0x8001)
    {
        track_cust_rev_8001_data(data);
    }
    else if(data->cmd ==  0x8201)
    {
        track_cust_rev_8201_data(data);
    }
    else if(data->cmd ==  0x8300)
    {
        track_cust_rev_8300_data(data);
    }
    else if(data->cmd ==  0x8103)
    {
        track_cust_rev_8103_data(data);
    }
    else if(data->cmd ==  0x8104)
    {
        track_cust_rev_8104_data(data);
    }
    else if(data->cmd ==  0x8500)
    {
        track_cust_rev_8500_data(data);
    }
    else if(data->cmd ==  0x8600)
    {
        track_cust_rev_8600_data(data);
    }
    else if(data->cmd ==  0x8601)
    {
        track_cust_rev_8601_data(data);
    }
    else
    {
        LOGD(L_APP, L_V5, "未定义协议号");
        track_cust_rev_error_data(data);
    }
}

#else
void track_cust_cmd_operate(nvram_bb_7edata_struct *data)
{
    kal_uint16 no = 0, result = 0, rev_cmd = 0, g_nvram_par = 0, g_nvram_impor = 0;
    kal_uint8 buffer[512];
    memset(buffer, 0, sizeof(buffer));
    LOGH(L_APP, L_V5, data->buf, data->buf_len);
    if(data->cmd == 0x8100)
    {
        no = (data->buf[0] << 8) & 0xFF00;
        no |= data->buf[1];
        result = data->buf[2];
        if(data->buf_len > 3)
        {
            memcpy(buffer, &data->buf[3], data->buf_len - 3);
            strcpy(G_parameter.bb_data.authorize_code, buffer);
        }

        LOGD(L_APP, L_V5, "no = %04x,result = %d", no, result);
        LOGD(L_APP, L_V5, "buffer= %s", buffer);
        LOGH(L_APP, L_V5, &data->buf[3], data->buf_len - 3);
        LOGH(L_APP, L_V5, buffer, data->buf_len - 3);
        Login_01_Information_Success =  KAL_TRUE;
        packet_login_bb((void *)2);
        g_nvram_par = 1;
    }
    else if(data->cmd == 0x8001)
    {
        no = (data->buf[0] << 8) & 0xFF00;
        no |= data->buf[1];
        rev_cmd = (data->buf[2] << 8) & 0xFF00;
        rev_cmd |= data->buf[3];
        result = data->buf[4];
        LOGD(L_APP, L_V5, "no = %04x,rev_cmd = %04x,result = %d", no, rev_cmd, result);
        if(rev_cmd == 0x0002)
        {
            LOGD(L_APP, L_V5, "心跳回复");
            track_cust_heartbeat_rev();
        }
        else if(rev_cmd ==  0x0102)
        {

            LOGD(L_APP, L_V5, "鉴权回复");
            Login_02_Information_Success =  KAL_TRUE;
            track_cust_login_rev();
        }
    }
    else if(data->cmd ==  0x8201)
    {
        LOGD(L_APP, L_V5, "位置查询");

    }
    else if(data->cmd ==  0x8300)
    {
        LOGD(L_APP, L_V5, "文本信息下发");

    }
    else if(data->cmd ==  0x8103)
    {
        LOGD(L_APP, L_V5, "设置终端参数");

    }
    else if(data->cmd ==  0x8104)
    {
        LOGD(L_APP, L_V5, "查询终端参数");

    }
    else if(data->cmd ==  0x8500)
    {
        LOGD(L_APP, L_V5, "车辆控制");
        result = data->buf[0] >> 6;
        if(result == 1)
        {
            LOGD(L_APP, L_V5, "回复油电");
        }
        else if(result == 2)
        {
            LOGD(L_APP, L_V5, "断开油电");
        }
    }
    else if(data->cmd ==  0x8600)
    {
        LOGD(L_APP, L_V5, "设置圆形区域");

    }



    if(g_nvram_par == 1)
    {
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
}
#endif /* 0 */

/******************************************************************************
 *  Function    -  track_cust_decode_7EData
 *
 *  Purpose     -  解析收到的7E数据
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-14,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_decode_7EData(kal_uint8 * buffer, kal_uint16 len)
{
    nvram_bb_7edata_struct data;
    LOGH(L_APP, L_V5, buffer, len);

    len = track_cust_check_rev_7EData(buffer, len);
    LOGH(L_APP, L_V5, buffer, len);
    //获取协议号
    data.cmd = (buffer[1] << 8) & 0xFF00;
    data.cmd |= buffer[2];
    LOGD(L_APP, L_V5, " cmd = %04x", data.cmd);
    //获取数据长度
    data.buf_len = ((buffer[3] & 0x01) << 8) & 0xFF00;
    data.buf_len |= buffer[4];
    LOGD(L_APP, L_V5, " buf_len = %04x; %d", data.buf_len, data.buf_len);
    //获取服务器下发流水号
    LOGD(L_APP, L_V5, " buffer[11+data.buf_len] = %02X,buffer[12+data.buf_len] =%02X", buffer[11+data.buf_len], buffer[12+data.buf_len]);
    data.rev_serialnumber = (buffer[11+data.buf_len] << 8) & 0xFF00;
    data.rev_serialnumber |= buffer[12+data.buf_len];
    LOGD(L_APP, L_V5, " rev_serialnumber = %04x; %d", data.rev_serialnumber, data.rev_serialnumber);
    //获取消息体内容
    memset(data.buf, 0, sizeof(data.buf));
    memcpy(data.buf, &buffer[13], data.buf_len);
    LOGH(L_APP, L_V5, data.buf, data.buf_len);
    track_cust_cmd_operate(&data);
}



/******************************************************************************
 *  Function    -  packet_login_bb
 *
 *  Purpose     -  登录包发送
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-15,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
void packet_login_bb(void * par)
{
    static kal_uint8 count1 = 0, recv_count = 0, count2 = 0;
    LOGD(L_APP, L_V5, "mode = %d,par=%d, count1=%d, count2=%d,recv_count=%d", G_importance_parameter_data.login_data.mode, (kal_uint32)par, count1, count2, recv_count);
    if(par == NULL)
    {
        recv_count = 0;
        count1 = 0;
        count2 = 0;
    }
    else if((U32)par == 1)
    {
        recv_count++;
        if(recv_count == 4)
        {
            track_os_intoTaskMsgQueueExt(track_soc_gprs_reconnect, (void *)24);
        }
        return;
    }
    if(count1 < 6 && (G_importance_parameter_data.login_data.mode == 1 && Login_01_Information_Success == KAL_FALSE))
    {

        LOGD(L_APP, L_V5, "@@@@1");
        count1++;
        track_cust_paket_0100();
        if(!track_is_timer_run(TRACK_CUST_NET_LOGIN_RESEND_TIMER_ID))
        {
            LOGD(L_APP, L_V5, "@@@@1 #1");
            track_start_timer(TRACK_CUST_NET_LOGIN_RESEND_TIMER_ID, 30 * 1000, packet_login_bb, (void*)2);
        }
        if(!track_is_timer_run(TRACK_CUST_NET_LOGIN_TIMEOUT_TIMER_ID))
        {
            LOGD(L_APP, L_V5, "@@@@1 #2");
            track_start_timer(TRACK_CUST_NET_LOGIN_TIMEOUT_TIMER_ID, 200 * 1000, track_soc_gprs_reconnect, (void *)25);
        }
    }
    else if(count2 < 10 && ((G_importance_parameter_data.login_data.mode == 2 && Login_02_Information_Success == KAL_FALSE) || (G_importance_parameter_data.login_data.mode == 1)))
    {
        LOGD(L_APP, L_V5, "@@@@2");
        if(count2 == 0 && count1 >= 6)
        {
            track_stop_timer(TRACK_CUST_NET_LOGIN_RESEND_TIMER_ID);
            track_stop_timer(TRACK_CUST_NET_LOGIN_TIMEOUT_TIMER_ID);
        }
        count2++;
        track_cust_paket_0102();
        if(!track_is_timer_run(TRACK_CUST_NET_LOGIN_RESEND_TIMER_ID))
        {
            track_start_timer(TRACK_CUST_NET_LOGIN_RESEND_TIMER_ID, 10000, packet_login_bb, (void*)2);
        }
        if(!track_is_timer_run(TRACK_CUST_NET_LOGIN_TIMEOUT_TIMER_ID))
        {
            track_start_timer(TRACK_CUST_NET_LOGIN_TIMEOUT_TIMER_ID, 120 * 1000, track_soc_gprs_reconnect, (void *)25);
        }
    }
    else
    {
        track_cust_login_rev();
        //track_cust_paket_0200();
    }
}
/******************************************************************************
 *  Function    -  track_cust_bb_set_soc_close_status
 *
 *  Purpose     -  重置网络状态
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-15,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_bb_set_soc_close_status(void)
{
    LOGD(L_APP, L_V5, "");
    Login_01_Information_Success =  KAL_FALSE;
    Login_02_Information_Success =  KAL_FALSE;
}


/******************************************************************************
 *  Function    -  track_cust_check_login_01_information
 *
 *  Purpose     -  获取登录状态
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-23,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
kal_bool track_cust_check_login_01_information(void)
{
    return Login_01_Information_Success;
}

kal_bool track_cust_check_login_02_information(void)
{
    return Login_02_Information_Success;
}
/******************************************************************************
 *  Function    -  track_nt22_test_data
 *
 *  Purpose     -  测试代码
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-12,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/


void track_nt22_test_data(kal_uint16 index)
{
    track_gps_data_struct *gd;
    LBS_Multi_Cell_Data_Struct *lbs;
    kal_uint8 buf[1024];
    kal_uint8 buf1[1024];
    static kal_uint16 t_index = 0;


    if(index == 0)
    {
        track_cust_paket_0100();
    }
    else if(index == 1)
    {
        track_cust_paket_0102();
    }
    else if(index == 2)
    {
        if(t_index == 0)
        {
            track_cust_alarm_bb_data(TR_CUST_ALARM_SOS , 1);
            t_index++;
        }
        else
        {
            track_cust_alarm_bb_data(TR_CUST_ALARM_SOS , 0);
            t_index = 0;
        }
        track_cust_paket_0200(gd, lbs);
    }
    else if(index == 3)
    {
        track_cust_paket_0002();
    }
    else if(index == 4)
    {
        memset(buf, 0, sizeof(buf));
        memset(buf1, 0, sizeof(buf1));
        sprintf(buf, "7E8100000D0131000000120001004B0033313030303030303132E57E");
        LOGD(L_APP, L_V5, "buf = %s", buf);
        memcpy(buf1, track_fun_stringtohex(buf, 56), 28);
        LOGH(L_APP, L_V5, buf1, 28);
        track_cust_decode_7EData(buf1, 28);
    }
}



#endif /* __XYBB__ */

