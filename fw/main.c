/* Pocket265 main
 * A.K. 2022
 */

#include "interrupt.h"
#include "monitor.h"
#include "dl1414.h"
#include "timer.h"

int main(void)
{
	/* Kickstart NMI */
	nmi_clr();

	monitor_run();

	/* Never reached */
	return 0;
}
