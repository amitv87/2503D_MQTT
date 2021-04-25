/******************************************************************************
 * track_sensor_da213.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        DA213 gSensor驱动
 *
 * modification history
 * --------------------
 * v1.0   2017-05-16,  chengjun create this file
 *
 ******************************************************************************/

#if defined (__MIRA_DA213__)

#include "kal_release.h"
#include "custom_config.h"
#include "gpio_sw.h"
#include "drv_comm.h"
#include "eint.h"
#include "motion_sensor.h"
#include "track_sensor_iic.h"
#include "track_sensor_da213.h"
#include "kal_trace.h"
#include "track_os_timer.h"
#include "track_drv_sensor.h"
#include "ell_os.h"

static kal_uint8 da213_chip_id = 0XFF;


kal_bool sensor_da213_write_byte(kal_uint8 ucBufferIndex, kal_uint8 pucData)
{
    return ms_i2c_send(DA213_I2C_SLAVE_ADDR, ucBufferIndex, &pucData, 1);
}


#define sensor_da213_read(a,b,c) ms_i2c_receive(DA213_I2C_SLAVE_ADDR,a,b,c)


#if defined (__TRACK_SENSOR__)

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
/******************************************************************************
 *
 *     Sensor 自定义部分
 *
 ******************************************************************************/
static void  sensor_da213_init(void)
{
    unsigned char threshold = 0;

    ms_i2c_configure(DA213_I2C_SLAVE_ADDR, MS_DELAY_TIME);
    sensor_da213_write_byte(0x00, 0x24);//软件复位
    delay_1ms(10);
    sensor_da213_read(0x01, &da213_chip_id, 1);   //chip ID

#if 0
//tap+active
    sensor_da213_write_byte(0x0F, 0x02); //14bit & 8g
    sensor_da213_write_byte(0x10, 0x08); //XYZ enable & ODR 250Hz
    sensor_da213_write_byte(0x11, 0x1E); //low power mode & bw-500Hz
    sensor_da213_write_byte(0x16, 0x27); //TAP-S+active enable

	//sensor_da213_write_byte(0x19, 0x04); //active to INT1
    //sensor_da213_write_byte(0x1B, 0x20); //tap-s to INT2
	sensor_da213_write_byte(0x19, 0x24); //tap-s+active to INT1

	sensor_da213_write_byte(0x20, 0x01); //int1 H-active
    sensor_da213_write_byte(0x21, 0x83); //ltach 1s
    sensor_da213_write_byte(0x2A, 0x04); //250
    sensor_da213_write_byte(0x2B, 31);//x*250mg tap
    sensor_da213_write_byte(0x27, 0x02); //active time (x+1)ms
    sensor_da213_write_byte(0x28, OTA_track_sensor_config_data->sensor_threshold);
#endif
	
#if 1
//only active
    sensor_da213_write_byte(0x0F, 0x00); //14bit & 2g
    sensor_da213_write_byte(0x10, 0x08); //XYZ enable & ODR 250Hz
    sensor_da213_write_byte(0x11, 0x1E); //low power mode & bw-500Hz
    sensor_da213_write_byte(0x16, 0x87); //active
    sensor_da213_write_byte(0x19, 0x04); //to INT1
    sensor_da213_write_byte(0x20, 0x01); //H-active
    sensor_da213_write_byte(0x21, 0x83); //ltach 1s
    sensor_da213_write_byte(0x27, 0x01); //active time (x+1)ms
    sensor_da213_write_byte(0x28, OTA_track_sensor_config_data->sensor_threshold);
#endif 
}

#if 0
#include "carMotion.h"
#include "mira_std.h"

static kal_timerid mir3da_timerid = 0;


s8_m handleEvent(carMotion_EVENT event, s32_m data)
{
    kal_uint8 buff[64] = {0};
    switch(event)
    {
        case TEVENT_VIDEOING_NOTIFY:
            //printf(" [ 发生碰撞 ].\r\n");
            sprintf(buff, "%s", " [ 发生碰撞 ].\r\n");
            break;

        case TEVENT_PARKING_NOTIFY:
            sprintf(buff, "%s", " [ 启动守卫 ].\r\n");
            break;

        case TEVENT_VIOLENT_NOTIFY:
            if(data == 1)
                sprintf(buff, "%s", " [ 加速 ].\r\n");
            else if(data == 2)
                sprintf(buff, "%s", " [ 减速 ].\r\n");
            else if(data == 3)
                sprintf(buff, "%s", " [ 左转 ].\r\n");
            else if(data == 4)
                sprintf(buff, "%s", " [ 右转 ].\r\n");
            else
                sprintf(buff, "%s", " [ 正常 ].\r\n");
            break;

        case TEVENT_STATE_NOTIFY:
            if(data == 1)
                sprintf(buff, "%s", " [ 汽车停止 ].\r\n");
            else if(data == 2)
                sprintf(buff, "%s", " [ 汽车运行 ].\r\n");
            else if(data == 3)
                sprintf(buff, "%s", " [ 翻车报警 ].\r\n");
            break;

        case TEVENT_JOLT_NOTIFY:
            if(data == 1)
                sprintf(buff, "%s", " [ 路面颠簸 ].\r\n");
            else
                sprintf(buff, "%s", " [ 路面正常 ].\r\n");
            break;

        default:
            break;
    }

    U_PutUARTBytes(0, (kal_uint8*)buff, strlen(buff));
    return 0;
}


s8_m readReg(u8_m addr, u8_m *data, u8_m len)
{
    s8_m ret;
    ret = sensor_da213_read(addr, data, len);
    return 0;
}

s8_m writeReg(u8_m addr, u8_m data)
{
    s8_m ret;
    ret = sensor_da213_write_byte(addr, data);
    return 0;
}

s8_m mir3da_save_cali_file(s16_m x, s16_m y, s16_m z)
{
    return 0;
}

s8_m mir3da_get_cali_file(s16_m *x, s16_m *y, s16_m *z)
{
    return 0;
}

u32_m mir3da_create_timer(void)
{

    if(mir3da_timerid == 0)
    {
        mir3da_timerid = kal_create_timer("mir3da sensor timer");
    }

    return 1;
}

s8_m mir3da_start_timer(u32_m id, u32_m msDelay, u8_m *auto_reset, void(*pCallback)())
{

    *auto_reset = 0;
    kal_cancel_timer(mir3da_timerid);
    kal_set_timer(mir3da_timerid, (kal_timer_func_ptr)pCallback, NULL, ((msDelay * 110) / 500), 0);

    return 0;
}

s8_m mir3da_cancel_timer(u32_m id)
{
    kal_cancel_timer(mir3da_timerid);
}

int myprintf(const char *fmt, ...)
{
    va_list ap;
    char buf[1000] = {0};

    va_start(ap, fmt);
    vsnprintf(buf, 1000, fmt, ap);
    va_end(ap);

    //kal_prompt_trace(MOD_TST, buf);
    U_PutUARTBytes(0, (kal_uint8*)buf, strlen(buf));
}


struct carMotion_op_s  ops_handle = {  {  PIN_ONE, PIN_LEVEL_HIGH, PIN_TWO, PIN_LEVEL_HIGH }, handleEvent, \
    { readReg, writeReg}, {mir3da_save_cali_file, mir3da_get_cali_file}, \
    {mir3da_create_timer, mir3da_start_timer, mir3da_cancel_timer}, myprintf
};


int track_carMotion_init(void)
{
    int res = 0;
    kal_uint8 buff[32] = {0};

    res = carMotion_Init(&ops_handle);

    sprintf(buff, "track_carMotion_init 初始化 %d", res);
    U_PutUARTBytes(0, (kal_uint8*)buff, strlen(buff));

    if(res != 0)
    {
        //kal_prompt_trace(MOD_TST, "------xotion_init failed  = %x-----\r\n", res);
        return res;
    }

    carMotion_Direction_Set_Parma(1);

    carMotion_Slope_Set_Parma(3, 3, 30, 60); //10s read one time

    carMotion_Set_Debug_level(0xFF);

	return res;

}
#endif

/******************************************************************************
 *  Function    -  sensor_da213_on
 *
 *  Purpose     -  sensor 寄存器和中断配置
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void sensor_da213_on(void)
{
#if 0
    /*急加速转弯行车监测库 -- chengjun  2017-05-25*/
    track_carMotion_init();
#else
    sensor_da213_init();
    track_sensor_da213_interrput_configuration();
#endif
}

/******************************************************************************
 *  Function    -  sensor_da213_pwr_down
 *
 *  Purpose     -  关闭sensor
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 05-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void sensor_da213_pwr_down(void)
{
    sensor_da213_write_byte(0x20, 0x81);//reset all latched int.
    track_drv_delay_1ms(5);
    sensor_da213_write_byte(0x20, 0x81);//reset all latched int.x2
    sensor_da213_write_byte(0x11, 0xDE);//to suspend mode
}

/******************************************************************************
 *  Function    -  track_sensor_da213_reset
 *
 *  Purpose     -  sensor 复位
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 05-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_sensor_da213_reset(void)
{
    //OTA_LOGD(L_DRV, L_V1, "sensor da213 reset");
    sensor_da213_pwr_down();
    OTA_tr_start_timer(MOTION_SENSOR_RESET_TIMER_ID,  10000, sensor_da213_on);
}

/******************************************************************************
 *  Function    -  track_sensor_da213_data_self_test
 *
 *  Purpose     -  sensor 工作异常监测
 *
 *  Description - 定时参数自检，若异常则重置
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_sensor_da213_data_self_test(void)
{
    unsigned char reg16, threshold = 0;
    unsigned char tmp[6] = {0};
    short x = 0 ,y = 0 ,z = 0;
    
    //sensor_da213_write_byte(0x16, 0x87); //active
    sensor_da213_read(0x16, &reg16, 1);
    //sensor_da213_write_byte(0x28, OTA_track_sensor_config_data->sensor_threshold);
    sensor_da213_read(0x28, &threshold, 1);

    sensor_da213_read(0x02, tmp, 6);
    x = (short)((tmp[1] <<8)|tmp[0])>>4;
    y = (short)((tmp[3] <<8)|tmp[2])>>4;
    z = (short)((tmp[5] <<8)|tmp[4])>>4;
    OTA_LOGD(L_DRV, L_V1, "Sensor xyz %d %d %d",x,y,z);

    if ((x == -1)&&(y == -1)&&(z == -1))
    {
        sensor_da213_init();
        OTA_tr_start_timer(MOTION_SENSOR_DATA_TIMER_ID, 60000, track_sensor_da213_data_self_test);
        return;
    }
    
    if((reg16 != 0x87) || (threshold != OTA_track_sensor_config_data->sensor_threshold))
    {
        OTA_LOGD(L_DRV, L_V1, "Sensor error (0x87-0x%02X,0x%02X-0x%02X", \
                 reg16, threshold, OTA_track_sensor_config_data->sensor_threshold);

        OTA_tr_stop_timer(MOTION_SENSOR_DATA_TIMER_ID);
        OTA_tr_start_timer(MOTION_SENSOR_RESET_TIMER_ID,  3000, track_sensor_da213_reset);
    }
    OTA_tr_start_timer(MOTION_SENSOR_DATA_TIMER_ID, 60000, track_sensor_da213_data_self_test);
}


/******************************************************************************
 *  Function    -  track_sensor_da213_interrput_configuration
 *
 *  Purpose     -  注册高电平(  震动) 响应
 *
 *  Description -
 *
 * 1、自动清标志
 * 2、软件防抖
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_sensor_da213_interrput_configuration(void)
{
    OTA_LOGD(L_DRV, L_V1, "Sensor eint reg H");
    EINT_Mask(MOTION_SENSOR_EINT_NO);
    EINT_Registration(MOTION_SENSOR_EINT_NO, KAL_TRUE, 1, track_sensor_da213_eint_hisr, KAL_TRUE);
    EINT_Set_Sensitivity(MOTION_SENSOR_EINT_NO, LEVEL_SENSITIVE);
    EINT_SW_Debounce_Modify(MOTION_SENSOR_EINT_NO, SENSOR_SW_DEBOUNCE_TIME);
    EINT_UnMask(MOTION_SENSOR_EINT_NO);

    if(!OTA_track_is_timer_run(MOTION_SENSOR_DATA_TIMER_ID))
    {
        OTA_tr_start_timer(MOTION_SENSOR_DATA_TIMER_ID, 10000, track_sensor_da213_data_self_test);
    }
}


/******************************************************************************
 *  Function    -  track_sensor_da213_get_xyz
 *
 *  Purpose     -  读取当前XYZ数值
 *
 *  Description -  2g /12bit
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_sensor_da213_get_xyz(short*x, short*y, short*z)
{

}

/******************************************************************************
 *  Function    -  track_sensor_da213_reset_int
 *
 *  Purpose     -  清除中断标志
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_sensor_da213_reset_int(void)
{
    //unsigned char fint;
    //sensor_da213_read(0x39, &fint, 1);
}

/******************************************************************************
 *  Function    -  track_sensor_da213_eint_hisr
 *
 *  Purpose     -  震动高电平响应
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 07-09-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_sensor_da213_eint_hisr(void)
{
    kal_uint8 status;
    
    sensor_da213_read(0x09, &status, 1); 
    OTA_LOGC(L_DRV, L_V1, "震动 status=%02X", status);

    if(status==0x24)//tap+active
    {
        OTA_track_drv_driving_record_send_msg(1);//1=碰撞
    }
    if(OTA_track_sensor_config_data->sensor_callback != NULL)
    {
        OTA_track_drv_sensor_interrupt_send_msg();
    }
}

/******************************************************************************
 *  Function    -  track_get_sensor_da213_id
 *
 *  Purpose     -  读取sensor ID
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint8 track_get_sensor_da213_id(void)
{
    return da213_chip_id;
}


/******************************************************************************
 *  Function    -  track_sensor_da213_calibration
 *
 *  Purpose     - sensor 三轴加速度初值校准
 *
 *  Description -  平放
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 25-11-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_sensor_da213_calibration(void)
{

}


/******************************************************************************
 *  Function    -  track_sensor_da213_on
 *
 *  Purpose     -  开启sensor 检测
 *
 *  Description -  motion_threshold  [in] : sensor 检测灵敏度
 *                       FuncPtr  [in] : sensor 触发回调函数
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_sensor_da213_on(kal_uint8 threshold , FuncPtr callback)
{
    OTA_LOGD(L_DRV, L_V1, "threshold=%d", threshold);

    if((threshold <= 0) || (callback == NULL))
    {
        return;
    }

    OTA_track_sensor_config_data->sensor_threshold = threshold;
    OTA_track_sensor_config_data->sensor_callback = callback;

    sensor_da213_on();

}

/******************************************************************************
 *  Function    -  track_sensor_da213_off
 *
 *  Purpose     -  关闭sensor 检测
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_sensor_da213_off(void)
{
    OTA_tr_stop_timer(MOTION_SENSOR_DATA_TIMER_ID);
    OTA_tr_stop_timer(MOTION_SENSOR_RESET_TIMER_ID);
    sensor_da213_pwr_down();
}

/******************************************************************************
 *  Function    -  track_sensor_da213_type_check
 *
 *  Purpose     -  是否为da213
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 26-02-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_bool track_sensor_da213_type_check(void)
{
    kal_uint8 da213_check = 0XFF;

    ms_i2c_configure(DA213_I2C_SLAVE_ADDR, MS_DELAY_TIME);
    sensor_da213_read(0x01, &da213_check, 1);
    
    OTA_LOGD(L_DRV, L_V1, "sensor id=%02X", da213_check);
    if(da213_check == 0x13)
    {
        return KAL_TRUE;
    }
    else
    {
        return KAL_FALSE;
    }
}

#endif /* __TRACK_SENSOR__ */

#endif /* __MIRA_DA213__ */
