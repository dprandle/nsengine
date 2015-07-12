@echo off
cd "f:/Users/DreamInColor/My Documents/EmacsProjects/nsengine"
if "%2" == "/c" (
   rm -r build
   mkdir build
)

cd build
SET libname=nsengine.lib
call "F:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall"
if "%1"=="/d" (
	cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
	SET projname=nsengined.lib
) else (
	cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release ..
)

jom -j 20
cd ../lib
if "%1"=="/d" (
   		lib /OUT:nsengined.lib nsengined.lib DevIL.lib ILU.lib ILUT.lib assimpd.lib glew32mxsd.lib
) else (
		lib /OUT:nsengine.lib nsengine.lib DevIL.lib ILU.lib ILUT.lib assimp.lib glew32mxs.lib
)
cd ..