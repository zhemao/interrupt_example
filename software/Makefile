CC=arm-linux-gnueabihf-gcc
CFLAGS=-Wall -O3

all: readstate

%: %.c
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@
