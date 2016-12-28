/*
 * logtime.h
 *
 *  Created on: Dec 22, 2016
 *      Author: junma
 */

#ifndef LOGTIME_H_
#define LOGTIME_H_

#include <time.h>
#include "buffer.h"

int gettimeofday (struct timeval *tv, void *tz);
const char * time_string (time_t t, int usec, int show_usec, struct gc_arena *gc);
#endif /* LOGTIME_H_ */
