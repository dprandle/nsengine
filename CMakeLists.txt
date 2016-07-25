cmake_minimum_required(VERSION 2.8)

project(nsengine)

# Set the source directories
set(NSENGINE_SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# Set the include directory
set(NSENGINE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/nsengine)
set(ASSIMP_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/assimp/include)
set(GLFW_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/glfw/include)
set(FREETYPE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/freetype/include)

# Postfix debug builds with d
set(CMAKE_DEBUG_POSTFIX d)

set(PLATFORM "x86" CACHE STRING "Platform for which to run cmake (run for each build type)")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/${PLATFORM})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/${PLATFORM})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${PLATFORM})


# Set the src files for the project
file(GLOB SRC_FILES 
"${NSENGINE_SRC_DIR}/*.cpp"
"${NSENGINE_SRC_DIR}/math/*.cpp"
"${NSENGINE_SRC_DIR}/component/*.cpp"
"${NSENGINE_SRC_DIR}/system/*.cpp"
"${NSENGINE_SRC_DIR}/asset/*.cpp"
"${NSENGINE_SRC_DIR}/opengl/*.cpp"
"${NSENGINE_SRC_DIR}/hash/*.cpp"
"${NSENGINE_SRC_DIR}/*.c"
"${NSENGINE_SRC_DIR}/math/*.c"
"${NSENGINE_SRC_DIR}/component/*.c"
"${NSENGINE_SRC_DIR}/system/*.c"
"${NSENGINE_SRC_DIR}/asset/*.c"
"${NSENGINE_SRC_DIR}/opengl/*.c"
"${NSENGINE_SRC_DIR}/hash/*.c"
)

file(GLOB HEADER_FILES
"${NSENGINE_INCLUDE_DIR}/*.h"
"${NSENGINE_INCLUDE_DIR}/math/*.h"
"${NSENGINE_INCLUDE_DIR}/component/*.h"
"${NSENGINE_INCLUDE_DIR}/system/*.h"
"${NSENGINE_INCLUDE_DIR}/asset/*.h"
"${NSENGINE_INCLUDE_DIR}/hash/*.h"
"${NSENGINE_INCLUDE_DIR}/opengl/*.h"
)

# Set project includes dir
include_directories(
"${NSENGINE_INCLUDE_DIR}"
"${NSENGINE_INCLUDE_DIR}/math"
"${NSENGINE_INCLUDE_DIR}/component"
"${NSENGINE_INCLUDE_DIR}/system"
"${NSENGINE_INCLUDE_DIR}/asset"
"${NSENGINE_INCLUDE_DIR}/opengl"
"${NSENGINE_INCLUDE_DIR}/hash"
"${ASSIMP_INCLUDE_DIR}"
"${GLFW_INCLUDE_DIR}"
"${FREETYPE_INCLUDE_DIR}"
)

# Preprocessor definitions
add_definitions(-DGLEW_MX -DGLEW_STATIC -DSTB_IMAGE_WRITE_IMPLEMENTATION -DSTB_IMAGE_IMPLEMENTATION)
add_definitions(-DNSDEBUG)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
message("Debug build starting...\n")
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif ()

if (UNIX)
  add_definitions(-std=c++11)
#elseif (APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fms-extensions")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fms-extensions")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
  if (PLATFORM STREQUAL "x86")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
  endif()
elseif (WIN32)
  set(LINKER_ARGS "/WX /NODEFAULTLIB:MSVCRT /NODEFAULTLIB:LIBCMT")
  set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ${LINKER_ARGS}" )
endif()

# Adding executable
add_library(nsengine STATIC ${SRC_FILES} ${HEADER_FILES})

#add_subdirectory(deps/assimp)
add_subdirectory(tests)
