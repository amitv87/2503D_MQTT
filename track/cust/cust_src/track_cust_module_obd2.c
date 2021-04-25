#if defined(__OBD__)
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
typedef void (*Func_CustRecvPtr)(void);

typedef struct
{
    const char           *cmd_string;
    Func_CustRecvPtr     func_ptr;
} cust_recv_struct;

typedef struct
{
    kal_bool        valid;
    kal_bool        update_valid;
    kal_uint8       source;             /*��֧��Ϊ0��֧��1ΪCAN��2Ϊ��ϡ�3ΪOBD��4Ϊģ����㷵��*/
    kal_uint32      value;
} obd_data_struct;

/*****************************************************************************
*  Local variable                       �ֲ�����
*****************************************************************************/
static U8 obd_datas[2][11] = {0};

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
#if 0
#endif /* 0 */
/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/
static U8 sum_check(U8 *data, int len)
{
    U8 dd = 0;
    int i;
    for(i=0; i<len; i++)
    {
        dd += data[i];
    }
    dd ^= 0xFF;
    return dd;
}

static void packet(U8 *data)
{
    static U8 tt = 0, change = 0;
    LOGH(L_OBD, L_V2, data, 11);
    if(data[0] == 0x21)
    {/*������ʾ��Ϣ*/
        if(memcmp(obd_datas[0], data, 11))
        {
            change |= 1;
            memcpy(obd_datas[0], data, 11);
        }
        tt |= 1;
    }
    else if(data[0] == 0x22)
    {
        if(memcmp(obd_datas[1], data, 11))
        {
            change |= 2;
            memcpy(obd_datas[1], data, 11);
        }
        tt |= 2;
    }
    else
    {
    }
    if(tt == 3 && change)
    {
        track_cust_paket_8E(obd_datas, 22);
        tt = 0;
        change = 0;
    }
}

static void obd_cmd_hex(kal_uint8 *data, int len)
{
    static kal_uint8 status = 0;
    static U32 g_tick_new = 0;
    LOGD(L_OBD, L_V5, "�յ�����������%d ������: %0.2X %0.2X", len, data[0], data[1]);
    LOGH(L_OBD, L_V1, data, len);
    track_stop_timer(TRACK_CUST_OBD_START_TIMER_ID);
    tr_stop_timer(TRACK_CUST_OBD_OPENDATA_TIMER_ID);
    g_tick_new = OTA_kal_get_systicks();
}

static void obd_wake_up(void)
{
    kal_uint8 data[5] = {0xAA, 0x02, 0x55, 0x0A, 0xF4};
    track_drv_uart1_write_data(data, 5);
}

/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/

void track_cust_obd2_data_hex(kal_uint8 *data, int len)
{
    static kal_uint8  buf[2000] = {0};
    static kal_uint32 flag = 0;
    static kal_uint32 tick = 0, tick2 = 0;
    kal_uint16 packet_len = 0, cmd;
    kal_uint8 *start = NULL, *end = NULL;
    kal_uint8 no = 0;
    int j = 0;

    if(len <= 0) return;
    tick2 = OTA_kal_get_systicks();
    if(tick > 0 && tick2 - tick > 20 * KAL_TICKS_1_SEC)
    {
        flag = 0;
    }
    tick = tick2;
    if(flag + len < 1999)
    {
        memcpy(&buf[flag], data, len);
        flag += len;
        buf[flag] = 0;
    }
    else
    {
        if(len > 1999)
        {
            LOGD(L_OBD, L_V1, "ERROR : Data too long!");
            buf[0] = 0;
            flag = 0;
            return;
        }
        memcpy(buf, data, len);
        flag = len;
        buf[flag] = 0;
    }
    LOGD(L_OBD, L_V2, "[recv](%d,%d)", len, flag);
    //LOGH(L_OBD, L_V2, data, len);
    LOGH(L_OBD, L_V2, buf, flag);
    if(flag >= 11)
        for(j=0; j<flag-1; j++)
        {
            if((buf[j] == 0x21 || buf[j] == 0x22) && buf[j+1] == 0x08)
            {
                if(flag - j < 11)
                {
                    start = &buf[j];
                    LOGD(L_OBD, L_V2, "1<11, %d", j);
                    break;
                }
                if(sum_check(&buf[j], 10) == buf[j+10])
                {
                    packet(&buf[j]);
                    j += 11;
                    start = &buf[j];
                    if(flag - j < 11)
                    {
                        LOGD(L_OBD, L_V2, "2<11, %d", j);
                        break;
                    }
                }
                else
                {
                    LOGD(L_OBD, L_V2, "crc error");
                }
            }
        }
    if(start != NULL && start > buf)
    {
        kal_uint8 *bb;
        j = &buf[flag] - start;
        if(j > 2000)
        {
            LOGD(L_OBD, L_V1, "error");
            return;
        }
        bb = (U8*)Ram_Alloc(6, j);
        if(bb == NULL)
        {
            LOGD(L_OBD, L_V1, "error 2");
            return;
        }
        memcpy(bb, start, j);
        memcpy(buf, bb, j);
        Ram_Free(bb);
        flag = j;
    }
    else
    {
        LOGD(L_OBD, L_V2, "start:%p, buf:%p", start, buf);
    }
    LOGD(L_OBD, L_V2, "3 flag:%d, buf:%p, start:%p, end:%p", flag, buf, start, end);
}

void track_cust_decode_packet_8F(kal_uint8 *data, kal_uint16 len)
{
    LOGH(L_OBD, L_V2, data, len-2);
    if(len - 3 > 0)
        track_drv_uart1_write_data(data+1, len-3);
}

#endif /* __OBD__ */
