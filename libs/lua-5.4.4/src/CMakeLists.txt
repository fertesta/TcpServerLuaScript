cmake_minimum_required(VERSION 3.20)
add_library(lua
    lapi.c      lcode.c     ldblib.c  ldump.c  linit.c   lmathlib.c  lobject.c   lparser.c  lstrlib.c  ltm.c   lundump.c   lzio.c
    lauxlib.c   lcorolib.c  ldebug.c  lfunc.c  liolib.c  lmem.c      lopcodes.c  lstate.c   ltable.c   lua.c   lutf8lib.c
    lbaselib.c  lctype.c    ldo.c     lgc.c    llex.c    loadlib.c   loslib.c    lstring.c  ltablib.c  luac.c  lvm.c
)
