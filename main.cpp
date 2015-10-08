#include <iostream>
#include "LuaUtils.hpp"
#include <math.h>
/*
 Check http://www.rasterbar.com/products/luabind/docs.html 
 for more sofisticated C++ to Lua binding.
 Refs: To see also Nginx binding to lua and usign of coroutines. 
    See also https://openresty.org/
 */

// testing exporting some C function to lua
 int l_sin (lua_State *L) {
    double d = luaL_checknumber(L, 1);
    lua_pushnumber(L, sin(d));
    return 1;  /* number of results */
}

int main()
{
    try {
        CLuaInterpreter interpreter;
        interpreter.register_function("mysin" ,l_sin);
        interpreter.load_file("test.lua");

        long x,y;
        {
            CLuaCall myfancyfunction(interpreter, "myfancyfunction");
            myfancyfunction << 12 << 13; // x,y
            myfancyfunction.call(2); // number of results expected. Must match.
            myfancyfunction >> y >> x; // get result, remember, Lua is a FILO stack
        }
        std::cout << "x,y=(" << x << "," << y << ") got Lua call\n";
    }
    catch(lua_exception& e)
    {
        std::cerr << "LUA EXCEPTION: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}   