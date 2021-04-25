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
#include "c_RamBuffer.h"

#include "track_os_data_types.h"
#include "track_os_log.h"
#include "Conversions.h"
#include "track_os_ell.h"
#include "track_os_paramete.h"

/*****************************************************************************
*  Define                               �궨��
******************************************************************************/
#define PI    3.14159265358979
#define T_PI  0.01745329251994    /* PI/180.0 */
#define EARTH_RAD 6378.1370       /* ȡWGS84��׼�ο������еĵ��򳤰뾶(��λ:km) */
#define DEFAULT_FEB_TIMESEC 1422748800
#define DEFAULT_SYS_TIMESEC 1420070400
/*****************************************************************************
*  Typedef  Enum                        ö�ٶ���
*****************************************************************************/
typedef enum
{
    GPS_STAUS_OFF,       // 0
    GPS_STAUS_ON,
    GPS_STAUS_SCAN,      // 2
    GPS_STAUS_2D,
    GPS_STAUS_3D          // 4
} track_enum_gps_staus;
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

/*---------------------------------------------------------------------------
 * FUNCTION			track_fun_get_GPS_2Points_distance
 *
 * DESCRIPTION		�����ʱ�׼������γ�ȼ����
 *
 * Input:           		double lat1,double lon1 ��һ�����γ��,����
                                    double lat2,double lon2 �ڶ������γ��,����

 * Output:            ���ؾ��뵥λ����
 * Returns:
 *
 * modification history
 * --------------------
 * v1.0  , 27-08-2010,  WangQi  written
 * --------------------
 *���õ�ʱ��û����γ�ȿ��ϱ������ȿ綫�����ж� -- chengjun  2017-03-09
 *---------------------------------------------------------------------------*/
float track_fun_get_GPS_2Points_distance(float lat1, float lon1, float lat2, float lon2)
{
    double radLat1, radLat2, aLat, aLon, s;
    float ret;

    radLat1 = lat1 * T_PI;
    radLat2 = lat2 * T_PI;

    aLat = radLat1 - radLat2;       //γ��֮��
    aLon = (lon1 - lon2) * T_PI;    //����֮��

    s = 2 * asin(sqrt(pow(sin(aLat / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(aLon / 2), 2)));
    s = s * EARTH_RAD * 1000;
    ret = s;
    return ret;
}


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
int track_fun_str_analyse(char *str_data, char **tar_data, int limit, char startChar, char *endChars, char splitChar)
{
    static char *blank = "";
    int len, i = 0, j = 0, status = 0;
    char *p;
    if(str_data == NULL || tar_data == NULL)
    {
        return -1;
    }
    len = strlen(str_data);
    for(i = 0, j = 0, p = str_data; i < len; i++, p++)
    {
        if(status == 0 && (*p == startChar || startChar == NULL))
        {
            status = 1;
            if(j >= limit)
            {
                return -2;
            }
            if(startChar == NULL)
            {
                tar_data[j++] = p;
            }
            else if(*(p + 1) == splitChar)
            {
                tar_data[j++] = blank;
            }
            else
            {
                tar_data[j++] = p + 1;
            }
        }
        if(status == 0)
        {
            continue;
        }
        if(strchr(endChars, *p) != NULL)
        {
            *p = 0;
            break;
        }
        if(*p == splitChar)
        {
            *p = 0;
            if(j >= limit)
            {
                return -3;
            }
            if(strchr(endChars, *(p + 1)) != NULL || *(p + 1) == splitChar)
            {
                tar_data[j++] = blank;
            }
            else
            {
                tar_data[j++] = p + 1;
            }
        }
    }
    for(i = j; i < limit; i++)
    {
        tar_data[i] = blank;
    }
    return j;
}

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
int track_fun_str_analyse2(char *str_data, char **tar_data, int limit, char startChar, char *endChars, char *splitChar)
{
    static char *blank = "";
    int len, i = 0, j = 0, status = 0;
    char *p;
    if(str_data == NULL || tar_data == NULL)
    {
        return -1;
    }
    len = strlen(str_data);
    for(i = 0, j = 0, p = str_data; i < len; i++, p++)
    {
        if(status == 0 && (*p == startChar || startChar == NULL))
        {
            status = 1;
            if(j >= limit)
            {
                return -2;
            }
            if(startChar == NULL)
            {
                tar_data[j++] = p;
            }
            else if(splitChar != NULL && strchr(splitChar, *(p + 1)) != NULL)
            {
                tar_data[j++] = blank;
            }
            else
            {
                tar_data[j++] = p + 1;
            }
        }
        if(status == 0)
        {
            continue;
        }
        if(endChars != NULL && strchr(endChars, *p) != NULL)
        {
            *p = 0;
            break;
        }
        if(strchr(splitChar, *p) != NULL)
        {
            *p = 0;
            if(j >= limit)
            {
                return -3;
            }
            if((endChars != NULL && strchr(endChars, *(p + 1)) != NULL) || (splitChar != NULL && strchr(splitChar, *(p + 1)) != NULL))
            {
                tar_data[j++] = blank;
            }
            else
            {
                tar_data[j++] = p + 1;
            }
        }
    }
    for(i = j; i < limit; i++)
    {
        tar_data[i] = blank;
    }
    return j;
}

kal_bool track_fun_check_ip(const char * str)
{
    kal_uint8 no_count = 0, dot_count = 0;
    while(*str)
    {
        no_count++;
        if(* str == '.')
        {
            dot_count++;
        }
        else if(*str < '0' || *str > '9')
        {
            return 0;
        }
        str++;
    }
    return (!(no_count > 15 || dot_count != 3));
}

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
kal_bool track_fun_asciiIP2Hex(char *asciiIP, kal_uint8 *hex)
{
    char *ip_temp[4], *asc_ip = NULL;
    kal_uint8 index = 0, part_len = 0, *ip_hex, eofstr = 0;
    int ipv = 0;

    if(asciiIP == NULL || hex == NULL)
    {
        return FALSE;
    }
    if(!track_fun_check_ip(asciiIP))
    {
        return FALSE;
    }
    eofstr = 0;
    asc_ip = asciiIP;
    ip_hex = hex;
    ip_temp[index] = asc_ip;

    while(1)
    {
        if(*asc_ip == '.' || *asc_ip == 0)
        {
            if(*asc_ip == 0)
            {
                eofstr = 1;
            }
            *asc_ip = '\0';

            if(part_len == 0)
            {
                return FALSE;//����Ƿ��п�IPֵ�ַ�
            }

            if((ipv = track_fun_parseInt(ip_temp[index])) < 0 || ipv > 255)
            {
                return FALSE;
            }

            *ip_hex = (kal_int8)ipv;

            part_len = 0;

            if(index > 3)
            {
                return FALSE;
            }

            if(eofstr)
            {
                break;
            }

            ip_temp[++index] = ++asc_ip;
            ip_hex++;

            continue;
        }

        part_len++;
        if(part_len > 3)
        {
            return FALSE;   //����3���ַ���IPֵ
        }

        asc_ip++;
    }

    LOGD(L_FUN, L_V5, "index=%d,IP:%d.%d.%d.%d", index, *ip_hex, *(ip_hex + 1), *(ip_hex + 2), *(ip_hex + 3));

    return TRUE ;
}

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
int track_fun_parseInt(char * str)
{
    static int value = 0;
    int radix = 0;
    value = 0;

    if(*str == 0)
    {
        return -1;
    }
    if(*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X'))
    {
        radix = 16;
        str += 2;
    }
    else
    {
        radix = 10;
    }

    while(*str)
    {
        if(radix == 16)
        {
            if(*str >= '0' && *str <= '9')
            {
                value = value * radix + *str - '0';
            }
            else if((*str >= 'a' || *str >= 'A') && (*str <= 'f' || *str <= 'F'))
            {
                value = value * radix + (*str | 0x20) - 'a' + 10;
            }
            else
            {
                value = -1;
                // value = value * radix + *str - 'a' + 10; // Ҳû���Ⱑ
            }
        }
        else
        {
            if(*str >= '0' && *str <= '9')
            {
                value = value * radix + *str - '0';
            }
            else
            {
                value = -1;
            }
        }

        str++;
    }

    return value;
}

kal_uint32 track_fun_parse4(char * str)
{
    kal_uint32 value = 0;
    kal_uint32 radix = 0;
    value = 0;

    if(*str == 0)
    {
        return -1;
    }
    if(*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X'))
    {
        radix = 16;
        str += 2;
    }
    else
    {
        radix = 10;
    }

    while(*str)
    {
        if(radix == 16)
        {
            if(*str >= '0' && *str <= '9')
            {
                value = value * radix + *str - '0';
            }
            else if((*str >= 'a' || *str >= 'A') && (*str <= 'f' || *str <= 'F'))
            {
                value = value * radix + (*str | 0x20) - 'a' + 10;
            }
            else
            {
                value = -1;
            }
        }
        else
        {
            if(*str >= '0' && *str <= '9')
            {
                value = value * radix + *str - '0';
            }
            else
            {
                value = -1;
            }
        }

        str++;
    }

    return value;
}

int track_fun_mem_to_ascii(char *out, int outlimit, void * buff, int size)
{
    char *ptr, *end;
    kal_uint8 *ptr1;
    int i = 0;
    ptr = (char*)out;
    ptr1 = (kal_uint8*)buff;

    if(size <= 0 || out == NULL || outlimit < 8)
    {
        return -1;
    }
    end = out + outlimit - 4;
    sprintf(out, "HEX:");
    ptr += 4;
    while(i < size && ptr < end)
    {
        sprintf(ptr, " %0.2X", *ptr1);
        ptr += 3;
        ptr1++;
        i++;
    }
    *ptr = 0;
    return i;
}

int track_fun_parset_int_skip_space(char * str)
{
    int value = 0;
    while(* str)
    {
        if(*str >= '0' && *str <= '9')
        {
            value = value * 10 + *str - '0';
        }
        else if(*str != ' ')
        {
            return -1;
        }
        str++;
    }

    return value;
}

void track_fun_toUpper(char *buf)
{
    char *ptr = buf;
    while(*ptr)
    {
        if(*ptr >= 'a' && *ptr <= 'z')
        {
            *ptr -= 0x20;
        }
        ptr++;
    }
}

void track_fun_make_small_letter(char* buf)
{
    kal_uint16  i;
    for(i = 0; i < strlen(buf); i++)
    {
        if(buf[i] >= 0x41 && buf[i] <= 0x5a)
        {
            buf[i] += 0x20;
        }
    }
}

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
kal_uint16 track_fun_string2U16(void *string)
{
    S8 str_len;
    S8 *str_ptr;
    U8 i = 0;
    U8 temp[10];
    U16 rpt;

    memset(temp, 0, sizeof(temp));
    str_ptr = (S8 *)string;
    str_len = strlen(string);

    if((str_len >= 6) || (str_len <= 0))
    {
        return 0xFFFF;
    }

    for(i = 0; i < str_len; i++)
    {
        if((*(str_ptr + i)) < '0' || (*(str_ptr + i)) > '9')
        {
            return 0xFFFF;
        }
        temp[str_len - 1 - i] = (*(str_ptr + i)) - '0';
    }

    rpt = temp[4] * 10000 + temp[3] * 1000 + temp[2] * 100 + temp[1] * 10 + temp[0];
    return rpt;
}

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
kal_uint16 track_fun_interval_range_manage(char *data_str, U16 min, U16  max)
{
    U16 interval = 0;
    U8 str_len;
    U8 i = 0;
    S8 interval_data[5];

    str_len = strlen(data_str);
    if((str_len == 1) && (*data_str == '0'))
    {
        interval = 0;
        return 0;
    }

    if(str_len > 5 || str_len == 0)
    {
        return 0xFFFF;
    }

    for(i = 0; i < str_len; i++)
    {
        if(*(data_str + i) < '0' || *(data_str + i) > '9')
        {
            return 0xFFFF;
        }
    }

    memset(interval_data, 0, sizeof(interval_data));
    memcpy(interval_data, data_str, str_len);
    interval = atoi(interval_data);
    if((interval < min) || (interval > max))
    {
        return 0xFFFF;
    }
    return  interval;
}

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
kal_bool track_fun_Get_GPS_Coordinate(char* coordinate, double* data)
{
    U8 i;
    char len;
    double  tmp;
    if((*coordinate == '0'))
    {
        *data = 0;
    }
    else if(!((*coordinate == 'N') || (*coordinate == 'S') || (*coordinate == 'E') || (*coordinate == 'W')))
    {
        return KAL_FALSE;
    }

    len = strlen(coordinate + 1);
    for(i = 1; i < len; i++)
    {
        if(*(coordinate + i) == '.')
        {
            continue;
        }
        if((*(coordinate + i) > '9') || (*(coordinate + i) < '0'))
        {
            *data = 0;
            return KAL_FALSE;

        }
    }
    tmp = atof(coordinate + 1);
    if((*coordinate == 'S') || (*coordinate == 'W'))
    {
        tmp = -tmp;
    }
    if((tmp > 180) || (tmp < -180))
    {
        *data = 0;
        return KAL_FALSE;
    }
    *data = tmp;
    return KAL_TRUE;
}

/******************************************************************************
 *  Function    -  track_fun_check_str_is_number
 *
 *  Purpose     -  ����ַ����ǲ���ȫ���������
 *
 *  Description -  flag & 1 �����ַ����а���'+'��'-'
 *                 flag & 2 �����ַ����а���'.'
 *                 flag & 4 ���ֲ��������7��С��1
 *  modification history
 * ----------------------------------------
 * v1.0  , 24-12-2012,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint8 track_fun_check_str_is_number(kal_uint8 flag, const char *str)
{
    kal_uint8 no_count = 0;
    while(*str)
    {
        if((flag & 1) && no_count == 0 && (*str == '+' || *str == '-'))
        {
            no_count++;
            str++;
        }
        else if((flag & 2) && *str == '.')
        {
            flag &= ~2;
            no_count++;
            str++;
        }
#if defined(__GT740__)||defined(__GT420D__)
		else if(flag & 4)
        {
			if(*str >= '1' && *str <= '7')
			{
            	no_count++;
            	str++;
			}
			else
			{
            	return 0;
			}
        }        
#endif        
        else if(*str >= '0' && *str <= '9')
        {
            no_count++;
            str++;
        }
        else
        {
            return 0;
        }
    }
    return no_count;
}

/******************************************************************************
 *  Function    -  track_fun_check_str_is_number_and_string
 *
 *  Purpose     -  ����ַ����ǲ���ȫ�����ֻ���ĸ���
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 01-06-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_uint8 track_fun_check_str_is_number_and_string(kal_uint8 flag, const char *str)
{
    kal_uint8 no_count = 0;
    while(*str)
    {
        if(*str >= '0' && *str <= '9')
        {
            no_count++;
            str++;
        }
        else if(*str >= 'A' && *str <= 'Z')
        {
            no_count++;
            str++;
        }
        else if(*str >= 'a' && *str <= 'z')
        {
            no_count++;
            str++;
        }
        else
        {
            return 0;
        }
    }
    return no_count;
}


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
kal_int32 track_fun_atoi(const char *str)
{
    if(track_fun_check_str_is_number(1, str) == 0)
    {
        return -99;
    }
    return atoi(str);
}
kal_int32 track_fun_atoi2(kal_uint8 flag,const char *str)
{
    if(track_fun_check_str_is_number(flag, str) == 0)
    {
        return -99;
    }
    return atoi(str);
}

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
float track_fun_atof(const char *str)
{
    if(track_fun_check_str_is_number(3, str) == 0)
    {
        return -99;
    }
    return atof(str);
}

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
kal_bool track_fun_check_data_is_port(void *data)
{
    S8 str_len;
    S8 *str_ptr;
    U8 i = 0;
    U8 temp[10];

    memset(temp, 0, sizeof(temp));
    str_ptr = (S8 *)data;
    str_len = strlen(data);


    if((str_len >= 6) || (str_len <= 0))
    {
        return KAL_FALSE;
    }
    for(i = 0; i < str_len; i++)
    {
        if((*(str_ptr + i)) < '0' || (*(str_ptr + i)) > '9')
        {
            return KAL_FALSE;
        }
        temp[str_len - 1 - i] = (*(str_ptr + i)) - '0';
    }

    if(atoi(str_ptr) > 0xFFFF)
    {
        return KAL_FALSE;
    }
    return KAL_TRUE;

}

kal_uint8 track_fun_check_phone_number(char *number)
{
    kal_uint8 index = 0, len;

    if(number == NULL)
    {
        return 0;
    }

    if(*number == '+')
    {
        //*number = '0';
        index++;
        number++;
    }
    len = track_fun_check_str_is_number(0, number);
    if(len != 0)
    {
        return index + len;
    }
    return 0;
}

void track_fun_reverse_16(kal_uint8 *dec, kal_uint16 src)
{
    dec[0] = (kal_uint8)((src & 0xFF00) >> 8);
    dec[1] = (kal_uint8)(src & 0x00FF);
}

void track_fun_reverse_32(kal_uint8 *dec, kal_uint32 src)
{
    dec[0] = (kal_uint8)((src & 0xFF000000) >> 24);
    dec[1] = (kal_uint8)((src & 0x00FF0000) >> 16);
    dec[2] = (kal_uint8)((src & 0x0000FF00) >> 8);
    dec[3] = (kal_uint8)(src & 0x000000FF);
}

kal_uint8 *track_fun_strWchr(const kal_uint8 *wchr, kal_uint8 *data, int len)
{
    kal_uint8 *out = NULL;
    while(len > 0)
    {
        if(data[0] == wchr[0] && data[1] == wchr[1])
        {
            //return (kal_uint8 *)&data[0];
            out = (kal_uint8 *)&data[0];
        }
        data += 2;
        len -= 2;
    }
    if(out)
    {
        return out;
    }
    return NULL;
}

kal_uint8 track_fun_str_replace_chr(const kal_uint8 *search_chr, const kal_uint8 *replace_chr, kal_uint8 *data, int len)
{
    if(search_chr[0] == 0 && search_chr[1] == 0)
    {
        return 0;
    }
    while(len > 0)
    {
        if(data[0] == search_chr[0] && data[1] == search_chr[1])
        {
            data[0] = replace_chr[0];
            data[1] = replace_chr[1];
        }
        data += 2;
        len -= 2;
    }
    return 1;
}

kal_uint16 track_fun_asc_str_to_ucs2_str(kal_int8 *pOutBuffer, kal_int8 *pInBuffer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_int16 count = -1;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while(*pInBuffer != '\0')
    {
        pOutBuffer[++count] = 0;
        pOutBuffer[++count] = *pInBuffer;
        pInBuffer++;
    }
    pOutBuffer[++count] = '\0';
    pOutBuffer[++count] = '\0';
    return count + 1;
}

kal_uint16 track_fun_ascii2ucs2(kal_int8 *pOutBuffer, kal_int8 *pInBuffer, kal_uint16 length)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_int16 count = -1;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while(*pInBuffer != '\0' && length-- )
    {
        pOutBuffer[++count] = 0;
        pOutBuffer[++count] = *pInBuffer;
        pInBuffer++;
    }
    pOutBuffer[++count] = '\0';
    pOutBuffer[++count] = '\0';
    return count + 1;
}
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
float track_fun_abs_degrees(float a, float b)
{
    float tmp;
    tmp = a - b;
    if(tmp < 0)
    {
        tmp = b - a;
    }
    if(tmp > 180)
    {
        tmp = 360 - tmp;
    }
    return tmp;
}

/*****************************************************************************
 * FUNCTION
 *  srv_phb_sse_convert_number_to_int
 * DESCRIPTION
 *  Converts the last n characters of a phone number to digits
 * PARAMETERS
 *  ascii_number       : [IN]        Input ascii number
 * RETURNS
 *  interger of input number
 *****************************************************************************/
kal_uint32 track_fun_convert_phone_number_to_int(char *ascii_number)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16 num_len;
    U16 ext_len;
    U16 buff_len = MMI_PHB_NUMBER_PLUS_LENGTH;
    S8 *number;
    S8 temp_number[MMI_PHB_NUMBER_PLUS_LENGTH + 1] = {0};
    //������ڶ�ֻƥ��7λ
    U8 compare_length = 7;  /* Maximum convert number length is 9 digits. (4 bytes integer) */

    U32 result;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(ascii_number != NULL)
    {
        strncpy(temp_number, ascii_number, buff_len);
        temp_number[buff_len] = 0;  /* Makre sure there will be terminator */
    }
    else
    {
        return 0;
    }

    number = temp_number;

    /* Skip the first plus sign */
    if(number[0] == '+')
    {
        number++;
    }

    /* number empty */
    if(number[0] == '\0')
    {
        return 0;
    }

    num_len = strlen(number);
    /* Convert at most 7 digits integer */
    if(num_len > compare_length)
    {
        number += num_len - compare_length;
    }
    /* return a 32 bits integer by atol() */
    result = atol(number);

    return result;
}

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
void track_fun_UTF16_B_and_L_swap(kal_uint8 *pOutBuffer, kal_uint8 *pInBuffer, kal_int32 nBuffsize)
{
    int i = 0;
    while(nBuffsize > 0)
    {
        pOutBuffer[0] = pInBuffer[1];
        pOutBuffer[1] = pInBuffer[0];
        pOutBuffer += 2;
        pInBuffer += 2;
        nBuffsize -= 2;
    }
}

void track_fun_view_UTF16BE_string(kal_uint8 *src_buff, kal_int32 src_len)
{
    kal_uint32 src_end_pos;
    char *tmp = NULL, *tmp2 = NULL;
    kal_int32 ret;
    if(src_len >= 1000)
    {
        return;
    }
    tmp = (char*)Ram_Alloc(4, src_len + 2);
    if(tmp == NULL)
    {
        return;
    }
    tmp2 = (char*)Ram_Alloc(4, src_len + 12);
    if(tmp2 == NULL)
    {
        Ram_Free(tmp);
        return;
    }
    memset(tmp, 0, src_len + 2);
    memset(tmp2, 0, src_len + 12);
    track_fun_UTF16_B_and_L_swap(tmp, src_buff, src_len);
    ret = mmi_chset_convert_ex(
              MMI_CHSET_UCS2, MMI_CHSET_GB2312,
              tmp, tmp2,
              src_len + 12,
              &src_end_pos);
    LOGD(L_APP, L_V4, "���ݣ�|%s|", tmp2);
    LOGH(L_APP, L_V6, tmp2, ret);
    Ram_Free(tmp);
    Ram_Free(tmp2);
}

kal_int32 track_fun_UTF16BE_to_GB2312(kal_uint8 *src_buff, kal_int32 src_len, char *dest_buff, kal_int32 dest_size)
{
    kal_uint32 src_end_pos;
    char *tmp = NULL;
    kal_int32 ret;
    if(src_len >= 1000)
    {
        return -1;
    }
    tmp = (U8*)Ram_Alloc(4, src_len + 2);
    if(tmp == NULL)
    {
        return -2;
    }
    memset(tmp, 0, src_len + 2);
    memset(dest_buff, 0, dest_size);
    track_fun_UTF16_B_and_L_swap(tmp, src_buff, src_len);
    ret = mmi_chset_convert_ex(
              MMI_CHSET_UCS2, MMI_CHSET_GB2312,
              tmp, dest_buff,
              dest_size,
              &src_end_pos);
    Ram_Free(tmp);
    LOGD(L_APP, L_V4, "�������ݣ�|%s|", dest_buff);
    LOGH(L_APP, L_V6, dest_buff, ret);
    return ret;
}

kal_int32 track_fun_GB2312_to_UTF16BE(kal_uint8 *src_buff, kal_int32 src_len, char *dest_buff, kal_int32 dest_size)
{
    kal_uint32 src_end_pos;
    char *tmp = NULL;
    kal_int32 ret;
    if(src_len >= dest_size / 2)
    {
        return -1;
    }
    tmp = (U8*)Ram_Alloc(4, src_len * 2 + 2);
    if(tmp == NULL)
    {
        return -2;
    }
    memset(tmp, 0, src_len * 2 + 2);
    memset(dest_buff, 0, dest_size);
    LOGD(L_APP, L_V4, "GB2312_to_UTF16BE: |%s|", src_buff);
    ret = mmi_chset_convert_ex(
              MMI_CHSET_GB2312, MMI_CHSET_UCS2,
              src_buff, tmp,
              dest_size,
              &src_end_pos);
    track_fun_UTF16_B_and_L_swap(dest_buff, tmp, ret);
    Ram_Free(tmp);
    LOGH(L_FUN, L_V6, src_buff, src_len);
    LOGH(L_FUN, L_V7, dest_buff, ret);
    return ret;
}

char *track_fun_strncpy(char *dec, const char *src, int size)
{
    strncpy(dec, src, size - 1);
    dec[size - 1] = 0;
    return dec;
}

char *track_fun_hextimestep_view(U8 *data)
{
    static char tmp[12] = {0};
    snprintf(tmp, 12, "%02d:%02d-%02d:%02d", data[0], data[1], data[2], data[3]);
    return tmp;
}

/******************************************************************************
 *  Function    -  track_fun_stringtime_to_hex
 *
 *  Purpose     -  ʱ���ַ�����﷽ʽת���ֽڱ�﷽ʽ
 *
 *  Description -
 *        "11:00-11:30"  ->   0B000B1E
 *  modification history
 * ----------------------------------------
 * v1.0  , 15-09-2014,  Cml  written
 * ----------------------------------------
*******************************************************************************/
int track_fun_stringtime_to_hex(char *src, U8 *dec)
{
    char tmp[12] = {0}, *valuestr = NULL;
    int i, step = 0, value, srclen = strlen(src);
    for(i = 0; i < srclen; i++)
    {
        if(src[i] == ' ') continue;
        if(src[i] == ':')
        {
            src[i] = 0;
            if(valuestr == NULL || strlen(valuestr) < 0 || strlen(valuestr) > 2)
                return -2;
            value = atoi(valuestr);
            if(value < 0 || value > 23)
                return -3;
            dec[step++] = value;
            valuestr = NULL;
            continue;
        }
        if(src[i] == '-')
        {
            src[i] = 0;
            if(valuestr == NULL || strlen(valuestr) < 0 || strlen(valuestr) > 2)
                return -2;
            value = atoi(valuestr);
            if(value < 0 || value > 59)
                return -3;
            dec[step++] = value;
            valuestr = NULL;
            continue;
        }
        if(src[i] < '0' || src[i] > '9')
        {
            return -1;
        }
        if(valuestr == NULL) valuestr = &src[i];
    }
    if(valuestr == NULL || strlen(valuestr) < 0 || strlen(valuestr) > 2)
        return -4;
    value = atoi(valuestr);
    if(value < 0 || value > 59)
        return -5;
    dec[step++] = value;
    return 0;
}

/******************************************************************************
 *  Function    -  strftime
 *
 *  Purpose     -  �ַ�������ת������ֵ
 *
 *  Description -
 *      Sep 28 2014 09:02:45    ->     2014/09/28 08:58
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 28-09-2014,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int8 strtoime(const char *date, const char *time, applib_time_struct *t)
{
    //Sep 28 2014 09:02:45       Oct  9 2014 15:07:22
    //2014/09/28 08:58
    int i;
    char *cmds[10] = {0}, *p, *end;
    char tmp[50] = {0};
    char m[13][4] = {" ", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    snprintf(tmp, 50, "%s %s", date, time);
    LOGD(L_APP, L_V5, "%s,%s ->", date, time);
    if(t == NULL || date == NULL || time == NULL) return -1;
    if(strlen(date) < 5 || strlen(time) < 5) return -2;
    p = (char *)tmp;
    while(*p == ' ' && *p != 0)
    {
        p++;
    }
    end = p;
    while(*end != ' ' && *end != ':' && *end != 0)
    {
        end++;
    }
    *end = 0;
    LOGH(L_APP, L_V5, p, strlen(p));
    for(i = 1; i <= 12; i++)
    {
        LOGD(L_APP, L_V5, "[%s],[%s] ->", m[i], p);
        if(!strcmp(m[i], p))
        {
            t->nMonth = i;
            break;
        }
    }
    p = end + 1;
    while(*p == ' ' && *p != 0)
    {
        p++;
    }
    end = p;
    while(*end != ' ' && *end != ':' && *end != 0)
    {
        end++;
    }
    *end = 0;
    t->nDay  = atoi(p);

    p = end + 1;
    while(*p == ' ' && *p != 0)
    {
        p++;
    }
    end = p;
    while(*end != ' ' && *end != ':' && *end != 0)
    {
        end++;
    }
    *end = 0;
    t->nYear = atoi(p);

    p = end + 1;
    while(*p == ' ' && *p != 0)
    {
        p++;
    }
    end = p;
    while(*end != ' ' && *end != ':' && *end != 0)
    {
        end++;
    }
    *end = 0;
    t->nHour = atoi(p);

    p = end + 1;
    while(*p == ' ' && *p != 0)
    {
        p++;
    }
    end = p;
    while(*end != ' ' && *end != ':' && *end != 0)
    {
        end++;
    }
    *end = 0;
    t->nMin  = atoi(p);

    p = end + 1;
    while(*p == ' ' && *p != 0)
    {
        p++;
    }
    /*end = p;
    while(*end != ' ' && *end != ':' && *end != 0)
    {
        end++;
    }
    *end = 0;*/
    t->nSec  = atoi(p);
    LOGD(L_APP, L_V5, "%s,%s -> %02d-%02d-%02d %02d:%02d:%02d", date, time,
         t->nYear, t->nMonth, t->nDay, t->nHour, t->nMin, t->nSec);
    return 0;
}

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
char *track_fun_trim(char *str)
{
    int i, len = strlen(str);
    kal_bool beg = KAL_TRUE; //�������ҿ�ͷ�Ŀո�
    char *p = str, *end = NULL;
    if(len <= 0)
        return "";
    while(len--)
    {
        if(beg && *p == ' ')
        {
            p++;
            continue;
        }
        if(beg)
        {
            beg = KAL_FALSE;
            str = p;
        }
        if(*p != ' ') end = p;
        p++;
    }
    if(beg) return "";
    if(end != NULL)
        *(end + 1) = '\0';
    return str;
}

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
char *track_fun_trim2(char *str)
{
	int len = strlen(str);
	char *p = str + len - 1;
	bool shunxu = KAL_FALSE;
	while(*p)
	{
		if(*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
		{
			if(shunxu) p++;
			else p--;
		}
		else if(shunxu)
		{
			break;
		}
		else
		{
			if(p < str + len - 1) *(p+1) = 0;
			shunxu = KAL_TRUE;
			p = str;
		}
	}
	return p;
}

/******************************************************************************/
/*   Ӱ��ϵͳ�ຯ��       */
/******************************************************************************/

void track_fun_msg_send(kal_uint16 msg_id, void *req, module_type mod_src, module_type mod_dst)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct *ilm_send;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ilm_send = (ilm_struct *)allocate_ilm(mod_src);
    ilm_send->src_mod_id = mod_src;
    ilm_send->dest_mod_id = (module_type)mod_dst;
    ilm_send->sap_id = INVALID_SAP;
    ilm_send->msg_id = (msg_type) msg_id;
    ilm_send->local_para_ptr = (local_para_struct*) req;
    ilm_send->peer_buff_ptr = (peer_buff_struct*) NULL;
    msg_send_ext_queue(ilm_send);
}

int track_utc_to_gpsepo_hour (kal_int32 iYr, kal_int32 iMo, kal_int32 iDay, kal_int32 iHr)
{
    kal_int32 iYearsElapsed; // Years since 1980
    kal_int32 iDaysElapsed; // Days elapsed since Jan 6, 1980
    kal_int32 iLeapDays; // Leap days since Jan 6, 1980
    kal_int32 i;
    // Number of days into the year at the start of each month (ignoring leap years)
    const kal_int32 doy[12] = {0,31,59,90,120,151,181,212,243,273,304,334};
    iYearsElapsed = iYr - 1980;
    i = 0;
    iLeapDays = 0;
    while (i <= iYearsElapsed)
    {
        if ((i % 100) == 20)
        {
            if ((i % 400) == 20)
            {
                iLeapDays++;
            }
        }
        else if ((i % 4) == 0)
        {
            iLeapDays++;
        }
        i++;
    }
    if ((iYearsElapsed % 100) == 20)
    {
        if (((iYearsElapsed % 400) == 20) && (iMo <= 2))
        {
            iLeapDays--;
        }
    }
    else if (((iYearsElapsed % 4) == 0) && (iMo <= 2))
    {
        iLeapDays--;
    }
    iDaysElapsed = iYearsElapsed * 365 + doy[iMo - 1] + iDay + iLeapDays - 6;
    // Convert time to GPS weeks and seconds
    return (iDaysElapsed * 24 + iHr);
}
/******************************************************************************
 *  Function    -  track_fun_update_time_to_system
 *
 *  Purpose     -  ����ϵͳʱ�䣬�����������ҪʱUTCʱ��
 *
 *  Description -  GPS ������������·���ʱ��
 *
 *             app:     1=GPS;2=������;3=AGPS������;4=��Ƭ������  
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 11-09-2012,  Cml  written
 * v2.0  , 2016-11-16,  Xzq  710���ӵ�Ƭ����GSMͬ��ʱ�䶯��  4 ST
 * v2.1  , 2016-12-12,  Xzq  �붨����ֻ��������ȡʱ���ſ���
 * ----------------------------------------
*******************************************************************************/
kal_uint8 track_fun_update_time_to_system(kal_uint8 app, applib_time_struct *t)
{
    /* 1422748800 = 2015-02-01 00:00:00*/
    static kal_uint8 appid = 0;
    applib_time_struct tmp = {0};
	U32 t1, t2;
    static kal_bool firstUpdate = KAL_TRUE, epoading=KAL_TRUE;
    applib_dt_get_rtc_time(&tmp);

#if defined(__JM81__) || defined(__GT740__)||defined (__JM66__)||defined(__GT420D__)
 	if((app >= 1 && app <= 3) || (app==4 && firstUpdate))
#else
 	if(app >= 1 && app <= 3)
#endif
    {
	    if(firstUpdate)
            {
			LOGD(L_APP, L_V6, "firstUpdate=%d, app=%d time: %0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", firstUpdate, app,
         t->nYear, t->nMonth, t->nDay,
         t->nHour, t->nMin, t->nSec);
            appid = app;
            t1 = OTA_applib_dt_mytime_2_utc_sec(&tmp, 0);
            t2 = OTA_applib_dt_mytime_2_utc_sec(t, 0);
            LOGD(L_DRV, L_V1, "����ϵͳʱ��[%d]",app);
        LOGD(L_APP, L_V5, "time: %0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",t->nYear, t->nMonth, t->nDay,
         t->nHour, t->nMin, t->nSec);
            track_drv_update_time_to_system(*t);
			
#if defined(__GT740__)||defined(__GT420D__)
            #if defined(__ZKE_EPO__)
            if(t->nYear == 0 && t->nMonth == 0 && t->nDay==0) return;
            #endif
	    if(firstUpdate || (t1 < DEFAULT_FEB_TIMESEC && t2 > DEFAULT_FEB_TIMESEC))
            {
                LOGD(L_APP,L_V5,"1");
               track_cust_syn_st_time(t2 - t1,app);
            }
            if(app!=4)//ST��Уʱ��Ӱ��ԭ��Уʱ�߼�
            {
            	char bufff[100]={0};
            	sprintf(bufff,"time(%d)(%d): %0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d ",app,(kal_get_systicks()-0)/KAL_TICKS_1_SEC,t->nYear, t->nMonth, t->nDay, t->nHour, t->nMin, t->nSec);
            	//if(G_parameter.extchip.mode!=1)track_cust_cache_netlink(bufff,strlen(bufff));
               firstUpdate = KAL_FALSE;
               #if defined(__ZKE_EPO__)
		  if(app!=1)track_drv_aiding_zkwepo();
               #endif
            }
            {
                extern kal_uint8 track_cust_wifi_packet_option(kal_uint8 sends);
                extern kal_uint16 track_cust_paket_2C(void);
                if(track_cust_wifi_packet_option(99))
                {
                	track_cust_wifi_packet_option(0);
                	track_os_intoTaskMsgQueue(track_cust_paket_2C);
                }
            }
#else
           firstUpdate = KAL_FALSE;
#endif      
        }
    }
#if defined(__GT740__)||defined(__GT420D__)
#else
    else if(appid == 0)
    {
        if(!(tmp.nYear < 2015 || (tmp.nYear == 2015 && tmp.nMonth == 1)))
        {
            return 99;
        }
    }
#endif
#ifdef __SECOND_SET__
		if(!track_is_testmode() && (app==2 || app==3) && track_cust_apgs_sw_status() == 1 )
		{
		    U8 gstus;
		    track_cust_epo_int();
		    gstus = track_cust_gps_status();
		    if((gstus<GPS_STAUS_2D && gstus>GPS_STAUS_OFF))
		    {
                        #if !defined(__GT420D__)
			    track_cust_aiding_reftime(t);
                        #endif
		    }
		}
#endif			

    return appid;
}

/******************************************************************************
 *  Function    -  track_fun_get_time
 *
 *  Purpose     -  ��ȡϵͳʱ��
 *
 *  Description -
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 05-08-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_fun_get_time(applib_time_struct *t, kal_bool isUtc, nvram_gps_time_zone_struct *zone)
{
    if(isUtc)
    {
        applib_dt_get_rtc_time(t);
    }
    else if(zone != NULL)
    {
        static applib_time_struct tmp = {0};
        applib_dt_get_rtc_time(&tmp);
        /* ʱ�����ʱ��У�� */
        track_drv_utc_timezone_to_rtc(t, &tmp, *zone);
    }
}

kal_uint16 app_ucs2_str_n_to_asc_str_B(kal_int8 *pOutBuffer, kal_int8 *pInBuffer, kal_uint32 len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    kal_uint16 count = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while((len) && (!((*pInBuffer == 0) && (*(pInBuffer + 1) == 0))))
    {
        //*pOutBuffer = *(pInBuffer);

        //#ifdef __FOR_TESTING    /* MMI_ON_HARDWARE_P */
        *pOutBuffer = *(pInBuffer + 1);
        //#endif
        pInBuffer += 2;
        pOutBuffer++;
        count++;
        len -= 2;
    }

    return count;
}

/******************************************************************************
 *  Function    -  track_fun_Conversion_Camma_to_ASCII
 *
 *  Purpose     -  ���Ų���ȫ�ǰ��
 *
 *  Description -����: ascii =0x2C �� ucs2=0xFF0C

 * modification history
 * ----------------------------------------
 * ----------------------------------------
 ******************************************************************************/
void track_fun_Conversion_Camma_to_ASCII(char* content)
{
    int chr = 0x0C;
    S8 *Pstr, *Pcontent;

    Pcontent = (char*)content;
    while((Pstr = strchr(Pcontent, chr)) != NULL)
    {
        *Pstr = 0x2C;
        Pcontent = Pstr;
    }
}

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
kal_int8 track_fun_SystemTime_Sync_FromStrTime(nvram_gps_time_zone_struct *ReferenceValue, char *data)
{
    applib_time_struct time = {0}, time1 = {0};
    char cmd_str[20] = {0};
    char *cmds[10] = {0};
    int i, tmp;
    kal_uint8 count;
    if(strlen(data) >= 20) return -1;
    strcpy(cmd_str, data);
    count = track_fun_str_analyse2(cmd_str, cmds, 10, NULL, NULL, "- :");
    if(count != 6) return -2;
    for(i = 0; i < 6; i++)
    {
        if(strlen(cmds[i]) == 0) return -3;
    }

    tmp = atoi(cmds[0]);
    time.nYear = tmp;

    tmp = atoi(cmds[1]);
    if(tmp < 1 || tmp > 12) return -4;
    time.nMonth = tmp;

    tmp = atoi(cmds[2]);
    if(tmp < 1 || tmp > 31) return -5;
    time.nDay = tmp;

    tmp = atoi(cmds[3]);
    if(tmp < 1 || tmp > 24) return -6;
    time.nHour = tmp;

    tmp = atoi(cmds[4]);
    if(tmp < 1 || tmp > 60) return -7;
    time.nMin = tmp;

    tmp = atoi(cmds[5]);
    if(tmp < 1 || tmp > 60) return -8;
    time.nSec = tmp;

    track_drv_utc_timezone_to_rtc(&time1, &time, *ReferenceValue);

    track_fun_update_time_to_system(2, &time1);
}

/******************************************************************************
 *  Function    -  track_domain_encode
 *
 *  Purpose     -  �������뺯��
 *
 *  Description -  ע�⣺����Ĭ��domain������󳤶�Ϊ�궨��ĳ��� TRACK_MAX_URL_SIZE
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
void track_domain_encode(char *src, kal_uint8 *dec)
{
    int i, len;
    len = strlen(src);
    if(len >= TRACK_MAX_URL_SIZE)
    {
        LOGD(L_APP, L_V2, "URL encode error!");
        return;
    }
    memset(dec, 0, TRACK_MAX_URL_SIZE);
    for(i = 0; i < len; i++)
    {
        dec[i] = (kal_uint8)src[i] + 128;
    }
}

/******************************************************************************
 *  Function    -  track_domain_decode
 *
 *  Purpose     -  �������뺯��
 *
 *  Description -  ע�⣺����Ĭ��domain������󳤶�Ϊ�궨��ĳ��� TRACK_MAX_URL_SIZE
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 09-11-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
char *track_domain_decode(kal_uint8 *data)
{
    static kal_uint8 buf[TRACK_MAX_URL_SIZE] = {0};
    kal_uint8 tmp[TRACK_MAX_URL_SIZE] = {0};
    int i, len;
    len = strlen(data);
    if(len >= TRACK_MAX_URL_SIZE)
    {
        LOGD(L_APP, L_V2, "URL encode error!");
        return NULL;
    }
    memcpy(tmp, data, TRACK_MAX_URL_SIZE);
    for(i = 0; i < len; i++)
    {
        tmp[i] -= 128;
    }
    if(memcmp(tmp, buf, TRACK_MAX_URL_SIZE))
    {
        memcpy(buf, tmp, TRACK_MAX_URL_SIZE);
    }
    return (char *)buf;
}

/******************************************************************************
 *  Function    -  track_fun_cov_number
 *  Purpose     -  ���ݺ������ͼ�+��
 *  Description -  ע�⣺����
 *  modification history
 * ----------------------------------------
 * v1.0  , 14-12-2013,  WangQi  written
 * ----------------------------------------
*******************************************************************************/
kal_uint8 track_fun_cov_number(kal_uint8 number_type, char* number, char* sta_number, kal_uint8 length)
{
    int index = 0;
    LOGD(L_APP, L_V5, "number_type = %d", number_type);
    if(number_type == 145)
    {
        sta_number[0] = '+';
        index++;
    }
    memcpy(&sta_number[index], number, length);
    LOGD(L_APP, L_V5, "sta_number:%s", sta_number);

}

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
U32 track_fun_FS_GetDiskInfo(void)
{
    FS_DiskInfo disk_info;
    U32 disk_free_space;
    S32 fs_ret;

    fs_ret = FS_GetDiskInfo(L"C:\\", &disk_info, FS_DI_BASIC_INFO | FS_DI_FREE_SPACE);
    disk_free_space = disk_info.FreeClusters * disk_info.SectorsPerCluster * disk_info.BytesPerSector;
    return disk_free_space;
}

/******************************************************************************
 *  Function    -  track_fun_del_string
 * 
 *  Purpose     -  ɾ���ַ����ض�����
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 06-05-2016,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint32 track_fun_del_string(char *str, char *sub)
{
    char *psrc = str, *pdest = str;  //����ָ�붼ָ��str����psrcָ����ַ���������������pdest
    char *p, *psub;
    char count = 0;

    if((str == NULL) || (sub == NULL))
    {
        return 0;
    }
    while(*psrc)
    {
        p = psrc;
        psub = sub;
        while(*p && *p == *psub)
        {
            p++;
            psub++;
        }

        if(*psub == 0)//�ҵ�һ���Ӵ�
        {
            psrc = p;
            count++;
        }
        else
        {
            *pdest++ = *psrc++;
        }
    }
    *pdest = 0;
    return count;
}

/******************************************************************************
 *  Function    -  track_fun_stringtohex
 * 
 *  Purpose     -  ���ַ���ֱ��ת����HEX ��"0b7f"   0x0b 0x7f
 * 
 *  Description -   buf:��ת��������len:Ҫ��װ���ĳ���
                            �˺���û����ǿ�Ƽ��������ַ������𳬳����߲����淶
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 09-05-2016,  ZengPing  written
 * ----------------------------------------
 ******************************************************************************/
kal_uint8* track_fun_stringtohex(kal_uint8 * buf, kal_uint16 len)
{
    int size, i, ret, index, tmp_i, tmp_j;
   static  kal_uint8 hex_mac[20] = {0};
    kal_uint8 highByte, lowByte;
   memset(hex_mac, 0, 20);
    LOGD(L_APP, L_V7, "buf = %s", buf);
    if(len>20)
    {
        len=20;
    }
    if(strlen(buf) > 0)
    {
        for(tmp_i = 0, tmp_j = 0; tmp_i < len; tmp_i++)
        {

            highByte = buf[tmp_j];
            lowByte  = buf[tmp_j+1];

//            LOGD(L_APP, L_V5, "highByte = %d", highByte);
//            LOGD(L_APP, L_V5, "lowByte = %d",lowByte);

            if(highByte > 0x59)
            {
                highByte -= 0x57;
            }
            else if(highByte > 0x39 )
            {
                highByte -= 0x37;
            }
            else
            {
                highByte -= 0x30;
            }

            if(lowByte > 0x59)
            {
                lowByte -= 0x57;
            }
            else if(lowByte > 0x39 )
            {
                lowByte -= 0x37;
            }
            else
            {
                lowByte -= 0x30;
            }
            hex_mac[tmp_i] = (highByte << 4) | lowByte;
            tmp_j += 2;
        }
        LOGH(L_APP, L_V5, hex_mac, 20);
    }
    return hex_mac;
}

/******************************************************************************
 *  Function    -  track_fun_sort_HighToLow
 * 
 *  Purpose     -  һ�����������ɴ�С
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 22-04-2017,  Cml  written
 * ----------------------------------------
 ******************************************************************************/
void track_fun_sort_HighToLow(U8 *data, int len)
{
    kal_bool flag;
    U8 swap = 0;
    int i;
    do{
        flag = 0;
        for(i=0;i<len-1;i++)
        {
            if(data[i] < data[i+1])
            {
                swap = data[i];
                data[i] = data[i+1];
                data[i+1] = swap;
                flag = 1;
            }
        }
    }while(flag);
}

int track_fun_ASCII_to_HEX(unsigned char *source, unsigned char *dectdata, int dectMax)
{
    unsigned char *p = source;
    unsigned char *out = dectdata;
    bool high = 1;
    unsigned char valid = 0;
    while(*p && out - dectdata < dectMax)
    {
        if(*p == ' ' || *p == ',' || *p == '\t' || *p == '\r' || *p == '\n')
        {
            p++;
            high = 1;
            continue;
        }
        if(*p == '0' && (*(p + 1) == 'x' || *(p + 1) == 'X'))
        {
            p += 2;
            continue;
        }
        if(*p >= '0' && *p <= '9')
        {
            valid = *p - 48;
        }
        else if(*p >= 'A' && *p <= 'F')
        {
            valid = *p - 55;
        }
        else if(*p >= 'a' && *p <= 'f')
        {
            valid = *p - 87;
        }
        else
        {
            p++;
            high = 1;
            continue;
        }
        if(high)
        {
            *out = valid << 4;
            high = 0;
        }
        else
        {
            *out |= valid;
            out++;
            high = 1;
        }
        p++;
    }
    return out - dectdata;
}
/******************************************************************************
 *  Function    -  track_fun_quicksort
 *  Description -  ���������㷨
 ******************************************************************************/
static int track_fun_split_small2big(kal_uint16 a[], int low, int high)
{
    kal_uint16 part_element =  a[low];
    for (;;)
    {
    	while (low<high&&part_element <= a[high])high--;//�Ӵ�С���Ϊ>=
    	if (low >= high)break;
    	a[low++] = a[high];
    	while (low<high&&part_element >= a[low])low++;//�Ӵ�С���Ϊ<=
    	if (low >= high)break;
    	a[high--] = a[low];
    }
    a[high] = part_element;
    return high;
}
void track_fun_quicksort(kal_uint16 a[], int low, int high)
{
    int middle;
    if (low >= high)return;
    middle = track_fun_split_small2big(a, low, high);
    track_fun_quicksort(a, low, middle - 1);
    track_fun_quicksort(a, middle +1, high);
}
#if defined(__JM66__)
static kal_uint16 const CRC16Table[256] = { 
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};
/*******************************************************************************
*��������: CRC16
*��������: �������CRC16.
*�������: dataIn -- ��У������
*                           length -- ���ݳ���
*�� �� ֵ: У��ֵ
*******************************************************************************/
kal_uint16 CRC16(kal_uint8* dataIn, int length)
{ 
    kal_uint16 result = 0;
    kal_uint16 tableNo = 0;
    int i;
    for(i = 0; i < length; i++)
    {
        tableNo = ((result & 0xff) ^ (dataIn[i] & 0xff));
        result = ((result >> 8) & 0xff) ^ CRC16Table[tableNo];
    }

    return result;
}
#endif//__JM66__

kal_uint8 track_fun_to_lower(kal_uint8 ch)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (ch >= 'A' && ch <= 'Z')
    {
        return (kal_uint8) (ch + ('a' - 'A'));
    }
    return ch;
}