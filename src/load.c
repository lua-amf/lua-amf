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
  int result = LUAAMF_EFAILURE;
  int base = lua_gettop(L);
  ls_init(&ls, data, len);
  if (!ls_good(&ls)) 
  {
    result = LUAAMF_EBADDATA;
  }
  else 
  {
    type = ls_readbyte(&ls);
    switch (type)
    {
    case LUAAMF_NULL_AMF:
      if(len >= 2)
      {
        result = LUAAMF_ETAILEFT;
        break;
      }
      lua_pushnil(L);
      result = LUAAMF_ESUCCESS;
      break;
  
    case LUAAMF_FALSE_AMF:
      if(len >= 2)
      {
        result = LUAAMF_ETAILEFT;
        break;
      }
      lua_pushboolean(L, 0);
      result = LUAAMF_ESUCCESS;     
      break;
  
    case LUAAMF_TRUE_AMF:
      if(len >= 2)
      {
        result = LUAAMF_ETAILEFT;
        break;
      }
      lua_pushboolean(L, 1);
      result = LUAAMF_ESUCCESS;   
      break;
  
    case LUAAMF_INT_AMF:
      if(len > 5)
      {
        result = LUAAMF_ETAILEFT;
      }
      else
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
      if(len > 9)
      {
        result = LUAAMF_ETAILEFT;
        break;
      }
      else
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
        int value = 0;
        int byte_cnt = 0;
        char byte;
        if (ls.pos == NULL) return LUAAMF_EBADDATA;
        byte = ls.pos[0];

        /* If 0x80 is set, int includes the next byte, up to 4 total bytes */
        while ((byte & 0x80) && (byte_cnt < 3)) {
            value <<= 7;
            value |= byte & 0x7F;
            byte = ls.pos[byte_cnt + 1];
            byte_cnt++;
        }
     
        /* shift bits in last byte */
        if (byte_cnt < 3) {
            /* shift by 7, since the 1st bit is reserved for next byte flag */
            value <<= 7;
            value |= byte & 0x7F;
        } else {
            /* shift by 8, since no further bytes are
               possible and 1st bit is not used for flag. */
            value <<= 8;
            value |= byte & 0xff;
        }
     
        /* Move sign bit, since we're converting 29bit->32bit */
        if (value & 0x10000000) {
            value -= 0x20000000;
        }
        if(value != ((ls.unread  - byte_cnt) * 2 - 1))
        {
          result = LUAAMF_EBADSIZE;
        }
        else
        {
          lua_pushlstring(L, (const char*)(ls.pos + byte_cnt + 1), ls.unread - byte_cnt - 1);
          result = LUAAMF_ESUCCESS;
        }
      }
      break;
    default:
      result = LUAAMF_EBADTYPE;
      break;
    }
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
    case LUAAMF_EBADTYPE:
      lua_pushliteral(L, "can't load: bad data type");
      break;
      
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
