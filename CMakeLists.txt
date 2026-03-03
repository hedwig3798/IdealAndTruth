cmake_minimum_required(VERSION 3.16)
project(IAT)

# 빌드 환경 설정
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/dist" CACHE PATH "..." FORCE)
endif()

# D3D 확인
if(WIN32)
    include(CheckIncludeFileCXX)
    check_include_file_cxx("d3d11.h" HAS_D3D11)
endif()

if(HAS_D3D11)
    message(STATUS "D3D11 : Enabled")
    add_subdirectory(src/IdealGraphicsD3D11)
else()
    message(STATUS "D3D11 : Pass")
endif()

# 글로벌 설정 파일 생성
configure_file(
    "Settings.lua.in"
    "Settings.lua"
    @ONLY        
)

install(
	TARGETS IdealGraphicsD3D11
    RUNTIME DESTINATION bin    # .exe, .dll
    LIBRARY DESTINATION lib    # 리눅스 .so 
    ARCHIVE DESTINATION lib    # .lib
)

# 공통 파일 설치 (루트 경로에서 바로 참조)
install(
    FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
    "${CMAKE_CURRENT_SOURCE_DIR}/THIRD-PARTY-NOTICES.md"
    "${CMAKE_CURRENT_BINARY_DIR}/Settings.lua"
    DESTINATION .
)

# 시작 프로젝트 (나중에 데모로 수정할 것)
# set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT IAT)
