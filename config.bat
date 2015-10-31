@echo off
xcopy "%~1\tests\resources" "%~1\bin\x64\resources" /e /v /f /i /y
xcopy "%~1\tests\import" "%~1\bin\x64\import" /e /v /f /i /y
xcopy "%~1\tests\plugins" "%~1\bin\x64\plugins" /e /v /f /i /y
xcopy "%~1\tests\resources" "%~1\bin\x86\resources" /e /v /f /i /y
xcopy "%~1\tests\import" "%~1\bin\x86\import" /e /v /f /i /y
xcopy "%~1\tests\plugins" "%~1\bin\x86\plugins" /e /v /f /i /y
xcopy "%~1\deps\assimp-3.1.1\bin\x64\*" "%~1\bin\x64\" /e /v /f /y
xcopy "%~1\deps\assimp-3.1.1\bin\x86\*" "%~1\bin\x86\" /e /v /f /y
xcopy "%~1\deps\devil-1.7.8\bin\x64\DevIL.dll" "%~1\bin\x64\" /e /v /f /y
xcopy "%~1\deps\devil-1.7.8\bin\x86\DevIL.dll" "%~1\bin\x86\" /e /v /f /y
