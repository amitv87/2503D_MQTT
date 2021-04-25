/******************************************************************************
 * track_drv_uart1_mux.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        串口1复用切换处理
 *
 * modification history
 * --------------------
 * v1.0   2017-04-18,  chengjun create this file
 *
 ******************************************************************************/
#if defined(__CUST_UART1_MUX__)
/****************************************************************************
* Include Files                         包含头文件
*****************************************************************************/
#include "track_os_log.h"
#include "track_os_ell.h"
#include "track_drv_system_param.h"

/*****************************************************************************
* Typedef  Enum                         枚举定义
*****************************************************************************/

/*****************************************************************************
 *  Struct                              数据结构定义
 *****************************************************************************/


/*****************************************************************************
 *  Define					宏定义
 *****************************************************************************/
//高使能
#define GPIO_RS485_AND_RS232_EN     29

//高-RS485/低-RS232
#define GPIO_RS485_OR_RS232_CS     27

//高-RS485发/低-RS485收
#define GPIO_RS485_TX_RX_EN     3

/*****************************************************************************
* Local variable                        局部变量
*****************************************************************************/
static kal_uint8 uart1_mux_mode = 0;
//0=uart1
//1=RS485
//2=RS232

#if defined (__CUST_TEMP_HUMI_SENSOR__)
kal_uint8 temp_humi_sensor_rs485_busy = 0;
#endif /* __CUST_TEMP_HUMI_SENSOR__ */

/****************************************************************************
* Global Variable                       全局变量
*****************************************************************************/


/****************************************************************************
* Global Variable - Extern              引用全局变量
*****************************************************************************/


/*****************************************************************************
 *  Global Functions    - Extern        引用外部函数
 *****************************************************************************/

/*****************************************************************************
 *  Local Functions                     本地函数
 *****************************************************************************/
void track_drv_uart1_mux_switch(kal_uint8 mode);


/*****************************************************************************
 *  Global Functions			全局函数
 *****************************************************************************/
void track_drv_sensor_get_temp_humi_req(void);
void track_drv_temp_humi_sensor_decode(kal_uint8 *data, int len);

/******************************************************************************
 *  Function    -  track_drv_rs485_rs232_gpio_init
 *
 *  Purpose     -  始终开启RS485/RS232模块供电，默认RS232通信
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-06-01,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_drv_rs485_rs232_gpio_init(void)
{
    static kal_uint8 first = 0;
    if(first == 0)
    {
        first = 1;
        GPIO_ModeSetup(GPIO_RS485_AND_RS232_EN, 0);
        GPIO_InitIO(1, GPIO_RS485_AND_RS232_EN);
        //GPIO_WriteIO(0, GPIO_RS485_AND_RS232_EN);
        GPIO_WriteIO(1, GPIO_RS485_AND_RS232_EN);//for hw test

        GPIO_ModeSetup(GPIO_RS485_OR_RS232_CS, 0);
        GPIO_InitIO(1, GPIO_RS485_OR_RS232_CS);
        GPIO_WriteIO(0, GPIO_RS485_OR_RS232_CS);
        GPIO_ModeSetup(GPIO_RS485_TX_RX_EN, 0);
        GPIO_InitIO(1, GPIO_RS485_TX_RX_EN);
        GPIO_WriteIO(0, GPIO_RS485_TX_RX_EN);

        uart1_mux_mode = 2;

#if defined (FOR_RS485_TEST_TOOL)
        /*一直在RS485接收模式，等待接收数据 -- chengjun  2017-06-02*/
        track_drv_uart1_mux_switch(1);
        track_drv_sleep_enable(SLEEP_EXT_UART_MOD, KAL_FALSE);
#endif /* FOR_RS485_TEST_TOOL */

    }
}

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

void track_drv_uart1_init_delay(void)
{
    track_drv_rs485_rs232_gpio_init();

    //tr_start_timer(TRACK_RS485_HW_TEST_TIMER_ID, 20000, track_drv_sensor_get_temp_humi_req);
}

void track_drv_uart1_mux_switch(kal_uint8 mode)
{
    if(mode == 1) //RS485-RX
    {
        GPIO_WriteIO(1, GPIO_RS485_OR_RS232_CS);
        GPIO_WriteIO(1, GPIO_RS485_AND_RS232_EN);
        GPIO_WriteIO(0, GPIO_RS485_TX_RX_EN);
        uart1_mux_mode = 1;
    }
    else if(mode == 2) //RS232
    {
        GPIO_WriteIO(0, GPIO_RS485_OR_RS232_CS);
        GPIO_WriteIO(1, GPIO_RS485_AND_RS232_EN);
        uart1_mux_mode = 2;
    }
    else
    {
        GPIO_WriteIO(0, GPIO_RS485_AND_RS232_EN);
        uart1_mux_mode = 0;
    }
}

void track_drv_uart1_mux_write(kal_uint8 *data, int len)
{
    if(uart1_mux_mode == 1) //rs485-tx
    {
        if(temp_humi_sensor_rs485_busy == 1)
        {
            return;
        }
        GPIO_WriteIO(1, GPIO_RS485_TX_RX_EN);
        delay_1ms(2);
        U_PutUARTBytes(uart_port1, data, len);

        delay_1ms(len);
        GPIO_WriteIO(0, GPIO_RS485_TX_RX_EN);
    }
    else
    {
        U_PutUARTBytes(uart_port1, data, len);
        U_PutUARTBytes(0, (kal_uint8*)"\r\n", 2);
    }
}

#if defined (FOR_RS485_TEST_TOOL)
void track_drv_uart1_mux_rsp_delay(void)
{
    track_drv_uart1_mux_write("RS485\r\n", 7);
}

#endif /* FOR_RS485_TEST_TOOL */

kal_uint8 track_drv_uart1_mux_read(kal_uint8 *data, int len)
{

#if defined (FOR_RS485_TEST_TOOL)
    /*一直在RS485接收模式，等待接收数据 -- chengjun  2017-06-02*/
    track_drv_sleep_enable(SLEEP_EXT_UART_MOD, KAL_FALSE);
    if(strstr(data, "RS485"))
    {
        tr_start_timer(TRACK_RS485_HW_TEST_TIMER_ID, 100, track_drv_uart1_mux_rsp_delay);
    }
    return 1;
#endif /* FOR_RS485_TEST_TOOL */

    if(track_is_testmode())
    {
        if(strstr(data, "RS485"))
        {
            if(track_is_timer_run(TRACK_RS485_HW_TEST_TIMER_ID))
            {
                track_drv_uart1_mux_switch(2);
                LOGS("%s", "RS485 OK");
                track_stop_timer(TRACK_RS485_HW_TEST_TIMER_ID);
            }
            return 1;
        }
    }


    if(data[0] == 0xF5)
    {
        track_drv_uart1_mux_switch(2);
        LOGD(L_DRV, L_V4, "rs485 get %d,0x%02X", len, data[0]);
        LOGH(L_DRV, L_V4, data, len);
        track_drv_temp_humi_sensor_decode(data, len);
        return 1;
    }

    return 0;

}


#if defined (__CUST_TEMP_HUMI_SENSOR__)

void track_drv_sensor_get_temp_humi_req_timeout(void)
{
    track_drv_uart1_mux_switch(2);
    temp_humi_sensor_rs485_busy = 0;
}


void track_drv_sensor_get_temp_humi_req(void)
{
    kal_uint8 sendBuff[] = {0xF5, 0x50, 0x07, 0x00, 0x00, 0x4C};
    delay_1ms(100);
    track_drv_uart1_mux_switch(1);
    track_drv_uart1_mux_write(sendBuff, sizeof(sendBuff));
    temp_humi_sensor_rs485_busy = 1;
    tr_start_timer(TRACK_RS485_HW_TEST_TIMER_ID, 2000, track_drv_sensor_get_temp_humi_req_timeout);
}

void track_drv_temp_humi_sensor_decode(kal_uint8 *data, int len)
{
    int i;
    kal_uint8 sum = 0;

    for(i = 0; i <= len - 2; i++)
    {
        sum = ((sum + data[i]) & 0xFF);
    }

    if(sum != data[len-1])
    {
        LOGD(L_DRV, L_V4, "校验和错误 0x%02X", sum);
        return;
    }

    LOGH(L_DRV, L_V4, data, len);

}


#endif /* __CUST_TEMP_HUMI_SENSOR__ */

#endif /* __CUST_UART1_MUX__ */
