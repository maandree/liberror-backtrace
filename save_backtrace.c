/* See LICENSE file for copyright and license details. */
#include "internal.h"

#define UNW_LOCAL_ONLY
#include <libunwind.h>


struct partial {
	uintptr_t rips[8];
	struct partial *next;
};


int
liberror_save_backtrace(struct liberror_error *error)
{
	struct liberror_backtrace *backtrace = NULL;
	unw_word_t rip;
	unw_cursor_t cursor;
	unw_context_t context;
	size_t i = 0;
	struct partial *current, head;
	int saved_errno = errno;
	int ret = -1;

	if (unw_getcontext(&context))
		goto out;
	if (unw_init_local(&cursor, &context))
		goto out;

	current = &head;

	for (; unw_step(&cursor) > 0; i++) {
		if (!(i & 7))
			current = current->next = alloca(sizeof(*current));
		if (unw_get_reg(&cursor, UNW_REG_IP, &rip))
			goto out;
		current->rips[i & 7] = (uintptr_t)rip;
	}

	backtrace = malloc(offsetof(struct liberror_backtrace, rips) + i * sizeof(*backtrace->rips));
	if (!backtrace)
		goto out;
	backtrace->refcount = 1;

	backtrace->n = i;
	current = &head;
	for (i = 0; i < backtrace->n; i++) {
		if (!(i & 7))
			current = current->next;
		backtrace->rips[i] = current->rips[i & 7];
	}

	ret = 0;
out:
	if (error) {
		if (error->backtrace && !--error->backtrace->refcount)
			free(error->backtrace);
		error->backtrace = backtrace;
	} else {
		if (liberror_saved_backtrace && !--liberror_saved_backtrace->refcount)
			free(liberror_saved_backtrace);
		liberror_saved_backtrace = backtrace;
	}
	errno = saved_errno;
	return ret;
}
