
## First rough version of a test script
##
## Assumes that
##  a) PostgreSQL is running, and
##  b) the current user can connect
## both of which are not viable for release but suitable while we test
##
## Dirk Eddelbuettel, 02 Jul 2008
##                    21 Oct 2008   make conditional on environment variables

## only run this if this env.var is set correctly
if (Sys.getenv("POSTGRES_USER") != "" & Sys.getenv("POSTGRES_HOST") != "" & Sys.getenv("POSTGRES_DATABASE") != "") {

    ## try to load our module and abort if this fails
    stopifnot(require(RPostgreSQL))

    ## load the PostgresSQL driver
    drv <- dbDriver("PostgreSQL")
    ## can't print result as it contains process id which changes  print(summary(drv))

    ## connect to the default db
    con <- dbConnect(drv,
                     user=Sys.getenv("POSTGRES_USER"),
                     password=Sys.getenv("POSTGRES_PASSWD"),
                     host=Sys.getenv("POSTGRES_HOST"),
                     dbname=Sys.getenv("POSTGRES_DATABASE"))

    ## run a simple query and show the query result
    res <- dbGetQuery(con, paste("select datname,encoding,datallowconn from pg_database",
                                 "where datname like 'template%' order by datname"))
    print(res)

    ## and disconnect
    dbDisconnect(con)
}
