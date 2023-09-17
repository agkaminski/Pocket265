/* SimAK65 instruction execution
 * Copyright A.K. 2018, 2023
 */

#ifndef SIMAK65_EXEC_H_
#define SIMAK65_EXEC_H_

#include "types.h"
#include "decoder.h"
#include "addrmode.h"

void exec_execute(struct simak65_cpustate *cpu, enum opcode instruction, enum argtype argtype, u8 *args, unsigned int *cycles);

void exec_irq(struct simak65_cpustate *cpu, unsigned int *cycles);

void exec_nmi(struct simak65_cpustate *cpu, unsigned int *cycles);

void exec_rst(struct simak65_cpustate *cpu, unsigned int *cycles);

#endif /* SIMAK65_EXEC_H_ */
