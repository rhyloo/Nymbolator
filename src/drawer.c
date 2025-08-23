// -*- compile-command: "(cd ../ && make compile)"; -*-
#include "main.h"
#include "drawer.h"

static FILE *svg_file;

static void CreateFile(){
  svg_file = fopen("output/diagram.svg","w");
  if(!svg_file){
    perror("Can't create diagram.svg");
  } 
}

static void WriteHeader() {
  fprintf(svg_file,
	  "<svg xmlns='http://www.w3.org/2000/svg' width='%d' height='%d'>\n"
	  "<style>"
	  "text{font-family:system-ui,Segoe UI,Roboto,sans-serif;font-size:12px}"
	  "rect{rx:3;ry:3;fill:#f3f4f6;stroke:#111827;stroke-width:1.5}"
	  "line{stroke:#111827;stroke-width:1.5}"
	  "circle{fill:#111827}"
	  "path{stroke:#2563eb;stroke-width:2;fill:none}"
	  ".label{fill:#111827}"
	  ".bus{fill:#2563eb}"
	  "</style>\n"
	  "<defs>"
	  "<marker id='arrow' markerWidth='10' markerHeight='10' refX='10' refY='3' orient='auto' markerUnits='strokeWidth'>"
	  "<path d='M0,0 L0,6 L9,3 z' fill='#2563eb'/></marker>"
	  "</defs>\n"
	  "<rect x='0' y='0' width='%d' height='%d' fill='none' stroke='#111827' stroke-width='1'/>\n",
	  *svg_width, *svg_height, *svg_width, *svg_height);
}

static void WriteFooter(){
  fprintf(svg_file,"</svg>\n");
}

void DrawComponents(StructComponent *component, int number_components){
  for(int i = 0; i < number_components; i++){
      int in_counter = 0,out_counter = 0;
    fprintf(svg_file, "<rect x='%d' y='%d' width='%d' height='%d'/>\n", component[i].pos_x, component[i].pos_y, component[i].width-GAP, component[i].height-GAP);
    fprintf(svg_file, "<text class='label' x='%d' y='%d' text-anchor='middle' style='font-weight:bold;'>%s</text>\n", component[i].pos_x + (component[i].width-GAP)/2, component[i].pos_y - 5, component[i].component_name);
   
    for (size_t j = 0; j < component[i].port_count; j++) {
      StructPort *p = &component[i].ports[j];
      if(strcmp(p->direction,"in") == 0){
	p->x = component[i].pos_x;
	p->y = component[i].pos_y + (in_counter+1)*10*1.5;
	      fprintf(svg_file, "<text class='label' x='%d' y='%d' text-anchor='start'>%s</text>\n", p->x+10, p->y+10, p->name);
	in_counter++;
      }else{
	p->x = component[i].pos_x + component[i].width-GAP;
	p->y = component[i].pos_y + (out_counter+1)*10*1.5;
		      fprintf(svg_file, "<text class='label' x='%d' y='%d' text-anchor='end'>%s</text>\n", p->x-10, p->y+10, p->name);
	out_counter++;
      }
      int value_line_x, value_line_y;
      int line_len = 4; // longitud de la línea
      if(strcmp(p->direction,"in") == 0){
        fprintf(svg_file, 
		"<line x1='%d' y1='%d' x2='%d' y2='%d' style='stroke:blue;stroke-width:2'/>\n", 
		p->x-5, p->y+5, p->x-5-line_len, p->y+5);
	value_line_x = p->x-9;
	value_line_y = p->y+5;
      } else {
        fprintf(svg_file, 
          "<line x1='%d' y1='%d' x2='%d' y2='%d' style='stroke:blue;stroke-width:2'/>\n", 
          p->x+5, p->y+5, p->x+5+line_len, p->y+5);
	value_line_x = p->x;
	value_line_y = p->y;
      }
            fprintf(svg_file, "<rect x='%d' y='%d' width='10' height='10' style='fill:green;stroke:black;rx:0;ry:0'/>\n", p->x-5, p->y);
	    p->x = value_line_x;
	    	    p->y = value_line_y;
    }
  }
}

void StartSVG(){
  CreateFile();
  WriteHeader();
}

void FinishSVG(){
  WriteFooter();
  printf("✅ diagram.svg generated.\n");
}

/*   svg_header(f, canvas_w, canvas_h);
   
// Draw blocks first
for (size_t i = 0; i < N; i++) draw_component(f, &comps[i]);
// Then draw interconnects (so they appear on top)
draw_connections(f, comps, N);
   
svg_footer(f);
fclose(f);
   
   
   
FreeMemory();
return 0;
} */


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
  } */

/* ----------------- SVG drawing ----------------- */




/* static void draw_component(FILE *f, const Component *c){
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
   } */

/* static void route_bus(FILE *f, const Port *src, const Port *dst){
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
   } */

/* static void route_bus(FILE *f, const Port *src, const Port *dst){
   PathFinderAStar(src->x,src->y,dst->x,dst->y);       
   SimplifyPathsToCorners();
   fprintf(f, "<path d='M %d %d L %d %d L %d %d L %d %d' marker-end='url(#arrow)'/>\n",
   src->x, src->y, midX, src->y, midX, dst->y, dst->x, dst->y);
   /\* // Orthogonal path: src -> midX -> vertical to dst.y -> dst
   int midX;
   // If going left-to-right, mid between; otherwise create a dogleg past the blocks
   if (src->x <= dst->x) {
   midX = (src->x + dst->x)/2;
   } else {
   // if destination is to the left, push a bit to the right first to avoid intersecting the source block edge
   midX = src->x + 40;
   } *\/
       
   } */

/* static void draw_connections(FILE *f, Component *comps, size_t n){
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

#define CELL_SIZE 1      // ajusta según tu escala

// Esta función asume que cada Node tiene un flag path=true si forma parte del camino
// y que tienes un canvas SVG o similar donde se dibuja.
void DrawPaths(void) {
    if (num_paths <= 0) return;

    fprintf(svg_file, "<g id='paths' stroke='red' fill='none' stroke-width='2'>\n");

    for (int i = 0; i < num_paths; i++) {
        int len = paths_lengths[i];
        if (len <= 1) continue;

        // Dibujar el path i como una polilínea
        fprintf(svg_file, "<polyline points='");
        for (int j = 0; j < len; j++) {
            int px = paths[i][j].coord_x * CELL_SIZE;
            int py = paths[i][j].coord_y * CELL_SIZE;
            fprintf(svg_file, "%d,%d ", px, py);
        }
        fprintf(svg_file, "' />\n");
    }

    fprintf(svg_file, "</g>\n");
}
