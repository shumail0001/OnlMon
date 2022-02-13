AC_INIT(configure.in)

AM_INIT_AUTOMAKE(ONLMONSERVER, 1.00)

AC_PROG_CXX(CC g++)
AC_ENABLE_STATIC(no)
AC_PROG_LIBTOOL

dnl   no point in suppressing warnings people should 
dnl   at least see them, so here we go for gcc: -Wall
dnl if test $ac_cv_prog_gcc = yes; then
  CXXFLAGS="$CXXFLAGS -Wno-deprecated-declarations"
dnl   CXXFLAGS="-g -Wall"
dnl fi

AC_OUTPUT(Makefile)
