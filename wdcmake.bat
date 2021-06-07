@echo off

REM Build the kernel using the WDC compiler for 65816

REM Set the target system: 1 = FMX, 2 = U, 3 = U+
SET TARGET=3

SET CFLAGS=-ML -LT -I include -I types -I fatfs -DTARGET=%TARGET%
SET ASFLAGS=-DUSING_816 -DLARGE -V

cd src
WDC816AS %ASFLAGS% -O ..\build\fxloader.obj sys\fxloader.asm
IF ERRORLEVEL 1 GOTO error

REM Build all the device drivers
FOR %%X IN (dev\*.c) DO (
    WDC816CC %CFLAGS% -O ..\build\%%~nX.obj dev\%%~nX.c
    IF ERRORLEVEL 1 GOTO error)

REM Build all the channel drivers
FOR %%X IN (dev\channel\*.c) DO (
    WDC816CC %CFLAGS% -O ..\build\%%~nX.obj dev\channel\%%~nX.c
    IF ERRORLEVEL 1 GOTO error)

REM Build all the channel drivers
FOR %%X IN (dev\block\*.c) DO (
    WDC816CC %CFLAGS% -O ..\build\%%~nX.obj dev\block\%%~nX.c
    IF ERRORLEVEL 1 GOTO error)

REM Build all the types
FOR %%X IN (types\*.c) DO (
    WDC816CC %CFLAGS% -O ..\build\%%~nX.obj types\%%~nX.c
    IF ERRORLEVEL 1 GOTO error)

REM Build FATFS
FOR %%X IN (fatfs\*.c) DO (
    WDC816CC %CFLAGS% -O ..\build\%%~nX.obj fatfs\%%~nX.c
    IF ERRORLEVEL 1 GOTO error)

REM Build command line interface
FOR %%X IN (cli\*.c) DO (
    WDC816CC %CFLAGS% -O ..\build\%%~nX.obj cli\%%~nX.c
    IF ERRORLEVEL 1 GOTO error)

REM Build all the top level C files
FOR %%X IN (*.c) DO (
    WDC816CC %CFLAGS% -O ..\build\%%~nX.obj %%~nX.c
    IF ERRORLEVEL 1 GOTO error)

REM Link all the built OBJ files together into the kernel file...
cd ..\build
DIR /B *.obj >files.lnk
WDCLN -HIE -V -T -P00 -F files.lnk -LCL -O kernel.hex -C010000 -D020000
IF ERRORLEVEL 1 GOTO error
goto :end

:error
echo Build failed!

:end
cd ..
