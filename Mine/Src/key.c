/****************************************************************
 * ����ɨ����
 * File  : key.c
 * Author: chenling
 * Date  : 2021.7
 * Func  : ����ɨ���ܣ�֧���¼�������/˫��/����/�ſ�/����/����
*****************************************************************/

// ******* ������Ҫ�޸�ͷ�ļ����壬key.h �� common.h �Ǳ����
// ******* һ����Ҫ��MCU�Ĵ���ͷ�ļ�������common.h�����Ա㶨�����İ���IO��
#include "key.h"
#include "embSnakeDevice.h"
#include "common.h"

// *******************************************************************************************************
// ******* ����Ĵ��벻��Ҫ�޸� *****************************************************************************
// *******************************************************************************************************

// �����ص�����
pBtnEventFunc btn1EventFunc;
pBtnEventFunc btn2EventFunc;
pBtnEventFunc btn3EventFunc;
pBtnEventFunc btn4EventFunc;
pBtnEventFunc btn5EventFunc;
pBtnEventFunc btn6EventFunc;
pBtnEventFunc btn7EventFunc;
pBtnEventFunc btn8EventFunc;
pBtnEventFunc btn9EventFunc;
pBtnEventFunc btn10EventFunc;

// ����ɨ��Ĺ�ͨ�������� ����Ҫ�޸ģ�
void keyScanCommon(u8 btnUpDown, pBtnEventFunc callBackFunc, u8 btnIdx);

// ��Ҫ�ڴ�ѭ���е��øú���
void KEY_keyscan(){
    u8 idx = 0;

    u32 buttons = buttons_get();

    // ����ɨ�����а����������ݻص��õĺ���ָ��(������֧������������)
    #if KEY_CNT>0
        keyScanCommon(BTN1, btn1EventFunc, idx++);
    #endif
    #if KEY_CNT>1
        keyScanCommon(BTN2, btn2EventFunc, idx++);
    #endif
    #if KEY_CNT>2
        keyScanCommon(BTN3, btn3EventFunc, idx++);
    #endif
    #if KEY_CNT>3
        keyScanCommon(BTN4, btn4EventFunc, idx++);
    #endif
    #if KEY_CNT>4
        keyScanCommon(BTN5, btn5EventFunc, idx++);
    #endif
    #if KEY_CNT>5
        keyScanCommon(BTN6, btn6EventFunc, idx++);
    #endif
    #if KEY_CNT>6
        keyScanCommon(BTN7, btn7EventFunc, idx++);
    #endif
    #if KEY_CNT>7
        keyScanCommon(BTN8, btn8EventFunc, idx++);
    #endif
    #if KEY_CNT>8
        keyScanCommon(BTN9, btn9EventFunc, idx++);
    #endif
    #if KEY_CNT>9
        keyScanCommon(BTN10, btn10EventFunc, idx++);
    #endif

}

#define STS_WAIT_CLICK_START        1
#define STS_WAIT_CLICK_END          2
#define STS_WAIT_KEEP_PRESS_END     3
#define STS_WAIT_DBCLICK_START      4
#define STS_WAIT_DBCLICK_END        5

u8 btnStatus[KEY_CNT];
u16 ttWaitKeyClickEnd[KEY_CNT];         //������ʼ�󾭹���ʱ��
u16 ttWaitKeyDBClickStart[KEY_CNT];     //������ɺ󾭹���ʱ�䣬�����TICK_DCLICKʱ�����ּ�⵽���������򵥻�ʧЧ
u16 ttWaitKeyDBClickEnd[KEY_CNT];       //˫����ʼ�󾭹���ʱ��

// ɨ��֮ǰ��Ҫ����һ�γ�ʼ��
void KEY_init(  pBtnEventFunc btn1Func,
                pBtnEventFunc btn2Func,
                pBtnEventFunc btn3Func,
                pBtnEventFunc btn4Func,
                pBtnEventFunc btn5Func,
                pBtnEventFunc btn6Func,
                pBtnEventFunc btn7Func,
                pBtnEventFunc btn8Func,
                pBtnEventFunc btn9Func,
                pBtnEventFunc btn10Func  ) {
    u8 i = 0;
    for (i = 0; i < KEY_CNT; i++)
    {
        btnStatus[i] = STS_WAIT_CLICK_START;
        ttWaitKeyClickEnd[i] = 0;
        ttWaitKeyDBClickStart[i] = 0;
        ttWaitKeyDBClickEnd[i] = 0;
    }

    btn1EventFunc = btn1Func; btn2EventFunc = btn2Func; btn3EventFunc = btn3Func; btn4EventFunc = btn4Func;
    btn5EventFunc = btn5Func; btn6EventFunc = btn6Func; btn7EventFunc = btn7Func; btn8EventFunc = btn8Func;
    btn9EventFunc = btn9Func; btn10EventFunc = btn10Func;
    
}

// ��ͨ�İ���ɨ���߼�
// btnUpDown : �����ļ�ֵ����ƽ��
// fnClick_or_fnKeyDwon : �����¼��ص��������ж����������¼���ʱ�����ָ��������
// fnDBClick_or_fnKeyUp : ˫���¼��ص��������ж�����˫���¼���ʱ�����ָ��������
// fnKeepPressStart : ��ס���ſ�ʼ�¼��ص��������ж�������ס���ſ�ʼ�¼���ʱ�����ָ��������
// fnKeepPressKeepping : ��ס���Ų�����ס���������¼��ص��������ᷴ������ֱ�����֣�
// btnIdx : ������ţ���0��ʼ����ͬ�İ������ظ����ɣ�Ŀ����ÿ������ʹ�õ�״̬���������Ը��ã�ͨ�������±���Էֿ�
// ����ҵ�����Ҫ���ò��ϵ��¼�ֱ�Ӵ��� 0 ����
void keyScanCommon(u8 btnUpDown, pBtnEventFunc callBackFunc, u8 btnIdx){
    
    // �������� �� ֮ǰΪ�ɿ�״̬���������ոձ����¡�
    if (btnUpDown == KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_CLICK_START){
        //TODO:G&W? delay_ms(XIAO_DOU);
        if (btnUpDown == KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_CLICK_START){
            #ifdef USE_KEY_DOWN_UP
                // ��Ӧ�����¼�
                btnStatus[btnIdx] = STS_WAIT_CLICK_END;
                if (callBackFunc != 0) callBackFunc(KEY_EVENT_DOWN);
            #else
                // �л����ȴ��������״̬
                ttWaitKeyClickEnd[btnIdx] = 0;
                btnStatus[btnIdx] = STS_WAIT_CLICK_END;
            #endif
        }
    }

    // �������� �� Ϊ�ȴ��������״̬�����������º󲢱����С�
    else if (btnUpDown == KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_CLICK_END){
        // ����
        if (ttWaitKeyClickEnd[btnIdx] < 0xffffL) {ttWaitKeyClickEnd[btnIdx]++;}

        // �ж��Ƿ�ﵽ��ס�����¼���Ҫ��ʱ��
        if (ttWaitKeyClickEnd[btnIdx] >= TICK_KEEPDOWN) {
            // �л����ȴ���ס�������״̬����������ס���ſ�ʼ�¼������¼�ֻ����һ�Σ�������ס����ʱ�����ظ���������һ���¼����Ǳ��¼���
            ttWaitKeyClickEnd[btnIdx] = 0;
            btnStatus[btnIdx] = STS_WAIT_KEEP_PRESS_END;
            if (callBackFunc != 0) callBackFunc(KEY_EVENT_LONG_PRESS);
        }
    }

    // �������� �� Ϊ�ȴ���ס�������״̬�������������������У���Ӧ�����¼���
    else if (btnUpDown == KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_KEEP_PRESS_END){
        if (btnUpDown == KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_KEEP_PRESS_END){
            // ����
            if (ttWaitKeyClickEnd[btnIdx] < 0xffff) {ttWaitKeyClickEnd[btnIdx]++;}

            // �ж��Ƿ�ﵽ��ס���������¼���Ҫ��ʱ��
            if (ttWaitKeyClickEnd[btnIdx] >= TICK_KEEPDOWN_NEXT) {
                // ���������¼�
                ttWaitKeyClickEnd[btnIdx] = 0;
                if (callBackFunc != 0) callBackFunc(KEY_EVENT_KEEPING_PRESS);
            }
        }
    }

    // �����ſ� �� Ϊ�ȴ���ס�������״̬������ʱ�䰴ס������ո��ɿ���
    else if (btnUpDown != KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_KEEP_PRESS_END){
        //TODO:G&W? delay_ms(XIAO_DOU);
        if (btnUpDown != KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_KEEP_PRESS_END){
            // ʲôҲ�������ص���ʼ״̬����Ȼ������Ҳ���Ը�һ������ʱ�䰴�º�ſ����¼������߽С���ʱ�䵥���¼���,Ŀǰû�б�Ҫ��
            btnStatus[btnIdx] = STS_WAIT_CLICK_START;
        }
    }

    // �����ſ� �� Ϊ�ȴ��������״̬���������ո��ɿ���
    else if (btnUpDown != KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_CLICK_END){
        //TODO:G&W? delay_ms(XIAO_DOU);
        if (btnUpDown != KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_CLICK_END){
            
            #ifdef __UART_H_
            //�����ã��ҵ����ʵļ��
            //UART_SendByte((ttWaitKeyClickEnd[btnIdx] >> 8) & 0xff);
            //UART_SendByte(ttWaitKeyClickEnd[btnIdx] & 0xff);
            #endif

            #ifndef USE_KEY_DOWN_UP
                // ������²��ɿ���ʱ�����Ѿ������˵���Ҫ������������Ϊ��Ч���������������µȴ�
                if (ttWaitKeyClickEnd[btnIdx] > TICK_WAIT_CLICK_END)
                {
                    btnStatus[btnIdx] = STS_WAIT_CLICK_START;
                }
                // ��֮�������ȴ�˫���¼���ʼ״̬
                // ��һ��ʱ�����ް������µĻ�����Ϊû��˫���������Żᴥ�������¼�
                else {
                    ttWaitKeyDBClickStart[btnIdx] = 0;
                    btnStatus[btnIdx] = STS_WAIT_DBCLICK_START;
                }
            #else
                // ��Ӧ����̧���¼�
                btnStatus[btnIdx] = STS_WAIT_CLICK_START;
                if (callBackFunc != 0) callBackFunc(KEY_EVENT_UP);
            #endif
        }
    }

    #ifndef USE_KEY_DOWN_UP
        // �����ɿ� �� Ϊ�ȴ�˫���¼���ʼ״̬���������������һ��ʱ���ڡ�
        else if (btnUpDown != KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_DBCLICK_START){

            // ����(���255)
            if (ttWaitKeyDBClickStart[btnIdx] < 0xffffL) {ttWaitKeyDBClickStart[btnIdx]++;}

            if (ttWaitKeyDBClickStart[btnIdx] > TICK_WAIT_DBCLICK_START)
            {
                // ��һ��ʱ�����ް������£����ٵȴ�˫����������Ч�����õ����ص�����
                btnStatus[btnIdx] = STS_WAIT_CLICK_START;
                if (callBackFunc != 0) callBackFunc(KEY_EVENT_CLICK);
            }
        }

        #ifndef NOT_USE_DBCLICK
            // �������� �� �ȴ�˫���¼���ʼ״̬����������������һ��ʱ�����ٴΰ��¡�
            else if (btnUpDown == KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_DBCLICK_START){
                //TODO:G&W? delay_ms(XIAO_DOU);
                if (btnUpDown == KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_DBCLICK_START){
                    // ����ȴ�˫�����״̬
                    ttWaitKeyDBClickEnd[btnIdx] = 0;
                    btnStatus[btnIdx] = STS_WAIT_DBCLICK_END;
                }
            }

            // �������� �� Ϊ�ȴ�˫�����״̬����˫���ĵڶ��ΰ������º󲢱����С�
            else if (btnUpDown == KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_DBCLICK_END){
                // ����(���255)
                if (ttWaitKeyDBClickEnd[btnIdx] < 0xffff) {ttWaitKeyDBClickEnd[btnIdx]++;}
            }

            // �����ɿ� �� Ϊ�ȴ�˫�����״̬����˫���ĵڶ��ΰ����ո��ɿ���
            else if (btnUpDown != KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_DBCLICK_END){
                //TODO:G&W? delay_ms(XIAO_DOU);
                if (btnUpDown != KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_DBCLICK_END){
                    
                    // ����ڶ��ΰ������²��ɿ���ʱ�����Ѿ�������һ����Ч����Ҫ������������Ϊ��Ч˫�����������������µȴ�
                    if (ttWaitKeyDBClickEnd[btnIdx] > TICK_WAIT_CLICK_END)
                    {
                        btnStatus[btnIdx] = STS_WAIT_CLICK_START;
                    }
                    // ��֮�����ټ����ڴ������¼�������˫���ص����������б�Ҫ��Ӧ�����������������¼����������ȴ�˫��һ��׷����ر�־λ�͵ȴ��߼����ɣ�
                    else {
                        btnStatus[btnIdx] = STS_WAIT_CLICK_START;
                        if (callBackFunc != 0) callBackFunc(KEY_EVENT_DBCLICK);
                    }
                }
            }
        #endif
    #endif
}
