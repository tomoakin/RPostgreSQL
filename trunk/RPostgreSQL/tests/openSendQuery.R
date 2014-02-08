## open send query test
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

    ## create two independent connections
    con <- dbConnect(drv,
                     user=Sys.getenv("POSTGRES_USER"),
                     password=Sys.getenv("POSTGRES_PASSWD"),
                     host=Sys.getenv("POSTGRES_HOST"),
                     dbname=Sys.getenv("POSTGRES_DATABASE"),
                     port=ifelse((p<-Sys.getenv("POSTGRES_PORT"))!="", p, 5432))

    if (dbExistsTable(con, "tmptest")) {
        cat("Removing tmptest\n")
        dbRemoveTable(con, "tmptest")
    }
# create temporary table (as a copy of any existing one)
#    dbGetQuery(con, "BEGIN TRANSACTION")
    cat("create temp table tmptest with dbGetQuery\n")
    dbGetQuery(con, "CREATE TEMP TABLE tmptest (f1 int)")

# query temp table
    rs<-dbGetQuery(con, "SELECT * from tmptest")
    print(rs)
    
    cat("create temp table tmptest with dbSendQuery\n")
    dbSendQuery(con, "CREATE TEMP TABLE tmptest2(f2 int)")

# query temp table
    rs2<-dbGetQuery(con, "SELECT * from tmptest2")
    print(rs2)
    ## and disconnect
    dbDisconnect(con)

    con <- dbConnect(drv,
                     user=Sys.getenv("POSTGRES_USER"),
                     password=Sys.getenv("POSTGRES_PASSWD"),
                     host=Sys.getenv("POSTGRES_HOST"),
                     dbname=Sys.getenv("POSTGRES_DATABASE"),
                     port=ifelse((p<-Sys.getenv("POSTGRES_PORT"))!="", p, 5432))

    if (dbExistsTable(con, "tmptest")) {
        cat("FAIL tmptest persisted after disconnection\n")
        cat("Removing tmptest\n")
        dbRemoveTable(con, "tmptest")
    }else{
        cat("PASS tmptest disappeared after disconnection\n")
    }
    dbDisconnect(con)
}else{
    cat("Skip.\n")
}
