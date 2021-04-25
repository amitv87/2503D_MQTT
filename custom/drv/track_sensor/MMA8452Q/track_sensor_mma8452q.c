/******************************************************************************
 * track_motion_sensor_afa750.c - 
 * 
 * Copyright .
 * 
 * DESCRIPTION: - 
 *        MMA8542Q Sensor ���� 
 * 
 * modification history
 * --------------------
 * v1.0   2012-12-14,  chengjun create this file
 * 
 ******************************************************************************/

#if defined (__FREESCALE_MMA8452Q__)

#include "kal_release.h"
#include "custom_config.h"
#include "gpio_sw.h"
#include "drv_comm.h"
#include "eint.h"
#include "motion_sensor.h"
#include "track_sensor_iic.h"
#include "track_sensor_mma8452q.h"
#include "kal_trace.h"
#include "track_os_timer.h"
#include "track_drv_sensor.h"
#include "ell_os.h"


static kal_uint8 mma8452q_chip_id = 0XFF;


kal_bool sensor_mma8452q_write_byte(kal_uint8 ucBufferIndex, kal_uint8 pucData)
{
	return ms_i2c_send(MMA8452Q_I2C_SLAVE_ADDR,ucBufferIndex,&pucData,1);
}


#define sensor_mma8452q_read(a,b,c) ms_i2c_receive(MMA8452Q_I2C_SLAVE_ADDR,a,b,c)


#if defined (__TRACK_SENSOR__)
#define  MMA8452Q_REG_CTRL_STANDBY_MODE  0x10
#define  MMA8452Q_REG_CTRL_ACTIVE_MODE  0x11

#define  MMA8452Q_REG_15_DATA  0xE8
#define  MMA8452Q_REG_18_DATA  0x02 
#define  MMA8452Q_REG_2C_DATA  0x02 
#define  MMA8452Q_REG_2D_DATA  0x04 
#define  MMA8452Q_REG_2E_DATA  0x04 


/******************************************************************************
 *
 *     Sensor �Զ��岿��
 *
 ******************************************************************************/
static void  sensor_mma8452q_init(void)
{
    ms_i2c_configure(MMA8452Q_I2C_SLAVE_ADDR, MS_DELAY_TIME); 

    sensor_mma8452q_read(0x0D, &mma8452q_chip_id, 1);
    //��ȡоƬID���ж�Ӳ���Ƿ�����

    sensor_mma8452q_write_byte(0x2A, MMA8452Q_REG_CTRL_STANDBY_MODE);
    //standby
    //ODR 200Hz
    
    sensor_mma8452q_write_byte(0x15, MMA8452Q_REG_15_DATA);
    //�ж�״̬����
    //XYZ ����Motion ģʽ

    sensor_mma8452q_write_byte(0x17, OTA_track_sensor_config_data->sensor_threshold);
    //������
    
    sensor_mma8452q_write_byte(0x18, MMA8452Q_REG_18_DATA);  
    //�����Чʱ�����
    
    sensor_mma8452q_write_byte(0x2C, MMA8452Q_REG_2C_DATA);
    //H-active
    
    sensor_mma8452q_write_byte(0x2D, MMA8452Q_REG_2D_DATA);
    //Motion mode enable
    
    sensor_mma8452q_write_byte(0x2E, MMA8452Q_REG_2E_DATA);
    //interrupt to INT1 (��Ӳ�����)
    
    sensor_mma8452q_write_byte(0x2A, MMA8452Q_REG_CTRL_ACTIVE_MODE);
    //active mode 
}


/******************************************************************************
 *  Function    -  sensor_mma8452q_on
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
static void sensor_mma8452q_on(void)
{
    sensor_mma8452q_init();
    track_sensor_mma8452q_interrput_configuration();
}

/******************************************************************************
 *  Function    -  sensor_afa750_pwr_down
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
static void sensor_mma8452q_pwr_down(void)
{
    sensor_mma8452q_write_byte(0x2A, MMA8452Q_REG_CTRL_STANDBY_MODE);
}

/******************************************************************************
 *  Function    -  track_sensor_mma8452q_reset
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
static void track_sensor_mma8452q_reset(void)
{
    OTA_LOGD(L_DRV, L_V1, "sensor mma8452Q reset");
    sensor_mma8452q_pwr_down();
    OTA_tr_start_timer(MOTION_SENSOR_RESET_TIMER_ID,  10000, sensor_mma8452q_on);
}

/******************************************************************************
 *  Function    -  track_sensor_mma8452q_data_self_test
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
static void track_sensor_mma8452q_data_self_test(void)
{
    unsigned char reg15,reg18,reg2c,reg2d,reg2e,threshold=0;
        
    sensor_mma8452q_read(0x15, &reg15,1);
    sensor_mma8452q_read(0x18, &reg18,1);
    sensor_mma8452q_read(0x2C, &reg2c,1);
    sensor_mma8452q_read(0x2D, &reg2d,1);
    sensor_mma8452q_read(0x2E, &reg2e,1);
    sensor_mma8452q_read(0x17, &threshold,1);
        
    if ((reg15!=MMA8452Q_REG_15_DATA )||(reg18!=MMA8452Q_REG_18_DATA)\
        ||(reg2c!=MMA8452Q_REG_2C_DATA)||(reg2d!=MMA8452Q_REG_2D_DATA)\
        ||(reg2e!=MMA8452Q_REG_2E_DATA)||(threshold!=OTA_track_sensor_config_data->sensor_threshold) )
    {
        OTA_LOGD(L_DRV, L_V1, "Sensor error (0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X) "
            "!=[0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X]", \
            reg15,reg18,reg2c,reg2d,reg2e,threshold,\
            MMA8452Q_REG_15_DATA,MMA8452Q_REG_18_DATA,MMA8452Q_REG_2C_DATA,\
            MMA8452Q_REG_2D_DATA,MMA8452Q_REG_2E_DATA,OTA_track_sensor_config_data->sensor_threshold);

        OTA_tr_stop_timer(MOTION_SENSOR_DATA_TIMER_ID);
        OTA_tr_start_timer(MOTION_SENSOR_RESET_TIMER_ID,  3000, track_sensor_mma8452q_reset);
    }
    OTA_tr_start_timer(MOTION_SENSOR_DATA_TIMER_ID, 60000, track_sensor_mma8452q_data_self_test);

}


/******************************************************************************
 *  Function    -  track_sensor_mma8452q_interrput_configuration
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
static void track_sensor_mma8452q_interrput_configuration(void)
{
    OTA_LOGD(L_DRV, L_V8, "Sensor eint  reg H");
    EINT_Mask(MOTION_SENSOR_EINT_NO);
    EINT_Registration(MOTION_SENSOR_EINT_NO, KAL_TRUE, 1, track_sensor_mma8452q_eint_hisr, KAL_TRUE);
    EINT_Set_Sensitivity(MOTION_SENSOR_EINT_NO, LEVEL_SENSITIVE);
    EINT_SW_Debounce_Modify(MOTION_SENSOR_EINT_NO, SENSOR_SW_DEBOUNCE_TIME);
    EINT_UnMask(MOTION_SENSOR_EINT_NO);

    if(!OTA_track_is_timer_run(MOTION_SENSOR_DATA_TIMER_ID))
    {
        OTA_tr_start_timer(MOTION_SENSOR_DATA_TIMER_ID, 10000, track_sensor_mma8452q_data_self_test);
    }
}


/******************************************************************************
 *  Function    -  track_sensor_mma8452q_get_xyz
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
void track_sensor_mma8452q_get_xyz(short*x, short*y, short*z)
{
    kal_uint8 Gsensor_Rx_Buffer[6];
    kal_uint16 tpm1=0;
    short Temp = 0;
        
    if(!sensor_mma8452q_read(0x01, Gsensor_Rx_Buffer, 6)) 
    {
            return;     //IIC failed
    }

    //LOGH(L_DRV, L_V1,Gsensor_Rx_Buffer, 6);

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

    
}

/******************************************************************************
 *  Function    -  track_sensor_mma8452q_reset_int
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
static void track_sensor_mma8452q_reset_int(void)
{
    unsigned char int_source,mt_src;

    sensor_mma8452q_read(0x0C, &int_source, 1);
    sensor_mma8452q_read(0x16, &mt_src, 1);
    
    OTA_LOGD(L_DRV, L_V8, "R0C=0x%0.2X,R16=0x%0.2X",int_source,mt_src);     
}

/******************************************************************************
 *  Function    -  track_sensor_mma8452q_eint_hisr
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
static void track_sensor_mma8452q_eint_hisr(void)
{
    if(OTA_track_sensor_config_data->sensor_callback != NULL)
    {
        OTA_track_drv_sensor_interrupt_send_msg();
    }

    OTA_LOGC(L_DRV, L_V6, "sensor mma8452Q ��");     
    track_sensor_mma8452q_reset_int();

}


/******************************************************************************
 *  Function    -  track_get_sensor_mma8452q_id
 * 
 *  Purpose     -  ��ȡsensor ID
 * 
 *  Description -  ���ֵ��оƬSA0  �ӿڵ�ѹ�й� 
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 18-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint8 track_get_sensor_mma8452q_id(void)
{
    return mma8452q_chip_id;
}

  
/******************************************************************************
 *  Function    -  track_sensor_mma8452q_calibration
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
void track_sensor_mma8452q_calibration(void)
{
    OTA_LOGC(L_DRV, L_V4, "sensor calibration ok");
}



/******************************************************************************
 *  Function    -  track_sensor_mma8452q_on
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
void track_sensor_mma8452q_on(kal_uint8 threshold , FuncPtr callback)
{
    LOGD(L_DRV, L_V7, "threshold=%d", threshold);

    if((threshold <= 0) || (callback == NULL))
    {
        return;
    }

    track_sensor_config_data.sensor_threshold = threshold;
    track_sensor_config_data.sensor_callback = callback;

    sensor_mma8452q_on();

}

/******************************************************************************
 *  Function    -  track_sensor_mma8452q_off
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
void track_sensor_mma8452q_off(void)
{
    track_stop_timer(MOTION_SENSOR_DATA_TIMER_ID);
    track_stop_timer(MOTION_SENSOR_RESET_TIMER_ID);   
    sensor_mma8452q_pwr_down();
}


/******************************************************************************
 *  Function    -  track_sensor_mma8452q_type_check
 * 
 *  Purpose     -  �Ƿ�ΪMMA8452Q
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 26-02-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_bool track_sensor_mma8452q_type_check(void)
{
    kal_uint8 mma8452q_check;
    
    ms_i2c_configure(MMA8452Q_I2C_SLAVE_ADDR, MS_DELAY_TIME); 

    sensor_mma8452q_read(0x0D, &mma8452q_check, 1);

    if(mma8452q_check==0x2A)
    {
        return KAL_TRUE;
    }
    else
    {
        return KAL_FALSE;
    }
}

#endif /* __TRACK_SENSOR__ */

#endif /* __FREESCALE_MMA8452Q__ */
