CC=gcc
CFLAGS=-I.


JTAG: main.c led_control.c file_utilities.c
	$(CC) -o JTAG main.c led_control.c file_utilities.c -I.
