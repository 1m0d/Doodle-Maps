#ifndef __A_STAR_H__
#define __A_STAR_H__

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "map_to_graph.h"

typedef struct PriorityQue{
  int node_index;
  int cost;
  int heuristic;
  struct PriorityQue *came_from;
  struct PriorityQue *next;
}PriorityQue;

typedef struct VisitedNodes{
  int node_index;
  struct VisitedNodes *next;
}VisitedNodes;

void free_que(PriorityQue *start);
void free_visited(VisitedNodes *start);
void insert_into_que(PriorityQue **start, PriorityQue *this);
bool already_visited(int node_index, VisitedNodes *visited);
bool already_in_que(PriorityQue *new, PriorityQue **start);
PriorityQue* find_paths(ParsedMap map, Graph graph, char* setting);
#endif
