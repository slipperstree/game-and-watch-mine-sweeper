#ifndef __MY_MATHUTIL_H__
#define __MY_MATHUTIL_H__

#include "def_types.h"

extern s32 __my_rand_seed;
extern s32 my_seed_rand(s32 a_val);
extern s32 my_rand();
extern s32 my_rand_range(s32 a_min, s32 a_max);
extern s32 sign(s32 a_val);
extern s32 abs(s32 a_val);

#endif //__MY_MATHUTIL_H__
