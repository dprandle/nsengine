
link_directories(${PROJECT_SOURCE_DIR}/lib/${PLATFORM})
link_libraries(nsengine)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  link_libraries(assimpd)
  link_libraries(freetyped)
  link_libraries(glfw3d)
else()
  link_libraries(assimp)
  link_libraries(freetype)
  link_libraries(glfw3)
endif()

# link with opengl (no debug version)
find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
message(ERROR "Error: OpenGL not found on your system - please make sure you have it downloaded.")
endif(NOT OPENGL_FOUND)
link_libraries(${OPENGL_LIBRARIES})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${PLATFORM})

add_executable(most_basic_test most_basic_test.cpp)
set_target_properties(most_basic_test PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

add_executable(window_test window_test.cpp)
set_target_properties(window_test PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
