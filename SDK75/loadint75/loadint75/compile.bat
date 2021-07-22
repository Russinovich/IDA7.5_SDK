@echo off
rem
rem	This file creates a new IDA.INT file (comments database).
rem

rem NLSPATH should point to a directory with IDA.HLP file!
set nlspath=.

win\loadint   comment.cmt ida.int
if errorlevel 1 goto error
win\loadint64 comment.cmt ida64.int
if errorlevel 1 goto error
goto exit
:error
pause
:exit