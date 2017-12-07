#ifndef __MAP_TO_GRAPH_H__
#define __MAP_TO_GRAPH_H__

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "parse_map.h"


typedef struct{
  int index_of_nodes[2];
  int weight;
}Edge;

typedef struct{
  int index;
  int distance_to_end;
  int edges_count;
  Edge **edges;
}Node;

typedef struct{
  Node *nodes;
  Edge *edges;
  int size;
}Graph;

void free_edges(Graph graph);

int get_tile_weight(ParsedMap map, int coordinates[2]);
int coo2index(ParsedMap map, int coordinates[2]);
int *index2coo(ParsedMap map, int index);
int find_distance(ParsedMap map, int from, int to);
int **get_neighbours(ParsedMap map, int *neighbour_count);

Graph create_graph(ParsedMap map);

#endif
