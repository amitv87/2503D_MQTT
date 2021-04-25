/**********************************************************************************************************************************************************
*********************************************************LOCKER模块**************************************************************************************
soc_send 一次未完整发送完
mqtt_buf_send 一次发送的数据大于1024溢出
***********************************************************************************************************************************************************/
#include <stdio.h>

#include "track_fun_md5.h"
#include "MQTTPacket.h"
#include "MQTTConnect.h"
#include "MQTT_transport.h"
#include "MQTT_main.h"
#include "kal_general_types.h"

#include "stack_msgs.h"
#include "app_ltlcom.h"       	/* Task message communiction */
#include "custom_data_account.h"
#include "app2soc_struct.h"
#include "stack_config.h"
#include "stack_msgs.h"
#include "L4c_common_enum.h"
#include "cbm_api.h"
#include "soc_api.h"
#include "track_os_timer.h"
#include "track_os_paramete.h"
#include "track_fun_cJSON.h"
#include "c_RamBuffer.h"
#include "track_cust.h"
#include "track_os_ell.h"
#include "c_vector.h"


#define MAX_CONTENT_SIZE 1024
#define MAX_BUFFER_SIZE 512
#define MAX_DEVICE_SIZE 64

#define MIN_BATTERY_VOLTAGE 3500000
#define MAX_BATTERY_VOLTAGE 4100000
#define ALERT_ANGLE_THRESHOLD 50
#define MAX_RECONNECT_TIME 12

#define LOCATOR_TIMER_20MS      20
#define LOCATOR_TIMER_5S        5000
#define LOCATOR_TIMER_10S       10000
#define LOCATOR_TIMER_15S       15000
#define LOCATOR_TIMER_30S       30000
#define LOCATOR_TIMER_5M        300000
#define LOCATOR_TIMER_40S       40000
#define LOCATOR_TIMER_10M       600000



#define LED_OFF 0
#define LED_ON 1

#define IOT_HOST_NAME_REQUEST_ID     0
#define DEVICE_HOST_NAME_REQUEST_ID     1

#define LOCK_TRANSPORT_STATE                1
#define BOARD_NORMAL_STATE                1

typedef void (*PsFuncPtr)(void *);

typedef enum
{
    DNS_QIBEI,
    DNS_IOT,
    DNS_DEVICE
} DnsType;


static MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
product_tag_t product_tag = {0};
static char mqtt_clientid[100] = {0};
static char mqtt_publish_location_topic[100] = {0};
static char mqtt_publish_response_topic[100] = {0};
static char mqtt_publish_acceptor_topic[100] = {0};
static char mqtt_subscribe_topic[100] = {0};
static char mqtt_username[100] = {0};
static char mqtt_password[64] = {0};
static char mqtt_domain_name[100] = {0};

static sockaddr_struct qibei_addr = {0};
//static sockaddr_struct mqtt_addr = {0};
static sockaddr_struct device_addr = {0};

static int reconnect_time = 0;
static kal_bool keep_alive = KAL_TRUE;
static int lockState = 0;
static int boardState = 0;
int iotrecnum = 0;
int authrecnum = 0;

static kal_int8 qibei_handle = -1;
static kal_int8 iot_handle = -1;
//static kal_int8 vs_sock_handle = -1;

static kal_int8 vs_addr[100] = {0};
static unsigned short vs_port = 0;

static char mqtt_buf_send[1024];
static int mqtt_buf_send_len = 0;

static kal_uint8 track_cust_Login_Success = 0;

#if defined(__RVCT__)
#define StartTimer(a, b, c) tr_start_timer(a, b, c)
#define StopTimer(a) tr_stop_timer(a)
#define led1_control(a)
#define led2_control(a)
#define Beep(a)
#define dispatch(a)
//#define log(format, ...)  LOGS("[debug] %d %s()| "format"\n", __LINE__, __FUNCTION__,##__VA_ARGS__);
#define log(format, ...)  LOGD(L_APP, L_V6, format, ##__VA_ARGS__);
#else
#define Beep(a)
char* track_cust_heartbeat(void)
{
    return "";
}
#endif /* __RVCT__ */

//用户需要根据设备信息完善以下宏定义中的四元组内容
#if 0
#define PRODUCT_KEY    "1000084271"             //10长度
#define PRODUCT_SECRET "CAOK853rYkzik5Ke"       //16
#define DEVICE_NAME    "ofo000017429"           //12
#define DEVICE_SECRET  "6PyoLvbtF22dIq3D"       //16
#define DEVICE_ID      "0xclLlfQVfuBK0q2y"      //13
#endif /* 0 */

#define RECV_BUFFER_MAX    2048
static PsFuncPtr fun_notify_ind = NULL, fun_get_host_by_name_ind = NULL;
static int       mqtt_recv_buffer_len = 0;
static kal_uint8 mqtt_recv_buffer[RECV_BUFFER_MAX];
//static kal_uint8 mqtt_recv_buffer2[RECV_BUFFER_MAX];
static sockaddr_struct mqtt_ip_addr = {0};
static c_vector* vec_send_queue = NULL;
static kal_uint8 send_status = 1;

extern void track_cust_gprs_conn_fail(void *arg);
extern void track_check_tick(kal_uint8 st, char *outstring);
extern void track_cust_restart(kal_uint8 mode, kal_uint32 sec);
//extern void track_single_led_flash(kal_uint16 led, kal_uint16 status);
extern applib_time_struct str_to_time(char *data);
extern nvram_importance_parameter_struct G_importance_parameter_data;
extern void track_soc_send(void *par);
extern kal_uint16 track_cust_check_firstpoweron_flag(kal_uint16 arg);


//函数声明
static void iot_conn(void);
static void mqtt_conn(void);
static vec_send(void);
void delay_mqtt_conn(void);
void track_cust_change_pk_acount(void *arg);
void delay_MQTT_main(void);
void mqtt_ping_overtime(void);


#if 0
#endif /* 0 */
void MQTT_check_gprs_status(void)
{
    log("网络断开了");
    track_cust_Login_Success = 0;
    tr_start_timer(MQTT_CHECK_GPRS_STATUS, data.keepAliveInterval * 1000 + 60 * 1000, MQTT_check_gprs_status);
    mqtt_ping_overtime();
}

kal_uint8 MQTT_get_gprs_status(void)
{
    return track_cust_Login_Success;
}

static void set_rtc_time(char *data)
{
    applib_time_struct utcTime = str_to_time(data);
    LOGD(L_APP, L_V5, "");
    track_drv_update_time_to_system(utcTime);
}

static void ResetSystemNow(void)
{
    log("ResetSystemNow");
    track_cust_restart(70, 3);
}


static vec_reset(void)
{
    int v_size;
    kal_uint8 *buf;
    v_size = VECTOR_Size(vec_send_queue);
    LOGD(L_APP, L_V5, "v_size:%d", v_size);
    while(v_size > 0)
    {
        buf = VECTOR_At(vec_send_queue, 0);
        if(buf) Ram_Free(buf);
        VECTOR_Erase(vec_send_queue, 0, 1);
        v_size = VECTOR_Size(vec_send_queue);
    }
    send_status = 1;
}

static vec_del(void)
{
    int v_size;
    kal_uint8 *buf;
    v_size = VECTOR_Size(vec_send_queue);
    LOGD(L_APP, L_V5, "v_size:%d", v_size);
    if(v_size)
    {
        buf = VECTOR_At(vec_send_queue, 0);
        if(buf) Ram_Free(buf);
        VECTOR_Erase(vec_send_queue, 0, 1);
    }
}

static vec_forced_send(void)
{
    send_status = 1;
    LOGD(L_APP, L_V5, "");
    vec_send();
}

static vec_send(void)
{
    int v_size, len;
    kal_uint8 *buf;
    v_size = VECTOR_Size(vec_send_queue);
    LOGD(L_APP, L_V5, "v_size:%d, send_status:%d", v_size, send_status);
    if(v_size && send_status)
    {
        buf = VECTOR_At(vec_send_queue, 0);
        if(buf)
        {
            memcpy((kal_uint8*)&len, buf, 4);
            send_status = 0;
            Socket_write_req(0, buf + 4, len, 0, 0, 0, 0);
            //LOGH(L_APP, L_V5, buf + 4, len);
            StartTimer(LOCATOR_SEND_TIMER, 150000, vec_forced_send);
        }
    }
}

static vec_pull(kal_uint8 *data, int len)
{
    int v_size;
    kal_uint8 *buf;
    v_size = VECTOR_Size(vec_send_queue);
    buf = (kal_uint8*)Ram_Alloc(3, len + 4);
    if(buf)
    {
        memcpy(buf, (kal_uint8*)&len, 4);
        memcpy(buf + 4, data, len);
        VECTOR_PushBack(vec_send_queue, buf);
        LOGD(L_APP, L_V5, "v_size:%d, req_send_len:%d", v_size + 1, len);
        LOGH(L_APP, L_V7, data, len);
    }
    if(send_status) vec_send();
}

int transport_sendPacketBuffer(kal_uint16 queue_id, unsigned char* buf, int buflen)
{
    char buf1[100] = {0};
    applib_time_struct currTime_ams = {0};
    LOGD(L_APP, L_V5, "");
    if(queue_id != 0) //定位数据包，已经有队列的了。这里直接发
    {
            #if defined(__GT420D__)
                track_fun_get_time(&currTime_ams, KAL_FALSE, &G_parameter.zone);
                snprintf(buf1, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d send data\n\r",
                            currTime_ams.nYear,
                            currTime_ams.nMonth,
                            currTime_ams.nDay,
                            currTime_ams.nHour,
                            currTime_ams.nMin,
                            currTime_ams.nSec
                            );
        track_cust_save_logfile(buf1, strlen(buf1));
    #endif

        return Socket_write_req(0, buf, buflen, queue_id, 0, 0, 1);
    }
    else
    {
        //应答包这些，还是走自己的队列就行了
        vec_pull(buf, buflen);
        return buflen;
    }

}

int transport_getdata(unsigned char* buf, int count)
{
    LOGD(L_APP, L_V5, "count:%d, mqtt_recv_buffer_len:%d", count, mqtt_recv_buffer_len);
    if(mqtt_recv_buffer_len < count) count = mqtt_recv_buffer_len;
    if(count > 0)
    {
        memcpy(buf, mqtt_recv_buffer, count);
        mqtt_recv_buffer_len -= count;
        if(mqtt_recv_buffer_len > 0);
        {
            //memcpy(mqtt_recv_buffer2, &mqtt_recv_buffer[count], mqtt_recv_buffer_len);
            //memcpy(mqtt_recv_buffer, mqtt_recv_buffer2, mqtt_recv_buffer_len);
            OTA_memmove(mqtt_recv_buffer, &mqtt_recv_buffer[count], mqtt_recv_buffer_len + 1);
        }
        LOGH(L_CMD, L_V1, buf, count);
    }
    return count;
}

void get_device_info()
{
    char input[256] = {0};
    char output[20] = {0};
    int i = 0;

    snprintf(product_tag.productKey, 12, G_importance_parameter_data.mqtt_login.product_key);//PRODUCT_KEY);
    snprintf(product_tag.productSecret, 48, G_importance_parameter_data.mqtt_login.product_secret);//PRODUCT_SECRET);
    snprintf(product_tag.deviceSecret, 48, G_importance_parameter_data.mqtt_login.device_secret);//DEVICE_SECRET);
    snprintf(product_tag.deviceName, 16, (char*)track_drv_get_imei(0));//DEVICE_NAME);
    snprintf(product_tag.deviceId, 48, "%s.%s", product_tag.productKey, product_tag.deviceName);

    log("[LOCKER] iot_appid = %s", product_tag.productKey);
    log("[LOCKER] iot_app_secret = %s", product_tag.productSecret);
    log("[LOCKER] client_id = %s", product_tag.deviceId);
    log("[LOCKER] secret_code = %s", product_tag.deviceSecret);

    snprintf(mqtt_clientid, 64, "%s|securemode=3,signmethod=hmacsha1|", product_tag.deviceId);
    snprintf(mqtt_publish_location_topic, 64, "/%s/%s/user/track", product_tag.productKey, product_tag.deviceName);
    snprintf(mqtt_publish_response_topic, 64, "/%s/%s/user/controlStatus", product_tag.productKey, product_tag.deviceName);
    snprintf(mqtt_subscribe_topic, 64, "/%s/%s/user/control", product_tag.productKey, product_tag.deviceName);
    snprintf(mqtt_publish_acceptor_topic, 64, "/%s/%s/user/acceptor", product_tag.productKey, product_tag.deviceName);
    snprintf(mqtt_username, 64, "%s&%s", product_tag.deviceName, product_tag.productKey);

    snprintf(input, 255, "clientId%sdeviceName%sproductKey%s", product_tag.deviceId, product_tag.deviceName, product_tag.productKey);
    log("[LOCKER] input = %s", input);
    aliot_hmac_sha1(input, strlen(input), mqtt_password, product_tag.deviceSecret, 32);

    log("[LOCKER] ---------------------------");
    log("[LOCKER] mqtt_clientid = %s", mqtt_clientid);
    log("[LOCKER] mqtt_publish_location_topic = %s", mqtt_publish_location_topic);
    log("[LOCKER] mqtt_publish_response_topic = %s", mqtt_publish_response_topic);
    log("[LOCKER] mqtt_subscribe_topic = %s", mqtt_subscribe_topic);
    log("[LOCKER] mqtt_acceptor_topic = %s", mqtt_publish_acceptor_topic);
    log("[LOCKER] mqtt_username = %s", mqtt_username);
    log("[LOCKER] mqtt_password = %s", mqtt_password);
    log("[LOCKER] device_id = %s", product_tag.deviceId);
    log("[LOCKER] ---------------------------");
}

// 设置MQTT参数信息(用户名，ID, etc...)
int mqtt_init()
{
    get_device_info();
    data.clientID.cstring = mqtt_clientid;
    data.keepAliveInterval = 180;   //The MAX value is 180S
    data.cleansession = 0;
    data.will.qos = 1;
    data.username.cstring = mqtt_username;
    data.password.cstring = mqtt_password;
    //data.MQTTVersion = 4;
    return 0;
}


// hmac_sha1签名
int iot_get_sign(char *sign)
{
    unsigned char uc_input[150];
    unsigned char uc_sign[200];
    unsigned char digest[16];
    unsigned int  random;
    int i = 0;
    
    random =track_drv_rand();
    memset(uc_input, 0, 150);
    sprintf(uc_input, "deviceName%sproductKey%srandom%d", product_tag.deviceName, product_tag.productKey, random);
    log("[LOCKER] key=%s,msg = %s", product_tag.productSecret,uc_input);

    memset(uc_sign, 0, 200);
    aliot_hmac_sha1(uc_input, strlen(uc_input), uc_sign, product_tag.productSecret, strlen(product_tag.productSecret));
    /*
    MDString(uc_input, digest);
    for(i = 0; i < 16; i++)
    {
        sprintf(uc_sign + i * 2, "%02X", digest[i]);
    }
    */
    log("[LOCKER] md5 = %s len:%d", uc_sign,strlen(uc_sign));

    memset(sign, 0, 500);
    sprintf(sign, "productKey=%s&deviceName=%s&random=%d&sign=%s&signMethod=Hmacsha1",
            product_tag.productKey,
            product_tag.deviceName,
            random,
            uc_sign);
    log("[LOCKER] sign = %s,len:%d", sign,strlen(sign));

    return 0;
}


int packageIotRequest(char *data)
{
    unsigned char poststr[1000];
    unsigned char sign[500];
    int m_len = 0;
    int m_send = 0;
    kal_int8 ret;

    //iot_get_sign(sign);
    snprintf(sign,500,
                "uid=%s&productKey=%s&deviceName=%s",
                G_importance_parameter_data.dasouche_login.uid,
                G_importance_parameter_data.mqtt_login.product_key,
                (char*)track_drv_get_imei(0));
    memset(data, 0, 1000);
    snprintf(data, 1000,
             "POST %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Content-type: application/x-www-form-urlencoded\r\n"
             "Content-length: %lu\r\n\r\n"
             "%s\r\n", IOT_APP_PATH, IOT_SERVER, strlen(sign), sign);

    return strlen(data);
}

//向阿里授权服务器请求的数据
//连接阿里授权服务器的SOCKET回调
void mqtt_ping_overtime(void)
{
    kal_uint8 server_ip[4] = {0};   // 十进制
    extern void MQTT_main(void);

    track_cust_Login_Success = FALSE;
    //soc_close(vs_sock_handle);
    log("[LOCKER] 断网一次，重连, RECONNECT IT,FAILED time:%d", iotrecnum++);
    // 网络断线，重新连接服务器
    //vs_sock_handle = -1;
    StopTimer(LOCATOR_MQTT_TIMER);
    StopTimer(LOCATOR_HEARTBEAT_TIMER);
    delay_mqtt_conn();
    log("[LOCKER] Notify MQTT SOCKET CLOSE!");
}

// PING
void mqtt_pingreq(void)
{
    int rc = 0;
    unsigned char buf[200];
    int buflen = sizeof(buf);
    int len = 0;

    LOGD(L_APP, L_V5, "");
    if(keep_alive == KAL_TRUE)
    {
        keep_alive = KAL_FALSE;

        len = MQTTSerialize_pingreq(buf, buflen);
        rc = transport_sendPacketBuffer(0, buf, len);
        log("[LOCKER] PING REQUEST...");
        StartTimer(LOCATOR_PING_TIMER, data.keepAliveInterval * 1000, mqtt_pingreq);
    }
    else
    {
        //soc_close(vs_sock_handle);
        track_cust_Login_Success = FALSE;
        log("[LOCKER] NETWORK DISCONNECT, RECONNECT IT,FAILED time:%d", iotrecnum++);
        // 网络断线，重新连接服务器
        //vs_sock_handle = -1;
        StopTimer(LOCATOR_MQTT_TIMER);
        StopTimer(LOCATOR_HEARTBEAT_TIMER);
        StopTimer(LOCATOR_PING_TIMER);
        delay_mqtt_conn();
    }
}

// 收到发布信息的确认？
void mqtt_publish_ack(int qos, unsigned char dup, unsigned short msg_id)
{
    int rc = 0;
    unsigned char buf[200];
    int buflen = sizeof(buf);
    int len = 0;
    LOGD(L_APP, L_V5, "");
    if(qos == 1)
        len = MQTTSerialize_puback(buf, buflen, msg_id);
    else
        len = MQTTSerialize_pubrel(buf, buflen, dup, msg_id);
    rc = transport_sendPacketBuffer(0, buf, len);
    log("[LOCKER] PUBLISH ACK [qos=%d, dup=%d, msg_id=%d]", qos, dup, msg_id);
}

// 发布
int mqtt_publish(kal_uint16 queue_id,const char* msg, const int length,kal_uint8 topic_type)
{
    int rc = 0;
    char buf[MAX_CONTENT_SIZE];
    int buflen = sizeof(buf);
    MQTTString topicString = MQTTString_initializer;
    char topic[50];
    int len = 0;

    LOGD(L_APP, L_V5, "");
    if(length > MAX_CONTENT_SIZE)
    {
        return 0;
    }
    LOGD(L_APP, L_V5, "[%s]\n", msg);
    if(topic_type == 1)
    {
        topicString.cstring = mqtt_publish_response_topic;
    }
    else if(topic_type == 0)
    {
        topicString.cstring = mqtt_publish_location_topic;
    }
    else if(topic_type == 2)
    {
        topicString.cstring = mqtt_publish_acceptor_topic;
    }
    len += MQTTSerialize_publish((unsigned char*)(buf + len), buflen - len, 0, data.will.qos, 0, 0, topicString, (unsigned char*)msg, length);    
    rc = transport_sendPacketBuffer(queue_id, buf, len);
    return rc;
}

static void mqtt_backup_pk_ds(void)
{
    if(strcmp(G_importance_parameter_data.mqtt_login.product_key,G_iot.product_key))
    {
        snprintf(G_iot.product_key,20,G_importance_parameter_data.mqtt_login.product_key);
        snprintf(G_iot.product_secret,48,G_importance_parameter_data.mqtt_login.product_secret);
        snprintf(G_iot.device_secret,48,G_importance_parameter_data.mqtt_login.device_secret);
        Track_nvram_write(NVRAM_EF_IOT_PARAMS_LID, 1, (void *)&G_iot, NVRAM_EF_IOT_PARAMS_SIZE);
    }
}
static void mqtt_pk_ds_form_backup(void)
{

        snprintf(G_importance_parameter_data.mqtt_login.product_key,20,G_iot.product_key);
        snprintf(G_importance_parameter_data.mqtt_login.product_secret,48,G_iot.product_secret);
        snprintf(G_importance_parameter_data.mqtt_login.device_secret,48,G_iot.device_secret);
        Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        delay_MQTT_main();
}
static void recv_mqtt(void)
{
    static kal_uint8 flg = 0;
    char ams_buf[100] = {0};
    applib_time_struct currTime = {0};
    while(mqtt_recv_buffer_len > 0)
    {
        unsigned char buf[300];
        int buflen = sizeof(buf);
        int len = 0;
        int rc_read = 0;
        int rc = 0;
        int ack = 0;
        int req_qos = 1;

        memset(buf, 0, 300);
        ack = MQTTPacket_read(buf, buflen, transport_getdata);
        log("[LOCKER]Received MQTT message, ACTION=%d", ack);
        if(ack == CONNACK)
        {
            unsigned char sessionPresent, connack_rc;
            log("[LOCKER]recv_mqtt->CONNACK");
            if(MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
            {
                log("[LOCKER] Unable to connect, return code %d", connack_rc);
                track_cust_Login_Success = 0;
                #if defined(__GT420D__)
                track_fun_get_time(&currTime, KAL_FALSE, &G_parameter.zone);
                snprintf(ams_buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d link down\n\r",
                            currTime.nYear,
                            currTime.nMonth,
                            currTime.nDay,
                            currTime.nHour,
                            currTime.nMin,
                            currTime.nSec);
                LOGD(5,5,"len:%d",strlen(ams_buf));
                track_cust_save_logfile(ams_buf, strlen(ams_buf));
                #endif
                track_soc_conn_status(KAL_FALSE);
                socket_disconnect(0);
                StartTimer(LOCATOR_MQTT_TIMER, LOCATOR_TIMER_30S, mqtt_conn);
            }
            else
            {
                unsigned char buf[300];
                int buflen = sizeof(buf);
                int msgid = 1;
                MQTTString topicString = MQTTString_initializer;
                int req_qos = 1;
                int len = 0;

                /* subscribe *//*此处添加客户端订阅信息的topic*/
                topicString.cstring = mqtt_subscribe_topic;

                log("[LOCKER]Topic:%s start to SUBSCRIBE!", mqtt_subscribe_topic);
                track_cust_Login_Success = 1;
                #if defined(__GT420D__)
                track_fun_get_time(&currTime, KAL_FALSE, &G_parameter.zone);
                snprintf(ams_buf, 99, "time: %d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d link up\n\r",
                            currTime.nYear,
                            currTime.nMonth,
                            currTime.nDay,
                            currTime.nHour,
                            currTime.nMin,
                            currTime.nSec);
                LOGD(5,5,"len:%d",strlen(ams_buf));
                track_cust_save_logfile(ams_buf, strlen(ams_buf));
                #endif
                if(track_cust_check_firstpoweron_flag(99) == 1)
                {
                    //track_cust_poweron_msg();
                    if(G_parameter.wifi.sw == 0 && track_cust_check_gpsflag_status(99) == 2 && G_parameter.extchip.mode == 2)
                    {
                        LOGD(5,5,"GPS WIFI开关都关着，连上网发现还没传过点，上报一个LBS点");
                        track_cust_sendlbs_limit();
                        }
                }
                track_soc_conn_status(KAL_TRUE);
                track_soc_send((void*)99);
                len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);

                rc = transport_sendPacketBuffer(0, buf, len);
                #if 0
                //连接成功，备份此时的PK,DS
                mqtt_backup_pk_ds();     
                if(G_parameter.changepk.is_change_pk && G_parameter.changepk.reconn_acount >=G_parameter.changepk.pk_factory)
                {
                    track_cust_24H_factory_pk_rsp();
                }
                track_cust_change_pk_acount((void*)0);    
                #endif
                log("[LOCKER] send=%d", rc);
            }
        }
        else if(ack == SUBACK)  /* wait for suback */
        {
            unsigned short submsgid;
            int subcount;
            int granted_qos;
            log("[LOCKER]recv_mqtt->SUBACK");
            rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
            if(flg == 0)
            {
                flg = 1;
                //track_single_led_flash(8, 0);
            }
            if(granted_qos != req_qos)
            {
                log("[LOCKER] granted qos != %d, %d", req_qos, granted_qos);
            }


            log("[LOCKER] SUBSCRIBE SUCCESSFUL!");
            // 启动定时心跳任务

            Beep(5);

            keep_alive = KAL_TRUE;
            StartTimer(LOCATOR_PING_TIMER, LOCATOR_TIMER_20MS, mqtt_pingreq);
            StopTimer(LOCATOR_MQTT_TIMER_MQTT_MAIN);
            led1_control(LED_OFF);
            led2_control(LED_ON);
        }
        else if(ack == PUBLISH)
        {
            unsigned char dup;
            int qos;
            unsigned char retained;
            unsigned short msgid;
            int payloadlen_in;
            unsigned char* payload_in;
            MQTTString receivedTopic;
            log("[LOCKER]recv_mqtt->PUBLISH");
            rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
                                         &payload_in, &payloadlen_in, buf, buflen);
            if(payload_in != 0)
            {
                if(qos > 0)
                {
                    mqtt_publish_ack(qos, dup, msgid);
                }
                log("[LOCKER]QOS:%d,Topic Received:%s", qos, payload_in);
                dispatch(payload_in);
                track_cust_mqtt_rev_data(payload_in);
            }
            else
            {
                log("[LOCKER]Topic Received failed, result:%d", rc);
            }
        }
        else if(ack == PINGRESP)
        {
            keep_alive = KAL_TRUE;
            track_cust_gprs_conn_fail((void*)1);
            track_check_tick(1, NULL);
            log("[LOCKER]recv_mqtt->PINGRESP");
        }
        else
        {
            log("[LOCKER]recv_mqtt->else");
            keep_alive = KAL_TRUE;
        }
        tr_start_timer(MQTT_CHECK_GPRS_STATUS, data.keepAliveInterval * 1000 + 60 * 1000, MQTT_check_gprs_status);
    }
}

//连接MQTT服务器的SOCKET回调
static void event_aliyun_mqtt(track_soc_Callback_struct *par)
{
    static kal_uint32 re_conn = 0;
    LOGD(L_APP, L_V5, "status=%d error=%d", par->status, par->errorid);
    switch(par->status)
    {
        case SOC_CONNECT_SUCCESS:
            {
                //向阿里MQTT服务器请求的数据
                int rc = 0;
                int buflen = sizeof(mqtt_buf_send);
                StopTimer(LOCATOR_HEARTBEAT_TIMER);
                vec_reset();
                mqtt_buf_send_len = MQTTSerialize_connect(mqtt_buf_send, buflen, &data);
                transport_sendPacketBuffer(0, (kal_uint8*)mqtt_buf_send, mqtt_buf_send_len);
                //Socket_write_req(0, mqtt_buf_send, mqtt_buf_send_len, 0, 0, 0, 0);
                log("[LOCKER] send=%d", mqtt_buf_send_len);
                break;
            }
        case SOC_CONNECT_ServerNotResponding:
            {
                track_cust_Login_Success = 0;
                break;
            }
        case SOC_CONNECT_CLOSE:
            {
                track_cust_Login_Success = 0;
                break;
            }
        case SOC_RECV_DATA:
            {
                log("[LOCKER] Notify MQTT SOCKET READ %d, mqtt_recv_buffer_len:%d", par->recvBuffLen, mqtt_recv_buffer_len);
                LOGH(L_APP, L_V5, par->recvBuff, par->recvBuffLen);
                if(mqtt_recv_buffer_len < 0) mqtt_recv_buffer_len = 0;
                if(par->recvBuffLen > 0)
                {
                    if(mqtt_recv_buffer_len + par->recvBuffLen < RECV_BUFFER_MAX)
                    {
                        memcpy(&mqtt_recv_buffer[mqtt_recv_buffer_len], par->recvBuff, par->recvBuffLen);
                        mqtt_recv_buffer_len += par->recvBuffLen;
                    }
                    else if(par->recvBuffLen < RECV_BUFFER_MAX)
                    {
                        memcpy(mqtt_recv_buffer, par->recvBuff, par->recvBuffLen);
                        mqtt_recv_buffer_len = par->recvBuffLen;
                    }
                }
                recv_mqtt();
                break;
            }
        case SOC_SEND_DATA:
            {
                if(par->errorid != 0)
                {
                    LOGD(L_APP, L_V1, "ERROR!!! id=%d", par->errorid);
                    //send_status = 1; ???为什么是1?
                    send_status = 0;
                }
                else
                {
                    
                    LOGD(L_APP, L_V1, "SOC_SEND_DATA !!!!!par->queue_id=%d!!!!",par->queue_id);   
                    if(par->queue_id == 0)
                    {
                        vec_del();
                    }
                    if((par->queue_id >= 1) && (par->queue_id < 9))
                    {
                        track_os_intoTaskMsgQueueExt(track_soc_send, (void*)par->queue_id);
                    }
                    else
                    {
                        LOGD(L_SOC, L_V1, "ERROR!!! no=%d", par->queue_id);
                    }                                        
                    send_status = 1;
                    track_os_intoTaskMsgQueue(vec_send);
                }
                break;
            }
        case SOC_RECV_HOST_BY_NAME:
            {
                if(par->errorid != 0)
                {
                }
                else
                {
                    int i, ret;
                    sockaddr_struct ip_addr = {0};
                    for(i = 0; i < par->num_entry; i++)
                    {
                        LOGD(L_SOC, L_V5, "域名解析[%d] (%d.%d.%d.%d)", i,
                             par->ip_entry[i].address[0],
                             par->ip_entry[i].address[1],
                             par->ip_entry[i].address[2],
                             par->ip_entry[i].address[3]);
                    }
                    //memcpy(mqtt_addr.addr, par->ip_entry[0].address, 4);
                    ip_addr.addr_len = 4;
                    if(G_parameter.transit.IsTransit ==1)
                    {
                        ip_addr.port = G_parameter.transit.server_port;
                        memcpy(ip_addr.addr, G_parameter.transit.server_ip, 4);
                    }
                    else
                    {
                        ip_addr.port = 1883;
                        memcpy(ip_addr.addr, par->ip_entry[0].address, 4);
                    }
                    ret = Socket_conn_req(0, &ip_addr, event_aliyun_mqtt, 0);//连接MQTT服务器
                }
                break;
            }
    }
}

static void mqtt_conn(void)
{
    int ret;
    static kal_uint8 flg = 0;
    LOGD(L_APP, L_V5, "");
#if 0    
    if(G_realtime_data.Factory_test == 1)
    {
        LOGD(L_APP, L_V5, "Factory_test");
        return;
    }
    if(G_realtime_data.PWRSAVE)
    {
        LOGD(L_APP, L_V5, "运输模式");
        return;
    }
    if(track_fly_status(0xff))
    {
        LOGD(L_APP, L_V5, "飞行模式");
        return;
    }
#endif
    socket_disconnect(0);
    mqtt_init();
    authrecnum++;
    iotrecnum++;
    if(flg == 0)
    {
        flg = 1;
    }
    else
    {
    }
    sprintf(mqtt_domain_name, MQTT_SERVER, G_importance_parameter_data.mqtt_login.product_key);
    LOGD(L_APP, L_V5, "HTTPS://%s",mqtt_domain_name);
    track_cust_Login_Success = FALSE;
    Socket_gethostbyname(mqtt_domain_name, event_aliyun_mqtt);
}
void delay_iot_conn(void)
{
    LOGD(L_APP, L_V5, "");
    if(!track_is_timer_run(LOCATOR_IOT_TIMER))
        StartTimer(LOCATOR_IOT_TIMER, 3000, iot_conn);
}
void delay_mqtt_conn(void)
{
    LOGD(L_APP, L_V5, "");
    if(!track_is_timer_run(LOCATOR_MQTT_TIMER))
        StartTimer(LOCATOR_MQTT_TIMER, LOCATOR_TIMER_30S, mqtt_conn);
}
void delay_MQTT_main(void)
{
    LOGD(L_APP, L_V5, "");
    if(!track_is_timer_run(LOCATOR_MQTT_TIMER_MQTT_MAIN))
    {
        get_device_info();
        StartTimer(LOCATOR_MQTT_TIMER_MQTT_MAIN, 5000, MQTT_main);
    }
}
static void aliyun_req_send(void)
{
    char buffer[1000] = {0};
    int size = packageIotRequest(buffer);
    log("[LOCKER] Enter IOT CALLBACK, CONNECT SUCCESSFUL!");
    log("[LOCKER] %dbuff:%s",size,buffer);
    Socket_write_req(0, buffer, size, 0, 0, 0, 0);
}

//连接大搜车设备认证平台
static void event_dasouche(track_soc_Callback_struct *par)
{
    static kal_uint32 re_conn = 0;
    int ret = 0;
    LOGD(L_APP, L_V5, "status=%d error=%d", par->status, par->errorid);
    switch(par->status)
    {
        case SOC_CONNECT_SUCCESS:
            {
                //向认证平台请求获取Devicesecret
                aliyun_req_send();
                break;
            }
        case SOC_CONNECT_ServerNotResponding:
            {
                break;
            }
        case SOC_CONNECT_CLOSE:
            {
                break;
            }
        case SOC_RECV_DATA:
            {
                char *p;
                char *end_p;
                log("[LOCKER] Enter IOT CALLBACK, READ content from Auth Server:%d\r\n|%s|\r\n", par->recvBuffLen, par->recvBuff);
                //LOGH(L_APP, L_V5, par->recvBuff, par->recvBuffLen);
                track_cust_iot_rev_data(par->recvBuff);
                StartTimer(LOCATOR_MQTT_TIMER, 3000, mqtt_conn);
                socket_disconnect(0);
                break;
            }
        case SOC_SEND_DATA:
            {
                if(par->errorid != 0)
                {
                    LOGD(L_APP, L_V1, "ERROR!!! id=%d", par->errorid);
                }
                else
                {
                }
                break;
            }
        case SOC_RECV_HOST_BY_NAME:
            {
                if(par->errorid != 0)
                {
                }
                else
                {
                    int i, ret;
                    sockaddr_struct ip_addr = {0};
                    for(i = 0; i < par->num_entry; i++)
                    {
                        LOGD(L_SOC, L_V5, "域名解析[%d] (%d.%d.%d.%d)", i,
                             par->ip_entry[i].address[0],
                             par->ip_entry[i].address[1],
                             par->ip_entry[i].address[2],
                             par->ip_entry[i].address[3]);
                    }
                    ip_addr.addr_len = 4;
                    ip_addr.port = 80;
                    memcpy(ip_addr.addr, par->ip_entry[0].address, 4);
                    ret = Socket_conn_req(0, &ip_addr, event_dasouche, 0);//连接
                }
                break;
            }
    }
}

static void iot_conn(void)
{
    track_cust_Login_Success = FALSE;
    iotrecnum++;
    LOGD(L_SOC, L_V5, "url:%s",IOT_SERVER);
    Socket_gethostbyname(IOT_SERVER, event_dasouche);
}

// 初始化
void MQTT_main(void)
{
    char *PK="a1O8XYuHyRu";
    socket_disconnect(0);
    StopTimer(LOCATOR_PING_TIMER);
    StopTimer(LOCATOR_MQTT_TIMER);
    StartTimer(LOCATOR_HEARTBEAT_TIMER, 3 * 60000, MQTT_main);
    if(NULL == vec_send_queue)
    {
        VECTOR_CreateInstance(vec_send_queue);
    }
    #if 0
    if(G_parameter.changepk.is_change_pk && G_parameter.changepk.reconn_acount ==G_parameter.changepk.pk_factory)
    {
        mqtt_pk_ds_form_backup();
    }
    if(strcmp(G_importance_parameter_data.mqtt_login.device_name,(char*)track_drv_get_imei(0)))
    {
        //IMEI丢失
        snprintf(G_importance_parameter_data.mqtt_login.product_key,20,PK);
        Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
    }
    
    if(!strcmp(G_importance_parameter_data.mqtt_login.product_key,PK))//判断是否走认证渠道
    {
           iot_conn(); 
    }
    else if(!strcmp(G_importance_parameter_data.mqtt_login.product_key,G_iot.product_key))
    {
           iot_conn(); 
    }
    else
    {
           mqtt_conn();
    }
    #endif
    mqtt_conn();
}

void track_cust_disconnect(void)
{
    socket_disconnect(0);
}
void track_cust_change_pk_acount(void *arg)
{
    kal_uint32 status = (kal_uint32)arg;
    if(status == 2)
    {
        if(!G_parameter.changepk.is_change_pk)
        {
            LOGD(L_APP, L_V5, "靓仔，不要乱搞PK改动");
            G_parameter.changepk.is_change_pk = 1;
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);    
        }
    }    
    if(status == 1)
    {
        if(G_parameter.changepk.is_change_pk)
        {
            LOGD(L_APP, L_V5, "靓仔，你改了PK后，上线不了了，当我计算到%d,就恢复成上次的PK，现在是%d",G_parameter.changepk.pk_factory,G_parameter.changepk.reconn_acount);
            G_parameter.changepk.reconn_acount +=1;
            Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);  
        }     
    }
    if(status == 0)
    {
        if(!G_parameter.changepk.is_change_pk)
        {
            return;
        }
        LOGD(L_APP, L_V5, "靓仔，算你好野，PK改完冇事");
        G_parameter.changepk.is_change_pk = 0;
        G_parameter.changepk.reconn_acount = 0;
        Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
}
