/**************************************************
 * File  : embSnakeDevice.h
 * Author: MANDA (slipperstree@gmail.com)
 * Date  : 2019.11
 * Updt  : 2021.07
 * Func  : 设备驱动模块头文件，用于适配不同的设备
***************************************************/

#ifndef __EMB_SNAKE_DEVICE_H_
#define __EMB_SNAKE_DEVICE_H_

#include "common.h"

// *******************************************************************************************************
// * 按键配置，根据需要修改以下宏定义
// *******************************************************************************************************

    // ******* 1-根据需要放开/注释下面的定义 ***********************************

        // 放开该定义=响应按下（keyDown）和抬起(keyUp)事件，
        //   同时会禁用按击（click）和双击(doubleClick)事件
        // 注释该定义=响应单击（click：按下并松手才会触发）和双击(doubleClick：快速单击两次)事件，
        //   同时会禁用按下（keyDown）和抬起(keyUp)事件
        #define USE_KEY_DOWN_UP

        // 放开该定义=禁用双击事件，可小幅节约程序体积，不需要双击事件的话保持放开状态即可
        // 注释该定义=启用用双击事件，但前提是上面的 USE_KEY_DOWN_UP 也需要同时被注释，否则依然不会启用双击事件
        #define NOT_USE_DBCLICK

    // ******* 2-根据硬件电气连接修改下列定义 ***********************************

        // ---------- 按键 按下状态和电平状态对应关系，根据具体电路修改 ----------
        // 按键按下为真
        #define KEY_DOWN_VALUE 1
        // 按键未按下为假
        #define KEY_UP_VALUE   0

        // ---------- 按键个数 ----------
        #define KEY_CNT  10

        // ---------- 按键 IO口定义 ----------
        // BTNn的名字不要修改，预留了最多八个按键，不需要的无视即可
        #define BTN1   (buttons & B_Up)?KEY_DOWN_VALUE:KEY_UP_VALUE
        #define BTN2   (buttons & B_Down)?KEY_DOWN_VALUE:KEY_UP_VALUE
        #define BTN3   (buttons & B_Left)?KEY_DOWN_VALUE:KEY_UP_VALUE
        #define BTN4   (buttons & B_Right)?KEY_DOWN_VALUE:KEY_UP_VALUE
        #define BTN5   (buttons & B_A)?KEY_DOWN_VALUE:KEY_UP_VALUE
        #define BTN6   (buttons & B_B)?KEY_DOWN_VALUE:KEY_UP_VALUE
        #define BTN7   (buttons & B_GAME)?KEY_DOWN_VALUE:KEY_UP_VALUE
        #define BTN8   (buttons & B_TIME)?KEY_DOWN_VALUE:KEY_UP_VALUE
        #define BTN9   (buttons & B_PAUSE)?KEY_DOWN_VALUE:KEY_UP_VALUE
        #define BTN10  (buttons & B_POWER)?KEY_DOWN_VALUE:KEY_UP_VALUE

    // ******* 3-根据需要适当修改下列用于判断单击，双击等各种按键动作判定的时间长短 ************
    // *******   并非以ms为单位，请根据实际速度调整到合适的数值） **************************
        
        // 判断单击用，按下和弹起之间不能超过多久 (USE_KEY_DOWN_UP 未定义时才会起作用)
        #define TICK_WAIT_CLICK_END         0x4000
        // 判断双击用，两次单击之间不能超过多久（如果判断成双击则单击无效）(USE_KEY_DOWN_UP 且 NOT_USE_DBCLICK 未定义时才会起作用)
        #define TICK_WAIT_DBCLICK_START     0x0050
        //判断按住不放用，按下以后需要持续多久
        #define TICK_KEEPDOWN               0xA000    
        //按住不放的时候，每隔多久自动触发一次按住不放的事件
        #define TICK_KEEPDOWN_NEXT          0x1000    

    // ******* 4-根据需要修改按键消抖动等待时间 *******
        #define XIAO_DOU 1

// *******************************************************************************************************
// * 显示配置，根据需要修改以下宏定义
// *******************************************************************************************************

    // 屏幕分辨率
    #define SCREEN_W  320
    #define SCREEN_H  240
    // #define SCREEN_W  240
    // #define SCREEN_H  160

    // 元素颜色设置
    // 默认背景/前景（程序中可通过 DISP_setBackColor/DISP_setForeColor 随时修改）
    #define COLOR_DEFAULT_BACK      COLOR_BLACK
    #define COLOR_DEFAULT_FRONT     COLOR_SKYLT
    // 开始页大标题
    #define COLOR_TITLE1            COLOR_SKYLT
    #define COLOR_TITLE2            COLOR_GREENLT
    // 蛇/食物
    #define COLOR_DEFAULT_SNAKE     0xa50c
    #define COLOR_DEFAULT_APPLE     COLOR_YELLOW
    // 游戏界面框架颜色
    #define COLOR_DEFAULT_FRAME     COLOR_DEFAULT_FRONT

    // 使用哪套位图字体绘制食物/蛇/障碍物等(在font.h / font.c中定义)
    #define BLOCK_VIEW_FONT FONT_IMG_SNAKE12

    // 框架厚度
    #define FRAME_THICKNESS 1

    // 游戏区域左上角坐标偏移
    #define GAME_AREA_X_OFFSET 2
    #define GAME_AREA_Y_OFFSET 0

    // 界面语言
    #define UI_LANG_EN
    // #define UI_LANG_CN



// *******************************************************************************************************
// * 接口定义，不用修改
// *******************************************************************************************************

// 初始化显示设备
// bgColor : 背景色，可用于初期清屏
void devDisplayInit(u16 bgColor);
// 填充一个矩形区域
void devFillRectange(u16 x, u16 y, u16 width, u16 height, u16 color);
// 为绘制一个区域做准备。（外部调用了这个函数后会批量调用devPointInDrawArea来绘制各种图案或点阵文字）
void devPrepareForDrawArea(u16 startX, u16 startY, u16 areaWidth, u16 areaHeight);
// 绘制一个指定颜色的点，无需考虑坐标位置。
void devPointInDrawArea(u16 color);
// 显示一个字符 / 汉字
// 根据传入的字库数组和偏移位置，以及字符需要绘制的区域，绘制该字符
void devShowChar(u16 x, u16 y, 
						u16 fontWidth, u16 fontHeight, u16 charDataStartIndex, 
						u8* fontData, 
						u16 colorBg, u16 colorFont);
// 绘制直线
void devDrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 width, u16 color);
// 关闭屏幕显示(跟函数devScreenON搭配使用，如无必要可不用实现留空即可，实现了更好，可以防止刷新画面的过程被用户看见)
void devScreenOFF();
// 打开屏幕显示(跟函数devScreenOFF搭配使用，如无必要可不用实现留空即可，实现了更好，可以防止刷新画面的过程被用户看见)
void devScreenON();

// 进入主画面时会调用这个函数，如有需要请实现想要的效果(比如关闭LED)
void devEnterHomePage();
// 进入Demo画面时会调用这个函数，如有需要请实现想要的效果
void devEnterDemoPage();
// 进入游戏画面时会调用这个函数，如有需要请实现想要的效果
void devEnterGamePage();
// 进入GameOver画面时会调用这个函数，如有需要请实现想要的效果
void devEnterGameOverPage();

// 当玩家改变游戏速度时会调用这个函数
void devSpeedChanged(u16 speed);
// 播放声音
typedef enum
{
    SOUND_EAT_APPLE = 0 ,
    SOUND_MOVE = 1 ,
    SOUND_DEAD = 2 ,
    SOUND_HISCORE = 3 ,
    SOUND_MENU = 4, 
    SOUND_GAMEOVER = 5
} Sound_Type;
void devPlaySound(Sound_Type soundType);

void devSndInit();
void devSndBeepShort();

// 设置保存/读取
typedef struct
{
    u16  hiScore;
    u8 soundOnOff;
    u16  colorBackGround;
    u16  colorFront;
    u16  colorSnake;
    u16  colorApple;
    u16  colorFrame;
} SaveData_Struct;
void devLoadSetting(SaveData_Struct *setting);
void devSaveSetting(SaveData_Struct *setting);

#endif
