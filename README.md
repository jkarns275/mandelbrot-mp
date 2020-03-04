# Compiling
```
mkdir build
cd build/
cmake ..
make
```

# Running
```
build/mb_par <x0> <y0> <viewport_width> <viewport_height> <image_width> <image_height> <output>.png <max_iters>
```

Running this command will create a mandelbrot set up to max\_iters in a rectangular region with it's bottom left corner at the point specified by (x0, y0) and the top right corner at (x0 + width, y0 + height).
It will save the resulting image as a PNG in the specified location, with the specified dimensions.
