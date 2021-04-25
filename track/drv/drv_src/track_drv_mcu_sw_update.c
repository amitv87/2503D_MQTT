/******************************************************************************
 * track_drv_mcu_sw_update.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        ��Ƭ�����������BIN�ļ�ͨ������YmodeЭ�����ص�MCU
 *
 * modification history
 * --------------------
 * v1.0   2016-09-06,  chengjun create this file
 *
 ******************************************************************************/
#if defined (__MCU_SW_UPDATE__)

/****************************************************************************
* Include Files                   		����ͷ�ļ�
*****************************************************************************/
#include "track_os_data_types.h"
#include "track_os_timer.h"
#include "track_os_log.h"
#include "c_RamBuffer.h"
#include "fs_gprot.h"
#include "track_os_ell.h"

/*****************************************************************************
 *  Define					�궨��
 *****************************************************************************/
//YmodeЭ��
#define PACKET_SEQNO_INDEX      (1)
#define PACKET_SEQNO_COMP_INDEX (2)

#define PACKET_HEADER           (3)
#define PACKET_TRAILER          (2)
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             (128)
#define PACKET_1K_SIZE          (1024)

#define FILE_NAME_LENGTH        (256)
#define FILE_SIZE_LENGTH        (16)

#define SOH                     (0x01)  /* start of 128-byte data packet */
#define STX                     (0x02)  /* start of 1024-byte data packet */
#define EOT                     (0x04)  /* end of transmission */
#define ACK                     (0x06)  /* acknowledge */
#define NAK                     (0x15)  /* negative acknowledge */
#define CA                      (0x18)  /* two of these in succession aborts transfer */
#define CRC16                   (0x43)  /* 'C' == 0x43, request 16-bit CRC */

#define ABORT1                  (0x41)  /* 'A' == 0x41, abort by user */
#define ABORT2                  (0x61)  /* 'a' == 0x61, abort by user */

#define NAK_TIMEOUT             (0x100000)
#define MAX_ERRORS              (5)

#define MODEM_SOH 0x01       // ���ݿ���ʼ�ַ�
#define MODEM_STX 0x02       // 1028�ֽڿ�ʼ
#define MODEM_EOT 0x04       // �ļ��������
#define MODEM_ACK 0x06       // ȷ��Ӧ��
#define MODEM_NAK 0x15       // ���ִ���
#define MODEM_CAN 0x18       // ȡ������
#define MODEM_C   0x43       // ��д��ĸC

#define MCU_NEW_SW_FILE_NAME "chengj.hex"

/*****************************************************************************
* Typedef  Enum
*****************************************************************************/
typedef enum
{
    UODATE_NO_NEED,
    UPDATE_STATUE_ENABLE_REQ,   //�� AT^GT_CM=99#
    UPDATE_STATUE_ENABLE_RSP,   //�յ� AT^GT_CM=99,OK#
    UPDATE_BOOTLOADER_MODE_REQ,     //�� UPGRADE
    UPDATE_BOOTLOADER_MODE_RSP,     //�յ� CCCCC
    UPDATE_YMODE_DOWNLOAD,          //Ymode������
    UPDATE_YMODE_EOT_END,
    UPDATE_NEW_SW_FINISH,           //�չ�
} mcu_sw_update_status_enum;

/*****************************************************************************
 *  Global Functions	- Extern		�����ⲿ����
 *****************************************************************************/
extern void track_drv_uart2_write_data(kal_uint8 *data, int len);

/*****************************************************************************
 *  Struct					���ݽṹ����
 *****************************************************************************/


/*****************************************************************************
* Local variable				�ֲ�����
*****************************************************************************/
static mcu_sw_update_status_enum mcu_sw_update_status = UODATE_NO_NEED;

static kal_uint8 *mcu_new_sw_buf = NULL;
static kal_uint32 mcu_new_sw_size = 0;

static kal_uint8  *SourceBuf;
static U32 pktNo = 0;

static WCHAR mcu_new_sw_file_name[128] = {0};

/*****************************************************************************
 *  Global Functions			ȫ�ֱ���
 *****************************************************************************/


/*****************************************************************************
 *  Local Functions			���غ���
 *****************************************************************************/
//static
void track_mcu_sw_update_start_req(void);
static void track_mcu_bootloader_mode_req(void);
static void track_mcu_sw_ymodem_begin(void);
static void track_mcu_new_sw_download_bin();
static void track_send_cmd_update_data(void *arg);
static void track_mcu_sw_update_finish(void);

static track_drv_uart_write_data_to_mcu(kal_uint8 *data, int len)
{
    //track_drv_uart2_write_data(data, len);
    track_drv_write_exmode(data, len);
}

static void track_drv_set_mcu_sw_update_status(mcu_sw_update_status_enum status)
{
    mcu_sw_update_status = status;
}

static kal_bool track_str_char(char* data, U8 c, U32 len)
{
    U32 j;
    for(j = 0; j < len ; j++)
    {
        if(data[j] == c)
        {
            return KAL_TRUE;
        }
    }
    return KAL_FALSE;
}

unsigned short track_fun_Get_CRC16_CCITT(unsigned char *ptr, unsigned int len)
{
    static unsigned short crc_table[16] ={0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef};
    unsigned short crc = 0;
    unsigned char crc_H4;
    while( len-- )
    {
        crc_H4 = (unsigned char)(crc >> 12);
        crc = crc << 4;
        crc = crc ^ crc_table[ crc_H4 ^ (*ptr >> 4)];
        crc_H4 = (unsigned char)(crc >> 12);
        crc = crc << 4;
        crc = crc ^ crc_table[ crc_H4 ^ (*ptr & 0x0f)];
        ptr++;
    }
    return crc;
}


kal_int8 track_new_sw_read(void)
{
    static kal_uint8 first = 0;
    int ret = 0;
    int fs_seek = 0, fs_read = 0;
    kal_uint32   file_operate_size, data_len;
    kal_uint16    CRC = 0;
    FS_HANDLE   file_hd;
    kal_uint32  size;

    OTA_kal_wsprintf(mcu_new_sw_file_name, "%s", MCU_NEW_SW_FILE_NAME);

    file_hd = FS_Open(mcu_new_sw_file_name ,  FS_READ_ONLY);
    if(file_hd < FS_NO_ERROR)
    {
        return -1;

    }
    FS_GetFileSize(file_hd, &size);
    if((size <= 0) || (size >= 30 * 1024))
    {
        return -2;
    }

    if(mcu_new_sw_buf != NULL)
    {
        Ram_Free(mcu_new_sw_buf);
    }
    mcu_new_sw_buf = (U8*)Ram_Alloc(7, size);

    mcu_new_sw_size = size;

    fs_seek = FS_Seek(file_hd, 0, FS_FILE_BEGIN);
    fs_read = FS_Read(file_hd, mcu_new_sw_buf, size , &file_operate_size);
    if(fs_read < FS_NO_ERROR)
    {
        FS_Close(file_hd);
        return -3;
    }

    FS_Close(file_hd);
    return 0;
}

kal_int8 track_new_sw_write(kal_uint8* data, kal_uint32 len)
{
    static kal_uint8 first = 0;
    int ret = 0;
    int fs_seek = 0, fs_write = 0;
    kal_uint32   file_operate_size, data_len;
    kal_uint16    CRC = 0;
    FS_HANDLE   file_hd;

    if((len <= 0) || (len >= 24 * 1024))
    {
        LOGD(L_DRV, L_V1, "MCU�������С����24K��return -2,%d",len);
        return -2;
    }
    
    OTA_kal_wsprintf(mcu_new_sw_file_name, "%s", MCU_NEW_SW_FILE_NAME);

    FS_Delete(mcu_new_sw_file_name);
    file_hd = FS_Open(mcu_new_sw_file_name , FS_CREATE | FS_READ_WRITE);
    if(file_hd  < FS_NO_ERROR)
    {
        FS_Close(file_hd);
        return -7;
    }
    fs_seek = FS_Seek(file_hd, 0, FS_FILE_BEGIN);
    if(fs_seek < FS_NO_ERROR)
    {
        FS_Close(file_hd);
        return -8;
    }

    fs_write = FS_Write(file_hd, (void *)data, len , &file_operate_size);
    if(fs_write < FS_NO_ERROR)
    {
        FS_Close(file_hd);
        return -6;
    }
    FS_Commit(file_hd);
    FS_Close(file_hd);
    LOGD(L_DRV, L_V1, "MCU������浽�ļ�C:/chengj.hex");
    return 0;
}


/*****************************************************************************
 *  Global Functions			ȫ�ֺ���
 *****************************************************************************/

/******************************************************************************
 *  Function    -  track_drv_get_mcu_sw_update_status
 *
 *  Purpose     -  MUC�����ܵ���һ����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-09-06,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
mcu_sw_update_status_enum track_drv_get_mcu_sw_update_status()
{
    return mcu_sw_update_status;
}

/******************************************************************************
 *  Function    -  track_is_in_update_bootload_cmd
 *
 *  Purpose     -  ����ģʽ�´��ڻظ��Ƚ����⣬������������ԭ��������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-09-06,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_bool track_is_in_update_bootload_cmd(void)
{
    if(track_drv_get_mcu_sw_update_status() >= UPDATE_STATUE_ENABLE_RSP)
    {
        return KAL_TRUE;
    }
    else
    {
        return KAL_FALSE;
    }
}


/******************************************************************************
 *  Function    -  track_cust_module_mcu_fireware_file
 *
 *  Purpose     -  ���ص������׼������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-09-29,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_cust_module_mcu_fireware_file(U8 *data, U32 len)
{
    if((data == NULL) || (len < 1))
    {
        LOGD(L_DRV, L_V1, "���ص�MCU������������쳣");
    }

    if(mcu_new_sw_buf != NULL)
    {
        Ram_Free(mcu_new_sw_buf);
    }
    mcu_new_sw_buf = data;
    mcu_new_sw_size = len;

    LOGD(L_DRV, L_V1, "���ص�MCU�������׼������ size=%d ", len);
    if(track_new_sw_write(data, len) < 0)
        {
            if(track_is_timer_run(TRACK_MCU_SW_UPDATE_DOWNLOAD_TMER_ID))
                {
                    tr_stop_timer(TRACK_MCU_SW_UPDATE_DOWNLOAD_TMER_ID);
                }
            return;
            }
    
    tr_start_timer(TRACK_MCU_SW_UPDATE_DOWNLOAD_TMER_ID, 2000, track_mcu_sw_update_start_req);

    //�ر�MCUι���źŶ�ʱ�� ����ŵ�Ƭ������bin�ļ�
   
    if(track_is_timer_run(TRACK_CUST_EXTCHIP_BATTERY_VOLTAGE_TIMER_ID))
    {
        track_stop_timer(TRACK_CUST_EXTCHIP_BATTERY_VOLTAGE_TIMER_ID);
    }
    track_drv_sleep_enable(SLEEP_DRV_MOD, KAL_FALSE);
}

/******************************************************************************
 *  Function    -  track_mcu_sw_update_start_req
 *
 *  Purpose     -  ��  AT^GT_CM=99#
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-09-06,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
//static
void track_mcu_sw_update_start_req(void)
{
#if defined (__JM81__)
	kal_uint8 data[7] = {0x43,0x4A,0x07,0xF0,0x15,0x0D,0x0A};
#else
    kal_uint8 data[] = {"AT^GT_CM=99#\r\n"};	
#endif /* __JM81__ */

	
    static kal_uint8 cnt = 0;

    track_drv_set_mcu_sw_update_status(UPDATE_STATUE_ENABLE_REQ);

#if defined (__JM81__)
//�����CJ�ĵ�Ƭ�����
    track_drv_uart_write_data_to_mcu(data, 7);
#else
//������YDF�ĵ�Ƭ�����
	track_drv_uart_write_data_to_mcu(data, strlen(data));
#endif
	
    if(cnt < 20)
    {
        cnt++;
        tr_start_timer(TRACK_MCU_SW_UPDATE_UART_RSP_TIMEOUT, 2000, track_mcu_sw_update_start_req);
    }
    else
    {
        cnt = 0;
        LOGD(L_DRV, L_V1, "��Ƭ����������");

        track_drv_set_mcu_sw_update_status(UODATE_NO_NEED);
        track_mcu_sw_update_finish();
    }
}


void track_drv_mcu_sw_update_rsp(void)
{
    if(track_is_timer_run(TRACK_MCU_SW_UPDATE_UART_RSP_TIMEOUT))
    {
        tr_stop_timer(TRACK_MCU_SW_UPDATE_UART_RSP_TIMEOUT);
    }
    track_drv_set_mcu_sw_update_status(UPDATE_STATUE_ENABLE_RSP);

#if defined (__JM81__)
	LOGD(L_DRV, L_V1, "�յ� 0xF0 OK");
#else
    LOGD(L_DRV, L_V1, "�յ� 99 OK");
#endif /* __JM81__ */

    tr_start_timer(TRACK_MCU_SW_UPDATE_UART_RSP_TIMEOUT, 1000, track_mcu_bootloader_mode_req);
}

/******************************************************************************
 *  Function    -  track_mcu_bootloader_mode_req
 *
 *  Purpose     -  ��Ƭ����ȥ����ģʽ���������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-09-06,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_mcu_bootloader_mode_req(void)
{
    kal_uint8 data[] = {"UPGRADE"};//no "\r\n"
    static kal_uint8 cnt = 0;

    track_drv_set_mcu_sw_update_status(UPDATE_BOOTLOADER_MODE_REQ);


    track_drv_uart_write_data_to_mcu(data, strlen(data));
    if(cnt < 5)
    {
        cnt++;
        tr_start_timer(TRACK_MCU_SW_UPDATE_UART_RSP_TIMEOUT, 400, track_mcu_bootloader_mode_req);
    }
    else
    {
        cnt = 0;
        LOGD(L_DRV, L_V1, "��Ƭ���޷�������������ģʽ");

        track_drv_set_mcu_sw_update_status(UODATE_NO_NEED);
        track_mcu_sw_update_finish();
    }
}

/******************************************************************************
 *  Function    -  track_mcu_bootloader_mode_rsp
 *
 *  Purpose     -  �յ�CC���Կ�ʼ���ػ���BB��ʾ��Ƭ����ҪҪ�����-->��ʼYmodem
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-09-06,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_mcu_bootloader_mode_rsp(void)
{
    if(track_is_timer_run(TRACK_MCU_SW_UPDATE_UART_RSP_TIMEOUT))
    {
        tr_stop_timer(TRACK_MCU_SW_UPDATE_UART_RSP_TIMEOUT);
    }
    track_drv_set_mcu_sw_update_status(UPDATE_BOOTLOADER_MODE_RSP);
    LOGD(L_DRV, L_V1, "��Ƭ��������������ģʽ ��ʼYmode����");
    track_mcu_sw_ymodem_begin();
}

static void track_ymodem_filenme_packet(U8 *data, const U8* fileName, U32 length)
{
    U16 i, j, tempCRC;
    U8 file_ptr[10] = {0};

    /* Make first three packet */
    data[0] = SOH;
    data[1] = 0x00;
    data[2] = 0xff;

    /* Filename packet has valid data */
    for(i = 0; (fileName[i] != '\0') && (i < FILE_NAME_LENGTH); i++)
    {
        data[i + PACKET_HEADER] = fileName[i];
    }

    data[i + PACKET_HEADER] = 0x00;
    sprintf(file_ptr, "%d", length);

    for(j = 0, i = i + PACKET_HEADER + 1; file_ptr[j] != '\0' ;)
    {
        data[i++] = file_ptr[j++];
    }

    for(j = i; j < PACKET_SIZE + PACKET_HEADER; j++)
    {
        data[j] = 0;
    }

    /* Send CRC or Check Sum based on CRC16_F */
    tempCRC = track_fun_Get_CRC16_CCITT(&data[3], PACKET_SIZE);
    data[PACKET_SIZE + PACKET_HEADER] = (U8)(tempCRC >> 8);
    data[PACKET_SIZE + PACKET_HEADER + 1] = (U8) tempCRC & 0xFF;
}


/******************************************************************************
 *  Function    -  track_mcu_sw_update_finish
 *
 *  Purpose     -  �˳�����ģʽ
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-09-29,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_mcu_sw_update_finish(void)
{
    if(track_drv_get_mcu_sw_update_status() != UODATE_NO_NEED)
    {
        track_drv_set_mcu_sw_update_status(UODATE_NO_NEED);
    }

    if(mcu_new_sw_buf != NULL)
    {
        Ram_Free(mcu_new_sw_buf);
    }

    track_drv_sleep_enable(SLEEP_DRV_MOD, KAL_TRUE);
    LOGD(L_DRV, L_V1, "MCU��������");
}

/******************************************************************************
 *  Function    -  track_force_quit_uodate_status
 *
 *  Purpose     -  ��ʱ10�����˳�����ģʽ
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-09-08,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_force_quit_uodate_status(void)
{
    if(track_drv_get_mcu_sw_update_status() != UODATE_NO_NEED)
    {
        track_drv_set_mcu_sw_update_status(UODATE_NO_NEED);
    }
    if(track_is_timer_run(TRACK_MCU_SW_UPDATE_DOWNLOAD_TMER_ID))
    {
        tr_stop_timer(TRACK_MCU_SW_UPDATE_DOWNLOAD_TMER_ID);
    }
    if(track_is_timer_run(TRACK_MCU_SW_UPDATE_UART_RSP_TIMEOUT))
    {
        tr_stop_timer(TRACK_MCU_SW_UPDATE_UART_RSP_TIMEOUT);
    }

    track_mcu_sw_update_finish();
}

static void track_mcu_sw_ymodem_begin(void)
{
    U8 packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
    U8 FileName[FILE_NAME_LENGTH] = {MCU_NEW_SW_FILE_NAME};
    U32 sizeFile;
    static kal_uint8 cnt = 0;

    sizeFile = mcu_new_sw_size;
    /* Prepare first block */
    track_ymodem_filenme_packet(&packet_data[0], FileName, sizeFile);
    /* Send Packet */
    track_drv_uart_write_data_to_mcu(packet_data,  PACKET_SIZE + PACKET_HEADER + 2);

    pktNo = 0;
    if(cnt < 5)
    {
        cnt++;
        tr_start_timer(TRACK_MCU_SW_UPDATE_UART_RSP_TIMEOUT, 10000, track_mcu_sw_ymodem_begin);
    }
    else
    {
        cnt = 0;
        LOGD(L_DRV, L_V1, "����Ymodem�ļ�����С��ͷ�޻ظ�");

        track_drv_set_mcu_sw_update_status(UODATE_NO_NEED);
        track_mcu_sw_update_finish();
    }
}

static void track_mcu_new_sw_download_start()
{
    if(track_is_timer_run(TRACK_MCU_SW_UPDATE_UART_RSP_TIMEOUT))
    {
        tr_stop_timer(TRACK_MCU_SW_UPDATE_UART_RSP_TIMEOUT);
    }
    track_drv_set_mcu_sw_update_status(UPDATE_YMODE_DOWNLOAD);
    LOGD(L_DRV, L_V1, "��Ƭ��Ymodeͷȷ���յ�����ʼ���������");

    track_mcu_new_sw_download_bin();
}


static void track_mcu_new_sw_download_bin()
{
    pktNo = 0;

    tr_start_timer(TRACK_MCU_SW_UPDATE_UART_RSP_TIMEOUT, 10 * 60 * 1000, track_force_quit_uodate_status);
    track_send_cmd_update_data((void*)1);
}

static void track_ymode_end_zero_data(void)
{
    U8 packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
    U16 i, size, packetSize, tempCRC;
    packet_data[0] = SOH;
    packet_data[1] = 0;
    packet_data [2] = 0xFF;
    LOGD(L_DRV, L_V1, " ȫ0���ݰ����ͣ���������");
    for(i = PACKET_HEADER; i < (PACKET_SIZE + PACKET_HEADER); i++)
    {
        packet_data [i] = 0x00;
    }
    tempCRC = track_fun_Get_CRC16_CCITT(&packet_data[3], PACKET_SIZE);
    packet_data[PACKET_SIZE + 3] = (U8)(tempCRC >> 8);
    packet_data[PACKET_SIZE + 3 + 1] = (U8)tempCRC & 0xFF;
    track_drv_uart_write_data_to_mcu(packet_data, PACKET_SIZE + PACKET_OVERHEAD);
}

static void track_mcu_sw_ymodem_packet(U8 *SourceBuf, U8 *data, U8 pktNo, U32 sizeBlk)
{
    U16 i, size, packetSize, tempCRC;
    U8* file_ptr;

    LOGD(L_DRV, L_V5, "pktNo:%d, sizeBlk:%d", pktNo, sizeBlk);
    /* Make first three packet */
    packetSize = sizeBlk >= PACKET_SIZE ? PACKET_1K_SIZE : PACKET_SIZE;
    size = sizeBlk < packetSize ? sizeBlk : packetSize;
    if(packetSize == PACKET_1K_SIZE)
    {
        data[0] = STX;
    }
    else
    {
        data[0] = SOH;
    }
    data[1] = pktNo;
    data[2] = (~pktNo);
    file_ptr = SourceBuf;

    /* Filename packet has valid data */
    for(i = PACKET_HEADER; i < size + PACKET_HEADER; i++)
    {
        data[i] = *file_ptr++;
    }
    if(size  <= packetSize)
    {
        for(i = size + PACKET_HEADER; i < packetSize + PACKET_HEADER; i++)
        {
            data[i] = 0x1A; /* EOF (0x1A) or 0x00 */
        }
    }

    tempCRC = track_fun_Get_CRC16_CCITT(&data[3], packetSize);
    data[PACKET_HEADER + packetSize] = (U8)(tempCRC >> 8);
    data[PACKET_HEADER + packetSize + 1] = (U8)tempCRC & 0xFF;
    track_drv_uart_write_data_to_mcu(data, PACKET_OVERHEAD + packetSize);
}


//9600�Ĳ�����̫�����ط�ʱ��15��
static void track_send_cmd_update_data(void *arg)
{
    U8 packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
    int len = mcu_new_sw_size;
    int m = len / PACKET_1K_SIZE;

    if((len >> 10) > 0)
    {
        m = m + 1;
    }

    SourceBuf = (U8*)mcu_new_sw_buf;
    if(SourceBuf == NULL)
    {
        LOGD(L_DRV, L_V1, "û�����������ʲô");
    }
    if((U32)arg == 0)   //�ط�
    {
        LOGD(L_DRV, L_V1, "�ط� ��%d/%d��, %d%%", pktNo, m, pktNo * 100 / m);
        track_mcu_sw_ymodem_packet((SourceBuf + (pktNo - 1)*PACKET_1K_SIZE), packet_data, pktNo, PACKET_1K_SIZE);
        track_start_timer(TRACK_MCU_SW_UPDATE_DOWNLOAD_TMER_ID, 15000, track_send_cmd_update_data, (void*)0);
    }
    else if(pktNo == 0xffff)
    {
        track_drv_set_mcu_sw_update_status(UPDATE_NEW_SW_FINISH);
        track_ymode_end_zero_data();
        pktNo = 0;
        track_start_timer(TRACK_MCU_SW_UPDATE_DOWNLOAD_TMER_ID, 15000, track_send_cmd_update_data, (void*)0);
    }
    else  if((U32)arg == 1)
    {
        pktNo++;
        LOGD(L_DRV, L_V1, "size=%d, ��%d/%d��, %d%%", len, pktNo, m, pktNo * 100 / m);
        if(len >= pktNo * PACKET_1K_SIZE)
        {
            track_mcu_sw_ymodem_packet((SourceBuf + (pktNo - 1)*PACKET_1K_SIZE), packet_data, pktNo, PACKET_1K_SIZE);
            track_start_timer(TRACK_MCU_SW_UPDATE_DOWNLOAD_TMER_ID, 15000, track_send_cmd_update_data, (void*)0);
            LOGD(L_DRV, L_V1, " ��%d�ַ���ʼ", (pktNo - 1)*PACKET_1K_SIZE);
        }
        else if(len >= (pktNo - 1)*PACKET_1K_SIZE)
        {
            LOGD(L_DRV, L_V1, "���һ��%d,%d,%d", len, pktNo, len - (pktNo - 1)*PACKET_1K_SIZE);
            track_mcu_sw_ymodem_packet((SourceBuf + (pktNo - 1)*PACKET_1K_SIZE), packet_data, pktNo, len - (pktNo - 1)*PACKET_1K_SIZE);
            track_start_timer(TRACK_MCU_SW_UPDATE_DOWNLOAD_TMER_ID, 15000, track_send_cmd_update_data, (void*)0);
        }
        else
        {
            track_drv_set_mcu_sw_update_status(UPDATE_YMODE_EOT_END);
            packet_data[0] = EOT;
            pktNo = 0xffff;
            LOGD(L_DRV, L_V1, "EOT", len, pktNo + 1);

            track_drv_uart_write_data_to_mcu(packet_data, 1);
            track_start_timer(TRACK_MCU_SW_UPDATE_DOWNLOAD_TMER_ID, 15000, track_send_cmd_update_data, (void*)0);
        }
    }
}


void track_mcu_update_req()
{
    kal_int8 ret = track_new_sw_read();
    if(ret < 0)
    {
        LOGD(L_DRV, L_V1, "���ػ����������쳣 %d", ret);
        return;
    }

    LOGD(L_DRV, L_V1, "���ػ��������� size=%d", mcu_new_sw_size);

    track_mcu_bootloader_mode_rsp();
}

/******************************************************************************
 *  Function    -  track_drv_urat_mcu_updata_ymode_decode
 *
 *  Purpose     -  ���ڽ���
 *
 *  Description -  Ymode����״̬���л�
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-09-06,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_urat_mcu_updata_ymode_decode(kal_uint8 *data, int len)
{
    mcu_sw_update_status_enum pre_update_status = track_drv_get_mcu_sw_update_status();

    if(strstr(data, "POW ON RST") != NULL)
    {
        LOGS("��Ƭ������������");
        track_force_quit_uodate_status();
        return;
    }

    switch(pre_update_status)
    {
        case UPDATE_BOOTLOADER_MODE_REQ:
            if(track_str_char(data, MODEM_C, len))
            {
                track_mcu_bootloader_mode_rsp();
            }
            break;

        case UPDATE_BOOTLOADER_MODE_RSP:
            if(track_str_char(data, MODEM_ACK, len))
            {
                track_mcu_new_sw_download_start();
            }
            break;

        case UPDATE_YMODE_DOWNLOAD:
            LOGD(L_DRV, L_V1, "�յ�NEXT %02X", *data);

            if(track_str_char(data, MODEM_ACK, len))
            {
                track_send_cmd_update_data((void*)1);//next
            }
            else if(track_str_char(data, MODEM_NAK, len))
            {
                track_force_quit_uodate_status();//error
            }
            break;

        case UPDATE_YMODE_EOT_END:
            LOGD(L_DRV, L_V1, "EOT���� %d", *data);

            /*
            //�������Ǳ�׼YmodemЭ�飬ʵ��ϸ�ڲ�ͬ
                        if(track_str_char(data, MODEM_EOT, len))
                        {
                            if(track_is_timer_run(TRACK_MCU_SW_UPDATE_DOWNLOAD_TMER_ID))
                            {
                                tr_stop_timer(TRACK_MCU_SW_UPDATE_DOWNLOAD_TMER_ID);
                            }
                        }

                        if(track_str_char(data, MODEM_C, len))
                        {
                            track_send_cmd_update_data((void*)1);//EOT����
                        }
            */

            if(track_str_char(data, MODEM_ACK, len))
            {
                if(track_is_timer_run(TRACK_MCU_SW_UPDATE_DOWNLOAD_TMER_ID))
                {
                    tr_stop_timer(TRACK_MCU_SW_UPDATE_DOWNLOAD_TMER_ID);
                }
                //track_send_cmd_update_data((void*)1);//EOT����
                track_force_quit_uodate_status();//��Ƭ��û����׼���̣�Ҫ��ǰ����
            }

            break;


        case UPDATE_NEW_SW_FINISH:
            if(track_str_char(data, MODEM_ACK, len))
            {
                //��Ƭ�����ȷ���������
                track_force_quit_uodate_status();
            }
            break;

        default:
            if(strstr(data, "BB"))
            {
                //��Ƭ������Ҫ���������˳�״̬����ֱ������
                LOGD(L_DRV, L_V1, "��Ƭ������Ҫ���������˳�״̬����ֱ������");
                track_mcu_update_req();
                return;
            }
            LOGD(L_DRV, L_V1, "�յ� %s û����", data);
            break;
    }

}

#endif 

