LuaServer
=========

LuaServer is a tiny TCP server that listens to a port and redirect to lua script functions any TCP events such as connection, data input, disconnection.

There is also an example on how to call a function trom Lua script back to the server, making it possible to write apis in C++.

All executions are asynchronous and there is no polling loop inside lua. As there is currently only one thread of execution, the script callbacks should return as soon as possible to allow C++ code to process other requests.

The program was used as an exercise to practice CMake, Lua bindings and Boost Asio, and although workable, not advisable to use in production.

To build the project you need the Boost libraries installed in your system. Lua is tiny enough to be provided with this project.

## Building

From project root, build the LuaServer with:
```
mkdir build
cd build
cmake ..
make
```

## Executing

Execute the echo `test.lua` demo with:
```
build/LuaServer --port 9876 --script test.lua
```

Connect with netcat and send some data to the echo server demo:
```
nc localhost 9876
```

