#--------------------------- Out of source builds ---------------------------
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

add_executable(client client.c)
add_executable(server server.c)

target_compile_features(client PRIVATE c_std_23)
target_compile_features(server PRIVATE c_std_23)

target_link_libraries(client PRIVATE rtp)
target_link_libraries(server PRIVATE rtp)