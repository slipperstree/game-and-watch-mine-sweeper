#include "common.h"
#include "string.h"
#include "display.h"
#include "embSnake.h"
#include "font.h"
#include "embSnakeDevice.h"

// define start ------------------------------------------------------------------------------------------

// �����ַ������壬�������޸�
// ��Ļ��С�Ļ�Ҫ�Ķ�һЩ,����Ҳ��Ҫ̫��,Ԥ���Ļ�����ֻ��128�ֽ�,�����Ļ��ᷢ��δ֪����
#ifdef UI_LANG_EN
    u8* STR_PRESS_ANY_KEY             = (u8*)"Press any btn";
    u8* STR_DEMO_MENU                 = (u8*)"G:Play B:Speed Other:Home";
    u8* STR_GAME_MENU                 = (u8*)"T:Auto";
    u8* STR_GAMEOVER_GAMEOVER         = (u8*)"Game Over";
    u8* STR_GAMEOVER_NEWRECORD        = (u8*)"New Record";
    u8* STR_GAMEOVER_HSCORE           = (u8*)"Max   ";
    u8* STR_GAMEOVER_SCORE            = (u8*)"Score ";
    u8* STR_GAMEOVER_MORE             = (u8*)"Press PAUSE/SET for more info.";
    u8* STR_GAMEOVER_INFO_EXIT        = (u8*)"EXIT : Press any button";

    // �ײ���Ϣ������
    #define FONT_BOTTOM_INFO          FONT_ASC12
    // �������ֵ����壬����GameOver�������������
    #define FONT_OTHER_TEXT           FONT32
    // ������Ϣ������
    //   ע�⣬�����������������·��������μ��������ĸ߶Ȳ����ƿ�ܣ�����Ϸ������ĸ߶��Ǹ���snake map size����ó��ģ�
    //        Ҫ��֤ʣ��ĸ߶��㹻��ʾ����ָ������
    #define FONT_TOP_INFO             FONT_ASC16
    
#endif
#ifdef UI_LANG_CN
    u8* STR_PRESS_ANY_KEY             = (u8*)"  �����������..";
    u8* STR_DEMO_MENU                 = (u8*)"B:Speed P:Sound Other:Home";
    u8* STR_GAME_MENU                 = (u8*)"P:Sound";
    u8* STR_GAMEOVER_GAMEOVER         = (u8*)"��Ϸ����";
    u8* STR_GAMEOVER_NEWRECORD        = (u8*)"��ϲ,ˢ�¼�¼!";
    u8* STR_GAMEOVER_HSCORE           = (u8*)"��¼:";
    u8* STR_GAMEOVER_SCORE            = (u8*)"�÷�:";
    u8* STR_GAMEOVER_MORE             = (u8*)"Press PAUSE/SET for more info";

    // �ײ���Ϣ������
    #define FONT_BOTTOM_INFO          FONT_ASC12
    // �������ֵ����壬����GameOver�������������
    #define FONT_OTHER_TEXT           FONT32
    // ������Ϣ������
    #define FONT_TOP_INFO             FONT_ASC20
    
#endif

u8* STR_GAME_INFO                     = (u8*)"Snake & Watch";

// Gameover page
#define  TITLE_Y         15
#define  SCROE_Y         80
#define HSCROE_Y        120

// define end ------------------------------------------------------------------------------------------

// ��Ϸ�������Ͻ�����Yƫ��(Ҳ���ǿ�ܺ��)
u16 gameArea_X_offset=0;
// ��Ϸ�������Ͻ�����Yƫ�ƣ�drawFrame�м��㲢���ã�(ȥ��������ܺͶ�����Ϣ�����Լ��Ϸ������߶�(Ҳ���ǿ�ܺ��))
u16 gameArea_Y_offset=0;
// ��Ϸ�����Ϸ�����λ��Y��drawFrame�м��㲢���ã�
u16 barTopY=0;
// ��Ϸ����ײ���Ϣ���߶ȣ�drawFrame�м��㲢���ã�
u16 barBottomY=0;
// ��Ϸ�����Ϸ���Ϣ��������λ�ã�drawFrame�м��㲢���ã�
u8 topInfoY=0;
// ��Ϸ�����Ϸ���Ϣ����ʳ��ͼ��λ�ã�drawFrame�м��㲢���ã�
u8 topFoodIconY=0;
// ��ʱ������(sprintf��),��Ҫ�޸�size�Ļ���һ���޸�common.c�е�ͬ������,����ֻ������
extern u8 buff[128];

// �ڲ���������
u16 _ColorBack=0x0000, _ColorFore=0x0000;
#define COLOR_BG       _ColorBack
#define COLOR_FO       _ColorFore
u16 _ColorSnake=0x0000, _ColorApple=0x0000, _ColorFrame=0x0000;
#define COLOR_SNAKE    _ColorSnake
#define COLOR_APPLE    _ColorApple
#define COLOR_FRAME    _ColorFrame

// ��������
void clearScreen(void);     // ����
Font_Type* showChar(u16 x, u16 y, u8 *chr, Font_Type *font, u16 colorBg, u16 colorFont);

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

void clearScreen(void){
    devFillRectange(0,0,SCREEN_W,SCREEN_H,COLOR_BG);
}

void devShowString(u16 x,u16 y, u8 *str,Font_Type *fontType, u16 colorBg, u16 colorFont)
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

void DISP_drawWall(){
    u8 x,y;
    for (x = 0; x < SNAKE_SIZE_X; ++x)
    {
        for (y = 0; y < SNAKE_SIZE_Y; ++y)
        {
            if (SNAKE_getView(x, y) == SNAKE_VIEW_WALL){
                DISP_updateGameBlock(x, y);
            }
        }
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
    u8 blockSts, blockView;
    u8 shiwei, gewei;
    u16 startX, startY;
    u8 width, height;

    blockSts = SNAKE_getMapData(x, y);
    blockView = SNAKE_getView(x, y);

    // �򵥻��Ʒ��� -------------------------------------------
    
    // #define BLOCK_PIC_SIZE 10
    // startX = x * BLOCK_PIC_SIZE + 1;
    // startY = y * BLOCK_PIC_SIZE + 1;
    // width = BLOCK_PIC_SIZE-2;
    // height = BLOCK_PIC_SIZE-2;
    // if (blockSts == STS_EMPTY) {
    //     devFillRectange(x * BLOCK_PIC_SIZE, y * BLOCK_PIC_SIZE, BLOCK_PIC_SIZE, BLOCK_PIC_SIZE, COLOR_BLACK);
    // } else if(blockSts == STS_APPLE) {
    //     //ILI9163LCD(pPIC[column - blockStartColumn],OLED_DATA);
    //     devFillRectange(startX, startY, width, height, 0x001f);
    // } else if(blockSts / 10 == STS_HEAD / 10) {
    //     devFillRectange(x * BLOCK_PIC_SIZE, y * BLOCK_PIC_SIZE, BLOCK_PIC_SIZE, BLOCK_PIC_SIZE, 0x07e0);
    // } else {
    //     devFillRectange(x * BLOCK_PIC_SIZE, y * BLOCK_PIC_SIZE, BLOCK_PIC_SIZE, BLOCK_PIC_SIZE, COLOR_BLACK);
    //     devFillRectange(startX, startY, width, height, COLOR_WHITE);
    // }

    // ��λͼ�������
    width = BLOCK_VIEW_FONT.fontWidth;
    height = BLOCK_VIEW_FONT.fontHeight;

    // ��ͼ��ʼ���꣬ԭʼ�������ƫ�����꣨��Ϸ����ʼλ�ã���drawFrame�����м��㲢���ã�
    startX = x * width + gameArea_X_offset;
    startY = y * height + gameArea_Y_offset;

    #if ISDEBUG
    sprintf(buff, "DISP_updateGameBlock 1 fontWidth=%b2d, fontHeight=%b2d\r\n", width, height);LOG(buff);
    sprintf(buff, "DISP_updateGameBlock 2 x=%b2d, y=%b2d, blockSts=%b2d, view=%b2d, startX=%d, startY=%d\r\n",
                     x, y, blockSts, blockView, startX, startY);LOG(buff);
    #endif
    
    // ����view��ͼ������view��������ġ�UDLR���������Լ����ڲ�λ���������ķ���
    // ����HEAD_U��ʾ����ͷ��������ͷ���ڲ�λ����������ͷ���Ϸ���Ҳ������Ҫ�������µ���ͷ���е��ƣ����������ڳ�����
    if (blockView == SNAKE_VIEW_APPLE)
    {
        // ʳ��
        showChar(startX, startY, FONT_SNAKE_BLOCK_APPLE, &BLOCK_VIEW_FONT, COLOR_BG, COLOR_APPLE);
    }
    else if (blockView == SNAKE_VIEW_HEAD_U) {
        // ��ͷ����
        showChar(startX, startY, FONT_SNAKE_BLOCK_HEAD_U, &BLOCK_VIEW_FONT, COLOR_BG, COLOR_SNAKE);
    }
    else if (blockView == SNAKE_VIEW_HEAD_D) {
        // ��ͷ����
        showChar(startX, startY, FONT_SNAKE_BLOCK_HEAD_D, &BLOCK_VIEW_FONT, COLOR_BG, COLOR_SNAKE);
    }
    else if (blockView == SNAKE_VIEW_HEAD_L) {
        // ��ͷ����
        showChar(startX, startY, FONT_SNAKE_BLOCK_HEAD_L, &BLOCK_VIEW_FONT, COLOR_BG, COLOR_SNAKE);
    }
    else if (blockView == SNAKE_VIEW_HEAD_R) {
        // ��ͷ����
        showChar(startX, startY, FONT_SNAKE_BLOCK_HEAD_R, &BLOCK_VIEW_FONT, COLOR_BG, COLOR_SNAKE);
    }
    else if (blockView == SNAKE_VIEW_BODY_V) {
        // ������ֱ
        showChar(startX, startY, FONT_SNAKE_BLOCK_BODY_V, &BLOCK_VIEW_FONT, COLOR_BG, COLOR_SNAKE);
    }
    else if (blockView == SNAKE_VIEW_BODY_LU) {
        // ������������ 
        showChar(startX, startY, FONT_SNAKE_BLOCK_BODY_LU, &BLOCK_VIEW_FONT, COLOR_BG, COLOR_SNAKE);
    }
    else if (blockView == SNAKE_VIEW_BODY_UR) {
        // ������������
        showChar(startX, startY, FONT_SNAKE_BLOCK_BODY_UR, &BLOCK_VIEW_FONT, COLOR_BG, COLOR_SNAKE);
    }
    else if (blockView == SNAKE_VIEW_BODY_LD) {
        // ������������
        showChar(startX, startY, FONT_SNAKE_BLOCK_BODY_LD, &BLOCK_VIEW_FONT, COLOR_BG, COLOR_SNAKE);
    }
    else if (blockView == SNAKE_VIEW_BODY_DR) {
        // ������������
        showChar(startX, startY, FONT_SNAKE_BLOCK_BODY_DR, &BLOCK_VIEW_FONT, COLOR_BG, COLOR_SNAKE);
    }
    else if (blockView == SNAKE_VIEW_BODY_H) {
        // ����ˮƽ
        showChar(startX, startY, FONT_SNAKE_BLOCK_BODY_H, &BLOCK_VIEW_FONT, COLOR_BG, COLOR_SNAKE);
    }
    else if (blockView == SNAKE_VIEW_TAIL_U) {
        // ��β β�⳯��
        showChar(startX, startY, FONT_SNAKE_BLOCK_TAIL_U, &BLOCK_VIEW_FONT, COLOR_BG, COLOR_SNAKE);
    }
    else if (blockView == SNAKE_VIEW_TAIL_D) {
        // ��β β�⳯��
        showChar(startX, startY, FONT_SNAKE_BLOCK_TAIL_D, &BLOCK_VIEW_FONT, COLOR_BG, COLOR_SNAKE);
    }
    else if (blockView == SNAKE_VIEW_TAIL_L) {
        // ��β β�⳯��
        showChar(startX, startY, FONT_SNAKE_BLOCK_TAIL_L, &BLOCK_VIEW_FONT, COLOR_BG, COLOR_SNAKE);
    }
    else if (blockView == SNAKE_VIEW_TAIL_R) {
        // ��β β�⳯��
        showChar(startX, startY, FONT_SNAKE_BLOCK_TAIL_R, &BLOCK_VIEW_FONT, COLOR_BG, COLOR_SNAKE);
    }
    else if (blockView == SNAKE_VIEW_EMPTY) {
        // ��շ���
        devFillRectange(startX, startY, width, height, COLOR_BG);
    }
    else if (blockView == SNAKE_VIEW_WALL) {
        // �ϰ��� ��Ҫ�����ﻭ������ֻ������Ϸ�лᶯ̬�仯������
        //devFillRectange(startX, startY, width, height, COLOR_FO);
    }
    else {
        // TODO: ע�⣡�������Ż�������΢���ߵ�ʱ��������Щ��Ч����������ſ��ͻ������ֵ�����ԭ������
        //       Ŀǰ�������ԣ������뼶������Ϊ0-6��������ֻҪ���ڵ���7�ͻ�����⡣
        //       �����Ż�����7��˵������չ���������Ż����ʵ�ʱ�ԼĴ���������DPTR����ָ���������ʽ���ִ���ٶȺʹ����С�Ż���
        //       ���ǲ�֪��Ϊʲô��Ӱ������߼���
        //       �ο� https://blog.csdn.net/tjcwt2011/article/details/94347250
        //       
        // shiwei = blockSts / 10*10;
        
        // switch (shiwei)
        // {
        // case STS_EMPTY:
        //     //devFillRectange(startX, startY, width, height, COLOR_BG);
        //     break;
        // default:
        //     break;
        // }
    }
}

//�����ַ�������ָ��������ռ�õ�ʵ�����ؿ��(�����ǻ��е�����)
u16 calcStringWidth(u8 *str, Font_Type *fontType){
    u8 j=0;
    u16 width=0, asciiWidth=0, hzWidth=0;

    #if ISDEBUG
    //sprintf(buff, "calcStringWidth: str=%s\r\n", str); LOG(buff);
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
    //sprintf(buff, "calcStringWidth: width=%hd done.\r\n", width); LOG(buff);
    #endif

    return width;
}

Font_Type* showChar(u16 x, u16 y, u8 *chr, Font_Type *font, u16 colorBg, u16 colorFont){

    // ���ݴ����chr�������ҵ����chr��fontData��������������е�ƫ��
    u8 nowRow=0,nowCol=0,fontWidth=0,fontHeight=0;
    u8 bytePerRow,bitIndex=0,charByteCnt=0;
    u16 charDataStartIndex=0;
    u8* fontData;
    u16 errSwitchColorTmp;
    Font_Type *useFont = NULL;

    // ��ȡ��ǰ�ַ�/���ֵ�ƫ��index �Լ� ʵ��ʹ�õ�Font(��������������Ǻ����ֿ⣬��ָ�����ַ��ǰ���ַ�����ôʵ��ʹ�õ�font���Ǻ����ֿ��������İ���ֿ�)
    charDataStartIndex = FONT_getByteIndexAtFontData(chr, font, &useFont);
    // ������������
    fontData = useFont->fontArray;
    // ����������
    fontWidth = useFont->fontWidth;
    // ��������߶�
    fontHeight = useFont->fontHeight;
    // ÿ��������������ռ�ü����ֽ�
    bytePerRow = (fontWidth-1)/8+1;

    if(charDataStartIndex == FONT_ERR_NOT_FOUND){
        // ������ֿ���û�ҵ������, ��ͼ��ʾһ����ǵ�X����ɫ��ʾ��ʾ����
        charDataStartIndex = 'X'-' ';
        errSwitchColorTmp = colorBg;
        colorBg = colorFont;
        colorFont = errSwitchColorTmp;
    }

	devPrepareForDrawArea(x, y, fontWidth, fontHeight);

	for(nowRow=0;nowRow<fontHeight;nowRow++){
		for(nowCol=0;nowCol<fontWidth;nowCol++){

			// ��������λ��ȷ����ǰӦ��ʹ���ĸ��ֽ��ĸ�λ
			bitIndex = nowCol%8;

			// ��λȡ�������ݡ�1����������ɫ��0�����Ʊ���ɫ
			if ((fontData[charDataStartIndex+(bytePerRow*nowRow) + nowCol/8] << bitIndex) & 0x80)
			{
				devPointInDrawArea(colorFont);
			}else{
				devPointInDrawArea(colorBg);
			}
		}
	}

    return useFont;
}

//������ʾ�ַ���,ָ����ɫ
void showStringCenterColor(u16 y, u8 *str, Font_Type *fontType, u16 colorBg, u16 colorFo, u8 isClear){
    u16 strWidth;

    #if ISDEBUG
    //sprintf(buff, "showStringCenterColor: %s\r\n", str); LOG(buff);
    #endif

    strWidth = calcStringWidth(str, fontType);
    
    if (isClear)
    {
        devFillRectange((SCREEN_W - strWidth) / 2, y, strWidth, fontType->fontHeight, colorBg);
    } else {
        devShowString((SCREEN_W - strWidth) / 2, y, str, fontType, colorBg, colorFo);
    }

    #if ISDEBUG
    //sprintf(buff, "showStringCenterColor: %s done.\r\n", str); LOG(buff);
    #endif
}

//������ʾ�ַ���
void showStringCenter(u16 y, u8 *str, Font_Type *fontType, u8 isClear){
    showStringCenterColor(y, str, fontType, COLOR_BG, COLOR_FO, isClear);
}

//������ʾ����Ӱ���ַ���
//TODO: �Ȼ��Ƶ���Ӱ���ֻᱻ����Ƶı���ı������ָ��ǵ�����ʵ��������ܱ�����ȡ�õ�ǰ���ص㵱ǰ��ɫ�����Ʊ���ǵ��󲿷�ʱ��Ҫ����ǰ��ɫ���Ǹõ�ԭ������ɫ.Ч�ʽϵ���ʱ������
void showStringCenterShadow(u16 y, u8 offsetX, u8 offsetY, u8 *str, Font_Type *fontType){
    //�Ȼ���Ӱ
    devShowString((SCREEN_W - fontType->fontWidth * strlen(str)) / 2 + offsetX, y + offsetY, str, fontType, COLOR_BG, COLOR_RED);
    //�ٻ�����
    showStringCenter(y, str, fontType, 0);
}

void DISP_setBackColor(u16 color){
    _ColorBack = color;
}

void DISP_setForeColor(u16 color){
    _ColorFore = color;
}

void DISP_setSnakeColor(u16 color){
    _ColorSnake = color;
}

void DISP_setAppleColor(u16 color){
    _ColorApple = color;
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
    #ifdef UI_LANG_CN
        // ̰����
        titleStartX = (SCREEN_W - FONT_TITLE.fontWidth * 4) / 2;
        titleY = SCREEN_H/8;
        showChar(titleStartX, titleY, "̰", &FONT_TITLE, COLOR_BG, COLOR_TITLE1);
        showChar(titleStartX + FONT_TITLE.fontWidth + FONT_TITLE.fontWidth/2, titleY, "��", &FONT_TITLE, COLOR_BG, COLOR_TITLE1);
        showChar(titleStartX + FONT_TITLE.fontWidth*3, titleY, "��", &FONT_TITLE, COLOR_BG, COLOR_TITLE2);
    #endif
    #ifdef UI_LANG_EN
        // SNAKE
        titleStartX = (SCREEN_W - FONT_TITLE.fontWidth * 6) / 2;
        titleY = SCREEN_H/8;
        showChar(titleStartX, titleY, "̰", &FONT_TITLE, COLOR_BG, COLOR_TITLE1);
        showChar(titleStartX+=(FONT_TITLE.fontWidth + FONT_TITLE.fontWidth/2), titleY, "��", &FONT_TITLE, COLOR_BG, COLOR_TITLE2);
        showChar(titleStartX+=(FONT_TITLE.fontWidth + FONT_TITLE.fontWidth/2), titleY, "��", &FONT_TITLE, COLOR_BG, COLOR_TITLE1);
        showChar(titleStartX+=(FONT_TITLE.fontWidth + FONT_TITLE.fontWidth/2), titleY, "��", &FONT_TITLE, COLOR_BG, COLOR_TITLE2);
        showChar(titleStartX+=(FONT_TITLE.fontWidth + FONT_TITLE.fontWidth/2), titleY, "��", &FONT_TITLE, COLOR_BG, COLOR_TITLE1);
    #endif

    devScreenON();

    // LOGO
    logoX = (SCREEN_W - FONT_LOGO70.fontWidth) / 2;
    logoY = titleY + FONT_TITLE.fontHeight + FONT_LOGO70.fontHeight / 2;
    if (isStartUp)
    {
        My_delay_ms(1000);
        // S&W LOGO�������ֵĶ���Ч��
        while(1){
            r+=1;
            if (r>=150) {
                showChar(logoX, logoY, FONT_LOGO_IDX_0, &FONT_LOGO70, RGB888toRGB565(150, 150, 150), COLOR_BG);
                break;
            } 
            showChar(logoX, logoY, FONT_LOGO_IDX_0, &FONT_LOGO70, RGB888toRGB565(r, r, r), COLOR_BG);
            My_delay_ms(350);
        }
        // ���ֺ�ȴ�һ���
        My_delay_ms(500);
    } else {
        // ���ϵ续�棬������ʾ
        // S&W LOGO�������ֵĶ���Ч��
        while(1){
            r+=5;
            if (r>=150) {
                showChar(logoX, logoY, FONT_LOGO_IDX_0, &FONT_LOGO70, RGB888toRGB565(150, 150, 150), COLOR_BG);
                break;
            } 
            showChar(logoX, logoY, FONT_LOGO_IDX_0, &FONT_LOGO70, RGB888toRGB565(r, r, r), COLOR_BG);
            My_delay_ms(200);
        }
        // ���ֺ�ȴ�һ���
        My_delay_ms(200);
    }
    
}

// ##### ��ҳ ���ڱ�����
void DISP_flashWelcome(u8 flashOnOff){
    u16 logoX, logoY;

    u8 tmp=0;

    #if ISDEBUG
    sprintf(buff, "  DISP_flashWelcome 1: call showStringCenter(str=%s pStr=%ld)\r\n", STR_PRESS_ANY_KEY, &STR_PRESS_ANY_KEY); LOG(buff);
    #endif
    
    // S&W LOGO�����ɫ
    logoX = (SCREEN_W - FONT_LOGO70.fontWidth) / 2;
    logoY = SCREEN_H/8 + FONT_TITLE.fontHeight + FONT_LOGO70.fontHeight / 2;
    //showChar(logoX, logoY, FONT_LOGO_IDX_0, &FONT_LOGO70, randRGB565(), COLOR_BG);

    // ��˸����
    #ifdef UI_LANG_EN
        showStringCenter(logoY + FONT_LOGO70.fontHeight + 20, STR_PRESS_ANY_KEY, &FONT_OTHER_TEXT, flashOnOff);
    #endif
    #ifdef UI_LANG_CN
        showStringCenter(logoY + FONT_LOGO70.fontHeight + 10, STR_PRESS_ANY_KEY, &FONT_OTHER_TEXT, flashOnOff);
    #endif

    // ���� ̰���� �����ɫ
    // showChar(10, 20, "̰", &FONT_TITLE, COLOR_BG, randRGB565());
    // showChar(50, 20, "��", &FONT_TITLE, COLOR_BG, randRGB565());
    // showChar(90, 20, "��", &FONT_TITLE, COLOR_BG, randRGB565());
}

// ##### Demoҳ ��ʼ��ʱ������һ��
void DISP_drawDemo(u8 soundOnOff){

    #if ISDEBUG
    sprintf(buff, "DISP_drawDemo\r\n"); LOG(buff);
    #endif

    devScreenOFF();

    clearScreen();
    DISP_drawFrame();

    // ������Ϣ
    // ʳ�� = 0  (��ǰ���˶��ٸ�)
    showChar(FRAME_THICKNESS + 10, topFoodIconY, FONT_SNAKE_BLOCK_APPLE, &BLOCK_VIEW_FONT, COLOR_BG, COLOR_FO);
    devShowString(FRAME_THICKNESS + 10 + BLOCK_VIEW_FONT.fontWidth, topInfoY, " =  0", &FONT_TOP_INFO, COLOR_BG, COLOR_FO);
    // ������ʾ����
    devShowString(SCREEN_W - FRAME_THICKNESS - calcStringWidth(STR_GAME_INFO, &FONT_TOP_INFO) - 5, topInfoY, STR_GAME_INFO, &FONT_TOP_INFO, COLOR_BG, COLOR_FO);

    // ��Ϸ����������ʾ ��ʾģʽ ����ʾ������Ҫ����Ϊ�ϰ��ﲻ����ͨ����
    showChar(SNAKE_DEMO_TITLE_1_X * 12  + gameArea_X_offset, 
                SNAKE_DEMO_TITLE_1_Y * 12 + gameArea_Y_offset, 
                FONT_DEMO_MODE_YAN, &FONT_DEMO_MOEDE24, COLOR_WINLOGO_R, COLOR_BG );
    showChar(SNAKE_DEMO_TITLE_2_X * 12  + gameArea_X_offset, 
                SNAKE_DEMO_TITLE_2_Y * 12 + gameArea_Y_offset, 
                FONT_DEMO_MODE_SHI1,&FONT_DEMO_MOEDE24, COLOR_WINLOGO_G, COLOR_BG );
    showChar(SNAKE_DEMO_TITLE_3_X * 12  + gameArea_X_offset, 
                SNAKE_DEMO_TITLE_3_Y * 12 + gameArea_Y_offset, 
                FONT_DEMO_MODE_MO,  &FONT_DEMO_MOEDE24, COLOR_WINLOGO_B, COLOR_BG );
    showChar(SNAKE_DEMO_TITLE_4_X * 12  + gameArea_X_offset, 
                SNAKE_DEMO_TITLE_4_Y * 12 + gameArea_Y_offset, 
                FONT_DEMO_MODE_SHI2,&FONT_DEMO_MOEDE24, COLOR_WINLOGO_Y, COLOR_BG );

    // �ײ���Ϣ
    showStringCenter(SCREEN_H - FONT_BOTTOM_INFO.fontHeight - FRAME_THICKNESS - 1, STR_DEMO_MENU, &FONT_BOTTOM_INFO, 0);

    // ��������״̬
    DISP_drawSound(soundOnOff);

    devScreenON();

    #if ISDEBUG
    sprintf(buff, "DISP_drawDemo done.\r\n"); LOG(buff);
    #endif
}

// ##### Demoҳ �Ե�ʳ��ʱ������һ�Σ����»����ϵķ���
void DISP_updateDemoScore(u16 maxDemoScore, u16 nowDemoScore){
    // ������Ϣ�����·���
    sprintf(buff, " =%3u", nowDemoScore);
    devShowString(FRAME_THICKNESS + 10 + BLOCK_VIEW_FONT.fontWidth, topInfoY, buff, &FONT_TOP_INFO, COLOR_BG, COLOR_FO);
}

// ##### Demoҳ ����ʱ������һ�Σ�����һЩ��ʾ����Ŀǰʲô��û��
void DISP_updateDemoGameover(u16 maxDemoScore, u16 avgDemoScore, u16 lastDemoScore){
    //devShowString(0, SCREEN_H - FONT_OTHER_TEXT.fontHeight, "                ", &FONT_ASC8, COLOR_BG, COLOR_FO);
    //sprintf(buff, "Max:%d Avg:%d", maxDemoScore, avgDemoScore, lastDemoScore);
    //devShowString(0, SCREEN_H - FONT_OTHER_TEXT.fontHeight, buff, &FONT_OTHER_TEXT, COLOR_BG, COLOR_FO);
}

// ##### Demoҳ ���ڱ����� Ŀǰû�õ�
void DISP_flashDemo(u8 flashOnOff){
    #if ISDEBUG
    //sprintf(buff, "DISP_flashDemo: flashOnOff=%b2d ...\r\n", flashOnOff); LOG(buff);
    #endif

    

    #if ISDEBUG
    //sprintf(buff, "DISP_flashDemo: flashOnOff=%b2d done.\r\n", flashOnOff); LOG(buff);
    #endif
}

// ##### Gameҳ ��ʼ��ʱ������һ��
void DISP_drawGame(u8 soundOnOff){

    #if ISDEBUG
    sprintf(buff, "DISP_drawGame\r\n"); LOG(buff);
    #endif

    devScreenOFF();

    clearScreen();
    DISP_drawFrame();

    // ������Ϣ
    // ʳ�� = 0  (��ǰ���˶��ٸ�)
    showChar(FRAME_THICKNESS + 10, topFoodIconY, FONT_SNAKE_BLOCK_APPLE, &BLOCK_VIEW_FONT, COLOR_BG, COLOR_FO);
    devShowString(FRAME_THICKNESS + 10 + BLOCK_VIEW_FONT.fontWidth, topInfoY, " =  0", &FONT_TOP_INFO, COLOR_BG, COLOR_FO);
    // ������ʾ����
    devShowString(SCREEN_W - FRAME_THICKNESS - calcStringWidth(STR_GAME_INFO, &FONT_TOP_INFO) - 5, topInfoY, STR_GAME_INFO, &FONT_TOP_INFO, COLOR_BG, COLOR_FO);

    // �ײ���Ϣ
    showStringCenter(SCREEN_H - FONT_BOTTOM_INFO.fontHeight - FRAME_THICKNESS - 1, STR_GAME_MENU, &FONT_BOTTOM_INFO, 0);

    // ��������״̬
    DISP_drawSound(soundOnOff);

    devScreenON();

    #if ISDEBUG
    sprintf(buff, "DISP_drawGame done.\r\n"); LOG(buff);
    #endif
}

// ##### Gameҳ �Ե�ʳ��ʱ������һ�Σ����»����ϵķ���
void DISP_updateGameScore(u16 maxDemoScore, u16 nowDemoScore){

    // ������Ϣ�����·���
    sprintf(buff, " =%3u", nowDemoScore);
    devShowString(FRAME_THICKNESS + 10 + BLOCK_VIEW_FONT.fontWidth, topInfoY, buff, &FONT_TOP_INFO, COLOR_BG, COLOR_FO);
}

// ##### GameOverҳ ��ʼ��ʱ������һ��
void DISP_drawGameOver(u16 score, u16 hiScore){
    u8 buff1[16];
    clearScreen();

    //showStringCenter(5, "__________", &FONT_OTHER_TEXT, 0);

    // ���⣬���������߷֣���ʾ ˢ�¼�¼ ���� ��ʾ��Ϸ����
    if (score > hiScore)
    {
        showStringCenter(TITLE_Y, STR_GAMEOVER_NEWRECORD, &FONT_OTHER_TEXT, 0);
    } else {
        showStringCenter(TITLE_Y, STR_GAMEOVER_GAMEOVER, &FONT_OTHER_TEXT, 0);
    }

    // �÷�
    sprintf(buff1, "%s %d", STR_GAMEOVER_SCORE, score);
    showStringCenter(SCROE_Y, buff1, &FONT_OTHER_TEXT, 0);
    // ��¼
    sprintf(buff1, "%s %d", STR_GAMEOVER_HSCORE, hiScore);
    showStringCenter(HSCROE_Y, buff1, &FONT_OTHER_TEXT, 0);

    //showStringCenter(150, "__________", &FONT_OTHER_TEXT, 0);

    // more info
    showStringCenter(SCREEN_H - FONT_BOTTOM_INFO.fontHeight - 2, STR_GAMEOVER_MORE, &FONT_BOTTOM_INFO, 0);
}

// ##### GameOverҳ ���ڱ�����
void DISP_flashGameOver(u8 flashOnOff, u8 isNewRecord){

    if (isNewRecord)
    {
        // ˢ�¼�¼��̬Ч��
        showStringCenterColor(TITLE_Y, STR_GAMEOVER_NEWRECORD, &FONT_OTHER_TEXT, COLOR_BG, randRGB565(), 0);
    }

    // �ײ���˸���� �����������...
    showStringCenter(190, STR_PRESS_ANY_KEY, &FONT_OTHER_TEXT, flashOnOff);
}

// ##### ��Ϸ����ҳ
void DISP_drawInfo(){
    clearScreen();
    devShowString(20, 30, "This is a port of the EmbSnake(https://gitee.com/slipperstree/EmbSnake) that runs on the Nintendo Game & Watch: Super Mario Bros / Zelda. game. You can find more information from following GitHub page.", &FONT_ASC16, COLOR_BG, COLOR_FO);
    showStringCenterColor(160, "https://github.com/slipperstree/", &FONT_BOTTOM_INFO, COLOR_BG, COLOR_WINLOGO_Y, 0);
    showStringCenterColor(174, "game-and-watch-snake", &FONT_BOTTOM_INFO, COLOR_BG, COLOR_WINLOGO_Y, 0);

    showStringCenter(SCREEN_H - FONT_BOTTOM_INFO.fontHeight - 2, STR_GAMEOVER_INFO_EXIT, &FONT_BOTTOM_INFO, 0);
}

// ##### ��Ϸ����ҳ ���ڱ�����
void DISP_flashInfo(u8 flashOnOff){

}   

// ���ƿ��
void DISP_drawFrame() {
    // ǽ���� = (��Ļ�� - ��Ϸ�����(��ͼSIZE_Y*��Block�����)) / 2
    //FRAME_THICKNESS = (SCREEN_W - SNAKE_SIZE_X * BLOCK_VIEW_FONT.fontWidth) / 2;

    // ��Ϸ�����·�����λ��Y = ��Ļ�� - �ײ��߿�� - �ײ���Ϣ���� - �·�������(Ҳ�Ǳ߿��)
    // ��Ϸ����ײ���Ϣ���߶�
    barBottomY = SCREEN_H - FRAME_THICKNESS - FONT_BOTTOM_INFO.fontHeight - 2 - FRAME_THICKNESS;

    // ��Ϸ�����Ϸ�����λ��Y = ��Ϸ�����·�����λ��Y - ��Ϸ�����(��ͼSIZE_Y*��Block�����) - �Ϸ�������(Ҳ�Ǳ߿��)
    barTopY = barBottomY - (SNAKE_SIZE_Y * BLOCK_VIEW_FONT.fontHeight) - FRAME_THICKNESS;

    // ��Ϸ�����Ϸ���Ϣλ��Y
    topInfoY = (barTopY - FRAME_THICKNESS - FONT_TOP_INFO.fontHeight) / 2;

    topFoodIconY = (barTopY - FRAME_THICKNESS - BLOCK_VIEW_FONT.fontHeight) / 2;

    // ��ʼ����(ʵ�ľ���ƴ��)
    devFillRectange(0, 0, SCREEN_W, FRAME_THICKNESS, COLOR_FRAME);                           // ��
    devFillRectange(0, 0, FRAME_THICKNESS, SCREEN_H, COLOR_FRAME);                           // ��
    devFillRectange(SCREEN_W - FRAME_THICKNESS, 0, FRAME_THICKNESS, SCREEN_H, COLOR_FRAME);   // ��
    devFillRectange(0, barTopY, SCREEN_W, FRAME_THICKNESS, COLOR_FRAME);                     // �Ϻ���
    devFillRectange(0, barBottomY, SCREEN_W, FRAME_THICKNESS, COLOR_FRAME);                  // �º���
    devFillRectange(0, SCREEN_H - FRAME_THICKNESS, SCREEN_W, FRAME_THICKNESS, COLOR_FRAME);   // ��

    // ��Ϸ����Yƫ��
    gameArea_Y_offset = barTopY + FRAME_THICKNESS + GAME_AREA_Y_OFFSET;
    gameArea_X_offset = FRAME_THICKNESS + GAME_AREA_X_OFFSET;
}

// �����;���״̬
void DISP_drawSound(u8 soundOnOff) {
    if (soundOnOff)
    {
        //showStringCenter(SCREEN_H - FONT_BOTTOM_INFO.fontHeight - FRAME_THICKNESS - 1, STR_DEMO_MENU, &FONT_BOTTOM_INFO, 0);
        
    } else {
        // ���ƾ���        
        //devDrawLine(SCREEN_W/2-13, SCREEN_H-34, SCREEN_W/2-17+54, SCREEN_H-7, 3, COLOR_RED);
        //devDrawLine(SCREEN_W/2-13, SCREEN_H-7,  SCREEN_W/2-17+54, SCREEN_H-34, 3, COLOR_RED);
    }
    
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
    // devShowString(0,0,buff,&FONT_OTHER_TEXT,COLOR_BLACK, COLOR_WHITE);
    // while (1);
    
}
