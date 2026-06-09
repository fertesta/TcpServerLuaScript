# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

LuaServer is a C++11 asynchronous TCP server that drives Lua scripts via event callbacks. TCP events (connect, receive, disconnect) are dispatched to Lua functions. Lua scripts can also call back into C++ via registered functions. Built as an exercise in CMake, Boost Asio, and Lua C API binding.

## Build

Requires Boost libraries installed system-wide (asio, program_options, system). Lua 5.4.4 is embedded in `libs/`.

```bash
mkdir build && cd build
cmake ..
make
```

Run the bundled echo demo:
```bash
build/LuaServer --port 9876 --script test.lua
```

Test with netcat:
```bash
nc localhost 9876
```

## Architecture

### Execution Model

- **Single-threaded**: Boost Asio `io_service` runs everything on one thread. Lua callbacks must return quickly.
- **Per-session Lua state**: Each TCP connection instantiates its own `CLuaInterpreter` (its own `lua_State*`). Scripts are reloaded from disk for every new connection — there is no shared Lua state between sessions.
- **Pre-allocation pattern**: `server::start_accept()` creates a new `session` (loading the Lua file) *before* `async_accept` fires, so every pending accept already has a Lua interpreter ready.

### Lua Callback Protocol

The Lua script must define these global functions:

| Function | Called when | Args |
|---|---|---|
| `handle_connect(session_id)` | Client connects | Light userdata pointer to `session` |
| `handle_recv(data)` -> `string` | Data arrives | Raw bytes as string; return value is written back to socket |
| `handle_disconnect(session_id)` | Client disconnects | Light userdata pointer to `session` |

`handle_recv` must return a string. An empty return suppresses the write-back.

### Registering C++ Functions into Lua

Functions follow `lua_CFunction` signature (`int f(lua_State*)`). They are registered in `session::handle_accepted()` via `interpreter_.register_function(name, f)`.

To retrieve the `session*` inside a C function, use the light-userdata mechanism:

```cpp
// In C++ (registration side, server.cpp):
set_lightuserdata(interpreter_.getState(), api_session_pointer, this);
interpreter_.register_function("funky", funkyfunction);

// Inside the lua_CFunction:
session* s = reinterpret_cast<session*>(get_lightuserdata(L, api_session_pointer));
```

`api_session_pointer` is a random string key (`"yhRI3OnFVRuq2JRh"`) used to store the pointer in Lua's global table without namespace collisions.

### Lua C++ Binding Utilities (`lua_utils`)

**`CLuaCall`** — calls a Lua global function from C++:
```cpp
CLuaCall call(interpreter_, "handle_recv");
call << data;          // push args with <<
call.call(1);          // invoke, specify expected result count
call >> response;      // pop results with >> (LIFO order)
```

**`CLuaOpt`** — reads arguments inside a `lua_CFunction` (Lua→C direction):
```cpp
int my_cfunc(lua_State* L) {
    std::string s;
    CLuaOpt(L, 1) >> s;   // expects 1 arg, reads as string
    return 0;
}
```

**`CLuaInterpreter`** — manages a `lua_State` lifetime (RAII). Implicit conversion to `lua_State*` is provided.

### Session Lifecycle and Memory

Sessions are owned by `server::sessions_` as `shared_ptr<session>`. When a disconnect or error is detected in an async handler, removal is `post()`-ed to the io_service rather than done inline, to avoid deleting `this` mid-callback:

```cpp
io_service_.post(boost::bind(&server::session_erase, server_, this));
```

`session_erase` calls `handle_disconnect()` on the Lua side before erasing.

## Known Issues / Code Quirks

- `main.cpp` exists but is **not compiled** — the CMakeLists.txt build target only includes `Application.cpp`, `lua_utils.cpp`, and `server.cpp`. `Application.cpp` contains its own `main()`. `main.cpp` is dead code.
- `Application.hpp` declares `void initialise()` but no implementation exists. The method is not called in the compiled `Application.cpp::main()`.
- `CLuaInterpreter::register_function(module_name, function_name, f)` (two-name overload) is declared in the header but unimplemented — it's a TODO stub.
- `CLuaCall::call()` silences `LUA_ERRRUN` (only reports, does not throw). Runtime Lua errors print to stderr but do not terminate the session.

## CI

GitHub Actions (`.github/workflows/cmake.yml`) runs on push/PR to `master`. It only builds — the `ctest` step is commented out. There is no automated test suite.
