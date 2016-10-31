@echo off
git pull
call build.bat /d /x64 /build
call build.bat /r /x64 /build
call build.bat /d /x86 /build
call build.bat /r /x86 /build