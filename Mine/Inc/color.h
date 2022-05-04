#ifndef __COLOR_H
#define __COLOR_H

#ifdef __COLOR_H
#define COLOR_BPP16_RGB555

typedef unsigned long       DWORD;
typedef unsigned char       BYTE;
typedef unsigned int        WORD;
typedef unsigned long       COLORREF;

/* Win RGB */
#define COLOR_RGB(r, g, b) ((COLORREF)(((BYTE)(r) | ((WORD)((BYTE)(g)) << 8)) | (((DWORD)(BYTE)(b)) << 16)))

#define COLOR_RGBA(r, g, b, a) ((COLORREF)((((DWORD)(BYTE)(a)) << 24) | COLOR_RGB(r, g, b)))

/* Win PALETTERGB */
#define COLOR_PALRGB(r, g, b) (0x02000000 | COLOR_RGB(r, g, b))

/* Win PALETTEINDEX */
#define COLOR_PALINDEX(i) ((COLORREF)(0x01000000 | (DWORD)(WORD)(i)))

/* Win GetRValue */
#define COLOR_GET_RED(rgb) ((BYTE)(((DWORD)(rgb)) & 0xff))

/* Win GetGValue */
#define COLOR_GET_GREEN(rgb) ((BYTE)(((DWORD)(((WORD)(rgb)) >> 8)) & 0xff))

/* Win GetBValue */
#define COLOR_GET_BLUE(rgb) ((BYTE)(((DWORD)((rgb) >> 16)) & 0xff))

#define COLOR_GET_ALPHA(rgba) ((BYTE)(((DWORD)((rgba) >> 24)) & 0xff))

/*
 * 24 bits color <=> 16 bits color
 */
#define RGB888toRGB565(r, g, b) ((WORD)((((WORD)(r) << 8) & 0xF800) | ((((WORD)(g)) << 3) & 0x7E0) | (((WORD)(b)) >> 3)))
#define RGBtoRGB565(rgb) ((WORD)(((((WORD)((rgb) >> 3)) & (0x1F)) << 11) | ((((WORD)((rgb) >> 10)) & (0x3F)) << 5) | (((WORD)((rgb) >> 19)) & 


#define RGB888toRGB555(r, g, b) ((WORD)(((((WORD)(r)) << 7) & 0x7C00) | ((((WORD)(g)) << 2) & 0x3E0) | (((WORD)(b)) >> 3)))
#define RGBtoRGB555(rgb) ((WORD)(((((WORD)((rgb) >> 3)) & (0x1F)) << 10) | ((((WORD)((rgb) >> 11)) & (0x1F)) << 5) | (((WORD)((rgb) >> 19)) & (0x1F))))
#define RGB555toRGB(rgb555) ((DWORD)(((BYTE)(((rgb555) >> 7) & 0xF8) | ((WORD)((BYTE)(((rgb555) >> 2) & 0xF8)) << 8)) | (((DWORD)(BYTE)(((rgb555) << 3) & 0xF8)) << 16)))
#define RGB565toRGB(rgb565) ((DWORD)(((BYTE)((((rgb565)&0xF800) >> 11) << 3) | ((WORD)((BYTE)((((rgb565)&0x07E0) >> 5) << 2)) << 8)) | (((DWORD)(BYTE)(((rgb565)&0x001F) << 3)) << 16)))

#ifdef COLOR_BPP16_RGB555
    #define BPP24toBPP16 RGB888toRGB555
    #define BPP16toBPP24 RGB555toRGB
#else
    #define BPP24toBPP16 RGB888toRGB565
    #define BPP16toBPP24 RGB565toRGB
#endif
#endif

// 常用16位RGB565颜色
#define COLOR_RED          0XF800	  //红色
#define COLOR_REDLT        RGB888toRGB565(248,45,87) //淡红
#define COLOR_GREEN        0X07E0	  //绿色
#define COLOR_GREENDK      RGB888toRGB565(10,100,0)
#define COLOR_GREENLT      0xb723
#define COLOR_BLUE         0X001F	  //蓝色
#define COLOR_BLUELT       0X7D7C	  //浅蓝色
#define COLOR_BLUEDK       0x00EA	  //深蓝色
#define COLOR_YELLOW       0XFFE0	  //黄色
#define COLOR_BLACK        0X0000	  //黑色
#define COLOR_WHITE        0XFFFF	  //白色
#define COLOR_SKY          RGB888toRGB565(88,80,216)
#define COLOR_SKYLT        0x9edd
#define COLOR_BROWN        0x8802
#define COLOR_BROWNLT      0xFBE4
#define COLOR_ORANGE       0XFC08	  //橙色
#define COLOR_GRAY  	   0X8430     //灰色
#define COLOR_GRAYLT       0XC618	  //浅灰色
#define COLOR_GRAYDK       0X8410	  //深灰色
#define COLOR_GRAYBLUE     0X5458     //灰蓝色
#define COLOR_GRAYBLUELT   0XA651     //浅灰蓝色
#define COLOR_GRAYBLUEDK   0X01CF	  //深灰蓝色
#define COLOR_PORPO        0X801F	  //紫色
#define COLOR_PORPOLT      RGB888toRGB565(200,83,255) //淡紫色
#define COLOR_PINK         0XF81F	  //粉红色
#define COLOR_WINLOGO_R    RGB888toRGB565(240,101,43)
#define COLOR_WINLOGO_G    RGB888toRGB565(140,189,0)
#define COLOR_WINLOGO_B    RGB888toRGB565(49,174,233)
#define COLOR_WINLOGO_Y    RGB888toRGB565(247,189,1)

// 扫雷网格线
#define COLOR_MINE_BLOCK_LINE           RGB888toRGB565(169,169,169)
// 按钮 未翻开左上高亮，右下阴影，中央凸起
#define COLOR_MINE_BLOCK_BTN_LT         RGB888toRGB565(247,247,247)
#define COLOR_MINE_BLOCK_BTN_RB         RGB888toRGB565(179,179,179)
#define COLOR_MINE_BLOCK_BTN_CENTER     RGB888toRGB565(230,230,230)
// 翻开后的底色
#define COLOR_MINE_BLOCK_DIGED          RGB888toRGB565(204,204,204)
// 数字颜色
#define COLOR_MINE_BLOCK_NUM_1          RGB888toRGB565(5,50,255)
#define COLOR_MINE_BLOCK_NUM_2          RGB888toRGB565(0,162,0)
#define COLOR_MINE_BLOCK_NUM_3          RGB888toRGB565(255,39,2)
#define COLOR_MINE_BLOCK_NUM_4          RGB888toRGB565(144,36,138)
#define COLOR_MINE_BLOCK_NUM_5          RGB888toRGB565(255,127,36)
#define COLOR_MINE_BLOCK_NUM_6          RGB888toRGB565(255,62,255)
#define COLOR_MINE_BLOCK_NUM_7          RGB888toRGB565(38,190,217)
#define COLOR_MINE_BLOCK_NUM_8          COLOR_BLACK













#endif
