## zzz.R                                           Last Modified:10-08-2008 12:24:00

## This package was developed as a part of Summer of Code program organized by Google.
## Thanks to David A. James & Saikat DebRoy, the authors of RMySQL package.
## Code from RMySQL package was reused with the permission from the authors.
## Also Thanks to my GSoC mentor Dirk Eddelbuettel for helping me in the development.


# DE: is this needed:   ".conflicts.OK" <- TRUE

# DE: commenting out library() calls as DESCRIPTION takes care of it
#library(methods)
#library(DBI, warn.conflicts = FALSE)

.First.lib <- function(lib, pkg) {
    ##   library(methods)
    ##   library(DBI, warn.conflicts = FALSE)
    library.dynam("RPostgreSQL", pkg, lib)
}



