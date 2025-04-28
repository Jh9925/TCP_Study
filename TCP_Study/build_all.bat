@echo off
REM If you are using Visual Studio's compiler, uncomment the next line and set the correct path:
REM call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

cl TCP_Server.cpp /Fe:TCP_Server.exe /EHsc
cl TCP_Client.cpp /Fe:TCP_Client.exe /EHsc

REM Create bin folder if it doesn't exist
if not exist bin (
    mkdir bin
)

REM Move exe files to bin folder
move /Y TCP_Server.exe bin\
move /Y TCP_Client.exe bin\

echo Build and move complete.
pause 