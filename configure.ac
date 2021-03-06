#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rct_payload], [2.0], [nthui@eng.ucsd.edu])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])
AC_LANG([C++])
AC_PROG_CXX

# Checks for python.
AM_PATH_PYTHON(2.7)
AC_PATH_PROGS(BASH, bash sh)

# Checks for programs.
AC_PROG_SED
AC_CHECK_PROG(PDFLATEX, pdflatex, pdflatex)
if test -z "$PDFLATEX"; then
	AC_MSG_WARN([Unable to create user manuals!])
fi
AC_CHECK_PROG(LUALATEX, lualatex, lualatex)
if test -z "$LUALATEX"; then
	AC_MSG_WARN([Unable to create diagrams!])
fi
AC_CHECK_PROG(PDF2SVG, pdf2svg, pdf2svg)
if test -z "$PDF2SVG"; then
	AC_MSG_WARN([Unable to create SVGs!])
fi
AM_CONDITIONAL([HAVE_PDFLATEX], test -n "$PDFLATEX")
AM_CONDITIONAL([HAVE_LUALATEX], test -n "$LUALATEX")
AM_CONDITIONAL([HAVE_PDF2SVG], test -n "$PDF2SVG")
AC_CHECK_PROG(MAKEGLOSSARIES, makeglossaries, makeglossaries)

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi

AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/Doxyfile])])
AM_COND_IF([HAVE_DOXYGEN], [DX_INIT_DOXYGEN([Radio Telemetry Tracker], doc/Doxyfile)])


# Checks for libraries.
PKG_CHECK_MODULES([uhd], [uhd])
AX_BOOST_BASE([1.50])
AX_BOOST_SYSTEM
AX_BOOST_THREAD
AX_BOOST_PROGRAM_OPTIONS

# Checks for Python libraries
# AC_PYTHON_MODULE([mraa], [fatal])

AC_PYTHON3_MODULE([subprocess], [fatal])
AC_PYTHON3_MODULE([os], [fatal])
AC_PYTHON3_MODULE([threading], [fatal])
AC_PYTHON3_MODULE([signal], [fatal])
AC_PYTHON3_MODULE([enum], [fatal])
AC_PYTHON3_MODULE([time], [fatal])
AC_PYTHON3_MODULE([mmap], [fatal])
AC_PYTHON3_MODULE([sys], [fatal])
AC_PYTHON3_MODULE([calendar], [fatal])
AC_PYTHON3_MODULE([math], [fatal])

# Checks for header files.
AC_CHECK_HEADERS([sys/time.h syslog.h signal.h error.h uhd.h getopt.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
LIBS="-lfftw3 -lfftw3_threads -lm -lutil -lboost_date_time"

# Check for and configure dlib
#AC_ARG_VAR([DLIB_INCLUDEDIR], [contains the include path to the DLIB headers, typically here /home/ntlhui/workspace/dlib/dlib-19.16/dlib])
#AC_MSG_CHECKING([whether dlib headers are available])
#DLIB_CPPFLAGS="-I$DLIB_INCLUDEDIR -DDLIB_NO_GUI_SUPPORT -O2"
#AC_SUBST([DLIB_CPPFLAGS])
#DLIB_LDFLAGS="-DDLIB_NO_GUI_SUPPORT -lpthread"
#AC_SUBST([DLIB_LDFLAGS])
#save_cppflags="$CPPFLAGS"
#CPPFLAGS="-std=c++11 $DLIB_CPPFLAGS $DLIB_LDFLAGS"
#AC_LINK_IFELSE([
#    AC_LANG_SOURCE(
#        [[
#        #include <dlib/optimization.h>
#        #include <dlib/global_optimization.h>
#        int main() { dlib::matrix<double,0,1> column_vector;; return 0; }
#        ]]
#    )],
#    [AC_MSG_RESULT([yes])],
#    [AC_MSG_WARN([could not find/link dlib headers! If the dlib headers are not located within the compiler's default search path, specify them with the DLIB_INCLUDEDIR variable to the configure script.])]
#)
#CPPFLAGS="$save_cppflags"

AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 scripts/Makefile
                 scripts/autostart/Makefile
                 src/Makefile
                 src/sdr_record/Makefile
                 tests/Makefile
                 tests/sdr_record/Makefile])
AC_OUTPUT
