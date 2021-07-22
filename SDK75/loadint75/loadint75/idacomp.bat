@echo off
rem
rem	This command file compresses the predefined comments file
rem
rem
if %1. == . goto Usage
win\btc %1 idabase.tmp
if errorlevel 1 goto Error
copy idabase.tmp %1
del idabase.tmp
goto Ex
:Usage
echo .
echo .  Usage: idacomp filename
echo .         where filename is ida.int file
echo .
goto Ex
:Error
echo .
echo .  Compression failed.
echo .
pause
:Ex
