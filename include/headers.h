#ifndef HEADERS_H
#define HEADERS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

typedef struct {
    const char *name;       // port label shown near the pin
    const char *direction;  // "in" or "out"
    const char *bus;        // net/bus name used to connect
    int x, y;               // computed pin location in SVG
} StructPort;

typedef struct {
    const char *component_name;
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

static int height_val = 1024;
static int width_val  = 1024;
static int *svg_height = &height_val;
static int *svg_width  = &width_val;

static int max(int a, int b){ return a > b ? a:b;}

#endif
