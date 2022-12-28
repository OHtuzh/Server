cmake_minimum_required(VERSION 3.22.1)
project(Task)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(Server)
add_subdirectory(OMFL-parser/lib)
add_subdirectory(ProgramArgumentParser/lib)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} server ITMLparse argparser)
