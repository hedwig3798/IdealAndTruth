#pragma once
// 64bit 루아 라이브러리 5.1
#pragma comment(lib, "E:\\LuaBinding\\x64\\Debug\\lua5.1.lib")

#include "lua.hpp"
#include "lua_tinker.h"

#define LUA_START(L)\
L = luaL_newstate();\
luaL_openlibs(L);

