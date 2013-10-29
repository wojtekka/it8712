/*
 *  (C) Copyright 2004 Wojtek Kaniewski <wojtekka@toxygen.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License Version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include "it8712.h"
#include "main.h"

void usage(const char *argv0)
{
	printf(
"Usage: %s [OPTIONS]\n"
"\n"
"  -l, --ldn=VALUE            Select logical device (0-10 or \"list\")\n"
"  -a, --address=VALUE        Select register address (0-255)\n"
"  -B, --byte                 Byte operations (default)\n"
"  -W, --word                 Word operations\n"
"  -r, --read                 Print register value\n"
"  -w, --write=VALUE          Set register value\n"
"  -n, --and=VALUE            Bitwise AND\n"
"  -o, --or=VALUE             Bitwise OR\n"
"  -s, --bit-set=INDEX        Set bit in register\n"
"  -c, --bit-clear=INDEX      Clear bit in register\n"
"  -d, --dump                 Print all registers of all devices to stdout\n"
"\n"
"  -V, --version              Print version\n"
"  -h, --help                 This help\n"
"\n"
"See manpage for warnings, examples and known bugs.\n"
"\n", argv0);
}

void dump()
{
	unsigned char ldn;

	for (ldn = 0; ldn < 10; ldn++) {
		int base1, base2, base3, irq, dma, x, y;

		it8712_ldn_set(ldn);

		printf("LDN %d: %s", ldn, it8712_ldn_label[ldn]);

		switch (it8712_read(IT8712_INDEX_ACTIVATE)) {
			case 0:
				printf(" (disabled");
				break;
			case 1:
				printf(" (enabled");
				break;
			default:
				printf(" (not found?)\n\n");
				continue;
		}

		base1 = it8712_readw(IT8712_INDEX_BASE1);
		base2 = it8712_readw(IT8712_INDEX_BASE2);
		base3 = it8712_readw(IT8712_INDEX_BASE3);
		irq = it8712_read(IT8712_INDEX_IRQ);
		dma = it8712_read(IT8712_INDEX_DMA);

		if (base1)
			printf(", 0x%.4x", base1);

		if (base2)
			printf(", 0x%.4x", base2);

		if (base3)
			printf(", 0x%.4x", base3);

		if ((ldn < 7 || ldn == 8 || ldn == 10) && irq)
			printf(", IRQ %d", irq);

		if ((ldn == 0 || ldn == 3) && dma)
			printf(", DMA %d", dma);

		printf(")\n");

		for (y = 0; y < 16; y++) {
			printf("0x%x0: ", y);

			for (x = 0; x < 16; x++) {
				printf("%.2x ", it8712_read(y * 16 + x));
				if (x == 7)
					printf(" ");
			}

			printf("\n");
		}
			
		printf("\n");
	}
}

void it8712_check(int ldn, int addr)
{
	static int done = 0;

	if (ldn == -1 || addr == -1) {
		fprintf(stderr, "LDN and address must be specified first\n");
		it8712_close();
		exit(1);
	}
	
	if (done)
		return;
	
	if (it8712_open() == -1) {
		perror("it8712_open");
		exit(1);
	}

}

int xatoi(const char *str)
{
	char *endptr;
	int result;

	errno = 0;

	result = strtol(str, &endptr, 0);

	if (endptr == str || errno == ERANGE) {
		if (errno != ERANGE)
			errno = EINVAL;
		perror(str);
		it8712_close();
		exit(1);
	}

	return result;
}

static struct option longopts[] = {
	{ "ldn", 2, 0, 'l' },
	{ "address", 1, 0, 'a' },
	{ "byet", 0, 0, 'B' },
	{ "word", 0, 0, 'W' },
	{ "read", 0, 0, 'r' },
	{ "write", 1, 0, 'w' },
	{ "and", 1, 0, 'n' },
	{ "or", 1, 0, 'o' },
	{ "bit-set", 1, 0, 's' },
	{ "bit-clear", 1, 0, 'c' },
	{ "dump", 0, 0, 'd' },
	{ "version", 0, 0, 'V' },
	{ "help", 0, 0, 'h' },
	{ NULL, 0, 0, 0 }
};

const char *argv_irda_start[] = { "", "-l", "2", "-a", "0xf0", "-n", "0x8f", "-o", "0x10" };
const char *argv_irda_stop[] = { "", "-l", "2", "-a", "0xf0", "-n", "0x8f" };

int main(int argc, char **argv)
{
	int ldn = -1, addr = -1, word = 0, value = 0;

	if (argc < 2) {
		usage(argv[0]);
		exit(1);
	}

	if (!strcmp(argv[1], "--irda-start")) {
		argv_irda_stop[0] = argv[0];
		argv = (char**) argv_irda_start;
		argc = 9;
	}
	
	if (!strcmp(argv[1], "--irda-stop")) {
		argv_irda_stop[0] = argv[0];
		argv = (char**) argv_irda_stop;
		argc = 7;
	}
	
	for (;;) {
		int ch, longindex = 0;

		if ((ch = getopt_long(argc, argv, "l:a:BWrw:n:o:s:c:dVh", longopts, &longindex)) == -1)
			break;

		switch (ch) {
			case 'l':
				if (!strcasecmp(optarg, "list")) {
					int i;
					
					for (i = 0; it8712_ldn_label[i]; i++)
						printf("%d: %s\n", i, it8712_ldn_label[i]);
				} else
					ldn = xatoi(optarg);
				break;
			case 'a':
				addr = xatoi(optarg);
				break;
			case 'B':
				word = 0;
				break;
			case 'W':
				word = 1;
				break;
			case 'r':
				it8712_check(ldn, addr);
				it8712_ldn_set(ldn);
				if (!word)
					printf("%d\n", it8712_read(addr));
				else
					printf("%d\n", it8712_readw(addr));
				break;
			case 'w':
				it8712_check(ldn, addr);
				value = xatoi(optarg);
				it8712_ldn_set(ldn);
				if (!word)
					it8712_write(addr, value);
				else
					it8712_writew(addr, value);
				break;
			case 'n':
				it8712_check(ldn, addr);
				it8712_ldn_set(ldn);
				value = xatoi(optarg);
				if (!word)
					it8712_write(addr, it8712_read(addr) & value);
				else
					it8712_writew(addr, it8712_readw(value) & value);
				break;
			case 'o':
				it8712_check(ldn, addr);
				value = xatoi(optarg);
				it8712_ldn_set(ldn);
				if (!word)
					it8712_write(addr, it8712_read(addr) | value);
				else
					it8712_writew(addr, it8712_readw(value) | value);
				break;
			case 's':
				it8712_check(ldn, addr);
				value = xatoi(optarg);
				it8712_ldn_set(ldn);
				if (!word)
					it8712_write(addr, it8712_read(addr) | (1 << value));
				else
					it8712_writew(addr, it8712_readw(value) | (1 << value));
				break;
			case 'c':
				it8712_check(ldn, addr);
				value = xatoi(optarg);
				it8712_ldn_set(ldn);
				if (!word)
					it8712_write(addr, it8712_read(addr) & ~(1 << value));
				else
					it8712_writew(addr, it8712_readw(value) & ~(1 << value));
				break;
			case 'd':
				it8712_check(0, 0);	/* just open it */
				dump();
				break;
			case 'V':
				printf("IT8712 tweaking tool version " VERSION "\n");
				break;
			case 'h':
				usage(argv[0]);
				break;
		}
	}
	
	it8712_close();

	return 0;
}
