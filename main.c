#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "tools.c"


bool write_entity_svg(const StructEntity *entity, const char *path);

int main(int argc, char *argv[]){

  if (argc != 2) {
    (void) fputs("usage: pl0c file.pl0\n", stderr);
    exit(1);
  }

  FileReader(argv[1]);
  FileTokenizer();
  FileAnalyzer();
  
  write_entity_svg(&entity, "entity.svg");

  return 0;
}

/* Utilidad para escapar XML básico */
static void xml_escape(const char *s, FILE *f) {
    for (; *s; ++s) {
        if (*s == '&')      fputs("&amp;", f);
        else if (*s == '<') fputs("&lt;", f);
        else if (*s == '>') fputs("&gt;", f);
        else                fputc(*s, f);
    }
}

/* Cuenta cuántos puertos hay de cada lado */
static void count_ports(const StructEntity *e, int *nin, int *nout, int *ninout){
    *nin = *nout = *ninout = 0;
    for (size_t i=0; i<e->portsCount; ++i){
        if (e->ports[i].mode == PM_IN) (*nin)++;
        else if (e->ports[i].mode == PM_OUT) (*nout)++;
        else (*ninout)++;
    }
}

/* Genera el SVG a fichero */
bool write_entity_svg(const StructEntity *entity, const char *path){
    /* Layout básico (ajústalo a gusto) */
    const int margin = 150;
    const int pin_len = 18;
    const int row_h = 18;
    const int header_h = 36;      // título
    const int generics_h = (int)(entity->genericsCount ? (entity->genericsCount * row_h + 12) : 0);
    const int section_gap = 10;

    int nin, nout, ninout;
    count_ports(entity, &nin, &nout, &ninout);

    int left_height  = nin * row_h;
    int right_height = nout * row_h;
    int bottom_height = (ninout ? (row_h + 8) : 0);

    /* Altura del bloque central (zona de caja) */
    int body_h = header_h + section_gap + generics_h + section_gap
               + (nin > nout ? left_height : right_height)
               + 2*section_gap;

    /* Anchos */
    int box_w = 480;              // ancho del rectángulo de la entidad
    int text_pad = 8;
    int svg_w = margin + pin_len + box_w + pin_len + margin;
    int svg_h = margin + body_h + bottom_height + pin_len + margin;

    FILE *f = fopen(path, "w");
    if (!f) return false;

    fprintf(f,
      "<svg xmlns='http://www.w3.org/2000/svg' width='%d' height='%d' viewBox='0 0 %d %d'>\n",
      svg_w, svg_h, svg_w, svg_h);
    /* Fondo */
    fprintf(f, "<rect x='0' y='0' width='%d' height='%d' fill='white-smoke' stroke='none'/>\n", svg_w, svg_h);

    /* Caja principal */
    int box_x = margin + pin_len;
    int box_y = margin;
    fprintf(f, "<rect x='%d' y='%d' width='%d' height='%d' rx='10' ry='10' fill='#f6f8ff' stroke='#334' stroke-width='1.5'/>\n",
            box_x, box_y, box_w, body_h);

    /* Título (nombre entidad) */
    fprintf(f, "  <text x='%d' y='%d' font-family='monospace' font-size='16' font-weight='bold' fill='#223'>",
            box_x + box_w/2, box_y + 22);
    
    fprintf(f, "<tspan text-anchor='left'>");
    xml_escape(entity->name, f);
    fprintf(f, "</tspan></text>\n");

    /* Separador bajo el título */
    int y = box_y + header_h;
    fprintf(f, "  <line x1='%d' y1='%d' x2='%d' y2='%d' stroke='#99a' stroke-width='1'/>\n",
            box_x+6, y, box_x+box_w-6, y);

    /* Generics (si hay) */
    if (entity->genericsCount){
        int gy = y + section_gap;
        fprintf(f, "  <text x='%d' y='%d' font-family='monospace' font-size='12' fill='#445'>Generics:</text>\n",
                box_x + text_pad, gy);
        gy += row_h;

        for (size_t i=0; i<entity->genericsCount; ++i, gy += row_h){
            fprintf(f, "  <text x='%d' y='%d' font-family='monospace' font-size='12' fill='#223'>",
                    box_x + text_pad + 10, gy);
            xml_escape(entity->generics[i].name, f);
            fprintf(f, "</text>\n");
        }
        /* Separador */
        y = box_y + header_h + section_gap + generics_h;
        fprintf(f, "  <line x1='%d' y1='%d' x2='%d' y2='%d' stroke='#99a' stroke-width='1'/>\n",
                box_x+6, y, box_x+box_w-6, y);
    }

    /* Puertos: calcula offsets verticales para alinear filas izquierda/derecha */
    int pins_top = y + section_gap;

    /* Izquierda (IN) */
    int ly = pins_top;
    for (size_t i=0; i<entity->portsCount; ++i){
        if (entity->ports[i].mode != PM_IN) continue;
        int pin_x1 = box_x - pin_len;
        int pin_x2 = box_x;
        int pin_y  = ly + row_h/2;
        /* pin */
        fprintf(f, "  <line x1='%d' y1='%d' x2='%d' y2='%d' stroke='#2a5' stroke-width='2'/>\n",
                pin_x1, pin_y, pin_x2, pin_y);
        /* etiqueta nombre */
        fprintf(f, "  <text x='%d' y='%d' font-family='monospace' font-size='12' text-anchor='end' fill='#111'>",
                pin_x1-6, pin_y+4);
        xml_escape(entity->ports[i].name, f);
        fprintf(f, "</text>\n");
        /* tipo */
        fprintf(f, "  <text x='%d' y='%d' font-family='monospace' font-size='11' text-anchor='start' fill='#556'>: ");
        fprintf(f, "</text>\n"); /* placeholder */
        fprintf(f, "  <text x='%d' y='%d' font-family='monospace' font-size='11' text-anchor='start' fill='#556'>",
                box_x + text_pad, pin_y+4);
        xml_escape(KWS(entity->ports[i].type), f);
        fprintf(f, "</text>\n");
        ly += row_h;
    }

    /* Derecha (OUT) */
    int ry = pins_top;
    for (size_t i=0; i<entity->portsCount; ++i){
        if (entity->ports[i].mode != PM_OUT) continue;
        int pin_x1 = box_x + box_w;
        int pin_x2 = box_x + box_w + pin_len;
        int pin_y  = ry + row_h/2;
        /* pin */
        fprintf(f, "  <line x1='%d' y1='%d' x2='%d' y2='%d' stroke='#a52' stroke-width='2'/>\n",
                pin_x1, pin_y, pin_x2, pin_y);
        /* etiqueta nombre */
        fprintf(f, "  <text x='%d' y='%d' font-family='monospace' font-size='12' text-anchor='start' fill='#111'>",
                pin_x2+6, pin_y+4);
        xml_escape(entity->ports[i].name, f);
        fprintf(f, "</text>\n");
        /* tipo (alineado dentro de la caja, lado derecho) */
        fprintf(f, "  <text x='%d' y='%d' font-family='monospace' font-size='11' text-anchor='end' fill='#556'>",
                box_x + box_w - text_pad, pin_y+4);
        xml_escape(KWS(entity->ports[i].type), f);
        fprintf(f, "</text>\n");
        ry += row_h;
    }

    /* Abajo (INOUT) */
    int by = box_y + body_h;  // borde inferior de la caja
    if (ninout){
        /* separador abajo */
        fprintf(f, "  <line x1='%d' y1='%d' x2='%d' y2='%d' stroke='#99a' stroke-width='1'/>\n",
                box_x+6, by-6, box_x+box_w-6, by-6);

        int col = 0;
        int col_w = 160; // ancho por pin inout
        int start_x = box_x + (box_w - (col_w * ninout)) / 2;

        for (size_t i=0; i<entity->portsCount; ++i){
            if (entity->ports[i].mode != PM_INOUT) continue;
            int px = start_x + col*col_w + col_w/2;
            /* pin vertical */
            fprintf(f, "  <line x1='%d' y1='%d' x2='%d' y2='%d' stroke='#25a' stroke-width='2'/>\n",
                    px, by, px, by + pin_len);
            /* etiqueta */
            fprintf(f, "  <text x='%d' y='%d' font-family='monospace' font-size='12' text-anchor='middle' fill='#111'>",
                    px, by + pin_len + 14);
            xml_escape(entity->ports[i].name, f);
            fprintf(f, "</text>\n");
            /* tipo */
            fprintf(f, "  <text x='%d' y='%d' font-family='monospace' font-size='11' text-anchor='middle' fill='#556'>",
                    px, by + pin_len + 28);
            xml_escape(KWS(entity->ports[i].type), f);
            fprintf(f, "</text>\n");
            col++;
        }
    }

    fprintf(f, "</svg>\n");
    fclose(f);
    return true;
}
