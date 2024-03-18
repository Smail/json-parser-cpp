cmake_minimum_required(VERSION 3.27)

project(json-parser)

include(CTest)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options("-Werror=return-type")

add_library(json-parser)
target_sources(json-parser
        PUBLIC FILE_SET HEADERS BASE_DIRS src FILES
        src/JSONParser.h
        PRIVATE
        src/JSONParser.cpp
)
add_library(JSONParser::JSONParser ALIAS json-parser)

if (BUILD_TESTING)
    add_executable(test-json-parser test/JSONParserTest.cpp)
    target_link_libraries(test-json-parser PUBLIC JSONParser::JSONParser)

    add_test(NAME test-json-parser COMMAND test-json-parser)
endif ()
