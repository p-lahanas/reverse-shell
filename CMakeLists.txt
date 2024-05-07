cmake_minimum_required(VERSION 3.11)
project(asd VERSION 0.1.0 LANGUAGES C)

add_library(asdpack asd.c)
add_executable(client client.c)
add_executable(server server.c)

target_link_libraries(client asdpack)
target_link_libraries(server asdpack)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
