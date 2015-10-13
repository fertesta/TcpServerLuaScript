#include "LuaUtils.hpp"
#include <ev.h>
#include <math.h>
#include <iostream>
#include <thread>
#include <vector>

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

void run_interpreter(const char * luascript)
{
    try {
        CLuaInterpreter interpreter;
        interpreter.register_function("main" ,l_sin);
        interpreter.load_file(luascript);
        
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
        return;
    }
}

int main()
{
    const int number_of_threads = 100;
    std::cout << "thread::hardware_concurrency = " << std::thread::hardware_concurrency() << std::endl;
    std::vector<std::thread> ts;
    for(int i=0;i<number_of_threads;i++)
    {
        ts.push_back(std::thread(run_interpreter,"test.lua"));
        
    }

    for(auto& it : ts)
        it.join();
    
    return 0;
}