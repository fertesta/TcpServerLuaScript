VPATH = src
CXX=clang -std=c++11 
CXXFLAGS = -I/usr/local/include
LDFLAGS = -L/usr/local/lib -lc++ -llua -lm -lboost_system-mt -lboost_program_options-mt

luaserver: luaserver.o LuaUtils.o Application.o


all: luaserver

clean: 
	rm -f *.o 
