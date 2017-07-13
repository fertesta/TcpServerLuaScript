//
//  Application.cpp
//  luaserver
//
//  Created by Fernando Testa on 13/10/2015.
//  Copyright © 2015 Fernando Testa. All rights reserved.
//

#include "Application.hpp"
#include "TCPServer.hpp"
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
namespace io = boost::asio;

// testing exporting some C function to lua
int l_sin (lua_State *L) {
    double d = luaL_checknumber(L, 1);
    lua_pushnumber(L, sin(d));
    return 1;  /* number of results */
}


Application::Application(int argc, const char ** argv)
: _ioservice()
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("port", po::value<uint32_t>(&_port)->default_value(6666), "set port")
        ("script", po::value<std::string>(&_script), "set the lua script to run")
    ;
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        throw std::runtime_error("help requested");
    }
    
    if (!vm.count("script")) {
        throw std::runtime_error("missing parameter 'script'");
    }
    
    if (!vm.count("port")) {
        throw std::runtime_error("missing parameter 'port'");
    }
    
}

void Application::initialise()
{
    _interpreter.register_function("mysin" ,l_sin);
    _interpreter.load_file(_script);
    TCPServer server(_port);
    auto handler_receive = [this]()
    {
        
    };
    server.set_handler_receive(handler_receive);
    
    server.run();
}

void Application::run()
{
    std::cout << "luaserver started with script '" << _script << "' on port " << _port << std::endl;
    
    std::string data;
    {
        CLuaCall call_handle_recv(_interpreter, "handle_recv");
        call_handle_recv << "fake data here";
        call_handle_recv.call(1); // number of results expected. Must match.
        call_handle_recv >> data; // get result, remember, Lua is a FILO stack
    }
    std::cout << "handle_recv()=(" << data << ") got Lua call\n";
    
    // run the IO service
    _ioservice.run();
    
}


