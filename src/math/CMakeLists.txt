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

if (MSVC)
    set_target_properties(nsmath PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
    set_target_properties(nsmath PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
endif (MSVC)
