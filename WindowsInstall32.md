# Binary Install #

Just use the command
```
install.packages('RPostgreSQL')
```
from inside R as it is sufficient for binary install.

# Source Install (which is no longer required) #

This needs package 0.2-0 or newer, and as of 0.2-1 Windows users can get a binary package pre-made (see previous paragraph).  But should you still desire to install from source, do this:

  1. Install Rtools
> > check http://www.murdoch-sutherland.com/Rtools/
> > for the appropriate Rtools version, get it and install.
  1. Install from source package
```
  install.packages('RPostgreSQL', type='source')
```


# Historical document for compilation #

## Introduction ##

To run RPosgreSQL on windows system, you need the compiler that
was used to compile the R binary and a libpq library compiled
with the same compiler.  As the R binary distribution is compiled
with MinGW, you should use that to compile the
PostgreSQL library (libpq) with MinGW.

# Status of this Document #

This document is **historical** as we included the libpq library source in the rpostgresql development tree.

This document is **provisional draft** and there is no guarantee
whatsoever on its accuracy.  Futhermore, the content may become
incoherent with the development of the software.  (It is better
if the install become simpler so that such document is not
necessary.)

# Details #


## Current ##

## Old way ##
**How to do**

The method described below is what seems the minimal step, but there can be something lacking or unnecessary. Please let me know if you find such thing.

  * Install Rtools

> check http://www.murdoch-sutherland.com/Rtools/
> for the appropriate Rtools version, get it and install.
  * Install MinGW and MSYS
> Install MinGW and MSYS by mingw-get-inst.
> http://www.mingw.org/wiki/Getting_Started
  * Get PostgreSQL source code
> http://www.postgresql.org/
  * Configure C: drive visible in MSYS environment
> add a line "C:\  /cygdrive/c" to /etc/fstab.
> This can be done as
```
    echo C:\\ /cygdrive/c >> /etc/fstab
```
> if you don't have an appropriate editor.
  * Compile and install the postgresql
```
   cd /cygdrive/c/R
   tar jxvf /some/path/to/file/postgresql-9.1.1.tar.bz2
   cd postgresql-9.1.1
   ./configure --without-zlib
   make
   make install
```
  * copy the new libpq to an appropriate place.
```
   cp src/interfaces/libpq/libpq.dll /cygdrive/c/Program\ Files/R/R-2.14.0/bin/i386
```

  * set the PG\_HOME environment variable to C:/MinGW/MSYS/1.0/local/pgsql
  * Invoke R
  * In R environment
```
install.packages("RPostgreSQL", type="source")
```


# Note: Is it necessary to compile the library #
The libpq that comes with the
binary package of postgresql is made with a microsoft C compiler
and R distribution is prepared with a variant of MinGW.
We got a report that the library coming with the binary package
of PostgreSQL did not work on 64 bit envirionment.
Nevertheless, I also heard that the library for 32 bit envirionment
works.