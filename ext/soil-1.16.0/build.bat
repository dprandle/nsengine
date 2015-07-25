@echo off
SET CONFIG=Debug
SET CLEAN=NO
SET ARCHSTR="Visual Studio 12 2013"
SET ARCH=x86

FOR %%a IN (%*) DO (
    IF /I "%%a"=="/r" SET CONFIG=Release
    IF /I "%%a"=="/c" SET CLEAN=YES
    IF /I "%%a"=="/x64" (
  	    SET ARCHSTR="Visual Studio 12 2013 Win64"
	    SET ARCH=x64
    )
  	IF /I "%%a"=="/d" SET CONFIG=Debug
  	IF /I "%%a"=="/x86" (
		SET ARCHSTR="Visual Studio 12"
		SET ARCH=x86
    )
)

cd "f:/Users/DreamInColor/My Documents/EmacsProjects/nsengine/ext/soil-1.16.0"
if %CLEAN% == YES (
    echo Removing build dir
    rm -r build
	echo Adding build dir
    mkdir build
)

cd build
call "F:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall"
cmake -G %ARCHSTR% -DCMAKE_BUILD_TYPE=%CONFIG% ..
echo Building... Config = %CONFIG% and Architecture = %ARCH%
msbuild /m /p:Configuration=%CONFIG% soil.sln