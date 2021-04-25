#ifndef _TRACK_CUST_MAIN_H
#define _TRACK_CUST_MAIN_H

/****************************************************************************
*  Include Files                        包含头文件
*****************************************************************************/
#include "track_drv.h"
#if defined(__GT06B__)  /* 52平台工程 */
#include "common_nvram_editor_data_item.h"
#else    /* 60 平台工程 */
#include "nvram_editor_data_item.h"
#endif /* __GT06B__ */

/*****************************************************************************
*  Define                               宏定义
******************************************************************************/

/*****************************************************************************
*  Typedef  Enum                        枚举定义
*****************************************************************************/
#if defined(__GT06B__)
typedef enum
{
   L4C_RAC_OK,
   L4C_RAC_NO_CELL,
   L4C_RAC_LIMITED_SERVICE,
   L4C_RAC_ERROR,
   L4C_RAC_INVALID_SIM,
   L4C_RAC_ATTEMPT_TO_UPDATE,
   L4C_RAC_SEARCHING, /* Johnny 2005/11/07: add new temp gsm_status for MMI: L4C_RAC_SEARCHING */
   L4C_RAC_NULL
} l4c_rac_response_enum;
#endif /* __GT06B__ */

/*****************************************************************************
*  Struct                               数据结构定义
******************************************************************************/


/****************************************************************************
*  Global Variable - Extern             引用全局变量
*****************************************************************************/

/*****************************************************************************
*  Functions Define                     函数声明
******************************************************************************/
extern void track_cust_eint_trigger_response(U8 eint, U8 level);
extern void track_cust_chargestatus_vbtstatus_check(pmic_status_enum status, battery_level_enum level, kal_uint32 volt);
extern void track_cust_call_incomming_number(char *inc_number);
extern void track_cust_call_status(kal_uint16 callState);
extern void track_cust_init(void);
extern void track_cust_external_batter_measure_done(kal_int32 ext_volt);
extern kal_bool track_cust_is_upload_UTC_time(void);
extern void track_cust_check_simalm(void);
extern void track_cust_mode3_work_check(void* flag);
extern kal_uint32 track_drv_lte_init_wdt(kal_uint8 op);
#if defined (__GT370__)|| defined (__GT380__)
extern void track_cust_mode_change(kal_uint8 flg);
#endif
#endif  /* _TRACK_CUST_MAIN_H */

