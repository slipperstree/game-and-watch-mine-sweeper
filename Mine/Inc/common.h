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

// ����������ɺ���
u16 My_real_rand();

// �������� ��������ѡ������ISDEBUG����Ч =============================================
#define ISDEBUG          0              //DEBUGģʽ�ܿ��ء�1=������0=�ر�
#define DEBUG_SKIP_HOME  0              //����HOMEҳ
#define DEBUG_DEMO_SPEED_L 200          //DEMO�ٶȡ�����Ϊ50������ԽСԽ��
#define DEBUG_DEMO_SPEED_M 10000          //DEMO�ٶȡ�����Ϊ50������ԽСԽ��
#define DEBUG_DEMO_SPEED_H  50          //DEMO�ٶȡ�����Ϊ50������ԽСԽ��
#define DEBUG_DEMO_SPEED_S   1          //DEMO�ٶȡ�����Ϊ50������ԽСԽ��
#define DEBUG_DEMO_APPLE 50             //DEMO��ʳ�����ޣ�������������
#define DEBUG_DEMO_GAMEOVER_NOWAIT 0    //DEMO�����󲻵ȴ�ֱ������

// �ٶȵȼ�������Խ���ٶ�Խ��
#if ISDEBUG
    #define SPEED_DEFAULT   500      //��ʼ�ٶ�
    #define SPEED_MAX       100      //����ٶ�
    #define SPEED_INTERVAL   50      //ÿ���ȼ��ٶ�������ֵ
    #define SPEED_DEMO_L    DEBUG_DEMO_SPEED_L      //DEMO�ٶȵ�
    #define SPEED_DEMO_M    DEBUG_DEMO_SPEED_M      //DEMO�ٶ���
    #define SPEED_DEMO_H    DEBUG_DEMO_SPEED_H      //DEMO�ٶȿ�
    #define SPEED_DEMO_S    DEBUG_DEMO_SPEED_S      //DEMO�ٶȳ���
#else
    #define SPEED_DEFAULT    30      //��ʼ�ٶ�
    #define SPEED_MAX         6      //����ٶ�
    #define SPEED_INTERVAL    2      //ÿ���ȼ��ٶ�������ֵ
    #define SPEED_DEMO_L     30      //DEMO�ٶ���(��ʼ)
    #define SPEED_DEMO_M     15      //DEMO�ٶ���
    #define SPEED_DEMO_H      9      //DEMO�ٶȿ�
    #define SPEED_DEMO_S      6      //DEMO�ٶȳ���
#endif

// LOG������ã��������ã���ISDEBUG�޹� =============================================
#define PRINT_LOG        0              //�Ƿ����log 1-�����0-�����
#if PRINT_LOG
    // Tips: ���ʹ��printf��ӡ���ֵĻ���51��Ƭ�����׼C������
    // KEIL����չ����b,h,l���������ֽڿ�����ã�
    // ��1��b��λ
    // ��2��hʮ��λ
    // ��3��l��ʮ��λ
    // �����ӡuchar�����֣�Ӧ��д�� printf("%bd", a), ������ printf("%d", a)������Ὣǰ8λ���ڴ�һ���ӡ����
    #define LOG(str)   UART1_Isr_Send_String(str)     //���log�õĺ궨�壬���庯����Ҫ����ʵ�֣����紮�ڴ�ӡ��������printf�ض���
#else
    #define LOG(str)   // ��һ�в�Ҫ�޸ģ��������ʱ��ҲҪ�����յĺ궨�壬��Ȼ����ᱨ��
#endif


#endif