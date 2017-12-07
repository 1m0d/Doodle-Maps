#ifndef __PARSE_MAP_H__
#define __PARSE_MAP_H__

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

ParsedMap parse_map(char *map_json);

#endif
