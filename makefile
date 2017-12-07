CC=gcc
CFLAGS=-I. -lm

main:
	     $(CC) -o run src/main.c src/cJSON.c src/parse_map.c src/map_to_graph.c src/a_star.c src/draw_map.c $(CFLAGS)
