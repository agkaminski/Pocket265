#ifndef _CORE_H_
#define _CORE_H_

#include "common/types.h"

void core_nmi(void);

void core_irq(void);

void core_rst(void);

void core_step(void);

void core_run(void);

void core_stop(void);

void core_getState(cpustate_t *cpu, cycles_t *cycles);

void core_setState(cpustate_t *cpu);

void core_setSpeed(unsigned int speed);

void core_init(void);

#endif
