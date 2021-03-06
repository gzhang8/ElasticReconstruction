#set(SCRIPT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")
LIST( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules )
SET( G2O_ROOT /usr/local/include/g2o )
FIND_PACKAGE( G2O )


if(G2O_SOLVERS_FOUND)
message("G2O_SOLVERS_FOUND")
endif()
# CSparse
FIND_PACKAGE( CSparse )
INCLUDE_DIRECTORIES( ${G2O_INCLUDE_DIR} ${CSPARSE_INCLUDE_DIR} )

FIND_PACKAGE( Boost 1.40 COMPONENTS system filesystem program_options REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_executable(GraphOptimizer
  GraphOptimizer.cpp
  OptApp.cpp
  vertigo/edge_se3Switchable.cpp
  vertigo/edge_switchPrior.cpp
  vertigo/types_g2o_robust.cpp
  vertigo/vertex_switchLinear.cpp
  )
set( G2O_LIBRARIES ${G2O_STUFF_LIBRARY}  ${G2O_CORE_LIBRARY}
  ${G2O_CLI_LIBRARY} ${G2O_SOLVER_CHOLMOD} ${G2O_SOLVER_CSPARSE}
  ${G2O_SOLVER_CSPARSE_EXTENSION} ${G2O_SOLVER_DENSE} ${G2O_SOLVER_PCG}
  ${G2O_SOLVER_SLAM2D_LINEAR} ${G2O_SOLVER_STRUCTURE_ONLY}
  ${G2O_SOLVER_EIGEN} ${G2O_TYPES_DATA} ${G2O_TYPES_ICP}
  ${G2O_TYPES_SBA} ${G2O_TYPES_SCLAM2D} ${G2O_TYPES_SIM3}
  ${G2O_TYPES_SLAM2D} ${G2O_TYPES_SLAM3D} ${CSPARSE_LIBRARY} )
message( ${G2O_LIBRARIES})
target_link_libraries(GraphOptimizer   ${Boost_LIBRARIES} ${G2O_LIBRARIES})# ${G2O_CORE_LIBRARY} ${G2O_STUFF_LIBRARY} ${Boost_LIBRARIES} ${G2O_SOLVER_CSPARSE} ${G2O_CLI_LIBRARY})
