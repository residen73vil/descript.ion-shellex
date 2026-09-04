@echo off
setlocal

if /i "%PROCESSOR_ARCHITECTURE%"=="AMD64" goto 64BIT
if /i "%PROCESSOR_ARCHITECTURE%"=="IA64"  goto ARCH_UNSUPORTED
if /i "%PROCESSOR_ARCHITECTURE%"=="ARM64"  goto ARCH_UNSUPORTED
if /i "%PROCESSOR_ARCHITECTURE%"=="ARM"    goto ARCH_UNSUPORTED

goto 32BIT

:ARCH_UNSUPORTED
echo This cpu architecture is not supported.
pause
exit /b 1

:64BIT
echo 64-bit Windows detected.
set "DLL=descript.ion-shellex64.dll"
goto INSTALL

:32BIT
echo 32-bit Windows detected.
set "DLL=descript.ion-shellex32.dll"


:INSTALL

:: Check for administrator privileges
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo Administrator rights are required. Requesting elevation...

    powershell -NoProfile -ExecutionPolicy Bypass -Command ^
        "Start-Process -FilePath '%~f0' -Verb RunAs"

    exit /b
)

echo Running with administrator rights.

:: File to copy, located next to this script
set "SOURCE_FILE=%~dp0%DLL%"
set "DEST_FILE=%WINDIR%\descript.ion-shellex.dll"

:: Check that the source file exists
if not exist "%SOURCE_FILE%" (
    echo Error: "%SOURCE_FILE%" was not found.
    pause
    exit /b 1
)

:: Copy the file to the Windows folder
copy /Y "%SOURCE_FILE%" "%DEST_FILE%" >nul

if errorlevel 1 (
    echo Error: File copy failed.
    pause
    exit /b 1
)

echo File copied to "%WINDIR%".

:: Run your command here
rundll32.exe %WINDIR%\descript.ion-shellex.dll,install

if errorlevel 1 (
    echo The command failed.
    pause
    exit /b 1
)

