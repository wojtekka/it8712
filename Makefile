CC = gcc
CFLAGS = -Wall
BIN = it8712
MAN = it8712.8
OBJS = it8712.o main.o

PREFIX = $(DESTDIR)/usr/local
SBINDIR = $(PREFIX)/sbin
MANDIR = $(PREFIX)/share/man

VERSION = $(shell grep VERSION main.h | sed -e 's/^[^"]*"//' -e 's/".*//')

default:	$(BIN) ec

it8712:	$(OBJS)

ec:	ec.o it8712.o

.PHONY:	clean

clean:
	rm -rf *.o $(BIN)

.PHONY:	install

install:	$(BIN)
	install -d $(SBINDIR)
	install -m 755 $(BIN) $(SBINDIR)
	install -d $(MANDIR)/man8
	install -m 644 $(MAN) $(MANDIR)/man8

.PHONY:	tarball

tarball:	clean
	cd ..; mv it8712 it8712-$(VERSION); tar zcvf it8712-$(VERSION)/it8712-$(VERSION).tar.gz --exclude it8712-$(VERSION)/it8712-$(VERSION).tar.gz it8712-$(VERSION); mv it8712-$(VERSION) it8712
	
