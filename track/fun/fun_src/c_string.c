/* ============================================================================
* C string implementation
*
* ����STL��string��ʵ��
*
* ���ߣ�skybuf  <MSN:skybuf@gmail.com>
*
* ��Դ���룬�����뽻��֮�ã��ް�Ȩ
*


	090512change: 1. �ڱȽϡ��滻��׷�ӡ����ơ�����������������C�ַ�����β����
			2. �޸���erase��BUG��ֻ�����˳��ȣ�������ָ��λ�ò�����ʣ���ַ����Ĳ���ʱ�Ǹ�����
*
* ========================================================================= */
#if !defined (__REMOVE_LOG_TO_FILE__)

#include "c_string.h"
#include <string.h>
#include <stdlib.h>
#include "kal_release.h"
#include "kal_trace.h"
#include "stack_common.h"
#include "stack_msgs.h"
#include "app_ltlcom.h" /* Task message communiction */
#include "syscomp_config.h"
#include "task_config.h"        /* Task creation */
#include "app_buff_alloc.h"     /* Declaration of buffer management API */
#include "stacklib.h"   /* Basic type for dll, evshed, stacktimer */
#include "event_shed.h" /* Event scheduler */
#include "stack_timer.h"        /* Stack timer */
#include "c_RamBuffer.h"

#define DEFAULT_CAPACITY_SIZE_ADD		64
#define DEFAULT_CAPACITY_LIMIT			1024
#define DEFAULT_CAPACITY_NEW_MUL		1.5
#define DEFAULT_CAPACITY_RELEASE_LIMIT	2048

/**
* string��ʵ�ֽṹ��
*/
typedef struct
{
    c_string base;					/*�ӿڼ�*/
    unsigned int length;			/*�ַ����ĵ�ǰ����*/
    unsigned int capacity;			/*��ǰ�Ŀռ��С*/
    char* content;					/*��̬�ַ���������*/
} c_string_impl_t;


static int append(struct _c_string* obj, const char* str, unsigned int pos_start, int num);
static int append_string(struct _c_string* obj, const struct _c_string* string);
static int compare(struct _c_string* obj, const char* str, unsigned int pos_start, int num, unsigned char b_uplower);
static int compare_string(struct _c_string* obj, const struct _c_string* string, unsigned char b_uplower);
static int copy(struct _c_string* obj, const char* str, unsigned int pos_start, int num);
static int copy_string(struct _c_string* obj, const struct _c_string* string);
static int erase(struct _c_string* obj, unsigned int pos_start, int num);
static int replace(struct _c_string* obj, unsigned int pos_start, int num, const char* str);
static int replace_str(struct _c_string* obj, const char* str_src, const char* str_dst);
static int insert(struct _c_string* obj, unsigned int pos_start_obj, const char* str, unsigned int pos_start_str, int num_str);
static int insert_string(struct _c_string* obj, unsigned int pos_start_obj, const struct _c_string* string);
static int insert_ch(struct _c_string* obj, unsigned int pos_start_obj, char ch, unsigned int num_ch);
static int find_first_of(struct _c_string* obj, const char* str, unsigned int pos_start, int num);
static int find_first_string_of(struct _c_string* obj, struct _c_string* string);
static int find_first_ch_of(struct _c_string* obj, char ch);
static int r_find_first_of(struct _c_string* obj, const char* str, unsigned int pos_start, int num);
static int r_find_first_string_of(struct _c_string* obj, struct _c_string* string);
static int r_find_first_ch_of(struct _c_string* obj, char ch);
static char at(struct _c_string* obj, unsigned int index);
static const char* str(struct _c_string* obj);
static void clear(struct _c_string* obj);
static unsigned char empty(struct _c_string* obj);
static int length(struct _c_string* obj);
static struct _c_string* substr(struct _c_string* obj, unsigned int pos_start, int num);
static void swap(struct _c_string* obj, struct _c_string* string);
static void to_upper(struct _c_string* obj);
static void to_lower(struct _c_string* obj);

/**
* �����µ�������С
*
* @param impl			c_string_impl_tʵ��ָ��
* @param capacity_new	�µ�������С
*
* @return				STRING_ALL_OK��ʾ�ɹ��������ʾʧ��
*
* @note					�������ԣ� ���capacity_new��С��Ŀǰ��capacity��
						�����յ�������СΪcapacity_new��DEFAULT_CAPACITY_NEW_MUL����

						����DEFAULT_CAPACITY_NEW_MUL��֮�󣬴�С��Խ��DEFAULT_CAPACITY_LIMIT��
						�����յ�����Ϊcapacity_new+DEFAULT_CAPACITY_SIZE_ADD
*
*/
static int l_make_new_capacity(c_string_impl_t* impl, unsigned int capacity_new)
{
    int cap = (int)(capacity_new * DEFAULT_CAPACITY_NEW_MUL);	/*������Ϊ��Ҫ��������DEFAULT_CAPACITY_NEW_MUL��*/
    /*
    	kal_prompt_trace(TRACE_INFO,"&&&    l_make_new_capacity( %d )"  ,  capacity_new  );
    	kal_prompt_trace(TRACE_INFO,"impl->capacity = %d", impl->capacity   );
    	kal_prompt_trace(TRACE_INFO,"cap = %d", cap   );
    */
    if(capacity_new < impl->capacity)
        return STRING_ALL_OK;

    /*�����µ�����*/
    if(cap > DEFAULT_CAPACITY_LIMIT)  /*��չ��DEFAULT_CAPACITY_NEW_MUL�������DEFAULT_CAPACITY_LIMIT���������DEFAULT_CAPACITY_SIZE_ADD�ֽ�*/
        cap = capacity_new + DEFAULT_CAPACITY_SIZE_ADD;

    {
        /*�������������ַ���*/
        char* data = (char*)Ram_Alloc(0, cap);


        if(!data)
            return STRING_ERR_NO_MEMORY;
        /*memset(data, 0, cap);  */ /*����set����length���Ƽ���*/

        if(impl->length)
        {
            /*����ԭ������*/
            //strcpy(data, impl->content);
            memcpy(data, impl->content, impl->length);
        }

        /* ֻ��������ָ��ǿյ�ʱ����ͷ�*/
        if(impl->content)
        {
            Ram_Free(impl->content);
        }

        /*ָ��*/
        impl->content = data;
        impl->capacity = cap;
    }

    return STRING_ALL_OK;
}

static const char* l_find_sub_str(const char* src, const char* sub_str, unsigned int pos_start, int num)
{
    unsigned int len = strlen(sub_str), len_src = strlen(src);
    const char* p_s_b = src;
    const char* p_d_b = NULL;
    int i = 0, j;

    /*����num*/
    if(pos_start + num > len || -1 == num)
        num = len - pos_start;

    /*�Ƚ�*/
    p_d_b = sub_str + pos_start;
    for(; i <= (int)len_src - num; i++)
    {
        for(j = 0; j < num; j++)
        {
            if(*(p_s_b + j) != *(p_d_b++))
                break;
        }

        if(j == num)
            return p_s_b;

        p_s_b++;
        p_d_b = sub_str + pos_start;
    }

    return NULL;
}

static const char* l_r_find_sub_str(const char* src, const char* sub_str, unsigned int pos_start, int num)
{
    unsigned int len = strlen(sub_str), len_src = strlen(src);
    const char* p_s_b = NULL;
    const char* p_d_b = NULL;
    int i = 0, j;

    /*����num*/
    if(pos_start + num > len || -1 == num)
        num = len - pos_start;

    /*�Ƚ�*/
    p_s_b = src + len_src - num;
    p_d_b = sub_str + pos_start;
    for(; i <= (int)len_src - num; i++)
    {
        for(j = 0; j < num; j++)
        {
            if(*(p_s_b + j) != *(p_d_b++))
                break;
        }

        if(j == num)
            return p_s_b;

        p_s_b--;
        p_d_b = sub_str + pos_start;
    }

    return NULL;
}

/**
* ��str��pos_startλ�ÿ�ʼ��num���ַ�׷�ӵ�obj��β����
*
* @param obj			c_stringʵ��ָ��
* @param str			��׷�ӵ��ַ���
* @param pos_start		����0��str�е���ʼλ�ã���С��str�ĳ���
* @param num			��str��pos_startλ������ַ�����-1��ʾʣ���ȫ��
*
* @return				STRING_ALL_OK��ʾ�ɹ��������ʾʧ��
*
* @note					��pos_start+num����str�ĳ��ȣ���num��ͬ��-1����׷��ʣ����ַ�����obj��
*
*/
static int append(struct _c_string* obj, const char* str, unsigned int pos_start, int num)
{
    c_string_impl_t* impl = (c_string_impl_t*)obj;
    int len_str = 0, ret = STRING_ALL_OK;

    /*�������*/
    if(!impl)
        return STRING_ERR_INVALID_INSTANCE;

    if(!str || num < -1)
        return STRING_ERR_PARAM;

    if(!num)
        return STRING_ALL_OK;

    len_str = strlen(str);
    /*������������⴦��*/
    if(!len_str)
        return STRING_ALL_OK;

    if(pos_start >= (unsigned int)len_str)
        return STRING_ERR_PARAM;

    /*����num*/
    if(num > len_str - (int)pos_start || -1 == num)
        num = len_str - (int)pos_start;

    /*����capacity*/
    if(STRING_ALL_OK != (ret = l_make_new_capacity(impl, num + impl->length)))
        return ret;

    /*׷��*/
    memcpy(impl->content + impl->length, str + pos_start, num);
    impl->length += num;
//	impl->content[impl->length] = 0;
    return STRING_ALL_OK;
}

/**
* ��string׷�ӵ�obj��β����
*
* @param obj			c_stringʵ��ָ��
* @param string			��׷�ӵ�c_string
*
* @return				STRING_ALL_OK��ʾ�ɹ��������ʾʧ��
*/
int append_string(struct _c_string* obj, const struct _c_string* string)
{
    c_string_impl_t* impl = (c_string_impl_t*)string;
    if(!impl)
        return STRING_ERR_INVALID_INSTANCE;

    impl->content[impl->length] = 0;
    return append(obj, impl->content, 0, -1);
}

/**
* ��str��pos_startλ�ÿ�ʼ��num���ַ���obj�����ݱȽϡ�
*
* @param obj			c_stringʵ��ָ��
* @param str			���Ƚϵ��ַ���
* @param pos_start		����0��str�е���ʼλ�ã���С��str�ĳ���
* @param num			��str��pos_startλ������ַ�����-1��ʾʣ���ȫ��
* @param b_uplower		�Ƿ����ִ�Сд��1��ʾ���֣�0��ʾ������
*
* @return				0��ʾ��ͬ��1��ʾobj>ָ�����ַ�����-1��ʾobj<ָ�����ַ����������ʾʧ�ܡ�
*
* @note					��pos_start+num����str�ĳ��ȣ���num��ͬ��-1�����Ƚ�ʣ����ַ���
						�������ִ�Сдʱ���Ὣ�����ַ�ת��Сд��ʽ�Ƚ�
*
*/
static int compare(struct _c_string* obj, const char* str, unsigned int pos_start, int num, unsigned char b_uplower)
{
    int len_str = 0;
    c_string_impl_t* impl = (c_string_impl_t*)obj;

    /*�������*/
    if(!impl)
        return STRING_ERR_INVALID_INSTANCE;

    if(!str || pos_start && pos_start >= (unsigned int)len_str || num < -1)
        return STRING_ERR_PARAM;

    if(!num)
        return 1;

    len_str = strlen(str);
    if(!len_str && !impl->length)
        return 0;

    /* added by skybuf at 090512 */
    impl->content[impl->length] = 0;

    /*����num�Ĵ�С���бȽ�*/
    if(num > len_str - (int)pos_start || -1 == num)
    {
        num = len_str - (int)pos_start;
        if(1 == b_uplower)
        {
            //impl->content[impl->length] = 0;     /* deleted by skybuf at 090512 */
            return strcmp(impl->content, str + pos_start);	/*���ڳ��ȣ������ִ�Сдʱ��ֱ����strcmp*/
        }
    }

    /*���ַ��Ƚ�*/
    {
        const char* s_c_p = impl->content;
        const char* d_c_p = str + pos_start;
        int num_d = 0;
        char s_c, d_c;

        while(*s_c_p && *d_c_p && num_d < num)
        {
            s_c = *(s_c_p++);
            d_c = *(d_c_p++);
            if(0 == b_uplower)
            {
                /*�����ִ�Сд*/
                if(s_c >= 65 && s_c <= 90)
                    s_c += 32; /*ת��Сд*/
                if(d_c >= 65 && d_c <= 90)
                    d_c += 32; /*ת��Сд*/
            }

            if(s_c > d_c)
                return 1;

            if(s_c < d_c)
                return -1;

            num_d++;
        }

        if(num_d == num)
        {
            if(*s_c_p)
                return 1;
            else if(*d_c_p)
                return -1;

            return 0;
        }

        if(*s_c_p)
            return 1;

        if(*d_c_p)
            return -1;
    }

    return 0;
}

/**
* ��string��obj�Ƚϡ�
*
* @param obj			c_stringʵ��ָ��
* @param string			���Ƚϵ�c_string
* @param b_uplower		�Ƿ����ִ�Сд��1��ʾ���֣�0��ʾ������
*
* @return				0��ʾ��ͬ��1��ʾobj>ָ�����ַ�����-1��ʾobj<ָ�����ַ����������ʾʧ�ܡ�
*
* @note					�������ִ�Сдʱ���Ὣ�����ַ�ת��Сд��ʽ�Ƚ�
*
*/
static int compare_string(struct _c_string* obj, const struct _c_string* string, unsigned char b_uplower)
{
    const c_string_impl_t* str_impl = (const c_string_impl_t*)string;
    c_string_impl_t* impl = (c_string_impl_t*)obj;
    if(!str_impl)
        return STRING_ERR_INVALID_INSTANCE;

    if(impl && !impl->length && !str_impl->length)   /*content��Ϊ��ʱ��ֱ�Ӵ����*/
        return 0;

    str_impl->content[str_impl->length] = 0; // added by skybuf at 090512
    return compare(obj, str_impl->content, 0, -1, b_uplower);
}

/**
* ��str��pos_startλ�ÿ�ʼ��num���ַ�������obj�У�obj��ԭ�����ݽ����滻��
*
* @param obj			c_stringʵ��ָ��
* @param str			���������ַ���
* @param pos_start		����0��str�е���ʼλ�ã���С��str�ĳ���
* @param num			��str��pos_startλ������ַ�����-1��ʾʣ���ȫ��
*
* @return				STRING_ALL_OK��ʾ�ɹ��������ʾʧ��
*
* @note					��pos_start+num����str�ĳ��ȣ���num��ͬ��-1��������ʣ����ַ���
*
*/
static int copy(struct _c_string* obj, const char* str, unsigned int pos_start, int num)
{
    int len_str = 0, ret = STRING_ALL_OK;
    c_string_impl_t* impl = (c_string_impl_t*)obj;

    /*�������*/
    if(!impl)
        return STRING_ERR_INVALID_INSTANCE;

    if(!str || num < -1)
        return STRING_ERR_PARAM;

    if(!num)
        return STRING_ALL_OK;

    len_str = strlen(str);
    /*������������⴦��*/
    if(!len_str)
    {
        if(impl->length)
        {
            /*strcpy(impl->content, str);*/ /*������գ���length���Ƽ���*/
            impl->length = 0;
        }
        return STRING_ALL_OK;
    }

    if(pos_start >= (unsigned int)len_str)
        return STRING_ERR_PARAM;

    /*����num*/
    if(num > len_str - (int)pos_start || -1 == num)
        num = len_str - (int)pos_start;

    /*����capacity*/
    if(STRING_ALL_OK != (ret = l_make_new_capacity(impl, num)))
        return ret;

    /*����*/
    memcpy(impl->content, str + pos_start, num);
    impl->length = num;
//	impl->content[impl->length]=0;
    return STRING_ALL_OK;
}

/**
* ��string������obj�У�obj��ԭ�����ݽ����滻��
*
* @param obj			c_stringʵ��ָ��
* @param string			��׷�ӵ�c_string
*
* @return				STRING_ALL_OK��ʾ�ɹ��������ʾʧ��
*/
static int copy_string(struct _c_string* obj, const struct _c_string* string)
{
    const c_string_impl_t* str_impl = (const c_string_impl_t*)string;
    c_string_impl_t* impl = (c_string_impl_t*)obj;
    if(!str_impl)
        return STRING_ERR_INVALID_INSTANCE;

    if(impl && impl->length && !str_impl->length)
    {
        /*memset(impl->content, 0, impl->length);*/ /*����set����length���Ƽ���*/
        impl->length = 0;
        return STRING_ALL_OK;
    }

    str_impl->content[str_impl->length] = 0; // added by skybuf at 090512
    return copy(obj, str_impl->content, 0, -1);
}

/**
* ��obj�д�pos_startλ�ÿ�ʼ��num���ַ�����
*
* @param obj			c_stringʵ��ָ��
* @param pos_start		����0��str�е���ʼλ�ã���С��str�ĳ���
* @param num			��str��pos_startλ������ַ�����-1��ʾʣ���ȫ����
*
* @return				STRING_ALL_OK��ʾ�ɹ��������ʾʧ��
*
* @note					��pos_start+num����obj���ַ����ĳ��ȣ���num��ͬ��-1��������ʣ����ַ���
*
*/
static int erase(struct _c_string* obj, unsigned int pos_start, int num)
{
    c_string_impl_t* impl = (c_string_impl_t*)obj;

    /*�������*/
    if(!impl)
        return STRING_ERR_INVALID_INSTANCE;

    if(num < -1 || pos_start && pos_start >= impl->length)
        return STRING_ERR_PARAM;

    if(!num || !impl->length)
        return STRING_ALL_OK;

    if(0 == pos_start && (num >= (int)(impl->length - pos_start) || num == -1))/*�൱��clear*/
    {
        clear(obj);
        return STRING_ALL_OK;
    }

    if(num > (int)(impl->length - pos_start))
        impl->length = pos_start + 1; /*����ĩβ�ģ��������ȼ���*/
    else
    {
        char* s_c_p = impl->content + pos_start;
        char* d_c_p = impl->content + pos_start + num;

        while(*d_c_p)
        {
            *(s_c_p++) = *(d_c_p++);
        }

        impl->length -= num; /*����length*/
    }

    return STRING_ALL_OK;
}

/**
* ��obj�д�pos_startλ�ÿ�ʼ��num���ַ��滻��str
*
* @param obj			c_stringʵ��ָ��
* @param pos_start		����0��obj���ַ�������ʼλ�ã���С��obj�ĳ���
* @param num			��obj���ַ�����pos_startλ������ַ�����-1��ʾʣ���ȫ��
* @param str			���滻���ַ���
*
* @return				STRING_ALL_OK��ʾ�ɹ��������ʾʧ��
*
* @note					��pos_start+num����obj���ַ����ĳ��ȣ���num��ͬ��-1�����滻ʣ����ַ���
*
*/
static int replace(struct _c_string* obj, unsigned int pos_start, int num, const char* str)
{
    int len_str = 0;
    int ret = STRING_ALL_OK;
    c_string_impl_t* impl = (c_string_impl_t*)obj;

    /*�������*/
    if(!impl)
        return STRING_ERR_INVALID_INSTANCE;

    if(!str || num < -1)
        return STRING_ERR_PARAM;

    if(!num)
        return STRING_ALL_OK;

    len_str = strlen(str);
    /*������������⴦��*/
    if(!len_str)
    {
        /*�滻�ɿ��ַ�������ͬ�ڲ���*/
        return erase(obj, pos_start, num);
    }

    if(pos_start && pos_start >= impl->length)
        return STRING_ERR_PARAM;

    /*����num*/
    if(num > (int)(impl->length - pos_start) || -1 == num)
        num = impl->length - pos_start;

    /*�����µ�����*/
    if(STRING_ALL_OK != (ret = l_make_new_capacity(impl, impl->length - num + len_str)))
        return ret;

    /*����滻ʣ���ȫ������ֱ�ӿ������ɣ��������*/
    if(num == (int)(impl->length - pos_start))
        strcpy(impl->content + pos_start, str);
    else
    {
        int n_time = len_str - num;/*ע�⣬n_time����Ϊ��ֵ*/
        char* p_start;
        impl->content[impl->length] = 0;  //added by skybuf at 090512

        if(n_time > 0)
        {
            p_start = impl->content + impl->length - 1;
            while(p_start != impl->content + pos_start + num - 1)
            {
                *(p_start + n_time) = *p_start;
                p_start--;
            }
        }
        else
        {
            p_start = impl->content + pos_start + num;
            while(*p_start)
            {
                *(p_start - n_time) = *p_start;
                p_start++;
            }

            *(p_start - n_time) = 0;
        }

        memcpy(impl->content + pos_start, str, len_str);
    }

    impl->length = impl->length - num + len_str;
//	impl->content[impl->length] = 0;
    return STRING_ALL_OK;
}

/**
* ��obj�е�str_src�ַ����滻��str_dst
*
* @param obj			c_stringʵ��ָ��
* @param str_src		���滻���ַ���
* @param str_dst		���滻���ַ���
*
* @return				STRING_ALL_OK��ʾ�ɹ��������ʾʧ��
*/
static int replace_str(struct _c_string* obj, const char* str_src, const char* str_dst)
{
    c_string_impl_t* impl = (c_string_impl_t*)obj;
    char* p_s = NULL;
    const char* p_s_sub = NULL;
    int ret = STRING_ALL_OK;

    /*�������*/
    if(!impl)
        return STRING_ERR_INVALID_INSTANCE;

    if(!str_src || !str_dst)
        return STRING_ERR_PARAM;

    if(!strlen(str_src) || !impl->length)
        return STRING_ALL_OK;

    /*����ÿһ��sub str���滻֮*/
    p_s = impl->content;
    p_s_sub = l_find_sub_str(p_s, str_src, 0, -1);
    while(p_s_sub)
    {
        ret = replace(obj, p_s_sub - impl->content, strlen(str_src), str_dst);
        if(STRING_ALL_OK != ret)
            return ret;

        p_s_sub += strlen(str_dst);
        p_s_sub = l_find_sub_str(p_s_sub, str_src, 0, -1); /*�滻�ɹ����������滻������ݲ���*/
    }

    return 0;
}

/**
* ��str��pos_start_strλ�ÿ�ʼ��num_str���ַ����뵽obj��pos_start_objλ��
*
* @param obj			c_stringʵ��ָ��
* @param pos_start_obj	obj�еĲ���λ�ã�����0
* @param str			��������ַ���
* @param pos_start_str	����0��str�е���ʼλ�ã���С��str�ĳ���
* @param num_str		��str��pos_start_strλ������ַ�����-1��ʾʣ���ȫ��
*
* @return				STRING_ALL_OK��ʾ�ɹ��������ʾʧ��
*
* @note					��pos_start_str+num_str����obj���ַ����ĳ��ȣ���num_str��ͬ��-1��������ʣ����ַ���
*
*/
static int insert(struct _c_string* obj, unsigned int pos_start_obj, const char* str, unsigned int pos_start_str, int num_str)
{
    int len_str = 0;
    int ret = STRING_ALL_OK;
    c_string_impl_t* impl = (c_string_impl_t*)obj;
    char* p_e = NULL;

    /*�������*/
    if(!impl)
        return STRING_ERR_INVALID_INSTANCE;

    if(!str || pos_start_obj && pos_start_obj >= impl->length || pos_start_str && pos_start_str >= (int)strlen(str) || num_str < -1)
        return STRING_ERR_PARAM;

    if(!num_str)
        return STRING_ALL_OK;

    /*����num_str*/
    len_str = strlen(str);
    if(-1 == num_str || num_str + (int)pos_start_str > len_str)
        num_str = len_str - pos_start_str;

    /*��������capacity*/
    ret = l_make_new_capacity(impl, impl->length + num_str);
    if(STRING_ALL_OK != ret)
        return ret;

    /*�ƶ��ַ���*/
    p_e = impl->content + impl->length - 1;
    while(p_e != impl->content + pos_start_obj)
    {
        *(p_e + num_str) = *p_e;
        p_e--;
    }
    *(p_e + num_str) = *p_e; /*����pos_start_obj��*/

    /*����*/
    memcpy(impl->content + pos_start_obj, str, num_str);
    impl->length += num_str;
//	impl->content[impl->length] = 0;
    return 0;
}

/**
* ��string���뵽obj��pos_start_objλ��
*
* @param obj			c_stringʵ��ָ��
* @param pos_start_obj	obj�еĲ���λ�ã�����0
* @param string			�������c_string
*
* @return				STRING_ALL_OK��ʾ�ɹ��������ʾʧ��
*/
static int insert_string(struct _c_string* obj, unsigned int pos_start_obj, const struct _c_string* string)
{
    const c_string_impl_t* str_impl = (const c_string_impl_t*)string;
    c_string_impl_t* impl = (c_string_impl_t*)obj;
    if(!str_impl)
        return STRING_ERR_INVALID_INSTANCE;

    if(impl && !str_impl->length)
        return STRING_ALL_OK;

    impl->content[impl->length] = 0;//added by skybuf at 090512
    return insert(obj, pos_start_obj, impl->content, 0, -1);
}

/**
* ��num_ch��ch���뵽obj��pos_start_objλ��
*
* @param obj			c_stringʵ��ָ��
* @param pos_start_obj	obj�еĲ���λ�ã�����0
* @param ch				��������ַ�
* @param num_ch			��������ַ���
*
* @return				STRING_ALL_OK��ʾ�ɹ��������ʾʧ��
*/
static int insert_ch(struct _c_string* obj, unsigned int pos_start_obj, char ch, unsigned int num_ch)
{
    int ret = STRING_ALL_OK;
    c_string_impl_t* impl = (c_string_impl_t*)obj;
    char* p_e = NULL;

    /*�������*/
    if(!impl)
        return STRING_ERR_INVALID_INSTANCE;

    if(pos_start_obj >= impl->length)
        return STRING_ERR_PARAM;

    if(!num_ch)
        return STRING_ALL_OK;

    /*��������capacity*/
    ret = l_make_new_capacity(impl, impl->length + num_ch);
    if(STRING_ALL_OK != ret)
        return ret;

    /*�ƶ��ַ���*/
    p_e = impl->content + impl->length - 1;
    while(p_e != impl->content + pos_start_obj)
    {
        *(p_e + num_ch) = *p_e;
        p_e--;
    }
    *(p_e + num_ch) = *p_e; /*����pos_start_obj��*/

    impl->length += num_ch;
    /*����*/
    while(num_ch--)
    {
        *(p_e++) = ch;
    }
//	impl->content[impl->length] = 0;
    return 0;
}

/**
* ��obj��˳��������ַ������״γ���λ�ã����ַ�����str��pos_start��ʼ��num���ַ����
*
* @param obj			c_stringʵ��ָ��
* @param str			�����ҵ��ַ���
* @param pos_start		����0��str�е���ʼλ��
* @param num			��pos_start��ʼ���ַ�����-1��ʾʣ���ȫ��
*
* @return				���ַ�������ʼλ�ã�-1��ʾû���ҵ���<-1��ʾʧ��
*
* @note					��pos_start+num����str�ĳ��ȣ���num��ͬ��-1��������ʣ����ַ���
*
*/
static int find_first_of(struct _c_string* obj, const char* str, unsigned int pos_start, int num)
{
    c_string_impl_t* impl = (c_string_impl_t*)obj;
    const char* p_sub_str = NULL;

    if(!obj)
        return STRING_ERR_INVALID_INSTANCE;

    if(!str || num < -1)
        return STRING_ERR_PARAM;

    if(!impl->length || !strlen(str))
        return -1;

    if(pos_start >= strlen(str))  /*������Ƿ�Ϊ�պ����ж��Ƿ�Խ��*/
        return STRING_ERR_PARAM;

    impl->content[impl->length] = 0;  //added by skybuf at 090512
    p_sub_str = l_find_sub_str(impl->content, str, pos_start, num);

    return (p_sub_str ? (p_sub_str - impl->content) : -1);
}

/**
* ��obj��˳��������ַ���string���״γ���λ��
*
* @param obj			c_stringʵ��ָ��
* @param string			�����ҵ����ַ���
*
* @return				���ַ�������ʼλ�ã�-1��ʾû���ҵ���<-1��ʾʧ��
*/
static int find_first_string_of(struct _c_string* obj, struct _c_string* string)
{
    const c_string_impl_t* str_impl = (const c_string_impl_t*)string;
    c_string_impl_t* impl = (c_string_impl_t*)obj;
    if(!str_impl)
        return STRING_ERR_INVALID_INSTANCE;

    if(impl && !str_impl->length)
        return -1;

    impl->content[impl->length] = 0;  //added by skybuf at 090512
    return find_first_of(obj, ((c_string_impl_t*)string)->content, 0, -1);
}

/**
* ��obj��˳������ַ�ch�״γ��ֵ�λ��
*
* @param obj			c_stringʵ��ָ��
* @param ch				�����ҵ��ַ�
*
* @return				�ַ�����ʼλ�ã�-1��ʾû���ҵ���<-1��ʾʧ��
*/
static int find_first_ch_of(struct _c_string* obj, char ch)
{
    char temp[2];
    temp[0] = ch;
    temp[1] = 0;

    return find_first_of(obj, temp, 0, -1);
}

/* ��obj������������ַ������״γ���λ�ã����ַ�����str��pos_start��ʼ��num���ַ����
*
* @param obj			c_stringʵ��ָ��
* @param str			�����ҵ��ַ���
* @param pos_start		����0��str�е���ʼλ��
* @param num			��pos_start��ʼ���ַ�����-1��ʾʣ���ȫ��
*
* @return				���ַ���������ʼλ�ã�-1��ʾû���ҵ���<-1��ʾʧ��
*
* @note					��pos_start+num����str�ĳ��ȣ���num��ͬ��-1��������ʣ����ַ���
*
*/
static int r_find_first_of(struct _c_string* obj, const char* str, unsigned int pos_start, int num)
{
    c_string_impl_t* impl = (c_string_impl_t*)obj;
    const char* p_sub_str = NULL;

    if(!impl)
        return STRING_ERR_INVALID_INSTANCE;

    if(!str || num < -1)
        return STRING_ERR_PARAM;

    if(!impl->length || !strlen(str))
        return -1;

    if(pos_start >= strlen(str))   /*������Ƿ�Ϊ�պ����ж��Ƿ�Խ��*/
        return STRING_ERR_PARAM;

    impl->content[impl->length] = 0;  //added by skybuf at 090512
    p_sub_str = l_r_find_sub_str(impl->content, str, pos_start, num);

    return (p_sub_str ? (p_sub_str - impl->content) : -1);
}

/**
* ��obj������������ַ���string���״γ���λ��
*
* @param obj			c_stringʵ��ָ��
* @param string			�����ҵ����ַ���
*
* @return				���ַ���������ʼλ�ã�-1��ʾû���ҵ���<-1��ʾʧ��
*/
static int r_find_first_string_of(struct _c_string* obj, struct _c_string* string)
{
    const c_string_impl_t* str_impl = (const c_string_impl_t*)string;
    c_string_impl_t* impl = (c_string_impl_t*)obj;
    if(!str_impl)
        return STRING_ERR_INVALID_INSTANCE;

    if(impl && !str_impl->length)
        return -1;

    impl->content[impl->length] = 0;  //added by skybuf at 090512
    return r_find_first_of(obj, ((c_string_impl_t*)string)->content, 0, -1);
}

/**
* ��obj��������ַ�ch�״γ��ֵ�λ��
*
* @param obj			c_stringʵ��ָ��
* @param ch				�����ҵ��ַ�
*
* @return				�ַ�������ʼλ�ã�-1��ʾû���ҵ���<-1��ʾʧ��
*/
static int r_find_first_ch_of(struct _c_string* obj, char ch)
{
    char temp[2];
    temp[0] = ch;
    temp[1] = 0;

    return r_find_first_of(obj, temp, 0, -1);
}

/**
* �õ�obj��λ��indexλ�õ��ַ�
*
* @param obj			c_stringʵ��ָ��
* @param index			����������0
*
* @return				�ַ�������0��ʾ����
*
*/
static char at(struct _c_string* obj, unsigned int index)
{
    c_string_impl_t* impl = (c_string_impl_t*)obj;

    if(!impl)
        return 0;

    if(index >= impl->length || !impl->length)
        return 0;

    return impl->content[index];
}
/**
* �õ�obj�е��ַ���
*
* @param obj			c_stringʵ��ָ��
*
* @return				�ַ����� NULL��ʾʧ��
*/
static const char* str(struct _c_string* obj)
{
    c_string_impl_t* impl = (c_string_impl_t*)obj;

    if(!impl)
        return NULL;

    /*��ȡ֮ǰ����*/
    if(impl->content)
        impl->content[impl->length] = 0;

    return impl->content;
}

/**
* ���obj�е�����
*
* @param obj			c_stringʵ��ָ��
*
* @return				��
*/
static void clear(struct _c_string* obj)
{
    c_string_impl_t* impl = (c_string_impl_t*)obj;

    if(impl && impl->length)
    {
        memset(impl->content, 0, impl->length);
        impl->length = 0;
    }
}

/**
* ����obj�Ƿ�Ϊ��
*
* @param obj			c_stringʵ��ָ��
*
* @return				1��ʾΪ�գ�0��ʾ�����ݣ������ʾʧ��
*/
static unsigned char empty(struct _c_string* obj)
{
    c_string_impl_t* impl = (c_string_impl_t*)obj;
    if(!impl)
        return 1;

    return impl->length ? 0 : 1;
}

/**
* �õ�obj���ַ����ĳ���
*
* @param obj			c_stringʵ��ָ��
*
* @return				���ȣ� <0��ʾʧ��
*/
static int length(struct _c_string* obj)
{
    c_string_impl_t* impl = (c_string_impl_t*)obj;


    if(!impl)
        return STRING_ERR_INVALID_INSTANCE;

    return impl->length;

}

/**
* �õ�obj�д�pos_start��ʼ��num���ַ���ɵ����ַ���
*
* @param obj			c_stringʵ��ָ��
* @param pos_start		����0��obj�����ַ�������ʼλ��
* @param num			��pos_start��ʼ���ַ�����-1��ʾʣ���ȫ��
*
* @return				c_stringʵ��ָ�룬�����ַ�������.NULL��ʾʧ��
*
* @note					���ص�ʵ��ָ�������c_string_release���ա�
*
*						��pos_start+num����obj�ĳ��ȣ���num��ͬ��-1�����õ�ʣ������ַ���
*
*/
static struct _c_string* substr(struct _c_string* obj, unsigned int pos_start, int num)
{
    c_string_impl_t* impl = (c_string_impl_t*)obj;
    if(!impl)
        return NULL;

    if(pos_start && pos_start >= impl->length)
        return NULL;

    return c_string_create(impl->content, pos_start, num);
}

/**
* ��obj��string�е����ݽ���
*
* @param obj			c_stringʵ��ָ��
* @param string 		c_stringʵ��ָ��
*
* @return				��
*/
static void swap(struct _c_string* obj, struct _c_string* string)
{
    c_string_impl_t temp;
    c_string_impl_t* impl_obj = (c_string_impl_t*)obj;
    c_string_impl_t* impl_string = (c_string_impl_t*)string;

    if(!impl_obj || !impl_string)
        return;

    memcpy(&temp,		impl_obj,		sizeof(c_string_impl_t));
    memcpy(impl_obj,	impl_string,	sizeof(c_string_impl_t));
    memcpy(impl_string,	&temp,			sizeof(c_string_impl_t));
}

/**
* ��obj�е��ַ���ת���ɴ�д
*
* @param obj			c_stringʵ��ָ��
*
* @return				��
*/
static void to_upper(struct _c_string* obj)
{
    unsigned int i = 0;
    c_string_impl_t* impl = (c_string_impl_t*)obj;
    if(!impl)
        return;

    for(; i < impl->length; i++)
    {
        if(impl->content[i] >= 97 && impl->content[i] <= 122)
            impl->content[i] -= 32;
    }
}
/**
* ��obj�е��ַ���ת����Сд
*
* @param obj			c_stringʵ��ָ��
*
* @return				��
*/
static void to_lower(struct _c_string* obj)
{
    unsigned int i = 0;
    c_string_impl_t* impl = (c_string_impl_t*)obj;
    if(impl)
    {
        for(; i < impl->length; i++)
        {
            if(impl->content[i] >= 65 && impl->content[i] <= 90)
                impl->content[i] += 32;
        }
    }
}

/**
* string������
*/
typedef struct _list_objs
{
    c_string_impl_t* data;			/*string����*/
    unsigned char b_valid;			/*�˶����Ƿ���ʹ��*/
    struct _list_objs* next;		/*��һ����*/
} c_string_list_objs;

/**
* string������������
*/
typedef struct
{
    c_string_list_objs* start;		/*��������ͷ���*/
    c_string_list_objs* end;		/*��������β���*/
    unsigned int num;				/*�������Ŀ*/
} c_string_list_objs_mgr;

static c_string_list_objs_mgr* l_mgr;
/**
* c_string��������߳�ʼ������
*

* @note					ͨ����ÿ��string���ǱȽ϶̵��ַ��������ÿ��string���ǴӶ��Ϸ����µ��ڴ棬
						��Ч�ʵ��£��һ����������ڴ���Ƭ��

						����c_string�����������Ϊ�˽���Щ���õ�c_string�����ݴ����������´���Ҫ
						����c_stringʱ������ʹ����ǰ���õ�c_string���������ͱ��������������⡣


						�ڳ��ε���c_string_createʱ������ص��ô˺�����������ᴴ��ʧ�ܡ�
*/
int c_string_mgr_init(void)
{
    if(l_mgr)
        return STRING_ALL_OK;

    l_mgr = (c_string_list_objs_mgr*)Ram_Alloc(0, sizeof(c_string_list_objs_mgr));
    if(!l_mgr)
        return STRING_ERR_NO_MEMORY;

    memset(l_mgr, 0, sizeof(c_string_list_objs_mgr));
    return STRING_ALL_OK;
}

/**
* c_string��������߻���
*

* @note					ͨ����ÿ��string���ǱȽ϶̵��ַ��������ÿ��string���ǴӶ��Ϸ����µ��ڴ棬
						��Ч�ʵ��£��һ����������ڴ���Ƭ��

						����c_string�����������Ϊ�˽���Щ���õ�c_string�����ݴ����������´���Ҫ
						����c_stringʱ������ʹ����ǰ���õ�c_string���������ͱ��������������⡣


						�ڳ����˳�ʱ������ص��ô˺�������������ڴ�й¶��
*/
void c_string_mgr_release(void)
{
    if(l_mgr)
    {
        c_string_list_objs* temp = NULL;
        while(l_mgr->start)						/*releaseÿ��c_string����*/
        {

            /*NOTE�� �˴����Զ�λ����Щobj���ڱ�ʹ���У����ռ�֮���������*/

            Ram_Free(l_mgr->start->data->content);
            l_mgr->start->data->content = NULL;
            Ram_Free(l_mgr->start->data);
            l_mgr->start->data = NULL;
            temp = l_mgr->start;
            l_mgr->start = l_mgr->start->next;
            Ram_Free(temp);
            temp = NULL;
        }

        Ram_Free(l_mgr);
        l_mgr = NULL;
    }
}


/**
* c_stringʵ��ָ�봴���ӿڣ���str�д�pos_startλ�ÿ�ʼ��num���ַ���Ϊʵ����ֵ
*
* @param str			����Ϊc_stringʵ����ֵ���ַ�����
* @param pos_start		str�е���ʼλ�ã�����0
* @param num			��pos_startλ�ÿ�ʼ���ַ�����-1��ʾʣ���ȫ��
*
* @return				c_stringʵ��ָ��, NULL��ʾʧ��
*
* @note					str����ΪNULL����ʱc_stringʵ����ֵΪ���ַ�����
						pos_start��numֻ����str��Ϊ��ʱ����Ч��

						��pos_start+num����str�ĳ��ȣ���num��ͬ��-1
*/
c_string* c_string_create(const char* str_src, unsigned int pos_start, int num)
{
    c_string_impl_t* obj = NULL;
    c_string_list_objs* temp = NULL;
    if(!l_mgr)
        return NULL;

    if(str_src && (pos_start && pos_start >= strlen(str_src) || num < -1))
        return NULL;
//      kal_prompt_trace(TRACE_INFO,"@@@   c_string_create()   1  "    );

    /*���ȴ�list����*/
    temp = l_mgr->start;
    while(temp)
    {
        if(!temp->b_valid)
        {
            /*��Ч�����ʾ�˶����Ѿ�����ʹ�ã�������Ϊ�µ�*/
//      kal_prompt_trace(TRACE_INFO,"@@@   c_string_create()   2  "    );

            temp->b_valid = 1; /*��Ϊ��Ч*/

            /*�趨����*/
            if(str_src)
                copy((c_string*)temp->data, str_src, pos_start, num);

            return (c_string*)temp->data;
        }

        temp = temp->next;
    }

    /*list��û�����������ģ�����*/
    obj = (c_string_impl_t*)Ram_Alloc(0, sizeof(c_string_impl_t));
    if(!obj)
        return NULL;
    /*���ɽڵ�*/
    temp = (c_string_list_objs*)Ram_Alloc(0, sizeof(c_string_list_objs));
    if(!temp)
    {
        Ram_Free(obj);
        obj = NULL;
        return NULL;
    }
    memset(obj,		0, sizeof(c_string_impl_t));

    /*�趨����*/
    if(str_src)
        copy((c_string*)obj, str_src, pos_start, num);

    /*���ýӿ�*/
    obj->base.append					= append;
    obj->base.append_string				= append_string;
    obj->base.at						= at;
    obj->base.clear						= clear;
    obj->base.compare					= compare;
    obj->base.compare_string			= compare_string;
    obj->base.copy						= copy;
    obj->base.copy_string				= copy_string;
    obj->base.empty						= empty;
    obj->base.erase						= erase;
    obj->base.find_first_ch_of			= find_first_ch_of;
    obj->base.find_first_of				= find_first_of;
    obj->base.find_first_string_of		= find_first_string_of;
    obj->base.insert					= insert;
    obj->base.insert_ch					= insert_ch;
    obj->base.insert_string				= insert_string;
    obj->base.r_find_first_ch_of		= r_find_first_ch_of;
    obj->base.r_find_first_of			= r_find_first_of;
    obj->base.r_find_first_string_of	= r_find_first_string_of;
    obj->base.replace					= replace;
    obj->base.replace_str				= replace_str;
    obj->base.str						= str;
    obj->base.substr					= substr;
    obj->base.swap						= swap;
    obj->base.length					= length;
    obj->base.to_lower					= to_lower;
    obj->base.to_upper					= to_upper;


    /*���ýڵ�*/
    temp->b_valid	= 1;
    temp->data		= obj;
    temp->next		= NULL;

    /*��������*/
    l_mgr->num++;
    if(!l_mgr->start)
        l_mgr->start = l_mgr->end = temp;
    else
    {
        l_mgr->end->next	= temp;
        l_mgr->end			= temp;
    }

    return (c_string*)obj;
}

/**
* c_stringʵ�����սӿ�
*
* @param obj			c_stringʵ��ָ�룬��c_string_create����substr����
*
* @return				��
*/
void c_string_release(c_string* obj)
{
    c_string_list_objs* temp = NULL;
    c_string_impl_t* impl = (c_string_impl_t*)obj;

//   kal_prompt_trace(TRACE_INFO,"@@@   c_string_release()   1  "    );

    if(impl)
    {
        /*���Ҵ�obj�Ƿ����ж�Ӧ�ڵ�*/
//        kal_prompt_trace(TRACE_INFO,"@@@   c_string_release()   2  "    );

        temp = l_mgr->start;
        while(temp)
        {
//            kal_prompt_trace(TRACE_INFO,"@@@   c_string_release()   3  "    );

            if(temp->data == impl)
            {
                /*�ڽڵ���*/
//            kal_prompt_trace(TRACE_INFO,"@@@   c_string_release()   4  "    );

                /*���ԭ��������*/
                if(temp->data->length)
                {
//                                kal_prompt_trace(TRACE_INFO,"@@@   c_string_release()   5  "    );
                    memset(temp->data->content, 0, temp->data->length);
                    temp->data->length = 0;
                }

                /*��Ϊ��Ч*/
                temp->b_valid = 0;
                return;
            }

            temp = temp->next;
        }
//		            kal_prompt_trace(TRACE_INFO,"@@@   c_string_release()   6  "    );

        /*���ڽڵ��У���Ȼ��ɷ�����ʹ�ã������Ǵ����˱ȽϺ�*/
        Ram_Free(impl->content);
        impl->content = NULL;
        Ram_Free(impl);
        impl = NULL;
    }
}

#endif /*__REMOVE_LOG_TO_FILE__*/

