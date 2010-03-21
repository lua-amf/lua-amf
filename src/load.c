/*
* load.c: Lua-AMF Lua module code
* Copyright (c) 2010, lua-amf authors
* See copyright information in the COPYRIGHT file
*/

#include <string.h> /* memcpy */

#include "luaheaders.h"
#include "luaamf.h"
#include "saveload.h"
#include "decode.h"

typedef struct amf_LoadState
{
  const unsigned char * pos;
  size_t unread;
} amf_LoadState;

#define ls_good(ls) \
  ((ls)->pos != NULL)

#define ls_unread(ls) \
  ((ls)->unread)

static unsigned char ls_readbyte(amf_LoadState * ls)
{
  if (ls_good(ls))
  {
    const unsigned char b = *ls->pos;
    ++ls->pos;
    --ls->unread;
    return b;
  }
  return 0;
}

static void ls_init(
    amf_LoadState * ls,
    const unsigned char * data,
    size_t len
  )
{
  ls->pos = (len > 0) ? data : NULL;
  ls->unread = len;
}

int luaamf_load(
    lua_State * L,
    const unsigned char * data,
    size_t len
  )
{
  unsigned char type;
  amf_LoadState ls;
  int result = LUAAMF_ESUCCESS;
  int base = lua_gettop(L);
  ls_init(&ls, data, len);
  if (!ls_good(&ls)) return LUAAMF_EBADDATA;
  type = ls_readbyte(&ls);

  switch (type)
  {
  case LUAAMF_NULL_AMF:
    lua_pushnil(L);
    break;

  case LUAAMF_FALSE_AMF:
    lua_pushboolean(L, 0);
    break;

  case LUAAMF_TRUE_AMF:
    lua_pushboolean(L, 1);
    break;

  case LUAAMF_INT_AMF:
    {
      int curr_value;
      lua_Number value;
      result = decode_int(ls.pos, &curr_value);
      value = curr_value;
      if (result == LUAAMF_ESUCCESS)
      {
        lua_pushnumber(L, value);
      }
    }
    break;

  case LUAAMF_DOUBLE_AMF:
    {
      double c_value;
      lua_Number value;
      result = decode_double(ls.pos, &c_value);
      value = c_value;
      if (result == LUAAMF_ESUCCESS)
      {
        lua_pushnumber(L, value);
      }
    }
    break;

  case LUAAMF_STRING_AMF:
    {
      int offset = ls.unread > 256 ? 2 : 1;
      /* TODO: think of long strings and bit masks */
      lua_pushlstring(L, (const char*)(ls.pos + offset), ls.unread - offset);
      result = LUAAMF_ESUCCESS;
    }
    break;

  default:
    result = LUAAMF_EBADDATA;
    break;
  }

  if (result == LUAAMF_ESUCCESS)
  {
    return result;
  }
  else
  {
    lua_settop(L, base); /* Discard intermediate results */
    switch (result)
    {
    case LUAAMF_EBADDATA:
      lua_pushliteral(L, "can't load: corrupt data");
      break;

    case LUAAMF_EBADSIZE:
      lua_pushliteral(L, "can't load: corrupt data, bad size");
      break;

    case LUAAMF_ETAILEFT:
      lua_pushliteral(L, "can't load: extra data at end");
      break;

    default: /* Should not happen */
      lua_pushliteral(L, "load failed");
      break;
    }
  }

  return result;
}
