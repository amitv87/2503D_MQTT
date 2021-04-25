/******************************************************************************
 * track_os_log.h -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        �ṩ�����־�ӿ�
 *
 * modification history
 * --------------------
 * v1.0   2012-07-21,  WangQi create this file
 *
 ******************************************************************************/

#ifndef _TRACK_LOG_H
#define _TRACK_LOG_H

/****************************************************************************
* Include Files             ����ͷ�ļ�
*****************************************************************************/


/*******************************************************************************
 * Type Definitions
 *******************************************************************************/
/*
 * general definitions
 */

typedef enum
{
    L_CON = 0,     //1������Ϣ�ȣ����ٴ�ӡ��
    L_DRV = 1,     //driver
    L_SOC = 2,     // network
    L_CMD = 3,     // at sms call factory
    L_OS  = 4,     //
    L_APP = 5,     //
    L_SYS = 6,     //
    L_FUN = 7,     //
    L_SENSOR = 8,
    L_TEST= 9,     //smt����ģʽ��
    L_OBD = 10,    // network
    L_GPS = 11,    // network
    L_MAX
} enum_LOG_MODULE;

typedef enum
{
    L_V1 = 1,
    L_V2 = 2,
    L_V3 = 3,
    L_V4 = 4,
    L_V5 = 5,
    L_V6 = 6,
    L_V7 = 7,
    L_V8 = 8,
    L_V9 = 9,
    L_VMAX
} enum_LOG_LEVEL;

#define gAT_PORT 0
#define gTST_PORT 4


/******************************************************************************
 *  Function    -  LOGH
 *  Purpose     -  ��ӡ��һ���ڴ�ռ�ʮ�����Ƶ�ֵ
 *  Description -  
 * modification history
 * ----------------------------------------
 * v1.0  , 13-07-2010,    written
 * ----------------------------------------
 ******************************************************************************/
extern void LOGHH(enum_LOG_MODULE module, enum_LOG_LEVEL grade, const char *fun_name, const char *text, void * buff, int size);

/******************************************************************************
 * FUNCTION:  LOGS *
 * DESCRIPTION: -  *ֱ�����
 * Input:  *
 * Output:
 * Returns:
 *
 * modification history
 * --------------------
 * v1.0  , 08-08-2012,   wangqi written
 * --------------------
 ******************************************************************************/
extern void LOGS(const char *fmt, ...);

/******************************************************************************
 * FUNCTION:  - LOGB * 
 * DESCRIPTION: -  ���ַ������������ * 
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 * v1.0  , 15-10-2012,  wangqi  written
 * --------------------
 ******************************************************************************/
extern void LOGB(enum_LOG_MODULE module,enum_LOG_LEVEL grade,const void * buff, int size);

extern void LOGDD(enum_LOG_MODULE module, enum_LOG_LEVEL grade, const char *fun_name, const char *fmt, ...);

extern void LOGF(enum_LOG_MODULE module, enum_LOG_LEVEL grade, void * buff, int size);
/* ���Զ���Ӻ����� */
#define LOGC(module, grade, fmt, args...) LOGDD(module, grade, "", fmt, ##args)

/* ���Զ���Ӻ����� */
#define LOGD(module, grade, fmt, args...) LOGDD(module, grade, __func__, fmt, ##args)

#if defined (__REMOVE_LOG_TO_FILE__)
#define LOGD2(module, grade, fmt, args...) LOGDD(module, grade, __func__, fmt, ##args)
#else
#define LOGD2(module, grade, fmt, args...) LOGDD2(module, grade, __func__, fmt, ##args)
#endif

#define LOGH(module, grade, buff, size) LOGHH(module, grade, __func__, NULL, buff, size) 

#define LOGH2(module, grade, text, buff, size) LOGHH(module, grade, __func__, text, buff, size) 

#endif  /* _TRACK_LOG_H */
//--#--define nvram_trace(...)    kal_brief_trace(__VA_ARGS__)
