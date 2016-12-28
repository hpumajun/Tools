/*
 *  OpenVPN -- An application to securely tunnel IP networks
 *             over a single UDP port, with support for SSL/TLS-based
 *             session authentication and key exchange,
 *             packet encryption, packet authentication, and
 *             packet compression.
 *
 *  Copyright (C) 2002-2010 OpenVPN Technologies, Inc. <sales@openvpn.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2
 *  as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program (see the file COPYING included with this
 *  distribution); if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "syshead.h"

#include "common.h"
#include "buffer.h"
#include "error.h"

void buf_size_error (const size_t size)
{
  msg (M_FATAL, "fatal buffer size error, size=%lu", (unsigned long)size);
}

struct buffer alloc_buf_gc (size_t size, struct gc_arena *gc)
{
  struct buffer buf;
  if (!buf_size_valid (size))
    buf_size_error (size);
  buf.capacity = (int)size;
  buf.offset = 0;
  buf.len = 0;

  buf.data = (uint8_t *) gc_malloc (size, false, gc);
  if (size)
    *buf.data = 0;
  return buf;
}


/*
 * printf append to a buffer with overflow check
 */
int buf_printf (struct buffer *buf, const char *format, ...)
{
    int ret = false;
    if (buf_defined (buf))
    {
        va_list arglist;
        uint8_t *ptr = BEND (buf);
        int cap = buf_forward_capacity (buf);

        if (cap > 0)
        {
            int stat;
            va_start (arglist, format);
            stat = vsnprintf ((char *)ptr, cap, format, arglist);
            va_end (arglist);
            *(buf->data + buf->capacity - 1) = 0; /* windows vsnprintf needs this */
            buf->len += (int) strlen ((char *)ptr);
            if (stat >= 0 && stat < cap)
            ret = true;
        }
    }
    return ret;
}




/*
 * This is necessary due to certain buggy implementations of snprintf,
 * that don't guarantee null termination for size > 0.
 *
 * This function is duplicated into service-win32/openvpnserv.c
 * Any modifications here should be done to the other place as well.
 */

int openvpn_snprintf(char *str, size_t size, const char *format, ...)
{
  va_list arglist;
  int ret = 0;
//  printf("m2 is %s",str);
  if (size > 0)
    {
      va_start (arglist, format);
      printf("m2 is %s",str);
      ret = vsnprintf (str, size, format, arglist);
      va_end (arglist);
      str[size - 1] = 0;
    }
  return ret;
}

void * gc_malloc (size_t size, int clear, struct gc_arena *a)
{
  void *ret;
  if (a)
    {
      struct gc_entry *e;

      e = (struct gc_entry *) malloc (size + sizeof (struct gc_entry));

      ret = (char *) e + sizeof (struct gc_entry);
      e->next = a->list;
      a->list = e;
    }
  else
    {

      ret = malloc (size);

    }
    memset (ret, 0, size);
  return ret;
}



/*
 * Allocate a string
 */
char *

string_alloc (const char *str, struct gc_arena *gc)
{
  if (str)
    {
      const int n = strlen (str) + 1;
      char *ret;

      ret = (char *) gc_malloc (n, false, gc);
      memcpy (ret, str, n);
      return ret;
    }
  else
    return NULL;
}

/*
 * Erase all characters in a string
 */
void
string_clear (char *str)
{
  if (str)
    {
      const int len = strlen (str);
      if (len > 0)
	memset (str, 0, len);
    }
}

/*
 * Return the length of a string array
 */
int
string_array_len (const char **array)
{
  int i = 0;
  if (array)
    {
      while (array[i])
        ++i;
    }
  return i;
}



int
buf_substring_len (const struct buffer *buf, int delim)
{
  int i = 0;
  struct buffer tmp = *buf;
  int c;

  while ((c = buf_read_u8 (&tmp)) >= 0)
    {
      ++i;
      if (c == delim)
	return i;
    }
  return -1;
}



/*
 * Print a string which might be NULL
 */
const char *
np (const char *str)
{
  if (str)
    return str;
  else
    return "[NULL]";
}

/*
 * Classify and mutate strings based on character types.
 */

int
char_class (const unsigned char c, const unsigned int flags)
{
  if (!flags)
    return false;
  if (flags & CC_ANY)
    return true;

  if ((flags & CC_NULL) && c == '\0')
    return true;

  if ((flags & CC_ALNUM) && isalnum (c))
    return true;
  if ((flags & CC_ALPHA) && isalpha (c))
    return true;
  if ((flags & CC_ASCII) && isascii (c))
    return true;
  if ((flags & CC_CNTRL) && iscntrl (c))
    return true;
  if ((flags & CC_DIGIT) && isdigit (c))
    return true;
  if ((flags & CC_PRINT) && isprint (c))
    return true;
  if ((flags & CC_PUNCT) && ispunct (c))
    return true;    
  if ((flags & CC_SPACE) && isspace (c))
    return true;
  if ((flags & CC_XDIGIT) && isxdigit (c))
    return true;

  if ((flags & CC_BLANK) && (c == ' ' || c == '\t'))
    return true;
  if ((flags & CC_NEWLINE) && c == '\n')
    return true;
  if ((flags & CC_CR) && c == '\r')
    return true;

  if ((flags & CC_BACKSLASH) && c == '\\')
    return true;
  if ((flags & CC_UNDERBAR) && c == '_')
    return true;
  if ((flags & CC_DASH) && c == '-')
    return true;
  if ((flags & CC_DOT) && c == '.')
    return true;
  if ((flags & CC_COMMA) && c == ',')
    return true;
  if ((flags & CC_COLON) && c == ':')
    return true;
  if ((flags & CC_SLASH) && c == '/')
    return true;
  if ((flags & CC_SINGLE_QUOTE) && c == '\'')
    return true;
  if ((flags & CC_DOUBLE_QUOTE) && c == '\"')
    return true;
  if ((flags & CC_REVERSE_QUOTE) && c == '`')
    return true;
  if ((flags & CC_AT) && c == '@')
    return true;
  if ((flags & CC_EQUAL) && c == '=')
    return true;

  return false;
}

/*
 * remove specific trailing character
 */

void buf_rmtail (struct buffer *buf, uint8_t remove)
{
  uint8_t *cp = BLAST(buf);
  if (cp && *cp == remove)
    {
      *cp = '\0';
      --buf->len;
    }
}





/*
 * struct buffer_list
 */

#ifdef ENABLE_BUFFER_LIST










#endif
