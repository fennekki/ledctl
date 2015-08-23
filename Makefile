all: ledctl

CFLAGS=-O2 -s

ledctl: ledctl.c
	$(CC) -o $@ ledctl.c
