/******************************************************************************************************
 * 贪食蛇模块
 * File  : embSnake.c
 * Author: MANDA (slipperstree@gmail.com)
 * Date  : 2019.11
 * Updt  : 2021.07
 * Func  : 实现了贪食蛇的基本数据模型以及一个自动寻找食物的算法，
 *         但不包含显示逻辑，只包含游戏状态和地图状态管理，与硬件无关可直接移植到各种硬件平台上
 ******************************************************************************************************/

#ifndef __EMB_SNAKE_H_
#define __EMB_SNAKE_H_

#include "def_types.h"

// ### 贪食蛇模块 定义 根据需要修改以下定义 ##################################################################

// 地图大小(非屏幕分辨率)
#define SNAKE_SIZE_X  25
#define SNAKE_SIZE_Y  16
// #define SNAKE_SIZE_X  19
// #define SNAKE_SIZE_Y  7

// 演示模式文字显示位置
#define SNAKE_DEMO_TITLE_1_X  5
#define SNAKE_DEMO_TITLE_1_Y  6
#define SNAKE_DEMO_TITLE_2_X  (SNAKE_DEMO_TITLE_1_X+5)
#define SNAKE_DEMO_TITLE_2_Y  (SNAKE_DEMO_TITLE_1_Y)
#define SNAKE_DEMO_TITLE_3_X  (SNAKE_DEMO_TITLE_2_X+5)
#define SNAKE_DEMO_TITLE_3_Y  (SNAKE_DEMO_TITLE_1_Y)
#define SNAKE_DEMO_TITLE_4_X  (SNAKE_DEMO_TITLE_3_X+5)
#define SNAKE_DEMO_TITLE_4_Y  (SNAKE_DEMO_TITLE_1_Y)

// 是否使用外部的真随机函数，如果放开这个宏定义表示使用外部的真随机函数，那么需要在外部c文件中实现下列函数。(实现例见common.c)
// 外部需要实现的函数: u16 My_real_rand()
// 如果不需要达到真随机的效果，可以注释掉这个定义，会直接使用标准rand()函数。但每次上电后的随机序列都是固定的，是假随机。
#define USE_EXTERN_REAL_RAND_FUNC

// ### 后面的定义不需要改 ###################################################################################

// 事件函数定义
typedef void (*pSnakeEventFunc)();
extern u16 SnakeEventId, SnakeEventParam1, SnakeEventParam2;

// 事件 SNAKE_EVENT_UPDATE_BLOCK 的参数3的取值
#define SNAKE_VIEW_APPLE    0
#define SNAKE_VIEW_HEAD_R   1
#define SNAKE_VIEW_HEAD_L   2
#define SNAKE_VIEW_HEAD_U   3
#define SNAKE_VIEW_HEAD_D   4
#define SNAKE_VIEW_BODY_V   5
#define SNAKE_VIEW_BODY_H   6
#define SNAKE_VIEW_BODY_DR  7
#define SNAKE_VIEW_BODY_LD  8
#define SNAKE_VIEW_BODY_LU  9
#define SNAKE_VIEW_BODY_UR  10
#define SNAKE_VIEW_TAIL_R   11
#define SNAKE_VIEW_TAIL_L   12
#define SNAKE_VIEW_TAIL_U   13
#define SNAKE_VIEW_TAIL_D   14
#define SNAKE_VIEW_EMPTY    15
#define SNAKE_VIEW_WALL     16

// 事件ID
#define SNAKE_EVENT_RESTART         0  // 重启游戏。无参数
#define SNAKE_EVENT_GAMEOVER        1  // 游戏结束。参数1：分数
#define SNAKE_EVENT_UPDATE_BLOCK    2  // 请求刷新指定位置。参数1：需要更新的地图x坐标，参数2：需要更新的地图y坐标
#define SNAKE_EVENT_EAT_APPLE       3  // 吃到苹果。参数1：分数
#define SNAKE_EVENT_UPDATE_AI_PATH  4  // 调试用。参数1：需要更新的x坐标，参数2：需要更新的y坐标

// ### 对外公开函数 ##########################
// 模块初始化，只应该被调用一次
void SNAKE_init(pSnakeEventFunc);

// 重置游戏(不会清除地图中的墙壁信息，除非强制调用 SNAKE_init 或者使用 SNAKE_clearWall / SNAKE_delWall)
void SNAKE_restart(void);

// 向指定方向移动蛇头
u8 SNAKE_moveUp(void);
u8 SNAKE_moveDown(void);
u8 SNAKE_moveLeft(void);
u8 SNAKE_moveRight(void);

// 向左转 （以当前朝向的方向为基准 ）
u8 SNAKE_turnLeft(void);
// 向右转 （以当前朝向的方向为基准 ）
u8 SNAKE_turnRight(void);

// 向当前方向前进一步，用于自动前进的功能（本模块自己不负责自动前进，需要在外部定时调用这个函数）
void SNAKE_moveNext(void);

// AI自动向某个方向走一步
void SNAKE_AI_moveNext(void);

// 重新生成一个食物
void SNAKE_reCreateFood(void);

// 从数组导入地图
void SNAKE_loadWall(u8* wallMap);
// 生成一个障碍物
void SNAKE_addWall(u8, u8);
// 清除一个障碍物
void SNAKE_delWall(u8 x, u8 y);
// 清除所有障碍物
void SNAKE_clearWall();

// 向指定方向移动蛇头
void SNAKE_moveAppleUp(void);
void SNAKE_moveAppleDown(void);
void SNAKE_moveAppleLeft(void);
void SNAKE_moveAppleRight(void);

// 获取当前地图指定位置的原始数据,一般不直接使用（包含多个信息在一个字节中）
u8 SNAKE_getMapData(u8, u8);

// 获取当前地图显示用数据,返回值为 SNAKE_VIEW_XXX 其中一个（可根据返回的ViewId直接刷新画面）
u8 SNAKE_getView(u8, u8);

// 获取当前分数
u16 SNAKE_getNowScroe(void);

#ifdef SNAKE_AI_DEBUG
union dfsMapType snake_getPointDataAI(u8, u8);
#endif

#endif
