#! /bin/bash

gcc -c -fPIC src/save.c -o obj/save.o
gcc -c -fPIC src/load.c -o obj/load.o
gcc -c -fPIC src/lua_amf.c -o obj/lua_amf.o
gcc -shared -Wl,-soname,libmean.so.1 -o lua_amf.so obj/lua_amf.o obj/load.o obj/save.o -llua

# gcc -c -fPIC src/mesa/prog_noise.c -o prog_noise.o
# gcc -c -fPIC src/simplex.c -o simplex.o
# gcc -shared -Wl,-soname,libmean.so.1 -o simplex.so prog_noise.o simplex.o -llua

