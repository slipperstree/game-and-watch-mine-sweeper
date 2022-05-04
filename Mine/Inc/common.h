#ifndef __EMB_MINE_COMMON_H_
#define __EMB_MINE_COMMON_H_

#include "binary.h"
#include "font.h"
#include "color.h"
#include "myMathUtil.h"

#include "def_types.h"

#include "buttons.h"
#include "flash.h"
#include "lcd.h"

// ��ͨ�궨��
#ifndef FALSE
#define FALSE		0
#endif
#ifndef TRUE
#define TRUE		1
#endif

void My_delay_ms(u16 n);
u8 My_strlen(char* x);

// ����������ɺ���
u16 My_real_rand();

// �������� ��������ѡ������ISDEBUG����Ч =============================================
#define ISDEBUG          0              //DEBUGģʽ�ܿ��ء�1=������0=�ر�
#define DEBUG_SKIP_HOME  1              //����HOMEҳ
#define DEBUG_DEMO_GAMEOVER_NOWAIT 0    //DEMO�����󲻵ȴ�ֱ������

// LOG������ã��������ã���ISDEBUG�޹� =============================================
#define PRINT_LOG        0              //�Ƿ����log 1-�����0-�����
#if PRINT_LOG
    // Tips: ���ʹ��printf��ӡ���ֵĻ���51��Ƭ�����׼C������
    // KEIL����չ����b,h,l���������ֽڿ�����ã�
    // ��1��b��λ
    // ��2��hʮ��λ
    // ��3��l��ʮ��λ
    // �����ӡuchar�����֣�Ӧ��д�� printf("%bd", a), ������ printf("%d", a)������Ὣǰ8λ���ڴ�һ���ӡ����
    #define LOG   printf                //���log�õĺ궨�壬���庯����Ҫ����ʵ�֣����紮�ڴ�ӡ��������printf�ض���
#else
    // ��һ�в�Ҫ�޸ģ��������ʱ��ҲҪ�����յĺ궨�壬��Ȼ����ᱨ��
    #define LOG   /##/
#endif


#endif