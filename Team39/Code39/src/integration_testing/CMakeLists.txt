file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/query_evaluator/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/query_evaluator/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/query_evaluator/*.hpp")
add_executable(integration_testing ${srcs})


target_link_libraries(integration_testing spa)
