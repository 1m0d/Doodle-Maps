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

void free_edges(Graph graph){
  for(int i = 0; i < graph.size; i++){
    free(graph.nodes[i].edges);
  }
  free(graph.edges);
}

int get_tile_weight(ParsedMap map, int coordinates[2]){
  for(int i = 0; i < map.tile_count; i++){
    if(coordinates[0] == map.tiles[i].coordinates[0] && coordinates[1] == map.tiles[i].coordinates[1])
      return map.tiles[i].weight;
  }
  return 0;
}

//indices are solely used as primary keys, there is no correlation between a value and its incremented pair
int coo2index(ParsedMap map, int coordinates[2]){
  for(int i = 0; i < map.tile_count; i++){
    if(map.tiles[i].coordinates[0] == coordinates[0] && map.tiles[i].coordinates[1] == coordinates[1])
      return i;
  }
  printf("coo2index() called on non-existent or 0 weight tile: %d %d", coordinates[0], coordinates[1]);
  exit(EXIT_FAILURE);
}

int *index2coo(ParsedMap map, int index){
  return map.tiles[index].coordinates;
}

int find_distance(ParsedMap map, int from, int to){
  int *from_coos = index2coo(map, from);
  int *to_coos = index2coo(map, to);

  if (from_coos[1] == to_coos[1])
    return abs(to_coos[0] - from_coos[0]);
  else if (from_coos[0] == to_coos[0])
    return abs(to_coos[1] - from_coos[1]);
  else{
    int dy = abs(to_coos[1] - from_coos[1]);
    int dx = abs(to_coos[0] - from_coos[0]);
    if(from_coos[0] < to_coos[0])
      return dx + dy - ceil(dx / 2.0);
    else
      return dx + dy - floor(dx / 2.0);
  }
}

int **get_neighbours(ParsedMap map, int *neighbour_count){
  int odd_row_offset[6][2] = {{-1, -1}, {-1, 0}, {0, 1}, {0, -1}, {1, 0}, {-1, 1}};
  int even_row_offset[6][2] = {{-1, 0}, {1, -1}, {0, 1}, {0, -1}, {1, 1}, {1, 0}};
  bool memory_allocated = false;
  int neighbours_added = 0;
  int **neighbours;

  while(true){
    for(int tile = 0; tile < map.tile_count; tile++){
      for (int adjacent_tile = 0; adjacent_tile < 6; adjacent_tile++){
        int coo2check[2];
        for(int i = 0; i < 2; i++){
          if(map.tiles[tile].coordinates[1] % 2 == 0)
            coo2check[i] = map.tiles[tile].coordinates[i] + odd_row_offset[adjacent_tile][i];
          else
            coo2check[i] = map.tiles[tile].coordinates[i] + even_row_offset[adjacent_tile][i];
        }
        if(coo2check[0] >= 0 && coo2check[1] >= 0 && get_tile_weight(map, coo2check) != 0){
          if(tile < coo2index(map, coo2check)){  //don't create duplicate data of same neighbours
            if(memory_allocated){
              neighbours[neighbours_added][0] = tile;
              neighbours[neighbours_added][1] = coo2index(map, coo2check);
              neighbours_added++;
            }
            else{
              (*neighbour_count)++;
            }
          }
        }
      }
    }

      if(!memory_allocated){
        neighbours = (int **) malloc(*neighbour_count * sizeof(int) * 2);
        for(int i = 0; i < *neighbour_count; i++){
          neighbours[i] = (int *) malloc(2 * sizeof(int));
        }
        memory_allocated = true;
      }
      if(neighbours_added == *neighbour_count)
        break;
  }
  return neighbours;
}

Graph create_graph(ParsedMap map){
  Graph graph;
  graph.size = map.tile_count;
  int edge_count = 0;
  int *edge_countPtr = &edge_count;
  int **neighbours = get_neighbours(map, edge_countPtr);

  Node *nodes = (Node *) malloc(sizeof(Node) * graph.size);
  Edge *edges = (Edge *) malloc(sizeof(Edge) * edge_count);

  //create edges
  for(int i = 0; i < edge_count; i++){
    edges[i].index_of_nodes[0] = neighbours[i][0];
    edges[i].index_of_nodes[1] = neighbours[i][1];
    edges[i].weight = get_tile_weight(map, index2coo(map, neighbours[i][0])) + get_tile_weight(map, index2coo(map, neighbours[i][1]));
  }
  for(int i = 0; i < edge_count; i++)
    free(neighbours[i]);
  free(neighbours);

  //assign edges to nodes
  for(int node_index = 0; node_index < graph.size; node_index++){
    int node_edge_count = 0;
    bool memory_allocated = false;
    //first allocate memory, then assign pointers
    for (int i = 0; i < 2; i++){
      for(int edge_index = 0; edge_index < edge_count; edge_index++){
        for(int j = 0; j < 2; j++){
          if(edges[edge_index].index_of_nodes[j] == node_index){
            if(!memory_allocated)
              node_edge_count++;
            else{
              node_edge_count--;
              nodes[node_index].edges[node_edge_count] = &(edges[edge_index]);
            }
          }
        }
      }
      if (!memory_allocated){
        nodes[node_index].edges = malloc(sizeof(Edge *) * node_edge_count);
        nodes[node_index].edges_count = node_edge_count;
        memory_allocated = true;
      }
    }
  }

  /*assign distance and index to nodes*/
  for(int i = 0; i < graph.size; i++){
    nodes[i].distance_to_end = find_distance(map, i, coo2index(map, map.tiles_of_interest[1]));
    nodes[i].index = i;
  }
  graph.nodes = nodes;
  graph.edges = edges;
  return graph;
}
