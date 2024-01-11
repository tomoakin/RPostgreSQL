# Process this file with autoconf to produce a configure script.
#
# Configure.in for RPostgreSQL
# Copyright (C) 2008 Dirk Eddelbuettel and licensed under GNU GPL
#
# This file draws heavily on configure.in files from littler, RMySQL, and RdbiPgSQL

: ${R_HOME=`R RHOME`}
if test -z "${R_HOME}"; then
  echo "could not determine R_HOME"
  exit 1
fi
CC=`"${R_HOME}/bin/R" CMD config CC`
CFLAGS=`"${R_HOME}/bin/R" CMD config CFLAGS`
CPPFLAGS=`"${R_HOME}/bin/R" CMD config CPPFLAGS`

# Set the name and version -- the version set here will propagate to other files from here
AC_INIT([RPostgreSQL],[0.7])

AC_CONFIG_AUX_DIR(src)

AC_CANONICAL_HOST
AC_CANONICAL_TARGET
case "${host_os}" in
  darwin*)
    R_OS_TYPE="darwin"
    ;;
esac


# Check for non-standard programs: pg_config(1) to configure PostgreSQL builds
AC_PATH_PROG([PG_CONFIG], [pg_config])

# By default, we will not use the accompanied libpq 
ENABLE_LIBPQ=

# If pg_config was found, let's use it
if test "${PG_CONFIG}" != ""; then
    # for debug purpose, let pg_config print all the information
    ${PG_CONFIG} | sed -e 's/^/PG_CONFIG_/' -e '/C.*FLAGS/s/-W/cflwarn/g'
    # edit the output to avoid unnecessary warning

    # Use pg_config for header and linker arguments
    PG_INCDIR=`${PG_CONFIG} --includedir`
    PG_LIBDIR=`${PG_CONFIG} --libdir`

fi
# If pg_config wasn't found, or was found but references a directory that doesn't exist,
# Let's look elsewhere

if ! test -d "${PG_INCDIR}" || ! test -d "${PG_LIBDIR}" ; then
    unset PG_INCDIR
    unset PG_LIBDIR
fi

    # let's look around -- code copied from RdbuiPgSQL but modified to use test -f
    AC_MSG_NOTICE([checking for PostgreSQL header files])
    if ! test -d "$PG_INCDIR"
    then
	for dir in \
	/usr/include \
	/usr/include/postgresql \
	/usr/local/include \
	/usr/local/include/postgresql \
	/usr/local/pgsql/include \
	/usr/local/postgresql/include 
	do
        AC_MSG_NOTICE([Checking include ${dir}.])
	    if test -f ${dir}/libpq-fe.h
	    then
	    	PG_INCDIR=${dir}
		break
	    fi
	done
    fi
	
    # likewise, let's look around for libpq.so
    if ! test $PG_LIBDIR
    then
	for dir in \
	/usr/lib \
	/usr/lib/postgresql \
	/usr/local/lib \
	/usr/local/lib/postgresql \
	/usr/local/postgresql/lib 
	do
        AC_MSG_NOTICE([Checking lib ${dir}.])
	    if test -f ${dir}/libpq.so
	    then
	    	PG_LIBDIR=${dir}
		break
	    fi
	    if test -f ${dir}/libpq.dylib
	    then
	    	PG_LIBDIR=${dir}
		break
	    fi
	done
    fi
    if test $PG_LIBDIR && test $PG_INCDIR
    then
       echo $PG_LIBDIR
       echo $PG_INCDIR
    fi
    if test "$R_OS_TYPE" = "darwin" ; then
# in case we cannot find any libpq library we will use the accompanied libpq
# This content would be written into src/Makevars at the end of this script
            DARWIN_INTERNAL_LIBPQ='

PKG_CPPFLAGS=-Ilibpq
PKG_LIBS=libpq/libpq.a

.PHONY: all
all:  $(SHLIB)
$(SHLIB): libpq/libpq.5.dylib

libpq/libpq.5.dylib:
	(cd libpq; $(MAKE) CC="$(CC)" CFLAGS="$(CFLAGS)" -f Makefile.darwin pg_config.h pg_config_os.h all)

clean:
	(cd libpq; $(MAKE) -f Makefile.darwin clean)
'
    fi 
# if darwin

# check if a program using libpq can be executed.
# int PQlibVersion(void);

## AC_CHECK_LIB(pq, PQlibVersion, ENABLE_LIBPQ="")

AC_LANG(C)
 echo '#include <libpq-fe.h>' > conftest.c
 echo 'int main(void);int main(void){printf("%i...",PQlibVersion());return 0;}' >> conftest.c
echo $CC $CFLAGS -I${PG_INCDIR} -L${PG_LIBDIR} conftest.c -lpq -o pqconftest
$CC $CFLAGS -I${PG_INCDIR} -L${PG_LIBDIR} conftest.c -lpq -o pqconftest
echo ./pqconftest
./pqconftest
status=$?
if test $status -eq 0 ; then
  echo "runs fine"
  ENABLE_LIBPQ="" 
else
  echo "system -lpq don't appear to work; use internal"
  ENABLE_LIBPQ=$DARWIN_INTERNAL_LIBPQ; 
fi
rm -f pqconftest conftest.c

# Expand into arguments

#  make sure we use the included libpq-fe.h if it was not previously found
if ! test "$ENABLE_LIBPQ" = '' ; then
	AC_MSG_NOTICE([Using internal package libpq-fe.h])
	PG_INCDIR="src/libpq"
fi

PKG_CPPFLAGS="-I${PG_INCDIR}"
PKG_LIBS="-L${PG_LIBDIR} -lpq"

# Test for sanity by looking for libpq-fe.h, no explicit action on found, error on failure
AC_CHECK_FILE(["${PG_INCDIR}/libpq-fe.h"],
	,
	AC_SUBST(R_OS_TYPE))

# Now substitute these two variable in src/Makevars.in to create src/Makevars
AC_SUBST(PKG_CPPFLAGS)
AC_SUBST(PKG_LIBS)

AC_SUBST(ENABLE_LIBPQ)
AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT
