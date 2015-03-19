# Binary Install #

Just
```
install.packages('RPostgreSQL')
```
is sufficient for binary install.

# Source Install #

Now the new source package is available on CRAN (but it may take a day til the package is distributed in many mirrors).

  1. Install Rtools
> > check http://www.murdoch-sutherland.com/Rtools/
> > for the appropriate Rtools version, get it and install.
  1. Install from source package
```
  install.packages('RPostgreSQL', type='source')
```


# Historical document for compilation #

# Introduction #

To run RPosgreSQL on windows system, you need the compiler that
was used to compile the R binary and a libpq library compiled
with the same compiler.  As the R binary distribution is compiled
with MinGW, you should use that to compile the
PostgreSQL library (libpq) with MinGW.

# Status of this Document #

This document is **historical** and there is no guarantee
whatsoever on its accuracy.  Futhermore, the content may become
incoherent with the development of the software.  (It is better
if the install become simpler so that such document is not
necessary.)

# Details #

**How to do**

The method described below is what seems the minimal step, but there can be something lacking or unnecessary. Please let me know if you find such thing.

  * Install Rtools

> check http://www.murdoch-sutherland.com/Rtools/
> for the appropriate Rtools version, get it and install.
  * Install MinGW and MSYS
> Install MinGW and MSYS by mingw-get-inst.
> http://www.mingw.org/wiki/Getting_Started
  * Get PostgreSQL source code
> > http://www.postgresql.org/
  * Configure C: drive visible in MSYS environment
> > add a line "C:\  /cygdrive/c" to /etc/fstab.
  * Compile and install the postgresql
```
   cd /cygdrive/c/R64
   tar jxvf /some/path/to/file/postgresql-9.1.1.tar.bz2
   cd postgresql-9.1.1
   ./configure --host=x86_64-w64-mingw32 --without-zlib
   make
   make install
```
  * copy the new libpq to an appropriate place.
```
   cp src/interfaces/libpq/libpq.dll /cygdrive/c/Program\ Files/R/R-2.14.0/bin/x64
```

  * set the PG\_HOME environment variable to C:/MinGW/MSYS/1.0/local/pgsql
  * Invoke R x64
  * In R environment
```
install.packages("RPostgreSQL", type="source")
```


# Note: Why you need to compile #
The libpq that comes with the
binary package of postgresql is made with a microsoft C compiler
and R distribution is prepared with a variant of MinGW.
These two system are quite different and therefore copy
The method will be briefly described.