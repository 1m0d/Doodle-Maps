#include <stdio.h>

#include "parse_map.h"
#include "map_to_graph.h"
#include "a_star.h"
#include "draw_map.h"

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
  ParsedMap map = parse_map(map_json);
  free(map_json);

  Graph graph = create_graph(map);

  PriorityQue *fastest_path = find_paths(map, graph, "fastest");
  PriorityQue *shortest_path = find_paths(map, graph, "shortest");

  draw_map(map, shortest_path, fastest_path);

  free(map.tiles);
  free_edges(graph);
  free(graph.nodes);
  free_que(fastest_path);
  free_que(shortest_path);
}
