/******************************************************************************************************
 * 扫雷模块
 * File  : embMine.c
 * Author: MANDA (slipperstree@gmail.com)
 * Date  : 2021.08
 * Updt  : 2021.08
 * Func  : 实现了扫雷的基本数据模型，
 *         但不包含显示逻辑，只包含游戏状态和地图状态管理，与硬件无关可直接移植到各种硬件平台上
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

// 临时变量用(sprintf等)
extern char buff[128];

// 调试模式
//#define SNAKE_AI_DEBUG

// 游戏状态
#define GAME_STS_PLAY 		0	// 游戏中
#define GAME_STS_GAMEOVER	1	// 游戏结束
#define GAME_STS_SETTING	2	// 设置
#define GAME_STS_PAUSE		3	// 暂停

// 地图二维数组取值意义，用于外部显示模块绘图用
#define STS_EMPTY 0				// 表示该点为空
#define STS_APPLE 1		        // 表示该点为食物

#define DIRECTION_UP 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3
#define DIRECTION_RIGHT 4

#define STS_NEXT_TO_U 1
#define STS_NEXT_TO_D 2
#define STS_NEXT_TO_L 3
#define STS_NEXT_TO_R 4

// 当前朝向，用于自动前进
#define NOW_TOWARDS_U 1	
#define NOW_TOWARDS_D 2		
#define NOW_TOWARDS_L 3		
#define NOW_TOWARDS_R 4

// 蛇身体的定义
// 蛇头：蛇头50+连接蛇头的蛇身方向(1-4)
// 蛇身：蛇身（10/20/30/40）+后面一块蛇身的方向(1-4)
// 蛇尾：蛇尾60+连接蛇尾的蛇身方向(1-4)
//     十位数 : 蛇头，蛇身（包含4个方向信息），蛇尾
//     个位数 : 相邻block的方向
//     如此细分是为了方便外面的绘图模块绘制更好看转弯的身体更连续的蛇，
//     对显示效果要求不高也可以简单取十位数为1-6即可

// 这个十位数的1-4跟其他的方向常量的1-4要保持一致，其他的改成6789的话这里也要改，而且不能跟蛇头蛇尾冲突
#define STS_BODY_PREV_U 10
#define STS_BODY_PREV_D 20
#define STS_BODY_PREV_L 30
#define STS_BODY_PREV_R 40
// 蛇头
#define STS_HEAD 50
// 蛇尾
#define STS_TAIL 60
// 表示该点为障碍物（效果跟身体一样，只是为了以后扩展程序预留）
#define STS_WALL 90

// SNAKE_AI_moveNext 的AI算法写了好几个实现，越简单rom空间越小，根据需要放开下列4个定义中的某一个（不可同时放开）
//#define AI_LEVEL_BABY     // 娃娃(只会随机乱走，测试最好成绩11)
//#define AI_LEVEL_KIDS     // 儿童(会多预判2步，测试最好成绩32)
//#define AI_LEVEL_SMART    // 聪明(会寻路找到苹果，测试最好成绩65，平均34) // Realse+优化开到最大：rom size<8k
#define AI_LEVEL_MASTER     // 天才(会寻路找到苹果，并且同时避免进入死胡同，测试最好成绩)

#if defined (AI_LEVEL_SMART) || defined(AI_LEVEL_MASTER)
    #define AI_STATUS_EMPTY 0
    #define AI_STATUS_ON_THE_WAY 1
    #define AI_STATUS_WAY_TO_DEATH 2
    #define AI_STATUS_WAY_TO_APPLE 3
    // DFS深度优先探索地图使用的『影子地图』数据类型定义
    // 每一个数据8位
    // bit[2:0]保存下一个路径的方向（1-4）其实4个方向2位就够了，不过其他代码(STS_BODY_PREV_X)也要改就先1-4了
    // bit[5:3]保存上一个路径的方向（1-4）
    // bit[6:7]
    //    0:未探索  1:在路上（正在探索）  2:已探索且是死路  3:已探索且通向苹果
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

// 调用者从init函数传进来的回调函数指针
pSnakeEventFunc callbackSnakeEventFunc;
u16 SnakeEventId, SnakeEventParam1, SnakeEventParam2;

// 游戏状态
u8 gameStatus;

// 地图数据
u8 map[SNAKE_SIZE_X][SNAKE_SIZE_Y];

#if defined (AI_LEVEL_SMART) || defined(AI_LEVEL_MASTER)
	// 寻路二维数组
	union dfsMapType mapAI[SNAKE_SIZE_X][SNAKE_SIZE_Y];
	s8 x_headAI, y_headAI, stepsFromHeadAI;
	void wander(u8); // 散步
#endif

#ifdef AI_LEVEL_MASTER
	// 避免苹果生成在某个地方导致死循环，如果蛇走了多少步以后还没有吃到苹果则重新生成一次
	#define RECREATE_APPLE_STEPS (SNAKE_SIZE_X*SNAKE_SIZE_Y*2)
	u16 reCreateAppleStep;

	// 如果重新生成好多次苹果还是没吃到过那么认为已经吃不到了，放弃这一局自杀
	#define RE_CREATE_APPLE_CNT 10
	u8 reCreateAppleCnt;
#endif

// 蛇头的位置
u8 x_head, y_head;

// 蛇尾的位置
u8 x_end, y_end;

// 食物的位置
u8 x_apple, y_apple;

// 当前朝向
u8 nowTowards;

// 分数
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
	// 各方向上的自由度
	u8 freeUp,freeDown,freeLeft,freeRight = 0;

	// 取绝对值
	u8 myAbs(u8 a, u8 b){
		if(a>=b)
			return a-b;
		else
			return b-a;
	}
	
	// 取某个位置的自由度（周围可走的方向数）
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

// 获取指定位置的视图内容（视图内容不完全等同地图原始内容，比如虽然都是身体，但有拐弯的有直的，拐弯的方向也是不一样的）
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

// 所有需要更新map数据的时候都调用这个函数，保证能及时通知控制端更新响应的画面
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

// 重启游戏(不会清除地图中的墙壁信息)
void SNAKE_restart(){
	u8 x, y;
	// 重启游戏不会清除地图中的墙壁信息
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

	// 事件参数没有使用
	fireEvent(SNAKE_EVENT_RESTART, 0, 0);

	// 初始分数
	score = 0;

	gameStatus = GAME_STS_PLAY;

	// 初始block 尾巴-身体-头 ->
	x_head = 2, y_head = 0;
	x_end = 0, y_end = 0;

	updateMap(x_end, y_end, STS_TAIL + STS_NEXT_TO_R);
	updateMap(1, 0, STS_BODY_PREV_R + STS_NEXT_TO_L);
	updateMap(x_head, y_head, STS_HEAD + STS_NEXT_TO_L);
	nowTowards = DIRECTION_RIGHT;

	createfood();
}

// 模块初始化,只应该被调用一次,init调用完以后就可以加入障碍物了
void SNAKE_init(pSnakeEventFunc pCallbackSnakeEventFunc){
	u8 x, y;

	callbackSnakeEventFunc = pCallbackSnakeEventFunc;

	// 防止内存中有垃圾数据，初始化所有地图为空(不通知外部刷新画面)
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

// 当前状态下N步以后指定坐标是否为蛇尾
// 这是为了判断当蛇头走过N步数以后来到这个点的时候某个方向上本来是蛇身，
//   走了N步以后有可能变成蛇尾，那么这个方向的自由度也有效
//   如果不考虑这一点那么自由度就是2（左，上），其实应该是3（左，上，右）
//   当蛇头走一步到A点时，蛇尾也移动了一步，于是A点的右侧变成了蛇尾，A点的蛇头是允许转向右侧的（不会撞到蛇尾）
//   比如下图，-为蛇尾，@为蛇头，+为蛇身，现在要判断A点的自由度，
//    -
//   A+	
//   @+	
//   ++    
//   蛇头向上走到A点后变成下面的样子，右侧变成蛇尾，可走的通
//     
//   @-
//   ++
//   ++
//   再向右走，没有问题
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

    //先直接判断当前这个点是不是就是蛇尾，如果是就不需要走N步再判断了
	if (tmp_tail_x == x && tmp_tail_y == y)
	{
		return 1;
	}

	// 让蛇尾沿着蛇身走相差的步数
	for (step = 0; step < afterSteps; step++)
	{
		if(step == 0){
			// 第一次是蛇尾，方向从个位数取
			tailNextTo = map[tmp_tail_x][tmp_tail_y] % 10;
		} else {
			// 蛇身，方向从十位数取（个位数是后面连接的方向）
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

		// 只要在途中蛇尾经过这个点就表示这个点是安全的
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

// x,y : 评估对象位置
// stepsFromHead : 从蛇头走到x,y已经走过了多少步（为了预估蛇尾的位置）
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

		// 回到上个节点
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
			// 当前是蛇头，则表示回溯结束
			return 0;
		} else {
			// 回到上个节点
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
	// 试图移动方向的反方向，移动后的x，移动后的y
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
		// 后退无效
		if (nowTowards == directionBackward){
			return 0;
		}

		if (canMoveComm(direction, x_head, y_head, 0)) 
		{
			// 前方是空地或者是尾巴才移动尾巴，如果是食物则不移动尾巴，也就是让身体变长
			if (map[x_next][y_next] != STS_APPLE) {
				moveEndBlock();

				#ifdef AI_LEVEL_MASTER
					// 每走一步就步数加一
					reCreateAppleStep++;
					if (reCreateAppleStep > RECREATE_APPLE_STEPS)
					{
						reCreateAppleStep = 0;

						// 蛇已经走了很多步了，但仍然不能吃到苹果就重新生成一个新的苹果
						SNAKE_reCreateFood();
						// 每重新生成一次苹果就计数一次
						reCreateAppleCnt++;
						if (reCreateAppleCnt > RE_CREATE_APPLE_CNT)
						{
							// 重新生成苹果多次，AI仍然吃不到苹果就认为无法继续游戏，强制gameover
							reCreateAppleCnt = 0;
							gameover();
						}
					}
					
				#endif
			} else {

				#ifdef AI_LEVEL_MASTER
					// 每吃到一次苹果就将重复生成苹果的次数和走的步数清零重新计数
					reCreateAppleCnt=0;
					reCreateAppleStep=0;
				#endif

				score++;
				createfood();

				fireEvent(SNAKE_EVENT_EAT_APPLE, score, 0);
			}

			// 原来的头变成身体，且前一块指向新的头部（十位数控制，也就是方向乘以10）
			// TODO:这个sprintf一旦去掉就出现奇怪的问题，暂时先保留。
			sprintf(buff,"dummy");
			updateMap(x_head, y_head, (direction * 10) + map[x_head][y_head] % 10);

			// 设置新的头，并更新头位置
			updateMap(x_next, y_next, STS_HEAD + directionBackward);
			x_head = x_next;
			y_head = y_next;

			// 修改当前朝向
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

	// 取个位数，得到蛇尾前一块block的方向
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

	// 原来的蛇尾清空
	updateMap(xTmp, yTmp, STS_EMPTY);

	// 原来的蛇身变成蛇尾(相邻的block方向信息改成蛇身的十位数(前一个block方向))
	updateMap(x_end, y_end, STS_TAIL + map[x_end][y_end] / 10);

}

void SNAKE_moveNext(){
	if (gameStatus == GAME_STS_PLAY)
	{
		snake_moveComm(nowTowards);
	}
}

#if defined (AI_LEVEL_SMART) || defined(AI_LEVEL_MASTER)
// 看不到苹果的时候，随便走一步
void wander(u8 exceptRirection){
	u8 canMoveUpValue, canMoveLeftValue, canMoveRightValue, canMoveDownValue;
	u8 directionRnd = RAND_FUNC() % 4 + 1;

	canMoveUpValue = canMoveUp(x_head, y_head, 0);
	canMoveDownValue = canMoveDown(x_head, y_head, 0);
	canMoveLeftValue = canMoveLeft(x_head, y_head, 0);
	canMoveRightValue = canMoveRight(x_head, y_head, 0);

	// 如果无路可走则gameover
	if (!(canMoveUpValue + canMoveDownValue + canMoveLeftValue + canMoveRightValue))
	{
		gameover();
	}
	// 如果只剩一个方向则必须走，不然会死循环
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
	// 有两个以上方向可走，随机走一步（这里可以再优化，太随机会搞死自己，需要有策略的随机）
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
		// 如果 x当前点 == x蛇尾
		if (x_headAI == x_end && y_headAI == y_end)
		{
			// 可以找到蛇尾，结束寻找
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
					// 当前是起始点，则表示探路找了一圈没找到退回原地了，没有通往蛇尾的路
					return 0;
				}else{
					// 当前点四个方向都不通，标记为死点并回到上一个节点
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
		// 如果 x当前点 == x苹果
		if (x_headAI == x_apple && y_headAI == y_apple)
		{
			// 可以找到苹果，结束寻找
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
					// 当前是起始点，则表示探路找了一圈没找到退回原地了，没有通往蛇尾的路
					return 0;
				}else{
					// 当前点四个方向都不通，标记为死点并回到上一个节点
					markDeadwayAndGoBackToPrevAI();
				}
			}
		}
	}
}
#endif

// 随机向周围的空地方向走一步
void SNAKE_AI_moveNext(){
	u8 towards;
	u8 isSafe;
	// 各方向可否移动(0:不可，1:可)
	u8 canMoveUpValue, canMoveDownValue, canMoveLeftValue, canMoveRightValue;

	#ifdef AI_LEVEL_MASTER
	// 各方向移动一步后能否找到蛇头(0:不可，1:可)
	//u8 canGetTailUpValue, canGetTailDownValue, canGetTailLeftValue, canGetTailRightValue;
	// 各方向移动一步后能否找到苹果(0:不可，1:可)
	//u8 canGetAppleUpValue, canGetAppleDownValue, canGetAppleLeftValue, canGetAppleRightValue;
	#endif

	if (gameStatus == GAME_STS_PLAY)
	{
		//先检查目前3个方向中还有没有空地，如果都没有就gameover了,否则会有死循环
		canMoveUpValue = canMoveUp(x_head, y_head, 0);
		canMoveDownValue = canMoveDown(x_head, y_head, 0);
		canMoveLeftValue = canMoveLeft(x_head, y_head, 0);
		canMoveRightValue = canMoveRight(x_head, y_head, 0);

		if( !canMoveUpValue && !canMoveDownValue && !canMoveLeftValue && !canMoveRightValue ) {
			gameover();
		}
		//周围有空地或苹果或尾巴
		else
		{
			#if defined(AI_LEVEL_BABY) || defined(AI_LEVEL_KIDS) || defined(AI_LEVEL_SMART)
				//如果旁边有苹果优先吃苹果(也可能因此挂掉)
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

			// 娃娃(只会随机乱走)
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

			// 儿童(会多预判2步)
			#ifdef AI_LEVEL_KIDS
				// 分别取下一步可走位置的自由度（走过去以后下下一步有几个方向可走）
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
				
				// 朝自由度最大的方向行走
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

			// 聪明(会寻路苹果)
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
					// 2-根据优先级决定探索方向
					// 如果 x当前点 == x苹果
					if (x_headAI == x_apple && y_headAI == y_apple)
					{
						// 找到苹果了，利用【上一个节点的方向】信息依次回溯（同时沿路标记apple）到最开始蛇头的位置
						while(markApplewayAndGoBackToPrevAI()){
							;
						}
						// 寻路结束,走一步,退出大循环(TODO:也可以一次性走完全程)
						snake_moveComm(mapAI[x_headAI][y_headAI].subDatas.next_direction);
						
						break;
					} else if (x_headAI > x_apple && y_headAI > y_apple) {
						// 当前点在苹果的右下方，搜索优先级:左上右下
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
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI > x_apple && y_headAI < y_apple) {
						// 当前点在苹果的右上方，搜索优先级:左下右上
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
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI < y_apple) {
						// 当前点在苹果的左上方，搜索优先级:右下左上
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
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI > y_apple) {
						// 当前点在苹果的左下方，搜索优先级:右上左下
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
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI == x_apple && y_headAI > y_apple) {
						// 当前点在苹果的正下方，搜索优先级:上左右下
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
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI == x_apple && y_headAI < y_apple) {
						// 当前点在苹果的正上方，搜索优先级:下左右上
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
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI == y_apple) {
						// 当前点在苹果的正左方，搜索优先级:右下上左
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
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI > x_apple && y_headAI == y_apple) {
						// 当前点在苹果的正右方，搜索优先级:左上下右
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
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					}
				}
			#endif

			// 大师(会优先避免死胡同其次寻路苹果)
			#ifdef AI_LEVEL_MASTER

				#ifdef SNAKE_AI_DEBUG
				if (score >= 36)
				{
					score = score;
				}
				#endif

				// 4个方向移动一步后能否找到蛇尾，苹果
				// TODO : 没必要全看，某方向上能找到蛇尾和苹果则不需要看其他方向了，节省计算量
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

				// // 优先走 既能 找到蛇尾 也能 找到苹果 的方向
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

				// // 其次走 只能 找到蛇尾 的方向（TODO 应该优化成走能通向蛇尾并且是最长的路径这么一条路，尽可能空出空间）
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

				// // 再次走 只能 找到苹果的方向
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

				// // 最差的情况，蛇尾和苹果都看不到，只能随便散步了(TODO 散步的策略也有讲究)
				// else {
				// 	wander(0);
				// }

				// 这一段是有效的AI逻辑，如果上面调试失败可以恢复下面这一段
				x_headAI = x_head;
				y_headAI = y_head;
				clearMapAI();
				while(1){
					// 2-根据优先级决定探索方向
					// 如果 x当前点 == x苹果
					if (x_headAI == x_apple && y_headAI == y_apple)
					{
						// 找到苹果了，利用【上一个节点的方向】信息依次回溯（同时沿路标记apple）到最开始蛇头的位置
						while(markApplewayAndGoBackToPrevAI()){
							;
						}
						// 寻路结束
						// 防止为了吃苹果走死路，这里加上一个判断，走完这个方向一步后如果还能找到蛇尾则允许走
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

						// // 走一步,退出大循环(TODO:也可以一次性走完全程)
						// snake_moveComm(mapAI[x_headAI][y_headAI].subDatas.next_direction);
						
						break;
					} else if (x_headAI > x_apple && y_headAI > y_apple) {
						// 当前点在苹果的右下方，搜索优先级:左上右下
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
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI > x_apple && y_headAI < y_apple) {
						// 当前点在苹果的右上方，搜索优先级:左下右上
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
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI < y_apple) {
						// 当前点在苹果的左上方，搜索优先级:右下左上
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
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI > y_apple) {
						// 当前点在苹果的左下方，搜索优先级:右上左下
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
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI == x_apple && y_headAI > y_apple) {
						// 当前点在苹果的正下方，搜索优先级:上左右下
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
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI == x_apple && y_headAI < y_apple) {
						// 当前点在苹果的正上方，搜索优先级:下左右上
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
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI < x_apple && y_headAI == y_apple) {
						// 当前点在苹果的正左方，搜索优先级:右下上左
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
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
								markDeadwayAndGoBackToPrevAI();
							}
						}
					} else if (x_headAI > x_apple && y_headAI == y_apple) {
						// 当前点在苹果的正右方，搜索优先级:左上下右
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
								// 当前是蛇头，则表示没有通往苹果的路
								// 只好随便走一步, 寻路结束, 退出大循环
								wander(0);
								break;
							}else{
								// 当前点四个方向都不通，标记为死点并回到上一个节点
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

// 从数组导入地图(不刷新画面，仅仅更新内存)
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
				// 仅仅更新内存
				//updateMap(_x, _y, STS_WALL); 
				map[_x][_y] = STS_WALL;
			}
		}
	}
}

// 生成一个障碍物
void SNAKE_addWall(u8 x, u8 y){
	if (map[x][y] == STS_EMPTY) {
		updateMap(x, y, STS_WALL);
	}
}

// 清除一个障碍物
void SNAKE_delWall(u8 x, u8 y){
	if (map[x][y] == STS_WALL) {
		updateMap(x, y, STS_EMPTY);
	}
}

// 清除所有障碍物(不刷新画面)
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

// 生成食物
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
