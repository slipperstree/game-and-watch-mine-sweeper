/**************************************************
 * File  : control.c
 * Author: MANDA (slipperstree@gmail.com)
 * Date  : 2019.11
 * Updt  : 2021.07
 * Func  : ��Ϸ��������ͳ�ܸ�ģ��
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

// ����Ǩ��״̬��
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

// ����ȫ������flg���ڶ�ʱ���е���
u16 ttWalk = 0,ttFlag = 0;
u16 ttFlagQuick = 0;
u16 ttGameTime = 0;

// ��Ϸ�Ѷ�
#define GAME_LVL_1   10
#define GAME_LVL_2   16
#define GAME_LVL_3   22
u8 gameLevel = 0;

// ��Ϸʱ��
u8 isPlaying = FALSE;
u8 isWin = FALSE;
u16 gameSec = 0;

// ��ǰ���λ��
extern u8 gDevCurPosX, gDevCurPosY;

// �ϴι��λ�ã�����ɾ����λ���ϵĹ�꣩
u8 lastCurX, lastCurY;

// ���������
u8 lockCurFlg;

// Ϊ����ͨ�����ڲ�������ǰ���岿�֣��ⲿ����Ҫ���ã�������ͷ�ļ��У�
void goPageWelcome(u8 isStartup);
void eventGame();
void switchSound();

// ��ʱ������(sprintf��)
extern u8 buff[128];

// =====================================
// ���ﴦ���ѭ������Ҫ��main������ѭ������
// =====================================
void CTL_run(){
	u16 flashSpeed=0;
    u16 flashSpeedQuick=100;

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
            // ������Ϸʱ��
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

            // ��ӭ���泬��һ��ʱ�䣨��˸���κ󣩣������Զ���ʾDemoģʽ
            if (welcomeWaitTime >= 10)
            {
                devEnterDemoPage();

                nowMode = MODE_WELCOME_DEMO;

                // ����Demo����
                DISP_drawDemo(gSetting.soundOnOff);
                welcomeWaitTime = 0;

                // һ��Ҫ�Ȼ���Demo����!! ��֤��ܵ�λ�ñ���ȷ����
                MINE_restart();

                // restart֮�������Ϸ����״̬������ȫ��ͼ
                DISP_drawAllMap();
            } else {
                // ���涯̬Ч��
                if (flashFlag) {
                    DISP_flashWelcome(flashOnOff);
                    //����Demoģʽ��ע�͵�������伴��
                    //welcomeWaitTime++;
                }
            }
            break;

        case MODE_WELCOME_DEMO:

            // DO NOTHING
            
            break;

        case MODE_GAME:
            // ��û����������ʱ�򣬲�ͣ��ȡ��ǰ���λ�ò�����
            if(!lockCurFlg){
                devUpdateCurPos();
                if(gDevCurPosX != lastCurX || gDevCurPosY != lastCurY){
                    DISP_hideCusor(lastCurX, lastCurY);
                    DISP_showCusor(gDevCurPosX, gDevCurPosY);

                    lastCurX = gDevCurPosX;
                    lastCurY = gDevCurPosY;
                }
            }

            // ��˸���
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
            // TODO:�����ж��Ƿ�ʧ�������ʧ������˸���׵ĵط��������ͨ����������Ͻ���ʾһ��Ц��
            if (flashFlag == 1) {
                DISP_flashBeforeGameOver(flashOnOff, isWin);
            }
            break;

        case MODE_GAMEOVER:
            // Gameover���涯̬Ч��
            if (flashFlag == 1) {
                if (gameLevel == GAME_LVL_1)
                {
                    DISP_flashGameOver(flashOnOff, (isWin && lastGameTime < oldSaveDataBestTime) ? TRUE : FALSE , "��");
                } else if (gameLevel == GAME_LVL_2) {
                    DISP_flashGameOver(flashOnOff, (isWin && lastGameTime < oldSaveDataBestTime) ? TRUE : FALSE , "��ͨ");
                } else if (gameLevel == GAME_LVL_3) {
                    DISP_flashGameOver(flashOnOff, (isWin && lastGameTime < oldSaveDataBestTime) ? TRUE : FALSE , "����");
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

            // ��ʼ�����λ��
            lastCurX = MINE_SIZE_X / 2;
            lastCurY = MINE_SIZE_Y / 2;

            // ��ʼ����������ǣ�����״̬��
            lockCurFlg = 0;

            // �����ж����ĸ�����
            switch (btnNo)
            {
            case KEY_A:
                // HOME-��
                gameLevel = GAME_LVL_1;
                break;
            case KEY_B:
                // HOME-��ͨ
                gameLevel = GAME_LVL_2;
                break;
            case KEY_TIME:
                // HOME-����
                gameLevel = GAME_LVL_3;
                break;
            default:
                break;
            }

            // ����� �ӱ��⻭�� ������Ϸ����
            nowMode = MODE_GAME;

            // ����
            devScreenOFF();

            // ������Ϸ����
            DISP_drawGame(gSetting.soundOnOff, gameLevel);

            // MINE_restart֮ǰһ��Ҫ�Ȼ�����Ϸ����!! ��֤��ܵ�λ�ñ���ȷ����
            MINE_setMineNUM(gameLevel);
            MINE_restart();

            // ��Ϸʱ������
            gameSec = 0;

            // ��Ϸ״̬�ָ�
            isWin = FALSE;

            // restart֮�������Ϸ����״̬������ȫ��ͼ
            DISP_drawAllMap();

            // ����
            devScreenON();
            break;
        default:
            break;
        }
        break;
    // Demoҳ -----------------------------------
    case MODE_WELCOME_DEMO:
        // DO NOTHING
        break;
    // ��Ϸҳ -----------------------------------
    case MODE_GAME:
        switch (event_id)
        {
        // ��������
        case KEY_EVENT_DOWN:
            // LOG("KEY_EVENT_DOWN in MODE_GAME\r\n");
            // sprintf(buff, "btn=%b2d nowModeA=%b2d\r\n", btnNo, nowMode);LOG(buff);

            switch (btnNo)
            {
            case KEY_LEFT:
                // Game-����1=�������
                lockCurFlg = 1;
                DISP_drawLock();
                break;
            case KEY_B:
                // Game-����2=����
                devPlaySound(SOUND_FLAG);
                MINE_switchFlag(gDevCurPosX, gDevCurPosY);
                break;
            case KEY_A:
                // Game-����3=���
                devPlaySound(SOUND_CLICK);
                MINE_click(gDevCurPosX, gDevCurPosY);
                break;
            default:
                break;
            }
            break;
        // ����̧��
        case KEY_EVENT_UP:
            switch (btnNo)
            {
            case KEY_LEFT:
                // Game-����1=�ͷŹ��
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
    // ��Ϸ����ʱ�Ĺ���״̬���������������Ϸ��������
    case MODE_BEFROE_GAMEOVER:
        switch (event_id)
        {
        // ��������
        case KEY_EVENT_DOWN:

            devEnterGameOverPage();

            lastGameTime = gameSec;
            if (gameLevel == GAME_LVL_1)
            {
                oldSaveDataBestTime = gSetting.hiScoreLvl1;
                DISP_drawGameOver(isWin, "��", lastGameTime, oldSaveDataBestTime);
            } else if (gameLevel == GAME_LVL_2) {
                oldSaveDataBestTime = gSetting.hiScoreLvl2;
                DISP_drawGameOver(isWin, "��ͨ", lastGameTime, oldSaveDataBestTime);
            } else if (gameLevel == GAME_LVL_3) {
                oldSaveDataBestTime = gSetting.hiScoreLvl3;
                DISP_drawGameOver(isWin, "����", lastGameTime, oldSaveDataBestTime);
            }

            //����÷ֳ�������߷��������߷ֲ�����
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
            
            //Ǩ����gameover����
            nowMode = MODE_GAMEOVER;
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

void eventGame(){
    switch (MineEventId)
    {
    case MINE_EVENT_UPDATE_BLOCK:
        // ֻ����Ϸ����DEMOҳ��Ӧ����¼����������Ϸ״̬��̬����ǽ��ʱˢ�»��棩
        if (nowMode != MODE_GAME && nowMode != MODE_WELCOME_DEMO)
        {
            // ����Ϸ/Demoҳ�治���»���
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
            // ����1Ϊ�Ƿ�ʤ�� ��ʱû�� ��������ʾʤ��ʧ��ʲô��
            isWin = MineEventParam1;
            if(isWin == FALSE){
                // ��Ϸʧ�ܣ��ȵ����ˣ���LED����һ��
                LED_ALL_ON();
                delay_ms(30);
                LED_ALL_OFF();
            }
            // ��ת����Ϸ����ǰ״̬�����治Ǩ�ƣ����ڹ۲���Ϸ�����
            nowMode = MODE_BEFROE_GAMEOVER;
            // ֹͣ��ʱ
            isPlaying = FALSE;
        } else if (nowMode == MODE_WELCOME_DEMO) {
            // DO NOTHING
        }
        
        break;
    case MINE_EVENT_UPDATE_FLAG_CNT:
        if (nowMode == MODE_GAME)
        {
            // ˢ��ʣ����������
            DISP_updateFlagCnt(MineEventParam1);
        }
        break;
    case MINE_EVENT_TIMESTART:
        // ��ʼ��ʱ
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

    // ��ʾ���⻭��
    LOG("---- ��ʾ���⻭�� start\r\n");
    DISP_drawWelcome(isStartup);
    LOG("---- ��ʾ���⻭�� end\r\n");

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
    
    // ÿ���л������������棬������˵��д10��Σ�д�������
    // TODO: �����������������ҳ����ͳһ����
    SD_saveSetting();
}

void CTL_init() {

    u8 x, y, sdCheck;
    
    // ��ȡ���ã���ֹ�״ζ�ȡʧ�������ú�Ĭ��ֵ���豸���α���
    gSetting.hiScoreLvl1     = 5999;                 //��߷�=99:59(��Ϊɨ���õ���ʱ������Ĭ�ϼ�¼����һ���ܴ����) 5999=99��59��
    gSetting.hiScoreLvl2     = 5999;                 //��߷�=99:59(��Ϊɨ���õ���ʱ������Ĭ�ϼ�¼����һ���ܴ����)
    gSetting.hiScoreLvl3     = 5999;                 //��߷�=99:59(��Ϊɨ���õ���ʱ������Ĭ�ϼ�¼����һ���ܴ����)
    gSetting.soundOnOff      = 1;                    //��������
    gSetting.colorBackGround = COLOR_DEFAULT_BACK;   //Ĭ�ϱ���ɫ
    gSetting.colorFront      = COLOR_DEFAULT_FRONT;  //Ĭ������ɫ
    gSetting.colorFrame      = COLOR_DEFAULT_FRAME;  //Ĭ�Ͽ����ɫ
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
