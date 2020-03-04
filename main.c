#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>

#include "bitmap.c"
#include "mandelbrot.c"

/* Given "value" and "max", the maximum value which we expect "value"
   to take, this returns an integer between 0 and 255 proportional to
   "value" divided by "max". */

static int pix (int value, int max) {
    if (value < 0) {
        return 0;
    }
    return (int) (255.0 *((double) (value)/(double) max));
}

pixel_t gray_coloring(int i, int maxi, double a, double b) {
    double pixel_value = ((double) (maxi - i)) / (double) maxi;
    uint8_t as_byte = (uint8_t) (pixel_value * 255.0);
    pixel_t r = { .red = as_byte, .green = as_byte, .blue = as_byte };
    return r;
}

pixel_t interp_pixels(pixel_t p0, double w0, pixel_t p1, double w1) {
    double r, g, b;
    
    r = w0 * (double) p0.red + w1 * (double) p1.red;
    g = w0 * (double) p0.green + w1 * (double) p1.green;
    b = w0 * (double) p0.blue + w1 * (double) p1.blue;
    
    pixel_t p = { .red = (uint8_t) r, .green = (uint8_t) g, .blue = (uint8_t) b };
    return p;
}

pixel_t interp_palette(const pixel_t palette[], int palette_size, int i, int maxi) {
    double indexf = ((double) i) / ((double) maxi) * (palette_size - 1);
    double w1 = indexf - floor(indexf);
    double w0 = 1.0 - w1;
    int index = floor(indexf);
    pixel_t p0 = palette[index];
    pixel_t p1 = palette[1 + index];
    return interp_pixels(p0, w0, p1, w1);
}

pixel_t rgb_coloring_1(int i, int maxi, double a, double b) {
    static const pixel_t palette[] = {
        { .red = 0, .green = 7, .blue = 100 },
        { .red = 32, .green = 107, .blue = 203 },
        { .red = 237, .green = 255, .blue = 255 },
        { .red = 255, .green = 170, .blue = 0 },
        { .red = 0, .green = 2, .blue = 0 }
    };
    return interp_palette(palette, 5, i, maxi);
}

int main (int argn, char **argv) {
    bitmap_t fruit;
    int x;
    int y;
    int status;

    status = 0;

    double x0, y0, height, width;
    int imwidth, imheight;
    int maxiter;

    assert(argn == 8);

    sscanf("");

    bitmap_t bitmap = mandelbrot(x0, y0, width, height, imwidth, imheight, maxiter, rgb_coloring_1);

    if (save_bitmap_to_png(&bitmap, "out.png")) {
	    fprintf (stderr, "Error writing file.\n");
	    status = -1;
    }

    free(bitmap.pixels);

    return status;
}

