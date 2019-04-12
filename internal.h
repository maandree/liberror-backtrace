/* See LICENSE file for copyright and license details. */
#include <alloca.h>
#include <errno.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


struct liberror_error {
	struct liberror_backtrace *backtrace;
};


struct liberror_backtrace {
	size_t refcount;
	size_t n;
	uintptr_t rips[];
};


void liberror_print_backtrace(struct liberror_error *, FILE *, const char *);
int liberror_save_backtrace_(struct liberror_error *);
