//
//  Application.hpp
//  luaserver
//
//  Created by Fernando Testa on 13/10/2015.
//  Copyright © 2015 Fernando Testa. All rights reserved.
//

#ifndef Application_hpp
#define Application_hpp

#include "lua_utils.hpp"
#include "LuaServerConfig.h"
#include <string>
#include <boost/asio.hpp>

namespace io = boost::asio;

class Application
{
    uint32_t _port = 0;
    std::string _script;
public:
    Application(int argc, const char ** argv);
    void run();
};

#endif /* Application_hpp */
