@echo off
SET CONFIG=Debug
SET ARCH=x86

FOR %%a IN (%*) DO (
    IF /I "%%a"=="/r" SET CONFIG=Release
    IF /I "%%a"=="/x64" (
	    SET ARCH=x64
    )
  	IF /I "%%a"=="/d" SET CONFIG=Debug
  	IF /I "%%a"=="/x86" (
		SET ARCH=x86
    )
)

SET ENGINELIB_PATH=lib/%ARCH%
SET GLEWLIB_PATH=ext/glew-1.12.0/lib/%ARCH%
SET HASHLIB_PATH=ext/hashlib/lib/%ARCH%
SET SOILLIB_PATH=ext/soil-1.16.0/lib/%ARCH%

echo Linking static/import %CONFIG% libraries for %ARCH%
if %CONFIG%==Debug (
    lib /OUT:%ENGINELIB_PATH%/nsenginesd_wdep.lib %ENGINELIB_PATH%/nsenginesd.lib %GLEWLIB_PATH%/glew32mxsd.lib %HASHLIB_PATH%/hashlibsd.lib %SOILLIB_PATH%/soilsd.lib
) else (
    lib /OUT:%ENGINELIB_PATH%/nsengines_wdep.lib %ENGINELIB_PATH%/nsengines.lib %GLEWLIB_PATH%/glew32mxs.lib %HASHLIB_PATH%/hashlibs.lib %SOILLIB_PATH%/soils.lib
)
