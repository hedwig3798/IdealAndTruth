
# DirectXTK 가져오기
add_library(DirectXTK STATIC IMPORTED)
set_target_properties(
    DirectXTK 
    PROPERTIES
        IMPORTED_CONFIGURATIONS "DEBUG;RELEASE"
        IMPORTED_LOCATION_DEBUG   "${EXT_INSTALL_DIR}/lib/Debug/DirectXTKd.lib"
        IMPORTED_LOCATION_RELEASE "${EXT_INSTALL_DIR}/lib/Release/DirectXTK.lib"
)

# DirectXTex 가져오기
add_library(DirectXTex STATIC IMPORTED)
set_target_properties(
    DirectXTex 
    PROPERTIES
        IMPORTED_CONFIGURATIONS "DEBUG;RELEASE"
        IMPORTED_LOCATION_DEBUG   "${EXT_INSTALL_DIR}/lib/Debug/DirectXTexd.lib"
        IMPORTED_LOCATION_RELEASE "${EXT_INSTALL_DIR}/lib/Release/DirectXTex.lib"
)

add_library(TLogger SHARED IMPORTED)
set_target_properties(
    TLogger 
    PROPERTIES
        IMPORTED_CONFIGURATIONS "DEBUG;RELEASE"
        IMPORTED_IMPLIB_DEBUG     "${EXT_INSTALL_DIR}/lib/Debug/TLoggerd.lib"
        IMPORTED_IMPLIB_RELEASE   "${EXT_INSTALL_DIR}/lib/Release/TLogger.lib"
        IMPORTED_LOCATION_DEBUG   "${BIN_OUTPUT_DIR}/Debug/TLoggerd.dll"
        IMPORTED_LOCATION_RELEASE "${BIN_OUTPUT_DIR}/Release/TLogger.dll"
)

# 프로젝트 파일 등록
file(GLOB_RECURSE SRC_FILES "private/*.h" "private/*.cpp" "public/*.h")
add_library(IdealGraphicsD3D11 SHARED ${SRC_FILES})

# 외부 라이브러리들이 있는 헤더
target_include_directories(
    IdealGraphicsD3D11 
    PUBLIC 
        "${CMAKE_CURRENT_SOURCE_DIR}/public"
        
    PRIVATE 
        "${CMAKE_CURRENT_SOURCE_DIR}/private"
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${EXT_INSTALL_DIR}/include"
)

target_compile_definitions(IdealGraphicsD3D11 PRIVATE IDEALGRAPHICS_EXPORTS)
target_precompile_headers(IdealGraphicsD3D11 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private/pch.h)

# D3D11 관련 라이브러리 링크
target_link_libraries(
    IdealGraphicsD3D11 PRIVATE
    d3d11
    dxgi
    dxguid
    d3dcompiler
    DirectXTK
    DirectXTex
    TLogger
)

