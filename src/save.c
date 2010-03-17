/*
* save.c
* Lua-amf Lua module code
*/

#include "luaheaders.h"
#include "lua_amf.h"
#include "saveload.h"

int lua_amf_save(lua_State * L)
{
  unsigned char num_to_save = 0;
  int result = LUA_AMF_ESUCCESS;
  int base = lua_gettop(L);
  int length = 0;
  unsigned char * sb;

  {
  //  void * alloc_ud = NULL;
  //  lua_Alloc alloc_fn = lua_getallocf(L, &alloc_ud);
  //  lbsSB_init(&sb, alloc_fn, alloc_ud);
  }

  switch (lua_type(L, 1))
  {
  case LUA_TNIL:
    result = LUA_AMF_ESUCCESS;
    sb = (char*) malloc(sizeof(char));
    length = 1;
    sb[0] = 1;
    break;

  case LUA_TBOOLEAN:
    result = LUA_AMF_ESUCCESS;
    sb = (char*) malloc(sizeof(char));
    length = 1;
    sb[0] = lua_toboolean(L, 1) ? 3 : 2;
    break;

  case LUA_TNUMBER:
    result = LUA_AMF_ESUCCESS;
    sb = (char*) malloc(sizeof(char) * 9);
    length = 9;
    sb[0] = 5;
    //lbs_writeNumber(sb, lua_tonumber(L, index));
    break;

  case LUA_TSTRING:
    {
      size_t len = 0;
      const char * buf = lua_tolstring(L, 1, &len);
      result = LUA_AMF_ESUCCESS;
      sb = (char*) malloc(sizeof(char) * sizeof(buf));
      length = len + 2;
      sb[0] = 6;
      //result = lbs_writeString(sb, buf, len);
    }
    break;

  case LUA_TTABLE:
  case LUA_TNONE:
  case LUA_TFUNCTION:
  case LUA_TTHREAD:
  case LUA_TUSERDATA:
  default:
    result = LUA_AMF_EBADTYPE;
  }

  if (result != LUA_AMF_ESUCCESS)
  {
    switch (result)
    {
    case LUA_AMF_EBADTYPE:
      lua_pushliteral(L, "can't save: unsupported type detected");
      break;

    case LUA_AMF_ETOODEEP:
      lua_pushliteral(L, "can't save: nesting is too deep");
      break;

    case LUA_AMF_ETOOLONG:
      lua_pushliteral(L, "can't save: not enough memory");
      break;

    default: // Should not happen
      lua_pushliteral(L, "save failed");
      break;
    }

    //lbsSB_destroy(&sb);
    free(sb);

    return result;
  }

  {
   // size_t len = 0UL;
   // const unsigned char * buf = lbsSB_buffer(&sb, &len);
   // const unsigned char buf[] = "\x04";
    int i;
    for (i = 0; i < length; i ++) {
      printf("\\%03d", sb[i]);
    }
    lua_pushlstring(L, (const char *)sb, length);
   // lbsSB_destroy(&sb);
  }
  free(sb);

  return LUA_AMF_ESUCCESS;
}
