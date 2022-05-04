/******************************************************************************************************
 * 扫雷模块
 * File  : embMine.c
 * Author: MANDA (slipperstree@gmail.com)
 * Date  : 2021.08
 * Updt  : 2021.08
 * Func  : 实现了扫雷的基本数据模型，
 *         但不包含显示逻辑，只包含游戏状态和地图状态管理，与硬件无关可直接移植到各种硬件平台上
 ******************************************************************************************************/

#ifndef __EMB_MINE_H_
#define __EMB_MINE_H_

#include "def_types.h"

// ### 根据需要修改以下定义 ##################################################################

// 地图大小(非屏幕分辨率)
#define MINE_SIZE_X     10
#define MINE_SIZE_Y     10

// 是否使用外部的真随机函数，如果放开这个宏定义表示使用外部的真随机函数，那么需要在外部c文件中实现下列函数。(实现例见common.c)
// 外部需要实现的函数: unsigned int My_real_rand()
// 如果不需要达到真随机的效果，可以注释掉这个定义，会直接使用标准rand()函数。但每次上电后的随机序列都是固定的，是假随机。
#define USE_EXTERN_REAL_RAND_FUNC

// ### 后面的定义不需要改 ###################################################################################

// 事件函数定义
typedef void (*pMineEventFunc)();
extern u16 MineEventId, MineEventParam1, MineEventParam2;

// 方块类型 MINE_EVENT_UPDATE_BLOCK 的参数3的取值
#define MINE_VIEW_NORMAL           0  // 未翻开
#define MINE_VIEW_DIGED            1  // 已翻开
#define MINE_VIEW_FLAG             2  // 插旗
#define MINE_VIEW_MINE             3  // 挖到雷时，显示其他未插旗的雷
#define MINE_VIEW_WRONG            4  // 挖到雷时，显示被并不是雷但被错插上旗子的地方
#define MINE_VIEW_DEADMINE         5  // 挖到雷时，显示爆炸的雷

// 事件ID
#define MINE_EVENT_RESTART         0  // 重启游戏。无参数
#define MINE_EVENT_GAMEOVER        1  // 游戏结束。参数1：胜利1还是失败0
#define MINE_EVENT_UPDATE_BLOCK    2  // 请求刷新指定位置。参数1：需要更新的地图x坐标，参数2：需要更新的地图y坐标
#define MINE_EVENT_UPDATE_FLAG_CNT 3  // 请求刷新剩余旗子数。参数1：旗子数
#define MINE_EVENT_TIMESTART       4  // 请求开始计时（第一次点击时会触发）。无参数

// ### 对外公开函数 ##########################
// 模块初始化，只应该被调用一次
void MINE_init(pMineEventFunc);

// 重置游戏
void MINE_restart(void);

// 挖开
void MINE_click(u8, u8);

// 插旗/拔旗
void MINE_switchFlag(u8, u8);

// 获取当前地图显示用数据MINE_VIEW_XXX 其中一个（可根据返回的ViewId直接刷新画面）
u8 MINE_getView(u8, u8);

// 获取指定位置周围雷的个数（只有当指定位置不为雷才有意义，如果该位置是雷则会返回MINE_STS_MINE(10)，此时不代表该位置周围有10颗雷）
u8 MINE_getViewNum(u8 mapX, u8 mapY);

void MINE_setMineNUM(u8 num);

// 获取当前剩余旗子数
u16 MINE_getNowFlags(void);

#endif
