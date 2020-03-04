# Arbitrary
cmake_minimum_required(VERSION 3.0)
project (mandelbrotpar LANGUAGES C CUDA)

find_package(OpenMP REQUIRED)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

add_executable(mb_par main.c)
target_compile_definitions(mb_par PUBLIC VOMP)
target_link_libraries(mb_par m)
target_link_libraries(mb_par png)


# add_executable(aescrack_cuda main.cu)
# target_compile_definitions(aescrack_cuda PUBLIC VCUDA)

add_executable(mb main.c)
target_link_libraries(mb m)
target_link_libraries(mb png)
