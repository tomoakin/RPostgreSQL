# Install RPostgreSQL on Mac OS X #

Currently some compiles from within R are broken due to the location of postgresql.  The source configure code attempts to locate the correct postgresql libraries.  If this does not work, you may need to manually compile the RPostgreSQL source.  The following example uses the fink installed location of PostgreSQL, however it will work for any PostgreSQL distribution.

  1. Download the source.
  1. Open Terminal.
  1. Move (cd) into the source location.
  1. Locate postgresql lib (libpq.a and libpq.dylib) and include (libpq-fe.h) directories
  1. Modify the following for the current RPostgreSQL and directories identified above
  1. Execute the modified command lines below:

**For Fink PostgreSQL 8.3**
```
sudo R --arch=i386 CMD INSTALL RPostgreSQL_0.1-6.tar.gz --configure-args='--with-pgsql-libraries=/sw/lib/postgresql-8.3 --with-pgsql-includes=/sw/include/postgresql'
```

If you install postgresql in a different location or use a different version you will need to modify the command line to match.

**For Fink PostgreSQL 8.4**
```
sudo R --arch=i386 CMD INSTALL RPostgreSQL_0.1-6.tar.gz --configure-args='--with-pgsql-libraries=/sw/opt/postgresql-8.4/lib --with-pgsql-includes=/sw/opt/postgresql-8.4/include'
```

If you install postgresql in a different location or use a different version you will need to modify the command line to match.

Note: The default R call compiles a 64-bit package, but installs it under 32-bit R, which results in an error when the package is loaded. For OS X Users, R needs to be called with the arch flag correctly set, as in
```
> sudo R --arch=i386 CMD INSTALL RPostgreSQL
```