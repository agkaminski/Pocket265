/* Pocket265 memory test
 * A.K. 2022
 */

#include <stdint.h>

#include "memory.h"

void memory_test(uint16_t *addr)
{
	/* ZP canary - memory aliasing detection */
	write_data(0, 0);

	while (1) {
		write_data(*addr, 0x55);
		if (read_data(*addr) != 0x55 || read_data(0) != 0)
			break;

		write_data(*addr, 0xaa);
		if (read_data(*addr) != 0xaa || read_data(0) != 0)
			break;

		/* Zero-out user memory while we're here */
		write_data(*addr, 0);
		++(*addr);
	}
}
