
#include "Custom_config_account.h"
#include "Soc_consts.h"
#include "Soc_api.h"
#include "cbm_consts.h"

#define MTHL_MAX_IP_SOCKET_NUM      (MAX_IP_SOCKET_NUM -1) //����Ԥ��һ����gethostbyname();
#if (GPRS_MAX_PDP_SUPPORT > 3)
#define MTHL_GPRS_MAX_PDP_NUM        3
#else
#define MTHL_GPRS_MAX_PDP_NUM        GPRS_MAX_PDP_SUPPORT
#endif
#define MAX_SOCKET_DATA_SIZE   512 
#define MAX_STRING_DATA_SIZE   (MAX_SOCKET_DATA_SIZE*2 + 1) 


typedef enum
{
    PDP_NONE,
    PDP_ACTIVATE_REQ,
    PDP_ACTIVATE_CNF,
    PDP_DEACTIVATE_IND,
    PDP_DEACTIVATE_REQ,
    PDP_DEACTIVATE_CNF,
}pdp_bear_op_enum;

typedef enum
{
   TRANSPARENT_ESC_DETECT,
   TRANSPARENT_PLUGOUT_DETECT,
   TRANSPARENT_NETWORK_ERROR,
   TRANSPARENT_ERROR_CAUSE_ENUM_SIZE
} transparent_error_cause_enum;


typedef enum
{
    TRA_SUCCESS           = 0,     /* success */
    TRA_INVALID_SOCKET    = -1,    /* invalid socket */
    TRA_INVALID_PARAM     = -2,    /* invalid para */
    TRA_GET_PORT_FAIL     = -3,    /* get uart port fail */
} transparent_rsp_error_enum;



typedef struct
{
    LOCAL_PARA_HDR
    kal_uint32              account_id;
    pdp_addr_type_enum      pdp_type;        /*pdp_type_enum*/
    kal_uint8               apn_length;
    kal_uint8               apn [ MAX_APN_LEN ];
    kal_uint16              proxy_port;       
    kal_uint8               addr_length;
    kal_uint8               proxy_addr[ MAX_PDP_ADDR_LEN ];
    kal_uint32              user_data;
}mthl_create_pdp_param_req_struct;

typedef enum
{
    SUCCESS,
    PARAM_INVALID,
	LIMITED_RES,	
}mthl_create_pdp_param_result;

typedef struct
{
    LOCAL_PARA_HDR
    //kal_bool       result;
    mthl_create_pdp_param_result    result;
    kal_uint32     account_id;//ori_acct_id
    kal_uint32     user_data;
}mthl_create_pdp_param_cnf_struct;
typedef struct
{
    LOCAL_PARA_HDR
    kal_uint32              user_data;
    
}mthl_query_pdp_param_req_struct;

typedef struct
{
    LOCAL_PARA_HDR
    kal_uint32              account_id;
    kal_uint8               pdp_type;        
    kal_uint8               apn_length;
    kal_uint8               apn [ MAX_APN_LEN ];
    kal_uint16              proxy_port;
    kal_uint8               proxy_len;   
    kal_uint8               proxy_addr[ MAX_PDP_ADDR_LEN ];
}mthl_pdp_param_info_struct;


typedef struct
{
    LOCAL_PARA_HDR
    kal_uint8 result;
    kal_uint8 pdp_param_num;
    mthl_pdp_param_info_struct pdp_param_info[3];

    kal_uint32              user_data;
}mthl_query_pdp_param_cnf_struct;

typedef struct
{
    LOCAL_PARA_HDR
    kal_uint32              user_data;
    
}mthl_query_pdp_state_req_struct;


typedef struct
{
    LOCAL_PARA_HDR
    kal_uint8   result;
    kal_uint8   num;
    kal_uint8   state_list[3][2];
    kal_uint32  user_data;
}mthl_query_pdp_state_cnf_struct;


typedef struct
{
    LOCAL_PARA_HDR
    kal_uint32     account_id;
    custom_dtcnt_prof_auth_struct Auth_info;       /* authentication info */
    kal_uint32     user_data;
} mthl_activate_pdp_req_struct;


typedef struct
{
    LOCAL_PARA_HDR
    kal_bool       result;
    kal_uint32     error_cause;
    kal_uint32     account_id;
    kal_uint32     user_data;
} mthl_activate_pdp_cnf_struct;


typedef struct
{
    LOCAL_PARA_HDR
    kal_uint32     account_id;
    kal_uint32     user_data;
} mthl_deactivate_pdp_req_struct;


typedef struct
{
  LOCAL_PARA_HDR 
  kal_uint32     account_id;
  kal_bool       result;
  kal_uint32     error_cause;
  kal_uint32     user_data;
} mthl_deactivate_pdp_cnf_struct;


typedef struct
{ 
    LOCAL_PARA_HDR
    kal_uint32     account_id;
    cbm_result_error_enum error;
    kal_uint32 error_cause;
} mthl_deactivate_pdp_ind_struct;


typedef struct
{
   LOCAL_PARA_HDR
   kal_uint32        account_id;

   sockaddr_struct   addr; //include socket_type
   kal_uint32        user_data;
} mthl_create_conn_req_struct;


typedef struct
{
   LOCAL_PARA_HDR 
   kal_uint32 account_id;
   kal_bool result;
   kal_int8 sock_id;
   kal_uint32    user_data;
} mthl_create_conn_cnf_struct;


typedef struct
{
  LOCAL_PARA_HDR
  kal_uint32 account_id;
  
  kal_uint32    user_data;
} mthl_query_sock_account_info_req_struct;

typedef struct
{
   LOCAL_PARA_HDR
   kal_int8 sock_id;
   sockaddr_struct   addr;
}sock_account_info_struct;

typedef struct
{
  LOCAL_PARA_HDR
  kal_bool result;//��ѯ�Ƿ�ɹ���
  kal_uint32 account_id;
  kal_uint8 sock_num;
  sock_account_info_struct socket_info[MTHL_MAX_IP_SOCKET_NUM]; //type, id, ip, port
  kal_uint32    user_data;
} mthl_query_sock_account_info_cnf_struct;


typedef struct
{ 
  LOCAL_PARA_HDR
  kal_uint8 sock_id;
  kal_uint32    user_data;
} mthl_close_sock_req_struct;


typedef struct
{ 
  LOCAL_PARA_HDR
  kal_bool result;//�ر��Ƿ�ɹ���
  kal_uint8 sock_id;//�رյ�Sockets ID��
  kal_uint32    user_data;
} mthl_close_sock_cnf_struct;

typedef struct
{ 
  LOCAL_PARA_HDR
  kal_bool result;
  kal_uint8 sock_id;
  kal_uint32    user_data;
} mthl_close_sock_ind_struct;

typedef struct
{
  LOCAL_PARA_HDR
  kal_uint8  sock_id;
  kal_uint8 * data;//Ҫ���͵�����;
  kal_int32 size;//Ҫ���͵�����Size;
  kal_uint8 flags;//���ͱ�־λ��
  sockaddr_struct addr;//���ն˵�ַ��Ի���UDP�ģ�

  kal_uint32    user_data;
} mthl_send_data_req_struct;


typedef struct
{
  LOCAL_PARA_HDR
  kal_uint8 sock_id;
  kal_bool result;//�Ƿ�ɹ���
  kal_int32 ret_val;//ʵ���͵�TCP����Buffer�е�data size
  kal_uint32    user_data;
} mthl_send_data_cnf_struct, mthl_send_data_ind_struct;




typedef struct
{ 
  LOCAL_PARA_HDR
  kal_uint8 sock_id;
//buff;//Ҫ�������ݵ�Buff;
//len;//Ҫ�������ݵ�Buff Size;
  kal_uint8 flags;//���ձ�־λ��

  kal_uint32    user_data;
} mthl_recv_data_req_struct;


typedef struct
{ 
  LOCAL_PARA_HDR
  kal_uint8 sock_id;
  kal_bool result;//�Ƿ�ɹ���
  kal_uint8 * buff;//Ҫ�������ݵ�Buff;
  kal_int32 ret_val;//ʵ�ʽ��յ���data size��
  sockaddr_struct addr;//��ȡ���ķ��Ͷ˵�ַ��Ի���UDP�ģ�
  kal_uint32    user_data;
} mthl_recv_data_cnf_struct, mthl_recv_data_ind_struct;



//����ͳ��
typedef struct
{ 
   LOCAL_PARA_HDR
   kal_int8 sock_id;

   kal_uint32    user_data;
} mthl_query_sock_info_req_struct;


typedef struct
{ 
   LOCAL_PARA_HDR
   kal_int8 sock_id;
   kal_bool result;//�Ƿ�ɹ���
   kal_uint64 total_send_size;//��ǰ�ܼƷ��͵���data size��
   kal_uint64 Ack_value;//��ǰTCP header��ACKֵ��
   kal_uint64 total_recv_size;//��ǰ�ܼƽ��յ���data size��
   kal_uint32    user_data;
} mthl_query_sock_info_cnf_struct;


typedef struct
{ 
   LOCAL_PARA_HDR
   kal_uint8 domain_len;
   kal_char domain_name[128+1];
   kal_uint32    user_data;
} mthl_get_host_by_name_req_struct;


typedef struct
{ 
    LOCAL_PARA_HDR
    kal_uint16    addr_len; /* address length */
    kal_uint8    addr[MAX_SOCK_ADDR_LEN];

    kal_uint32   user_data;
} mthl_get_host_by_addr_req_struct;


typedef struct
{  
   LOCAL_PARA_HDR
   kal_bool     result;//�Ƿ�ɹ���(-2)
   kal_uint8    domain_len;
   kal_char     domain_name[128+1];//
   kal_int16	addr_len; /* address length */
   kal_uint8	addr[MAX_SOCK_ADDR_LEN];
   kal_uint32   user_data;
   
} mthl_get_host_by_name_cnf_struct, mthl_get_host_by_addr_cnf_struct,mthl_get_host_by_name_ind_struct, mthl_get_host_by_addr_ind_struct;











typedef struct
{ 
    LOCAL_PARA_HDR
    kal_uint8 port;
    kal_int8 sock_id;
	sockaddr_struct addr;//���ն˵�ַ��Ի���UDP�ģ�

	kal_uint32   user_data;
} mthl_transparent_trans_req_struct;


typedef struct
{ 
    LOCAL_PARA_HDR
    kal_int8 result;//�Ƿ�ɹ�ռ��UART�������Ƿ�OK���Ƿ���Խ���͸�����䣻
	kal_int8 sock_id;
	kal_uint8  port; //add 
	kal_uint32   user_data;
} mthl_transparent_trans_cnf_struct;


typedef struct  //����͸�����䣻
{ 
    LOCAL_PARA_HDR
    transparent_error_cause_enum cause;
    kal_int8 sock_id;
    kal_int8 port;//modify 

} mthl_transparent_trans_end_ind_struct;


//TCP only
typedef struct
{ 
    LOCAL_PARA_HDR
    kal_uint32  account_id;
    
	kal_uint8	addr_len; /* address length */
    kal_uint8	addr[MAX_SOCK_ADDR_LEN];
	kal_uint16  port;//�󶨵�Server��port��

	kal_uint32   user_data;
} mthl_server_launch_req_struct;


typedef struct
{
    LOCAL_PARA_HDR
    kal_uint8 result;//֪ͨ�Ƿ�󶨣��������ɹ���
	kal_int8 sock_id;
    kal_uint32   user_data;
} mthl_server_listen_cnf_struct;


typedef struct
{
    LOCAL_PARA_HDR
    kal_uint8 conn_sock_id;//�����׽���ID
	sockaddr_struct addr;//��ȡ���ķ������������Client�˵�ַ��IP��Port����
} mthl_server_accept_ind_struct;

typedef struct 
{    
   
    kal_uint16 size;
    kal_uint16 used;
    
    kal_uint8 *data_ptr;
    kal_uint8 *cur_ptr;
    kal_uint8 *end_ptr;    
} mthl_transport_buff_head_struct;


typedef struct  
{
    //kal_uint8 phase;
    kal_bool is_read_suspend;
    kal_bool is_write_suspend;
    /*
     * FLC locks
     */
    kal_bool GPRS_FLC_UL_LOCKED;                /* The FLC UL buffer is not available)*/
    kal_bool GPRS_FLC_DL_LOCKED;                /* While uart writed len < data len */
   // kal_bool GPRS_IN_ESCAPE_STATE;

	kal_int8 sock_id;
	socket_type_enum sock_type;
	sockaddr_struct addr;//���ն˵�ַ��Ի���UDP�ģ�
   
    kal_uint8  port;                            /* Keep for L4C UART port used */
   
    /* 
     * UL Datapath parameters
     */
    //mthl_transport_buff_head_struct inbuff_context;
    mthl_transport_buff_head_struct * ibuff_head_ptr;
    //peer_buff_struct * gprs_flc_ul_buff_ptr;     /* If NULL, FLC_UL_LOCKED */   
   
    /* 
     * DL Datapath parameters
     * if GPRS_FLC_DL_LOCKED we should enqueue the FLC peer buff from TCM
     */
    //mthl_transport_buff_head_struct outbuff_context;
    mthl_transport_buff_head_struct * outbuff_head_ptr;


} mthl_transparent_mod_struct;

#define MAX_CTRL_PKT_SIZE                    256

typedef struct
{
    LOCAL_PARA_HDR
    kal_uint32 user_data;
} mthl_wifi_activate_req_struct;


typedef struct
{
    LOCAL_PARA_HDR
    kal_bool result;
	kal_uint32 user_data;
} mthl_wifi_activate_cnf_struct;

typedef struct
{
    LOCAL_PARA_HDR
    kal_uint32 user_data;
} mthl_wifi_deactivate_req_struct;


typedef struct
{
    LOCAL_PARA_HDR
    kal_bool result;
	kal_uint32 user_data;
} mthl_wifi_deactivate_cnf_struct;

typedef struct
{ 
    LOCAL_PARA_HDR
    cbm_result_error_enum error; //wifi
    kal_uint32 error_cause;
} mthl_wifi_deactivate_ind_struct;


typedef struct
{
    LOCAL_PARA_HDR
    kal_uint32 user_data;
} mthl_wifi_get_state_req_struct;


typedef struct
{
    LOCAL_PARA_HDR
    kal_uint8 result;
    kal_uint8 state;
	kal_uint32 user_data;
} mthl_wifi_get_state_cnf_struct;






