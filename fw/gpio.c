/* Pocket265 GPIO driver
 * A.K. 2022
 */

#include <pocket265.h>

unsigned char g_gpio_output;

void gpio_setPin(char pin, char state)
{
	volatile unsigned char *ptr = (void *)GPIO_BASE;

	if (state)
		g_gpio_output |= (1 << pin);
	else
		g_gpio_output &= ~(1 << pin);

	*ptr = g_gpio_output;
}

char gpio_getPin(char pin)
{
	volatile unsigned char *ptr = (void *)GPIO_BASE;

	return !!(*ptr & (1 << pin));
}

void gpio_init(void)
{
	volatile unsigned char *ptr = (void *)GPIO_BASE;

	*ptr = 0xf;
	g_gpio_output = 0xf;
}

