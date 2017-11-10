#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "cJSON.h"

typedef struct{
  int coordinates[2];
  int weight;
}Tile;

typedef struct{
  int size[2];
  int tiles_of_interest[2][2];
  Tile *tiles;
  int hexagon_height;
  int draw_area[2];
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

int get_tile_weight(ParsedMap map, int coordinates[2]){
  for(int i = 0; i < map.size[0] * map.size[1]; i++){
    if(coordinates[0] == map.tiles[i].coordinates[0] && coordinates[1] == map.tiles[i].coordinates[1])
      return map.tiles[i].weight;
  }
  return 0;
}

void draw_map(ParsedMap parsed_map){
  //TODO compute needed memory
  char svg[100000];
  char hex[200];
  sprintf(svg, "<svg id=\"map\" width=\"%d\" height = \"%d\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n",
          parsed_map.draw_area[0], parsed_map.draw_area[1]);

  char *hex_images = "<defs>\n"
                     "<pattern id=\"pattern0\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"hex_icons/volcanos.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern1\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"hex_icons/grassland.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern2\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"hex_icons/cactus.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern3\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"hex_icons/jungle.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern4\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"hex_icons/mountains.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern5\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"hex_icons/evergreenmountains.png\" />\n"
                     "</pattern>\n"
                     "</defs>";

  strcat(svg, hex_images);

  int height = parsed_map.hexagon_height;
  int width = sqrt(3)/2 * height;

  int point1[2] = {width / 2, height};
  int point2[2] = {0, height * 3/4};
  int point3[2] = {0, height * 1/4};
  int point4[2] = {width / 2, 0};
  int point5[2] = {width, height * 1/4};
  int point6[2] = {width, height * 3/4};
  int verticalpush = height * 3/4;

  //TODO differentiate between weights
  for(int i = 0; i < parsed_map.size[1]; i++){
    int horizontal_margin = 30;
    if (i % 2 != 0)
      horizontal_margin += width/2;
    for(int j = 0; j < parsed_map.size[0]; j++){
      sprintf(hex, "<polygon class=\"hex\" fill=\"url(#pattern1)\" stroke=\"black\" stroke-width=\"1\" points=\"%d,%d %d,%d %d,%d %d,%d %d,%d %d,%d\" transform = \"translate(%d,%d)\"></polygon>\n",
                    point1[0], point1[1], point2[0], point2[1], point3[0], point3[1], point4[0], point4[1], point5[0], point5[1], point6[0], point6[1],
                    horizontal_margin+j*width, 40+i*verticalpush);
      strcat(svg, hex);
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
  draw_map(parsed_map);
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
