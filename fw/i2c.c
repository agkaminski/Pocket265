/* Pocket265 I2C bus 24xx memory driver
 * A.K. 2022
 */

#include <stdint.h>

#include "gpio.h"
#include "timer.h"
#include "monitor.h"

/* M24256 */
#define PAGESZ 32
#define MEMSZ (32 * 1024)
#define DEVADDR 0xa0

#define SDA_ON gpio_setPin(GPIO_SDA, 1)
#define SDA_OFF gpio_setPin(GPIO_SDA, 0)
#define SCL_ON gpio_setPin(GPIO_SCL, 1)
#define SCL_OFF gpio_setPin(GPIO_SCL, 0)
#define SDA_READ gpio_getPin(GPIO_SDA)
#define SCL_READ gpio_getPin(GPIO_SCL)

static void i2c_generateStart(void)
{
	SCL_ON;
	SDA_ON;
	SDA_OFF;
	SCL_OFF;
}


static void i2c_generateStop(void)
{
	SDA_OFF;
	SCL_ON;
	SDA_ON;
}

static int i2c_sendByte(uint8_t byte)
{
	char i;
	uint8_t ack;

	for (i = 0; i < 8; ++i) {
		if (byte & 0x80)
			SDA_ON;
		else
			SDA_OFF;

		byte <<= 1;
		SCL_ON;
		while (!SCL_READ);
		SCL_OFF;
	}

	SDA_ON;
	SCL_ON;
	while (!SCL_READ);
	ack = SDA_READ;
	SCL_OFF;

	return ack ? -1 : 0;
}

static int i2c_getByte(uint8_t *byte, uint8_t ack)
{
	char i;

	SDA_ON;

	for (i = 0; i < 8; ++i) {
		SCL_ON;
		while (!SCL_READ);

		*byte = ((*byte) << 1) | !!SDA_READ;

		SCL_OFF;
	}

	if (ack)
		SDA_OFF;

	SCL_ON;
	while (!SCL_READ);
	SCL_OFF;

	return 0;
}

int i2c_write(uint16_t address, uint8_t *data, uint16_t len)
{
	uint16_t i;
	char retry;

	if (address >= MEMSZ)
		return 0;

	if (address + len > MEMSZ)
		len = MEMSZ - address;

	for (i = 0; i < len; ++i) {
		i2c_generateStart();

		for (retry = 20; retry > 0; --retry) {
			if (i2c_sendByte(DEVADDR) >= 0)
				break;

			timer_wait_ms(1);
			i2c_generateStart();
		}

		if (retry <= 0) {
			i2c_generateStop();
			return -1;
		}

		if (i2c_sendByte((address + i) >> 8) < 0) {
			i2c_generateStop();
			return -1;
		}
		if (i2c_sendByte((address + i) & 0xff) < 0) {
			i2c_generateStop();
			return -1;
		}

		if (i2c_sendByte(data[i]) < 0) {
			i2c_generateStop();
			return -1;
		}

		i2c_generateStop();

		monitor_displayProgress(i, len);
	}

	return len;
}

int i2c_read(uint16_t address, uint8_t *data, uint16_t len)
{
	uint16_t i;
	char retry;

	if (address >= MEMSZ)
		return 0;

	if (address + len > MEMSZ)
		len = MEMSZ - address;

	i2c_generateStart();

	for (retry = 10; retry > 0; --retry) {
		if (i2c_sendByte(DEVADDR) >= 0)
			break;

		timer_wait_ms(1);
		i2c_generateStart();
	}

	if (retry <= 0) {
		i2c_generateStop();
		return -1;
	}

	if (i2c_sendByte(address >> 8) < 0) {
		i2c_generateStop();
		return -1;
	}
	if (i2c_sendByte(address & 0xff) < 0) {
		i2c_generateStop();
		return -1;
	}

	i2c_generateStart();

	for (retry = 10; retry > 0; --retry) {
		if (i2c_sendByte(DEVADDR | 1) >= 0)
			break;

		timer_wait_ms(1);
		i2c_generateStart();
	}

	if (retry <= 0) {
		i2c_generateStop();
		return -1;
	}

	for (i = 0; i < len - 1; ++i) {
		i2c_getByte(&data[i], 1);
		monitor_displayProgress(i, len);
	}

	i2c_getByte(&data[len - 1], 0);

	i2c_generateStop();

	return len;
}
