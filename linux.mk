LIBEXT = so
LIBFLAGS = -shared -Wl,-soname,liberror-backtrace.$(LIBEXT).$(LIB_MAJOR)

LIBMAJOREXT = $(LIBEXT).$(LIB_MAJOR)
LIBMINOREXT = $(LIBEXT).$(LIB_VERSION)
