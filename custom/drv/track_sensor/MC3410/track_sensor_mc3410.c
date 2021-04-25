
/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 *    motion_sensor_custom.c
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   This Module is for motion sensor driver.
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#if defined(__MCUBE_MC3410__)

#include "kal_release.h"
#include "custom_config.h"
#include "gpio_sw.h"
#include "drv_comm.h"
#include "eint.h"
#include "motion_sensor.h"
#include "track_sensor_iic.h"
#include "track_sensor_mc3410.h"
#include "kal_trace.h"
#include "track_os_timer.h"
#include "track_drv_sensor.h"
#include "ell_os.h"


char sensor_mc3410_buffer[10];

#if defined (__TRACK_SENSOR__)

static kal_uint8 mc3410_chip_id = 0XFF;
static kal_bool sensor_position = KAL_FALSE; /*KAL_FALSE = 终端放置方向错误*/

#define MS_MODE_WAKE_STATUS  0XC1
#define MS_MODE_STANDBY_STATUS  0XC3

#define INTEN_SHAKE_MODE 0XE0
#define INTEN_TAP_MODE  0X04
#define INTEN_ALL_MODE 0XE7

#define MS_INTEN_DATA   0xE4
#define MS_TAPEN_DATA   0XE0
#define MS_TAPP_DATA 0X0C
#define MS_SHAKE_TH_DATA 0X44
#define MS_SHDB_DATA 0X10
#define MS_OUTCFG_DATA   0X33

//#define SENSOR_SAMPLING
//Sensor 数据采集

#if defined (SENSOR_SAMPLING)
#define SAMPLING_INTEN_DATA 0X10
#define SAMPLING_OUTCFG_DATA   0X23
#endif /* SENSOR_SAMPLING */

#endif /* __TRACK_SENSOR__ */

kal_bool sensor_mc3410_write_byte(kal_uint8 ucBufferIndex, kal_uint8 pucData)
{
    return ms_i2c_send(MC3410_I2C_SLAVE_ADDR, ucBufferIndex, &pucData, 1);
}


#define sensor_mc3410_read(a,b,c) ms_i2c_receive(MC3410_I2C_SLAVE_ADDR,a,b,c)

void mc3230_read_xyz(kal_int16 *x, kal_int16 *y, kal_int16 *z)
{
    sensor_mc3410_read(MS_REG_XOUT, sensor_mc3410_buffer, 3);
    *x = sensor_mc3410_buffer[0];
    *y = sensor_mc3410_buffer[1];
    *z = sensor_mc3410_buffer[2];
}

/*It is for mCube time stamp*/
kal_uint32 mc3230_get_milli_time(void)
{
    kal_uint32 time;
    kal_get_time(&time);
    return kal_ticks_to_milli_secs(time);
}

void sensor_mc3410_get_data(kal_uint16 *x_adc, kal_uint16 *y_adc, kal_uint16 *z_adc)
{
    kal_int16 x_temp, y_temp, z_temp;
    kal_int16 x, y, z;
    kal_uint32 time_ms;

    mc3230_read_xyz(&x_temp, &y_temp, &z_temp);
    time_ms = mc3230_get_milli_time(); /*get time stamp*/

    x_temp &= 0xFF;
    y_temp &= 0xFF;
    z_temp &= 0xFF;

    if(x_temp < 128)
        x = x_temp + 127;
    else
    {
        x = x_temp - 128;
    }

    if(y_temp < 128)
        y = y_temp + 127;
    else
    {
        y = y_temp - 128;
    }

    if(z_temp < 128)
        z = z_temp + 127;
    else
    {
        z = z_temp - 128;
    }

#if defined(MOTION_SENSOR_BACK_0)
    *x_adc = x;
    *y_adc = y;
    *z_adc = z;
#elif defined(MOTION_SENSOR_BACK_90)
    *x_adc = y;
    *y_adc = 2 * ACC_0G_X - x;
    *z_adc = z;
#elif defined(MOTION_SENSOR_BACK_180)
    *x_adc = 2 * ACC_0G_X - x;
    *y_adc = 2 * ACC_0G_Y - y;
    *z_adc = z;
#elif defined(MOTION_SENSOR_BACK_270)
    *x_adc = 2 * ACC_0G_Y - y;
    *y_adc = x;
    *z_adc = z;
#elif defined(MOTION_SENSOR_FRONT_0)
    *x_adc = 2 * ACC_0G_X - x;
    *y_adc = y;
    *z_adc = 2 * ACC_0G_Z - z;
#elif defined(MOTION_SENSOR_FRONT_90)
    *x_adc = 2 * ACC_0G_Y - y;
    *y_adc = 2 * ACC_0G_X - x;
    *z_adc = 2 * ACC_0G_Z - z;
#elif defined(MOTION_SENSOR_FRONT_180)
    *x_adc = x;
    *y_adc = 2 * ACC_0G_Y - y;
    *z_adc = 2 * ACC_0G_Z - z;
#elif defined(MOTION_SENSOR_FRONT_270)
    *x_adc = y;
    *y_adc = x;
    *z_adc = 2 * ACC_0G_Z - z;
#endif

#ifdef MS_DBG
    kal_prompt_trace(MOD_ENG, "Motion Sensor Readed and value is x:%d  ,y:%d   ,z:%d  ", *x_adc, *y_adc, *z_adc);
#endif
}


/* Get tilt status*/
void sensor_mc3410_get_status(kal_uint32 *status)
{
    kal_uint8 new_tilt_status;
    kal_uint8 bafr_status;
    kal_uint8 pola_status;
    kal_uint32 time_ms;

    sensor_mc3410_read(MS_REG_TILT, sensor_mc3410_buffer, 1);
    *status = sensor_mc3410_buffer[0];
    new_tilt_status = *status & (TILT_STATUS_TAP_EVENT | TILT_STATUS_DROP_EVENT | TILT_STATUS_SHAKE_EVENT);
    bafr_status = *status & TILT_STATUS_BAFR_MASK;
    pola_status = *status & TILT_STATUS_POLA_MASK;

#if defined(MOTION_SENSOR_BACK_0) //MCUBE_PLACEMENT_TOP_LEFT_DOWN	

    switch(bafr_status)
    {
        case TILT_STATUS_BAFR_FRONT:
            new_tilt_status |= TILT_STATUS_BAFR_FRONT;
            break;

        case TILT_STATUS_BAFR_BACK:
            new_tilt_status |= TILT_STATUS_BAFR_BACK;
            break;
    }
    switch(pola_status)
    {
        case TILT_STATUS_POLA_LEFT:
            new_tilt_status |= TILT_STATUS_POLA_UP;
            break;

        case TILT_STATUS_POLA_RIGHT:
            new_tilt_status |= TILT_STATUS_POLA_DOWN;
            break;

        case TILT_STATUS_POLA_DOWN:
            new_tilt_status |= TILT_STATUS_POLA_LEFT;
            break;

        case TILT_STATUS_POLA_UP:
            new_tilt_status |= TILT_STATUS_POLA_RIGHT;
            break;
    }

#elif defined(MOTION_SENSOR_BACK_90) //MCUBE_PLACEMENT_TOP_LEFT_UP

    switch(bafr_status)
    {
        case TILT_STATUS_BAFR_FRONT:
            new_tilt_status |= TILT_STATUS_BAFR_FRONT;
            break;

        case TILT_STATUS_BAFR_BACK:
            new_tilt_status |= TILT_STATUS_BAFR_BACK;
            break;
    }
    switch(pola_status)
    {
        case TILT_STATUS_POLA_LEFT:
            new_tilt_status |= TILT_STATUS_POLA_LEFT;
            break;

        case TILT_STATUS_POLA_RIGHT:
            new_tilt_status |= TILT_STATUS_POLA_RIGHT;
            break;

        case TILT_STATUS_POLA_DOWN:
            new_tilt_status |= TILT_STATUS_POLA_DOWN;
            break;

        case TILT_STATUS_POLA_UP:
            new_tilt_status |= TILT_STATUS_POLA_UP;
            break;
    }

#elif defined(MOTION_SENSOR_BACK_180)  //MCUBE_PLACEMENT_TOP_RIGHT_UP

    switch(bafr_status)
    {
        case TILT_STATUS_BAFR_FRONT:
            new_tilt_status |= TILT_STATUS_BAFR_FRONT;
            break;

        case TILT_STATUS_BAFR_BACK:
            new_tilt_status |= TILT_STATUS_BAFR_BACK;
            break;
    }
    switch(pola_status)
    {
        case TILT_STATUS_POLA_LEFT:
            new_tilt_status |= TILT_STATUS_POLA_DOWN;
            break;

        case TILT_STATUS_POLA_RIGHT:
            new_tilt_status |= TILT_STATUS_POLA_UP;
            break;

        case TILT_STATUS_POLA_DOWN:
            new_tilt_status |= TILT_STATUS_POLA_RIGHT;
            break;

        case TILT_STATUS_POLA_UP:
            new_tilt_status |= TILT_STATUS_POLA_LEFT;
            break;
    }

#elif defined(MOTION_SENSOR_BACK_270) //MCUBE_PLACEMENT_TOP_RIGH_DOWN

    switch(bafr_status)
    {
        case TILT_STATUS_BAFR_FRONT:
            new_tilt_status |= TILT_STATUS_BAFR_FRONT;
            break;

        case TILT_STATUS_BAFR_BACK:
            new_tilt_status |= TILT_STATUS_BAFR_BACK;
            break;
    }
    switch(pola_status)
    {
        case TILT_STATUS_POLA_LEFT:
            new_tilt_status |= TILT_STATUS_POLA_RIGHT;
            break;

        case TILT_STATUS_POLA_RIGHT:
            new_tilt_status |= TILT_STATUS_POLA_LEFT;
            break;

        case TILT_STATUS_POLA_DOWN:
            new_tilt_status |= TILT_STATUS_POLA_UP;
            break;

        case TILT_STATUS_POLA_UP:
            new_tilt_status |= TILT_STATUS_POLA_DOWN;
            break;
    }

#elif defined(MOTION_SENSOR_FRONT_0) //MCUBE_PLACEMENT_BOTTOM_RIGHT_DOWN

    switch(bafr_status)
    {
        case TILT_STATUS_BAFR_FRONT:
            new_tilt_status |= TILT_STATUS_BAFR_BACK;
            break;

        case TILT_STATUS_BAFR_BACK:
            new_tilt_status |= TILT_STATUS_BAFR_FRONT;
            break;
    }
    switch(pola_status)
    {
        case TILT_STATUS_POLA_LEFT:
            new_tilt_status |= TILT_STATUS_POLA_UP;
            break;

        case TILT_STATUS_POLA_RIGHT:
            new_tilt_status |= TILT_STATUS_POLA_DOWN;
            break;

        case TILT_STATUS_POLA_DOWN:
            new_tilt_status |= TILT_STATUS_POLA_RIGHT;
            break;

        case TILT_STATUS_POLA_UP:
            new_tilt_status |= TILT_STATUS_POLA_LEFT;
            break;
    }

#elif defined(MOTION_SENSOR_FRONT_90)  //MCUBE_PLACEMENT_BOTTOM_RIGHT_UP

    switch(bafr_status)
    {
        case TILT_STATUS_BAFR_FRONT:
            new_tilt_status |= TILT_STATUS_BAFR_BACK;
            break;

        case TILT_STATUS_BAFR_BACK:
            new_tilt_status |= TILT_STATUS_BAFR_FRONT;
            break;
    }
    switch(pola_status)
    {
        case TILT_STATUS_POLA_LEFT:
            new_tilt_status |= TILT_STATUS_POLA_RIGHT;
            break;

        case TILT_STATUS_POLA_RIGHT:
            new_tilt_status |= TILT_STATUS_POLA_LEFT;
            break;

        case TILT_STATUS_POLA_DOWN:
            new_tilt_status |= TILT_STATUS_POLA_DOWN;
            break;

        case TILT_STATUS_POLA_UP:
            new_tilt_status |= TILT_STATUS_POLA_UP;
            break;
    }

#elif defined(MOTION_SENSOR_FRONT_180) //MCUBE_PLACEMENT_BOTTOM_LEFT_UP

    switch(bafr_status)
    {
        case TILT_STATUS_BAFR_FRONT:
            new_tilt_status |= TILT_STATUS_BAFR_BACK;
            break;

        case TILT_STATUS_BAFR_BACK:
            new_tilt_status |= TILT_STATUS_BAFR_FRONT;
            break;
    }
    switch(pola_status)
    {
        case TILT_STATUS_POLA_LEFT:
            new_tilt_status |= TILT_STATUS_POLA_DOWN;
            break;

        case TILT_STATUS_POLA_RIGHT:
            new_tilt_status |= TILT_STATUS_POLA_UP;
            break;

        case TILT_STATUS_POLA_DOWN:
            new_tilt_status |= TILT_STATUS_POLA_LEFT;
            break;

        case TILT_STATUS_POLA_UP:
            new_tilt_status |= TILT_STATUS_POLA_RIGHT;
            break;
    }

#elif defined(MOTION_SENSOR_FRONT_270) //MCUBE_PLACEMENT_BOTTOM_LEFT_DOWN

    switch(bafr_status)
    {
        case TILT_STATUS_BAFR_FRONT:
            new_tilt_status |= TILT_STATUS_BAFR_BACK;
            break;

        case TILT_STATUS_BAFR_BACK:
            new_tilt_status |= TILT_STATUS_BAFR_FRONT;
            break;
    }
    switch(pola_status)
    {
        case TILT_STATUS_POLA_LEFT:
            new_tilt_status |= TILT_STATUS_POLA_LEFT;
            break;

        case TILT_STATUS_POLA_RIGHT:
            new_tilt_status |= TILT_STATUS_POLA_RIGHT;
            break;

        case TILT_STATUS_POLA_DOWN:
            new_tilt_status |= TILT_STATUS_POLA_UP;
            break;

        case TILT_STATUS_POLA_UP:
            new_tilt_status |= TILT_STATUS_POLA_DOWN;
            break;
    }

#endif

    *status = new_tilt_status;
    time_ms = mc3230_get_milli_time(); /*get time stamp*/
    kal_prompt_trace(MOD_ENG, "Motion Sensor Status = : %x, time_stamp = %d   ", new_tilt_status, time_ms);

}



/******************************************************************************
 *
 *  Purpose     -  寄存器配置
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 22-10-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/

static void sensor_mc3410_pwr_up(void)
{
    sensor_mc3410_write_byte(MS_REG_MODE, MS_MODE_STANDBY_STATUS);
    sensor_mc3410_write_byte(MS_REG_MODE, MS_MODE_WAKE_STATUS);
}

static void sensor_mc3410_pwr_down(void)
{
    sensor_mc3410_write_byte(MS_REG_MODE, MS_MODE_STANDBY_STATUS);
}


static void sensor_mc3410_init(void)
{
     ms_i2c_configure(MC3410_I2C_SLAVE_ADDR, MS_DELAY_TIME);

    sensor_mc3410_write_byte(MS_REG_MODE, MS_MODE_STANDBY_STATUS);

    sensor_mc3410_write_byte(MS_REG_MODE, MS_MODE_WAKE_STATUS);

    sensor_mc3410_read(MS_REG_CHIPID, &mc3410_chip_id, 1);
    //读取芯片ID，判断硬件是否正常

    sensor_mc3410_write_byte(MS_REG_MODE, MS_MODE_STANDBY_STATUS);
    //进入待机模式，该模式才允许对寄存器进行写操作

#if 0
    /********************************************************/
    //单shake 模式

    sensor_mc3410_write_byte(0X20, 0X33);
    //64Hz   (-2g~+2g)  10bit

    sensor_mc3410_write_byte(0x2B, 0X44);
    //Shake 阙值

    sensor_mc3410_write_byte(0X0C, 0X1F);
    //Shake 触发抖动

    sensor_mc3410_write_byte(0X06, 0XE0);
    //允许Shake XYZ 三轴检测中断

    /********************************************************/
    //单tap 模式

    sensor_mc3410_write_byte(0x09, 0xE0);
    //允许TAP中断 XYZ 三轴检测中断

    sensor_mc3410_write_byte(0X20, 0X33);
    //64Hz   (-2g~+2g)  10bit

    sensor_mc3410_write_byte(0X0A, 0x0C);
    //最大范围的允许TAP 脉冲中断

    sensor_mc3410_write_byte(0X32, 0X04);
    //TAP 灵敏度

    sensor_mc3410_write_byte(0x06, 0x07);
    //允许TAP中断

    /********************************************************/
#endif

#if defined (SENSOR_SAMPLING)

    sensor_mc3410_write_byte(0x06, SAMPLING_INTEN_DATA);
    //允许GINT 采样中断

    sensor_mc3410_write_byte(0x20, SAMPLING_OUTCFG_DATA);
    //128Hz   (-2g~+2g)  10bit

#else

    sensor_mc3410_write_byte(0x20, MS_OUTCFG_DATA);
    //64Hz   (-2g~+2g)  10bit

    sensor_mc3410_write_byte(0x0A, MS_TAPP_DATA);
    //允许TAP 脉冲个数

    sensor_mc3410_write_byte(0X32, OTA_track_sensor_config_data->sensor_threshold);
    //TAP 灵敏度

    sensor_mc3410_write_byte(0X2B, MS_SHAKE_TH_DATA);
    //Shake 阙值1.5g

    sensor_mc3410_write_byte(0X0C, MS_SHDB_DATA);
    //Shake 触发抖动

    sensor_mc3410_write_byte(0x09, MS_TAPEN_DATA);
    //允许TAP中断 XYZ 三轴检测中断

    sensor_mc3410_write_byte(0x06, MS_INTEN_DATA);
    //允许TAP中断，允许Shake XYZ 三轴检测中断

#endif /* SENSOR_SAMPLING */

    sensor_mc3410_write_byte(MS_REG_MODE, MS_MODE_WAKE_STATUS);
    //进入工作模式，中断高有效

}

/******************************寄存器配置end*****************************/

/******************************************************************************
 *  Function    -  sensor_mc3410_on
 *
 *  Purpose     -  sensor 寄存器和中断配置
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 22-10-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void sensor_mc3410_on(void)
{
    sensor_mc3410_init();
    track_sensor_mc3410_interrput_configuration();
}


/******************************************************************************
 *  Function    -  track_sensor_mc3410_data_self_test
 *
 *  Purpose     -  sensor 工作异常监测
 *
 *  Description - 定时参数自检，若异常则重置
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 07-09-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_sensor_mc3410_data_self_test(void)
{
#if defined (SENSOR_SAMPLING)

    unsigned char inten = 0, outcfg = 0;

    sensor_mc3410_read(0x06, &inten,1);
    sensor_mc3410_read(0x20, &outcfg,1);

    if ((inten!=SAMPLING_INTEN_DATA )||(outcfg!=SAMPLING_OUTCFG_DATA))
    {
        LOGD(L_DRV, L_V1, "Sensor data error (0x%0.2X-0x%0.2X) !=[0x%0.2X-0x%0.2X]", inten, outcfg,SAMPLING_INTEN_DATA,SAMPLING_OUTCFG_DATA);
        tr_stop_timer(MOTION_SENSOR_DATA_TIMER_ID);
        tr_start_timer(MOTION_SENSOR_RESET_TIMER_ID,  3000, track_sensor_mc3410_reset);
    }
    tr_start_timer(MOTION_SENSOR_DATA_TIMER_ID, 60000, track_sensor_mc3410_data_self_test);

#else

    unsigned char outcfg = 0, tapp = 0,threshold=0,shake_th=0,shdb=0,tapen=0,inten=0;

    sensor_mc3410_read(0x20, &outcfg,1);
    sensor_mc3410_read(0x0A, &tapp,1);
    sensor_mc3410_read(0x32, &threshold,1);
    sensor_mc3410_read(0x2B, &shake_th,1);
    sensor_mc3410_read(0x0C, &shdb,1);
    sensor_mc3410_read(0x09, &tapen,1);
    sensor_mc3410_read(0x06, &inten,1);

    if ((outcfg!=MS_OUTCFG_DATA )||(tapp!=MS_TAPP_DATA)
        ||(threshold!=OTA_track_sensor_config_data->sensor_threshold)||(shake_th!=MS_SHAKE_TH_DATA)
        ||(shdb!=MS_SHDB_DATA)||(tapen!=MS_TAPEN_DATA)
        ||(inten!=MS_INTEN_DATA) )
    {
        OTA_LOGD(L_DRV, L_V1, "Sensor error (0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X) !=[0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X-0x%0.2X]", \
            outcfg,tapp,threshold,shake_th,shdb,tapen,inten,MS_OUTCFG_DATA,MS_TAPP_DATA,OTA_track_sensor_config_data->sensor_threshold,\
            MS_SHAKE_TH_DATA,MS_SHDB_DATA,MS_TAPEN_DATA,MS_INTEN_DATA);

        OTA_tr_stop_timer(MOTION_SENSOR_DATA_TIMER_ID);
        OTA_tr_start_timer(MOTION_SENSOR_RESET_TIMER_ID,  3000, track_sensor_mc3410_reset);
    }
    OTA_tr_start_timer(MOTION_SENSOR_DATA_TIMER_ID, 60000, track_sensor_mc3410_data_self_test);

#endif /* SENSOR_SAMPLING */

}


/******************************************************************************
 *  Function    -  track_sensor_mc3410_reset
 *
 *  Purpose     -  sensor 复位
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 07-09-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_sensor_mc3410_reset(void)
{
    OTA_LOGD(L_DRV, L_V1, "sensor mc3410 reset");
    sensor_mc3410_pwr_down();
    OTA_tr_start_timer(MOTION_SENSOR_RESET_TIMER_ID,  10000, sensor_mc3410_on);
}


/******************************************************************************
 *  Function    -  track_sensor_mc3410_interrput_configuration
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
 * v1.0  , 07-09-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_sensor_mc3410_interrput_configuration(void)
{
    OTA_LOGD(L_DRV, L_V8, "Sensor eint  reg H");
    EINT_Mask(MOTION_SENSOR_EINT_NO);
    EINT_Registration(MOTION_SENSOR_EINT_NO, KAL_TRUE, 1, track_sensor_mc3410_eint_hisr, KAL_TRUE);
    EINT_Set_Sensitivity(MOTION_SENSOR_EINT_NO, LEVEL_SENSITIVE);
    EINT_SW_Debounce_Modify(MOTION_SENSOR_EINT_NO, SENSOR_SW_DEBOUNCE_TIME);
    EINT_UnMask(MOTION_SENSOR_EINT_NO);

    if(!OTA_track_is_timer_run(MOTION_SENSOR_DATA_TIMER_ID))
    {
        OTA_tr_start_timer(MOTION_SENSOR_DATA_TIMER_ID, 10000, track_sensor_mc3410_data_self_test);
    }
}


/******************************************************************************
 *  Function    -  track_sensor_mc3410_get_xyz
 *
 *  Purpose     -  允许采样中断，路测提取数据
 *
 *  Description -  注意: 读出的是负数的补码，需要转换
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 23-11-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_sensor_mc3410_get_xyz(short*x, short*y, short*z)
{
    unsigned char L_out, H_out;
    unsigned short Temp = 0;

    L_out = 0;
    H_out = 0;
    sensor_mc3410_read(MS_REG_XOUT_EX_L, &L_out, 1);
    sensor_mc3410_read(MS_REG_XOUT_EX_H, &H_out, 1);
    Temp = ((H_out << 8) | L_out);
    if((H_out & 0x80) != 0)
    {
        *x = -(((Temp - 1) ^ 0xFFFF) & 0x03FF);
    }
    else
    {
        *x = Temp & 0x03FF;
    }
    OTA_LOGC(L_DRV, L_V9, "X=%0.2X%0.2X  (%d)", H_out, L_out, *x);

    L_out = 0;
    H_out = 0;
    sensor_mc3410_read(MS_REG_YOUT_EX_L, &L_out, 1);
    sensor_mc3410_read(MS_REG_YOUT_EX_H, &H_out, 1);
    Temp = ((H_out << 8) | L_out);
    if((H_out & 0x80) != 0)
    {
        *y = -(((Temp - 1) ^ 0xFFFF) & 0x03FF);
    }
    else
    {
        *y = Temp & 0x03FF;
    }
    OTA_LOGC(L_DRV, L_V9, "Y=%0.2X%0.2X  (%d)", H_out, L_out, *y);


    L_out = 0;
    H_out = 0;
    sensor_mc3410_read(MS_REG_ZOUT_EX_L, &L_out, 1);
    sensor_mc3410_read(MS_REG_ZOUT_EX_H, &H_out, 1);
    Temp = ((H_out << 8) | L_out);
    if((H_out & 0x80) != 0)
    {
        *z = -(((Temp - 1) ^ 0xFFFF) & 0x03FF);
    }
    else
    {
        *z = Temp & 0x03FF;
    }
    OTA_LOGC(L_DRV, L_V9, "Z=%0.2X%0.2X  (%d)", H_out, L_out, *z);

}


/******************************************************************************
 *  Function    -  track_sensor_mc3410_eint_hisr
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
static void track_sensor_mc3410_eint_hisr(void)
{
    unsigned char tilt = 0;

#if defined (SENSOR_SAMPLING)
    //char temp[30] = {0};
    short x = 0, y = 0, z = 0;
    track_sensor_mc3410_get_xyz(&x, &y, &z);
    //sprintf(temp, "(%d,%d,%d)\r\n", x, y, z);
    //OTA_U_PutUARTBytes(0, (kal_uint8*)temp, strlen(temp));
    sensor_mc3410_read(MS_REG_TILT, &tilt, 1);
#else

    if(OTA_track_sensor_config_data->sensor_callback != NULL)
    {
        OTA_track_drv_sensor_interrupt_send_msg();
    }

    OTA_LOGC(L_DRV, L_V8, "R3=0x%0.2X",tilt); 

    if((tilt & 0x03) == 0x01)
    {
        sensor_position = KAL_TRUE;
    }
    else
    {
        sensor_position = KAL_FALSE;
        //LOGC(L_DRV, L_V1, "Error:GPS antenna upward");
    }

    sensor_mc3410_read(MS_REG_TILT, &tilt, 1);
        
#endif /* SENSOR_SAMPLING */

}


/******************************************************************************
 *  Function    -  track_get_sensor_mc3410_id
 *
 *  Purpose     -  读取sensor ID
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 21-10-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint8 track_get_sensor_mc3410_id(void)
{
    return mc3410_chip_id;
}

/******************************************************************************
 *  Function    -  track_get_sensor_mc3410_position
 * 
 *  Purpose     -  终端放置方位是否正确
 * 
 *  Description -  GPS天线必须朝上
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 02-12-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_bool track_get_sensor_mc3410_position(void)
{
    return sensor_position;
}

    
/******************************************************************************
 *  Function    -  track_sensor_mc3410_calibration
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
void track_sensor_mc3410_calibration(void)
{
    signed short X_raw = 0, Y_raw = 0, Z_raw = 0;
    signed short X_offset = 0, Y_offset = 0, Z_offset = 0;
    signed short X_gain = 0, Y_gain = 0, Z_gain = 0;
    signed short X_dot = 0, Y_dot = 0, Z_dot = 0;
    unsigned char raw_buf[6] = {0}, offset_buf[6] = {0}, gain_buf[3] = {0};
    unsigned short tmp = 0;
    char i = 0;

    sensor_mc3410_read(0X0D, raw_buf, 6);
    X_raw = (signed short)((raw_buf[0]) | (raw_buf[1] << 8));
    Y_raw = (signed short)((raw_buf[2]) | (raw_buf[3] << 8));
    Z_raw = (signed short)((raw_buf[4]) | (raw_buf[5] << 8));

    LOGC(L_DRV, L_V7, "XYZ_data (%d,%d,%d)", X_raw, Y_raw, Z_raw);

    sensor_mc3410_read(0x21, offset_buf, 6);
    tmp = (offset_buf[0]) | ((offset_buf[1] & 0x3F) << 8);
    if(tmp & 0x2000)
    {
        tmp |= 0xC000;
    }
    X_offset = tmp;


    tmp = (offset_buf[2]) | ((offset_buf[3] & 0x3F) << 8);
    if(tmp & 0x2000)
    {
        tmp |= 0xC000;
    }
    Y_offset = tmp;

    tmp = (offset_buf[4]) | ((offset_buf[5] & 0x3F) << 8);
    if(tmp & 0x2000)
    {
        tmp |= 0xC000;
    }
    Z_offset = tmp;

    OTA_LOGC(L_DRV, L_V7, "XYZ_offset (%X,%X,%X)", X_offset, Y_offset, Z_offset);

    sensor_mc3410_read(0x27, gain_buf, 3);
    X_gain = ((offset_buf[1] >> 7) << 8) + gain_buf[0];
    Y_gain = ((offset_buf[3] >> 7) << 8) + gain_buf[1];
    Z_gain = ((offset_buf[5] >> 7) << 8) + gain_buf[2];

    OTA_LOGC(L_DRV, L_V7, "XYZ_gain (%X,%X,%X)", X_gain, Y_gain, Z_gain);

    X_dot = X_offset - 16 * X_raw * 256 * 128 / 3 / 256 / (40 + X_gain);
    Y_dot = Y_offset - 16 * Y_raw * 256 * 128 / 3 / 256 / (40 + Y_gain);

    //水平反向
    //Z_dot = Z_offset -16* (Z_raw+256)*256*128/3/256/(40+Z_gain);

    //水平正向
    Z_dot = Z_offset - 16 * (Z_raw - 256) * 256 * 128 / 3 / 256 / (40 + Z_gain);

    OTA_LOGC(L_DRV, L_V7, "XYZ_dot (%X,%X,%X)", X_dot, Y_dot, Z_dot);

    offset_buf[0] = X_dot & 0xff;
    offset_buf[1] = ((X_dot >> 8) & 0x3f) | (X_gain & 0x0100 ? 0x80 : 0);
    offset_buf[2] = Y_dot & 0xff;
    offset_buf[3] = ((Y_dot >> 8) & 0x3f) | (Y_gain & 0x0100 ? 0x80 : 0);
    offset_buf[4] = Z_dot & 0xff;
    offset_buf[5] = ((Z_dot >> 8) & 0x3f) | (Z_gain & 0x0100 ? 0x80 : 0);

    sensor_mc3410_write_byte(MS_REG_MODE, MS_MODE_STANDBY_STATUS);
    for(i = 0; i < 6; i++)
    {
        sensor_mc3410_write_byte(0x21 + i, offset_buf[i]);
    }
    sensor_mc3410_write_byte(MS_REG_MODE, MS_MODE_WAKE_STATUS);

    OTA_LOGC(L_DRV, L_V4, "sensor calibration ok");
}


/******************************************************************************
 *  Function    -  track_sensor_mc3410_on
 *
 *  Purpose     -  开启sensor 检测
 *
 *  Description -  motion_threshold  [in] : sensor 检测灵敏度
 *                       FuncPtr  [in] : sensor 触发回调函数
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 27-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_sensor_mc3410_on(kal_uint8 threshold , FuncPtr callback)
{
    OTA_LOGD(L_DRV, L_V7, "threshold=%d", threshold);

    if((threshold <= 0) || (callback == NULL))
    {
        return;
    }

    OTA_track_sensor_config_data->sensor_threshold = threshold;
    OTA_track_sensor_config_data->sensor_callback = callback;

    sensor_mc3410_on();
}

/******************************************************************************
 *  Function    -  track_sensor_mc3410_off
 *
 *  Purpose     -  关闭sensor 检测
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 27-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_sensor_mc3410_off(void)
{
    OTA_tr_stop_timer(MOTION_SENSOR_DATA_TIMER_ID);
    OTA_tr_stop_timer(MOTION_SENSOR_RESET_TIMER_ID);   
    sensor_mc3410_pwr_down();
}

/******************************************************************************
 *  Function    -  track_sensor_mc3410_type_check
 * 
 *  Purpose     -  是否为mc3410
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 26-02-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_bool track_sensor_mc3410_type_check(void)
{
    kal_uint8 mc3410_check = 0XFF;
    
    ms_i2c_configure(MC3410_I2C_SLAVE_ADDR, MS_DELAY_TIME);
    sensor_mc3410_write_byte(MS_REG_MODE, MS_MODE_STANDBY_STATUS);
    sensor_mc3410_read(MS_REG_CHIPID, &mc3410_check, 1);

    if(mc3410_check==0x02)
    {
        return KAL_TRUE;
    }
    else
    {
        return KAL_FALSE;
    }
}

#endif /*__MCUBE_MC3410__*/
