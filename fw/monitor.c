/* Pocket265 monitor
 * A.K. 2022
 */

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
#include "print.h"

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

void monitor_getu16param(const char *prompt, uint16_t *param, char type)
{
	unsigned char key, done = 0, pos;
	unsigned char num[6] = { 0 }, i;

	while (!done) {
		g_buff[0] = '\r';
		pos = print_string(g_buff + 1, prompt, 6, 6) + 1;
		g_buff[pos++] = ':';

		if (type) {
			for (i = 5, *param = 0; i > 0; --i) {
				*param *= 10;
				*param += num[i];
			}

			print_u16(g_buff + pos, *param, 5);
		}
		else {
			g_buff[pos++] = ' ';
			print_x16(g_buff + pos, *param);
		}
		dl1414_puts(g_buff);

		key = keyboard_get();

		if (key <= 15) {
			if (type) {
				if (key <= 9) {
					num[0] = key;

					for (i = 5, *param = 0; i > 0; --i)
						num[i] = num[i - 1];
				}
			}
			else {
				*param = ((*param) << 4) | key;
			}
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
				if (type) {
					for (i = 0; i < 5; ++i)
						num[i] = num[i + 1];

					num[5] = 0;
				}
				else {
					*param >>= 4;
				}
			}
		}
	}
}

static void monitor_printResult(const char *str, uint16_t count)
{
	unsigned char pos;
	pos = print_string(g_buff, str, 0, 7);
	pos += print_u16(g_buff + pos, count, 0);
	print_string(g_buff + pos, "B", 0, 255);
	dl1414_puts(g_buff);
}

static void monitor_memcpy(void)
{
	uint16_t source = 0, len = 0;

	monitor_getu16param("Source", &source, 0);
	monitor_getu16param("Length", &len, 0);

	dl1414_puts("\nIn progress");

	memcpy((void *)g_address, (void *)source, len);

	monitor_printResult("\nDone ", len);

	keyboard_get();
}

static void monitor_i2c_save(void)
{
	uint16_t dest = 0, len = 0;
	int ret;

	monitor_getu16param("Dest", &dest, 0);
	monitor_getu16param("Length", &len, 0);

	ret = i2c_write(dest, (void *)g_address, len);

	if (ret < 0)
		dl1414_puts("\nI2C error");
	else
		monitor_printResult("\nWrote ", len);

	keyboard_get();
}

static void monitor_i2c_load(void)
{
	uint16_t source = 0, len = 0;
	int ret;

	monitor_getu16param("Source", &source, 0);
	monitor_getu16param("Length", &len, 0);

	ret = i2c_read(source, (void *)g_address, len);

	if (ret < 0)
		dl1414_puts("\nI2C error");
	else
		monitor_printResult("\nRead ", len);

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

static void monitor_version(void)
{
	g_buff[0] = '\n';
	print_string(g_buff + 1, VERSION, 0, 12);
	dl1414_puts(g_buff);
	keyboard_get();
}

static void monitor_memclr(void)
{
	memset((void *)USR_MEM_START, 0, user_memsz);
	dl1414_puts("\nMemclr done");
	keyboard_get();
}

static void monitor_menu(void)
{
	unsigned char selection = 0;
	unsigned char key, exit = 0;
	static const struct {
		const char *name;
		void (*f)(void);
	} items[] = {
		{ "\n 1:Copy mem", monitor_memcpy },
		{ "\n 2:I2C save", monitor_i2c_save },
		{ "\n 3:I2C load", monitor_i2c_load },
		{ "\n 4:UART com", monitor_uart },
		{ "\n 5:Version",  monitor_version },
		{ "\n 6:Clr mem",  monitor_memclr }
	};

	while (!exit) {
		dl1414_puts(items[selection].name);

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
	unsigned char key_pressed = 0, key, refresh = 1, pos;
	struct mon_ctx ctx;
	uint8_t ndata;
	static const char *modeLUT[] = { " >", "< ", "+>", "+<" };

	g_dataPrev = read_data(g_address);

	ctx.mode = mode_data;
	ctx.autoinc = 0;
	ctx.autoinc_cnt = 0;

	while (1) {
		ndata = read_data(g_address);

		if (refresh || ndata != g_dataPrev) {
			g_dataPrev = ndata;

			pos = print_string(g_buff, "\rMON ", 0, 255);
			pos += print_x16(g_buff + pos, g_address);
			pos += print_string(g_buff + pos, modeLUT[ctx.autoinc ? ctx.mode + 2 : ctx.mode], 2, 2);
			print_x8(g_buff + pos, ndata);
			dl1414_puts(g_buff);
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

		monitor_do(key, &ctx);
	}
}
