// 自定义ASCII点阵字库头文件

#ifndef __FONT_H_
#define __FONT_H_

#include "def_types.h"

// 字库结构体定义。不要修改 ----------
typedef struct 
{
  u8  fontWidth;   // 字体宽度
  u8  fontHeight;  // 字体高度
  u8* fontArray;   // 点阵数组
  u8  type;        // 点阵内容（0:ASCII 1:汉字 2:位图(组)）
  u8* hzStr;       // 汉字字库索引，按顺序包含了字库中所有汉字的字符串。Ascii不需要，可直接根据ascii码计算出索引。
  void*          asciiZk;     // 如果想做到汉字和半角混排，那么汉字字库里提供一个半角的字库指针。
} Font_Type;

// 上面type的取值范围
#define FONT_TYPE_ASCII 0
#define FONT_TYPE_HZ    1
#define FONT_TYPE_IMG   2

// 添加自定义字体，可修改 ------------------------------------------------------------------------------------------------------------
// ### 步骤1 ###
//     -根据需要增加一個宏定义，比如 SUPPORT_CHAR_SIZE_24。或直接使用已经定义过的。
//     为了节省rom空间，将不需要的定义注释掉

// 英文字体(包含所有ASCII) --------------------
//#define SUPPORT_CHAR_SIZE_8
#define SUPPORT_CHAR_SIZE_12
#define SUPPORT_CHAR_SIZE_16
#define SUPPORT_CHAR_SIZE_20
#define SUPPORT_CHAR_SIZE_32

// 汉字字体(只包含需要的汉字) ------------------
//   注意，对应的汉字字体必须要外链一个英文字体，
//   所以需要同时放开外链的英文字体，一般是同Size的字体
//#define SUPPORT_SIZE_12
//#define SUPPORT_SIZE_16
#define SUPPORT_SIZE_32

// 标题用字体 --------------------------------
//#define SUPPORT_TITLE_32
#define SUPPORT_TITLE_64

// 位图用字体 --------------------------------
// S&W LOGO两个字
//#define SUPPORT_LOGO_20
//#define SUPPORT_LOGO_32
#define SUPPORT_LOGO_70

// demo中央显示的演示模式4个字用
#define SUPPORT_DEMO_MODE_24

// 蛇
//#define SUPPORT_IMG_SNAKE8
#define SUPPORT_IMG_SNAKE12

// ### 步骤2 ###
//     -添加一个extern Font_Type定义供文字绘制方法调用，变量名随意。比如 extern Font_Type FONT_24;

// 英文字体(包含所有ASCII) --------------------
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

// 汉字字体(只包含需要的汉字) ------------------
#ifdef SUPPORT_SIZE_12
  extern Font_Type FONT12;
#endif
#ifdef SUPPORT_SIZE_16
  extern Font_Type FONT16;
#endif
#ifdef SUPPORT_SIZE_32
  extern Font_Type FONT32;
#endif

// 标题用字体 --------------------------------
#ifdef SUPPORT_TITLE_32
  extern Font_Type FONT_TITLE32;
#endif
#ifdef SUPPORT_TITLE_64
  extern Font_Type FONT_TITLE;
#endif

// 位图用字体 --------------------------------
// S&W LOGO两个字
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

// 蛇
#ifdef SUPPORT_IMG_SNAKE8
  extern Font_Type FONT_IMG_SNAKE8;
#endif
#ifdef SUPPORT_IMG_SNAKE12
  extern Font_Type FONT_IMG_SNAKE12;
#endif

// ### 步骤3 ###
//     -在font.c中加一个code类型的数组，里面存放点阵数据（点阵数据的规则在font.c里有说明）。比如 const u8 _F12x24[]={.....};
// font.c

// ### 步骤4 ###
//     -在font.c中追加步骤一里声明的外部变量实体定义，变量名要和步骤一一致。
// 内容有4个成员，分别是每个字符的点阵宽度，高度，以及在步骤3里添加的数组地址（用符号&获取），是否为汉字字库(非汉字可省略)。
// 比如 Font_Type FONT24 = { 12, 24, &_F12x24, IS_HANZI };

// 步骤1-4完成后即可在字体绘制函数里直接调用了(注意传参的时候要用符号&取一下地址)，比如
// devShowString(10, 10, "hello", &FONT24, 0x00, 0x00, 0xff, 0xff);


// 贪食蛇游戏用
// 其中的RLUD表示开口的方向（与另一块临接的方向，比如蛇头向下的ID方向为U，因为蛇头向下时，开口方向是上方）
// 身体有两个开口方向，V和H表示纵向和横向
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



// 位图字体工具函数
// 获取字符（包括汉字）在指定字体中的位置
#define FONT_ERR_NOT_FOUND  9999
u16 FONT_getByteIndexAtFontData(u8 *chr, Font_Type *fontType, Font_Type **useFont);

#endif
