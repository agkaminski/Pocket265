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
#include "i2c.h"
#include "interrupt.h"
#include "uart.h"

typedef enum { mode_data = 0, mode_addr } edit_mode_t;

extern uint16_t user_memsz;

static uint16_t g_address = USR_MEM_START;
static uint8_t g_dataPrev;
static char g_buff[SCREEN_LENGTH + 2];

struct mon_ctx {
	edit_mode_t mode;
	unsigned char autoinc;
	unsigned char autoinc_cnt;
};

static void monitor_do(unsigned char input, struct mon_ctx *monctx);

static int monitor_getu16param(const char *prompt, uint16_t *param)
{
	unsigned char key, done = 0;

	while (!done) {
		sprintf(g_buff, "\r%5s: %04x", prompt, *param);
		dl1414_puts(g_buff);

		key = keyboard_get();

		if (key <= 15) {
			*param = ((*param) << 4) | key;
		}
		else {
			switch (key) {
			case KEY_GO:
				done = 1;
				break;
			case KEY_INC:
				++(*param);
				break;
			case KEY_DEC:
				--(*param);
				break;
			case KEY_SEL:
				return -1;
			}
		}
	}

	return 0;
}

static void monitor_memcpy(void)
{
	uint16_t source = 0, len = 0;

	if (monitor_getu16param("Source", &source) < 0) {
		dl1414_puts("\nAbort");
		keyboard_get();
		return;
	}

	if (monitor_getu16param("Length", &len) < 0 || !len) {
		dl1414_puts("\nAbort");
		keyboard_get();
		return;
	}

	dl1414_puts("\nIn progress");

	memcpy((void *)g_address, (void *)source, len);

	dl1414_puts("\nMemcopy done");
	keyboard_get();
}

static void monitor_i2c_save(void)
{
	uint16_t dest = 0, len = 0;
	int ret;

	if (monitor_getu16param("Dest", &dest) < 0) {
		dl1414_puts("\nAbort");
		keyboard_get();
		return;
	}

	if (monitor_getu16param("Length", &len) < 0 || !len) {
		dl1414_puts("\nAbort");
		keyboard_get();
		return;
	}

	ret = i2c_write(dest, (void *)g_address, len);

	if (ret < 0) {
		dl1414_puts("\nI2C error");
	}
	else {
		sprintf(g_buff, "\nWrote %dB", ret);
		dl1414_puts(g_buff);
	}

	keyboard_get();
}

static void monitor_i2c_load(void)
{
	uint16_t source = 0, len = 0;
	int ret;

	if (monitor_getu16param("Source", &source) < 0) {
		dl1414_puts("\nAbort");
		keyboard_get();
		return;
	}

	if (monitor_getu16param("Length", &len) < 0 || !len) {
		dl1414_puts("\nAbort");
		keyboard_get();
		return;
	}

	ret = i2c_read(source, (void *)g_address, len);

	if (ret < 0) {
		dl1414_puts("\nI2C error");
	}
	else {
		sprintf(g_buff, "\nRead %dB", ret);
		dl1414_puts(g_buff);
	}

	keyboard_get();
}


static void monitor_uart(void)
{
	unsigned char data, key;
	struct mon_ctx ctx;

	ctx.mode = mode_data;
	ctx.autoinc = 1;
	ctx.autoinc_cnt = 0;

	dl1414_puts("\nUART 200 BN1");

	while (keyboard_get_nonblock() != KEY_NONE)
		;

	while (keyboard_get_nonblock() == KEY_NONE) {
		key = KEY_NONE;

		if (uart_rx(&data) > 0) {
			if (data >= '0' && data <= '9') {
				key = data - '0';
			}
			else if (data >= 'a' && data <= 'f') {
				key = data - 'a' + 10;
			}
			else if (data >= 'A' && data <= 'F') {
				key = data - 'A' + 10;
			}
			else if (data == '@') {
				ctx.mode = mode_addr;
				ctx.autoinc_cnt = 0;
				uart_tx(data);
				continue;
			}
			else if (data == '#') {
				ctx.mode = mode_data;
				ctx.autoinc_cnt = 0;
				uart_tx(data);
				continue;
			}
			else if (data == '+') {
				key = KEY_INC;
			}
			else if (data == '-') {
				key = KEY_DEC;
			}
			else if (data == '!') {
				uart_tx(data);
				monitor_do(KEY_GO, &ctx);
				break;
			}
			else {
				continue;
			}

			monitor_do(key, &ctx);
			uart_tx(data);
		}
	}
}

static void monitor_cpuid(void)
{
	const char *str;
	unsigned char cpu = getcpu();

	dl1414_puts("\ncpu: ");

	switch (cpu) {
	case CPU_6502:    str = "6502";    break;
	case CPU_65C02:   str = "65C02";   break;
	case CPU_65816:   str = "65816";   break;
	case CPU_4510:    str = "4510";    break;
	case CPU_65SC02:  str = "65SC02";  break;
	case CPU_65CE02:  str = "65CE02";  break;
	case CPU_HUC6280: str = "HUC6280"; break;
	case CPU_2A0x:    str = "2A0x";    break;
	case CPU_45GS02:  str = "45GS02";  break;
	default:          str = "Unknown"; break;
	}

	dl1414_puts(str);
	keyboard_get();
}

static void monitor_version(void)
{
	dl1414_puts("\n" VERSION);
	keyboard_get();
}

static void monitor_memclr(void)
{
	dl1414_puts("\nIn progress");
	memset((void *)USR_MEM_START, 0, user_memsz);
	dl1414_puts("\nMemclr done");
	keyboard_get();
}

static void monitor_menu(void)
{
	unsigned char selection = 0;
	char g_buff[SCREEN_LENGTH + 2];
	unsigned char key, exit = 0;
	static const struct {
		const char *name;
		void (*f)(void);
	} items[] = {
		{ "Copy mem", monitor_memcpy },
		{ "I2C save", monitor_i2c_save },
		{ "I2C load", monitor_i2c_load },
		{ "UART com", monitor_uart },
		{ "CPU id",   monitor_cpuid },
		{ "Version",  monitor_version },
		{ "Clr mem",  monitor_memclr }
	};

	while (!exit) {
		sprintf(g_buff, "\n%2d:%s", selection + 1, items[selection]);
		dl1414_puts(g_buff);

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
			exit = 1;
			break;
		case KEY_F2:
		case KEY_SEL:
			exit = 1;
			break;
		}
	}
}

void monitor_displayProgress(unsigned int curr, unsigned int total)
{
	unsigned int chunk = total / (SCREEN_LENGTH - 2);
	unsigned char i, segments = (curr + (chunk >> 1)) / chunk;

	g_buff[0] = '[';
	g_buff[SCREEN_LENGTH - 1] = ']';
	g_buff[SCREEN_LENGTH] = '\0';

	for (i = 1; i < SCREEN_LENGTH - 1; ++i) {
		if (i <= segments)
			g_buff[i] = '#';
		else
			g_buff[i] = ' ';
	}
	dl1414_puts(g_buff);
}

static void monitor_refresh(struct mon_ctx *monctx, unsigned char force)
{
	uint8_t ndata = read_data(g_address);
	static const char *modeLUT[] = { " >", "< ", "+>", "+<" };

	if (force || ndata != g_dataPrev) {
		g_dataPrev = ndata;
		sprintf(g_buff, "\rMON %04x%s%02x", g_address,
			modeLUT[monctx->autoinc ? monctx->mode + 2 : monctx->mode], ndata);
		dl1414_puts(g_buff);
	}
}

static void monitor_do(unsigned char input, struct mon_ctx *monctx)
{
	unsigned char data;
	struct regs ctx;

	if (input <= 15) {
		if (monctx->mode == mode_data) {
			data = read_data(g_address);
			data <<= 4;
			data |= input;
			write_data(g_address, data);

			if (monctx->autoinc) {
				if (++(monctx->autoinc_cnt) >= 2) {
					monctx->autoinc_cnt = 0;
					++g_address;
				}
			}
		}
		else {
			monctx->autoinc_cnt = 0;
			g_address = (g_address << 4) | input;
		}
	}
	else {
		monctx->autoinc_cnt = 0;

		switch (input) {
		case KEY_F1:
			monctx->autoinc = monctx->autoinc ? 0 : 1;
			break;

		case KEY_F2:
			monitor_menu();
			break;

		case KEY_F3:
		case KEY_F4:
			/* Free for the future use */
			break;

		case KEY_INC:
			++g_address;
			break;

		case KEY_DEC:
			--g_address;
			break;

		case KEY_SEL:
			monctx->mode = monctx->mode == mode_addr ? mode_data : mode_addr;
			break;

		case KEY_GO:
			/* Jump into the user program */
			ctx.a = 0;
			ctx.x = 0;
			ctx.y = 0;
			ctx.flags = 0;
			ctx.pc = g_address;
			_sys(&ctx);

			g_nmi_valid = 0;
			g_irq_valid = 0;
			break;
		}
	}
}

void monitor_run(void)
{
	unsigned char key_pressed = 0, key, refresh = 1;
	struct mon_ctx ctx;

	g_dataPrev = read_data(g_address);

	ctx.mode = mode_data;
	ctx.autoinc = 0;
	ctx.autoinc_cnt = 0;

	while (1) {
		monitor_refresh(&ctx, refresh);
		refresh = 0;

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

		monitor_do(key, &ctx);
	}
}
