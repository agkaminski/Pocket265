/* SimAK65 interface
 * Copyright A.K. 2018, 2023
 */

#include <stddef.h>
#include "simak65.h"
#include "decoder.h"
#include "addrmode.h"
#include "types.h"
#include "bus.h"
#include "exec.h"

void simak65_step(struct simak65_cpustate *cpu, unsigned int *cycles)
{
	unsigned int tcycles = 0;
	u8 args[2];
	struct opinfo instruction;
	enum argtype argtype;

	instruction = decode(addrmode_nextpc(cpu));
	argtype = addrmode_getArgs(cpu, args, instruction.mode, &tcycles);
	exec_execute(cpu, instruction.opcode, argtype, args, &tcycles);

	if (cycles != NULL)
		*cycles += tcycles + 1;
}

void simak65_rst(struct simak65_cpustate *cpu, unsigned int *cycles)
{
	unsigned int tcycles = 0;

	exec_rst(cpu, &tcycles);

	if (cycles != NULL)
		*cycles += tcycles;
}

void simak65_nmi(struct simak65_cpustate *cpu, unsigned int *cycles)
{
	unsigned int tcycles = 0;

	exec_nmi(cpu, &tcycles);

	if (cycles != NULL)
		*cycles += tcycles;
}

void simak65_irq(struct simak65_cpustate *cpu, unsigned int *cycles)
{
	unsigned int tcycles = 0;

	exec_irq(cpu, &tcycles);

	if (cycles != NULL)
		*cycles += tcycles;
}

void simak65_init(struct simak65_cpustate *cpu, const struct simak65_bus *ops)
{
	cpu->pc = 0;
	cpu->a = 0;
	cpu->x = 0;
	cpu->y = 0;
	cpu->sp = 0;
	cpu->flags = 0;

	bus_init(ops);
}
