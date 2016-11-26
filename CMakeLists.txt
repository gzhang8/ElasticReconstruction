cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(ElasticReconstruction)

find_package(PCL 1.7 REQUIRED COMPONENTS common io kdtree visualization registration search features gpu_features segmentation surface)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(
    -std=c++11
    -fpermissive
    #-mavx2
    ${PCL_DEFINITIONS})


FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
message("OPENMP FOUND")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -O0")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
add_subdirectory(GlobalRegistration)
add_subdirectory(GraphOptimizer)

