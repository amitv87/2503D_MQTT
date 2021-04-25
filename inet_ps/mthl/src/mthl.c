
//#ifndef M2M_MTHL_TASK_HANDLE
//#define M2M_MTHL_TASK_HANDLE
//#endif

#ifdef M2M_MTHL_TASK_HANDLE
//#include "task_main_func.h"
//#include "kal_general_types.h"
//#include "kal_pubilc_types.h"
//#include "stack_config.h"
#include "mthl.h"
//#include "mthl_atci_struct.h"
//#include "mthl_struct.h"
#include "Uart_sw.h"
#include "Dcl.h"

#ifndef DEBUG_INFO_M2M_MTHL_TASK_HANDLE_ONLY_FOR_IT
#define DEBUG_INFO_M2M_MTHL_TASK_HANDLE_ONLY_FOR_IT   //����ӵ�assert������������ITʹ�ã�
#endif 

//#define MTHL_MAX_IP_SOCKET_NUM      (MAX_IP_SOCKET_NUM -1) //����Ԥ��һ����gethostbyname();


//kal_uint32 g_nwk_acct_id = CBM_INVALID_NWK_ACCT_ID;
#if 0
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif
typedef enum
{
    WIFI_NONE,
    WIFI_ACTIVATE_REQ,
    WIFI_ACTIVATE_CNF,
    WIFI_DEACTIVATE_IND,
    WIFI_DEACTIVATE_REQ,
    WIFI_DEACTIVATE_CNF,
}wifi_bear_op_enum;

typedef struct
{
  wifi_bear_op_enum    wifi_bear_op;
  kal_uint32    user_data;
}mthl_wifi_bearer_info_save_struct;


typedef struct
{
   kal_int8 sock_id;
   kal_uint64 total_send_size;//��ǰ�ܼƷ��͵���data size��
   kal_uint64 Ack_value;//��ǰTCP header��ACKֵ��
   kal_uint64 total_recv_size;//��ǰ�ܼƽ��յ���data size��
}mthl_send_recv_soc_save_struct;

typedef struct
{
  //kal_bool result;//��ѯ�Ƿ�ɹ���
  kal_uint32 account_id;
  kal_uint8 sock_num;
  
  //����Ԥ��һ����gethostbyname();
  sock_account_info_struct socket_info[MTHL_MAX_IP_SOCKET_NUM]; //type, id, ip, port
  mthl_send_recv_soc_save_struct socket_statistics_info[MTHL_MAX_IP_SOCKET_NUM];
  kal_uint32    user_data;
} mthl_create_conn_soc_save_struct;

typedef struct
{ 
   kal_uint8 domain_len;
   kal_char domain_name[128];
   kal_uint32    user_data;
}mthl_get_host_by_name_save_struct;

typedef struct
{ 
   kal_uint16 addr_len;
   kal_uint8 addr[128];
   kal_uint32    user_data;
}mthl_get_host_by_addr_save_struct;
typedef struct
{
	kal_uint32 server_account_id;
	kal_int8   listen_sock_id;
}mthl_server_info;


typedef enum
{
    SRV_DTCNT_DB_TRANS_NONE,
    SRV_DTCNT_DB_TRANS_CREATE,
    SRV_DTCNT_DB_TRANS_FIRST_BOOTUP,
    SRV_DTCNT_DB_TRANS_UPDATE,
    SRV_DTCNT_DB_TRANS_ADD,
    SRV_DTCNT_DB_TRANS_DELETE,
    SRV_DTCNT_DB_TRANS_TOTAL
} srv_dtcnt_db_trans_type_enum;

/* This structure defines MMI CBM service error cause */
typedef enum
{
    SRV_CBM_OK                  = CBM_OK,  /* success */
    SRV_CBM_ERROR               = CBM_ERROR, /* error */
    SRV_CBM_LIMIT_RESOURCE      = CBM_LIMIT_RESOURCE, /* limited resource */
    SRV_CBM_INVALID_ACCT_ID     = CBM_INVALID_ACCT_ID, /* invalid account id*/
    SRV_CBM_INVALID_AP_ID       = CBM_INVALID_AP_ID, /* invalid application id*/
    SRV_CBM_INVALID_SIM_ID      = CBM_INVALID_SIM_ID, /* invalid SIM id */
    SRV_CBM_BEARER_FAIL         = CBM_BEARER_FAIL, /* bearer fail */
    SRV_CBM_DHCP_ERROR          = CBM_DHCP_ERROR, /* DHCP get IP error */
    SRV_CBM_CANCEL_ACT_BEARER   = CBM_CANCEL_ACT_BEARER, /* cancel the account query screen, such as always ask or bearer fallback screen */
    SRV_CBM_DISC_BY_CM          = CBM_DISC_BY_CM /* bearer is deactivated by the connection management */
} srv_cbm_result_error_enum;


/* This enum is used to define bearer type */
typedef enum
{
    SRV_CBM_BEARER_INVALID = CBM_BEARER_NONE, /* invalid bearer type*/
    SRV_CBM_BEARER_CSD = CBM_CSD, /* csd bearer type */
    
    SRV_CBM_BEARER_GPRS = CBM_GPRS, /* gprs bearer type */
    SRV_CBM_BEARER_EDGE = CBM_EDGE, /* edge bearer type */
    SRV_CBM_BEARER_UMTS = CBM_UMTS, /* umts (3G) bearer type */

    SRV_CBM_BEARER_PS = CBM_PS, /* packet service type */
    
    SRV_CBM_BEARER_WIFI = CBM_WIFI, /* wireless lan bearer type */
    
    SRV_CBM_BEARER_DIALUP = CBM_BEARER_TOTAL+1, /* dialup bearer type, it is the interface used by non-phone application */
    SRV_CBM_BEARER_LOCAL = SRV_CBM_BEARER_DIALUP << 1, /* local bearer type, it is the interface used by phone application */
    SRV_CBM_BEARER_TOTAL
} srv_cbm_bearer_type_enum;

/* This enum is used to define bearer status */
typedef enum
{
    SRV_CBM_DEACTIVATED             = CBM_DEACTIVATED, /* bearer deactivated */
    SRV_CBM_ACTIVATING              = CBM_ACTIVATING, /* bearer activating */
    SRV_CBM_ACTIVATED               = CBM_ACTIVATED, /* bearer activated */
    SRV_CBM_DEACTIVATING            = CBM_DEACTIVATING, /* bearer deactivating */
    SRV_CBM_CSD_AUTO_DISC_TIMEOUT   = CBM_CSD_AUTO_DISC_TIMEOUT, /* csd auto disconnection timeout */
    SRV_CBM_GPRS_AUTO_DISC_TIMEOUT  = CBM_GPRS_AUTO_DISC_TIMEOUT, /* gprs auto disconnection timeout */
    SRV_CBM_NWK_NEG_QOS_MODIFY      = CBM_NWK_NEG_QOS_MODIFY, /* negotiated network qos modify notification */
    SRV_CBM_BEARER_STATE_TOTAL
} srv_cbm_bearer_state_enum;


mthl_create_pdp_param_save_struct g_mthl_create_pdp_param_save[MTHL_GPRS_MAX_PDP_NUM];
//wifi_bear_op_enum                 g_mthl_wifi_bear_op = WIFI_NONE;
mthl_wifi_bearer_info_save_struct  g_mthl_wifi_bearer_info_save;

mthl_create_conn_soc_save_struct  g_mthl_create_conn_soc_save[MTHL_GPRS_MAX_PDP_NUM];

//mthl_send_recv_soc_save_struct    g_mthl_send_recv_soc_save[MAX_IP_SOCKET_NUM];

mthl_get_host_by_name_save_struct g_get_host_by_name_save;
mthl_get_host_by_addr_save_struct g_get_host_by_addr_save;



kal_bool g_mthl_task_init_done = KAL_FALSE;

kal_uint8 g_mthl_atci_app_id = CBM_INVALID_APP_ID;
kal_uint32 g_mthl_wifi_nwk_acct_id = CBM_INVALID_NWK_ACCT_ID;

kal_int32 g_request_id_for_gethostbyname = 0;
kal_int32 g_request_id_for_gethostbyaddr = 0;

mthl_server_info g_server_info;

//add for transparent transport mode begin

mthl_transparent_mod_struct mthl_transparent_mode_context;
kal_bool is_mthl_in_transparent_mode = KAL_FALSE;



//add for transparent transport mode end




void mthl_task_main(task_entry_struct *task_entry_ptr);
kal_bool mthl_init(task_indx_type task_indx);
void mthl_main(ilm_struct *ilm_ptr);
kal_bool mthl_reset(task_indx_type task_indx);
void mthl_send_create_pdp_param_cnf(mthl_create_pdp_param_cnf_struct *mthl_create_pdp_param_cnf);
void mthl_send_query_pdp_param_cnf(mthl_query_pdp_param_cnf_struct *mthl_query_pdp_param_cnf);
void mthl_send_activate_pdp_cnf(mthl_activate_pdp_cnf_struct * mthl_activate_pdp_cnf);
#if defined(__WIFI_TCPIP_AT_SUPPORT__)
void mthl_send_activate_wifi_cnf(mthl_wifi_activate_cnf_struct * mthl_wifi_activate_cnf);
#endif
void mthl_send_deactivate_pdp_ind(mthl_deactivate_pdp_ind_struct * mthl_deactivate_pdp_ind);
#if defined(__WIFI_TCPIP_AT_SUPPORT__)
void mthl_send_deactivate_wifi_ind(mthl_wifi_deactivate_ind_struct * mthl_wifi_deactivate_ind);
#endif
void mthl_send_deactivate_pdp_cnf(mthl_deactivate_pdp_cnf_struct * mthl_deactivate_pdp_cnf);
#if defined(__WIFI_TCPIP_AT_SUPPORT__)
void mthl_send_deactivate_wifi_cnf(mthl_wifi_deactivate_cnf_struct * mthl_wifi_deactivate_cnf);
#endif
void mthl_send_query_pdp_state_cnf(mthl_query_pdp_state_cnf_struct *mthl_query_pdp_state_cnf);
#if defined(__WIFI_TCPIP_AT_SUPPORT__)
void mthl_send_query_wifi_state_cnf(mthl_wifi_get_state_cnf_struct *mthl_wifi_get_state_cnf);
#endif
void mthl_send_create_conn_cnf(mthl_create_conn_cnf_struct *mthl_create_conn_cnf);
void mthl_query_sock_num_cnf(mthl_query_sock_account_info_cnf_struct *mthl_query_sock_account_info_cnf);
void mthl_close_sock_cnf(mthl_close_sock_cnf_struct *mthl_close_cnf);
void mthl_close_sock_ind(mthl_close_sock_ind_struct *mthl_close_ind);
void mthl_send_sock_cnf(mthl_send_data_cnf_struct *mthl_send_data_cnf);
void mthl_send_sock_ind(mthl_send_data_ind_struct *mthl_send_data_ind);
void mthl_recv_sock_cnf(mthl_recv_data_cnf_struct *mthl_recv_data_cnf);
void mthl_query_info_sock_cnf(mthl_query_sock_info_cnf_struct *mthl_query_sock_info_cnf);
void mthl_get_host_by_name_soc_cnf(mthl_get_host_by_name_cnf_struct *mthl_get_host_by_name_cnf);
void mthl_get_host_by_addr_soc_cnf(mthl_get_host_by_addr_cnf_struct *mthl_get_host_by_addr_cnf);
void mthl_tcp_server_listen_cnf(mthl_server_listen_cnf_struct *mthl_server_listen_cnf);
void mthl_tcp_server_accept_ind(mthl_server_accept_ind_struct *mthl_server_accept_ind);


//void mthl_send_msg_to_abm(kal_uint16 msg_id, void *req, int mod_dst);
//void mthl_prof_query_ind_hdlr(void *info);
kal_int8 mthl_cbm_release_bearer(kal_uint32 ori_acct_id);
void mthl_send_abm_acc_info(srv_dtcnt_db_trans_type_enum opcode, kal_uint32 account_id, cbm_bearer_enum bearer_type);
void mthl_send_msg2atci(msg_type msg_id, 
                         module_type dst_mod_id, 
                         sap_type      sap_id,
                         local_para_struct *p_local_para);
void mthl_soc_notify(void *inMsg);
void mthl_soc_get_host_by_name_ind_hdlr(app_soc_get_host_by_name_ind_struct *p);
void mthl_soc_get_host_by_addr_ind_hdlr(app_soc_get_host_by_addr_ind_struct* get_host_by_addr_ind);


static void mthl_transparent_reveive_data(void);
static void mthl_transparent_uart_ready_to_read_ind_hdlr(void);
//static void mthl_process_output (void);
mthl_transport_buff_head_struct* mthl_transparent_mem_alloc(void);                    
static void mthl_transparent_send_end_ind(kal_int8 port , transparent_error_cause_enum cause);
void mthl_transparent_ready_to_write_uart_ind_hdlr(void);



kal_bool mthl_create(comptask_handler_struct **handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    static const comptask_handler_struct mthl_handler_info = 
    {
        mthl_task_main,  /* task entry function */
        mthl_init,       /* task initialization function */
        NULL,           /* task configuration function */
        mthl_reset,      /* task reset handler */
        NULL,           /* task termination handler */
    };

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *handle = (comptask_handler_struct*) & mthl_handler_info;
    return KAL_TRUE;
}


void mthl_task_main(task_entry_struct *task_entry_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct current_ilm;
    kal_uint32 my_index = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_get_my_task_index(&my_index);

    while (1)
    {
        //receive_msg_ext_q(task_info_g[task_entry_ptr->task_indx].task_ext_qid, &current_ilm);
		receive_msg_ext_q_for_stack(task_info_g[task_entry_ptr->task_indx].task_ext_qid, &current_ilm);
        stack_set_active_module_id(my_index, current_ilm.dest_mod_id);

        mthl_main((void*)&current_ilm);

        free_ilm(&current_ilm);

    }
}


kal_bool mthl_init(task_indx_type task_indx)
{
    int i,j;
    
	//init save global;
	memset(g_mthl_create_pdp_param_save, 0x00, sizeof(mthl_create_pdp_param_save_struct)*MTHL_GPRS_MAX_PDP_NUM);
	for(i =0 ;i< MTHL_GPRS_MAX_PDP_NUM;i++)
	{
		 g_mthl_create_pdp_param_save[i].account_id = CBM_INVALID_NWK_ACCT_ID;
	}

	memset(g_mthl_create_conn_soc_save, 0x00, sizeof(mthl_create_conn_soc_save_struct)*MTHL_GPRS_MAX_PDP_NUM);
	for(i =0 ;i< MTHL_GPRS_MAX_PDP_NUM;i++)
	{
		 g_mthl_create_conn_soc_save[i].account_id = CBM_INVALID_NWK_ACCT_ID;

		 for(j=0;j<MTHL_MAX_IP_SOCKET_NUM;j++)
		 {
		     g_mthl_create_conn_soc_save[i].socket_info[j].sock_id = SOC_ERROR;
			 g_mthl_create_conn_soc_save[i].socket_statistics_info[j].sock_id = SOC_ERROR;
		 }	 	
	}

	memset(&g_get_host_by_name_save, 0x00, sizeof(mthl_get_host_by_name_save_struct));
	memset(&g_get_host_by_addr_save, 0x00, sizeof(mthl_get_host_by_addr_save_struct));
	g_server_info.server_account_id = CBM_INVALID_NWK_ACCT_ID;
	g_server_info.listen_sock_id    = SOC_ERROR;

	g_mthl_wifi_bearer_info_save.wifi_bear_op = WIFI_NONE;
	g_mthl_wifi_bearer_info_save.user_data    = 0;
	g_mthl_task_init_done = KAL_TRUE;
		
    return KAL_TRUE;
}

kal_bool mthl_reset(task_indx_type task_indx)
{
    return KAL_TRUE;
}



//handle API:

//�����ڹػ���Ҫ��������ݣ�
void mthl_send_create_pdp_param_req_hdlr(ilm_struct *ilm_ptr)
{ 
            int i;
			kal_int8 result;
	 		mthl_create_pdp_param_req_struct   *mthl_create_pdp_param_req;  
			mthl_create_pdp_param_cnf_struct   mthl_create_pdp_param_cnf;
			
			mthl_create_pdp_param_req = (mthl_create_pdp_param_req_struct*)ilm_ptr->local_para_ptr;
			memset(&mthl_create_pdp_param_cnf, 0x00, sizeof(mthl_create_pdp_param_cnf_struct));

			if(mthl_create_pdp_param_req->account_id < 0
				|| mthl_create_pdp_param_req->account_id == CBM_INVALID_NWK_ACCT_ID
				|| mthl_create_pdp_param_req->apn_length == 0 
				|| mthl_create_pdp_param_req->pdp_type == NULL_PDP_ADDR_TYPE
				|| (mthl_create_pdp_param_req->proxy_addr[0] == 0 && mthl_create_pdp_param_req->proxy_port > 0)
				|| (mthl_create_pdp_param_req->proxy_addr[0] != 0 && mthl_create_pdp_param_req->proxy_port == 0)
				)
			{
			    mthl_create_pdp_param_cnf.result = PARAM_INVALID;
                mthl_create_pdp_param_cnf.account_id = mthl_create_pdp_param_req->account_id;
				mthl_create_pdp_param_cnf.user_data  = mthl_create_pdp_param_req->user_data;//ԭ�ⲻ������;
				
				//����cnf;
				mthl_send_create_pdp_param_cnf(&mthl_create_pdp_param_cnf);

				kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): PARAM_INVALID case.");
				return;
			}

            if(g_mthl_task_init_done == KAL_TRUE)
            {
					//cbm_register_app_id_with_app_info(&info, &app_id); ����src_mod������һ��for ATCI��app_id;
					cbm_app_info_struct info;
					//kal_uint8 app_id;
					info.app_str_id = 0;//MOD_L4C;
					info.app_icon_id = 0;
					info.app_type = DTCNT_APPTYPE_NONE;
					
					result = cbm_register_app_id_with_app_info(&info, &g_mthl_atci_app_id); 

					if(result ==CBM_OK)
					    kal_prompt_trace(MOD_MTHL, "mthl_init(): g_mthl_atci_app_id = %d.", g_mthl_atci_app_id);
					else
					    kal_prompt_trace(MOD_MTHL, "mthl_init(): g_mthl_atci_app_id error, result = %d.", result);

			
					g_mthl_wifi_nwk_acct_id = cbm_encode_data_account_id(CBM_WIFI_ACCT_ID, CBM_SIM_ID_SIM1, g_mthl_atci_app_id, KAL_FALSE);
				    if(g_mthl_wifi_nwk_acct_id == CBM_INVALID_NWK_ACCT_ID)
						kal_prompt_trace(MOD_MTHL, "mthl_init(): g_mthl_wifi_nwk_acct_id is invalid.");

					g_mthl_task_init_done = KAL_FALSE;
			}

            //���ж��Ƿ�Ϊ����account_id��ز����Ĳ�����
            for(i =0 ;i< MTHL_GPRS_MAX_PDP_NUM;i++)
            {
	            //Update param;
				if(g_mthl_create_pdp_param_save[i].account_id == mthl_create_pdp_param_req->account_id)
				{
				    kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): match acct_id, update param case, param:");
					
				    //ͨ��ori_acct_id���浱ǰacct_id��Ӧ������������MSG�������ģ���
					g_mthl_create_pdp_param_save[i].account_id = mthl_create_pdp_param_req->account_id;
					kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): g_mthl_create_pdp_param_save[%d].account_id = %d.", i,g_mthl_create_pdp_param_save[i].account_id);
					
					g_mthl_create_pdp_param_save[i].nwk_acct_id= cbm_encode_data_account_id(mthl_create_pdp_param_req->account_id, CBM_SIM_ID_SIM1 , g_mthl_atci_app_id, KAL_FALSE);
					kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): g_mthl_create_pdp_param_save[%d].nwk_acct_id = %d.", i,g_mthl_create_pdp_param_save[i].nwk_acct_id);
					
					g_mthl_create_pdp_param_save[i].apn_length = mthl_create_pdp_param_req->apn_length;
					kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): g_mthl_create_pdp_param_save[%d].apn_length = %d.", i,g_mthl_create_pdp_param_save[i].apn_length);
					
					strncpy(g_mthl_create_pdp_param_save[i].apn, mthl_create_pdp_param_req->apn, g_mthl_create_pdp_param_save[i].apn_length);
					kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): g_mthl_create_pdp_param_save[%d].apn = %s.", i,g_mthl_create_pdp_param_save[i].apn);
					
					g_mthl_create_pdp_param_save[i].pdp_type = mthl_create_pdp_param_req->pdp_type;
					kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): g_mthl_create_pdp_param_save[%d].pdp_type = %d.", i,g_mthl_create_pdp_param_save[i].pdp_type);

					if(mthl_create_pdp_param_req->proxy_port != 0)
					{
					    g_mthl_create_pdp_param_save[i].proxy_flags = 1;
						g_mthl_create_pdp_param_save[i].proxy_port = mthl_create_pdp_param_req->proxy_port;
						g_mthl_create_pdp_param_save[i].proxy_addr[0] = mthl_create_pdp_param_req->proxy_addr[0];
						g_mthl_create_pdp_param_save[i].proxy_addr[1] = mthl_create_pdp_param_req->proxy_addr[1];
						g_mthl_create_pdp_param_save[i].proxy_addr[2] = mthl_create_pdp_param_req->proxy_addr[2];
						g_mthl_create_pdp_param_save[i].proxy_addr[3] = mthl_create_pdp_param_req->proxy_addr[3];
						kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): use proxy.");
					}
					else
					{
						g_mthl_create_pdp_param_save[i].proxy_flags = 0;
						kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): no use proxy.");
					}

					break;  
				}
				else
			   	   continue;
            }

            //match acct_id, update param�����;
			if(i < MTHL_GPRS_MAX_PDP_NUM)
			{
			    //����cnf SUCCESS֮ǰ����Ҫupdate acc_info;
	            //mthl_send_abm_acc_info(SRV_DTCNT_DB_TRANS_UPDATE, mthl_create_pdp_param_req->account_id, CBM_PS);
	            mthl_create_pdp_param_cnf.result     = SUCCESS;
				mthl_create_pdp_param_cnf.account_id = mthl_create_pdp_param_req->account_id;
				mthl_create_pdp_param_cnf.user_data  = mthl_create_pdp_param_req->user_data;//ԭ�ⲻ������;
				mthl_send_create_pdp_param_cnf(&mthl_create_pdp_param_cnf);

				kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): match acct_id, update param done.");
				return;
			}

			//not match acct_id, ��:�����һ��param;
			//mthl task save param; 
			for(i =0 ;i< MTHL_GPRS_MAX_PDP_NUM;i++)
			{
			   //Add param;
			   if(g_mthl_create_pdp_param_save[i].account_id == CBM_INVALID_NWK_ACCT_ID)
			   {
			        kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): Add param case, param:");
					
			        //ͨ��ori_acct_id���浱ǰacct_id��Ӧ������������MSG�������ģ���
					g_mthl_create_pdp_param_save[i].account_id = mthl_create_pdp_param_req->account_id;
					kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): g_mthl_create_pdp_param_save[%d].account_id = %d.", i,g_mthl_create_pdp_param_save[i].account_id);
					
					g_mthl_create_pdp_param_save[i].nwk_acct_id= cbm_encode_data_account_id(mthl_create_pdp_param_req->account_id, CBM_SIM_ID_SIM1 , g_mthl_atci_app_id, KAL_FALSE);
					kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): g_mthl_create_pdp_param_save[%d].nwk_acct_id = %d.", i,g_mthl_create_pdp_param_save[i].nwk_acct_id);
					
					g_mthl_create_pdp_param_save[i].apn_length = mthl_create_pdp_param_req->apn_length;
					kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): g_mthl_create_pdp_param_save[%d].apn_length = %d.", i,g_mthl_create_pdp_param_save[i].apn_length);
					
					strncpy(g_mthl_create_pdp_param_save[i].apn, mthl_create_pdp_param_req->apn, g_mthl_create_pdp_param_save[i].apn_length);
					kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): g_mthl_create_pdp_param_save[%d].apn = %s.", i,g_mthl_create_pdp_param_save[i].apn);
					
					g_mthl_create_pdp_param_save[i].pdp_type = mthl_create_pdp_param_req->pdp_type;
                    kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): g_mthl_create_pdp_param_save[%d].pdp_type = %d.", i,g_mthl_create_pdp_param_save[i].pdp_type);
					
					if(mthl_create_pdp_param_req->proxy_port != 0)
					{
					    g_mthl_create_pdp_param_save[i].proxy_flags = 1;
						g_mthl_create_pdp_param_save[i].proxy_port = mthl_create_pdp_param_req->proxy_port;
						g_mthl_create_pdp_param_save[i].proxy_addr[0] = mthl_create_pdp_param_req->proxy_addr[0];
						g_mthl_create_pdp_param_save[i].proxy_addr[1] = mthl_create_pdp_param_req->proxy_addr[1];
						g_mthl_create_pdp_param_save[i].proxy_addr[2] = mthl_create_pdp_param_req->proxy_addr[2];
						g_mthl_create_pdp_param_save[i].proxy_addr[3] = mthl_create_pdp_param_req->proxy_addr[3];
						kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): use proxy.");
					}
					else
					{
						g_mthl_create_pdp_param_save[i].proxy_flags = 0;
					    kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): no use proxy.");
					}

					break;
			   }
			   else
			   	   continue;
			}

			if(i == MTHL_GPRS_MAX_PDP_NUM)
			{
			    mthl_create_pdp_param_cnf.result = LIMITED_RES;
				mthl_create_pdp_param_cnf.account_id = mthl_create_pdp_param_req->account_id;
				mthl_create_pdp_param_cnf.user_data  = mthl_create_pdp_param_req->user_data;//ԭ�ⲻ������;
				
				//����cnf;
				mthl_send_create_pdp_param_cnf(&mthl_create_pdp_param_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): LIMITED_RES case.");
				return;
			}

            //����cnf SUCCESS֮ǰ����Ҫupdate acc_info;
            mthl_send_abm_acc_info(SRV_DTCNT_DB_TRANS_ADD, mthl_create_pdp_param_req->account_id, CBM_PS);
            mthl_create_pdp_param_cnf.result     = SUCCESS;
			mthl_create_pdp_param_cnf.account_id = mthl_create_pdp_param_req->account_id;
			mthl_create_pdp_param_cnf.user_data  = mthl_create_pdp_param_req->user_data;//ԭ�ⲻ������;
			mthl_send_create_pdp_param_cnf(&mthl_create_pdp_param_cnf);
            kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): SUCCESS case.");
			
			return;
    
}

void mthl_get_pdp_param_req_hdlr(ilm_struct *ilm_ptr)
{
            int i;
	 		mthl_query_pdp_param_req_struct   *mthl_query_pdp_param_req;  
			mthl_query_pdp_param_cnf_struct    mthl_query_pdp_param_cnf;
			
			mthl_query_pdp_param_req = (mthl_query_pdp_param_req_struct*)ilm_ptr->local_para_ptr;
			memset(&mthl_query_pdp_param_cnf, 0x00, sizeof(mthl_query_pdp_param_cnf_struct));

			//query the param of mthl task save ; 
			for(i =0 ;i< MTHL_GPRS_MAX_PDP_NUM;i++)
			{
			   //find valid account_id;
			   if(g_mthl_create_pdp_param_save[i].account_id != CBM_INVALID_NWK_ACCT_ID)
			   {
			        kal_prompt_trace(MOD_MTHL, "mthl_get_pdp_param_req_hdlr(): query param OK case, param:");
					
			        //��ѯ��һ��account_id��Ч����Ӧ�Ĳ��������ǳɹ���
					mthl_query_pdp_param_cnf.result = KAL_TRUE;
					mthl_query_pdp_param_cnf.pdp_param_num++;
					
					mthl_query_pdp_param_cnf.pdp_param_info[i].account_id = g_mthl_create_pdp_param_save[i].account_id;
					kal_prompt_trace(MOD_MTHL, "mthl_get_pdp_param_req_hdlr(): g_mthl_create_pdp_param_save[%d].account_id = %d.", i,g_mthl_create_pdp_param_save[i].account_id);

					//g_mthl_create_pdp_param_save[i].nwk_acct_id;
					//kal_prompt_trace(MOD_MTHL, "mthl_send_create_pdp_param_req_hdlr(): g_mthl_create_pdp_param_save[%d].nwk_acct_id = %d.", i,g_mthl_create_pdp_param_save[i].nwk_acct_id);

					mthl_query_pdp_param_cnf.pdp_param_info[i].apn_length = g_mthl_create_pdp_param_save[i].apn_length;
					kal_prompt_trace(MOD_MTHL, "mthl_get_pdp_param_req_hdlr(): g_mthl_create_pdp_param_save[%d].apn_length = %d.", i,g_mthl_create_pdp_param_save[i].apn_length);
					
					strncpy(mthl_query_pdp_param_cnf.pdp_param_info[i].apn, g_mthl_create_pdp_param_save[i].apn, g_mthl_create_pdp_param_save[i].apn_length);
					kal_prompt_trace(MOD_MTHL, "mthl_get_pdp_param_req_hdlr(): g_mthl_create_pdp_param_save[%d].apn = %s.", i,g_mthl_create_pdp_param_save[i].apn);

					mthl_query_pdp_param_cnf.pdp_param_info[i].pdp_type = g_mthl_create_pdp_param_save[i].pdp_type;
                    kal_prompt_trace(MOD_MTHL, "mthl_get_pdp_param_req_hdlr(): g_mthl_create_pdp_param_save[%d].pdp_type = %d.", i,g_mthl_create_pdp_param_save[i].pdp_type);
					
					if(g_mthl_create_pdp_param_save[i].proxy_port != 0)
					{
						mthl_query_pdp_param_cnf.pdp_param_info[i].proxy_port = g_mthl_create_pdp_param_save[i].proxy_port;
                        mthl_query_pdp_param_cnf.pdp_param_info[i].proxy_addr[0] = g_mthl_create_pdp_param_save[i].proxy_addr[0];
						mthl_query_pdp_param_cnf.pdp_param_info[i].proxy_addr[1] = g_mthl_create_pdp_param_save[i].proxy_addr[1];
						mthl_query_pdp_param_cnf.pdp_param_info[i].proxy_addr[2] = g_mthl_create_pdp_param_save[i].proxy_addr[2];
						mthl_query_pdp_param_cnf.pdp_param_info[i].proxy_addr[3] = g_mthl_create_pdp_param_save[i].proxy_addr[3];
						kal_prompt_trace(MOD_MTHL, "mthl_get_pdp_param_req_hdlr(): use proxy case.");
					}
					else
					{
					    kal_prompt_trace(MOD_MTHL, "mthl_get_pdp_param_req_hdlr(): no use proxy case.");
					}
                    //��ѯ��һ����Ч�ģ���Ҫ������ѯ��
					continue;
			   }
			   else
			   	   continue;
			}

			//����֮��not find; 
			if(mthl_query_pdp_param_cnf.pdp_param_num == 0)
			{
			    mthl_query_pdp_param_cnf.result = KAL_FALSE;
				//mthl_query_pdp_param_cnf.pdp_param_num++; //0
				mthl_query_pdp_param_cnf.user_data  = mthl_query_pdp_param_req->user_data;//ԭ�ⲻ������;
				
				//����cnf;
				mthl_send_query_pdp_param_cnf(&mthl_query_pdp_param_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_get_pdp_param_req_hdlr(): query param fail case.");
				return;
			}

            //find valid account_id;
			mthl_query_pdp_param_cnf.user_data  = mthl_query_pdp_param_req->user_data;//ԭ�ⲻ������;
			mthl_send_query_pdp_param_cnf(&mthl_query_pdp_param_cnf);
            kal_prompt_trace(MOD_MTHL, "mthl_get_pdp_param_req_hdlr(): SUCCESS case.");    

            return;
}
void mthl_send_activate_pdp_req_hdlr(ilm_struct *ilm_ptr)
{ 
            int i;
			cbm_result_error_enum ret;
			kal_int32 return_val;
			
            mthl_activate_pdp_req_struct   *mthl_activate_pdp_req;  
			mthl_activate_pdp_cnf_struct   mthl_activate_pdp_cnf;
			
			mthl_activate_pdp_req = (mthl_activate_pdp_req_struct*)ilm_ptr->local_para_ptr;
			memset(&mthl_activate_pdp_cnf, 0x00, sizeof(mthl_activate_pdp_cnf_struct));

			for(i =0 ;i< MTHL_GPRS_MAX_PDP_NUM;i++)
			{
			    if(mthl_activate_pdp_req->account_id == g_mthl_create_pdp_param_save[i].account_id)
			    {
			        kal_prompt_trace(MOD_MTHL, "mthl_send_activate_pdp_req_hdlr(): match acct_id, param:");
					
			        //ori_acct_idƥ��ɹ�������auth info;
			        g_mthl_create_pdp_param_save[i].Auth_info.AuthType = mthl_activate_pdp_req->Auth_info.AuthType;
					kal_prompt_trace(MOD_MTHL, "mthl_send_activate_pdp_req_hdlr(): g_mthl_create_pdp_param_save[%d].Auth_info.AuthType = %d", i,g_mthl_create_pdp_param_save[i].Auth_info.AuthType);
					
					kal_mem_cpy(g_mthl_create_pdp_param_save[i].Auth_info.UserName, mthl_activate_pdp_req->Auth_info.UserName, CUSTOM_DTCNT_PROF_MAX_USER_LEN);
					kal_prompt_trace(MOD_MTHL, "mthl_send_activate_pdp_req_hdlr(): g_mthl_create_pdp_param_save[%d].Auth_info.UserName = %s", i,g_mthl_create_pdp_param_save[i].Auth_info.UserName);
					
					kal_mem_cpy(g_mthl_create_pdp_param_save[i].Auth_info.Passwd, mthl_activate_pdp_req->Auth_info.Passwd, CUSTOM_DTCNT_PROF_MAX_USER_LEN);
					kal_prompt_trace(MOD_MTHL, "mthl_send_activate_pdp_req_hdlr(): g_mthl_create_pdp_param_save[%d].Auth_info.Passwd = %s", i,g_mthl_create_pdp_param_save[i].Auth_info.Passwd);
					
					break;
			    }
				else
					continue;
			}

            //account_idƥ��ʧ�ܣ�
			if(i == MTHL_GPRS_MAX_PDP_NUM)
			{
			    mthl_activate_pdp_cnf.account_id = mthl_activate_pdp_req->account_id;
				mthl_activate_pdp_cnf.result = KAL_FALSE;
				mthl_activate_pdp_cnf.error_cause = 0;
				mthl_activate_pdp_cnf.user_data = mthl_activate_pdp_req->user_data;
				
				//����cnf;
				mthl_send_activate_pdp_cnf(&mthl_activate_pdp_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_send_activate_pdp_req_hdlr(): not match acct_id.");
				//g_mthl_create_pdp_param_save[i].bear_op = PDP_ACTIVATE_CNF;
				g_mthl_create_pdp_param_save[i].bear_op = PDP_NONE; //����ʧ�ܣ�
				return;
			}
			
			//ͨ��ori_acct_id�ҵ�g_nwk_acct_id��
			//����srv_cbm_register_bearer_info()������;
		    /*srv_cbm_register_bearer_info(
		            app_id,
		            SRV_CBM_ACTIVATING | SRV_CBM_ACTIVATED | SRV_CBM_DEACTIVATED,
		            SRV_CBM_BEARER_CSD | SRV_CBM_BEARER_PS,
		            srv_nwk_bearer_info_ind,
		            NULL);*/
		            
			return_val = cbm_register_bearer_info_by_app_id(
                        g_mthl_atci_app_id,
                        MOD_MTHL,
                        SRV_CBM_BEARER_CSD | SRV_CBM_BEARER_PS,
                        SRV_CBM_ACTIVATING | SRV_CBM_ACTIVATED | SRV_CBM_DEACTIVATED);
			
            //register_bearer_info IND����ʧ�ܣ�
			if(CBM_OK != return_val)
			{
			    mthl_activate_pdp_cnf.account_id = g_mthl_create_pdp_param_save[i].account_id;
				mthl_activate_pdp_cnf.result = KAL_FALSE;
				mthl_activate_pdp_cnf.error_cause = 0;
				mthl_activate_pdp_cnf.user_data = mthl_activate_pdp_req->user_data;
				
			     //����cnf;
			    mthl_send_activate_pdp_cnf(&mthl_activate_pdp_cnf);  
				kal_prompt_trace(MOD_MTHL, "mthl_send_activate_pdp_req_hdlr(): register_bearer_info fail.");
				
				g_mthl_create_pdp_param_save[i].bear_op = PDP_NONE;//����ʧ�ܣ�
				return;
			}
			
			ret = cbm_open_bearer(g_mthl_create_pdp_param_save[i].nwk_acct_id);

			//��¼��ǰaccount_id���Ǽ��������
			g_mthl_create_pdp_param_save[i].bear_op = PDP_ACTIVATE_REQ;

            //�������سɹ���ʧ�ܣ�
			if(ret != CBM_WOULDBLOCK)
			{
			    mthl_activate_pdp_cnf.account_id = g_mthl_create_pdp_param_save[i].account_id;
				if(ret == CBM_OK)
				{
		           cbm_hold_bearer(g_mthl_atci_app_id);
				   mthl_activate_pdp_cnf.result = KAL_TRUE;
				   g_mthl_create_pdp_param_save[i].bear_op = PDP_ACTIVATE_CNF;//����ɹ���
				}
				else
				{
				   mthl_activate_pdp_cnf.result = KAL_FALSE;
				   g_mthl_create_pdp_param_save[i].bear_op = PDP_NONE;//����ʧ�ܣ�
				}
				mthl_activate_pdp_cnf.error_cause = 0;
				mthl_activate_pdp_cnf.user_data = mthl_activate_pdp_req->user_data;
				
				//����cnf;
				mthl_send_activate_pdp_cnf(&mthl_activate_pdp_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_send_activate_pdp_req_hdlr(): open_bearer = %d.", ret);
				
				//g_mthl_create_pdp_param_save[i].bear_op = PDP_ACTIVATE_CNF;
				return;
			}
			
			//WOULDBLOCK: IND�ϱ��󣬿�֪bearer�Ƿ񼤻�ɹ���		
			//���崦��ABM��INIT/QUERY��Ϣ;��apn/ auth/usrname/passwd����ȥ��
			kal_prompt_trace(MOD_MTHL, "activate_pdp: WOULDBLOCK case.");
			g_mthl_create_pdp_param_save[i].user_data = mthl_activate_pdp_req->user_data;
			return;
			
}

#if defined(__WIFI_TCPIP_AT_SUPPORT__)
void mthl_send_activate_wifi_req_hdlr(ilm_struct *ilm_ptr)
{ 
				int i;
				cbm_result_error_enum ret;
				kal_int32 return_val;
				kal_int8 result;
				cbm_app_info_struct info = {0};

				kal_int32 wlan_status;
    			srv_dtcnt_wlan_state_enum wlan_state;
	
				mthl_wifi_activate_req_struct   *mthl_wifi_activate_req;	
				mthl_wifi_activate_cnf_struct   mthl_wifi_activate_cnf;
				
				mthl_wifi_activate_req = (mthl_wifi_activate_req_struct*)ilm_ptr->local_para_ptr;
				kal_prompt_trace(MOD_MTHL, "mthl_send_activate_wifi_req_hdlr() param:");
				kal_prompt_trace(MOD_MTHL, "mthl_send_activate_wifi_req_hdlr() mthl_wifi_activate_req->user_data :%d",mthl_wifi_activate_req->user_data);
				memset(&mthl_wifi_activate_cnf, 0x00, sizeof(mthl_wifi_activate_cnf_struct));

                //��ǰ�Ǵ���һ��WIFI_ACTIVATE_REQ��
				g_mthl_wifi_bearer_info_save.wifi_bear_op = WIFI_ACTIVATE_REQ;

				//for WIFI case, need to register_app_id;		
			    if(g_mthl_task_init_done == KAL_TRUE)
			   	{
					info.app_str_id = 0;//MOD_L4C;
					info.app_icon_id = 0;
					info.app_type = DTCNT_APPTYPE_NONE;
					
					result = cbm_register_app_id_with_app_info(&info, &g_mthl_atci_app_id); 

					if(result ==CBM_OK)
					{
					    kal_prompt_trace(MOD_MTHL, "mthl_init(): g_mthl_atci_app_id = %d.", g_mthl_atci_app_id);

						g_mthl_wifi_nwk_acct_id = cbm_encode_data_account_id(CBM_WIFI_ACCT_ID, CBM_SIM_ID_SIM1, g_mthl_atci_app_id, KAL_FALSE);
						
					    if(g_mthl_wifi_nwk_acct_id == CBM_INVALID_NWK_ACCT_ID)
							kal_prompt_trace(MOD_MTHL, "mthl_init(): g_mthl_wifi_nwk_acct_id is invalid.");
						else
							kal_prompt_trace(MOD_MTHL, "mthl_init(): g_mthl_wifi_nwk_acct_id = %x.", g_mthl_wifi_nwk_acct_id);
					}
					else
					{
					    kal_prompt_trace(MOD_MTHL, "mthl_init(): g_mthl_atci_app_id error, result = %d.", result);
						
						mthl_wifi_activate_cnf.result = KAL_FALSE;
						mthl_wifi_activate_cnf.user_data = mthl_wifi_activate_req->user_data;
						
						//����cnf;
						mthl_send_activate_wifi_cnf(&mthl_wifi_activate_cnf);
						kal_prompt_trace(MOD_MTHL, "mthl_send_activate_pdp_req_hdlr(): register_app_id fail.");
						
						g_mthl_wifi_bearer_info_save.wifi_bear_op = WIFI_NONE; //����ʧ�ܣ�
						return;
					}

					g_mthl_task_init_done = KAL_FALSE;
			    }
	            /* Check the WLAN state */
    			wlan_state = srv_dtcnt_wlan_state();
				kal_prompt_trace(MOD_MTHL, "mthl_send_activate_wifi_req_hdlr() wlan_state = %d", wlan_state);

				/* If the WLAN is turned off , send the response directly ���޷�����*/
				//WIFIû��������
			    if (wlan_state == SRV_DTCNT_WLAN_STATE_NULL ||
			        wlan_state == SRV_DTCNT_WLAN_STATE_TOTAL)
			    {
					mthl_wifi_activate_cnf.result = KAL_FALSE;
					mthl_wifi_activate_cnf.user_data = mthl_wifi_activate_req->user_data;
					
					//����cnf;
					mthl_send_activate_wifi_cnf(&mthl_wifi_activate_cnf);
					kal_prompt_trace(MOD_MTHL, "mthl_send_activate_wifi_req_hdlr(): WLAN is turned off.");
					//g_mthl_create_pdp_param_save[i].bear_op = PDP_ACTIVATE_CNF;
					g_mthl_wifi_bearer_info_save.wifi_bear_op = WIFI_NONE; //����ʧ�ܣ�
					return;
				}
                

			    /* Start to query the WLAN status */
			    wlan_status = abm_get_bearer_available_status(CBM_BEARER_WIFI, CBM_SIM_ID_SIM1);

	            /* If the WLAN is available, send the response directly ���Ѿ�����*/
			    if (wlan_state == SRV_DTCNT_WLAN_STATE_CONNECTED &&
			        wlan_status == CBM_ACTIVATED)
				{
					mthl_wifi_activate_cnf.result = KAL_TRUE;
					mthl_wifi_activate_cnf.user_data = mthl_wifi_activate_req->user_data;
					
					//����cnf;
					mthl_send_activate_wifi_cnf(&mthl_wifi_activate_cnf);
					kal_prompt_trace(MOD_MTHL, "mthl_send_activate_wifi_req_hdlr(): WLAN is available, is activated.");
					//g_mthl_create_pdp_param_save[i].bear_op = PDP_ACTIVATE_CNF;
					g_mthl_wifi_bearer_info_save.wifi_bear_op = WIFI_ACTIVATE_CNF; //����ɹ���
					return;
				}
				
				kal_prompt_trace(MOD_MTHL, "mthl_send_activate_wifi_req_hdlr(): WLAN is turned on, need to be activated.");
				
				//ͨ��ori_acct_id�ҵ�g_nwk_acct_id��
				//����srv_cbm_register_bearer_info()������;
				/*srv_cbm_register_bearer_info(
						app_id,
						SRV_CBM_ACTIVATING | SRV_CBM_ACTIVATED | SRV_CBM_DEACTIVATED,
						SRV_CBM_BEARER_CSD | SRV_CBM_BEARER_PS,
						srv_nwk_bearer_info_ind,
						NULL);*/
						
				return_val = cbm_register_bearer_info_by_app_id(
							g_mthl_atci_app_id,
							MOD_MTHL,
							//SRV_CBM_BEARER_CSD | SRV_CBM_BEARER_PS,
							SRV_CBM_BEARER_WIFI,
							SRV_CBM_ACTIVATING | SRV_CBM_ACTIVATED | SRV_CBM_DEACTIVATED);
				
				//register_bearer_info IND����ʧ�ܣ�
				if(CBM_OK != return_val)
				{
					mthl_wifi_activate_cnf.result = KAL_FALSE;
					mthl_wifi_activate_cnf.user_data = mthl_wifi_activate_req->user_data;
					
					//����cnf;
					mthl_send_activate_wifi_cnf(&mthl_wifi_activate_cnf);
					kal_prompt_trace(MOD_MTHL, "mthl_send_activate_pdp_req_hdlr(): register_bearer_info fail.");
					
					g_mthl_wifi_bearer_info_save.wifi_bear_op = WIFI_NONE; //����ʧ�ܣ�
					return;
				}

                kal_prompt_trace(MOD_MTHL, "mthl_send_activate_pdp_req_hdlr(): register_bearer_info success.");
				//wifi_nwk_acct_id����mthl_atci����Թ̶���
				ret = cbm_open_bearer(g_mthl_wifi_nwk_acct_id);
	
				//��¼��ǰaccount_id���Ǽ��������
				//g_mthl_create_pdp_param_save[i].bear_op = PDP_ACTIVATE_REQ;
	
				//�������سɹ���ʧ�ܣ�
				if(ret != CBM_WOULDBLOCK)
				{
					if(ret == CBM_OK)
					{
					   mthl_wifi_activate_cnf.result = KAL_TRUE;
					   g_mthl_wifi_bearer_info_save.wifi_bear_op = WIFI_ACTIVATE_CNF; //����ɹ���
					}
					else
					{
					   mthl_wifi_activate_cnf.result = KAL_FALSE;
					   g_mthl_wifi_bearer_info_save.wifi_bear_op = WIFI_NONE;//����ʧ�ܣ�
					}
				
					mthl_wifi_activate_cnf.user_data = mthl_wifi_activate_req->user_data;
					
					//����cnf;
					mthl_send_activate_wifi_cnf(&mthl_wifi_activate_cnf);
					kal_prompt_trace(MOD_MTHL, "mthl_send_activate_wifi_req_hdlr(): open_bearer = %d.", ret);
					
					return;
				}
				
				//WOULDBLOCK: IND�ϱ��󣬿�֪bearer�Ƿ񼤻�ɹ���		
				//���崦��ABM��INIT/QUERY��Ϣ;��apn/ auth/usrname/passwd����ȥ��
				kal_prompt_trace(MOD_MTHL, "activate_wifi: WOULDBLOCK case.");
				g_mthl_wifi_bearer_info_save.user_data = mthl_wifi_activate_req->user_data;
				return;
				
}
#endif

void mthl_send_deactivate_pdp_req_hdlr(ilm_struct *ilm_ptr)
{
            mthl_deactivate_pdp_req_struct   *mthl_deactivate_pdp_req;  
			mthl_deactivate_pdp_cnf_struct   mthl_deactivate_pdp_cnf;
			kal_int8 ret;
			int i=0;
			
			mthl_deactivate_pdp_req = (mthl_deactivate_pdp_req_struct*)ilm_ptr->local_para_ptr;
			memset(&mthl_deactivate_pdp_cnf, 0x00, sizeof(mthl_deactivate_pdp_cnf_struct));

			for(i =0 ;i< MTHL_GPRS_MAX_PDP_NUM;i++)
			{
			    if(mthl_deactivate_pdp_req->account_id == g_mthl_create_pdp_param_save[i].account_id)
			    {
			        //ori_acct_idƥ��ɹ�;
			        kal_prompt_trace(MOD_MTHL, "mthl_send_deactivate_pdp_req_hdlr(): match acct_id case.");
					break;
			    }
				else
					continue;
			}

            //account_idƥ��ʧ�ܣ�
			if(i == MTHL_GPRS_MAX_PDP_NUM)
			{
			    mthl_deactivate_pdp_cnf.account_id = mthl_deactivate_pdp_req->account_id;
				mthl_deactivate_pdp_cnf.result = KAL_FALSE;
				mthl_deactivate_pdp_cnf.error_cause = 0;
				mthl_deactivate_pdp_cnf.user_data = mthl_deactivate_pdp_req->user_data;
				
				//����cnf;
				mthl_send_deactivate_pdp_cnf(&mthl_deactivate_pdp_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_send_deactivate_pdp_req_hdlr(): not match acct_id.");
				
				g_mthl_create_pdp_param_save[i].bear_op = PDP_DEACTIVATE_CNF;
				return;
			}
			
			ret = mthl_cbm_release_bearer(mthl_deactivate_pdp_req->account_id); 
			g_mthl_create_pdp_param_save[i].bear_op = PDP_DEACTIVATE_REQ;

			//�������سɹ���ʧ�ܣ�
			if(ret != CBM_WOULDBLOCK)
			{
			    mthl_deactivate_pdp_cnf.account_id = g_mthl_create_pdp_param_save[i].account_id;
				
				if(ret == CBM_OK)
				   mthl_deactivate_pdp_cnf.result = KAL_TRUE;
				else
				   mthl_deactivate_pdp_cnf.result = KAL_FALSE;
				
				mthl_deactivate_pdp_cnf.error_cause = 0;
				mthl_deactivate_pdp_cnf.user_data = mthl_deactivate_pdp_req->user_data;
				
				//����cnf;
				mthl_send_deactivate_pdp_cnf(&mthl_deactivate_pdp_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_send_deactivate_pdp_req_hdlr(): release_bearer = %d.", ret);
				
				g_mthl_create_pdp_param_save[i].bear_op = PDP_DEACTIVATE_CNF;
				return;
			}
			
			//IND�ϱ��󣬿�֪bearer�Ƿ�ȥ����ɹ���
			kal_prompt_trace(MOD_MTHL, "deactivate_pdp: WOULDBLOCK case.");
			
}

#if defined(__WIFI_TCPIP_AT_SUPPORT__)
void mthl_send_deactivate_wifi_req_hdlr(ilm_struct *ilm_ptr)
{
			mthl_wifi_deactivate_req_struct   *mthl_wifi_deactivate_req;  
			mthl_wifi_deactivate_cnf_struct   mthl_wifi_deactivate_cnf;
			kal_int8 ret;
			int i=0;

			kal_int8 result;
            cbm_app_info_struct info = {0};
			kal_int32 wlan_status;
    		srv_dtcnt_wlan_state_enum wlan_state;
				
			mthl_wifi_deactivate_req = (mthl_wifi_deactivate_req_struct*)ilm_ptr->local_para_ptr;
			kal_prompt_trace(MOD_MTHL, "mthl_send_deactivate_wifi_req_hdlr() param:");
			kal_prompt_trace(MOD_MTHL, "mthl_send_deactivate_wifi_req_hdlr() mthl_wifi_deactivate_req->user_data :%d",mthl_wifi_deactivate_req->user_data);
			memset(&mthl_wifi_deactivate_cnf, 0x00, sizeof(mthl_wifi_deactivate_cnf_struct));

				//��ǰ�Ǵ���һ��WIFI_DEACTIVATE_REQ��
				g_mthl_wifi_bearer_info_save.wifi_bear_op = WIFI_DEACTIVATE_REQ;
				//for WIFI case, need to register_app_id����֤��û����	ACTIVATE��������ֱ��DEACTIVATE�Ĵ���case��Ҳ�кϷ���app_id;	
			    if(g_mthl_task_init_done == KAL_TRUE)
			   	{
					info.app_str_id = 0;//MOD_L4C;
					info.app_icon_id = 0;
					info.app_type = DTCNT_APPTYPE_NONE;
					
					result = cbm_register_app_id_with_app_info(&info, &g_mthl_atci_app_id); 

					if(result ==CBM_OK)
					{
					    kal_prompt_trace(MOD_MTHL, "mthl_init(): g_mthl_atci_app_id = %d.", g_mthl_atci_app_id);

						g_mthl_wifi_nwk_acct_id = cbm_encode_data_account_id(CBM_WIFI_ACCT_ID, CBM_SIM_ID_SIM1, g_mthl_atci_app_id, KAL_FALSE);
						
					    if(g_mthl_wifi_nwk_acct_id == CBM_INVALID_NWK_ACCT_ID)
							kal_prompt_trace(MOD_MTHL, "mthl_init(): g_mthl_wifi_nwk_acct_id is invalid.");
						else
							kal_prompt_trace(MOD_MTHL, "mthl_init(): g_mthl_wifi_nwk_acct_id = %x.", g_mthl_wifi_nwk_acct_id);
					}
					else
					{
					    kal_prompt_trace(MOD_MTHL, "mthl_init(): g_mthl_atci_app_id error, result = %d.", result);
						
						mthl_wifi_deactivate_cnf.result = KAL_FALSE;
						mthl_wifi_deactivate_cnf.user_data = mthl_wifi_deactivate_req->user_data;
						
						//����cnf;
						mthl_send_deactivate_wifi_cnf(&mthl_wifi_deactivate_cnf);
						kal_prompt_trace(MOD_MTHL, "mthl_send_activate_pdp_req_hdlr(): register_app_id fail.");
						
						g_mthl_wifi_bearer_info_save.wifi_bear_op = WIFI_NONE; //����ʧ�ܣ�
						return;
					}

					g_mthl_task_init_done = KAL_FALSE;
			    }
	            /* Check the WLAN state */
    			wlan_state = srv_dtcnt_wlan_state();
				kal_prompt_trace(MOD_MTHL, "mthl_send_deactivate_wifi_req_hdlr() wlan_state = %d", wlan_state);

				/* If the WLAN is turned off , send the response directly ���޷�����*/
				//WIFIû��������
			    if (wlan_state == SRV_DTCNT_WLAN_STATE_NULL ||
			        wlan_state == SRV_DTCNT_WLAN_STATE_TOTAL)
			    {
					mthl_wifi_deactivate_cnf.result = KAL_FALSE;
					mthl_wifi_deactivate_cnf.user_data = mthl_wifi_deactivate_req->user_data;
					
					//����cnf;
					mthl_send_deactivate_wifi_cnf(&mthl_wifi_deactivate_cnf);
					kal_prompt_trace(MOD_MTHL, "mthl_send_deactivate_wifi_req_hdlr(): WLAN is turned off.");
					//g_mthl_create_pdp_param_save[i].bear_op = PDP_ACTIVATE_CNF;
					g_mthl_wifi_bearer_info_save.wifi_bear_op = WIFI_NONE; //����ʧ�ܣ�
					return;
				}

            	/* Start to query the WLAN status */
			    wlan_status = abm_get_bearer_available_status(CBM_BEARER_WIFI, CBM_SIM_ID_SIM1);
				kal_prompt_trace(MOD_MTHL, "mthl_send_deactivate_wifi_req_hdlr() wlan_status = %d", wlan_status);

	            /* If the WLAN is available, send the response directly ���Ѿ�ȥ����*/
			    if (//wlan_state != SRV_DTCNT_WLAN_STATE_CONNECTED &&
			        wlan_status == CBM_DEACTIVATED)
				{
					mthl_wifi_deactivate_cnf.result = KAL_TRUE;
					mthl_wifi_deactivate_cnf.user_data = mthl_wifi_deactivate_req->user_data;
					
					//����cnf;
					mthl_send_deactivate_wifi_cnf(&mthl_wifi_deactivate_cnf);
					kal_prompt_trace(MOD_MTHL, "mthl_send_deactivate_wifi_req_hdlr(): WLAN is deactivated.");
					//g_mthl_create_pdp_param_save[i].bear_op = PDP_ACTIVATE_CNF;
					g_mthl_wifi_bearer_info_save.wifi_bear_op = WIFI_DEACTIVATE_CNF; //ȥ����ɹ���
					return;
				}
			kal_prompt_trace(MOD_MTHL, "mthl_send_deactivate_wifi_req_hdlr(): WLAN is turned on, need to be deactivated.");
			
			ret = mthl_cbm_release_bearer(CBM_WIFI_ACCT_ID); 

			//�������سɹ���ʧ�ܣ�
			if(ret != CBM_WOULDBLOCK)
			{			
				if(ret == CBM_OK)
				{
				   mthl_wifi_deactivate_cnf.result = KAL_TRUE;
				   g_mthl_wifi_bearer_info_save.wifi_bear_op  = WIFI_DEACTIVATE_CNF;
				}
				else
				{
				   mthl_wifi_deactivate_cnf.result = KAL_FALSE;
				   g_mthl_wifi_bearer_info_save.wifi_bear_op  = WIFI_NONE;
				}
				
				mthl_wifi_deactivate_cnf.user_data = mthl_wifi_deactivate_req->user_data;
				
				//����cnf;
				mthl_send_deactivate_wifi_cnf(&mthl_wifi_deactivate_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_send_deactivate_wifi_req_hdlr(): release_bearer = %d.", ret);
				
				return;
			}
			
			//IND�ϱ��󣬿�֪bearer�Ƿ�ȥ����ɹ���
			kal_prompt_trace(MOD_MTHL, "deactivate_pdp: WOULDBLOCK case.");    
			g_mthl_wifi_bearer_info_save.user_data = mthl_wifi_deactivate_req->user_data;
}
#endif
void mthl_get_pdp_state_req_hdlr(ilm_struct *ilm_ptr)
{
			int i;
			kal_int32 bearer_status;
	 		mthl_query_pdp_state_req_struct   *mthl_query_pdp_state_req;  
			mthl_query_pdp_state_cnf_struct    mthl_query_pdp_state_cnf;
			
			mthl_query_pdp_state_req = (mthl_query_pdp_state_req_struct*)ilm_ptr->local_para_ptr;
			memset(&mthl_query_pdp_state_cnf, 0x00, sizeof(mthl_query_pdp_state_cnf_struct));   

            //query the param of mthl task save ; 
			for(i =0 ;i< MTHL_GPRS_MAX_PDP_NUM;i++)
			{
			   //find valid account_id;
			   if(g_mthl_create_pdp_param_save[i].account_id != CBM_INVALID_NWK_ACCT_ID)
			   {
			        kal_prompt_trace(MOD_MTHL, "mthl_get_pdp_state_req_hdlr(): query state OK case, param:");
					
			        //��ѯ��һ��account_id��Ч����Ӧ�Ĳ��������ǳɹ���
					mthl_query_pdp_state_cnf.result = KAL_TRUE;
					mthl_query_pdp_state_cnf.num++;
					
					mthl_query_pdp_state_cnf.state_list[i][0] = (kal_uint8)(g_mthl_create_pdp_param_save[i].account_id);
                    kal_prompt_trace(MOD_MTHL, "mthl_get_pdp_state_req_hdlr(): g_mthl_create_pdp_param_save[%d].account_id = %d.", i,g_mthl_create_pdp_param_save[i].account_id);
					
					if(g_mthl_create_pdp_param_save[i].bear_op == PDP_ACTIVATE_CNF)
					   mthl_query_pdp_state_cnf.state_list[i][1] = KAL_TRUE;
					//�����deactivateʧ�ܣ���ʱ����Ҳ����ACTIVATED;
					else if(g_mthl_create_pdp_param_save[i].bear_op == PDP_NONE)
					{
					   bearer_status = abm_get_bearer_available_status(CBM_BEARER_PS, CBM_SIM_ID_SIM1);
					   
					   if(bearer_status == CBM_ACTIVATED)
					   	 mthl_query_pdp_state_cnf.state_list[i][1] = KAL_TRUE;
					   else
					   	 mthl_query_pdp_state_cnf.state_list[i][1] = KAL_FALSE;
					}
					else
					   mthl_query_pdp_state_cnf.state_list[i][1] = KAL_FALSE;

                    //��ѯ��һ����Ч�ģ���Ҫ������ѯ��
					continue;
			   }
			   else
			   	   continue;
			}

			//����֮��not find; 
			if(mthl_query_pdp_state_cnf.num == 0)
			{
			    mthl_query_pdp_state_cnf.result = KAL_FALSE;
				//mthl_query_pdp_state_cnf.num; //0
				mthl_query_pdp_state_cnf.user_data  = mthl_query_pdp_state_req->user_data;//ԭ�ⲻ������;
				
				//����cnf;
				mthl_send_query_pdp_state_cnf(&mthl_query_pdp_state_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_get_pdp_state_req_hdlr(): query state fail case.");
				return;
			}

            //find valid account_id;
			mthl_query_pdp_state_cnf.user_data  = mthl_query_pdp_state_req->user_data;//ԭ�ⲻ������;
			mthl_send_query_pdp_state_cnf(&mthl_query_pdp_state_cnf);
            kal_prompt_trace(MOD_MTHL, "mthl_get_pdp_state_req_hdlr(): SUCCESS case.");    

            return;			
}

#if defined(__WIFI_TCPIP_AT_SUPPORT__)
void mthl_get_wifi_state_req_hdlr(ilm_struct *ilm_ptr)
{
				int i;
				kal_int32 bearer_status;
				
				mthl_wifi_get_state_req_struct   *mthl_wifi_get_state_req;  
				mthl_wifi_get_state_cnf_struct    mthl_wifi_get_state_cnf;
				
				mthl_wifi_get_state_req = (mthl_wifi_get_state_req_struct*)ilm_ptr->local_para_ptr;
				memset(&mthl_wifi_get_state_cnf, 0x00, sizeof(mthl_wifi_get_state_cnf_struct));	
	
				bearer_status = abm_get_bearer_available_status(CBM_BEARER_WIFI, CBM_SIM_ID_SIM1);
				
				//һ���Ǽ����case; 
				if(g_mthl_wifi_bearer_info_save.wifi_bear_op == WIFI_ACTIVATE_CNF)
				{
					mthl_wifi_get_state_cnf.result = KAL_TRUE;
					mthl_wifi_get_state_cnf.state  = CBM_ACTIVATED;
					mthl_wifi_get_state_cnf.user_data	= mthl_wifi_get_state_req->user_data;//ԭ�ⲻ������;
					
					//����cnf;
					mthl_send_query_wifi_state_cnf(&mthl_wifi_get_state_cnf);
					kal_prompt_trace(MOD_MTHL, "mthl_get_wifi_state_req_hdlr(): query state activates case.");
					return;
				}	
				else if(g_mthl_wifi_bearer_info_save.wifi_bear_op == WIFI_NONE)
				{
					if(bearer_status == CBM_ACTIVATED)
					{
					  mthl_wifi_get_state_cnf.result = KAL_TRUE;
					  mthl_wifi_get_state_cnf.state  = bearer_status;
					}
					else
					{
					  mthl_wifi_get_state_cnf.result = KAL_FALSE;
					  mthl_wifi_get_state_cnf.state  = bearer_status;
					}

					mthl_wifi_get_state_cnf.user_data	= mthl_wifi_get_state_req->user_data;//ԭ�ⲻ������;
					mthl_send_query_wifi_state_cnf(&mthl_wifi_get_state_cnf);	
					return;
				}
				else
				{
				    mthl_wifi_get_state_cnf.result = KAL_FALSE;
					mthl_wifi_get_state_cnf.state  = bearer_status;
					mthl_wifi_get_state_cnf.user_data	= mthl_wifi_get_state_req->user_data;//ԭ�ⲻ������;
					mthl_send_query_wifi_state_cnf(&mthl_wifi_get_state_cnf);	
					return;
				}
					
				return; 

}
#endif

/*******************************************socket handle*************************************************/
void mthl_create_and_conn_req_hdlr(ilm_struct *ilm_ptr)
{
    mthl_create_conn_req_struct   *mthl_create_conn_req;  
	mthl_create_conn_cnf_struct    mthl_create_conn_cnf;
    int i=0,j = 0;
	kal_int8 error_soc_create= 0;
	kal_int8 ret_conn = 0;
	kal_uint8 val;
	
	mthl_create_conn_req = (mthl_create_conn_req_struct*)ilm_ptr->local_para_ptr;
	memset(&mthl_create_conn_cnf, 0x00, sizeof(mthl_create_conn_cnf_struct));
	
	        //ͨ��ori_acct_id�ҵ�g_nwk_acct_id��
            for(i =0 ;i< MTHL_GPRS_MAX_PDP_NUM;i++)
			{
			    if(mthl_create_conn_req->account_id == g_mthl_create_pdp_param_save[i].account_id)
			    {
			        //ori_acct_idƥ��ɹ�;
			        kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): match acct_id case.");
					kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): g_mthl_create_pdp_param_save[%d].account_id = %d", i,g_mthl_create_pdp_param_save[i].account_id);
					break;
			    }
				else
					continue;
			}

            //ori_acct_idƥ��ʧ�ܻ򴴽���socket_type��֧�֣�
			if(i == MTHL_GPRS_MAX_PDP_NUM || mthl_create_conn_req->addr.sock_type == SOC_SOCK_SMS || mthl_create_conn_req->addr.sock_type == SOC_SOCK_RAW)
			{
			    mthl_create_conn_cnf.account_id = mthl_create_conn_req->account_id;
				mthl_create_conn_cnf.result     = KAL_FALSE;
				mthl_create_conn_cnf.sock_id    = SOC_ERROR;
				mthl_create_conn_cnf.user_data  = mthl_create_conn_req->user_data;
				
				//����cnf;
				mthl_send_create_conn_cnf(&mthl_create_conn_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): not match acct_id or not support sock_type.");
				return;
			}	
  #if defined(__MTK_TARGET__) 				
	#ifndef __TCPIP_LOOP_BACK__
	   //����ϣ�loopback is disable,  check 127.0.0.1 is invalid;
	   if(mthl_create_conn_req->addr.sock_type == SOC_SOCK_STREAM && 
	   	  mthl_create_conn_req->addr.addr[0]==127 && 
	   	  mthl_create_conn_req->addr.addr[1]==0 && 
	   	  mthl_create_conn_req->addr.addr[2]==0 && 
	   	  mthl_create_conn_req->addr.addr[3]==1)   
	   {
			    mthl_create_conn_cnf.account_id = mthl_create_conn_req->account_id;
				mthl_create_conn_cnf.result     = KAL_FALSE;
				mthl_create_conn_cnf.sock_id    = SOC_ERROR;
				mthl_create_conn_cnf.user_data  = mthl_create_conn_req->user_data;
				
				//����cnf;
				mthl_send_create_conn_cnf(&mthl_create_conn_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): TCP Case & loopback is disable.");
				return;
	   }	  
	#endif
  #endif/*__MTK_TARGET__*/

	//��¼sock_id��socket_type��Ӧ��ϵ��
	//ÿ�η���һ��RSP�ɹ��ģ�����ori_acct_id���������
	error_soc_create = soc_create(SOC_PF_INET, mthl_create_conn_req->addr.sock_type, 0, MOD_MTHL, g_mthl_create_pdp_param_save[i].nwk_acct_id);

    //ֻҪsocket�����ɹ����������첽����������
 #if defined(__MTK_TARGET__)
    if(error_soc_create >= 0)
    {
		val = KAL_TRUE;
	    soc_setsockopt(error_soc_create, SOC_NBIO, &val, sizeof(val));
	    val = SOC_READ | SOC_WRITE | SOC_CONNECT | SOC_CLOSE;
	    soc_setsockopt(error_soc_create, SOC_ASYNC, &val, sizeof(val));
		
		kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): error_soc_create = %d.", error_soc_create);
    }
 #else
    if(error_soc_create >= 0)
    {
      //if(mthl_create_conn_req->addr.sock_type == SOC_SOCK_DGRAM)
      {
		val = KAL_TRUE;
	    soc_setsockopt(error_soc_create, SOC_NBIO, &val, sizeof(val));
	    val = SOC_READ | SOC_WRITE | SOC_CONNECT | SOC_CLOSE;
	    soc_setsockopt(error_soc_create, SOC_ASYNC, &val, sizeof(val));
		
		kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): error_soc_create = %d.", error_soc_create);
      }
    }
 #endif

  /****************************************************UDP*************************************************************/
	//����ǻ���UDP�ģ��������ӵģ�ֱ�ӷ���RSP��
	//UDP success;
    if(mthl_create_conn_req->addr.sock_type == SOC_SOCK_DGRAM && error_soc_create >= 0)
    {
        g_mthl_create_conn_soc_save[i].account_id               = g_mthl_create_pdp_param_save[i].account_id;
		//g_mthl_create_conn_soc_save[i].sock_num++;

		//�ҵ�һ�����õ�socket_info[j]���ٱ�����Ϣ��
		for(j=0;j<MTHL_MAX_IP_SOCKET_NUM;j++)
		{
		   if(g_mthl_create_conn_soc_save[i].socket_info[j].sock_id == SOC_ERROR)
		   {
		        kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): find free sock_info, param:");
				
				g_mthl_create_conn_soc_save[i].socket_info[j].sock_id      = error_soc_create;
				kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): g_mthl_create_conn_soc_save[%d].socket_info[%d].sock_id = %d", i,j,g_mthl_create_conn_soc_save[i].socket_info[j].sock_id);
				
				g_mthl_create_conn_soc_save[i].socket_info[j].addr.sock_type  = SOC_SOCK_DGRAM;
				kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): UDP case.");

	            //UDPҲҪ����req��������Ĭ�Ϸ��͵�ַ��
				g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[0] = mthl_create_conn_req->addr.addr[0];
				g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[1] = mthl_create_conn_req->addr.addr[1];
				g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[2] = mthl_create_conn_req->addr.addr[2];
				g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[3] = mthl_create_conn_req->addr.addr[3];
				g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr_len= mthl_create_conn_req->addr.addr_len;
				g_mthl_create_conn_soc_save[i].socket_info[j].addr.port    = mthl_create_conn_req->addr.port;
				//������Ϣok���ټ�����
				g_mthl_create_conn_soc_save[i].sock_num++;
				kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): g_mthl_create_conn_soc_save[%d].sock_num = %d",i,g_mthl_create_conn_soc_save[i].sock_num);

				//Ϊ����ͳ���ٱ���һ�ݣ�
				g_mthl_create_conn_soc_save[i].socket_statistics_info[j].sock_id = error_soc_create;
				break;
		   }
		   else
		   	continue;
		}

		//�����ɹ�������ʧ�ܣ�
		if(j == MTHL_MAX_IP_SOCKET_NUM)
		{
		        mthl_create_conn_cnf.account_id = mthl_create_conn_req->account_id;
				mthl_create_conn_cnf.result     = KAL_FALSE;
				mthl_create_conn_cnf.sock_id    = SOC_ERROR;
				mthl_create_conn_cnf.user_data  = mthl_create_conn_req->user_data;
				
				//����cnf;
				mthl_send_create_conn_cnf(&mthl_create_conn_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): not find free sock_info.");
				//close socket;
				soc_close(error_soc_create);
				return;
		}
		//�����ɹ�������ɹ���
		else
		{
		        mthl_create_conn_cnf.account_id = mthl_create_conn_req->account_id;
				mthl_create_conn_cnf.result     = KAL_TRUE;
				mthl_create_conn_cnf.sock_id    = error_soc_create;
				mthl_create_conn_cnf.user_data  = mthl_create_conn_req->user_data;
			    //����cnf;
				mthl_send_create_conn_cnf(&mthl_create_conn_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): find free sock_info, done.");
				return;
		}
			
    }
	//UDP ���� fail;
	if(mthl_create_conn_req->addr.sock_type == SOC_SOCK_DGRAM && error_soc_create < 0)
	{
	            mthl_create_conn_cnf.account_id = mthl_create_conn_req->account_id;
				mthl_create_conn_cnf.result     = KAL_FALSE;
				mthl_create_conn_cnf.sock_id    = SOC_ERROR;
				mthl_create_conn_cnf.user_data  = mthl_create_conn_req->user_data;
				
				//����cnf;
				mthl_send_create_conn_cnf(&mthl_create_conn_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): UDP error_soc_create fail = %d.", error_soc_create);
				return;
	}
	
/********************************************************************************************************************/

	
	
	//����ǻ���TCP�ģ���soc_connect(��);�������ӽ���ٷ���RSP��
	//TCP ���� success;
    if(mthl_create_conn_req->addr.sock_type == SOC_SOCK_STREAM && error_soc_create >= 0)
    {
         g_mthl_create_conn_soc_save[i].account_id               = g_mthl_create_pdp_param_save[i].account_id;
		 
         ret_conn = soc_connect(error_soc_create, &(mthl_create_conn_req->addr));
		 //�����������ӳɹ���ʧ�ܣ�
		 if(ret_conn !=SOC_WOULDBLOCK)
		 {
		     if(ret_conn == SOC_SUCCESS)//�����������ӳɹ���
		     {
		            kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): soc_connect, success.");
					
					//�ҵ�һ�����õ�socket_info[j]���ٱ�����Ϣ��
					for(j=0;j<MTHL_MAX_IP_SOCKET_NUM;j++)
					{
					   if(g_mthl_create_conn_soc_save[i].socket_info[j].sock_id == SOC_ERROR)
					   {
					        kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): find free sock_info, param:");
							
							g_mthl_create_conn_soc_save[i].socket_info[j].sock_id      = error_soc_create;
							kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): g_mthl_create_conn_soc_save[%d].socket_info[%d].sock_id = %d", i,j,g_mthl_create_conn_soc_save[i].socket_info[j].sock_id);
							
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.sock_type  = SOC_SOCK_STREAM;
							kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): TCP case.");

				            //TCP ���ñ���req��������Ҫ���ӵ�Server��ַ��
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[0] = mthl_create_conn_req->addr.addr[0];
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[1] = mthl_create_conn_req->addr.addr[1];
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[2] = mthl_create_conn_req->addr.addr[2];
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[3] = mthl_create_conn_req->addr.addr[3];
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr_len= mthl_create_conn_req->addr.addr_len;
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.port    = mthl_create_conn_req->addr.port;
							
							//������Ϣok���ټ�����
							g_mthl_create_conn_soc_save[i].sock_num++;
							kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): g_mthl_create_conn_soc_save[%d].sock_num = %d",i,g_mthl_create_conn_soc_save[i].sock_num);

							//Ϊ����ͳ���ٱ���һ�ݣ�
							g_mthl_create_conn_soc_save[i].socket_statistics_info[j].sock_id = error_soc_create;
							break;
					   }
					   else
					   	continue;
					}
                    //���������ӳɹ�������ʧ�ܣ�
					if(j == MTHL_MAX_IP_SOCKET_NUM)
					{
					        mthl_create_conn_cnf.account_id = mthl_create_conn_req->account_id;
							mthl_create_conn_cnf.result     = KAL_FALSE;
							mthl_create_conn_cnf.sock_id    = SOC_LIMIT_RESOURCE;
							mthl_create_conn_cnf.user_data  = mthl_create_conn_req->user_data;
							
							//����cnf;
							mthl_send_create_conn_cnf(&mthl_create_conn_cnf);
							kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): soc_connect, success, not find free sock_info.");
							//close socket;
							soc_close(error_soc_create);
							return;
					}
					//���������ӳɹ�������ɹ���
					else
					{
					        mthl_create_conn_cnf.account_id = mthl_create_conn_req->account_id;
							mthl_create_conn_cnf.result     = KAL_TRUE;
							mthl_create_conn_cnf.sock_id    = error_soc_create;
							mthl_create_conn_cnf.user_data  = mthl_create_conn_req->user_data;
						    //����cnf;
							mthl_send_create_conn_cnf(&mthl_create_conn_cnf);
							kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): soc_connect, success, find free sock_info.");
							return;
					}
		     }
			 else//������������ʧ�ܣ�
			 {
			    mthl_create_conn_cnf.account_id = mthl_create_conn_req->account_id;
				mthl_create_conn_cnf.result     = KAL_FALSE;
				mthl_create_conn_cnf.sock_id    = SOC_ERROR;
				mthl_create_conn_cnf.user_data  = mthl_create_conn_req->user_data;
				
				//����cnf;
				mthl_send_create_conn_cnf(&mthl_create_conn_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): soc_connect, fail = %d.", ret_conn);
				
				//close socket;
				soc_close(error_soc_create);
				return;
			 }
		 }
		 //�����������ӽ����
		 else
		 { 
		        kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): soc_connect, wouldblock.");
					
					//�ҵ�һ�����õ�socket_info[j]���ٱ�����Ϣ��
					for(j=0;j<MTHL_MAX_IP_SOCKET_NUM;j++)
					{
					   if(g_mthl_create_conn_soc_save[i].socket_info[j].sock_id == SOC_ERROR)
					   {
					        kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): find free sock_info, param:");
							
							g_mthl_create_conn_soc_save[i].socket_info[j].sock_id      = error_soc_create;
							kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): g_mthl_create_conn_soc_save[%d].socket_info[%d].sock_id = %d", i,j,g_mthl_create_conn_soc_save[i].socket_info[j].sock_id);
							
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.sock_type  = SOC_SOCK_STREAM;
							kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): TCP case.");

				            //TCP ���ñ���req��������Ҫ���ӵ�Server��ַ��
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[0] = mthl_create_conn_req->addr.addr[0];
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[1] = mthl_create_conn_req->addr.addr[1];
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[2] = mthl_create_conn_req->addr.addr[2];
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[3] = mthl_create_conn_req->addr.addr[3];
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr_len= mthl_create_conn_req->addr.addr_len;
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.port    = mthl_create_conn_req->addr.port;
							
							//������Ϣok���ټ�����
							g_mthl_create_conn_soc_save[i].sock_num++;
							kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): g_mthl_create_conn_soc_save[%d].sock_num = %d",i,g_mthl_create_conn_soc_save[i].sock_num);

							//Ϊ����ͳ���ٱ���һ�ݣ�
							g_mthl_create_conn_soc_save[i].socket_statistics_info[j].sock_id = error_soc_create;
							break;
					   }
					   else
					   	continue;
					}
                    //����������SOC_WOULDBLOCK������ʧ�ܣ�
					if(j == MTHL_MAX_IP_SOCKET_NUM)
					{
					        mthl_create_conn_cnf.account_id = mthl_create_conn_req->account_id;
							mthl_create_conn_cnf.result     = KAL_FALSE;
							mthl_create_conn_cnf.sock_id    = SOC_LIMIT_RESOURCE;
							mthl_create_conn_cnf.user_data  = mthl_create_conn_req->user_data;
							
							//����cnf;
							mthl_send_create_conn_cnf(&mthl_create_conn_cnf);
							kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): soc_connect, success, not find free sock_info.");
							//close socket;
							soc_close(error_soc_create);
							return;
					}
					//����������SOC_WOULDBLOCK������ɹ���
					else
					{
					        mthl_create_conn_cnf.account_id = mthl_create_conn_req->account_id;
							mthl_create_conn_cnf.result     = KAL_FALSE;
							mthl_create_conn_cnf.sock_id    = SOC_WOULDBLOCK;
							mthl_create_conn_cnf.user_data  = mthl_create_conn_req->user_data;
							g_mthl_create_conn_soc_save[i].user_data = mthl_create_conn_req->user_data;
						    //����cnf;
							//mthl_send_create_conn_cnf(&mthl_create_conn_cnf);û�ж�Ӧ��MSG���Ͳ�����wouldblock;
							kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): soc_connect, success, find free sock_info.");
							return;
					}
		 }
    }
    //TCP ���� fail;
	if(mthl_create_conn_req->addr.sock_type == SOC_SOCK_STREAM && error_soc_create < 0)
	{
	            mthl_create_conn_cnf.account_id = mthl_create_conn_req->account_id;
				mthl_create_conn_cnf.result     = KAL_FALSE;
				mthl_create_conn_cnf.sock_id    = SOC_ERROR;
				mthl_create_conn_cnf.user_data  = mthl_create_conn_req->user_data;
				
				//����cnf;
				mthl_send_create_conn_cnf(&mthl_create_conn_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): TCP error_soc_create fail = %d.", error_soc_create);
				return;
	} 
	

	//��TCP/UDP���쳣Case��
	kal_prompt_trace(MOD_MTHL, "create_and_conn: no TCP & UDP case.");
    mthl_create_conn_cnf.account_id = mthl_create_conn_req->account_id;
	mthl_create_conn_cnf.result     = KAL_FALSE;
	mthl_create_conn_cnf.sock_id    = SOC_ERROR;
	mthl_create_conn_cnf.user_data  = mthl_create_conn_req->user_data;
				
	//����cnf;
	mthl_send_create_conn_cnf(&mthl_create_conn_cnf);
	return;	
	
}


void mthl_query_sock_num_req_hdlr(ilm_struct *ilm_ptr)
{
    mthl_query_sock_account_info_req_struct   *mthl_query_sock_account_info_req;  
	mthl_query_sock_account_info_cnf_struct    mthl_query_sock_account_info_cnf;
    int i =0;
	int j = 0;
	kal_int8 error_soc_create= 0;
	kal_int8 ret_conn = 0;
	
	mthl_query_sock_account_info_req = (mthl_query_sock_account_info_req_struct*)ilm_ptr->local_para_ptr;
    memset(&mthl_query_sock_account_info_cnf, 0x00, sizeof(mthl_query_sock_account_info_cnf_struct));
    
	for(i =0 ;i< MTHL_GPRS_MAX_PDP_NUM;i++)
	{
		if(mthl_query_sock_account_info_req->account_id == g_mthl_create_pdp_param_save[i].account_id)
		{
			//ori_acct_idƥ��ɹ�;
			kal_prompt_trace(MOD_MTHL, "mthl_query_sock_num_req_hdlr(): match acct_id case.");
			kal_prompt_trace(MOD_MTHL, "mthl_query_sock_num_req_hdlr(): g_mthl_create_pdp_param_save[%d].account_id = %d", i,g_mthl_create_pdp_param_save[i].account_id);
			break;
		}
		else
			continue;
	}
	
	//ori_acct_idƥ��ʧ�ܣ�
	if(i == MTHL_GPRS_MAX_PDP_NUM)
	{
		mthl_query_sock_account_info_cnf.account_id = mthl_query_sock_account_info_req->account_id;
		mthl_query_sock_account_info_cnf.result     = KAL_FALSE;
		//mthl_query_sock_account_info_cnf.socket_info��ѯʧ�ܣ��޲�ѯ���������default;
		//mthl_query_sock_account_info_cnf.sock_num��ѯʧ�ܣ��޲�ѯ���������default;
		mthl_query_sock_account_info_cnf.user_data  = mthl_query_sock_account_info_req->user_data;
					
		//����cnf;
		mthl_query_sock_num_cnf(&mthl_query_sock_account_info_cnf);
		kal_prompt_trace(MOD_MTHL, "mthl_query_sock_num_req_hdlr(): not match acct_id case.");
		return;
	}	

	//Check֮ǰ�ķ��������������ɵõ������
	mthl_query_sock_account_info_cnf.account_id = mthl_query_sock_account_info_req->account_id;
	kal_prompt_trace(MOD_MTHL, "mthl_query_sock_num_req_hdlr():mthl_query_sock_account_info_req->account_id = %d", mthl_query_sock_account_info_req->account_id);
	
	mthl_query_sock_account_info_cnf.result     = KAL_TRUE;
	memcpy(mthl_query_sock_account_info_cnf.socket_info, g_mthl_create_conn_soc_save[i].socket_info, sizeof(sock_account_info_struct)*MTHL_MAX_IP_SOCKET_NUM);
	
	mthl_query_sock_account_info_cnf.sock_num   = g_mthl_create_conn_soc_save[i].sock_num;
	kal_prompt_trace(MOD_MTHL, "mthl_query_sock_num_req_hdlr():g_mthl_create_conn_soc_save[%d].sock_num = %d", i, g_mthl_create_conn_soc_save[i].sock_num);
	
	mthl_query_sock_account_info_cnf.user_data  = mthl_query_sock_account_info_req->user_data;

	//����cnf;
	mthl_query_sock_num_cnf(&mthl_query_sock_account_info_cnf);
	
	return;
}

void mthl_close_req_hdlr(ilm_struct *ilm_ptr)
{
	mthl_close_sock_req_struct   *mthl_close_req;  
	mthl_close_sock_cnf_struct    mthl_close_cnf;
    kal_int8 ret;
	int i,j;
	
	mthl_close_req = (mthl_close_sock_req_struct*)ilm_ptr->local_para_ptr;
	memset(&mthl_close_cnf, 0x00, sizeof(mthl_close_sock_cnf_struct));

	ret = soc_close(mthl_close_req->sock_id);

	if(ret == SOC_INVALID_SOCKET)
	{
	    mthl_close_cnf.result    = KAL_FALSE;
		mthl_close_cnf.sock_id   = mthl_close_req->sock_id;
		mthl_close_cnf.user_data = mthl_close_req->user_data;
		
	    //����fail cnf;
		mthl_close_sock_cnf(&mthl_close_cnf);
		kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr() fail: soc_close ret = %d.", ret);
	}
	else if(ret == SOC_SUCCESS)
	{
	    mthl_close_cnf.result    = KAL_TRUE;
		mthl_close_cnf.sock_id   = mthl_close_req->sock_id;
		mthl_close_cnf.user_data = mthl_close_req->user_data;
		
	    //����success cnf;
		mthl_close_sock_cnf(&mthl_close_cnf);
		kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr() success: soc_close ret = %d.", ret);

		//�ر�server listen_sock;
        if(mthl_close_cnf.sock_id == g_server_info.listen_sock_id)
        {
           g_server_info.listen_sock_id = SOC_ERROR;
		   g_server_info.server_account_id = CBM_INVALID_NWK_ACCT_ID;
        }
		else   
		{
		        //ͨ��sock_idƥ�䣬��close��socket id������statistic�Ĳ��֣������Ϊinit value;
                for(i=0; i<MTHL_GPRS_MAX_PDP_NUM; i++)
                {
                    for(j=0; j<MTHL_MAX_IP_SOCKET_NUM; j++)
                    {
                       if(g_mthl_create_conn_soc_save[i].socket_info[j].sock_id== mthl_close_req->sock_id)
                       {
                         kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr() match sock_id.");
						 
                         kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear ,g_mthl_create_conn_soc_save[%d].socket_info[%d].sock_id = %d", i,j,g_mthl_create_conn_soc_save[i].socket_info[j].sock_id);
                         g_mthl_create_conn_soc_save[i].socket_info[j].sock_id = SOC_ERROR;

						 kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear ,g_mthl_create_conn_soc_save[%d].socket_statistics_info[%d].sock_id = %d", i,j,g_mthl_create_conn_soc_save[i].socket_statistics_info[j].sock_id);
						 g_mthl_create_conn_soc_save[i].socket_statistics_info[j].sock_id = SOC_ERROR;
						 
						 g_mthl_create_conn_soc_save[i].sock_num--;
						 kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear init value, SOC_ERROR, done.");
					   	 break;
                       }
					   else
					   	 continue;
                    }
					if(j<MTHL_MAX_IP_SOCKET_NUM)	
					  break;
					else
					  continue;
                }	

				//���ʧ�ܣ�
				if(i == MTHL_GPRS_MAX_PDP_NUM)
				{
					kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear init value, fail, not match sock_id..");   
					kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): mthl_close_req->sock_id = %d.", mthl_close_req->sock_id);
				}
		 }		
	}
	else
	{
	    //soc_close()�ķ���ֵֻ���������֣���Case ���쳣�ģ�
	    kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): error case.");
	  #ifdef DEBUG_INFO_M2M_MTHL_TASK_HANDLE_ONLY_FOR_IT
		ASSERT(0);
	  #endif/*DEBUG_INFO_M2M_MTHL_TASK_HANDLE_ONLY_FOR_IT*/
	}

	return;
}

void mthl_send_req_hdlr(ilm_struct *ilm_ptr)
{
    mthl_send_data_req_struct   *mthl_send_data_req;  
	mthl_send_data_cnf_struct    mthl_send_data_cnf;
	mthl_close_sock_ind_struct   mthl_close_ind;
    kal_int32 ret = SOC_ERROR;
	kal_int32 close_ret = SOC_ERROR;
	int i=0,j = 0;
	
	mthl_send_data_req = (mthl_send_data_req_struct*)ilm_ptr->local_para_ptr;
	memset(&mthl_send_data_cnf, 0x00, sizeof(mthl_send_data_cnf_struct));
	memset(&mthl_close_ind, 0x00, sizeof(mthl_close_sock_ind_struct));
	
	// ����sock_id ��ѯ��Ӧ��socket_type��Check�Ƿ���˽��ն˵�ַ��
    kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): sock_id = %d", mthl_send_data_req->sock_id);
	
	//ͨ��sock_id������ƥ�䵽account_id;
                for(i=0; i<MTHL_GPRS_MAX_PDP_NUM; i++)
                {
                    for(j=0; j<MTHL_MAX_IP_SOCKET_NUM; j++)
                    {
                       if(g_mthl_create_conn_soc_save[i].socket_info[j].sock_id== mthl_send_data_req->sock_id)
                       {
                         kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): match sock_id.");
                         kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): g_mthl_create_conn_soc_save[%d].socket_info[%d].sock_id = %d", i,j,g_mthl_create_conn_soc_save[i].socket_info[j].sock_id);
					   	 break;
                       }
					   else
					   	 continue;
                    }
					if(j<MTHL_MAX_IP_SOCKET_NUM)
					{
					  kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): j= %d.", j);
					  break;
					}
					else
					  continue;
                }

     //sock_idƥ��ʧ�ܣ�
	 if(i == MTHL_GPRS_MAX_PDP_NUM || mthl_send_data_req->data == NULL)
	 {
                  mthl_send_data_cnf.sock_id   = mthl_send_data_req->sock_id;
				 mthl_send_data_cnf.result    = KAL_FALSE;
				 mthl_send_data_cnf.ret_val   = SOC_INVALID_SOCKET;
				 mthl_send_data_cnf.user_data = mthl_send_data_req->user_data;

				 //����cnf;
				 mthl_send_sock_cnf(&mthl_send_data_cnf);
				 kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): not match sock_id, or mthl_send_data_req->data == NULL.");
				 //�쳣����ǰ���ͷ�buff������user���ط���
				if(mthl_send_data_req->data != NULL)
				{
        			free_ctrl_buffer(mthl_send_data_req->data);
				    mthl_send_data_req->data = NULL;
					kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): free_ctrl_buffer().");
				}
			     return;       
	 }

     //�ܵ��˴�û��return��˵���Ѿ��ǲ�ΪNULL��
     kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): mthl_send_data_req->data != NULL.");
	 //TCP case;
	 if(g_mthl_create_conn_soc_save[i].socket_info[j].addr.sock_type == SOC_SOCK_STREAM)		
	 {
	    // soc_send()	for TCP
	    kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): TCP send case.");
	    ret = soc_send(mthl_send_data_req->sock_id, mthl_send_data_req->data, mthl_send_data_req->size, mthl_send_data_req->flags);

        //���Ͷ���call�꣬���ͷ�buff������user���ط���
        free_ctrl_buffer(mthl_send_data_req->data);
		mthl_send_data_req->data = NULL;
		
		//�������ͳɹ���
		if(ret > 0)
		{
		     mthl_send_data_cnf.sock_id   = mthl_send_data_req->sock_id;
			 mthl_send_data_cnf.result    = KAL_TRUE;
			 
			 mthl_send_data_cnf.ret_val   = ret;
			 kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): TCP send ok, ret = %d.", ret);
			 
			 g_mthl_create_conn_soc_save[i].socket_statistics_info[j].total_send_size += ret;
			 mthl_send_data_cnf.user_data = mthl_send_data_req->user_data;

			 //����cnf;
			 mthl_send_sock_cnf(&mthl_send_data_cnf);
		 	 return;
		}
		//����ָʾ���ͽ����
		else if(ret == SOC_WOULDBLOCK)
		{
		     mthl_send_data_cnf.sock_id   = mthl_send_data_req->sock_id;
			 mthl_send_data_cnf.result    = KAL_FALSE;
			 mthl_send_data_cnf.ret_val   = ret;
			 mthl_send_data_cnf.user_data = mthl_send_data_req->user_data;

			 //����cnf;
			 mthl_send_sock_cnf(&mthl_send_data_cnf);
			 kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): TCP send WOULDBLOCK case.");
			 
		     return;
		}
		//��������ʧ�ܣ�
		else
		{
		     //����TCP ��������ʧ�ܵ�Case:���ڶԶ�ǿ�ƹرա������硢bearer�쳣����TCP���ӳ���
		     //һ��retΪ-1, �ͷ�socket;
		     if(ret == SOC_ERROR || ret == SOC_CONNABORTED || ret == SOC_BEARER_FAIL || ret == SOC_CONNRESET)
		     {
		         close_ret = soc_close(mthl_send_data_req->sock_id);
				 
				 if(close_ret == SOC_INVALID_SOCKET)
				 	kal_prompt_trace(MOD_MTHL, "send data fail: soc_close ret = SOC_INVALID_SOCKET");
				 else if(close_ret == SOC_SUCCESS)
				 {
				    //ͨ��sock_idƥ�䣬��close��socket id������statistic�Ĳ��֣������Ϊinit value;
	                for(i=0; i<MTHL_GPRS_MAX_PDP_NUM; i++)
	                {
	                    for(j=0; j<MTHL_MAX_IP_SOCKET_NUM; j++)
	                    {
	                       if(g_mthl_create_conn_soc_save[i].socket_info[j].sock_id== mthl_send_data_req->sock_id)
	                       {
	                         kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr() match sock_id.");
							 
	                         kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear ,g_mthl_create_conn_soc_save[%d].socket_info[%d].sock_id = %d", i,j,g_mthl_create_conn_soc_save[i].socket_info[j].sock_id);
	                         g_mthl_create_conn_soc_save[i].socket_info[j].sock_id = SOC_ERROR;

							 kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear ,g_mthl_create_conn_soc_save[%d].socket_statistics_info[%d].sock_id = %d", i,j,g_mthl_create_conn_soc_save[i].socket_statistics_info[j].sock_id);
							 g_mthl_create_conn_soc_save[i].socket_statistics_info[j].sock_id = SOC_ERROR;
							 g_mthl_create_conn_soc_save[i].sock_num--;
							 
							 kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear init value, SOC_ERROR, done.");
						   	 break;
	                       }
						   else
						   	 continue;
	                    }
						if(j<MTHL_MAX_IP_SOCKET_NUM)	
						  break;
						else
						  continue;
	                }	

					//���ʧ�ܣ�
					if(i == MTHL_GPRS_MAX_PDP_NUM)
					{
						kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear init value, fail, not match sock_id..");   
						kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): mthl_close_req->sock_id = %d.", mthl_send_data_req->sock_id);
					}
					//����ɹ���
					else
					{
					    kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear init value, success.");   
						kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): mthl_close_req->sock_id = %d.", mthl_send_data_req->sock_id);

						mthl_close_ind.result    = KAL_TRUE;
						mthl_close_ind.sock_id   = mthl_send_data_req->sock_id;
						mthl_close_ind.user_data = 0;
						
					    //����success IND;
						mthl_close_sock_ind(&mthl_close_ind);
					}
				 }				 	
				 else
				 	kal_prompt_trace(MOD_MTHL, "send data fail: soc_close ret = %d.",close_ret);
		     } 
		     mthl_send_data_cnf.sock_id   = mthl_send_data_req->sock_id;
			 mthl_send_data_cnf.result    = KAL_FALSE;
			 mthl_send_data_cnf.ret_val   = ret;
			 kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): TCP send fail, ret = %d.", ret);
			 
			 mthl_send_data_cnf.user_data = mthl_send_data_req->user_data;

			 //����cnf;
			 mthl_send_sock_cnf(&mthl_send_data_cnf);
		     return;
		}
	 }

	 //UDP case;
	 if(g_mthl_create_conn_soc_save[i].socket_info[j].addr.sock_type == SOC_SOCK_DGRAM)
	 {
	    //soc_sendto() for UDP
	    kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): UDP send case.");
		
	    //����ʱ��UDP��ǰָ�����յ�ַ��
	    if(mthl_send_data_req->addr.port !=0 && mthl_send_data_req->addr.addr[0] != 0)
	    {
	       kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): UDP send, current set addr:");
		   kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): UDP send, port = %d, addr = %d.%d.%d.%d", 
		   	                mthl_send_data_req->addr.port, mthl_send_data_req->addr.addr[0], mthl_send_data_req->addr.addr[1],mthl_send_data_req->addr.addr[2],mthl_send_data_req->addr.addr[3]);
		   
		   ret = soc_sendto(mthl_send_data_req->sock_id, mthl_send_data_req->data, mthl_send_data_req->size, mthl_send_data_req->flags, &(mthl_send_data_req->addr));
			//���Ͷ���call�꣬���ͷ�buff������user���ط���
	        free_ctrl_buffer(mthl_send_data_req->data);
			mthl_send_data_req->data = NULL;
			
			//�������ͳɹ���
			if(ret > 0)
			{
			     mthl_send_data_cnf.sock_id   = mthl_send_data_req->sock_id;
				 mthl_send_data_cnf.result    = KAL_TRUE;
				 mthl_send_data_cnf.ret_val   = ret;
				 kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): UDP send ok, ret = %d.", ret);
				 
				 g_mthl_create_conn_soc_save[i].socket_statistics_info[j].total_send_size += ret;
				 mthl_send_data_cnf.user_data = mthl_send_data_req->user_data;

				 //����cnf;
				 mthl_send_sock_cnf(&mthl_send_data_cnf);
			 	 return;
			}
			//����ָʾ���ͽ����
			else if(ret == SOC_WOULDBLOCK)
			{
			     mthl_send_data_cnf.sock_id   = mthl_send_data_req->sock_id;
				 mthl_send_data_cnf.result    = KAL_FALSE;
				 kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): UDP send WOULDBLOCK case.");
				 mthl_send_data_cnf.ret_val   = ret;
				 mthl_send_data_cnf.user_data = mthl_send_data_req->user_data;

				 //����cnf;
				 mthl_send_sock_cnf(&mthl_send_data_cnf);
			     return;
			}
			//��������ʧ�ܣ�
			else
			{
			     mthl_send_data_cnf.sock_id   = mthl_send_data_req->sock_id;
				 mthl_send_data_cnf.result    = KAL_FALSE;
				 mthl_send_data_cnf.ret_val   = ret;
				 kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): UDP send fail, ret = %d.", ret);
				 
				 mthl_send_data_cnf.user_data = mthl_send_data_req->user_data;

				 //����cnf;
				 mthl_send_sock_cnf(&mthl_send_data_cnf);
			     return;
			}
	    }
        //����ʱ��UDP��ǰָ�����յ�ַ�����Ǵ���ĵ�ַ���Ϸ���
		if((mthl_send_data_req->addr.port ==0 && mthl_send_data_req->addr.addr[0] != 0)||(mthl_send_data_req->addr.port !=0 && mthl_send_data_req->addr.addr[0] == 0))
		{
		          kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): UDP send, current set error addr:");
		          kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): UDP send, port = %d, addr = %d.%d.%d.%d", 
		   	                mthl_send_data_req->addr.port, mthl_send_data_req->addr.addr[0], mthl_send_data_req->addr.addr[1],mthl_send_data_req->addr.addr[2],mthl_send_data_req->addr.addr[3]);
		   
				 mthl_send_data_cnf.sock_id   = mthl_send_data_req->sock_id;
				 mthl_send_data_cnf.result    = KAL_FALSE;
				 mthl_send_data_cnf.ret_val   = SOC_INVAL;
				 mthl_send_data_cnf.user_data = mthl_send_data_req->user_data;

				 //����cnf;
				 mthl_send_sock_cnf(&mthl_send_data_cnf);
				  //�쳣����ǰ���ͷ�buff������user���ط���
        		free_ctrl_buffer(mthl_send_data_req->data);
				mthl_send_data_req->data = NULL;
                kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): free_ctrl_buffer().");
			     return;
		}
		
		//����ʱ��UDP��ǰδָ�����յ�ַ����:ʹ��֮ǰָ����Ĭ�ϵ�ַ��
		if(mthl_send_data_req->addr.port ==0 && mthl_send_data_req->addr.addr[0] == 0)
		{
		   //֮ǰָ����Ĭ�ϵ�ַ��
		   if(g_mthl_create_conn_soc_save[i].socket_info[j].addr.port != 0 && g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[0]!=0)
		   {
		        kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): UDP send, default addr:");
				kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): UDP send, port = %d, addr = %d.%d.%d.%d", 
		   	                g_mthl_create_conn_soc_save[i].socket_info[j].addr.port, 
		   	                g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[0], 
		   	                g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[1],
		   	                g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[2],
		   	                g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[3]);
				
		        ret = soc_sendto(mthl_send_data_req->sock_id, mthl_send_data_req->data, mthl_send_data_req->size, mthl_send_data_req->flags, &(g_mthl_create_conn_soc_save[i].socket_info[j].addr));
				//���Ͷ���call�꣬���ͷ�buff������user���ط���
		        free_ctrl_buffer(mthl_send_data_req->data);
				mthl_send_data_req->data = NULL;
				
				//�������ͳɹ���
				if(ret > 0)
				{
				     mthl_send_data_cnf.sock_id   = mthl_send_data_req->sock_id;
					 mthl_send_data_cnf.result    = KAL_TRUE;
					 mthl_send_data_cnf.ret_val   = ret;
					 kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): UDP send ok, ret = %d.", ret);
					 
					 g_mthl_create_conn_soc_save[i].socket_statistics_info[j].total_send_size += ret;
					 mthl_send_data_cnf.user_data = mthl_send_data_req->user_data;

					 //����cnf;
					 mthl_send_sock_cnf(&mthl_send_data_cnf);
				 	 return;
				}
				//����ָʾ���ͽ����
				else if(ret == SOC_WOULDBLOCK)
				{
				     mthl_send_data_cnf.sock_id   = mthl_send_data_req->sock_id;
					 mthl_send_data_cnf.result    = KAL_FALSE;
					 kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): UDP send WOULDBLOCK case.");
					 
					 mthl_send_data_cnf.ret_val   = ret;
					 mthl_send_data_cnf.user_data = mthl_send_data_req->user_data;

					 //����cnf;
					 mthl_send_sock_cnf(&mthl_send_data_cnf);
				     return;
				}
				//��������ʧ�ܣ�
				else
				{
				     mthl_send_data_cnf.sock_id   = mthl_send_data_req->sock_id;
					 mthl_send_data_cnf.result    = KAL_FALSE;
					 mthl_send_data_cnf.ret_val   = ret;
					 kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): UDP send fail, ret = %d.", ret);
					 
					 mthl_send_data_cnf.user_data = mthl_send_data_req->user_data;

					 //����cnf;
					 mthl_send_sock_cnf(&mthl_send_data_cnf);
				     return;
				}
		   }
		   //֮ǰδָ����Ч��Ĭ�ϵ�ַ��
		   else
		   {
		        kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): UDP send, error default addr:");
				kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): UDP send, port = %d, addr = %d.%d.%d.%d", 
		   	                g_mthl_create_conn_soc_save[i].socket_info[j].addr.port, 
		   	                g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[0], 
		   	                g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[1],
		   	                g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[2],
		   	                g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[3]);
				
		         mthl_send_data_cnf.sock_id   = mthl_send_data_req->sock_id;
				 mthl_send_data_cnf.result    = KAL_FALSE;
				 mthl_send_data_cnf.ret_val   = SOC_INVAL;
				 mthl_send_data_cnf.user_data = mthl_send_data_req->user_data;

				 //����cnf;
				 mthl_send_sock_cnf(&mthl_send_data_cnf);
				  //�쳣����ǰ���ͷ�buff������user���ط���
        		free_ctrl_buffer(mthl_send_data_req->data);
				mthl_send_data_req->data = NULL;
                kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): free_ctrl_buffer().");
			     return;
		   }
		}
	 }
	
	 //��TCP/UDP���쳣case��
	 kal_prompt_trace(MOD_MTHL, "send data: no TCP & UDP case.");
     mthl_send_data_cnf.sock_id   = mthl_send_data_req->sock_id;
	 mthl_send_data_cnf.result    = KAL_FALSE;
	 mthl_send_data_cnf.ret_val   = SOC_OPNOTSUPP;
	 mthl_send_data_cnf.user_data = mthl_send_data_req->user_data;

	 //����cnf;
	 mthl_send_sock_cnf(&mthl_send_data_cnf);
	 //�쳣����ǰ���ͷ�buff������user���ط���
     free_ctrl_buffer(mthl_send_data_req->data);
	 mthl_send_data_req->data = NULL;
     kal_prompt_trace(MOD_MTHL, "mthl_send_req_hdlr(): free_ctrl_buffer().");
	 return;	 
}

void mthl_recv_req_hdlr(ilm_struct *ilm_ptr)
{
    mthl_recv_data_req_struct   *mthl_recv_data_req;  
	mthl_recv_data_cnf_struct    mthl_recv_data_cnf;
	mthl_close_sock_ind_struct   mthl_close_ind;
    kal_int32 ret = 0;
	kal_int32 close_ret = 0;
	int i=0,j=0;
	kal_uint8 *buff = NULL;
	
	buff = get_ctrl_buffer(512);
	memset(buff, 0x00, 512);
	
	mthl_recv_data_req = (mthl_recv_data_req_struct*)ilm_ptr->local_para_ptr;
	memset(&mthl_recv_data_cnf, 0x00, sizeof(mthl_recv_data_cnf_struct));

	kal_prompt_trace(MOD_MTHL, "mthl_recv_req_hdlr(): sock_id = %d", mthl_recv_data_req->sock_id);
	
	// ����sock_id ��ѯ��Ӧ��socket_type��
                for(i=0; i<MTHL_GPRS_MAX_PDP_NUM; i++)
                {
                    for(j=0; j<MTHL_MAX_IP_SOCKET_NUM; j++)
                    {
                       if(g_mthl_create_conn_soc_save[i].socket_info[j].sock_id== mthl_recv_data_req->sock_id)
                       {
                         kal_prompt_trace(MOD_MTHL, "mthl_recv_req_hdlr(): match sock_id.");
                         kal_prompt_trace(MOD_MTHL, "mthl_recv_req_hdlr(): g_mthl_create_conn_soc_save[%d].socket_info[%d].sock_id = %d", i,j,g_mthl_create_conn_soc_save[i].socket_info[j].sock_id);
					   	 break;
                       	}
					   else
					   	 continue;
                    }
					if(j<MTHL_MAX_IP_SOCKET_NUM)	
					{
					  kal_prompt_trace(MOD_MTHL, "mthl_recv_req_hdlr(): j= %d.", j);
					  break;
					}					  
					else
					  continue;
                }

     //sock_idƥ��ʧ�ܣ�
	 if(i == MTHL_GPRS_MAX_PDP_NUM)
	 {
                 mthl_recv_data_cnf.sock_id   = mthl_recv_data_req->sock_id;
				 mthl_recv_data_cnf.result    = KAL_FALSE;
				 mthl_recv_data_cnf.ret_val   = SOC_INVALID_SOCKET;
				 mthl_recv_data_cnf.user_data = mthl_recv_data_req->user_data;

				 //����cnf;
				 mthl_recv_sock_cnf(&mthl_recv_data_cnf);
				 kal_prompt_trace(MOD_MTHL, "mthl_recv_req_hdlr(): not match sock_id.");

				 //�����쳣���أ�MTHLֱ���Լ�free memory;
				 free_ctrl_buffer(buff);
				 buff = NULL;
				 
			     return;       
	 }
    
	// ����sock_id ��ѯ��Ӧ��socket_type ��
	//TCP case;
	 if(g_mthl_create_conn_soc_save[i].socket_info[j].addr.sock_type == SOC_SOCK_STREAM)		
	 {
	     // soc_recv() for TCP;
	     kal_prompt_trace(MOD_MTHL, "mthl_recv_req_hdlr(): TCP recv case.");
		 
	     ret = soc_recv(mthl_recv_data_req->sock_id, buff, 512, mthl_recv_data_req->flags);
		 
		                 //�������ճɹ���
						 if(ret > 0)
						 {
							  mthl_recv_data_cnf.sock_id   = mthl_recv_data_req->sock_id;
							  mthl_recv_data_cnf.result    = KAL_TRUE;

							  //buffָ�봫�أ��Ա�ATCI free memory;
							  mthl_recv_data_cnf.buff      = buff;
							  mthl_recv_data_cnf.ret_val   = ret;
							  
							  kal_prompt_trace(MOD_MTHL, "mthl_recv_req_hdlr(): TCP recv ok, ret = %d.", ret);
							  
							  g_mthl_create_conn_soc_save[i].socket_statistics_info[j].total_recv_size += ret;
							  mthl_recv_data_cnf.user_data = mthl_recv_data_req->user_data;
		 
							  //����cnf;
							  mthl_recv_sock_cnf(&mthl_recv_data_cnf);
							  return;
						 }
						 //����ָʾ���ս����
						 else if(ret == SOC_WOULDBLOCK)
						 {
							  mthl_recv_data_cnf.sock_id   = mthl_recv_data_req->sock_id;
							  mthl_recv_data_cnf.result    = KAL_FALSE;
									  
							  //mthl_recv_data_cnf.buff      = buff;
							  mthl_recv_data_cnf.ret_val   = ret;
							  
							  //�����쳣���أ�MTHLֱ���Լ�free memory;
							 free_ctrl_buffer(buff);
							 buff = NULL;
				 
							  kal_prompt_trace(MOD_MTHL, "mthl_recv_req_hdlr(): TCP recv WOULDBLOCK case.");
							  
							  mthl_recv_data_cnf.user_data = mthl_recv_data_req->user_data;
		 
							  //����cnf;
							  mthl_recv_sock_cnf(&mthl_recv_data_cnf);
							  return;
						 }
						 //��������ʧ�ܣ�
						 else
						 {
						     //����TCP ��������ʧ�ܵ�Case:���ڶԶ�ǿ�ƹرա������硢bearer�쳣����TCP���ӳ���
						     //retΪ-1���Է�ǿ�йر�������close sock FIN�����ͷ�socket;
						     //[ע]: retΪ0���Է������ر����ӣ���: close sock FIN������������CLOSE_IND��
						     if(ret == SOC_ERROR || ret == SOC_CONNABORTED || ret == SOC_BEARER_FAIL || ret == SOC_CONNRESET)
						     {
						         close_ret = soc_close(mthl_recv_data_req->sock_id);
								 
								 if(close_ret == SOC_INVALID_SOCKET)
								 	kal_prompt_trace(MOD_MTHL, "recv data fail: soc_close ret = SOC_INVALID_SOCKET");
								 else if(close_ret == SOC_SUCCESS)
								 {
								    //ͨ��sock_idƥ�䣬��close��socket id������statistic�Ĳ��֣������Ϊinit value;
					                for(i=0; i<MTHL_GPRS_MAX_PDP_NUM; i++)
					                {
					                    for(j=0; j<MTHL_MAX_IP_SOCKET_NUM; j++)
					                    {
					                       if(g_mthl_create_conn_soc_save[i].socket_info[j].sock_id== mthl_recv_data_req->sock_id)
					                       {
					                         kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr() match sock_id.");
											 
					                         kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear ,g_mthl_create_conn_soc_save[%d].socket_info[%d].sock_id = %d", i,j,g_mthl_create_conn_soc_save[i].socket_info[j].sock_id);
					                         g_mthl_create_conn_soc_save[i].socket_info[j].sock_id = SOC_ERROR;

											 kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear ,g_mthl_create_conn_soc_save[%d].socket_statistics_info[%d].sock_id = %d", i,j,g_mthl_create_conn_soc_save[i].socket_statistics_info[j].sock_id);
											 g_mthl_create_conn_soc_save[i].socket_statistics_info[j].sock_id = SOC_ERROR;
											 g_mthl_create_conn_soc_save[i].sock_num--;
											 
											 kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear init value, SOC_ERROR, done.");
										   	 break;
					                       }
										   else
										   	 continue;
					                    }
										if(j<MTHL_MAX_IP_SOCKET_NUM)	
										  break;
										else
										  continue;
					                }	

									//���ʧ�ܣ�
									if(i == MTHL_GPRS_MAX_PDP_NUM)
									{
										kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear init value, fail, not match sock_id..");   
										kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): mthl_close_req->sock_id = %d.", mthl_recv_data_req->sock_id);
									}
									else
									{
									    kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear init value, success.");   
										kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): mthl_close_req->sock_id = %d.", mthl_recv_data_req->sock_id);

										mthl_close_ind.result    = KAL_TRUE;
										mthl_close_ind.sock_id   = mthl_recv_data_req->sock_id;
										mthl_close_ind.user_data = 0;
										
									    //����success IND;
										mthl_close_sock_ind(&mthl_close_ind);
									}
								 }				 	
								 else
								 	kal_prompt_trace(MOD_MTHL, "send data fail: soc_close ret = %d.",close_ret);
						     } 
							  mthl_recv_data_cnf.sock_id   = mthl_recv_data_req->sock_id;
							  mthl_recv_data_cnf.result    = KAL_FALSE;
							  //mthl_recv_data_cnf.buff      = buff;
							  mthl_recv_data_cnf.ret_val   = ret;
							  //�����쳣���أ�MTHLֱ���Լ�free memory;
							  free_ctrl_buffer(buff);
							  buff = NULL;
							 
							  kal_prompt_trace(MOD_MTHL, "mthl_recv_req_hdlr(): TCP recv fail, ret = %d.", ret);
							  mthl_recv_data_cnf.user_data = mthl_recv_data_req->user_data;
		 
							  //����cnf;
							  mthl_recv_sock_cnf(&mthl_recv_data_cnf);
							  return;
						 }		 	
	 }

	//UDP case;
	if(g_mthl_create_conn_soc_save[i].socket_info[j].addr.sock_type == SOC_SOCK_DGRAM) 
	{
	    //soc_recvfrom ()  for UDP
	    kal_prompt_trace(MOD_MTHL, "mthl_recv_req_hdlr(): UDP recv case.");
		
	    ret = soc_recvfrom(mthl_recv_data_req->sock_id, buff, 512, mthl_recv_data_req->flags,&(mthl_recv_data_cnf.addr));
		
                         //�������ճɹ���
						 if(ret > 0)
						 {
							  mthl_recv_data_cnf.sock_id   = mthl_recv_data_req->sock_id;
							  mthl_recv_data_cnf.result    = KAL_TRUE;

							  //buffָ�봫�أ��Ա�ATCI free memory;
							  mthl_recv_data_cnf.buff      = buff;
							  mthl_recv_data_cnf.ret_val   = ret;
							  kal_prompt_trace(MOD_MTHL, "mthl_recv_req_hdlr(): UDP recv ok, ret = %d.", ret);
							  
							  g_mthl_create_conn_soc_save[i].socket_statistics_info[j].total_recv_size += ret;
							  mthl_recv_data_cnf.user_data = mthl_recv_data_req->user_data;
		 
							  //����cnf;
							  mthl_recv_sock_cnf(&mthl_recv_data_cnf);
							  return;
						 }
						 //����ָʾ���ս����
						 else if(ret == SOC_WOULDBLOCK)
						 {
							  mthl_recv_data_cnf.sock_id   = mthl_recv_data_req->sock_id;
							  mthl_recv_data_cnf.result    = KAL_FALSE;
							  //mthl_recv_data_cnf.buff      = buff;
							  kal_prompt_trace(MOD_MTHL, "mthl_recv_req_hdlr(): UDP recv WOULDBLOCK case.");
							  
							  mthl_recv_data_cnf.ret_val   = ret;
							  //�����쳣���أ�MTHLֱ���Լ�free memory;
							 free_ctrl_buffer(buff);
							 buff = NULL;
							 
							  mthl_recv_data_cnf.user_data = mthl_recv_data_req->user_data;
		 
							  //����cnf;
							  mthl_recv_sock_cnf(&mthl_recv_data_cnf);
							  return;
						 }
						 //��������ʧ�ܣ�
						 else
						 {
							  mthl_recv_data_cnf.sock_id   = mthl_recv_data_req->sock_id;
							  mthl_recv_data_cnf.result    = KAL_FALSE;
							  //mthl_recv_data_cnf.buff      = buff;
							  mthl_recv_data_cnf.ret_val   = ret;
							  //�����쳣���أ�MTHLֱ���Լ�free memory;
							 free_ctrl_buffer(buff);
							 buff = NULL;
							 
							  kal_prompt_trace(MOD_MTHL, "mthl_recv_req_hdlr(): UDP recv fail, ret = %d.", ret);
							  
							  mthl_recv_data_cnf.user_data = mthl_recv_data_req->user_data;
		 
							  //����cnf;
							  mthl_recv_sock_cnf(&mthl_recv_data_cnf);
							  return;
						 }		
	}
	 
     //��TCP/UDP���쳣case��
     kal_prompt_trace(MOD_MTHL, "recv data: no TCP & UDP case.");
     mthl_recv_data_cnf.sock_id   = mthl_recv_data_req->sock_id;
	 mthl_recv_data_cnf.result    = KAL_FALSE;	
	 //mthl_recv_data_cnf.buff      = buff;
	 mthl_recv_data_cnf.ret_val   = SOC_OPNOTSUPP;
	 //�����쳣���أ�MTHLֱ���Լ�free memory;
	 free_ctrl_buffer(buff);
	 buff = NULL;
							 
	 mthl_recv_data_cnf.user_data = mthl_recv_data_req->user_data;
		 
	  //����cnf;
	 mthl_recv_sock_cnf(&mthl_recv_data_cnf);
	 return;	
	
}

void mthl_query_info_req_hdlr(ilm_struct *ilm_ptr)
{
    mthl_query_sock_info_req_struct   *mthl_query_sock_info_req;  
	mthl_query_sock_info_cnf_struct    mthl_query_sock_info_cnf;
    kal_int32 ret;
	int i,j;
	kal_uint8 *buff;
	
	mthl_query_sock_info_req = (mthl_query_sock_info_req_struct*)ilm_ptr->local_para_ptr;
	memset(&mthl_query_sock_info_cnf, 0x00, sizeof(mthl_query_sock_info_cnf_struct));

	kal_prompt_trace(MOD_MTHL, "mthl_query_info_req_hdlr(): sock_id = %d", mthl_query_sock_info_req->sock_id);
	
	            // ����sock_id ��ѯ���������
	            for(i=0; i<MTHL_GPRS_MAX_PDP_NUM; i++)
                {
                    for(j=0; j<MTHL_MAX_IP_SOCKET_NUM; j++)
                    {
                       if(g_mthl_create_conn_soc_save[i].socket_statistics_info[j].sock_id== mthl_query_sock_info_req->sock_id)
                       {
                         kal_prompt_trace(MOD_MTHL, "mthl_query_info_req_hdlr(): match sock_id.");
                         kal_prompt_trace(MOD_MTHL, "mthl_query_info_req_hdlr(): g_mthl_create_conn_soc_save[%d].socket_statistics_info[%d].sock_id = %d", i,j,g_mthl_create_conn_soc_save[i].socket_statistics_info[j].sock_id);
					   	 break;
                       }
					   else
					   	 continue;
                    }
					if(j<MTHL_MAX_IP_SOCKET_NUM)	
					  break;
					else
					  continue;
                }

			     //sock_idƥ��ʧ�ܣ�
				 if(i == MTHL_GPRS_MAX_PDP_NUM)
				 {
			                 mthl_query_sock_info_cnf.sock_id   = mthl_query_sock_info_req->sock_id;
							 mthl_query_sock_info_cnf.result    = KAL_FALSE;
							 //mthl_send_data_cnf.ret_val   = ret;
							 mthl_query_sock_info_cnf.user_data = mthl_query_sock_info_req->user_data;

							 //����cnf;
							 mthl_query_info_sock_cnf(&mthl_query_sock_info_cnf);
                             kal_prompt_trace(MOD_MTHL, "mthl_query_info_req_hdlr(): not match sock_id.");
						     return;       
				 }
	
	//�Ѿ����յ�Data Size
	// ACKED��ֵ���������͵��Զ˲���ȷ�ϵ�ackֵ��ͨ��sockopt��ȡ��
	// ���͵��Զ˲���ȷ�ϵ����ݵ�Size��
	kal_prompt_trace(MOD_MTHL, "mthl_query_info_req_hdlr(): match sock_id, query info:");
	
	mthl_query_sock_info_cnf.sock_id         = mthl_query_sock_info_req->sock_id;
	kal_prompt_trace(MOD_MTHL, "mthl_query_info_req_hdlr(): mthl_query_sock_info_req->sock_id = %d", mthl_query_sock_info_req->sock_id);
	
	mthl_query_sock_info_cnf.result          = KAL_TRUE;
	
	mthl_query_sock_info_cnf.total_send_size = g_mthl_create_conn_soc_save[i].socket_statistics_info[j].total_send_size;
	kal_prompt_trace(MOD_MTHL, "mthl_query_info_req_hdlr(): mthl_query_sock_info_cnf.total_send_size = %ld", mthl_query_sock_info_cnf.total_send_size);
	
	mthl_query_sock_info_cnf.total_recv_size = g_mthl_create_conn_soc_save[i].socket_statistics_info[j].total_recv_size;
	kal_prompt_trace(MOD_MTHL, "mthl_query_info_req_hdlr(): mthl_query_sock_info_cnf.total_recv_size = %ld", mthl_query_sock_info_cnf.total_recv_size);
	
	soc_getsockopt(mthl_query_sock_info_req->sock_id, SOC_TCP_ACKED_DATA, &(mthl_query_sock_info_cnf.Ack_value), sizeof(mthl_query_sock_info_cnf.Ack_value));
	kal_prompt_trace(MOD_MTHL, "mthl_query_info_req_hdlr(): mthl_query_sock_info_cnf.Ack_value = %ld", mthl_query_sock_info_cnf.Ack_value);
	mthl_query_sock_info_cnf.user_data       = mthl_query_sock_info_req->user_data;
	
	//����cnf;
	mthl_query_info_sock_cnf(&mthl_query_sock_info_cnf);
    return;	
}

void mthl_get_host_by_name_req_hdlr(ilm_struct *ilm_ptr)
{
    mthl_get_host_by_name_req_struct   *mthl_get_host_by_name_req;  
	mthl_get_host_by_name_cnf_struct    mthl_get_host_by_name_cnf;
    kal_int8 ret;
	int i,j;
	kal_uint8 *buff;
	kal_uint32 nwk_account_id;
	
	mthl_get_host_by_name_req = (mthl_get_host_by_name_req_struct*)ilm_ptr->local_para_ptr;
	memset(&mthl_get_host_by_name_cnf, 0x00, sizeof(mthl_get_host_by_name_cnf_struct));

	
	//ά��request_id;//req/rsp�ɶԳ��֣����Դ�0,1,2��;
	
	//nwk_account_id;//encode��ori_acct_id\sim_id\app_id��
	        //����һ�����õ�account_id��
            for(i =0 ;i< MTHL_GPRS_MAX_PDP_NUM;i++)
			{
			    if(g_mthl_create_pdp_param_save[i].account_id != CBM_INVALID_NWK_ACCT_ID)
			    {
			        //ori_acct_id���ҳɹ�;
			        kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_name_req_hdlr(): match account_id.");
					kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_name_req_hdlr(): g_mthl_create_pdp_param_save[%d].account_id = %d", i,g_mthl_create_pdp_param_save[i].account_id);
			        nwk_account_id = cbm_encode_data_account_id(g_mthl_create_pdp_param_save[i].account_id, CBM_SIM_ID_SIM1 , g_mthl_atci_app_id, KAL_FALSE);
					break;
			    }
				else
					continue;
			}

            //account_idƥ��ʧ�ܻ�domain_nameΪ0��Ч������
			if(i == MTHL_GPRS_MAX_PDP_NUM || strlen(mthl_get_host_by_name_req->domain_name)==0)
			{
			    mthl_get_host_by_name_cnf.result = KAL_FALSE;
				mthl_get_host_by_name_cnf.user_data = mthl_get_host_by_name_req->user_data;

				//����ʧ�ܣ���Ҫ��req��������domain name��Ϣ���أ�
				mthl_get_host_by_name_cnf.domain_len = strlen(mthl_get_host_by_name_req->domain_name);
		        strncpy(mthl_get_host_by_name_cnf.domain_name, mthl_get_host_by_name_req->domain_name, mthl_get_host_by_name_cnf.domain_len);
				
				//����cnf;
				mthl_get_host_by_name_soc_cnf(&mthl_get_host_by_name_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_name_req_hdlr(): not match account_id!");
				return;
			}

			//�ܵ��˴�˵��domain_name��Ч������0��
			kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_name_req_hdlr(): domain_name > 0.");
	  //[Notes]: check֮ǰ��DNS query���첽case���Ƿ��з���RSP�������ܿ�ʼ��һ�β�ѯ��
			if(g_get_host_by_name_save.domain_len != 0 || strlen(g_get_host_by_name_save.domain_name)>0)
			{
			    //û�б���գ�˵��֮ǰ��DNS queryû�з���RSP��
			    kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_name_req_hdlr(): DNS query async case: not RSP!");
			    mthl_get_host_by_name_cnf.result = KAL_FALSE;
				mthl_get_host_by_name_cnf.user_data = mthl_get_host_by_name_req->user_data;

				//����ʧ�ܣ���Ҫ����һ��req��������domain name��Ϣ���أ�
				mthl_get_host_by_name_cnf.domain_len = strlen(mthl_get_host_by_name_req->domain_name);
		        strncpy(mthl_get_host_by_name_cnf.domain_name, mthl_get_host_by_name_req->domain_name, mthl_get_host_by_name_cnf.domain_len);
				
				//����cnf;
				mthl_get_host_by_name_soc_cnf(&mthl_get_host_by_name_cnf);
				return;	    
			}
	
	ret = soc_gethostbyname(KAL_FALSE,MOD_MTHL,g_request_id_for_gethostbyname, mthl_get_host_by_name_req->domain_name,
							   mthl_get_host_by_name_cnf.addr,
							   (kal_uint8 *)(&(mthl_get_host_by_name_cnf.addr_len)),
							   0,nwk_account_id);
	
    //���ϴ�DNS Cache�У��õ������
	if(ret == SOC_SUCCESS)
	{
         mthl_get_host_by_name_cnf.result = KAL_TRUE;
		 mthl_get_host_by_name_cnf.domain_len = strlen(mthl_get_host_by_name_req->domain_name);
		 strncpy(mthl_get_host_by_name_cnf.domain_name, mthl_get_host_by_name_req->domain_name, mthl_get_host_by_name_cnf.domain_len);
		 kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_name_req_hdlr(): DNS query from cache, Success: IP addr:");
		 kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_name_req_hdlr(): mthl_get_host_by_name_cnf.addr = %d.%d.%d.%d", 
			             mthl_get_host_by_name_cnf.addr[0], mthl_get_host_by_name_cnf.addr[1],mthl_get_host_by_name_cnf.addr[2],mthl_get_host_by_name_cnf.addr[3]);
         mthl_get_host_by_name_cnf.user_data = mthl_get_host_by_name_req->user_data;

		 //����cnf;
		 mthl_get_host_by_name_soc_cnf(&mthl_get_host_by_name_cnf);
		 kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_name_req_hdlr(): DNS query from cache, Success!");
		 //return;	 
	}
	//�������ؽ����
	else if(ret == SOC_WOULDBLOCK)
	{
	      //���������������domain name��Ϣ�����ں������أ�
	      g_get_host_by_name_save.domain_len = strlen(mthl_get_host_by_name_req->domain_name);
		  strncpy(g_get_host_by_name_save.domain_name, mthl_get_host_by_name_req->domain_name, g_get_host_by_name_save.domain_len);
		  g_get_host_by_name_save.user_data = mthl_get_host_by_name_req->user_data;
	      kal_prompt_trace(MOD_MTHL, "get_host_by_name: WOULDBLOCK case."); 
	}
	//�����õ�fail;
	else
	{
                mthl_get_host_by_name_cnf.result = KAL_FALSE;
				mthl_get_host_by_name_cnf.user_data = mthl_get_host_by_name_req->user_data;

				//����ʧ�ܣ���Ҫ��req��������domain name��Ϣ���أ�
				mthl_get_host_by_name_cnf.domain_len = strlen(mthl_get_host_by_name_req->domain_name);
		        strncpy(mthl_get_host_by_name_cnf.domain_name, mthl_get_host_by_name_req->domain_name, mthl_get_host_by_name_cnf.domain_len);
				
				//����cnf;
				mthl_get_host_by_name_soc_cnf(&mthl_get_host_by_name_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_name_req_hdlr(): DNS query fail!");
				//return;	    
	}

	return;
	
	//����SOC�ϱ���IND��// ��ѯ�����nwk_account_id�� request_id�������֣�
}

void mthl_get_host_by_addr_req_hdlr(ilm_struct *ilm_ptr)
{
	mthl_get_host_by_addr_req_struct   *mthl_get_host_by_addr_req;  
	mthl_get_host_by_addr_cnf_struct    mthl_get_host_by_addr_cnf;
    kal_int8 ret;
	int i,j = 0;
	kal_uint8 *buff;
	kal_uint32 nwk_account_id;
	
	mthl_get_host_by_addr_req = (mthl_get_host_by_addr_req_struct*)ilm_ptr->local_para_ptr;
	memset(&mthl_get_host_by_addr_cnf, 0x00, sizeof(mthl_get_host_by_addr_cnf_struct));

	
	//ά��request_id;//req/rsp�ɶԳ��֣����Դ�0,1,2��;
	
	//nwk_account_id;//encode��ori_acct_id\sim_id\app_id��
	        //����һ�����õ�account_id��
            for(i =0 ;i< MTHL_GPRS_MAX_PDP_NUM;i++)
			{
			    if(g_mthl_create_pdp_param_save[i].account_id != CBM_INVALID_NWK_ACCT_ID)
			    {
			        //ori_acct_id���ҳɹ�;
			        kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_addr_req_hdlr(): match account_id.");
					kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_addr_req_hdlr(): g_mthl_create_pdp_param_save[%d].account_id = %d", i,g_mthl_create_pdp_param_save[i].account_id);
			        nwk_account_id = cbm_encode_data_account_id(g_mthl_create_pdp_param_save[i].account_id, CBM_SIM_ID_SIM1 , g_mthl_atci_app_id, KAL_FALSE);
					break;
			    }
				else
					continue;
			}

            //account_idƥ��ʧ�ܻ�addrΪ0��Ч������
			if(i == MTHL_GPRS_MAX_PDP_NUM || (mthl_get_host_by_addr_req->addr[0]==0))
			{
			    mthl_get_host_by_addr_cnf.result = KAL_FALSE;
				mthl_get_host_by_addr_cnf.user_data = mthl_get_host_by_addr_req->user_data;

				//����ʧ�ܣ���Ҫ��req��������addr��Ϣ���أ�
				mthl_get_host_by_addr_cnf.addr_len = strlen(mthl_get_host_by_addr_req->addr);
		        memcpy(mthl_get_host_by_addr_cnf.addr, mthl_get_host_by_addr_req->addr, mthl_get_host_by_addr_req->addr_len);
				
				//����cnf;
				mthl_get_host_by_addr_soc_cnf(&mthl_get_host_by_addr_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_addr_req_hdlr(): not match account_id!");
				return;
			}

            //�ܵ��˴�˵��addr��Ч����Ϊ0��
			kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_addr_req_hdlr(): addr not 0.");
	  //[Notes]: check֮ǰ��IP Addr query���첽case���Ƿ��з���RSP�������ܿ�ʼ��һ�β�ѯ��
			if(g_get_host_by_addr_save.addr_len!= 0 || strlen(g_get_host_by_addr_save.addr)>0)
			{
			    //û�б���գ�˵��֮ǰ��IP Addr queryû�з���RSP��
			    kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_addr_req_hdlr(): IP Addr query async case: not RSP!");
			    mthl_get_host_by_addr_cnf.result = KAL_FALSE;
				mthl_get_host_by_addr_cnf.user_data = mthl_get_host_by_addr_req->user_data;

				//����ʧ�ܣ���Ҫ����һ��req��������domain name��Ϣ���أ�
				mthl_get_host_by_addr_cnf.addr_len = strlen(mthl_get_host_by_addr_req->addr);
		        memcpy(mthl_get_host_by_addr_cnf.addr, mthl_get_host_by_addr_req->addr, mthl_get_host_by_addr_req->addr_len);
				
				//����cnf;
				mthl_get_host_by_addr_soc_cnf(&mthl_get_host_by_addr_cnf);
				return;	    
			}
	
	 mthl_get_host_by_addr_cnf.domain_len = 128;
	 ret = soc_gethostbyaddr(KAL_FALSE,
                           MOD_MTHL,
                           g_request_id_for_gethostbyaddr,
                           mthl_get_host_by_addr_cnf.domain_name,
                           (kal_uint32 *)&mthl_get_host_by_addr_cnf.domain_len,
                           mthl_get_host_by_addr_req->addr,
                           (kal_uint8)mthl_get_host_by_addr_req->addr_len,
                           0,
                           nwk_account_id);
	
    //���ϴ�DNS Cache�У��õ������
	if(ret == SOC_SUCCESS)
	{
         mthl_get_host_by_addr_cnf.result = KAL_TRUE;
		 mthl_get_host_by_addr_cnf.addr_len = strlen(mthl_get_host_by_addr_req->addr);
		 memcpy(mthl_get_host_by_addr_cnf.addr, mthl_get_host_by_addr_req->addr, mthl_get_host_by_addr_req->addr_len);
         mthl_get_host_by_addr_cnf.user_data = mthl_get_host_by_addr_req->user_data;

		 //����cnf;
		 mthl_get_host_by_addr_soc_cnf(&mthl_get_host_by_addr_cnf);
		 kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_addr_req_hdlr(): IP Addr query from cache, Success: domain_name:");
		 kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_addr_req_hdlr(): domain_name = %s", mthl_get_host_by_addr_cnf.domain_name);
		 //return;	 
	}
	//�������ؽ����
	else if(ret == SOC_WOULDBLOCK)
	{
	      //���������������IP Addr ��Ϣ�����ں������أ�
	      g_get_host_by_addr_save.addr_len= mthl_get_host_by_addr_req->addr_len;
		  memcpy(g_get_host_by_addr_save.addr, mthl_get_host_by_addr_req->addr, g_get_host_by_addr_save.addr_len);
		  g_get_host_by_addr_save.user_data = mthl_get_host_by_addr_req->user_data;
	      kal_prompt_trace(MOD_MTHL, "get_host_by_addr: WOULDBLOCK case."); 
	}
	//�����õ�fail;
	else
	{
                mthl_get_host_by_addr_cnf.result = KAL_FALSE;
				mthl_get_host_by_addr_cnf.user_data = mthl_get_host_by_addr_req->user_data;

				//����ʧ�ܣ���Ҫ��req��������IP Addr ��Ϣ���أ�
				mthl_get_host_by_addr_cnf.addr_len = strlen(mthl_get_host_by_addr_req->addr);
		        memcpy(mthl_get_host_by_addr_cnf.addr, mthl_get_host_by_addr_req->addr, mthl_get_host_by_addr_req->addr_len);
				
				//����cnf;
				mthl_get_host_by_addr_soc_cnf(&mthl_get_host_by_addr_cnf);
				kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_addr_req_hdlr(): IP Addr query fail.");
				//return;	    
	}

	return;
	
	//����SOC�ϱ���IND��  
}

void mthl_tcp_server_listen_and_accept_req_hdlr(ilm_struct *ilm_ptr)
{
    mthl_server_launch_req_struct   *mthl_server_launch_req;  
	mthl_server_listen_cnf_struct    mthl_server_listen_cnf;
	mthl_server_accept_ind_struct    mthl_server_accept_ind;
    kal_int8 ret;
	kal_int8 error_soc_create= 0;
	int i=0;
	int j=0;
	kal_uint8 val;
	sockaddr_struct server_addr;
	sockaddr_struct conn_addr;

	mthl_server_launch_req = (mthl_server_launch_req_struct*)ilm_ptr->local_para_ptr;
	memset(&mthl_server_listen_cnf, 0x00, sizeof(mthl_server_listen_cnf_struct));
	memset(&mthl_server_accept_ind, 0x00, sizeof(mthl_server_accept_ind_struct));
	
	        //ͨ��ori_acct_id�ҵ�g_nwk_acct_id��
            for(i =0 ;i< MTHL_GPRS_MAX_PDP_NUM;i++)
			{
			    if(mthl_server_launch_req->account_id == g_mthl_create_pdp_param_save[i].account_id)
			    {
			        //ori_acct_idƥ��ɹ�;
					break;
			    }
				else
					continue;
			}

            //ori_acct_idƥ��ʧ�ܣ�
			if(i == MTHL_GPRS_MAX_PDP_NUM)
			{
			    //mthl_server_listen_cnf.account_id = mthl_server_launch_req->account_id;
				mthl_server_listen_cnf.result     = KAL_FALSE;
				mthl_server_listen_cnf.sock_id    = SOC_ERROR;
				mthl_server_listen_cnf.user_data  = mthl_server_launch_req->user_data;
				
				//����cnf;
				mthl_tcp_server_listen_cnf(&mthl_server_listen_cnf);
				return;
			}	

			//Ŀǰֻ֧��ͬʱlaunch 1��Server��check֮ǰ�Ƿ�launch Server��
			if(g_server_info.listen_sock_id != SOC_ERROR)
			{
			    mthl_server_listen_cnf.result     = KAL_FALSE;
				mthl_server_listen_cnf.sock_id    = SOC_ERROR;
				mthl_server_listen_cnf.user_data  = mthl_server_launch_req->user_data;
				
				//����cnf;
				mthl_tcp_server_listen_cnf(&mthl_server_listen_cnf);
				return;  
			}
            //IP��Ҫ�����Լ�ָ����ֻУ��port���ɣ�
			if(mthl_server_launch_req->port == 0)
			{
			    mthl_server_listen_cnf.result     = KAL_FALSE;
				mthl_server_listen_cnf.sock_id    = SOC_ERROR;
				mthl_server_listen_cnf.user_data  = mthl_server_launch_req->user_data;
				
				//����cnf;
				mthl_tcp_server_listen_cnf(&mthl_server_listen_cnf);
				return;  
			}
	// soc_create(); //�����׽���ID��
	// soc_bind();
	// soc_listen();
	//soc_accept()//-2
	
	//TCP only;
	error_soc_create = soc_create(SOC_PF_INET, SOC_SOCK_STREAM, 0, MOD_MTHL, g_mthl_create_pdp_param_save[i].nwk_acct_id);
	//TCP ���� fail;
	if(error_soc_create < 0)
	{
	            mthl_server_listen_cnf.result     = KAL_FALSE;
				mthl_server_listen_cnf.sock_id    = SOC_ERROR;
				mthl_server_listen_cnf.user_data  = mthl_server_launch_req->user_data;
				
				//����cnf;
				mthl_tcp_server_listen_cnf(&mthl_server_listen_cnf);
				return;
	}

	//TCP ���� success����:Server listen_sock������Ϊ�����ģ�
	val = KAL_FALSE;
    soc_setsockopt(error_soc_create, SOC_NBIO, &val, sizeof(val));
    val = SOC_READ | SOC_WRITE | SOC_CONNECT | SOC_CLOSE | SOC_ACCEPT;
    soc_setsockopt(error_soc_create, SOC_ASYNC, &val, sizeof(val));
				
    memset(&server_addr, 0x00, sizeof(sockaddr_struct));
	server_addr.sock_type = SOC_SOCK_STREAM;
#if !defined(__MTK_TARGET__) 	
	if(mthl_server_launch_req->port == 0)
		mthl_server_launch_req->port = 8888;//TCP Server port;
	/*if(mthl_server_launch_req->addr[0] ==0)
	{
	     mthl_server_launch_req->addr[0]=10;
		 mthl_server_launch_req->addr[1]=16;
		 mthl_server_launch_req->addr[2]=3;
		 mthl_server_launch_req->addr[3]=11;
		 mthl_server_launch_req->addr_len = 4;
	}*/    
#endif	
	server_addr.port      = mthl_server_launch_req->port;
    if(mthl_server_launch_req->addr_len != 0)
	server_addr.addr_len  = mthl_server_launch_req->addr_len;
	else
		server_addr.addr_len  = 4; //now,only support IPv4;	
#if !defined(__MTK_TARGET__) 
	memcpy(server_addr.addr, mthl_server_launch_req->addr, mthl_server_launch_req->addr_len);
#else
    kal_prompt_trace(MOD_MTHL, "mthl_server_launch_req->addr:%d.%d.%d.%d",
    				mthl_server_launch_req->addr[0],
    				mthl_server_launch_req->addr[1],
    				mthl_server_launch_req->addr[2],
    				mthl_server_launch_req->addr[3]); 
    //#define INADDR_ANY          (u_int32_t)0x00000000
    //memset(server_addr.addr, 0x00, mthl_server_launch_req->addr_len);
    //server_addr.addr_len = 0;
    //INADDR_ANY ����connect fail;
    if(mthl_server_launch_req->addr[0] ==0)
	{
	     kal_prompt_trace(MOD_MTHL, "mthl_server_launch_req->addr[0] ==0 case.");
	     mthl_server_launch_req->addr[0]=127;
		 mthl_server_launch_req->addr[1]=0;
		 mthl_server_launch_req->addr[2]=0;
		 mthl_server_launch_req->addr[3]=1;
	}
	memcpy(server_addr.addr, mthl_server_launch_req->addr, server_addr.addr_len);
#endif

	ret = soc_bind(error_soc_create, &server_addr);//wait event;

	//���첽��
	//success;
	if(ret == SOC_SUCCESS)
	{
	    kal_prompt_trace(MOD_MTHL, "soc_bind(): SOC_SUCCESS case."); 
	}
	//����case fail;
	else
	{
	            mthl_server_listen_cnf.result     = KAL_FALSE;
				mthl_server_listen_cnf.sock_id    = error_soc_create;
				mthl_server_listen_cnf.user_data  = mthl_server_launch_req->user_data;
				
				//����cnf;
				mthl_tcp_server_listen_cnf(&mthl_server_listen_cnf);
				//close socket;
				soc_close(error_soc_create);
				return;
	}

	ret = soc_listen(error_soc_create, 5);

	if(ret != SOC_WOULDBLOCK)
	{
	    //�������سɹ���
	    if(ret == SOC_SUCCESS)
	    {
	       		kal_prompt_trace(MOD_MTHL, "soc_listen(): SOC_SUCCESS case."); 
		        mthl_server_listen_cnf.result     = KAL_TRUE;
				mthl_server_listen_cnf.sock_id    = error_soc_create;
				mthl_server_listen_cnf.user_data  = mthl_server_launch_req->user_data;
				
				//����cnf;
				mthl_tcp_server_listen_cnf(&mthl_server_listen_cnf);
				
				//�����ɹ��󣬱���listen_sock_id���ټ���accept;
				g_server_info.listen_sock_id = error_soc_create;
	    }
		//��������ʧ�ܣ�
		else
		{
		        mthl_server_listen_cnf.result     = KAL_FALSE;
				mthl_server_listen_cnf.sock_id    = error_soc_create;
				mthl_server_listen_cnf.user_data  = mthl_server_launch_req->user_data;
				
				//��������cnf;
				mthl_tcp_server_listen_cnf(&mthl_server_listen_cnf);
				//close socket;
				soc_close(error_soc_create);
				return;
		}
	}
	else
	{
	    //Server listen_sock��Ĭ���������ģ����Բ��᷵��SOC_WOULDBLOCK��
	    kal_prompt_trace(MOD_MTHL, "soc_listen(): SOC_WOULDBLOCK error case."); 
	}

    //accept֮ǰ��������Ϊ�첽�������ģ�
    val = KAL_TRUE;
    soc_setsockopt(error_soc_create, SOC_NBIO, &val, sizeof(val));
    val = SOC_READ | SOC_WRITE | SOC_CONNECT | SOC_CLOSE | SOC_ACCEPT;
    soc_setsockopt(error_soc_create, SOC_ASYNC, &val, sizeof(val));
	//Server������ֻ������soc_accept()һ�Σ�
	ret = soc_accept(error_soc_create, &conn_addr);
	
	//�������ؽ����
	if(ret != SOC_WOULDBLOCK)
	{
	    //fail;
	    if(ret < 0)
	    {
	            mthl_server_accept_ind.conn_sock_id   = SOC_ERROR;
				//mthl_server_accept_ind.addrΪ��ʼֵ0��
				
				//����cnf;
				mthl_tcp_server_accept_ind(&mthl_server_accept_ind);
				return;
	    }
		//success conn_sock_id;
		else
		{
		      mthl_server_accept_ind.conn_sock_id = ret;
			  //�ҵ�һ�����õ�socket_info[j]���ٱ�����Ϣ��
					for(j=0;j<MTHL_MAX_IP_SOCKET_NUM;j++)
					{
					   if(g_mthl_create_conn_soc_save[i].socket_info[j].sock_id == SOC_ERROR)
					   {
					        kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): find free sock_info, param:");
							
							g_mthl_create_conn_soc_save[i].socket_info[j].sock_id      = mthl_server_accept_ind.conn_sock_id;
							kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): g_mthl_create_conn_soc_save[%d].socket_info[%d].sock_id = %d", i,j,g_mthl_create_conn_soc_save[i].socket_info[j].sock_id);
							
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.sock_type  = SOC_SOCK_STREAM;
							kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): TCP case.");

				            //TCP ���ñ���req��������Ҫ���ӵ�Server��ַ��
							/*g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[0] = mthl_create_conn_req->addr.addr[0];
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[1] = mthl_create_conn_req->addr.addr[1];
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[2] = mthl_create_conn_req->addr.addr[2];
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[3] = mthl_create_conn_req->addr.addr[3];
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.port    = mthl_create_conn_req->addr.port;*/
							
							//������Ϣok���ټ�����
							g_mthl_create_conn_soc_save[i].sock_num++;
							kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): g_mthl_create_conn_soc_save[%d].sock_num = %d",i,g_mthl_create_conn_soc_save[i].sock_num);

							//Ϊ����ͳ���ٱ���һ�ݣ�
							g_mthl_create_conn_soc_save[i].socket_statistics_info[j].sock_id = mthl_server_accept_ind.conn_sock_id;
							break;
					   }
					   else
					   	continue;
					}
                    //�������ӳɹ�������ʧ�ܣ�
					if(j == MTHL_MAX_IP_SOCKET_NUM)
					{
							mthl_server_accept_ind.conn_sock_id = SOC_LIMIT_RESOURCE; 
							//memcpy(&(mthl_server_accept_ind.addr), &conn_addr, sizeof(sockaddr_struct));//���ֳ�ֵ0���ɣ�
							
							kal_prompt_trace(MOD_MTHL, "soc_accept(): success ,but save conn_sock_id fail case.");

							 //����cnf;
			  				mthl_tcp_server_accept_ind(&mthl_server_accept_ind);
							//close socket;
							soc_close(mthl_server_accept_ind.conn_sock_id);
							return;
					}
					//�������ӳɹ�������ɹ���
					else
					{
					        memcpy(&(mthl_server_accept_ind.addr), &conn_addr, sizeof(sockaddr_struct));
							kal_prompt_trace(MOD_MTHL, "soc_accept(): success conn_sock_id case.");
							
						    //����cnf;
			  				mthl_tcp_server_accept_ind(&mthl_server_accept_ind);
							return;
					}
			  //memcpy(&(mthl_server_accept_ind.addr), &conn_addr, sizeof(sockaddr_struct));
			  //kal_prompt_trace(MOD_MTHL, "soc_accept(): success conn_sock_id case."); 

			  //����cnf;
			  //mthl_tcp_server_accept_ind(&mthl_server_accept_ind);
		}			
	}
	//�����첽֪ͨ��//����SOC�ϱ���IND��
	else
	{
	    //�첽֪ͨʱ���ȱ���һ��account_id���Ա����ƥ��ʹ�ã�
	    g_server_info.server_account_id = mthl_server_launch_req->account_id;
	    kal_prompt_trace(MOD_MTHL, "soc_accept(): SOC_WOULDBLOCK case."); 
	}

	return;	
}



void mthl_send_create_pdp_param_cnf(mthl_create_pdp_param_cnf_struct *mthl_create_pdp_param_cnf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_create_pdp_param_cnf_struct *create_pdp_param_cnf = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    create_pdp_param_cnf = (mthl_create_pdp_param_cnf_struct *)construct_local_para(sizeof(mthl_create_pdp_param_cnf_struct), TD_RESET);
	//memset(create_pdp_param_cnf, 0x00, sizeof(mthl_create_pdp_param_cnf_struct));

    create_pdp_param_cnf->account_id = mthl_create_pdp_param_cnf->account_id;
	create_pdp_param_cnf->result     = mthl_create_pdp_param_cnf->result;
	create_pdp_param_cnf->user_data  = mthl_create_pdp_param_cnf->user_data;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_CREATE_PDP_PARAM_CNF, 
                                            MOD_L4C, 0, (local_para_struct*) create_pdp_param_cnf);
}

void mthl_send_query_pdp_param_cnf(mthl_query_pdp_param_cnf_struct *mthl_query_pdp_param_cnf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_query_pdp_param_cnf_struct *query_pdp_param_cnf = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    query_pdp_param_cnf = (mthl_query_pdp_param_cnf_struct *)construct_local_para(sizeof(mthl_query_pdp_param_cnf_struct), TD_RESET);
	//memset(query_pdp_param_cnf, 0x00, sizeof(mthl_query_pdp_param_cnf_struct));

    query_pdp_param_cnf->result   = mthl_query_pdp_param_cnf->result;
	query_pdp_param_cnf->pdp_param_num = mthl_query_pdp_param_cnf->pdp_param_num;
	memcpy(query_pdp_param_cnf->pdp_param_info, mthl_query_pdp_param_cnf->pdp_param_info, sizeof(mthl_pdp_param_info_struct)*MTHL_GPRS_MAX_PDP_NUM);
    query_pdp_param_cnf->user_data = mthl_query_pdp_param_cnf->user_data;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_GET_PDP_PARAM_CNF, 
                                            MOD_L4C, 0, (local_para_struct*) query_pdp_param_cnf); 
}
void mthl_send_activate_pdp_cnf(mthl_activate_pdp_cnf_struct * mthl_activate_pdp_cnf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_activate_pdp_cnf_struct *activate_pdp_cnf = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    activate_pdp_cnf = (mthl_activate_pdp_cnf_struct *)construct_local_para(sizeof(mthl_activate_pdp_cnf_struct), TD_RESET);
	//memset(activate_pdp_cnf, 0x00, sizeof(mthl_activate_pdp_cnf_struct));

    activate_pdp_cnf->account_id  = mthl_activate_pdp_cnf->account_id;
	activate_pdp_cnf->result      = mthl_activate_pdp_cnf->result;
	activate_pdp_cnf->error_cause = mthl_activate_pdp_cnf->error_cause;
	activate_pdp_cnf->user_data   = mthl_activate_pdp_cnf->user_data;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_ACTIVATE_PDP_CNF, 
                                            MOD_L4C, 0, (local_para_struct*) activate_pdp_cnf);
}

#if defined(__WIFI_TCPIP_AT_SUPPORT__)
void mthl_send_activate_wifi_cnf(mthl_wifi_activate_cnf_struct * mthl_wifi_activate_cnf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_wifi_activate_cnf_struct *wifi_activate_cnf = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    wifi_activate_cnf = (mthl_wifi_activate_cnf_struct *)construct_local_para(sizeof(mthl_wifi_activate_cnf_struct), TD_RESET);
	//memset(activate_pdp_cnf, 0x00, sizeof(mthl_activate_pdp_cnf_struct));

    wifi_activate_cnf->result    = mthl_wifi_activate_cnf->result;
	wifi_activate_cnf->user_data = mthl_wifi_activate_cnf->user_data;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_ACTIVATE_WIFI_CNF, 
                                            MOD_L4C, 0, (local_para_struct*) wifi_activate_cnf);
}
#endif

void mthl_send_deactivate_pdp_ind(mthl_deactivate_pdp_ind_struct * mthl_deactivate_pdp_ind)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_deactivate_pdp_ind_struct *deactivate_pdp_ind = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    deactivate_pdp_ind = (mthl_deactivate_pdp_ind_struct *)construct_local_para(sizeof(mthl_deactivate_pdp_ind_struct), TD_RESET);
	//memset(deactivate_pdp_ind, 0x00, sizeof(mthl_deactivate_pdp_ind_struct));

    deactivate_pdp_ind->account_id   =  mthl_deactivate_pdp_ind->account_id;
	deactivate_pdp_ind->error        =  mthl_deactivate_pdp_ind->error;
	deactivate_pdp_ind->error_cause  =  mthl_deactivate_pdp_ind->error_cause;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_DEACTIVATED_PDP_IND, 
                                            MOD_L4C, 0, (local_para_struct*) deactivate_pdp_ind);
}

#if defined(__WIFI_TCPIP_AT_SUPPORT__)
void mthl_send_deactivate_wifi_ind(mthl_wifi_deactivate_ind_struct * mthl_wifi_deactivate_ind)
{
		/*----------------------------------------------------------------*/
		/* Local Variables												  */
		/*----------------------------------------------------------------*/
		mthl_wifi_deactivate_ind_struct *wifi_deactivate_ind = NULL;
	
		/*----------------------------------------------------------------*/
		/* Code Body													  */
		/*----------------------------------------------------------------*/
		wifi_deactivate_ind = (mthl_wifi_deactivate_ind_struct *)construct_local_para(sizeof(mthl_wifi_deactivate_ind_struct), TD_RESET);
		//memset(deactivate_pdp_ind, 0x00, sizeof(mthl_deactivate_pdp_ind_struct));
	
		wifi_deactivate_ind->error		 =	mthl_wifi_deactivate_ind->error;
		wifi_deactivate_ind->error_cause  =	mthl_wifi_deactivate_ind->error_cause;
	
		mthl_send_msg2atci(MSG_ID_APP_MTHL_DEACTIVATE_WIFI_IND, 
												MOD_L4C, 0, (local_para_struct*) wifi_deactivate_ind);

}
#endif
void mthl_send_deactivate_pdp_cnf(mthl_deactivate_pdp_cnf_struct * mthl_deactivate_pdp_cnf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_deactivate_pdp_cnf_struct *deactivate_pdp_cnf = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    deactivate_pdp_cnf = (mthl_deactivate_pdp_cnf_struct *)construct_local_para(sizeof(mthl_deactivate_pdp_cnf_struct), TD_RESET);	
	//memset(deactivate_pdp_cnf, 0x00, sizeof(mthl_deactivate_pdp_cnf_struct));

    deactivate_pdp_cnf->account_id    =  mthl_deactivate_pdp_cnf->account_id;
	deactivate_pdp_cnf->result        =  mthl_deactivate_pdp_cnf->result;
	deactivate_pdp_cnf->error_cause   =  mthl_deactivate_pdp_cnf->error_cause;
	deactivate_pdp_cnf->user_data     =  mthl_deactivate_pdp_cnf->user_data;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_DEACTIVATE_PDP_CNF, 
                                            MOD_L4C, 0, (local_para_struct*) deactivate_pdp_cnf);
}

#if defined(__WIFI_TCPIP_AT_SUPPORT__)
void mthl_send_deactivate_wifi_cnf(mthl_wifi_deactivate_cnf_struct * mthl_wifi_deactivate_cnf)
{
		/*----------------------------------------------------------------*/
		/* Local Variables												  */
		/*----------------------------------------------------------------*/
		mthl_wifi_deactivate_cnf_struct *wifi_deactivate_cnf = NULL;
	
		/*----------------------------------------------------------------*/
		/* Code Body													  */
		/*----------------------------------------------------------------*/
		wifi_deactivate_cnf = (mthl_wifi_deactivate_cnf_struct *)construct_local_para(sizeof(mthl_wifi_deactivate_cnf_struct), TD_RESET);	
		//memset(deactivate_pdp_cnf, 0x00, sizeof(mthl_deactivate_pdp_cnf_struct));
	
		wifi_deactivate_cnf->result  	= mthl_wifi_deactivate_cnf->result;
		wifi_deactivate_cnf->user_data  = mthl_wifi_deactivate_cnf->user_data;
	
		mthl_send_msg2atci(MSG_ID_APP_MTHL_DEACTIVATE_WIFI_CNF, 
												MOD_L4C, 0, (local_para_struct*) wifi_deactivate_cnf);
}
#endif
void mthl_send_query_pdp_state_cnf(mthl_query_pdp_state_cnf_struct *mthl_query_pdp_state_cnf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_query_pdp_state_cnf_struct *query_pdp_state_cnf = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    query_pdp_state_cnf = (mthl_query_pdp_state_cnf_struct *)construct_local_para(sizeof(mthl_query_pdp_state_cnf_struct), TD_RESET);
	//memset(query_pdp_state_cnf, 0x00, sizeof(mthl_query_pdp_param_cnf_struct));

    query_pdp_state_cnf->result   = mthl_query_pdp_state_cnf->result;
	query_pdp_state_cnf->num      = mthl_query_pdp_state_cnf->num;
	memcpy(query_pdp_state_cnf->state_list, mthl_query_pdp_state_cnf->state_list, sizeof(kal_uint8)*3*2);
	query_pdp_state_cnf->user_data = mthl_query_pdp_state_cnf->user_data;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_GET_PDP_STATE_CNF, 
                                            MOD_L4C, 0, (local_para_struct*) query_pdp_state_cnf); 
}
#if defined(__WIFI_TCPIP_AT_SUPPORT__)
void mthl_send_query_wifi_state_cnf(mthl_wifi_get_state_cnf_struct *mthl_wifi_get_state_cnf)
{
		/*----------------------------------------------------------------*/
		/* Local Variables												  */
		/*----------------------------------------------------------------*/
		mthl_wifi_get_state_cnf_struct *wifi_get_state_cnf = NULL;
	
		/*----------------------------------------------------------------*/
		/* Code Body													  */
		/*----------------------------------------------------------------*/
		wifi_get_state_cnf = (mthl_wifi_get_state_cnf_struct *)construct_local_para(sizeof(mthl_wifi_get_state_cnf_struct), TD_RESET);
		//memset(query_pdp_state_cnf, 0x00, sizeof(mthl_query_pdp_param_cnf_struct));
	
		wifi_get_state_cnf->result   = mthl_wifi_get_state_cnf->result;
		wifi_get_state_cnf->state    = mthl_wifi_get_state_cnf->result;
		wifi_get_state_cnf->user_data = mthl_wifi_get_state_cnf->user_data;
	
		mthl_send_msg2atci(MSG_ID_APP_MTHL_GET_WIFI_STATE_CNF, 
												MOD_L4C, 0, (local_para_struct*) wifi_get_state_cnf); 

}
#endif
void mthl_send_create_conn_cnf(mthl_create_conn_cnf_struct *mthl_create_conn_cnf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_create_conn_cnf_struct *create_conn_cnf = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    create_conn_cnf = (mthl_create_conn_cnf_struct *)construct_local_para(sizeof(mthl_create_conn_cnf_struct), TD_RESET);
	//memset(create_conn_cnf, 0x00, sizeof(mthl_create_conn_cnf_struct));

    create_conn_cnf->account_id   = mthl_create_conn_cnf->account_id;
	create_conn_cnf->result       = mthl_create_conn_cnf->result;
	create_conn_cnf->sock_id      = mthl_create_conn_cnf->sock_id;
	create_conn_cnf->user_data    = mthl_create_conn_cnf->user_data;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_CREATE_CONN_CNF, 
                                            MOD_L4C, 0, (local_para_struct*) create_conn_cnf);
}

void mthl_query_sock_num_cnf(mthl_query_sock_account_info_cnf_struct *mthl_query_sock_account_info_cnf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_query_sock_account_info_cnf_struct *query_sock_account_info_cnf = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    query_sock_account_info_cnf = (mthl_query_sock_account_info_cnf_struct *)construct_local_para(sizeof(mthl_query_sock_account_info_cnf_struct), TD_RESET);
	//memset(query_sock_account_info_cnf, 0x00, sizeof(mthl_query_sock_account_info_cnf_struct));

    query_sock_account_info_cnf->account_id = mthl_query_sock_account_info_cnf->account_id;
	query_sock_account_info_cnf->result     = mthl_query_sock_account_info_cnf->result;
	
	if(query_sock_account_info_cnf->result == KAL_TRUE)
	{
		memcpy(query_sock_account_info_cnf->socket_info, mthl_query_sock_account_info_cnf->socket_info, sizeof(sock_account_info_struct)*MTHL_MAX_IP_SOCKET_NUM);
		query_sock_account_info_cnf->sock_num   = mthl_query_sock_account_info_cnf->sock_num;
	}
	
	query_sock_account_info_cnf->user_data  = mthl_query_sock_account_info_cnf->user_data;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_QUERY_SOCK_NUM_CNF, 
                                            MOD_L4C, 0, (local_para_struct*) query_sock_account_info_cnf);
}

void mthl_close_sock_cnf(mthl_close_sock_cnf_struct *mthl_close_cnf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_close_sock_cnf_struct *close_cnf = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    close_cnf = (mthl_close_sock_cnf_struct *)construct_local_para(sizeof(mthl_close_sock_cnf_struct), TD_RESET);
	//memset(close_cnf, 0x00, sizeof(mthl_close_sock_cnf_struct));

    close_cnf->result    = mthl_close_cnf->result;
	close_cnf->sock_id   = mthl_close_cnf->sock_id;
	close_cnf->user_data = mthl_close_cnf->user_data;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_SOCK_CLOSE_CNF, 
                                            MOD_L4C, 0, (local_para_struct*) close_cnf);
}

void mthl_close_sock_ind(mthl_close_sock_ind_struct *mthl_close_ind)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_close_sock_ind_struct *close_ind = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    close_ind = (mthl_close_sock_ind_struct *)construct_local_para(sizeof(mthl_close_sock_ind_struct), TD_RESET);
	//memset(close_cnf, 0x00, sizeof(mthl_close_sock_cnf_struct));

    close_ind->result    = mthl_close_ind->result;
	close_ind->sock_id   = mthl_close_ind->sock_id;
	close_ind->user_data = mthl_close_ind->user_data;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_SOCK_CLOSE_IND, 
                                            MOD_L4C, 0, (local_para_struct*) close_ind);
}
void mthl_send_sock_cnf(mthl_send_data_cnf_struct *mthl_send_data_cnf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_send_data_cnf_struct *send_data_cnf = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    send_data_cnf = (mthl_send_data_cnf_struct *)construct_local_para(sizeof(mthl_send_data_cnf_struct), TD_RESET);
	//memset(send_data_cnf, 0x00, sizeof(mthl_send_data_cnf_struct));

    send_data_cnf->result    = mthl_send_data_cnf->result;
	send_data_cnf->ret_val   = mthl_send_data_cnf->ret_val;
	send_data_cnf->sock_id   = mthl_send_data_cnf->sock_id;
	send_data_cnf->user_data = mthl_send_data_cnf->user_data;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_SOCK_SEND_CNF, 
                                            MOD_L4C, 0, (local_para_struct*) send_data_cnf);
}

void mthl_send_sock_ind(mthl_send_data_ind_struct *mthl_send_data_ind)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_send_data_ind_struct *send_data_ind = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    send_data_ind = (mthl_send_data_ind_struct *)construct_local_para(sizeof(mthl_send_data_ind_struct), TD_RESET);
	//memset(send_data_ind, 0x00, sizeof(mthl_send_data_ind_struct));

    send_data_ind->result    = mthl_send_data_ind->result;
	//send_data_ind->ret_val;
	send_data_ind->sock_id   = mthl_send_data_ind->sock_id;
	send_data_ind->user_data = mthl_send_data_ind->user_data;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_SOCK_SEND_IND, 
                                            MOD_L4C, 0, (local_para_struct*) send_data_ind);
}

void mthl_recv_sock_cnf(mthl_recv_data_cnf_struct *mthl_recv_data_cnf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_recv_data_cnf_struct *recv_data_cnf = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    recv_data_cnf = (mthl_recv_data_cnf_struct *)construct_local_para(sizeof(mthl_recv_data_cnf_struct), TD_RESET);
	//memset(recv_data_cnf, 0x00, sizeof(mthl_recv_data_cnf_struct));

    recv_data_cnf->sock_id = mthl_recv_data_cnf->sock_id;
	recv_data_cnf->result  = mthl_recv_data_cnf->result;
	recv_data_cnf->buff    = mthl_recv_data_cnf->buff;
	recv_data_cnf->ret_val = mthl_recv_data_cnf->ret_val;
	recv_data_cnf->addr    = mthl_recv_data_cnf->addr;
	recv_data_cnf->user_data = mthl_recv_data_cnf->user_data;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_SOCK_RECV_CNF, 
                                            MOD_L4C, 0, (local_para_struct*) recv_data_cnf);
}

void mthl_recv_sock_ind(mthl_recv_data_ind_struct *mthl_recv_data_ind)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_recv_data_ind_struct *recv_data_ind = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    recv_data_ind = (mthl_recv_data_ind_struct *)construct_local_para(sizeof(mthl_recv_data_ind_struct), TD_RESET);
	//memset(recv_data_ind, 0x00, sizeof(mthl_recv_data_ind_struct));

    recv_data_ind->sock_id = mthl_recv_data_ind->sock_id;
	recv_data_ind->result  = mthl_recv_data_ind->result;
	recv_data_ind->buff    = mthl_recv_data_ind->buff;
	recv_data_ind->ret_val = mthl_recv_data_ind->ret_val;
	recv_data_ind->addr    = mthl_recv_data_ind->addr;
	recv_data_ind->user_data = mthl_recv_data_ind->user_data;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_SOCK_RECV_IND, 
                                            MOD_L4C, 0, (local_para_struct*) recv_data_ind);
}

void mthl_query_info_sock_cnf(mthl_query_sock_info_cnf_struct *mthl_query_sock_info_cnf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_query_sock_info_cnf_struct *query_sock_info_cnf = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    query_sock_info_cnf = (mthl_query_sock_info_cnf_struct *)construct_local_para(sizeof(mthl_query_sock_info_cnf_struct), TD_RESET);
	//memset(query_sock_info_cnf, 0x00, sizeof(mthl_query_sock_info_cnf_struct));

    query_sock_info_cnf->sock_id   =   mthl_query_sock_info_cnf->sock_id;
	query_sock_info_cnf->result    =   mthl_query_sock_info_cnf->result;
	query_sock_info_cnf->total_send_size = mthl_query_sock_info_cnf->total_send_size;
	query_sock_info_cnf->total_recv_size = mthl_query_sock_info_cnf->total_recv_size;
	query_sock_info_cnf->Ack_value       = mthl_query_sock_info_cnf->Ack_value;
	query_sock_info_cnf->user_data       = mthl_query_sock_info_cnf->user_data;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_QUERY_SOCK_INFO_CNF, 
                                            MOD_L4C, 0, (local_para_struct*) query_sock_info_cnf);
}

void mthl_get_host_by_name_soc_cnf(mthl_get_host_by_name_cnf_struct *mthl_get_host_by_name_cnf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_get_host_by_name_cnf_struct *get_host_by_name_cnf = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    get_host_by_name_cnf = (mthl_get_host_by_name_cnf_struct *)construct_local_para(sizeof(mthl_get_host_by_name_cnf_struct), TD_RESET);
	//memset(get_host_by_name_cnf, 0x00, sizeof(mthl_get_host_by_name_cnf_struct));

    get_host_by_name_cnf->result     = mthl_get_host_by_name_cnf->result;
	
	get_host_by_name_cnf->domain_len = mthl_get_host_by_name_cnf->domain_len;
	if(get_host_by_name_cnf->domain_len > 0)
	   strncpy(get_host_by_name_cnf->domain_name, mthl_get_host_by_name_cnf->domain_name, get_host_by_name_cnf->domain_len);
	
	get_host_by_name_cnf->addr_len   = mthl_get_host_by_name_cnf->addr_len;
	if(get_host_by_name_cnf->addr_len > 0)
	   strncpy(get_host_by_name_cnf->addr, mthl_get_host_by_name_cnf->addr, get_host_by_name_cnf->addr_len);
	
	get_host_by_name_cnf->user_data  = mthl_get_host_by_name_cnf->user_data;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_GET_HOST_BY_NAME_CNF, 
                                            MOD_L4C, 0, (local_para_struct*) get_host_by_name_cnf);
}

void mthl_get_host_by_addr_soc_cnf(mthl_get_host_by_addr_cnf_struct *mthl_get_host_by_addr_cnf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_get_host_by_addr_cnf_struct *get_host_by_addr_cnf = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    get_host_by_addr_cnf = (mthl_get_host_by_addr_cnf_struct *)construct_local_para(sizeof(mthl_get_host_by_addr_cnf_struct), TD_RESET);
	//memset(get_host_by_addr_cnf, 0x00, sizeof(mthl_get_host_by_addr_cnf_struct));

    get_host_by_addr_cnf->result     = mthl_get_host_by_addr_cnf->result;
	
	get_host_by_addr_cnf->domain_len = mthl_get_host_by_addr_cnf->domain_len;
	if(get_host_by_addr_cnf->domain_len > 0)
	   strncpy(get_host_by_addr_cnf->domain_name, mthl_get_host_by_addr_cnf->domain_name, get_host_by_addr_cnf->domain_len);
	
	get_host_by_addr_cnf->addr_len   = mthl_get_host_by_addr_cnf->addr_len;
	if(get_host_by_addr_cnf->addr_len > 0)
	   strncpy(get_host_by_addr_cnf->addr, mthl_get_host_by_addr_cnf->addr, get_host_by_addr_cnf->addr_len);
	
	get_host_by_addr_cnf->user_data  = mthl_get_host_by_addr_cnf->user_data;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_GET_HOST_BY_ADDR_CNF, 
                                            MOD_L4C, 0, (local_para_struct*) get_host_by_addr_cnf);
}

void mthl_tcp_server_listen_cnf(mthl_server_listen_cnf_struct *mthl_server_listen_cnf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_server_listen_cnf_struct *server_listen_cnf = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    server_listen_cnf = (mthl_server_listen_cnf_struct *)construct_local_para(sizeof(mthl_server_listen_cnf_struct), TD_RESET);
	//memset(server_listen_cnf, 0x00, sizeof(mthl_server_listen_cnf_struct));

    server_listen_cnf->result  	 = mthl_server_listen_cnf->result;
	server_listen_cnf->sock_id   = mthl_server_listen_cnf->sock_id;
	server_listen_cnf->user_data = mthl_server_listen_cnf->user_data;

    mthl_send_msg2atci(MSG_ID_APP_MTHL_SERVER_LISTEN_CNF, 
                                            MOD_L4C, 0, (local_para_struct*) server_listen_cnf);
}

void mthl_tcp_server_accept_ind(mthl_server_accept_ind_struct *mthl_server_accept_ind)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_server_accept_ind_struct *server_accept_ind = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    server_accept_ind = (mthl_server_accept_ind_struct *)construct_local_para(sizeof(mthl_server_accept_ind_struct), TD_RESET);
	//memset(server_accept_ind, 0x00, sizeof(mthl_server_accept_ind_struct));

    server_accept_ind->conn_sock_id = mthl_server_accept_ind->conn_sock_id;
	memcpy(&(server_accept_ind->addr), &(mthl_server_accept_ind->addr), sizeof(sockaddr_struct));  

    mthl_send_msg2atci(MSG_ID_APP_MTHL_SERVER_ACCEPT_IND, 
                                            MOD_L4C, 0, (local_para_struct*) server_accept_ind);
}



//common API:
void mthl_send_msg2atci(msg_type msg_id, 
                         module_type dst_mod_id, 
                         sap_type      sap_id,
                         local_para_struct *p_local_para)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct *ilm_ptr = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ilm_ptr = allocate_ilm(MOD_MTHL);
    ilm_ptr->src_mod_id = MOD_MTHL;
    ilm_ptr->dest_mod_id = dst_mod_id;
    ilm_ptr->msg_id = msg_id;
    ilm_ptr->sap_id = sap_id;
    ilm_ptr->local_para_ptr = p_local_para;
    ilm_ptr->peer_buff_ptr = NULL;
    SEND_ILM(MOD_MTHL, dst_mod_id, sap_id, ilm_ptr);
}

void mthl_send_abm_acc_info(srv_dtcnt_db_trans_type_enum opcode, kal_uint32 account_id, cbm_bearer_enum bearer_type)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mmi_abm_update_account_info_req_struct *info_req = 
            (mmi_abm_update_account_info_req_struct *)construct_local_para(sizeof(mmi_abm_update_account_info_req_struct), TD_RESET);
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (opcode == SRV_DTCNT_DB_TRANS_ADD)
    {
        info_req->action = MMI_ABM_PROFILE_ADD;
        info_req->account_id = account_id;
        info_req->bearer_type = bearer_type;
    }
    else /* DELETE */
    {
		/* APP_INFO_MODIFY */
		if((bearer_type & CBM_CSD) || (bearer_type & CBM_GPRS) || (bearer_type & CBM_EDGE) || (bearer_type & CBM_UMTS))
		{
	        info_req->action = MMI_ABM_PROFILE_DEL;
	        info_req->account_id = account_id;
	        info_req->bearer_type = bearer_type;
		}
        else
        {
            free_local_para((local_para_struct *)info_req);
            return;
        }
    }
	
    mthl_send_msg_to_abm(MSG_ID_MMI_ABM_UPDATE_ACCOUNT_INFO_REQ, (void *)info_req, MOD_ABM);
}

void mthl_send_msg_to_abm(kal_uint16 msg_id, void *req, int mod_dst)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct *ilm_send;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ilm_send = allocate_ilm(MOD_MTHL);   
    ilm_send->src_mod_id = MOD_MTHL;
    ilm_send->dest_mod_id = (module_type)mod_dst;
    ilm_send->sap_id = 0;
    ilm_send->msg_id = (msg_type) msg_id;
    ilm_send->local_para_ptr = (local_para_struct*) req;
    ilm_send->peer_buff_ptr = (peer_buff_struct*) NULL;    
    msg_send_ext_queue(ilm_send);    
}

//add for transparent transport mode begin

static void mthl_transparent_mode_uart_set_owner_id(UART_PORT port, module_type ownerid)
{

	DCL_HANDLE handle;
	UART_CTRL_OWNER_T data;
	data.u4OwenrId = ownerid;

	handle = DclSerialPort_Open(port,0);
	DclSerialPort_Control(handle,SIO_CMD_SET_OWNER, (DCL_CTRL_DATA_T*)&data);

}

static module_type mthl_transparent_mode_uart_get_owner_id(UART_PORT port)
{


	DCL_HANDLE handle;
	UART_CTRL_OWNER_T data;

#ifndef __MTK_TARGET__
	return MOD_MTHL;
#endif

	handle = DclSerialPort_Open(port,0);
	DclSerialPort_Control(handle,SIO_CMD_GET_OWNER_ID, (DCL_CTRL_DATA_T*)&data);
	
	kal_prompt_trace(MOD_MTHL, "mthl_transparent_mode_uart_get_owner_id: = %d, MOD_MTHL= %d",data.u4OwenrId,MOD_MTHL);
	
	return (module_type)(data.u4OwenrId);
	
}

static kal_uint16 mthl_transparent_mode_uart_put_bytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, module_type ownerid)
{

	DCL_HANDLE handle;
	UART_CTRL_PUT_BYTES_T data;

	data.u4OwenrId = ownerid;
	data.u2Length = Length;
	data.puBuffaddr = Buffaddr;

    handle = DclSerialPort_Open(port,0);
    DclSerialPort_Control(handle,SIO_CMD_PUT_BYTES, (DCL_CTRL_DATA_T*)&data);
    return data.u2RetSize;
}

static kal_uint16 mthl_transparent_mode_uart_get_bytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, kal_uint8 *status, module_type ownerid)
{

    DCL_HANDLE handle;
    UART_CTRL_GET_BYTES_T data;

    data.u4OwenrId = ownerid;
    data.u2Length = Length;
    data.puBuffaddr = Buffaddr;
    data.pustatus = status;
    handle = DclSerialPort_Open(port,0);
    DclSerialPort_Control(handle,SIO_CMD_GET_BYTES, (DCL_CTRL_DATA_T*)&data);
    return data.u2RetSize;

}


kal_bool mthl_get_transparent_mode_status()
{
    
	kal_prompt_trace(MOD_MTHL, "mthl_get_transparent_mode_status = %d",is_mthl_in_transparent_mode);
    return is_mthl_in_transparent_mode;
}

void mthl_set_transparent_mode_status(kal_bool status)
{
    is_mthl_in_transparent_mode = status;
}

void mthl_transparent_req_get_udp_address(int i,int j)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	if(mthl_transparent_mode_context.addr.addr_len ==0)
	{
		mthl_transparent_mode_context.addr.addr[0] = g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[0];
		mthl_transparent_mode_context.addr.addr[1] = g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[1];
		mthl_transparent_mode_context.addr.addr[2] = g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[2];
		mthl_transparent_mode_context.addr.addr[3] = g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[3];
		mthl_transparent_mode_context.addr.addr_len = g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr_len;
		mthl_transparent_mode_context.addr.port = g_mthl_create_conn_soc_save[i].socket_info[j].addr.port;
	}
}

socket_type_enum mthl_get_socket_type_by_socket_id(kal_int8 socket_id)
{
    int i,j;
	
	// ����sock_id ��ѯ��Ӧ��socket_type��
	for(i=0; i<MTHL_GPRS_MAX_PDP_NUM; i++)
	{
		for(j=0; j<MTHL_MAX_IP_SOCKET_NUM; j++)
		{
			if(g_mthl_create_conn_soc_save[i].socket_info[j].sock_id== socket_id)
				break;
			else
				continue;
		}
		if(j<MTHL_MAX_IP_SOCKET_NUM && i<MTHL_GPRS_MAX_PDP_NUM)	
		{
		    mthl_transparent_req_get_udp_address(i,j);
			kal_prompt_trace(MOD_MTHL, "mthl_get_socket_type_by_socket_id(): g_mthl_create_conn_soc_save[%d].socket_info[%d].sock_id = %d", i,j,g_mthl_create_conn_soc_save[i].socket_info[j].sock_id);
			return g_mthl_create_conn_soc_save[i].socket_info[j].addr.sock_type;
		}
		else
			continue;
	}
	
		 //sock_idƥ��ʧ�ܣ�
	if(i == MTHL_GPRS_MAX_PDP_NUM)
	{
		return SOC_SOCK_SMS;
	}

	return SOC_SOCK_SMS;
}

kal_bool mthl_check_udp_address_valid(sockaddr_struct *sockaddr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	socket_type_enum sock_type; 
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

	if(sockaddr->port == 0 || sockaddr->addr_len == 0)
		return KAL_FALSE;
	if((sockaddr->addr[0]|sockaddr->addr[1]|sockaddr->addr[2]|sockaddr->addr[3])==0)
		return KAL_FALSE;

	return KAL_TRUE;
}

static void mthl_transparent_init_mem_size()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	if (mthl_transparent_mode_context.ibuff_head_ptr ==NULL) 
    {
        mthl_transparent_mode_context.ibuff_head_ptr = mthl_transparent_mem_alloc();
    
        if (mthl_transparent_mode_context.ibuff_head_ptr ==NULL) 
        {
            mthl_transparent_mode_context.GPRS_FLC_UL_LOCKED = KAL_TRUE;
            return;
        }
        else
        {
            mthl_transparent_mode_context.GPRS_FLC_UL_LOCKED = KAL_FALSE;
        }
    }
	if (mthl_transparent_mode_context.outbuff_head_ptr ==NULL) 
    {
        mthl_transparent_mode_context.outbuff_head_ptr = mthl_transparent_mem_alloc();
    
        if (mthl_transparent_mode_context.outbuff_head_ptr ==NULL) 
        {
            mthl_transparent_mode_context.GPRS_FLC_DL_LOCKED = KAL_TRUE;
            return;
        }
        else
        {
            mthl_transparent_mode_context.GPRS_FLC_DL_LOCKED = KAL_FALSE;
        }
    }
}
	
void mthl_transparent_trans_req_hdlr(mthl_transparent_trans_req_struct *trans_p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	socket_type_enum sock_type; 
	mthl_transparent_trans_cnf_struct *trans_cnf;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

	trans_cnf = (mthl_transparent_trans_cnf_struct*)construct_local_para(sizeof(mthl_transparent_trans_cnf_struct), TD_RESET);
	//memset(trans_cnf, 0x00, sizeof(mthl_transparent_trans_cnf_struct));	
	trans_cnf->sock_id = trans_p->sock_id;
	trans_cnf->port = trans_p->port;
	trans_cnf->user_data = trans_p->user_data;
	mthl_transparent_mode_context.port = trans_p->port;
	
	sock_type = mthl_get_socket_type_by_socket_id(trans_p->sock_id);

	kal_prompt_trace(MOD_MTHL, "mthl_transparent_trans_req_hdlr: sock_type = %d",sock_type);

	if(sock_type==SOC_SOCK_SMS)
	{
		trans_cnf->result = TRA_INVALID_SOCKET;
		goto send_cnf;
	}

	if(sock_type==SOC_SOCK_DGRAM)
	{
	    if(mthl_check_udp_address_valid(&trans_p->addr) == KAL_FALSE)
	    {
		if(mthl_check_udp_address_valid(&mthl_transparent_mode_context.addr)== KAL_FALSE)
		{
				trans_cnf->result = TRA_INVALID_PARAM;
			goto send_cnf;
		}		
	}
		else
		{
			mthl_transparent_mode_context.addr.addr[0] = trans_p->addr.addr[0];
			mthl_transparent_mode_context.addr.addr[1] = trans_p->addr.addr[1];
			mthl_transparent_mode_context.addr.addr[2] = trans_p->addr.addr[2];
			mthl_transparent_mode_context.addr.addr[3] = trans_p->addr.addr[3];
			mthl_transparent_mode_context.addr.addr_len = trans_p->addr.addr_len;
			mthl_transparent_mode_context.addr.port = trans_p->addr.port;
		}
			
	}

	#ifdef __MTK_TARGET__

	//ret= set uart owner
	if(mthl_transparent_mode_uart_get_owner_id(trans_p->port)!= MOD_MTHL)
    {
        //UART_SetOwner (trans_p->port, MOD_MTHL);
		mthl_transparent_mode_uart_set_owner_id(trans_p->port, MOD_MTHL);
    }
	if(mthl_transparent_mode_uart_get_owner_id(trans_p->port)!= MOD_MTHL)
	{
		trans_cnf->result = TRA_GET_PORT_FAIL;
		goto send_cnf;
	}
	#endif
	
	mthl_set_transparent_mode_status(KAL_TRUE);

	kal_prompt_trace(MOD_MTHL, "mthl_transparent_trans_req_hdlr: trans_cnf->result = %d",trans_cnf->result);

	trans_cnf->result = TRA_SUCCESS;
	mthl_transparent_mode_context.sock_id = trans_p->sock_id;
	mthl_transparent_mode_context.sock_type = sock_type;
	//mthl_transparent_mode_context.addr = trans_p->addr;
	mthl_send_msg2atci(MSG_ID_APP_MTHL_TRANSPARENT_TRANS_CNF, MOD_L4C, 0, (local_para_struct*) trans_cnf);
	mthl_transparent_init_mem_size();
	//mthl_transparent_reveive_data();
	mthl_transparent_uart_ready_to_read_ind_hdlr(); /* Read UART */
    //mthl_transparent_ready_to_write_uart_ind_hdlr(); /* Write UART */
		
	return;
		
	send_cnf:

		
	kal_prompt_trace(MOD_MTHL, "mthl_transparent_trans_req_hdlr: trans_cnf->result = %d",trans_cnf->result);

    mthl_send_msg2atci(MSG_ID_APP_MTHL_TRANSPARENT_TRANS_CNF, MOD_L4C, 0, (local_para_struct*) trans_cnf);
    
}

	
/*******************************************************************************
 * FUNCTION
 *    mthl_transparent_mem_alloc
 * DESCRIPTION
 *    This function is used to prepare the decode buff for the PPP framer. 
 *   
 * PARAMETERS
 *     IN/OUT    **flc_buff   
 *     IN        is_data_mode
 *         IN        type
 * RETURNS
 *    ppp_buff_head_struct *
 * GLOBALS AFFECTED
 *    context of ppp framer
 *******************************************************************************/
mthl_transport_buff_head_struct* mthl_transparent_mem_alloc()                    
{
    mthl_transport_buff_head_struct * new_buff_ptr = NULL;
        
    /* Construct PPP input buffer header */
    new_buff_ptr = (mthl_transport_buff_head_struct *)get_ctrl_buffer(sizeof(mthl_transport_buff_head_struct));
    kal_mem_set ((kal_uint8 *) new_buff_ptr, 0, sizeof(mthl_transport_buff_head_struct));


     new_buff_ptr->data_ptr = (kal_uint8 *)get_ctrl_buffer(MAX_CTRL_PKT_SIZE);
        
     kal_mem_set((kal_uint8 *)new_buff_ptr->data_ptr,0, MAX_CTRL_PKT_SIZE);
     new_buff_ptr->size = MAX_CTRL_PKT_SIZE;
     new_buff_ptr->end_ptr = new_buff_ptr->cur_ptr = new_buff_ptr->data_ptr;
    
    return new_buff_ptr;
}

void mthl_transparent_outbuffer_mem_dealloc ()                    
{
	/*----------------------------------------------------------------*/
	/* Local Variables												  */
	/*----------------------------------------------------------------*/
	
	/*----------------------------------------------------------------*/
	/* Code Body													  */
	/*----------------------------------------------------------------*/
	if(mthl_transparent_mode_context.outbuff_head_ptr!=NULL)
	{
		if (mthl_transparent_mode_context.outbuff_head_ptr->data_ptr !=NULL) 
		{
			free_ctrl_buffer(mthl_transparent_mode_context.outbuff_head_ptr->data_ptr);
			mthl_transparent_mode_context.outbuff_head_ptr->data_ptr = NULL;
		}
	}
	if (mthl_transparent_mode_context.outbuff_head_ptr !=NULL) 
	{
		free_ctrl_buffer(mthl_transparent_mode_context.outbuff_head_ptr);
		mthl_transparent_mode_context.outbuff_head_ptr = NULL;
	}	
}

void mthl_transparent_inbuffer_mem_dealloc ()                    
{
	/*----------------------------------------------------------------*/
	/* Local Variables												  */
	/*----------------------------------------------------------------*/
	
	/*----------------------------------------------------------------*/
	/* Code Body													  */
	/*----------------------------------------------------------------*/
	if(mthl_transparent_mode_context.ibuff_head_ptr!=NULL)
	{
		if(mthl_transparent_mode_context.ibuff_head_ptr->data_ptr !=NULL)
		{
			free_ctrl_buffer(mthl_transparent_mode_context.ibuff_head_ptr->data_ptr);
			mthl_transparent_mode_context.ibuff_head_ptr->data_ptr = NULL;
		}
	}
	if (mthl_transparent_mode_context.ibuff_head_ptr !=NULL) 
	{
		free_ctrl_buffer(mthl_transparent_mode_context.ibuff_head_ptr);
		mthl_transparent_mode_context.ibuff_head_ptr = NULL;
	}	
}


void mthl_transparent_mem_dealloc ()                    
{
	/*----------------------------------------------------------------*/
	/* Local Variables												  */
	/*----------------------------------------------------------------*/
    
	/*----------------------------------------------------------------*/
	/* Code Body													  */
	/*----------------------------------------------------------------*/
	mthl_transparent_inbuffer_mem_dealloc();
	mthl_transparent_outbuffer_mem_dealloc();
		mthl_transparent_mode_context.GPRS_FLC_UL_LOCKED = KAL_TRUE;
		mthl_transparent_mode_context.GPRS_FLC_DL_LOCKED = KAL_TRUE;
}


static void mthl_transparent_send_data()
{
    kal_int32 ret;
	int i,j;
    mthl_transport_buff_head_struct *ibuff_head_ptr = mthl_transparent_mode_context.ibuff_head_ptr;

	if(mthl_get_transparent_mode_status()==KAL_FALSE ||(mthl_transparent_mode_uart_get_owner_id (mthl_transparent_mode_context.port)!=MOD_MTHL))
		return;
   //TCP case;
	 if(mthl_transparent_mode_context.sock_type== SOC_SOCK_STREAM)		
	 {
	    // soc_send()	for TCP
	    ret = soc_send(mthl_transparent_mode_context.sock_id, ibuff_head_ptr->cur_ptr, ibuff_head_ptr->used,0);

        //���Ͷ���call�꣬���ͷ�buff������user���ط���
        //free_ctrl_buffer(mthl_send_data_req->data);
		//mthl_send_data_req->data = NULL;
		
		//�������ͳɹ���
		if(ret > 0)
		{
		     ibuff_head_ptr->cur_ptr += ret;
			 ibuff_head_ptr->used -= ret;
			 if(ibuff_head_ptr->used!= 0)
			 {
			 	mthl_transparent_send_data();
			 }
			 else
			 {
			 	mthl_transparent_inbuffer_mem_dealloc();
				mthl_transparent_reveive_data();
				mthl_transparent_uart_ready_to_read_ind_hdlr();
			 }
		}
		//����ָʾ���ͽ����
		else if(ret == SOC_WOULDBLOCK)
		{
		     
		     return;
		}
		//��������ʧ�ܣ�
		else
		{
		    //error
		    mthl_transparent_mem_dealloc();
		    mthl_transparent_send_end_ind(mthl_transparent_mode_context.port,TRANSPARENT_NETWORK_ERROR);
		     return;
		}
	 }

	 //UDP case;
	 if(mthl_transparent_mode_context.sock_type== SOC_SOCK_DGRAM)
	 {
	    //soc_sendto() for UDP
	    //����ʱ��UDP��ǰָ�����յ�ַ��
	    if(mthl_check_udp_address_valid(&mthl_transparent_mode_context.addr)==KAL_TRUE)
	    {
		   ret = soc_sendto(mthl_transparent_mode_context.sock_id, mthl_transparent_mode_context.ibuff_head_ptr->cur_ptr, 
		   	mthl_transparent_mode_context.ibuff_head_ptr->used,0,&mthl_transparent_mode_context.addr);
			//���Ͷ���call�꣬���ͷ�buff������user���ط���
	        //free_ctrl_buffer(mthl_send_data_req->data);
			//mthl_send_data_req->data = NULL;
			
			//�������ͳɹ���
			if(ret > 0)
			{
				mthl_transparent_mode_context.ibuff_head_ptr->cur_ptr += ret;
				mthl_transparent_mode_context.ibuff_head_ptr->used -= ret;
				if(mthl_transparent_mode_context.ibuff_head_ptr->used!= 0)
				{
					mthl_transparent_send_data();
				}
				else
				{
					mthl_transparent_inbuffer_mem_dealloc();
					mthl_transparent_reveive_data();
					mthl_transparent_uart_ready_to_read_ind_hdlr();
				}
			}
			//����ָʾ���ͽ����
			else if(ret == SOC_WOULDBLOCK)
			{
			     return;
			}
			//��������ʧ�ܣ�
			else
			{
			     mthl_transparent_mem_dealloc();
			     mthl_transparent_send_end_ind(mthl_transparent_mode_context.port,TRANSPARENT_NETWORK_ERROR);
			     return;
			}
	    }
		else
		{
        //����ʱ��UDP��ǰָ�����յ�ַ�����Ǵ���ĵ�ַ���Ϸ���
                 mthl_transparent_mem_dealloc();
                 mthl_transparent_send_end_ind(mthl_transparent_mode_context.port,TRANSPARENT_NETWORK_ERROR);
			     return;
		
		}
	 	}
		
	 return;	 
}


static void mthl_transparent_reveive_data()
{
    kal_int32 ret;
	int i,j;
    mthl_transport_buff_head_struct *pDec;
    kal_bool is_frame_decode = KAL_FALSE;
    kal_bool is_release_flc_buff = KAL_FALSE;

    /* Locals in stack */
    kal_uint8 EscDect = 0x00; 
    kal_uint32 is_read_suspend = KAL_FALSE;

	if(mthl_get_transparent_mode_status()==KAL_FALSE ||(mthl_transparent_mode_uart_get_owner_id(mthl_transparent_mode_context.port)!=MOD_MTHL))
		return;
    
    /* The input buff is NULL for new incoming packet */
    if (mthl_transparent_mode_context.outbuff_head_ptr ==NULL) 
    {
        mthl_transparent_mode_context.outbuff_head_ptr = mthl_transparent_mem_alloc();
    
        if (mthl_transparent_mode_context.outbuff_head_ptr ==NULL) 
        {
            mthl_transparent_mode_context.GPRS_FLC_DL_LOCKED = KAL_TRUE;
            return;
        }
        else
        {
        }
    }
	
	pDec = mthl_transparent_mode_context.outbuff_head_ptr;
	
	//TCP case;
	 if(mthl_transparent_mode_context.sock_type== SOC_SOCK_STREAM)		
	 {
	     // soc_recv() for TCP;
	     ret = soc_recv(mthl_transparent_mode_context.sock_id, pDec->cur_ptr, pDec->size,0);
		 kal_prompt_trace(MOD_MTHL, "soc_recv: = %s",pDec->cur_ptr);
         //�������ճɹ���
		 if(ret > 0)
		 {
		     pDec->end_ptr += ret;
			 pDec->used += ret;
			 mthl_transparent_ready_to_write_uart_ind_hdlr();
			 if(pDec->used==0)
			 {
			 	mthl_transparent_reveive_data();
				//mthl_transparent_uart_ready_to_read_ind_hdlr();
			 }
			 else
			 {
			 	//free_ctrl_buffer(mthl_transparent_mode_context.inbuff_context.data_ptr);
				 mthl_transparent_ready_to_write_uart_ind_hdlr();
			 }
		}
		 //����ָʾ���ս����
		 else if(ret == SOC_WOULDBLOCK)
		 {
			  //if(pDec->used>0 && mthl_transparent_mode_context.is_write_suspend == KAL_FALSE)
			  	//mthl_process_output();
			  	//mthl_transparent_ready_to_write_uart_ind_hdlr();
			  	mthl_transparent_uart_ready_to_read_ind_hdlr();
		 }
		 //��������ʧ�ܣ�
		 else
		 {
		      mthl_transparent_mem_dealloc();
			  mthl_transparent_send_end_ind(mthl_transparent_mode_context.port,TRANSPARENT_NETWORK_ERROR);
			  return;
		 }		 	
	 }

	//UDP case;
	if(mthl_transparent_mode_context.sock_type== SOC_SOCK_DGRAM) 
	{
	    //soc_recvfrom ()  for UDP
	    ret = soc_recvfrom(mthl_transparent_mode_context.sock_id, pDec->cur_ptr, pDec->size,0,&mthl_transparent_mode_context.addr);
		kal_prompt_trace(MOD_MTHL, "soc_recvfrom: = %s, ret = %d",pDec->cur_ptr, ret);
         //�������ճɹ���
		 if(ret > 0)
		 {
		     pDec->end_ptr += ret;
			 pDec->used += ret;
			 mthl_transparent_ready_to_write_uart_ind_hdlr();
			 if(pDec->used==0)
			 {
			 	mthl_transparent_reveive_data();
				//mthl_transparent_uart_ready_to_read_ind_hdlr();
			 }
			 else
			 {
			 	//free_ctrl_buffer(mthl_transparent_mode_context.inbuff_context.data_ptr);
				 mthl_transparent_ready_to_write_uart_ind_hdlr();
			 }
		}		 
		 //����ָʾ���ս����
		 else if(ret == SOC_WOULDBLOCK)
		 {
			  //return;
			  mthl_transparent_uart_ready_to_read_ind_hdlr();
		 }
		 //��������ʧ�ܣ�
		 else
		 {
		      mthl_transparent_mem_dealloc();
			  mthl_transparent_send_end_ind(mthl_transparent_mode_context.port,TRANSPARENT_NETWORK_ERROR);
			  return;
		 }		
	}
	 
	 return;	
	
}

    
/*******************************************************************************
 * FUNCTION
 *   mthl_process_input
 * DESCRIPTION
 *   This function is to process data from UART.
 *
 * PARAMETERS
 *    IN/OUT    
 * RETURNS
 *   void
 * GLOBALS AFFECTED
 *   context of mthl_transparent_mode_context
 *******************************************************************************/
static void mthl_process_input ()
{
    /* Locals in Registers */
    mthl_transport_buff_head_struct *pDec;
    kal_bool is_frame_decode = KAL_FALSE;
    kal_bool is_release_flc_buff = KAL_FALSE;

    /* Locals in stack */
    kal_uint8 EscDect = 0x00; 
    kal_uint32 is_read_suspend = KAL_FALSE;

	#ifndef __MTK_TARGET__

	kal_uint32 ret_send=0;
    kal_char CRLF[] = {0x0d,0x0a,0x00};
    kal_char send_buf[80]={0};
    memset(send_buf,0,sizeof(send_buf));
    strcat(send_buf,"GET / HTTP/1.1");
    strcat(send_buf,CRLF);
    strcat(send_buf,"host: www.baidu.com");    
    strcat(send_buf,CRLF);
    strcat(send_buf,"Connection: Closed");
    strcat(send_buf,CRLF);
    strcat(send_buf,CRLF);
	#endif

	kal_prompt_trace(MOD_MTHL, "mthl_process_input.");
    
    /* The input buff is NULL for new incoming packet */
    if (mthl_transparent_mode_context.ibuff_head_ptr ==NULL) 
    {
        mthl_transparent_mode_context.ibuff_head_ptr = mthl_transparent_mem_alloc();
    
        if (mthl_transparent_mode_context.ibuff_head_ptr ==NULL) 
        {
            mthl_transparent_mode_context.GPRS_FLC_UL_LOCKED = KAL_TRUE;
            return;
        }
        else
        {
        }
    }

	pDec = mthl_transparent_mode_context.ibuff_head_ptr;
	
	#ifdef __MTK_TARGET__

    while ((mthl_transparent_mode_uart_get_owner_id(mthl_transparent_mode_context.port)==MOD_MTHL) &&is_read_suspend==KAL_FALSE)
    {           
        if (0 == pDec->used) 
        {
            pDec->used = 
                mthl_transparent_mode_uart_get_bytes(
                    mthl_transparent_mode_context.port,
                    pDec->data_ptr,
                    pDec->size,
                    &EscDect,
                    MOD_MTHL);
            
        
		   kal_prompt_trace(MOD_MTHL, "mthl_transparent_mode_uart_get_bytes:used=%d, data = %s",pDec->used,pDec->data_ptr);
		   if(pDec->used>0)
		   	{       
            pDec->end_ptr = pDec->data_ptr + pDec->used;     
            pDec->cur_ptr = pDec->data_ptr;
     
		        if (pDec->used <= pDec->size)
            {
               // kal_trace (TRACE_WARNING, WARNING_UART_SUSPEND, pDec->used);                
                mthl_transparent_mode_context.is_read_suspend = KAL_TRUE;
                is_read_suspend = KAL_TRUE;
            }
            else
            {
                //kal_trace (TRACE_GROUP_1, WARNING_UART_GETBYTE, pDec->used);
            }
        }
		    else
		    {
		    	return;
		    }
        }

    }

	#endif

	#ifndef __MTK_TARGET__

	pDec->used = strlen(send_buf);
	strncpy(pDec->data_ptr,send_buf, pDec->used);
	pDec->end_ptr = pDec->data_ptr + pDec->used;     
    pDec->cur_ptr = pDec->data_ptr;
	kal_prompt_trace(MOD_MTHL, "mthl_transparent_mode_uart_get_bytes: = %s",pDec->data_ptr);

	#endif

	mthl_transparent_send_data(); 


    return;
}

#if 0
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif

static void mthl_transparent_uart_ready_to_read_ind_hdlr()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

	if (mthl_transparent_mode_context.GPRS_FLC_UL_LOCKED == KAL_TRUE) 
    {
        /* We have no buffer to put */
        return;
    }
	
	mthl_transparent_mode_context.is_read_suspend = KAL_FALSE;
    mthl_process_input();    
}


/*******************************************************************************
 * FUNCTION
 *   pppg_uart_ready_to_write_ind_hdlr
 * DESCRIPTION
 *   This function is to handle UART write resume.
 *
 * PARAMETERS
 *    IN	            ilm_ptr
 *    IN/OUT         ppp_ptr
 * RETURNS
 *   void
 * GLOBALS AFFECTED
 *   N/A
 *******************************************************************************/
void mthl_transparent_ready_to_write_uart_ind_hdlr ()
{
    /* Locals in Registers */
    mthl_transport_buff_head_struct *pDec = mthl_transparent_mode_context.outbuff_head_ptr;
    peer_buff_struct *peerbuff = NULL;

    /* Locals in Stack */
    kal_uint16 w_len = 0;    
    kal_uint8 dl_pool_id;
    kal_uint8 context_id;

	kal_prompt_trace(MOD_MTHL, "mthl_transparent_ready_to_write_uart_ind_hdlr");

	mthl_transparent_mode_context.is_write_suspend = KAL_FALSE ;


    /* We should write the remaining data to the uart first! */
    if (mthl_transparent_mode_uart_get_owner_id(mthl_transparent_mode_context.port)==MOD_MTHL)
    {
        /*w_len = UART_PutBytes (
                mthl_transparent_mode_context.port,
                pDec->cur_ptr,
                pDec->used,
                MOD_MTHL);*/

		
		kal_prompt_trace(MOD_MTHL, "mthl_transparent_ready_to_write_uart_ind_hdlr: = %s",pDec->cur_ptr);

		w_len = mthl_transparent_mode_uart_put_bytes(
                mthl_transparent_mode_context.port,
                pDec->cur_ptr,
                pDec->used,
                MOD_MTHL);

		#ifndef __MTK_TARGET__

		w_len = pDec->used;

		#endif

		kal_prompt_trace(MOD_MTHL, "mthl_transparent_mode_uart_put_bytes: = %d/%d",w_len,pDec->used);

		pDec->used    -= w_len;            
        pDec->cur_ptr += w_len;

        if (w_len < pDec->used) 
        {
            /* We start from cur_ptr while receive UART_READY_TO_WRITE */
            //pDec->is_write_suspend = KAL_TRUE;
            mthl_transparent_ready_to_write_uart_ind_hdlr();
            return;
        } 
        else 
        {
            mthl_transparent_outbuffer_mem_dealloc();
        }
    }// ~ MAUI_02991936, mtk02285, 20111111    
       
}  

 
 
/*******************************************************************************
 * FUNCTION
 *   mthl_transparent_uart_plugout_ind_hdlr
 * DESCRIPTION
 *   This function is to handle UART PLUG OUT IND.
 *
 * PARAMETERS
 *    IN   
 *    IN       
 * RETURNS
 *   void
 * GLOBALS AFFECTED
 *   N/A
 *******************************************************************************/
static void mthl_transparent_send_end_ind(kal_int8 port , transparent_error_cause_enum cause)
{
	/*----------------------------------------------------------------*/
	/* Local Variables												  */
	/*----------------------------------------------------------------*/
	mthl_transparent_trans_end_ind_struct *trans_end_ind;
	/*----------------------------------------------------------------*/
	/* Code Body													  */
	/*----------------------------------------------------------------*/

	mthl_set_transparent_mode_status(KAL_FALSE);
	mthl_transparent_mode_context.is_read_suspend = KAL_TRUE;
	mthl_transparent_mode_context.is_write_suspend = KAL_TRUE;

	mthl_transparent_mem_dealloc();

	
	trans_end_ind = construct_local_para(sizeof(uart_escape_detected_ind_struct), TD_RESET);
	trans_end_ind->sock_id = mthl_transparent_mode_context.sock_id;
	trans_end_ind->port= port;
	trans_end_ind->cause = cause;
	
	kal_prompt_trace(MOD_MTHL, "mthl_transparent_send_end_ind,cause =%d,sock_id=%d",cause,trans_end_ind->sock_id);
	
	mthl_send_msg2atci(MSG_ID_APP_MTHL_TRANSPARENT_TRANS_END_IND, MOD_L4C, 0, (local_para_struct*) trans_end_ind);
	
}


/*******************************************************************************
 * FUNCTION
 *   mthl_transparent_uart_plugout_ind_hdlr
 * DESCRIPTION
 *   This function is to handle UART PLUG OUT IND.
 *
 * PARAMETERS
 *    IN   
 *    IN       
 * RETURNS
 *   void
 * GLOBALS AFFECTED
 *   N/A
 *******************************************************************************/
void mthl_transparent_uart_plugout_ind_hdlr (uart_plugout_ind_struct *plugout_p)
{
	/*----------------------------------------------------------------*/
	/* Local Variables												  */
	/*----------------------------------------------------------------*/
	mthl_transparent_trans_end_ind_struct *trans_end_ind;
	/*----------------------------------------------------------------*/
	/* Code Body													  */
	/*----------------------------------------------------------------*/

	//ret= set uart owner
	if(mthl_transparent_mode_uart_get_owner_id(plugout_p->port)!= MOD_MTHL)
	{
		return;
	}
	mthl_transparent_mem_dealloc();
	mthl_transparent_send_end_ind(plugout_p->port,TRANSPARENT_PLUGOUT_DETECT);	
}



/*******************************************************************************
 * FUNCTION
 *   mthl_transparent_uart_escape_detected_ind_hdlr
 * DESCRIPTION
 *       UART/USB will send UART_ESCAPE_DETECTED_IND to UART/USB owner if
 *       it detect ESCAPE string.
 * PARAMETERS
 *    IN     ilm_ptr
 *    IN     ppp_ptr  
 * RETURNS
 *   void
 * GLOBALS AFFECTED
 *   context of ppp_gprs
 *******************************************************************************/
void mthl_transparent_uart_escape_detected_ind_hdlr(uart_escape_detected_ind_struct *escape_p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	mthl_transparent_trans_end_ind_struct *trans_end_ind;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

	//ret= set uart owner
	if(mthl_transparent_mode_uart_get_owner_id(escape_p->port)!= MOD_MTHL)
    {
        return;
    }
	mthl_transparent_mem_dealloc();
	mthl_transparent_send_end_ind(escape_p->port,TRANSPARENT_ESC_DETECT);	    
}




//add for transparent transport mode end
void mthl_main(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //kal_uint32 account_id = CBM_ALL_NWK_ACCT_ID;  /* for MSG_ID_APP_SOC_DEACTIVATE_REQ */
    //kal_uint8 qos_profile = CBM_ALL_QOS_ID;  /* for MSG_ID_APP_SOC_DEACTIVATE_REQ */

	/*kal_uint8 context_idx;
    kal_uint8 i;
    kal_bool found_context = KAL_FALSE;
    kal_uint8 uart_port;*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch(ilm_ptr->msg_id)
    {
        /* messages from ATCI */
		// PDP����: �������ACTIVATE_PDPʱ����Ҫʹ�õĲ�����
		case MSG_ID_APP_MTHL_CREATE_PDP_PARAM_REQ:
			mthl_send_create_pdp_param_req_hdlr(ilm_ptr);	
			//MSG_ID_APP_MTHL_CREATE_PDP_PARAM_CNF;
			break;		

		//PDP����������ṩ��ѯ���ܣ�
		case MSG_ID_APP_MTHL_GET_PDP_PARAM_REQ:
			mthl_get_pdp_param_req_hdlr(ilm_ptr);
            //MSG_ID_APP_MTHL_GET_PDP_PARAM_CNF
            break;
		//�ڲ�PDP���
		case MSG_ID_APP_MTHL_ACTIVATE_PDP_REQ:
			mthl_send_activate_pdp_req_hdlr(ilm_ptr);
			//MSG_ID_APP_MTHL_ACTIVATE_PDP_CNF;
			break;
        //WIFI����;
    #if defined(__WIFI_TCPIP_AT_SUPPORT__)
		case MSG_ID_APP_MTHL_ACTIVATE_WIFI_REQ:
			mthl_send_activate_wifi_req_hdlr(ilm_ptr);
			//MSG_ID_APP_MTHL_ACTIVATE_WIFI_CNF;
			break;
	#endif		
		
	#if 0
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
	#endif

        //�ڲ�PDPȥ���
		case MSG_ID_APP_MTHL_DEACTIVATE_PDP_REQ:
			mthl_send_deactivate_pdp_req_hdlr(ilm_ptr);
			//MSG_ID_APP_MTHL_DEACTIVATE_PDP_CNF;
			break;
			
		//�ڲ�WIFIȥ���
	#if defined(__WIFI_TCPIP_AT_SUPPORT__)
		case MSG_ID_APP_MTHL_DEACTIVATE_WIFI_REQ:
			mthl_send_deactivate_wifi_req_hdlr(ilm_ptr);
			//MSG_ID_APP_MTHL_DEACTIVATE_WIFI_CNF;
			break;
	#endif
		
        case MSG_ID_APP_CBM_BEARER_INFO_IND:
		{
			app_cbm_bearer_info_ind_struct *bearer_info;
			kal_uint8 ori_acc_id, comp_acc_id;
			int i;
			mthl_activate_pdp_cnf_struct   mthl_activate_pdp_cnf;
			mthl_wifi_activate_cnf_struct  mthl_wifi_activate_cnf;
			mthl_deactivate_pdp_ind_struct mthl_deactivate_pdp_ind;
			mthl_wifi_deactivate_ind_struct mthl_wifi_deactivate_ind;
			mthl_deactivate_pdp_cnf_struct   mthl_deactivate_pdp_cnf;
			mthl_wifi_deactivate_cnf_struct   mthl_wifi_deactivate_cnf;
			
            bearer_info = (app_cbm_bearer_info_ind_struct*)ilm_ptr->local_para_ptr;

		    ori_acc_id = cbm_get_original_account(bearer_info->account_id);
	/**********************************************for wifi handle*******************************************/
	 #if defined(__WIFI_TCPIP_AT_SUPPORT__)		
			if(ori_acc_id == CBM_WIFI_ACCT_ID)
			{
			   if(bearer_info->bearer == CBM_BEARER_WIFI || bearer_info->bearer == CBM_BEARER_NONE)
			   {
			        if(g_mthl_wifi_bearer_info_save.wifi_bear_op == WIFI_ACTIVATE_REQ)
		            {
		               //���ڼ���ȴ�����IND�ϱ��Ľ�����Ȳ�����CNF;
		               if(bearer_info->state == SRV_CBM_ACTIVATING)
		               {
		                  kal_prompt_trace(MOD_MTHL, "activate_wifi_ind_hdlr: ACTIVATING.");
                          return;
		               }
		               //����ɹ���
		               if(bearer_info->state == SRV_CBM_ACTIVATED && bearer_info->error == SRV_CBM_OK)
		               {                    
							mthl_wifi_activate_cnf.result = KAL_TRUE;
							g_mthl_wifi_bearer_info_save.wifi_bear_op  = WIFI_ACTIVATE_CNF;//����ɹ���
		               }
					   //���ڼ���״̬��Ȼ���ϱ�DISC IND;
                       else if(bearer_info->state == SRV_CBM_DEACTIVATED && bearer_info->error == SRV_CBM_BEARER_FAIL)
					   {
						   mthl_wifi_deactivate_ind.error       = bearer_info->error;
						   mthl_wifi_deactivate_ind.error_cause = bearer_info->error_cause;
						   
						   //�첽�ϱ�deactivate ind;
			               mthl_send_deactivate_wifi_ind(&mthl_wifi_deactivate_ind);
						   kal_prompt_trace(MOD_MTHL, "deactivate_wifi_ind_hdlr fail: DISC_IND.");
						   g_mthl_wifi_bearer_info_save.wifi_bear_op = WIFI_DEACTIVATE_IND;

						   return;
					   }
					   //����ʧ�ܣ�
					   else
					   {
					        mthl_wifi_activate_cnf.result = KAL_FALSE;
							g_mthl_wifi_bearer_info_save.wifi_bear_op = WIFI_NONE;//����ʧ�ܣ�
					   }

					   mthl_wifi_activate_cnf.user_data = g_mthl_wifi_bearer_info_save.user_data;
					   
					   //�첽����activate cnf;
					   mthl_send_activate_wifi_cnf(&mthl_wifi_activate_cnf);  
					   //g_mthl_create_pdp_param_save[i].bear_op = PDP_ACTIVATE_CNF;
                       kal_prompt_trace(MOD_MTHL, "activate_wifi_ind_hdlr: result = %d.", mthl_wifi_activate_cnf.result);

					   return;
		            }

			   			   if(g_mthl_wifi_bearer_info_save.wifi_bear_op == WIFI_DEACTIVATE_REQ) 
						   {
						       //����ȥ����ȴ�����IND�ϱ��Ľ�����Ȳ�����CNF;
				               if(bearer_info->state == SRV_CBM_DEACTIVATING)
				               {
				                  kal_prompt_trace(MOD_MTHL, "deactivate_wifi_ind_hdlr: DEACTIVATING.");
		                          return;
				               }
							   //ȥ����ɹ���
							   if(bearer_info->state == SRV_CBM_DEACTIVATED && bearer_info->error == SRV_CBM_OK)
							   {
								   mthl_wifi_deactivate_cnf.result = KAL_TRUE; 
								   g_mthl_wifi_bearer_info_save.wifi_bear_op  = WIFI_DEACTIVATE_CNF;//����ɹ���
							   }
							   //����ȥ����״̬��Ȼ���ϱ�DISC IND;
		                       else if(bearer_info->state == SRV_CBM_DEACTIVATED && bearer_info->error == SRV_CBM_BEARER_FAIL)
							   {
								   mthl_wifi_deactivate_ind.error       = bearer_info->error;
								   mthl_wifi_deactivate_ind.error_cause = bearer_info->error_cause;
								   
								   //�첽�ϱ�deactivate ind;
					               mthl_send_deactivate_wifi_ind(&mthl_wifi_deactivate_ind);
								   kal_prompt_trace(MOD_MTHL, "deactivate_wifi_ind_hdlr fail: DISC_IND.");
								   g_mthl_wifi_bearer_info_save.wifi_bear_op = WIFI_DEACTIVATE_IND;

								   return;
							   }
							   //ȥ����ʧ�ܣ�
							   else
							   {
								   mthl_wifi_deactivate_cnf.result = KAL_FALSE;
								   g_mthl_wifi_bearer_info_save.wifi_bear_op = WIFI_NONE;//ȥ����ʧ�ܣ�
							   }
			   
							   mthl_wifi_deactivate_cnf.user_data = g_mthl_wifi_bearer_info_save.user_data;
							   
							   //�첽����deactivate cnf;
							   mthl_send_deactivate_wifi_cnf(&mthl_wifi_deactivate_cnf);
							   //g_mthl_create_pdp_param_save[i].bear_op = PDP_DEACTIVATE_CNF;
			                   kal_prompt_trace(MOD_MTHL, "deactivate_wifi_ind_hdlr: result = %d.", mthl_wifi_deactivate_cnf.result);
			   
							   return;
						   }

		            //�����ϱ�WIFIȥ�����֪ͨ��MSG_ID_APP_MTHL_DEACTIVATED_PDP_IND;
		            //SRV_CBM_BEARER_FAIL(MSG_ID_L4C_PS_ABM_DISC_IND�ϱ����Ӧ�Ĵ�������)��
		            if(g_mthl_wifi_bearer_info_save.wifi_bear_op  == WIFI_ACTIVATE_CNF || 
					   g_mthl_wifi_bearer_info_save.wifi_bear_op == WIFI_NONE) //�ٴ��ϱ�WIFI bearer ind;
		            {
					   if(bearer_info->state == SRV_CBM_DEACTIVATED && bearer_info->error == SRV_CBM_BEARER_FAIL)
					   {
						   mthl_wifi_deactivate_ind.error       = bearer_info->error;
						   mthl_wifi_deactivate_ind.error_cause = bearer_info->error_cause;
						   
						   //�첽�ϱ�deactivate ind;
			               mthl_send_deactivate_wifi_ind(&mthl_wifi_deactivate_ind);
						   kal_prompt_trace(MOD_MTHL, "deactivate_wifi_ind_hdlr fail: DISC_IND.");
						   g_mthl_wifi_bearer_info_save.wifi_bear_op = WIFI_DEACTIVATE_IND;
						   return;
					   }
					   else
					   {
					       //kal_trace_prompt();
					       //�ٴ��ϱ�����ɹ���
			               if(bearer_info->state == SRV_CBM_ACTIVATED && bearer_info->error == SRV_CBM_OK)
			               {                    
								mthl_wifi_activate_cnf.result = KAL_TRUE;	
								g_mthl_wifi_bearer_info_save.wifi_bear_op  = WIFI_ACTIVATE_CNF;
			               }
						   else
						   {
						        mthl_wifi_activate_cnf.result = KAL_FALSE;
								g_mthl_wifi_bearer_info_save.wifi_bear_op  = WIFI_NONE;//����ʧ�ܣ�
						   }
						   mthl_wifi_activate_cnf.user_data = g_mthl_wifi_bearer_info_save.user_data;
   						   
						   //�첽����activate cnf;
						   mthl_send_activate_wifi_cnf(&mthl_wifi_activate_cnf);  
                           kal_prompt_trace(MOD_MTHL, "activate_wifi_ind_hdlr  result = %d.",mthl_wifi_activate_cnf.result);
						   return;
					   }
					   break;
		            }						   
					
			   }
			   else
			   {
			       kal_prompt_trace(MOD_MTHL, "BEARER_INFO_IND hdlr: WIFI acc_id&bearer not match case, bearer_type = %x", bearer_info->bearer); 
				   mthl_wifi_activate_cnf.result = KAL_FALSE;
				   g_mthl_wifi_bearer_info_save.wifi_bear_op = WIFI_NONE;//����ʧ�ܣ�
					
				   mthl_wifi_activate_cnf.user_data = g_mthl_wifi_bearer_info_save.user_data;
					   
				   //�첽����activate cnf;
				   mthl_send_activate_wifi_cnf(&mthl_wifi_activate_cnf);  

				   return;
			   }
			   
			   break;
			}
	  #endif/*__WIFI_TCPIP_AT_SUPPORT__*/
	/**************************************************************************************************/

	#if defined(__GPRS_TCPIP_AT_SUPPORT__)	
             //PDP hdlr;
		     for(i=0;i<MTHL_GPRS_MAX_PDP_NUM;i++)
		     {
		         comp_acc_id = (kal_uint8)(g_mthl_create_pdp_param_save[i].account_id);
				 
		         if(ori_acc_id == comp_acc_id)
				    break;
				 else
				 	continue;
		     }
			 
		      /*if(i == MTHL_GPRS_MAX_PDP_NUM) 
		      {
		           rsp->result = KAL_FALSE;
				   mthl_send_msg_to_abm(MSG_ID_MMI_ABM_PROFILE_QUERY_RSP, (void *)rsp, MOD_ABM);
				   return;
		      }    */  
			  
            
            if(g_mthl_create_pdp_param_save[i].bear_op == PDP_ACTIVATE_REQ)
            {
               //���ڼ���ȴ�����IND�ϱ��Ľ�����Ȳ�����CNF;
		               if(bearer_info->state == SRV_CBM_ACTIVATING)
		               {
		                  kal_prompt_trace(MOD_MTHL, "activate_pdp_ind_hdlr: ACTIVATING.");
                          return;
		               }
               //����ɹ���
               if(bearer_info->state == SRV_CBM_ACTIVATED && bearer_info->error == SRV_CBM_OK)
               {                    
                    cbm_hold_bearer(g_mthl_atci_app_id);
					mthl_activate_pdp_cnf.result = KAL_TRUE;					
					g_mthl_create_pdp_param_save[i].bear_op = PDP_ACTIVATE_CNF;//����ɹ���
               }
			   else
			   {
			        mthl_activate_pdp_cnf.result = KAL_FALSE;
					g_mthl_create_pdp_param_save[i].bear_op = PDP_NONE;//����ʧ�ܣ�
			   }

               mthl_activate_pdp_cnf.account_id = g_mthl_create_pdp_param_save[i].account_id;		   
			   mthl_activate_pdp_cnf.error_cause = bearer_info->error_cause;
			   mthl_activate_pdp_cnf.user_data = g_mthl_create_pdp_param_save[i].user_data;
			   
			   //�첽����activate cnf;
			   mthl_send_activate_pdp_cnf(&mthl_activate_pdp_cnf);  
			   //g_mthl_create_pdp_param_save[i].bear_op = PDP_ACTIVATE_CNF;
			   return;
            }

			
            if(g_mthl_create_pdp_param_save[i].bear_op == PDP_DEACTIVATE_REQ) 
            {
				//����ȥ����ȴ�����IND�ϱ��Ľ�����Ȳ�����CNF;
				if(bearer_info->state == SRV_CBM_DEACTIVATING)
				{
					kal_prompt_trace(MOD_MTHL, "deactivate_pdp_ind_hdlr: DEACTIVATING.");
					return;
				}
	            //ȥ����ɹ���
				if(bearer_info->state == SRV_CBM_DEACTIVATED && bearer_info->error == SRV_CBM_OK)
				{
				    mthl_deactivate_pdp_cnf.result = KAL_TRUE; 
					g_mthl_create_pdp_param_save[i].bear_op = PDP_DEACTIVATE_CNF;//ȥ����ɹ���
				}
				else
				{
				    mthl_deactivate_pdp_cnf.result = KAL_FALSE;
					g_mthl_create_pdp_param_save[i].bear_op = PDP_NONE;//ȥ����ʧ�ܣ�
				}

				mthl_deactivate_pdp_cnf.account_id = g_mthl_create_pdp_param_save[i].account_id;				
				mthl_deactivate_pdp_cnf.error_cause = bearer_info->error_cause;
				mthl_deactivate_pdp_cnf.user_data = g_mthl_create_pdp_param_save[i].user_data;
				
				//�첽����deactivate cnf;
				mthl_send_deactivate_pdp_cnf(&mthl_deactivate_pdp_cnf);
			    //g_mthl_create_pdp_param_save[i].bear_op = PDP_DEACTIVATE_CNF;
				return;
            }


             //�����ϱ��ڲ�PDPȥ�����֪ͨ��MSG_ID_APP_MTHL_DEACTIVATED_PDP_IND;
            //SRV_CBM_BEARER_FAIL(MSG_ID_L4C_PS_ABM_DISC_IND�ϱ����Ӧ�Ĵ�������)��
            if(g_mthl_create_pdp_param_save[i].bear_op == PDP_ACTIVATE_CNF || 
			   g_mthl_create_pdp_param_save[i].bear_op == PDP_NONE)
            {
			   if(bearer_info->state == SRV_CBM_DEACTIVATED && bearer_info->error == SRV_CBM_BEARER_FAIL)
			   {
			   mthl_deactivate_pdp_ind.account_id  = g_mthl_create_pdp_param_save[i].account_id;
			   mthl_deactivate_pdp_ind.error       = bearer_info->error;
			   mthl_deactivate_pdp_ind.error_cause = bearer_info->error_cause;
			   
			   //�첽�ϱ�deactivate ind;
               mthl_send_deactivate_pdp_ind(&mthl_deactivate_pdp_ind);
			   
			   g_mthl_create_pdp_param_save[i].bear_op = PDP_DEACTIVATE_IND;
			   }
			   else
			   {
			       //kal_trace_prompt();
			       //�ٴ��ϱ�����ɹ���
	               if(bearer_info->state == SRV_CBM_ACTIVATED && bearer_info->error == SRV_CBM_OK)
	               {                    
	                    cbm_hold_bearer(g_mthl_atci_app_id);
						mthl_activate_pdp_cnf.result = KAL_TRUE;	
						g_mthl_create_pdp_param_save[i].bear_op = PDP_ACTIVATE_CNF;
	               }
				   else
				   {
				        mthl_activate_pdp_cnf.result = KAL_FALSE;
						g_mthl_create_pdp_param_save[i].bear_op = PDP_NONE;//����ʧ�ܣ�
				   }

	               mthl_activate_pdp_cnf.account_id = g_mthl_create_pdp_param_save[i].account_id;		   
				   mthl_activate_pdp_cnf.error_cause = bearer_info->error_cause;
				   mthl_activate_pdp_cnf.user_data = g_mthl_create_pdp_param_save[i].user_data;
				   
				   //�첽����activate cnf;  ���ACTIVATING���������ϴ��ϱ�IND ��ֻ�����bear_op���ɣ�
				   if(mthl_activate_pdp_cnf.result == KAL_TRUE)
				   mthl_send_activate_pdp_cnf(&mthl_activate_pdp_cnf);  
				   //g_mthl_create_pdp_param_save[i].bear_op = PDP_ACTIVATE_CNF;

				   //break;
			   }
			   return;
            }
	  #endif/*__GPRS_TCPIP_AT_SUPPORT__*/	
				
            break;		
        }

        //�ڲ�PDP���ȥ����������ṩ��ѯPDP state ���ܣ�
        case MSG_ID_APP_MTHL_GET_PDP_STATE_REQ:
			mthl_get_pdp_state_req_hdlr(ilm_ptr);
			//MSG_ID_APP_MTHL_GET_PDP_STATE_CNF
			break;
		//WIFI���ȥ����������ṩ��ѯWIFI state ���ܣ�
	#if defined(__WIFI_TCPIP_AT_SUPPORT__)
		case MSG_ID_APP_MTHL_GET_WIFI_STATE_REQ:
			mthl_get_wifi_state_req_hdlr(ilm_ptr);
            //MSG_ID_APP_MTHL_GET_WIFI_STATE_CNF
            break;
	#endif
        //opΪ������socket����connect ��
		case MSG_ID_APP_MTHL_CREATE_CONN_REQ:
			mthl_create_and_conn_req_hdlr(ilm_ptr);		
			//MSG_ID_APP_MTHL_CREATE_CONN_CNF;
			break;

		//����conn/send/recv/close/accept ��async case;
        case MSG_ID_APP_SOC_NOTIFY_IND:
			mthl_soc_notify(ilm_ptr->local_para_ptr);
			break;

		
        //opΪ����ѯ�ж��ٸ�socket ��
		case MSG_ID_APP_MTHL_QUERY_SOCK_NUM_REQ:
			mthl_query_sock_num_req_hdlr(ilm_ptr);	
			//MSG_ID_APP_MTHL_QUERY_SOCK_NUM_CNF;
			break;
			
		
        //opΪ���ر�socket ��
		case MSG_ID_APP_MTHL_SOCK_CLOSE_REQ:
			mthl_close_req_hdlr(ilm_ptr);
			//MSG_ID_APP_MTHL_SOCK_CLOSE_CNF;
			break;
			

        //ͨ��socket���������ݡ�
		case MSG_ID_APP_MTHL_SOCK_SEND_REQ:
			mthl_send_req_hdlr(ilm_ptr);	
			//MSG_ID_APP_MTHL_SOCK_SEND_CNF;
		    //MSG_ID_APP_MTHL_SOCK_SEND_IND;
			break;
			

		//ͨ��socket���������ݡ�	
		case MSG_ID_APP_MTHL_SOCK_RECV_REQ:
			mthl_recv_req_hdlr(ilm_ptr);
			//MSG_ID_APP_MTHL_SOCK_RECV_CNF;
			//MSG_ID_APP_MTHL_SOCK_RECV_IND;
			break;
			
		

		//��ѯsocket��Ϣ,�յ�����local APP�����ݣ���ack���ݣ��Ѿ�����peer���ݡ�
		case MSG_ID_APP_MTHL_QUERY_SOCK_INFO_REQ:
			mthl_query_info_req_hdlr(ilm_ptr);
			//MSG_ID_APP_MTHL_QUERY_SOCK_INFO_CNF;
			break;

			
  /****************************************************����Ϊ�����Ĳ���**************************************************/		
  //����Ҫ��:create/activate/conn/�Ȳ�����send query req֮ǰ��
  
		//ͨ��������ȡIP ��
		case MSG_ID_APP_MTHL_GET_HOST_BY_NAME_REQ:
			mthl_get_host_by_name_req_hdlr(ilm_ptr);
			//MSG_ID_APP_MTHL_GET_HOST_BY_NAME_CNF;		
			//MSG_ID_APP_MTHL_GET_HOST_BY_NAME_IND remove;
			break;

		case MSG_ID_APP_SOC_GET_HOST_BY_NAME_IND:
			ASSERT(ilm_ptr->local_para_ptr != NULL);
            mthl_soc_get_host_by_name_ind_hdlr((app_soc_get_host_by_name_ind_struct*) ilm_ptr->local_para_ptr);
			break;

		//ͨ��IP��ȡ������
		case MSG_ID_APP_MTHL_GET_HOST_BY_ADDR_REQ:
			mthl_get_host_by_addr_req_hdlr(ilm_ptr);
			//MSG_ID_APP_MTHL_GET_HOST_BY_ADDR_CNF;
		    //MSG_ID_APP_MTHL_GET_HOST_BY_ADDR_IND remove;
			break;

		case MSG_ID_APP_SOC_GET_HOST_BY_ADDR_IND:
			mthl_soc_get_host_by_addr_ind_hdlr((app_soc_get_host_by_addr_ind_struct*) ilm_ptr->local_para_ptr);
			//app_soc_get_host_by_addr_ind_struct *ind_ptr =
            //    (app_soc_get_host_by_addr_ind_struct*) ilm_ptr->local_para_ptr;
			break;
		

		// TCP Server listen and auto accept ��		
		case MSG_ID_APP_MTHL_SERVER_LAUNCH_REQ:
			mthl_tcp_server_listen_and_accept_req_hdlr(ilm_ptr);	
			break;
			
		//MSG_ID_APP_MTHL_SERVER_LISTEN_CNF;
		//MSG_ID_APP_MTHL_SERVER_ACCEPT_IND;

//add for transparent transport mode begin
        //����͸����������������͸��������esc_string�����͸����
        /*************************************************************************************/
		case MSG_ID_APP_MTHL_TRANSPARENT_TRANS_REQ:
		{
			 mthl_transparent_trans_req_struct *p_ilm;
             p_ilm = (mthl_transparent_trans_req_struct*)(ilm_ptr->local_para_ptr);
			 mthl_transparent_trans_req_hdlr(p_ilm);
			//MSG_ID_APP_MTHL_TRANSPARENT_TRANS_CNF;		
		    //MSG_ID_APP_MTHL_TRANSPARENT_TRANS_END_IND;
			break;	
		}
		case MSG_ID_UART_ESCAPE_DETECTED_IND:
		{
			 uart_escape_detected_ind_struct *p_ilm;
             p_ilm = (uart_escape_detected_ind_struct*)(ilm_ptr->local_para_ptr);
			 mthl_transparent_uart_escape_detected_ind_hdlr(p_ilm);
			//MSG_ID_APP_MTHL_TRANSPARENT_TRANS_CNF;		
		    //;
			break;	
		}
		case MSG_ID_UART_READY_TO_READ_IND:
		{
                uart_ready_to_read_ind_struct *p_ilm;
                p_ilm = (uart_ready_to_read_ind_struct*)(ilm_ptr->local_para_ptr);
                //uart_port = p_ilm->port;

                #if 0
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
				#endif
				mthl_transparent_uart_ready_to_read_ind_hdlr();
				break;
			}
		case MSG_ID_UART_READY_TO_WRITE_IND:
			mthl_transparent_ready_to_write_uart_ind_hdlr();
			break;		
        /*************************************************************************************/
   
		case MSG_ID_UART_PLUGOUT_IND:
        {
            uart_plugout_ind_struct *p_local;
            p_local = (uart_plugout_ind_struct*)(ilm_ptr->local_para_ptr);
            mthl_transparent_uart_plugout_ind_hdlr(p_local);
        }
        break;
			
        /*************************************************************************************/
//add for transparent transport mode end
        default:
            switch(ilm_ptr->src_mod_id)
            {
                /* messages from TCPIP */
                case MOD_TCPIP:
                    //soc_tcpip_entry_func(ilm_ptr);
                    break;

                /* messages from ABM */
                case MOD_ABM:
                    //soc_abm_entry_func(ilm_ptr);
                    break;

            
                /* abnormal message */
                default:
                    //kal_lib_trace(TRACE_ERROR, SOC_WRONG_MSG, ilm_ptr->msg_id);
                    break;
            }
    }
        
}

#if 0
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif

kal_int8 mthl_cbm_release_bearer(kal_uint32 ori_acct_id)
{
#ifdef __NWK_APP_UT__
    kal_int8 ret_val;
    ABM_GLOBAL_LOCK;
    ret_val = cbm_ut_release_bearer(g_mthl_atci_app_id);
    ABM_GLOBAL_UNLOCK;
    return (ret_val);
#else /* !__NWK_APP_UT__ */
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    app_soc_deactivate_req_struct *ind_ptr;
    ilm_struct *ilm_ptr;
    abm_app_proc_struct *app_proc = NULL;
    kal_uint32 nwk_acct_id = 0;
    module_type mod_id;
    kal_uint8 i;
    cbm_bearer_enum bearer_type;
    mmi_abm_bearer_deinit_ind_struct *acct_deinit_ind;
    abm_app_proc_struct *notify_app_proc = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ABM_GLOBAL_LOCK;
    
    if (!abm_is_app_id_valid(g_mthl_atci_app_id))
    {
        ABM_GLOBAL_UNLOCK;
        return CBM_INVALID_AP_ID;
    }
    kal_lib_trace(TRACE_FUNC, CBM_REL_BEARER, g_mthl_atci_app_id);
    
    /* find application slot at first */
    app_proc = abm_find_app_proc(MOD_NIL, g_mthl_atci_app_id);
    if (app_proc == NULL)   /* No ap slot, AP is in deactivated state */
    {
        ABM_GLOBAL_UNLOCK;
        return CBM_OK;
    }
    /* clear non-auto attributes */
    app_proc->app_event &= ~(ABM_APP_OPT_NON_AUTO_DEACT);

    notify_app_proc = (abm_app_proc_struct*)get_ctrl_buffer(sizeof(abm_app_proc_struct));
    memcpy(notify_app_proc, app_proc, sizeof(abm_app_proc_struct));

    /* reset any state which is still under init or reinit */
    for (i = 0; i < ABM_MAX_APP_ACCT_NUM; i++)
    {
        if (app_proc->account_state[i] == ABM_APP_INIT ||
            app_proc->account_state[i] == ABM_APP_REINIT)
        {
            acct_deinit_ind = construct_local_para(sizeof(mmi_abm_bearer_deinit_ind_struct), TD_RESET);
            acct_deinit_ind->account_id = app_proc->ori_account_id[i];
            abm_send_msg2extmod(
                MSG_ID_MMI_ABM_BEARER_DEINIT_IND,
                MOD_MMI,
                ABM_MMI_SAP,
                (local_para_struct*)acct_deinit_ind);

            if (app_proc->account_state[i] == ABM_APP_REINIT)
            {
                bearer_type = CBM_BEARER_WIFI;
            }
            else
            {
                bearer_type = CBM_BEARER_NONE;
            }

   

            if (app_proc->app_state[i] & ABM_STATE_SOC_ACTIVATE)
            {
                abm_send_rej2soc(
                    MSG_ID_SOC_ABM_ACTIVATE_CNF,
                    app_proc->app.activate_mod_id,
                    bearer_type,
                    ABM_E_REJECTED,
                    0,
                    app_proc->ori_account_id[i],
                    ABM_DEFAULT_QOS,
                    ABM_NULL_BEARER,
                    KAL_FALSE);
            }
            else
            {
                abm_send_rej2soc(
                    MSG_ID_SOC_ABM_DEACTIVATE_IND,
                    app_proc->app.activate_mod_id,
                    bearer_type,
                    ABM_E_REJECTED,
                    0,
                    app_proc->ori_account_id[i],
                    ABM_DEFAULT_QOS,
                    ABM_NULL_BEARER,
                    KAL_FALSE);
            }
            /* reset */
            abm_reset_app_slot(app_proc, i);
        }
    }

    /* reset all fallback options except the always-ask */
    if (!abm_is_app_bearer_running(app_proc))
    {
        kal_uint8 i;
        for (i = 0; i < ABM_MAX_APP_ACCT_NUM; i++)
        {
            abm_reset_app_slot(app_proc, i);
        }

        free_ctrl_buffer(notify_app_proc);

        ABM_GLOBAL_UNLOCK;
        return CBM_OK;
    }

    for (i = 0; i < ABM_MAX_APP_ACCT_NUM; i++)
    {
        if (notify_app_proc->account_state[i] == ABM_APP_INIT ||
            notify_app_proc->account_state[i] == ABM_APP_REINIT)
        {
            if (notify_app_proc->account_state[i] == ABM_APP_REINIT)
            {
                bearer_type = CBM_BEARER_WIFI;
            }
            else
            {
                bearer_type = CBM_BEARER_NONE;
            }

            abm_send_bearer_info_ind2app(
                &notify_app_proc->app,
                notify_app_proc->ori_account_id[i],
                notify_app_proc->account_id[i],
                ABM_DEFAULT_QOS,
                CBM_DEACTIVATED,
                bearer_type,
                NULL,
                0,
                CBM_BEARER_FAIL,
                NULL);
        }
    }

    free_ctrl_buffer(notify_app_proc);

    ABM_GLOBAL_UNLOCK;
    
    mod_id = stack_get_active_module_id();
    nwk_acct_id = cbm_encode_data_account_id(ori_acct_id, CBM_SIM_ID_SIM1, g_mthl_atci_app_id, KAL_FALSE);

    ind_ptr = (app_soc_deactivate_req_struct *) 
              construct_local_para(sizeof(app_soc_deactivate_req_struct), TD_CTRL | TD_RESET);
    ind_ptr->account_id = nwk_acct_id;
    ind_ptr->qos_profile_id = CBM_ALL_QOS_ID;
   
    ilm_ptr = allocate_ilm(mod_id);
    ilm_ptr->msg_id = (msg_type) MSG_ID_APP_SOC_DEACTIVATE_REQ;
    ilm_ptr->local_para_ptr = (local_para_struct*)ind_ptr;
    ilm_ptr->peer_buff_ptr = NULL;
    SEND_ILM(mod_id, MOD_SOC, SOC_APP_SAP, ilm_ptr);

    return CBM_WOULDBLOCK; 
#endif /* __NWK_APP_UT__ */
}

void mthl_soc_close_recv_close_ind(kal_int8 socket_id)
{
	int i=0;
	int j = 0;
	kal_int8 error;	
	mthl_close_sock_ind_struct mthl_close_ind;
	

	error = soc_close(socket_id); 
	 if(error == SOC_INVALID_SOCKET)
		kal_prompt_trace(MOD_MTHL, "mthl_soc_notify(): SOC_CLOSE evt, soc_close ret = SOC_INVALID_SOCKET.");
	 else if(error == SOC_SUCCESS)
	 {
		kal_prompt_trace(MOD_MTHL, "mthl_soc_notify(): SOC_CLOSE evt, soc_close ret = SOC_SUCCESS.");
		//ͨ��sock_idƥ�䣬��close��socket id������statistic�Ĳ��֣������Ϊinit value;
		for(i=0; i<MTHL_GPRS_MAX_PDP_NUM; i++)
		{
			for(j=0; j<MTHL_MAX_IP_SOCKET_NUM; j++)
			{
			   if(g_mthl_create_conn_soc_save[i].socket_info[j].sock_id== socket_id)
			   {
				 kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr() match sock_id.");
				 
				 kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear ,g_mthl_create_conn_soc_save[%d].socket_info[%d].sock_id = %d", i,j,g_mthl_create_conn_soc_save[i].socket_info[j].sock_id);
				 g_mthl_create_conn_soc_save[i].socket_info[j].sock_id = SOC_ERROR;

				 kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear ,g_mthl_create_conn_soc_save[%d].socket_statistics_info[%d].sock_id = %d", i,j,g_mthl_create_conn_soc_save[i].socket_statistics_info[j].sock_id);
				 g_mthl_create_conn_soc_save[i].socket_statistics_info[j].sock_id = SOC_ERROR;
				 g_mthl_create_conn_soc_save[i].sock_num--;
				 
				 kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear init value, SOC_ERROR, done.");
				 break;
			   }
			   else
				 continue;
			}
			if(j<MTHL_MAX_IP_SOCKET_NUM)	
			  break;
			else
			  continue;
		}	

		//���ʧ�ܣ�
		if(i == MTHL_GPRS_MAX_PDP_NUM)
		{
			kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear init value, fail, not match sock_id..");	
			kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): ind_ptr->sock_id = %d.", socket_id);
		}
		//����ɹ���
		else
		{
			kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): clear init value, success.");   
			kal_prompt_trace(MOD_MTHL, "mthl_close_req_hdlr(): ind_ptr->sock_id = %d.", socket_id);

			//mthl_close_cnf.result    = KAL_TRUE;
			//mthl_close_cnf.sock_id   = ind_ptr->socket_id;
			//mthl_close_cnf.user_data = g_mthl_create_conn_soc_save[i].user_data;
			mthl_close_ind.result	 = KAL_TRUE;
			mthl_close_ind.sock_id	 = socket_id;
			mthl_close_ind.user_data = g_mthl_create_conn_soc_save[i].user_data;
			
			//����success cnf;
			mthl_close_sock_ind(&mthl_close_ind);
		}
	 }					
	 else
		kal_prompt_trace(MOD_MTHL, "send data fail: soc_close ret = %d.",error);

}
/*****************************************************************************
 * FUNCTION
 *  imps_soc_notify
 * DESCRIPTION
 *  This function is used to handle the notification for socket events.
 * PARAMETERS
 *  inMsg 
 * RETURNS
 *  void
 *****************************************************************************/
void mthl_soc_notify(void *inMsg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    app_soc_notify_ind_struct *ind_ptr;
    //kal_int8 socketid ;
	mthl_create_conn_cnf_struct    mthl_create_conn_cnf;
	mthl_send_data_ind_struct mthl_send_data_ind;
	mthl_recv_data_ind_struct mthl_recv_data_ind;
	mthl_server_accept_ind_struct    mthl_server_accept_ind;
	//mthl_close_sock_cnf_struct    mthl_close_cnf;
	mthl_close_sock_ind_struct mthl_close_ind;
	
	int i = 0;
	int j = 0;
	kal_int8 error;
	kal_int8 conn_sock_id = SOC_ERROR;
	sockaddr_struct client_addr = {0};
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(inMsg != NULL);

	memset(&mthl_create_conn_cnf, 0x00, sizeof(mthl_create_conn_cnf_struct));
	memset(&mthl_send_data_ind, 0x00, sizeof(mthl_send_data_ind_struct));
	memset(&mthl_recv_data_ind, 0x00, sizeof(mthl_recv_data_ind_struct));
	memset(&mthl_server_accept_ind, 0x00, sizeof(mthl_server_accept_ind_struct));

    
    ind_ptr = (app_soc_notify_ind_struct*)inMsg;

    //kal_prompt_trace(MOD_IMPS, "[IMPS] imps_soc_notify(%d, %d, %d)", socketid, ind_ptr->event_type, est_state);
    
	if((mthl_get_transparent_mode_status()== KAL_FALSE)||(ind_ptr->socket_id != mthl_transparent_mode_context.sock_id))
		{
    
    switch (ind_ptr->event_type)
    {
        case SOC_READ:
        {
			//MTHL����cover���ղ��֣�ֻ�ñ�IND������User�Լ���������գ�
            /*kal_int32 size=0;             
            kal_uint8* cir_data = get_ctrl_buffer(256);		
            memset((void*)cir_data,0,256);            
            size=soc_recv(socketid,cir_data,256,0);
            free_ctrl_buffer(cir_data);   */    
            mthl_recv_data_ind.result = ind_ptr->result;
			//mthl_send_data_ind.ret_val = 0;
			mthl_recv_data_ind.sock_id = ind_ptr->socket_id;
			mthl_recv_data_ind.user_data = 0;

			//����cnf;
			mthl_recv_sock_ind(&mthl_recv_data_ind);
            break ;
        }
      
        case SOC_WRITE:
			//MTHL����cover�ط����֣�ֻ�ñ�IND������User�Լ�������ط���
            //error=soc_send(imps_context_p->imps_cir_channel.cir_socket,(kal_uint8*) hello,len,0);
            
            mthl_send_data_ind.result = ind_ptr->result;
			//mthl_send_data_ind.ret_val = 0;
			mthl_send_data_ind.sock_id = ind_ptr->socket_id;
			mthl_send_data_ind.user_data = 0;

			//����cnf;
			mthl_send_sock_ind(&mthl_send_data_ind);
            break ;
         
        case SOC_CLOSE:    
			//����bearer fail ind/����Ҫ�رն�Ӧ��socket���ͷ�sock res;
            mthl_soc_close_recv_close_ind(ind_ptr->socket_id);
            break;
         
        case SOC_CONNECT:
                //ͨ��sock_id������ƥ�䵽account_id;
                for(i=0; i<MTHL_GPRS_MAX_PDP_NUM; i++)
                {
                    for(j=0; j<MTHL_MAX_IP_SOCKET_NUM; j++)
                    {
                       if(g_mthl_create_conn_soc_save[i].socket_info[j].sock_id== ind_ptr->socket_id)
					   	 break;
					   else
					   	 continue;
                    }
					if(j<MTHL_MAX_IP_SOCKET_NUM)	
					  break;
					else
					  continue;
                }	
				
			if (ind_ptr->result == KAL_FALSE)
            {
                //����첽IND fail����: clear ֮ǰsave��sock_id  info;
                g_mthl_create_conn_soc_save[i].socket_info[j].sock_id = SOC_ERROR;
				g_mthl_create_conn_soc_save[i].socket_statistics_info[j].sock_id = SOC_ERROR;
				memset(&(g_mthl_create_conn_soc_save[i].socket_info[j].addr), 0x00, sizeof(sockaddr_struct));
                mthl_create_conn_cnf.account_id = g_mthl_create_conn_soc_save[i].account_id;
				mthl_create_conn_cnf.result     = KAL_FALSE;
				mthl_create_conn_cnf.sock_id    = SOC_ERROR;
				mthl_create_conn_cnf.user_data  = g_mthl_create_conn_soc_save[i].user_data;
				
				//�첽����fail cnf;
				mthl_send_create_conn_cnf(&mthl_create_conn_cnf); 

				//error = soc_connect(...);����������feature;
				//close socket;
				soc_close(ind_ptr->socket_id);
				g_mthl_create_conn_soc_save[i].sock_num--;
            } 
            else
            {
                
				
                mthl_create_conn_cnf.account_id = g_mthl_create_conn_soc_save[i].account_id; 
				mthl_create_conn_cnf.result     = KAL_TRUE;
				mthl_create_conn_cnf.sock_id    = ind_ptr->socket_id;
				mthl_create_conn_cnf.user_data  = g_mthl_create_conn_soc_save[i].user_data;
				
				//�첽����success cnf;
				mthl_send_create_conn_cnf(&mthl_create_conn_cnf); 
            }
            break;

		 case SOC_ACCEPT:
		 	//MTHL����cover����accept���֣�ֻ�ñ�IND������User�Լ����������accept��
		 	if(ind_ptr->result == KAL_FALSE)
		 	{
		 	    mthl_server_accept_ind.conn_sock_id   = SOC_ERROR;
				//mthl_server_accept_ind.addrΪ��ʼֵ0��
				
				//����cnf;
				mthl_tcp_server_accept_ind(&mthl_server_accept_ind);
		 	}
			else
			{
			   conn_sock_id = soc_accept(ind_ptr->socket_id, &client_addr);
			   mthl_server_accept_ind.conn_sock_id = conn_sock_id;
			    //mthl_server_accept_ind.addrΪ��ʼֵ0��
			        //��ƥ��account_id;
		            for(i =0 ;i< MTHL_GPRS_MAX_PDP_NUM;i++)
					{
					    if(g_server_info.server_account_id == g_mthl_create_pdp_param_save[i].account_id)
					    {
					        //ori_acct_idƥ��ɹ�;
							break;
					    }
						else
							continue;
					}

		            //ori_acct_idƥ��ʧ�ܣ�
					if(i == MTHL_GPRS_MAX_PDP_NUM)
					{
					   mthl_server_accept_ind.conn_sock_id = SOC_ERROR; 
					   //memcpy(&(mthl_server_accept_ind.addr), &conn_addr, sizeof(sockaddr_struct));//���ֳ�ֵ0���ɣ�
							
					   kal_prompt_trace(MOD_MTHL, "soc_accept(): IND success ,but match account_id fail case.");

					   //IND֮�󣬲�����0��server_account_id�ǹ̶��ģ�
                       //g_server_info.server_account_id = CBM_INVALID_NWK_ACCT_ID;
						//����cnf;
			  			mthl_tcp_server_accept_ind(&mthl_server_accept_ind);
					
					    //close socket;
						soc_close(conn_sock_id);
						return;
					}	
			   	    
			    //�ҵ�һ�����õ�socket_info[j]���ٱ�����Ϣ��
					for(j=0;j<MTHL_MAX_IP_SOCKET_NUM;j++)
					{
					   if(g_mthl_create_conn_soc_save[i].socket_info[j].sock_id == SOC_ERROR)
					   {
					        kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): find free sock_info, param:");
							
							g_mthl_create_conn_soc_save[i].socket_info[j].sock_id      = mthl_server_accept_ind.conn_sock_id;
							kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): g_mthl_create_conn_soc_save[%d].socket_info[%d].sock_id = %d", i,j,g_mthl_create_conn_soc_save[i].socket_info[j].sock_id);
							
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.sock_type  = SOC_SOCK_STREAM;
							kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): TCP case.");

				            //TCP ���ñ���req��������Ҫ���ӵ�Server��ַ��
							/*g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[0] = mthl_create_conn_req->addr.addr[0];
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[1] = mthl_create_conn_req->addr.addr[1];
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[2] = mthl_create_conn_req->addr.addr[2];
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.addr[3] = mthl_create_conn_req->addr.addr[3];
							g_mthl_create_conn_soc_save[i].socket_info[j].addr.port    = mthl_create_conn_req->addr.port;*/
							
							//������Ϣok���ټ�����
							g_mthl_create_conn_soc_save[i].sock_num++;
							kal_prompt_trace(MOD_MTHL, "mthl_create_and_conn_req_hdlr(): g_mthl_create_conn_soc_save[%d].sock_num = %d",i,g_mthl_create_conn_soc_save[i].sock_num);

							//Ϊ����ͳ���ٱ���һ�ݣ�
							g_mthl_create_conn_soc_save[i].socket_statistics_info[j].sock_id = mthl_server_accept_ind.conn_sock_id;
							break;
					   }
					   else
					   		continue;
					}
                    //�������ӳɹ�������ʧ�ܣ�
					if(j == MTHL_MAX_IP_SOCKET_NUM)
					{
							mthl_server_accept_ind.conn_sock_id = SOC_LIMIT_RESOURCE; 
							//memcpy(&(mthl_server_accept_ind.addr), &conn_addr, sizeof(sockaddr_struct));//���ֳ�ֵ0���ɣ�
							
							kal_prompt_trace(MOD_MTHL, "soc_accept(): success ,but save conn_sock_id fail case.");

							//IND֮�󣬲�����0��server_account_id�ǹ̶��ģ�
                       		//g_server_info.server_account_id = CBM_INVALID_NWK_ACCT_ID;
							
							 //����cnf;
			  				mthl_tcp_server_accept_ind(&mthl_server_accept_ind);
							//close socket;
						    soc_close(conn_sock_id);
							break;
					}
					//�������ӳɹ�������ɹ���
					else
					{
					        memcpy(&(mthl_server_accept_ind.addr), &client_addr, sizeof(sockaddr_struct));
							kal_prompt_trace(MOD_MTHL, "soc_accept(): success conn_sock_id case.");

							//IND֮�󣬲�����0��server_account_id�ǹ̶��ģ�
                       		//g_server_info.server_account_id = CBM_INVALID_NWK_ACCT_ID;
							
						    //����cnf;
			  				mthl_tcp_server_accept_ind(&mthl_server_accept_ind);
							break;
					}
				
				//����cnf;
				//mthl_tcp_server_accept_ind(&mthl_server_accept_ind);
			}
			break;
            
        default:
			break;
			
    } 

		}
	else
	{
		switch (ind_ptr->event_type)
		{
				case SOC_READ:
				{
					mthl_transparent_reveive_data();
					break ;
				}
			  
				case SOC_WRITE:
					mthl_transparent_send_data();
					break ;
				 
				case SOC_CLOSE:    
				{
					//����bearer fail ind/����Ҫ�رն�Ӧ��socket���ͷ�sock res;
					//error = soc_close(ind_ptr->socket_id); 
					mthl_transparent_mem_dealloc();
					mthl_transparent_send_end_ind(mthl_transparent_mode_context.port,TRANSPARENT_NETWORK_ERROR);
					mthl_soc_close_recv_close_ind(ind_ptr->socket_id);
					break;					
				}
				default:
					return; 
			}

		}
	return; 
}

void mthl_soc_get_host_by_name_ind_hdlr(app_soc_get_host_by_name_ind_struct *p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_get_host_by_name_cnf_struct    mthl_get_host_by_name_cnf;
 
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	memset(&mthl_get_host_by_name_cnf, 0x00, sizeof(mthl_get_host_by_name_cnf_struct));
	
	//�첽���ؽ��ʧ�ܣ�
    if (p->result == KAL_FALSE)
    {
		mthl_get_host_by_name_cnf.result = KAL_FALSE;
        mthl_get_host_by_name_cnf.domain_len = g_get_host_by_name_save.domain_len;
		strncpy(mthl_get_host_by_name_cnf.domain_name, g_get_host_by_name_save.domain_name, mthl_get_host_by_name_cnf.domain_len);
		
		mthl_get_host_by_name_cnf.user_data = g_get_host_by_name_save.user_data;
				
		//����cnf;
		mthl_get_host_by_name_soc_cnf(&mthl_get_host_by_name_cnf);
		kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_name_req_hdlr(): DNS query fail!");

		//�첽���ز�����ɣ����save;
		memset(&g_get_host_by_name_save, 0x00, sizeof(mthl_get_host_by_name_save_struct));
		
        return;
    }

        //�첽���ؽ��success; 
        mthl_get_host_by_name_cnf.result = KAL_TRUE;
        mthl_get_host_by_name_cnf.domain_len = g_get_host_by_name_save.domain_len;
		strncpy(mthl_get_host_by_name_cnf.domain_name, g_get_host_by_name_save.domain_name, mthl_get_host_by_name_cnf.domain_len);

		mthl_get_host_by_name_cnf.addr_len = p->addr_len;
		strncpy(mthl_get_host_by_name_cnf.addr, p->addr, mthl_get_host_by_name_cnf.addr_len);
		kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_name_req_hdlr(): DNS query from server, Success: IP addr:");
		kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_name_req_hdlr(): mthl_get_host_by_name_cnf.addr = %d.%d.%d.%d", 
			             mthl_get_host_by_name_cnf.addr[0], mthl_get_host_by_name_cnf.addr[1],mthl_get_host_by_name_cnf.addr[2],mthl_get_host_by_name_cnf.addr[3]);
			
		mthl_get_host_by_name_cnf.user_data = g_get_host_by_name_save.user_data;
				
		//����cnf;
		mthl_get_host_by_name_soc_cnf(&mthl_get_host_by_name_cnf);

		//�첽���ز�����ɣ����save;
		memset(&g_get_host_by_name_save, 0x00, sizeof(mthl_get_host_by_name_save_struct));
		
        return;
    
}

void mthl_soc_get_host_by_addr_ind_hdlr(app_soc_get_host_by_addr_ind_struct* get_host_by_addr_ind)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mthl_get_host_by_addr_cnf_struct    mthl_get_host_by_addr_cnf;
 
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	memset(&mthl_get_host_by_addr_cnf, 0x00, sizeof(mthl_get_host_by_addr_cnf_struct));
	
	//�첽���ؽ��ʧ�ܣ�
    if (get_host_by_addr_ind->result == KAL_FALSE)
    {
		mthl_get_host_by_addr_cnf.result = KAL_FALSE;
        mthl_get_host_by_addr_cnf.addr_len = g_get_host_by_addr_save.addr_len;
		memcpy(mthl_get_host_by_addr_cnf.addr, g_get_host_by_addr_save.addr, g_get_host_by_addr_save.addr_len);
		
		mthl_get_host_by_addr_cnf.user_data = g_get_host_by_addr_save.user_data;
				
		//����cnf;
		mthl_get_host_by_addr_soc_cnf(&mthl_get_host_by_addr_cnf);
        kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_addr_req_hdlr(): IP Addr query fail.");

		//�첽���ز�����ɣ����save;
		memset(&g_get_host_by_addr_save, 0x00, sizeof(mthl_get_host_by_addr_save_struct));
		
        return;
    }

        //�첽���ؽ��success; 
        mthl_get_host_by_addr_cnf.result = KAL_TRUE;
		
       mthl_get_host_by_addr_cnf.addr_len = g_get_host_by_addr_save.addr_len;
		memcpy(mthl_get_host_by_addr_cnf.addr, g_get_host_by_addr_save.addr, g_get_host_by_addr_save.addr_len);

		mthl_get_host_by_addr_cnf.domain_len= strlen(get_host_by_addr_ind->name);
		strncpy(mthl_get_host_by_addr_cnf.domain_name, get_host_by_addr_ind->name, mthl_get_host_by_addr_cnf.domain_len);
		kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_addr_req_hdlr(): IP Addr query from server, Success: domain_name:");
		kal_prompt_trace(MOD_MTHL, "mthl_get_host_by_addr_req_hdlr(): domain_name = %s", mthl_get_host_by_addr_cnf.domain_name);
			
		mthl_get_host_by_addr_cnf.user_data = g_get_host_by_addr_save.user_data;
				
		//����cnf;
		mthl_get_host_by_addr_soc_cnf(&mthl_get_host_by_addr_cnf);

		//�첽���ز�����ɣ����save;
		memset(&g_get_host_by_addr_save, 0x00, sizeof(mthl_get_host_by_addr_save_struct));
		
        return;  
}


#endif/*M2M_MTHL_TASK_HANDLE*/

