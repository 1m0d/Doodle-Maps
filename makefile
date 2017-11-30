CC=gcc
CFLAGS=-I. -g -lm

main:
	     $(CC) -o map_print main.c cJSON.c $(CFLAGS)
