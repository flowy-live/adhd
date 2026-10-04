CC = cc
CFLAGS = -g -Wall -Iinclude -I/opt/homebrew/include
SOURCES = src/main.c src/state.c
OBJECTS = src/main.o src/state.o
LDFLAGS = -L/opt/homebrew/lib
LDLIBS = -lraylib -lm -lsqlite3
HEADERS = \
	include/raygui.h include/common.h include/state.h

PREFIX ?= /usr/local
BINDIR := $(PREFIX)/bin

# adhd: $(OBJECTS)
# 	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# %.o: %.c $(HEADERS)
# 	$(CC) $(CFLAGS) -c $< -o $@

adhd: src/main.o src/state.o
	$(CC) $(CFLAGS) src/main.o src/state.o -o $@ $(LDFLAGS) $(LDLIBS)

src/main.o: src/main.c $(HEADERS)
	$(CC) -c $(CFLAGS) src/main.c -o $@

src/state.o: src/state.c $(HEADERS)
	$(CC) -c $(CFLAGS) src/state.c -o $@
clean:
	rm -rf $(OBJECTS) adhd

.PHONY: install
install: adhd
	install -D adhd $(BINDIR)/adhd

