/* Pocket265 lite print routines
 * A.K. 2022
 */

#include <stdint.h>

unsigned char print_string(char *out, const char *in, unsigned char min, unsigned char max)
{
	unsigned char pos = 0;

	for (pos = 0; in[pos] != '\0' && pos < max; ++pos)
		out[pos] = in[pos];

	for (; pos < min; ++pos)
		out[pos] = ' ';

	out[pos] = '\0';

	return pos;
}

unsigned char print_u16(char *out, uint16_t num, unsigned char min)
{
	unsigned char snum[5], i, skip = 1, cnt = 0;

	for (i = 0; i < 5; ++i) {
		snum[i] = num % 10;
		num /= 10;
	}

	for (i = 4; i > 0; --i) {
		if (snum[i]) {
			skip = 0;
		}
		else if (skip) {
			if (i < min) {
				*(out++) = ' ';
				++cnt;
			}

			continue;
		}

		*(out++) = snum[i] + '0';
		++cnt;
	}

	*(out++) = snum[0] + '0';
	*out = '\0';

	return cnt + 1;
}

unsigned char print_s16(char *out, int16_t num, unsigned char min)
{
	unsigned char pos = 0;

	if (num < 0) {
		*(out++) = '-';
		num = -num;
		pos = 1;
	}

	return print_u16(out, num, min) + pos;
}

static void print_x4(char *out, unsigned char nibble)
{
	if (nibble < 10)
		*out = nibble + '0';
	else
		*out = nibble + 'A' - 10;
}

unsigned char print_x8(char *out, unsigned char byte)
{
	print_x4(out, byte >> 4);
	print_x4(out + 1, byte & 0xf);
	out[2] = '\0';
	return 2;
}

unsigned char print_x16(char *out, uint16_t num)
{
	print_x8(out, num >> 8);
	print_x8(out + 2, num & 0xff);
	out[4] = '\0';
	return 4;
}
