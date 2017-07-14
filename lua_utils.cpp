// LuaInterpreter.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <sstream>
#include <stdio.h>
#include "lua.hpp"
#include "lua_utils.hpp"

void report (lua_State *L) {
  if (!lua_isnil(L, -1)) {
    const char *msg = lua_tostring(L, -1);
    if (msg == NULL) msg = "(error object is not a string)";
    std::cerr << "LUA SCRIPT: " << msg << std::endl;
    lua_pop(L, 1);
  }
}

//////////////////

CLuaOpt::CLuaOpt(lua_State*L, int nargs)
:L_(L),calls_(1)
{
	nopts_ = lua_gettop(L);
	if(nopts_<nargs)
    {
        std::stringstream ss;
        ss << "Missing arguments: " << nopts_ << " out of " << nargs;
        throw lua_exception(ss.str());
    }
}

CLuaOpt::~CLuaOpt()
{
	for(int i=0;i<nopts_;i++)
		lua_pop(L_,-1);
}

CLuaOpt& CLuaOpt::operator >>(double& v)
{
	if(calls_<=nopts_)
	{
		v = lua_tonumber(L_,calls_);
		++calls_;	
		return *this;
	}
	else
		throw lua_exception("CLuaOpt::operator>>(double): no more values on stack.");
}

CLuaOpt& CLuaOpt::operator >>(long& v)
{
	if(calls_<=nopts_)
	{
		v = lua_tointeger(L_,calls_);
		++calls_;	
		return *this;
	}
	else
		throw lua_exception("CLuaOpt::operator>>(int): no more values on stack.");

}

CLuaOpt& CLuaOpt::operator >>(std::string& v)
{
	if(calls_<=nopts_)
	{
		const char * str = lua_tostring(L_,calls_);
		if(str) v = str;
		++calls_;	
		return *this;
	}
	else
		throw lua_exception("CLuaOpt::operator>>(std::string): no more values on stack.");

}


//////////////////

CLuaInterpreter::CLuaInterpreter()
: _L(nullptr)
{
	_L=luaL_newstate();
	luaL_openlibs(_L);
}

CLuaInterpreter::~CLuaInterpreter() 
{
	if (_L) {
		lua_close(_L);
		_L=NULL; // we really don't need this...
	}
}

void CLuaInterpreter::load_file(const char * filename)
{
	int rc = luaL_dofile(_L, filename);
	if(rc) {
		report(_L);
        std::stringstream ss;
        ss << "CLuaInterpreter::load_file("<< filename << ")";
		throw lua_exception(ss.str());
	}
}

void CLuaInterpreter::load_file(const std::string& filename)
{
    load_file(filename.c_str());
}


void CLuaInterpreter::register_function(const char * function_name, const lua_CFunction f)
{
    lua_pushcfunction(_L, f);
    lua_setglobal(_L, function_name); // TODO: check return codes
}

void register_function(const char * module_name, const char * function_name, const lua_CFunction f)
{
    // TODO: implement
}

void set_lightuserdata(lua_State*L,const char * name, void * data)
{
  lua_pushlightuserdata(L,data);
  lua_setglobal(L,name);
}

void * get_lightuserdata(lua_State*L,const char * name)
{
    lua_getglobal(L,name); // pushes on stack
    void * ud = lua_touserdata(L,-1); // pick from stack
    lua_pop(L,1);
    if(ud)
        return ud;
    std::stringstream ss;
    ss << "get_lightuserdata(" << name << " ) failed";
    throw lua_exception(ss.str());
}

////////////////////////////////////////////// 

CLuaCall::CLuaCall(lua_State *L, const char * func_name)
: _L(L), nargs_(0)
{
    // See http://stackoverflow.com/questions/10087226/lua-5-2-lua-globalsindex-alternative
    lua_getglobal(_L,func_name); // pushes func_name function onto the stack
    if(LUA_TFUNCTION!=lua_type(_L, -1))
    {
        std::stringstream ss;
        ss << "CLuaCall::CLuaCall(): lua_getglobal('"<< func_name << "') must return a LUA_TFUNCTION. Attempted ";
        throw lua_exception(ss.str());
    }
}

CLuaCall::~CLuaCall()
{

}

void CLuaCall::call(int nresults)
{
	int rc = lua_pcall(_L, nargs_, nresults, NULL);
	switch(rc)
	{
	case 0:
		return;
	case LUA_ERRRUN:
		report(_L);
        //throw lua_exception("LUA_ERRRUN: Lua script runtime error.");
            
	case LUA_ERRMEM:
		report(_L);
		lua_exception("LUA_ERRMEM: memory allocation error");
	case LUA_ERRERR:
		report(_L);
		lua_exception("LUA_ERRRUN: error while running the error handler function.");
	}
}

CLuaCall& CLuaCall::operator<< (const int v)
{
	lua_pushinteger(_L,v);
	++nargs_;
	return *this;
}

CLuaCall& CLuaCall::operator<< (const std::string& v)
{
	lua_pushstring(_L,v.c_str());
	++nargs_;
	return *this;
}

CLuaCall& CLuaCall::operator>> (double& v)
{
	v = lua_tonumber(_L,-1);
	lua_pop(_L,1);
	return *this;	
}

CLuaCall& CLuaCall::operator>> (long& v)
{
	v = lua_tointeger(_L,-1);
	lua_pop(_L,1);
	return *this;	
}

CLuaCall& CLuaCall::operator>> (std::string& v)
{
	const char * str = lua_tostring(_L,-1);
	if(str) v = str;
	lua_pop(_L,1);
	return *this;
}

