#include "common.h"
#include "saveData.h"
#include "embSnakeDevice.h"

// 临时变量用(sprintf等)
extern u8 buff[128];

SaveData_Struct gSetting = {0};

// 读取配置
void SD_loadSetting(){

    LOG("LOAD DATA Start\r\n");

    devLoadSetting(&gSetting);

    LOG("LOAD DATA done.\r\n");
}

// 保存设置(调用之前在外部设置好gSetting的值)
void SD_saveSetting(){
    #if ISDEBUG
    u16 _hiScore=gSetting.hiScore;
    u8 _soundOnOff=gSetting.soundOnOff;
    u16 _colorBackGround=gSetting.colorBackGround;
    u16 _colorFront=gSetting.colorFront;
    u16 _colorSnake=gSetting.colorSnake;
    u16 _colorApple=gSetting.colorApple;
    u16 _colorFrame=gSetting.colorFrame;
    LOG("SAVE DATA Start\r\n");
    sprintf(buff, "write SaveDataHScore:%d\r\n", gSetting.hiScore);LOG(buff);
    sprintf(buff, "write SaveDataSound:%bd\r\n", gSetting.soundOnOff);LOG(buff);
    sprintf(buff, "write SaveDataColorBg:0x%x\r\n", gSetting.colorBackGround);LOG(buff);
    sprintf(buff, "write SaveDataColorFo:0x%x\r\n", gSetting.colorFront);LOG(buff);
    sprintf(buff, "write SaveDataColorSnake:0x%x\r\n", gSetting.colorSnake);LOG(buff);
    sprintf(buff, "write SaveDataColorApple:0x%x\r\n", gSetting.colorApple);LOG(buff);
    sprintf(buff, "write SaveDataColorFrame:0x%x\r\n", gSetting.colorFrame);LOG(buff);
    #endif

    devSaveSetting(&gSetting);

    #if ISDEBUG
    SaveData_Struct tmp = {0,0,0,0,0,0,0};
    SD_loadSetting(&tmp);
    _hiScore==gSetting.hiScore          ?LOG("OK:"):LOG("NG:");sprintf(buff, "read SaveDataHScore:%d\r\n", gSetting.hiScore);LOG(buff);
    _soundOnOff==gSetting.soundOnOff    ?LOG("OK:"):LOG("NG:");sprintf(buff, "read SaveDataSound:%bd\r\n", gSetting.soundOnOff);LOG(buff);
    _colorBackGround==gSetting.colorBackGround?LOG("OK:"):LOG("NG:");sprintf(buff, "read SaveDataColorBg:0x%x\r\n", gSetting.colorBackGround);LOG(buff);
    _colorFront==gSetting.colorFront    ?LOG("OK:"):LOG("NG:");sprintf(buff, "read SaveDataColorFo:0x%x\r\n", gSetting.colorFront);LOG(buff);
    _colorSnake==gSetting.colorSnake  ?LOG("OK:"):LOG("NG:");sprintf(buff, "read SaveDataColorSnake:0x%x\r\n", gSetting.colorSnake);LOG(buff);
    _colorApple==gSetting.colorApple  ?LOG("OK:"):LOG("NG:");sprintf(buff, "read SaveDataColorApple:0x%x\r\n", gSetting.colorApple);LOG(buff);
    _colorFrame==gSetting.colorFrame  ?LOG("OK:"):LOG("NG:");sprintf(buff, "read SaveDataColorFrame:0x%x\r\n", gSetting.colorFrame);LOG(buff);
    LOG("SAVE DATA done.\r\n");
    #endif
}
