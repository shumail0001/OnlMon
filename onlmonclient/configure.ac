AC_INIT(onlmonclient,[2.00])
AC_CONFIG_SRCDIR([configure.ac])

AM_INIT_AUTOMAKE

AC_PROG_CXX(CC g++)
LT_INIT([disable-static])

dnl   no point in suppressing warnings people should 
dnl   at least see them, so here we go for gcc: -Wall
dnl if test $ac_cv_prog_gxx = yes; then
  CXXFLAGS="$CXXFLAGS -Wno-deprecated-declarations -Wall -Werror -Wextra -Wshadow -Woverloaded-virtual"
dnl  CXXFLAGS="-g -Wall"
dnl fi

AC_OUTPUT(Makefile)
