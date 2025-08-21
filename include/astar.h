#ifndef ASTAR_H
#define ASTAR_H

#define COLUMNS 1024
#define ROWS 1024

typedef struct {
  int coord_x;
  int coord_y;
} Point;

typedef struct {
  Point child;
  Point parent;
  bool list_open, list_closed;
  int g_cost;   // distance from start
  int h_cost;   // heuristic to goal
  int f_cost;   // g + h
} Node;

extern Point **paths;        // array de paths generados
extern int *paths_lengths;   // longitudes de cada path
extern int num_paths;        // número de paths generados

// ---------------------------- Functions --------------------------------

void Initialization();
void DrawMatrix();
void DefineRestriction(int pos_x, int pos_y, int width, int height);
void PathFinderAStar(int start_x, int start_y, int end_x, int end_y);
void SimplifyPathsToCorners();
void FreeMemory();
#endif
