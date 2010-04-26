/*
* test.c: Lua-AMF test suite stub
* Copyright (c) 2010, lua-amf authors
* See copyright information in the COPYRIGHT file
*/

#include <stdio.h>

#include "test.h"

int main()
{
#ifdef __cplusplus
  printf("lua-amf C API test compiled as C++\n");
#else
  printf("lua-amf C API test compiled as plain C\n");
#endif /* __cplusplus */

  return 0;
}
