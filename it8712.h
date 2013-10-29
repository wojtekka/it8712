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

#ifndef __IT8712_H
#define __IT8712_H

int it8712_open();
void it8712_close();

const char *it8712_ldn_label[12];

#define IT8712_INDEX_ACTIVATE 0x30
#define IT8712_INDEX_BASE1 0x60
#define IT8712_INDEX_BASE2 0x62
#define IT8712_INDEX_BASE3 0x64
#define IT8712_INDEX_IRQ 0x70
#define IT8712_INDEX_DMA 0x74

unsigned char it8712_read(unsigned char addr);
unsigned short it8712_readw(unsigned char addr);
void it8712_write(unsigned char addr, unsigned char value);
void it8712_writew(unsigned char addr, unsigned short value);
void it8712_ldn_set(unsigned char ldn);
unsigned char it8712_ldn_get();

#endif /* __IT8712_H */
