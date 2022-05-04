/****************************************************************
 * 按键扫描框架
 * File  : key.c
 * Author: chenling
 * Date  : 2021.7
 * Func  : 按键扫描框架，支持事件：单击/双击/按下/放开/长按/连击
 ****************************************************************
 *  使用例, 两个按键, 分别为两个按键定义事件响应函数btn1Func和btn2Func
 *  然后作为参数传入Key_init，最后在主循环中调用KEY_keyscan即可。
 *  示例代码:
 *    void btn1Func(u8 event_id){ 
 *       switch(event_id){
 *       case KEY_EVENT_CLICK:
 *           // 按键1单击处理
 *           // ...
 *           break;
 *       case KEY_EVENT_LONG_PRESS:
 *           // 按键1长按处理
 *           // ...
 *           break;
 *       } 
 *    }
 * 
 *    void btn2Func(u8 event_id){ 
 *       switch(event_id){
 *       case KEY_EVENT_CLICK:
 *           // 按键2单击处理
 *           // ...
 *           break;
 *       } 
 *    }
 * 
 *    void main(){
 *      // 将各按键的事件处理函数传入KEY_init
 *      KEY_init(btn1Func, btn2Func);
 *      while(1){
 *          // 在主循环中扫描按键，一旦检测到事件会回调给各事件函数
 *          KEY_keyscan();
 *      }
 *    }
 *****************************************************************/
#ifndef __KEY_H_
#define __KEY_H_

// for G&W
#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_LEFT 3
#define KEY_RIGHT 4
#define KEY_A 5
#define KEY_B 6
#define KEY_GAME 7
#define KEY_TIME 8
#define KEY_PAUSE 9
#define KEY_POWER 10



// 按键回调事件函数指针类型定义 （不要修改）
// 定义了一个名为『pBtnEventFunc』的函数指针类型，可以指向的函数没有返回值，有一个参数用于接受不同事件(见后面的事件宏定义)
typedef void (*pBtnEventFunc)(u8);

// 事件ID定义
#define KEY_EVENT_CLICK         0 //单击（按下并在指定时间内放开后触发一次）此事件与按下/抬起事件只能选择其一
#define KEY_EVENT_DOWN          1 //按下
#define KEY_EVENT_UP            2 //抬起
#define KEY_EVENT_DBCLICK       3 //双击（连续两次有效的单击后触发一次）此事件启用时在完成双击事件之前，已经确认有效的单击事件不会被触发
#define KEY_EVENT_LONG_PRESS    4 //长按（按住不放也只会触发一次）
#define KEY_EVENT_KEEPING_PRESS 5 //按住不放连发（特定时间间隔连续触发）

void KEY_init(
    pBtnEventFunc btn1Func,
    pBtnEventFunc btn2Func,
    pBtnEventFunc btn3Func,
    pBtnEventFunc btn4Func,
    pBtnEventFunc btn5Func,
    pBtnEventFunc btn6Func,
    pBtnEventFunc btn7Func,
    pBtnEventFunc btn8Func,
    pBtnEventFunc btn9Func,
    pBtnEventFunc btn10Func
    );

// 主循环里调用该函数
void KEY_keyscan();

#endif