add_executable(GlobalRegistration GlobalRegistration.cpp RansacCurvature.cpp )
target_link_libraries(GlobalRegistration  ${Boost_LIBRARIES} ${PCL_LIBRARIES} ${PCL_SURFACE_LIBRARIES} ${PCL_SEGMENTATION_LIBRARIES} ${PCL_FEATURES_LIBRARIES}  ${PCL_SEARCH_LIBRARIES}  ${PCL_IO_LIBRARIES} ${PCL_KDTREE_LIBRARIES} ${PCL_GPU_FEATURES_LIBRARIES} ${PCL_VISUALIZATION_LIBRARIES})

#add_executable(registrate registrate.cpp RansacCurvature.cpp )
#target_link_libraries(registrate   ${Boost_LIBRARIES} ${PCL_LIBRARIES} ${PCL_SURFACE_LIBRARIES} ${PCL_SEGMENTATION_LIBRARIES} ${PCL_FEATURES_LIBRARIES}  ${PCL_SEARCH_LIBRARIES}  ${PCL_IO_LIBRARIES} ${PCL_KDTREE_LIBRARIES} ${PCL_GPU_FEATURES_LIBRARIES} ${PCL_VISUALIZATION_LIBRARIES})
#
#add_executable(extract_fea extract_fea.cpp RansacCurvature.cpp )
#target_link_libraries(extract_fea   ${Boost_LIBRARIES} ${PCL_LIBRARIES} ${PCL_SURFACE_LIBRARIES} ${PCL_SEGMENTATION_LIBRARIES} ${PCL_FEATURES_LIBRARIES}  ${PCL_SEARCH_LIBRARIES}  ${PCL_IO_LIBRARIES} ${PCL_KDTREE_LIBRARIES} ${PCL_GPU_FEATURES_LIBRARIES} ${PCL_VISUALIZATION_LIBRARIES})
