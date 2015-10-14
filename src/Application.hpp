//
//  Application.hpp
//  luaserver
//
//  Created by Fernando Testa on 13/10/2015.
//  Copyright © 2015 Fernando Testa. All rights reserved.
//

#ifndef Application_hpp
#define Application_hpp

#include "LuaUtils.hpp"
#include <string>

class Application
{
    int _port;
    std::string _script;
    CLuaInterpreter _interpreter;
public:
    Application(int argc, const char ** argv);
    void initialise();
    void run();
};

#endif /* Application_hpp */
