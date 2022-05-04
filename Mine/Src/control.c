/**************************************************
 * File  : control.c
 * Author: MANDA (slipperstree@gmail.com)
 * Date  : 2019.11
 * Updt  : 2021.07
 * Func  : 贪食蛇游戏控制器，统管snake和display等模块
***************************************************/

#include "common.h"
#include "embSnake.h"
#include "control.h"
#include "display.h"
#include "key.h"
#include "saveData.h"
#include "embSnakeDevice.h"


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
#define MODE_GAMEOVER 3
#define MODE_INFO 4

u8 nowMode;
u16 maxDemoScore, totalDemoScore, totalDemoCnt, avgDemoScore, lastDemoScore;
u16 oldSaveDataHScore=0, lastGameScore=0;
u8 welcomeWaitTime;
u8 tmpX, tmpY;
u8 flashFlag = 0, flashOnOff = 0;

// 各种全局心跳flg，在定时器中递增
u16 ttWalk = 0,ttFlag = 0;

// 为编译通过，内部函数提前定义部分（外部不需要调用，不放在头文件中）
void goPageWelcome(u8 isStartup);
void eventSnake();
void setDemoSpeed(u16 speed);
void switchSound();
void resetDemoWall();

#ifdef SNAKE_AI_DEBUG
u8 isPauseAfterEatAppleMode, isPauseEveryStep, isPause;
void updateBlockAI(u8 x, u8 y);
void updateAllAI();
#endif

// 临时变量用(sprintf等)
extern u8 buff[128];

// =====================================
// 事物处理大循环，需要在main函数中循环调用
// =====================================
void CTL_run(){
	u16 flashSpeed=0;

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
                // 一旦restartSNAKE就会尝试绘制最初的几个block（初始蛇，食物）
                // 这个函数里面会先清除地图上所有内容包括蛇食物和障碍物,然后会回调一个RESTART的事件，如果需要障碍物在事件响应中设置障碍物
                // 回调函数执行完毕后会设置固定的开始位置（左上角横向3个block蛇头向右）并在没有障碍物的空位随机生成一个食物。（所以设置障碍物不可占用左上3个block）
                SNAKE_restart();
            } else {
                // 画面动态效果
                if (flashFlag) {
                    DISP_flashWelcome(flashOnOff);
                    welcomeWaitTime++;
                }
            }
            break;

        case MODE_WELCOME_DEMO:

            #if ISDEBUG
                // 吃N个苹果就强制结束demo
                if (SNAKE_getNowScroe() > DEBUG_DEMO_APPLE){
                    // 模拟gameover
                    LOG("模拟gameover...\r\n");
                    SnakeEventId = SNAKE_EVENT_GAMEOVER;
                    SnakeEventParam1 = SNAKE_getNowScroe();
                    SnakeEventParam2 = 0;
                    eventSnake();
                    LOG("模拟gameover done.\r\n");
                    return;
                }
            #endif

            // AI前进一步
            if (ttWalk >= nowSpeed)
            {
                ttWalk = 0;
                SNAKE_AI_moveNext();

                #ifdef SNAKE_AI_DEBUG
                updateAllAI();
                #endif
            }

            if (flashFlag) {
                DISP_flashDemo(flashOnOff);
            }
            
            break;

        case MODE_GAME:
            // 自动前进
            if (ttWalk >= nowSpeed)
            {
                ttWalk = 0;
                SNAKE_moveNext();
            }
            break;
        case MODE_GAMEOVER:
            // Gameover画面动态效果
            if (flashFlag == 1) {
                DISP_flashGameOver(flashOnOff, lastGameScore > oldSaveDataHScore ? 1 : 0);
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

            // 任意键 从标题画面 进入游戏画面
            nowMode = MODE_GAME;
            // 绘制游戏画面
            DISP_drawGame(gSetting.soundOnOff);

            // SNAKE_restart之前一定要先绘制游戏画面!! 保证框架的位置被正确设置
            SNAKE_restart();
            break;
        default:
            break;
        }
        break;
    // Demo页 -----------------------------------
    case MODE_WELCOME_DEMO:
        switch (event_id)
        {
        // 按键按下
        case KEY_EVENT_DOWN:
            // LOG("KEY_EVENT_DOWN in MODE_WELCOME_DEMO\r\n");
            // sprintf(buff, "btn=%b2d nowModeA=%b2d\r\n", btnNo, nowMode);LOG(buff);

            // 继续判断是哪个按键
            switch (btnNo)
            {
            case KEY_B:
                // Demo-按键B=切换速度
                if (nowSpeed == SPEED_DEMO_L) {
                    setDemoSpeed(SPEED_DEMO_M);
                } else if (nowSpeed == SPEED_DEMO_M) {
                    setDemoSpeed(SPEED_DEMO_H);;
                } else if (nowSpeed == SPEED_DEMO_H) {
                    setDemoSpeed(SPEED_DEMO_S);
                } else if (nowSpeed == SPEED_DEMO_S) {
                    setDemoSpeed(SPEED_DEMO_L);
                }
                break;
            case KEY_GAME:
                devEnterGamePage();
                // 进入游戏画面
                nowMode = MODE_GAME;
                // 绘制游戏画面
                DISP_drawGame(gSetting.soundOnOff);
                // SNAKE_restart之前一定要先绘制游戏画面!! 保证框架的位置被正确设置
                SNAKE_restart();
                break;
            // case KEY_PAUSE:
            //     // Demo-按键SELECT=切换声音
            //     switchSound();
            //    break;
            default:
                // Demo-其他按键=返回标题画面
                goPageWelcome(DISP_NO);
                break;
            }
            break;
        default:
            break;
        }
        break;
    // 游戏页 -----------------------------------
    case MODE_GAME:
        switch (event_id)
        {
        // 按键按下
        case KEY_EVENT_DOWN:
            // LOG("KEY_EVENT_DOWN in MODE_GAME\r\n");
            // sprintf(buff, "btn=%b2d nowModeA=%b2d\r\n", btnNo, nowMode);LOG(buff);

            // 手动移动成功的话，自动前进计时清零
            if ((btnNo == KEY_UP && SNAKE_moveUp()) || 
                (btnNo == KEY_DOWN && SNAKE_moveDown()) ||
                (btnNo == KEY_LEFT && SNAKE_moveLeft()) ||
                (btnNo == KEY_RIGHT && SNAKE_moveRight())
                ) {
                // 手动移动成功的话，自动前进计时清零
                ttWalk = 0;
            }

            // if (btnNo == KEY_PAUSE) {
            //     // 切换声音
            //     switchSound();
            // }

            if (btnNo == KEY_TIME) {
                // 进入Demo
                devEnterDemoPage();
                nowMode = MODE_WELCOME_DEMO;
                // 绘制Demo画面
                DISP_drawDemo(gSetting.soundOnOff);
                SNAKE_restart();
            }
            break;
        // 按键按被按住不放（连发）
        case KEY_EVENT_KEEPING_PRESS:
            if ((btnNo == KEY_UP && SNAKE_moveUp()) || 
                (btnNo == KEY_DOWN && SNAKE_moveDown()) ||
                (btnNo == KEY_LEFT && SNAKE_moveLeft()) ||
                (btnNo == KEY_RIGHT && SNAKE_moveRight())
                )
            {
                SNAKE_moveNext();
                ttWalk = 0;
            }
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

void eventSnake(){
    switch (SnakeEventId)
    {
    case SNAKE_EVENT_UPDATE_BLOCK:
        // 只在游戏或者DEMO页响应这个事件（避免非游戏状态动态设置墙体时刷新画面）
        if (nowMode != MODE_GAME && nowMode != MODE_WELCOME_DEMO)
        {
            // 非游戏/Demo页面不更新画面
            //return;
        }
        
        DISP_updateGameBlock(SnakeEventParam1, SnakeEventParam2);
        break;
    #ifdef SNAKE_AI_DEBUG
    case SNAKE_EVENT_UPDATE_AI_PATH:
        if (SNAKE_getNowScroe() >= 5)
        {
            updateBlockAI(SnakeEventParam1, SnakeEventParam2);
        }
        break;
    #endif
    case SNAKE_EVENT_RESTART:
        // 如果需要用到障碍物功能，必须在这个事件点中设置。且不可使用左上角1X3的位置。（预留给初始蛇身体用）
        // 处理顺序如下
        // 1-Snake模块的restart函数中首先将map全部清除,包括障碍物。
        // 2-Snake模块回调此事件。
        // 3-在此事件中根据需要添加障碍物。（开发者实现）
        // 4-Snake模块在左上角生成1X3大小的蛇的身体。空地随机生成第一个食物。游戏开始。
        // 所以，早了会在1处被清除。晚了有可能跟在4处随机生成的食物位置冲突。你并不知道食物在哪里，所以要在随机生成食物之前，Map清除之后添加障碍物。
        if (nowMode == MODE_WELCOME_DEMO){
            // 设置Demo画面的障碍物
            resetDemoWall();
            DISP_drawWall();
            setDemoSpeed(lastDemoSpeed);
        } else {
            nowSpeed = SPEED_DEFAULT;
        }
        break;
    case SNAKE_EVENT_GAMEOVER:

        if (nowMode == MODE_GAME)
        {
            if (gSetting.soundOnOff)
            {
                devPlaySound(SOUND_GAMEOVER);
            }

            //先停一会死掉的状态(TODO:GBA 这里会影响死掉时候的maxmod声音播放？？)
            //TODO:这里不要用延时，改成订阅等待帧数的方式，等帧数到达指定数字之后执行回调函数。
            My_delay_ms(2000);

            devEnterGameOverPage();

            lastGameScore = SnakeEventParam1;
            oldSaveDataHScore = gSetting.hiScore;
            DISP_drawGameOver(lastGameScore, oldSaveDataHScore);

            //如果得分超过了最高分则更新最高分并保存
            if (lastGameScore > gSetting.hiScore)
            {
                gSetting.hiScore = lastGameScore;
                SD_saveSetting();
            }

            nowMode = MODE_GAMEOVER;
        } else if (nowMode == MODE_WELCOME_DEMO) {
            lastDemoScore = SnakeEventParam1;

            // 平均得分 TODO: 计算有问题，每次只跟上一次做平均数是不对的
            totalDemoCnt++;
            totalDemoScore += SnakeEventParam1;
            avgDemoScore = totalDemoScore / totalDemoCnt;
            
            // 最高得分
            if (SnakeEventParam1 > maxDemoScore)
            {
                maxDemoScore = SnakeEventParam1;
            }

            // 显示最高分，平均分等信息
            DISP_updateDemoGameover(maxDemoScore, avgDemoScore, lastDemoScore);

            #if !(ISDEBUG && DEBUG_DEMO_GAMEOVER_NOWAIT)
                // 停一会
                My_delay_ms(3000);
            #endif
            
            // 回到欢迎页
            goPageWelcome(DISP_NO);
        }
        
        break;
    case SNAKE_EVENT_EAT_APPLE:

        if (gSetting.soundOnOff)
        {
            // 播放声音
            devPlaySound(SOUND_EAT_APPLE);
        }
        
        if (nowMode == MODE_WELCOME_DEMO)
        {
            // 刷新分数
            DISP_updateDemoScore(maxDemoScore, SnakeEventParam1);
        } else {
            // 刷新分数
            DISP_updateGameScore(maxDemoScore, SnakeEventParam1);

            // 手动游戏才改变速度
            nowSpeed -= SPEED_INTERVAL;
            if (nowSpeed <= SPEED_MAX) 
            {
                // 设置速度上限
                nowSpeed = SPEED_MAX;
            }
        }
        
        break;
    default:
        break;
    }
}

#ifdef SNAKE_AI_DEBUG
void updateBlockAI(u8 x, u8 y){
    switch (snake_getPointDataAI(x, y).subDatas.status)
    {
    case AI_STATUS_ON_THE_WAY:
        //OLED_ShowChar(x*8,y,'O',8);
        break;
    case AI_STATUS_WAY_TO_DEATH:
        //OLED_ShowChar(x*8,y,'X',8);
        break;
    case AI_STATUS_WAY_TO_APPLE:
        OLED_ShowChar(x*8,y,'.',8);
        break;
    default:
        break;
    }
}

void updateAllAI(){
    u8 x,y;
	for (x = 0; x < SNAKE_SIZE_X; ++x)
	{
		for (y = 0; y < SNAKE_SIZE_Y; ++y)
		{
            if (SNAKE_getMapData(x,y) == STS_EMPTY)
            {
                OLED_ShowChar(x*8,y,' ',8);
            }
		}
	}
}
#endif

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

void setDemoSpeed(u16 speed){
    nowSpeed = speed;
    lastDemoSpeed = speed;
    devSpeedChanged(speed);
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

void resetDemoWall(){
    // 清除当前障碍物
    SNAKE_clearWall();

    //「演示模式」所在区域设置为障碍物
    SNAKE_addWall(SNAKE_DEMO_TITLE_1_X,   SNAKE_DEMO_TITLE_1_Y);
    SNAKE_addWall(SNAKE_DEMO_TITLE_1_X+1, SNAKE_DEMO_TITLE_1_Y);
    SNAKE_addWall(SNAKE_DEMO_TITLE_1_X,   SNAKE_DEMO_TITLE_1_Y+1);
    SNAKE_addWall(SNAKE_DEMO_TITLE_1_X+1, SNAKE_DEMO_TITLE_1_Y+1);

    SNAKE_addWall(SNAKE_DEMO_TITLE_2_X,   SNAKE_DEMO_TITLE_2_Y);
    SNAKE_addWall(SNAKE_DEMO_TITLE_2_X+1, SNAKE_DEMO_TITLE_2_Y);
    SNAKE_addWall(SNAKE_DEMO_TITLE_2_X,   SNAKE_DEMO_TITLE_2_Y+1);
    SNAKE_addWall(SNAKE_DEMO_TITLE_2_X+1, SNAKE_DEMO_TITLE_2_Y+1);

    SNAKE_addWall(SNAKE_DEMO_TITLE_3_X,   SNAKE_DEMO_TITLE_3_Y);
    SNAKE_addWall(SNAKE_DEMO_TITLE_3_X+1, SNAKE_DEMO_TITLE_3_Y);
    SNAKE_addWall(SNAKE_DEMO_TITLE_3_X,   SNAKE_DEMO_TITLE_3_Y+1);
    SNAKE_addWall(SNAKE_DEMO_TITLE_3_X+1, SNAKE_DEMO_TITLE_3_Y+1);

    SNAKE_addWall(SNAKE_DEMO_TITLE_4_X,   SNAKE_DEMO_TITLE_4_Y);
    SNAKE_addWall(SNAKE_DEMO_TITLE_4_X+1, SNAKE_DEMO_TITLE_4_Y);
    SNAKE_addWall(SNAKE_DEMO_TITLE_4_X,   SNAKE_DEMO_TITLE_4_Y+1);
    SNAKE_addWall(SNAKE_DEMO_TITLE_4_X+1, SNAKE_DEMO_TITLE_4_Y+1);
}

void CTL_init() {

    u8 x, y, sdCheck;
    
    // 读取配置，防止首次读取失败先设置好默认值供设备初次保存
    gSetting.hiScore         = 0;                    //最高分=0
    gSetting.soundOnOff      = 1;                    //开启声音
    gSetting.colorBackGround = COLOR_DEFAULT_BACK;   //默认背景色
    gSetting.colorFront      = COLOR_DEFAULT_FRONT;  //默认文字色
    gSetting.colorSnake      = COLOR_DEFAULT_SNAKE;  //默认蛇的颜色
    gSetting.colorApple      = COLOR_DEFAULT_APPLE;  //默认食物颜色
    gSetting.colorFrame      = COLOR_DEFAULT_FRAME;  //默认框架颜色
    SD_loadSetting();

    // 上电默认demo速度（这个不做保存，没啥意义）
    lastDemoSpeed = SPEED_DEMO_H;

    DISP_setBackColor(gSetting.colorBackGround);
    DISP_setForeColor(gSetting.colorFront);
    DISP_setSnakeColor(gSetting.colorSnake);
    DISP_setAppleColor(gSetting.colorApple);
    DISP_setFrameColor(gSetting.colorFrame);

    DISP_init();
    devSndInit();
    KEY_init(eventKey1, eventKey2, eventKey3, eventKey4, eventKey5, eventKey6, eventKey7, eventKey8, eventKey9, powerKey);
    SNAKE_init(eventSnake);
    
    #if ISDEBUG
        LOG("========== MAP DATA (before load Wallmap) START ==========\r\n");
        for (y = 0; y < SNAKE_SIZE_Y; ++y)
        {
            for (x = 0; x < SNAKE_SIZE_X; ++x)
            {   // x= 0,y= 1,idx= 2,offset= 0
                sprintf(buff, 
                    "x=%b2d,y=%b2d,idx=%b2d,offset=%b2d,mapSts=%b2d,isWall=%b2d\r\n", 
                    x, y,
                    (SNAKE_SIZE_X/8)*y+(x/8), 
                    x%8,
                    SNAKE_getMapData(x, y),
                    ((WALLMAP_1[(SNAKE_SIZE_X/8)*y+(x/8)]<<(x%8)) & 0x80));
                LOG(buff);
            }
        }
        LOG("========== MAP DATA (before load Wallmap) END ==========\r\n");
    #endif

    LOG("---- SNAKE_loadWall start\r\n");
    // SNAKE_loadWall(WALLMAP_1);
    LOG("---- SNAKE_loadWall end\r\n");

    #if ISDEBUG
        LOG("========== MAP DATA (After load Wallmap) START ==========\r\n");
        for (y = 0; y < SNAKE_SIZE_Y; ++y)
        {
            for (x = 0; x < SNAKE_SIZE_X; ++x)
            {
                sprintf(buff, 
                    "x=%b2d,y=%b2d,sts=%b2d\r\n", 
                    x, y, SNAKE_getMapData(x, y));
                //sprintf(buff, "x=%b2d,y=%b2d,idx=%b2d,offset=%b2d\r\n", x, y, 5, x%8);
                LOG(buff);
                // if(map[x][y] == STS_EMPTY && ((wallMap[(SNAKE_SIZE_X/8)*y+(x/8)]<<(x%8)) & 0x80) ) {
                //     updateMap(x, y, STS_WALL);
                // }
            }
        }
        LOG("========== MAP DATA (After load Wallmap) END ==========\r\n");
	#endif

    goPageWelcome(DISP_YES);

    LOG("-- CTL_init end\r\n");
}
