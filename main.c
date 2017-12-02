#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "cJSON.h"

typedef struct{
  int coordinates[2];
  int weight;
}Tile;

typedef struct{
  int size[2];
  int tiles_of_interest[2][2];
  Tile *tiles;
  int tile_count;
  int hexagon_height;
  int draw_area[2];
}ParsedMap;

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
  int size;
}Graph;

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
  return graph;
}


char *file_to_string(FILE *file){
  char *file_contents;
  long file_size;
  fseek(file, 0, SEEK_END);
  file_size = ftell(file);
  rewind(file);
  file_contents = malloc(file_size * (sizeof(char)));
  fread(file_contents, sizeof(char), file_size, file);
  fclose(file);
  return file_contents;
}

ParsedMap parse_map(char *map_json){
  cJSON *root = cJSON_Parse(map_json);
  if (!root){
    printf("Error in file before: [%s]\n", cJSON_GetErrorPtr());
    exit(EXIT_FAILURE);
  }

  //parse keys
  cJSON *map = cJSON_GetObjectItem(root, "map");
  cJSON *size = cJSON_GetObjectItem(map, "size");
  cJSON *tiles = cJSON_GetObjectItem(map, "tiles");
  cJSON *tiles_of_interest = cJSON_GetObjectItem(map, "tiles_of_interest");
  cJSON *svg = cJSON_GetObjectItem(root, "svg");
  cJSON *hexagon_height = cJSON_GetObjectItem(svg, "hexagon_height");
  cJSON *draw_area = cJSON_GetObjectItem(svg, "draw_area");
  //TODO check for validity

  //parse size
  ParsedMap parsed_map;
  parsed_map.size[0] = cJSON_GetArrayItem(size, 0)->valueint;
  parsed_map.size[1] = cJSON_GetArrayItem(size, 1)->valueint;

  //parse tiles of interest
  for(int i = 0; i < 2; i++){
    cJSON *sub_arr = cJSON_GetArrayItem(tiles_of_interest, i);
    for(int j = 0; j < 2; j++){
      parsed_map.tiles_of_interest[i][j] = cJSON_GetArrayItem(sub_arr, j)->valueint;
    }
  }

  //count number of tiles for memory allocation
  int tile_count = 0;
  cJSON *row = tiles->child;
  for (int i = 0; i < cJSON_GetArraySize(tiles); i++){
    for (int j = 0; j < cJSON_GetArraySize(row); j++){
      tile_count++;
    }
    row = row->next;
  }

  parsed_map.tile_count = tile_count;

  parsed_map.tiles = malloc(sizeof(Tile) * tile_count);
  if (parsed_map.tiles == NULL){
    printf("Out of memory! Exiting...\n");
    exit(EXIT_FAILURE);
  }

  //parse tiles
  row = tiles->child;
  int index = 0;
  for (int i = 0; i < cJSON_GetArraySize(tiles); i++){
    for (int j = 0; j < cJSON_GetArraySize(row); j++){
      cJSON *sub_arr = cJSON_GetArrayItem(row, j);
      parsed_map.tiles[index].weight = atoi(row->string);
      for(int k = 0; k < 2; k++){
        parsed_map.tiles[index].coordinates[k] = cJSON_GetArrayItem(sub_arr, k)->valueint;
      }
      index++;
    }
    row = row->next;
  }

  //parse svg
  parsed_map.hexagon_height = hexagon_height->valueint;
  parsed_map.draw_area[0] = cJSON_GetArrayItem(draw_area, 0)->valueint;
  parsed_map.draw_area[1] = cJSON_GetArrayItem(draw_area, 1)->valueint;

  return parsed_map;
}

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

    if(moving->heuristic > this->heuristic){
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
      if(moving->cost >= new->cost){
        //it's first in que
        if(moving == *start){
          free(*start);
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
  while(active_node->node_index != end_node_index){
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
  /*PriorityQue *temp = active_node;*/
  /*while(temp != NULL){*/
    /*printf("%d, ", temp->node_index);*/
    /*temp = temp->came_from;*/
  /*}*/
  return active_node;
}

void draw_map(ParsedMap parsed_map, PriorityQue *shortest_path, PriorityQue *fastest_path){
  //TODO compute needed memory
  char svg[100000];
  char hex[200];
  sprintf(svg, "<svg id=\"map\" width=\"%d\" height = \"%d\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" "
               "xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n",
               parsed_map.draw_area[0], parsed_map.draw_area[1]);

  char *hex_images = "<defs>\n"
                     "<pattern id=\"pattern0\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"hex_icons/0.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern1\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"hex_icons/1.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern2\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"hex_icons/2.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern3\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"hex_icons/3.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern4\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"hex_icons/4.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern5\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"hex_icons/5.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern10\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"hex_icons/start.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern11\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"hex_icons/finish.png\" />\n"
                     "</pattern>\n"
                     "</defs>";

  strcat(svg, hex_images);

  int height = parsed_map.hexagon_height;
  int width = sqrt(3)/2 * height;
  int path_element_height = height / 5;

  int point1[2] = {width / 2, height};
  int point2[2] = {0, height * 3/4};
  int point3[2] = {0, height * 1/4};
  int point4[2] = {width / 2, 0};
  int point5[2] = {width, height * 1/4};
  int point6[2] = {width, height * 3/4};
  int verticalpush = height * 3/4;

  int *start = parsed_map.tiles_of_interest[0];
  int *finish = parsed_map.tiles_of_interest[1];

  for(int row = 0; row < parsed_map.size[1]; row++){
    int horizontal_margin = 30;
    if (row % 2 != 0)
      horizontal_margin += width/2;
    for(int column = 0; column < parsed_map.size[0]; column++){
      int coordinates[2] = {column, row};
      int weight = get_tile_weight(parsed_map, coordinates);

      //if current tile is a tile of interest give it special weight
      if(coordinates[0] == start[0] && coordinates[1] == start[1]){
        weight = 10;
      }
      else if(coordinates[0] == finish[0] && coordinates[1] == finish[1]){
        weight = 11;
      }

      //place hexes
      sprintf(hex, "<polygon class=\"hex\" fill=\"url(#pattern%d)\" stroke=\"green\" stroke-width=\"1\" "
                   "points=\"%d,%d %d,%d %d,%d %d,%d %d,%d %d,%d\" transform = \"translate(%d,%d)\"></polygon>\n",
                    weight, point1[0], point1[1], point2[0], point2[1], point3[0], point3[1], point4[0], point4[1],
                    point5[0], point5[1], point6[0], point6[1],
                    horizontal_margin + column * width, 40 + row * verticalpush);
      strcat(svg, hex);

      if(weight == 0 || weight == 10 || weight == 11)
        continue;

      int fastest_x_offset = width/4;
      int shortest_x_offset = fastest_x_offset + path_element_height;

      //place path element
      int tile_index = coo2index(parsed_map, coordinates);
      PriorityQue *moving = fastest_path;
      while(moving != NULL){
        if(moving->node_index == tile_index){
          char path[200];
          sprintf(path, "<image x=\"%d\" y=\"%d\" width =\"%d\" height=\"%d\" xlink:href=\"./hex_icons/alienGreen_badge1.png\"/>\n",
                        fastest_x_offset + horizontal_margin + column * width, 50 + height/3 + row * verticalpush,
                        path_element_height, path_element_height);
          strcat(svg, path);
          break;
        }
        else
          moving = moving->came_from;
      }
      moving = shortest_path;
      while(moving != NULL){
        if(moving->node_index == tile_index){
          char path[200];
          sprintf(path, "<image x=\"%d\" y=\"%d\" width =\"%d\" height=\"%d\" xlink:href=\"./hex_icons/alienYellow_badge1.png\"/>\n",
                        shortest_x_offset + horizontal_margin + column * width, 50 + height/3 + row * verticalpush,
                        path_element_height, path_element_height);
          strcat(svg, path);
          break;
        }
        else
          moving = moving->came_from;
      }
    }
  }
  strcat(svg, "</svg>");
  printf("%s", svg);
}

int main(int argc, char *argv[]){
  char *extension = ".json";
  FILE *map_file;

  if (argc != 2){
    printf("You must have exactly 1 argument!\n");
    return EXIT_FAILURE;
  }
  if (strstr(argv[1], extension)){
    map_file = fopen(argv[1], "r");
  }
  else{
    printf("The given argument must be a .json file!\n");
    return EXIT_FAILURE;
  }

  char *map_json = file_to_string(map_file);
  ParsedMap parsed_map = parse_map(map_json);
  Graph graph = create_graph(parsed_map);
  PriorityQue *fastest_path = find_paths(parsed_map, graph, "fastest");
  PriorityQue *shortest_path = find_paths(parsed_map, graph, "shortest");
  draw_map(parsed_map, shortest_path, fastest_path);
}
