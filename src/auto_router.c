#include "main.h"
#include "auto_router.h"

static uint8_t **matrix;
static Node nodes[ROWS][COLUMNS];

static int direction_x[4] = {0, 0, -1, 1};
static int direction_y[4] = {-1, 1, 0, 0};

static bool IsValidPoint(int x, int y);
static int heuristic(int x1, int y1, int x2, int y2);
static void ValidNeighbor(int child_x, int child_y, int neighbor_x, int neighbor_y, int end_x, int end_y);
static void AreValidPoints(int start_x, int start_y, int end_x, int end_y);
static Node* GetLowestF();
static void SavePathFound(int end_x, int end_y);
static void InitializationMatrix();
static void InitializationPaths();
static void InitializationNodes(bool init);

/* static void DrawMatrixWithPath(int path_index);
   static void VisualizePaths(); */

void DrawMatrix(){
  for(int i=0;i<*svg_height;i++){
    for(int j=0;j<*svg_width;j++){
      char c='.';
      if(matrix[i][j]==1) c='#';
      printf("%c",c);
    }
    printf("\n");
  }
}

// ---------------------------- Helpers --------------------------------
static bool IsValidPoint(int x, int y){
  return (matrix[y][x] == 0); // 0 = libre
}

static int heuristic(int x1, int y1, int x2, int y2){
  return abs(x1 - x2) + abs(y1 - y2); // Manhattan
}

static void ValidNeighbor(int child_x, int child_y, int neighbor_x, int neighbor_y, int end_x, int end_y){
  if(neighbor_x < 0 || neighbor_x >= COLUMNS || neighbor_y < 0 || neighbor_y >= ROWS) return;
  if(!IsValidPoint(neighbor_x, neighbor_y)) return;
  if(nodes[neighbor_y][neighbor_x].list_closed) return;

  int g_new = nodes[child_y][child_x].g_cost + 1;
  if(!nodes[neighbor_y][neighbor_x].list_open || g_new < nodes[neighbor_y][neighbor_x].g_cost){
    nodes[neighbor_y][neighbor_x].g_cost = g_new;
    nodes[neighbor_y][neighbor_x].h_cost = heuristic(neighbor_x, neighbor_y, end_x, end_y);
    nodes[neighbor_y][neighbor_x].f_cost = nodes[neighbor_y][neighbor_x].g_cost + nodes[neighbor_y][neighbor_x].h_cost;
    nodes[neighbor_y][neighbor_x].parent.coord_x = child_x;
    nodes[neighbor_y][neighbor_x].parent.coord_y = child_y;
    nodes[neighbor_y][neighbor_x].child.coord_x = neighbor_x;
    nodes[neighbor_y][neighbor_x].child.coord_y = neighbor_y;
    nodes[neighbor_y][neighbor_x].list_open = true;
  }
}

static void AreValidPoints(int start_x, int start_y, int end_x, int end_y){
  if(start_x < 0 || start_x >= COLUMNS || start_y < 0 || start_y >= ROWS){
    fprintf(stderr, "Start fuera de rango\n");
    exit(1);
  }
  if(end_x < 0 || end_x >= COLUMNS || end_y < 0 || end_y >= ROWS){
    fprintf(stderr, "End fuera de rango\n");
    exit(1);
  }
  if(!IsValidPoint(start_x, start_y)){
    fprintf(stderr, "Start está en un obstáculo\n");
    exit(1);
  }
  if(!IsValidPoint(end_x, end_y)){
    fprintf(stderr, "End está en un obstáculo\n");
    exit(1);
  }
}

static Node* GetLowestF(){
  Node *best = NULL;
  for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLUMNS; j++){
      if(nodes[i][j].list_open){
	if(best == NULL || nodes[i][j].f_cost < best->f_cost)
	  best = &nodes[i][j];
      }
    }
  }
  return best;
}

static void SavePathFound(int end_x, int end_y){
  int px = end_x, py = end_y;
  int length = 0;

  // contar longitud
  while(px != -1 && py != -1){
    length++;
    int tmp_x = nodes[py][px].parent.coord_x;
    int tmp_y = nodes[py][px].parent.coord_y;
    px = tmp_x;
    py = tmp_y;
  }

  // realloc para nuevo path
  num_paths++;
  paths = realloc(paths, num_paths * sizeof(Point*));
  paths_lengths = realloc(paths_lengths, num_paths * sizeof(int));

  paths[num_paths-1] = malloc(length * sizeof(Point));
  paths_lengths[num_paths-1] = length;

  // reconstruir path en orden
  px = end_x; py = end_y;
  for(int i = length-1; i >= 0; i--){
    paths[num_paths-1][i].coord_x = px;
    paths[num_paths-1][i].coord_y = py;
    /* matrix[py][px] = 2; // marcar path en matriz */
    int tmp_x = nodes[py][px].parent.coord_x;
    int tmp_y = nodes[py][px].parent.coord_y;
    px = tmp_x; py = tmp_y;
  }
}

// ---------------------------- Inicializaciones -----------------------
static void InitializationMatrix(){
  matrix = malloc(*svg_height * sizeof(uint8_t*));
  for(int i = 0; i < *svg_height; i++){
    matrix[i] = malloc(*svg_width * sizeof(uint8_t));
    for(int j = 0; j < *svg_width; j++)
      matrix[i][j] = 0;
  }
}

static void InitializationPaths(){
  paths = NULL;
  paths_lengths = NULL;
  num_paths = 0;
}

static void InitializationNodes(bool init){
  for(int i = 0; i < ROWS; i++)
    for(int j = 0; j < COLUMNS; j++){
      if(init){
	nodes[i][j].child.coord_x = j;
	nodes[i][j].child.coord_y = i;
      }
      nodes[i][j].g_cost = INT_MAX;
      nodes[i][j].h_cost = 0;
      nodes[i][j].f_cost = INT_MAX;
      nodes[i][j].list_open = false;
      nodes[i][j].list_closed = false;
      nodes[i][j].parent.coord_x = -1;
      nodes[i][j].parent.coord_y = -1;
    }
}



// ---------------------------- A* -----------------------
void PathFinderAStar(int start_x, int start_y, int end_x, int end_y){
  printf("A* start=(%d,%d) end=(%d,%d)\n", start_x, start_y, end_x, end_y);
  AreValidPoints(start_x, start_y, end_x, end_y);
  InitializationNodes(false);

  // inicializar nodo de inicio
  nodes[start_y][start_x].g_cost = 0;
  nodes[start_y][start_x].h_cost = heuristic(start_x, start_y, end_x, end_y);
  nodes[start_y][start_x].f_cost = nodes[start_y][start_x].h_cost;
  nodes[start_y][start_x].parent.coord_x = -1;
  nodes[start_y][start_x].parent.coord_y = -1;
  nodes[start_y][start_x].child.coord_x = start_x;
  nodes[start_y][start_x].child.coord_y = start_y;
  nodes[start_y][start_x].list_open = true;

  while(true){
    Node *current = GetLowestF();
    if(current == NULL){
      printf("No path found!\n");
      return;
    }

    if(current->child.coord_x == end_x && current->child.coord_y == end_y){
      SavePathFound(end_x, end_y);
      return;
    }

    current->list_open = false;
    current->list_closed = true;

    for(int k = 0; k < 4; k++){
      int neighbor_x = current->child.coord_x + direction_x[k];
      int neighbor_y = current->child.coord_y + direction_y[k];
      ValidNeighbor(current->child.coord_x, current->child.coord_y, neighbor_x, neighbor_y, end_x, end_y);
    }
  }
}

void Initialization(){
  InitializationMatrix();
  InitializationPaths();
  InitializationNodes(true);
}


void ConnectPorts(StructComponent *component, int number_components){
  for (size_t i = 0; i < number_components; i++) {
    for (size_t j = 0; j < component[i].port_count; j++) {

      StructPort *p_out = &component[i].ports[j];

      if (strcmp(p_out->direction,"out")!=0){
	continue;
      }

      for (size_t a = 0; a < number_components; a++) {
	for (size_t b = 0; b < component[a].port_count; b++) {

	  StructPort *p_in = &component[a].ports[b];

	  if (strcmp(p_in->direction,"in")==0 && strcmp(p_in->bus, p_out->bus)==0) {
	    PathFinderAStar(p_out->x, p_out->y, p_in->x-1, p_in->y);           
	  }
	}
      }
    }
  }
}

// Devuelve los paths calculados
Point **get_paths(void) { return paths; }
int *get_paths_lengths(void) { return paths_lengths; }
int get_num_paths(void) { return num_paths; }

// Imprime todos los paths guardados en consola
void print_paths_debug(void) {
    printf("=== Paths calculados: %d ===\n", num_paths);

    for (int i = 0; i < num_paths; i++) {
        printf("Path %d (len=%d): ", i, paths_lengths[i]);
        for (int j = 0; j < paths_lengths[i]; j++) {
            printf("(%d,%d)", paths[i][j].coord_x, paths[i][j].coord_y);
            if (j < paths_lengths[i] - 1) printf(" -> ");
        }
        printf("\n");
    }
}

void AutoRouter(StructComponent *component, int number_components){
  Initialization();
  DefineRestriction(component,number_components);
  /* DrawMatrix(); */
  ConnectPorts(component, number_components);
  SimplifyPathsToCorners();
  print_paths_debug();
}



void DefineRestriction(StructComponent *component, int number_components){
  for(int k = 0; k < number_components; k++){
    int pos_x = component[k].pos_x;
    int pos_y = component[k].pos_y;
    int width = component[k].width-GAP;
    int height = component[k].height-GAP;
    for(int i = pos_y; i < pos_y + height && i < *svg_height; i++){
      for(int j = pos_x; j < pos_x + width && j < *svg_width; j++){
	matrix[i][j] = 1;
      }
    }
  }
}

/* static void DrawMatrixWithPath(int path_index){
     if(num_paths==0){ printf("No hay paths calculados\n"); return; }
     if(path_index<0 || path_index>=num_paths){ printf("Índice inválido\n"); return; }
   
     for(int i=0;i<ROWS;i++){
       for(int j=0;j<COLUMNS;j++){
         char c='.';
         if(matrix[i][j]==1) c='#';
         for(int k=0;k<paths_lengths[path_index];k++)
   	if(paths[path_index][k].coord_x==j && paths[path_index][k].coord_y==i){ c='*'; break; }
         printf("%c",c);
       }
       printf("\n");
     }
   } */

/* static void VisualizePaths(){
     if(num_paths==0){ printf("No hay paths calculados\n"); return; }
     int choice=0; char cmd;
     while(1){
       printf("\nMostrando path %d de %d:\n",choice,num_paths-1);
       DrawMatrixWithPath(choice);
       printf("\nComandos: n=next, p=prev, q=quit: ");
       cmd=getchar(); while(getchar()!='\n');
       if(cmd=='q') break;
       else if(cmd=='n') choice=(choice+1)%num_paths;
       else if(cmd=='p') choice=(choice-1+num_paths)%num_paths;
     }
   } */

void SimplifyPathsToCorners() {
  for(int p = 0; p < num_paths; p++){
    int len = paths_lengths[p];
    if(len <= 2) continue; // no hay que simplificar

    Point *old_path = paths[p];
    Point *new_path = malloc(len * sizeof(Point));

    int new_len = 1;
    new_path[0] = old_path[0]; // siempre conservar el inicio

    int dx_prev = old_path[1].coord_x - old_path[0].coord_x;
    int dy_prev = old_path[1].coord_y - old_path[0].coord_y;

    for(int i = 1; i < len-1; i++){
      int dx = old_path[i+1].coord_x - old_path[i].coord_x;
      int dy = old_path[i+1].coord_y - old_path[i].coord_y;

      if(dx != dx_prev || dy != dy_prev){
	// cambio de dirección → esquina
	new_path[new_len++] = old_path[i];
      }

      dx_prev = dx;
      dy_prev = dy;
    }

    new_path[new_len++] = old_path[len-1]; 
    free(paths[p]);
    paths[p] = new_path;
    paths_lengths[p] = new_len;
  }
}

// ---------------------------- Liberación memoria --------------------
void FreeMemory(){
  for(int i = 0; i < ROWS; i++) free(matrix[i]);
  free(matrix);

  for(int i = 0; i < num_paths; i++) free(paths[i]);
  free(paths);
  free(paths_lengths);
}


