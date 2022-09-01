#include <stdio.h>
#include <stdint.h>
#include <pocket265.h>

#include "dl1414.h"
#include "keyboard.h"
#include "interrupt.h"

typedef enum { mode_data = 0, mode_addr } edit_mode_t;

static uint8_t read_data(uint16_t addr)
{
	return *((volatile uint8_t *)addr);
}

static void write_data(uint16_t addr, uint8_t data)
{
	*((volatile uint8_t *)addr) = data;
}

int main(void)
{
	edit_mode_t mode = mode_data;
	static const char *modeLUT[] = { " >", "< ", "+>", "+<" };
	char buff[SCREEN_LENGTH + 2];
	uint16_t address = USR_MEM_START;
	uint8_t data = read_data(address), ndata;
	unsigned char key_pressed = 0, key;
	unsigned char autoinc = 0, autoinc_cnt = 0, refresh = 1;

	/* Kickstart NMI */
	nmi_clr();

	while (1) {
		ndata = read_data(address);

		if (refresh || ndata != data) {
			data = ndata;
			sprintf(buff, "\rMEM %04x%s%02x", address, modeLUT[autoinc ? mode + 2 : mode], data);
			dl1414_puts(buff);
			refresh = 0;
		}

		key = keyboard_get_nonblock();

		if (key == KEY_NONE) {
			if (key_pressed)
				key_pressed = 0;
			continue;
		}

		if (key_pressed)
			continue;

		key_pressed = 1;

		refresh = 1;

		if (key <= 15) {
			if (mode == mode_data) {
				data = read_data(address);
				data <<= 4;
				data |= key;
				write_data(address, data);

				if (autoinc) {
					if (++autoinc_cnt >= 2) {
						autoinc_cnt = 0;
						++address;
					}
				}
			}
			else {
				autoinc_cnt = 0;
				address = (address << 4) | key;
			}
		}
		else {
			autoinc_cnt = 0;

			switch (key) {
			case KEY_F1:
				autoinc = autoinc ? 0 : 1;
				break;

			case KEY_F2:
			case KEY_F3:
			case KEY_F4:
				/* TODO */
				break;

			case KEY_INC:
				++address;
				break;

			case KEY_DEC:
				--address;
				break;

			case KEY_SEL:
				mode = mode == mode_addr ? mode_data : mode_addr;
				break;

			case KEY_GO:
				/* TODO */
				break;
			}
		}
	}

	/* Never reached */
	return 0;
}
