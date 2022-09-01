/* Pocket265 direct memory operations
 * A.K. 2022
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdint.h>

#define read_data(addr) *((volatile uint8_t *)(addr))
#define write_data(addr, data) *((volatile uint8_t *)(addr)) = (data)

#endif
