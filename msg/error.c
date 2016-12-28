/*
 *  OpenVPN -- An application to securely tunnel IP networks
 *             over a single TCP/UDP port, with support for SSL/TLS-based
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

#include "error.h"
#include "buffer.h"
#include "logtime.h"
/**misc.h include the function that translate errno to the string which explain the detail info of errno**/
#include "misc.h"
#ifdef USE_CRYPTO
#include <openssl/err.h>
#endif



#if SYSLOG_CAPABILITY
#ifndef LOG_OPENVPN
#define LOG_OPENVPN LOG_DAEMON
#endif
#endif

/* Globals */
unsigned int x_debug_level; /* GLOBAL */

/* Mute state */
static int mute_cutoff;     /* GLOBAL */
static int mute_count;      /* GLOBAL */
static int mute_category;   /* GLOBAL */

/*
 * Output mode priorities are as follows:
 *
 *  (1) --log-x overrides everything
 *  (2) syslog is used if --daemon or --inetd is defined and not --log-x
 *  (3) if OPENVPN_DEBUG_COMMAND_LINE is defined, output
 *      to constant logfile name.
 *  (4) Output to stdout.
 */

/* If true, indicates that stdin/stdout/stderr
   have been redirected due to --log */
static int std_redir;      /* GLOBAL */

/* Should messages be written to the syslog? */
static int use_syslog;     /* GLOBAL */

/* Should timestamps be included on messages to stdout/stderr? */
static int suppress_timestamps; /* GLOBAL */

/* The program name passed to syslog */
#if SYSLOG_CAPABILITY
static char *pgmname_syslog;  /* GLOBAL */
#endif

/* If non-null, messages should be written here (used for debugging only) */
static FILE *msgfp;         /* GLOBAL */

/* If true, we forked from main OpenVPN process */
static int forked;         /* GLOBAL */

/* our default output targets */
static FILE *default_out; /* GLOBAL */
static FILE *default_err; /* GLOBAL */




void
msg_forked (void)
{
  forked = true;
}

int
set_debug_level (const int level, const unsigned int flags)
{
  const int ceiling = 15;

  if (level >= 0 && level <= ceiling)
    {
      x_debug_level = level;
      return true;
    }
  else if (flags & SDL_CONSTRAIN)
    {
      x_debug_level = constrain_int (level, 0, ceiling);
      return true;
    }
  return false;
}

int
set_mute_cutoff (const int cutoff)
{
  if (cutoff >= 0)
    {
      mute_cutoff = cutoff;
      return true;
    }
  else
    return false;
}

int
get_debug_level (void)
{
  return x_debug_level;
}

int
get_mute_cutoff (void)
{
  return mute_cutoff;
}

void
set_suppress_timestamps (int suppressed)
{
  suppress_timestamps = suppressed;
}

void error_reset ()
{
  use_syslog = std_redir = false;
  suppress_timestamps = false;
  x_debug_level = 4;
  mute_cutoff = 0;
  mute_count = 0;
  mute_category = 0;
  default_out = OPENVPN_MSG_FP;
  default_err = OPENVPN_MSG_FP;

#ifdef OPENVPN_DEBUG_COMMAND_LINE
  msgfp = fopen (OPENVPN_DEBUG_FILE, "w");
  if (!msgfp)
    openvpn_exit (OPENVPN_EXIT_STATUS_CANNOT_OPEN_DEBUG_FILE); /* exit point */
#else
  msgfp = NULL;
#endif
}

void
errors_to_stderr (void)
{
  default_err = OPENVPN_ERROR_FP;  
}



#define SWAP { tmp = m1; m1 = m2; m2 = tmp; }

int x_msg_line_num; /* GLOBAL */

FILE * msg_fp(const unsigned int flags)
{
  FILE *fp = msgfp;
  if (!fp)
    fp = (flags & (M_FATAL|M_USAGE_SMALL)) ? default_err : default_out;
  return fp;
}

void x_msg (const unsigned int flags, const char *format, ...)
{
    struct gc_arena gc;
    va_list arglist;

    char *m1;
    char *m2;
    char *tmp;
    int e;
    const char *prefix;
    const char *prefix_sep;

    #ifndef HAVE_VARARG_MACROS
    /* the macro has checked this otherwise */
    if (!MSG_TEST (flags))
        return;
    #endif
    if (flags & M_ERRNO_SOCK)
        e = openvpn_errno_socket ();
    else
        e = openvpn_errno ();

    gc_init (&gc);

    m1 = (char *) gc_malloc (ERR_BUF_SIZE, false, &gc);
    m2 = (char *) gc_malloc (ERR_BUF_SIZE, false, &gc);

    va_start (arglist, format);
    vsnprintf (m1, ERR_BUF_SIZE, format, arglist);
    va_end (arglist);
    m1[ERR_BUF_SIZE - 1] = 0; /* windows vsnprintf needs this */

    if ((flags & (M_ERRNO|M_ERRNO_SOCK)) && e)
    {
    //      printf("flag is *** 0x%x ***\n",flags);
        openvpn_snprintf (m2, ERR_BUF_SIZE, "%s: %s (errno=%d)",
                          m1, strerror_ts(e, &gc), e);
        SWAP;
    //      printf("m2 is %s",m2);
    }

    if (flags & M_OPTERR)
    {
        openvpn_snprintf (m2, ERR_BUF_SIZE, "Options error: %s", m1);
        SWAP;
    }


    /* set up client prefix */
    if (flags & M_NOIPREFIX)
        prefix = NULL;
    else
        prefix = msg_get_prefix ();
    prefix_sep = " ";
    if (!prefix)
        prefix_sep = prefix = "";

    /* virtual output capability used to copy output to management subsystem */
    if (!forked)
    {
        const struct virtual_output *vo = msg_get_virtual_output ();
        if (vo)
        {
            openvpn_snprintf (m2, ERR_BUF_SIZE, "%s%s%s",
                    prefix,
                    prefix_sep,
                    m1);
        }
    }
    if (!(flags & M_MSG_VIRT_OUT))
    {
        FILE *fp = msg_fp(flags);
        const int show_usec = check_debug_level (DEBUG_LEVEL_USEC_TIME);
        //        printf("%d",show_usec);
        if ((flags & M_NOPREFIX) || suppress_timestamps)
        {
            fprintf (fp, "%s%s%s%s",
                     prefix,
                     prefix_sep,
                     m1,
                     (flags&M_NOLF) ? "" : "\n");
        }
        else
        {
            fprintf (fp, "%s %s%s%s%s",
                     time_string (0, 0, show_usec, &gc),
                     prefix,
                     prefix_sep,
                     m1,
                     (flags&M_NOLF) ? "" : "\n");
        }
        fflush(fp);
        ++x_msg_line_num;
    }

    //   printf("%s",m1);
    if (flags & M_FATAL)
    {
        msg (M_INFO, "Exiting");
    }

}

void assert_failed (const char *filename, const char *funcname, int line)
{
  msg (M_FATAL, "Assertion failed at FILE [%s] FUNC [%s]:%d", filename, funcname, line);
}


/*
 * Functions used to check return status
 * of I/O operations.
 */

unsigned int x_cs_info_level;    /* GLOBAL */
unsigned int x_cs_verbose_level; /* GLOBAL */
unsigned int x_cs_err_delay_ms;  /* GLOBAL */

void
reset_check_status ()
{
  x_cs_info_level = 0;
  x_cs_verbose_level = 0;
}

void
set_check_status (unsigned int info_level, unsigned int verbose_level)
{
  x_cs_info_level = info_level;
  x_cs_verbose_level = verbose_level;
}


/*
 * In multiclient mode, put a client-specific prefix
 * before each message.
 */
const char *x_msg_prefix; /* GLOBAL */

/*
 * Allow MSG to be redirected through a virtual_output object
 */

const struct virtual_output *x_msg_virtual_output; /* GLOBAL */

/*
 * Exiting.
 */


