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

#include <sys/io.h>
#include <errno.h>
#include "it8712.h"

const char *it8712_ldn_label[12] = { "FDC", "Serial Port 1", "Serial Port 2",
	"Parallel Port", "Environment Controller", "KBC (Keyboard)",
	"KBC (Mouse)", "GPIO", "MIDI Port", "Game Port", "Consumer IR", 0 };

static int it8712_opened = 0;

int it8712_open()
{
	if (ioperm(0x2e, 2, 1) == -1)
		return -1;

	outb(0x87, 0x2e);
	outb(0x01, 0x2e);
	outb(0x55, 0x2e);
	outb(0x55, 0x2e);

	if (it8712_read(0x20) != 0x87 || it8712_read(0x21) != 0x12) {
		errno = ENODEV;
		return -1;
	}

	it8712_opened = 1;

	return 0;
}

void it8712_write(unsigned char addr, unsigned char value)
{
	outb(addr, 0x2e);
	outb(value, 0x2f);
}

void it8712_writew(unsigned char addr, unsigned short value)
{
	it8712_write(addr, value >> 8);
	it8712_write(addr + 1, value & 255);
}

unsigned char it8712_read(unsigned char addr)
{
	outb(addr, 0x2e);
	return inb(0x2f);
}

unsigned short it8712_readw(unsigned char addr)
{
	return it8712_read(addr) << 8 | it8712_read(addr + 1);
}

void it8712_close()
{
	if (!it8712_opened)
		return;

	it8712_write(0x02, 0x02);
	ioperm(0x2e, 2, 0);
}

void it8712_ldn_set(unsigned char ldn)
{
	it8712_write(0x07, ldn);
}

unsigned char it8712_ldn_get()
{
	return it8712_read(0x07);
}

