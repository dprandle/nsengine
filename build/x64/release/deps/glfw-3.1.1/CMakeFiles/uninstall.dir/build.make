# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/cmake-gui

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dprandle/Documents/code/nsengine

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dprandle/Documents/code/nsengine/build/x64/release

# Utility rule file for uninstall.

# Include the progress variables for this target.
include deps/glfw-3.1.1/CMakeFiles/uninstall.dir/progress.make

deps/glfw-3.1.1/CMakeFiles/uninstall:
	cd /home/dprandle/Documents/code/nsengine/build/x64/release/deps/glfw-3.1.1 && /usr/bin/cmake -P /home/dprandle/Documents/code/nsengine/build/x64/release/deps/glfw-3.1.1/cmake_uninstall.cmake

uninstall: deps/glfw-3.1.1/CMakeFiles/uninstall
uninstall: deps/glfw-3.1.1/CMakeFiles/uninstall.dir/build.make
.PHONY : uninstall

# Rule to build all files generated by this target.
deps/glfw-3.1.1/CMakeFiles/uninstall.dir/build: uninstall
.PHONY : deps/glfw-3.1.1/CMakeFiles/uninstall.dir/build

deps/glfw-3.1.1/CMakeFiles/uninstall.dir/clean:
	cd /home/dprandle/Documents/code/nsengine/build/x64/release/deps/glfw-3.1.1 && $(CMAKE_COMMAND) -P CMakeFiles/uninstall.dir/cmake_clean.cmake
.PHONY : deps/glfw-3.1.1/CMakeFiles/uninstall.dir/clean

deps/glfw-3.1.1/CMakeFiles/uninstall.dir/depend:
	cd /home/dprandle/Documents/code/nsengine/build/x64/release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dprandle/Documents/code/nsengine /home/dprandle/Documents/code/nsengine/deps/glfw-3.1.1 /home/dprandle/Documents/code/nsengine/build/x64/release /home/dprandle/Documents/code/nsengine/build/x64/release/deps/glfw-3.1.1 /home/dprandle/Documents/code/nsengine/build/x64/release/deps/glfw-3.1.1/CMakeFiles/uninstall.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : deps/glfw-3.1.1/CMakeFiles/uninstall.dir/depend

