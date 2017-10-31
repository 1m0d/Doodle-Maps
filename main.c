#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cJSON.h"

typedef struct{
  int size[2];
  int tiles_of_interest[2][2];
  /*int *tiles;*/
}ParsedMap;

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

  cJSON *map = cJSON_GetObjectItem(root, "map");
  cJSON *size = cJSON_GetObjectItem(map, "size");
  cJSON *tiles = cJSON_GetObjectItem(map, "tiles");
  cJSON *tiles_of_interest = cJSON_GetObjectItem(map, "tiles_of_interest");
  //TODO check for validity

  ParsedMap parsed_map;
  parsed_map.size[0] = cJSON_GetArrayItem(size, 0)->valueint;
  parsed_map.size[1] = cJSON_GetArrayItem(size, 1)->valueint;

  for(int i = 0; i < 2; i++){
    cJSON *sub_arr = cJSON_GetArrayItem(tiles_of_interest, i);
    for(int j = 0; j < 2; j++){
      parsed_map.tiles_of_interest[i][j] = cJSON_GetArrayItem(sub_arr, j)->valueint;
    }
  }

  return parsed_map;
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
  parse_map(map_json);
  //map_parsed = parse_json(map_json)
  //map_graph = graph(map_parsed)
  //check if path is possible
  //paths = find_paths(map_graph)
  //draw_map(map_parsed, paths)
}

//graph: nodes: connections to nodes w diff

/*typedef struct Node{*/
  /*int position[2];*/
  /*struct Node *neighbors;*/
/*}Node;*/

/*typedef struct{*/
  /*int x, y, difficulty;*/
/*}Tile;*/

////graph
//typedef struct{
//  nodes, connections w difficulty
//}Graph;

////find_path
//shortest_path()
//change all non-zero difficulty to 1
//fastest_path()

////draw_map
//draw whole map
//draw paths
