cmake_minimum_required (VERSION 3.6)
project (LuaServer)

# The version number.
set (LuaServer_VERSION_MAJOR 0)
set (LuaServer_VERSION_MINOR 1)

# Enable c++11 standard
set (CMAKE_CXX_STANDARD 11)

# add the binary tree to the search path for include files
# so that we will find LuaServerConfig.h
# include_directories("${PROJECT_BINARY_DIR}/build")

configure_file (
  LuaServerConfig.h.in
  "${PROJECT_SOURCE_DIR}/LuaServerConfig.h"
  )
 
# add the executable
add_executable(LuaServer 
  luaserver.cpp
  Application.cpp
  LuaUtils.cpp
  TCPServer.cpp
)

target_link_libraries (LuaServer 
  lua
  boost_system
  boost_program_options
)

# install (TARGETS MathFunctions DESTINATION bin)
# install (FILES "${PROJECT_SOURCE_DIR}/MathFunctions/mysqrt.h" DESTINATION include)
# install (FILES MathFunctions/mysqrt.h DESTINATION include)

# add the install targets
# install (TARGETS Tutorial DESTINATION bin)
# install (FILES "${PROJECT_BINARY_DIR}/TutorialConfig.h" 
#         DESTINATION include)
