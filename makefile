CC=gcc
CFLAGS=-I. -g -lm

main:
	     $(CC) -o main.out main.c cJSON.c $(CFLAGS)
