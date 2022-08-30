#ifndef _TYPES_H_
#define _TYPES_H_

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned long      u32;
typedef unsigned long long u64;

typedef signed char        s8;
typedef signed short       s16;
typedef signed long        s32;
typedef signed long long   s64;

typedef struct {
	u16 pc;
	u8 a;
	u8 x;
	u8 y;
	u8 sp;
	u8 flags;
} cpustate_t;

typedef unsigned int cycles_t;

enum { mode_monitor = 0, mode_run };

#endif
