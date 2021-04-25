
#include "c_vector.h"
#include <stdlib.h>
#include <string.h>

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

#define DEFAULT_CAPACITY_SIZE_ADD		512/*32*/
#define DEFAULT_CAPACITY_LIMIT			512
#define DEFAULT_CAPACITY_NEW_MUL		1.5
#define DEFAULT_CAPACITY_RELEASE_LIMIT	1024

typedef struct
{
    c_vector base;

    unsigned long* content;
    unsigned int length;
    unsigned int capacity;
} c_vector_impl_t;

static int copy_vec(struct _c_vector* obj, const struct _c_vector* vec);
static void* at(struct _c_vector* obj, unsigned int index);
static void* back(struct _c_vector* obj);
static void* front(struct _c_vector* obj);
static void clear(struct _c_vector* obj);
static unsigned char empty(struct _c_vector* obj);
static int erase(struct _c_vector* obj, unsigned int pos_start, int num);
static int insert(struct _c_vector* obj, unsigned int index, void* data);
static int insert_vec(struct _c_vector* obj, unsigned int index, const struct _c_vector* vec);
static int size(struct _c_vector* obj);
static int push_back(struct _c_vector* obj, void* data);
static int swap(struct _c_vector* obj, struct _c_vector* vec);
static int sort(struct _c_vector* obj, fun_sort_t fun_sort);
static int for_each(struct _c_vector* obj, fun_each_t fun_each);


/**
* �����µ�������С
*
* @param impl			c_vector_impl_tʵ��ָ��
* @param capacity_new	�µ�������С
*
* @return				VECTOR_ALL_OK��ʾ�ɹ��������ʾʧ��
*
* @note					�������ԣ� ���capacity_new��С��Ŀǰ��capacity��
						�����յ�������СΪcapacity_new��DEFAULT_CAPACITY_NEW_MUL����

						����DEFAULT_CAPACITY_NEW_MUL��֮�󣬴�С��Խ��DEFAULT_CAPACITY_LIMIT��
						�����յ�����Ϊcapacity_new+DEFAULT_CAPACITY_SIZE_ADD
*
*/
static int l_make_new_capacity(c_vector_impl_t* impl, unsigned int capacity_new)
{
    int cap = (int)(capacity_new * DEFAULT_CAPACITY_NEW_MUL);	/*������Ϊ��Ҫ��������DEFAULT_CAPACITY_NEW_MUL��*/
    if(capacity_new <= impl->capacity)
        return VECTOR_ALL_OK;

    /*�����µ�����*/
    if(cap > DEFAULT_CAPACITY_LIMIT)  /*��չ��DEFAULT_CAPACITY_NEW_MUL�������DEFAULT_CAPACITY_LIMIT���������DEFAULT_CAPACITY_SIZE_ADD�ֽ�*/
        cap = capacity_new + DEFAULT_CAPACITY_SIZE_ADD;

    {
        /*����������������*/
        unsigned long* data = (unsigned long*)Ram_Alloc(0, cap * sizeof(unsigned long));
        if(!data)
            return VECTOR_ERR_NO_MEMORY;

        if(impl->length)
        {
            /*����ԭ������*/
            memcpy(data, impl->content, sizeof(unsigned long) * impl->length);
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

    return VECTOR_ALL_OK;
}


/**
* ��vec�����ݿ�����obj�У�obj��ԭ�����ݽ�������
*
* @param obj			c_vectorʵ��ָ��
* @param vec			��������c_vectorʵ��ָ��
*
* @return				VECTOR_ALL_OK��ʾ�ɹ��������ʾʧ��
*
*/
static int copy_vec(struct _c_vector* obj, const struct _c_vector* vec)
{
    int ret = VECTOR_ALL_OK;
    c_vector_impl_t* impl = (c_vector_impl_t*)obj;
    c_vector_impl_t* impl_vec = (c_vector_impl_t*)vec;
    if(!impl || !impl_vec)
        return VECTOR_ERR_INVALID_INSTANCE;

    if(!impl_vec->length)
    {

        impl->length = 0;
        return VECTOR_ALL_OK;
    }

    if(VECTOR_ALL_OK != (ret = l_make_new_capacity(impl, impl_vec->length)))
        return ret;

    memcpy(impl->content, impl_vec->content, sizeof(unsigned long)*impl_vec->length);
    impl->length = impl_vec->length;
    return VECTOR_ALL_OK;
}

/**
* ��obj��λ��index����Ԫ�ء�index����0
*
* @param obj			c_vectorʵ��ָ��
* @param index			�±�
*
* @return				��Ӧ��Ԫ�أ�NULL��ʾʧ��
*
*/
static void* at(struct _c_vector* obj, unsigned int index)
{
    c_vector_impl_t* impl = (c_vector_impl_t*)obj;
    if(!impl || index >= impl->length)
        return NULL;

    return (void*)impl->content[index];
}

/**
* ȡobj�е����һ��Ԫ��
*
* @param obj			c_vectorʵ��ָ��
*
* @return				��Ӧ��Ԫ�أ�NULL��ʾʧ��
*
*/
static void* back(struct _c_vector* obj)
{
    c_vector_impl_t* impl = (c_vector_impl_t*)obj;
    if(!impl || !impl->length)
        return NULL;

    return (void*)impl->content[impl->length - 1];
}
/**
* ȡobj�еĵ�һ��Ԫ��
*
* @param obj			c_vectorʵ��ָ��
*
* @return				��Ӧ��Ԫ�أ�NULL��ʾʧ��
*
*/
static void* front(struct _c_vector* obj)
{
    c_vector_impl_t* impl = (c_vector_impl_t*)obj;
    if(!impl || !impl->length)
        return NULL;

    return (void*)impl->content[0];
}

/**
* ���obj
*
* @param obj			c_vectorʵ��ָ��
*
* @return				��
*
*/
static void clear(struct _c_vector* obj)
{
    c_vector_impl_t* impl = (c_vector_impl_t*)obj;
    if(!impl || !impl->length)
        return;

    impl->length = 0;
    return;
}
/**
* �ж�obj�Ƿ�Ϊ��
*
* @param obj			c_vectorʵ��ָ��
*
* @return				0��ʾ��Ϊ�գ�1��ʾΪ��
*
*/
static unsigned char empty(struct _c_vector* obj)
{
    c_vector_impl_t* impl = (c_vector_impl_t*)obj;
    if(!impl)
        return 1;

    return impl->length ? 0 : 1;
}

/**
* ����obj�д�pos_start��ʼ��num��Ԫ��
*
* @param obj			c_vectorʵ��ָ��
* @param pos_start		obj�е��±꣬����0
* @param obj			��������Ԫ�ص���Ŀ����=-1�����ʾ������pos_start��ʼ��ʣ��Ԫ��
*
* @return				VECTOR_ALL_OK��ʾ�ɹ�������Ϊʧ��
*
* @note					����num+pos_start>obj��size����num��ͬ��-1
*
*/
static int erase(struct _c_vector* obj, unsigned int pos_start, int num)
{
    c_vector_impl_t* impl = (c_vector_impl_t*)obj;
    if(!impl)
        return VECTOR_ERR_INVALID_INSTANCE;

    if(pos_start && pos_start >= impl->length || num < -1)
        return VECTOR_ERR_PARAM;

    if(!impl->length || 0 == num)
        return VECTOR_ALL_OK;

    /*����num*/
    if(pos_start + num > impl->length || -1 == num)
        num = impl->length - pos_start;


    if(pos_start + num < impl->length)
    {
        /*��λ*/
        unsigned long* p_s = impl->content + pos_start;
        int num_temp = impl->length - pos_start;
        while(num_temp)
        {
            *p_s = *(p_s + num);
            p_s++;
            num_temp--;
        }
    }
    impl->length -= num;

    return VECTOR_ALL_OK;
}

/**
* ��data���뵽obj��index��
*
* @param obj			c_vectorʵ��ָ��
* @param index			obj�е��±꣬����0
* @param data			�������Ԫ��
*
* @return				VECTOR_ALL_OK��ʾ�ɹ�������Ϊʧ��
*
*/
static int insert(struct _c_vector* obj, unsigned int index, void* data)
{
    int ret = VECTOR_ALL_OK;
    unsigned int pos = 0;
    c_vector_impl_t* impl = (c_vector_impl_t*)obj;
    if(!impl)
        return VECTOR_ERR_INVALID_INSTANCE;

    if(!data || index >= impl->length)
        return VECTOR_ERR_PARAM;

    if(VECTOR_ALL_OK != (ret = l_make_new_capacity(impl, impl->length + 1)))
        return ret;

    for(pos = impl->length; pos > index; pos--)
    {
        impl->content[pos] = impl->content[pos - 1];
    }
    impl->content[index] = (unsigned long)data;
    impl->length += 1;
    return VECTOR_ALL_OK;
}

/**
* ��vec���뵽obj��index��
*
* @param obj			c_vectorʵ��ָ��
* @param index			obj�е��±꣬����0
* @param vec			�������c_vectorʵ��ָ��
*
* @return				VECTOR_ALL_OK��ʾ�ɹ�������Ϊʧ��
*
*/
static int insert_vec(struct _c_vector* obj, unsigned int index, const struct _c_vector* vec)
{
    int ret = VECTOR_ALL_OK;
    unsigned int pos = 0;
    c_vector_impl_t* impl = (c_vector_impl_t*)obj;
    c_vector_impl_t* impl_vec = (c_vector_impl_t*)vec;
    if(!impl || !impl_vec)
        return VECTOR_ERR_INVALID_INSTANCE;

    if(index >= impl->length)
        return VECTOR_ERR_PARAM;

    if(VECTOR_ALL_OK != (ret = l_make_new_capacity(impl, impl->length + impl_vec->length)))
        return ret;

    /*��λ*/
    if(impl_vec->length > impl->length - index)
    {
        /*û�н��棬ֱ����memcpy*/
        memcpy(impl->content + index + impl_vec->length, impl->content + index, sizeof(unsigned long) * (impl->length - index));
    }
    else
    {
        for(pos = impl->length - 1; pos >= index; pos--)
        {
            impl->content[pos + impl_vec->length] = impl->content[pos];
        }
    }

    memcpy(impl->content + index, impl_vec->content, impl_vec->length * sizeof(unsigned long));
    impl->length += impl_vec->length;
    return VECTOR_ALL_OK;
}

/**
* ��data׷�ӵ�obj��
*
* @param obj			c_vectorʵ��ָ��
* @param data			��׷�ӵ�Ԫ��
*
* @return				VECTOR_ALL_OK��ʾ�ɹ�������Ϊʧ��
*
*/
static int push_back(struct _c_vector* obj, void* data)
{
    int ret = VECTOR_ALL_OK;
    c_vector_impl_t* impl = (c_vector_impl_t*)obj;
    if(!impl)
        return VECTOR_ERR_INVALID_INSTANCE;

    if(!data)
        return VECTOR_ERR_PARAM;

    if(VECTOR_ALL_OK != (ret = l_make_new_capacity(impl, impl->length + 1)))
        return ret;

    impl->content[impl->length] = (unsigned long)data;
    impl->length += 1;
    return VECTOR_ALL_OK;
}

/**
* obj��Ԫ�صĸ���
*
* @param obj			c_vectorʵ��ָ��
*
* @return				Ԫ�صĸ�����<0��ʾʧ��
*
*/
static 	int size(struct _c_vector* obj)
{
    c_vector_impl_t* impl = (c_vector_impl_t*)obj;
    if(!impl)
        return VECTOR_ERR_INVALID_INSTANCE;

    return impl->length;
}

/**
* ��obj��vec�����ݽ���
*
* @param obj			c_vectorʵ��ָ��
* @param obj			���������ݵ�c_vectorʵ��ָ��
*
* @return				VECTOR_ALL_OK��ʾ�ɹ�������Ϊʧ��
*
*/
static int swap(struct _c_vector* obj, struct _c_vector* vec)
{
    c_vector_impl_t temp;
    c_vector_impl_t* impl = (c_vector_impl_t*)obj;
    c_vector_impl_t* impl_vec = (c_vector_impl_t*)vec;
    if(!impl || !impl_vec)
        return VECTOR_ERR_INVALID_INSTANCE;

    memcpy(&temp,		impl,		sizeof(c_vector_impl_t));
    memcpy(impl,		impl_vec,	sizeof(c_vector_impl_t));
    memcpy(impl_vec,	&temp,		sizeof(c_vector_impl_t));

    return VECTOR_ALL_OK;
}

/**
* ��obj����������
*
* @param obj			c_vectorʵ��ָ��
* @param fun_sort		ָ��������Եĺ����Ļص�ָ��
*
* @return				VECTOR_ALL_OK��ʾ�ɹ�������Ϊʧ��
*
*/
static int sort(struct _c_vector* obj, fun_sort_t fun_sort)
{
    unsigned long temp;
    unsigned int pos, pos_sort, pos_max;
    c_vector_impl_t* impl = (c_vector_impl_t*)obj;
    if(!impl)
        return VECTOR_ERR_INVALID_INSTANCE;

    if(!fun_sort)
        return VECTOR_ERR_PARAM;

    if(impl->length < 2)
        return VECTOR_ALL_OK;

    /*ֻ������Ԫ�أ�ֱ�Ӵ���*/
    if(impl->length == 2)
    {
        if(-1 == fun_sort((void*)impl->content[0], (void*)impl->content[1]))
        {
            temp = impl->content[0];
            impl->content[0] = impl->content[1];
            impl->content[1] = temp;
        }

        return VECTOR_ALL_OK;
    }

    /*ð������*/
    for(pos = 0; pos < impl->length - 1; pos++)
    {
        pos_max = pos;
        for(pos_sort = pos + 1; pos_sort < impl->length; pos_sort++)
        {
            if(-1 == fun_sort((void*)impl->content[pos_sort], (void*)impl->content[pos_max]))
                pos_max = pos_sort;
        }

        if(pos_max != pos)
        {
            temp					= impl->content[pos];
            impl->content[pos]		= impl->content[pos_max];
            impl->content[pos_max]	= temp;
        }
    }

    return VECTOR_ALL_OK;
}

/**
* ��obj���е�ÿ��Ԫ�ص�����fun_each
*
* @param obj			c_vectorʵ��ָ��
* @param fun_each		�����õĺ���
*
* @return				VECTOR_ALL_OK��ʾ�ɹ�������Ϊʧ��
*
*/
static int for_each(struct _c_vector* obj, fun_each_t fun_each)
{
    unsigned int pos = 0;
    c_vector_impl_t* impl = (c_vector_impl_t*)obj;
    if(!impl)
        return VECTOR_ERR_INVALID_INSTANCE;

    if(!fun_each)
        return VECTOR_ERR_PARAM;

    for(; pos < impl->length; pos++)
        fun_each((void*)impl->content[pos]);

    return VECTOR_ALL_OK;
}

/**
* vector������
*/
typedef struct _list_objs
{
    c_vector_impl_t* data;			/*vector����*/
    unsigned char b_valid;			/*�˶����Ƿ���ʹ��*/
    struct _list_objs* next;		/*��һ����*/
} c_vector_list_objs;

/**
* vector������������
*/
typedef struct
{
    c_vector_list_objs* start;		/*��������ͷ���*/
    c_vector_list_objs* end;		/*��������β���*/
    unsigned int num;				/*�������Ŀ*/
} c_vector_list_objs_mgr;

static c_vector_list_objs_mgr* l_mgr;


/**
* c_vector��������߳�ʼ������
*
* @return				ALL_OK��ʾ�ɹ��������ʾʧ��
*
* @note					����c_vector�����������Ϊ�˽���Щ���õ�c_vector�����ݴ����������´���Ҫ
						����c_vectorʱ������ʹ����ǰ���õ�c_vector���󣬶���Ƶ��ʹ��c_vector����
						����˵�������Ч�ʣ������ڴ���Ƭ����c_vector��������߲��Ǳ����

*/
int c_vector_mgr_init(void)
{
    if(l_mgr)
        return VECTOR_ALL_OK;

    l_mgr = (c_vector_list_objs_mgr*)Ram_Alloc(0, sizeof(c_vector_list_objs_mgr));
    if(!l_mgr)
        return VECTOR_ERR_NO_MEMORY;

    memset(l_mgr, 0, sizeof(c_vector_list_objs_mgr));
    return VECTOR_ALL_OK;
}

/**
* c_vector��������߻���
*
*
* @note					����c_vector�����������Ϊ�˽���Щ���õ�c_vector�����ݴ����������´���Ҫ
						����c_vectorʱ������ʹ����ǰ���õ�c_vector���󣬶���Ƶ��ʹ��c_vector����
						����˵�������Ч�ʣ������ڴ���Ƭ����c_vector��������߲��Ǳ����


						һ��������c_vector_mgr_init()���ڳ����˳�ʱ������ص��ô˺�������������ڴ�й¶��
*/
void c_vector_mgr_release(void)
{
    if(l_mgr)
    {
        c_vector_list_objs* temp = NULL;
        while(l_mgr->start)						/*releaseÿ��c_vector����*/
        {

            /*NOTE�� �˴����Զ�λ����Щobj���ڱ�ʹ���У����ռ�֮���������*/

            Ram_Free(l_mgr->start->data->content);
            Ram_Free(l_mgr->start->data);
            temp = l_mgr->start;
            l_mgr->start = l_mgr->start->next;
            Ram_Free(temp);
        }

        Ram_Free(l_mgr);
        l_mgr = NULL;
    }
}

/**
* c_vectorʵ�������ӿ�
*
* @return				c_vectorʵ��ָ��
*
*/
c_vector* c_vector_create(void)
{
    c_vector_impl_t* obj = NULL;
    c_vector_list_objs* temp = NULL;
    if(l_mgr)   /*�Ƿ�ʹ���˶����*/
    {
        /*���ȴ�list����*/
        temp = l_mgr->start;
        while(temp)
        {
            if(!temp->b_valid)
            {
                /*��Ч�����ʾ�˶����Ѿ�����ʹ�ã�������Ϊ�µ�*/

                temp->b_valid = 1; /*��Ϊ��Ч*/
                return (c_vector*)temp->data;
            }

            temp = temp->next;
        }
    }

    /*list��û�����������ģ�����*/
    obj = (c_vector_impl_t*)Ram_Alloc(0, sizeof(c_vector_impl_t));
    if(!obj)
        return NULL;
    if(l_mgr)   /*�Ƿ�ʹ���˶����*/
    {
        /*���ɽڵ�*/
        temp = (c_vector_list_objs*)Ram_Alloc(0, sizeof(c_vector_list_objs));
        if(!temp)
        {
            Ram_Free(obj);
            return NULL;
        }
    }
    memset(obj,		0, sizeof(c_vector_impl_t));


    /*���ýӿ�*/
    obj->base.at			= at;
    obj->base.back			= back;
    obj->base.clear			= clear;
    obj->base.copy_vec		= copy_vec;
    obj->base.empty			= empty;
    obj->base.erase			= erase;
    obj->base.for_each		= for_each;
    obj->base.front			= front;
    obj->base.insert		= insert;
    obj->base.insert_vec	= insert_vec;
    obj->base.push_back		= push_back;
    obj->base.size			= size;
    obj->base.sort			= sort;
    obj->base.swap			= swap;

    if(l_mgr)   /*�Ƿ�ʹ���˶����*/
    {
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
    }

    return (c_vector*)obj;
}

/**
* c_vectorʵ�����սӿ�
*
* @param obj			c_vectorʵ��ָ��
*
*/
void c_vector_release(c_vector* obj)
{
    c_vector_impl_t* impl = (c_vector_impl_t*)obj;
    if(impl)
    {
        if(l_mgr)
        {
            /*���Ҵ�obj�Ƿ����ж�Ӧ�ڵ�*/
            c_vector_list_objs* temp = l_mgr->start;
            while(temp)
            {
                if(temp->data == impl)
                {
                    /*�ڽڵ���*/

                    /*���ԭ������������ָ��ֵ�������ͷŵ�  modified bu skybuf at 08.2.12*/
                    if(temp->data->capacity > DEFAULT_CAPACITY_RELEASE_LIMIT)
                    {
                        Ram_Free(temp->data->content);
                        temp->data->content		= NULL;
                        temp->data->capacity	= 0;
                    }
                    /*end modified*/

                    temp->data->length = 0;

                    /*��Ϊ��Ч*/
                    temp->b_valid = 0;
                    return;
                }

                temp = temp->next;
            }
        }


        Ram_Free(impl->content);
        Ram_Free(impl);
    }
}
