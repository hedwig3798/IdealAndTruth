#pragma once
#define LUA_ENUM(L, NAME) \
lua_tinker::def(L, #NAME, static_cast<int>(NAME));

#define DO_STREAM(L, STREAM) \
do \
{ \
const std::string& script = STREAM->str(); \
lua_tinker::dobuffer(L, script.c_str(), script.size()); \
} \
while(0)

#define GET_LUA_TABLE_NEW(L, OUTPUT, NAME) \
lua_tinker::table OUTPUT = lua_tinker::get<lua_tinker::table>(L, NAME);

#define GET_VALUE_NEW(TABLE, OUTPUT, NAME, TYPE) \
TYPE OUTPUT = TABLE.get<TYPE>(NAME);

#define GET_VALUE(TABLE, OUTPUT, NAME, TYPE) \
OUTPUT = TABLE.get<TYPE>(NAME);
