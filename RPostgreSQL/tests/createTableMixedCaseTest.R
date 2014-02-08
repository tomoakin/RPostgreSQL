## createTableMixedCaseTest test
##
##
## Assumes that
##  a) PostgreSQL is running, and
##  b) the current user can connect
## both of which are not viable for release but suitable while we test
##
## Neil Tiffin, 30 Oct 2009

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
                     port=Sys.getenv("POSTGRES_PORT"))



    res <- dbGetQuery(con, "create table Foo1 (f1 int)")
    res <- dbGetQuery(con, "create table \"Foo2\" (f1 int)")

    cat("Test should create foo1 and Foo2 tables\n")
    ## res <- dbGetQuery(con, "SELECT * FROM information_schema.tables WHERE table_schema = 'public'")
    ## print res

    if (dbExistsTable(con, "Foo1")) {
        cat("FAIL - Foo1 Table exists.\n")
    }
    else {
        cat("Pass - Foo1 Table does not exist.\n")
    }

    if (dbExistsTable(con, "foo1")) {
        cat("Pass - foo1 Table exists.\n")
    }
    else {
        cat("FAIL - foo1 Table does not exist.\n")
    }

    if (dbExistsTable(con, "Foo2")) {
        cat("Pass - Foo2 Table exists.\n")
    }
    else {
        cat("FAIL - Foo2 Table does not exist.\n")
    }

    if (dbExistsTable(con, "foo2")) {
        cat("FAIL - foo2 Table exists.\n")
    }
    else {
        cat("Pass - foo2 Table does not exist.\n")
    }

    if (dbExistsTable(con, "\"Foo2\"")) {
        cat("FAIL - \"Foo2\" Table exists.\n")
    }
    else {
        cat("Pass - \"Foo2\" Table does not exist.\n")
    }

    if (dbExistsTable(con, "\"foo2\"")) {
        cat("FAIL - \"foo2\" Table exists.\n")
    }
    else {
        cat("Pass - \"foo2\" Table does not exist.\n")
    }

    res <- dbGetQuery(con, "drop table Foo1")
    res <- dbGetQuery(con, "drop table \"Foo2\"")
    ## and disconnect
    dbDisconnect(con)
}else{
    cat("Skip.\n")
}
