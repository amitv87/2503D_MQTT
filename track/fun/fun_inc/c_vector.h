
/* ============================================================================
* C vector interface
* 
* ����STL��vector�Ľӿڣ�ʵ��C�汾��vector��
*
* ���ߣ�skybuf  <MSN:skybuf@gmail.com>
* 
* ��Դ���룬�����뽻��֮�ã��ް�Ȩ
*
* ========================================================================= */
#ifndef __C_VECTOR_H__
#define __C_VECTOR_H__

#ifdef _cplusplus
extern "C"{
#endif


/*����ֵ����*/
#define VECTOR_ALL_OK					0				/*��ã�û�д���*/
#define VECTOR_ERR_NO_MEMORY			-100			/*�ڴ治��*/
#define VECTOR_ERR_INVALID_INSTANCE		-101			/*��Ч��c_vectorʵ��*/
#define VECTOR_ERR_PARAM				-102			/*��������*/

/**
* �������ָ���ص��������Ͷ���
*
* @param data1			���Ƚϵ�Ԫ��1
* @param data2			���Ƚϵ�Ԫ��2
*
* @return				��vectorԪ�ش������ҵ�˳�򿴣�-1��ʾdata1����data2�����棬 1��ʾdata1����data2�����棬0��ʾ���
*
*/
typedef int (*fun_sort_t)(void* data1, void* data2);

/**
* for_each�ص��������Ͷ��壬����Ϊc_vector�е�ÿ��Ԫ�ص���һ��each����
*
* @param data			c_vector�е�Ԫ��
*
* @return				��
*
*/
typedef void (*fun_each_t)(void* data);

/**
* c_vector�Ľӿڼ���
*/
typedef struct _c_vector
{	
	/**
	* ��vec�����ݿ�����obj�У�obj��ԭ�����ݽ�������
	*
	* @param obj			c_vectorʵ��ָ��
	* @param vec			��������c_vectorʵ��ָ��
	*
	* @return				VECTOR_ALL_OK��ʾ�ɹ��������ʾʧ��
	*
	*/
	int (*copy_vec)(struct _c_vector* obj, const struct _c_vector* vec);

	/**
	* ��obj��λ��index����Ԫ�ء�index����0
	*
	* @param obj			c_vectorʵ��ָ��
	* @param index			�±�
	*
	* @return				��Ӧ��Ԫ�أ�NULL��ʾʧ��
	*
	*/
	void* (*at)(struct _c_vector* obj, unsigned int index);
	/**
	* ȡobj�е����һ��Ԫ��
	*
	* @param obj			c_vectorʵ��ָ��
	*
	* @return				��Ӧ��Ԫ�أ�NULL��ʾʧ��
	*
	*/
	void* (*back)(struct _c_vector* obj);
	/**
	* ȡobj�еĵ�һ��Ԫ��
	*
	* @param obj			c_vectorʵ��ָ��
	*
	* @return				��Ӧ��Ԫ�أ�NULL��ʾʧ��
	*
	*/
	void* (*front)(struct _c_vector* obj);

	/**
	* ���obj
	*
	* @param obj			c_vectorʵ��ָ��
	*
	* @return				��
	*
	*/
	void (*clear)(struct _c_vector* obj);
	/**
	* �ж�obj�Ƿ�Ϊ��
	*
	* @param obj			c_vectorʵ��ָ��
	*
	* @return				0��ʾ��Ϊ�գ�1��ʾΪ��
	*
	*/
	unsigned char (*empty)(struct _c_vector* obj);

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
	int (*erase)(struct _c_vector* obj, unsigned int pos_start, int num);
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
	int (*insert)(struct _c_vector* obj, unsigned int index, void* data);
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
	int (*insert_vec)(struct _c_vector* obj, unsigned int index, const struct _c_vector* vec);

	/**
	* ��data׷�ӵ�obj��
	*
	* @param obj			c_vectorʵ��ָ��
	* @param data			��׷�ӵ�Ԫ��
	*
	* @return				VECTOR_ALL_OK��ʾ�ɹ�������Ϊʧ��
	*
	*/
	int (*push_back)(struct _c_vector* obj, void* data);

	/**
	* obj��Ԫ�صĸ���
	*
	* @param obj			c_vectorʵ��ָ��
	*
	* @return				Ԫ�صĸ�����<0��ʾʧ��
	*
	*/
	int (*size)(struct _c_vector* obj);

	/**
	* ��obj��vec�����ݽ���
	*
	* @param obj			c_vectorʵ��ָ��
	* @param obj			���������ݵ�c_vectorʵ��ָ��
	*
	* @return				VECTOR_ALL_OK��ʾ�ɹ�������Ϊʧ��
	*
	*/
	int (*swap)(struct _c_vector* obj, struct _c_vector* vec);

	/**
	* ��obj����������
	*
	* @param obj			c_vectorʵ��ָ��
	* @param fun_sort		ָ��������Եĺ����Ļص�ָ��
	*
	* @return				VECTOR_ALL_OK��ʾ�ɹ�������Ϊʧ��
	*
	*/
	int (*sort)(struct _c_vector* obj, fun_sort_t fun_sort);

	/**
	* ��obj���е�ÿ��Ԫ�ص�����fun_each
	*
	* @param obj			c_vectorʵ��ָ��
	* @param fun_each		�����õĺ���
	*
	* @return				VECTOR_ALL_OK��ʾ�ɹ�������Ϊʧ��
	*
	*/
	int (*for_each)(struct _c_vector* obj, fun_each_t fun_each);
}c_vector;


/**
* c_vector��������߳�ʼ������
*
* @return				ALL_OK��ʾ�ɹ��������ʾʧ��
*
* @note					����c_vector�����������Ϊ�˽���Щ���õ�c_vector�����ݴ����������´���Ҫ
						����c_vectorʱ������ʹ����ǰ���õ�c_vector���󣬶���Ƶ��ʹ��c_vector����
						����˵�������Ч�ʣ������ڴ���Ƭ����c_vector��������߲��Ǳ����

*/
int c_vector_mgr_init(void);

/**
* c_vector��������߻���
*
* 				
* @note					����c_vector�����������Ϊ�˽���Щ���õ�c_vector�����ݴ����������´���Ҫ
						����c_vectorʱ������ʹ����ǰ���õ�c_vector���󣬶���Ƶ��ʹ��c_vector����
						����˵�������Ч�ʣ������ڴ���Ƭ����c_vector��������߲��Ǳ����
  
	
						һ��������c_vector_mgr_init()���ڳ����˳�ʱ������ص��ô˺�������������ڴ�й¶��
*/
void c_vector_mgr_release(void);

/**
* c_vectorʵ�������ӿ�
*
* @return				c_vectorʵ��ָ��
*
*/
c_vector* c_vector_create(void);

/**
* c_vectorʵ�����սӿ�
*
* @param obj			c_vectorʵ��ָ��
*
*/
void c_vector_release(c_vector* obj);




/**
* ����ʹ�õĺ궨�塣����������BREW�ӿڵ�������ʽ
*/
#define VECTORMGR_INIT()									c_vector_mgr_init()
#define VECTORMGR_RELEASE()									c_vector_mgr_release()

#define VECTOR_CreateInstance(p_instance)					p_instance = c_vector_create()
#define VECTOR_Release(p_instance)							c_vector_release(p_instance)

#define VECTOR_CopyVec( obj, vec )							obj->copy_vec(obj, vec)
#define VECTOR_At( obj, index )								obj->at(obj, index)
#define VECTOR_Back( obj )									obj->back(obj)
#define VECTOR_Front( obj )									obj->front(obj)
#define VECTOR_Clear( obj )									obj->clear(obj)
#define VECTOR_Empty( obj )									obj->empty(obj)
#define VECTOR_Erase( obj, pos_start, num )					obj->erase(obj, pos_start, num)
#define VECTOR_Insert( obj, index, data )					obj->insert( obj, index, data )
#define VECTOR_InsertVec( obj, index, vec)					obj->insert_vec(obj, index, vec)
#define VECTOR_Size(obj)									obj->size(obj)
#define VECTOR_PushBack( obj, data )						obj->push_back(obj, data)
#define VECTOR_Swap( obj, vec )								obj->swap(obj, vec)
#define VECTOR_Sort( obj, fun_sort )						obj->sort(obj, fun_sort)
#define VECTOR_ForEach( obj, fun_each )						obj->for_each(obj, fun_each)

#ifdef _cplusplus
}
#endif

#endif