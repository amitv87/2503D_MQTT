/******************************************************************************
 * track_at_sim.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *
 *
 * modification history
 * --------------------
 * v1.0   2012-11-17,  wangqi create this file
 *
 ******************************************************************************/
/****************************************************************************
* Include Files             包含头文件
*****************************************************************************/
#include "track_at_sim.h"
#include "track_at.h"
#include "track_at_sms.h"
#include "track_os_timer.h"
#include "track_os_data_types.h"

#include "c_RamBuffer.h"
#include "c_vector.h"

/*****************************************************************************
 *  Define			    宏定义
*****************************************************************************/
#define BACK_MSG_MAX_LENGTH     137             //140-2-1
/*****************************************************************************
* Typedef  Enum         枚举
*****************************************************************************/


/*****************************************************************************
*  Struct			    数据结构定义
*****************************************************************************/

typedef struct
{
    kal_uint8                   *backup_buf;
    int                         backup_len;
    
    kal_uint8                   packets;
    kal_uint8                   send_sucess;//发送成功次数
    
    kal_uint8                   read_index;//一共读到的备份指令信息条数
    kal_uint8                   r_saved_total;//读到的信息内记录总条数

    char *                      read_msg;
    kal_uint16                  read_msg_len;


    kal_timer_func_ptr          backup_sucess_ind;
    kal_timer_func_ptr          read_msg_ind;
} bsms_cmd_struct;

typedef struct
{
    kal_uint16                  back_crc;
    kal_uint8                   packet_total_cur;
    kal_uint8                   ctx[BACK_MSG_MAX_LENGTH];
} backup_sos_para_struct;


/*****************************************************************************
* Local variable            局部变量
*****************************************************************************/
static bsms_cmd_struct          bsms = {0};

/****************************************************************************
* Global Variable           全局变量
*****************************************************************************/

/*  Global Variable - Extern             引用全局变量
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            引用外部函数
******************************************************************************/

/*****************************************************************************
*  Local Functions Define               本地函数声明
******************************************************************************/

/*****************************************************************************
 *  Local Functions	            本地函数
*****************************************************************************/


/*****************************************************************************
 *  Global Functions            全局函数
*****************************************************************************/

void track_sim_handle_senderbox(kal_timer_func_ptr read_callback)
{
    LOGD(L_CMD, L_V4, "");

    if(track_sim_get_sms_from_senderbox())
    {
        tr_start_timer(TRACK_SIM_READ_SB_TIMER_ID, 5000, track_sim_handle_senderbox);
    }

    bsms.read_msg_ind = read_callback;
}

static int backup_sms_ciphertext_conver(char* string, kal_uint8 sign)
{
    int index = 0;
    while(* string && index++ < 160)
    {
        * string ^= sign;
        string++;
    }
    return index;
}


static void send_sms_to_senderbox(char * ctx, int ilength)
{
    sms_msg_struct ssms = {0};
    memset(&ssms, 0, sizeof(sms_msg_struct));
    ssms.codetype = KAL_TRUE;
    ssms.ctxlen = ilength;
    ssms.ctx = ctx;
    ssms.nocount = 1;
    ssms.smsdir = SMS_SENDER_BOX;
    ssms.numbers[0] = "";
    track_sms_sender(&ssms);
    LOGD(L_CMD, L_V5, "---->>len:%d,over<<----", ilength);
}



static void backup_msg_callback(int type)
{
    LOGD(L_CMD, L_V5, "---->>type:%d----", type);
    sim_read_senderbox_lock(0);
    if(bsms.backup_sucess_ind != NULL)
    {
        bsms.backup_sucess_ind((void *)type);
    }
    if(bsms.backup_buf)
    {
        Ram_Free(bsms.backup_buf);
        bsms.backup_buf = NULL;
    }
}

static void backup_to_senderbox(void)
{
    kal_bool ret;
    kal_uint8 *ptr_sos = NULL , total_p = 0 , cur_index = 0;
    int  modelen = 0, buf_len = 0;

    sms_msg_struct ssms = {0};
    backup_sos_para_struct backup_sos = {0};

    LOGD(L_CMD, L_V5, "");

    if(bsms.backup_buf == NULL || 0 == bsms.backup_len)
    {
        LOGD(L_CMD, L_V5, "backup_buf:%d,backup_len:%d----------", bsms.backup_buf, bsms.backup_len);
        backup_msg_callback(-1);
        return;
    }
    ptr_sos = bsms.backup_buf;
    buf_len = bsms.backup_len;

    total_p = (kal_uint8)(buf_len / BACK_MSG_MAX_LENGTH) ; //分包整包数

    modelen = buf_len % BACK_MSG_MAX_LENGTH;//分完整包后剩余长度

    LOGD(L_CMD, L_V5, "total_p:%d,modelen:%d------------------", total_p, modelen);

    if(total_p > 6)
    {
        LOGD(L_CMD, L_V5, "----backup msg too long-----");
        backup_msg_callback(-2);
        return;
    }
    
    bsms.packets = total_p;
    bsms.send_sucess = 0;
    do
    {
        memset(&ssms, 0, sizeof(sms_msg_struct));

        backup_sos.packet_total_cur = (total_p << 4) + cur_index;

        if(cur_index == total_p)
        {
            ssms.ctxlen = modelen;
        }
        else
        {
            ssms.ctxlen = BACK_MSG_MAX_LENGTH;
        }

        memcpy(backup_sos.ctx, ptr_sos, ssms.ctxlen);
        backup_sos.back_crc = GetCrc16((U8 *)&backup_sos.packet_total_cur, ssms.ctxlen + 1);

        ptr_sos += ssms.ctxlen;

        LOGD(L_CMD, L_V5, "---packet_total_cur:%x--crc:%x--", backup_sos.packet_total_cur, backup_sos.back_crc);

        ssms.codetype = KAL_FALSE;
        ssms.ctxlen += 3;
        ssms.ctx = (char*)&backup_sos;
        LOGH(L_CMD, L_V5, ssms.ctx, ssms.ctxlen);
        ssms.nocount = 1;
        ssms.smsdir = SMS_SENDER_BOX;
        ssms.numbers[0] = "\0";
        track_sms_sender(&ssms);

        cur_index++;

    }
    while(cur_index <= total_p);

    LOGD(L_CMD, L_V5, "total_p:%d,cur_index:%d--------------------", total_p, cur_index);
    
    //track_start_timer(TRACK_SIM_W_CALLBACK_TIMER_ID, 4 * (total_p + 1) * 1000 ,backup_msg_callback, (void *) 0);
    //backup_msg_callback(0);
    track_start_timer(TRACK_SIM_W_CALLBACK_TIMER_ID,5 * (total_p + 1) * 1000,backup_msg_callback, (void *) -3);
}


void track_sim_sendmsg_rsp(sms_send_rsp_struct *rsps)
{

    LOGD(L_CMD, L_V5, "total:%d,sucess:%d,result:%d,status:%d------------"
        ,bsms.packets,bsms.send_sucess,rsps->result,rsps->result);

    
    if(rsps->result == sSENDOK)
    {
        if(bsms.send_sucess == bsms.packets)
        {
            tr_stop_timer(TRACK_SIM_W_CALLBACK_TIMER_ID);
            bsms.send_sucess = 0;
            backup_msg_callback(0);
        }
        
        bsms.send_sucess++;
    }
}
kal_int8 track_sim_backup_msg(void *buf, int buf_len, kal_timer_func_ptr callback)
{
    LOGD(L_CMD, L_V5, "");

    if(sim_read_senderbox_lock(2) != 0)
    {
        return -1;
    }


    sim_read_senderbox_lock(1);
    bsms.backup_buf = (kal_uint8 *)Ram_Alloc(1, buf_len);
    
    if(bsms.backup_buf == NULL)
    {
        //sim_read_senderbox_lock(0);
        return -2;
    }
    bsms.backup_sucess_ind = callback;
    bsms.send_sucess = 0;

    bsms.backup_len = buf_len;
    memcpy(bsms.backup_buf, buf, bsms.backup_len);

    track_sim_clean_senderbox();

    tr_start_timer(TRACK_SIM_WRITE_SB_TIMER_ID, 3000, backup_to_senderbox);

    return 0;
}

static kal_int8 track_set_sms_to_buffer(backup_sos_para_struct *r_msg)
{
    kal_uint16 cur_msg_index = 0;

    LOGD(L_CMD, L_V5, "-----------VVVVV---------------------------VVVVV--------------");

    cur_msg_index = r_msg->packet_total_cur & 0x0F;
    bsms.r_saved_total = (r_msg->packet_total_cur >> 4) & 0x0F;


    if(NULL == bsms.read_msg)
    {
        bsms.read_msg_len = (bsms.r_saved_total + 1) * BACK_MSG_MAX_LENGTH;
        bsms.read_msg = (kal_uint8 *)Ram_Alloc(1, bsms.read_msg_len);

        if(NULL == bsms.read_msg)
        {
            LOGD(L_CMD, L_V5, "---ram_alloc fail error!-----");
            return -2;
        }
        memset(bsms.read_msg, 0, bsms.read_msg_len);
    }

    LOGD(L_CMD, L_V5, "---r_saved_total:%d,cur:%d!-readlen:%d----", bsms.r_saved_total, cur_msg_index, bsms.read_msg_len);

    if(bsms.r_saved_total)
    {
        char *ptrin = NULL, *ptreof = NULL;
        ptrin = bsms.read_msg + cur_msg_index * BACK_MSG_MAX_LENGTH;
        ptreof = bsms.read_msg + bsms.read_msg_len;
        LOGD(L_CMD, L_V5, "begin:%x,eof:%x,cur:%x", bsms.read_msg, ptreof, ptrin);
        if(ptrin < bsms.read_msg || ptrin > ptreof - BACK_MSG_MAX_LENGTH)
        {
            LOGD(L_CMD, L_V5, "---ram_alloc find place error!-----");
            return -3;
        }
        memcpy(ptrin, r_msg->ctx, BACK_MSG_MAX_LENGTH);
    }
    else
    {
        memcpy(bsms.read_msg, r_msg->ctx, BACK_MSG_MAX_LENGTH);
    }

    bsms.read_index++;

    LOGH(L_CMD, L_V5, bsms.read_msg, bsms.read_msg_len);

    LOGD(L_CMD, L_V5, "--------^^^^^-----have read:%d,cur:%d----------^^^^^----------", bsms.read_index, cur_msg_index);

    return 0;
}



static kal_int8 tr_sim_handle_back_sms(SM_PARAM * sms_get)
{
    //char *p = NULL;
    kal_int8 ret = -1;
    kal_uint16 newcrc = 0;
    kal_uint16 oldcrc = 0;
    backup_sos_para_struct read_msg = {0};

    LOGD(L_CMD, L_V5, ":%d,DSC:%d,UDL:%d",
         sms_get->index, sms_get->TP_DCS, sms_get->TP_UDL);


    if(sms_get->TP_UDL < 4 && sms_get->TP_UDL > SMSUCS2MAX) /*CRC+smstotal+ctx*/
    {
        LOGD(L_CMD, L_V5, "msg length erorr!%d", sms_get->TP_UDL);
        return 0;
    }

    if(sms_get->TP_DCS == GSM_UCS2) //其它备份信息用GSM_UCS2保存
    {

        //LOGD(L_CMD, L_V5, ":%x,%x",*sms_get->TP_UD,*(sms_get->TP_UD+1));
        //分解

        memcpy(&oldcrc, sms_get->TP_UD, 2);
        //oldcrc = (kal_uint16)sms_get->TP_UD;

        newcrc = GetCrc16((U8*)(sms_get->TP_UD + 2), sms_get->TP_UDL - 2);
        LOGD(L_CMD, L_V5, "crc old:%x,new:%x,udl:%d", oldcrc, newcrc, sms_get->TP_UDL);
        //验证CRC
        //将信息存储入Buffer
        if(oldcrc == newcrc)
        {
            memset(&read_msg, 0, sizeof(backup_sos_para_struct));
            read_msg.back_crc = newcrc;
            read_msg.packet_total_cur = *(sms_get->TP_UD + 2);

            memcpy(&read_msg.ctx, sms_get->TP_UD + 3, sms_get->TP_UDL - 3);

            ret = track_set_sms_to_buffer(&read_msg);
        }
        else
        {
            LOGD(L_CMD, L_V5, "CRC Error!");
            ret = -4;
        }

        LOGD(L_CMD, L_V5, "ret:%d read total packet:%d,r_saved_total:%d,read_index:%d"
             , ret, read_msg.packet_total_cur, bsms.r_saved_total, bsms.read_index);

    }

    return ret;
}

/******************************************************************************
 * FUNCTION:  - track_drv_get_sim_sms_callback
 * DESCRIPTION: - 获取收件箱所有信息后调用此接口进行备份信息分解
 * modification history
 * --------------------
 * v1.0  , 09-12-2012,  wangqi  written
 * --------------------
 ******************************************************************************/
void track_sim_get_sms_callback(void)
{
    SM_PARAM sms_get = {0};
    read_msg_struct read_ind = {0};

    LOGD(L_CMD, L_V5, "vec_outbox:%d", get_vec_out_size());

    if(sim_read_senderbox_lock(2))
    {
        tr_start_timer(TRACK_OUTBOX_READY_TIMER_ID, 3000, track_sim_get_sms_callback);
    }//TODO ?
    else if(get_vec_out_size() == 0)
    {
        //短信为空
        read_ind.error_msg = -1;
        if(bsms.read_msg_ind != NULL)
        {
            bsms.read_msg_ind(&read_ind);
        }
    }
    else
    {
        //memset(&bsms,0,sizeof(bsms_cmd_struct));
        bsms.read_index = 0;//一共读到的备份指令信息条数
        bsms.r_saved_total = 0 ; //读到的信息内记录总条数

        bsms.read_msg = NULL;//??????
        bsms.read_msg_len = 0;
        while(track_sms_get_out_sms(&sms_get) /*&& bsms.cread_back_sms > BACK_SMS_MAX*/)
        {
            tr_sim_handle_back_sms(&sms_get);
            LOGD(L_CMD, L_V5, "read_index:%d<-----------", bsms.read_index);
            //临时关闭调试多条备份if(bsms.cread_back_sms == BACK_SMS_MAX) break;
        }

        track_sms_clean_out_vec();

        LOGD(L_CMD, L_V4, "结r_saved_total:%d,read_index:%d", bsms.r_saved_total, bsms.read_index);

        if(bsms.read_index == bsms.r_saved_total + 1)
        {
            read_ind.error_msg = 0;
            read_ind.msg_ctx = bsms.read_msg;
            read_ind.msg_len = bsms.read_msg_len;
        }
        else
        {
            LOGD(L_CMD, L_V5, "read sim msg error");
            //读取短信不正确
            read_ind.error_msg = -2;
            track_sim_clean_senderbox();
        }
        LOGD(L_CMD, L_V5, "---------------------");
        if(bsms.read_msg_ind)
        {
            bsms.read_msg_ind(&read_ind);
        }
        if(bsms.read_msg)
        {
            Ram_Free(bsms.read_msg);
            bsms.read_msg = NULL;
        }
    }
}



void track_sim_main(void)
{

    memset(&bsms, 0, sizeof(bsms_cmd_struct));
    //track_sim_clean_allsms();//清空收件箱

    //tr_start_timer(TRACK_SIM_READ_SB_TIMER_ID, 3000, track_sim_handle_senderbox);
}



/*****************************************************************************/
#if !defined (__SAVING_CODE_SPACE__)
static unsigned char test_data[801] =
{
    0x01, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x00, 0xCF, 0x90, 0x73, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x5D, 0x38, 0x74, 0xC0, 0x82, 0x68, 0x00, 0x4D, 0xDB, 0x1B, 0x00, 0xE0,
    0xF7, 0x2A, 0x00, 0x02, 0x03, 0x13, 0xD2, 0x4A, 0x3D, 0x71, 0x96, 0x41, 0x1D, 0x33, 0x48, 0x00,
    0x20, 0x00, 0x00, 0x00, 0x47, 0x58, 0x51, 0x35, 0x32, 0x5F, 0x33, 0x32, 0x33, 0x32, 0x5F, 0x4D,
    0x4F, 0x44, 0x5F, 0x31, 0x31, 0x42, 0x5F, 0x50, 0x43, 0x42, 0x30, 0x31, 0x5F, 0x67, 0x70, 0x72,
    0x73, 0x5F, 0x4D, 0x54, 0x36, 0x32, 0x35, 0x32, 0x5F, 0x53, 0x30, 0x31, 0x2E, 0x47, 0x54, 0x30,
    0x36, 0x42, 0x5F, 0x31, 0x30, 0x5F, 0x38, 0x4D, 0x4D, 0x5F, 0x42, 0x32, 0x35, 0x5F, 0x56, 0x31,
    0x30, 0x5F, 0x4C, 0x41, 0x2E, 0x62, 0x69, 0x6E, 0x00, 0x01, 0xC0, 0x42, 0x14, 0x1D, 0x55, 0x54,
    0xCC, 0x8C, 0xD9, 0x9C, 0x14, 0x1D, 0xF7, 0xC5, 0xD5, 0xC4, 0x9A, 0x6D, 0xA6, 0xC6, 0x02, 0x6D,
    0xA6, 0xCE, 0x24, 0xFA, 0x04, 0xD8, 0x10, 0x4D, 0x8C, 0x23, 0x08, 0x49, 0x13, 0x48, 0xE8, 0x49,
    0xF4, 0x03, 0x3A, 0x24, 0xA4, 0x91, 0xC8, 0x27, 0xD0, 0x11, 0x36, 0xC9, 0x04, 0x75, 0x46, 0x10,
    0x84, 0x4D, 0x84, 0x91, 0x30, 0x96, 0x56, 0x10, 0x96, 0x30, 0x92, 0xC8, 0xC2, 0x4B, 0x68, 0x16,
    0x59, 0x5D, 0x96, 0x3F, 0x3D, 0x96, 0xF9, 0x5D, 0xAC, 0x96, 0x5B, 0x61, 0x24, 0xF6, 0xD4, 0x87,
    0x0B, 0xE9, 0x24, 0x25, 0xBC, 0x27, 0x4B, 0x02, 0x01, 0x12, 0x92, 0xD8, 0x26, 0x93, 0x6D, 0x7D,
    0xC2, 0xE2, 0x6D, 0xAE, 0x8E, 0x25, 0xAD, 0xA6, 0xC9, 0x0B, 0x7D, 0xBE, 0x7D, 0xF3, 0x1E, 0xE3,
    0xCF, 0xD5, 0x98, 0xE2, 0xE6, 0xF9, 0xBE, 0x6F, 0xB3, 0x5B, 0xD7, 0x46, 0x75, 0xBD, 0xF0, 0xFE,
    0x4D, 0x69, 0xE7, 0x99, 0x5F, 0x16, 0x9E, 0x56, 0x96, 0x71, 0xF3, 0xE6, 0x73, 0xAD, 0x2E, 0x6F,
    0x8B, 0xA7, 0x00, 0xCF, 0xC0, 0xC1, 0x81, 0x50, 0x85, 0xD1, 0x4C, 0xE3, 0xCF, 0xF9, 0x7F, 0xE9,
    0xEB, 0x8B, 0x0B, 0xC9, 0xD0, 0x1C, 0x80, 0x57, 0x61, 0xB7, 0x0F, 0xAF, 0x1F, 0x0F, 0xFB, 0xD5,
    0xE1, 0x89, 0x61, 0xE0, 0x86, 0x39, 0x21, 0x18, 0xAC, 0xC4, 0x73, 0xF6, 0x67, 0x5C, 0x3D, 0xC4,
    0x10, 0x8F, 0xCF, 0x9B, 0xF9, 0xC5, 0x08, 0xBD, 0x2C, 0x42, 0xAB, 0xF4, 0x3E, 0x94, 0xDD, 0x42,
    0x79, 0x4B, 0x47, 0xE7, 0x00, 0xC6, 0xAB, 0xFE, 0x71, 0x06, 0x21, 0x16, 0x4E, 0xF4, 0xC0, 0x91,
    0x1D, 0x69, 0xB0, 0x95, 0x8D, 0x47, 0xEB, 0x34, 0xA7, 0x45, 0x90, 0xDD, 0x91, 0x0E, 0xFA, 0xA9,
    0xB4, 0x2E, 0x34, 0x1E, 0x28, 0x49, 0x04, 0x81, 0x5E, 0x76, 0xAE, 0x16, 0x05, 0x2B, 0x40, 0x1D,
    0x08, 0x52, 0x3C, 0xEC, 0xA0, 0xAF, 0x3E, 0x3C, 0x80, 0x9A, 0x7C, 0x77, 0x8F, 0x84, 0xE3, 0x33,
    0xCE, 0x58, 0xAF, 0x29, 0xF2, 0xC3, 0x27, 0x93, 0xF2, 0xC3, 0xD0, 0xBE, 0x23, 0xA9, 0x6E, 0x3E,
    0x38, 0x7E, 0xA9, 0xE7, 0x4F, 0xC1, 0x68, 0xF5, 0x22, 0x84, 0xF0, 0x2D, 0xD7, 0xCA, 0x48, 0x74,
    0x73, 0x0D, 0xB9, 0x94, 0xFD, 0xDC, 0xEF, 0x48, 0x22, 0xFD, 0x14, 0xB8, 0x64, 0x31, 0x88, 0xBE,
    0x2A, 0xC8, 0x1F, 0xEB, 0x4B, 0x86, 0x94, 0x60, 0x70, 0xAB, 0x3E, 0x77, 0xB8, 0x14, 0x07, 0x95,
    0xEE, 0xFE, 0x43, 0xC2, 0x69, 0xF1, 0x1E, 0x4B, 0x67, 0xF1, 0x45, 0x5B, 0x4D, 0xCE, 0x83, 0xED,
    0xA8, 0x82, 0x2E, 0x89, 0xFB, 0x42, 0xEE, 0x2A, 0x79, 0xEE, 0x90, 0x4E, 0xFC, 0x86, 0x6B, 0xA0,
    0xEF, 0xE8, 0xA8, 0xF3, 0xE3, 0x4E, 0x13, 0x40, 0x64, 0xB3, 0x32, 0xFD, 0xB3, 0x2C, 0xE8, 0x9E,
    0x04, 0xD1, 0x61, 0x48, 0x77, 0x1E, 0xBA, 0x18, 0x7D, 0xFE, 0xEE, 0xB0, 0x1C, 0x62, 0x09, 0x48,
    0x7E, 0xC9, 0x05, 0xB4, 0x95, 0xA7, 0xCB, 0x20, 0xB2, 0x11, 0xC7, 0xD4, 0xE1, 0x84, 0xBF, 0x44,
    0xB2, 0x0F, 0x3D, 0x53, 0x86, 0x67, 0xC7, 0x0D, 0xB6, 0x21, 0x45, 0x40, 0x6E, 0x72, 0x25, 0xC0,
    0xF2, 0x0F, 0x62, 0xF6, 0xAB, 0x11, 0xE9, 0xC5, 0xE1, 0xDE, 0x90, 0xDE, 0x1D, 0xFC, 0x61, 0xDC,
    0x57, 0x30, 0x9D, 0x31, 0x10, 0xBF, 0xDF, 0x08, 0x7C, 0xA9, 0x04, 0x5F, 0x3A, 0x47, 0xDF, 0x23,
    0xE7, 0xAF, 0xBE, 0x64, 0x38, 0x85, 0x50, 0xE9, 0xA2, 0xB3, 0xF9, 0x6F, 0x71, 0x1F, 0x50, 0x3A,
    0x41, 0x23, 0x09, 0xF6, 0x63, 0x8B, 0xB9, 0x30, 0xF2, 0x0C, 0x79, 0xE1, 0x22, 0x2B, 0x71, 0x2B,
    0xF7, 0x30, 0xF7, 0xD2, 0x0A, 0x6F, 0xB9, 0x88, 0x9B, 0xBE, 0xEA, 0x06, 0xA3, 0x29, 0xF2, 0xF5,
    0xB1, 0xEC, 0x9E, 0x3E, 0x3C, 0x38, 0x16, 0x29, 0x7D, 0x74, 0x7C, 0x93, 0xEF, 0xEE, 0xFB, 0x26,
    0xF8, 0x95, 0x26, 0x71, 0x2E, 0x67, 0x29, 0xDF, 0x65, 0x1F, 0x75, 0xF9, 0xE8, 0x71, 0x61, 0x85,
    0x7E, 0x3A, 0x35, 0x71, 0x57, 0x1D, 0xFB, 0x3F, 0xD8, 0x69, 0x7E, 0x21, 0x50, 0x6F, 0xEF, 0x85,
    0x25, 0x69, 0x7D, 0x13, 0xEC, 0x42, 0x64, 0x22, 0xDA, 0x9C, 0x1D, 0x05, 0xB7, 0xCA, 0x38, 0x71,
    0xD3, 0x01, 0xEF, 0x23, 0x7D, 0x7C, 0xFE, 0x0C, 0x94, 0xFC, 0x11, 0x20, 0x24, 0x65, 0x17, 0xE1,
    0x4B, 0x88, 0x47, 0xE3, 0xCA, 0xB8, 0x59, 0x98, 0xE8, 0x47, 0xB3, 0x7E, 0x1D, 0x62, 0x15, 0x40,
    0xC7, 0x47, 0x8A, 0xD3, 0xFC, 0x48, 0x04, 0x2E, 0x12, 0x56, 0x3E, 0x2F, 0xC4, 0x73, 0x4C, 0xB0,
    0xB3, 0xCB, 0x3E, 0xFE, 0xEF, 0x17, 0xF1, 0x37, 0x67, 0xC0, 0x94, 0x0C, 0xCB, 0xBC, 0x1C, 0x59,
    0x01, 0x28, 0x75, 0x60, 0xEC, 0xCE, 0x07, 0xCA, 0x79, 0x2E, 0x2E, 0x49, 0x4F, 0x92, 0x2B, 0x7C,
    0x7C, 0x76, 0xA9, 0x7F, 0x7B, 0xE2, 0xC9, 0x61, 0xA8, 0x42, 0x1B, 0x86, 0x97, 0x7C, 0xFA, 0x8D,
    0x4D
};
static int ccount = 0, autorun = 1;
static void test_callback(void * cb);
static void test_read_callback(void * cb);
static void test1()
{
    int i, ret;
    for(i = 0; i < 801; i++)
    {
        test_data[i]++;
    }
    ccount++;
    ret = track_sim_backup_msg(test_data, 801, test_callback);
    LOGD(L_CMD, L_V5, "%d", ret);
}

static void test_callback(void * cb)
{
    LOGD(L_CMD, L_V5, "%d", (kal_uint32)cb);

    if((kal_uint32)cb == 0 && autorun == 1)
    {
        track_sim_handle_senderbox(test_read_callback);
    }
}

static void test_read_callback(void * cb)
{
    read_msg_struct *back_ind = (read_msg_struct*)cb;

    LOGD(L_CMD, L_V5, "%d", back_ind->error_msg);

    LOGH(L_CMD, L_V5, back_ind->msg_ctx, back_ind->msg_len);
    if(back_ind->error_msg != 0 && autorun == 0) return;
    if(memcmp(back_ind->msg_ctx, test_data, 801) == 0)
    {
        LOGD(L_CMD, L_V5, "匹配！继续下一循环测试, count:%d", ccount);

        track_os_intoTaskMsgQueue(test1);
    }
    else
    {
        LOGD(L_CMD, L_V5, "不匹配");
    }
}

#if defined(__SAVING_CODE_SPACE__)
#else
void sim_test(int m)
{
    extern kal_bool ishanld ;

    if(m == 90)
    {
        int ret = 0;
        ret = track_sim_backup_msg(test_data, 801, test_callback);
        LOGD(L_CMD, L_V5, "%d", ret);
    }
    else if(m == 91)
    {
        track_sim_handle_senderbox(test_read_callback);
    }
    else if(m == 92)
    {
        /*VX,IMEI,1,353413532150362,8520#*/
        ishanld = KAL_TRUE;
    }
    else if(m == 93)
    {
        ishanld = KAL_FALSE;
    }
    else if(m == 94)
    {
        autorun = 1;
    }
    else if(m == 95)
    {
        autorun = 0;
    }
    else if(m == 96)
    {
          track_cust_module_set_out();
    }
      else if(m == 97)
    {
          track_cust_module_set_in();
    }
}
#endif
#endif
