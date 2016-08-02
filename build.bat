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

rmdir /q /s "bin" 2> nul
rmdir /q /s "lib" 2> nul

if %CLEANUP_BUILD%==YES (
    echo Cleaning previous builds...
    rmdir /q /s "build"
    rmdir /q /s "deps\assimp\build"
    rmdir /q /s "deps\freetype\build"
    rmdir /q /s "deps\glfw\build"
)

if %BUILD_64%==YES (
    echo Setting up x64 build....................................
    set PLATFORM="x64"
    set CMAKE_PFORM = " Win64"
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
    set CMAKE_PFORM = ""
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
mkdir "deps\assimp\build\%PLATFORM%" 2> nul
mkdir "deps\glfw\build\%PLATFORM%" 2> nul
mkdir "deps\freetype\build\%PLATFORM%" 2> nul
mkdir "build\%PLATFORM%" 2> nul
EXIT /B

:doBuild
echo Configuring assimp for %PLATFORM% %CONFIG_CMAKE%...
call :buildAssimp
echo Configuring freetype for %PLATFORM% %CONFIG_CMAKE%...
call :buildFreetype
echo Configuring glfw for %PLATFORM% %CONFIG_CMAKE%....
call :buildGLFW
echo Configuring nsengine for %PLATFORM% %CONFIG_CMAKE%...
call :buildNSEngine
if %DO_BUILD%==YES (
    echo Building %CONFIG_CMAKE% version of assimp for %PLATFORM%...
    cmake --build deps\assimp\build\\%PLATFORM% --config %CONFIG_CMAKE%
    echo Building %CONFIG_CMAKE% version freetype for %PLATFORM%...
    cmake --build deps\freetype\build\\%PLATFORM% --config %CONFIG_CMAKE%
    echo Building %CONFIG_CMAKE% version glfw for %PLATFORM%....
    cmake --build deps\glfw\build\\%PLATFORM% --config %CONFIG_CMAKE%
    echo Building %CONFIG_CMAKE% version nsengine for %PLATFORM%...
    cmake --build build\\%PLATFORM% --config %CONFIG_CMAKE%
)
call :doCopy
EXIT /B

:doCopy
echo Copying files from tests\imports and tests\resources...
xcopy "tests\import" "bin\\%PLATFORM%\\%CONFIG_CMAKE%\import" /e /v /f /i /y > nul
xcopy "tests\resources" "bin\\%PLATFORM%\\%CONFIG_CMAKE%\resources" /e /v /f /i /y > nul
mkdir "bin\\%PLATFORM%\\%CONFIG_CMAKE%\logs" > nul
EXIT /B

:buildAssimp
chdir "deps\assimp\build\\%PLATFORM%"
cmake -G "Visual Studio 14"^
 -DCMAKE_LIBRARY_OUTPUT_DIRECTORY="%PROJ_PATH%/lib/%PLATFORM%"^
 -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY="%PROJ_PATH%/lib/%PLATFORM%"^
 -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="%PROJ_PATH%/bin/%PLATFORM%"^
 -DASSIMP_BUILD_ZLIB=OFF^
 -DCMAKE_BUILD_TYPE=%CONFIG_CMAKE% ^
 -DASSIMP_BUILD_ASSIMP_TOOLS=OFF^
 -DASSIMP_BUILD_TESTS=OFF ..\..
chdir %PROJ_PATH%
EXIT /B

:buildFreetype
chdir "deps\freetype\build\\%PLATFORM%"
cmake -G "Visual Studio 14"^
 -DCMAKE_LIBRARY_OUTPUT_DIRECTORY="%PROJ_PATH%/lib/%PLATFORM%"^
 -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY="%PROJ_PATH%/lib/%PLATFORM%"^
 -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="%PROJ_PATH%/bin/%PLATFORM%"^
 -DWITH_ZLIB=OFF^
 -DCMAKE_BUILD_TYPE=%CONFIG_CMAKE% ..\..
chdir %PROJ_PATH%
EXIT /B

:buildGLFW
chdir "deps\glfw\build\\%PLATFORM%"
cmake -G "Visual Studio 14"^
 -DCMAKE_LIBRARY_OUTPUT_DIRECTORY="%PROJ_PATH%/lib/%PLATFORM%"^
 -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY="%PROJ_PATH%/lib/%PLATFORM%"^
 -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="%PROJ_PATH%/bin/%PLATFORM%"^
 -DGLFW_BUILD_EXAMPLES=OFF^
 -DGLFW_BUILD_TESTS=OFF^
 -DBUILD_SHARED_LIBS=ON^
 -DGLFW_BUILD_DOCS=OFF^
 -DCMAKE_DEBUG_POSTFIX="d"^
 -DCMAKE_BUILD_TYPE=%CONFIG_CMAKE% ..\..
chdir %PROJ_PATH%
EXIT /B

:buildNSEngine
chdir "build\\%PLATFORM%"
cmake -G "Visual Studio 14"^
 -DCMAKE_BUILD_TYPE=%CONFIG_CMAKE%^
 -DPLATFORM=%PLATFORM% ..\..
chdir %PROJ_PATH%
EXIT /B