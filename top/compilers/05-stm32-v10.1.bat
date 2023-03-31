echo off
IF "/%1"=="/name" ECHO:GCC-ARM (none-eabi) compiler & exit /b 0

call set PATH=%NQBP_XPKGS_ROOT%\GCC-ARM-Toolchain\bin;%PATH%
echo:GCC Arm-none-eabi Environment set 
xsrc\GCC-ARM-Toolchain\bin