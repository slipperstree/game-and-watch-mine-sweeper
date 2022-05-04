#include "common.h"
#include "saveData.h"
#include "embMineDevice.h"

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

    devSaveSetting(&gSetting);

}
