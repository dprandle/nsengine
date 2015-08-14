#! /bin/bash 

CONFIG=debug
CONFIG_CMAKE=Debug
PLATFORM=x86
BUILD_ALL=NO

cd ~/Documents/code/nsengine

build()
{
    mkdir -p build/$PLATFORM/$CONFIG
    cd build/$PLATFORM/$CONFIG
    cmake -DCMAKE_BUILD_TYPE=$CONFIG_CMAKE -DPLATFORM=$PLATFORM -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../../..
    make -j8
    mv compile_commands.json ../../
    cd ../..
    rc -J
    cd ..
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
    elif [ "$var" = "-call" ]; then
	rm -r build
    elif [ "$var" = "-all" ]; then
	BUILD_ALL=YES
    fi
done

if [ "$BUILD_ALL" = NO ]; then
    build
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
fi

if [ $CONFIG = debug ]; then
    NSENGINE_NAME=libnsengined
    GLEWLIB=libGLEWmxd
    HASHLIB=libhashlibd
    SOILLIB=libsoild
else
    NSENGINE_NAME=libnsengine
    GLEWLIB=libGLEWmx
    HASHLIB=libhashlib
    SOILLIB=libsoil
fi

ar -M <<EOM
    CREATE ./lib/${PLATFORM}/${NSENGINE_NAME}_wdep.a
    ADDLIB ./lib/${PLATFORM}/${NSENGINE_NAME}.a
    ADDLIB ./deps/hashlib-1.0.0/lib/${PLATFORM}/${HASHLIB}.a
    ADDLIB ./deps/soil-1.16.0/lib/${PLATFORM}/${SOILLIB}.a
    ADDLIB ./deps/glew-1.12.0/lib/${PLATFORM}/${GLEWLIB}.a
    SAVE
    END
EOM
ranlib ./lib/${PLATFORM}/${NSENGINE_NAME}_wdep.a
