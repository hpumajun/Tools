#include "syshead.h"

#include "logtime.h"

#define VALGRIND_MAKE_READABLE(addr, len)


time_t now_in_vpn = 0;            /* GLOBAL */

#if TIME_BACKTRACK_PROTECTION && defined(HAVE_GETTIMEOFDAY)

static time_t now_adj = 0; /* GLOBAL */
time_t now_usec = 0;       /* GLOBAL */

/*
 * Try to filter out time instability caused by the system
 * clock backtracking or jumping forward.
 */

void
update_now (const time_t system_time)
{
  const int forward_threshold = 86400; /* threshold at which to dampen forward jumps */
  const int backward_trigger  = 10;    /* backward jump must be >= this many seconds before we adjust */
  time_t real_time = system_time + now_adj;

  if (real_time > now_in_vpn)
    {
      const time_t overshoot = real_time - now_in_vpn - 1;
      if (overshoot > forward_threshold && now_adj >= overshoot)
        {
          now_adj -= overshoot;
          real_time -= overshoot;
        }
      now_in_vpn = real_time;
    }
  else if (real_time < now_in_vpn - backward_trigger)
    now_adj += (now_in_vpn - real_time);
}

void
update_now_usec (struct timeval *tv)
{
  const time_t last = now_in_vpn;
  update_now (tv->tv_sec);
  if (now_in_vpn > last || (now_in_vpn == last && tv->tv_usec > now_usec))
    now_usec = tv->tv_usec;
}

#endif /* TIME_BACKTRACK_PROTECTION && defined(HAVE_GETTIMEOFDAY) */

/* format a time_t as ascii, or use current time if 0 */

const char * time_string (time_t t, int usec, int show_usec, struct gc_arena *gc)
{
    struct buffer out = alloc_buf_gc (64, gc);
    struct timeval tv;

    if (t)
    {
        tv.tv_sec = t;
        tv.tv_usec = usec;
    }
    else
    {
        #ifdef HAVE_GETTIMEOFDAY
        if (gettimeofday (&tv, NULL))
            #endif
        {
            tv.tv_sec = time (NULL);
            tv.tv_usec = 0;
        }
    }

    t = tv.tv_sec;
    buf_printf (&out, "%s", ctime(&t));
    buf_rmtail (&out, '\n');

    if (show_usec && tv.tv_usec)
        buf_printf (&out, " us=%d", (int)tv.tv_usec);

    return BSTR (&out);
}



#ifdef WIN32

static time_t gtc_base = 0;
static DWORD gtc_last = 0;
static time_t last_sec = 0;
static unsigned int last_msec = 0;
static int bt_last = false;

static void
gettimeofday_calibrate (void)
{
  const time_t t = time(NULL);
  const DWORD gtc = GetTickCount();
  gtc_base = t - gtc/1000;
  gtc_last = gtc;
}

/*
 * Rewritten by JY for OpenVPN 2.1, after I realized that
 * QueryPerformanceCounter takes nearly 2 orders of magnitude
 * more processor cycles than GetTickCount.
 */
int
gettimeofday (struct timeval *tv, void *tz)
{
  const DWORD gtc = GetTickCount();
  int bt = false;
  time_t sec;
  unsigned int msec;
  const int backtrack_hold_seconds = 10;

  /* recalibrate at the dreaded 49.7 day mark */
  if (!gtc_base || gtc < gtc_last)
    gettimeofday_calibrate ();
  gtc_last = gtc;

  sec = gtc_base + gtc / 1000;
  msec = gtc % 1000;

  if (sec == last_sec)
    {
      if (msec < last_msec)
        {
          msec = last_msec;
          bt = true;
        }
    }
  else if (sec < last_sec)
    {
      /* We try to dampen out backtracks of less than backtrack_hold_seconds.
         Larger backtracks will be passed through and dealt with by the
         TIME_BACKTRACK_PROTECTION code (if enabled) */
      if (sec > last_sec - backtrack_hold_seconds)
        {
          sec = last_sec;
          msec = last_msec;
        }
      bt = true;
    }

  tv->tv_sec = last_sec = sec;
  tv->tv_usec = (last_msec = msec) * 1000;

  if (bt && !bt_last)
    gettimeofday_calibrate ();
  bt_last = bt;

  return 0;
}

#endif /* WIN32 */

#ifdef TIME_TEST
void
time_test (void)
{
  struct timeval tv;
  time_t t;
  int i;
  for (i = 0; i < 10000; ++i)
    {
      t = time(NULL);
      gettimeofday (&tv, NULL);
#if 1
      msg (M_INFO, "t=%u s=%u us=%u",
               (unsigned int)t,
               (unsigned int)tv.tv_sec,
               (unsigned int)tv.tv_usec);
#endif
    }
}
#endif
