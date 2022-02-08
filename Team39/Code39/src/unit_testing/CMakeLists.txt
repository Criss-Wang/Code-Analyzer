file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/Utility/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/SP/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/tables/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/tables/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/Utility/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/Utility/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/SP/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/SP/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/*.hpp")
add_executable(unit_testing ${srcs} )

target_link_libraries(unit_testing spa)