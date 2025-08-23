#include "main.h"
#include "stb_rect_pack.h"
#include "astar.h"
#include "auto_placer.h"

static int GetMaxNumberPorts(StructPort *port, int port_count);
static int GetMaxNameSize(StructPort *port, int port_count);
/* static void CalcPosComponents(StructComponent *component, int number_components); */
static void CalcSizeComponents(StructComponent *component, int number_components);

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


#define GRID_W 100
#define GRID_H 100
#define MARGIN_X 60
#define MARGIN_Y 60

/* static void CalcPosComponents(StructComponent *component, int number_components) {
     int columns = ceil(sqrt(number_components));
     printf("Columns:%d\n", columns);
     int rows = ceil((float)number_components / columns);
     printf("Rows:%d\n", rows);
     int needed_width = GAP;
     int needed_height = GAP;
     int max_width_col[columns];
     int max_height_row[rows];
     
     for(int i = 0; i < number_components; i++){
       int col = i % columns;
       int row = i / columns;
   
       component[i].pos_x = MARGIN_X + col * GRID_W;
       component[i].pos_y = MARGIN_Y + row * GRID_H;
     }
   } */

void PackRectanglesAuto(StructComponent *components, int number_components) {
  // --- Paso 1: calcular área total y dimensiones máximas ---
  long total_area = 0;
  int max_w = 0, max_h = 0;
  for (int i = 0; i < number_components; i++) {
    total_area += components[i].width * components[i].height;
    if (components[i].width > max_w) max_w = components[i].width;
    if (components[i].height > max_h) max_h = components[i].height;
  }

  int side = (int) sqrt((double) total_area);
  int atlas_w = (side < max_w) ? max_w : side;
  int atlas_h = (side < max_h) ? max_h : side;

  stbrp_rect *rects = malloc(sizeof(stbrp_rect) * number_components);
  if (!rects) {
    fprintf(stderr, "Error: malloc failed\n");
    return;
  }

  int success = 0;
  while (!success) {
    // Inicializar rectángulos
    for (int i = 0; i < number_components; i++) {
      rects[i].id = i;
      rects[i].w = components[i].width;
      rects[i].h = components[i].height;
    }

    // Crear contexto para este atlas
    stbrp_context context;
    stbrp_node *nodes = malloc(sizeof(stbrp_node) * atlas_w);
    if (!nodes) {
      fprintf(stderr, "Error: malloc failed\n");
      free(rects);
      return;
    }

    stbrp_init_target(&context, atlas_w, atlas_h, nodes, atlas_w);
    success = stbrp_pack_rects(&context, rects, number_components);

    free(nodes);

    if (!success) {
      // Si falla, agrandar canvas sumando +20 al lado más pequeño
      if (atlas_w < atlas_h)
	atlas_w += GAP*5;
      else
	atlas_h += GAP*5;
    }
  }

  // Transferir posiciones a components
  for (int i = 0; i < number_components; i++) {
    components[rects[i].id].pos_x = rects[i].x+GAP;
    components[rects[i].id].pos_y = rects[i].y+GAP;
  }

  free(rects);
  *svg_height=atlas_h;
  *svg_width=atlas_w;
  printf("Canvas final: %dx%d\n", atlas_w, atlas_h);
}

static int GetNameSize(char *name) {
  int text_size = 0;
  for (; *name != '\0'; name++) {
    text_size++;
  }
  return text_size;
}

static int BestFit(int a, int b){
  if(a > b){
    return a;
  }else{
    return (a+b)/2;
  }
  
}

static void CalcSizeComponents(StructComponent *component, int number_components) {
  for(int i = 0; i < number_components; i++){
    component[i].width  = (10 * BestFit(GetMaxNameSize(component[i].ports, component[i].port_count),GetNameSize(component[i].component_name)) + 20)+GAP;  
    component[i].height = (GetMaxNumberPorts(component[i].ports, component[i].port_count) * 20 + 10)+GAP; 
    /* printf("Component: %s\n", component[i].component_name);
       printf("NumberPorts_height: %d\n", component[i].height - COMPONENT_HEIGHT_OFFSET); */
  }
}

StructComponent* AutoLayout(StructComponent *component, int number_components) {
  CalcSizeComponents(component, number_components);
  PackRectanglesAuto(component, number_components);
  /* DefineRestriction(component, number_components); */
  return component;
}
