/* Pocket265 DL1414T LED display driver
 * A.K. 2022
 */

#include "bsp/pocket265.h"
#include "dl1414.h"

static char buff[SCREEN_LENGTH];

static void dl1414_refresh(void)
{
	unsigned char *ptr = (void *)SCREEN_BASE;
	int i;

	for (i = 0; i < sizeof(buff); ++i)
		ptr[SCREEN_LENGTH - 1 - i] = buff[i];
}

int dl1414_puts(const char *str)
{
	static int pos = 0;
	int i;

	for (i = 0; str[i] != '\0'; ++i) {
		if (str[i] == '\n' || str[i] == '\r')
			pos = 0;
		else
			buff[pos] = str[i];

		if (++pos >= SCREEN_LENGTH)
			pos = SCREEN_LENGTH - 1;
	}

	dl1414_refresh();

	return i;
}
