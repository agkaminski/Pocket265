/* Pocket265 monitor
 * A.K. 2022
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <6502.h>
#include <pocket265.h>

#include "dl1414.h"
#include "keyboard.h"
#include "memory.h"

typedef enum { mode_data = 0, mode_addr } edit_mode_t;

extern uint16_t user_memsz;

void monitor_memcpy(void)
{
	dl1414_puts("\nNot implemented");
	keyboard_get();
}

void monitor_i2c_save(void)
{
	dl1414_puts("\nNot implemented");
	keyboard_get();
}

void monitor_i2c_load(void)
{
	dl1414_puts("\nNot implemented");
	keyboard_get();
}

void monitor_cpuid(void)
{
	const char *str;
	unsigned char cpu = getcpu();

	switch (cpu) {
	case CPU_6502:    str = "\n6502";    break;
	case CPU_65C02:   str = "\n65C02";   break;
	case CPU_65816:   str = "\n65816";   break;
	case CPU_4510:    str = "\n4510";    break;
	case CPU_65SC02:  str = "\n65SC02";  break;
	case CPU_65CE02:  str = "\n65CE02";  break;
	case CPU_HUC6280: str = "\nHUC6280"; break;
	case CPU_2A0x:    str = "\n2A0x";    break;
	case CPU_45GS02:  str = "\n45GS02";  break;
	default:          str = "\nUnknown"; break;
	}

	dl1414_puts(str);
	keyboard_get();
}

void monitor_memclr(void)
{
	memset((void *)USR_MEM_START, 0, user_memsz);
	dl1414_puts("\nMemclr done");
	keyboard_get();
}

void monitor_menu(void)
{
	unsigned char selection = 0;
	char buff[SCREEN_LENGTH + 2];
	unsigned char key, exit = 0;
	static const struct {
		const char *name;
		void (*f)(void);
	} items[] = {
		{ "Copy mem", monitor_memcpy },
		{ "I2C save", monitor_i2c_save },
		{ "I2C load", monitor_i2c_load },
		{ "CPU id", monitor_cpuid },
		{ "Clear mem", monitor_memclr }
	};

	while (!exit) {
		sprintf(buff, "\n%d:%s", selection + 1, items[selection]);
		dl1414_puts(buff);

		key = keyboard_get();

		switch (key) {
		case KEY_INC:
			if (++selection >= sizeof(items) / sizeof(items[0]))
				selection = 0;
			break;
		case KEY_DEC:
			if (selection == 0)
				selection = (sizeof(items) / sizeof(items[0]));
			--selection;
			break;
		case KEY_GO:
			items[selection].f();
			break;
		case KEY_SEL:
			exit = 1;
			break;
		}
	}
}

void monitor_run(void)
{
	edit_mode_t mode = mode_data;
	static const char *modeLUT[] = { " >", "< ", "+>", "+<" };
	char buff[SCREEN_LENGTH + 2];
	uint16_t address = USR_MEM_START;
	uint8_t data = read_data(address), ndata;
	unsigned char key_pressed = 0, key;
	unsigned char autoinc = 0, autoinc_cnt = 0, refresh = 1;
	struct regs ctx;

	while (1) {
		ndata = read_data(address);

		if (refresh || ndata != data) {
			data = ndata;
			sprintf(buff, "\rMON %04x%s%02x", address, modeLUT[autoinc ? mode + 2 : mode], data);
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
				monitor_menu();
				break;

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
				/* Jump into user program */
				ctx.a = 0;
				ctx.x = 0;
				ctx.y = 0;
				ctx.flags = 0;
				ctx.pc = address;
				_sys(&ctx);
				break;
			}
		}
	}
}
