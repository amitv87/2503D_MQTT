/******************************************************************************
 * track_sensor_kxtj31057.c - 
 * 
 * Copyright .
 * 
 * DESCRIPTION: - 
 *        KXTJ3-1057/KXTJ3-1058 (ͬһ��оƬ��ͬ�ƺ�) gSensor���� 
 * 
 * modification history
 * --------------------
 * v1.0   2016-12-09,  chengjun create this file
 * 
 ******************************************************************************/
#if defined (__KIONIX_KXTJ31057__)

#include "kal_release.h"
#include "custom_config.h"
#include "gpio_sw.h"
#include "drv_comm.h"
#include "eint.h"
#include "motion_sensor.h"
#include "track_sensor_iic.h"
#include "track_sensor_kxtj31057.h"
#include "kal_trace.h"
#include "track_os_timer.h"
#include "track_drv_sensor.h"
#include "ell_os.h"

static kal_uint8 kxtj31057_chip_id = 0XFF;

static void track_sensor_kxtj31057_reset_int(void);

kal_bool sensor_kxtj31057_write_byte(kal_uint8 ucBufferIndex, kal_uint8 pucData)
{
    return ms_i2c_send(KXTJ31057_I2C_SLAVE_ADDR, ucBufferIndex, &pucData, 1);
}


#define sensor_kxtj31057_read(a,b,c) ms_i2c_receive(KXTJ31057_I2C_SLAVE_ADDR,a,b,c)


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
 *     Sensor �Զ��岿��
 *
 ******************************************************************************/
static void  sensor_kxtj31057_init(void)
{
    unsigned char threshold = 0;

    ms_i2c_configure(KXTJ31057_I2C_SLAVE_ADDR, MS_DELAY_TIME);

    sensor_kxtj31057_read(0x0F, &kxtj31057_chip_id, 1);   //��ȡоƬID

    sensor_kxtj31057_write_byte(0x1B, 0x00); //standby mode PC1=0
    sensor_kxtj31057_write_byte(0x21, 0x02); //ODR=50HZ
    sensor_kxtj31057_write_byte(0x1D, 0x06); //wakeup ODR=50HZ
    sensor_kxtj31057_write_byte(0x17, 0x3F); //enable the 3-axis detect
    sensor_kxtj31057_write_byte(0x29, 0x01); //wakeup time 02��������Բ���BMA250E

    threshold=OTA_track_sensor_config_data->sensor_threshold;
    sensor_kxtj31057_write_byte(0x6A, (threshold>>4)&0xFF); //wakeup threshold 6A+6B
    sensor_kxtj31057_write_byte(0x6B, ((threshold&0x0F)<<4)); //wakeup threshold 6A+6B
      
    sensor_kxtj31057_write_byte(0x1E, 0x30); //enable interrupt-H
    sensor_kxtj31057_write_byte(0x1B, 0xC2); //operate mode,PC1=1,wakeup enable,2g
}


/******************************************************************************
 *  Function    -  sensor_kxtj31057_on
 *
 *  Purpose     -  sensor �Ĵ������ж�����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void sensor_kxtj31057_on(void)
{
    sensor_kxtj31057_init();
    track_sensor_kxtj31057_interrput_configuration();
}

/******************************************************************************
 *  Function    -  sensor_kxtj31057_pwr_down
 *
 *  Purpose     -  �ر�sensor
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 05-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void sensor_kxtj31057_pwr_down(void)
{
    track_sensor_kxtj31057_reset_int();
    track_drv_delay_1ms(5);
    sensor_kxtj31057_write_byte(0x1B, 0x00);//standby
}

/******************************************************************************
 *  Function    -  track_sensor_kxtj31057_reset
 *
 *  Purpose     -  sensor ��λ
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 05-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_sensor_kxtj31057_reset(void)
{
    //OTA_LOGD(L_DRV, L_V1, "sensor kxtj31057 reset");
    sensor_kxtj31057_pwr_down();
    OTA_tr_start_timer(MOTION_SENSOR_RESET_TIMER_ID,  10000, sensor_kxtj31057_on);
}

/******************************************************************************
 *  Function    -  track_sensor_kxtj31057_data_self_test
 *
 *  Purpose     -  sensor �����쳣���
 *
 *  Description - ��ʱ�����Լ죬���쳣������
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_sensor_kxtj31057_data_self_test(void)
{
    unsigned char reg;

    //sensor_kxtj31057_write_byte(0x1E, 0x30); //enable interrupt-H
    sensor_kxtj31057_read(0x1E, &reg, 1);

    if(reg != 0x30)
    {
        OTA_LOGD(L_DRV, L_V1, "Sensor error (0x%0.2X-0x30",reg);

        OTA_tr_stop_timer(MOTION_SENSOR_DATA_TIMER_ID);
        OTA_tr_start_timer(MOTION_SENSOR_RESET_TIMER_ID,  3000, track_sensor_kxtj31057_reset);
    }
    OTA_tr_start_timer(MOTION_SENSOR_DATA_TIMER_ID, 60000, track_sensor_kxtj31057_data_self_test);

}


/******************************************************************************
 *  Function    -  track_sensor_kxtj31057_interrput_configuration
 *
 *  Purpose     -  ע��ߵ�ƽ(  ��) ��Ӧ
 *
 *  Description -
 *
 * 1���Զ����־
 * 2���������
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_sensor_kxtj31057_interrput_configuration(void)
{
    OTA_LOGD(L_DRV, L_V1, "Sensor eint reg H");
    EINT_Mask(MOTION_SENSOR_EINT_NO);
    EINT_Registration(MOTION_SENSOR_EINT_NO, KAL_TRUE, 1, track_sensor_kxtj31057_eint_hisr, KAL_TRUE);
    EINT_Set_Sensitivity(MOTION_SENSOR_EINT_NO, LEVEL_SENSITIVE);
    EINT_SW_Debounce_Modify(MOTION_SENSOR_EINT_NO, SENSOR_SW_DEBOUNCE_TIME);
    EINT_UnMask(MOTION_SENSOR_EINT_NO);

    if(!OTA_track_is_timer_run(MOTION_SENSOR_DATA_TIMER_ID))
    {
        OTA_tr_start_timer(MOTION_SENSOR_DATA_TIMER_ID, 10000, track_sensor_kxtj31057_data_self_test);
    }
}


/******************************************************************************
 *  Function    -  track_sensor_kxtj31057_get_xyz
 *
 *  Purpose     -  ��ȡ��ǰXYZ��ֵ
 *
 *  Description -  2g /12bit
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_sensor_kxtj31057_get_xyz(short*x, short*y, short*z)
{

}

/******************************************************************************
 *  Function    -  track_sensor_kxtj31057_reset_int
 *
 *  Purpose     -  ����жϱ�־
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_sensor_kxtj31057_reset_int(void)
{
    unsigned char fint;
    sensor_kxtj31057_read(0x1A, &fint, 1);
}

/******************************************************************************
 *  Function    -  track_sensor_kxtj31057_eint_hisr
 *
 *  Purpose     -  �𶯸ߵ�ƽ��Ӧ
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 07-09-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_sensor_kxtj31057_eint_hisr(void)
{
    if(OTA_track_sensor_config_data->sensor_callback != NULL)
    {
        OTA_track_drv_sensor_interrupt_send_msg();
    }

    OTA_LOGC(L_DRV, L_V1, "sensor �� ");
    track_sensor_kxtj31057_reset_int();
}

/******************************************************************************
 *  Function    -  track_get_sensor_kxtj31057_id
 *
 *  Purpose     -  ��ȡsensor ID
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint8 track_get_sensor_kxtj31057_id(void)
{
    return kxtj31057_chip_id;
}


/******************************************************************************
 *  Function    -  track_sensor_kxtj31057_calibration
 *
 *  Purpose     - sensor ������ٶȳ�ֵУ׼
 *
 *  Description -  ƽ��
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 25-11-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_sensor_kxtj31057_calibration(void)
{

}


/******************************************************************************
 *  Function    -  track_sensor_kxtj31057_on
 *
 *  Purpose     -  ����sensor ���
 *
 *  Description -  motion_threshold  [in] : sensor ���������
 *                       FuncPtr  [in] : sensor �����ص�����
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_sensor_kxtj31057_on(kal_uint8 threshold , FuncPtr callback)
{
    OTA_LOGD(L_DRV, L_V1, "threshold=%d", threshold);

    if((threshold <= 0) || (callback == NULL))
    {
        return;
    }

    OTA_track_sensor_config_data->sensor_threshold = threshold;
    OTA_track_sensor_config_data->sensor_callback = callback;

    sensor_kxtj31057_on();

}

/******************************************************************************
 *  Function    -  track_sensor_kxtj31057_off
 *
 *  Purpose     -  �ر�sensor ���
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_sensor_kxtj31057_off(void)
{
    OTA_tr_stop_timer(MOTION_SENSOR_DATA_TIMER_ID);
    OTA_tr_stop_timer(MOTION_SENSOR_RESET_TIMER_ID);
    sensor_kxtj31057_pwr_down();
}

/******************************************************************************
 *  Function    -  track_sensor_kxtj31057_type_check
 *
 *  Purpose     -  �Ƿ�ΪKXTJ31057
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 26-02-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_bool track_sensor_kxtj31057_type_check(void)
{
    kal_uint8 kxtj31057_check = 0XFF;

    ms_i2c_configure(KXTJ31057_I2C_SLAVE_ADDR, MS_DELAY_TIME);
    sensor_kxtj31057_read(0x0F, &kxtj31057_check, 1);
    OTA_LOGD(L_DRV, L_V1, "sensor id=%02X", kxtj31057_check);
    if(kxtj31057_check == 0x35)
    {
        return KAL_TRUE;
    }
    else
    {
        return KAL_FALSE;
    }
}

#endif /* __TRACK_SENSOR__ */

#endif /* __KIONIX_KXTJ31057__ */
