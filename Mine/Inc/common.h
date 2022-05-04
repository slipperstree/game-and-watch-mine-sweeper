#ifndef __EMB_SNAKE_COMMON_H_
#define __EMB_SNAKE_COMMON_H_

#include "binary.h"
#include "font.h"
#include "color.h"
#include "myMathUtil.h"

#include "def_types.h"

#include "buttons.h"
#include "flash.h"
#include "lcd.h"

void My_delay_ms(u16 n);
u8 My_strlen(char* x);

// 真随机数生成函数
u16 My_real_rand();

// 调试设置 以下所有选项都必须打开ISDEBUG才有效 =============================================
#define ISDEBUG          0              //DEBUG模式总开关。1=开启，0=关闭
#define DEBUG_SKIP_HOME  0              //跳过HOME页
#define DEBUG_DEMO_SPEED_L 200          //DEMO速度。正常为50，数字越小越快
#define DEBUG_DEMO_SPEED_M 10000          //DEMO速度。正常为50，数字越小越快
#define DEBUG_DEMO_SPEED_H  50          //DEMO速度。正常为50，数字越小越快
#define DEBUG_DEMO_SPEED_S   1          //DEMO速度。正常为50，数字越小越快
#define DEBUG_DEMO_APPLE 50             //DEMO吃食物上限（超过就重来）
#define DEBUG_DEMO_GAMEOVER_NOWAIT 0    //DEMO死亡后不等待直接重来

// 速度等级，分数越高速度越快
#if ISDEBUG
    #define SPEED_DEFAULT   500      //初始速度
    #define SPEED_MAX       100      //最高速度
    #define SPEED_INTERVAL   50      //每个等级速度相差的数值
    #define SPEED_DEMO_L    DEBUG_DEMO_SPEED_L      //DEMO速度低
    #define SPEED_DEMO_M    DEBUG_DEMO_SPEED_M      //DEMO速度中
    #define SPEED_DEMO_H    DEBUG_DEMO_SPEED_H      //DEMO速度快
    #define SPEED_DEMO_S    DEBUG_DEMO_SPEED_S      //DEMO速度超快
#else
    #define SPEED_DEFAULT    30      //初始速度
    #define SPEED_MAX         6      //最高速度
    #define SPEED_INTERVAL    2      //每个等级速度相差的数值
    #define SPEED_DEMO_L     30      //DEMO速度慢(初始)
    #define SPEED_DEMO_M     15      //DEMO速度中
    #define SPEED_DEMO_H      9      //DEMO速度快
    #define SPEED_DEMO_S      6      //DEMO速度超快
#endif

// LOG输出设置，单独设置，与ISDEBUG无关 =============================================
#define PRINT_LOG        0              //是否输出log 1-输出，0-不输出
#if PRINT_LOG
    // Tips: 如果使用printf打印数字的话，51单片机与标准C有区别。
    // KEIL里扩展出了b,h,l来对输入字节宽的设置：
    // （1）b八位
    // （2）h十六位
    // （3）l三十二位
    // 比如打印uchar的数字，应该写成 printf("%bd", a), 而不是 printf("%d", a)，否则会将前8位的内存一起打印出来
    #define LOG(str)   UART1_Isr_Send_String(str)     //输出log用的宏定义，具体函数需要自行实现，比如串口打印函数或者printf重定向
#else
    #define LOG(str)   // 这一行不要修改，不输出的时候也要保留空的宏定义，不然编译会报错
#endif


#endif