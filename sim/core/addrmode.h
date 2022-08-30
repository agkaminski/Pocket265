#ifndef _ADDRMODE_H_
#define _ADDRMODE_H_

#include "common/types.h"
#include "decoder.h"

typedef enum { arg_none, arg_byte, arg_addr } argtype_t;

u8 addrmode_nextpc(cpustate_t *cpu);

argtype_t addrmode_getArgs(cpustate_t *cpu, u8 *args, addrmode_t mode, cycles_t *cycles);

#endif
