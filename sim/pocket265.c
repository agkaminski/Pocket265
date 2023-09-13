/* Pocket265 SBC simulator
 * Copyright A.K. 2023
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <time.h>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <simak65.h>

#define RAM_START 0x0000u
#define RAM_SIZE  0x2000u

#define ROM_START 0xe000u
#define ROM_SIZE  0x2000u

#define PERIPH_ADDR_MASK  0xfc00u
#define PERIPH_ADDR_START 0xc000u
#define PERIPH_ADDR_END   0xcfffu

#define KEYBOARD_ADDR 0xc000u
#define SCREEN_ADDR   0xc400u
#define NMI_ACK_ADDR  0xc800u
#define GPIO_ADDR     0xcc00u

static uint8_t *g_ram;
static size_t g_ramsz = RAM_SIZE;
static uint8_t g_rom[ROM_SIZE];

static uint8_t g_gpio_out;
static char g_screen[12];
static int g_screen_update = 1;
static uint8_t g_key_row[6];
static int g_nmi = 1;
static volatile int g_exit = 0;

static void pocket265_nmi_ack(void)
{
	g_nmi = 0;
}

static uint8_t pocket265_keyread(uint16_t addr)
{
	uint8_t byte = 0xff;

	for (size_t i = 0; i < sizeof(g_key_row) / sizeof(*g_key_row); ++i) {
		if (!(addr & (1 << i)))
			byte &= g_key_row[i];
	}

	return byte;
}

static uint8_t pocket265_ioread(uint16_t addr)
{
	switch (addr & PERIPH_ADDR_MASK) {
		case KEYBOARD_ADDR:
			return pocket265_keyread(addr);
		case NMI_ACK_ADDR:
			pocket265_nmi_ack();
			break;
		case GPIO_ADDR:
			return g_gpio_out & 0xf;
	}

	return 0xff;
}

static void pocket265_iowrite(uint16_t addr, uint8_t byte)
{
	switch (addr & PERIPH_ADDR_MASK) {
		case SCREEN_ADDR:
			if ((addr & 0xf) < 12) {
				g_screen[11 - (addr & 0xf)] = byte;
				g_screen_update = 1;
			}
			break;
		case NMI_ACK_ADDR:
			pocket265_nmi_ack();
			break;
		case GPIO_ADDR:
			g_gpio_out = byte & 0xf;
			break;
	}
}

static int pocket265_is_rom_wp(void)
{
	return (g_gpio_out & (1 << 3)) ? 1 : 0;
}

static uint8_t pocket265_read(uint16_t addr)
{
	if (addr >= RAM_START && addr < (RAM_START + g_ramsz))
		return g_ram[addr - RAM_START];
	else if (addr >= ROM_START && addr < (ROM_START + ROM_SIZE))
		return g_rom[addr - ROM_START];
	else if (addr >= PERIPH_ADDR_START && addr <= PERIPH_ADDR_END)
		return pocket265_ioread(addr);
	else
		return 0xff;
}

static void pocket265_write(uint16_t addr, uint8_t byte)
{
	if (addr >= RAM_START && addr < (RAM_START + g_ramsz))
		g_ram[addr - RAM_START] = byte;
	else if (addr >= ROM_START && addr < (ROM_START + ROM_SIZE) && !pocket265_is_rom_wp())
		g_rom[addr - ROM_START] = byte;
	else if (addr >= PERIPH_ADDR_START && addr <= PERIPH_ADDR_END)
		pocket265_iowrite(addr, byte);
}

static void ui_process_key(char c)
{
	if (isalpha(c))
		c = tolower(c);

	memset(g_key_row, 0xff, sizeof(g_key_row));

	switch (c) {
		case '0': g_key_row[4] = 0xfe; break;
		case '1': g_key_row[4] = 0xfd; break;
		case '2': g_key_row[4] = 0xfb; break;
		case '3': g_key_row[4] = 0xf7; break;

		case '4': g_key_row[3] = 0xfe; break;
		case '5': g_key_row[3] = 0xfd; break;
		case '6': g_key_row[3] = 0xfb; break;
		case '7': g_key_row[3] = 0xf7; break;

		case '8': g_key_row[2] = 0xfe; break;
		case '9': g_key_row[2] = 0xfd; break;
		case 'a': g_key_row[2] = 0xfb; break;
		case 'b': g_key_row[2] = 0xf7; break;

		case 'c': g_key_row[1] = 0xfe; break;
		case 'd': g_key_row[1] = 0xfd; break;
		case 'e': g_key_row[1] = 0xfb; break;
		case 'f': g_key_row[1] = 0xf7; break;

		case 'h': g_key_row[5] = 0xfe; break; /* INC */
		case 'j': g_key_row[5] = 0xfd; break; /* DEC */
		case 'k': g_key_row[5] = 0xfb; break; /* SEL */
		case 'l': g_key_row[5] = 0xf7; break; /* GO */

		case 'u': g_key_row[0] = 0xfe; break; /* F1 */
		case 'i': g_key_row[0] = 0xfd; break; /* F2 */
		case 'o': g_key_row[0] = 0xfb; break; /* F3 */
		case 'p': g_key_row[0] = 0xf7; break; /* F4 */
		
		case 'q': g_exit = 1; break;
	}
}

static void ui_draw(void)
{
	if (g_screen_update) {
		printf("\033[2J\x1B[H[%12s]\n\n"
			"\r[ F1  ][ F2  ][ F3  ][ F4  ]\n"
			"\r[ C   ][ D   ][ E   ][ F   ]\n"
			"\r[ 8   ][ 9   ][ A   ][ B   ]\n"
			"\r[ 4   ][ 5   ][ 6   ][ 7   ]\n"
			"\r[ 0   ][ 1   ][ 2   ][ 3   ]\n"
			"\r[ INC ][ DEC ][ SEL ][ GO  ]\n\r", g_screen);

		g_screen_update = 0;
	}
}

static useconds_t gettime_us(void)
{
	struct timespec now;

	clock_gettime(CLOCK_MONOTONIC, &now);

	return (now.tv_sec * 1000 * 1000) + ((now.tv_nsec + 500) / 1000);
}

static void usage(const char *p)
{
	printf("usage: %s -c <path to firmware binary> [-r <RAM size in bytes> ] [-f <cpu frequency in Hz 0-20000000>]\n", p);
}

static void sighandler(int n)
{
    (void)n;

    g_exit = 1;
}

int main(int argc, char *argv[])
{
	int c;
	FILE *firmware = NULL;
	struct simak65_cpustate cpu;
	const struct simak65_bus ops = { .read = pocket265_read, .write = pocket265_write };
	unsigned int cycles = 0;
	unsigned int frequency = 2 * 1000 * 1000;
	useconds_t prev, now, prev_nmi = 0, prev_ui = 0, ns_per_cycle = 0;
	int ns_error = 0;
	static struct termios oldt, newt;

	while ((c = getopt(argc, argv, "c:f:r:h")) != -1) {
		switch (c) {
			case 'c':
				if ((firmware = fopen(optarg, "r")) == NULL) {
					fprintf(stderr, "can't open firmare file %s\n", optarg);
					return 1;
				}
				break;
			case 'r':
				g_ramsz = strtoul(optarg, NULL, 0);
				if (g_ramsz == 0) {
					fprintf(stderr, "invalid RAM size\n");
					return 1;
				}
				break;
			case 'f':
				errno = 0;
				frequency = strtoul(optarg, NULL, 10);
				if (errno != 0 || frequency > 20 * 1000 * 1000) {
					fprintf(stderr, "invalid frequency %u\n", frequency);
					return 1;
				}
				break;
			case 'h':
				usage(argv[0]);
				return 0;
			default:
				usage(argv[0]);
				return 1;
		}
	}

	if (frequency != 0)
		ns_per_cycle = (1000 * 1000 * 1000) / frequency;

	if (firmware == NULL) {
		usage(argv[0]);
		return 1;
	}

	if (fread(g_rom, 1, sizeof(g_rom), firmware) == 0) {
		fprintf(stderr, "failed to read the firmware file\n");
		return 1;
	}

	fclose(firmware);

	g_ram = malloc(g_ramsz);
	if (g_ram == NULL) {
		perror("failed to allocate RAM");
		return 1;
	}

	signal(SIGINT, sighandler);

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	cfmakeraw(&newt);
	newt.c_lflag |= ISIG;
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

	prev = gettime_us();

	simak65_init(&cpu, &ops);
	simak65_rst(&cpu, &cycles);

	while (!g_exit) {
		simak65_step(&cpu, &cycles);

		now = gettime_us();

		/* Handle 1 ms NMI tick */
		if (now - prev_nmi >= 1000) {
			prev_nmi = now;
			if (!g_nmi) {
				simak65_nmi(&cpu, &cycles);
				g_nmi = 1;
			}
			now = gettime_us();
		}

		/* Handle simulator UI */
		if (now - prev_ui > 50 * 1000) {
			prev_ui = now;

			ui_process_key(getchar());
			ui_draw();

			now = gettime_us();
		}

		/* Handle speed of the simulation */
		if (frequency != 0) {
			ns_error += cycles * ns_per_cycle - ((now - prev) * 1000);
			cycles = 0;
			if (ns_error >= 1000) {
				useconds_t to_sleep = ns_error / 1000;
				ns_error -= to_sleep * 1000;
				usleep(to_sleep);
			}
		}

		prev = now;
	}

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	free(g_ram);

	return 0;
}
