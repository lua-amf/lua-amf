/*
* saveload.h
* Lua-amf internal constants
*/

#ifndef LUA_AMF_SAVELOAD_H_
#define LUA_AMF_SAVELOAD_H_

/* Find minimum of two values */
#define luabins_min(a, b) \
  ( ((a) < (b)) ? (a) : (b) )

/* Find maximum of two values */
#define luabins_max(a, b) \
  ( ((a) > (b)) ? (a) : (b) )

/* Find minimum of three values */
#define luabins_min3(a, b, c) \
  ( ((a) < (b)) ? luabins_min((a), (c)) : luabins_min((b), (c)) )

/* Internal error codes */
#define LUA_AMF_ESUCCESS (0)
#define LUA_AMF_EFAILURE (1)
#define LUA_AMF_EBADTYPE (2)
#define LUA_AMF_ETOODEEP (3)
#define LUA_AMF_ENOSTACK (4)
#define LUA_AMF_EBADDATA (5)
#define LUA_AMF_ETAILEFT (6)
#define LUA_AMF_EBADSIZE (7)
#define LUA_AMF_ETOOLONG (8)

/* Type bytes */
#define UNDEFINED_TYPE 0x00
#define NULL_TYPE 0x01
#define FALSE_TYPE 0x02
#define TRUE_TYPE 0x03
#define INT_TYPE 0x04
#define DOUBLE_TYPE 0x05
#define STRING_TYPE 0x06
#define XML_DOC_TYPE 0x07
#define DATE_TYPE 0x08
#define ARRAY_TYPE 0x09
#define OBJECT_TYPE 0x0A
#define XML_TYPE 0x0B
#define BYTE_ARRAY_TYPE 0x0C

#endif /* LUA_AMF_SAVELOAD_H_ */
