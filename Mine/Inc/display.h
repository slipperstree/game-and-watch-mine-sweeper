#ifndef __EMB_SNAKE_DISPLAY_H_
#define __EMB_SNAKE_DISPLAY_H_

#include "common.h"
#include "color.h"
#include "embSnakeDevice.h"

// --- 以下不要修改 ------------------------------------------------------------------------------------------------------------
#define DISP_YES 1
#define DISP_NO  0

void DISP_init(void);     						// 初始化（一般用于调用底层初始化函数）

void DISP_drawWall();                           // 绘制地图上的障碍物，准备开始游戏

void DISP_updateGameBlock(u8 x, u8 y);    // 更新地图

void DISP_setBackColor(u16 color);
void DISP_setForeColor(u16 color);
void DISP_setSnakeColor(u16 color);
void DISP_setAppleColor(u16 color);
void DISP_setFrameColor(u16 color);

void DISP_drawWelcome(u8 isStartUp);            // 绘制欢迎页
void DISP_flashWelcome(u8 flashOnOff);          // 绘制欢迎页闪烁文字

void DISP_drawGameOver(u16 score, u16 hiScore);                 // 绘制游戏结束页
void DISP_flashGameOver(u8 flashOnOff, u8 isNewRecord);         // 绘制游戏结束页闪烁文字

void DISP_drawInfo();                 // 绘制游戏介绍页
void DISP_flashInfo(u8 flashOnOff);   // 绘制游戏介绍页闪烁文字

void DISP_drawDemo(u8 soundOnOff);              // 绘制Demo页
void DISP_updateDemoScore(u16, u16);            // 更新Demo页(运行中)
void DISP_updateDemoGameover(u16, u16, u16);    // 更新Demo页(死亡后)
void DISP_flashDemo(u8 flashOnOff);             // 绘制Demo页闪烁文字

void DISP_drawGame(u8 soundOnOff);              // 绘制Game页
void DISP_updateGameScore(u16, u16);            // 更新Game页(运行中)

void DISP_drawFrame();                          // 绘制框架

void DISP_drawSound(u8 soundOnOff);             // 声音和静音状态
#endif
