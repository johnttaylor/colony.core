@echo off
:: Windows batch file wrapper to execute an orc.py command and then
:: to conditionally execute the output of speficied orc command.
::
:: If the last output line of the orc.py script starts with: '.run: ' - then 
:: this batch file will attempt to execute the last output line - after the 
:: '.run: ' has been stripped off

set _cmd=orc.py %*
set _tempfile=%TEMP%\__orcw_temp_%RANDOM%.bat

call :runorc
IF ERRORLEVEL 1 goto :cleanup
call %_tempfile%

:cleanup
del %_tempfile% 2>NUL
set _cmd=
set _tempfile=
exit /b 0


:: Run the orc script
:runorc
setlocal ENABLEDELAYEDEXPANSION
set _LF=^


:: The two empty lines above are required!
set _W=
FOR /F "usebackq delims==" %%I IN (`%_cmd%`) DO set _W=!_W!%%I!_LF!
set _C=!_W:~0,6!
IF "%_C%" == ".run: " goto :execute 
echo:!_W! 
ENDLOCAL
exit /b 1

:execute
echo:!_W:~6! > %_tempfile%
ENDLOCAL
exit /b 0
