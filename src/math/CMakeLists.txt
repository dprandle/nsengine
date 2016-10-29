# Set the src files for the project
file(GLOB MATH_SRC_FILES 
"*.cpp"
"*.c"
)

file(GLOB MATH_HEADER_FILES
"*.h"
)

message("${CMAKE_BUILD_TYPE} building math library...\n")

# Adding executable
add_library(nsmath STATIC ${MATH_SRC_FILES} ${MATH_HEADER_FILES})
