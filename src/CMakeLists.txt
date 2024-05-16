file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)

# Module's public include headers
set(PUBLIC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

# Optionally glob, but only for CMake 3.12 or later:
# file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${ModernCMakeExample_SOURCE_DIR}/include/modern/*.hpp")
set(HEADER_LIST "${${PROJECT_NAME}_SOURCE_DIR}/include/asd/asd.h")

# Make an automatic library - will be static or dynamic based on user setting
add_library(asd asd.c ${HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(asd
    PUBLIC
    "$<BUILD_INTERFACE:${PUBLIC_INCLUDE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# All users of this library will need at least C23
target_compile_features(asd PUBLIC c_std_23)

install(TARGETS asd
        EXPORT ${PROJECT_NAME}_Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

# Install module headers
install(DIRECTORY ${PUBLIC_INCLUDE_DIR}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
