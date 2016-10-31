@echo off

set PROJ_PATH=%cd%
set DO_BUILD=NO
set CLEANUP_BUILD=NO
set BUILD_64=YES
set BUILD_32=YES
set BUILD_DEBUG=YES
set BUILD_RELEASE=YES

for %%x in (%*) do (
    if "%%x"=="/c" (
        echo Cleanup enabled
        set CLEANUP_BUILD=YES
    )
    if "%%x"=="/x64" (
        echo 64 bit build enabled
        set BUILD_32=NO
        set BUILD_64=YES
    )
    if "%%x"=="/x86" (
        echo 32 bit build enabled
        set BUILD_64=NO
        set BUILD_32=YES
    )
    if "%%x"=="/d" (
        echo Debug build enabled
        set BUILD_RELEASE=NO
        set BUILD_DEBUG=YES
    )
    if "%%x"=="/r" (
        echo Release build enabled
        set BUILD_DEBUG=NO
        set BUILD_RELEASE=YES
    )
    if "%%x"=="/build" (
        echo Build with msvc enabled
        set DO_BUILD=YES
    )
)

if %CLEANUP_BUILD%==YES (
    echo Cleaning previous builds...
    rmdir /q /s "build"
)

if %BUILD_64%==YES (
    echo Setting up x64 build....................................
    set PLATFORM="x64"
    set CMAKE_PFORM= Win64
    if %CLEANUP_BUILD%==YES (
        echo Making directories...
        call :doMakeDirs
    )
    if %BUILD_DEBUG%==YES (
        echo Setting up debug build........
        set CONFIG_CMAKE="Debug"
        call :doBuild
    )
    if %BUILD_RELEASE%==YES (
        echo Setting up release build........
        set CONFIG_CMAKE="Release"
        call :doBuild
    )
)

if %BUILD_32%==YES (
    echo Setting up x86 build....................................
    set PLATFORM="x86"
    set CMAKE_PFORM=
    if %CLEANUP_BUILD%==YES (
        echo Making directories...
        call :doMakeDirs
    )
    if %BUILD_DEBUG%==YES (
        echo Setting up debug build........
        set CONFIG_CMAKE="Debug"
        call :doBuild
    )
    if %BUILD_RELEASE%==YES (
        echo Setting up release build........
        set CONFIG_CMAKE="Release"
        call :doBuild
    )
)

:doMakeDirs
mkdir "build\\%PLATFORM%" 2> nul
mkdir "lib\\%PLATFORM%" 2> nul
mkdir "bin\\%PLATFORM%" 2> nul
EXIT /B

:doBuild
call :doLibCopy
echo Configuring nsengine for %PLATFORM% %CONFIG_CMAKE%...
call :buildNSEngine
if %DO_BUILD%==YES (
    echo Building %CONFIG_CMAKE% version nsengine for %PLATFORM%...
    cmake --build build\\%PLATFORM% --config %CONFIG_CMAKE%
)
call :doCopy
EXIT /B

:doCopy
echo Copying files from tests\imports and tests\resources...
xcopy "tests\import" "bin\\%PLATFORM%\import" /e /v /f /i /y > nul
xcopy "tests\resources" "bin\\%PLATFORM%\resources" /e /v /f /i /y > nul
mkdir "bin\\%PLATFORM%\logs" > nul
EXIT /B

:doLibCopy
xcopy "deps\sndfile\lib\\%PLATFORM%" "lib\\%PLATFORM%" /e /v /f /i /y > nul
xcopy "deps\sndfile\bin\\%PLATFORM%" "bin\\%PLATFORM%" /e /v /f /i /y > nul
xcopy "deps\glfw\lib\\%PLATFORM%" "lib\\%PLATFORM%" /e /v /f /i /y > nul
xcopy "deps\glfw\bin\\%PLATFORM%" "bin\\%PLATFORM%" /e /v /f /i /y > nul
xcopy "deps\freetype\lib\\%PLATFORM%" "lib\\%PLATFORM%" /e /v /f /i /y > nul
xcopy "deps\assimp\lib\\%PLATFORM%" "lib\\%PLATFORM%" /e /v /f /i /y > nul
xcopy "deps\assimp\bin\\%PLATFORM%" "bin\\%PLATFORM%" /e /v /f /i /y > nul
xcopy "deps\openal\lib\\%PLATFORM%" "lib\\%PLATFORM%" /e /v /f /i /y > nul
xcopy "deps\openal\bin\\%PLATFORM%" "bin\\%PLATFORM%" /e /v /f /i /y > nul
EXIT /B

:buildNSEngine
chdir "build\\%PLATFORM%"
echo "Visual Studio 14 2015%CMAKE_PFORM%"
cmake -G "Visual Studio 14 2015%CMAKE_PFORM%"^
 -DCMAKE_BUILD_TYPE=%CONFIG_CMAKE%^
 -DPLATFORM=%PLATFORM% ..\..
chdir %PROJ_PATH%
EXIT /B