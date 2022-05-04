/**************************************************
 * File  : control.c
 * Author: MANDA (slipperstree@gmail.com)
 * Date  : 2019.11
 * Updt  : 2021.07
 * Func  : 游戏控制器，统管各模块
***************************************************/

#include "common.h"
#include "embMine.h"
#include "control.h"
#include "display.h"
#include "key.h"
#include "saveData.h"
#include "embMineDevice.h"


#if ISDEBUG
extern void devShowString(u16 x,u16 y, u8 *str, Font_Type *fontType, u16 colorBg, u16 colorFont);
extern void gameover();
#endif

u16 nowSpeed;
u16 lastDemoSpeed;

// 画面迁移状态机
#define MODE_WELCOME 0
#define MODE_WELCOME_DEMO 1
#define MODE_GAME 2
#define MODE_BEFROE_GAMEOVER 3
#define MODE_GAMEOVER 4
#define MODE_INFO 5

u8 nowMode;
u16 maxDemoScore, totalDemoScore, totalDemoCnt, avgDemoScore, lastDemoScore;
u16 oldSaveDataBestTime=0, lastGameTime=0;
u8 welcomeWaitTime;
u8 tmpX, tmpY;
u8 flashFlag = 0, flashOnOff = 0;
u8 flashFlagQuick = 0, flashOnOffQuick = 0;

// 各种全局心跳flg，在定时器中递增
u16 ttWalk = 0,ttFlag = 0;
u16 ttFlagQuick = 0;
u16 ttGameTime = 0;

// 游戏难度
#define GAME_LVL_1   10
#define GAME_LVL_2   16
#define GAME_LVL_3   22
u8 gameLevel = 0;

// 游戏时间
u8 isPlaying = FALSE;
u8 isWin = FALSE;
u16 gameSec = 0;

// 当前光标位置
extern u8 gDevCurPosX, gDevCurPosY;

// 上次光标位置（用于删除旧位置上的光标）
u8 lastCurX, lastCurY;

// 锁定光标标记
u8 lockCurFlg;

// 为编译通过，内部函数提前定义部分（外部不需要调用，不放在头文件中）
void goPageWelcome(u8 isStartup);
void eventGame();
void switchSound();

// 临时变量用(sprintf等)
extern u8 buff[128];

// =====================================
// 事物处理大循环，需要在main函数中循环调用
// =====================================
void CTL_run(){
	u16 flashSpeed=0;
    u16 flashSpeedQuick=100;

    KEY_keyscan();

    
    // TODO:G&W? 本来是在定时器中断里面做的，GBA定时器中断有吗？
    ttWalk++;
    ttFlag++;

    #if ISDEBUG
    devShowString(0,0, "D", &FONT32, COLOR_BLACK, COLOR_YELLOW);
    #endif
    
    if (nowMode == MODE_WELCOME_DEMO)
    {
        flashSpeed = 30;
    } else {
        flashSpeed = 30;
    }

    if (ttFlagQuick > flashSpeedQuick)
    {
        ttFlagQuick = 0;
        flashFlagQuick = 1;
        if (flashOnOffQuick == 1)
        {
            flashOnOffQuick = 0;
        } else {
            flashOnOffQuick = 1;
        }
    }

    //TODO: GW 1sec ?
    if (ttGameTime >= 1000) {
        ttGameTime = 0;
        if (isPlaying)
        {
            // 更新游戏时间
            gameSec++;
            DISP_updateTime(gameSec);
        }
        
    }

    if (ttFlag > flashSpeed)
    {
        ttFlag = 0;
        flashFlag = 1;
        if (flashOnOff == 1)
        {
            flashOnOff = 0;
        } else {
            flashOnOff = 1;
        }
    }

    switch (nowMode)
    {
        case MODE_WELCOME:
            #if ISDEBUG && DEBUG_SKIP_HOME
                welcomeWaitTime = 100;
            #endif

            // 欢迎画面超过一段时间（闪烁几次后），进入自动演示Demo模式
            if (welcomeWaitTime >= 10)
            {
                devEnterDemoPage();

                nowMode = MODE_WELCOME_DEMO;

                // 绘制Demo画面
                DISP_drawDemo(gSetting.soundOnOff);
                welcomeWaitTime = 0;

                // 一定要先绘制Demo画面!! 保证框架的位置被正确设置
                MINE_restart();

                // restart之后绘制游戏初期状态，绘制全地图
                DISP_drawAllMap();
            } else {
                // 画面动态效果
                if (flashFlag) {
                    DISP_flashWelcome(flashOnOff);
                    //禁用Demo模式，注释掉下面这句即可
                    //welcomeWaitTime++;
                }
            }
            break;

        case MODE_WELCOME_DEMO:

            // DO NOTHING
            
            break;

        case MODE_GAME:
            // 在没有锁定光标的时候，不停获取当前光标位置并绘制
            if(!lockCurFlg){
                devUpdateCurPos();
                if(gDevCurPosX != lastCurX || gDevCurPosY != lastCurY){
                    DISP_hideCusor(lastCurX, lastCurY);
                    DISP_showCusor(gDevCurPosX, gDevCurPosY);

                    lastCurX = gDevCurPosX;
                    lastCurY = gDevCurPosY;
                }
            }

            // 闪烁光标
            if (flashFlagQuick) {
                if (flashOnOffQuick)
                {
                    DISP_hideCusor(gDevCurPosX, gDevCurPosY);
                } else {
                    DISP_showCusor(gDevCurPosX, gDevCurPosY);
                }
            }
            
            break;

        case MODE_BEFROE_GAMEOVER:
            // TODO:这里判断是否失败如果是失败则闪烁暴雷的地方，如果是通关则可在右上角显示一个笑脸
            if (flashFlag == 1) {
                DISP_flashBeforeGameOver(flashOnOff, isWin);
            }
            break;

        case MODE_GAMEOVER:
            // Gameover画面动态效果
            if (flashFlag == 1) {
                if (gameLevel == GAME_LVL_1)
                {
                    DISP_flashGameOver(flashOnOff, (isWin && lastGameTime < oldSaveDataBestTime) ? TRUE : FALSE , "简单");
                } else if (gameLevel == GAME_LVL_2) {
                    DISP_flashGameOver(flashOnOff, (isWin && lastGameTime < oldSaveDataBestTime) ? TRUE : FALSE , "普通");
                } else if (gameLevel == GAME_LVL_3) {
                    DISP_flashGameOver(flashOnOff, (isWin && lastGameTime < oldSaveDataBestTime) ? TRUE : FALSE , "困难");
                }
            }
            break;
        case MODE_INFO:
            if (flashFlag == 1) {
                DISP_flashInfo(flashOnOff);
            }
            break;
        default:
            break;
    }

    // 恢复标志,避免在等于1的期间反复刷新
    flashFlag = 0;
}

// =====================================
// 按键共通处理
// =====================================
void doBtnCommon(u8 btnNo, u8 event_id){

    switch (nowMode)
    {
    // 标题页 -----------------------------------
    case MODE_WELCOME:
        switch (event_id)
        {
        // 按键按下
        case KEY_EVENT_DOWN:
            // LOG("KEY_EVENT_DOWN in MODE_WELCOME\r\n");
            // sprintf(buff, "btn=%b2d nowModeA=%b2d\r\n", btnNo, nowMode);LOG(buff);

            devEnterGamePage();

            // 初始化光标位置
            lastCurX = MINE_SIZE_X / 2;
            lastCurY = MINE_SIZE_Y / 2;

            // 初始化锁定光标标记（解锁状态）
            lockCurFlg = 0;

            // 继续判断是哪个按键
            switch (btnNo)
            {
            case KEY_A:
                // HOME-简单
                gameLevel = GAME_LVL_1;
                break;
            case KEY_B:
                // HOME-普通
                gameLevel = GAME_LVL_2;
                break;
            case KEY_TIME:
                // HOME-困难
                gameLevel = GAME_LVL_3;
                break;
            default:
                break;
            }

            // 任意键 从标题画面 进入游戏画面
            nowMode = MODE_GAME;

            // 关屏
            devScreenOFF();

            // 绘制游戏画面
            DISP_drawGame(gSetting.soundOnOff, gameLevel);

            // MINE_restart之前一定要先绘制游戏画面!! 保证框架的位置被正确设置
            MINE_setMineNUM(gameLevel);
            MINE_restart();

            // 游戏时间清零
            gameSec = 0;

            // 游戏状态恢复
            isWin = FALSE;

            // restart之后绘制游戏初期状态，绘制全地图
            DISP_drawAllMap();

            // 开屏
            devScreenON();
            break;
        default:
            break;
        }
        break;
    // Demo页 -----------------------------------
    case MODE_WELCOME_DEMO:
        // DO NOTHING
        break;
    // 游戏页 -----------------------------------
    case MODE_GAME:
        switch (event_id)
        {
        // 按键按下
        case KEY_EVENT_DOWN:
            // LOG("KEY_EVENT_DOWN in MODE_GAME\r\n");
            // sprintf(buff, "btn=%b2d nowModeA=%b2d\r\n", btnNo, nowMode);LOG(buff);

            switch (btnNo)
            {
            case KEY_LEFT:
                // Game-按键1=锁定光标
                lockCurFlg = 1;
                DISP_drawLock();
                break;
            case KEY_B:
                // Game-按键2=插旗
                devPlaySound(SOUND_FLAG);
                MINE_switchFlag(gDevCurPosX, gDevCurPosY);
                break;
            case KEY_A:
                // Game-按键3=点击
                devPlaySound(SOUND_CLICK);
                MINE_click(gDevCurPosX, gDevCurPosY);
                break;
            default:
                break;
            }
            break;
        // 按键抬起
        case KEY_EVENT_UP:
            switch (btnNo)
            {
            case KEY_LEFT:
                // Game-按键1=释放光标
                lockCurFlg = 0;
                DISP_drawUnLock();
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        break;
    // 游戏结束时的过渡状态，按任意键进入游戏结束画面
    case MODE_BEFROE_GAMEOVER:
        switch (event_id)
        {
        // 按键按下
        case KEY_EVENT_DOWN:

            devEnterGameOverPage();

            lastGameTime = gameSec;
            if (gameLevel == GAME_LVL_1)
            {
                oldSaveDataBestTime = gSetting.hiScoreLvl1;
                DISP_drawGameOver(isWin, "简单", lastGameTime, oldSaveDataBestTime);
            } else if (gameLevel == GAME_LVL_2) {
                oldSaveDataBestTime = gSetting.hiScoreLvl2;
                DISP_drawGameOver(isWin, "普通", lastGameTime, oldSaveDataBestTime);
            } else if (gameLevel == GAME_LVL_3) {
                oldSaveDataBestTime = gSetting.hiScoreLvl3;
                DISP_drawGameOver(isWin, "困难", lastGameTime, oldSaveDataBestTime);
            }

            //如果得分超过了最高分则更新最高分并保存
            if (isWin){
                if (gameLevel == GAME_LVL_1 && lastGameTime < gSetting.hiScoreLvl1)
                {
                    gSetting.hiScoreLvl1 = lastGameTime;
                    SD_saveSetting();
                }
                if (gameLevel == GAME_LVL_2 && lastGameTime < gSetting.hiScoreLvl2)
                {
                    gSetting.hiScoreLvl2 = lastGameTime;
                    SD_saveSetting();
                }
                if (gameLevel == GAME_LVL_3 && lastGameTime < gSetting.hiScoreLvl3)
                {
                    gSetting.hiScoreLvl3 = lastGameTime;
                    SD_saveSetting();
                }
            }
            
            //迁移至gameover画面
            nowMode = MODE_GAMEOVER;
            break;
        default:
            break;
        }
        break;
    // GameOver页 -----------------------------------
    case MODE_GAMEOVER:
        switch (event_id)
        {
        // 按键按下
        case KEY_EVENT_DOWN:
            if (btnNo == KEY_PAUSE) {
                // 进入介绍页
                DISP_drawInfo();
                nowMode = MODE_INFO;
            } else {
                // 其他键回到标题画面
                goPageWelcome(DISP_NO);
            }
            
            break;
        default:
            break;
        }
        
        break;
    // 介绍页 -----------------------------------
    case MODE_INFO:
        // 按任意键回到标题画面
        switch (event_id)
        {
        // 按键按下
        case KEY_EVENT_DOWN:
            goPageWelcome(DISP_NO);
            break;
        default:
            break;
        }
        
        break;
    default:
        break;
    }
}

// =====================================
// 按键个别处理，响应按键事件
// =====================================
void eventKey1(u8 event_id){
    doBtnCommon(KEY_UP, event_id);
}

void eventKey2(u8 event_id){
    doBtnCommon(KEY_DOWN, event_id);
}

void eventKey3(u8 event_id){
    doBtnCommon(KEY_LEFT, event_id);
}

void eventKey4(u8 event_id){
    doBtnCommon(KEY_RIGHT, event_id);
}

void eventKey5(u8 event_id){
    doBtnCommon(KEY_A, event_id);
}

void eventKey6(u8 event_id){
    doBtnCommon(KEY_B, event_id);
}

void eventKey7(u8 event_id){
    doBtnCommon(KEY_GAME, event_id);
}

void eventKey8(u8 event_id){
    doBtnCommon(KEY_TIME, event_id);
}

void eventKey9(u8 event_id){
    doBtnCommon(KEY_PAUSE, event_id);
}

void powerKey(u8 event_id){
    if (event_id == KEY_EVENT_DOWN)
    {
        // EnterDeepSleep
        // Stop SAI DMA (audio)
        //HAL_SAI_DMAStop(&hsai_BlockA1);

        // Enable wakup by PIN1, the power button（should disabe it in main() for as an input button）
        HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1_LOW);

        lcd_backlight_off();

        HAL_PWR_EnterSTANDBYMode();

        // Execution stops here, this function will not return
        while(1) {
            // If we for some reason survive until here, let's just reboot
            HAL_NVIC_SystemReset();
        }
    }

}

void eventGame(){
    switch (MineEventId)
    {
    case MINE_EVENT_UPDATE_BLOCK:
        // 只在游戏或者DEMO页响应这个事件（避免非游戏状态动态设置墙体时刷新画面）
        if (nowMode != MODE_GAME && nowMode != MODE_WELCOME_DEMO)
        {
            // 非游戏/Demo页面不更新画面
            //return;
        }
        
        DISP_updateGameBlock(MineEventParam1, MineEventParam2);
        break;
    case MINE_EVENT_RESTART:
        if (nowMode == MODE_WELCOME_DEMO){
            // DO NOTHING
        } else {
            //TODO:
        }
        break;
    case MINE_EVENT_GAMEOVER:

        if (nowMode == MODE_GAME)
        {   
            // 参数1为是否胜利 暂时没用 可用来显示胜利失败什么的
            isWin = MineEventParam1;
            if(isWin == FALSE){
                // 游戏失败，踩到雷了，让LED爆闪一下
                LED_ALL_ON();
                delay_ms(30);
                LED_ALL_OFF();
            }
            // 跳转到游戏结束前状态（画面不迁移，便于观察游戏结果）
            nowMode = MODE_BEFROE_GAMEOVER;
            // 停止计时
            isPlaying = FALSE;
        } else if (nowMode == MODE_WELCOME_DEMO) {
            // DO NOTHING
        }
        
        break;
    case MINE_EVENT_UPDATE_FLAG_CNT:
        if (nowMode == MODE_GAME)
        {
            // 刷新剩余旗子数量
            DISP_updateFlagCnt(MineEventParam1);
        }
        break;
    case MINE_EVENT_TIMESTART:
        // 开始计时
        isPlaying = TRUE;
        break;
    default:
        break;
    }
}

void goPageWelcome(u8 isStartup) {
    ttWalk = 0;
    ttFlag = 0;
    welcomeWaitTime = 0;

    devEnterHomePage();

    // 显示标题画面
    LOG("---- 显示标题画面 start\r\n");
    DISP_drawWelcome(isStartup);
    LOG("---- 显示标题画面 end\r\n");

    nowMode = MODE_WELCOME;
}

void switchSound(){
    if (gSetting.soundOnOff)
    {
        gSetting.soundOnOff = 0;
    }else{
        gSetting.soundOnOff = 1;
    }

    DISP_drawSound(gSetting.soundOnOff);
    
    // 每次切换声音都做保存，反正据说擦写10万次，写着玩儿呗
    // TODO: 理想的做法是在设置页面里统一保存
    SD_saveSetting();
}

void CTL_init() {

    u8 x, y, sdCheck;
    
    // 读取配置，防止首次读取失败先设置好默认值供设备初次保存
    gSetting.hiScoreLvl1     = 5999;                 //最高分=99:59(因为扫雷用的是时间所以默认记录设置一个很大的数) 5999=99分59秒
    gSetting.hiScoreLvl2     = 5999;                 //最高分=99:59(因为扫雷用的是时间所以默认记录设置一个很大的数)
    gSetting.hiScoreLvl3     = 5999;                 //最高分=99:59(因为扫雷用的是时间所以默认记录设置一个很大的数)
    gSetting.soundOnOff      = 1;                    //开启声音
    gSetting.colorBackGround = COLOR_DEFAULT_BACK;   //默认背景色
    gSetting.colorFront      = COLOR_DEFAULT_FRONT;  //默认文字色
    gSetting.colorFrame      = COLOR_DEFAULT_FRAME;  //默认框架颜色
    SD_loadSetting();

    DISP_setBackColor(gSetting.colorBackGround);
    DISP_setForeColor(gSetting.colorFront);
    DISP_setFrameColor(gSetting.colorFrame);

    DISP_init();
    devSndInit();
    KEY_init(eventKey1, eventKey2, eventKey3, eventKey4, eventKey5, eventKey6, eventKey7, eventKey8, eventKey9, powerKey);
    MINE_init(eventGame);

    goPageWelcome(DISP_YES);

    LOG("-- CTL_init end\r\n");
}
