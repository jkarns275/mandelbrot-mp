#include "bitmap.c"

#ifndef VCUDA

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

#else

__device__ __host__ pixel_t interp_pixels(pixel_t p0, double w0, pixel_t p1, double w1) {
    double r, g, b;
    
    r = w0 * (double) p0.red + w1 * (double) p1.red;
    g = w0 * (double) p0.green + w1 * (double) p1.green;
    b = w0 * (double) p0.blue + w1 * (double) p1.blue;
    
    pixel_t p = { .red = (uint8_t) r, .green = (uint8_t) g, .blue = (uint8_t) b };
    return p;
}

__device__ __host__ pixel_t interp_palette(const pixel_t palette[], int palette_size, int i, int maxi) {
    double indexf = ((double) i) / ((double) maxi) * (palette_size - 1);
    double w1 = indexf - floor(indexf);
    double w0 = 1.0 - w1;
    int index = floor(indexf);
    pixel_t p0 = palette[index];
    pixel_t p1 = palette[1 + index];
    return interp_pixels(p0, w0, p1, w1);
}

__device__ __host__ pixel_t cuda_coloring(int i, int maxi, double a, double b) {
    static const pixel_t palette[] = {
        { .red = 0, .green = 7, .blue = 100 },
        { .red = 32, .green = 107, .blue = 203 },
        { .red = 237, .green = 255, .blue = 255 },
        { .red = 255, .green = 170, .blue = 0 },
        { .red = 0, .green = 2, .blue = 0 }
    };
    
    return interp_palette(palette, 5, i, maxi);
}

#define CHUNK_SIZE 1024
#define nobranch_min(x, y) y ^ ((x ^ y) & -(x < y))
#define nobranch_max(x, y) x ^ ((x ^ y) & -(x < y))

__global__ void mandelbrot( double x0, double y0, double pixwidth, double pixheight,
                            int imwidth, int imheight,
                            int max_iterations, pixel_t *output) {

    int64_t ix = ((int64_t) blockIdx.x) * ((int64_t) blockDim.x) + ((int64_t) threadIdx.x);
    int64_t size = (int64_t) imwidth * (int64_t) imheight;
    int64_t imax = ix * CHUNK_SIZE + CHUNK_SIZE;
    imax = nobranch_min(size, imax);

    for (int64_t i = ix * CHUNK_SIZE; i < imax; i++) {
        double a = 0.0, b = 0.0, t;
        
        double x = x0 + pixwidth * (i % imwidth);
        double y = y0 + pixheight * (i / imwidth);       
        
        int iter = 0;
        for (; iter < max_iterations; iter++) {
            t = a * a - b * b + x; b = 2.0 * a * b + y; a = t;
            if (a * a + b * b > 4.0) break;
        }

        pixel_t cuda_color = cuda_coloring(i, max_iterations, a, b);
        output[i] = cuda_color;
    }

}
#endif
