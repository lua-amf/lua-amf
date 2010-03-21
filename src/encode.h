/*
* encode.h: Lua-AMF internal constants
* Copyright (c) 2010, lua-amf authors
* See copyright information in the COPYRIGHT file
*/

#ifndef LUAAMF_ENCODE_H_
#define LUAAMF_ENCODE_H_

int encode_double(luaamf_SaveBuffer *sb, double value);
int encode_int(luaamf_SaveBuffer *sb, int value);
int encode_string(luaamf_SaveBuffer *sb, const char *value, int len);

#endif /* LUAAMF_ENCODE_H_ */
