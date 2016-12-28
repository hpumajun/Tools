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

#ifndef SYSHEAD_H
#define SYSHEAD_H


/* branch prediction hints */

# define likely(x)      (x)
# define unlikely(x)    (x)




#include <time.h>







/*
 * Pedantic mode is meant to accomplish lint-style program checking,
 * not to build a working executable.
 */

# define PEDANTIC 0


/*
 * Do we have the capability to support the --passtos option?
 */


#define PASSTOS_CAPABILITY 0


/*
 * Do we have the capability to report extended socket errors?
 */

#define EXTENDED_SOCKET_ERROR_CAPABILITY 0


/*
 * Does this platform support linux-style IP_PKTINFO?
 */
#define ENABLE_IP_PKTINFO 0

/*
 * Disable ESEC
 */
#if 0
#undef EXTENDED_SOCKET_ERROR_CAPABILITY
#define EXTENDED_SOCKET_ERROR_CAPABILITY 0
#endif

/*
 * Do we have a syslog capability?
 */

#define SYSLOG_CAPABILITY 0


/*
 * Does this OS draw a distinction between binary and ascii files?
 */
#ifndef O_BINARY
#define O_BINARY 0
#endif

/*
 * Directory separation char
 */

#define OS_SPECIFIC_DIRSEP '/'


/*
 * Define a boolean value based
 * on Win32 status.
 */


#define WIN32_0_1 0


/*
 * Our socket descriptor type.
 */

#define SOCKET_UNDEFINED (-1)
typedef int socket_descriptor_t;


static inline int
socket_defined (const socket_descriptor_t sd)
{
  return sd != SOCKET_UNDEFINED;
}

/*
 * Should statistics counters be 64 bits?
 */
#define USE_64_BIT_COUNTERS

/*
 * Should we enable the use of execve() for calling subprocesses,
 * instead of system()?
 */
#if defined(HAVE_EXECVE) && defined(HAVE_FORK)
#define ENABLE_EXECVE
#endif

/*
 * Do we have point-to-multipoint capability?
 */

#if defined(ENABLE_CLIENT_SERVER) && defined(USE_CRYPTO) && defined(USE_SSL) && defined(HAVE_GETTIMEOFDAY)
#define P2MP 1
#else
#define P2MP 0
#endif

#if P2MP && !defined(ENABLE_CLIENT_ONLY)
#define P2MP_SERVER 1
#else
#define P2MP_SERVER 0
#endif

/*
 * HTTPS port sharing capability
 */
#if defined(ENABLE_PORT_SHARE) && P2MP_SERVER && defined(SCM_RIGHTS) && defined(HAVE_MSGHDR) && defined(HAVE_CMSGHDR) && defined(HAVE_IOVEC) && defined(CMSG_FIRSTHDR) && defined(CMSG_NXTHDR) && defined(HAVE_RECVMSG) && defined(HAVE_SENDMSG)
#define PORT_SHARE 1
#else
#define PORT_SHARE 0
#endif

/*
 * Do we have a plug-in capability?
 */
#if defined(USE_LIBDL) || defined(USE_LOAD_LIBRARY)
#define ENABLE_PLUGIN
#endif

/*
 * Enable deferred authentication?
 */
#if defined(CONFIGURE_DEF_AUTH) && P2MP_SERVER && defined(ENABLE_PLUGIN)
#define PLUGIN_DEF_AUTH
#endif
#if defined(CONFIGURE_DEF_AUTH) && P2MP_SERVER && defined(ENABLE_MANAGEMENT)
#define MANAGEMENT_DEF_AUTH
#endif
#if defined(PLUGIN_DEF_AUTH) || defined(MANAGEMENT_DEF_AUTH)
#define ENABLE_DEF_AUTH
#endif

/*
 * Enable packet filter?
 */
#if defined(CONFIGURE_PF) && P2MP_SERVER && defined(ENABLE_PLUGIN) && defined(HAVE_STAT)
#define PLUGIN_PF
#endif
#if defined(CONFIGURE_PF) && P2MP_SERVER && defined(MANAGEMENT_DEF_AUTH)
#define MANAGEMENT_PF
#endif
#if defined(PLUGIN_PF) || defined(MANAGEMENT_PF)
#define ENABLE_PF
#endif

/*
 * Do we support Unix domain sockets?
 */
#if defined(PF_UNIX) && !defined(WIN32)
#define UNIX_SOCK_SUPPORT 1
#else
#define UNIX_SOCK_SUPPORT 0
#endif

/*
 * Compile the struct buffer_list code
 */
#define ENABLE_BUFFER_LIST
/*
 * Should we include OCC (options consistency check) code?
 */
#ifndef ENABLE_SMALL
#define ENABLE_OCC
#endif

/*
 * Should we include NTLM proxy functionality
 */
#if defined(USE_CRYPTO) && defined(ENABLE_HTTP_PROXY)
#define NTLM 1
#else
#define NTLM 0
#endif

/*
 * Should we include proxy digest auth functionality
 */
#if defined(USE_CRYPTO) && defined(ENABLE_HTTP_PROXY)
#define PROXY_DIGEST_AUTH 1
#else
#define PROXY_DIGEST_AUTH 0
#endif

/*
 * Should we include code common to all proxy methods?
 */
#if defined(ENABLE_HTTP_PROXY) || defined(ENABLE_SOCKS)
#define GENERAL_PROXY_SUPPORT
#endif

/*
 * Do we have PKCS11 capability?
 */
#if defined(USE_PKCS11) && defined(USE_CRYPTO) && defined(USE_SSL)
#define ENABLE_PKCS11
#endif

/*
 * Is poll available on this platform?
 */
#if defined(HAVE_POLL) && defined(HAVE_SYS_POLL_H)
#define POLL 1
#else
#define POLL 0
#endif

/*
 * Is epoll available on this platform?
 */
#if defined(HAVE_EPOLL_CREATE) && defined(HAVE_SYS_EPOLL_H)
#define EPOLL 1
#else
#define EPOLL 0
#endif

/* Disable EPOLL */
#if 0
#undef EPOLL
#define EPOLL 0
#endif

/*
 * Should we allow ca/cert/key files to be
 * included inline, in the configuration file?
 */
#define ENABLE_INLINE_FILES 1

/*
 * Support "connection" directive
 */
#if ENABLE_INLINE_FILES
#define ENABLE_CONNECTION 1
#endif

/*
 * Should we include http proxy fallback functionality
 */
#if defined(ENABLE_CONNECTION) && defined(ENABLE_MANAGEMENT) && defined(ENABLE_HTTP_PROXY)
#define HTTP_PROXY_FALLBACK 1
#else
#define HTTP_PROXY_FALLBACK 0
#endif

/*
 * Reduce sensitivity to system clock instability
 * and backtracks.
 */
#define TIME_BACKTRACK_PROTECTION 1

/*
 * Is non-blocking connect() supported?
 */
#if defined(HAVE_GETSOCKOPT) && defined(SOL_SOCKET) && defined(SO_ERROR) && defined(EINPROGRESS) && defined(ETIMEDOUT)
#define CONNECT_NONBLOCK
#endif

/*
 * Do we have the capability to support the AUTO_USERID feature? 
 */
#if defined(ENABLE_AUTO_USERID)
#define AUTO_USERID 1
#else
#define AUTO_USERID 0
#endif

/*
 * Do we support challenge/response authentication, as a console-based client?
 */
#define ENABLE_CLIENT_CR

/*
 * Do we support pushing peer info?
 */
#if defined(USE_CRYPTO) && defined(USE_SSL)
#define ENABLE_PUSH_PEER_INFO
#endif

#endif
