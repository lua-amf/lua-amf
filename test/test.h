/*
* test.h: Lua-AMF test basics
* Copyright (c) 2010, lua-amf authors
* See copyright information in the COPYRIGHT file
*/

#ifndef LUAAMF_TEST_H_
#define LUAAMF_TEST_H_

#define TEST(name, body) \
  static void name() \
  { \
    printf("---> BEGIN %s\n", #name); \
    body \
    printf("---> OK\n"); \
  }

#endif /* LUAAMF_TEST_H_ */
