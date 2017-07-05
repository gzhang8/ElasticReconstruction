cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(ElasticReconstruction)

add_definitions(
    -std=c++11)
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(/include/usr/local/include/eigen3/unsupported/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_subdirectory(GraphOptimizer)

find_package(PCL 1.7 REQUIRED COMPONENTS common io kdtree registration search features segmentation)
include_directories(${PCL_INCLUDE_DIRS} /include/usr/local/include/eigen3/unsupported/)

link_directories(${PCL_LIBRARY_DIRS})
add_definitions(
    -std=c++11
    -fpermissive
    ${PCL_DEFINITIONS})

FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
message("OPENMP FOUND")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


#add_subdirectory(FragmentOptimizer)
#add_subdirectory(Integrate)
#add_subdirectory(GlobalRegistration)
#add_subdirectory(BuildCorrespondence)
