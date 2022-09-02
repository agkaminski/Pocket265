/* Pocket265 main
 * A.K. 2022
 */

#include <stdint.h>
#include <stdio.h>
#include <pocket265.h>

#include "interrupt.h"
#include "monitor.h"
#include "dl1414.h"
#include "timer.h"
#include "memtest.h"
#include "gpio.h"

uint16_t user_memsz;

int main(void)
{
	static const char copyright[] = "version " VERSION " A.K.    2022";
	char buff[SCREEN_LENGTH + 2];
	unsigned char i;

	gpio_init();

	/* Kickstart NMI */
	nmi_clr();

	dl1414_puts("\nPocket265   ");
	timer_wait_sec(1);

	buff[1] = '\0';
	for (i = 0; i < sizeof(copyright) - 1; ++i) {
		buff[0] = copyright[i];
		dl1414_puts(buff);
		timer_wait_ms(120);
	}

	/* memory test provide big enough delay... */
	user_memsz = USR_MEM_START;
	memory_test(&user_memsz);
	user_memsz -= USR_MEM_START;

	sprintf(buff, "\n%5d B free", user_memsz);
	dl1414_puts(buff);
	timer_wait_sec(1);

	monitor_run();

	/* Never reached */
	return 0;
}

