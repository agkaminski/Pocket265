#include <stdlib.h>
#include "common/error.h"
#include "common/threads.h"
#include "bus/bus.h"
#include "core.h"
#include "addrmode.h"
#include "decoder.h"
#include "exec.h"
#include "flags.h"

typedef enum { CORE_IDLE, CORE_RUN, CORE_STEP, CORE_RST, CORE_IRQ, CORE_NMI } corestate_t;

struct {
	cpustate_t cpu;
	cycles_t cycles;
	thread_t thread;
	corestate_t state;
	mutex_t lock;
	cond_t cpucond;
	cond_t reqcond;
	int running;
	unsigned int speed;
} core_global;

static void step(void)
{
	opinfo_t instruction;
	argtype_t argtype;
	u8 args[2];

	instruction = decode(addrmode_nextpc(&core_global.cpu));
	argtype = addrmode_getArgs(&core_global.cpu, args, instruction.mode, &core_global.cycles);
	exec_execute(&core_global.cpu, instruction.opcode, argtype, args, &core_global.cycles);

	core_global.cycles += 1;
}

static void *core_thread(void *arg)
{
	//cycles_t lastCycles, newCycles;
	//unsigned int speed;

	while (1) {
		lock(&core_global.lock);

		//lastCycles = core_global.cycles;

		while (core_global.state == CORE_IDLE)
			thread_wait(&core_global.cpucond, &core_global.lock);

		switch (core_global.state) {
			case CORE_RUN:
			case CORE_STEP:
				step();
				break;

			case CORE_IRQ:
				exec_irq(&core_global.cpu, &core_global.cycles);
				break;

			case CORE_NMI:
				exec_nmi(&core_global.cpu, &core_global.cycles);
				break;

			case CORE_RST:
				exec_rst(&core_global.cpu, &core_global.cycles);
				break;

			default:
				break;
		}

		if (core_global.running)
			core_global.state = CORE_RUN;
		else
			core_global.state = CORE_IDLE;

		//newCycles = core_global.cycles;
		//speed = core_global.speed;

		thread_signal(&core_global.reqcond);
		unlock(&core_global.lock);

		//thread_sleep((2 * 1000000 * (newCycles - lastCycles)) / speed);
	}

	return NULL;
}

void core_nmi(void)
{
	lock(&core_global.lock);
	core_global.state = CORE_NMI;
	thread_signal(&core_global.cpucond);
	while (core_global.state == CORE_NMI)
		thread_wait(&core_global.reqcond, &core_global.lock);
	unlock(&core_global.lock);
}

void core_irq(void)
{
	lock(&core_global.lock);
	core_global.state = CORE_IRQ;
	thread_signal(&core_global.cpucond);
	while (core_global.state == CORE_IRQ)
		thread_wait(&core_global.reqcond, &core_global.lock);
	unlock(&core_global.lock);
}

void core_rst(void)
{
	lock(&core_global.lock);
	core_global.state = CORE_RST;
	thread_signal(&core_global.cpucond);
	while (core_global.state == CORE_RST)
		thread_wait(&core_global.reqcond, &core_global.lock);
	unlock(&core_global.lock);
}

void core_step(void)
{
	lock(&core_global.lock);
	core_global.state = CORE_STEP;
	thread_signal(&core_global.cpucond);
	while (core_global.state == CORE_STEP)
		thread_wait(&core_global.reqcond, &core_global.lock);
	unlock(&core_global.lock);
}

void core_run(void)
{
	lock(&core_global.lock);
	core_global.state = CORE_RUN;
	core_global.running = 1;
	thread_signal(&core_global.cpucond);
	unlock(&core_global.lock);
}

void core_stop(void)
{
	lock(&core_global.lock);
	core_global.state = CORE_IDLE;
	core_global.running = 0;
	thread_signal(&core_global.cpucond);
	unlock(&core_global.lock);
}

void core_getState(cpustate_t *cpu, cycles_t *cycles)
{
	lock(&core_global.lock);
	if (cpu != NULL)
		*cpu = core_global.cpu;
	if (cycles != NULL)
		*cycles = core_global.cycles;
	unlock(&core_global.lock);
}

void core_setState(cpustate_t *cpu)
{
	lock(&core_global.lock);
	core_global.cpu = *cpu;
	unlock(&core_global.lock);
}

void core_setSpeed(unsigned int speed)
{
	lock(&core_global.lock);
	core_global.speed = speed;
	unlock(&core_global.lock);
}

void core_init(void)
{
	mutex_init(&core_global.lock);
	thread_condInit(&core_global.cpucond);
	thread_condInit(&core_global.reqcond);

	core_global.state = CORE_IDLE;
	core_global.running = 0;
	core_global.speed = 1000000;

	thread_create(&core_global.thread, core_thread, NULL);
}
