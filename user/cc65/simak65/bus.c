/* SimAK65 memory access
 * Copyright A.K. 2018, 2023
 */

#include "simak65.h"
#include "bus.h"

struct simak65_bus bus;

void bus_init(const struct simak65_bus *ops)
{
	bus = *ops;
}
