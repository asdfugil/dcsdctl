CC ?= cc

LIBS += -lusb-1.0
ifeq ($(shell sw_vers -productName 2> /dev/null),macOS)
LIBS += -lftdi1
else
LIBS += -lftdi
endif
CFLAGS ?= -I/opt/homebrew/include/libftdi1 -Wall -Wextra -Os
LDFLAGS ?= -L/opt/homebrew/lib

PREFIX ?= /usr/local
BINDIR ?= /bin
INSTALL ?= install

all: dcsdctl

dcsdctl: dcsdctl.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) dcsdctl.c -o dcsdctl

clean:
	rm -f dcsdctl

install: dcsdctl
	$(INSTALL) -m755 dcsdctl $(DESTDIR)$(PREFIX)$(BINDIR)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)$(BINDIR)/dcsdctl

.PHONY: all clean install uninstall
