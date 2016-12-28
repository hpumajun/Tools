/*
 * interge.h
 *
 *  Created on: Dec 21, 2016
 *      Author: junma
 */

#ifndef INTERGE_H_
#define INTERGE_H_

#include "error.h"
#define M_FATAL           (1<<4)
/*
 * min/max functions
 */

static inline int
max_int (int x, int y)
{
  if (x > y)
    return x;
  else
    return y;
}

static inline int
min_int (int x, int y)
{
  if (x < y)
    return x;
  else
    return y;
}

static inline int
constrain_int (int x, int min, int max)
{
  if (min > max)
    return min;
  if (x < min)
    return min;
  else if (x > max)
    return max;
  else
    return x;
}

/*
 * Functions used for circular buffer index arithmetic.
 */



#endif /* INTERGE_H_ */
