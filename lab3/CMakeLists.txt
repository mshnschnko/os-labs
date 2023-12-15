cmake_minimum_required (VERSION 3.22)

project ("lab3")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Werror)

find_library(LIBRT rt)
find_package(Threads)

add_library(fine_set src/fine_set.h)
set_target_properties(fine_set PROPERTIES LINKER_LANGUAGE CXX)
add_library(tester src/tester.h)
set_target_properties(tester PROPERTIES LINKER_LANGUAGE CXX)

add_executable (lab3 ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_link_libraries(lab3 PUBLIC pthread rt fine_set tester)