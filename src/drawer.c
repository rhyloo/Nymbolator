#include "headers.h"
#include "astar.h"
#include "drawer.h"

/* ----------------- Main (your test case) ----------------- */
int main(void){

    // Instancia: clk25gen
    Port clk25gen_ports[] = {
        {"clk50", "in",  "clk50", 0,0},
        {"clk25", "out", "clk25",  0,0}
    };
    Component clk25gen = {"clk25gen", clk25gen_ports, 2, 0,0, 0,0};

    // Instancia: debounce_circuit
    Port debounce_ports[] = {
        {"clk",   "in",  "clk50",  0,0},
        {"input", "in",  "pb",     0,0},
        {"output","out", "resend", 0,0}
    };
    Component debounce_circuit = {"debounce_circuit", debounce_ports, 3, 0,0, 0,0};

    // Instancia: ov7670_controller
    Port ov7670_ctrl_ports[] = {
        {"clk",       "in",  "clk50",         0,0},
        {"resend",    "in",  "resend",        0,0},
        {"sioc",      "out", "ov7670_sioc",   0,0},
        {"siod",      "out", "ov7670_siod",   0,0},
        {"conf_done", "out", "led",           0,0},
        {"pwdn",      "out", "ov7670_pwdn",   0,0},
        {"reset",     "out", "ov7670_reset",  0,0},
        {"xclk_in",   "in",  "clk25",         0,0},
        {"xclk_out",  "out", "ov7670_xclk",   0,0}
    };
    Component ov7670_controller = {"ov7670_controller", ov7670_ctrl_ports, 9, 0,0, 0,0};

    // Instancia: ov7670_capture
    Port ov7670_cap_ports[] = {
        {"pclk", "in",  "ov7670_pclk", 0,0},
        {"vsync","in",  "ov7670_vsync",0,0},
        {"href", "in",  "ov7670_href", 0,0},
        {"d",    "in",  "ov7670_data", 0,0},
        {"addr", "out", "wr_a",        0,0},
        {"dout", "out", "wr_d",        0,0},
        {"we",   "out", "wren",        0,0}
    };
    Component ov7670_capture = {"ov7670_capture", ov7670_cap_ports, 7, 0,0, 0,0};

    // Instancia: frame_buffer
    Port frame_buf_ports[] = {
        {"weA",  "in",  "wren",        0,0},
        {"clkA", "in",  "ov7670_pclk", 0,0},
        {"addrA","in",  "wr_a",        0,0},
        {"dinA", "in",  "wr_d",        0,0},
        {"clkB", "in",  "clk25",       0,0},
        {"addrB","in",  "rd_a",        0,0},
        {"doutB","out", "rd_d",        0,0}
    };
    Component frame_buffer = {"frame_buffer", frame_buf_ports, 7, 0,0, 0,0};

    // Instancia: address_generator
    Port addr_gen_ports[] = {
        {"clk25",  "in",  "clk25",        0,0},
        {"enable", "in",  "active",       0,0},
        {"vsync",  "in",  "vga_vsync_sig",0,0},
        {"address","out", "rd_a",         0,0}
    };
    Component address_generator = {"address_generator", addr_gen_ports, 4, 0,0, 0,0};

    // Instancia: vga_imagegenerator
    Port vga_img_ports[] = {
        {"Data_in",     "in",  "rd_d",    0,0},
        {"active_area", "in",  "active",  0,0},
        {"RGB_out",     "out", "vga_rgb", 0,0}
    };
    Component vga_imagegenerator = {"vga_imagegenerator", vga_img_ports, 3, 0,0, 0,0};

    // Instancia: VGA_timing_synch
    Port vga_timing_ports[] = {
        {"clk25",      "in",  "clk25",         0,0},
        {"Hsync",      "out", "vga_hsync",     0,0},
        {"Vsync",      "out", "vga_vsync_sig", 0,0},
        {"activeArea", "out", "active",        0,0}
    };
    Component VGA_timing_synch = {"VGA_timing_synch", vga_timing_ports, 4, 0,0, 0,0};
  /* ----- Pack in array ----- */
  Component comps[] = {
    clk25gen,
    debounce_circuit,
    ov7670_controller,
    ov7670_capture,
    frame_buffer,
    address_generator,
    vga_imagegenerator,
    VGA_timing_synch
  };
  size_t N = sizeof(comps)/sizeof(comps[0]);

  Initialization();

  /* ----- Layout & pin placement ----- */
  int canvas_w = 1000, canvas_h = 900;
  for (size_t i = 0; i < N; i++){
    auto_layout(&comps[i], (int)i);
    place_ports(&comps[i]);
    // Expand canvas if needed
    canvas_w = max(canvas_w, comps[i].x + comps[i].w + 120);
    canvas_h = max(canvas_h, comps[i].y + comps[i].h + 120);
  }

  /* ----- Emit SVG ----- */
  FILE *f = fopen("output/diagram.svg","w");
  if (!f) { perror("diagram.svg"); return 1; }

  svg_header(f, canvas_w, canvas_h);

  // Draw blocks first
  for (size_t i = 0; i < N; i++) draw_component(f, &comps[i]);
  // Then draw interconnects (so they appear on top)
  draw_connections(f, comps, N);

  svg_footer(f);
  fclose(f);

  printf("✅ diagram.svg generated.\n");

  FreeMemory();
  return 0;
}

/* ----------------- Small helpers ----------------- */
static int max(int a, int b){ return a > b ? a:b;}

static void auto_layout(Component *c, int idx) {
  // Simple grid layout: 3 columns
  const int COLS = 3;
  const int GRID_W = 280;
  const int GRID_H = 220;
  const int MARGIN_X = 60;
  const int MARGIN_Y = 60;

  int col = idx % COLS;
  int row = idx / COLS;

  c->x = MARGIN_X + col * GRID_W;
  c->y = MARGIN_Y + row * GRID_H;

  // Height based on ports, width fixed-ish
  c->w = 180;
  c->h = (int)(max((int)c->port_count, 1) * 22 + 32);
  DefineRestriction(c->x,c->y,c->w,c->h);
}

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

/* ----------------- SVG drawing ----------------- */
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
}
