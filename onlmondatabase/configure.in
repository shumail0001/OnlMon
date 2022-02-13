AC_INIT(configure.in)

AM_INIT_AUTOMAKE(ODBC, 1.00)

AC_PROG_CXX(CC g++)
AC_ENABLE_STATIC(no)
AC_PROG_LIBTOOL

dnl   no point in suppressing warnings people should 
dnl   at least see them, so here we go for gcc: -Wall
if test $ac_cv_prog_gxx = yes; then
  CXXFLAGS="$CXXFLAGS -Wno-deprecated-declarations"
fi

AC_OUTPUT(Makefile)
