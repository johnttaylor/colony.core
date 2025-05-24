@echo on
:: This script is used by the CI\Build machine to build the Windows test
:: projects
::
:: usage: build_all_windoze.bat <buildNumber> [branch]
::

set _TOPDIR=%~dp0
set _ROOT=%_TOPDIR%..
echo:%_ROOT%
echo:%_TOPDIR%

:: Set the CI build flag
set NQBP_CI_BUILD=1

:: Set the NQBP_BIN path (and other magic variables - but no compiler selected)
call ./env.bat

@REM :: Set Build info (and force build number to zero for "non-official" builds")
@REM set BUILD_TYPE=%2
@REM set BUILD_NUMBER=%1
@REM IF "%BUILD_TYPE%"=="pr" set BUILD_NUMBER=0
@REM IF "%BUILD_TYPE%"=="PR" set BUILD_NUMBER=0
@REM IF "%BUILD_TYPE%"=="unknown" set BUILD_NUMBER=0
@REM IF "/%BUILD_TYPE%" =="/" set BUILD_TYPE=private
@REM echo:
@REM echo:BUILD TYPE=%BUILD_TYPE%, BUILD_NUMBER=%BUILD_NUMBER%
@REM echo:

:: Make sure the _artifacts directory exists and is empty
cd %_ROOT%
rmdir /s /q _artifacts
mkdir _artifacts

@REM :: Run Doxygen first 
@REM cd %_TOPDIR%
@REM run_doxygen.py %BUILD_TYPE% %BUILD_NUMBER%
@REM IF ERRORLEVEL 1 EXIT /b 1
@REM copy %_ROOT%\docs\colony.core-library_%BUILD_NUMBER%-%BUILD_TYPE%.chm %_ROOT%\_artifacts\colony.core-library__%BUILD_NUMBER%-%BUILD_TYPE%.chm
@REM IF ERRORLEVEL 1 EXIT /b 1

@REM ::
@REM :: Build Mingw projects
@REM ::
@REM echo on
@REM call %_ROOT%\env.bat 3

@REM :: Build the Catch2 static library
@REM cd %_ROOT%\projects
@REM %_ROOT%\xsrc\nqbp2\other\bob.py -v4 mingw_w64 -c --bld-all

@REM :: Build the unit tests
@REM cd %_ROOT%\tests
@REM %_ROOT%\xsrc\nqbp2\other\bob.py -v4 mingw_w64 -c --bldtime --bld-all --bldnum %BUILD_NUMBER%
@REM IF ERRORLEVEL 1 EXIT /b 1

@REM :: Run unit tests
@REM cd %_TOPDIR%\..\tests
@REM %_ROOT%\xsrc\nqbp2\other\chuck.py -vt --match a.exe --dir mingw_w64
@REM IF ERRORLEVEL 1 EXIT /b 1
@REM %_ROOT%\xsrc\nqbp2\other\chuck.py -v --match aa.exe --dir mingw_w64
@REM IF ERRORLEVEL 1 EXIT /b 1
@REM %_ROOT%\xsrc\nqbp2\other\chuck.py -vt --match a.py --dir mingw_w64
@REM IF ERRORLEVEL 1 EXIT /b 1
@REM %_ROOT%\xsrc\nqbp2\other\chuck.py -v --match aa.py --dir mingw_w64
@REM IF ERRORLEVEL 1 EXIT /b 1

@REM :: Generate code coverage metrics
@REM %_ROOT%\xsrc\nqbp2\other\chuck.py -v --dir mingw_w64 --match tca.py args --ci rpt --xml jenkins-gcovr.xml 
@REM IF ERRORLEVEL 1 EXIT /b 1


::
:: Build Visual Studio projects
::
@REM echo on
@REM call %_TOPDIR%..\env.bat 1

:: Build the Catch2 static library
cd %_ROOT%\projects
echo:"Build projects..."
echo:"python.exe %_ROOT%\xsrc\nqbp2\other\bob.py -h"
echo:"Build Catch2 static library..."
echo:"%_ROOT%\xsrc\nqbp2\other\bob.py -h"

python.exe %_ROOT%\xsrc\nqbp2\other\bob.py -v vc12 -c --bld-all
IF ERRORLEVEL 1 EXIT /b 1

:: Build the unit tests
cd %_ROOT%\tests
echo:"Building unit tests..."
python %_ROOT%\xsrc\nqbp2\other\bob.py -v vc12 -c --bldtime --bld-all --bldnum %BUILD_NUMBER%
IF ERRORLEVEL 1 EXIT /b 1

@REM :: Run unit tests
@REM cd %_TOPDIR%\..\tests
@REM %_ROOT%\xsrc\nqbp2\other\chuck.py -vt --match a.exe --dir vc12
@REM IF ERRORLEVEL 1 EXIT /b 1
@REM %_ROOT%\xsrc\nqbp2\other\chuck.py -v --match aa.exe --dir vc12
@REM IF ERRORLEVEL 1 EXIT /b 1
@REM %_ROOT%\xsrc\nqbp2\other\chuck.py -vt --match a.py --dir vc12
@REM IF ERRORLEVEL 1 EXIT /b 1
@REM %_ROOT%\xsrc\nqbp2\other\chuck.py -v --match aa.py --dir vc12
@REM IF ERRORLEVEL 1 EXIT /b 1

::
:: Everything worked!
::
exit /b 0
