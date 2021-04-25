///////////////////////////////////////////////////////////////
//	�� �� �� : des.h
//	�ļ����� : DES/3DES����/����
//	��    �� : ��˫ȫ
//	����ʱ�� : 2006��9��2��
//	��Ŀ���� : DES�����㷨
//	��    ע :
//	��ʷ��¼ : 
///////////////////////////////////////////////////////////////
//#include "custom_nvram_editor_data_item.h"

//#include "track_os_data_types.h"
//#include "track_os_log.h"
#include "track_fun_des.h"

//Ϊ����߳���Ч�ʣ���������λ�������ܶ����ںꡣ

//��ȡ��������ָ��λ.
#define GET_BIT(p_array, bit_index)  \
			((p_array[(bit_index) >> 3] >> (7 - ((bit_index) & 0x07))) & 0x01)

//���û�������ָ��λ.
#define SET_BIT(p_array, bit_index, bit_val) \
			if (1 == (bit_val)) \
			{\
				p_array[(bit_index) >> 3] |= 0x01 << (7 - ((bit_index) & 0x07));\
			}\
			else\
			{\
				p_array[(bit_index) >> 3] &= ~(0x01 << (7 - ((bit_index) & 0x07)));\
			}

//�ӽ��ܱ�ʶ����������ʶ�漰���Ա�Ķ�ȡλ��,
//���뱣֤DES_ENCRYPT = 0 DES_DECRYPT = 1
typedef enum
{
	DES_ENCRYPT = 0,
	DES_DECRYPT = 1
}DES_MODE;

///////////////////////////////////////////////////////////////
//	�� �� �� : des
//	�������� : DES�ӽ���
//	������� : ���ݱ�׼��DES�����㷨�������64λ��Կ��64λ���Ľ��м�/����
//				������/���ܽ���洢��p_output��
//	ʱ    �� : 2006��9��2��
//	�� �� ֵ : 
//	����˵�� :	const char * p_data		����, ����ʱ��������, ����ʱ��������, 64λ(8�ֽ�)
//				const char * p_key		����, ��Կ, 64λ(8�ֽ�)
//				char * p_output			���, ����ʱ�������, ����ʱ��������, 64λ(8�ֽ�)
//				uint8 mode				0 ����  1 ����
///////////////////////////////////////////////////////////////
static void des(kal_int8 * p_data, kal_int8 * p_key, kal_int8 * p_output, DES_MODE mode);

// ��ʼ�û� 
static kal_uint8 Table_IP[64] = 
{ 
	58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4, 
	62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8, 
	57, 49, 41, 33, 25, 17,  9, 1, 59, 51, 43, 35, 27, 19, 11, 3, 
	61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7 
}; 

// ĩ�û� 
static kal_uint8 Table_InverseIP[64] = 
{ 
	40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31, 
	38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29, 
	36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27, 
	34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41,  9, 49, 17, 57, 25 
}; 

// ��չ�û�
static kal_uint8 Table_E[48] = 
{ 
	32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9, 
	8,  9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17, 
	16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25, 
	24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1 
}; 

// ��Կ��ʼ�û� 
static kal_uint8 Table_PC1[56] = { 
	57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18, 
	10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36, 
	63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22, 
	14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4 
}; 

// ���������� 
static kal_int8 Table_Move[2][16] = 
{ 
	//��������
	{1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1},

	//��������
	{0, -1, -2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -1}
}; 

// ��Կѹ���û� 
static kal_uint8 Table_PC2[48] = 
{ 
	14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10, 
	23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2, 
	41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48, 
	44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32 
}; 

// S�� 
static kal_uint8 Table_SBOX[8][4][16] = 
{ 
	// S1 
	14, 4, 13, 1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7, 
	0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8, 
	4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0, 
	15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13, 
	// S2 
	15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10, 
	3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5, 
	0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15, 
	13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9, 
	// S3 
	10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8, 
	13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1, 
	13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7, 
	1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12, 
	// S4 
	7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15, 
	13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9, 
	10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4, 
	3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14, 
	// S5 
	2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9, 
	14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6, 
	4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14, 
	11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3, 
	// S6 
	12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11, 
	10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8, 
	9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6, 
	4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13, 
	// S7 
	4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1, 
	13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6, 
	1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2, 
	6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12, 
	// S8 
	13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7, 
	1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2, 
	7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8, 
	2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11 
}; 

// P���û� 
static kal_uint8 Table_P[32] = 
{ 
	16, 7, 20, 21, 29, 12, 28, 17, 1,  15, 23, 26, 5, 18, 31, 10, 
	2,  8, 24, 14, 32, 27, 3,  9,  19, 13, 30, 6, 22, 11, 4,  25 
}; 

static kal_uint16 crctab16[] = 
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

static kal_uint16 GetCrc16(const kal_uint8* pData, int nLength)
{
    kal_uint16 fcs = 0xffff;    // ��ʼ��
    kal_uint8 tmp;
    while(nLength>0)
    {
        fcs = (fcs >> 8) ^ crctab16[(fcs ^ *pData) & 0xff];
        nLength--;
        pData++;
    }
    return ~fcs;
}

static kal_bool IsDataCrc16ext(const kal_uint8* pData, int nLength, kal_uint16 crc)
{
    kal_uint16 fcs;
    fcs = GetCrc16(pData, nLength);
    return (crc == fcs);
}

//�������С��ͬ���ڴ����������
//��������浽��һ���ڴ�
//kal_uint8 * p_buf_1		�ڴ���1
//kal_uint8 * p_buf_2	�ڴ���2
//kal_uint8 bytes			�ڴ�����С(��λ���ֽ�)
static void Xor(kal_uint8 * p_buf_1, kal_uint8 * p_buf_2, kal_uint8 bytes)
{
	while(bytes > 0)
	{
		bytes--;

		p_buf_1[bytes] ^= p_buf_2[bytes];
	}
}

//���������ӵ�bit_startλ����bit_end����ѭ������
//offsetֻ����1��2
//���δ��뻹�����Ż���
static void move_left(kal_uint8 * p_input, kal_uint8 bit_start, kal_uint8 bit_end, kal_uint8 offset) 
{ 
	kal_uint8 b_val = 0;
	kal_uint8 b_tmp1 = 0;
	kal_uint8 b_tmp2 = 0;

	//��ȡbit_startλ
	b_tmp1 = GET_BIT(p_input, bit_start);
	b_tmp2 = GET_BIT(p_input, bit_start + 1);

	//ѭ������offsetλ
	for(; bit_start <= (bit_end - offset); bit_start++)
	{
		b_val = GET_BIT(p_input, bit_start + offset);
		SET_BIT(p_input, bit_start, b_val);
	}

	//��bit_start��ʼ��offsetλ�Ƶ�bit_end��ͷ��
	if (1 == offset)
	{
		SET_BIT(p_input, bit_end, b_tmp1);
	}
	else
	{
		SET_BIT(p_input, bit_end, b_tmp2);
		SET_BIT(p_input, bit_end - 1, b_tmp1);
	}
} 

//���������ӵ�bit_startλ����bit_end����ѭ������
//offsetֻ����1��2
//���δ����������ϻ������Ż���
static void move_right(kal_uint8 * p_input, kal_uint8 bit_start, kal_uint8 bit_end, kal_uint8 offset) 
{ 
	kal_uint8 b_val = 0;
	kal_uint8 b_tmp1 = 0;
	kal_uint8 b_tmp2 = 0;

	//��ȡbit_endλ
	b_tmp1 = GET_BIT(p_input, bit_end);
	b_tmp2 = GET_BIT(p_input, bit_end - 1);

	//ѭ������offsetλ
	for(; bit_end >= (bit_start + offset); bit_end--)
	{
		b_val = GET_BIT(p_input, bit_end - offset);
		SET_BIT(p_input, bit_end, b_val);
	}

	//��bit_end������offsetλ�Ƶ�bit_start��
	if (1 == offset)
	{
		SET_BIT(p_input, bit_start, b_tmp1);
	}
	else
	{
		SET_BIT(p_input, bit_start, b_tmp2);
		SET_BIT(p_input, bit_start + 1, b_tmp1);
	}
} 

//��������λ
//offset����0ʱ����
//offsetС��0ʱ����
static void move_bits(kal_uint8 * p_input, kal_uint8 bit_start, kal_uint8 bit_end, kal_int8 offset)
{
	if(0 < offset)	//����
	{
		move_left(p_input, bit_start, bit_end, offset);	
	}	
	else if(0 > offset)	//����
	{
		move_right(p_input, bit_start, bit_end, -offset);
	}
}

//ͨ���û�����, bits <= 64
//p_input��p_output����ָ��ͬһ����ַ�������û������
static void Permutation(kal_uint8 * p_input, kal_uint8 * p_output, kal_uint8 * Table, kal_uint8 bits) 
{ 
	kal_uint8 b_val = KAL_FALSE;
	kal_uint8 bit_index = 0;

	for(bit_index = 0; bit_index < bits; bit_index++) 
	{
		b_val = GET_BIT(p_input, Table[bit_index] - 1);
		
		SET_BIT(p_output, bit_index, b_val);
	}
} 

//��ȡ��bit_sΪ��ʼ�ĵ�1, 6 λ�����
static kal_uint8 S_GetLine(kal_uint8 * p_data_ext, kal_uint8 bit_s)
{
	return (GET_BIT(p_data_ext, bit_s + 0) << 1) + GET_BIT(p_data_ext, bit_s + 5);
}

//��ȡ��bit_sΪ��ʼ�ĵ�2,3,4,5λ�����
static kal_uint8 S_GetRow(kal_uint8 * p_data_ext, kal_uint8 bit_s)
{
	kal_uint8 row;

	//2,3,4,5λ�����
	row = GET_BIT(p_data_ext, bit_s + 1);
	row <<= 1; 
	row += GET_BIT(p_data_ext, bit_s + 2);
	row <<= 1; 
	row += GET_BIT(p_data_ext, bit_s + 3);
	row <<= 1; 
	row += GET_BIT(p_data_ext, bit_s + 4);

	return row;
}

///////////////////////////////////////////////////////////////
//	�� �� �� : des
//	�������� : DES�ӽ���
//	������� : ���ݱ�׼��DES�����㷨�������64λ��Կ��64λ���Ľ��м�/����
//				������/���ܽ���洢��p_output��
//	ʱ    �� : 2006��9��2��
//	�� �� ֵ : 
//	����˵�� :	kal_uint8 * p_data		����, ����ʱ��������, ����ʱ��������, 64λ(8�ֽ�)
//				kal_uint8 * p_key		����, ��Կ, 64λ(8�ֽ�)
//				kal_uint8 * p_output			���, ����ʱ�������, ����ʱ��������, 64λ(8�ֽ�)
//				kal_uint8 mode				DES_ENCRYPT ����  DES_DECRYPT ����
///////////////////////////////////////////////////////////////
static void des(kal_int8 * p_data, kal_int8 * p_key, kal_int8 * p_output, DES_MODE mode)
{
	kal_uint8 loop = 0;		//16�������ѭ��������
	kal_uint8 key_tmp[8];	//��Կ����ʱ�洢�м���
	kal_uint8 sub_key[6];	//���ڴ洢����Կ

	kal_uint8 * p_left;
	kal_uint8 * p_right;

	kal_uint8 p_right_ext[8];	//R[i]������չ�û����ɵ�48λ����(6�ֽ�), �����ս���Ĵ洢
	kal_uint8 p_right_s[4];		//����S_BOX�û����32λ����(4�ֽ�)

	kal_uint8 s_loop = 0;		//S_BOX�û���ѭ��������

	//��Կ��һ����С��λ, �õ�һ��56λ����Կ����
	Permutation(p_key, key_tmp, Table_PC1, 56);

	//���ĳ�ʼ���û�
	Permutation(p_data, p_output, Table_IP, 64);

	p_left  = p_output;		//L0
	p_right = &p_output[4];	//R0

	for(loop = 0; loop < 16; loop++)
	{
		//������С��İ���56λ��Ϊ��28λ����28λ,
		//����28λ����28λ�ֱ�ѭ����/����, �õ�һ��������
		//�ӽ��ܲ���ʱֻ����λʱ�в���
		move_bits(key_tmp, 0, 27, Table_Move[mode][loop]);
		move_bits(key_tmp, 28, 55, Table_Move[mode][loop]);

		//��Կ�ڶ�����С��λ���õ�һ����48λ������Կ
		Permutation(key_tmp, sub_key, Table_PC2, 48);

		//R0��չ�û�
		Permutation(p_right, p_right_ext, Table_E, 48);

		//��R0��չ�û���õ���48λ����(6�ֽ�)������Կ�������
		Xor(p_right_ext, sub_key, 6);

		//S_BOX�û�
		for(s_loop = 0; s_loop < 4; s_loop++)
		{
			kal_uint8 s_line = 0;
			kal_uint8 s_row = 0;
			kal_uint8 s_bit = s_loop * 12;

			s_line = S_GetLine(p_right_ext, s_bit);
			s_row  = S_GetRow(p_right_ext,	s_bit);
			
			p_right_s[s_loop] = Table_SBOX[s_loop * 2][s_line][s_row];

			s_bit += 6;
			
			s_line = S_GetLine(p_right_ext, s_bit);
			s_row  = S_GetRow(p_right_ext,	s_bit);
			
			p_right_s[s_loop] <<= 4;
			p_right_s[s_loop] += Table_SBOX[(s_loop * 2) + 1][s_line][s_row];
		}

		//P�û�
		Permutation(p_right_s, p_right_ext, Table_P, 32);

		Xor(p_right_ext, p_left, 4);

		memcpy(p_left, p_right, 4);
		memcpy(p_right, p_right_ext, 4);
	}

	memcpy(&p_right_ext[4], p_left, 4);
	memcpy(p_right_ext,	p_right, 4);

	//����ٽ���һ�����û�, �õ����ռ��ܽ��
	Permutation(p_right_ext, p_output, Table_InverseIP, 64);		
}

void track_fun_decryption2(kal_uint8 *dec, kal_uint8 *src, int size)
{
    unsigned char key[] = "abcdefgh";
    unsigned char en_out_put[8] = {0};
    while(size)
    {
        des(src, key, en_out_put, DES_DECRYPT);
        if(size >= 8)
        {
            memcpy(dec, en_out_put, 8);
            size -= 8;
            src += 8;
            dec += 8;
        }
        else
        {
            memcpy(dec, en_out_put, size);
            size = 0;
        }
    }
}

/******************************************************************************
 *  Function    -  track_fun_decryption
 *
 *  Purpose     -  EPO���ܺ���
 *
 *  Description -  ����
 *                 dec  ----  ���ܺ��Ŀ��Buffer
 *                 src  ----  �����ܵ�����Buffer
 *                 szie ----  �����ܵ����ݳ���
 *  Return
 *        >0   ���ܳɹ���ֵ���ڽ��ܺ���ļ���С
 *        -1   ����ʧ�ܣ��ļ�δ������
 *        -2   ����ʧ�ܣ�У�����
 *
 *  modification history
 * ----------------------------------------
 * v1.0  , 06-08-2013,  Cml  written
 * ----------------------------------------
*******************************************************************************/
kal_int32 track_fun_decryption(kal_uint8 *dec, kal_uint8 *src, int size)
{
    kal_uint32 flag = 0;
    kal_uint16 file_crc = 0;
    memcpy(&flag, src, 4);
    if(flag == 0)
    {
        file_crc = src[4];
        file_crc = (file_crc << 8) + src[5];
        track_fun_decryption2(dec, src + 6, size - 6);
        if(!IsDataCrc16ext(dec, size - 6, file_crc))
        {
            return -2;
        }
        else
        {
            return size - 6;
        }
    }
    else
    {
        return -1;
    }
}

#if 0
void track_fun_des_test()
{
    unsigned char key1[] = "12345678";
    unsigned char key2[] = "abcdefgh";
    unsigned char key3[] = "~!@#$%^&";	//���ֻ��Ҫ������Կ��������Կ���Ժ���Կ1һ����
    unsigned char en_data[] = "��������";
    unsigned char en_out_put[8];

    LOGD(L_FUN, L_V5, "\r\n++++++++3DES ����ʾ��++++++++");

    LOGH2(L_FUN, L_V5, "3DES [����]���� ", en_data, 8);

    //3DES ����
    des(en_data, key1, en_out_put, DES_ENCRYPT);
    LOGH2(L_FUN, L_V5, "3DES [����]��Կ1", key1, 8);
    des(en_out_put, key2, en_data, DES_DECRYPT);
    LOGH2(L_FUN, L_V5, "3DES [����]��Կ2", key2, 8);
    des(en_data, key3, en_out_put, DES_ENCRYPT);
    LOGH2(L_FUN, L_V5, "3DES [����]��Կ3", key3, 8);

    LOGH2(L_FUN, L_V5, "3DES [���]���� ", en_out_put, 8);

    LOGD(L_FUN, L_V5, "\r\n");
    LOGD(L_FUN, L_V5, "\r\n++++++++3DES ����ʾ��++++++++");

    LOGH2(L_FUN, L_V5, "3DES [����]���� ", en_out_put, 8);
    //3DES ����
    des(en_out_put, key3, en_data, DES_DECRYPT);
    LOGH2(L_FUN, L_V5, "3DES [����]��Կ3", key3, 8);
    des(en_data, key2, en_out_put, DES_ENCRYPT);
    LOGH2(L_FUN, L_V5, "3DES [����]��Կ2", key2, 8);
    des(en_out_put, key1, en_data, DES_DECRYPT);
    LOGH2(L_FUN, L_V5, "3DES [����]��Կ1", key1, 8);

    LOGH2(L_FUN, L_V5, "3DES [���]���� ", en_data, 8);
}
#endif /* 0 */

