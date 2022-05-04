#ifndef __EMB_MINE_DISPLAY_H_
#define __EMB_MINE_DISPLAY_H_

#include "common.h"
#include "color.h"
#include "embMineDevice.h"

// --- 以下不要修改 ------------------------------------------------------------------------------------------------------------
#define DISP_YES 1
#define DISP_NO  0

void DISP_init(void);     						// 初始化（一般用于调用底层初始化函数）

void DISP_drawAllMap();
void DISP_updateGameBlock(u8 x, u8 y);    // 更新地图

void DISP_setBackColor(u16 color);
void DISP_setForeColor(u16 color);
void DISP_setFrameColor(u16 color);

void DISP_drawWelcome(u8 isStartUp);            // 绘制欢迎页
void DISP_flashWelcome(u8 flashOnOff, u8 gamelevel);          // 绘制欢迎页闪烁文字

void DISP_flashBeforeGameOver(u8 flashOnOff, u8 isWin); // 绘制游戏结束前面一页

void DISP_drawGameOver(u8 isWin, u8* levelStr, u16 score, u16 hiScoreLvl1);       // 绘制游戏结束页
void DISP_flashGameOver(u8 flashOnOff, u8 isNewRecord, u8* levelStr);         // 绘制游戏结束页闪烁文字

void DISP_drawInfo();                 // 绘制游戏介绍页
void DISP_flashInfo(u8 flashOnOff);   // 绘制游戏介绍页闪烁文字

void DISP_drawDemo(u8 soundOnOff);              // 绘制Demo页
void DISP_updateDemoScore(u16, u16);            // 更新Demo页(运行中)
void DISP_updateDemoGameover(u16, u16, u16);    // 更新Demo页(死亡后)
void DISP_flashDemo(u8 flashOnOff);             // 绘制Demo页闪烁文字

void DISP_drawGame(u8 soundOnOff, u8 flagCnt);  // 绘制Game页
void DISP_updateFlagCnt(u8 flagCnt);            // 更新Game页的旗子数
void DISP_updateTime(u16 sec);                  // 更新Game页的时间

void DISP_drawFrame();                          // 绘制框架

void DISP_hideCusor(u8 x, u8 y);          // 隐藏光标
void DISP_showCusor(u8 x, u8 y);          // 显示光标

void DISP_drawSound(u8 soundOnOff);             // 声音和静音状态
void DISP_drawUnLock();
void DISP_drawLock();

#endif
