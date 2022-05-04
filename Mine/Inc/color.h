#ifndef __COLOR_H
#define __COLOR_H

#ifdef __COLOR_H
#define COLOR_BPP16_RGB555

typedef unsigned long       DWORD;
typedef u8       BYTE;
typedef u16        WORD;
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

#define RGB888toBGR555(r, g, b) ((WORD)(((((WORD)(b)) << 7) & 0x7C00) | ((((WORD)(g)) << 2) & 0x3E0) | (((WORD)(r)) >> 3)))

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

// // 常用16位BGR555颜色(GBA 的颜色系统 15位色 红色在低位 所以是BGR555)
// #define COLOR_RED          0x013f	  //红色
// #define COLOR_GREEN        0x03e0	  //绿色
// #define COLOR_GREENDK      RGB888toBGR555(10,100,0)
// #define COLOR_GREENLT      0x1f36
// #define COLOR_BLUE         0x1f	        //蓝色
// #define COLOR_BLUELT       0x7daf	    //浅蓝色
// #define COLOR_BLUEDK       0x2860	    //深蓝色
// #define COLOR_YELLOW       0x3eff	    //黄色
// #define COLOR_BLACK        0X0000	    //黑色
// #define COLOR_WHITE        0x7fff	    //白色
// #define COLOR_SKY          RGB888toBGR555(88,80,216)
// #define COLOR_SKYLT        0x7f33
// #define COLOR_BROWN        0x0bf1
// #define COLOR_BROWNLT      0x3cff
// #define COLOR_ORANGE       0x3d1f	    //橙色
// #define COLOR_GRAY  	   0x4210       //灰色
// #define COLOR_GRAYLT       0x6278	    //浅灰色
// #define COLOR_GRAYDK       0x41f0	    //深灰色
// #define COLOR_GRAYBLUE     0x696a       //灰蓝色
// #define COLOR_GRAYBLUELT   0x56d4       //浅灰蓝色
// #define COLOR_GRAYBLUEDK   0x3cc0 	    //深灰蓝色
// #define COLOR_PORPO        0x7c10	    //紫色
// #define COLOR_PINK         0x7f1f	    //粉红色
// #define COLOR_WINLOGO_R    RGB888toBGR555(240,101,43)
// #define COLOR_WINLOGO_G    RGB888toBGR555(140,189,0)
// #define COLOR_WINLOGO_B    RGB888toBGR555(49,174,233)
// #define COLOR_WINLOGO_Y    RGB888toBGR555(247,189,1)

//常用16位RGB565颜色
#define COLOR_RED          0XF800	  //红色
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
#define COLOR_PINK         0XF81F	  //粉红色
#define COLOR_WINLOGO_R    RGB888toRGB565(240,101,43)
#define COLOR_WINLOGO_G    RGB888toRGB565(140,189,0)
#define COLOR_WINLOGO_B    RGB888toRGB565(49,174,233)
#define COLOR_WINLOGO_Y    RGB888toRGB565(247,189,1)











#endif
