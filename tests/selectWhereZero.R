## selectWhereZero test
##
## test for the 'Issue 6' on the Google Code issue log
## Buffer overflow when numeric condition is given for a character variable
## Issue 19 is also on this test script.
## 
## This test script appeared at r111 by dirk.eddelbuettel on Oct 9, 2009
## 

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


    if (dbExistsTable(con, "tmpirisdata")) {
        print("Removing tmpirisdata\n")
        dbRemoveTable(con, "tmpirisdata")
    }

    dbWriteTable(con, "tmpirisdata", iris)

    ## run a simple query and show the query result
    cat("Testing if erroneous SQL cause normal error without segmentation fault.\n")

    res <- dbGetQuery(con, "select * from tmpirisdata where \"Species\"=0")
    print(res)

    ## cleanup
    if (dbExistsTable(con, "tmpirisdata")) {
        print("Removing tmpirisdata\n")
        dbRemoveTable(con, "tmpirisdata")
    }

    ## and disconnect
    dbDisconnect(con)
    cat("PASS:  reached to the end of the test code without segmentation fault\n")
    ## this test is success if we reach here regardless of any other message
}else{
    cat("Skip.\n")
}
