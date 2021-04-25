
#if defined (__MCUBE_MC3410__)

#ifndef _TRACK_SENSOR_MC3410_H
#define _TRACK_SENSOR_MC3410_H

#include "stack_ltlcom.h"
#include "track_os_log.h"


//Register define start
#define MS_REG_XOUT 0x00
#define MS_REG_YOUT 0x01
#define MS_REG_ZOUT 0x02
#define MS_REG_TILT 0x03
#define MS_REG_OPSTAT 0x04
#define MS_REG_SC 0x05

#define MS_REG_INTEN 0x06
#define MS_REG_MODE 0x07
#define MS_REG_SAMPR 0x08
#define MS_REG_TAPEN 0x09
#define MS_REG_TAPP 0x0A

#define MS_REG_DROP_CTRL 0x0B
#define MS_REG_SHDB 0x0C

#define MS_REG_XOUT_EX_L 0x0D
#define MS_REG_XOUT_EX_H 0x0E
#define MS_REG_YOUT_EX_L 0x0F
#define MS_REG_YOUT_EX_H 0x10
#define MS_REG_ZOUT_EX_L 0x11
#define MS_REG_ZOUT_EX_H 0x12

#define MS_REG_CHIPID 0x18
//0x19~0x1F Reserved
#define MS_REG_OUTPUT_CFG 0x20

#define MS_REG_XOFFL 0x21
#define MS_REG_XOFFH 0x22
#define MS_REG_YOFFL 0x23
#define MS_REG_YOFFH 0x24
#define MS_REG_ZOFFL 0x25
#define MS_REG_ZOFFH 0x26

#define MS_REG_XGAIN 0x27
#define MS_REG_YGAIN 0x28
#define MS_REG_ZGAIN 0x29
//0x2A RESERVED

#define MS_REG_SHAKE_TH 0x2B
#define MS_REG_UD_Z_TH 0x2C
#define MS_REG_UD_X_TH 0x2D
#define MS_REG_RL_Z_TH 0x2E
#define MS_REG_RL_Y_TH 0x2F
#define MS_REG_FB_Z_TH 0x30
#define MS_REG_DROP_TH 0x31
#define MS_REG_TAP_TH 0x32
//0x33~0x3A RESERVED

#define MS_REG_PCODE 0x3B
//0x3C~0x3F RESERVED
//Register define End

//Tilt Status
#define TILT_STATUS_BAFR_MASK    0x03
#define TILT_STATUS_BAFR_UNKNOWN 0x00
#define TILT_STATUS_BAFR_FRONT   0x01
#define TILT_STATUS_BAFR_BACK    0x02

#define TILT_STATUS_POLA_MASK    0x1C
#define TILT_STATUS_POLA_UNKNOWN 0x00
#define TILT_STATUS_POLA_LEFT    0x04
#define TILT_STATUS_POLA_RIGHT   0x08
#define TILT_STATUS_POLA_DOWN    0x14
#define TILT_STATUS_POLA_UP      0x18

#define TILT_STATUS_TAP_EVENT    0x20
#define TILT_STATUS_DROP_EVENT   0x40
#define TILT_STATUS_SHAKE_EVENT  0x80

//Sleep Count
#define SLEEP_COUNT 0

//Mode
#define AUTO_WAKE_ENABLE 0x08
#define AUTO_SNIFF_ENABLE 0x10

//TAP
#define TAP_X_ENABLE 0x20
#define TAP_Y_ENABLE 0x40
#define TAP_Z_ENABLE 0x80
#define TAP_PULSE_COUNT 0x0F //This value sets the number of samples for which a tap pulse must exceed the TAP_TH threshold before it is rejected as not a tap event. If the values detected by the sensor exceed the TAP_TH threshold for longer than the reject count, no tap event is detected and the interrupt is not set.
#define TAP_THRESHOLD 0x20

//Drop
#define DROP_DEBOUNCE   0x05 // 0~255
#define DROP_INTERRUPT  0x40  //bit6: 0: disable ; 1: enable
#define DROP_MODE  0x00 //bit7: 0: mode_A; 1: mode_B
#define DROP_THRESHOLD 0x7F //~2.9mg/LSB

//Shake
#define SHAKE_DEBOUNCE 0x0a  //0~63
#define SHAKE_THRESHOLD 0x0  //offset value:mag(X/Y/Z) > 1.3g + SHAKE_TH, ~2.9mg/LSB

#define MC3410_I2C_SLAVE_ADDR       0x98

typedef enum
{
   MCUBE_DROP_MODE_A,
   MCUBE_DROP_MODE_B
} mCubeAccelDropMode_t;

typedef void (*FuncPtr) (void);

#define TAP_DETECT_X   0x20
	/** Enable Tap Detection on Y-axis */
#define TAP_DETECT_Y   0x40
	/** Enable Tap Detection on Z-axis */
#define TAP_DETECT_Z   0x80

/* Calibration parameters for gesture event triggering */
#define MC3230_DFLT_UDZ_THRESH           0x0
#define MC3230_DFLT_UDX_THRESH           0x0
#define MC3230_DFLT_RLZ_THRESH           0x0
#define MC3230_DFLT_RLY_THRESH           0x0
#define MC3230_DFLT_FBZ_THRESH           0x0
#define MC3230_DFLT_DROP_THRESH          0x0
#define MC3230_DFLT_TAP_THRESH           0x0
#define MC3230_DFLT_SHAKE_DEBOUNCE       0x0
#define MC3230_DFLT_SHAKE_THESH_OFFSET   0x0
#define MC3230_DFLT_DROP_DEBOUNCE        0x0
#define MC3230_DFLT_DROP_MODE            MCUBE_DROP_MODE_A
#define MC3230_DFLT_TAP_AXES             TAP_DETECT_Z
#define MC3230_DFLT_TAP_DETECT_SAMP      0x0

/* acceleration 8-Bits,+/-1.5g */
#define ACC_0G_X      127
#define ACC_1G_X      212
#define ACC_MINUS1G_X 43
#define ACC_0G_Y      127   
#define ACC_1G_Y      212
#define ACC_MINUS1G_Y 43
#define ACC_0G_Z      127       
#define ACC_1G_Z      212
#define ACC_MINUS1G_Z 43


#if defined (__TRACK_SENSOR__)
static void track_sensor_mc3410_interrput_configuration(void);
static void track_sensor_mc3410_eint_hisr(void);
static void track_sensor_mc3410_reset(void);

void track_sensor_mc3410_on(kal_uint8 threshold , FuncPtr callback);
void track_sensor_mc3410_off(void);
kal_bool track_sensor_mc3410_type_check(void);
#endif /* __TRACK_SENSOR__ */


#endif  /* _TRACK_SENSOR_MC3410_H */

#endif /* __MCUBE_MC3410__ */

