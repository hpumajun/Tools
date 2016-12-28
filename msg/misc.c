/*
 * misc.c
 *
 *  Created on: Dec 27, 2016
 *      Author: junma
 */

/* thread-safe strerror */
#include "buffer.h"
#include "error.h"
const char * strerror_ts (int errnum, struct gc_arena *gc)
{
#ifdef HAVE_STRERROR
  struct buffer out = alloc_buf_gc (256, gc);

  buf_printf (&out, "%s", openvpn_strerror (errnum, gc));
  return BSTR (&out);
#else
  return "[error string unavailable]";
#endif
}
