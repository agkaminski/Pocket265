/* Pocket265 naive prime number calculation example
 * A.K. 2022
 */

#include <stdint.h>
#include <syscall.h>
#include <keyboard.h>

int main(void)
{
	uint16_t candidate = 3, limit, i, primeno = 1;
	int prime;

	svc_putch('\n');

	/* End loop on 16-bit wrap-around */
	while (candidate >= 3) {
		prime = 1;
		limit = candidate >> 1;

		/* Exclude 2, we consider only odd numbers */
		for (i = 3; i <= limit; i += 2) {
			if (candidate % i == 0) {
				prime = 0;
				break;
			}
		}

		if (prime) {
			++primeno;
			svc_puts("\n#");
			svc_printdU16(primeno);
			svc_puts(": ");
			svc_printdU16(candidate);
			svc_msleep(200);
		}

		candidate += 2;
		
		if (svc_getKeyNb() == KEY_SEL)
			break;
	}

	return 0;
}
