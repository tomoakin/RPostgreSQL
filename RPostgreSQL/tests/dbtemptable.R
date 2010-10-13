
## dbWriteTable test
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



    a <- dbSendQuery(con, "CREATE TABLE foo (name text)")
    b <- dbSendQuery(con, "INSERT INTO foo VALUES ('bar')")

    ## run a simple query and show the query result
    x <- dbSendQuery(con, "CREATE TEMPORARY TABLE xyz ON COMMIT DROP AS select * from foo limit 1; select * from xyz;")
    res <- fetch(x, n=-1)
    print(res)
    a <- dbSendQuery(con, "DROP TABLE foo")


    ## cleanup

    ## and disconnect
    dbDisconnect(con)
    cat("PASS -- ended without segmentation fault\n")
}
