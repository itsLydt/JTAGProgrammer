CC=gcc
CFLAGS=-I.


JTAG: main.c led_control.c
	$(CC) -o JTAG main.c led_control.c -I.
