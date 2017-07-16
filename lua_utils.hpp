#pragma once
#include "lua.hpp"
#include <string>
#include <stdexcept>

class lua_exception : public std::runtime_error
{
public:
    lua_exception(const std::string &s) :
        std::runtime_error(s) {}
};


// handy function to save on global table pointers.
void set_lightuserdata(lua_State*L, const char * name, void*data);
void*get_lightuserdata(lua_State*L, const char * name);

class CLuaOpt
{
    lua_State*L_;
    int nopts_;
    int calls_;
public:
    CLuaOpt(lua_State*L,int nargs);
    ~CLuaOpt();
    CLuaOpt& operator>> (double& v);
    CLuaOpt& operator>> (long& v);
    CLuaOpt& operator>> (std::string& v);
};

/**
 Helper class to manipulate calls to lua functions
 */
class CLuaCall
{
    lua_State *_L;
    int nargs_;
public:
    CLuaCall(lua_State*L, const char * f);
    ~CLuaCall();

    CLuaCall& operator<< (const int v);
    CLuaCall& operator<< (const std::string& v);
    void call(int nresults);
    CLuaCall& operator>> (double& v);
    CLuaCall& operator>> (long& v);
    CLuaCall& operator>> (std::string& v);
};


class CLuaInterpreter
{
    lua_State *_L;

public:
    CLuaInterpreter(void);
    ~CLuaInterpreter(void);

    void load_file(const std::string& filename);
    void load_file(const char * filename);
    lua_State * getState() const { return _L; }
    
    /** This operator enables us to mindlessly pass the CLuaInterpreter to lua calls
     and an implicit conversion will be done by the complier 
     */
    operator lua_State*() { return _L; };
    
    /** Register a function at global level.
     */
    void register_function(const char * function_name, const lua_CFunction f);
    /** Registers a single function within a module */
    void register_function(const char * module_name, const char * function_name, const lua_CFunction f);
};


template<class T>
T luaH_getlightpointer(lua_State*L, const char * name) {
    return reinterpret_cast<T> ( get_lightuserdata(L,name) );
}
