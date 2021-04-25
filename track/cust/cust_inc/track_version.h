
/*******
版本号设置变更：
应用版本号的更改，修改文件：\track\cust\cust_inc\track_version.h
TRACK_VER_APP_  表示应用版本
TRACK_VER_PARAMETER_  表示参数版本（一般情况不便，只有需要单独出变更参数升级时需要修改）
TRACK_VER_BASE_ 表示基础版本，基础层未变更的情况无需修改，不修改意味着相同的基础版本支持交互升级
原\make\Verno_GXQ60D_MOD_11B.bld 文件不在需要修改。
******/

/*按目前的格式扩展，每个项目宏下面有3个#define关键字，否则就悲剧       --    chengjun  2016-07-15*/

#if defined (__NT31__)
#define _TRACK_VER_PARAMETER_   "R0"
#define _TRACK_VER_BASE_        "NT32_10_A1D_B4_F2"
#define _TRACK_VER_APP_         "NT32_11_A1D_D23_V09_INA"
//#define _TRACK_VER_APP_         "NT32_11_A1D_B4_V05_WM_PATCH1"
//#define _TRACK_VER_APP_         "NT32_10_A1D_B4_V07"
//#define _TRACK_VER_APP_         "NT32_11_A1D_B25E_V05_WM"
//#define _TRACK_VER_BASE_        "NT32_10_A1D_B4_F2"
//#define _TRACK_VER_APP_         "NT32_10_A1D_B4_V03"
//#define _TRACK_VER_BASE_        "NT32S_11_A1D_B4_F1"
//#define _TRACK_VER_APP_         "NT32S_11_A1D_D23_V06"
#elif defined (__NT22__)
#define _TRACK_VER_PARAMETER_   "R0"
#define _TRACK_VER_BASE_        "NT31_10_A1D_B4_F2"
#define _TRACK_VER_APP_         "NT31_10_A1D_B4_V04"
#elif defined (__MT200__)
#define _TRACK_VER_PARAMETER_   "R0"
//#define _TRACK_VER_BASE_        "MT200_11_A1D_B4_F1"
//#define _TRACK_VER_APP_         "MT200_11_A1D_B4_V02_WM"
//#define _TRACK_VER_APP_         "MT200_11_A1D_B4_V03_WM"
//#define _TRACK_VER_APP_         "MT200_11_A1D_B4_V04"
//#define _TRACK_VER_APP_         "MT200_11_A1D_B4_V05"
//#define _TRACK_VER_APP_         "MT200_11_A1D_D23_V05_RW"
//#define _TRACK_VER_APP_         "MT200_11_A1D_D23_V06_RW"
//#define _TRACK_VER_APP_         "MT200_11_A1D_D23_V07_WM"

#define _TRACK_VER_BASE_        "NT51_10_A1D_SC1_F1"
#define _TRACK_VER_APP_         "NT51_10_A1D_SC1_V02"

#elif defined (__GT300S__)
#define _TRACK_VER_PARAMETER_   "R0"
#define _TRACK_VER_BASE_        "GT300S_12_A1D_SC1_F1"
#define _TRACK_VER_APP_         "GT300S_12_A1D_SC1_V07"

#elif defined (__ET320__)
#define _TRACK_VER_PARAMETER_   "R0"
#define _TRACK_VER_BASE_        "ET320_10_A1D_B25E_F1"
#define _TRACK_VER_APP_         "XCWS_D02_10_A1D_B25E_V02"

#elif defined (__ET310__)
#define _TRACK_VER_PARAMETER_   "R0"
#define _TRACK_VER_BASE_        "ET310_20_A1D_RM1_F1"
//#define _TRACK_VER_APP_         "ET310_20_A1D_B25E_V03_WM"
//#define _TRACK_VER_APP_         "ET310_10_A1D_B25E_V04"
//#define _TRACK_VER_APP_         "ET310_10_A1D_B25E_V05_BCA"
//#define _TRACK_VER_APP_         "ET310_10_A1D_B25E_V06_BCA"
//#define _TRACK_VER_APP_         "ET310_10_A1D_B25E_V07"
//#define _TRACK_VER_APP_         "ET310_20_A1D_B4_V07"
//#define _TRACK_VER_APP_         "ET310_20_A1D_B4_V08_WM"
//#define _TRACK_VER_APP_         "ET310_20_A1D_B4_V09"
//#define _TRACK_VER_APP_         "ET310_20_A1D_B4_V10_BCA"
//#define _TRACK_VER_APP_         "ET310_20_A1D_RM1_V11"
//#define _TRACK_VER_APP_         "ET310_20_A1D_SC1_V12_WM"
//#define _TRACK_VER_APP_         "ET310_20_A1D_SC1_V13_BCA"
//#define _TRACK_VER_APP_         "ET310_20_A1D_SC1_V14_WM"
#define _TRACK_VER_APP_         "ET310_20_A1D_D23_V15"

#elif defined (__GT02E__)
#define _TRACK_VER_PARAMETER_   "R0"
#define _TRACK_VER_BASE_        "GT02E_20_61DM2_B25E_F2"
#define _TRACK_VER_APP_         "GT02ES_20_61DM2_B25E_VM_V20"

#elif defined (__NT36__)
#define _TRACK_VER_PARAMETER_   "R0"
//#define _TRACK_VER_BASE_        "NT36_10_A1D_B4_F1"
//#define _TRACK_VER_BASE_        "NT36_11_A1D_B4_F1"
//#define _TRACK_VER_BASE_        "NT36_10_A1D_RM1_F1"
#define _TRACK_VER_BASE_        "NT36_10_A1D_SC1_F1"

//#define _TRACK_VER_APP_         "NT36_10_A1D_B4_V07"
//#define _TRACK_VER_APP_         "NT36_11_A1D_B4_V03_WM"
//#define _TRACK_VER_APP_         "NT36_11_A1D_B4_V08_L_WM"
//#define _TRACK_VER_APP_         "NT36_11_A1D_B4_V10_L_GER"
//#define _TRACK_VER_APP_         "NT36_11_A1D_B4_V14_L_GER"
//#define _TRACK_VER_APP_         "NT36_11_A1D_B4_V12_L_MEX"
//#define _TRACK_VER_APP_         "NT36_10_A1D_RM1_V15"
//#define _TRACK_VER_APP_         "NT36_11_A1D_B4_V11_L_GT"
//#define _TRACK_VER_APP_         "NT36_11_A1D_B4_V13_L_WM"
//#define _TRACK_VER_APP_         "NT36_10_A1D_SC1_V16"
#define _TRACK_VER_APP_         "NT36_11_A1D_V13_DA213"

#elif defined (__NT33__)
#define _TRACK_VER_PARAMETER_   "R0"

//#define _TRACK_VER_BASE_        "NT33_20_A1D_D23_F1"
//#define _TRACK_VER_APP_         "NT33_20_A1D_D23_V08"

//#define _TRACK_VER_BASE_        "NT33_10_A1D_RM1_F1"
//#define _TRACK_VER_APP_         "NT33_10_A1D_RM1_V04_WM"

//#define _TRACK_VER_BASE_        "NT33_10_A1D_RM1_F1"
//#define _TRACK_VER_APP_         "NT33_10_A1D_RM1_V05_DDWL"
//#define _TRACK_VER_APP_         "NT33_10_A1D_RM1_V05_ANT"

//#define _TRACK_VER_BASE_        "NT33_20_A1D_SC1_F1"
//#define _TRACK_VER_APP_         "NT33_20_A1D_SC1_V06_WM"

//#define _TRACK_VER_BASE_        "NT33_20_A1D_SC1_F1"
//#define _TRACK_VER_APP_         "NT33_20_A1D_SC1_V09_WM_YDME"

#define _TRACK_VER_BASE_        "NT33_20_A1D_D23_F1"
#define _TRACK_VER_APP_         "NT33_20_A1D_D23_V11_WM_YDME"


#elif defined (__GT370__)
#define _TRACK_VER_PARAMETER_   "R0"

#define _TRACK_VER_BASE_        "NT97_10_A1D_D23_F1"
#define _TRACK_VER_APP_         "NT97_10_A1D_D23_V04"

//#define _TRACK_VER_BASE_        "NT97_10_A1D_RM1_F1"
//#define _TRACK_VER_APP_         "NT97_10_A1D_RM1_V03_WM"

#elif defined (__GT380__)
#define _TRACK_VER_PARAMETER_   "R0"
#define _TRACK_VER_BASE_        "NT34_10_A1D_D23_F1"
#define _TRACK_VER_APP_         "NT34_10_A1D_D23_V02"
//#define _TRACK_VER_APP_         "NT34_10_RS485_TEST_TOOL"

#elif defined (__GT740__)
#define _TRACK_VER_PARAMETER_   "R0"
//#define _TRACK_VER_BASE_        "GT740_10_A1D_B4_F1"
//#define _TRACK_VER_APP_         "GT740_10_A1D_B4_V01"
//#define _TRACK_VER_APP_         "NT74_10_A1D_B4_V01"
//#define _TRACK_VER_APP_         "NT74_10_A1D_B4_V02"
//#define _TRACK_VER_APP_         "GT740_10_A1D_B25E_V01"
//#define _TRACK_VER_APP_         "NT74_10_A1D_B4_V03"
//#define _TRACK_VER_APP_         "NT74_10_A1D_D23_V04"
//#define _TRACK_VER_APP_         "NT74_10_A1D_D23_V05"
//#define _TRACK_VER_APP_         "NT74_10_A1D_V07"
//#define _TRACK_VER_APP_         "NT74_10_A1D_V08"

//#define _TRACK_VER_APP_         "NT74_10_A1D_V07_3C"
//记得修改NT76 NT77版本号 _TRACK_VER_APP2_          NT76_10_A1D_D23_V05 

#define _TRACK_VER_BASE_        "GT741_10_A1D_B4_F1"
#define _TRACK_VER_APP_         "GT741_10_A1D_V01"

#elif defined (__GT420D__)
#define _TRACK_VER_PARAMETER_   "R0"
#define _TRACK_VER_BASE_        "GT420D_51_61DM4_D23_F1"
//#define _TRACK_VER_APP_ 	    "GT411_51_61DM4_D23_V00_TEST"
//#define _TRACK_VER_APP_           "JM_L120_10_61DA1_D23_V02_XK"
#define _TRACK_VER_APP_           "JM_L120_10_61DA1_D23_V03_XK_PATCH"
#elif defined (__ET350__)
#define _TRACK_VER_PARAMETER_   "R0"
#define _TRACK_VER_BASE_        "NT15_10_A1D_D23_F1"
#define _TRACK_VER_APP_         "NT15_10_A1D_D23_V03_BCA"

#elif defined (__GT310__)
#define _TRACK_VER_PARAMETER_   "R0"
#define _TRACK_VER_BASE_        "NT61_10_A1D_B4_F1"
#define _TRACK_VER_APP_         "NT61_10_A1D_B4_V01"

#elif defined (__NT37__)
#define _TRACK_VER_PARAMETER_   "R0"
#define _TRACK_VER_BASE_        "NT37_10_A1D_SC1_F1"
#define _TRACK_VER_APP_         "NT37_10_A1D_SC1_V01_WM"

#elif defined (__JM81__)
#define _TRACK_VER_PARAMETER_   "R0"
#define _TRACK_VER_BASE_        "JM81_10_A1D_B4_F1"
#define _TRACK_VER_APP_         "JM81_10_A1D_B4_V01"

#elif defined (__JM66__)
#define _TRACK_VER_PARAMETER_   "R0"
#define _TRACK_VER_BASE_        "JM66_10_A1D_B4_F1"
#define _TRACK_VER_APP_         "JM66_10_A1D_B4_V01"

#elif defined (__HVT001__)
#define _TRACK_VER_PARAMETER_   "R0"
#define _TRACK_VER_BASE_        "HVT001_10_A1D_SC1_F1"
#define _TRACK_VER_APP_         "HVT001_10_A1D_SC1_V01"

#else
#error 
#endif

