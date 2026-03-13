@echo off
setlocal

:: dir setting
set ROOT_DIR=%~dp0
set PREBUILT_DIR=%ROOT_DIR%External
set BIN_OUTPUT_DIR=%ROOT_DIR%bin

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
cmake --build . --config Debug --parallel 4
cmake --install . --config Debug --prefix "%PREBUILT_DIR%"

echo Release build...
cmake --build . --config Release --parallel 4
cmake --install . --config Release --prefix "%PREBUILT_DIR%"

cd "%ROOT_DIR%"
echo build all extern source done

endlocal
pause