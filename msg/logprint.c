/*
 ============================================================================
 Name        : logprint.c
 Author      : junma
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "error.h"

#define M_FATAL           (1<<4)
int main(void)
{
	printf("!!!Hello World!!!\n"); /* prints !!!Hello World!!! */
	error_reset ();
	msg (M_DEBUG,"**************");
	ASSERT(1<0);
	return EXIT_SUCCESS;
}
