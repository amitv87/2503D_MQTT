/******************************************************************************
 * track_motion_sensor_afa750.h - 
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

#ifndef _TRACK_SENSOR_AFA750_H
#define _TRACK_SENSOR_AFA750_H

#include "kal_release.h"
#include "drv_features.h"

typedef void (*FuncPtr) (void);

void AFA750_sensor_set_reg(void);
void AFA750_sensor_pwr_up(void);
void AFA750_sensor_pwr_down(void);
void AFA750_sensor_get_data(kal_uint16 * x_adc, kal_uint16 * y_adc, kal_uint16 *z_adc);

#define AFA750_PWR_REG	3
#define AFA750_xyz_REG	0x10
#define AFA750_RANGE 16
#define AFA750_RESOLUTION 16

	 
#define AFA750_I2C_SLAVE_ADDR  0x78
//参考硬件接口图
//0x7A <-> SA0=1 
//0x78 <-> SA0=0

static void track_sensor_afa750_interrput_configuration(void);
static void track_sensor_afa750_eint_hisr(void);
static void track_sensor_afa750_reset(void);

void track_sensor_afa750_on(kal_uint8 threshold , FuncPtr callback);
void track_sensor_afa750_off(void);
kal_bool track_sensor_afa750_type_check(void);

#endif  /* _TRACK_SENSOR_AFA750_H */

#endif /* __MICRO_AFA750__ */
