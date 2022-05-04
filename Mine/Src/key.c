/****************************************************************
 * 按键扫描框架
 * File  : key.c
 * Author: chenling
 * Date  : 2021.7
 * Func  : 按键扫描框架，支持事件：单击/双击/按下/放开/长按/连击
*****************************************************************/

// ******* 根据需要修改头文件定义，key.h 和 common.h 是必须的
// ******* 一般需要将MCU寄存器头文件包含在common.h里面以便定义后面的按键IO宏
#include "key.h"
#include "embSnakeDevice.h"
#include "common.h"

// *******************************************************************************************************
// ******* 后面的代码不需要修改 *****************************************************************************
// *******************************************************************************************************

// 按键回调函数
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

// 按键扫描的共通函数定义 （不要修改）
void keyScanCommon(u8 btnUpDown, pBtnEventFunc callBackFunc, u8 btnIdx);

// 需要在大循环中调用该函数
void KEY_keyscan(){
    u8 idx = 0;

    u32 buttons = buttons_get();

    // 依次扫描所有按键，并传递回调用的函数指针(理论上支持任意多个按键)
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
u16 ttWaitKeyClickEnd[KEY_CNT];         //单击开始后经过的时间
u16 ttWaitKeyDBClickStart[KEY_CNT];     //单击完成后经过的时间，如果在TICK_DCLICK时间内又检测到按键按下则单击失效
u16 ttWaitKeyDBClickEnd[KEY_CNT];       //双击开始后经过的时间

// 扫描之前需要调用一次初始化
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

// 共通的按键扫描逻辑
// btnUpDown : 按键的键值（电平）
// fnClick_or_fnKeyDwon : 单击事件回调函数（判定发生单击事件的时候调用指定函数）
// fnDBClick_or_fnKeyUp : 双击事件回调函数（判定发生双击事件的时候调用指定函数）
// fnKeepPressStart : 按住不放开始事件回调函数（判定发生按住不放开始事件的时候调用指定函数）
// fnKeepPressKeepping : 按住不放并保持住产生连击事件回调函数（会反复触发直到松手）
// btnIdx : 按键编号（从0开始）不同的按键不重复即可，目的是每个按键使用的状态变量不可以复用，通过数组下标各自分开
// 根据业务的需要，用不上的事件直接传递 0 即可
void keyScanCommon(u8 btnUpDown, pBtnEventFunc callBackFunc, u8 btnIdx){
    
    // 按键按下 且 之前为松开状态：【按键刚刚被按下】
    if (btnUpDown == KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_CLICK_START){
        //TODO:G&W? delay_ms(XIAO_DOU);
        if (btnUpDown == KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_CLICK_START){
            #ifdef USE_KEY_DOWN_UP
                // 响应按下事件
                btnStatus[btnIdx] = STS_WAIT_CLICK_END;
                if (callBackFunc != 0) callBackFunc(KEY_EVENT_DOWN);
            #else
                // 切换到等待单击完成状态
                ttWaitKeyClickEnd[btnIdx] = 0;
                btnStatus[btnIdx] = STS_WAIT_CLICK_END;
            #endif
        }
    }

    // 按键按下 且 为等待单击完成状态：【按键按下后并保持中】
    else if (btnUpDown == KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_CLICK_END){
        // 计数
        if (ttWaitKeyClickEnd[btnIdx] < 0xffffL) {ttWaitKeyClickEnd[btnIdx]++;}

        // 判断是否达到按住不放事件的要求时间
        if (ttWaitKeyClickEnd[btnIdx] >= TICK_KEEPDOWN) {
            // 切换到等待按住不放完成状态，并触发按住不放开始事件（该事件只触发一次，连续按住不放时会多次重复触发另外一个事件，非本事件）
            ttWaitKeyClickEnd[btnIdx] = 0;
            btnStatus[btnIdx] = STS_WAIT_KEEP_PRESS_END;
            if (callBackFunc != 0) callBackFunc(KEY_EVENT_LONG_PRESS);
        }
    }

    // 按键按下 且 为等待按住不放完成状态：【按键长按并保持中，响应连击事件】
    else if (btnUpDown == KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_KEEP_PRESS_END){
        if (btnUpDown == KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_KEEP_PRESS_END){
            // 计数
            if (ttWaitKeyClickEnd[btnIdx] < 0xffff) {ttWaitKeyClickEnd[btnIdx]++;}

            // 判断是否达到按住不放连击事件的要求时间
            if (ttWaitKeyClickEnd[btnIdx] >= TICK_KEEPDOWN_NEXT) {
                // 触发连击事件
                ttWaitKeyClickEnd[btnIdx] = 0;
                if (callBackFunc != 0) callBackFunc(KEY_EVENT_KEEPING_PRESS);
            }
        }
    }

    // 按键放开 且 为等待按住不放完成状态：【长时间按住按键后刚刚松开】
    else if (btnUpDown != KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_KEEP_PRESS_END){
        //TODO:G&W? delay_ms(XIAO_DOU);
        if (btnUpDown != KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_KEEP_PRESS_END){
            // 什么也不做，回到初始状态（当然，这里也可以搞一个【长时间按下后放开的事件】或者叫【长时间单击事件】,目前没有必要）
            btnStatus[btnIdx] = STS_WAIT_CLICK_START;
        }
    }

    // 按键放开 且 为等待单击完成状态：【按键刚刚松开】
    else if (btnUpDown != KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_CLICK_END){
        //TODO:G&W? delay_ms(XIAO_DOU);
        if (btnUpDown != KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_CLICK_END){
            
            #ifdef __UART_H_
            //测试用，找到合适的间隔
            //UART_SendByte((ttWaitKeyClickEnd[btnIdx] >> 8) & 0xff);
            //UART_SendByte(ttWaitKeyClickEnd[btnIdx] & 0xff);
            #endif

            #ifndef USE_KEY_DOWN_UP
                // 如果按下并松开的时间间隔已经超过了单击要求的最大间隔，视为无效操作，丢弃，重新等待
                if (ttWaitKeyClickEnd[btnIdx] > TICK_WAIT_CLICK_END)
                {
                    btnStatus[btnIdx] = STS_WAIT_CLICK_START;
                }
                // 反之，则进入等待双击事件开始状态
                // 在一定时间内无按键按下的话就认为没有双击发生，才会触发单击事件
                else {
                    ttWaitKeyDBClickStart[btnIdx] = 0;
                    btnStatus[btnIdx] = STS_WAIT_DBCLICK_START;
                }
            #else
                // 响应按键抬起事件
                btnStatus[btnIdx] = STS_WAIT_CLICK_START;
                if (callBackFunc != 0) callBackFunc(KEY_EVENT_UP);
            #endif
        }
    }

    #ifndef USE_KEY_DOWN_UP
        // 按键松开 且 为等待双击事件开始状态：【单击结束后的一段时间内】
        else if (btnUpDown != KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_DBCLICK_START){

            // 计数(最大255)
            if (ttWaitKeyDBClickStart[btnIdx] < 0xffffL) {ttWaitKeyDBClickStart[btnIdx]++;}

            if (ttWaitKeyDBClickStart[btnIdx] > TICK_WAIT_DBCLICK_START)
            {
                // 在一定时间内无按键按下，不再等待双击，单击生效，调用单击回调函数
                btnStatus[btnIdx] = STS_WAIT_CLICK_START;
                if (callBackFunc != 0) callBackFunc(KEY_EVENT_CLICK);
            }
        }

        #ifndef NOT_USE_DBCLICK
            // 按键按下 且 等待双击事件开始状态：【单击结束后并在一定时间内再次按下】
            else if (btnUpDown == KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_DBCLICK_START){
                //TODO:G&W? delay_ms(XIAO_DOU);
                if (btnUpDown == KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_DBCLICK_START){
                    // 进入等待双击完成状态
                    ttWaitKeyDBClickEnd[btnIdx] = 0;
                    btnStatus[btnIdx] = STS_WAIT_DBCLICK_END;
                }
            }

            // 按键按下 且 为等待双击完成状态：【双击的第二次按键按下后并保持中】
            else if (btnUpDown == KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_DBCLICK_END){
                // 计数(最大255)
                if (ttWaitKeyDBClickEnd[btnIdx] < 0xffff) {ttWaitKeyDBClickEnd[btnIdx]++;}
            }

            // 按键松开 且 为等待双击完成状态：【双击的第二次按键刚刚松开】
            else if (btnUpDown != KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_DBCLICK_END){
                //TODO:G&W? delay_ms(XIAO_DOU);
                if (btnUpDown != KEY_DOWN_VALUE && btnStatus[btnIdx] == STS_WAIT_DBCLICK_END){
                    
                    // 如果第二次按键按下并松开的时间间隔已经超过了一次有效单击要求的最大间隔，视为无效双击操作，丢弃，重新等待
                    if (ttWaitKeyDBClickEnd[btnIdx] > TICK_WAIT_CLICK_END)
                    {
                        btnStatus[btnIdx] = STS_WAIT_CLICK_START;
                    }
                    // 反之，不再继续期待三击事件，调用双击回调函数（如有必要响应三击甚至更多连击事件，跟单击等待双击一样追加相关标志位和等待逻辑即可）
                    else {
                        btnStatus[btnIdx] = STS_WAIT_CLICK_START;
                        if (callBackFunc != 0) callBackFunc(KEY_EVENT_DBCLICK);
                    }
                }
            }
        #endif
    #endif
}
