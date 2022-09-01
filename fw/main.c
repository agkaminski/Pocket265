#include <stdio.h>

#include "dl1414.h"
#include "keyboard.h"
#include "interrupt.h"

int main(void)
{
	char str[16];
	unsigned char key;
	static const char *keys[] = { "INC", "DEC", "SEL", "GO", "F1", "F2", "F3", "F4" };

	dl1414_puts("\nPOCKET265");

	nmi_clr();

	while (1) {
		key = keyboard_get();

		if (key <= 15)
			sprintf(str, "\nKey: 0x%x", key);
		else
			sprintf(str, "\nKey: %s", keys[key - 16]);

		dl1414_puts(str);
	}

	return 0;
}
