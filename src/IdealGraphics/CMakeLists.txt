include(FetchContent)
include(CheckIncludeFileCXX)

# 외부 라이브러리 가져오기
# DXTK
FetchContent_Declare(
    directxtk
    GIT_REPOSITORY https://github.com/microsoft/DirectXTK.git
    GIT_TAG        oct2025
)
set(DIRECTXTK_AUDIO OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(directxtk)

# 프로젝트 파일 등록
add_library(
    IdealGraphicsD3D11 SHARED 
    Camera.h Camera.cpp
    D3D11Renderer.h D3D11Renderer.cpp
    IRenderer.h IRenderer.cpp
    Sphere.h Sphere.cpp
    stringUtil.h stringUtil.cpp
    Vertex.h Vertex.cpp
    pch.h pch.cpp
    Color.h
    CustomBuffer.h
    framework.h
    Geometry.h
    ICamera.h
    IdealError.h
    Material.h
    RenameType.h
    VertexType.h
    dllmain.cpp
)
target_precompiled_headers(IdealGraphicsD3D11 PRIVATE pch.h)

# 현재 폴더 내의 파일을 탐색
target_include_directories(
    IdealGraphicsD3D11 PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# D3D11 관련 라이브러리 링크
target_link_libraries(
    IdealGraphicsD3D11 PRIVATE
    d3d11
    dxgi
    dxguid
    d3dcompiler
    DirectXTK
)

iat_install_target(IdealGraphicsD3D11)