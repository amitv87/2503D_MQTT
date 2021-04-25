#ifndef _DECRYPTION_H_
#define _DECRYPTION_H_

/* portable character for multichar character set */
typedef char                    kal_char;
/* portable wide character for unicode character set */
typedef unsigned short          kal_wchar;

/* portable 8-bit unsigned integer */
typedef unsigned char           kal_uint8;
/* portable 8-bit signed integer */
typedef signed char             kal_int8;
/* portable 16-bit unsigned integer */
typedef unsigned short int      kal_uint16;
/* portable 16-bit signed integer */
typedef signed short int        kal_int16;
/* portable 32-bit unsigned integer */
typedef unsigned int            kal_uint32;
/* portable 32-bit signed integer */
typedef signed int              kal_int32;

typedef enum 
{
    /* FALSE value */
    KAL_FALSE,
    /* TRUE value */
    KAL_TRUE
} kal_bool;

/******************************************************************************
 *  Function    -  track_fun_decryption
 *
 *  Purpose     -  EPO���ܺ���
 *
 *  Description -  ����
 *                 dec  ----  ���ܺ��Ŀ��Buffer(���ȶ�����ڴ����ܵĳ���)
 *                 src  ----  �����ܵ�����Buffer
 *                 szie ----  �����ܵ����ݳ���
 *  Return
 *        >0   ���ܳɹ���ֵ���ڽ��ܺ���ļ���С
 *        -1   ����ʧ�ܣ��ļ�δ������
 *        -2   ����ʧ�ܣ�У�����
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 06-08-2013
 * ----------------------------------------
*******************************************************************************/
extern kal_int32 track_fun_decryption(kal_uint8 *dec, kal_uint8 *src, int size);
#endif
