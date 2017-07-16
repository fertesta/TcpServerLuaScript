//
//  Application.cpp
//  luaserver
//
//  Created by Fernando Testa on 13/10/2015.
//  Copyright © 2015 Fernando Testa. All rights reserved.
//

#include "application.hpp"
#include "server.hpp"
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


Application::Application(int argc, const char ** argv) {
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

//    _interpreter.register_function("mysin" ,l_sin);
//    _interpreter.load_file(_script);    
}

void Application::run()
{
    std::cout << "luaserver started with script '" << _script << "' on port " << _port << std::endl;
    
    boost::asio::io_service io_service;
    server srv(io_service, _port, _script);
    
    // run the IO service
    io_service.run();
    
}



/*
 Check http://www.rasterbar.com/products/luabind/docs.html 
 for more sofisticated C++ to Lua binding.
 Refs: To see also Nginx binding to lua and coroutines. 
    See also https://openresty.org/
 */


int main(int argc, const char**argv)
{
    try{
        Application app(argc,argv);
        app.run();
    }
    catch(std::exception&e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
