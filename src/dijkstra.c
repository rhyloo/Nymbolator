/* #include <stdio.h>
   #include <stdlib.h>
   #include <stdbool.h>
   #include <limits.h>
   #include <string.h>
   
   #define SIZE_W 100
   #define SIZE_H 100
   
   typedef struct {
       int x, y;
       int g, h, f;
       int parent_x, parent_y;
       bool in_open, in_closed;
   } Node;
   
   static bool **matrix;
   static Node nodes[SIZE_H][SIZE_W];
   static bool path_matrix[SIZE_H][SIZE_W]; // para marcar el camino
   static bool path_matrix2[SIZE_H][SIZE_W]; // para marcar el primer camino
   
   int dx[4] = {0, 0, -1, 1};
   int dy[4] = {-1, 1, 0, 0};
   
   int heuristic(int x1, int y1, int x2, int y2) {
       return abs(x1 - x2) + abs(y1 - y2);
   }
   
   void try_add(int cx, int cy, int nx, int ny, int end_x, int end_y) {
       if(nx < 0 || nx >= SIZE_W || ny < 0 || ny >= SIZE_H) return;
       if(matrix[ny][nx]) return; // obstáculo
       if(nodes[ny][nx].in_closed) return;
   
       int g_new = nodes[cy][cx].g + 1;
       if(!nodes[ny][nx].in_open || g_new < nodes[ny][nx].g) {
           nodes[ny][nx].g = g_new;
           nodes[ny][nx].h = heuristic(nx, ny, end_x, end_y);
           nodes[ny][nx].f = nodes[ny][nx].g + nodes[ny][nx].h;
           nodes[ny][nx].parent_x = cx;
           nodes[ny][nx].parent_y = cy;
           nodes[ny][nx].in_open = true;
       }
   }
   
   Node* get_lowest_f() {
       Node *best = NULL;
       for(int i = 0; i < SIZE_H; i++){
           for(int j = 0; j < SIZE_W; j++){
               if(nodes[i][j].in_open) {
                   if(best == NULL || nodes[i][j].f < best->f)
                       best = &nodes[i][j];
               }
           }
       }
       return best;
   }
   
   void astar(int start_x, int start_y, int end_x, int end_y) {
       /\* Inicializa nodos y borra path_matrix *\/
       for(int i = 0; i < SIZE_H; i++)
           for(int j = 0; j < SIZE_W; j++){
               nodes[i][j].x = j;
               nodes[i][j].y = i;
               nodes[i][j].g = INT_MAX;
               nodes[i][j].h = 0;
               nodes[i][j].f = INT_MAX;
               nodes[i][j].in_open = false;
               nodes[i][j].in_closed = false;
               nodes[i][j].parent_x = -1;
               nodes[i][j].parent_y = -1;
               path_matrix[i][j] = false;
           }
   
       /\* Comprobaciones básicas de validez *\/
       if(start_x < 0 || start_x >= SIZE_W || start_y < 0 || start_y >= SIZE_H) {
           fprintf(stderr, "Start fuera de rango\n");
           return;
       }
       if(end_x < 0 || end_x >= SIZE_W || end_y < 0 || end_y >= SIZE_H) {
           fprintf(stderr, "End fuera de rango\n");
           return;
       }
       if(matrix[start_y][start_x]) {
           fprintf(stderr, "Start está en un obstáculo\n");
           return;
       }
       if(matrix[end_y][end_x]) {
           fprintf(stderr, "End está en un obstáculo\n");
           return;
       }
   
       nodes[start_y][start_x].g = 0;
       nodes[start_y][start_x].h = heuristic(start_x, start_y, end_x, end_y);
       nodes[start_y][start_x].f = nodes[start_y][start_x].h;
       nodes[start_y][start_x].in_open = true;
   
       while(true) {
           Node *current = get_lowest_f();
           if(current == NULL) {
               printf("No path found!\n");
               return;
           }
   
           if(current->x == end_x && current->y == end_y) {
               int px = end_x, py = end_y;
               while(px != -1 && py != -1) {
                   path_matrix[py][px] = true;
                   int tmp_x = nodes[py][px].parent_x;
                   int tmp_y = nodes[py][px].parent_y;
                   px = tmp_x; py = tmp_y;
               }
               return;
           }
   
           current->in_open = false;
           current->in_closed = true;
   
           for(int k = 0; k < 4; k++){
               int nx = current->x + dx[k];
               int ny = current->y + dy[k];
               try_add(current->x, current->y, nx, ny, end_x, end_y);
           }
       }
   }
   
   void save_component(int pos_x, int pos_y, int width, int height){
       for(int i = pos_y; i < pos_y + height && i < SIZE_H; i++)
           for(int j = pos_x; j < pos_x + width && j < SIZE_W; j++)
               matrix[i][j] = true;
   }
   
   void draw_matrix() {
       for(int i = 0; i < SIZE_H; i++){
           for(int j = 0; j < SIZE_W; j++){
               if(path_matrix[i][j])
                   printf("*");
               else if(path_matrix2[i][j])
                   printf("*");
               else if(matrix[i][j])
                   printf("#");
               else
                   printf(".");
           }
           printf("\n");
       }
   }
   
   void find_corners(){
     int actual_x, actual_y;
     int medio_x, medio_y;
     int siguiente_x, siguiente_y;
     bool path_matrix_corners[SIZE_H][SIZE_W];
   
     for(int i = 0; i < SIZE_H; i++){
       for(int j = 0; j < SIZE_W; j++){
         if(path_matrix[i][j]){
   	actual_x
         }
       }
     }
     
     memcpy(path_matrix_corners, path_matrix, sizeof(path_matrix));
   }
   
   int main() {
       matrix = malloc(SIZE_H * sizeof(bool*));
       if(!matrix) return 1;
       for(int i = 0; i < SIZE_H; i++){
           matrix[i] = malloc(SIZE_W * sizeof(bool));
           if(!matrix[i]) return 1;
           for(int j = 0; j < SIZE_W; j++)
               matrix[i][j] = false;
       }
   
       save_component(1, 1, 10, 10);
       save_component(20, 1, 10, 10);
       save_component(35, 1, 10, 10);
       save_component(1, 15, 15, 15);
       save_component(30, 30, 20, 20);
   
       astar(50, 30, 16, 17);
     
       memcpy(path_matrix2, path_matrix, sizeof(path_matrix));
   
       astar(50, 30, 16, 18);
   
       find_corners();
   
       draw_matrix();
   
       for(int i = 0; i < SIZE_H; i++)
           free(matrix[i]);
       free(matrix);
   
       return 0;
   } */
