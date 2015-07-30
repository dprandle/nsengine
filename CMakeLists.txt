cmake_minimum_required(VERSION 2.8)

project(nsengine)

# Set the source directory
set(NSENGINE_SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# Set the include directory
set(ASSIMP_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/assimp)
set(HASH_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/hash)
set(IL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/IL)
set(MYGL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/myGL)
set(NSENGINE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/nsengine)
set(SOIL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/soil)
set(EXT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

# Postfix debug builds with d
set(CMAKE_DEBUG_POSTFIX d)

set(PLATFORM "x86" CACHE STRING "Platform for which to run cmake (run for each build type)")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/${PLATFORM})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/${PLATFORM})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${PLATFORM})

# Set the src files for the project
file(GLOB SRC_FILES "${NSENGINE_SRC_DIR}/*.c*")
file(GLOB HEADER_FILES
"${ASSIMP_INCLUDE_DIR}/*.h*"
"${HASH_INCLUDE_DIR}/*.h*"
"${IL_INCLUDE_DIR}/*.h*"
"${MYGL_INCLUDE_DIR}/*.h*"
"${NSENGINE_INCLUDE_DIR}/*.h*"
"${SOIL_INCLUDE_DIR}/*.h*")

# Set project includes dir
include_directories(
"${NSENGINE_INCLUDE_DIR}"
"${EXT_INCLUDE_DIR}")

# Preprocessor definitions
add_definitions(-DGLEW_MX -DGLEW_STATIC -D_CRT_SECURE_NO_WARNINGS)

# Linker flags
set(LINKER_ARGS "/WX /NODEFAULTLIB:LIBCMT")
set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} ${LINKER_ARGS}" )

# Compiler flags (none right now)
#set(MY_COMPILER_FLAGS "/")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MY_COMPILER_FLAGS}" )

# Adding executable
add_library(nsengines STATIC ${SRC_FILES} ${HEADER_FILES})
