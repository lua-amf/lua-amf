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

static int save_value(
    luaamf_SaveBuffer * sb,
    lua_State * L,
    int index,
    int use_code
  );

/* Returns 0 on success, non-zero on failure */
static int save_table(
    lua_State * L,
    luaamf_SaveBuffer * sb,
    int index
  )
{
  luaamf_SaveBuffer numeric;
  luaamf_SaveBuffer associated;
  int result = LUAAMF_ESUCCESS;
  int numeric_index = 1;
  int key_value_pairs_number = 0;

  {
    void * alloc_ud = NULL;
    lua_Alloc alloc_fn = lua_getallocf(L, &alloc_ud);
    sb_init(&numeric, alloc_fn, alloc_ud);
    sb_init(&associated, alloc_fn, alloc_ud);
  }

  lua_pushnil(L);
  while (result == LUAAMF_ESUCCESS && lua_next(L, index) != 0)
  {
    int value_pos = lua_gettop(L);  /* We need absolute values */
    int key_pos = value_pos - 1;

    if(lua_type(L, key_pos) == LUA_TNUMBER && lua_tonumber(L, key_pos) == (float)numeric_index)
    {
      /* Save enumerated value. */
      result = save_value(&numeric, L, value_pos, 1);
      numeric_index++;
    }
    else
    {
      /* Save associated key. */
      result = save_value(&associated, L, key_pos, 0);

      /* Save associated value. */
      if (result == LUAAMF_ESUCCESS)
      {
        result = save_value(&associated, L, value_pos, 1);
        key_value_pairs_number++;
      }
    }

    if (result == LUAAMF_ESUCCESS)
    {
      /* Remove value from stack, leave key for the next iteration. */
      lua_pop(L, 1);
    }
    else return result;
  }

  /* write serilization here */
  sb_writechar(sb, LUAAMF_ARRAY);
  encode_int(sb, 2 * key_value_pairs_number + 1);
  sb_write(sb, sb_buffer(&associated, &(associated.buf_size)), associated.buf_size);
  sb_writechar(sb, 0x001);
  sb_write(sb, sb_buffer(&numeric, &(numeric.buf_size)), numeric.buf_size);
  result = LUAAMF_ESUCCESS;

  sb_destroy(&numeric);
  sb_destroy(&associated);
  return result;
}

/* Returns LUAAMF_ESUCCESS on success, error code on failure */
static int save_value(
    luaamf_SaveBuffer * sb,
    lua_State * L,
    int index,
    int use_code
  )
{
  int result = LUAAMF_EFAILURE;

  switch (lua_type(L, index))
  {
  case LUA_TNIL:
    sb_writechar(sb, LUAAMF_NULL);
    result = LUAAMF_ESUCCESS;
    break;

  case LUA_TBOOLEAN:
    sb_writechar(sb, lua_toboolean(L, index) ? LUAAMF_TRUE : LUAAMF_FALSE);
    result = LUAAMF_ESUCCESS;
    break;

  case LUA_TNUMBER:
    result = encode_double(sb, lua_tonumber(L, index));
    break;

  case LUA_TSTRING:
    {
      size_t len;
      const char * buf = lua_tolstring(L, index, &len);
      if(use_code) sb_writechar(sb, LUAAMF_STRING);
      result = encode_string(sb, buf, len);
      break;
    }

  case LUA_TTABLE:
    result = save_table(L, sb, index);
    break;

  case LUA_TNONE:
  case LUA_TFUNCTION:
  case LUA_TTHREAD:
  case LUA_TUSERDATA:
  default:
    result = LUAAMF_EBADTYPE;
  }

  return result;
}

int luaamf_save(lua_State * L)
{
  int result = LUAAMF_EFAILURE;
  luaamf_SaveBuffer sb;

  {
    void * alloc_ud = NULL;
    lua_Alloc alloc_fn = lua_getallocf(L, &alloc_ud);
    sb_init(&sb, alloc_fn, alloc_ud);
  }

  result = save_value(&sb, L, 1, 1);

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
