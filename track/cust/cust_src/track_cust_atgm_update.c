/*******************************************************************************************
 * Filename:    track_cust_atgm_update.c
 * Author :     Liwen
 * Date :       2018/1/4
 * Comment:     �п�΢GPS�̼���������
 ******************************************************************************************/
#if defined(__ATGM_UPDATE__) && defined(__BD_AT6558__)
/****************************************************************************
*  Include Files                        ����ͷ�ļ�
*****************************************************************************/
#include "track_os_timer.h"
#include "track_at.h"
#include "track_drv_uart.h"
#include "Track_drv_eint_gpio.h"
#include "gpio_hw.h"
#include "dcl_gpio.h"
#include "track_cust.h"
#include "track_os_ell.h"

/*****************************************************************************
*  Define                               �궨��
******************************************************************************/
#define ATGM_UPDATE_FILE "ATGM_UPDATE_FILE"
#define ATGM_HEAD 0xDB
#define ATGM_TAIL 0xDE

#define ATGM_ONECE_DATA 2000

#define ATGM_MAX_UPDATE_TIME_SEC 120
#define ATGM_REPLY_TIMEOUT_SEC 4
/*****************************************************************************
*  Typedef  Enum                        ö�ٶ���
*****************************************************************************/

/*****************************************************************************
*  Struct                               ���ݽṹ����
******************************************************************************/

/*****************************************************************************
*  Local variable                       �ֲ�����
*****************************************************************************/
static U8 update_flag = 3;//3//3�״ο���GPS����,2��������д�ļ�������,1�����ļ�д�ļ�ʧ�ܺ�����
static U8* update_file_p = NULL;
static U32 update_file_len = 0;
static U16 need_packet_cnt = 0;
static U16 index_send = 1;
static U32 update_image_offset = 0;
static U16 this_time_send_len = 0;
static U32 baud_table[] = {9600, 9600, 19200, 38400, 57600, 115200};
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
void track_cust_atgm_update_fail(U8 arg);
static void track_cust_atgm_delete_image(void);
ATGM_UPDATE_STATUS track_cust_atgm_update_status(ATGM_UPDATE_STATUS arg);
/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/
static U8 calc_checksum(U8* sentence, int len)
{
    U8 checksum = 0;
    while(len--){checksum ^= (U8) * sentence++;}
    return  checksum;
}
//ת�����
static int track_cust_atgm_encode(U8* dest, U8* src, int src_len)
{    
    int i = 0, j = 0;    
    while(i < src_len)    
    {        
        if     (src[i] == ATGM_HEAD) {dest[j++] = 0xDC;dest[j++] = 0xCB;}        
        else if(src[i] == ATGM_TAIL) {dest[j++] = 0xDC;dest[j++] = 0xCE;}
        else if(src[i] == 0xDC)      {dest[j++] = 0xDC;dest[j++] = 0xCC;}        
        else                         dest[j++] = src[i];        
        i++;    
    }    
    return j;
}
//ת�����
static int track_cust_atgm_decode(U8* dest, U8* src, int src_len)
{    
    int i = 0, j = 0;    
    while(i < src_len)    
    {        
        if(src[i] == 0xDC)        
        {            
            if     (src[i + 1] == 0xCB)dest[j++] = ATGM_HEAD;            
            else if(src[i + 1] == 0xCE)dest[j++] = ATGM_TAIL;            
            else if(src[i + 1] == 0xCC)dest[j++] = 0xDC;            
            else            
            {                
                LOGD(L_APP, L_V1, "ERROR,[%d]%04X %04X", i, src[i], src[i + 1]);                
                return -1;            
            }            
            i++;        
        }        
        else  dest[j++] = src[i];        
        i++;    
    }    
    return j;
}
//ģ����д�ɹ�,����
static void track_cust_atgm_reset_gps(void)
{
    int len;
    U8 out[10] = {0};
    U8 src[0 + 5] = {0};
    LOGD(L_APP, L_V1, "--��������ģ������--");
    len = track_cust_atgm_packet(out, src, src + 4, 0, 0x06);
    U_PutUARTBytes(GPS_UART_PORT, out, len);
    track_cust_atgm_delete_image();
    G_realtime_data.atgm_update_failtimes = 0;
    memset(G_realtime_data.atgm_version, 0, sizeof(G_realtime_data.atgm_version));
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    track_cust_restart(101, 5);
}
//����������
static void track_cust_atgm_send_filedata(void)
{
    U32 rest;int len;
    U8 out[ATGM_ONECE_DATA * 3 / 2] = {0};
    U8 src[ATGM_ONECE_DATA + 6 + 5] = {0};
    
    if(update_file_p == NULL)
    {
        LOGD(L_APP, L_V1, "ERROR");
        track_cust_atgm_update_fail(1);
        return;
    }
    if(update_image_offset < update_file_len)
    {
        track_cust_atgm_update_status(ATGM_UPDATE_STATUS_send_file);
        rest = update_file_len - update_image_offset;
        this_time_send_len = (rest > ATGM_ONECE_DATA?ATGM_ONECE_DATA:rest);
        memcpy(src + 4, (U8*)&need_packet_cnt, 2);
        memcpy(src + 4 + 2, (U8*)&index_send, 2);
        memcpy(src + 4 + 4, (U8*)&this_time_send_len, 2);
        memcpy(src + 4 + 6, update_file_p + update_image_offset, this_time_send_len);

        len = track_cust_atgm_packet(out, src, src + 4, this_time_send_len + 6, 0x05);
        LOGD(L_APP, L_V1, "---���͵�[%d/%d]������,���η���:%d/%d;��ȷ��:%d/%d---", 
            index_send, need_packet_cnt, this_time_send_len, len - 2, update_image_offset, update_file_len);
        U_PutUARTBytes(GPS_UART_PORT, out, len);
        track_start_timer(TRACK_ATGM_UPDATE_TIMEOUT_TIMER_ID, ATGM_REPLY_TIMEOUT_SEC * 1000, track_cust_atgm_update_fail, (void*)2);
    }
    else
    {
        LOGD(L_APP, L_V1, "---�������������%d/%d,�ȴ���д���֪ͨ---", update_image_offset, update_file_len);
        track_cust_atgm_update_status(ATGM_UPDATE_STATUS_send_done);
        track_start_timer(TRACK_ATGM_UPDATE_TIMEOUT_TIMER_ID, ATGM_REPLY_TIMEOUT_SEC * 3000, track_cust_atgm_update_fail, (void*)3);
    }
}

void track_cust_atgm_update_fail(U8 arg)
{
    LOGD(L_APP, L_V1, "����ʧ��:%d", arg);
    track_stop_timer(TRACK_ATGM_UPDATE_TIMEOUT_TIMER_ID);
    track_cust_restart(100, 3);
}
//�������
static int track_cust_atgm_packet(U8* out, U8* encode, U8* src, int srclen, U8 cmd)
{
    U16 len = srclen + 3;
    int out_len = 0;
    out[0] = ATGM_HEAD;
    
    memcpy(encode, (U8*)&len, 2);
    encode[2] = 0x01;
    encode[3] = cmd;
    if((encode + 4) != src && srclen != 0)memcpy(encode + 4, src, srclen);
    encode[4 + srclen] = calc_checksum(encode, srclen + 4);

    out_len = track_cust_atgm_encode(out + 1, encode, srclen + 5);
    out[out_len + 1] = ATGM_TAIL;
    out_len += 2;
    LOGH(L_FUN, L_V1, out, out_len);
    return out_len;
}
//02Э���
static void track_cust_atgm_update_setpar(void)
{
    U8 out[32] = {0};U8 encode[30] = {0};int len;
    U8 src[] = {0x01, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00};//10
    
    memcpy(src + 2, (U8*)&update_file_len, 4);
    len = track_cust_atgm_packet(out, encode, src, sizeof(src), 0x02);
    
    U_PutUARTBytes(GPS_UART_PORT, out, len);
    track_start_timer(TRACK_ATGM_UPDATE_TIMEOUT_TIMER_ID, ATGM_REPLY_TIMEOUT_SEC * 1000, track_cust_atgm_update_fail, (void*)4);
}
//��������ʱ�Ĳ�����
static void track_cust_atgm_change_baud(void)
{
    static U8 index = 6;
    int len;
    U8 out[9] = {0};
    U8 src[1 + 5] = {0};

    index--;
    if(index == 0)
    {
        track_cust_atgm_update_fail(5);
        return;
    }
    src[4] = index;
    len = track_cust_atgm_packet(out, src, src + 4, 1, 0x01);
    LOGD(L_APP, L_V1, "---����GPS������[%d]---", baud_table[index]);
    U_PutUARTBytes(GPS_UART_PORT, out, len);
    track_start_timer(TRACK_ATGM_UPDATE_TIMEOUT_TIMER_ID, ATGM_REPLY_TIMEOUT_SEC * 1000, track_cust_atgm_update_fail, (void*)6);
}

//����ʼ����
void track_cust_atgm_update_begin_req(void)
{
    char get_soft_version[] = "$PCAS20*03\r\n";
    LOGD(L_APP, L_V1, "%s", get_soft_version);
    update_flag = 0;
    track_start_timer(TRACK_ATGM_UPDATE_FORCE_RESET_TIMER_ID, ATGM_MAX_UPDATE_TIME_SEC * 1000, track_cust_atgm_update_fail, (void*)12);
    track_drv_sleep_enable(SLEEP_DRV_MOD, KAL_FALSE);
    track_cust_atgm_update_status(ATGM_UPDATE_STATUS_update_begin_req);
    U_PutUARTBytes(GPS_UART_PORT, get_soft_version, strlen(get_soft_version));
    track_start_timer(TRACK_ATGM_UPDATE_TIMEOUT_TIMER_ID, ATGM_REPLY_TIMEOUT_SEC * 1000, track_cust_atgm_update_fail, (void*)7);
    //����һ��ʼ��+1,��Ȼ��;δ֪�������¼�������ʧ��,�ɹ������0
    G_realtime_data.atgm_update_failtimes++;
    Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
}
//��������ļ��Ƿ����
kal_bool track_cust_check_atgm_update_image_exsist(void)
{
    FS_HANDLE fp = NULL;
    WCHAR FilePath[60] = {0};
    OTA_kal_wsprintf(FilePath, ATGM_UPDATE_FILE);
    fp = FS_Open(FilePath, FS_READ_ONLY);
    if(fp < 0)
    {
        LOGD(L_APP, L_V1, "δ��⵽ATGM�����ļ�");
        return 0;
    }
    FS_Close(fp);
    LOGD(L_APP, L_V1, "��⵽ATGM�����ļ�");
    return 1;
}
//��ȡ�����ļ�
static int track_cust_atgm_read_image(void)
{
    FS_HANDLE fp = NULL;
    WCHAR FilePath[60] = {0};
    U32 filelen, readsize;
    int fs_result;

    OTA_kal_wsprintf(FilePath, ATGM_UPDATE_FILE);
    fp = FS_Open(FilePath, FS_READ_ONLY);
    if(fp < 0)
    {
        return fp;
    }
    FS_GetFileSize(fp, &filelen);
    update_file_p = (U8*)Ram_Alloc(4, filelen+ 1);
    if(update_file_p == NULL)
    {
        FS_Close(fp);
        return -1;
    }
    fs_result = FS_Read(fp, update_file_p, filelen+ 1, &readsize);
    if(fs_result < 0 || readsize != filelen)
    {
        LOGS("%d,%d", readsize, filelen);
        FS_Close(fp);
        return -2;
    }
    FS_Close(fp);
    update_file_len = readsize;
    need_packet_cnt = (update_file_len / ATGM_ONECE_DATA) + 1;
    LOGD(L_APP, L_V1, "filesize:%d,readsize:%d,cnt:%d", filelen, readsize, need_packet_cnt);
    return readsize;
}
//ɾ�������ļ�
static void track_cust_atgm_delete_image(void)
{
    WCHAR FilePath[60] = {0};
    LOGD(L_APP, L_V1, "ɾ��ATGM�����ļ�");
    OTA_kal_wsprintf(FilePath, ATGM_UPDATE_FILE);
    FS_Delete(FilePath);
}
//д�����ļ�
static kal_int8 track_cust_atgm_update_write_updatefile(kal_uint8* data, kal_uint32 len)
{
    static kal_uint8 first = 0;
    int ret = 0;
    int fs_seek = 0, fs_write = 0;
    kal_uint32   file_operate_size;
    kal_uint16    CRC = 0;
    FS_HANDLE   file_hd;
    WCHAR FilePath[60] = {0};
    OTA_kal_wsprintf(FilePath, "%s", ATGM_UPDATE_FILE);
    FS_Delete(FilePath);
    file_hd = FS_Open(FilePath , FS_CREATE | FS_READ_WRITE);
    if(file_hd  < FS_NO_ERROR){FS_Close(file_hd);return -7;}
    fs_seek = FS_Seek(file_hd, 0, FS_FILE_BEGIN);
    if(fs_seek < FS_NO_ERROR){FS_Close(file_hd);return -8;}
    fs_write = FS_Write(file_hd, (void *)data, len , &file_operate_size);
    if(fs_write < FS_NO_ERROR){FS_Close(file_hd);return -6;}
    if(file_operate_size != len){FS_Close(file_hd);return -5;}
    FS_Commit(file_hd);
    FS_Close(file_hd);
    return 0;
}
/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/
//����������ʽ
U8 track_cust_atgm_return_update_flag(void)
{
    return update_flag;//1
}
//��ȡ�̼��汾ָ��
void track_cust_atgm_get_soft_version_cmd(void)
{
    char get_soft_version[] = "$PCAS06,0*1B\r\n";
    U_PutUARTBytes(GPS_UART_PORT, get_soft_version, strlen(get_soft_version));
}
//���ع̼��汾
U8* track_cust_atgm_return_version(void)
{
    return G_realtime_data.atgm_version;
}
//���ñ���̼��汾
void track_cust_atgm_set_version(U8* ver)
{
    int len = strlen(ver);
    if(len >= sizeof(G_realtime_data.atgm_version) || len < 6)return;
    if(strcmp(ver, G_realtime_data.atgm_version) != 0)//�°汾
    {
        LOGD(L_APP, L_V1, "---������ATGM�汾[%s]---", ver);
        memset(G_realtime_data.atgm_version, 0, sizeof(G_realtime_data.atgm_version));
        memcpy(G_realtime_data.atgm_version, ver, len);
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    }
}
//����״̬
ATGM_UPDATE_STATUS track_cust_atgm_update_status(ATGM_UPDATE_STATUS arg)
{
    static ATGM_UPDATE_STATUS atgm_update_status = ATGM_UPDATE_STATUS_none;
    if(arg != STATUS_CHECK)
    {
        atgm_update_status = arg;
    }
    return atgm_update_status;
}
//����Э�����
void track_cust_atgm_update_decode(U8* data, U16 len)
{
    int after_len;
    U8 after_decode[300] = {0};
    
    if(track_cust_atgm_update_status(STATUS_CHECK) >= ATGM_UPDATE_STATUS_update_done)return;
    LOGH(L_APP, L_V1, data, len);
    if(data[0] != ATGM_HEAD)
    {
        LOGD(L_APP, L_V1, "---��Э������---");
        return;
    }
    if(len > sizeof(after_decode) || len < 5)
    {
        LOGD(L_APP, L_V1, "---���ȴ���---");
        return;
    }
    track_stop_timer(TRACK_ATGM_UPDATE_TIMEOUT_TIMER_ID);
    after_len = track_cust_atgm_decode(after_decode, data + 1, len - 2);
    if(after_len != (len - 2))
    {
        LOGH(L_APP, L_V1, after_decode, after_len);
    }
    
    if(after_decode[3] == 0x02)
    {
        if(after_decode[6] == 0x00)
        {
            LOGD(L_APP, L_V1, "---��ʼ����������---");
            track_cust_atgm_send_filedata();
        }
        else
        {
            track_cust_atgm_update_fail(8);
        }
    }
    else if(after_decode[3] == 0x05)
    {
        U16 confirm_index = 0;
        memcpy((U8*)&confirm_index, &after_decode[4], 2);
        if(after_decode[6] == 0x00 || after_decode[6] == 0x02)
        {
            LOGD(L_APP, L_V1, "--��%d���յ��ɹ�ȷ��--", index_send);
            if(confirm_index != index_send)
            {
                LOGD(L_APP, L_V1, "����%d %d", index_send, confirm_index);
                track_cust_atgm_update_fail(9);
                return;
            }
            index_send = confirm_index + 1;
            update_image_offset += this_time_send_len;
            track_cust_atgm_send_filedata();
        }
        else
        {
            track_cust_atgm_update_fail(10);
        }
        
    }
    else if(after_decode[3] == 0x06)
    {
        if(after_decode[4] == 0x00)
        {
            LOGD(L_APP, L_V1, "--�ɹ�ȷ��ģ������--");
        }
    }
    else if(after_decode[3] == 0x86)
    {
        if(after_decode[4] == 0x00)
        {
            LOGD(L_APP, L_V1, "--ģ�������ɹ�!--");
            track_cust_atgm_update_status(ATGM_UPDATE_STATUS_update_done);
            track_cust_atgm_reset_gps();
        }
        else
        {
            track_cust_atgm_update_fail(11);
        }
    }
    else if(after_decode[3] == 0x01)
    {
        if(after_decode[5] == 0x00)
        {
            track_drv_mgps_uart_init_for_atgm(baud_table[after_decode[4]]);
            tr_start_timer(TRACK_ATGM_UPDATE_DELAY_TIMER_ID, 2 * 1000, track_cust_atgm_update_setpar);
        }
        else if(after_decode[5] == 0x01)
        {
            LOGD(L_APP, L_V1, "��֧�ֵĲ�����");
            track_cust_atgm_change_baud();
        }
    }
}

void track_cust_atgm_update_start(void)
{
    track_cust_atgm_update_status(ATGM_UPDATE_STATUS_update_start);
    track_cust_atgm_change_baud();
}
//��������ļ����ж��Ƿ��������
int track_cust_atgm_update_check_updatefile(void)//return 0 ��������
{
    update_flag = 0;
    if(G_realtime_data.atgm_update_failtimes > ATGM_MAX_FAIL_TIMES)
    {
        LOGD(L_APP, L_V1, "���ݴ���%d", G_realtime_data.atgm_update_failtimes);
        G_realtime_data.atgm_update_failtimes = 0;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);
    }
    LOGD(L_APP, L_V1, "ATGMʧ�ܴ���:%d", G_realtime_data.atgm_update_failtimes);
    if(track_cust_check_atgm_update_image_exsist())
    {
        if(track_cust_atgm_read_image() > 0 && G_realtime_data.atgm_update_failtimes < ATGM_MAX_FAIL_TIMES)
        {
            track_cust_atgm_update_begin_req();
            return 0;
        }
        track_cust_atgm_delete_image();
    }
    if(G_realtime_data.atgm_update_failtimes == ATGM_MAX_FAIL_TIMES)
    {
        LOGD(L_APP, L_V1, "ATGM�������ʧ��,��������%d", G_realtime_data.atgm_update_failtimes);
    }
    return 1;
}

void track_cust_atgm_update_init(void)
{
    if(update_file_p != NULL)
    {
        Ram_Free(update_file_p);
    }
    index_send = 1;
    update_image_offset = 0;
    update_file_len = 0;
    need_packet_cnt = 0;
}
//�����ļ����ݽ������
void track_cust_atgm_update_recv_file(U8 *data, U32 len)
{
    kal_int8 ret;
    if((data == NULL) || (len < 1))
    {
        LOGD(L_APP, L_V1, "���ص�������������쳣");
        return;
    }
    LOGD(L_APP, L_V1, "ATGM�����ļ��������,��ʼ����");
    update_flag = 0;
    track_cust_atgm_update_init();
    //����д���ļ�,�������ʧ��,����ʹ���ļ��ٴ�����,��ʡ����
    if((ret = track_cust_atgm_update_write_updatefile(data, len)) != 0)//д�ļ�ʧ��,ֱ�����ڴ���������
    {
        LOGD(L_APP, L_V1, "д�ļ�ʧ��,����������ʽ%d", ret);
        track_cust_atgm_delete_image();
        update_file_p = (U8*)Ram_Alloc(4, len + 1);
        memcpy(update_file_p, data, len);
        update_file_len = len;
        need_packet_cnt = (update_file_len / ATGM_ONECE_DATA) + 1;

        update_flag = 1;
    }
    else
    {
        update_flag = 2;
    }
    track_cust_gps_work_req((void*)7);
}

#endif /*__ATGM_UPDATE__*/


