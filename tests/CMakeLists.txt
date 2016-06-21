link_libraries(nsengine)
link_libraries(glfw)


if (APPLE)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Building debug inputtest")
    link_libraries(${PROJECT_SOURCE_DIR}/deps/assimp-3.1.1/lib/${PLATFORM}/libassimp.dylib)
    link_libraries(${PROJECT_SOURCE_DIR}/deps/devil-1.7.8/lib/${PLATFORM}/libIL.dylib)
  else()
    message("Building release inputtest")
    link_libraries(${PROJECT_SOURCE_DIR}/deps/assimp-3.1.1/lib/${PLATFORM}/libassimp.dylib)
    link_libraries(${PROJECT_SOURCE_DIR}/deps/devil-1.7.8/lib/${PLATFORM}/libIL.dylib)
  endif()

elseif (UNIX)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Building debug inputtest")
    link_libraries(${PROJECT_SOURCE_DIR}/deps/assimp-3.1.1/lib/${PLATFORM}/libassimpd.so.3)
    link_libraries(${PROJECT_SOURCE_DIR}/deps/devil-1.7.8/lib/${PLATFORM}/libILd.so)
  else()
    message("Building release inputtest")
    link_libraries(${PROJECT_SOURCE_DIR}/deps/assimp-3.1.1/lib/${PLATFORM}/libassimp.so.3)
    link_libraries(${PROJECT_SOURCE_DIR}/deps/devil-1.7.8/lib/${PLATFORM}/libIL.so)
  endif()

elseif (WIN32)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    link_libraries(${PROJECT_SOURCE_DIR}/deps/assimp-3.1.1/lib/${PLATFORM}/assimpd.lib)
    link_libraries(${PROJECT_SOURCE_DIR}/deps/devil-1.7.8/lib/${PLATFORM}/IL.lib)
    link_libraries(DbgHelp)
  else()
    link_libraries(${PROJECT_SOURCE_DIR}/deps/assimp-3.1.1/lib/${PLATFORM}/assimp.lib)
    link_libraries(${PROJECT_SOURCE_DIR}/deps/devil-1.7.8/lib/${PLATFORM}/IL.lib)
    link_libraries(DbgHelp)
  endif()
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

# add_executable(inputtest inputtest.cpp)
# set_target_properties(inputtest PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

#add_executable(buildsystemtest buildsystemtest.cpp)
#set_target_properties(buildsystemtest PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

#add_executable(skyboxtest skyboxtest.cpp)
#set_target_properties(skyboxtest PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

#add_executable(fogtest fogtest.cpp)
#set_target_properties(fogtest PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

#add_executable(res_load_test resource_load_test.cpp)
#set_target_properties(res_load_test PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

#add_executable(animation_test animation_test.cpp)
#set_target_properties(animation_test PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

#add_executable(particle_test particle_test.cpp)
#set_target_properties(particle_test PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

#add_executable(resource_copy_test resource_copy_test.cpp)
#set_target_properties(particle_test PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
