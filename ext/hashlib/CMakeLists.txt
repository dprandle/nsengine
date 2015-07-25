cmake_minimum_required(VERSION 2.8)

project(nsengine)

# Set the source directory
set(PROJECT_SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# Set the include directory
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

set(CMAKE_DEBUG_POSTFIX d)

# Set the library output path
if(CMAKE_GENERATOR MATCHES "Win64")
set(POSTFIX x64)
else()
set(POSTFIX x86)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/lib/${POSTFIX})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/lib/${POSTFIX})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/${POSTFIX})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/lib/${POSTFIX})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/lib/${POSTFIX})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/${POSTFIX})


# Set the src files for the project
file(GLOB SRC_FILES "${PROJECT_SRC_DIR}/*.c*")

# Set project includes dir
include_directories(
"${PROJECT_INCLUDE_DIR}"
)

# Preprocessor definitions
# add_definitions()

# Linker flags
set(LINKER_ARGS "/WX /NODEFAULTLIB:LIBCMT")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${LINKER_ARGS}" )

# Compiler flags
#set(MY_COMPILER_FLAGS "/")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MY_COMPILER_FLAGS}" )

# Adding library
add_library(hashprojs STATIC ${SRC_FILES})
