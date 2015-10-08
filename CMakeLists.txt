cmake_minimum_required(VERSION 2.8)

project(nsengine)

# Set the source directory
set(NSENGINE_SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# Set the include directory
set(NSENGINE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/nsengine)
set(ASSIMP_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/assimp-3.1.1/include)
set(DEVIL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/devil-1.7.8/include)
set(GLEW_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/glew-1.12.0/include)
set(GLFW_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/glfw-3.1.1/include)
set(HASH_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/hashlib-1.0.0/include)
set(SOIL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/soil-1.16.0/include)

# Postfix debug builds with d
set(CMAKE_DEBUG_POSTFIX d)

set(PLATFORM "x86" CACHE STRING "Platform for which to run cmake (run for each build type)")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/${PLATFORM})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/${PLATFORM})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${PLATFORM})

# Set the src files for the project
file(GLOB SRC_FILES "${NSENGINE_SRC_DIR}/*.c*")
file(GLOB HEADER_FILES
"${NSENGINE_INCLUDE_DIR}/*.h*"
"${ASSIMP_INCLUDE_DIR}/*.h*"
"${DEVIL_INCLUDE_DIR}/*.h*"
"${GLEW_INCLUDE_DIR}/*.h*"
"${GLFW_INCLUDE_DIR}/*.h*"
"${HASH_INCLUDE_DIR}/*.h*"
"${SOIL_INCLUDE_DIR}/*.h*")

# Set project includes dir
include_directories(
"${NSENGINE_INCLUDE_DIR}"
"${ASSIMP_INCLUDE_DIR}"
"${DEVIL_INCLUDE_DIR}"
"${GLEW_INCLUDE_DIR}"
"${GLFW_INCLUDE_DIR}"
"${HASH_INCLUDE_DIR}"
"${SOIL_INCLUDE_DIR}"
)

# Preprocessor definitions
add_definitions(-DGLEW_MX -DGLEW_STATIC -D_CRT_SECURE_NO_WARNINGS -DNSDEBUG -std=c++11)

if (UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -fms-extensions")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpermissive -fms-extensions")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
  if (PLATFORM STREQUAL "x86")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
  endif()
  
elseif (WIN32)
  set(LINKER_ARGS "/WX /NODEFAULTLIB:LIBCMT")
  set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} ${LINKER_ARGS}" )
endif()

# Adding executable
add_library(nsengine STATIC ${SRC_FILES} ${HEADER_FILES})

add_subdirectory(tests)