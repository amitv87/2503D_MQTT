#ifndef _madpmenudef_h_
#define _madpmenudef_h_

#ifdef __MCARE_COMMON__
#include "MAdpConfig.h"
#ifdef _MCR_MMI_RESOURCE_XML_GEN_
//ע��:mmi_rp_tencent_mcare_def.h��xml��Դʱ�Զ����ɵ��ļ�
#include "mmi_rp_app_tencent_mcare_def.h"
#else
/*IMG_MCARE_xxx*/
typedef enum
{
	IMG_MCARE_MAIN = APP_TENCENT_MCARE_BASE + 1,
	IMG_MCARE_STAT_MCR,
	IMG_MCARE_QQ_LOGO,
	IMG_MCARE_STAT_QQ_ON,
	IMG_MCARE_STAT_QQ_OFF,
	IMG_MCARE_STAT_QQ_AWAY,
	IMG_MCARE_STAT_QQ_INVISBLE,
	IMG_MCARE_STAT_QQ_MESSEGE,
	IMG_MCARE_STAT_CONTACT_ON,
	
#ifdef __MCARE_QQBROWSER__
	IMG_MCARE_QQBROWSER_LOGO,
#endif
#ifdef __MCARE_QQFARM__
	IMG_MCARE_QQFARM_LOGO,
#endif
#ifdef __MCARE_QQPASTURE__
	IMG_MCARE_QQPASTURE_LOGO,
#endif
	IMG_MCARE_LAST
}IMG_ID_MCARE_ENUM;

/*STR_MCARE_xxx*/
typedef enum
{
	STR_MCARE_MAIN = APP_TENCENT_MCARE_BASE+1,
	STR_MCARE_QQ,
#ifdef __MCARE_QQBROWSER__
    STR_MCARE_QQBROWSER,
#endif
#ifdef __MCARE_QQFARM__
    STR_MCARE_QQFARM,
#endif
#ifdef __MCARE_QQPASTURE__
    STR_MCARE_QQPASTURE,
#endif
    STR_MCARE_RUNNING,
    STR_MCARE_QQ_RUNNING,
    STR_MCARE_INSERT_VALID_SIM,
    STR_MCARE_IS_ON_FLY_MODE,
    STR_MCARE_STORAGE_ERROR_NOTE,
    STR_MCARE_SIM_ERROR,
    STR_MCARE_NOT_ENOUGH_MEM,
    STR_MCARE_STORAGE_NO_SPACE,
    STR_MCARE_FILE_CORRUPT_RESTART,
    STR_MCARE_LOAD_FAIL,
    STR_MCARE_FILE_CORRUPT_ERR,
    STR_MCARE_EXEC_FAIL,
    STR_MCARE_NETWORK_NOT_READY,
    STR_MCARE_USB_MODE,
    STR_MCARE_LAST
}STR_ID_MCARE_ENUM;



/*MENU_ID_MCARE_xxx*/
enum 
{
    MENU_ID_MCARE_MAIN = MENU_MCARE_COMM_BASE,
#ifdef __MCARE_QQBROWSER__
    MENU_ID_MCARE_QQBROWSER,
#endif
#ifdef __MCARE_QQFARM__
    MENU_ID_MCARE_QQFARM,
#endif
#ifdef __MCARE_QQPASTURE__
    MENU_ID_MCARE_QQPASTURE,
#endif
    //max items
    MENU_ID_MCARE_MAX = MENU_MCARE_COMM_END
};


#endif//_MCR_MMI_RESOURCE_XML_GEN_

#ifndef DEVELOPER_BUILD_FIRST_PASS
enum 
{//  
    STATUS_ICON_MCARE = STATUS_ICON_MCARE_FIRST,
    STATUS_ICON_QQ_ONLINE,
    STATUS_ICON_QQ_OFFLINE,
    STATUS_ICON_QQ_AWAYLINE,
    STATUS_ICON_QQ_INVISBLE,
    STATUS_ICON_QQ_HAVEMSG,
    STATUS_ICON_CONTACT_ON,
    //����STATUS_ICON_RESERVED1/2/3��Ϊ����ʹ��
    STATUS_ICON_RESERVED1,
    STATUS_ICON_RESERVED2,
    STATUS_ICON_RESERVED3,

    
    /**********************************************/
    /*NOTE: DO NOT exceed STATUS_ICON_MCARE_LAST   */
    STATUS_ICON_INVALID = STATUS_ICON_MCARE_LAST +1
};
#endif
//����3.0�Ķ���
#if 1
/*_____compatible_30_defines_beg*/
#define MENU_ID_TENCENT_MCARE       MENU_ID_MCARE_MAIN
#ifdef __MCARE_QQBROWSER__
#define MENU_ID_TENCENT_BROWSER     MENU_ID_MCARE_QQBROWSER
#define MENU_ID_TENCENT_MCARE_BROWSER   MENU_ID_MCARE_QQBROWSER
#endif
#ifdef __MCARE_QQFARM__
#define MENU_ID_TENCENT_FARM        MENU_ID_MCARE_QQFARM
#endif
#ifdef __MCARE_QQPASTURE__
#define MENU_ID_TENCENT_PASTURE     MENU_ID_MCARE_QQPASTURE
#endif 
#define STR_TENCENT_QQBROWSER           STR_MCARE_QQBROWSER
#define STR_TENCENT_MCARE_Q             STR_MCARE_QQ
#define STR_TENCENT_MCARE_QQ_FARM       STR_MCARE_QQFARM
#define STR_TENCENT_MCARE_QQ_PASTURE    STR_MCARE_QQPASTURE
/*_____compatible_30_defines_end*/
#endif
#endif

#if ((MCR_MTK_VER >= MTK_VER_1112) && (defined(__COSMOS_MMI_PACKAGE__)))
typedef enum
{
    MCF_STATUS_ICON_MCARE = 1,
    MCF_STATUS_ICON_QQ_ONLINE,
    MCF_STATUS_ICON_QQ_OFFLINE,
    MCF_STATUS_ICON_QQ_AWAYLINE,
    MCF_STATUS_ICON_QQ_INVISBLE,
    MCF_STATUS_ICON_QQ_HAVEMSG,
    MCF_STATUS_ICON_TOTAL
}EMcrIconId;
#endif

#endif 