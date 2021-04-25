#if defined(__IMPORTANT_PARAMETER_4K__)
/******************************************************************************
 *  -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *
 *
 *
 *
 * modification history
 * --------------------
 *
 ******************************************************************************/

/****************************************************************************
* Include Files             包含头文件
*****************************************************************************/
#include "track_drv_system_param.h"
#include "nvram_editor_data_item.h"
#include "track_os_ell.h"
#include "track_cust.h"
#include "..\..\ell\ell-common\ell_fota_memory_define.h"

/*****************************************************************************
*  Define			               宏定义
*****************************************************************************/


/*****************************************************************************
* Typedef  Enum                      枚举
*****************************************************************************/


/*****************************************************************************
 *  Struct			               数据结构定义
*****************************************************************************/


/*****************************************************************************
* Local variable                     局部变量
*****************************************************************************/


/****************************************************************************
* Global Variable                    全局变量
*****************************************************************************/


/****************************************************************************
* Global Variable - Extern           引用全局变量
*****************************************************************************/
extern nvram_alone_parameter_struct NVRAM_ALONE_PARAMETER_DEFAULT;

/*****************************************************************************
*  Global Functions	- Extern	    引用外部函数
*****************************************************************************/
extern void track_os_importance_backup_write(void);

/*****************************************************************************
*  Local Functions	               本地函数申明
*****************************************************************************/

#if 0
#endif /* 0 */

static kal_int32 tr_writeRawDiskData(kal_uint32 blkIdx, void* data_ptr, kal_uint16 len)
{
    int i, ret;
    LOGD(L_DRV, L_V2, "blkIdx:%X", blkIdx);
    if(len > SDS_PAGE_SIZE)
    {
        len = SDS_PAGE_SIZE;
        LOGS("tr_writeRawDiskData() len:%d 待写入的数据长度太大，超过了4096");
    }
    for(i = 0; i < 3; i++)
    {
        ret = OTA_eraseRawDiskBlock(&OTA_FlashDiskDriveData[1], blkIdx);
        if(ret == RAW_DISK_ERR_NONE) break;
    }
    if(ret != RAW_DISK_ERR_NONE)
    {
        LOGS("tr_writeRawDiskData() eraseRawDiskBlock 严重错误，出现既需要关注分析! %d,%d", ret,blkIdx);
        return ret;
    }
    for(i = 0; i < 3; i++)
    {
        ret = OTA_writeRawDiskData(&OTA_FlashDiskDriveData[1], blkIdx * SDS_PAGE_SIZE, data_ptr, len);
        if(ret == RAW_DISK_ERR_NONE) break;
    }
    if(ret != RAW_DISK_ERR_NONE)
    {
        LOGS("tr_writeRawDiskData() writeRawDiskData 严重错误，出现既需要关注分析! %d", ret);
        return ret;
    }
    return RAW_DISK_ERR_NONE;
}

static int track_nvram_alone_parameter_write(nvram_alone_parameter_struct *data)
{
    static nvram_alone_parameter_struct d;
    static char page_buf[SDS_PAGE_SIZE] = {0};
    if(data->flag != 0x12345678)
    {
        //LOGD(L_DRV, L_V1, "Error: 数据写入格式校验错误");
        LOGS("Error: 数据写入格式校验错误");
        return -1;
    }
    OTA_readRawDiskData(&OTA_FlashDiskDriveData[1], DISK1_DATA_PARAMETER_BACKUP_ADD, (kal_uint8*)&d, sizeof(nvram_alone_parameter_struct));
    if(memcmp(&d, data, sizeof(nvram_alone_parameter_struct)))
    {
        memset(page_buf, 0, SDS_PAGE_SIZE);
        memcpy(&page_buf, data, NVRAM_EF_ALONE_PARAMTER_SIZE);
        tr_writeRawDiskData(DISK1_DATA_PARAMETER_BACKUP_PAGE, page_buf, SDS_PAGE_SIZE);
    }
    tr_start_timer(TRACK_CUST_WRITE_BACKUP_TIMER, 100, track_os_importance_backup_write);
    return 0;
}

nvram_alone_parameter_struct * track_nvram_alone_parameter_read(void)
{
    static nvram_alone_parameter_struct data = {0};
    OTA_readRawDiskData(&OTA_FlashDiskDriveData[1], DISK1_DATA_PARAMETER_BACKUP_ADD, (kal_uint8*)&data, NVRAM_EF_ALONE_PARAMTER_SIZE);
    if(data.flag != 0x12345678)
    {
        LOGD(L_DRV, L_V1, "Error: 还原");
        memcpy(&data, (void*)OTA_NVRAM_ALONE_PARAMETER_DEFAULT, NVRAM_EF_ALONE_PARAMTER_SIZE);
        track_nvram_alone_parameter_write(&data);
        memcpy(&data, 0, NVRAM_EF_ALONE_PARAMTER_SIZE);
        OTA_readRawDiskData(&OTA_FlashDiskDriveData[1], DISK1_DATA_PARAMETER_BACKUP_ADD, (kal_uint8*)&data, NVRAM_EF_ALONE_PARAMTER_SIZE);
        if(data.flag != 0x12345678)
        {
            LOGS("Error: 数据恢复默认后，校验还是错误");
        }
    }    
    return &data;
}

static int parameter_apn_write(nvram_GPRS_APN_struct *data)
{
    nvram_alone_parameter_struct *d = track_nvram_alone_parameter_read();
    memcpy(&d->apn, data, sizeof(nvram_GPRS_APN_struct));
    return track_nvram_alone_parameter_write(d);
}

static int parameter_server_write(nvram_server_addr_struct *data)
{
    nvram_alone_parameter_struct *d = track_nvram_alone_parameter_read();
    memcpy(&d->server, data, sizeof(nvram_server_addr_struct));
    return track_nvram_alone_parameter_write(d);
}

static int parameter_ams_server_write(nvram_server_addr_struct *data)
{
    nvram_alone_parameter_struct *d = track_nvram_alone_parameter_read();
    memcpy(&d->ams_server, data, sizeof(nvram_server_addr_struct));
    return track_nvram_alone_parameter_write(d);
}

static int parameter_gprson_write(kal_uint8 data)
{
    nvram_alone_parameter_struct *d = track_nvram_alone_parameter_read();
    d->gprson = data;
    return track_nvram_alone_parameter_write(d);
}

static int parameter_reset_default(kal_uint8 data)
{
    return track_nvram_alone_parameter_write(&NVRAM_ALONE_PARAMETER_DEFAULT);
}

nvram_alone_parameter_cmd_struct nvram_alone_parameter_cmd = 
{
    parameter_reset_default,
    parameter_apn_write,
    parameter_server_write,
    parameter_ams_server_write,
    parameter_gprson_write
};

#endif /* __IMPORTANT_PARAMETER_4K__ */
