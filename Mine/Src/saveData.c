#include "common.h"
#include "saveData.h"
#include "embMineDevice.h"

// ��ʱ������(sprintf��)
extern u8 buff[128];

SaveData_Struct gSetting = {0};

// ��ȡ����
void SD_loadSetting(){

    LOG("LOAD DATA Start\r\n");

    devLoadSetting(&gSetting);

    LOG("LOAD DATA done.\r\n");
}

// ��������(����֮ǰ���ⲿ���ú�gSetting��ֵ)
void SD_saveSetting(){

    devSaveSetting(&gSetting);

}
