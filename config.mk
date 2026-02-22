PREFIX    = /usr
MANPREFIX = $(PREFIX)/share/man

PACKAGE = xkbdbind

CC = c99

CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
CFLAGS   =
LDFLAGS  = -lxcb -lxcb-keysyms
