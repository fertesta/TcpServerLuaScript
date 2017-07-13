#include <math.h>
#include <iostream>

#include "Application.hpp"

/*
 Check http://www.rasterbar.com/products/luabind/docs.html 
 for more sofisticated C++ to Lua binding.
 Refs: To see also Nginx binding to lua and usign of coroutines. 
    See also https://openresty.org/
 */


int main(int argc, const char**argv)
{
    try{
        Application app(argc,argv);
        app.initialise();
        app.run();
    }
    catch(std::exception&e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
