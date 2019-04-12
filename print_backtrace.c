/* See LICENSE file for copyright and license details. */
#include "internal.h"

#include <elfutils/libdwfl.h>


void
liberror_print_backtrace(struct liberror_error *error, FILE *fp, const char *indent)
{
	struct liberror_backtrace *backtrace;
	int saved_errno = errno, full_paths, lineno;
	char *debuginfo_path = NULL, *env;
	Dwarf_Addr ip;
	Dwfl_Callbacks callbacks;
	Dwfl *dwfl = NULL;
	Dwfl_Line *line = NULL;
	Dwfl_Module *module = NULL;
	const char *filename = NULL, *p;
	const char *funcname = NULL;
	size_t i;

	if (!error)
		return;

	backtrace = error->backtrace;
	if (!backtrace)
		return;

	memset(&callbacks, 0, sizeof(callbacks));

	callbacks.find_elf       = dwfl_linux_proc_find_elf;
	callbacks.find_debuginfo = dwfl_standard_find_debuginfo;
	callbacks.debuginfo_path = &debuginfo_path;

	env = getenv("LIBERROR_FULL_PATHS");
	full_paths = env && (*env == 'Y' || *env == 'y' || *env == '1');

	dwfl = dwfl_begin(&callbacks);

	if (dwfl) {
		if (dwfl_linux_proc_report(dwfl, getpid()) ||
		    dwfl_report_end(dwfl, NULL, NULL)) {
			dwfl_end(dwfl);
			dwfl = NULL;
		}
	}

	for (i = 0; i < backtrace->n; i++) {
		ip = (Dwarf_Addr)backtrace->rips[i];

		if (dwfl) {
			module   = dwfl_addrmodule(dwfl, ip);
			funcname = module ? dwfl_module_addrname(module, ip) : NULL;
			line     = dwfl_getsrc(dwfl, ip);
			if (line) {
				filename = dwfl_lineinfo(line, &(Dwarf_Addr){0}, &lineno, NULL, NULL, NULL);
				if (!full_paths && (p = strrchr(filename, '/')))
					filename = &p[1];
			}
		}

		fprintf(fp, "%s%s 0x%016"PRIxPTR": %s",
		        indent, !i ? "at" : "by",
			(uintptr_t)ip,
		        funcname ? funcname : "???");
		if (line)
			fprintf(fp, " (%s:%i)\n", filename, lineno);
		else
			fprintf(fp, "\n");
	}

	if (dwfl)
		dwfl_end(dwfl);
	errno = saved_errno;
}
