/******************************************************************************
 * track_motion_sensor_afa750.h - 
 * 
 * Copyright .
 * 
 * DESCRIPTION: - 
 *        MMA8542Q Sensor Çý¶¯ 
 * 
 * modification history
 * --------------------
 * v1.0   2012-12-14,  chengjun create this file
 * 
 ******************************************************************************/
#if defined (__FREESCALE_MMA8452Q__)

#ifndef _TRACK_SENSOR_MMA8452Q_H
#define _TRACK_SENSOR_MMA8452Q_H

#include "kal_release.h"
#include "drv_features.h"

typedef void (*FuncPtr) (void);
	 
#define MMA8452Q_I2C_SLAVE_ADDR  0x38

static void track_sensor_mma8452q_interrput_configuration(void);
static void track_sensor_mma8452q_eint_hisr(void);
static void track_sensor_mma8452q_reset(void);

void track_sensor_mma8452q_on(kal_uint8 threshold , FuncPtr callback);
void track_sensor_mma8452q_off(void);
kal_bool track_sensor_mma8452q_type_check(void);

#endif  /* _TRACK_SENSOR_MMA8452Q_H */

#endif /* __FREESCALE_MMA8452Q__ */
