@echo off
REM Set up Visual Studio 2022 x64 Native Tools environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

echo Building TCP version...
cl TCP_Server.cpp /Fe:TCP_Server.exe /EHsc
cl TCP_Client.cpp /Fe:TCP_Client.exe /EHsc

echo Building UDP version...
cl UDP_Server.cpp /Fe:UDP_Server.exe /EHsc
cl UDP_Client.cpp /Fe:UDP_Client.exe /EHsc

REM Create bin folder if it doesn't exist
if not exist bin (
    mkdir bin
)

REM Move exe files to bin folder
move /Y TCP_Server.exe bin\
move /Y TCP_Client.exe bin\
move /Y UDP_Server.exe bin\
move /Y UDP_Client.exe bin\

echo.
echo All Build and move complete.
echo.
echo TCP Server: bin\TCP_Server.exe
echo TCP Client: bin\TCP_Client.exe
echo UDP Server: bin\UDP_Server.exe
echo UDP Client: bin\UDP_Client.exe
echo.
echo Usage:
echo 1. For TCP: Run TCP_Server.exe first, then TCP_Client.exe
echo 2. For UDP: Run UDP_Server.exe first, then UDP_Client.exe
echo.
pause 