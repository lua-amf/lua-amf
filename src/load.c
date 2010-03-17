/*
* load.c
* Lua-amf Lua module code
*/

#include "luaheaders.h"
#include "lua_amf.h"
#include "saveload.h"

// Decode a double to a native C double.
// Pass in reference, so we can detect an buffer error.
static int decode_double(const unsigned char *context, double *val)
{
    if (!context) return LUA_AMF_EBADDATA;

    // Put bytes from byte array into double
    union aligned {
        double d_val;
        char c_val[8];
    } d;

    // Flip endianness
    d.c_val[0] = context[7];
    d.c_val[1] = context[6];
    d.c_val[2] = context[5];
    d.c_val[3] = context[4];
    d.c_val[4] = context[3];
    d.c_val[5] = context[2];
    d.c_val[6] = context[1];
    d.c_val[7] = context[0];

    *val = d.d_val;
    return LUA_AMF_ESUCCESS;
}

// Decode an int to a native C int.
static int decode_int(const unsigned char *byte_ref, int *val)
{
    int result = 0;
    int byte_cnt = 0;
    if (!byte_ref) return LUA_AMF_EBADDATA;
    char byte = byte_ref[0];

    // If 0x80 is set, int includes the next byte, up to 4 total bytes
    while ((byte & 0x80) && (byte_cnt < 3)) {
        result <<= 7;
        result |= byte & 0x7F;
        byte = byte_ref[byte_cnt + 1];
        byte_cnt++;
    }

    // shift bits in last byte
    if (byte_cnt < 3) {
        result <<= 7; // shift by 7, since the 1st bit is reserved for next byte flag
        result |= byte & 0x7F;
    } else {
        result <<= 8; // shift by 8, since no further bytes are possible and 1st bit is not used for flag.
        result |= byte & 0xff;
    }

    // Move sign bit, since we're converting 29bit->32bit
    if (result & 0x10000000) {
        result -= 0x20000000;
    }

    *val = result;
    return  LUA_AMF_ESUCCESS;
}

decode_string(const unsigned char *byte_ref, char *string, size_t * len)
{
  *len = 6;
  strcpy(string, "string");
  return LUA_AMF_ESUCCESS;
}

typedef struct lbs_LoadState
{
  const unsigned char * pos;
  size_t unread;
} lbs_LoadState;

static void lbsLS_init(
    lbs_LoadState * ls,
    const unsigned char * data,
    size_t len
  )
{
  ls->pos = (len > 0) ? data : NULL;
  ls->unread = len;
}

#define lbsLS_good(ls) \
  ((ls)->pos != NULL)

#define lbsLS_unread(ls) \
  ((ls)->unread)

static unsigned char lbsLS_readbyte(lbs_LoadState * ls)
{
  if (lbsLS_good(ls))
  {
    const unsigned char b = *ls->pos;
    ++ls->pos;
    --ls->unread;
    return b;
  }
  return 0;
}

int lua_amf_load(
    lua_State * L,
    const unsigned char * data,
    size_t len,
    int * count
  )
{
  lbs_LoadState ls;
  int result = LUA_AMF_ESUCCESS;
  unsigned char num_items = 0;
  int base = 0;
  int i = 0;

  base = lua_gettop(L);

  lbsLS_init(&ls, data, len);

  if (!lbsLS_good(&ls)) return LUA_AMF_EBADDATA;

  unsigned char type = lbsLS_readbyte(&ls);
  switch (type)
  {
  case NULL_TYPE:
    lua_pushnil(L);
    break;

  case FALSE_TYPE:
    lua_pushboolean(L, 0);
    break;

  case TRUE_TYPE:
    lua_pushboolean(L, 1);
    break;

  case INT_TYPE:
    {
      int c_value;
      result = decode_int(ls.pos, &c_value);
      lua_Number value = c_value;
      if (result == LUA_AMF_ESUCCESS)
      {
        lua_pushnumber(L, value);
      }
    }
    break;

  case DOUBLE_TYPE:
    {
      double c_value;
      result = decode_double(ls.pos, &c_value);
      lua_Number value = c_value;
      if (result == LUA_AMF_ESUCCESS)
      {
        lua_pushnumber(L, value);
      }
    }
    break;

  case STRING_TYPE:
    {
      lua_pushlstring(L, ls.pos + 1, ls.unread - 1);
    }
    break;

  default:
    result = LUA_AMF_EBADDATA;
    break;
  }
/*
  if (result == LUA_AMF_ESUCCESS && lbsLS_unread(&ls) > 0)
  {
    SPAM(("load: %lu chars left at tail\n", lbsLS_unread(&ls)));
    result = LUA_AMF_ETAILEFT;
  }

  if (result == LUA_AMF_ESUCCESS)
  {
    *count = num_items;
  }
  else
  {
    lua_settop(L, base); // Discard intermediate results
    switch (result)
    {
    case LUA_AMF_EBADDATA:
      lua_pushliteral(L, "can't load: corrupt data");
      break;

    case LUA_AMF_EBADSIZE:
      lua_pushliteral(L, "can't load: corrupt data, bad size");
      break;

    case LUA_AMF_ETAILEFT:
      lua_pushliteral(L, "can't load: extra data at end");
      break;

    default: // Should not happen
      lua_pushliteral(L, "load failed");
      break;
    }
  }
*/
  return result;
}
