/*
* saveload.h: Lua-AMF internal constants
* Copyright (c) 2010, lua-amf authors
* See copyright information in the COPYRIGHT file
*/

#ifndef LUAAMF_SAVELOAD_H_
#define LUAAMF_SAVELOAD_H_

/* Internal error codes */
#define LUAAMF_ESUCCESS (0)
#define LUAAMF_EFAILURE (1)
#define LUAAMF_EBADTYPE (2)
#define LUAAMF_ETOODEEP (3)
#define LUAAMF_ENOSTACK (4)
#define LUAAMF_EBADDATA (5)
#define LUAAMF_ETAILEFT (6)
#define LUAAMF_EBADSIZE (7)
#define LUAAMF_ETOOLONG (8)

#define LUAAMF_LINT      (sizeof(int))
#define LUAAMF_LSIZET    (sizeof(size_t))
#define LUAAMF_LNUMBER   (sizeof(lua_Number))

/* Type bytes */
#define UNDEFINED_AMF  0x00
#define NULL_AMF       0x01
#define FALSE_AMF      0x02
#define TRUE_AMF       0x03
#define INT_AMF        0x04
#define DOUBLE_AMF     0x05
#define STRING_AMF     0x06
#define XML_DOC_AMF    0x07
#define DATE_AMF       0x08
#define ARRAY_AMF      0x09
#define OBJECT_AMF     0x0A
#define XML_AMF        0x0B
#define BYTE_ARRAY_AMF 0x0C

#endif /* LUAAMF_SAVELOAD_H_ */
