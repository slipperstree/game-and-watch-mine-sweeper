/******************************************************************************************************
 * 扫雷模块
 * File  : embMine.c
 * Author: MANDA (slipperstree@gmail.com)
 * Date  : 2021.08
 * Updt  : 2021.08
 * Func  : 实现了扫雷的基本数据模型，
 *         但不包含显示逻辑，只包含游戏状态和地图状态管理，与硬件无关可直接移植到各种硬件平台上
 ******************************************************************************************************/

#include "embMine.h"

#ifndef USE_EXTERN_REAL_RAND_FUNC
#include "stdlib.h"
#define RAND_FUNC rand
#endif

#ifdef USE_EXTERN_REAL_RAND_FUNC
extern u16 extRealRand4EmbSnake();
#define RAND_FUNC extRealRand4EmbSnake
#endif

// 临时变量用(sprintf等)
extern u8 buff[128];

// 调用者从init函数传进来的回调函数指针
pMineEventFunc callbackMineEventFunc;
u16 MineEventId, MineEventParam1, MineEventParam2;

// 地图数据
union mapType
{
    struct bit_feild
    {
		u8 isVisitied:1;  	// 访问状态:  1=已访问
        u8 view:3;           // view状态: MINE_VIEW_XXX
		u8 mineSts:4;        // 雷区状态: 0=不是雷，周围也没有雷, 1-8=周围雷数, 10=是雷
    } subDatas;
    u8 dat;
};

// 地图数据
union mapType map[MINE_SIZE_X][MINE_SIZE_Y];

// 游戏状态
u8 m_iGameStatus = 0;
#define GAME_STATUS_WELCOME		9
#define GAME_STATUS_NORMAL		0
#define GAME_STATUS_WIN			1
#define GAME_STATUS_LOSE		2

// 雷区状态(1-8的数字表示周围有几颗雷不用定义了)
#define MINE_STS_EMPTY	0	// 不是雷，周围也没有雷
#define MINE_STS_MINE	10	// 雷(不可以占用1-8的数字,也不可以超过15)

//翻开模式
#define TAP_TYPE_DIG	1
//插旗模式
#define TAP_TYPE_FLAG	2
//放大镜模式
#define TAP_TYPE_ZOOM	3
u8 mTapType = TAP_TYPE_DIG;

u8 mMineCnt = 15; //雷的个数
u8 isOver = FALSE; //当前游戏是否为非活动状态
u8 mIsFirstTap = TRUE; //是否是第一次点击（需要布雷）
u8 mFlags;  //拥有的旗子数

// 函数声明
void createMineMap(u8 firstTapX, u8 firstTapY);
void clear_visited();
u8 judgeView(u8 x, u8 y, u8 viewType);
u8 judgeWin();
void autoDig(u8 x, u8 y);

void fireEvent(u16 id, u16 p1, u16 p2);
void gameover();
void updView(u8 x, u8 y, u8 viewId);


//设定雷数
void MINE_setMineNUM(u8 num) {
	mMineCnt=num;
	mFlags=num;
}

void createMineMap(u8 firstTapX, u8 firstTapY){
	u8 cnt,x,y;

	//先检查雷数是否大于总单元数 - 9 (减9是因为要为玩家首次点击的地方空出周围一圈空地)
	if (mMineCnt>MINE_SIZE_X*MINE_SIZE_Y-9)
	{
		mMineCnt=MINE_SIZE_X*MINE_SIZE_Y-9; //是则使之相等
	}

	//随机产生mMineCnt个雷(|0 取整)
	for (cnt=0;cnt<mMineCnt;cnt++)
	{
		while(TRUE)
		{
			x=RAND_FUNC() % MINE_SIZE_X;
			y=RAND_FUNC() % MINE_SIZE_Y;
			
			//当前位置没有布过雷 并且 不处于玩家首次点击周围一格范围内
			//否则重新随机
			if (map[x][y].subDatas.mineSts != MINE_STS_MINE
				&& (abs(x-firstTapX)>1 || abs(y-firstTapY)>1) 
				) {
				break;
			}
		}
		map[x][y].subDatas.mineSts = MINE_STS_MINE;
	}

	//根据布好的雷计算数字
	for (x=0;x<MINE_SIZE_X ;x++ )
	{
		for (y=0;y<MINE_SIZE_Y ;y++ )
		{
			if (map[x][y].subDatas.mineSts != MINE_STS_MINE) //如果该点不是雷就算周围的雷数
			{
				if (x-1>=0 && y-1>=0) //left top
				{
					if (map[x-1][y-1].subDatas.mineSts == MINE_STS_MINE) map[x][y].subDatas.mineSts++;
				}
				
				if (y-1>=0) //top
				{
					if (map[x][y-1].subDatas.mineSts == MINE_STS_MINE) map[x][y].subDatas.mineSts++;
				}
				
				if (x+1<MINE_SIZE_X && y-1>=0) //right top
				{
					if (map[x+1][y-1].subDatas.mineSts == MINE_STS_MINE) map[x][y].subDatas.mineSts++;
				}

				if (x-1>=0) //left
				{
					if (map[x-1][y].subDatas.mineSts == MINE_STS_MINE) map[x][y].subDatas.mineSts++;
				}

				if (x+1<MINE_SIZE_X) //right
				{
					if (map[x+1][y].subDatas.mineSts == MINE_STS_MINE) map[x][y].subDatas.mineSts++;
				}

				if (x-1>=0 && y+1<MINE_SIZE_Y) //left bottom
				{
					if (map[x-1][y+1].subDatas.mineSts == MINE_STS_MINE) map[x][y].subDatas.mineSts++;
				}

				if (y+1<MINE_SIZE_Y) //bottom
				{
					if (map[x][y+1].subDatas.mineSts == MINE_STS_MINE) map[x][y].subDatas.mineSts++;
				}

				if (x+1<MINE_SIZE_X && y+1<MINE_SIZE_Y) //right
				{
					if (map[x+1][y+1].subDatas.mineSts == MINE_STS_MINE) map[x][y].subDatas.mineSts++;
				}
			} //if 不是雷
		} //for j
	} //for i
}

//插旗/拔旗
void MINE_switchFlag(u8 x, u8 y){

	// 游戏结束后无动作
	if (isOver) return;

	if (judgeView(x, y, MINE_VIEW_NORMAL) && mFlags>0) {
		// 插红旗
		updView(x, y, MINE_VIEW_FLAG);
		mFlags--;
		fireEvent(MINE_EVENT_UPDATE_FLAG_CNT, mFlags, 0);
	}
	else if (judgeView(x, y, MINE_VIEW_FLAG)) {
		// 拔红旗
		updView(x, y, MINE_VIEW_NORMAL);
		mFlags++;
		fireEvent(MINE_EVENT_UPDATE_FLAG_CNT, mFlags, 0);
	}
}

void digOut(u8 x, u8 y){
	u8 i,j;

	//如果该点不是雷
	if (map[x][y].subDatas.mineSts != MINE_STS_MINE) 
	{
		//自动往周围探索空地
		clear_visited();
		autoDig(x,y); 
		
		//挖好雷后判断是否胜利
		if (judgeWin()) 
		{
			m_iGameStatus = GAME_STATUS_WIN;
			isOver=TRUE;

			gameover();
		}
	}
	//如果是雷（被炸死 停止记时 标所有雷等）
	else 
	{
		isOver=TRUE;

		// 事件通知外部被炸死
		m_iGameStatus = GAME_STATUS_LOSE;
		gameover();

		//将所有雷标出，并标出插错红旗的地方。
		for (i=0;i<MINE_SIZE_X ;i++ )
		{
			for (j=0;j<MINE_SIZE_Y ;j++ )
			{	
				// 如果是雷且未翻开的状态，标记为翻开的雷
				if (map[i][j].subDatas.mineSts == MINE_STS_MINE && judgeView(i, j, MINE_VIEW_NORMAL)) {
					updView(i, j, MINE_VIEW_MINE);
				}
				// 如果不是雷但被错误的标记上旗子的状态，标记为打叉的雷
				if (map[i][j].subDatas.mineSts != MINE_STS_MINE && judgeView(i, j, MINE_VIEW_FLAG)) {
					updView(i, j, MINE_VIEW_WRONG);
				}
			}
		}

		//当前挖到的雷, 画红色背景的雷（玩家踩到的地雷）
		updView(x, y, MINE_VIEW_DEADMINE);
		mTapType = TAP_TYPE_DIG;
	}
}

//挖开
void MINE_click(u8 x, u8 y)
{
	u8 isDigedDight=FALSE;
	u8 cntFlagArroundDight=0;

	// 游戏结束后不允许挖雷
	if (isOver) return;
	
	// 检查越界
	if(x<0 || y<0 || x>=MINE_SIZE_X || y>=MINE_SIZE_Y) return;

	// 当前 为未翻开的状态 或者 是翻开的数字 才允许继续挖 （点击数字时如果数字周围旗子数够了就自动翻开周围剩下未翻开的）
	if (judgeView(x, y, MINE_VIEW_NORMAL) || (judgeView(x, y, MINE_VIEW_DIGED) && map[x][y].subDatas.mineSts >=1 && map[x][y].subDatas.mineSts <= 8)) {
		if(judgeView(x, y, MINE_VIEW_DIGED) && map[x][y].subDatas.mineSts >=1 && map[x][y].subDatas.mineSts <= 8){
			isDigedDight = TRUE;
		}
	} else {
		return;
	}

	// 是否为游戏开始后首次点击
	if (mIsFirstTap)
	{
		// 布雷
		createMineMap(x, y);
		mIsFirstTap = FALSE;
		fireEvent(MINE_EVENT_TIMESTART, 0, 0);
	}

	// 当前单元格是被翻开的数字？
	if( isDigedDight ){
		// 周围已经标记的旗子数量超过了应有的数量吗
		cntFlagArroundDight = 0;
		if (judgeView( x-1, y-1 , MINE_VIEW_FLAG )) cntFlagArroundDight++;      //left top
		if (judgeView( x,   y-1 , MINE_VIEW_FLAG )) cntFlagArroundDight++;      //top
		if (judgeView( x+1, y-1 , MINE_VIEW_FLAG )) cntFlagArroundDight++;      //right top
		if (judgeView( x-1, y   , MINE_VIEW_FLAG )) cntFlagArroundDight++;      //left
		if (judgeView( x+1, y   , MINE_VIEW_FLAG )) cntFlagArroundDight++;      //right
		if (judgeView( x-1, y+1 , MINE_VIEW_FLAG )) cntFlagArroundDight++;      //left bottom
		if (judgeView( x,   y+1 , MINE_VIEW_FLAG )) cntFlagArroundDight++;      //bottom
		if (judgeView( x+1, y+1 , MINE_VIEW_FLAG )) cntFlagArroundDight++;      //right

		// 旗子数量已经够了，翻开周围一圈剩下的格子
		if (cntFlagArroundDight >= map[x][y].subDatas.mineSts) {
			if (judgeView( x-1, y-1 , MINE_VIEW_NORMAL )) digOut( x-1, y-1 );    //left top
			if (judgeView( x,   y-1 , MINE_VIEW_NORMAL )) digOut( x,   y-1 );    //top
			if (judgeView( x+1, y-1 , MINE_VIEW_NORMAL )) digOut( x+1, y-1 );    //right top
			if (judgeView( x-1, y   , MINE_VIEW_NORMAL )) digOut( x-1, y   );    //left
			if (judgeView( x+1, y   , MINE_VIEW_NORMAL )) digOut( x+1, y   );    //right
			if (judgeView( x-1, y+1 , MINE_VIEW_NORMAL )) digOut( x-1, y+1 );    //left bottom
			if (judgeView( x,   y+1 , MINE_VIEW_NORMAL )) digOut( x,   y+1 );    //bottom
			if (judgeView( x+1, y+1 , MINE_VIEW_NORMAL )) digOut( x+1, y+1 );    //right
		}
	} 
	// 当前单元格是数字 END
	// 当前单元格未翻开
	else{
		// 直接开挖！
		digOut(x, y);
	}
}

// 四周存在已经被标记过的空地(非数字，必须得是空地)吗？
u8 hasArrivedEmptyBlockArrowndMe(char x, char y){
	if(x<0 || x>=MINE_SIZE_X || y<0 || y>=MINE_SIZE_Y) {
		return FALSE;
	} else {
		return (map[x][y].subDatas.isVisitied && map[x][y].subDatas.mineSts == MINE_STS_EMPTY) ? TRUE : FALSE;
	}
}

// 自动挖开周围空白区域
void autoDig(u8 startX, u8 startY)
{
	char x,y;
	// 每次扫描一遍全图有没有翻开新的空地（非数字）？,有的话还需要继续扫描一次全图
	u8 hasNewDigedEmptyBlockFlg=1;

	// 首先翻开起始位置并标记
	updView(startX, startY, MINE_VIEW_DIGED);
	map[startX][startY].subDatas.isVisitied = TRUE;

	// 多次遍历整张地图，翻开所有与起始位置相连的空地
	while(hasNewDigedEmptyBlockFlg){
		hasNewDigedEmptyBlockFlg = 0;
		for (y = 0; y < MINE_SIZE_Y; y++)
		{
			for (x = 0; x < MINE_SIZE_X; x++){
				// 如果这个点不是雷 且 尚未被翻开过，那么对其进行判断，看四周是否有本次已经被标记的空地如果有，表示这个点是跟起始点连在一起的所以也应该被挖开
				if (map[x][y].subDatas.mineSts != MINE_STS_MINE && judgeView(x, y, MINE_VIEW_NORMAL))
				{
					// 四周存在已经被标记过的空地的话翻开这个格子并标记 (非数字，必须得是空地)
					if( 	hasArrivedEmptyBlockArrowndMe(x-1, y-1) 	||
							hasArrivedEmptyBlockArrowndMe(x  , y-1) 	||
							hasArrivedEmptyBlockArrowndMe(x+1, y-1) 	||
							hasArrivedEmptyBlockArrowndMe(x-1, y  ) 	||
							hasArrivedEmptyBlockArrowndMe(x+1, y  ) 	||
							hasArrivedEmptyBlockArrowndMe(x-1, y+1) 	||
							hasArrivedEmptyBlockArrowndMe(x  , y+1) 	||
							hasArrivedEmptyBlockArrowndMe(x+1, y+1)
							) {

						updView(x, y, MINE_VIEW_DIGED);
						map[x][y].subDatas.isVisitied = TRUE;
						LOG("     自动翻开!: x=%b2u,y=%b2u,sts=%b2u\r\n", x, y, map[x][y].subDatas.mineSts);

						// 如果当前被翻开的这个格子是空地（非数字）那么需要继续下一轮循环以便继续寻找这个空地四周有没有连着的空地或数字
						// 否则表示当前被翻开的这个格子是数字，那么只翻开这个格子就可以了，不用继续向周围扩张
						if (map[x][y].subDatas.mineSts == MINE_STS_EMPTY)
						{
							hasNewDigedEmptyBlockFlg = 1;
						}
					}
				}
			}
		}
	}
}

//清空已访问标志(自动挖开一片之前调用)
void clear_visited()
{
	u8 i, j;
	for (i=0;i<MINE_SIZE_X;i++)
		for (j=0;j<MINE_SIZE_Y;j++)
			map[i][j].subDatas.isVisitied=FALSE;
}

/** 判断指定位置的view种类，如果指定位置越界或不是指定种类返回FALSE，是指定种类返回TRUE */
u8 judgeView(u8 x, u8 y, u8 viewType){
	// 检查越界
	if(x<0 || y<0 || x>=MINE_SIZE_X || y>=MINE_SIZE_Y){
		// 越界了直接返回FALSE
		return FALSE;
	}
	else {
		return (map[x][y].subDatas.view == viewType) ? TRUE : FALSE;
	}
}

//判断当前非雷区是否已被全部翻开，是则自动标上所有红旗并判为赢返回TRUE
//否则返回FALSE
u8 judgeWin() {
	u8 i,j;
	for (i=0;i<MINE_SIZE_X ;i++ )
	{
		for (j=0;j<MINE_SIZE_Y ;j++ )
		{
			if ((map[i][j].subDatas.mineSts != MINE_STS_MINE && !judgeView(i, j, MINE_VIEW_DIGED)))
			{
				return FALSE;
			}
		}
	}

	//非雷区已被全部翻开 自动标上所有红旗
	for (i=0;i<MINE_SIZE_X ;i++ )
	{
		for (j=0;j<MINE_SIZE_Y ;j++ )
		{
			if (map[i][j].subDatas.mineSts == MINE_STS_MINE && !judgeView(i, j, MINE_VIEW_FLAG))
			{
				updView(i, j, MINE_VIEW_FLAG);
			}
		}
	}

	return TRUE;
}












void fireEvent(u16 id, u16 p1, u16 p2){
	MineEventId = id;
	MineEventParam1 = p1;
	MineEventParam2 = p2;
	callbackMineEventFunc();
}

// 获取指定位置的视图内容（视图内容不完全等同地图原始内容，比如虽然都是身体，但有拐弯的有直的，拐弯的方向也是不一样的）
u8 MINE_getView(u8 mapX, u8  mapY){
	return map[mapX][mapY].subDatas.view;
}

u8 MINE_getViewNum(u8 mapX, u8  mapY){
	if (map[mapX][mapY].subDatas.mineSts != MINE_STS_MINE)
	{
		// 不为 MINE_STS_MINE（10）的时候表示该位置周围的雷数字(0-8)
		return map[mapX][mapY].subDatas.mineSts;
	} else {
		return MINE_STS_MINE;
	}
}

// 所有需要更新map数据的时候都调用这个函数，保证能及时通知控制端更新响应的画面
void updView(u8 x, u8 y, u8 viewId){
	u8 oldValue;
	// 检查越界
	if(x<0 || y<0 || x>=MINE_SIZE_X || y>=MINE_SIZE_Y){
		// 越界了什么都不做直接返回
		return;
	}
	oldValue=map[x][y].subDatas.view;
	if (oldValue != viewId)
	{
		map[x][y].subDatas.view = viewId;
		fireEvent(MINE_EVENT_UPDATE_BLOCK, x, y);
	}
}

// 重启游戏
void MINE_restart(){
	u8 x, y;

	// 初始分数
	m_iGameStatus = GAME_STATUS_WELCOME;

	mIsFirstTap = TRUE;
	mTapType = TAP_TYPE_DIG;
	isOver = FALSE;

	for (x=0;x<MINE_SIZE_X;x++){
		for (y=0;y<MINE_SIZE_Y;y++){
			map[x][y].dat = 0;
			// 因为是union数据，dat=0相当于下面3句的效果
			// map[x][y].subDatas.isVisitied=FALSE;
			// map[x][y].subDatas.mineSts=MINE_STS_EMPTY;
			// map[x][y].subDatas.view=MINE_VIEW_NORMAL;
		}
	}

	//重置旗子数
	mFlags = mMineCnt;

	// 事件参数没有使用
	fireEvent(MINE_EVENT_RESTART, 0, 0);
}

// 模块初始化,只应该被调用一次,init调用完以后就可以加入障碍物了
void MINE_init(pMineEventFunc pCallbackMineEventFunc){

	callbackMineEventFunc = pCallbackMineEventFunc;

	// 防止内存中有垃圾数据，初始化所有地图为空(不通知外部刷新画面)
	MINE_restart();
}

void gameover(){
	fireEvent(MINE_EVENT_GAMEOVER, m_iGameStatus == GAME_STATUS_WIN ? TRUE : FALSE, 0);
}

u16 MINE_getNowFlags(){
	return mFlags;
}
