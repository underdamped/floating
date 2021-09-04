# Makefile to build 'floating' app
# javier

CC := gcc
CFLAGS := -W -Wall -O2
BIN_DIR := /usr/local/bin

PROG := floating
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

all: $(PROG)

$(PROG): $(OBJS)

debug: CFLAGS += -ggdb3
debug: all

clean:
	$(RM) $(PROG) $(OBJS)

install: all
	install -d $(BIN_DIR)
	install -m 755 $(PROG) $(BIN_DIR)

.PHONY: clean install debug
