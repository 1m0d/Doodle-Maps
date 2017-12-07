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

void free_que(PriorityQue *start){
  while(start != NULL){
    if(start->next != NULL){
      PriorityQue *temp = start->next;
      free(start);
      start = temp;
    }
    else{
      free(start);
      break;
    }
    start = start->next;
  }
}

void free_visited(VisitedNodes *start){
  while(start != NULL){
    if(start->next != NULL){
      VisitedNodes *temp = start->next;
      free(start);
      start = temp;
    }
    else{
      free(start);
      break;
    }
    start = start->next;
  }
}

void insert_into_que(PriorityQue **start, PriorityQue *this){
  if(*start == NULL){
    *start = this;
    return;
  }

  PriorityQue *moving = *start;
  PriorityQue *moving_behind;
  while(moving != NULL){
    if(moving == NULL)
      moving_behind->next = this;

    if(moving->heuristic >= this->heuristic){
      if(moving == *start){
        this->next = *start;
        *start = this;
      }
      else{
        moving_behind->next = this;
        this->next = moving->next;
      }
      return;
    }
    else{
    moving_behind = moving;
    moving = moving->next;
    }
  }
  moving_behind->next = this;
}

bool already_visited(int node_index, VisitedNodes *visited){
  if(visited == NULL)
    return false;

  while(visited != NULL){
    if(visited->node_index == node_index)
      return true;
    visited = visited->next;
  }
  return false;
}

bool already_in_que(PriorityQue *new, PriorityQue **start){
  if(*start == NULL)
    return false;

  PriorityQue *moving, *moving_behind;
  moving = *start;

  while(moving != NULL){
    //it's a match
    if(moving->node_index == new->node_index){
      //new is better, remove old que element
      if(moving->cost > new->cost){
        //it's first in que
        if(moving == *start){
          free(*start);
          *start = NULL;
        }
        //it's not first
        else{
          moving_behind->next = moving->next;
          free(moving);
        }
        insert_into_que(start, new);
      }
      //new is not better
      else{
        free(new);
      }
      return true;
    }
    //not a match
    else{
      moving_behind = moving;
      moving = moving->next;
    }
  }
  return false;
}

PriorityQue* find_paths(ParsedMap map, Graph graph, char* setting){
  int end_node_index = coo2index(map, map.tiles_of_interest[1]);

  PriorityQue *start = (PriorityQue *) malloc(sizeof(PriorityQue));
  start->node_index = coo2index(map, map.tiles_of_interest[0]);
  start->cost = 0;
  start->heuristic = 0;
  start->came_from = NULL;
  start->next = NULL;

  VisitedNodes *visited = NULL;

  PriorityQue *active_node = start;
  while(true){
    active_node = start;
    if(start->next != NULL)
      start = start->next;
    else
      start = NULL;

    //loop through all neighbours
    for(int i = 0; i < graph.nodes[active_node->node_index].edges_count; i++){
      //find node to check
      Edge *current_edge = graph.nodes[active_node->node_index].edges[i];
      Node *current_node;

      if (current_edge->index_of_nodes[0] == active_node->node_index)
        current_node = &graph.nodes[current_edge->index_of_nodes[1]];
      else
        current_node = &graph.nodes[current_edge->index_of_nodes[0]];

      if(already_visited(current_node->index, visited))
        continue;

      //if we're not looking for fastest path weight doesn't matter
      if(strcmp(setting, "shortest") == 0)
        current_edge->weight = 1;

      PriorityQue *new = (PriorityQue*) malloc(sizeof(PriorityQue));
      new->came_from = active_node;
      new->node_index = current_node->index;
      new->cost = current_edge->weight + new->came_from->cost;
      new->heuristic = new->cost + current_node->distance_to_end;
      new->next = NULL;

      //found end tile
      if(new->node_index == end_node_index){
        active_node = new;
        free_visited(visited);
        return active_node;
      }

      //if already in que change que accordingly
      if(already_in_que(new, &start))
        continue;

      insert_into_que(&start, new);
    }

    //insert active_node into visited nodes
    if(visited == NULL){
      visited = (VisitedNodes *) malloc(sizeof(VisitedNodes));
      visited->node_index = active_node->node_index;
      visited->next = NULL;
      continue;
    }

    VisitedNodes *moving = visited;
    while(moving->next != NULL){
      moving = moving->next;
    }
    VisitedNodes *new =  (VisitedNodes *) malloc(sizeof(VisitedNodes));
    new->node_index = active_node->node_index;
    new->next = NULL;
    moving->next = new;
  }
}
