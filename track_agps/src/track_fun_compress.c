/*******************************************************************************************
 * Filename:
 * Author :
 * Date :
 * Comment:
 ******************************************************************************************/

/****************************************************************************
*  Include Files                        ����ͷ�ļ�
*****************************************************************************/
#include "custom_nvram_editor_data_item.h"
#include "app_datetime.h"

#include "track_os_data_types.h"
#include "track_os_log.h"
#include "ell_os.h"

/*****************************************************************************
*  Define                               �궨��
******************************************************************************/

/*****************************************************************************
*  Typedef  Enum                        ö�ٶ���
*****************************************************************************/

/*****************************************************************************
*  Struct                               ���ݽṹ����
******************************************************************************/


/*****************************************************************************
*  Local variable                       �ֲ�����
*****************************************************************************/

/****************************************************************************
*  Global Variable                      ȫ�ֱ���
*****************************************************************************/

/****************************************************************************
*  Global Variable - Extern             ����ȫ�ֱ���
*****************************************************************************/

/*****************************************************************************
*  Global Functions - Extern            �����ⲿ����
******************************************************************************/

/*****************************************************************************
*  Local Functions Define               ���غ�������
******************************************************************************/

/*****************************************************************************
*  Local Functions                      ���غ���
******************************************************************************/

/*****************************************************************************
*  Global Functions                     ȫ�ֺ���
******************************************************************************/

const static U16 crctab16[] = 
{
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

    
/******************************************************************************
 *  Function    -  GetCrc16
 *
 *  Purpose     -  ��������������ݵ�16λCRC
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static U16 GetCrc16(const U8* pData, int nLength)
{
    U16 fcs = 0xffff;    // ��ʼ��
    U8 tmp;
    while(nLength>0)
    {
        fcs = (fcs >> 8) ^ crctab16[(fcs ^ *pData) & 0xff];
        nLength--;
        pData++;
    }
    return ~fcs;
}
    
/******************************************************************************
 *  Function    -  IsCrc16Good
 *
 *  Purpose     -  �������������ݵ�16λCRC�Ƿ���ȷ
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static BOOL IsCrc16Good(const U8* pData, int nLength)
{
    U16 fcs = 0xffff;    // ��ʼ��
    
    while(nLength>0)
    {
        fcs = (fcs >> 8) ^ crctab16[(fcs ^ *pData) & 0xff];
        nLength--;
        pData++;
    }
    
    return (fcs == 0xf0b8);  // 0xf0b8��CRC-ITU��"Magic Value"
}

/******************************************************************************
 *  Function    -  IsDataCrc16
 *
 *  Purpose     -  �ж� CRC-ITU16 У����
 *
 *  Description -  ���ݵ������λΪCRCֵ
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 07-03-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static BOOL IsDataCrc16(const U8* pData, int nLength)
{
    U16 fcs, crc;
    fcs = GetCrc16(pData, nLength - 2);
    crc = *(pData + nLength - 2);
    crc <<= 8;
    crc |= *(pData + nLength - 1);
    return (crc == fcs);
}

/******************************************************************************
 *  Function    -  IsDataCrc16ext
 *
 *  Purpose     -  �жϸ������� CRC-ITU16 У����
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
static kal_bool IsDataCrc16ext(const U8* pData, int nLength, kal_uint16 crc)
{
    kal_uint16 fcs;
    fcs = GetCrc16(pData, nLength);
    LOGD(L_APP, L_V5, "%0.4X ? %0.4X", fcs, crc);
    return (crc == fcs);
}

/******************************************************************************
 *  Function    -  compress_encode
 *
 *  Purpose     -  �г̱���
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int32 compress_encode(kal_uint8 *dect, kal_uint8 *src, int len)
{
    kal_uint8 last = 0, count = 0;
    kal_uint8 *dd = dect;
    kal_uint16 crc;
    kal_bool flag = 1;
    if(len <= 0)
    {
        return -1;
    }
    crc = GetCrc16(src, len);
    dect[0] = (kal_uint8)(crc & 0x00FF);
    dect[1] = (kal_uint8)((crc & 0xFF00) >> 8);
    dect += 2;
    while(len)
    {
        if(flag)
        {
            flag = 0;
        }
        else if(last == 0x81)
        {
            *dect++ = 0x81;
            *dect++ = 0x00;
        }
        else if(last == *src)
        {
            if(count < 0xFF)
            {
                count++;
            }
            else
            {
                *dect++ = 0x81;
                *dect++ = count;
                *dect++ = last;
                count = 0;
            }
        }
        else
        {
            count++;
            if(count > 3)
            {
                *dect++ = 0x81;
                *dect++ = count;
                *dect++ = last;
                count = 0;
            }
            else
            {
                while(count)
                {
                    *dect++ = last;
                    count--;
                }
            }
        }
        last = *src;
        src++;
        len--;
        if(len == 0)
        {
            count++;
            if(count > 3)
            {
                *dect++ = 0x81;
                *dect++ = count;
                *dect++ = last;
                count = 0;
            }
            else
            {
                while(count)
                {
                    if(last == 0x81)
                    {
                        *dect++ = 0x81;
                        *dect++ = 0x00;
                    }
                    else
                    {
                        *dect++ = last;
                    }
                    count--;
                }
            }
        }
    }
    return (dect - dd);
}

/******************************************************************************
 *  Function    -  compress_decode
 *
 *  Purpose     -  ����
 *
 *  Description -  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 29-07-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int32 compress_decode(kal_uint8 *dect, kal_uint8 *src, int len)
{
    kal_uint8 cc = 0;
    kal_uint8 *dd = dect, *src2 = src;
    kal_uint16 crc;
    if(len <= 2)
    {
        return -1;
    }
    src += 2;
    len -= 2;
    while(len)
    {
        if(*src == 0x81)
        {
            if(len == 1 || (src[1] == 0 && len < 2) || (src[1] != 0 && len < 3))
            {
                return -2;
            }
            cc = src[1];
            if(!cc)
            {
                *dect++ = 0x81;
                src += 1;
                len -= 1;
            }
            else
            {
                while(cc)
                {
                    *dect++ = src[2];
                    cc--;
                }
                src += 2;
                len -= 2;
            }
        }
        else
        {
            *dect++ = *src;
        }
        src++;
        len--;
    }
    crc = GetCrc16(dd, dect - dd);
    if(crc != *((kal_uint16*)src2))
    {
        //OTA_LOGH(L_APP, L_V5, &crc, 2);
        //OTA_LOGH(L_APP, L_V5, src2, 2);
        OTA_LOGD(L_APP, L_V1, "error");
        //OTA_LOGH(L_APP, L_V5, dd, dect - dd);
        return -3;
    }
    else
    {
        OTA_LOGD(L_APP, L_V5, "OK");
    }
    return (dect - dd);
}

