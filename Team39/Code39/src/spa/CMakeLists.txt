file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/tables/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Utility/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/SP/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/tables/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/tables/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Utility/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/Utility/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/SP/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/SP/*.hpp")
add_library(spa ${srcs} ${headers} src/PQL/Parser.cpp src/PQL/Parser.h src/PQL/lexer.cpp src/PQL/Token.cpp src/PQL/Token.h)

# this makes the headers accessible for other projects which uses spa lib
target_include_directories(spa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)