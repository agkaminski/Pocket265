/* SimAK65 on Pocket265
 * Copyright A.K. 2023
 */

#include <stddef.h>

#include "../bsp/pocket265.h"
#include "simak65.h"

static int g_nmi_enable = 1;
static uint8_t g_ram[4 * 1024];

static void bus_write(uint16_t addr, uint8_t byte)
{
	if (addr < 0xc000) {
		g_ram[addr % sizeof(g_ram)] = byte;
	}
	else if ((addr & 0xf800) == NMI_CLEAR_BASE) {
		g_nmi_enable = 1;
	}
	else {
		*((uint8_t *)addr) = byte;
	}
}

static uint8_t bus_read(uint16_t addr)
{
	if (addr < 0xc000) {
		return g_ram[addr % sizeof(g_ram)];
	}
	else if ((addr & 0xf800) == NMI_CLEAR_BASE) {
		g_nmi_enable = 1;
		return 0xff;
	}
	else {
		return *((uint8_t *)addr);
	}
}

int main(void)
{
	struct simak65_cpustate cpu;
	struct simak65_bus ops;
	int i = 0;

	ops.read = bus_read;
	ops.write = bus_write;

	simak65_init(&cpu,  &ops);
	simak65_rst(&cpu, NULL);

	for (;;) {
		simak65_step(&cpu, NULL);
		if (i++ > 200) {
			simak65_nmi(&cpu, NULL);
			i = 0;
		}
	}

	return 0;
}
