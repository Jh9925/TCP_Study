@echo off
REM Set up Visual Studio 2022 x64 Native Tools environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

echo Building UDP version...
cl UDP_Server.cpp /Fe:UDP_Server.exe /EHsc
cl UDP_Client.cpp /Fe:UDP_Client.exe /EHsc

REM Create bin folder if it doesn't exist
if not exist bin (
    mkdir bin
)

REM Move exe files to bin folder
move /Y UDP_Server.exe bin\
move /Y UDP_Client.exe bin\

echo.
echo UDP Build and move complete.
echo.
echo UDP Server: bin\UDP_Server.exe
echo UDP Client: bin\UDP_Client.exe
echo.
echo Usage:
echo 1. Run UDP_Server.exe first
echo 2. Run UDP_Client.exe to connect
echo.
pause 