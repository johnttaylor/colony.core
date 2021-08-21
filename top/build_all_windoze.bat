::@echo off
:: This script is used by the CI\Build machine to build the Windows test
:: projects
::
:: usage: build_all_windoze.bat <buildType> <buildNumber>
::

set _TOPDIR=%~dp0

:: Create the outcast workspace
cd
dir ..\
echo:orc.py mkwrk --clean ..\
orc.py mkwrk --clean ..\
dir ..\

:: Get dependent packages
cd
echo:orc.py -v getdeps . 
orc.py -v getdeps . 

:: Build Visual Studio 32-bit projects
echo on
call %_TOPDIR%..\env.bat 1
echo:%1 %2

cd %_TOPDIR%..\tests
%_TOPDIR%..\..\xpkgs\nqbp\other\bob.py  -v vc12 -t --try win32
IF ERRORLEVEL 1 EXIT /b 1
%_TOPDIR%..\..\xpkgs\nqbp\other\bob.py  -v vc12 -t --try cpp11
IF ERRORLEVEL 1 EXIT /b 1

:: Run unit tests
cd %_TOPDIR%\..\tests
%_TOPDIR%..\..\xpkgs\nqbp\other\chuck.py -v --match a.exe --dir vc12
IF ERRORLEVEL 1 EXIT /b 1
%_TOPDIR%..\..\xpkgs\nqbp\other\chuck.py -v --match aa.exe --dir vc12
IF ERRORLEVEL 1 EXIT /b 1
%_TOPDIR%..\..\xpkgs\nqbp\other\chuck.py -v --match a.py --dir vc12
IF ERRORLEVEL 1 EXIT /b 1
%_TOPDIR%..\..\xpkgs\nqbp\other\chuck.py -v --match aa.py --dir vc12
IF ERRORLEVEL 1 EXIT /b 1

:: Everything worked!
exit /b 0
