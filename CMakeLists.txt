cmake_minimum_required(VERSION 3.10)
project(asd VERSION 0.1.0 LANGUAGES C)

add_executable(client client.c)
add_executable(server server.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
