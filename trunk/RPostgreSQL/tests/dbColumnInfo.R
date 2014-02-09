## dbColumnInfo test
## Initial report was sporadic segfault (Issue 42)
## and it was found reproducile under gctorture()
## However, test with gctorture() is moved outside the package
## because its execution time is too long.
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

    ## load the PostgresSQL driver
    drv <- dbDriver("PostgreSQL")

    ## connect to the default db
    con <- dbConnect(drv,
                     user=Sys.getenv("POSTGRES_USER"),
                     password=Sys.getenv("POSTGRES_PASSWD"),
                     host=Sys.getenv("POSTGRES_HOST"),
                     dbname=Sys.getenv("POSTGRES_DATABASE"),
                     port=ifelse((p<-Sys.getenv("POSTGRES_PORT"))!="", p, 5432))


    #  create a table
    dbGetQuery(con, "set client_min_messages to 'WARNING'")
    res <- dbGetQuery(con, "CREATE TABLE aa (pk integer primary key, v1 float not null, v2 float)" )

    ## run a simple query and show the query result
    res <- dbGetQuery(con, "INSERT INTO aa VALUES(3, 2, NULL)" )
    res <- dbSendQuery(con, "select pk, v1, v2, v1+v2 from aa")
    cat("dbColumnInfo\n")
    print(dbColumnInfo(res))
    cat("SELECT result\n")
    df <- fetch(res, n=-1)
    print(df)

    ## cleanup
    cat("Removing \"AA\"\n")
    dbRemoveTable(con, "aa")
    ## and disconnect
    dbDisconnect(con)
}else{
    cat("Skip.\n")
}
