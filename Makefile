.POSIX:

OBJ = xkbdbind.o
HDR = config.h

CONFIGFILE = config.mk
include $(CONFIGFILE)

all: xkbdbind
$(OBJ): $(HDR)

xkbdbind: $(OBJ) $(HDR)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

.c.o:
	$(CC) -c -o $@ $< $(CFLAGS)

install: xkbdbind
	mkdir -p -- "$(DESTDIR)$(PREFIX)/bin"
	mkdir -p -- "$(DESTDIR)$(PREFIX)/src/$(PACKAGE)"
	mkdir -p -- "$(DESTDIR)$(MANPREFIX)/man1"
	cp -- xkbdbind "$(DESTDIR)$(PREFIX)/bin/"
	cp -- $(OBJ:.o=.c) $(HDR) Makefile "$(DESTDIR)$(PREFIX)/src/$(PACKAGE)/"
	test ! -e -- "$(DESTDIR)$(PREFIX)/src/$(PACKAGE)/config.mk"
	cp -- $(CONFIGFILE) "$(DESTDIR)$(PREFIX)/src/$(PACKAGE)/config.mk"
	cp -- xkbdbind.1 "$(DESTDIR)$(MANPREFIX)/man1/"

uninstall:
	-rm -f -- "$(DESTDIR)$(PREFIX)/bin/xkbdbind"
	-rm -rf -- "$(DESTDIR)$(PREFIX)/src/$(PACKAGE)"
	-rm -f -- "$(DESTDIR)$(MANPREFIX)/man1/xkbdbind.1"

clean:
	-rm -f -- xkbdbind *.o

.SUFFIXES:
.SUFFIXES: .c .o

.PHONY: all install uninstall clean
