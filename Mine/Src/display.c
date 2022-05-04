#include "common.h"
#include "string.h"
#include "display.h"
#include "embMine.h"
#include "font.h"
#include "embMineDevice.h"

// �����ַ������壬�������޸�
// ��Ļ��С�Ļ�Ҫ�Ķ�һЩ,����Ҳ��Ҫ̫��,Ԥ���Ļ�����ֻ��128�ֽ�,�����Ļ��ᷢ��δ֪����
u8* STR_PRESS_ANY_KEY             = (u8*)"  �����������..";
u8* STR_DEMO_MENU                 = (u8*)"Ҽ�ٶ� �E���� ������";  //Ҽ�E������ť�٢ڢ�ͼ�� //Demoδʹ��
u8* STR_LEVEL_SEL                 = (u8*)"Ҽ�� �E��ͨ ������";

u8* STR_GAME_MENU1_LOCK           = (u8*)"Ҽ��";
u8* STR_GAME_MENU1_UNLOCK         = (u8*)"Ҽ��";
u8* STR_GAME_MENU2                = (u8*)"�E��";
u8* STR_GAME_MENU3                = (u8*)"����";

u8* STR_GAMEOVER_GAMEOVER         = (u8*)"��Ϸ����";
u8* STR_GAMEOVER_NEWRECORD        = (u8*)"��ϲ,ˢ�¼�¼!";
u8* STR_GAMEOVER_HSCORE           = (u8*)"��¼:";
u8* STR_GAMEOVER_SCORE            = (u8*)"��ʱ:";

// �Ҳ���Ϣ��Ԫ��λ��
#define FRAME_RIGHT_INFO_WIDTH  75
#define TIME_X          250
#define TIME_Y           60
#define FLAG_X          248
#define FLAG_Y           10
#define MENU_X          250
#define MENU_LOCK_Y     130
#define MENU_FLAG_Y    165
#define MENU_CLICK_Y    200

// ��ܺ�ȣ�drawFrame�м��㲢���ã�
u8 frameThickness=1;

// ��Ϸ�������Ͻ�����Yƫ��(Ҳ���ǿ�ܺ��)
u16 GAME_AREA_X_OFFSET=1;

// ��Ϸ�������Ͻ�����Yƫ�ƣ�drawFrame�м��㲢���ã�(ȥ��������ܺͶ�����Ϣ�����Լ��Ϸ������߶�(Ҳ���ǿ�ܺ��))
u16 GAME_AREA_Y_OFFSET=0;

// ��ʱ������(sprintf��),��Ҫ�޸�size�Ļ���һ���޸�common.c�е�ͬ������,����ֻ������
extern u8 buff[128];

// �ڲ���������
u16 _ColorBack=0x0000, _ColorFore=0x0000;
#define COLOR_BG       _ColorBack
#define COLOR_FO       _ColorFore
u16 _ColorFrame=0x0000;
#define COLOR_FRAME    _ColorFrame

// ��������
void clearScreen(void);     // ����
Font_Type* showChar(u16 x, u16 y, u8 *chr, Font_Type *font, u16 colorBg, u16 colorFont);
void showStringMutiColor(u16 x, u16 y, u8 *str, Font_Type *font, u8 isClear, u16 colorBg, u8 colorCnt, u16 color1, u16 color2, u16 color3, u16 color4, u16 color5, u16 color6, u16 color7);
Font_Type* showMutiBitImg(u16 x, u16 y, u8 *chr, Font_Type *font, u16 color1, u16 color2, u16 color3, u16 color4, u16 color5, u16 color6, u16 color7);

// ��ȡ�����ɫ
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

		// ÿ���һ���ַ����ֺ����λ��һ���ַ����ֵĿ��
        x+= lastUseFont->fontWidth;
        // �Զ�����
		if(x>SCREEN_W-fontType->fontWidth){
            x=0;
            y+=fontType->fontHeight;
        }
        // ������ʾ��һ���ַ�����
		j++;
        // ����Ǻ�����ô��Ҫ����ƶ������ֽ�
        if (lastUseFont->type == FONT_TYPE_HZ) j++;
	}
}

// �ֲ�ˢ�º��������ݴ������ϷMAPָ��λ�ã��ڻ�������Ӧ����������ػ�
// x,y : ��ϷMAP����
// 
// ������ʾ�豸�Ļ���ˢ�µ�λ���������ͬ����һЩ����ָ��ĳһ���㣬
// ����һЩ���������12864��oled���������ֽ�Ϊ��λ����ˢ�µ�
// ���ԣ�������ˢ�µ��������ڵ��ֽ���Ҳ���ܰ���������Ӧ�ñ�ˢ�µĲ��֣�
// �Ӷ�Ҳ��Ҫȡ���ⲿ�ֲ�Ӧ�ñ�ˢ�µĲ��ֵ����ݣ��ټ������������ˢ�µ����ݵĲ��֣����ܽ���ˢ�£�����ԭ�������ݾͶ�ʧ��
void DISP_updateGameBlock(u8 x, u8 y){
    u8 blockView;
    u8 shiwei, gewei;
    u16 startX, startY;
    u8 width, height;
    u16 numColor;

    // ��Χ����
    u8 blockViewNum = MINE_getViewNum(x, y);

    blockView = MINE_getView(x, y);

    // �򵥻��Ʒ��� -------------------------------------------

    // ��λͼ�������
    width = BLOCK_VIEW_FONT.fontWidth;
    height = BLOCK_VIEW_FONT.fontHeight;

    // ��ͼ��ʼ���꣬ԭʼ�������ƫ�����꣨��Ϸ����ʼλ�ã���drawFrame�����м��㲢���ã�
    startX = x * width + GAME_AREA_X_OFFSET;
    startY = y * height + GAME_AREA_Y_OFFSET;

    #if ISDEBUG
    //LOG("DISP_updateGameBlock 1 fontWidth=%b2d, fontHeight=%b2d\r\n", width, height);
    #endif
    
    // ����view��ͼ
    if (blockView == MINE_VIEW_NORMAL)
    {
        // δ����
        showMutiBitImg(startX, startY, FONT_MINE_BLOCK_BG23_BTN, &FONT_IMG_2BIT_BLOCK_BG23, 
                        COLOR_MINE_BLOCK_BTN_CENTER, COLOR_MINE_BLOCK_BTN_RB, COLOR_MINE_BLOCK_BTN_LT, COLOR_MINE_BLOCK_LINE,0,0,0);
    }
    else if (blockView == MINE_VIEW_DIGED)
    {
        // �ѷ���
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
            // ������(�������ö�Ӧ�����е�λ��)
            showMutiBitImg(startX+1, startY+1, (u8 *)blockViewNum, &FONT_IMG_2BIT_BLOCK_MINE_AND_NUMS22, 
                        numColor, COLOR_MINE_BLOCK_DIGED, 0, 0,0,0,0);
        }
    }
    else if (blockView == MINE_VIEW_FLAG)
    {
        // ����
        showMutiBitImg(startX + 4, startY + 4, FONT_MINE_BLOCK_FLAG16, &FONT_IMG_2BIT_BLOCK_FLAG16, 
                        COLOR_MINE_BLOCK_BTN_CENTER, COLOR_RED, 0, COLOR_BLACK,0,0,0);
    }
    else if (blockView == MINE_VIEW_MINE)
    {
        // �ڵ���ʱ����ʾ����δ�������
        showMutiBitImg(startX+1, startY+1, FONT_MINE_BLOCK_MINE, &FONT_IMG_2BIT_BLOCK_MINE_AND_NUMS22, 
                        COLOR_WHITE, COLOR_MINE_BLOCK_DIGED, 0, COLOR_BLACK,0,0,0);
    }
    else if (blockView == MINE_VIEW_DEADMINE)
    {
        // �ڵ���ʱ����ʾ��ը����
        showMutiBitImg(startX+1, startY+1, FONT_MINE_BLOCK_MINE, &FONT_IMG_2BIT_BLOCK_MINE_AND_NUMS22, 
                        COLOR_WHITE, COLOR_RED, 0, COLOR_BLACK,0,0,0);
    }
    else if (blockView == MINE_VIEW_WRONG) {
        // �ڵ���ʱ����ʾ���������׵�����������ӵĵط�
        showMutiBitImg(startX+1, startY+1, FONT_MINE_BLOCK_MINE_WRONG, &FONT_IMG_2BIT_BLOCK_MINE_AND_NUMS22, 
                        COLOR_WHITE, COLOR_MINE_BLOCK_DIGED, COLOR_RED, COLOR_BLACK,0,0,0);
    }
    else {
    }
}

//�����ַ�������ָ��������ռ�õ�ʵ�����ؿ��(�����ǻ��е�����)
u16 calcStringWidth(u8 *str, Font_Type *fontType){
    u8 j=0;
    u16 width=0, asciiWidth=0, hzWidth=0;

    #if ISDEBUG
    //LOG("calcStringWidth: str=%s\r\n", str);
    #endif

    if (fontType->type == FONT_TYPE_HZ) {
        hzWidth = fontType->fontWidth;
        asciiWidth = ((Font_Type *)fontType->asciiZk)->fontWidth; // ʹ�ú�������ʱ��������ڰ�����ֻ���ʾ�����İ������
    } else {
        asciiWidth = fontType->fontWidth;
        hzWidth = asciiWidth*2; // ʹ��ascii����ʱ��������ں��ֻ���ʾ2����ǵ�X
    }
    while (str[j]!='\0')
	{
        if (str[j] >= 0x20 && str[j] <= 0x7E) {
            // ���
            width += asciiWidth;
            j++;
        } else {
            // ȫ��
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

// ��ʾ����ɫ����(���7����ɫ)
Font_Type* showCharMutiColor(u16 x, u16 y, u8 *chr, Font_Type *font, u16 colorBg, 
                            u8 oneBitMutiColorCnt, u16 color1, u16 color2, u16 color3, u16 color4, u16 color5, u16 color6, u16 color7){
    // ���ݴ����chr�������ҵ����chr��fontData��������������е�ƫ��
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

    // ��ȡ��ǰ�ַ�/���ֵ�ƫ��index �Լ� ʵ��ʹ�õ�Font(��������������Ǻ����ֿ⣬��ָ�����ַ��ǰ���ַ�����ôʵ��ʹ�õ�font���Ǻ����ֿ��������İ���ֿ�)
    charDataStartIndex = FONT_getByteIndexAtFontData(chr, font, &useFont);
    // ������������
    fontData = useFont->fontArray;
    // ����������
    fontWidth = useFont->fontWidth;
    // ��������߶�
    fontHeight = useFont->fontHeight;
    // ��ɫλ��
    bitPerPixel = useFont->bitPerPixel;
    // ÿ��������������ռ�ü����ֽ�(���ص���xλ��)
    bytePerRow = (fontWidth * bitPerPixel-1)/8+1;

    if(charDataStartIndex == FONT_ERR_NOT_FOUND){
        // ������ֿ���û�ҵ������, ��ͼ��ʾһ����ǵ�X����ɫ��ʾ��ʾ����
        charDataStartIndex = 'X'-' ';
        errSwitchColorTmp = colorBg;
        colorBg = color1;
        color1 = errSwitchColorTmp;
    }

	devPrepareForDrawArea(x, y, fontWidth, fontHeight);

    // �������ָ���˵�λ��ɫ���� ���� ʹ�õ�����ȷʵ��1λɫ��� ��ôʹ�ô���Ķ����ɫ��Y�����Ͼ�����ʾ
    if (oneBitMutiColorCnt > 1 && bitPerPixel == 1)
    {
        // ��ɫ����
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

                // ��������λ��ȷ����ǰӦ��ʹ���ĸ��ֽ��ĸ�λ
                bitIndex = nowCol%8;

                // ��λȡ�������ݡ�1����������ɫ��0�����Ʊ���ɫ
                if ((fontData[charDataStartIndex+(bytePerRow*nowRow) + nowCol/8] << bitIndex) & 0x80)
                {
                    devPointInDrawArea(nowColor);
                }else{
                    devPointInDrawArea(colorBg);
                }
            }
        }
        
    } 
    // ���ָ�������Ƕ�λɫ�����ôʹ�ö�λɫ��ģʽ����ʱ���Ӳ���oneBitMutiColorCnt
    else if (bitPerPixel > 1) {
        for(nowRow=0;nowRow<fontHeight;nowRow++){
            for(nowCol=0;nowCol<fontWidth;nowCol++){

                // ��������λ��ȷ����ǰӦ��ʹ���ĸ��ֽ��ĸ�λ
                bitIndex = (nowCol*bitPerPixel)%8;

                // ȡ�ø����ض�Ӧ�Ķ�λ����
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

                // ���ݶ�λ���ݻ��Ʋ�ͬ����ɫ������2λ��ÿ�����ؿ��ܵ�ȡֵ��4�֣��ֱ��Ӧ color1-color4 4����ɫ
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
    // ����ʹ�õ�ɫ����(ʹ��color1)
    else {
        for(nowRow=0;nowRow<fontHeight;nowRow++){
            for(nowCol=0;nowCol<fontWidth;nowCol++){

                // ��������λ��ȷ����ǰӦ��ʹ���ĸ��ֽ��ĸ�λ
                bitIndex = nowCol%8;

                // ��λȡ�������ݡ�1����������ɫ��0�����Ʊ���ɫ
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

//������ʾ�ַ���,ָ����ɫ
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

//������ʾ�ַ���
void showStringCenter(u16 y, u8 *str, Font_Type *fontType, u8 isClear){
    showStringCenterColor(y, str, fontType, COLOR_BG, COLOR_FO, isClear);
}

// ��ʾ����ɫ�ַ���
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

		// ÿ���һ���ַ����ֺ����λ��һ���ַ����ֵĿ��
        x+= lastUseFont->fontWidth;
        // �Զ�����
		if(x>SCREEN_W-font->fontWidth){
            x=0;
            y+=font->fontHeight;
        }
        // ������ʾ��һ���ַ�����
		j++;
        // ����Ǻ�����ô��Ҫ����ƶ������ֽ�
        if (lastUseFont->type == FONT_TYPE_HZ) j++;
	}
}

// ������ʾ����ɫ�ַ���������1λ�����壩
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

// ########### ���Ƹ����� ######################################################################################################
// һ��ֳ�3�����͵ĺ���
// 1-������ĳ�ʼ��������ֻ���ڽ���û���ʱ������һ�Σ����������ƻ����ϲ���仯������
// 2-���в���flashOnOff�ĺ������ᱻ��ʱ���ã�flashOnOff ����1��0֮�������л����Լ�������Ҫˢ�»����ϵĶ�̬���ݣ�������˸���ֵ�
// 3-�����Ĳ���flashOnOff�ĺ��������и�����;��һ�����ض��¼�����ʱ�����ò��ᴫ��������ݣ�����Ե�ʳ���ʱ��ᴫ�����µķ�����

// ##### ��ҳ ��ʼ��ʱ������һ��(isStartUp��1=�ϵ���һ�ε��ã�0=���ϵ���һ�ε���)
void DISP_drawWelcome(u8 isStartUp){
    u16 titleStartX, titleY, logoX, logoY;
    u16 r=50;

    devScreenOFF();

    clearScreen();

    #if ISDEBUG && DEBUG_SKIP_HOME
        //����ģʽ������������
        return;
    #endif

    // ����
    titleY = SCREEN_H/6;
    showStringCenterColor(titleY, 0, &FONTHZ_TITLE64, COLOR_BG, COLOR_YELLOW, 0);
    devFillRectange(0, titleY, SCREEN_W, FONTHZ_TITLE64.fontHeight, COLOR_GRAYLT);
    showMutiBitImg((SCREEN_W-FONTHZ_TITLE64.fontWidth) / 2, titleY, 0, &FONTHZ_TITLE64, COLOR_GRAYLT, COLOR_GRAYDK, COLOR_GRAYDK, COLOR_BLACK,0,0,0);

    devScreenON();

    // ѧ��LOGO
    logoX = (SCREEN_W - FONTHZ_XD_LOGO40.fontWidth) / 2;
    logoY = titleY + FONTHZ_TITLE64.fontHeight + FONTHZ_XD_LOGO40.fontHeight / 2;
//TODO:Fortest
//isStartUp = 0;
    if (isStartUp)
    {
        delay_ms(500);
        // ѧ��LOGO�������ֵĶ���Ч��
        while(1){
            r+=5;
            if (r>=256) break;
            showChar(logoX, logoY, FONT_HZ_XD_LOGO, &FONTHZ_XD_LOGO40, RGB888toRGB565(r, 0, 0), COLOR_BG);
        }
        // ���ֺ�ȴ�һ���
        Pre_Delay_ms(300);
    } else {
        // ���ϵ续�棬������ʾ
        // ѧ��LOGO�������ֵĶ���Ч��
        while(r<255){
            r+=30;
            if (r>=256) r = 255;
            showChar(logoX, logoY, FONT_HZ_XD_LOGO, &FONTHZ_XD_LOGO40, RGB888toRGB565(r, 0, 0), COLOR_BG);
        }
        // ���ֺ�ȴ�һ���
        //Pre_Delay_ms(500);
    }

    // �Ѷ�ѡ��
    showStringCenter(logoY + FONTHZ_XD_LOGO40.fontHeight + FONTHZ_XD_LOGO40.fontHeight / 2, STR_LEVEL_SEL, &FONTHZ32, 0);
}

// ##### ��ҳ ���ڱ�����
void DISP_flashWelcome(u8 flashOnOff){
    u16 logoX, logoY;

    u8 tmp=0;

    #if ISDEBUG
    LOG("  DISP_flashWelcome 1: call showStringCenter(str=%s pStr=%ld)\r\n", STR_PRESS_ANY_KEY, &STR_PRESS_ANY_KEY);
    #endif
    
    // ѧ��LOGO�����ɫ
    logoX = (SCREEN_W - FONTHZ_XD_LOGO40.fontWidth) / 2;
    logoY = SCREEN_H/6 + FONTHZ_TITLE64.fontHeight + FONTHZ_XD_LOGO40.fontHeight / 2;
    showChar(logoX, logoY, FONT_HZ_XD_LOGO, &FONTHZ_XD_LOGO40, randRGB565(), COLOR_BG);
}

// ##### Demoҳ ��ʼ��ʱ������һ��
void DISP_drawDemo(u8 soundOnOff){
    // NOTHING
}

// ##### Demoҳ �Ե�ʳ��ʱ������һ�Σ����»����ϵķ���
void DISP_updateDemoScore(u16 maxDemoScore, u16 nowDemoScore){
    // DONOTHING
}

// ##### Demoҳ ����ʱ������һ�Σ�����һЩ��ʾ����Ŀǰʲô��û��
void DISP_updateDemoGameover(u16 maxDemoScore, u16 avgDemoScore, u16 lastDemoScore){
    // DONOTHING
}

// ##### Demoҳ ���ڱ����� Ŀǰû�õ�
void DISP_flashDemo(u8 flashOnOff){
    // DONOTHING
}

// ##### Gameҳ ��ʼ��ʱ������һ��
void DISP_drawGame(u8 soundOnOff, u8 flagCnt){

    #if ISDEBUG
    LOG("DISP_drawGame\r\n");
    #endif

    clearScreen();
    DISP_drawFrame();

    // �����Ҳ���Ϣ��
    // ע�⣬������������DISP_drawFrame�ĺ��档�����һ�μ���ʱ��δ��������ȷ����ֵ��ΪĬ��ֵ1��
    // ����Ϊ1����ȷ��ֵ���������λ�õ�һ���ǶԵģ��ڶ��ξ�ȫ���ˡ�
    //topInfoY = frameThickness + 2;

    // ��Ȩ
    showString(SCREEN_W - FRAME_RIGHT_INFO_WIDTH+3, 100, "Chen.L", &FONT20, COLOR_FO, COLOR_BG);

    // ��ʼʱ��
    DISP_updateTime(0);

    // ��ʼ������
    // ������
    devFillRectange(SCREEN_W - FRAME_RIGHT_INFO_WIDTH - frameThickness, frameThickness, FRAME_RIGHT_INFO_WIDTH, 45, COLOR_FO );
    showChar(FLAG_X, FLAG_Y, "��", &FONTHZ32, COLOR_FO, COLOR_BG);
    DISP_updateFlagCnt(flagCnt);

    // �˵�
    DISP_drawUnLock();
    DISP_drawSound(1);
    showString(MENU_X, MENU_CLICK_Y, STR_GAME_MENU3, &FONTHZ32, COLOR_BG, COLOR_FO);

    // ��������״̬
    DISP_drawSound(soundOnOff);

    #if ISDEBUG
    LOG("DISP_drawGame done.\r\n");
    #endif
}

// ##### Gameҳ ���»����ϵ�ʱ��
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

    // �������Ӹ���
    sprintf(buff, "%b02d", flagCnt);
    showString(FLAG_X + 30, FLAG_Y + 5, buff, &FONT32, COLOR_FO, COLOR_BG);
}

// ##### BeforeGameOverҳ ���ڱ�����
void DISP_flashBeforeGameOver(u8 flashOnOff, u8 isWin){
    // ��˸���� �����������
    if (flashOnOff)
    {
        showString(MENU_X, MENU_LOCK_Y, "����", &FONTHZ32, COLOR_BG, COLOR_FO);
        showString(MENU_X, MENU_FLAG_Y, "���", &FONTHZ32, COLOR_BG, COLOR_FO);
        showString(MENU_X, MENU_CLICK_Y, "����", &FONTHZ32, COLOR_BG, COLOR_FO);
    } else {
        devFillRectange(MENU_X, MENU_LOCK_Y, FONTHZ32.fontWidth * 2, MENU_CLICK_Y - MENU_LOCK_Y + FONTHZ32.fontHeight, COLOR_BG);
    }
}

// ##### GameOverҳ ��ʼ��ʱ������һ��
void DISP_drawGameOver(u8 isWin, u8* levelStr, u16 score, u16 hiScore){
    u8 buff1[32];
    clearScreen();

    #define  TITLE_Y         35
    #define  SCROE_Y         80
    #define HSCROE_Y        120

    //showStringCenter(5, "__________", &FONT32, 0);

    // ���⣬���������߷֣���ʾ ˢ�¼�¼ ���� ��ʾ��Ϸ����
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
    

    // �÷�
    if (isWin)
    {
        sprintf(buff1, "%s %02d:%02d", STR_GAMEOVER_SCORE, score/60, score%60);
    } else {
        // û�е÷�
        sprintf(buff1, "%s --:--", STR_GAMEOVER_SCORE);
    }
    showStringCenter(SCROE_Y, buff1, &FONTHZ32, 0);
    
    // ��¼
    sprintf(buff1, "%s %02d:%02d", STR_GAMEOVER_HSCORE, hiScore/60, hiScore%60);
    showStringCenter(HSCROE_Y, buff1, &FONTHZ32, 0);

    showStringCenter(150, "__________", &FONT32, 0);
}

// ##### GameOverҳ ���ڱ�����
void DISP_flashGameOver(u8 flashOnOff, u8 isNewRecord, u8* levelStr){

    if (isNewRecord)
    {
        // // ˢ�¼�¼��̬Ч��
        // sprintf(buff, "%s-%s", STR_GAMEOVER_NEWRECORD, levelStr);
        // showStringCenterColor(35, buff, &FONTHZ32, COLOR_FO, randRGB565DK(), 0);
    }

    // ��˸���� �����������...
    showStringCenter(190, STR_PRESS_ANY_KEY, &FONTHZ32, flashOnOff);
}

// ���ƿ��
void DISP_drawFrame() {
    u16 barTopY, barRightX;

    // �߿���
    frameThickness = 5;

    // ��Ϸ�����ҷ�����λ��Y = ��Ļ�� - �ҷ��߿�� - �ҷ���Ϣ���� - �ҷ�������(Ҳ�Ǳ߿���) 
    // ��1��Ŀ����Ϊ�������Ҳ��һ���صĿռ���������ѡ��״̬�����
    barRightX = SCREEN_W - frameThickness - FRAME_RIGHT_INFO_WIDTH - frameThickness + 1;

    // ��ʼ����(ʵ�ľ���ƴ��)
    devFillRectange(0, 0, SCREEN_W, frameThickness, COLOR_FRAME);                           // ��
    devFillRectange(0, 0, frameThickness, SCREEN_H, COLOR_FRAME);                           // ��
    devFillRectange(SCREEN_W - frameThickness, 0, frameThickness, SCREEN_H, COLOR_FRAME);   // ��
    devFillRectange(barRightX, 0, frameThickness, SCREEN_H, COLOR_FRAME);                   // ������
    devFillRectange(SCREEN_W - frameThickness - FRAME_RIGHT_INFO_WIDTH, MENU_LOCK_Y - 35, FRAME_RIGHT_INFO_WIDTH, 30, COLOR_FRAME);    // �Һ���
    devFillRectange(0, SCREEN_H - frameThickness+1, SCREEN_W, frameThickness-1, COLOR_FRAME);   // ��

    // ���������� ���Ҳ� �� ��ײ� ȱ�ٵ�һ�����ؿ�ı߿���
    devDrawLine(frameThickness, SCREEN_H-frameThickness, barRightX - 1, SCREEN_H-frameThickness, 1, COLOR_MINE_BLOCK_LINE);
    devDrawLine(barRightX - 1, frameThickness, barRightX - 1, SCREEN_H-frameThickness, 1, COLOR_MINE_BLOCK_LINE);

    // ��Ϸ����Yƫ��
    GAME_AREA_Y_OFFSET = frameThickness;
    GAME_AREA_X_OFFSET = frameThickness;
}

// ���ƹ��
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

// ���ع��
void DISP_hideCusor(u8 x, u8 y){
    drawCusor(x, y, COLOR_MINE_BLOCK_LINE);
}

// ��ʾ���     
void DISP_showCusor(u8 x, u8 y){
    drawCusor(x, y, COLOR_RED);
}

// �����;���״̬
void DISP_drawSound(u8 soundOnOff) {
    if (soundOnOff)
    {
        showString(MENU_X, MENU_FLAG_Y, STR_GAME_MENU2, &FONTHZ32, COLOR_BG, COLOR_FO);
    } else {
        // ���ƾ���
        devDrawLine(MENU_X+27, MENU_FLAG_Y+2, MENU_X+51, MENU_FLAG_Y+28, 3, COLOR_RED);
    }
    
}

void DISP_drawLock() {
    showString(MENU_X, MENU_LOCK_Y, STR_GAME_MENU1_LOCK, &FONTHZ32, COLOR_BG, COLOR_RED);
}

void DISP_drawUnLock() {
    showString(MENU_X, MENU_LOCK_Y, STR_GAME_MENU1_UNLOCK, &FONTHZ32, COLOR_BG, COLOR_FO);
}

// ��ʼ��
void DISP_init(void){
    // u32 clock;
    // u16 clockH;
    // u16 clockL;

    devDisplayInit(COLOR_BG);

    // ���Դ�ӡ��ϵͳʱ��
    // clock=Get_SysClk_FRE();
    // clockH = clock >> 16;
    // clockL = clock & 0xFFFF;
    // sprintf(buff, "H:%u  L:%u", clockH, clockL);
    // showString(0,0,buff,&FONTHZ32,COLOR_BLACK, COLOR_WHITE);
    // while (1);
    
}
