#include "main.h"
#include "drawer.h"
#include "auto_placer.h"
#include "auto_router.h"

static int height_val = WINDOWSIZE;
static int width_val  = WINDOWSIZE;

int *svg_height = &height_val;
int *svg_width  = &width_val;

Point **paths;
int *paths_lengths;
int num_paths;

#define ELEMENTOS 8

int main() {

    // Instancia: clk25gen
    StructPort clk25gen_ports[] = {
        {"clk50", "in",  "clk50", 0,0},
        {"clk25", "out", "clk25",  0,0}
    };
    StructComponent clk25gen = {"clk25gen", clk25gen_ports, 2, 0,0, 0,0};

    // Instancia: debounce_circuit
    StructPort debounce_ports[] = {
        {"clk",   "in",  "clk50",  0,0},
        {"input", "in",  "pb",     0,0},
        {"output","out", "resend", 0,0}
    };
    StructComponent debounce_circuit = {"debounce_circuit", debounce_ports, 3, 0,0, 0,0};

    // Instancia: ov7670_controller
    StructPort ov7670_ctrl_ports[] = {
        {"clk",       "in",  "clk50",         0,0},
        {"resend",    "in",  "resend",        0,0},
        {"sioc",      "out", "ov7670_sioc",   0,0},
        {"siod",      "out", "ov7670_siod",   0,0},
        {"conf_done", "out", "led",           0,0},
        {"pwdn",      "out", "ov7670_pwdn",   0,0},
        {"reset",     "out", "ov7670_reset",  0,0},
        {"xclk_in",   "in",  "clk25",         0,0},
        {"xclk_out",  "out", "ov7670_xclk",   0,0}
    };
    StructComponent ov7670_controller = {"ov7670_controller", ov7670_ctrl_ports, 9, 0,0, 0,0};

    // Instancia: ov7670_capture
    StructPort ov7670_cap_ports[] = {
        {"pclk", "in",  "ov7670_pclk", 0,0},
        {"vsync","in",  "ov7670_vsync",0,0},
        {"href", "in",  "ov7670_href", 0,0},
        {"d",    "in",  "ov7670_data", 0,0},
        {"addr", "out", "wr_a",        0,0},
        {"dout", "out", "wr_d",        0,0},
        {"we",   "out", "wren",        0,0}
    };
    StructComponent ov7670_capture = {"ov7670_capture", ov7670_cap_ports, 7, 0,0, 0,0};

    // Instancia: frame_buffer
    StructPort frame_buf_ports[] = {
        {"weA",  "in",  "wren",        0,0},
        {"clkA", "in",  "ov7670_pclk", 0,0},
        {"addrA","in",  "wr_a",        0,0},
        {"dinA", "in",  "wr_d",        0,0},
        {"clkB", "in",  "clk25",       0,0},
        {"addrB","in",  "rd_a",        0,0},
        {"doutB","out", "rd_d",        0,0}
    };
    StructComponent frame_buffer = {"frame_buffer", frame_buf_ports, 7, 0,0, 0,0};

    // Instancia: address_generator
    StructPort addr_gen_ports[] = {
        {"clk25",  "in",  "clk25",        0,0},
        {"enable", "in",  "active",       0,0},
        {"vsync",  "in",  "vga_vsync_sig",0,0},
        {"address","out", "rd_a",         0,0}
    };
    StructComponent address_generator = {"address_generator", addr_gen_ports, 4, 0,0, 0,0};

    // Instancia: vga_imagegenerator
    StructPort vga_img_ports[] = {
        {"Data_in",     "in",  "rd_d",    0,0},
        {"active_area", "in",  "active",  0,0},
        {"RGB_out",     "out", "vga_rgb", 0,0}
    };
    StructComponent vga_imagegenerator = {"vga_image_generator", vga_img_ports, 3, 0,0, 0,0};

    // Instancia: VGA_timing_synch
    StructPort vga_timing_ports[] = {
        {"clk25",      "in",  "clk25",         0,0},
        {"Hsync",      "out", "vga_hsync",     0,0},
        {"Vsync",      "out", "vga_vsync_sig", 0,0},
        {"activeArea", "out", "active",        0,0}
    };
    StructComponent VGA_timing_synch = {"VGA_timing_synch", vga_timing_ports, 4, 0,0, 0,0};
  /* ----- Pack in array ----- */
  StructComponent comps[] = {
    VGA_timing_synch,
    clk25gen,
    frame_buffer,
    address_generator,
    debounce_circuit,
    vga_imagegenerator,
    ov7670_controller,
    ov7670_capture
  };

  (void) AutoLayout(comps, ELEMENTOS);
  StartSVG();
  DrawComponents(comps, ELEMENTOS);
  (void) AutoRouter(comps, ELEMENTOS);
  DrawPaths(); 	/* Change by DrawSVGSignals */
  FinishSVG();  
  return 0;
}

