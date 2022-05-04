#ifndef __EMB_SNAKE_DISPLAY_H_
#define __EMB_SNAKE_DISPLAY_H_

#include "common.h"
#include "color.h"
#include "embSnakeDevice.h"

// --- ���²�Ҫ�޸� ------------------------------------------------------------------------------------------------------------
#define DISP_YES 1
#define DISP_NO  0

void DISP_init(void);     						// ��ʼ����һ�����ڵ��õײ��ʼ��������

void DISP_drawWall();                           // ���Ƶ�ͼ�ϵ��ϰ��׼����ʼ��Ϸ

void DISP_updateGameBlock(u8 x, u8 y);    // ���µ�ͼ

void DISP_setBackColor(u16 color);
void DISP_setForeColor(u16 color);
void DISP_setSnakeColor(u16 color);
void DISP_setAppleColor(u16 color);
void DISP_setFrameColor(u16 color);

void DISP_drawWelcome(u8 isStartUp);            // ���ƻ�ӭҳ
void DISP_flashWelcome(u8 flashOnOff);          // ���ƻ�ӭҳ��˸����

void DISP_drawGameOver(u16 score, u16 hiScore);                 // ������Ϸ����ҳ
void DISP_flashGameOver(u8 flashOnOff, u8 isNewRecord);         // ������Ϸ����ҳ��˸����

void DISP_drawInfo();                 // ������Ϸ����ҳ
void DISP_flashInfo(u8 flashOnOff);   // ������Ϸ����ҳ��˸����

void DISP_drawDemo(u8 soundOnOff);              // ����Demoҳ
void DISP_updateDemoScore(u16, u16);            // ����Demoҳ(������)
void DISP_updateDemoGameover(u16, u16, u16);    // ����Demoҳ(������)
void DISP_flashDemo(u8 flashOnOff);             // ����Demoҳ��˸����

void DISP_drawGame(u8 soundOnOff);              // ����Gameҳ
void DISP_updateGameScore(u16, u16);            // ����Gameҳ(������)

void DISP_drawFrame();                          // ���ƿ��

void DISP_drawSound(u8 soundOnOff);             // �����;���״̬
#endif
