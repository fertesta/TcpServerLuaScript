cmake_minimum_required (VERSION 3.20)
project (LuaServer)

# The version number.
set (LuaServer_VERSION_MAJOR 0)
set (LuaServer_VERSION_MINOR 1)

# Enable c++11 standard
set (CMAKE_CXX_STANDARD 11)
set (LUA_DIR libs/lua-5.4.4)
set (LUA_SRC libs/lua-5.4.4/src)

find_package(Boost REQUIRED COMPONENTS program_options system)

include_directories(${PROJECT_SOURCE_DIR}/${LUA_SRC})

configure_file (
  LuaServerConfig.h.in
  "${PROJECT_SOURCE_DIR}/LuaServerConfig.h"
  )

# add the executable
add_executable(LuaServer
  Application.cpp
  lua_utils.cpp
  server.cpp
)

add_library(lua STATIC IMPORTED)
set_target_properties(lua PROPERTIES IMPORTED_LOCATION ${LUA_SRC})

target_link_libraries (${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/${LUA_SRC}/liblua.a
  boost_system
  pthread
  dl
  boost_program_options
)
