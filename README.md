LuaServer
=========

LuaServer is a tiny TCP server that listens to a port and upon connection, data receiving, and disconnection, it calls the corresponding lua callbacks.

There is also an example on how to call a function trom Lua script back to the server.

All executions are asynchronous and there is no polling loop inside lua. As there is currently only one thread of execution, the script callbacks should return as soon as possible to allow C++ code to process other requests.

The program is used as an exercise to practice CMake, Lua bindings and Boost Asio.

To build the project you need the Boost libraries installed in your system. Lua is tiny enough so it is provider
To build:
```
mkdir build
cd build
cmake ..
make
```

Execute with:
```
./LuaServer --port 9876 --script test.lua
```

Connect with netcat and send some stuff:
```
nc localhost 9876
asdfasdf
```

