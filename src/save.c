/*
* save.c: Lua-AMF Lua module code
* Copyright (c) 2010, lua-amf authors
* See copyright information in the COPYRIGHT file
*/

#include "luaheaders.h"
#include "luaamf.h"
#include "saveload.h"
#include "savebuffer.h"
#include "encode.h"

int luaamf_save(lua_State * L)
{
  int result = LUAAMF_EFAILURE;
  luaamf_SaveBuffer sb;

  {
    void * alloc_ud = NULL;
    lua_Alloc alloc_fn = lua_getallocf(L, &alloc_ud);
    sb_init(&sb, alloc_fn, alloc_ud);
  }

  switch (lua_type(L, 1))
  {
  case LUA_TNIL:
    sb_writechar(&sb, LUAAMF_NULL);
    result = LUAAMF_ESUCCESS;
    break;

  case LUA_TBOOLEAN:
    sb_writechar(&sb, lua_toboolean(L, 1) ? LUAAMF_TRUE : LUAAMF_FALSE);
    result = LUAAMF_ESUCCESS;
    break;

  case LUA_TNUMBER:
    result = encode_double(&sb, lua_tonumber(L, 1));
    break;

  case LUA_TSTRING:
    {
      size_t len;
      const char * buf = lua_tolstring(L, 1, &len);
      result = encode_string(&sb, buf, len);
      break;
    }

  case LUA_TTABLE:
  case LUA_TNONE:
  case LUA_TFUNCTION:
  case LUA_TTHREAD:
  case LUA_TUSERDATA:
  default:
    result = LUAAMF_EBADTYPE;
  }

  if (result != LUAAMF_ESUCCESS)
  {
    switch (result)
    {
    case LUAAMF_EBADTYPE:
      lua_pushliteral(L, "can't save: unsupported type detected");
      break;

    case LUAAMF_ETOODEEP:
      lua_pushliteral(L, "can't save: nesting is too deep");
      break;

    case LUAAMF_ETOOLONG:
      lua_pushliteral(L, "can't save: not enough memory");
      break;

    default: /* Should not happen */
      lua_pushliteral(L, "save failed");
      break;
    }

    sb_destroy(&sb);

    return result;
  }
  {
    size_t len = 0UL;
    const unsigned char * buf = sb_buffer(&sb, &len);
    lua_pushlstring(L, (const char *)buf, len);
    sb_destroy(&sb);
  }

  return LUAAMF_ESUCCESS;
}
