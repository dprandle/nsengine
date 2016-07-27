#! /bin/bash 

CONFIG=debug
CONFIG_CMAKE=Debug
PLATFORM=x64
BUILD_ALL=NO
RUN_RC=NO
CHANGE_DIR=NO

copy_dirs()
{    
    cp -r tests/import bin/$PLATFORM 
    cp -r tests/resources bin/$PLATFORM
    mkdir -p bin/$PLATFORM/logs
}

build()
{
    mkdir -p deps/freetype/build
    cd deps/freetype/build
    cmake -DCMAKE_LIBRARY_OUTPUT_DIRECTORY="../../../lib/x64"\
          -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY="../../../lib/x64"\
          -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="../../../bin/x64"\
	  -DBUILD_SHARED_LIBS=ON\
          -DCMAKE_BUILD_TYPE=$CONFIG_CMAKE ..
    make -j
    cd ../../..

    mkdir -p deps/glfw/build
    cd deps/glfw/build
    cmake -DCMAKE_LIBRARY_OUTPUT_DIRECTORY="../../../../lib/x64"\
          -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY="../../../../lib/x64"\
          -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="../../../../bin/x64"\
	  -DBUILD_SHARED_LIBS=ON\
          -DCMAKE_BUILD_TYPE=$CONFIG_CMAKE\
          -DGLFW_BUILD_EXAMPLES=OFF\
          -DGLFW_BUILD_TESTS=OFF\
          -DGLFW_BUILD_DOCS=OFF\
          -DCMAKE_DEBUG_POSTFIX="d"\
          -DGLFW_INSTALL=OFF ..
    make -j
    cd ../../..

    mkdir -p deps/assimp/build
    cd deps/assimp/build
    cmake -DCMAKE_LIBRARY_OUTPUT_DIRECTORY="../../../../lib/x64"\
          -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="../../../../bin/x64"\
          -DCMAKE_BUILD_TYPE=$CONFIG_CMAKE\
          -DASSIMP_BUILD_ASSIMP_TOOLS=OFF\
          -DASSIMP_BUILD_TESTS=OFF ..
    make -j
    cd ../../..

    mkdir -p build/$PLATFORM/$CONFIG
    cd build/$PLATFORM/$CONFIG
    cmake -DCMAKE_BUILD_TYPE=$CONFIG_CMAKE -DPLATFORM=$PLATFORM -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../../..
    make -j
    mv compile_commands.json ../../
    cd ../../..
}

for var in "$@"
do
    if [ "$var" = "-d" ]; then
	CONFIG=debug
	CONFIG_CMAKE=Debug
    elif [ "$var" = "-r" ]; then
	CONFIG=release
	CONFIG_CMAKE=Release
    elif [ "$var" = "-x32" ]; then
	PLATFORM=x86
    elif [ "$var" = "-x64" ]; then
	PLATFORM=x64
    elif [ "$var" = "-c" ]; then
	rm -r -f deps/assimp/build
	rm -r -f build/$PLATFORM/$CONFIG
	rm -r -f bin/$PLATFORM
	rm -r -f lib/$PLATFORM
    elif [ "$var" = "-call" ]; then
	rm -r -f deps/assimp/build
	rm -r -f build
	rm -r -f bin
	rm -r -f lib
    elif [ "$var" = "-all" ]; then
	BUILD_ALL=YES
    fi
done

if [ "$BUILD_ALL" = NO ]; then
    build
    copy_dirs
else
    CONFIG=debug
    CONFIG_CMAKE=Debug
    PLATFORM=x86
    build
    PLATFORM=x64
    build
    CONFIG=release
    CONFIG_CMAKE=Release
    build
    PLATFORM=x86
    build
    copy_dirs
fi
