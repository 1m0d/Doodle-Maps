#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "parse_map.h"
#include "map_to_graph.h"
#include "a_star.h"

void draw_map(ParsedMap parsed_map, PriorityQue *shortest_path, PriorityQue *fastest_path){
  char svg[10000];
  char hex[200];
  sprintf(svg, "<svg id=\"map\" width=\"%d\" height = \"%d\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" "
               "xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n",
               parsed_map.draw_area[0], parsed_map.draw_area[1]);

  char *hex_images = "<defs>\n"
                     "<pattern id=\"pattern0\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"assets/Environment/0.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern1\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"assets/Environment/1.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern2\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"assets/Environment/2.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern3\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"assets/Environment/3.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern4\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"assets/Environment/4.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern5\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"assets/Environment/5.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern10\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"assets/Environment/start.png\" />\n"
                     "</pattern>\n"
                     "<pattern id=\"pattern11\" height=\"100%\" width=\"100%\" patternContentUnits=\"objectBoundingBox\">\n"
                     "<image height=\"1\" width=\"1\" preserveAspectRatio=\"none\" xlink:href=\"assets/Environment/finish.png\" />\n"
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
          sprintf(path, "<image x=\"%d\" y=\"%d\" width =\"%d\" height=\"%d\" xlink:href=\"assets/Characters/alienGreen_badge1.png\"/>\n",
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
          sprintf(path, "<image x=\"%d\" y=\"%d\" width =\"%d\" height=\"%d\" xlink:href=\"assets/Characters/alienYellow_badge1.png\"/>\n",
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
