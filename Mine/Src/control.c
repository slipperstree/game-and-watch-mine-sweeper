/**************************************************
 * File  : control.c
 * Author: MANDA (slipperstree@gmail.com)
 * Date  : 2019.11
 * Updt  : 2021.07
 * Func  : ̰ʳ����Ϸ��������ͳ��snake��display��ģ��
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

// ����Ǩ��״̬��
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

// ����ȫ������flg���ڶ�ʱ���е���
u16 ttWalk = 0,ttFlag = 0;

// Ϊ����ͨ�����ڲ�������ǰ���岿�֣��ⲿ����Ҫ���ã�������ͷ�ļ��У�
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

// ��ʱ������(sprintf��)
extern u8 buff[128];

// =====================================
// ���ﴦ���ѭ������Ҫ��main������ѭ������
// =====================================
void CTL_run(){
	u16 flashSpeed=0;

    KEY_keyscan();

    
    // TODO:G&W? �������ڶ�ʱ���ж��������ģ�GBA��ʱ���ж�����
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

            // ��ӭ���泬��һ��ʱ�䣨��˸���κ󣩣������Զ���ʾDemoģʽ
            if (welcomeWaitTime >= 10)
            {
                devEnterDemoPage();

                nowMode = MODE_WELCOME_DEMO;

                // ����Demo����
                DISP_drawDemo(gSetting.soundOnOff);
                welcomeWaitTime = 0;

                // һ��Ҫ�Ȼ���Demo����!! ��֤��ܵ�λ�ñ���ȷ����
                // һ��restartSNAKE�ͻ᳢�Ի�������ļ���block����ʼ�ߣ�ʳ�
                // �������������������ͼ���������ݰ�����ʳ����ϰ���,Ȼ���ص�һ��RESTART���¼��������Ҫ�ϰ������¼���Ӧ�������ϰ���
                // �ص�����ִ����Ϻ�����ù̶��Ŀ�ʼλ�ã����ϽǺ���3��block��ͷ���ң�����û���ϰ���Ŀ�λ�������һ��ʳ������������ϰ��ﲻ��ռ������3��block��
                SNAKE_restart();
            } else {
                // ���涯̬Ч��
                if (flashFlag) {
                    DISP_flashWelcome(flashOnOff);
                    welcomeWaitTime++;
                }
            }
            break;

        case MODE_WELCOME_DEMO:

            #if ISDEBUG
                // ��N��ƻ����ǿ�ƽ���demo
                if (SNAKE_getNowScroe() > DEBUG_DEMO_APPLE){
                    // ģ��gameover
                    LOG("ģ��gameover...\r\n");
                    SnakeEventId = SNAKE_EVENT_GAMEOVER;
                    SnakeEventParam1 = SNAKE_getNowScroe();
                    SnakeEventParam2 = 0;
                    eventSnake();
                    LOG("ģ��gameover done.\r\n");
                    return;
                }
            #endif

            // AIǰ��һ��
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
            // �Զ�ǰ��
            if (ttWalk >= nowSpeed)
            {
                ttWalk = 0;
                SNAKE_moveNext();
            }
            break;
        case MODE_GAMEOVER:
            // Gameover���涯̬Ч��
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

    // �ָ���־,�����ڵ���1���ڼ䷴��ˢ��
    flashFlag = 0;
}

// =====================================
// ������ͨ����
// =====================================
void doBtnCommon(u8 btnNo, u8 event_id){

    switch (nowMode)
    {
    // ����ҳ -----------------------------------
    case MODE_WELCOME:
        switch (event_id)
        {
        // ��������
        case KEY_EVENT_DOWN:
            // LOG("KEY_EVENT_DOWN in MODE_WELCOME\r\n");
            // sprintf(buff, "btn=%b2d nowModeA=%b2d\r\n", btnNo, nowMode);LOG(buff);

            devEnterGamePage();

            // ����� �ӱ��⻭�� ������Ϸ����
            nowMode = MODE_GAME;
            // ������Ϸ����
            DISP_drawGame(gSetting.soundOnOff);

            // SNAKE_restart֮ǰһ��Ҫ�Ȼ�����Ϸ����!! ��֤��ܵ�λ�ñ���ȷ����
            SNAKE_restart();
            break;
        default:
            break;
        }
        break;
    // Demoҳ -----------------------------------
    case MODE_WELCOME_DEMO:
        switch (event_id)
        {
        // ��������
        case KEY_EVENT_DOWN:
            // LOG("KEY_EVENT_DOWN in MODE_WELCOME_DEMO\r\n");
            // sprintf(buff, "btn=%b2d nowModeA=%b2d\r\n", btnNo, nowMode);LOG(buff);

            // �����ж����ĸ�����
            switch (btnNo)
            {
            case KEY_B:
                // Demo-����B=�л��ٶ�
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
                // ������Ϸ����
                nowMode = MODE_GAME;
                // ������Ϸ����
                DISP_drawGame(gSetting.soundOnOff);
                // SNAKE_restart֮ǰһ��Ҫ�Ȼ�����Ϸ����!! ��֤��ܵ�λ�ñ���ȷ����
                SNAKE_restart();
                break;
            // case KEY_PAUSE:
            //     // Demo-����SELECT=�л�����
            //     switchSound();
            //    break;
            default:
                // Demo-��������=���ر��⻭��
                goPageWelcome(DISP_NO);
                break;
            }
            break;
        default:
            break;
        }
        break;
    // ��Ϸҳ -----------------------------------
    case MODE_GAME:
        switch (event_id)
        {
        // ��������
        case KEY_EVENT_DOWN:
            // LOG("KEY_EVENT_DOWN in MODE_GAME\r\n");
            // sprintf(buff, "btn=%b2d nowModeA=%b2d\r\n", btnNo, nowMode);LOG(buff);

            // �ֶ��ƶ��ɹ��Ļ����Զ�ǰ����ʱ����
            if ((btnNo == KEY_UP && SNAKE_moveUp()) || 
                (btnNo == KEY_DOWN && SNAKE_moveDown()) ||
                (btnNo == KEY_LEFT && SNAKE_moveLeft()) ||
                (btnNo == KEY_RIGHT && SNAKE_moveRight())
                ) {
                // �ֶ��ƶ��ɹ��Ļ����Զ�ǰ����ʱ����
                ttWalk = 0;
            }

            // if (btnNo == KEY_PAUSE) {
            //     // �л�����
            //     switchSound();
            // }

            if (btnNo == KEY_TIME) {
                // ����Demo
                devEnterDemoPage();
                nowMode = MODE_WELCOME_DEMO;
                // ����Demo����
                DISP_drawDemo(gSetting.soundOnOff);
                SNAKE_restart();
            }
            break;
        // ����������ס���ţ�������
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
    // GameOverҳ -----------------------------------
    case MODE_GAMEOVER:
        switch (event_id)
        {
        // ��������
        case KEY_EVENT_DOWN:
            if (btnNo == KEY_PAUSE) {
                // �������ҳ
                DISP_drawInfo();
                nowMode = MODE_INFO;
            } else {
                // �������ص����⻭��
                goPageWelcome(DISP_NO);
            }
            
            break;
        default:
            break;
        }
        
        break;
    // ����ҳ -----------------------------------
    case MODE_INFO:
        // ��������ص����⻭��
        switch (event_id)
        {
        // ��������
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
// ������������Ӧ�����¼�
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

        // Enable wakup by PIN1, the power button��should disabe it in main() for as an input button��
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
        // ֻ����Ϸ����DEMOҳ��Ӧ����¼����������Ϸ״̬��̬����ǽ��ʱˢ�»��棩
        if (nowMode != MODE_GAME && nowMode != MODE_WELCOME_DEMO)
        {
            // ����Ϸ/Demoҳ�治���»���
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
        // �����Ҫ�õ��ϰ��﹦�ܣ�����������¼��������á��Ҳ���ʹ�����Ͻ�1X3��λ�á���Ԥ������ʼ�������ã�
        // ����˳������
        // 1-Snakeģ���restart���������Ƚ�mapȫ�����,�����ϰ��
        // 2-Snakeģ��ص����¼���
        // 3-�ڴ��¼��и�����Ҫ����ϰ����������ʵ�֣�
        // 4-Snakeģ�������Ͻ�����1X3��С���ߵ����塣�յ�������ɵ�һ��ʳ���Ϸ��ʼ��
        // ���ԣ����˻���1��������������п��ܸ���4��������ɵ�ʳ��λ�ó�ͻ���㲢��֪��ʳ�����������Ҫ���������ʳ��֮ǰ��Map���֮������ϰ��
        if (nowMode == MODE_WELCOME_DEMO){
            // ����Demo������ϰ���
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

            //��ͣһ��������״̬(TODO:GBA �����Ӱ������ʱ���maxmod�������ţ���)
            //TODO:���ﲻҪ����ʱ���ĳɶ��ĵȴ�֡���ķ�ʽ����֡������ָ������֮��ִ�лص�������
            My_delay_ms(2000);

            devEnterGameOverPage();

            lastGameScore = SnakeEventParam1;
            oldSaveDataHScore = gSetting.hiScore;
            DISP_drawGameOver(lastGameScore, oldSaveDataHScore);

            //����÷ֳ�������߷��������߷ֲ�����
            if (lastGameScore > gSetting.hiScore)
            {
                gSetting.hiScore = lastGameScore;
                SD_saveSetting();
            }

            nowMode = MODE_GAMEOVER;
        } else if (nowMode == MODE_WELCOME_DEMO) {
            lastDemoScore = SnakeEventParam1;

            // ƽ���÷� TODO: ���������⣬ÿ��ֻ����һ����ƽ�����ǲ��Ե�
            totalDemoCnt++;
            totalDemoScore += SnakeEventParam1;
            avgDemoScore = totalDemoScore / totalDemoCnt;
            
            // ��ߵ÷�
            if (SnakeEventParam1 > maxDemoScore)
            {
                maxDemoScore = SnakeEventParam1;
            }

            // ��ʾ��߷֣�ƽ���ֵ���Ϣ
            DISP_updateDemoGameover(maxDemoScore, avgDemoScore, lastDemoScore);

            #if !(ISDEBUG && DEBUG_DEMO_GAMEOVER_NOWAIT)
                // ͣһ��
                My_delay_ms(3000);
            #endif
            
            // �ص���ӭҳ
            goPageWelcome(DISP_NO);
        }
        
        break;
    case SNAKE_EVENT_EAT_APPLE:

        if (gSetting.soundOnOff)
        {
            // ��������
            devPlaySound(SOUND_EAT_APPLE);
        }
        
        if (nowMode == MODE_WELCOME_DEMO)
        {
            // ˢ�·���
            DISP_updateDemoScore(maxDemoScore, SnakeEventParam1);
        } else {
            // ˢ�·���
            DISP_updateGameScore(maxDemoScore, SnakeEventParam1);

            // �ֶ���Ϸ�Ÿı��ٶ�
            nowSpeed -= SPEED_INTERVAL;
            if (nowSpeed <= SPEED_MAX) 
            {
                // �����ٶ�����
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

    // ��ʾ���⻭��
    LOG("---- ��ʾ���⻭�� start\r\n");
    DISP_drawWelcome(isStartup);
    LOG("---- ��ʾ���⻭�� end\r\n");

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
    
    // ÿ���л������������棬������˵��д10��Σ�д�������
    // TODO: �����������������ҳ����ͳһ����
    SD_saveSetting();
}

void resetDemoWall(){
    // �����ǰ�ϰ���
    SNAKE_clearWall();

    //����ʾģʽ��������������Ϊ�ϰ���
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
    
    // ��ȡ���ã���ֹ�״ζ�ȡʧ�������ú�Ĭ��ֵ���豸���α���
    gSetting.hiScore         = 0;                    //��߷�=0
    gSetting.soundOnOff      = 1;                    //��������
    gSetting.colorBackGround = COLOR_DEFAULT_BACK;   //Ĭ�ϱ���ɫ
    gSetting.colorFront      = COLOR_DEFAULT_FRONT;  //Ĭ������ɫ
    gSetting.colorSnake      = COLOR_DEFAULT_SNAKE;  //Ĭ���ߵ���ɫ
    gSetting.colorApple      = COLOR_DEFAULT_APPLE;  //Ĭ��ʳ����ɫ
    gSetting.colorFrame      = COLOR_DEFAULT_FRAME;  //Ĭ�Ͽ����ɫ
    SD_loadSetting();

    // �ϵ�Ĭ��demo�ٶȣ�����������棬ûɶ���壩
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
