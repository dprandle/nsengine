@echo off

set CONFIG_CMAKE="Debug"
set CONFIG=debug
set PLATFORM="x64"
set PROJ_PATH=%cd%

call "%VS120COMNTOOLS:~0,-14%VC\vcvarsall.bat" amd64

rmdir /q /s "bin"
rmdir /q /s "lib"
rmdir /q /s "build"
rmdir /q /s "deps\assimp\build"
rmdir /q /s "deps\freetype\build"
rmdir /q /s "deps\glfw\build"
mkdir "build"
mkdir "deps\assimp\build"
mkdir "deps\glfw\build"
mkdir "deps\freetype\build"

chdir "deps\assimp\build"
cmake -G "NMake Makefiles JOM"^
 -DCMAKE_LIBRARY_OUTPUT_DIRECTORY="%PROJ_PATH%/lib/%PLATFORM%"^
 -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY="%PROJ_PATH%/lib/%PLATFORM%"^
 -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="%PROJ_PATH%/bin/%PLATFORM%"^
 -DASSIMP_BUILD_ZLIB=ON^
 -DCMAKE_BUILD_TYPE=%CONFIG_CMAKE% ^
 -DASSIMP_BUILD_ASSIMP_TOOLS=OFF^
 -DASSIMP_BUILD_TESTS=OFF ..
chdir %PROJ_PATH%
copy /Y /B jom.exe "deps\assimp\build\jom.exe"
chdir "deps\assimp\build"
jom /J8
chdir %PROJ_PATH%

chdir "deps\freetype\build"
cmake -G "NMake Makefiles JOM"^
 -DCMAKE_LIBRARY_OUTPUT_DIRECTORY="%PROJ_PATH%/lib/%PLATFORM%"^
 -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY="%PROJ_PATH%/lib/%PLATFORM%"^
 -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="%PROJ_PATH%/bin/%PLATFORM%"^
 -DWITH_ZLIB=OFF^
 -DCMAKE_BUILD_TYPE=%CONFIG_CMAKE% ..
chdir %PROJ_PATH%
copy /Y /B jom.exe "deps\freetype\build\jom.exe"
chdir "deps\freetype\build"
jom /J8
chdir %PROJ_PATH%

chdir "deps\glfw\build"
cmake -G "NMake Makefiles JOM"^
 -DCMAKE_LIBRARY_OUTPUT_DIRECTORY="%PROJ_PATH%/lib/%PLATFORM%"^
 -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY="%PROJ_PATH%/lib/%PLATFORM%"^
 -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="%PROJ_PATH%/bin/%PLATFORM%"^
 -DGLFW_BUILD_EXAMPLES=OFF^
 -DGLFW_BUILD_TESTS=OFF^
 -DGLFW_BUILD_DOCS=OFF^
 -DCMAKE_DEBUG_POSTFIX="d"^
 -DCMAKE_BUILD_TYPE=%CONFIG_CMAKE% ..
chdir %PROJ_PATH%
copy /Y /B jom.exe "deps\glfw\build\jom.exe"
chdir "deps\glfw\build"
jom /J8
chdir %PROJ_PATH%

mkdir "build\\%PLATFORM%\\%CONFIG%"
copy /Y /B jom.exe "build\\%PLATFORM%\\%CONFIG%\jom.exe"
chdir "build\\%PLATFORM%\\%CONFIG%"
cmake -G "NMake Makefiles"^
 -DCMAKE_BUILD_TYPE=%CONFIG_CMAKE%^
 -DPLATFORM=%PLATFORM%^
 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..\..\..
jom /J8
REM move /Y compile_commands.json ..\..\compile_commands.json
cd %PROJ_PATH%

xcopy "tests\import" "bin\\%PLATFORM%\import" /e /v /f /i /y
xcopy "tests\resources" "bin\\%PLATFORM%\resources" /e /v /f /i /y
mkdir "bin\\%PLATFORM%\logs"