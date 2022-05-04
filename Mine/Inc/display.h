#ifndef __EMB_MINE_DISPLAY_H_
#define __EMB_MINE_DISPLAY_H_

#include "common.h"
#include "color.h"
#include "embMineDevice.h"

// --- ���²�Ҫ�޸� ------------------------------------------------------------------------------------------------------------
#define DISP_YES 1
#define DISP_NO  0

void DISP_init(void);     						// ��ʼ����һ�����ڵ��õײ��ʼ��������

void DISP_drawAllMap();
void DISP_updateGameBlock(u8 x, u8 y);    // ���µ�ͼ

void DISP_setBackColor(u16 color);
void DISP_setForeColor(u16 color);
void DISP_setFrameColor(u16 color);

void DISP_drawWelcome(u8 isStartUp);            // ���ƻ�ӭҳ
void DISP_flashWelcome(u8 flashOnOff, u8 gamelevel);          // ���ƻ�ӭҳ��˸����

void DISP_flashBeforeGameOver(u8 flashOnOff, u8 isWin); // ������Ϸ����ǰ��һҳ

void DISP_drawGameOver(u8 isWin, u8* levelStr, u16 score, u16 hiScoreLvl1);       // ������Ϸ����ҳ
void DISP_flashGameOver(u8 flashOnOff, u8 isNewRecord, u8* levelStr);         // ������Ϸ����ҳ��˸����

void DISP_drawInfo();                 // ������Ϸ����ҳ
void DISP_flashInfo(u8 flashOnOff);   // ������Ϸ����ҳ��˸����

void DISP_drawDemo(u8 soundOnOff);              // ����Demoҳ
void DISP_updateDemoScore(u16, u16);            // ����Demoҳ(������)
void DISP_updateDemoGameover(u16, u16, u16);    // ����Demoҳ(������)
void DISP_flashDemo(u8 flashOnOff);             // ����Demoҳ��˸����

void DISP_drawGame(u8 soundOnOff, u8 flagCnt);  // ����Gameҳ
void DISP_updateFlagCnt(u8 flagCnt);            // ����Gameҳ��������
void DISP_updateTime(u16 sec);                  // ����Gameҳ��ʱ��

void DISP_drawFrame();                          // ���ƿ��

void DISP_hideCusor(u8 x, u8 y);          // ���ع��
void DISP_showCusor(u8 x, u8 y);          // ��ʾ���

void DISP_drawSound(u8 soundOnOff);             // �����;���״̬
void DISP_drawUnLock();
void DISP_drawLock();

#endif
