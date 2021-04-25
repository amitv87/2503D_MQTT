#ifndef _TRACK_FUN_COMMON_H
#define _TRACK_FUN_COMMON_H

/****************************************************************************
*  Include Files                        ����ͷ�ļ�
*****************************************************************************/
#include "track_drv.h"

/*****************************************************************************
*  Define                               �궨��
******************************************************************************/

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
{                                                       \
    (n) = ( (unsigned int) (b)[(i)    ] << 24 )             \
        | ( (unsigned int) (b)[(i) + 1] << 16 )             \
        | ( (unsigned int) (b)[(i) + 2] <<  8 )             \
        | ( (unsigned int) (b)[(i) + 3]       );            \
}
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

/*
 * 16-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT16_BE
#define GET_UINT16_BE(n,b,i)                            \
{                                                       \
    (n) = ( (unsigned short) (b)[(i)    ] <<  8 )             \
        | ( (unsigned short) (b)[(i) + 1]       );            \
}
#endif

#ifndef PUT_UINT16_BE
#define PUT_UINT16_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 1] = (unsigned char) ( (n)       );       \
}
#endif

/*****************************************************************************
*  Typedef  Enum                        ö�ٶ���
*****************************************************************************/

/*****************************************************************************
*  Struct                               ���ݽṹ����
******************************************************************************/

/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
*  Functions Define                     ��������
******************************************************************************/


extern void track_fun_msg_send(kal_uint16 msg_id, void *req, module_type mod_src, module_type mod_dst);


/******************************************************************************/
/*   �ַ������ຯ��       */
/******************************************************************************/

/******************************************************************************
 *  Function    -  track_fun_str_analyse
 *
 *  Purpose     -  ���ַ������ض������ַ����в���޸�
 *
 *  Description -  str_data    ��Ҫ���в�ֵ��ַ���(ע�⾭�������Դ�ַ��������ı�)
 *                 tar_data    ָ�����飬��ֺ�Ľ����ʹ��ָ�����ָ��
 *                 limit       ָ�������С
 *                 startChar   ��ʼ�ַ�������'*'��ʾ����str_data�ַ��������Ȳ���'*'�ţ����ҵ�'*'֮ǰ�����κ���������
 *                                 ��ֵ����ΪNULL�����ʾ��ֽ����׸��ַ���ʼ
 *                 endChars    �����ַ�������һ���ַ��������ʱ�����κ�һ�����е��ַ��������������
 *                 splitChar   ����ַ��������жϲ�ֵ�Ψһ��ʶ
 *
 *  example     -
 *
 *  Warning     -  limit ֵ���ܴ��� tar_data ʵ�ʴ�С��������Խ��д����
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
extern int track_fun_str_analyse(char *str_data, char **tar_data, int limit, char startChar, char *endChars, char splitChar);


/******************************************************************************
 *  Function    -  track_fun_str_analyse2
 *
 *  Purpose     -  ���ַ������ض������ַ����в���޸� V2
 *
 *  Description -  str_data    ��Ҫ���в�ֵ��ַ���(ע�⾭�������Դ�ַ��������ı�)
 *                 tar_data    ָ�����飬��ֺ�Ľ����ʹ��ָ�����ָ��
 *                 limit       ָ�������С
 *                 startChar   ��ʼ�ַ�������'*'��ʾ����str_data�ַ��������Ȳ���'*'�ţ����ҵ�'*'֮ǰ�����κ���������
 *                                 ��ֵ����ΪNULL�����ʾ��ֽ����׸��ַ���ʼ
 *                 endChars    �����ַ�������һ���ַ��������ʱ�����κ�һ�����е��ַ��������������
 *                 splitChar   ����ַ����������жϲ�ֵ�Ψһ��ʶ(str_analyse�����������ǵ����ַ�)
 *
 *  example     -
 *
 *  Warning     -  limit ֵ���ܴ��� tar_data ʵ�ʴ�С��������Խ��д����
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 11-09-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
extern int track_fun_str_analyse2(char *str_data, char **tar_data, int limit, char startChar, char *endChars, char *splitChar);


extern kal_uint8 track_fun_check_ip(const char * str);


/*---------------------------------------------------------------------------------------
 * FUNCTION			track_fun_asciiIP2Hex
 *
 * DESCRIPTION		ת��IP��ַ��ʽ
 *
 * PARAMETERS		void *asciiIP	����asc��
 *					void *hex	���ʮ������
 *
 * RETURNS
 *
 *---------------------------------------------------------------------------------------*/
extern kal_bool track_fun_asciiIP2Hex(char *asciiIP, kal_uint8 *hex);


/******************************************************************************
 * FUNCTION:  - track_fun_parseInt
 * DESCRIPTION: - ��10���ƻ���16���Ƶ��ַ���ת��������
 * Input: string ,radix ����10Ϊ10����,16Ϊ16����,���϶��봫0
 * Output:-1Ϊ��������ת��
 * Returns:
  * modification history
 * --------------------
 * v1.0  , 18-06-2012,  WangQi  written
 * --------------------
 ******************************************************************************/
extern int track_fun_parseInt(char * str);

extern kal_uint32 track_fun_parse4(char * str);

extern int track_fun_mem_to_ascii(char *out, int outlimit, void * buff, int size);

extern void track_fun_toUpper(char *buf);


extern void track_fun_make_small_letter(char* buf);


/*---------------------------------------------------------------------------------------
 * FUNCTION			track_fun_string2U16
 *
 * DESCRIPTION		�ַ�ת16λ���
 *
 * PARAMETERS		void *string	��ת�ַ���
 *
 * RETURNS			U16			ת�����(16λ)
 *
 *---------------------------------------------------------------------------------------*/
extern kal_uint16 track_fun_string2U16(void *string);


/******************************************************************************
 *  Function    -  track_fun_interval_range_manage
 *
 *  Purpose     -  ��ָ���е���ֵת��Ϊ����
 *
 *  Description - [min,max]
 *
 * modification history
 * ----------------------------------------
 *
 * ----------------------------------------
 ******************************************************************************/
extern kal_uint16 track_fun_interval_range_manage(char *data_str, U16 min, U16  max);


/******************************************************************************
 *  Function    -  track_fun_Get_GPS_Coordinate
 *
 *  Purpose     -  �����������ת��������
 *
 *  Description -
 *
 * modification history
 * ----------------------------------------
 * v1.0  , 07-08-2010,  jwj  written
 * ----------------------------------------
 ******************************************************************************/
extern kal_bool track_fun_Get_GPS_Coordinate(char* coordinate, double* data);

/******************************************************************************
 *  Function    -  track_fun_check_str_is_number
 *
 *  Purpose     -  ����ַ����ǲ���ȫ���������
 *
 *  Description -  flag == 1 �����ַ����а���'+'��'-'
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 24-12-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
extern kal_uint8 track_fun_check_str_is_number(kal_uint8 flag, const char *str);

/******************************************************************************
 *  Function    -  track_fun_atoi
 *
 *  Purpose     -  ��չatoi����
 *
 *  Description -  ���ַ������ڷ���Ч�ַ�ʱ���� -99
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 25-12-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
extern kal_int32 track_fun_atoi(const char *str);

/******************************************************************************
 *  Function    -  track_fun_atof
 *
 *  Purpose     -  ��չatof����
 *
 *  Description -  ���ַ������ڷ���Ч�ַ�ʱ���� -99
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 25-12-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
extern float track_fun_atof(const char *str);

/*---------------------------------------------------------------------------------------
* FUNCTION			track_fun_check_data_is_port
*
* DESCRIPTION		�ж��Ƿ�Ϊ�˿ں�
*
* PARAMETERS		void *data	asc��˿ں�
*
* RETURNS			BOOL
*
*---------------------------------------------------------------------------------------*/
extern kal_bool track_fun_check_data_is_port(void *data);

extern kal_uint8 track_fun_check_phone_number(char *number);

/******************************************************************************
 *  Function    -  track_fun_abs_degrees
 *
 *  Purpose     -  ������������ĽǶ�ֵ����󲻳���180��
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 17-11-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
extern float track_fun_abs_degrees(float a, float b);

extern kal_uint32 track_fun_convert_phone_number_to_int(char *ascii_number);

/******************************************************************************
 *  Function    -  track_fun_UTF16_B_and_L_swap
 *
 *  Purpose     -  ����UTF16���С�Σ���λ��λ����
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 13-12-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
extern void track_fun_UTF16_B_and_L_swap(kal_uint8 *pOutBuffer, kal_uint8 *pInBuffer, kal_int32 nBuffsize);

extern char *track_fun_hextimestep_view(U8 *data);

extern int track_fun_stringtime_to_hex(char *src, U8 *dec);

/******************************************************************************/
/*   Ӱ��ϵͳ�ຯ��       */
/******************************************************************************/

/******************************************************************************
 *  Function    -  track_fun_update_time_to_system
 *
 *  Purpose     -  ����ϵͳʱ��
 *
 *  Description -  GPS ������������·���ʱ��
 *
 *             app: 1 GPS   ,2 ������
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 11-09-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
extern kal_uint8 track_fun_update_time_to_system(kal_uint8 app, applib_time_struct *utcTime);

/******************************************************************************
 *  Function    -  track_fun_FS_GetDiskInfo
 *
 *  Purpose     -  ���FATʣ��ռ��С
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 12-06-2014,  Cml  written
 * ----------------------------------------
*******************************************************************************/
extern U32 track_fun_FS_GetDiskInfo(void);

/******************************************************************************
 *  Function    -  track_fun_SystemTime_Sync_FromStrTime
 *
 *  Purpose     -  ���ַ�����ʽ��ʾʱ��Ľ�������ͬ����ϵͳ
 *
 *  Description -  �ַ�����ʽ�����磺"2012-9-11 14:36:44"
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 11-09-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
extern kal_int8 track_fun_SystemTime_Sync_FromStrTime(nvram_gps_time_zone_struct *ReferenceValue, char *data);

extern void track_fun_reverse_16(kal_uint8 *dec, kal_uint16 src);

extern void track_fun_reverse_32(kal_uint8 *dec, kal_uint32 src);

extern kal_uint8 *track_fun_strWchr(const kal_uint8 *wchr, kal_uint8 *data, int len);

extern void track_fun_view_UTF16BE_string(kal_uint8 *src_buff, kal_int32 src_len);

extern kal_int32 track_fun_UTF16BE_to_GB2312(kal_uint8 *src_buff, kal_int32 src_len, char *dest_buff, kal_int32 dest_size);

/******************************************************************************
 *  Function    -  track_fun_trim
 *
 *  Purpose     -  ɾ���ַ���ͷβ����ո�
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 02-12-2014,  Cml  written
 * ----------------------------------------
*******************************************************************************/
extern char *track_fun_trim(char *str);

/******************************************************************************
 *  Function    -  track_fun_trim2
 *
 *  Purpose     -  ɾ���ַ���ͷβ�Ŀո��Ʊ�������з�
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 26-05-2016,  Cml  written
 * ----------------------------------------
*******************************************************************************/
extern char *track_fun_trim2(char *str);


extern kal_uint32 track_fun_del_string(char *str, char *sub);
extern kal_uint8* track_fun_stringtohex(kal_uint8 * buf, kal_uint16 len);
#endif  /* _TRACK_FUN_COMMON_H */
