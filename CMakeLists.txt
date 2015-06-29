cmake_minimum_required(VERSION 2.8)

project(nsengine)

# Set the source directory
set(PROJECT_SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# Set the include directory
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

# Set the static lib directory
set(PROJECT_STATIC_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_DEBUG_POSTFIX d)
set(LINKER_ARGS "/WX /NODEFAULTLIB:LIBCMT")
#set(MY_COMPILER_FLAGS "/")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/release)

# Set the library output path
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# Set the src files for the project
file(GLOB SRC_FILES "${PROJECT_SRC_DIR}/*.c*")

# Set project includes dir
include_directories("${PROJECT_INCLUDE_DIR}")

add_definitions(-DGLEW_MX -DGLEW_STATIC -D_CRT_SECURE_NO_WARNINGS)
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${LINKER_ARGS}" )
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MY_COMPILER_FLAGS}" )

# Adding executable
add_library(nsengine STATIC ${SRC_FILES})

# add libraries
# add OpenGL
find_library(OPENGL opengl32.lib)
target_link_libraries(nsengine debug
{OPENGL}
${PROJECT_STATIC_LIB_DIR}/glew32mxsd.lib 
${PROJECT_STATIC_LIB_DIR}/assimpsd.libz
${PROJECT_STATIC_LIB_DIR}/DevIL.lib
${PROJECT_STATIC_LIB_DIR}/ILU.lib
${PROJECT_STATIC_LIB_DIR}/ILUT.lib
)

target_link_libraries(nsengine optimized
{OPENGL}
${PROJECT_STATIC_LIB_DIR}/glew32mxs.lib
${PROJECT_STATIC_LIB_DIR}/assimps.lib
${PROJECT_STATIC_LIB_DIR}/DevIL.lib
${PROJECT_STATIC_LIB_DIR}/ILU.lib
${PROJECT_STATIC_LIB_DIR}/ILUT.lib
)
