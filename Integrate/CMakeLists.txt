#
#find_package(PCL 1.7 REQUIRED COMPONENTS common io kdtree visualization)
#find_package(Boost 1.54.0 COMPONENTS filesystem system thread REQUIRED)
#include_directories(${PCL_INCLUDE_DIRS})
#link_directories(${PCL_LIBRARY_DIRS})
#add_definitions(
    #-std=c++11
    #${PCL_DEFINITIONS})
#
file( GLOB src
    "src/*.h"
    "src/*.cpp"
)

#FIND_PACKAGE( OpenMP REQUIRED)
#if(OPENMP_FOUND)
#message("OPENMP FOUND")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -O3")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -O3")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    
#endif()

add_executable(Integrate ${src})

target_link_libraries(Integrate ${Boost_LIBRARIES} ${PCL_LIBRARIES} ${PCL_SURFACE_LIBRARIES} ${PCL_SEGMENTATION_LIBRARIES} ${PCL_FEATURES_LIBRARIES}  ${PCL_SEARCH_LIBRARIES}  ${PCL_IO_LIBRARIES} ${PCL_KDTREE_LIBRARIES} ${PCL_GPU_FEATURES_LIBRARIES} ${PCL_VISUALIZATION_LIBRARIES} )
