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

cd "f:/Users/DreamInColor/My Documents/EmacsProjects/nsengine"
if %CLEAN% == YES (
    echo Removing build dir
    rm -r build
	echo Adding build dir
    mkdir build
)

cd build
SET libname=nsengine.lib
call "F:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall"
cmake -G %ARCHSTR% -DCMAKE_BUILD_TYPE=%CONFIG% ..
echo Building... Config = %CONFIG% and Architecture = %ARCH%
msbuild /m /p:Configuration=%CONFIG% nsengine.sln

SET ENGINELIB_PATH=../lib/%ARCH%
SET ASSIMPLIB_PATH=../ext/assimp-3.1.1/lib/%ARCH%
SET GLEWLIB_PATH=../ext/glew-1.12.0/lib/%ARCH%
SET HASHLIB_PATH=../ext/hashlib/lib/%ARCH%
SET SOILLIB_PATH=../ext/soil-1.16.0/lib/%ARCH%

echo Linking static/import %CONFIG% libraries for %ARCH%
if %CONFIG%==Debug (
    lib /OUT:%ENGINELIB_PATH%/nsengined_wdep.lib %ENGINELIB_PATH%/nsengined.lib %GLEWLIB_PATH%/glew32mxsd.lib %ASSIMPLIB_PATH%/assimp-vc120-mtd.lib %HASHLIB_PATH%/hashprojsd.lib %SOILLIB_PATH%/soilsd.lib
) else (
    lib /OUT:%ENGINELIB_PATH%/nsengine_wdep.lib %ENGINELIB_PATH%/nsengine.lib %HASHLIB_PATH%/hashprojs.lib %GLEWLIB_PATH%/glew32mxs.lib %SOILLIB_PATH%/soils.lib %ASSIMPLIB_PATH%/assimp-vc120-mt.lib
)
cd ..
:: Do not need to include the following - they are included with pragma comments - DevIL.lib ILU.lib ILUT.lib