/*
* savebuffer.c: Lua-AMF save buffer
* Code quoted from MIT-licensed luabins v0.2
* See copyright information in the COPYRIGHT file
*/

#include <string.h> /* memcpy() */

#include "luaheaders.h"

#include "saveload.h"
#include "savebuffer.h"

#if 0
  #define SPAM(a) printf a
#else
  #define SPAM(a) (void)0
#endif

/* Minimum allocation size */
#define LUAAMF_SAVEMINALLOC (256)

/* TODO: Test this with custom allocator! */

void sb_init(
    luaamf_SaveBuffer * sb,
    lua_Alloc alloc_fn,
    void * alloc_ud
  )
{
  sb->alloc_fn = alloc_fn;
  sb->alloc_ud = alloc_ud;

  sb->buffer = NULL;
  sb->buf_size = 0UL;

  sb->end = 0UL;
}

/*
* Ensures that there is at least delta size available in buffer.
* New size is aligned by blockSize increments
* Returns non-zero if resize failed.
* If you pre-sized the buffer, subsequent writes up to the new size
* are guaranteed to not fail.
*/
int sb_grow(luaamf_SaveBuffer * sb, size_t delta)
{
  size_t needed_size = sb->end + delta;

  if (needed_size > sb->buf_size)
  {
    /*
    * Growth factor x1.5
    * Discussion on possible values:
    * http://stackoverflow.com/questions/2269063/buffer-growth-strategy
    */

    /* TODO: Handle size_t overflow? */

    size_t new_size = 0;

    size_t add_size = sb->buf_size / 2;
    if (add_size < LUAAMF_SAVEMINALLOC)
    {
      add_size = LUAAMF_SAVEMINALLOC;
    }

    new_size = sb->buf_size + add_size;

/*
    SPAM((
        "trying %lu + %lu = %lu (needed %lu)\n",
        sb->buf_size,
        add_size,
        new_size,
        needed_size
      ));
*/

    while (new_size < needed_size)
    {
      SPAM(("...+%lu not enough, growing more\n", add_size));
      /* Grow exponentially as needed */
      add_size += new_size / 2;
      new_size += add_size;
    }

    SPAM((
        "growing from %lu to %lu (needed %lu)\n",
        sb->buf_size,
        new_size,
        needed_size
      ));

    sb->buffer = (unsigned char *)sb->alloc_fn(
        sb->alloc_ud,
        sb->buffer,
        sb->buf_size,
        new_size
      );
    if (sb->buffer == NULL)
    {
      /* TODO: We probably should free the buffer here */
      sb->buf_size = 0UL;
      sb->end = 0;
      return LUAAMF_ETOOLONG;
    }

    sb->buf_size = new_size;
  }

  return LUAAMF_ESUCCESS;
}

/*
* Returns non-zero if write failed.
* Allocates buffer as needed.
*/
int sb_write(
    luaamf_SaveBuffer * sb,
    const unsigned char * bytes,
    size_t length
  )
{
  int result = sb_grow(sb, length);
  if (result != LUAAMF_ESUCCESS)
  {
    return result;
  }

  memcpy(&sb->buffer[sb->end], bytes, length);
  sb->end += length;

  return LUAAMF_ESUCCESS;
}

/*
* Returns non-zero if write failed.
* Allocates buffer as needed.
* Convenience function.
*/
int sb_writechar(
    luaamf_SaveBuffer * sb,
    const unsigned char byte
  )
{
  /* TODO: Shouldn't this be a macro? */
  int result = sb_grow(sb, 1);
  if (result != LUAAMF_ESUCCESS)
  {
    return result;
  }

  sb->buffer[sb->end] = byte;
  sb->end++;

  return LUAAMF_ESUCCESS;
}

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
  )
{
  if (offset > sb->end)
  {
    offset = sb->end;
  }

  if (offset + length > sb->end)
  {
    int result = sb_grow(sb, length);
    if (result != LUAAMF_ESUCCESS)
    {
      return result;
    }

    sb->end = offset + length;
  }

  memcpy(&sb->buffer[offset], bytes, length);

  return LUAAMF_ESUCCESS;
}

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
  )
{
  if (offset > sb->end)
  {
    offset = sb->end;
  }

  if (offset + 1 > sb->end)
  {
    int result = sb_grow(sb, 1);
    if (result != LUAAMF_ESUCCESS)
    {
      return result;
    }

    sb->end = offset + 1;
  }

  sb->buffer[offset] = byte;

  return LUAAMF_ESUCCESS;
}

/*
* Returns a pointer to the internal buffer with data.
* Note that buffer is NOT zero-terminated.
* Buffer is valid until next operation with the given sb.
*/
const unsigned char * sb_buffer(luaamf_SaveBuffer * sb, size_t * length)
{
  if (length != NULL)
  {
    *length = sb->end;
  }
  return sb->buffer;
}

void sb_destroy(luaamf_SaveBuffer * sb)
{
  if (sb->buffer != NULL)
  {
    /* Ignoring errors */
    SPAM(("dealloc size %lu\n", sb->buf_size));
    sb->alloc_fn(sb->alloc_ud, sb->buffer, sb->buf_size, 0UL);
    sb->buffer = NULL;
    sb->buf_size = 0UL;
    sb->end = 0UL;
  }
}
