/*
* decode.h: Lua-AMF internal constants
* Copyright (c) 2010, lua-amf authors
* See copyright information in the COPYRIGHT file
*/

#ifndef LUAAMF_DECODE_H_
#define LUAAMF_DECODE_H_

int decode_double(const unsigned char *byte_ref, double *val);
int decode_int(const unsigned char *byte_ref, int *val);

#endif /* LUAAMF_DECODE_H_ */
