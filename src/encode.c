/*
* encode.c: Lua-AMF Lua module code
* Copyright (c) 2010, lua-amf authors
* See copyright information in the COPYRIGHT file
*/

#include "luaheaders.h"
#include "saveload.h"
#include "savebuffer.h"
#include "encode.h"

int encode_double(luaamf_SaveBuffer *sb, double value)
{
   /* Put bytes from double into byte array */
   union aligned { /* use the same memory for d_value and c_value */
       double d_value;
       char c_value[8];
   } d_aligned;
   char *char_value = d_aligned.c_value;
   d_aligned.d_value = value;

   /* Flip */
   {
     int i;
     unsigned char context[9];
     context[0] = LUAAMF_DOUBLE_AMF;
     for(i = 1; i <= 8; i++) { context[i] = char_value[8 - i]; }
     sb_write(sb, context, 9);
   }
   return LUAAMF_ESUCCESS;
}

int encode_int(luaamf_SaveBuffer *sb, int value)
{
    char tmp[4];
    size_t tmp_size;

    /*
     * Int can be up to 4 bytes long.
     *
     * The first bit of the first 3 bytes
     * is set if another byte follows.
     *
     * The integer value is the last 7 bits from
     * the first 3 bytes and the 8 bits of the last byte
     * (29 bits).
     *
     * The int is negative if the 1st bit of the 29 int is set.
     */

    /* Ignore 1st 3 bits of 32 bit int, since we're encoding to 29 bit. */
    value &= 0x1fffffff;
    if (value < 0x80) {
        tmp_size = 1;
        tmp[0] = value;
    } else if (value < 0x4000) {
        tmp_size = 2;
        /* Shift bits by 7 to fill 1st byte and set next byte flag */
        tmp[0] = (value >> 7 & 0x7f) | 0x80;
        /* Shift bits by 7 to fill 2nd byte, leave next byte flag unset */
        tmp[1] = value & 0x7f;
    } else if (value < 0x200000) {
        tmp_size = 3;
        tmp[0] = (value >> 14 & 0x7f) | 0x80;
        tmp[1] = (value >> 7 & 0x7f) | 0x80;
        tmp[2] = value & 0x7f;
    } else if (value < 0x40000000) {
        tmp_size = 4;
        tmp[0] = (value >> 22 & 0x7f) | 0x80;
        tmp[1] = (value >> 15 & 0x7f) | 0x80;
        /* Shift bits by 8, since we can use all bits in the 4th byte */
        tmp[2] = (value >> 8 & 0x7f) | 0x80;
        tmp[3] = (value & 0xff);
    } else {
      return LUAAMF_EFAILURE;
    }
  sb_write(sb, (unsigned char*)tmp, tmp_size);

  return LUAAMF_ESUCCESS;
}

int encode_string(luaamf_SaveBuffer *sb, const char *value, int len)
{
  sb_writechar(sb, LUAAMF_STRING_AMF);
  encode_int(sb, len * 2 + 1);
  sb_write(sb, (unsigned char*)value, len);
  return LUAAMF_ESUCCESS;
}
