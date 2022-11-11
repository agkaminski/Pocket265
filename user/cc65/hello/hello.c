/* Pocket265 Hello World! example
 * A.K. 2022
 */

#include <syscall.h>

int main(void)
{
	svc_puts("\nHello World!");
	svc_getKey();

	return 0;
}
