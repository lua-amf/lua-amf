/*
* decode.c: Lua-AMF Lua module code
* Code quoted from MIT-licensed AmFast 0.5.0 internals
* See copyright information in the COPYRIGHT file
*/

#include "saveload.h"
#include "decode.h"

/* Decode a double to a native C double. */
/* Pass in reference, so we can detect an buffer error. */
int decode_double(const unsigned char *byte_ref, double *val)
{
  /* Put bytes from byte array into double */
  union aligned {
    double d_val;
    char string[8];
  } d;

  if (!byte_ref) return LUAAMF_EBADDATA;

  /* Flip */
  d.string[0] = byte_ref[7];
  d.string[1] = byte_ref[6];
  d.string[2] = byte_ref[5];
  d.string[3] = byte_ref[4];
  d.string[4] = byte_ref[3];
  d.string[5] = byte_ref[2];
  d.string[6] = byte_ref[1];
  d.string[7] = byte_ref[0];

  *val = d.d_val;
  return LUAAMF_ESUCCESS;
}

/* Decode an int to a native C int. */
int decode_int(const unsigned char *byte_ref, int *val)
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
