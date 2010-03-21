/*
* savebuffer.h: Lua-AMF save buffer
* Copyright (c) 2010, lua-amf authors
* See copyright information in the COPYRIGHT file
*/

#ifndef LUAAMF_SAVEBUFFER_H_
#define LUAAMF_SAVEBUFFER_H_

typedef struct luaamf_SaveBuffer
{
  lua_Alloc alloc_fn;
  void * alloc_ud;

  unsigned char * buffer;
  size_t buf_size;
  size_t end;

} luaamf_SaveBuffer;

void sb_init(
    luaamf_SaveBuffer * sb,
    lua_Alloc alloc_fn,
    void * alloc_ud
  );

/*
* Ensures that there is at least delta size available in buffer.
* New size is aligned by blockSize increments.
* Returns non-zero if resize failed.
* If you pre-sized the buffer, subsequent writes up to the new size
* are guaranteed to not fail.
*/
int sb_grow(luaamf_SaveBuffer * sb, size_t delta);

/*
* Returns non-zero if write failed.
* Allocates buffer as needed.
*/
int sb_write(
    luaamf_SaveBuffer * sb,
    const unsigned char * bytes,
    size_t length
  );

/*
* Returns non-zero if write failed.
* Allocates buffer as needed.
* Convenience function.
*/
int sb_writechar(
    luaamf_SaveBuffer * sb,
    unsigned char byte
  );

#define sb_length(sb) ( (sb)->end )

/*
* If offset is greater than total length, data is appended to the end.
* Returns non-zero if write failed.
* Allocates buffer as needed.
*/
int sb_overwrite(
    luaamf_SaveBuffer * sb,
    size_t offset,
    const unsigned char * bytes,
    size_t length
  );

/*
* If offset is greater than total length, data is appended to the end.
* Returns non-zero if write failed.
* Allocates buffer as needed.
* Convenience function.
*/
int sb_overwritechar(
    luaamf_SaveBuffer * sb,
    size_t offset,
    unsigned char byte
  );

/*
* Returns a pointer to the internal buffer with data.
* Note that buffer is NOT zero-terminated.
* Buffer is valid until next operation with the given sb.
*/
const unsigned char * sb_buffer(luaamf_SaveBuffer * sb, size_t * length);

void sb_destroy(luaamf_SaveBuffer * sb);

#endif /* LUAAMF_SAVEBUFFER_H_ */
