/*
* lua-amf.h
* lua-amf -- Lua AMF Module
* See copyright notice at the end of this file.
*/

#ifndef LUA_AMF_H_
#define LUA_AMF_H_

#define LUAAMF_VERSION "0.1"

#define LUA_AMF_MAXTUPLE (250)

int lua_amf_load(
    lua_State * L,
    const unsigned char * data,
    size_t len,
    int * count
  );

int lua_amf_save(lua_State * L);

#endif /* LUA_AMF_H_ */
