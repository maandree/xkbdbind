.POSIX:

CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
CFLAGS   = -std=c99 -Wall -Wextra -pedantic -O2 $(CPPFLAGS)
LDFLAGS  = -s -lxcb -lxcb-keysyms

all: xkbdbind

xkbdbind: xkbdbind.c config.h
	$(CC) -o $@ $@.c $(CFLAGS) $(LDFLAGS)

clean:
	-rm -f -- xkbdbind *.o

.SUFFIXES:

.PHONY: all clean
