/* Pocket265 keyboard driver
 * A.K. 2022
 */

#include <pocket265.h>

#include "dl1414.h"

#define COOLDOWN 30

volatile unsigned char key_curr = KEY_NONE;
volatile unsigned char key_prev = KEY_NONE;
volatile unsigned char key_cooldown = 0;

unsigned char keyboard_get_nonblock(void)
{
	return key_curr;
}
#include <stdio.h>
unsigned char keyboard_get(void)
{
	unsigned char key;

	while (key_curr != KEY_NONE);

	dl1414_puts("\ngot none");

	while ((key = key_curr) == KEY_NONE);

	return key;
}

/* bad keys 4, 5, 1, 2, 3, dec */
