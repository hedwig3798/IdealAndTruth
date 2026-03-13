
include(FetchContent)

# simpleMath 때문에 임시로
# DirectXTK 가져오기
add_library(DirectXTK STATIC IMPORTED)
set_target_properties(
    DirectXTK 
    PROPERTIES
        IMPORTED_CONFIGURATIONS "DEBUG;RELEASE"
        IMPORTED_LOCATION_DEBUG   "${EXT_INSTALL_DIR}/lib/Debug/DirectXTKd.lib"
        IMPORTED_LOCATION_RELEASE "${EXT_INSTALL_DIR}/lib/Release/DirectXTK.lib"
)

list(
    APPEND 
    CMAKE_PREFIX_PATH 
    "${EXT_INSTALL_DIR}"
    "${EXT_INSTALL_DIR}/lib/Debug/cmake/FMS"
    "${EXT_INSTALL_DIR}/lib/Release/cmake/FMS"
)
find_package(FMS REQUIRED)

# Imgui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.92.6-docking
)
FetchContent_MakeAvailable(imgui)
# imgui D3D source
set(
    IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
)

# lua
FetchContent_Declare(
  lua
  GIT_REPOSITORY https://github.com/lua/lua.git
  GIT_TAG        v5.1.1
)
FetchContent_MakeAvailable(lua)

# lua 의 경우 라이브러리 빌드가 필요하다
file(GLOB LUA_SOURCES "${lua_SOURCE_DIR}/*.c")
add_library(lua STATIC ${LUA_SOURCES})
target_include_directories(lua PUBLIC "${lua_SOURCE_DIR}")

# sol2 라이브러리 (일단 나중에 처리할 것)
# FetchContent_Declare(
#   sol2
#   GIT_REPOSITORY https://github.com/ThePhD/sol2.git
#   GIT_TAG        v3.3.0
# )
# FetchContent_MakeAvailable(sol2)

file(GLOB_RECURSE SRC_FILES "private/*.h" "private/*.cpp")

# 데모 프로젝트
add_executable(IdealGraphicsD3D11Demo WIN32 IdealGraphicsDemo.cpp ${IMGUI_SOURCES} ${SRC_FILES})

target_link_libraries(
    IdealGraphicsD3D11Demo 
    PRIVATE 
        lua
        IdealGraphicsD3D11
        DirectXTK
        FMS::FMS
)

target_include_directories(
    IdealGraphicsD3D11Demo
    PRIVATE
        IdealGraphicsD3D11
        "${lua_SOURCE_DIR}"
        "${imgui_SOURCE_DIR}"
        "${imgui_SOURCE_DIR}/backends"
        "${CMAKE_CURRENT_SOURCE_DIR}/private"
        "${EXT_INSTALL_DIR}/include"
)

if (NOT RESOURCE_ROOT_PATH)
    set(RESOURCE_ROOT_PATH ".")
endif()

configure_file(
    "${CMAKE_SOURCE_DIR}/Settings.lua.in"
    "${BIN_OUTPUT_DIR}/Settings.lua"
    @ONLY # @VAR@ 형태의 변수만 치환하고 싶을 때 사용 (권장)
)

set_target_properties(
    IdealGraphicsD3D11Demo 
    PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)