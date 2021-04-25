/*******************************************************************************/
 /**
  ******************************************************************************
  * @file    mira_std.h
  * @author  ycwang@miramems.com
  * @version V1.0
  * @date    26-Nov-2014
  * @brief
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, MiraMEMS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2014 MiraMEMS</center></h2>
  */
/*******************************************************************************/
#ifndef __mira_std_h
#define __mira_std_h

/*******************************************************************************
Macro definitions - Register define for Gsensor asic
********************************************************************************/
#define NSA_REG_SPI_I2C                 0x00
#define NSA_REG_WHO_AM_I                0x01
#define NSA_REG_ACC_X_LSB               0x02
#define NSA_REG_ACC_X_MSB               0x03
#define NSA_REG_ACC_Y_LSB               0x04
#define NSA_REG_ACC_Y_MSB               0x05
#define NSA_REG_ACC_Z_LSB               0x06
#define NSA_REG_ACC_Z_MSB               0x07 
#define NSA_REG_MOTION_FLAG             0x09 
#define NSA_REG_G_RANGE                 0x0f
#define NSA_REG_ODR_AXIS_DISABLE        0x10
#define NSA_REG_POWERMODE_BW            0x11
#define NSA_REG_SWAP_POLARITY           0x12
#define NSA_REG_FIFO_CTRL               0x14
#define NSA_REG_INTERRUPT_SETTINGS0     0x15
#define NSA_REG_INTERRUPT_SETTINGS1     0x16
#define NSA_REG_INTERRUPT_SETTINGS2     0x17
#define NSA_REG_INTERRUPT_MAPPING1      0x19
#define NSA_REG_INTERRUPT_MAPPING2      0x1a
#define NSA_REG_INTERRUPT_MAPPING3      0x1b
#define NSA_REG_INT_PIN_CONFIG          0x20
#define NSA_REG_INT_LATCH               0x21
#define NSA_REG_ACTIVE_DURATION         0x27
#define NSA_REG_ACTIVE_THRESHOLD        0x28
#define NSA_REG_TAP_DURATION            0x2A
#define NSA_REG_TAP_THRESHOLD           0x2B
#define NSA_REG_ENGINEERING_MODE        0x7f
#define NSA_REG_SENS_COMP               0x8c
#define NSA_REG_MEMS_OPTION             0x8f
#define NSA_REG_CHIP_INFO               0xc0

/*******************************************************************************
Typedef definitions
********************************************************************************/
#define ARM_BIT_8               0

#if ARM_BIT_8
//����������������8λ���϶���ģ�������ƽ̨������32λ�����ܴ��ڲ����Ҫ����ʵ������޸� �� 
typedef unsigned char    u8_m;                   /* �޷���8λ���ͱ���*/
typedef signed   char    s8_m;                   /* �з���8λ���ͱ���*/
typedef unsigned int     u16_m;                  /* �޷���16λ���ͱ���*/
typedef signed   int     s16_m;                  /* �з���16λ���ͱ���*/
typedef unsigned long    u32_m;                  /* �޷���32λ���ͱ���*/
typedef signed   long    s32_m;                  /* �з���32λ���ͱ���*/
typedef float            fp32_m;                 /* �����ȸ�������32λ���ȣ�*/
typedef double           fp64_m;                 /* ˫���ȸ�������64λ���ȣ�*/
#else
//����������������32λ���϶���ģ�������ƽ̨������8λ�����ܴ��ڲ����Ҫ����ʵ������޸� �� 
typedef unsigned char    u8_m;                   /* �޷���8λ���ͱ���*/
typedef signed   char    s8_m;                   /* �з���8λ���ͱ���*/
typedef unsigned short   u16_m;                  /* �޷���16λ���ͱ���*/
typedef signed   short   s16_m;                  /* �з���16λ���ͱ���*/
typedef unsigned int     u32_m;                  /* �޷���32λ���ͱ���*/
typedef signed   int     s32_m;                  /* �з���32λ���ͱ���*/
typedef float            fp32_m;                 /* �����ȸ�������32λ���ȣ�*/
typedef double           fp64_m;                 /* ˫���ȸ�������64λ���ȣ�*/
#endif

typedef struct AccData_tag{
   s16_m ax;                                   //���ٶȼ�ԭʼ���ݽṹ��  ���ݸ�ʽ 0 0 1024
   s16_m ay;
   s16_m az;

}AccData;

#define mir3da_abs(x)          (((x) > 0) ? (x) : (-(x)))
s32_m mir3da_sqrt(s32_m val);
void mir3da_memset(void *dest, u8_m count);
void* mir3da_memcpy(void* dest, void* source, u8_m count);
#endif
