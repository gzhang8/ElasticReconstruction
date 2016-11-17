cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(ElasticReconstruction)
find_package(PCL 1.7 REQUIRED COMPONENTS common io kdtree visualization registration search features gpu_features segmentation surface)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(
    -std=c++11
    -fpermissive
    ${PCL_DEFINITIONS})
#
#file( GLOB src
#    "src/*.h"
#    "src/*.cpp"
#)

#set(boost_lib /usr/lib/x86_64-linux-gnu/libboost_thread.so.1.54.0 
#            /usr/lib/x86_64-linux-gnu/libboost_system.so.1.54.0 
#            /lib/x86_64-linux-gnu/libpthread.so.0
#            /usr/lib/libOpenNI.so)


FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
message("OPENMP FOUND")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -O3")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
add_subdirectory(GlobalRegistration)
#include("GlobalRegistration/")

#add_executable(Intergrate ${src})
#
#target_link_libraries(Intergrate pcl_common pcl_io pcl_kdtree pcl_visualization ${boost_lib} ${Boost_LIBRARIES} )
