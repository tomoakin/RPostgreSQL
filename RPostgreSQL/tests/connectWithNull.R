## connectWithNull test
##
## test for the 'Issue 2' on the Google Code issue log
## reported in April 2009, still open ?
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
    stopifnot(require(datasets))

    ## load the PostgresSQL driver
    drv <- dbDriver("PostgreSQL")

    ## connect to the default db -- replacing any of these with NULL 
    ## should be treated as the default value. 
    ## Thus, success or failure depends on the environment.
    ## Importantly, Segmentation fault should not happen.
    con <- dbConnect(drv,
                     user=Sys.getenv("POSTGRES_USER"),
                     password=Sys.getenv("POSTGRES_PASSWD"),
                     host=Sys.getenv("POSTGRES_HOST"),
                     dbname=Sys.getenv("POSTGRES_DATABASE"),
                     port=ifelse((p<-Sys.getenv("POSTGRES_PORT"))!="", p, 5432))

    ## do we get here?
    cat("Normal connection\n")
    cat("Note connection handle will change every time\n")
    print(con)
    ## and disconnect
    dbDisconnect(con)

    cat("connection with user=NULL\n")
    try({
    con <- dbConnect(drv,
                     user=NULL,
                     password=Sys.getenv("POSTGRES_PASSWD"),
                     host=Sys.getenv("POSTGRES_HOST"),
                     dbname=Sys.getenv("POSTGRES_DATABASE"),
                     port=ifelse((p<-Sys.getenv("POSTGRES_PORT"))!="", p, 5432))
    print(con)
    dbDisconnect(con)
    })
    try({
    cat("connection with password=NULL\n")
    con <- dbConnect(drv,
                     user=Sys.getenv("POSTGRES_USER"),
                     password=NULL,
                     host=Sys.getenv("POSTGRES_HOST"),
                     dbname=Sys.getenv("POSTGRES_DATABASE"),
                     port=ifelse((p<-Sys.getenv("POSTGRES_PORT"))!="", p, 5432))
    print(con)
    dbDisconnect(con)
    })
    try({
    cat("connection with host=NULL\n")
    con <- dbConnect(drv,
                     user=Sys.getenv("POSTGRES_USER"),
                     password=Sys.getenv("POSTGRES_PASSWD"),
                     host=NULL,
                     dbname=Sys.getenv("POSTGRES_DATABASE"),
                     port=ifelse((p<-Sys.getenv("POSTGRES_PORT"))!="", p, 5432))
    print(con)
    dbDisconnect(con)
    })
    try({
    cat("connection with dbname=NULL\n")
    con <- dbConnect(drv,
                     user=Sys.getenv("POSTGRES_USER"),
                     password=Sys.getenv("POSTGRES_PASSWD"),
                     host=Sys.getenv("POSTGRES_HOST"),
                     dbname=NULL,
                     port=ifelse((p<-Sys.getenv("POSTGRES_PORT"))!="", p, 5432))
    print(con)
    dbDisconnect(con)
    })
    try({
    cat("connection with port=NULL\n")
    con <- dbConnect(drv,
                     user=Sys.getenv("POSTGRES_USER"),
                     password=Sys.getenv("POSTGRES_PASSWD"),
                     host=Sys.getenv("POSTGRES_HOST"),
                     dbname=Sys.getenv("POSTGRES_DATABASE"),
                     port=NULL)
    print(con)
    ## and disconnect
    dbDisconnect(con)
    })
    cat("PASS:  reached to the end of the test code without segmentation fault\n")
}else{
    cat("Skip.\n")
}
