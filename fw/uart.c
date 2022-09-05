/* Pocket265 UART driver
 * A.K. 2022
 */

#include "gpio.h"
#include "timer.h"

#define GET_RX        gpio_getPin(GPIO_RX)
#define SET_TX(state) gpio_setPin(GPIO_TX, state)

int uart_rx(unsigned char *data)
{
	unsigned char probe[3], i, j;

	if (GET_RX != 0)
		return 0;

	timer_wait_ms(5);

	for (i = 0; i < 8; ++i) {
		timer_wait_ms(1);

		for (j = 0; j < 3; ++j) {
			probe[j] = GET_RX;
			timer_wait_ms(1);
		}

		*data >>= 1;

		if ((probe[0] && probe[1]) ||
				(probe[1] && probe[2]) ||
				(probe[0] && probe[2])) {
			*data |= 0x80;
		}

		timer_wait_ms(1);
	}

	return 1;
}

void uart_tx(unsigned char data)
{
	unsigned char i;

	timer_wait_ms(1);
	SET_TX(0);

	timer_wait_ms(5);

	for (i = 0; i < 8; ++i) {
		SET_TX(data & 1);
		data >>= 1;
		timer_wait_ms(5);
	}

	SET_TX(1);
	timer_wait_ms(5);
}

