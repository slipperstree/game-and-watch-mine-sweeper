// �Զ���ASCII�����ֿ�ͷ�ļ�

#ifndef __FONT_H_
#define __FONT_H_

#include "def_types.h"

// �ֿ�ṹ�嶨�塣��Ҫ�޸� ----------
typedef struct 
{
  u8  fontWidth;   // ������
  u8  fontHeight;  // ����߶�
  u8* fontArray;   // ��������
  u8  type;        // �������ݣ�0:ASCII 1:���� 2:λͼ(��)��
  u8* hzStr;       // �����ֿ���������˳��������ֿ������к��ֵ��ַ�����Ascii����Ҫ����ֱ�Ӹ���ascii������������
  void*          asciiZk;     // ������������ֺͰ�ǻ��ţ���ô�����ֿ����ṩһ����ǵ��ֿ�ָ�롣
} Font_Type;

// ����type��ȡֵ��Χ
#define FONT_TYPE_ASCII 0
#define FONT_TYPE_HZ    1
#define FONT_TYPE_IMG   2

// ����Զ������壬���޸� ------------------------------------------------------------------------------------------------------------
// ### ����1 ###
//     -������Ҫ����һ���궨�壬���� SUPPORT_CHAR_SIZE_24����ֱ��ʹ���Ѿ�������ġ�
//     Ϊ�˽�ʡrom�ռ䣬������Ҫ�Ķ���ע�͵�

// Ӣ������(��������ASCII) --------------------
//#define SUPPORT_CHAR_SIZE_8
#define SUPPORT_CHAR_SIZE_12
#define SUPPORT_CHAR_SIZE_16
#define SUPPORT_CHAR_SIZE_20
#define SUPPORT_CHAR_SIZE_32

// ��������(ֻ������Ҫ�ĺ���) ------------------
//   ע�⣬��Ӧ�ĺ����������Ҫ����һ��Ӣ�����壬
//   ������Ҫͬʱ�ſ�������Ӣ�����壬һ����ͬSize������
//#define SUPPORT_SIZE_12
//#define SUPPORT_SIZE_16
#define SUPPORT_SIZE_32

// ���������� --------------------------------
//#define SUPPORT_TITLE_32
#define SUPPORT_TITLE_64

// λͼ������ --------------------------------
// S&W LOGO������
//#define SUPPORT_LOGO_20
//#define SUPPORT_LOGO_32
#define SUPPORT_LOGO_70

// demo������ʾ����ʾģʽ4������
#define SUPPORT_DEMO_MODE_24

// ��
//#define SUPPORT_IMG_SNAKE8
#define SUPPORT_IMG_SNAKE12

// ### ����2 ###
//     -���һ��extern Font_Type���幩���ֻ��Ʒ������ã����������⡣���� extern Font_Type FONT_24;

// Ӣ������(��������ASCII) --------------------
#ifdef SUPPORT_CHAR_SIZE_8
  extern Font_Type FONT_ASC8;
#endif
#ifdef SUPPORT_CHAR_SIZE_12
  extern Font_Type FONT_ASC12;
#endif
#ifdef SUPPORT_CHAR_SIZE_16
  extern Font_Type FONT_ASC16;
#endif
#ifdef SUPPORT_CHAR_SIZE_20
  extern Font_Type FONT_ASC20;
#endif
#ifdef SUPPORT_CHAR_SIZE_32
  extern Font_Type FONT_ASC32;
#endif

// ��������(ֻ������Ҫ�ĺ���) ------------------
#ifdef SUPPORT_SIZE_12
  extern Font_Type FONT12;
#endif
#ifdef SUPPORT_SIZE_16
  extern Font_Type FONT16;
#endif
#ifdef SUPPORT_SIZE_32
  extern Font_Type FONT32;
#endif

// ���������� --------------------------------
#ifdef SUPPORT_TITLE_32
  extern Font_Type FONT_TITLE32;
#endif
#ifdef SUPPORT_TITLE_64
  extern Font_Type FONT_TITLE;
#endif

// λͼ������ --------------------------------
// S&W LOGO������
#ifdef SUPPORT_LOGO_20
  extern Font_Type FONT_LOGO20;
  #ifndef FONT_LOGO_IDX_0
  #define FONT_LOGO_IDX_0 0
  #endif
#endif
#ifdef SUPPORT_LOGO_32
  extern Font_Type FONT_LOGO32;
  #ifndef FONT_LOGO_IDX_0
  #define FONT_LOGO_IDX_0 0
  #endif
#endif
#ifdef SUPPORT_LOGO_70
  extern Font_Type FONT_LOGO70;
  #ifndef FONT_LOGO_IDX_0
  #define FONT_LOGO_IDX_0 0
  #endif
#endif
#ifdef SUPPORT_DEMO_MODE_24
  extern Font_Type FONT_DEMO_MOEDE24;
  #define FONT_DEMO_MODE_YAN  0
  #define FONT_DEMO_MODE_SHI1 1
  #define FONT_DEMO_MODE_MO   2
  #define FONT_DEMO_MODE_SHI2 3
#endif

// ��
#ifdef SUPPORT_IMG_SNAKE8
  extern Font_Type FONT_IMG_SNAKE8;
#endif
#ifdef SUPPORT_IMG_SNAKE12
  extern Font_Type FONT_IMG_SNAKE12;
#endif

// ### ����3 ###
//     -��font.c�м�һ��code���͵����飬�����ŵ������ݣ��������ݵĹ�����font.c����˵���������� const u8 _F12x24[]={.....};
// font.c

// ### ����4 ###
//     -��font.c��׷�Ӳ���һ���������ⲿ����ʵ�嶨�壬������Ҫ�Ͳ���һһ�¡�
// ������4����Ա���ֱ���ÿ���ַ��ĵ����ȣ��߶ȣ��Լ��ڲ���3����ӵ������ַ���÷���&��ȡ�����Ƿ�Ϊ�����ֿ�(�Ǻ��ֿ�ʡ��)��
// ���� Font_Type FONT24 = { 12, 24, &_F12x24, IS_HANZI };

// ����1-4��ɺ󼴿���������ƺ�����ֱ�ӵ�����(ע�⴫�ε�ʱ��Ҫ�÷���&ȡһ�µ�ַ)������
// devShowString(10, 10, "hello", &FONT24, 0x00, 0x00, 0xff, 0xff);


// ̰ʳ����Ϸ��
// ���е�RLUD��ʾ���ڵķ�������һ���ٽӵķ��򣬱�����ͷ���µ�ID����ΪU����Ϊ��ͷ����ʱ�����ڷ������Ϸ���
// �������������ڷ���V��H��ʾ����ͺ���
#define FONT_SNAKE_BLOCK_APPLE    0
#define FONT_SNAKE_BLOCK_HEAD_R   1
#define FONT_SNAKE_BLOCK_HEAD_L   2
#define FONT_SNAKE_BLOCK_HEAD_U   3
#define FONT_SNAKE_BLOCK_HEAD_D   4
#define FONT_SNAKE_BLOCK_BODY_V   5
#define FONT_SNAKE_BLOCK_BODY_H   6
#define FONT_SNAKE_BLOCK_BODY_DR  7
#define FONT_SNAKE_BLOCK_BODY_LD  8
#define FONT_SNAKE_BLOCK_BODY_LU  9
#define FONT_SNAKE_BLOCK_BODY_UR  10
#define FONT_SNAKE_BLOCK_TAIL_R   11
#define FONT_SNAKE_BLOCK_TAIL_L   12
#define FONT_SNAKE_BLOCK_TAIL_U   13
#define FONT_SNAKE_BLOCK_TAIL_D   14



// λͼ���幤�ߺ���
// ��ȡ�ַ����������֣���ָ�������е�λ��
#define FONT_ERR_NOT_FOUND  9999
u16 FONT_getByteIndexAtFontData(u8 *chr, Font_Type *fontType, Font_Type **useFont);

#endif
