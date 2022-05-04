#include "common.h"
#include "string.h"
#include "display.h"
#include "embMine.h"
#include "font.h"
#include "embMineDevice.h"

// 各种字符串定义，可自由修改
// 屏幕较小的话要改短一些,大屏也不要太长,预留的缓冲区只有128字节,超出的话会发生未知问题
u8* STR_PRESS_ANY_KEY             = (u8*)"  按任意键继续..";
u8* STR_DEMO_MENU                 = (u8*)"壹速度 貳声音 叁返回";  //壹貳叁代表按钮①②③图标 //Demo未使用
u8* STR_LEVEL_SEL                 = (u8*)"壹简单 貳普通 叁困难";

u8* STR_GAME_MENU1_LOCK           = (u8*)"壹锁";
u8* STR_GAME_MENU1_UNLOCK         = (u8*)"壹解";
u8* STR_GAME_MENU2                = (u8*)"貳旗";
u8* STR_GAME_MENU3                = (u8*)"叁挖";

u8* STR_GAMEOVER_GAMEOVER         = (u8*)"游戏结束";
u8* STR_GAMEOVER_NEWRECORD        = (u8*)"恭喜,刷新记录!";
u8* STR_GAMEOVER_HSCORE           = (u8*)"记录:";
u8* STR_GAMEOVER_SCORE            = (u8*)"用时:";

// 右侧信息栏元素位置
#define FRAME_RIGHT_INFO_WIDTH  75
#define TIME_X          250
#define TIME_Y           60
#define FLAG_X          248
#define FLAG_Y           10
#define MENU_X          250
#define MENU_LOCK_Y     130
#define MENU_FLAG_Y    165
#define MENU_CLICK_Y    200

// 框架厚度（drawFrame中计算并设置）
u8 frameThickness=1;

// 游戏区域左上角坐标Y偏移(也就是框架厚度)
u16 GAME_AREA_X_OFFSET=1;

// 游戏区域左上角坐标Y偏移（drawFrame中计算并设置）(去掉顶部框架和顶部信息区域以及上方横栏高度(也就是框架厚度))
u16 GAME_AREA_Y_OFFSET=0;

// 临时变量用(sprintf等),需要修改size的话请一并修改common.c中的同名变量,这里只是声明
extern u8 buff[128];

// 内部变量定义
u16 _ColorBack=0x0000, _ColorFore=0x0000;
#define COLOR_BG       _ColorBack
#define COLOR_FO       _ColorFore
u16 _ColorFrame=0x0000;
#define COLOR_FRAME    _ColorFrame

// 函数声明
void clearScreen(void);     // 清屏
Font_Type* showChar(u16 x, u16 y, u8 *chr, Font_Type *font, u16 colorBg, u16 colorFont);
void showStringMutiColor(u16 x, u16 y, u8 *str, Font_Type *font, u8 isClear, u16 colorBg, u8 colorCnt, u16 color1, u16 color2, u16 color3, u16 color4, u16 color5, u16 color6, u16 color7);
Font_Type* showMutiBitImg(u16 x, u16 y, u8 *chr, Font_Type *font, u16 color1, u16 color2, u16 color3, u16 color4, u16 color5, u16 color6, u16 color7);

// 获取随机颜色
u16 randRGB565(){
    u8 r=0, g=0, b=0;

    r = My_real_rand() % 10;
    #define RAND_COLOR My_real_rand()%50+200
    switch (r)
    {
    case 0:
        r = 0;
        g = RAND_COLOR;
        b = RAND_COLOR;
        break;
    case 1:
        r = RAND_COLOR;
        g = 0;
        b = RAND_COLOR;
        break;
    case 2:
        r = RAND_COLOR;
        g = RAND_COLOR;
        b = 0;
        break;
    case 3:
        r = RAND_COLOR;
        g = 0;
        b = 0;
        break;
    case 4:
        r = 0;
        g = RAND_COLOR;
        b = 0;
        break;
    case 5:
        r = 0;
        g = 0;
        b = RAND_COLOR;
        break;
    case 6:
        r = RAND_COLOR;
        g = RAND_COLOR;
        b = RAND_COLOR;
        break;
    default:
        r = My_real_rand();
        g = My_real_rand();
        b = My_real_rand();
        while (r + g + b < 300 || r + g + b > 600)
        {
            r = My_real_rand();
            g = My_real_rand();
            b = My_real_rand();
        }
        break;
    }
    return RGB888toRGB565(r, g, b);
}

u16 randRGB565DK(){
    u8 r=0, g=0, b=0;

    r = My_real_rand() % 10;
    #define RAND_COLOR My_real_rand()%50+200
    switch (r)
    {
    case 0:
        r = 0;
        g = RAND_COLOR;
        b = RAND_COLOR;
        break;
    case 1:
        r = RAND_COLOR;
        g = 0;
        b = RAND_COLOR;
        break;
    case 2:
        r = RAND_COLOR;
        g = RAND_COLOR;
        b = 0;
        break;
    case 3:
        r = RAND_COLOR;
        g = 0;
        b = 0;
        break;
    case 4:
        r = 0;
        g = RAND_COLOR;
        b = 0;
        break;
    case 5:
        r = 0;
        g = 0;
        b = RAND_COLOR;
        break;
    case 6:
        r = RAND_COLOR;
        g = RAND_COLOR;
        b = RAND_COLOR;
        break;
    default:
        r = My_real_rand();
        g = My_real_rand();
        b = My_real_rand();
        while (r + g + b > 300)
        {
            r = My_real_rand();
            g = My_real_rand();
            b = My_real_rand();
        }
        break;
    }
    return RGB888toRGB565(r, g, b);
}

void clearScreen(void){
    devFillRectange(0,0,SCREEN_W,SCREEN_H,COLOR_BG);
}

void DISP_drawAllMap(){
    u8 x,y;

    for (y = 0; y < MINE_SIZE_Y; y++)
    {
        for (x = 0; x < MINE_SIZE_X; x++)
        {
            DISP_updateGameBlock(x,y);
        }
    }
}

void showString(u16 x,u16 y, u8 *str,Font_Type *fontType, u16 colorBg, u16 colorFont)
{
	u8 j=0;
    u8 charWidth;
    Font_Type *lastUseFont;
    char* str1 = NULL;

	while (str[j]!='\0')
	{
        lastUseFont = showChar(x, y, &str[j], fontType, colorBg, colorFont);
        charWidth = lastUseFont->fontWidth;

		// 每输出一个字符或汉字后，向后位移一个字符或汉字的宽度
        x+= lastUseFont->fontWidth;
        // 自动换行
		if(x>SCREEN_W-fontType->fontWidth){
            x=0;
            y+=fontType->fontHeight;
        }
        // 继续显示下一个字符或汉字
		j++;
        // 如果是汉字那么需要向后移动两个字节
        if (lastUseFont->type == FONT_TYPE_HZ) j++;
	}
}

// 局部刷新函数，根据传入的游戏MAP指定位置，在画面上相应的区域进行重绘
// x,y : 游戏MAP坐标
// 
// 由于显示设备的画面刷新单位区域各不相同，有一些可以指定某一个点，
// 但有一些（比如这款12864的oled屏）是以字节为单位进行刷新的
// 所以，由于想刷新的区域所在的字节里也可能包含其他不应该被刷新的部分，
// 从而也需要取到这部分不应该被刷新的部分的数据，再加上这次真正想刷新的数据的部分，才能进行刷新，否则原来的数据就丢失了
void DISP_updateGameBlock(u8 x, u8 y){
    u8 blockView;
    u8 shiwei, gewei;
    u16 startX, startY;
    u8 width, height;
    u16 numColor;

    // 周围雷数
    u8 blockViewNum = MINE_getViewNum(x, y);

    blockView = MINE_getView(x, y);

    // 简单绘制方块 -------------------------------------------

    // 用位图字体绘制
    width = BLOCK_VIEW_FONT.fontWidth;
    height = BLOCK_VIEW_FONT.fontHeight;

    // 绘图开始坐标，原始坐标加上偏移坐标（游戏区域开始位置，在drawFrame函数中计算并设置）
    startX = x * width + GAME_AREA_X_OFFSET;
    startY = y * height + GAME_AREA_Y_OFFSET;

    #if ISDEBUG
    //LOG("DISP_updateGameBlock 1 fontWidth=%b2d, fontHeight=%b2d\r\n", width, height);
    #endif
    
    // 根据view绘图
    if (blockView == MINE_VIEW_NORMAL)
    {
        // 未翻开
        showMutiBitImg(startX, startY, FONT_MINE_BLOCK_BG23_BTN, &FONT_IMG_2BIT_BLOCK_BG23, 
                        COLOR_MINE_BLOCK_BTN_CENTER, COLOR_MINE_BLOCK_BTN_RB, COLOR_MINE_BLOCK_BTN_LT, COLOR_MINE_BLOCK_LINE,0,0,0);
    }
    else if (blockView == MINE_VIEW_DIGED)
    {
        // 已翻开
        //devFillRectange(startX+1, startY+1, 22, 22, COLOR_MINE_BLOCK_DIGED);
        showMutiBitImg(startX, startY, FONT_MINE_BLOCK_BG23_DIGED, &FONT_IMG_2BIT_BLOCK_BG23, 
                        COLOR_MINE_BLOCK_DIGED, COLOR_MINE_BLOCK_DIGED, COLOR_MINE_BLOCK_DIGED, COLOR_MINE_BLOCK_LINE,0,0,0);
        if (blockViewNum != 0)
        {
            switch (blockViewNum)
            {
            case 1:
                numColor = COLOR_MINE_BLOCK_NUM_1;
                break;
            case 2:
                numColor = COLOR_MINE_BLOCK_NUM_2;
                break;
            case 3:
                numColor = COLOR_MINE_BLOCK_NUM_3;
                break;
            case 4:
                numColor = COLOR_MINE_BLOCK_NUM_4;
                break;
            case 5:
                numColor = COLOR_MINE_BLOCK_NUM_5;
                break;
            case 6:
                numColor = COLOR_MINE_BLOCK_NUM_6;
                break;
            case 7:
                numColor = COLOR_MINE_BLOCK_NUM_7;
                break;
            case 8:
                numColor = COLOR_MINE_BLOCK_NUM_8;
                break;
            default:
                numColor = COLOR_MINE_BLOCK_NUM_8;
                break;
            }
            // 画数字(数字正好对应数组中的位置)
            showMutiBitImg(startX+1, startY+1, (u8 *)blockViewNum, &FONT_IMG_2BIT_BLOCK_MINE_AND_NUMS22, 
                        numColor, COLOR_MINE_BLOCK_DIGED, 0, 0,0,0,0);
        }
    }
    else if (blockView == MINE_VIEW_FLAG)
    {
        // 插旗
        showMutiBitImg(startX + 4, startY + 4, FONT_MINE_BLOCK_FLAG16, &FONT_IMG_2BIT_BLOCK_FLAG16, 
                        COLOR_MINE_BLOCK_BTN_CENTER, COLOR_RED, 0, COLOR_BLACK,0,0,0);
    }
    else if (blockView == MINE_VIEW_MINE)
    {
        // 挖到雷时，显示其他未插旗的雷
        showMutiBitImg(startX+1, startY+1, FONT_MINE_BLOCK_MINE, &FONT_IMG_2BIT_BLOCK_MINE_AND_NUMS22, 
                        COLOR_WHITE, COLOR_MINE_BLOCK_DIGED, 0, COLOR_BLACK,0,0,0);
    }
    else if (blockView == MINE_VIEW_DEADMINE)
    {
        // 挖到雷时，显示爆炸的雷
        showMutiBitImg(startX+1, startY+1, FONT_MINE_BLOCK_MINE, &FONT_IMG_2BIT_BLOCK_MINE_AND_NUMS22, 
                        COLOR_WHITE, COLOR_RED, 0, COLOR_BLACK,0,0,0);
    }
    else if (blockView == MINE_VIEW_WRONG) {
        // 挖到雷时，显示被并不是雷但被错插上旗子的地方
        showMutiBitImg(startX+1, startY+1, FONT_MINE_BLOCK_MINE_WRONG, &FONT_IMG_2BIT_BLOCK_MINE_AND_NUMS22, 
                        COLOR_WHITE, COLOR_MINE_BLOCK_DIGED, COLOR_RED, COLOR_BLACK,0,0,0);
    }
    else {
    }
}

//计算字符串在在指定字体下占用的实际像素宽度(不考虑换行的问题)
u16 calcStringWidth(u8 *str, Font_Type *fontType){
    u8 j=0;
    u16 width=0, asciiWidth=0, hzWidth=0;

    #if ISDEBUG
    //LOG("calcStringWidth: str=%s\r\n", str);
    #endif

    if (fontType->type == FONT_TYPE_HZ) {
        hzWidth = fontType->fontWidth;
        asciiWidth = ((Font_Type *)fontType->asciiZk)->fontWidth; // 使用汉字字体时，如果存在半角文字会显示外链的半角字体
    } else {
        asciiWidth = fontType->fontWidth;
        hzWidth = asciiWidth*2; // 使用ascii字体时，如果存在汉字会显示2个半角的X
    }
    while (str[j]!='\0')
	{
        if (str[j] >= 0x20 && str[j] <= 0x7E) {
            // 半角
            width += asciiWidth;
            j++;
        } else {
            // 全角
            width += hzWidth;
            j+=2;
        }
        //LOG("  calcStringWidth 5 in while...\r\n");
    }

    #if ISDEBUG
    //LOG("calcStringWidth: width=%hd done.\r\n", width);
    #endif

    return width;
}

// 显示多颜色字体(最多7种颜色)
Font_Type* showCharMutiColor(u16 x, u16 y, u8 *chr, Font_Type *font, u16 colorBg, 
                            u8 oneBitMutiColorCnt, u16 color1, u16 color2, u16 color3, u16 color4, u16 color5, u16 color6, u16 color7){
    // 根据传入的chr和字体找到这个chr的fontData数组和其在数组中的偏移
    u8 nowRow=0,nowCol=0,fontWidth=0,fontHeight=0;
    u8 bytePerRow,bitIndex=0,charByteCnt=0;
    u16 charDataStartIndex=0;
    u8* fontData;
    u16 errSwitchColorTmp;
    u8 bitPerPixel;
    u8 bitIdxInPixel;
    u8 bitsValue;
    u16 nowColor;
    Font_Type *useFont = NULL;

    // 获取当前字符/汉字的偏移index 以及 实际使用的Font(例如给定的字体是汉字字库，但指定的字符是半角字符，那么实际使用的font就是汉字字库里外链的半角字库)
    charDataStartIndex = FONT_getByteIndexAtFontData(chr, font, &useFont);
    // 点阵字体数组
    fontData = useFont->fontArray;
    // 点阵字体宽度
    fontWidth = useFont->fontWidth;
    // 点阵字体高度
    fontHeight = useFont->fontHeight;
    // 颜色位深
    bitPerPixel = useFont->bitPerPixel;
    // 每行在字体数组中占用几个字节(像素点数x位深)
    bytePerRow = (fontWidth * bitPerPixel-1)/8+1;

    if(charDataStartIndex == FONT_ERR_NOT_FOUND){
        // 如果在字库中没找到这个字, 试图显示一个半角的X并反色显示提示错误
        charDataStartIndex = 'X'-' ';
        errSwitchColorTmp = colorBg;
        colorBg = color1;
        color1 = errSwitchColorTmp;
    }

	devPrepareForDrawArea(x, y, fontWidth, fontHeight);

    // 如果参数指定了单位多色个数 并且 使用的字体确实是1位色深的 那么使用传入的多个颜色在Y方向上均分显示
    if (oneBitMutiColorCnt > 1 && bitPerPixel == 1)
    {
        // 多色文字
        for(nowRow=0;nowRow<fontHeight;nowRow++){
            if (oneBitMutiColorCnt > 6 && nowRow > (fontHeight/oneBitMutiColorCnt * 6)){
                nowColor = color7;
            } else if (oneBitMutiColorCnt > 5 && nowRow > (fontHeight/oneBitMutiColorCnt * 5)) {
                nowColor = color6;
            } else if (oneBitMutiColorCnt > 4 && nowRow > (fontHeight/oneBitMutiColorCnt * 4)) {
                nowColor = color5;
            } else if (oneBitMutiColorCnt > 3 && nowRow > (fontHeight/oneBitMutiColorCnt * 3)) {
                nowColor = color4;
            } else if (oneBitMutiColorCnt > 2 && nowRow > (fontHeight/oneBitMutiColorCnt * 2)) {
                nowColor = color3;
            } else if (oneBitMutiColorCnt > 1 && nowRow > (fontHeight/oneBitMutiColorCnt * 1)) {
                nowColor = color2;
            } else {
                nowColor = color1;
            }

            for(nowCol=0;nowCol<fontWidth;nowCol++){

                // 根据行列位置确定当前应该使用哪个字节哪个位
                bitIndex = nowCol%8;

                // 按位取点阵内容。1：绘制字体色，0：绘制背景色
                if ((fontData[charDataStartIndex+(bytePerRow*nowRow) + nowCol/8] << bitIndex) & 0x80)
                {
                    devPointInDrawArea(nowColor);
                }else{
                    devPointInDrawArea(colorBg);
                }
            }
        }
        
    } 
    // 如果指定字体是多位色深的那么使用多位色深模式，此时无视参数oneBitMutiColorCnt
    else if (bitPerPixel > 1) {
        for(nowRow=0;nowRow<fontHeight;nowRow++){
            for(nowCol=0;nowCol<fontWidth;nowCol++){

                // 根据行列位置确定当前应该使用哪个字节哪个位
                bitIndex = (nowCol*bitPerPixel)%8;

                // 取得该像素对应的多位数据
                bitsValue = 0;
                for (bitIdxInPixel = 0; bitIdxInPixel < bitPerPixel; bitIdxInPixel++)
                {
                    if ((fontData[charDataStartIndex+(bytePerRow*nowRow) + (nowCol*bitPerPixel)/8] << (bitIndex+bitIdxInPixel)) & 0x80){
                        bitsValue |= (0x01<<bitIdxInPixel);
                    }
                }

                // LOG("nowCol=%b3u,dataIndex=%3d,bitsValue=%b2d, data=0x%b2x\r\n", 
                //             nowCol,
                //             charDataStartIndex+(bytePerRow*nowRow) + (nowCol*bitPerPixel)/8, 
                //             bitsValue,
                //             fontData[charDataStartIndex+(bytePerRow*nowRow) + (nowCol*bitPerPixel)/8]
                //             );

                // 根据多位数据绘制不同的颜色，比如2位深每个像素可能的取值有4种，分别对应 color1-color4 4种颜色
                switch (bitsValue)
                {
                case 0:
                    devPointInDrawArea(color1);
                    break;
                case 1:
                    devPointInDrawArea(color2);
                    break;
                case 2:
                    devPointInDrawArea(color3);
                    break;
                case 3:
                    devPointInDrawArea(color4);
                    break;
                case 4:
                    devPointInDrawArea(color5);
                    break;
                case 5:
                    devPointInDrawArea(color6);
                    break;
                case 6:
                    devPointInDrawArea(color7);
                    break;
                default:
                    devPointInDrawArea(color1);
                    break;
                }
            }
        }
    }
    // 否则使用单色文字(使用color1)
    else {
        for(nowRow=0;nowRow<fontHeight;nowRow++){
            for(nowCol=0;nowCol<fontWidth;nowCol++){

                // 根据行列位置确定当前应该使用哪个字节哪个位
                bitIndex = nowCol%8;

                // 按位取点阵内容。1：绘制字体色，0：绘制背景色
                if ((fontData[charDataStartIndex+(bytePerRow*nowRow) + nowCol/8] << bitIndex) & 0x80)
                {
                    devPointInDrawArea(color1);
                }else{
                    devPointInDrawArea(colorBg);
                }
            }
        }
    }

    return useFont;
}

Font_Type* showChar(u16 x, u16 y, u8 *chr, Font_Type *font, u16 colorBg, u16 colorFont){

    return showCharMutiColor(x, y, chr, font, colorBg, 1, colorFont,0,0,0,0,0,0);
}

Font_Type* showMutiBitImg(u16 x, u16 y, u8 *chr, Font_Type *font, u16 color1, u16 color2, u16 color3, u16 color4, u16 color5, u16 color6, u16 color7){

    return showCharMutiColor(x, y, chr, font, 0, 0, color1, color2, color3, color4, color5, color6, color7 );
}

//居中显示字符串,指定颜色
void showStringCenterColor(u16 y, u8 *str, Font_Type *fontType, u16 colorBg, u16 colorFo, u8 isClear){
    u16 strWidth;

    #if ISDEBUG
    //LOG("showStringCenterColor: %s\r\n", str);
    #endif

    strWidth = calcStringWidth(str, fontType);
    
    if (isClear)
    {
        devFillRectange((SCREEN_W - strWidth) / 2, y, strWidth, fontType->fontHeight, colorBg);
    } else {
        //showString((SCREEN_W - strWidth) / 2, y, str, fontType, colorBg, colorFo);
        showStringMutiColor((SCREEN_W - strWidth) / 2, y, str, fontType, FALSE, colorBg, 1, colorFo,0,0,0,0,0,0);
    }

    #if ISDEBUG
    //LOG("showStringCenterColor: %s done.\r\n", str);
    #endif
}

//居中显示字符串
void showStringCenter(u16 y, u8 *str, Font_Type *fontType, u8 isClear){
    showStringCenterColor(y, str, fontType, COLOR_BG, COLOR_FO, isClear);
}

// 显示多颜色字符串
void showStringMutiColor(u16 x, u16 y, u8 *str, Font_Type *font, u8 isClear, u16 colorBg, 
                            u8 colorCnt, u16 color1, u16 color2, u16 color3, u16 color4, u16 color5, u16 color6, u16 color7
                            ){
    u8 j=0;
    u8 charWidth;
    Font_Type *lastUseFont;
    char* str1 = NULL;
    u16 strWidth = calcStringWidth(str, font);

    if (isClear)
    {
        devFillRectange(x, y, strWidth, font->fontHeight, colorBg);
        return;
    }

	while (str[j]!='\0')
	{
        if (isClear)
        {
            lastUseFont = showCharMutiColor(x, y, &str[j], font, colorBg, 1, colorBg, colorBg, colorBg, colorBg, colorBg, colorBg, colorBg);
        } else {
            lastUseFont = showCharMutiColor(x, y, &str[j], font, colorBg, colorCnt, color1, color2, color3, color4, color5, color6, color7);
        }

        charWidth = lastUseFont->fontWidth;

		// 每输出一个字符或汉字后，向后位移一个字符或汉字的宽度
        x+= lastUseFont->fontWidth;
        // 自动换行
		if(x>SCREEN_W-font->fontWidth){
            x=0;
            y+=font->fontHeight;
        }
        // 继续显示下一个字符或汉字
		j++;
        // 如果是汉字那么需要向后移动两个字节
        if (lastUseFont->type == FONT_TYPE_HZ) j++;
	}
}

// 居中显示多颜色字符串（适用1位深字体）
void showStringCenterMutiColor(u16 y, u8 *str, Font_Type *font, u8 isClear, u16 colorBg, 
                            u8 colorCnt, u16 color1, u16 color2, u16 color3, u16 color4, u16 color5, u16 color6, u16 color7
                            ){
    u16 strWidth = calcStringWidth(str, font);
    if (isClear)
    {
        devFillRectange((SCREEN_W - strWidth) / 2, y, strWidth, font->fontHeight, colorBg);
    }else{
        showStringMutiColor((SCREEN_W - strWidth) / 2, y, str, font, isClear, colorBg, colorCnt, color1, color2, color3, color4, color5, color6, color7);
    }
}

void DISP_setBackColor(u16 color){
    _ColorBack = color;
}

void DISP_setForeColor(u16 color){
    _ColorFore = color;
}

void DISP_setFrameColor(u16 color){
    _ColorFrame = color;
}

// ########### 绘制各画面 ######################################################################################################
// 一般分成3种类型的函数
// 1-各画面的初始化函数。只会在进入该画面时被调用一次，可用来绘制画面上不会变化的内容
// 2-带有参数flashOnOff的函数。会被定时调用，flashOnOff 会在1和0之间来回切换，自己根据需要刷新画面上的动态内容，比如闪烁文字等
// 3-其他的不带flashOnOff的函数。各有各的用途，一般在特定事件发生时被调用并会传入相关数据，比如吃到食物的时候会传入最新的分数。

// ##### 首页 初始化时被调用一次(isStartUp：1=上电后第一次调用，0=非上电后第一次调用)
void DISP_drawWelcome(u8 isStartUp){
    u16 titleStartX, titleY, logoX, logoY;
    u16 r=50;

    devScreenOFF();

    clearScreen();

    #if ISDEBUG && DEBUG_SKIP_HOME
        //调试模式并跳过主画面
        return;
    #endif

    // 标题
    titleY = SCREEN_H/6;
    showStringCenterColor(titleY, 0, &FONTHZ_TITLE64, COLOR_BG, COLOR_YELLOW, 0);
    devFillRectange(0, titleY, SCREEN_W, FONTHZ_TITLE64.fontHeight, COLOR_GRAYLT);
    showMutiBitImg((SCREEN_W-FONTHZ_TITLE64.fontWidth) / 2, titleY, 0, &FONTHZ_TITLE64, COLOR_GRAYLT, COLOR_GRAYDK, COLOR_GRAYDK, COLOR_BLACK,0,0,0);

    devScreenON();

    // 学电LOGO
    logoX = (SCREEN_W - FONTHZ_XD_LOGO40.fontWidth) / 2;
    logoY = titleY + FONTHZ_TITLE64.fontHeight + FONTHZ_XD_LOGO40.fontHeight / 2;
//TODO:Fortest
//isStartUp = 0;
    if (isStartUp)
    {
        delay_ms(500);
        // 学电LOGO缓缓出现的动画效果
        while(1){
            r+=5;
            if (r>=256) break;
            showChar(logoX, logoY, FONT_HZ_XD_LOGO, &FONTHZ_XD_LOGO40, RGB888toRGB565(r, 0, 0), COLOR_BG);
        }
        // 出现后等待一会儿
        Pre_Delay_ms(300);
    } else {
        // 非上电画面，快速显示
        // 学电LOGO缓缓出现的动画效果
        while(r<255){
            r+=30;
            if (r>=256) r = 255;
            showChar(logoX, logoY, FONT_HZ_XD_LOGO, &FONTHZ_XD_LOGO40, RGB888toRGB565(r, 0, 0), COLOR_BG);
        }
        // 出现后等待一会儿
        //Pre_Delay_ms(500);
    }

    // 难度选择
    showStringCenter(logoY + FONTHZ_XD_LOGO40.fontHeight + FONTHZ_XD_LOGO40.fontHeight / 2, STR_LEVEL_SEL, &FONTHZ32, 0);
}

// ##### 首页 定期被调用
void DISP_flashWelcome(u8 flashOnOff){
    u16 logoX, logoY;

    u8 tmp=0;

    #if ISDEBUG
    LOG("  DISP_flashWelcome 1: call showStringCenter(str=%s pStr=%ld)\r\n", STR_PRESS_ANY_KEY, &STR_PRESS_ANY_KEY);
    #endif
    
    // 学电LOGO变幻颜色
    logoX = (SCREEN_W - FONTHZ_XD_LOGO40.fontWidth) / 2;
    logoY = SCREEN_H/6 + FONTHZ_TITLE64.fontHeight + FONTHZ_XD_LOGO40.fontHeight / 2;
    showChar(logoX, logoY, FONT_HZ_XD_LOGO, &FONTHZ_XD_LOGO40, randRGB565(), COLOR_BG);
}

// ##### Demo页 初始化时被调用一次
void DISP_drawDemo(u8 soundOnOff){
    // NOTHING
}

// ##### Demo页 吃到食物时被调用一次，更新画面上的分数
void DISP_updateDemoScore(u16 maxDemoScore, u16 nowDemoScore){
    // DONOTHING
}

// ##### Demo页 死亡时被调用一次，可做一些显示处理，目前什么都没做
void DISP_updateDemoGameover(u16 maxDemoScore, u16 avgDemoScore, u16 lastDemoScore){
    // DONOTHING
}

// ##### Demo页 定期被调用 目前没用到
void DISP_flashDemo(u8 flashOnOff){
    // DONOTHING
}

// ##### Game页 初始化时被调用一次
void DISP_drawGame(u8 soundOnOff, u8 flagCnt){

    #if ISDEBUG
    LOG("DISP_drawGame\r\n");
    #endif

    clearScreen();
    DISP_drawFrame();

    // 绘制右侧信息栏
    // 注意，这句计算必须放在DISP_drawFrame的后面。否则第一次计算时还未被设置正确的数值，为默认值1。
    // 误以为1是正确的值调整后面的位置第一次是对的，第二次就全错了。
    //topInfoY = frameThickness + 2;

    // 版权
    showString(SCREEN_W - FRAME_RIGHT_INFO_WIDTH+3, 100, "Chen.L", &FONT20, COLOR_FO, COLOR_BG);

    // 初始时间
    DISP_updateTime(0);

    // 初始旗子数
    // 画旗子
    devFillRectange(SCREEN_W - FRAME_RIGHT_INFO_WIDTH - frameThickness, frameThickness, FRAME_RIGHT_INFO_WIDTH, 45, COLOR_FO );
    showChar(FLAG_X, FLAG_Y, "旗", &FONTHZ32, COLOR_FO, COLOR_BG);
    DISP_updateFlagCnt(flagCnt);

    // 菜单
    DISP_drawUnLock();
    DISP_drawSound(1);
    showString(MENU_X, MENU_CLICK_Y, STR_GAME_MENU3, &FONTHZ32, COLOR_BG, COLOR_FO);

    // 绘制声音状态
    DISP_drawSound(soundOnOff);

    #if ISDEBUG
    LOG("DISP_drawGame done.\r\n");
    #endif
}

// ##### Game页 更新画面上的时间
void DISP_updateTime(u16 sec){

    sprintf(buff, "%02d", sec / 60);
    //sprintf(buff, "%b02d", 16);
    showString(TIME_X-4, TIME_Y, buff, &FONT_DIGIT23, COLOR_BG, COLOR_RED);
    sprintf(buff, "%s", ":");
    showString(TIME_X+26, TIME_Y, buff, &FONT_DIGIT23, COLOR_BG, COLOR_RED);
    sprintf(buff, "%02d", sec % 60);
    //sprintf(buff, "%b02d", 27);
    showString(TIME_X+32, TIME_Y, buff, &FONT_DIGIT23, COLOR_BG, COLOR_RED);
}

void DISP_updateFlagCnt(u8 flagCnt){

    // 更新旗子个数
    sprintf(buff, "%b02d", flagCnt);
    showString(FLAG_X + 30, FLAG_Y + 5, buff, &FONT32, COLOR_FO, COLOR_BG);
}

// ##### BeforeGameOver页 定期被调用
void DISP_flashBeforeGameOver(u8 flashOnOff, u8 isWin){
    // 闪烁文字 按任意键继续
    if (flashOnOff)
    {
        showString(MENU_X, MENU_LOCK_Y, "按任", &FONTHZ32, COLOR_BG, COLOR_FO);
        showString(MENU_X, MENU_FLAG_Y, "意键", &FONTHZ32, COLOR_BG, COLOR_FO);
        showString(MENU_X, MENU_CLICK_Y, "继续", &FONTHZ32, COLOR_BG, COLOR_FO);
    } else {
        devFillRectange(MENU_X, MENU_LOCK_Y, FONTHZ32.fontWidth * 2, MENU_CLICK_Y - MENU_LOCK_Y + FONTHZ32.fontHeight, COLOR_BG);
    }
}

// ##### GameOver页 初始化时被调用一次
void DISP_drawGameOver(u8 isWin, u8* levelStr, u16 score, u16 hiScore){
    u8 buff1[32];
    clearScreen();

    #define  TITLE_Y         35
    #define  SCROE_Y         80
    #define HSCROE_Y        120

    //showStringCenter(5, "__________", &FONT32, 0);

    // 标题，如果超过最高分，显示 刷新记录 否则 显示游戏结束
    devFillRectange(0, TITLE_Y, SCREEN_W, FONTHZ32.fontHeight, COLOR_FO);
    if (isWin && score < hiScore)
    {
        sprintf(buff1, "%s-%s", STR_GAMEOVER_NEWRECORD, levelStr);
        showStringCenterMutiColor(TITLE_Y, buff1, &FONTHZ32, 0, COLOR_FO, 
            7, 
            COLOR_RED, COLOR_RED, COLOR_REDLT, COLOR_REDLT, COLOR_ORANGE, COLOR_ORANGE, COLOR_YELLOW);
    } else {
        sprintf(buff1, "%s-%s", STR_GAMEOVER_GAMEOVER, levelStr);
        showStringCenterColor(TITLE_Y, buff1, &FONTHZ32, COLOR_FO, COLOR_BLACK, 0);
    }
    

    // 得分
    if (isWin)
    {
        sprintf(buff1, "%s %02d:%02d", STR_GAMEOVER_SCORE, score/60, score%60);
    } else {
        // 没有得分
        sprintf(buff1, "%s --:--", STR_GAMEOVER_SCORE);
    }
    showStringCenter(SCROE_Y, buff1, &FONTHZ32, 0);
    
    // 记录
    sprintf(buff1, "%s %02d:%02d", STR_GAMEOVER_HSCORE, hiScore/60, hiScore%60);
    showStringCenter(HSCROE_Y, buff1, &FONTHZ32, 0);

    showStringCenter(150, "__________", &FONT32, 0);
}

// ##### GameOver页 定期被调用
void DISP_flashGameOver(u8 flashOnOff, u8 isNewRecord, u8* levelStr){

    if (isNewRecord)
    {
        // // 刷新记录动态效果
        // sprintf(buff, "%s-%s", STR_GAMEOVER_NEWRECORD, levelStr);
        // showStringCenterColor(35, buff, &FONTHZ32, COLOR_FO, randRGB565DK(), 0);
    }

    // 闪烁文字 按任意键继续...
    showStringCenter(190, STR_PRESS_ANY_KEY, &FONTHZ32, flashOnOff);
}

// 绘制框架
void DISP_drawFrame() {
    u16 barTopY, barRightX;

    // 边框厚度
    frameThickness = 5;

    // 游戏区域右方竖栏位置Y = 屏幕宽 - 右方边框高 - 右方信息栏高 - 右方竖栏高(也是边框厚度) 
    // 加1的目的是为了留出右侧的一像素的空间用来绘制选中状态的外框
    barRightX = SCREEN_W - frameThickness - FRAME_RIGHT_INFO_WIDTH - frameThickness + 1;

    // 开始绘制(实心矩形拼接)
    devFillRectange(0, 0, SCREEN_W, frameThickness, COLOR_FRAME);                           // 顶
    devFillRectange(0, 0, frameThickness, SCREEN_H, COLOR_FRAME);                           // 左
    devFillRectange(SCREEN_W - frameThickness, 0, frameThickness, SCREEN_H, COLOR_FRAME);   // 右
    devFillRectange(barRightX, 0, frameThickness, SCREEN_H, COLOR_FRAME);                   // 右竖栏
    devFillRectange(SCREEN_W - frameThickness - FRAME_RIGHT_INFO_WIDTH, MENU_LOCK_Y - 35, FRAME_RIGHT_INFO_WIDTH, 30, COLOR_FRAME);    // 右横栏
    devFillRectange(0, SCREEN_H - frameThickness+1, SCREEN_W, frameThickness-1, COLOR_FRAME);   // 底

    // 最后绘制雷区 最右侧 和 最底部 缺少的一个像素宽的边框线
    devDrawLine(frameThickness, SCREEN_H-frameThickness, barRightX - 1, SCREEN_H-frameThickness, 1, COLOR_MINE_BLOCK_LINE);
    devDrawLine(barRightX - 1, frameThickness, barRightX - 1, SCREEN_H-frameThickness, 1, COLOR_MINE_BLOCK_LINE);

    // 游戏区域Y偏移
    GAME_AREA_Y_OFFSET = frameThickness;
    GAME_AREA_X_OFFSET = frameThickness;
}

// 绘制光标
void drawCusor(u8 x, u8 y, u16 color){
    u16 x1,y1,x2,y2;
    #define CUSOR_LEN 9

    x1 = x*BLOCK_VIEW_FONT.fontWidth + GAME_AREA_X_OFFSET;
    y1 = y*BLOCK_VIEW_FONT.fontHeight + GAME_AREA_Y_OFFSET;
    x2 = x1+BLOCK_VIEW_FONT.fontWidth;
    y2 = y1+BLOCK_VIEW_FONT.fontHeight;
    devDrawLine(x1, y1, x1+CUSOR_LEN, y1, 1, color);
    devDrawLine(x1, y1, x1, y1+CUSOR_LEN, 1, color);
    
    devDrawLine(x1, y2, x1+CUSOR_LEN, y2, 1, color);
    devDrawLine(x1, y2, x1, y2-CUSOR_LEN, 1, color);

    devDrawLine(x2, y1, x2-CUSOR_LEN, y1, 1, color);
    devDrawLine(x2, y1, x2, y1+CUSOR_LEN, 1, color);
    
    devDrawLine(x2, y2, x2-CUSOR_LEN, y2, 1, color);
    devDrawLine(x2, y2, x2, y2-CUSOR_LEN, 1, color);
}

// 隐藏光标
void DISP_hideCusor(u8 x, u8 y){
    drawCusor(x, y, COLOR_MINE_BLOCK_LINE);
}

// 显示光标     
void DISP_showCusor(u8 x, u8 y){
    drawCusor(x, y, COLOR_RED);
}

// 声音和静音状态
void DISP_drawSound(u8 soundOnOff) {
    if (soundOnOff)
    {
        showString(MENU_X, MENU_FLAG_Y, STR_GAME_MENU2, &FONTHZ32, COLOR_BG, COLOR_FO);
    } else {
        // 绘制静音
        devDrawLine(MENU_X+27, MENU_FLAG_Y+2, MENU_X+51, MENU_FLAG_Y+28, 3, COLOR_RED);
    }
    
}

void DISP_drawLock() {
    showString(MENU_X, MENU_LOCK_Y, STR_GAME_MENU1_LOCK, &FONTHZ32, COLOR_BG, COLOR_RED);
}

void DISP_drawUnLock() {
    showString(MENU_X, MENU_LOCK_Y, STR_GAME_MENU1_UNLOCK, &FONTHZ32, COLOR_BG, COLOR_FO);
}

// 初始化
void DISP_init(void){
    // u32 clock;
    // u16 clockH;
    // u16 clockL;

    devDisplayInit(COLOR_BG);

    // 测试打印出系统时钟
    // clock=Get_SysClk_FRE();
    // clockH = clock >> 16;
    // clockL = clock & 0xFFFF;
    // sprintf(buff, "H:%u  L:%u", clockH, clockL);
    // showString(0,0,buff,&FONTHZ32,COLOR_BLACK, COLOR_WHITE);
    // while (1);
    
}
