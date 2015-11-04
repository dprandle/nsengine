#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "glfw" for configuration "Debug"
set_property(TARGET glfw APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(glfw PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "/usr/lib/x86_64-linux-gnu/libX11.so;-lpthread;/usr/lib/x86_64-linux-gnu/librt.so;/usr/lib/x86_64-linux-gnu/libXrandr.so;/usr/lib/x86_64-linux-gnu/libXinerama.so;/usr/lib/x86_64-linux-gnu/libXi.so;/usr/lib/x86_64-linux-gnu/libXxf86vm.so;/usr/lib/x86_64-linux-gnu/librt.so;/usr/lib/x86_64-linux-gnu/libm.so;/usr/lib/x86_64-linux-gnu/libXcursor.so;/usr/lib/x86_64-linux-gnu/libGL.so"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libglfwd.so.3.1"
  IMPORTED_SONAME_DEBUG "libglfwd.so.3"
  )

list(APPEND _IMPORT_CHECK_TARGETS glfw )
list(APPEND _IMPORT_CHECK_FILES_FOR_glfw "${_IMPORT_PREFIX}/lib/libglfwd.so.3.1" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
