@echo off
call "%VS120COMNTOOLS%\vsvars32.bat"
msbuild "%~dp0\projects\vs2013\Xli Library.sln" /p:Configuration=Debug /p:Platform=Win32 /m || exit /b -1
msbuild "%~dp0\projects\vs2013\Xli Library.sln" /p:Configuration=Release /p:Platform=Win32 /m || exit /b -1
msbuild "%~dp0\projects\vs2013\Xli Library.sln" /p:Configuration=Debug /p:Platform=x64 /m || exit /b -1
msbuild "%~dp0\projects\vs2013\Xli Library.sln" /p:Configuration=Release /p:Platform=x64 /m || exit /b -1