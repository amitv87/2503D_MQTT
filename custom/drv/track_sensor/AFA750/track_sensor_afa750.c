/******************************************************************************
 * track_motion_sensor_afa750.c - 
 * 
 * Copyright .
 * 
 * DESCRIPTION: - 
 *        AFA750 Sensor 驱动 
 * 
 * modification history
 * --------------------
 * v1.0   2012-12-14,  chengjun create this file
 * 
 ******************************************************************************/

#if defined (__MICRO_AFA750__)

#include "kal_release.h"
#include "custom_config.h"
#include "gpio_sw.h"
#include "drv_comm.h"
#include "eint.h"
#include "motion_sensor.h"
#include "track_sensor_iic.h"
#include "track_sensor_afa750.h"
#include "kal_trace.h"
#include "track_os_timer.h"
#include "track_drv_sensor.h"
#include "ell_os.h"

static kal_uint8 afa750_chip_id = 0XFF;


kal_bool sensor_afa750_write_byte(kal_uint8 ucBufferIndex, kal_uint8 pucData)
{
	return ms_i2c_send(AFA750_I2C_SLAVE_ADDR,ucBufferIndex,&pucData,1);
}


#define sensor_afa750_read(a,b,c) ms_i2c_receive(AFA750_I2C_SLAVE_ADDR,a,b,c)


#if defined (__TRACK_SENSOR__)

#define AFA750_PWR_NORMAL_MODE 0x00
#define AFA750_PWR_POWER_DOWN_MODE 0x02

#define  AFA750_REG_01_DATA  0xEF
#define  AFA750_REG_02_DATA  0x00 
#define  AFA750_REG_04_DATA  0x0C 
#define  AFA750_REG_05_DATA  0x00 
#define  AFA750_REG_08_DATA  0x1B 
#define  AFA750_REG_09_DATA  0x40 
#define  AFA750_REG_0B_DATA  0xBF 
#define  AFA750_REG_23_DATA  0x01 


/******************************************************************************
 *
 *     Sensor 自定义部分
 *
 ******************************************************************************/
static void  sensor_afa750_init(void)
{
    unsigned char tmp=0;
    
    ms_i2c_configure(AFA750_I2C_SLAVE_ADDR, MS_DELAY_TIME); 

    sensor_afa750_write_byte(0x03,AFA750_PWR_POWER_DOWN_MODE);
    //先关机

    sensor_afa750_read(0x37, &afa750_chip_id, 1);
    //读取芯片ID，判断硬件是否正常

#if 0
    sensor_afa750_write_byte(0x2E, 0X01);
    //允许温度转换
    
    sensor_afa750_read(0x2F, &tmp, 1);
    //读温度
#endif

    sensor_afa750_write_byte(0x01,AFA750_REG_01_DATA);
    //bit4-INT1 active high
    //bit3-12bit
    
    sensor_afa750_write_byte(0x02,AFA750_REG_02_DATA);
    //2g 量程
    
    sensor_afa750_write_byte(0x04,AFA750_REG_04_DATA);
    //trigger
    
    sensor_afa750_write_byte(0x05,AFA750_REG_05_DATA);
    //输出数据频率400Hz
    
    sensor_afa750_write_byte(0x08,AFA750_REG_08_DATA);
    //INT2 和 SD0/SA0 两个Pull-up disable
    
    sensor_afa750_write_byte(0x21,OTA_track_sensor_config_data->sensor_threshold); 
    //TAP_THRE

    sensor_afa750_write_byte(0x23,AFA750_REG_23_DATA); 
    //TAP_HYST
    
    sensor_afa750_write_byte(0x0B,AFA750_REG_0B_DATA); 
    //tap INT signal appear at INT1

    sensor_afa750_write_byte(0x09,AFA750_REG_09_DATA);
    //tap interrupt enable  
    
    sensor_afa750_write_byte(0x03,AFA750_PWR_NORMAL_MODE);
    //供电模式，标准模式开始工作

}


/******************************************************************************
 *  Function    -  sensor_afa750_on
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
static void sensor_afa750_on(void)
{
    sensor_afa750_init();
    track_sensor_afa750_interrput_configuration();
}

/******************************************************************************
 *  Function    -  sensor_afa750_pwr_down
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
static void sensor_afa750_pwr_down(void)
{
    sensor_afa750_write_byte(0x03,AFA750_PWR_POWER_DOWN_MODE);
}

/******************************************************************************
 *  Function    -  track_sensor_afa750_reset
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
static void track_sensor_afa750_reset(void)
{
    //OTA_LOGD(L_DRV, L_V1, "sensor afa750 reset");
    sensor_afa750_pwr_down();
    OTA_tr_start_timer(MOTION_SENSOR_RESET_TIMER_ID,  10000, sensor_afa750_on);
}

/******************************************************************************
 *  Function    -  track_sensor_afa750_data_self_test
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
static void track_sensor_afa750_data_self_test(void)
{
    unsigned char reg01,reg02,reg04,reg05,reg08,reg09,reg0B,reg23,threshold=0;
        
    sensor_afa750_read(0x01, &reg01,1);
    sensor_afa750_read(0x02, &reg02,1);
    sensor_afa750_read(0x04, &reg04,1);
    sensor_afa750_read(0x05, &reg05,1);
    sensor_afa750_read(0x08, &reg08,1);
    sensor_afa750_read(0x09, &reg09,1);
    sensor_afa750_read(0x0B, &reg0B,1);
    sensor_afa750_read(0x23, &reg23,1);
    sensor_afa750_read(0x21, &threshold,1);
        
    if ((reg01!=AFA750_REG_01_DATA )||(reg02!=AFA750_REG_02_DATA)\
        ||(reg04!=AFA750_REG_04_DATA)||(reg05!=AFA750_REG_05_DATA)\
        ||(reg08!=AFA750_REG_08_DATA)||(reg09!=AFA750_REG_09_DATA)\
        ||(reg0B!=AFA750_REG_0B_DATA)||(reg23!=AFA750_REG_23_DATA)\
        ||(threshold!= OTA_track_sensor_config_data->sensor_threshold) )
    {
        /*OTA_LOGD(L_DRV, L_V1, "Sensor error (0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X) "
            "!=[0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X]", \
            reg01,reg02,reg04,reg05,reg08,reg09,reg0B,reg23,threshold,\
            AFA750_REG_01_DATA,AFA750_REG_02_DATA,AFA750_REG_04_DATA,AFA750_REG_05_DATA,\
            AFA750_REG_08_DATA,AFA750_REG_09_DATA,AFA750_REG_0B_DATA,AFA750_REG_23_DATA,\
            OTA_track_sensor_config_data->sensor_threshold);*/

        OTA_tr_stop_timer(MOTION_SENSOR_DATA_TIMER_ID);
        OTA_tr_start_timer(MOTION_SENSOR_RESET_TIMER_ID,  3000, track_sensor_afa750_reset);
    }
    OTA_tr_start_timer(MOTION_SENSOR_DATA_TIMER_ID, 60000, track_sensor_afa750_data_self_test);

}


/******************************************************************************
 *  Function    -  track_sensor_afa750_interrput_configuration
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
static void track_sensor_afa750_interrput_configuration(void)
{
    //OTA_LOGD(L_DRV, L_V8, "Sensor eint  reg H");
    EINT_Mask(MOTION_SENSOR_EINT_NO);
    EINT_Registration(MOTION_SENSOR_EINT_NO, KAL_TRUE, 1, track_sensor_afa750_eint_hisr, KAL_TRUE);
    EINT_Set_Sensitivity(MOTION_SENSOR_EINT_NO, LEVEL_SENSITIVE);
    EINT_SW_Debounce_Modify(MOTION_SENSOR_EINT_NO, SENSOR_SW_DEBOUNCE_TIME);
    EINT_UnMask(MOTION_SENSOR_EINT_NO);

    if(!OTA_track_is_timer_run(MOTION_SENSOR_DATA_TIMER_ID))
    {
        OTA_tr_start_timer(MOTION_SENSOR_DATA_TIMER_ID, 10000, track_sensor_afa750_data_self_test);
    }
}


/******************************************************************************
 *  Function    -  track_sensor_afa750_get_xyz
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
void track_sensor_afa750_get_xyz(short*x, short*y, short*z)
{
#if 0
    kal_uint8 Gsensor_Rx_Buffer[6];
    kal_uint16 tpm1=0;
    short Temp = 0;
        
    if(!sensor_afa750_read(AFA750_xyz_REG, Gsensor_Rx_Buffer, 6)) 
    {
            return;
    }

    tpm1=(Gsensor_Rx_Buffer[5]<<8)|Gsensor_Rx_Buffer[4]; 
    if ((Gsensor_Rx_Buffer[5]&0x80)!=0) 
    {
        Temp=-( (tpm1-1)^0xFFFF);
    }
    else
    {
        Temp=tpm1;
    }
    OTA_LOGD(L_DRV, L_V1, "Z=%d",Temp);

    *x = (Gsensor_Rx_Buffer[1]<<8)|Gsensor_Rx_Buffer[0];
    *y = (Gsensor_Rx_Buffer[3]<<8)|Gsensor_Rx_Buffer[2];
    *z = (Gsensor_Rx_Buffer[5]<<8)|Gsensor_Rx_Buffer[4]; 
#endif /* 0 */

    
}

/******************************************************************************
 *  Function    -  track_sensor_afa750_reset_int
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
static void track_sensor_afa750_reset_int(void)
{
    unsigned char F1_int,F2_int;
    sensor_afa750_read(0x0D, &F1_int, 1);
    sensor_afa750_read(0x0E, &F2_int, 1);
    //OTA_LOGD(L_DRV, L_V8, "R_D=0x%0.2X,R_E=0x%0.2X",F1_int,F2_int);    
}

/******************************************************************************
 *  Function    -  track_sensor_afa750_eint_hisr
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
static void track_sensor_afa750_eint_hisr(void)
{
    if(OTA_track_sensor_config_data->sensor_callback != NULL)
    {
        OTA_track_drv_sensor_interrupt_send_msg();
    }

    //OTA_LOGC(L_DRV, L_V6, "sensor afa750 震动 "); 
    track_sensor_afa750_reset_int();
}

/******************************************************************************
 *  Function    -  track_get_sensor_afa750_id
 * 
 *  Purpose     -  读取sensor ID
 * 
 *  Description -  这个值和芯片SA0  接口电压有关 
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint8 track_get_sensor_afa750_id(void)
{
    return afa750_chip_id;
}

    
/******************************************************************************
 *  Function    -  track_sensor_afa750_calibration
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
void track_sensor_afa750_calibration(void)
{
    //OTA_LOGC(L_DRV, L_V4, "sensor calibration ok -[Bank]");
}


/******************************************************************************
 *  Function    -  track_sensor_afa750_on
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
void track_sensor_afa750_on(kal_uint8 threshold , FuncPtr callback)
{
    //OTA_LOGD(L_DRV, L_V7, "threshold=%d", threshold);

    if((threshold <= 0) || (callback == NULL))
    {
        return;
    }

    OTA_track_sensor_config_data->sensor_threshold = threshold;
    OTA_track_sensor_config_data->sensor_callback = callback;

    sensor_afa750_on();

}

/******************************************************************************
 *  Function    -  track_sensor_afa750_off
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
void track_sensor_afa750_off(void)
{
    OTA_tr_stop_timer(MOTION_SENSOR_DATA_TIMER_ID);
    OTA_tr_stop_timer(MOTION_SENSOR_RESET_TIMER_ID);   
    sensor_afa750_pwr_down();
}

/******************************************************************************
 *  Function    -  track_sensor_afa750_type_check
 * 
 *  Purpose     -  是否为AFA750
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 26-02-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_bool track_sensor_afa750_type_check(void)
{
    kal_uint8 afa750_check = 0XFF;
    
    ms_i2c_configure(AFA750_I2C_SLAVE_ADDR, MS_DELAY_TIME);
    sensor_afa750_write_byte(0x03,AFA750_PWR_POWER_DOWN_MODE);
    sensor_afa750_read(0x37, &afa750_check, 1);

    if(afa750_check==0x3C)
    {
        return KAL_TRUE;
    }
    else
    {
        return KAL_FALSE;
    }
}

#endif /* __TRACK_SENSOR__ */

#endif /* __MICRO_AFA750__ */
