/******************************************************************************
 * track_drv_gps_epo.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        EPO ����������λ����
 *
 * modification history
 * --------------------
 * v1.0   2013-03-12,  chengjun create this file
 *
 ******************************************************************************/
/****************************************************************************
* Include Files             ����ͷ�ļ�
*****************************************************************************/
#include "track_drv_gps_epo.h"
#include "stack_msgs.h"
#include "stack_ltlcom.h"
#include "app_datetime.h"
#include "track_cmd.h"

#include "gps_struct.h"
/*****************************************************************************
 *  Define			    �궨��
*****************************************************************************/
//#define __AUTO_TEST_EPO_DEBUG__

#define __EPO_LONG_DATA__
//�Ӵ�EPO�������ݵ��η��͵ĳ��ȣ���ߴ���Ч��

#define EPO_LONG_DATA_HEAD "AT^GT_LONG="

/*****************************************************************************
* Typedef  Enum         ö��
*****************************************************************************/


/*****************************************************************************
 *  Struct			    ���ݽṹ����
*****************************************************************************/
#if defined (__AUTO_TEST__)
static mtk_param_epo_time_cfg epo_time_cfg = {0};
#endif /* __AUTO_TEST__ */

/*****************************************************************************
* Local variable            �ֲ�����
*****************************************************************************/

/****************************************************************************
* Global Variable           ȫ�ֱ���
*****************************************************************************/


/****************************************************************************
* Global Variable - Extern          ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
 *  Global Functions	- Extern	    �����ⲿ����
*****************************************************************************/
extern void track_cust_secset(void* p1, float* lat1, float* lon1);

/*****************************************************************************
 *  Local Functions			���غ���
 *****************************************************************************/

#if defined (__AUTO_TEST__)
/******************************************************************************
 *  Function    -  track_test_epo_char_convey
 *
 *  Purpose     -  ��16���Ƶ��ַ���ת��Ϊ��ֵ
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 13-05-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static kal_uint8 track_test_epo_char2int(char p)
{
    if(p >= '0' && p <= '9')
    {
        return (p - '0');
    }
    else if(p >= 'A' && p <= 'F')
    {
        return (p - 'A' + 10);
    }
    else if(p >= 'a' && p <= 'f')
    {
        return (p - 'a' + 10);
    }
    else
    {
        return 0xFF;
    }
}

/******************************************************************************
 *  Function    -  track_test_epo_decode_time
 *
 *  Purpose     -  ע��ο�ʱ��
 *
 *  Description -  AT^GT_CM=EPO,TIME,year,mon,date,hour,min,sec
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 12-03-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_epo_decode_time(char **str)
{
    epo_time_cfg.u2YEAR = atoi(str[2]);
    epo_time_cfg.u1MONTH = atoi(str[3]);
    epo_time_cfg.u1DAY = atoi(str[4]);
    epo_time_cfg.u1HOUR = atoi(str[5]);
    epo_time_cfg.u1MIN = atoi(str[6]);
    epo_time_cfg.u1SEC = atoi(str[7]);

#if defined (__AUTO_TEST_EPO_DEBUG__)
    LOGD(L_DRV, L_V4, "EPO time %04d-%02d-%02d %02d:%02d:%02d", \
         epo_time_cfg.u2YEAR, epo_time_cfg.u1MONTH, epo_time_cfg.u1DAY, \
         epo_time_cfg.u1HOUR, epo_time_cfg.u1MIN, epo_time_cfg.u1SEC);
#endif /* __AUTO_TEST_EPO_DEBUG__ */

    track_drv_sys_mtk_gps_set_param(MTK_PARAM_CMD_CONFIG_EPO_TIME, &epo_time_cfg);
    LOGS("EPO_TIME_OK");

}

void track_epo_update_time(mtk_param_epo_time_cfg *epo_time)
{
    LOGD(L_DRV, L_V5, "EPO time %04d-%02d-%02d %02d:%02d:%02d", \
         epo_time->u2YEAR, epo_time->u1MONTH, epo_time->u1DAY, \
         epo_time->u1HOUR, epo_time->u1MIN, epo_time->u1SEC);
    track_drv_sys_mtk_gps_set_param(MTK_PARAM_CMD_CONFIG_EPO_TIME, epo_time);
    LOGD(L_DRV, L_V4, "EPO_TIME_OK");
}


/******************************************************************************
 *  Function    -  track_test_epo_decode_pos
 *
 *  Purpose     -  ע��ο�λ��
 *
 *  Description -  AT^GT_CM=EPO,POS,lat,lon,alt
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 12-03-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_epo_decode_pos(char **str)
{
    mtk_param_epo_pos_cfg epo_pos_cfg;

    //Minus: south; Plus: north
    epo_pos_cfg.dfLAT = (mtk_r8)atof(str[2]);

    //Minus: west; Plus: east
    epo_pos_cfg.dfLON = (mtk_r8)atof(str[3]);

    //ellipsoidal altitude(m)
    epo_pos_cfg.dfALT = (mtk_r8)atof(str[4]);

    epo_pos_cfg.u2YEAR = epo_time_cfg.u2YEAR;
    epo_pos_cfg.u1MONTH = epo_time_cfg.u1MONTH;
    epo_pos_cfg.u1DAY = epo_time_cfg.u1DAY;
    epo_pos_cfg.u1HOUR = epo_time_cfg.u1HOUR;
    epo_pos_cfg.u1MIN = epo_time_cfg.u1MIN;
    epo_pos_cfg.u1SEC = epo_time_cfg.u1SEC - 1;


#if defined (__AUTO_TEST_EPO_DEBUG__)
    LOGD(L_DRV, L_V4, "EPO pos lat=%f,lon=%f,alt=%f", epo_pos_cfg.dfLAT, epo_pos_cfg.dfLON, epo_pos_cfg.dfALT);
#endif /* __AUTO_TEST_EPO_DEBUG__ */

    track_drv_sys_mtk_gps_set_param(MTK_PARAM_CMD_CONFIG_EPO_POS, &epo_pos_cfg);
    LOGS("EPO_POS_OK");

}

/******************************************************************************
 *  Function    -  track_test_epo_decode_data
 *
 *  Purpose     -  ע������
 *
 *  Description - AT^GT_CM=EPO,DATA, index,"36�ֽ�ʮ���������ݵ�ASCII"
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 12-03-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_epo_decode_data(char **str)
{
    static kal_uint8 epo_data_hex[72] = {0};
    kal_uint8 epo_half[36] = {0};
    mtk_param_epo_data_cfg epo_data;
    kal_uint8 *epo_data_str;
    kal_uint8 index, i, temp;

    index = atoi(str[2]); // 1-64   (32*72/36=64)
    if((index < 1) || (index > 64))
    {
        LOGS("EPO_DATA_ERROR");
        return;
    }

    epo_data_str = str[3];
    if(strlen(epo_data_str) != 72)
    {
        LOGS("EPO_DATA_ERROR");
        return;
    }

    memset(epo_half, 0x00, sizeof(epo_half));
    for(i = 0; i < 36; i++)
    {
        temp = track_test_epo_char2int(*epo_data_str);
        epo_half[i] = temp << 4;
        epo_data_str++;

        temp = track_test_epo_char2int(*epo_data_str);
        epo_half[i] |= temp;
        epo_data_str++;
    }

    if((index & 0x01) == 0)
    {
        memcpy(&epo_data_hex[36], epo_half, 36);

        memset(&epo_data, 0x00, sizeof(epo_data));
        memcpy(epo_data.u4EPOWORD, epo_data_hex, sizeof(epo_data.u4EPOWORD));
        epo_data.u1SatID = index >> 1; //1-32

#if defined (__AUTO_TEST_EPO_DEBUG__)
        LOGD(L_DRV, L_V4, "EPO data (%d)", epo_data.u1SatID);
        LOGH(L_DRV, L_V4, epo_data_hex, 72);
#endif /* __AUTO_TEST_EPO_DEBUG__ */

        track_drv_sys_mtk_gps_set_param(MTK_PARAM_CMD_CONFIG_EPO_DATA, &epo_data);
        memset(epo_data_hex, 0x00, sizeof(epo_data_hex));

        LOGS("EPO_DATA_%02d_OK", index);
    }
    else
    {
        memcpy(epo_data_hex, epo_half, 36);
        LOGS("EPO_DATA_%02d_OK", index);
    }
}


#if defined (__EPO_LONG_DATA__)

/******************************************************************************
 *  Function    -  track_test_epo_deocde_long_data
 *
 *  Purpose     -  ע������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 13-05-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_test_epo_deocde_long_data(kal_int8 id, char* epo_str)
{
    char epo_long_data_str[1154] = {0};
    //1154��Э���йأ����ܸ�
    //1152�ַ���=576�ֽ���Ч16��������=8С��(72�ֽ�)����
    kal_uint8 epo_data_hex[72] = {0};
    char *p_epo = NULL, *p_epo_end = NULL;
    mtk_param_epo_data_cfg epo_data = {0};
    kal_uint8 temp, i = 0;
    kal_int8 index = 0;

    LOGD(L_DRV, L_V4, "id=%d", id);
    index = ((id - 1) << 3) + 1;
    strncpy(epo_long_data_str, epo_str, 1152);

    p_epo = epo_long_data_str;
    p_epo_end = epo_long_data_str + 1152;

    memset(&epo_data_hex, 0x00, sizeof(epo_data_hex));
    while(p_epo < p_epo_end)
    {
        temp = track_test_epo_char2int(*p_epo);
        epo_data_hex[i] = temp << 4;
        p_epo++;

        temp = track_test_epo_char2int(*p_epo);
        epo_data_hex[i] |= temp;
        p_epo++;

        i++;
        if(i >= 72) //144���ַ�=72�ֽ�-->>ע������
        {
            memset(&epo_data, 0x00, sizeof(epo_data));
            memcpy(epo_data.u4EPOWORD, epo_data_hex, sizeof(epo_data.u4EPOWORD));
            epo_data.u1SatID = index; //1-32

#if defined (__AUTO_TEST_EPO_DEBUG__)
            LOGD(L_DRV, L_V4, "EPO data (%d)", epo_data.u1SatID);
            LOGH(L_DRV, L_V4, epo_data_hex, 72);
#endif /* __AUTO_TEST_EPO_DEBUG__ */

            track_drv_sys_mtk_gps_set_param(MTK_PARAM_CMD_CONFIG_EPO_DATA, &epo_data);
            memset(epo_data_hex, 0x00, sizeof(epo_data_hex));
            index++;
            i = 0;
        }
    }
    LOGS("EPO_DATA_%02d_OK", id);
}

/******************************************************************************
 *  Function    -  track_test_epo_long_data
 *
 *  Purpose     -  ����EPO��������
 *
 *  Description -  AT^GT_LONG=EPO_DATA,index,"576�ֽ�ʮ���������ݵ�ASCII"
 *
 *                 ǰ��ӵ�EPO_DATA���Ƕ���ģ�
 *                 ��Ҫ��Ϊ�˱����Ժ���չ����ATָ�Ԥ����ʶ����
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 13-05-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static kal_int8 track_test_epo_long_data(kal_char*str_epo)
{
    kal_char *p_str_epo = NULL, *p_split = NULL;
    kal_int8 id;
    char head[] = {"EPO_DATA,"};

    LOGD(L_DRV, L_V4, "");
    p_str_epo = strstr(str_epo, head);
    if(p_str_epo == NULL)
    {
        return -1;
    }

    p_str_epo = p_str_epo + strlen(head);
    p_split = strchr(p_str_epo, ',');
    if(p_split == NULL)
    {
        return -2;
    }
    *p_split = 0;

    id = (kal_int8)atoi(p_str_epo);
    if((id < 1) || (id > 4))
    {
        return -3;
    }

    p_str_epo = p_split + 1; //ָ����Ч��������ͷ

    p_split = strchr(p_str_epo, 0x0D); //���������ǿɼ��ַ�������������0x0D��һ���ǽ�����
    if(p_split == NULL)
    {
        return -4;
    }
    *p_split = 0; //������0D 0A��Ϊ'\0'������

    if(strlen(p_str_epo) != 1152)
    {
        return -5;
    }

    track_test_epo_deocde_long_data(id, p_str_epo);

    return id;
}
#endif /* __EPO_LONG_DATA__ */

/*****************************************************************************
 *  Global Functions            ȫ�ֺ���
*****************************************************************************/

void track_epo_data_day(kal_uint8 days, kal_uint8 *data, int len)
{
    mtk_param_epo_data_cfg epo_data = {0};
    kal_uint8 u1SatID;
    kal_uint8 segment;
    kal_uint8 day;

    for(day = 0; day < days; day++)
    {
        for(segment = 0; segment < 4; segment++)
        {
            for(u1SatID = 1; u1SatID <= 32; u1SatID++)
            {
                memset(&epo_data, 0, sizeof(epo_data));
                memcpy(epo_data.u4EPOWORD, data, 72);
                epo_data.u1SatID = u1SatID;//1-32
                track_drv_sys_mtk_gps_set_param(MTK_PARAM_CMD_CONFIG_EPO_DATA, &epo_data);  // ���� 72�ֽ� ע������
                LOGD(L_DRV, L_V6, "EPO data (%d,%d,%d)", day, segment, u1SatID);
                LOGH(L_DRV, L_V9, data, 72);
                data += 72;
            }
        }
    }
}


/******************************************************************************
 *  Function    -  track_epo_data_decode
 *
 *  Purpose     -  �������յ���EPO����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 12-03-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_epo_data_decode(CMD_DATA_STRUCT * Cmds)
{
    kal_uint16 part;
    char* type;

    if(Cmds == NULL)
    {
        return;
    }

    part = Cmds->part;
    type = Cmds->pars[1];

    if(strcmp(type, (char*)"TIME") == 0)
    {
        if(part == 7)
        {
            track_test_epo_decode_time(Cmds->pars);
        }
        else
        {
            LOGS("EPO_TIME_ERROR");
        }
    }
    else if(strcmp(type, (char*)"POS") == 0)
    {
        if(part == 4)
        {
            track_test_epo_decode_pos(Cmds->pars);
        }
        else
        {
            LOGS("EPO_POS_ERROR");
        }
    }
    else if(strcmp(type, (char*)"DATA") == 0)
    {
        if(part == 3)
        {
            track_test_epo_decode_data(Cmds->pars);
        }
        else
        {
            LOGS("EPO_DATA_ERROR");
        }
    }
    else
    {
        LOGD(L_DRV, L_V4, "Error: This command does not exist");
    }
}

#if defined (__EPO_LONG_DATA__)
/******************************************************************************
 *  Function    -  track_is_epo_long_data
 *
 *  Purpose     -  �Ƿ���EPO��������
 *
 *  Description -  ��ATָ��ȳ���1K�����뵥���г�������
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 13-05-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_bool track_is_epo_long_data(kal_uint16 leng, char* string)
{
    kal_char *p_str_epo = NULL;
    kal_int8 error;

    if((track_is_autotestmode() == KAL_FALSE) && (track_is_testmode() == KAL_FALSE))
    {
        return KAL_FALSE;
    }

    p_str_epo = strstr(string, EPO_LONG_DATA_HEAD);
    if(p_str_epo == NULL)
    {
        return KAL_FALSE;
    }

    error = track_test_epo_long_data(p_str_epo);
    if(error < 0)
    {
        LOGS("EPO_DATA_ERROR");
        LOGC(L_DRV, L_V5, "epo long data error=%d", error);
    }

    return KAL_TRUE;

}

#endif /* __EPO_LONG_DATA__ */

#endif /* __AUTO_TEST__ */

void track_cust_aiding_reftime(applib_time_struct *currTime)
{
    char singlebuf[100] = {0};
    U8 checksum = 0;
    if(currTime == NULL)
    {
        applib_time_struct tmp = {0};
        applib_dt_get_rtc_time(&tmp);
        if(tmp.nYear == 2015)
        {
            return;
        }
        snprintf(singlebuf, 99, "$PMTK740,%d,%d,%d,%d,%d,%d", tmp.nYear, tmp.nMonth, tmp.nDay, tmp.nHour, tmp.nMin, tmp.nSec);
    }
    else
    {
        snprintf(singlebuf, 99, "$PMTK740,%d,%d,%d,%d,%d,%d", currTime->nYear, currTime->nMonth, currTime->nDay, currTime->nHour, currTime->nMin, currTime->nSec);
    }
    checksum = track_cust_xor_buf(&singlebuf[1], strlen(singlebuf) - 1);
    snprintf(singlebuf + strlen(singlebuf), 99, "*%02X", checksum);
    track_drv_uart2_write_data(singlebuf, strlen(singlebuf));
    LOGD(L_GPS, L_V5, "%s, %d, %d, %x", singlebuf, strlen(singlebuf), checksum, checksum);

}

void track_drv_statistics_conn_time(U8 op)
{
    static U32 tick = 0;
    static U8 sts = 1;
    LOGD(L_TEST, L_V6, "op %d", op);
    if(op == 1 && (tick == 0))
    {
        tick = kal_get_systicks();
        LOGD(L_TEST, L_V2, "������������ %d", tick);
    }
    else if(op == 2 && tick > 0)
    {
        kal_uint32 curr_tick;
        curr_tick = kal_get_systicks();
        LOGD(L_TEST, L_V2, "�����ɹ� %d ��, curr %d, tick %d", kal_ticks_to_secs(curr_tick - tick), curr_tick, tick);
        LOGD(L_TEST, L_V6, "�����������ɹ� %d��", kal_ticks_to_secs(curr_tick - 1));
        tick = curr_tick;
    }
    else if(op == 3 && tick > 0)
    {
        kal_uint32 curr_tick;
        curr_tick = kal_get_systicks();
        LOGD(L_TEST, L_V2, "SERVER1 �ɹ���ȡʱ�� %d ��, curr %d, tick %d", kal_ticks_to_secs(curr_tick - tick), curr_tick, tick);
    }
    else if(op == 4 && tick > 0)
    {
        kal_uint32 curr_tick;
        curr_tick = kal_get_systicks();
        LOGD(L_TEST, L_V2, "SERVER2 �ɹ���ȡʱ�� %d ��, curr %d, tick %d", kal_ticks_to_secs(curr_tick - tick), curr_tick, tick);
    }
    else if(sts && (op == 5) && tick > 0)
    {
        kal_uint32 curr_tick;
        sts = 0;
        curr_tick = kal_get_systicks();
        LOGD(L_TEST, L_V2, "SYN_EPO �ɹ���ȡʱ�� %d ��, curr %d, tick %d", kal_ticks_to_secs(curr_tick - tick), curr_tick, tick);
    }
}

void track_drv_cal_secset(U8 op)
{
    static kal_uint32 tick = 0;
    static kal_uint32 tick2 = 0;
    LOGD(L_TEST, L_V7, "op %d", op);
    if(op == 1)
    {
        tick = kal_get_systicks();
        LOGD(L_TEST, L_V4, "������GPS %d", tick);
    }
    else if(op == 2 && tick > 0)
    {
        kal_uint32 curr_tick;
        curr_tick = kal_get_systicks();
        LOGD(L_TEST, L_V4, "�����п�����λ����ɶ�λ��ʱ %d ��, curr %d, tick %d", kal_ticks_to_secs(curr_tick - tick), curr_tick, tick);
        LOGD(L_TEST, L_V4, "ע��ο�λ����ɶ�λ��ʱ%d��", tick2 <= 0 ? tick2 : kal_ticks_to_secs(curr_tick - tick2));
        tick = 0;
        tick2 = 0;
    }
    if(op == 3 && (tick2 == 0))
    {
        tick2 = kal_get_systicks();
    }
}

//$PMTK741,22.568370,113.866129,0000,2016,04,22,13,33,14*24

kal_bool track_drv_epo_aid_interface()
{
    applib_time_struct currTime = {0};
    kal_bool epoadid = KAL_FALSE;
    kal_uint32 chip_gps_hours;
	gps_aiding_hours_struct *myMsgPtr =NULL;
	ilm_struct *send_ilm;
    track_fun_get_time(&currTime, KAL_TRUE, NULL);//utcʱ��
    if(currTime.nYear != 2015)
    {
        int hours = track_utc_to_gpsepo_hour(currTime.nYear, currTime.nMonth, currTime.nDay, currTime.nHour);
        LOGD(L_GPS, L_V4, "int %d, U32 %d", hours, chip_gps_hours);
        chip_gps_hours = (kal_uint32)hours;
    }
    else if(currTime.nYear == 2015)
    {
        return epoadid;
    }
       myMsgPtr = (gps_aiding_hours_struct*) construct_local_para(sizeof(myMsgPtr), TD_RESET);
       myMsgPtr->chip_gps_hours = chip_gps_hours;
       track_fun_msg_send(MSG_ID_EPO_AIDING_ID, myMsgPtr, MOD_MMI, MOD_GPS);
    return 0;
}

