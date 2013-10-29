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
#include <errno.h>
#include <getopt.h>
#include <sys/io.h>
#include "it8712.h"
#include "main.h"

int ec_addr = 0;

int ec_open()
{
	if (ioperm(ec_addr + 5, 2, 1) == -1) {
		perror("ioperm");
		exit(1);
	}

	return 0;
}

unsigned char ec_read(unsigned char addr)
{
	outb(addr, ec_addr + 5);
	return inb(ec_addr + 6);
}

float ec_vin(unsigned char i)
{
	return 0.016 * ((float) ec_read(0x20 + i));
}

void ec_close()
{
	ioperm(ec_addr + 5, 2, 0);
}

void dump()
{
	unsigned char x, y;

	ec_open();

	for (y = 0; y < 8; y++) {
		printf("0x%x0: ", y);

		for (x = 0; x < 16; x++) {
			printf("%.2x ", ec_read(y * 16 + x));
			if (x == 7)
				printf(" ");
		}

		printf("\n");
	}
			
	ec_close();
}

void usage(const char *argv0)
{
	printf(
"Usage: %s [OPTIONS]\n"
"\n"
"  -d, --dump                 Print all registers to stdout\n"
"\n"
"  -V, --version              Print version\n"
"  -h, --help                 This help\n"
"\n"
"By default all available data is printed.\n"
"\n", argv0);
}

static struct option longopts[] = {
	{ "dump", 0, 0, 'd' },
	{ "version", 0, 0, 'V' },
	{ "help", 0, 0, 'h' },
	{ NULL, 0, 0, 0 }
};

int main(int argc, char **argv)
{
	if (it8712_open() == -1) {
		perror("it8712_open");
		return 1;
	}

	it8712_ldn_set(4);

	if (!(it8712_read(0x30) & 1)) {
		fprintf(stderr, "Environment Controller not enabled\n");
		return 1;
	}

	if (!(ec_addr = it8712_readw(0x60))) {
		fprintf(stderr, "Environment Controller base address not set\n");
		return 1;
	}

	printf("ec_addr = 0x%.x\n", ec_addr);

	for (;;) {
		int ch, longindex = 0;

		if ((ch = getopt_long(argc, argv, "dVh", longopts, &longindex)) == -1)
			break;

		switch (ch) {
			case 'd':
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

	if (argc == 1) {
		ec_open();
	
		printf("Vcore1 = %.3f\n", ec_vin(0));
		printf("Vcore2 = %.3f\n", ec_vin(1));
		printf("Vcc3 = %.3f\n", ec_vin(2));
		printf("Vcc = %.3f\n", ec_vin(3) * 1.68);
		printf("+12V = %.3f\n", ec_vin(4) * 4);
		printf("-12V = %.3f\n", ec_vin(5) * (1+232/56) - (232/56)*4.096);
		printf("-5V = %.3f\n", ec_vin(6) * (1+120/56) - (120/56)*4.096);
		printf("VccH = %.3f\n", ec_vin(7) * 1.68);

		ec_close();
	}
	
	it8712_close();

	return 0;
}
