/*
* load.c: Lua-AMF Lua module code
* Copyright (c) 2010, lua-noise authors
* See copyright information in the COPYRIGHT file
*/

#include <string.h> /* memcpy */

#include "luaheaders.h"
#include "luaamf.h"
#include "saveload.h"

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

/* Decode a double to a native C double. */
/* Pass in reference, so we can detect an buffer error. */
static int decode_double(const unsigned char *byte_ref, double *val)
{
    /* Put bytes from byte array into double */
    union aligned {
        double d_val;
        char c_val[8];
    } d;

    if (!byte_ref) return LUAAMF_EBADDATA;

    /* Flip */
    d.c_val[0] = byte_ref[7];
    d.c_val[1] = byte_ref[6];
    d.c_val[2] = byte_ref[5];
    d.c_val[3] = byte_ref[4];
    d.c_val[4] = byte_ref[3];
    d.c_val[5] = byte_ref[2];
    d.c_val[6] = byte_ref[1];
    d.c_val[7] = byte_ref[0];

    *val = d.d_val;
    return LUAAMF_ESUCCESS;
}

/* Decode an int to a native C int. */
static int decode_int(const unsigned char *byte_ref, int *val)
{
    int result = 0;
    int byte_cnt = 0;
    char byte;
    if (!byte_ref) return LUAAMF_EBADDATA;
    byte = byte_ref[0];

    /* If 0x80 is set, int includes the next byte, up to 4 total bytes */
    while ((byte & 0x80) && (byte_cnt < 3)) {
        result <<= 7;
        result |= byte & 0x7F;
        byte = byte_ref[byte_cnt + 1];
        byte_cnt++;
    }

    /* shift bits in last byte */
    if (byte_cnt < 3) {
        /* shift by 7, since the 1st bit is reserved for next byte flag */
        result <<= 7;
        result |= byte & 0x7F;
    } else {
        /* shift by 8, since no further bytes are
           possible and 1st bit is not used for flag. */
        result <<= 8;
        result |= byte & 0xff;
    }

    /* Move sign bit, since we're converting 29bit->32bit */
    if (result & 0x10000000) {
        result -= 0x20000000;
    }

    *val = result;
    return  LUAAMF_ESUCCESS;
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
  case NULL_AMF:
    lua_pushnil(L);
    break;

  case FALSE_AMF:
    lua_pushboolean(L, 0);
    break;

  case TRUE_AMF:
    lua_pushboolean(L, 1);
    break;

  case INT_AMF:
    {
      int c_value;
      lua_Number value;
      result = decode_int(ls.pos, &c_value);
      value = c_value;
      if (result == LUAAMF_ESUCCESS)
      {
        lua_pushnumber(L, value);
      }
    }
    break;

  case DOUBLE_AMF:
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

  case STRING_AMF:
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
