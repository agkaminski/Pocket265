/* Pocket265 DL1414T LED display driver
 * A.K. 2022
 */

#include <pocket265.h>
#include <ctype.h>
#include <string.h>

#include "dl1414.h"

int dl1414_puts(const char *str)
{
	static int pos = 0;
	volatile unsigned char *ptr = (void *)SCREEN_BASE;
	static char buff[SCREEN_LENGTH];
	char i, c;

	for (i = 0; str[i] != '\0'; ++i) {
		if (str[i] == '\n' || str[i] == '\r') {
			pos = 0;

			if (str[i] == '\n')
				memset(buff, 0, sizeof(buff));

			continue;
		}

		c = str[i];

		if (isalpha(c))
			c = toupper(c);

		buff[pos] = c;

		if (++pos >= SCREEN_LENGTH)
			pos = SCREEN_LENGTH - 1;
	}

	for (i = 0; i < sizeof(buff); ++i)
		ptr[SCREEN_LENGTH - 1 - i] = buff[i];

	return i;
}