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
#include <boost/asio.hpp>

namespace io = boost::asio;

class Application
{
    uint32_t _port = 0;
    std::string _script;
    CLuaInterpreter _interpreter;
    io::io_service _ioservice;
public:
    Application(int argc, const char ** argv);
    void initialise();
    void run();
};

#endif /* Application_hpp */
