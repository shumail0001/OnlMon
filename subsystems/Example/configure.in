AC_INIT
AC_CONFIG_SRCDIR([configure.in])

AM_INIT_AUTOMAKE(EXAMPLEMONITOR, 1.00)

AC_PROG_CXX(CC g++)
AC_ENABLE_STATIC(no)
AC_PROG_LIBTOOL

dnl   no point in suppressing warnings people should 
dnl   at least see them, so here we go for gcc: -Wall
dnl if test $ac_cv_prog_gcc = yes; then
dnl  CXXFLAGS="$CXXFLAGS -ggdb3 -Wall -Werror"
dnl fi

AC_OUTPUT(Makefile calib/Makefile)
