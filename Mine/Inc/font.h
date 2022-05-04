// �Զ���ASCII�����ֿ�ͷ�ļ�

#ifndef __FONT_H_
#define __FONT_H_

#include "def_types.h"

// �ֿ�ṹ�嶨�塣��Ҫ�޸� ----------
typedef struct 
{
  u8  bitPerPixel; // ÿ������ʹ�ü�λ
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
//#define SUPPORT_CHAR_SIZE_12
//#define SUPPORT_CHAR_SIZE_16
#define SUPPORT_CHAR_SIZE_20
#define SUPPORT_CHAR_SIZE_32

// ��������(ֻ������Ҫ�ĺ���) ------------------
//   ע�⣬��Ӧ�ĺ����������Ҫ����һ��Ӣ�����壬
//   ������Ҫͬʱ�ſ�������Ӣ�����壬һ����ͬSize������
//#define SUPPORT_HZ_SIZE_12
//#define SUPPORT_HZ_SIZE_16
#define SUPPORT_HZ_SIZE_32

// ���������� --------------------------------
//#define SUPPORT_HZ_TITLE_32
#define SUPPORT_HZ_TITLE_64

// λͼ������ --------------------------------
// ѧ��LOGO������
//#define SUPPORT_HZ_XD_LOGO_20
//#define SUPPORT_HZ_XD_LOGO_32
#define SUPPORT_HZ_XD_LOGO_40

// demo������ʾ����ʾģʽ4������
#define SUPPORT_HZ_DEMO_MODE_24

// ����
#define SUPPORT_IMG_STAR_23

// POP STAR
#define SUPPORT_IMG_POPSTAR

// ### ����2 ###
//     -���һ��extern Font_Type���幩���ֻ��Ʒ������ã����������⡣���� extern Font_Type FONT_24;

// Ӣ������(��������ASCII) --------------------
#ifdef SUPPORT_CHAR_SIZE_8
  extern Font_Type FONT8;
#endif
#ifdef SUPPORT_CHAR_SIZE_12
  extern Font_Type FONT12;
#endif
#ifdef SUPPORT_CHAR_SIZE_16
  extern Font_Type FONT16;
#endif
#ifdef SUPPORT_CHAR_SIZE_20
  extern Font_Type FONT20;
#endif
#ifdef SUPPORT_CHAR_SIZE_32
  extern Font_Type FONT32;
#endif

// ��������(ֻ������Ҫ�ĺ���) ------------------
#ifdef SUPPORT_HZ_SIZE_12
  extern Font_Type FONTHZ12;
#endif
#ifdef SUPPORT_HZ_SIZE_16
  extern Font_Type FONTHZ16;
#endif
#ifdef SUPPORT_HZ_SIZE_32
  extern Font_Type FONTHZ32;
#endif

// ���������� --------------------------------
#ifdef SUPPORT_HZ_TITLE_32
  extern Font_Type FONTHZ_TITLE32;
#endif
#ifdef SUPPORT_HZ_TITLE_64
  extern Font_Type FONTHZ_TITLE64;
#endif

// λͼ������ --------------------------------
// ѧ��LOGO������
#ifdef SUPPORT_HZ_XD_LOGO_20
  extern Font_Type FONTHZ_XD_LOGO20;
  #ifndef FONT_HZ_XD_LOGO
  #define FONT_HZ_XD_LOGO 0
  #endif
#endif
#ifdef SUPPORT_HZ_XD_LOGO_32
  extern Font_Type FONTHZ_XD_LOGO32;
  #ifndef FONT_HZ_XD_LOGO
  #define FONT_HZ_XD_LOGO 0
  #endif
#endif
#ifdef SUPPORT_HZ_XD_LOGO_40
  extern Font_Type FONTHZ_XD_LOGO40;
  #ifndef FONT_HZ_XD_LOGO
  #define FONT_HZ_XD_LOGO 0
  #endif
#endif
#ifdef SUPPORT_HZ_DEMO_MODE_24
  extern Font_Type FONTHZ_DEMO_MOEDE24;
  #define FONT_HZ_DEMO_MODE_YAN  0
  #define FONT_HZ_DEMO_MODE_SHI1 1
  #define FONT_HZ_DEMO_MODE_MO   2
  #define FONT_HZ_DEMO_MODE_SHI2 3
#endif

extern Font_Type FONT_DIGIT23;
extern Font_Type FONT_IMG_2BIT_BLOCK_BG23;
extern Font_Type FONT_IMG_2BIT_BLOCK_FLAG16;
extern Font_Type FONT_IMG_2BIT_BLOCK_MINE_AND_NUMS22;


// ### ����3 ###
//     -��font.c�м�һ��code���͵����飬�����ŵ������ݣ��������ݵĹ�����font.c����˵���������� code u8 _F12x24[]={.....};
// font.c

// ### ����4 ###
//     -��font.c��׷�Ӳ���һ���������ⲿ����ʵ�嶨�壬������Ҫ�Ͳ���һһ�¡�
// ������4����Ա���ֱ���ÿ���ַ��ĵ����ȣ��߶ȣ��Լ��ڲ���3����ӵ������ַ���÷���&��ȡ�����Ƿ�Ϊ�����ֿ�(�Ǻ��ֿ�ʡ��)��
// ���� Font_Type FONT24 = { 12, 24, &_F12x24, IS_HANZI };

// ����1-4��ɺ󼴿���������ƺ�����ֱ�ӵ�����(ע�⴫�ε�ʱ��Ҫ�÷���&ȡһ�µ�ַ)������
// showString(10, 10, "hello", &FONT24, 0x00, 0x00, 0xff, 0xff);

// ����λͼ
// δ��������ť��״��
#define FONT_MINE_BLOCK_BG23_BTN    0
#define FONT_MINE_BLOCK_BG23_DIGED  1
// ����
#define FONT_MINE_BLOCK_FLAG16      0
// ��������
#define FONT_MINE_BLOCK_MINE        0
#define FONT_MINE_BLOCK_NUM1        1
#define FONT_MINE_BLOCK_NUM2        2
#define FONT_MINE_BLOCK_NUM3        3
#define FONT_MINE_BLOCK_NUM4        4
#define FONT_MINE_BLOCK_NUM5        5
#define FONT_MINE_BLOCK_NUM6        6
#define FONT_MINE_BLOCK_NUM7        7
#define FONT_MINE_BLOCK_NUM8        8
#define FONT_MINE_BLOCK_MINE_WRONG  9

// λͼ���幤�ߺ���
// ��ȡ�ַ����������֣���ָ�������е�λ��
#define FONT_ERR_NOT_FOUND  9999
u16 FONT_getByteIndexAtFontData(u8 *chr, Font_Type *fontType, Font_Type **useFont);
#endif
