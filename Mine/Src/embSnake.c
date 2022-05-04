/******************************************************************************************************
 * ɨ��ģ��
 * File  : embMine.c
 * Author: MANDA (slipperstree@gmail.com)
 * Date  : 2021.08
 * Updt  : 2021.08
 * Func  : ʵ����ɨ�׵Ļ�������ģ�ͣ�
 *         ����������ʾ�߼���ֻ������Ϸ״̬�͵�ͼ״̬������Ӳ���޹ؿ�ֱ����ֲ������Ӳ��ƽ̨��
 ******************************************************************************************************/

#include "embSnake.h"

#ifndef USE_EXTERN_REAL_RAND_FUNC
#include "stdlib.h"
#define RAND_FUNC rand
#endif

#ifdef USE_EXTERN_REAL_RAND_FUNC
extern u16 extRealRand4EmbSnake();
#define RAND_FUNC extRealRand4EmbSnake
#endif

// ��ʱ������(sprintf��)
extern char buff[128];

// ����ģʽ
//#define SNAKE_AI_DEBUG

// ��Ϸ״̬
#define GAME_STS_PLAY 		0	// ��Ϸ��
#define GAME_STS_GAMEOVER	1	// ��Ϸ����
#define GAME_STS_SETTING	2	// ����
#define GAME_STS_PAUSE		3	// ��ͣ

// ��ͼ��ά����ȡֵ���壬�����ⲿ��ʾģ���ͼ��
#define STS_EMPTY 0				// ��ʾ�õ�Ϊ��
#define STS_APPLE 1		        // ��ʾ�õ�Ϊʳ��

#define DIRECTION_UP 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3
#define DIRECTION_RIGHT 4

#define STS_NEXT_TO_U 1
#define STS_NEXT_TO_D 2
#define STS_NEXT_TO_L 3
#define STS_NEXT_TO_R 4

// ��ǰ���������Զ�ǰ��
#define NOW_TOWARDS_U 1	
#define NOW_TOWARDS_D 2		
#define NOW_TOWARDS_L 3		
#define NOW_TOWARDS_R 4

// ������Ķ���
// ��ͷ����ͷ50+������ͷ��������(1-4)
// ��������10/20/30/40��+����һ������ķ���(1-4)
// ��β����β60+������β��������(1-4)
//     ʮλ�� : ��ͷ����������4��������Ϣ������β
//     ��λ�� : ����block�ķ���
//     ���ϸ����Ϊ�˷�������Ļ�ͼģ����Ƹ��ÿ�ת���������������ߣ�
//     ����ʾЧ��Ҫ�󲻸�Ҳ���Լ�ȡʮλ��Ϊ1-6����

// ���ʮλ����1-4�������ķ�������1-4Ҫ����һ�£������ĸĳ�6789�Ļ�����ҲҪ�ģ����Ҳ��ܸ���ͷ��β��ͻ
#define STS_BODY_PREV_U 10
#define STS_BODY_PREV_D 20
#define STS_BODY_PREV_L 30
#define STS_BODY_PREV_R 40
// ��ͷ
#define STS_HEAD 50
// ��β
#define STS_TAIL 60
// ��ʾ�õ�Ϊ�ϰ��Ч��������һ����ֻ��Ϊ���Ժ���չ����Ԥ����
#define STS_WALL 90

// SNAKE_AI_moveNext ��AI�㷨д�˺ü���ʵ�֣�Խ��rom�ռ�ԽС��������Ҫ�ſ�����4�������е�ĳһ��������ͬʱ�ſ���
//#define AI_LEVEL_BABY     // ����(ֻ��������ߣ�������óɼ�11)
//#define AI_LEVEL_KIDS     // ��ͯ(���Ԥ��2����������óɼ�32)
//#define AI_LEVEL_SMART    // ����(��Ѱ·�ҵ�ƻ����������óɼ�65��ƽ��34) // Realse+�Ż��������rom size<8k
#define AI_LEVEL_MASTER     // ���(��Ѱ·�ҵ�ƻ��������ͬʱ�����������ͬ��������óɼ�)

#if defined (AI_LEVEL_SMART) || defined(AI_LEVEL_MASTER)
    #define AI_STATUS_EMPTY 0
    #define AI_STATUS_ON_THE_WAY 1
    #define AI_STATUS_WAY_TO_DEATH 2
    #define AI_STATUS_WAY_TO_APPLE 3
    // DFS�������̽����ͼʹ�õġ�Ӱ�ӵ�ͼ���������Ͷ���
    // ÿһ������8λ
    // bit[2:0]������һ��·���ķ���1-4����ʵ4������2λ�͹��ˣ�������������(STS_BODY_PREV_X)ҲҪ�ľ���1-4��
    // bit[5:3]������һ��·���ķ���1-4��
    // bit[6:7]
    //    0:δ̽��  1:��·�ϣ�����̽����  2:��̽��������·  3:��̽����ͨ��ƻ��
    union dfsMapType
    {
        struct bit_feild
        {
            u8 next_direction:3;
            u8 prev_direction:3;
            u8 status:2;
        } subDatas;
        u8 dat;
    };
#endif

// �����ߴ�init�����������Ļص�����ָ��
pSnakeEventFunc callbackSnakeEventFunc;
u16 SnakeEventId, SnakeEventParam1, SnakeEventParam2;

// ��Ϸ״̬
u8 gameStatus;

// ��ͼ����
u8 map[SNAKE_SIZE_X][SNAKE_SIZE_Y];

#if defined (AI_LEVEL_SMART) || defined(AI_LEVEL_MASTER)
	// Ѱ·��ά����
	union dfsMapType mapAI[SNAKE_SIZE_X][SNAKE_SIZE_Y];
	s8 x_headAI, y_headAI, stepsFromHeadAI;
	void wander(u8); // ɢ��
#endif

#ifdef AI_LEVEL_MASTER
	// ����ƻ��������ĳ���ط�������ѭ������������˶��ٲ��Ժ�û�гԵ�ƻ������������һ��
	#define RECREATE_APPLE_STEPS (SNAKE_SIZE_X*SNAKE_SIZE_Y*2)
	u16 reCreateAppleStep;

	// ����������ɺö��ƻ������û�Ե�����ô��Ϊ�Ѿ��Բ����ˣ�������һ����ɱ
	#define RE_CREATE_APPLE_CNT 10
	u8 reCreateAppleCnt;
#endif

// ��ͷ��λ��
u8 x_head, y_head;

// ��β��λ��
u8 x_end, y_end;

// ʳ���λ��
u8 x_apple, y_apple;

// ��ǰ����
u8 nowTowards;

// ����
u16 score;

static u8 _x, _y;
static u8 _idx;
static u8 _tmp;

void gameover(void);
void createfood(void);
void moveEndBlock(void);
u8 canMoveComm(u8 direction, u8 x, u8 y, u8 stepsFromHead);
u8 canMoveUp(u8 x, u8 y, u8 stepsFromHead);
u8 canMoveDown(u8 x, u8 y, u8 stepsFromHead);
u8 canMoveLeft(u8 x, u8 y, u8 stepsFromHead);
u8 canMoveRight(u8 x, u8 y, u8 stepsFromHead);
u8 canGetTail(u8 x, u8 y);
void fireEvent(u16 id, u16 p1, u16 p2);

#ifdef AI_LEVEL_KIDS
	// �������ϵ����ɶ�
	u8 freeUp,freeDown,freeLeft,freeRight = 0;

	// ȡ����ֵ
	u8 myAbs(u8 a, u8 b){
		if(a>=b)
			return a-b;
		else
			return b-a;
	}
	
	// ȡĳ��λ�õ����ɶȣ���Χ���ߵķ�������
	u8 ai_kids_getFree(u8 x, u8 y){
		u8 stepsFromHead = myAbs(x_head, x) + myAbs(y_head, y);
		return canMoveUp(x,y,stepsFromHead) + canMoveDown(x,y,stepsFromHead) + canMoveLeft(x,y,stepsFromHead) + canMoveRight(x,y,stepsFromHead);
	}
#endif

void fireEvent(u16 id, u16 p1, u16 p2){
	SnakeEventId = id;
	SnakeEventParam1 = p1;
	SnakeEventParam2 = p2;
	callbackSnakeEventFunc();
}

// ��ȡָ��λ�õ���ͼ���ݣ���ͼ���ݲ���ȫ��ͬ��ͼԭʼ���ݣ�������Ȼ�������壬���й������ֱ�ģ�����ķ���Ҳ�ǲ�һ���ģ�
u8 SNAKE_getView(u8 mapX, u8  mapY){
	u8 blockSts, shiwei, gewei;
	blockSts = map[mapX][mapY];

	if (blockSts == STS_APPLE)
	{
		return SNAKE_VIEW_APPLE;
	} else {
		shiwei = (blockSts / 10) * 10;
		gewei = blockSts % 10;
		
		switch (shiwei)
		{
		case STS_HEAD:
			switch (gewei)
			{
			case STS_NEXT_TO_U:
				return SNAKE_VIEW_HEAD_U;
			case STS_NEXT_TO_D:
				return SNAKE_VIEW_HEAD_D;
			case STS_NEXT_TO_L:
				return SNAKE_VIEW_HEAD_L;
			case STS_NEXT_TO_R:
				return SNAKE_VIEW_HEAD_R;
			default:
				//return SNAKE_VIEW_HEAD_U;
				break;
			}
			break;
		case STS_BODY_PREV_U:
			switch (gewei)
			{
			case STS_NEXT_TO_D:
				return SNAKE_VIEW_BODY_V;
			case STS_NEXT_TO_L:
				return SNAKE_VIEW_BODY_LU;
			case STS_NEXT_TO_R:
				return SNAKE_VIEW_BODY_UR;
			default:
				//return SNAKE_VIEW_BODY_V;
				break;
			}
			break;
		case STS_BODY_PREV_D:
			switch (gewei)
			{
			case STS_NEXT_TO_U:
				return SNAKE_VIEW_BODY_V;
			case STS_NEXT_TO_L:
				return SNAKE_VIEW_BODY_LD;
			case STS_NEXT_TO_R:
				return SNAKE_VIEW_BODY_DR;
			default:
				//return SNAKE_VIEW_BODY_V;
				break;
			}
			break;
		case STS_BODY_PREV_L:
			switch (gewei)
			{
			case STS_NEXT_TO_U:
				return SNAKE_VIEW_BODY_LU;
			case STS_NEXT_TO_D:
				return SNAKE_VIEW_BODY_LD;
			case STS_NEXT_TO_R:
				return SNAKE_VIEW_BODY_H;
			default:
				//return SNAKE_VIEW_BODY_LU;
				break;
			}
			break;
		case STS_BODY_PREV_R:
			switch (gewei)
			{
			case STS_NEXT_TO_U:
				return SNAKE_VIEW_BODY_UR;
			case STS_NEXT_TO_D:
				return SNAKE_VIEW_BODY_DR;
			case STS_NEXT_TO_L:
				return SNAKE_VIEW_BODY_H;
			default:
				//return SNAKE_VIEW_BODY_UR;
				break;
			}
			break;
		case STS_TAIL:
			switch (gewei)
			{
			case STS_NEXT_TO_U:
				return SNAKE_VIEW_TAIL_U;
			case STS_NEXT_TO_D:
				return SNAKE_VIEW_TAIL_D;
			case STS_NEXT_TO_L:
				return SNAKE_VIEW_TAIL_L;
			case STS_NEXT_TO_R:
				return SNAKE_VIEW_TAIL_R;
			default:
				//return SNAKE_VIEW_TAIL_U;
				break;
			}
			break;
		case STS_EMPTY:
			return SNAKE_VIEW_EMPTY;
		case STS_WALL:
			return SNAKE_VIEW_WALL;
		default:
			break;
		}

		return SNAKE_VIEW_EMPTY;
	}
}

// ������Ҫ����map���ݵ�ʱ�򶼵��������������֤�ܼ�ʱ֪ͨ���ƶ˸�����Ӧ�Ļ���
void updateMap(u8 x, u8 y, u8 sts){
	map[x][y] = sts;
	fireEvent(SNAKE_EVENT_UPDATE_BLOCK, x, y);
}

#if defined (AI_LEVEL_SMART) || defined(AI_LEVEL_MASTER)
void clearMapAI(){
	u8 x,y;
	for (x = 0; x < SNAKE_SIZE_X; ++x)
	{
		for (y = 0; y < SNAKE_SIZE_Y; ++y)
		{
			mapAI[x][y].dat = 0;
		}
	}

	stepsFromHeadAI=0;
}
#endif

// ������Ϸ(���������ͼ�е�ǽ����Ϣ)
void SNAKE_restart(){
	u8 x, y;
	// ������Ϸ���������ͼ�е�ǽ����Ϣ
	for (x = 0; x < SNAKE_SIZE_X; ++x)
	{
		for (y = 0; y < SNAKE_SIZE_Y; ++y)
		{
			//if(map[x][y] != STS_WALL) map[x][y] = STS_EMPTY;
			map[x][y] = STS_EMPTY;
		}
	}

	#if defined (AI_LEVEL_SMART) || defined(AI_LEVEL_MASTER)
	clearMapAI();
	#endif

	// �¼�����û��ʹ��
	fireEvent(SNAKE_EVENT_RESTART, 0, 0);

	// ��ʼ����
	score = 0;

	gameStatus = GAME_STS_PLAY;

	// ��ʼblock β��-����-ͷ ->
	x_head = 2, y_head = 0;
	x_end = 0, y_end = 0;

	updateMap(x_end, y_end, STS_TAIL + STS_NEXT_TO_R);
	updateMap(1, 0, STS_BODY_PREV_R + STS_NEXT_TO_L);
	updateMap(x_head, y_head, STS_HEAD + STS_NEXT_TO_L);
	nowTowards = DIRECTION_RIGHT;

	createfood();
}

// ģ���ʼ��,ֻӦ�ñ�����һ��,init�������Ժ�Ϳ��Լ����ϰ�����
void SNAKE_init(pSnakeEventFunc pCallbackSnakeEventFunc){
	u8 x, y;

	callbackSnakeEventFunc = pCallbackSnakeEventFunc;

	// ��ֹ�ڴ������������ݣ���ʼ�����е�ͼΪ��(��֪ͨ�ⲿˢ�»���)
	for (x = 0; x < SNAKE_SIZE_X; ++x)
	{
		for (y = 0; y < SNAKE_SIZE_Y; ++y)
		{
			map[x][y] = STS_EMPTY;
		}
	}
}

void gameover(){
	gameStatus = GAME_STS_GAMEOVER;
	fireEvent(SNAKE_EVENT_GAMEOVER, score, 0);
}

// ��ǰ״̬��N���Ժ�ָ�������Ƿ�Ϊ��β
// ����Ϊ���жϵ���ͷ�߹�N�����Ժ�����������ʱ��ĳ�������ϱ���������
//   ����N���Ժ��п��ܱ����β����ô�����������ɶ�Ҳ��Ч
//   �����������һ����ô���ɶȾ���2�����ϣ�����ʵӦ����3�����ϣ��ң�
//   ����ͷ��һ����A��ʱ����βҲ�ƶ���һ��������A����Ҳ�������β��A�����ͷ������ת���Ҳ�ģ�����ײ����β��
//   ������ͼ��-Ϊ��β��@Ϊ��ͷ��+Ϊ��������Ҫ�ж�A������ɶȣ�
//    -
//   A+	
//   @+	
//   ++    
//   ��ͷ�����ߵ�A�������������ӣ��Ҳ�����β�����ߵ�ͨ
//     
//   @-
//   ++
//   ++
//   �������ߣ�û������
//     
//   +@
//   +-
//   ++
#ifdef AI_LEVEL_MASTER
u8 isTail(u8 x, u8 y, u8 afterSteps){
	u8 step;
	u8 tmp_tail_x = x_end;
	u8 tmp_tail_y = y_end;
	u8 tailNextTo;

    //��ֱ���жϵ�ǰ������ǲ��Ǿ�����β������ǾͲ���Ҫ��N�����ж���
	if (tmp_tail_x == x && tmp_tail_y == y)
	{
		return 1;
	}

	// ����β�������������Ĳ���
	for (step = 0; step < afterSteps; step++)
	{
		if(step == 0){
			// ��һ������β������Ӹ�λ��ȡ
			tailNextTo = map[tmp_tail_x][tmp_tail_y] % 10;
		} else {
			// ���������ʮλ��ȡ����λ���Ǻ������ӵķ���
			tailNextTo = map[tmp_tail_x][tmp_tail_y] / 10;
		}
		
		switch (tailNextTo)
		{
		case STS_NEXT_TO_U:
			tmp_tail_y--;
			break;
		case STS_NEXT_TO_D:
			tmp_tail_y++;
			break;
		case STS_NEXT_TO_L:
			tmp_tail_x--;
			break;
		case STS_NEXT_TO_R:
			tmp_tail_x++;
			break;
		default:
			break;
		}

		// ֻҪ��;����β���������ͱ�ʾ������ǰ�ȫ��
		if (tmp_tail_x == x && tmp_tail_y == y)
		{
			return 1;
		}
	}

	return 0;

	// if (tmp_tail_x == x && tmp_tail_y == y)
	// {
	// 	return 1;
	// } else {
	// 	return 0;
	// }
}
#endif

// x,y : ��������λ��
// stepsFromHead : ����ͷ�ߵ�x,y�Ѿ��߹��˶��ٲ���Ϊ��Ԥ����β��λ�ã�
u8 canMoveComm(u8 direction, u8 x, u8 y, u8 stepsFromHead){
	switch (direction)
	{
	case DIRECTION_UP:
		return canMoveUp(x, y, stepsFromHead);
	case DIRECTION_DOWN:
		return canMoveDown(x, y, stepsFromHead);
	case DIRECTION_LEFT:
		return canMoveLeft(x, y, stepsFromHead);
	case DIRECTION_RIGHT:
		return canMoveRight(x, y, stepsFromHead);
	default:
		return 0;
	}
}

u8 canMoveRight(u8 x, u8 y, u8 stepsFromHead){

	if (x<SNAKE_SIZE_X-1 && map[x+1][y] == STS_APPLE) {
		return 5;
	#ifdef AI_LEVEL_MASTER
	} else if (x<SNAKE_SIZE_X-1 && (map[x+1][y] == STS_EMPTY ||isTail(x+1,y,stepsFromHead) )) {
	#else
	} else if (x<SNAKE_SIZE_X-1 && (map[x+1][y] == STS_EMPTY)) {
	#endif
		return 1;
	} else {
		return 0;
	}
}

u8 canMoveLeft(u8 x, u8 y, u8 stepsFromHead){

	if (x>0 && map[x-1][y] == STS_APPLE) {
		return 5;
	#ifdef AI_LEVEL_MASTER
	} else if (x>0 && (map[x-1][y] == STS_EMPTY || isTail(x-1,y,stepsFromHead) )) {
	#else
	} else if (x>0 && (map[x-1][y] == STS_EMPTY)) {
	#endif
		return 1;
	} else {
		return 0;
	}
}

u8 canMoveDown(u8 x, u8 y, u8 stepsFromHead){

	if (y<SNAKE_SIZE_Y-1 && map[x][y+1] == STS_APPLE) {
		return 5;
	#ifdef AI_LEVEL_MASTER
	} else if (y<SNAKE_SIZE_Y-1 && ( map[x][y+1] == STS_EMPTY || isTail(x,y+1,stepsFromHead)) ) {
	#else
	} else if (y<SNAKE_SIZE_Y-1 && ( map[x][y+1] == STS_EMPTY ) ) {
	#endif
		return 1;
	} else {
		return 0;
	}
}

u8 canMoveUp(u8 x, u8 y, u8 stepsFromHead){

	if (y>0 && map[x][y-1] == STS_APPLE) {
		return 5;
	#ifdef AI_LEVEL_MASTER
	} else if (y>0 && (map[x][y-1] == STS_EMPTY || isTail(x,y-1,stepsFromHead))) {
	#else
	} else if (y>0 && (map[x][y-1] == STS_EMPTY)) {
	#endif
		return 1;
	} else {
		return 0;
	}
}

#if defined (AI_LEVEL_SMART) || defined(AI_LEVEL_MASTER)
	void updateMapAI(u8 x, u8 y, u8 status){
		mapAI[x][y].subDatas.status = status;

		#ifdef SNAKE_AI_DEBUG
		fireEvent(SNAKE_EVENT_UPDATE_AI_PATH, x, y);
		#endif
	}

	u8 canMoveCommAI(u8 direction){
		switch (direction)
		{
		case DIRECTION_UP:
			if (canMoveUp(x_headAI, y_headAI, stepsFromHeadAI)
					&& mapAI[x_headAI][y_headAI-1].subDatas.status == AI_STATUS_EMPTY
					//&& canGetTail(x_headAI, y_headAI-1)
					)
			{
				return 1;
			} else {
				return 0;
			}
		case DIRECTION_DOWN:
			if (canMoveDown(x_headAI, y_headAI, stepsFromHeadAI) 
					&& mapAI[x_headAI][y_headAI+1].subDatas.status == AI_STATUS_EMPTY 
					//&& canGetTail(x_headAI, y_headAI+1)
					)
			{
				return 1;
			} else {
				return 0;
			}
		case DIRECTION_LEFT:
			if (canMoveLeft(x_headAI, y_headAI, stepsFromHeadAI) 
					&& mapAI[x_headAI-1][y_headAI].subDatas.status == AI_STATUS_EMPTY
					//&& canGetTail(x_headAI-1, y_headAI)
					)
			{
				return 1;
			} else {
				return 0;
			}
		case DIRECTION_RIGHT:
			if (canMoveRight(x_headAI, y_headAI, stepsFromHeadAI)
					&& mapAI[x_headAI+1][y_headAI].subDatas.status == AI_STATUS_EMPTY 
					//&& canGetTail(x_headAI+1, y_headAI)
					)
			{
				return 1;
			} else {
				return 0;
			}
		default:
			return 0;
		}
	}

	void markDeadwayAndGoBackToPrevAI(){
		updateMapAI(x_headAI, y_headAI, AI_STATUS_WAY_TO_DEATH);

		// �ص��ϸ��ڵ�
		switch (mapAI[x_headAI][y_headAI].subDatas.prev_direction)
		{
		case DIRECTION_LEFT:
			x_headAI--;
			break;
		case DIRECTION_RIGHT:
			x_headAI++;
			break;
		case DIRECTION_UP:
			y_headAI--;
			break;
		case DIRECTION_DOWN:
			y_headAI++;
			break;
		default:
			break;
		}

		if (stepsFromHeadAI > 0)
		{
			stepsFromHeadAI--;
		}
	}

	u8 markApplewayAndGoBackToPrevAI(){
		updateMapAI(x_headAI, y_headAI, AI_STATUS_WAY_TO_APPLE);

		if (x_headAI == x_head && y_headAI == y_head)
		{
			// ��ǰ����ͷ�����ʾ���ݽ���
			return 0;
		} else {
			// �ص��ϸ��ڵ�
			switch (mapAI[x_headAI][y_headAI].subDatas.prev_direction)
			{
			case DIRECTION_LEFT:
				x_headAI--;
				break;
			case DIRECTION_RIGHT:
				x_headAI++;
				break;
			case DIRECTION_UP:
				y_headAI--;
				break;
			case DIRECTION_DOWN:
				y_headAI++;
				break;
			default:
				break;
			}

			return 1;
		}
	}

	void doMoveUpAI(){
		mapAI[x_headAI][y_headAI].subDatas.next_direction = DIRECTION_UP;
		updateMapAI(x_headAI, y_headAI, AI_STATUS_ON_THE_WAY);
		mapAI[x_headAI][y_headAI-1].subDatas.prev_direction = DIRECTION_DOWN;
		y_headAI--;
		stepsFromHeadAI++;
	}

	void doMoveDownAI(){
		mapAI[x_headAI][y_headAI].subDatas.next_direction = DIRECTION_DOWN;
		updateMapAI(x_headAI, y_headAI, AI_STATUS_ON_THE_WAY);
		mapAI[x_headAI][y_headAI+1].subDatas.prev_direction = DIRECTION_UP;
		y_headAI++;
		stepsFromHeadAI++;
	}

	void doMoveLeftAI(){
		mapAI[x_headAI][y_headAI].subDatas.next_direction = DIRECTION_LEFT;
		updateMapAI(x_headAI, y_headAI, AI_STATUS_ON_THE_WAY);
		mapAI[x_headAI-1][y_headAI].subDatas.prev_direction = DIRECTION_RIGHT;
		x_headAI--;
		stepsFromHeadAI++;
	}

	void doMoveRightAI(){
		mapAI[x_headAI][y_headAI].subDatas.next_direction = DIRECTION_RIGHT;
		updateMapAI(x_headAI, y_headAI, AI_STATUS_ON_THE_WAY);
		mapAI[x_headAI+1][y_headAI].subDatas.prev_direction = DIRECTION_LEFT;
		x_headAI++;
		stepsFromHeadAI++;
	}
#endif

u8 snake_moveComm(u8 direction){
	// ��ͼ�ƶ�����ķ������ƶ����x���ƶ����y
	u8 directionBackward=0, x_next=0, y_next=0;
	switch (direction)
	{
	case DIRECTION_UP:
		directionBackward = DIRECTION_DOWN;
		x_next = x_head;
		y_next = y_head-1;
		break;
	case DIRECTION_DOWN:
		directionBackward = DIRECTION_UP;
		x_next = x_head;
		y_next = y_head+1;
		break;
	case DIRECTION_LEFT:
		directionBackward = DIRECTION_RIGHT;
		x_next = x_head-1;
		y_next = y_head;
		break;
	case DIRECTION_RIGHT:
		directionBackward = DIRECTION_LEFT;
		x_next = x_head+1;
		y_next = y_head;
		break;
	default:
		break;
	}

	if (gameStatus == GAME_STS_PLAY)
	{
		// ������Ч
		if (nowTowards == directionBackward){
			return 0;
		}

		if (canMoveComm(direction, x_head, y_head, 0)) 
		{
			// ǰ���ǿյػ�����β�Ͳ��ƶ�β�ͣ������ʳ�����ƶ�β�ͣ�Ҳ����������䳤
			if (map[x_next][y_next] != STS_APPLE) {
				moveEndBlock();

				#ifdef AI_LEVEL_MASTER
					// ÿ��һ���Ͳ�����һ
					reCreateAppleStep++;
					if (reCreateAppleStep > RECREATE_APPLE_STEPS)
					{
						reCreateAppleStep = 0;

						// ���Ѿ����˺ܶಽ�ˣ�����Ȼ���ܳԵ�ƻ������������һ���µ�ƻ��
						SNAKE_reCreateFood();
						// ÿ��������һ��ƻ���ͼ���һ��
						reCreateAppleCnt++;
						if (reCreateAppleCnt > RE_CREATE_APPLE_CNT)
						{
							// ��������ƻ����Σ�AI��Ȼ�Բ���ƻ������Ϊ�޷�������Ϸ��ǿ��gameover
							reCreateAppleCnt = 0;
							gameover();
						}
					}
					
				#endif
			} else {

				#ifdef AI_LEVEL_MASTER
					// ÿ�Ե�һ��ƻ���ͽ��ظ�����ƻ���Ĵ������ߵĲ����������¼���
					reCreateAppleCnt=0;
					reCreateAppleStep=0;
				#endif

				score++;
				createfood();

				fireEvent(SNAKE_EVENT_EAT_APPLE, score, 0);
			}

			// ԭ����ͷ������壬��ǰһ��ָ���µ�ͷ����ʮλ�����ƣ�Ҳ���Ƿ������10��
			// TODO:���sprintfһ��ȥ���ͳ�����ֵ����⣬��ʱ�ȱ�����
			sprintf(buff,"dummy");
			updateMap(x_head, y_head, (direction * 10) + map[x_head][y_head] % 10);

			// �����µ�ͷ��������ͷλ��
			updateMap(x_next, y_next, STS_HEAD + directionBackward);
			x_head = x_next;
			y_head = y_next;

			// �޸ĵ�ǰ����
			nowTowards = direction;
		} else {
			gameover();
		}
	}

	return 1;
}

u8 SNAKE_moveUp(){
	return snake_moveComm(DIRECTION_UP);
}

u8 SNAKE_moveDown(){
	return snake_moveComm(DIRECTION_DOWN);
}

u8 SNAKE_moveLeft(){
	return snake_moveComm(DIRECTION_LEFT);
}

u8 SNAKE_moveRight(){
	return snake_moveComm(DIRECTION_RIGHT);
}

u8 SNAKE_turnLeft(void){
	switch (nowTowards)
	{
	case NOW_TOWARDS_U:
		return SNAKE_moveLeft();
		break;
	case NOW_TOWARDS_D:
		return SNAKE_moveRight();
		break;
	case NOW_TOWARDS_L:
		return SNAKE_moveDown();
		break;
	case NOW_TOWARDS_R:
		return SNAKE_moveUp();
		break;
	default:
		return 0;
		break;
	}
}

u8 SNAKE_turnRight(void){
	switch (nowTowards)
	{
	case NOW_TOWARDS_U:
		return SNAKE_moveRight();
		break;
	case NOW_TOWARDS_D:
		return SNAKE_moveLeft();
		break;
	case NOW_TOWARDS_L:
		return SNAKE_moveUp();
		break;
	case NOW_TOWARDS_R:
		return SNAKE_moveDown();
		break;
	default:
		return 0;
		break;
	}
}

void moveEndBlock(){
	u8 xTmp = x_end;
	u8 yTmp = y_end;

	// ȡ��λ�����õ���βǰһ��block�ķ���
	switch (map[x_end][y_end] % 10) {
		case STS_NEXT_TO_U:
			y_end--;
			break;
		case STS_NEXT_TO_D:
			y_end++;
			break;
		case STS_NEXT_TO_L:
			x_end--;
			break;
		case STS_NEXT_TO_R:
			x_end++;
			break;
	}

	// ԭ������β���
	updateMap(xTmp, yTmp, STS_EMPTY);

	// ԭ������������β(���ڵ�block������Ϣ�ĳ������ʮλ��(ǰһ��block����))
	updateMap(x_end, y_end, STS_TAIL + map[x_end][y_end] / 10);

}

void SNAKE_moveNext(){
	if (gameStatus == GAME_STS_PLAY)
	{
		snake_moveComm(nowTowards);
	}
}

#if defined (AI_LEVEL_SMART) || defined(AI_LEVEL_MASTER)
// ������ƻ����ʱ�������һ��
void wander(u8 exceptRirection){
	u8 canMoveUpValue, canMoveLeftValue, canMoveRightValue, canMoveDownValue;
	u8 directionRnd = RAND_FUNC() % 4 + 1;

	canMoveUpValue = canMoveUp(x_head, y_head, 0);
	canMoveDownValue = canMoveDown(x_head, y_head, 0);
	canMoveLeftValue = canMoveLeft(x_head, y_head, 0);
	canMoveRightValue = canMoveRight(x_head, y_head, 0);

	// �����·������gameover
	if (!(canMoveUpValue + canMoveDownValue + canMoveLeftValue + canMoveRightValue))
	{
		gameover();
	}
	// ���ֻʣһ������������ߣ���Ȼ����ѭ��
	else if (canMoveUpValue && !(canMoveDownValue + canMoveLeftValue + canMoveRightValue))
	{
		SNAKE_moveUp();
	}
	else if (canMoveDownValue && !(canMoveUpValue + canMoveLeftValue + canMoveRightValue))
	{
		SNAKE_moveDown();
	}
	else if (canMoveLeftValue && !(canMoveUpValue + canMoveDownValue + canMoveRightValue))
	{
		SNAKE_moveLeft();
	}
	else if (canMoveRightValue && !(canMoveUpValue + canMoveDownValue + canMoveLeftValue))
	{
		SNAKE_moveRight();
	}
	// ���������Ϸ�����ߣ������һ��������������Ż���̫���������Լ�����Ҫ�в��Ե������
	else{
		while(directionRnd == exceptRirection || canMoveComm(directionRnd, x_head, y_head, 0) == 0){
		directionRnd = RAND_FUNC() % 4 + 1;
		}
		snake_moveComm(directionRnd);
	}
}
#endif

#ifdef AI_LEVEL_MASTER
u8 canGetTail(u8 x_from, u8 y_from){
	clearMapAI();

	x_headAI = x_from;
	y_headAI = y_from;
	while(1){
		// ��� x��ǰ�� == x��β
		if (x_headAI == x_end && y_headAI == y_end)
		{
			// �����ҵ���β������Ѱ��
			return 1;
		} else {
			if (canMoveCommAI(DIRECTION_LEFT)){
				doMoveLeftAI();
			} else if (canMoveCommAI(DIRECTION_UP)) {
				doMoveUpAI();
			} else if (canMoveCommAI(DIRECTION_RIGHT)) {
				doMoveRightAI();
			} else if (canMoveCommAI(DIRECTION_DOWN)) {
				doMoveDownAI();
			} else {
				if (x_headAI == x_from && y_headAI == y_from)
				{
					// ��ǰ����ʼ�㣬���ʾ̽·����һȦû�ҵ��˻�ԭ���ˣ�û��ͨ����β��·
					return 0;
				}else{
					// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
					markDeadwayAndGoBackToPrevAI();
				}
			}
		}
	}
}

u8 canGetApple(u8 x_from, u8 y_from){
	clearMapAI();

	x_headAI = x_from;
	y_headAI = y_from;
	while(1){
		// ��� x��ǰ�� == xƻ��
		if (x_headAI == x_apple && y_headAI == y_apple)
		{
			// �����ҵ�ƻ��������Ѱ��
			return 1;
		} else {
			if (canMoveCommAI(DIRECTION_LEFT)){
				doMoveLeftAI();
			} else if (canMoveCommAI(DIRECTION_UP)) {
				doMoveUpAI();
			} else if (canMoveCommAI(DIRECTION_RIGHT)) {
				doMoveRightAI();
			} else if (canMoveCommAI(DIRECTION_DOWN)) {
				doMoveDownAI();
			} else {
				if (x_headAI == x_from && y_headAI == y_from)
				{
					// ��ǰ����ʼ�㣬���ʾ̽·����һȦû�ҵ��˻�ԭ���ˣ�û��ͨ����β��·
					return 0;
				}else{
					// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
					markDeadwayAndGoBackToPrevAI();
				}
			}
		}
	}
}
#endif

// �������Χ�Ŀյط�����һ��
void SNAKE_AI_moveNext(){
	u8 towards;
	u8 isSafe;
	// ������ɷ��ƶ�(0:���ɣ�1:��)
	u8 canMoveUpValue, canMoveDownValue, canMoveLeftValue, canMoveRightValue;

	#ifdef AI_LEVEL_MASTER
	// �������ƶ�һ�����ܷ��ҵ���ͷ(0:���ɣ�1:��)
	//u8 canGetTailUpValue, canGetTailDownValue, canGetTailLeftValue, canGetTailRightValue;
	// �������ƶ�һ�����ܷ��ҵ�ƻ��(0:���ɣ�1:��)
	//u8 canGetAppleUpValue, canGetAppleDownValue, canGetAppleLeftValue, canGetAppleRightValue;
	#endif

	if (gameStatus == GAME_STS_PLAY)
	{
		//�ȼ��Ŀǰ3�������л���û�пյأ������û�о�gameover��,���������ѭ��
		canMoveUpValue = canMoveUp(x_head, y_head, 0);
		canMoveDownValue = canMoveDown(x_head, y_head, 0);
		canMoveLeftValue = canMoveLeft(x_head, y_head, 0);
		canMoveRightValue = canMoveRight(x_head, y_head, 0);

		if( !canMoveUpValue && !canMoveDownValue && !canMoveLeftValue && !canMoveRightValue ) {
			gameover();
		}
		//��Χ�пյػ�ƻ����β��
		else
		{
			#if defined(AI_LEVEL_BABY) || defined(AI_LEVEL_KIDS) || defined(AI_LEVEL_SMART)
				//����Ա���ƻ�����ȳ�ƻ��(Ҳ������˹ҵ�)
				if(x_head+1 < SNAKE_SIZE_X && map[x_head+1][y_head] == STS_APPLE){
					SNAKE_moveRight();
					return;
				}
				else if(x_head !=0 && map[x_head-1][y_head] == STS_APPLE){
					SNAKE_moveLeft();
					return;
				}
				else if(y_head+1 < SNAKE_SIZE_Y && map[x_head][y_head+1] == STS_APPLE){
					SNAKE_moveDown();
					return;
				}
				else if(y_head != 0 && map[x_head][y_head-1] == STS_APPLE){
					SNAKE_moveUp();
					return;
				}
			#endif

			// ����(ֻ���������)
			#ifdef AI_LEVEL_BABY
				do{
					if(RAND_FUNC() % 2 == 0 && canMoveRightValue) {
						SNAKE_moveRight();
						return;
					} else if(RAND_FUNC() % 2 == 0 && canMoveLeftValue) {
						SNAKE_moveLeft();
						return;
					} else if(RAND_FUNC() % 2 == 0 && canMoveDownValue) {
						SNAKE_moveDown();
						return;
					} else if(RAND_FUNC() % 2 == 0 && canMoveUpValue) {
						SNAKE_moveUp();
						return;
					}
				} while(1);
			#endif

			// ��ͯ(���Ԥ��2��)
			#ifdef AI_LEVEL_KIDS
				// �ֱ�ȡ��һ������λ�õ����ɶȣ��߹�ȥ�Ժ�����һ���м���������ߣ�
				if (canMoveUpValue)
				{
					freeUp = ai_kids_getFree(x_head, y_head-1);
				} else {
					freeUp = 0;
				}
				if (canMoveDownValue)
				{
					freeDown = ai_kids_getFree(x_head, y_head+1);
				} else {
					freeDown = 0;
				}
				if (canMoveLeftValue)
				{
					freeLeft = ai_kids_getFree(x_head-1, y_head);
				} else {
					freeLeft = 0;
				}
				if (canMoveRightValue)
				{
					freeRight = ai_kids_getFree(x_head+1, y_head);
				} else {
					freeRight = 0;
				}
				
				// �����ɶ����ķ�������
				do{
					if (canMoveUpValue && RAND_FUNC() % 4 == 0 && freeUp >= freeDown && freeUp >= freeLeft && freeUp >= freeRight) {
						SNAKE_moveUp();
						return;
					} else if (canMoveDownValue && RAND_FUNC() % 4 == 0 && freeDown >= freeUp && freeDown >= freeLeft && freeDown >= freeRight) {
						SNAKE_moveDown();
						return;
					} else if (canMoveLeftValue && RAND_FUNC() % 4 == 0 && freeLeft >= freeUp && freeLeft >= freeDown && freeLeft >= freeRight) {
						SNAKE_moveLeft();
						return;
					} else if (canMoveRightValue && RAND_FUNC() % 4 == 0 && freeRight >= freeUp && freeRight >= freeDown && freeRight >= freeLeft) {
						SNAKE_moveRight();
						return;
					}
				}while(1);
			#endif

			// ����(��Ѱ·ƻ��)
			#ifdef AI_LEVEL_SMART
				x_headAI = x_head;
				y_headAI = y_head;

				#ifdef SNAKE_AI_DEBUG
				if (score >= 36)
				{
					score = score;
				}
				#endif

				clearMapAI();
				while(1){
					// 2-�������ȼ�����̽������
					// ��� x��ǰ�� == xƻ��
					if (x_headAI == x_apple && y_headAI == y_apple)
					{
						// �ҵ�ƻ���ˣ����á���һ���ڵ�ķ�����Ϣ���λ��ݣ�ͬʱ��·���apple�����ʼ��ͷ��λ��
						while(markApplewayAndGoBackToPrevAI()){
							;
						}
						// Ѱ·����,��һ��,�˳���ѭ��(TODO:Ҳ����һ��������ȫ��)
						snake_moveComm(mapAI[x_headAI][y_headAI].subDatas.next_direction);
						
						break;
					} else if (x_headAI > x_apple && y_headAI > y_apple) {
						// ��ǰ����ƻ�������·����������ȼ�:��������
						if (canMoveCommAI(DIRECTION_LEFT)){
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)) {
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI > x_apple && y_headAI < y_apple) {
						// ��ǰ����ƻ�������Ϸ����������ȼ�:��������
						if (canMoveCommAI(DIRECTION_LEFT)){
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)) {
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI < y_apple) {
						// ��ǰ����ƻ�������Ϸ����������ȼ�:��������
						if (canMoveCommAI(DIRECTION_RIGHT)){
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)) {
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI > y_apple) {
						// ��ǰ����ƻ�������·����������ȼ�:��������
						if (canMoveCommAI(DIRECTION_RIGHT)){
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)) {
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI == x_apple && y_headAI > y_apple) {
						// ��ǰ����ƻ�������·����������ȼ�:��������
						if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)){
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)) {
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI == x_apple && y_headAI < y_apple) {
						// ��ǰ����ƻ�������Ϸ����������ȼ�:��������
						if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)){
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)) {
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI == y_apple) {
						// ��ǰ����ƻ�������󷽣��������ȼ�:��������
						if (canMoveCommAI(DIRECTION_RIGHT)){
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)) {
							doMoveLeftAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI > x_apple && y_headAI == y_apple) {
						// ��ǰ����ƻ�������ҷ����������ȼ�:��������
						if (canMoveCommAI(DIRECTION_LEFT)) {
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)){
							doMoveRightAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					}
				}
			#endif

			// ��ʦ(�����ȱ�������ͬ���Ѱ·ƻ��)
			#ifdef AI_LEVEL_MASTER

				#ifdef SNAKE_AI_DEBUG
				if (score >= 36)
				{
					score = score;
				}
				#endif

				// 4�������ƶ�һ�����ܷ��ҵ���β��ƻ��
				// TODO : û��Ҫȫ����ĳ���������ҵ���β��ƻ������Ҫ�����������ˣ���ʡ������
				// canGetTailUpValue = 0;		canGetAppleUpValue = 0;
				// canGetTailDownValue = 0;	    canGetAppleDownValue = 0;
				// canGetTailLeftValue = 0;	    canGetAppleLeftValue = 0;
				// canGetTailRightValue = 0;	canGetAppleRightValue = 0;

				// if (canMoveUpValue) {
				// 	canGetTailUpValue = canGetTail(x_head, y_head-1);
				// 	canGetAppleUpValue = canGetApple(x_head, y_head-1);
				// }
				// if (canMoveDownValue) {
				// 	canGetTailDownValue = canGetTail(x_head, y_head+1);
				// 	canGetAppleDownValue = canGetApple(x_head, y_head+1);
				// }
				// if (canMoveLeftValue) {
				// 	canGetTailLeftValue = canGetTail(x_head-1, y_head);
				// 	canGetAppleLeftValue = canGetApple(x_head-1, y_head);
				// }
				// if (canMoveRightValue) {
				// 	canGetTailRightValue = canGetTail(x_head+1, y_head);
				// 	canGetAppleRightValue = canGetApple(x_head+1, y_head);
				// }

				// // ������ ���� �ҵ���β Ҳ�� �ҵ�ƻ�� �ķ���
				// if (canGetTailUpValue & canGetAppleUpValue) {
				// 	canGetApple(x_head, y_head-1);
				// }
				// else if (canGetTailDownValue & canGetAppleDownValue){
				// 	canGetApple(x_head, y_head+1);
				// }
				// else if (canGetTailLeftValue & canGetAppleLeftValue){
				// 	canGetApple(x_head-1, y_head);
				// }
				// else if (canGetTailRightValue & canGetAppleRightValue){
				// 	canGetApple(x_head+1, y_head);
				// }

				// // ����� ֻ�� �ҵ���β �ķ���TODO Ӧ���Ż�������ͨ����β���������·����ôһ��·�������ܿճ��ռ䣩
				// else if (canGetTailUpValue) {
				// 	SNAKE_moveUp();
				// }
				// else if (canGetTailDownValue){
				// 	SNAKE_moveDown();
				// }
				// else if (canGetTailLeftValue){
				// 	SNAKE_moveLeft();
				// }
				// else if (canGetTailRightValue){
				// 	SNAKE_moveRight();
				// }

				// // �ٴ��� ֻ�� �ҵ�ƻ���ķ���
				// else if (canGetAppleUpValue) {
				// 	SNAKE_moveUp();
				// }
				// else if (canGetAppleDownValue){
				// 	SNAKE_moveDown();
				// }
				// else if (canGetAppleLeftValue){
				// 	SNAKE_moveLeft();
				// }
				// else if (canGetAppleRightValue){
				// 	SNAKE_moveRight();
				// }

				// // �����������β��ƻ������������ֻ�����ɢ����(TODO ɢ���Ĳ���Ҳ�н���)
				// else {
				// 	wander(0);
				// }

				// ��һ������Ч��AI�߼�������������ʧ�ܿ��Իָ�������һ��
				x_headAI = x_head;
				y_headAI = y_head;
				clearMapAI();
				while(1){
					// 2-�������ȼ�����̽������
					// ��� x��ǰ�� == xƻ��
					if (x_headAI == x_apple && y_headAI == y_apple)
					{
						// �ҵ�ƻ���ˣ����á���һ���ڵ�ķ�����Ϣ���λ��ݣ�ͬʱ��·���apple�����ʼ��ͷ��λ��
						while(markApplewayAndGoBackToPrevAI()){
							;
						}
						// Ѱ·����
						// ��ֹΪ�˳�ƻ������·���������һ���жϣ������������һ������������ҵ���β��������
						isSafe = 0;
						towards = mapAI[x_headAI][y_headAI].subDatas.next_direction;
						switch (towards)
						{
						case DIRECTION_UP:
							isSafe = canGetTail(x_head, y_head-1);
							break;
						case DIRECTION_DOWN:
							isSafe = canGetTail(x_head, y_head+1);
							break;
						case DIRECTION_LEFT:
							isSafe = canGetTail(x_head-1, y_head);
							break;
						case DIRECTION_RIGHT:
							isSafe = canGetTail(x_head+1, y_head);
							break;
						default:
							break;
						}

						if (isSafe)
						{
							snake_moveComm(towards);
						} else {
							wander(towards);
						}

						// // ��һ��,�˳���ѭ��(TODO:Ҳ����һ��������ȫ��)
						// snake_moveComm(mapAI[x_headAI][y_headAI].subDatas.next_direction);
						
						break;
					} else if (x_headAI > x_apple && y_headAI > y_apple) {
						// ��ǰ����ƻ�������·����������ȼ�:��������
						if (canMoveCommAI(DIRECTION_LEFT)){
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)) {
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI > x_apple && y_headAI < y_apple) {
						// ��ǰ����ƻ�������Ϸ����������ȼ�:��������
						if (canMoveCommAI(DIRECTION_LEFT)){
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)) {
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI < y_apple) {
						// ��ǰ����ƻ�������Ϸ����������ȼ�:��������
						if (canMoveCommAI(DIRECTION_RIGHT)){
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)) {
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI > y_apple) {
						// ��ǰ����ƻ�������·����������ȼ�:��������
						if (canMoveCommAI(DIRECTION_RIGHT)){
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)) {
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI == x_apple && y_headAI > y_apple) {
						// ��ǰ����ƻ�������·����������ȼ�:��������
						if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)){
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)) {
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI == x_apple && y_headAI < y_apple) {
						// ��ǰ����ƻ�������Ϸ����������ȼ�:��������
						if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)){
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)) {
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI == y_apple) {
						// ��ǰ����ƻ�������󷽣��������ȼ�:��������
						if (canMoveCommAI(DIRECTION_RIGHT)){
							doMoveRightAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_LEFT)) {
							doMoveLeftAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI > x_apple && y_headAI == y_apple) {
						// ��ǰ����ƻ�������ҷ����������ȼ�:��������
						if (canMoveCommAI(DIRECTION_LEFT)) {
							doMoveLeftAI();
						} else if (canMoveCommAI(DIRECTION_UP)) {
							doMoveUpAI();
						} else if (canMoveCommAI(DIRECTION_DOWN)) {
							doMoveDownAI();
						} else if (canMoveCommAI(DIRECTION_RIGHT)){
							doMoveRightAI();
						} else {
							if (x_headAI == x_head && y_headAI == y_head)
							{
								// ��ǰ����ͷ�����ʾû��ͨ��ƻ����·
								// ֻ�������һ��, Ѱ·����, �˳���ѭ��
								wander(0);
								break;
							}else{
								// ��ǰ���ĸ����򶼲�ͨ�����Ϊ���㲢�ص���һ���ڵ�
								markDeadwayAndGoBackToPrevAI();
							}
						}
					}
				}
			#endif
		}
	}
}

void SNAKE_reCreateFood(){
	updateMap(x_apple, y_apple, STS_EMPTY);
	createfood();
}

// �����鵼���ͼ(��ˢ�»��棬���������ڴ�)
void SNAKE_loadWall(u8* wallMap){

	SNAKE_clearWall();
	for (_y = 0; _y < SNAKE_SIZE_Y; _y++)
	{
		for (_x = 0; _x < SNAKE_SIZE_X; _x++)
		{
			_tmp = _x/8;
			_idx = (SNAKE_SIZE_X/8)*_y + _tmp;
			_tmp = wallMap[_idx];
			if(map[_x][_y] == STS_EMPTY && ((_tmp<<(_x%8)) & 0x80) ) {
				// ���������ڴ�
				//updateMap(_x, _y, STS_WALL); 
				map[_x][_y] = STS_WALL;
			}
		}
	}
}

// ����һ���ϰ���
void SNAKE_addWall(u8 x, u8 y){
	if (map[x][y] == STS_EMPTY) {
		updateMap(x, y, STS_WALL);
	}
}

// ���һ���ϰ���
void SNAKE_delWall(u8 x, u8 y){
	if (map[x][y] == STS_WALL) {
		updateMap(x, y, STS_EMPTY);
	}
}

// ��������ϰ���(��ˢ�»���)
void SNAKE_clearWall(){
	u8 x, y;
	for (x = 0; x < SNAKE_SIZE_X; ++x)
	{
		for (y = 0; y < SNAKE_SIZE_Y; ++y)
		{
			if(map[x][y] == STS_WALL) map[x][y] = STS_EMPTY;
		}
	}
}

u8 isFull(){
	if (score + 3 >= SNAKE_SIZE_X * SNAKE_SIZE_Y)
	{
		return 1;
	}

	return 0;
}

// ����ʳ��
void createfood()
{
	if(isFull()){
		gameover();
		return;
	}

	do
	{
		x_apple = RAND_FUNC() % SNAKE_SIZE_X;
		y_apple = RAND_FUNC() % SNAKE_SIZE_Y;
	} while(map[x_apple][y_apple] != STS_EMPTY);

	updateMap(x_apple, y_apple, STS_APPLE);
}

void SNAKE_moveAppleUp(){
	if(y_apple-1 >= 0 && map[x_apple][y_apple-1] == STS_EMPTY) {
		updateMap(x_apple, y_apple, STS_EMPTY);
		updateMap(x_apple, y_apple-1, STS_APPLE);
		y_apple--;
	}
}

void SNAKE_moveAppleDown(){
	if(y_apple+1 < SNAKE_SIZE_Y && map[x_apple][y_apple+1] == STS_EMPTY) {
		updateMap(x_apple, y_apple, STS_EMPTY);
		updateMap(x_apple, y_apple+1, STS_APPLE);
		y_apple++;
	}
}

void SNAKE_moveAppleLeft(){
	if(x_apple-1 >= 0 && map[x_apple-1][y_apple] == STS_EMPTY) {
		updateMap(x_apple, y_apple, STS_EMPTY);
		updateMap(x_apple-1, y_apple, STS_APPLE);
		x_apple--;
	}
}

void SNAKE_moveAppleRight(){
	if(x_apple+1 < SNAKE_SIZE_X && map[x_apple+1][y_apple] == STS_EMPTY) {
		updateMap(x_apple, y_apple, STS_EMPTY);
		updateMap(x_apple+1, y_apple, STS_APPLE);
		x_apple++;
	}
}

u8 SNAKE_getMapData(u8 x, u8 y){
	return map[x][y];
}

#ifdef SNAKE_AI_DEBUG
union dfsMapType SNAKE_getPointDataAI(u8 x, u8 y){
	return mapAI[x][y];
}
#endif

u16 SNAKE_getNowScroe(){
	return score;
}
