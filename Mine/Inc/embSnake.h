/******************************************************************************************************
 * ̰ʳ��ģ��
 * File  : embSnake.c
 * Author: MANDA (slipperstree@gmail.com)
 * Date  : 2019.11
 * Updt  : 2021.07
 * Func  : ʵ����̰ʳ�ߵĻ�������ģ���Լ�һ���Զ�Ѱ��ʳ����㷨��
 *         ����������ʾ�߼���ֻ������Ϸ״̬�͵�ͼ״̬������Ӳ���޹ؿ�ֱ����ֲ������Ӳ��ƽ̨��
 ******************************************************************************************************/

#ifndef __EMB_SNAKE_H_
#define __EMB_SNAKE_H_

#include "def_types.h"

// ### ̰ʳ��ģ�� ���� ������Ҫ�޸����¶��� ##################################################################

// ��ͼ��С(����Ļ�ֱ���)
#define SNAKE_SIZE_X  25
#define SNAKE_SIZE_Y  16
// #define SNAKE_SIZE_X  19
// #define SNAKE_SIZE_Y  7

// ��ʾģʽ������ʾλ��
#define SNAKE_DEMO_TITLE_1_X  5
#define SNAKE_DEMO_TITLE_1_Y  6
#define SNAKE_DEMO_TITLE_2_X  (SNAKE_DEMO_TITLE_1_X+5)
#define SNAKE_DEMO_TITLE_2_Y  (SNAKE_DEMO_TITLE_1_Y)
#define SNAKE_DEMO_TITLE_3_X  (SNAKE_DEMO_TITLE_2_X+5)
#define SNAKE_DEMO_TITLE_3_Y  (SNAKE_DEMO_TITLE_1_Y)
#define SNAKE_DEMO_TITLE_4_X  (SNAKE_DEMO_TITLE_3_X+5)
#define SNAKE_DEMO_TITLE_4_Y  (SNAKE_DEMO_TITLE_1_Y)

// �Ƿ�ʹ���ⲿ�����������������ſ�����궨���ʾʹ���ⲿ���������������ô��Ҫ���ⲿc�ļ���ʵ�����к�����(ʵ������common.c)
// �ⲿ��Ҫʵ�ֵĺ���: u16 My_real_rand()
// �������Ҫ�ﵽ�������Ч��������ע�͵�������壬��ֱ��ʹ�ñ�׼rand()��������ÿ���ϵ���������ж��ǹ̶��ģ��Ǽ������
#define USE_EXTERN_REAL_RAND_FUNC

// ### ����Ķ��岻��Ҫ�� ###################################################################################

// �¼���������
typedef void (*pSnakeEventFunc)();
extern u16 SnakeEventId, SnakeEventParam1, SnakeEventParam2;

// �¼� SNAKE_EVENT_UPDATE_BLOCK �Ĳ���3��ȡֵ
#define SNAKE_VIEW_APPLE    0
#define SNAKE_VIEW_HEAD_R   1
#define SNAKE_VIEW_HEAD_L   2
#define SNAKE_VIEW_HEAD_U   3
#define SNAKE_VIEW_HEAD_D   4
#define SNAKE_VIEW_BODY_V   5
#define SNAKE_VIEW_BODY_H   6
#define SNAKE_VIEW_BODY_DR  7
#define SNAKE_VIEW_BODY_LD  8
#define SNAKE_VIEW_BODY_LU  9
#define SNAKE_VIEW_BODY_UR  10
#define SNAKE_VIEW_TAIL_R   11
#define SNAKE_VIEW_TAIL_L   12
#define SNAKE_VIEW_TAIL_U   13
#define SNAKE_VIEW_TAIL_D   14
#define SNAKE_VIEW_EMPTY    15
#define SNAKE_VIEW_WALL     16

// �¼�ID
#define SNAKE_EVENT_RESTART         0  // ������Ϸ���޲���
#define SNAKE_EVENT_GAMEOVER        1  // ��Ϸ����������1������
#define SNAKE_EVENT_UPDATE_BLOCK    2  // ����ˢ��ָ��λ�á�����1����Ҫ���µĵ�ͼx���꣬����2����Ҫ���µĵ�ͼy����
#define SNAKE_EVENT_EAT_APPLE       3  // �Ե�ƻ��������1������
#define SNAKE_EVENT_UPDATE_AI_PATH  4  // �����á�����1����Ҫ���µ�x���꣬����2����Ҫ���µ�y����

// ### ���⹫������ ##########################
// ģ���ʼ����ֻӦ�ñ�����һ��
void SNAKE_init(pSnakeEventFunc);

// ������Ϸ(���������ͼ�е�ǽ����Ϣ������ǿ�Ƶ��� SNAKE_init ����ʹ�� SNAKE_clearWall / SNAKE_delWall)
void SNAKE_restart(void);

// ��ָ�������ƶ���ͷ
u8 SNAKE_moveUp(void);
u8 SNAKE_moveDown(void);
u8 SNAKE_moveLeft(void);
u8 SNAKE_moveRight(void);

// ����ת ���Ե�ǰ����ķ���Ϊ��׼ ��
u8 SNAKE_turnLeft(void);
// ����ת ���Ե�ǰ����ķ���Ϊ��׼ ��
u8 SNAKE_turnRight(void);

// ��ǰ����ǰ��һ���������Զ�ǰ���Ĺ��ܣ���ģ���Լ��������Զ�ǰ������Ҫ���ⲿ��ʱ�������������
void SNAKE_moveNext(void);

// AI�Զ���ĳ��������һ��
void SNAKE_AI_moveNext(void);

// ��������һ��ʳ��
void SNAKE_reCreateFood(void);

// �����鵼���ͼ
void SNAKE_loadWall(u8* wallMap);
// ����һ���ϰ���
void SNAKE_addWall(u8, u8);
// ���һ���ϰ���
void SNAKE_delWall(u8 x, u8 y);
// ��������ϰ���
void SNAKE_clearWall();

// ��ָ�������ƶ���ͷ
void SNAKE_moveAppleUp(void);
void SNAKE_moveAppleDown(void);
void SNAKE_moveAppleLeft(void);
void SNAKE_moveAppleRight(void);

// ��ȡ��ǰ��ͼָ��λ�õ�ԭʼ����,һ�㲻ֱ��ʹ�ã����������Ϣ��һ���ֽ��У�
u8 SNAKE_getMapData(u8, u8);

// ��ȡ��ǰ��ͼ��ʾ������,����ֵΪ SNAKE_VIEW_XXX ����һ�����ɸ��ݷ��ص�ViewIdֱ��ˢ�»��棩
u8 SNAKE_getView(u8, u8);

// ��ȡ��ǰ����
u16 SNAKE_getNowScroe(void);

#ifdef SNAKE_AI_DEBUG
union dfsMapType snake_getPointDataAI(u8, u8);
#endif

#endif
