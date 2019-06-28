cmake_minimum_required(VERSION 2.8)

project(xport)

aux_source_directory(. SRC_LIST)

add_library(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} pthread)

set(HEADERS 
    xport.h 
    imedia.h 
    imedia_creator.h 
    media_request.h
    media_factory.h)
file(COPY ${HEADERS} DESTINATION ${CMAKE_BINARY_DIR}/include)