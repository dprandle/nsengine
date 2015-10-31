@echo off
xcopy "tests\resources" "bin\x64\resources" /e /v /f /i /y
xcopy "tests\import" "bin\x64\import" /e /v /f /i /y
xcopy "tests\resources" "bin\x86\resources" /e /v /f /i /y
xcopy "tests\import" "bin\x86\import" /e /v /f /i /y
xcopy "deps\assimp-3.1.1\bin\x64\*" "bin\x64\" /e /v /f /y
xcopy "deps\assimp-3.1.1\bin\x86\*" "bin\x86\" /e /v /f /y
xcopy "deps\devil-1.7.8\bin\x64\DevIL.dll" "bin\x64\" /e /v /f /y
xcopy "deps\devil-1.7.8\bin\x86\DevIL.dll" "bin\x86\" /e /v /f /y
