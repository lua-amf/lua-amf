/*
* luaheaders.h: C++-safe Lua headers inclusion
* Copyright (c) 2010, lua-noise authors
* See copyright information in the COPYRIGHT file
*/

#ifndef LUAAMF_LUAHEADERS_H_
#define LUAAMF_LUAHEADERS_H_

#if defined (__cplusplus) && !defined (LUAAMF_LUABUILTASCPP)
extern "C" {
#endif

#include <lua.h>
#include <lauxlib.h>
#if defined (__cplusplus) && !defined (LUAAMF_LUABUILTASCPP)
}
#endif

#endif /* LUAAMF_LUAHEADERS_H_ */
