.POSIX:

CONFIGFILE = config.mk
include $(CONFIGFILE)

OS = linux
# linux = Linux
# macos = Mac OS
include $(OS).mk

LIB_MAJOR = 1
LIB_MINOR = 0
LIB_VERSION = $(LIB_MAJOR).$(LIB_MINOR)


HDR =\
	internal.h

OBJ =\
	print_backtrace.o\
	save_backtrace.o

LOBJ = $(OBJ:.o=.lo)


all: liberror-backtrace.a liberror-backtrace.$(LIBEXT)
$(OBJ): $(@:.o=.c) $(HDR)
$(LOBJ): $(@:.lo=.c) $(HDR)

liberror-backtrace.a: $(OBJ)
	-rm -f -- $@
	$(AR) rc $@ $(OBJ)
	$(AR) s $@

liberror-backtrace.$(LIBEXT): $(LOBJ)
	$(CC) $(LIBFLAGS) -o $@ $(LOBJ) $(LDFLAGS)

.c.o:
	$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

.c.lo:
	$(CC) -c -o $@ $< -fPIC $(CFLAGS) $(CPPFLAGS)

install: liberror-backtrace.a liberror-backtrace.$(LIBEXT)
	mkdir -p -- "$(DESTDIR)$(PREFIX)/lib"
	mkdir -p -- "$(DESTDIR)$(PREFIX)/share/licenses/liberror-backtrace"
	cp -- liberror-backtrace.a "$(DESTDIR)$(PREFIX)/lib"
	cp -- liberror-backtrace.$(LIBEXT) "$(DESTDIR)$(PREFIX)/lib/liberror-backtrace.$(LIBMINOREXT)"
	ln -sf -- liberror-backtrace.$(LIBMINOREXT) "$(DESTDIR)$(PREFIX)/lib/liberror-backtrace.$(LIBMAJOREXT)"
	ln -sf -- liberror-backtrace.$(LIBMINOREXT) "$(DESTDIR)$(PREFIX)/lib/liberror-backtrace.$(LIBEXT)"
	cp -- LICENSE "$(DESTDIR)$(PREFIX)/share/licenses/liberror-backtrace"

uninstall:
	-rm -f -- "$(DESTDIR)$(PREFIX)/lib/liberror-backtrace."*
	-rm -rf -- "$(DESTDIR)$(PREFIX)/share/licenses/liberror-backtrace"

clean:
	-rm -f -- *.o *.lo *.a *.so *.so.* *.su *.test

.SUFFIXES:
.SUFFIXES: .c .o .lo .a

.PHONY: all install uninstall clean
