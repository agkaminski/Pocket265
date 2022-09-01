/* Pocket265 software timer
 * A.K. 2022
 */

#include <pocket265.h>

volatile unsigned int jiffies;
volatile unsigned char seconds;

unsigned char timer_get_jiffies(void)
{
	/* Will get only low byte, so it's ok to share with NMI */
	return jiffies;
}

unsigned char timer_get_seconds(void)
{
	return seconds;
}

void timer_wait_ms(unsigned char ms)
{
	unsigned char start = timer_get_jiffies(), curr;

	do {
		curr = timer_get_jiffies();
	} while (curr - start < ms);
}

void timer_wait_sec(unsigned char sec)
{
	int i;

	for (i = 0; i < (sec << 3); ++i)
		timer_wait_ms(128);
}

