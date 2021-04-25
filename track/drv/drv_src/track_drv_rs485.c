/******************************************************************************
 * track_drv_rs485.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        �豸UART3תRS485��������ͨ�� ��˫��
 *
 * modification history
 * --------------------
 * v1.0   2017-02-06,  chengjun create this file
 *
 ******************************************************************************/
#if defined(__CUST_RS485__)
/****************************************************************************
* Include Files                         ����ͷ�ļ�
*****************************************************************************/
#include "track_drv_rs485.h"
#include "track_os_log.h"
#include "track_os_ell.h"
#include "track_drv_system_param.h"

/*****************************************************************************
* Typedef  Enum                         ö�ٶ���
*****************************************************************************/

/*****************************************************************************
 *  Struct                              ���ݽṹ����
 *****************************************************************************/


/*****************************************************************************
 *  Define					�궨��
 *****************************************************************************/
#define RS485_SEND_ENABLE_GPIO  29

#define RS485_PACKET_HEAD 0xAA


/*****************************************************************************
* Local variable                        �ֲ�����
*****************************************************************************/
static kal_uint8 force_send_to_server = 0; //1=������ת����������
static kal_uint8 rs485_uart3_or_gpio_mode = 0; //0=unknow;1=uart;2=gpio

/****************************************************************************
* Global Variable                       ȫ�ֱ���
*****************************************************************************/


/****************************************************************************
* Global Variable - Extern              ����ȫ�ֱ���
*****************************************************************************/


/*****************************************************************************
 *  Global Functions    - Extern        �����ⲿ����
 *****************************************************************************/
extern void track_drv_write_exmode(kal_uint8 *data, int len);

/*****************************************************************************
 *  Local Functions                     ���غ���
 *****************************************************************************/
void track_rs485_read_vehicle_failure_req(void);
void track_rs485_read_vehicle_status_req(void);
void track_rs485_vehicle_failure_decode(kal_uint8* data, kal_uint16 len);
static kal_int8 track_rs485_data_decode(kal_uint8* data, kal_uint16 length, kal_uint16 *addr);
void track_rs485_send_device_data_to_server(kal_uint8* data, kal_uint16 len);

static void  delay_1ms(kal_uint16 ms)
{
    volatile kal_uint16 i;
    while(ms--)
    {
        for(i = 0; i < 2048; i++)
        {
            ;
        }
    }
}

static unsigned short track_get_crc(kal_uint8 *buff, unsigned short lenth)
{
    unsigned short crc = 0xFFFF;
    unsigned short i, j;

    j = 0;
    while(j < lenth)
    {
        crc ^= buff[j];
        for(i = 0; i < 8; i++)
        {
            if(crc & 0x01)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
        j++;
    }

    return crc;
}


/*****************************************************************************
 *  Global Functions			ȫ�ֺ���
 *****************************************************************************/
void track_drv_rs485_gpio_init(void)
{
    //����RS485Ϊ����ģʽ
    GPIO_ModeSetup(RS485_SEND_ENABLE_GPIO, 0);
    GPIO_InitIO(1, RS485_SEND_ENABLE_GPIO);
    GPIO_WriteIO(0, RS485_SEND_ENABLE_GPIO);

    tr_start_timer(TRACK_CUST_READ_VEHICLE_STATUS_TIMER_ID, 120 * 1000, track_rs485_read_vehicle_failure_req);
}

/******************************************************************************
 *  Function    -  track_set_rs485_to_uart3_or_gpio_mode
 *
 *  Purpose     -  �ж��봮�ڸ��ã���ѯGPIO��ǰ����ģʽ
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-02-09,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint8 track_set_rs485_to_uart3_or_gpio_mode(kal_uint8 mode)
{
    if(mode == 1)   //uart3
    {
        rs485_uart3_or_gpio_mode = 1;                
        GPIO_ModeSetup(0, 3);//gpio0-->uart3Rx
        GPIO_ModeSetup(1, 3);//gpio1-->uart3Tx        
    }
    else if(mode == 2)  //gpio
    {
        rs485_uart3_or_gpio_mode = 2;
        GPIO_ModeSetup(0, 0);
        GPIO_ModeSetup(1, 0);
        
        //GPIO_InitIO(0, 0);
        //GPIO_InitIO(0, 1);
        GPIO_InitIO(1, 0);
        GPIO_InitIO(1, 1);
        GPIO_WriteIO(0, 0); 
        GPIO_WriteIO(0, 1); 

		
    }
    return rs485_uart3_or_gpio_mode;
}


/******************************************************************************
 *  Function    -  track_drv_rs485_sleep_enable
 *
 *  Purpose     -  �Ӻ��������� 
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-02-09,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_rs485_sleep_enable(void)
{
    LOGD(L_DRV, L_V5, "RS485 sleep");
    track_drv_sleep_enable(SLEEP_EXT_UART_MOD, KAL_TRUE);
    track_set_rs485_to_uart3_or_gpio_mode(2);
}

/******************************************************************************
 *  Function    -  track_drv_rs485_write
 *
 *  Purpose     -  RS485д����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-02-06,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_rs485_write(kal_uint8 *data, int len)
{
    //�����Լ���ͬʱ��������ʹ��
    track_drv_sleep_enable(SLEEP_EXT_UART_MOD, KAL_FALSE);
    tr_start_timer(TRACK_DRV_RS485_RX_WAKEUP_TIMER_ID, 5 * 1000, track_drv_rs485_sleep_enable);

    if(track_set_rs485_to_uart3_or_gpio_mode(0xFF) != 1)
    {
        track_set_rs485_to_uart3_or_gpio_mode(1);
    }

    GPIO_WriteIO(1, RS485_SEND_ENABLE_GPIO);
    delay_1ms(2);

    track_drv_write_exmode(data, len);
    delay_1ms(len + 10);
    GPIO_WriteIO(0, RS485_SEND_ENABLE_GPIO);
}


void track_drv_rs485_rec_data(kal_uint8* data, kal_uint16 len)
{
    kal_int8 ret;
    kal_uint16 addr;

    if(data == NULL)
    {
        return;
    }

    //Ӳ�����ڹ̶���ë��
    if((len==1)&&((data[0]&0xF0)==0xF0))
    {
        LOGD(L_DRV, L_V4, "RS485����0xF*�������쳣");
        return;
    }

    //5��δ�յ��������������ߣ�����ж�
    if(track_is_timer_run(TRACK_DRV_RS485_RX_WAKEUP_TIMER_ID))
    {
        tr_stop_timer(TRACK_DRV_RS485_RX_WAKEUP_TIMER_ID);
    }
    tr_start_timer(TRACK_DRV_RS485_RX_WAKEUP_TIMER_ID, 5 * 1000, track_drv_rs485_sleep_enable);


    LOGD(L_DRV, L_V4, "RS485���� %d", len);
    LOGH(L_DRV, L_V4, data, len);

    if(track_is_testmode())
    {
        if(strstr(data, "RS485"))
        {
            if(track_is_timer_run(TRACK_RS485_HW_TEST_TIMER_ID))
            {
                LOGS("%s","RS485 OK");
                track_stop_timer(TRACK_RS485_HW_TEST_TIMER_ID);
            }
            return;
        }
    }
    
    ret = track_rs485_data_decode(data, len, &addr);
    if(ret < 0)
    {
        LOGD(L_DRV, L_V4, "ret=%d", ret);
        return;
    }

    if(track_is_testmode())
    {
		if(track_is_timer_run(TRACK_RS485_HW_TEST_TIMER_ID))
		{
			LOGS("%s","RS485 OK");
			track_stop_timer(TRACK_RS485_HW_TEST_TIMER_ID);
		}
    }

    if(force_send_to_server == 0)
    {
        if(addr == 0x0090)
        {
            track_rs485_vehicle_failure_decode(data, len);
        }
        else
        {
            track_rs485_send_device_data_to_server(data, len);
        }

    }
    else
    {
        track_rs485_send_device_data_to_server(data, len);
    }
    force_send_to_server = 0;
}


/******************************************************************************
 *  Function    -  track_rs485_data_decode
 *
 *  Purpose     -  ��������ƴ���ֽ�
 *
 *  Description -  ����ƴ��
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-02-20,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static kal_int8 track_rs485_data_decode(kal_uint8* data, kal_uint16 length, kal_uint16 *addr)
{
    kal_uint16 i, cmd, len, k;
    kal_uint16 crc1, crc2;

    if((data == NULL) || (length < 15))
    {
        return -1;
    }

    for(i = 0; i < length; i++)
    {
        if(data[i] == RS485_PACKET_HEAD)
        {
            break;
        }
    }

    if(i >= length)
    {
        LOGD(L_DRV, L_V4, "no 0xAA %d", i);
        return -2;
    }
    cmd = data[i+17];
    *addr = ((data[i+18] << 8) | data[i+19]);
    len = data[i+20];
    k = len + i + 21;
    LOGD(L_DRV, L_V4, "k %d,length %d", k, len);
    if((k + 2) > length)
    {
        return -3;
    }

    crc1 = data[k] + (data[k+1] << 8);
    crc2 = track_get_crc(&data[i], len + 21);
    if(crc1 != crc2)
    {
        LOGD(L_DRV, L_V4, "crc error 0x%02X  0x%02X", crc1, crc2);
        return -4;
    }
    LOGD(L_DRV, L_V4, "RS485 cmd=0x%02X addr=0x%04X", cmd, *addr);
    return 0;
}

/******************************************************************************
 *  Function    -  track_rs485_read_failure_status
 *
 *  Purpose     -  ��ʱ���������ϣ�ȫ0��ת��������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-03-06,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_rs485_read_vehicle_failure_req(void)
{
    kal_uint8 sendBuff[24] = {0};
    kal_uint16 crc;

    sendBuff[0] = RS485_PACKET_HEAD;
    // 1-16 �豸IDȫ0
    sendBuff[17] = 0x02; //������
    sendBuff[18] = 0x00;
    sendBuff[19] = 0x90;
    sendBuff[20] = 0x01;
    sendBuff[21] = 0x10;
    crc = track_get_crc(sendBuff, 22);
    sendBuff[22] = crc & 0xFF;
    sendBuff[23] = (crc >> 8) & 0xFF;
    track_drv_rs485_write(sendBuff, 24);
    tr_start_timer(TRACK_CUST_READ_VEHICLE_STATUS_TIMER_ID, 182 * 1000, track_rs485_read_vehicle_failure_req);
}

/******************************************************************************
 *  Function    -  track_rs485_vehicle_failure_decode
 *
 *  Purpose     -  ��ʱ���������ϣ�ȫ0��ת��������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-03-06,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_rs485_vehicle_failure_decode(kal_uint8* data, kal_uint16 len)
{
    kal_uint16 i;

    for(i = 0; i < 16; i++)
    {
        if(data[i+20] != 0)
        {
            track_rs485_send_device_data_to_server(data, len);
            break;
        }
    }

    tr_start_timer(TRACK_CUST_READ_VEHICLE_STATUS_TIMER_ID, 2 * 1000, track_rs485_read_vehicle_status_req);
}

/******************************************************************************
 *  Function    -  track_rs485_read_vehicle_status
 *
 *  Purpose     -  ��ʱ������״̬������ظ���������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-02-20,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_rs485_read_vehicle_status_req(void)
{
    kal_uint8 sendBuff[24] = {0};
    kal_uint16 crc;

    sendBuff[0] = RS485_PACKET_HEAD;
    // 1-16 �豸IDȫ0
    sendBuff[17] = 0x02; //������
    sendBuff[18] = 0x00;
    sendBuff[19] = 0xB0;
    sendBuff[20] = 0x01;
    sendBuff[21] = 0x30;
    crc = track_get_crc(sendBuff, 22);
    sendBuff[22] = crc & 0xFF;
    sendBuff[23] = (crc >> 8) & 0xFF;
    track_drv_rs485_write(sendBuff, 24);

    if(!track_is_timer_run(TRACK_CUST_READ_VEHICLE_STATUS_TIMER_ID))
    {
        tr_start_timer(TRACK_CUST_READ_VEHICLE_STATUS_TIMER_ID, 180 * 1000, track_rs485_read_vehicle_status_req);
    }
}


void track_rs485_write_server_data_to_device(kal_uint8* data, kal_uint16 len)
{
    LOGD(L_DRV, L_V4, "len=%d", len);
    LOGH(L_DRV, L_V4, data, len);
    force_send_to_server = 1;
    track_drv_rs485_write(data, len);
}

void track_rs485_send_device_data_to_server(kal_uint8* data, kal_uint16 len)
{
    LOGD(L_DRV, L_V4, "len=%d", len);
    LOGH(L_DRV, L_V4, data, len);
    track_cust_paket_9C(data, len, 1);
}

#endif  /*__CUST_RS485__*/


