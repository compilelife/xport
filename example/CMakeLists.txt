cmake_minimum_required(VERSION 2.8)

project(xportexample)

set(SRC_LIST )
aux_source_directory(. SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} xport)