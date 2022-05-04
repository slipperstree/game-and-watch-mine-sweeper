/******************************************************************************************************
 * ɨ��ģ��
 * File  : embMine.c
 * Author: MANDA (slipperstree@gmail.com)
 * Date  : 2021.08
 * Updt  : 2021.08
 * Func  : ʵ����ɨ�׵Ļ�������ģ�ͣ�
 *         ����������ʾ�߼���ֻ������Ϸ״̬�͵�ͼ״̬������Ӳ���޹ؿ�ֱ����ֲ������Ӳ��ƽ̨��
 ******************************************************************************************************/

#ifndef __EMB_MINE_H_
#define __EMB_MINE_H_

#include "def_types.h"

// ### ������Ҫ�޸����¶��� ##################################################################

// ��ͼ��С(����Ļ�ֱ���)
#define MINE_SIZE_X     10
#define MINE_SIZE_Y     10

// �Ƿ�ʹ���ⲿ�����������������ſ�����궨���ʾʹ���ⲿ���������������ô��Ҫ���ⲿc�ļ���ʵ�����к�����(ʵ������common.c)
// �ⲿ��Ҫʵ�ֵĺ���: unsigned int My_real_rand()
// �������Ҫ�ﵽ�������Ч��������ע�͵�������壬��ֱ��ʹ�ñ�׼rand()��������ÿ���ϵ���������ж��ǹ̶��ģ��Ǽ������
#define USE_EXTERN_REAL_RAND_FUNC

// ### ����Ķ��岻��Ҫ�� ###################################################################################

// �¼���������
typedef void (*pMineEventFunc)();
extern u16 MineEventId, MineEventParam1, MineEventParam2;

// �������� MINE_EVENT_UPDATE_BLOCK �Ĳ���3��ȡֵ
#define MINE_VIEW_NORMAL           0  // δ����
#define MINE_VIEW_DIGED            1  // �ѷ���
#define MINE_VIEW_FLAG             2  // ����
#define MINE_VIEW_MINE             3  // �ڵ���ʱ����ʾ����δ�������
#define MINE_VIEW_WRONG            4  // �ڵ���ʱ����ʾ���������׵�����������ӵĵط�
#define MINE_VIEW_DEADMINE         5  // �ڵ���ʱ����ʾ��ը����

// �¼�ID
#define MINE_EVENT_RESTART         0  // ������Ϸ���޲���
#define MINE_EVENT_GAMEOVER        1  // ��Ϸ����������1��ʤ��1����ʧ��0
#define MINE_EVENT_UPDATE_BLOCK    2  // ����ˢ��ָ��λ�á�����1����Ҫ���µĵ�ͼx���꣬����2����Ҫ���µĵ�ͼy����
#define MINE_EVENT_UPDATE_FLAG_CNT 3  // ����ˢ��ʣ��������������1��������
#define MINE_EVENT_TIMESTART       4  // ����ʼ��ʱ����һ�ε��ʱ�ᴥ�������޲���

// ### ���⹫������ ##########################
// ģ���ʼ����ֻӦ�ñ�����һ��
void MINE_init(pMineEventFunc);

// ������Ϸ
void MINE_restart(void);

// �ڿ�
void MINE_click(u8, u8);

// ����/����
void MINE_switchFlag(u8, u8);

// ��ȡ��ǰ��ͼ��ʾ������MINE_VIEW_XXX ����һ�����ɸ��ݷ��ص�ViewIdֱ��ˢ�»��棩
u8 MINE_getView(u8, u8);

// ��ȡָ��λ����Χ�׵ĸ�����ֻ�е�ָ��λ�ò�Ϊ�ײ������壬�����λ��������᷵��MINE_STS_MINE(10)����ʱ�������λ����Χ��10���ף�
u8 MINE_getViewNum(u8 mapX, u8 mapY);

void MINE_setMineNUM(u8 num);

// ��ȡ��ǰʣ��������
u16 MINE_getNowFlags(void);

#endif
