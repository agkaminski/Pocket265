#include <stdio.h>

#include "dl1414.h"

extern void _isr_irq(void)
{
}


extern void _isr_nmi(void)
{
}


extern void _isr_break(void)
{
}


int main(void)
{
	char str[16];

	sprintf(str, "POCKET265");

	dl1414_puts(str);

	while (1);

	return 0;
}
