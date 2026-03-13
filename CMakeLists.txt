cmake_minimum_required(VERSION 3.16)
project(IAT)

if(BIN_OUTPUT_DIR)
else()
    set(BIN_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BIN_OUTPUT_DIR}/$<CONFIG>")



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
    add_subdirectory(src/IdealGraphicsD3D11Demo)

    install(
    TARGETS 
        IdealGraphicsD3D11 
        IdealGraphicsD3D11Demo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
else()
    message(STATUS "D3D11 : Pass")
endif()

# 글로벌 설정 파일 생성
configure_file(
    "Settings.lua.in"
    "Settings.lua"
    @ONLY        
)

# 시작 프로젝트 (나중에 데모로 수정할 것)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT IdealGraphicsD3D11Demo)
