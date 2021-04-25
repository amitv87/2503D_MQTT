/******************************************************************************
 * track_drv_uart.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        输出调试信息
 *
 * modification history
 * --------------------
 * v1.0   2013-04-17,  wangqi create this file
 *
 ******************************************************************************/
/****************************************************************************
* Include Files             包含头文件
*****************************************************************************/
#include "track_os_timer.h"
#include "track_at.h"
#include "track_drv_uart.h"
#include "Track_drv_eint_gpio.h"

#include "gpio_hw.h"
#include "dcl_gpio.h"
#include "track_os_ell.h"
#include "track_cust.h"

/*****************************************************************************
 *  Define			    宏定义
*****************************************************************************/
//2、 UART初始化示例代码：

#if defined(__OBD__)
#define OBD_UART_PORT               uart_port2
#else
#define OBD_UART_PORT               uart_port1
#endif

#if defined(__GT741__) ||defined(__GT420D__)
#define EXT_UART_PORT               uart_port2
#elif defined(__GT530__) || defined (__NT33__) || defined (__GT370__)|| defined (__GT380__) || defined (__GT740__)||defined (__ET350__)||defined (__JM81__)||defined (__JM66__)
#define EXT_UART_PORT               uart_port3
#elif defined (__MT200__) && defined (__CUST_ACC_BY_MCU__)
#define EXT_UART_PORT               uart_port3
#elif defined(__GT02F__)
#define EXT_UART_PORT               uart_port2
#else
#define EXT_UART_PORT               uart_port1
#endif

//#if defined __GPS_MT3333__
//#define GPS_COMM_OWNER_ID           MOD_GPS
//#else
#define GPS_COMM_OWNER_ID           MOD_MMI
//#endif
#if defined(__BD_AT6558__)
#define GPS_POWER_GPIO              3
#else
#define GPS_POWER_GPIO              18
#endif /* __BD_AT6558__ */

#if defined( __GPS_G3333__) //||defined( __BD_AT6558__)
#define GPS_UART_BAUD               UART_BAUD_115200
#else
#define GPS_UART_BAUD               UART_BAUD_9600
#endif /* __GPS_G3333__ */

#if defined(__GT741__) ||defined(__GT420D__)
#define UART2_BAUD                  UART_BAUD_9600
#elif defined(__GT740__)
#define UART2_BAUD                  UART_BAUD_115200
#else
#define UART2_BAUD                  UART_BAUD_115200
#endif /* __GT740__ */

#if defined(__GT740__)||defined (__GT370__) || defined (__GT380__)|| defined (__ET350__)||defined (__JM81__)
#define UART3_BAUD                  UART_BAUD_9600
#elif defined (__MT200__) && defined (__CUST_ACC_BY_MCU__)
#define UART3_BAUD                  UART_BAUD_9600
#elif defined (__NT33__)||defined (__JM66__)
#define UART3_BAUD                  UART_BAUD_115200
#else
#define UART3_BAUD                  UART_BAUD_115200
#endif /* __GT740__ */

#define AT_COMM_OWNER_ID            MOD_ATCI
#define OBD_COMM_OWNER_ID           MOD_MMI

#define GPS_REVICE_BUFF_SIZE        1600
#define GPS_REVICE_BUFF_LIMIT       GPS_REVICE_BUFF_SIZE - 31

#define SERAIL_CACHE_BUF_SIZE          5120
#define SERAIL_BACK_BUF_SIZE          2048
#define GPS_SINGLE_ITME_MAX_SIZE    250
#define GPS_DECODE_BUF_SIZE         (GPS_REVICE_BUFF_SIZE + GPS_SINGLE_ITME_MAX_SIZE * 2)

/*****************************************************************************
* Typedef  Enum         枚举
*****************************************************************************/


/*****************************************************************************
 *  Struct			    数据结构定义
*****************************************************************************/


/*****************************************************************************
* Local variable            局部变量
*****************************************************************************/
static kal_uint8 uart_buf[GPS_DECODE_BUF_SIZE] = {0};
static kal_uint8 uart_log_mode = 0;  /*0: 初始值，待转OBD专用; 1: 已经转为OBD专用;  2: 禁止转为OBD专用*/

#if defined (__GT530__)
static const char gpson[] = {0x77, 0x77, 0x00, 0x0C, 0x91, 0x01, 0x01, 0x47, 0x50, 0x53, 0x4F, 0x4E, 0x00, 0x02, 0xAB, 0x00, 0x0D, 0x0A};
static const char gpsoff[] = {0x77, 0x77, 0x00, 0x0D, 0x91, 0x01, 0x01, 0x47, 0x50, 0x53, 0x4F, 0x46, 0x46, 0x00, 0x03, 0x41, 0xDA, 0x0D, 0x0A};
static const char gpsTC[] = {0x77, 0x77, 0x00, 0x0C, 0x91, 0x01, 0x01, 0x47, 0x50, 0x53, 0x54, 0x43, 0x00, 0x06, 0x11, 0xEF, 0x0D, 0x0A};
static const char wifion[] = {0x77, 0x77, 0x00, 0x0D, 0x91, 0x01, 0x01, 0x57, 0x49, 0x46, 0x49, 0x4F, 0x4E, 0x00, 0x06, 0xEB, 0xF1, 0x0D, 0x0A};
static const char wifioff[] = {0x77, 0x77, 0x00, 0x0E, 0x91, 0x01, 0x01, 0x57, 0x49, 0x46, 0x49, 0x4F, 0x46, 0x46, 0x00, 0x07, 0x0D, 0x7A, 0x0D, 0x0A};
static const char wifitest[] = {0x77, 0x77, 0x00, 0x0F, 0x91, 0x01, 0x01, 0x54, 0x45, 0x53, 0x54, 0x57, 0x49, 0x46, 0x49,
                                0x00, 0x09, 0x72, 0x2E, 0x0D, 0x0A
                               };
static const char gpsTcOff[] = {0x77, 0x77, 0x00, 0x0E, 0x91, 0x01, 0x01, 0x47, 0x50, 0x53, 0x54, 0x43, 0x2C, 0x30, 0x00, 0x09, 0xBA, 0x95, 0x0D, 0x0A};
#endif /* __GT530__ */

kal_uint8 seridata[2];
kal_uint8 serverapp = 0;

track_soc_Callback_struct cb = {0};
char Imsi_s[16] = {0};
char Imei_s[16] = {0};
/****************************************************************************
* Global Variable - Extern          引用全局变量
*****************************************************************************/
#if defined(__SAVING_CODE_SPACE__)
#else

void track_drv_wifi_switch(kal_uint8 sw);
void track_cust_lte_packet92_decode(kal_uint8 *pdata, kal_uint8 datalen);
void track_cust_lte_net_status_recv(kal_uint8 *pdata, kal_uint8 packet_len, kal_uint8 sign);
void track_cust_lte_server_data_recv(kal_uint8 *pdata, kal_uint8 packet_len);
extern void track_soc_conn_status(kal_bool st);
extern void gt530_test_gps(void* arg);
extern void track_cust_lte_recv_data(kal_uint8 *data, int len);
extern void track_lte_net_event(track_soc_Callback_struct *par);
extern void track_set_login_status(kal_bool sta);
extern void track_cust_server2_conn(void *arg);
extern void track_lte_event2_recv(track_soc_Callback_struct *par);
extern void track_cust_lte_recv_data2(kal_uint8 *data, int len);
extern void track_drv_lbs_data_decode(kal_uint8 *pdata, kal_uint8 datalen);
extern void track_drv_set_lte_lbs_data(LBS_Multi_Cell_Data_Struct lbs);
extern void socket_send_status(kal_uint8 app, track_soc_status_enum mode, kal_int8 errorid, kal_uint8 queue_id);

extern void track_read_imsi_data(void);
extern void track_read_imei_data(void);
extern void track_cust_gps_check(void);
extern void track_cust_status_sim_set(kal_int8 value);
extern kal_int8 track_drv_sleep_enable(sleep_mod_enum mod , BOOL enable);

#endif


/*****************************************************************************
 *  Global Functions	- Extern	    引用外部函数
*****************************************************************************/

#if defined(__GT740__)||defined(__GT420D__) || defined(__GT370__)|| defined (__GT380__)|| defined (__JM81__)|| defined (__JM66__)
extern void track_cust_modeule_into_sleep_mode();
#endif

#if defined(__GT370__)|| defined (__GT380__)|| defined (__JM81__)|| defined (__JM66__)
void track_drv_eint_wakeup_mcu(void);
#endif

#if defined(__OIL_DETECTION__)
extern void track_cust_oil_main(void);
extern kal_uint8 track_drv_uart1_mode(kal_uint8 mode);
#endif


extern kal_bool track_recv_cmd_status(kal_uint8 par);

static void UART_ClrRxBuffer(UART_PORT port, module_type ownerid);
static void UART_ClrTxBuffer(UART_PORT port, module_type ownerid);


static DCL_HANDLE tr_at_get_handle(void)
{
    static DCL_HANDLE handle = -1;

    if(handle == -1)
    {
        handle = DclSerialPort_Open(track_get_at_port(), 0);  //MagicNing  最好在这里检查一下UART的所有者是不是ATCI
    }
    return handle;
}

#if 0
void track_uart_SetOwnerID(UART_PORT port, module_type ownerid)
{

    DCL_HANDLE handle;
    UART_CTRL_OWNER_T data;
    data.u4OwenrId = ownerid;

    handle = DclSerialPort_Open(port, 0);
    DclSerialPort_Control(handle, SIO_CMD_SET_OWNER, (DCL_CTRL_DATA_T*)&data);

}

module_type track_uart_GetOwnerID(UART_PORT port)
{

    DCL_HANDLE handle;
    UART_CTRL_OWNER_T data;

    handle = DclSerialPort_Open(port, 0);
    DclSerialPort_Control(handle, SIO_CMD_GET_OWNER_ID, (DCL_CTRL_DATA_T*) &data);
    return data.u4OwenrId;
}
void track_uart_SetDCBConfig(UART_PORT port, UARTDCBStruct *UART_Config, module_type ownerid)
{

    DCL_HANDLE handle;
    UART_CTRL_DCB_T data;
    DCL_STATUS dcl_ret = 0;
    /* config the UART */
    data.u4OwenrId = ownerid;
    data.rUARTConfig.u4Baud = UART_Config->baud;
    data.rUARTConfig.u1DataBits = UART_Config->dataBits;
    data.rUARTConfig.u1StopBits = UART_Config->stopBits;
    data.rUARTConfig.u1Parity = UART_Config->parity;
    data.rUARTConfig.u1FlowControl = UART_Config->flowControl;
    data.rUARTConfig.ucXonChar = UART_Config->xonChar;
    data.rUARTConfig.ucXoffChar = UART_Config->xoffChar;
    data.rUARTConfig.fgDSRCheck = UART_Config->DSRCheck;

    /* open the UART port, then setup the config information. */
    handle = DclSerialPort_Open(port, 0);
    dcl_ret = DclSerialPort_Control(handle, SIO_CMD_SET_DCB_CONFIG, (DCL_CTRL_DATA_T*) &data);
    LOGD(L_DRV, L_V2, "track_uart_SetDCBConfig:%d,MOD_MMI:%d,%d", dcl_ret, MOD_MMI, track_uart_GetOwnerID(port));

}

void track_uart_Bootup_Init(DCL_DEV port)
{
    DCL_HANDLE handle;
    DCL_STATUS dcl_ret = 0;
    handle = DclSerialPort_Open(port, 0);
    dcl_ret = DclSerialPort_Control(handle, UART_CMD_BOOTUP_INIT, NULL);
    LOGD(L_DRV, L_V2, "track_uart_Bootup_Init:%d", dcl_ret);

}
#endif /* 0 */

void track_uart_Close(DCL_DEV port, module_type owerid)
{
    DCL_HANDLE handle;
    UART_CTRL_CLOSE_T data;
    UART_CTRL_POWERON_T data2;
    DCL_STATUS dcl_ret1 = 0, dcl_ret = 0;

    UART_ClrRxBuffer(port, owerid);
    UART_ClrTxBuffer(port, owerid);

    data.u4OwenrId = owerid;
    handle = DclSerialPort_Open(port, 0);
    dcl_ret1 = DclSerialPort_Control(handle, SIO_CMD_CLOSE, (DCL_CTRL_DATA_T*)&data);

    data2.bFlag_Poweron = DCL_FALSE;
    handle = DclSerialPort_Open(port, 0);
    dcl_ret = DclSerialPort_Control(handle, UART_CMD_POWER_ON, (DCL_CTRL_DATA_T*)&data2);
    LOGD(L_DRV, L_V2, "track_uart_Close ret:%d,%d", dcl_ret1, dcl_ret);

}

#if 0
void track_uart_init_port(UART_PORT port, UART_baudrate uart_baud, module_type ownerid)
{
    UARTDCBStruct dcb = {0};

    dcb.baud = uart_baud;
    dcb.dataBits = len_8;
    dcb.stopBits = sb_1;			/*dataBits*/
    dcb.parity = pa_none;/*stopBits*/
    dcb.flowControl = fc_none;		/*parity*/
    dcb.xonChar = 0x11; 			/*no flow control*/
    dcb.xoffChar = 0x13;			/*xonChar*/
    dcb.DSRCheck = KAL_FALSE;			/*xoffChar*/

    LOGD(L_CMD, L_V5, "<--");

    track_uart_Bootup_Init(port);
    ownerid = track_uart_GetOwnerID(port);
    //LOGD(L_DRV, L_V2, "track_uart_init_port()ownerid:%d,MOD_MMI=%d", ownerid,MOD_MMI);
    track_uart_SetOwnerID(port, ownerid);
    track_uart_SetDCBConfig(port, &dcb, ownerid);
}

void track_uart_write(kal_uint16 device, void * out_buf, kal_uint16 o_len)
{
    DCL_HANDLE handle;
    UART_CTRL_PUT_BYTES_T data;
    DCL_STATUS dcl_ret = 0;
    kal_uint16 dev_port = device;

    data.u4OwenrId = MOD_MMI;
    data.u2Length = o_len;
    data.puBuffaddr = (kal_uint8 *)out_buf;

    handle = DclSerialPort_Open(dev_port, 0);
//   dcl_ret = DclSerialPort_Control(handle, SIO_CMD_RMMI_UART_WRITE, (DCL_CTRL_DATA_T*)&data);
    //result_count = data.u2RetSize;
}

void track_uart_read(void * port_ptr_port)
{
    DCL_UINT16 to_port = 0 ;
    DCL_HANDLE handle;
    UART_CTRL_GET_BYTES_T data;
    kal_uint8 status;
    kal_uint8 buf[128] = {0};
    DCL_STATUS dcl_ret = 0;

    to_port = (DCL_UINT16)port_ptr_port;
    memset(buf, 0, sizeof(buf));
    data.u4OwenrId = MOD_MMI;
    data.u2Length = 128;
    data.puBuffaddr = &buf[0];
    data.pustatus = &status;
    handle = DclSerialPort_Open(to_port, 0);
//    dcl_ret = DclSerialPort_Control(handle, SIO_CMD_RMMI_UART_READ, (DCL_CTRL_DATA_T*)&data);

    LOGD(L_DRV, L_V2, "MMI%d,%d@_@ %d  at: %s ", to_port, dcl_ret , data.u2RetSize, data.puBuffaddr);

//如果是自定义AT就直接处理,如果是标准AT就直接下发
//增加指令头  GT_AT=  加标准AT指令直接下发走模拟AT发送，供测试模拟功能

}

void track_uart_Boot_PutUARTBytes(UART_PORT port, kal_uint8 *data, kal_uint16 len)
{
    DCL_HANDLE handle;
    UART_CTRL_BOOT_PUTBYTES_T data1;
    data1.puBuffaddr = data;
    data1.u2Length = len;
    handle = DclSerialPort_Open(port, 0);
    DclSerialPort_Control(handle, UART_CMD_BOOT_PUTBYTES, (DCL_CTRL_DATA_T *) &data1);
}

kal_uint16 track_uart_GetBytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, kal_uint8 *status, module_type ownerid)
{
    DCL_HANDLE handle;
    UART_CTRL_GET_BYTES_T data;

    data.u4OwenrId = ownerid;
    data.u2Length = Length;
    data.puBuffaddr = Buffaddr;
    data.pustatus = status;
    handle = DclSerialPort_Open(port, 0);
    DclSerialPort_Control(handle, SIO_CMD_GET_BYTES, (DCL_CTRL_DATA_T*)&data);
    LOGD(L_DRV, L_V2, "GetBytes(%s,%d)", Buffaddr, data.u2RetSize);
    return data.u2RetSize;

}
#endif

static DCL_UINT16 UART_CheckTxAllSentOut(DCL_DEV port)
{
    DCL_HANDLE handle;
    UART_CTRL_CHECK_TSO_T data;

    handle = DclSerialPort_Open(port, 0);
    DclSerialPort_Control(handle, UART_CMD_CHECK_TX_SEND_OUT, (DCL_CTRL_DATA_T*)&data);
    DclSerialPort_Close(handle);
    return data.bFlag;
}

static void UART_ClrRxBuffer(UART_PORT port, module_type ownerid)
{
    DCL_HANDLE handle;
    UART_CTRL_CLR_BUFFER_T data;
    data.u4OwenrId = ownerid;
    handle = DclSerialPort_Open(port, 0);
    DclSerialPort_Control(handle, SIO_CMD_CLR_RX_BUF, (DCL_CTRL_DATA_T*)&data);
}

static void UART_ClrTxBuffer(UART_PORT port, module_type ownerid)
{
    DCL_HANDLE handle;
    UART_CTRL_CLR_BUFFER_T data;
    data.u4OwenrId = ownerid;
    handle = DclSerialPort_Open(port, 0);
    DclSerialPort_Control(handle, SIO_CMD_CLR_TX_BUF, (DCL_CTRL_DATA_T*)&data);
}
DCL_UINT16 UART_GetBytesAvail(DCL_DEV port)
{
    DCL_HANDLE handle;
    UART_CTRL_RX_AVAIL_T data;

    handle = DclSerialPort_Open(port, 0);
    DclSerialPort_Control(handle, SIO_CMD_GET_RX_AVAIL, (DCL_CTRL_DATA_T*)&data);
    DclSerialPort_Close(handle);
    return data.u2RetSize;
}

kal_uint16 UART_PutBytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, module_type ownerid)
{
    DCL_HANDLE handle;
    UART_CTRL_PUT_BYTES_T data;
//UART_CTRL_PUT_UART_BYTE_T data;
    DCL_STATUS ret;

    data.u4OwenrId = ownerid;
    data.u2Length = Length;
    data.puBuffaddr = Buffaddr;
    handle = DclSerialPort_Open(port, 0);
    ret = DclSerialPort_Control(handle, SIO_CMD_PUT_BYTES, (DCL_CTRL_DATA_T*)&data);
    return data.u2RetSize;
}

static kal_uint16 UART_GetBytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, kal_uint8 *status, module_type ownerid)
{
    DCL_HANDLE handle;
    UART_CTRL_GET_BYTES_T data;
    data.u4OwenrId = ownerid;
    data.u2Length = Length;
    data.puBuffaddr = Buffaddr;
    data.pustatus = status;
    handle = DclSerialPort_Open(port, 0);
    DclSerialPort_Control(handle, SIO_CMD_GET_BYTES, (DCL_CTRL_DATA_T*)&data);
    return data.u2RetSize;
}

static void UART_TurnOnPower(UART_PORT port, kal_bool on)
{
    DCL_HANDLE handle;
    UART_CTRL_POWERON_T data;

    handle = DclSerialPort_Open(port, 0);
    data.bFlag_Poweron = on;
    DclSerialPort_Control(handle, UART_CMD_POWER_ON, (DCL_CTRL_DATA_T*)&data);
    DclSerialPort_Close(handle);
}

static void UART_SetDCBConfig(DCL_DEV port, UART_CONFIG_T *UART_Config, DCL_UINT32 ownerid)
{
    DCL_HANDLE handle;
    UART_CTRL_DCB_T data;
    DCL_STATUS dcl_ret = 0;

    data.u4OwenrId = ownerid;
    data.rUARTConfig.u4Baud = UART_Config->u4Baud;
    data.rUARTConfig.u1DataBits = UART_Config->u1DataBits;
    data.rUARTConfig.u1StopBits = UART_Config->u1StopBits;
    data.rUARTConfig.u1Parity = UART_Config->u1Parity;
    data.rUARTConfig.u1FlowControl = UART_Config->u1FlowControl;
    data.rUARTConfig.ucXonChar = UART_Config->ucXonChar;
    data.rUARTConfig.ucXoffChar = UART_Config->ucXoffChar;
    data.rUARTConfig.fgDSRCheck = UART_Config->fgDSRCheck;
    handle = DclSerialPort_Open(port, 0);
    dcl_ret = DclSerialPort_Control(handle, SIO_CMD_SET_DCB_CONFIG, (DCL_CTRL_DATA_T*)&data);
    DclSerialPort_Close(handle);
    LOGD(L_DRV, L_V5, "ret:%d;ownerid =%d;u4OwenrId=%d;u4Baud =%d;u4Baud=%d", dcl_ret, ownerid, data.u4OwenrId, data.rUARTConfig.u4Baud, UART_Config->u4Baud);
}
static void UART_ReadDCBConfig(DCL_DEV port)
{
    DCL_HANDLE handle;
    UART_CTRL_DCB_T data;
    handle = DclSerialPort_Open(port, 0);
    DclSerialPort_Control(handle, SIO_CMD_READ_DCB_CONFIG, (DCL_CTRL_DATA_T*)&data);
    LOGD(L_DRV, L_V7, "id=%d;u4Baud=%d", data.u4OwenrId, data.rUARTConfig.u4Baud);
    DclSerialPort_Close(handle);
}

//STATUS_OK = 0
static DCL_INT32 UART_Open(UART_PORT port, module_type ownerid)
{
    DCL_HANDLE handle;
    UART_CTRL_OPEN_T data;
    DCL_INT32  status;

    data.u4OwenrId = ownerid;
    handle = DclSerialPort_Open(port, 0);
    status = DclSerialPort_Control(handle, SIO_CMD_OPEN, (DCL_CTRL_DATA_T*)&data);
    LOGD(L_DRV, L_V5, "port:%d,handle:%d,Status=%d", port, handle, status);

    return status;
}

static module_type UART_GetOwnerID(UART_PORT port)
{
    DCL_HANDLE handle;
    UART_CTRL_OWNER_T data;
    DCL_STATUS ret;
    handle = DclSerialPort_Open(port, 0);
    ret = DclSerialPort_Control(handle, SIO_CMD_GET_OWNER_ID, (DCL_CTRL_DATA_T*)&data);
    return (module_type)(data.u4OwenrId);
}

static void UART_SetOwner(UART_PORT port, module_type ownerid)
{
    DCL_HANDLE handle;
    UART_CTRL_OWNER_T data;
    data.u4OwenrId = ownerid;
    handle = DclSerialPort_Open(port, 0);
    DclSerialPort_Control(handle, SIO_CMD_SET_OWNER, (DCL_CTRL_DATA_T*)&data);

}

kal_uint16 USB_PutBytes(kal_uint8 * out_context, kal_uint16 length)
{
    return UART_PutBytes(4, out_context, length, UART_GetOwnerID(4));
}

static void track_drv_gps_enable(kal_uint8 flag)
{
#if defined(__ATGM_UPDATE__)
    if(track_cust_atgm_update_status(STATUS_CHECK) >= ATGM_UPDATE_STATUS_update_begin_req)
    {
        LOGD(L_APP, L_V1, "升级中,不uart_init");
        return;
    }
#endif /*__ATGM_UPDATE__*/
    if(1 == flag)
    {
//#if !defined( __BD_AT6558__)
        GPIO_ModeSetup(GPS_POWER_GPIO, 0);
        GPIO_InitIO(1, GPS_POWER_GPIO);
        GPIO_WriteIO(1, GPS_POWER_GPIO);
//#endif /* __BD_AT6558__ */

        track_drv_Set_VMC(KAL_TRUE, 3);
    }
    else
    {
//#if !defined( __BD_AT6558__)
        GPIO_WriteIO(0, GPS_POWER_GPIO);
//#endif /* __BD_AT6558__ */
        track_drv_Set_VMC(KAL_FALSE, 0);
    }
}
static kal_uint8 drv_gps_status = 0;
void track_set_drv_gps_status(U8 op)
{
    drv_gps_status = op;
}
static void track_drv_mgps_uart_Iint(void)
{
    DCL_INT32 status;
    UART_CONFIG_T Dcb;
    module_type IR_Owner_save;
    static kal_uint32 index = 0;
#if defined(__ATGM_UPDATE__)
    if(track_cust_atgm_update_status(STATUS_CHECK) >= ATGM_UPDATE_STATUS_update_begin_req)
    {
        LOGD(L_APP, L_V1, "升级中,不uart_init");
        return;
    }
#endif /*__ATGM_UPDATE__*/
    tr_stop_timer(GPS_UART2_RESET_INIT_TIME_ID);
    //对UART TX RX对应的GPIO进行配置，
    //注意配置包括mode dir ，示例部分只配置了mode。这部分也可以在dct tool中设定
    GPIO_ModeSetup(U_TX, U_TX_M);
    GPIO_ModeSetup(U_RX, U_RX_M);
    //GPIO_ModeSetup(0, 3); //open uart3
    //GPIO_ModeSetup(1, 3);
    LOGD(L_DRV, L_V5, "PORT:%d", GPS_UART_PORT);
    //防止需要使用的uart port被其他模块所打开，
    //故使用如下函数将当前的owner id注册到该uart port
    IR_Owner_save = UART_GetOwnerID(GPS_UART_PORT);
    if(GPS_COMM_OWNER_ID != IR_Owner_save)
    {
        track_uart_Close(GPS_UART_PORT, IR_Owner_save);
    }
    //开启uart power
    UART_TurnOnPower(GPS_UART_PORT, KAL_TRUE);  //open uart
    UART_SetOwner(GPS_UART_PORT, GPS_COMM_OWNER_ID);

    status = UART_Open(GPS_UART_PORT, GPS_COMM_OWNER_ID);
    LOGD(L_DRV, L_V5, "index:%d,status:%d,返回值(!=0)时1秒后重新初始化", index, status);
    if(status != STATUS_OK)
    {
        if(index < 5)
        {
            tr_start_timer(GPS_UART2_RESET_INIT_TIME_ID, 1000, track_drv_mgps_uart_Iint);
            index++;
            return;
        }
        else
        {
            ASSERT(0);
        }
    }
    index = 0;
    //assert(status3  == KAL_TRUE);
    //如果UART OPEN成功返回KAL_TRUE,故此处检查uart是否open成功。
    Dcb.u4Baud = GPS_UART_BAUD;
    Dcb.u1DataBits = len_8;
    Dcb.u1StopBits = sb_1;			/*dataBits*/
    Dcb.u1Parity = pa_none;/*stopBits*/
    Dcb.u1FlowControl = fc_none;		/*parity*/
    Dcb.ucXonChar = 0x11; 			/*no flow control*/
    Dcb.ucXoffChar = 0x13;			/*xonChar*/
    Dcb.fgDSRCheck = KAL_FALSE;			/*xoffChar*/
    //  设定uart 波特率等信息，注意在调用此函数之前需要先初始化Dcb结构体
    UART_SetDCBConfig(GPS_UART_PORT, &Dcb, GPS_COMM_OWNER_ID);

    LOGD(L_APP, L_V5, "GPS供电开启");

    track_drv_gps_enable(1);

    //track_drv_led_vcama(1);
}

#if defined(__ATGM_UPDATE__)
void track_drv_mgps_uart_init_for_atgm(U32 BAUD)
{
    DCL_INT32 status;
    UART_CONFIG_T Dcb;
    module_type IR_Owner_save;
    static kal_uint32 index = 0;
    LOGD(L_APP, L_V1, "--设置主机波特率[%d]--", BAUD);
    tr_stop_timer(GPS_UART2_RESET_INIT_TIME_ID);
    //对UART TX RX对应的GPIO进行配置，
    //注意配置包括mode dir ，示例部分只配置了mode。这部分也可以在dct tool中设定
    GPIO_ModeSetup(U_TX, U_TX_M); 
    GPIO_ModeSetup(U_RX, U_RX_M);
    //GPIO_ModeSetup(0, 3); //open uart3
    //GPIO_ModeSetup(1, 3);
    LOGD(L_DRV, L_V5, "PORT:%d",GPS_UART_PORT);
    //防止需要使用的uart port被其他模块所打开，
    //故使用如下函数将当前的owner id注册到该uart port
    IR_Owner_save = UART_GetOwnerID(GPS_UART_PORT);
    if(GPS_COMM_OWNER_ID != IR_Owner_save)
    {
        track_uart_Close(GPS_UART_PORT, IR_Owner_save);
    }
    //开启uart power
    UART_TurnOnPower(GPS_UART_PORT, KAL_TRUE);  //open uart
    UART_SetOwner(GPS_UART_PORT, GPS_COMM_OWNER_ID);

    status = UART_Open(GPS_UART_PORT, GPS_COMM_OWNER_ID);
    LOGD(L_DRV, L_V5, "index:%d,status:%d,返回值非0时1秒后重新初始化", index, status);
    index = 0;
    //assert(status3  == KAL_TRUE);
    //如果UART OPEN成功返回KAL_TRUE,故此处检查uart是否open成功。
    Dcb.u4Baud = BAUD;
    Dcb.u1DataBits = len_8;
    Dcb.u1StopBits = sb_1;			/*dataBits*/
    Dcb.u1Parity = pa_none;/*stopBits*/
    Dcb.u1FlowControl = fc_none;		/*parity*/
    Dcb.ucXonChar = 0x11; 			/*no flow control*/
    Dcb.ucXoffChar = 0x13;			/*xonChar*/
    Dcb.fgDSRCheck = KAL_FALSE;			/*xoffChar*/
    //  设定uart 波特率等信息，注意在调用此函数之前需要先初始化Dcb结构体
    UART_SetDCBConfig(GPS_UART_PORT, &Dcb, GPS_COMM_OWNER_ID);

}
#endif /*__ATGM_UPDATE__*/


UART_BAUDRATE_T track_drv_set_uart1_Baud(int _type)
{
    static UART_BAUDRATE_T uart1Baud = UART_BAUD_115200;

    switch(_type)
    {
        case 1:
            uart1Baud = UART_BAUD_9600;
            break;
        case 2:
            uart1Baud = UART_BAUD_57600;
            break;
        case 3:
            uart1Baud = UART_BAUD_115200;
            break;
    }

    return uart1Baud;
}

void tarck_drv_uart2_init(void)
{
    DCL_INT32 status;
    UART_CONFIG_T Dcb;
    module_type IR_Owner_save;
    static kal_uint32 index = 0;

    tr_stop_timer(GPS_UART2_RESET_INIT_TIME_ID);
    //对UART TX RX对应的GPIO进行配置，
    //注意配置包括mode dir ，示例部分只配置了mode。这部分也可以在dct tool中设定
    GPIO_ModeSetup(12, 2); //61D open uart2
    GPIO_ModeSetup(17, 2);
    LOGD(L_DRV, L_V5, "");
    //防止需要使用的uart port被其他模块所打开，
    //故使用如下函数将当前的owner id注册到该uart port
    IR_Owner_save = UART_GetOwnerID(uart_port2);
    if(MOD_MMI != IR_Owner_save)
    {
        LOGD(L_DRV, L_V6, "close");
        track_uart_Close(uart_port2, IR_Owner_save);
    }
    //开启uart power
    UART_TurnOnPower(uart_port2, KAL_TRUE);  //open uart
    UART_SetOwner(uart_port2, MOD_MMI);


    //assert(status3  == KAL_TRUE);
    //如果UART OPEN成功返回KAL_TRUE,故此处检查uart是否open成功。
    Dcb.u4Baud = UART2_BAUD;
    Dcb.u1DataBits = len_8;
    Dcb.u1StopBits = sb_1;			/*dataBits*/
    Dcb.u1Parity = pa_none;/*stopBits*/
    Dcb.u1FlowControl = fc_none;		/*parity*/
    Dcb.ucXonChar = 0x11; 			/*no flow control*/
    Dcb.ucXoffChar = 0x13;			/*xonChar*/
    Dcb.fgDSRCheck = KAL_FALSE;			/*xoffChar*/
    //  设定uart 波特率等信息，注意在调用此函数之前需要先初始化Dcb结构体
    UART_SetDCBConfig(uart_port2, &Dcb, MOD_MMI);
    status = UART_Open(uart_port2, MOD_MMI);
    LOGD(L_DRV, L_V6, "index:%d,status:%d,返回值为0时1秒后重新初始化", index, status);
    /*if(status == KAL_FALSE)
    {
        if(index < 5)
        {
            //tr_start_timer(GPS_UART2_RESET_INIT_TIME_ID, 1000, tarck_drv_uart2_init);
            index++;
            return;
        }
        else
        {
            //ASSERT(0);
        }
    }
    index = 0;
    //assert(status3  == KAL_TRUE);
    //如果UART OPEN成功返回KAL_TRUE,故此处检查uart是否open成功。
    */
#if 0
    Dcb.u4Baud = UART_BAUD_115200;
    Dcb.u1DataBits = len_8;
    Dcb.u1StopBits = sb_1;			/*dataBits*/
    Dcb.u1Parity = pa_none;/*stopBits*/
    Dcb.u1FlowControl = fc_none;		/*parity*/
    Dcb.ucXonChar = 0x11; 			/*no flow control*/
    Dcb.ucXoffChar = 0x13;			/*xonChar*/
    Dcb.fgDSRCheck = KAL_FALSE;			/*xoffChar*/
    //  设定uart 波特率等信息，注意在调用此函数之前需要先初始化Dcb结构体
    UART_SetDCBConfig(uart_port2, &Dcb, MOD_MMI);

#endif
}
static void track_drv_uart3_Iint(void)
{
    DCL_INT32 status;
    UART_CONFIG_T Dcb;
    module_type IR_Owner_save;
    static kal_uint32 index = 0;

    tr_stop_timer(GPS_UART1_RESET_INIT_TIME_ID);
    //对UART TX RX对应的GPIO进行配置，
    //注意配置包括mode dir ，示例部分只配置了mode。这部分也可以在dct tool中设定
    GPIO_ModeSetup(0, 3); //open uart3
    GPIO_ModeSetup(1, 3);
    LOGD(L_DRV, L_V5, "");
    //防止需要使用的uart port被其他模块所打开，
    //故使用如下函数将当前的owner id注册到该uart port
    IR_Owner_save = UART_GetOwnerID(uart_port3);
    if(MOD_MMI != IR_Owner_save)
    {
        track_uart_Close(uart_port3, IR_Owner_save);
    }
    //开启uart power
    UART_TurnOnPower(uart_port3, KAL_TRUE);  //open uart
    UART_SetOwner(uart_port3, MOD_MMI);

    status = UART_Open(uart_port3, MOD_MMI);
    LOGD(L_DRV, L_V5, "index:%d,status:%d,返回值(!=0)时1秒后重新初始化", index, status);
    if(status != STATUS_OK)
    {
        if(index < 5)
        {
            tr_start_timer(GPS_UART1_RESET_INIT_TIME_ID, 1000, track_drv_uart3_Iint);
            index++;
            return;
        }
        else
        {
            //ASSERT(0);
        }
    }
    index = 0;
    //assert(status3  == KAL_TRUE);
    //如果UART OPEN成功返回KAL_TRUE,故此处检查uart是否open成功。
    Dcb.u4Baud = UART3_BAUD;
    Dcb.u1DataBits = len_8;
    Dcb.u1StopBits = sb_1;			/*dataBits*/
    Dcb.u1Parity = pa_none;/*stopBits*/
    Dcb.u1FlowControl = fc_none;		/*parity*/
    Dcb.ucXonChar = 0x11; 			/*no flow control*/
    Dcb.ucXoffChar = 0x13;			/*xonChar*/
    Dcb.fgDSRCheck = KAL_FALSE;			/*xoffChar*/
    //  设定uart 波特率等信息，注意在调用此函数之前需要先初始化Dcb结构体
    UART_SetDCBConfig(uart_port3, &Dcb, MOD_MMI);


}
void track_drv_uart_init_by_set(UART_PORT op_port, U8 baud_type, module_type Owner_set)
{
    UART_BAUDRATE_T uart1Baud;
    UART_CONFIG_T Dcb;
    DCL_INT32 status3;

    module_type IR_Owner_save;
    char buff[50] = {0};
    switch(baud_type)
    {
        case 1:
            uart1Baud = UART_BAUD_9600;
            break;
        case 2:
            uart1Baud = UART_BAUD_57600;
            break;
        case 3:
            uart1Baud = UART_BAUD_115200;
            break;
        default:
            uart1Baud = UART_BAUD_9600;
            break;
    }
    LOGD(L_DRV, L_V5, "====port:%d baud_type:%d module_type:%d====", op_port, uart1Baud, Owner_set);
    if(uart_port1 == op_port)
    {
        GPIO_ModeSetup(22, 1); //61D open uart1
        GPIO_ModeSetup(23, 1);
    }
    else if(uart_port2 == op_port)
    {
        GPIO_ModeSetup(12, 2); //61D open uart2
        GPIO_ModeSetup(17, 2);
    }
    else if(uart_port3 == op_port)
    {
        GPIO_ModeSetup(0, 3); //open uart3
        GPIO_ModeSetup(1, 3);
    }
    IR_Owner_save = UART_GetOwnerID(op_port);
    if(Owner_set != IR_Owner_save)
    {
        track_uart_Close(op_port, IR_Owner_save);
    }

    //开启uart power
    UART_TurnOnPower(op_port, KAL_TRUE);  //open uart
    IR_Owner_save = UART_GetOwnerID(op_port);
    UART_SetOwner(op_port, Owner_set);
    status3 = UART_Open(op_port, Owner_set);
    if(G_realtime_data.netLogControl == 512)
    {
        sprintf(buff, "port %d, baud %d, mould %d, status3 %d", op_port, uart1Baud, Owner_set, status3);
        cust_packet_log_data_ext2(8, "[PARAM]", (kal_uint8 *)buff, strlen((char *)buff));
    }
    Dcb.u4Baud = uart1Baud;
    Dcb.u1DataBits = len_8;
    Dcb.u1StopBits = sb_1;			/*dataBits*/
    Dcb.u1Parity = pa_none;         /*stopBits*/
    Dcb.u1FlowControl = fc_none;	/*parity*/
    Dcb.ucXonChar = 0x11; 			/*no flow control*/
    Dcb.ucXoffChar = 0x13;			/*xonChar*/
    Dcb.fgDSRCheck = KAL_FALSE;			/*xoffChar*/
    //  设定uart 波特率等信息，注意在调用此函数之前需要先初始化Dcb结构体
    UART_SetDCBConfig(op_port, &Dcb, Owner_set);


}

void track_drv_uart1_Init(void* owner)
{
    DCL_INT32 status3;
    UART_CONFIG_T Dcb;
    module_type IR_Owner_save, Owner_set;
    static kal_uint32 index = 0;
    int _owner = (int)owner;
    UART_PORT op_port = uart_port1;

    if(0 == _owner)
    {
#if defined(__OBD__) && defined(__GT530__)
        op_port = OBD_UART_PORT;
#endif
        LOGS("串口%d初始化为OBD用途", op_port);
        Owner_set = OBD_COMM_OWNER_ID;
#if !defined(__GT06F__)
        if(G_parameter.obd.sw != 1) return;
        track_drv_set_uart1_Baud(2);
#else
        track_drv_set_uart1_Baud(1);
#endif

    }
    else if(1 == _owner)
    {
        Owner_set = AT_COMM_OWNER_ID;
        track_drv_set_uart1_Baud(3);
    }
    else if(2 == _owner) //GPS OTA升级
    {
        LOGS("串口一初始化为GPS OTA升级用途");
        Owner_set = OBD_COMM_OWNER_ID;
#if defined(__GT740__)||defined(__GT420D__)
        op_port = EXT_UART_PORT;
#endif
        track_drv_set_uart1_Baud(3);
    }

#if 0
	else if (3 == _owner) //MCU
    {
        LOGS("串口一初始化为外设通信");
        Owner_set = MOD_MMI;
        track_drv_set_uart1_Baud(3);
    }
#endif
	
    LOGD(L_DRV, L_V5, "====index:%d owner:%d====", index, Owner_set);
    //tr_stop_timer(GPS_UART1_RESET_INIT_TIME_ID);
    //对UART TX RX对应的GPIO进行配置，
    //注意配置包括mode dir ，示例部分只配置了mode。这部分也可以在dct tool中设定
    if(uart_port1 == op_port)
    {
        GPIO_ModeSetup(22, 1); //
        GPIO_ModeSetup(23, 1);
    }
    else if(uart_port2 == op_port)
    {
        GPIO_ModeSetup(12, 2); //61D open uart2
        GPIO_ModeSetup(17, 2);
    }

    //防止需要使用的uart port被其他模块所打开，
    //故使用如下函数将当前的owner id注册到该uart port
    IR_Owner_save = UART_GetOwnerID(op_port);
    LOGD(L_DRV, L_V5, "IR_Owner_save:%d:%d MOD_ATCI:%d", IR_Owner_save, Owner_set, MOD_ATCI);
    if(Owner_set != IR_Owner_save)
    {
        track_uart_Close(op_port, IR_Owner_save);
    }
    //开启uart power
    UART_TurnOnPower(op_port, KAL_TRUE);  //open uart
    IR_Owner_save = UART_GetOwnerID(op_port);
    LOGD(L_DRV, L_V5, "IR_Owner_save:%d:%d", IR_Owner_save, Owner_set);
    UART_SetOwner(op_port, Owner_set);
    status3 =   UART_Open(op_port, Owner_set);
    LOGD(L_DRV, L_V5, "---status3:%d---", status3);
    if(status3 != STATUS_OK)
    {
        if(index < 10)
        {
            //track_start_timer(GPS_UART1_RESET_INIT_TIME_ID, 1000, track_drv_obd_uart_Init, owner);
            index++;
            //return;
        }
        else
        {
            //ASSERT(0);
        }
    }
    index = 0;
#if defined(__OBD2__)
    Dcb.u4Baud = UART_BAUD_38400;
#else
    Dcb.u4Baud = track_drv_set_uart1_Baud(0);
#endif /* __OBD2__ */
    LOGD(L_DRV, L_V2, "rart1 init ---Baud:%d---", Dcb.u4Baud);
    Dcb.u1DataBits = len_8;
    Dcb.u1StopBits = sb_1;			/*dataBits*/
    Dcb.u1Parity = pa_none;         /*stopBits*/
    Dcb.u1FlowControl = fc_none;	/*parity*/
    Dcb.ucXonChar = 0x11; 			/*no flow control*/
    Dcb.ucXoffChar = 0x13;			/*xonChar*/
    Dcb.fgDSRCheck = KAL_FALSE;			/*xoffChar*/
    //  设定uart 波特率等信息，注意在调用此函数之前需要先初始化Dcb结构体
    UART_SetDCBConfig(op_port, &Dcb, Owner_set);
}

static void track_drv_reset_gps_cb(void)
{
    track_drv_mgps_on();
}

static void track_drv_reset_gps(void)
{
#if defined(__ATGM_UPDATE__)
    if(track_cust_atgm_update_status(STATUS_CHECK) >= ATGM_UPDATE_STATUS_update_begin_req)
    {
        LOGD(L_APP, L_V1, "升级流程中,不reset_gps");
        return;
    }
#endif /*__ATGM_UPDATE__*/

    LOGD(L_DRV, L_V5, "gps_Ota:%d", track_get_gps_ota(0xFF));
    if(track_get_gps_ota(0xFF))
    {
        return;
    }
    tr_stop_timer(GPS_UART2_RESET_INIT_TIME_ID);
    track_drv_mgps_off();
    tr_start_timer(GPS_DATA_UPDATA_TIMER_ID, 1000, track_drv_reset_gps_cb);
}


void track_drv_mgps_off(void)
{
    module_type IR_Owner_save;
    #if defined(__ATGM_UPDATE__)
    if(track_cust_atgm_update_status(STATUS_CHECK) >= ATGM_UPDATE_STATUS_update_begin_req)
    {
        LOGD(L_APP, L_V1, "升级流程中,不mgps_off");
        return;
    }
    #endif /*__ATGM_UPDATE__*/

    LOGD(L_DRV, L_V5, "gps_Ota:%d", track_get_gps_ota(0xFF));
#if defined( __GT530__)
    //GPIO_WriteIO(0, GPS_POWER_GPIO);
    track_drv_Set_VMC(KAL_FALSE, 0);
    track_drv_lte_uart1_ctrl(1);
    track_drv_write_exmode((kal_uint8 *)gpsoff, sizeof(gpsoff));
    drv_gps_status = 0;
    track_drv_sleep_enable(SLEEP_DRV_MOD, TRUE); //test

#else
    if(track_get_gps_ota(0xFF))
    {
        return;
    }
    //GPIO_WriteIO(0, GPS_POWER_GPIO);
    // track_drv_Set_VMC(KAL_FALSE, 0);
    track_drv_gps_enable(0);
#if defined( __BD_AT6558__)
    track_drv_led_vcama(KAL_FALSE);
#endif /* __BD_AT6558__ */
    IR_Owner_save = UART_GetOwnerID(GPS_UART_PORT);
    LOGD(L_APP, L_V5, "关闭GPS IR_Owner_save=%d", IR_Owner_save);
    track_uart_Close(GPS_UART_PORT, IR_Owner_save);
    drv_gps_status = 0;
    tr_stop_timer(GPS_DATA_UPDATA_TIMER_ID);
#endif /* __GT530__ */

#if defined( __BD_AT6558__)
    GPIO_ModeSetup(U_TX, 0);
    GPIO_ModeSetup(U_RX, 0);
#endif /* __BD_AT6558__ */

}

void track_drv_mgps_on(void)
{
    LOGD(L_DRV, L_V5, "");
#if defined( __GT530__)
    track_drv_sleep_enable(SLEEP_DRV_MOD, FALSE); //test
    track_drv_Set_VMC(KAL_TRUE, 4);
    track_drv_lte_uart1_ctrl(1);
    track_drv_write_exmode((kal_uint8 *)gpson, sizeof(gpson));
    drv_gps_status = 1;
#else
    drv_gps_status = 1;

    tr_start_timer(GPS_DATA_UPDATA_TIMER_ID, 10000, track_drv_reset_gps);/*TEMPEEEEEWANGQI*/


#if defined( __GPS_G3333__)||defined( __ET130__) || defined(__NT23__)||defined( __BD_AT6558__)
    OTA_track_GPIO_SetClkOut(1, 0x82/*mode_f32k_ck*/);//输出32K CLK
#endif /* __BD300__ */
#if defined( __BD_AT6558__)
    //track_drv_led_vcama(KAL_TRUE);
    track_drv_set_vcama(KAL_TRUE, 2800000);

#endif /* __BD_AT6558__ */
    track_drv_mgps_uart_Iint();

#endif /* __GT530__ */
}
#if defined(__GT530__)

void track_drv_lte_gpstc(kal_uint8 sw)
{
    track_drv_lte_uart1_ctrl(1);
    if(0 == sw)
    {
        track_drv_write_exmode((kal_uint8 *)gpsTcOff, sizeof(gpsTcOff));
    }
    else
    {
        track_drv_write_exmode((kal_uint8 *)gpsTC, sizeof(gpsTC));
    }
    track_status_gpstc(sw);
}
#endif

void track_obd_init(void)
{
#if defined(__GT530__)
    GPIO_ModeSetup(OBD_POWER_GPIO, 0);
    GPIO_InitIO(1/*OUTPUT*/, OBD_POWER_GPIO);
    GPIO_WriteIO(0, OBD_POWER_GPIO);
#if defined( __NETWORK_LICENSE__)
    return;//test
#endif /* __NETWORK_LICENSE__ */

#else
    kal_bool ret;
    int len = 0;
    static port_setting_struct port_setting = {0};
    ret = Track_nvram_read(NVRAM_EF_PORT_SETTING_LID, 1, (void *)&port_setting, NVRAM_EF_PORT_SETTING_SIZE);
    if(ret)
    {
        if(port_setting.ps_port == 0 && track_recv_cmd_status(0))
        {
            LOGD(L_DRV, L_V5, "收到有效指令，禁止初始化串口1为OBD专用");
            
            #if defined (__NT37__)
            //go go go
            #else
                        return;
            #endif /* __NT37__ */
        }
    }
    if(track_is_testmode() || track_is_autotestmode())
    {
        LOGD(L_DRV, L_V5, "在测试模式下，禁止初始化串口1为OBD专用");
        return;
    }
    if(uart_log_mode == 2)
    {
        LOGD(L_DRV, L_V5, "禁止初始化串口1为OBD专用");
        return;
    }
    uart_log_mode = 1;

#endif
#if defined(__OIL_DETECTION__)
    track_log_switch(0);
    track_drv_uart1_mode(1);
    track_drv_uart_init_by_set(uart_port1, 3, MOD_MMI);//NT37油量传感器 UART1-115200
#elif defined(__OBD__)
    obd_poweron(0);
    LOGD(L_DRV, L_V5, "========");
    track_drv_uart1_Init((void*) 0);
    track_cust_obd_opendata();
    obd_poweron(21);
#endif
}
//#endif

kal_uint8 track_drv_uart_log_mode(kal_uint8 par)
{
    if(par != 99)
    {
        uart_log_mode = par;
    }
    return uart_log_mode;
}
extern void track_eint_B_registration(void);
/******************************************************************************
 *  Function    -  track_drv_uart1_mode
 *  Purpose     -  串口1工作模式
 *  Description -
 *                 请使用0XFF进行状态查询
 *  Return      -  1   SOS模式
 *                 0   串口模式
                    0xff 为默认模式
 * modification history
 * ----------------------------------------
 * v1.0  , 20141230  WangQi  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint8 track_drv_uart1_mode(kal_uint8 mode)
{
    static kal_uint8 u_m = 0xff;
#if defined(__UART1_SOS__) || defined(__OIL_DETECTION__)
    if(mode < 2)
    {
        u_m = mode;
    }
#endif
    return u_m;
}
void track_uart_close(void)
{
    module_type IR_Owner_save;
    //track_drv_uart_log_mode(2);
    track_log_switch(0);
    track_drv_uart1_mode(1);
    track_drv_uart1_Init((void*)2);
    LOGD(L_DRV, L_V5, "");
    IR_Owner_save = UART_GetOwnerID(uart_port1);
    track_uart_Close(uart_port1, IR_Owner_save);
    GPIO_ModeSetup(22, 3); //open uart1
    track_eint_B_registration();

}
#if defined(__UART1_SOS__)
void track_uart_close3(void)
{
    track_log_switch(0);
    track_drv_uart1_mode(1);
    GPIO_ModeSetup(11, 0);
    GPIO_InitIO(0, 11);
    GPIO_ModeSetup(10, 3);
    track_eint_B_registration();
}
#endif

extern void track_drv_acc_by_mcu_read(void);
/******************************************************************************
 *  Function    -  track_drv_uart_init
 *
 *  Purpose     -  UART初始化
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-08-30,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_uart_init(void)
{
    LOGD(L_DRV, L_V5, "");

#if defined( __GPS_G3333__)||defined( __ET130__)
    OTA_track_GPIO_SetClkOut(1, 0x82/*mode_f32k_ck*/);//输出32K
#endif

#if defined(__TEST_DEVICE__)
    if(track_rid_given())
    {
        track_drv_sleep_enable(SLEEP_OBD_MOD, FALSE);
    }
#endif /* __TEST_DEVICE__ */

#if defined(__GPS_UART__)
        #if !defined(__GT420D__)
        track_drv_mgps_on();
        #endif
#endif /* __GPS_UART__ */

#if defined(__GT530__)
    track_drv_uart3_Iint();

    if(G_parameter.obd.sw == 1)
    {
        track_obd_init();
    }
    track_drv_lte_uart1_ctrl(1);
    track_drv_Set_VMC(KAL_TRUE, 4); //test

#elif defined(__OIL_DETECTION__)
    if(G_parameter.oil.sw)
    {
        tr_start_timer(DEALY_INIT_OBD_TIMER_ID, 10000, track_obd_init);
    }
#elif defined(__G3333_OTA__)
    track_cust_gps_OTA_start();
#elif defined(__OBD__) && defined(__NT23__)
    if(G_parameter.obd.sw == 1)
    {
        /*if(track_os_uart1_at_mode())
            LOGD(L_DRV, L_V5, "Uart1 AT Mode, 不能切换成OBD使用");
        else*/
        tr_start_timer(DEALY_INIT_OBD_TIMER_ID, 10000, track_obd_init);
    }
#elif defined(__OBD__) && defined(__GT520__)
    LOGD(L_DRV, L_V5, "G_parameter.obd.sw=%d", G_parameter.obd.sw);
    if(G_parameter.obd.sw == 1)
    {
        if(track_os_uart1_at_mode())
            LOGD(L_DRV, L_V5, "Uart1 AT Mode, 不能切换成OBD使用");
        else
            track_obd_init();
    }
#elif defined(__UART1_SOS__)
    LOGD(L_APP, L_V5, "若串口无外部输入20S后转为SOS中断使用");
    tr_start_timer(DEALY_INIT_OBD_TIMER_ID, 20000, track_uart_close3);
#endif /* __OBD__ */

#if defined (__NT33__) && defined (__CUST_BT__)
     track_drv_uart3_Iint();     //蓝牙UART /波特率：115200 /数据位：8/停止位：1 /校验位：None
#endif

#if defined(__GT741__)||defined(__GT420D__)
	tarck_drv_uart2_init();
#elif defined (__GT370__)|| defined (__GT380__) || defined(__GT740__)|| defined(__JM81__)||defined (__JM66__)
    track_drv_uart3_Iint();     //单片机UART /波特率：UART3_BAUD /数据位：8/停止位：1 /校验位：None
#endif

#if defined (__ET350__) && defined (__CUST_RS485__)
     track_drv_uart3_Iint();     //RS485转外设 UART /波特率：9600 /数据位：8/停止位：1 /校验位：None
#endif

#if defined (__CUST_UART1_MUX__)
    track_drv_uart1_init_delay();
#endif /* __CUST_UART1_MUX__ */

#if defined (__MT200__) && defined (__CUST_ACC_BY_MCU__)
   track_drv_uart3_Iint();     //单片机UART /波特率：UART3_BAUD /数据位：8/停止位：1 /校验位：None
    tr_start_timer(TRACK_CUST_ACC_BY_MCU_TIMER_ID, 10*1000, track_drv_acc_by_mcu_read);//单片机串口初始化太慢
#endif /* __CUST_ACC_BY_MCU__ */

#if defined(__NT37__)
#if defined(__OIL_DETECTION__)
    tr_start_timer(TRACK_CUST_OIL_TIMER_ID, 15000, track_cust_oil_main);
#endif
#endif

}

void track_drv_check_sim(void)
{
#if defined( __GT530__)
    track_drv_encode_exmode(0x77, 0x0101, 0x91, "SIM", 3);
#endif /* __GT530__ */
}
void track_drv_check_imei(void)
{
    track_drv_encode_exmode(0x77, 0x0101, 0x91, "IMEI", 4);
}
static track_drv_ringcall_handle(kal_uint8 *pdata, int packet_len)
{
    l4c_number_struct number = {0};
    kal_uint8 nmLen = 0;
    nmLen = packet_len - 5 - 5;
    number.type = 0;
    number.length = 0;
    if(nmLen > 0)
    {
        memcpy(number.number, pdata + 8, nmLen);
    }
    LOGD(L_CMD, L_V5, "%d, type:%d,length:%d,number:%s", nmLen, number.type, number.length, number.number);
    track_l4c_sendmsg_call_ring_ind(&number, 0, 0);
}
/**
 case TelephonyManager.SIM_STATE_ABSENT :sb.append("无卡");break;
 case TelephonyManager.SIM_STATE_UNKNOWN :sb.append("未知状态");break;
 case TelephonyManager.SIM_STATE_NETWORK_LOCKED :sb.append("需要NetworkPIN解锁");break;
 case TelephonyManager.SIM_STATE_PIN_REQUIRED :sb.append("需要PIN解锁");break;
 case TelephonyManager.SIM_STATE_PUK_REQUIRED :sb.append("需要PUK解锁");break;
 case TelephonyManager.SIM_STATE_READY :sb.append("良好");break;
 **/
kal_uint8 ex_imei[16] = "358688000000158"; //868120108619649
void track_get_imsi_hex(kal_uint8* imsi_data)
{
    memcpy(imsi_data, &Imsi_s, 16);
}
void track_get_imei_hex(kal_uint8* imei_data)
{
    if(strlen(Imei_s) == 0)
    {
        memcpy(imei_data, &ex_imei, 16);
    }
    else
    {
        memcpy(imei_data, &Imei_s, 16);
    }
}
void track_set_imei_str(void)
{
    //if(Imei_s == 0)
    {
        memcpy(&Imei_s, &ex_imei, 16);
    }
}
static void track_cust_recv_serial_cmd(kal_uint8 cmd, kal_uint8 *pdata, int packet_len)
{
#if 0
    //此处可解析出内容传入AT指令处理接口wanqi
    kal_uint16 pLen = packet_len;
    int nmLen = 0;
    static kal_uint8 nmea[GPS_REVICE_BUFF_SIZE] = 0;
    char * pdu = NULL;
    char Imsi_H[16] = {0};
    if(cmd != 0x93)
    {
        LOGD(L_OS, L_V6, " cmd:%X, buflen:%d, SerialNumber:%.2X%.2X,[%c%c%c%c]",
             cmd, packet_len, pdata[packet_len - 2], pdata[packet_len - 1], *(pdata + 3), *(pdata + 4), *(pdata + 5), *(pdata + 6));
        LOGH(L_OS, L_V6, pdata, packet_len);
        LOGF(L_OS, L_V6, pdata, packet_len);
    }
    else
    {
        LOGD(L_OS, L_V7, " cmd:%X, buflen:%d, SerialNumber:%.2X%.2X,[%c%c%c%c]",
             cmd, packet_len, pdata[packet_len - 2], pdata[packet_len - 1], *(pdata + 3), *(pdata + 4), *(pdata + 5), *(pdata + 6));
        LOGH(L_OS, L_V7, pdata, packet_len);
        LOGF(L_OS, L_V7, pdata, packet_len);
    }

    // 指令长度
    if(track_is_testmode())
    {
        //LOGS(pdata + 3, nmLen-5);
    }

    if(cmd == 0x91)
    {
        //77 77 00 11 91 01 01 47 50 53 4F 4E 2C 50 41 53 53 00 02 15 67 0D 0A
        //91 01 01    47 50 53 4F 4E 2C 50 41 53 53    00 02
        if(track_cust_gps_status() == 0)   track_cust_pre_lte_sleep();
        if(*(pdata + 5) == 0x53 && *(pdata + 6) == 0x4f && *(pdata + 9) == 0x50 && *(pdata + 10) == 0x41)
        {
            //when we receive gpson,pass, and send gpstc on after 3s
            //if(track_is_testmode())
            {
                //gt530_test_gps((void*)2);
            }
            //else
            {
                gt530_test_gps((void*)4);
            }
        }
        else if(*(pdata + 5) == 0x53 && *(pdata + 6) == 0x54 && *(pdata + 9) == 0x50 && *(pdata + 10) == 0x41)
        {
            //whe we receive gpstc,pass, stop timer
            gt530_test_gps((void*)4);
        }// 91 01 01 53 49 4D 2C 2C 41 42 53 45 4E 54 00 00
        else if(*(pdata + 3) == 'S' && *(pdata + 4) == 'I' && *(pdata + 5) == 'M')
        {
            //77 77 00 10 91 01 01 41 54 44 2C 36 36 38 35 35 00 02 3D 9B 0D 0A
            *(pdata + pLen - 2) = 0;
            LOGS(pdata + 3);
            pdata += 7;
            if(strstr(pdata, "READY"))
            {
                memcpy(&Imsi_s, pdata, 15);
                LOGD(L_APP, L_V5, "imsi = %s;", Imsi_s);
                track_drv_set_sim_type(SIM_STATE_READY);
                track_read_imsi_data();
                track_cust_status_sim_set(1);
                track_drv_wifi_switch(1);
            }
            else if(strstr(pdata, "ABSENT"))
            {
                track_drv_set_sim_type(SIM_STATE_ABSENT);
            }
            else if(strstr(pdata, "UNKNOWN"))
            {
                track_drv_set_sim_type(SIM_STATE_UNKNOWN);
            }
            else if(strstr(pdata, "NETWORK_LOCKED"))
            {
                track_drv_set_sim_type(SIM_STATE_NETWORK_LOCKED);
            }
            else if(strstr(pdata, "PIN_REQUIRED"))
            {
                track_drv_set_sim_type(SIM_STATE_PIN_REQUIRED);
            }
            else if(strstr(pdata, "PUK_REQUIRED"))
            {
                track_drv_set_sim_type(SIM_STATE_PUK_REQUIRED);
            }
        }
        else if(*(pdata + 3) == 'W' && *(pdata + 4) == 'I' && *(pdata + 5) == 'F' && *(pdata + 6) == 'I' && *(pdata + 7) == 'O' && *(pdata + 8) == 'N')
        {
            if(strstr(pdata, "PASS"))
                LOGD(L_APP, L_V5, "Attention Please,AP mode success!");
        }
        else if(*(pdata + 3) == 'I' && *(pdata + 4) == 'M' && *(pdata + 5) == 'E' && *(pdata + 6) == 'I' && packet_len > 24)
        {
            //大约24是因为测到4g有只回复IMEI,PASS的情况
            pdata += 8;
            memcpy(&Imei_s, pdata, 15);
            LOGD(L_APP, L_V5, "imei = %s;", Imei_s);
            tr_stop_timer(TRACK_CUST_GET_IMEI_TIMER_ID);
            track_read_imei_data();
        }
        else if(*(pdata + 3) == 'G' && *(pdata + 4) == 'P' && *(pdata + 5) == 'S' && *(pdata + 6) == 'O' && *(pdata + 7) == 'F' && *(pdata + 8) == 'F')
        {
            LOGD(L_APP, L_V5, "gps 关闭SUCCESS");
            //if(strstr(pdata, "PASS"))  track_cust_pre_lte_sleep();
        }
        else if(*(pdata + 3) == 'L' && *(pdata + 4) == 'B' && *(pdata + 5) == 'S')
        {
            track_drv_lbs_data_decode(pdata + 7, packet_len - 9);
        }
        else if(*(pdata + 3) == 'S' && *(pdata + 4) == 'E' && *(pdata + 5) == 'R' && *(pdata + 6) == 'V' && *(pdata + 7) == 'E' && *(pdata + 8) == 'R')
        {
            track_cust_lte_server_data_recv(pdata, packet_len);
        }
        else if(*(pdata + 3) == 'A' && *(pdata + 4) == 'T')
        {
            //77 77 00 10 91 01 01 41 54 44 2C 36 36 38 35 35 00 02 3D 9B 0D 0A

            if(*(pdata + 5) == 'D')
            {
                track_l4c_cc_exe_call_setup_lrsp(0, 0, 0);
            }
            else if(*(pdata + 5) == 'A')
            {
                track_l4c_cc_exe_ata_lrsp(0, 0, 0);
            }

        }
        else if(*(pdata + 3) == 'H' && *(pdata + 4) == 'A' && *(pdata + 5) == 'N')
        {
            // 91 01 01 48 41 4E 47 55 50 2C 50 41 53 53 00 01
            //track_l4c_cc_exe_call_setup_lrsp(0, 0, 0);
            LOGS("HANGUP");
        }
// 91 01 01 57 49 46 49 43 4F 4E 4E 45 43 54 2C 57 57 57 2C 31 32 33 34 35 36 37 38 39 30 2C 50 41 53 53 00 01
        else if(*(pdata + 3) == 'W' && *(pdata + 4) == 'I' && *(pdata + 5) == 'F' && *(pdata + 6) == 'I')
        {
            /*WIFICONNECT,WWW,1234567890,PASS*/
            *(pdata + pLen - 2) = 0;
            LOGS(pdata + 3);
        }
        else if(*(pdata + 3) == 'T' && *(pdata + 4) == 'E' && *(pdata + 5) == 'S' && *(pdata + 6) == 'T')
        {
            //91 01 01 54 45 53 54 4D 49 43 2C 33 30 30 2C 31 30 2C 50 41 53 53 00 00
//91 01 01 54 45 53 54 57 49 46 49 2C 28 4A 43 47 2D 38 43 42 46 35 38 2C 2D 38 34 29 2C 28 4E 65 77 74 74 6C 2C 2D 38 35
//29 2C 28 4E 65 77 54 68 69 6E 6B 69 6E 67 2D 31 2C 2D 39 35 29 00 09
            *(pdata + pLen - 2) = 0;
            LOGS(pdata + 3);
#if 0
            if(*(pdata + 6) == 'W' && *(pdata + 7) == 'I' && *(pdata + 8) == 'F' && *(pdata + 9) == 'I')
            {
                LOGS(pdata + 6);
            }
            else if(*(pdata + 6) == 'M' && *(pdata + 7) == 'I' && *(pdata + 8) == 'C')
            {
                if(*(pdata + 10) == 'P' && *(pdata + 11) == 'A')
                {
                    LOGS("MIC Voice OK!");
                }
                else
                {
                    LOGS("MIC NOT Ready!");
                }
            }
#endif
        }
    }
    else if(cmd == 0x92)
    {
        //92 01 01 00 03 2C 50 41 53 53 00 06
        track_cust_lte_packet92_decode(pdata, packet_len);
    }
    else if(cmd == 0x93)
    {
        nmLen = pLen - 5;/*长度=协议号+信息内容+信息序列号+错误校验*/
        //LOGD(L_OS, L_V5,"%d", nmLen);
        //LOGH(L_OS, L_V5,pCm + 7, nmLen);
        memset(nmea, 0, GPS_REVICE_BUFF_SIZE);
        if(nmLen > 0 && nmLen < GPS_REVICE_BUFF_SIZE - 2)
        {
            //透传过来的星历里面没有0D0A
            memcpy(nmea, pdata + 3, nmLen);
            nmea[nmLen] = 0x0a;
            track_uart_decode(nmea, nmLen + 1);
        }
        track_cust_gps_check();
    }
    else if(cmd == 0x94)
    {
        // 94 01 01 53 59 53 54 45 4D 55 50 00 00

        if(*(pdata + 3) == 0x53 && *(pdata + 4) == 0x59 && *(pdata + 9) == 0x55 && *(pdata + 10) == 0x50)
        {
            //SYSTEM then starttime 3s ,send gpson
            LOGS("==4G SYSTEMUP 模块启动成功!==tesetmode:%d", track_is_testmode());

            track_fun_msg_send(MSG_ID_LTE_SYSYTEMUP_IND, NULL, MOD_MMI, MOD_MMI);
        }
        else  if(*(pdata + 3) == 'R' && *(pdata + 4) == 'I' && *(pdata + 5) == 'N' && *(pdata + 6) == 'G')
        {
            //RING////94 01 01 52 49 4E 47 2C 36 36 38 35 35 00 06
            track_drv_ringcall_handle(pdata, packet_len);
        }
        else if(*(pdata + 3) == 'A' && *(pdata + 4) == 'C' && *(pdata + 5) == 'C' && *(pdata + 6) == 'E')
        {
            track_l4c_cc_call_accept_req_ind(0);
        }
        else if(*(pdata + 3) == 'R' && *(pdata + 4) == 'E' && *(pdata + 5) == 'L' && *(pdata + 6) == 'E')
        {
            track_l4c_sendmsg_call_disc_release_lind(1, 0, 0, 0, 0);
        }
        else if(*(pdata + 3) == 'W' && *(pdata + 4) == 'I' && *(pdata + 5) == 'F' && *(pdata + 6) == 'I')
        {
            /*WIFICONNECT,WWW,1234567890,PASS*/
            *(pdata + pLen - 2) = 0;
            pdata += 3;
            if(strstr(pdata, "COMPLETED") || strstr(pdata, "ASSOCIATED") || strstr(pdata, "HANDSHAKE") || strstr(pdata, "DISCONNECTED"))
            {
                LOGS(pdata);
            }
            else
            {
                LOGD(L_OS, L_V5, pdata);
            }
        }
        else if(*(pdata + 3) == 'S' && *(pdata + 4) == 'M' && *(pdata + 5) == 'S' && *(pdata + 6) == 'M' && *(pdata + 7) == 'T')
        {

            //SMSMT,68,0891683108702505F0240D91685110689058F500005150039015212305C8329BFD06

            if(*(pdata + 10) == ',')
            {
                nmLen = *(pdata + 9) - '0';
                pdu = pdata + 11;
            }
            else if(*(pdata + 11) == ',')
            {
                nmLen = (*(pdata + 9) - '0') * 10 + (*(pdata + 10) - '0');
                pdu = pdata + 12;
            }
            else if(*(pdata + 12) == ',')
            {
                nmLen = (*(pdata + 9) - '0') * 100 + (*(pdata + 10) - '0') * 10 + (*(pdata + 11) - '0');
                pdu = pdata + 13;
            }
            else
            {
                track_drv_encode_exmode(0x77, 0x0101, 0x94, "SMSMT,FAIL", 10);
                return;
            }
            track_drv_encode_exmode(0x77, 0x0101, 0x94, "SMSMT,OK", 8);
            LOGD(L_OS, L_V5, "GET PDU %d", nmLen);
            LOGH(L_OS, L_V5, pdu, nmLen);
            track_sms_receive(nmLen, pdu);
        }
        else if(*(pdata + 3) == 'S' && *(pdata + 4) == 'E' && *(pdata + 5) == 'R' && *(pdata + 6) == 'V' && *(pdata + 7) == 'E' && *(pdata + 8) == 'R' && *(pdata + 9) == 'S' && *(pdata + 10) == 'T' && *(pdata + 11) == 'A')
        {
            serverapp = *(pdata + 16) - 48;
            LOGS("==LTE 0x94!%d", serverapp);
            if(serverapp == 0)
            {
                if(strstr(pdata, "CONNECTING"))
                {
                    cb.status = 7;
                    tr_stop_timer(TRACK_CUST_NET_LOGIN_RESEND_TIMER_ID);
                }
                else if(strstr(pdata, "CONNECTED"))
                {
                    cb.status = 2;
                    tr_stop_timer(TRACK_CUST_NET_CONN_TIMER_ID);
                    track_lte_net_event(&cb);
                }
                else
                {
                    tr_stop_timer(TRACK_CUST_NET_LOGIN_RESEND_TIMER_ID);
                }
                //track_lte_net_event(&cb);
            }
        }
    }
    else if(cmd == 0x95)
    {
        //发送短信回执

        track_sms_send_rsp(0);
    }
    else if(cmd == 0x97)
    {
        //SMSMT,68,0891683108702505F0240D91685110689058F500005150039015212305C8329BFD06
    }
    else if(cmd == 0x96)
    {
        serverapp = *(pdata + 3);
        LOGD(L_APP, L_V6, "==LTE 0x96!%d", serverapp);
        if(serverapp == 0)
        {
            track_cust_lte_recv_data(pdata + 4, packet_len - 6);
        }
        else
        {
            track_cust_lte_recv_data2(pdata + 4, packet_len - 6);
        }
    }
#endif
}

static void track_cust_recv_serial_data(kal_uint8 *data, int len)
{
    static kal_uint8 *buf = NULL;
    static int buflen = 0;
    kal_uint8 cmd = 0, *pdata = NULL, *tmp = NULL;
    kal_uint16 packet_len = 0;
    int len_size = 0, i = 0, j, flag = 1, backupbegin = -1;

    if(buf == NULL)
    {
        buf = (U8*)Ram_Alloc(5, SERAIL_CACHE_BUF_SIZE);
        if(buf == NULL)
        {
            LOGD(L_APP, L_V1, "ERROR: Ram_Alloc");
            track_cust_restart(64, 0);
            return;
        }
    }

    if(buflen < 0) buflen = 0;
    if(buflen + len >= SERAIL_CACHE_BUF_SIZE)
    {
        LOGD(L_OS, L_V8, "%d,%d 接收数据异常", len, buflen);
        buflen = 0;
        return;
    }
    memcpy(&buf[buflen], data, len);
    buflen += len;
    while(buflen - i > 0)
    {
        if(buf[i] == 0x77 && buf[i + 1] == 0x77)
        {
            len_size = 2;
            flag = 1;
        }
        else
        {
            if(flag)
            {
                flag = 0;
                LOGD(L_OS, L_V8, "len=%d, buflen=%d,%d 不能解析的数据包3 !!!!", len, buflen, i);
                LOGH(L_OS, L_V8, &buf[i], buflen - i);
                //LOGF(L_OS, L_V6, &buf[i], buflen - i);
            }
            i++;
            continue;
        }

        if(len_size > 0)
        {
            packet_len = buf[i + 2];
            if(len_size == 2)
            {
                packet_len = (packet_len << 8) + buf[i + 3];
            }

            if(i + packet_len + len_size + 4 > buflen)
            {
                LOGD(L_OS, L_V8, "len=%d, i=%d, buflen=%d, packet_len=%d 不能解析的数据包1 !!!!", len, i, buflen, packet_len);
                LOGH(L_OS, L_V8, &buf[i], buflen - i);
                //LOGF(L_OS, L_V6, &buf[i], buflen - i);
                flag = 0;
                i++;
                continue;
            }

            if(buf[i + packet_len + len_size + 2] != 0x0D || buf[i + packet_len + len_size + 3] != 0x0A)
            {
                LOGD(L_OS, L_V8, "buflen=%d 不能解析的数据包2 !!!!", buflen);
                LOGH(L_OS, L_V8, &buf[i], buflen - i);
                //LOGF(L_OS, L_V6, &buf[i], buflen - i);

                for(j = i + 2; j < buflen; j++)
                {
                    if(buf[j - 1] == 0x77 && buf[j] == 0x77)
                    {
                        break;
                    }
                }
                if(j < buflen)
                {
                    i = j - 1;
                    continue;
                }
                break;
            }
            if(!IsDataCrc16(&buf[i + 2], packet_len + len_size))
            {
                LOGD(L_APP, L_V1, "crc error");
                LOGH(L_APP, L_V4, &buf[i], buflen - i);
                i += packet_len + len_size + 4;
                continue;
            }
            i += len_size + 2;
            pdata = &buf[i];
            cmd = pdata[0];
            i += packet_len + 2;
            packet_len -= 2; // 减去头、长度、CRC、尾
        }
        if(pdata != NULL && packet_len > 0)
        {
            //LOGD(L_APP, L_V5, "cmd:%X, buflen:%d, SerialNumber:%.2X%.2X", cmd, packet_len, pdata[packet_len - 2], pdata[packet_len - 1]);
            track_cust_recv_serial_cmd(cmd, pdata, packet_len);
            backupbegin = i;
        }
    }
    if(backupbegin > 0 && buflen > 0)
    {
        buflen -= backupbegin;
        if(buflen > 0 && buflen < SERAIL_BACK_BUF_SIZE)
        {
            tmp = (U8*)Ram_Alloc(5, buflen);
            memcpy(tmp, &buf[backupbegin], buflen);
            memcpy(buf, tmp, buflen);
            Ram_Free(tmp);
        }
        return;
    }
    //buflen = 0;
}

void track_drv_gps_read_uart(void)
{
    kal_uint16 readTotal = 0, avail = 0, readSize = 0 , index = 0, iEnd = 0, tCous = 0;
    kal_uint8 status;
    char *pIndex = NULL, *p77 = NULL;
    static kal_uint8 buf[GPS_REVICE_BUFF_SIZE] = {0};
    static kal_uint16 jIdx = 0;
    module_type IR_Owner_save;
#if defined(__ATGM_UPDATE__)
    if(track_cust_atgm_update_status(STATUS_CHECK) >= ATGM_UPDATE_STATUS_update_done)
    {
        LOGD(L_APP, L_V1, "完成ATGM升级,即将重启,不处理GPS串口数据");
        return;
    }
#endif /*__ATGM_UPDATE__*/

    memset(buf, 0, GPS_REVICE_BUFF_SIZE);
    IR_Owner_save = UART_GetOwnerID(GPS_UART_PORT);
    avail = UART_GetBytesAvail(GPS_UART_PORT);
    LOGD(L_DRV, L_V7, "port:%d,owner=%d,gps_status:%d,avail:%d", GPS_UART_PORT, IR_Owner_save, drv_gps_status, avail);
    readTotal = 0;

    while(avail)
    {
        readSize  = UART_GetBytes(GPS_UART_PORT, (kal_uint8*)&buf[readTotal], 255, &status, IR_Owner_save);
        #if defined(__ATGM_UPDATE__)
        if(track_cust_atgm_update_status(STATUS_CHECK) >= ATGM_UPDATE_STATUS_update_start)
        {
            track_cust_atgm_update_decode(&buf[readTotal], readSize);
            return;
        }
        #endif /*__ATGM_UPDATE__*/

        readTotal += readSize;
        if(readTotal == 0)
        {
            break;
        }
        else if(readTotal > GPS_REVICE_BUFF_LIMIT || readSize != 255)
        {
            LOGD(L_DRV, L_V9, "11status:%d,size=%d,buf:%s", status, readTotal, buf);

#if defined( __GT530__)
            LOGH(L_OS, L_V8, buf, readTotal);
            LOGF(L_OS, L_V7, buf, readTotal);
            track_cust_recv_serial_data(buf, readTotal);
#else
            if(track_status_gpstc(2))
            {
                track_trace((kal_uint8*)buf, readTotal);
            }
            if(track_get_gps_ota(0xFF) == 2)
            {
                LOGD(L_DRV, L_V9, "size=%d,buf:%s", readTotal, buf);
                track_drv_uart_log_mode(2);
                U_PutUARTBytes(uart_port1, buf, readTotal);
            }
            else if(drv_gps_status) //GPS关闭时不解码
            {
                track_uart_decode(buf, readTotal);
            }
#endif /* __GT530__ */

            readTotal = 0;
            if(readSize != 30)
            {
                break;
            }
        }
    }
    readSize = strlen((char*) buf);
    if(readSize > 10 && track_is_timer_run(GPS_DATA_UPDATA_TIMER_ID))
    {
        track_stop_timer(GPS_DATA_UPDATA_TIMER_ID);
    }
    LOGD(L_DRV, L_V9, "readSize:%d,status:%d,%d", readSize, status, drv_gps_status);
    LOGH(L_DRV, L_V7, buf, 10);
}

void track_drv_uart1_write_data(kal_uint8 *data, int len)
{
    module_type IR_Owner_save = UART_GetOwnerID(OBD_UART_PORT);
    if(len < 2000) LOGH(L_OBD, L_V7, data, len);
    else if(len >= 2000) LOGH(L_OBD, L_V8, data, len);
    UART_PutBytes(OBD_UART_PORT, data, len, IR_Owner_save);
}

void track_drv_uart2_write_data(kal_uint8 *data, int len)
{
    module_type IR_Owner_save = UART_GetOwnerID(uart_port2);
    if(len < 2000)
    {
        LOGD(L_APP, L_V5, "%s", (char*)data);
        LOGH(L_OBD, L_V7, data, len);
    }
    else if(len >= 2000) LOGH(L_OBD, L_V8, data, len);
    UART_PutBytes(uart_port2, data, len, IR_Owner_save);
}

void track_drv_write_exmode(kal_uint8 *data, int len)
{
    module_type IR_Owner_save = UART_GetOwnerID(EXT_UART_PORT);
    if(len < 2000)
    {
        LOGH(L_DRV, L_V5, data, len);
        //LOGD(L_DRV, L_V5, "UART>>%s", (char*)data);
    }
    else if(len >= 2000)
    {
        LOGH(L_OS, L_V6, data, len);
    }

#if defined(__GT370__)|| defined (__GT380__)|| defined (__JM81__)|| defined (__JM66__)
    track_drv_eint_wakeup_mcu();
#endif

#if defined(__NT33__) && defined(__CUST_BT__)
    track_drv_bt_wakeup(1);
#elif defined(__GT740__) ||defined(__GT420D__)|| defined(__GT370__)|| defined (__GT380__)|| defined (__JM81__)|| defined (__JM66__)
    track_drv_sleep_enable(SLEEP_OBD_MOD, KAL_FALSE);

    #if defined (__MCU_SW_UPDATE__)
    if(!track_is_in_update_bootload_cmd())
         {
        tr_start_timer(TRACK_CUST_EXTCHIP_SLEEP_TIMER_ID, 2000, track_cust_modeule_into_sleep_mode);
    }
    else
    {
        if(track_is_timer_run(TRACK_CUST_EXTCHIP_SLEEP_TIMER_ID))
        {
            track_stop_timer(TRACK_CUST_EXTCHIP_SLEEP_TIMER_ID);
        }
    }       
    #else
    tr_start_timer(TRACK_CUST_EXTCHIP_SLEEP_TIMER_ID, 2000, track_cust_modeule_into_sleep_mode);
    #endif
#endif

#if defined (__MCU_SW_UPDATE__)
    if(track_is_in_update_bootload_cmd())
    {
		
        if(strstr(data, "AT^GT_CM=") != NULL)
        {
            LOGD(L_DRV, L_V1, "MCU升级中禁止发 AT^GT_CM=");
            return;
        }
    }
#endif 

    UART_PutBytes(EXT_UART_PORT, data, len, IR_Owner_save);
}

static kal_uint16 getSN(void)
{
    static kal_uint16 g_SerialNumber4g = 0;
    return g_SerialNumber4g++;
}

kal_uint16 track_drv_encode_exmode(kal_uint8 head, kal_uint16 packet, kal_uint8 pid, kal_uint8 *data, kal_uint16 len)
{
#if 0
    kal_uint16 no, tmp1;
    int size;
    kal_uint8 *sendData ;


    LOGD(L_APP, L_V5, "len:%d", len);
    size = 7 + len + 6;
    sendData = (kal_uint8*)Ram_Alloc(1, size);

    if(sendData == NULL)
    {
        return 0;
    }
    memset(sendData, 0, size);

    //起始位
    sendData[0] = head;
    sendData[1] = head;
    //长度
    sendData[2] = ((len + 7) & 0xFF00) >> 8;
    sendData[3] = (len + 7) & 0x00FF;
    //协议号
    sendData[4] = pid;
    //总包分包
    sendData[5] = (packet & 0xFF00) >> 8;
    sendData[6] = packet & 0x00FF;
    //内容
    memcpy(&sendData[7], data,  len);

    //信息序列号
    tmp1 = getSN();
    sendData[size - 6] = (tmp1 & 0xFF00) >> 8;
    sendData[size - 5] = (tmp1 & 0x00FF);
    LOGD(L_APP, L_V5, "SerialNumber:%.2X%.2X", sendData[size - 6], sendData[size - 5]);

    //CRC校验
    tmp1 = GetCrc16(&sendData[2], size - 6);
    sendData[size - 4] = (tmp1 & 0xFF00) >> 8;
    sendData[size - 3] = (tmp1 & 0x00FF);

    //结束
    sendData[size - 2] = '\r';
    sendData[size - 1] = '\n';
    if(track_cust_gps_status() == 0) track_cust_lte_wakeup();
    track_drv_write_exmode(sendData, size);
    Ram_Free(sendData);
    return no;
#endif
}
#if 0
kal_uint16 track_cust_upload_position_data_repacket(kal_uint8 head, kal_uint16 packet, kal_uint8 pid, kal_uint8 *data, kal_uint16 len, kal_uint32 nodata, kal_uint8  queue_id, kal_uint8 app)
{
    kal_uint16 no, tmp1;
    int size;
    kal_uint8 *sendData ;
    char tmp[300] = {0};
    kal_bool ptype = 0;


    LOGD(L_APP, L_V5, "len:%d,%d", len, queue_id);
    size = 9 + len + 6;
    sendData = (kal_uint8*)Ram_Alloc(1, size);

    if(sendData == NULL)
    {
        return 0;
    }
    memset(sendData, 0, size);
    memset(&seridata, 0, 2);
    if(*(data + 3) == 0x22)
    {
        ptype = 1;
    }
    //起始位
    sendData[0] = head;
    sendData[1] = head;
    //长度
    sendData[2] = ((len + 9) & 0xFF00) >> 8;
    sendData[3] = (len + 9) & 0x00FF;
    //协议号
    sendData[4] = pid;
    //总包分包
    sendData[5] = (packet & 0xFF00) >> 8;
    sendData[6] = packet & 0x00FF;
    sendData[7] = app;//服务器标示
    sendData[8] = queue_id;//包类型标示
    //内容
    memcpy(&sendData[9], data,  len);

    //信息序列号
    tmp1 = getSN();
    sendData[size - 6] = (tmp1 & 0xFF00) >> 8;
    sendData[size - 5] = (tmp1 & 0x00FF);
    if(ptype)
    {
        seridata[0] = sendData[size - 6];
        seridata[1] = sendData[size - 5];
    }
    LOGD(L_APP, L_V5, "SerialNumber:%.2X%.2X", sendData[size - 6], sendData[size - 5]);

    //CRC校验
    tmp1 = GetCrc16(&sendData[2], size - 6);
    sendData[size - 4] = (tmp1 & 0xFF00) >> 8;
    sendData[size - 3] = (tmp1 & 0x00FF);

    //结束
    sendData[size - 2] = '\r';
    sendData[size - 1] = '\n';
    LOGH(L_APP, L_V5, sendData, size);
    track_drv_write_exmode(sendData, size);
    Ram_Free(sendData);
    return no;

}
#endif
void track_drv_send_to_oil()
{
    kal_uint8 data[20] = {0};
    data[0] = 0x68;
    data[1] = 0x53;
    data[2] = 0x59;
    data[3] = 0x01;
    data[4] = 0x02;
    data[5] = 0x17;
    track_drv_uart1_write_data(data, strlen(&data[0]));
}
#if defined (__OIL_DETECTION__)
void track_cust_recv_ultrasonic_data(kal_uint8* data, kal_uint16 len)
{
    static kal_uint8  buf[2000] = {0};
    static kal_uint16 flag = 0;
    static kal_uint32 tick = 0;
    static int last_len = 0;
    kal_uint8 *start = NULL, *end = NULL;
    const char* findindex = NULL;
    int tmp = 0;

    int j = 0, x;


    if(len <= 0 || (len == 1 && (*data == 0 || *data == 0xFF))) return;

    if(tick >= 0 && OTA_kal_get_systicks() - tick > 8 * KAL_TICKS_1_SEC)
    {
        flag = 0;
    }

    tick = OTA_kal_get_systicks();

    if(flag + len < 1999)
    {
        for(j = 0; j < len; j++)
        {
            if(data[j] > 0 && data[j] <= 128)
            {
                buf[flag++] = data[j];
            }
        }
        buf[flag] = 0;
    }
    else
    {
        if(len > 1999)
        {
            LOGD(L_DRV, L_V1, "ERROR : Data too long!");
            return;
        }
        for(j = 0, flag = 0; j < len; j++)
        {
            if(data[j] > 0 && data[j] <= 126)
            {
                buf[flag++] = data[j];
            }
        }
        buf[flag] = 0;
    }
    LOGD(L_DRV, L_V5, "[recv](%d,%d,%d): %s", strlen(buf), flag, len, data);
    LOGH(L_DRV, 6, buf, flag);
	
    if(flag <= 0) return;
    buf[flag] = 0;
    for(j = 0; j < flag - 5; j++)
    {
        if(buf[j] == '!' && buf[j+1] == 'A' && buf[j+2] == 'I' && buf[j+3] == 'O' && buf[j+4] == 'I')
        {
            start = &buf[j];
            end = strstr(start, "\r\n");
            if(end != NULL)
            {
                flag = 0;
                findindex = strchr(start, '\r');
                tmp = findindex - start;
                LOGD(L_DRV, L_V1, "%d", tmp);
                if(G_realtime_data.netLogControl == 512)
                {
                    cust_packet_log_data_ext2(8, "[start]", start, tmp);
                }
                track_cust_oil_data_to_server(start,tmp);
            }
        }
    }

}
#endif
static void track_drv_uart_read_data(UART_PORT port)
{
    kal_uint16 readTotal = 0, avail = 0, readSize = 0;
    kal_uint8 status;
    static kal_uint8 buf[1024];
    module_type IR_Owner_save;

    memset(buf, 0, 1024);
    IR_Owner_save = UART_GetOwnerID(port);
    avail = UART_GetBytesAvail(port);
    LOGD(L_DRV, L_V7, "owner=%d,avail:%d", IR_Owner_save, avail);

    while(avail)
    {
        readSize  = UART_GetBytes(port, (kal_uint8*)&buf[readTotal], 60, &status, IR_Owner_save);
        readTotal += readSize;
        if(readTotal == 0)
        {
            break;
        }
        else if(readTotal > 1024 - 61 || readSize != 60 || (readTotal > 0 && readSize == 0))
        {
#if defined (__OIL_DETECTION__)
            track_cust_recv_ultrasonic_data(buf, readTotal);
#endif

#if defined (__NT33__) && defined (__CUST_BT__)
    track_drv_bt_rec_data(buf, readTotal);  //收到蓝牙串口数据，单独去解析
#endif

#if defined (__ET350__) && defined (__CUST_RS485__)
    track_drv_rs485_rec_data(buf, readTotal);  //收到RS485串口3数据，单独去解析
#endif

#if defined (__MT200__) && defined (__CUST_ACC_BY_MCU__)
    track_drv_acc_by_mcu_rece(buf, readTotal);  //单独去解析
#endif

#if defined(__GPS_G3333__)
            if(track_get_gps_ota(0xFF))
            {
                if(track_get_gps_ota(0xff) == 1 && strstr(buf, "$PMTK"))
                {
                    track_get_gps_ota(2);
                }
                U_PutUARTBytes(GPS_UART_PORT, buf, readTotal);
            }
#endif /* __GPS_G3333__ */


            if(readSize != 60)
            {
				if (port==EXT_UART_PORT)
				{
#if defined(__GT370__)|| defined (__GT380__)||defined (__GT740__)||defined(__GT420D__)||defined (__JM81__)||defined (__JM66__)

    #if defined (__MCU_SW_UPDATE__)
        track_cust_module_uart_update_data((kal_uint8*)buf, readTotal);
    #else
        track_cust_module_power_data((kal_uint8*)buf, readTotal);
    #endif
                
#endif /* GT700 */
				}
#if defined(__OBD__)
                if(G_parameter.obd.sw == 1)
                {
#if defined(__OBD2__)
                    track_cust_obd2_data_hex(buf, readTotal);
#else
                    track_cust_obd_data(buf, readTotal);
#endif /* __OBD2__ */
                }
#endif
                break;
            }
            
            readTotal = 0;
        }
    }
}

static void track_drv_uart2_read_data(void)
{
    kal_uint16 readTotal = 0, avail = 0, readSize = 0;
    kal_uint8 status;
    static kal_uint8 buf[1024];
    module_type IR_Owner_save;

    memset(buf, 0, 1024);
    IR_Owner_save = UART_GetOwnerID(uart_port2);
    avail = UART_GetBytesAvail(uart_port2);
    LOGD(L_DRV, L_V7, "owner=%d,avail:%d", IR_Owner_save, avail);

    while(avail)
    {
        readSize  = UART_GetBytes(uart_port2, (kal_uint8*)&buf[readTotal], 60, &status, IR_Owner_save);
        readTotal += readSize;
        if(readTotal == 0)
        {
            break;
        }
        else if(readTotal > 1024 - 61 || readSize != 60 || (readTotal > 0 && readSize == 0))
        {
            LOGD(L_DRV, L_V7, "ota:%d total:%d:%s", track_get_gps_ota(0xFF), readTotal, buf);
            if(readSize != 60)
            {

#if defined(__OBD__)
                if(G_parameter.obd.sw == 1)
                {
#if defined(__OBD2__)
                    track_cust_obd2_data_hex(buf, readTotal);
#else
                    track_cust_obd_data(buf, readTotal);
#endif /* __OBD2__ */
                }
#endif
                break;
            }
            readTotal = 0;
        }
    }
}


/******************************************************************************
 *  Function    -  track_drv_uart_ready_read
 * 
 *  Purpose     -  UART数据接收
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-08-30,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_uart_ready_read(void* msg)
{
    uart_ready_to_read_ind_struct *tmp = (uart_ready_to_read_ind_struct*)msg;
    //LOGD(L_DRV, L_V7, "msg_len:%d,ref_count:%d,port:==%d==", tmp->msg_len, tmp->ref_count, tmp->port);

#if defined (__NT33__) && defined (__CUST_BT__)
    if(uart_port3 == tmp->port)
    {
        track_drv_uart_read_data(uart_port3);
    }
#elif defined(__GT741__)||defined(__GT420D__)
    if(uart_port2 == tmp->port)
    {
        track_drv_uart_read_data(uart_port2);
    }
#elif defined (__GT370__)|| defined (__GT380__)||defined (__GT740__)||defined (__JM81__)||defined (__JM66__)
    if(uart_port3 == tmp->port)
    {
        track_drv_uart_read_data(uart_port3);
    }
#elif defined (__ET350__) && defined (__CUST_RS485__)
    if(uart_port3 == tmp->port)
    {
        track_drv_uart_read_data(uart_port3);
    }
#else
    if(GPS_UART_PORT == tmp->port)
    {
        track_drv_gps_read_uart();
    }
#endif
    else if(uart_port1 == tmp->port)
    {
        track_drv_uart_read_data(uart_port1);
    }
    else if(uart_port2 == tmp->port)
    {
        track_drv_uart2_read_data();
    }
    else if(uart_port3 == tmp->port)
    {
        //track_drv_uart_read_data(uart_port3);
        track_drv_gps_read_uart();
    }
    else
    {
        LOGD(L_DRV, L_V5, "Who am i? %d",tmp->port);
    }

}

/******************************************************************************
 *  Function    -  track_uart_decode
 *
 *  Purpose     -  uart数据接收后分包
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 12-08-2013,  jwj  written
 * v1.1  , 15-11-2013,  wangqi jwj modify
 * ----------------------------------------
 ******************************************************************************/
void track_uart_decode(char* buf, kal_uint16 len)
{
    char BD_buf[4] = {"$BD"};
    char buf2[250] = {0};
    char buf_tmp[250] = {0};
    char *p_start = NULL, *p_end, *p_start_bdchr;
    int buf_length, item_length = 0;
    static int i = 0;

    if(GPS_DECODE_BUF_SIZE < len)
    {
        len = GPS_DECODE_BUF_SIZE - 1;
    }
    //LOGH(L_OS, L_V6, buf, len);

    strncat(uart_buf, buf, GPS_DECODE_BUF_SIZE);
    buf_length = strlen(uart_buf);

    if(buf_length > 0 || buf_length <= GPS_REVICE_BUFF_SIZE + GPS_SINGLE_ITME_MAX_SIZE)
    {
        p_start = strstr(uart_buf, "$");
        if(p_start != NULL)
        {
            while(1)
            {
                item_length = 0;
                p_end = strstr(p_start, "\r\n");
                if(p_end != NULL)
                {
                    memset(buf2, 0, GPS_SINGLE_ITME_MAX_SIZE);
                    item_length = p_end - p_start + 2;
                    if(item_length < GPS_SINGLE_ITME_MAX_SIZE)
                    {
                        memcpy(buf2, p_start, item_length);
                        #if defined(__GT420D__)
                        p_start_bdchr = strstr(buf2, "GNGSA");
                        if(p_start_bdchr != NULL)
                            {
                                if(i == 0)
                                    {
                                        i = 1;
                                        }
                                    else
                                    {
                                        memcpy(buf2, BD_buf, 3);
                                        //LOGD(L_APP,L_V5,"%s",buf2);
                                        i = 0;
                                        }
                                    }
                        #endif
                        GPS_Data_Decode2(buf2, strlen(buf2));
                    }
                    else
                    {
                        LOGC(L_DRV, L_V5, "single packet legth error");
                    }
                    p_start = p_end + 2;
                }
                else
                {
                    item_length = strlen(p_start);
                    if(item_length > GPS_SINGLE_ITME_MAX_SIZE)
                    {
                        LOGD(L_DRV, L_V5, "解码异常(%d > 200)", item_length);
                        memset(uart_buf, 0x00, GPS_DECODE_BUF_SIZE);
                        break;
                    }
                    memset(buf_tmp, 0x00, GPS_SINGLE_ITME_MAX_SIZE);
                    strncpy(buf_tmp, p_start, GPS_SINGLE_ITME_MAX_SIZE);
                    memset(uart_buf, 0x00, GPS_DECODE_BUF_SIZE);
                    strncpy(uart_buf, buf_tmp, GPS_SINGLE_ITME_MAX_SIZE);
                    break;
                }

            }/*end while*/
        }
        else
        {
            memset(uart_buf, 0x00, GPS_DECODE_BUF_SIZE);
        }

    }
    else
    {
        LOGD(L_DRV, L_V6, "超长或零数据:%d", buf_length);
        memset(uart_buf, 0x00, GPS_DECODE_BUF_SIZE);
    }

}

void track_cust_gps_factory(void)
{
#if defined( __BD_AT6558__)
    char coldstart[] = "$PCAS10,3*1F\r\n";
    #if defined(__ATGM_UPDATE__)
    if(track_cust_atgm_update_status(STATUS_CHECK) >= ATGM_UPDATE_STATUS_update_begin_req)
    {
        LOGD(L_APP, L_V1, "升级流程中,不gps_factory");
        return;
    }
    #endif /*__ATGM_UPDATE__*/

    U_PutUARTBytes(GPS_UART_PORT, coldstart, 14);
#endif /* __BD_AT6558__ */
}

void track_cust_ubxcold(void)
{
#if defined( __BD_AT6558__)
    char coldstart[] = "$PCAS10,2*1E\r\n";
    #if defined(__ATGM_UPDATE__)
    if(track_cust_atgm_update_status(STATUS_CHECK) >= ATGM_UPDATE_STATUS_update_begin_req)
    {
        LOGD(L_APP, L_V1, "升级流程中,不ubxcold");
        return;
    }
    #endif /*__ATGM_UPDATE__*/

    U_PutUARTBytes(GPS_UART_PORT, coldstart, 14);
#elif defined( __GPS_G3337__) || defined( __GPS_G3333__)||defined(__GPS_MT3333__)
    char coldstart[] = "$PMTK103*30\r\n";
    U_PutUARTBytes(GPS_UART_PORT, coldstart, 13);
#else
    char ubxColdStart[] = {0xB5, 0x62 , 0x06 , 0x04 , 0x04 , 0x00 , 0xFF , 0x07 , 0x02 , 0x00 , 0x16 , 0x79}; // UBX Message to force a coldstart
    U_PutUARTBytes(GPS_UART_PORT, ubxColdStart, 12);
#endif /* __GPS_MT3337__ */
    LOGD(L_DRV, L_V4, "GPS cold");
}

void track_cust_exgps_warm(void)
{
#if defined( __BD_AT6558__)
    char coldstart[] = "$PCAS10,1*1D\r\n";
    #if defined(__ATGM_UPDATE__)
    if(track_cust_atgm_update_status(STATUS_CHECK) >= ATGM_UPDATE_STATUS_update_begin_req)
    {
        LOGD(L_APP, L_V1, "升级流程中,不exgps_warm");
        return;
    }
    #endif /*__ATGM_UPDATE__*/

    U_PutUARTBytes(GPS_UART_PORT, coldstart, 14);
#elif defined( __GPS_G3337__) || defined( __GPS_G3333__)||defined(__GPS_MT3333__)
    char coldstart[] = "$PMTK102*31\r\n";
    U_PutUARTBytes(GPS_UART_PORT, coldstart, 13);
#else
    LOGS("cant support!");
#endif /* __GPS_MT3337__ */
}

void track_cust_ubxhot(void)
{
#if defined( __BD_AT6558__)
    char coldstart[] = "$PCAS10,0*1C\r\n";
    #if defined(__ATGM_UPDATE__)
    if(track_cust_atgm_update_status(STATUS_CHECK) >= ATGM_UPDATE_STATUS_update_begin_req)
    {
        LOGD(L_APP, L_V1, "升级流程中,不ubxhot");
        return;
    }
    #endif /*__ATGM_UPDATE__*/

    U_PutUARTBytes(GPS_UART_PORT, coldstart, 14);
#elif defined( __GPS_G3337__) || defined( __GPS_G3333__)||defined(__GPS_MT3333__)
    char coldstart[] = "$PMTK101*32\r\n";
    U_PutUARTBytes(GPS_UART_PORT, coldstart, 13);
#else
    char ubxColdStart[] = {0xB5, 0x62 , 0x06 , 0x04 , 0x04 , 0x00 , 0xFF , 0x07 , 0x02 , 0x00 , 0x16 , 0x79}; // UBX Message to force a coldstart
    LOGS("can not support this function!");
    U_PutUARTBytes(GPS_UART_PORT, ubxColdStart, 12);
#endif /* __GPS_MT3337__ */
}

kal_uint8 track_get_gps_ota(kal_uint8 flag)
{
    static kal_uint8 gps_ota = 0;
    if(flag == 1 || flag == 0 || flag == 2)
    {
        gps_ota = flag;
    }
    return gps_ota;
}

void track_cust_gps_OTA_start(void)
{
    LOGD(L_DRV, L_V2, "GPS OTA START!");
    track_log_switch(0x02);//日志输出走Catcher
    if(drv_gps_status == 1)
    {
        track_drv_mgps_off();
    }
    track_drv_uart_log_mode(2);
    track_drv_uart1_Init((void*)2);
    track_get_gps_ota(1);
    if(drv_gps_status == 0)
    {
        track_drv_mgps_on();
    }
}

void track_cust_gps_OTA_stop(void)
{
    track_get_gps_ota(0);
    track_drv_uart_log_mode(2);
    track_drv_uart1_Init((void*)1);
    track_drv_uart1_Init((void*)1);
    track_log_switch(0x00);

    track_drv_uart1_mode(0);
    LOGD(L_DRV, L_V2, "GPS OTA OVER!");
}

void track_drv_wifi_switch(kal_uint8 sw)
{
#if 0
    LOGD(L_DRV, L_V5, "%d", sw);

    if(sw > 2)
    {
        return;
    }

    track_drv_lte_uart1_ctrl(1);

    if(sw == 1)
    {
        track_drv_write_exmode((kal_uint8 *)wifion, sizeof(wifion));
    }
    else if(sw == 0)
    {
        track_drv_write_exmode((kal_uint8 *)wifioff, sizeof(wifioff));
    }
    else if(sw == 2)
    {
        track_drv_write_exmode((kal_uint8 *)wifitest, sizeof(wifitest));
    }
#endif
}

/****************************************************************************************/

void gt530_test_gps(void* arg)
{
#if 0
    int ar = (int) arg;
    static int index = 0;
    LOGD(L_SYS, L_V5, "ar:%d, index:%d", ar, index);

    //track_drv_lte_uart1_ctrl(1);
    if(ar == 0)
    {
        track_start_timer(TRACK_DRV_GPS_CHECK_TIMER_ID, 3000, gt530_test_gps, (void*)1);
    }
    else if(ar == 1)
    {
        track_drv_mgps_on();
        track_start_timer(TRACK_DRV_GPS_CHECK_TIMER_ID, 3000, gt530_test_gps, (void*)1);
    }
    else if(ar == 2)
    {
        track_start_timer(TRACK_DRV_GPS_CHECK_TIMER_ID, 2000, gt530_test_gps, (void*)3);
    }
    else if(ar == 3)
    {
        track_drv_lte_gpstc(1);
        track_start_timer(TRACK_DRV_GPS_CHECK_TIMER_ID, 5000, gt530_test_gps, (void*)3);
    }
    else
    {
        track_stop_timer(TRACK_DRV_GPS_CHECK_TIMER_ID);
    }
#endif
}

#if defined(__SAVING_CODE_SPACE__)
#else
void track_uart_case(int m)
{
    char str[] = "this is track uart case send a msg to me, thanks!\r\n";
    char str1[] = {0x77, 0x77, 0x00, 0x0C, 0x91, 0x01, 0x01, 0x47, 0x50, 0x53, 0x4F, 0x4E, 0x00, 0x02, 0xAB, 0x00, 0x0D, 0x0A, 0};

    module_type IR_Owner_save = UART_GetOwnerID(uart_port1);
    LOGD(L_DRV, L_V5, "IR_Owner_save:%d:%d", IR_Owner_save, OBD_COMM_OWNER_ID);
    if(m == 105)
    {
        OTA_track_GPIO_SetClkOut(1, 0x82/*mode_f32k_ck*/);//输出32K
    }
    else if(m == 106)
    {
        track_drv_set_vcama(KAL_TRUE, 2800000);
    }
    else if(m == 107)
    {
        track_drv_mgps_uart_Iint();
    }
    else if(m == 108)
    {
        track_drv_gps_enable(1);
    }
    else if(m == 109)
    {
        track_drv_gps_enable(0);
    }
    else if(m == 110)
    {
        //LOGD(L_DRV, L_V2, "track_uart_case_init_port1()");
        //track_uart_init_port(uart_port1, UART_BAUD_9600, MOD_MMI);
        tarck_drv_uart2_init();
    }
    else if(m == 111)
    {
        UART_PutBytes(uart_port1, str, sizeof(str), IR_Owner_save);
        //track_drv_write_exmode("hello!\r\n", 8);
        //"7777000C9101014750534F4E0002AB000D0A"
        track_drv_lte_uart1_ctrl(1);
        track_drv_write_exmode((kal_uint8 *)str1, sizeof(str1));
        track_drv_write_exmode((kal_uint8 *)str1, sizeof(str1));

    }
    else if(m == 112)
    {
        //U_PutBytes(uart_port1, str, strlen(str), IR_Owner_save);
        track_cust_gps_OTA_stop();
    }
    else if(m == 113)
    {
        //U_PutUARTBytes(uart_port1, str, sizeof(str));
        GPIO_ModeSetup(12, 0); //61D open uart2
        GPIO_ModeSetup(17, 0);
        track_drv_gpio_set(1, 12);
        track_drv_gpio_set(1, 17);
    }
    else if(m == 114)
    {
        track_uart_Close(0, IR_Owner_save);
    }
    else if(m == 115)
    {
        //kal_uint8 str[256] = {0};
        // kal_uint8 sLen = 255, status = 0;
        IR_Owner_save = UART_GetOwnerID(uart_port2);
        track_uart_Close(uart_port2, IR_Owner_save);

        //track_uart_GetBytes(uart_port1, str, sLen, &status, MOD_MMI);
    }
    else if(m == 116)
    {
        track_drv_sleep_enable(/*SLEEP_DRV_MOD*/0, FALSE);
        track_drv_uart1_Init((void*)2);
    }
    else if(m == 117)
    {
        track_drv_mgps_on();
        track_drv_mgps_uart_Iint();
    }
    else if(m == 118)
    {
    }
    else if(m == 119)
    {
        track_drv_uart3_Iint();
    }
}


void track_cust_lte_packet92_decode(kal_uint8 *pdata, kal_uint8 packet_len)
{
    kal_uint8 error_id = 1;
    kal_uint8 queue_id = *(pdata + 4);
    serverapp = *(pdata + 3);
    if(*(pdata + 6) == 0x50 && *(pdata + 7) == 0x41 && *(pdata + 8) == 0x53 && *(pdata + 9) == 0x53)
    {
        error_id = 0;
    }
    LOGD(L_APP, L_V5, "==0x92!queue_id = %d,errorid=%d,%d", queue_id, error_id, serverapp);
    if(serverapp == 0)
    {
        socket_send_status(0, 4, error_id, queue_id);
    }
    else {}
    if(track_cust_gps_status() == 0)
    {
        track_cust_pre_lte_sleep();
    }
}
void track_cust_lte_net_status_recv(kal_uint8 *pdata, kal_uint8 packet_len, kal_uint8 sign)
{

    switch(sign)
    {
        case 0:
            {
                //SERVERSET,0,61.143.127.62,9080,PASS
                serverapp = *(pdata + 13) - 48;
                LOGD(L_APP, L_V5, "%d联网", serverapp);
                pdata += 11;
                if(serverapp == 0)
                {
                    if(strstr(pdata, "PASS"))//  CONNECTED
                    {
                        LOGD(L_APP, L_V6, "CONNECTED set 成功,2s severon");
                        track_cust_serverset(1);
                        cb.status = 2;
                        //track_lte_net_event(&cb);
                        tr_start_timer(TRACK_CUST_NET_CONN_TIMER_ID, 2000, track_cust_net_conn);
                    }
                    else
                    {
                        track_cust_serverset(0);
                        //SET BEGIN
                        cb.status = 7;
                        track_lte_net_event(&cb);
                        //SET END
                        tr_start_timer(TRACK_CUST_NET_CONN_TIMER_ID, 10000, track_cust_net_conn);
                    }
                }
                else if(serverapp == 1)
                {
                    if(strstr(pdata, "CONNECTED"))
                    {
                        track_cust_serverset(3);
                        cb.status = 2;
                        track_lte_event2_recv(&cb);
                    }
                    else
                    {
                        track_cust_serverset(2);
                        track_start_timer(TRACK_CUST_NET_CONN_TIMER_ID2, 10000, track_cust_server2_conn, (void*)13);
                    }
                }
            }
            break;
        case 1:
            {
                serverapp = *(pdata + 12) - 48; //这是转码,搞siao
                LOGD(L_APP, L_V5, "%d联网", serverapp); //SERVERON,0,PASS
                if(serverapp == 0)
                {
                    if(strstr(pdata, "CONNECTED")) //
                    {
                        cb.status = 2;
                        tr_stop_timer(TRACK_CUST_NET_CONN_TIMER_ID);
                    }
                    else if(strstr(pdata, "CONNECTING"))
                    {
                        track_set_login_status(KAL_FALSE);
                        //track_soc_conn_status(KAL_FALSE);
                    }
                    else if(strstr(pdata, "PASS"))
                    {
                    }
                    else
                    {
                        cb.status = 7;
                        track_soc_conn_status(KAL_FALSE);
                    }
                    track_lte_net_event(&cb);
                }
                else
                {
                    if(strstr(pdata, "PASS")) //CONNECTED
                    {
                        cb.status = 2;
                        track_lte_event2_recv(&cb);
                    }
                    else if(strstr(pdata, "CONNECTING"))
                    {
                    }
                    else
                    {
                        cb.status = 7;
                    }
                    //track_lte_event2_recv(&cb);
                }
            }
            break;
        case 2:
            {
                serverapp = *(pdata + 13) - 48;
                LOGD(L_APP, L_V5, "%d断网", serverapp);
                if(strstr(pdata, "PASS") && !serverapp)
                {
                    cb.status = 7;
                    track_soc_conn_status(KAL_FALSE);
                    track_lte_net_event(&cb);
                    tr_stop_timer(TRACK_CUST_NET_CONN_TIMER_ID);
                }
                else if(strstr(pdata, "FAIL") && !serverapp)
                {
                    ;
                }
            }
            break;
        default :
            break;
    }
}
void track_drv_lte_server_close(kal_uint8 app)
{
    if(app == 1)
    {
        track_drv_encode_exmode(0x77, 0x0101, 0x91, "SERVEROFF,1", 11);
    }
    else
    {
        track_drv_encode_exmode(0x77, 0x0101, 0x91, "SERVEROFF,0", 11);
    }
}
void track_cust_lte_server_data_recv(kal_uint8 *pdata, kal_uint8 packet_len)
{
    kal_uint8 sign = 0;
    if(strstr(pdata, "SET"))
    {
        sign = 0;
    }
    else if(strstr(pdata, "ON"))
    {
        sign = 1;
    }
    else if(strstr(pdata, "OFF"))
    {
        sign = 2;
    }
    track_cust_lte_net_status_recv(pdata, packet_len, sign);
}
void track_cust_get_lbs_data(void * arg)
{
    kal_uint16 sign = (kal_uint16)arg;
    if(sign == 0 || sign == 1)
    {
        track_drv_encode_exmode(0x77, 0x0101, 0x91, "LBS", 3);
        track_start_timer(TRACK_CUST_GET_LBS_TIMER_ID, 10000, track_cust_get_lbs_data, (void*)1);
    }
    else if(sign == 2)
    {
        if(track_cust_sensor_delay_valid() == KAL_TRUE)
        {
            track_drv_encode_exmode(0x77, 0x0101, 0x91, "LBS", 3);
        }
        track_start_timer(TRACK_CUST_GET_LBS_TIMER_ID, 30000, track_cust_get_lbs_data, (void*)2);
    }
    else
    {
        track_stop_timer(TRACK_CUST_GET_LBS_TIMER_ID);
    }

}
LBS_Multi_Cell_Data_Struct lbs_multi_cell_data_struct_temp;
void track_drv_lbs_data_decode(kal_uint8 *pdata, kal_uint8 datalen)
{
    //460,0,10365,8000,6,7703,3,8050,3
    int splitnum = 0, counts = 0, countemp = 0;
    int i = 0, j = 0, k = 0;
    char temp[100] = {0};
    char data[10];
    kal_uint32 inteval = 0;
    kal_uint16 mcc = 0, mnc = 0, lac = 0;
    kal_uint16 cellid[15];//12
    kal_uint8 templen = 0;

    if(strstr(pdata, "PASS")) return;
    memset(temp, 0, 100);
    memcpy(&temp, pdata, datalen);
    LOGD(L_APP, L_V5, "lbs解码len=%d,%s", datalen, temp);
    for(i = 0; i < datalen; i++)
    {
        if(*(pdata + i) == ',')
        {
            splitnum ++;
        }
    }
    counts = splitnum;
    if(splitnum < 5) return;
    splitnum = (splitnum - 3) / 2 + 1;
    LOGD(L_APP, L_V5, "小区数%d", splitnum);

    for(i = 0; i < datalen; i++)
    {
        if(*(pdata + i) == ',')
        {
            memset(data, 0, 10);
            memcpy(&data, pdata, i);
            inteval = track_fun_atoi(data);
            if(j == 0)
            {
                mcc = inteval;
                j = 1;
            }
            else if(j == 1)
            {
                mnc = inteval;
                j = 2;
            }
            else if(j == 2)
            {
                lac = inteval;
                j = 3;
            }
            else
            {
                cellid[k++] = inteval;
            }
            //LOGD(L_DRV, L_V5, "%d,%d,%d,%d,%d,%s", countemp,counts,inteval, i, datalen,data);
            pdata += i + 1;
            datalen = datalen - (i + 1);
            i = 0;
            countemp ++;
            if(counts == countemp)
            {
                memset(data, 0, 10);
                memcpy(&data, pdata, datalen);
                inteval = track_fun_atoi(data);
                //LOGD(L_DRV, L_V5, "zu%d,%d,%d,%s", inteval, i, datalen, data);
                cellid[k++] = inteval;
                break;
            }
        }
    }
    if(splitnum > 6) splitnum = 6;
    for(i = j = 0; i < splitnum; i++)
    {
        lbs_multi_cell_data_struct_temp.NbrCell_Info[i].mcc = mcc;
        lbs_multi_cell_data_struct_temp.NbrCell_Info[i].mnc = mnc;
        lbs_multi_cell_data_struct_temp.NbrCell_Info[i].lac = lac;
        lbs_multi_cell_data_struct_temp.NbrCell_Info[i].cell_id = cellid[j++];
        lbs_multi_cell_data_struct_temp.NbrCell_Info[i].rxlev = cellid[j++];
        if(i == 0)
        {
            lbs_multi_cell_data_struct_temp.MainCell_Info.mcc = mcc;
            lbs_multi_cell_data_struct_temp.MainCell_Info.mnc = mnc;
            lbs_multi_cell_data_struct_temp.MainCell_Info.lac = lac;
            lbs_multi_cell_data_struct_temp.MainCell_Info.cell_id = lbs_multi_cell_data_struct_temp.NbrCell_Info[i].cell_id;
            lbs_multi_cell_data_struct_temp.MainCell_Info.rxlev = lbs_multi_cell_data_struct_temp.NbrCell_Info[i].rxlev;

        }
    }
    lbs_multi_cell_data_struct_temp.ta = 0;
    lbs_multi_cell_data_struct_temp.valid_cel_num = splitnum;
    lbs_multi_cell_data_struct_temp.status = TRUE;
    track_drv_set_lte_lbs_data(lbs_multi_cell_data_struct_temp);
    track_start_timer(TRACK_CUST_GET_LBS_TIMER_ID, 30000, track_cust_get_lbs_data, (void*)2);
}
#endif

#if defined(__GT370__)|| defined (__GT380__)|| defined (__JM81__)|| defined (__JM66__)
#define GSM_EINT_WAKEUP_MCU_GPIO 30

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

void track_drv_eint_wakeup_mcu(void)
{
    static kal_bool first = KAL_TRUE;
    LOGD(L_DRV, L_V5,  "wakeup mcu");

    if(GSM_EINT_WAKEUP_MCU_GPIO == 0xFF)
    {
        return;
    }

#if 1
    if(first)
    {
        first = KAL_FALSE;
        GPIO_ModeSetup(GSM_EINT_WAKEUP_MCU_GPIO, 0);
        GPIO_InitIO(1, GSM_EINT_WAKEUP_MCU_GPIO);
        track_drv_gpio_set(0, GSM_EINT_WAKEUP_MCU_GPIO);
    }
#else
    /*单片机不休眠，无需唤醒操作       --    chengjun  2017-03-27*/
    if(first)
    {
        first = KAL_FALSE;
        GPIO_ModeSetup(GSM_EINT_WAKEUP_MCU_GPIO, 0);
        GPIO_InitIO(1, GSM_EINT_WAKEUP_MCU_GPIO);
    }

    track_drv_gpio_set(1, GSM_EINT_WAKEUP_MCU_GPIO);
    delay_1ms(2);
    track_drv_gpio_set(0, GSM_EINT_WAKEUP_MCU_GPIO);
    delay_1ms(10);
    track_drv_gpio_set(1, GSM_EINT_WAKEUP_MCU_GPIO);
    delay_1ms(2);
#endif
}

#endif

