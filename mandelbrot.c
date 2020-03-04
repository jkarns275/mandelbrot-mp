#include "bitmap.c"

bitmap_t mandelbrot(    double x0, double y0, double width, double height,
                        int imwidth, int imheight, 
                        int max_iterations,
                        pixel_t (*get_pixel_value)(int, int, double, double)) {
    
    bitmap_t bitmap = mkbitmap(imwidth, imheight);

    double pixel_width = width / (double) imwidth;
    double pixel_height = height / (double) imheight;
    
#ifdef VOMP
#pragma omp parallel
#pragma omp for
#endif
    for (int ix = 0; ix < imwidth; ix++) {
        const double x = ix * pixel_width + x0;
        for (int iy = 0; iy < imheight; iy++) {
            const double y = iy * pixel_height + y0;
            double a = 0.0, b = 0.0, t;
            
            int i = 0;
            for (; i < max_iterations; i++) {
                t = a * a - b * b + x; b = 2.0 * a * b + y; a = t;
                if (a * a + b * b > 4.0) break;
            }

            set_pixel_at(&bitmap, ix, iy, get_pixel_value(i, max_iterations, a, b));
        }
    }

    return bitmap;
}
