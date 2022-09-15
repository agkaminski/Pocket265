/* Pocket265 system calls
 * A.K. 2022
 */

#include <stdint.h>
#include <pocket265.h>

#include "timer.h"
#include "keyboard.h"
#include "dl1414.h"
#include "interrupt.h"
#include "monitor.h"
#include "print.h"

struct brk_param {
	uint8_t a;
	uint8_t x;
	uint8_t y;
	uint8_t n;
} g_brk_param;

static char g_buff[SCREEN_LENGTH + 1];

void brk_irqRegister(void)
{
	g_irq_callback = (uint16_t)g_brk_param.x | ((uint16_t)g_brk_param.y << 8);
	g_irq_valid = 1;
}

void brk_nmiRegister(void)
{
	g_nmi_callback = (uint16_t)g_brk_param.x | ((uint16_t)g_brk_param.y << 8);
	g_nmi_valid = 1;
}

void brk_putch(void)
{
	g_buff[0] = g_brk_param.a;
	g_buff[1] = '\0';

	dl1414_puts(g_buff);
}

void brk_puts(void)
{
	uint16_t address = (uint16_t)g_brk_param.x | ((uint16_t)g_brk_param.y << 8);

	dl1414_puts((void *)address);
}

void brk_putb(void)
{
	print_x8(g_buff, g_brk_param.a);
	dl1414_puts(g_buff);
}

void brk_printxU16(void)
{
	uint16_t param = (uint16_t)g_brk_param.x | ((uint16_t)g_brk_param.y << 8);

	print_x16(g_buff, param);
	dl1414_puts(g_buff);
}

void brk_printdU16(void)
{
	uint16_t param = (uint16_t)g_brk_param.x | ((uint16_t)g_brk_param.y << 8);

	print_u16(g_buff, param, 0);
	dl1414_puts(g_buff);
}

void brk_printdS16(void)
{
	uint16_t param = (uint16_t)g_brk_param.x | ((uint16_t)g_brk_param.y << 8);

	print_s16(g_buff, (int16_t)param, 0);
	dl1414_puts(g_buff);
}

void brk_scanxU16(void)
{
	uint16_t output = 0, prompt = (uint16_t)g_brk_param.x | ((uint16_t)g_brk_param.y << 8);

	monitor_getu16param((void *)prompt, &output, 0);

	g_brk_param.x = output & 0xff;
	g_brk_param.y = output >> 8;
}

void brk_scandU16(void)
{
	uint16_t output = 0, prompt = (uint16_t)g_brk_param.x | ((uint16_t)g_brk_param.y << 8);

	monitor_getu16param((void *)prompt, &output, 1);

	g_brk_param.x = output & 0xff;
	g_brk_param.y = output >> 8;
}

void brk_getKey(void)
{
	g_brk_param.a = keyboard_get();
}

void brk_getKeyNb(void)
{
	g_brk_param.a = keyboard_get_nonblock();
}

void brk_getJiffies(void)
{
	g_brk_param.a = timer_get_jiffies();
}

void brk_getSeconds(void)
{
	g_brk_param.a = timer_get_seconds();
}

void brk_msleep(void)
{
	timer_wait_ms(g_brk_param.a);
}

void brk_sleep(void)
{
	timer_wait_sec(g_brk_param.a);
}

void brk_nmiDisable(void)
{
	g_nmi_disable = 1;
	while (g_nmi_disable)
		;
}

void brk_nmiStart(void)
{
	nmi_clr();
}

void brk_handle(void)
{
	static const void (*ftable[])(void) = {
		brk_irqRegister,
		brk_nmiRegister,
		brk_putch,
		brk_puts,
		brk_putb,
		brk_printxU16,
		brk_printdU16,
		brk_printdS16,
		brk_scanxU16,
		brk_scandU16,
		brk_getKey,
		brk_getKeyNb,
		brk_getJiffies,
		brk_getSeconds,
		brk_msleep,
		brk_sleep,
		brk_nmiDisable,
		brk_nmiStart
	};

	if (g_brk_param.n < sizeof(ftable) / sizeof(ftable[0]))
		ftable[g_brk_param.n]();
}
