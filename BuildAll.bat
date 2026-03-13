@echo off
setlocal

:: dir setting
set ROOT_DIR=%~dp0
set PREBUILT_DIR=%ROOT_DIR%External
set BIN_OUTPUT_DIR=%ROOT_DIR%bin
set RESOURCE_ROOT_PATH=%ROOT_DIR%Resource

:: extern dependency load
echo extern dependenvy load

mkdir "%PREBUILT_DIR%/build"
mkdir "%BIN_OUTPUT_DIR%"
cd "%PREBUILT_DIR%/build"

:: CMake extern dependency build
echo download extern source... 
cmake .. ^
    -DEXT_INSTALL_DIR="%PREBUILT_DIR%" ^
    -DBIN_OUTPUT_DIR="%BIN_OUTPUT_DIR%" ^
    -DBUILD_D3D11_PROJECT=ON ^
    -G "Visual Studio 17 2022" ^
    -A x64

echo Debug build...
cmake --build . --config Debug

echo Release build...
cmake --build . --config Release

cd "%ROOT_DIR%"
echo build all extern source done

:: Ideal Graphics engine build
mkdir "%ROOT_DIR%build"
cd "%ROOT_DIR%build"

cmake .. ^
    -DEXT_INSTALL_DIR="%PREBUILT_DIR%" ^
    -DBIN_OUTPUT_DIR="%BIN_OUTPUT_DIR%" ^
    -DRESOURCE_ROOT_PATH="%RESOURCE_ROOT_PATH%" ^
    -DCMAKE_INSTALL_LIBDIR="lib/$<CONFIG>" ^
    -DCMAKE_INSTALL_BINDIR="bin/$<CONFIG>" ^
    -G "Visual Studio 17 2022" ^
    -A x64

cd "%ROOT_DIR%"

endlocal
pause