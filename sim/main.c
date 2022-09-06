#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include "common/threads.h"
#include "common/error.h"
#include "core/core.h"
#include "parser/parser.h"
#include "bus/memory.h"
#include "bus/bus.h"

#define DEFAULT_FREQ 1000000

void usage(char *progname)
{
	printf("Usage: %s [options]\n", progname);
	printf("Options:\n");
	printf("  -f <file>    Initialize RAM with intel hex, srec or binary file.\n");
	printf("  -o <offset>  Put data from file begging at offet <offset>.\n");
	printf("  -s <freq>    Set core clock frequency to <freq> Hz (default 1 MHz)\n");
	printf("  -h           This help.\n");
}


volatile u8 screen[12];
u8 key_row[6];
volatile u8 nmi_dis = 1;


void screen_write(u16 addr, u8 data)
{
	u16 pos = addr & 0xf;

	if (data == '\0')
		data = ' ';

	if (pos < sizeof(screen))
		screen[sizeof(screen) - 1 - pos] = data;
}


u8 screen_read(u16 addr)
{
	return 0xff;
}


void keyboard_write(u16 addr, u8 data)
{
}


u8 keyboard_read(u16 addr)
{
	u16 i;
	u8 ret = 0xff;

	for (i = 0; i < 6; ++i) {
		if (!((1 << i) & addr)) {
			ret = key_row[i] | 0xf0;
		}
	}

	DEBUG("%04x %02x\n", addr, ret);

	return ret;
}


void nmiclr_write(u16 addr, u8 data)
{
	nmi_dis = 0;
}


u8 nmiclr_read(u16 addr)
{
	nmi_dis = 0;
	return 0xff;
}


void *keyboard_thread(void *arg)
{
	int c, pressed;

	static struct termios oldt, newt;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	while (1) {
		c = getchar();

		if (isalpha(c))
			c = tolower(c);

		pressed = 1;

		switch (c) {
			case '0': key_row[4] = 0xfe; break;
			case '1': key_row[4] = 0xfd; break;
			case '2': key_row[4] = 0xfb; break;
			case '3': key_row[4] = 0xf7; break;

			case '4': key_row[3] = 0xfe; break;
			case '5': key_row[3] = 0xfd; break;
			case '6': key_row[3] = 0xfb; break;
			case '7': key_row[3] = 0xf7; break;

			case '8': key_row[2] = 0xfe; break;
			case '9': key_row[2] = 0xfd; break;
			case 'a': key_row[2] = 0xfb; break;
			case 'b': key_row[2] = 0xf7; break;

			case 'c': key_row[1] = 0xfe; break;
			case 'd': key_row[1] = 0xfd; break;
			case 'e': key_row[1] = 0xfb; break;
			case 'f': key_row[1] = 0xf7; break;

			case 'h': key_row[5] = 0xfe; break; /* INC */
			case 'j': key_row[5] = 0xfd; break; /* DEC */
			case 'k': key_row[5] = 0xfb; break; /* SEL */
			case 'l': key_row[5] = 0xf7; break; /* GO */

			case 'u': key_row[0] = 0xfe; break; /* F1 */
			case 'i': key_row[0] = 0xfd; break; /* F2 */
			case 'o': key_row[0] = 0xfb; break; /* F3 */
			case 'p': key_row[0] = 0xf7; break; /* F4 */
			default:  pressed = 0; break;
		}

		if (pressed) {
			usleep(100 * 1000);
			memset(key_row, 0xff, sizeof(key_row));
		}
	}

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}



void pocket265(void)
{
	busentry_t busentry;
	thread_t keyb_thread;

	memset(key_row, 0xff, sizeof(key_row));

	/* screen */
	busentry.begin = 0xc400;
	busentry.end = 0xc7ff;
	busentry.write = screen_write;
	busentry.read = screen_read;

	bus_register(busentry);

	/* keyboard */
	busentry.begin = 0xc000;
	busentry.end = 0xc3ff;
	busentry.write = keyboard_write;
	busentry.read = keyboard_read;

	bus_register(busentry);

	/* NMI clr */
	busentry.begin = 0xc800;
	busentry.end = 0xcbff;
	busentry.write = nmiclr_write;
	busentry.read = nmiclr_read;

	bus_register(busentry);

	core_run();

	thread_create(&keyb_thread, keyboard_thread, NULL);

	while (1) {
		printf("\033[2J\x1B[H[%12s]\n\n"
		       "[ F1  ][ F2  ][ F3  ][ F4  ]\n"
		       "[ C   ][ D   ][ E   ][ F   ]\n"
		       "[ 8   ][ 9   ][ A   ][ B   ]\n"
		       "[ 4   ][ 5   ][ 6   ][ 7   ]\n"
		       "[ 0   ][ 1   ][ 2   ][ 3   ]\n"
		       "[ INC ][ DEC ][ SEL ][ GO  ]\n", screen);

		usleep(1 * 1000);
		if (!nmi_dis) {
			nmi_dis = 1;
			core_nmi();
		}
	}
}


int main(int argc, char *argv[])
{
	cycles_t cycles;
	u16 offset = 0;
	u8 *rom = NULL;
	char path[128];
	int c;
	int freq = DEFAULT_FREQ;

	memset(path, '\0', sizeof(path));

	while ((c = getopt(argc, argv, "o:f:s:hv")) != -1) {
		switch (c) {
			case 'o':
				offset = strtol(optarg, NULL, 0);
				break;

			case 'f':
				strncpy(path, optarg, sizeof(path) - 1);
				path[sizeof(path) - 1] = '\0';
				break;

			case 's':
				freq = strtol(optarg, NULL, 10);
				if (freq == 0) {
					WARN("Invalid frequency. Falling back to default.");
					freq = DEFAULT_FREQ;
				}
				break;

			case 'h':
				printf("AK6502 CPU simulator. Made by Aleksander Kaminski in 2017.\n");
				usage(argv[0]);
				return 0;

			case 'v':
				printf("AK6502 CPU simulator. Made by Aleksander Kaminski 2017.\n");
				printf("Version: %s\n", VERSION);
				return 0;

			case '?':
				if (optopt == 'o' || optopt == 'f')
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint(optopt))
					fprintf(stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
			default:
				usage(argv[0]);
				return -1;
		}
	}

	if (path[0] != '\0') {
		INFO("Initializing RAM from %s file at offset 0x%04x", path, offset);

		if ((rom = malloc(65536)) == NULL)
			FATAL("Out of memory!");

		if (parse_file(path, offset, rom, 65536) < 0) {
			WARN("Could not parse initialization file");
			free(rom);
			rom = NULL;
		}
	}

	if (rom == NULL) {
		FATAL("Memory has not been initialized");
	}

	memory_init(rom);
	free(rom);

	core_init();
	core_setSpeed(freq);

	INFO("Resetting CPU...");
	core_rst();
	INFO("Ready.");

	/* Pocket65 code here */
	pocket265();

	core_getState(NULL, &cycles);
	INFO("Ending simulation. CPU took total of %u cycles.", cycles);

	return 0;
}
