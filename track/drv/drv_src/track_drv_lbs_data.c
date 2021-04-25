/******************************************************************************
 * track_lbs_data.c -
 *
 * Copyright .
 *
 * DESCRIPTION: -
 *        ��ȡ��վ��Ϣ
 *
 * modification history
 * --------------------
 * v1.0   2012-07-27,  chengjun create this file
 *
 ******************************************************************************/

#if defined (__LBS_DATA__)

/****************************************************************************
* Include Files                         ����ͷ�ļ�
*****************************************************************************/
#include "track_drv_lbs_data.h"
#include "track_os_timer.h"
#include "stack_ltlcom.h"
#include "nbr_public_struct.h"
#include "track_os_ell.h"

/*****************************************************************************
* Typedef  Enum                         ö�ٶ���
*****************************************************************************/

/*****************************************************************************
 *  Struct                              ���ݽṹ����
 *****************************************************************************/


/*****************************************************************************
 *  Define					�궨��
 *****************************************************************************/
#define  CUST_NBR_CELL_INFO
//������������L4C��Ϣ���رպ���ǰ��ģʽ����L4C����Ϣ��������

/*****************************************************************************
* Local variable                        �ֲ�����
*****************************************************************************/
static LBS_Multi_Cell_Data_Struct      Current_Multi_Cell_Info; //���վ����
static LBS_Multi_Cell_Data_Struct      Last_Multi_Cell_Info; //������վ����

/****************************************************************************
* Global Variable                       ȫ�ֱ���
*****************************************************************************/


/****************************************************************************
* Global Variable - Extern              ����ȫ�ֱ���
*****************************************************************************/


/*****************************************************************************
 *  Global Functions    - Extern        �����ⲿ����
 *****************************************************************************/

/*****************************************************************************
 *  Local Functions                     ���غ���
 *****************************************************************************/

static void track_send_cell_infor_req(kal_bool flag)
{
    ilm_struct *ilm_ptr;
    ilm_ptr = (ilm_struct *)allocate_ilm(MOD_MMI);
    ilm_ptr->src_mod_id = MOD_MMI;
    ilm_ptr->dest_mod_id = MOD_L4C;
    ilm_ptr->sap_id = MMI_L4C_SAP;

    if(flag)
    {
        ilm_ptr->msg_id = MSG_ID_L4C_NBR_CELL_INFO_REG_REQ;//start
    }
    else
    {
        ilm_ptr->msg_id = MSG_ID_L4C_NBR_CELL_INFO_DEREG_REQ;//stop
    }

    ilm_ptr->local_para_ptr = NULL;
    ilm_ptr->peer_buff_ptr = NULL;
    msg_send_ext_queue(ilm_ptr);
}

/******************************************************************************
 *  Function    -  tarck_lbs_celll_info_start
 *
 *  Purpose     -  �����ȡ�ڽ�С��
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 17-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void tarck_lbs_celll_info_start()
{
    track_send_cell_infor_req(KAL_TRUE);
}

/******************************************************************************
 *  Function    -  tarck_lbs_celll_info_stop
 *
 *  Purpose     -  ��ͣС����Ϣ�ϱ�
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 17-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void tarck_lbs_celll_info_stop()
{
    Last_Multi_Cell_Info.status = FALSE;
    track_send_cell_infor_req(KAL_FALSE);
}


/******************************************************************************
 *  Function    -  tracki_multi_cell_info_buff
 *
 *  Purpose     -  �����վ��Ϣ
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 17-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void tracki_multi_cell_info_buff(void)
{
    memset(&Last_Multi_Cell_Info, 0,  sizeof(LBS_Multi_Cell_Data_Struct));
    memcpy(&Last_Multi_Cell_Info, &Current_Multi_Cell_Info, sizeof(LBS_Multi_Cell_Data_Struct));
}


#if defined (__LBS_DATA_DEBUG__)

/******************************************************************************
 *  Function    -  track_print_lbs_data_debug
 *
 *  Purpose     -  ���վ��Ϣ���
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 29-02-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
static void track_print_lbs_data_debug(LBS_Multi_Cell_Data_Struct Cell_Info)
{
    char str[200];

    memset(str, 0, sizeof(str));
    snprintf(str, 199, "MCC=%d,MNC=%02d,TA=%d,LAC=%d;Main[%2d,%d];Near {[%2d,%d] [%2d,%d] [%2d,%d] [%2d,%d] [%2d,%d] [%2d,%d]}#",
             Cell_Info.MainCell_Info.mcc, Cell_Info.MainCell_Info.mnc,
             Cell_Info.ta,Cell_Info.MainCell_Info.lac,
             Cell_Info.MainCell_Info.cell_id, Cell_Info.MainCell_Info.rxlev,
             Cell_Info.NbrCell_Info[0].cell_id, Cell_Info.NbrCell_Info[0].rxlev,
             Cell_Info.NbrCell_Info[1].cell_id, Cell_Info.NbrCell_Info[1].rxlev,
             Cell_Info.NbrCell_Info[2].cell_id, Cell_Info.NbrCell_Info[2].rxlev,
             Cell_Info.NbrCell_Info[3].cell_id, Cell_Info.NbrCell_Info[3].rxlev,
             Cell_Info.NbrCell_Info[4].cell_id, Cell_Info.NbrCell_Info[4].rxlev,
             Cell_Info.NbrCell_Info[5].cell_id, Cell_Info.NbrCell_Info[5].rxlev);

    LOGC(L_DRV, L_V7, "%s", str);
}

#endif /* __LBS_DATA_DEBUG__ */



/******************************************************************************
 *  Function    -  track_drv_multi_cell_info_decode
 *
 *  Purpose     -  ���վ��Ϣ����
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 17-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_multi_cell_info_decode(void *info)
{
    int valid_cell;
    int i;

    l4c_nbr_cell_info_ind_struct *nbr_cell_info = (l4c_nbr_cell_info_ind_struct *)info;

    if(nbr_cell_info->ps_nbr_cell_info_union.gas_nbr_cell_info.nbr_cell_num > 6)
    {
        valid_cell = 6;
    }
    else
    {
        valid_cell = nbr_cell_info->ps_nbr_cell_info_union.gas_nbr_cell_info.nbr_cell_num;
    }

    if(nbr_cell_info->is_nbr_info_valid)
    {

        if(nbr_cell_info->ps_nbr_cell_info_union.gas_nbr_cell_info.serv_info.gci.ci != 0) //��С��ID Ϊ0  ����
        {
            memset((void *)&Current_Multi_Cell_Info, 0, sizeof(LBS_Multi_Cell_Data_Struct));

            Current_Multi_Cell_Info.MainCell_Info.mcc = nbr_cell_info->ps_nbr_cell_info_union.gas_nbr_cell_info.serv_info.gci.mcc;
            Current_Multi_Cell_Info.MainCell_Info.mnc = nbr_cell_info->ps_nbr_cell_info_union.gas_nbr_cell_info.serv_info.gci.mnc;
            Current_Multi_Cell_Info.MainCell_Info.lac = nbr_cell_info->ps_nbr_cell_info_union.gas_nbr_cell_info.serv_info.gci.lac;
            Current_Multi_Cell_Info.MainCell_Info.cell_id = nbr_cell_info->ps_nbr_cell_info_union.gas_nbr_cell_info.serv_info.gci.ci;
            Current_Multi_Cell_Info.MainCell_Info.rxlev =
                nbr_cell_info->ps_nbr_cell_info_union.gas_nbr_cell_info.nbr_meas_rslt.nbr_cells[nbr_cell_info->ps_nbr_cell_info_union.gas_nbr_cell_info.serv_info.nbr_meas_rslt_index].rxlev;
            LOGD(5,8,"1MCC:%d,MNC:%d,LAC:%d,CELLID:%d,RXLEV:%d,VALID:%d",Current_Multi_Cell_Info.MainCell_Info.mcc ,Current_Multi_Cell_Info.MainCell_Info.mnc,Current_Multi_Cell_Info.MainCell_Info.lac,Current_Multi_Cell_Info.MainCell_Info.cell_id,Current_Multi_Cell_Info.MainCell_Info.rxlev,valid_cell);
            
            for(i = 0; i < valid_cell; i++)
            {
                Current_Multi_Cell_Info.NbrCell_Info[i].mcc =
                    nbr_cell_info->ps_nbr_cell_info_union.gas_nbr_cell_info.nbr_cell_info[i].gci.mcc;
                Current_Multi_Cell_Info.NbrCell_Info[i].mnc =
                    nbr_cell_info->ps_nbr_cell_info_union.gas_nbr_cell_info.nbr_cell_info[i].gci.mnc;
                Current_Multi_Cell_Info.NbrCell_Info[i].lac =
                    nbr_cell_info->ps_nbr_cell_info_union.gas_nbr_cell_info.nbr_cell_info[i].gci.lac;
                Current_Multi_Cell_Info.NbrCell_Info[i].cell_id =
                    nbr_cell_info->ps_nbr_cell_info_union.gas_nbr_cell_info.nbr_cell_info[i].gci.ci;
                Current_Multi_Cell_Info.NbrCell_Info[i].rxlev =
                    nbr_cell_info->ps_nbr_cell_info_union.gas_nbr_cell_info.nbr_meas_rslt.nbr_cells[nbr_cell_info->ps_nbr_cell_info_union.gas_nbr_cell_info.nbr_cell_info[i].nbr_meas_rslt_index].rxlev;
                LOGD(5,8,"2MCC:%d,MNC:%d,LAC:%d,CELLID:%d,RXLEV:%d",Current_Multi_Cell_Info.NbrCell_Info[i].mcc,Current_Multi_Cell_Info.NbrCell_Info[i].mnc,Current_Multi_Cell_Info.NbrCell_Info[i].lac,Current_Multi_Cell_Info.NbrCell_Info[i].cell_id,Current_Multi_Cell_Info.NbrCell_Info[i].rxlev);
            }

            Current_Multi_Cell_Info.ta = nbr_cell_info->ps_nbr_cell_info_union.gas_nbr_cell_info.ta;
            Current_Multi_Cell_Info.valid_cel_num = valid_cell;

            Current_Multi_Cell_Info.status = TRUE;
            if(Current_Multi_Cell_Info.status == KAL_TRUE)
            {
                LOGD(5,8,"1");
                }
            tracki_multi_cell_info_buff();
        }
    }

#if  defined __LBS_DATA_DEBUG__
    track_print_lbs_data_debug(Current_Multi_Cell_Info);
#endif /* __LBS_DATA_DEBUG__ */

}


/******************************************************************************
 *  Function    -  track_drv_get_lbs_data
 *
 *  Purpose     -  �ⲿ��ȡ���վ��Ϣ�Ľӿ�
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 17-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
LBS_Multi_Cell_Data_Struct * track_drv_get_lbs_data(void)
{
    return &Last_Multi_Cell_Info;
}

/******************************************************************************
 *  Function    -  track_drv_convert_rxlev_to_rssi
 *
 *  Purpose     -  ��վ�źŵȼ���ת��Ϊ�ź�ǿ��
 *
 *  Description - �ź�ǿ��Ϊ����
 -------------------------------------------------------------------------------
        1��AT+CSQ ��ȡ�ĵȼ�(  ��֧��)
 -------------------------------------------------------------------------------
0 -             x <-111     8 -97�Q x <-95            16 -81�Q x <-79                  24 -65�Q x <-63
1 -111�Q x <-109      9 -95�Q x <-93            17 -79�Q x <-77                  25 -63�Q x <-61
2 -109�Q x <-107    10 -93�Q x <-91             18 -77�Q x <-75                 26 -61�Q x <-59
3 -107�Q x <-105    11 -91�Q x <-89             19 -75�Q x <-73                 27 -59�Q x <-57
4 -105�Q x <-103    12 -89�Q x <-87             20 -73�Q x <-71                 28 -57�Q x <-55
5 -103�Q x <-101    13 -87�Q x <-85             21 -71�Q x <-69                 29 -55�Q x <-53
6 -101�Q x <-99      14 -85�Q x <-83             22 -69�Q x <-67                 30 -53�Q x <-51
7 -99�Q x <-97        15 -83�Q x <-81             23 -67�Q x <-65                 31 -51�Q x

*  rssi=(rxlev<<1)-112;
* rxlev=24 ��Ӧ[-65,-63)  ,  ȡ(-64)=2*24-112
 -------------------------------------------------------------------------------
        2��ͨ����Ϣ��ȡС����Ϣ�ĵȼ�
 -------------------------------------------------------------------------------
     if (meas_ptr->serv_cell_rla_struct.rla_value < (-110 * 4))
    {
        nbr_meas_info_ptr->nbr_cells[index].rxlev = 0;
    }
    else if (meas_ptr->serv_cell_rla_struct.rla_value > (-48 * 4))
    {
        nbr_meas_info_ptr->nbr_cells[index].rxlev = 63;
    }
    else
    {
        nbr_meas_info_ptr->nbr_cells[index].rxlev =
            meas_ptr->serv_cell_rla_struct.rla_value/4 + 110;
    }

    rxlev �ǻ�ȡ������ֵ, rla_value/4 ���ź�ǿ��
------------------------------------------------------------------------------------
------------------------------------------------------------------------------------
 * modification history
 * ----------------------------------------
 * v1.0  , 08-03-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_int16 track_drv_convert_rxlev_to_rssi(kal_int16 rxlev)
{
    kal_int16 rssi;

    if(rxlev <= 0)
    {
        rssi = -110;
    }
    else if(rxlev >= 63)
    {
        rssi = -48;
    }
    else
    {
        rssi = rxlev - 110;
    }

    return rssi;
}

/******************************************************************************
 *  Function    -  track_drv_get_service_cell_level
 *
 *  Purpose     -  ��ȡ����С���źŵȼ�
 *
 *  Description -  GSM �ź�ǿ��
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 18-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint8 track_drv_get_service_cell_level(void)
{
    if(Last_Multi_Cell_Info.status)
    {
        return Last_Multi_Cell_Info.MainCell_Info.rxlev;
    }
    else
    {
        return 0xFF;
    }
}


/******************************************************************************
 *  Function    -  track_drv_lbs_info_switch
 *
 *  Purpose     -  ��ȡ��վ��Ϣ����
 *
 *  Description - ���忨Ҳ���Ի�ȡ��ȷ��Ϣ
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 17-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
kal_int8 track_drv_lbs_info_switch(U8 status)
{
    static U8 pre_status = 0xFF;

    if(status == pre_status)
    {
        return -1;//�Ѿ�ִ����
    }
    else if(status == 1)
    {
        tarck_lbs_celll_info_start();
        pre_status = status;
        return 1;
    }
    else if(status == 0)
    {
        tarck_lbs_celll_info_stop();
        pre_status = status;
        return 0;
    }
    else
    {
        return -2;
    }
}

/******************************************************************************
 *  Function    -  track_drv_get_network_info
 *
 *  Purpose     -  ��ʼ��ȡ��վ��Ϣ
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 17-08-2012,  chengjun  written
 * ----------------------------------------
 ******************************************************************************/
void track_drv_get_network_info(void)
{
#if defined (CUST_NBR_CELL_INFO)
    //nc
    /*����ϵͳ�̶����,����Ҫ�������󣬵�Ҳ����ֹͣ  -- chengjun  2013-05-07*/
#else
    track_drv_lbs_info_switch(1);
#endif /* CUST_NBR_CELL_INFO */
    
}

void track_drv_set_lte_lbs_data(LBS_Multi_Cell_Data_Struct lbs)
{
    memset(&Last_Multi_Cell_Info, 0,  sizeof(LBS_Multi_Cell_Data_Struct));
    memset(&Current_Multi_Cell_Info, 0,  sizeof(LBS_Multi_Cell_Data_Struct));
    memcpy(&Current_Multi_Cell_Info, &lbs, sizeof(LBS_Multi_Cell_Data_Struct));
    memcpy(&Last_Multi_Cell_Info, &Current_Multi_Cell_Info, sizeof(LBS_Multi_Cell_Data_Struct));
}
#endif /* __LBS_DATA__ */
