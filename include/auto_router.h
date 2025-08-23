#ifndef AUTOROUTER_H
#define AUTOROUTER_H

#define COLUMNS 1024
#define ROWS 1024

extern Point **paths;        // array de paths generados
extern int *paths_lengths;   // longitudes de cada path
extern int num_paths;        // número de paths generados

// ---------------------------- Functions --------------------------------

void Initialization();
void DrawMatrix();
void DefineRestriction(StructComponent *component, int number_components);
void PathFinderAStar(int start_x, int start_y, int end_x, int end_y);
void SimplifyPathsToCorners();
void FreeMemory();
void AutoRouter(StructComponent *component, int number_components);
#endif
