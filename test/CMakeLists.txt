cmake_minimum_required(VERSION 2.8)

project(xporttest)

set(SRC_LIST )
aux_source_directory(. SRC_LIST)

include_directories("/opt/evpp/platform/arm_openwrt/candidates/include/")
link_directories("/opt/evpp/platform/arm_openwrt/candidates/lib/")

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} gtest gtest_main xport dl)