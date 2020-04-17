#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.


AC_PREREQ([2.69])
AC_INIT([tt], [2.1], [zkutch@yahoo.com])
#AC_CONFIG_FILES([Makefile.my])
#AC_CONFIG_SRCDIR([moving_average_variance.c])
#AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lrt':
#AC_CHECK_LIB([rt], [main])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stdlib.h string.h sys/time.h unistd.h libintl.h locale.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_FORK
AC_CHECK_FUNCS([clock_gettime getcwd gettimeofday strerror times clock timespec_get time getrusage execve])


#   https://www.gnu.org/software/automake/manual/html_node/Usage-of-Conditionals.html
#   https://autotools.io/autoconf/arguments.html
AC_ARG_ENABLE([MEMORY], AS_HELP_STRING([--enable-MEMORY], [Puts arrays to heap]), AC_SUBST([mem], ["-DMEMORY=1"])) 


# using example: ./configure LDIR=/home/mako/work/po/ , if not defined "/usr/share/locale" will be used
AC_ARG_VAR(LDIR, Directory for i18n)
if [[ "$LDIR" != "" ]]
then 
AC_SUBST([i18ndir], ["-DLOCALEDIR=$LDIR"])
else 
AC_SUBST([i18ndir], [""])
fi

# using example: OPTS_DELIM="\&" or OPTS_DELIM='\&'  or  OPTS_DELIM=\'\&\' if not defined '@' will be used
AC_ARG_VAR(OPTS_DELIM, Delimiter for child application options, default '@')
if [[ "$OPTS_DELIM" != "" ]]
then 
AC_SUBST([opt_delim], ["-DOPTS_DELIMITER=$OPTS_DELIM"])
else 
AC_SUBST([opt_delim], [""])
fi

#AM_CONDITIONAL([CONDITIONALDIR], [test LDIR != ""])
#AM_COND_IF([CONDITIONALDIR], [AC_SUBST([i18ndir], ["-DLOCALEDIR=$LDIR"])], [AC_SUBST([i18ndir], [""])])
#AC_ARG_ENABLE([i18ndir], AS_HELP_STRING([--enable-i18ndir[=$LDIR]], [Directory for i18n]), AC_SUBST([i18ndir], ["-DLOCALEDIR=$LDIR"])) 




AC_CONFIG_FILES([Makefile])

AC_OUTPUT
