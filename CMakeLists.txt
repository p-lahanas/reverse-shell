cmake_minimum_required(VERSION 3.11)
project(asd VERSION 0.1 LANGUAGES C)

#---------------------------- Additional Modules ----------------------------
set(CMAKE_MODULE_PATH "${${PROJECT_NAME}_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include(CTest)

add_subdirectory(src) # Library Code
add_subdirectory(apps) # Executable Code