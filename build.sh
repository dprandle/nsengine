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
    cp -r tests/logs bin/$PLATFORM 
}

build()
{
    mkdir -p build/$PLATFORM/$CONFIG
    cd build/$PLATFORM/$CONFIG
    cmake -DCMAKE_BUILD_TYPE=$CONFIG_CMAKE -DPLATFORM=$PLATFORM -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../../..
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
	rm -r build/$PLATFORM/$CONFIG
	rm -r bin/$PLATFORM
	rm -r lib/$PLATFORM
    elif [ "$var" = "-call" ]; then
	rm -r build
	rm -r bin
	rm -r lib
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
