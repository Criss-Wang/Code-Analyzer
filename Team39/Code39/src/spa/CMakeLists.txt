file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/tables/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/Utility/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/SP/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/SP/tokens/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/query_evaluator/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/query_evaluator/clause/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/query_evaluator/formatter/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/query_evaluator/solver/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/query_parser/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/Utility/CFG/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/tables/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/PKB/tables/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/Utility/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/Utility/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/SP/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/SP/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/SP/tokens/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/SP/tokens/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/query_evaluator/clause/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/query_evaluator/clause/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/query_evaluator/solver/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/query_evaluator/solver/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/query_evaluator/formatter/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/query_evaluator/formatter/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/query_parser/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/query_parser/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/query_evaluator/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/PQL/query_evaluator/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/Utility/CFG/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/Utility/CFG/*.hpp")

add_library(spa ${srcs} ${headers})

# this makes the headers accessible for other projects which uses spa lib
target_include_directories(spa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)