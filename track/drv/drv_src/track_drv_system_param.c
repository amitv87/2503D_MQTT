/******************************************************************************
 * track_system_param.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *      ϵͳ������ȡ�����
 *
 * modification history
 * --------------------
 * v1.0   2012-07-30,  chengjun create this file
 *
 ******************************************************************************/

/****************************************************************************
* Include Files             ����ͷ�ļ�
*****************************************************************************/
#include "track_drv_system_param.h"
#include "nvram_editor_data_item.h"
#include "track_os_ell.h"
#include "track_cust.h"
#include "..\..\ell\ell-common\ell_fota_memory_define.h"

/*****************************************************************************
 *  Define			    �궨��
*****************************************************************************/
#define SDS_PAGE_SIZE  4096

/*****************************************************************************
* Typedef  Enum         ö��
*****************************************************************************/

/*****************************************************************************
 *  Struct			    ���ݽṹ����
*****************************************************************************/
#if defined( __TRACK_DISK_NVRAM__)

typedef struct
{
    int         item;
    kal_uint32  address;
    int         size;
} sds_data_struct;

/*
ADDRESS	StructSize
389120	436           436/128=3 4*128=512   389120+512=389632
389632	516
390272	1
390400	360
393216	36

#if  0
0x103a0000      272236544
0x103f5000      272584704   389120
====            ADDRESS	StructSize====
IMPORTANCE:     272586798	564
PARAMETER:      272587444	644
REALTIME:       272588008	129
DRV_PARAMETER:  272588172	488
PHONE_NUMBER:   272588301	164
========
#endif
*/

const sds_data_struct sds[] =
{
    {NVRAM_EF_PARAMETER_LID,            IT0_PARAMETER_ADD,      NVRAM_EF_PARAMETER_SIZE},
    {NVRAM_EF_IMPORTANCE_PARAMETER_LID, IT1_IMPORTANCE_ADD,      NVRAM_EF_IMPORTANCE_PARAMETER_SIZE},
    {NVRAM_EF_PHONE_NUMBER_LID,         IT2_PHONE_NUMBER_ADD,   NVRAM_EF_PHONE_NUMBER_SIZE},
    {NVRAM_EF_IOT_PARAMS_LID,            IOT_IMPORT_PARAMS_ADD,      NVRAM_EF_IOT_PARAMS_SIZE},
    {IMPORT_PARAM_ORG_LID,              0,                          NVRAM_EF_IMPORTANCE_PARAMETER_SIZE},
    {DISK1_LAST_LID,                    DISK1_LAST_SECTOR_ADD,  NVRAM_EF_IMPORTANCE_PARAMETER_SIZE},//notice change
    {DISK1_BACKUP_LID,                  DISK1_BACKUP_ADD,  NVRAM_EF_DRV__IMP_BACKUP_SIZE}//notice change

};


static void toString(void)
{
    kal_uint32 def_para = (kal_uint32)OTA_NVRAM_EF_PARAMETER_DEFAULT - DISK1_START_ADD;
    kal_uint32 def_imp = (kal_uint32)OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT - DISK1_START_ADD;
    kal_uint32 def_pho = (kal_uint32)OTA_NVRAM_EF_PHONE_NUMBER_DEFAULT - DISK1_START_ADD;

    LOGS("====ADDRESS\t\tStructSize====");
    LOGS("%x\t%d\t%d", IT0_PARAMETER_ADD,    IT0_PARAMETER_ADD,  NVRAM_EF_PARAMETER_SIZE);
    LOGS("%x\t%d\t%d", IT1_IMPORTANCE_ADD,   IT1_IMPORTANCE_ADD,   NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
    LOGS("%x\t%d\t%d", IT2_PHONE_NUMBER_ADD, IT2_PHONE_NUMBER_ADD, NVRAM_EF_PHONE_NUMBER_SIZE);
    LOGS("%x\t%d\t%d", IOT_IMPORT_PARAMS_ADD,    IOT_IMPORT_PARAMS_ADD,  NVRAM_EF_IOT_PARAMS_SIZE);
    
    LOGS("SDS ADD:%x\t%d\t%d", def_imp,     def_imp,   NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
    LOGS("SDS ADD:%x\t%d\t%d", def_para,     def_para,  NVRAM_EF_PARAMETER_SIZE);
    LOGS("SDS ADD:%x\t%d\t%d", def_pho,     def_pho, NVRAM_EF_PHONE_NUMBER_SIZE);
    LOGS("==========================");
}
#if 0
== == ADDRESS	StructSize == ==
272588800	584, 272320593
272589440	644, 272320605
272590160	488, 272320617
272590480	164, 272320629
== == == == == == == == == == == == ==
== == ADDRESS	StructSize == ==
PAR: 23b37c	584	2339708
IMP: 23b384	644	2339716
PHO: 23b39c	488	2339740
REA: 23b388	164	2339720
NEED_BUQI: 1984
== == == == == == == == == == == == ==
10: 27: 49.212 > == == ADDRESS		StructSize == ==
10: 27: 49.212 > 58000	360448	740 //0x103A0000+0x58000 =0x103f8000   //88
10: 27: 49.212 > 58400	361472	700
10: 27: 49.212 > 58800	362496	488
10: 27: 49.212 > 58a80	363136	284
10: 27: 49.291 > == == == == == == == == == == == == ==
20150411
10: 27: 30.545 > == == ADDRESS		StructSize == ==
10: 27: 30.545 > PAR: 103f5328	272585512	740     0x103f5328 - 0x103A0000 = 0x55328 //85
        10: 27: 30.545 > IMP: 103f506a	272584810	700     0x103f506a - 0x103A0000 = 0x5506B //0x103f5150
                10: 27: 30.545 > PHO: 103f560c	272586252	488
                10: 27: 30.545 > REA: 103f57f4	272586740	284     0x103f57f4 -  0x103A0000 = 0x557f4 //85
                        10: 27: 30.545 > NEED_BUQI: 1984 //56000 =86*4096
                        10: 27: 30.545 > == == == == == == == == == == == == ==
                        9: 59: 01.137 > == == ADDRESS		StructSize == ==
                        9: 59: 01.137 > 58000	360448	740
                        9: 59: 01.137 > 58400	361472	700
                        9: 59: 01.137 > 58800	362496	488
                        9: 59: 01.137 > 57400	357376	700
                        9: 59: 01.137 > 103f7400	272593920	700
                        9: 59: 01.137 > == == == == == == == == == == == == ==
                        9: 59: 01.137 > == == ADDRESS		StructSize == ==
                        9: 59: 01.137 > PAR: 103f540c	272585740	740
                        9: 59: 01.137 > IMP: 103f5150	272585040	700
                        9: 59: 01.137 > PHO: 103f56f0	272586480	488
                        9: 59: 01.137 > REA: 103f58d8	272586968	288
                        9: 59: 01.137 > NEED_BUQI: 2060
                        10: 27: 30.545 > == == == == == == == == == == == == ==
#endif
                        static void toStrings(void)
{
    LOGS("====ADDRESS\t\tStructSize====");
    LOGS("PAR:%x\t%d\t%d", (kal_uint32)OTA_NVRAM_EF_PARAMETER_DEFAULT, OTA_NVRAM_EF_PARAMETER_DEFAULT, NVRAM_EF_PARAMETER_SIZE);
    LOGS("IMP:%x\t%d\t%d", (kal_uint32)OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT, (kal_uint32)OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
    LOGS("PHO:%x\t%d\t%d", (kal_uint32)OTA_NVRAM_EF_PHONE_NUMBER_DEFAULT, (kal_uint32)OTA_NVRAM_EF_PHONE_NUMBER_DEFAULT, NVRAM_EF_PHONE_NUMBER_SIZE);
    LOGS("REA:%x\t%d\t%d", (kal_uint32)OTA_NVRAM_EF_REALTIME_DEFAULT, (kal_uint32)OTA_NVRAM_EF_REALTIME_DEFAULT,   NVRAM_EF_REALTIME_SIZE);
    //LOGS("NEED_BUQI:%d", NEED_BUQI);
    LOGS("==========================");
}


void track_drv_importance_paramter_toStrings(void)
{
    //int iptr[] = {IMPORT_PARAM_ORG, IMPORT_PARAM_SUT, IMPORT_USER_SUT};
    nvram_importance_parameter_struct *pOrg =  OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT;
    LOGS("====================================================");
#if defined(__BACKUP_SERVER__)
    LOGS("ORGI:data[%s %s],version[%s:%s],server[type:%d,url:%s,ip:%d.%d.%d.%d,port:%d],server2[port:%d]\r\n",
         pOrg->build_date, pOrg->build_time, pOrg->version_string, pOrg->version_string2, pOrg->server.conecttype,
         track_domain_decode(pOrg->server.url), pOrg->server.server_ip[0], pOrg->server.server_ip[1], pOrg->server.server_ip[2],
         pOrg->server.server_ip[3], pOrg->server.server_port,  pOrg->server2.server_port);
#else
	LOGS("ORGI:data[%s %s],version[%s:%s],server[type:%d,url:%s,ip:%d.%d.%d.%d,port:%d],server2[port:%d],dserver:[port:%d]\r\n",
         pOrg->build_date, pOrg->build_time, pOrg->version_string, pOrg->version_string2, pOrg->server.conecttype,
         track_domain_decode(pOrg->server.url), pOrg->server.server_ip[0], pOrg->server.server_ip[1], pOrg->server.server_ip[2],
         pOrg->server.server_ip[3], pOrg->server.server_port,  pOrg->server2.server_port, pOrg->dserver.server_port);
#endif
    pOrg = (nvram_importance_parameter_struct *)IMPORT_USER_SUT;
#if defined(__BACKUP_SERVER__)
    LOGS("USER:data[%s %s],version[%s:%s],server[type:%d,url:%s,ip:%d.%d.%d.%d,port:%d],server2[port:%d]\r\n",
         pOrg->build_date, pOrg->build_time, pOrg->version_string, pOrg->version_string2, pOrg->server.conecttype,
         track_domain_decode(pOrg->server.url), pOrg->server.server_ip[0], pOrg->server.server_ip[1], pOrg->server.server_ip[2],
         pOrg->server.server_ip[3], pOrg->server.server_port,  pOrg->server2.server_port);
#else
    LOGS("USER:data[%s %s],version[%s:%s],server[type:%d,url:%s,ip:%d.%d.%d.%d,port:%d],server2[port:%d],dserver:[port:%d]\r\n",
         pOrg->build_date, pOrg->build_time, pOrg->version_string, pOrg->version_string2, pOrg->server.conecttype,
         track_domain_decode(pOrg->server.url), pOrg->server.server_ip[0], pOrg->server.server_ip[1], pOrg->server.server_ip[2],
         pOrg->server.server_ip[3], pOrg->server.server_port,  pOrg->server2.server_port, pOrg->dserver.server_port);
#endif
    pOrg = (nvram_importance_parameter_struct *)&G_importance_parameter_data;
#if defined(__BACKUP_SERVER__)
    LOGS("MEMORY:data[%s %s],version[%s:%s],server[type:%d,url:%s,ip:%d.%d.%d.%d,port:%d],server2[port:%d]\r\n",
         pOrg->build_date, pOrg->build_time, pOrg->version_string, pOrg->version_string2, pOrg->server.conecttype,
         track_domain_decode(pOrg->server.url), pOrg->server.server_ip[0], pOrg->server.server_ip[1], pOrg->server.server_ip[2],
         pOrg->server.server_ip[3], pOrg->server.server_port,  pOrg->server2.server_port);
#else
    LOGS("MEMORY:data[%s %s],version[%s:%s],server[type:%d,url:%s,ip:%d.%d.%d.%d,port:%d],server2[port:%d],dserver:[port:%d]\r\n",
         pOrg->build_date, pOrg->build_time, pOrg->version_string, pOrg->version_string2, pOrg->server.conecttype,
         track_domain_decode(pOrg->server.url), pOrg->server.server_ip[0], pOrg->server.server_ip[1], pOrg->server.server_ip[2],
         pOrg->server.server_ip[3], pOrg->server.server_port,  pOrg->server2.server_port, pOrg->dserver.server_port);
#endif
    LOGS("====================================================");

}

void track_drv_Disk1_last_sector_toStrings(void)
{
    //int iptr[] = {IMPORT_PARAM_ORG, IMPORT_PARAM_SUT, IMPORT_USER_SUT};
    nvram_importance_parameter_struct last =  {0};
    Track_nvram_read(DISK1_LAST_LID, 1, &last, sds[DISK1_LAST_LID].size);

    LOGS("====================================================");
#if defined(__BACKUP_SERVER__)
	LOGS("LAST:data[%s %s],version[%s:%s],server[type:%d,url:%s,ip:%d.%d.%d.%d,port:%d],server2[port:%d]\r\n",
         last.build_date, last.build_time, last.version_string, last.version_string2, last.server.conecttype,
         track_domain_decode(last.server.url), last.server.server_ip[0], last.server.server_ip[1], last.server.server_ip[2],
         last.server.server_ip[3], last.server.server_port,  last.server2.server_port);
#else
    LOGS("LAST:data[%s %s],version[%s:%s],server[type:%d,url:%s,ip:%d.%d.%d.%d,port:%d],server2[port:%d],dserver:[port:%d]\r\n",
         last.build_date, last.build_time, last.version_string, last.version_string2, last.server.conecttype,
         track_domain_decode(last.server.url), last.server.server_ip[0], last.server.server_ip[1], last.server.server_ip[2],
         last.server.server_ip[3], last.server.server_port,  last.server2.server_port, last.dserver.server_port);
#endif
    LOGS("====================================================");
}
#endif /* __TRACK_DISK_NVRAM__ */

/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/
extern void track_os_importance_backup_read(void);
extern void track_os_importance_backup_write(void);

/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/

/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/

/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/



/*****************************************************************************
* Local variable            �ֲ�����
*****************************************************************************/
static kal_uint8 imei_str[16] = {0};
static kal_uint8 imei_hex[8] = {0};

static kal_uint8 imsi_str[16] = {0};
static kal_uint8 imsi_hex[8] = {0};

static kal_uint8 chip_rid[16] = {0};
/*����оƬ�������ֵRID������Ψһ��*/

static kal_bool writeValid = KAL_TRUE;
static kal_bool notWrite = KAL_TRUE;

/****************************************************************************
* Global Variable           ȫ�ֱ���
*****************************************************************************/


/****************************************************************************
* Global Variable - Extern          ����ȫ�ֱ���
*****************************************************************************/


/*****************************************************************************
 *  Global Functions	- Extern	    �����ⲿ����
*****************************************************************************/


/*****************************************************************************
 *  Local Functions	            ���غ���
*****************************************************************************/
static kal_uint32 get_import_dataIndex(void)
{
    return (kal_uint32)OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT - DISK1_START_ADD;
}

nvram_importance_parameter_struct * track_drv_get_importPara_default(void)
{
    return (nvram_importance_parameter_struct*)OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT;
}

static int operate_back_file(kal_uint8 type, void* data)
{
    kal_uint32   file_operate_size;
    int fs_seek = 0, fs_write = 0, fs_read = 0, file_hd;
    WCHAR       backup_file_name[32];

    kal_wsprintf(backup_file_name, "%w", L"DEFAULT");
    file_hd = FS_Open(backup_file_name , FS_CREATE | FS_READ_WRITE);
    if(file_hd < 0)
    {
        LOGD(L_DRV, L_V5, "Open file Error %d ", file_hd);
        return file_hd;
    }
    FS_GetFileSize(file_hd, &file_operate_size);

    LOGD(L_DRV, L_V5, "type:%d,log_file_int:%d,file_size:%d ", type, file_hd, file_operate_size);

    fs_seek = FS_Seek(file_hd, 0, FS_FILE_BEGIN);
    if(fs_seek < FS_NO_ERROR)
    {
        FS_Close(file_hd);
        LOGD(L_DRV, L_V5, "FS_Seek %d", fs_seek);
        return fs_seek;
    }

    if(1 == type)
    {
        fs_write = FS_Write(file_hd, (void *)data, SDS_PAGE_SIZE , &file_operate_size);
        if(fs_write < FS_NO_ERROR)
        {
            FS_Close(fs_write);
            fs_seek = FS_Delete(backup_file_name);
            LOGD(L_DRV, L_V5, "ret fs_write:%d FS_Delete=%d", fs_write, fs_seek);
            return fs_write;
        }
        FS_Commit(file_hd);
    }
    else
    {
        memset(data, 0, SDS_PAGE_SIZE);
        fs_read = FS_Read(file_hd, data, SDS_PAGE_SIZE, &file_operate_size);
        if(fs_read < FS_NO_ERROR)
        {
            FS_Close(file_hd);
            LOGD(L_DRV, L_V5, "can read:%d", file_operate_size);
            return fs_read;
        }
    }

    LOGD(L_DRV, L_V5, "%s File Sucesses:%d", (type == 1 ? "WRITE" : "READ"), file_operate_size);

    FS_Close(file_hd);
    return file_operate_size;
}

/******************************************************************************
 *  Function    -  track_read_imei_data
 *
 *  Purpose     -  ��ȡ IMEI
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 21-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_read_imei_data(void)
{
    nvram_ef_imei_imeisv_struct imei_data = {0};
    kal_uint8 str[17] = {0};
    kal_uint8 tmp_str[17] = {0}, tmp_hex[9] = {0};
    kal_uint8 i, j;
    kal_uint8 imei_s;
#if defined(__GT530__)
    tr_stop_timer(TRACK_CUST_GET_IMEI_TIMER_ID);
    track_get_imei_hex(&imei_s);
    snprintf(imei_str, 16, "%s", &imei_s);
    memcpy(&tmp_str[1], &imei_str, 16);
    LOGH(L_DRV, L_V5, tmp_str, 17);
    for(i = 0, j = 0; i < 8; i++)
    {
        tmp_hex[i] = (((tmp_str[j] & 0x0F) << 4)) | ((tmp_str[j+1]) & 0x0F);
        j += 2;
    }
    memset(imei_hex, 0x00, sizeof(imei_hex));
    memcpy(imei_hex, tmp_hex, sizeof(imei_hex));
    LOGD(L_DRV, L_V5, "IMEI=%s", imei_str);
    return;
#endif
    Track_nvram_read(NVRAM_EF_IMEI_IMEISV_LID, 1, (void *)&imei_data, NVRAM_EF_IMEI_IMEISV_SIZE);

    for(i = 0, j = 0; i < 8; i++)
    {
        str[j++] = (imei_data.imei[i] & 0x0F) + 0x30;
        str[j++] = ((imei_data.imei[i] >> 4) & 0x0F) + 0x30;
    }
    str[j - 1] = 0; //���һλ0  ����

    memset(imei_str, 0x00, sizeof(imei_str));
    snprintf(imei_str, 16, "%s", str);

    tmp_str[0] = '0';
    strncpy(&tmp_str[1], str, sizeof(tmp_str) - 2);

    for(i = 0, j = 0; i < 16;)
    {
        tmp_hex[j++] = (((tmp_str[i] - 0x30) & 0x0F) << 4) | ((tmp_str[i + 1] - 0x30) & 0x0F);
        i += 2;
    }
    memset(imei_hex, 0x00, sizeof(imei_hex));
    memcpy(imei_hex, tmp_hex, sizeof(imei_hex));

    LOGD(L_DRV, L_V5, "IMEI=%s", imei_str);

}

/******************************************************************************
 *  Function    -  track_read_imsi_data
 *
 *  Purpose     -  ֱ�Ӵ�L4C ��ȡIMSI
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 21-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_read_imsi_data(void)
{
    extern void track_get_imsi_hex(kal_uint8 * imsi_data);
    kal_uint8 imsi_data[10] = {0};
    kal_uint8 str[17] = {0};
    kal_uint8 i, j;
    kal_uint8 imsi_s;

#if defined(__GT530__)
    track_get_imsi_hex(&imsi_s);
    snprintf(imsi_str, 15, "%s", &imsi_s);
#else
    l4csmu_get_imsi(imsi_data);

    /*imsi_data ȫ0 ��ʾû��SIM ��*/

    for(i = 0, j = 0; i < 8; i++)
    {
        str[j++] = (imsi_data[i] & 0x0F) + 0x30;
        str[j++] = ((imsi_data[i] >> 4) & 0x0F) + 0x30;
    }
    str[j] = 0;
    memset(imsi_str, 0x00, sizeof(imsi_str));
    snprintf(imsi_str, 16, "%s", &str[1]);   //��λ9 ��֪����ʲô

    memset(str, 0x00, sizeof(str));
    for(i = 0, j = 0; i < 8; i++)
    {
        str[j++] = ((imsi_data[i] & 0x0F) << 4) | ((imsi_data[i] >> 4) & 0x0F);
    }
    str[j] = 0;
    str[0] = str[0] & 0x0F;
    memset(imsi_hex, 0x00, sizeof(imsi_hex));
    memcpy(imsi_hex, str, sizeof(imsi_hex));
#endif
    LOGD(L_DRV, L_V5, "IMSI=%s", imsi_str);

}


/******************************************************************************
 *  Function    -  track_read_rid_data
 *
 *  Purpose     -  ������оƬRID
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 21-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_read_rid_data(void)
{
    memset(chip_rid, 0x00, sizeof(chip_rid));
    SST_Get_ChipRID(chip_rid, 128);
}

#if defined(__TEST_DEVICE__)
kal_bool track_rid_given(void)
{
    kal_uint8 data[16] =
    {
        //0xC1, 0x26, 0xBF, 0x05, 0x1E, 0x43, 0x8B, 0x1B, 0x4D, 0x48, 0xA4, 0xE7, 0xFD, 0x55, 0xAE, 0x59
        0xB6, 0xE0, 0x90, 0x50, 0x87, 0x7C, 0x4A, 0x77, 0x75, 0x62, 0x0C, 0xCF, 0xBF, 0xBA, 0xEF, 0xAB
    };
    if(chip_rid[0] == 0 && chip_rid[1] == 0)
    {
        track_read_rid_data();
    }
    LOGH(L_APP, L_V7, chip_rid, 16);
    if(!memcmp(chip_rid, data, 16))
    {
        return KAL_TRUE;
    }
    return KAL_FALSE;
}
#endif /* __TEST_DEVICE__ */
#if 1
/*****************************************************************************
 *  Global Functions            ȫ�ֺ���
*****************************************************************************/
#endif

/******************************************************************************
 *  Function    -  track_drv_get_system_param
 *
 *  Purpose     -  ��ȡϵͳ����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 21-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_get_system_param(void)
{
    track_read_imei_data();
    track_read_rid_data();
}

/******************************************************************************
 *  Function    -  track_drv_get_imei
 *
 *  Purpose     -  ��ȡ IMEI
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 21-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint8* track_drv_get_imei(kal_uint8 flag)
{
    if(flag == 0)
    {
        return imei_str;
    }
    else
    {
        return imei_hex;
    }
}

/******************************************************************************
 *  Function    -  track_drv_get_imsi
 *
 *  Purpose     -  ��ȡIMSI
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 21-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint8* track_drv_get_imsi(kal_uint8 flag)
{
    if(flag == 0)
    {
        return imsi_str;
    }
    else
    {
        return imsi_hex;
    }
}


/******************************************************************************
 *  Function    -  track_drv_get_chip_id
 *
 *  Purpose     -  ��ȡоƬID
 *
 *  Description -  RID ����Ψһ��
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 21-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint8* track_drv_get_chip_id(void)
{
    return chip_rid;
}

/******************************************************************************
 *  Function    -  track_drv_rtc_to_gmt0
 *
 *  Purpose     -  ʱ��������ת��
 *
 *  Description -  ����ʱ����RTC ��ת�ó�GTM0 ��ʱ��
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 20-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_rtc_to_gmt0(applib_time_struct *out_dt, applib_time_struct *in_dt, nvram_gps_time_zone_struct timezone)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    applib_time_struct diff_time = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    diff_time.nDay = (U8)(timezone.time / 24);
    diff_time.nHour = ((U8)timezone.time) % 24;
    diff_time.nMin = timezone.time_min % 60;

    if(timezone.zone == 'E')
    {
        applib_dt_decrease_time(in_dt, &diff_time, out_dt);
    }
    else
    {
        applib_dt_increase_time(in_dt, &diff_time, out_dt);
    }
}


/******************************************************************************
 *  Function    -  track_drv_utc_timezone_to_rtc
 *
 *  Purpose     -  ʱ��У׼
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 20-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_utc_timezone_to_rtc(applib_time_struct *out_dt, applib_time_struct *in_dt, nvram_gps_time_zone_struct timezone)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    applib_time_struct diff_time = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    diff_time.nDay = (U8)(timezone.time / 24);
    diff_time.nHour = ((U8)timezone.time) % 24;
    diff_time.nMin = timezone.time_min % 60;

    if(timezone.zone == 'W')
    {
        applib_dt_decrease_time(in_dt, &diff_time, out_dt);
        // ���ο�ʱ���С����ȥ
    }
    else
    {
        applib_dt_increase_time(in_dt, &diff_time, out_dt);
        //���ο�ʱ�����ӵ�δ��
    }
}

/******************************************************************************
 *  Function    -  track_drv_update_time_to_system
 *
 *  Purpose     -  ����ϵͳʱ��
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 20-08-2012,  chengjun  written
 * ----------------------------------------
 * ��ȡRTC ʱ��2 �ַ���
 *    RTC_GetTimeOnly  ( ע��rtc_year+2000)
 *    applib_dt_get_rtc_time
 *
 ******************************************************************************/
void track_drv_update_time_to_system(applib_time_struct utcTime)
{
    t_rtc rtcTime = {0};

    rtcTime.rtc_year = utcTime.nYear - 2000;
    rtcTime.rtc_mon = utcTime.nMonth;
    rtcTime.rtc_day = utcTime.nDay;
    rtcTime.rtc_hour = utcTime.nHour;
    rtcTime.rtc_min = utcTime.nMin;
    rtcTime.rtc_sec = utcTime.nSec;

    RTC_InitTC_Time(&rtcTime);
}
#if 0
/*��ֻ�������жϴ�ṹ��������Ƿ�Ե�,���ԵĻ�����ȡ��˭�����������д���ʱ�򣬶���ȫ����д������*/
void track_drv_disk1_check_crc(kal_uint8* data,kal_uint32 crc_begin)
{
    kal_uint32 ret = 99;
    static char backup_buf[SDS_PAGE_SIZE] = {0};
    kal_uint16 crc_page = 0xFF,crc_backup = 0xFF;
    ret = OTA_readRawDiskData(&OTA_FlashDiskDriveData[1], DISK1_ASSI_BACKUP_ADD, backup_buf, SDS_PAGE_SIZE);
    crc_page = GetCrc16(data,crc_begin);
    crc_backup = GetCrc16(backup_buf,crc_begin);
    if(data[crc_begin] == backup_buf[crc_begin] && data[crc_begin+1] ==backup_buf[crc_begin+1])//�뱸����У��
    {
        if(data[crc_begin] == 0xFF && data[crc_begin+1]==0xFF)//������ҲûУ��ֵ,�ն����״ο�����
        {
            data[crc_begin] = (crc_page >> 8) & 0x00FF;
            data[crc_begin+1] = crc_page & 0x00FF;
        }
        else
        {
            if(data[crc_begin] !=((crc_page >> 8) & 0x00FF) || data[crc_begin+1]!=(crc_page & 0x00FF))//������У�鲻��
            {
                if(backup_buf[crc_begin]!=((crc_backup >> 8) & 0x00FF) || backup_buf[crc_begin+1]!=(crc_backup & 0x00FF))//������У�鲻��
                {
                    memcpy(&G_parameter, (void*)OTA_NVRAM_EF_PARAMETER_DEFAULT, NVRAM_EF_PARAMETER_SIZE);
                    memcpy(&G_importance_parameter_data, (void*)OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
                    memcpy(&G_phone_number, (void*)OTA_NVRAM_EF_PHONE_NUMBER_DEFAULT, NVRAM_EF_PHONE_NUMBER_SIZE);

                    memcpy(&data[IT0_PARAMETER_ADD - DISK1_ASSI_START_ADD], &G_parameter, sds[NVRAM_EF_PARAMETER_LID].size);
                    memcpy(&data[IT1_IMPORTANCE_ADD - DISK1_ASSI_START_ADD], &G_importance_parameter_data, sds[NVRAM_EF_IMPORTANCE_PARAMETER_LID].size);
                    memcpy(&data[IT2_PHONE_NUMBER_ADD - DISK1_ASSI_START_ADD], &G_phone_number, sds[NVRAM_EF_PHONE_NUMBER_LID].size);
                    crc_page = GetCrc16(data,crc_begin);
                    data[crc_begin] = (crc_page >> 8) & 0x00FF;
                    data[crc_begin+1] = crc_page & 0x00FF;        
                }
                else
                {
                    LOGD(L_DRV, L_V1, "copy for backup_buf,data_crc==backup_buf");
                    memcpy(data, backup_buf, SDS_PAGE_SIZE);
                }
            }
            else
            {
                LOGD(L_DRV, L_V1, "readRawDiskData and backup data all is ok!");
            }
        }
    }
    else
    {
        if(data[crc_begin] !=((crc_page >> 8) & 0x00FF) || data[crc_begin+1]!=(crc_page & 0x00FF))//������У�鲻��
        {
            if(backup_buf[crc_begin]!=((crc_backup >> 8) & 0x00FF) || backup_buf[crc_begin+1]!=(crc_backup & 0x00FF))//������У�鲻��
            {
                memcpy(&G_parameter, (void*)OTA_NVRAM_EF_PARAMETER_DEFAULT, NVRAM_EF_PARAMETER_SIZE);
                memcpy(&G_importance_parameter_data, (void*)OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
                memcpy(&G_phone_number, (void*)OTA_NVRAM_EF_PHONE_NUMBER_DEFAULT, NVRAM_EF_PHONE_NUMBER_SIZE);

                memcpy(&data[IT0_PARAMETER_ADD - DISK1_ASSI_START_ADD], &G_parameter, sds[NVRAM_EF_PARAMETER_LID].size);
                memcpy(&data[IT1_IMPORTANCE_ADD - DISK1_ASSI_START_ADD], &G_importance_parameter_data, sds[NVRAM_EF_IMPORTANCE_PARAMETER_LID].size);
                memcpy(&data[IT2_PHONE_NUMBER_ADD - DISK1_ASSI_START_ADD], &G_phone_number, sds[NVRAM_EF_PHONE_NUMBER_LID].size);
                crc_page = GetCrc16(data,crc_begin);
                data[crc_begin] = (crc_page >> 8) & 0x00FF;
                data[crc_begin+1] = crc_page & 0x00FF; 
            }
            else
            {
                LOGD(L_DRV, L_V1, "copy for backup_buf,data_crc==backup_buf");
                memcpy(data, backup_buf, SDS_PAGE_SIZE);
            }
        }
        else
        {
            LOGD(L_DRV, L_V1, "readRawDiskData is ok and backup data maybe not good,but who care?");
        }

    }
}
/*�ҽ��ڴ�д���������ͱ����������ݡ�*/
kal_uint32 track_drv_write_disk1_to_flash(kal_uint32 data_page,kal_uint32 page_start_index,kal_uint8 *data,kal_uint16 crc,kal_uint32 crc_len)
{
    static char data_buf[SDS_PAGE_SIZE] = {0};
    static char temp_buf[SDS_PAGE_SIZE] = {0};
    kal_uint32 ret =99,ret0 =99,i=0,backup_page=0;
    kal_uint16 crc_temp = 0xFF;

    backup_page =  DISK1_ASSI_BACKUP_ADD / (SDS_PAGE_SIZE);
        
    memset(data_buf, 0, SDS_PAGE_SIZE);
    memset(temp_buf, 0, SDS_PAGE_SIZE);
    
    memcpy(data_buf, data, SDS_PAGE_SIZE); 
    //������д��
    do{
        ret0 = OTA_eraseRawDiskBlock(&OTA_FlashDiskDriveData[1], data_page);
        ret = OTA_writeRawDiskData(&OTA_FlashDiskDriveData[1], page_start_index, data_buf, SDS_PAGE_SIZE);
        OTA_readRawDiskData(&OTA_FlashDiskDriveData[1], page_start_index, temp_buf, SDS_PAGE_SIZE);
        crc_temp = GetCrc16(temp_buf,crc_len);
        i++;
    }while((crc != crc_temp) && i<3);

    memset(temp_buf, 0, SDS_PAGE_SIZE);
    i=0;

    //������д��   
    do{
        ret0 = OTA_eraseRawDiskBlock(&OTA_FlashDiskDriveData[1], backup_page);
        ret = OTA_writeRawDiskData(&OTA_FlashDiskDriveData[1], DISK1_ASSI_BACKUP_ADD, data_buf, SDS_PAGE_SIZE);
        OTA_readRawDiskData(&OTA_FlashDiskDriveData[1], DISK1_ASSI_BACKUP_ADD, temp_buf, SDS_PAGE_SIZE);
        crc_temp = GetCrc16(temp_buf,crc_len);
        i++;
    }while((crc != crc_temp) && i<3);
    LOGD(L_DRV, L_V6, "erase:%d, write ret:%d, page_start:%x, page:%d\r\n", ret0, ret, page_start_index, data_page);   
    return ret;
}
#endif
kal_bool track_drv_update_disk1(kal_uint8 type, kal_uint8 *data, kal_uint32 data_index, kal_uint32 len, kal_uint8 *md5)
{
    kal_uint32 data_page = 0, ret = 99, ret0 = 99, page_start_index = 0;
    int fs_ret = 0;
    static char page_buf[SDS_PAGE_SIZE] = {0};

    if(1 == type)
    {
        data_index = DISK1_ASSI_START_ADD;
    }
    data_page = data_index / (SDS_PAGE_SIZE);
    page_start_index = data_page * SDS_PAGE_SIZE;

    memset(page_buf, 0, SDS_PAGE_SIZE);

    ret = OTA_readRawDiskData(&OTA_FlashDiskDriveData[1], page_start_index, page_buf, SDS_PAGE_SIZE);
    
    LOGD(L_DRV, L_V6, "type:%d,read ret:%d, page_start:%x, data_index:%x, page:%d",
         type, ret, page_start_index, data_index, data_page);

#if defined( __TRACK_DISK_NVRAM__)
    if(type == 1)
    {
        ret = sds[NVRAM_EF_PHONE_NUMBER_LID].address - page_start_index + sds[NVRAM_EF_PHONE_NUMBER_LID].size;
        if(ret > SDS_PAGE_SIZE)
        {
            LOGD(L_DRV, L_V5, "1Out of Rang 4K:%d", ret);
        }

        memcpy(&G_parameter, (void*)OTA_NVRAM_EF_PARAMETER_DEFAULT, NVRAM_EF_PARAMETER_SIZE);
        memcpy(&G_importance_parameter_data, (void*)OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        memcpy(&G_phone_number, (void*)OTA_NVRAM_EF_PHONE_NUMBER_DEFAULT, NVRAM_EF_PHONE_NUMBER_SIZE);

        memcpy(&page_buf[IT0_PARAMETER_ADD - page_start_index], &G_parameter, sds[NVRAM_EF_PARAMETER_LID].size);
        memcpy(&page_buf[IT1_IMPORTANCE_ADD - page_start_index], &G_importance_parameter_data, sds[NVRAM_EF_IMPORTANCE_PARAMETER_LID].size);
        //memcpy(&page_buf[IT2_PHONE_NUMBER_ADD - page_start_index], &G_phone_number, sds[NVRAM_EF_PHONE_NUMBER_LID].size);
        ret0 = OTA_eraseRawDiskBlock(&OTA_FlashDiskDriveData[1], data_page);

    }
    else if(2 == type)
    {
        //ԭʼ����д��ʧ���ļ��ָ�
        fs_ret = operate_back_file(0, &page_buf);
        if(fs_ret >= 0)
        {
            ret0 = OTA_eraseRawDiskBlock(&OTA_FlashDiskDriveData[1], data_page);
        }
        else
        {
            LOGS("Serious  Error the default value lose!!!!!!!%d", ret);
        }

    }
    else if(type == 0 || type == 3)/*type = 3 test backup file*/
    {
        LOGD(L_DRV, L_V5, "data_index=%x,page_start_index=%x,%x",
             data_index, page_start_index, IT1_IMPORTANCE_ADD);

        memcpy(&page_buf[data_index - page_start_index], data, len);
        if(data_index == get_import_dataIndex() && type == 0)
        {
            //д�ļ�����
            operate_back_file(1, (void*)page_buf);
        }

        ret0 = OTA_eraseRawDiskBlock(&OTA_FlashDiskDriveData[1], data_page);

    }
#endif
    ret = OTA_writeRawDiskData(&OTA_FlashDiskDriveData[1], page_start_index, page_buf, SDS_PAGE_SIZE);
    LOGD(L_DRV, L_V6, "erase:%d, write ret:%d, page_start:%x, page:%d\r\n", ret0, ret, page_start_index, data_page);

    return 1;
}

#if 0
kal_bool track_drv_update_disk2(kal_uint8 type, kal_uint8 *data, kal_uint32 data_index, kal_uint32 len, kal_uint8 *md5)
{
		kal_uint32 data_page = 0, ret = 99, ret0 = 99, page_start_index = 0;
		int fs_ret = 0;
		static char page_buf[SDS_PAGE_SIZE] = {0};
	
		if(1 == type)
		{
			data_index = IT1_IMPORTANCE_ADD;
		}
		data_page = data_index / (SDS_PAGE_SIZE);
		page_start_index = data_page * SDS_PAGE_SIZE;
	
		memset(page_buf, 0, SDS_PAGE_SIZE);
	
		ret = OTA_readRawDiskData(&OTA_FlashDiskDriveData[1], page_start_index, page_buf, SDS_PAGE_SIZE);
		LOGD(L_DRV, L_V6, "��ԭ��4K���ݣ�type:%d,read ret:%d, page_start:%x, data_index:%x, page:%d",
			 type, ret, page_start_index, data_index, data_page);
	
#if defined( __TRACK_DISK_NVRAM__)
		if(type == 1)
		{
			memcpy(&G_importance_parameter_data, (void*)OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
			memcpy(&page_buf[IT1_IMPORTANCE_ADD - page_start_index], &G_importance_parameter_data, sds[NVRAM_EF_IMPORTANCE_PARAMETER_LID].size);
			ret0 = OTA_eraseRawDiskBlock(&OTA_FlashDiskDriveData[1], data_page);
		}
#endif
		ret = OTA_writeRawDiskData(&OTA_FlashDiskDriveData[1], page_start_index, page_buf, SDS_PAGE_SIZE);
		LOGD(L_DRV, L_V6, "erase:%d, write ret:%d, page_start:%x, page:%d\r\n", ret0, ret, page_start_index, data_page);
	
		return 1;

}
#endif

#if defined( __TRACK_DISK_NVRAM__)
kal_bool track_drv_check_disk1_nvram(kal_uint8 type)
{
    kal_bool result = KAL_TRUE;
    kal_uint8 plac1 = 0, plac2 = 0;
    nvram_importance_parameter_struct *t_imp = (nvram_importance_parameter_struct*)IMPORT_USER_SUT;
    nvram_importance_parameter_struct *t_def = (nvram_importance_parameter_struct*)OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT;

    if(t_imp->build_date[0] == 0xFF || t_imp->build_time[0] == 0xFF) /* ������������Ŀ�걸����δ��ʼ��ʱ��ȫ��0xFF����־���ַ���ӡ��ʾ��������ܵ������� */
        LOGD(L_DRV, L_V5, "type:%d %s: , %s: ", type, t_def->build_date, t_def->build_time);
    else
        LOGD(L_DRV, L_V5, "type:%d %s:%s, %s:%s", type, t_def->build_date, t_imp->build_date, t_def->build_time, t_imp->build_time);
    
    if(strcmp(t_imp->build_date, t_def->build_date) || strcmp(t_imp->build_time, t_def->build_time))
    {
#if defined (__GT740__)//||defined(__GT420D__)
		if(G_realtime_data.ams_log)
		{
		    char sttime_str[100] = {0};
		    snprintf(sttime_str, 99, "(Y)(%d)%d, (%s,%s), (%s,%s)", RUNING_TIME_SEC, RUNING_TIME_SEC,t_def->build_date, t_def->build_time,t_imp->build_date,  t_imp->build_time);
		    track_cust_save_logfile(sttime_str, strlen(sttime_str));
		    G_realtime_data.ams_push=1;
			Track_nvram_write(NVRAM_EF_REALTIME_LID, 1, (void *)&G_realtime_data, NVRAM_EF_REALTIME_SIZE);

		}
#endif
        LOGD(L_DRV, L_V6, "DISK1 %d not match! Reset Factory Default!!%d,%d", type, plac1, plac2);

        result = KAL_FALSE;
        track_drv_update_disk1(type, (kal_uint8 *)OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT, sds[0].address, sds[0].size, NULL);
    }
    return result;
}

#endif /* __TRACK_DISK_NVRAM__ */
/******************************************************************************
 *  Function    -  Track_nvram_read
 *
 *  Purpose     -  NVRAM ��
 *
 *  Description -�˺���������Ҫ����־��Ϣ���ᵼ������������ӣ������ϸ���ԡ�liujw
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 15-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_bool Track_nvram_read(nvram_lid_enum LID, kal_uint16 rec_index, void *buffer, kal_uint32 buffer_size)
{
    kal_int32 result = 0;
#if defined( __TRACK_DISK_NVRAM__)
    if(LID <= NVRAM_EF_REALTIME_LID)
    {
        //LOGD(L_DRV, L_V6, "LID %d address:%x, bsize:%d,ssize%d", LID, sds[LID].address, buffer_size, sds[LID].size);
    }
    switch(LID)
    {
        case NVRAM_EF_IMPORTANCE_PARAMETER_LID:
        case NVRAM_EF_PHONE_NUMBER_LID:
        case NVRAM_EF_IOT_PARAMS_LID:    
        case NVRAM_EF_PARAMETER_LID:
            notWrite = 0;
            if(buffer_size != sds[LID].size)
            {
                LOGD(L_DRV, L_V1, "ERROR! %d,%d", buffer_size, sds[LID].size);
                track_cust_restart(67, 0);
                return KAL_FALSE;
            }
            result = OTA_readRawDiskData(&OTA_FlashDiskDriveData[1], sds[LID].address, buffer, sds[LID].size);
            if(KAL_FALSE == track_drv_check_disk1_nvram(1))
            {
                // track_os_intoTaskMsgQueue2(track_os_importance_backup_read);//ԭ�汾�����а汾��ƥ����Ҫ��ԭ
                tr_start_timer(TRACK_CUST_READ_BACKUP_TIMER, 1000, track_os_importance_backup_read);
            }
            return KAL_TRUE;

        case DISK1_LAST_LID:
            LOGD(L_DRV, L_V6, "LID %d address:%x, bsize:%d,ssize%d", LID, sds[LID].address, buffer_size, sds[LID].size);
            if(buffer_size >= 4096)
            {
                LOGD(L_DRV, L_V1, "ERROR! %d,%d", buffer_size, sds[LID].size);
                track_cust_restart(67, 0);
                return;
            }
            result = OTA_readRawDiskData(&OTA_FlashDiskDriveData[1], sds[LID].address, buffer, buffer_size);
            track_drv_check_disk1_nvram(1);
            LOGH(L_DRV, L_V6, buffer, buffer_size);
            return KAL_TRUE;

        case DISK1_BACKUP_LID:
            LOGD(L_DRV, L_V6, "LID %d address:%x, bsize:%d,ssize%d", LID, sds[LID].address, buffer_size, sds[LID].size);
            if(buffer_size >= 4096)
            {
                LOGD(L_DRV, L_V1, "ERROR! %d,%d", buffer_size, sds[LID].size);
                track_cust_restart(67, 0);
                return KAL_FALSE;
            }
            result = OTA_readRawDiskData(&OTA_FlashDiskDriveData[1], sds[LID].address, buffer, buffer_size);
            return KAL_TRUE;
        case     NVRAM_EF_LOGD_LID:
            track_log_manage(1, buffer, buffer_size);
            return KAL_TRUE;
        case  NVRAM_EF_REALTIME_LID:
            track_realtime_manage(1, buffer, buffer_size);
            return KAL_TRUE;

        case NVRAM_EF_GPS_BACKUP_DATA_LID:
            gps_data_backup_manage(1 , buffer, rec_index, buffer_size);
            return KAL_TRUE;
        default:
            break;
    }
#endif /* __TRACK_DISK_NVRAM__ */
    return nvram_external_read_data(LID, rec_index, (kal_uint8*) buffer, buffer_size);
}

/******************************************************************************
 *  Function    -  Track_nvram_write
 *  Purpose     -  NVRAM д
 *  Description -
 * modification history
16:23:32.923> ====ADDRESS	StructSize====
16:23:32.923> 350252	564
16:23:32.923> 350816	644
16:23:32.923> 351460	128
16:23:32.923> 351588	488
16:23:32.923> 352076	164
16:23:32.923> ========
16:23:34.984> AT^GT_CM=CS,211
16:23:34.984>
16:23:34.984> ====ADDRESS	StructSize====
16:23:34.984> PAR:272586796	564
16:23:34.984> IMP:272587360	644
16:23:34.984> DRV:272588004	128
16:23:34.984> PHO:272588132	488
16:23:34.984> REA:272588620	164
16:23:34.984> ========
 * ----------------------------------------
 * v1.0  , 15-01-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_bool Track_nvram_write(nvram_lid_enum LID, kal_uint16 rec_index, void *buffer, kal_uint32 buffer_size)
{
#if defined( __TRACK_DISK_NVRAM__)
    kal_bool result = KAL_FALSE;
    kal_uint32 data_index = 0;
    kal_int32 len = 0;
    kal_uint8 data[100] = {0};
    LOGD(5,5,"111:%d",LID);
    switch(LID)
    {
            //case NVRAM_EF_REALTIME_LID:/*��400ticksʱ��д�������*/
            //    LOGD(L_DRV, L_V5, "NVRAM_EF_REALTIME_LID ticks:%d", kal_get_systicks());
        case NVRAM_EF_IMPORTANCE_PARAMETER_LID:
        case NVRAM_EF_PHONE_NUMBER_LID:
        case NVRAM_EF_PARAMETER_LID:
        case NVRAM_EF_IOT_PARAMS_LID: 
        case IMPORT_PARAM_ORG_LID:
        case DISK1_LAST_LID:
        case DISK1_BACKUP_LID:
            if(notWrite)
            {
                LOGD(L_DRV, L_V5, "δ�����ݳ�ʼ������ֹд��");
                return;
            }
            if(!writeValid)
            {
                LOGD(L_DRV, L_V5, "��ʱ��ֹд������! LID:%d,bufsize:%d, address:ox%x, ssize:%d, writeValid:%d", LID, buffer_size, sds[LID].address, sds[LID].size, writeValid);
                return KAL_TRUE;
            }
            else
            {
                LOGD(L_DRV, L_V6, "LID:%d,bufsize:%d, address:ox%x, ssize:%d, writeValid:%d", LID, buffer_size, sds[LID].address, sds[LID].size, writeValid);
            }
            if(sds[LID].size  >= buffer_size)
            {
                //LOGH(L_DRV, L_V7, buffer, buffer_size);
                if(IMPORT_PARAM_ORG_LID == LID)
                {
                    data_index = get_import_dataIndex();
                    LOGD(L_DRV, L_V5, "data_index=0x%x,0x%x", data_index, (kal_uint32)OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT);
                    result = track_drv_update_disk1(0, (kal_uint8 *)buffer, data_index, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE, NULL);
                    track_drv_check_disk1_nvram(2);
                    track_drv_importance_paramter_toStrings();

                }
                else if(DISK1_LAST_LID == LID || DISK1_BACKUP_LID == LID)
                {
                    result = track_drv_update_disk1(0, buffer, sds[LID].address, buffer_size, NULL);
                }
                else
                {
                    result = track_drv_update_disk1(0, buffer, sds[LID].address, buffer_size, NULL);
                    result = track_drv_check_disk1_nvram(1);
                    if(result == KAL_FALSE)
                    {

                        tr_start_timer(TRACK_CUST_READ_BACKUP_TIMER, 1000, track_os_importance_backup_read);
                        // track_os_intoTaskMsgQueue2(track_os_importance_backup_read) ;  //��ͬ��������
                    }
                    else
                    {
                        tr_start_timer(TRACK_CUST_WRITE_BACKUP_TIMER, 1000, track_os_importance_backup_write);
                        //track_os_intoTaskMsgQueue2(track_os_importance_backup_write) ;  //��ͬ��������
                    }
                }

            }
            else
            {
                LOGD(L_DRV, L_V5, "====beyond your size[s %d,b %d]===", sds[LID].size, buffer_size);
            }
            return result;

        case NVRAM_EF_LOGD_LID:
            //OGD(L_DRV, L_V5, "buffer_size=%d", buffer_size);
            //LOGH(L_CMD, L_V1, buffer, buffer_size);
            track_log_manage(2, buffer, buffer_size);
            return KAL_TRUE;

        case NVRAM_EF_REALTIME_LID:
            track_realtime_manage(2, buffer, buffer_size);
            return  KAL_TRUE;

        case NVRAM_EF_GPS_BACKUP_DATA_LID:
            gps_data_backup_manage(2 , buffer, rec_index, buffer_size);
            return  KAL_TRUE;

        default:
            break;
    }
#endif /* __TRACK_DISK_NVRAM__ */
    return nvram_external_write_data(LID, rec_index, (kal_uint8*) buffer, buffer_size);
}

kal_bool track_drv_nvram_write_switch(kal_uint8 sw)
{
    if(sw == 0) writeValid = KAL_FALSE;
    else if(sw == 1) writeValid = KAL_TRUE;
    LOGD(L_DRV, L_V5, "sw:%d, writeValid:%d", sw, writeValid);
    return writeValid;
}

/******************************************************************************
 *  Function    -  track_drv_get_mcc_from_imsi
 *
 *  Purpose     -  ��IMSI �л�ȡMCC
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 04-03-2013,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
char* track_drv_get_mcc_from_imsi(void)
{
    char *imsi;
    static char mcc[6] = {0};

    imsi = (char *)track_drv_get_imsi(0);
    strncpy(mcc, imsi, 3);
    LOGD(L_APP, L_V5, "%s", imsi);
    return mcc;
}


#if defined(__SAVING_CODE_SPACE__)
#else
void track_drv_test_disk1(int m)
{

#if defined( __TRACK_DISK_NVRAM__)
    int result = 0;
    nvram_parameter_struct              t_par = {0};

    if(m == 210)
    {
        track_drv_importance_paramter_toStrings();
        toString();
        toStrings();
    }
    else if(m == 211)
    {
        char domain[TRACK_MAX_URL_SIZE] = {0};
        nvram_importance_parameter_struct   t_imp = {0};
        Track_nvram_read(NVRAM_EF_IMPORTANCE_PARAMETER_LID, 1, &t_imp, sds[NVRAM_EF_IMPORTANCE_PARAMETER_LID].size);

        Track_nvram_write(DISK1_LAST_LID, 1, (kal_uint8 *)&t_imp, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);

        track_drv_Disk1_last_sector_toStrings();
        track_domain_encode("fs.vcanx.com", domain);
        memcpy(t_imp.server.url, domain, TRACK_MAX_URL_SIZE - 1);
        memset(t_imp.server.server_ip, 0xf0, 4);

        Track_nvram_write(DISK1_LAST_LID, 1, (kal_uint8 *)&t_imp, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        track_drv_Disk1_last_sector_toStrings();

    }
    else if(m == 212)
    {
        //add[2329292,350272,584]
        sprintf(G_parameter.sos_num[0], "66855");
        sprintf(G_parameter.sos_num[1], "15018609855");
        LOGD(L_DRV, L_V5, "add[%d,%d,%d]sos[%s,%s]",
             (kal_uint8 *)&G_parameter, sds[0].address, sds[0].size, G_parameter.sos_num[0], G_parameter.sos_num[1]);

        result = track_drv_update_disk1(0, (kal_uint8 *)&G_parameter, sds[0].address, sds[0].size, NULL);

        Track_nvram_read(NVRAM_EF_PARAMETER_LID, 1, &t_par, sds[NVRAM_EF_PARAMETER_LID].size);

        LOGD(L_DRV, L_V5, "result:%d, sos_num[%s,%s,%s], url[%s]",
             result, (char*)(t_par.sos_num[0]), t_par.sos_num[1], t_par.sos_num[2], t_par.url);

    }
    else if(m == 213)
    {
        kal_uint32 data_index = 0;
        char domain[TRACK_MAX_URL_SIZE] = {0};
        nvram_importance_parameter_struct pOrg =  {0};

        memcpy(&pOrg, (void*)OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        track_domain_encode("gps300.vcanx.com", domain);
        memcpy(pOrg.server.url, domain, TRACK_MAX_URL_SIZE - 1);
        memset(pOrg.server.server_ip, 0xaa, 4);

        data_index = (kal_uint32)get_import_dataIndex();
        //0x103f5150
        LOGD(L_DRV, L_V5, "data_index=0x%x,0x%x", data_index, (kal_uint32)OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT);
        track_drv_update_disk1(3, (kal_uint8 *)&pOrg, data_index, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE, NULL);

        track_drv_importance_paramter_toStrings();
        track_drv_check_disk1_nvram(2);
        track_drv_importance_paramter_toStrings();

    }
    else if(m == 214)
    {
        result = OTA_eraseRawDiskBlock(&OTA_FlashDiskDriveData[1], 88);
        LOGD(L_DRV, L_V5, "eraseRawDiskBlock result = %d", result);
    }
    else if(m == 215)
    {
    }
    else if(m == 216)
    {
        char domain[TRACK_MAX_URL_SIZE] = {0};
        kal_uint32 data_index = 0;
        nvram_importance_parameter_struct pOrg =  {0};

        memcpy(&pOrg, (void*)IMPORT_USER_SUT, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        track_domain_encode("gps.vcanx.com", domain);

        memcpy(pOrg.server.url, domain, TRACK_MAX_URL_SIZE - 1);
        memset(pOrg.server.server_ip, 0x0f, 4);

        data_index = (kal_uint32)OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT - DISK1_START_ADD;
        //0x103f5150
        LOGD(L_DRV, L_V5, "data_index=0x%x,0x%x", data_index, (kal_uint32)OTA_NVRAM_EF_IMPORTANCE_PARAMETER_DEFAULT);
        track_drv_update_disk1(0, (kal_uint8 *)&pOrg, data_index, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE, NULL);

        track_drv_importance_paramter_toStrings();


        track_domain_encode("fs.vcanx.com", domain);
        memcpy(pOrg.server.url, domain, TRACK_MAX_URL_SIZE - 1);
        memset(pOrg.server.server_ip, 0xf0, 4);

        Track_nvram_write(IMPORT_PARAM_ORG_LID, 1, (kal_uint8 *)&pOrg, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        track_drv_importance_paramter_toStrings();

        Track_nvram_write(IMPORT_PARAM_ORG_LID, 1, (kal_uint8 *)&G_importance_parameter_data, NVRAM_EF_IMPORTANCE_PARAMETER_SIZE);
        track_drv_importance_paramter_toStrings();

    }

#else
    LOGD(L_DRV, L_V5, "not support!");
#endif /* __TRACK_DISK_NVRAM__ */
}
#endif
void track_drv_iccid(kal_uint8* data, kal_uint8 len)
{
    LOGD(L_DRV, L_V5, "%s", data);
}

/******************************************************************************
 *  Function    -  track_drv_rand
 * 
 *  Purpose     -  ����ʱ���оƬID���������
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-10-19,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint32 track_drv_rand(void)
{
    applib_time_struct currTime = {0};
    kal_uint8 chip_id[16] = {0};    
    kal_uint32 sec,rid,result;
    
    applib_dt_get_rtc_time(&currTime);
    sec = applib_dt_mytime_2_utc_sec(&currTime, 0);

    memcpy(chip_id, track_drv_get_chip_id(), 16);
    rid=(chip_id[0]<<16)+(chip_id[1]<<8)+(chip_id[2]);    
   
    srand(sec+rid);
    result=rand();
    LOGD(L_DRV, L_V6, "���ֵ %d", result);
    return result;
}


/******************************************************************************
 *  Function    -  track_drv_get_one_day_reset_time
 * 
 *  Purpose     -  ����ÿ������ʱ�����������λms
 * 
 *  Description -  ʱ��h [21,23.5]  [24.5-27]
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2016-10-19,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint32 track_drv_get_one_day_reset_time(void)
{
    kal_uint32 r,sec,result;
    kal_uint8 flag;
    
    r=track_drv_rand();

    sec=r&0x7FFF;
    flag=r&0x01;
   
    result=(sec%9000)+(12600*flag)+75600;
    result=result*1000;
    LOGD(L_DRV, L_V5, "24h : %dms",result);
    return result;
}
