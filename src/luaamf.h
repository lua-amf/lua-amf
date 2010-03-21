/*
* lua-amf.h: Lua-AMF Module
* Copyright (c) 2010, lua-amf authors
* See copyright information in the COPYRIGHT file
*/

#ifndef LUAAMF_H_
#define LUAAMF_H_

#define LUAAMF_VERSION "0.1"

int luaamf_load(
    lua_State * L,
    const unsigned char * data,
    size_t len
  );

int luaamf_save(lua_State * L);

#endif /* LUAAMF_H_ */
