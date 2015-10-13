VPATH = src
CXX=clang -std=c++11
CXXFLAGS = -I/usr/local/include 
LDFLAGS = -L/usr/local/lib -lc++ -llua -lm -lev

luaserver: luaserver.o LuaUtils.o


all: luaserver

clean: 
	rm -f src/*.o 
