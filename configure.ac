dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([adips],[1.0])


AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE()

AC_PROG_CXX

CXXFLAGS="-std=c++0x"

# AC_CANONICAL_HOST is needed to access the 'host_os' variable    
AC_CANONICAL_HOST

build_linux=no
build_windows=no
build_mac=no

# Detect the target system
case "${host_os}" in
    linux*)
        build_linux=yes
        ;;
    cygwin*|mingw*)
        build_windows=yes
        ;;
    darwin*)
        build_mac=yes
        ;;
    *)
        AC_MSG_ERROR(["OS $host_os is not supported"])
        ;;
esac

# Pass the conditionals to automake
AM_CONDITIONAL([LINUX], [test "$build_linux" = "yes"])
AM_CONDITIONAL([WINDOWS], [test "$build_windows" = "yes"])
AM_CONDITIONAL([OSX], [test "$build_mac" = "yes"])

AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
               [enable debugging, default: no]),
[case "${enableval}" in
             yes) debug=true ;;
             no)  debug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],
[debug=false])

AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")


AC_CHECK_HEADERS([sys/param.h unistd.h strings.h])
AC_CHECK_FUNCS([getcwd mkdir bzero strstr memset sqrt])
AC_CHECK_HEADER_STDBOOL
AC_FUNC_FORK
AC_TYPE_PID_T

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT

