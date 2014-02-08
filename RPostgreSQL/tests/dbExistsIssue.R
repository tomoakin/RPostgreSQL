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

    if (dbExistsTable(con, "rockdata")) {
        print("Removing rockdata\n")
        dbRemoveTable(con, "rockdata")
    }

    dbWriteTable(con, "rockdata", rock)

    ## run a simple dbExists
    cat("Does rockdata exist? ")
    print(res <- dbExistsTable(con, "rockdata"))

    ## this should return TRUE but did not -- does now after patch
    cat("Does public.rockdata exist? ")
    print(res <- dbExistsTable(con, "public.rockdata"))

    ## cleanup
    if (dbExistsTable(con, "rockdata")) {
        print("Removing rockdata\n")
        dbRemoveTable(con, "rockdata")
    }

    ## and disconnect
    dbDisconnect(con)
}else{
    cat("Skip.\n")
}
