/******************************************************************************
 * track_os_paramete.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        通用头文件
 *
 * modification history
 * --------------------
 * v1.0   2012-07-14,  jwj create this file
 *
 ******************************************************************************/



/****************************************************************************
* Include Files             包含头文件
*****************************************************************************/
#include "track_os_data_types.h"
#include "track_version.h"
#include "track_os_paramete.h"
#include "track_os_log.h"
#include "track_cust.h"

/*****************************************************************************
 *  Define			    宏定义
*****************************************************************************/
#define ON_OFF(value) (value==0 ? s_OFF : (value==1 ? s_ON : s_Error))

/*****************************************************************************
* Typedef  Enum         枚举
*****************************************************************************/


/*****************************************************************************
 *  Struct			    数据结构定义
*****************************************************************************/


/*****************************************************************************
* Local variable            局部变量
*****************************************************************************/


/****************************************************************************
* Global Variable           全局变量
*****************************************************************************/

#pragma arm section rodata = "TRACK_NVRAM_RODATA"

const nvram_importance_parameter_struct  NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT =
{
    {__DATE__},
    {__TIME__},
#if defined(__GT02__) && defined(__SYS_VERSION__)
    {"TQ"
    },
#else
    {0},
#endif /* __GT02__ */
    {_TRACK_VER_PARAMETER_},
    {KAL_FALSE, "CMNET", "", ""},
    {
#if defined(__GT530__) && defined(__NULL_SERVER__)
        /*默认为空*/
        {
            0
        },
        1, {0, 0, 0, 0}, 0, 0, 0, 0, 0
#elif defined(__BCA__)
        /*et02ddchz.dkwgps.com:9883*/
        {
            0xE5, 0xF4, 0xB0, 0xB2, 0xE4, 0xE4, 0xE3, 0xE8, 0xFA, 0xAE, 0xE4, 0xEB, 0xF7, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {202, 91, 246, 233}, 9883, 0, 0, 0, 0
#elif defined(__GT06F__)&&defined(__SERVER_COOACCESS__)
        /*www.cooaccess.net(网页www.cootrack.net)*/
        {
            0xF7, 0xF7, 0xF7, 0xAE, 0xE3, 0xEF, 0xEF, 0xE1, 0xE3, 0xE3, 0xE5, 0xF3, 0xF3, 0xAE, 0xEE, 0xE5, 0xF4
        },
        1, {113, 12, 82, 249}, 8841, 0, 0, 0, 0
#elif defined(__GT02__) && defined(__SERVER_TUQIANG__)
        /*gpsdev.tracksolid.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {202, 70, 2, 53}, 21101, 0, 0, 0, 0
#elif defined(__GT02E__)  ||defined(__GT800__)
        /*gt200.szdatasource.com*/
        {
            0xE7, 0xF4, 0xB2, 0xB0, 0xB0, 0xAE, 0xF3, 0xFA, 0xE4, 0xE1, 0xF4, 0xE1, 0xF3, 0xEF, 0xF5, 0xF2, 0xE3, 0xE5, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {183, 60, 142, 140}, 8841, 0, 0, 0, 0
#elif defined(__NT33__)
#if defined (__LANG_ENGLISH__)
        /*gpsdev.tracksolid.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xF4, 0xF2, 0xE1, 0xE3, 0xEB, 0xF3, 0xEF, 0xEC, 0xE9, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {49, 213, 12, 232}, 21100, 0, 0, 0
#elif defined (__CUSTOM_DDWL__)
        /*dc.didigps.com*/
        {
            0xE4, 0xE3, 0xAE, 0xE4, 0xE9, 0xE4, 0xE9, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {115, 28, 252, 32}, 5206, 0, 0, 0
#else
        /*gpsdev.jimicloud.com*/
        {0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80},
        1, {112, 74, 143, 157}, 21100, 0, 0, 0
#endif /* __LANG_ENGLISH__ */

#elif defined(__NT31__)&& defined(__LANG_ENGLISH__)
        /*gpsdev.tracksolid.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xF4, 0xF2, 0xE1, 0xE3, 0xEB, 0xF3, 0xEF, 0xEC, 0xE9, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {49, 213, 12, 232}, 21100, 0, 0, 0
#elif defined(__NT31__)
        /*gpsdev.jimicloud.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {112, 74, 143, 157}, 21100, 0, 0, 0
#elif defined(__NT22__)
        /*gpsdev.jimicloud.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {112, 74, 143, 157}, 21100, 0, 0, 0
#elif defined(__GT06D__) && defined(__SERVER_COOACCESS__)
        /*www.cooaccess.net(网页www.cootrack.net)*/
        {
            0xF7, 0xF7, 0xF7, 0xAE, 0xE3, 0xEF, 0xEF, 0xE1, 0xE3, 0xE3, 0xE5, 0xF3, 0xF3, 0xAE, 0xEE, 0xE5, 0xF4
        },
        1, {113, 12, 82, 249}, 8841, 0, 0, 0, 0
#elif defined(__GT06D__) && defined(__SERVER_GPSLUCKLY__)
        /*www.gpsluckly.com*/
        {
            0xF7, 0xF7, 0xF7, 0xAE, 0xE7, 0xF0, 0xF3, 0xEC, 0xF5, 0xE3, 0xEB, 0xEC, 0xF9, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {118, 194, 230, 3}, 8841, 0, 0, 0, 0

#elif defined(__GT06D__) && defined(__SERVER_TUQIANG__)
        /*atr06.tuqianggps.com:8841*/
        {
            0xE1, 0xF4, 0xF2, 0xB0, 0xB6, 0xAE, 0xF4, 0xF5, 0xF1, 0xE9, 0xE1, 0xEE, 0xE7, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {202, 70, 2, 53}, 8841, 0, 0, 0, 0

#elif defined(__GT06D__)
        /*hgt06.szdatasource.com*/
        {
            0xE8, 0xE7, 0xF4, 0xB0, 0xB6, 0xAE, 0xF3, 0xFA, 0xE4, 0xE1, 0xF4, 0xE1, 0xF3, 0xEF, 0xF5, 0xF2, 0xE3, 0xE5, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {113, 12, 82, 249}, 8841, 0, 0, 0

#elif defined (__SERVER_COOACCESS__)&&defined (__GT02__)
        /*www.cooaccess.net*/
        {
            0xF7, 0xF7, 0xF7, 0xAE, 0xE3, 0xEF, 0xEF, 0xE1, 0xE3, 0xE3, 0xE5, 0xF3, 0xF3, 0xAE, 0xEE, 0xE5, 0xF4
        },
        1, {183, 60, 142, 137}, 8821, 0, 0, 0, 0

#elif defined(__GT02E__) && defined(__SERVER_COOACCESS__)
        /*gt02d.cooaccess.net*/
        {
            0xF7, 0xF7, 0xF7 , 0xAE, 0xE3, 0xEF, 0xEF, 0xE1, 0xE3, 0xE3, 0xE5, 0xF3, 0xF3, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {202, 70, 2, 53}, 8841, 0, 0, 0, 0

#elif defined(__GT02D__) && defined(__SERVER_COOACCESS__)
        /*gt02d.cooaccess.net*/
        {
            0xE7, 0xF4, 0xB0, 0xB2, 0xE4, 0xAE, 0xE3, 0xEF, 0xEF, 0xE1, 0xE3, 0xE3, 0xE5, 0xF3, 0xF3, 0xAE, 0xEE, 0xE5, 0xF4
        },
        1, {202, 70, 2, 53}, 8841, 0, 0, 0, 0

#elif defined(__GT02D__) && defined(__SERVER_TUQIANG__)
        /*gt200.tuqianggps.com:8841*/
        {
            0xE7, 0xF4, 0xB2, 0xB0, 0xB0, 0xAE, 0xF4, 0xF5, 0xF1, 0xE9, 0xE1, 0xEE, 0xE7, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {202, 70, 2, 53}, 8841, 0, 0, 0, 0

#elif defined(__GT02D__)
        /*gt200.szdatasource.com*/
        {
            0xE7, 0xF4, 0xB2, 0xB0, 0xB0, 0xAE, 0xF3, 0xFA, 0xE4, 0xE1, 0xF4, 0xE1, 0xF3, 0xEF, 0xF5, 0xF2, 0xE3, 0xE5, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {183, 60, 142, 140}, 8841, 0, 0, 0, 0

#elif defined(__GT02E__) && defined(__SERVER_TUQIANG__)
        /*gt200.tuqianggps.com:8841*/
        {
            0xE7, 0xF4, 0xB2, 0xB0, 0xB0, 0xAE, 0xF4, 0xF5, 0xF1, 0xE9, 0xE1, 0xEE, 0xE7, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {202, 70, 2, 53}, 8841, 0, 0, 0

#elif (defined(__GT100__) || defined(__ET100__)) && defined(__SERVER_GPSLUCKLY__)
        /*www.gpsluckly.com*/
        {
            0xF7, 0xF7, 0xF7, 0xAE, 0xE7, 0xF0, 0xF3, 0xEC, 0xF5, 0xE3, 0xEB, 0xEC, 0xF9, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {118, 194, 230, 3}, 88, 0, 0, 0, 0

#elif (defined(__GT100__) || defined(__ET100__)) && defined(__SERVER_TUQIANG__)
        /*et100.tuqianggps.com:8841*/
        {
            0xE5, 0xF4, 0xB1, 0xB0, 0xB0, 0xAE, 0xF4, 0xF5, 0xF1, 0xE9, 0xE1, 0xEE, 0xE7, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {202, 70, 2, 53}, 8841, 0, 0, 0, 0

#elif defined(__ET200__) && defined(__XCWS__)
        /*ddc.cmmat.com:1087*/
        {
            0xE4 , 0xE4 , 0xE3 , 0xAE , 0xE3 , 0xED , 0xED , 0xE1 , 0xF4 , 0xAE , 0xE3 , 0xEF , 0xED
        },
        1, {218, 206, 24, 230}, 1087, 0, 0, 0, 0

#elif defined(__ET200__) && defined(__SERVER_TUQIANG__)
        /*et200.tuqianggps.com:8841*/
        {
            0xE5 , 0xF4 , 0xB2 , 0xB0 , 0xB0 , 0xAE , 0xF4 , 0xF5 , 0xF1 , 0xE9 , 0xE1 , 0xEE , 0xE7 , 0xE7 , 0xF0 , 0xF3 , 0xAE , 0xE3 , 0xEF , 0xED
        },
        1, {220, 250, 64, 24}, 8841, 0, 0, 0, 0

#elif defined(__ET200__) && defined(__SERVER_COOACCESS__)
        /*et200.cooaccess.net*/
        {
            0xE5 , 0xF4 , 0xB2 , 0xB0 , 0xB0 , 0xAE, 0xE3, 0xEF, 0xEF, 0xE1, 0xE3, 0xE3, 0xE5, 0xF3, 0xF3, 0xAE, 0xEE, 0xE5, 0xF4
        },
        1, {202, 70, 2, 53}, 8841, 0, 0, 0, 0

#elif defined(__ET200__) && defined(__SERVER_GTSX__)
        {
            0xF7, 0xF7, 0xF7, 0xAE, 0xF1, 0xF3, 0xFA, 0xF8, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {123, 103, 16, 60}, 7700, 0, 0, 0, 0

#elif defined(__ET100__)
        /*et100.szdatasource.com : 8841*/
        {
            0xE5, 0xF4, 0xB1, 0xB0, 0xB0, 0xAE, 0xF3, 0xFA, 0xE4, 0xE1, 0xF4, 0xE1, 0xF3, 0xEF, 0xF5, 0xF2, 0xE3, 0xE5, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {202, 70, 2, 53}, 8841, 0, 0, 0, 0

#elif defined(__BD300__)
        /*hgt06.szdatasource.com*/
        {
            0xE8, 0xE7, 0xF4, 0xB0, 0xB6, 0xAE, 0xF3, 0xFA, 0xE4, 0xE1, 0xF4, 0xE1, 0xF3, 0xEF, 0xF5, 0xF2, 0xE3, 0xE5, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {183, 60, 142, 140}, 8841, 0, 0, 0, 0

#elif defined(__NT23__) && defined(__OBD2__)
        /*v50.tujunlbs.net:8641*/
        {
            0XF6, 0XB5, 0XB0, 0XAE, 0XF4, 0XF5, 0XEA, 0XF5, 0XEE, 0XEC, 0XE2, 0XF3, 0XAE, 0XEE, 0XE5, 0XF4
        },
        1, {202, 104, 150, 136}, 8641, 0, 0, 0, 0

#elif defined(__NT23__) && defined(__LANG_ENGLISH__)
        /*gt230.obdconcox.net:4000*/
        {
            0xE7, 0xF4, 0xB2, 0xB3, 0xB0, 0xAE, 0xEF, 0xE2, 0xE4, 0xE3, 0xEF, 0xEE, 0xE3, 0xEF, 0xF8, 0xAE, 0xEE, 0xE5, 0xF4, 0x80
        },
        1, {183, 62, 138, 54}, 4000, 0, 0, 0, 0

#elif defined(__NT23__) && defined(__OBD__)
        /*202.104.150.136:6006*/
        {
            0xE7, 0xF4, 0xB2, 0xB3, 0xB0, 0xAE, 0xF4, 0xF5, 0xF1, 0xE9, 0xE1, 0xEE, 0xE7, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED
        },
        0, {202, 104, 150, 136}, 6006, 0, 0, 0, 0

#elif defined(__NT23__) && defined(__SERVER_TUQIANG__)
        /*gt230.tuqianggps.com:8841*/
        {
            0xE7, 0xF4, 0xB2, 0xB3, 0xB0, 0xAE, 0xF4, 0xF5, 0xF1, 0xE9, 0xE1, 0xEE, 0xE7, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {202, 70, 2, 53}, 8841, 0, 0, 0, 0

#elif defined(__NT23__)
        /*gt230.tuqianggps.com:8841*/
        {
            0xE7, 0xF4, 0xB2, 0xB3, 0xB0, 0xAE, 0xF4, 0xF5, 0xF1, 0xE9, 0xE1, 0xEE, 0xE7, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED
        },
        0, {202, 104, 150, 136}, 6006, 0, 0, 0, 0

#elif defined(__GT520__) && defined(__SERVER_TUQIANG__)
        /*gt500.tuqianggps.com:8841*/
        {
            0xE7, 0xF4, 0xB5, 0xB0, 0xB0, 0xAE, 0xF4, 0xF5, 0xF1, 0xE9, 0xE1, 0xEE, 0xE7, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {202, 70, 2, 53}, 8841, 0, 0, 0, 0

#elif defined(__GT520__)
        /*gt500.tuqianggps.com:8841*/
        {
            0xE7, 0xF4, 0xB5, 0xB0, 0xB0, 0xAE, 0xF4, 0xF5, 0xF1, 0xE9, 0xE1, 0xEE, 0xE7, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED
        },
        0, {183, 62, 138, 9}, 2332, 0, 0, 0, 0

#elif defined(__GT530__)
        /*gt500.tuqianggps.com:8841*///
        {
            0xE1, 0xE3, 0xE3, 0xE5, 0xF3, 0xF3, 0xAE, 0xB4, 0xF3, 0xE9, 0xE3, 0xE1, 0xF2, 0xAE, 0xEE, 0xE5, 0xF4, 0x80
        },
        1, {183, 62, 138, 54}, 3000, 0, 0, 0, 0

#elif defined(__GT500__) && defined(__LANG_ENGLISH__)
        /*www.cooaccess.net*/
        {
            0xF7, 0xF7, 0xF7, 0xAE, 0xE3, 0xEF, 0xEF, 0xE1, 0xE3, 0xE3, 0xE5, 0xF3, 0xF3, 0xAE, 0xEE, 0xE5, 0xF4
        },
        1, {183, 60, 142, 137}, 8841, 0, 0, 0, 0

#elif defined(__GT500__)
        /*gt500.tuqianggps.com:8841*/
        {
            0xE7, 0xF4, 0xB5, 0xB0, 0xB0, 0xAE, 0xF4, 0xF5, 0xF1, 0xE9, 0xE1, 0xEE, 0xE7, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {183, 60, 142, 140}, 8841, 0, 0, 0, 0

#elif defined(__GT03F__) && defined(__SERVER_GPSLUCKLY__)
        /*www.gpsluckly.com:8827*/
        {
            0xF7, 0xF7, 0xF7, 0xAE, 0xE7, 0xF0, 0xF3, 0xEC, 0xF5, 0xE3, 0xEB, 0xEC, 0xF9, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {118, 194, 230, 3}, 8827, 0, 0, 0, 0

#elif defined(__GT03F__) && defined(__SERVER_TUQIANG__)
        /*gt03a.tuqianggps.com*/
        /*{0xE7, 0xF4, 0xB0, 0xB3, 0xE1, 0xAE, 0xF4, 0xF5, 0xF1, 0xE9, 0xE1, 0xEE, 0xE7, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED},
        1, {183, 60, 142, 137}, 9900, 0, 0, 0, 0*/

        /*gt03d.tuqianggps.com*/
        {
            0xE7, 0xF4, 0xB0, 0xB3, 0xE4, 0xAE, 0xF4, 0xF5, 0xF1, 0xE9, 0xE1, 0xEE, 0xE7, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {183, 60, 142, 137}, 8827, 0, 0, 0, 0

#elif defined(__GT03F__)
        /*gt03a.goobaby.net:8827*/
        {
            0xE7, 0xF4, 0xB0, 0xB3, 0xE1, 0xAE, 0xE7, 0xEF, 0xEF, 0xE2, 0xE1, 0xE2, 0xF9, 0xAE, 0xEE, 0xE5, 0xF4
        },
        1, {183, 60, 142, 137}, 8827, 0, 0, 0, 0

#elif defined(__GT740__)
        /*gpsdev.jimicloud.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {112, 74, 143, 157}, 21102, 0, 0, 0
#elif defined(__GT420D__)
        /*gpsdev.jimicloud.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {112, 74, 143, 157}, 21102, 0, 0, 0
#elif defined(__GT420D__) && defined(__LANG_ENGLISH__)
        /*gpsdev.jimicloud.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xF4, 0xF2, 0xE1, 0xE3, 0xEB, 0xF3, 0xEF, 0xEC, 0xE9, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {49, 213, 12, 232}, 21102, 0, 0, 0
#elif defined(__GW100__) && defined(__LANG_ENGLISH__)
        /*JI01.jimicare.com,8217*/
        {
            0xCA, 0xC9, 0xB0, 0xB1, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xE1, 0xF2, 0xE5, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {122, 10, 252, 11}, 8217, 0, 0, 0, 0

#elif defined(__GW100__)
        /*gw100.tuqianggps.com:8827*/
        {
            0xE7, 0xF7, 0xB1, 0xB0, 0xB0, 0xAE, 0xF4, 0xF5, 0xF1, 0xE9, 0xE1, 0xEE, 0xE7, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {202, 70, 2, 53}, 8827, 0, 0, 0, 0

#elif defined(__GT300__) && defined(__NULL_SERVER__)
        /*默认为空*/
        {
            0
        },
        1, {0, 0, 0, 0}, 0, 0, 0, 0, 0
#elif defined(__GT300S__)
        /*gpsdev.jimicloud.com:21102*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {183, 60, 142, 137}, 21102, 0, 0, 0, 0
#elif defined(__GT300__) && defined(__LANG_ENGLISH__)
        /*gt300.goobaby.net*/
        {
            0xE7, 0xF4, 0xB3, 0xB0, 0xB0, 0xAE, 0xE7, 0xEF, 0xEF, 0xE2, 0xE1, 0xE2, 0xF9, 0xAE, 0xEE, 0xE5, 0xF4
        },
        1, {183, 60, 142, 137}, 8827, 0, 0, 0, 0

#elif defined(__GT300__) || defined(__GW100__)
        /*gt300.tuqianggps.com:8827*/
        {
            0xE7, 0xF4, 0xB3, 0xB0, 0xB0, 0xAE, 0xF4, 0xF5, 0xF1, 0xE9, 0xE1, 0xEE, 0xE7, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {183, 60, 142, 137}, 8827, 0, 0, 0, 0

#elif defined(__ET130__) && defined(__XCWS__)
        /*ddc.cmmat.com:1087*/
        {
            0xE4 , 0xE4 , 0xE3 , 0xAE , 0xE3 , 0xED , 0xED , 0xE1 , 0xF4 , 0xAE , 0xE3 , 0xEF , 0xED
        },
        1, {218, 206, 24, 230}, 1087, 0, 0, 0, 0

#elif defined(__ET130__) && defined(__JIMISHARE__)
        /*www.jimishare.com:8841*/
        /*{0xF7, 0xF7, 0xF7, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xF3, 0xE8, 0xE1, 0xF2, 0xE5, 0xAE, 0xE3, 0xEF, 0xED, 0x80},
        1, {122, 10, 252, 11}, 8841, 0, 0, 0, 0
        */
        /*JM01.jimishare.com:8211*/
        {
            0xCA, 0xCD, 0xB0, 0xB1, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xF3, 0xE8, 0xE1, 0xF2, 0xE5, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {118, 194, 230, 11}, 8211, 0, 0, 0, 0

#elif defined(__ET130__) && defined(__NEWJIMISHARE__)
        /*JM01.jimishare.com:8211*/
        {
            0xCA, 0xCD, 0xB0, 0xB1, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xF3, 0xE8, 0xE1, 0xF2, 0xE5, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {118, 194, 230, 11}, 8211, 0, 0, 0, 0

#elif defined(__ET130__) && defined(__LANG_ENGLISH__)
        /*et130.cooaccess.net:8841*/
        {
            0xE5, 0xF4, 0xB1, 0xB3, 0xB0, 0xAE, 0xE3, 0xEF, 0xEF, 0xE1, 0xE3, 0xE3, 0xE5, 0xF3, 0xF3, 0xAE, 0xEE, 0xE5, 0xF4, 0x80
        },
        1, {218, 206, 24, 230}, 8841, 0, 0, 0, 0

#elif defined(__ET130__)
        /*et130.tuqianggps.com:8841*/
        {
            0xE5 , 0xF4 , 0xB1 , 0xB3 , 0xB0 , 0xAE , 0xF4 , 0xF5 , 0xF1 , 0xE9 , 0xE1 , 0xEE , 0xE7 , 0xE7 , 0xF0 , 0xF3 , 0xAE , 0xE3 , 0xEF , 0xED
        },
        1, {220, 250, 64, 24}, 8841, 0, 0, 0, 0

#elif defined(__V20__)
        /*gpsdev.jimicloud.com:21100*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {220, 250, 64, 24}, 21100, 0, 0, 0, 0
#elif defined(__MT200__) && defined(__SERVER_IP_LINK__)
        /*gpsdev.jimicloud.com:21100*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED
        },
        0, {103, 253, 108, 24}, 5000, 0, 0, 0, 0
#elif defined(__MT200__) && defined(__SERVER_TRACKSOLID__)
        /*gpsdev.tracksolid.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xF4, 0xF2, 0xE1, 0xE3, 0xEB, 0xF3, 0xEF, 0xEC, 0xE9, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {49, 213, 12, 232}, 21100, 0, 0, 0, 0

#elif defined(__MT200__) && defined(__SERVER_JIMICLOUD__)
        /*gpsdev.jimicloud.com:21100*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {220, 250, 64, 24}, 21100, 0, 0, 0, 0

#elif defined(__MT200__)||defined(__BD220__)
        /*et200.tuqianggps.com:8841*/
        {
            0xE5 , 0xF4 , 0xB2 , 0xB0 , 0xB0 , 0xAE , 0xF4 , 0xF5 , 0xF1 , 0xE9 , 0xE1 , 0xEE , 0xE7 , 0xE7 , 0xF0 , 0xF3 , 0xAE , 0xE3 , 0xEF , 0xED
        },
        1, {220, 250, 64, 24}, 8841, 0, 0, 0, 0

#elif defined(__ET320__)
        /*ddc.cmmat.com:1087*/
        {
            0xE4 , 0xE4 , 0xE3 , 0xAE , 0xE3 , 0xED , 0xED , 0xE1 , 0xF4 , 0xAE , 0xE3 , 0xEF , 0xED
        },
        1, {218, 206, 24, 230}, 1087, 0, 0, 0, 0

#elif defined(__ET310__) && defined(__LANG_ENGLISH__)
        /*gpsdev.tracksolid.com:21100*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xF4, 0xF2, 0xE1, 0xE3, 0xEB, 0xF3, 0xEF, 0xEC, 0xE9, 0xE4, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {49, 213, 12, 232}, 21100, 0, 0, 0, 0

#elif defined(__ET310__)
        /*gpsdev.jimicloud.com:21100*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {112, 74, 143, 157}, 21100, 0, 0, 0, 0

#elif defined(__NT36__)&& defined(__MEX__)
        /*gpsdev.tracksolid.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xF4, 0xF2, 0xE1, 0xE3, 0xEB, 0xF3, 0xEF, 0xEC, 0xE9, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        0, {80, 241, 210, 18}, 8821, 0, 0, 0

#elif defined(__NT36__)&& defined(__LANG_ENGLISH__)
        /*gpsdev.tracksolid.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xF4, 0xF2, 0xE1, 0xE3, 0xEB, 0xF3, 0xEF, 0xEC, 0xE9, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {49, 213, 12, 232}, 21100, 0, 0, 0

#elif defined(__NT36__)
        /*gpsdev.jimicloud.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {112, 74, 143, 157}, 21100, 0, 0, 0
#elif defined(__NT37__)
#if defined (__LANG_ENGLISH__)
        /*gpsdev.tracksolid.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xF4, 0xF2, 0xE1, 0xE3, 0xEB, 0xF3, 0xEF, 0xEC, 0xE9, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {49, 213, 12, 232}, 21100, 0, 0, 0
#else
        /*gpsdev.jimicloud.com*/
        {0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80},
        1, {112, 74, 143, 157}, 21100, 0, 0, 0
#endif /* __LANG_ENGLISH__ */
#elif defined(__GT310__)
        /*gpsdev.jimicloud.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {112, 74, 143, 157}, 21102, 0, 0, 0

#elif defined(__GT370__)|| defined (__GT380__)
#if defined (__LANG_ENGLISH__)
        /*gpsdev.tracksolid.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xF4, 0xF2, 0xE1, 0xE3, 0xEB, 0xF3, 0xEF, 0xEC, 0xE9, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {49, 213, 12, 232}, 21100, 0, 0, 0
#else
        /*gpsdev.jimicloud.com*/
        {0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80},
        1, {112, 74, 143, 157}, 21102, 0, 0, 0
#endif
#elif defined(__ET350__)
        /*gpsdev.jimicloud.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {112, 74, 143, 157}, 21102, 0, 0, 0
#elif defined(__JM81__)
        /*gpsdev.jimicloud.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {112, 74, 143, 157}, 21102, 0, 0, 0
#elif defined (__JM66__)
        /*gpsdev.jimicloud.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {112, 74, 143, 157}, 21100, 0, 0, 0
#elif defined(__HVT001__)
        /*gpsdev.jimicloud.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        },
        1, {112, 74, 143, 157}, 21102, 0, 0, 0
#else
#error "please define project_null"
#endif
    }, {
#if defined(__ZKE_EPO__) && !defined(__LANG_ENGLISH__)
        {
            0xF7, 0xF7, 0xF7, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE1, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED
        },
        1, {120, 77, 2, 16}, 12283, 0, 0, 0
#else
#if defined(__LANG_ENGLISH__)&& !defined(__NT36__)&& !defined(__MT200__)&&!defined(__GT740__)&&!defined(__GT420D__)
        /*agps.topstargps.com:12283*/
        {
            0xE4 , 0xE5 , 0xF6 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE3 , 0xEC , 0xEF , 0xF5 , 0xE4 , 0xAE , 0xE3 , 0xEF , 0xED
        },
        1, {116, 204, 8, 57}, 12283, 0, 0, 0
#else
        /*agps.topstargps.com:12283*/
        {0xE4 , 0xE5 , 0xF6 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE3 , 0xEC , 0xEF , 0xF5 , 0xE4 , 0xAE , 0xE3 , 0xEF , 0xED},
        1, {116, 204, 8, 57}, 12283, 0, 0, 0
#endif /* __LANG_ENGLISH__ */
#endif
    },
    {
        {
            /*www.jimiagps.com:12283*/
            {0xF7, 0xF7, 0xF7, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE1, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED},
            1, {120, 77, 2, 16}, 12283, 0, 0, 0
        }
        , {
            /*www.jimiams.com:12283*/
            {0xF7, 0xF7, 0xF7, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE1, 0xED, 0xF3, 0xAE, 0xE3, 0xEF, 0xED},
            1, {120, 77, 2, 16}, 12283, 0, 0, 0
        }
        , {
            /*dev.jimicloud.com:12283*/
            {0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED},
            1, {120, 77, 2, 16}, 12283, 0, 0, 0
        }
        , {
            /*agps.topstargps.com:12283*/
            {0xE4 , 0xE5 , 0xF6 , 0xAE , 0xEA , 0xE9 , 0xED , 0xE9 , 0xE3 , 0xEC , 0xEF , 0xF5 , 0xE4 , 0xAE , 0xE3 , 0xEF , 0xED},
            0, {120, 77, 2, 16}, 12283, 0, 0, 0
        }
    }
#if defined(__GT02__) && (defined(__GUMI__) || defined(__GUMI_GENERAL__))
    , {
        //gt02.goomegps.net
        {0xE7 , 0xF4, 0xB0 , 0xB2, 0xAE , 0xE7 , 0xEF, 0xEF, 0xED , 0xE5 , 0xE7, 0xF0, 0xF3, 0xAE, 0xEE, 0xE5 , 0xF4},
        1,
        {202, 104, 149, 80},
        9888,
        0,
        1,
        0,
        0,
    }
#else
    , {
        {0xE7 , 0xF4, 0xB0 , 0xB2, 0xAE , 0xE7 , 0xEF, 0xEF, 0xED , 0xE5 , 0xE7, 0xF0, 0xF3, 0xAE, 0xEE, 0xE5 , 0xF4},
        1,
        {202, 104, 149, 80},
        9888,
        0,
        0,
        0,
        0
    }
#endif
#if defined(__BACKUP_SERVER__)
    , {0}
#endif/*__BACKUP_SERVER__*/
#if defined(__XCWS__)
#if defined(__XCWS_TPATH__)
    , {
        /*et130.tuqianggps.com:8841*/
        {0xE5, 0xF4, 0xB1, 0xB1, 0xB0, 0xAE, 0xF4, 0xF5, 0xF1, 0xE9, 0xE1, 0xEE, 0xE7, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED, 0x80},
        1, {113, 98, 255, 53}, 9555, 0, 0, 0, 0
    }
#endif
    , {1, "V1.0.1", "", "", "", "", "123456"}
#endif
#ifdef __GT02__///谷米为默认版本
#if defined( __GUMI__)
    , {1, 1, 0, 1} //version
#elif defined( __GUMI_GENERAL__)
    , {2, 1, 0, 0} //谷米通用版本，不支持SENSOR，不支持国外版功能
#elif defined(__SERVER_TRACKSOLID__) || defined( __SERVER_COOACCESS__)
    , {3, 1, 1, 0} //外贸版本，SENSOR，支持国外版功能
#elif defined( __SERVER_TUQIANG__)
    , {4, 1, 0, 0} //图强版本，支持SENSOR，不支持国外版功能
#endif
#elif defined(__GT740__)||defined(__GT420D__)
    , {1, 0, 0, 0} //version
#else
    , {0, 0, 0, 0} //version
#endif /* __GT02__ */
#if defined (__XYBB__)
    , {"", "000000", "70969", "BD220", "", 9, "", 1}
#endif /* __XYBB__ */

#if defined (__LUYING__)
    ,{  /*tcpupload.jimicloud.com:20010*/
        {0xF4, 0xE3, 0xF0, 0xF5, 0xF0, 0xEC, 0xEF, 0xE1, 0xE4, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80},
        1, {202, 70, 2, 53}, 20010, 0, 0, 0, 0
    }
#endif /* __LUYING__ */
#if defined(__MQTT__)
    , {"a1UieaAbEaI","","","jXFGY4w2RKtzmA5BtC7GWscJYSJnhsie",""}
    , {"jXFGY4w2RKtzmA5BtC7GWscJYSJnhsie",1}
#endif
};
const nvram_alone_parameter_struct NVRAM_ALONE_PARAMETER_DEFAULT =
{
    0x12345678,
    {KAL_FALSE, "CMNET", "", ""},
    {
#if defined (__LANG_ENGLISH__)
        /*gpsdev.tracksolid.com*/
        {
            0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xF4, 0xF2, 0xE1, 0xE3, 0xEB, 0xF3, 0xEF, 0xEC, 0xE9, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80
        }, 1, {49, 213, 12, 232}, 21102, 0, 0, 0
#elif defined (__CUSTOM_DDWL__)
        /*dc.didigps.com*/
        {
            0xE4, 0xE3, 0xAE, 0xE4, 0xE9, 0xE4, 0xE9, 0xE7, 0xF0, 0xF3, 0xAE, 0xE3, 0xEF, 0xED
        }, 1, {115, 28, 252, 32}, 5206, 0, 0, 0
#elif defined(__COPS_CMCC_)
        /*iocp.wwvas.com*/
        {
            0xE9, 0xEF, 0xE3, 0xF0, 0xAE, 0xF7, 0xF7, 0xF6, 0xE1, 0xF3, 0xAE, 0xE3, 0xEF, 0xED
        }, 1,{139, 219, 100, 197}, 9301, 0, 0, 0
#else
        /*gpsdev.jimicloud.com*/
        {0xE7, 0xF0, 0xF3, 0xE4, 0xE5, 0xF6, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE3, 0xEC, 0xEF, 0xF5, 0xE4, 0xAE, 0xE3, 0xEF, 0xED, 0x80}, 1, {112, 74, 143, 157}, 21102, 0, 0, 0
#endif /* __LANG_ENGLISH__ */
    },
    {
        /*www.jimiams.com:12283*/
        {0xF7, 0xF7, 0xF7, 0xAE, 0xEA, 0xE9, 0xED, 0xE9, 0xE1, 0xED, 0xF3, 0xAE, 0xE3, 0xEF, 0xED}, 1, {120, 77, 2, 16}, 12283, 0, 0, 0
    },
    1
};


const nvram_parameter_struct NVRAM_EF_PARAMETER_DEFAULT =
{
    {0},
    {NULL},
#ifdef __LANG_ENGLISH__
    {"http://maps.google.com/maps?q="},                 // URL设置
#else
    {"http://ditu.google.cn/maps?q="},                 // URL设置
#endif /* __LANG_ENGLISH__ */
#ifndef __GW100__
    10,                                                 // 监听延时，范围：5-18S。默认设置 10S
#else
    5,
#endif /* __GW100__ */
#if defined(__LANG_ENGLISH__)
    0,                                                  // 1 表示为中文，0 表示为英文，用于地址查询
#else
    1,                                                  // 1 表示为中文，0 表示为英文，用于地址查询
#endif /* __LANG_ENGLISH__ */
    1,                                                  // 1 开启GPRS   0 关闭GPRS
#if defined(__XCWS__)
    120,
    120,
#elif defined(__GW100__)
    300,                                                  // 心跳包设置间隔(分钟)  ACC ON 上传间隔
    300,
#elif defined(__GT03F__) || defined(__GT300__)|| defined(__GT300S__)||defined(__GT02__) || defined(__GT740__)|| defined(__GT370__)|| defined (__GT380__)||defined (__NT33__)|| defined(__GT420D__)
    180,                                                  // 心跳包设置间隔(设置指令为分钟，实际存储为秒)  ACC ON 上传间隔
    180,                                                  // 心跳包设置间隔(设置指令为分钟，实际存储为秒)  ACC OFF 上传间隔
#else
    180,                                                  // 心跳包设置间隔
    300,
#endif
#if defined(__GT300__) || defined(__GW100__)
    2,                                                  // 来电响铃方式
#else
    0,                                                  // 来电响铃方式
#endif //WangQi
#if defined(__BCA__)
    1,                                                  // 电话重拨次数
#else
    3,                                                  // 电话重拨次数
#endif
    10,                                                 // 监听延时；0、5～18秒；设置远程监听延时；默认值为：10秒
#if defined(__SPANISH__)
    1,                                                  // 来电呼叫自动返回地址（1 有效， 0 禁用）
#else
    0,                                                  // 来电呼叫自动返回地址（1 有效， 0 禁用）
#endif
#if defined(__XCWS__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    30,                                                 // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 20, 2}, {0, 300}, {1, 15, 15, 2, 30}}, // GPS工作模式
    {1, 1, 10, 2, 1, 300, 5, 2},                         // 震动报警
    {1, 2, 5, 300, 0},                                  // 断电报警设置
    {1, 60, 1, 20},                                    // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {1, 1, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS
#elif  defined(__GT06F__)
    5,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,                                                // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 1, 1}, {1, 20, 2}, {0, 300}, {1, 10, 10, 0, 30}}, // GPS工作模式
    {0, 2, 10, 5, 1, 30, 180, 2},                       // 震动报警
    {1, 2, 5, 300, 0},                                  // 断电报警设置
    {0, 100, 1, 20},                                    // 超速报警设置
    {"000000", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 10, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},
#if defined(__LANG_ENGLISH__)
    {
        0, 1, 0, 0
    },                                       // AGPS
#else
    {1, 1, 0, 0},                                       // AGPS
#endif /* __LANG_ENGLISH__ */
#elif defined(__GT02__)
#if defined(__NO_SENSOR__)
    0,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
#else
    5,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
#endif /* __NO_SENSOR__ */
    2,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,                                                // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 1, 1}, {1, 20, 2}, {0, 300}, {1, 10, 10, 2, 30}}, // GPS工作模式
    {0, 1, 10, 5, 1, 1800, 180, 2},                     // 震动报警
    {0, 1, 10, 300, 0, 0},                                // 断电报警设置
    {0, 100, 1, 20},                                    // 超速报警设置
    {"666666", "999999", 1, 0, 0, 1, 0},                // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 10, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {1, 1, 1, 0},                                          // AGPS
#elif  defined(__NT31__)
    5,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    2,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,                                                // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 1, 1}, {1, 20, 2}, {0, 300}, {1, 10, 10, 0, 30}}, // GPS工作模式
    {0, 1, 10, 5, 1, 1800, 180, 2},                     // 震动报警
    {1, 1, 20, 300, 0, 0},                                 // 断电报警设置
    {0, 100, 1, 20},                                    // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 30, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
#if defined(__LANG_ENGLISH__)
    {
        0, 1, 0, 0
    },                                       // AGPS
#else
    {1, 1, 0, 0},                                       // AGPS
#endif /* __LANG_ENGLISH__ */
#elif  defined(__NT22__)
    5,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    2,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,                                                // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 1, 1}, {1, 15, 2}, {0, 300}, {1, 30, 180, 0, 30}}, // GPS工作模式
    {1, 1, 10, 5, 1, 1800, 180, 2},                     // 震动报警
    {1, 1, 20, 300, 0},                                  // 断电报警设置
    {0, 100, 1, 20},                                    // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 10, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
#if defined(__LANG_ENGLISH__)
    {
        0, 1, 0, 0
    },                                       // AGPS
#else
    {1, 1, 0, 0},                                       // AGPS
#endif /* __LANG_ENGLISH__ */
#elif defined(__GT02D__)
    5,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,                                                // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 1, 1}, {1, 20, 2}, {0, 300}, {1, 10, 10, 0, 30}}, // GPS工作模式
    {0, 1, 10, 5, 1, 1800, 180, 2},                     // 震动报警
    {1, 1, 10, 300, 0},                                 // 断电报警设置
    {0, 100, 1, 20},                                    // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 0},                 // 权限设置
    {1, 1},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 10, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS
#elif defined(__GT100__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    60,                                                 // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 3, {10, 3, 1}, {1, 30, 3}, {0, 300}, {1, 20, 20, 0, 30}}, // GPS工作模式
    {1, 1, 10, 5, 1, 60, 30, 2},                        // 震动报警
    {1, 2, 10, 1, 0},                                   // 断电报警设置
    {0, 80, 1, 20},                                     // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 0},                 // 权限设置
    {1, 1},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {1, 1, 0, 0},                                       // AGPS
#elif defined(__ET100__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    60,                                                 // 延时进入设防时间（秒）
    0,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 3, {10, 3, 1}, {1, 30, 3}, {0, 300}, {1, 20, 20, 0, 30}}, // GPS工作模式
    {1, 1, 10, 5, 1, 60, 30, 2},                        // 震动报警
    {1, 2, 10, 1, 0},                                   // 断电报警设置
    {0, 50, 1, 20},                                     // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 0},                 // 权限设置
    {1, 1},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {1, 1, 0, 0},                                       // AGPS
#elif defined(__BD300__)
    5,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,                                                // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 1, 1}, {1, 20, 2}, {0, 300}, {1, 10, 10, 2, 30}}, // GPS工作模式
    {1, 1, 10, 5, 1, 1800, 180, 2},                     // 震动报警
    {1, 1, 10, 300, 0},                                 // 断电报警设置
    {0, 100, 1, 20},                                    // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {0, 10, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {0, 1, 0, 0},                                       // AGPS
#elif  defined(__GT530__)
    5,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,                                                // 延时进入设防时间（秒）
    0,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 20, 2}, {0, 300}, {1, 10, 10, 2, 30}}, // GPS工作模式
    {1, 0, 10, 5, 1, 1800, 180, 2},                     // 震动报警
    {1, 0, 5, 1, 0},                                    // 断电报警设置
    {0, 100, 0, 20},                                    // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 10, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 0, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS
#elif defined(__GT500__) && defined(__GT500S__)
    5,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,                                                // 延时进入设防时间（秒）
    0,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 20, 2}, {0, 300}, {1, 10, 10, 2, 30}}, // GPS工作模式
    {0, 0, 10, 5, 1, 1800, 180, 2},                     // 震动报警
    {1, 1, 5, 1, 0},                                    // 断电报警设置
    {0, 100, 0, 20},                                    // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 0},                                     // GPS盲区设置
    {1, 10, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 0, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS
#elif defined(__GT500__) && defined(__LANG_ENGLISH__) && !defined(__GT520__)
    5,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,                                                // 延时进入设防时间（秒）
    0,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 20, 2}, {0, 300}, {1, 10, 10, 2, 30}}, // GPS工作模式
    {1, 1, 10, 5, 1, 1800, 180, 2},                     // 震动报警
    {1, 1, 5, 1, 0},                                    // 断电报警设置
    {0, 100, 0, 20},                                    // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 0},                                     // GPS盲区设置
    {1, 10, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 0, 300},                                        // 位移报警
    {0, 1, 1, 0},                                       // AGPS
#elif defined(__GT500__)
    5,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,                                                // 延时进入设防时间（秒）
    0,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 20, 2}, {0, 300}, {1, 10, 10, 2, 30}}, // GPS工作模式
    {1, 0, 10, 5, 1, 1800, 180, 2},                     // 震动报警
    {1, 0, 5, 1, 0},                                    // 断电报警设置
    {0, 100, 0, 20},                                    // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 0},                                     // GPS盲区设置
    {1, 10, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 0, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS
#elif defined(__ET200__) && defined(__SERVER_GTSX__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    60,                                                 // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 30, 3}, {0, 300}, {1, 20, 20, 2, 30}}, // GPS工作模式
    {1, 3, 10, 5, 1, 60, 30, 2},                        // 震动报警
    {1, 3, 10, 1, 0},                                   // 断电报警设置
    {0, 50, 0, 20},                                     // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 0},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 3, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS
#elif defined(__ET200__) && defined(__LANG_ENGLISH__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    60,                                                 // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 30, 3}, {0, 300}, {1, 20, 20, 2, 30}}, // GPS工作模式
    {1, 1, 10, 5, 1, 60, 30, 2},                        // 震动报警
    {1, 2, 10, 1, 0},                                   // 断电报警设置
    {0, 50, 1, 20},                                     // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 0},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS
#elif defined(__MT200__) && defined(__LANG_ENGLISH__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    60,                                                 // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
#if defined(__SERVER_IP_LINK__)
    {
        1, 100, 60, 1, 5, {10, 3, 1}, {1, 30, 3}, {0, 300}, {1, 60, 600, 2, 30}
    }, // GPS工作模式
#else
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 30, 3}, {0, 300}, {1, 20, 20, 2, 30}}, // GPS工作模式
#endif
    {1, 1, 10, 5, 1, 60, 30, 2},                        // 震动报警
    {1, 2, 10, 1, 0},                                   // 断电报警设置
    {0, 50, 1, 20},                                     // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 0},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {0, 1, 1, 0},                                       // AGPS
#elif defined(__MT200__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    60,                                                 // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 30, 3}, {0, 300}, {1, 20, 20, 2, 30}}, // GPS工作模式
    {1, 0, 10, 5, 1, 60, 30, 2},                        // 震动报警
    {1, 1, 10, 1, 0},                                   // 断电报警设置
    {0, 50, 0, 20},                                     // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 0},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 0, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS
#elif defined(__GT03F__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    30,                                                 // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 20, 2}, {0, 300}, {1, 20, 20, 2, 30}}, // GPS工作模式
    {1, 1, 10, 5, 1, 300, 10, 2},                       // 震动报警
    {0, 2, 5, 300, 0},                                  // 断电报警设置
    {0, 100, 1, 20},                                    // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS
#elif defined(__GT740__) || defined(__GT420D__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    30,                                                 // 延时进入设防时间（秒）
    0,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 10, 2}, {0, 300}, {1, 20, 20, 20, 30}}, // GPS工作模式
    {0, 1, 10, 5, 1, 300, 10, 2},                       // 震动报警
    {0, 2, 5, 300, 0},                                  // 断电报警设置
    {0, 100, 1, 20},                                    // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS
#elif defined(__GW100__) && defined (__LANG_ENGLISH__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    30,                                                 // 延时进入设防时间（秒）
    0,                                                  // 断油电功能（1 有效， 0 禁用）
    {0, 100, 60, 1, 5, {10, 3, 1}, {1, 20, 2}, {0, 300}, {1, 30, 30, 10, 0}}, // GPS工作模式
    {1, 1, 10, 5, 1, 300, 10, 2},                       // 震动报警
    {0, 1, 5, 300, 0},                                  // 断电报警设置
    {0, 100, 1, 20},                                    // 超速报警设置
    {"666666", "SUPER", 0, 0, 1, 0, 0},                 // 权限设置
    {1, 1},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS
#elif defined(__GW100__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    30,                                                 // 延时进入设防时间（秒）
    0,                                                  // 断油电功能（1 有效， 0 禁用）
    {0, 100, 60, 1, 5, {10, 3, 1}, {1, 20, 2}, {0, 300}, {1, 30, 30, 10, 0}}, // GPS工作模式
    {1, 1, 10, 5, 1, 300, 10, 2},                       // 震动报警
    {0, 2, 5, 300, 0},                                  // 断电报警设置
    {0, 100, 1, 20},                                    // 超速报警设置
    {"666666", "SUPER", 0, 0, 1, 0, 0},                 // 权限设置
    {1, 3},                                             // SOS报警设置
    {0, 60, 20, 0},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS
#elif defined(__GT300__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    30,                                                 // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {0, 100, 60, 1, 5, {10, 3, 1}, {1, 20, 2}, {0, 300}, {1, 20, 20, 2, 30}}, // GPS工作模式
    {1, 1, 10, 5, 1, 300, 10, 2},                       // 震动报警
    {0, 2, 5, 300, 0},                                  // 断电报警设置
    {0, 100, 1, 20},                                    // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS
#elif defined(__GT300S__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    30,                                                 // 延时进入设防时间（秒）
    0,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 20, 2}, {0, 300}, {1, 10, 10, 5, 30}}, // GPS工作模式
    {1, 1, 10, 5, 1, 300, 10, 2},                       // 震动报警
    {0, 2, 5, 300, 0},                                  // 断电报警设置
    {0, 100, 1, 20},                                    // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 3},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS
#elif defined (__GT370__)
    3,							    // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,							    // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    30,							    // 延时进入设防时间（秒）
    0,							    // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 20, 2}, {0, 300}, {1, 10, 3600, 5, 600}}, // GPS工作模式
    {1, 1, 10, 5, 1, 300, 10, 2}, 					   // 震动报警
    {0, 2, 5, 300, 0},							    // 断电报警设置
    {0, 100, 1, 20},							    // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2}, 				    // 权限设置
    {1, 2}, 								    // SOS报警设置
    {0, 60, 20, 1}, 							    // GPS盲区设置
    {1, 180, 6, 3},							    // 静止上报最后位置开关指令
    {0, 1, 300},								    // 位移报警
#if defined(__LANG_ENGLISH__)
    {
        0, 1, 1, 0
    },                                                   // AGPS
#else
    {1, 1, 1, 0},								    // AGPS
#endif

#elif defined (__GT380__)
    3,							    // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,							    // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    30,							    // 延时进入设防时间（秒）
    0,							    // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 20, 2}, {0, 300}, {1, 10, 3600, 5, 600}}, // GPS工作模式
    {1, 1, 10, 5, 1, 300, 10, 2}, 					   // 震动报警
    {0, 2, 5, 60, 0},							    // 断电报警设置
    {0, 100, 1, 20},							    // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2}, 				    // 权限设置
    {1, 2}, 								    // SOS报警设置
    {0, 60, 20, 1}, 							    // GPS盲区设置
    {1, 180, 6, 3},							    // 静止上报最后位置开关指令
    {0, 1, 300},								    // 位移报警
#if defined(__LANG_ENGLISH__)
    {
        0, 1, 1, 0
    },                                                   // AGPS
#else
    {1, 1, 1, 0},								    // AGPS
#endif

#elif defined(__GT02E__)
#if defined(__NO_SENSOR__)
    0,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
#else
    5,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
#endif /* __NO_SENSOR__ */
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,                                                // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 1, 1}, {1, 20, 2}, {0, 300}, {1, 10, 10, 0, 30}}, // GPS工作模式
    {0, 1, 10, 5, 1, 1800, 180, 2},                     // 震动报警
    {1, 1, 10, 300, 0},                                 // 断电报警设置
    {0, 100, 1, 20},                                    // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 0},			    	// 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 10, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
#if defined(__LANG_ENGLISH__)
    {
        0, 1, 0, 0
    },                                       // AGPS
#else
    {1, 1, 0, 0},                                       // AGPS
#endif /* __LANG_ENGLISH__ */
#elif defined(__NT23__)
    5,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,                                                // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 1, 1}, {1, 20, 2}, {0, 300}, {1, 10, 10, 0, 30}}, // GPS工作模式
    {0, 1, 10, 5, 1, 1800, 180, 2},                     // 震动报警
    {1, 1, 10, 300, 0},                                 // 断电报警设置
    {0, 100, 1, 20},                                    // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 0},			    	// 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 10, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {1, 1, 1, 0},                                          // AGPS
#elif defined(__ET130__) && defined(__JIMISHARE__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    60,                                                 // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 30, 3}, {0, 300}, {1, 20, 20, 0, 30}}, // GPS工作模式
    {1, 1, 10, 5, 1, 60, 30, 2},                        // 震动报警
    {1, 2, 10, 1, 0},                                   // 断电报警设置
    {0, 50, 1, 20},                                     // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 0},				// 权限设置
    {1, 1},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {0, 1, 1, 0},                                          // AGPS
#elif defined(__ET130__) && defined(__LANG_ENGLISH__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    60,                                                 // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 30, 3}, {0, 300}, {1, 20, 20, 0, 30}}, // GPS工作模式
    {1, 1, 10, 5, 1, 60, 30, 2},                        // 震动报警
    {1, 2, 10, 1, 0},                                   // 断电报警设置
    {0, 50, 1, 20},                                     // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 0},	            // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {0, 1, 1, 0},                                       // AGPS
#elif defined(__ET130__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    60,                                                 // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 30, 3}, {0, 300}, {1, 20, 20, 0, 30}}, // GPS工作模式
    {1, 0, 10, 5, 1, 60, 30, 2},                        // 震动报警
    {1, 1, 10, 1, 0},                                   // 断电报警设置
    {0, 50, 0, 20},                                     // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 0},				// 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 1},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 0, 300},                                        // 位移报警
    {1, 1, 1, 0},                                          // AGPS
#elif defined(__V20__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    60,                                                 // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 30, 3}, {0, 300}, {1, 20, 20, 2, 30}}, // GPS工作模式
    {1, 0, 10, 5, 1, 60, 30, 2},                        // 震动报警
    {1, 1, 10, 1, 0},                                   // 断电报警设置
    {0, 50, 0, 20},                                     // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 0},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 0, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS



#elif defined (__BCA__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    2,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    60,                                                 // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 30, 3}, {0, 300}, {1, 15, 15, 2, 30}}, // GPS工作模式
    {1, 2, 10, 3, 1, 60, 30, 2},                        // 震动报警
    {1, 2, 10, 1, 0},                                   // 断电报警设置
    {0, 50, 1, 20},                                     // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 0},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS
#elif defined (__ET310__) && defined(__LANG_ENGLISH__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    2,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    60,                                                 // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 30, 3}, {0, 300}, {1, 10, 10, 2, 30}}, // GPS工作模式
    {1, 1, 10, 5, 1, 60, 30, 2},                        // 震动报警
    {1, 2, 10, 1, 0},                                   // 断电报警设置
    {0, 50, 1, 20},                                     // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 0},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {0, 1, 1, 0},                                       // AGPS
#elif defined (__ET310__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    60,                                                 // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 30, 3}, {0, 300}, {1, 20, 20, 2, 30}}, // GPS工作模式
    {1, 0, 10, 5, 1, 60, 30, 2},                        // 震动报警
    {1, 1, 10, 1, 0},                                   // 断电报警设置
    {0, 50, 0, 20},                                     // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 0},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 0, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS
#elif defined (__ET320__)
    3,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    60,                                                 // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 30, 3}, {0, 300}, {1, 20, 20, 2, 30}}, // GPS工作模式
    {1, 0, 10, 5, 1, 60, 30, 2},                        // 震动报警
    {1, 1, 10, 1, 0},                                   // 断电报警设置
    {0, 50, 0, 20},                                     // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 60, 20, 0},                                     // GPS盲区设置
    {1, 20, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 0, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS
#elif defined(__NT36__) && defined(__SPANISH__)
    5,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,                                                // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 1, 1}, {0, 30, 3}, {0, 300}, {1, 20, 30, 2, 30}}, // GPS工作模式
    {0, 2, 10, 5, 1, 1800, 180, 2},                       // 震动报警
    {1, 0, 5, 300, 0},                                  // 断电报警设置
    {0, 100, 1, 20},                                    // 超速报警设置
    {"000000", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 0, 1, "SOS Alert!"},                                             // SOS报警设置
    {0, 10, 10, 1},                                     // GPS盲区设置
    {1, 10, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},
    {0, 1, 1, 0},                                       // AGPS
#elif defined(__NT36__) && defined(__LANG_ENGLISH__)
    5,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,                                                // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 1, 1}, {1, 30, 3}, {0, 300}, {1, 10, 10, 2, 30}}, // GPS工作模式
    {0, 2, 10, 5, 1, 1800, 180, 2},                       // 震动报警
    {1, 2, 5, 300, 0},                                  // 断电报警设置
    {0, 100, 1, 20},                                    // 超速报警设置
    {"000000", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 10, 10, 1},                                     // GPS盲区设置
    {1, 10, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},
    {0, 1, 1, 0},                                       // AGPS
#elif defined (__NT36__)
    5,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,                                                 // 延时进入设防时间（秒）
    1,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 1, 1}, {1, 30, 3}, {0, 300}, {1, 10, 10, 2, 30}}, // GPS工作模式
    {0, 2, 10, 5, 1, 1800, 180, 2},                        // 震动报警
    {1, 2, 5, 300, 0},                                   // 断电报警设置
    {0, 100, 1, 20},                                     // 超速报警设置
    {"000000", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 10, 10, 1},                                     // GPS盲区设置
    {1, 10, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
    {1, 1, 1, 0},                                       // AGPS
#elif defined (__NT33__)
    5,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    180,                                                 // 延时进入设防时间（秒）
    0,                                                  // 断油电功能（1 有效， 0 禁用）

#if defined (__CUSTOM_DDWL__)
    {
        1, 100, 60, 1, 5, {10, 3, 1}, {1, 20, 2}, {0, 300}, {1, 30, 30, 30, 30}
    }, // GPS工作模式
#else
    {1, 100, 60, 1, 5, {10, 3, 1}, {1, 20, 2}, {0, 300}, {1, 10, 10, 180, 30}}, // GPS工作模式
#endif /* __CUSTOM_DDWL__ */

    {0, 0, 10, 5, 1, 1800, 1, 2},                        // 震动报警
    {1, 1, 20, 300, 0},                                   // 断电报警设置
    {0, 100, 1, 20},                                     // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 10, 10, 1},                                     // GPS盲区设置
    {1, 10, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
#if defined(__LANG_ENGLISH__)
    {
        0, 1, 1, 0
    },                                       // AGPS
#else
    {1, 1, 1, 0},                                       // AGPS
#endif /* __LANG_ENGLISH__ */

#elif defined (__ET350__)
    5,                                                  // 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,                                                  // 跟随GPS关闭 LED休眠，1为启用，0为禁止
    180,                                                 // 延时进入设防时间（秒）
    0,                                                  // 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 1, 1}, {1, 20, 2}, {0, 300}, {1, 10, 10, 180, 30}}, // GPS工作模式
    {0, 0, 10, 5, 1, 1800, 1, 2},                        // 震动报警

#if defined (__LANG_ENGLISH__)
    {
        1, 2, 20, 300, 0
    },                                   // 断电报警设置
#else
    {1, 1, 20, 300, 0},                                   // 断电报警设置
#endif /* __LANG_ENGLISH__ */

    {0, 100, 1, 20},                                     // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2},                 // 权限设置
    {1, 2},                                             // SOS报警设置
    {0, 10, 10, 1},                                     // GPS盲区设置
    {1, 10, 6, 3},                                      // 静止上报最后位置开关指令
    {0, 1, 300},                                        // 位移报警
#if defined(__LANG_ENGLISH__)
    {
        0, 1, 1, 0
    },										// AGPS
#else
    {1, 1, 1, 0},										// AGPS
#endif /* __LANG_ENGLISH__ */

#elif defined (__GT310__)
    2,													// 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,													// 跟随GPS关闭 LED休眠，1为启用，0为禁止
    180,												 // 延时进入设防时间（秒）
    0,													// 断油电功能（1 有效， 0 禁用）
    {0, 100, 60, 1, 5, {10, 1, 1}, {1, 20, 2}, {0, 300}, {1, 30, 30, 180, 30}}, // GPS工作模式
    {0, 0, 10, 5, 1, 1800, 1, 2},						 // 震动报警
    {1, 1, 20, 300, 0}, 								  // 断电报警设置
    {0, 100, 1, 20},									 // 超速报警设置
    {"666666", "SUPER", 0, 0, 0, 1, 2, 2}, 				// 权限设置
    {1, 2}, 											// SOS报警设置
    {0, 10, 10, 1}, 									// GPS盲区设置
    {1, 10, 6, 3},										// 静止上报最后位置开关指令
    {0, 1, 300},										// 位移报警
    {1, 1, 1, 0},										// AGPS
#elif defined (__NT37__)
    5,													// 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,													// 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,												 // 延时进入设防时间（秒）
    1,													// 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 1, 1}, {1, 30, 3}, {0, 300}, {1, 10, 10, 2, 30}}, // GPS工作模式
    {0, 2, 10, 5, 1, 1800, 180, 2}, 					   // 震动报警
    {1, 2, 5, 300, 0},									 // 断电报警设置
    {0, 100, 1, 20},									 // 超速报警设置
    {"000000", "SUPER", 0, 0, 0, 1, 2}, 				// 权限设置
    {1, 2}, 											// SOS报警设置
    {0, 10, 10, 1}, 									// GPS盲区设置
    {1, 10, 6, 3},										// 静止上报最后位置开关指令
    {0, 1, 300},										// 位移报警
#if defined(__LANG_ENGLISH__)
    {
        0, 1, 1, 0
    },                                       // AGPS
#else
    {1, 1, 1, 0},                                       // AGPS
#endif /* __LANG_ENGLISH__ */

#elif defined (__JM81__)
    5,													// 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,													// 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,												 // 延时进入设防时间（秒）
    0,													// 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 1, 1}, {1, 30, 3}, {0, 300}, {1, 10, 10, 2, 30}}, // GPS工作模式
    {0, 1, 2, 10, 5, 1, 1800, 180, 2}, 					   // 震动报警
    {1, 2, 5, 300, 0},									 // 断电报警设置
    {0, 100, 1, 20},									 // 超速报警设置
    {"000000", "SUPER", 0, 0, 0, 1, 2}, 				// 权限设置
    {1, 2}, 											// SOS报警设置
    {0, 10, 10, 1}, 									// GPS盲区设置
    {1, 10, 6, 3},										// 静止上报最后位置开关指令
    {0, 1, 300},										// 位移报警
#if defined(__LANG_ENGLISH__)
    {
        0, 1, 1, 0
    },                                       // AGPS
#else
    {1, 1, 1, 0},                                       // AGPS
#endif /* __LANG_ENGLISH__ */
#elif defined (__JM66__)
    2,													// 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,													// 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,												 // 延时进入设防时间（秒）
    0,													// 断油电功能（1 有效， 0 禁用）
    {0, 100, 60, 1, 5, {10, 1, 1}, {1, 30, 3}, {0, 300}, {1, 10, 10, 2, 30}}, // GPS工作模式
    {0, 2, 10, 3, 1, 1800, 180, 2}, 					   // 震动报警
    {1, 2, 5, 300, 0},									 // 断电报警设置
    {0, 100, 1, 20},									 // 超速报警设置
    {"000000", "SUPER", 0, 0, 0, 1, 2}, 				// 权限设置
    {1, 2}, 											// SOS报警设置
    {0, 10, 10, 1}, 									// GPS盲区设置
    {1, 10, 6, 3},										// 静止上报最后位置开关指令
    {0, 1, 300},										// 位移报警
#if defined(__LANG_ENGLISH__)
    {
        0, 1, 1, 0
    },                                       // AGPS
#else
    {1, 1, 1, 0},                                       // AGPS
#endif /* __LANG_ENGLISH__ */
#elif defined (__HVT001__)
    5,													// 0～300分钟，检测到一次震动，开启 GPS 工作的时间
    1,													// 跟随GPS关闭 LED休眠，1为启用，0为禁止
    600,												 // 延时进入设防时间（秒）
    1,													// 断油电功能（1 有效， 0 禁用）
    {1, 100, 60, 1, 5, {10, 1, 1}, {1, 30, 3}, {0, 300}, {1, 10, 10, 2, 30}}, // GPS工作模式
    {0, 2, 10, 5, 1, 1800, 180, 2}, 					   // 震动报警
    {1, 2, 5, 300, 0},									 // 断电报警设置
    {0, 100, 1, 20},									 // 超速报警设置
    {"000000", "SUPER", 0, 0, 0, 1, 2}, 				// 权限设置
    {1, 2}, 											// SOS报警设置
    {0, 10, 10, 1}, 									// GPS盲区设置
    {1, 10, 6, 3},										// 静止上报最后位置开关指令
    {0, 1, 300},										// 位移报警
#if defined(__LANG_ENGLISH__)
    {
        0, 1, 1, 0
    },                                       // AGPS
#else
    {1, 1, 1, 0},                                       // AGPS
#endif /* __LANG_ENGLISH__ */
#else
#error "please define project_null"
#endif /* __GT02D__ */

#if defined( __GW100__)||defined(__GT370__)
    {
        1, 0
    },                                             // 开机报警开关
#elif defined (__GT380__)
    {
        0, 0
    },                                             // 开机报警开关
#else
    {0, 1},                                             // 开机报警开关
#endif /* __GW100__ */

#ifdef __GT300S__
    {0, 30, 30},                                        // 组包个数
#elif defined(__GT370__)|| defined (__GT380__)
    {
        0, 30, 30
    },										// 组包个数
#else
    {0, 10, 10},                                        // 组包个数
#endif /* __GT300S__ */

#if !defined(__INDIA__)
    {
        0, 0, 1000
    },                                       // 里程统计
#else
    {1, 0, 1000},
#endif

    {0, 1, 60},                                         // 停车超时报警
#if defined (__NT33__)
    {'E', 8, 0, 1
    },
#elif defined(__GT06D__) && !defined(__NT31__) || defined(__GT03F__) || defined(__BD300__) || defined(__GT02__)
    {'E', 8, 0, 0
    },                                     // 时区设置
#elif (defined(__ET200__) || defined(__ET310__)||defined(__MEX__)) && defined(__LANG_ENGLISH__)
    {'E', 8, 0, 0
    },                                     // 时区设置
#else
    {'E', 8, 0, 1},                                     // 时区设置
#endif

#if defined(__XCWS__)||defined(__GT420D__)
    {
        1, 0, 3600000
    },                                             // 低电报警设置
#elif defined(__GT02__)
    {
        0, 1
    },                                             // 低电报警设置
#elif (defined(__ET200__) || defined(__ET130__) || defined(__GT500__) || defined(__GW100__) || defined(__ET310__)||defined(__MT200__)||defined(__ET350__)) && defined(__LANG_ENGLISH__)
    {
        1, 1
    },
#elif defined(__ET200__) || defined(__GT500__) || defined(__ET130__) || defined(__GW100__) || defined(__V20__) || defined(__ET310__)||defined (__ET320__)||defined(__MT200__)||defined(__SPANISH__)||defined(__ET350__)
    {
        1, 0
    },
#elif defined(__GT740__)
    {
        1, 0
    },                                             // 低电报警设置
#else
    {1, 1},                                             // 低电报警设置
#endif /* __GT02__ */

    {1, 0, 240, 20},                                    // 疲劳驾驶提醒
    {
#if defined(__GW100__) && defined(__LANG_ENGLISH__)
        {
            0, 2, 1, 0, 3, 0, 0, 0, 0
        },
        {0, 2, 1, 0, 3, 0, 0, 0, 0},
        {0, 2, 1, 0, 3, 0, 0, 0, 0},
        {0, 2, 1, 0, 3, 0, 0, 0, 0},
        {0, 2, 1, 0, 3, 0, 0, 0, 0}

#elif defined(__GT300__) || defined(__GW100__)||defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
        {
            0, 2, 1, 0, 3, 0, 0, 0, 0
        },
        {0, 2, 1, 0, 3, 0, 0, 0, 0},
        {0, 2, 1, 0, 3, 0, 0, 0, 0},
        {0, 2, 1, 0, 3, 0, 0, 0, 0},
        {0, 2, 1, 0, 3, 0, 0, 0, 0}
#elif defined(__GT500__)
        {
            0, 2, 0, 0, 3, 0, 0, 0, 0
        },
        {0, 2, 0, 0, 3, 0, 0, 0, 0},
        {0, 2, 0, 0, 3, 0, 0, 0, 0},
        {0, 2, 0, 0, 3, 0, 0, 0, 0},
        {0, 2, 0, 0, 3, 0, 0, 0, 0},
        {0, 2, 0, 0, 3, 0, 0, 0, 0},
        {0, 2, 0, 0, 3, 0, 0, 0, 0},
        {0, 2, 0, 0, 3, 0, 0, 0, 0},
        {0, 2, 0, 0, 3, 0, 0, 0, 0},
        {0, 2, 0, 0, 3, 0, 0, 0, 0}
#elif defined(__GT02E__) || defined(__GT03F__) || defined(__GT06D__) || defined(__NT23__)||defined(__NT36__)
        {
            0, 2, 1, 0, 3, 0, 0, 0, 0
        }
#elif (defined(__ET200__) || defined(__ET130__)||defined(__MT200__)||defined(__ET310__)) && defined(__LANG_ENGLISH__)
        {
            0, 2, 1, 0, 3, 0, 0, 0, 0
        }
#elif defined (__NT37__)
        {
            0, 2, 1, 0, 3, 0, 0, 0, 0
        }
#else
        {0, 2, 0, 0, 3, 0, 0, 0, 0}
#endif /* #if defined(__GT03F__)||defined(__GT030__) */
    },                                                  // 电子围栏设置
    {0, 0, 3000, 40, 10},                               // 锁电机参数

#if defined (__GT03F__) || defined(__GT300__) || defined(__GW100__)|| defined(__GT300S__)
    {
        1, 2, 60 * 24, 0 , 30, 2, 20
    },                                      // 终端工作模式
#elif defined(__GT310__)
    {
        1, 2, 24, 480, 30, 2, 20, 180, 30
    },
#elif defined(__GT370__)|| defined (__GT380__)
    {
        1, 2, 60 * 24, 0 , 30, 2, 20
    },						 // 终端工作模式
#else
    {0, 2, 60 * 24, 0 , 30, 2, 20},                                      // 终端工作模式
#endif /* __GT03F__ */

#if defined(__XCWS__)
    {
        1, 0, 128, 138, 10
    },                               // 外部电源低电报警
    {1, 0, 125, 130, 10},                               // 外部低电切断保护
#elif defined(__BCA__)
    {
        0, 0, 128, 138, 10
    },                               // 外部电源低电报警
    {0, 0, 125, 130, 10},                               // 外部低电切断保护
#elif defined(__GT500__)&& defined(__LANG_ENGLISH__)
    {
        0, 0, 110, 130, 10
    },                               // 外部电源低电报警
    {0, 0, 125, 130, 10},                               // 外部低电切断保护
#elif defined(__GT500__)
    {
        1, 0, 110, 130, 10
    },                               // 外部电源低电报警
    {0, 0, 125, 130, 10},                               // 外部低电切断保护
#elif (defined(__ET130__) ||defined(__ET310__) ||defined (__ET320__)) && defined(__LANG_ENGLISH__)
    {
        0, 1, 128, 138, 10
    },                               // 外部电源低电报警
    {1, 0, 125, 130, 10},                               // 外部低电切断保护
#elif defined(__ET130__)
    {
        0, 0, 128, 138, 10
    },                               // 外部电源低电报警
    {1, 0, 125, 130, 10},                               // 外部低电切断保护
#elif defined(__ET310__)|| defined(__V20__)
    {
        0, 0, 128, 138, 10
    },                               // 外部电源低电报警
    {1, 0, 125, 130, 10},                               // 外部低电切断保护
#elif defined(__GT06F__)
    {
        0, 1, 115, 125, 10
    },                               // 外部电源低电报警
    {0, 0, 125, 130, 10},                               // 外部低电切断保护
#elif defined(__NT31__)
    {
        0, 0, 115, 125, 10
    },                               // 外部电源低电报警
    {0, 0, 125, 130, 10},                               // 外部低电切断保护
#elif defined(__NT51__)
    {
        0, 1, 128, 138, 10
    },                               // 外部电源低电报警
    {1, 1, 115, 120, 10},                               // 外部低电切断保护
#elif defined(__MT200__) && defined(__LANG_ENGLISH__)
#if defined(__SERVER_IP_LINK__)
    {
        0, 1, 128, 138, 10
    },								// 外部电源低电报警
    {1, 0, 115, 120, 10},								// 外部低电切断保护
#else
    {0, 1, 128, 138, 10},                               // 外部电源低电报警
    {1, 0, 125, 125, 10},                               // 外部低电切断保护
#endif
#elif defined(__MT200__)
    {
        0, 0, 128, 138, 10
    },                               // 外部电源低电报警
    {1, 0, 125, 125, 10},                               // 外部低电切断保护
#elif defined(__NT37__)
    {
        0, 1, 115, 125, 10
    },                               // 外部电源低电报警
    {0, 0, 125, 130, 10},                               // 外部低电切断保护
#else
    {0, 1, 128, 138, 10},                               // 外部电源低电报警
    {0, 1, 125, 130, 10},                               // 外部低电切断保护
#endif /* __GT500__ */

#if defined( __GW100__)
    {
        0, 5
    },                                             // GPRS 阻塞报警
#elif defined (__NT33__)
    {
        0, 6
    },
#else
    {0, 3},                                             // GPRS 阻塞报警
#endif /* __GW100__ */


#if defined (__NT37__)
    {
        0, 1, 0, 1
    },                                         // 门报警,新加项用于过滤重复报警
#else
    {0, 1, 0},                                            // 门报警
#endif /* __NT37__ */


#if defined(__OBD__)
#ifdef  __OBD2__
    {1, 0, 7, 0, 0, {0}, 600, 0, 0, 0},                 // OBD
#else
    {1, 0, 7, 0, 0, {0}, 600, 0, 0, 0},                 // OBD
#endif /*  __OBD2__ */
#else
    {0, 1, 7, 0, 0, {0}, 60, 0, 0, 0},                  // OBD
#endif /* __OBD__ */
#if !defined(__GT500__)&&defined(__NT26__)
    {
        132, 135
    }                                          // ACC外部检测电压
#else
    {126, 128}                                          // ACC外部检测电压
#endif
#if defined(__XCWS__)
    , {1, 1, 60, 2}, //休眠参数
    15,                                            //acc off 后进入设防时间
    1,                                              //是否开启acc off 后自动进入设防
    1,                                              //电话呼入是撤设防还是定位
    15,                                              //acc off 后多久进行断电报警判断，保护时间
    1,                                              //是否实时上报定位数据
    0,                                              //是否开通来显
    0,                                              //震动呼叫功能
    0,                                              //是否需要短信激活
    1,                                              //非法位移报警发短信开关
    0,                                              //非法位移报警发平台开关
    1,                                              //断电和低电发短信开关
    0,                                              //断电和低电发平台开关
    1,                                              //位移报警短信开关
    0,                                              //位移报警网络开关
    1,                                              //标记acc状态变化
    15,                                              //移动gps数据发送频率
    10                                            //断电报警静默时间单位分钟
#endif
#if defined(__ACC_ALARM__)
    , 1
#else
    , 0                                             // 0 表示为ACC切换无短信通知，1 表示为ACC切换有短信通知
#endif
#if (defined(__BCA__) || defined(__ET130__) || defined(__ET200__)|| defined(__V20__) || defined(__ET310__) ||defined (__ET320__) || defined(__MT200__)) && !defined(__XCWS__)

#if defined(__BCA__) || defined(__SERVER_IP_LINK__) ||((defined (__ET310__) || defined(__ET130__)) && defined(__LANG_ENGLISH__))
    , 0
#else
    , 80
#endif /* __ET310__ */

#else
    , 0                                             // 电动车限速阀值  车载定位器必须为0
#endif

#if defined(__GW100__)
#if defined(__LANG_ENGLISH__)
    , {1, 1}                                        // 关机报警开关
    , {0, 1, "0000000000000000", "0000000000000000"}                   //换卡报警
#else
    , {1, 0}                                        // 开机报警开关
    , {0, 0, "0000000000000000", "0000000000000000"}                   //换卡报警
#endif/* __LANG_ENGLISH__ */
#elif defined (__GT370__)|| defined (__GT380__)
    , {1, 0}                                        // 开机报警开关
    , {0, 0, "0000000000000000"}                    // 换卡报警
#elif defined(__GT310__)
    , {0, 0}                                        // 开机报警开关
    , {0, 1, "0000000000000000"}                    // 换卡报警
#else
    , {0, 0}                                        // 开机报警开关
    , {1, 0, "0000000000000000"}                    // 换卡报警
#endif /* __GW100__ */

    , 0                                             // 限制关机功能,只用于有开关机键的项目

#if defined( __GW100__)
    , 10                                            // 屏显时间
#else
    , 20
#endif /* __GW100__ */
    , 0                                             // ADC4补偿值
    , 0                                             // mode3_work_interval;//休眠模式下定时上报间隔
    , 1                                             // mode3_up_counts;//休眠模式下单次工作上报次数
#if defined(__GT740__)
    , {1, 180, 43200, 0, 1440, 0, 1, 0, {0}, 0, 2, 0
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
       ,2015, 6, 6, {0}
#endif//__MCU_LONG_STANDBY_PROTOCOL__
      }
#elif  defined(__GT420D__)
    , {1, 15, 0, 45, 0, 0, 0, 0, {0}, 0, 0, 0
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
       ,2015, 6, 6, {0}
#endif//__MCU_LONG_STANDBY_PROTOCOL__
      }  // extchip
#else
    ,{0}
#endif
    , {"OBDMIFI", "123456"}                         // ap 用户名+密码
#if defined(__GT300S__) || defined(__GT370__)|| defined (__GT380__)
    , {1, 0, "0000000000000000"}                    // smstc 透传
#else
    , {0, 0, "0000000000000000"}                    // smstc 透传
#endif /* __GT300S__ */
#if defined(__XCWS_TPATH__)
    , 1                                             // 双线开关
#endif
#if defined(__XCWS__)
    , 0
#elif defined(__ET200__) || (defined(__NT23__) && defined(__OBD__))||defined(__ET130__)
#if defined(__ET200__) || defined(__ET130__)
    , 0
#else
#ifdef __NT26__
    , 2
#else
    , 1                     // 自动设防模式 X=0  自动设防模式；X=1 手动设防模式
#endif /* __NT23__ */
#endif
#elif defined(__GT500S__)
    , 1                     // 自动设防模式 X=0  自动设防模式；X=1 手动设防模式
#else
    , 0                     // 自动设防模式 X=0  自动设防模式；X=1 手动设防模式
#endif /* __ET200__ */
#if defined(__MT200__) || defined(__ET310__)||defined (__ET320__)||defined(__NT37__)
    , {0, 600}                                       //(几米国际)外接电源电压信息上传开关
#elif defined(__GT380__)
    , {1, 600}                  //adc4 to server
#else
    , {0, 60}                                       //(几米国际)外接电源电压信息上传开关
#endif

#if defined (__GT370__)||defined (__GT380__)
    , {1, 0}                                        //teardown_str //防拆报警
#elif defined(__GT740__)
    ,{1,0,3,15}
#elif defined(__GT420D__)
    ,{1,0,160}									//crash_str   碰撞报警
    ,{0,0,45}										//incline_str   倾斜报警
#else
    , {0, 0}                                        //teardown_str //防拆报警

#endif /* __GT370__ */

#ifdef __WIFI__
#if defined(__GT740__)||defined(__GT370__)|| defined (__GT380__)||defined(__GT310__)         //WIFI设置
    , {1, 10, 0}
#elif defined(__GT420D__)
    , {1, 10, 0}
#else
    , {0, 10, 10 * 60}
#endif
#else
    , {0, 10, 10 * 60}
#endif /* __WIFI__ */
#if defined(__ET310__) || defined(__NT36__)|| defined(__NT33__)||defined(__NT37__)
    , {1, 60, 60}           //lbson // LBS传点设置
#elif defined(__NT31__)
    , {0, 60, 60}           //lbson // LBS传点设置
#elif defined(__GT370__)|| defined (__GT380__)
    , {1, 120, 120}        //LBS传点设置
#else
    , {0, 60, 60}
#endif /* __NT31__ */

#if defined (__NT33__)
    , {0, 0, 30}           //伪基站报警
#else
    , {0, 0, 30}           //伪基站报警
#endif

#if defined(__BCA__) || (defined(__ET310__) && defined(__LANG_ENGLISH__))
    , 0                     //飞行模式开关默认开
#else
    , 1                     //飞行模式开关默认开
#endif /* __LANG_ENGLISH__ */
    , {0}                  //在线下载配置参数文件的MD5
#if defined(__SECOND_SET__)
    , 1    // EPO下载开关
#else
    , 0
#endif
    , 2                                             // 5V电源输出状态
#if defined(__ACC_ALARM__)
    , {0, 1, 10, 0}            //ACC报警包检测时间
#endif
#if defined(__GT740__)||defined(__GT420D__)
    , 1                 //电量次数计算开关，默认开
    , 0                  //启用新定位包上传开关 默认关
    , {0, 20}          //静态补传
#endif

#if defined (__NETWORK_LICENSE__)
    , 0
#elif defined (__NT33__)
//fake_cell_enable  国内版开启，外贸版关闭
#if defined (__LANG_ENGLISH__)
    , 0
#else
    , 1
#endif /* __LANG_ENGLISH__ */

#elif defined(__GT740__)|| defined(__GT300S__)||defined(__GT420D__)
    , 0                 //fake_cell_enable
#elif defined (__NT31__)
    , 0                 //fake_cell_enable
#else
    , 0                 //fake_cell_enable 防伪基站全部关闭，生产按需打开
#endif

    , 0                //伪基站信息是否上报AMS

#if defined (__GT370__)|| defined (__GT380__) || defined(__SERVER_IP_LINK__)||defined(__INDIA__)
    , 1                 //gpsDup gps位置数据上报开关
#else
    , 0                 //gpsDup gps位置数据上报开关
#endif

#if defined (__CUST_BT__) && defined(__NT33__)
    , {0, 2, 2, 0}      //bt
#endif /* __CUST_BT__ */
    , 0  //唤醒GPS 默认0

#if defined (__GT370__)|| defined (__GT380__)
    ,{1,0}      //开盖报警
#endif /* __GT370__ */
    , 0xFF         //临时自动关防伪功能
#if defined(__SPANISH__)
    , 0
#endif
    , {1, 1, 1}
#if defined(__GT310__)
    , {1, 720, 420} //飞行机制
#endif
#if defined(__ET310__)
    , {0, 20, 5}
#else
    , {0, 10, 5}
#endif /* __ET310__ */
    , 0
#if defined(__DROP_ALM__)//跌倒报警
    , {1, 1}
#endif
    , 0  //通话模式
    , 0
    ,1
    ,0 //电量不校准
#if defined(__GT420D__)
    ,0
    ,0
    ,{1,180,3,60,1}
    ,{1,15}
    ,{180,60}
    ,{0,0,3,15}									//teardown_str //防拆报警
    ,0
    ,10
    ,5
#endif
#if defined(__DASOUCHE__)
    ,{0,0,70,""}
#endif
#if defined(__MQTT__)
    ,{0,{0,0,0,0},0}
    ,0
    ,0.5
    ,2
#endif
    ,0
};


const nvram_phone_number_struct NVRAM_EF_PHONE_NUMBER_DEFAULT =
{
    {0},
    {0},
#if defined(__GT03F__)
    {
        1, 1, 1, 1
    }
#else
    {1, 1, 1}
#endif
};

const nvram_ef_logd_struct NVRAM_EF_LOGD_DEFAULT =
{
    /*save paramter for log wangqi*/
    {0}, //kal_uint8          logd_mdoule[L_MAX];
    //{L_CON,L_DRV,L_SOC,L_CMD,L_OS,L_APP,L_SYS,}//请注意参数个数与L_MAX对应
    0,        //kal_uint16          log_sw;
    LOG_AUTO,    //log_target          logd_target;
    ""
};

const nvram_realtime_struct NVRAM_EF_REALTIME_DEFAULT =
{
    {0},                    // 电子围栏 当前是出于围栏内(1)还是围栏外(2)
    0,                      //低电报包只发送一次标记
    0,                      // 低电报警发送标记
    0,                      // 外部电源低电报警发送标记
    0,                      // 外部电源断开发送标记
    0,                      // 位移报警 标记
    0,                      // 位移报警
    0,                      // 位移报警
    0,                      // 在线网络上报日志控制
    0,                      // 20分钟重启连续计数
    0,                      // 设防状态  1 撤防，2 预设防，3 设防
#if defined(__GT500__)||defined(__NT31__)||defined(__MT200__)||defined(__ET310__)||defined (__NT33__)||defined(__ET350__)
    0,                      //ledsw
#else
    1,                      // 按键状态  0 关    1 开
#endif
    0,                      // 油电状态
    0,                      // 断油电指令来源
    2,                      // 用于记录ACC状态，防止开机后的状态与上次关机前一致
    0,                      // 低电关机报警发送标记
    0,                      // 省电模式(0:正常模式;1:省电模式)
    1,                      // 是否发送阻塞报警短信(0不发送，1发送)
    0,                      // L4C_RAC_LIMITED_SERVICE 重启计数
    2,                      // 盲区报警状态0，为进，1为出，2为初始值。
#if (defined(__NT23__) && defined(__OBD__))
    1,                      // 自动设防模式 X=0  自动设防模式；X=1 手动设防模式，默认X=1
#elif defined (__GT370__)||defined(__GT380__)
    1,                      // 自动设防模式 X=0  自动设防模式；X=1 手动设防模式
#else
    0,                      // 自动设防模式 X=0  自动设防模式；X=1 手动设防模式，默认X=1
#endif /* __ET200__ */
    1,                       // 定位数据上传开关  0 关闭  1 打开
    {0},
    1,                       // 外部电压6v之下报警后设置为1, 6v之上为0
    0,                       //换卡报警标志位
    500,
    0,//音频数量
    10,//时长
    {{0}, {0}, {0}, {0}, {0}}, //音频文件
    0,                       // 休眠模式开启工作倒计时
    {0},                     //server_ip  域名解析得到的IP地址备份
    0,
    0,
#if defined(__MECH_SENSOR__)
    1,
    700,
#else
    1,
    1000,
#endif
    0,
    20,//追踪模式时间
    0,
    0,
    0,
    1,//自动上传参数
    1, //ams 保存LOG开关
    0,    //710上传LOG开关
    0,     //定位包协议次数
    0    // 联网重启标记
#if defined (__BAT_TEMP_TO_AMS__)
    , {0}
    , 0
#endif /* __BAT_TEMP_TO_AMS__ */
    , 0, //是否不停上传断电报警标志位
    {0}//伪基站信息记录

    , 0 //主基站ID
#if defined(__GT740__)||defined(__GT420D__)
    , 0 // 重启标志
    ,0
    ,{0}//电量算法
    ,0//追踪模式结束时间
    ,0
#endif
    ,2

#if defined (__LUYING__)
    ,0      //record_file
    ,0
    ,0      //recard_time
    ,{0}    //录音上传的流水号
    ,0
#endif /* __LUYING__ */
#if defined(__MCU_LONG_STANDBY_PROTOCOL__)
#if defined(__GT740__)||defined(__GT420D__)
    ,0
    ,0
    ,0
#endif//__GT740__
    ,0//辅助测试开关
    ,0//GSM开机次数
    ,0
#endif//__MCU_LONG_STANDBY_PROTOCOL__
#if defined(__MCU_LONG_STANDBY_PROTOCOL__) || defined(__JM66__)
    ,0
#endif//__MCU_LONG_STANDBY_PROTOCOL__
#if defined(__JM66__)
    ,{0}
    ,0
    ,0
#endif//__JM66__
#if defined(__ATGM_UPDATE__)
    ,0
    ,{0}
#endif /*__ATGM_UPDATE__*/
    ,0//eph数据存储时间
#if defined(__GT420D__)
    ,0//记录追踪次数
    ,0
    ,1
    ,2
    ,0
    ,0
#endif
};

#pragma arm section rodata

nvram_drv_importance_backup_struct   importance_backup = {0};

nvram_parameter_struct* OTA_NVRAM_EF_PARAMETER_DEFAULT = (nvram_parameter_struct*)&NVRAM_EF_PARAMETER_DEFAULT;
nvram_importance_parameter_struct* OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT  = (nvram_importance_parameter_struct*)&NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT;
nvram_phone_number_struct* OTA_NVRAM_EF_PHONE_NUMBER_DEFAULT = (nvram_phone_number_struct*)&NVRAM_EF_PHONE_NUMBER_DEFAULT ;
nvram_ef_logd_struct* OTA_NVRAM_EF_LOGD_DEFAULT = (nvram_ef_logd_struct*)&NVRAM_EF_LOGD_DEFAULT;
nvram_realtime_struct* OTA_NVRAM_EF_REALTIME_DEFAULT = (nvram_realtime_struct*)&NVRAM_EF_REALTIME_DEFAULT;
nvram_alone_parameter_struct* OTA_NVRAM_ALONE_PARAMETER_DEFAULT = (nvram_alone_parameter_struct*)&NVRAM_ALONE_PARAMETER_DEFAULT;

extern void track_cust_paket_9404_handle(void);

void track_os_importance_backup_write(void)
{
    static char *s_ON = {"ON"}, *s_OFF = {"OFF"}, *s_Error = {"ERROR"};
    kal_uint32 len = 2000, l = 0;
    //LOGD(L_APP, L_V5, "");
#ifdef __GT02__
    kal_uint8 chip_rid[16];
    kal_uint16 CRC, CRC_IN = 0;
    memcpy(chip_rid, track_drv_get_chip_id(), 16);
    CRC = GetCrc16(chip_rid, 16);
#endif /* __GT02__ */
#if defined(__GT740__)||defined(__GT420D__)
    if (track_cust_check_disk1_nvram_parameter(1) ==KAL_FALSE)
    {
        LOGD(L_DRV, L_V5, "error eurl=%s sensor level:%d", G_parameter.url,G_parameter.vibrates_alarm.sensitivity_level);
        return;
    }
    LOGD(L_APP,L_V5,"G_parameter.teardown_str.sw:%d",G_parameter.teardown_str.sw);
#endif
    memset(&importance_backup, 0x00, sizeof(importance_backup));

    l += OTA_snprintf(importance_backup.data + l, len - l, "ASETAPN,%s\r\n", (track_nvram_alone_parameter_read()->apn.custom_apn_valid ? s_OFF : s_ON));
    if (track_nvram_alone_parameter_read()->apn.custom_apn_valid)
    {
        extern nvram_GPRS_APN_struct *track_socket_get_use_apn(void);
        nvram_GPRS_APN_struct *use_apn = track_socket_get_use_apn();
        l += OTA_snprintf(importance_backup.data + l, len - l, "APN,%s,%s,%s\r\n", use_apn->apn, use_apn->username, use_apn->password);
    }
#ifdef __GT02__
    l += OTA_snprintf(importance_backup.data + l, len - l, "SYS_VERSION,%d,%d\r\n", G_importance_parameter_data.app_version.version, CRC);
#endif /* __GT02__ */

#if defined(__BACKUP_SERVER__)
    {
        if (G_importance_parameter_data.bserver.conecttype == 0)
        {
            l += OTA_snprintf(importance_backup.data + l, len - l, "BSERVER,0,%d.%d.%d.%d,%d,%d\r\n",
                              G_importance_parameter_data.bserver.server_ip[0], G_importance_parameter_data.bserver.server_ip[1],
                              G_importance_parameter_data.bserver.server_ip[2], G_importance_parameter_data.bserver.server_ip[3],
                              G_importance_parameter_data.bserver.server_port, G_importance_parameter_data.bserver.soc_type);
        }
        else
        {
            l += OTA_snprintf(importance_backup.data + l, len - l, "BSERVER,%d,%s,%d,%d\r\n",
                              G_importance_parameter_data.bserver.conecttype,
                              track_domain_decode(G_importance_parameter_data.bserver.url),
                              G_importance_parameter_data.bserver.server_port,
                              G_importance_parameter_data.bserver.soc_type);
        }
    }
#endif

    if (G_importance_parameter_data.dserver.server_type)
    {
        if (G_importance_parameter_data.dserver.conecttype == 0)
        {
            l += OTA_snprintf(importance_backup.data + l, len - l, "DSERVER,0,%d.%d.%d.%d,%d,%d\r\n",
                              G_importance_parameter_data.dserver.server_ip[0], G_importance_parameter_data.dserver.server_ip[1],
                              G_importance_parameter_data.dserver.server_ip[2], G_importance_parameter_data.dserver.server_ip[3],
                              G_importance_parameter_data.dserver.server_port, G_importance_parameter_data.dserver.soc_type);
        }
        else
        {
            l += OTA_snprintf(importance_backup.data + l, len - l, "DSERVER,%d,%s,%d,%d\r\n",
                              G_importance_parameter_data.dserver.conecttype,
                              track_domain_decode(G_importance_parameter_data.dserver.url),
                              G_importance_parameter_data.dserver.server_port,
                              G_importance_parameter_data.dserver.soc_type);
        }
    }
    else
    {
        if (track_nvram_alone_parameter_read()->server.conecttype == 0)
        {
            l += OTA_snprintf(importance_backup.data + l, len - l, "SERVER,0,%d.%d.%d.%d,%d,%d\r\n",
                              track_nvram_alone_parameter_read()->server.server_ip[0], track_nvram_alone_parameter_read()->server.server_ip[1],
                              track_nvram_alone_parameter_read()->server.server_ip[2], track_nvram_alone_parameter_read()->server.server_ip[3],
                              track_nvram_alone_parameter_read()->server.server_port, track_nvram_alone_parameter_read()->server.soc_type);
        }
        else
        {
            l += OTA_snprintf(importance_backup.data + l, len - l, "SERVER,%d,%s,%d,%d\r\n",
                              track_nvram_alone_parameter_read()->server.conecttype,
                              track_domain_decode(track_nvram_alone_parameter_read()->server.url),
                              track_nvram_alone_parameter_read()->server.server_port,
                              track_nvram_alone_parameter_read()->server.soc_type);
        }
    }

#if defined(__DASOUCHE__)
    l += OTA_snprintf(importance_backup.data + l, len - l, "IOT,%s,%s\r\n",                 
                G_importance_parameter_data.mqtt_login.product_key,               
                G_importance_parameter_data.mqtt_login.device_secret);
    if(G_parameter.sim_alarm.sw == 0)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "SIMALM,%s\r\n", ON_OFF(G_parameter.sim_alarm.sw));
        }
    else
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "SIMALM,%s,%d\r\n", ON_OFF(G_parameter.sim_alarm.sw),G_parameter.sim_alarm.simalm_type);
        }
    l += OTA_snprintf(importance_backup.data + l, len - l, "SENSORTIME,1,%d,%d\r\n", G_parameter.sensortime.static_check_time,G_parameter.sensortime.static_sensorcount);
    l += OTA_snprintf(importance_backup.data + l, len - l, "SENSORTIME,2,%d,%d\r\n", G_parameter.sensortime.exercise_check_time,G_parameter.sensortime.exercise_sensorcount);
    l += OTA_snprintf(importance_backup.data + l, len - l, "STATICTIME,1,%d\r\n", G_parameter.statictime.mode1_statictime);
    l += OTA_snprintf(importance_backup.data + l, len - l, "STATICTIME,2,%d\r\n", G_parameter.statictime.mode2_statictime);
#endif
#if defined(__GT740__)||defined(__GT420D__)
    if (G_parameter.extchip.mode == 3)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "MODE,3,%d,%0.2d:%0.2d\r\n", G_parameter.extchip.mode3_par1, G_parameter.extchip.mode3_par2 / 60, G_parameter.extchip.mode3_par2 % 60);
    }
    else if (G_parameter.extchip.mode == 4)
    {
        char m_buf[100] = {0};
        U8 vle = G_parameter.extchip.mode4_par1, i = 0;
        for (; i < vle; i++)
        {
            if (0 == i)
            {
                snprintf(m_buf + strlen(m_buf), 99, "%0.2d:%0.2d", G_parameter.extchip.mode4_par2[i] / 60, G_parameter.extchip.mode4_par2[i] % 60);
            }
            else
            {
                snprintf(m_buf + strlen(m_buf), 99, ",%0.2d:%0.2d", G_parameter.extchip.mode4_par2[i] / 60, G_parameter.extchip.mode4_par2[i] % 60);
            }
        }
        l += OTA_snprintf(importance_backup.data + l, len - l, "MODE,4,%d,%s\r\n", vle, m_buf);
    }
#if defined(__GT420D__)
    if (track_nvram_alone_parameter_read()->ams_server.conecttype == 0)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "SERVER2,0,%d.%d.%d.%d,%d,%d\r\n",
                          track_nvram_alone_parameter_read()->ams_server.server_ip[0], track_nvram_alone_parameter_read()->ams_server.server_ip[1],
                          track_nvram_alone_parameter_read()->ams_server.server_ip[2], track_nvram_alone_parameter_read()->ams_server.server_ip[3],
                          track_nvram_alone_parameter_read()->ams_server.server_port, track_nvram_alone_parameter_read()->ams_server.soc_type);
    }
    else
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "SERVER2,%d,%s,%d,%d\r\n",
                          track_nvram_alone_parameter_read()->ams_server.conecttype,
                          track_domain_decode(track_nvram_alone_parameter_read()->ams_server.url),
                          track_nvram_alone_parameter_read()->ams_server.server_port,
                          track_nvram_alone_parameter_read()->ams_server.soc_type);
    }
    if (G_realtime_data.gps_420d_flag == 1)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "_SYS,GPS,ON\r\n");
    }
    else if (G_realtime_data.gps_420d_flag ==2)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "_SYS,GPS,OFF\r\n");
    }
    if (G_parameter.extchip.mode == 1)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "MODE,1,%0.2d\r\n", G_parameter.extchip.mode1_par1);
    }
    else if (G_parameter.extchip.mode == 2)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "MODE,2,%0.2d\r\n", G_parameter.extchip.mode2_par1);
    }
    l += OTA_snprintf(importance_backup.data + l, len - l, "ANGLEREP,%s,%d,%d\r\n", ON_OFF(G_parameter.gps_work.anglerep.sw),G_parameter.gps_work.anglerep.angleOfDeflection,G_parameter.gps_work.anglerep.detection_time);
    l += OTA_snprintf(importance_backup.data + l, len - l, "LOWBAT,%d\r\n", G_parameter.percent);
    l += OTA_snprintf(importance_backup.data + l, len - l, "LINK,%d\r\n", G_parameter.gprs_obstruction_alarm.link);
    l += OTA_snprintf(importance_backup.data + l, len - l, "ALMREP,%d,%d,%d\r\n", G_phone_number.almrep[0], G_phone_number.almrep[1], G_phone_number.almrep[2]);
    if (G_parameter.low_power_alarm.sw == 1)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "BATALM,%s,%d\r\n", ON_OFF(G_parameter.low_power_alarm.sw), G_parameter.low_power_alarm.alarm_type);
    }
    else
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "BATALM,%s\r\n", ON_OFF(G_parameter.low_power_alarm.sw));
    }
    l += OTA_snprintf(importance_backup.data + l, len - l, "HBT,%d\r\n", (G_parameter.hbt_acc_off/60));
    if (G_parameter.teardown_str.sw == 1)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "REMALM,%s,%d\r\n", ON_OFF(G_parameter.teardown_str.sw), G_parameter.teardown_str.alarm_type);
    }
    else
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "REMALM,%s\r\n", ON_OFF(G_parameter.teardown_str.sw));
    }
    l += OTA_snprintf(importance_backup.data + l, len - l, "HUMITURE,%s\r\n", ON_OFF(G_parameter.humiture_sw));
    l += OTA_snprintf(importance_backup.data + l, len - l, "HUMIThr,%f,%f\r\n",G_parameter.temperature_threshold,G_parameter.humidity_threshold);
#else
    else
    {
        if (G_parameter.extchip.mode2_par2 % 60 == 0)
        {
            l += OTA_snprintf(importance_backup.data + l, len - l, "MODE,2,%d:%d,%d\r\n", G_parameter.extchip.mode2_par1/60,G_parameter.extchip.mode2_par1%60,G_parameter.extchip.mode2_par2/60);
        }
        else
        {
            l += OTA_snprintf(importance_backup.data + l, len - l, "MODE,2,%d:%d,MIN%d\r\n", G_parameter.extchip.mode2_par1/60,G_parameter.extchip.mode2_par1%60,G_parameter.extchip.mode2_par2);
        }
    }
#endif
    l += OTA_snprintf(importance_backup.data + l, len - l, "PWDSW,%s\r\n", ON_OFF(G_parameter.Permission.password_sw));
    if (G_parameter.Permission.password_sw)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "PASSWORD,%s\r\n", G_parameter.Permission.password);
    }
    l += OTA_snprintf(importance_backup.data + l, len - l, "REMALMSET,%d,%d\r\n", G_parameter.teardown_str.alarm_upcount,G_parameter.teardown_str.alarm_vel_min);
    l += OTA_snprintf(importance_backup.data + l, len - l, "GPSWAKE,%s\r\n",   ON_OFF(G_parameter.gpswake));
    //l += OTA_snprintf(importance_backup.data + l, len - l, "SYS_VERSION,%d\r\n", G_importance_parameter_data.app_version.version);
    l += OTA_snprintf(importance_backup.data + l, len - l, "TEARDOWN,%s,%d\r\n", ON_OFF(G_parameter.teardown_str.sw), G_parameter.teardown_str.alarm_type);
#else
    if (track_nvram_alone_parameter_read()->ams_server.conecttype == 0)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "SERVER2,0,%d.%d.%d.%d,%d,%d\r\n",
                          track_nvram_alone_parameter_read()->ams_server.server_ip[0], track_nvram_alone_parameter_read()->ams_server.server_ip[1],
                          track_nvram_alone_parameter_read()->ams_server.server_ip[2], track_nvram_alone_parameter_read()->ams_server.server_ip[3],
                          track_nvram_alone_parameter_read()->ams_server.server_port, track_nvram_alone_parameter_read()->ams_server.soc_type);
    }
    else
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "SERVER2,%d,%s,%d,%d\r\n",
                          track_nvram_alone_parameter_read()->ams_server.conecttype,
                          track_domain_decode(track_nvram_alone_parameter_read()->ams_server.url),
                          track_nvram_alone_parameter_read()->ams_server.server_port,
                          track_nvram_alone_parameter_read()->ams_server.soc_type);
    }

    l += OTA_snprintf(importance_backup.data + l, len - l, "GPRSON,%d\r\n", track_nvram_alone_parameter_read()->gprson);

    l += OTA_snprintf(importance_backup.data + l, len - l, "LINK,%d\r\n", G_parameter.gprs_obstruction_alarm.link);

    /*需要重点测试*/
    l += OTA_snprintf(importance_backup.data + l, len - l, "HBT,%d,%d\r\n", G_parameter.hbt_acc_on, G_parameter.hbt_acc_off);
    l += OTA_snprintf(importance_backup.data + l, len - l, "ADC_BC,B,%d\r\n", G_parameter.adc4_offset);
    l += OTA_snprintf(importance_backup.data + l, len - l, "DEFMODE,%d\r\n", G_realtime_data.defense_mode);
    if (G_parameter.speed_limit_alarm.sw)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "SPEED,%s,%d,%d,%d\r\n",
                          ON_OFF(G_parameter.speed_limit_alarm.sw), G_parameter.speed_limit_alarm.delay_time,
                          G_parameter.speed_limit_alarm.threshold, G_parameter.speed_limit_alarm.alarm_type);
    }
    else
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "SPEED,%s\r\n",
                          ON_OFF(G_parameter.speed_limit_alarm.sw));
    }
    if (G_parameter.power_fails_alarm.sw)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "POWERALM,%s,%d,%d,%d,%d\r\n",
                          ON_OFF(G_parameter.power_fails_alarm.sw),
                          G_parameter.power_fails_alarm.alarm_type,
                          G_parameter.power_fails_alarm.delay_time_off,
                          G_parameter.power_fails_alarm.delay_time_on,
                          G_parameter.power_fails_alarm.delay_time_acc_on_to_off);
    }
    else
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "POWERALM,%s\r\n",
                          ON_OFF(G_parameter.power_fails_alarm.sw));
    }
    if (G_parameter.vibrates_alarm.sw)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "SENALM,%s,%d\r\n", ON_OFF(G_parameter.vibrates_alarm.sw), G_parameter.vibrates_alarm.alarm_type);
    }
    else
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "SENALM,%s\r\n", ON_OFF(G_parameter.vibrates_alarm.sw));
    }

#if defined(__XCWS__)||defined(__NT36__)||defined(__MT200__)
    l += OTA_snprintf(importance_backup.data + l, len - l, "SENSOR,%d,%d,%d\r\n",
                      G_parameter.vibrates_alarm.detection_time, G_parameter.vibrates_alarm.alarm_delay_time,
                      G_parameter.vibrates_alarm.alarm_interval / 60);
#else
    l += OTA_snprintf(importance_backup.data + l, len - l, "SENSOR2,%d,%d,%d\r\n",
                      G_parameter.vibrates_alarm.detection_time, G_parameter.vibrates_alarm.alarm_delay_time,
                      G_parameter.vibrates_alarm.alarm_interval);
#endif
    /*需要重点测试*/
    l += OTA_snprintf(importance_backup.data + l, len - l, "PWDSW,%s\r\n", ON_OFF(G_parameter.Permission.password_sw));
    if (G_parameter.Permission.password_sw)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "PASSWORD,%s\r\n", G_parameter.Permission.password);
    }

    l += OTA_snprintf(importance_backup.data + l, len - l, "CENTER,A,%s\r\n", G_parameter.centerNumber);
    l += OTA_snprintf(importance_backup.data + l, len - l, "SOSALM,%s,%d\r\n", ON_OFF(G_parameter.sos_alarm.sw), G_parameter.sos_alarm.alarm_type);
    l += OTA_snprintf(importance_backup.data + l, len - l, "ALMREP,%d,%d,%d\r\n", G_phone_number.almrep[0], G_phone_number.almrep[1], G_phone_number.almrep[2]);

    l += OTA_snprintf(importance_backup.data + l, len - l, "PERMIT,%d\r\n", G_parameter.Permission.Permit);

#endif


    l += OTA_snprintf(importance_backup.data + l, len - l, "EURL,%s\r\n", G_parameter.url);

    l += OTA_snprintf(importance_backup.data + l, len - l, "LANG,%d\r\n", G_parameter.lang);

    /*需要重点测试*/
//   l += OTA_snprintf(importance_backup.data + l, len - l, "IMEI,1,%s\r\n", (char *)track_drv_get_imei(0));

    l += OTA_snprintf(importance_backup.data + l, len - l, "AGPS,%s\r\n", ON_OFF(G_parameter.agps.sw));

    if (track_nvram_alone_parameter_read()->server.lock)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "LOCKIP\r\n");
    }

    l += OTA_snprintf(importance_backup.data + l, len - l, "ASETGMT,%s\r\n", ON_OFF(G_parameter.zone.timezone_auto));
    if (G_parameter.zone.timezone_auto == 0)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "GMT,%c,%d,%d\r\n", G_parameter.zone.zone, G_parameter.zone.time, G_parameter.zone.time_min);
    }


    l += OTA_snprintf(importance_backup.data + l, len - l, "SOS,A,%s,%s,%s\r\n", G_parameter.sos_num[0], G_parameter.sos_num[1], G_parameter.sos_num[2]);


    if (G_parameter.low_power_alarm.sw)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "BATALM,%s,%d\r\n", ON_OFF(G_parameter.low_power_alarm.sw), G_parameter.low_power_alarm.alarm_type);
    }
    else
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "BATALM,%s\r\n", ON_OFF(G_parameter.low_power_alarm.sw));
    }



#if defined(__XCWS__)
    l += OTA_snprintf(importance_backup.data + l, len - l, "SN,%d,%s,%s\r\n", G_importance_parameter_data.login.MachineType, G_importance_parameter_data.login.ProtocolVersion, G_importance_parameter_data.login.SequenceNumber);
#if defined(__XCWS_TPATH__)
    l += OTA_snprintf(importance_backup.data + l, len - l, "SERVER3,%d,%s,%d,%d\r\n",
                      G_importance_parameter_data.server3.conecttype,
                      track_domain_decode(G_importance_parameter_data.server3.url),
                      G_importance_parameter_data.server3.server_port,
                      G_importance_parameter_data.server3.soc_type);
#endif
#endif

    /*
    l += OTA_snprintf(importance_backup.data + l, len - l, "REPORT\r\n", );
    l += OTA_snprintf(importance_backup.data + l, len - l, "\r\n", );
    l += OTA_snprintf(importance_backup.data + l, len - l, "\r\n", );
    l += OTA_snprintf(importance_backup.data + l, len - l, "\r\n", );
    l += OTA_snprintf(importance_backup.data + l, len - l, "\r\n", );
    l += OTA_snprintf(importance_backup.data + l, len - l, "\r\n", );
    l += OTA_snprintf(importance_backup.data + l, len - l, "\r\n", );
    l += OTA_snprintf(importance_backup.data + l, len - l, "\r\n", );
    l += OTA_snprintf(importance_backup.data + l, len - l, "\r\n", );
    l += OTA_snprintf(importance_backup.data + l, len - l, "\r\n", );
    l += OTA_snprintf(importance_backup.data + l, len - l, "\r\n", );*/



#if defined(__GT300S__)
    if (G_parameter.work_mode.mode == 2)
    {
        sprintf(importance_backup.data + l, "MODE,%d,%d",  G_parameter.work_mode.mode, G_parameter.gps_work.Fti.interval_acc_on);
    }
    else  if (G_parameter.work_mode.mode == 1)
    {
        sprintf(importance_backup.data + l, "MODE,%d,%d", G_parameter.work_mode.mode, G_parameter.work_mode.mode1_timer);

    }
    else if (G_parameter.work_mode.mode == 3)
    {
        sprintf(importance_backup.data + l, "MODE,%d,%0.2d:%0.2d,%d",  G_parameter.work_mode.mode,
                G_parameter.work_mode.datetime / 60,  G_parameter.work_mode.datetime % 60,  G_parameter.work_mode.mode3_timer / 60);
    }
#elif defined(__GT370__)|| defined (__GT380__)
    if (G_parameter.work_mode.mode == 2)
    {
        sprintf(importance_backup.data + l, "MODE,%d,%d,%d",  G_parameter.work_mode.mode, G_parameter.gps_work.Fti.interval_acc_on, G_parameter.gps_work.Fti.interval_acc_off);
    }
    else  if (G_parameter.work_mode.mode == 1)
    {
        sprintf(importance_backup.data + l, "MODE,%d,%d,%d", G_parameter.work_mode.mode, G_parameter.gps_work.Fti.interval_acc_on, G_parameter.gps_work.Fti.interval_acc_off);

    }
    else if (G_parameter.work_mode.mode == 3)
    {
        sprintf(importance_backup.data + l, "MODE,%d,%0.2d:%0.2d,%d",  G_parameter.work_mode.mode,
                G_parameter.work_mode.datetime / 60,  G_parameter.work_mode.datetime % 60,	G_parameter.work_mode.mode3_timer / 60);
    }

#endif /* __GT300S__ */


#if defined (__XYBB__)
    if (track_nvram_alone_parameter_read()->server.conecttype == 0)
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "LOGIN,%d,%d.%d.%d.%d:%d,,%d,%s,%s,%s,%s,%s,%d,%s\r\n", G_importance_parameter_data.login_data.mode, track_nvram_alone_parameter_read()->server.server_ip[0], track_nvram_alone_parameter_read()->server.server_ip[1],
                          track_nvram_alone_parameter_read()->server.server_ip[2], track_nvram_alone_parameter_read()->server.server_ip[3], track_nvram_alone_parameter_read()->server.server_port, track_nvram_alone_parameter_read()->server.soc_type,
                          G_importance_parameter_data.login_data.sim_num, G_importance_parameter_data.login_data.provincial, G_importance_parameter_data.login_data.manufacturer,
                          G_importance_parameter_data.login_data.terminal_model, G_importance_parameter_data.login_data.terminal_id, G_importance_parameter_data.login_data.car_color,
                          G_importance_parameter_data.login_data.car_num);
    }
    else
    {
        l += OTA_snprintf(importance_backup.data + l, len - l, "LOGIN,%d,%s:%d,,%d,%s,%s,%s,%s,%s,%d,%s\r\n", G_importance_parameter_data.login_data.mode,track_domain_decode(track_nvram_alone_parameter_read()->server.url), track_nvram_alone_parameter_read()->server.server_port, track_nvram_alone_parameter_read()->server.soc_type,
                          G_importance_parameter_data.login_data.sim_num, G_importance_parameter_data.login_data.provincial, G_importance_parameter_data.login_data.manufacturer,
                          G_importance_parameter_data.login_data.terminal_model, G_importance_parameter_data.login_data.terminal_id, G_importance_parameter_data.login_data.car_color,
                          G_importance_parameter_data.login_data.car_num);
    }
#endif /* __XYBB__ */
    //LOGD(L_DRV, L_V5, "result=%d,strlen(data)=%d ,%s", len, strlen(importance_backup.data), importance_backup.data);
    l = strlen(importance_backup.data);
    LOGD(L_DRV, L_V2, "result=%d,strlen(data)=%d, stringCRC:%0.4X,\r\n%s", len, l, GetCrc16(importance_backup.data, l), importance_backup.data);
    Track_nvram_write(DISK1_BACKUP_LID, 1, (void *)&importance_backup, NVRAM_EF_DRV__IMP_BACKUP_SIZE);


    /*简单粗暴，但是会发重复发，有的客户不爽       --    chengjun  2017-07-13*/
#if defined(__NT31__)||defined(__GT03F__)||defined(__GT300S__)
    track_cust_paket_9404_handle();
#endif

}

void track_os_importance_backup_read(void)
{
    int l;
    LOGD(L_APP, L_V5, "");

    memset(&importance_backup, 0x00, sizeof(importance_backup));
    Track_nvram_read(DISK1_BACKUP_LID, 1, (void *)&importance_backup, NVRAM_EF_DRV__IMP_BACKUP_SIZE);
    l = strlen(importance_backup.data);
    LOGD(L_DRV, L_V7, "strlen(data)=%d, stringCRC:%0.4X,\r\n%s", l, GetCrc16(importance_backup.data, l), importance_backup.data);
    track_cmd_params_recv(importance_backup.data, CM_PARAM_CMD);
#if defined (__GT740__)||defined(__GT420D__)

    if (G_realtime_data.ams_log)
    {
        char sttime_str[100] = {0};
        snprintf(sttime_str, 99, "(Z)(%d)Update Software, Backup Important Data!!!", RUNING_TIME_SEC);
        //track_cust_save_logfile(sttime_str, strlen(sttime_str));
        G_realtime_data.ams_push=1;
        Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

    }

#endif

}

