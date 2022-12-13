CC=gcc
CFLAGS=-I.


JTAG: main.c gpio_control.c file_utilities.c
	$(CC) -o JTAG main.c gpio_control.c file_utilities.c -I.
