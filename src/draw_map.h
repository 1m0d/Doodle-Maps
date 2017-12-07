#ifndef __DRAW_MAP_H__
#define __DRAW_MAP_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "parse_map.h"
#include "map_to_graph.h"
#include "a_star.h"

void draw_map(ParsedMap parsed_map, PriorityQue *shortest_path, PriorityQue *fastest_path);
#endif
