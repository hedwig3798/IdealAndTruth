#pragma once
#include "IdealRenderer/IRenderer.h"
#include "stringUtil.h"
typedef lua_tinker::table table;

#define LUA_ENUM(L, NAME) \
lua_tinker::def(L, #NAME, static_cast<int>(NAME));

#define DO_STREAM(L, STREAM) \
do \
{ \
lua_tinker::dobuffer(L, reinterpret_cast<const char*>(STREAM.data()), STREAM.size()); \
} \
while(0)

template <typename T>
struct LuaValueGetter 
{
	static T Get(lua_State* _L, const char* _name)
	{
		return lua_tinker::get<T>(_L, _name);
	}

	static T Get(lua_tinker::table& _table, const char* _name)
	{
		return _table.get<T>(_name);
	}

	static T Get(lua_tinker::table& _table, int _index)
	{
		return _table.get<T>(_index);
	}

	static T Get(lua_tinker::table& _table, size_t _index)
	{
		return _table.get<T>(static_cast<int>(_index));
	}
};

template <>
struct LuaValueGetter<Vector3>
{
	static Vector3 Get(lua_tinker::table& _table, const char* _name)
	{
		Vector3 result = Vector3::Zero;

		lua_tinker::table temp = LuaValueGetter<lua_tinker::table>::Get(_table, _name);
		if (3 != temp.size())
		{
			return result;
		}

		result.x = LuaValueGetter<float>::Get(temp, 1);
		result.y = LuaValueGetter<float>::Get(temp, 2);
		result.z = LuaValueGetter<float>::Get(temp, 3);

		return result;
	}

	static Vector3 Get(lua_State* _L, const char* _name)
	{
		Vector3 result = Vector3::Zero;

		lua_tinker::table temp = LuaValueGetter<lua_tinker::table>::Get(_L, _name);
		if (3 != temp.size())
		{
			return result;
		}

		result.x = LuaValueGetter<float>::Get(temp, 1);
		result.y = LuaValueGetter<float>::Get(temp, 2);
		result.z = LuaValueGetter<float>::Get(temp, 3);

		return result;
	}
};

template <>
struct LuaValueGetter<std::string>
{
	static std::string Get(lua_State* _L, const char* _name)
	{
		char* value = lua_tinker::get<char*>(_L, _name);
		std::string result = "";
		if (nullptr != value)
		{
			result = value;
		}
		return result;
	}

	static std::string Get(lua_tinker::table& _table, const char* _name)
	{
		char* value = _table.get<char*>(_name);
		std::string result = "";
		if (nullptr != value)
		{
			result = value;
		}
		return result;
	}

	static std::string Get(lua_tinker::table& _table, int _index)
	{
		char* value = _table.get<char*>(_index);
		std::string result = "";
		if (nullptr != value)
		{
			result = value;
		}
		return result;
	}

	static std::string Get(lua_tinker::table& _table, size_t _index)
	{
		char* value = _table.get<char*>(static_cast<int>(_index));
		std::string result = "";
		if (nullptr != value)
		{
			result = value;
		}
		return result;
	}
};

template <>
struct LuaValueGetter<std::wstring>
{
	static std::wstring Get(lua_tinker::table& _table, const char* _name)
	{
		std::wstring result = L"";
		std::string s = _table.get<std::string>(_name);
		result = ::StrToWstr(s);
		return result;
	}

	static std::wstring Get(lua_State* _L, const char* _name)
	{
		std::wstring result = L"";
		std::string s = LuaValueGetter<std::string>::Get(_L, _name);
		result = ::StrToWstr(s);
		return result;
	}
};





