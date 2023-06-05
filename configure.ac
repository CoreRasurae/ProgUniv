#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([ProgUniv], [1.00], [BUG-REPORT-ADDRESS])
AC_CONFIG_AUX_DIR(m4)
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
#AC_CONFIG_SRCDIR([proguniv/src/ChipATMega88.cc])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

AC_MSG_CHECKING([operating system])

case $host in
*-linux*)
	AC_MSG_RESULT([Linux])
	backend="linux"
	;;
x86_64-*-mingw*)
	AC_MSG_RESULT([Windows64])
	backend="windows64"
	;;
i686-*-mingw*)
	AC_MSG_RESULT([Windows32])
	backend="windows32"
	;;

*)
#        AC_MSG_ERROR("$host: Unsupported platform")
	AC_MSG_RESULT([Host is empty, Assuming Linux platform])
	backend="linux"
	;;
esac

case $backend in
linux)
	AC_DEFINE(OS_LINUX, 1, [Linux backend])
	AC_SUBST(OS_LINUX)
	AC_SEARCH_LIBS(clock_gettime, rt, [], [], -pthread)
        AC_CHECK_LIB([pthread], pthread_create)
#	AC_CHECK_HEADERS([poll.h])
#	AC_DEFINE([POLL_NFDS_TYPE],[nfds_t],[type of second poll() argument])
	;;
windows64)
	AC_DEFINE(OS_WINDOWS64, 1, [Windows64 backend])
	AC_SUBST(OS_WINDOWS64)
#	AC_DEFINE([POLL_NFDS_TYPE],[unsigned int],[type of second poll() argument])
	;;
windows32)
	AC_DEFINE(OS_WINDOWS32, 1, [Windows32 backend])
	AC_SUBST(OS_WINDOWS32)
#	AC_DEFINE([POLL_NFDS_TYPE],[unsigned int],[type of second poll() argument])
	;;
esac

AC_SUBST(LIBS)

AM_CONDITIONAL(OS_LINUX, test "x$backend" = xlinux)
AM_CONDITIONAL(OS_WINDOWS64, test "x$backend" = xwindows64)
AM_CONDITIONAL(OS_WINDOWS32, test "x$backend" = xwindows32)

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lrt':
AC_CHECK_LIB([rt], [main])

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([clock_gettime])

AC_CONFIG_FILES([Makefile
                 proguniv/Makefile])
AC_OUTPUT
