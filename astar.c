#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>

#define COLUMNS 50
#define ROWS 50

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

static uint8_t **matrix;
static Node nodes[ROWS][COLUMNS];

static Point **paths;
static int *paths_lengths;
static int num_paths;

static int direction_x[4] = {0, 0, -1, 1};
static int direction_y[4] = {-1, 1, 0, 0};

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
  matrix = malloc(ROWS * sizeof(uint8_t*));
  for(int i = 0; i < ROWS; i++){
    matrix[i] = malloc(COLUMNS * sizeof(uint8_t));
    for(int j = 0; j < COLUMNS; j++)
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

static void Initialization(){
  InitializationMatrix();
  InitializationPaths();
  InitializationNodes(true);
}

// ---------------------------- A* -----------------------
static void PathFinderAStar(int start_x, int start_y, int end_x, int end_y){
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

// ---------------------------- Restricciones y visual -----------------
static void DefineRestriction(int pos_x, int pos_y, int width, int height){
  for(int i = pos_y; i < pos_y + height && i < ROWS; i++)
    for(int j = pos_x; j < pos_x + width && j < COLUMNS; j++)
      matrix[i][j] = 1;
}

static void DrawMatrixWithPath(int path_index){
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
}

static void VisualizePaths(){
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
}

static void SimplifyPathsToCorners() {
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
static void FreeMemory(){
  for(int i = 0; i < ROWS; i++) free(matrix[i]);
  free(matrix);

  for(int i = 0; i < num_paths; i++) free(paths[i]);
  free(paths);
  free(paths_lengths);
}

// ---------------------------- Main -----------------------------------
int main(){
  Initialization();

  // ------------------------ Ejemplo de obstáculos ------------------------
  DefineRestriction(0, 0, 5, 3);    
  DefineRestriction(9, 9, 3, 5);    
  DefineRestriction(20, 20, 5, 5);  
  DefineRestriction(45, 0, 5, 5);   
  DefineRestriction(0, 45, 5, 5);   
  DefineRestriction(40, 40, 5, 5);  

  // ------------------------ Casos de test ------------------------
  PathFinderAStar(12, 10, 44, 1);       
  PathFinderAStar(21, 25, 44, 1);       
  PathFinderAStar(41, 45, 44, 1);       

  SimplifyPathsToCorners();

  printf("Matriz base:\n");
  for(int i=0;i<ROWS;i++){
    for(int j=0;j<COLUMNS;j++) printf("%c",matrix[i][j]==1?'#':'.');
    printf("\n");
  }

  VisualizePaths();

  FreeMemory();
  return 0;
}
