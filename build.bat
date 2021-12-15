@echo off

echo Building using GCC...

REM Try to use GCC compiler, if available.

gcc -m32 -Wall -I "%~dp0res" "%~dp0src\main.c" "%~dp0src\poison.c" "%~dp0src\pfunc.c" "%~dp0res\rsrc.o" "%~dp0lib\ufmod.obj" -o "%~dp0poison.exe" -lpsapi -lwinmm -mwindows

echo;

if %errorlevel%==9009 echo GCC is not installed or PATH not properly set! & echo; & goto msvc

if %errorlevel% NEQ 0 ( echo Building failed... & goto end ) else ( echo; & echo Build OK... & goto end )


REM If GCC is not installed, try MSVC compiler

:msvc

echo Building using MSVC compiler... & echo;

cl /I "%~dp0res" "%~dp0src\main.c" "%~dp0src\poison.c" "%~dp0src\pfunc.c" "%~dp0res\rsrc.o" "%~dp0lib\ufmod.obj" /Fo:"%~dp0\" user32.lib gdi32.lib advapi32.lib comdlg32.lib winmm.lib psapi.lib /link /out:"%~dp0poison.exe"

echo;

if %errorlevel%==9009 echo MSVC compiler is not installed or PATH not properly set! & goto end

echo;

if %errorlevel% NEQ 0 (echo; & echo Building failed... & echo;) else (echo; & echo Build OK... & echo;)

REM Clean object files

if exist "%~dp0*.obj" (
echo Deleting object files... 
del "%~dp0\*.obj" 
)

:end

echo;
pause
exit