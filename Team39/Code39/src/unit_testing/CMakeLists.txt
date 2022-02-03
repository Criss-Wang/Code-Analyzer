file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/Utility/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/Utility/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/Utility/*.hpp")
add_executable(unit_testing ${srcs} )

target_link_libraries(unit_testing spa)

