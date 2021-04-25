
/* ============================================================================
* C string interface
* 
* ����STL��string�Ľӿڣ�ʵ��C�汾��string��
*
* ���ߣ�skybuf  <MSN:skybuf@gmail.com>
* 
* ��Դ���룬�����뽻��֮�ã��ް�Ȩ
*
* ========================================================================= */
#ifndef __C_STRING_H__
#define __C_STRING_H__

#ifdef _cplusplus
extern "C"{
#endif

#if !defined (__REMOVE_LOG_TO_FILE__)

/*����ֵ����*/
#define STRING_ALL_OK					0				/*��ã�û�д���*/
#define STRING_ERR_NO_MEMORY			-100			/*�ڴ治��*/
#define STRING_ERR_INVALID_INSTANCE		-101			/*��Ч��c_stringʵ��*/
#define STRING_ERR_PARAM				-102			/*��������*/



/**
* c_string�Ľӿڼ���
*/
typedef struct _c_string
{
	/**
	* ��str��pos_startλ�ÿ�ʼ��num���ַ�׷�ӵ�obj��β����
	*
	* @param obj			c_stringʵ��ָ��
	* @param str			��׷�ӵ��ַ���
	* @param pos_start		����0��str�е���ʼλ�ã���С��str�ĳ���
	* @param num			��str��pos_startλ������ַ�����-1��ʾʣ���ȫ��
	*
	* @return				ALL_OK��ʾ�ɹ��������ʾʧ��
	*
	* @note					��pos_start+num����str�ĳ��ȣ���num��ͬ��-1����׷��ʣ����ַ�����obj��
	*
	*/
	int (*append)(struct _c_string* obj, const char* str, unsigned int pos_start, int num);
	/**
	* ��string׷�ӵ�obj��β����
	*
	* @param obj			c_stringʵ��ָ��
	* @param string			��׷�ӵ�c_string
	*
	* @return				ALL_OK��ʾ�ɹ��������ʾʧ��
	*/
	int (*append_string)(struct _c_string* obj, const struct _c_string* string);

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
	int (*compare)(struct _c_string* obj, const char* str, unsigned int pos_start, int num, unsigned char b_uplower);

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
	int (*compare_string)(struct _c_string* obj, const struct _c_string* string, unsigned char b_uplower);

	/**
	* ��str��pos_startλ�ÿ�ʼ��num���ַ�������obj�У�obj��ԭ�����ݽ����滻��
	*
	* @param obj			c_stringʵ��ָ��
	* @param str			���������ַ���
	* @param pos_start		����0��str�е���ʼλ�ã���С��str�ĳ���
	* @param num			��str��pos_startλ������ַ�����-1��ʾʣ���ȫ��
	*
	* @return				ALL_OK��ʾ�ɹ��������ʾʧ��
	*
	* @note					��pos_start+num����str�ĳ��ȣ���num��ͬ��-1��������ʣ����ַ���
	*
	*/
	int (*copy)(struct _c_string* obj, const char* str, unsigned int pos_start, int num);
	/**
	* ��string������obj�У�obj��ԭ�����ݽ����滻��
	*
	* @param obj			c_stringʵ��ָ��
	* @param string			��׷�ӵ�c_string
	*
	* @return				ALL_OK��ʾ�ɹ��������ʾʧ��
	*/
	int (*copy_string)(struct _c_string* obj, const struct _c_string* string);

	/**
	* ��obj�д�pos_startλ�ÿ�ʼ��num���ַ�����
	*
	* @param obj			c_stringʵ��ָ��
	* @param pos_start		����0��str�е���ʼλ�ã���С��str�ĳ���
	* @param num			��str��pos_startλ������ַ�����-1��ʾʣ���ȫ����
	*
	* @return				ALL_OK��ʾ�ɹ��������ʾʧ��
	*
	* @note					��pos_start+num����obj���ַ����ĳ��ȣ���num��ͬ��-1��������ʣ����ַ���
	*
	*/
	int (*erase)(struct _c_string* obj, unsigned int pos_start, int num);

	/**
	* ��obj�д�pos_startλ�ÿ�ʼ��num���ַ��滻��str
	*
	* @param obj			c_stringʵ��ָ��
	* @param pos_start		����0��obj���ַ�������ʼλ�ã���С��obj�ĳ���
	* @param num			��obj���ַ�����pos_startλ������ַ�����-1��ʾʣ���ȫ��
	* @param str			���滻���ַ���
	*
	* @return				ALL_OK��ʾ�ɹ��������ʾʧ��
	*
	* @note					��pos_start+num����obj���ַ����ĳ��ȣ���num��ͬ��-1�����滻ʣ����ַ���
	*
	*/
	int (*replace)(struct _c_string* obj, unsigned int pos_start, int num, const char* str);
	/**
	* ��obj�е�str_src�ַ����滻��str_dst
	*
	* @param obj			c_stringʵ��ָ��
	* @param str_src		���滻���ַ���
	* @param str_dst		���滻���ַ���
	*
	* @return				ALL_OK��ʾ�ɹ��������ʾʧ��
	*/
	int (*replace_str)(struct _c_string* obj, const char* str_src, const char* str_dst);

	/**
	* ��str��pos_start_strλ�ÿ�ʼ��num_str���ַ����뵽obj��pos_start_objλ��
	*
	* @param obj			c_stringʵ��ָ��
	* @param pos_start_obj	obj�еĲ���λ�ã�����0
	* @param str			��������ַ���
	* @param pos_start_str	����0��str�е���ʼλ�ã���С��str�ĳ���
	* @param num_str		��str��pos_start_strλ������ַ�����-1��ʾʣ���ȫ��
	*
	* @return				ALL_OK��ʾ�ɹ��������ʾʧ��
	*
	* @note					��pos_start_str+num_str����obj���ַ����ĳ��ȣ���num_str��ͬ��-1��������ʣ����ַ���
	*
	*/
	int (*insert)(struct _c_string* obj, unsigned int pos_start_obj, const char* str,unsigned int pos_start_str, int num_str);
	/**
	* ��string���뵽obj��pos_start_objλ��
	*
	* @param obj			c_stringʵ��ָ��
	* @param pos_start_obj	obj�еĲ���λ�ã�����0
	* @param string			�������c_string
	*
	* @return				ALL_OK��ʾ�ɹ��������ʾʧ��
	*/
	int (*insert_string)( struct _c_string* obj, unsigned int pos_start_obj, const struct _c_string* string );
	/**
	* ��num_ch��ch���뵽obj��pos_start_objλ��
	*
	* @param obj			c_stringʵ��ָ��
	* @param pos_start_obj	obj�еĲ���λ�ã�����0
	* @param ch				��������ַ�
	* @param num_ch			��������ַ���
	*
	* @return				ALL_OK��ʾ�ɹ��������ʾʧ��
	*/
	int (*insert_ch)(struct _c_string* obj, unsigned int pos_start_obj, char ch, unsigned int num_ch);

	/**
	* ��obj��˳��������ַ������״γ���λ�ã�����0�����ַ�����str��pos_start��ʼ��num���ַ����
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
	int (*find_first_of)(struct _c_string* obj, const char* str, unsigned int pos_start, int num);
	/**
	* ��obj��˳��������ַ���string���״γ���λ�ã�����0
	*
	* @param obj			c_stringʵ��ָ��
	* @param string			�����ҵ����ַ���
	*
	* @return				���ַ�������ʼλ�ã�-1��ʾû���ҵ���<-1��ʾʧ��
	*/
	int (*find_first_string_of)(struct _c_string* obj, struct _c_string* string);
	/**
	* ��obj��˳������ַ�ch�״γ��ֵ�λ�ã�����0
	*
	* @param obj			c_stringʵ��ָ��
	* @param ch				�����ҵ��ַ�
	*
	* @return				�ַ�����ʼλ�ã�-1��ʾû���ҵ���<-1��ʾʧ��
	*/
	int (*find_first_ch_of)(struct _c_string* obj, char ch);

	/**
	* ��obj������������ַ������״γ���λ�ã�����0�����ַ�����str��pos_start��ʼ��num���ַ����
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
	int (*r_find_first_of)(struct _c_string* obj, const char* str, unsigned int pos_start, int num);
	/**
	* ��obj������������ַ���string���״γ���λ�ã�����0
	*
	* @param obj			c_stringʵ��ָ��
	* @param string			�����ҵ����ַ���
	*
	* @return				���ַ���������ʼλ�ã�-1��ʾû���ҵ���<-1��ʾʧ��
	*/
	int (*r_find_first_string_of)(struct _c_string* obj, struct _c_string* string);
	/**
	* ��obj��������ַ�ch�״γ��ֵ�λ�ã�����0
	*
	* @param obj			c_stringʵ��ָ��
	* @param ch				�����ҵ��ַ�
	*
	* @return				�ַ�������ʼλ�ã�-1��ʾû���ҵ���<-1��ʾʧ��
	*/
	int (*r_find_first_ch_of)(struct _c_string* obj, char ch);

	/**
	* �õ�obj��λ��indexλ�õ��ַ�
	*
	* @param obj			c_stringʵ��ָ��
	* @param index			����������0
	*
	* @return				�ַ�������0��ʾ����
	*
	*/
	char (*at)(struct _c_string* obj, unsigned int index);
	/**
	* �õ�obj�е��ַ���
	*
	* @param obj			c_stringʵ��ָ��
	*
	* @return				�ַ���
	*/
	const char* (*str)(struct _c_string* obj);
	/**
	* ���obj�е�����
	*
	* @param obj			c_stringʵ��ָ��
	*
	* @return				��
	*/
	void (*clear)(struct _c_string* obj);

	/**
	* ����obj�Ƿ�Ϊ��
	*
	* @param obj			c_stringʵ��ָ��
	*
	* @return				1��ʾΪ�գ�0��ʾ�����ݣ������ʾʧ��
	*/
	unsigned char (*empty)(struct _c_string* obj);
	/**
	* �õ�obj���ַ����ĳ���
	*
	* @param obj			c_stringʵ��ָ��
	*
	* @return				���ȣ� <0��ʾʧ��
	*/
	int (*length)(struct _c_string* obj);

	/**
	* �õ�obj�д�pos_start��ʼ��num���ַ���ɵ����ַ���
	*
	* @param obj			c_stringʵ��ָ��
	* @param pos_start		����0��obj�����ַ�������ʼλ��
	* @param num			��pos_start��ʼ���ַ�����-1��ʾʣ���ȫ��
	*
	* @return				c_stringʵ��ָ�룬�����ַ�������
	*
	* @note					���ص�ʵ��ָ�������c_string_release���ա�
	*
	*						��pos_start+num����obj�ĳ��ȣ���num��ͬ��-1�����õ�ʣ������ַ���
	*
	*/
	struct _c_string* (*substr)(struct _c_string* obj, unsigned int pos_start, int num);
	/**
	* ��obj��string�е����ݽ���
	*
	* @param obj			c_stringʵ��ָ��
	* @param string 		c_stringʵ��ָ��
	*
	* @return				��
	*/
	void (*swap)(struct _c_string* obj, struct _c_string* string);

	/**
	* ��obj�е��ַ���ת���ɴ�д
	*
	* @param obj			c_stringʵ��ָ��
	*
	* @return				��
	*/
	void (*to_upper)(struct _c_string* obj);
	/**
	* ��obj�е��ַ���ת����Сд
	*
	* @param obj			c_stringʵ��ָ��
	*
	* @return				��
	*/
	void (*to_lower)(struct _c_string* obj);
	
}c_string;

/**
* c_string��������߳�ʼ������
*
* @return				ALL_OK��ʾ�ɹ��������ʾʧ��
*
* @note					ͨ����һ�������ڻ�ʹ�ý϶���ַ����������󲿷��ַ�����Ƚ϶̣�
						���ÿ��string�����ǴӶ��Ϸ����µ��ڴ����洢����ʾ���ַ�����
						��Ч�ʵ��£��һ����ܲ����ڴ���Ƭ��

						����c_string�����������Ϊ�˽���Щ���õ�c_string�����ݴ����������´���Ҫ
						����c_stringʱ������ʹ����ǰ���õ�c_string���������ͱ��������������⡣


						�ڳ��ε���c_string_createʱ������ص��ô˺�����������ᴴ��ʧ�ܡ�
*/
int c_string_mgr_init(void);

/**
* c_string��������߻���
*
* 				
* @note					ͨ����һ�������ڻ�ʹ�ý϶���ַ����������󲿷��ַ�����Ƚ϶̣�
						���ÿ��string�����ǴӶ��Ϸ����µ��ڴ����洢����ʾ���ַ�����
						��Ч�ʵ��£��һ����ܲ����ڴ���Ƭ��

						����c_string�����������Ϊ�˽���Щ���õ�c_string�����ݴ����������´���Ҫ
						����c_stringʱ������ʹ����ǰ���õ�c_string���������ͱ��������������⡣
  
	
						�ڳ����˳�ʱ������ص��ô˺�������������ڴ�й¶��
*/
void c_string_mgr_release(void);

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
c_string* c_string_create(const char* str, unsigned int pos_start, int num);

/**
* c_stringʵ�����սӿ�
*
* @param obj			c_stringʵ��ָ�룬��c_string_create����substr����
*
* @return				��
*/
void c_string_release(c_string* obj);


/**
* ����ʹ�õĺ궨�塣����������BREW�ӿڵ�������ʽ
*/
#define STRINGMGR_INIT()													c_string_mgr_init()
#define STRINGMGR_RELEASE()													c_string_mgr_release()

#define STRING_CreateInstance(p_instance, p_str, pos_start, num)			p_instance = c_string_create(p_str, pos_start, num)
#define STRING_Release(p_instance)											c_string_release(p_instance)

#define STRING_Append( obj, str, pos_start, num )							obj->append(obj, str, pos_start, num)
#define STRING_AppendString( obj, string )									obj->append_string(obj, string)
#define STRING_Compare( obj, str, pos_start, num, b_uplower )				obj->compare(obj, str, pos_start, num, b_uplower)
#define STRING_CompareString( obj, string, b_uplower )						obj->compare_string(obj, string, b_uplower)
#define STRING_Copy( obj, str, pos_start, num )								obj->copy(obj, str, pos_start, num)
#define STRING_CopyString( obj, string )									obj->copy_string(obj, string)
#define STRING_Erase( obj, pos_start, num )									obj->erase(obj, pos_start, num)
#define STRING_Replace( obj, pos_start, num, str)							obj->replace(obj, pos_start, num, str)
#define STRING_ReplaceStr( obj, str_src, str_dst )							obj->replace_str( obj, str_src, str_dst )
#define STRING_Insert( obj, pos_start_obj, str, pos_start_str, num_str)		obj->insert(obj, pos_start_obj, str, pos_start_str, num_str)
#define STRING_InsertString( obj, pos_start_obj, string )					obj->insert_string(obj, pos_start_obj, string)
#define STRING_InsertCh(obj, pos_start_obj, ch, num_ch)						obj->insert_ch(obj, pos_start_obj, ch, num_ch)
#define STRING_Find( obj, str, pos_start, num )								obj->find_first_of(obj, str, pos_start, num)
#define STRING_FindString( obj, string )									obj->find_first_string_of(obj, string)
#define STRING_FindCh( obj, ch )											obj->find_first_ch_of(obj, ch)
#define STRING_RFind( obj,  str,  pos_start, num )							obj->r_find_first_of(obj,  str,  pos_start, num)
#define STRING_RFindString( obj, string )									obj->r_find_first_string_of(obj, string)
#define STRING_RFindCh( obj, ch )											obj->r_find_first_ch_of(obj, ch)
#define STRING_At( obj, index )												obj->at(obj, index)
#define STRING_Str( obj )													obj->str(obj)
#define STRING_Clear( obj )													obj->clear(obj)
#define STRING_Empty( obj )													obj->empty(obj)
#define STRING_Length( obj )												obj->length(obj)
#define STRING_Substr( obj,  pos_start,  num )								obj->substr(obj,  pos_start,  num)
#define STRING_Swap( obj,  string )											obj->swap(obj,  string)
#define STRING_ToUpper( obj )												obj->to_upper(obj)
#define STRING_ToLower( obj )												obj->to_lower(obj)

#endif /*__REMOVE_LOG_TO_FILE__*/

#ifdef _cplusplus
}
#endif

#endif