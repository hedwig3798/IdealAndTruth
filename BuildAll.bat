@echo off
setlocal

:: dir setting
set ROOT_DIR=%~dp0
set PREBUILT_DIR=%ROOT_DIR%External

:: extern dependency load
echo extern dependenvy load

mkdir "%PREBUILT_DIR%/build"
cd "%PREBUILT_DIR%/build"

:: CMake extern dependency build
echo download extern source... 
cmake .. -DEXT_INSTALL_DIR="%PREBUILT_DIR%" -G "Visual Studio 17 2022" -A x64

echo Debug build...
cmake --build . --config Debug

echo Release build...
cmake --build . --config Release

cd "%ROOT_DIR%"
echo build all extern source done

:: Ideal Graphics engine build
mkdir "%ROOT_DIR%build"
cd "%ROOT_DIR%build"

cmake .. -DEXT_INSTALL_DIR="%PREBUILT_DIR%" -G "Visual Studio 17 2022" -A x64

cd "%ROOT_DIR%"

endlocal
pause