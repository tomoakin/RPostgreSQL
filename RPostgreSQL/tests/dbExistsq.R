
## dbExists test with schema name: reported as Issue 3 at
## http://code.google.com/p/rpostgresql/issues/detail?id=3
## and based on an earlier email by Prasenjit Kapat
##
## Assumes that
##  a) PostgreSQL is running, and
##  b) the current user can connect
## both of which are not viable for release but suitable while we test
##
## Dirk Eddelbuettel, 10 Sep 2009

## only run this if this env.var is set correctly
if (Sys.getenv("POSTGRES_USER") != "" & Sys.getenv("POSTGRES_HOST") != "" & Sys.getenv("POSTGRES_DATABASE") != "") {

    ## try to load our module and abort if this fails
    stopifnot(require(RPostgreSQL))
    stopifnot(require(datasets))

    ## load the PostgresSQL driver
    drv <- dbDriver("PostgreSQL")

    ## connect to the default db
    con <- dbConnect(drv,
                     user=Sys.getenv("POSTGRES_USER"),
                     password=Sys.getenv("POSTGRES_PASSWD"),
                     host=Sys.getenv("POSTGRES_HOST"),
                     dbname=Sys.getenv("POSTGRES_DATABASE"),
                     port=ifelse((p<-Sys.getenv("POSTGRES_PORT"))!="", p, 5432))

    if (dbExistsTable(con, "rock'data")) {
        cat("Removing rock'data\n")
        dbRemoveTable(con, "rock'data")
    }

    cat("Write rock'data\n")
    dbWriteTable(con, "rock'data", rock)

    ## run a simple dbExists
    cat("Does rock'data exist? ")
    print(res <- dbExistsTable(con, "rock'data"))

    ## this should return TRUE but did not -- does now after patch
    cat("Does \"public.rock'data\" exist? ")
    print(res <- dbExistsTable(con, "public.rock'data"))

    ## cleanup
    if (dbExistsTable(con, "rock'data")) {
        cat("Removing rock'data\n")
        dbRemoveTable(con, "rock'data")
    }

    ## and disconnect
    dbDisconnect(con)
}
