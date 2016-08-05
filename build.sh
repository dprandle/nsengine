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

copy_libs()
{
    rm -r lib/$PLATFORM
    mkdir -p lib/$PLATFORM
    cp -r deps/sndfile/lib/$PLATFORM/libsndfile* lib/$PLATFORM
    cp -r deps/freetype/lib/$PLATFORM/libfreetype* lib/$PLATFORM
    cp -r deps/glfw/lib/$PLATFORM/libglfw* lib/$PLATFORM
    cp -r deps/assimp/lib/$PLATFORM/libassimp* lib/$PLATFORM
}

build()
{
    mkdir -p build/$PLATFORM/$CONFIG
    cd build/$PLATFORM/$CONFIG
    cmake -DCMAKE_BUILD_TYPE=$CONFIG_CMAKE\
	  -DPLATFORM=$PLATFORM\
	  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../../..
    make -j8
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
    copy_libs
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
