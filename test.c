#include <stdio.h>

void draw_block(FILE *f, int x, int y, const char *label) {
    fprintf(f, "<rect x='%d' y='%d' width='140' height='60' "
               "style='fill:lime;stroke:black;stroke-width:2' />\n", x, y);
    fprintf(f, "<text x='%d' y='%d' font-family='monospace' font-size='12'>%s</text>\n",
            x + 10, y + 30, label);
}

void draw_connection(FILE *f, int x1, int y1, int x2, int y2, const char *label) {
    fprintf(f, "<line x1='%d' y1='%d' x2='%d' y2='%d' "
               "style='stroke:black;stroke-width:2;marker-end:url(#arrow)' />\n",
            x1, y1, x2, y2);
    if (label)
        fprintf(f, "<text x='%d' y='%d' font-size='12'>%s</text>\n",
                (x1 + x2) / 2, (y1 + y2) / 2 - 5, label);
}

int main() {
    FILE *f = fopen("diagram.svg", "w");
    if (!f) { perror("fopen"); return 1; }

    fprintf(f, "<svg xmlns='http://www.w3.org/2000/svg' width='1000' height='600'>\n");

    // Definir flecha
    fprintf(f, "<defs><marker id='arrow' markerWidth='10' markerHeight='10' "
               "refX='10' refY='3' orient='auto' markerUnits='strokeWidth'>"
               "<path d='M0,0 L0,6 L9,3 z' fill='black' /></marker></defs>\n");

    // Bloques en cuadrícula
    draw_block(f, 50, 100, "inverter\\nSwitch");
    draw_block(f, 250, 100, "d_flipflop\\nSwitch");
    draw_block(f, 450, 100, "and_gate");
    draw_block(f, 650, 100, "multiplexer");
    draw_block(f, 850, 100, "d_flipflop\\nLED");
    draw_block(f, 450, 300, "inverter\\nLED");

    // Conexiones
    draw_connection(f, 190, 130, 250, 130, "i_Switch_1");
    draw_connection(f, 390, 130, 450, 130, "q_Switch_1");
    draw_connection(f, 590, 130, 650, 130, "falling_edge_switch");
    draw_connection(f, 590, 330, 650, 130, "r_LED_1_not");
    draw_connection(f, 790, 130, 850, 130, "r_LED_d");
    draw_connection(f, 920, 160, 520, 330, "r_LED_q");

    fprintf(f, "</svg>\n");
    fclose(f);

    printf("Generado diagram.svg con layout manual (más complejo)\n");
    return 0;
}
