#include "headers.h"
#include "astar.h"
#include "auto_placer.h"

static int GetMaxNumberPorts(StructPort *port, int port_count){
  int number_port_in = 0;
  int number_port_out = 0;

  for (int i = 0; i < port_count; i++) {
    if(strcmp(port[i].direction,"in") == 0){
      number_port_in++;
    }else if(strcmp(port[i].direction,"out") == 0){
      number_port_out++;
    }
  }
  return max(number_port_in,number_port_out);
}

static int GetMaxNameSize(StructPort *port, int port_count) {
  int text_size = 0;
  for (int i = 0; i < port_count; i++) {
    int size = strlen(port[i].name);
    if (size > text_size) {
      text_size = size;
    }
  }
  return text_size;
}

static void CalcPosComponents(StructComponent *component, int number_components) {
  int columns = ceil(sqrt(number_components));
  int rows = ceil(number_components / columns);
  int needed_width = GAP;
  int needed_height = GAP;
  int max_width_col[columns];
  int max_height_row[rows];
  
  for (int c = 0; c < columns; c++) {
    max_width_col[c] = 0;
  }
  for (int r = 0; r < rows; r++){
    max_height_row[r] = 0;
  }

  for (int i = 0; i < number_components; i++) {
    int col = i % columns;
    int row = i / columns;
    if (component[i].width > max_width_col[col]){
      max_width_col[col] = component[i].width;
    }
    if (component[i].height > max_height_row[row]){
      max_height_row[row] = component[i].height;
    }
  }

  for (int c = 0; c < columns; c++){
    needed_width += max_width_col[c] + GAP;
  }
  for (int r = 0; r < rows; r++){
    needed_height += max_height_row[r] + GAP;
  }

  *svg_width = needed_width;
  *svg_height = needed_height;


  int y_offset = GAP;
  for (int r = 0; r < rows; r++) {
    int x_offset = GAP;
    for (int c = 0; c < columns; c++) {
      int i = r * columns + c;
      if (i >= number_components) break;

      component[i].pos_x = x_offset + max_width_col[c]/2;
      component[i].pos_y = y_offset + max_height_row[r]/2;

      x_offset += max_width_col[c] + GAP;
    }
    y_offset += max_height_row[r] + GAP;
  }
}

static void CalcSizeComponents(StructComponent *component, int number_components) {
  for(int i = 0; i < number_components; i++){
    component[i].width = 2*GetMaxNameSize(component[i].ports,component[i].port_count) + COMPONENT_WIDTH_OFFSET;
    component[i].height = GetMaxNumberPorts(component[i].ports,component[i].port_count) + COMPONENT_HEIGHT_OFFSET;
  }
}

StructComponent* AutoLayout(StructComponent *component, int number_components) {

  CalcSizeComponents(component, number_components);
  CalcPosComponents(component, number_components);
  /* DefineRestriction(component, number_components); */
    printf("Resultado: X: %d, Y: %d, Width: %d, Height: %d\n",component->pos_x,component->pos_y,component->width,component->height);
  printf("Resultado: X: %d, Y: %d, Width: %d, Height: %d\n",component->pos_x,component->pos_y,component->width,component->height);
  return component;
}

int main() {

  StructPort clk25gen_ports[] = {
    {"clk50", "in",  "clk50", 0,0},
    {"clk25", "out", "clk25",  0,0}
  };

  StructComponent clk25gen = {"clk25gen", clk25gen_ports, 2, 0,0, 0,0};
  // Instancia: debounce_circuit
  StructPort debounce_ports[] = {
    {"clk",   "in",  "clk50",  0,0},
    {"input", "in",  "pb",     0,0},
    {"output","out", "resend", 0,0}
  };
  StructComponent debounce_circuit = {"debounce_circuit", debounce_ports, 3, 0,0, 0,0};

  StructComponent comps[] = {clk25gen, debounce_circuit};

  (void) AutoLayout(comps, 2);

  /* Debugging */
  printf("WIND_HEIGHT: %d, WIND_WIDTH: %d\n", height_val, width_val);

  return 0;
}




/*
static void place_ports(Component *c){
// inputs on left, outputs on right; vertically spaced
int in_slot = 0, out_slot = 0;
for (size_t j = 0; j < c->port_count; j++) {
Port *p = &c->ports[j];
if (strcmp(p->direction,"in")==0) {
p->x = c->x - 12;
p->y = c->y + 28 + in_slot*22;
in_slot++;
} else {
p->x = c->x + c->w;
p->y = c->y + 28 + out_slot*22;
out_slot++;
}
}
}

/\* ----------------- SVG drawing ----------------- *\/
static void svg_header(FILE *f, int width, int height) {
fprintf(f,
"<svg xmlns='http://www.w3.org/2000/svg' width='%d' height='%d'>\n"
"<style>"
"text{font-family:system-ui,Segoe UI,Roboto,sans-serif;font-size:12px}"
"rect{rx:14;ry:14;fill:#f3f4f6;stroke:#111827;stroke-width:1.5}"
"line{stroke:#111827;stroke-width:1.5}"
"circle{fill:#111827}"
"path{stroke:#2563eb;stroke-width:2;fill:none}"
".label{fill:#111827}"
".bus{fill:#2563eb}"
"</style>\n"
"<defs>"
"<marker id='arrow' markerWidth='10' markerHeight='10' refX='10' refY='3' orient='auto' markerUnits='strokeWidth'>"
"<path d='M0,0 L0,6 L9,3 z' fill='#2563eb'/></marker>"
"</defs>\n",
width, height
);
}

static void svg_footer(FILE *f){ fprintf(f,"</svg>\n"); }

static void draw_component(FILE *f, const Component *c){
fprintf(f, "<rect x='%d' y='%d' width='%d' height='%d'/>\n", c->x, c->y, c->w, c->h);
fprintf(f, "<text class='label' x='%d' y='%d'>%s</text>\n", c->x+12, c->y+18, c->component_name);

for (size_t j = 0; j < c->port_count; j++) {
const Port *p = &c->ports[j];
// short pin stub into the box
int x2 = (strcmp(p->direction,"in")==0) ? (c->x) : (c->x + c->w);
fprintf(f, "<circle cx='%d' cy='%d' r='3'/>\n", p->x, p->y);
fprintf(f, "<line x1='%d' y1='%d' x2='%d' y2='%d'/>\n", p->x, p->y, x2, p->y);

// port label
if (strcmp(p->direction,"in")==0) {
fprintf(f, "<text class='label' x='%d' y='%d' text-anchor='end'>%s</text>\n",
p->x-8, p->y+4, p->name);
} else {
fprintf(f, "<text class='label' x='%d' y='%d'>%s</text>\n",
p->x+8, p->y+4, p->name);
}
}
}

static void route_bus(FILE *f, const Port *src, const Port *dst) {
PathFinderAStar(src->x, src->y, dst->x, dst->y);
SimplifyPathsToCorners();

// usamos solo el path calculado en ESTA llamada
if (num_paths <= 0) return;

Point *path = paths[num_paths - 1];
int len = paths_lengths[num_paths - 1];
if (len == 0) return;

fprintf(f, "<path d='M %d %d", path[0].coord_x, path[0].coord_y);

for (int i = 1; i < len; i++) {
fprintf(f, " L %d %d", path[i].coord_x, path[i].coord_y);
}

fprintf(f, "' marker-end='url(#arrow)'/>\n");
}

/\* static void route_bus(FILE *f, const Port *src, const Port *dst){
PathFinderAStar(src->x, src->y, dst->x, dst->y);
SimplifyPathsToCorners();

// asumimos que solo tenemos un path simplificado: paths[0] y paths_lengths[0]
Point *path = paths[0];
int len = paths_lengths[0];
if(len == 0) return;

fprintf(f, "<path d='M %d %d", path[0].coord_x, path[0].coord_y);

for(int i = 1; i < len; i++){
fprintf(f, " L %d %d", path[i].coord_x, path[i].coord_y);
}

fprintf(f, "' marker-end='url(#arrow)'/>\n");
} *\/

/\* static void route_bus(FILE *f, const Port *src, const Port *dst){
PathFinderAStar(src->x,src->y,dst->x,dst->y);
SimplifyPathsToCorners();
fprintf(f, "<path d='M %d %d L %d %d L %d %d L %d %d' marker-end='url(#arrow)'/>\n",
src->x, src->y, midX, src->y, midX, dst->y, dst->x, dst->y);
/\\* // Orthogonal path: src -> midX -> vertical to dst.y -> dst
int midX;
// If going left-to-right, mid between; otherwise create a dogleg past the blocks
if (src->x <= dst->x) {
midX = (src->x + dst->x)/2;
} else {
// if destination is to the left, push a bit to the right first to avoid intersecting the source block edge
midX = src->x + 40;
} *\\/

} *\/

static void draw_connections(FILE *f, Component *comps, size_t n){
// For each OUT port, connect to all IN ports that share the same bus
for (size_t i = 0; i < n; i++) {
for (size_t j = 0; j < comps[i].port_count; j++) {
Port *p_out = &comps[i].ports[j];
if (strcmp(p_out->direction,"out")!=0) continue;

int any = 0;
for (size_t a = 0; a < n; a++) {
for (size_t b = 0; b < comps[a].port_count; b++) {
Port *p_in = &comps[a].ports[b];
if (strcmp(p_in->direction,"in")==0 && strcmp(p_in->bus, p_out->bus)==0) {
any = 1;
route_bus(f, p_out, p_in);
// bus label near the bend
int labelX = (p_out->x + p_in->x)/2 + 6;
int labelY = (p_out->y + p_in->y)/2 - 6;
fprintf(f, "<text class='bus' x='%d' y='%d'>%s</text>\n", labelX, labelY, p_in->bus);
}
}
}

// If this bus has no consumers, draw a small dangling arrow to the right (sink)
if (!any) {
fprintf(f, "<path d='M %d %d L %d %d' marker-end='url(#arrow)'/>\n",
p_out->x, p_out->y, p_out->x+36, p_out->y);
fprintf(f, "<text class='bus' x='%d' y='%d'>%s</text>\n",
p_out->x+40, p_out->y-6, p_out->bus);
}
}
}

// Also: if a bus only has IN ports and no OUT ports, create a small left-side source stub
// Scan all buses
typedef struct { const char *bus; int has_out; int has_in; } BusStat;
BusStat stats[256]; int sc = 0;

for (size_t i = 0; i < n; i++){
for (size_t j = 0; j < comps[i].port_count; j++){
const Port *p = &comps[i].ports[j];
// find/add bus
int k;
for (k=0; k<sc; k++) if (strcmp(stats[k].bus,p->bus)==0) break;
if (k==sc){ stats[sc].bus=p->bus; stats[sc].has_in=0; stats[sc].has_out=0; sc++; }
if (strcmp(p->direction,"in")==0)  { stats[k].has_in = 1; }
else                                { stats[k].has_out= 1; }
}
}

for (int s=0; s<sc; s++){
if (stats[s].has_in && !stats[s].has_out){
// Find the leftmost consumer; place a small source arrow to its left
const Port *minConsumer = NULL;
for (size_t i = 0; i < n; i++){
for (size_t j = 0; j < comps[i].port_count; j++){
const Port *p = &comps[i].ports[j];
if (strcmp(p->direction,"in")==0 && strcmp(p->bus, stats[s].bus)==0){
if (!minConsumer || p->x < minConsumer->x) minConsumer = p;
}
}
}
if (minConsumer){
int sx = minConsumer->x - 40;
int sy = minConsumer->y;
fprintf(f, "<path d='M %d %d L %d %d L %d %d' marker-end='url(#arrow)'/>\n",
sx, sy, minConsumer->x - 18, sy, minConsumer->x, sy);
fprintf(f, "<text class='bus' x='%d' y='%d' text-anchor='end'>%s</text>\n",
sx-6, sy-6, stats[s].bus);
}
}
}
} */
