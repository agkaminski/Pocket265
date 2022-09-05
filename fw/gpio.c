/* Pocket265 GPIO driver
 * A.K. 2022
 */

#include <pocket265.h>

unsigned char g_gpio_output;

void gpio_setPin(char pin, char state)
{
	if (state)
		g_gpio_output |= (1 << pin);
	else
		g_gpio_output &= ~(1 << pin);

	*((volatile unsigned char *)GPIO_BASE) = g_gpio_output;
}

char gpio_getPin(char pin)
{
	return (*((volatile unsigned char *)GPIO_BASE) & (1 << pin)) ? 1 : 0;
}

void gpio_init(void)
{
	*((volatile unsigned char *)GPIO_BASE) = 0xf;
	g_gpio_output = 0xf;
}

