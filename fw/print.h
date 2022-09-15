/* Pocket265 lite print routines
 * A.K. 2022
 */

#include <stdint.h>

#ifndef PRINT_H_
#define PRINT_H_

unsigned char print_string(char *out, const char *in, unsigned char min, unsigned char max);

unsigned char print_u16(char *out, uint16_t num, unsigned char min);

unsigned char print_s16(char *out, int16_t num, unsigned char min);

unsigned char print_x8(char *out, unsigned char byte);

unsigned char print_x16(char *out, uint16_t num);

#endif
