#ifndef MAIN_H
#define MAIN_H

#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#define BORDER_SEP 30
#define GAP 35
#define GAP_SEP 10
#define WINDOWSIZE 1024
#define CHAR_WIDTH 10
#define PORT_HEIGHT 20
#define PADDING_WIDTH 20
#define PADDING_HEIGHT 10

typedef struct {
    const char *name;       // port label shown near the pin
    const char *direction;  // "in" or "out"
    const char *bus;        // net/bus name used to connect
    int x, y;               // computed pin location in SVG
} StructPort;

typedef struct {
    char *component_name;
    StructPort *ports;
    size_t port_count;
    int pos_x, pos_y;               // top-left position of block
    int width, height;               // computed size of block
} StructComponent;

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

extern Point **paths;
extern int *paths_lengths;
extern int num_paths;

/* extern int height_val;
   extern int width_val; */
extern int *svg_height;
extern int *svg_width;


#endif
