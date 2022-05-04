#include "common.h"

//生成随机数用的种子
u16 seedForRand=0;

//临时变量
u8 buff[128]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// #pragma optimize=none 是给IAR看的，避免被编译器优化掉导致延时被跳过。
// 不使用iar环境时可能需要删掉或者改成keil支持的语法
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