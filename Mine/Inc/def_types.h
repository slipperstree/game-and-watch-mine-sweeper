#ifndef __DEF_TYPES_H_
#define __DEF_TYPES_H_

typedef unsigned char  u8,  byte, uchar, echar;
typedef unsigned short u16, hword, ushort, eshort;
typedef unsigned int   u32, word, uint, eint;
typedef unsigned long long u64;

typedef signed char  s8;
typedef signed short s16;
typedef signed int   s32;
typedef signed long long s64;

typedef volatile u8  vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;

typedef volatile s8  vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;

typedef const u8  cu8;
typedef const u16 cu16;
typedef const u32 cu32;
typedef const u64 cu64;

typedef const s8  cs8;
typedef const s16 cs16;
typedef const s32 cs32;
typedef const s64 cs64;

typedef struct { u32 data[8];  } BLOCK;

typedef const char * const CSTR;

typedef s32 FIXED;
typedef u16 COLOR;

#ifndef __cplusplus
typedef enum { false, true } bool;
#endif

#ifndef BOOL
typedef u8 BOOL;
#define TRUE 1
#define FALSE 0
#endif

#endif
