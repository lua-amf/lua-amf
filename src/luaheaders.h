/*
* luaheaders.h: C++-safe Lua headers inclusion
* Copyright (c) 2009, lua-noise authors
* See copyright information in the COPYRIGHT file
*/

#ifndef LUANOISE_LUAHEADERS_H_
#define LUANOISE_LUAHEADERS_H_

#if defined (__cplusplus) && !defined (LUANOISE_LUABUILTASCPP)
extern "C" {
#endif

#include <lua.h>
#include <lauxlib.h>
#if defined (__cplusplus) && !defined (LUANOISE_LUABUILTASCPP)
}
#endif

#endif /* LUANOISE_LUAHEADERS_H_ */
