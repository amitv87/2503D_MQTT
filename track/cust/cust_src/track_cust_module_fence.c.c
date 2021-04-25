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
static gps_gprmc_struct tmp_gpsrmc = {0};
static struct_msg_send g_msg = {0};
static U8 fence_status = 0;
static track_gps_data_struct gps_data_buf = {0};
static kal_uint8 last_status[TRACK_DEFINE_FENCE_SUM] = {0};
static U8 g_fence_id = 0;
/****************************************************************************
*  Global Variable                      ȫ�ֱ���
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/
extern void track_cust_sms_send(kal_uint8 number_type, char *number, kal_bool isChar, kal_uint8 *data, kal_uint16 len);

/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/

#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/

/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/
/******************************************************************************
 *  Function    -  track_cust_fence_OverTime_sendsms
 *
 *  Purpose     -  �������Χ�������Ķ��ŷ���
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_fence_OverTime_sendsms(void *arg)
{
    char buf[320] = {0};
    U32 ret;
    if(G_parameter.fence[g_fence_id].sw)
    {
        static kal_uint8 in[10] =
        {
            // ��Χ������
            0x8F, 0xDB, 0x56, 0xF4, 0x68, 0x0F, 0x62, 0xA5, 0x8B, 0x66
        };
        static kal_uint8 out[10] =
        {
            // ��Χ������
            0x51, 0xFA, 0x56, 0xF4, 0x68, 0x0F, 0x62, 0xA5, 0x8B, 0x66
        };

        memset(&g_msg, 0, sizeof(struct_msg_send));
        g_msg.addUrl = KAL_TRUE;
        g_msg.cm_type = CM_SMS;
        if((kal_uint32)arg == 0)
        {
            if(TRACK_DEFINE_FENCE_SUM > 1)
            {
                sprintf(buf, "��Χ��%d������", g_fence_id + 1);
                ret = track_fun_GB2312_to_UTF16BE(buf, strlen(buf), g_msg.msg_cn, 320);
                g_msg.msg_cn_len = ret - 2;
#if defined(__GERMANY__)
                snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, "GeoFence %d Alarm - Ausfahrt! ", g_fence_id + 1);
#else
                snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Out The Fence %d Alarm!", g_fence_id + 1);
#endif
            }
            else
            {
#if defined(__GERMANY__)
                snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, "GeoFence Alarm - Ausfahrt! ");
#else
                snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Out The Fence Alarm!");
#endif
                g_msg.msg_cn_len = 10;
                memcpy(g_msg.msg_cn, out, g_msg.msg_cn_len);
            }
            track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_OutFence);
        }
        else
        {
            if((TRACK_DEFINE_FENCE_SUM > 1))
            {
                sprintf(buf, "��Χ��%d������", g_fence_id + 1);
                ret = track_fun_GB2312_to_UTF16BE(buf, strlen(buf), g_msg.msg_cn, 320);
                g_msg.msg_cn_len = ret - 2;
#if defined(__GERMANY__)
                snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, "GeoFence %d Alarm - Ainfahrt! ", g_fence_id + 1);
#else
                snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Enter The Fence %d Alarm!", g_fence_id + 1);
#endif
            }
            else
            {
#if defined(__GERMANY__)
                snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, "GeoFence Alarm - Einfahrt! ");
#else
                snprintf(g_msg.msg_en, CM_PARAM_LONG_LEN_MAX, "Enter The Fence Alarm!");
#endif
                g_msg.msg_cn_len = 10;
                memcpy(g_msg.msg_cn, in, g_msg.msg_cn_len);
            }
#if defined (__XYBB__)
#else
            track_alarm_vec_queue_remove(2, 0, TR_CUST_ALARM_InFence);
#endif /* __XYBB__ */
        }
#if defined(__MORE_FENCE__)
        if(G_parameter.fence[g_fence_id].alarm_type >= 1 && G_parameter.fence[g_fence_id].alarm_type  != 3)
        {
            track_cust_module_alarm_msg_send(&g_msg, &gps_data_buf);
        }
#else
        track_cust_module_alarm_msg_send(&g_msg, &gps_data_buf);
#endif

        if(G_parameter.fence[g_fence_id].alarm_type  == 2 || G_parameter.fence[g_fence_id].alarm_type  == 3)
        {
            track_start_timer(TRACK_CUST_ALARM_LowBattery_Call_TIMER, 60000, track_cust_make_call, (void*)1);
        }
    }
    //track_cust_module_alarm_set_type(0);
}

/******************************************************************************
 *  Function    -  fence_alart
 *
 *  Purpose     -  Χ����������
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static void fence_alart(track_gps_data_struct *gps_data, kal_uint8 fence_id, kal_bool in)
{
    kal_uint16 sn;
    kal_uint8 call_type;

    kal_bool isReturn = FALSE;

    LOGD(L_APP, L_V4, "fence_id=%d", fence_id);
    memcpy(&tmp_gpsrmc, &gps_data->gprmc, sizeof(gps_gprmc_struct));
    if(G_parameter.fence[fence_id].alarm_type == 1 || G_parameter.fence[fence_id].alarm_type == 2)
    {
        isReturn = TRUE;
    }
    if(track_nvram_alone_parameter_read()->gprson)
    {
        if(G_parameter.fence[fence_id].alarm_type == 2)
        {
            call_type = 1;
        }
        else
        {
            call_type = 0;
        }
#if defined (__XYBB__)
        if(in)
        {
            track_cust_alarm_type(TR_CUST_ALARM_InFence);
        }
        else
        {
            track_cust_alarm_type(TR_CUST_ALARM_OutFence);
        }
        track_cust_paket_0200(gps_data, track_drv_get_lbs_data());
#else
        if(in)
        {
            sn = track_cust_paket_16(gps_data, track_drv_get_lbs_data(), TR_CUST_ALARM_InFence, isReturn, call_type);
        }
        else
        {
            sn = track_cust_paket_16(gps_data, track_drv_get_lbs_data(), TR_CUST_ALARM_OutFence, isReturn, call_type);
        }
#endif /* __XYBB__ */
        if(G_realtime_data.netLogControl & 16)
        {
            cust_packet_log(16, 6, in, G_realtime_data.in_or_out[g_fence_id], 0, G_parameter.fence[g_fence_id].lat, G_parameter.fence[g_fence_id].lon);
        }
        LOGD(L_APP, L_V4, "Χ��%d,����ģʽ%d", fence_id + 1, G_parameter.fence[fence_id].alarm_type);

        if(G_parameter.fence[fence_id].alarm_type == 1 || G_parameter.fence[fence_id].alarm_type == 2)
        {
            //0 gprs 1 gprs+sms 2 gprs+sms+call 3 gprs_call
            if(track_is_timer_run(TRACK_CUST_FENCE_OVERTIME_TIMER))
            {
                LOGD(L_APP, L_V4, "��һ����Χ����������δ���!");
            }
            else
            {
#if defined (__GT300__) && defined(__LANG_ENGLISH__)
                if(0)
#else
                if(G_parameter.lang == 0)
#endif /* __GT300__ */
                {
                    if(in)
                    {
                        track_cust_fence_OverTime_sendsms((void*)1);
                    }
                    else
                    {
                        track_cust_fence_OverTime_sendsms((void*)0);
                    }
                }
                else
                {
                    if(in)
                    {
                        track_start_timer(TRACK_CUST_FENCE_OVERTIME_TIMER, TRACK_ALARM_DELAY_TIME_SECONDS, track_cust_fence_OverTime_sendsms, (void*)1);
                    }
                    else
                    {
                        track_start_timer(TRACK_CUST_FENCE_OVERTIME_TIMER, TRACK_ALARM_DELAY_TIME_SECONDS, track_cust_fence_OverTime_sendsms, (void*)0);
                    }
                }
            }
        }
        else if(G_parameter.fence[fence_id].alarm_type == 3)
        {
            track_start_timer(TRACK_ALARM_DELAY_TIME_SECONDS, 60000, track_cust_make_call, (void*)1);
        }



    }
    else
    {
        if(G_parameter.fence[fence_id].alarm_type >= 1)
        {
            if(in)
            {
                track_os_intoTaskMsgQueueExt(track_cust_fence_OverTime_sendsms, (void*)1);
            }
            else
            {
                track_os_intoTaskMsgQueueExt(track_cust_fence_OverTime_sendsms, (void*)0);
            }
        }
    }
}

/******************************************************************************
 *  Function    -  fence_status_ind
 *
 *  Purpose     -  ȷ��������Χ���ڻ���Χ����
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void fence_status_ind(void *par)
{
    kal_uint32 st = (kal_uint32)par;
    char  data[250];
    if(st == 1)
    {
        // ��
        LOGD(L_APP, L_V5, "[%d]10���ȶ� Χ����, %d, %d", g_fence_id, G_parameter.fence[g_fence_id].in_out, G_realtime_data.in_or_out[g_fence_id]);
        if(G_realtime_data.netLogControl & 8)
        {
            sprintf(data, "fence_status_ind  fence  %dout,%d,%d in\n", g_fence_id, G_parameter.fence[g_fence_id].in_out, G_realtime_data.in_or_out[g_fence_id]);
            cust_packet_log_data(8, (kal_uint8 *)data, strlen((char *)(data)));
        }
        if(G_parameter.fence[g_fence_id].in_out >= 1 && G_realtime_data.in_or_out[g_fence_id] == 1)
        {
            LOGD(L_APP, L_V5, "Χ���� ����");

            fence_alart(&gps_data_buf, g_fence_id, 0);
        }
        if(G_realtime_data.in_or_out[g_fence_id] != 2)
        {
            if(G_realtime_data.in_or_out[g_fence_id] == 0)
            {
                LOGD(L_APP, L_V5, "Χ�������״β���");
            }
            G_realtime_data.in_or_out[g_fence_id] = 2;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        }
    }
    else if(st == 2)
    {
        // ��
        LOGD(L_APP, L_V6, "[%d]10���ȶ� Χ����, %d, %d", g_fence_id, G_parameter.fence[g_fence_id].in_out, G_realtime_data.in_or_out[g_fence_id]);
        if(G_realtime_data.netLogControl & 8)
        {
            sprintf(data, "fence_status_ind  fence  %d in,%d,%d in\n", g_fence_id, G_parameter.fence[g_fence_id].in_out, G_realtime_data.in_or_out[g_fence_id]);
            cust_packet_log_data(8, (kal_uint8 *)data, strlen((char *)(data)));
        }
        if(G_parameter.fence[g_fence_id].in_out != 1 && G_realtime_data.in_or_out[g_fence_id] == 2)
        {
            LOGD(L_APP, L_V5, "Χ���� ���� g_fence_id=%d", g_fence_id);
            fence_alart(&gps_data_buf, g_fence_id, 1);
        }
        if(G_realtime_data.in_or_out[g_fence_id] != 1)
        {
            if(G_realtime_data.in_or_out[g_fence_id] == 0)
            {
                LOGD(L_APP, L_V5, "Χ�������״β���");
            }
            G_realtime_data.in_or_out[g_fence_id] = 1;
            Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
        }
    }
}

/******************************************************************************
 *  Function    -  track_cust_module_fence_check_InOut
 *
 *  Purpose     -  Χ���������ƣ���ֹ�ظ���Χ���ڻ�Χ����
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_fence_check_InOut(track_gps_data_struct *gps_data, kal_uint8 fence_id, kal_uint8 status)
{
    char  data[250];
    memcpy(&gps_data_buf, gps_data, sizeof(track_gps_data_struct));
    if(status)
    {
        LOGD(L_APP, L_V7, "%dΧ����, alart type=%d, in_or_out=%d, last_status=%d",
             fence_id, G_parameter.fence[fence_id].in_out,
             G_realtime_data.in_or_out[fence_id], last_status[fence_id]);
        // ��
        if(last_status[fence_id] != 1)
        {
            LOGD(L_APP, L_V6, "Χ����, alart type=%d, in_or_out=%d�ڣ�fence_id=%d,g_fence_id=%d",
                 G_parameter.fence[fence_id].in_out,
                 G_realtime_data.in_or_out[fence_id], fence_id, g_fence_id);
            //��ʱ������������²���id������������������ʾ�ж�
            if(!(track_is_timer_run(TRACK_CUST_FENCE_TRIGGER_TIMER) && g_fence_id != fence_id) && !track_is_timer_run(TRACK_CUST_FENCE_OVERTIME_TIMER))
            {
                LOGD(L_APP, L_V6, "Χ����=%d��10��֮�󴥷�", fence_id);
                if(G_realtime_data.netLogControl & 8)
                {
                    sprintf(data, "track_cust_module_fence_check_InOut  fence %d out\n", fence_id);
                    cust_packet_log_data(8, (kal_uint8 *)data, strlen((char *)(data)));
                }
                last_status[fence_id] = 1;
                track_start_timer(TRACK_CUST_FENCE_TRIGGER_TIMER, 10000, fence_status_ind, (void*)1);
                g_fence_id = fence_id;
            }
        }
    }
    else
    {

        // ��
        if(last_status[fence_id] != 2)
        {
            if(G_realtime_data.netLogControl & 8)
            {
                sprintf(data, "track_cust_module_fence_check_InOut  fence %d in\n", fence_id);
                cust_packet_log_data(8, (kal_uint8 *)data, strlen((char *)(data)));
            }
            LOGD(L_APP, L_V6, "Χ����, alart type=%d, in_or_out=%d�ڣ�fence_id=%d,g_fence_id=%d",
                 G_parameter.fence[fence_id].in_out,
                 G_realtime_data.in_or_out[fence_id], fence_id, g_fence_id);
            //��ʱ������������²���id������������������ʾ�ж�
            if(!(track_is_timer_run(TRACK_CUST_FENCE_TRIGGER_TIMER) && g_fence_id != fence_id) && !track_is_timer_run(TRACK_CUST_FENCE_OVERTIME_TIMER))
            {
                LOGD(L_APP, L_V5, "Χ����=%d��10��֮�󴥷�", fence_id);
                last_status[fence_id] = 2;
                g_fence_id = fence_id;
                track_start_timer(TRACK_CUST_FENCE_TRIGGER_TIMER, 10000, fence_status_ind, (void*)2);
            }
        }
    }
}

/******************************************************************************
 *  Function    -  track_cust_module_fence_gps
 *
 *  Purpose     -  Χ���������������ж�
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_fence_gps(track_gps_data_struct *gps_data, int fence_id)
{
    U16 sn, i = 0;
    i = fence_id;
#if defined(__GT310__) ||defined(__GT03F__) || defined(__GW100__)
    if(track_cust_get_work_mode() == WORK_MODE_1)
    {
        return;
    }
#if  defined(__GW100__)
    if(!track_is_timer_run(TRACK_CUST_WORK_MODE_GPSON_UPDATE_TIMER_ID))
    {
        LOGD(L_APP, L_V6, "GW100,ֻ������������gps����Χ����Ч");
        return;
    }
#endif
#elif defined(__GT370__)|| defined (__GT380__)
    if(track_cust_get_work_mode() == WORK_MODE_3)
    {
        return;
    }
#endif
    if(G_parameter.fence[fence_id].sw)
    {
        float lat, lon;

        if(gps_data->gprmc.EW == 'E')
            lon = gps_data->gprmc.Longitude;
        else
            lon = gps_data->gprmc.Longitude * -1;
        if(gps_data->gprmc.NS == 'N')
            lat = gps_data->gprmc.Latitude;
        else
            lat = gps_data->gprmc.Latitude * -1;
        if(G_parameter.fence[i].square == 0)
        {
            float ret = track_fun_get_GPS_2Points_distance(G_parameter.fence[i].lat, G_parameter.fence[i].lon, lat, lon);
            LOGD(L_APP, L_V7, "fence_id=%dԲ��, lat:%.6f, lon:%.6f, radius:=%.1fm < %dm", fence_id, lat, lon, ret, G_parameter.fence[i].radius * 100);
            if(ret <= G_parameter.fence[i].radius * 100)
            {
                // ��
                track_cust_module_fence_check_InOut(gps_data, i, 0);
            }
            else
            {
                // ��
                track_cust_module_fence_check_InOut(gps_data, i, 1);
            }
        }
        else if(G_parameter.fence[i].square == 1)
        {
            kal_uint8 flag = 0;
            //LOGD(L_APP, L_V7, "����, lon:%.6f, lat:%.6f", lon, lat);
            if(G_parameter.fence[i].lat < G_parameter.fence[i].lat2)
            {
                if(lat >= G_parameter.fence[i].lat) flag |= 1;
                if(lat <= G_parameter.fence[i].lat2) flag |= 2;
            }
            else
            {
                if(lat <= G_parameter.fence[i].lat) flag |= 1;
                if(lat >= G_parameter.fence[i].lat2) flag |= 2;
            }
            if(G_parameter.fence[i].lon < G_parameter.fence[i].lon2)
            {
                if(lon >= G_parameter.fence[i].lon) flag |= 4;
                if(lon <= G_parameter.fence[i].lon2) flag |= 8;
            }
            else
            {
                if(lon <= G_parameter.fence[i].lon) flag |= 4;
                if(lon >= G_parameter.fence[i].lon2) flag |= 8;
            }
            if(flag == 15)
            {
                // ��
                track_cust_module_fence_check_InOut(gps_data, i, 0);
            }
            else
            {
                // ��
                char tmp[5] = {0};
                track_cust_module_fence_check_InOut(gps_data, i, 1);
                if((flag & 3) == 1)
                    sprintf(tmp, "��");
                else if((flag & 3) == 2)
                    sprintf(tmp, "��");
                else
                    sprintf(tmp, "��");

                if((flag & 12) == 4)
                    LOGD(L_APP, L_V7, "����, lon:%.6f, lat:%.6f ����(��) �ϱ�(%s)", lon, lat, tmp);
                else if((flag & 12) == 8)
                    LOGD(L_APP, L_V7, "����, lon:%.6f, lat:%.6f ����(��) �ϱ�(%s)", lon, lat, tmp);
                else
                    LOGD(L_APP, L_V7, "����, lon:%.6f, lat:%.6f ����(��) �ϱ�(%s)", lon, lat, tmp);
            }
        }
    }
}

/******************************************************************************
 *  Function    -  track_cust_module_fence_cmd_update
 *
 *  Purpose     -  ָ�����Χ������ز���
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_cust_module_fence_cmd_update(kal_uint8  fence_id)
{
    if(fence_id == g_fence_id)
    {
        track_stop_timer(TRACK_CUST_FENCE_TRIGGER_TIMER);
    }
    G_realtime_data.in_or_out[fence_id] = 0;
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    last_status[fence_id] = 0;
    if(G_realtime_data.netLogControl & 16)
    {
        cust_packet_log(16, 5, G_parameter.fence[0].square, G_realtime_data.in_or_out[g_fence_id], G_parameter.fence[0].radius, G_parameter.fence[0].lat, G_parameter.fence[0].lon);
    }
}
kal_uint8 track_cust_module_get_fence_id(void)
{
    return g_fence_id;
}
void track_cust_module_set_out(void)
{
    memset(last_status, 1, TRACK_DEFINE_FENCE_SUM);
    memset(G_realtime_data.in_or_out, 2, TRACK_DEFINE_FENCE_SUM);

}

void track_cust_module_set_in(void)
{
    memset(last_status, 2, TRACK_DEFINE_FENCE_SUM);
    memset(G_realtime_data.in_or_out, 1, TRACK_DEFINE_FENCE_SUM);
}
void track_cust_module_init(void)
{
    memset(last_status, 0, TRACK_DEFINE_FENCE_SUM);
    memset(G_realtime_data.in_or_out, 0, TRACK_DEFINE_FENCE_SUM);
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
}
void cmd_seefence(CMD_DATA_STRUCT *cmd)
{
    char msg[50];
    U8   j;
    if(cmd->part == 0)
    {
        for(j = 0; j < TRACK_DEFINE_FENCE_SUM; j++)
        {
            if(j == TRACK_DEFINE_FENCE_SUM - 1)
            {
                sprintf(msg, "fence status%d:%d", j + 1, G_realtime_data.in_or_out[j]);
            }
            else
            {
                sprintf(msg, "fence status%d:%d,", j + 1, G_realtime_data.in_or_out[j]);
            }
            strcat(cmd->rsp_msg, msg);
        }
        return;
    }
    else
    {
        sprintf(cmd->rsp_msg, "error");
    }
}

#if defined(__XCWS__)
track_gps_data_struct *track_cust_get_module_fence_alarm_locale(void)
{
    return &gps_data_buf;
}

nvram_electronic_fence_struct* track_cust_get_module_fence_data(void)
{
    return G_parameter.fence;
}

#endif
