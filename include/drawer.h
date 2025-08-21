#ifndef DRAWER_H
#define DRAWER_H
/* ----------------- Data types ----------------- */
typedef struct {
    const char *name;       // port label shown near the pin
    const char *direction;  // "in" or "out"
    const char *bus;        // net/bus name used to connect
    int x, y;               // computed pin location in SVG
} Port;

typedef struct {
    const char *component_name;
    Port *ports;
    size_t port_count;
    int x, y;               // top-left position of block
    int w, h;               // computed size of block
} Component;


static int max(int a, int b);
static void auto_layout(Component *c, int idx);
static void place_ports(Component *c);
static void svg_header(FILE *f, int width, int height);
static void svg_footer(FILE *f);
static void draw_component(FILE *f, const Component *c);
static void route_bus(FILE *f, const Port *src, const Port *dst);
static void draw_connections(FILE *f, Component *comps, size_t n);
#endif
