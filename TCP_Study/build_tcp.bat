@echo off
REM Set up Visual Studio 2022 x64 Native Tools environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

echo Building TCP version...
cl TCP_Server.cpp /Fe:TCP_Server.exe /EHsc
cl TCP_Client.cpp /Fe:TCP_Client.exe /EHsc

REM Create bin folder if it doesn't exist
if not exist bin (
    mkdir bin
)

REM Move exe files to bin folder
move /Y TCP_Server.exe bin\
move /Y TCP_Client.exe bin\

echo.
echo TCP Build and move complete.
echo.
echo TCP Server: bin\TCP_Server.exe
echo TCP Client: bin\TCP_Client.exe
echo.
echo Usage:
echo 1. Run TCP_Server.exe first
echo 2. Run TCP_Client.exe to connect
echo.
pause 