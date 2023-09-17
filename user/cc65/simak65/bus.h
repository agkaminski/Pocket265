/* SimAK65 memory access
 * Copyright A.K. 2018, 2023
 */

#ifndef SIMAK65_BUS_H_
#define SIMAK65_BUS_H_

extern struct simak65_bus bus;

void bus_init(const struct simak65_bus *ops);

#endif /* SIMAK65_BUS_H_ */
