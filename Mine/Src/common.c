#include "common.h"

//����������õ�����
u16 seedForRand=0;

//��ʱ����
u8 buff[128]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// #pragma optimize=none �Ǹ�IAR���ģ����ⱻ�������Ż���������ʱ��������
// ��ʹ��iar����ʱ������Ҫɾ�����߸ĳ�keil֧�ֵ��﷨
//#pragma optimize=none   //for IAR
//#pragma GCC optimize(0) //for GCC
#pragma GCC push_options
#pragma GCC optimize(0)
void My_delay_ms(u16 n)
{
	u16 x,y;
	for(x=n;x>0;x--)
	{
		for(y=1200;y>0;y--);
	}
}
#pragma GCC pop_options

u8 My_strlen(char* x)
{
	u8 count = 0;
	while(*x != 0x00)
	{
		count++;
		x++;
	}
	return count;
}

u16 My_real_rand(){

	return (u16)my_rand();
}

u16 extRealRand4EmbSnake(){

	return (u16)my_rand();
}