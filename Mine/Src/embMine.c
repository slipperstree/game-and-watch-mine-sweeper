/******************************************************************************************************
 * ɨ��ģ��
 * File  : embMine.c
 * Author: MANDA (slipperstree@gmail.com)
 * Date  : 2021.08
 * Updt  : 2021.08
 * Func  : ʵ����ɨ�׵Ļ�������ģ�ͣ�
 *         ����������ʾ�߼���ֻ������Ϸ״̬�͵�ͼ״̬������Ӳ���޹ؿ�ֱ����ֲ������Ӳ��ƽ̨��
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

// ��ʱ������(sprintf��)
extern u8 buff[128];

// �����ߴ�init�����������Ļص�����ָ��
pMineEventFunc callbackMineEventFunc;
u16 MineEventId, MineEventParam1, MineEventParam2;

// ��ͼ����
union mapType
{
    struct bit_feild
    {
		u8 isVisitied:1;  	// ����״̬:  1=�ѷ���
        u8 view:3;           // view״̬: MINE_VIEW_XXX
		u8 mineSts:4;        // ����״̬: 0=�����ף���ΧҲû����, 1-8=��Χ����, 10=����
    } subDatas;
    u8 dat;
};

// ��ͼ����
union mapType map[MINE_SIZE_X][MINE_SIZE_Y];

// ��Ϸ״̬
u8 m_iGameStatus = 0;
#define GAME_STATUS_WELCOME		9
#define GAME_STATUS_NORMAL		0
#define GAME_STATUS_WIN			1
#define GAME_STATUS_LOSE		2

// ����״̬(1-8�����ֱ�ʾ��Χ�м����ײ��ö�����)
#define MINE_STS_EMPTY	0	// �����ף���ΧҲû����
#define MINE_STS_MINE	10	// ��(������ռ��1-8������,Ҳ�����Գ���15)

//����ģʽ
#define TAP_TYPE_DIG	1
//����ģʽ
#define TAP_TYPE_FLAG	2
//�Ŵ�ģʽ
#define TAP_TYPE_ZOOM	3
u8 mTapType = TAP_TYPE_DIG;

u8 mMineCnt = 15; //�׵ĸ���
u8 isOver = FALSE; //��ǰ��Ϸ�Ƿ�Ϊ�ǻ״̬
u8 mIsFirstTap = TRUE; //�Ƿ��ǵ�һ�ε������Ҫ���ף�
u8 mFlags;  //ӵ�е�������

// ��������
void createMineMap(u8 firstTapX, u8 firstTapY);
void clear_visited();
u8 judgeView(u8 x, u8 y, u8 viewType);
u8 judgeWin();
void autoDig(u8 x, u8 y);

void fireEvent(u16 id, u16 p1, u16 p2);
void gameover();
void updView(u8 x, u8 y, u8 viewId);


//�趨����
void MINE_setMineNUM(u8 num) {
	mMineCnt=num;
	mFlags=num;
}

void createMineMap(u8 firstTapX, u8 firstTapY){
	u8 cnt,x,y;

	//�ȼ�������Ƿ�����ܵ�Ԫ�� - 9 (��9����ΪҪΪ����״ε���ĵط��ճ���ΧһȦ�յ�)
	if (mMineCnt>MINE_SIZE_X*MINE_SIZE_Y-9)
	{
		mMineCnt=MINE_SIZE_X*MINE_SIZE_Y-9; //����ʹ֮���
	}

	//�������mMineCnt����(|0 ȡ��)
	for (cnt=0;cnt<mMineCnt;cnt++)
	{
		while(TRUE)
		{
			x=RAND_FUNC() % MINE_SIZE_X;
			y=RAND_FUNC() % MINE_SIZE_Y;
			
			//��ǰλ��û�в����� ���� ����������״ε����Χһ��Χ��
			//�����������
			if (map[x][y].subDatas.mineSts != MINE_STS_MINE
				&& (abs(x-firstTapX)>1 || abs(y-firstTapY)>1) 
				) {
				break;
			}
		}
		map[x][y].subDatas.mineSts = MINE_STS_MINE;
	}

	//���ݲ��õ��׼�������
	for (x=0;x<MINE_SIZE_X ;x++ )
	{
		for (y=0;y<MINE_SIZE_Y ;y++ )
		{
			if (map[x][y].subDatas.mineSts != MINE_STS_MINE) //����õ㲻���׾�����Χ������
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
			} //if ������
		} //for j
	} //for i
}

//����/����
void MINE_switchFlag(u8 x, u8 y){

	// ��Ϸ�������޶���
	if (isOver) return;

	if (judgeView(x, y, MINE_VIEW_NORMAL) && mFlags>0) {
		// �����
		updView(x, y, MINE_VIEW_FLAG);
		mFlags--;
		fireEvent(MINE_EVENT_UPDATE_FLAG_CNT, mFlags, 0);
	}
	else if (judgeView(x, y, MINE_VIEW_FLAG)) {
		// �κ���
		updView(x, y, MINE_VIEW_NORMAL);
		mFlags++;
		fireEvent(MINE_EVENT_UPDATE_FLAG_CNT, mFlags, 0);
	}
}

void digOut(u8 x, u8 y){
	u8 i,j;

	//����õ㲻����
	if (map[x][y].subDatas.mineSts != MINE_STS_MINE) 
	{
		//�Զ�����Χ̽���յ�
		clear_visited();
		autoDig(x,y); 
		
		//�ں��׺��ж��Ƿ�ʤ��
		if (judgeWin()) 
		{
			m_iGameStatus = GAME_STATUS_WIN;
			isOver=TRUE;

			gameover();
		}
	}
	//������ף���ը�� ֹͣ��ʱ �������׵ȣ�
	else 
	{
		isOver=TRUE;

		// �¼�֪ͨ�ⲿ��ը��
		m_iGameStatus = GAME_STATUS_LOSE;
		gameover();

		//�������ױ���������������ĵط���
		for (i=0;i<MINE_SIZE_X ;i++ )
		{
			for (j=0;j<MINE_SIZE_Y ;j++ )
			{	
				// ���������δ������״̬�����Ϊ��������
				if (map[i][j].subDatas.mineSts == MINE_STS_MINE && judgeView(i, j, MINE_VIEW_NORMAL)) {
					updView(i, j, MINE_VIEW_MINE);
				}
				// ��������׵�������ı�������ӵ�״̬�����Ϊ������
				if (map[i][j].subDatas.mineSts != MINE_STS_MINE && judgeView(i, j, MINE_VIEW_FLAG)) {
					updView(i, j, MINE_VIEW_WRONG);
				}
			}
		}

		//��ǰ�ڵ�����, ����ɫ�������ף���Ҳȵ��ĵ��ף�
		updView(x, y, MINE_VIEW_DEADMINE);
		mTapType = TAP_TYPE_DIG;
	}
}

//�ڿ�
void MINE_click(u8 x, u8 y)
{
	u8 isDigedDight=FALSE;
	u8 cntFlagArroundDight=0;

	// ��Ϸ��������������
	if (isOver) return;
	
	// ���Խ��
	if(x<0 || y<0 || x>=MINE_SIZE_X || y>=MINE_SIZE_Y) return;

	// ��ǰ Ϊδ������״̬ ���� �Ƿ��������� ����������� ���������ʱ���������Χ���������˾��Զ�������Χʣ��δ�����ģ�
	if (judgeView(x, y, MINE_VIEW_NORMAL) || (judgeView(x, y, MINE_VIEW_DIGED) && map[x][y].subDatas.mineSts >=1 && map[x][y].subDatas.mineSts <= 8)) {
		if(judgeView(x, y, MINE_VIEW_DIGED) && map[x][y].subDatas.mineSts >=1 && map[x][y].subDatas.mineSts <= 8){
			isDigedDight = TRUE;
		}
	} else {
		return;
	}

	// �Ƿ�Ϊ��Ϸ��ʼ���״ε��
	if (mIsFirstTap)
	{
		// ����
		createMineMap(x, y);
		mIsFirstTap = FALSE;
		fireEvent(MINE_EVENT_TIMESTART, 0, 0);
	}

	// ��ǰ��Ԫ���Ǳ����������֣�
	if( isDigedDight ){
		// ��Χ�Ѿ���ǵ���������������Ӧ�е�������
		cntFlagArroundDight = 0;
		if (judgeView( x-1, y-1 , MINE_VIEW_FLAG )) cntFlagArroundDight++;      //left top
		if (judgeView( x,   y-1 , MINE_VIEW_FLAG )) cntFlagArroundDight++;      //top
		if (judgeView( x+1, y-1 , MINE_VIEW_FLAG )) cntFlagArroundDight++;      //right top
		if (judgeView( x-1, y   , MINE_VIEW_FLAG )) cntFlagArroundDight++;      //left
		if (judgeView( x+1, y   , MINE_VIEW_FLAG )) cntFlagArroundDight++;      //right
		if (judgeView( x-1, y+1 , MINE_VIEW_FLAG )) cntFlagArroundDight++;      //left bottom
		if (judgeView( x,   y+1 , MINE_VIEW_FLAG )) cntFlagArroundDight++;      //bottom
		if (judgeView( x+1, y+1 , MINE_VIEW_FLAG )) cntFlagArroundDight++;      //right

		// ���������Ѿ����ˣ�������ΧһȦʣ�µĸ���
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
	// ��ǰ��Ԫ�������� END
	// ��ǰ��Ԫ��δ����
	else{
		// ֱ�ӿ��ڣ�
		digOut(x, y);
	}
}

// ���ܴ����Ѿ�����ǹ��Ŀյ�(�����֣�������ǿյ�)��
u8 hasArrivedEmptyBlockArrowndMe(char x, char y){
	if(x<0 || x>=MINE_SIZE_X || y<0 || y>=MINE_SIZE_Y) {
		return FALSE;
	} else {
		return (map[x][y].subDatas.isVisitied && map[x][y].subDatas.mineSts == MINE_STS_EMPTY) ? TRUE : FALSE;
	}
}

// �Զ��ڿ���Χ�հ�����
void autoDig(u8 startX, u8 startY)
{
	char x,y;
	// ÿ��ɨ��һ��ȫͼ��û�з����µĿյأ������֣���,�еĻ�����Ҫ����ɨ��һ��ȫͼ
	u8 hasNewDigedEmptyBlockFlg=1;

	// ���ȷ�����ʼλ�ò����
	updView(startX, startY, MINE_VIEW_DIGED);
	map[startX][startY].subDatas.isVisitied = TRUE;

	// ��α������ŵ�ͼ��������������ʼλ�������Ŀյ�
	while(hasNewDigedEmptyBlockFlg){
		hasNewDigedEmptyBlockFlg = 0;
		for (y = 0; y < MINE_SIZE_Y; y++)
		{
			for (x = 0; x < MINE_SIZE_X; x++){
				// �������㲻���� �� ��δ������������ô��������жϣ��������Ƿ��б����Ѿ�����ǵĿյ�����У���ʾ������Ǹ���ʼ������һ�������ҲӦ�ñ��ڿ�
				if (map[x][y].subDatas.mineSts != MINE_STS_MINE && judgeView(x, y, MINE_VIEW_NORMAL))
				{
					// ���ܴ����Ѿ�����ǹ��ĿյصĻ�����������Ӳ���� (�����֣�������ǿյ�)
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
						LOG("     �Զ�����!: x=%b2u,y=%b2u,sts=%b2u\r\n", x, y, map[x][y].subDatas.mineSts);

						// �����ǰ����������������ǿյأ������֣���ô��Ҫ������һ��ѭ���Ա����Ѱ������յ�������û�����ŵĿյػ�����
						// �����ʾ��ǰ��������������������֣���ôֻ����������ӾͿ����ˣ����ü�������Χ����
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

//����ѷ��ʱ�־(�Զ��ڿ�һƬ֮ǰ����)
void clear_visited()
{
	u8 i, j;
	for (i=0;i<MINE_SIZE_X;i++)
		for (j=0;j<MINE_SIZE_Y;j++)
			map[i][j].subDatas.isVisitied=FALSE;
}

/** �ж�ָ��λ�õ�view���࣬���ָ��λ��Խ�����ָ�����෵��FALSE����ָ�����෵��TRUE */
u8 judgeView(u8 x, u8 y, u8 viewType){
	// ���Խ��
	if(x<0 || y<0 || x>=MINE_SIZE_X || y>=MINE_SIZE_Y){
		// Խ����ֱ�ӷ���FALSE
		return FALSE;
	}
	else {
		return (map[x][y].subDatas.view == viewType) ? TRUE : FALSE;
	}
}

//�жϵ�ǰ�������Ƿ��ѱ�ȫ�������������Զ��������к��첢��ΪӮ����TRUE
//���򷵻�FALSE
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

	//�������ѱ�ȫ������ �Զ��������к���
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

// ��ȡָ��λ�õ���ͼ���ݣ���ͼ���ݲ���ȫ��ͬ��ͼԭʼ���ݣ�������Ȼ�������壬���й������ֱ�ģ�����ķ���Ҳ�ǲ�һ���ģ�
u8 MINE_getView(u8 mapX, u8  mapY){
	return map[mapX][mapY].subDatas.view;
}

u8 MINE_getViewNum(u8 mapX, u8  mapY){
	if (map[mapX][mapY].subDatas.mineSts != MINE_STS_MINE)
	{
		// ��Ϊ MINE_STS_MINE��10����ʱ���ʾ��λ����Χ��������(0-8)
		return map[mapX][mapY].subDatas.mineSts;
	} else {
		return MINE_STS_MINE;
	}
}

// ������Ҫ����map���ݵ�ʱ�򶼵��������������֤�ܼ�ʱ֪ͨ���ƶ˸�����Ӧ�Ļ���
void updView(u8 x, u8 y, u8 viewId){
	u8 oldValue;
	// ���Խ��
	if(x<0 || y<0 || x>=MINE_SIZE_X || y>=MINE_SIZE_Y){
		// Խ����ʲô������ֱ�ӷ���
		return;
	}
	oldValue=map[x][y].subDatas.view;
	if (oldValue != viewId)
	{
		map[x][y].subDatas.view = viewId;
		fireEvent(MINE_EVENT_UPDATE_BLOCK, x, y);
	}
}

// ������Ϸ
void MINE_restart(){
	u8 x, y;

	// ��ʼ����
	m_iGameStatus = GAME_STATUS_WELCOME;

	mIsFirstTap = TRUE;
	mTapType = TAP_TYPE_DIG;
	isOver = FALSE;

	for (x=0;x<MINE_SIZE_X;x++){
		for (y=0;y<MINE_SIZE_Y;y++){
			map[x][y].dat = 0;
			// ��Ϊ��union���ݣ�dat=0�൱������3���Ч��
			// map[x][y].subDatas.isVisitied=FALSE;
			// map[x][y].subDatas.mineSts=MINE_STS_EMPTY;
			// map[x][y].subDatas.view=MINE_VIEW_NORMAL;
		}
	}

	//����������
	mFlags = mMineCnt;

	// �¼�����û��ʹ��
	fireEvent(MINE_EVENT_RESTART, 0, 0);
}

// ģ���ʼ��,ֻӦ�ñ�����һ��,init�������Ժ�Ϳ��Լ����ϰ�����
void MINE_init(pMineEventFunc pCallbackMineEventFunc){

	callbackMineEventFunc = pCallbackMineEventFunc;

	// ��ֹ�ڴ������������ݣ���ʼ�����е�ͼΪ��(��֪ͨ�ⲿˢ�»���)
	MINE_restart();
}

void gameover(){
	fireEvent(MINE_EVENT_GAMEOVER, m_iGameStatus == GAME_STATUS_WIN ? TRUE : FALSE, 0);
}

u16 MINE_getNowFlags(){
	return mFlags;
}
