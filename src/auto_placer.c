#include "stb_rect_pack.h"

#include "main.h"
#include "tools.h"

#include "auto_placer.h"

/*====================*/
/*  Static Helpers    */
/*====================*/
static void CalcSizeComponents(StructComponent *component, int number_components);
static void PackRectanglesAuto(StructComponent *components, int number_components);
static int EstimateBestFitting(int a, int b);
  
StructComponent* AutoLayout(StructComponent *component, int number_components) {
  CalcSizeComponents(component, number_components);
  PackRectanglesAuto(component, number_components);
  return component;
}

static void PackRectanglesAuto(StructComponent *components, int number_components) {
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

    int step = 5; // inicial

    if (!success) {
      if (atlas_w < atlas_h){
	atlas_w += step;
      }else{
        atlas_h += step;
      }
      if (step > 1){
	step--;
      }
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



/*
 * EstimateBestFitting - Returns an estimated "best fit" size for a component.
 *
 * Parameters:
 *   max_name_size_port   - Maximum name length among the component's ports
 *   component_name_size  - Length of the component's own name
 *
 * Returns:
 *   If max_name_size_port is larger, returns it.
 *   Otherwise, returns the average of max_name_size_port and component_name_size.
 *
 * Description:
 *   Used to estimate a suitable width for a component when laying out
 *   its ports and name. Prefers the larger value, but averages if the
 *   component's name is longer.
 */
static int EstimateBestFitting(int max_name_size_port, int component_name_size){
    if (max_name_size_port > component_name_size) {
        return max_name_size_port;
    } else {
        return (max_name_size_port + component_name_size) / 2;
    }
}



/*
 * CalcSizeComponents - Calculates width and height for each component.
 *
 * Parameters:
 *   component         - array of StructComponent
 *   number_components - number of components in the array
 *
 * Description:
 *   For each component, the width is based on the longer of the component
 *   name or the longest port name, scaled by character width plus padding.
 *   The height is based on the number of ports multiplied by port height plus padding.
 */
static void CalcSizeComponents(StructComponent *component, int number_components) {
  for(int i = 0; i < number_components; i++){
    component[i].width  = (10 * EstimateBestFitting(GetMaxNameSize(component[i].ports, component[i].port_count),GetNameSize(component[i].component_name)) + 20)+GAP;  
    component[i].height = (GetMaxNumberPorts(component[i].ports, component[i].port_count) * 20 + 10)+GAP; 
    /* printf("Component: %s\n", component[i].component_name);
       printf("NumberPorts_height: %d\n", component[i].height - COMPONENT_HEIGHT_OFFSET); */
  }
}
