#if defined(__XCWS__)
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
#include "Conversions.h"
#include <math.h>
#include "us_timer.h"
#include "uem_context.h"
#include "track_at_call.h"
#include "track_os_ell.h"
#include "track_nvram_default_value.h"
/*****************************************************************************
*  Define                               �궨��
******************************************************************************/


/*****************************************************************************
*  Typedef  Enum                        ö�ٶ���
*****************************************************************************/


/*****************************************************************************
*  Struct                               ���ݽṹ����
******************************************************************************/

typedef struct
{
    char**          cmd;
    kal_uint8       count;
    Cmd_Type        type;
} cmd_online_struct;

typedef void (*Func_CustRecvCmdPtr)(char*, char*, Cmd_Type);

typedef struct
{
    char                    *cmd_string;
    Func_CustRecvCmdPtr     func_ptr;
} cust_recv_cmd_struct;

typedef void (*Func_CustRecvPtr)(cmd_online_struct*);

typedef struct
{
    const char           *cmd_string;
    Func_CustRecvPtr     func_ptr;
} cust_recv_struct;

/*****************************************************************************
*  Local variable                       �ֲ�����
*****************************************************************************/
static char *blank = {""};

/****************************************************************************
*  Global Variable                      ȫ�ֱ���
*****************************************************************************/
static CMD_DATA_STRUCT *sms_cmd={0};
static char buf[300] = {0};
/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/
//extern kal_bool sleep_up_sign;

/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/
extern void track_set_login_status(kal_bool sta);
extern kal_bool track_soc_login_status(void);
extern void track_soc_send(void *par);
extern int parseInt(char * str);
extern void set_nvram_device_setting(void);
extern void heartbeat_packets_cq(void);
/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/

/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/
static kal_int8 check_str_is_bool_type(char *str)
{
    kal_int8 flag = 0;

    if(str == NULL)
    {
        return -1;
    }

    if(strlen(str) != 1)
    {
        return -2;
    }

    if(*str == '0')
    {
        return 0;
    }
    else if(*str == '1')
    {
        return 1;
    }
    else
    {
        return -3;
    }
}

static void check_status_gpsstatus(char *buf)
{
    gps_gpgsv_struct  *gpgsvsta;
    char str_buf[60] = {0}, state[10] = {0};
    kal_bool flag = KAL_FALSE;
    int i;
    gpgsvsta = track_drv_get_gps_GSV();
    memset(str_buf, 0, 60);
    if(gpgsvsta->Satellites_In_View)
    {
        for(i = 0; i < 12; i++)
        {
            LOGD(L_APP, L_V5, "%d,%d", gpgsvsta->Satellite_ID[i], gpgsvsta->SNR[i]);
            if(gpgsvsta->SNR[i] <= 99 && gpgsvsta->Satellite_ID[i] >= 1 && gpgsvsta->Satellite_ID[i] <= 32)
            {
                memset(state, 0, 10);
                if(flag)
                    sprintf((char *)state, " %02d%02d", gpgsvsta->Satellite_ID[i], gpgsvsta->SNR[i]);
                else sprintf((char *)state, "%02d%02d", gpgsvsta->Satellite_ID[i], gpgsvsta->SNR[i]);
                strcat((char *)str_buf, (char *)state);
                flag = KAL_TRUE;
            }
        }
    }
    memcpy(buf, str_buf, 60);
}

static void recv_cmd_softversion(char*head, char *parameter, Cmd_Type type)
{
    //char buf[128] = {0};
    kal_int8 ret;
    char versionchr[50];
    char *ver;
    ver = track_get_app_version();
    if(strlen(ver) <= 0)
    {
        ver = (char*)release_verno();
    }
    sprintf(versionchr, "%s", ver);
    LOGD(L_APP, L_V5, "parameter:%s,type:%d", parameter, type);
    ret = track_cust_paket_T5(buf, sizeof(buf), head, versionchr);
    track_cmd_send_rsp(type, ret, buf, 14);
}

static void recv_cmd_gsm(char*head, char *parameter, Cmd_Type type)
{
    //char buf[128] = {0};
    char num[10] = {0};
    kal_uint8 level = 0;
    kal_uint16 signal = 0;
    int tmp = 0;

    LOGD(L_APP, L_V5, " recv_cmd_gsm %s", parameter);

    //level = track_drv_get_service_cell_level();
    tmp = track_cust_get_rssi(-1);
    LOGD(L_APP, L_V5, "gsm_level %d,tmp %d", level, tmp);
    /*if(level == 0xFF || level <= 0)
    {
        signal = 0;
    }
    else if(level >= 58)  //������Ӧ����62
    {
        signal = 100;
    }
    else
    {
        signal = (kal_uint16)((level * 5) / 3);
    }

    if((signal > 20) && (signal < 60))
    {
        signal = signal + 10;
    }*/
    if(tmp < 0 || tmp >= 99)
    {
        tmp = 0;
    }
    else if(tmp >= 32)
    {
        tmp = 32;
    }

    //sprintf(num, "%d%%", signal); //98%
    sprintf(num, "%d", tmp); //98%

    track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, num), buf, 14);
}

static void recv_cmd_gps(char*head, char *parameter, Cmd_Type type)
{
    //char buf[300] = {0};
    char gpsmsg[60];
    LOGD(L_APP, L_V5, "recv_cmd_gps %s", parameter);
    check_status_gpsstatus(gpsmsg);
    track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, gpsmsg), buf, 14);
}

static void recv_cmd_vbat(char*head, char *parameter, Cmd_Type type)
{
    //char buf[128] = {0};
    char num[50] = {0};
    kal_uint32 volt;
    kal_uint32 ichr = 0, vchr = 0;
    LOGD(L_APP, L_V5, "%s", parameter);
    volt = track_cust_get_battery_volt();
    if(track_cust_status_charger() > 0)
    {
        charge_measure_struct charge_measure = {0};
        track_bmt_get_ADC_result(&charge_measure);
        ichr = charge_measure.bmt_iChr * 1000000.0;
        vchr = charge_measure.bmt_vChr * 1000000.0;
        sprintf(num, "%d:%d:%d", volt, vchr, ichr);
    }
    else
    {
        sprintf(num, "%d:%d:%d", volt, 0, 0);
    }
    track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, num), buf,  14);
}

static void recv_cmd_vin(char*head, char *parameter, Cmd_Type type) //������ӵ�Դ��ѹ����ʱδ�õ�
{
    //char buf[100] = {0};
    kal_uint16 volt = 0;
    char num[50] = {0};
    kal_int8 ret;
    volt = track_drv_get_external_voltage_change();
    sprintf(num, "%d", volt);
    track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, num), buf,  14);
    LOGD(L_APP, L_V5, "%s", parameter);
}

static void recv_cmd_login(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_int8 ret;
    LOGD(L_APP, L_V5, "%s", parameter);
    ret = track_cust_paket_T5(buf, sizeof(buf), head, (track_soc_login_status() ? "1" : "0"));
    track_cmd_send_rsp(type, ret, buf, 14);
}

static void recv_cmd_psw_get(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    LOGD(L_APP, L_V5, "%s", parameter);
    track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, "123456"), buf, 14);
}

static void recv_cmd_imsi_get(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    LOGD(L_APP, L_V5, "%s", parameter);
    track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, track_drv_get_imsi(0)), buf, 14);
}

static void recv_cmd_imei_get(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    LOGD(L_APP, L_V5, "%s", parameter);
    track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, track_drv_get_imei(0)), buf, 14);
}

static void recv_cmd_acc_get(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0}, 
        char tmp[5] = {0};
    LOGD(L_APP, L_V5, "%s", parameter);
    snprintf(tmp, 4, "%d", track_cust_status_acc());
    track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
}


static void recv_cmd_psw(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    char tmp[TRACK_PASSWD_LEN_MAX] = {0};
    kal_uint8 len, i;
    kal_bool flag = KAL_FALSE;

    if((parameter != NULL))
    {
        LOGD(L_APP, L_V5, "%s=%s", head, parameter);
    }

    if(parameter == NULL)
    {

    }
    else if(strlen(parameter) == 0)
    {
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, G_importance_parameter_data.login.password), buf, 14);
        return;
    }
    else if(strlen(parameter) != 6)
    {

    }
    else
    {
        flag = KAL_TRUE;
        for(i = 0; i < 6; i++)
        {
            if((*(parameter + i)) < '0' || (*(parameter + i)) > '9')
            {
                flag = KAL_FALSE;
            }
        }

        if(flag)
        {
            memset(G_importance_parameter_data.login.password, 0, TRACK_PASSWD_LEN_MAX);
            strcpy((char*)G_importance_parameter_data.login.password, parameter);
            flag = Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        }
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}

static void recv_cmd_domain(char*head, char *parameter, Cmd_Type type)
{
    //www.cheweishi.cn:8500
    //120.195.105.20:8500
    char *port;//buf[128] = {0};
    kal_uint8 IP[4], dnsLen = 0;
    kal_bool flag = KAL_FALSE;
    kal_uint16 portInt = 0;
    nvram_server_addr_struct server = {0};
    char domain[TRACK_MAX_URL_SIZE] = {0};
    LOGD(L_APP, L_V5, "%s", parameter);
    do
    {
        if(parameter == NULL) break;
        if(strlen(parameter) == 0)
        {
            char tmp[100] = {0};
            if(track_nvram_alone_parameter_read()->server.conecttype == 1)
            {
                snprintf(tmp, 99, "%s:%d", track_domain_decode(track_nvram_alone_parameter_read()->server.url), track_nvram_alone_parameter_read()->server.server_port);
            }
            else if(track_nvram_alone_parameter_read()->server.conecttype == 0)
            {
                snprintf(tmp, 99, "%d.%d.%d.%d:%d",
                         track_nvram_alone_parameter_read()->server.server_ip[0],
                         track_nvram_alone_parameter_read()->server.server_ip[1],
                         track_nvram_alone_parameter_read()->server.server_ip[2],
                         track_nvram_alone_parameter_read()->server.server_ip[3],
                         track_nvram_alone_parameter_read()->server.server_port);
            }
            else
            {
                break;
            }
            track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
            return;
        }
        if((port = strchr(parameter, ':')) == NULL) break;
        *port++ = '\0';
        LOGD(L_APP, L_V5, "Server:%s:%s", parameter, port);
        if((dnsLen = strlen(parameter)) > 99/*sizeof(gps_server_param.url)*/)break;

        if(track_fun_asciiIP2Hex(parameter, IP) == KAL_FALSE) memset(IP, 0, 4);

        if(track_fun_check_data_is_port(port) == KAL_FALSE || (portInt = parseInt(port)) <= 0) break;
        flag = KAL_TRUE;
    }
    while(0);
    if(flag)
    {
        if(IP[0] == 0 && IP[3] == 0)
        {
            server.conecttype = 1;
            memset((void *)server.url, 0, sizeof(track_nvram_alone_parameter_read()->server.url));//��ǰ������Ϊurlʱ���͵���Ϣ����2���˿ں��������
            //memcpy((void *)&track_nvram_alone_parameter_read()->server.url, (void *)parameter, dnsLen);
            memcpy(domain, (void *)parameter, dnsLen);
            track_domain_encode(domain, track_nvram_alone_parameter_read()->server.url);
        }
        else
        {
            server.conecttype = 0;
            memcpy(server.server_ip, IP, 4);
        }
        server.soc_type = 0;
        server.server_port = portInt;
        flag = nvram_alone_parameter_cmd.server_set(&server);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
    track_start_timer(GPRS_SOCKET_INIT_TIMER_ID, 1000, track_soc_gprs_reconnect, (void*)4);
    //track_os_intoTaskMsgQueue(track_cust_write_sim_backup_for_parameter);
}

static void recv_cmd_freq(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    int timef = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if(parameter == NULL)
    {
    }
    else if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        //snprintf(tmp, 20, "%d", G_parameter.gps_work.Fti.interval_acc_on);
        snprintf(tmp, 20, "%d", G_parameter.yd_gps_interval);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    else if((timef = parseInt(parameter)) >= 1 && timef < 65535)
    {
        //G_parameter.gps_work.Fti.interval_acc_on = timef;
        //G_parameter.gps_work.Fti.interval_acc_off = timef;
        G_parameter.yd_gps_interval = timef;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }

    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}

static void recv_cmd_trace(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    int trace = 0;

    LOGD(L_APP, L_V5, "%s,%d", parameter, (kal_uint8)*parameter - '0');
    if((parameter == NULL) || (strlen(parameter) > 1))
    {
        track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, KAL_FALSE), buf, 2);
        return;
    }
    if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.t_trace);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    trace = (kal_uint8) * parameter - '0';
    if(trace == 0 || trace == 1)
    {
        G_parameter.t_trace = trace;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}

static void recv_cmd_pulse(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    kal_uint16 pulse = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if(parameter == NULL)
    {
    }
    else if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.hbt_acc_on);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    else if((pulse = parseInt(parameter)) >= 30 && pulse < 65535)
    {
        G_parameter.hbt_acc_on = pulse;
        G_parameter.hbt_acc_off = pulse;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
    track_os_intoTaskMsgQueue(heartbeat_packets_cq);
}

static void recv_cmd_phone(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    char tmp[TRACK_DEFINE_PHONE_NUMBER_LEN_MAX] = {0};
    kal_uint8 len, i;
    kal_bool flag = KAL_FALSE;

    if((parameter != NULL))
    {
        LOGD(L_APP, L_V5, "%s=%s", head, parameter);
    }

    if(parameter == NULL)
    {

    }
    else if(strlen(parameter) == 0)
    {
        snprintf(tmp, 20, "%s", G_importance_parameter_data.login.owner_num);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    else
    {
        len = strlen(parameter);
        if((len > 5) && (len < 21))
        {
            flag = KAL_TRUE;
            for(i = 0; i < len; i++)
            {
                if((*(parameter + i)) < '0' || (*(parameter + i)) > '9')
                {
                    flag = KAL_FALSE;
                }
            }

            if(flag)
            {
                memset(G_importance_parameter_data.login.owner_num, 0, sizeof(G_importance_parameter_data.login.owner_num));
                strcpy(G_importance_parameter_data.login.owner_num, parameter);
                set_nvram_device_setting();
            }
        }
    }

    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 14);
}

static void recv_cmd_user(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    char tmp[TRACK_DEFINE_PHONE_NUMBER_LEN_MAX] = {0};
    kal_uint8 len, i;
    kal_bool flag = KAL_FALSE;

    if((parameter != NULL))
    {
        LOGD(L_APP, L_V5, "%s=%s", head, parameter);
    }

    if(parameter == NULL)
    {

    }
    else if(strlen(parameter) == 0)
    {
        snprintf(tmp, 20, "%s", G_importance_parameter_data.login.host_num);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    else
    {
        len = strlen(parameter);
        if((len > 5) && (len < 21))
        {
            flag = KAL_TRUE;
            for(i = 0; i < len; i++)
            {
                if((*(parameter + i)) < '0' || (*(parameter + i)) > '9')
                {
                    flag = KAL_FALSE;
                }
            }

            if(flag)
            {
                memset(G_importance_parameter_data.login.host_num, 0x00, sizeof(G_importance_parameter_data.login.host_num));
                strcpy(G_importance_parameter_data.login.host_num, parameter);
                set_nvram_device_setting();
            }
        }
    }

    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 14);
}


static void recv_cmd_radius(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    int radius = 0;

    LOGD(L_APP, L_V5, "%s", parameter);

    if(parameter == NULL)
    {
    }
    else if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.displacement_alarm.radius);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    else if((radius = parseInt(parameter)) >= 300 && radius < 65535)
    {
        G_parameter.displacement_alarm.radius = radius;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);

}

/*�𶯱������ſ���*/
static void recv_cmd_vib(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    kal_uint8 alarm = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if((parameter == NULL) || (strlen(parameter) > 1))
    {
        track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, KAL_FALSE), buf, 2);
        return;
    }
    if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.vib);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    alarm = (kal_uint8) * parameter - '0';
    if(alarm == 0 || alarm == 1)
    {
        G_parameter.vib = alarm;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);

}

/*�𶯱�����ƽ̨����*/
static void recv_cmd_vibs(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    kal_uint8 vibs = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if((parameter == NULL) || (strlen(parameter) > 1))
    {
        track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, KAL_FALSE), buf, 2);
        return;
    }
    if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.vibs);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    vibs = (kal_uint8) * parameter - '0';
    if(vibs == 0 || vibs == 1)
    {
        G_parameter.vibs = vibs;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}

/*�𶯱�������������*/
static void recv_cmd_vibl(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    int vibl = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if(parameter == NULL)
    {
    }
    else if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.vibrates_alarm.sensitivity_level);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    else if(((vibl = parseInt(parameter)) >= 0) && (vibl <= 15))
    {
        G_parameter.vibrates_alarm.sensitivity_level = vibl;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);

        track_cust_sensor_init();// sensor��ʼ��
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);

}

/*�ϵ�͵繲��һ���ȣ�Ҫ����ͬʱ������*/
static void recv_cmd_pof(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    kal_uint8 pof = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if((parameter == NULL) || (strlen(parameter) > 1))
    {
        track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, KAL_FALSE), buf, 2);
        return;
    }
    if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.pof);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }

    pof = (kal_uint8) * parameter - '0';
    if(pof == 0 || pof == 1)
    {
        G_parameter.pof = pof;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);

}


static void recv_cmd_pofs(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    kal_uint8 pofs = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if((parameter == NULL) || (strlen(parameter) > 1))
    {
        track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, KAL_FALSE), buf, 2);
        return;
    }
    if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.pofs);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    pofs = (kal_uint8) * parameter - '0';
    if(pofs == 0 || pofs == 1)
    {
        G_parameter.pofs = pofs;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}

static void recv_cmd_sleep(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    kal_uint8 sleep = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if((parameter == NULL) || (strlen(parameter) > 1))
    {
        track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, KAL_FALSE), buf, 2);
        return;
    }
    if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.sleep_wakeup.t_sleep);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    sleep = (kal_uint8) * parameter - '0';
    if(sleep == 0 || sleep == 1)
    {
        G_parameter.sleep_wakeup.t_sleep = sleep;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }

    if(G_parameter.sleep_wakeup.t_sleep == 0)
    {
        tr_stop_timer(TERMINAL_SLEEP_TO_WAKEUP_TIMER_ID);
        track_cust_set_work_mode((void *)NORNAL_MODE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);

}

static void recv_cmd_vibgps(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    kal_uint8 vibgps = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if(parameter == NULL)
    {
    }
    else if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.gps_work.static_filter_sw);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    else if((vibgps = (kal_uint8) * parameter - '0') == 0 || vibgps == 1)
    {
        G_parameter.gps_work.static_filter_sw = vibgps;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}

/*
static void recv_cmd_chk(char*head, char *parameter, Cmd_Type type)
{
    LOGD(L_APP, L_V5, "%s", parameter);
}*/

static void recv_cmd_speed(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    int speed = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if(parameter == NULL)
    {
    }
    else if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.speed_limit_alarm.threshold);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    else if((speed = parseInt(parameter)) > 0 && speed < 255)
    {
        G_parameter.speed_limit_alarm.threshold = speed;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}

static void recv_cmd_calllock(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    kal_uint8 calllock = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if((parameter == NULL) || strlen(parameter) > 1)
    {
        track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, KAL_FALSE), buf, 2);
        return;
    }
    if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.t_calllock);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    calllock = (kal_uint8) * parameter - '0';
    if(calllock == 0 || calllock == 1)
    {
        G_parameter.t_calllock = calllock;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}

static void recv_cmd_calldisp(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    kal_uint8 calldsp = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if((parameter == NULL) || strlen(parameter) > 1)
    {
        track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, KAL_FALSE), buf, 2);
        return;
    }
    if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.t_calldsp);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, 100, head, tmp), buf, 14);
        return;
    }
    calldsp = (kal_uint8) * parameter - '0';
    if(calldsp == 0 || calldsp == 1)
    {
        G_parameter.t_calldsp = calldsp;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);

}

static void recv_cmd_vibcall(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    kal_uint8 vibcall = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if((parameter == NULL) || (strlen(parameter) > 1))
    {
        track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, KAL_FALSE), buf, 2);
        return;
    }
    if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.t_vibcall);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, 100, head, tmp), buf, 14);
        return;
    }
    vibcall = (kal_uint8) * parameter - '0';
    if(vibcall == 0 || vibcall == 1)
    {
        G_parameter.t_vibcall = vibcall;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}

static void recv_cmd_sms(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    kal_uint8 tsms = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if((parameter == NULL) || (strlen(parameter) > 1))
    {
        track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, KAL_FALSE), buf, 2);
        return;
    }
    if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.t_sms);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, 100, head, tmp), buf, 14);
        return;
    }
    tsms = (kal_uint8) * parameter - '0';
    if(tsms == 0 || tsms == 1)
    {
        G_parameter.t_sms = tsms;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);

}


static void recv_cmd_vibchk(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    int vati = 0, count = 0;
    char *p;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    do
    {
        if(parameter == NULL)break;
        if(strlen(parameter) == 0)
        {
            char tmp[20] = {0};
            snprintf(tmp, 20, "%d:%d", (G_parameter.vibrates_alarm.detection_time), G_parameter.vibrates_alarm.detection_count);
            track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
            return;
        }
        if((p = strchr(parameter, ':')) == NULL)break;

        *p = '\0';
        p++;
        LOGD(L_CMD, L_V6, "p1:%s,p2:%s", parameter, p);
        if(strlen(parameter) == 0 || strlen(p) == 0)break;

        vati = parseInt(parameter);
        count = parseInt(p);

        if(((vati > 3) && (vati < 31)) && ((count > 1) && (count < 31)))
        {
            if(vati >= count + 2) //������ÿ�����Ӧ�ò��ϱ�һ�Σ�����Ҫ����ʱ����ڴ�����������Զ���ᴥ����Ч��
            {
                G_parameter.vibrates_alarm.detection_time = vati;
                G_parameter.vibrates_alarm.detection_count = count;
                flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
            }
        }
    }
    while(0);

    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}

static void recv_cmd_poft(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    int alct = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if(parameter == NULL)
    {
    }
    else if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.acc_low_chargeout_time);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    else if((alct = parseInt(parameter)) >= 0 && alct <= 3600)
    {
        G_parameter.acc_low_chargeout_time = alct;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        /*if(track_is_timer_run(TRACK_ACCL_CHARGEOUT_CHECK_TIMER_ID))
        {
            tr_start_timer(TRACK_ACCL_CHARGEOUT_CHECK_TIMER_ID, time_interval.acc_low_chargeout_time * 1000, termimal_accl_charge_status);
        }*/
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}

static void recv_cmd_wakeup(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    kal_uint8 wakeup = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if((parameter == NULL) || (strlen(parameter) > 1))
    {
        track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, KAL_FALSE), buf, 2);
        return;
    }
    if(strlen(parameter) == 0)
    {
        if(G_parameter.sleep_wakeup.t_wakeup)
        {
            track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, "1"), buf, 14);
        }
        else
        {
            track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, "0"), buf, 14);
        }
        return;
    }
    if(strlen(parameter) == 1 && (*parameter == '0' || *parameter == '1'))
    {
        if((*parameter - 0x30) == 0)
        {
            if(track_is_timer_run(TERMINAL_SLEEP_TO_WAKEUP_TIMER_ID))
            {
                tr_stop_timer(TERMINAL_SLEEP_TO_WAKEUP_TIMER_ID);
            }
        }
        G_parameter.sleep_wakeup.t_wakeup = *parameter - 0x30;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);

    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}

static void recv_cmd_wakeupt(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    int stint = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if(parameter == NULL)
    {
    }
    else if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 19, "%d", G_parameter.sleep_wakeup.GSM_Sleep_Starting_Time_interval);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
    }
    else if((stint = parseInt(parameter)) >= 1 && stint <= 3000)
    {
        G_parameter.sleep_wakeup.GSM_Sleep_Starting_Time_interval = stint;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
        if(track_is_timer_run(TERMINAL_SLEEP_TO_WAKEUP_TIMER_ID) && G_parameter.sleep_wakeup.t_wakeup == 1)
        {
            track_start_timer(TERMINAL_SLEEP_TO_WAKEUP_TIMER_ID, G_parameter.sleep_wakeup.GSM_Sleep_Starting_Time_interval * 60000, track_cust_work_mode_ind, (void *)w_timer_in_nornal_msg);
        }
    }
}

static void recv_cmd_sleept(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    int chargerout = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if(parameter == NULL)
    {
    }
    else if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.sleep_wakeup.chargerout_wait);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    else if((chargerout = parseInt(parameter)) >= 1  && chargerout <= 3000)
    {
        G_parameter.sleep_wakeup.chargerout_wait = chargerout;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        /*if(track_is_timer_run(TERMINAL_GOTO_SLEEP_TIMER_ID))
        {
            tr_start_timer(TERMINAL_GOTO_SLEEP_TIMER_ID, time_interval.chargerout_wait * 60000, track_custom_goto_sleep_mode);
        }*/
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);

}

static void recv_cmd_acclt(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    int stime = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if(parameter == NULL)
    {
    }
    else if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.acc_starting_time);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    else if((stime = parseInt(parameter)) >= 1 && stime <= 3600)
    {
        G_parameter.acc_starting_time = stime;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
        /*if(track_is_timer_run(TRACK_DEFENCE_RELAY_TIMER_ID))
        {
            tr_start_timer(TRACK_DEFENCE_RELAY_TIMER_ID, time_interval.acc_starting_time * 1000, track_acc_low_down);
        }*/
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);

}

static void recv_cmd_acclock(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    kal_uint8 acclock = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);
    if((parameter == NULL) || (strlen(parameter) > 1))
    {
        track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, KAL_FALSE), buf, 2);
        return;
    }
    if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.t_acclock);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    acclock = (kal_uint8) * parameter - '0';
    if(acclock == 0 || acclock == 1)
    {
        G_parameter.t_acclock = acclock;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    if(G_parameter.t_acclock == 0)
    {
        tr_stop_timer(TRACK_DEFENCE_RELAY_TIMER_ID);
        track_stop_timer(TRACK_CUST_STATUS_DEFENCES_TIMER_ID);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}

static void recv_cmd_lbv(char *head, char*parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    int stime = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if(parameter == NULL)
    {
    }
    else if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.low_power_alarm.threshold);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    else if((stime = parseInt(parameter)) >= 3500000 && stime <= 4200000)
    {
        /*if((G_parameter.low_power_alarm.threshold != stime) && is_sended_lowbat_sms)
        {
            is_sended_lowbat_sms = FALSE;
        }*/
        G_parameter.low_power_alarm.threshold = stime;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    LOGD(L_APP, L_V5, "%d", G_parameter.low_power_alarm.threshold);
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}

static void recv_cmd_out(char *head, char*parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    int t_out = 0;

    LOGD(L_APP, L_V5, "%s,%d", parameter, (kal_uint8)*parameter - '0');
    if((parameter == NULL) || (strlen(parameter) > 1))
    {
        track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, KAL_FALSE), buf, 2);
        return;
    }
    if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.out);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    t_out = (kal_uint8) * parameter - '0';
    if(t_out == 0 || t_out == 1)
    {
        G_parameter.out = t_out;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}

static void recv_cmd_outs(char *head, char*parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    int t_outs = 0;

    LOGD(L_APP, L_V5, "%s,%d", parameter, (kal_uint8)*parameter - '0');
    if((parameter == NULL) || (strlen(parameter) > 1))
    {
        track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, KAL_FALSE), buf, 2);
        return;
    }
    if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.outs);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    t_outs = (kal_uint8) * parameter - '0';
    if(t_outs == 0 || t_outs == 1)
    {
        G_parameter.outs = t_outs;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}

static kal_uint32 recv_s19_hex_to_dec(char *hex)
{
    kal_uint32 dec = 0;
    kal_uint8 len, i;
    char *p;

    if(hex == NULL)
    {
        return 0;
    }

    p = hex;
    len = strlen(p);
    if((len <= 0) || (len > 8))
    {
        return -1;
    }

    for(i = 0; i < len; i++)
    {
        if(p[i] >= '0' && p[i] <= '9')
        {
            dec = (dec << 4) + (p[i] - '0');
        }
        else if(p[i] >= 'A' && p[i] <= 'F')
        {
            dec = (dec << 4) + (p[i] - 'A' + 10);
        }
        else if(p[i] >= 'a' && p[i] <= 'f')
        {
            dec = (dec << 4) + (p[i] - 'a' + 10);
        }
        else
        {
            return -2;
        }
    }

    return dec;
}

static kal_bool recv_s19_check(cmd_online_struct *cmds)
{
    kal_uint8 in_out, square;
    kal_uint8 tmp;
    kal_uint8 part_max;

    in_out = check_str_is_bool_type(cmds->cmd[2]);
    if(in_out < 0)
    {
        return KAL_FALSE;
    }
    if(strlen(cmds->cmd[3]) != 1)
    {
        return KAL_FALSE;
    }

    square = (char) * (cmds->cmd[3]) - '0';
    if((square < 1) || (square > 3))
    {
        return KAL_FALSE;
    }
    if(square == 1)
    {
        if(cmds->count != 7)
        {
            return KAL_FALSE;
        }
    }
    else if(square == 2)
    {
        if(cmds->count != 8)
        {
            return KAL_FALSE;
        }
    }
    else if(square == 3)
    {
        part_max = (TRACK_POLYGON_MAX << 1) + 4;
        if(cmds->count > part_max)
        {
            return KAL_FALSE;
        }
        if((cmds->count % 2) != 0)
        {
            return KAL_FALSE;
        }
    }
    return KAL_TRUE;
}

static void recv_cmd_pwralm_filter(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    kal_uint16 pulse = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if(parameter == NULL)
    {
    }
    else if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.pwralm_filter);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    else if((pulse = parseInt(parameter)) >= 1 && pulse <= 60)
    {
        G_parameter.pwralm_filter = pulse;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}
static void recv_cmd_shkalm_filter(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    kal_uint16 pulse = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if(parameter == NULL)
    {
    }
    else if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.vibrates_alarm.alarm_interval/60);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    else if((pulse = parseInt(parameter)) >= 1 && pulse <= 60)
    {
        G_parameter.vibrates_alarm.alarm_interval = pulse * 60;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}
static void recv_cmd_pwralm_input_filter(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    kal_uint16 pulse = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if(parameter == NULL)
    {
    }
    else if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.power_fails_alarm.delay_time_off);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    else if((pulse = parseInt(parameter)) >= 1 && pulse <= 60)
    {
        G_parameter.power_fails_alarm.delay_time_off = pulse;
        //G_parameter.power_fails_alarm.delay_time_on = pulse;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}
static void recv_cmd_shkalm_delay(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    kal_uint16 pulse = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if(parameter == NULL)
    {
    }
    else if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%d", G_parameter.vibrates_alarm.alarm_delay_time);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    else if((pulse = parseInt(parameter)) >= 1 && pulse < 65535)
    {
        G_parameter.vibrates_alarm.alarm_delay_time = pulse;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}
static void recv_cmd_XLH(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    kal_uint16 temp = 0;

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if(parameter == NULL)
    {
    }
    else if(strlen(parameter) == 0)
    {
        char tmp[20] = {0};
        snprintf(tmp, 20, "%s", G_importance_parameter_data.login.SequenceNumber);
        track_cmd_send_rsp(type, track_cust_paket_T5(buf, sizeof(buf), head, tmp), buf, 14);
        return;
    }
    else
    {
        temp = strlen(parameter);
        if(temp > 0 && temp < 19)
        {
            memcpy(G_importance_parameter_data.login.SequenceNumber, parameter, 18);
            flag = Track_nvram_write(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, (void *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        }
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
    track_os_intoTaskMsgQueueExt(track_soc_gprs_reconnect, (void*)4);//���½�������
}
static void recv_cmd_NVRESET(char*head, char *parameter, Cmd_Type type)
{
    //char buf[100] = {0};
    kal_bool flag = KAL_FALSE;
    char tmp[20] = {0};

    LOGD(L_APP, L_V5, "%s=%s", head, parameter);

    if(parameter == NULL)
    {
    }
    else if(strlen(parameter) != 0 && !strcmp(G_importance_parameter_data.login.password,parameter))
    {
        track_cmd_xcws_factory();
        flag = 1;
    }
    track_cmd_send_rsp(type, track_cust_paket_T6(buf, sizeof(buf), head, flag), buf, 2);
}
static cust_recv_cmd_struct cust_recv_cmd_table[] =
{

    /* ���ɲ�ѯ */
    {"SOFTVERSION",     recv_cmd_softversion    },      //��ѯ����汾��
    {"GSM",             recv_cmd_gsm            },      //��ѯ GSM �ź�ǿ�� 0~32

    {"GPS",             recv_cmd_gps            },      //��ѯ GPS ���յ����Ǳ�ź�ǿ��
    {"VBAT",            recv_cmd_vbat           },      //��ѯ��ص�ѹ
    {"VIN",             recv_cmd_vin            },       //��ѯ��ӵ�Դ�����ѹ
    {"LOGIN",           recv_cmd_login          },      //��ѯ�Ƿ��Ѿ����ӵ���̨
    {"IMSI",            recv_cmd_imsi_get       },      //��ȡ�ն�sim��IMSI��
    {"IMEI",            recv_cmd_imei_get       },      //��ȡ�ն�IMEI��
    {"ACC",             recv_cmd_acc_get        },      //��ȡ�ն�ACC״̬(ACC�߷���1���ͷ���0)

    /* �ɲ�ѯ������ */
    {"PSW",             recv_cmd_psw            },      //����
    {"DOMAIN",          recv_cmd_domain         },      //���÷�������ַ�Ͷ˿�
    {"FREQ",            recv_cmd_freq           },      //�ϱ� GPS λ��Ƶ���趨
    {"TRACE",           recv_cmd_trace          },      //�������ر�׷��
    {"PULSE",           recv_cmd_pulse          },      //������Ƶ��
    {"PHONE",           recv_cmd_phone          },      //���÷�������
    {"USER",            recv_cmd_user           },      //���ó�������
    {"RADIUS",          recv_cmd_radius         },      //���� GPS Χ���澯����
    {"VIB",             recv_cmd_vib            },      //�����񶯸澯����
    {"VIBL",            recv_cmd_vibl           },      //������������
    {"POF",             recv_cmd_pof            },      //�ϵ�͵͵�澯����
    {"SLEEP",           recv_cmd_sleep          },      //�������ر�����
    {"VIBGPS",          recv_cmd_vibgps         },      //�������ر� GPS ���˹���
//    {"CHK",             recv_cmd_chk          },    //�Ƿ������ӵ�Դ��ѹ
    {"SPEED",           recv_cmd_speed          },      //���ٱ�������
    {"CALLLOCK",        recv_cmd_calllock       },      //���з������ڿ���������������������Ƕ�λ
    //{"CALLDISP",        recv_cmd_calldisp     },    //������湦�����ĸ����������Ƿ���Ҫ��ͨ����
    //{"VIBCALL",         recv_cmd_vibcall      },    //�񶯺��й���
    //{"SMS",             recv_cmd_sms          },    //�Ƿ���Ҫͨ�����ż���
    {"VIBCHK",          recv_cmd_vibchk         },      //������X ��ʱ���ڲ�����Y ���𶯣��Ų����𶯸澯
    {"POFT",            recv_cmd_poft           },      //����acc off ��ȴ���ÿ����ϵ�澯����
    {"WAKEUP",          recv_cmd_wakeup         },      //�����Ƿ��ͨ��WAKEUPT ��ʱ�����ն�
    {"WAKEUPT",         recv_cmd_wakeupt        },      //�������ߺ����Զ�����
    {"SLEEPT",          recv_cmd_sleept         },      //�����ϵ��೤ʱ���������
    {"ACCLT",           recv_cmd_acclt          },      //�������ߺ����Զ�����
    {"ACCLOCK",         recv_cmd_acclock        },      //�Ƿ�������ACC ״̬�Զ��������
    {"LBV",             recv_cmd_lbv            },      //�͵籨����ֵ
    {"VIBSMS",             recv_cmd_vibs          },    //�Ƿ��ƶ��澯��ƽ̨����VIBS
    {"POFSMS",             recv_cmd_pofs          },    //�ϵ�͵͵�澯�� ƽ̨����POFS
    {"OUT",             recv_cmd_out          },    //λ�Ʊ������ſ���
    {"OUTSMS",             recv_cmd_outs          },    //λ�Ʊ������翪�� OUTS
    {"PWRALARMFILTER",             recv_cmd_pwralm_filter          },//�ϵ籨����Ĭʱ��M
    {"SHKALARMFILTER",             recv_cmd_shkalm_filter          },//�𶯱�����Ĭʱ��M
    {"PWRINPUTFILTER",             recv_cmd_pwralm_input_filter          },//�ָ��ϵ�ʱ��s
    {"SHKALARMMDLY",             recv_cmd_shkalm_delay          },//�𶯱�����ʱs
    {"XLH",             recv_cmd_XLH          },//���к�
    {"NVRESET",             recv_cmd_NVRESET          },    //�ظ���������    
    {" ",               NULL}
};

/* ���沿��Ϊ����ָ��� */

/* �����Ϊ�������ݽ������ */


static void recv_data_s1(cmd_online_struct *cmds)
{
    //[2008-12-16 10:00:00,S1,1]
    LOGD(L_APP, L_V5, "size=%d, %s", cmds->count, cmds->cmd[2]);
    if(!strcmp(cmds->cmd[2], "1"))
    {
        nvram_gps_time_zone_struct tmp = {0};
        memcpy(&tmp, &G_parameter.zone, sizeof(nvram_gps_time_zone_struct));
        tmp.zone = 'W'; //��Ϊ���������ص�ʱ���Ƕ�8����ʱ�䣬����ϵͳ��0ʱ����ʱ�䣬�����������óɷ�����
        track_fun_SystemTime_Sync_FromStrTime(&tmp, cmds->cmd[0]);

        tr_stop_timer(TRACK_CUST_NET_LOGIN_TIMEOUT_TIMER_ID);
        tr_stop_timer(TRACK_CUST_NET_LOGIN_RESEND_TIMER_ID);
        tr_stop_timer(TRACK_CUST_NET_CONN_BAKDATA_TIMER_ID);
        LED_GSM_StatusSet(GPRS_CONNECT_STATE);
        track_soc_conn_status(KAL_TRUE);
        if(!track_soc_login_status())
        {
            track_set_login_status(KAL_TRUE);
            //reconnect_count = 0;
            track_cust_gpsmode_net_new_conn();
            track_soc_send((void*)99);
            track_soc_bakup_data((void*)2);
        }
        track_os_intoTaskMsgQueueExt(heartbeat_packets, (void *)111);
        track_os_intoTaskMsgQueueExt(track_soc_send, (void*)9);
        /*if(G_parameter.agps.server_mode != 1)
        {
            track_os_intoTaskMsgQueue(track_cust_agps_start);
        }     */
    }
    else
    {
        //����֪ͨ
        //track_send_sms_from_socrev(atoi(cmds->cmd[2]));
    }

}
/*�������ظ�*/
static void recv_data_s2(cmd_online_struct *cmds)
{

    LOGD(L_APP, L_V5, "recv_data_s2");
    track_cust_gprs_conn_fail((void*)1);
    track_check_tick(1, NULL);
    tr_stop_timer(TRACK_CUST_NET_HEARTBEAT_OVER_TIMER_ID);
}

static void recv_data_s3(cmd_online_struct *cmds)
{
    LOGD(L_APP, L_V5, "");
    // [,S3]   ����ʵʱ�켣�ϱ���һ��Ҫ�յ� S3 Ӧ���ű�ʾ�ϱ��ɹ�
}

extern void track_cust_work_mode_ind(work_msg_ind type);
static void recv_data_s4(cmd_online_struct *cmds)
{
    // [,S4]   ����ƽ̨�����·�ָ����նˣ�Ҫ���ն��������ص�ǰ�ľ�γ�ȡ�
    track_gps_data_struct *gpsdata;
    LOGD(L_APP, L_V5, "recv_data_s4");
    track_cust_work_mode_ind(w_dw_msg);
    gpsdata = track_cust_backup_gps_data(0, NULL);
    if(gpsdata != NULL)
    {
        track_cust_packet_params_struct t4 = {0};
        char bufstr[256] = {0};
        int len;
        t4.gd = gpsdata;
        t4.status = track_get_terminal_status();//�ն�״̬λ
        t4.GSMSignalStrength = track_get_gsm_signal_level();
        t4.lbs = track_drv_get_lbs_data();
        //t3.battery = track_battary_volt_get();
        //LOGD(L_APP, L_V5, "%d,%f", gpsdata->gpgga.sat, track_battary_volt_get());
        len = track_cust_paket_T4(bufstr, 256, &t4);
        track_cmd_send_rsp(cmds->type, len, bufstr, 10);
    }
}
static void recv_data_s5(cmd_online_struct *cmds)
{
    // [,S14,PSW]   ƽ̨�·�ָ���ѯ�ն˲���
    int i, size;
    kal_bool find = KAL_FALSE;
    kal_bool isChar = KAL_TRUE;
    LOGD(L_APP, L_V5, "cmd0:%s,cmd1:%s,cmd2:%s", cmds->cmd[0], cmds->cmd[1], cmds->cmd[2]);
    size = sizeof(cust_recv_cmd_table) / sizeof(cust_recv_cmd_struct);
    for(i = 0; i < size; i++)
    {
        if(!strcmp(cmds->cmd[2], cust_recv_cmd_table[i].cmd_string))
        {
            cust_recv_cmd_table[i].func_ptr(cust_recv_cmd_table[i].cmd_string, blank, cmds->type);
            find = KAL_TRUE;
            break;
        }
    }
    if(!find) 
    {
        LOGD(L_APP, L_V5, "�г���ʿ##invalid command!");
        return;
    }
    if(sms_cmd->return_sms.cm_type == CM_SMS)
    {
         track_cust_sms_send(sms_cmd->return_sms.num_type, sms_cmd->return_sms.ph_num, isChar, buf, strlen(buf));
    }
}

/*���ò���*/
static void recv_data_s6(cmd_online_struct *cmds)
{
    //[,S2,DOMAIN=www.cheweishi.cn:8500]
    char tmp[21] = {0};
    char *p;
    int len, i, size;
    kal_bool find = KAL_FALSE;
    kal_bool isChar = KAL_TRUE;
    LOGD(L_APP, L_V5, "cmd0:%s,cmd1:%s,cmd2:%s", cmds->cmd[0], cmds->cmd[1], cmds->cmd[2]);

    if((p = strchr(cmds->cmd[2], '=')) == NULL)
    {
        LOGD(L_APP, L_V1, "ERROR 1");
        return;
    }

    if(p - cmds->cmd[2] >= 50)
    {
        LOGD(L_APP, L_V1, "ERROR 2");
        return;
    }
    if(strlen(p) <= 1)
    {
        LOGD(L_APP, L_V1, "ERROR 3");
        return;
    }

    strncpy(tmp, cmds->cmd[2], p - cmds->cmd[2]);
    p++;

    LOGD(L_APP, L_V6, "head=%s,string=%s", tmp, p);
    size = sizeof(cust_recv_cmd_table) / sizeof(cust_recv_cmd_struct);

    for(i = 0; i < size; i++)
    {
        if(!strcmp(tmp, cust_recv_cmd_table[i].cmd_string))
        {
            LOGD(L_APP, L_V6, "head=%s,string=%s", tmp, cust_recv_cmd_table[i].cmd_string);
            cust_recv_cmd_table[i].func_ptr(cust_recv_cmd_table[i].cmd_string, p, cmds->type);
            find = KAL_TRUE;
            break;
        }
    }
    if(!find)  
    {
        LOGD(L_APP, L_V5, "�г���ʿ##invalid command!");
        return;
    }

    if(sms_cmd->return_sms.cm_type == CM_SMS)
    {
         track_cust_sms_send(sms_cmd->return_sms.num_type, sms_cmd->return_sms.ph_num, isChar, buf, strlen(buf));
    }
}
extern terminal_defense_mode track_cust_get_defense_mode(void);
extern void track_cust_set_defense_mode_cq(void *arg);

void System_Reset_online(void)
{
    extern void track_cust_restart(kal_uint8 mode, kal_uint32 sec);
    LOGD(L_APP, L_V5, "====System_Reset==== restart by 2 sencond later");
    track_cust_restart(28, 0);
}

static void recv_data_s7(cmd_online_struct *cmds)
{
    LOGD(L_APP, L_V5, "");
    // [,S11]   Զ�������ն�
    System_Reset_online();
}

static void recv_data_s8(cmd_online_struct *cmds)
{
    char bufstr[256] = {0};
    int len;
    LOGD(L_APP, L_V5, "");

    if(track_is_timer_run(TRACK_DEFENCE_RELAY_TIMER_ID))
    {
        tr_stop_timer(TRACK_DEFENCE_RELAY_TIMER_ID);
    }
    track_cust_set_defense_mode_cq((void *)3);
    len = track_cust_paket_T8(bufstr, 256, 1);
    track_cmd_send_rsp(cmds->type, len, bufstr, 12);
}

static void recv_data_s9(cmd_online_struct *cmds)
{
    char bufstr[256] = {0};
    int len;
    LOGD(L_APP, L_V5, "%d", track_cust_get_defense_mode());
    // [,S13]   ƽ̨�·�Զ�̳���ָ��
    if(track_is_timer_run(TRACK_DEFENCE_RELAY_TIMER_ID))
    {
        tr_stop_timer(TRACK_DEFENCE_RELAY_TIMER_ID);
    }
    //if(track_cust_set_defense_mode_cq((void*)99) != 1)
    {
        track_cust_set_defense_mode_cq((void*)1);
    }
    len = track_cust_paket_T9(bufstr, 256, 1);
    track_cmd_send_rsp(cmds->type, len, bufstr, 13);
}

static void recv_data_s10(void)
{
}

static void recv_data_s11(void)
{
}

#if 0
static void recv_data_s19(cmd_online_struct *cmds)
{
    kal_uint8 in_out, square;
    char buf[256] = {0};
    kal_int8 len;
    kal_bool flag = KAL_FALSE;
    kal_uint32 radius = 0;
    kal_uint8 part, i, index;

    flag = recv_s19_check(cmds);
    LOGD(L_APP, L_V5, "%d", flag);

    if(flag)
    {
        //Բ��Χ���뾶�������100��----------
        if(((atoi(cmds->cmd[3]) - 1) == 0) && (((kal_uint32)recv_s19_hex_to_dec(cmds->cmd[6])) <= 100))
        {
            len = track_cust_paket_T19(buf, sizeof(buf), KAL_FALSE);
            track_cmd_send_rsp(cmds->type, len, buf, 19);
            return;
        }
        //-------------------------------------

        memset(&G_parameter.fence[0], 0x00, sizeof(G_parameter.fence[0]));
        G_parameter.fence[0].in_out = atoi(cmds->cmd[2]);
        G_parameter.fence[0].square = atoi(cmds->cmd[3]) - 1;
        //Э����[1=Բ�ģ�2=���Σ�3=�����]����ʵ�ʱ�׼����[0=Բ�Σ�1=���Σ�2=�����]
        if(G_parameter.fence[0].square == 0) //Բ��
        {
            G_parameter.fence[0].lon = recv_s19_hex_to_dec(cmds->cmd[4]) / 3600000.0;
            G_parameter.fence[0].lat = recv_s19_hex_to_dec(cmds->cmd[5]) / 3600000.0;
            radius = (kal_uint32)recv_s19_hex_to_dec(cmds->cmd[6]) / 100;
            if(radius < 1)
            {
                radius = 1;
            }
            G_parameter.fence[0].radius = radius;
            //Э����Բ������뾶��λΪ[��]����ʵ�ʱ�׼����[����]
        }
        else if(G_parameter.fence[0].square == 1) //����
        {
            G_parameter.fence[0].lon = recv_s19_hex_to_dec(cmds->cmd[4]) / 3600000.0;
            G_parameter.fence[0].lat = recv_s19_hex_to_dec(cmds->cmd[5]) / 3600000.0;
            G_parameter.fence[0].lon2 = recv_s19_hex_to_dec(cmds->cmd[6]) / 3600000.0;
            G_parameter.fence[0].lat2 = recv_s19_hex_to_dec(cmds->cmd[7]) / 3600000.0;
        }
        else if(G_parameter.fence[0].square == 2) //�����
        {
            memset(&G_fence_polygon, 0x00, NVRAM_EF_FENCE_POLYGON_SIZE);
            part = (cmds->count - 4) >> 1;
            G_fence_polygon.count = part;
            for(i = 0; i < part; i++)
            {
                index = (i << 1) + 4;
                G_fence_polygon.lon[i] = recv_s19_hex_to_dec(cmds->cmd[index]) / 3600000.0;
                G_fence_polygon.lat[i] = recv_s19_hex_to_dec(cmds->cmd[index+1]) / 3600000.0;
            }
            Track_nvram_write(NVRAM_EF_FENCE_POLYGON_LID, 1, (void *)&G_fence_polygon, NVRAM_EF_FENCE_POLYGON_SIZE);

        }
        G_parameter.fence[0].sw = 1;
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);

        G_realtime_data.in_or_out = 0;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

    }

    len = track_cust_paket_T19(buf, sizeof(buf), flag);
    track_cmd_send_rsp(cmds->type, len, buf, 19);
}

static void recv_data_s20(cmd_online_struct *cmds)
{
    char buf[256] = {0};
    kal_int8 len;
    kal_bool flag = KAL_FALSE;

    LOGD(L_APP, L_V5, "");

    if(cmds->count == 2)
    {
        memset(&G_fence_polygon, 0x00, NVRAM_EF_FENCE_POLYGON_SIZE);
        Track_nvram_write(NVRAM_EF_FENCE_POLYGON_LID, 1, (void *)&G_fence_polygon, NVRAM_EF_FENCE_POLYGON_SIZE);
        memset(&G_parameter.fence[0], 0x00, sizeof(G_parameter.fence[0]));
        flag = Track_nvram_write(NVRAM_EF_PARAMETER_LID, 1, (void *)&G_parameter, NVRAM_EF_PARAMETER_SIZE);
    }
    len = track_cust_paket_T20(buf, sizeof(buf), flag);
    track_cmd_send_rsp(cmds->type, len, buf, 20);
}

#endif
static void recv_data_s21(cmd_online_struct *cmds)
{
    LOGD(L_APP, L_V5, "");
    //nc
}

char * track_cust_get_host_num(void)
{
    return G_importance_parameter_data.login.host_num;
}

static cust_recv_struct cust_recv_online_table[] =
{

    //{"S0",    recv_data_s0,     },      // ����������
    {"S1",    recv_data_s1,     },      // ��½������
    {"S2",    recv_data_s2,     },      // ������ָ���·�
    {"S3",    recv_data_s3,     },      //
    {"S4",    recv_data_s4,     },      //ƽ̨������ѯ�ն˾�γ����Ϣ
    {"S5",    recv_data_s5,     },      //��ѯ����
    {"S6",    recv_data_s6,     },      //���ò���
    {"S7",    recv_data_s7,     },      //�����ն�
    {"S8",    recv_data_s8,     },      //Զ�����
    {"S9",    recv_data_s9,     },      //Զ�̳���
    {"S10",   recv_data_s10,    },      //
    {"S11",   recv_data_s11,    },      //
    //{"S19",   recv_data_s19,    },      //�������򱨾�
    //{"S20",   recv_data_s20,    },      //ȡ�����򱨾�
    {" ", NULL}
};

/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/
int str_analyse(char *str_data, char **tar_data, int limit, char startChar, char *endChars, char splitChar)
{
    static char *blank = {""};
    int len, i, j, status = 0;
    char *p;
    len = strlen(str_data);
    for(i = 0, j = 0, p = str_data; i < len; i++, p++)
    {
        if(status == 0 && (*p == startChar || startChar == NULL))
        {
            status = 1;
            if(j >= limit) return -1;
            if(startChar == NULL)
                tar_data[j++] = p;
            else if(*(p + 1) == splitChar)
                tar_data[j++] = blank;
            else tar_data[j++] = p + 1;
        }
        if(status == 0) continue;
        if(strchr(endChars, *p) != NULL)
        {
            *p = 0;
            break;
        }
        if(*p == splitChar)
        {
            *p = 0;
            if(j >= limit) return -2;
            if(strchr(endChars, *(p + 1)) != NULL || *(p + 1) == splitChar)
                tar_data[j++] = blank;
            else tar_data[j++] = p + 1;
        }
    }
    for(i = j; i < limit; i++)
    {
        tar_data[i] = blank;
    }
    return j;
}

kal_int8 track_cust_recv_cmd_online(char *data, int len, Cmd_Type type)
{
    static char cmd_str[1024] = {0};
    /*static*/
    int temp_len = 0;
    char tmp[1024] = {0};
    char *cmds[150] = {0}, *start, *end;
    kal_uint8 count;
    int i, size, j = 0;
    cmd_online_struct this_cmd = {0};

    LOGD(L_APP, L_V5, "temp_len:%d,len:%d,data:%s", temp_len, len, data);
    if(len >= 1024) return -1;

    //if((temp_len + len) > 1024) return -1;

    //strncpy(&cmd_str[temp_len], data, 1023 - temp_len);
    //temp_len += len;
    strncpy(&cmd_str[0], data, len);
    while(1)
    {
        LOGD(L_APP, L_V5, "����%d,%s", j, &cmd_str[j]);
        start = strchr(&cmd_str[j], '[');
        end = strchr(&cmd_str[j], ']');
        if(start == NULL || end == NULL)
        {
            LOGD(L_APP, L_V5, "start=end=null");
            break;
        }
        strncpy(tmp, start, end - start + 1);
        j = end - cmd_str + 1;

        //count = str_analyse(tmp, cmds, 10, '[', "]", ',');
        count = str_analyse(tmp, cmds, 150, '[', "]", ',');
        /*�����55���㣬����110��       --    chengjun  2013-04-09*/

        if(count <= 0)
        {
            LOGD(L_APP, L_V5, "count<=0");

            return -2;
        }
        this_cmd.cmd = cmds;
        this_cmd.count = count;
        this_cmd.type = type;
        LOGD(L_APP, L_V6, "%s,%s,%s,%s", cmds[0], cmds[1], cmds[2], cmds[3]);
        size = sizeof(cust_recv_online_table) / sizeof(cust_recv_struct);
        for(i = 0; i < size; i++)
        {
            //LOGD(L_APP, L_V6, "%s =?= %s", cmds[1], cust_recv_table[i].cmd_string);
            if(!strcmp(cmds[1], cust_recv_online_table[i].cmd_string))
            {
                cust_recv_online_table[i].func_ptr(&this_cmd);
                break;
            }
        }
    }

    memset(tmp, 0, 1024);
    if(j > 0 && strlen(&cmd_str[j]) > 0)
    {
        strncpy(tmp, &cmd_str[j], 1023);
    }
    memset(cmd_str, 0, 1024);
    if(strlen(tmp) > 0)
    {
        strncpy(cmd_str, tmp, 1023);
    }
    return 0;
}
kal_uint8 track_fun_ascii_str_replace_chr(const kal_uint8 *search_chr, const kal_uint8 *replace_chr, kal_uint8 *data, int len)
{
    if(search_chr[0] == 0)
    {
        return 0;
    }
    while(len > 0)
    {
        if(data[0] == search_chr[0])
        {
            data[0] = replace_chr[0];
        }
        data += 1;
        len -= 1;
    }
    return 1;
}

void track_cust_cmd_build_format(kal_uint8 *data, kal_uint16 len,Cmd_Type cmd_type,CMD_DATA_STRUCT * gps_cmd)
{
    //char head="[,",end="]",split=",";
    char cmd_str[1024]={0};
    kal_uint16 str_len = 0;
    int i =0;
    kal_bool ptr = KAL_FALSE;
    kal_uint8 wchr1[1] = {0x2C};  // ,
    kal_uint8 wchr2[1] = {0x3D};  // =
    str_len = len;

    LOGH(L_APP, L_V6, data,len);
    if(strstr(data,","))//= �Ÿĳ�,
    {
        ptr = KAL_TRUE;
    }
    LOGD(L_APP, L_V6, "%d,%d", ptr,str_len);
    track_fun_ascii_str_replace_chr(wchr1,wchr2,data,len);
    track_fun_toUpper(data);    //ת���ɴ�д
    if(ptr)
    {
       if(strchr(data,'=') != strrchr(data,'=')) 
       {
            LOGD(L_APP, L_V6, "�г���ʿ##Number of Parameters is error!!!");
            return;
       }
       strncpy(&cmd_str[0],"[,S6,",5);
    }
    else
    {
        strncpy(&cmd_str[0],"[,S5,",5);
    }
    strncpy(&cmd_str[5],data,len);
    strncpy(&cmd_str[5+len],"]",1);

    str_len += 6;
    sms_cmd = gps_cmd;
    track_cust_recv_cmd_online(cmd_str, str_len, cmd_type);
    
}
#endif /* __XCWS__ */
