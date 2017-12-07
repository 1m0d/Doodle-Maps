#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>

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
