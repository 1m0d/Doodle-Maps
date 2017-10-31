#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    printf("You must add exactly 1 argument!\n");
    return 1;
  }
  if (strstr(argv[1], extension)){
    map_file = fopen(argv[1], "r");
  }
  else{
    printf("The given argument must be a .json file!\n");
    return 1;
  }
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
