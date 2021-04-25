/******************************************************************************
 * track_interrupt.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *       端口模块 : 中断，GPIO(油电、马达)，SIM卡
 *
 * modification history
 * --------------------
 * v1.0   2012-07-24,  chengjun create this file
 *
 ******************************************************************************/

/****************************************************************************
* Include Files                   		包含头文件
*****************************************************************************/
#include "track_drv_eint_gpio.h"
#include "stack_msgs.h"
#include "stack_ltlcom.h"
#include "sim_public_enum.h"
#include "track_drv.h"
#include "dcl_pmu_common_sw.h"
#include "track_os_ell.h"
#include "Track_cust.h"
/*****************************************************************************
 *  Define					宏定义
 *****************************************************************************/
#define PMU_VIBR_1300_MV  0
#define PMU_VIBR_1500_MV  0x10
#define PMU_VIBR_1800_MV  0x20
#define PMU_VIBR_2500_MV  0x30
#define PMU_VIBR_2800_MV  0x40
#define PMU_VIBR_3000_MV  0x50
#define PMU_VIBR_3300_MV  0x60
/*定义马达输出电压，配置寄存器*/

/*****************************************************************************
* Typedef  Enum
*****************************************************************************/


/*****************************************************************************
 *  Struct					数据结构定义
 *****************************************************************************/


/*****************************************************************************
* Local variable				局部变量
*****************************************************************************/
static enum_SIM_STATE lte_sms_state = SIM_STATE_MAX;
static kal_bool    touch_panel_state1 = KAL_FALSE;
#if defined __USE_SPI__
static const kal_uint8 spi_wod = EINT_SPI_NO;
static const kal_uint8 spi_gpio = EINT_SPI_GPIO;
static void track_eint_resp_CSPI_low(void);
#endif
/*****************************************************************************
* Golbal variable				全局变量
*****************************************************************************/


/****************************************************************************
* Global Variable - Extern          引用全局变量
*****************************************************************************/
extern nvram_realtime_struct    G_realtime_data;

/*****************************************************************************
 *  Global Functions	- Extern		引用外部函数
 *****************************************************************************/
extern kal_uint8 track_cust_status_acc(void);
extern char track_GPIO_ReadIO(kal_uint16 port);
/*****************************************************************************
 *  Local Functions			本地函数
 *****************************************************************************/

static void track_eint_C_registration(void);
static void track_eint_resp_C_high(void);
static void track_eint_resp_C_low(void);

static void track_eint_trigger_response(U8 eint, U8 level);
static void track_eint_resp_D_high(void);
static void track_eint_resp_D_low(void);
static void track_eint_D_registration(void);

/******************************************************************************
 *  Function    -  track_drv_set_LDO
 *
 *  Purpose     -  GPS开启VCAMD供电
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 23-01-2013,  WJ  written
  * v2.0  , 20150515,  wq  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_set_LDO(kal_bool ON, PMU_LDO_BUCK_LIST_ENUM ldo, PMU_VOLTAGE_ENUM ldo_volt)
{
    PMU_CTRL_LDO_BUCK_SET_VOLTAGE_EN pmu_ldo_voltage_en;
    PMU_CTRL_LDO_BUCK_SET_EN pmu_ldo_en;
    DCL_HANDLE ctp_pmu_handle;

    LOGD(L_DRV, L_V5, "ON/OFF:%d, ldo:%d, volt:%d", ON, ldo, ldo_volt);
    if(ctp_pmu_handle == DCL_HANDLE_NONE)
    {
        ctp_pmu_handle = DclPMU_Open(DCL_PMU, FLAGS_NONE);
    }
    if(ON && (ldo_volt != 0))
    {
        pmu_ldo_voltage_en.mod = (PMU_LDO_BUCK_LIST_ENUM)ldo;
        pmu_ldo_voltage_en.voltage = (PMU_VOLTAGE_ENUM)ldo_volt;
        DclPMU_Control(ctp_pmu_handle, LDO_BUCK_SET_VOLTAGE_EN, (DCL_CTRL_DATA_T *)& pmu_ldo_voltage_en);
    }
    else if(ON && (ldo_volt == 0))
    {
        pmu_ldo_en.mod = (PMU_LDO_BUCK_LIST_ENUM)ldo;
        pmu_ldo_en.enable = KAL_TRUE;
        DclPMU_Control(ctp_pmu_handle, LDO_BUCK_SET_EN, (DCL_CTRL_DATA_T *)&pmu_ldo_en);
    }
    else
    {
        pmu_ldo_en.mod = (PMU_LDO_BUCK_LIST_ENUM)ldo;
        pmu_ldo_en.enable = KAL_FALSE;
        DclPMU_Control(ctp_pmu_handle, LDO_BUCK_SET_EN, (DCL_CTRL_DATA_T *)&pmu_ldo_en);
    }
}
void track_drv_Set_VMC(kal_bool flag, kal_uint32 volt_status)
{

    DCL_HANDLE handle;
    PMU_CTRL_LDO_BUCK_SET_VOLTAGE_EN val1;
    PMU_CTRL_LDO_BUCK_SET_ON_SEL val2;
    PMU_CTRL_LDO_BUCK_SET_EN val;
    DCL_STATUS status;
    kal_uint32 volt = 0;

    if(volt_status == 0)
    {
        volt = 0;//0V
    }
    else if(volt_status == 1)
    {
        volt = 1800000;//1.8V
    }
    else if(volt_status == 2)
    {
        volt = 2800000;//2.8V
    }
    else if(volt_status == 3)
    {
        volt = 3000000;//3.0V
    }
    else
    {
        volt = 3300000;//3.3V
    }

    LOGD(L_DRV, L_V5, "@track_drv_Set_VMC@");
    //设置电压
    val1.mod = VMC;
    val1.voltage = volt;
    handle = DclPMU_Open(DCL_PMU, FLAGS_NONE);
    status = DclPMU_Control(handle, LDO_BUCK_SET_VOLTAGE, (DCL_CTRL_DATA_T *) & val1);
    LOGD(L_DRV, L_V5, "status 1=%d", status);
    //VMC控制口开关
    val2.onSel = 1;
    val2.mod = VMC;
    DclPMU_Control(handle, LDO_BUCK_SET_ON_SEL, (DCL_CTRL_DATA_T *)&val2);

    //举例：打开VMC
    val.enable = flag;
    val.mod = VMC;
    status = DclPMU_Control(handle, LDO_BUCK_SET_EN, (DCL_CTRL_DATA_T *) & val);
    LOGD(L_DRV, L_V5, "status 2=%d", status);

    DclPMU_Close(handle);
}

/*对于中断口初始状态注册 :
推荐使用GPIO_ReadIO，但部分GPIO 读入结果与实际不符，
因此通过定时器主动反向注册第二次*/

// A
/******************************************************************************
 *  Function    -  track_eint_A_hisr
 *
 *  Purpose     -  中断A 响应函数
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 29-01-2013,  chengjun  written
 * v1.1  , 16-05-2013,  cml       written
 * ----------------------------------------
 ******************************************************************************/
static void track_eint_A_hisr(void)
{
    kal_bool eint_A_status;

    if(EINT_A_NO == 0xFF) return;
    EINT_Mask(EINT_A_NO);
    if(GPIO_ReadIO(EINT_A_GPIO))
    {
        track_eint_trigger_response(EINT_A_NO, LEVEL_HIGH);
        eint_A_status = LEVEL_LOW;
    }
    else
    {
        track_eint_trigger_response(EINT_A_NO, LEVEL_LOW);
        eint_A_status = LEVEL_HIGH;
    }
    LOGD(L_DRV, L_V8, "ACC EINT %d,%d", eint_A_status, kal_get_active_module_id());
    EINT_SW_Debounce_Modify(EINT_A_NO, EINT_SW_DEBOUNCE_TIME);
    EINT_Set_Polarity(EINT_A_NO, eint_A_status);
    EINT_UnMask(EINT_A_NO);
}

/******************************************************************************
 *  Function    -  track_eint_A_registration
 *
 *  Purpose     -  中断A 初始化
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 29-01-2013,  chengjun  written
 * v1.1  , 16-05-2013,  cml       written
 * ----------------------------------------
 ******************************************************************************/
static void track_eint_A_registration(void)
{
    if(EINT_A_NO == 0xFF) return;
    EINT_Mask(EINT_A_NO);
    //GPIO_InitIO(0, EINT_A_GPIO);
    LOGD(L_DRV, L_V6, "EINTA %d, GPIO:%d, M:%d, %d", EINT_A_NO, EINT_A_GPIO, EINT_A_EINT_MODE, kal_get_active_module_id());
    //GPIO_ModeSetup(EINT_A_GPIO, 1);
#if defined(__V20__) || defined(__GT02F__) ||defined(__ET210__) || (defined (__NT33__)&&defined(__FUN_WITH_ACC__)) || defined(__GT420D__)
    GPIO_ModeSetup(EINT_A_GPIO, EINT_A_EINT_MODE);
#endif
#if  defined(__GT420D__)
	GPIO_InitIO(INPUT, EINT_A_GPIO);
#endif 
    EINT_SW_Debounce_Modify(EINT_A_NO, EINT_SW_DEBOUNCE_TIME);
    EINT_Set_Sensitivity(EINT_A_NO, EINT_SENSITIVE_MODE);
    if(GPIO_ReadIO(EINT_A_GPIO))
    {
        track_eint_trigger_response(EINT_A_NO, LEVEL_HIGH);
        EINT_Registration(EINT_A_NO, KAL_TRUE, LEVEL_LOW, track_eint_A_hisr, KAL_TRUE);
    }
    else
    {
        track_eint_trigger_response(EINT_A_NO, LEVEL_LOW);
        EINT_Registration(EINT_A_NO, KAL_TRUE, LEVEL_HIGH, track_eint_A_hisr, KAL_TRUE);
    }
    EINT_UnMask(EINT_A_NO);
}


// B
/******************************************************************************
 *  Function    -  track_eint_B_hisr
 *
 *  Purpose     -  中断B 响应函数
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 29-01-2013,  chengjun  written
 * v1.1  , 16-05-2013,  cml       written
 * ----------------------------------------
 ******************************************************************************/
static void track_eint_B_hisr(void)
{
    kal_bool eint_A_status;
    if(EINT_B_NO == 0xFF) return;
    EINT_Mask(EINT_B_NO);
    if(GPIO_ReadIO(EINT_B_GPIO))
    {
        track_eint_trigger_response(EINT_B_NO, LEVEL_HIGH);
        eint_A_status = LEVEL_LOW;
    }
    else
    {
        track_eint_trigger_response(EINT_B_NO, LEVEL_LOW);
        eint_A_status = LEVEL_HIGH;
    }
    LOGD(L_DRV, L_V9, "EINTB %d,%d", eint_A_status, kal_get_active_module_id());
#if defined (__NT31__)|| defined(__NT22__) 
    EINT_SW_Debounce_Modify(EINT_B_NO, EINT_SW_DEBOUNCE_20MS);
#else
    EINT_SW_Debounce_Modify(EINT_B_NO, EINT_SW_DEBOUNCE_TIME);
#endif /* __nt31__ */
    EINT_Set_Polarity(EINT_B_NO, eint_A_status);

    EINT_UnMask(EINT_B_NO);
}

/******************************************************************************
 *  Function    -  track_eint_B_registration
 *
 *  Purpose     -  中断B 初始化
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 29-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_eint_B_registration(void)
{
    if(EINT_B_NO == 0xFF) return;
    EINT_Mask(EINT_B_NO);
    // GPIO_ModeSetup(EINT_B_GPIO, 0);
    //GPIO_InitIO(0, EINT_B_GPIO);
    LOGD(L_DRV, L_V6, "EINTB %d, GPIO:%d, M:%d, %d", EINT_B_NO, EINT_B_GPIO, EINT_B_EINT_MODE, kal_get_active_module_id());
#if defined(__UART1_SOS__)
    GPIO_ModeSetup(EINT_B_GPIO, 3);
#else
    GPIO_ModeSetup(EINT_B_GPIO, EINT_B_EINT_MODE);
#endif
    EINT_SW_Debounce_Modify(EINT_B_NO, EINT_SW_DEBOUNCE_TIME);
    EINT_Set_Sensitivity(EINT_B_NO, EINT_SENSITIVE_MODE);
    if(GPIO_ReadIO(EINT_B_GPIO))
    {
        track_eint_trigger_response(EINT_B_NO, LEVEL_HIGH);
        EINT_Registration(EINT_B_NO, KAL_TRUE, LEVEL_LOW, track_eint_B_hisr, KAL_TRUE);
    }
    else
    {
        track_eint_trigger_response(EINT_B_NO, LEVEL_LOW);
        EINT_Registration(EINT_B_NO, KAL_TRUE, LEVEL_HIGH, track_eint_B_hisr, KAL_TRUE);
    }
    EINT_UnMask(EINT_B_NO);
}

// C
/******************************************************************************
 *  Function    -  track_eint_C_registration
 *
 *  Purpose     -  中断C 初始化
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 29-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_eint_C_registration(void)
{
    LOGD(L_DRV, L_V9, "EINTC %d, eint:%d, mode:%d", kal_get_active_module_id(), EINT_C_NO, EINT_C_EINT_MODE);
    /*EINT_SW_Debounce_Modify(EINT_C_NO, EINT_SW_DEBOUNCE_TIME);
    EINT_Set_Sensitivity(EINT_C_NO, EINT_SENSITIVE_MODE);
    EINT_Registration(EINT_C_NO, KAL_TRUE, LEVEL_LOW, track_eint_resp_C_low, KAL_FALSE);
    EINT_UnMask(EINT_C_NO);*/

    if(EINT_C_NO == 0xFF) return;
    EINT_Mask(EINT_C_NO);
#if defined(__GT530__)||defined(__GT02F__)
    GPIO_ModeSetup(EINT_C_GPIO, EINT_C_EINT_MODE);
#endif
    EINT_SW_Debounce_Modify(EINT_C_NO, EINT_SW_DEBOUNCE_TIME);
    EINT_Set_Sensitivity(EINT_C_NO, EINT_SENSITIVE_MODE);
    if(GPIO_ReadIO(EINT_C_GPIO))
    {
        EINT_Registration(EINT_C_NO, KAL_TRUE, LEVEL_LOW, track_eint_resp_C_low, KAL_TRUE);
    }
    else
    {
        EINT_Registration(EINT_C_NO, KAL_TRUE, LEVEL_HIGH, track_eint_resp_C_high, KAL_TRUE);
    }
    EINT_UnMask(EINT_C_NO);

}

#if defined __USE_SPI__
static void track_eint_resp_CSPI_high(void)
{
    EINT_Mask(spi_wod);
    LOGD(L_DRV, L_V6, "init spi cs high");
    EINT_Registration(spi_wod, KAL_TRUE, LEVEL_LOW, track_eint_resp_CSPI_low, KAL_FALSE);
    EINT_UnMask(spi_wod);
}
static void track_eint_resp_CSPI_low(void)
{
    if(spi_wod == 0xFF) return;
    EINT_Mask(spi_wod);
    GPIO_WriteIO(0, 28);
    track_eint_trigger_response(spi_wod, LEVEL_LOW);
    EINT_Registration(spi_wod, KAL_TRUE, LEVEL_HIGH, track_eint_resp_CSPI_high, KAL_FALSE);
    EINT_UnMask(spi_wod);
}
void track_eint_CSPI_registration(void)
{
    EINT_Mask(spi_wod);
    //EINT_SW_Debounce_Modify(spi_wod, EINT_SW_DEBOUNCE_TIME);
    EINT_Set_Sensitivity(spi_wod, EINT_SENSITIVE_MODE);
    if(GPIO_ReadIO(spi_gpio))
    {
        EINT_Registration(spi_wod, KAL_TRUE, LEVEL_LOW, track_eint_resp_CSPI_low, KAL_TRUE);
    }
    else
    {
        EINT_Registration(spi_wod, KAL_TRUE, LEVEL_HIGH, track_eint_resp_CSPI_high, KAL_TRUE);
    }
    EINT_UnMask(spi_wod);

}
#endif

static void track_eint_resp_C_high(void)
{
    if(EINT_C_NO == 0xFF) return;
    EINT_Mask(EINT_C_NO);
    track_eint_trigger_response(EINT_C_NO, LEVEL_HIGH);
    EINT_Registration(EINT_C_NO, KAL_TRUE, LEVEL_LOW, track_eint_resp_C_low, KAL_FALSE);
    EINT_UnMask(EINT_C_NO);
}
static void track_eint_resp_C_low(void)
{
    if(EINT_C_NO == 0xFF) return;
    EINT_Mask(EINT_C_NO);
    track_eint_trigger_response(EINT_C_NO, LEVEL_LOW);
    EINT_Registration(EINT_C_NO, KAL_TRUE, LEVEL_HIGH, track_eint_resp_C_high, KAL_FALSE);
    EINT_UnMask(EINT_C_NO);
}


#if defined( __GT02F__)||defined(__GT06F__) || defined(__GT800__)||defined(__NT31__) || defined (__NT36__)|| defined (__NT37__)|| defined(__NT22__) 
static void track_eint_D_registration(void)
{
    LOGD(L_DRV, L_V6, "EINTD %d, GPIO:%d, M:%d, %d", EINT_D_NO, EINT_D_GPIO, EINT_D_EINT_MODE, kal_get_active_module_id());
        
    EINT_Mask(EINT_D_NO);
    GPIO_ModeSetup(EINT_D_GPIO, EINT_D_EINT_MODE);
    EINT_SW_Debounce_Modify(EINT_D_NO, EINT_SW_DEBOUNCE_TIME);
    EINT_Set_Sensitivity(EINT_D_NO, EINT_SENSITIVE_MODE);    
        
    if(track_GPIO_ReadIO(EINT_D_GPIO))
    {
        EINT_Registration(EINT_D_NO, KAL_TRUE, LEVEL_LOW, track_eint_resp_D_low, KAL_TRUE);
    }
    else
    {
        EINT_Registration(EINT_D_NO, KAL_TRUE, LEVEL_HIGH, track_eint_resp_D_high, KAL_TRUE);
    }
    EINT_UnMask(EINT_D_NO);
}

static void track_eint_resp_D_high(void)
{
    EINT_Mask(EINT_D_NO);
    track_eint_trigger_response(EINT_D_NO, LEVEL_HIGH);
    EINT_Registration(EINT_D_NO, KAL_TRUE, LEVEL_LOW, track_eint_resp_D_low, KAL_FALSE);
    EINT_UnMask(EINT_D_NO);
}
static void track_eint_resp_D_low(void)
{
    EINT_Mask(EINT_D_NO);
    track_eint_trigger_response(EINT_D_NO, LEVEL_LOW);
    EINT_Registration(EINT_D_NO, KAL_TRUE, LEVEL_HIGH, track_eint_resp_D_high, KAL_FALSE);
    EINT_UnMask(EINT_D_NO);
}

#if defined(__GT06F__) || defined (__GT800__)||defined(__NT31__) 
static void track_eint_resp_AUX_low(void);
// AUX
/******************************************************************************
 *  Function    -  track_eint_AUX_hisr
 ******************************************************************************/
static void track_eint_resp_AUX_high(void)
{
    LOGD(L_DRV, L_V5, "EINTB %d, GPIO:%d, M:%d", AUX_EINT_NO, AUX_EINT_GPIO_NO, GPIO_ReadIO(AUX_EINT_GPIO_NO));

    EINT_Mask(AUX_EINT_NO);
    track_eint_trigger_response(AUX_EINT_NO, LEVEL_HIGH);
    EINT_Registration(AUX_EINT_NO, KAL_TRUE, LEVEL_LOW, track_eint_resp_AUX_low, KAL_FALSE);
    EINT_UnMask(AUX_EINT_NO);
}
static void track_eint_resp_AUX_low(void)
{
    LOGD(L_DRV, L_V5, "EINTB %d, GPIO:%d, M:%d", AUX_EINT_NO, AUX_EINT_GPIO_NO, GPIO_ReadIO(AUX_EINT_GPIO_NO));

    EINT_Mask(AUX_EINT_NO);
    track_eint_trigger_response(AUX_EINT_NO, LEVEL_LOW);
    EINT_Registration(AUX_EINT_NO, KAL_TRUE, LEVEL_HIGH, track_eint_resp_AUX_high, KAL_FALSE);
    EINT_UnMask(AUX_EINT_NO);
}

static void track_eint_AUX_hisr(void)
{
    kal_bool eint_Aux_status;
    if(AUX_EINT_NO == 0xFC) return;
    EINT_Mask(AUX_EINT_NO);

    if(GPIO_ReadIO(AUX_EINT_GPIO_NO))
    {
        track_eint_trigger_response(AUX_EINT_NO, LEVEL_HIGH);
        eint_Aux_status = LEVEL_LOW;
    }
    else
    {
        track_eint_trigger_response(AUX_EINT_NO, LEVEL_LOW);
        eint_Aux_status = LEVEL_HIGH;
    }
    LOGD(L_DRV, L_V5, "AUX_EINT_NO %d,%d", eint_Aux_status, kal_get_active_module_id());
    EINT_SW_Debounce_Modify(AUX_EINT_NO, EINT_SW_DEBOUNCE_TIME);
    EINT_Set_Polarity(AUX_EINT_NO, eint_Aux_status);
    EINT_UnMask(AUX_EINT_NO);
}
/******************************************************************************
 *  Function    -  track_eint_AUX_registration
 *  Purpose     -  中断A 初始化
 *  Description -
 * modification history
 * ----------------------------------------
 * ----------------------------------------
 ******************************************************************************/
static void track_eint_AUX_registration(void)
{
    if(AUX_EINT_NO == 0xFC) return;
    EINT_Mask(AUX_EINT_NO);
    LOGD(L_DRV, L_V5, "EINTB %d, GPIO:%d, M:%d", AUX_EINT_NO, AUX_EINT_GPIO_NO, kal_get_active_module_id());
    GPIO_ModeSetup(AUX_EINT_GPIO_NO, 2);
    EINT_SW_Debounce_Modify(AUX_EINT_NO, EINT_SW_DEBOUNCE_TIME);
    EINT_Set_Sensitivity(AUX_EINT_NO, EINT_SENSITIVE_MODE);
    if(GPIO_ReadIO(AUX_EINT_GPIO_NO))
    {
        track_eint_trigger_response(AUX_EINT_NO, LEVEL_HIGH);
        EINT_Registration(AUX_EINT_NO, KAL_TRUE, LEVEL_LOW, track_eint_resp_AUX_low, KAL_TRUE);
    }
    else
    {
        track_eint_trigger_response(AUX_EINT_NO, LEVEL_LOW);
        EINT_Registration(AUX_EINT_NO, KAL_TRUE, LEVEL_HIGH, track_eint_resp_AUX_high, KAL_TRUE);
    }
    EINT_UnMask(AUX_EINT_NO);

}


#endif
#endif /* __GT02F__ */

/*为什么删除呢？       --    chengjun  2017-03-20*/
/*稳定供电时正常，不接外设，RS485转换芯片不上电，有干扰，这个边沿触发中断难过滤 ，导致不休眠      --    chengjun  2017-03-20*/
#if 0
defined (__ET350__) && defined (__CUST_RS485__)
/******************************************************************************
 *  Function    -  track_drv_set_gpio_uart3_or_eint_mode
 *
 *  Purpose     -  中断与串口复用，切换GPIO模式
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-02-09,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_set_gpio_uart3_or_eint_mode(kal_uint8 mode)
{
    if(mode == 1)
    {
        //uart3
        GPIO_ModeSetup(EINT_EDGE_GPIO, 3);//Eint0-->uart3Rx
        GPIO_ModeSetup(1, 3);//Gpio1-->uart3Tx
    }
    else if(mode == 2)
    {
        //eint
        GPIO_ModeSetup(EINT_EDGE_GPIO, 1);//-->Eint0
        GPIO_ModeSetup(1, 0);
        GPIO_WriteIO(1, 0);
    }

    track_gpio_uart3_or_eint_mode(mode);
}


static void track_eint_edge_hisr(void)
{
    static kal_uint32 last_tick=0;
    kal_uint32 tick=0;
    
    EINT_Mask(EINT_EDGE_NO);

    tick = kal_get_systicks();
    
    if ((tick-last_tick)>20)
    {
        LOGD(L_DRV, L_V5, "过滤本次中断");
        last_tick=tick;
        EINT_UnMask(EINT_EDGE_NO);
        return;
    }

    GPIO_ModeSetup(EINT_EDGE_GPIO, 3);//Eint0-->uart3Rx
    last_tick=tick;
    track_drv_set_gpio_uart3_or_eint_mode(1);
    
    track_eint_trigger_response(EINT_EDGE_NO, LEVEL_LOW);
    EINT_UnMask(EINT_EDGE_NO);
}

/******************************************************************************
 *  Function    -  track_eint_edge_registration
 * 
 *  Purpose     -  新增处理边沿触发的中断
 * 
 *  Description -   下降沿触发
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2017-02-08,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_eint_edge_registration(void)
{
    LOGD(L_DRV, L_V5, "EINT-%d Uart3 RX eint enable", EINT_EDGE_NO);
    EINT_Mask(EINT_EDGE_NO);
        
    //休眠时切换模式
    track_drv_set_gpio_uart3_or_eint_mode(2);

    EINT_Registration(EINT_EDGE_NO, KAL_FALSE, 0, track_eint_edge_hisr, KAL_FALSE);
    EINT_Set_Sensitivity(EINT_EDGE_NO, EDGE_SENSITIVE);

    EINT_UnMask(EINT_EDGE_NO);
}
#endif

/******************************************************************************
 *  Function    -  track_eint_trigger_response
 *
 *  Purpose     -  中断触发
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 29-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_eint_trigger_response(U8 eint, U8 level)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct *ilm_ptr;
    eint_status_struct *myMsgPtr = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    myMsgPtr = (eint_status_struct*) construct_local_para(sizeof(eint_status_struct), TD_CTRL);
    myMsgPtr->eint = eint;
    myMsgPtr->level = level;

    ilm_ptr = (ilm_struct *)allocate_ilm(MOD_DRV_HISR);
    /*注意:所有中断即使在MOD_MMI注册，track_drv_eint_trigger_msg()
    其响应函数是在MOD_DRV_HISR --  chengjun  2013-05-16*/
    ilm_ptr->src_mod_id = MOD_DRV_HISR;
    ilm_ptr->dest_mod_id = MOD_MMI;
    ilm_ptr->sap_id = INVALID_SAP;
    ilm_ptr->msg_id = (msg_type) MSG_ID_EINT_TRIGGER_STATUS;
    ilm_ptr->local_para_ptr = (local_para_struct*) myMsgPtr;
    ilm_ptr->peer_buff_ptr = NULL;
    msg_send_ext_queue(ilm_ptr);
}

#if 1
/*****************************************************************************
*  Global Functions                     全局函数
******************************************************************************/
#endif

/******************************************************************************
 *  Function    -  track_drv_eint_init_registration
 *
 *  Purpose     -  注册需要使用的中断口
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 26-07-2012,  chengjun  written
 * v1.1  , 16-05-2013,  cml       written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_eint_init_registration(void)
{
#if defined(__V20__) || defined(__GT800__) || defined(__NT22__)   ||defined(__MT200__) || defined(__GT03D__) || defined(__GT06D__) || defined(__ET200__) || defined(__GT310__)|| defined(__ET130__) || defined(__GT740__)||defined(__GT420D__) || defined(__NT23__) || defined(__ET310__) || defined(__ET320__) || defined(__NT31__) || defined(__NT36__)|| defined(__ET350__)|| defined (__NT37__) || (defined (__NT33__)&&defined (__FUN_WITH_ACC__)) || defined (__HVT001__)
    if (EINT_A_NO != EINT_CHANNEL_NOT_EXIST)
    {
        track_eint_A_registration();
    }
#if !defined(__UART1_SOS__)
    if (EINT_B_NO != EINT_CHANNEL_NOT_EXIST)
    {
        track_eint_B_registration();
    }
#endif
#endif /* __GT06B__ */

#if defined(__NT22__)  ||defined(__GT02F__) ||defined(__GT06F__)|| defined (__GT800__)|| defined(__NT31__) || defined (__NT36__) || defined (__NT33__)|| defined (__NT37__)
    if (EINT_C_NO != 0xFF)
    {
        track_eint_C_registration();
    }
#if defined(__GT02F__)|| defined(__NT22__) || defined(__GT800__)|| defined(__NT31__) || defined (__NT36__)|| defined (__NT37__)
    if (EINT_D_NO != 0xFE)
    {
        track_eint_D_registration();
    }
#endif
#if defined(__GT06F__) || defined( __GT800__)|| defined(__NT31__)
    if (AUX_EINT_NO != 0xFC)
    {
        track_eint_AUX_registration();
    }
#endif
#endif 

}

/******************************************************************************
 *  Function    -  track_drv_eint_trigger_msg
 *
 *  Purpose     -  中断消息处理
 *
 *  Description -  中断触发发消息处理，不直接调用
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 29-07-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_eint_trigger_msg(void *msg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    eint_status_struct *localBuff_p = (eint_status_struct*)msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    #if !defined(__GT420D__)
    LOGD(L_DRV, L_V5, "Eint%d , Level=%d", localBuff_p->eint, localBuff_p->level);
    #endif
#if defined (__TEST_PCBA__)
    track_drv_test_pcba_eint_status_updata(localBuff_p->eint, localBuff_p->level);
#endif /* __TEST_PCBA__ */

    track_drv_eint_trigger_response(localBuff_p->eint, localBuff_p->level);
}
/******************************************************************************
 *  Function    -  track_set_GPIO21_CUR
 *  Purpose     -  设置GPIO输出口电流
 *  0xA0020810  GPIO_DRV1
 *  GPIO20          KROW1       [7:6]
 *  GPIO21          KROW0       [9:8]
 *  00                  4ma
 *  01                  8ma
 *  10                  12ma
 *  11                  16ma
 * modification history
 * ----------------------------------------
 * v1.0  , 20150911,  wangqi  written
 * ----------------------------------------
 ******************************************************************************/
#define SFI_ReadReg32(addr)         *((volatile unsigned int *)(addr))
#define SFI_WriteReg32(addr, data)  *((volatile unsigned int *)(addr)) = (unsigned int)(data)
void track_set_GPIO21_CUR(kal_uint8 level)
{
    unsigned int drv1_value = *((volatile unsigned int *)(0xA0020810));
    if(level == 4)
    {
        //修改GPIO21的输出电流   1111 1100  1111 1111    9 8
        SFI_WriteReg32(0xA0020810, (SFI_ReadReg32(0xA0020810)&(0xFFFFFCFF)));  // driving 4mA
    }
    else if(level == 16)
    {
        SFI_WriteReg32(0xA0020810, (SFI_ReadReg32(0xA0020810) | 0x00000300)); // driving 16mA
    }
    //LOGS("GPIO21 [9:8]:(drv1_value:%x,%x)", drv1_value, *((volatile unsigned int *)(0xA0020810)));

}
void track_set_GPIO20_CUR(kal_uint8 level)
{
    unsigned int drv1_value = *((volatile unsigned int *)(0xA0020810));

    if(level == 4)
    {
        //修改GPIO20 KROW1的输出电流   1111 1111 0011 1111   7  6
        SFI_WriteReg32(0xA0020810, (SFI_ReadReg32(0xA0020810)&(0xFFFFFF3F))); //SLT driving 4mA
    }
    else if(level == 16)
    {
        SFI_WriteReg32(0xA0020810, (SFI_ReadReg32(0xA0020810) | 0x000000C0)); // driving 16mA
    }
    //LOGS("GPIO20 [7:6]:(drv1_value:%x,%x)", drv1_value, *((volatile unsigned int *)(0xA0020810)));

}

void track_set_GPIO29_CUR(kal_uint8 level)
{
    unsigned int drv1_value = *((volatile unsigned int *)(0xA0020810));

    if (level == 4)
    {//修改GPIO29 CMMCLK的输出电流   1100 1111 1111 1111   13  12
        SFI_WriteReg32(0xA0020810, (SFI_ReadReg32(0xA0020810)&(0xFFFFCFFF))); //SLT driving 4mA
    }
    else if (level == 16)
    {
        SFI_WriteReg32(0xA0020810, (SFI_ReadReg32(0xA0020810) | 0x00003000)); // driving 16mA
    }
    //LOGS("GPIO29 [13:12]:(drv1_value:%x,%x)", drv1_value, *((volatile unsigned int *)(0xA0020810)));

}
/******************************************************************************
 *  Function    -  track_drv_gpio_init
 *
 *  Purpose     -  GPIO 初始化
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 01-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_gpio_init(void)
{
    LOGD(L_DRV, L_V5, "CUT:%d,433IO:%d", CUT_OIL_ELE_GPIO, CTRL_433_GPIO);
#if defined (__GT800__)||defined(__NT31__) || defined (__ET310__) ||defined(__ET320__) || defined(__MT200__)|| defined(__NT36__)|| defined(__NT22__) 
	track_set_GPIO29_CUR(16);
#endif

    if(0xff != CUT_OIL_ELE_GPIO)
    {
        GPIO_ModeSetup(CUT_OIL_ELE_GPIO, 0);
        GPIO_InitIO(1, CUT_OIL_ELE_GPIO);
#if defined(__GT02F__) || defined(__V20__) ||defined(__GT06F__) ||defined(__ET210__) || defined(__MT200__)
        track_set_GPIO21_CUR(16);
#endif
    }
    if(0xff != CTRL_433_GPIO)
    {
        GPIO_ModeSetup(CTRL_433_GPIO, 0);
        GPIO_InitIO(1/*OUTPUT*/, CTRL_433_GPIO);
    }
    if(0xff != AP_433_GPIO)
    {
        GPIO_ModeSetup(AP_433_GPIO, 0);
        GPIO_InitIO(1/*OUTPUT*/, AP_433_GPIO);
    }
    if(0xff != CUST_OUT1_GPIO)
    {
        GPIO_ModeSetup(CUST_OUT1_GPIO, 0);
        GPIO_InitIO(1/*OUTPUT*/, CUST_OUT1_GPIO);
#if defined(__GT02F__) || defined(__V20__) ||defined(__GT06F__)
        GPIO_WriteIO(0, CUST_OUT1_GPIO);//拉低IO
        track_set_GPIO20_CUR(16);
#endif
    }
#if defined(__MT200__) && defined(__FLY_MODE__)    
    if(0xff != EXT_POWER_GPIO)
    {
        GPIO_ModeSetup(EXT_POWER_GPIO, 0);
        GPIO_InitIO(1/*OUTPUT*/, EXT_POWER_GPIO);
    }
#endif    
#if defined( __ET210__)
    track_drv_set_vcama(KAL_TRUE, 2800000);
#endif /* __ET210__ */

#if defined(__GT02F__) ||defined(__GT06F__)

#elif defined( __V20__)
    track_drv_Set_VMC(KAL_TRUE, 2);
#endif /* __V20__ */

#if defined (__ET350__) && defined (__CUST_RS485__)
    track_drv_rs485_gpio_init();
#endif

#if defined (__NT37__)
    //高输出5V供电
    GPIO_ModeSetup(CTRL_5V_OUT_GPIO, 0);
    GPIO_InitIO(1, CTRL_5V_OUT_GPIO);
    GPIO_WriteIO(1, CTRL_5V_OUT_GPIO);
#endif /* __NT37__ */
}

kal_uint8  track_drv_gpio_out1(kal_int8 state)
{
    kal_uint8 rece;
    LOGD(L_DRV, L_V5, "rece=%d,state=%d", rece, state);
	if(CUST_OUT1_GPIO == 0xFF)
	{
		return 0;
	}
    rece = GPIO_ReadIO(CUST_OUT1_GPIO);
    if(state == 0 || state == 1)
    {
        if(rece != state)
        {
            int i;
            GPIO_WriteIO(state, CUST_OUT1_GPIO);
            rece = GPIO_ReadIO(CUST_OUT1_GPIO);
        }
    }
    return rece;
}


kal_uint8  track_drv_gpio_out2(kal_int8 state)
{
    kal_uint8 rece;
    LOGD(L_DRV, L_V5, "rece=%d,state=%d", rece, state);
	if(CUST_OUT2_GPIO == 0xFF)
	{
		return 0;
	}
		
    rece = GPIO_ReadIO(CUST_OUT2_GPIO);
    if(state == 0 || state == 1)
    {
        if(rece != state)
        {
            int i;
            GPIO_WriteIO(state, CUST_OUT2_GPIO);
            rece = GPIO_ReadIO(CUST_OUT2_GPIO);
        }
    }
    return rece;
}



/*2503平台GPIO3 需要更换读取函数wangqi20160409*/
/*GPIO回读接口升级  --    chengjun  2016-07-13*/
char track_GPIO_ReadIO(kal_uint16 port)
{
    char rece = 0;

    #if 1
    rece = OTA_GPIO_ReturnDout(port);
    #else
    if(3 == port)
    {
        rece = OTA_GPIO_ReturnDout(port);
    }
    else
    {
        rece = GPIO_ReadIO(port);
    }
    #endif
    return rece;
}
/******************************************************************************
 *  Function    -  track_drv_gpio_cut_oil_elec
 *
 *  Purpose     -  控制断油电
 *
 *  Description - state [in] 1 = 断油电/0 = 恢复  其它值查询返回当前的状态
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 01-08-2012,  chengjun  written
 * v1.1  , 05-10-2013,  cml       written
 * ----------------------------------------
 ******************************************************************************/
kal_int8 track_drv_gpio_cut_oil_elec(kal_int8 state)
{
    char rece;
    rece = track_GPIO_ReadIO(CUT_OIL_ELE_GPIO);

    if(state == 0 || state == 1)
    {
        if(rece != state)
        {
            int i;
            for(i = 0; i < 3; i++)
            {
                // 最多重复设置3次，防止设置失败。
                GPIO_WriteIO(state, CUT_OIL_ELE_GPIO);
                rece = track_GPIO_ReadIO(CUT_OIL_ELE_GPIO);
                if(rece == state) return rece;
            }
            if(rece != state) return -1;
        }
    }
    return rece;
}
#if defined(__BCA__)
S8 track_drv_gpio_pw5v(S8 sw)
{
    char rece;
    GPIO_ModeSetup(0, 0);
    rece = track_GPIO_ReadIO(0);
    LOGD(L_DRV, L_V5, "%d -> %d", rece, sw);
    GPIO_InitIO(sw, 0);
    GPIO_WriteIO(sw, 0);
    return rece;
}
#endif
kal_int8 track_drv_gpio_set(kal_int8 state, kal_int8 port)
{
    char rece;
    rece = track_GPIO_ReadIO(port);
    LOGD(L_DRV, L_V5, "port%d=%d -> %d", port, rece, state);
    if(state == 0 || state == 1)
    {
        if(rece != state)
        {
            int i;
            for(i = 0; i < 3; i++)
            {
                // 最多重复设置3次，防止设置失败。
                GPIO_WriteIO(state, port);
                rece = track_GPIO_ReadIO(port);
                if(rece == state) return rece;
            }
            if(rece != state)
            {
                LOGS("Can not option GPIO:%d", port);
                return -1;
            }
        }
    }
    return rece;
}

void track_drv_set_gpio_low(void* port)
{
	kal_uint8 pr = (kal_uint8)port;
	track_drv_gpio_set(0,pr);
    LOGD(L_DRV, L_V5, "外电GPIO%d拉低",pr);
}

/******************************************************************************
 *  Function    -  track_drv_gpio_disconnect_ext_power
 *
 *  Purpose     -  外电导通控制
 *
 *  Description -  state
 *                 1         断开外电
 *                 0         导通外电
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 26-08-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int8 track_drv_gpio_disconnect_ext_power(kal_int8 state)
{
#if defined(__EXT_VBAT_ADC__)
    char rece;
    if(0xFF == EXT_POWER_GPIO)
    {
        LOGD(L_DRV, L_V5, "无IO控制");
        return;
    }
    
    //rece = GPIO_ReadIO(EXT_POWER_GPIO);
    rece = OTA_GPIO_ReturnDout(EXT_POWER_GPIO);

    
    if(state == 0 || state == 1)
    {
#if defined __GT100__
        track_cust_module_set_power_saving_mode(state);
#endif /* __GT100__ */
        if(rece != state)
        {
            int i;
            for(i = 0; i < 3; i++)
            {
                // 最多重复设置3次，防止设置失败。
                GPIO_WriteIO(state, EXT_POWER_GPIO);
                
                //rece = GPIO_ReadIO(EXT_POWER_GPIO);
                rece = OTA_GPIO_ReturnDout(EXT_POWER_GPIO);
                
                if(rece == state)
                {
                    LOGD(L_DRV, L_V5, "return state=%d==race=%d",state,rece);
#if defined(__MT200__)
            LOGD(L_DRV, L_V5, "state=%d",state);
		if(1==state)
		{
		    	track_start_timer(TRACK_CUST_EXT_POWER_GPIO_LOW_TIMER_ID, 1000, track_drv_set_gpio_low,(void*)EXT_POWER_GPIO);// 脉冲
		}
#endif                    
                    return rece;
                }
            }
            if(rece != state)
            {
                LOGS("!外电控制口执行动作不成功!current:%d", rece);
                return -1;
            }
        }		
    }
    return rece;
#else
    return -1;
#endif /* __EXT_VBAT_ADC__ */
}

/******************************************************************************
 *  Function    -  track_drv_set_vibr
 *
 *  Purpose     -  震动马达(默认输出3.3V)
 *
 *  Description -  enable [in]
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 01-03-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_set_vibr(kal_bool enable)
{

    DCL_HANDLE handle;
    PMU_CTRL_LDO_BUCK_SET_EN val;
    static kal_bool first = KAL_TRUE;
    PMU_CTRL_LDO_BUCK_SET_VOLTAGE pmuCtrlVoltage;

    LOGD(L_APP, 1, "");
    if(first)
    {
        first = KAL_FALSE;
        PMU_DRV_SetData16(0xA07001B0, 0x70, /*PMU_VIBR_3300_MV*/PMU_VIBR_3000_MV);//WangQi
    }

    val.enable = enable;
    val.mod = VIBR;
    handle = DclPMU_Open(DCL_PMU, FLAGS_NONE);
    DclPMU_Control(handle, LDO_BUCK_SET_EN, (DCL_CTRL_DATA_T *)&val);
    pmuCtrlVoltage.voltage = PMU_VOLT_03_000000_V;
    pmuCtrlVoltage.mod = VIBR;
    DclPMU_Control(handle, LDO_BUCK_SET_VOLTAGE, (DCL_CTRL_DATA_T *)&pmuCtrlVoltage);
    DclPMU_Close(handle);

    /*
    DCL_HANDLE handle;
    PMU_CTRL_LDO_BUCK_SET_VOLTAGE pmuCtrlVoltage;
    handle = DclPMU_Open(DCL_PMU, FLAGS_NONE);
    pmuCtrlVoltage.voltage=PMU_VOLT_03_000000_V;
    pmuCtrlVoltage.mod=VIBR;
    DclPMU_Control(handle, LDO_BUCK_SET_VOLTAGE, (DCL_CTRL_DATA_T *)&pmuCtrlVoltage);
    DclPMU_Close(handle);*/
}

void track_drv_set_sim_type(kal_uint16 simtype)
{
    if(simtype < SIM_STATE_MAX)
    {
        lte_sms_state = simtype;
    }
    LOGD(L_DRV, L_V7, "lte_sms_state:%d,simtype:%d", lte_sms_state, simtype);
}
/******************************************************************************
 *  Function    -  track_drv_get_sim_type
 *
 *  Purpose     -  查询SIM 卡状态/类型
 *
 *  Description -
 *
 *      return : 0=无卡/1=有普通SIM 卡/ 2=有测试卡/-1=未初始化/-2=异常/
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 28-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_int8 track_drv_get_sim_type(void)
{
    sim_card_status_enum status;
    kal_int8 sim = 0;

#if defined( __GT530__)
    //if(lte_sms_state == SIM_STATE_MAX)//持续获取IMSI号
    {
        track_drv_check_sim();
    }

    if(lte_sms_state > SIM_STATE_READY)
    {
        sim = 1 - (kal_uint8)lte_sms_state;
    }
    else
    {
        sim = (kal_uint8)lte_sms_state;
    }

#else
    status = check_sim_card_status();
    if(status == SIM_CARD_NOT_INSERTED)
    {
        sim = 0;
    }
    else if(status == NORMAL_SIM_PRESENCE)
    {
        sim = 1;
    }
    else if(status == TEST_SIM_PRESENCE)
    {
        sim = 2;
    }
    else if(status == SIM_NOT_READY)
    {
        sim = -1;
    }
    else
    {
        sim = -2;
    }
#endif /* __GT530__ */
    LOGD(L_DRV, L_V6, "sim=%d,%d,%d", sim, lte_sms_state, status);
    return sim;

}


/******************************************************************************
 *  Function    -  track_remind_sim_for_factory
 *
 *  Purpose     -  为工厂测试提示SIM 卡状态
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 30-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_remind_sim_for_factory(kal_int8 sim)
{
    char str[20] = {0};
    if(sim == 0)
    {
        sprintf(str, "No SIM");
    }
    else if(sim == 1)
    {
        sprintf(str, "SIM OK");
    }
    else if(sim == 2)
    {
        sprintf(str, "Test card");
    }
    else
    {
        sprintf(str, "SIM error");
    }
    LOGS("%s", str);
}

/******************************************************************************
 *  Function    -  track_drv_query_sim_type
 *
 *  Purpose     -  读SIM 卡类型
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 23-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_query_sim_type(void)
{
    static kal_uint8 count=0;
    kal_int8 sim_type;

    sim_type = track_drv_get_sim_type();
    track_remind_sim_for_factory(sim_type);
    track_drv_remind_sim_type(sim_type);

    if(sim_type<=0)
    {
        count++;
        if(count<3)
        {
            LOGD(L_DRV, L_V5, "%d,%d", sim_type, count);
            tr_start_timer(TRACK_QUERY_SIM_TYPE_TIMER_ID, 5000, track_drv_query_sim_type);
        }
    }
}


/******************************************************************************
 *  Function    -  track_drv_set_vcama
 *  Purpose     -  GPS开启VCAMA供电
 *  Description -
 * modification history
 * ----------------------------------------
 * v1.0  , 20150303,    written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_set_vcama(kal_bool ON,/* kal_uint32 ldo,*/ kal_uint32 ldo_volt)
{
    PMU_CTRL_LDO_BUCK_SET_VOLTAGE_EN pmu_ldo_voltage_en;
    PMU_CTRL_LDO_BUCK_SET_EN pmu_ldo_en;
    DCL_HANDLE ctp_pmu_handle;

    LOGD(L_DRV, L_V7, "ON/OFF:%d, volt:%d", ON, ldo_volt);

    if(ctp_pmu_handle == DCL_HANDLE_NONE)
    {
        ctp_pmu_handle = DclPMU_Open(DCL_PMU, FLAGS_NONE);
    }
    if(ON && (ldo_volt != 0))
    {
        pmu_ldo_voltage_en.mod = (PMU_LDO_BUCK_LIST_ENUM)VCAMA;
        pmu_ldo_voltage_en.voltage = (PMU_VOLTAGE_ENUM)ldo_volt;
        DclPMU_Control(ctp_pmu_handle, LDO_BUCK_SET_VOLTAGE_EN, (DCL_CTRL_DATA_T *)& pmu_ldo_voltage_en);
    }
    else if(ON && (ldo_volt == 0))
    {
        pmu_ldo_en.mod = (PMU_LDO_BUCK_LIST_ENUM)VCAMA;
        pmu_ldo_en.enable = KAL_TRUE;
        DclPMU_Control(ctp_pmu_handle, LDO_BUCK_SET_EN, (DCL_CTRL_DATA_T *)&pmu_ldo_en);
    }
    else
    {
        pmu_ldo_en.mod = (PMU_LDO_BUCK_LIST_ENUM)VCAMA;
        pmu_ldo_en.enable = KAL_FALSE;
        DclPMU_Control(ctp_pmu_handle, LDO_BUCK_SET_EN, (DCL_CTRL_DATA_T *)&pmu_ldo_en);
    }
}
#if defined(__GT530__)
/*****************************************************************************
 *  Define			    宏定义
*****************************************************************************/

#define LTE_PWR_GPIO                    6
#define LTE_RESET_GPIO                  7
#define LTE_WAKE_GPIO                    45
#define LTE_UART1_CTRL_GPIO             4
#define LTE_REVERT_SIZE                 4
#define LTE_WDT_GPIO                    50
#define LTE_SIX_MINS                     420//4G模块启动时间大约360+60
/*****************************************************************************
* Typedef  Enum         枚举
*****************************************************************************/


/*****************************************************************************
 *  Struct			    数据结构定义
*****************************************************************************/
typedef struct
{
    kal_uint8 gpio_port;
    kal_uint8 r_status;
} gpio_revert_struct;

/*****************************************************************************
* Local variable            局部变量
*****************************************************************************/
static gpio_revert_struct gpio_revert[LTE_REVERT_SIZE] =
{
    {LTE_PWR_GPIO,           0},
    {LTE_RESET_GPIO,         0},
    {LTE_WAKE_GPIO,           0},
    {LTE_UART1_CTRL_GPIO,    0}
};

/****************************************************************************
* Global Variable - Extern          引用全局变量
*****************************************************************************/


/*****************************************************************************
 *  Global Functions	- Extern	    引用外部函数
*****************************************************************************/
extern void track_drv_lte_reset_pwron(void* op);


#define WDT_DELAY_TIME_1MS              400  //大约
static void  MS_Delay(kal_uint16 delay)
{
    kal_uint16 i;
    while(delay--)
    {
        for(i = 0; i < WDT_DELAY_TIME_1MS; i++)
        {
            ;
        }
    }
}

kal_uint8 track_drv_lte_systemup(kal_uint8 work)
{
    static kal_uint8 iswork = 0;
    LOGD(L_DRV, L_V7, "%d,%d,Test:%d", iswork, work, track_is_testmode());
    if(work < 2)
    {
        iswork = work;
    }
    return iswork;
}

/******************************************************************************
 *  Function    -  track_drv_lte_pwron_low
 *  Purpose     -
 *  Description -
 *  modification history
 * ----------------------------------------
 * v1.0  , 20150129,  wangqi  written
 * ----------------------------------------
*******************************************************************************/
static void track_drv_lte_revert_GPIO(void* index)
{
    kal_uint32 gpio_index = (kal_uint32)index;
    LOGD(L_DRV, L_V5, "%d", index);
    if(gpio_index > LTE_REVERT_SIZE)
    {
        LOGD(L_DRV, L_V5, "Error index :%d, cannot > %d", gpio_index, LTE_REVERT_SIZE);
        return;
    }
    if(-1 == track_drv_gpio_set(gpio_revert[gpio_index].r_status, gpio_revert[gpio_index].gpio_port))
    {

    }
    if(0 == gpio_index)
    {
        track_drv_lte_init_wdt(0);
    }

}

kal_uint32 track_drv_lte_init_wdt(kal_uint8 op)
{
    LOGD(L_DRV, L_V6, "%d", op);

    if(0 == op)
    {
        track_start_timer(LTE_DECT_WDT_TIMER_ID, LTE_SIX_MINS * 1000, track_drv_lte_reset_pwron, (void*)0);
    }
    else if(1 == op)
    {
        track_start_timer(LTE_DECT_WDT_TIMER_ID, 60000, track_drv_lte_reset_pwron, (void*)1);
    }
    else if(0xFF == op)
    {
        track_stop_timer(LTE_DECT_WDT_TIMER_ID);
    }

    return 1;
}
/******************************************************************************
 *  Function    -  track_drv_lte_pwron
 *  Purpose     -
 *  Description -  s=0长按开机   s=1短按唤醒
 *  modification history
 * ----------------------------------------
 * v1.0  , 20150129,  wangqi  written
 * ----------------------------------------
*******************************************************************************/
void track_drv_lte_pwron(kal_uint8 s)
{
    kal_uint32 dealy = 2000;
    GPIO_ModeSetup(LTE_PWR_GPIO, 0);
    GPIO_InitIO(1/*OUTPUT*/, LTE_PWR_GPIO);
    if(-1 == track_drv_gpio_set(1, LTE_PWR_GPIO))
    {
    }
    LOGD(L_DRV, L_V5, "status:%d %d", s, dealy);
    track_start_timer(DEALY_INIT_POWERKEY_TIMER_ID, dealy, track_drv_lte_revert_GPIO, (void*)0);

}
/******************************************************************************
 *  Function    -  track_drv_lte_reset
 *  Purpose     -
 *  Description -  RESET机闭4G模块
 *  modification history
 * ----------------------------------------
 * v1.0  , 20150129,  wangqi  written
 * ----------------------------------------
*******************************************************************************/
static void track_drv_lte_reset(void)
{
    LOGD(L_DRV, L_V5, "");
#if 0
    GPIO_ModeSetup(LTE_RESET_GPIO, 0);
    GPIO_InitIO(1/*OUTPUT*/, LTE_RESET_GPIO);
    track_drv_gpio_set(1, LTE_RESET_GPIO);
    track_start_timer(DEALY_INIT_RESET_TIMER_ID, 120, track_drv_lte_revert_GPIO, (void*)1);
#endif
    track_drv_encode_exmode(0x77, 0x0101, 0x91, "RESET", 5);

}
/******************************************************************************
 *  Function    -  track_drv_lte_wake
 *  Purpose     -  输出给LTE使其进入下载模式
 *  Description -
 *  modification history
 * ----------------------------------------
 * v1.0  , 20150129,  wangqi  written
 * ----------------------------------------
*******************************************************************************/
void track_drv_lte_wake(void)
{
    LOGD(L_DRV, L_V5, "");
    GPIO_ModeSetup(LTE_WAKE_GPIO, 0);
    GPIO_InitIO(1/*OUTPUT*/, LTE_WAKE_GPIO);
    track_drv_gpio_set(1, LTE_WAKE_GPIO);
    track_start_timer(LTE_WAKE_KT_TIMER_ID, 100, track_drv_lte_revert_GPIO, (void*)2);
    MS_Delay(2);
    LOGD(L_DRV, L_V5, "end");
}

/******************************************************************************
 *  Function    -  track_drv_lte_uart1_ctrl
 *  Purpose     -  UART通讯电平转换需要与LTE通讯时拉高
 *  Description -
 *  modification history
 * ----------------------------------------
 * v1.0  , 20150129,  wangqi  written
 * ----------------------------------------
*******************************************************************************/
void track_drv_lte_uart1_ctrl1(const char * fn, kal_uint8 s)
{
    LOGD(L_DRV, L_V5, "fn:%s,status:%d", fn, s);
    GPIO_ModeSetup(LTE_UART1_CTRL_GPIO, 0);
    GPIO_InitIO(1/*OUTPUT*/, LTE_UART1_CTRL_GPIO);
    if(1 < s)
    {
        LOGD(L_DRV, L_V5, "ERROR status:%d", s);
        return;
    }
    track_drv_gpio_set(s, LTE_UART1_CTRL_GPIO);

}
/******************************************************************************
 *  Function    -  track_drv_lte_reset_pwron
 *  Purpose     -  正常情况下0 RESET，测试模块下增加2 RESET
                         0 ,2,4,8,16,32,64= RESET
                        TEST 128==POWEROFF 256 ==RESET
 *  Description -
 *  modification history
 * ----------------------------------------
 * v1.0  , 20150129,  wangqi  written
 * ----------------------------------------
*******************************************************************************/
void track_drv_lte_reset_pwron(void* op)
{
    kal_uint32 operate = (kal_uint32)op;
    kal_uint32 power_on_time = kal_ticks_to_secs(kal_get_systicks());

    LOGS("track_drv_lte_reset_pwron!!! Reset LTE %d, ticks %dS", operate, power_on_time);
#if defined( __NETWORK_LICENSE__)
    return;
#endif /* __NETWORK_LICENSE__ */
    if(track_is_testmode() && operate < 100)
    {
        return;
    }
    if(power_on_time < LTE_SIX_MINS)
    {
        return;
    }

    track_cust_lte_systemup(0);
    track_stop_timer(LTE_DECT_WDT_TIMER_ID);
    track_drv_lte_init_wdt(0);

    if(100 > operate)
    {
        track_drv_lte_reset();
        track_start_timer(LTE_DECT_WDT_TIMER_ID, 1900, track_drv_lte_reset_pwron, (void*)256);
    }
    else if(128 == operate)
    {
        track_drv_lte_reset();
    }
    else if(256 == operate)
    {
        track_drv_lte_pwron(0);
    }

}
/******************************************************************************
 *  Function    -  track_drv_lte_dect_wdt_eint
 *  Purpose     -  中断方式检测LTE模块喂狗信号
 *  Description -
 *  modification history
 * ----------------------------------------
 * v1.0  , 20150129,  wangqi  written
 * ----------------------------------------
*******************************************************************************/
void track_drv_lte_dect_wdt_eint(kal_uint8 status)
{
    static kal_uint8 last_status = 0xff;

    LOGD(L_DRV, L_V7, "%d,%d", status, last_status);

    if(track_drv_lte_systemup(0xff) && status == 0)
    {
        track_drv_lte_init_wdt(1);
    }
    last_status = status;

}
#endif

#if defined(__433M__)

void track_drv_433_alarm(void * sw)
{
    kal_uint32 pswitch = (kal_uint32)sw;

    if(0 == pswitch)
    {
        track_stop_timer(ALARM_TO433_TIMER_ID);
        track_drv_gpio_set(0, CTRL_433_GPIO);
    }
    else if(1 == pswitch)
    {
        track_drv_gpio_set(1, CTRL_433_GPIO);
    }
    else if(4 == pswitch)
    {
        track_drv_gpio_set(0, CTRL_433_GPIO);
        track_start_timer(ALARM_TO433_TIMER_ID, 1000, track_drv_433_alarm, (void *) 8);
    }
    else if(8 == pswitch)
    {
        track_drv_gpio_set(1, CTRL_433_GPIO);
        track_start_timer(ALARM_TO433_TIMER_ID, 1000, track_drv_433_alarm, (void *) 4);
    }
}

/******************************************************************************
 * FUNCTION:  - 433模块控制
 *
 * DESCRIPTION: -
 * 1:GPIO 33 为电源控制口
 * 2:GPIO 36 这里的控制是反向的
 *
 * Input:
 * Output:
 * Returns:
 *
 * modification history
 * --------------------
 * v1.0  , 14-09-2015,  ZengPing  written
 * --------------------
 ******************************************************************************/
static void track_drv_433_control_mode(kal_uint8 mode)
{
    LOGD(L_CMD, L_V5, "mode =%d", mode);
    switch(mode)
    {
        case 1:
        case 2:
        case 3:
            track_drv_gpio_set(1, AP_433_GPIO);
            if(mode == 1)
            {
                track_start_timer(TRACK_CUST_433_CONTROL_TIMER_ID, 1000, track_drv_433_control_mode, (void*)4);
            }
            else if(mode == 2)
            {
                track_start_timer(TRACK_CUST_433_CONTROL_TIMER_ID, 4000, track_drv_433_control_mode, (void*)4);
            }
            else if(mode == 3)
            {
                track_start_timer(TRACK_CUST_433_CONTROL_TIMER_ID, 7000, track_drv_433_control_mode, (void*)4);
            }
            break;
        case 4:
            if(track_is_timer_run(TRACK_CUST_433_CONTROL_INI_TIMER_ID) == KAL_FALSE)
            {
                track_drv_gpio_set(0, CTRL_433_GPIO);
            }
            else
            {
                LOGD(L_APP, L_V5, "check");
            }
            break;
        default:
            break;

    }
}
void track_drv_433_control(kal_uint32 mode)
{
    static kal_uint32 lock = 0;
    LOGD(L_DRV, L_V5,  "mode =%d", mode);
    if(lock == 0)
    {
        lock = 1;
        GPIO_ModeSetup(CTRL_433_GPIO, 0);
        GPIO_InitIO(1/*OUTPUT*/, CTRL_433_GPIO);
        GPIO_ModeSetup(AP_433_GPIO, 0);
        GPIO_InitIO(1/*OUTPUT*/, AP_433_GPIO);
    }
    switch(mode)
    {
        case 1://初始化GPIO口
            LOGD(L_DRV, L_V5, "@init");
            track_drv_gpio_set(1, CTRL_433_GPIO);//给433模块供电
            track_start_timer(TRACK_CUST_433_CONTROL_INI_TIMER_ID, 60 * 1000, track_drv_433_control_mode, (void*)4); //如若没有其它操作1分钟关闭433模块
            break;
        case 2://停止433 工作
            LOGD(L_DRV, L_V5, "@close");
            track_drv_gpio_set(1, CTRL_433_GPIO);
            track_drv_gpio_set(0, AP_433_GPIO);
            track_start_timer(TRACK_CUST_433_CONTROL_TIMER_ID, 10 * 1000, track_drv_433_control_mode, (void*)4);
            break;
        case 3://ACC OFF
            LOGD(L_DRV, L_V5, "@ACC OFF");
            track_stop_timer(TRACK_CUST_433_CONTROL_DELAY_TIMER_ID);
            if((track_cust_status_acc() == 1 && G_realtime_data.defense_mode == 1) || (G_realtime_data.oil_lock_status == 1 && track_cust_status_acc() == 1))
            {
                LOGD(L_DRV, L_V5, "@ACC OFF    手动模式ACCON情况下或者指令");
                track_drv_gpio_set(1, CTRL_433_GPIO);
                track_drv_gpio_set(1, AP_433_GPIO);
                track_start_timer(TRACK_CUST_433_CONTROL_TIMER_ID, 60 * 1000, track_drv_433_control_mode, (void*)4);
            }
            else
            {
                LOGD(L_DRV, L_V5, "@ACC OFF    正常ACCOFF");
                track_drv_gpio_set(0, CTRL_433_GPIO);
            }
            break;
        case 4://远程喇叭设防
            LOGD(L_DRV, L_V5, "@远程喇叭设防");
            track_stop_timer(TRACK_CUST_433_CONTROL_DELAY_TIMER_ID);
            track_drv_gpio_set(1, CTRL_433_GPIO);
            track_drv_gpio_set(0, AP_433_GPIO);
            track_start_timer(TRACK_CUST_433_CONTROL_TIMER_ID, 100, track_drv_433_control_mode, (void*)1);
            break;
        case 5://远程喇叭寻车
            LOGD(L_DRV, L_V5, "@远程喇叭寻车");
            track_stop_timer(TRACK_CUST_433_CONTROL_DELAY_TIMER_ID);
            track_drv_gpio_set(1, CTRL_433_GPIO);
            track_drv_gpio_set(0, AP_433_GPIO);
            track_start_timer(TRACK_CUST_433_CONTROL_TIMER_ID, 100, track_drv_433_control_mode, (void*)2);
            break;
        case 6://远程喇叭报警
            LOGD(L_DRV, L_V5, "@远程喇叭报警");
            track_stop_timer(TRACK_CUST_433_CONTROL_DELAY_TIMER_ID);
            track_drv_gpio_set(1, CTRL_433_GPIO);
            track_drv_gpio_set(0, AP_433_GPIO);
            track_start_timer(TRACK_CUST_433_CONTROL_TIMER_ID, 100, track_drv_433_control_mode, (void*)3);
            break;
        case 7://恢复断油电
            LOGD(L_DRV, L_V5, "@恢复断油电");
            track_stop_timer(TRACK_CUST_433_CONTROL_TIMER_ID);
            track_stop_timer(TRACK_CUST_433_CONTROL_DELAY_TIMER_ID);
            track_stop_timer(TRACK_CUST_433_CONTROL_INI_TIMER_ID);
            track_drv_gpio_set(1, CTRL_433_GPIO);
            if(G_realtime_data.oil_lock_status == 0)
            {
                track_drv_gpio_set(0, AP_433_GPIO);
            }
            else
            {
                LOGD(L_DRV, L_V5, "@锁定状态 433 不执行恢复断油电");
            }
            break;
        default:
            LOGD(L_DRV, L_V5, "@99");
            break;
    }
}
#endif
#if !defined(__NT37__)
kal_int8 track_drv_oli_cut_pwm(void* run_time)
{
    static kal_uint32 count = 1;

    count = (kal_uint32)run_time;
    LOGD(L_DRV, L_V4, "count=%d", count);
    if(count == 1)
    {
        GPIO_WriteIO(1, CUT_OIL_ELE_GPIO);
        if(GPIO_ReadIO(CUT_OIL_ELE_GPIO) == 1)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        count --;
        if(GPIO_ReadIO(CUT_OIL_ELE_GPIO) == 0)
        {
            LOGD(L_DRV, L_V5, "继电器 H - %d", count);
            GPIO_WriteIO(1, CUT_OIL_ELE_GPIO);
        }
        else
        {
            LOGD(L_DRV, L_V5, "继电器 L - %d", count);
            GPIO_WriteIO(0, CUT_OIL_ELE_GPIO);
        }
        track_start_timer(TRACK_DRV_OIL_CUT_PULSE_TIMER_ID, 1000, track_drv_oli_cut_pwm, (void *)count);
        return count;
    }
}
#endif
#if defined (__SOD_ALM__)
/*外部声音检测模块，有声音触发中断       --    chengjun  2017-09-18*/
#define SOD_LDO_GPIO_NO 15
void track_drv_voice_detection_control(kal_uint8 on)
{
    static kal_uint8 first = 1;

    if(0xFF == SOD_LDO_GPIO_NO)
    {
        return;
    }
    if(first == 1)
    {
        first = 0;
        GPIO_ModeSetup(SOD_LDO_GPIO_NO, 0);
        GPIO_InitIO(1, SOD_LDO_GPIO_NO);
    }

    if(on == 0)
    {
        GPIO_WriteIO(0, SOD_LDO_GPIO_NO);
    }
    else
    {
        GPIO_WriteIO(1, SOD_LDO_GPIO_NO);
    }    
}

#endif /*__SOD_ALM__*/

