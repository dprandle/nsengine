link_libraries(nsengine)

link_directories(${PROJECT_SOURCE_DIR}/lib/${PLATFORM})
link_libraries(debug freetyped)
link_libraries(debug glfwd)
link_libraries(optimized freetype)
link_libraries(optimized glfw)

if (UNIX)
  link_libraries(debug assimpd)
  link_libraries(optimized assimp)
elseif (WIN32)
  link_libraries(debug assimp-vc120-mtd)
  link_libraries(optimized assimp-vc120-mt)
  link_libraries(DbgHelp)
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
