## unknowntype test
##
## test for
## Issue 5 comment #12
## on the Google Code issue log
##
## Assumes that
##  a) PostgreSQL is running, and
##  b) the current user can connect
## both of which are not viable for release but suitable while we test
##
## Dirk Eddelbuettel, 03 Oct 2009

## only run this if this env.var is set correctly
if (Sys.getenv("POSTGRES_USER") != "" & Sys.getenv("POSTGRES_HOST") != "" & Sys.getenv("POSTGRES_DATABASE") != "") {

    ## try to load our module and abort if this fails
    stopifnot(require(RPostgreSQL))

    ## load the PostgresSQL driver
    drv <- dbDriver("PostgreSQL")

    ## connect to the default db
    con <- dbConnect(drv,
                     user=Sys.getenv("POSTGRES_USER"),
                     password=Sys.getenv("POSTGRES_PASSWD"),
                     host=Sys.getenv("POSTGRES_HOST"),
                     dbname=Sys.getenv("POSTGRES_DATABASE"),
                     port=ifelse((p<-Sys.getenv("POSTGRES_PORT"))!="", p, 5432))


    if (dbExistsTable(con, "tmpirisdata")) {
        print("Removing tmpirisdata\n")
        dbRemoveTable(con, "tmpirisdata")
    }


    ## run a simple query and show the query result
    res <- dbGetQuery(con, "create table tmpirisdata (ra REAL[])")
    res <- dbSendQuery(con, "select ra from tmpirisdata")
    cat("Note connection handle will change every time\n")
# connection data are variable, so don't print
#    print(res)
    type <- dbColumnInfo(res)
    print(type)
    data <- fetch(res, -1)
    print(data)

    ## cleanup
    if (dbExistsTable(con, "tmpirisdata")) {
        print("Removing tmpirisdata\n")
        dbRemoveTable(con, "tmpirisdata")
    }

    ## and disconnect
    dbDisconnect(con)
    cat("PASS:  reached to the end of the test code without segmentation fault\n")
}else{
    cat("Skip.\n")
}
