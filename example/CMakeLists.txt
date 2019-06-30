cmake_minimum_required(VERSION 2.8)

project(xportexample)

add_executable(simple_demo simple.cpp)
target_link_libraries(simple_demo xport)

add_executable(decrypt_demo decrypt.cpp)
target_link_libraries(decrypt_demo xport)

add_executable(m3u8_demo m3u8.cpp)
target_link_libraries(m3u8_demo xport)

add_executable(mix_demo mix.cpp)
target_link_libraries(mix_demo xport)