/**************************************************
 * File  : embMineDevice.c
 * Author: MANDA (slipperstree@gmail.com)
 * Date  : 2019.11
 * Updt  : 2021.07
 * Func  : �豸����ģ�飬������ʵ�ֱ��ļ��ڵĸ���������������豸���ⲿ������Ҫ��ʱ�������Щ����
***************************************************/

#include "embMineDevice.h"
#include "boradSupport.h"
#include "gw_draw.h"

// ��ʱ������(sprintf��)
extern u8 buff[128];

// ��ǰ���λ��
u8 gDevCurPosX = 0, gDevCurPosY = 0;

u16 drawAreaStartX = 0;
u16 drawAreaStartY = 0;
u16 drawAreaEndX = 0;
u16 drawAreaEndY = 0;
u16 drawAreaCurrX = 0;
u16 drawAreaCurrY = 0;

// =========================================================================================
// ==    ��ʾ�豸���    =====================================================================
// =========================================================================================

// ��ʼ����ʾ�豸
// bgColor : ����ɫ�������ڳ�������
void devDisplayInit(u16 bgColor){
    //TODO:G&W? LCD_Init();
    devScreenOFF(); // �ر�LCD��ʾ
    //TODO:G&W? LCD_Clear(bgColor);
    devScreenON();  // ��LCD��ʾ
    //TODO:G&W? TFT_BL = 0;     // ��Һ���������
}

// Ϊ����һ��������׼�������ⲿ������������������������devPointInDrawArea�����Ƹ���ͼ����������֣�
// һ����˵��������õײ��������ʾ���������Լ���ʼ��ͼ����
// LCD_SetArea(startX, startY, startX+areaWidth-1, startY+areaHeight-1);
// LCD_WR_REG(0x2C);
void devPrepareForDrawArea(u16 startX, u16 startY, u16 areaWidth, u16 areaHeight){
    drawAreaStartX = startX;
    drawAreaStartY = startY;
    drawAreaEndX = startX+areaWidth-1;
    drawAreaEndY = startY+areaHeight-1;
    drawAreaCurrX = drawAreaStartX;
    drawAreaCurrY = drawAreaStartY;
}

// ����һ��ָ����ɫ�ĵ㣬���迼������λ�á����ⲿ���ȵ��ú���devPrepareForDrawArea��ȷ����ͼ��Χ��
void devPointInDrawArea(u16 color){

    if (drawAreaCurrX>drawAreaEndX) {
        drawAreaCurrX=drawAreaStartX;
        drawAreaCurrY++;
    }

    // �Ѿ�����ָ����ͼ��Χ��������
    if (drawAreaCurrY>drawAreaEndY || drawAreaCurrY > SCREEN_H || drawAreaCurrX > SCREEN_W) return;

    // �ڵ�ǰλ�û���һ����
    gw_plot(drawAreaCurrX, drawAreaCurrY, color);

    drawAreaCurrX++;
}

// ����ֱ��
void devDrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 width, u16 color){
    gw_line_width(x1, y1, x2, y2, width, color);
}

// ���һ����������
void devFillRectange(u16 x, u16 y, u16 width, u16 height, u16 color){
    //gw_rect(x, y, x+width, y+height, color);
    devPrepareForDrawArea(x, y, width, height);
    u16 wTmp = width;
    u16 hTmp = height;
    while(wTmp--){
        hTmp = height;
        while(hTmp--){
            devPointInDrawArea(color);
        }
    }
}

// �ر���Ļ��ʾ(������devScreenON����ʹ�ã����ޱ�Ҫ�ɲ���ʵ�����ռ��ɣ�ʵ���˸��ã����Է�ֹˢ�»���Ĺ��̱��û�����)
void devScreenOFF(){
    //lcd_backlight_off();
}

// ����Ļ��ʾ(������devScreenOFF����ʹ�ã����ޱ�Ҫ�ɲ���ʵ�����ռ��ɣ�ʵ���˸��ã����Է�ֹˢ�»���Ĺ��̱��û�����)
void devScreenON(){
    //lcd_backlight_on();
}

// =========================================================================================
// ==    �������       =====================================================================
// =========================================================================================
// TODO:

// =========================================================================================
// ==    ������       =====================================================================
// =========================================================================================

// ���µ�ǰ���λ�ã���ѭ���᲻ͣ��������ӿ���ˢ�»����ϵĹ�꣩
void devUpdateCurPos(){
    // TODO: gw��ʹ�������ǣ�Ҫ�ĳɰ������ƹ��ķ�ʽ����ʱ�̶�
    gDevCurPosX = 2;
    gDevCurPosY = 2;
}


// =========================================================================================
// ==    �������       =====================================================================
// =========================================================================================

// ����������EEPROM��ַ
#define SD_ADDR_PAGE_BASE       0x000
#define SD_ADDR_HSCORE_LVL1_H   SD_ADDR_PAGE_BASE
#define SD_ADDR_HSCORE_LVL1_L   SD_ADDR_HSCORE_LVL1_H+1
#define SD_ADDR_HSCORE_LVL2_H   SD_ADDR_HSCORE_LVL1_L+1
#define SD_ADDR_HSCORE_LVL2_L   SD_ADDR_HSCORE_LVL2_H+1
#define SD_ADDR_HSCORE_LVL3_H   SD_ADDR_HSCORE_LVL2_L+1
#define SD_ADDR_HSCORE_LVL3_L   SD_ADDR_HSCORE_LVL3_H+1
#define SD_ADDR_SOUND           SD_ADDR_HSCORE_LVL3_L+1
#define SD_ADDR_COLOR_BG_H      SD_ADDR_SOUND+1
#define SD_ADDR_COLOR_BG_L      SD_ADDR_COLOR_BG_H+1
#define SD_ADDR_COLOR_FO_H      SD_ADDR_COLOR_BG_L+1
#define SD_ADDR_COLOR_FO_L      SD_ADDR_COLOR_FO_H+1
#define SD_ADDR_COLOR_FRAME_H   SD_ADDR_COLOR_FO_L+1
#define SD_ADDR_COLOR_FRAME_L   SD_ADDR_COLOR_FRAME_H+1
#define SD_ADDR_PAGE_LAST5      0x1FB

#define H8(DATA16)              (unsigned char) ((DATA16>>8) & 0xFF)
#define L8(DATA16)              (unsigned char) (DATA16 & 0xFF)
#define HL16(DATA16H8,DATA16L8) (unsigned int)  ((DATA16H8)*256 + (DATA16L8 & 0xFF))
#define READ8(ADDR)             EEPROM_Read_Byte(ADDR)
#define READ16(ADDR)            HL16(EEPROM_Read_Byte(ADDR), EEPROM_Read_Byte(ADDR+1))
#define WRITE8(ADDR,DATA8)      EEPROM_Write_Byte(ADDR, DATA8)
#define WRITE16(ADDR,DATA16)    EEPROM_Write_Byte(ADDR, H8(DATA16));EEPROM_Write_Byte(ADDR+1, L8(DATA16))

//TODO:G&W? ��������
// setting: Ĭ�����á���ֹ�״��ϵ�ʱ����������������Ϸ���ݣ���ʱ�������жϲ����洫���Ĭ�����á�
void devLoadSetting(SaveData_Struct *setting){

    // // ����Ƿ��Ѿ���ʼ������������״��ϵ磬EEPROM�л����������ã���ʱ�����ݲ������ã�
    // // ��鷽���ǿ��������5���ֽڵ������Ƿ���Ԥ��õ����ݡ�
    // EEPROM_Read_Arry(SD_ADDR_PAGE_LAST5, buff, 5);
    // if (buff[0]!='C' || buff[1]!='M' || buff[2]!='I' || buff[3]!='N' || buff[4]!='E')
    // {
    //     // ������ǣ��򱣴洫���Ĭ������
    //     devSaveSetting(setting);
    // } else {
    //     // ������
    //     setting->hiScoreLvl1     = READ16(SD_ADDR_HSCORE_LVL1_H);
    //     setting->hiScoreLvl2     = READ16(SD_ADDR_HSCORE_LVL2_H);
    //     setting->hiScoreLvl3     = READ16(SD_ADDR_HSCORE_LVL3_H);
    //     setting->soundOnOff      = READ8 (SD_ADDR_SOUND);
    //     setting->colorBackGround = READ16(SD_ADDR_COLOR_BG_H);
    //     setting->colorFront      = READ16(SD_ADDR_COLOR_FO_H);
    //     setting->colorFrame      = READ16(SD_ADDR_COLOR_FRAME_H);
    // }
}

//TODO:G&W? 
void devSaveSetting(SaveData_Struct *setting){

    // // �Ȳ�������
    // EEPROM_Erase_Page(SD_ADDR_PAGE_BASE);
    // // д����
    // WRITE16(SD_ADDR_HSCORE_LVL1_H,  setting->hiScoreLvl1);
    // WRITE16(SD_ADDR_HSCORE_LVL2_H,  setting->hiScoreLvl2);
    // WRITE16(SD_ADDR_HSCORE_LVL3_H,  setting->hiScoreLvl3);
    // WRITE8 (SD_ADDR_SOUND,          setting->soundOnOff);
    // WRITE16(SD_ADDR_COLOR_BG_H,     setting->colorBackGround);
    // WRITE16(SD_ADDR_COLOR_FO_H,     setting->colorFront);
    // WRITE16(SD_ADDR_COLOR_FRAME_H,  setting->colorFrame);
    // EEPROM_Write_Str(SD_ADDR_PAGE_LAST5, "CMINE");

}

// =========================================================================================
// ==    �������       =====================================================================
// =========================================================================================

// ����ģ���ʼ��
void devSndInit(){
    // TODO:G&W?
}

void playMaxmodSnd(){
    // TODO:G&W?
}

//  �������������ʱ������
//  5444321551
//  .      ...
//  GFFFEDCGGC
//  .      ...
void playGameOver(){
    // TODO:G&W?
}

void playBeep(u8 beepLen){
    // TODO:G&W?
}

// ��beep����
void playBeepShort(){
    playBeep(1);
}

// ��������
// ��������
void devPlaySound(Sound_Type soundType){

    switch (soundType)
    {
    case SOUND_CLICK:
        playBeepShort();
        break;
    case SOUND_FLAG:
        playBeepShort();
        break;
    case SOUND_HISCORE:
        break;
    case SOUND_MENU:
        break;
    case SOUND_DEAD:
        break;
    case SOUND_GAMEOVER:
        playGameOver();
        break;
    default:
        break;
    }
}

// =========================================================================================
// ==    �������       =====================================================================
// =========================================================================================

// ����������ʱ��������������������Ҫ��ʵ����Ҫ��Ч��(����ر�LED)
// û��Ҫ�������ռ���
void devEnterHomePage(){
    //lcd_backlight_off();
}

// ����Demo����ʱ��������������������Ҫ��ʵ����Ҫ��Ч��
// û��Ҫ�������ռ���
void devEnterDemoPage(){
    
}

// ������Ϸ����ʱ��������������������Ҫ��ʵ����Ҫ��Ч��
// û��Ҫ�������ռ���
void devEnterGamePage(){
    
}

// ����GameOver����ʱ��������������������Ҫ��ʵ����Ҫ��Ч��
// û��Ҫ�������ռ���
void devEnterGameOverPage(){
    
}

// =========================================================================================
// ==    ����          =====================================================================
// =========================================================================================
                                


