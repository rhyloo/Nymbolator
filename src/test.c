// -*- compile-command: "(cd ../ && make test)"; -*-
#include <stdio.h>
#include <math.h>

typedef struct {
    float width, height; // tamaño original
    float x, y;          // centro final
} Node;

// Distribución de nodos en rejilla y ajuste automático del SVG
void distribute_and_fit(Node* nodes, int n, float* svg_width, float* svg_height, float gap) {
    if (n == 0) return;

    // Paso 1: columnas y filas aproximadas
    int columns = ceil(sqrt(n));
    int rows = ceil(n / columns);

    // Paso 2: calcular ancho máximo por columna y alto máximo por fila
    float max_width_col[columns];
    float max_height_row[rows];
    for (int c = 0; c < columns; c++) max_width_col[c] = 0;
    for (int r = 0; r < rows; r++) max_height_row[r] = 0;

    for (int i = 0; i < n; i++) {
        int col = i % columns;
        int row = i / columns;
        if (nodes[i].width > max_width_col[col]) max_width_col[col] = nodes[i].width;
        if (nodes[i].height > max_height_row[row]) max_height_row[row] = nodes[i].height;
    }

    // Paso 3: calcular tamaño total necesario
    float needed_width = gap;
    for (int c = 0; c < columns; c++) needed_width += max_width_col[c] + gap;

    float needed_height = gap;
    for (int r = 0; r < rows; r++) needed_height += max_height_row[r] + gap;

    // Ajustar SVG si es menor o mayor al necesario
    *svg_width = needed_width;
    *svg_height = needed_height;

    // Paso 4: colocar nodos
    float y_offset = gap;
    for (int r = 0; r < rows; r++) {
        float x_offset = gap;
        for (int c = 0; c < columns; c++) {
            int i = r * columns + c;
            if (i >= n) break;
	    printf("WTF: %d\n",i);
            nodes[i].x = x_offset + max_width_col[c]/2;
            nodes[i].y = y_offset + max_height_row[r]/2;

            x_offset += max_width_col[c] + gap;
        }
        y_offset += max_height_row[r] + gap;
    }
}

// Generar SVG desde nodos distribuidos
void generate_svg(Node* nodes, int n, const char* filename, float svg_width, float svg_height) {
    FILE* f = fopen(filename, "w");
    if (!f) return;

    fprintf(f, "<svg xmlns='http://www.w3.org/2000/svg' width='%.0f' height='%.0f' style='border:1px solid black'>\n", svg_width, svg_height);

    const char* colors[] = {"lightblue","lightgreen","lightpink","lightyellow","orange",
                            "violet","cyan","magenta","lightgray","lightcoral"};

    for (int i = 0; i < n; i++) {
        float svg_x = nodes[i].x - nodes[i].width / 2;
        float svg_y = nodes[i].y - nodes[i].height / 2;
        fprintf(f, "  <rect x='%.2f' y='%.2f' width='%.2f' height='%.2f' fill='%s' stroke='black'/>\n",
                svg_x, svg_y, nodes[i].width, nodes[i].height, colors[i % 10]);
    }

    fprintf(f, "</svg>\n");
    fclose(f);
    printf("SVG generado: %s con tamaño %.0fx%.0f\n", filename, svg_width, svg_height);
}

int main() {
    int n = 20;
    Node nodes[20];


    float widths[]  = {50,60,70,5,6,7,5,6,7,5,5,6,7,5,6,7,5,6,7,200};
    float heights[] = {30,40,30,4,3,4,3,4,3,4,3,4,3,4,3,4,3,4,3,100};

    for (int i = 0; i < n; i++) {
        nodes[i].width = widths[i];
        nodes[i].height = heights[i];
    }

    float svg_width = 1024;
    float svg_height = 1024;
    float gap = 50;

    distribute_and_fit(nodes, n, &svg_width, &svg_height, gap);

    generate_svg(nodes, n, "adaptive_grid.svg", svg_width, svg_height);

    return 0;
}
