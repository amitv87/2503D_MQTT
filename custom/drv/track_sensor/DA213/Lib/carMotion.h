/*******************************************************************************/
 /**
  ******************************************************************************
  * @file    carMotion.h
  * @author  yzhou@miramems.com
  * @version V1.0
  * @date    06-May-2016
  * @brief
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, MiraMEMS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  */
/*******************************************************************************/
#ifndef __CARMOTION_h
#define __CARMOTION_h

/*******************************************************************************
Includes
********************************************************************************/
#include "mira_std.h"

/*******************************************************************************
Macro definitions - Algorithm Build Options
********************************************************************************/
#define VIDEOING_ENABLE    1
#define PARKING_ENABLE     1
#define VIOLENT_ENABLE     1
#define STATE_ENABLE       1
#define JOLT_ENABLE        1
#define SLOPE_ENABLE       1

/*******************************************************************************
Typedef definitions
********************************************************************************/
enum{
  DEBUG_ERR = 1,	
  DEBUG_MSG = 1<<1,
  DEBUG_DATA= 1<<2,
  DEBUG_RAW = 1<<3,
};

typedef enum {
  PIN_NONE,
  PIN_ONE,
  PIN_TWO		
}tPIN_NUM;

typedef enum {
  PIN_LEVEL_NONE,
  PIN_LEVEL_LOW,
  PIN_LEVEL_HIGH	
}tPIN_LEVEL;

typedef enum {
  NO_LATCHED,
  LATCHED_250MS,
  LATCHED_500MS,
  LATCHED_1S,
  LATCHED_2S,
  LATCHED_4S,
  LATCHED_8S,
  LATCHED_1MS=10,
  LATCHED_2MS,
  LATCHED_25MS,
  LATCHED_50MS,
  LATCHED_100MS,
  LATCHED
}tLATCH_MODE;

typedef enum {
  TEVENT_NONE,
  TEVENT_VIDEOING_NOTIFY,
  TEVENT_PARKING_NOTIFY,
  TEVENT_VIOLENT_NOTIFY,
  TEVENT_STATE_NOTIFY,
  TEVENT_JOLT_NOTIFY,
  TEVENT_SLOPE_NOTIFY,
}carMotion_EVENT;

typedef enum {
  NONE_T,
  VIDEOING_T,
  PARKING_T,
  VIOLENT_T,
  STATE_T,
  JOLT_T,
  SLOPE_T,
}tAlgorithm;

typedef enum {
  DISABLE_T,
  ENABLE_T
}tSwitchCmd;

typedef enum {
  VIOLENT_NONE = 0,
  VIOLENT_SPEED_UP   = 1,
  VIOLENT_SPEED_DOWN  = 2,
  VIOLENT_TURN_LEFT   = 3,
  VIOLENT_TURN_RIGHT  = 4  
}Tviolent;

typedef enum {
  STATE_NONE     = 0,
  STATE_STATIC   = 1,
  STATE_MOVE     = 2,
  STATE_FLIP     = 3,
}Tstate;

typedef enum {
  SLOPE_NONE     = 0,
  SLOPE_UP       = 1,
  SLOPE_DOWN     = -1,
}Tslope;

struct tpin_Config{
  tPIN_NUM pin_num;
  tPIN_LEVEL pin_level;
};

struct tInt_Pin_Config{
  tPIN_NUM videoing_pin_num;
  tPIN_LEVEL videoing_pin_level;
  tPIN_NUM parking_pin_num;
  tPIN_LEVEL parking_pin_level;
};

struct tREG_Func{
  s8_m (*read)(u8_m addr, u8_m *data, u8_m len);
  s8_m (*write)(u8_m addr, u8_m data);
};

struct tFILE_Func{
  s8_m (*save)(s16_m x, s16_m y,s16_m z);
  s8_m (*get)(s16_m *x, s16_m *y,s16_m *z);
};

struct tTIME_Func{
  u32_m (*creat)(void);
  s8_m (*start)(u32_m id, u32_m msDelay,u8_m *auto_reset,void(*pCallback)());
  s8_m (*cancel)(u32_m id);  
};

struct tDEBUG_Func{
  s32_m  (*myprintf)(const char *fmt, ...); 
};

struct carMotion_op_s {
  struct tInt_Pin_Config pin;
  s8_m (*event_handle)(carMotion_EVENT event, s32_m data);
  struct tREG_Func reg;
  struct tFILE_Func file;
  struct tTIME_Func timer;
  struct tDEBUG_Func debug;  
   
};  

/*******************************************************************************
Global variables and functions
********************************************************************************/

/*******************************************************************************
* Function Name: carMotion_Init
* Description  : This function initializes the carMotion.
* Arguments    : carMotion_op_s *ops
* Return Value : 0: OK; -1: Type Error; -2: OP Error; -3: Chip Error
********************************************************************************/
s8_m carMotion_Init(struct carMotion_op_s *ops);

#if VIDEOING_ENABLE 
/*******************************************************************************
* Function Name: carMotion_Videoing_Set_Parma
* Description  : This function sets videoing parmas.
* Arguments    : 
*              threshold - set interrupt threshold 1~255
*              latch_mode - set interrupt latched mode (tLATCH_MODE)
*              duration - set interrupt duration 0~3   
* Return Value : None
********************************************************************************/
void carMotion_Videoing_Set_Parma(u8_m threshold,tLATCH_MODE latch_mode,u8_m duration);
#endif

#if PARKING_ENABLE
/*******************************************************************************
* Function Name: carMotion_Parking_Set_Parma
* Description  : This function sets parking parmas.
* Arguments    : 
*              threshold - set interrupt threshold 1~255
*              latch_mode - set interrupt latched mode (tLATCH_MODE)
*              duration - set interrupt duration 0~3                 
* Return Value : None
********************************************************************************/
void carMotion_Parking_Set_Parma(u8_m threshold,tLATCH_MODE latch_mode,u8_m duration);
#endif

#if VIOLENT_ENABLE
/*******************************************************************************
* Function Name: carMotion_Violent_Set_Parma
* Description  : This function sets violent parmas.
* Arguments    : level - set the violent level,defalut 2 (1~6 )
* Return Value : None
********************************************************************************/
void carMotion_Violent_Set_Parma(u8_m level);
#endif

#if SLOPE_ENABLE
/*******************************************************************************
* Function Name: carMotion_Slope_Set_Parma
* Description  :This function set the time and angle.
* Arguments    :level - set the slope level (1~3 )
                min - set the monitoring time (1~60 )
                angle_f - set the llowable deviation angle in normal (1~45 )
                angle_t - set the trigger angle1 (1~90 )
* Return Value : None
********************************************************************************/
void carMotion_Slope_Set_Parma(u8_m level,u8_m min,fp32_m angle_f,fp32_m angle_t);
#endif

/*******************************************************************************
* Function Name: carMotion_Direction_Set_Parma
* Description  : This function sets direction parmas.
* Arguments    : dir - direction of coordinate axes, default is 0 (0~15)
* Return Value : None
********************************************************************************/
void carMotion_Direction_Set_Parma(u8_m dir);

/*******************************************************************************
* Function Name: carMotion_Control
* Description  : This function initializes the carMotion.
* Arguments    : name - select which algorithm to control
                 cmd - enable/disable
* Return Value : None
********************************************************************************/
void carMotion_Control(tAlgorithm name, tSwitchCmd cmd);

/*******************************************************************************
* Function Name: carMotion_Get_Version
* Description  : This function gets xMotion version.
* Arguments    : ver - xMotion version Num
* Return Value : None
********************************************************************************/
void carMotion_Get_Version(u8_m *ver);

/*******************************************************************************
* Function Name: carMotion_QueryControl
* Description  : This function gets algorithm state.
* Arguments    : name - select which algorithm
* Return Value : SwitchCmd - enable/disable
********************************************************************************/
tSwitchCmd carMotion_QueryControl(tAlgorithm name);

/*******************************************************************************
* Function Name: carMotion_chip_check_id
* Description  : This function checks the chip id.
* Arguments    : None
* Return Value : 0: OK; -1: Error
********************************************************************************/
s8_m carMotion_chip_check_id(void);

/*******************************************************************************
* Function Name: carMotion_chip_read_xyz
* Description  : This function reads the chip xyz.
* Arguments    : x, y, z - acc data
* Return Value : 0: OK; -1: Error
********************************************************************************/
s8_m carMotion_chip_read_xyz(s16_m *x,s16_m *y,s16_m *z);

/*******************************************************************************
* Function Name: carMotion_buffer_read_xyz
* Description  : This function reads the buffer xyz.
* Arguments    : x, y, z - acc data
* Return Value : 0: OK; -1: Error
********************************************************************************/
s8_m carMotion_buffer_read_xyz(s16_m *x,s16_m *y,s16_m *z);

/*******************************************************************************
* Function Name: carMotion_chip_calibrate_offset
* Description  : This function calibrates the offset.
* Arguments    : None
* Return Value : 0: OK; -1: Error
********************************************************************************/
s8_m carMotion_chip_calibrate_offset(void);

/*******************************************************************************
* Function Name: carMotion_chip_power_on
* Description  : This function enables the chip.
* Arguments    : None
* Return Value : 0: OK; -1: Error
********************************************************************************/
s8_m carMotion_chip_power_on(void);

/*******************************************************************************
* Function Name: carMotion_chip_power_off
* Description  : This function disables on the chip.
* Arguments    : None
* Return Value : 0: OK; -1: Error
********************************************************************************/
s8_m carMotion_chip_power_off(void);

/*******************************************************************************
* Function Name: carMotion_Set_Debug_level
* Description  : This function sets the debug log level
* Arguments    : Log level
* Return Value : None
********************************************************************************/
void carMotion_Set_Debug_level(u8_m level);
#endif 
