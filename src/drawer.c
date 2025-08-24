// -*- compile-command: "(cd ../ && make compile)"; -*-
#include "main.h"
#include "drawer.h"
#include "auto_router.h"

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
    fprintf(svg_file, "<rect x='%d' y='%d' width='%d' height='%d'/>\n", component[i].pos_x, component[i].pos_y, component[i].width, component[i].height);
    fprintf(svg_file, "<text class='label' x='%d' y='%d' text-anchor='middle' style='font-weight:bold;'>%s</text>\n", component[i].pos_x + (component[i].width)/2, component[i].pos_y - 5, component[i].component_name);
   
    for (size_t j = 0; j < component[i].port_count; j++) {
      /* fprintf(svg_file, "<text class='label' x='%d' y='%d' text-anchor='start'>%s</text>\n", component[i].ports[j].x, component[i].ports[j].y, component[i].ports[j].name); */
	     fprintf(svg_file, "<rect x='%d' y='%d' width='10' height='10' style='fill:green;stroke:black;rx:0;ry:0'/>\n", component[i].ports[j].x, component[i].ports[j].y);
	     fprintf(svg_file,
    "<rect x='%d' y='%d' width='2' height='2' style='fill:red;stroke:none'/>\n",
    component[i].ports[j].x,
    component[i].ports[j].y);
	     fprintf(svg_file,
    "<rect x='%d' y='%d' width='1' height='1' style='fill:yellow;stroke:none'/>\n",
125,
160+5);
    }
  }
}

void StartSVG(){
  CreateFile();
  WriteHeader();
}

void FinishSVG(){
  /* DebugMatrixSVG(svg_file); */
  WriteFooter();
  printf("✅ diagram.svg generated.\n");
}


#define CELL_SIZE 1      // ajusta según tu escala

// Esta función asume que cada Node tiene un flag path=true si forma parte del camino
// y que tienes un canvas SVG o similar donde se dibuja.
void DrawPaths(void) {
    if (num_paths <= 0) return;

    fprintf(svg_file, "<g id='paths' stroke='blue' fill='none' stroke-width='1'>\n");

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
