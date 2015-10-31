cmake_minimum_required(VERSION 2.8)

project(nsengine)

# Set the source directory
set(NSENGINE_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(HASH_SRC_DIR ${PROJECT_SOURCE_DIR}/src/hash)
set(SOIL_SRC_DIR ${PROJECT_SOURCE_DIR}/src/soil)
set(GLEW_SRC_DIR ${PROJECT_SOURCE_DIR}/src/glew)

# Set the include directory
set(NSENGINE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/nsengine)
set(LIB_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(ASSIMP_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/assimp-3.1.1/include)
set(DEVIL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/devil-1.7.8/include)
set(GLFW_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/glfw-3.1.1/include)

# Postfix debug builds with d
set(CMAKE_DEBUG_POSTFIX d)

set(PLATFORM "x86" CACHE STRING "Platform for which to run cmake (run for each build type)")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/${PLATFORM})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/${PLATFORM})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${PLATFORM})

# Set the src files for the project
file(GLOB SRC_FILES 
"${NSENGINE_SRC_DIR}/*.c*" 
"${HASH_SRC_DIR}/*.c*" 
"${SOIL_SRC_DIR}/*.c*" 
"${GLEW_SRC_DIR}/*.c*"
)

file(GLOB HEADER_FILES
"${NSENGINE_INCLUDE_DIR}/*.h*"
)

# Set project includes dir
include_directories(
"${NSENGINE_INCLUDE_DIR}"
"${ASSIMP_INCLUDE_DIR}"
"${DEVIL_INCLUDE_DIR}"
"${LIB_INCLUDE_DIR}"
"${GLFW_INCLUDE_DIR}"
)

# Preprocessor definitions
add_definitions(-DGLEW_MX -DGLEW_STATIC -D_CRT_SECURE_NO_WARNINGS -DNSDEBUG)

if (UNIX)
add_definitions(-std=c++11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -fms-extensions")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpermissive -fms-extensions")
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

if (WIN32)
execute_process(COMMAND ${PROJECT_SOURCE_DIR}/config.bat ${PROJECT_SOURCE_DIR})
endif()

add_subdirectory(deps/glfw-3.1.1)
add_subdirectory(tests)
